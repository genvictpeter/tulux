/*
 *  Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       WdsQmiClient.hpp
 *
 * @brief      Data Qmi Wds Client is a singleton class to list, create, modify and
 *             delete modem profiles. It contains structure definitions and function
 *             prototype for WDS QMI Client, this class provides mechanism to send
 *             QMI messages to WDS QMI Service and get the results. It then send the
 *             events back to the caller.
 *
 */

#ifndef WDSQMICLIENT_HPP
#define WDSQMICLIENT_HPP

#include <map>
#include <memory>
#include <vector>

extern "C" {
#include <qmi/wireless_data_service_v01.h>
}

#include <telux/common/CommonDefines.hpp>
#include <telux/data/DataDefines.hpp>

#include "QmiClient.hpp"

#define DEFAULT_TIMEOUT_IN_MILLISECONDS 4000
#define DEFAULT_INTERFACE_UP_TIMEOUT_MILLISECONDS 100
#define DEFAULT_INTERFACE_UP_NUM_RETRIES 100

namespace telux {
namespace qmi {
// forward declarations
class IQmiCommandResponseCallback;
class IQmiWdsCreateProfileCallback;
class IQmiWdsGetProfileSettingsCallback;
class IQmiWdsGetProfileListCallback;
class IQmiWdsV2xSpsFlowInfoCallback;
class IQmiWdsRuntimeSettingsCallback;
class IQmiWdsV2xCapabilityInfoCallback;
class IQmiWdsListener;
class IQmiWdsGetDormancyStatusCallback;

/**
 * Structure to send the user data for each QMI requests, it's supplied
 * by the client
 */
struct WdsQmiUserData {
   void *data;
   int profileType;
   uint8_t profileIndex;
};

/**
 * Structure to send the profile params supplied by the client for QMI request.
 */
struct QmiProfileParams {
   std::string profileName; /**< Profile Name */
   int profileType;         /**< Technology preference */
   std::string apn;         /**< APN name */
   std::string userName;    /**< APN user name (if any) */
   std::string password;    /**< APN password (if any) */
   int authType;            /**< Authentication preference */
   int ipFamilyType;        /**< Preferred IP family for the call */
   uint64_t apnTypes = 0;     /**< APN Types ref @ApnMaskType */
};

/**
 * Structure to represent single profile settings, this is updated
 * as a response of sendWdsGetProfileSettingsRequest()
 */
struct ProfileSettingsResp {
   uint8_t profileIndex{};
   QmiProfileParams profileParams{};
};

/**
 * Structure to represent single profile list, this is updated
 * as a response of sendWdsGetProfileListRequest()
 */
struct ProfileListInfo {
   int profileIndex;
   std::string profileName;
   int profileType;
};

/**
 * This class provides mechanism to send messages to QMI WDS Subsystem and
 * supports creating, retrieving, modifying and deleting operations on data profiles
 * existed on the device.
 */

class WdsQmiClient : public QmiClient {
public:

   /**
    * Initializes the QmiClient for specific QMI service type.
    * param [in] idlServiceObject QMI IDL Service Object Type.
    */
   telux::common::Status initClientByIface(qmi_idl_service_object_type idlServiceObject,
                                           std::string ifaceName, int muxId);

   /**
    * This function is called by the QmiClient::qmiIndicationCallback
    * infrastructure receives an indication for this client.
    *
    * NOTE: Callback happens in the QCCI thread context and raises signal.
    *
    * @param [in] userHandle           Opaque handle used by the infrastructure to
    *                                  identify different services.
    * @param [in] msgId                Message ID of the indication
    * @param [in] indBuf               Buffer holding the encoded indication
    * @param [in] indBufLen            Length of the encoded indication
    * @param [in] indCbData            Cookie user data value supplied by the client during
    *                                  registration
    */
   void indicationHandler(qmi_client_type userHandle, unsigned int msgId, void *indBuf,
                          unsigned int indBufLen, void *indCbData) override;

   /**
    * This function is called by the QmiClient::qmiAsyncResponseHandler
    * infrastructure receives an asynchronous response for this client.
    *
    * NOTE: Callback happens in the QCCI thread context and raises signal.
    *
    * @param [in] msgId                Message ID of the indication
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             Command callback pointer
    */
   void asyncResponseHandler(unsigned int msgId, void *respCStruct, unsigned int respCStructLen,
                             void *userData, qmi_client_error_type transpErr,
                             std::shared_ptr<telux::common::ICommandCallback> callback) override;

   /**
    * This function is called by qmiClientErrorCallbackSync when the service
    * terminates or deregisters.
    *
    * NOTE: Callback happens on new thread created by std::async.
    *
    * @param [in] clientError           Error value
    */
    void errorHandler(qmi_client_error_type clientError);

   /**
    * This function is called by qmiClientNotifierCallbackSync when a service
    * event occurs indicating that the service count has changed.
    *
    * NOTE: Callback happens on new thread created by std::async.
    *
    * @param [in] serviceEvent          Event type
    */
    void notifierHandler(qmi_client_notify_event_type serviceEvent);

   /**
    * Registers for QMI Indications supported by WdsQmiClient API.
    *
    * @param [in] callback       Command callback pointer
    * @param [in] userData       Cookie user data value supplied by the client
    */
   telux::common::Status registerForIndications(
      std::shared_ptr<IQmiCommandResponseCallback> callback=nullptr, void *userData = NULL);

