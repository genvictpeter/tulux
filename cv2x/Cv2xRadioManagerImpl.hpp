/*
 *  Copyright (c) 2018-2022 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef CV2XRADIOMANAGERIMPL_HPP
#define CV2XRADIOMANAGERIMPL_HPP


#include <vector>

#include <telux/cv2x/Cv2xRadioManager.hpp>
#include <telux/common/CommonDefines.hpp>

#include "common/AsyncTaskQueue.hpp"
#include "common/ListenerManager.hpp"
#include "Cv2xSysCtrl.hpp"
#include "Cv2xRadioHelper.hpp"

namespace telux {

namespace data {

class DsiClient;

} //namespace data

namespace qmi {

class DmsQmiClient;
class NasQmiClient;
class IQmiV2xStartCallback;
class IQmiV2xStopCallback;
class IQmiV2xStatusCallback;

} // namespace qmi

namespace cv2x {

class ServiceStatusListener;

class Cv2xRadioManager : public ICv2xRadioManager {
public:
    Cv2xRadioManager();

    virtual bool isReady();

    virtual std::future<bool> onReady();

    telux::common::ServiceStatus getServiceStatus() override;

    virtual std::shared_ptr<ICv2xRadio> getCv2xRadio(TrafficCategory category);

    virtual telux::common::Status startCv2x(StartCv2xCallback cb);

    virtual telux::common::Status stopCv2x(StopCv2xCallback cb);

    virtual telux::common::Status requestCv2xStatus(RequestCv2xStatusCallback cb);

    virtual telux::common::Status requestCv2xStatus(RequestCv2xStatusCallbackEx cb);

    virtual telux::common::Status updateConfiguration(const std::string & configFilePath,
                                                      UpdateConfigurationCallback cb);

    virtual telux::common::Status registerListener(std::weak_ptr<ICv2xListener> listener);

    virtual telux::common::Status deregisterListener(std::weak_ptr<ICv2xListener> listener);

    virtual telux::common::Status setPeakTxPower(int8_t txPower, common::ResponseCallback cb);
    virtual telux::common::Status setL2Filters(const std::vector<L2FilterInfo> &filterList,
        common::ResponseCallback cb);
    virtual telux::common::Status removeL2Filters(const std::vector<uint32_t> &l2IdList,
        common::ResponseCallback cb);

    void init(telux::common::InitResponseCb callback = nullptr);

    ~Cv2xRadioManager();

private:
    static constexpr auto CONFIG_MODEM_MODE = "CV2X_MODEM_MODE";

    // Define device operation mode
    enum class OperatingMode {
        OP_MODE_ONLINE,               /**< Online */
        OP_MODE_LOW_POWER,            /**< Low power */
        OP_MODE_FACTORY_TEST_MODE,    /**< Factory Test mode */
        OP_MODE_OFFLINE,              /**< Offline */
        OP_MODE_RESETTING,            /**< Resetting */
        OP_MODE_SHUTTING_DOWN,        /**< Shutting down */
        OP_MODE_PERSISTENT_LOW_POWER, /**< Persistent low power */
        OP_MODE_MODE_ONLY_LOW_POWER,  /**< Mode-only low power */
        OP_MODE_NET_TEST_GW,          /**< Conducting network test for GSM/WCDMA */
        OP_MODE_CAMP_ONLY,            /**< Camp only */
        OP_MODE_LOW_POWER_2,          /**< Low Power mode 2 - use for OEM customization,
                                           do not use for regular LPM operations. */
        OP_MODE_UNKNOWN,              /**< Device mode in switching state, not stable yet */
        OP_MODE_MALFUNCTION           /**< Nonoperational state */
    };

    enum class Cv2xStartStage {
        IDLE,            /* initial state when boot up, SSR, stopCv2x is called, or
                            start cv2x returned with failure */
        STARTING,        /* Enter this state when startCv2x has been called */
        STARTED          /* NAS v2x start request is succeeded, cv2x mode is started */
    };

    enum class OnlineConfiguration {
        ENFORCE,
        IGNORE
    };

    uint32_t getNextReqId();

    telux::common::Status sendConfigFileSync(const std::string & configFilePath,
                                             uint32_t bytesInFile,
                                             UpdateConfigurationCallback cb);
    void initSync();

    telux::common::Status queryOperatingMode(OperatingMode& mode) const;
    telux::common::Status setOperatingMode(OperatingMode mode);
    telux::common::Status requestModeChangeEventReport() const;
    telux::common::Status registerForNasIndications();
    telux::common::Status waitForInitialization();
    telux::common::Status waitOprtModeAvailable();
    telux::common::ErrorCode cv2xStartDecision(StartCv2xCallback cb);
    void onStartCv2xFail(StartCv2xCallback cb, telux::common::ErrorCode ec);

    void updateOperatingModeSync(OperatingMode mode);

    void setInitializedStatus(telux::common::Status status);
    void setServiceStatusCallbacks();

    bool initDmsClient();
    bool initNasClient();
    bool initDsiClient();
    bool initCv2xSysCtrlClient();
    telux::common::Status setL2FiltersSync(const std::vector<L2FilterInfo> &filterList,
        common::ResponseCallback cb);
    telux::common::Status removeL2FiltersSync(const std::vector<uint32_t> &l2IdList,
        common::ResponseCallback cb);

    std::shared_ptr<qmi::IQmiV2xStartCallback> v2xStartCallback_;
    std::shared_ptr<qmi::IQmiV2xStopCallback> v2xStopCallback_;
    std::shared_ptr<qmi::IQmiV2xStatusCallback> v2xStatusCallback_;
    std::shared_ptr<qmi::IQmiV2xStatusCallback> v2xStatusCallbackEx_;
    std::shared_ptr<qmi::IQmiSetPeakTxPowerCb> setPeakTxPowerCb_;

    telux::common::AsyncTaskQueue<void> taskQ_;
    std::shared_ptr<ICv2xRadio> radio_;
    std::shared_ptr<qmi::DmsQmiClient> dmsQmiClient_;
    std::shared_ptr<qmi::NasQmiClient> nasQmiClient_;
    std::shared_ptr<data::DsiClient> dsiClient_;
    std::shared_ptr<Cv2xSysCtrl> cv2xSysCtrlClient_;
    std::shared_ptr<ServiceStatusListener> serviceStatusListener_;

    OnlineConfiguration triggerModeOnline_ = OnlineConfiguration::IGNORE;
    std::atomic<OperatingMode> opMode_ = {OperatingMode::OP_MODE_UNKNOWN};
    std::mutex mutex_;
    uint32_t reqId_ = 0;
    std::condition_variable initializedCv_;
    std::condition_variable modeChangeCv_;
    telux::common::Status initializedStatus_ = telux::common::Status::NOTREADY;
    std::atomic<bool> exiting_;
    std::atomic<Cv2xStartStage> cv2xStartStage_;
    telux::common::ServiceStatus serviceStatus_ =
        telux::common::ServiceStatus::SERVICE_UNAVAILABLE;
    telux::common::InitResponseCb initCb_ = nullptr;
};


} // namespace cv2x

} // namespace telux

#endif // #ifndef CV2XRADIOMANAGERIMPL_HPP
