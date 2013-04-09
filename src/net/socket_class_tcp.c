/**
 * @file   socket_class_tcp.c
 * @author reginaldl <reginald.l@gmail.com> - Copyright 2013
 * @date   Fri Mar  8 16:01:17 2013
 *
 * @brief  TCP socket class
 *
 *
 */

#include "rinoo/rinoo.h"

const t_rinoosocket_class socket_class_tcp = {
	.domain = AF_INET,
	.type = SOCK_STREAM,
	.create = rinoo_socket_class_tcp_create,
	.destroy = rinoo_socket_class_tcp_destroy,
	.open = rinoo_socket_class_tcp_open,
	.close = rinoo_socket_class_tcp_close,
	.read = rinoo_socket_class_tcp_read,
	.write = rinoo_socket_class_tcp_write,
	.connect = rinoo_socket_class_tcp_connect,
	.listen = rinoo_socket_class_tcp_listen,
	.accept = rinoo_socket_class_tcp_accept
};

/**
 * Allocates a TCP socket.
 *
 * @param sched Scheduler pointer
 *
 * @return Pointer to the new socket or NULL if an error occurs
 */
t_rinoosocket *rinoo_socket_class_tcp_create(t_rinoosched *sched)
{
	t_rinoosocket *socket;

	socket = calloc(1, sizeof(*socket));
	if (unlikely(socket == NULL)) {
		return NULL;
	}
	socket->node.sched = sched;
	return socket;
}

/**
 * Frees a TCP socket.
 *
 * @param socket Socket pointer
 */
void rinoo_socket_class_tcp_destroy(t_rinoosocket *socket)
{
	free(socket);
}

/**
 * Opens a TCP socket.
 *
 * @param sock Socket pointer.
 *
 * @return 0 on success or -1 if an error occurs
 */
int rinoo_socket_class_tcp_open(t_rinoosocket *sock)
{
	int fd;
	int enabled;

	XASSERT(sock != NULL, -1);

	fd = socket(sock->class->domain, sock->class->type, 0);
	if (unlikely(fd == -1)) {
		return -1;
	}
	sock->node.fd = fd;
	enabled = 1;
	if (unlikely(ioctl(sock->node.fd, FIONBIO, &enabled) == -1)) {
		close(fd);
		return -1;
	}
	return 0;
}

/**
 * Closes a TCP socket.
 *
 * @param socket Socket pointer
 *
 * @return 0 on success or -1 if an error occurs
 */
int rinoo_socket_class_tcp_close(t_rinoosocket *socket)
{
	XASSERT(socket != NULL, -1);

	return close(socket->node.fd);
}

/**
 * Replacement to the read(2) syscall in this library.
 * This function waits for the socket to be available for read operations and calls the read(2) syscall.
 *
 * @param socket Pointer to the socket to read
 * @param buf Buffer where to store the information read
 * @param count Buffer size
 *
 * @return The number of bytes read on success or -1 if an error occurs
 */
ssize_t rinoo_socket_class_tcp_read(t_rinoosocket *socket, void *buf, size_t count)
{
	ssize_t ret;

	if (rinoo_socket_waitio(socket) != 0) {
		return -1;
	}
	errno = 0;
	while ((ret = read(socket->node.fd, buf, count)) < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			return -1;
		}
		if (rinoo_socket_waitin(socket) != 0) {
			return -1;
		}
		errno = 0;
	}
	if (ret <= 0) {
		return -1;
	}
	return ret;
}

/**
 * Replacement to the write(2) syscall in this library.
 * This function waits for the socket to be available for write operations and calls the write(2) syscall.
 *
 * @param socket Pointer to the socket to read
 * @param buf Buffer which stores the information to write
 * @param count Buffer size
 *
 * @return The number of bytes written on success or -1 if an error occurs
 */
