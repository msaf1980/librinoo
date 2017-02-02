/**
 * @file   epoll.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file for epoll function declarations.
 *
 *
 */

#ifndef RINOO_EPOLL_H_
#define RINOO_EPOLL_H_

#define RN_EPOLL_MAX_EVENTS	128

#include <sys/epoll.h>

struct rn_sched_s;		/* Defined in scheduler.h */
struct rn_sched_node_s;	/* Defined in scheduler.h */
enum rn_sched_mode_e;		/* Defined in scheduler.h */

typedef struct rn_epoll_s {
	int fd;
	int curevent;
	struct epoll_event events[RN_EPOLL_MAX_EVENTS];
} rn_epoll_t;

int rn_epoll_init(struct rn_sched_s *sched);
void rn_epoll_destroy(struct rn_sched_s *sched);
int rn_epoll_insert(struct rn_sched_node_s *node, enum rn_sched_mode_e mode);
int rn_epoll_addmode(struct rn_sched_node_s *node, enum rn_sched_mode_e mode);
int rn_epoll_remove(struct rn_sched_node_s *node);
int rn_epoll_poll(struct rn_sched_s *sched, int timeout);

#endif /* !RINOO_RINOO_EPOLL_H_ */
