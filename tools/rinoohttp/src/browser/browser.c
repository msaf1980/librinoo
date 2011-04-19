/**
 * @file   browser.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
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
  rb->timeout = 15000;
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
  if (rb->http != NULL)
    {
      rinoo_http_socket_destroy(rb->http);
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

static void	rinoo_browser_call_fsm(t_rinoobrowser *rb,
				       t_rinoobrowser_event event,
				       t_buffer *result,
				       u32 nbresults,
				       void *arg)
{
  if (rb->done == 0)
    {
      rb->event_fsm(rb,
		    event,
		    result,
		    nbresults,
		    arg);
      switch (event)
	{
	case EVENT_BROWSER_MATCH:
	case EVENT_BROWSER_NOMATCH:
	  break;
	case EVENT_BROWSER_ERROR:
	case EVENT_BROWSER_MATCH_END:
	  rb->done = 1;
	  break;
	}
    }
}

static void	rinoo_browser_process_regexps(t_rinoobrowser *rb, t_buffer *result)
{
  int			i;
  int			rc;
  u32			offset;
  int			succeed;
  u32			nbsubstrs;
  t_listiterator	listit = NULL;
  t_rinoobrowser_search	*cur_search;
  int			ovector[RINOO_BROWSER_MAX_SUBSTRINGS];
  t_buffer		substrs[RINOO_BROWSER_MAX_SUBSTRINGS];

  XDASSERTN(rb->regexps != NULL);

  while ((cur_search = list_getnext(rb->regexps, &listit)) != NULL)
    {
      offset = 0;
      succeed = 0;
      while (offset < buffer_len(result))
	{
	  rc = pcre_exec(cur_search->regexp,
			 NULL,
			 buffer_ptr(result),
			 buffer_len(result),
			 offset,
			 0,
			 ovector,
			 RINOO_BROWSER_MAX_SUBSTRINGS);

	  if (rc > 0)
	    {
	      memset(substrs, 0, sizeof(substrs));
	      for (i = (rc == 1 ? 0 : 1), nbsubstrs = 0; i < rc; i++)
		{
		  buffer_static(substrs[nbsubstrs],
				buffer_ptr(result) + ovector[2 * i],
				ovector[2 * i + 1] - ovector[2 * i]);
		  succeed++;
		  nbsubstrs++;
		}
	      offset = ovector[1];
	      if (nbsubstrs > 0)
		{
		  rinoo_browser_call_fsm(rb,
				EVENT_BROWSER_MATCH,
				substrs,
				nbsubstrs,
				cur_search->arg);
		}
	    }
	  else
	    {
	      break;
	    }
	}
      if (succeed == 0)
	{
	  rinoo_browser_call_fsm(rb, EVENT_BROWSER_NOMATCH, NULL, 0, cur_search->arg);
	}
    }
  rinoo_browser_call_fsm(rb, EVENT_BROWSER_MATCH_END, NULL, 0, NULL);
}

static void	rinoo_browser_fsm(t_rinoohttp *httpsock, t_rinoohttp_event event)
{
  t_rinoobrowser	*rb;

  rb = (t_rinoobrowser *) httpsock->data;
  XDASSERTN(rb != NULL);
  XDASSERTN(rb->http != NULL);
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
	  rinoo_browser_call_fsm(rb, EVENT_BROWSER_MATCH_END, NULL, 0, NULL);
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
	      rinoo_browser_call_fsm(rb,
				     EVENT_BROWSER_MATCH,
				     httpsock->tcpsock->socket.rdbuf,
				     1,
				     NULL);
	      rinoo_browser_call_fsm(rb,
				     EVENT_BROWSER_MATCH_END,
				     NULL,
				     0,
				     NULL);
	    }
	}
      break;
    case EVENT_HTTP_ERROR:
      rinoo_browser_call_fsm(rb, EVENT_BROWSER_ERROR, NULL, 0, NULL);
      rb->http = NULL;
      break;
    case EVENT_HTTP_CLOSE:
      /* CLOSE is an error if request is not done (see rinoo_browser_call_fsm) */
      rinoo_browser_call_fsm(rb, EVENT_BROWSER_ERROR, NULL, 0, NULL);
      rb->http = NULL;
      break;
    case EVENT_HTTP_TIMEOUT:
      rinoo_browser_call_fsm(rb, EVENT_BROWSER_ERROR, NULL, 0, NULL);
      rb->http = NULL;
      break;
    }
}

static void		rinoo_browser_generate_request(t_rinoobrowser *rb)
{
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
      rinoo_browser_call_fsm(rb, EVENT_BROWSER_ERROR, NULL, 0, NULL);
      return;
    }
  if (rb->useragent != NULL &&
      rinoo_http_header_adddata(rb->http->request.headers,
				"User-Agent",
				rb->useragent,
				strlen(rb->useragent)) != 0)
    {
      rinoo_http_socket_destroy(rb->http);
      rb->http = NULL;
      rinoo_browser_call_fsm(rb, EVENT_BROWSER_ERROR, NULL, 0, NULL);
      return;
    }
  rinoo_socket_timeout_reset(&rb->http->tcpsock->socket);
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
      rb->http = rinoo_http_client(rb->sched,
				   rb->ip,
				   rb->port,
				   rb->timeout,
				   rinoo_browser_fsm);
      rinoo_browser_generate_request(rb);
      break;
    case RINOODNS_EVENT_ERROR:
      rinoo_browser_call_fsm(rb, EVENT_BROWSER_ERROR, NULL, 0, NULL);
      break;
    }
}

