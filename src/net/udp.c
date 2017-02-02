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

rn_socket_t *rn_udp_client(rn_sched_t *sched, rn_ip_t *ip, uint16_t port)
{
	rn_ip_t loopback;
	rn_socket_t *socket;
	socklen_t addr_len;
	struct sockaddr *addr;

	if (ip == NULL) {
		memset(&loopback, 0, sizeof(loopback));
		loopback.v4.sin_family = AF_INET;
		loopback.v4.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		ip = &loopback;
	}
	socket = rn_socket(sched, (IS_IPV6(ip) ? &socket_class_udp6 : &socket_class_udp));
	if (unlikely(socket == NULL)) {
		return NULL;
	}
	if (ip->v4.sin_family == AF_INET) {
		ip->v4.sin_port = htons(port);
		addr = (struct sockaddr *) &ip->v4;
		addr_len = sizeof(ip->v4);
	} else {
		ip->v6.sin6_port = htons(port);
		addr = (struct sockaddr *) &ip->v6;
		addr_len = sizeof(ip->v6);
	}
	if (rn_socket_connect(socket, addr, addr_len) != 0) {
		rn_socket_destroy(socket);
		return NULL;
	}
	return socket;
}
