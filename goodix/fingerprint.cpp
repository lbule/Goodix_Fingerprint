/*
 * Copyright (C) 2014 The Android Open Source Project
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
#define LOG_TAG "FingerprintHal"
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <cutils/log.h>
#include <hardware/hardware.h>
#include <hardware/fingerprint.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/IInterface.h>

#include "GoodixFingerprintDaemonProxy.h"
#include "IGoodixFingerprintDaemon.h"
#include "IGoodixFingerprintDaemonCallback.h"
#include "GoodixFingerprintDaemonCallbackProxy.h"

using namespace android;

sp<IGoodixFingerprintDaemon> g_service = NULL;

fingerprint_device_t *g_device = NULL;

sp<IGoodixFingerprintDaemon> getService();

class GoodixBinderDiednotify: public IBinder::DeathRecipient {
    public:
        void binderDied(const wp<IBinder> __unused &who) {
            ALOGV("binderDied");
            g_service = NULL;

            getService();
        }
};

static sp<GoodixBinderDiednotify> gDeathRecipient = new GoodixBinderDiednotify();

sp<IGoodixFingerprintDaemon> getService() {

    do {
        if (g_service == NULL) {
            ALOGV("getService g_servie is NULL");

            sp<IServiceManager> sm = defaultServiceManager();
            sp<IBinder> binder = sm->getService(android::GoodixFingerprintDaemonProxy::descriptor);
            if (binder == NULL) {
                ALOGE("getService failed");
                break;
            }
            g_service = interface_cast<IGoodixFingerprintDaemon>(binder);
            binder->linkToDeath(gDeathRecipient, NULL, 0);

            if (g_service != NULL) {
                ALOGV("getService succeed");
                sp<android::GoodixFingerprintDaemonCallbackProxy> callback =
                        new GoodixFingerprintDaemonCallbackProxy();
                GoodixFingerprintDaemonCallbackProxy::setDevice(g_device);
                g_service->init(callback);

                int ret = g_service->openHal();
                if (ret != 0) {
                    ALOGE("getService openHal failed!");
                    g_service = NULL;
                }
            }
        }
    } while (0);

    return g_service;
}

static int fingerprint_close(hw_device_t *dev) {
    ALOGV("fingerprint_close");
    if (dev) {
        sp<IGoodixFingerprintDaemon> service = getService();
        if (service == NULL) {
            return FINGERPRINT_ERROR;
        } else {
            g_service->closeHal();
            free(dev);
            g_service = NULL;
            return 0;
        }
    } else {
        return FINGERPRINT_ERROR;
    }
}

static uint64_t fingerprint_pre_enroll(struct fingerprint_device __unused *dev) {
    ALOGV("fingerprint_pre_enroll");
    sp<IGoodixFingerprintDaemon> service = getService();
    if(service == NULL) {
        return FINGERPRINT_ERROR;
    } else {
        return service->preEnroll();
    }
}

static int fingerprint_enroll(struct fingerprint_device __unused *dev,
                                const hw_auth_token_t *hat,
                                uint32_t gid,
                                uint32_t timeout_sec) {
    ALOGV("fingerprint_enroll");
    sp<IGoodixFingerprintDaemon> service = getService();
    if(service == NULL) {
        return FINGERPRINT_ERROR;
    } else {
        return service->enroll((const uint8_t*)hat, sizeof(hw_auth_token_t), gid, timeout_sec);
    }
}

static int fingerprint_post_enroll(struct fingerprint_device __unused *dev) {
    ALOGV("fingerprint_post_enroll");
    sp<IGoodixFingerprintDaemon> service = getService();
    if(service == NULL) {
        return FINGERPRINT_ERROR;
    } else {
        return service->postEnroll();
    }
}

static uint64_t fingerprint_get_auth_id(struct fingerprint_device __unused *dev) {
    ALOGV("fingerprint_get_auth_id");
    sp<IGoodixFingerprintDaemon> service = getService();
    if(service == NULL) {
        return FINGERPRINT_ERROR;
    } else {
        return service->getAuthenticatorId();
    }
}

static int fingerprint_cancel(struct fingerprint_device __unused *dev) {
    ALOGV("fingerprint_cancel");
    sp<IGoodixFingerprintDaemon> service = getService();
    if(service == NULL) {
        return FINGERPRINT_ERROR;
    } else {
        return service->stopEnrollment();
    }
}

static int fingerprint_remove(struct fingerprint_device __unused *dev,
                                uint32_t gid, uint32_t fid) {
    ALOGV("fingerprint_remove");
    sp<IGoodixFingerprintDaemon> service = getService();
    if(service == NULL) {
        return FINGERPRINT_ERROR;
    } else {
        return service->remove(fid, gid);
    }
}

static int fingerprint_set_active_group(struct fingerprint_device __unused *dev,
                                        uint32_t __unused gid, const char __unused *store_path) {
    ALOGV("fingerprint_set_active_group");
    uint32_t path_len = strlen(store_path);
    uint8_t path_name[PATH_MAX] = {0};
    memcpy(path_name, store_path, path_len);
    sp<IGoodixFingerprintDaemon> service = getService();
    if(service == NULL) {
        return FINGERPRINT_ERROR;
    } else {
        return service->setActiveGroup(gid, path_name, path_len);
    }
}

static int fingerprint_authenticate(struct fingerprint_device __unused *dev,
                                    uint64_t operation_id, uint32_t gid) {
    ALOGV("fingerprint_authenticate");
    sp<IGoodixFingerprintDaemon> service = getService();
    if(service == NULL) {
        return FINGERPRINT_ERROR;
    } else {
        return service->authenticate(operation_id, gid);
    }
}

static int set_notify_callback(struct fingerprint_device *dev, fingerprint_notify_t notify) {
    ALOGV("set_notify_callback");
    /* Decorate with locks */
    dev->notify = notify;
    return 0;
}

