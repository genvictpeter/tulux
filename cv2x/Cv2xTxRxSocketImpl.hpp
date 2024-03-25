/*
 *  Copyright (c) 2020 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef CV2XTXRXSOCKETIMPL_HPP
#define CV2XTXRXSOCKETIMPL_HPP

#include <telux/cv2x/Cv2xTxRxSocket.hpp>

#include "qmi/WdsQmiClient.hpp"

namespace telux {

namespace cv2x {

/**
 * This class encapsulates a CV2X socket associated with a Tx flow
 * and a Rx subscription.
 */
class Cv2xTxRxSocket : public ICv2xTxRxSocket {
public:

    Cv2xTxRxSocket(uint32_t serviceId, int sock, const sockaddr_in6 &srcSockAddr);

    virtual uint32_t getId() const;

    virtual uint32_t getServiceId() const;

    virtual int getSocket() const;

    virtual struct sockaddr_in6 getSocketAddr() const;

    virtual uint16_t getPortNum() const;

    virtual ~Cv2xTxRxSocket() {}

protected:

    static std::atomic<uint32_t> id;

    uint32_t id_;

    uint32_t serviceId_;

    int sock_ = -1;

    struct sockaddr_in6 sockAddr_ = {0};
};

} // namespace cv2x

} // namespace telux

#endif // #ifndef CV2XTXRXSOCKETIMPL_HPP
