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

typedef enum	e_rinoohttp_event
  {
    EVENT_HTTP_CONNECT = 0,
    EVENT_HTTP_REQUEST,
    EVENT_HTTP_REQBODY,
    EVENT_HTTP_RESPONSE,
    EVENT_HTTP_RESPBODY,
    EVENT_HTTP_ERROR,
    EVENT_HTTP_CLOSE,
    EVENT_HTTP_TIMEOUT
  }		t_rinoohttp_event;

typedef struct		s_rinoohttp
{
  t_rinootcp		*tcpsock;
  t_rinoohttp_request	request;
  t_rinoohttp_response	response;
  t_rinoohttp_event	last_event;
  void			(*event_fsm)(struct s_rinoohttp *httpsock,
				     t_rinoohttp_event event);
  void			*data;
}			t_rinoohttp;

t_rinoohttp	*rinoo_http_socket(t_rinootcp *tcpsock,
				   void (*event_fsm)(t_rinoohttp *httpsock,
						     t_rinoohttp_event event));
void		rinoo_http_socket_free(t_rinoohttp *httpsock);
void		rinoo_http_socket_destroy(t_rinoohttp *httpsock);
void		rinoo_http_socket_reset(t_rinoohttp *httpsock);

#endif		/* !RINOOHTTP_GLOBAL_HTTPSOCKET_H_ */
