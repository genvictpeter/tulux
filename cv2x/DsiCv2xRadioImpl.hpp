/*
 *  Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef DSICV2XRADIOIMPL_HPP
#define DSICV2XRADIOIMPL_HPP

#include <deque>
#include <memory>
#include <vector>
#include <atomic>

#include <telux/cv2x/Cv2xRadio.hpp>
#include "common/AsyncTaskQueue.hpp"
#include "data/DsiClient.hpp"

using telux::common::ResponseCallback;

namespace telux {

namespace qmi {
class NasQmiClient;
}

namespace cv2x {

class Cv2xIndsListener;
class Cv2xSysCtrl;
class TxStatusReportListener;
class Cv2xDataCallListener;
struct NwIfaceInfo;

struct GlobalIPSession
{
    common::ResponseCallback userCb;
    IPv6AddrType prefix;
};

class DsiCv2xRadio : public ICv2xRadio {

public:

    DsiCv2xRadio();

    void init();

    Cv2xRadioCapabilities getCapabilities() const;

    bool isInitialized() const;

    bool isReady() const;

    std::future<telux::common::Status> onReady();

    telux::common::Status registerListener(std::weak_ptr<ICv2xRadioListener> listener);

    telux::common::Status deregisterListener(std::weak_ptr<ICv2xRadioListener> listener);

    telux::common::Status createRxSubscription(
        TrafficIpType ipType, uint16_t port, CreateRxSubscriptionCallback cb,
        std::shared_ptr<std::vector<uint32_t>> idList = nullptr);
    telux::common::Status enableRxMetaDataReport(TrafficIpType ipType,
                                                 bool enable,
                                                 std::shared_ptr<std::vector<std::uint32_t>> idList,
                                                 telux::common::ResponseCallback cb) override;
    telux::common::Status createTxSpsFlow(TrafficIpType ipType,
                                          uint32_t serviceId,
                                          const SpsFlowInfo& spsInfo,
                                          uint16_t spsSrcPort,
                                          bool eventSrcPortValid,
                                          uint16_t eventSrcPort,
                                          CreateTxSpsFlowCallback cb);

    telux::common::Status createTxEventFlow(TrafficIpType ipType,
                                            uint32_t serviceId,
                                            uint16_t eventSrcPort,
                                            CreateTxEventFlowCallback cb);

    telux::common::Status createTxEventFlow(TrafficIpType ipType,
                                            uint32_t serviceId,
                                            const EventFlowInfo& flowInfo,
                                            uint16_t eventSrcPort,
                                            CreateTxEventFlowCallback cb);

    telux::common::Status closeRxSubscription(std::shared_ptr<ICv2xRxSubscription> rxSub,
                                              CloseRxSubscriptionCallback cb);

    telux::common::Status closeTxFlow(std::shared_ptr<ICv2xTxFlow> txFlow,
                                      CloseTxFlowCallback cb);

    telux::common::Status changeSpsFlowInfo(std::shared_ptr<ICv2xTxFlow> txFlow,
                                            const SpsFlowInfo& spsInfo,
                                            cv2x::ChangeSpsFlowInfoCallback cb);

    telux::common::Status requestSpsFlowInfo(std::shared_ptr<ICv2xTxFlow> txFlow,
                                             RequestSpsFlowInfoCallback cb);

    telux::common::Status changeEventFlowInfo(std::shared_ptr<ICv2xTxFlow> txFlow,
                                              const EventFlowInfo &flowInfo,
                                              ChangeEventFlowInfoCallback cb);

    telux::common::Status requestCapabilities(RequestCapabilitiesCallback cb);

    telux::common::Status requestDataSessionSettings(RequestDataSessionSettingsCallback cb);

    telux::common::Status updateSrcL2Info(UpdateSrcL2InfoCallback cb);

    telux::common::Status updateTrustedUEList(const TrustedUEInfoList & info,
                                              UpdateTrustedUEListCallback cb);

    std::string showAllFlows(void);

    telux::common::Status setDestinationIPv6Addr(std::string iPv6Addr);

    std::string getDestinationIPv6Addr(void);

    std::string getIfaceNameIP(void);

    std::string getIfaceNameNonIP(void);

    ~DsiCv2xRadio();

    std::string getIfaceNameFromIpType(TrafficIpType ipType);

    static void initCv2xRadioCapabilities(Cv2xRadioCapabilities& capabilities);

    telux::common::Status createCv2xTcpSocket(const EventFlowInfo &eventInfo,
                                              const SocketInfo &sockInfo,
                                              CreateTcpSocketCallback cb);

    telux::common::Status closeCv2xTcpSocket(std::shared_ptr<ICv2xTxRxSocket> sock,
                                             CloseTcpSocketCallback cb);

    telux::common::Status registerTxStatusReportListener(uint16_t port,
        std::shared_ptr<ICv2xTxStatusReportListener> listener, ResponseCallback cb);

    telux::common::Status deregisterTxStatusReportListener(uint16_t port, ResponseCallback cb);

    telux::common::Status setGlobalIPInfo(const IPv6AddrType &ipv6Addr,
        common::ResponseCallback cb);

    telux::common::Status setGlobalIPUnicastRoutingInfo(
        const GlobalIPUnicastRoutingInfo &destL2Addr, common::ResponseCallback cb);

    static constexpr uint32_t RX_PORT_NUM = 9000u; // cv2x "wildcard" port number
    static constexpr uint32_t SPS_MAX_NUM_FLOWS = 2u; // Max number of SPS flows supported

    // Capabilities are hardcoded for now
    // Payload max size, plus IPV6 header is 1500 bytes
    static constexpr uint32_t LINK_IP_MTU_BYTES = 1452u;
    static constexpr uint32_t LINK_NON_IP_MTU_BYTES = 2000u;
    static constexpr RadioConcurrencyMode SUPPORTED_CONCURRENCY_MODE =
        RadioConcurrencyMode::WWAN_NONCONCURRENT;

    static constexpr uint16_t IP_TX_PAYLOAD_OFFSET_BYTES = 0u;
    static constexpr uint16_t IP_RX_PAYLOAD_OFFSET_BYTES = 0u;

    static constexpr uint8_t MAX_NUM_AUTO_RETRANSMISSIONS = 1u;
    static constexpr uint8_t LAYER_2_MAC_ADDRESS_SIZE = 3u;

    // For calculating max payload size from MTU
    static constexpr uint16_t IPV6_HDR_SIZE = 40u;
    static constexpr uint16_t TCP_HDR_SIZE = 8u;
    static constexpr uint8_t MAX_RETRIES = 10u;
    static constexpr uint16_t RETRY_SLEEP_TIME_MS = 100u;

    // Max number of Non-SPS flows supported
    static constexpr uint32_t NON_SPS_MAX_NUM_FLOWS = DSI_V2X_NON_SPS_MAX_FLOWS;
    /*max/min tx powre is from 3gpp ts 36.331 & 36.101, and keep same with MPSS implementation*/
    static constexpr int CV2X_MAX_TX_POWER = 33;
    static constexpr int CV2X_MIN_TX_POWER = -30;
    // minimum port number, avoid use well known port number under 1024
    static constexpr unsigned int MINIMUM_PORT_NUMBER = 1024;

