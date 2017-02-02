/**
 * @file   node.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file for scheduler node structures.
 *
 *
 */

#ifndef RINOO_SCHEDULER_NODE_H_
#define RINOO_SCHEDULER_NODE_H_

/* Declared in scheduler.h */
struct rn_sched_s;

typedef enum rn_sched_mode_e {
	RINOO_MODE_NONE = 0,
	RINOO_MODE_IN = 1,
	RINOO_MODE_OUT = 2,
} rn_sched_mode_t;

typedef struct rn_sched_node_s {
	int fd;
	int error;
	rn_task_t *task;
	rn_list_node_t lnode;
	rn_sched_mode_t mode;
	rn_sched_mode_t waiting;
	rn_sched_mode_t received;
	struct rn_sched_s *sched;
} rn_sched_node_t;

#endif /* !RINOO_SCHEDULER_NODE_H_ */
