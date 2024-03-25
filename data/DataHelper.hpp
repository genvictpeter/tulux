/*
 *  Copyright (c) 2018 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef DATAHELPER_HPP
#define DATAHELPER_HPP

#include <string>
#include <list>

extern "C" {
#include <qmi-framework/qmi_client.h>
#ifdef TELSDK_QOS_ENABLE
#include <qmi/qualcomm_mobile_access_point_msgr_v01.h>
#endif // TELSDK_QOS_ENABLE
}

#include <telux/data/DataDefines.hpp>
#include <telux/common/CommonDefines.hpp>
#ifdef TELSDK_QOS_ENABLE
#include <data/QCMAP_Client.h>
#endif // TELSDK_QOS_ENABLE
#include <telux/data/DataConnectionManager.hpp>
#include "IpFilterImpl.hpp"

const std::string APN_NAME_V2X_IP = std::string("v2x_ip");
const std::string APN_NAME_V2X_NON_IP = std::string("v2x_non_ip");

#define PROTO_ICMP 1        // Internet Control Message Protocol - RFC 792
#define PROTO_IGMP 2        // Internet Group Management Protocol - RFC 1112
#define PROTO_TCP  6        // Transmission Control Protocol - RFC 793
#define PROTO_UDP 17        // User Datagram Protocol - RFC 768
#define PROTO_ESP 50        // Encapsulating Security Payload - RFC 4303
#define PROTO_TCP_UDP 253   // Contain both TCP and UDP info

namespace telux {
namespace data {

class DataHelper {
 public:
    static std::string callEndReasonTypeToString(EndReasonType reasonType);
    static DataCallEndReason qmiToDataCallEndReason(int reasonType, int reasonCode);

    static void logDataCallDetails(DataCallStatus dataCallStatus, int profileId, SlotId slotId,
        DataCallEndReason callEndReason, const std::string interfaceName,
        std::list<telux::data::IpAddrInfo> ipAddrList, DataBearerTechnology bearerTech);
    static bool isValidIpv4Address(const std::string &addr);
    static bool isValidIpv6Address(const std::string &addr);

    // Convert given string address into outAddress
    static int convertAddress(const std::string &inAddr, unsigned char *outAddr);

    // convert a numeric address into a text string suitable  for presentation
    static int converAddrToString(int af, uint32_t *addr, char *str);
#ifdef TELSDK_QOS_ENABLE
    static QosFlowStateChangeEvent fillQosParams(std::shared_ptr<TrafficFlowTemplate> &tft,
        qcmap_msgr_global_qos_flow_ind_msg_v01 &indData, uint32_t &txOffset, uint32_t &rxOffset);

    static void fillIPInfo(std::shared_ptr<IIpFilter> &qosFilter,
        qcmap_msgr_qos_filter_rule_resp_ind_type_v01 qcmFilter);

    static void fillPortInfo(std::shared_ptr<IIpFilter> &qosFilter,
        qcmap_msgr_qos_filter_rule_resp_ind_type_v01 qcmFilter, IpProtocol protocol);

    static void fillIPv4Info(std::shared_ptr<IIpFilter> &qosFilter,
        qcmap_msgr_qos_filter_rule_resp_ind_type_v01 &qcmFilter);

    static void fillIPv6Info(std::shared_ptr<IIpFilter> &qosFilter,
        qcmap_msgr_qos_filter_rule_resp_ind_type_v01 &qcmFilter);

    static void fillQosGrantedFlow(QosIPFlowInfo &qosFilter,
        qcmap_msgr_qos_flow_type_v01 &grantedFlow);

    static void fillQosFilter(std::shared_ptr<TrafficFlowTemplate> &tft,
        qcmap_msgr_global_qos_flow_ind_msg_v01 &indData, uint32_t &txOffset, uint32_t &rxOffset);

    static void addFilterInfo(QosFilterRule &qosFilter,
        qcmap_msgr_qos_filter_rule_resp_ind_type_v01 &filter);
#endif // TELSDK_QOS_ENABLE

};
}
}

#endif
