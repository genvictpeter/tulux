/*
 *  Copyright (c) 2018-2021, Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       VoiceQmiClient.hpp
 * @brief      QMI Client interacts with QMI voice service to send requests
 *             and dispatch received indications to respective listeners
 */

#ifndef VOICEQMICLIENT_HPP
#define VOICEQMICLIENT_HPP

extern "C" {
#include <qmi/voice_service_v02.h>
}
#include <bitset>

#include "QmiClient.hpp"

namespace telux {
namespace qmi {

// Forward declarations
class IQmiVoiceSetConfigCallback;
class IQmiECallRequestCallback;
class IQmiRegisterVoiceIndicationsCallback;
class IQmiSetECallOperatingModeCallback;
class IQmiGetECallOperatingModeCallback;
class IQmiGetECallHlapTimerStatusCallback;

/**
 * Defines Voice QMI service indications being used
 */
enum VoiceIndicationType {
   ECALL_STATUS,                /**< QMI_VOICE_ECALL_STATUS_IND_V02 */
   ECALL_HLAP_TIMER_STATUS,     /**< QMI_VOICE_ECALL_HLAP_TIMER_STATUS_IND_V02 */
   ECALL_OPERATING_MODE,        /**< QMI_VOICE_ECALL_OPRT_MODE_IND_V02 */
};

/**
 * 16 bit mask that denotes which of the Voice QMI service indication defined in VoiceIndicationType
 * are used to enable the indications.
 */
using VoiceIndicationMask = std::bitset<16>;

/**
 * This class provides mechanism to send messages to QMI voice service
 */
class VoiceQmiClient : public QmiClient {
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
    * Sends QMI request which sets various configuration parameters that control the modem behavior
    * related to circuit-switched services (Currently used for setting eCall MSD).
    *
    * @param [in] Msd             eCall MSD can contain up to 140-byte
    *     - ASN.1 unaligned PER data as described in \hyperref[CEN-EN-15722]{CEN EN 15722}.
    * @param [in] callback        Command Callback pointer
    * @param [in] userData        Cookie user data value supplied by the client
    *
    * @returns Status - success or suitable error code
    */
   telux::common::Status sendVoiceSetConfigRequest(
      std::vector<uint8_t> msdData, std::shared_ptr<IQmiVoiceSetConfigCallback> callback,
      void *userData = nullptr);

   /**
    * Sends QMI request for eCall
    *
    * @param [in] msdData       eCall Minimum Set of Data (MSD) can contain up to 140-byte
    *              ASN.1 unaligned PER data as described in \hyperref[CEN-EN-15722]{CEN EN 15722}.
    * @param [in] transmitMsd   Represents if MSD should be transmitted or not at call connect
    * @param [in] emergencyCategory denotes the call category
    *                     (ECallCatogery::VOICE_EMER_CAT_MANUAL = 32,
    *                      ECallCatogery::VOICE_EMER_CAT_AUTO_ECALL = 64)
    * @param [in] ecallVariant  Denotes the call variant enum which can take following values:
    *     - 0x01 -- ECALL_TEST (Originate VOICE call), Default
    *     - 0x02 -- ECALL_EMERGENCY (Originate EMERGENCY call)
    * @param [in] callType denotes the call type enum which can take following values:
    *     - 0x09 -- EmergencyCallType::ECALL_TYPE_EMERGENCY     -- Emergency
    *     - 0x0C -- EmergencyCallType::ECALL_TYPE_ECALL         -- eCall
    * @param [in] callback        Command Callback pointer
    * @param [in] userData        Cookie user data value supplied by the client
    *
    * @returns Status - success or suitable error code
    */
   telux::common::Status sendECallRequest(std::vector<uint8_t> msdData, bool transmitMsd,
                                          int emergencyCategory, int eCallVariant, int callType,
                                          std::string callingNumber,
                                          std::shared_ptr<IQmiECallRequestCallback> callback,
                                          void *userData = nullptr);

