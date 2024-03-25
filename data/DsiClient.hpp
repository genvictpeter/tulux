/*
 *  Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       DsiClient.hpp
 *
 * @brief      DSI Client is a singleton class interacts with dsi_netctrl Client APIs to
 *             send data call related requests and receive the indications.
 *
 *             It does conversion of telux data structures into DSI data structures and vice
 *             versa
 *
 */

#ifndef DSICLIENT_HPP
#define DSICLIENT_HPP

extern "C" {
#include <data/dsi_netctrl.h>
}

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <list>
#include <string>
#include <condition_variable>

#include <telux/data/DataConnectionManager.hpp>
#include <telux/data/DataFilterListener.hpp>
#include <telux/data/DataFilterManager.hpp>
#include <telux/cv2x/Cv2xRadioTypes.hpp>
#include <telux/cv2x/Cv2xTxFlow.hpp>
#include "common/ListenerManager.hpp"
#include "common/CommonUtils.hpp"
#include "IpFilterImpl.hpp"
#include "DataCallImpl.hpp"

using telux::cv2x::SpsFlowInfo;
using std::shared_ptr;

namespace telux {
namespace data {

class EventHandler;
class DsiEventDispatcher;

struct DsiCallbackData {
    dsi_hndl_t hndl;
    void *userData;
    dsi_net_evt_t evt;
    dsi_evt_payload_t *payload;
};

struct DsiStatusCallbackData {
    dsi_init_mode_t mode;
    void *userData;
};

class DsiClient {
 public:
    friend class DsiEventDispatcher;

     // TODO: shared_ptrs should not be passed by reference
    static const shared_ptr<DsiClient> &getDsiClient();

    /**
     * Perform dsi initialization
     *
     * @param [in] afterSSR        indicates whether this method is invoked after SSR or not
     */

    telux::common::Status init();

    telux::common::Status cleanup();

    int getMuxidByIface(const std::string ifaceName);

    telux::common::Status registerListener(std::weak_ptr<IDataConnectionListener> listener);

    telux::common::Status deregisterListener(std::weak_ptr<IDataConnectionListener> listener);

#ifndef FEATURE_DATA_QCMAP

    telux::common::Status startDataCall(
        int profileId, int ipFamilyType, DataCallResponseCb callback = nullptr);

    telux::common::Status stopDataCall(
        int profileId, int ipFamilyType, DataCallResponseCb callback = nullptr);

    telux::common::Status requestDataCallStatistics(
        int profileId, int ipFamilyType, StatisticsResponseCb callback = nullptr);

    telux::common::Status resetDataCallStatistics(
        int profileId, int ipFamilyType, common::ResponseCallback callback = nullptr);

    void handleDsiNetCallback(const std::shared_ptr<DsiCallbackData> &cbData);

    // This method will be invoked for SSR - UNAVAILABILITY, if service is down NO_NET
    // event callback comes from dsi, incase SSR down occurs before NO_NET callback
    // we need to reset all the pending calls and notify the listener
    void resetAndNotifyDataCalls();

    dsi_hndl_t getDataSrvcHndl(dsi_net_ev_cb cb,
                               void * userData,
                               uint16_t dsiMode = DSI_MODE_GENERAL);

    // Post DsiNetCallback to dispatcher
    void postEvent(
        dsi_hndl_t hndl, void *userData, dsi_net_evt_t evt, dsi_evt_payload_t *payloadPtr);

#endif //ifndef FEATURE_DATA_QCMAP

    telux::common::Status setDataFilterMode(int profileId, int ipFamilyType, uint8_t powersaveMode,
        uint8_t autoexit, common::ResponseCallback callback = nullptr);

    telux::common::Status getDataFilterMode(std::string ifaceName, DataRestrictModeCb callback);

    telux::common::Status addDataFilter(int profileId, int ipFamilyType,
        shared_ptr<IIpFilter> &filter, common::ResponseCallback callback = nullptr);

