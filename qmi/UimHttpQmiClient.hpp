/*
 *  Copyright (c) 2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       UimHttpQmiClient.hpp
 * @brief      QMI Client interacts with QMI user identity module http service to send requests
 *             and dispatch received indications to respective listeners
 */

#ifndef UIMHTTPQMICLIENT_HPP
#define UIMHTTPQMICLIENT_HPP

extern "C" {
#include <qmi/user_identity_module_http_v01.h>
}

#include "telux/common/CommonDefines.hpp"
#include "common/CommandCallbackManager.hpp"

#include "QmiClient.hpp"

namespace telux {
namespace qmi {

/**
 *Header information to be sent along with HTTP post request.
 */
struct HttpHeader {
    std::string name;  /**< Header name */
    std::string value; /**< Header value */

    HttpHeader(std::string headerName, std::string headerValue)
       : name(headerName)
       , value(headerValue) {
    }
};

struct UimHttpQmiUserData {
   void *data;
   uint32_t token;
};

struct HttpTransactionResult {
    uint16_t noOfChunks = 0;          //  Total no of Chunks need to be sent to modem
    uint16_t currentChunk = 0;        //  Current Chunk being sent to modem. Index starts from 0
    uint32_t tokenId = 0;             //  Token Identifier corresponding to HTTP transaction.
    uint32_t responseLen = 0;         //  Response Payload length
    std::vector<uint8_t> httpResponsePayload; // Storing response payload of new HTTP request,
                                               // this is required since we need to divide the
                                               // payload into chunks, since QMI max payload size
                                               // is 2KB
    uint16_t httpResult = 0;          // Http Transaction Result

};

using QmiResponseCallback = std::function<void(int qmiError, void *userData, int transpErr)>;

/**
 * This class provides mechanism to send messages to QMI user identity module
 */
class UimHttpQmiClient : public QmiClient {
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
     * Send HTTP reset request to reset the service state variables.
     *
     * @param [in] callback        Optional callback to receive response for HTTP reset request
     * @param [in] userData        Cookie user data value supplied by the client
     *
     */
    telux::common::Status sendHttpResetReq(QmiResponseCallback callback = nullptr,
        void *userData = nullptr);

    /**
     * Send HTTP response/payload of HTTP request back to modem.
     *
     * @param [in] tokenId         Token Identifier
     * @param [in] result          Contains the result of the HTTP request
     * @param [in] headers         Customer headers
     * @param [in] payload         HTTP response payload
     * @param [in] callback        Callback to receive HTTP response
     * @param [in] userData        Cookie user data value supplied by the client
     */
    telux::common::Status sendHttpTransactionReq(uint32_t tokenId, int result,
        const std::vector<HttpHeader> &headers, const std::vector<uint8_t> &payload,
        QmiResponseCallback callback = nullptr, void *userData = nullptr);

    UimHttpQmiClient();
    ~UimHttpQmiClient();

 private:

    std::mutex mtx_;

    telux::common::CommandCallbackManager cmdCallbackMgr_;

    //Map with key as TokenId and corresponding value as HttpTransactionResult
    std::map<uint32_t , telux::qmi::HttpTransactionResult *> transactionResultMap_ = {};

    // deleting copy constructor, to implement UimHttpQmiClient as singleton
    UimHttpQmiClient(const UimHttpQmiClient &) = delete;
    // deleting assigning operator , to implement UimHttpQmiClient as singleton
    UimHttpQmiClient &operator=(const UimHttpQmiClient &) = delete;

    void handleHttpResetResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleHttpTransactionResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);

    // Indication handlers
    int handleHttpTransactionInd(qmi_client_type userHandle, void *indBuf, unsigned int indBufLen);

    void notifyHttpTransactionInd(const uim_http_transaction_ind_msg_v01 &indData);

};

/**
 * Listener interface for unsolicited notifications. Clients should implement
 * this interface and register with UimHttpQmiClient in order to receive
 * unsolicited notifications/ indications for HTTP request.
 */
class IQmiUimHttpListener : public IQmiListener {
 public:
    /**
     * This function is called to indicate to the control point to transmit
     * a HTTP request to the server
     *
     * @param [in] ind -  UIM Http Transaction Message received from modem
     */
    virtual void onHttpTransactionInd(const uim_http_transaction_ind_msg_v01 &ind) {
    }

    /**
     * This function will be called whenever the QMI UIM HTTP service status changes
     *
     * @param [in] status         service status
     */
    virtual void onUimHttpServiceStatusChange(telux::common::ServiceStatus status) {}

    virtual ~IQmiUimHttpListener() {
    }
};

}  // end namespace qmi
}  // end namespace telux

#endif  // UIMHTTPQMICLIENT_HPP