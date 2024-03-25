/*
 *  Copyright (c) 2020 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       TmdQmiClient.hpp
 * @brief      TMD QMI Client interacts with Thermal-mitigation-device (TMD) QMI service
 *             to send/receive QMI requests/indications and dispatch to respective listeners
 *
 */

#ifndef TMDQMICLIENT_HPP
#define TMDQMICLIENT_HPP

extern "C" {
#include <qmi/thermal_mitigation_device_service_v01.h>
}

#include "QmiClient.hpp"

namespace telux {
namespace qmi {

// Forward declarations
class IQmiTmdMitigationLevelListener;
class IQmiTmdGetMitigationDeviceListCallback;
class IQmiTmdSetMitigationLevelCallback;
class IQmiTmdGetMitigationLevelCallback;
class IQmiTmdRegisterNotificationMitigationLevelCallback;
class IQmiTmdDeregisterNotificationMitigationLevelCallback;


//TmdQmiClient class which is extension of base class QmiClient
class TmdQmiClient : public QmiClient {
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
     * Function to send Thermal Mitigation device list request to TMD QMI service
     */
    telux::common::Status sendGetMitigationDeviceList(
        std::shared_ptr<qmi::IQmiTmdGetMitigationDeviceListCallback> callback,
        void *userData = nullptr);

    /**
     * Function to send a request to TMD QMI service to set Thermal Mitigation level for a device
     */
    telux::common::Status sendSetMitigationLevel(uint8_t mitigation_level,
        tmd_mitigation_dev_id_type_v01 mitigation_dev_id,
        std::shared_ptr<qmi::IQmiTmdSetMitigationLevelCallback> callback,
        void *userData = nullptr);

    /**
     *  Function to send a request to TMD QMI service to get Thermal Mitigation level for a device
     *
     * It also returns the requested_mitigation_level of the current control point (as requested in
     * set_mitigation_level). The control point compares the current_mitigation_level with the
     * requested_mitigation_level. If these values are different, it is possible that the mitigation
     * device level has been set by another control point.
     */
    telux::common::Status sendGetMitigationLevel(tmd_mitigation_dev_id_type_v01 mitigation_device,
        std::shared_ptr<qmi::IQmiTmdGetMitigationLevelCallback> callback, void *userData = nullptr);

    /**
     * Function to register for notification of any change in mitigation level of a device.
     *
     * The control point learns of every change in mitigation levels for the specified device via a
     * QMI_TMD_MITIGATION_LEVEL_REPORT_IND indication, until the mitigation level notification for
     * that device is deregistered.If the client registers for notification more than once for the
     * same device (without deregistration), it is ignored by the service and has no effect on the
     * existing registration.
     */
    telux::common::Status sendRegisterNotificationMitigationLevel(
        tmd_mitigation_dev_id_type_v01 mitigation_device,
        std::shared_ptr<qmi::IQmiTmdRegisterNotificationMitigationLevelCallback> callback,
        void *userData = nullptr);

    /**
     * Function to deregisters notification for changes in the specified device mitigation level
     * settings.
     *
     * The notification needs to be previously registered using
     * sendRegisterNotificationMitigationLevel API for the same device
     */
    telux::common::Status sendDeregisterNotificationMitigationLevel(
        tmd_mitigation_dev_id_type_v01 mitigation_device,
        std::shared_ptr<qmi::IQmiTmdDeregisterNotificationMitigationLevelCallback> callback,
        void *userData = nullptr);

    /**
     * Initializes QMI Request and Response structures.
     * Also allocates memory to ResponseType pointer.
     * Takes request in the form of a pointer.
     *
     * @param [in] request   QMI request pointer
     * @param [in] response  QMI response pointer
     *
     */
    template <typename RequestType, typename ResponseType>
    telux::common::Status mallocAndInitParams(RequestType *&request, ResponseType *&response) {
        if (request) {
            memset(request, 0, sizeof(RequestType));
        }
        response = (ResponseType *)malloc(sizeof(ResponseType));
        if (!response) {
            LOG(ERROR, "Unable to allocate memory");
            return telux::common::Status::FAILED;
        }
        memset(response, 0, sizeof(ResponseType));
        return telux::common::Status::SUCCESS;
    }

