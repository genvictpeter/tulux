/*
 *  Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       UimQmiClient.hpp
 * @brief      QMI Client interacts with QMI user identity module service to send requests
 *             and dispatch received indications to respective listeners
 */

#ifndef UIMQMICLIENT_HPP
#define UIMQMICLIENT_HPP

#include <bitset>

extern "C" {
#include <qmi/user_identity_module_v01.h>
}

#include <telux/tel/SimProfileDefines.hpp>
#include "common/CommandCallbackManager.hpp"
#include "telux/common/CommonDefines.hpp"

#include "QmiClient.hpp"

/**
 * Structure to send the user data for each QMI request along with slot details.
 * The same userdata will come back in response which helps in identifying
 * response came corresponding to which slot.
 */
struct UimQmiUserData {
   void *data;
   int slotId = DEFAULT_SLOT_ID;
};

namespace telux {
namespace qmi {

// Forward declarations
class IQmiSapConnectionCallback;
class IQmiSapRequestCallback;

/**
 * Defines SAP connection operation type
 */
enum SapConnectOperation {
   SAP_CONNECT_OPERATION_DISCONNECT = 0x00,    /**< Perform SAP disconnect operation */
   SAP_CONNECT_OPERATION_CONNECT = 0x01,       /**< Perform SAP connect operation */
   SAP_CONNECT_OPERATION_CHECK_STATUS = 0x02   /**< Perform SAP operation Status check */
};

/**
 * Defines SAP request type
 */
enum SapRequestType {
   SAP_REQUEST_GET_ATR = 0x00,           /**<  Gets the ATR */
   SAP_REQUEST_SEND_APDU = 0x01,         /**<  Sends the APDU */
   SAP_REQUEST_POWER_SIM_OFF = 0x02,     /**<  Powers off the SIM */
   SAP_REQUEST_POWER_SIM_ON = 0x03,      /**<  Powers on the SIM */
   SAP_REQUEST_RESET_SIM = 0x04,         /**<  Resets the SIM */
   SAP_REQUEST_CARD_READER_STATUS = 0x05 /**<  Read status of card reader */
};

/**
 * Defines UIM QMI service indications being used
 */
enum UimIndicationType {
   PHYSICAL_SLOT_STATUS = 4,      /**< QMI_UIM_SLOT_STATUS_CHANGE_IND_V01 */
};

/**
 * 16 bit mask that denotes which of the UIM QMI service indications defined in UimIndicationType
 * are used to enable the indications.
 */
using UimIndicationMask = std::bitset<16>;

/**
 * Indicates profile information of eUICC card
 */
struct ProfileInfo {
    int profileId;                       /**< Profile ID */
    bool active;                         /**< Profile active or not */
    telux::tel::ProfileType profileType; /**< Profile Type */
};

/**
 * This function is called with the response to sendGetEidReq API.
 *
 * @param [out] resp               Pointer to UIM get EID request response structure
 * @param [out] userData           Cookie user data value supplied by the client
 * @param [out] transpErr          Transport error
 */
using QmiGetEidCallback = std::function<void(
   uim_get_eid_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendSwitchActiveSlotRequest API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [out] qmiError     QMI error
 * @param [out] userData     Cookie user data value supplied by the client
 * @param [out] transpErr    Transport error
 *
 * @note   Eval: This is a new API and is being evaluated. It is subject to
 *         change and could break backwards compatibility.
 */
using SwitchActiveSlotCallback = std::function<void(int qmiError, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendGetSlotStatusRequest API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [out] resp         QMI UIM response message for slot status request
 * @param [out] userData     Cookie user data value supplied by the client
 * @param [out] transpErr    Transport error
 *
 * @note   Eval: This is a new API and is being evaluated. It is subject to
 *         change and could break backwards compatibility.
 */
using GetSlotStatusCallback = std::function<void(uim_get_slots_status_resp_msg_v01 *resp,
                                void *userData, int transpErr)>;

/**
 * This function is called with the response to registerForUimIndications API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [out] qmiError     QMI error
 * @param [out] transpErr    Transport error
 *
 * @note   Eval: This is a new API and is being evaluated. It is subject to
 *         change and could break backwards compatibility.
 */
using RegisterUimIndicationsCallback = std::function<void(int qmiError, int transpErr)>;

/**
 * This function is called with the response to registerForUimRefreshIndication API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [out] qmiError     QMI error
 * @param [out] transpErr    Transport error
 *
 * @note   Eval: This is a new API and is being evaluated. It is subject to
 *         change and could break backwards compatibility.
 */
using RegisterUimSimRefreshIndCallback = std::function<void(int qmiError, int transpErr)>;

/**
 * This function is called with the response to sendAddProfileReq API.
 *
 * @param [out] resp               UIM add profile response message
 * @param [out] userData           Cookie user data value supplied by the client
 * @param [out] transpErr          Transport error
 * @note    Eval: This is a new API and is being evaluated. It is subject to change and could
 *          break backwards compatibility.
 */
using QmiAddProfileCallback
    = std::function<void(uim_add_profile_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendDeleteProfileReq API.
 *
 * @param [out] resp               UIM delete profile response message
 * @param [out] userData           Cookie user data value supplied by the client
 * @param [out] transpErr          Transport error
 */
using QmiDeleteProfileCallback
    = std::function<void(uim_delete_profile_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendGetSimProfileListReq API.
 *
 * @param [out] resp               UIM get profile list response message
 * @param [out] userData           Cookie user data value supplied by the client
 * @param [out] transpErr          Transport error
 */
using QmiGetSimProfileListCallback
    = std::function<void(uim_get_sim_profile_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendGetProfileInfoReq API.
 *
 * @param [out] resp               UIM get profile info response message
 * @param [out] userData           Cookie user data value supplied by the client
 * @param [out] transpErr          Transport error
 */
using QmiGetProfileInfoCallback
    = std::function<void(uim_get_profile_info_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendSetProfileReq API.
 *
 * @param [out] resp               UIM set profile response message
 * @param [out] userData           Cookie user data value supplied by the client
 * @param [out] transpErr          Transport error
 */
using QmiSetProfileCallback
    = std::function<void(uim_set_sim_profile_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendUpdateNickNameReq API.
 *
 * @param [out] resp               UIM update profile nickname response message
 * @param [out] userData           Cookie user data value supplied by the client
 * @param [out] transpErr          Transport error
 */
using QmiUpdateNickNameCallback = std::function<void(
    uim_update_profile_nickname_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendUserConsentReq API.
 *
 * @param [out] resp               UIM user consent response message
 * @param [out] userData           Cookie user data value supplied by the client
 * @param [out] transpErr          Transport error
 */
using QmiUserConsentCallback = std::function<void(
    uim_profile_user_consent_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendCardPowerUp and sendCardPowerDown APIs.
 *
 * @param [out] qmiError           QMI error
 * @param [out] userData           Cookie user data value supplied by the client
 * @param [out] transpErr          Transport error
 */
using cardPowerCallback = std::function<void(int qmiError, void *userData, int transpErr)>;


/**
 * This function is called with the response to sendConfirmationCodeRequest API.
 *
 * @param [out] resp               UIM user confirmation code response message
 * @param [out] userData           Cookie user data value supplied by the client
 * @param [out] transpErr          Transport error
 */
using QmiConfirmationCodeCallback = std::function<void(
    uim_profile_confirmation_code_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendSetEuiccServerAddressReq API.
 *
 * @param [in] resp               Set eUICC server address (SMDP+) response message
 * @param [in] userData           Cookie user data value supplied by the client
 * @param [in] transpErr          Transport error
 */
using QmiSetEuiccServerAddressCallback = std::function<void(
    uim_euicc_default_server_address_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendGetEuiccServerAddressReq API.
 *
 * @param [in] resp               Get eUICC server address (SMDP+ / SMDS) response message
 * @param [in] userData           Cookie user data value supplied by the client
 * @param [in] transpErr          Transport error
 */
using QmiGetEuiccServerAddressCallback = std::function<void(
    uim_euicc_default_server_address_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This function is called with the response to sendReadTransparentRequest API.
 *
 * @param [out] resp               UIM read transparent response message
 * @param [out] userData           Cookie user data value supplied by the client
 * @param [out] transpErr          Transport error
 */
using QmiReadTransparentCallback = std::function<void(uim_read_transparent_resp_msg_v01 *respMsg,
    void *userData, int transpErr)>;

/**
 * This function is called with the response to sendEuiccMemoryResetReq API.
 *
 * @param [in] resp               Reset memory response message
 * @param [in] userData           Cookie user data value supplied by the client
 * @param [in] transpErr          Transport error
 */
using QmiEuiccMemoryResetCallback = std::function<void(
    uim_euicc_memory_reset_resp_msg_v01 *resp, void *userData, int transpErr)>;

/**
 * This class provides mechanism to send messages to QMI user identity module
 */
class UimQmiClient : public QmiClient {
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
    * Send SAP request to get SAP state
    *
    * @deprecated Use sendSapConnectionStatusRequest() API below to request SAP state
    */
   telux::common::Status sendSapStateRequest(SapConnectOperation sapConnOp, int &sapState,
                                             int slotId);

   /**
    * Send SAP connection status request
    */
   telux::common::Status sendSapConnectionStatusRequest(
      int slotId, std::shared_ptr<IQmiSapConnectionCallback> callback, void *userData);

   /**
    * Send SAP open connection request
    */
   telux::common::Status sendSapOpenConnectionRequest(
      int slotId, int condition, std::shared_ptr<IQmiSapConnectionCallback> callback,
      void *userData);

   /**
    * Send SAP close connection request
    */
   telux::common::Status sendSapCloseConnectionRequest(
      int slotId, std::shared_ptr<IQmiSapConnectionCallback> callback, void *userData);

   /**
    * Send SAP ATR request
    */
   telux::common::Status sendAtrRequest(int slotIndex,
                                        std::shared_ptr<IQmiSapRequestCallback> callback,
                                        void *userData);
   /**
    * Send SAP transmit APDU request
    */
   telux::common::Status sendTransmitApduRequest(int slotIndex, std::vector<uint8_t> apdu,
                                                 std::shared_ptr<IQmiSapRequestCallback> callback,
                                                 void *userData);

   /**
    * Send SAP SIM operations request
    */
   telux::common::Status sendSimRequest(int slotIndex, SapRequestType sapRequestType,
                                        std::shared_ptr<IQmiSapRequestCallback> callback,
                                        void *userData);

   /**
    * Send read transparent request for requested file info like for ICCID, IMSI, etc.
    */
   telux::common::Status sendReadTransparentRequest(uim_read_transparent_req_msg_v01 readTranspReq,
      QmiReadTransparentCallback callback, void *userData, int slotId);

   /**
    * Retrieve slot count for number of SIM slots
    */
   telux::common::Status getSlotCount(int &slotCount);

   /**
    * Get unique identifier of the eUICC card.
    *
    * @param [in] slotId                   Unique identifier for the SIM slot.
    * @param [in] callback                 Callback to receive response for get EID request.
    * @param [in] userData                 Cookie user data value supplied by the client.
    *
    * @returns Status of get EID request i.e. success or suitable error code.
    */
   telux::common::Status sendGetEidReq(int slotId,
                                       QmiGetEidCallback callback,
                                       void *userData = nullptr);

   /**
    * Send request to set active Physical slot on Dual-Sim Single Active platforms
    *
    * @param [in] slotId                   Physical SlotId to be made active
    * @param [in] callback                 Command Callback pointer
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of sendSwitchActiveSlotRequest i.e. success or suitable error code.
    */
   telux::common::Status sendSwitchActiveSlotRequest(SlotId slotId,
      SwitchActiveSlotCallback callback, void *userData = nullptr);

   /**
    * Send request to get physical slots status
    *
    * @param [in] callback                 Command Callback pointer
    * @param [in] userData                 Cookie user data value supplied by the client
    *
    * @returns Status of sendGetSlotStatusRequest i.e. success or suitable error code.
    */
   telux::common::Status sendGetSlotStatusRequest(GetSlotStatusCallback callback,
      void *userData = nullptr);

   /**
    * Register for a specific set of QMI indications supported by UimQmiClient
    *
    * @param [in] indMask                  Set of indications to be enabled
    *                                      @ref UimIndicationMask
    * @param [in] callback                 Command Callback pointer
    *
    * @returns Status of registerForUimIndications i.e. success or suitable error code.
    */
   telux::common::Status registerForUimIndications(UimIndicationMask indMask,
                                                   RegisterUimIndicationsCallback callback);

   /**
    * Register for a UIM SIM refresh indication during reset mode for UIM Session Type
    * UIM_SESSION_TYPE_CARD_ON_SLOT_1_V01 and UIM_SESSION_TYPE_CARD_ON_SLOT_2_V01
    *
    * @param [in] slotId                   Unique identifier for the SIM slot.
    * @param [in] callback                 Command Callback pointer
    *
    * @returns Status of registerForUimRefreshIndication i.e. success or suitable error code.
    */
   telux::common::Status registerForUimRefreshIndication(SlotId slotId,
     RegisterUimSimRefreshIndCallback callback);

   /**
    * Add profile on the card
    *
    * @param [in] slotId                   Unique identifier for the SIM slot.
    * @param [in] activationCode           Activation code required for downloading profile.
    *                                      length shouldn't be exceeding 256.
    * @param [in] confirmationCode         Confirmation Code.
    * @param [in] isUserConsentRequired    User consent required or not.
    * @param [in] callback                 Callback to receive response for add profile.
    * @param [in] userData                 Cookie user data value supplied by the client.
    *
    * @returns Status of add profile request i.e. success or suitable error code.
    * @note    Eval: This is a new API and is being evaluated. It is subject to change and could
    *          break backwards compatibility.
    */
   telux::common::Status sendAddProfileReq(SlotId slotId, const std::string &activationCode,
        const std::string &confirmationCode = "", const bool isUserConsentRequired = false,
        QmiAddProfileCallback callback = nullptr, void *userData = nullptr);

   /**
    * Delete profile from the eUICC card
    *
    * @param [in] slotId           Unique identifier for the SIM slot
    * @param [in] profileId        Profile identifier
    * @param [in] callback         Callback to receive response for delete profile request.
    * @param [in] userData         Cookie user data value supplied by the client.
    *
    * @returns Status of delete profile i.e. success or suitable error code.
    */
   telux::common::Status sendDeleteProfile(SlotId slotId, int profileId,
        QmiDeleteProfileCallback callback = nullptr, void *userData = nullptr);

   /**
    * Enable/disable profile on the eUICC card
    *
    * @param [in] slotId           Unique identifier for the SIM slot
    * @param [in] profileId        Profile identifier
    * @param [in] enable           Enable or disable profile on card
    * @param [in] callback         Callback to receive response for set profile request.
    * @param [in] userData         Cookie user data value supplied by the client.
    *
    * @returns Status of setProfileRequest i.e. success or suitable error code.
    */
   telux::common::Status sendSetProfileReq(SlotId slotId, int profileId, bool enable,
        QmiSetProfileCallback callback = nullptr, void *userData = nullptr);

   /**
    * Retrieve profile list information present on the eUICC card
    *
    * @param [in] slotId           Unique identifier for the SIM slot
    * @param [in] profileId        Profile identifier
    * @param [in] callback         Callback to receive response for get profile info request.
    * @param [in] userData         Cookie user data value supplied by the client.
    *
    * @returns Status of get profile information request i.e. success or suitable error code.
    */
   telux::common::Status sendGetProfileInfoReq(SlotId slotId, int profileId,
        QmiGetProfileInfoCallback callback, void *userData = nullptr);

   /**
    * Retrieve available list of profiles on the eUICC card
    *
    * @param [in] slotId           Unique identifier for the SIM slot
    * @param [in] callback         Callback to receive response for get profile list request.
    * @param [in] userData         Cookie user data value supplied by the client.
    *
    * @returns Status of get profile list request i.e. success or suitable error code.
    */
   telux::common::Status sendGetSimProfileListReq(SlotId slotId,
        QmiGetSimProfileListCallback callback, void *userData = nullptr);

   /**
    * Update profile nickname on eUICC card.
    *
    * @param [in] slotId           Unique identifier for the SIM slot
    * @param [in] profileId        Profile identifier
    * @param [in] nickName         Profile nickName.length shouldn't be exceeding 64.
    * @param [in] callback         Callback to receive response for update nickname request.
    * @param [in] userData         Cookie user data value supplied by the client.
    *
    * @returns Status of update nickname request i.e. success or suitable error code.
    */
   telux::common::Status sendUpdateNickNameReq(SlotId slotId, int profileId,
        const std::string &nickName, QmiUpdateNickNameCallback callback = nullptr,
        void *userData = nullptr);

   /**
    *  Provides user consent for proﬁle download and installation when UIM_ADD_PROFILE_IND is
    *  received with status UIM_PROFILE_USER_CONSENT_REQUIRED.
    *
    * @param [in] slotId           Unique identifier for the SIM slot.
    * @param [in] userConsent      User consent.
    * @param [in] reason           Reason for user consent not provided.
    * @param [in] callback         Callback to receive response for get user consent request.
    * @param [in] userData         Cookie user data value supplied by the client.
    *
    * @returns Status of get user consent request i.e. success or suitable error code.
    */
   telux::common::Status sendUserConsent(SlotId slotId, bool isUserConsent, int reason,
        QmiUserConsentCallback callback = nullptr, void *userData = nullptr);

   /**
    *  Power on the SIM card.
    *
    * @param [in] powerUpReq       Indicates the information like slot id which needs to be
    *                              powered up.
    * @param [in] callback         Callback to receive response for sendCardPowerUp.
    * @param [in] userData         Cookie user data value supplied by the client.
    *
    * @returns Status of sendCardPowerUp i.e. success or suitable error code.
    */
   telux::common::Status sendCardPowerUp(uim_power_up_req_msg_v01 powerUpReq,
        cardPowerCallback callback = nullptr, void *userData = nullptr);

   /**
    *  Power off the SIM card.
    *
    * @param [in] powerDownReq     Indicates the information like slot id which needs to be
    *                              powered off.
    * @param [in] callback         Callback to receive response for sendCardPowerDown.
    * @param [in] userData         Cookie user data value supplied by the client.
    *
    * @returns Status of sendCardPowerDown i.e. success or suitable error code.
    */
   telux::common::Status sendCardPowerDown(uim_power_down_req_msg_v01 powerDownReq,
        cardPowerCallback callback = nullptr, void *userData = nullptr);


   /**
    *  Provides confirmation code for profile download and installation when UIM_ADD_PROFILE_IND is
    *  received with status UIM_PROFILE_CONFIRMATION_CODE_REQUIRED.
    *
    * @param [in] slotId           Unique identifier for the SIM slot.
    * @param [in] code             Confirmation code.
    * @param [in] callback         Callback to receive response for get confirmation code request.
    * @param [in] userData         Cookie user data value supplied by the client.
    *
    * @returns Status of get user consent request i.e. success or suitable error code.
    */
   telux::common::Status sendConfirmationCodeRequest(SlotId slotId, std::string code,
        QmiConfirmationCodeCallback callback = nullptr, void *userData = nullptr);

   /**
    *  Configures the default SM-DP+ address on the eUICC.
    *
    * @param [in] slotId           Unique identifier for the SIM slot.
    * @param [in] smdpAddress      SMDP+ address to be configured on eUICC.
    * @param [in] callback         Callback to receive response for set eUICC server address
    *                              request.
    * @param [in] userData         Cookie user data value supplied by the client.
    *
    * @returns Status of set eUICC server address request i.e. success or suitable error code.
    */
   telux::common::Status sendSetEuiccServerAddressReq(SlotId slotId, std::string smdpAddress,
        QmiSetEuiccServerAddressCallback callback = nullptr, void *userData = nullptr);

   /**
    *  Retrieves the default SM-DS address and the default SM-DP+ address from the eUICC.
    *
    * @param [in] slotId           Unique identifier for the SIM slot.
    * @param [in] callback         Callback to receive response for get eUICC server address
    *                              request.
    * @param [in] userData         Cookie user data value supplied by the client.
    *
    * @returns Status of get eUICC server address request i.e. success or suitable error code.
    */
   telux::common::Status sendGetEuiccServerAddressReq(SlotId slotId,
      QmiGetEuiccServerAddressCallback callback, void *userData = nullptr);

   /**
    * Resets the memory of the eUICC card based on the reset mask.
    *
    * @param [in] slotId           Unique identifier for the SIM slot.
    * @param [in] mask             Reset mask for options such as UIM_RESET_TEST_PROFILES,
    *                              UIM_RESET_OPERATIONAL_PROFILES and
    *                              UIM_RESET_TO_DEFAULT_SMDP_ADDRESS.
    * @param [in] callback         Callback to receive response for reset memory
    *                              request.
    * @param [in] userData         Cookie user data value supplied by the client.
    *
    * @returns Status of reset memory request i.e. success or suitable error code.
    */
   telux::common::Status sendEuiccMemoryResetReq(SlotId slotId, uint64_t resetMask,
      QmiEuiccMemoryResetCallback callback, void *userData = nullptr);

    UimQmiClient();
    ~UimQmiClient();

private:
   // deleting copy constructor, to implement UimQmiClient as singleton
   UimQmiClient(const UimQmiClient &) = delete;
   // deleting assigning operator , to implement UimQmiClient as singleton
   UimQmiClient &operator=(const UimQmiClient &) = delete;

   // Handler methods for responses to asynchronous requests
   void handleSapConnectionResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                qmi_client_error_type transpErr,
                                std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleSapRequestResp(void *respCStruct, uint32_t respCStructLen, void *data,
                             qmi_client_error_type transpErr,
                             std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleReadTransparentRequestResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                         qmi_client_error_type transpErr,
                                         std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleEidResp(void *respCStruct, uint32_t respCStructLen, void *data,
                      qmi_client_error_type transpErr,
                      std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleSwitchSlotResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                 qmi_client_error_type transpErr,
                                 std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleGetSlotStatusResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                 qmi_client_error_type transpErr,
                                 std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleEventRegResp(void *respCStruct, uint32_t respCStructLen, void *data,
                                 qmi_client_error_type transpErr,
                                 std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleSlotStatusIndication(qmi_client_type userHandle, void *indBuf,
                                            unsigned int indBufLen);
   void handleSetProfileResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleGetProfileInfoResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleGetSimProfileListResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleUpdateNickNameResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleAddProfileResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleDeleteProfileResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleUserConsentResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleCardPowerUpResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleCardPowerDownResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleConfirmationCodeResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleEuiccServerAddressResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleRefreshRegResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);
   void handleEuiccMemoryResetResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr, std::weak_ptr<telux::common::ICommandCallback> callback);
   // Indication handlers
   int handleAddProfileInd(qmi_client_type userHandle, void *indBuf, unsigned int indBufLen);
   int handleSimRefreshInd(qmi_client_type userHandle, void *indBuf, unsigned int indBufLen);

   void notifyAddProfileInd(const uim_add_profile_ind_msg_v01 &indData);
   void notifySimRefreshInd(const uim_refresh_ind_msg_v01 &indData);

   telux::common::CommandCallbackManager cmdCallbackMgr_;
};

/**
 * Callback class for SAP connection response.
 */
class IQmiSapConnectionCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] resp               Pointer to QMI SAP connection response structure
    * @param [out] userData           Cookie user data value supplied by the client
    */
   virtual void onSapConnectionResponse(uim_sap_connection_resp_msg_v01 *resp, void *userData) = 0;
};

/**
 * Callback class for SAP request response.
 */
class IQmiSapRequestCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] resp               Pointer to QMI SAP request response structure
    * @param [out] userData           Cookie user data value supplied by the client
    */
   virtual void onSapRequestResponse(uim_sap_request_resp_msg_v01 *resp, void *userData) = 0;
};

/**
 * Listener interface for QMI indications. Clients should implement
 * this interface and register with UimQmiClient in order to receive
 * indications.
 */
class IQmiUimListener : public IQmiListener {
public:
   /**
    * This function will be called whenever the QMI UIM service status changes
    *
    * @param [in] status         service status
    */
   virtual void onUimServiceStatusChange(telux::common::ServiceStatus status) {}

   virtual void onSlotStatusChanged(uim_slot_status_change_ind_msg_v01 indData) {}

   /**
    * This function is called to indicate the progress of the new profile
    *
    * @param [in] ind      Add profile indication
    */
   virtual void onAddProfileInd(const uim_add_profile_ind_msg_v01 &ind) {}

   /**
    * This function is called to indicate the SIM refresh due to reset mode
    * @param [in] ind      SIM Refresh indication
    */
   virtual void onSimRefreshInd(const uim_refresh_ind_msg_v01 &ind) {}

   virtual ~IQmiUimListener() {}
};

}  // end namespace qmi
}  // end namespace telux

#endif  // UIMQMICLIENT_HPP
