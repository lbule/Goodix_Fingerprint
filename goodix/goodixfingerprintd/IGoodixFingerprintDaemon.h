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

#ifndef IGOODIX_FINGERPRINT_DAEMON_H_
#define IGOODIX_FINGERPRINT_DAEMON_H_

#include <binder/IInterface.h>
#include <binder/Parcel.h>

#include "gf_fingerprint.h"

namespace android {

class IGoodixFingerprintDaemonCallback;

/*
* Abstract base class for native implementation of FingerprintService.
*
* Note: This must be kept manually in sync with IGoodixFingerprintDaemon.aidl
*/
class IGoodixFingerprintDaemon : public IInterface, public IBinder::DeathRecipient {
    public:
        enum {
            AUTHENTICATE = IBinder::FIRST_CALL_TRANSACTION + 0,
            CANCEL_AUTHENTICATION = IBinder::FIRST_CALL_TRANSACTION + 1,
            ENROLL = IBinder::FIRST_CALL_TRANSACTION + 2,
            CANCEL_ENROLLMENT = IBinder::FIRST_CALL_TRANSACTION + 3,
            PRE_ENROLL = IBinder::FIRST_CALL_TRANSACTION + 4,
            REMOVE = IBinder::FIRST_CALL_TRANSACTION + 5,
            GET_AUTHENTICATOR_ID = IBinder::FIRST_CALL_TRANSACTION + 6,
            SET_ACTIVE_GROUP = IBinder::FIRST_CALL_TRANSACTION + 7,
            OPEN_HAL = IBinder::FIRST_CALL_TRANSACTION + 8,
            CLOSE_HAL = IBinder::FIRST_CALL_TRANSACTION + 9,
            INIT = IBinder::FIRST_CALL_TRANSACTION + 10,
            POST_ENROLL = IBinder::FIRST_CALL_TRANSACTION + 11,
            SET_SAFE_CLASS = IBinder::FIRST_CALL_TRANSACTION + 12,
            NAVIGATE = IBinder::FIRST_CALL_TRANSACTION +13,
            CANCEL_NAVIGATION = IBinder::FIRST_CALL_TRANSACTION +14,
            ENABLE_FINGERPRINT_MODULE = IBinder::FIRST_CALL_TRANSACTION + 15,
            CAMERA_CAPTURE = IBinder::FIRST_CALL_TRANSACTION +16,
            CANCEL_CAMERA_CAPTURE = IBinder::FIRST_CALL_TRANSACTION +17,
            ENABLE_FF_FEATURE = IBinder::FIRST_CALL_TRANSACTION +18,
            TEST_INIT = IBinder::FIRST_CALL_TRANSACTION + 19,
            TEST_CMD = IBinder::FIRST_CALL_TRANSACTION + 20,
            SCREEN_ON = IBinder::FIRST_CALL_TRANSACTION + 21,
            SCREEN_OFF = IBinder::FIRST_CALL_TRANSACTION +22,
            START_HBD = IBinder::FIRST_CALL_TRANSACTION + 23,
        };

        // Binder interface methods
        virtual void init(const sp<IGoodixFingerprintDaemonCallback>& callback) = 0;
        virtual int32_t enroll(const uint8_t* token, ssize_t tokenLength, int32_t groupId,
                int32_t timeout) = 0;
        virtual uint64_t preEnroll() = 0;
        virtual int32_t postEnroll() = 0;
        virtual int32_t stopEnrollment() = 0;
        virtual int32_t authenticate(uint64_t sessionId, uint32_t groupId) = 0;
        virtual int32_t stopAuthentication() = 0;
        virtual int32_t remove(int32_t fingerId, int32_t groupId) = 0;
        virtual uint64_t getAuthenticatorId() = 0;
        virtual int32_t setActiveGroup(int32_t groupId, const uint8_t* path, ssize_t pathLen) = 0;
        virtual int32_t setSafeClass(uint32_t safeClass) = 0;
        virtual int32_t navigate(uint32_t navMode) = 0;
        virtual int32_t stopNavigation() = 0;
        virtual int32_t enableFingerprintModule(uint8_t enableFlag) = 0;
        virtual int32_t cameraCapture() = 0;
        virtual int32_t stopCameraCapture() = 0;
        virtual int32_t enableFfFeature(uint8_t enableFlag) = 0;
        virtual void testInit(const sp<IGoodixFingerprintDaemonCallback>& callback) = 0;
        virtual int32_t testCmd(uint32_t cmdId, const uint8_t* param, uint32_t paramLen) = 0;
        virtual int32_t screenOn() = 0;
        virtual int32_t screenOff() = 0;
        virtual int32_t startHbd() = 0;

        virtual int64_t openHal() = 0;
        virtual int32_t closeHal() = 0;

        DECLARE_META_INTERFACE(GoodixFingerprintDaemon);

        static void hal_notify_callback(const gf_fingerprint_msg_t *msg);
};

// ----------------------------------------------------------------------------

class BnGoodixFingerprintDaemon: public BnInterface<IGoodixFingerprintDaemon> {
    public:
       virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply,
               uint32_t flags = 0);
    private:
       bool checkPermission(const String16& permission);
};

} // namespace android

#endif // IGOODIX_FINGERPRINT_DAEMON_H_

