/**
 * @file   tcp.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Tue Mar 20 18:10:17 2012
 *
 * @brief  Header file for TCP function declarations
 *
 *
 */

#ifndef		RINOO_NET_TCP_H_
# define	RINOO_NET_TCP_H_

# define	RINOO_TCP_BACKLOG	128

t_rinoosocket *rinoo_tcp(t_rinoosched *sched, void (*run)(t_rinoosocket *socket));
int rinoo_tcp_connect(t_rinoosocket *socket, t_ip ip, u32 port, u32 timeout);
int rinoo_tcp_listen(t_rinoosocket *socket, t_ip ip, u32 port);
t_rinoosocket *rinoo_tcp_accept(t_rinoosocket *socket,  void (*run)(t_rinoosocket *socket), t_ip *fromip, u32 *fromport);

#endif		/* !RINOO_NET_TCP_H_ */
