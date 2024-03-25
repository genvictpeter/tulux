/*
 *  Copyright (c) 2017-2018,2020-2021 The Linux Foundation. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *    * Neither the name of The Linux Foundation nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 *  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 *  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 *  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 *  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file  CallManager.hpp
 * @brief Call Manager is the primary interface for performing call related
 *        operations. Allows to conference calls, swap calls, make normal
 *        voice call and emergency call, send and update MSD pdu. Registers
 *        the listener and notify about incoming call, call info change and
 *        eCall MSD transmission status change to listener.
 *
 */

#ifndef CALLMANAGER_HPP
#define CALLMANAGER_HPP

#include <memory>
#include <string>
#include <vector>

#include <telux/tel/Call.hpp>
#include <telux/tel/CallListener.hpp>
#include <telux/tel/ECallDefines.hpp>
#include <telux/tel/PhoneDefines.hpp>

#include <telux/common/CommonDefines.hpp>

namespace telux {

namespace tel {

/** @addtogroup telematics_call
 * @{ */

class IMakeCallCallback;

/**
 * This function is called with the response to make normal call and
 * emergency call.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [out] error  @ref ErrorCode
 * @param [out] call   Pointer to Call object or nullptr in case of failure
 *
 */
using MakeCallCallback
   = std::function<void(telux::common::ErrorCode error, std::shared_ptr<ICall> call)>;

/**
 * This function is called with response to request for eCall High Level Application Protocol(HLAP)
 * timers status.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [out] error         @ref ErrorCode
 * @param [out] phoneId       Represents the phone corresponding to which the response is being
 *                            reported.
 * @param [out] timersStatus  @ref ECallHlapTimerStatus
 *
 */
using ECallHlapTimerStatusCallback = std::function<void(telux::common::ErrorCode error, int phoneId,
                                                        ECallHlapTimerStatus timersStatus)>;

/**
 * This function is called with response to request for ECBM(requestEcbm API).
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [out] ecbMode       Indicates the status of the ECBM.
 *                            @ref EcbMode
 * @param [out] error         @ref ErrorCode
 *
 * @note     Eval: This is a new API and is being evaluated.It is subject to change and could
 *           break backwards compatibility.
 */
using EcbmStatusCallback
    = std::function<void(telux::tel::EcbMode ecbMode, telux::common::ErrorCode error)>;

/**
 * @brief Call Manager is the primary interface for call related operations
 *        Allows to conference calls, swap calls, make normal voice call and
 *        emergency call, send and update MSD pdu.
 */
class ICallManager {
public:
   /**
    * Initiate a voice call. This API can also be used for e911/e112 type of regular emergency call.
    * This is not meant for an automotive eCall.
    *
    * @param [in] phoneId      Represents phone corresponding to which on make
    *                          call operation is performed
    * @param [in] dialNumber   String representing the dialing number
    * @param [in] callback     Optional callback pointer to get the response of
    *                          makeCall request.
    *                          Possible(not exhaustive) error codes for callback response
    *                          - @ref telux::common::ErrorCode::SUCCESS
    *                          - @ref telux::common::ErrorCode::RADIO_NOT_AVAILABLE
    *                          - @ref telux::common::ErrorCode::DIAL_MODIFIED_TO_USSD
    *                          - @ref telux::common::ErrorCode::DIAL_MODIFIED_TO_SS
    *                          - @ref telux::common::ErrorCode::DIAL_MODIFIED_TO_DIAL
    *                          - @ref telux::common::ErrorCode::INVALID_ARGUMENTS
    *                          - @ref telux::common::ErrorCode::NO_MEMORY
    *                          - @ref telux::common::ErrorCode::INVALID_STATE
    *                          - @ref telux::common::ErrorCode::NO_RESOURCES
    *                          - @ref telux::common::ErrorCode::INTERNAL_ERR
    *                          - @ref telux::common::ErrorCode::FDN_CHECK_FAILURE
    *                          - @ref telux::common::ErrorCode::MODEM_ERR
    *                          - @ref telux::common::ErrorCode::NO_SUBSCRIPTION
    *                          - @ref telux::common::ErrorCode::NO_NETWORK_FOUND
    *                          - @ref telux::common::ErrorCode::INVALID_CALL_ID
    *                          - @ref telux::common::ErrorCode::DEVICE_IN_USE
    *                          - @ref telux::common::ErrorCode::MODE_NOT_SUPPORTED
    *                          - @ref telux::common::ErrorCode::ABORTED
    *                          - @ref telux::common::ErrorCode::GENERIC_FAILURE
    *
    * @returns Status of makeCall i.e. success or suitable status code.
    */
   virtual telux::common::Status makeCall(int phoneId, const std::string &dialNumber,
                                          std::shared_ptr<IMakeCallCallback> callback = nullptr)
      = 0;

