/**
 * @file   epoll.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Tue Mar 20 15:44:45 2012
 *
 * @brief  Header file for epoll function declarations.
 *
 *
 */

#ifndef RINOO_EPOLL_H_
#define RINOO_EPOLL_H_

#define RINOO_EPOLL_MAX_EVENTS	128

#include <sys/epoll.h>

struct s_rinoosched;		/* Defined in scheduler.h */
struct s_rinoosched_node;	/* Defined in scheduler.h */
enum e_rinoosched_mode;		/* Defined in scheduler.h */

typedef struct s_rinooepoll {
	int fd;
	int curevent;
	sigset_t sigmask;
	struct epoll_event events[RINOO_EPOLL_MAX_EVENTS];
} t_rinooepoll;

int rinoo_epoll_init(struct s_rinoosched *sched);
void rinoo_epoll_destroy(struct s_rinoosched *sched);
int rinoo_epoll_insert(struct s_rinoosched_node *node, enum e_rinoosched_mode mode);
int rinoo_epoll_addmode(struct s_rinoosched_node *node, enum e_rinoosched_mode mode);
int rinoo_epoll_remove(struct s_rinoosched_node *node);
int rinoo_epoll_poll(struct s_rinoosched *sched, uint32_t timeout);

#endif /* !RINOO_RINOO_EPOLL_H_ */
