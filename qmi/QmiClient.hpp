/*
 *  Copyright (c) 2017-2019, 2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       QmiClient.hpp
 *
 * @brief      QMI Client is the base class for all QMI Clients such as LocationQmiClient,
 *             VoiceQmiClient, UimQmiClient etc.
 *             It provides common methods such as initialize/ release QMI client and
 *             common handler and utility methods that are required for all QMI Clients.
 *
 */

#ifndef QMICLIENT_HPP
#define QMICLIENT_HPP

extern "C" {
#include <qmi-framework/qmi_client.h>
}

#include <vector>
#include <memory>
#include <mutex>
#include <vector>

#include <telux/common/CommonDefines.hpp>

#include "common/TaskDispatcher.hpp"
#include "common/ErrorHelper.hpp"
#include "common/CommandCallbackManager.hpp"
#include "common/Logger.hpp"
#include "common/AsyncTaskQueue.hpp"
#include "common/ListenerManager.hpp"

#define DEFAULT_TIMEOUT_IN_MILLISECONDS 4000

namespace telux {
namespace qmi {

/**
 * Structure to send the user data for each QMI request, it's supplied
 * by the client
 */
struct QmiUserData {
   int cmdCallbackId = -1;
   void *qmiClient = nullptr;
   void *data;
};

/**
 * This is an empty base class for all Qmi Listener classes.
 * It allows child classes to inherit and implement the required notification methods.
 */
class IQmiListener {
public:
   virtual ~IQmiListener() {
   }
};

/**
 * This class is the base class of all QMI Client classes.
 * It provides methods to initialize, release QMI Clients as well as common
 * handler and utility methods required by all QMI Clients.
 */
class QmiClient {
public:
   /**
    * This function will be called by qmiIndicationCallback when it
    * receives an indications from QMI.
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
   virtual void indicationHandler(qmi_client_type userHandle, unsigned int msgId, void *indBuf,
                                  unsigned int indBufLen, void *indCbData)
      = 0;

   /**
    * This function is called by the QCCI infrastructure when
    * infrastructure receives an asynchronous response for this client.
    *
    * NOTE: Callback happens in the QCCI thread context and raises signal.
    *
    * @param [in] userHandle           Opaque handle used by the infrastructure to
    *                                  identify different services.
    * @param [in] msgId                Message ID of the indication
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] transpErr            Transport error
    * @param [in] callback             Command callback pointer
    */
   virtual void asyncResponseHandler(unsigned int msgId, void *respCStruct,
                                     unsigned int respCStructLen, void *userData,
                                     qmi_client_error_type transpErr,
                                     std::shared_ptr<telux::common::ICommandCallback> callback)
      = 0;

   /**
    * This function is called by qmiClientErrorCallbackSync when the service
    * terminates or deregisters.
    *
    * NOTE: Callback happens on new thread created by std::async.
    *
    * @param [in] clientError           Error value
    */
   virtual void errorHandler(qmi_client_error_type clientError) {
   }

   /**
    * This function is called by qmiClientNotifierCallbackSync when a service
    * event occurs indicating that the service count has changed.
    *
    * NOTE: Callback happens on new thread created by std::async.
    *
    * @param [in]   serviceEvent       Event type
    */
   virtual void notifierHandler(qmi_client_notify_event_type serviceEvent) {
   }

   /**
    * Establishes QMI client connection for a specific QMI service type. Registers error callback
    * as well to get notification about Service termination/deregistration event.
    *
    * param [in] idlServiceObject QMI IDL Service Object Type.
    */
   telux::common::Status initClient(qmi_idl_service_object_type idlServiceObject);

   /**
    * Initiates QMI client connection process
    *
    * @param [in]   idlServiceObject    QMI IDL Service Object Type
    */
   telux::common::Status init(qmi_idl_service_object_type idlServiceObject);

   /**
    * Releases the client from QMI service
    */
   telux::common::Status releaseClient();

