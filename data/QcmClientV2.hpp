/*
 *  Copyright (c) 2020-2022 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       QcmClient.hpp
 *
 * @brief      Data QCMAP Client is a singleton class interacts with QCMAP Client APIs to
 *             send data call related requests and receive the indications.
 *
 *             1. it does conversion of telux data structures into QCMAP data structures.
 *                for example: telux::data::IpFamilyType convert into
 *                qcmap_msgr_ip_family_enum_v01::QCMAP_MSGR_IP_FAMILY_V4
 *             2. DataConnectionManager registers with QcMapClient to get the indications
 *             3. Implements DSDA Functionality
 *
 */

#ifndef QCMCLIENT_HPP
#define QCMCLIENT_HPP

#include <condition_variable>
#include <memory>
#include <mutex>

#include <telux/common/CommonDefines.hpp>
#include <telux/data/DataConnectionManager.hpp>
#include <telux/data/DataDefines.hpp>
#include <telux/data/net/VlanManager.hpp>
#include <telux/data/net/FirewallManager.hpp>
#include <telux/data/net/NatManager.hpp>
#include <telux/data/net/SocksManager.hpp>
#include <telux/data/net/BridgeManager.hpp>
#include <telux/data/net/L2tpManager.hpp>
#include <telux/data/DataSettingsManager.hpp>


extern "C" {
#include <qmi/qualcomm_mobile_access_point_msgr_v01.h>
}
#include <data/QCMAP_Client.h>

#include "common/AsyncTaskQueue.hpp"
#include "common/CommandCallbackManager.hpp"
#include "common/ListenerManager.hpp"

#include "DataCallImpl.hpp"
#include "DataHelper.hpp"
#include "net/FirewallEntryImpl.hpp"

#ifdef TELSDK_QOS_ENABLE
#include "TftBuilder.hpp"
#endif // TELSDK_QOS_ENABLE

#define INVALID_HANDLE 0

// V2x profile Ids set by the modem
// in the SW MBN
#define MIN_V2X_PROFILE_ID 38
#define MAX_V2X_PROFILE_ID 40

#define FROM_CACHE     true
#define NOT_FROM_CACHE false

using namespace telux::data::net;

namespace telux {
namespace data {

// forward declarations
class IQcmapGetWwanPolicyListCallback;
class IQcmapCreateWwanPolicyCallback;
class IQcmapStartStopDataCallback;
class DataSubSystemStateMachine;

enum class IndicationType {
    BRING_UP_WWAN_INDICATION = 0x00,
    TEAR_DOWN_WWAN_INDICATION = 0x01,
    WWAN_STATUS_INDICATION = 0x02,
};

struct GetWwanPolicyUserData {
    int cmdCallbackId = INVALID_COMMAND_ID;
    void *data = NULL;
    int profileId = 0;
    qcmap_msgr_ip_family_enum_v01 ipFamily = QCMAP_MSGR_IP_FAMILY_V4V6_V01;
};

struct QcmapUserData {
    int cmdCallbackId = INVALID_COMMAND_ID;
    void *data = NULL;
};

class IDataCallsCountListener {
 public:
    /**
     * Called when active data calls count changed from 0 to non-zero or from non-zero to 0.
     *
     * [param] haveCalls -- indicate whether there is active wwan data calls or not
     *
     */
    virtual void onHaveActiveCalls(bool haveNonV2xCall){};

    /**
     * Destructor for IDataCallsCountListener
     */
    virtual ~IDataCallsCountListener(){};
};

class QcmClient {
 public:
    static const std::shared_ptr<QcmClient> getQcmapClient();

    telux::common::Status setDefaultProfile(OperationType oprType, uint8_t profileId,
        SlotId slotId, telux::common::ResponseCallback callback = nullptr) ;

    telux::common::Status getDefaultProfile(OperationType oprType,
        DefaultProfileIdResponseCb callback) ;

