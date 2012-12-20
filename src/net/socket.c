/**
 * @file   socket.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Dec 30 17:25:49 2009
 *
 * @brief  All functions needed to manage sockets.
 *
 *
 */

#include	"rinoo/rinoo.h"

/**
 * Generic socket creation.
 * This function should be used internally only.
 *
 * @param sched Pointer to a scheduler
 * @param fd Socket file descriptor
 *
 * @return 0 on success, otherwise -1
 */
int rinoo_socket_init(t_rinoosched *sched, t_rinoosocket *socket, int fd, void (*autodestroy)(t_rinoosocket *socket))
{
	int enabled;

	XASSERT(sched != NULL, -1);
	XASSERT(socket != NULL, -1);

	socket->fd = fd;
	socket->sched = sched;
	enabled = 1;
	if (unlikely(ioctl(socket->fd, FIONBIO, &enabled) == -1)) {
		return -1;
	}
	socket->autodestroy = autodestroy;
	return 0;
}

/**
 * Socket initialisation function. This function is similar to rinoo_socket,
 * except that it does not allocate memory for socket structure.
 *
 * @param sched Pointer to a scheduler
 * @param sock Pointer to a socket structure to fill
 * @param domain Socket domain (see man socket)
 * @param type Socket type (see man socket)
 * @param autodestroy Pointer to a function to destroy the socket (if NULL, the socket will be automatically closed)
 *
 * @return 0 on success, otherwise -1
 */
int rinoo_socket_set(t_rinoosched *sched, t_rinoosocket *sock, int domain, int type, void (*autodestroy)(t_rinoosocket *socket))
{
	int fd;

	XASSERT(sched != NULL, -1);
	XASSERT(sock != NULL, -1);

	fd = socket(domain, type, 0);
	if (unlikely(fd == -1)) {
		return -1;
	}
	if (autodestroy == NULL) {
		autodestroy = rinoo_socket_close;
	}
	if (unlikely(rinoo_socket_init(sched, sock, fd, autodestroy) != 0)) {
		close(fd);
		return -1;
	}
	return 0;

}

/**
 * Socket creation function. It is a replacement of the socket(2) syscall in this library.
 *
 * @param sched Pointer to a scheduler
 * @param domain Socket domain (see man socket)
 * @param type Socket type (see man socket)
 *
 * @return A pointer to the new socket or NULL if an error occurs
 */
t_rinoosocket *rinoo_socket(t_rinoosched *sched, int domain, int type)
{
	int fd;
	t_rinoosocket *new;

	XASSERT(sched != NULL, NULL);

	fd = socket(domain, type, 0);
	if (unlikely(fd == -1)) {
		return NULL;
	}
	new = calloc(1, sizeof(*new));
	if (unlikely(new == NULL)) {
		close(fd);
		return NULL;
	}
	if (unlikely(rinoo_socket_init(sched, new, fd, rinoo_socket_destroy) != 0)) {
		close(fd);
		free(new);
		return NULL;
	}
	return new;
}

/**
 * Close a socket. This function should be used when the socket structure
 * has been allocated by the user (by using rinoo_socket_set).
 *
 * @param socket Pointer to the socket to close
 */
void rinoo_socket_close(t_rinoosocket *socket)
{
	XASSERTN(socket != NULL);

	rinoo_sched_socket(socket, RINOO_SCHED_REMOVE, RINOO_MODE_NONE);
	close(socket->fd);
}

/**
 * Destroys a socket. This function is automatically called at the end
 * of the run() function.
 *
 * @param socket Pointer to the socket to destroy
 */
void rinoo_socket_destroy(t_rinoosocket *socket)
{
	XASSERTN(socket != NULL);

	rinoo_socket_close(socket);
	free(socket);
}

void rinoo_socket_error_set(t_rinoosocket *socket, int error)
{
	XASSERTN(socket != NULL);

	socket->error = error;
}

int rinoo_socket_error_get(t_rinoosocket *socket)
{
	XASSERT(socket != NULL, 0);

	return socket->error;
}

int rinoo_socket_schedule(t_rinoosocket *socket, u32 ms)
{
	struct timeval res;
	struct timeval toadd;

	XASSERT(socket != NULL, -1);

	toadd.tv_sec = ms / 1000;
	toadd.tv_usec = (ms % 1000) * 1000;
	timeradd(&socket->sched->clock, &toadd, &res);
	return rinoo_task_schedule(socket->sched->driver.current, &res);
}

