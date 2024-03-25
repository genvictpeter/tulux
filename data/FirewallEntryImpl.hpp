/*
 *  Copyright (c) 2019 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef FIREWALLENTRYIMPL_HPP
#define FIREWALLENTRYIMPL_HPP

#include <telux/data/net/FirewallManager.hpp>

namespace telux {
namespace data {
namespace net {

/**
 * @brief   Firewall entry class is used for configuring firewall rules
 */
class FirewallEntryImpl : public IFirewallEntry {
 public:
    /**
     * Get IProtocol filter type
     *
     * @returns @ref telux::data::IIpFilter.
     *
     */
    std::shared_ptr<IIpFilter> getIProtocolFilter() override;

    /**
     * Get firewall direction
     *
     * @returns @ref telux::data::Direction.
     *
     */
    telux::data::Direction getDirection() override;

    /**
     * Get Ip FamilyType
     *
     * @returns @ref telux::data::IpFamilyType.
     *
     */
    telux::data::IpFamilyType getIpFamilyType() override;

    /**
     * Get Entry handler
     *
     * @returns uint32_t
     *
     */
    uint32_t getHandle() override;

    void setHandle(uint32_t handle);

    FirewallEntryImpl(
        std::shared_ptr<IIpFilter> ipfilter, Direction direction, IpFamilyType ipFamilyType);
    ~FirewallEntryImpl();

 private:
    std::shared_ptr<IIpFilter> ipFilter_;
    Direction direction_;
    IpFamilyType ipFamilyType_;
    uint32_t handle_;
};
}
}
}
#endif