    telux::common::Status startDataCall(int profileId, int ipFamilyType, SlotId slotId,
        DataCallResponseCb callback = nullptr, OperationType oprType = OperationType::DATA_LOCAL);


    telux::common::Status stopDataCall(int profileId, int ipFamilyType, SlotId slotId,
        DataCallResponseCb callback = nullptr, OperationType oprtype = OperationType::DATA_LOCAL);

    telux::common::Status requestDataCallBitRate(int profileId, SlotId slotId,
        OperationType operationType, requestDataCallBitRateResponseCb callback);

    telux::common::Status requestConnectedDataCallList(OperationType oprtype, SlotId slotId,
        DataCallListResponseCb callback, bool fromCache = false);

    telux::common::Status requestDataCallStatistics(
        int profileId, int ipFamilyType, SlotId slotId, StatisticsResponseCb callback = nullptr);

    telux::common::Status resetDataCallStatistics(int profileId, int ipFamilyType,
        SlotId slotId, telux::common::ResponseCallback callback = nullptr);

    telux::common::Status requestTrafficFlowTemplate(int profileId, int ipFamilyType,
        TrafficFlowTemplateCb callback);

    telux::common::Status setBackhaulPreference(OperationType oprType,
        std::vector<BackhaulType> backhaulPref, telux::common::ResponseCallback callback);

    telux::common::Status requestBackhaulPreference(OperationType oprType,
        RequestBackhaulPrefResponseCb callback);

    telux::common::Status setBandInterferenceConfig(OperationType oprType, bool enable,
        std::shared_ptr<BandInterferenceConfig> config, telux::common::ResponseCallback callback);

    telux::common::Status requestBandInterferenceConfig(
        OperationType oprType, RequestBandInterferenceConfigResponseCb callback);

    telux::common::Status init();
    bool isDataSubSysUp();

    /* Interfaces to support the sub-system state machine */
    telux::common::Status enableMobileAP(OperationType operationType);
    telux::common::Status registerForIndications(OperationType operationType);
    void onDataSubSystemInitialized(bool isInitializationSuccessful);
    void onDataSubSystemStatusChange(telux::common::ServiceStatus status);
    void notifyNoNetOnDataCalls(OperationType operationType);

    telux::common::Status registerListener(std::weak_ptr<IDataConnectionListener> listener);
    telux::common::Status deregisterListener(std::weak_ptr<IDataConnectionListener> listener);

    static void qcmapMsgrIndication(qmi_client_type userHandle, /* QMI user handle       */
        unsigned int msgId,                                     /* Indicator message ID  */
        void *indBuf,                                           /* Raw indication data   */
        unsigned int indBufLen,                                 /* Raw data length       */
        void *indCallbackData                                   /* User call back handle */
        );

    QcmClient();
    ~QcmClient();
    QcmClient(QcmClient const &) = delete;
    const QcmClient &operator=(QcmClient const &) = delete;

    // Set LOCAL or REMOTE client configuration
    telux::common::Status setClientConfig(OperationType oprType, qmi_error_type_v01 &qmiErr);

    telux::common::Status addStaticNatEntry(telux::data::OperationType oprType, SlotId slotId,
        int profileid, const NatConfig &snatConfig,
        telux::common::ResponseCallback callback = nullptr);

    telux::common::Status removeStaticNatEntry(telux::data::OperationType oprType, SlotId slotId,
        int profileid, const NatConfig &snatConfig,
        telux::common::ResponseCallback callback = nullptr);

    telux::common::Status requestStaticNatEntries(telux::data::OperationType oprType, SlotId slotId,
        int profileid,StaticNatEntriesCb snatEntriesCb);

