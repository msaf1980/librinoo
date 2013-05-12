/**
 * @file   socket.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Jan 25 00:28:29 2012
 *
 * @brief  Header file for socket function declarations.
 *
 *
 */

#ifndef RINOO_NET_SOCKET_H_
#define RINOO_NET_SOCKET_H_

#define MAX_IO_CALLS	10

typedef struct s_rinoosocket {
	int io_calls;
	t_rinoosched_node node;
	struct s_rinoosocket *parent;
	const t_rinoosocket_class *class;
} t_rinoosocket;

typedef in_addr_t t_ip;

int rinoo_socket_init(t_rinoosched *sched, t_rinoosocket *sock, const t_rinoosocket_class *class);
t_rinoosocket *rinoo_socket(t_rinoosched *sched, const t_rinoosocket_class *class);
void rinoo_socket_close(t_rinoosocket *socket);
void rinoo_socket_destroy(t_rinoosocket *socket);

int rinoo_socket_resume(t_rinoosocket *socket);
int rinoo_socket_release(t_rinoosocket *socket);
int rinoo_socket_waitin(t_rinoosocket *socket);
int rinoo_socket_waitout(t_rinoosocket *socket);
int rinoo_socket_waitio(t_rinoosocket *socket);
int rinoo_socket_timeout(t_rinoosocket *socket, uint32_t ms);

int rinoo_socket_connect(t_rinoosocket *socket, const struct sockaddr *addr, socklen_t addrlen);
int rinoo_socket_listen(t_rinoosocket *socket, const struct sockaddr *addr, socklen_t addrlen, int backlog);
t_rinoosocket *rinoo_socket_accept(t_rinoosocket *socket, struct sockaddr *addr, socklen_t *addrlen);
ssize_t rinoo_socket_read(t_rinoosocket *socket, void *buf, size_t count);
ssize_t	rinoo_socket_write(t_rinoosocket *socket, const void *buf, size_t count);
ssize_t rinoo_socket_readb(t_rinoosocket *socket, t_buffer *buffer);
ssize_t rinoo_socket_readline(t_rinoosocket *socket, t_buffer *buffer, const char *delim, size_t maxsize);
ssize_t rinoo_socket_expect(t_rinoosocket *socket, t_buffer *buffer, const char *expected);
ssize_t rinoo_socket_writeb(t_rinoosocket *socket, t_buffer *buffer);
ssize_t rinoo_socket_sendfile(t_rinoosocket *socket, int in_fd, off_t offset, size_t count);

#endif /* !RINOO_NET_SOCKET */
