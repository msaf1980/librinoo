/**
 * @file   socket_class_tcp.h
 * @author Reginald Lips <reginald.@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  TCP socket class
 *
 *
 */

#ifndef RINOO_NET_SOCKET_CLASS_TCP_H_
#define RINOO_NET_SOCKET_CLASS_TCP_H_

rn_socket_t *rn_socket_class_tcp_create(rn_sched_t *sched);
void rn_socket_class_tcp_destroy(rn_socket_t *socket);
int rn_socket_class_tcp_open(rn_socket_t *sock);
rn_socket_t *rn_socket_class_tcp_dup(rn_sched_t *destination, rn_socket_t *socket);
int rn_socket_class_tcp_close(rn_socket_t *socket);
ssize_t rn_socket_class_tcp_read(rn_socket_t *socket, void *buf, size_t count);
ssize_t rn_socket_class_tcp_recvfrom(rn_socket_t *socket, void *buf, size_t count, struct sockaddr *addrfrom, socklen_t *addrlen);
ssize_t rn_socket_class_tcp_write(rn_socket_t *socket, const void *buf, size_t count);
ssize_t rn_socket_class_tcp_writev(rn_socket_t *socket, rn_buffer_t **buffers, int count);
ssize_t rn_socket_class_tcp_sendto(rn_socket_t *socket, void *buf, size_t count, const struct sockaddr *addrto, socklen_t addrlen);
ssize_t rn_socket_class_tcp_sendfile(rn_socket_t *socket, int in_fd, off_t offset, size_t count);
int rn_socket_class_tcp_connect(rn_socket_t *socket, const struct sockaddr *addr, socklen_t addrlen);
int rn_socket_class_tcp_bind(rn_socket_t *socket, const struct sockaddr *addr, socklen_t addrlen, int backlog);
rn_socket_t *rn_socket_class_tcp_accept(rn_socket_t *socket, struct sockaddr *addr, socklen_t *addrlen);

#endif /* !RINOO_NET_SOCKET_CLASS_TCP_H_ */
