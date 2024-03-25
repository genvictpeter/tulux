/*
 *  Copyright (c) 2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef EVENT_HPP
#define EVENT_HPP

#include <string>

namespace telux {

namespace common {

class Event {
 public:
    Event(uint32_t id)
       : Event(id, "Event-" + std::to_string(id)) {
    }

    Event(uint32_t id, std::string name)
       : id_(id)
       , name_(name) {
    }

    virtual ~Event() {
    }

    const uint32_t id_;
    const std::string name_;
};

}  // namespace common
}  // namespace telux

#endif  // EVENT_HPP