    telux::common::Status removeAllDataFilter(int profileId, int ipFamilyType,
        common::ResponseCallback callback = nullptr);

    telux::common::Status registerDataFilterListener(std::weak_ptr<IDataFilterListener> listener);
    telux::common::Status deregisterDataFilterListener(std::weak_ptr<IDataFilterListener> listener);

    void handleDsiStatusCallback(const std::shared_ptr<DsiStatusCallbackData> &cbData);

    void postEvent(uint16_t mode, void* userData);

    /**********************************************************************************
     *                   Cv2x Control APIs
     *********************************************************************************/

    telux::common::ErrorCode enableRxMetaDataSync(uint32_t reqId, bool enable,
                                                  const std::string& ifaceName,
                                                  shared_ptr<std::vector<uint32_t>> idList);

    telux::common::ErrorCode registerSpsFlowSync(
        uint32_t reqId, uint32_t serviceId, const SpsFlowInfo& spsInfo, uint16_t spsSrcPort,
        bool eventSrcPortValid, uint16_t eventSrcPort, const std::string ifaceName, uint8_t& spsId);

    telux::common::ErrorCode deregisterSpsFlowSync(
        uint32_t reqId, const std::string ifaceName, uint32_t spsId);

    telux::common::ErrorCode registerNonSpsFlowSync(
        uint32_t reqId, const std::string ifaceName, uint32_t serviceId,
        const cv2x::EventFlowInfo& flowInfo, uint16_t eventSrcPort,
        dsi_protocol_type_t protocol = DSI_V2X_PROTO_UDP);

    telux::common::ErrorCode deregisterNonSpsFlowSync(
        uint32_t reqId, const std::string ifaceName,
        std::vector<shared_ptr<cv2x::ICv2xTxFlow>>& txFlows);

    telux::common::ErrorCode deregisterNonSpsFlowSync(
        uint32_t reqId, const std::string ifaceName,
        uint32_t serviceId, uint16_t eventSrcPort);

    telux::common::ErrorCode updateSpsFlowSync(
        uint32_t reqId, const std::string ifaceName,
        const shared_ptr<cv2x::ICv2xTxFlow> txFlow, const SpsFlowInfo& spsInfo);

    telux::common::Status requestSpsFlowInfoSync(
        uint32_t reqId, const std::string ifaceName, uint8_t spdId, SpsFlowInfo &spsInfo);

    telux::common::ErrorCode addServiceSubscriptionSync(
        uint32_t reqId, uint16_t port, const std::string ifaceName,
        shared_ptr<std::vector<uint32_t>> idList);

    telux::common::ErrorCode removeServiceSubscriptionSync(
        uint32_t reqId, const std::string ifaceName,
        shared_ptr<std::vector<uint32_t>> idList);

    telux::common::Status requestRuntimeSettings(const std::string &ifaceName,
        std::vector<cv2x::DataSessionSettings>& settings);

    /**
     * Static DSI Client Helper Functions
     */

    // TODO: reorganize to make static calls private
    // temporary global functions should be redesigned to use the same one as Cv2xRadioHelper
    static void initServiceSubReq(dsi_v2x_service_subscribe_req_info_t &req, uint32_t reqId,
                                  uint16_t port, shared_ptr<std::vector<uint32_t>> idList,
                                  dsi_v2x_service_sub_action_type_t action) {

        memset(&req, 0, sizeof(req));
        if (INVALID_PORT != port) {
            req.optional_params_mask |= DSI_V2X_OPT_PARAM_MASK_PORT;
            req.port = port;
        }

        req.req_id = reqId;

        if (DSI_V2X_SERVICE_SUBS_REMOVE_WILDCARD == action ||
            DSI_V2X_SERVICE_SUBS_ADD_WILDCARD == action) {
            req.service_id_list_len = 0;
        } else if (idList) {
            req.service_id_list_len =
                idList->size() < DSI_V2X_MAX_SUB ?
                idList->size() : DSI_V2X_MAX_SUB;
            std::copy(idList->begin(),
                      idList->begin()+ req.service_id_list_len,
                      req.service_id_list);
            req.optional_params_mask |= DSI_V2X_OPT_PARAM_MASK_SERVICE_LIST;
        }

        req.action = action;
    }

