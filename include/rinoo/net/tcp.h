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

# define	RINOO_TCP_LISTENSIZE		128

typedef enum	e_rinootcp_mode
  {
    RINOO_TCP_CLIENT = 0,
    RINOO_TCP_SERVER
  }		t_rinootcp_mode;

typedef enum	e_rinootcp_event
  {
    EVENT_TCP_CONNECT = 0,
    EVENT_TCP_IN,
    EVENT_TCP_OUT,
    EVENT_TCP_ERROR,
    EVENT_TCP_CLOSE,
    EVENT_TCP_TIMEOUT
  }		t_rinootcp_event;

typedef struct		s_rinootcp
{
  t_rinoosocket		socket;
  t_ip			ip;
  u16			port;
  t_rinootcp_mode	mode;
  t_rinootcp_event	errorstep;
  void			(*event_fsm)(struct s_rinootcp *tcpsock,
				     t_rinootcp_event event);
  void			*data;
}			t_rinootcp;

t_rinootcp	*rinoo_tcp_client(t_rinoosched *sched,
				  t_ip ip,
				  u16 port,
				  u32 timeout,
				  void (*event_fsm)(t_rinootcp *tcpsock,
						    t_rinootcp_event event));
t_rinootcp	*rinoo_tcp_server(t_rinoosched *sched,
				  t_ip ip,
				  u16 port,
				  u32 timeout,
				  void (*event_fsm)(t_rinootcp *tcpsock,
						    t_rinootcp_event event));
void		rinoo_tcp_destroy(t_rinootcp *tcpsock);
int		rinoo_tcp_print(t_rinootcp *socket, const char *format, ...);
int		rinoo_tcp_printdata(t_rinootcp *socket,
				    const char *data,
				    size_t size);
int		rinoo_tcp_isserver(t_rinootcp *tcpsock);
t_rinootcp	*rinoo_tcp_getparent(t_rinootcp *tcpsock);

#endif		/* !RINOO_TCP_H_ */

