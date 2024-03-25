/*
 *  Copyright (c) 2019-2020, The Linux Foundation. All rights reserved.
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

#ifndef MODEMCONFIGURATOR_HPP
#define MODEMCONFIGURATOR_HPP

#include <telux/config/ConfigFactory.hpp>
#include <telux/config/ModemConfigManager.hpp>
#include <telux/config/ModemConfigListener.hpp>

using namespace telux::config;

class ModemConfigurator : public IModemConfigListener,
                          public std::enable_shared_from_this<ModemConfigurator> {
public:
    ModemConfigurator();

    ~ModemConfigurator();

    void init();
    void cleanup();
    telux::common::Status parseArguments(int argc, char **argv);

    void onConfigUpdateStatus(ConfigUpdateStatus status, int slotId) override;

    void onServiceStatusChange(telux::common::ServiceStatus status) override;

private:
    void changeConfigType(int type);
    void changeSlotId(int slotId);
    void requestConfigList();
    void loadConfigFile(std::string filePath);
    void activateConfig(int configNo);
    void getActiveConfig();
    void getSelectionMode();
    void setSelectionMode(int selMode);
    void deactivateConfig();
    void deleteConfigFile(int configNo = -1);
    void printHelp();

    void printConfigList();

    std::shared_ptr<telux::config::IModemConfigManager> modemConfigManager_;
    std::vector<telux::config::ConfigInfo> configList_;
    telux::config::ConfigType configType_;
    int slotId_;
};


#endif // MODEMCONFIGURATOR_HPP