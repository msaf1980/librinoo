/**
 * @file   tcp.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Dec 30 17:05:32 2009
 *
 * @brief  Header file for tcp functions declarations.
 *
 *
 */

#ifndef		RINOO_TCP_H_
# define	RINOO_TCP_H_

# define	TCP_LISTEN_SIZE		128

typedef enum	e_tcpmode
  {
    MODE_TCP_CLIENT = 0,
    MODE_TCP_SERVER
  }		t_tcpmode;

typedef enum	e_tcpevent
  {
    EVENT_TCP_CONNECT = 0,
    EVENT_TCP_IN,
    EVENT_TCP_OUT,
    EVENT_TCP_ERROR,
    EVENT_TCP_CLOSE,
    EVENT_TCP_TIMEOUT
  }		t_tcpevent;

typedef struct	s_tcpsocket
{
  t_socket	socket;
  t_ip		ip;
  u32		port;
  t_tcpmode	mode;
  void		(*event_fsm)(struct s_tcpsocket *tcpsock, t_tcpevent event);
  void		*data;
  t_tcpevent	errorstep;
  u32		child_timeout;
}		t_tcpsocket;

t_tcpsocket	*tcp_create(t_sched *sched,
			    t_ip ip,
			    u32 port,
			    t_tcpmode mode,
			    u32 timeout,
			    void (*event_fsm)(t_tcpsocket *tcpsock,
					      t_tcpevent event));
inline void	tcp_destroy(t_tcpsocket *tcpsock);
int		tcp_print(t_tcpsocket *socket, const char *format, ...);
int		tcp_printdata(t_tcpsocket *socket, const char *data, size_t size);
int		tcp_isserver(t_tcpsocket *tcpsock);
t_tcpsocket	*tcp_getparent(t_tcpsocket *tcpsock);

#endif		/* !RINOO_TCP_H_ */

