/*
 *  Copyright (c) 2020, The Linux Foundation. All rights reserved.
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
 * This is a Data Connection Manager Sample Application using Telematics SDK.
 * It is used to demonstrate APIs to start/stop data call, get list of data calls, reset data call
 * statistics, and get data call statistics
 */

#ifndef DATACONNECTIONMENU_HPP
#define DATACONNECTIONMENU_HPP

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <iomanip>

#include <telux/data/DataConnectionManager.hpp>
#include "console_app_framework/ConsoleApp.hpp"

#include <telux/data/DataDefines.hpp>
#include <telux/data/DataFactory.hpp>

#include "../DataListener.hpp"
#include "../DataResponseCallback.hpp"
#include "../MyProfileListener.hpp"

using namespace telux::data;
using namespace telux::common;

class DataConnectionMenu : public ConsoleApp {
public:
    // initialize Profile Managers
    bool init();
    // Display Profile Management Menu
    void displayMenu();

    // Data Connection Management APIs
    void startDataCall(std::vector<std::string> inputCommand);
    void stopDataCall(std::vector<std::string> inputCommand);
    void requestDataCallStatistics(std::vector<std::string> inputCommand);
    void resetDataCallStatistics(std::vector<std::string> inputCommand);
    void requestDataCallList();
    void setDefaultProfile();
    void getDefaultProfile();

    DataConnectionMenu(std::string appName, std::string cursor);
    ~DataConnectionMenu();
private:
    bool initConnectionManagerAndListener(SlotId slotId);
    void requestDataCallList(OperationType operationType, SlotId slotId, DataCallListResponseCb cb);

    std::map<SlotId, std::shared_ptr<DataListener>> dataListeners_;
    std::map<SlotId, std::shared_ptr<telux::data::IDataConnectionManager>>dataConnectionManagerMap_;
};
#endif