   /**
    * Sends QMI request for set eCall operating mode
    * @param [in] eCallMode can take following values:
    *     - NORMAL -- 0 (eCall and normal Voice call)
    *     - ECALL_ONLY -- 1 (Only eCall)
    *     - NONE -- 2 (eCall operating mode none)
    * @param [in] callback   SetECallOperatingMode callback pointer
    * @param [in] userData   Optional user data value supplied by the client
    * @returns Status - success or suitable error code
    */
   telux::common::Status sendSetECallOperatingMode(
      int eCallMode, std::shared_ptr<IQmiSetECallOperatingModeCallback> callback,
      void *userData = nullptr);

   /**
    * Sends QMI request for get eCall operating mode
    * @param [in] callback  GetECallOperatingMode callback pointer
    * @param [in] userData  Optional user data value supplied by the client
    * @returns Status - success or suitable error code
    */
   telux::common::Status sendGetECallOperatingMode(
      std::shared_ptr<IQmiGetECallOperatingModeCallback> callback, void *userData = nullptr);

   /**
    * Sends QMI request to get eCall HLAP Timers status
    * @param [in] callback  GetECallHlapTimerStatus callback pointer
    * @param [in] userData  Optional user data value supplied by the client
    * @returns Status - success or suitable error code
    */
   telux::common::Status sendGetECallHlapTimerStatus(
      std::shared_ptr<IQmiGetECallHlapTimerStatusCallback> callback, void *userData = nullptr);

   /**
    * Registers for eCall Status indication
    * @param [in] indMask         Set of voice QMI service indications to be enabled
                                  @ref VoiceIndicationMask
    * @param [in] callback        Command Callback pointer
    * @param [in] userData        Cookie user data value supplied by the client
    *
    * @returns Status - success or suitable error code
    */
   telux::common::Status registerForVoiceIndications(VoiceIndicationMask indMask,
     std::shared_ptr<qmi::IQmiRegisterVoiceIndicationsCallback> callback, void *userData = nullptr);

   bool isReady() override;
   telux::common::Status init(qmi_idl_service_object_type idlServiceObject);

   VoiceQmiClient(int slotId = DEFAULT_SLOT_ID);
   ~VoiceQmiClient();

private:
   int slotId_;
   std::atomic<bool> isBound_ = {false};
   telux::common::AsyncTaskQueue<void> taskQ_;

   void notifyECallStatus(voice_ecall_status_ind_msg_v02 *indData);
   void notifyECallHlapTimerStatus(voice_ecall_hlap_timer_status_ind_msg_v02 *indData);
   void notifyECallOperatingMode(voice_ecall_oprt_mode_ind_msg_v02 *indData);

   // deleting copy constructor, to implement VoiceQmiClient as singleton
   VoiceQmiClient(const VoiceQmiClient &) = delete;
   // deleting assigning operator , to implement VoiceQmiClient as singleton
   VoiceQmiClient &operator=(const VoiceQmiClient &) = delete;

   bool waitForInitialization() override;
   void initSync(void);
   void setBoundState(bool isBound);

   /**
    * Binds client to the subscription of the SIM in a specific slot
    * @param [in] slotId    Slot ID
    * @param [in] userData  Optional user data value supplied by the client
    * @returns Status - success or suitable error code
    */
   telux::common::Status bindSubscriptionRequest(int slotId, void *userData = nullptr);

