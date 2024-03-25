/*
 *  Copyright (c) 2018-2021, Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       DmsQmiClient.hpp
 * @brief      QMI Client interacts with QMI device management service to send requests
 *             and dispatch received indications to respective listeners
 */

#ifndef DMSQMICLIENT_HPP
#define DMSQMICLIENT_HPP

#include <bitset>

extern "C" {
#include <qmi/device_management_service_v01.h>
}
#include "QmiClient.hpp"

namespace telux {
namespace qmi {

// enum declarations

/* Defines possible indication types provided/accepted by DMS service*/
enum DmsIndicationType {
    PSM_STATUS, /* Bitmask to specify if Power Save Mode Status needs to be reported */
    PSM_CFG_CHANGE, /* Bitmask to specify if Power Save Mode Configuration changes
                        needs to be reported */
    IMS_CAPABILITY, /* Bitmask to specify if IMS capability needs to be reported */
    SENSOR_STATUS, /* Bitmask to specify if sensor status needs to be reported */
    PWR_CFG_REQUESTS, /* Bitmask to specify if Power Configurations are requested */
    INTERACTIVE_STATE_REQUESTS, /* Bitmask to specify if Device Interactive State indications are
                                    requested */
    DEVICE_ACTIVITY_STATE_REQUESTS, /* Bitmask to specify if device activity state indications
                                        are requested. */
    INDICATION_TYPE_COUNT  /**< Bitset Max count */
};

using DmsIndication = std::bitset<INDICATION_TYPE_COUNT>;

// Forward declarations
class IQmiCellularCapabilityCallback;
class IQmiSetEventReportCallback;
class IQmiSetOperatingModeCallback;
class IQmiGetOperatingModeCallback;
class IQmiMsisdnRequestCallback;
class IQmiRegisterDmsIndicationCallback;
class IQmiDeviceActivityStateCallback;
class IQmiDmsListener;

/**
 * This class provides mechanism to send messages to QMI device management Subsystem and
 * receive indications for operating mode from QMI.
 */
class DmsQmiClient : public QmiClient {
public:
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
    * This function is called by qmiClientErrorCallbackSync when the QMI service
    * terminates or deregisters.
    *
    * NOTE: Callback happens on new thread created by std::async.
    *
    * @param [in] clientError           Error value
    */
   void errorHandler(qmi_client_error_type clientError);

   /**
    * This function is called by qmiClientNotifierCallbackSync when a QMI service
    * event occurs indicating that the service count has changed.
    *
    * NOTE: Callback happens on new thread created by std::async.
    *
    * @param [in]   serviceEvent       Event type
    */
   void notifierHandler(qmi_client_notify_event_type serviceEvent);

   /**
    * Sends QMI_DMS_GET_DEVICE_CAP_REQ to get cellular capabilities.
    *
    * @param [in] callback        Command Callback pointer
    * @param [in] userData        Cookie user data value supplied by the client
    *
    * @returns Status of sendCellularCapabilityRequest i.e. success or suitable error code.
    */
   telux::common::Status sendCellularCapabilityRequest(
      std::shared_ptr<IQmiCellularCapabilityCallback> callback, void *userData = nullptr);

   /**
    * Query the current operating mode of the device
    *
    * @param [in] callback        Command Callback pointer
    * @param [in] userData        Cookie user data value supplied by the client
    *
    * @returns Status of sendOperatingModeRequest i.e. success or suitable error code.
    */
   telux::common::Status sendOperatingModeRequest(
      std::shared_ptr<IQmiGetOperatingModeCallback> callback, void *userData = nullptr);

   /**
    * Set the operating mode of the device
    *
    * @param [in] callback        Command Callback pointer
    * @param [in] userData        Cookie user data value supplied by the client
    *
    * @returns Status of sendSetOperatingModeRequest i.e. success or suitable error code.
    */
   telux::common::Status
      sendSetOperatingModeRequest(std::shared_ptr<IQmiSetOperatingModeCallback> callback,
                                  int operatingMode, void *userData = nullptr);

   /**
    * Sets the device management state reporting conditions for operating mode
    *
    * @param [in] enableOptMode   enable operating mode true or false
    * @param [in] callback        Command Callback pointer
    * @param [in] userData        Cookie user data value supplied by the client
    *
    * @returns Status of sendSetEventReport i.e. success or suitable error code.
    */
   telux::common::Status sendSetEventReport(bool enableOptMode,
                                            std::shared_ptr<IQmiSetEventReportCallback> callback,
                                            void *userData = nullptr);

   /**
    * Sends QMI_DMS_GET_MSISDN_REQ
    * Queries for assigned voice number and IMSI
    * @param handler derived class object that implements the ResponseHandler for handling callback
    */
   telux::common::Status sendDmsMsisdnRequest(std::shared_ptr<IQmiMsisdnRequestCallback> callback,
                                              void *userData = nullptr);

   /**
    * Sends QMI_DMS_INDICATION_REGISTER_REQ to register for indications
    *
    * @param [in] DmsIndication     Set of Indication types to be sent/received from DMS
    * @param [in] callback          Command Callback pointer
    * @param [in] userData          Cookie user data value supplied by the client
    */
   telux::common::Status registerDmsIndications(DmsIndication indType,
            std::shared_ptr<IQmiRegisterDmsIndicationCallback> callback, void *userData = nullptr);