   /**
    * Initiate an automotive eCall.
    *
    * @param [in] phoneId      Represents phone corresponding to which make
    *                          eCall operation is performed
    * @param [in] eCallMsdData The structure containing required fields to
    *                          create eCall Minimum Set of Data (MSD)
    * @param [in] category     @ref ECallCategory
    * @param [in] variant      @ref ECallVariant
    * @param [in] callback     Optional callback pointer to get the response of
    *                          makeECall request.
    *                          Possible(not exhaustive) error codes for callback response
    *                          - @ref telux::common::ErrorCode::SUCCESS
    *                          - @ref telux::common::ErrorCode::RADIO_NOT_AVAILABLE
    *                          - @ref telux::common::ErrorCode::NO_MEMORY
    *                          - @ref telux::common::ErrorCode::MODEM_ERR
    *                          - @ref telux::common::ErrorCode::INTERNAL_ERR
    *                          - @ref telux::common::ErrorCode::INVALID_STATE
    *                          - @ref telux::common::ErrorCode::INVALID_CALL_ID
    *                          - @ref telux::common::ErrorCode::INVALID_ARGUMENTS
    *                          - @ref telux::common::ErrorCode::OPERATION_NOT_ALLOWED
    *                          - @ref telux::common::ErrorCode::GENERIC_FAILURE
    *
    * @returns Status of makeECall i.e. success or suitable status code.
    */
   virtual telux::common::Status makeECall(int phoneId, const ECallMsdData &eCallMsdData,
                                           int category, int variant,
                                           std::shared_ptr<IMakeCallCallback> callback = nullptr)
      = 0;

   /**
    * Initiate an automotive eCall to the specified phone number for TPS eCall. It will be
    * treated like a regular voice call by the UE and the network.
    *
    * @param [in] phoneId      Represents phone corresponding to which make
    *                          eCall operation is performed
    * @param [in] dialNumber   String representing the dialing number
    * @param [in] eCallMsdData The structure containing required fields to
    *                          create eCall Minimum Set of Data (MSD)
    * @param [in] category     @ref ECallCategory
    * @param [in] callback     Optional callback pointer to get the response of
    *                          makeECall request.
    *                          Possible(not exhaustive) error codes for callback response
    *                          - @ref telux::common::ErrorCode::SUCCESS
    *                          - @ref telux::common::ErrorCode::RADIO_NOT_AVAILABLE
    *                          - @ref telux::common::ErrorCode::NO_MEMORY
    *                          - @ref telux::common::ErrorCode::MODEM_ERR
    *                          - @ref telux::common::ErrorCode::INTERNAL_ERR
    *                          - @ref telux::common::ErrorCode::INVALID_STATE
    *                          - @ref telux::common::ErrorCode::INVALID_CALL_ID
    *                          - @ref telux::common::ErrorCode::INVALID_ARGUMENTS
    *                          - @ref telux::common::ErrorCode::OPERATION_NOT_ALLOWED
    *                          - @ref telux::common::ErrorCode::GENERIC_FAILURE
    *
    * @returns Status of makeECall i.e. success or suitable status code.
    *
    */
   virtual telux::common::Status makeECall(int phoneId, const std::string dialNumber,
                                           const ECallMsdData &eCallMsdData, int category,
                                           std::shared_ptr<IMakeCallCallback> callback = nullptr)
      = 0;