int rinoo_socket_unschedule(t_rinoosocket *socket)
{
	XASSERT(socket != NULL, -1);

	return rinoo_task_unschedule(socket->task);
}

int rinoo_socket_timeout(t_rinoosocket *socket, u32 ms)
{
	return rinoo_socket_schedule(socket, ms);
}

/**
 * Releases the current socket execution
 *
 * @param sched Pointer to the scheduler currently used
 *
 * @return 0 on success or -1 if an error occurs
 */
int rinoo_socket_release(t_rinoosocket *socket)
{
	XASSERT(socket != NULL, -1);

	rinoo_socket_error_set(socket, ETIMEDOUT);
	return rinoo_task_release(socket->sched->driver.current);
}

/**
 * Releases socket execution and waits for the socket to be available for read operations.
 *
 * @param socket Pointer to the socket to wait for
 *
 * @return 0 on success or -1 if an error occurs (timeout is considered as error)
 */
int rinoo_socket_waitin(t_rinoosocket *socket)
{
	if (unlikely(rinoo_sched_socket(socket, RINOO_SCHED_ADD, RINOO_MODE_IN) != 0)) {
		return -1;
	}
	if (rinoo_socket_release(socket) != 0) {
		return -1;
	}
	if (socket->error != 0) {
		errno = socket->error;
		return -1;
	}
	return 0;
}

/**
 * Releases socket execution and waits for the socket to be available for write operations.
 *
 * @param socket Pointer to the socket to wait for
 *
 * @return 0 on success or -1 if an error occurs (timeout is considered as error)
 */