    static cv2x::Periodicity uint32ToPeriodicity(uint32_t p);

    static uint32_t periodicityToUint32(cv2x::Periodicity p);

    template <typename T>
    static void convertSpsInfoToReq(T& req, const SpsFlowInfo& spsInfo) {

        req.periodicity = spsInfo.periodicityMs;
        req.optional_params_mask |= DSI_V2X_OPT_PARAM_MASK_PERIODICITY;

        req.msg_size = spsInfo.nbytesReserved;
        req.optional_params_mask |= DSI_V2X_OPT_PARAM_MASK_MSG_SIZE;

        if (spsInfo.peakTxPowerValid) {
            req.peak_tx_power = spsInfo.peakTxPower;
            req.optional_params_mask |= DSI_V2X_OPT_PARAM_MASK_PEAK_TX_POWER;
        }

        if (spsInfo.autoRetransEnabledValid) {
            req.retx_setting = (spsInfo.autoRetransEnabled) ? DSI_V2X_RETX_ON: DSI_V2X_RETX_OFF;
            req.optional_params_mask |= DSI_V2X_OPT_PARAM_MASK_RETX_SETTING;
        }

        if (spsInfo.mcsIndexValid) {
            req.mcs_index = spsInfo.mcsIndex;
            req.optional_params_mask |= DSI_V2X_OPT_PARAM_MASK_MCS_INDEX;
        }

        if (spsInfo.txPoolIdValid) {
            req.tx_pool_id = spsInfo.txPoolId;
            req.optional_params_mask |= DSI_V2X_OPT_PARAM_MASK_TX_POOL_ID;
        }
    };

    static void initSpsFlowRegReqMsg(dsi_v2x_sps_flow_reg_req_info_t & req, uint32_t reqId,
                                     uint32_t serviceId, const SpsFlowInfo& spsInfo,
                                     uint16_t spsSrcPort, bool eventSrcPortValid,
                                     uint16_t eventSrcPort);

    static void initNonSpsFlowRegReqMsg(dsi_v2x_non_sps_flow_reg_req_info_t& req,
                                        uint32_t reqId, uint32_t serviceId,
                                        const cv2x::EventFlowInfo &flowInfo,
                                        uint16_t eventSrcPort,
                                        dsi_protocol_type_t protocol = DSI_V2X_PROTO_UDP);

    static void initNonSpsFlowDeregReqMsg(dsi_v2x_non_sps_flow_dereg_req_info_t & req,
                                          uint32_t reqId,
                                          const std::vector<shared_ptr<cv2x::ICv2xTxFlow>>& flows);

    static void initNonSpsFlowDeregReqMsg(dsi_v2x_non_sps_flow_dereg_req_info_t & req,
                                          uint32_t reqId, uint32_t serviceId,
                                          uint16_t eventSrcPort);

    static void initRxMetaDataReq(dsi_v2x_rx_metadata_setting_req_t& req,
                                  uint32_t reqId, bool enable,
                                  shared_ptr<std::vector<uint32_t>> idList);

    static void initSpsFlowUpdateReqMsg(dsi_v2x_sps_flow_update_req_info_t& req,
                                        uint32_t reqId, const shared_ptr<cv2x::ICv2xTxFlow> txFlow,
                                        const SpsFlowInfo& spsInfo);

    static void convertRespToSpsInfo(const dsi_v2x_sps_flow_get_info_resp_t &resp,
                                     SpsFlowInfo &spsInfo);

    // Prevent construction and copying of this class
    ~DsiClient();
    DsiClient(DsiClient const &) = delete;
    const DsiClient &operator=(DsiClient const &) = delete;

 private:

    enum class DsiMode {
        UNINITIALIZED,
        SSR,
        INITIALIZED
    };

