/*
 *  Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef DATACALLIMPL_HPP
#define DATACALLIMPL_HPP

#ifdef FEATURE_DATA_QCMAP
extern "C" {
#include <qmi/qualcomm_mobile_access_point_msgr_v01.h>
}
#include <data/QCMAP_Client.h>
#else
extern "C" {
#include <data/dsi_netctrl.h>
}
#endif

#include <memory>
#include <string>

#include <telux/data/DataConnectionManager.hpp>

namespace telux {
namespace data {

class DataCallImpl : public IDataCall {
 public:
    DataCallImpl();
    ~DataCallImpl();
    const std::string &getInterfaceName();
    DataBearerTechnology getCurrentBearerTech();
    DataCallEndReason getDataCallEndReason();
    DataCallStatus getDataCallStatus();
    void getDataCallStatus(DataCallStatus &ipv4, DataCallStatus &ipv6) const;
    IpFamilyInfo getIpv4Info();
    IpFamilyInfo getIpv6Info();
    TechPreference getTechPreference();
    std::list<IpAddrInfo> getIpAddressInfo();
    IpFamilyType getIpFamilyType();
    int getProfileId();
    SlotId getSlotId();
    OperationType getOperationType();
    telux::common::Status requestDataCallStatistics(StatisticsResponseCb callback = nullptr);
    telux::common::Status resetDataCallStatistics(
        telux::common::ResponseCallback callback = nullptr);
    telux::common::Status requestTrafficFlowTemplate(IpFamilyType family,
        TrafficFlowTemplateCb callback);
    telux::common::Status requestDataCallBitRate(
        requestDataCallBitRateResponseCb callback) override;

    void setProfileId(int id);
    void setSlotId(SlotId id);
    void setIpFamilyType(IpFamilyType family);
    void setIpAddrList(std::list<IpAddrInfo> ipAddrList);
    void setTechPreference(TechPreference techPref);
    void setDataCallStatus(DataCallStatus status);
    void setDataCallStatus(DataCallStatus status, IpFamilyType family);
    void setDataCallEndReason(DataCallEndReason endReason);
    void setDataBearerTechnology(DataBearerTechnology bearerTech);
    void setInterfaceName(std::string ifName);
    void setOperationType(OperationType type);

#ifdef FEATURE_DATA_QCMAP
    profile_handle_type_v01 getQcmapProfileHandle();
    void setQcmapProfileHandle(profile_handle_type_v01 ph);
    qcmap_msgr_wwan_call_type_v01 getQcmapCallType();
    void setQcmapCallType(qcmap_msgr_wwan_call_type_v01 callType);
#else
    dsi_hndl_t getDsiHandle();
    void setDsiHandle(dsi_hndl_t hndl);
#endif

 private:
    int profileId_;
    SlotId slotId_;
    IpFamilyType family_ = IpFamilyType::UNKNOWN;
    std::list<IpAddrInfo> ipAddrList_;
    TechPreference techPref_ = TechPreference::TP_ANY;
    DataCallStatus ipv4Status_ = DataCallStatus::INVALID;
    DataCallStatus ipv6Status_ = DataCallStatus::INVALID;
    DataCallStatus status_ = DataCallStatus::INVALID;
    DataCallEndReason endReason_;
    DataBearerTechnology bearerTech_ = DataBearerTechnology::UNKNOWN;
    std::string ifName_;
    OperationType operationType_ = OperationType::DATA_LOCAL;
    mutable std::mutex statusMutex_;

#ifdef FEATURE_DATA_QCMAP
    profile_handle_type_v01 qcmapProfileHandle_;
    qcmap_msgr_wwan_call_type_v01 qcmapCallType_;
#else
    // DSI handle associated with this IDatacall
    dsi_hndl_t dsiHndl_ = nullptr;
#endif
};
}
}

#endif
