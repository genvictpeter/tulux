/*
 *  Copyright (c) 2018-2021, Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       NasQmiClient.hpp
 * @brief      QMI Client interacts with QMI network access service to send/receive
 *             QMI requests/ indications and dispatch to respective listeners
 */

#ifndef NASQMICLIENT_HPP
#define NASQMICLIENT_HPP

extern "C" {
#include <qmi/network_access_service_v01.h>
}

#include <bitset>

#include "QmiClient.hpp"

namespace telux {
namespace qmi {

// Forward declarations
class IQmiNasRegisterForIndsCallback;
class IQmiSetSystemSelectionPreferenceCallback;
class IQmiRegisterNasIndicationsCallback;
class IQmiGetPreferredNetworksCallback;
class IQmiSetPreferredNetworksCallback;
class IQmiPerformNetworkScanCallback;
class IQmiNasListener;
class IQmiV2xStartCallback;
class IQmiV2xStopCallback;
class IQmiV2xStatusCallback;
class IQmiSetPeakTxPowerCb;

/**
 * Network selection mode structure
 */
struct QmiNetworkSelectionConfig {
   int networkSelectionMode; /**< Network selection mode */
   uint16_t mcc;             /**< Mobile Country Code */
   uint16_t mnc;             /**< Mobile Network Code */
};

/**
 * Preferred network structure
 */
struct QmiPreferredNetwork {
   uint16_t mcc;             /**< Mobile Country Code */
   uint16_t mnc;             /**< Mobile Network Code */
   uint16_t radioTechnology; /**< Radio Access Technology */
};

/**
 * Network selection preference operation
 */
enum NetworkOperation {
   SET_SELECTION_MODE = 0x00, /**< Flag to set network selection mode */
   SET_SERVICE_DOMAIN = 0x01, /**< Flag to set service domain preference */
};

/**
 * Defines NAS QMI service indications being used
 */
enum NasIndicationType {
   SYSTEM_SELECTION_PREF,      /**< QMI_NAS_SYSTEM_SELECTION_PREFERENCE_IND_MSG_V01 */
   SYSTEM_INFO,                /**< QMI_NAS_SYS_INFO_IND_MSG_V01 */
   HIGH_CAPABILITY,            /**< QMI_NAS_HIGH_CAPABILITY_SUB_IND_MSG_V01 */
   EMERGENCY_NW_SCAN_FAIL,     /**< QMI_NAS_E911_SCAN_FAIL_IND_V01 */
   V2X_STATUS,                 /**< QMI_NAS_V2X_NOTIFICATION_IND_V01 */
   TX_STATUS_REPORT            /**< QMI_NAS_V2X_TX_STATUS_REPORT_IND_V01 */
};

/**
 * 16 bit mask that denotes which of the NAS QMI service indications defined in NasIndicationType
 * are used to enable the indications.
 */
using NasIndicationMask = std::bitset<16>;

/**
 * This function is called with the response to sendGetSystemSelectionPreferenceRequest API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [out] resp         QMI NAS selection mode response message
 * @param [out] userData     Cookie user data value supplied by the client
 * @param [out] transpErr    Transport error
 *
 * @note   Eval: This is a new API and is being evaluated. It is subject to
 *         change and could break backwards compatibility.
 */
using GetSystemSelectionPreferenceCb = std::function<void(
   nas_get_system_selection_preference_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendGetSystemInfoRequest API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [out] resp         QMI NAS get-sys-info response message
 * @param [out] userData     Cookie user data value supplied by the client
 * @param [out] transpErr    Transport error
 *
 * @note   Eval: This is a new API and is being evaluated. It is subject to
 *         change and could break backwards compatibility.
 */
using GetSystemInfoCb = std::function<void(
   nas_get_sys_info_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendGetHighCapabilitySubRequest API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [out] resp         QMI NAS get high capability response message
 * @param [out] userData     Cookie user data value supplied by the client
 * @param [out] transpErr    Transport error
 *
 * @note   Eval: This is a new API and is being evaluated. It is subject to
 *         change and could break backwards compatibility.
 */
using GetHighCapabilityCb = std::function<void(
   nas_get_high_capability_sub_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendSetHighCapabilitySubRequest API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [out] qmiError     QMI error
 * @param [out] userData     Cookie user data value supplied by the client
 * @param [out] transpErr    Transport error
 *
 * @note   Eval: This is a new API and is being evaluated. It is subject to
 *         change and could break backwards compatibility.
 */
using SetHighCapabilityCb = std::function<void(int qmiError, void *userData, int transpErr)>;

/**
 * This class provides mechanism to send messages to QMI network access Subsystem
 */
class NasQmiClient : public QmiClient {
public:
   /**
    * This function is called by the QmiClient::qmiIndicationCallback when an indication is
    * received by QCCI infrastructure.
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
    * This function is called by the QmiClient::qmiAsyncResponseHandler when an asynchronous
    * response is received by QCCI infrastructure.
    *
    * NOTE: Callback happens in the QCCI thread context and raises signal.
    *
    * @param [in] msgId                Message ID of the indication
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] transpErr            QMI error
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
    * Registers for QMI Indications supported by NasQmiClient API.
    *
    * @param [in] callback       Command callback pointer
    * @param [in] userData       Cookie user data value supplied by the client
    */
   telux::common::Status registerForIndications(
      std::shared_ptr<IQmiNasRegisterForIndsCallback> callback = nullptr, void *userData = NULL);

   /**
    * Get network selection mode and service domain preference
    *
    * @param [in] callback        Command Callback pointer
    * @param [in] userData        Cookie user data value supplied by the client
    *
    * @returns Status of sendGetSystemSelectionPreferenceRequest i.e. success or suitable error
    * code.
    */
   telux::common::Status sendGetSystemSelectionPreferenceRequest(
      GetSystemSelectionPreferenceCb callback, void *userData = nullptr);

   /**
    * Set network selection mode
    *
    * @param [in] selectConfig    @ref QmiNetworkSelectionConfig
    * @param [in] callback        Command Callback pointer
    * @param [in] userData        Cookie user data value supplied by the client
    *
    * @returns Status of sendSetNetworkSelectionModeRequest i.e. success or suitable error code.
    */
   telux::common::Status sendSetNetworkSelectionModeRequest(
      QmiNetworkSelectionConfig &selectConfig,
      std::shared_ptr<IQmiSetSystemSelectionPreferenceCallback> callback, void *userData = nullptr);

   /**
    * Get preferred network list for 3GPP preferred networks and static 3GPP preferred
    * networks
    *
    * @param [in] callback        Command Callback pointer
    * @param [in] userData        Cookie user data value supplied by the client
    *
    * @returns Status of sendGetPreferredNetworksRequest i.e. success or suitable error code.
    */
   telux::common::Status sendGetPreferredNetworksRequest(
      std::shared_ptr<IQmiGetPreferredNetworksCallback> callback, void *userData = nullptr);

   /**
    * Set 3GPP preferred networks list
    *
    * @param [in] preferredNetworks3gpp      list of preferred network ( @ref QmiPreferredNetwork )
    * @param [in] clearPrevPreferredNetworks If flag is false new network list is appended to
    *                                        existing list. If flag is true existing list will be
    *                                        flushed and new list get added
    * @param [in] callback                   Command Callback pointer
    * @param [in] userData                   Cookie user data value supplied by the client
    *
    * @returns Status of sendSetPreferredNetworksRequest i.e. success or suitable error code.
    */
   telux::common::Status sendSetPreferredNetworksRequest(
      std::vector<QmiPreferredNetwork> &preferredNetworks3gpp, bool clearPrevPreferredNetworks,
      std::shared_ptr<IQmiSetPreferredNetworksCallback> callback, void *userData = nullptr);

   /**
    * Set service domain preference
    *
    * @param [in] serviceDomainPreference  service domain preference
    * @param [in] callback                 Command Callback pointer
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of sendSetServiceDomainPreference i.e. success or suitable error code.
    */
   telux::common::Status sendSetServiceDomainPreference(
      int &serviceDomainPreference,
      std::shared_ptr<IQmiSetSystemSelectionPreferenceCallback> callback, void *userData = nullptr);

   /**
    * Set radio access technology mode preference
    *
    * @param [in] callback                 Command Callback pointer
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of setModePref i.e. success or suitable error code.
    */
   telux::common::Status
      setModePref(int ratPref, std::shared_ptr<IQmiSetSystemSelectionPreferenceCallback> callback,
                  void *userData = nullptr);

   /**
    * Get system information
    *
    * @param [in] callback        Command Callback pointer
    * @param [in] userData        Cookie user data value supplied by the client
    *
    * @returns Status of sendGetSystemInfoRequest i.e. success or suitable error
    * code.
    */
   telux::common::Status sendGetSystemInfoRequest(
      GetSystemInfoCb callback, void *userData = nullptr);

   /**
    * Perform scan and get available networks
    *
    * @param [in] callback                 Command Callback pointer
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of networkScanReq i.e. success or suitable error code.
    */
   telux::common::Status networkScanReq(std::shared_ptr<IQmiPerformNetworkScanCallback> callback,
                                        void *userData = nullptr);
   /**
    * Send incremental network scan request and get available networks through
    * QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_IND_MSG_V01 indication.
    *
    * @param [in] networkScanReq           Indicates the network scan request.
    * @param [in] callback                 Command Callback pointer
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of sendIncrementalNetworkScanReq i.e. success or suitable error code.
    */
   telux::common::Status sendIncrementalNetworkScanReq(
      nas_perform_incremental_network_scan_req_msg_v01 networkScanReq,
      std::shared_ptr<IQmiPerformNetworkScanCallback> callback, void *userData = nullptr);

   /**
    * Register for a specific set of QMI indications supported by NasQmiClient API
    *
    * @param [in] indMask                  Set of indications to be enabled
    *                                      @ref NasIndicationMask
    * @param [in] callback                 Command Callback pointer
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of registerForNasIndications i.e. success or suitable error code.
    */
   telux::common::Status registerForNasIndications(NasIndicationMask indMask,
      std::shared_ptr<IQmiRegisterNasIndicationsCallback> callback, void *userData = nullptr);

   /**
    * Start V2X mode
    *
    * @param [in] callback                 Command Callback pointer
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of cv2xStart i.e. success or suitable error
    *        code.
    */
   telux::common::Status v2xStart(std::shared_ptr<IQmiV2xStartCallback> callback,
                                  void *userData = nullptr);

   /**
    * Stop V2X mode
    *
    * @param [in] callback                 Command Callback pointer
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of cv2xStop i.e. success or suitable error
    *        code.
    */
   telux::common::Status v2xStop(std::shared_ptr<IQmiV2xStopCallback> callback,
                                 void *userData = nullptr);

   /**
    * Send request for V2X Status
    *
    * @param [in] callback                 Command Callback pointer
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of cv2xStop i.e. success or suitable error
    *        code.
    */
   telux::common::Status requestV2xStatus(std::shared_ptr<IQmiV2xStatusCallback> callback,
                                          void *userData = nullptr);
   /**
    * Send request to set high capability.
    *
    * @param [in] slotId                   Slot ID
    * @param [in] callback                 Command Callback pointer
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of highcapabilitySubRequest i.e. success or suitable error code.
    */
   telux::common::Status sendSetHighCapabilitySubRequest(int slotId,
      SetHighCapabilityCb callback, void *userData = nullptr);

   /**
    * Send request to get high capability.
    *
    * @param [in] callback                 Command Callback pointer
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of highcapabilitySubRequest i.e. success or suitable error code.
    */
   telux::common::Status sendGetHighCapabilitySubRequest(GetHighCapabilityCb callback,
      void *userData = nullptr);

   /**
    * Set modem peak cv2x transmit power
    *
    * @param [in] txPower                desired cv2x peak tx power
    * @param [in] callback                 Command Callback pointer
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of setPeakTxPower i.e. success or suitable error
    *        code.
    */
   telux::common::Status setPeakTxPower(int8_t txPower,
       std::shared_ptr<IQmiSetPeakTxPowerCb> callback, void *userData = nullptr);

   /**
    * Exit emergency call back mode
    * @param [in] ecbMode                  Indicates to exit from emergency callback mode
    *                                      0 - Exit from ECBM
    * @param [in] callback                 Command Callback pointer
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of sendExitEcbm i.e. success or suitable error code.
    */
   telux::common::Status sendExitEcbm(int ecbMode,
       std::shared_ptr<IQmiSetSystemSelectionPreferenceCallback> callback,
           void *userData = nullptr);

   /**
    * Deregister for a specific set of QMI indications supported by NasQmiClient API
    *
    * @param [in] indMask                  Set of indications to be disabled
    *                                      @ref NasIndicationMask
    * @param [in] callback                 Command Callback pointer
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of deregisterForNasIndications i.e. success or suitable error code.
    */
   telux::common::Status deregisterForNasIndications(NasIndicationMask indMask,
      std::shared_ptr<IQmiRegisterNasIndicationsCallback> callback, void *userData = nullptr);

   bool isReady() override;
   telux::common::Status init(qmi_idl_service_object_type idlServiceObject);

   NasQmiClient(int slotId = DEFAULT_SLOT_ID);
   ~NasQmiClient();

private:
   int slotId_;
   std::atomic<bool> isBound_ = {false};
   telux::common::AsyncTaskQueue<void> taskQ_;

   // deleting copy constructor, to implement NasQmiClient as singleton
   NasQmiClient(const NasQmiClient &) = delete;
   // deleting assigning operator , to implement NasQmiClient as singleton
   NasQmiClient &operator=(const NasQmiClient &) = delete;

   bool waitForInitialization() override;
   void initSync(void);
   void setBoundState(bool isBound);

   void notifySystemSelectionPreference(nas_system_selection_preference_ind_msg_v01 *indData);
   void notifyIncrementalNetworkScan(nas_perform_incremental_network_scan_ind_msg_v01 *indData);
   /**
    * Binds client to the subscription of the SIM in a specific slot
    *
    * @param [in] slotId                   Slot ID
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of bindSubscriptionRequest i.e. success or suitable error code.
    */
   telux::common::Status bindSubscriptionRequest(int slotId, void *userData = nullptr);

   // Handler methods for responses to asynchronous requests
   void handleIndicationsRegResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                 qmi_client_error_type transpErr,
                                 std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleGetSystemSelectionPreferenceResp(
      void *respCStruct, uint32_t respCStructLen, void *data, qmi_client_error_type transpErr,
      std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleGetPreferredNetworksResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                       qmi_client_error_type transpErr,
                                       std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleSetPreferredNetworksResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                       qmi_client_error_type transpErr,
                                       std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleSetSystemSelectionPreferenceResp(
      void *respCStruct, uint32_t respCStructLen, void *data, qmi_client_error_type transpErr,
      std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleNetworkScanResp(void *respCStruct, uint32_t respCStructLen, void *data,
                              qmi_client_error_type transpErr,
                              std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleIncrementalNetworkScanResp(void *respCStruct, uint32_t respCStructLen, void *data,
      qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleGetSystemInfoResp(void *respCStruct, uint32_t respCStructLen, void *data,
      qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleRegisterForIndResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                       qmi_client_error_type transpErr,
                                       std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleBindSubscriptionResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                   qmi_client_error_type transpErr,
                                   std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleSelectionModeInd(qmi_client_type userHandle, void *indBuf, unsigned int indBufLen);

   void notifySystemInfo(nas_sys_info_ind_msg_v01 *indData);

   void handleSystemInfoInd(qmi_client_type userHandle, void *indBuf, unsigned int indBufLen);

   void handleV2xStartResp(void *respCStruct, uint32_t respCStructLen, void *data,
                           qmi_client_error_type transpErr,
                           std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleV2xStopResp(void *respCStruct, uint32_t respCStructLen, void *data,
                          qmi_client_error_type transpErr,
                          std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleV2xStatusResp(void *respCStruct, uint32_t respCStructLen, void *data,
                            qmi_client_error_type transpErr,
                            std::weak_ptr<telux::common::ICommandCallback> callback);

   // Indication handlers
   int handleV2xStatusIndication(qmi_client_type userHandle, void *indBuf, unsigned int indBufLen);

   void handleHighCapabilityInd(qmi_client_type userHandle, void *indBuf, unsigned int indBufLen);

   void notifyHighCapability(nas_high_capability_sub_ind_msg_v01 *indData);

   void handleGetHighCapabilityResp(void *respCStruct, uint32_t respCStructLen, void *data,
      qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleSetHighCapabilityResp(void *respCStruct, uint32_t respCStructLen, void *data,
      qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);

   void notifyV2xStatusIndication(const nas_v2x_notification_ind_v01 &indData);

   void handlePeakTxPowerResp(void *respCStruct, uint32_t respCStructLen, void *data,
                              qmi_client_error_type transpErr,
                              std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleIncrementalNetworkScanInd(qmi_client_type userHandle, void *indBuf,
      unsigned int indBufLen);

   telux::common::CommandCallbackManager cmdCallbackMgr_;

   void notifyEmergencyNetworkScanFail();

   int handleV2xTxStatusReportIndication(qmi_client_type userHandle, void *indBuf,
                                         unsigned int indBufLen);

   void notifyV2xTxStatusReportIndication(const nas_v2x_tx_status_report_ind_msg_v01 &indData);
};

/**
 * Listener interface for unsolicited notifications. Clients should implement
 * this interface and register with NasQmiClient in order to receive
 * unsolicited notifications/ indications for selection mode change.
 */
class IQmiNasListener : public IQmiListener {
public:
   /**
    * This function is called whenever there is a change in system selection preference
    * like service domain preference, selection mode and RAT mode preference.
    *
    * @param [in] indData - System selection preference indication
    */
   virtual void
      onSystemSelectionPreferenceChanged(nas_system_selection_preference_ind_msg_v01 *indData) {
   }

   /**
    * This function is called whenever there is a change in system information
    * like ENDC availability, DCNR restriction status,etc.
    *
    * @param [in] indData - System selection preference indication
    */
   virtual void onSystemInfoChanged(nas_sys_info_ind_msg_v01 *indData) {
   }

   virtual void onV2xNotificationInd(const nas_v2x_notification_ind_v01 &ind) {
   }

   virtual void onNasServiceStatusChange(telux::common::ServiceStatus status) {
   }

   virtual void onHighCapabilityChanged(nas_high_capability_sub_ind_msg_v01 *indData) {
   }

   virtual void onIncrementalNetworkScanInd(
      nas_perform_incremental_network_scan_ind_msg_v01 *indData) {
   }

   virtual void onEmergencyNetworkScanFail(SlotId slotId) {
   }

   virtual void onTxStatusReport(
      const nas_v2x_tx_status_report_ind_msg_v01 & indData) {
   }

   virtual ~IQmiNasListener() {
   }
};

/**
 * Callback class for register Indications request
 */
class IQmiNasRegisterForIndsCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [in] resp                           QMI NAS selection mode response message
    * @param [in] userData                       Cookie user data value supplied by the client
    */
   virtual void onResponse(int qmiError, void *userData) = 0;

   virtual ~IQmiNasRegisterForIndsCallback() {
   }
};

/**
 * Callback class for set network selection mode and service domain preference response.
 */
class IQmiSetSystemSelectionPreferenceCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] qmiError               QMI error
    * @param [out] userData               Cookie user data value supplied by the client
    * @param [out] transpErr              Transport error
    */
   virtual void onResponse(int qmiError, void *userData, int transpErr) = 0;
};

/**
 * Callback class for request preferred networks response.
 */
class IQmiGetPreferredNetworksCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] resp                      QMI NAS get preferred networks response message
    * @param [out] userData                  Cookie user data value supplied by the client
    * @param [out] transpErr                 Transport error
    */
   virtual void onResponse(nas_get_preferred_networks_resp_msg_v01 *resp, void *userData,
                           int transpErr)
      = 0;
};

/**
 * Callback class for set preferred networks response.
 */
class IQmiSetPreferredNetworksCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] qmiError               QMI error
    * @param [out] userData               Cookie user data value supplied by the client
    * @param [out] transpErr              Transport error
    */
   virtual void onResponse(int qmiError, void *userData, int transpErr) = 0;
};

/**
 * Callback class for perform network scan response.
 */
class IQmiPerformNetworkScanCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] resp                   QMI NAS perform network scan response
    * @param [out] userData               Cookie user data value supplied by the client
    * @param [out] transpErr              Transport error
    */
   virtual void onResponse(nas_perform_network_scan_resp_msg_v01 *resp, void *userData,
                           int transpErr)
      = 0;

   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] qmiError               QMI error
    * @param [out] userData               Cookie user data value supplied by the client
    * @param [out] transpErr              Transport error
    */
   virtual void onResponse(int qmiError, void *userData, int transpErr) = 0;
};

/**
 * Callback class for register for specific nas indications response.
 */
class IQmiRegisterNasIndicationsCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] qmiError               QMI error
    * @param [out] transpErr              Transport error
    */
   virtual void onIndRegisterResponse(int qmiError, int transpErr) = 0;
};

/**
 * Callback class for send v2x start response.
 */
class IQmiV2xStartCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [in] qmiError               QMI error
    * @param [in] userData               Cookie user data value supplied by the client
    */
   virtual void onResponse(int qmiError, void *userData) = 0;

   virtual ~IQmiV2xStartCallback() {
   }
};

/**
 * Callback class for send v2x stop response.
 */
class IQmiV2xStopCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [in] qmiError               QMI error
    * @param [in] userData               Cookie user data value supplied by the client
    */
   virtual void onResponse(int qmiError, void *userData) = 0;

   virtual ~IQmiV2xStopCallback() {
   }
};

/**
 * Callback class for get v2x status response.
 */
class IQmiV2xStatusCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [in] qmiError               QMI error
    * @param [in] userData               Cookie user data value supplied by the client
    */
   virtual void onResponse(nas_get_v2x_status_resp_msg_v01 *resp, void *userData) = 0;

   virtual ~IQmiV2xStatusCallback() {
   }
};

/**
 * Callback class for set v2x peak TX power response.
 */
class IQmiSetPeakTxPowerCb : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [in] qmiError               QMI error
    * @param [in] userData               Cookie user data value supplied by the client
    */
   virtual void onResponse(int qmiError, void *userData) = 0;

   virtual ~IQmiSetPeakTxPowerCb() {
   }
};


}  // end namespace qmi
}  // end namespace telux

#endif  // NASQMICLIENT_HPP
