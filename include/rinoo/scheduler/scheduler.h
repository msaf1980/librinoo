/**
 * @file   scheduler.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file for scheduler function declarations.
 *
 *
 */

#ifndef RINOO_SCHEDULER_SCHEDULER_H_
#define RINOO_SCHEDULER_SCHEDULER_H_

typedef struct rn_sched_s {
	int id;
	bool stop;
	rn_list_t nodes;
	uint32_t nbpending;
	struct timeval clock;
	rn_task_driver_t driver;
	struct rn_epoll_s epoll;
	rn_sched_spawns_t spawns;
} rn_sched_t;

rn_sched_t *rn_sched(void);
void rn_sched_destroy(rn_sched_t *sched);
int rn_sched_spawn(rn_sched_t *sched, int count);
rn_sched_t *rn_sched_spawn_get(rn_sched_t *sched, int id);
rn_sched_t *rn_sched_self(void);
void rn_sched_stop(rn_sched_t *sched);
int rn_sched_waitfor(rn_sched_node_t *node,  rn_sched_mode_t mode);
int rn_sched_remove(rn_sched_node_t *node);
void rn_sched_wakeup(rn_sched_node_t *node, rn_sched_mode_t mode, int error);
int rn_sched_poll(rn_sched_t *sched);
void rn_sched_loop(rn_sched_t *sched);

#endif /* !RINOO_SCHEDULER_SCHEDULER_H */