   /**
    * Initiate an automotive eCall with raw MSD pdu.
    *
    * @param [in] phoneId   Represents phone corresponding to which on make eCall
    *                       operation is performed
    * @param [in] msdPdu    Encoded MSD(Minimum Set of Data) PDU as per spec EN
    *                       15722 2015 or GOST R 54620-2011/33464-2015
    * @param [in] category  @ref ECallCategory
    * @param [in] variant   @ref ECallVariant
    * @param [in] callback  Callback function to get the response of makeECall
    *                       request.
    *                       Possible(not exhaustive) error codes for callback response
    *                       - @ref telux::common::ErrorCode::SUCCESS
    *                       - @ref telux::common::ErrorCode::RADIO_NOT_AVAILABLE
    *                       - @ref telux::common::ErrorCode::NO_MEMORY
    *                       - @ref telux::common::ErrorCode::MODEM_ERR
    *                       - @ref telux::common::ErrorCode::INTERNAL_ERR
    *                       - @ref telux::common::ErrorCode::INVALID_STATE
    *                       - @ref telux::common::ErrorCode::INVALID_CALL_ID
    *                       - @ref telux::common::ErrorCode::INVALID_ARGUMENTS
    *                       - @ref telux::common::ErrorCode::OPERATION_NOT_ALLOWED
    *                       - @ref telux::common::ErrorCode::GENERIC_FAILURE
    *
    * @returns Status of makeECall i.e. success or suitable status code.
    */
   virtual telux::common::Status makeECall(int phoneId, const std::vector<uint8_t> &msdPdu,
                                           int category, int variant,
                                           MakeCallCallback callback = nullptr)
      = 0;

   /**
    * Initiate an automotive eCall with raw MSD pdu, to the specified phone number for TPS eCall. It
    * will be treated like a regular voice call by the UE and the network.
    *
    * @param [in] phoneId   Represents phone corresponding to which on make eCall
    *                       operation is performed
    * @param [in] dialNumber   String representing the dialing number
    * @param [in] msdPdu    Encoded MSD(Minimum Set of Data) PDU as per spec EN
    *                       15722 2015 or GOST R 54620-2011/33464-2015
    * @param [in] category  @ref ECallCategory
    * @param [in] callback  Callback function to get the response of makeECall
    *                       request.
    *                       Possible(not exhaustive) error codes for callback response
    *                       - @ref telux::common::ErrorCode::SUCCESS
    *                       - @ref telux::common::ErrorCode::RADIO_NOT_AVAILABLE
    *                       - @ref telux::common::ErrorCode::NO_MEMORY
    *                       - @ref telux::common::ErrorCode::MODEM_ERR
    *                       - @ref telux::common::ErrorCode::INTERNAL_ERR
    *                       - @ref telux::common::ErrorCode::INVALID_STATE
    *                       - @ref telux::common::ErrorCode::INVALID_CALL_ID
    *                       - @ref telux::common::ErrorCode::INVALID_ARGUMENTS
    *                       - @ref telux::common::ErrorCode::OPERATION_NOT_ALLOWED
    *                       - @ref telux::common::ErrorCode::GENERIC_FAILURE
    *
    * @returns Status of makeECall i.e. success or suitable status code.
    *
    */
   virtual telux::common::Status makeECall(int phoneId, const std::string dialNumber,
                                           const std::vector<uint8_t> &msdPdu, int category,
                                           MakeCallCallback callback = nullptr)
      = 0;

   /**
    * Initiate an automotive eCall without transmitting Minimum Set of Data (MSD) at call connect.
    *
    * @param [in] phoneId      Represents phone corresponding to which make
    *                          eCall operation is performed
    * @param [in] category     @ref ECallCategory
    * @param [in] variant      @ref ECallVariant
    * @param [in] callback     Optional callback function to get the response of
    *                          makeECall request.
    *                          Possible(not exhaustive) error codes for callback response
    *                          - @ref telux::common::ErrorCode::SUCCESS
    *                          - @ref telux::common::ErrorCode::RADIO_NOT_AVAILABLE
    *                          - @ref telux::common::ErrorCode::NO_MEMORY
    *                          - @ref telux::common::ErrorCode::MODEM_ERR
    *                          - @ref telux::common::ErrorCode::INTERNAL_ERR
    *                          - @ref telux::common::ErrorCode::INVALID_STATE
    *                          - @ref telux::common::ErrorCode::INVALID_CALL_ID
    *                          - @ref telux::common::ErrorCode::INVALID_ARGUMENTS
    *                          - @ref telux::common::ErrorCode::OPERATION_NOT_ALLOWED
    *                          - @ref telux::common::ErrorCode::GENERIC_FAILURE
    *
    * @returns Status of makeECall i.e. success or suitable status code.
    */
   virtual telux::common::Status makeECall(int phoneId, int category, int variant,
                                           MakeCallCallback callback = nullptr)
      = 0;

