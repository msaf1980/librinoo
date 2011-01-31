/**
 * @file   poller.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Mon Dec 28 02:33:18 2009
 *
 * @brief  Header file which defines a poller.
 *
 *
 */

#ifndef		RINOO_POLLER_H_
# define	RINOO_POLLER_H_

# ifdef		HAS_EPOLL
#  define	DEFAULT_POLLER	POLLER_EPOLL
# else
#  ifdef	HAS_SELECT
#   define	DEFAULT_POLLER	POLLER_SELECT
#  endif	/* !HAS_SELECT */
# endif		/* !HAS_EPOLL */

typedef enum	e_rinoopoller_type
  {
    POLLER_SELECT = 0,
    POLLER_EPOLL,
    NB_POLLERS
  }		t_rinoopoller_type;

typedef struct	s_rinoopoller
{
  int		(*init)(t_rinoosched *sched);
  void		(*destroy)(t_rinoosched *sched);
  int		(*insert)(t_rinoosocket *socket, t_rinoosched_event mode);
  int		(*addmode)(t_rinoosocket *socket, t_rinoosched_event mode);
  int		(*delmode)(t_rinoosocket *socket, t_rinoosched_event mode);
  int		(*remove)(t_rinoosocket *socket);
  int		(*poll)(t_rinoosched *sched, u32 timeout);
}		t_rinoopoller;

#endif		/* !RINOO_POLLER_H_ */
