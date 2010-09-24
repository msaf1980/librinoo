/**
 * @file   jobqueue.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2010
 * @date   Wed Apr 28 19:38:24 2010
 *
 * @brief  Header file for the job queue function declarations.
 *
 *
 */

#ifndef		RINOO_JOBQUEUE_H_
# define	RINOO_JOBQUEUE_H_

# define	DEFAULT_TIMEOUT		1000

typedef enum	e_jobstate
  {
    JOB_DONE = 0,
    JOB_REDO
  }		t_jobstate;

struct	s_socket;

typedef struct		s_job
{
  t_jobstate		(*func)(struct s_job *job);
  void			*args;
  t_listnode		*listnode;
  struct timeval	tv;
  struct s_socket	*socket;
}			t_job;

struct s_sched;
t_list		*jobqueue_create();
void		jobqueue_destroy(void *ptr);
t_job		*jobqueue_add(struct s_sched *sched,
			      struct s_socket *socket,
			      t_jobstate (*func)(t_job *job),
			      void *args,
			      const struct timeval *tv);
t_job		*jobqueue_addms(struct s_sched *sched,
				struct s_socket *socket,
				t_jobstate (*func)(t_job *job),
				void *args,
				const u32 msec);
int		jobqueue_reschedule(struct s_sched *sched,
				    t_job *job,
				    const struct timeval *tv);
int		jobqueue_resettime(t_job *job, const struct timeval *tv);
int		jobqueue_resettimems(t_job *job, const u32 msec);
u32		jobqueue_gettimeout(struct s_sched *sched);
void		jobqueue_exec(struct s_sched *sched);

#endif		/* !RINOO_JOBQUEUE_H_ */
