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

rn_sched_t *rn_scheduler(void);
void rn_scheduler_destroy(rn_sched_t *sched);
int rn_scheduler_spawn(rn_sched_t *sched, int count);
rn_sched_t *rn_scheduler_spawn_get(rn_sched_t *sched, int id);
rn_sched_t *rn_scheduler_self(void);
void rn_scheduler_stop(rn_sched_t *sched);
int rn_scheduler_waitfor(rn_sched_node_t *node,  rn_sched_mode_t mode);
int rn_scheduler_remove(rn_sched_node_t *node);
void rn_scheduler_wakeup(rn_sched_node_t *node, rn_sched_mode_t mode, int error);
int rn_scheduler_poll(rn_sched_t *sched);
void rn_scheduler_loop(rn_sched_t *sched);

#endif /* !RINOO_SCHEDULER_SCHEDULER_H */
