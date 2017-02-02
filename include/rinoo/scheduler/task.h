/**
 * @file   task.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file for tasks function declarations.
 *
 *
 */

#ifndef RINOO_SCHEDULER_TASK_H_
#define RINOO_SCHEDULER_TASK_H_

#define RN_TASK_STACK_SIZE	(16 * 1024)

/* Defined in scheduler.h */
struct rn_sched_s;

typedef struct rn_task_s {
	bool scheduled;
	struct timeval tv;
	struct rn_sched_s *sched;
	rn_rbtree_node_t proc_node;
	rn_fcontext_t context;
	char stack[RN_TASK_STACK_SIZE];

#ifdef RINOO_DEBUG
	int valgrind_stackid;
#endif /* !RINOO_DEBUG */
} rn_task_t;

typedef struct rn_task_driver_s {
	rn_task_t main;
	rn_task_t *current;
	rn_rbtree_t proc_tree;
} rn_task_driver_t;

int rn_task_driver_init(struct rn_sched_s *sched);
void rn_task_driver_destroy(struct rn_sched_s *sched);
int rn_task_driver_run(struct rn_sched_s *sched);
int rn_task_driver_stop(struct rn_sched_s *sched);
uint32_t rn_task_driver_nbpending(struct rn_sched_s *sched);
rn_task_t *rn_task_driver_getcurrent(struct rn_sched_s *sched);

rn_task_t *rn_task(struct rn_sched_s *sched, rn_task_t *parent, void (*function)(void *arg), void *arg);
void rn_task_destroy(rn_task_t *task);
int rn_task_start(struct rn_sched_s *sched, void (*function)(void *arg), void *arg);
int rn_task_run(struct rn_sched_s *sched, void (*function)(void *arg), void *arg);
int rn_task_resume(rn_task_t *task);
int rn_task_release(struct rn_sched_s *sched);
int rn_task_schedule(rn_task_t *task, struct timeval *tv);
int rn_task_unschedule(rn_task_t *task);
int rn_task_start(struct rn_sched_s *sched, void (*function)(void *arg), void *arg);
int rn_task_wait(struct rn_sched_s *sched, uint32_t ms);
int rn_task_pause(struct rn_sched_s *sched);
rn_task_t *rn_task_self(void);

#endif /* RINOO_SCHEDULER_TASK_H_ */
