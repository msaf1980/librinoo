/**
 * @file   poller.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2010
 * @date   Mon Dec 28 02:33:18 2009
 *
 * @brief  Header file which defines a poller.
 *
 *
 */

#ifndef		RINOO_POLLER_H_
# define	RINOO_POLLER_H_

#  ifdef	HAS_EPOLL
#  define	DEFAULT_POLLER	POLLER_EPOLL
#  else
#   ifdef	HAS_SELECT
#    define	DEFAULT_POLLER	POLLER_SELECT
#   endif	/* !HAS_SELECT */
#  endif	/* !HAS_EPOLL */

typedef enum	e_pollertype
  {
    POLLER_SELECT = 0,
    POLLER_EPOLL = 1,
    NB_POLLERS
  }		t_pollertype;

typedef struct	s_pollerapi
{
  int		(*init)(t_sched *sched);
  void		(*destroy)(t_sched *sched);
  int		(*insert)(t_socket *socket, t_schedevent mode);
  int		(*addmode)(t_socket *socket, t_schedevent mode);
  int		(*delmode)(t_socket *socket, t_schedevent mode);
  int		(*remove)(t_socket *socket);
  int		(*poll)(t_sched *sched, u32 timeout);
}		t_pollerapi;

#endif		/* !RINOO_POLLER_H_ */
