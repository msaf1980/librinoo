/**
 * @file   epoll.h
 * @author Reginald Lips <reginald.l@gmail.com>
 * @date   Tue Mar 20 15:44:45 2012
 *
 * @brief  Header file for epoll function declarations.
 *
 *
 */

#ifndef		RINOO_EPOLL_H_
# define	RINOO_EPOLL_H_

# define	RINOO_EPOLL_MAX_EVENTS	128

# include	<sys/epoll.h>

struct s_rinoosocket;	/* Defined in net/socket.h */

typedef struct		s_rinooepoll
{
  int			fd;
  sigset_t		sigmask;
  struct epoll_event	events[RINOO_EPOLL_MAX_EVENTS];
}			t_rinooepoll;

int rinoo_epoll_init(t_rinoosched *sched);
void rinoo_epoll_destroy(t_rinoosched *sched);
int rinoo_epoll_insert(struct s_rinoosocket *socket, t_rinoosched_mode mode);
int rinoo_epoll_addmode(struct s_rinoosocket *socket, t_rinoosched_mode mode);
int rinoo_epoll_remove(struct s_rinoosocket *socket);
int rinoo_epoll_poll(t_rinoosched *sched, u32 timeout);

#endif		/* !RINOO_RINOO_EPOLL_H_ */
