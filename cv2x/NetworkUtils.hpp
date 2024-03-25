/*
 *  Copyright (c) 2018 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef NETWORKUTILS_HPP
#define NETWORKUTILS_HPP


#include <netinet/in.h>
#include <sys/types.h>

#include <string>


namespace telux {

namespace utils {

/**
 * This function takes an interface name and returns zero if the interface's ipv6
 * global address structure is found, and populates foundGlobalAddr. Returns
 * non-zero if not found.
 *
 * @param [in] ifaceName           - Name of network interface
 * @param [in] foundGlobalAddr     - Will be populated if found
 *
 * @returns zero if found; non-zero otherwise
 *
 */
int getGlobalIn6AddrByIface(std::string & ifaceName, struct in6_addr & foundGlobalAddr);


} // namespace utils

} // namespace telux


#endif // #ifndef NETWORKUTILS_HPP
