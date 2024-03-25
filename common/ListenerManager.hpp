/*
 *  Copyright (c) 2019, 2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * ListenerManager class to register and deregister listeners of specific events
 */
#ifndef LISTENERMANAGER_HPP
#define LISTENERMANAGER_HPP
#include <iostream>
#include <memory>
#include <iomanip>
#include <mutex>
#include <vector>
#include <algorithm>
#include <bitset>
#include <set>
#include <map>

#include <telux/common/CommonDefines.hpp>
#include "common/Logger.hpp"

namespace telux {

namespace common {

template <typename T, typename U = std::bitset<32>>
class ListenerManager {
public:


ListenerManager() {
    LOG(DEBUG, __FUNCTION__);
}

~ListenerManager() {
    LOG(DEBUG, __FUNCTION__);
    registrationMap_.clear();
}

telux::common::Status registerListener(std::weak_ptr<T> listener) {
   auto sp = listener.lock();

   if(sp == nullptr) {
      LOG(ERROR, "Null listener");
      return telux::common::Status::INVALIDPARAM;
   }

   std::lock_guard<std::mutex> lock(listenerMutex_);
   // Check whether the listener existed ...
   auto itr = std::find_if(
      std::begin(listeners_), std::end(listeners_),
      [=](std::weak_ptr<T> listenerExisted) { return (listenerExisted.lock() == sp); });
   if(itr != std::end(listeners_)) {
      LOG(DEBUG, "registerListener() - listener already exists");
      return telux::common::Status::ALREADY;
   }

   LOG(DEBUG, "registerListener() - creates a new listener entry");
   listeners_.emplace_back(listener);  // store listener

   return telux::common::Status::SUCCESS;
}

telux::common::Status deRegisterListener(std::weak_ptr<T> listener) {
   bool listenerExisted = false;
   std::lock_guard<std::mutex> lock(listenerMutex_);
   for(auto it = listeners_.begin(); it != listeners_.end();) {
      auto sp = (*it).lock();
      if(!sp) {
         LOG(DEBUG, "Erasing obsolete weak pointer from Listener");
         it = listeners_.erase(it);
      } else if(sp == listener.lock()) {
         it = listeners_.erase(it);
         LOG(DEBUG, "removeListener success");
         listenerExisted = true;
      } else {
         ++it;
      }
   }
   if(listenerExisted) {
      return telux::common::Status::SUCCESS;
   } else {
      LOG(WARNING, "QmiClient removeListener: listener not found");
      return telux::common::Status::NOSUCH;
   }
}

void getAvailableListeners(
   std::vector<std::weak_ptr<T>> &availableListeners) {
   // Entering critical section, copy lockable shared_ptr from global listener
   std::lock_guard<std::mutex> lock(listenerMutex_);
   for(auto it = listeners_.begin(); it != listeners_.end();) {
      auto sp = (*it).lock();
      if(sp) {
         availableListeners.emplace_back(sp);
         ++it;
      } else {
         // if we unable to lock the listener, we should remove it from
         // listenerList
         LOG(DEBUG, "erased obsolete weak pointer from PhoneManager's listeners");
         it = listeners_.erase(it);
      }
   }
}

/** Map the listener for selected indications as present in the bitset.*/
telux::common::Status registerListener(std::weak_ptr<T> listener, U indications) {
    auto sp = listener.lock();
    if(sp == nullptr) {
        LOG(ERROR, "Null listener");
        return telux::common::Status::INVALIDPARAM;
    }
    std::lock_guard<std::mutex> lock(listenerMutex_);
    for(size_t itr = 0; itr < indications.size(); itr++) {
        if(indications.test(itr)) {
            registrationMap_[itr].insert(sp);
        }
    }
    return telux::common::Status::SUCCESS;
}

/** Erase the listener for selected indications as present in the bitset. */
telux::common::Status deRegisterListener(std::weak_ptr<T> listener, U indications) {
    auto sp = listener.lock();
    if(sp == nullptr) {
        LOG(ERROR, "Null listener");
        return telux::common::Status::INVALIDPARAM;
    }
    bool listenerExisted = false;
    std::lock_guard<std::mutex> lock(listenerMutex_);
    for(size_t itr = 0; itr < indications.size(); itr++) {
        if(indications.test(itr)) {
            if(registrationMap_.find(itr) != registrationMap_.end()) {
                if(registrationMap_[itr].erase(sp)) {
                    listenerExisted = true;
                }
            }
        }
    }
    if(listenerExisted) {
        return telux::common::Status::SUCCESS;
    } else {
        return telux::common::Status::NOSUCH;
    }
}

/** If the indication is present in the map, return the corresponding list of listeners. */
void getAvailableListeners(uint32_t indication, std::vector<std::weak_ptr<T>> &vec) {
    std::lock_guard<std::mutex> lock(listenerMutex_);
    if(registrationMap_.find(indication) != registrationMap_.end()) {
        vec.assign(registrationMap_[indication].begin(), registrationMap_[indication].end());
    }
}

private:

std::mutex listenerMutex_;
std::vector<std::weak_ptr<T>> listeners_;

/** std::weak_ptr doesn't support relational operators. Need to use a binary predicate. */
struct SetPredicate {
    bool operator() (const std::weak_ptr<T> &lhs, const std::weak_ptr<T> &rhs)const {
        auto lptr = lhs.lock();
        auto rptr = rhs.lock();
        if(!rptr) {
            //RHS is a nullptr. Eg: Any address (address < 0) is false.
            return false;
        }
        if(!lptr) {
            //LHS is a nullptr. Any address (0 < address) is true.
            return true;
        }
        //Both Lhs and Rhs are legal addresses. So we compare and return.
        return lptr < rptr;
    }
};

/** We maintain a mapping between an indication and all the corresponding listeners registered. */
std::map<uint32_t, std::set<std::weak_ptr<T>, SetPredicate> > registrationMap_;


};
}// common
} // telux
#endif
