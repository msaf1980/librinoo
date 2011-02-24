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
  rb->headers = rinoo_http_header_createtable();
  if (rb->headers == NULL)
    {
      xfree(rb);
      return NULL;
    }
  return rb;
}

void		rinoo_browser_destroy(t_rinoobrowser *rb)
{
  XASSERTN(rb != NULL);

  if (rb->regexps != NULL)
    {
      list_destroy(rb->regexps);
      rb->regexps = NULL;
    }
  if (rb->post_data != NULL)
    {
      xfree(rb->post_data);
      rb->post_data = NULL;
    }
  rinoo_http_header_destroytable(rb->headers);
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

static void	rinoo_browser_process_regexps(t_rinoobrowser *rb, t_buffer *result)
{
  int			i;
  int			rc;
  u32			offset;
  int			succeed = 0;
  pcre			*cur_regexp;
  t_buffer		cur_substr;
  t_listiterator	listit = NULL;
  int			ovector[RINOO_BROWSER_MAX_SUBSTRINGS];

  XDASSERTN(rb->regexps != NULL);

  while ((cur_regexp = list_getnext(rb->regexps, &listit)) != NULL)
    {
      offset = 0;
      while (offset < buffer_len(result))
	{
	  rc = pcre_exec(cur_regexp,
			 NULL,
			 buffer_ptr(result),
			 buffer_len(result),
			 offset,
			 0,
			 ovector,
			 RINOO_BROWSER_MAX_SUBSTRINGS);

	  if (rc > 0)
	    {
	      for (i = (rc == 1 ? 0 : 1); i < rc; i++)
		{
		  succeed++;
		  cur_substr.buf = buffer_ptr(result) + ovector[2 * i];
		  cur_substr.len = ovector[2 * i + 1] - ovector[2 * i];
		  rb->event_fsm(rb, &cur_substr, EVENT_BROWSER_MATCH);
		}
	      offset = ovector[1];
	    }
	  else
	    {
	      break;
	    }
	}
    }
  rb->event_fsm(rb,
		NULL,
		(succeed == 0 ? EVENT_BROWSER_NOMATCH : EVENT_BROWSER_MATCH_END));
}

static void	rinoo_browser_fsm(t_rinoohttp *httpsock, t_rinoohttp_event event)
{
  t_rinoobrowser	*rb;

  rb = (t_rinoobrowser *) httpsock->data;
  XDASSERTN(rb != NULL);
  switch (event)
    {
    case EVENT_HTTP_CONNECT:
      break;
    case EVENT_HTTP_REQUEST:
      if (rb->post_data != NULL)
	{
	  httpsock->request.method = HTTP_METHOD_POST;
	  httpsock->request.contentlength = strlen(rb->post_data);
	}
      rinoo_http_header_copytable(httpsock->request.headers, rb->headers);
      break;
    case EVENT_HTTP_REQBODY:
      if (rb->post_data != NULL)
	{
	  rinoo_tcp_printdata(httpsock->tcpsock, rb->post_data, strlen(rb->post_data));
	  xfree(rb->post_data);
	  rb->post_data = NULL;
	}
      break;
    case EVENT_HTTP_RESPONSE:
      if (httpsock->response.contentlength == 0)
	{
	  rb->event_fsm(rb, NULL, EVENT_BROWSER_MATCH_END);
	}
      break;
    case EVENT_HTTP_RESPBODY:
      if (httpsock->response.received == httpsock->response.contentlength)
	{
	  if (rb->regexps != NULL)
	    {
	      rinoo_browser_process_regexps(rb, httpsock->tcpsock->socket.rdbuf);
	    }
	  else
	    {
	      rb->event_fsm(rb, httpsock->tcpsock->socket.rdbuf, EVENT_BROWSER_MATCH);
	    }
	}
      break;
    case EVENT_HTTP_ERROR:
      rb->event_fsm(rb, NULL, EVENT_BROWSER_ERROR);
      rb->http = NULL;
      break;
    case EVENT_HTTP_CLOSE:
      rb->http = NULL;
      break;
    case EVENT_HTTP_TIMEOUT:
      rb->event_fsm(rb, NULL, EVENT_BROWSER_ERROR);
      rb->http = NULL;
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
    default:
    case RINOODNS_EVENT_OK:
      rb->ip = ip;
      rb->http = rinoo_http_client(rb->sched,
				       ip,
				       rb->port,
				       rb->timeout,
				       rinoo_browser_fsm);
      rb->http->data = rb;
      buffer_add(rb->http->request.uri,
		 buffer_ptr(&rb->uri),
		 buffer_len(&rb->uri));
      if (rinoo_http_header_adddata(rb->http->request.headers,
				    "Host",
				    buffer_ptr(&rb->host),
				    buffer_len(&rb->host)) != 0)
	{
	  rinoo_http_socket_destroy(rb->http);
	  rb->http = NULL;
	  rb->event_fsm(rb, NULL, EVENT_BROWSER_ERROR);
	  break;
	}
      if (rb->useragent != NULL &&
	  rinoo_http_header_adddata(rb->http->request.headers,
				    "User-Agent",
				    rb->useragent,
				    strlen(rb->useragent)) != 0)
	{
	  rinoo_http_socket_destroy(rb->http);
	  rb->http = NULL;
	  rb->event_fsm(rb, NULL, EVENT_BROWSER_ERROR);
	  break;
	}
      break;
    case RINOODNS_EVENT_ERROR:
      rb->event_fsm(rb, NULL, EVENT_BROWSER_ERROR);
      break;
    }
}

int		rinoo_browser_get(t_rinoobrowser *rb,
				  const char *url,
				  void (*event_fsm)(t_rinoobrowser *rb,
						    t_buffer *result,
						    t_rinoobrowser_event event))
{
  char		host[256];
  t_rinoodns	*rdns;

  XASSERT(rb != NULL, -1);
  XASSERT(rb->sched != NULL, -1);
  XASSERT(url != NULL, -1);
  XASSERT(event_fsm != NULL, -1);

  rb->url = url;
  rb->event_fsm = event_fsm;
  if (rb->http != NULL)
    {
      rinoo_http_socket_destroy(rb->http);
      rb->http = NULL;
    }
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
  if (rb->post_data != NULL)
    {
      xfree(rb->post_data);
      rb->post_data = NULL;
    }
  return 0;
}

int		rinoo_browser_post(t_rinoobrowser *rb,
				   const char *url,
				   const char *post_data,
				   void (*event_fsm)(t_rinoobrowser *rb,
						     t_buffer *result,
						     t_rinoobrowser_event event))
{
  char		host[256];
  t_rinoodns	*rdns;

  XASSERT(rb != NULL, -1);
  XASSERT(rb->sched != NULL, -1);
  XASSERT(url != NULL, -1);
  XASSERT(post_data != NULL, -1);
  XASSERT(event_fsm != NULL, -1);

  rb->url = url;
  rb->event_fsm = event_fsm;
  if (rb->post_data != NULL)
    {
      xfree(rb->post_data);
    }
  rb->post_data = strdup(post_data);
  if (rb->post_data == NULL)
    {
      return -1;
    }
  if (rinoo_parse_url(url, &rb->host, &rb->port, &rb->uri) != 0)
    {
      xfree(rb->post_data);
      rb->post_data = NULL;
      return -1;
    }
  snprintf(host, 256, "%.*s", buffer_len(&rb->host), buffer_ptr(&rb->host));
  rdns = rinoo_resolv(rb->sched, host, RINOODNS_TYPE_A, 0, rinoo_browser_dnsfsm);
  if (rdns == NULL)
    {
      xfree(rb->post_data);
      rb->post_data = NULL;
      return -1;
    }
  rdns->data = rb;
  return 0;
}

static int	rinoo_browser_regexp_cmp(void *node1, void *node2)
{
  if (node1 == node2)
    {
      return 0;
    }
  return 1;
}

int		rinoo_browser_search(t_rinoobrowser *rb, const char *regexp)
{
  pcre		*re;
  const char	*error;
  int		erroffset;

  re = pcre_compile(regexp, PCRE_DOTALL, &error, &erroffset, NULL);
  XASSERTSTR(re != NULL, -1, error);
  if (rb->regexps == NULL)
    {
      rb->regexps = list_create(LIST_SORTED_HEAD, rinoo_browser_regexp_cmp);
      if (rb->regexps == NULL)
	{
	  pcre_free(re);
	  XASSERT(0, -1);
	}
    }
  if (list_add(rb->regexps, re, pcre_free) == NULL)
    {
      pcre_free(re);
      XASSERT(0, -1);
    }
  return 0;
}
