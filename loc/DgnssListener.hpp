/*
 *  Copyright (c) 2020, The Linux Foundation. All rights reserved.
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
 * @file       DgnssListener.hpp
 *
 * @brief      Interface for dgnss injection service listener object. Client needs to
 *             implement this interface to get dgnss injection event notifications.
 *
 */

#ifndef DGNSSLISTENER_HPP
#define DGNSSLISTENER_HPP

#include "telux/loc/LocationDefines.hpp"

namespace telux {

namespace loc {

/** @addtogroup telematics_location
* @{ */

/**
 * @brief Listener class for getting RTCM injection event notification
 * information.
 *
 */
class IDgnssStatusListener {
public:
/**
 * This function is called asynchronously to report RTCM injection status
 *
 * @param [in] status - the status enum.
 *
 */
  virtual void onDgnssStatusUpdate(DgnssStatus status) {}

/**
 * Destructor of IRTCMStatusListener
 */
  virtual ~IDgnssStatusListener() {}
};
/** @} */ /* end_addtogroup telematics_location */
}         // end of namespace loc

} // end of namespace telux

#endif // DGNSSLISTENER_HPP
