/**
 * @file   scheduler.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Mon Dec 28 00:14:20 2009
 *
 * @brief  Header file for the scheduler function declarations.
 *
 *
 */

#ifndef		RINOO_SCHEDULER_H_
# define	RINOO_SCHEDULER_H_

# define	RINOO_SCHED_MAXFDS		1000000

struct s_rinoopoller; /* defined in poller.h */
struct s_rinoosocket; /* defined in socket.h */
struct s_rinoojob_queue; /* defined in jobqueue.h */

typedef enum	e_rinooshed_action
  {
    RINOO_SCHED_INSERT = 0,
    RINOO_SCHED_MODADD,
    RINOO_SCHED_MODDEL,
    RINOO_SCHED_REMOVE
  }		t_rinoosched_action;

typedef enum	e_rinoosched_event
  {
    EVENT_SCHED_IN = 1,
    EVENT_SCHED_OUT = 2,
    EVENT_SCHED_ERROR = 4,
    EVENT_SCHED_TIMEOUT = 8,
    EVENT_SCHED_STOP = 16
  }		t_rinoosched_event;

typedef struct		s_rinootimeout
{
  u32			ms;
  struct timeval	expire;
  t_listnode		*node;
}			t_rinootimeout;

typedef struct			s_rinoosched
{
  int				stop;
  t_list			*timeoutq;
  struct s_rinoojob_queue	*jobq;
  struct timeval		curtime;
  struct s_rinoopoller		*poller;
  void				*poller_data;
  struct s_rinoosocket		*sock_pool[RINOO_SCHED_MAXFDS];
}				t_rinoosched;

t_rinoosched		*rinoo_sched();
void			rinoo_sched_destroy(t_rinoosched *sched);
int			rinoo_sched_socket(t_rinoosched_action action,
					   struct s_rinoosocket *socket,
					   t_rinoosched_event event);
struct s_rinoosocket	*rinoo_sched_get(t_rinoosched *sched, int fd);
void			rinoo_sched_stop(t_rinoosched *sched);
int			rinoo_sched_clock(t_rinoosched *sched);
int			rinoo_sched_loop(t_rinoosched *sched);

#endif	        /* !RINOO_SCHEDULER_H */
