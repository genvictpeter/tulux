/*
 *  Copyright (c) 2021, Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       ImsaQmiClient.hpp
 * @brief      QMI Client interacts with QMI ip multimedia sub system application to send/receive
 *             QMI requests/indications and dispatch to respective listeners
 */

#ifndef IMSAPPLICATIONQMICLIENT_HPP
#define IMSAPPLICATIONQMICLIENT_HPP

extern "C" {
#include <qmi/ip_multimedia_subsystem_application_v01.h>
}

#include <bitset>

#include "QmiClient.hpp"

namespace telux {
namespace qmi {

// Forward declarations
class IQmiRegisterImsaIndicationsCallback;
class IQmiImsaListener;


/**
 * Defines IMSA service indications being used
 */
enum ImsaIndicationType {
    IMSA_REG_STATUS,         /**< QMI_IMSA_REGISTRATION_STATUS_IND_V01 */
};

/**
 * 16 bit mask that denotes which of the IMS application service indications defined in
 * ImsaIndicationType are used to enable the indications.
 */
using ImsaIndicationMask = std::bitset<16>;

/**
 * This function is called with the response to sendGetRegStatusReq API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [in] slotId        Unique identifier for the SIM slot
 * @param [out] resp         Get IMSA registration status response message
 * @param [out] userData     Cookie user data value supplied by the client
 * @param [out] transpErr    Transport error
 *
 * @note   Eval: This is a new API and is being evaluated. It is subject to
 *         change and could break backwards compatibility.
 */
using GetImsServSysRegStatusCb = std::function<void(SlotId slotId,
    imsa_get_registration_status_resp_msg_v01 *resp, void *userData, int transpErr)>;


/**
 * This class provides mechanism to send messages to QMI IMS application Subsystem
 */
class ImsaQmiClient : public QmiClient {
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
    * Send request to get IMSA registration status parameters
    *
    * @param [in] slotId                   Unique identifier for the SIM slot
    * @param [in] callback                 Callback to get respone for get MSA registration
    *                                      status request
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of sendGetRegStatusReq i.e. success or suitable error code.
    */
    telux::common::Status sendGetRegStatusReq(SlotId slotId,
        GetImsServSysRegStatusCb callback, void *userData);

    /**
    * Register for a specific set of QMI indications supported by ImsaQmiClient API
    *
    * @param [in] indMask                  Set of indications to be enabled
    *                                      @ref ImsaIndicationMask
    * @param [in] callback                 Command Callback pointer
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of registerForImsaIndications i.e. success or suitable error code.
    */
    telux::common::Status registerForImsaIndications(ImsaIndicationMask indMask,
        std::shared_ptr<IQmiRegisterImsaIndicationsCallback> callback,
        void *userData = nullptr);

    bool isReady() override;

    ImsaQmiClient(SlotId slotId = SlotId::DEFAULT_SLOT_ID);
    ~ImsaQmiClient();

private:
    SlotId slotId_;
    std::atomic<bool> isBound_ = {false};
    telux::common::AsyncTaskQueue<void> taskQ_;

    // Deleting copy constructor, to implement ImsaQmiClient as singleton
    ImsaQmiClient(const ImsaQmiClient &) = delete;
    // Deleting assigning operator , to implement ImsaQmiClient as singleton
    ImsaQmiClient &operator=(const ImsaQmiClient &) = delete;

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

    void handleBindSubscriptionResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleGetRegStatusResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);

    // Indication handlers
    void handleRegStatusInd(qmi_client_type userHandle, void *indBuf,
        unsigned int indBufLen);
    void notifyRegStatusChange(
        imsa_registration_status_ind_msg_v01 *indData);

    telux::common::CommandCallbackManager cmdCallbackMgr_;
};

/**
 * Listener interface for unsolicited notifications. Clients should implement
 * this interface and register with ImsaQmiClient in order to receive
 * unsolicited notifications/ indications for Ims service enable config params change.
 */
class IQmiImsaListener : public IQmiListener {
public:

    /**
    * This function is called whenever any IMS application registration status changes.
    *
    * @param [in] ind                            Indication data has information of IMS service
    *                                            registration status
    * @param [in] slotId                         Unique identifier for the SIM slot
    */
    virtual  void onImsServSysRegStatusChange(
       imsa_registration_status_ind_msg_v01 *ind, SlotId slotId) {
    }

    virtual  void onImsaServiceStatusChange(telux::common::ServiceStatus status) {
    }

    virtual ~IQmiImsaListener() {
    }
};

/**
 * Callback class for register Indications request
 */
class IQmiRegisterImsaIndicationsCallback : public telux::common::ICommandCallback {
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

#endif  // IMSAPPLICATIONQMICLIENT_HPP