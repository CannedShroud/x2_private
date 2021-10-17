/*
 * Copyright (C) 2021 The LineageOS Project
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
#define LOG_TAG "android.hardware.biometrics.fingerprint@2.3-service.raphael"

#include "BiometricsFingerprint.h"

#include <android-base/logging.h>
#include <fstream>
#include <thread>

#define FP_PRESS_PATH "/sys/kernel/oppo_display/notify_fppress"
#define NOTIFY_BLANK_PATH "/sys/kernel/oppo_display/notify_panel_blank"
#define AOD_MODE_PATH "/sys/kernel/oppo_display/aod_light_mode_set"
#define DOZE_STATUS "/proc/touchpanel/DOZE_STATUS"
#define FP_BEGIN 1
#define FP_ENDIT 0

namespace android {
namespace hardware {
namespace biometrics {
namespace fingerprint {
namespace V2_3 {
namespace implementation {

template <typename T>
static inline void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value;
}

BiometricsFingerprint::BiometricsFingerprint() {
    biometrics_2_1_service = IBiometricsFingerprint_2_1::getService();
}

Return<uint64_t> BiometricsFingerprint::setNotify(const sp<IBiometricsFingerprintClientCallback>& clientCallback) {
    return biometrics_2_1_service->setNotify(clientCallback);
}

Return<uint64_t> BiometricsFingerprint::preEnroll() {
    return biometrics_2_1_service->preEnroll();
}

Return<RequestStatus> BiometricsFingerprint::enroll(const hidl_array<uint8_t, 69>& hat, uint32_t gid, uint32_t timeoutSec) {
    return biometrics_2_1_service->enroll(hat, gid, timeoutSec);
}

Return<RequestStatus> BiometricsFingerprint::postEnroll() {
    return biometrics_2_1_service->postEnroll();
}

Return<uint64_t> BiometricsFingerprint::getAuthenticatorId() {
    return biometrics_2_1_service->getAuthenticatorId();
}

Return<RequestStatus> BiometricsFingerprint::cancel() {
    return biometrics_2_1_service->cancel();
}

Return<RequestStatus> BiometricsFingerprint::enumerate() {
    return biometrics_2_1_service->enumerate();
}

Return<RequestStatus> BiometricsFingerprint::remove(uint32_t gid, uint32_t fid) {
    return biometrics_2_1_service->remove(gid, fid);
}

Return<RequestStatus> BiometricsFingerprint::setActiveGroup(uint32_t gid, const hidl_string& storePath) {
    return biometrics_2_1_service->setActiveGroup(gid, storePath);
}

Return<RequestStatus> BiometricsFingerprint::authenticate(uint64_t operationId, uint32_t gid) {
    return biometrics_2_1_service->authenticate(operationId, gid);
}

Return<bool> BiometricsFingerprint::isUdfps(uint32_t) {
    return true;
}

Return<void> BiometricsFingerprint::onFingerDown(uint32_t, uint32_t, float, float) {
    mFingerPressed = true;
    set(DIMLAYER_PATH, FP_BEGIN); 
    std::thread([this]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(39));
        if (mFingerPressed) {
            set(FP_PRESS_PATH, FP_BEGIN);
        }
    }).detach();
    return Void();
}

Return<void> BiometricsFingerprint::onFingerUp() {
    mFingerPressed = false;
    set(DIMLAYER_PATH, FP_ENDIT);
    set(FP_PRESS_PATH, FP_ENDIT);
    return Void();
}

Return<int32_t> FingerprintInscreen::getDimAmount(int32_t brightness) {
    return(int32_t)(255 + ((-12.08071) * pow((double)brightness, 0.4)));
}
  // namespace implementation
}  // namespace V2_3
}  // namespace fingerprint
}  // namespace biometrics
}  // namespace hardware
}