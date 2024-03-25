/*
 *  Copyright (c) 2020 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef MODEMSTATEUPDATER_HPP
#define MODEMSTATEUPDATER_HPP

#include <vector>

#include "telux/power/TcuActivityDefines.hpp"
#include "qmi/DmsQmiClient.hpp"
#include "common/ListenerManager.hpp"

namespace telux {
namespace power {

using namespace telux::common;

/**
 * Class provides functions to update modem about the imminent TCU activity state transition
 */

class ModemStateUpdater: public telux::qmi::IQmiDmsListener,
                         public telux::qmi::IQmiDeviceActivityStateCallback,
                         public telux::qmi::IQmiRegisterDmsIndicationCallback,
                         public std::enable_shared_from_this<ModemStateUpdater> {
public:
    ModemStateUpdater();

    /**
     * Function to initialize connection to Modem State updater services and register for events
     *
     * @returns  Status of init, success or suitable status code
     *
     */
    telux::common::Status init();


    /**
     * Function to send TCU-activity state to modem.
     *
     * @param [in] tcuState - Current TCU Activity state.
     * @param [in] callback - Optional callback to get the response for the command that
     *                        updates tcuState to modem.
     *
     * @returns Status of the API, success or suitable status code
     */
    Status sendTcuStateToModem(TcuActivityState tcuState,
                telux::common::ResponseCallback callback = nullptr);

    ~ModemStateUpdater();
private:

    /**
     * Function to check the subsystem readiness.
     */
    bool isSubSystemReady();

    /**
     * Function to check modem client's readiness to accept the reports
     */
    bool isClientReady();

    /**
     * Function to update the subsystem readiness.
     */
    void setSubSystemReadyState(bool ready);

    /**
     * Function to update the readiness of the modem clients
     */
    void setClientReadyState(bool ready);

    /**
     * QMI listener and callback implementing functions
     */
    void onModemActivityClientReady(bool report);
    void onDmsServiceStatusChange(telux::common::ServiceStatus status, int slotId);
    void onDmsIndicationRegisterResponse(int qmiError, void *userData);
    void onSendDeviceActivityStateResponse(int qmiError, void *userData);

    /**
     * Miscellanous member functions and variables
     */
    std::mutex mutex_;
    bool subSystemReady_;
    bool clientReady_;
    bool isInitSyncTriggered_ = false;
    std::shared_ptr<telux::qmi::DmsQmiClient> dmsQmiClient_;
    telux::common::AsyncTaskQueue<void> taskQ_;
    void initSync();
};

}  // end of namespace power
}  // end of namespace telux

#endif  // MODEMSTATEUPDATER_HPP
