/*
 *  Copyright (c) 2021, Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       ImsSettingsQmiClient.hpp
 * @brief      QMI Client interacts with QMI ip multimedia sub system settings to send/receive
 *             QMI requests/indications and dispatch to respective listeners
 */

#ifndef IMSSETTINGSQMICLIENT_HPP
#define IMSSETTINGSQMICLIENT_HPP

extern "C" {
#include <qmi/ip_multimedia_subsystem_settings_v01.h>
}

#include <bitset>

#include "QmiClient.hpp"

namespace telux {
namespace qmi {

// Forward declarations
class IQmiRegisterImsSettingsIndicationsCallback;
class IQmiImsSettingsListener;


/**
 * Defines IMS service indications being used
 */
enum ImsSettingsIndicationType {
   IMS_SERVICE_CONFIG,         /**< QMI_IMS_SETTINGS_IMS_SERVICE_ENABLE_CONFIG_IND_V01 */
};

/**
 * 16 bit mask that denotes which of the IMS settings service indications defined in
 * ImsSettingsIndicationType are used to enable the indications.
 */
using ImsSettingsIndicationMask = std::bitset<16>;

/**
 * This function is called with the response to sendSetServiceConfig API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [in] slotId        Unique identifier for the SIM slot
 * @param [out] resp         Set IMS service configurations response message
 * @param [out] userData     Cookie user data value supplied by the client
 * @param [out] transpErr    Transport error
 *
 * @note   Eval: This is a new API and is being evaluated. It is subject to
 *         change and could break backwards compatibility.
 */
using SetImsServiceConfigCb = std::function<void(SlotId slotId,
   ims_settings_set_ims_service_enable_config_rsp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendGetServiceConfig API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [in] slotId        Unique identifier for the SIM slot
 * @param [out] resp         Get IMS service configurations response message
 * @param [out] userData     Cookie user data value supplied by the client
 * @param [out] transpErr    Transport error
 *
 * @note   Eval: This is a new API and is being evaluated. It is subject to
 *         change and could break backwards compatibility.
 */
using GetImsServiceConfigCb = std::function<void(SlotId slotId,
   ims_settings_get_ims_service_enable_config_rsp_msg_v01 *resp, void *userData, int transpErr)>;


/**
 * This class provides mechanism to send messages to QMI IMS settings Subsystem
 */
class ImsSettingsQmiClient : public QmiClient {
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
    * Send request to get IMS service configuration parameters
    *
    * @param [in] slotId                   Unique identifier for the SIM slot
    * @param [in] callback                 Callback to get respone for get service config request
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of sendGetServiceConfig i.e. success or suitable error code.
    */
   telux::common::Status sendGetServiceConfig(SlotId slotId,
       GetImsServiceConfigCb callback, void *userData);

   /**
    * Send request to either enable or disable the IMS service configurations
    *
    * @param [in] slotId                   Unique identifier for the SIM slot
    * @param [in] config                   Config indicates which IMS service configurations are
    *                                      configured
    * @param [in] callback                 Callback to get respone for set service config request
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of sendSetServiceConfig i.e. success or suitable error code.
    */
   telux::common::Status sendSetServiceConfig(SlotId slotId,
       ims_settings_set_ims_service_enable_config_req_msg_v01 config,
           SetImsServiceConfigCb callback = nullptr, void *userData = nullptr);

   /**
    * Register for a specific set of QMI indications supported by ImsSettingsQmiClient API
    *
    * @param [in] indMask                  Set of indications to be enabled
    *                                      @ref ImsSettingsIndicationMask
    * @param [in] callback                 Command Callback pointer
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of registerForImsSettingsIndications i.e. success or suitable error code.
    */
   telux::common::Status registerForImsSettingsIndications(ImsSettingsIndicationMask indMask,
      std::shared_ptr<IQmiRegisterImsSettingsIndicationsCallback> callback,
      void *userData = nullptr);

   bool isReady() override;

   ImsSettingsQmiClient(SlotId slotId = SlotId::DEFAULT_SLOT_ID);
   ~ImsSettingsQmiClient();

private:
   SlotId slotId_;
   std::atomic<bool> isBound_ = {false};
   telux::common::AsyncTaskQueue<void> taskQ_;

   // Deleting copy constructor, to implement ImsSettingsQmiClient as singleton
   ImsSettingsQmiClient(const ImsSettingsQmiClient &) = delete;
   // Deleting assigning operator , to implement ImsSettingsQmiClient as singleton
   ImsSettingsQmiClient &operator=(const ImsSettingsQmiClient &) = delete;

   bool waitForInitialization() override;
   void initSync(void);
   void setBoundState(bool isBound);

   /**
    * Binds client to the subscription of the SIM in a specific slot
    *
    * @param [in] slotId                   Unique identifier for the SIM slot
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of bindSubscriptionRequest i.e. success or suitable error code.
    */
   telux::common::Status bindSubscriptionRequest(int slotId, void *userData = nullptr);

   // Handler methods for responses to asynchronous requests
   void handleIndicationsRegResp(void *respCStruct, uint32_t respCStructLen, void *data,
       qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleRegisterForIndResp(void *respCStruct, uint32_t respCStructLen, void *data,
       qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleBindSubscriptionResp(void *respCStruct, uint32_t respCStructLen, void *data,
       qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleGetServiceConfigResp(void *respCStruct, uint32_t respCStructLen, void *data,
       qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleSetServiceConfigResp(void *respCStruct, uint32_t respCStructLen, void *data,
       qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);

   // Indication handlers
   void handleImsServiceConfigInd(qmi_client_type userHandle, void *indBuf,
       unsigned int indBufLen);
   void notifyImsServiceConfigChange(
       ims_settings_ims_service_enable_config_ind_msg_v01 *indData);

   telux::common::CommandCallbackManager cmdCallbackMgr_;
};

/**
 * Listener interface for unsolicited notifications. Clients should implement
 * this interface and register with ImsSettingsQmiClient in order to receive
 * unsolicited notifications/ indications for Ims service enable config params change.
 */
class IQmiImsSettingsListener : public IQmiListener {
public:

   /**
    * This function is called whenever any IMS service configuration changes.
    *
    * @param [in] ind                            Indication data has information of all valid TLV(s)
    *                                            and whether the config is enabled or disabled
    * @param [in] slotId                         Unique identifier for the SIM slot
    */
   virtual  void onImsServiceConfigChange(
      ims_settings_ims_service_enable_config_ind_msg_v01 *ind, SlotId slotId) {
   }

   virtual  void onImsServiceStatusChange(telux::common::ServiceStatus status) {
   }

   virtual ~IQmiImsSettingsListener() {
   }
};

/**
 * Callback class for register Indications request
 */
class IQmiRegisterImsSettingsIndicationsCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] qmiError               QMI error
    * @param [out] transpErr              Transport error
    */
   virtual void onIndRegisterResponse(int qmiError, int transpErr) = 0;
};

}  // end namespace qmi
}  // end namespace telux

#endif  // IMSSETTINGSQMICLIENT_HPP
