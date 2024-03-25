/*
 *  Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       Cv2xSysCtrl.hpp
 *
 * @brief      Cv2xSysCtrl Client is a singleton class  that wraps cv2x_sysctrl Client APIs to send
 *             cv2x requests that aren't data call specific
 *
 *             It does conversion of telux data structures into cv2x_sysctrl data structures and
 *             vice versa
 *
 */

#ifndef CV2XSYSCTRL_HPP
#define CV2XSYSCTRL_HPP

extern "C" {
#include <data/cv2x_sysctrl.h>
}

#include <functional>
#include <memory>
#include <mutex>
#include <list>
#include <string>
#include <condition_variable>

#include <telux/common/CommonDefines.hpp>
#include <telux/cv2x/Cv2xRadioListener.hpp>
#include <telux/cv2x/Cv2xConfig.hpp>
#include <telux/cv2x/Cv2xRadioTypes.hpp>
#include "common/CommonUtils.hpp"
#include "common/ListenerManager.hpp"

using telux::common::IServiceStatusListener;

namespace telux {
namespace cv2x {

class EventHandler;
class EventDispatcher;
class ICv2xSysCtrlListener;

struct IndicationData {
    cv2x_hndl_t hndl;
    cv2x_msg_type_t msgType;
    cv2x_msg_info_t payload;
    void* userData;
};

struct ServiceStatusData {
    cv2x_init_mode_t mode;
    void *userData;
};

class Cv2xSysCtrl: public common::ListenerManager<ICv2xSysCtrlListener> {
 public:
    friend class EventDispatcher;

    static const std::shared_ptr<Cv2xSysCtrl> getInstance();

    /**
     * Perform Cv2xSysCtrl initialization
     */
    telux::common::Status init();

    // Post Event to dispatcher
    void postEvent(uint16_t mode, void* userData);

    void postEvent(cv2x_hndl_t hndl, cv2x_msg_type_t msgType,
                   cv2x_msg_info_t* payloadPtr, void* userData);

    telux::common::ErrorCode sendConfigFileSync(const std::string & configFilePath);

    telux::common::ErrorCode retrieveConfigFileSync(uint8_t& txId);

    telux::common::ErrorCode requestCapabilitiesSync(Cv2xRadioCapabilities &cap);

    telux::common::ErrorCode updateTrustedUEListSync(const TrustedUEInfoList & info);

    telux::common::ErrorCode updateSrcL2InfoSync();

    telux::common::ErrorCode setL2FiltersSync(const std::vector<L2FilterInfo> &filterList);

    telux::common::ErrorCode removeL2FiltersSync(const std::vector<uint32_t> &l2IdList);

    telux::common::ErrorCode setGlobalIPInfoSync(const IPv6AddrType &addr);

    telux::common::ErrorCode setGlobalIPUnicastRoutingInfoSync(
        const GlobalIPUnicastRoutingInfo &destL2Addr);

    Cv2xSysCtrl();

    ~Cv2xSysCtrl();

    // Prevent construction and copying of this class
    Cv2xSysCtrl(Cv2xSysCtrl const &) = delete;

    const Cv2xSysCtrl &operator=(Cv2xSysCtrl const &) = delete;

 private:
    std::mutex mtx_;
    bool isInitialized_ = false;
    std::shared_ptr<EventDispatcher> eventDispatcher_ = nullptr;

    void releaseHandle(cv2x_hndl_t hndl);

    cv2x_hndl_t getHandle(cv2x_ind_cb cb, void* userData);

    /**
     * Cv2xSysCtrl Init callback for service status updates.
     */
    static void serviceStatusCallback(uint16_t mode, void* userData);

    /**
     * Functions to handle 5 different Cv2xSysCtrl indications
     */
    void handleServiceStatusCallback(const std::shared_ptr<ServiceStatusData> cbData);

    void handleCapabilityInfoInd (std::shared_ptr<IndicationData> data);

    void handleSendConfigInd (std::shared_ptr<IndicationData> data);

    void handleSpsScheduleInd (std::shared_ptr<IndicationData> data);

    void handleSrcL2InfoInd (std::shared_ptr<IndicationData> data);

    void handleConfigChangeInd(std::shared_ptr<IndicationData> data);

    void handleConfigFileRetrieveInd(std::shared_ptr<IndicationData> data);
    /**
     * Static Helper Functions
     */
    static void convertCapabilityInfoMsg(Cv2xRadioCapabilities & capabilities,
                                         const cv2x_capability_info_ind_msg_t& msg);

    static void initTunnelModeInfoReqMsg(cv2x_tunnel_mode_info_req_msg_t& req,
                                         const TrustedUEInfoList& info);

    std::string tunnelModeInfoReqMsgToString(const cv2x_tunnel_mode_info_req_msg_t &req);

    std::string spsSchedulingInfoIndToString(const cv2x_sps_scheduling_info_ind_msg_t &ind);

    enum class Cv2xSysMode {
        OUT_OF_SERVICE,
        IN_SERVICE
    };

    Cv2xSysMode mode_;
    std::mutex modeMtx_;
    std::condition_variable modeCv_;
    cv2x_hndl_t indicationHandle_;
    bool exiting_ = false;
};

/**
 * Helper class to handle callbacks in a separate thread
 */
class EventDispatcher {
 public:
    EventDispatcher();
    ~EventDispatcher();
    void postEvent(const std::shared_ptr<ServiceStatusData> data);
    void postEvent(const std::shared_ptr<IndicationData> data);

 private:
    std::mutex mtx_;
    std::shared_ptr<EventHandler> handler_ = nullptr;
};

class EventInfoBase {
 public:
    virtual void process() = 0;
};

template <typename T>
class EventInfo : public EventInfoBase {
 public:
    const std::shared_ptr<T> cbData_;
    std::function<void(const std::shared_ptr<T>)> func_;

    EventInfo(const std::shared_ptr<T> cbData,
        std::function<void(const std::shared_ptr<T>)> cbFun)
       : cbData_(cbData)
       , func_(cbFun) {
    }

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
    void post(std::shared_ptr<EventInfoBase> evt);

 private:
    // This thread processes data stored in queuedEvents_. It gets woken up when there is
    // data in that list.
    std::shared_ptr<std::thread> eventHandler_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool exit_ = false;
    // list of pending events data and function pointer
    std::list<std::shared_ptr<EventInfoBase>> queuedEvents_;
};

// Wrapper class for Listener callbacks.
class ICv2xSysCtrlListener: public ICv2xRadioListener,
    public IServiceStatusListener, public ICv2xConfigListener {
public:
    virtual void onV2xGetConfigurationInd(uint8_t tx_id,
                                          uint16_t total_seg,
                                          uint16_t seg_index,
                                          uint32_t content_len,
                                          uint8_t* content) {}

};

}
}
#endif
