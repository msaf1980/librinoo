/**
 * @file   socket_helper.c
 * @author reginaldl <reginald.l@gmail.com>
 * @date   Fri Mar  8 16:01:17 2013
 *
 * @brief  Socket helper functions
 *
 *
 */

#include "rinoo/rinoo.h"

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
ssize_t rinoo_socket_helper_read(t_rinoosocket *socket, void *buf, size_t count)
{
	ssize_t ret;

	if (rinoo_socket_waitin(socket) != 0) {
		return -1;
	}
	ret = read(socket->fd, buf, count);
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
ssize_t	rinoo_socket_helper_write(t_rinoosocket *socket, const void *buf, size_t count)
{
	ssize_t ret;

	if (rinoo_socket_waitout(socket) != 0) {
		return -1;
	}
	ret = write(socket->fd, buf, count);
	if (ret <= 0) {
		return -1;
	}
	return ret;
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
int rinoo_socket_helper_connect(t_rinoosocket *socket, const struct sockaddr *addr, socklen_t addrlen)
{
	int val;
	int enabled;
	socklen_t size;

	XASSERT(socket != NULL, -1);

	errno = 0;
	enabled = 1;
	if (setsockopt(socket->fd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)) != 0) {
		return -1;
	}
	if (connect(socket->fd, addr, addrlen) == 0) {
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
	if (getsockopt(socket->fd, SOL_SOCKET, SO_ERROR, (void *) &val, &size) < 0) {
		return -1;
	}
	if (val != 0) {
		errno = val;
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
t_rinoosocket *rinoo_socket_helper_accept(t_rinoosocket *socket, struct sockaddr *addr, socklen_t *addrlen)
{
	int fd;
	int enabled;
	t_rinoosocket *new;

	if (rinoo_socket_waitin(socket) != 0) {
		return NULL;
	}
	fd = accept(socket->fd, addr, addrlen);
	if (fd == -1) {
		return NULL;
	}
	new = calloc(1, sizeof(*new));
	if (unlikely(new == NULL)) {
		close(fd);
		return NULL;
	}
	new->fd = fd;
	new->parent = socket;
	new->sched = socket->sched;
	enabled = 1;
	if (unlikely(ioctl(new->fd, FIONBIO, &enabled) == -1)) {
		close(fd);
		free(new);
		return NULL;
	}
	return new;
}

