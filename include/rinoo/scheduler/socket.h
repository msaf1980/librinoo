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

typedef struct		s_socket
{
  int			fd;
  t_sched		*sched;
  t_schedevent		poll_mode;
  void			(*event_fsm)(struct s_socket *socket, t_schedevent event);
  void			*data;
  struct s_socket	*parent;
  t_buffer		*rdbuf;
  t_buffer		*wrbuf;
}			t_socket;

int		socket_print(t_socket *socket, const char *format, ...);

#endif		/* !RINOO_SOCKET_H_ */
