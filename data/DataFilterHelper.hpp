/*
 *  Copyright (c) 2019 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef DATAFILTERHELPER_HPP
#define DATAFILTERHELPER_HPP

#include "telux/data/DataDefines.hpp"
#include "IpFilterImpl.hpp"
#include "DsiClient.hpp"
#include "DataHelper.hpp"

namespace telux {
namespace data {

class DataFilterHelper {
 public:

    /**
     * Function to convert DFS Restrict Filter from Telematics-SDK format to DSI DFS format
     *
     * @param [in] filter - Data Restrict Filter object in Telematics-SDK format
     *
     * @returns dsi_ip_filter_info
     */
    static bool convertFilterToDSIFilter(
        const std::shared_ptr<IpFilterImpl> &filter, dsi_ip_filter_info &dfsFilter);

    static int fillIPParameters(
        dsi_ip_filter_info &dfsFilter, const std::shared_ptr<IpFilterImpl> &filter);
    static void fillUdpParameters(
        dsi_ip_filter_info &dfsFilter, const std::shared_ptr<IpFilterImpl> &filter);
    static void fillTcpParameters(
        dsi_ip_filter_info &dfsFilter, const std::shared_ptr<IpFilterImpl> &filter);

    static void dumpFilter(dsi_ip_filter_info &dfsFilter);

    static uint32_t convertIPv4AddressToInt(std::string addr);
};

}  // end of namespace data
}  // end of namespace telux

#endif  // end of DATAFILTERHELPER_HPP
