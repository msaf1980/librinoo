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

typedef struct		s_rinoosched
{
  int			stop;
  struct s_rinoopoller	*poller;
  void			*poller_data;
  struct s_rinoosocket	*sock_pool[RINOO_SCHED_MAXFDS];
  struct timeval	curtime;
  t_list		*jobq;
  t_list		*timeoutq;
}			t_rinoosched;

t_rinoosched		*rinoo_sched();
void			rinoo_sched_destroy(t_rinoosched *sched);
struct s_rinoosocket	*rinoo_sched_getsocket(t_rinoosched *sched, int fd);
void			rinoo_sched_stop(t_rinoosched *sched);
int			rinoo_sched_insert(struct s_rinoosocket *socket,
					  t_rinoosched_event event,
					  u32 timeout);
int			rinoo_sched_addmode(struct s_rinoosocket *socket,
					   t_rinoosched_event event);
int			rinoo_sched_delmode(struct s_rinoosocket *socket,
					   t_rinoosched_event event);
int			rinoo_sched_remove(struct s_rinoosocket *socket);
int			rinoo_sched_loop(t_rinoosched *sched);

#endif	        /* !RINOO_SCHEDULER_H */
