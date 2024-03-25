/*
 *  Copyright (c) 2019 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       ThermalQmiClient.hpp
 * @brief      Thermal QMI Client interacts with Thermal-mitigation-management (tsens) QMI service
 *             to send/receive QMI requests/indications and dispatch to respective listeners
 *
 */

#ifndef THERMALQMICLIENT_HPP
#define THERMALQMICLIENT_HPP

extern "C" {
#include "tsens_service_v01.h"
}

#include "QmiClient.hpp"

namespace telux {
namespace qmi {

// Forward declarations
class IQmiRegisterAutoShutdownEventCallback;
class IQmiSetShutdownModeCallback;
class IQmiGetShutdownModeCallback;
class IQmiAutoShutdownModeListener;

/**
 * This class provides mechanism to send messages to thermal-automatic-shutdown-management QMI
 * Service and receive indications for auto-shutdown state updates.
 */
class TsensQmiClient : public QmiClient {
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
     * Function to register with thermal-automatic-shutdown-management QMI service for mitigation
     * state update events
     */
    telux::common::Status registerAutoShutdownModeEvents(
            std::shared_ptr<qmi::IQmiRegisterAutoShutdownEventCallback> callback,
            void *userData = nullptr);

    /**
     * Function to query auto-shutdown mode from thermal-mitigation-management service.
     */
    telux::common::Status sendGetAutoShutdownModeCommand(
            std::shared_ptr<qmi::IQmiGetShutdownModeCallback> callback, void *userData = nullptr);

    /**
     * Function to send auto-shutdown mode command to thermal-mitigation-management service and
     * initiate a automatic thermal shutdown mode transition
     */
    telux::common::Status sendSetAutoShutdownModeCommand(tsens_state_v01 qmiTsensState,
                uint32_t timeout, std::shared_ptr<qmi::IQmiSetShutdownModeCallback> callback,
                void *userData = nullptr);

    TsensQmiClient();
    ~TsensQmiClient();

private:
    void notifyAutoShutdownModeUpdate(tsens_broadcast_indication_msg_v01 *indData);

    // deleting copy constructor, to implement TsensQmiClient as singleton
    TsensQmiClient(const TsensQmiClient &) = delete;
    // deleting assigning operator , to implement TsensQmiClient as singleton
    TsensQmiClient &operator=(const TsensQmiClient &) = delete;

    // Handler methods for responses to asynchronous requests
    void handleRegisterEventResp(void *respCStruct, uint32_t respCStructLen, void *data,
            qmi_client_error_type transpErr,
            std::weak_ptr<telux::common::ICommandCallback> callback);
    void handleSetCommandResp(void *respCStruct, uint32_t respCStructLen, void *data,
            qmi_client_error_type transpErr,
            std::weak_ptr<telux::common::ICommandCallback> callback);
    void handleGetCommandResp(void *respCStruct, uint32_t respCStructLen, void *data,
            qmi_client_error_type transpErr,
            std::weak_ptr<telux::common::ICommandCallback> callback);

    // Handler methods for indications
    int handleAutoShutdownModeUpdate(qmi_client_type userHandle, void *indBuf,
            unsigned int indBufLen);
};

/**
 * Listener interface for unsolicited notifications. Clients should implement
 * this interface and register with TsensQmiClient in order to receive
 * unsolicited notifications/indications for auto-shutdown mode updates
 */
class IQmiAutoShutdownModeListener : public IQmiListener {
public:
    /**
     * This function will be called when the auto-shutdown mode change occurs
     *
     * @param [in] indData  Buffer holding the activity state information
     */
    virtual void onAutoShutdownModeUpdate(tsens_broadcast_indication_msg_v01 *indData){
    };

    /**
     * This function will be called whenever the thermal-automatic-shutdown-management service
     * status changes
     *
     * @param [in] status  service status
     */
    virtual void onAutoShutdownServiceStatusChange(telux::common::ServiceStatus status) {
    }
};

/**
 * Callback for registerAutoShutdownModeEvents
 */
class IQmiRegisterAutoShutdownEventCallback : public telux::common::ICommandCallback {
public:
    /**
     * This function is called with response to registering for Thermal auto-shutdown mode updates
     *
     * @param [in] qmiError               Qmi error
     * @param [in] duration               Imminent duation for auto-enablement of shutdown mode
     * @param [in] userData               Cookie user data value supplied by the client
     */
    virtual void registerEventsResponse(int qmiError, uint32_t duration, void *userData) = 0;
};

/**
 * Callback for sendSetAutoShutdownModeCommand, which was sent earlier to initiate auto shutdown
 * mode transition
 */
class IQmiSetShutdownModeCallback : public telux::common::ICommandCallback {
public:
    /**
     * This function is called with response to sendSetAutoShutdownModeCommand.
     *
     * @param [in] qmiError               Qmi error
     * @param [in] userData               Cookie user data value supplied by the client
     */
    virtual void onSetModeCommandResponse(int qmiError, void *userData) = 0;
};

/**
 * Callback sendGetAutoShutdownModeCommand, which was sent earlier to query auto shutdown mode
 */
class IQmiGetShutdownModeCallback : public telux::common::ICommandCallback {
public:
    /**
     * This function is called with the response to sendGetAutoShutdownModeCommand request.
     *
     * @param [in] qmiError               Qmi error
     * @param [in] qmiResp                pointer to Qmi response message
     * @param [in] userData               Cookie user data value supplied by the client
     */
    virtual void onGetModeCommandResponse(int qmiError, tsens_get_state_resp_msg_v01 *qmiResp,
                            void *userData) = 0;
};

}  // end namespace qmi
}  // end namespace telux

#endif  // THERMALQMICLIENT_HPP