   /**
    * Send an asynchronous WDS create profile request to the Service
    *
    * @param [in] profileParams  Profile Params configuration to be passed for modifying profile
    *                            either for 3GPP or 3GPP2
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendCreateProfileRequest i.e. success or suitable status code.
    */
   telux::common::Status sendCreateProfileRequest(
      const QmiProfileParams &profileParams,
      std::shared_ptr<IQmiWdsCreateProfileCallback> callback = nullptr, void *userData = NULL);

   /**
    * Send an asynchronous WDS delete profile request to the Service
    *
    * @param [in] profileType    Technology type of the profile 3GPP, 3GPP2, EPC
    * @param [in] profileIndex   unique identifier for the profile
    *                            packet data network
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendDeleteProfileRequest i.e. success or suitable status code.
    */
   telux::common::Status sendDeleteProfileRequest(
      int profileType, uint8_t profileIndex,
      std::shared_ptr<IQmiCommandResponseCallback> callback = nullptr, void *userData = NULL);

   /**
    * Send an asynchronous WDS profile settings request to the Service
    *
    * @param [in] profileType    Technology type of the profile 3GPP, 3GPP2, EPC
    * @param [in] profileIndex   unique identifier for the profile
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendProfileSettingsRequest i.e. success or suitable status code.
    */
   telux::common::Status sendGetProfileSettingsRequest(
      int profileType, uint8_t profileIndex,
      std::shared_ptr<IQmiWdsGetProfileSettingsCallback> callback = nullptr, void *userData = NULL);

   /**
    * Send an asynchronous WDS profile list request to the Service
    *
    * @param [in] profileType    Technology type of the profile 3GPP, 3GPP2, EPC
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendProfileSettingsRequest i.e. success or suitable status code.
    */
   telux::common::Status sendGetProfileListRequest(
      int profileType, std::shared_ptr<IQmiWdsGetProfileListCallback> callback = nullptr,
      void *userData = NULL);

   /**
    * Send an asynchronous WDS modify profile request to the Service
    *
    * @param [in] profileId      Profile identifier.
    * @param [in] profileParams  Profile Params configuration to be passed for modifying profile
    *                            either for 3GPP or 3GPP2
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendModifyProfileRequest i.e. success or suitable status code.
    */
   telux::common::Status sendModifyProfileRequest(
      int profileId, const QmiProfileParams &profileParams,
      std::shared_ptr<IQmiCommandResponseCallback> callback = nullptr, void *userData = NULL);

   /**
    * Register with QMI for events like profile change from the WDS Subsystem
    */
   telux::common::Status registerProfileChangeIndication(
      std::shared_ptr<IQmiCommandResponseCallback> callback, void *userData = nullptr);

   /**
    * Register with QMI for RRC events like DRB Status change from the WDS Subsystem
    */
   telux::common::Status registerDrbChangeIndication(
      std::shared_ptr<IQmiCommandResponseCallback> callback, void *userData = nullptr);

   /**
    * Deregister from QMI RRC events like DRB Status change from the WDS Subsystem
    */
   telux::common::Status deregisterDrbChangeIndication(
      std::shared_ptr<IQmiCommandResponseCallback> callback, void *userData = nullptr);

   /**
    * Configure all profiles for which profile change indication will be received.
    */
   telux::common::Status configureProfilesForIndication(
      std::shared_ptr<IQmiCommandResponseCallback> callback, void *userData = nullptr);

   /**
    * De-configure profiles to stop receiving indication for profile change.
    */
   telux::common::Status deconfigureProfilesForIndication(
      std::shared_ptr<IQmiCommandResponseCallback> callback, void *userData = nullptr);

   /**
    * Send an asynchronous WDS set Client IP Family request
    *
    * @param [in] ipFamily       IP Family type.
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendModifyProfileRequest i.e. success or suitable status code.
    */
   telux::common::Status setClientIpFamily(wds_ip_family_enum_v01 ipFamily,
      std::shared_ptr<IQmiCommandResponseCallback> callback = nullptr, void *userData = NULL);

   /**
    * Send an asynchronous WDS bind mux to data port request
    *
    * @param [in] muxId          Desired MUX ID
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendModifyProfileRequest i.e. success or suitable status code.
    */
   telux::common::Status bindMuxDataPort(uint8_t muxId,
      std::shared_ptr<IQmiCommandResponseCallback> callback = nullptr, void *userData = NULL);

   /**
    * Send an asynchronous WDS Bind subscription request
    *
    * @param [in] subType        Subscription type to bind to
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendModifyProfileRequest i.e. success or suitable status code.
    */
   telux::common::Status bindSubscription(wds_bind_subscription_enum_v01 subType,
      std::shared_ptr<IQmiCommandResponseCallback> callback = nullptr, void *userData = NULL);

   /**
    * Send an asynchronous WDS V2X Service List Subscribe request to subscribe
    *
    * @param [in] reqId          ID of this request
    * @param [in] port           Port number
    * @param [in] idList         List of service Ids to subscribe to. Defaults to wildcard if nullptr
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendModifyProfileRequest i.e. success or suitable status code.
    */
   telux::common::Status v2xServiceListSubscribe(
      uint32_t reqId, uint16_t port,
      std::shared_ptr<std::vector<uint32_t>> idList = nullptr,
      std::shared_ptr<IQmiCommandResponseCallback> callback = nullptr, void *userData = NULL);

