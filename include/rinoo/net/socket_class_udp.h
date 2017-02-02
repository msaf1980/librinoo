/**
 * @file   socket_class_udp.h
 * @author Reginald Lips <reginald.@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  UDP socket class
 *
 *
 */

#ifndef RINOO_NET_SOCKET_CLASS_UDP_H_
#define RINOO_NET_SOCKET_CLASS_UDP_H_

rn_socket_t *rn_socket_class_udp_create(rn_sched_t *sched);
void rn_socket_class_udp_destroy(rn_socket_t *socket);
int rn_socket_class_udp_open(rn_socket_t *sock);
rn_socket_t *rn_socket_class_udp_dup(rn_sched_t *destination, rn_socket_t *socket);
int rn_socket_class_udp_close(rn_socket_t *socket);
ssize_t rn_socket_class_udp_read(rn_socket_t *socket, void *buf, size_t count);
ssize_t rn_socket_class_udp_recvfrom(rn_socket_t *socket, void *buf, size_t count, struct sockaddr *addrfrom, socklen_t *addrlen);
ssize_t rn_socket_class_udp_write(rn_socket_t *socket, const void *buf, size_t count);
ssize_t rn_socket_class_udp_writev(rn_socket_t *socket, rn_buffer_t **buffers, int count);
ssize_t rn_socket_class_udp_sendto(rn_socket_t *socket, void *buf, size_t count, const struct sockaddr *addrto, socklen_t addrlen);
ssize_t rn_socket_class_udp_sendfile(rn_socket_t *socket, int in_fd, off_t offset, size_t count);
int rn_socket_class_udp_connect(rn_socket_t *socket, const struct sockaddr *addr, socklen_t addrlen);
int rn_socket_class_udp_bind(rn_socket_t *socket, const struct sockaddr *addr, socklen_t addrlen, int backlog);

#endif /* !RINOO_NET_SOCKET_CLASS_UDP_H_ */
