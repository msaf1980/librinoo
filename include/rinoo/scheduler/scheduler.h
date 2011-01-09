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

# define	SCHED_MAXFDS		1000000

typedef enum	e_schedevent
  {
    EVENT_SCHED_IN = 1,
    EVENT_SCHED_OUT = 2,
    EVENT_SCHED_ERROR = 4,
    EVENT_SCHED_TIMEOUT = 8,
    EVENT_SCHED_STOP = 16
  }		t_schedevent;

struct s_pollerapi; /* defined in poller.h */
struct s_socket; /* defined in socket.h */

typedef struct		s_sched
{
  int			stop;
  struct s_pollerapi	*poller;
  void			*poller_data;
  struct s_socket	*sock_pool[SCHED_MAXFDS];
  struct timeval	curtime;
  t_list		*expirequeue;
  t_list		*jobqueue;
}			t_sched;

t_sched		*sched_create();
void		sched_destroy(t_sched *sched);
struct s_socket	*sched_getsocket(t_sched *sched, int fd);
void		sched_stop(t_sched *sched);
int		sched_insert(struct s_socket *socket,
			     t_schedevent event,
			     u32 timeout);
int		sched_addmode(struct s_socket *socket, t_schedevent event);
int		sched_delmode(struct s_socket *socket, t_schedevent event);
int		sched_remove(struct s_socket *socket);
int		sched_loop(struct s_sched *sched);

#endif	        /* !RINOO_SCHEDULER_H */
