/*
 * Copyright (c) 2019-2020, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * @file btp.c
 * @brief Implement BTP(Basic Transport Protocol)
 */
#include "btp.h"
#include <arpa/inet.h>
#include "v2x_msg.h"

int btp_encode(msg_contents *mc) {
    abuf_t *bp = &mc->abuf;
    btp_data_t *btp = (btp_data_t *)mc->btp;

    if (!mc || !mc->btp || !mc->abuf.data) {
        fprintf(stderr, "%s: invalid parameters\n", __func__);
        return -1;
    }
    if ((bp->data - bp->head) < sizeof(struct _btp_hdr_A)) {
        fprintf(stderr, "%s: buffer too small\n", __func__);
        return -1;
    }
    if (btp->pkt_type == BTP_PACKET_TYPE_A) {
        struct _btp_hdr_A *h = (struct _btp_hdr_A *)abuf_push(bp, sizeof(struct _btp_hdr_A));
        h->d_port = htons(btp->d_port);
        h->s_port = htons(btp->s_port);
    } else {
        struct _btp_hdr_B *h = (struct _btp_hdr_B *)abuf_push(bp, sizeof(struct _btp_hdr_B));
        h->d_port = htons(btp->d_port);
        h->dp_info = htons(btp->dp_info);
    }
    return 0;
}

/**
 * NOTE: the pkt_type will be supplied by lower stack layer, e.g. GeoNetwork
 */
int btp_decode(msg_contents *mc ) {
    btp_data_t *btp;

    if (!mc || !mc->abuf.data) {
        fprintf(stderr, "%s: invalid input parameters\n", __func__);
        return -1;
    }
    if (!mc->btp) {
        mc->btp = calloc(sizeof(btp_data_t), 1);
    }
    btp = (btp_data_t *)mc->btp;
    //mc->btp->pkt_type should've been filled by lower layer stack,e.g
    //GeoNetwork.
    if (btp->pkt_type == BTP_PACKET_TYPE_A) {
        struct _btp_hdr_A *h = (struct _btp_hdr_A *)abuf_pull(&mc->abuf, sizeof(struct _btp_hdr_A));
        btp->d_port = ntohs(h->d_port);
        btp->s_port = ntohs(h->s_port);
    } else {
        struct _btp_hdr_B *h = (struct _btp_hdr_B *)abuf_pull(&mc->abuf, sizeof(struct _btp_hdr_B));
        btp->d_port = ntohs(h->d_port);
        btp->dp_info = ntohs(h->dp_info);
    }
    return 0;
}
