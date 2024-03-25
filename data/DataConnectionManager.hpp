/*
 *  Copyright (c) 2018-2021, The Linux Foundation. All rights reserved.
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

/*
 *  Changes from Qualcomm Innovation Center are provided under the following license:

 *  Copyright (c) 2021 Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the
 * disclaimer below) provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *
 *     * Neither the name of Qualcomm Innovation Center, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
 * GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
 * HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file       DataConnectionManager.hpp
 *
 * @brief      DataConnectionManager is a primary interface for cellular connectivity. This
 *             interface provides APIs for start and stop data call connections, get data
 *             call information and add or remove listeners for monitoring data calls
 *             status.
 *
 */

#ifndef DATACONNECTIONMANAGER_HPP
#define DATACONNECTIONMANAGER_HPP

#include <future>
#include <vector>
#include <list>
#include <memory>

#include <telux/data/DataDefines.hpp>
#include <telux/data/DataProfile.hpp>
#include <telux/data/IpFilter.hpp>

#include <telux/common/CommonDefines.hpp>

namespace telux {
namespace data {

/** @addtogroup telematics_data
 * @{ */

// Forward declarations
class IDataConnectionListener;
class IDataCall;

/**
 * IP Family related Info
 */
struct IpFamilyInfo {
    DataCallStatus status;
    IpAddrInfo addr;
};

/**
 * Encapsulate the Qos Filter rule
 */
struct QosFilterRule{
    std::vector<std::shared_ptr<IIpFilter>> filter; /**< @ref IIpFilter */
    uint16_t filterId;                              /**< Unique identifier for each filter. */
    uint16_t filterPrecedence;                      /**< Specifies the order in which filters are
                                                         applied. A lower numerical value has a
                                                         higher precedence. */
};

/**
 * QOS TFT Flow info
 */
struct TrafficFlowTemplate {
    /** Mandatory */
    QosFlowId qosId;                            /**< defines current flow id */
    QosFlowStateChangeEvent stateChange;        /**< Flow state change event */

    QosFlowMask mask;                           /**< bitmask to denote which of the optional fields
                                                     in TrafficFlowTemplate are valid */
    /** Optional */
    QosIPFlowInfo txGrantedFlow;                /* Tx Granted Flow IP info */
    QosIPFlowInfo rxGrantedFlow;                /* Rx Granted Flow IP info */

    uint32_t txFiltersLength;                   /* Tx Filters length */
    QosFilterRule txFilters[MAX_QOS_FILTERS];   /* Tx QoS Filters that apply to a
                                                   granted Tx QoS flow. */

