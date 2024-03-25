/*
 *  Copyright (c) 2019,2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef DATAFILTERMANAGERIMPL_HPP
#define DATAFILTERMANAGERIMPL_HPP

#include <map>
#include <memory>
#include <mutex>
#include <list>

#include <telux/data/DataFilterManager.hpp>
#include <telux/data/DataDefines.hpp>
#include <telux/data/DataFactory.hpp>
#include <telux/data/DataConnectionManager.hpp>

#include "DsiClient.hpp"
#include "IpFilterImpl.hpp"

#include "common/ResponseHandler.hpp"
#include "common/CommandCallbackManager.hpp"
#include "common/AsyncTaskQueue.hpp"

namespace telux {
namespace data {

class DataFilterManagerImpl : public IDataFilterManager,
                              public IDataFilterListener,
                              public std::enable_shared_from_this<DataFilterManagerImpl> {

 public:
    DataFilterManagerImpl(int slotId);

    ~DataFilterManagerImpl();

    telux::common::Status init(telux::common::InitResponseCb callback);

    void cleanup();

    bool isReady() override;

    std::future<bool> onReady() override;

    telux::common::ServiceStatus getServiceStatus() override;

    telux::common::Status setDataRestrictMode(DataRestrictMode mode,
        telux::common::ResponseCallback callback = nullptr, int profileId = PROFILE_ID_MAX,
        IpFamilyType ipFamilyType = IpFamilyType::UNKNOWN) override;

    telux::common::Status requestDataRestrictMode(
        std::string ifaceName, DataRestrictModeCb callback) override;

    telux::common::Status addDataRestrictFilter(std::shared_ptr<IIpFilter> &filter,
        telux::common::ResponseCallback callback = nullptr, int profileId = PROFILE_ID_MAX,
        IpFamilyType ipFamilyType = IpFamilyType::UNKNOWN) override;

    telux::common::Status removeAllDataRestrictFilters(
        telux::common::ResponseCallback callback = nullptr, int profileId = PROFILE_ID_MAX,
        IpFamilyType ipFamilyType = IpFamilyType::UNKNOWN) override;

    int getSlotId() override;

    telux::common::Status registerListener(std::weak_ptr<IDataFilterListener> listener) override;

    telux::common::Status deregisterListener(std::weak_ptr<IDataFilterListener> listener) override;

    virtual void onDataRestrictModeChange(DataRestrictMode mode) override;
    virtual void onServiceStatusChange(telux::common::ServiceStatus status) override;

    void initSync();

 private:
    DataFilterManagerImpl(DataFilterManagerImpl const &) = delete;
    DataFilterManagerImpl &operator=(DataFilterManagerImpl const &) = delete;

    int slotId_ = DEFAULT_SLOT_ID;
    std::mutex mutex_;
    bool isInitComplete_;

    std::condition_variable initCV_;
    std::shared_ptr<DsiClient> dsiClient_ = nullptr;
    std::vector<std::weak_ptr<IDataFilterListener>> listeners_;
    telux::common::ServiceStatus subSystemStatus_;
    telux::common::InitResponseCb initCb_;
    telux::common::AsyncTaskQueue<void> taskQ_;

    bool waitForInitialization();
    void getAvailableListeners(std::vector<std::shared_ptr<IDataFilterListener>> &listeners);
    void setSubSystemStatus(telux::common::ServiceStatus status);
    void invokeInitCallback(telux::common::ServiceStatus status);

};  // end of DataFilterManagerImpl class
}  // namespace data
}  // namespace telux

#endif  // DATAFILTERMANAGERIMPL_HPP
