/**
 * @file   socket_class_tcp.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  TCP socket class
 *
 *
 */

#include "rinoo/net/module.h"

const rn_socket_class_t socket_class_tcp = {
	.domain = AF_INET,
	.type = SOCK_STREAM,
	.create = rn_socket_class_tcp_create,
	.destroy = rn_socket_class_tcp_destroy,
	.open = rn_socket_class_tcp_open,
	.dup = rn_socket_class_tcp_dup,
	.close = rn_socket_class_tcp_close,
	.read = rn_socket_class_tcp_read,
	.recvfrom = rn_socket_class_tcp_recvfrom,
	.write = rn_socket_class_tcp_write,
	.writev = rn_socket_class_tcp_writev,
	.sendto = rn_socket_class_tcp_sendto,
	.sendfile = rn_socket_class_tcp_sendfile,
	.connect = rn_socket_class_tcp_connect,
	.bind = rn_socket_class_tcp_bind,
	.accept = rn_socket_class_tcp_accept
};

const rn_socket_class_t socket_class_tcp6 = {
	.domain = AF_INET6,
	.type = SOCK_STREAM,
	.create = rn_socket_class_tcp_create,
	.destroy = rn_socket_class_tcp_destroy,
	.open = rn_socket_class_tcp_open,
	.dup = rn_socket_class_tcp_dup,
	.close = rn_socket_class_tcp_close,
	.read = rn_socket_class_tcp_read,
	.recvfrom = rn_socket_class_tcp_recvfrom,
	.write = rn_socket_class_tcp_write,
	.writev = rn_socket_class_tcp_writev,
	.sendto = rn_socket_class_tcp_sendto,
	.sendfile = rn_socket_class_tcp_sendfile,
	.connect = rn_socket_class_tcp_connect,
	.bind = rn_socket_class_tcp_bind,
	.accept = rn_socket_class_tcp_accept
};

/**
 * Allocates a TCP socket.
 *
 * @param sched Scheduler pointer
 *
 * @return Pointer to the new socket or NULL if an error occurs
 */
