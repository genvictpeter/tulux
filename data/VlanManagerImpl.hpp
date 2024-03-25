/*
 *  Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef VLANMANGERIMPL_HPP
#define VLANMANGERIMPL_HPP

#include <telux/data/net/VlanManager.hpp>

#include "common/AsyncTaskQueue.hpp"
#include "data/QcmClient.hpp"

namespace telux {
namespace data {
namespace net {

class VlanManagerImpl : public IVlanManager,
                        public IDataConnectionListener,
                        public std::enable_shared_from_this<VlanManagerImpl> {
 public:
    VlanManagerImpl(telux::data::OperationType oprType, std::shared_ptr<QcmClient> qcmapClient);
    ~VlanManagerImpl();
    /**
     * Checks the status of location manager and returns the result.
     *
     * @returns SERVICE_AVAILABLE      If Vlan manager object is ready for service.
     *          SERVICE_UNAVAILABLE    If Vlan manager object is temporarily unavailable.
     *          SERVICE_FAILED       - If Vlan manager object encountered an irrecoverable failure.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    virtual telux::common::ServiceStatus getServiceStatus() override;

    /**
     * Checks if the data subsystem is ready.
     *
     * @returns True if VLAN Manager is ready for service, otherwise
     * returns false.
     *
     */
    bool isSubsystemReady() override;

    /**
     * Wait for data subsystem to be ready.
     *
     * @returns A future that caller can wait on to be notified
     * when VLAN manager is ready.
     *
     */
    std::future<bool> onSubsystemReady() override;

    /**
     * Create a VLAN associated with multiple interfaces
     *
     * @note       if interface configured as VLAN for the first time, it may trigger auto reboot.
     *
     * @param [in] vlanConfig       vlan configuration
     * @param [out] callback        optional callback to get the response createVlan
     *
     * @returns Immediate status of createVlan() request sent i.e. success or suitable status
     * code.
     *
     * @note     Eval: This is a new API and is being evaluated.It is subject to change and could
     *           break backwards compatibility.
     *
     */
    telux::common::Status createVlan(
        const VlanConfig &vlanConfig, CreateVlanCb callback = nullptr) override;

    /**
     * Remove VLAN configuration
     *
     * @note   This will delete all clients associated with interface
     *
     * @param [in] vlanId       VLAN ID
     * @param [in] ifaceType       @ref telux::net::InterfaceType
     * @param [out] callback       optional callback to get the response removeVlan
     *
     * @returns Immediate status of removeVlan() request sent i.e. success or suitable status
     * code.
     *
     * @note     Eval: This is a new API and is being evaluated.It is subject to change and could
     *           break backwards compatibility.
     */
    telux::common::Status removeVlan(int16_t vlanId, InterfaceType ifaceType,
        telux::common::ResponseCallback callback = nullptr) override;

    /**
     * Query information about all the VLANs in the system
     *
     * @param [out] callback        Response callback with list of configured VLANs
     *
     * @returns Immediate status of queryVlanInfo() request sent i.e. success or suitable status
     * code.
     *
     * @note     Eval: This is a new API and is being evaluated.It is subject to change and could
     *           break backwards compatibility.
     */
    telux::common::Status queryVlanInfo(QueryVlanResponseCb callback) override;

    /**
     * Bind a Vlan with a particular profile ID. When a WWAN network interface is
     * brought up using IDataConnectionManager::startDataCall on that profile ID,
     * that interface will be accessible from this Vlan
     *
     * @param [in] profileId    profile id for vlan association
     * @param [in] vlanId       sets vlan id
     * @param [out] callback    callback to get the response of associateWithProfileId API
     * @param [in] slotId       Specify slot id which has the sim that contains profile id.
     *
     * @returns Immediate status of associateWithProfileId() request sent i.e. success or
     * suitable status code.
     *
     * @note     Eval: This is a new API and is being evaluated.It is subject to change and could
     *           break backwards compatibility.
     */
    telux::common::Status bindWithProfile(int profileId, int vlanId,
        telux::common::ResponseCallback callback = nullptr, SlotId slotId = DEFAULT_SLOT_ID) override;

    /**
     * Unbind VLAN id with given profile id
     *
     * @param [in] profileId    profile id for vlan association
     * @param [in] vlanId       vlan id
     * @param [in] callback     callback to get the response of associateWithProfileId API
     * @param [in] slotId       Specify slot id which has the sim that contains profile id.
     *
     * @returns Immediate status of disassociateFromProfileId() request sent i.e. success or
     * suitable status code
     *
     * @note     Eval: This is a new API and is being evaluated.It is subject to change and could
     *           break backwards compatibility.
     */
    telux::common::Status unbindFromProfile(int profileId, int vlanId,
        telux::common::ResponseCallback callback = nullptr, SlotId slotId = DEFAULT_SLOT_ID) override;

    /**
     * Query VLAN mapping list with associated profile id and vlan id
     *
     * @param [in] callback    callback to get the response of queryVlanMappingList API
     * @param [in] slotId      Specify slot id which has the sim that contains profile id
     *                         mapping to vlan id.
     *
     * @returns Immediate status of queryVlanMappingList() request sent i.e. success or
     * suitable status code
     *
     * @note     Eval: This is a new API and is being evaluated.It is subject to change and could
     *           break backwards compatibility.
     */
    telux::common::Status queryVlanMappingList(VlanMappingResponseCb callback,
        SlotId slotId = DEFAULT_SLOT_ID) override;

    /**
     * Register a listener for Data Service health events in the Vlan Manager like service available
     * or data service not available.
     *
     * @param [in] listener    pointer of IVlanListener object that processes the
     * notification
     *
     * @returns Status of registerListener success or suitable status code
     *
     */
    virtual telux::common::Status registerListener(std::weak_ptr<IVlanListener> listener) override;

    /**
     * Removes a previously added listener.
     *
     * @param [in] listener    pointer of IVlanListener object that needs to be removed
     *
     * @returns Status of deregisterListener success or suitable status code
     *
     */
    virtual telux::common::Status deregisterListener(
        std::weak_ptr<IVlanListener> listener) override;

    /**
     * Get the associated operation type for this instance.
     *
     * @returns OperationType of getOperationType i.e. LOCAL or REMOTE.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    telux::data::OperationType getOperationType() override;

    telux::common::Status cleanup();
    telux::common::Status init(telux::common::InitResponseCb callback);
    void onServiceStatusChange(telux::common::ServiceStatus status) override;
    void onHwAccelerationChanged(telux::data::ServiceState state) override;
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
    std::condition_variable cv_;
    bool ready_ = false;
    bool isInitComplete_;
    telux::common::ServiceStatus subSystemStatus_;
    telux::common::InitResponseCb initCb_;
    telux::data::OperationType oprType_;
    std::shared_ptr<QcmClient> qcmClient_ = nullptr;
    telux::common::AsyncTaskQueue<void> taskQ_;
    std::shared_ptr<telux::common::ListenerManager<IVlanListener>> listenerMgr_;
};
}
}
}

#endif