   /**
    * Send an asynchronous WDS V2X Service List Subscribe request to unsubscribe
    *
    * @param [in] reqId          ID of this request
    * @param [in] idList         List of service Ids to subscribe to. Defaults to wildcard if nullptr
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendModifyProfileRequest i.e. success or suitable status code.
    */
   telux::common::Status v2xServiceListUnsubscribe(
      uint32_t reqId, std::shared_ptr<std::vector<uint32_t>> idList = nullptr,
      std::shared_ptr<IQmiCommandResponseCallback> callback = nullptr, void *userData = NULL);

   /**
    * Send an asynchronous WDS get V2X Service Subscription Info request
    *
    * @param [in] reqId          ID of this request
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendModifyProfileRequest i.e. success or suitable status code.
    */
   telux::common::Status requestV2xServiceSubscriptionInfo(
      uint32_t reqId,
      std::shared_ptr<IQmiCommandResponseCallback> callback = nullptr, void *userData = NULL);

   /**
    * Send an asynchronous WDS V2X SPS Flow Register request
    *
    * @param [in] reqId            ID of this request
    * @param [in] flow             SPS flow parameters
    * @param [in] nonSpsFlowValid  true if nonSpsFlow is valid
    * @param [in] nonSpsFlow       optional non-SPS flow
    * @param [in] callback         optional callback to get the asynchronous response for this request
    * @param [in] userData         Cookie user data value supplied by the client
    *
    * @returns Status of sendModifyProfileRequest i.e. success or suitable status code.
    */
   telux::common::Status v2xSpsFlowReg(
      wds_v2x_sps_flow_reg_req_msg_v01 & req,
      std::shared_ptr<IQmiCommandResponseCallback> callback = nullptr, void *userData = NULL);

   /**
    * Send an asynchronous WDS V2X SPS Flow Deregister request
    *
    * @param [in] reqId          ID of this request
    * @param [in] spsId          ID of SPS flow to deregister
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendModifyProfileRequest i.e. success or suitable status code.
    */
   telux::common::Status v2xSpsFlowDereg(
      uint32_t reqId, uint8_t spsId,
      std::shared_ptr<IQmiCommandResponseCallback> callback = nullptr, void *userData = NULL);

   /**
    * Send an asynchronous WDS SPS Flow Update request
    *
    * @param [in] reqId          ID of this request
    * @param [in] spsId          ID of SPS flow to update
   * @param [in] periodicity    New periodicity in ms
   * @param [in] msgSize        New message size in bytes
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendModifyProfileRequest i.e. success or suitable status code.
    */
   telux::common::Status v2xSpsFlowUpdate(
      wds_v2x_sps_flow_update_req_msg_v01 & req,
      std::shared_ptr<IQmiCommandResponseCallback> callback = nullptr, void *userData = NULL);

   /**
    * Send an asynchronous WDS get V2X SPS Flow Info request
    *
    * @param [in] reqId          ID of this request
    * @param [in] spsId          ID of SPS flow to update
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendModifyProfileRequest i.e. success or suitable status code.
    */
   telux::common::Status requestV2xSpsFlowInfo(
      uint32_t reqId, uint8_t spsId,
      std::shared_ptr<IQmiWdsV2xSpsFlowInfoCallback> callback = nullptr, void *userData = NULL);

   /**
    * Send an asynchronous WDS V2X get capability request
    *
    * @param [in] reqId          ID of this request
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendModifyProfileRequest i.e. success or suitable status code.
    */
    telux::common::Status requestV2xCapabilityInfo(
        std::shared_ptr<IQmiCommandResponseCallback> callback = nullptr,
        void *userData = NULL);

   /**
    * Send an asynchronous WDS V2X Non-SPS Flow Register request
    *
    * @param [in] reqId          ID of this request
    * @param [in] flows          List of Non-SPS flows to register
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendModifyProfileRequest i.e. success or suitable status code.
    */
   telux::common::Status v2xNonSpsFlowReg(
      wds_v2x_non_sps_flow_reg_req_msg_v01 & req,
      std::shared_ptr<IQmiCommandResponseCallback> callback = nullptr, void *userData = NULL);

   /**
    * Send an asynchronous WDS V2X Non-SPS FLow Deregister request
    *
    * @param [in] reqId          ID of this request
    * @param [in] flows          List of Non-SPS flows to deregister
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendModifyProfileRequest i.e. success or suitable status code.
    */
   telux::common::Status v2xNonSpsFlowDereg(
      wds_v2x_non_sps_flow_dereg_req_msg_v01 & req,
      std::shared_ptr<IQmiCommandResponseCallback> callback = nullptr, void *userData = NULL);

   /**
    * Send an asynchronous WDS V2X Send Config File request
    *
    * @param [in] reqId          ID of this request. Should be the
    *        same for all segments of the config file.
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendModifyProfileRequest i.e. success or suitable status code.
    */
   telux::common::Status v2xSendConfigFile(uint32_t reqId,
      uint32_t nsegments, uint16_t currentSegment, uint32_t nbytes, char * buf,
      std::shared_ptr<IQmiCommandResponseCallback> callback = nullptr, void *userData = NULL);

