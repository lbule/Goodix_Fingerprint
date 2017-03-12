#ifndef GOODIX_FINGERPRINT_DAEMON_CALLBACK_PROXY_H_
#define GOODIX_FINGERPRINT_DAEMON_CALLBACK_PROXY_H_

#include <hardware/fingerprint.h>
#include "IGoodixFingerprintDaemonCallback.h"

namespace android {

class GoodixFingerprintDaemonCallbackProxy: public BnGoodixFingerprintDaemonCallback {
public:
    static void setDevice(fingerprint_device_t *device) {
        mDevice = device;
    }

    virtual status_t onEnrollResult(int64_t devId, int32_t fpId, int32_t gpId, int32_t rem);
    virtual status_t onAcquired(int64_t devId, int32_t acquiredInfo);
    virtual status_t onAuthenticated(int64_t devId, int32_t fingerId, int32_t groupId, const uint8_t* hat, int32_t hatSize);
    virtual status_t onError(int64_t devId, int32_t error);
    virtual status_t onRemoved(int64_t devId, int32_t fingerId, int32_t groupId);
    virtual status_t onEnumerate(int64_t devId, const int32_t* fpIds, const int32_t* gpIds,
            int32_t sz);
    virtual status_t onTestCmd(int64_t devId, int32_t cmdId, const int8_t *result, int32_t resultLen);
    virtual status_t onHbdData(int64_t devId, int32_t heartBeatRate, int32_t status, const int32_t* displayData, int32_t resultLen);
    GoodixFingerprintDaemonCallbackProxy();
    virtual ~GoodixFingerprintDaemonCallbackProxy();
private:
    static fingerprint_device_t *mDevice;
};

}

#endif // GOODIX_FINGERPRINT_DAEMON_CALLBACK_PROXY_H_