   /**
    * Sends QMI_DMS_DEVICE_ACTIVITY_STATE_V01 to send current device activity state to modem
    *
    * @param [in] activityState     Device activity state
    * @param [in] callback          Command Callback pointer
    * @param [in] userData          Optional cookie user data value supplied by the client
    */
   telux::common::Status sendDeviceActivityStateRequest(dms_activity_state_type_v01 activityState,
            std::shared_ptr<IQmiDeviceActivityStateCallback> callback, void *userData = nullptr);

   bool isReady() override;
   telux::common::Status init(qmi_idl_service_object_type idlServiceObject);

   DmsQmiClient(int slotId = DEFAULT_SLOT_ID);
   ~DmsQmiClient();

private:
   int slotId_;
   std::atomic<bool> isBound_ = {false};
   telux::common::AsyncTaskQueue<void> taskQ_;

   void notifyOperatingModeChanged(dms_operating_mode_enum_v01 indData);
   void notifyModemClientReadinessChanged(uint8_t reportDeviceActivityState);
   // deleting copy constructor, to implement DmsQmiClient as singleton
   DmsQmiClient(const DmsQmiClient &) = delete;
   // deleting assigning operator , to implement DmsQmiClient as singleton
   DmsQmiClient &operator=(const DmsQmiClient &) = delete;

   bool waitForInitialization() override;
   void initSync(void);
   void setBoundState(bool isBound);
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
   void handleCellularCapabilityResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                     qmi_client_error_type transpErr,
                                     std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleOperatingModeResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                qmi_client_error_type transpErr,
                                std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleSetOperatingModeResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                   qmi_client_error_type transpErr,
                                   std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleSetEventReportResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                 qmi_client_error_type transpErr,
                                 std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleDmsMsisdnRequestResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                   qmi_client_error_type transpErr,
                                   std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleDmsBindSubscriptionResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                      qmi_client_error_type transpErr,
                                      std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleRegisterDmsIndicationResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                        qmi_client_error_type transpErr,
                                        std::weak_ptr<telux::common::ICommandCallback> callback);

   void handleSendDeviceActivityStateResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                          qmi_client_error_type transpErr,
                                          std::weak_ptr<telux::common::ICommandCallback> callback);

   // Handler methods for indications
   int handleSetEventReportIndication(qmi_client_type userHandle, void *indBuf,
                                      unsigned int indBufLen);

   int handleDeviceActivityStateIndication(qmi_client_type userHandle, void *indBuf,
                                           unsigned int indBufLen);
};

/**
 * Callback class for requesting cellular capabilities response.
 */
class IQmiCellularCapabilityCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] dmsGetDeviceCapRespMsg   Response
    * @param [out] qmiError                 QMI error
    * @param [out] userData                 Cookie user data value supplied by the client
    */
   virtual void getCellularCapabilityResponse(
      dms_get_device_cap_resp_msg_v01 *dmsGetDeviceCapRespMsg, int qmiError, void *userData)
      = 0;
};

/**
 * Callback class for requesting operating mode response.
 */
class IQmiGetOperatingModeCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] operatingMode          Operating Mode
    * @param [out] qmiError               QMI error
    * @param [out] userData               Cookie user data value supplied by the client
    */
   virtual void getOperatingModeResponse(int operatingMode, int qmiError, void *userData) = 0;
};

/**
 * Callback class to set operating mode response.
 */
class IQmiSetOperatingModeCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] qmiError               QMI error
    * @param [out] userData               Cookie user data value supplied by the client
    */
   virtual void setOperatingModeResponse(int qmiError, void *userData) = 0;
};

/**
 * Callback class to set operating mode event report response.
 */
class IQmiSetEventReportCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] errorCode              QMI error
    * @param [out] userData               Cookie user data value supplied by the client
    */
   virtual void setEventReportResponse(int qmiError, void *userData) = 0;
};

/**
 * Callback class for requesting assigned voice number and IMSI response.
 */
class IQmiMsisdnRequestCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] respMsg                Get Msisdn response message
    * @param [out] userData               Cookie user data value supplied by the client
    */
   virtual void onDmsMsisdnRequestResp(dms_get_msisdn_resp_msg_v01 *respMsg, void *userData) = 0;
};

/**
 * Callback class for registerDmsIndications's response.
 */
class IQmiRegisterDmsIndicationCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] errorCode              QMI error
    * @param [out] userData               Cookie user data value supplied by the client
    */
   virtual void onDmsIndicationRegisterResponse(int qmiError, void *userData) = 0;
};

/**
 * Callback class for sendDeviceActivityStateRequest's response.
 */
class IQmiDeviceActivityStateCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] errorCode              QMI error
    * @param [out] userData               Cookie user data value supplied by the client
    */
   virtual void onSendDeviceActivityStateResponse(int qmiError, void *userData) = 0;
};

/**
 * Listener interface for QMI indications. Clients should implement
 * this interface and register with DmsQmiClient in order to receive
 * indications for Operating mode changes.
 */
class IQmiDmsListener : public IQmiListener {
public:
   /**
    * This function is called when indication for change in operating mode is received.
    *
    * @param [out] operatingMode              Operating Mode
    */
   virtual void onOperatingModeChange(int operatingMode) {}

   /**
    * This function is called when the Modem activity client readiness changes.
    *
    * @param [out] report                     Consent to report device activity state.
    */
   virtual void onModemActivityClientReady(bool report) {}

   /**
    * This function will be called whenever the QMI DMS service status changes
    *
    * @param [in] status         service status
    */
   virtual void onDmsServiceStatusChange(telux::common::ServiceStatus status, int slotId) {}
};

}  // end namespace qmi
}  // end namespace telux

#endif  // DMSQMICLIENT_HPP