   /**
    * Send an asynchronous WDS V2X Get Config File request
    *
    * @param [in] reqId          ID of this request
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of v2xSendConfigFile i.e. success or suitable status code.
    */
   telux::common::Status v2xGetConfigFile(
     std::shared_ptr<telux::common::ICommandCallback> callback = nullptr,
     void *userData = nullptr);

   /**
    * Send an asynchronous WDS V2X Update Src L2 Info request
    *
    * @param [in] reqId          ID of this request
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of v2xUpdateSrcL2Info i.e. success or suitable status code.
    */
   telux::common::Status v2xUpdateSrcL2Info(
      uint32_t reqId,
      std::shared_ptr<IQmiCommandResponseCallback> callback = nullptr,
      void* userData = NULL);

   /**
    * Send an asynchronous WDS V2X Send Tunnel Mode Info request
    *
    * @param [in] req            Request message
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of v2xSendTunnelModeInfo i.e. success or suitable status code.
    */
   telux::common::Status v2xSendTunnelModeInfo(
      wds_v2x_tunnel_mode_info_req_msg_v01 & req,
      std::shared_ptr<IQmiCommandResponseCallback> callback = nullptr,
      void* userData = NULL);

   /**
    * Send an asynchronous WDS Runtime Settings request
    *
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of requestRuntimeSettings i.e. success or suitable status code.
    */
   telux::common::Status requestRuntimeSettings(
      std::shared_ptr<IQmiWdsRuntimeSettingsCallback> callback = nullptr,
      void* userData = NULL);

   /**
    * Register with QMI for CV2X config relevant indications from the WDS Subsystem
    */
   telux::common::Status registerV2xConfigIndication(
      std::shared_ptr<IQmiCommandResponseCallback> callback, void *userData = nullptr);

   /**
    * Send an asynchronous WDS get dormancy status to the Service
    *
    * @param [in] callback       callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendGetDormancyStatusRequest i.e. success or suitable status code.
    */
   telux::common::Status sendGetDormancyStatusRequest(
      std::shared_ptr<IQmiWdsGetDormancyStatusCallback> callback, void *userData = NULL);

   bool isReady() override;

   WdsQmiClient(SlotId slotId = DEFAULT_SLOT_ID);
   ~WdsQmiClient();
   // Prevent construction and copying of this class
   WdsQmiClient(WdsQmiClient const &) = delete;
   WdsQmiClient &operator=(WdsQmiClient const &) = delete;

private:
   SlotId slotId_;
   std::atomic<bool> isBound_ = {false};
   telux::common::AsyncTaskQueue<void> taskQ_;

   bool waitForInitialization() override;
   void initSync(void);
   void setBoundState(bool isBound);

   qmi_idl_service_object_type idlServiceObject_;
   qmi_client_os_params clientOsParams_;
   qmi_client_type qmiSvcClient_;

   // Map to store authentication type to wds enum type.
   std::map<int, wds_profile_auth_protocol_enum_v01> authTypeToWdsAuthMap_;

   // Map to store ip family type to wds enum type.
   std::map<int, wds_pdp_type_enum_v01> ipTypeToWdsPdpMap_;

   // Map to store profile type to wds enum type.
   std::map<int, wds_profile_type_enum_v01> profTypeToWdsProfMap_;

   // Initialize data members
   void initMap();

   bool createProfileReq(wds_create_profile_req_msg_v01 &req,
                         const QmiProfileParams &profileParams);

   void createProfileSettingsFromQmiResponse(
      wds_get_profile_settings_resp_msg_v01 *profileSettingsResp, int profileType,
      uint8_t profileIndex, ProfileSettingsResp &psResp);

   bool isInterfaceUp(std::string interface);

   bool waitForInterfaceUp(std::string interface,
                           unsigned int retrySleepTime,
                           unsigned int maxNumRetries);

   int getModemPort() override;

   /**
    * Private method that actually sends the V2X Service List
    * Subscribe method.
    *
    * @param [in] subscribe    true if subscribing. false if unsubscribing
    * @param [in] reqId        ID of this request
    * @param [in] port         Port number
    * @param [in] idList       List of service IDs to either subscribe or unsubscribe from.
    *                          Defaults to wildcard if nullptr
    * @param [in] callback     callback for this request
    * @param [in] userData     Cookie user data value supplied by the client
    */
   telux::common::Status v2xServiceListSub(
      bool subscribe, uint32_t reqId, uint16_t port,
      std::shared_ptr<std::vector<uint32_t>> idList,
      std::shared_ptr<telux::common::ICommandCallback> callback = nullptr, void *userData = NULL);

