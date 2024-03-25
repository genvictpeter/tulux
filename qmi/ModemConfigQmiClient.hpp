/*
 *  Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/**
 * @file       ModemConfigQmiClient.hpp

 * @brief      QMI Client interacts with QMI PDC service to send/receive QMI
 *             requests/responses and indications
 */

#ifndef MODEMCONFIGQMICLIENT_HPP
#define MODEMCONFIGQMICLIENT_HPP

extern "C" {
#include <qmi/persistent_device_configuration_v01.h>
}

#include "QmiClient.hpp"

struct ModemConfigUserData {
   int cmdCallbackId;
};

namespace telux {

namespace qmi {

//Forward Declarations
class IQmiResponseCallback;
class IQmiRegisterListenerCallback;
class IQmiModemConfigListener;

/**
 * This class provides mechanism to send requests and receive responses and indications from
 * PDC service of QMI.
 */

class ModemConfigQmiClient : public QmiClient {
public:
    ModemConfigQmiClient();

   /**
    * This function is called by the QmiClient::qmiIndicationCallback
    * infrastructure receives an indication for this client.
    *
    * NOTE: Callback happens in the QCCI thread context and raises signal.
    *
    * @param [in] userHandle           Opaque handle used by the infrastructure to
    *                                  identify different services.
    * @param [in] msgId                Message ID of the indication
    * @param [in] indBuf               Buffer holding the encoded indication
    * @param [in] indBufLen            Length of the encoded indication
    * @param [in] indCbData            Cookie user data value supplied by the client during
    *                                  registration
    */
    void indicationHandler(qmi_client_type userHandle, unsigned int msgId, void *indBuf,
                          unsigned int indBufLen, void *indCbData) override;

    /**
    * This function is called by the QmiClient::qmiAsyncResponseHandler
    * infrastructure receives an asynchronous response for this client.
    *
    * NOTE: Callback happens in the QCCI thread context and raises signal.
    *
    * @param [in] msgId                Message ID of the indication
    * @param [in] respCStruct          Buffer holding the decoded response
    * @param [in] respCStructLen       Length of the decoded response
    * @param [in] userData             Cookie user data value supplied by the client
    * @param [in] transpErr            Qmi Error
    * @param [in] callback             Command callback pointer
    */
    void asyncResponseHandler(unsigned int msgId, void *respCStruct, unsigned int respCStructLen,
                             void *userData, qmi_client_error_type transpErr,
                             std::shared_ptr<telux::common::ICommandCallback> callback) override;

    /**
     * This function is called by qmiClientErrorCallbackSync when the QMI service
     * terminates or deregisters.
     *
     * NOTE: Callback happens on new thread created by std::async.
     *
     * @param [in] clientError           Error value
     */
    void errorHandler(qmi_client_error_type clientError);

    /**
     * This function is called by qmiClientNotifierCallbackSync when a QMI service
     * event occurs indicating that the service count has changed.
     *
     * NOTE: Callback happens on new thread created by std::async.
     *
     * @param [in]   serviceEvent       Event type
     */
    void notifierHandler(qmi_client_notify_event_type serviceEvent);

    telux::common::Status sendGetConfigList(pdc_config_type_enum_v01 type,
            std::shared_ptr<qmi::IQmiResponseCallback> callback, void *userData = nullptr);

    telux::common::Status sendGetConfigInfo(pdc_config_info_req_type_v01 reqType,
            std::shared_ptr<qmi::IQmiResponseCallback> callback, void *userData = nullptr);

    telux::common::Status sendGetDefaultConfigInfo(pdc_config_type_enum_v01 configType,
            std::shared_ptr<qmi::IQmiResponseCallback> callback, void *userData = nullptr);

    telux::common::Status sendLoadConfig(pdc_load_config_info_type_v01 loadInfo,
            std::shared_ptr<qmi::IQmiResponseCallback> callback, void *userData = nullptr);

    telux::common::Status sendSetSelectConfig(pdc_config_info_req_type_v01 reqType, uint32_t slotId,
            std::shared_ptr<qmi::IQmiResponseCallback> callback, void *userData = nullptr);

    telux::common::Status sendGetSelectedConfig(pdc_config_type_enum_v01 configType, uint32_t
            slotId,  std::shared_ptr<qmi::IQmiResponseCallback> callback, void *userData = nullptr);

    telux::common::Status sendActivateConfig(pdc_config_type_enum_v01 configType,  uint32_t slotId,
            std::shared_ptr<qmi::IQmiResponseCallback> callback, void *userData = nullptr);

    telux::common::Status sendDeactivateConfig(pdc_config_type_enum_v01 configType, uint32_t slotId,
            std::shared_ptr<qmi::IQmiResponseCallback> callback, void *userData = nullptr);

    telux::common::Status sendDeleteConfig(pdc_config_info_req_type_v01 reqType, int idValid,
            std::shared_ptr<qmi::IQmiResponseCallback> callback, void *userData = nullptr);

    telux::common::Status sendGetFeature(uint32_t slotId,
            std::shared_ptr<qmi::IQmiResponseCallback> callback, void *userData = nullptr);

    telux::common::Status sendSetFeature(uint32_t slotId, pdc_selection_mode_enum_v01 selectionMode,
            std::shared_ptr<qmi::IQmiResponseCallback> callback, void *userData = nullptr);

    telux::common::Status sendRegisterListener(uint8_t indState,
            std::shared_ptr<qmi::IQmiRegisterListenerCallback> callback, void *userData = nullptr);

