/*
 *  Copyright (c) 2020 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef BRIDGEMANGERIMPL_HPP
#define BRIDGEMANGERIMPL_HPP

#include <telux/data/net/BridgeManager.hpp>

#include "common/AsyncTaskQueue.hpp"
#include "data/QcmClient.hpp"

namespace telux {
namespace data {
namespace net {

class BridgeManagerImpl : public IBridgeManager,
                        public IDataConnectionListener,
                        public std::enable_shared_from_this<BridgeManagerImpl> {
 public:
    BridgeManagerImpl(std::shared_ptr<QcmClient> qcmapClient);
    ~BridgeManagerImpl();
    /**
     * Checks the status of location manager and returns the result.
     *
     * @returns SERVICE_AVAILABLE      If Nat manager is ready for service.
     *          SERVICE_UNAVAILABLE    If Nat manager is temporarily unavailable.
     *          SERVICE_FAILED       - If Nat manager encountered an irrecoverable failure.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    telux::common::ServiceStatus getServiceStatus() override;

    /**
     * Checks if the data subsystem is ready.
     *
     * @returns True if BridgeManager Manager is ready for service, otherwise returns false.
     *
     */
    bool isSubsystemReady() override;

    /**
     * Wait for data subsystem to be ready.
     *
     * @returns A future that caller can wait until the BridgeManager Manager succeed/fail to be
     *          ready.
     *
     */
    std::future<bool> onSubsystemReady() override;

    /**
     * Enable/Disable the software bridge in the system.
     *
     * @param [in] enable    TRUE to enable, FALSE to disable the bridge
     * @param [in] callback  Optional callback to get the response for enableBridge
     *
     * @returns Status of enableBridge request i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated.It is subject to change
     *          and could break backwards compatibility.
     */
    telux::common::Status enableBridge( bool enable,
                        telux::common::ResponseCallback callback = nullptr) override;

    /**
     * Add software bridge configuration for an interface.
     *
     * @param [in] config    configuration for an interface
     * @param [in] callback  Optional callback to get the response for addBridge
     *
     * @returns Status of addBridge request i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated.It is subject to change
     *          and could break backwards compatibility.
     */
    telux::common::Status addBridge( BridgeInfo config,
                        telux::common::ResponseCallback callback = nullptr) override;

    /**
     * Request information about all software bridge configurations in the system
     *
     * @param [in] callback    Response callback with list of bridge configurations
     *
     * @returns Status of requestBridgeInfo request i.e. success or suitable status code.
     *
     * @note     Eval: This is a new API and is being evaluated.It is subject to change and could
     *           break backwards compatibility.
     */
    telux::common::Status requestBridgeInfo(BridgeInfoResponseCb callback) override;

    /**
     * Delete bridge configuration for an interface.
     *
     * @param [in] ifaceName   Name of the interface whose configuration needs to be deleted
     * @param [in] callback    Optional callback to get the response for removeBridge
     *
     * @returns Status of removeBridge request i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated.It is subject to change
     *          and could break backwards compatibility.
     */
    telux::common::Status removeBridge( std::string ifaceName,
                        telux::common::ResponseCallback callback = nullptr) override;

    /**
     * Register a listener for Data Service health events in the Vlan Manager like service available
     * or data service not available.
     *
     * @param [in] listener    pointer of IBridgeListener object that processes the
     * notification
     *
     * @returns Status of registerListener success or suitable status code
     *
     */
    virtual telux::common::Status registerListener(
        std::weak_ptr<IBridgeListener> listener) override;

    /**
     * Removes a previously added listener.
     *
     * @param [in] listener    pointer of IBridgeListener object that needs to be removed
     *
     * @returns Status of deregisterListener success or suitable status code
     *
     */
    virtual telux::common::Status deregisterListener(
        std::weak_ptr<IBridgeListener> listener) override;

    telux::common::Status init(telux::common::InitResponseCb callback);
    void onServiceStatusChange(telux::common::ServiceStatus status) override;

 private:
    /**
     * Perform synchronous initialization
     */
    void initSync();
    bool waitForInitialization();
    void setSubsystemReady(bool status);
    void setSubSystemStatus(telux::common::ServiceStatus status);
    void invokeInitCallback(telux::common::ServiceStatus status);

    std::mutex mtx_;
    bool ready_ = false;
    bool isInitComplete_;
    std::condition_variable cv_;
    telux::common::ServiceStatus subSystemStatus_;
    telux::data::OperationType oprType_;
    telux::common::InitResponseCb initCb_;
    telux::common::AsyncTaskQueue<void> taskQ_;
    std::shared_ptr<QcmClient> qcmClient_ = nullptr;
    std::shared_ptr<telux::common::ListenerManager<IBridgeListener>> listenerMgr_;
};
}
}
}

#endif