    telux::common::Status setFirewall(telux::data::OperationType oprType, SlotId slotId, int profileid,
        bool enable, bool allowPackets, telux::common::ResponseCallback callback = nullptr);
    telux::common::Status requestFirewallStatus(
        telux::data::OperationType oprType, SlotId slotId, int profileid, FirewallStatusCb callback);
    telux::common::Status addFirewallEntry(telux::data::OperationType oprType, SlotId slotId,
        int profileid, std::shared_ptr<IFirewallEntry> entry,
        telux::common::ResponseCallback callback = nullptr);
    telux::common::Status requestFirewallEntries(
        telux::data::OperationType oprType, SlotId slotId, int profileid, FirewallEntriesCb callback);
    telux::common::Status removeFirewallEntry(telux::data::OperationType oprType, SlotId slotId,
        int profileid, uint32_t handle, telux::common::ResponseCallback callback = nullptr);
    telux::common::Status enableDmz(telux::data::OperationType oprType, SlotId slotId, int profileid,
        const std::string ipAddr, telux::common::ResponseCallback callback = nullptr);
    telux::common::Status disableDmz(telux::data::OperationType oprType, SlotId slotId, int profileid,
        const telux::data::IpFamilyType ipType, telux::common::ResponseCallback callback);
    telux::common::Status requestDmzEntry(
        telux::data::OperationType oprType, SlotId slotId, int profileid, DmzEntriesCb dmzCb);

    telux::common::Status createVlan(
        OperationType oprType, const VlanConfig &vlanConfig, CreateVlanCb callback);
    telux::common::Status removeVlan(OperationType oprType,
        int16_t vlanId, InterfaceType ifaceType, telux::common::ResponseCallback callback);
    telux::common::Status queryVlanInfo(OperationType oprType, QueryVlanResponseCb callback);
    telux::common::Status bindWithProfile(OperationType oprType, SlotId slotId,
        int profileId, int vlanId, telux::common::ResponseCallback callback);
    telux::common::Status unbindFromProfile(OperationType oprType, SlotId slotId,
        int profileId, int vlanId, telux::common::ResponseCallback callback);
    telux::common::Status queryVlanMappingList(
        OperationType oprType, SlotId slotId, VlanMappingResponseCb callback);

    telux::common::Status enableSocks(bool enable,
        telux::data::OperationType oprType, telux::common::ResponseCallback callback = nullptr);

    telux::common::Status enableBridge(OperationType oprType, bool enable,
        telux::common::ResponseCallback callback = nullptr);
    telux::common::Status addBridge(OperationType oprType, BridgeInfo config,
        telux::common::ResponseCallback callback = nullptr);
    telux::common::Status requestBridgeInfo(OperationType oprType, BridgeInfoResponseCb callback);
    telux::common::Status removeBridge(OperationType oprType, std::string ifaceName,
                        telux::common::ResponseCallback callback = nullptr);

    telux::common::Status setL2tpConfigSync(OperationType oprType, bool enable, bool enableMss,
        bool enableMtu, uint32_t mtuSize, telux::common::ResponseCallback callback = nullptr);
    telux::common::Status addL2tpTunnelSync(OperationType oprType,
      const L2tpTunnelConfig &l2tpTunnelConfig, telux::common::ResponseCallback callback = nullptr);
    telux::common::Status requestL2tpConfigSync(OperationType oprType, L2tpConfigCb l2tpConfigCb);
    telux::common::Status removeL2tpTunnelSync(OperationType oprType,
        uint32_t tunnelId, telux::common::ResponseCallback callback = nullptr);
    void onRatChanged(DataBearerTechnology bearer, SlotId slotId);
    telux::common::Status setWwanConnectivityConfig(OperationType oprType, SlotId slotId,
        bool allow, telux::common::ResponseCallback callback);
    telux::common::Status requestWwanConnectivityConfig(OperationType oprType, SlotId slotId,
        requestWwanConnectivityConfigResponseCb callback);
    telux::common::Status registerDataCallCountListener(SlotId slotId,
        std::weak_ptr<IDataCallsCountListener> listener);
    telux::common::Status deregisterDataCallCountListener(SlotId slotId,
        std::weak_ptr<IDataCallsCountListener> listener);
	static std::mutex exitingMutex_;
    static bool exiting_;
 private:
    std::mutex initMtx_;
    std::mutex mtx_;
    std::mutex qcmapConfigMtx_;
    //Todo: explore possibility to reuse mtx_
    std::mutex ssrUpdateMtx_;
    std::mutex dataCallNotifyMtx_;
    bool dataSubSysUp_ = false;
    bool modemStatusUp_ = false;
    bool modemQcmapUp_ = false;
    bool eapQcmapUp_ = false;
    std::condition_variable ssrCondVar_;
    bool modemServiceStatusAvailable_ = false;
    std::shared_ptr<DataSubSystemStateMachine> dataSubSystemStateMachine_;
    std::condition_variable callListCv_;
    std::map<OperationType, bool> callListValid_ {
        {OperationType::DATA_LOCAL, false}, {OperationType::DATA_REMOTE, false}};

