# Create Firewall DMZ

Please follow below steps to create firewall DMZ

### 1. Get the DataFactory and FirewallManager instances

   ~~~~~~{.cpp}
   auto &dataFactory = telux::data::DataFactory::getInstance();
   auto dataFwMgr  = dataFactory.getFirewallManager(opType);
   ~~~~~~

### 2. Check if data subsystem is ready

   ~~~~~~{.cpp}
   bool subSystemsStatus = dataFwMgr->isSubsystemReady();
   ~~~~~~

### 2.1 If data subsystem is not ready, wait for it to be ready

Data subsystems is to make sure that device is ready for services like create and remove DMZ.
if subsystems were not ready, wait for unconditionally.

   ~~~~~~{.cpp}
   if(!subSystemsStatus) {
      std::future<bool> f = dataFwMgr->onSubsystemReady();
      subSystemsStatus = f.get();
   }
   ~~~~~~

### 3. Implement callback for create DMZ ###

   ~~~~~~{.cpp}
   auto respCb = [](telux::common::ErrorCode error) {
      std::cout << std::endl << std::endl;
      std::cout << "CALLBACK: "
                  << "addDmz Response"
                  << (error == telux::common::ErrorCode::SUCCESS ? " is successful" : " failed");
   };
   ~~~~~~

### 4. Create DMZ based on profile id and local ip address ###

   ~~~~~~{.cpp}
   dataFwMgr->enableDmz(profileId,ipAddr, respCb);
   ~~~~~~

### 5. Response callback will be called for the addDmz response ###