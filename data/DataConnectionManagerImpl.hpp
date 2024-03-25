/*
 *  Copyright (c) 2018-2022 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       DataConnectionManagerImpl.hpp
 *
 * @brief      This is the implementation class for IDataConnectionManager
 *
 */

#ifndef DATACONNECTIONMANAGERIMPL_HPP
#define DATACONNECTIONMANAGERIMPL_HPP

#include <memory>
#include <mutex>
#include <string>

#include <telux/data/DataConnectionManager.hpp>

#include "common/AsyncTaskQueue.hpp"
#include "common/Logger.hpp"

#include "qmi/QmiClientFactory.hpp"
#include "qmi/DsdQmiClient.hpp"

#ifdef FEATURE_DATA_QCMAP
#include "QcmClient.hpp"
#else
#include "DsiClient.hpp"
#endif
#include "DataCallImpl.hpp"

namespace telux {
namespace data {

#ifdef FEATURE_DATA_QCMAP
class DataCallsCountListener;
#endif

class DataConnectionManagerImpl : public IDataConnectionManager,
                                  public IDataConnectionListener,
                                  public telux::qmi::IQmiWdsListener,
                                  public telux::qmi::IQmiDsdListener,
                                  public std::enable_shared_from_this<DataConnectionManagerImpl> {
 public:
    DataConnectionManagerImpl(SlotId slotId);
#ifdef FEATURE_DATA_QCMAP
    DataConnectionManagerImpl(SlotId slotId, std::shared_ptr<QcmClient> qcmapClient);
#endif
    ~DataConnectionManagerImpl();

    telux::common::ServiceStatus getServiceStatus() override;
    bool isSubsystemReady();
    std::future<bool> onSubsystemReady();

    telux::common::Status setDefaultProfile(OperationType oprType, uint8_t profileId,
        telux::common::ResponseCallback callback = nullptr);
    telux::common::Status getDefaultProfile(
        OperationType oprType, DefaultProfileIdResponseCb callback);
    telux::common::Status startDataCall(int profileId,
        IpFamilyType ipFamilyType = IpFamilyType::IPV4V6, DataCallResponseCb callback = nullptr,
        OperationType type = OperationType::DATA_LOCAL, std::string apn = "");
    telux::common::Status stopDataCall(int profileId,
        IpFamilyType ipFamilyType = IpFamilyType::IPV4V6, DataCallResponseCb callback = nullptr,
        OperationType type = OperationType::DATA_LOCAL, std::string apn = "");
    telux::common::Status registerListener(std::weak_ptr<IDataConnectionListener> listener);
    telux::common::Status deregisterListener(std::weak_ptr<IDataConnectionListener> listener);
    int getSlotId();
    telux::common::Status requestDataCallList(OperationType type, DataCallListResponseCb callback);
    telux::common::Status init(telux::common::InitResponseCb callback);
    telux::common::Status cleanup();

    void onDataCallInfoChanged(const std::shared_ptr<IDataCall> &dataCall) override;
    void onServiceStatusChange(common::ServiceStatus status) override;
#ifdef FEATURE_DATA_QCMAP
    void onHwAccelerationChanged(ServiceState state) override;
#else
    void onWdsServiceStatusChange(telux::common::ServiceStatus status) override;
#endif
    void onTrafficFlowTemplateChange(const std::shared_ptr<IDataCall> &dataCall,
        const std::vector<std::shared_ptr<TftChangeInfo>> &tft) override;
    void onWwanConnectivityConfigChange(SlotId slotId, bool isConnectivityAllowed) override;
 private:
    std::mutex mtx_;
    std::condition_variable cv_;
    SlotId slotId_ = DEFAULT_SLOT_ID;
    bool ready_ = false;
    bool isInitComplete_;
    telux::common::ServiceStatus subSystemStatus_;
    telux::common::InitResponseCb initCb_;

#ifdef FEATURE_DATA_QCMAP
    std::shared_ptr<QcmClient> qcmClient_ = nullptr;
    std::shared_ptr<DataCallsCountListener> callsCountListener_ = nullptr;
    std::atomic<bool> haveNonV2xCalls_ = {false};
#else
    std::shared_ptr<DsiClient> dsiClient_ = nullptr;
#endif

    std::vector<std::weak_ptr<IDataConnectionListener>> listeners_;
    std::shared_ptr<telux::qmi::WdsQmiClient> wdsQmiClient_ = nullptr;
    telux::common::AsyncTaskQueue<void> taskQ_;
    std::shared_ptr<telux::qmi::DsdQmiClient> dsdQmiClient_ = nullptr;

    /**
     * Perform synchronous initialization
     *
     * @param [in] fromSsr        indicates whether this method is invoked after SSR or not
     */
    void initSync(bool fromSsr = false);
    void setSubsystemReady(bool status);
    bool waitForInitialization();
    void setSubSystemStatus(telux::common::ServiceStatus status);
    void getAvailableListeners(std::vector<std::shared_ptr<IDataConnectionListener>> &listeners);
#ifdef FEATURE_DATA_QCMAP
    void invokeInitCallback(telux::common::ServiceStatus status);
    void onDsdServiceStatusChange(telux::common::ServiceStatus status) override;
    void onServiceStateChangeInd (const dsd_system_status_v2_ind_msg_v01& ind) override;
    DataBearerTechnology translateQmiToBearerTech(const dsd_system_status_info_type_v01 &dsdInfo);
    void onServiceStatusResultInd(
      const dsd_get_system_status_v2_result_ind_msg_v01& ind, int qmiErr) override;
    bool initDsdQmiClient();
    bool enableDSDIndication(bool enable);
    void onRatChanged(const dsd_system_status_info_type_v01 &dsdInfo);
    void onHaveActiveCalls(bool haveNonV2xCall);
#endif
};
}
}

#endif
