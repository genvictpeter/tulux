/*
 *  Copyright (c) 2019 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       UimRemoteQmiClient.hpp
 * @brief      QMI Client interacts with QMI user identity module remote service to send requests
 *             and dispatch received indications to respective listeners
 */

#ifndef UIMREMOTEQMICLIENT_HPP
#define UIMREMOTEQMICLIENT_HPP

#include "QmiClient.hpp"

extern "C" {
#include <qmi/user_identity_module_remote_v01.h>
}

/**
 * Structure to send the user data for each QMI request along with slot details.
 * The same userdata will come back in response which helps in identifying
 * response came corresponding to which slot.
 */
struct UimRemoteQmiUserData {
   void *data;
   int slotId = DEFAULT_SLOT_ID;
};

namespace telux {
namespace qmi {

class IQmiUimRemoteListener;

/**
 * This function is called with the response to sendResetRequest API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [out] resp         QMI UIM remote reset response message
 * @param [out] qmiError     QMI error
 * @param [out] userData     Cookie user data supplied by the client
 */
using UimRemoteResetCb = std::function<void(uim_remote_reset_resp_msg_v01 *resp, int qmiError,
                                            void *userData)>;

/**
 * This function is called with the response to sendEventRequest API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [out] resp         QMI UIM remote event response message
 * @param [out] qmiError     QMI error
 * @param [out] userData     Cookie user data supplied by the client
 */
using UimRemoteEventCb = std::function<void(uim_remote_event_resp_msg_v01 *resp, int qmiError,
                                            void *userData)>;

/**
 * This function is called with the response to sendApduRequest API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [out] resp         QMI UIM remote APDU response message
 * @param [out] qmiError     QMI error
 * @param [out] userData     Cookie user data supplied by the client
 */
using UimRemoteApduCb = std::function<void(uim_remote_apdu_resp_msg_v01 *resp, int qmiError,
                                           void *userData)>;

/**
 * This class provides mechanism to send messages to QMI user identity module remote
 */
class UimRemoteQmiClient : public QmiClient {
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
    * Send UIM Remote request to reset the service state variables
    * of the requesting control point
    */
    telux::common::Status sendResetRequest(UimRemoteResetCb callback, void *userData);

   /**
    * Send UIM Remote Event request to notify the service of remote
    * UIM events
    */
    telux::common::Status sendEventRequest(uim_remote_event_req_msg_v01 &eventReq,
                                           UimRemoteEventCb callback, void *userData);

   /**
    * Send UIM Remote APDU request to exchange the APDU with the
    * remote card
    */
    telux::common::Status sendApduRequest(uim_remote_apdu_req_msg_v01 &apduReq,
                                          UimRemoteApduCb callback, void *userData);

    UimRemoteQmiClient();
    ~UimRemoteQmiClient();

private:
    // deleting copy constructor, to implement UimQmiRemoteClient as singleton
    UimRemoteQmiClient(const UimRemoteQmiClient &) = delete;
    // deleting assigning operator, to implement UimQmiRemoteClient as singleton
    UimRemoteQmiClient &operator=(const UimRemoteQmiClient &) = delete;

    // Handler methods for responses to asynchronous requests
    void handleResetResp(void *respCStruct, uint32_t respCStructLen, void *data,
                         qmi_client_error_type transpErr,
                         std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleEventResp(void *respCStruct, uint32_t respCStructLen, void *data,
                         qmi_client_error_type transpErr,
                         std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleApduResp(void *respCStruct, uint32_t respCStructLen, void *data,
                        qmi_client_error_type transpErr,
                        std::weak_ptr<telux::common::ICommandCallback> callback);

    // Handler methods for indications
    void handleApduInd(qmi_client_type userHandle, void *indBuf, unsigned int indBufLen);

    void handleConnectInd(qmi_client_type userHandle, void *indBuf, unsigned int indBufLen);

    void handleDisconnectInd(qmi_client_type userHandle, void *indBuf, unsigned int indBufLen);

    void handleCardPowerUpInd(qmi_client_type userHandle, void *indBuf, unsigned int indBufLen);

    void handleCardPowerDownInd(qmi_client_type userHandle, void *indBuf, unsigned int indBufLen);

    void handleCardResetInd(qmi_client_type userHandle, void *indBuf, unsigned int indBufLen);

    // Notify methods for indications
    void notifyApduTransfer(uim_remote_apdu_ind_msg_v01 *indData);

    void notifyCardConnect(uim_remote_connect_ind_msg_v01 *indData);

    void notifyCardDisconnect(uim_remote_disconnect_ind_msg_v01 *indData);

    void notifyCardPowerUp(uim_remote_card_power_up_ind_msg_v01 *indData);

    void notifyCardPowerDown(uim_remote_card_power_down_ind_msg_v01 *indData);

    void notifyCardReset(uim_remote_card_reset_ind_msg_v01 *indData);

    // Handles callback execution
    telux::common::CommandCallbackManager cmdCallbackMgr_;
};

/**
 * Listener interface for unsolicited notifications. Clients should implement
 * this interface and register with UimRemoteQmiClient in order to receive
 * unsolicited indications.
 */
class IQmiUimRemoteListener : public IQmiListener {
public:
    virtual void onApduTransferNotification(uim_remote_apdu_ind_msg_v01 *indData){}
    virtual void onCardConnectNotification(uim_remote_connect_ind_msg_v01 *indData){}
    virtual void onCardDisconnectNotification(uim_remote_disconnect_ind_msg_v01 *indData){}
    virtual void onCardPowerUpNotification(uim_remote_card_power_up_ind_msg_v01 *indData){}
    virtual void onCardPowerDownNotification(uim_remote_card_power_down_ind_msg_v01 *indData){}
    virtual void onCardResetNotification(uim_remote_card_reset_ind_msg_v01 *indData){}
    virtual void onRemoteSimServiceStatusChange(telux::common::ServiceStatus status){}
    virtual ~IQmiUimRemoteListener(){}
};

}  // end namespace qmi
}  // end namespace telux

#endif  // UIMREMOTEQMICLIENT_HPP
