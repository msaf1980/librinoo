/**
 * @file   socket_class.h
 * @author reginaldl <reginald.l@gmail.com> - Copyright 2013
 * @date   Fri Mar  8 15:25:01 2013
 *
 * @brief  Rinoo socket class
 *
 *
 */

#ifndef RINOO_NET_SOCKET_CLASS_H_
#define RINOO_NET_SOCKET_CLASS_H_

struct s_rinoosocket;

typedef struct s_rinoosocket_class {
	int domain;
	int type;
	struct s_rinoosocket *(*create)(t_rinoosched *sched);
	void (*destroy)(struct s_rinoosocket *socket);
	int (*open)(struct s_rinoosocket *socket);
	struct s_rinoosocket *(*dup)(t_rinoosched *destination, struct s_rinoosocket *socket);
	int (*close)(struct s_rinoosocket *socket);
	ssize_t (*read)(struct s_rinoosocket *socket, void *buf, size_t count);
	ssize_t (*write)(struct s_rinoosocket *socket, const void *buf, size_t count);
	ssize_t (*sendfile)(struct s_rinoosocket *socket, int in_fd, off_t offset, size_t count);
	int (*connect)(struct s_rinoosocket *socket, const struct sockaddr *addr, socklen_t addrlen);
	int (*listen)(struct s_rinoosocket *socket, const struct sockaddr *addr, socklen_t addrlen, int backlog);
	struct s_rinoosocket *(*accept)(struct s_rinoosocket *socket, struct sockaddr *addr, socklen_t *addrlen);
} t_rinoosocket_class;

#endif /* !RINOO_NET_SOCKET_CLASS_H_ */
