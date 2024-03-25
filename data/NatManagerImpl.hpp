/*
 *  Copyright (c) 2019 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef NATMANGERIMPL_HPP
#define NATMANGERIMPL_HPP

#include <telux/data/net/NatManager.hpp>
#include "common/AsyncTaskQueue.hpp"
#include "data/QcmClient.hpp"

namespace telux {
namespace data {
namespace net {

class NatManagerImpl : public INatManager,
                       public IDataConnectionListener,
                       public std::enable_shared_from_this<NatManagerImpl> {
 public:
    NatManagerImpl(telux::data::OperationType oprType, std::shared_ptr<QcmClient> qcmapClient);
    ~NatManagerImpl();
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
     * Checks if the NAT manager subsystem is ready.
     *
     * @returns True if NAT Manager is ready for service, otherwise
     * returns false.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    bool isSubsystemReady() override;

    /**
     * Wait for NAT manager subsystem to be ready.
     *
     * @returns A future that caller can wait on to be notified
     * when NAT manager is ready.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    std::future<bool> onSubsystemReady() override;

    /**
     * Adds a static Network Address Translation (NAT) entry in the NAT table, these
     * entries are persistent across object, connection and reboot lifetimes. To remove
     * an entry it needs a explicit call to removeStaticNatEntry() API, it supports both
     * IPv4 and IPv6
     *
     * @param [in] profileId         Profile identifier to which static entry will be mapped to.
     * @param [in] snatConfig        snatConfiguration @ref telux::net::NatConfig
     * @param [in] callback          optional callback to get the response addStaticNatEntry
     * @param [in] slotId            Specify slot id which has the sim that contains profile id
     *                               static entry to be added on.
     *
     * @returns Status of addStaticNatEntry i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    telux::common::Status addStaticNatEntry(int profileId, const NatConfig &snatConfig,
        telux::common::ResponseCallback callback = nullptr, SlotId slotId = DEFAULT_SLOT_ID) override;

    /**
     * Removes a static Network Address Translation (NAT) entry in the NAT table,
     * it supports both IPv4 and IPv6
     *
     * @param [in] profileId         Profile identifier to which static entry will be removed from.
     * @param [in] snatConfig        snatConfiguration @ref telux::net::NatConfig
     * @param [in] callback          optional callback to get the response removeStaticNatEntry
     * @param [in] slotId            Specify slot id which has the sim that contains profile id
     *                               static entry to be removed from.
     *
     * @returns Status of removeStaticNatEntry i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    telux::common::Status removeStaticNatEntry(int profileId, const NatConfig &snatConfig,
        telux::common::ResponseCallback callback = nullptr, SlotId slotIdtIdtId = DEFAULT_SLOT_ID) override;

    /**
     * Request list of static nat entries available in the NAT table
     *
     * @param [in] profileId         Profile identifier to which static entries will be retrieved.
     * @param[in] snatEntriesCb      Asynchronous callback to get the list of static
     *                               Network Address Translation (NAT) entries
     * @param [in] slotId            Specify slot id which has the sim that contains profile id
     *                               static entries to be retrieved from.
     *
     * @returns Status of requestStaticNatEntries i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    telux::common::Status requestStaticNatEntries(int profileId,
        StaticNatEntriesCb snatEntriesCb, SlotId slotId = DEFAULT_SLOT_ID) override;

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
     * Register a listener for Data Service heath events in the Vlan Manager like service available
     * or data service not available.
     *
     * @param [in] listener    pointer of INatListener object that processes the
     * notification
     *
     * @returns Status of registerListener success or suitable status code
     *
     */
    virtual telux::common::Status registerListener(std::weak_ptr<INatListener> listener) override;

    /**
     * Removes a previously added listener.
     *
     * @param [in] listener    pointer of INatListener object that needs to be removed
     *
     * @returns Status of deregisterListener success or suitable status code
     *
     */
    virtual telux::common::Status deregisterListener(std::weak_ptr<INatListener> listener) override;

    telux::common::Status init(telux::common::InitResponseCb callback);
    telux::common::Status cleanup();
    void onServiceStatusChange(telux::common::ServiceStatus status) override;

 private:
    /**
     * Perform synchronous initialization
     */
    void initSync();
    void setSubsystemReady(bool status);
    void setSubSystemStatus(telux::common::ServiceStatus status);
    bool waitForInitialization();
    void invokeInitCallback(telux::common::ServiceStatus status);

    std::mutex mtx_;
    std::condition_variable cv_;
    bool ready_ = false;
    bool isInitComplete_;
    telux::common::ServiceStatus subSystemStatus_;
    telux::common::InitResponseCb initCb_;
    telux::data::OperationType oprType_;
    std::shared_ptr<QcmClient> qcmClient_ = nullptr;
    telux::common::AsyncTaskQueue<void> taskQ_;
    std::shared_ptr<telux::common::ListenerManager<INatListener>> listenerMgr_;
};
}
}
}

#endif