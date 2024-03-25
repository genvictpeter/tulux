/*
 *  Copyright (c) 2020 The Linux Foundation. All rights reserved.
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
 * @file       L2tpManager.hpp
 *
 * @brief      L2tpManager is a primary interface for configuring L2TP feature.
 *             Currently only un-managed tunnels are supported
 *
 */

#ifndef L2TPMANAGER_HPP
#define L2TPMANAGER_HPP

#include <future>
#include <vector>
#include <list>
#include <memory>

#include <telux/common/CommonDefines.hpp>

#include <telux/data/DataDefines.hpp>

namespace telux {
namespace data {
namespace net {

/** @addtogroup telematics_data_net
 * @{ */

// Forward declarations
class IL2tpListener;

/**
 * L2TP encapsulation protocols
 */
enum class L2tpProtocol {
    NONE = 0,
    IP   = 0x01,   /**< IP Protocol used for encapsulation */
    UDP  = 0x02    /**< UDP Protocol used for encapsulation */
};

/**
 * L2TP tunnel sessions configuration
 */
struct L2tpSessionConfig {
    uint32_t locId;      /**< Local session id */
    uint32_t peerId;     /**< Peer session id  */
};

/**
 * L2TP tunnel configuration
 */
struct L2tpTunnelConfig {
    L2tpProtocol                prot;          /**< Encapsulation protocols */
    uint32_t                    locId;         /**< Local tunnel id */
    uint32_t                    peerId;        /**< Peer tunnel id */
    uint32_t                    localUdpPort;  /**< Local udp port - if UDP encapsulation is used */
    uint32_t                    peerUdpPort;   /**< Peer udp port - if IP encapsulation is used */
    std::string                 peerIpv6Addr;  /**< Peer IPv6 Address - for Ipv6 tunnels */
    std::string                 peerIpv4Addr;  /**< Peer IPv4 Address - for Ipv4 tunnels */
    std::string                 locIface;      /**< interface name to create L2TP tunnel on */
    telux::data::IpFamilyType   ipType;        /**< Ip family type @ref telux::data::IpFamilyType */
    std::vector<L2tpSessionConfig> sessionConfig;  /**< List of L2tp tunnel sessions */
};

/**
 * L2TP Configuration
 */
struct L2tpSysConfig {
    std::vector<L2tpTunnelConfig>configList;       /**< List of L2tp tunnel configurations */
    bool enableMtu;   /**< Enable MTU size setting on underlying interfaces to avoid segmentation */
    bool enableTcpMss;   /**< Enable TCP MSS clampping on L2TP interfaces to avoid segmentation */
    uint32_t mtuSize; /**< Current MTU size in bytes */
};

/**
 * This function is called as a response to @ref requestConfig()
 *
 * @param [in] l2tpSysConfig       Current L2TP configuration
 * @param [in] error          Return code which indicates whether the operation
 *                            succeeded or not @ref telux::common::ErrorCode
 *
 */
using L2tpConfigCb
    = std::function<void(const L2tpSysConfig &l2tpSysConfig, telux::common::ErrorCode error)>;

/**
 *@brief    L2tpManager is a primary interface for configuring L2TP Service.
 *          It also provides interface to Subsystem Restart events by registering as listener.
 *          Notifications will be received when modem is ready/not ready.
 */
class IL2tpManager {
 public:
    /**
     * Checks the status of L2tp manager and returns the result.
     *
     * @returns SERVICE_AVAILABLE      If L2tp manager is ready for service.
     *          SERVICE_UNAVAILABLE    If L2tp manager is temporarily unavailable.
     *          SERVICE_FAILED       - If L2tp manager encountered an irrecoverable failure.
     *
     */
    virtual telux::common::ServiceStatus getServiceStatus() = 0;

    /**
     * Checks if the L2tp manager subsystem is ready.
     *
     * @returns True if L2tp Manager is ready for service, otherwise
     *          returns false.
     *
     * @note    This API will be deprecated. getServiceStatus API is recommended as an alternative
     */
    virtual bool isSubsystemReady() = 0;

    /**
     * Wait for L2tp manager subsystem to be ready.
     *
     * @returns A future that caller can wait on to be notified
     *          when L2tp manager is ready.
     *
     * @note    This API will be deprecated. Callback of type InitResponseCb argument in data
     *          factory API getL2tpManager is recommended as an alternative.
     */
    virtual std::future<bool> onSubsystemReady() = 0;

    /**
     * Enable L2TP for unmanaged Tunnel State
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
     */
    virtual telux::common::Status setConfig(bool enable, bool enableMss, bool enableMtu,
        telux::common::ResponseCallback callback = nullptr, uint32_t mtuSize = 0) = 0;

    /**
     * Set L2TP Configuration for one tunnel
     *
     * @param [in] l2tpTunnelConfig     Configuration to be set @ref telux::net::L2tpTunnelConfig
     * @param [in] callback             Optional callback to get the response addTunnel
     *
     * @returns Status of addTunnel i.e. success or suitable status code.
     *
     */
    virtual telux::common::Status addTunnel(const L2tpTunnelConfig &l2tpTunnelConfig,
        telux::common::ResponseCallback callback = nullptr) = 0;

    /**
     * Get Current L2TP Configuration
     *
     * @param [in] l2tpConfigCb      Asynchronous callback to get current L2TP configurations
     *
     * @returns Status of requestConfig i.e. success or suitable status code.
     *
     */
    virtual telux::common::Status requestConfig(L2tpConfigCb l2tpConfigCb) = 0;

    /**
     * Remove L2TP Tunnel
     *
     * @param [in] tunnelId          Tunnel ID to be removed
     * @param [in] callback          optional callback to get the response removeConfig
     *
     * @returns Status of removeTunnel i.e. success or suitable status code.
     *
     */
    virtual telux::common::Status removeTunnel(
        uint32_t tunnelId, telux::common::ResponseCallback callback = nullptr) = 0;

    /**
     * Register L2TP Manager as listener for Data Service heath events like data service available
     * or data service not available.
     *
     * @param [in] listener    pointer of IL2tpListener object that processes the
     * notification
     *
     * @returns Status of registerListener success or suitable status code
     *
     */
    virtual telux::common::Status registerListener(std::weak_ptr<IL2tpListener> listener) = 0;

    /**
     * Removes a previously added listener.
     *
     * @param [in] listener    pointer of IL2tpListener object that needs to be removed
     *
     * @returns Status of deregisterListener success or suitable status code
     *
     */
    virtual telux::common::Status deregisterListener(std::weak_ptr<IL2tpListener> listener) = 0;

    /**
     * Destructor for IL2tpManager
     */
    virtual ~IL2tpManager(){};
};  // end of IL2tpManager

/**
 * Interface for L2TP listener object. Client needs to implement this interface to get
 * access to L2TP services notifications like onServiceStatusChange.
 *
 * The methods in listener can be invoked from multiple different threads. The implementation
 * should be thread safe.
 *
 */
class IL2tpListener {
 public:
    /**
     * This function is called when service status changes.
     *
     * @param [in] status - @ref ServiceStatus
     */
    virtual void onServiceStatusChange(telux::common::ServiceStatus status) {}

    /**
     * Destructor for IL2tpListener
     */
    virtual ~IL2tpListener(){};
};

/** @} */ /* end_addtogroup telematics_data_net */
}
}
}

#endif
