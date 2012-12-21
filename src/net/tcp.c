/**
 * @file   tcp.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Tue Mar 20 18:06:41 2012
 *
 * @brief  TCP connection management
 *
 *
 */

#include"rinoo/rinoo.h"

/**
 * Creates a TCP socket.
 *
 * @param sched Pointer to a scheduler
 *
 * @return A pointer to a socket structure on success, or NULL if an error occurs
 */
t_rinoosocket *rinoo_tcp(t_rinoosched *sched)
{
	return rinoo_socket(sched, AF_INET, SOCK_STREAM);
}

t_rinoosocket *rinoo_tcp_client(t_rinoosched *sched, t_ip ip, u32 port, u32 unused(timeout))
{
	t_rinoosocket *socket;

	socket = rinoo_tcp(sched);
	if (unlikely(socket == NULL)) {
		return NULL;
	}
	if (rinoo_tcp_connect(socket, ip, port) != 0) {
		rinoo_socket_destroy(socket);
		return NULL;
	}
	return socket;
}

t_rinoosocket *rinoo_tcp_server(t_rinoosched *sched, t_ip ip, u32 port)
{
	t_rinoosocket *socket;

	socket = rinoo_tcp(sched);
	if (unlikely(socket == NULL)) {
		return NULL;
	}
	if (rinoo_tcp_listen(socket, ip, port) != 0) {
		rinoo_socket_destroy(socket);
		return NULL;
	}
	return socket;
}

/**
 * Connects a TCP socket to the specified ip and port.
 * Occasionally, it sets a timeout on the socket.
 *
 * @param socket Pointer to the socket to connect
 * @param ip IP to connect to
 * @param port Port to connect to
 *
 * @return 0 on success or -1 if an error occurs (timeout is considered as an error)
 */
int rinoo_tcp_connect(t_rinoosocket *socket, t_ip ip, u32 port)
{
	struct sockaddr_in addr;

	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	return rinoo_socket_connect(socket, (struct sockaddr *) &addr, sizeof(addr));
}

/**
 * Marks the specified socket to be listening to new connection using rinoo_tcp_accept.
 * Additionally, this function will bind the socket to the specified ip and port.
 *
 * @param socket Pointer to the socket to use
 * @param ip Ip to bind
 * @param port Port to bind
 *
 * @return 0 on success or -1 if an error occurs
 */
int rinoo_tcp_listen(t_rinoosocket *socket, t_ip ip, u32 port)
{
	struct sockaddr_in addr;

	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	return rinoo_socket_listen(socket, (struct sockaddr *) &addr, sizeof(addr), RINOO_TCP_BACKLOG);
}

/**
 * Accepts a new connection from a listening socket.
 *
 * @param socket Pointer to the socket which is listening to
 * @param fromip Pointer to a t_ip where to store the from_ip
 * @param fromport Pointer to a u32 where to store the from_port
 *
 * @return A pointer to the new socket on success or NULL if an error occurs
 */
t_rinoosocket *rinoo_tcp_accept(t_rinoosocket *socket, t_ip *fromip, u32 *fromport)
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
