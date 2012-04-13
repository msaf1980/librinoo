/**
 * @file   scheduler.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Mon Dec 28 00:14:20 2009
 *
 * @brief  Header file for scheduler function declarations.
 *
 *
 */

#ifndef		RINOO_SCHEDULER_SCHEDULER_H_
# define	RINOO_SCHEDULER_SCHEDULER_H_

# define	RINOO_SCHEDULER_MAXFDS		1000000

typedef enum e_rinooshed_action
  {
	  RINOO_SCHED_ADD = 0,
	  RINOO_SCHED_REMOVE
  } t_rinoosched_action;

typedef enum e_rinoosched_mode
{
	RINOO_MODE_NONE = 0,
	RINOO_MODE_IN = 1,
	RINOO_MODE_OUT = 2
} t_rinoosched_mode;

typedef struct s_rinoosched
{
	int			stop;
	struct timeval		clock;
	t_rinootask_driver	driver;
} t_rinoosched;

t_rinoosched *rinoo_sched();
void rinoo_sched_destroy(t_rinoosched *sched);
void rinoo_sched_stop(t_rinoosched *sched);
void rinoo_sched_loop(t_rinoosched *sched);

#endif	        /* !RINOO_SCHEDULER_SCHEDULER_H */
