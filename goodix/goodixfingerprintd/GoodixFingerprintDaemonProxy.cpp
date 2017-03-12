/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "goodixfingerprintd"

#include <binder/IServiceManager.h>
#include <hardware/hardware.h>
#include <hardware/hw_auth_token.h>
#include <utils/Log.h>

#include "GoodixFingerprintDaemonProxy.h"
#include "gf_hal.h"

namespace android {

GoodixFingerprintDaemonProxy* GoodixFingerprintDaemonProxy::sInstance = NULL;

GoodixFingerprintDaemonProxy::GoodixFingerprintDaemonProxy() : mDevice(NULL), mCallback(NULL), mTestCallback(NULL) {

}

GoodixFingerprintDaemonProxy::~GoodixFingerprintDaemonProxy() {
    closeHal();
}

void GoodixFingerprintDaemonProxy::hal_notify_callback(const gf_fingerprint_msg_t *msg) {
    GoodixFingerprintDaemonProxy* instance = GoodixFingerprintDaemonProxy::getInstance();
    const sp<IGoodixFingerprintDaemonCallback> callback = instance->mCallback;
    const sp<IGoodixFingerprintDaemonCallback> testCallback = instance->mTestCallback;
    if (callback == NULL) {
        ALOGE("Invalid callback object");
        return;
    }
    const int64_t device = (int64_t) instance->mDevice;
    switch (msg->type) {
        case GF_FINGERPRINT_ERROR:
            ALOGD("onError(%d)", msg->data.error);
            callback->onError(device, msg->data.error);
            break;
        case GF_FINGERPRINT_ACQUIRED:
            ALOGD("onAcquired(%d)", msg->data.acquired.acquired_info);
            callback->onAcquired(device, msg->data.acquired.acquired_info);
            break;
        case GF_FINGERPRINT_AUTHENTICATED:
            ALOGD("onAuthenticated(fid=%u, gid=%u)",
                    msg->data.authenticated.finger.fid,
                    msg->data.authenticated.finger.gid);
            callback->onAuthenticated(device,
                    msg->data.authenticated.finger.fid,
                    msg->data.authenticated.finger.gid,
                    reinterpret_cast<const uint8_t *>(&msg->data.authenticated.hat),
                    sizeof(hw_auth_token_t));
            break;
        case GF_FINGERPRINT_TEMPLATE_ENROLLING:
            ALOGD("onEnrollResult(fid=%u, gid=%u, rem=%u)",
                    msg->data.enroll.finger.fid,
                    msg->data.enroll.finger.gid,
                    msg->data.enroll.samples_remaining);
            callback->onEnrollResult(device,
                    msg->data.enroll.finger.fid,
                    msg->data.enroll.finger.gid,
                    msg->data.enroll.samples_remaining);
            break;
        case GF_FINGERPRINT_TEMPLATE_REMOVED:
            ALOGD("onRemove(fid=%u, gid=%u)",
                    msg->data.removed.finger.fid,
                    msg->data.removed.finger.gid);
            callback->onRemoved(device,
                    msg->data.removed.finger.fid,
                    msg->data.removed.finger.gid);
            break;
        case GF_FINGERPRINT_TEST_CMD:
            ALOGD("onTestCmd(cmd_id = %u, result = %s, result_len = %u)",
                    msg->data.test.cmd_id, msg->data.test.result, msg->data.test.result_len);
            if(testCallback == NULL) {
                ALOGE("Invalid testCallback object");
                return;
            }
            testCallback->onTestCmd(device, msg->data.test.cmd_id,
                    (const int8_t *)(msg->data.test.result),
                    msg->data.test.result_len);
            break;
        case GF_FINGERPRINT_HBD:
            ALOGD("onHBD.");
            if(testCallback == NULL) {
                ALOGE("Invalid testCallback object");
                return;
            }
            testCallback->onHbdData(device, msg->data.hbd.heart_beat_rate, msg->data.hbd.status,
                    msg->data.hbd.display_data, msg->data.hbd.data_len);
            break;
        default:
            ALOGE("invalid msg type: %u", msg->type);
            return;
    }
}

void GoodixFingerprintDaemonProxy::init(const sp<IGoodixFingerprintDaemonCallback>& callback) {
    if (mCallback != NULL && IInterface::asBinder(callback) != IInterface::asBinder(mCallback)) {
        IInterface::asBinder(mCallback)->unlinkToDeath(this);
    }
    IInterface::asBinder(callback)->linkToDeath(this);
    mCallback = callback;
}

int32_t GoodixFingerprintDaemonProxy::enroll(const uint8_t* token, ssize_t tokenSize, int32_t groupId,
        int32_t timeout) {
    ALOG(LOG_VERBOSE, LOG_TAG, "enroll(gid=%d, timeout=%d)\n", groupId, timeout);
    if (tokenSize != sizeof(hw_auth_token_t) ) {
        ALOG(LOG_VERBOSE, LOG_TAG, "enroll() : invalid token size %zu\n", tokenSize);
        return -1;
    }
    const hw_auth_token_t* authToken = reinterpret_cast<const hw_auth_token_t*>(token);
    return gf_hal_enroll(mDevice, authToken, groupId, timeout);
}

uint64_t GoodixFingerprintDaemonProxy::preEnroll() {
    return gf_hal_pre_enroll(mDevice);
}

int32_t GoodixFingerprintDaemonProxy::postEnroll() {
    return gf_hal_post_enroll(mDevice);
}

int32_t GoodixFingerprintDaemonProxy::stopEnrollment() {
    ALOG(LOG_VERBOSE, LOG_TAG, "stopEnrollment()\n");
    return gf_hal_cancel(mDevice);
}

int32_t GoodixFingerprintDaemonProxy::authenticate(uint64_t sessionId, uint32_t groupId) {
    ALOG(LOG_VERBOSE, LOG_TAG, "authenticate(sid=%" PRId64 ", gid=%d)\n", sessionId, groupId);
    return gf_hal_authenticate(mDevice, sessionId, groupId);
}

int32_t GoodixFingerprintDaemonProxy::stopAuthentication() {
    ALOG(LOG_VERBOSE, LOG_TAG, "stopAuthentication()\n");
    return gf_hal_cancel(mDevice);
}

int32_t GoodixFingerprintDaemonProxy::remove(int32_t fingerId, int32_t groupId) {
    ALOG(LOG_VERBOSE, LOG_TAG, "remove(fid=%d, gid=%d)\n", fingerId, groupId);
    return gf_hal_remove(mDevice, groupId, fingerId);
}

uint64_t GoodixFingerprintDaemonProxy::getAuthenticatorId() {
    return gf_hal_get_auth_id(mDevice);
}

int32_t GoodixFingerprintDaemonProxy::setActiveGroup(int32_t groupId, const uint8_t* path,
        ssize_t pathlen) {
    if (pathlen >= PATH_MAX || pathlen <= 0) {
        ALOGE("Bad path length: %zd", pathlen);
        return -1;
    }
    // Convert to null-terminated string
    char path_name[PATH_MAX];
    memcpy(path_name, path, pathlen);
    path_name[pathlen] = '\0';
    ALOG(LOG_VERBOSE, LOG_TAG, "setActiveGroup(%d, %s, %zu)", groupId, path_name, pathlen);
    return gf_hal_set_active_group(mDevice, groupId);
}

int32_t GoodixFingerprintDaemonProxy::setSafeClass(uint32_t safeClass) {
    return gf_hal_set_safe_class(mDevice, (gf_safe_class_t)safeClass);
}

int32_t GoodixFingerprintDaemonProxy::navigate(uint32_t navMode) {
    return gf_hal_navigate(mDevice, (gf_nav_mode_t)navMode);
}

int32_t GoodixFingerprintDaemonProxy::stopNavigation() {
    return gf_hal_cancel(mDevice);
}

int32_t GoodixFingerprintDaemonProxy::enableFingerprintModule(uint8_t enableFlag) {
    return gf_hal_enable_fingerprint_module(mDevice, enableFlag);
}

int32_t GoodixFingerprintDaemonProxy::cameraCapture() {
    return gf_hal_camera_capture(mDevice);
}

int32_t GoodixFingerprintDaemonProxy::stopCameraCapture() {
    return gf_hal_cancel(mDevice);
}

int32_t GoodixFingerprintDaemonProxy::enableFfFeature(uint8_t enableFlag) {
    return gf_hal_enable_ff_feature(mDevice, enableFlag);
}

void GoodixFingerprintDaemonProxy::testInit(const sp<IGoodixFingerprintDaemonCallback>& callback) {
    if (mTestCallback != NULL && IInterface::asBinder(callback) != IInterface::asBinder(mTestCallback)) {
        IInterface::asBinder(mTestCallback)->unlinkToDeath(this);
    }
    IInterface::asBinder(callback)->linkToDeath(this);
    mTestCallback = callback;
}

int32_t GoodixFingerprintDaemonProxy::testCmd(uint32_t cmdId, const uint8_t* param, uint32_t paramLen) {
    return gf_hal_test_cmd(mDevice, cmdId, param, paramLen);
}

int32_t GoodixFingerprintDaemonProxy::screenOn() {
    return gf_hal_screen_on();
}

int32_t GoodixFingerprintDaemonProxy::screenOff() {
    return gf_hal_screen_off();
}

int32_t GoodixFingerprintDaemonProxy::startHbd(){
    return gf_hal_start_hbd(mDevice);
}

int64_t GoodixFingerprintDaemonProxy::openHal() {
    ALOG(LOG_VERBOSE, LOG_TAG, "nativeOpenHal()\n");
    mDevice = (gf_fingerprint_device_t *)malloc(sizeof(gf_fingerprint_device_t));
    memset(mDevice, 0, sizeof(gf_fingerprint_device_t));
    mDevice->notify = hal_notify_callback;
    gf_error_t err = gf_hal_init(mDevice);
    if (GF_SUCCESS != err) {
        return -EINVAL;
    }

    return 0;
}

int32_t GoodixFingerprintDaemonProxy::closeHal() {
    ALOG(LOG_VERBOSE, LOG_TAG, "nativeCloseHal()\n");
    gf_hal_exit(mDevice);

    if(mDevice != NULL) {
        free(mDevice);
        mDevice = NULL;
    }

    return 0;
}

void GoodixFingerprintDaemonProxy::binderDied(const wp<IBinder>& who) {
    ALOGD("binder died");
    if (IInterface::asBinder(mTestCallback) == who) {
        mTestCallback = NULL;
        return;
    }

    int err;
    if (0 != (err = closeHal())) {
        ALOGE("Can't close fingerprint device, error: %d", err);
    }
    if (IInterface::asBinder(mCallback) == who) {
        mCallback = NULL;
    }
}

}
