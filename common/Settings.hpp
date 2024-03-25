/*
 *  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @brief Settings class reads config file and caches the config settings
 * It provides utility functions to read the config values
 */

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <map>
#include <string>

namespace telux {

namespace common {

/*
 * Settings class caches the config settings from tel.conf file
 * It provides utility methods to get value of a config setting by passing the key
 */
class Settings {
public:
   // Get the user defined value for any configuration setting
   static std::string getValue(std::string key, bool format = false);

   // Function to read settings config file containing key value pairs
   static void readSettingsFile(std::string settingsFile);

   // Print all the key value pairs in the cache
   static void printSettings();

   // Get the configuration file
   static std::string getConfigFilePath();

private:
   // Hashmap to store all settings as key-value pairs
   static std::map<std::string, std::string> settingsMap_;

   // Config file to read
   static std::string configFile_;

   // Get current application path
   static std::string appPath_;

   // avoid creating instance for this object
   Settings() {
   }
};  // end of class Settings

}  // End of namespace common

}  // End of namespace telux

#endif  // SETTINGS_HPP
