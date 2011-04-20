/**
 * @file   httpeasysend.c
 * @author Reginald LIPS <reginald.l@gmail.com>
 * @date   Mon Mar  7 00:01:31 2011
 *
 * @brief  HTTP Server easy send interface.
 *
 *
 */

#include	"rinoo/rinoohttp.h"

static void			rinoo_http_easysend_fsm(t_rinoohttp *httpsock,
							t_rinoohttp_event event);

t_rinoohttp_send_ctx	*rinoo_http_easysend(t_rinoohttp *httpsock)
{
  t_rinoohttp_send_ctx	*sctx;

  XASSERT(httpsock != NULL, NULL);

  sctx = xcalloc(1, sizeof(*sctx));
  XASSERT(sctx != NULL, NULL);
  sctx->httpsock = httpsock;
  sctx->orig_data = httpsock->data;
  sctx->orig_event_fsm = httpsock->event_fsm;
  httpsock->data = sctx;
  httpsock->event_fsm = rinoo_http_easysend_fsm;
  return sctx;
}

void		rinoo_http_easysend_destroy(t_rinoohttp_send_ctx *sctx)
{
  XASSERTN(sctx != NULL);
  XASSERTN(sctx->httpsock != NULL);

  sctx->httpsock->data = sctx->orig_data;
  sctx->httpsock->event_fsm = sctx->orig_event_fsm;
  if (sctx->free_func != NULL)
    {
      sctx->free_func(sctx->data);
    }
  xfree(sctx);
}

static void			rinoo_http_easysend_fsm(t_rinoohttp *httpsock,
							t_rinoohttp_event event)
{
  t_rinoohttp_send_ctx		*sctx;

  sctx = (t_rinoohttp_send_ctx *) httpsock->data;
  XASSERTN(sctx != NULL);
  XASSERTN(sctx->tosend != NULL);
  switch (event)
    {
    case EVENT_HTTP_CONNECT:
    case EVENT_HTTP_REQUEST:
    case EVENT_HTTP_REQBODY:
    case EVENT_HTTP_RESPONSE:
      break;
    case EVENT_HTTP_RESPBODY:
      if (sctx->offset >= buffer_len(sctx->tosend))
	{
	  rinoo_http_easysend_destroy(sctx);
	  return;
	}
      if (buffer_len(sctx->tosend) - sctx->offset > 2048)
	{
	  rinoo_tcp_printdata(httpsock->tcpsock,
			      buffer_ptr(sctx->tosend) + sctx->offset,
			      2048);
	  sctx->offset += 2048;
	}
      else
	{
	  rinoo_tcp_printdata(httpsock->tcpsock,
			      buffer_ptr(sctx->tosend) + sctx->offset,
			      buffer_len(sctx->tosend) - sctx->offset);
	  sctx->offset = buffer_len(sctx->tosend);
	}
      break;
    case EVENT_HTTP_ERROR:
    case EVENT_HTTP_CLOSE:
    case EVENT_HTTP_TIMEOUT:
      rinoo_http_easysend_destroy(sctx);
      httpsock->event_fsm(httpsock, event);
      break;
    }
}

static void	rinoo_http_easysend_buffer_destroy(void *data)
{
  buffer_destroy((t_buffer *) data);
}

int		rinoo_http_easysend_buffer(t_rinoohttp *httpsock, t_buffer *buffer)
{
  t_rinoohttp_send_ctx	*sctx;

  XASSERT(httpsock != NULL, -1);

  if (buffer_len(buffer) == 0)
    {
      httpsock->response.code = 200;
      httpsock->response.contentlength = 0;
      buffer_destroy(buffer);
      return 0;
    }
  sctx = rinoo_http_easysend(httpsock);
  XASSERT(sctx != NULL, -1);
  sctx->tosend = buffer;
  sctx->data = buffer;
  sctx->free_func = rinoo_http_easysend_buffer_destroy;
  httpsock->response.code = 200;
  httpsock->response.contentlength = buffer_len(sctx->tosend);
  return 0;
}

int		rinoo_http_easysend_print(t_rinoohttp *httpsock,
					  const char *format, ...)
{
  int			res;
  va_list		ap;
  t_buffer		*response;

  XASSERT(httpsock != NULL, -1);

  response = buffer_create(1024, BUFFER_DEFAULT_MAXSIZE);
  if (response == NULL)
    {
      return -1;
    }
  va_start(ap, format);
  res = buffer_vprint(response, format, ap);
  va_end(ap);
  if (res < 0)
    {
      buffer_destroy(response);
      return -1;
    }
  res = rinoo_http_easysend_buffer(httpsock, response);
  if (res < 0)
    {
      buffer_destroy(response);
      return -1;
    }
  return 0;
}

int		rinoo_http_easysend_file(t_rinoohttp *httpsock, const char *path)
{
  t_rinoohttp_file	*httpfile;
  t_rinoohttp_send_ctx	*sctx;

  XASSERT(httpsock != NULL, -1);
  XASSERT(path != NULL, -1);

  sctx = rinoo_http_easysend(httpsock);
  XASSERT(sctx != NULL, -1);
  httpfile = rinoo_http_file_open(sctx, path);
  if (httpfile == NULL)
    {
      rinoo_http_easysend_destroy(sctx);
      return -1;
    }
  sctx->data = httpfile;
  sctx->free_func = rinoo_http_file_destroy;
  httpsock->response.code = 200;
  httpsock->response.contentlength = buffer_len(sctx->tosend);
  return 0;
}
