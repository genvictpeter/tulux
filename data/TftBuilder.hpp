/*
 *  Copyright (c) 2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TFTBUILDER_HPP
#define TFTBUILDER_HPP

#include <map>
#include <memory>
#include <mutex>
#include <list>

extern "C" {
#include <qmi/qualcomm_mobile_access_point_msgr_v01.h>
}

#include <data/QCMAP_Client.h>
#include <telux/data/DataConnectionManager.hpp>
#include "common/CommandCallbackManager.hpp"
#include "DataHelper.hpp"

namespace telux {
namespace data {

#define QOS_LAST_SEGMENT 1
#define QOS_INDICATION 0
#define QOS_RESPONSE 1

/**
 * This function is called when TFTBuilder has finished building flows.
 * that are received as part of request response.
 *
 * @param [in] tft        vector of TFT flow info. @ref TrafficFlowTemplate
 */
using TftResponseCb =
    std::function<void(const std::vector<std::shared_ptr<TrafficFlowTemplate>> tft)>;

/**
 * This function is called when TFTBuilder has finished building flows
 * that are received as part of Unsol Indication.
 *
 * @param [in] tft        vector of TFT flow change info. @ref TftChangeInfo
 * @param [in] profHndl   Qcmap profile handle
 */
using TftIndicationCb =
    std::function<void(const std::vector<std::shared_ptr<TftChangeInfo>> tft,
        profile_handle_type_v01 profHndl)>;

/**
 * TftBuilder.hpp
 *
 * TftBuilder is a primary class that implements the logic for creating complete
 * flows from received qos segment and forwarding the complete flow.
 *
 */
class TftBuilder : public std::enable_shared_from_this<TftBuilder> {

 public:
    TftBuilder(TftResponseCb resCb, TftIndicationCb indCb);
    ~TftBuilder();

    void buildTft(qcmap_msgr_global_qos_flow_ind_msg_v01 &indData);

 private:
    TftBuilder(TftBuilder const &) = delete;
    TftBuilder &operator=(TftBuilder const &) = delete;

    void processTft(qcmap_msgr_global_qos_flow_ind_msg_v01 &indData);

    // mutex to access control TFT vectors and RX/TX Offsets.
    std::mutex mutex_;
    TftResponseCb resCb_;
    TftIndicationCb indCb_;

    uint32_t indTxOffset_ = 0;
    uint32_t indRxOffset_ = 0;
    uint32_t respTxOffset_ = 0;
    uint32_t respRxOffset_ = 0;

    std::vector<std::shared_ptr<TftChangeInfo>> indTftChangeInfo_;
    std::vector<std::shared_ptr<TrafficFlowTemplate>> respTft_;

}; // end of TftBuilder class
}  // namespace data
}  // namespace telux

#endif  // TftBuilder_HPP
