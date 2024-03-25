/*
 *  Copyright (c) 2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @file       DataSubSystemStateMachine.hpp
 *
 * @brief      The sub-system readiness handling for the data sub-systems are implemented
 *             here. All the necessary indications and preparations related to sub-system
 *             readiness are handled here.
 */

#ifndef DATASUBSYSTEMSTATEMACHINE_HPP
#define DATASUBSYSTEMSTATEMACHINE_HPP

#include <set>

#include "common/BaseState.hpp"
#include "common/BaseStateMachine.hpp"
#include "common/Event.hpp"
#include <telux/common/CommonDefines.hpp>
#include <telux/data/DataDefines.hpp>

#define DATA_SUB_SYSTEM_MODEM_QCMAP_UP 0x02  // Sub-system state bit-mask for modem qcmap
#define DATA_SUB_SYSTEM_EAP_QCMAP_UP 0x04    // Sub-system state bit-mask for eap qcmap
#define DATA_SUB_SYSTEM_MODEM_UP 0x08        // Sub-system state bit-mask for modem (Q6)

namespace telux {
namespace data {

class QcmClient;
class DataSubSystemStateMachine;

/**
 * DataSubSystemEvent is the Event DataSubSystemStateMachine can recognize
 * and handle. This event can have IDs:
 *      MODEM_QCMAP_STATUS_INDICATION
 *      EAP_QCMAP_STATUS_INDICATION
 *      MODEM_STATUS_INDICATION
 * The state, apart from an ID and name has a boolean to indicate
 * the state of sub-system
 */
class DataSubSystemEvent : public telux::common::Event {
 public:
    const bool isAvailable_;  // Indicates the state of the sub-system

    DataSubSystemEvent(uint32_t id, std::string name, bool isAvailable)
       : Event(id, name)
       , isAvailable_(isAvailable) {
    }
};

/**
 * Concrete state (from BaseState) representing the system when the
 * data sub-system is not available.
 * All events which indicate the sub-systems becoming available are
 * prominent here. The state has the responsibility to
 *    enable mobileAP and
 *    register for indications
 * which it achieves via the QcmClient
 */
class DataSubSystemStateUnavailable : public telux::common::BaseState {
 public:
    /**
     * Constructor for DataSubSystemStateUnavailable
     * @param [in] name - The parent state-machine of type DataSubSystemStateMachine
     */
    DataSubSystemStateUnavailable(std::weak_ptr<BaseStateMachine> parent);

    /**
     * Event handler for DataSubSystemStateUnavailable
     * @param [in] event - The DataSubSystemEvent that needs to be handled
     * @returns true if the event was handled
     */
    bool onEvent(std::shared_ptr<telux::common::Event> event) override;

 private:
    /**
     * Internal helper to setup required operations to enableMobileAP or register with QCMAPs
     */
    void populateOperations(std::shared_ptr<DataSubSystemEvent> dataSubSystemEvent);

    /**
     * Internal wrapper to QcmClient to enable required MobileAPs
     */
    telux::common::Status enableMobileAP(
        std::shared_ptr<DataSubSystemStateMachine> dataSubSystemStateMachine);

    /**
     * Internal wrapper to QcmClient to register with required QCMAPs
     */
    telux::common::Status registerForIndications(
        std::shared_ptr<DataSubSystemStateMachine> dataSubSystemStateMachine);

    std::set<telux::data::OperationType> enableSet_;
    std::set<telux::data::OperationType> registerSet_;
};

/**
 * Concrete state (from BaseState) representing the system when the
 * data sub-system is available.
 * All events which indicate the sub-systems becoming unavailable are
 * prominent here. The state has the responsibility to
 *    notify inititalization
 *    notify sub-system state changes
 *    handle qcamp becoming unavailable and notifying no-net on appropriate data calls
 */
class DataSubSystemStateAvailable : public telux::common::BaseState {
 public:
    /**
     * Constructor for DataSubSystemStateAvailable
     * @param [in] name - The parent state-machine of type DataSubSystemStateMachine
     */
    DataSubSystemStateAvailable(std::weak_ptr<BaseStateMachine> parent);