   /**
    * Handler method to process WDS indication register response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] transpErr            Transport error
    * @param [in] callback             callback for this request
    */
   void handleIndicationsRegResp(void *respCStruct, unsigned int respCStructLen, void *userData,
                                 qmi_client_error_type transpErr,
                                 std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process create profile response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleCreateProfileResp(void *respCStruct, unsigned int respCStructLen, void *userData,
                                qmi_client_error_type transpErr,
                                std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process delete profile response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleDeleteProfileResp(void *respCStruct, unsigned int respCStructLen, void *userData,
                                qmi_client_error_type transpErr,
                                std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process profile settings response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleGetProfileSettingsResp(void *respCStruct, unsigned int respCStructLen, void *userData,
                                     qmi_client_error_type transpErr,
                                     std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process profile list response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleGetProfileListResp(void *respCStruct, unsigned int respCStructLen, void *userData,
                                 qmi_client_error_type transpErr,
                                 std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process modify profile response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleModifyProfileResp(void *respCStruct, unsigned int respCStructLen, void *userData,
                                qmi_client_error_type transpErr,
                                std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process indication for event like profile change.
    *
    * @param [in] userHandle           Opaque handle used by the infrastructure to
    *                                  identify different services.
    * @param [in] indBuf               Buffer holding the encoded indication
    * @param [in] indBufLen            Length of the encoded indication
    *
    */
   void handleProfileEventRegisterIndication(qmi_client_type userHandle, void *indBuf,
                                             unsigned int indBufLen);

   /**
    * Handler method to process configuring of profiles for profile change indication
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] transpErr            QMI transport error
    * @param [in] callback             callback for this request
    */
   void handleConfigureProfilesResp(void *respCStruct, unsigned int respCStructLen, void *userData,
                                    qmi_client_error_type transpErr,
                                    std::weak_ptr<telux::common::ICommandCallback> callback);

   void notifyProfileChangeEvent(wds_profile_changed_ind_msg_v01 *indData);

   /**
    * Handler method to process Set Client IP-Family response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleSetClientIpFamilyResp(void *respCStruct,
                                    unsigned int respCStructLen, void *userData,
                                    qmi_client_error_type transpErr,
                                    std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process Bind Mux Data Port response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleBindMuxDataPortResp(void *respCStruct, unsigned int respCStructLen, void *userData,
                                  qmi_client_error_type transpErr,
                                  std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process Bind Subscription response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleBindSubscriptionResp(void *respCStruct, unsigned int respCStructLen, void *userData,
                                   qmi_client_error_type transpErr,
                                   std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process V2X Service List response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleV2xServiceListResp(void *respCStruct, unsigned int respCStructLen,
                                 void *userData,
                                 qmi_client_error_type transpErr,
                                 std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process get V2X Service Subscription Info response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleV2xServiceSubscriptionInfoResp(
      void *respCStruct, unsigned int respCStructLen,
      void *userData,
      qmi_client_error_type transpErr,
      std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process V2X SPS flow Register response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleV2xSpsFlowRegResp(void *respCStruct, unsigned int respCStructLen,
                                void *userData,
                                qmi_client_error_type transpErr,
                                std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process V2X SPS flow Deregister response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleV2xSpsFlowDeregResp(void *respCStruct, unsigned int respCStructLen,
                                  void *userData,
                                  qmi_client_error_type transpErr,
                                  std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process V2X SPS flow update response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleV2xSpsFlowUpdateResp(void *respCStruct, unsigned int respCStructLen,
                                   void *userData,
                                   qmi_client_error_type transpErr,
                                   std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process get V2X SPS flow info response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleV2xSpsFlowInfoResp(void *respCStruct, unsigned int respCStructLen,
                                 void *userData,
                                 qmi_client_error_type transpErr,
                                 std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process V2X Non-SPS flow register response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleV2xNonSpsFlowRegResp(void *respCStruct, unsigned int respCStructLen,
                                   void *userData,
                                   qmi_client_error_type transpErr,
                                   std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process V2X Non-SPS flow deregister response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleV2xNonSpsFlowDeregResp(void *respCStruct, unsigned int respCStructLen,
                                     void *userData,
                                     qmi_client_error_type transpErr,
                                     std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process V2X send config file response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleV2xSendConfigFileResp(void *respCStruct, unsigned int respCStructLen,
                                    void *userData,
                                    qmi_client_error_type transpErr,
                                    std::weak_ptr<telux::common::ICommandCallback> callback);
   /**
    * Handler method to process V2X get config file response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleV2xGetConfigFileResp(void *respCStruct, unsigned int respCStructLen,
                                   void *userData,
                                   qmi_client_error_type transpErr,
                                   std::weak_ptr<telux::common::ICommandCallback> callback);
   /**
    * Handler method to process V2X update SRC L2 info response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleV2xUpdateSrcL2InfoResp(void *respCStruct, unsigned int respCStructLen,
                                     void *userData,
                                     qmi_client_error_type transpErr,
                                     std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process V2X query capabilities response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
    void handleV2xCapabilityInfoResp(void *respCStruct, unsigned int respCStructLen,
                                     void *userData,
                                     qmi_client_error_type transpErr,
                                     std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process V2X Tunnel Mode info response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
    void handleV2xSendTunnelModeInfoResp(void *respCStruct, unsigned int respCStructLen,
                                         void *userData,
                                         qmi_client_error_type transpErr,
                                         std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process Get Runtime Settings response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
    void handleGetRuntimeSettingsResp(void *respCStruct, unsigned int respCStructLen,
                                      void *userData,
                                      qmi_client_error_type transpErr,
                                      std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process Get Dormancy Status response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
    void handleGetDormancyStatusResp(void *respCStruct, unsigned int respCStructLen,
                                     void *userData,
                                     qmi_client_error_type transpErr,
                                     std::weak_ptr<telux::common::ICommandCallback> callback);

   // QMI Indication handlers

   /**
    * Handler method to process QMI_WDS_PKT_SRVC_STATUS_IND_V01
    *
    * @param [in] userHandle        qmi client handle
    * @param [in] indBuf            pointer to buffer holding indication data
    * @param [in] indBufLen         number of bytes in indBuf
    */
   int handlePktSrvcStatusIndication(qmi_client_type userHandle, void *indBuf,
                                     unsigned int indBufLen);

   /**
    * Handler method to process QMI_WDS_V2X_SPS_FLOW_REG_RESULT_IND_V01
    *
    * @param [in] userHandle        qmi client handle
    * @param [in] indBuf            pointer to buffer holding indication data
    * @param [in] indBufLen         number of bytes in indBuf
    */
   int handleV2xSpsFlowRegResultIndication(qmi_client_type userHandle, void *indBuf,
                                           unsigned int indBufLen);

   /**
    * Handler method to process QMI_WDS_V2X_SPS_FLOW_DEREG_RESULT_IND_V01 indication
    *
    * @param [in] userHandle        qmi client handle
    * @param [in] indBuf            pointer to buffer holding indication data
    * @param [in] indBufLen         number of bytes in indBuf
    */
   int handleV2xSpsFlowDeregResultIndication(qmi_client_type userHandle, void *indBuf,
                                              unsigned int indBufLen);

   /**
    * Handler method to process QMI_WDS_V2X_SPS_FLOW_UPDATE_RESULT_IND_V01 indication
    *
    * @param [in] userHandle        qmi client handle
    * @param [in] indBuf            pointer to buffer holding indication data
    * @param [in] indBufLen         number of bytes in indBuf
    */
   int handleV2xSpsFlowUpdateResultIndication(qmi_client_type userHandle, void *indBuf,
                                              unsigned int indBufLen);

   /**
    * Handler method to process QMI_WDS_V2X_SERVICE_SUBSCRIBE_RESULT_IND_V01 indication
    *
    * @param [in] userHandle        qmi client handle
    * @param [in] indBuf            pointer to buffer holding indication data
    * @param [in] indBufLen         number of bytes in indBuf
    */
   int handleV2xServiceSubscribeResultIndication(qmi_client_type userHandle, void *indBuf,
                                                 unsigned int indBufLen);

   /**
    * Handler method to process QMI_WDS_V2X_SPS_SCHEDULING_INFO_IND_V01 indication
    *
    * @param [in] userHandle        qmi client handle
    * @param [in] indBuf            pointer to buffer holding indication data
    * @param [in] indBufLen         number of bytes in indBuf
    */
   int handleV2xSpsSchedulingInfoIndication(qmi_client_type userHandle, void *indBuf,
                                            unsigned int indBufLen);

   /**
    * Handler method to process QMI_WDS_V2X_SEND_CONFIG_FILE_RESULT_IND_V01 indication
    *
    * @param [in] userHandle        qmi client handle
    * @param [in] indBuf            pointer to buffer holding indication data
    * @param [in] indBufLen         number of bytes in indBuf
    */
   int handleV2xSendConfigFileResultIndication(qmi_client_type userHandle, void *indBuf,
                                               unsigned int indBufLen);
   /**
    * Handler method to process QMI_WDS_V2X_CONFIG_FILE_IND_V01 indication
    *
    * @param [in] userHandle        qmi client handle
    * @param [in] indBuf            pointer to buffer holding indication data
    * @param [in] indBufLen         number of bytes in indBuf
    */
   int handleV2xConfigFileIndication(qmi_client_type userHandle, void *indBuf,
                                     unsigned int indBufLen);
   /**
    * Handler method to process QMI_WDS_V2X_CONFIG_CHANGED_IND_V01
    *
    * @param [in] userHandle        qmi client handle
    * @param [in] indBuf            pointer to buffer holding indication data
    * @param [in] indBufLen         number of bytes in indBuf
    */
   int handleV2xConfigChangedIndication(qmi_client_type userHandle, void *indBuf,
                                        unsigned int indBufLen);
   /**
    * Handler method to process QMI_WDS_V2X_SRC_L2_INFO_IND_V01 indication
    *
    * @param [in] userHandle        qmi client handle
    * @param [in] indBuf            pointer to buffer holding indication data
    * @param [in] indBufLen         number of bytes in indBuf
    */
   int handleV2xSrcL2InfoIndication(qmi_client_type userHandle, void *indBuf,
                                    unsigned int indBufLen);

    /**
     * Handler method to process QMI_WDS_V2X_CAPABILITY_INFO_IND_V01 indication
     *
     * @param [in] userHandle        qmi client handle
     * @param [in] indBuf            pointer to buffer holding indication data
     * @param [in] indBufLen         number of bytes in indBuf
     */
    int handleV2xCapabilityInfoIndication(qmi_client_type userHandle, void *indBuf,
                                          unsigned int indBufLen);

   /**
    * Handler method to process QMI_WDS_GLOBAL_DORMANCY_STATUS_IND_V01
    *
    * @param [in] userHandle        qmi client handle
    * @param [in] indBuf            pointer to buffer holding indication data
    * @param [in] indBufLen         number of bytes in indBuf
    */
   int handleDormancyStatusChangedIndication(qmi_client_type userHandle, void *indBuf,
                                             unsigned int indBufLen);

   // Listener Notification methods

   /**
    * Notifies all listeners that are listening for QMI_WDS_PKT_SRVC_STATUS_IND_V01
    * indication
    *
    * @param [in] ind    Indication data
    */
   void notifyPktSrvcStatusInd(
      const wds_pkt_srvc_status_ind_msg_v01 & ind);

   /**
    * Notifies all listeners that are listening for QMI_WDS_V2X_SPS_FLOW_REG_RESULT_IND_V01
    * indication
    *
    * @param [in] ind    Indication data
    */
   void notifyV2xSpsFlowRegResultInd(
      const wds_v2x_sps_flow_reg_result_ind_msg_v01 & ind);

   /**
    * Notifies all listeners that are listening for QMI_WDS_V2X_SPS_FLOW_DEREG_RESULT_IND_V01
    * indication
    *
    * @param [in] ind    Indication data
    */
   void notifyV2xSpsFlowDeregResultInd(const wds_v2x_sps_flow_dereg_result_ind_msg_v01 & ind);

   /**
    * Notifies all listeners that are listening for QMI_WDS_V2X_SPS_FLOW_UPDATE_RESULT_IND_V01
    * indication
    *
    * @param [in] ind    Indication data
    */
   void notifyV2xSpsFlowUpdateResultInd(
      const wds_v2x_sps_flow_update_result_ind_msg_v01 & ind);

   /**
    * Notifies all listeners that are listening for QMI_WDS_V2X_SERVICE_SUBSCRIBE_RESULT_IND_V01
    * indication
    *
    * @param [in] ind    Indication data
    */
   void notifyV2xServiceSubscribeResultInd(
      const wds_v2x_service_subscribe_result_ind_msg_v01 & ind);

   /**
    * Notifies all listeners that are listening for QMI_WDS_V2X_SPS_SCHEDULING_INFO_IND_V01
    * indication
    *
    * @param [in] ind    Indication data
    */
   void notifyV2xSpsSchedulingInfoInd(
      const wds_v2x_sps_scheduling_info_ind_msg_v01 & ind);

   /**
    * Notifies all listeners that are listening for QMI_WDS_V2X_SEND_CONFIG_FILE_RESULT_IND_V01
    * indication
    *
    * @param [in] ind    Indication data
    */
   void notifyV2xSendConfigFileInd(
      const wds_v2x_send_config_file_result_ind_msg_v01 & ind);

   /**
    * Notifies all listeners that are listening for QMI_WDS_V2X_CONFIG_FILE_IND_V01
    * indication
    *
    * @param [in] ind    Indication data
    */
   void notifyV2xConfigInd(
      const wds_v2x_config_file_ind_msg_v01 & ind);


    /**
     * Notifies all listeners that are listening for QMI_WDS_V2X_CONFIG_CHANGED_IND_V01 indication
     *
     * @param [in] ind    Indication data
     */
    void notifyV2xConfigChangeInd(
        const wds_v2x_config_changed_ind_msg_v01& ind);

   /**
    * Notifies all listeners that are listening for QMI_WDS_V2X_SRC_L2_INFO_IND_V01 indication
    *
    * @param [in] ind    Indication data
    */
   void notifyV2xSrcL2InfoInd(
      const wds_v2x_src_l2_info_ind_msg_v01 & ind);

    /**
     * Notifies all listeners that are listening for QMI_WDS_V2X_SRC_L2_INFO_IND_V01 indication
     *
     * @param [in] ind    Indication data
     */
    void notifyV2xCapabilityInfoInd(
        const wds_v2x_capability_info_ind_msg_v01& ind);

    /**
     * Notifies all listeners that are listening for QMI_WDS_GLOBAL_DORMANCY_STATUS_IND_V01
     * indication
     *
     * @param [in] ind    Indication data
     */
    void notifyDormancyStatusChangeInd(
        const wds_global_dormancy_status_ind_msg_v01& ind);

}; // class WdsQmiClient


/**
 * @brief General command response callback for most of the requests, client needs to implement
 * this interface to get single shot response.
 *
 * The methods in callback can be invoked from multiple different threads. The implementation
 * should be thread safe.
 */
class IQmiCommandResponseCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] qmiError
    * @param [out] dsExtendedError  DS Profile extended error
    * @param [out] userData         Cookie user data value supplied by the client
    */
   virtual void commandResponse(int qmiError, int dsExtendedError, void *userData) = 0;

   virtual ~IQmiCommandResponseCallback() {}
};

class IQmiWdsCreateProfileCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] qmiError         Qmi error
    * @param [out] profileIndex     Profile identifier
    * @param [out] dsExtendedError  DS Profile extended error
    * @param [out] userData         Cookie user data value supplied by the client
    */
   virtual void createProfileResponse(int qmiError, int profileIndex, int dsExtendedError,
                                      void *userData)
      = 0;

   virtual ~IQmiWdsCreateProfileCallback() {}
};

class IQmiWdsGetProfileSettingsCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] qmiError               Qmi error
    * @param [out] dsExtendedError        DS Profile extended error
    * @param [out] profileSettingsResp    profile response
    * @param [out] userData               Cookie user data value supplied by the client
    */
   virtual void profileSettingsResponse(int qmiError, int dsExtendedError,
                                        const ProfileSettingsResp &profileSettingsResp,
                                        void *userData)
      = 0;

