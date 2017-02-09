/**
 * @file   tcp.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  TCP connection management
 *
 *
 */

#include "rinoo/net/module.h"

extern const rn_socket_class_t socket_class_tcp;
extern const rn_socket_class_t socket_class_tcp6;

/**
 * Creates a TCP client to be connected to a specific address.
 *
 * @param sched Scheduler pointer
 * @param dst Destination address to connect to
 * @param timeout Socket timeout
 *
 * @return Socket pointer on success or NULL if an error occurs
 */
rn_socket_t *rn_tcp_client(rn_sched_t *sched, rn_addr_t *dst, uint32_t timeout)
{
	rn_socket_t *socket;

	socket = rn_socket(sched, (IS_IPV6(dst) ? &socket_class_tcp6 : &socket_class_tcp));
	if (unlikely(socket == NULL)) {
		return NULL;
	}
	if (timeout != 0 && rn_socket_timeout(socket, timeout) != 0) {
		rn_socket_destroy(socket);
		return NULL;
	}
	if (rn_socket_connect(socket, dst) != 0) {
		rn_socket_destroy(socket);
		return NULL;
	}
	return socket;
}

/**
 * Creates a TCP server listening to a specific address.
 *
 * @param sched Scheduler pointer
 * @param dst Address to bind
 *
 * @return Socket pointer to the server on success or NULL if an error occurs
 */
rn_socket_t *rn_tcp_server(rn_sched_t *sched, rn_addr_t *dst)
{
	rn_socket_t *socket;

	socket = rn_socket(sched, (IS_IPV6(dst) ? &socket_class_tcp6 : &socket_class_tcp));
	if (unlikely(socket == NULL)) {
		return NULL;
	}
	if (rn_socket_bind(socket, dst, RN_TCP_BACKLOG) != 0) {
		rn_socket_destroy(socket);
		return NULL;
	}
	return socket;
}
