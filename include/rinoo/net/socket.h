/**
 * @file   socket.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file for socket function declarations.
 *
 *
 */

#ifndef RINOO_NET_SOCKET_H_
#define RINOO_NET_SOCKET_H_

#define MAX_IO_CALLS	10

typedef struct rn_socket_s {
	int io_calls;
	rn_sched_node_t node;
	struct rn_socket_s *parent;
	const rn_socket_class_t *class;
} rn_socket_t;

typedef union rn_ip_u {
	struct sockaddr_in v4;
	struct sockaddr_in6 v6;
} rn_ip_t;

#define IP_ANY		(NULL)
#define IP_LOOPBACK	(NULL)
#define IS_IPV4(ip)	(ip->v4.sin_family == AF_INET)
#define IS_IPV6(ip)	(ip->v6.sin6_family == AF_INET6)

int rn_socket_init(rn_sched_t *sched, rn_socket_t *sock, const rn_socket_class_t *class);
rn_socket_t *rn_socket(rn_sched_t *sched, const rn_socket_class_t *class);
rn_socket_t *rn_socket_dup(rn_sched_t *destination, rn_socket_t *socket);
void rn_socket_close(rn_socket_t *socket);
void rn_socket_destroy(rn_socket_t *socket);

int rn_socket_resume(rn_socket_t *socket);
int rn_socket_release(rn_socket_t *socket);
int rn_socket_waitin(rn_socket_t *socket);
int rn_socket_waitout(rn_socket_t *socket);
int rn_socket_waitio(rn_socket_t *socket);
int rn_socket_timeout(rn_socket_t *socket, uint32_t ms);

int rn_socket_connect(rn_socket_t *socket, const struct sockaddr *addr, socklen_t addrlen);
int rn_socket_bind(rn_socket_t *socket, const struct sockaddr *addr, socklen_t addrlen, int backlog);
rn_socket_t *rn_socket_accept(rn_socket_t *socket, struct sockaddr *addr, socklen_t *addrlen);
ssize_t rn_socket_read(rn_socket_t *socket, void *buf, size_t count);
ssize_t rn_socket_recvfrom(rn_socket_t *socket, void *buf, size_t count, struct sockaddr *addrfrom, socklen_t *addrlen);
ssize_t rn_socket_write(rn_socket_t *socket, const void *buf, size_t count);
ssize_t rn_socket_writev(rn_socket_t *socket, rn_buffer_t **buffers, int count);
ssize_t rn_socket_sendto(rn_socket_t *socket, void *buf, size_t count, const struct sockaddr *addrto, socklen_t addrlen);
ssize_t rn_socket_readb(rn_socket_t *socket, rn_buffer_t *buffer);
ssize_t rn_socket_readline(rn_socket_t *socket, rn_buffer_t *buffer, const char *delim, size_t maxsize);
ssize_t rn_socket_expect(rn_socket_t *socket, rn_buffer_t *buffer, const char *expected);
ssize_t rn_socket_writeb(rn_socket_t *socket, rn_buffer_t *buffer);
ssize_t rn_socket_sendfile(rn_socket_t *socket, int in_fd, off_t offset, size_t count);

#endif /* !RINOO_NET_SOCKET_H_ */
