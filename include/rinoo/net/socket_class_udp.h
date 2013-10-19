/**
 * @file   socket_class_udp.h
 * @author reginaldl <reginald.@gmail.com> - Copyright 2013
 * @date   Fri Mar  8 22:03:51 2013
 *
 * @brief  UDP socket class
 *
 *
 */

#ifndef RINOO_NET_SOCKET_CLASS_UDP_H_
#define RINOO_NET_SOCKET_CLASS_UDP_H_

t_rinoosocket *rinoo_socket_class_udp_create(t_rinoosched *sched);
void rinoo_socket_class_udp_destroy(t_rinoosocket *socket);
int rinoo_socket_class_udp_open(t_rinoosocket *sock);
t_rinoosocket *rinoo_socket_class_udp_dup(t_rinoosched *destination, t_rinoosocket *socket);
int rinoo_socket_class_udp_close(t_rinoosocket *socket);
ssize_t rinoo_socket_class_udp_read(t_rinoosocket *socket, void *buf, size_t count);
ssize_t rinoo_socket_class_udp_recvfrom(t_rinoosocket *socket, void *buf, size_t count, struct sockaddr *addrfrom, socklen_t *addrlen);
ssize_t	rinoo_socket_class_udp_write(t_rinoosocket *socket, const void *buf, size_t count);
ssize_t rinoo_socket_class_udp_sendto(t_rinoosocket *socket, void *buf, size_t count, const struct sockaddr *addrto, socklen_t addrlen);
ssize_t rinoo_socket_class_udp_sendfile(t_rinoosocket *socket, int in_fd, off_t offset, size_t count);
int rinoo_socket_class_udp_connect(t_rinoosocket *socket, const struct sockaddr *addr, socklen_t addrlen);
int rinoo_socket_class_udp_bind(t_rinoosocket *socket, const struct sockaddr *addr, socklen_t addrlen, int backlog);

#endif /* !RINOO_NET_SOCKET_CLASS_UDP_H_ */
