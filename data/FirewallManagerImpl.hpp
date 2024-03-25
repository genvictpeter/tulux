/*
 *  Copyright (c) 2019 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef FIREWALLMANGERIMPL_HPP
#define FIREWALLMANGERIMPL_HPP

#include <telux/data/net/FirewallManager.hpp>

#include "common/AsyncTaskQueue.hpp"
#include "data/QcmClient.hpp"

namespace telux {
namespace data {
namespace net {

/**
 * @brief   Firewall entry class is used for configuring firewall rules
 */
class FirewallManagerImpl : public IFirewallManager,
                        public IDataConnectionListener,
                        public std::enable_shared_from_this<FirewallManagerImpl> {
 public:
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
    virtual telux::common::ServiceStatus getServiceStatus() override;

    /**
     * Checks if the data subsystem is ready.
     *
     * @returns True if Firewall Manager is ready for service, otherwise
     * returns false.
     *
     * @deprecated Use getServiceStatus API.
     */
    bool isSubsystemReady() override;

    /**
     * Wait for data subsystem to be ready.
     *
     * @returns A future that caller can wait on to be notified
     * when firewall manager is ready.
     *
     * @deprecated Use InitResponseCb callback in factory API getNewFirewallEntry.
     */
    std::future<bool> onSubsystemReady() override;

    /**
     * Sets firewall configuration to enable or disable and update configuration to
     * drop or accept the packets matching the rules.
     *
     * @param [in] profileId         Profile identifier on which firewall will be set.
     * @param [in] enable            Indicates whether the firewall is enabled
     * @param [in] allowPackets      Indicates whether to accept or drop packets
     *                               matching the rules
     * @param [in] callback          optional callback to get the response setFirewall
     * @param [in] slotId            Specify slot id which has the sim that contains profile id
     *
     * @returns Status of setFirewall i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    telux::common::Status setFirewall(int profileId, bool enable, bool allowPackets,
        telux::common::ResponseCallback callback = nullptr, SlotId slotId = DEFAULT_SLOT_ID) override;

    /**
     * Request status of firewall
     *
     * @param [in] profileId         Profile identifier for which firewall status is requested.
     * @param [in] callback          callback to get the response of requestFirewallStatus
     * @param [in] slotId            Specify slot id which has the sim that contains profile id
     *
     * @returns Status of requestFirewallStatus i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    telux::common::Status requestFirewallStatus(int profileId,
        FirewallStatusCb callback, SlotId slotId = DEFAULT_SLOT_ID) override;

    /**
     * Adds the firewall rule
     *
     * @param [in] profileId        Profile identifier on which firewall rule will be added.
     * @param[in] entry             Firewall entry based on protocol type
     * @param[in] callback          optional callback to get the response addFirewallEntry
     * @param [in] slotId           Specify slot id which has the sim that contains profile id
     *
     * @returns Status of addFirewallEntry i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    telux::common::Status addFirewallEntry(int profileId, std::shared_ptr<IFirewallEntry> entry,
        telux::common::ResponseCallback callback = nullptr, SlotId slotId = DEFAULT_SLOT_ID) override;

    /**
     * Request Firewall rules
     *
     * @param[in] profileId         Profile identifier on which firewall entries are retrieved.
     * @param[in] callback          callback to get the response requestFirewallEntries.
     * @param [in] slotId           Specify slot id which has the sim that contains profile id
     *
     * @returns Status of requestFirewallEntries i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    telux::common::Status requestFirewallEntries(int profileId,
        FirewallEntriesCb callback, SlotId slotId = DEFAULT_SLOT_ID) override;

    /**
     * Remove firewall entry
     *
     * @param[in] profileId         Profile identifier on which firewall entry will be removed.
     * @param[in] handle            handle of Firewall entry to be removed.
     * @param[in] callback          callback to get the response removeFirewallEntry
     * @param[in] slotId            Specify slot id which has the sim that contains profile id
     *
     * @returns Status of removeFirewallEntry i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    telux::common::Status removeFirewallEntry(int profileId, uint32_t handle,
        telux::common::ResponseCallback callback = nullptr, SlotId slotId = DEFAULT_SLOT_ID) override;

    /**
     * Enable demilitarized zone (DMZ)
     *
     * @param [in] profileId     Profile identifier on which DMZ will be enabled.
     * @param [in] ipAddr        IP address for which DMZ will be enabled
     * @param [in] callback      optional callback to get the response addDmz
     * @param [in] slotId        Specify slot id which has the sim that contains profile id
     *
     * @returns Status of enableDmz i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    telux::common::Status enableDmz(int profileId, const std::string ipAddr,
        telux::common::ResponseCallback callback = nullptr, SlotId slotId = DEFAULT_SLOT_ID) override;

    /**
     * Disable demilitarized zone (DMZ)
     *
     * @param [in] profileId     Profile identifier on which DMZ will be disabled.
     * @param [in] ipType        Specify IP type of the DMZ to be disabled
     * @param [in] callback      optional callback to get the response removeDmz
     * @param [in] slotId        Specify slot id which has the sim that contains profile id
     *
     * @returns Status of disableDmz i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    telux::common::Status disableDmz(int profileId, const telux::data::IpFamilyType ipType,
        telux::common::ResponseCallback callback = nullptr, SlotId slotId = DEFAULT_SLOT_ID) override;

    /**
     * Request DMZ entry that was previously set using enableDmz API
     *
     * @param [in] profileId     Profile identifier on which DMZ entries are requested.
     * @param [in] dmzCb         callback to get the response requestDmzEntry
     * @param [in] slotId        Specify slot id which has the sim that contains profile id
     *
     * @returns Status of requestDmzEntry i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    telux::common::Status requestDmzEntry(int profileId,
        DmzEntriesCb dmzCb, SlotId slotId = DEFAULT_SLOT_ID) override;

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
     * @param [in] listener    pointer of IFirewallListener object that processes the
     * notification
     *
     * @returns Status of registerListener success or suitable status code
     *
     */
    virtual telux::common::Status registerListener(
        std::weak_ptr<IFirewallListener> listener) override;

    /**
     * Removes a previously added listener.
     *
     * @param [in] listener    pointer of IFirewallListener object that needs to be removed
     *
     * @returns Status of deregisterListener success or suitable status code
     *
     */
    virtual telux::common::Status deregisterListener(
        std::weak_ptr<IFirewallListener> listener) override;

    FirewallManagerImpl(telux::data::OperationType oprType, std::shared_ptr<QcmClient> qcmapClient);
    ~FirewallManagerImpl();

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
    void invokeInitCallback(telux::common::ServiceStatus status);
    bool waitForInitialization();

    std::mutex mtx_;
    std::condition_variable cv_;
    telux::common::ServiceStatus subSystemStatus_;
    telux::common::InitResponseCb initCb_;
    bool isInitComplete_;
    bool ready_ = false;
    telux::data::OperationType oprType_;
    std::shared_ptr<QcmClient> qcmClient_ = nullptr;
    telux::common::AsyncTaskQueue<void> taskQ_;
    std::shared_ptr<telux::common::ListenerManager<IFirewallListener>> listenerMgr_;
};
}
}
}
#endif