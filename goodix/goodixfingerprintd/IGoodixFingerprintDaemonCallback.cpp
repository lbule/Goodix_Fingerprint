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

#define LOG_TAG "IGoodixFingerprintDaemonCallback"
#include <stdint.h>
#include <sys/types.h>
#include <utils/Log.h>
#include <binder/Parcel.h>

#include "IGoodixFingerprintDaemonCallback.h"

namespace android {

status_t BnGoodixFingerprintDaemonCallback::onTransact(uint32_t code, const Parcel& data, Parcel* reply,
        uint32_t flags) {
    switch (code) {
    case ON_ENROLL_RESULT: {
        CHECK_INTERFACE(IGoodixFingerprintDaemonCallback, data, reply);
        int64_t devId = data.readInt64();
        int32_t fpId = data.readInt32();
        int32_t gpId = data.readInt32();
        int32_t rem = data.readInt32();
        onEnrollResult(devId, fpId, gpId, rem);
        return NO_ERROR;
    }
    case ON_ACQUIRED: {
        CHECK_INTERFACE(IGoodixFingerprintDaemonCallback, data, reply);
        int64_t devId = data.readInt64();
        int32_t acquiredInfo = data.readInt32();
        onAcquired(devId, acquiredInfo);
        return NO_ERROR;
    }
    case ON_AUTHENTICATED: {
        CHECK_INTERFACE(IGoodixFingerprintDaemonCallback, data, reply);
        int64_t devId = data.readInt64();
        int32_t fpId = data.readInt32();
        int32_t gpId = data.readInt32();
        int32_t hatSize = data.readInt32();
        const uint8_t* hat = static_cast<const uint8_t *>(data.readInplace(hatSize));
        onAuthenticated(devId, fpId, gpId, hat, hatSize);
        return NO_ERROR;
    }
    case ON_ERROR: {
        CHECK_INTERFACE(IGoodixFingerprintDaemonCallback, data, reply);
        int64_t devId = data.readInt64();
        int32_t error = data.readInt32();
        onError(devId, error);
        return NO_ERROR;
    }
    case ON_REMOVED: {
        CHECK_INTERFACE(IGoodixFingerprintDaemonCallback, data, reply);
        int64_t devId = data.readInt64();
        int32_t fpId = data.readInt32();
        int32_t gpId = data.readInt32();
        onRemoved(devId, fpId, gpId);
        return NO_ERROR;
    }
    case ON_ENUMERATE: {
        CHECK_INTERFACE(IGoodixFingerprintDaemonCallback, data, reply);
        int64_t devId = data.readInt64();
        int32_t fpIds_size = data.readInt32();
        int8_t *fpIds = (int8_t *) malloc(fpIds_size * sizeof(int32_t));
        memcpy(fpIds, static_cast<const uint8_t *>(data.readInplace(fpIds_size)), fpIds_size);
        int32_t gpIds_size = data.readInt32();
        int8_t *gpIds = (int8_t *) malloc(gpIds_size * sizeof(gpIds_size));
        memcpy(gpIds, static_cast<const uint8_t *>(data.readInplace(gpIds_size)), gpIds_size);
        onEnumerate(devId, (const int32_t*) fpIds, (const int32_t*) gpIds,
                gpIds_size);
        return NO_ERROR;
    }
    case ON_TEST_CMD: {
        CHECK_INTERFACE(IGoodixFingerprintDaemonCallback, data, reply);
        int64_t devId = data.readInt64();
        int32_t cmdId = data.readInt32();
        int32_t resultLen = data.readInt32();
        const int8_t* result = static_cast<const int8_t *>(data.readInplace(resultLen));
        onTestCmd(devId, cmdId, result, resultLen);
        return NO_ERROR;
    }
    case ON_HBD: {
        CHECK_INTERFACE(IGoodixFingerprintDaemonCallback, data, reply);
        int64_t devId = data.readInt64();
        int32_t heartBeatRate = data.readInt32();
        int32_t status = data.readInt32();
        int32_t dataLen =data.readInt32();
        const int32_t* displayData = static_cast<const int32_t *>(data.readInplace(dataLen));
        onHbdData(devId, heartBeatRate, status, displayData, dataLen);
        return NO_ERROR;
    }

    default:
        return BBinder::onTransact(code, data, reply, flags);
    }
}

class BpGoodixFingerprintDaemonCallback : public BpInterface<IGoodixFingerprintDaemonCallback>
{
public:
    BpGoodixFingerprintDaemonCallback(const sp<IBinder>& impl) :
            BpInterface<IGoodixFingerprintDaemonCallback>(impl) {
    }
    virtual status_t onEnrollResult(int64_t devId, int32_t fpId, int32_t gpId, int32_t rem) {
        Parcel data, reply;
        data.writeInterfaceToken(IGoodixFingerprintDaemonCallback::getInterfaceDescriptor());
        data.writeInt64(devId);
        data.writeInt32(fpId);
        data.writeInt32(gpId);
        data.writeInt32(rem);
        return remote()->transact(ON_ENROLL_RESULT, data, &reply, IBinder::FLAG_ONEWAY);
    }