rn_socket_t *rn_socket_class_tcp_create(rn_sched_t *sched)
{
	rn_socket_t *socket;

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
void rn_socket_class_tcp_destroy(rn_socket_t *socket)
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
int rn_socket_class_tcp_open(rn_socket_t *sock)
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
 * Duplicates a TCP socket.
 *
 * @param destination Destination scheduler
 * @param socket Socket to duplicate
 *
 * @return Pointer to the new socket or NULL if an error occurs
 */
rn_socket_t *rn_socket_class_tcp_dup(rn_sched_t *destination, rn_socket_t *socket)
{
	rn_socket_t *new;

	new = calloc(1, sizeof(*new));
	if (unlikely(new == NULL)) {
		return NULL;
	}
	*new = *socket;
	new->node.fd = dup(socket->node.fd);
	if (unlikely(new->node.fd < 0)) {
		free(new);
		return NULL;
	}
	new->node.sched = destination;
	return new;
}

/**
 * Closes a TCP socket.
 *
 * @param socket Socket pointer
 *
 * @return 0 on success or -1 if an error occurs
 */
int rn_socket_class_tcp_close(rn_socket_t *socket)
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
ssize_t rn_socket_class_tcp_read(rn_socket_t *socket, void *buf, size_t count)
{
	ssize_t ret;

	if (rn_socket_waitio(socket) != 0) {
		return -1;
	}
	errno = 0;
	while ((ret = read(socket->node.fd, buf, count)) < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			return -1;
		}
		if (rn_socket_waitin(socket) != 0) {
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
 * Replacement to the recvfrom(2) syscall in this library.
 * This function waits for the socket to be available for read operations and calls the recvfrom(2) syscall.
 *
 * @param socket Pointer to the socket to read
 * @param buf Buffer where to store the information read
 * @param count Buffer size
 * @param addrfrom Sockaddr where to store the source address
 * @param addrlen Socklen where to store the size of the source address
 *
 * @return The number of bytes read on success or -1 if an error occurs
 */
ssize_t rn_socket_class_tcp_recvfrom(rn_socket_t *socket, void *buf, size_t count, struct sockaddr *addrfrom, socklen_t *addrlen)
{
	ssize_t ret;

	if (rn_socket_waitio(socket) != 0) {
		return -1;
	}
	errno = 0;
	while ((ret = recvfrom(socket->node.fd, buf, count, MSG_DONTWAIT, addrfrom, addrlen)) < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			return -1;
		}
		if (rn_socket_waitin(socket) != 0) {
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
ssize_t	rn_socket_class_tcp_write(rn_socket_t *socket, const void *buf, size_t count)
{
	size_t sent;
	ssize_t ret;

	sent = count;
	while (count > 0) {
		if (rn_socket_waitio(socket) != 0) {
			return -1;
		}
		errno = 0;
		ret = write(socket->node.fd, buf, count);
		if (ret == 0) {
			return -1;
		} else if (ret < 0) {
			if (errno != EAGAIN && errno != EWOULDBLOCK) {
				return -1;
			}
			if (rn_socket_waitout(socket) != 0) {
				return -1;
			}
			ret = 0;
		}
		count -= ret;
		buf += ret;
	}
	return sent;
}

/**
 * Replacement to the writev(2) syscall in this library.
 * This function waits for the socket to be available for write operations and calls the write(2) syscall.
 *
 * @param socket Pointer to the socket to read
 * @param buffers Array of buffers
 * @param count Array size
 *
 * @return The number of bytes written on success or -1 if an error occurs
 */
ssize_t	rn_socket_class_tcp_writev(rn_socket_t *socket, rn_buffer_t **buffers, int count)
{
	int i;
	ssize_t ret;
	ssize_t sent;
	size_t total;
	struct iovec *iov;

	if (count > IOV_MAX) {
		errno = EINVAL;
		return -1;
	}
	iov = alloca(sizeof(*iov) * count);
	for (i = 0, total = 0; i < count; i++) {
		iov[i].iov_base = rn_buffer_ptr(buffers[i]);
		iov[i].iov_len = rn_buffer_size(buffers[i]);
		total += rn_buffer_size(buffers[i]);
	}
	sent = 0;
	while (count > 0) {
		if (rn_socket_waitio(socket) != 0) {
			return -1;
		}
		errno = 0;
		ret = writev(socket->node.fd, iov, count);
		if (ret == 0) {
			return -1;
		} else if (ret < 0) {
			if (errno != EAGAIN && errno != EWOULDBLOCK) {
				return -1;
			}
			if (rn_socket_waitout(socket) != 0) {
				return -1;
			}
			ret = 0;
		}
		sent += ret;
		if (((size_t) sent) == total) {
			break;
		}
		for (i = 0; i < count; i++) {
			if (ret > 0 && ((size_t) ret) >= iov[i].iov_len) {
				ret -= iov[i].iov_len;
			} else {
				iov[i].iov_base += ret;
				iov[i].iov_len -= ret;
				iov = &iov[i];
				break;
			}
		}
		count -= i;
	}
	return sent;
}

/**
 * Equivalent to rn_socket_class_tcp_write, addrfrom and addrlen are ignored.
 *
 * @param socket Pointer to the socket to read
 * @param buf Buffer which stores the information to write
 * @param count Buffer size
 * @param addrto Ignored
 * @param addrlen Ignored
 *
 * @return The number of bytes written on success or -1 if an error occurs
 */
ssize_t rn_socket_class_tcp_sendto(rn_socket_t *socket, void *buf, size_t count, const struct sockaddr *unused(addrto), socklen_t unused(addrlen))
{
	return rn_socket_class_tcp_write(socket, buf, count);
}

/**
 * Replacement function for sendfile(2).
 *
 * @param socket Pointer to the socket to send the file to
 * @param in_fd File descriptor of file to send
 * @param offset File offset
 * @param count Number of bytes to send
 *
 * @return Number of bytes correctly sent or -1 if an error occurs
 */
ssize_t rn_socket_class_tcp_sendfile(rn_socket_t *socket, int in_fd, off_t offset, size_t count)
{
	size_t sent;
	ssize_t ret;

	if (rn_socket_waitio(socket) != 0) {
		return -1;
	}
	sent = count;
	while (count > 0) {
		errno = 0;
		ret = sendfile(socket->node.fd, in_fd, &offset, count);
		if (ret == 0) {
			return -1;
		} else if (ret < 0) {
			if (errno != EAGAIN && errno != EWOULDBLOCK) {
				return -1;
			}
			if (rn_socket_waitout(socket) != 0) {
				return -1;
			}
			ret = 0;
		} else if (rn_socket_waitio(socket) != 0) {
			return -1;
		}
		count -= ret;
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
int rn_socket_class_tcp_connect(rn_socket_t *socket, const struct sockaddr *addr, socklen_t addrlen)
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
	if (rn_socket_waitout(socket) != 0) {
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
int rn_socket_class_tcp_bind(rn_socket_t *socket, const struct sockaddr *addr, socklen_t addrlen, int backlog)
{
	int enabled;

	enabled = 1;

#ifdef SO_REUSEPORT
	if (setsockopt(socket->node.fd, SOL_SOCKET, SO_REUSEPORT, &enabled, sizeof(enabled)) == -1) {
		return -1;
	}
#endif /* !SO_REUSEPORT */

	if (setsockopt(socket->node.fd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)) == -1) {
		return -1;
	}
	if (bind(socket->node.fd, addr, addrlen) == -1) {
		return -1;
	}
	if (listen(socket->node.fd, backlog) == -1) {
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
rn_socket_t *rn_socket_class_tcp_accept(rn_socket_t *socket, struct sockaddr *addr, socklen_t *addrlen)
{
	int fd;
	rn_socket_t *new;

	if (rn_socket_waitio(socket) != 0) {
		return NULL;
	}
	errno = 0;
	while ((fd = accept4(socket->node.fd, addr, addrlen, SOCK_NONBLOCK)) < 0) {
		switch (errno) {
			case EAGAIN:
			case ENETDOWN:
			case EPROTO:
			case ENOPROTOOPT:
			case EHOSTDOWN:
			case ENONET:
			case EHOSTUNREACH:
			case EOPNOTSUPP:
			case ENETUNREACH:
				break;
			default:
				return NULL;
		}
		if (rn_socket_waitin(socket) != 0) {
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
	return new;
}
