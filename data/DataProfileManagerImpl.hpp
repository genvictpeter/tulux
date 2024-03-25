/*
 *  Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef DATAPROFILEMANAGERIMPL_HPP
#define DATAPROFILEMANAGERIMPL_HPP

#include <map>
#include <memory>
#include <mutex>
#include <list>

#include <telux/data/DataProfileManager.hpp>
#include <telux/data/DataDefines.hpp>

#include "qmi/WdsQmiClient.hpp"
#include "qmi/QmiClientFactory.hpp"
#include "ProfileRetriever.hpp"
#include "DataHelper.hpp"

#include "common/ResponseHandler.hpp"

namespace telux {
namespace data {

class WdsCmdListener;

enum class qmiReqStatus {
   FAILED = 0,
   PASSED,
   NOT_STARTED,
};

class DataProfileManagerImpl : public IDataProfileManager,
                               public IProfileRetrieverCallback,
                               public telux::qmi::IQmiWdsCreateProfileCallback,
                               public std::enable_shared_from_this<DataProfileManagerImpl> {

public:
   DataProfileManagerImpl(SlotId slotId);

   ~DataProfileManagerImpl();

   telux::common::Status init(telux::common::InitResponseCb callback);

   void cleanup();

   bool validateAPN(std::string input);

   telux::common::ServiceStatus getServiceStatus() override;

   bool isSubsystemReady() override;

   std::future<bool> onSubsystemReady() override;

   telux::common::Status requestProfileList(std::shared_ptr<IDataProfileListCallback> callback
                                            = nullptr);

   telux::common::Status createProfile(const ProfileParams &profileParams,
                                       std::shared_ptr<IDataCreateProfileCallback> callback
                                       = nullptr);

   telux::common::Status
      deleteProfile(uint8_t profileId, TechPreference techPref,
                    std::shared_ptr<telux::common::ICommandResponseCallback> callback = nullptr);

   telux::common::Status requestProfile(uint8_t profileId, TechPreference techPreference,
                                        std::shared_ptr<IDataProfileCallback> callback = nullptr);

   telux::common::Status
      modifyProfile(uint8_t profileId, const ProfileParams &profileParams,
                    std::shared_ptr<telux::common::ICommandResponseCallback> callback = nullptr);

   telux::common::Status queryProfile(const ProfileParams &profileParams,
                                      std::shared_ptr<IDataProfileListCallback> callback = nullptr);

   int getSlotId();

   telux::common::Status registerListener(std::weak_ptr<telux::data::IDataProfileListener> listener);

   telux::common::Status
      deregisterListener(std::weak_ptr<telux::data::IDataProfileListener> listener);

   void createProfileResponse(int qmiError, int profileIndex, int dsExtendedError,
                              void *data) override;

   void onProfileRetrieved(std::shared_ptr<ProfileRetriever> dataProfileRetriever) override;

private:
   SlotId slotId_ = DEFAULT_SLOT_ID;

   std::mutex profileMgrMutex_;
   std::mutex initMtx_;
   std::mutex qmiMtx_;
   std::condition_variable initCondVar_;
   std::condition_variable initCV_;
   bool isInitComplete_;
   telux::common::ServiceStatus subSystemStatus_;
   telux::common::InitResponseCb initCb_;
   std::shared_ptr<telux::qmi::WdsQmiClient> wdsQmiClient_ = nullptr;
   bool ready_ = false;
   qmiReqStatus registerFlag_;   // Flag to check status for register profile
                                 // change indication request.
   qmiReqStatus configureFlag_;  // Flag to check status for configure profiles request.

   ProfileUserData * registerUserData_ = nullptr;
   ProfileUserData * configureUserData_ = nullptr;
   std::shared_ptr<WdsCmdListener> wdsListener_ = nullptr;

   std::shared_ptr<telux::common::AsyncTaskQueue<void>> taskQ_;
   telux::common::CommandCallbackManager cmdCallbackMgr_;

   std::atomic<bool> exit_;

   void initSync();
   void resetState();
   void setSubSystemStatus(telux::common::ServiceStatus status);

   /**
    * Copy telux::data::ProfileParams to QMI profile params
    *
    * @param [in] profileParams
    * @param [out] qmiProfileParams
    */
   void copyDataProfileParamsToQmi(const ProfileParams &profileParams,
                                   telux::qmi::QmiProfileParams *qmiProfileParams);

   /**
    * Remove DataProfileRetriever object's shared pointer from the retrieverList_
    * if the client was notified
    */
   void removeProfileRetriever(std::shared_ptr<ProfileRetriever> dataProfileRetriever);
   std::list<std::shared_ptr<ProfileRetriever>> retrieverList_;  // store the ProfileRetriever
                                                                 // object's shared pointer till the
                                                                 // client was notified

   void handleCommandResp(telux::common::ErrorCode error, int callbackId);

   bool waitForInitialization();

   // copy all the lockable listeners from the listeners_ list
   void getAvailableListeners(
      std::vector<std::shared_ptr<telux::data::IDataProfileListener>> &listeners);
   std::vector<std::weak_ptr<telux::data::IDataProfileListener>> listeners_;

   void setSubsystemReady(bool status);

   void commandResponse(telux::common::ErrorCode errCode, void *data);

   void onProfileUpdate(wds_profile_changed_ind_msg_v01 *indData);

   void onWdsServiceStatusChange(telux::common::ServiceStatus status);

};  // end of DataProfileManagerImpl class
}
}

#endif  // DATAPROFILEMANAGERIMPL_HPP
