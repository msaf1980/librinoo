/**
 * @file   task.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Tue Jul  5 17:21:44 2011
 *
 * @brief  Header file for tasks function declarations.
 *
 *
 */

#ifndef		RINOO_SCHEDULER_TASK_H_
# define	RINOO_SCHEDULER_TASK_H_

# define	RINOO_TASK_STACK_SIZE	(16 * 1024)

typedef void (*t_rinootask_func)(t_rinoosched *sched, void *arg);

typedef union u_rinootask_arg
{
	void	*ptr;
	int	args[2];
} t_rinootask_arg;

typedef struct s_rinootask
{
	t_rinoosched		*sched;
	struct timeval		tv;
	void			*arg;
	t_rinootask_func	func;
	ucontext_t		context;
	char			stack[RINOO_TASK_STACK_SIZE];
#ifdef RINOO_DEBUG
	int			valgrind_id;
#endif /* !RINOO_DEBUG */
} t_rinootask;

typedef struct s_rinootask_driver
{
	t_rinootask	main_task;
	t_rinootask	*cur_task;
	t_rinooskip	*task_list;
} t_rinootask_driver;

int rinoo_task_driver_init(t_rinoosched *sched);
void rinoo_task_driver_destroy(t_rinoosched *sched);
u32 rinoo_task_driver_run(t_rinoosched *sched);
t_rinootask *rinoo_task(t_rinoosched *sched, t_rinootask_func task_func, void *arg);
int rinoo_task_schedule(t_rinootask *task, struct timeval *tv);
void rinoo_task_destroy(t_rinootask *task);
int rinoo_task_release(t_rinoosched *sched);
int rinoo_task_run(t_rinootask *task);

#endif		/* RINOO_SCHEDULER_TASK_H_ */
