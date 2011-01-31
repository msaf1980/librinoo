/**
 * @file   jobqueue.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Apr 28 19:38:24 2010
 *
 * @brief  Header file for the job queue function declarations.
 *
 *
 */

#ifndef		RINOO_JOBQUEUE_H_
# define	RINOO_JOBQUEUE_H_

# define	DEFAULT_TIMEOUT		1000

typedef enum	e_rinoojob_state
  {
    JOB_DONE = 0,
    JOB_LOOP,
    JOB_REDO
  }		t_rinoojob_state;

typedef struct		s_rinoojob
{
  t_rinoojob_state	(*func)(struct s_rinoojob *job);
  void			*args;
  t_listnode		*listnode;
  t_rinoosched		*sched;
  struct timeval	creatime;
  struct timeval	exectime;
}			t_rinoojob;

t_list		*jobqueue_create();
void		jobqueue_destroy(void *ptr);
void		jobqueue_removejob(t_rinoojob *job);
t_rinoojob	*jobqueue_add(t_rinoosched *sched,
			      t_rinoojob_state (*func)(t_rinoojob *job),
			      void *args,
			      const struct timeval *tv);
t_rinoojob	*jobqueue_addms(t_rinoosched *sched,
				t_rinoojob_state (*func)(t_rinoojob *job),
				void *args,
				const u32 msec);
int		jobqueue_reschedule(t_rinoosched *sched, t_rinoojob *job);
int		jobqueue_resettime(t_rinoosched *sched,
				   t_rinoojob *job,
				   const struct timeval *tv);
int		jobqueue_resettimems(t_rinoosched *sched,
				     t_rinoojob *job,
				     const u32 msec);
u32		jobqueue_gettimeout(t_rinoosched *sched);
void		jobqueue_exec(t_rinoosched *sched);

#endif		/* !RINOO_JOBQUEUE_H_ */
