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

#ifndef IGOODIX_FINGERPRINT_DAEMON_CALLBACK_H_
#define IGOODIX_FINGERPRINT_DAEMON_CALLBACK_H_

#include <inttypes.h>
#include <utils/Errors.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>

namespace android {

/*
* Communication channel back to FingerprintService.java
*/
class IGoodixFingerprintDaemonCallback : public IInterface {
    public:
        // must be kept in sync with IFingerprintService.aidl
        enum {
            ON_ENROLL_RESULT = IBinder::FIRST_CALL_TRANSACTION + 0,
            ON_ACQUIRED = IBinder::FIRST_CALL_TRANSACTION + 1,
            ON_AUTHENTICATED = IBinder::FIRST_CALL_TRANSACTION + 2,
            ON_ERROR = IBinder::FIRST_CALL_TRANSACTION + 3,
            ON_REMOVED = IBinder::FIRST_CALL_TRANSACTION + 4,
            ON_ENUMERATE = IBinder::FIRST_CALL_TRANSACTION + 5,
            ON_TEST_CMD = IBinder::FIRST_CALL_TRANSACTION + 6,
            ON_HBD = IBinder::FIRST_CALL_TRANSACTION + 7,
        };

        virtual status_t onEnrollResult(int64_t devId, int32_t fpId, int32_t gpId, int32_t rem) = 0;
        virtual status_t onAcquired(int64_t devId, int32_t acquiredInfo) = 0;
        virtual status_t onAuthenticated(int64_t devId, int32_t fingerId, int32_t groupId,
                const uint8_t* hat, int32_t hatSize) = 0;
        virtual status_t onError(int64_t devId, int32_t error) = 0;
        virtual status_t onRemoved(int64_t devId, int32_t fingerId, int32_t groupId) = 0;
        virtual status_t onEnumerate(int64_t devId, const int32_t* fpIds, const int32_t* gpIds,
                int32_t sz) = 0;
        virtual status_t onTestCmd(int64_t devId, int32_t cmdId, const int8_t *result, int32_t resultLen) = 0;
        virtual status_t onHbdData(int64_t devId, int32_t heartBeatRate, int32_t status, const int32_t* displayData, int32_t dataLen) = 0;

        DECLARE_META_INTERFACE(GoodixFingerprintDaemonCallback);
};

class BnGoodixFingerprintDaemonCallback: public BnInterface<IGoodixFingerprintDaemonCallback> {
    public:
        virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply,
                uint32_t flags = 0);
};

}; // namespace android

#endif // IGOODIX_FINGERPRINT_DAEMON_CALLBACK_H_
