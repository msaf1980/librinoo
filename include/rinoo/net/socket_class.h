/**
 * @file   socket_class.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Rinoo socket class
 *
 *
 */

#ifndef RINOO_NET_SOCKET_CLASS_H_
#define RINOO_NET_SOCKET_CLASS_H_

struct rn_socket_s;

typedef struct rn_socket_class_s {
	int domain;
	int type;
	struct rn_socket_s *(*create)(rn_sched_t *sched);
	void (*destroy)(struct rn_socket_s *socket);
	int (*open)(struct rn_socket_s *socket);
	struct rn_socket_s *(*dup)(rn_sched_t *destination, struct rn_socket_s *socket);
	int (*close)(struct rn_socket_s *socket);
	ssize_t (*read)(struct rn_socket_s *socket, void *buf, size_t count);
	ssize_t (*recvfrom)(struct rn_socket_s *socket, void *buf, size_t count, struct sockaddr *addrfrom, socklen_t *addrlen);
	ssize_t (*write)(struct rn_socket_s *socket, const void *buf, size_t count);
	ssize_t (*writev)(struct rn_socket_s *socket, rn_buffer_t **buffers, int count);
	ssize_t (*sendto)(struct rn_socket_s *socket, void *buf, size_t count, const struct sockaddr *addrto, socklen_t addrlen);
	ssize_t (*sendfile)(struct rn_socket_s *socket, int in_fd, off_t offset, size_t count);
	int (*connect)(struct rn_socket_s *socket, const struct sockaddr *addr, socklen_t addrlen);
	int (*bind)(struct rn_socket_s *socket, const struct sockaddr *addr, socklen_t addrlen, int backlog);
	struct rn_socket_s *(*accept)(struct rn_socket_s *socket, struct sockaddr *addr, socklen_t *addrlen);
} rn_socket_class_t;

#endif /* !RINOO_NET_SOCKET_CLASS_H_ */
