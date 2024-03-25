/*
 *  Copyright (c) 2017-2019, The Linux Foundation. All rights reserved.
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

#include <chrono>
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <mutex>

#include <telux/tel/PhoneListener.hpp>
#include <telux/tel/SmsManager.hpp>
#include <telux/tel/SubscriptionManager.hpp>
#include <telux/tel/PhoneFactory.hpp>
#include <telux/loc/LocationFactory.hpp>

#include "MyLocationListener.hpp"
#include "MySmsListener.hpp"

#define PRINT_NOTIFICATION std::cout << "\033[1;35mNOTIFICATION: \033[0m"

const std::string GREEN = "\033[0;32m";
const std::string RED = "\033[0;31m";
const std::string BOLD_RED = "\033[1;31m";
const std::string DONE = "\033[0m";  // No color

/**
 * Main routine
 */
int main(int, char **) {

   // [1] Get the PhoneFactory and PhoneManager instances
   auto &phoneFactory = telux::tel::PhoneFactory::getInstance();
   auto phoneManager = phoneFactory.getPhoneManager();

   // [2] Check if telephony subsystem is ready
   bool telSubSystemsStatus = phoneManager->isSubsystemReady();

   // [2.1] If telephony subsystem is not ready, wait for it to be ready
   if(!telSubSystemsStatus) {
      // std::cout << "Telephony subsystem is not ready" << std::endl;
      // std::cout << "wait unconditionally for it to be ready " << std::endl;
      std::future<bool> f = phoneManager->onSubsystemReady();
      // If we want to wait unconditionally for telephony subsystem to be ready
      telSubSystemsStatus = f.get();
   }

   // [2.2] Exit the application, if SDK is unable to initialize telephony subsystems
   if(!telSubSystemsStatus) {
      std::cout << " *** ERROR - Unable to initialize Location-Tracker App" << std::endl;
      exit(1);
   }

   // [3] Get subscriptionMgr and read Phone number
   std::shared_ptr<telux::tel::ISubscriptionManager> subscriptionMgr
      = phoneFactory.getSubscriptionManager();

   telux::common::Status status;
   if(!subscriptionMgr->isSubsystemReady()) {
      subscriptionMgr->onSubsystemReady().get();
   }

   auto subscription = subscriptionMgr->getSubscription(DEFAULT_SLOT_ID, &status);
   if(!subscription) {
      std::cout << " *** ERROR - Subscription is empty" << std::endl;
      exit(1);
   }

   std::cout << "\n\n";
   std::cout << "-------------------------------------------\n";
   std::cout << "            Location Tracker App \n";
   std::cout << "-------------------------------------------\n";

   // [4] Get the LocationFactory and LocationManager instances
   auto &locationFactory = telux::loc::LocationFactory::getInstance();

   std::shared_ptr<telux::loc::ILocationManager> locationMgr = locationFactory.getLocationManager();

   // [5] Check if location subsystem is ready
   std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
   startTime = std::chrono::system_clock::now();
   bool locSubSystemsStatus = locationMgr->isSubsystemReady();

   // [5.1] If location subsystem is not ready, wait for it to be ready
   if(!locSubSystemsStatus) {
      // std::cout << "GNSS Location subsystem is not ready, wait for it to be ready " << std::endl;
      std::future<bool> f = locationMgr->onSubsystemReady();
      locSubSystemsStatus = f.get();
   }

   // [5.2] Exit the application, if SDK is unable to initialize location subsystems
   if(locSubSystemsStatus) {
      endTime = std::chrono::system_clock::now();
      std::chrono::duration<double> elapsedTime = endTime - startTime;
      std::cout << std::endl << "Send SMS in the following format";
      std::cout << std::endl
                << "to the phone number " << subscription->getPhoneNumber()
                << " to get current location" << std::endl;
      std::cout << std::endl << "Format: " << GREEN << "Location <SMS Token>" << DONE << std::endl;
      std::cout << "Example: Location 1234" << std::endl;
   } else {
      std::cout << " *** ERROR - Unable to initialize GNSS Location subsystem" << std::endl;
      exit(1);
   }

   // [6] Get Default SMS manager instance
   std::shared_ptr<telux::tel::ISmsManager> smsManager = phoneFactory.getSmsManager();

   // [7] Instantiate LocationListener, SmsListener
   std::shared_ptr<MyLocationListener> myLocationListener = std::make_shared<MyLocationListener>();
   std::shared_ptr<MySmsListener> mySmsListener = std::make_shared<MySmsListener>();
   if(smsManager) {
      mySmsListener->setLocationListener(myLocationListener);
      smsManager->registerListener(mySmsListener);
   } else {
      std::cout << " *** ERROR - Unable to initialize GNSS Location subsystem" << std::endl;
      exit(1);
   }

   // [8] Instantiate global ILocationListener
   locationMgr->registerListenerEx(myLocationListener);
   // [9]Starting the reports for fixes
   locationMgr->startDetailedReports(1000,NULL);
   // [10] Exit logic is specific to an application
   std::cout << "Press enter to exit" << std::endl;
   std::string input;
   std::getline(std::cin, input);
   locationMgr->deRegisterListenerEx(myLocationListener);
   std::cout << "Exiting application..." << std::endl;
   return 0;
}
