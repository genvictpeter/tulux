/*
 *  Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * @brief Helper class performs common error checks and conversions
 */

#ifndef ERROR_HELPER_HPP
#define ERROR_HELPER_HPP

#include <map>
#include <memory>
#include <string>

extern "C" {
#include <qmi-framework/qmi_client.h>
#include <qmi-framework/common_v01.h>
}

#include "telux/common/CommonDefines.hpp"

#include "Logger.hpp"

namespace telux {

namespace common {

class ErrorHelper {
public:
   /**
    * Get error description for given ErrorCode
    */
   static std::string getErrorCodeAsString(ErrorCode error);

   /**
    * Get error description for given error
    */
   static std::string getErrorAsString(int error);

   /**
    * Convert QMI error codes to TelSDK error code
    */
   static ErrorCode qmiErrorToErrorCode(int error);
   /**
    * Convert QMI WDS extended Error to SDK ErrorCode
    */
   static ErrorCode wdsExtendedErrorToErrorCode(int error);

   /**
    * QMI error as string
    */
   static std::string getQmiErrorAsString(int errorCode);

   // Singleton implementation , disable copy constructor
   ErrorHelper(const ErrorHelper &) = delete;

   ErrorHelper &operator=(const ErrorHelper &) = delete;

   ~ErrorHelper();

private:
   ErrorHelper();
   /**
    * Initialize error code vs description map
    */
   static void initErrorCodeToStringMap();

   /**
    * Initialize QMI errors description
    */
   static void initQmiErrorToStringMap();

   /**
    * Initialize QMI client errors description
    */
   static void initQmiClientErrorToStringMap();

   /**
    * Initialize QMI error to error code map, this map contains QMI errors that has same name as
    * existing ril error.
    * e.g NO_MEMORY
    */
   static void initQmiErrorToErrorCodeMap();

   /**
    * Initialize QMI client error to error code map
    * e.g SERVICE_ERROR
    */
   static void initQmiClientErrorToErrorCodeMap();
   /**
    * Initialize all error code descriptions and maps
    */
   static void init();

   static std::map<ErrorCode, std::string> errorCodeToStringMap_;
   static std::map<int, std::string> qmiErrorToStringMap_;
   static std::map<int, std::string> qmiClientErrorToStringMap_;

   // This map holds a mapping of QMI errors which overlap with RIL errors
   static std::map<int, ErrorCode> qmiErrorOverlapMap_;
   // This map holds mapping of QMI client errors which indicates an issue
   // with the QMI framework
   static std::map<int, ErrorCode> qmiClientErrorMap_;
   // This flag holds the initialization status of the above maps
   static bool initialized_;
   // This mutex guarantee initialization happen once only
   static std::mutex mutex_;
};  // end of class Error

}  // End of namespace common

}  // End of namespace telux

#endif  // ERROR_HELPER_HPP
