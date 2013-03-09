/**
 * @file   socket_class.h
 * @author reginaldl <reginald.l@gmail.com>
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
	ssize_t (*read)(struct s_rinoosocket *socket, void *buf, size_t count);
	ssize_t (*write)(struct s_rinoosocket *socket, void *buf, size_t count);
	int (*connect)(struct s_rinoosocket *socket, const struct sockaddr *addr, socklen_t addrlen);
	struct s_rinoosocket (*accept)(struct s_rinoosocket *socket, struct sockaddr *addr, socklen_t *addrlen);
} t_rinoosocket_class;

#endif /* !RINOO_NET_SOCKET_CLASS_H_ */