    uint32_t rxFiltersLength;                   /* Rx Filters length*/
    QosFilterRule rxFilters[MAX_QOS_FILTERS];   /* Rx QoS Filters that apply to a
                                                   granted Rx QoS flow. */
};

/**
 * QOS TFT flow change info
 */
struct TftChangeInfo {
    std::shared_ptr<TrafficFlowTemplate> tft;   /**< TFT flow info @ref TrafficFlowTemplate */
    QosFlowStateChangeEvent stateChange;        /**< Flow state change event */
};

/**
 * Data call bit rate info
 */
struct BitRateInfo {
    uint64_t txRate;      /**< Instantaneous channel transmit rate in bits/sec                  */
    uint64_t rxRate;      /**< Instantaneous channel receive rate in bits/sec                   */
    uint64_t maxTxRate;   /**< Maximum transmit rate that can be assigned to device in bits/sec */
    uint64_t maxRxRate;   /**< Maximum receive rate that can be assigned to device in bits/sec  */
};

/**
 * This function is called with the response to startDataCall / stopDataCall API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * When callback is used with startDataCall, expected behavior is as following:
 *  - If this is first client to start datacall in the system and no error is detected, state of
 *    data call will be NET_CONNECTING and onDataCallInfoChanged will be called once data call
 *    is brought up successfully or failed.
 *  - If client tries to start data call that is already up and no error is detected, state of data
 *    call will NET_CONNECTED and onDataCallInfoChanged will not get called.
 *  - If any client that start data call and error is detected, error argument will contain error
 *    code and onDataCallInfoChanged will not get called.

 * When callback is used with stopDataCall, expected behavior is as following:
 *  - First/Last client that attempts to stop data call and no error is detected, state of data call
 *    will be NET_DISCONNECTING and onDataCallInfoChanged will be called once data call is down.
 *  - If a client starts a data call and then tries to stop it while there are other clients in
 *    the system who also started the same data call, and no error is detected, data call status
 *    will be NET_CONNECTED and onDataCallInfoChanged will not get called.
 *  - If any client attemp to stop data call and error detected, error argument will contain error
 *    code and onDataCallInfoChanged will not get called.
 *
 * @param [in] dataCall        Pointer to IDataCall
 * @param [in] error           Return code for whether the operation succeeded or failed
 *
 */
using DataCallResponseCb = std::function<void(
    const std::shared_ptr<IDataCall> &dataCall, telux::common::ErrorCode error)>;

/**
 * This function is called with the response to requestDataCallStatistics API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [in] dataStats       Data Call statistics
 * @param [in] error           Return code for whether the operation
 *                             succeeded or failed
 */
using StatisticsResponseCb
    = std::function<void(const DataCallStats dataStats, telux::common::ErrorCode error)>;

/**
 * This function is called with the response to requestDataCallList API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [in] dataCall        vector of of IDataCall list
 * @param [in] error           Return code for whether the operation
 *                             succeeded or failed
 *
 */
using DataCallListResponseCb = std::function<void(
    const std::vector<std::shared_ptr<IDataCall>> &dataCallList, telux::common::ErrorCode error)>;

/**
 * This function is called with the response to getDefaultProfile API.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [in] profileId       Current default profile id
 * @param [in] slotId          Slot id that contains current default profile id
 * @param [in] error           Return code for whether the operation
 *                             succeeded or failed
 */
using DefaultProfileIdResponseCb
    = std::function<void(int profileId, SlotId slotId, telux::common::ErrorCode error)>;

/**
 * This function is called in the response to requestTrafficFlowTemplate().
 *
 * @param [in] tft        Vector of TFT flow info. @ref TrafficFlowTemplate
 * @param [in] error      Code which indicates whether the operation succeeded or not.
 *                        @ref ErrorCode.
 */
using TrafficFlowTemplateCb =
    std::function<void(const std::vector<std::shared_ptr<TrafficFlowTemplate>> &tft,
        telux::common::ErrorCode error)>;

/**
 * This function is called in response to requestDataCallBitRate.
 *
 * The callback can be invoked from multiple different threads.
 * The implementation should be thread safe.
 *
 * @param [in] bitRate         Bit Rate Info for requested data call
 * @param [in] error           Return code for whether the operation succeeded or failed
 *
 */
using requestDataCallBitRateResponseCb
    = std::function<void(BitRateInfo& bitRate, telux::common::ErrorCode error)>;

/**
 *@brief IDataConnectionManager is a primary interface for cellular connectivity
 *       This interface provides APIs for start and stop data call connections,
 *       get data call information and listener for monitoring data calls.
 *       It also provides interface to Subsystem Restart events by registering as listener.
 *       Notifications will be received when modem is ready/not ready.
 */
class IDataConnectionManager {
 public:
    /**
     * Checks the status of data connection manager and returns the result.
     *
     * @returns SERVICE_AVAILABLE    If data connection manager is ready for service.
     *          SERVICE_UNAVAILABLE  If data connection manager is temporarily unavailable.
     *          SERVICE_FAILED       If data connection manager encountered an irrecoverable failure.
     *
     */
    virtual telux::common::ServiceStatus getServiceStatus() = 0;

