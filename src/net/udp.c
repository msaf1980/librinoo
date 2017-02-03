/**
 * @file   udp.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  UDP socket management
 *
 *
 */

#include "rinoo/net/module.h"

extern const rn_socket_class_t socket_class_udp;
extern const rn_socket_class_t socket_class_udp6;

rn_socket_t *rn_udp_client(rn_sched_t *sched, rn_addr_t *dst)
{
	rn_socket_t *socket;

	socket = rn_socket(sched, (IS_IPV6(dst) ? &socket_class_udp6 : &socket_class_udp));
	if (unlikely(socket == NULL)) {
		return NULL;
	}
	if (rn_socket_connect(socket, dst) != 0) {
		rn_socket_destroy(socket);
		return NULL;
	}
	return socket;
}
