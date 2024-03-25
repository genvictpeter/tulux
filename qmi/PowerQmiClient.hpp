/*
 *  Copyright (c) 2019-2020, Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       PowerQmiClient.hpp
 * @brief      Power QMI Client interacts with power-management QMI service to send/receive QMI
 *             requests/ indications and dispatch to respective listeners
 *
 */

#ifndef POWERQMICLIENT_HPP
#define POWERQMICLIENT_HPP

extern "C" {
#include "power_manager_service_v01.h"
}

#include <vector>

#include "QmiClient.hpp"

namespace telux {
namespace qmi {

// Forward declarations
class IQmiRegisterTcuStateEventCallback;
class IQmiTcuStateAckCallback;
class IQmiTcuStateCommandCallback;
class IQmiTcuActivityListener;

/**
 * This class provides mechanism to send messages to power-management QMI Service and receive
 * indications for TCU-activity state updates.
 */
class PowerQmiClient : public QmiClient {
public:
    /**
     * This function is called by the QmiClient::qmiIndicationCallback infrastructure when it
     * receives an indication for this client.
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
     * This function is called by the QmiClient::qmiAsyncResponseHandler infrastructure when it
     * receives an asynchronous response for this client.
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
     * Function to register with power-management QMI service for TCU-activity state update events
     */
    telux::common::Status registerTcuStateEvents(bool isMasterApp,
            std::shared_ptr<qmi::IQmiRegisterTcuStateEventCallback> callback,
            void *userData = nullptr);

    /**
     * Function to send acknowledgement to power-management service. The acknowledgemnt corresponds
     * to a TCU-activity state update event
     */
    telux::common::Status sendActivityStateUpdateAck(pwr_state_ack_v01 qmiTcuStateAck,
            std::shared_ptr<qmi::IQmiTcuStateAckCallback> callback, void *userData = nullptr);

    /**
     * Function to send a TCU-activity state command to power-management service, to initiate a
     * TCU-activity state transition
     */
    telux::common::Status sendActivityStateCommand(pwr_state_v01 qmiTcuState,
            std::shared_ptr<qmi::IQmiTcuStateCommandCallback> callback, void *userData = nullptr);

    PowerQmiClient(qmi_service_instance svcInstanceId);
    ~PowerQmiClient();

private:
    void notifyTcuStateUpdate(pwr_mgr_broadcast_indication_msg_v01 *indData);

    // deleting copy constructor, to implement PowerQmiClient as singleton
    PowerQmiClient(const PowerQmiClient &) = delete;
    // deleting assigning operator , to implement PowerQmiClient as singleton
    PowerQmiClient &operator=(const PowerQmiClient &) = delete;

    // Handler methods for responses to asynchronous requests
    void handleRegisterEventResp(void *respCStruct, uint32_t respCStructLen, void *data,
            qmi_client_error_type transpErr,
            std::weak_ptr<telux::common::ICommandCallback> callback);
    void handleTcuStateCommandResp(void *respCStruct, uint32_t respCStructLen, void *data,
            qmi_client_error_type transpErr,
            std::weak_ptr<telux::common::ICommandCallback> callback);
    void handleTcuStateUpdateAckResp(void *respCStruct, uint32_t respCStructLen, void *data,
            qmi_client_error_type transpErr,
            std::weak_ptr<telux::common::ICommandCallback> callback);

    // Handler methods for indications
    int handleTcuStateUpdateIndication(qmi_client_type userHandle, void *indBuf,
            unsigned int indBufLen);
    int handleSlaveAppsStatusIndication(qmi_client_type userHandle, void *indBuf,
            unsigned int indBufLen);
};

/**
 * Listener interface for unsolicited notifications. Clients should implement
 * this interface and register with PowerQmiClient in order to receive
 * unsolicited notifications/ indications for TCU-activity state updates
 */
class IQmiTcuActivityListener : public IQmiListener {
public:
    /**
     * This function will be called when the TCU-activity state change occurs
     *
     * @param [in] indData               Buffer holding the activity state information
     */
    virtual void onTcuStateUpdate(pwr_mgr_broadcast_indication_msg_v01 *indData){};

    /**
     * This function is called with the overall acknowledgement status from all the SLAVE
     * applications, for state change triggered previously by MASTER application.
     *
     * @param [in] indData               Buffer holding the activity state information
     */
    virtual void onSlaveAckStatusUpdate(pwr_mgr_state_change_v01 indData){};

    /**
     * This function will be called whenever the power service status changes
     *
     * @param [in] status         service status
     */
    virtual void onTcuActivityServiceStatusChange(telux::common::ServiceStatus status) {}
};

/**
 * Callback for registering TCU-activity state update events
 */
class IQmiRegisterTcuStateEventCallback : public telux::common::ICommandCallback {
public:
    /**
     * This function is called with response to registering for TCU-activity state updates
     *
     * @param [in] qmiError               Qmi error
     * @param [in] userData               Cookie user data value supplied by the client
     */
    virtual void registerEventsResponse(int qmiError,  pwr_mgr_register_resp_msg_v01* initialState,
                                                void *userData) = 0;
};

/**
 * Callback for the acknowledgements sent, corresponding to a TCU-activity state update event
 */
class IQmiTcuStateAckCallback : public telux::common::ICommandCallback {
public:
    /**
     * This function is called with the response to the acknowledgement sent.
     *
     * @param [in] qmiError               Qmi error
     * @param [in] userData               Cookie user data value supplied by the client
     */
    virtual void onTcuStateAckResponse(int qmiError, void *userData) = 0;
};

/**
 * Callback for TCU-activity state command, which was sent earlier to initiate TCU-activity state
 * transition
 */
class IQmiTcuStateCommandCallback : public telux::common::ICommandCallback {
public:
    /**
     * This function is called with the response to the TCU-activity state command operation.
     *
     * @param [in] qmiError               Qmi error
     * @param [in] userData               Cookie user data value supplied by the client
     */
    virtual void onTcuStateCommandResponse(int qmiError, void *userData) = 0;
};

}  // end namespace qmi
}  // end namespace telux

#endif  // POWERQMICLIENT_HPP
