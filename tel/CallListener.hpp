/*
 *  Copyright (c) 2017-2018,2020 The Linux Foundation. All rights reserved.
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
 * @file       CallListener.hpp
 * @brief      Interface for Call listener object. Client needs to implement this interface
 *             to get access to Call related notifications like call state changes and ecall
 *             state change.
 *
 *             The methods in listener can be invoked from multiple different threads. The
 *             implementation should be thread safe.
 */

#ifndef CALLLISTENER_HPP
#define CALLLISTENER_HPP

#include <vector>
#include <memory>

#include <telux/common/CommonDefines.hpp>

#include <telux/tel/Call.hpp>
#include <telux/tel/PhoneDefines.hpp>
#include <telux/tel/Phone.hpp>
#include <telux/tel/ECallDefines.hpp>

namespace telux {

namespace tel {

/** @addtogroup telematics_call
 * @{ */

class ICall;

/**
 * @brief A listener class for monitoring changes in call,
 * including call state change and ECall state change.
 * Override the methods for the state that you wish to receive updates for.
 *
 * The methods in listener can be invoked from multiple different threads. The implementation
 * should be thread safe.
 */
class ICallListener : public common::IServiceStatusListener{
public:
   /**
    * This function is called when device receives an incoming/waiting call.
    *
    * @param [in] call -  Pointer to ICall instance
    */
   virtual void onIncomingCall(std::shared_ptr<ICall> call) {
   }

   /**
    * This function is called when there is a change in call attributes
    *
    * @param [in] call -  Pointer to ICall instance
    */
   virtual void onCallInfoChange(std::shared_ptr<ICall> call) {
   }

   /**
    * This function is called when device completes MSD Transmission.
    *
    * @param [in] phoneId - Unique Id of phone on which MSD Transmission Status is being reported
    * @param [in] status - Indicates MSD Transmission status i.e. success or failure
    *
    * @deprecated Use another onECallMsdTransmissionStatus() API with argument
    * @Ref ECallMsdTransmissionStatus
    */
   virtual void onECallMsdTransmissionStatus(int phoneId, telux::common::ErrorCode errorCode) {
   }

   /**
    * This function is called when MSD Transmission status is changed.
    *
    * @param [in] phoneId - Unique Id of phone on which MSD Transmission Status is being reported
    * @param [in] msdTransmissionStatus - Indicates MSD Transmission status
    * @Ref ECallMsdTransmissionStatus
    */
   virtual void onECallMsdTransmissionStatus(
      int phoneId, telux::tel::ECallMsdTransmissionStatus msdTransmissionStatus) {
   }

   /**
    * This function is called when the eCall High Level Application Protocol(HLAP) timers status
    * is changed.
    *
    * @param [in] phoneId - Unique Id of phone on which HLAP timer status is being reported
    * @param [in] timersStatus - Indicates the HLAP timer event
    *                            @Ref ECallHlapTimerEvents
    *
    */
   virtual void onECallHlapTimerEvent(int phoneId, ECallHlapTimerEvents timersStatus) {
   }

    /**
    * This function is called whenever there is a scan failure after one round of network scan
    * during origination of emergency call or at any time during the emergency call.
    *
    * During origination of an ecall or in between an ongoing ecall, if the UE is in an area of
    * no/poor coverage and loses service, the modem will perform network scan and try to register
    * on any available network.
    * If the scan completes successfully and the device finds a suitable cell, the ecall will be
    * placed and the call state changes to the active state.
    * If the network scan fails then this function will be invoked after one round of network scan.
    *
    * @param [in] phoneId - Unique Id of phone on which network scan failure reported.
    *
    * @note    Eval: This is a new API and is being evaluated.It is subject to change
    *          and could break backwards compatibility.
    */
   virtual void onEmergencyNetworkScanFail(int phoneId) {
   }

   /**
    * This function is called whenever emergency callback mode(ECBM) changes.
    *
    * @param [in] mode   - Indicates the status of the ECBM.
    *                      @ref EcbMode
    *
    * @note    Eval: This is a new API and is being evaluated. It is subject to
    *          change and could break backwards compatibility.
    */
   virtual void onEcbmChange(telux::tel::EcbMode mode) {
   }


   virtual ~ICallListener() {
   }
};
/** @} */ /* end_addtogroup telematics_call */

}  // End of namespace tel

}  // End of namespace telux

#endif  // CALLLISTENER_HPP
