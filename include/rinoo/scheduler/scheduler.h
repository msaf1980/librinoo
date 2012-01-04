/**
 * @file   scheduler.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Mon Dec 28 00:14:20 2009
 *
 * @brief  Header file for the scheduler function declarations.
 *
 *
 */

#ifndef		RINOO_SCHEDULER_SCHEDULER_H_
# define	RINOO_SCHEDULER_SCHEDULER_H_

# define	RINOO_SCHEDULER_MAXFDS		1000000

typedef struct			s_rinoosched
{
  int				stop;
}				t_rinoosched;

t_rinoosched		*rinoo_sched();
void			rinoo_sched_destroy(t_rinoosched *sched);

#endif	        /* !RINOO_SCHEDULER_SCHEDULER_H */