    /**
     * Event handler for DataSubSystemStateAvailable
     * @param [in] event - The DataSubSystemEvent that needs to be handled
     * @returns true if the event was handled
     */
    bool onEvent(std::shared_ptr<telux::common::Event> event) override;

    /**
     * Method invoked by state-machine framework on entering DataSubSystemStateAvailable
     */
    void onEnter() override;

    /**
     * Method invoked by state-machine framework on exiting DataSubSystemStateAvailable
     */
    void onExit() override;
};
/**
 * Concrete state-machine (from BaseStateMachine) representing the
 * data-subsystem state-machine handling
 * Does basic (common) handling of the sub-system events (DataSubSystemEvent)
 * and passes on the event to the current state
 */
class DataSubSystemStateMachine : public telux::common::BaseStateMachine,
                                  public std::enable_shared_from_this<DataSubSystemStateMachine> {
 private:
    int currentSubSystemStatus_;  // The sub-system status
                                  // This is represented by bit-mask
                                  // 0th bit - unused
                                  // 1st bit - Modem QCMAP status
                                  // 2nd bit - EAP QCMAP status
                                  // 3rd bit - Modem status

    bool isSubSystemInitialized_;  // Indication to identify if this is the
                                   // first time we are entering AVAILABLE
                                   // so that we can indicate the QcmClient
                                   // appropriately. If this flag is set
                                   // we instead notify the sub-system service
                                   // status

    const std::weak_ptr<QcmClient> qcmClient_;  // The QcmClient service to achieve functionality
                                                // It would be weak since qcmClient owns us and
                                                // has a shared_ptr to us within itself

 public:
    /**
     * Constructor for DataSubSystemStateMachine
     * @param [in] name - The service, should be type QcmClient
     */
    DataSubSystemStateMachine(std::shared_ptr<QcmClient> qcmClient);

    /**
     * Overridden start method, would move the state machine to DataSubSystemStateUnavailable
     */
    void start() override;

    /**
     * Method to acquire the service in underlying states
     * @returns the pointer to the QcmClient service
     */
    std::shared_ptr<QcmClient> getQcmClient() const;

    /**
     * Top-level event handler for the state-machine
     * Handles the incoming events, identifies the event and the availability
     * of the sub-system and updates the currentSubSystemStatus_ accordingly
     * The event is then passed on further to the current state for further
     * handling
     * @param [in] event - The DataSubSystemEvent that needs to be handled
     * @returns true if the event was handled
     */
    bool onEvent(std::shared_ptr<telux::common::Event> event) override;

    /**
     * Helper method to identify if all the sub-systems are up and running
     * This method considers the possibility that could be running on VT or MDM
     * and evaluates accordingly
     * @returns true if the all the required sub-systems are up, else false
     */
    bool areAllSubSystemsUp() const;

    /**
     * Helper method to identify if all the QCMAPs are up and running
     * This method considers the possibility that could be running on VT or MDM
     * and evaluates accordingly
     * @returns true if the all the required QCMAPs are up, else false
     */
    bool areQcmapsUp() const;

    /**
     * Getter method for isSubSystemInitialized_
     * @returns the current value of isSubSystemInitialized_
     */
    bool isSubSystemInitialized() const;

    enum EventID {
        NONE = EVENT_ID_INVALID,
        MODEM_QCMAP_STATUS_INDICATION,  // ID for Modem QCMAP events
        EAP_QCMAP_STATUS_INDICATION,    // ID for EAP QCMAP events
        MODEM_STATUS_INDICATION         // ID for Modem events
    };

    /**
     * Utility method to create DataSubSystemEvent
     * @param [in] id - The event ID identifying the sub-system
     * @param [in] id - The state of the sub-system
     * @returns the DataSubSystemEvent created from id and state
     */
    std::shared_ptr<telux::common::Event> createDataSubSystemEvent(EventID id, bool state);

    enum StateID {
        STATE_NONE = STATE_ID_INVALID,
        STATE_DATA_SUBSYSTEM_UNAVAILABLE,  // ID for DataSubSystemStateUnavailable
        STATE_DATA_SUBSYSTEM_AVAILABLE     // ID for DataSubSystemStateAvailable
    };
};

}  // namespace data
}  // namespace telux
#endif  // DATASUBSYSTEMSTATEMACHINE_HPP
