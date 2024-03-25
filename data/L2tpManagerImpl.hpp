/*
 *  Copyright (c) 2020 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef L2TPMANGERIMPL_HPP
#define L2TPMANGERIMPL_HPP

#include <telux/data/net/L2tpManager.hpp>

#include "common/AsyncTaskQueue.hpp"
#include "data/QcmClient.hpp"

namespace telux {
namespace data {
namespace net {

class L2tpManagerImpl : public IL2tpManager,
                        public IDataConnectionListener,
                        public std::enable_shared_from_this<L2tpManagerImpl> {
 public:
    L2tpManagerImpl(std::shared_ptr<QcmClient> qcmapClient);
    ~L2tpManagerImpl();
    /**
     * Checks the status of location manager and returns the result.
     *
     * @returns SERVICE_AVAILABLE      If Nat manager object is ready for service.
     *          SERVICE_UNAVAILABLE    If Nat manager object is temporarily unavailable.
     *          SERVICE_FAILED       - If Nat manager object encountered an irrecoverable failure.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    telux::common::ServiceStatus getServiceStatus() override;

    /**
     * Checks if the data subsystem is ready.
     *
     * @returns True if L2TP Manager is ready for service, otherwise
     *          returns false.
     *
     */
    bool isSubsystemReady() override;

    /**
     * Wait for data subsystem to be ready.
     *
     * @returns A future that caller can wait on to be notified
     *          when L2TP manager is ready.
     *
     */
    std::future<bool> onSubsystemReady() override;

    /**
     * Enables L2TP for unmanaged Tunnel State
     *
     * @param [in] enable         Enable/Disable L2TP for unmanaged tunnels.
     * @param [in] enableMss      Enable/Disable TCP MSS to be clamped on L2TP interfaces to
     *                            avoid Segmentation
     * @param [in] enableMtu      Enable/Disable MTU size to be set on underlying interfaces to
     *                            avoid fragmentation
     * @param [in] callback       optional callback to get the response setConfig
     *
     * @param [in] mtuSize        optional MTU size in bytes. If not set, MTU size will be set to
     *                            default 1422 bytes
     * @returns Status of setConfig i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    virtual telux::common::Status setConfig(bool enable, bool enableMss, bool enableMtu,
        telux::common::ResponseCallback callback = nullptr, uint32_t mtuSize = 0) override;

    /**
     * Set L2TP Configuration,
     *
     * @param [in] l2tpTunnelConfig        Configuration to be set @ref telux::net::L2tpTunnelConfig
     * @param [in] callback          optional callback to get the response addTunnel
     *
     * @returns Status of addTunnel i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    virtual telux::common::Status addTunnel(const L2tpTunnelConfig &l2tpTunnelConfig,
        telux::common::ResponseCallback callback = nullptr) override;

    /**
     * Get Current L2TP Configuration
     *
     * @param [in] l2tpConfigCb      Asynchronous callback to get current L2TP configurations
     *
     * @returns Status of requestConfig i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    virtual telux::common::Status requestConfig(L2tpConfigCb l2tpConfigCb) override;

    /**
     * Remove L2TP Tunnel
     *
     * @param [in] tunnelId          Tunnel ID to be removed
     * @param [in] callback          optional callback to get the response removeConfig
     *
     * @returns Status of removeTunnel i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    virtual telux::common::Status removeTunnel(
        uint32_t tunnelId, telux::common::ResponseCallback callback = nullptr) override;

    /**
     * Register a listener for Data Service health events in the Vlan Manager like service available
     * or data service not available.
     *
     * @param [in] listener    pointer of IL2tpListener object that processes the
     * notification
     *
     * @returns Status of registerListener success or suitable status code
     *
     */
    virtual telux::common::Status registerListener(std::weak_ptr<IL2tpListener> listener) override;

    /**
     * Removes a previously added listener.
     *
     * @param [in] listener    pointer of IL2tpListener object that needs to be removed
     *
     * @returns Status of deregisterListener success or suitable status code
     *
     */
    virtual telux::common::Status deregisterListener(std::weak_ptr<IL2tpListener> listener) override;

    telux::common::Status init(telux::common::InitResponseCb callback);
    telux::common::Status cleanup();
    void onServiceStatusChange(telux::common::ServiceStatus status) override;

 private:
    /**
     * Perform synchronous initialization
     */
    void initSync();
    void setSubsystemReady(bool status);
    bool waitForInitialization();
    void setSubSystemStatus(telux::common::ServiceStatus status);
    void invokeInitCallback(telux::common::ServiceStatus status);

    telux::common::Status setConfigSync(bool enable, bool enableMss, bool enableMtu,
        uint32_t mtuSize, telux::common::ResponseCallback callback = nullptr);
    telux::common::Status addTunnelSync(const L2tpTunnelConfig &l2tpTunnelConfig,
        telux::common::ResponseCallback callback = nullptr);
    telux::common::Status requestConfigSync(L2tpConfigCb l2tpConfigCb);
    telux::common::Status removeTunnelSync(
        uint32_t tunnelId, telux::common::ResponseCallback callback = nullptr);

    std::mutex mtx_;
    std::condition_variable cv_;
    bool ready_ = false;
    telux::common::ServiceStatus subSystemStatus_;
    bool isInitComplete_;
    telux::common::InitResponseCb initCb_;
    telux::data::OperationType oprType_;
    std::shared_ptr<QcmClient> qcmClient_ = nullptr;
    telux::common::AsyncTaskQueue<void> taskQ_;
    std::shared_ptr<telux::common::ListenerManager<IL2tpListener>> listenerMgr_;
};
}
}
}

#endif