static int fingerprint_open(const hw_module_t* module, const char __unused *id,
                            hw_device_t** device)
{
    ALOGV("fingerprint_open");

    if (device == NULL) {
        ALOGE("NULL device on open");
        return -EINVAL;
    }

    fingerprint_device_t *dev = (fingerprint_device_t *)malloc(sizeof(fingerprint_device_t));
    memset(dev, 0, sizeof(fingerprint_device_t));

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = FINGERPRINT_MODULE_API_VERSION_2_0;
    dev->common.module = (struct hw_module_t*) module;
    dev->common.close = fingerprint_close;

    dev->pre_enroll = fingerprint_pre_enroll;
    dev->enroll = fingerprint_enroll;
    dev->post_enroll = fingerprint_post_enroll;
    dev->get_authenticator_id = fingerprint_get_auth_id;
    dev->cancel = fingerprint_cancel;
    dev->remove = fingerprint_remove;
    dev->set_active_group = fingerprint_set_active_group;
    dev->authenticate = fingerprint_authenticate;
    dev->set_notify = set_notify_callback;
    dev->notify = NULL;

    g_device = dev;
    if(g_device == NULL) {
        ALOGV("g_device is NULL");
    } else {
        ALOGV("g_device is not NULL");
    }

    *device = (hw_device_t*) dev;

    if(getService() == NULL) {
        return -EINVAL;
    }

    return 0;
}

static struct hw_module_methods_t fingerprint_module_methods = {
    .open = fingerprint_open,
};

fingerprint_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag                = HARDWARE_MODULE_TAG,
        .module_api_version = FINGERPRINT_MODULE_API_VERSION_2_0,
        .hal_api_version    = HARDWARE_HAL_API_VERSION,
        .id                 = FINGERPRINT_HARDWARE_MODULE_ID,
        .name               = "Goodix Fingerprint HAL",
        .author             = "Goodix",
        .methods            = &fingerprint_module_methods,
        .dso                = NULL
    },
};
