/**
 * @file   httpserver.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Tue Oct 26 21:48:01 2010
 *
 * @brief  Header file for http server declarations
 *
 *
 */

#ifndef		RINOOHTTP_SERVER_HTTPSERVER_H_
# define	RINOOHTTP_SERVER_HTTPSERVER_H_

t_rinoohttp	*rinoo_http_server(t_rinoosched *sched,
				   t_ip ip,
				   u32 port,
				   u32 timeout,
				   void (*event_fsm)(t_rinoohttp *httpsock,
						     t_rinoohttp_event event));

#endif		/* !RINOOHTTP_SERVER_HTTPSERVER_H_ */
