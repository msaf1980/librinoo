/**
 * @file   rinoo_simplehttp.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Sun Jan 23 22:03:44 2011
 *
 * @brief  RiNOO Simple HTTP interface
 *
 *
 */

#include	"rinoo/rinoohttp.h"

t_rinoohttp	*rinoo_simplehttp_init()
{
  t_rinoohttp	*rsh;

  rsh = xcalloc(1, sizeof(*rsh));
  XASSERT(rsh != NULL, NULL);
  rsh->sched = rinoo_sched();
  if (rsh->sched == NULL)
    {
      xfree(rsh);
      XASSERT(0, NULL);
    }
  return rsh;
}

void		rinoo_simplehttp_destroy(t_rinoohttp *rsh)
{
  XASSERTN(rsh != NULL);

  if (rsh->httpsock != NULL)
    {
      httpsocket_destroy(rsh->httpsock);
    }
  if (rsh->sched != NULL)
    {
      rinoo_sched_destroy(rsh->sched);
    }
  xfree(rsh);
}

static void	rinoo_simplehttp_fsm(t_httpsocket *httpsock, t_httpevent event)
{
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
	  rinoo_sched_stop(((t_rinoohttp *) httpsock->data)->sched);
	}
      break;
    case EVENT_HTTP_RESPBODY:
      rinoo_log("httpc - body len: %d", buffer_len(httpsock->tcpsock->socket.rdbuf));
      if (httpsock->response.received == httpsock->response.contentlength)
	{
	  rinoo_log("httpc - responsebody: %.*s",
		    buffer_len(httpsock->tcpsock->socket.rdbuf),
		    buffer_ptr(httpsock->tcpsock->socket.rdbuf));
	  rinoo_sched_stop(((t_rinoohttp *) httpsock->data)->sched);
	}
      break;
    case EVENT_HTTP_ERROR:
      rinoo_log("httpc - error");
      rinoo_sched_stop(((t_rinoohttp *) httpsock->data)->sched);
      break;
    case EVENT_HTTP_CLOSE:
      rinoo_log("httpc - close");
      rinoo_sched_stop(((t_rinoohttp *) httpsock->data)->sched);
      break;
    case EVENT_HTTP_TIMEOUT:
      rinoo_log("httpc - timeout");
      rinoo_sched_stop(((t_rinoohttp *) httpsock->data)->sched);
      break;
    }
}

int		rinoo_parse_url(const char *url,
				t_buffer *host,
				u32 *port,
				t_buffer *uri)
{
  u32		url_len;
  char		*tmp;

  if (strncmp("http://", url, 7) != 0)
    {
      return -1;
    }
  url = url + 7;
  url_len = strlen(url);
  host->buf = (char *) url;
  url = memchr(url, '/', url_len);
  if (url == NULL)
    {
      host->len = url_len;
      uri->buf = "/";
      uri->len = 1;
    }
  else
    {
      host->len = url - host->buf;
      uri->buf = (char *) url;
      uri->len = url_len - host->len;
    }
  url = memchr(host->buf, ':', host->len);
  if (url == NULL)
    {
      *port = 80;
      return 0;
    }
  host->len = url - host->buf;
  url++;
  *port = strtoul(url, &tmp, 10);
  if (*port == 0 || *port > 65535 || tmp != uri->buf)
    {
      return -1;
    }
  return 0;
}

int		rinoo_simplehttp_get(t_rinoohttp *rsh, const char *url)
{
  t_buffer	host;
  t_buffer	uri;
  u32		port;

  XASSERT(rsh != NULL, -1);
  XASSERT(rsh->sched != NULL, -1);

  if (rinoo_parse_url(url, &host, &port, &uri) != 0)
    {
      return -1;
    }
  rsh->httpsock = httpclient_create(rsh->sched, 0, port, 10000, rinoo_simplehttp_fsm);
  rsh->httpsock->data = rsh;
  buffer_add(rsh->httpsock->request.uri, buffer_ptr(&uri), buffer_len(&uri));
  httpheader_adddata(rsh->httpsock->request.headers,
		     "Host",
		     host.buf,
		     host.len);
  rinoo_sched_loop(rsh->sched);
  /* httpsock should have been destroyed already */
  rsh->httpsock = NULL;
  return 0;
}
