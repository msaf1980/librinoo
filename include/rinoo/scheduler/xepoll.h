/**
 * @file   xepoll.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
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

typedef struct		s_rinooepoll
{
  int			fd;
  sigset_t		sigmask;
  struct epoll_event	events[MAX_EVENTS];
}			t_rinooepoll;

int		xepoll_init(t_rinoosched *sched);
void		xepoll_destroy(t_rinoosched *sched);
int		xepoll_insert(t_rinoosocket *socket,
			      t_rinoosched_event mode);
int		xepoll_addmode(t_rinoosocket *socket,
			       t_rinoosched_event mode);
int		xepoll_delmode(t_rinoosocket *socket,
			       t_rinoosched_event mode);
int		xepoll_remove(t_rinoosocket *socket);
int		xepoll_poll(t_rinoosched *sched, u32 timeout);

#endif		/* !RINOO_XEPOLL_H_ */
