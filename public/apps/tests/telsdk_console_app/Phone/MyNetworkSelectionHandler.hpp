/*
 *  Copyright (c) 2018, 2020 The Linux Foundation. All rights reserved.
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

#ifndef MYNETWORKSELECTIONHANDLER_HPP
#define MYNETWORKSELECTIONHANDLER_HPP

#include <memory>
#include <string>
#include <vector>

#include <telux/common/CommonDefines.hpp>
#include <telux/tel/NetworkSelectionManager.hpp>

class MySelectionModeResponseCallback {
public:
   static void selectionModeResponse(telux::tel::NetworkSelectionMode networkSelectionMode,
                                     telux::common::ErrorCode error);
};

class MyPreferredNetworksResponseCallback {
public:
   static void preferredNetworksResponse(
      std::vector<telux::tel::PreferredNetworkInfo> preferredNetworks3gppInfo,
      std::vector<telux::tel::PreferredNetworkInfo> staticPreferredNetworksInfo,
      telux::common::ErrorCode error);
};

class MyNetworkResponsecallback {
public:
   static void setNetworkSelectionModeResponseCb(telux::common::ErrorCode error);
   static void setPreferredNetworksResponseCb(telux::common::ErrorCode error);
};

class MyPerformNetworkScanCallback {
public:
   static void performNetworkScanResponseCb(telux::common::ErrorCode error);
};

class MyNetworkSelectionHelper {
public:
   static std::string networkSelectionModeToString(telux::tel::NetworkSelectionMode mode);
   static void logInUseStatus(int status);
   static void logRoamingStatus(int status);
   static void logForbiddenStatus(int status);
   static void logPreferredStatus(int status);
   static void logPreferredNetworkInfo(std::vector<telux::tel::PreferredNetworkInfo> nwInfo);
};

class MyNetworkSelectionListener : public telux::tel::INetworkSelectionListener {
public:
   void onSelectionModeChanged(telux::tel::NetworkSelectionMode mode) override;
   void onNetworkScanResults(telux::tel::NetworkScanStatus scanStatus,
      std::vector<telux::tel::OperatorInfo> operatorInfos) override;
   static std::string networkScanStatusToString(telux::tel::NetworkScanStatus scanStatus);
   static std::string convertRatTypeAsString(telux::tel::RadioTechnology rat);
};

#endif  // MYNETWORKSELECTIONHANDLER_HPP
