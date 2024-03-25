/*
 *  Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef CV2XTHROTTLEMANAGERIMPL_HPP
#define CV2XTHROTTLEMANAGERIMPL_HPP

#include <memory>
#include <mutex>
#include <condition_variable>
#include <telux/cv2x/Cv2xThrottleManager.hpp>
#include "throttle_client.h"
#include "common/AsyncTaskQueue.hpp"
#include "common/ListenerManager.hpp"

namespace telux {

namespace cv2x {

class Cv2xThrottleManager : public ICv2xThrottleManager {
public:
    Cv2xThrottleManager() {
        exiting_ = false;
        listenerMgr_ =
            std::make_shared<telux::common::ListenerManager<ICv2xThrottleManagerListener>>();
    };

    virtual telux::common::ServiceStatus getServiceStatus();

    virtual telux::common::Status registerListener(
            std::weak_ptr<ICv2xThrottleManagerListener> listener);

    virtual telux::common::Status deregisterListener(
            std::weak_ptr<ICv2xThrottleManagerListener> listener);

    virtual telux::common::Status setVerificationLoad(int load, setVerificationLoadCallback cb);

    telux::common::Status init(telux::common::InitResponseCb callback = nullptr);

    ~Cv2xThrottleManager();

private:

    void initSync();
    std::mutex mutex_;
    telux::common::AsyncTaskQueue<void> taskQ_;
    std::condition_variable cv_;
    telux::common::ServiceStatus status_ = telux::common::ServiceStatus::SERVICE_UNAVAILABLE;
    void onFilterRateUpdate(int rate);
    void onTmStatusUpdate(Tm_status status);
    std::shared_ptr<telux::common::ListenerManager<ICv2xThrottleManagerListener>> listenerMgr_;
    std::atomic<bool> exiting_;
    telux::common::InitResponseCb initCb_ = nullptr;
};

}  //namespace cv2x

}  //namespace telux

#endif // ifndef CV2XTHROTTLEMANAGERIMPL_HPP
