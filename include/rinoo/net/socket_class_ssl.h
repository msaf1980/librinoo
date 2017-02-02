/**
 * @file   socket_class_ssl.h
 * @author Reginald Lips <reginald.@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Secure socket class
 *
 *
 */

#ifndef RINOO_NET_SOCKET_CLASS_SSL_H_
#define RINOO_NET_SOCKET_CLASS_SSL_H_

rn_socket_t *rn_socket_class_ssl_create(rn_sched_t *sched);
void rn_socket_class_ssl_destroy(rn_socket_t *socket);
ssize_t rn_socket_class_ssl_read(rn_socket_t *socket, void *buf, size_t count);
ssize_t	rn_socket_class_ssl_write(rn_socket_t *socket, const void *buf, size_t count);
int rn_socket_class_ssl_connect(rn_socket_t *socket, const struct sockaddr *addr, socklen_t addrlen);
rn_socket_t *rn_socket_class_ssl_accept(rn_socket_t *socket, struct sockaddr *addr, socklen_t *addrlen);

#endif /* !RINOO_NET_SOCKET_CLASS_SSL_H_ */
