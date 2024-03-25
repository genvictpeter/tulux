/*
 *  Copyright (c) 2018, The Linux Foundation. All rights reserved.
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

#include "Card/CardServicesMenu.hpp"
#include "SapCard/SapCardServicesMenu.hpp"

#include "SimCardServicesMenu.hpp"

SimCardServicesMenu::SimCardServicesMenu(std::string appName, std::string cursor)
   : ConsoleApp(appName, cursor) {
}

SimCardServicesMenu::~SimCardServicesMenu() {
}

void SimCardServicesMenu::init() {
   std::shared_ptr<ConsoleAppCommand> cardServicesMenuCommand = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("1", "Card_Services", {}, std::bind(&SimCardServicesMenu::cardServicesMenu,
                                                            this, std::placeholders::_1)));
   std::shared_ptr<ConsoleAppCommand> sapCardMenuCommand = std::make_shared<ConsoleAppCommand>(
      ConsoleAppCommand("2", "Sap_Card_Services", {},
                        std::bind(&SimCardServicesMenu::sapCardMenu, this, std::placeholders::_1)));

   std::vector<std::shared_ptr<ConsoleAppCommand>> mainMenuCommands
      = {cardServicesMenuCommand, sapCardMenuCommand};

   addCommands(mainMenuCommands);
   ConsoleApp::displayMenu();
}

void SimCardServicesMenu::cardServicesMenu(std::vector<std::string> userInput) {
   CardServicesMenu cardServicesMenu("Card Services Menu", "card> ");
   cardServicesMenu.init();
   cardServicesMenu.mainLoop();  // Main loop to continuously read and execute commands
}

void SimCardServicesMenu::sapCardMenu(std::vector<std::string> userInput) {
   SapCardServicesMenu sapCardServicesMenu("SapCard Services Menu", "sap> ");
   sapCardServicesMenu.init();
   sapCardServicesMenu.mainLoop();  // Main loop to continuously read and execute commands
}
