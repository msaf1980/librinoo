/**
 * @file   socket.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Wed Jan 25 00:28:29 2012
 *
 * @brief  Header file for socket function declarations.
 *
 *
 */

#ifndef		RINOO_NET_SOCKET_H_
# define	RINOO_NET_SOCKET_H_

struct s_rinoosocket;

typedef void	(*t_rinoosocket_run)(struct s_rinoosocket *socket);

typedef struct s_rinoosocket
{
	int			fd;
	int			error;
	t_rinootask		task;
	t_rinoosocket_run	run;
	struct s_rinoosocket	*parent;
} t_rinoosocket;

typedef in_addr_t       t_ip;

t_rinoosocket *rinoo_socket(t_rinoosched *sched, int domain, int type, t_rinoosocket_run run_func);
void rinoo_socket_destroy(t_rinoosocket *socket);

void rinoo_socket_error_set(t_rinoosocket *socket, int error);
int rinoo_socket_error_get(t_rinoosocket *socket);
int rinoo_socket_schedule(t_rinoosocket *socket, u32 ms);
int rinoo_socket_resume(t_rinoosocket *socket);
int rinoo_socket_release(t_rinoosocket *socket);
int rinoo_socket_waitin(t_rinoosocket *socket);
int rinoo_socket_waitout(t_rinoosocket *socket);

int rinoo_socket_connect(t_rinoosocket *socket, const struct sockaddr *addr, socklen_t addrlen);
int rinoo_socket_bind(t_rinoosocket *socket, const struct sockaddr *addr, socklen_t addrlen);
int rinoo_socket_listen(t_rinoosocket *socket, const struct sockaddr *addr, socklen_t addrlen, int backlog);
t_rinoosocket *rinoo_socket_accept(t_rinoosocket *socket, struct sockaddr *addr, socklen_t *addrlen, t_rinoosocket_run run_func);
ssize_t rinoo_socket_read(t_rinoosocket *socket, void *buf, size_t count);
ssize_t	rinoo_socket_write(t_rinoosocket *socket, const void *buf, size_t count);
ssize_t rinoo_socket_readb(t_rinoosocket *socket, t_buffer *buffer);
ssize_t rinoo_socket_writeb(t_rinoosocket *socket, t_buffer *buffer);

#endif		/* !RINOO_NET_SOCKET */