   /**
    * Initiate an automotive eCall to the specified phone number for TPS eCall, without transmitting
    * Minimum Set of Data(MSD) at call connect. It will be treated like a regular voice call by the
    * UE and the network.
    *
    * @param [in] phoneId      Represents phone corresponding to which make
    *                          eCall operation is performed
    * @param [in] dialNumber   String representing the dialing number
    * @param [in] category     @ref ECallCategory
    * @param [in] callback     Optional callback function to get the response of
    *                          makeECall request.
    *                          Possible(not exhaustive) error codes for callback response
    *                          - @ref telux::common::ErrorCode::SUCCESS
    *                          - @ref telux::common::ErrorCode::RADIO_NOT_AVAILABLE
    *                          - @ref telux::common::ErrorCode::NO_MEMORY
    *                          - @ref telux::common::ErrorCode::MODEM_ERR
    *                          - @ref telux::common::ErrorCode::INTERNAL_ERR
    *                          - @ref telux::common::ErrorCode::INVALID_STATE
    *                          - @ref telux::common::ErrorCode::INVALID_CALL_ID
    *                          - @ref telux::common::ErrorCode::INVALID_ARGUMENTS
    *                          - @ref telux::common::ErrorCode::OPERATION_NOT_ALLOWED
    *                          - @ref telux::common::ErrorCode::GENERIC_FAILURE
    *
    * @returns Status of makeECall i.e. success or suitable status code.
    */
   virtual telux::common::Status makeECall(int phoneId, const std::string dialNumber, int category,
                                           MakeCallCallback callback = nullptr)
      = 0;

   /**
    * Update the eCall MSD in modem to be sent to Public Safety Answering Point
    * (PSAP) when requested.
    *
    * @param [in] phoneId   Represents phone corresponding to which
    *                       updateECallMsd operation is performed
    * @param [in] eCallMsd  The data structure represents the Minimum Set of Data
    *                       (MSD)
    * @param [in] callback  Optional callback pointer to get the response of
    *                       updateECallMsd.
    *
    * @returns Status of updateECallMsd i.e. success or suitable error code.
    */
   virtual telux::common::Status
      updateECallMsd(int phoneId, const ECallMsdData &eCallMsd,
                     std::shared_ptr<telux::common::ICommandResponseCallback> callback = nullptr)
      = 0;

   /**
    * Update the eCall MSD in modem to be sent to Public Safety Answering Point
    * (PSAP) when requested.
    *
    * @param [in] phoneId   Represents phone corresponding to which
    *                       updateECallMsd operation is performed
    * @param [in] msdPdu    Encoded MSD(Minimum Set of Data) PDU as per spec EN
    *                       15722 2015 or GOST R 54620-2011/33464-2015
    * @param [in] callback  Callback function to get the response of
    *                       updateECallMsd.
    *
    * @returns Status of updateECallMsd i.e. success or suitable error code.
    */
   virtual telux::common::Status updateECallMsd(int phoneId, const std::vector<uint8_t> &msdPdu,
                                                telux::common::ResponseCallback callback)
      = 0;

   /**
    * Request for status of eCall High Level Application Protocol(HLAP) timers that are maintained
    * by the UE state machine. This does not retrieve status of timers maintained by the PSAP.
    * The provided timers are as per EN 16062:2015 standard.
    *
    * @param [in] phoneId   Represents phone corresponding on which requestECallHlapTimerStatus
    *                       operation is performed
    * @param [in] callback  Callback function to get the response of requestECallHlapTimerStatus
    *
    * @returns Status of requestECallHlapTimerStatus i.e. success or suitable error code.
    *
    */
   virtual telux::common::Status requestECallHlapTimerStatus(int phoneId,
                                                         ECallHlapTimerStatusCallback callback) = 0;

   /**
    * Get in-progress calls.
    *
    * @returns List of active calls.
    */
   virtual std::vector<std::shared_ptr<ICall>> getInProgressCalls() = 0;

   /**
    * Merge two calls in a conference.
    *
    * @param [in] call1     Call object to conference.
    * @param [in] call2     Call object to conference.
    * @param [in] callback  Optional callback pointer to get the result of
    *                       conference function
    *
    * @returns Status of conference i.e. success or suitable error code.
    */
   virtual telux::common::Status
      conference(std::shared_ptr<ICall> call1, std::shared_ptr<ICall> call2,
                 std::shared_ptr<telux::common::ICommandResponseCallback> callback = nullptr)
      = 0;

