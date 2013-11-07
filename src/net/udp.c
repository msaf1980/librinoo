/**
 * @file   udp.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Mon Oct 21 22:52:23 2013
 *
 * @brief  UDP socket management
 *
 *
 */

#include "rinoo/rinoo.h"

extern const t_rinoosocket_class socket_class_udp;

t_rinoosocket *rinoo_udp(t_rinoosched *sched)
{
	return rinoo_socket(sched, &socket_class_udp);
}

t_rinoosocket *rinoo_udp_client(t_rinoosched *sched, t_ip ip, uint16_t port)
{
	t_rinoosocket *socket;
	struct sockaddr_in addr;

	socket = rinoo_udp(sched);
	if (unlikely(socket == NULL)) {
		return NULL;
	}
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	if (rinoo_socket_connect(socket, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
		rinoo_socket_destroy(socket);
		return NULL;
	}
	return socket;
}