   virtual ~IQmiWdsGetProfileSettingsCallback() {}
};

class IQmiWdsGetProfileListCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] qmiError               Qmi error
    * @param [out] dsExtendedError        DS Profile extended error
    * @param [out] profileList            List of profile ids present in the device
    * @param [out] userData               Cookie user data value supplied by the client
    */
   virtual void profileListResponse(int qmiError, int dsExtendedError,
                                    const std::vector<ProfileListInfo> &profileList, void *userData)
      = 0;

   virtual ~IQmiWdsGetProfileListCallback() {}
};

class IQmiWdsV2xSpsFlowInfoCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] qmiError               Qmi error
    * @param [out] dsExtendedError        DS Profile extended error
    * @param [out] reservation            Reservation info
    * @param [out] userData               Cookie user data value supplied by the client
    */
   virtual void v2xSpsInfoResponse(int qmiError, int dsExtendedError,
                                   const wds_v2x_sps_flow_get_info_resp_msg_v01 & resp,
                                   void *userData) = 0;

   virtual ~IQmiWdsV2xSpsFlowInfoCallback() {}
};

class IQmiWdsV2xCapabilityInfoCallback : public telux::common::ICommandCallback {
public:
    /**
     * This function is called with the response to the command operation.
     *
     * @param [out] qmiError               Qmi error
     * @param [out] dsExtendedError        DS Profile extended error
     * @param [out] resp                   Capability info
     * @param [out] userData               Cookie user data value supplied by the client
     */
    virtual void v2xCapabilityInfoResponse(int qmiError, int dsExtendedError,
                                           const wds_v2x_get_capability_info_resp_msg_v01& resp,
                                           void *userData) = 0;