    std::shared_ptr<QCMAP_Client> qcmapClient_;
#ifdef TELSDK_QOS_ENABLE
    std::shared_ptr<TftBuilder> tftBuilder_ = nullptr;
#endif // TELSDK_QOS_ENABLE
    std::shared_ptr<telux::common::ListenerManager<IDataConnectionListener>> listenerMgr_;
    // map for qcmap ip family to wwan call type
    std::map<qcmap_msgr_ip_family_enum_v01, qcmap_msgr_wwan_call_type_v01> ipFamilyCallTypeMap_;

    // Contains list of data calls
    std::vector<std::shared_ptr<DataCallImpl>> dataCalls_;
    TrafficFlowTemplateCb qosFlowRespCallback_;
    std::shared_ptr<telux::common::AsyncTaskQueue<void>> taskQ_;
    std::mutex callsCountMtx_;
    std::map<SlotId, std::shared_ptr<telux::common::ListenerManager<IDataCallsCountListener>>>
        callsCountListenerMgr_ {{SlotId::SLOT_ID_1, nullptr}, {SlotId::SLOT_ID_2, nullptr}};
    std::map<SlotId,int> nonV2xCalls_{{SlotId::SLOT_ID_1,0}, {SlotId::SLOT_ID_2,0}};
    // Qcm SSR related function
    void subSysRestartCleanup();
    void subSysRestartRecover();


    // Get QCMAP_Client mobile_ap handle, Handle identifying the mobile AP call instance
    // The value must be the handle previously returned by enable Mobile Ap
    profile_handle_type_v01 getMobileApHandle();

    void handleBringupInd(qmi_client_type userHandle, unsigned int msgId, void *indBuf,
        unsigned int indBufLen, uint32_t qcmapClientHandle);
    void handleTeardownInd(qmi_client_type userHandle, unsigned int msgId, void *indBuf,
        unsigned int indBufLen, uint32_t qcmapClientHandle);
    void handleWwanStatusInd(qmi_client_type userHandle, unsigned int msgId, void *indBuf,
        unsigned int indBufLen, uint32_t qcmapClientHandle);
    void handleSubSysRestartInd(qmi_client_type userHandle, unsigned int msgId, void *indBuf,
        unsigned int indBufLen, uint32_t qcmapClientHandle);
    void handleHwAccelStatusInd(qmi_client_type userHandle, unsigned int msgId, void *indBuf,
        unsigned int indBufLen, uint32_t qcmapClientHandle);
    void handleWwanConnectivityConfigInd(qmi_client_type userHandle, unsigned int msgId, void *indBuf,
        unsigned int indBufLen, uint32_t qcmapClientHandle);
    void updateAndNotifyDataConnectionListener(IndicationType dataCallInd,
        DataCallStatus dataCallStatus, int callEndReasonType, int callEndReasonCode,
        const std::string interfaceName, std::list<telux::data::IpAddrInfo> ipAddrList,
        profile_handle_type_v01 profHandle, SlotId slotId, IpFamilyType family);
    void updateAndNotifyDataConnectionListenerSync(IndicationType dataCallInd,
        DataCallStatus dataCallStatus, int callEndReasonType, int callEndReasonCode,
        const std::string interfaceName, std::list<telux::data::IpAddrInfo> ipAddrList,
        profile_handle_type_v01 profHandle, SlotId slotId, IpFamilyType family);
    void handleQosFlowInd(qmi_client_type userHandle, unsigned int msgId, void *indBuf,
        unsigned int indBufLen, uint32_t qcmapClientHandle);

