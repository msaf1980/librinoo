/**
 * @file   httpclient.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Tue Jan 18 22:56:51 2011
 *
 * @brief  Header file for http client declarations
 *
 *
 */

#ifndef		RINOOHTTP_CLIENT_HTTPCLIENT_H_
# define	RINOOHTTP_CLIENT_HTTPCLIENT_H_

t_rinoohttp	*rinoo_http_client(t_rinoosched *sched,
				   t_ip ip,
				   u32 port,
				   u32 timeout,
				   void (*event_fsm)(t_rinoohttp *httpsock,
						     t_rinoohttp_event event));

#endif		/* !RINOOHTTP_CLIENT_HTTPCLIENT_H_ */
