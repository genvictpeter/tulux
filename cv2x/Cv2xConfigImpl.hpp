/*
 *  Copyright (c) 2020 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef CV2XCONFIGIMPL_HPP
#define CV2XCONFIGIMPL_HPP

#include <telux/cv2x/Cv2xConfig.hpp>
#include "common/AsyncTaskQueue.hpp"
#include "Cv2xSysCtrl.hpp"

namespace telux {

namespace cv2x {

class Cv2xConfigListener;

class Cv2xConfig : public ICv2xConfig {
public:
    Cv2xConfig();
    ~Cv2xConfig();

    virtual bool isReady();
    virtual std::future<bool> onReady();
    telux::common::ServiceStatus getServiceStatus() override;

    telux::common::Status updateConfiguration(
        const std::string& configFilePath, telux::common::ResponseCallback cb) override;
    telux::common::Status retrieveConfiguration(
        const std::string& configFilePath, telux::common::ResponseCallback cb) override;
    telux::common::Status registerListener(std::weak_ptr<ICv2xConfigListener> listener);
    telux::common::Status deregisterListener(std::weak_ptr<ICv2xConfigListener> listener);

    void init(telux::common::InitResponseCb callback = nullptr);

private:
    static constexpr uint32_t SEND_CONFIG_BUFSIZE = 1500u;

    telux::common::Status sendConfigFileSync(const std::string& configFilePath,
                                             telux::common::ResponseCallback cb);
    telux::common::Status retrieveConfigFileSync(const std::string& configFilePath,
                                                 telux::common::ResponseCallback cb);
    void initSync();
    telux::common::Status waitForInitialization();
    void setInitializedStatus(telux::common::Status status);
    telux::common::Status initSysCtrlClientSync();

    uint32_t reqId_ = 0;
    telux::common::AsyncTaskQueue<void> taskQ_;
    telux::common::Status initializedStatus_{telux::common::Status::NOTREADY};
    std::atomic<bool> exiting_{false};
    std::mutex mutex_;
    std::condition_variable initializedCv_;
    telux::common::ServiceStatus serviceStatus_ =
        telux::common::ServiceStatus::SERVICE_UNAVAILABLE;
    telux::common::InitResponseCb initCb_{nullptr};
    std::shared_ptr<Cv2xConfigListener> configListener_{nullptr};
    std::shared_ptr<Cv2xSysCtrl> sysCtrlClient_{nullptr};
};

} // namespace cv2x
} // namespace telux

#endif // #ifndef CV2XCONFIGIMPL_HPP