    std::shared_ptr<DataCallImpl> getDataCall(int profileId, OperationType oprType, SlotId slotId);
    std::shared_ptr<DataCallImpl> getDataCall(uint32_t profileHandle);

    /**
     * update dataCallMap_ based on requested profile id
     *
     * @param [in] profileId
     * @param [in] ipFamilyType
     * @param [out] dataCall     update datacall impl class
     * @param [out] dsiHndl      update dsi handle for the given input
     */
    bool createAndUpdateDataCall(int profileId, int ipFamilyType,
        std::shared_ptr<DataCallImpl> &dataCall, OperationType oprType, SlotId slotId);

    qcmap_msgr_wwan_call_type_v01 qmiCallTypeFromIpFamily(qcmap_msgr_ip_family_enum_v01 ipFamily);

    profile_handle_type_v01 getProfileHandleFromWwanPolicyList(
        int profileId, SlotId slotId, qcmap_net_policy_info *policy, qmi_error_type_v01 *qmiErr);

    bool getDataCallInfoFromProfileHandle(SlotId& slotId, int& profileId, int& ipFamily,
        OperationType& oprType, profile_handle_type_v01 profileHandle);

    bool getWwanPolicyFromProfileHandle(qcmap_net_profile_and_policy_info& wanPolicy,
        profile_handle_type_v01 profileHandle, OperationType oprType);

    uint32_t getProfileHandle(const qcmap_wwan_policy_list_info &resp,
        int profileId, SlotId slotId, qcmap_net_policy_info *policy);

    void getCallEndReason(qcmap_msgr_wwan_call_end_reason_v01 endReason,
        int &endReasonType, int &endReasonCode);
    void qcmToDataCallStatus(profile_handle_type_v01 profHndl,
        qcmap_msgr_wwan_status_enum_v01 status, DataCallStatus &dcStatus, IpFamilyType &family);
    void qcmWwanInfoToIpAddr(qcmap_msgr_wwan_info_v01 wwanInfo, std::string &interfaceName,
        std::list<telux::data::IpAddrInfo> &ipAddrList);
    void qcmWwanInfoToIpAddr(qcmap_msgr_wwan_info_ex_v01 wwanInfoEx, std::string &interfaceName,
        std::list<telux::data::IpAddrInfo> &ipAddrList);
    void qcmWwanInfoToIpAddr(qcmap_connected_wwan_info wwanInfoEx, std::string &interfaceName,
        std::list<telux::data::IpAddrInfo> &ipAddrList, IpFamilyType &ipFamilyType, SlotId &slotId);
    void logQcmapWwanStatus(
        profile_handle_type_v01 profHndl, qcmap_msgr_wwan_status_enum_v01 status);

    void notifyDataCallInfoChanged(std::shared_ptr<IDataCall> dataCall);
    void notifyDataSysHealthChangedSync(unsigned int msgId, uint8_t modemStatus,
        uint8_t qcmapServerType, uint8_t qcmapServerStatues);
    void notifyHwAccelStatusChangedSync(ServiceState serviceState);
    void notifyWwanConnectivityConfigChangedSync(SlotId slotId, bool isConnectivityAllowed);
    void notifyTrafficFlowTemplateChange(std::shared_ptr<IDataCall> dataCall,
        std::vector<std::shared_ptr<TftChangeInfo>> tft);

    void onTftIndicationComplete(const std::vector<std::shared_ptr<TftChangeInfo>> tft,
        profile_handle_type_v01 profHndl);

    void onTftResponseComplete(const std::vector<std::shared_ptr<TrafficFlowTemplate>> tft);

