/*
 *  Copyright (c) 2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       DsdQmiClient.hpp
 *
 * @brief      Data System Determination Client is a singleton class to register for
 *             indication and request modem status.
 *             It contains structure definitions and function prototype for DSD QMI Client,
 *             this class provides mechanism to send QMI messages to DSD QMI Service and get
 *             the results. It then send the events back to the caller.
 *
 */

#ifndef DSDQMICLIENT_HPP
#define DSDQMICLIENT_HPP

#include <map>
#include <memory>
#include <vector>

extern "C" {
#include <qmi/data_system_determination_v01.h>
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
class IQmiDsdCommandResponseCallback;
class IQmiDsdGetRoamingStatusCallback;
class IQmiDsdServiceStatusResultsCallback;
class IQmiDsdListener;


/**
 * This class provides mechanism to send messages to QMI DSD Subsystem
 */

class DsdQmiClient : public QmiClient {
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
    * Registers for QMI DSD Roaming Indications supported by DsdQmiClient.
    */
   telux::common::Status registerForRoamingIndications(
      std::shared_ptr<IQmiDsdCommandResponseCallback> callback, void *userData = nullptr);

   /**
    * Deregister from QMI DSD Roaming Indications supported by DsdQmiClient.
    */
   telux::common::Status deregisterRoamingIndications(
      std::shared_ptr<IQmiDsdCommandResponseCallback> callback, void *userData = nullptr);

   /**
    * Registers for QMI Service Status Change Indications supported by DsdQmiClient.
    */
   telux::common::Status registerServiceStatusIndications(
      std::shared_ptr<IQmiDsdCommandResponseCallback> callback, void *userData = nullptr);

   /**
    * Deregister from QMI Service Status Change Indications supported by DsdQmiClient.
    */
   telux::common::Status deregisterServiceStatusIndications(
      std::shared_ptr<IQmiDsdCommandResponseCallback> callback, void *userData = nullptr);

   /**
    * Send an asynchronous DSD Bind subscription request
    *
    * @param [in] subType        Subscription type to bind to
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of bindSubscription i.e. success or suitable status code.
    */
   telux::common::Status bindSubscription(dsd_bind_subscription_enum_v01 subType,
      std::shared_ptr<IQmiDsdCommandResponseCallback> callback = nullptr, void *userData = NULL);

   /**
    * Send an asynchronous DSD Set System Capability request
    *
    * @param [in] capVersion     System Capability Version to set to
    * @param [in] callback       optional callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of setSysCapabilityReq i.e. success or suitable status code.
    */
   telux::common::Status setSysCapabilityReq(
      dsd_capability_system_status_version_enum_v01 capVersion,
      std::shared_ptr<IQmiDsdCommandResponseCallback> callback = nullptr, void *userData = NULL);

   /**
    * Send an asynchronous DSD get service status to the Service
    *
    * @param [in] callback       callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendGetServiceStatusRequest i.e. success or suitable status code.
    */
   telux::common::Status sendGetServiceStatusRequest(
      std::shared_ptr<IQmiDsdCommandResponseCallback> callback, void *userData = NULL);

   /**
    * Send an asynchronous DSD get roaming status to the Service
    *
    * @param [in] callback       callback to get the asynchronous response for this request
    * @param [in] userData       Cookie user data value supplied by the client
    *
    * @returns Status of sendGetRoamingStatusRequest i.e. success or suitable status code.
    */
   telux::common::Status sendGetRoamingStatusRequest(
      std::shared_ptr<IQmiDsdGetRoamingStatusCallback> callback, void *userData = NULL);

   bool isReady() override;

   DsdQmiClient(SlotId slotId = DEFAULT_SLOT_ID);
   ~DsdQmiClient();
   // Prevent construction and copying of this class
   DsdQmiClient(DsdQmiClient const &) = delete;
   DsdQmiClient &operator=(DsdQmiClient const &) = delete;

private:
   SlotId slotId_;
   std::atomic<bool> isAvail_ = {false};
   telux::common::AsyncTaskQueue<void> taskQ_;

   bool waitForInitialization() override;
   void initSync(void);
   void setAvailState(bool isAvail);

   qmi_idl_service_object_type idlServiceObject_;

   /**
    * Handler method to process DSD register indications response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] transpErr            Transport error
    * @param [in] callback             callback for this request
    */
   void handleIndicationsRegResp(void *respCStruct, unsigned int respCStructLen,
                                 void *userData, qmi_client_error_type transpErr,
                                 std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process DSD register service status indication response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] transpErr            Transport error
    * @param [in] callback             callback for this request
    */
   void handleServiceStatusIndRegResp(void *respCStruct, unsigned int respCStructLen,
                                      void *userData, qmi_client_error_type transpErr,
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
    * Handler method to process Set System Capability Response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
   void handleSetSysCapabilityResp(void *respCStruct, unsigned int respCStructLen, void *userData,
                                   qmi_client_error_type transpErr,
                                   std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process Get service Status response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
    void handleGetServiceStatusResp(void *respCStruct, unsigned int respCStructLen,
                                  void *userData,
                                  qmi_client_error_type transpErr,
                                  std::weak_ptr<telux::common::ICommandCallback> callback);

   /**
    * Handler method to process Get Roaming Status response
    *
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] callback             callback for this request
    */
    void handleGetRoamingStatusResp(void *respCStruct, unsigned int respCStructLen,
                                    void *userData,
                                    qmi_client_error_type transpErr,
                                    std::weak_ptr<telux::common::ICommandCallback> callback);

   // QMI Indication handlers

   /**
    * Handler method to process QMI_DSD_SYSTEM_STATUS_V2_IND_V01
    *
    * @param [in] userHandle        qmi client handle
    * @param [in] indBuf            pointer to buffer holding indication data
    * @param [in] indBufLen         number of bytes in indBuf
    */
   int handleServiceStatusChangedIndication(qmi_client_type userHandle, void *indBuf,
                                            unsigned int indBufLen);

   /**
    * Handler method to process QMI_DSD_GET_SYSTEM_STATUS_V2_RESULT_IND_V01
    *
    * @param [in] userHandle        qmi client handle
    * @param [in] indBuf            pointer to buffer holding indication data
    * @param [in] indBufLen         number of bytes in indBuf
    */
    int handleGetServiceStatusResultInd(qmi_client_type userHandle, void *indBuf,
                                         unsigned int indBufLen);

   /**
    * Handler method to process QMI_DSD_ROAMING_STATUS_CHANGE_IND_V01
    *
    * @param [in] userHandle        qmi client handle
    * @param [in] indBuf            pointer to buffer holding indication data
    * @param [in] indBufLen         number of bytes in indBuf
    */
   int handleRoamingStatusChangedIndication(qmi_client_type userHandle, void *indBuf,
                                             unsigned int indBufLen);

   // Listener Notification methods

    /**
     * Notifies all listeners that are listening for QMI_DSD_SYSTEM_STATUS_V2_IND_V01
     * indication
     *
     * @param [in] ind    Indication data
     */
    void notifyServiceStatusChangeInd(const dsd_system_status_v2_ind_msg_v01& ind);

    /**
     * Notifies all listeners that are listening for QMI_DSD_GET_SYSTEM_STATUS_V2_RESULT_IND_V01
     * indication
     *
     * @param [in] ind    Indication data
     */
    void notifyServiceStatusResultsInd(const dsd_get_system_status_v2_result_ind_msg_v01& ind,
                                       int qmiErr);

    /**
     * Notifies all listeners that are listening for QMI_DSD_ROAMING_STATUS_CHANGE_IND_V01
     * indication
     *
     * @param [in] ind    Indication data
     */
    void notifyRoamingStatusChangeInd(const dsd_roaming_status_change_ind_msg_v01& ind);

}; // class DsdQmiClient


/**
 * @brief General command response callback for most of the requests, client needs to implement
 * this interface to get single shot response.
 *
 * The methods in callback can be invoked from multiple different threads. The implementation
 * should be thread safe.
 */
class IQmiDsdCommandResponseCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] qmiError
    * @param [out] dsExtendedError  DS Profile extended error
    * @param [out] userData         Cookie user data value supplied by the client
    */
   virtual void commandResponse(int qmiError, int dsExtendedError, void *userData) = 0;

   virtual ~IQmiDsdCommandResponseCallback() {}
};

class IQmiDsdGetRoamingStatusCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] qmiError               Qmi error
    * @param [out] dsExtendedError        DS Profile extended error
    * @param [out] resp                   roaming status response
    * @param [out] userData               Cookie user data value supplied by the client
    */
   virtual void roamingStatusResponse(int qmiError, int dsExtendedError,
                                      const dsd_get_current_roaming_status_info_resp_msg_v01 &resp,
                                      void *userData) = 0;

   virtual ~IQmiDsdGetRoamingStatusCallback() {}
};

/**
 * This is the listener class. Clients that want to be notified
 * of DSD indications must implement this interface and register
 * as a listener to a DsdQmiClient
 */
class IQmiDsdListener : public IQmiListener {
public:
   virtual void onServiceStateChangeInd(
      const dsd_system_status_v2_ind_msg_v01& ind) {}

   virtual void onServiceStatusResultInd(
      const dsd_get_system_status_v2_result_ind_msg_v01& ind, int qmiErr) {}

   virtual void onRoamingStatusChangeInd(
      const dsd_roaming_status_change_ind_msg_v01& ind) {}

   virtual void onDsdServiceStatusChange(telux::common::ServiceStatus status) {}

   virtual ~IQmiDsdListener() {}
};


}  // end namespace qmi
}  // end namespace telux

#endif  // end of DSDQMICLIENT_HPP
