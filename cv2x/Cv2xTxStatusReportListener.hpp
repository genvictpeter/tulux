/*
 *  Copyright (c) 2021, The Linux Foundation. All rights reserved.
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
* @file       Cv2xTxStatusReportListener.hpp
*
* @brief      Cv2xTxStatusReportListener is the listener interface for CV2X Tx status report
*/

#ifndef CV2XTXSTATUSREPORTLISTENER_HPP
#define CV2XTXSTATUSREPORTLISTENER_HPP

#include <telux/common/CommonDefines.hpp>
#include <telux/cv2x/Cv2xRadioTypes.hpp>

namespace telux {

namespace cv2x {

/** @addtogroup telematics_cv2x_cpp
 * @{ */

/**
 *@brief Listeners for CV2X Tx status report must implement this interface.
 */
class ICv2xTxStatusReportListener {
public:

    /**
     * Called when a CV2X transport block is transmitted in low layer if CV2X
     * Tx status report has been enabled by calling @ref setTxStatusReport.
     * @param [in] info - Tx status of the transport block.
     */
    virtual void onTxStatusReport(const TxStatusReport & info) {}

    /**
     * Destructor for ICv2xTxStatusReportListener
     */
    virtual ~ICv2xTxStatusReportListener(){}
};

/** @} */ /* end_addtogroup telematics_cv2x_cpp */

} // namespace cv2x

} // namespace telux

#endif // #ifndef CV2XTXSTATUSREPORTLISTENER_HPP
