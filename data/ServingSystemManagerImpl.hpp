/*
 *  Copyright (c) 2020-2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       ServingSystemManagerImpl.hpp
 *
 * @brief Serving System Manager class provides APIs related to the serving system for data
 *        functionality. For example, ability to query or be notified about the state of the
 *        platforms data connectivity, etc.
 */

#ifndef SERVING_SYSTEM_MANAGER_IMPLE_HPP
#define SERVING_SYSTEM_MANAGER_IMPLE_HPP

#include <future>
#include <memory>
#include <mutex>

#include <telux/data/ServingSystemManager.hpp>
#include <telux/data/DataDefines.hpp>
#include <telux/common/CommonDefines.hpp>
#include "qmi/WdsQmiClient.hpp"
#include "qmi/QmiClientFactory.hpp"
#include "common/ListenerManager.hpp"


namespace telux {
namespace data {

class ServingSystemManagerImpl;

using qmiIndCbType = std::function<telux::common::Status(
               std::shared_ptr<ServingSystemManagerImpl> callback, void* userData)>;


enum class ServingSystemRequestType {
   INVALID = 0,
   REGISTER_DRB_CHANGE_IND,
   REQUEST_DRB_STATUS,
   REGISTER_SERVICE_STATUS_CHANGE_IND,
   REGISTER_ROAMING_IND,
   REQUEST_SERVICE_STATUS,
};

enum class ReqRespStatus {
   FAILED = 0,
   PASSED,
   PENDING,
};

struct ServingSystemUserData {
   ServingSystemRequestType reqType;
   ReqRespStatus reqStatus;
   int cmdCallbackId;
};


class ServingSystemManagerImpl : public IServingSystemManager,
                                 public telux::qmi::IQmiWdsListener,
                                 public telux::qmi::IQmiDsdListener,
                                 public telux::qmi::IQmiCommandResponseCallback,
                                 public telux::qmi::IQmiDsdCommandResponseCallback,
                                 public telux::qmi::IQmiDsdGetRoamingStatusCallback,
                                 public std::enable_shared_from_this<ServingSystemManagerImpl> {

public:
   ServingSystemManagerImpl(SlotId slotId);
   ~ServingSystemManagerImpl();

    /**
     * Checks the status of serving manager and returns the result.
     *
     * @returns SERVICE_AVAILABLE    -  If serving manager is ready for service.
     *          SERVICE_UNAVAILABLE  -  If serving manager is temporarily unavailable.
     *          SERVICE_FAILED       -  If serving manager encountered an irrecoverable failure.
     *
     * @note Eval: This is a new API and is being evaluated. It is subject to change and
     *             could break backwards compatibility.
     */
    telux::common::ServiceStatus getServiceStatus() override;

    /**
     * get the dedicated radio bearer (DRB) status
     *
     * @returns current DrbStatus @ref DrbStatus.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
   DrbStatus getDrbStatus() override;

    /**
     * Queries the current serving network status
     *
     * @param [in] callback          callback to get the response requestServiceStatus
     *
     * @returns Status of requestServiceStatus i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    telux::common::Status requestServiceStatus(RequestServiceStatusResponseCb callback) override;

    /**
     * Queries the current roaming status
     *
     * @param [in] callback          callback to get the response requestRoamingStatus
     *
     * @returns Status of requestRoamingStatus i.e. success or suitable status code.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    telux::common::Status requestRoamingStatus(RequestRoamingStatusResponseCb callback) override;

   /**
    * Get associated slot id for the Serving System Manager.
    *
    * @returns SlotId
    *
    *
    */
   SlotId getSlotId() override;

   /**
    * Register a listener for specific updates from serving system.
    *
    * @param [in] listener     Pointer of IServingSystemListener object that
    *                          processes the notification
    *
    * @returns Status of registerListener i.e success or suitable status code.
    */
   telux::common::Status registerListener(std::weak_ptr<IServingSystemListener> listener) override;

   /**
    * Deregister the previously added listener.
    *
    * @param [in] listener     Previously registered IServingSystemListener that
    *                          needs to be removed
    *
    * @returns Status of removeListener i.e. success or suitable status code
    */
   telux::common::Status deregisterListener(std::weak_ptr<IServingSystemListener> listener) override;

   void commandResponse(int qmiError, int dsExtendedError, void *data) override;
   void onWdsServiceStatusChange(telux::common::ServiceStatus status) override;
   void onDormancyStatusChangeInd(const wds_global_dormancy_status_ind_msg_v01& indData) override;
   void onServiceStateChangeInd(const dsd_system_status_v2_ind_msg_v01& ind) override;
   void onServiceStatusResultInd(const dsd_get_system_status_v2_result_ind_msg_v01& ind,
                                 int qmiErr) override;
   void onRoamingStatusChangeInd(const dsd_roaming_status_change_ind_msg_v01& ind) override;
   void roamingStatusResponse(int qmiError, int dsExtendedError,
                              const dsd_get_current_roaming_status_info_resp_msg_v01 &resp,
                              void *data) override;
   telux::common::Status init(telux::common::InitResponseCb callback);
   void cleanup();
private:
   void initSync();
   void setSubSystemStatus(telux::common::ServiceStatus status);
   void resetState();
   void processServiceStatusResults(const dsd_system_status_info_type_v01& qmiStatus,
                                    ServiceStatus& status);
   void processRoamingStatusResults(uint8_t qmiStatus,
                                    dsd_roaming_type_enum_type_v01 type, RoamingStatus& status);
   telux::common::Status registerDrbChangeIndication();
   telux::common::Status registerRoamingIndications();
   telux::common::Status registerServiceStatusChangeIndication();
   telux::common::Status registerQmiIndication(ServingSystemUserData** userData,
                                               qmiIndCbType regType,
                                               ServingSystemRequestType requestType);

   telux::common::AsyncTaskQueue<void> taskQ_;

   std::mutex mtx_;
   std::mutex qmiMtx_;
   std::condition_variable initCv_;
   bool isInit_;
   DataServiceState lastReportedServiceStatus_;
   DrbStatus drbStatus_;
   ServingSystemUserData *drbRegIndRespStatus_ = nullptr;
   ServingSystemUserData *serviceStatusRespStatus_ = nullptr;
   ServingSystemUserData *roamingRespStatus_ = nullptr;
   telux::common::InitResponseCb initCb_;
   SlotId slotId_ = DEFAULT_SLOT_ID;
   telux::common::ServiceStatus subSystemStatus_;
   std::shared_ptr<telux::qmi::WdsQmiClient> wdsQmiClient_ = nullptr;
   std::shared_ptr<telux::qmi::DsdQmiClient> dsdQmiClient_ = nullptr;
   std::shared_ptr<telux::common::ListenerManager<IServingSystemListener>> listenerMgr_;
   telux::common::CommandCallbackManager cmdCallbackMgr_;
   std::vector<RequestServiceStatusResponseCb> serviceStatusCbs_;
   std::vector<RequestRoamingStatusResponseCb> roamingStatusCbs_;
};

} // end of namespace data
} // end of namespace telux

#endif