   ~ModemConfigQmiClient();

private:
    void handleGetConfigListResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr,std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleGetConfigInfoResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr,std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleGetDefaultConfigInfoResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr,std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleLoadConfigResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr,std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleSetSelectConfigResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr,std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleGetSelectConfigResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr,std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleActivateConfigResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr,std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleDeactivateConfigResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr,std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleDeleteConfigResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr,std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleGetFeatureResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr,std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleSetFeatureResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr,std::weak_ptr<telux::common::ICommandCallback> callback);

    void handleRegisterIndicationResp(void *respCStruct, uint32_t respCStructLen, void *data,
        qmi_client_error_type transpErr,std::weak_ptr<telux::common::ICommandCallback> callback);

    // All indication handlers
    int handleGetConfigListIndication(qmi_client_type userHandle,
            void *indBuf, unsigned int indBufLen);

    int handleGetConfigInfoIndication(qmi_client_type userHandle,
            void *indBuf, unsigned int indBufLen);

    int handleGetDefaultConfigInfoIndication(qmi_client_type userHandle,
            void *indBuf, unsigned int indBufLen);

    int handleLoadConfigIndication(qmi_client_type userHandle,
            void *indBuf, unsigned int indBufLen);

    int handleSetSelectConfigIndication(qmi_client_type userHandle,
            void *indBuf, unsigned int indBufLen);

    int handleGetSelectConfigIndication(qmi_client_type userHandle,
            void *indBuf, unsigned int indBufLen);

    int handleActivateConfigIndication(qmi_client_type userHandle,
            void *indBuf, unsigned int indBufLen);

    int handleDeactivateConfigIndication(qmi_client_type userHandle,
            void *indBuf, unsigned int indBufLen);

    int handleDeleteConfigIndication(qmi_client_type userHandle,
            void *indBuf, unsigned int indBufLen);

    int handleGetFeatureIndication(qmi_client_type userHandle,
            void *indBuf, unsigned int indBufLen);

    int handleSetFeatureIndication(qmi_client_type userHandle,
            void *indBuf, unsigned int indBufLen);

    int handleConfigRefreshIndication(qmi_client_type userHandle,
            void *indBuf, unsigned int indBufLen);

    void notifyGetConfigListInd(const pdc_list_configs_ind_msg_v01 & ind);

    void notifyGetConfigInfoInd(const pdc_get_config_info_ind_msg_v01 & ind);

    void notifyGetDefaultConfigInfoInd(const pdc_get_default_config_info_ind_msg_v01 &ind);

    void notifyLoadConfigInd(const pdc_load_config_ind_msg_v01 & ind);

    void notifySetSelectConfigInd(const pdc_set_selected_config_ind_msg_v01 & ind);

    void notifyGetSelectConfigInd(const pdc_get_selected_config_ind_msg_v01 & ind);

    void notifyActivateConfigInd(const pdc_activate_config_ind_msg_v01 & ind);

    void notifyDeactivateConfigInd(const pdc_deactivate_config_ind_msg_v01 & ind);

    void notifyDeleteConfigInd(const pdc_delete_config_ind_msg_v01 & ind);

    void notifyGetFeatureConfigInd(const pdc_get_feature_ind_msg_v01 & ind);

    void notifySetFeatureConfigInd(const pdc_set_feature_ind_msg_v01 & ind);

    void notifyConfigRefreshInd(const pdc_refresh_ind_msg_v01 & ind);

};

/**
 * @brief General command response callback for most of the requests, client needs to implement
 * this interface to get single shot response.
 *
 * The methods in callback can be invoked from multiple different threads. The implementation
 * should be thread safe.
 */
class IQmiResponseCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] qmiError         Qmi error
    * @param [out] userData         Cookie user data value supplied by the client
    */
   virtual void onCommandResponse(int qmiError, void *userData) = 0;

   virtual ~IQmiResponseCallback() {}
};

class IQmiRegisterListenerCallback : public telux::common::ICommandCallback {
public:
   /**
    * This function is called with the response to the command operation.
    *
    * @param [out] qmiError         Qmi error
    * @param [out] userData         Cookie user data value supplied by the client
    */
   virtual void onRegisterListenerResponse(int qmiError, void *userData) = 0;

   virtual ~IQmiRegisterListenerCallback() {}
};
/**
 * This is the listener class. Clients that want to be notified of ModemConfig indications must
 * implement this interface and register as a listener to a ModemConfigClient
 */

class IQmiModemConfigListener : public IQmiListener {
public:
    virtual void onGetConfigListInd(const pdc_list_configs_ind_msg_v01 & ind) {}

    virtual void onGetConfigInfoInd(const pdc_get_config_info_ind_msg_v01 & ind) {}

    virtual void onGetDefaultConfigInfoInd(const pdc_get_default_config_info_ind_msg_v01 &ind) {}

    virtual void onLoadConfiInd(const pdc_load_config_ind_msg_v01 &ind) {}

    virtual void onSetSelectConfigInd(const pdc_set_selected_config_ind_msg_v01 &ind) {}

    virtual void onGetSelectConfigInd(const pdc_get_selected_config_ind_msg_v01 &ind) {}

    virtual void onActivateConfigInd(const pdc_activate_config_ind_msg_v01 &ind) {}

    virtual void onDeactivateConfigInd(const pdc_deactivate_config_ind_msg_v01 &ind) {}

    virtual void onGetFeatureInd(const pdc_get_feature_ind_msg_v01 &ind) {}

    virtual void onSetFeatureInd(const pdc_set_feature_ind_msg_v01 &ind) {}

    virtual void onDeleteConfigInd(const pdc_delete_config_ind_msg_v01 &ind) {}

    virtual void onConfigRefreshInd(const pdc_refresh_ind_msg_v01 &ind) {}

    virtual void onModemConfigServiceStatusChange(telux::common::ServiceStatus status) {}

};

} //end of namespace config
} //end of namespace telux

#endif // MODEMCONFIGQMICLIENT_HPP