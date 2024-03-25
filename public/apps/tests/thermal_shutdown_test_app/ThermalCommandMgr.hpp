/*
 *  Copyright (c) 2019, The Linux Foundation. All rights reserved.
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

#ifndef THERMALCOMMANDMGR_HPP
#define THERMALCOMMANDMGR_HPP

#include<memory>

#include "telux/common/CommonDefines.hpp"

#include <telux/therm/ThermalDefines.hpp>
#include <telux/therm/ThermalFactory.hpp>
#include <telux/therm/ThermalShutdownManager.hpp>

#include "ThermalCommandCallback.hpp"
#include "ThermalListener.hpp"

#define APP_NAME "\033[1;32mThermal_shutdown_test_app\033[0m"

class ThermalCommandMgr : public std::enable_shared_from_this<ThermalCommandMgr> {
public:
   ThermalCommandMgr();
   ~ThermalCommandMgr();

   int init();
   void registerForUpdates();
   void deregisterForUpdates();
   void sendAutoShutdownModeCommand(telux::therm::AutoShutdownMode state);
   void sendRecurringDisableCommand();
   std::future<bool> getAutoShutdownModeCommand();
   void setAutoDisableFlag(bool disable);
   bool getAutoDisableFlag();

private:
  std::atomic<bool> autoDisable_;
  std::shared_ptr<telux::therm::IThermalShutdownManager> thermShutdownMgr_;
  std::shared_ptr<ThermalListener> myThermListener_;
  std::shared_ptr<ThermalCommandCallback> cmdRspCb_ = nullptr;

};

#endif  // THERMALCOMMANDMGR_HPP