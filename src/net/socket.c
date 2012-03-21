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

static void rinoo_socket_run(t_rinoosched *sched, void *arg);

/**
 * Generic socket creation.
 * This function should be used internally only.
 *
 * @param sched Pointer to a scheduler
 * @param fd Socket file descriptor
 * @param run Pointer to the function which will run the socket
 *
 * @return A pointer to the new socket or NULL if an error occurs
 */
static t_rinoosocket *rinoo_socket_init(t_rinoosched *sched, int fd, t_rinoosocket_run run_func)
{
	int enabled;
	t_rinoosocket *sock;

	XASSERT(sched != NULL, NULL);
	XASSERT(run_func != NULL, NULL);

	sock = calloc(1, sizeof(*sock));
	sock->fd = fd;
	sock->sched = sched;
	sock->run = run_func;
	enabled = 1;
	if (unlikely(ioctl(sock->fd, FIONBIO, &enabled) == -1)) {
		free(sock);
		return NULL;
	}
	sock->task = rinoo_task(sched, rinoo_socket_run, sock);
	if (unlikely(sock->task == NULL)) {
		free(sock);
		return NULL;
	}
	return sock;
}

/**
 * Socket creation function. It is a replacement of the socket(2) syscall in this library.
 *
 * @param sched Pointer to a scheduler
 * @param domain Socket domain (see man socket)
 * @param type Socket type (see man socket)
 * @param run Pointer to the function which will run the socket
 *
 * @return A pointer to the new socket or NULL if an error occurs
 */
t_rinoosocket *rinoo_socket(t_rinoosched *sched, int domain, int type, t_rinoosocket_run run_func)
{
	int fd;
	t_rinoosocket *new;

	XASSERT(sched != NULL, NULL);
	XASSERT(run_func != NULL, NULL);

	fd = socket(domain, type, 0);
	if (unlikely(fd == -1)) {
		return NULL;
	}
	new = rinoo_socket_init(sched, fd, run_func);
	if (unlikely(new == NULL)) {
		close(fd);
		return NULL;
	}
	return new;
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

	if (socket->run != NULL) {
		rinoo_task_destroy(socket->task);
	}
	rinoo_sched_socket(socket, RINOO_SCHED_REMOVE, RINOO_MODE_NONE);
	close(socket->fd);
	free(socket);
}

int rinoo_socket_schedule(t_rinoosocket *socket, u32 ms)
{
	struct timeval res;
	struct timeval toadd;

	XASSERT(socket != NULL, -1);
	XASSERT(socket->sched != NULL, -1);
	XASSERT(socket->task != NULL, -1);

	if (ms == 0) {
		return rinoo_task_schedule(socket->task, NULL);
	}
	toadd.tv_sec = ms / 1000;
	toadd.tv_usec = (ms % 1000) * 1000;
	timeradd(&socket->sched->clock, &toadd, &res);
	return rinoo_task_schedule(socket->task, &res);
}

/**
 * Main socket run function. It is called at socket treatment startup.
 * This function should be called internally only by rinoo_task_run.
 *
 * @param sched Pointer to the scheduler dealing with this task
 * @param arg Pointer to the socket to run
 */
static void rinoo_socket_run(t_rinoosched *sched, void *arg)
{
	t_rinoosocket *socket;

	XASSERTN(sched != NULL);
	XASSERTN(arg != NULL);

	socket = arg;
	socket->run(socket);
	/* That means we can destroy the socket */
	socket->run = NULL;
}

/**
 * Resumes socket execution by switching to the socket context.
 *
 * @param socket Pointer to the socket to resume
 *
 * @return 0 on sucess or -1 if an error occurs
 */
int rinoo_socket_resume(t_rinoosocket *socket)
{
	int ret;

	ret = rinoo_task_run(socket->task);
	if (ret == 0 && socket->run == NULL) {
		rinoo_socket_destroy(socket);
	}
	return ret;
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
	XASSERT(socket->sched->task_driver->cur_task == socket->task, -1);

	return rinoo_task_release(socket->sched);
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
	return rinoo_socket_release(socket);
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
	return rinoo_socket_release(socket);
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
	socklen_t size;

	XASSERT(socket != NULL, -1);

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
 * @param run_func Pointer to the function which will run the socket
 *
 * @return A pointer to the new client socket or NULL if an error occurs
 */
t_rinoosocket *rinoo_socket_accept(t_rinoosocket *socket, struct sockaddr *addr, socklen_t *addrlen, t_rinoosocket_run run_func)
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
	new = rinoo_socket_init(socket->sched, fd, run_func);
	if (new == NULL) {
		close(fd);
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
 * Adds the result to the t_buffer structure. It does extend the buffer is necessary.
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
		   buffer_ptr(buffer) + buffer_len(buffer),
		   buffer_size(buffer) - buffer_len(buffer));
	buffer_setlen(buffer, buffer_len(buffer) + res);
	return res;
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
	len = buffer_len(buffer);
	while (len > 0) {
		if (rinoo_socket_waitout(socket) != 0) {
			return -1;
		}
		res = write(socket->fd, buffer_ptr(buffer) + buffer_len(buffer) - len, len);
		if (res < 0) {
			return -1;
		}
		total += res;
		len -= res;
	}
	return total;
}
