/**
 * @file   httpsocket.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Tue Oct 26 08:28:48 2010
 *
 * @brief  Header file for http socket declarations.
 *
 *
 */

#ifndef		RINOOHTTP_GLOBAL_HTTPSOCKET_H_
# define	RINOOHTTP_GLOBAL_HTTPSOCKET_H_

# define	URI_INITSIZE	1024
# define	URI_MAXSIZE	1024 * 8

typedef enum	e_httpevent
  {
    EVENT_HTTP_CONNECT = 0,
    EVENT_HTTP_REQUEST,
    EVENT_HTTP_REQBODY,
    EVENT_HTTP_RESPONSE,
    EVENT_HTTP_RESPBODY,
    EVENT_HTTP_ERROR,
    EVENT_HTTP_CLOSE,
    EVENT_HTTP_TIMEOUT
  }		t_httpevent;

typedef struct		s_httpsocket
{
  t_rinootcp		*tcpsock;
  t_httprequest		request;
  t_httpresponse	response;
  t_httpevent		last_event;
  void			(*event_fsm)(struct s_httpsocket *httpsock,
				     t_httpevent event);
  void			*data;
}			t_httpsocket;

t_httpsocket	*httpsocket_create(t_rinootcp *tcpsock,
				   void (*event_fsm)(t_httpsocket *httpsock,
						     t_httpevent event));
void		httpsocket_free(t_httpsocket *httpsock);
void		httpsocket_destroy(t_httpsocket *httpsock);
void		httpsocket_reset(t_httpsocket *httpsock);

#endif		/* !RINOOHTTP_GLOBAL_HTTPSOCKET_H_ */
