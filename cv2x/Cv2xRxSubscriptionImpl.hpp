/*
 *  Copyright (c) 2018 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef CV2XRXSUBSCRIPTIONIMPL_HPP
#define CV2XRXSUBSCRIPTIONIMPL_HPP

#include <atomic>
#include <map>
#include <vector>
#include <memory>

#include <telux/cv2x/Cv2xRxSubscription.hpp>

#include "qmi/WdsQmiClient.hpp"


namespace telux {

namespace cv2x {


class Cv2xRxSubscription : public ICv2xRxSubscription {
public:
    Cv2xRxSubscription(int sock, const struct sockaddr_in6 & sockAddr);

    virtual uint32_t getSubscriptionId() const;

    virtual TrafficIpType getIpType() const;

    virtual int getSock() const;

    virtual struct sockaddr_in6 getSockAddr() const;

    virtual uint16_t getPortNum() const;

    virtual void closeSock();

    virtual std::shared_ptr<std::vector<uint32_t>> getServiceIDList() const;

    virtual void setServiceIDList(const std::shared_ptr<std::vector<uint32_t>> idList);

    virtual ~Cv2xRxSubscription() {}

protected:

    static std::atomic<uint32_t> Id;

    uint32_t id_;
    TrafficIpType ipType_;
    int sock_ = -1;
    struct sockaddr_in6 sockAddr_ = {0};
    std::shared_ptr<std::vector<uint32_t>> idList_ = nullptr;
};


} // namespace cv2x

} // namespace telux


#endif // #ifndef CV2XRXSUBSCRIPTIONIMPL_HPP
