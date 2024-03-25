/*
 *  Copyright (c) 2019 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef SOCKSMANGERIMPL_HPP
#define SOCKSMANGERIMPL_HPP

#include <telux/data/net/SocksManager.hpp>

#include "common/AsyncTaskQueue.hpp"
#include "data/QcmClient.hpp"

namespace telux {
namespace data {
namespace net {

class SocksManagerImpl : public ISocksManager,
                         public IDataConnectionListener,
                         public std::enable_shared_from_this<SocksManagerImpl> {
 public:
    SocksManagerImpl(telux::data::OperationType oprType, std::shared_ptr<QcmClient> qcmapClient);
    ~SocksManagerImpl();
    /**
     * Checks the status of location manager and returns the result.
     *
     * @returns SERVICE_AVAILABLE      If Socks manager object is ready for service.
     *          SERVICE_UNAVAILABLE    If Socks manager object is temporarily unavailable.
     *          SERVICE_FAILED       - If Socks manager object encountered an irrecoverable failure.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    telux::common::ServiceStatus getServiceStatus() override;

    /**
     * Checks if the data subsystem is ready.
     *
     * @returns True if SocksManager is ready for service, otherwise
     * returns false.
     *
     */
    bool isSubsystemReady() override;

    /**
     * Wait for data subsystem to be ready.
     *
     * @returns A future that caller can wait on to be notified
     * when SocksManager is ready.
     *
     */
    std::future<bool> onSubsystemReady() override;

    /**
     * Enable/Disable Socks Proxy
     *
     * @param [in] socksEnable       true: enable proxy, false: disable proxy
     * @param [out] callback         optional callback to get the operation error code if any
     *
     * @returns Immediate status of socksEnable() request sent i.e. success or suitable status
     * code.
     *
     * @note     Eval: This is a new API and is being evaluated.It is subject to change and could
     *           break backwards compatibility.
     *
     */
    telux::common::Status enableSocks(
        bool enable, telux::common::ResponseCallback callback = nullptr) override;

    /**
     * Get the associated operation type for this instance.
     *
     * @returns OperationType of getOperationType i.e. LOCAL or REMOTE.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    telux::data::OperationType getOperationType() override;

    /**
     * Register a listener for Data Service health events in the Vlan Manager like service available
     * or data service not available.
     *
     * @param [in] listener    pointer of ISocksListener object that processes the
     * notification
     *
     * @returns Status of registerListener success or suitable status code
     *
     */
    virtual telux::common::Status registerListener(std::weak_ptr<ISocksListener> listener) override;

    /**
     * Removes a previously added listener.
     *
     * @param [in] listener    pointer of ISocksListener object that needs to be removed
     *
     * @returns Status of deregisterListener success or suitable status code
     *
     */
    virtual telux::common::Status deregisterListener(
        std::weak_ptr<ISocksListener> listener) override;

    telux::common::Status init(telux::common::InitResponseCb callback);
    telux::common::Status cleanup();
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
    std::condition_variable cv_;
    bool isInitComplete_;
    telux::common::InitResponseCb initCb_;
    telux::data::OperationType oprType_;
    telux::common::ServiceStatus subSystemStatus_;
    std::shared_ptr<QcmClient> qcmClient_ = nullptr;
    telux::common::AsyncTaskQueue<void> taskQ_;
    std::shared_ptr<telux::common::ListenerManager<ISocksListener>> listenerMgr_;
};
}
}
}

#endif