   /**
    * Checks the status of QMI Service and returns the result.
    *
    * @returns True if QMI Service is ready for service otherwise false.
    *
    * @note   Eval: This is a new API and is being evaluated. It is subject to
    *         change and could break backwards compatibility.
    */
   virtual bool isReady();

   /**
    * Wait for QMI Service to be ready.
    *
    * @returns  A future that caller can wait on to be notified when network
    *           subsystem is ready.
    *
    * @note   Eval: This is a new API and is being evaluated. It is subject to
    *         change and could break backwards compatibility.
    */
   std::future<bool> onReady();

   QmiClient(qmi_service_instance svcInstanceId = QMI_CLIENT_INSTANCE_ANY);
   virtual ~QmiClient();

   /**
    * Adds a listener to notify about QMI indications
    */
   telux::common::Status registerListener(std::weak_ptr<IQmiListener> listener);

   /**
    * Removes a previously registered listener
    */
   telux::common::Status deregisterListener(std::weak_ptr<IQmiListener> listener);

   struct ErrorParam {
       qmi_client_error_type error;
   };


   struct NotifierParam {
       qmi_idl_service_object_type serviceObj;
       qmi_client_notify_event_type serviceEvent;
   };

   struct ServiceParam {
       common::ServiceStatus status;
       qmi_client_type usrHandle;
       union {
           ErrorParam err;
           NotifierParam notif;
       };
       bool operator==(const ServiceParam& sp) {
           return sp.status == status;
       }

       bool operator!=(const ServiceParam& sp) {
           return sp.status != status;
       }
   };

   void handleNewState(std::shared_ptr<ServiceParam> sp);

   /**
    * This function is used to clean up the qmi client and should be used only when the qmi client
    * is no longer to be used and eventually expected to be destroyed.
    */
    void cleanupClient();


protected:
   /**
    * Get qmi client type
    */
   qmi_client_type getClientHandle();

   /**
    * Initializes QMI Request and Response structures.
    * Also allocates memory to ResponseType pointer
    *
    * @param [in] request   QMI request object
    * @param [in] response  QMI response pointer
    *
    */
   template <typename RequestType, typename ResponseType>
   telux::common::Status mallocAndInitParams(RequestType &request, ResponseType *&response) {
      memset(&request, 0, sizeof(RequestType));
      response = (ResponseType *)malloc(sizeof(ResponseType));
      if(!response) {
         LOG(ERROR, "Unable to allocate memory");
         return telux::common::Status::FAILED;
      }
      memset(response, 0, sizeof(ResponseType));
      return telux::common::Status::SUCCESS;
   }

