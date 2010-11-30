/**
 * @file   httpserver.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2010
 * @date   Tue Oct 26 21:48:01 2010
 *
 * @brief  Header file for http server declarations
 *
 *
 */

#ifndef		RINOOHTTP_SERVER_HTTPSERVER_H_
# define	RINOOHTTP_SERVER_HTTPSERVER_H_

t_httpsocket	*httpserver_create(t_sched *sched,
				   t_ip ip,
				   u32 port,
				   u32 timeout,
				   void (*event_fsm)(t_httpsocket *httpsock,
						     t_httpevent event));

#endif		/* !RINOOHTTP_SERVER_HTTPSERVER_H_ */
