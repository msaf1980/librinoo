/**
 * @file   rinoo_browser.c
 * @author Reginald LIPS <reginald.l@gmail.com>
 * @date   Sun Feb 20 16:07:38 2011
 *
 * @brief  RiNOO HTTP Browser Interface
 *
 *
 */

#include	"rinoo/rinoohttp.h"

t_rinoobrowser		*rinoo_browser(t_rinoosched *sched)
{
  t_rinoobrowser	*rb;

  XASSERT(sched != NULL, NULL);

  rb = xcalloc(1, sizeof(*rb));
  XASSERT(rb != NULL, NULL);
  rb->sched = sched;
  rb->timeout = 10000;
  return rb;
}

void		rinoo_browser_destroy(t_rinoobrowser *rb)
{
  XASSERTN(rb != NULL);

  xfree(rb);
}

int		rinoo_browser_setproperty(t_rinoobrowser *rb,
					  t_rinoobrowser_property prop,
					  ...)
{
  va_list	ap;

  va_start(ap, prop);
  switch (prop)
    {
    case RINOOBROWSER_TIMEOUT:
      rb->timeout = va_arg(ap, u32);
      break;
    case RINOOBROWSER_USERAGENT:
      rb->useragent = va_arg(ap, char *);
      break;
    }
  va_end(ap);
  return 0;
}

static void	rinoo_browser_fsm(t_rinoohttp *httpsock, t_rinoohttp_event event)
{
  t_rinoobrowser	*rb;

  rb = (t_rinoobrowser *) httpsock->data;
  XDASSERTN(rb != NULL);
  switch (event)
    {
    case EVENT_HTTP_CONNECT:
      rinoo_log("httpc - connect");
      break;
    case EVENT_HTTP_REQUEST:
      rinoo_log("httpc - request");
      break;
    case EVENT_HTTP_REQBODY:
      rinoo_log("httpc - requestbody");
      break;
    case EVENT_HTTP_RESPONSE:
      rinoo_log("httpc - response code: %d - %.*s - %d",
		httpsock->response.code,
		buffer_len(&httpsock->response.msg),
		buffer_ptr(&httpsock->response.msg),
		httpsock->response.contentlength);
      if (httpsock->response.contentlength == 0)
	{
	  rb->callback(rb, NULL);
	}
      break;
    case EVENT_HTTP_RESPBODY:
      rinoo_log("httpc - body len: %d", buffer_len(httpsock->tcpsock->socket.rdbuf));
      if (httpsock->response.received == httpsock->response.contentlength)
	{
	  /* rinoo_log("httpc - responsebody: %.*s", */
	  /* 	    buffer_len(httpsock->tcpsock->socket.rdbuf), */
	  /* 	    buffer_ptr(httpsock->tcpsock->socket.rdbuf)); */
	  rb->callback(rb, httpsock->tcpsock->socket.rdbuf);
	}
      break;
    case EVENT_HTTP_ERROR:
      rinoo_log("httpc - error");
      rb->callback(rb, NULL);
      break;
    case EVENT_HTTP_CLOSE:
      rinoo_log("httpc - close");
      break;
    case EVENT_HTTP_TIMEOUT:
      rinoo_log("httpc - timeout");
      rb->callback(rb, NULL);
      break;
    }
}

static void		rinoo_browser_dnsfsm(t_rinoodns *rdns,
					     t_rinoodns_event event,
					     t_ip ip)
{
  t_rinoobrowser	*rb;

  rb = (t_rinoobrowser *) rdns->data;
  XDASSERTN(rb != NULL);
  switch (event)
    {
    case RINOODNS_EVENT_OK:
      rb->ip = ip;
      rb->httpsock = rinoo_http_client(rb->sched,
				       ip,
				       rb->port,
				       rb->timeout,
				       rinoo_browser_fsm);
      rb->httpsock->data = rb;
      buffer_add(rb->httpsock->request.uri,
		 buffer_ptr(&rb->uri),
		 buffer_len(&rb->uri));
      if (rinoo_http_header_adddata(rb->httpsock->request.headers,
				    "Host",
				    buffer_ptr(&rb->host),
				    buffer_len(&rb->host)) != 0)
	{
	  rinoo_http_socket_destroy(rb->httpsock);
	  rb->httpsock = NULL;
	  rb->callback(rb, NULL);
	  break;
	}
      if (rb->useragent != NULL &&
	  rinoo_http_header_adddata(rb->httpsock->request.headers,
				    "User-Agent",
				    rb->useragent,
				    strlen(rb->useragent)) != 0)
	{
	  rinoo_http_socket_destroy(rb->httpsock);
	  rb->httpsock = NULL;
	  rb->callback(rb, NULL);
	  break;
	}
      break;
    case RINOODNS_EVENT_ERROR:
      rb->callback(rb, NULL);
      break;
    }
}

int		rinoo_browser_get(t_rinoobrowser *rb,
				  const char *url,
				  void (*callback)(t_rinoobrowser *rb,
						   t_buffer *result))
{
  char		host[256];
  t_rinoodns	*rdns;

  XASSERT(rb != NULL, -1);
  XASSERT(rb->sched != NULL, -1);
  XASSERT(callback != NULL, -1);

  rb->url = url;
  rb->callback = callback;
  if (rinoo_parse_url(url, &rb->host, &rb->port, &rb->uri) != 0)
    {
      return -1;
    }
  snprintf(host, 256, "%.*s", buffer_len(&rb->host), buffer_ptr(&rb->host));
  rdns = rinoo_resolv(rb->sched, host, RINOODNS_TYPE_A, 0, rinoo_browser_dnsfsm);
  if (rdns == NULL)
    {
      return -1;
    }
  rdns->data = rb;
  return 0;
}