   // Handler methods for responses to asynchronous requests
   void handleVoiceSetConfigResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                 qmi_client_error_type transpErr,
                                 std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleDialECallResponse(void *respCStruct, uint32_t respCStructLen, void *data,
                                qmi_client_error_type transpErr,
                                std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleECallIndRegisterResponse(void *respCStruct, uint32_t respCStructLen, void *data,
                                       qmi_client_error_type transpErr,
                                       std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleSetECallOprtModeEventResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                        qmi_client_error_type transpErr,
                                        std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleSetECallOperatingModeResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                        qmi_client_error_type transpErr,
                                        std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleGetECallOperatingModeResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                        qmi_client_error_type transpErr,
                                        std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleGetECallHlapTimerStatusResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                        qmi_client_error_type transpErr,
                                        std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleBindSubscriptionResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                   qmi_client_error_type transpErr,
                                   std::weak_ptr<telux::common::ICommandCallback> callback);

   // Handler methods for indications
   int handleECallStatusIndication(qmi_client_type userHandle, void *indBuf,
                                   unsigned int indBufLen);

   int handleECallHlapTimerStatusIndication(qmi_client_type userHandle, void *indBuf,
                                         unsigned int indBufLen);

   int handleECallOperatingModeIndication(qmi_client_type userHandle, void *indBuf,
                                          unsigned int indBufLen);
};

/**
 * Listener interface for QMI indication. Clients should implement
 * this interface and register with VoiceQmiClient in order to receive
 * indication for eCall status and eCall operating mode change.
 */
class IQmiVoiceListener : public IQmiListener {
public:
   /**
     * This function is called whenever there is change in eCall transmission status.
     *
     * @param [in] indData               eCall transmission status info.
     */
   virtual void onECallStatus(voice_ecall_status_ind_msg_v02 *indData, int slotId){};

   /**
     * This function is called whenever there is a change in eCall HLAP timer status.
     *
     * @param [in] indData               eCall HLAP timer status info.
     */
   virtual void onECallHlapTimerStatus(voice_ecall_hlap_timer_status_ind_msg_v02 *indData,
                            int slotId){};

   /**
    * This function is called whenever there is change in eCall operating mode.
    *
    * @param [in] indData               eCall operating mode change info.
    */
   virtual void onECallOprtMode(voice_ecall_oprt_mode_ind_msg_v02 *indData, int slotId){};
    /**
     * This function will be called whenever the QMI voice service status changes
     *
     * @param [in] status         service status
     */
    virtual void onVoiceServiceStatusChange(telux::common::ServiceStatus status, int slotId) {}
};

/**
 * Callback called in response to set voice configuration.
 */
class IQmiVoiceSetConfigCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [in] qmiError               QMI error
    * @param [in] userData               Cookie user data value supplied by the client
    */
   virtual void onVoiceSetConfigResponse(int qmiError, void *userData) = 0;
};

/**
 * Callback called in response to dial eCall.
 */
class IQmiECallRequestCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [in] respMsg                Voice dial call response message
    * @param [in] userData               Cookie user data value supplied by the client
    */
   virtual void onDialECallResponse(voice_dial_call_resp_msg_v02 *respMsg, void *userData) = 0;
};

/**
 * Callback called in response to register for Voice QMI service indications.
 */
class IQmiRegisterVoiceIndicationsCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [in] qmiError               QMI error
    */
   virtual void onRegisterResponse(int qmiError) = 0;
};

/**
 * Callback called in response to set eCall operating mode.
 */
class IQmiSetECallOperatingModeCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    * @param [in] qmiError              QMI error
    * @param [in] userData               Cookie user data value supplied by the client
    */
   virtual void onSetECallOperatingModeResponse(int qmiError, void *userData) = 0;
};

/**
 * Callback called in response to get eCall operating mode.
 */
class IQmiGetECallOperatingModeCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    * @param [in] eCallMode             eCall Operating mode
    * @param [in] qmiError              QMI error
    * @param [in] userData               Cookie user data value supplied by the client
    */
   virtual void onGetECallOperatingModeResponse(int eCallMode, int qmiError, void *userData) = 0;
};

/**
 * Callback called in response to get HLAP Timers status.
 */
class IQmiGetECallHlapTimerStatusCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    * @param [in] resp                  response indicating eCall HLAP timers status
    * @param [in] qmiError              QMI error
    * @param [in] slotId                Slot ID
    * @param [in] userData              Cookie user data value supplied by the client
    */
   virtual void onGetECallHlapTimerStatusResponse(
                                        voice_get_ecall_hlap_timer_status_resp_msg_v02 *respMsg,
                                        int qmiError, int slotId, void *userData) = 0;
};

}  // end namespace qmi
}  // end namespace telux

#endif  // VOICEQMICLIENT_HPP