    void setDefaultProfileSync(OperationType oprType, uint8_t profileId,
        SlotId slotId, telux::common::ResponseCallback callback = nullptr);
    void getDefaultProfileSync(OperationType oprType, DefaultProfileIdResponseCb callback);
    void startDataCallSync(int profileId, int ipFamilyType, SlotId slotId,
        DataCallResponseCb callback = nullptr, OperationType oprType = OperationType::DATA_LOCAL);
    void stopDataCallSync(int profileId, int ipFamilyType, SlotId slotId,
        DataCallResponseCb callback = nullptr, OperationType oprType = OperationType::DATA_LOCAL);
    void requestDataCallBitRateSync(int profileId, SlotId slotId,
         OperationType operationType, requestDataCallBitRateResponseCb callback);
    void requestConnectedDataCallListSync(OperationType oprType, SlotId slotId,
         DataCallListResponseCb callback, bool fromCache);
    void requestDataCallStatisticsSync(
        int profileId, int ipFamilyType, SlotId slotId, StatisticsResponseCb callback = nullptr);
    void resetDataCallStatisticsSync(int profileId, int ipFamilyType,
        SlotId slotId, telux::common::ResponseCallback callback = nullptr);
    bool getAllConnectedPDN(OperationType oprType, uint8 &numIface,
                            qcmap_connected_wwan_info *wwanInfoEx,
                            qmi_error_type_v01 &qmiErr);
    void getConnectedDataCallListWithOpTypeAndSlotId(
        std::vector<std::shared_ptr<IDataCall>> &dataCallList, OperationType type,
        SlotId slotId, qmi_error_type_v01 &qmiErr);
    DataBearerTechnology getBearerTechSync(std::shared_ptr<DataCallImpl> dataCall);

    telux::common::Status addStaticNatEntrySync(telux::data::OperationType oprType, SlotId slotId,
        int profileid, const NatConfig &snatConfig,
        telux::common::ResponseCallback callback = nullptr);
    telux::common::Status removeStaticNatEntrySync(telux::data::OperationType oprType, SlotId slotId,
        int profileid, const NatConfig &snatConfig,
        telux::common::ResponseCallback callback = nullptr);
    telux::common::Status requestStaticNatEntriesSync(telux::data::OperationType oprType,
        SlotId slotId, int profileid,StaticNatEntriesCb snatEntriesCb);
    telux::common::Status setFirewallSync(telux::data::OperationType oprType, SlotId slotId,
        int profileid, bool enable, bool allowPackets,
        telux::common::ResponseCallback callback = nullptr);
    telux::common::Status requestFirewallStatusSync(telux::data::OperationType oprType,
        SlotId slotId, int profileid,FirewallStatusCb callback);
    telux::common::Status addFirewallEntrySync(telux::data::OperationType oprType, SlotId slotId,
        int profileid, std::shared_ptr<IFirewallEntry> entry,
        telux::common::ResponseCallback callback = nullptr);
    telux::common::Status requestFirewallEntriesSync(telux::data::OperationType oprType,
        SlotId slotId, int profileid,FirewallEntriesCb callback);
    telux::common::Status removeFirewallEntrySync(telux::data::OperationType oprType, SlotId slotId,
        int profileid, uint32_t handle, telux::common::ResponseCallback callback = nullptr);
    telux::common::Status enableDmzSync(telux::data::OperationType oprType, SlotId slotId,
        int profileid,const std::string ipAddr, telux::common::ResponseCallback callback = nullptr);
    telux::common::Status disableDmzSync(telux::data::OperationType oprType, SlotId slotId,
        int profileid, const telux::data::IpFamilyType ipType,
        telux::common::ResponseCallback callback = nullptr);
    telux::common::Status requestDmzEntrySync(telux::data::OperationType oprType, SlotId slotId,
        int profileid, DmzEntriesCb dmzCb);
    void getEntriesFromQcmHandleList(int handle_list_len,
        qcmap_msgr_firewall_conf_t extd_firewall_handle_list,
        std::vector<std::shared_ptr<IFirewallEntry>> &entries);