    /**
     * Checks if the data subsystem is ready.
     *
     * @returns True if Data Connection Manager is ready for service, otherwise
     * returns false.
     *
     *  @deprecated Use getServiceStatus API.
     */
    virtual bool isSubsystemReady() = 0;

    /**
     * Wait for data subsystem to be ready.
     *
     * @returns A future that caller can wait on to be notified
     * when card manager is ready.
     *
     * @deprecated Use InitResponseCb callback in factory API getDataConnectionManager.
     */
    virtual std::future<bool> onSubsystemReady() = 0;

   /**
    * Set a profile as default which results in the following:.
    *  - Default routes in the system will route traffic over the network interface associated
    *    with this profile.
    *  - Bridge 0 will be associated with traffic from this profile.
    *
    * @param [in] operationType     @ref telux::data::OperationType
    * @param [in] profileId         Profile identifier to be associated with default handler
    * @param [in] callback          optional callback to get the response setDefaultProfile
    *
    * @returns Immediate status of setDefaultProfile i.e. success or suitable status.
    *
    */
   virtual telux::common::Status setDefaultProfile(OperationType oprType, uint8_t profileId,
       telux::common::ResponseCallback callback = nullptr)  = 0;

   /**
    * Get current default profile to which associated network traffic is routed through bridge 0
    * and default system routes.
    *
    * @param [in] operationType     @ref telux::data::OperationType
    * @param [in] callback          callback to get the response getDefaultProfile
    *
    * @returns Immediate status of getDefaultProfile i.e. success or suitable status.
    *
    */
   virtual telux::common::Status getDefaultProfile(
       OperationType oprType, DefaultProfileIdResponseCb callback)  = 0;

    /**
     * Starts a data call corresponding to default or specified profile identifier.
     *
     * This will bring up data call connection based on specified profile identifier. This is an
     * asynchronous API, client receives notification indicating the data call establishment
     * or failure in callback.
     *
     * @note       if application starts data call on IPV4V6 then it's expected to stop the
     *             data call on same ip family type (i.e IPV4V6).
     *
     * @param [in] profileId     Profile identifier corresponding to which data call bring up
     *                           will be done. Use IDataProfileManager::requestProfileList to get
     *                           list of available profiles.
     * @param [in] ipFamilyType  Identifies IP family type
     * @param [out] callback     Optional callback to get the response of start data call.
     * @param [in] operationType Optional @ref telux::data::OperationType
     * @param [in] apn           Deprecated and currently unsued
     *
     * @returns Immediate status of startDataCall() request sent
     *                   i.e. success or suitable status code.
     *
     *
     */
    virtual telux::common::Status startDataCall(int profileId,
        IpFamilyType ipFamilyType = IpFamilyType::IPV4V6, DataCallResponseCb callback = nullptr,
        OperationType operationType = OperationType::DATA_LOCAL, std::string apn = "")
        = 0;

    /**
     * Stops a data call corresponding to default or specified profile identifier.
     *
     * This will tear down specific data call connection based on profile identifier.
     *
     * @note       If application starts data call on IPV4V6 then it's expected to stop the
     *             data call on same ip family type (i.e IPV4V6).
     *             Client can only stop data call it started.
     *
     * @param [in] profileId     Profile identifier corresponding to which data call tear down
     *                           will be done. Use data profile manager to get the list of
     *                           available profiles.
     * @param [in] ipFamilyType  Identifies IP family type
     * @param [out] callback     Optional callback to get the response of stop data call
     * @param [in] operationType Optional @ref telux::data::OperationType
     * @param [in] apn           Deprecated and currently unused
     *
     * @returns Immediate status of stopDataCall() request sent i.e. success or
     *          suitable status code. The client receives asynchronous notifications
     *          indicating the data call tear-down.
     *
     *
     */
    virtual telux::common::Status stopDataCall(int profileId,
        IpFamilyType ipFamilyType = IpFamilyType::IPV4V6, DataCallResponseCb callback = nullptr,
        OperationType operationType = OperationType::DATA_LOCAL, std::string apn = "")
        = 0;

