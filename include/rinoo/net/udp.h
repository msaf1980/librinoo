/**
 * @file   udp.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2010
 * @date   Wed Dec 30 17:05:32 2009
 *
 * @brief  Header file for udp functions declarations.
 *
 *
 */

#ifndef		RINOO_UDP_H_
# define	RINOO_UDP_H_

typedef enum	e_udpmode
  {
    MODE_UDP_CLIENT = 0,
    MODE_UDP_SERVER
  }		t_udpmode;

typedef enum	e_udpevent
  {
    EVENT_UDP_IN = 0,
    EVENT_UDP_OUT,
    EVENT_UDP_ERROR,
    EVENT_UDP_CLOSE,
    EVENT_UDP_TIMEOUT
  }		t_udpevent;

typedef struct	s_udphost
{
  t_ip		ip;
  u32		port;
}		t_udphost;

typedef struct	s_udppacket
{
  t_udphost	host;
  t_buffer	buffer;
}		t_udppacket;

typedef struct	s_udpsocket
{
  t_socket	socket;
  t_ip		ip;
  u32		port;
  t_udpmode	mode;
  void		(*event_fsm)(struct s_udpsocket *udpsock,
			     t_udpevent event,
			     t_udphost *host);
  void		*data;
  t_fifo	*wrfifo;
  t_udpevent	errorstep;
}		t_udpsocket;

t_udpsocket	*udp_create(t_sched *sched, t_ip ip,
			    u32 port, t_udpmode mode,
			    void (*event_fsm)(t_udpsocket *udpsock,
					      t_udpevent event,
					      t_udphost *host));
inline void	udp_destroy(t_udpsocket *udpsock);
int		udp_print(t_udpsocket *socket,
			  t_udphost *host,
			  const char *format, ...);

#endif		/* !RINOO_UDP_H_ */

