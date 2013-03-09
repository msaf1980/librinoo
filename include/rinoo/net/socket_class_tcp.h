/**
 * @file   socket_class_tcp.h
 * @author reginaldl <reginald.@gmail.com> - Copyright 2013
 * @date   Fri Mar  8 22:03:51 2013
 *
 * @brief  TCP socket class
 *
 *
 */

#ifndef RINOO_NET_SOCKET_CLASS_TCP_H_
#define RINOO_NET_SOCKET_CLASS_TCP_H_

t_rinoosocket *rinoo_socket_class_tcp_create(t_rinoosched *sched);
void rinoo_socket_class_tcp_destroy(t_rinoosocket *socket);
int rinoo_socket_class_tcp_open(t_rinoosocket *sock);
int rinoo_socket_class_tcp_close(t_rinoosocket *socket);
ssize_t rinoo_socket_class_tcp_read(t_rinoosocket *socket, void *buf, size_t count);
ssize_t	rinoo_socket_class_tcp_write(t_rinoosocket *socket, const void *buf, size_t count);
int rinoo_socket_class_tcp_connect(t_rinoosocket *socket, const struct sockaddr *addr, socklen_t addrlen);
int rinoo_socket_class_tcp_listen(t_rinoosocket *socket, const struct sockaddr *addr, socklen_t addrlen, int backlog);
t_rinoosocket *rinoo_socket_class_tcp_accept(t_rinoosocket *socket, struct sockaddr *addr, socklen_t *addrlen);

#endif /* !RINOO_NET_SOCKET_CLASS_TCP_H_ */