    /**
     * Register a listener for specific events in the Connection Manager like establishment of new
     * data call, data call info change and call failure.
     *
     * @param [in] listener    pointer of IDataConnectionListener object that processes the
     * notification
     *
     * @returns Status of registerListener success or suitable status code
     *
     */
    virtual telux::common::Status registerListener(std::weak_ptr<IDataConnectionListener> listener)
        = 0;

    /**
     * Removes a previously added listener.
     *
     * @param [in] listener    pointer of IDataConnectionListener object that needs to be removed
     *
     * @returns Status of deregisterListener success or suitable status code
     *
     */
    virtual telux::common::Status deregisterListener(
        std::weak_ptr<IDataConnectionListener> listener) = 0;

    /**
     * Get associated slot id for the Data Connection Manager.
     *
     * @returns SlotId
     *
     */
    virtual int getSlotId() = 0;

    /**
     * Request list of data calls available in the system
     *
     * @param [out] OperationType    @ref telux::data::OperationType
     * @param [out] callback         Callback with list of supported data calls
     *
     */
    virtual telux::common::Status requestDataCallList(OperationType type,
        DataCallListResponseCb callback) = 0;

    /**
     * Destructor for IDataConnectionManager
     */
    virtual ~IDataConnectionManager(){};
};  // end of IDataConnectionManager

/**
 * @brief Represents single established data call on the device.
 *
 */
class IDataCall {
 public:
    /**
     * Get interface name for the data call associated.
     *
     * @returns Interface Name.
     *
     */
    virtual const std::string &getInterfaceName() = 0;

    /**
     * Get the bearer technology on which earlier data call was brought up like LTE, WCDMA and etc.
     * This is synchronous API called by client to get bearer technology corresponding to data call.
     *
     * @returns @ref DataBearerTechnology
     *
     */
    virtual DataBearerTechnology getCurrentBearerTech() = 0;

    /**
     * Get failure reason for the data call.
     *
     * @returns @ref DataCallFailReason.
     *
     */
    virtual DataCallEndReason getDataCallEndReason() = 0;

    /**
     * Get data call status like connected, disconnected and IP address changes.
     *
     * @returns @ref DataCallStatus.
     *
     */
    virtual DataCallStatus getDataCallStatus() = 0;

    /**
     * Get IPv4 Family info like connected, disconnected and IP address changes.
     *
     * @returns @ref IpFamilyInfo.
     *
     */
    virtual IpFamilyInfo getIpv4Info() = 0;

    /**
     * Get IPv6 Family info like connected, disconnected and IP address changes.
     *
     * @returns @ref IpFamilyInfo.
     *
     */
    virtual IpFamilyInfo getIpv6Info() = 0;

    /**
     * Get the technology on which the call was brought up.
     *
     * @returns @ref TechPreference.
     *
     */
    virtual TechPreference getTechPreference() = 0;

    /**
     * Get list of IP address information.
     *
     * @returns List of IP address details.
     *
     */
    virtual std::list<IpAddrInfo> getIpAddressInfo() = 0;

    /**
     * Get IP Family Type i.e. IPv4, IPv6 or Both
     *
     * @returns @ref IpFamilyType.
     *
     */
    virtual IpFamilyType getIpFamilyType() = 0;

    /**
     * Get Profile Id
     *
     * @returns Profile Identifier.
     *
     */
    virtual int getProfileId() = 0;

    /**
     * Get Slot Id
     *
     * @returns Subscription Slot Identifier.
     *
     */
    virtual SlotId getSlotId() = 0;

    /**
     * Get data operation used for the DataCall.
     *
     * @returns @ref OperationType
     */
    virtual OperationType getOperationType() = 0;

