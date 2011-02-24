/**
 * @file   httpclient.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Tue Jan 18 21:49:56 2011
 *
 * @brief  RinOO HTTP client library
 *
 *
 */


#include	"rinoo/rinoohttp.h"

static void	rinoo_http_client_fsm(t_rinootcp *tcpsock, t_rinootcp_event event);

t_rinoohttp	*rinoo_http_client(t_rinoosched *sched,
				   t_ip ip,
				   u32 port,
				   u32 timeout,
				   void (*event_fsm)(t_rinoohttp *httpsock,
						     t_rinoohttp_event event))
{
  t_rinootcp	*clientsock;
  t_rinoohttp	*httpsock;

  XDASSERT(sched != NULL, NULL);

  clientsock = rinoo_tcp_client(sched,
				ip,
				port,
				timeout,
				rinoo_http_client_fsm);
  XASSERT(clientsock != NULL, NULL);
  httpsock = rinoo_http_socket(clientsock, event_fsm);
  XASSERT(httpsock != NULL, NULL);
  return httpsock;
}

void		rinoo_http_client_genrequest(t_rinoohttp *httpsock)
{
  char			*method;
  char			*version;
  t_rinoohttp_header	*header;
  t_hashiterator	iterator = { 0, 0 };

  switch (httpsock->request.method)
    {
    case HTTP_METHOD_NONE:
    case HTTP_METHOD_GET:
      method  = "GET";
      break;
    case HTTP_METHOD_POST:
      method = "POST";
      break;
    }
  switch (httpsock->request.version)
    {
    case HTTP_VERSION_10:
      version = "HTTP/1.0";
      break;
    case HTTP_VERSION_NONE:
    case HTTP_VERSION_11:
      version = "HTTP/1.1";
      break;
    }


  if (buffer_len(httpsock->request.uri) == 0)
    {
      buffer_add(httpsock->request.uri, "/", 1);
    }

  rinoo_http_request_setdefaultheaders(httpsock);

  rinoo_tcp_print(httpsock->tcpsock,
		  "%s %.*s %s\r\n",
		  method,
		  buffer_len(httpsock->request.uri),
		  buffer_ptr(httpsock->request.uri),
		  version);

  while ((header = (t_rinoohttp_header *) hashtable_getnext(httpsock->request.headers,
							    &iterator)) != NULL)
    {
      rinoo_tcp_print(httpsock->tcpsock,
		      "%.*s: %.*s\r\n",
		      buffer_len(&header->key),
		      buffer_ptr(&header->key),
		      buffer_len(&header->value),
		      buffer_ptr(&header->value));
    }
  rinoo_tcp_print(httpsock->tcpsock, "\r\n");
}

static void	rinoo_http_client_fsm(t_rinootcp *tcpsock, t_rinootcp_event event)
{
  t_rinoohttp	*httpsock;

  httpsock = (t_rinoohttp *) tcpsock->data;
  switch (event)
    {
    case EVENT_TCP_CONNECT:
      XASSERTN(httpsock != NULL);
      httpsock->event_fsm(httpsock, EVENT_HTTP_CONNECT);
      httpsock->last_event = EVENT_HTTP_CONNECT;
      rinoo_sched_socket(RINOO_SCHED_MODDEL, &tcpsock->socket, EVENT_SCHED_IN);
      rinoo_sched_socket(RINOO_SCHED_MODADD, &tcpsock->socket, EVENT_SCHED_OUT);
      break;
    case EVENT_TCP_IN:
      switch (httpsock->last_event)
	{
	case EVENT_HTTP_REQUEST:
	case EVENT_HTTP_REQBODY:
	  switch(rinoo_http_response_read(httpsock))
	    {
	    case 0:
	      return;
	    case -1:
	      /* bad response */
	      rinoo_http_socket_destroy(httpsock);
	      return;
	    case 1:
	      rinoo_socket_timeout_reset(&tcpsock->socket);
	      httpsock->event_fsm(httpsock, EVENT_HTTP_RESPONSE);
	      httpsock->last_event = EVENT_HTTP_RESPONSE;
	      buffer_erase(tcpsock->socket.rdbuf, httpsock->response.length);
	      if (buffer_len(tcpsock->socket.rdbuf) == 0)
		{
		  return;
		}
	      break;
	    }
	  /* no break */
	case EVENT_HTTP_RESPONSE:
	case EVENT_HTTP_RESPBODY:
	  switch (rinoo_http_response_readbody(httpsock))
	    {
	    case 0:
	      rinoo_socket_timeout_reset(&tcpsock->socket);
	      httpsock->event_fsm(httpsock, EVENT_HTTP_RESPBODY);
	      httpsock->last_event = EVENT_HTTP_RESPBODY;
	      break;
	    case -1:
	      /* bad request */
	      rinoo_http_socket_destroy(httpsock);
	      return;
	    case 1:
	      rinoo_socket_timeout_reset(&tcpsock->socket);
	      httpsock->event_fsm(httpsock, EVENT_HTTP_RESPBODY);
	      httpsock->last_event = EVENT_HTTP_RESPBODY;
	      rinoo_http_socket_reset(httpsock);
	      /* rinoo_sched_socket(RINOO_SCHED_MODDEL, &tcpsock->socket, EVENT_SCHED_IN); */
	      /* rinoo_sched_socket(RINOO_SCHED_MODADD, &tcpsock->socket, EVENT_SCHED_OUT); */
	      break;
	    }
	  break;
	default:
	  break;
	}
      break;
    case EVENT_TCP_OUT:
      switch (httpsock->last_event)
	{
	case EVENT_HTTP_CONNECT:
	  httpsock->event_fsm(httpsock, EVENT_HTTP_REQUEST);
	  httpsock->last_event = EVENT_HTTP_REQUEST;
	  rinoo_http_client_genrequest(httpsock);
	  /* no break */
	case EVENT_HTTP_REQUEST:
	case EVENT_HTTP_REQBODY:
	  httpsock->event_fsm(httpsock, EVENT_HTTP_REQBODY);
	  httpsock->last_event = EVENT_HTTP_REQBODY;
	  if (buffer_len(tcpsock->socket.wrbuf) == 0)
	    {
	      rinoo_sched_socket(RINOO_SCHED_MODADD, &tcpsock->socket, EVENT_SCHED_IN);
	    }
	  break;
	default:
	  break;
	}
      break;
    case EVENT_TCP_ERROR:
      httpsock->event_fsm(httpsock, EVENT_HTTP_ERROR);
      rinoo_http_socket_free(httpsock);
      break;
    case EVENT_TCP_CLOSE:
      httpsock->event_fsm(httpsock, EVENT_HTTP_CLOSE);
      rinoo_http_socket_free(httpsock);
      break;
    case EVENT_TCP_TIMEOUT:
      httpsock->event_fsm(httpsock, EVENT_HTTP_TIMEOUT);
      rinoo_http_socket_free(httpsock);
      break;
    }
}
