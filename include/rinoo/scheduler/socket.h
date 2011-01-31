/**
 * @file   socket.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Mon Dec 28 01:10:17 2009
 *
 * @brief  Header file for socket functions declarations.
 *
 *
 */

#ifndef		RINOO_SOCKET_H_
# define	RINOO_SOCKET_H_

# define	RDBUF_INITSIZE		1024
# define	RDBUF_MAXSIZE		1024 * 1024
# define	WRBUF_INITSIZE		1024
# define	WRBUF_MAXSIZE		1024 * 1024

typedef struct		s_rinoosocket
{
  int			fd;
  t_rinoosched		*sched;
  t_rinoosched_event	poll_mode;
  t_rinootimeout	timeout;
  void			*data;
  t_buffer		*rdbuf;
  t_buffer		*wrbuf;
  struct s_rinoosocket	*parent;
  void			(*event_fsm)(struct s_rinoosocket *socket,
				     t_rinoosched_event event);
}			t_rinoosocket;

int		rinoo_socket_timeout_cmp(void *node1, void *node2);
int		rinoo_socket_timeout_set(t_rinoosocket *socket, u32 timeout);
int		rinoo_socket_timeout_reset(t_rinoosocket *socket);
void		rinoo_socket_timeout_remove(t_rinoosocket *socket);
u32		rinoo_socket_timeout_getmin(t_rinoosched *sched);
t_rinoosocket	*rinoo_socket_getexpired(t_rinoosched *sched);

#endif		/* !RINOO_RINOO_SOCKET_H_ */
