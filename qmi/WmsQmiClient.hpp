/*
 *  Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       WmsQmiClient.hpp
 *
 * @brief      Qmi WMS Client is a singleton class to set and get broadcast
 *             configuration and activate broadcast configuration. It contains
 *             structure definitions and function prototype for WMS QMI Client,
 *             this class provides mechanism to send QMI messages to WMS QMI Service
 *             and get the results. It then send the events back to the caller.
 *
 */

#ifndef WMSQMICLIENT_HPP
#define WMSQMICLIENT_HPP

#include <memory>
#include <vector>
#include <bitset>

extern "C" {
#include <qmi/wireless_messaging_service_v01.h>
}

#include "common/CommandCallbackManager.hpp"
#include "telux/common/CommonDefines.hpp"
#include "telux/tel/CellBroadcastDefines.hpp"
#include "QmiClient.hpp"

namespace telux {
namespace qmi {
// forward declarations
class IQmiWmsListener;

/**
 * Defines WMS QMI service indications being used
 */
enum WmsEventReportType {
   BROADCAST_MSG,         /**< QMI_WMS_EVENT_REPORT_IND_V01 */
};

/**
 * 16 bit mask that denotes which of the WMS QMI service indications defined in WmsEventReportType
 * are used to enable the event report.
 */
using WmsEventReportMask = std::bitset<16>;

/**
 * Defines WMS QMI service indications being used
 */
enum WmsIndicationType {
   BROADCAST_CONFIG,      /**< QMI_WMS_BROADCAST_CONFIG_IND_V01 */
};

/**
 * 16 bit mask that denotes which of the WMS QMI service indications defined in WmsIndicationType
 * are used to enable the indications.
 */
using WmsIndicationMask = std::bitset<16>;

/**
 * This function is called with the response to registerForIndications API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [out] resp         QMI WMS register for indications response message
 * @param [out] userData     Cookie user data value supplied by the client
 * @param [out] transpErr    Transport error
 *
 * @note   Eval: This is a new API and is being evaluated. It is subject to
 *         change and could break backwards compatibility.
 */
using WmsRegisterForIndicationsCb = std::function<void(
    wms_indication_register_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendSetBroadcastConfigRequest API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [out] resp         QMI WMS set broadcast config response message
 * @param [out] userData     Cookie user data value supplied by the client
 * @param [out] transpErr    Transport error
 *
 * @note   Eval: This is a new API and is being evaluated. It is subject to
 *         change and could break backwards compatibility.
 */
using SetBroadcastConfigCb = std::function<void(
   wms_set_broadcast_config_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendGetBroadcastConfigRequest API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [out] resp         QMI WMS get broadcast config response message
 * @param [out] userData     Cookie user data value supplied by the client
 * @param [out] transpErr    Transport error
 *
 * @note   Eval: This is a new API and is being evaluated. It is subject to
 *         change and could break backwards compatibility.
 */
using GetBroadcastConfigCb = std::function<void(
   wms_get_broadcast_config_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendSetBroadcastActivationRequest API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [out] resp         QMI WMS set broadcast activation response message
 * @param [out] userData     Cookie user data value supplied by the client
 * @param [out] transpErr    Transport error
 *
 * @note   Eval: This is a new API and is being evaluated. It is subject to
 *         change and could break backwards compatibility.
 */
using SetBroadcastActivationCb = std::function<void(
   wms_set_broadcast_activation_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendSetEventReportRequest API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [out] resp         QMI WMS set event repot response message
 * @param [out] userData     Cookie user data value supplied by the client
 * @param [out] transpErr    Transport error
 *
 * @note   Eval: This is a new API and is being evaluated. It is subject to
 *         change and could break backwards compatibility.
 */
using SetEventReportCb = std::function<void(
    wms_set_event_report_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This class provides mechanism to send messages to QMI WMS Subsystem and supports
 * set and get broadcast configuration and activation of broadcast configuration.
 */

class WmsQmiClient : public QmiClient {
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
     * Registers for QMI Indications supported by WmsQmiClient API like change in broadcast
     * configurations.
     *
     * @param [in] indMask        @ref WmsIndicationMask
     * @param [in] callback       Command callback
     * @param [in] userData       Cookie user data value supplied by the client
     *
     * @returns Status of registerForIndications i.e. success or suitable error
     * code.
     */
    telux::common::Status registerForIndications(WmsIndicationMask indMask,
        WmsRegisterForIndicationsCb callback = nullptr, void *userData = NULL);

    /**
     * Sets the cellbroadcast SMS conﬁguration.
     *
     * @param [in] configs         3GPP Broadcast SMS configurations
     * @param [in] callback        Command Callback
     * @param [in] userData        Cookie user data value supplied by the client
     *
     * @returns Status of sendSetBroadcastConfigRequest i.e. success or suitable error
     * code.
     */
    telux::common::Status sendSetBroadcastConfigRequest(
        std::vector<telux::tel::CellBroadcastFilter> configs, SetBroadcastConfigCb callback,
        void *userData = nullptr);

    /**
     *  Gets the current cellbroadcast SMS conﬁguration.
     *
     * @param [in] callback        Command Callback
     * @param [in] userData        Cookie user data value supplied by the client
     *
     * @returns Status of sendGetBroadcastConfigRequest i.e. success or suitable error
     * code.
     */
    telux::common::Status sendGetBroadcastConfigRequest(GetBroadcastConfigCb callback,
        void *userData = nullptr);

    /**
     * Activate or deactivate the reception of cellbroadcast SMS messages.
     *
     * @param [in] isActivate      Enable or disable broadcast SMS Message
     * @param [in] activateAll     If true filter 3GPP cell broadcast messages based on
     *                             language preferences. If false ignore language preferences.
     * @param [in] callback        Command Callback
     * @param [in] userData        Cookie user data value supplied by the client
     *
     * @returns Status of sendSetBroadcastActivationRequest i.e. success or suitable error
     * code.
     */
    telux::common::Status sendSetBroadcastActivationRequest(bool isActivate, bool activateAll,
        SetBroadcastActivationCb callback, void *userData = nullptr);

    /**
     * Set WMS event report to get cell broadcast notification
     *
     * @param [in] eventMask      @ref WmsEventReportMask
     * @param [in] callback       Command Callback
     * @param [in] userData       Cookie user data value supplied by the client
     *
     * @returns Status of sendSetEventReportRequest i.e. success or suitable error
     * code.
     */
    telux::common::Status sendSetEventReportRequest(WmsEventReportMask eventMask,
        SetEventReportCb callback, void *userData = nullptr);

    bool isReady() override;

    telux::common::Status init(qmi_idl_service_object_type idlServiceObject);

    WmsQmiClient(int slotId = DEFAULT_SLOT_ID);
    ~WmsQmiClient();

private:
    int slotId_;
    std::atomic<bool> isBound_ = {false};
    telux::common::AsyncTaskQueue<void> taskQ_;
    telux::common::CommandCallbackManager cmdCallbackMgr_;

    // deleting copy constructor, to implement WmsQmiClient as singleton
    WmsQmiClient(const WmsQmiClient &) = delete;
    // deleting assigning operator , to implement WmsQmiClient as singleton
    WmsQmiClient &operator=(const WmsQmiClient &) = delete;

    bool waitForInitialization() override;
    void initSync(void);
    void setBoundState(bool isBound);

    telux::common::Status bindSubscriptionRequest(int slotId, void *userData = nullptr);

    // Handler methods for responses to asynchronous requests
    void handleBindSubscriptionResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleIndicationsRegResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);

    void handlSetBroadcastConfigResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleGetBroadcastConfigResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleSetBroadcastActivationResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleSetEventReportResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);

    // Handler methods for handling indications
    void handleBroadcastConfigInd(qmi_client_type userHandle, void *indBuf, unsigned int indBufLen);

    void notifyBroadcastConfig(wms_broadcast_config_ind_msg_v01 *indData);

    void handleEventReportInd(qmi_client_type userHandle, void *indBuf, unsigned int indBufLen);

    void notifyEventReport(wms_event_report_ind_msg_v01 *indData);

}; // class WmsQmiClient

/**
 * Listener interface for unsolicited notifications. Clients should implement
 * this interface and register with WmsQmiClient in order to receive
 * unsolicited notifications/ indications for broadcast config change and
 * event report indication.
 */
class IQmiWmsListener : public IQmiListener {
public:
    /**
    * This function is called whenever there is a change in broadcast configuration
    *
    * @param [in] indData - Broadcast configuration change indication.
    */
    virtual void onBroadcastConfigurationChanged(wms_broadcast_config_ind_msg_v01 *indData) {}

    /**
    * This function is called whenever there is event report indication
    * like new cell broadcast SMS recieved.
    *
    * @param [in] indData - Event report indication
    */
    virtual void onEventReportIndication(wms_event_report_ind_msg_v01 *indData) {}

    /**
    * This function is called whenever there is WMS service status change
    * @param [in] status - @ref telux::common::ServiceStatus
    */
    virtual void onWmsServiceStatusChange(telux::common::ServiceStatus status) {}

    virtual ~IQmiWmsListener() {}
};


}  // end namespace qmi
}  // end namespace telux

#endif  // end of WMSQMICLIENT_HPP