    virtual ~IQmiWdsV2xCapabilityInfoCallback(void) { }
};

class IQmiWdsRuntimeSettingsCallback : public telux::common::ICommandCallback
{
public:
    /**
     * This function is called with the response to the command operation.
     *
     * @param [out] qmiError               Qmi error
     * @param [out] dsExtendedError        DS Profile extended error
     * @param [out] reservation            Reservation info
     * @param [out] userData               Cookie user data value supplied by the client
     */
    virtual void v2xRuntimeSettingsResponse(int qmiError, int dsExtendedError,
        const wds_get_runtime_settings_resp_msg_v01& resp,
        void *userData) = 0;

    virtual ~IQmiWdsRuntimeSettingsCallback() { }
};

class IQmiWdsV2xConfigRetrievalCallback : public telux::common::ICommandCallback {
public:
    /**
     * This function is called with the response to the command operation.
     *
     * @param [out] qmiError               Qmi error
     * @param [out] dsExtendedError        DS Profile extended error
     * @param [out] resp                   V2X Configuration Retrieval Response
     * @param [out] userData               Cookie user data value supplied by the client
     */
    virtual void v2xConfigResponse(int qmiError, int dsExtendedError,
                                   const wds_v2x_get_config_file_resp_msg_v01& resp,
                                   void *userData) = 0;

