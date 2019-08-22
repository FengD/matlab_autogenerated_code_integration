/* -*- mode: C++ -*-
 *
 *  Copyright (C) 2019 Feng DING, Hirain
 *
 *  License: Modified BSD Software License Agreement
 *
 */


#include "can_no_fd.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <stdbool.h>

static int can_socket_cfg(struct can_hdl *hdl, struct can_cfg *cfg) {
	// int mtu,
	int ret = 0;
	struct sockaddr_can addr;
	struct ifreq ifr;

	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, cfg->ifname);

	hdl->fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (hdl->fd < 0) {
		fprintf(stderr, "%s Error while opening socket. errno: %d\n",
			__func__, errno);
		ret = -1;
		goto out;
	}

	if ((ioctl(hdl->fd, SIOCGIFINDEX, &ifr)) == -1) {
		fprintf(stderr, "%s Error getting interface index. errno: %d\n",
			__func__, errno);
		ret = -2;
		goto out;
	}

	hdl->ifindex = ifr.ifr_ifindex;

	if (cfg->mtu > CAN_MTU) {
		/* check if the frame fits into the CAN netdevice */
		if (ioctl(hdl->fd, SIOCGIFMTU, &ifr) == -1) {
			fprintf(stderr,
				"%s Error getting interface MTU. errno: %d\n",
				__func__, errno);
			ret = -3;
			goto out;
		}
	}

	addr.can_family  = AF_CAN;
	addr.can_ifindex = hdl->ifindex;
	ret = bind(hdl->fd, (struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0) {
		fprintf(stderr, "%s Error in socket bind. errno: %d\n",
			__func__, errno);
		ret = -6;
		goto out;
	}

	if (cfg->rx_filter)
		/* Setup CAN ID filter*/
		setsockopt(hdl->fd, SOL_CAN_RAW, CAN_RAW_FILTER,
			   cfg->rx_filter, sizeof(struct can_filter) * cfg->rx_filter_len);

out:
	return ret;
}

int can_open(struct can_hdl **hdl, struct can_cfg *cfg) {
	int ret;

	*hdl = (struct can_hdl *) malloc(sizeof(struct can_hdl));
	if (!*hdl) {
		ret = -1;
		goto err;
	}

	memset(*hdl, 0, sizeof(struct can_hdl));

	ret = can_socket_cfg(*hdl, cfg);
	if (ret != 0)
		goto err;

	goto out;
err:
	free(*hdl);
	*hdl = NULL;
out:
	return ret;
}

int can_close(struct can_hdl **hdl) {
	int ret;

	if (!hdl || !(*hdl)) {
		ret = -1;
		goto out;
	}

	ret = close((*hdl)->fd);
	if (ret < 0)
		ret = -errno;

	free(*hdl);
	*hdl = NULL;
out:
	return ret;
}

ssize_t can_read(struct can_hdl *hdl, struct can_frame *frame) {
	return read(hdl->fd, frame, sizeof(struct can_frame));
}

ssize_t can_write(struct can_hdl *hdl, const struct can_frame *frame) {
	ssize_t nbytes = -1;
	struct sockaddr_can addr;

	addr.can_ifindex = hdl->ifindex;
	addr.can_family  = AF_CAN;

	nbytes = sendto(hdl->fd, frame, sizeof(struct can_frame), MSG_DONTWAIT,
			(struct sockaddr *)&addr, sizeof(addr));


	if ((nbytes < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
		/* Socket TX buffer is full. This could happen if the unit is
		 * not connected to a CAN bus.
		 */
		nbytes = 0;
	return nbytes;
}