    /**
     * Utility method for sending request to QMI.
     * Takes request in the form of pointer.
     */
    template <typename RequestType, typename ResponseType>
    telux::common::Status sendRequest(int cmdId, unsigned int qmiMessageId, RequestType *&request,
        ResponseType *&response, void *userData) {
        // User data
        QmiUserData *qmiUserData = (QmiUserData *)malloc(sizeof(QmiUserData));
        if (qmiUserData == NULL) {
            LOG(ERROR, "Memory allocation failed");
            return telux::common::Status::FAILED;
        }
        memset(qmiUserData, 0, sizeof(QmiUserData));

        qmiUserData->data = userData;
        qmiUserData->qmiClient = this;
        qmiUserData->cmdCallbackId = cmdId;

        qmi_txn_handle txnHandle;
        qmi_client_error_type clientErr = QMI_NO_ERR;
        uint8_t reqSize = 0;
        if (request != nullptr) {
            reqSize = sizeof(RequestType);
        }

        // Sending async request to QMI
        clientErr = qmi_client_send_msg_async(getClientHandle(), qmiMessageId, request, reqSize,
            response, sizeof(ResponseType), qmiAsyncResponseCallback, qmiUserData, &txnHandle);
        telux::common::ErrorCode errorCode
            = telux::common::ErrorHelper::qmiErrorToErrorCode(clientErr);
        LOG(DEBUG, __FUNCTION__, " Client error(", clientErr,
            ") errStr: ", telux::common::ErrorHelper::getQmiErrorAsString(clientErr),
            " Error Code: ", static_cast<int>(errorCode));

        if (clientErr) {
            LOG(ERROR, "Unable to send qmi message");
            if (NULL != qmiUserData) {
                LOG(DEBUG, "freeing qmiUserData");
                free(qmiUserData);
                qmiUserData = NULL;
            }
            return telux::common::Status::FAILED;
        }
        return telux::common::Status::SUCCESS;
    }

    /**
     * Sends an Asynchronous request to QMI service.
     * Takes request in the form of a pointer
     *
     * @param [in] qmiMessageId    QMI message Id.
     * @param [in] request         QMI request pointer.
     * @param [in] response        QMI response pointer.
     * @param [in] callback        Command callback pointer.
     * @param [in] userData        Cookie data sent by the caller.
     *
     */
    template <typename RequestType, typename ResponseType, typename... Args>
    telux::common::Status sendAsyncRequest(unsigned int qmiMessageId, RequestType *&request,
        ResponseType *&response, std::shared_ptr<telux::common::ICommandCallback> callback,
        void *userData) {
        LOG(DEBUG, __FUNCTION__);

        int cmdId = INVALID_COMMAND_ID;
        if (callback) {
            cmdId = cmdCallbackMgr_.addCallback(callback);
        }

        auto status = sendRequest(cmdId, qmiMessageId, request, response, userData);
        return status;
    }

    TmdQmiClient();
    ~TmdQmiClient();

private:
    void notifyMitigationLevelUpdate(tmd_mitigation_level_report_ind_msg_v01 *indData);

   // deleting copy constructor, to implement TmdQmiClient as singleton
   TmdQmiClient(const TmdQmiClient &) = delete;
   // deleting assigning operator, to implement TmdQmiClient as singleton
   TmdQmiClient &operator=(const TmdQmiClient &) = delete;

