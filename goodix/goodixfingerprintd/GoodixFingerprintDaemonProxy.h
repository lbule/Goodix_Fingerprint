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

#ifndef GOODIX_FINGERPRINT_DAEMON_PROXY_H_
#define GOODIX_FINGERPRINT_DAEMON_PROXY_H_

#include "IGoodixFingerprintDaemon.h"
#include "IGoodixFingerprintDaemonCallback.h"

namespace android {

class GoodixFingerprintDaemonProxy : public BnGoodixFingerprintDaemon {
    public:
        static GoodixFingerprintDaemonProxy* getInstance() {
            if (sInstance == NULL) {
                sInstance = new GoodixFingerprintDaemonProxy();
            }
            return sInstance;
        }

        // These reflect binder methods.
        virtual void init(const sp<IGoodixFingerprintDaemonCallback>& callback);
        virtual int32_t enroll(const uint8_t* token, ssize_t tokenLength, int32_t groupId, int32_t timeout);
        virtual uint64_t preEnroll();
        virtual int32_t postEnroll();
        virtual int32_t stopEnrollment();
        virtual int32_t authenticate(uint64_t sessionId, uint32_t groupId);
        virtual int32_t stopAuthentication();
        virtual int32_t remove(int32_t fingerId, int32_t groupId);
        virtual uint64_t getAuthenticatorId();
        virtual int32_t setActiveGroup(int32_t groupId, const uint8_t* path, ssize_t pathLen);
        virtual int32_t setSafeClass(uint32_t safeClass);
        virtual int32_t navigate(uint32_t navMode);
        virtual int32_t stopNavigation();
        virtual int32_t enableFingerprintModule(uint8_t enableFlag);
        virtual int32_t cameraCapture();
        virtual int32_t stopCameraCapture();
        virtual int32_t enableFfFeature(uint8_t enableFlag);
        virtual void testInit(const sp<IGoodixFingerprintDaemonCallback>& callback);
        virtual int32_t testCmd(uint32_t cmdId, const uint8_t* param, uint32_t paramLen);
        virtual int32_t screenOn();
        virtual int32_t screenOff();
        virtual int32_t startHbd();

        virtual int64_t openHal();
        virtual int32_t closeHal();

    private:
        GoodixFingerprintDaemonProxy();
        virtual ~GoodixFingerprintDaemonProxy();
        void binderDied(const wp<IBinder>& who);
        static void hal_notify_callback(const gf_fingerprint_msg_t *msg);

        static GoodixFingerprintDaemonProxy* sInstance;
        gf_fingerprint_device_t* mDevice;
        sp<IGoodixFingerprintDaemonCallback> mCallback;
        sp<IGoodixFingerprintDaemonCallback> mTestCallback;
};

} // namespace android

#endif // GOODIX_FINGERPRINT_DAEMON_PROXY_H_
