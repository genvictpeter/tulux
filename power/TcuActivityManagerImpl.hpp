/*
 *  Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TCUACTIVITYMANAGERIMPL_HPP
#define TCUACTIVITYMANAGERIMPL_HPP

#include <vector>

extern "C" {
#include "power_manager_service_v01.h"
}

#include "telux/power/TcuActivityDefines.hpp"
#include "telux/power/TcuActivityManager.hpp"
#include "telux/power/TcuActivityListener.hpp"

#include "qmi/PowerQmiClient.hpp"
#include "ModemStateUpdater.hpp"
#include "common/AsyncTaskQueue.hpp"
#include "common/ListenerManager.hpp"

namespace telux {
namespace power {

using namespace telux::common;

struct TcuActivityUserData {
    int cmdCallbackId;
    TcuActivityState prevState;
};

class TcuActivityManagerImpl : public ITcuActivityManager,
                         public telux::qmi::IQmiTcuActivityListener,
                         public telux::qmi::IQmiRegisterTcuStateEventCallback,
                         public telux::qmi::IQmiTcuStateAckCallback,
                         public telux::qmi::IQmiTcuStateCommandCallback,
                         public std::enable_shared_from_this<TcuActivityManagerImpl> {
public:
    TcuActivityManagerImpl(ClientType clientType, ProcType procType);

    /**
     * API to check the status of TCU-activity services and if the other APIs are ready for use,
     * and returns the result.
     * Implements ITcuActivityManager::isReady
     *
     * @returns  True if the services are ready otherwise false.
     *
     */
    bool isReady() override;

    /**
     * API to wait for TCU-activity management services to be ready.
     * Implements ITcuActivityManager::onReady
     *
     * @returns  A future that caller can wait on to be notified when the TCU-activity services
     *           are ready.
     *
     */
    std::future<bool> onReady() override;

    /**
     * This status indicates whether the ITcuActivityManager object is in a usable state.
     *
     * @returns SERVICE_AVAILABLE    -  If the Manager is ready for service.
     *          SERVICE_UNAVAILABLE  -  If the Manager is temporarily unavailable.
     *          SERVICE_FAILED       -  If the Manager encountered an irrecoverable failure.
     *
     */
    telux::common::ServiceStatus getServiceStatus() override;

    /**
     * Function to initialize connection to TCU-activity management services and register for events
     *
     * @returns  Status of init, success or suitable status code
     *
     */
    telux::common::Status init(telux::common::InitResponseCb callback);

    /**
     * API to register a listener for updates on TCU-activity state changes.
     * Implements ITcuActivityManager::registerListener
     *
     * @param [in] listener - Pointer of ITcuActivityListener object that processes the notification
     *
     * @returns Status of registerListener i.e success or suitable status code.
     *
     */
    telux::common::Status registerListener(std::weak_ptr<ITcuActivityListener> listener) override;

    /**
     * API to remove a previously registered listener.
     * Implements ITcuActivityManager::deregisterListener
     *
     * @param [in] listener - Previously registered ITcuActivityListener that needs to be removed
     *
     * @returns Status of deregisterListener, success or suitable status code
     *
     */
    telux::common::Status deregisterListener(std::weak_ptr<ITcuActivityListener> listener) override;

    /**
     * API to register a listener for updates on TCU-activity management service status.
     * Implements ITcuActivityManager::registerServiceStateListener
     *
     * @param [in] listener - Pointer of IServiceStatusListener object that processes the
     *                        notification
     *
     * @returns Status of registerServiceStateListener i.e success or suitable status code.
     *
     */
    telux::common::Status registerServiceStateListener(
                                std::weak_ptr<IServiceStatusListener> listener) override;

    /**
     * API to remove a previously registered listener for service status updates.
     * Implements ITcuActivityManager::deregisterServiceStateListener
     *
     * @param [in] listener - Previously registered IServiceStatusListener that needs to be removed
     *
     * @returns Status of deregisterServiceStateListener, success or suitable status code
     *
     */
    telux::common::Status deregisterServiceStateListener(
                                std::weak_ptr<IServiceStatusListener> listener) override;

    /**
     * API to initiate a TCU-activity state transition.
     * Implements ITcuActivityManager::setActivityState
     *
     * This API needs to be used cautiously, as it changes the power-state of the system and may
     * affect other processes.
     *
     * @param [in] state    - TCU-activity state that the System is intended to enter
     * @param [in] callback - Optional callback to get the response for the state transition
     *                        command
     *
     * @returns Status of setActivityState i.e. success or suitable status code.
     *
     */
    telux::common::Status setActivityState( TcuActivityState state,
                                telux::common::ResponseCallback callback = nullptr) override;

    /**
     * API to get the current TCU-activity state.
     * Implements ITcuActivityManager::getActivityState
     *
     * @returns TcuActivityState
     *
     */
    TcuActivityState getActivityState() override;

    /**
     * API to send the acknowledgement, after processing a TCU-activity state notification.
     * This indicates that the client is prepared for state transition.Only one acknowledgement is
     * expected from a single client process(may have multiple listeners).
     * Implements ITcuActivityManager::sendActivityStateAck
     *
     * @param [in] ack Acknowledgement for a TcuActivityState notification.
     *
     * @returns Status of sendActivityStateAck i.e. success or suitable status code.
     *
     */
    telux::common::Status sendActivityStateAck(TcuActivityStateAck ack) override;

    /**
     * This function is called by qmi-client when a TCU-activity state QMI indication is received.
     * Implements IQmiTcuActivityListener::onTcuStateUpdate
     *
     * @param [in] indData - pointer to QMI indication message buffer
     *
     */
    void onTcuStateUpdate(pwr_mgr_broadcast_indication_msg_v01 *indData) override;

    /**
     * This function is called with the overall acknowledgement status from all the SLAVE
     * applications, for state change triggered previously by MASTER application.
     *
     * @param [in]   serviceEvent       Event type
     */
    void onSlaveAckStatusUpdate(pwr_mgr_state_change_v01 indData) override;

    /**
     * This function will be called whenever the TCU-activity service status changes
     *
     * @param [in]   serviceEvent       Event type
     */
    void onTcuActivityServiceStatusChange(telux::common::ServiceStatus status) override;

    ~TcuActivityManagerImpl();

