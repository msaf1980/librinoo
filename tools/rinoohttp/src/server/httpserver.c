/**
 * @file   httpserver.c
 * @author Reginald <reginald.l@gmail.com>
 * @date   Wed Nov 10 14:41:50 2010
 *
 * @brief  RinOO HTTP server library
 *
 *
 */

#include	"rinoo/rinoohttp.h"

static void	httpserver_fsm(t_tcpsocket *tcpsock, t_tcpevent event);

t_httpsocket	*httpserver_create(t_sched *sched,
				   t_ip ip,
				   u32 port,
				   u32 timeout,
				   void (*event_fsm)(t_httpsocket *httpsock,
						     t_httpevent event))
{
  t_tcpsocket	*servsock;
  t_httpsocket	*httpsock;

  XDASSERT(sched != NULL, NULL);

  servsock = tcp_create(sched,
			ip,
			port,
			MODE_TCP_SERVER,
			timeout,
			httpserver_fsm);
  XASSERT(servsock != NULL, NULL);
  httpsock = httpsocket_create(servsock, event_fsm);
  XASSERT(httpsock != NULL, NULL);
  return httpsock;
}

void		httpserver_generate_response(t_httpsocket *httpsock)
{
  char		*version;
  u64		contentlength;

  switch (httpsock->request.version)
    {
    case HTTP_10:
      version = "HTTP/1.0";
      break;
    case HTTP_11:
      version = "HTTP/1.1";
      break;
    }

  if (buffer_len(&httpsock->response.body) > 0)
    {
      contentlength = buffer_len(&httpsock->response.body);
    }
  else if (httpsock->response.contentlength > 0)
    {
      contentlength = httpsock->response.contentlength;
    }
  else
    {
      contentlength = 0;
    }

  tcp_print(httpsock->tcpsock,
	    "%s %d %.*s\r\n"
	    "Server: RinOO/0.1\r\n"
	    "Content-length: %d\r\n\r\n",
	    version,
	    httpsock->response.code,
	    buffer_len(&httpsock->response.msg),
	    buffer_ptr(&httpsock->response.msg),
	    contentlength);

  if (buffer_len(&httpsock->response.body) > 0)
    {
      tcp_print(httpsock->tcpsock,
		"%.*s",
		buffer_len(&httpsock->response.body),
		buffer_ptr(&httpsock->response.body));
    }
}

static void	httpserver_fsm(t_tcpsocket *tcpsock, t_tcpevent event)
{
  t_tcpsocket	*parent;
  t_httpsocket	*httpsock;

  httpsock = (t_httpsocket *) tcpsock->data;
  if (event != EVENT_TCP_CONNECT && httpsock == NULL)
    {
      /* Ignoring events from server socket */
      XDASSERTN(tcp_isserver(tcpsock) == 1);
      return;
    }
  switch (event)
    {
    case EVENT_TCP_CONNECT:
      XASSERTN(tcpsock->data == NULL);
      parent = tcp_getparent(tcpsock);
      if (parent == NULL)
	{
	  tcp_destroy(tcpsock);
	  XASSERTN(0);
	}
      XDASSERTN(parent->data != NULL);
      httpsock = httpsocket_create(tcpsock,
				   ((t_httpsocket *) parent->data)->event_fsm);
      if (httpsock == NULL)
	{
	  tcp_destroy(tcpsock);
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
	      httpsock->event_fsm(httpsock, EVENT_HTTP_REQBODY);
	      httpsock->last_event = EVENT_HTTP_REQBODY;
	      sched_delmode(&tcpsock->socket, EVENT_SCHED_IN);
	      sched_addmode(&tcpsock->socket, EVENT_SCHED_OUT);
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
	      if (httpsock->request.contentlength == 0)
		{
		  sched_delmode(&tcpsock->socket, EVENT_SCHED_IN);
		  sched_addmode(&tcpsock->socket, EVENT_SCHED_OUT);
		}
	      httpsock->event_fsm(httpsock, EVENT_HTTP_REQUEST);
	      httpsock->last_event = EVENT_HTTP_REQUEST;
	      if (httpsock->request.length < buffer_len(tcpsock->socket.rdbuf))
		{
		  buffer_erase(tcpsock->socket.rdbuf, httpsock->request.length);
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
		      sched_delmode(&tcpsock->socket, EVENT_SCHED_IN);
		      sched_addmode(&tcpsock->socket, EVENT_SCHED_OUT);
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
	  if (httpsock->response.code != 0)
	    {
	      httpserver_generate_response(httpsock);
	    }
	  if (buffer_len(&httpsock->response.body) == 0)
	    {
	      httpsock->last_event = EVENT_HTTP_RESPONSE;
	    }
	  else
	    {
	      httpsock->last_event = EVENT_HTTP_RESPBODY;
	    }
	  break;
	case EVENT_HTTP_RESPONSE:
	case EVENT_HTTP_RESPBODY:
	  httpsock->event_fsm(httpsock, EVENT_HTTP_RESPBODY);
	  httpsock->last_event = EVENT_HTTP_RESPBODY;
	  if (buffer_len(tcpsock->socket.wrbuf) == 0)
	    {
	      httpsocket_reset(httpsock);
	      sched_addmode(&tcpsock->socket, EVENT_SCHED_IN);
	    }
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
