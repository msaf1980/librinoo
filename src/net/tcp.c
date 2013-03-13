/**
 * @file   tcp.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Tue Mar 20 18:06:41 2012
 *
 * @brief  TCP connection management
 *
 *
 */

#include "rinoo/rinoo.h"

extern const t_rinoosocket_class socket_class_tcp;

/**
 * Creates a TCP client to be connected to a specific IP, on a specific port.
 *
 * @param sched Scheduler pointer
 * @param ip Destination IP to connect to
 * @param port Destination port to connect to
 * @param timeout Socket timeout
 *
 * @return Socket pointer on success or NULL if an error occurs
 */
t_rinoosocket *rinoo_tcp_client(t_rinoosched *sched, t_ip ip, uint32_t port, uint32_t timeout)
{
	t_rinoosocket *socket;
	struct sockaddr_in addr;

	socket = rinoo_socket(sched, &socket_class_tcp);
	if (unlikely(socket == NULL)) {
		return NULL;
	}
	if (timeout != 0 && rinoo_socket_timeout(socket, timeout) != 0) {
		rinoo_socket_destroy(socket);
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

/**
 * Creates a TCP server listening to a specific port, on a specific IP.
 *
 * @param sched Scheduler pointer
 * @param ip IP to bind
 * @param port Port to bind
 *
 * @return Socket pointer to the server on success or NULL if an error occurs
 */
t_rinoosocket *rinoo_tcp_server(t_rinoosched *sched, t_ip ip, uint32_t port)
{
	t_rinoosocket *socket;
	struct sockaddr_in addr;

	socket = rinoo_socket(sched, &socket_class_tcp);
	if (unlikely(socket == NULL)) {
		return NULL;
	}
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	if (rinoo_socket_listen(socket, (struct sockaddr *) &addr, sizeof(addr), RINOO_TCP_BACKLOG) != 0) {
		rinoo_socket_destroy(socket);
		return NULL;
	}
	return socket;
}

/**
 * Accepts a new connection from a listening socket.
 *
 * @param socket Pointer to the socket which is listening to
 * @param fromip Pointer to a t_ip where to store the from_ip
 * @param fromport Pointer to a uint32_t where to store the from_port
 *
 * @return A pointer to the new socket on success or NULL if an error occurs
 */
t_rinoosocket *rinoo_tcp_accept(t_rinoosocket *socket, t_ip *fromip, uint32_t *fromport)
{
	t_rinoosocket *new;
	struct sockaddr_in addr;

	new = rinoo_socket_accept(socket, (struct sockaddr *) &addr, (socklen_t *)(int[]){(sizeof(struct sockaddr))});
	if (fromip != NULL) {
		*fromip = addr.sin_addr.s_addr;
	}
	if (fromport != NULL) {
		*fromport = ntohs(addr.sin_port);
	}
	return new;
}