private:

    /**
     * Function to set the status of TCU-activity manager
     *
     * @param [in] status - @ref ServiceStatus
     *
     */
    void setServiceStatus(telux::common::ServiceStatus status);

    /**
     * A generic function that is used to invoke a client's ResponseCallback(extracted from
     * TcuActivityUserData) with the result of client's previous request.
     *
     * @param [in] errorCode - result to be conveyed
     *        [in] userData - pointer to TcuActivityUserData, that contains ResponseCallback info
     *
     */
    void handleResponse(int errorCode, void *userData);

    /**
     * This function is called by qmi-client when it receives a response for event registration.
     * Implements IQmiRegisterTcuStateEventCallback::registerEventsResponse
     *
     * @param [in] errorCode - status of registation
     *        [in] initialState - pointer to register response QMI message
     *        [in] userData - pointer to TcuActivityUserData
     *
     */

    void registerEventsResponse(int errorCode, pwr_mgr_register_resp_msg_v01* initialState,
                                    void *userData);

    /**
     * This function is called by qmi-client when it receives a response for acknowledgement.
     * Implements IQmiTcuStateAckCallback::onTcuStateAckResponse
     *
     * @param [in] errorCode - status of registation
     *        [in] userData - pointer to TcuActivityUserData
     *
     */
    void onTcuStateAckResponse(int errorCode, void *userData);

    /**
     * This function is called by qmi-client when it receives a response for TCU-activity state
     * command.
     * Implements IQmiTcuStateCommandCallback::onTcuStateCommandResponse
     *
     * @param [in] errorCode - status of registation
     *        [in] userData - pointer to TcuActivityUserData
     *
     */
    void onTcuStateCommandResponse(int errorCode, void *userData);

    /**
     * Function to set the value of the member variable - currentTcuState_, which indicates the
     * current TCU-activity state.
     *
     * @param [in] state - TcuActivityState to be set
     *
     */
    void setCachedTcuState(TcuActivityState state);

    TcuActivityManagerImpl(TcuActivityManagerImpl const &) = delete;
    TcuActivityManagerImpl &operator=(TcuActivityManagerImpl const &) = delete;

    std::shared_ptr<telux::common::ListenerManager<ITcuActivityListener>> listenerMgr_;
    std::shared_ptr<telux::common::ListenerManager<IServiceStatusListener>> svcStatusListenerMgr_;
    telux::common::CommandCallbackManager cmdCallbackMgr_;
    TcuActivityState currentTcuState_;
    std::mutex mutex_;
    bool isInitsyncTriggered_ = false;
    telux::common::AsyncTaskQueue<void> taskQ_;
    std::shared_ptr<telux::qmi::PowerQmiClient> qmiClient_;
    ClientType clientType_ = ClientType::SLAVE;
    ProcType procType_ = ProcType::LOCAL_PROC;
    std::shared_ptr<ModemStateUpdater> modemStateUpdater_;
    std::condition_variable initCV_;
    bool waitForInitialization();
    void initSync();
    void cleanup();
    telux::common::ServiceStatus subSystemStatus_
        = telux::common::ServiceStatus::SERVICE_UNAVAILABLE;
    telux::common::InitResponseCb initCb_;
};

}  // end of namespace power
}  // end of namespace telux

#endif  // TCUACTIVITYMANAGERIMPL_HPP
