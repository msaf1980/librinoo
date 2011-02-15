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

static void	httpserver_fsm(t_rinootcp *tcpsock, t_rinootcp_event event);

t_httpsocket	*httpserver_create(t_rinoosched *sched,
				   t_ip ip,
				   u32 port,
				   u32 timeout,
				   void (*event_fsm)(t_httpsocket *httpsock,
						     t_httpevent event))
{
  t_rinootcp	*servsock;
  t_httpsocket	*httpsock;

  XDASSERT(sched != NULL, NULL);

  servsock = rinoo_tcp_server(sched,
			      ip,
			      port,
			      timeout,
			      httpserver_fsm);
  XASSERT(servsock != NULL, NULL);
  httpsock = httpsocket_create(servsock, event_fsm);
  XASSERT(httpsock != NULL, NULL);
  return httpsock;
}

void		httpserver_generate_response(t_httpsocket *httpsock)
{
  char			*version;
  t_httpheader		*header;
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
      httpresponse_setdefaultmsg(httpsock);
    }

  httpresponse_setdefaultheaders(httpsock);

  rinoo_tcp_print(httpsock->tcpsock,
		  "%s %d %.*s\r\n",
		  version,
		  httpsock->response.code,
		  buffer_len(&httpsock->response.msg),
		  buffer_ptr(&httpsock->response.msg));

  while ((header = (t_httpheader *) hashtable_getnext(httpsock->response.headers,
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

static void	httpserver_fsm(t_rinootcp *tcpsock, t_rinootcp_event event)
{
  t_rinootcp	*parent;
  t_httpsocket	*httpsock;

  httpsock = (t_httpsocket *) tcpsock->data;
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
      httpsock = httpsocket_create(tcpsock,
				   ((t_httpsocket *) parent->data)->event_fsm);
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
	  switch (httprequest_readbody(httpsock))
	    {
	    case 0:
	      httpsock->event_fsm(httpsock, EVENT_HTTP_REQBODY);
	      httpsock->last_event = EVENT_HTTP_REQBODY;
	      break;
	    case -1:
	      /* bad request */
	      httpsocket_destroy(httpsock);
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
	  switch (httprequest_read(httpsock))
	    {
	    case 0:
	      return;
	    case -1:
	      /* bad request */
	      httpsocket_destroy(httpsock);
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
		  switch (httprequest_readbody(httpsock))
		    {
		    case 0:
		      httpsock->event_fsm(httpsock, EVENT_HTTP_REQBODY);
		      httpsock->last_event = EVENT_HTTP_REQBODY;
		      break;
		    case -1:
		      /* bad request */
		      httpsocket_destroy(httpsock);
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
	  /* httpserver_generate_response(httpsock); */
	  /* if (httpsock->response.contentlength == 0) */
	  /*   { */
	  /*     httpsock->last_event = EVENT_HTTP_RESPBODY; */
	  /*     break; */
	  /*   } */
	  /* no break */
	case EVENT_HTTP_RESPBODY:
	  if (httpsock->response.contentlength > 0)
	    {
	      httpsock->event_fsm(httpsock, EVENT_HTTP_RESPBODY);
	    }
	  httpsock->last_event = EVENT_HTTP_RESPBODY;
	  /* if (buffer_len(tcpsock->socket.wrbuf) == 0) */
	  /*   { */
	  /*     httpsocket_reset(httpsock); */
	  /*     rinoo_sched_socket(RINOO_SCHED_MODADD, &tcpsock->socket, EVENT_SCHED_IN); */
	  /*   } */
	  break;
	default:
	  break;
	}
      break;
    case EVENT_TCP_ERROR:
      httpsock->event_fsm(httpsock, EVENT_HTTP_ERROR);
      httpsocket_free(httpsock);
      break;
    case EVENT_TCP_CLOSE:
      httpsock->event_fsm(httpsock, EVENT_HTTP_CLOSE);
      httpsocket_free(httpsock);
      break;
    case EVENT_TCP_TIMEOUT:
      httpsock->event_fsm(httpsock, EVENT_HTTP_TIMEOUT);
      httpsocket_free(httpsock);
      break;
    }
}
