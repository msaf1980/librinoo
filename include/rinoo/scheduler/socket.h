/**
 * @file   socket.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2010
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

typedef struct		s_sockettimeout
{
  u32			ms;
  struct timeval	expire;
  t_listnode		*node;
}			t_sockettimeout;

typedef struct		s_socket
{
  int			fd;
  t_sched		*sched;
  t_schedevent		poll_mode;
  t_sockettimeout	timeout;
  void			(*event_fsm)(struct s_socket *socket, t_schedevent event);
  void			*data;
  struct s_socket	*parent;
  t_buffer		*rdbuf;
  t_buffer		*wrbuf;
}			t_socket;

t_list		*socket_expirequeue_create();
void		socket_expirequeue_destroy(t_list *expirequeue);
int		socket_settimeout(t_socket *socket, u32 timeout);
int		socket_resettimeout(t_socket *socket);
void		socket_removetimeout(t_socket *socket);
u32		socket_gettimeout(t_sched *sched);
t_socket	*socket_getexpired(t_sched *sched);
void		socket_flush(t_socket *socket);

#endif		/* !RINOO_SOCKET_H_ */