   /**
    * Swap calls to make one active and put the another on hold.
    *
    * @param [in] callToHold      Active call object to swap to hold state.
    * @param [in] callToActivate  Hold call object to swap to active state.
    * @param [in] callback        Optional callback pointer to get the result of
    *                             swap function
    *
    * @returns Status of swap i.e. success or suitable error code.
    */
   virtual telux::common::Status
      swap(std::shared_ptr<ICall> callToHold, std::shared_ptr<ICall> callToActivate,
           std::shared_ptr<telux::common::ICommandResponseCallback> callback = nullptr)
      = 0;

   /**
    * Hangup all the foreground call(s) if any and accept the background call as the active call.
    * The foreground call here could be active call, incoming call or multiple active calls in case
    * of conference and background call could be held call or waiting call.
    *
    * If a call(s) is active, the active call(s) will be terminated or if a call is waiting, the
    * waiting call will be accepted and becomes active.  Otherwise, if a held call is present, the
    * held call becomes active.
    * In case of hold and waiting calls, the hold call will still be on hold and waiting call will
    * be accepted.
    * In case of hold, active and waiting scenario, the hold call will still be on hold, active
    * call will be ended and waiting call will be accepted.
    *
    * @param [in] callback - optional callback pointer to get the response of hangup request
    * below are possible error codes for callback response
    *        - @ref SUCCESS
    *        - @ref RADIO_NOT_AVAILABLE
    *        - @ref NO_MEMORY
    *        - @ref MODEM_ERR
    *        - @ref INTERNAL_ERR
    *        - @ref INVALID_STATE
    *        - @ref INVALID_CALL_ID
    *        - @ref INVALID_ARGUMENTS
    *        - @ref OPERATION_NOT_ALLOWED
    *        - @ref GENERIC_FAILURE
    *
    * @returns Status of hangupForegroundResumeBackground i.e. success or suitable error code.
    *
    * @note     Eval: This is a new API and is being evaluated.It is subject to change and could
    *           break backwards compatibility.
    */
   virtual telux::common::Status hangupForegroundResumeBackground(int phoneId,
      common::ResponseCallback callback = nullptr)
      = 0;

   /**
    * Request for emergency callback mode
    * @param [in] phoneId      Represents the phone corresponding to which the emergency callback
    *                          mode(ECBM) status is requested.
    * param [in] callback      Callback pointer to get the result of ECBM status request
    *
    * @returns Status of requestEcbm i.e. success or suitable error code.
    *
    * @note    Eval: This is a new API and is being evaluated. It is subject to
    *          change and could break backwards compatibility.
    */
   virtual telux::common::Status requestEcbm(int phoneId, EcbmStatusCallback callback) = 0;

   /**
    * Exit emergency callback mode.
    * @param [in] phoneId      Represents the phone corresponding to which the emergency callback
    *                          mode(ECBM) exit is requested.
    * param [in] callback      Optional callback pointer to get the result of exit ECBM request
    *
    * @returns Status of exitEcbm i.e. success or suitable error code.
    *
    * @note    Eval: This is a new API and is being evaluated. It is subject to
    *          change and could break backwards compatibility.
    */
   virtual telux::common::Status exitEcbm(int phoneId, common::ResponseCallback callback = nullptr)
      = 0;

   /**
    * Add a listener to listen for incoming call, call info change and eCall MSD
    * transmission status change.
    *
    * @param [in] listener  Pointer to ICallListener object which receives event
    *                       corresponding to phone
    *
    * @returns Status of registerListener i.e. success or suitable error code.
    */
   virtual telux::common::Status
      registerListener(std::shared_ptr<telux::tel::ICallListener> listener)
      = 0;

   /**
    * Remove a previously added listener.
    *
    * @param [in] listener  Listener to be removed.
    *
    * @returns Status of removeListener i.e. success or suitable error code.
    */
   virtual telux::common::Status removeListener(std::shared_ptr<telux::tel::ICallListener> listener)
      = 0;

   virtual ~ICallManager(){};
};

/**
 * @brief Interface for Make Call callback object.
 * Client needs to implement this interface to get single shot responses for
 * commands like make call.
 *
 * The methods in callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 */
class IMakeCallCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to makeCall API.
    *
    * @param [out] error  @ref ErrorCode
    * @param [out] call   Pointer to Call object or nullptr in case of failure
    */
   virtual void makeCallResponse(telux::common::ErrorCode error,
                                 std::shared_ptr<ICall> call = nullptr) {
   }

   virtual ~IMakeCallCallback(){};
};

/** @} */ /* end_addtogroup telematics_call */

}  // End of namespace tel

}  // End of namespace telux

#endif  // CALLMANAGER_HPP