    virtual status_t onAcquired(int64_t devId, int32_t acquiredInfo) {
        Parcel data, reply;
        data.writeInterfaceToken(IGoodixFingerprintDaemonCallback::getInterfaceDescriptor());
        data.writeInt64(devId);
        data.writeInt32(acquiredInfo);
        return remote()->transact(ON_ACQUIRED, data, &reply, IBinder::FLAG_ONEWAY);
    }

    virtual status_t onAuthenticated(int64_t devId, int32_t fpId, int32_t gpId,
            const uint8_t* hat, int32_t hatSize) {
        Parcel data, reply;
        data.writeInterfaceToken(IGoodixFingerprintDaemonCallback::getInterfaceDescriptor());
        data.writeInt64(devId);
        data.writeInt32(fpId);
        data.writeInt32(gpId);
        data.writeInt32(hatSize);
        data.write(hat, hatSize);
        return remote()->transact(ON_AUTHENTICATED, data, &reply, IBinder::FLAG_ONEWAY);
    }

    virtual status_t onError(int64_t devId, int32_t error) {
        Parcel data, reply;
        data.writeInterfaceToken(IGoodixFingerprintDaemonCallback::getInterfaceDescriptor());
        data.writeInt64(devId);
        data.writeInt32(error);
        return remote()->transact(ON_ERROR, data, &reply, IBinder::FLAG_ONEWAY);
    }

    virtual status_t onRemoved(int64_t devId, int32_t fpId, int32_t gpId) {
        Parcel data, reply;
        data.writeInterfaceToken(IGoodixFingerprintDaemonCallback::getInterfaceDescriptor());
        data.writeInt64(devId);
        data.writeInt32(fpId);
        data.writeInt32(gpId);
        return remote()->transact(ON_REMOVED, data, &reply, IBinder::FLAG_ONEWAY);
    }

    virtual status_t onEnumerate(int64_t devId, const int32_t* fpIds, const int32_t* gpIds,
            int32_t sz) {
        Parcel data, reply;
        data.writeInterfaceToken(IGoodixFingerprintDaemonCallback::getInterfaceDescriptor());
        data.writeInt64(devId);
        data.writeInt32Array(sz, fpIds);
        data.writeInt32Array(sz, gpIds);
        return remote()->transact(ON_ENUMERATE, data, &reply, IBinder::FLAG_ONEWAY);
    }

    virtual status_t onTestCmd(int64_t devId, int32_t cmdId, const int8_t *result, int32_t resultLen) {
        Parcel data, reply;
        data.writeInterfaceToken(IGoodixFingerprintDaemonCallback::getInterfaceDescriptor());
        data.writeInt64(devId);
        data.writeInt32(cmdId);
        data.writeInt32(resultLen);
        data.write(result, resultLen);
        return remote()->transact(ON_TEST_CMD, data, &reply, IBinder::FLAG_ONEWAY);
    }

    virtual status_t onHbdData(int64_t devId, int32_t heartBeatRate, int32_t status, const int32_t* displayData, int32_t dataLen){
        Parcel data, reply;
        data.writeInterfaceToken(IGoodixFingerprintDaemonCallback::getInterfaceDescriptor());
        data.writeInt64(devId);
        data.writeInt32(heartBeatRate);
        data.writeInt32(status);
        data.writeInt32(dataLen);
        data.write(displayData, dataLen);
        return remote()->transact(ON_HBD, data, &reply, IBinder::FLAG_ONEWAY);
    }
};

IMPLEMENT_META_INTERFACE(GoodixFingerprintDaemonCallback,
        "android.hardware.fingerprint.IGoodixFingerprintDaemonCallback");

}; // namespace android