   // Handler methods for responses to asynchronous requests
   void handleGetMitigationDeviceListResp(void *respCStruct, uint32_t respCStructLen, void *data,
       qmi_client_error_type transpErr,
       std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleSetMitigationLevelResp(void *respCStruct, uint32_t respCStructLen, void *data,
       qmi_client_error_type transpErr,
       std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleGetMitigationLevelResp(void *respCStruct, uint32_t respCStructLen, void *data,
       qmi_client_error_type transpErr,
       std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleRegisterNotificationMitigationLevel(void *respCStruct, uint32_t respCStructLen,
       void *data,qmi_client_error_type transpErr,
       std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleDeregisterNotificationMitigationLevel(void *respCStruct, uint32_t respCStructLen,
       void *data, qmi_client_error_type transpErr,
       std::weak_ptr<telux::common::ICommandCallback> callback);

   // Handler methods for indications
   int handleMitigationLevelUpdate(
       qmi_client_type userHandle, void *indBuf, unsigned int indBufLen);

};

/**
 * Listener interface for unsolicited notifications. Clients should implement
 * this interface and register with TmdQmiClient in order to receive
 * unsolicited notifications/indications for device mitigation level updates
 */
class IQmiTmdMitigationLevelListener : public IQmiListener {
 public:
    /**
     * This function will be called when the mitigation level for previously registered devices
     * has changed
     * @param [in] indData  Buffer holding the activity state information
     */
    virtual void onTmdMitigationLevelUpdate(tmd_mitigation_level_report_ind_msg_v01 *indData) {
    };

    /**
     * This function will be called whenever there is a TMD service status change.
     *
     * @param [in] status  service status
     */
    virtual void onTmdMitigationServiceStatusChange(telux::common::ServiceStatus status){};
};

/**
 * Callback for sendGetMitigationDeviceList which is sent to get Mitigation Device Lists.
 */
class IQmiTmdGetMitigationDeviceListCallback : public telux::common::ICommandCallback {
public:
    /**
     * This function is called with response to  sendGetMitigationDevicelist
     *
     * @param [in] qmiResp               pointer to Qmi response message
     * @param [in] qmiError              Qmi error
     * @param [in] userData              Cookie user data value supplied by the client
     */
   virtual void onGetMitigationDeviceListResp(tmd_get_mitigation_device_list_resp_msg_v01 *qmiResp,
       int qmiError, void *userData)
       = 0;
};

/**
 * Callback for sendSetMitigatioLevel which is sent earlier to update/set the Device Mitigation
 * Level.
 */
class IQmiTmdSetMitigationLevelCallback : public telux::common::ICommandCallback {
 public:
    /**
     * This function is called with response to  sendSetMitigationLevel
     *
     * @param [in] qmiResp               pointer to Qmi response message
     * @param [in] qmiError              Qmi error
     * @param [in] userData              Cookie user data value supplied by the client
     */
    virtual void onSetMitigationLevelResp(tmd_set_mitigation_level_resp_msg_v01 *qmiResp,
        int qmiError, void *userData) = 0;
};

/**
 * Callback for sendGetMitigatioLevel which is sent earlier to get the Current Mitigation Level of
 * Device Level.
 */
class IQmiTmdGetMitigationLevelCallback : public telux::common::ICommandCallback {
 public:
    /**
     * This function is called with response to  sendGetMitigationLevel
     *
     * @param [in] qmiResp               pointer to Qmi response message
     * @param [in] qmiError              Qmi error
     * @param [in] userData              Cookie user data value supplied by the client
     */
    virtual void onGetMitigationLevelResp(tmd_get_mitigation_level_resp_msg_v01 *qmiResp,
        int qmiError, void *userData) = 0;
};

/**
 * Callback for sendRegisterNotificationMitigatioLevel which was sent earlier to register the device
 * to receive mitigation level notification
 */
class IQmiTmdRegisterNotificationMitigationLevelCallback : public telux::common::ICommandCallback {
 public:
    /**
     * This function is called with response to sendRegisterNotificationMitigatioLevel
     *
     * @param [in] qmiResp               pointer to Qmi response message
     * @param [in] qmiError              Qmi error
     * @param [in] userData              Cookie user data value supplied by the client
     */
    virtual void onRegisterNotificationMitigationLevelResp(
        tmd_register_notification_mitigation_level_resp_msg_v01 *qmiResp, int qmiError,
        void *userData) = 0;
};

/**
 * Callback for sendDeregisterNotificationMitigatioLevel which was sent earlier to Deregister the
 * device to stop getting mitigation level notification/updates
 */
class IQmiTmdDeregisterNotificationMitigationLevelCallback : public telux::common::ICommandCallback
{
 public:
    /**
     * This function is called with response to  senDeregisterNotificationMitigatioLevel
     *
     * @param [in] qmiResp               pointer to Qmi response message
     * @param [in] qmiError              Qmi error
     * @param [in] userData              Cookie user data value supplied by the client
     */
    virtual void onDeregisterNotificationMitigationLevelResp(
        tmd_deregister_notification_mitigation_level_resp_msg_v01 *qmiResp, int qmiError,
        void *userData) = 0;
};

} // end namespace qmi
} // end namespace telux

#endif  // TMDQMICLIENT_HPP

