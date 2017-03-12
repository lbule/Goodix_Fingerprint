
#define LOG_TAG "GoodixFingerprintDaemonCallbackProxy"

#include <stdlib.h>
#include <utils/String16.h>
#include <utils/Log.h>
#include <hardware/hardware.h>
#include <hardware/hw_auth_token.h>
#include <hardware/fingerprint.h>
#include "IGoodixFingerprintDaemonCallback.h"
#include "GoodixFingerprintDaemonCallbackProxy.h"

using namespace android;

fingerprint_device_t* GoodixFingerprintDaemonCallbackProxy::mDevice = NULL;

GoodixFingerprintDaemonCallbackProxy::GoodixFingerprintDaemonCallbackProxy() {
}
GoodixFingerprintDaemonCallbackProxy::~GoodixFingerprintDaemonCallbackProxy() {
}

status_t GoodixFingerprintDaemonCallbackProxy::onEnrollResult(int64_t devId, int32_t fpId, int32_t  gpId,
        int32_t rem) {
    ALOGD("onEnrollResult devId = %ld, fpId = %u, gpId = %u, rem = %u", devId, fpId, gpId, rem);
    fingerprint_msg_t message;
    message.type = FINGERPRINT_TEMPLATE_ENROLLING;
    message.data.enroll.finger.fid = fpId;
    message.data.enroll.finger.gid = gpId;
    message.data.enroll.samples_remaining = rem;

    if(mDevice != NULL) {
        mDevice->notify(&message);
    } else {
        ALOGD("onEnrollResult mDevice is NULL");
    }

    return 0;
}

status_t GoodixFingerprintDaemonCallbackProxy::onAcquired(int64_t  devId, int32_t  acquiredInfo) {
    ALOGD("onAcquired devId = %ld, acquiredInfo = %u", devId, acquiredInfo);
    fingerprint_msg_t message;
    message.type = FINGERPRINT_ACQUIRED;
    message.data.acquired.acquired_info = (fingerprint_acquired_info_t)acquiredInfo;

    if(mDevice != NULL) {
        mDevice->notify(&message);
    } else {
        ALOGD("onAcquired mDevice is NULL");
    }

    return 0;
}

status_t GoodixFingerprintDaemonCallbackProxy::onAuthenticated(int64_t  devId, int32_t  fingerId,
        int32_t groupId, const uint8_t* hat, int32_t hatSize) {
    ALOGD("onAuthenticated devId = %ld, fpId = %u, gpId = %u", devId, fingerId, groupId);
    fingerprint_msg_t message;
    message.type = FINGERPRINT_AUTHENTICATED;
    message.data.authenticated.finger.fid = fingerId;
    message.data.authenticated.finger.gid = groupId;
    memcpy(&(message.data.authenticated.hat), hat, hatSize);

    if(mDevice != NULL) {
        mDevice->notify(&message);
    } else {
        ALOGD("onAuthenticated mDevice is NULL");
    }

    return 0;
}

status_t GoodixFingerprintDaemonCallbackProxy::onError(int64_t  devId, int32_t  error) {
    ALOGD("onError devId = %ld, error = %u", devId, error);
    fingerprint_msg_t message;
    message.type = FINGERPRINT_ERROR;
    message.data.error = (fingerprint_error_t)error;

    if(mDevice != NULL) {
        mDevice->notify(&message);
    } else {
        ALOGD("onError mDevice is NULL");
    }

    return 0;
}

status_t GoodixFingerprintDaemonCallbackProxy::onRemoved(int64_t  devId,
        int32_t  fingerId, int32_t  groupId) {
    ALOGD("onRemoved devId = %ld, fpId = %u, gpId = %u", devId, fingerId, groupId);
    fingerprint_msg_t message;
    message.type = FINGERPRINT_TEMPLATE_REMOVED;
    message.data.removed.finger.fid = fingerId;
    message.data.removed.finger.gid = groupId;

    if(mDevice != NULL) {
        mDevice->notify(&message);
    } else {
        ALOGD("onRemoved mDevice is NULL");
    }

    return 0;
}

status_t GoodixFingerprintDaemonCallbackProxy::onEnumerate(int64_t  devId,
        const int32_t  *fpIds, const int32_t  *gpIds, int32_t  sz) {
    ALOGD("onEnumerate devId = %ld, fpId = %u, gpId = %u, sz = %u", devId, fpIds[0], gpIds[0], sz);
    return 0;
}

status_t GoodixFingerprintDaemonCallbackProxy::onTestCmd(int64_t  devId,
            int32_t  cmdId, const int8_t  *result, int32_t  resultLen) {
    ALOGD("onTestCmddevId = %ld, cmdId = %u, result[0] = %u, resultLen = %u", devId, cmdId, result[0], resultLen);
    return 0;
}

status_t GoodixFingerprintDaemonCallbackProxy::onHbdData(int64_t devId, int32_t heartBeatRate,
        int32_t status, const int32_t* displayData, int32_t dataLen) {
    ALOGD("onHbdData devId = %ld, heartBeatRate = %u, status = %u, displayData = %p , dataLen = %u.",
            devId, heartBeatRate, status,displayData,dataLen);
    return 0;
}
