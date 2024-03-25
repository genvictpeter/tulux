/*
 *  Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * Cv2xRadio helper class - To convert periodicity to uint,
 * convert WDS errors to strings, and to initialize QMI request
 * messages.
 */

#ifndef CV2X_RADIO_HELPER_HPP
#define CV2X_RADIO_HELPER_HPP

#include <string>
#include <vector>
#include <memory>
#include <bitset>
#include <sstream>
#include <algorithm>

#include "telux/cv2x/Cv2xRadioTypes.hpp"
#include "qmi/NasQmiClient.hpp"
#include "common/CommonUtils.hpp"
#include "common/Logger.hpp"

namespace telux {
namespace cv2x {

// Helper class for Cv2xRadio
class Cv2xRadioHelper {
public:
    static uint32_t periodicityToUint32(Periodicity p);

    static Periodicity uint32ToPeriodicity(uint32_t p);

    static std::bitset<8> periodicityListToBitset(const uint32_t periodicityList[],
                                                  uint32_t periodicityListLen);

    static std::vector<uint64_t> periodicityListToVector(const uint32_t periodicityList[],
                                                  uint32_t periodicityListLen);
    template <typename T>
    static std::bitset<8> priorityListToBitset(const T priorityList[],
                                               uint32_t priorityListLen) {
        std::bitset<8> bitset;
        Priority p = Priority::PRIORITY_UNKNOWN;

        if (priorityListLen > static_cast<uint32_t>(Priority::PRIORITY_UNKNOWN)) {
            return bitset;
        }
        for (auto i = 0u; i < priorityListLen; ++i) {
            if (uint8ToPriority(priorityList[i].priority, p) && p <= Priority::PRIORITY_UNKNOWN) {
                bitset.set(common::getUnderlyingValue(p));
            }
        }
        return bitset;
    }

    static bool updateCv2xStatusFromInd(Cv2xStatusEx &status,
                                        const nas_v2x_notification_ind_v01 &ind);

    static bool updateCv2xStatusFromResp(Cv2xStatusEx &status,
                                         const nas_get_v2x_status_resp_msg_v01 &resp);

    static void resetV2xStatusEx(Cv2xStatusEx &status);

    static std::string getV2xIpApn();

    static std::string getV2xNonIpApn();

    template <typename T>
    static bool convertNasV2xStatusMsg(Cv2xStatusEx &status,
                                       const T &msg) {
        bool statusChanged = false;

        if (msg.rx_status_valid) {
            auto rxStatus = static_cast<Cv2xStatusType>(msg.rx_status);

            if (rxStatus != status.status.rxStatus) {
                statusChanged = true;
                status.status.rxStatus = rxStatus;
            }
        }

        if (msg.tx_status_valid) {
            auto txStatus = static_cast<Cv2xStatusType>(msg.tx_status);

            if (txStatus != status.status.txStatus) {
                statusChanged = true;
                status.status.txStatus = txStatus;
            }
        }

        if (msg.rx_multi_pool_status_valid) {
            for (uint8_t i = 0u; i < msg.rx_multi_pool_status_len;  ++i) {
                if (i >= status.poolStatus.size()) {
                    statusChanged = true;
                    Cv2xPoolStatus addStatus;
                    addStatus.poolId = i;
                    addStatus.status.rxStatus =
                        static_cast<Cv2xStatusType>(msg.rx_multi_pool_status[i].status);
                    addStatus.status.rxCause = convertV2xCause(msg.rx_multi_pool_status[i].cause);
                    status.poolStatus.push_back(addStatus);
                } else {
                    auto multiStatus =
                        static_cast<Cv2xStatusType>(msg.rx_multi_pool_status[i].status);
                    if (status.poolStatus[i].status.rxStatus != multiStatus) {
                        statusChanged = true;
                        status.poolStatus[i].status.rxStatus = multiStatus;
                    }
                    auto multiCause = convertV2xCause(msg.rx_multi_pool_status[i].cause);
                    if (status.poolStatus[i].status.rxCause != multiCause) {
                        statusChanged = true;
                        status.poolStatus[i].status.rxCause = multiCause;
                    }
                }
            }
        }

        if (msg.tx_multi_pool_status_valid) {
            for (uint8_t i = 0u; i < msg.tx_multi_pool_status_len;  ++i) {
                if (i >= status.poolStatus.size()) {
                    statusChanged = true;
                    Cv2xPoolStatus addStatus;
                    addStatus.poolId = i;
                    addStatus.status.txStatus =
                        static_cast<Cv2xStatusType>(msg.tx_multi_pool_status[i].status);
                    addStatus.status.txCause = convertV2xCause(msg.tx_multi_pool_status[i].cause);
                    status.poolStatus.push_back(addStatus);
                } else {
                    auto multiStatus =
                        static_cast<Cv2xStatusType>(msg.tx_multi_pool_status[i].status);
                    if (status.poolStatus[i].status.txStatus != multiStatus) {
                        statusChanged = true;
                        status.poolStatus[i].status.txStatus = multiStatus;
                    }
                    auto multiCause = convertV2xCause(msg.tx_multi_pool_status[i].cause);
                    if (status.poolStatus[i].status.txCause != multiCause) {
                        statusChanged = true;
                        status.poolStatus[i].status.txCause = multiCause;
                    }
                }
            }
        }

        return statusChanged;
    }

    static bool convertTxStatusReport(
        const nas_v2x_tx_status_report_ind_msg_v01 & ind,
        TxStatusReport & report);

private:
    static bool periodicityToUint32(Periodicity p, uint32_t &result);
    static bool uint32ToPeriodicity(uint32_t p, Periodicity &result);
    static bool priorityToUint8(Priority p, uint8_t &result);
    static bool uint8ToPriority(uint8_t p, Priority &result);
    static Cv2xCauseType convertV2xCause(nas_v2x_cause_e_type_v01 cause);

    static const std::string APN_NAME_V2X_IP;
    static const std::string APN_NAME_V2X_NON_IP;
    static const int32_t INVALID_TX_POWER;
    static const std::string CONFIG_CV2X_CBR_DELTA;
    static const std::string CONFIG_CV2X_TUNC_DELTA;
    static bool settingsInited_;
    static uint8_t cbrChangeDelta_;
    static float tuncChangeDelta_;
};
}  // end namespace cv2x
}  // end namespace telux
#endif
