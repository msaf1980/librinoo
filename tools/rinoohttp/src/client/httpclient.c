/**
 * @file   httpclient.c
 * @author Reginald LIPS <reginald.l@gmail.com>
 * @date   Tue Jan 18 21:49:56 2011
 *
 * @brief  RinOO HTTP client library
 *
 *
 */


#include	"rinoo/rinoohttp.h"

static void	httpclient_fsm(t_tcpsocket *tcpsock, t_tcpevent event);

t_httpsocket	*httpclient_create(t_sched *sched,
				   t_ip ip,
				   u32 port,
				   u32 timeout,
				   void (*event_fsm)(t_httpsocket *httpsock,
						     t_httpevent event))
{
  t_tcpsocket	*clientsock;
  t_httpsocket	*httpsock;

  XDASSERT(sched != NULL, NULL);

  clientsock = tcp_create(sched,
			  ip,
			  port,
			  MODE_TCP_CLIENT,
			  timeout,
			  httpclient_fsm);
  XASSERT(clientsock != NULL, NULL);
  httpsock = httpsocket_create(clientsock, event_fsm);
  XASSERT(httpsock != NULL, NULL);
  return httpsock;
}

void		httpclient_generate_request(t_httpsocket *httpsock)
{
  char			*method;
  char			*version;
  t_httpheader		*header;
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

  httprequest_setdefaultheaders(httpsock);

  tcp_print(httpsock->tcpsock,
	    "%s %.*s %s\r\n",
	    method,
	    buffer_len(httpsock->request.uri),
	    buffer_ptr(httpsock->request.uri),
	    version);

  while ((header = (t_httpheader *) hashtable_getnext(httpsock->request.headers,
						      &iterator)) != NULL)
    {
      tcp_print(httpsock->tcpsock,
		"%.*s: %.*s\r\n",
		buffer_len(&header->key),
		buffer_ptr(&header->key),
		buffer_len(&header->value),
		buffer_ptr(&header->value));
    }
  tcp_print(httpsock->tcpsock, "\r\n");
}

static void	httpclient_fsm(t_tcpsocket *tcpsock, t_tcpevent event)
{
  t_httpsocket	*httpsock;

  httpsock = (t_httpsocket *) tcpsock->data;
  switch (event)
    {
    case EVENT_TCP_CONNECT:
      XASSERTN(httpsock != NULL);
      httpsock->event_fsm(httpsock, EVENT_HTTP_CONNECT);
      httpsock->last_event = EVENT_HTTP_CONNECT;
      sched_delmode(&tcpsock->socket, EVENT_SCHED_IN);
      sched_addmode(&tcpsock->socket, EVENT_SCHED_OUT);
      break;
    case EVENT_TCP_IN:
      switch (httpsock->last_event)
	{
	case EVENT_HTTP_REQUEST:
	case EVENT_HTTP_REQBODY:
	  switch(httpresponse_read(httpsock))
	    {
	    case 0:
	      return;
	    case -1:
	      /* bad response */
	      httpsocket_destroy(httpsock);
	      return;
	    case 1:
	      httpsock->event_fsm(httpsock, EVENT_HTTP_RESPONSE);
	      httpsock->last_event = EVENT_HTTP_RESPONSE;
	      if (httpsock->response.length < buffer_len(tcpsock->socket.rdbuf))
		{
		  buffer_erase(tcpsock->socket.rdbuf, httpsock->response.length);
		  switch (httpresponse_readbody(httpsock))
		    {
		    case 0:
		      httpsock->event_fsm(httpsock, EVENT_HTTP_RESPBODY);
		      httpsock->last_event = EVENT_HTTP_RESPBODY;
		      break;
		    case -1:
		      /* bad request */
		      httpsocket_destroy(httpsock);
		      return;
		    case 1:
		      httpsock->event_fsm(httpsock, EVENT_HTTP_RESPBODY);
		      httpsock->last_event = EVENT_HTTP_RESPBODY;
		      sched_delmode(&tcpsock->socket, EVENT_SCHED_IN);
		      sched_addmode(&tcpsock->socket, EVENT_SCHED_OUT);
		      httpsock->last_event = EVENT_HTTP_CONNECT;
		      break;
		    }
		}
	      break;
	    }
	case EVENT_HTTP_RESPONSE:
	case EVENT_HTTP_RESPBODY:
	  switch (httprequest_readbody(httpsock))
	    {
	    case 0:
	      httpsock->event_fsm(httpsock, EVENT_HTTP_RESPBODY);
	      httpsock->last_event = EVENT_HTTP_RESPBODY;
	      break;
	    case -1:
	      /* bad request */
	      httpsocket_destroy(httpsock);
	      return;
	    case 1:
	      httpsock->event_fsm(httpsock, EVENT_HTTP_RESPBODY);
	      httpsock->last_event = EVENT_HTTP_RESPBODY;
	      sched_delmode(&tcpsock->socket, EVENT_SCHED_IN);
	      sched_addmode(&tcpsock->socket, EVENT_SCHED_OUT);
	      httpsock->last_event = EVENT_HTTP_CONNECT;
	      break;
	    }
	  break;
	default:
	  break;
	}
      buffer_erase(tcpsock->socket.rdbuf, buffer_len(tcpsock->socket.rdbuf));
      break;
    case EVENT_TCP_OUT:
      switch (httpsock->last_event)
	{
	case EVENT_HTTP_CONNECT:
	  httpsock->event_fsm(httpsock, EVENT_HTTP_REQUEST);
	  httpsock->last_event = EVENT_HTTP_REQUEST;
	  httpclient_generate_request(httpsock);
	  /* no break */
	case EVENT_HTTP_REQUEST:
	case EVENT_HTTP_REQBODY:
	  httpsock->event_fsm(httpsock, EVENT_HTTP_REQBODY);
	  httpsock->last_event = EVENT_HTTP_REQBODY;
	  if (buffer_len(tcpsock->socket.wrbuf) == 0)
	    {
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
