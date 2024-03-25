/*
 *  Copyright (c) 2018-2019, The Linux Foundation. All rights reserved.
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

#ifndef MYPHONELISTENER_HPP
#define MYPHONELISTENER_HPP

#include <telux/tel/Phone.hpp>
#include <telux/tel/PhoneListener.hpp>
#include <telux/tel/PhoneDefines.hpp>
#include <telux/tel/VoiceServiceInfo.hpp>
#include <telux/tel/ECallDefines.hpp>

#include <telux/common/CommonDefines.hpp>

class MyPhoneListener : public telux::tel::IPhoneListener {
public:
   void onServiceStateChanged(int phoneId, telux::tel::ServiceState state) override;
   void onSignalStrengthChanged(
      int phoneId, std::shared_ptr<telux::tel::SignalStrength> signalStrength) override;
   void onVoiceRadioTechnologyChanged(int phoneId,
                                      telux::tel::RadioTechnology radioTechnology) override;
   void onVoiceServiceStateChanged(
      int phoneId, const std::shared_ptr<telux::tel::VoiceServiceInfo> &serviceInfo) override;
   void onOperatingModeChanged(telux::tel::OperatingMode mode) override;
   void onCellInfoListChanged(
      int phoneId, std::vector<std::shared_ptr<telux::tel::CellInfo>> cellInfoList) override;
   void onECallOperatingModeChange(int phoneId, telux::tel::ECallModeInfo info) override;
   std::string getCurrentTime();

   ~MyPhoneListener() {
   }

private:
   std::string radioStateToString(telux::tel::RadioState radioState);
   std::string serviceStateToString(telux::tel::ServiceState serviceState);
   std::string eCallModeReasonToString(telux::tel::ECallModeReason reason);
   telux::tel::VoiceServiceState voiceSrvcState_ = telux::tel::VoiceServiceState::UNKNOWN;
};

class MyCellularCapabilityCallback : public telux::tel::ICellularCapabilityCallback {
public:
   void cellularCapabilityResponse(telux::tel::CellularCapabilityInfo capabilityInfo,
                                   telux::common::ErrorCode error) override;

private:
   std::string
      voiceServiceTechnologiesMaskToString(telux::tel::VoiceServiceTechnologiesMask capabilities);
   std::string ratCapabilitiesMaskToString(telux::tel::RATCapabilitiesMask ratCapabilitiesMask);
};

class MyVoiceRadioTechnologyCallback {
public:
   void voiceRadioTechnologyResponse(telux::tel::RadioTechnology radioTechnology,
                                     telux::common::ErrorCode error);

private:
   std::string radioTechToString(telux::tel::RadioTechnology radioTech);
};

class MyVoiceServiceStateCallback : public telux::tel::IVoiceServiceStateCallback {
public:
   void voiceServiceStateResponse(const std::shared_ptr<telux::tel::VoiceServiceInfo> &serviceInfo,
                                  telux::common::ErrorCode error) override;
};

class MyGetOperatingModeCallback : public telux::tel::IOperatingModeCallback {
public:
   void operatingModeResponse(telux::tel::OperatingMode operatingMode,
                              telux::common::ErrorCode error) override;
};

class MySetOperatingModeCallback {
public:
   void setOperatingModeResponse(telux::common::ErrorCode error);
};

class MySetECallOperatingModeCallback {
public:
   static void setECallOperatingModeResponse(telux::common::ErrorCode error);
};

class MyGetECallOperatingModeCallback {
public:
   static void getECallOperatingModeResponse(telux::tel::ECallMode eCallMode,
                                             telux::common::ErrorCode error);
};

class MyPhoneHelper {
public:
   static std::string operatingModeToString(telux::tel::OperatingMode operatingMode);
   static void printCellInfoDetails(std::vector<std::shared_ptr<telux::tel::CellInfo>> cellInfoList);
   static std::string eCallOperatingModeToString(telux::tel::ECallMode eCallMode);
   static std::string voiceServiceStateToString(telux::tel::VoiceServiceState vocSrvState);
   static std::string signalLevelToString(telux::tel::SignalStrengthLevel level);
};

#endif  // MYPHONELISTENER_HPP
