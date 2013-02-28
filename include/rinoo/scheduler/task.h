/**
 * @file   task.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Tue Jul  5 17:21:44 2011
 *
 * @brief  Header file for tasks function declarations.
 *
 *
 */

#ifndef RINOO_SCHEDULER_TASK_H_
#define RINOO_SCHEDULER_TASK_H_

#define RINOO_TASK_STACK_SIZE	(16 * 1024)

/* Defined in scheduler.h */
struct s_rinoosched;

typedef struct s_rinootask {
 	bool scheduled;
	struct timeval tv;
	struct s_rinoosched *sched;
	t_rinoorbtree_node proc_node;
	t_fcontext context;
	char stack[RINOO_TASK_STACK_SIZE];
} t_rinootask;

typedef struct s_rinootask_driver {
	t_rinootask main;
	t_rinootask *current;
	t_rinoorbtree proc_tree;
} t_rinootask_driver;

int rinoo_task_driver_init(struct s_rinoosched *sched);
void rinoo_task_driver_destroy(struct s_rinoosched *sched);
u32 rinoo_task_driver_run(struct s_rinoosched *sched);
u32 rinoo_task_driver_nbpending(struct s_rinoosched *sched);
t_rinootask *rinoo_task_driver_getcurrent(struct s_rinoosched *sched);

t_rinootask *rinoo_task(struct s_rinoosched *sched, t_rinootask *parent, void (*function)(void *arg), void *arg);
void rinoo_task_destroy(t_rinootask *task);
int rinoo_task_start(struct s_rinoosched *sched, void (*function)(void *arg), void *arg);
int rinoo_task_run(struct s_rinoosched *sched, void (*function)(void *arg), void *arg);
int rinoo_task_resume(t_rinootask *task);
void rinoo_task_release(struct s_rinoosched *sched);
int rinoo_task_schedule(t_rinootask *task, struct timeval *tv);
int rinoo_task_unschedule(t_rinootask *task);
int rinoo_task_start(struct s_rinoosched *sched, void (*function)(void *arg), void *arg);
int rinoo_task_wait(struct s_rinoosched *sched, u32 ms);

#endif /* RINOO_SCHEDULER_TASK_H_ */
