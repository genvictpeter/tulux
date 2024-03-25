/*
 *  Copyright (c) 2017-2021, Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       QmiClientFactory.hpp
 * @brief      QmiClientFactory allows creation of respective QMI clients such as Location,
 *             Device management service, User identity module service, Voice service etc.
 *
 * @note       Eval: This is a new API and is being evaluated.It is subject to
 *             change and could break backwards compatibility.
 */

#ifndef QMICLIENTFACTORY_HPP
#define QMICLIENTFACTORY_HPP

#include <telux/common/CommonDefines.hpp>

#include "NasQmiClient.hpp"
#include "DmsQmiClient.hpp"
#include "UimQmiClient.hpp"
#include "UimRemoteQmiClient.hpp"
#include "VoiceQmiClient.hpp"
#include "WdsQmiClient.hpp"
#include "AudioQmiClient.hpp"
#include "ThermalQmiClient.hpp"
#include "TmdQmiClient.hpp"
#include "ModemConfigQmiClient.hpp"
#include "WmsQmiClient.hpp"
#include "ImsSettingsQmiClient.hpp"
#include "UimHttpQmiClient.hpp"
#include "DsdQmiClient.hpp"
#include "ImsaQmiClient.hpp"
#ifdef PWR_MGR_QMI_SUPPORTED
#include "PowerQmiClient.hpp"
#endif


namespace telux {
namespace qmi {

class PowerQmiClient;

/**
 * @brief   QmiClientFactory allows creation of QMI clients.
 *
 * @note    Eval: This is a new API and is being evaluated. It is subject to change
 *          and could break backwards compatibility.
 */
class QmiClientFactory {
public:
   /**
    * Get QMI Client Factory instance.
    */
   static QmiClientFactory &getInstance();

   /**
    * Get instance of DMS QMI Client.
    *
    * @returns DmsQmiClient pointer.
    *
    * @note    Eval: This is a new API and is being evaluated. It is subject to change
    *          and could break backwards compatibility.
    */
   std::shared_ptr<DmsQmiClient> getDmsQmiClient(int slotId = DEFAULT_SLOT_ID);

   /**
    * Get instance of NAS QMI Client.
    *
    * @returns NasQmiClient pointer.
    *
    * @note    Eval: This is a new API and is being evaluated. It is subject to change
    *          and could break backwards compatibility.
    */
   std::shared_ptr<NasQmiClient> getNasQmiClient(int slotId = DEFAULT_SLOT_ID);

   /**
    * Get instance of UIM QMI Client.
    *
    * @returns UimQmiClient pointer.
    *
    * @note    Eval: This is a new API and is being evaluated. It is subject to change
    *          and could break backwards compatibility.
    */
   std::shared_ptr<UimQmiClient> getUimQmiClient();

   /**
    * Get instance of UIM Remote QMI Client.
    *
    * @returns UimRemoteQmiClient pointer.
    *
    * @note    Eval: This is a new API and is being evaluated. It is subject to change
    *          and could break backwards compatibility.
    */
   std::shared_ptr<UimRemoteQmiClient> getUimRemoteQmiClient();

   /**
    * Get instance of Voice QMI Client.
    *
    * @returns VoiceQmiClient pointer.
    *
    * @note    Eval: This is a new API and is being evaluated. It is subject to change
    *          and could break backwards compatibility.
    */
   std::shared_ptr<VoiceQmiClient> getVoiceQmiClient(int slotId = DEFAULT_SLOT_ID);

   /**
    * Get instance of Wds QMI Client.
    *
    * @returns WdsQmiClient pointer.
    *
    * @note    Eval: This is a new API and is being evaluated. It is subject to change
    *          and could break backwards compatibility.
    */
   std::shared_ptr<WdsQmiClient> getWdsQmiClient(SlotId slotId = DEFAULT_SLOT_ID);

   /**
    * Get instance of Wds QMI Client.
    *
    * @returns WdsQmiClient pointer.
    *
    * @note    Eval: This is a new API and is being evaluated. It is subject to change
    *          and could break backwards compatibility.
    */
   std::shared_ptr<WdsQmiClient> getWdsQmiClient(std::string ifaceName, int muxId);

   /**
    * Get instance of Audio QMI Client.
    *
    * @returns AudioQmiClient pointer.
    *
    * @note    Eval: This is a new API and is being evaluated. It is subject to change
    *          and could break backwards compatibility.
    */
   std::shared_ptr<AudioQmiClient> getAudioQmiClient();

