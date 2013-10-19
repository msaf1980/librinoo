/**
 * @file   node.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2013
 * @date   Mon Dec 28 00:14:20 2009
 *
 * @brief  Header file for scheduler node structures.
 *
 *
 */

#ifndef RINOO_SCHEDULER_NODE_H_
#define RINOO_SCHEDULER_NODE_H_

/* Declared in scheduler.h */
struct s_rinoosched;

typedef enum e_rinoosched_mode {
	RINOO_MODE_NONE = 0,
	RINOO_MODE_IN = 1,
	RINOO_MODE_OUT = 2,
} t_rinoosched_mode;

typedef struct s_rinoosched_node {
	int fd;
	int error;
	t_rinootask *task;
	t_rinoolist_node lnode;
	t_rinoosched_mode waiting;
	t_rinoosched_mode received;
	struct s_rinoosched *sched;
} t_rinoosched_node;

#endif /* !RINOO_SCHEDULER_NODE_H_ */
