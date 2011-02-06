/**
 * @file   udp.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Dec 30 17:05:32 2009
 *
 * @brief  Header file for udp functions declarations.
 *
 *
 */

#ifndef		RINOO_UDP_H_
# define	RINOO_UDP_H_

typedef enum	e_rinooudp_mode
  {
    RINOO_UDP_CLIENT = 0,
    RINOO_UDP_SERVER
  }		t_rinooudp_mode;

typedef enum	e_rinooudp_event
  {
    EVENT_UDP_IN = 0,
    EVENT_UDP_OUT,
    EVENT_UDP_ERROR,
    EVENT_UDP_CLOSE,
    EVENT_UDP_TIMEOUT
  }		t_rinooudp_event;

typedef struct		s_rinooudp
{
  t_rinoosocket		socket;
  t_ip			ip;
  u32			port;
  t_rinooudp_mode	mode;
  t_rinooudp_event	errorstep;
  void			(*event_fsm)(struct s_rinooudp *udpsock,
				     t_rinooudp_event event);
  void			*data;
}			t_rinooudp;

t_rinooudp	*rinoo_udp_client(t_rinoosched *sched,
				  t_ip ip,
				  u32 port,
				  u32 timeout,
				  void (*event_fsm)(t_rinooudp *udpsock,
						    t_rinooudp_event event));
t_rinooudp	*rinoo_udp_server(t_rinoosched *sched,
				  t_ip ip,
				  u32 port,
				  u32 timeout,
				  void (*event_fsm)(t_rinooudp *udpsock,
						    t_rinooudp_event event));
void		rinoo_udp_destroy(t_rinooudp *udpsock);
int		rinoo_udp_print(t_rinooudp *socket, const char *format, ...);
int		rinoo_udp_printdata(t_rinooudp *socket,
				    const char *data,
				    size_t size);
int		rinoo_udp_isserver(t_rinooudp *udpsock);

#endif		/* !RINOO_UDP_H_ */