    telux::common::Status createVlanSync(
        OperationType oprType, const VlanConfig &vlanConfig, CreateVlanCb callback);
    telux::common::Status removeVlanSync(OperationType oprType,
        int16_t vlanId, InterfaceType ifaceType, telux::common::ResponseCallback callback);
    telux::common::Status queryVlanInfoSync(OperationType oprType, QueryVlanResponseCb callback);
    telux::common::Status bindWithProfileSync(OperationType oprType, SlotId slotId, int profileId,
        int vlanId, telux::common::ResponseCallback callback);
    telux::common::Status unbindFromProfileSync(OperationType oprType, SlotId slotId, int profileId,
        int vlanId, telux::common::ResponseCallback callback);
    telux::common::Status queryVlanMappingListSync(
        OperationType oprType, SlotId slotId, VlanMappingResponseCb callback);

    telux::common::Status enableSocksSync(bool enable,
        telux::data::OperationType oprType, telux::common::ResponseCallback callback = nullptr);

    telux::common::Status enableBridgeSync(OperationType oprType, bool enable,
        telux::common::ResponseCallback callback = nullptr);
    telux::common::Status addBridgeSync(OperationType oprType, BridgeInfo config,
        telux::common::ResponseCallback callback = nullptr);
    telux::common::Status requestBridgeInfoSync(OperationType oprType,
        BridgeInfoResponseCb callback);
    telux::common::Status removeBridgeSync(OperationType oprType, std::string ifaceName,
                        telux::common::ResponseCallback callback = nullptr);

    void requestTrafficFlowTemplateSync(int profileId, int ipFamilyType,
        TrafficFlowTemplateCb callback);
    void setBackhaulPreferenceSync(OperationType oprType,
        std::vector<BackhaulType> backhaulPref, telux::common::ResponseCallback callback);
    void requestBackhaulPreferenceSync(
        OperationType oprType, RequestBackhaulPrefResponseCb callback);

    void setBandInterferenceConfigSync(OperationType oprType, bool enable,
        std::shared_ptr<BandInterferenceConfig> config, telux::common::ResponseCallback callback);
    void requestBandInterferenceConfigSync(
        OperationType oprType, RequestBandInterferenceConfigResponseCb callback);

    void setWwanConnectivityConfigSync(OperationType oprType, SlotId slotId, bool allow,
        telux::common::ResponseCallback callback);
    void requestWwanConnectivityConfigSync(OperationType oprType, SlotId slotId,
        requestWwanConnectivityConfigResponseCb callback);

    uint32_t getProfileIdFromWwanPolicyList(int& slotId,
        profile_handle_type_v01 profHndl, qmi_error_type_v01 *qmiErr);

    uint32_t getProfileId(int& slotId,
        const qcmap_wwan_policy_list_info &resp, profile_handle_type_v01 profHndl);

    qcmap_msgr_backhaul_type_enum_v01 convertToQcmapBackhaul(BackhaulType backhaul);
    BackhaulType convertFromQcmapBackhaul(qcmap_msgr_backhaul_type_enum_v01 backhaul, bool& error);

    /**
     * Perform QCMAP initialization by setting correct operation type and then
     * sets QCMAP profile handle associated with profile Id argument. If QCMAP profile handle
     * Does not exist, it will return error if createHandle is false or create new handle if
     * createHandle are is set to true
     *
     * @param [in] oprType        Operation type (LOCAL vs REMOTE)
     * @param [in] profileId      Set QCMAP handle associated with this profile id
     * @param [in] createHandle   If qcmap handle associated with profile id is not found, function
     *                            will create new handle if this parameter is set to true. Otherwise
     *                            default behavior will be return error
    */
    profile_handle_type_v01 setQcmapConfigLocked(telux::data::OperationType oprType,
        int profileId, qmi_error_type_v01 &qmiErr, SlotId slotId, IpFamilyType ipFamily,
        bool createHandle = false);
};
}
}
#endif
