/**
 * @file   task.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Tue Jul  5 17:21:44 2011
 *
 * @brief  Header file for the tasks function declarations.
 *
 *
 */

#ifndef		RINOO_SCHEDULER_TASK_H_
# define	RINOO_SCHEDULER_TASK_H_

# define	RINOO_TASK_STACK_SIZE	16384

typedef void (*t_rinootask_func)(t_rinoosched *sched, void *arg);

typedef union		u_rinootask_arg
{
  void			*ptr;
  int			args[2];
}			t_rinootask_arg;

typedef struct		s_rinootask
{
  char			stack[RINOO_TASK_STACK_SIZE];
  ucontext_t		context;
}			t_rinootask;

#endif		/* RINOO_SCHEDULER_TASK_H_ */
