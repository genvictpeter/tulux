/*
 *  Copyright (c) 2019-2021, The Linux Foundation. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *    * Neither the name of The Linux Foundation nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 *  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 *  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 *  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 *  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file       AudioFactory.hpp
 *
 * @brief      AudioFactory is the central factory to create all audio instances
 */

#ifndef AUDIOFACTORY_HPP
#define AUDIOFACTORY_HPP

#include <telux/audio/AudioManager.hpp>

namespace telux {

namespace audio {
/** @addtogroup telematics_audio_stream
 * @{ */

/**
 * @brief   AudioFactory allows creation of audio manager.
 */
class AudioFactory {
public:
   /**
    * Get Audio Factory instance.
    */
   static AudioFactory &getInstance();

   /**
    * Get instance of audio manager.
    *
    * @param[in] callback     Optional callback to get the response of AudioManager initialization.
    *
    * @returns IAudioManager pointer.
    */
   std::shared_ptr<IAudioManager> getAudioManager(telux::common::InitResponseCb callback = nullptr);

private:
   /*
    * The below callback is invoked after manager initialization.
    */
   void initCompleteNotifier( std::vector<telux::common::InitResponseCb>& initCbs,
      telux::common::ServiceStatus status);
   std::mutex audioFactoryMutex_;
   std::shared_ptr<IAudioManager> audioManager_;
   std::vector<telux::common::InitResponseCb> audioManagerCallbacks_;

   AudioFactory();
   AudioFactory(const AudioFactory &) = delete;
   AudioFactory &operator=(const AudioFactory &) = delete;
   ~AudioFactory();
};

/** @} */ /* end_addtogroup telematics_audio_stream */
}  // End of namespace audio

}  // End of namespace telux

#endif  // AUDIOFACTORY_HPP