private:

    void initSync();

    uint32_t getNextReqId();

    telux::common::Status waitForInitialization();

    void setInitializedStatus(telux::common::Status status);

    telux::common::Status initRxSock(TrafficIpType ipType, int & sock, uint16_t port,
                                     struct sockaddr_in6 & sockAddr);

    telux::common::Status createRxSubscriptionSync(
        TrafficIpType ipType, uint16_t port, CreateRxSubscriptionCallback cb,
        std::shared_ptr<std::vector<uint32_t>> idList = nullptr);

    telux::common::ErrorCode initTxEventFlow(TrafficIpType ipType, uint32_t serviceId,
                                          const EventFlowInfo & flowInfo,
                                          uint16_t eventSrcPort,
                                          std::shared_ptr<ICv2xTxFlow> & txEventFlow);

    telux::common::Status createTxEventFlowSync(TrafficIpType ipType, uint32_t serviceId,
                                                const EventFlowInfo & flowinfo,
                                                uint16_t eventSrcPort,
                                                CreateTxEventFlowCallback cb);

    telux::common::Status initTxUdpSock(TrafficIpType ipType, int & sock, uint16_t port,
                                        struct sockaddr_in6 & socAddr);

    telux::common::ErrorCode initTxSpsFlow(TrafficIpType ipType,
                       uint32_t serviceId,
                       const SpsFlowInfo & spsInfo,
                       uint16_t spsSrcPort,
                       bool eventSrcPortValid,
                       uint16_t eventSrcPort,
                       std::shared_ptr<ICv2xTxFlow> & txSpsFlow,
                       std::shared_ptr<ICv2xTxFlow> & txEventFlow,
                       telux::common::Status & spsStatus,
                       telux::common::Status & eventStatus);

    telux::common::Status createTxSpsFlowSync(TrafficIpType ipType,
                                              uint32_t serviceId,
                                              const SpsFlowInfo & spsInfo,
                                              uint16_t spsSrcPort,
                                              bool eventSrcPortValid,
                                              uint16_t eventSrcPort,
                                              CreateTxSpsFlowCallback cb);

    telux::common::ErrorCode unsubscribeRxSubscriptionSync(
        TrafficIpType ipType, std::shared_ptr<std::vector<uint32_t>> idList = nullptr);

    telux::common::Status closeRxSubscriptionSync(std::shared_ptr<ICv2xRxSubscription> rxSub,
                                                  CloseRxSubscriptionCallback cb);

    telux::common::ErrorCode closeTxEventFlowsSync(
        std::vector<std::shared_ptr<ICv2xTxFlow>> & txFlows);

    telux::common::ErrorCode closeTxEventFlowSync(std::shared_ptr<ICv2xTxFlow> txFlow,
                                               CloseTxFlowCallback cb);

    telux::common::ErrorCode deregisterSpsFlowSync(TrafficIpType ipType, uint8_t spsId);

    telux::common::ErrorCode closeTxSpsFlowSync(std::shared_ptr<ICv2xTxFlow> txFlow,
                                                CloseTxFlowCallback cb);

    telux::common::Status changeSpsFlowInfoSync(std::shared_ptr<ICv2xTxFlow> txFlow,
                                                const SpsFlowInfo & spsInfo,
                                                ChangeSpsFlowInfoCallback cb);

    telux::common::Status addSubscription(std::shared_ptr<ICv2xRxSubscription> sub);
    telux::common::Status removeSubscription(std::shared_ptr<ICv2xRxSubscription> sub);

    template<class T>
    telux::common::Status addFlow(std::shared_ptr<T> flow,
                                  std::map<uint32_t, std::shared_ptr<ICv2xTxFlow>> & vec);

    template<class T>
    telux::common::Status removeFlow(std::shared_ptr<T> flow,
                                     std::map<uint32_t, std::shared_ptr<ICv2xTxFlow>> & vec);

    telux::common::Status updateSrcL2InfoSync(UpdateSrcL2InfoCallback cb);

    telux::common::Status updateTrustedUEListSync(const TrustedUEInfoList & info,
                                                 UpdateTrustedUEListCallback cb);

    telux::common::Status requestSpsFlowInfoSync(std::shared_ptr<ICv2xTxFlow> txFlow,
                                                 RequestSpsFlowInfoCallback cb);

    telux::common::Status changeEventFlowInfoSync(std::shared_ptr<ICv2xTxFlow> txFlow,
                                                  const EventFlowInfo &flowInfo,
                                                  ChangeEventFlowInfoCallback cb);

    telux::common::ErrorCode registerEventFlowSync(std::shared_ptr<ICv2xTxFlow> txFlow,
                                                const EventFlowInfo &flowInfo);

    telux::common::Status requestCapabilitiesSync(RequestCapabilitiesCallback cb);

    telux::common::Status enableRxMetaDataReportSync(TrafficIpType type,
                                                     bool enable,
                                                     std::shared_ptr<std::vector<uint32_t>> idList,
                                                     telux::common::ResponseCallback cb);

    telux::common::Status requestDataSessionSettingsSync(RequestDataSessionSettingsCallback cb);

    void unsubscribeAllRxSubs();
    void cleanupAllEventFlows();
    void cleanupAllSpsFlows();

    int getSockAddr(TrafficIpType ipType,
                    uint16_t port,
                    struct sockaddr_in6 &sockAddr,
                    bool &isGlobalAddr);

    telux::common::Status initTcpSock(const SocketInfo &sockInfo,
                                      int &sock,
                                      struct sockaddr_in6 &sockAddr);

    void closeTcpSock(int sock);

    bool isTcpSocketPresent(uint32_t serviceId, bool isExclId, uint32_t exclId);

    telux::common::Status addTcpSocket(std::shared_ptr<ICv2xTxRxSocket> sock);

    telux::common::Status removeTcpSocket(std::shared_ptr<ICv2xTxRxSocket> sock);

    telux::common::Status createCv2xTcpSocketSync(const EventFlowInfo &eventInfo,
                                                  const SocketInfo &sockInfo,
                                                  CreateTcpSocketCallback cb);

    telux::common::Status closeCv2xTcpSocketSync(std::shared_ptr<ICv2xTxRxSocket> sock,
                                                 CloseTcpSocketCallback cb);

    telux::common::Status registerTxStatusReportSync(ResponseCallback cb);

    telux::common::Status deregisterTxStatusReportSync(ResponseCallback cb);

    void closeAllCv2xTcpSockets();

    // MTU and MaxPayloadSize helper functions
    enum class MTUResultType {
        SUCCESS,
        RETRY,
        FAIL
    };
    MTUResultType getMTU(std::string interface, uint32_t & mtu);

    MTUResultType getMaxPayloadSize(uint32_t mtu, uint32_t & maxPayloadSize);

    bool getInterfaceData(std::string interface, uint32_t & mtu, uint32_t & maxPayloadSize);

    telux::common::Status getCv2xIfaceNames();

    void onIfaceChange(NwIfaceInfo &cv2xIface);

    void deferResponseGlobalIP(std::string &addr);
    void handleSetGlobalIP(const IPv6AddrType ipv6Addr, common::ResponseCallback cb);

    uint32_t reqId_ = 0; // TODO: Consider putting this in manager class

    Cv2xRadioCapabilities capabilities_;

    std::mutex mutex_;
    std::condition_variable initializedCv_;
    telux::common::Status initializedStatus_ = telux::common::Status::NOTREADY;

    std::recursive_mutex rxSubscriptionsMutex_;
    std::map<uint32_t, std::shared_ptr<ICv2xRxSubscription>> rxSubscriptions_;

    std::recursive_mutex flowsMutex_;

    // TODO: Consider making the values in these flow maps a shared pointer
    // to the derived type rather than the interface. It may clean up
    // some of the code.
    std::map<uint32_t, std::shared_ptr<ICv2xTxFlow>> eventFlows_;
    std::map<uint32_t, std::shared_ptr<ICv2xTxFlow>> spsFlows_;

    std::shared_ptr<Cv2xIndsListener> indicationsListener_;

    std::shared_ptr<qmi::NasQmiClient> nasQmiClient_      = nullptr;
    std::shared_ptr<data::DsiClient> dsiClient_ = nullptr;
    std::shared_ptr<Cv2xSysCtrl> cv2xSysCtrl_ = nullptr;

    std::atomic<bool> isExiting_;
    telux::common::AsyncTaskQueue<void> taskQ_;
    std::string ifaceNameIp_;
    std::string ifaceNameNonIp_;
    static std::string DEFAULT_DEST_IP_ADDR; // "ff02::1";

    // map of socket ID to TCP sockets
    std::recursive_mutex tcpSockMutex_;
    std::map<uint32_t, std::shared_ptr<ICv2xTxRxSocket>> tcpSockets_;

    // registered nas qmi listener for Tx status report
    std::shared_ptr<TxStatusReportListener> txReportListener_ = nullptr;
    std::shared_ptr<Cv2xDataCallListener> dataCallListener_ = nullptr;
    common::ResponseCallback globalIPCb_ = nullptr;
    std::map<int, GlobalIPSession> requestedPrefix_;
    std::mutex globalIPMtx_;
};


} // namespace cv2x

} // namespace telux

#endif // #ifndef CV2XRADIOIMPL_HPP