    /**
     * Get instance of Power QMI Client.
     *
     * @returns PowerQmiClient pointer.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    std::shared_ptr<PowerQmiClient> getPowerQmiClient(common::ProcType procType);

    /**
     * Get instance of Thermal QMI Client.
     *
     * @returns TsensQmiClient pointer.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    std::shared_ptr<TsensQmiClient> getTsensQmiClient();

    /**
     * Get instance of TMD QMI Client.
     *
     * @returns TmdQmiClient pointer.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    std::shared_ptr<TmdQmiClient> getTmdQmiClient();

    /**
     * Get instance of ModemConfig QMI Client.
     *
     * @returns ModemConfigQmiClient pointer.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to change
     *          and could break backwards compatibility.
     */
    std::shared_ptr<ModemConfigQmiClient> getModemConfigQmiClient();

    /**
     * Get instance of WMS QMI Client for given slot.
     *
     * @returns WmsQmiClient shared pointer.
     */
    std::shared_ptr<WmsQmiClient> getWmsQmiClient(SlotId slotId = DEFAULT_SLOT_ID);

    /**
     * Get instance of IMS SETTINGS QMI Client for given slot.
     *
     * @returns ImsSettingsQmiClient pointer.
     *
     */
    std::shared_ptr<ImsSettingsQmiClient> getImsSettingsQmiClient(
       SlotId slotId = SlotId::DEFAULT_SLOT_ID);

   /**
    * Get instance of UIM HTTP client
    *
    * @returns UimHttpQmiClient shared pointer.
    *
    */
   std::shared_ptr<UimHttpQmiClient> getUimHttpQmiClient();

   /**
    * Get instance of DSD QMI Client.
    *
    * @returns DsdQmiClient pointer.
    *
    * @note    Eval: This is a new API and is being evaluated. It is subject to change
    *          and could break backwards compatibility.
    */
   std::shared_ptr<DsdQmiClient> getDsdQmiClient(SlotId slotId = DEFAULT_SLOT_ID);

    /**
     * Get instance of IMS APPLICATION QMI Client for given slot.
     *
     * @returns ImsaQmiClient pointer.
     *
     */
   std::shared_ptr<ImsaQmiClient> getImsApplicationQmiClient(
       SlotId slotId = SlotId::DEFAULT_SLOT_ID);

   ~QmiClientFactory();

private:
   std::shared_ptr<DmsQmiClient> dmsQmiClient_ = nullptr;
   std::shared_ptr<UimQmiClient> uimQmiClient_ = nullptr;
   std::shared_ptr<UimRemoteQmiClient> uimRemoteQmiClient_ = nullptr;
   std::map<int, std::shared_ptr<WdsQmiClient>> wdsQmiClientMap_;
   std::map<qmi_service_instance, std::shared_ptr<PowerQmiClient>> powerQmiClientMap_;
   std::shared_ptr<TsensQmiClient> thermalQmiClient_ = nullptr;
   std::shared_ptr<TmdQmiClient> tmdQmiClient_ = nullptr;
   std::shared_ptr<ModemConfigQmiClient> modemConfigQmiClient_ = nullptr;
   std::shared_ptr<UimHttpQmiClient> uimHttpQmiClient_ = nullptr;
   std::map<int, std::shared_ptr<VoiceQmiClient>> voiceQmiClientMap_;
   std::map<int, std::shared_ptr<NasQmiClient>> nasQmiClientMap_;
   std::map<int, std::shared_ptr<DmsQmiClient>> dmsQmiClientMap_;
   std::map<SlotId, std::shared_ptr<WmsQmiClient>> wmsQmiClientMap_;
   std::map<SlotId, std::shared_ptr<ImsSettingsQmiClient>> imssQmiClientMap_;
   std::map<SlotId, std::shared_ptr<DsdQmiClient>> dsdQmiClientMap_;
   std::map<SlotId, std::shared_ptr<ImsaQmiClient>> imsaQmiClientMap_;
   std::mutex qmiClientFactoryMutex_;
   QmiClientFactory();
   QmiClientFactory(const QmiClientFactory &) = delete;
   QmiClientFactory &operator=(const QmiClientFactory &) = delete;
};

}  // end of namespace qmi
}  // end of namespace telux

#endif  // QMICLIENTFACTORY_HPP
