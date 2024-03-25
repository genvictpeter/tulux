/*
 *  Copyright (c) 2018 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef PROFILERETRIEVER_HPP
#define PROFILERETRIEVER_HPP

#include <map>
#include <memory>
#include <mutex>

#include <telux/data/DataProfileManager.hpp>
#include <telux/data/DataDefines.hpp>

#include "telux/common/CommonDefines.hpp"
#include "qmi/WdsQmiClient.hpp"
#include "qmi/QmiClientFactory.hpp"
#include "common/ResponseHandler.hpp"

extern "C" {
#include <qmi/wireless_data_service_v01.h>
}

namespace telux {
namespace data {

class IProfileRetrieverCallback;

enum class ProfileRequestType {
   GET_PROFILE_LIST = 0,
   QUERY_PROFILE,
   GET_PROFILE_BY_ID,
   MODIFY_PROFILE,
   REGISTER_PROFILE_CHANGE_IND,
   CONFIGURE_PROFILE_LIST,
   DECONFIGURE_PROFILE_LIST,
};

struct ProfileUserData {
   int cmdCallbackId;
   ProfileParams profileParams;
   ProfileRequestType profileReqType;
   bool isValid3GPPProfileFound;
};

class ProfileRetriever : public telux::qmi::IQmiWdsGetProfileSettingsCallback,
                         public telux::qmi::IQmiWdsGetProfileListCallback,
                         public std::enable_shared_from_this<ProfileRetriever> {
public:
   ProfileRetriever(std::shared_ptr<IProfileRetrieverCallback> dataProfileManager,
                    std::shared_ptr<telux::qmi::WdsQmiClient> wdsQmiClient);
   ~ProfileRetriever();
   /**
    * Sends QMI requests to fetch all the existing data profiles.
   * The process of getting DataProfileList involves 2 QMI calls
    * 1. Get the profile list for the tech type (3GPP and 3GPP2)
    * 2. The response of profile list request will contain profile IDs and profile Length
    * 3. after receiving profile length, send bunch of profile settings request for the respective
    * profile IDs
    * 4. The response of profile settings request, create IDataProfile instance and store it in the
    * list
    * 5. Notify the caller after receiving all the profile settings response
    */
   telux::common::Status requestProfileList(TechPreference techPref,
                                            std::shared_ptr<IDataProfileListCallback> callback);

   telux::common::Status queryProfile(const ProfileParams &profileParams,
                                      std::shared_ptr<IDataProfileListCallback> callback);
   /**
    * Sends QMI request to fetch a data profile.
    */
   telux::common::Status requestProfile(uint8_t profileId, TechPreference techPref,
                                        std::shared_ptr<IDataProfileCallback> callback);
   void profileSettingsResponse(int qmiError, int dsExtendedError,
                                const qmi::ProfileSettingsResp &profileSettingsResp,
                                void *userData) override;
   void profileListResponse(int qmiError, int dsExtendedError,
                            const std::vector<qmi::ProfileListInfo> &profileList,
                            void *userData) override;

   /**
    * Convert the TechPreference to wds profile type enum value.
    */
   wds_profile_type_enum_v01 techPrefToWdsProfType(TechPreference techPref);

private:
   std::shared_ptr<IProfileRetrieverCallback> callback_ = nullptr;
   std::shared_ptr<telux::qmi::WdsQmiClient> wdsQmiClient_ = nullptr;
   std::mutex mutex_;
   // store number of profile settings for 3GPP and 3GPP2 to be retrieved per request.
   int totalProfileListLen_;
   // vector to store dataProfiles per 3GPP and 3GPP2 corresponding to request.
   // profiles settings data corresponding to
   // 3GPP and
   // 3GPP2 per request
   std::vector<std::shared_ptr<DataProfile>> dataProfilesForRequest_;
   telux::common::CommandCallbackManager cmdCallbackMgr_;

   std::shared_ptr<DataProfile>
      makeDataProfileFromQmi(const telux::qmi::ProfileSettingsResp &profileSettingsResp);
   void responseForQueryProfile(telux::common::ErrorCode error,
                                const telux::qmi::ProfileSettingsResp &profileSettingsResp,
                                ProfileUserData *data);
   void responseForProfileList(telux::common::ErrorCode error,
                               const telux::qmi::ProfileSettingsResp &profileSettingsResp,
                               ProfileUserData *data);
   void responseForProfileById(telux::common::ErrorCode error,
                               const telux::qmi::ProfileSettingsResp &profileSettingsResp,
                               ProfileUserData *data);

   /**
    * Initializes ProfileUserData before making QMI request
    */
   ProfileUserData *mallocAndInitUserData(
      ProfileRequestType profileReqType, std::shared_ptr<telux::common::ICommandCallback> callback);
   /**
    * Copy profile params given by the client
    *
    * @param [in] profileParams
    * @param [out] params
    */
   void copyProfileParams(const ProfileParams &profileParams, ProfileParams *params);
   /**
    * Notifies DataProfileManager after invoking the client callback
    */
   void notifyDataProfileManager();

   /**
    * Copy the profile which matched with user query.
    */
   void filterProfiles(ProfileUserData *userData,
                       std::vector<std::shared_ptr<DataProfile>> &profiles);

   /**
    * Send empty list along with error code.
    */
   void sendEmptyProfileList(int cmdID, telux::common::ErrorCode error);

   /**
    * Cleanup profile list and free the user data.
    */
   void cleanProfileList(ProfileUserData *userData);

   IpFamilyType wdsPdpTypeToIpFamilyType(int wdsPdpType);
};  // end of DataProfileRetriever

class IProfileRetrieverCallback : public telux::common::ICommandCallback {

public:
   /**
    * This function is called to notify DataProfileManager after invoking the client callback.
    * This will be useful to clear the DataProfileRetriever object reference from the
    * DataProfileManager.
    */
   virtual void onProfileRetrieved(std::shared_ptr<ProfileRetriever> dataProfileRetriever) = 0;
};
}
}
#endif  // PROFILERETRIEVER_HPP
