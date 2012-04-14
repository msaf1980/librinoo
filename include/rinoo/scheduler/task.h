/**
 * @file   task.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Tue Jul  5 17:21:44 2011
 *
 * @brief  Header file for tasks function declarations.
 *
 *
 */

#ifndef		RINOO_SCHEDULER_TASK_H_
# define	RINOO_SCHEDULER_TASK_H_

# define	RINOO_TASK_STACK_SIZE	(16 * 1024)

/* Defined in scheduler.h */
struct s_rinoosched;
/* Defined in this file */
struct s_rinootask;

typedef void (*t_rinootask_func)(struct s_rinootask *task);

typedef union u_rinootask_arg
{
	void	*ptr;
	int	args[2];
} t_rinootask_arg;

typedef struct s_rinootask
{
	struct s_rinoosched	*sched;
	struct timeval		tv;
	int			queued;
	ucontext_t		context;
	t_rinootask_func	function;
	t_rinoorbtree_node	proc_node;
	char			stack[RINOO_TASK_STACK_SIZE];
} t_rinootask;

typedef struct s_rinootask_driver
{
	t_rinootask	main;
	t_rinootask	*current;
	t_rinoorbtree	*proc_tree;
} t_rinootask_driver;

int rinoo_task_driver_init(struct s_rinoosched *sched);
void rinoo_task_driver_destroy(struct s_rinoosched *sched);
u32 rinoo_task_driver_run(struct s_rinoosched *sched);

int rinoo_task(struct s_rinoosched *sched, t_rinootask *task, t_rinootask_func function);
void rinoo_task_destroy(t_rinootask *task);
int rinoo_task_release(struct s_rinoosched *sched);
int rinoo_task_run(t_rinootask *task);
int rinoo_task_schedule(t_rinootask *task, struct timeval *tv);
int rinoo_task_unschedule(t_rinootask *task);

#endif		/* RINOO_SCHEDULER_TASK_H_ */