int rinoo_socket_waitout(t_rinoosocket *socket)
{
	if (unlikely(rinoo_sched_socket(socket, RINOO_SCHED_ADD, RINOO_MODE_OUT) != 0)) {
		return -1;
	}
	if (rinoo_socket_release(socket) != 0) {
		return -1;
	}
	if (socket->error != 0) {
		errno = socket->error;
		return -1;
	}
	return 0;
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
int rinoo_socket_connect(t_rinoosocket *socket, const struct sockaddr *addr, socklen_t addrlen)
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
 * Assigns the address specified to by addr to the socket.
 * This is a replacement to the bind(2) syscall in this library.
 *
 * @param socket Pointer to the socket to bind
 * @param addr Address used for binding (see man bind)
 * @param addrlen Sockaddr structure size (see man bind)
 *
 * @return 0 on success or -1 if an error occurs
 */
int rinoo_socket_bind(t_rinoosocket *socket, const struct sockaddr *addr, socklen_t addrlen)
{
	return bind(socket->fd, addr, addrlen);
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
int rinoo_socket_listen(t_rinoosocket *socket, const struct sockaddr *addr, socklen_t addrlen, int backlog)
{
	int enabled;

	enabled = 1;
	if (setsockopt(socket->fd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)) == -1 ||
	    rinoo_socket_bind(socket, addr, addrlen) == -1 ||
	    listen(socket->fd, backlog) == -1) {
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
t_rinoosocket *rinoo_socket_accept(t_rinoosocket *socket, struct sockaddr *addr, socklen_t *addrlen)
{
	int fd;
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
	if (unlikely(rinoo_socket_init(socket->task->sched, new, fd, rinoo_socket_destroy) != 0)) {
		close(fd);
		free(new);
		return NULL;
	}
	new->parent = socket;
	if (rinoo_sched_socket(new, RINOO_SCHED_ADD, RINOO_MODE_OUT) != 0) {
		rinoo_socket_destroy(new);
		return NULL;
	}
	return new;
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
ssize_t rinoo_socket_read(t_rinoosocket *socket, void *buf, size_t count)
{
	if (rinoo_socket_waitin(socket) != 0) {
		return -1;
	}
	return read(socket->fd, buf, count);
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
ssize_t	rinoo_socket_write(t_rinoosocket *socket, const void *buf, size_t count)
{
	if (rinoo_socket_waitout(socket) != 0) {
		return -1;
	}
	return write(socket->fd, buf, count);
}

/**
 * Socket read interface for t_buffer.
 * This function waits for and reads information available on the socket.
 * Adds the result to the t_buffer structure. It does extend the buffer if necessary.
 *
 * @param socket Pointer to the socket to read
 * @param buffer Buffer where to store the result
 *
 * @return The number of bytes read on success or -1 if an error occurs
 */
ssize_t rinoo_socket_readb(t_rinoosocket *socket, t_buffer *buffer)
{
	ssize_t res;

	if (buffer_isfull(buffer) && buffer_extend(buffer, 0) <= 0) {
		return -1;
	}
	if (rinoo_socket_waitin(socket) != 0) {
		return -1;
	}
	res = read(socket->fd,
		   buffer_ptr(buffer) + buffer_size(buffer),
		   buffer_msize(buffer) - buffer_size(buffer));
	buffer_setsize(buffer, buffer_size(buffer) + res);
	return res;
}

/**
 * Reads a line from a socket.
 * This function waits for and reads information available on a socket until
 * it finds a delimiter or the maximum line size speficied.
 * Adds the result to the t_buffer structure. It does extend the buffer if necessary.
 * The size returned is the size of the last line found. It is actually not the total
 * amount of bytes read. The function can implicitly store more data in buffer.
 * If the calling function aims to call sequentially rinoo_socket_readline
 * with the same buffer, it has to remove the last line from it by calling buffer_erase
 * with the returned size.
 *
 * @param socket Pointer to the socket to read
 * @param buffer Pointer to the buffer where to store data read
 * @param delim Pointer to the line delimiter
 * @param maxsize Maximum line size to be read
 *
 * @return The size of the last line found, or -1 if an error occurs.
 */
ssize_t rinoo_socket_readline(t_rinoosocket *socket, t_buffer *buffer, const char *delim, size_t maxsize)
{
	void *ptr;
	size_t dlen;
	ssize_t res;
	off_t offset;

	offset = 0;
	dlen = strlen(delim);
	while (buffer_size(buffer) < maxsize) {
		if (buffer_size(buffer) - offset >= dlen) {
			ptr = memmem(buffer_ptr(buffer) + offset, buffer_size(buffer) - offset, delim, dlen);
			if (ptr != NULL) {
				return (ptr - buffer_ptr(buffer) + dlen);
			}
			offset = buffer_size(buffer) - dlen;
		}
		if (buffer_isfull(buffer) && buffer_extend(buffer, 0) <= 0) {
			return -1;
		}
		if (rinoo_socket_waitin(socket) != 0) {
			return -1;
		}
		res = read(socket->fd,
			   buffer_ptr(buffer) + buffer_size(buffer),
			   buffer_msize(buffer) - buffer_size(buffer));
		buffer_setsize(buffer, buffer_size(buffer) + res);
	}
	return maxsize;
}

/**
 * This function reads socket input to find an expected string.
 *
 * @param socket Pointer to the socket to read
 * @param buffer Pointer to the buffer where to store data read
 * @param expected Expected string to be matched
 *
 * @return Buffer size if the string has been found, otherwise -1
 */
ssize_t rinoo_socket_expect(t_rinoosocket *socket, t_buffer *buffer, const char *expected)
{
	size_t len;

	len = strlen(expected);
	while (buffer_size(buffer) < len) {
		if (rinoo_socket_readb(socket, buffer) != 0) {
			return -1;
		}
	}
	if (buffer_strncmp(buffer, expected, len) == 0) {
		return buffer_size(buffer);
	}
	return -1;
}

/**
 * Socket write interface for t_buffer.
 * This function waits for the socket to be available for write operations and writes the buffer content into the socket.
 *
 * @param socket Pointer to the socket to write to
 * @param buffer Buffer which contains the information to write
 *
 * @return The number of bytes written on success or -1 if an error occurs
 */
ssize_t rinoo_socket_writeb(t_rinoosocket *socket, t_buffer *buffer)
{
	size_t len;
	size_t total;
	ssize_t res;

	total = 0;
	len = buffer_size(buffer);
	while (len > 0) {
		if (rinoo_socket_waitout(socket) != 0) {
			return -1;
		}
		res = write(socket->fd, buffer_ptr(buffer) + buffer_size(buffer) - len, len);
		if (res < 0) {
			return -1;
		}
		total += res;
		len -= res;
	}
	return total;
}
