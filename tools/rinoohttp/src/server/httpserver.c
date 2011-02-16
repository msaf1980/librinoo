/**
 * @file   httpserver.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Nov 10 14:41:50 2010
 *
 * @brief  RinOO HTTP server library
 *
 *
 */

#include	"rinoo/rinoohttp.h"

static void	rinoo_http_server_fsm(t_rinootcp *tcpsock, t_rinootcp_event event);

t_rinoohttp	*rinoo_http_server(t_rinoosched *sched,
				   t_ip ip,
				   u32 port,
				   u32 timeout,
				   void (*event_fsm)(t_rinoohttp *httpsock,
						     t_rinoohttp_event event))
{
  t_rinootcp	*servsock;
  t_rinoohttp	*httpsock;

  XDASSERT(sched != NULL, NULL);

  servsock = rinoo_tcp_server(sched,
			      ip,
			      port,
			      timeout,
			      rinoo_http_server_fsm);
  XASSERT(servsock != NULL, NULL);
  httpsock = rinoo_http_socket(servsock, event_fsm);
  XASSERT(httpsock != NULL, NULL);
  return httpsock;
}

void		rinoo_http_server_genresponse(t_rinoohttp *httpsock)
{
  char			*version;
  t_rinoohttp_header	*header;
  t_hashiterator	iterator = { 0, 0 };

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

  if (httpsock->response.code == 0||
      httpsock->response.msg.buf == NULL ||
      httpsock->response.msg.len == 0)
    {
      rinoo_http_response_setdefaultmsg(httpsock);
    }

  rinoo_http_response_setdefaultheaders(httpsock);

  rinoo_tcp_print(httpsock->tcpsock,
		  "%s %d %.*s\r\n",
		  version,
		  httpsock->response.code,
		  buffer_len(&httpsock->response.msg),
		  buffer_ptr(&httpsock->response.msg));

  while ((header = (t_rinoohttp_header *) hashtable_getnext(httpsock->response.headers,
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

static void	rinoo_http_server_fsm(t_rinootcp *tcpsock, t_rinootcp_event event)
{
  t_rinootcp	*parent;
  t_rinoohttp	*httpsock;

  httpsock = (t_rinoohttp *) tcpsock->data;
  if (event != EVENT_TCP_CONNECT && httpsock == NULL)
    {
      /* Ignoring events from server socket */
      XDASSERTN(rinoo_tcp_isserver(tcpsock) == 1);
      return;
    }
  switch (event)
    {
    case EVENT_TCP_CONNECT:
      XASSERTN(tcpsock->data == NULL);
      parent = rinoo_tcp_getparent(tcpsock);
      if (parent == NULL)
	{
	  rinoo_tcp_destroy(tcpsock);
	  XASSERTN(0);
	}
      XDASSERTN(parent->data != NULL);
      httpsock = rinoo_http_socket(tcpsock,
				   ((t_rinoohttp *) parent->data)->event_fsm);
      if (httpsock == NULL)
	{
	  rinoo_tcp_destroy(tcpsock);
	  XASSERTN(0);
	}
      httpsock->event_fsm(httpsock, EVENT_HTTP_CONNECT);
      httpsock->last_event = EVENT_HTTP_CONNECT;
      break;
    case EVENT_TCP_IN:
      switch (httpsock->last_event)
	{
	case EVENT_HTTP_REQUEST:
	case EVENT_HTTP_REQBODY:
	  switch (rinoo_http_request_readbody(httpsock))
	    {
	    case 0:
	      httpsock->event_fsm(httpsock, EVENT_HTTP_REQBODY);
	      httpsock->last_event = EVENT_HTTP_REQBODY;
	      break;
	    case -1:
	      /* bad request */
	      rinoo_http_socket_destroy(httpsock);
	      return;
	    case 1:
	      rinoo_socket_timeout_reset(&tcpsock->socket);
	      httpsock->event_fsm(httpsock, EVENT_HTTP_REQBODY);
	      httpsock->last_event = EVENT_HTTP_REQBODY;
	      rinoo_sched_socket(RINOO_SCHED_MODDEL, &tcpsock->socket, EVENT_SCHED_IN);
	      rinoo_sched_socket(RINOO_SCHED_MODADD, &tcpsock->socket, EVENT_SCHED_OUT);
	      break;
	    }
	  break;
	default:
	  switch (rinoo_http_request_read(httpsock))
	    {
	    case 0:
	      return;
	    case -1:
	      /* bad request */
	      rinoo_http_socket_destroy(httpsock);
	      return;
	    case 1:
	      rinoo_socket_timeout_reset(&tcpsock->socket);
	      if (httpsock->request.contentlength == 0)
		{
		  rinoo_sched_socket(RINOO_SCHED_MODDEL, &tcpsock->socket, EVENT_SCHED_IN);
		  rinoo_sched_socket(RINOO_SCHED_MODADD, &tcpsock->socket, EVENT_SCHED_OUT);
		}
	      httpsock->event_fsm(httpsock, EVENT_HTTP_REQUEST);
	      httpsock->last_event = EVENT_HTTP_REQUEST;
	      buffer_erase(tcpsock->socket.rdbuf, httpsock->request.length);
	      if (buffer_len(tcpsock->socket.rdbuf) > 0)
		{
		  switch (rinoo_http_request_readbody(httpsock))
		    {
		    case 0:
		      httpsock->event_fsm(httpsock, EVENT_HTTP_REQBODY);
		      httpsock->last_event = EVENT_HTTP_REQBODY;
		      break;
		    case -1:
		      /* bad request */
		      rinoo_http_socket_destroy(httpsock);
		      return;
		    case 1:
		      httpsock->event_fsm(httpsock, EVENT_HTTP_REQBODY);
		      httpsock->last_event = EVENT_HTTP_REQBODY;
		      rinoo_sched_socket(RINOO_SCHED_MODDEL, &tcpsock->socket, EVENT_SCHED_IN);
		      rinoo_sched_socket(RINOO_SCHED_MODADD, &tcpsock->socket, EVENT_SCHED_OUT);
		      break;
		    }
		}
	      break;
	    }
	  break;
	}
      buffer_erase(tcpsock->socket.rdbuf, buffer_len(tcpsock->socket.rdbuf));
      break;
    case EVENT_TCP_OUT:
      switch (httpsock->last_event)
	{
	case EVENT_HTTP_REQUEST:
	case EVENT_HTTP_REQBODY:
	  httpsock->event_fsm(httpsock, EVENT_HTTP_RESPONSE);
	  httpsock->last_event = EVENT_HTTP_RESPONSE;
	  /* no break */
	case EVENT_HTTP_RESPONSE:
	  rinoo_http_server_genresponse(httpsock);
	  if (httpsock->response.contentlength == 0)
	    {
	      httpsock->last_event = EVENT_HTTP_RESPBODY;
	      break;
	    }
	  /* no break */
	case EVENT_HTTP_RESPBODY:
	  if (httpsock->response.contentlength > 0)
	    {
	      httpsock->event_fsm(httpsock, EVENT_HTTP_RESPBODY);
	    }
	  httpsock->last_event = EVENT_HTTP_RESPBODY;
	  if (buffer_len(tcpsock->socket.wrbuf) == 0)
	    {
	      rinoo_http_socket_reset(httpsock);
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