    static constexpr uint16_t INVALID_PORT = 0;

    /*In standalone mode, DSI does NOT support low latency*/
    static const uint16_t DSI_MODE =
#ifdef FEATURE_EXTERNAL_AP
        DSI_MODE_LOW_LATENCY_EX | DSI_MODE_GENERAL_EX;
#else/*FEATURE_EXTERNAL_AP*/
        DSI_MODE_GENERAL_EX;
#endif/*FEATURE_EXTERNAL_AP*/

    const uint16_t srvcHandleMode_ = (DSI_MODE & DSI_MODE_LOW_LATENCY_EX) ?
        DSI_MODE_LOW_LATENCY_EX : DSI_MODE_GENERAL_EX;

    DsiMode dsiMode_;
    std::mutex dsiModeMtx_;
    std::condition_variable dsiModeCv_;
    std::mutex mtx_;
    bool isInitialized_ = false;
    bool exiting_ = false;
    shared_ptr<DsiEventDispatcher> dsiEventDispatcher_ = nullptr;
    std::shared_ptr<telux::common::ListenerManager<IDataConnectionListener>> listenerMgr_;
    std::vector<std::weak_ptr<IDataFilterListener>> dataFilterListeners_;
    std::vector<std::shared_ptr<DataCallImpl>> dataCalls_;

    DsiClient();
    std::shared_ptr<DataCallImpl> getDataCall(int profileId, int ipFamilyType);
    std::shared_ptr<DataCallImpl> getDataCall(std::string ifaceName);
    void releaseDsiHandle(dsi_hndl_t dsiHndl);

    telux::common::Status setDataCallParams(
        const dsi_hndl_t &dsiHndl, int profileId, int ipFamilyType);

    DataCallStatus dsiToDataCallStatus(dsi_net_evt_t evt);
    telux::common::Status updateIpAddrInfo(
        dsi_addr_info_t *addrInfo, int numAddrCount, shared_ptr<DataCallImpl> &dataCall);
    /**
     * formats the given sockaddr to a string and prepends a space delimiter (if necessary)
     *
     *  @param
     *  addr               [in]    - sock addr to convert to string
     *  prefixLen          [in]    - prefix length
     *  prefixLenValid     [in]    - if given prefix_len is valid or not
     *  addrStr            [out]   - storage for the formatted address string
     *  availLen           [inout] - size of available storage in addr_str before and
     *                               after storing the formatted address string
     *  preprendDelimiter  [in]    - if a delimiter should be perpended to the address
     *                               string
     */
    void formatSocketAddr(struct sockaddr_storage *addr, unsigned int prefixLen,
        bool prefixLenValid, char *addrStr, int addrStrSize, bool prependDelimiter);

    void getAvailableDataListeners(std::vector<shared_ptr<IDataFilterListener>> &listeners);
    void notifyDataFilterModeChanged(dsi_evt_payload_t *payload);
    void notifyDataCallInfoChanged(const shared_ptr<IDataCall> &dataCall);

    void updateDataCall(
        dsi_hndl_t hndl, dsi_net_evt_t evt, shared_ptr<DataCallImpl> &dataCall);

    /**
     * DSINetctrl lib init callback which indiate DSI running mode.
     *
     * @param [in]   mode         dsiNetctrl mode.
     * @param [in]   userData   parameter from dsi_init_ex2.
     * @returns None.
     *
     * @note    None.
     */
    static void dsiInitCallback(uint16_t mode, void* userData);

    /**
     * Retrieve DSI handle for use with Dsi calls.
     *
     * @param [in]   cb
     * @param [in]   userData
     *
     * @return dsi_hndl_t
     */
    dsi_hndl_t getDataSrvcHndl(dsi_net_ev_cb_ex cb,
                               void * userData,
                               uint16_t dsiMode = DSI_MODE_GENERAL_EX);