ssize_t	rinoo_socket_class_tcp_write(t_rinoosocket *socket, const void *buf, size_t count)
{
	size_t sent;
	ssize_t ret;

	if (rinoo_socket_waitio(socket) != 0) {
		return -1;
	}
	sent = count;
	while (count > 0) {
		errno = 0;
		ret = write(socket->node.fd, buf, count);
		if (ret == 0) {
			return -1;
		} else if (ret < 0) {
			if (errno != EAGAIN && errno != EWOULDBLOCK) {
				return -1;
			}
			if (rinoo_socket_waitout(socket) != 0) {
				return -1;
			}
			ret = 0;
		} else if (rinoo_socket_waitio(socket) != 0) {
			return -1;
		}
		count -= ret;
		buf += ret;
	}
	return sent;
}

/**
 * Replacement to the connect(2) syscall.
 *
 * @param socket Pointer to the socket to connect.
 * @param addr Pointer to a sockaddr structure (see man connect)
 * @param addrlen Sockaddr structure size (see man connect)
 *
 * @return 0 on success or -1 if an error occurs (timeout is considered as an error)
 */
int rinoo_socket_class_tcp_connect(t_rinoosocket *socket, const struct sockaddr *addr, socklen_t addrlen)
{
	int val;
	int enabled;
	socklen_t size;

	XASSERT(socket != NULL, -1);

	errno = 0;
	enabled = 1;
	if (setsockopt(socket->node.fd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)) != 0) {
		return -1;
	}
	if (connect(socket->node.fd, addr, addrlen) == 0) {
		return 0;
	}
	switch (errno) {
	case EINPROGRESS:
	case EALREADY:
		break;
	default:
		return -1;
	}
	if (rinoo_socket_waitout(socket) != 0) {
		return -1;
	}
	size = sizeof(val);
	if (getsockopt(socket->node.fd, SOL_SOCKET, SO_ERROR, (void *) &val, &size) < 0) {
		return -1;
	}
	if (val != 0) {
		errno = val;
		return -1;
	}
	return 0;
}

/**
 * Marks the specified socket to be listening to new connection.
 * Additionally, this function will bind the socket to the specified addr info.
 * This is a replacement of the listen(2) syscall in this library.
 *
 * @param socket Pointer to the socket to listen to
 * @param addr Pointer to a sockaddr structure (see man listen)
 * @param addrlen Sockaddr structure size (see man listen)
 * @param backlog Maximum listening queue size (see man listen)
 *
 * @return 0 on success or -1 if an error occurs
 */
int rinoo_socket_class_tcp_listen(t_rinoosocket *socket, const struct sockaddr *addr, socklen_t addrlen, int backlog)
{
	int enabled;

	enabled = 1;
	if (setsockopt(socket->node.fd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)) == -1 ||
	    bind(socket->node.fd, addr, addrlen) == -1 ||
	    listen(socket->node.fd, backlog) == -1) {
		return -1;
	}
	return 0;
}

/**
 * Accepts a new connection from a listening socket.
 * This is a replacement to the accept(2) syscall in this library.
 *
 * @param socket Pointer to the socket which is listening to
 * @param addr Address to the peer socket (see man accept)
 * @param addrlen Sockaddr structure size (see man accept)
 *
 * @return A pointer to the new client socket or NULL if an error occurs
 */
t_rinoosocket *rinoo_socket_class_tcp_accept(t_rinoosocket *socket, struct sockaddr *addr, socklen_t *addrlen)
{
	int fd;
	int enabled;
	t_rinoosocket *new;

	if (rinoo_socket_waitio(socket) != 0) {
		return NULL;
	}
	errno = 0;
	while ((fd = accept(socket->node.fd, addr, addrlen)) < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			return NULL;
		}
		if (rinoo_socket_waitin(socket) != 0) {
			return NULL;
		}
		errno = 0;
	}
	new = calloc(1, sizeof(*new));
	if (unlikely(new == NULL)) {
		close(fd);
		return NULL;
	}
	new->node.fd = fd;
	new->node.sched = socket->node.sched;
	new->parent = socket;
	new->class = socket->class;
	enabled = 1;
	if (unlikely(ioctl(new->node.fd, FIONBIO, &enabled) == -1)) {
		close(fd);
		free(new);
		return NULL;
	}
	return new;
}

