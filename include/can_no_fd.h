/* -*- mode: C++ -*-
 *
 *  Copyright (C) 2019 Feng DING, Hirain
 *
 *  License: Modified BSD Software License Agreement
 *
 */

#ifndef _CAN_NOFD_H_
#define _CAN_NOFD_H_

#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/types.h>

struct can_hdl {
	int fd;

	int ifindex;
};

struct can_cfg {
/**
* CAN interface name
*/
char *ifname;
/**
* Requested CAN MTU (Maximum Transmission Unit).
* If the value is greater than 8, the interface will be put
* in FD (Flexible Datarate) mode.
*/

size_t mtu;
/**
* Optional RX filter array.
* If NULL, no filtering will be used.
*/
struct can_filter *rx_filter;
/**
* Length of the rx_filter array
*/
size_t rx_filter_len;
};

int can_open(struct can_hdl **hdl, struct can_cfg *cfg);

int can_close(struct can_hdl **hdl);

ssize_t can_read(struct can_hdl *hdl, struct can_frame *frame);

ssize_t can_write(struct can_hdl *hdl, const struct can_frame *frame);

#endif //_CAN_NOFD_H_