int		rinoo_browser_get(t_rinoobrowser *rb,
				  const char *url,
				  void (*event_fsm)(t_rinoobrowser *rb,
						    t_rinoobrowser_event event,
						    t_buffer *result,
						    u32 nbresults,
						    void *arg))
{
  u32		port;
  t_buffer	hostbuf;
  char		host[256];
  t_rinoodns	*rdns;

  XASSERT(rb != NULL, -1);
  XASSERT(rb->sched != NULL, -1);
  XASSERT(url != NULL, -1);
  XASSERT(event_fsm != NULL, -1);

  rb->url = url;
  rb->event_fsm = event_fsm;
  if (rinoo_parse_url(url, &hostbuf, &port, &rb->uri) != 0)
    {
      return -1;
    }
  if (rb->post_data != NULL)
    {
      xfree(rb->post_data);
      rb->post_data = NULL;
    }
  if (rb->http != NULL)
    {
      if (rb->port == port &&
	  buffer_len(&rb->host) == buffer_len(&hostbuf) &&
	  memcmp(buffer_ptr(&rb->host), buffer_ptr(&hostbuf), buffer_len(&hostbuf)) == 0 &&
	  rb->done == 1)
	{
	  rb->done = 0;
	  rinoo_sched_socket(RINOO_SCHED_MODADD, &rb->http->tcpsock->socket, EVENT_SCHED_OUT);
	  rinoo_browser_generate_request(rb);
	  return 0;
	}
      rinoo_http_socket_destroy(rb->http);
      rb->http = NULL;
    }
  rb->done = 0;
  rb->host = hostbuf;
  rb->port = port;
  snprintf(host, 256, "%.*s", buffer_len(&rb->host), buffer_ptr(&rb->host));
  rdns = rinoo_resolv(rb->sched, host, RINOODNS_TYPE_A, 0, rinoo_browser_dnsfsm);
  if (rdns == NULL)
    {
      return -1;
    }
  rdns->data = rb;
  return 0;
}

int		rinoo_browser_post(t_rinoobrowser *rb,
				   const char *url,
				   const char *post_data,
				   void (*event_fsm)(t_rinoobrowser *rb,
						     t_rinoobrowser_event event,
						     t_buffer *result,
						     u32 nbresults,
						     void *arg))
{
  u32		port;
  t_buffer	hostbuf;
  char		host[256];
  t_rinoodns	*rdns;

  XASSERT(rb != NULL, -1);
  XASSERT(rb->sched != NULL, -1);
  XASSERT(url != NULL, -1);
  XASSERT(post_data != NULL, -1);
  XASSERT(event_fsm != NULL, -1);

  rb->url = url;
  rb->event_fsm = event_fsm;
  if (rinoo_parse_url(url, &hostbuf, &port, &rb->uri) != 0)
    {
      return -1;
    }
  if (rb->post_data != NULL)
    {
      xfree(rb->post_data);
    }
  rb->post_data = strdup(post_data);
  if (rb->post_data == NULL)
    {
      return -1;
    }
  if (rb->http != NULL)
    {
      if (rb->port == port &&
	  buffer_len(&rb->host) == buffer_len(&hostbuf) &&
	  memcmp(buffer_ptr(&rb->host), buffer_ptr(&hostbuf), buffer_len(&hostbuf)) == 0 &&
	  rb->done == 1)
	{
	  rb->done = 0;
	  rinoo_sched_socket(RINOO_SCHED_MODADD, &rb->http->tcpsock->socket, EVENT_SCHED_OUT);
	  rinoo_browser_generate_request(rb);
	  return 0;
	}
      rinoo_http_socket_destroy(rb->http);
      rb->http = NULL;
    }
  rb->done = 0;
  rb->host = hostbuf;
  rb->port = port;
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

static void		rinoo_browser_search_destroy(void *data)
{
  XASSERTN(data != NULL);

  pcre_free(((t_rinoobrowser_search *) data)->regexp);
  xfree(data);
}

int			rinoo_browser_search(t_rinoobrowser *rb,
					     const char *regexp,
					     void *arg)
{
  pcre			*re;
  const char		*error;
  int			erroffset;
  t_rinoobrowser_search	*newsearch;

  re = pcre_compile(regexp, PCRE_DOTALL, &error, &erroffset, NULL);
  XASSERTSTR(re != NULL, -1, error);
  if (rb->regexps == NULL)
    {
      rb->regexps = list_create(LIST_SORTED_HEAD, rinoo_browser_regexp_cmp);
      if (rb->regexps == NULL)
	{
	  pcre_free(re);
	  return -1;
	}
    }
  newsearch = xcalloc(1, sizeof(*newsearch));
  if (newsearch == NULL)
    {
      pcre_free(re);
      return -1;
    }
  newsearch->regexp = re;
  newsearch->arg = arg;
  if (list_add(rb->regexps, newsearch, rinoo_browser_search_destroy) == NULL)
    {
      rinoo_browser_search_destroy(newsearch);
    }
  return 0;
}