    virtual ~IQmiWdsV2xConfigRetrievalCallback() { }
};

class IQmiWdsGetDormancyStatusCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] qmiError               Qmi error
    * @param [out] dsExtendedError        DS Profile extended error
    * @param [out] profileSettingsResp    profile response
    * @param [out] userData               Cookie user data value supplied by the client
    */
   virtual void dormancyStatusResponse(int qmiError, int dsExtendedError,
                                       const wds_get_dormancy_status_resp_msg_v01 &resp,
                                       void *userData)
      = 0;

   virtual ~IQmiWdsGetDormancyStatusCallback() {}
};

/**
 * This is the listener class. Clients that want to be notified
 * of WDS indications must implement this interface and register
 * as a listener to a WdsQmiClient
 */
class IQmiWdsListener : public IQmiListener {
public:
   virtual void onPktSrvcStatusInd(
      const wds_pkt_srvc_status_ind_msg_v01 & ind) {}

   virtual void onV2xSpsFlowRegResultInd(
      const wds_v2x_sps_flow_reg_result_ind_msg_v01 & ind) {}

   virtual void onV2xSpsFlowDeregResultInd(
      const wds_v2x_sps_flow_dereg_result_ind_msg_v01 & ind) {}

   virtual void onV2xSpsFlowUpdateResultInd(
      const wds_v2x_sps_flow_update_result_ind_msg_v01 & ind) {}

   virtual void onV2xServiceSubscribeResultInd(
      const wds_v2x_service_subscribe_result_ind_msg_v01 & ind) {}

   virtual void onV2xSpsSchedulingInfoInd(
      const wds_v2x_sps_scheduling_info_ind_msg_v01 & ind) {}

   virtual void onV2xSendConfigFileInd(
      const wds_v2x_send_config_file_result_ind_msg_v01 & ind) {}

   virtual void onV2xGetConfigurationInd(
      const wds_v2x_config_file_ind_msg_v01 & ind) {}

   virtual void onV2xConfigChangeInd(
       const wds_v2x_config_changed_ind_msg_v01 & ind) {}

   virtual void onV2xSrcL2InfoInd(
      const wds_v2x_src_l2_info_ind_msg_v01 & ind) {}

   virtual void onV2xCapabilityInfoInd(
      const wds_v2x_capability_info_ind_msg_v01& ind) {}

   virtual void onDormancyStatusChangeInd(
      const wds_global_dormancy_status_ind_msg_v01& ind) {}

   virtual void onProfileUpdate(wds_profile_changed_ind_msg_v01 *indData) {}

   virtual void onWdsServiceStatusChange(telux::common::ServiceStatus status) {}

   virtual ~IQmiWdsListener() {}
};


}  // end namespace qmi
}  // end namespace telux

#endif  // end of WDSQMICLIENT_HPP