    /**
     * Get the current installed QOS Traffic flow template information.
     *
     * @param [in]  ipFamilyType    - IP Family type @ref IpFamilyType. TFT's are installed per IP
     *                                Family.
     * @param [in]  callback        - callback function to get the result of API.
     *
     * @returns Status of requestTrafficFlowTemplate i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    virtual telux::common::Status requestTrafficFlowTemplate(IpFamilyType ipFamilyType,
        TrafficFlowTemplateCb callback) = 0;

    /**
     * Request the data transfer statistics for data call corresponding
     * to specified profile identifier.
     *
     * @param [in] callback    Optional callback to get the response of request Data Call
     *                         Statistics
     *
     * @returns Status of getDataCallStatistics i.e. success or suitable status code.
     */
    virtual telux::common::Status requestDataCallStatistics(StatisticsResponseCb callback = nullptr)
        = 0;

    /**
     * Reset data transfer statistics for data call corresponding to specified profile identifier.
     *
     * @param [in] callback   optional callback to get the response of reset Data call statistics
     *
     * @returns Status of resetDataCallStatistics i.e. success or suitable status code.
     */
    virtual telux::common::Status resetDataCallStatistics(
        telux::common::ResponseCallback callback = nullptr)
        = 0;

    /**
     * Request data call bit rate in (bits/sec).
     *
     * @param [out] callback     callback to be called with bit rate results
     *                           @ref requestDataCallBitRateResponseCb
     *
     * @returns Status of requestDataCallBitRate success or suitable status code
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    virtual telux::common::Status requestDataCallBitRate(
        requestDataCallBitRateResponseCb callback) = 0;

    /**
     * Destructor for IDataCall
     */
    virtual ~IDataCall(){};
};

/**
 * Interface for Data connection listener object. Client needs to implement this interface to get
 * access to data services notifications like onNewDataCall, onDataCallStatusChanged and
 * onDataCallFailure.
 *
 * The methods in listener can be invoked from multiple different threads. The implementation
 * should be thread safe.
 *
 * The notification delivery mechanism uses the same thread to deliver all the queued notifications
 * to ensure they are delivered in order.
 * Considering this, the thread on which the notifications are delivered should not be blocked for
 * longer operations since this would result in delay in delivery of further notifications that are
 * in the queue waiting to be dispatched.
 *
 */
class IDataConnectionListener : public telux::common::IServiceStatusListener {
 public:
    /**
     * This function is called when there is a change in the data call.
     *
     * @param [in] dataCall   Pointer to IDataCall
     *
     */
    virtual void onDataCallInfoChanged(const std::shared_ptr<IDataCall> &dataCall){};

    /**
     * This function is called when a change occur in hardware acceleration service.
     *
     * @param [in] state   New state of hardware Acceleration service (Active/Inactive)
     *
     */
    virtual void onHwAccelerationChanged(const ServiceState state){};

    /**
     * This function is called when the TFT's parameters are changed for a packet data session.
     *
     * @param [in] dataCall     Pointer to IDataCall
     * @param [in] tft          vector of TftChangeInfo @ref TftChangeInfo
     *
     * @note     Eval: This is a new API and is being evaluated. It is subject to change and could
     *           break backwards compatibility.
     */
    virtual void onTrafficFlowTemplateChange(const std::shared_ptr<IDataCall> &dataCall,
        const std::vector<std::shared_ptr<TftChangeInfo>> &tft) {};

    /**
     * This function is called when WWAN backhaul connectivity config changes.
     *
     * @param [in] slotId                - Slot Id for which connectivity has changed.
     * @param [in] isConnectivityAllowed - Connectivity status allowed/disallowed.
     *
     */
    virtual void onWwanConnectivityConfigChange(SlotId slotId, bool isConnectivityAllowed) {}

    /**
     * Destructor for IDataConnectionListener
     */
    virtual ~IDataConnectionListener(){};
};

/** @} */ /* end_addtogroup telematics_data */
}
}

#endif