    dsi_hndl_t getDataSrvcHndl(dsi_net_ev_cb_ex cb, dsi_init_mode_t mode,
                               const std::string ifaceName, void* userData);

#ifndef FEATURE_DATA_QCMAP

    dsi_hndl_t getDsiHandle(int profileId, int ipFamilyType);
    void notifyDataListener(std::shared_ptr<DataCallImpl>& dataCall, dsi_net_evt_t evt);

    /**
     * update dataCallMap_ based on requested profile id
     *
     * @param [in] profileId
     * @param [in] ipFamilyType
     * @param [out] dataCall     update datacall impl class
     * @param [out] dsiHndl      update dsi handle for the given input
     */
    void createAndUpdateDataCall(
        int profileId, int ipFamilyType, std::shared_ptr<DataCallImpl> &dataCall);

#endif //ifndef FEATURE_DATA_QCMAP
};

/**
 * Helper class to handle DSI Net callback
 * in a separate thread
 */
/// TODO: rename to EventDispatcher and define in a common file.
class DsiEventDispatcher {
 public:
    DsiEventDispatcher();
    ~DsiEventDispatcher();

#ifndef FEATURE_DATA_QCMAP
    void postEvent(const shared_ptr<DsiCallbackData> &cbData);
#else
    void postEvent(const shared_ptr<DsiStatusCallbackData> &cbData);
#endif //ifndef FEATURE_DATA_QCMAP

 private:
    std::mutex mtx_;
    shared_ptr<EventHandler> handler_ = nullptr;

#ifndef FEATURE_DATA_QCMAP
    void onDsiNetDispatchCallback(const shared_ptr<DsiCallbackData> &cbData);
#endif //ifndef FEATURE_DATA_QCMAP
};

class EventInfoBase {
 public:
    virtual void process() = 0;
};

template <typename T>
class EventInfo : public EventInfoBase {
 public:
    EventInfo(const shared_ptr<T> &cbData,
        std::function<void(const shared_ptr<T> &)> cbFun)
       : cbData_(cbData)
       , func_(cbFun) {
    }
    const shared_ptr<T> cbData_;
    std::function<void(const shared_ptr<T>)> func_;

    void process() override;
};

/**
 * Handler class to run a specific event in a non-blocking thread, each handler instance
 * runs in a single thread
 */
class EventHandler {
 public:
    /**
     * Start the event handler thread
     */
    void start();

    /**
     * Stop the event handler thread
     */
    void stop();

    /**
     * Event processor wait for the incoming event and process the data
     */
    void eventProcessor();

    /**
     * Post the data and callback function
     */
    void post(shared_ptr<EventInfoBase> evt);

 private:
    // This thread processes data stored in queuedEvents_. It gets woken up when there is
    // data in that list.
    shared_ptr<std::thread> eventHandler_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool exit_ = false;
    // list of pending events data and function pointer
    std::list<shared_ptr<EventInfoBase>> queuedEvents_;
};

/**
 * DSI utility class
 */
class DsiHelper {
 public:
    static void logDsiNetEvent(dsi_net_evt_t netEvt);
    static void logDsiErrType(dsi_v2x_err_type_t errNo);
    static telux::common::ErrorCode dsiErrType2ErrorCode(dsi_v2x_err_type_t error);
    static std::string spsFlowRegReqMsgToString(
        const dsi_v2x_sps_flow_reg_req_info_t &req);

    static std::string spsFlowUpdateReqMsgToString(
        const dsi_v2x_sps_flow_update_req_info_t &req);

    static std::string nonSpsFlowRegReqMsgToString(
        const dsi_v2x_non_sps_flow_reg_req_info_t &req);

    static std::string serviceSubsReqMsgToString(
        const dsi_v2x_service_subscribe_req_info_t &req);
 private:
    // Map have the string representation of dsi_net_evt_t, used for logging
    static std::map<dsi_net_evt_t, std::string> dsiNetEventMap_;
    static std::map<dsi_v2x_err_type_t, std::string> dsiErrTypeMap_;
};
}
}

#endif
