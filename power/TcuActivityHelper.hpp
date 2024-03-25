/*
 *  Copyright (c) 2019 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TCUACTIVITYHELPER_HPP
#define TCUACTIVITYHELPER_HPP

extern "C" {
#include "power_manager_service_v01.h"
#include <qmi/device_management_service_v01.h>
}

#include "telux/power/TcuActivityDefines.hpp"

namespace telux {
namespace power {

class TcuActivityHelper {
public:
    /**
     * Function to extract TCU-activity state from QMI indication message(notification)
     *
     * @param [in] tcuStateInfoInd - pointer to QMI indication message
     *
     * @returns TcuActivityState
     */
    static TcuActivityState getTcuStateFromQmiInd(void *tcuStateInfoInd);

    /**
     * Function to extract TCU-activity state from QMI response message to events registration
     * request
     *
     * @param [in] tcuStateRegResp - pointer to registration QMI response message
     *
     * @returns TcuActivityState
     */
    static TcuActivityState getTcuStateFromQmiRegResp(void *tcuStateRegResp);

    /**
     * Function to convert TCU-activity state from QMI enum format to Telematics-SDK enum format
     *
     * @param [in] qmiTcuState - TCU-activity state in QMI enum format
     *
     * @returns TcuActivityState
     */
    static TcuActivityState convertTcuStateFromQmi(pwr_state_v01 qmiTcuState);

    /**
     * Function to convert TCU-activity state from Telematics-SDK enum format to QMI enum format
     *
     * @param [in] tcuState - TCU-activity state in Telematics-SDK enum format
     *
     * @returns pwr_state_v01
     */
    static pwr_state_v01 convertTcuStateToQmi(TcuActivityState tcuState);

    /**
     * Function to convert TCU-activity state from Telematics-SDK enum format to QMI enum format
     *
     * @param [in] tcuState - TCU-activity state in Telematics-SDK enum format
     *
     * @returns dms_activity_state_type_v01
     */
    static dms_activity_state_type_v01 convertTcuStateToDmsQmi(TcuActivityState tcuState);

    /**
     * Function to convert TCU-activity state acknowledgemnt from Telematics-SDK enum format to
     * QMI enum format
     *
     * @param [in]  tcuStateAck - TCU-activity state acknowledgement in Telematics-SDK enum format
     *        [in]  qmiAck - pointer to store the converted acknowledgement in QMI enum format
     *
     * @returns Status of conversion
     */
    static telux::common::Status convertTcuStateAckToQmi(TcuActivityStateAck tcuStateAck,
                                    pwr_state_ack_v01* qmiAck);

    /**
     * Function to get appropriate TCU-activity state acknowledgement(in Telematics-SDK enum format)
     * for a TCU-activity state
     *
     * @param [in]  tcuState - TCU-activity state in Telematics-SDK enum format
     *        [in]  tcuStateAck - pointer to store acknowledgement in Telematics-SDK enum format
     *
     * @returns Status of conversion
     */
    static telux::common::Status getAckForTcuState(TcuActivityState tcuState,
                                    TcuActivityStateAck *tcuStateAck);
};

}  // end of namespace power
}  // end of namespace telux

#endif  // end of TCUACTIVITYHELPER_HPP
