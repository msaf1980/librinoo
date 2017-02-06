/**
 * @file   socket_class_udp.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  UDP socket class
 *
 *
 */

#include "rinoo/net/module.h"

const rn_socket_class_t socket_class_udp = {
	.domain = AF_INET,
	.type = SOCK_DGRAM,
	.create = rn_socket_class_udp_create,
	.destroy = rn_socket_class_udp_destroy,
	.open = rn_socket_class_udp_open,
	.dup = rn_socket_class_udp_dup,
	.close = rn_socket_class_udp_close,
	.read = rn_socket_class_udp_read,
	.recvfrom = rn_socket_class_udp_recvfrom,
	.write = rn_socket_class_udp_write,
	.writev = rn_socket_class_udp_writev,
	.sendto = rn_socket_class_udp_sendto,
	.sendfile = NULL,
	.connect = rn_socket_class_udp_connect,
	.bind = rn_socket_class_udp_bind,
	.accept = NULL
};

const rn_socket_class_t socket_class_udp6 = {
	.domain = AF_INET6,
	.type = SOCK_DGRAM,
	.create = rn_socket_class_udp_create,
	.destroy = rn_socket_class_udp_destroy,
	.open = rn_socket_class_udp_open,
	.dup = rn_socket_class_udp_dup,
	.close = rn_socket_class_udp_close,
	.read = rn_socket_class_udp_read,
	.recvfrom = rn_socket_class_udp_recvfrom,
	.write = rn_socket_class_udp_write,
	.writev = rn_socket_class_udp_writev,
	.sendto = rn_socket_class_udp_sendto,
	.sendfile = NULL,
	.connect = rn_socket_class_udp_connect,
	.bind = rn_socket_class_udp_bind,
	.accept = NULL
};

/**
 * Allocates a UDP socket.
 *
 * @param sched Scheduler pointer
 *
 * @return Pointer to the new socket or NULL if an error occurs
 */
rn_socket_t *rn_socket_class_udp_create(rn_sched_t *sched)
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
 * Frees a UDP socket.
 *
 * @param socket Socket pointer
 */
void rn_socket_class_udp_destroy(rn_socket_t *socket)
{
	free(socket);
}

/**
 * Opens a UDP socket.
 *
 * @param sock Socket pointer.
 *
 * @return 0 on success or -1 if an error occurs
 */
int rn_socket_class_udp_open(rn_socket_t *sock)
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
 * Duplicates a UDP socket.
 *
 * @param destination Destination scheduler
 * @param socket Socket to duplicate
 *
 * @return Pointer to the new socket or NULL if an error occurs
 */
rn_socket_t *rn_socket_class_udp_dup(rn_sched_t *destination, rn_socket_t *socket)
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
 * Closes a UDP socket.
 *
 * @param socket Socket pointer
 *
 * @return 0 on success or -1 if an error occurs
 */
