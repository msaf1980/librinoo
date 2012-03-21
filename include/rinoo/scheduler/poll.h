/**
 * @file   poll.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Tue Mar 20 15:33:15 2012
 *
 * @brief  Header file defining poll interface
 *
 *
 */

#ifndef		RINOO_POLL_H_
# define	RINOO_POLL_H_

# define	RINOO_DEFAULT_POLL	RINOO_POLL_EPOLL

struct s_rinoosocket;	/* Defined in net/socket.h */

typedef enum	e_rinoopoll_type
  {
	  RINOO_POLL_EPOLL = 0,
	  RINOO_NB_POLLS
  }		t_rinoopoll_type;

typedef struct	s_rinoopoll
{
  void		*context;
  int		(*init)(t_rinoosched *sched);
  void		(*destroy)(t_rinoosched *sched);
  int		(*insert)(struct s_rinoosocket *socket, t_rinoosched_mode mode);
  int		(*addmode)(struct s_rinoosocket *socket, t_rinoosched_mode mode);
  int		(*remove)(struct s_rinoosocket *socket);
  int		(*poll)(t_rinoosched *sched, u32 timeout);
}		t_rinoopoll;

#endif		/* !RINOO_POLL_H_ */
