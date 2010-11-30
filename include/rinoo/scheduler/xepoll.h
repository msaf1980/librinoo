/**
 * @file   xepoll.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2010
 * @date   Mon Dec 28 02:41:48 2009
 *
 * @brief  Header file for the epoll functions declarations.
 *
 *
 */

#ifndef		RINOO_XEPOLL_H_
# define	RINOO_XEPOLL_H_

# define	MAX_EVENTS	128

# include	<sys/epoll.h>

typedef struct		s_epolldata
{
  int			fd;
  struct epoll_event	events[MAX_EVENTS];
  sigset_t		sigmask;
}			t_epolldata;

int		xepoll_init(t_sched *sched);
void		xepoll_destroy(t_sched *sched);
int		xepoll_insert(t_socket *socket, t_schedevent mode);
int		xepoll_addmode(t_socket *socket, t_schedevent mode);
int		xepoll_delmode(t_socket *socket, t_schedevent mode);
int		xepoll_remove(t_socket *socket);
int		xepoll_poll(t_sched *sched, u32 timeout);

#endif		/* !RINOO_XEPOLL_H_ */