int rn_socket_class_udp_close(rn_socket_t *socket)
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
ssize_t rn_socket_class_udp_read(rn_socket_t *socket, void *buf, size_t count)
{
	ssize_t ret;

	if (rn_socket_waitio(socket) != 0) {
		return -1;
	}
	while ((ret = read(socket->node.fd, buf, count)) < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			rn_error_set(errno);
			return -1;
		}
		if (rn_socket_waitin(socket) != 0) {
			return -1;
		}
	}
	if (ret <= 0) {
		//FIXME set rn_error
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
 * @param from Originating address
 *
 * @return The number of bytes read on success or -1 if an error occurs
 */
ssize_t rn_socket_class_udp_recvfrom(rn_socket_t *socket, void *buf, size_t count, rn_addr_t *from)
{
	ssize_t ret;
	socklen_t addr_len;

	if (rn_socket_waitio(socket) != 0) {
		return -1;
	}
	addr_len = sizeof(*from);
	while ((ret = recvfrom(socket->node.fd, buf, count, MSG_DONTWAIT, &from->sa, &addr_len)) < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			rn_error_set(errno);
			return -1;
		}
		if (rn_socket_waitin(socket) != 0) {
			return -1;
		}
		addr_len = sizeof(*from);
	}
	if (ret <= 0) {
		//FIXME set rn_error
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
ssize_t	rn_socket_class_udp_write(rn_socket_t *socket, const void *buf, size_t count)
{
	size_t sent;
	ssize_t ret;

	sent = count;
	while (count > 0) {
		if (rn_socket_waitio(socket) != 0) {
			return -1;
		}
		ret = write(socket->node.fd, buf, count);
		if (ret == 0) {
			//FIXME set rn_error
			return -1;
		} else if (ret < 0) {
			if (errno != EAGAIN && errno != EWOULDBLOCK) {
				rn_error_set(errno);
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
ssize_t	rn_socket_class_udp_writev(rn_socket_t *socket, rn_buffer_t **buffers, int count)
{
	int i;
	ssize_t ret;
	ssize_t sent;
	size_t total;
	struct iovec *iov;

	if (count > IOV_MAX) {
		rn_error_set(EINVAL);
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
		ret = writev(socket->node.fd, iov, count);
		if (ret == 0) {
			//FIXME set rn_error
			return -1;
		} else if (ret < 0) {
			if (errno != EAGAIN && errno != EWOULDBLOCK) {
				rn_error_set(errno);
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
 * Equivalent to rn_socket_class_udp_write, addrfrom and addrlen are ignored.
 *
 * @param socket Pointer to the socket to read
 * @param buf Buffer which stores the information to write
 * @param count Buffer size
 * @param dst Destination address
 *
 * @return The number of bytes written on success or -1 if an error occurs
 */
ssize_t rn_socket_class_udp_sendto(rn_socket_t *socket, void *buf, size_t count, const rn_addr_t *dst)
{
	size_t sent;
	ssize_t ret;

	sent = count;
	while (count > 0) {
		if (rn_socket_waitio(socket) != 0) {
			return -1;
		}
		ret = sendto(socket->node.fd, buf, count, MSG_DONTWAIT, &dst->sa, sizeof(*dst));
		if (ret == 0) {
			//FIXME set rn_error
			return -1;
		} else if (ret < 0) {
			if (errno != EAGAIN && errno != EWOULDBLOCK) {
				rn_error_set(errno);
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
 * Replacement to the connect(2) syscall.
 *
 * @param socket Pointer to the socket to connect.
 * @param dst Destination address
 *
 * @return 0 on success or -1 if an error occurs (timeout is considered as an error)
 */
int rn_socket_class_udp_connect(rn_socket_t *socket, const rn_addr_t *dst)
{
	int val;
	int enabled;
	socklen_t size;

	XASSERT(socket != NULL, -1);

	enabled = 1;
	if (setsockopt(socket->node.fd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)) != 0) {
		rn_error_set(errno);
		return -1;
	}
	if (connect(socket->node.fd, &dst->sa, sizeof(*dst)) == 0) {
		return 0;
	}
	switch (errno) {
	case EINPROGRESS:
	case EALREADY:
		break;
	default:
		rn_error_set(errno);
		return -1;
	}
	if (rn_socket_waitout(socket) != 0) {
		return -1;
	}
	size = sizeof(val);
	if (getsockopt(socket->node.fd, SOL_SOCKET, SO_ERROR, (void *) &val, &size) < 0) {
		rn_error_set(errno);
		return -1;
	}
	if (val != 0) {
		rn_error_set(val);
		return -1;
	}
	return 0;
}

/**
 * Binds the socket to the specified addr info.
 * This is a replacement of the bind(2) syscall in this library.
 *
 * @param socket Pointer to the socket to listen to
 * @param dst Address to bind
 * @param backlog Ignored
 *
 * @return 0 on success or -1 if an error occurs
 */
int rn_socket_class_udp_bind(rn_socket_t *socket, const rn_addr_t *dst, int unused(backlog))
{
	int enabled;

	enabled = 1;

#ifdef SO_REUSEPORT
	if (setsockopt(socket->node.fd, SOL_SOCKET, SO_REUSEPORT, &enabled, sizeof(enabled)) == -1) {
		return -1;
	}
#endif /* !SO_REUSEPORT */

	if (setsockopt(socket->node.fd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)) == -1 ||
			bind(socket->node.fd, &dst->sa, sizeof(*dst)) == -1) {
		return -1;
	}
	return 0;
}