   /**
    * Utility method for sending request to QMI.
    */
   template <typename RequestType, typename ResponseType>
   telux::common::Status sendRequest(int cmdId, unsigned int qmiMessageId, RequestType &request,
                                     ResponseType *&response, void *userData) {
      // User data
      QmiUserData *qmiUserData = (QmiUserData *)malloc(sizeof(QmiUserData));
      if(qmiUserData == NULL) {
         LOG(ERROR, "Memory allocation failed");
         return telux::common::Status::FAILED;
      }
      memset(qmiUserData, 0, sizeof(QmiUserData));

      qmiUserData->data = userData;
      qmiUserData->qmiClient = this;
      qmiUserData->cmdCallbackId = cmdId;

      qmi_txn_handle txnHandle;
      qmi_client_error_type clientErr = QMI_NO_ERR;

      // Sending async request to QMI
      clientErr = qmi_client_send_msg_async(getClientHandle(), qmiMessageId, &request,
                                            sizeof(RequestType), response, sizeof(ResponseType),
                                            qmiAsyncResponseCallback, qmiUserData, &txnHandle);
      telux::common::ErrorCode errorCode
         = telux::common::ErrorHelper::qmiErrorToErrorCode(clientErr);

      LOG(DEBUG, __FUNCTION__, " Client error(", clientErr, ") errStr: ",
          telux::common::ErrorHelper::getQmiErrorAsString(clientErr), " Error Code: ",
          static_cast<int>(errorCode));

      if(clientErr) {
         LOG(ERROR, "Unable to send qmi message");
         if(NULL != qmiUserData) {
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
    *
    * @param [in] qmiMessageId    QMI message Id.
    * @param [in] request         QMI request object.
    * @param [in] response        QMI response pointer.
    * @param [in] callback        Command callback pointer.
    * @param [in] userData        Cookie data sent by the caller.
    *
    */
   template <typename RequestType, typename ResponseType>
   telux::common::Status sendAsyncRequest(unsigned int qmiMessageId, RequestType &request,
                                          ResponseType *&response,
                                          std::shared_ptr<telux::common::ICommandCallback> callback,
                                          void *userData) {
      LOG(DEBUG, __FUNCTION__);

      int cmdId = INVALID_COMMAND_ID;
      if(callback) {
         cmdId = cmdCallbackMgr_.addCallback(callback);
      }

      auto status = sendRequest(cmdId, qmiMessageId, request, response, userData);
      return status;
   }

   /**
    * Sends an Asynchronous request to QMI service.
    *
    * @param [in] qmiMessageId    QMI message Id.
    * @param [in] request         QMI request object.
    * @param [in] response        QMI response pointer.
    * @param [in] callback        Command callback pointer.
    * @param [in] userData        Cookie data sent by the caller.
    *
    */
   template <typename RequestType, typename ResponseType, typename... Args>
   telux::common::Status sendAsyncRequest(unsigned int qmiMessageId, RequestType &request,
                                          ResponseType *&response,
                                          std::function<void(Args...)> callback, void *userData) {
      LOG(DEBUG, __FUNCTION__);

      int cmdId = INVALID_COMMAND_ID;
      if(callback) {
         cmdId = cmdCallbackMgr_.addCallback(callback);
      }

      auto status = sendRequest(cmdId, qmiMessageId, request, response, userData);
      return status;
   }

   /**
    * Sends a synchronous request to QMI service.
    *
    * @param [in] qmiMessageId    QMI message Id.
    * @param [in] request         QMI request object.
    * @param [in] response        QMI response pointer.
    * @param [in] timeout         Timeout in seconds
    *
    */
   template <typename RequestType, typename ResponseType>
   telux::common::Status sendSyncRequest(unsigned int qmiMessageId, RequestType &request,
                                         ResponseType *&response,
                                         int timeout = DEFAULT_TIMEOUT_IN_MILLISECONDS) {
      LOG(DEBUG, __FUNCTION__);

      qmi_client_error_type clientErr = QMI_NO_ERR;

      // Sending async request to QMI
      clientErr
         = qmi_client_send_msg_sync(getClientHandle(), qmiMessageId, &request, sizeof(RequestType),
                                    response, sizeof(ResponseType), timeout);
      telux::common::ErrorCode errorCode
         = telux::common::ErrorHelper::qmiErrorToErrorCode(clientErr);

      LOG(DEBUG, __FUNCTION__, " Client error(", clientErr, ") errStr: ",
          telux::common::ErrorHelper::getQmiErrorAsString(clientErr), " Error Code: ",
          static_cast<int>(errorCode));

      if(clientErr) {
         LOG(ERROR, "Unable to send qmi message");
         return telux::common::Status::FAILED;
      }
      return telux::common::Status::SUCCESS;
   }

   /**
    * Fetches a list of registered listeners.
    */
   void getAvailableListeners(std::vector<std::shared_ptr<IQmiListener>> &listeners);

   /**
    * This function is called by the QCCI infrastructure when
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
   static void qmiIndicationCallback(qmi_client_type userHandle, unsigned int msgId, void *indBuf,
                                     unsigned int indBufLen, void *indCbData);

   /**
    * This function is called by the QCCI infrastructure when
    * infrastructure receives an asynchronous response for this client.
    *
    * NOTE: Callback happens in the QCCI thread context and raises signal.
    *
    * @param [in] userHandle           Opaque handle used by the infrastructure to
    *                                  identify different services.
    * @param [in] msgId                Message ID of the indication
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] respCbData           Cookie user data value supplied by the client
    * @param [in] transpErr            Error value
    */
   static void qmiAsyncResponseCallback(qmi_client_type userHandle, unsigned int msgId,
                                        void *respCStruct, unsigned int respCStructLen,
                                        void *respCbData, qmi_client_error_type transpErr);

   /**
    * This callback function is called by the QCCI infrastructure when the service terminates or
    * deregisters
    *
    * NOTE: Callback happens in the QCCI thread context.
    *
    * @param [in] userHandle           Opaque handle used by the infrastructure to
    *                                  identify different services.
    * @param [in] error                Error value
    * @param [in] errorCbData          Cookie user data value supplied by client
    */
   static void qmiClientErrorCallback(qmi_client_type userHandle, qmi_client_error_type error,
                                      void *errorCbData);

   /**
    * This function is called by qmiClientErrorCallback when the service
    * terminates or deregisters.
    *
    * NOTE: Callback happens on new thread created by std::async.
    *
    * @param [in] userHandle           Opaque handle used by the infrastructure to
    *                                  identify different services.
    * @param [in] errorCbData          Cookie user data value supplied by client
    * @param [in] error                Error value
    */
   void qmiClientErrorCallbackSync(qmi_client_type userHandle, qmi_client_error_type error);

   /**
    * This callback function is called by the QCCI infrastructure when a service
    * event occurs indicating that the service count has changed.
    *
    * NOTE: Callback happens in the QCCI thread context.
    *
    * @param [in]   userHandle         Handle of the client.
    * @param [in]   serviceObj         Service object.
    * @param [in]   serviceEvent       Event type
    * @param [in]   notifyCbData       User data passed in
    */
   static void qmiClientNotifierCallback(qmi_client_type userHandle,
                                         qmi_idl_service_object_type serviceObj,
                                         qmi_client_notify_event_type serviceEvent,
                                         void *notifyCbData);

   /**
    * This function is called by qmiClientNotifierCallback when a service event
    * occurs indicating that the service count has changed.
    *
    * NOTE: Callback happens on new thread created by std::async.
    *
    * @param [in]   userHandle         Handle of the client.
    * @param [in]   serviceObj         Service object.
    * @param [in]   serviceEvent       Event type
    */
   void qmiClientNotifierCallbackSync(qmi_client_type userHandle,
                                      qmi_idl_service_object_type serviceObj,
                                      qmi_client_notify_event_type serviceEvent);

   /**
    * Initializes QMI client notifier and registers notifier-callback with QMI framework, to get
    * notifications about QMI Service count increment/decrement events
    */
   telux::common::Status initNotifier();

   virtual int getModemPort();

   /**
    * Sets the service status
    *
    * @param [in]   status         Status of the service
    */
   void setServiceStatus(telux::common::ServiceStatus status);
   virtual bool waitForInitialization();

   qmi_client_os_params clientOsParams_;
   qmi_client_type qmiClientHandle_ = nullptr;
   qmi_client_type qmiNotifierHandle_ = nullptr;
   qmi_service_instance qmiServiceInstanceId_ = QMI_CLIENT_INSTANCE_ANY;
   qmi_idl_service_object_type idlServiceObject_;
   telux::common::CommandCallbackManager cmdCallbackMgr_;
   std::shared_ptr<telux::common::ListenerManager<IQmiListener>> listenerMgr_ = nullptr;
   std::shared_ptr<telux::common::TaskDispatcher> taskDispatcher_;

   std::mutex qmiClientMutex_;
   std::condition_variable cv_;
   // Check the readiness of the service
   telux::common::ServiceStatus serviceReady_ = telux::common::ServiceStatus::SERVICE_UNAVAILABLE;

   std::atomic<bool> exiting_;
};

}  // end namespace qmi
}  // end namespace telux

#endif  // QMICLIENT_HPP
