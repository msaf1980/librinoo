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
	RN_MODE_NONE = 0,
	RN_MODE_IN = 1,
	RN_MODE_OUT = 2,
} rn_sched_mode_t;

typedef struct rn_sched_node_s {
	int fd;
	int error;
	rn_task_t *task;
	unsigned char modes;
	rn_list_node_t lnode;
	struct rn_sched_s *sched;
} rn_sched_node_t;

/*
 * unsigned char events
 * |                    8 bits                     |
 * |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 * |  UNUSED   |    RECV   |   WAIT    | REGISTERED|
 *
 * `modes` holds the state of each polling mode in a scheduler node.
 * A mode must first registered (in other words, added to epoll).
 * Then, when a task waits for an event on a scheduler node,
 * RiNOO sets `WAIT` field with this mode. When RiNOO receives
 * this event, it sets `RECV`. Finally, when the corresponding task resumes,
 * it will reset both `RECV` and `WAIT`.
 */
#define rn_mode(node, x)			(((node)->modes & (x)) == (x))
#define rn_mode_set(node, x)			do { (node)->modes |= (x); } while (0)
#define rn_mode_unset(node, x)			do { (node)->modes &= ~(x); } while (0)

#define rn_mode_registered(node, x)		rn_mode(node, x)
#define rn_mode_registered_get(node)		((node)->modes & 0x03)
#define rn_mode_registered_set(node, x)		rn_mode_set(node, x)
#define rn_mode_registered_unset(node, x)	rn_mode_unset(node, x)

#define rn_mode_waiting(node, x)		rn_mode(node, (x) << 2)
#define rn_mode_waiting_set(node, x)		rn_mode_set(node, (x) << 2)
#define rn_mode_waiting_unset(node, x)		rn_mode_unset(node, (x) << 2)

#define rn_mode_received(node, x)		rn_mode(node, (x) << 4)
#define rn_mode_received_set(node, x)		rn_mode_set(node, (x) << 4)
#define rn_mode_received_unset(node, x)		rn_mode_unset(node, (x) << 4)

#endif /* !RINOO_SCHEDULER_NODE_H_ */
