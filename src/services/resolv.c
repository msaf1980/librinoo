/**
 * @file   resolv.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Sun Jan 30 16:36:46 2011
 *
 * @brief  RiNOO Resolver Interface
 *
 *
 */

#include	"rinoo/rinoo.h"

static void	rinoo_resolv_fsm(t_rinooudp *udpsock, t_rinooudp_event event);
static int	rinoo_dns_query_a(t_rinooudp *udpsock, const char *host);
static int	rinoo_dns_parse_a(t_rinooudp *udpsock,
				  const char *host,
				  t_ip *ip);

/**
 * Start asynchronous resolving
 *
 * @param sched Pointer to the scheduler to use.
 * @param host Host name to resolv.
 * @param type DNS query type.
 * @param timeout Force timeout. If 0, resolver will use system defaults (see man 5 resolver).
 * @param result_cb Pointer to the callback function to get results.
 *
 * @return A pointer to the new DNS structure, or NULL if an error occurs.
 */
t_rinoodns	*rinoo_resolv(t_rinoosched *sched,
			      const char *host,
			      t_rinoodns_type type,
			      u32 timeout,
			      void (*result_cb)(t_rinoodns *dns,
						t_rinoodns_event event,
						t_ip ip))
{
  t_rinoodns	*rdns;
  t_rinooudp	*udpsock;

  XASSERT(sched != NULL, NULL);
  XASSERT(host != NULL, NULL);
  XASSERT(result_cb != NULL, NULL);

  /* Using resolv library to get resolver information (see man resolver) */
  XASSERT(res_init() == 0, NULL);

  rdns = xcalloc(1, sizeof(*rdns));
  XASSERT(rdns != NULL, NULL);
  rdns->host = strdup(host);
  rdns->query_type = type;
  rdns->result_cb = result_cb;
  if (timeout == 0)
    {
      timeout = _res.retrans * 1000;
    }
  if (_res.nscount <= 0)
    {
      xfree(rdns->host);
      xfree(rdns);
      XASSERT(0, NULL);
    }
  udpsock = rinoo_udp_client(sched,
			     _res.nsaddr_list[0].sin_addr.s_addr,
			     ntohs(_res.nsaddr_list[0].sin_port),
			     timeout,
			     rinoo_resolv_fsm);
  if (udpsock == NULL)
    {
      xfree(rdns->host);
      xfree(rdns);
      XASSERT(0, NULL);
    }
  udpsock->data = rdns;
  return rdns;
}

static void	rinoo_resolv_destroy(t_rinoodns *rdns)
{
  if (rdns->host != NULL)
    {
      xfree(rdns->host);
    }
  xfree(rdns);
}

static void	rinoo_resolv_fsm(t_rinooudp *udpsock, t_rinooudp_event event)
{
  t_ip		ip;
  t_rinoodns	*rdns;

  rdns = (t_rinoodns *) udpsock->data;
  XASSERTN(rdns != NULL);
  switch (event)
    {
    case EVENT_UDP_IN:
      if (rinoo_dns_parse_a(udpsock, rdns->host, &ip) == 0)
	{
	  rdns->result_cb(rdns, RINOODNS_EVENT_OK, ip);
	}
      else
	{
	  rdns->result_cb(rdns, RINOODNS_EVENT_ERROR, 0);
	}
      rinoo_udp_destroy(udpsock);
      rinoo_resolv_destroy(rdns);
      break;
    case EVENT_UDP_OUT:
      if (rdns->state != RINOODNS_STATE_SENT)
	{
	  rinoo_dns_query_a(udpsock, rdns->host);
	  rdns->state = RINOODNS_STATE_SENT;
	  rinoo_sched_socket(RINOO_SCHED_MODADD,
			     &udpsock->socket,
			     EVENT_SCHED_IN);
	}
      break;
    case EVENT_UDP_TIMEOUT:
      rdns->result_cb(rdns, RINOODNS_EVENT_ERROR, 0);
    case EVENT_UDP_CLOSE:
    case EVENT_UDP_ERROR:
      rinoo_resolv_destroy(rdns);
      break;
    }
}

static int	rinoo_dns_query_a(t_rinooudp *udpsock, const char *host)
{
  char			tmp;
  char			*ptr;
  unsigned short	qid;
  const char		dns_header[] =
    {
      0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
  const char		dns_qfooter[] =
    {
      0x00, 0x00, 0x01, 0x00, 0x01
    };

  qid = htons((unsigned short) ((u64) udpsock % USHRT_MAX));
  rinoo_udp_printdata(udpsock, (const char *) &qid, 2);
  rinoo_udp_printdata(udpsock, dns_header, 10);
  while (*host != 0)
    {
      ptr = strchrnul(host, '.');
      tmp = ptr - host;
      rinoo_udp_printdata(udpsock, &tmp, 1);
      rinoo_udp_printdata(udpsock, host, (size_t) tmp);
      host += tmp;
      if (*host == '.')
	{
	  host++;
	}
    }
  rinoo_udp_printdata(udpsock, dns_qfooter, 5);
  return 0;
}

static int	rinoo_dns_getshort(t_buffer *rbuf, unsigned short *result)
{
  unsigned short	res;

  if (buffer_len(rbuf) < sizeof(res))
    {
      return -1;
    }
  res = ntohs(*(unsigned short *) buffer_ptr(rbuf));
  rbuf->buf += sizeof(res);
  rbuf->len -= sizeof(res);
  *result = res;
  return 0;
}

static int	rinoo_dns_checkshort(t_buffer *rbuf, unsigned short expected)
{
  unsigned short	res;

  if (buffer_len(rbuf) < sizeof(res))
    {
      return 0;
    }
  res = ntohs(*(unsigned short *) buffer_ptr(rbuf));
  rbuf->buf += sizeof(res);
  rbuf->len -= sizeof(res);
  return (res == expected);
}

static int		rinoo_dns_checkhost(t_buffer *rbuf, const char *host)
{
  char			*ptr;
  unsigned short	tmp;

  /* Parse question */
  while (*host != 0)
    {
      ptr = strchrnul(host, '.');
      tmp = ptr - host;
      if (buffer_len(rbuf) < 1 || *rbuf->buf != tmp)
	{
	  /* Wrong string size */
	  return 0;
	}
      rbuf->buf++;
      rbuf->len--;
      if (buffer_len(rbuf) < tmp || strncmp(host, rbuf->buf, tmp) != 0)
	{
	  /* Wrong question domain name */
	  return 0;
	}
      rbuf->buf += tmp;
      rbuf->len -= tmp;
      host += tmp;
      if (*host == '.')
	{
	  host++;
	}
    }
  if (buffer_len(rbuf) < 1)
    {
      return 0;
    }
  rbuf->buf++;
  rbuf->len--;
  return 1;
}

static int	rinoo_dns_checkdnsa(t_buffer *rbuf)
{
  const char		dns_ainet[] =
    {
      0x00, 0x01, 0x00, 0x01
    };

  if (buffer_len(rbuf) < sizeof(dns_ainet))
    {
      return 0;
    }
  if (memcmp(buffer_ptr(rbuf), dns_ainet, sizeof(dns_ainet)) != 0)
    {
      return 0;
    }
  rbuf->buf += sizeof(dns_ainet);
  rbuf->len += sizeof(dns_ainet);
  return 1;
}

static int	rinoo_dns_checkdnscname(t_buffer *rbuf)
{
  const char		dns_ainet[] =
    {
      0x00, 0x05, 0x00, 0x01
    };

  if (buffer_len(rbuf) < sizeof(dns_ainet))
    {
      return 0;
    }
  if (memcmp(buffer_ptr(rbuf), dns_ainet, sizeof(dns_ainet)) != 0)
    {
      return 0;
    }
  rbuf->buf += sizeof(dns_ainet);
  rbuf->len += sizeof(dns_ainet);
  return 1;
}

static int	rinoo_dns_parse_a(t_rinooudp *udpsock,
				  const char *host,
				  t_ip *ip)
{
  int			skip;
  unsigned short	tmp;
  unsigned short	offset;
  t_buffer		response;

  response = *udpsock->socket.rdbuf;
  if (rinoo_dns_checkshort(&response, (unsigned short) ((u64) udpsock % USHRT_MAX)) == 0)
    {
      /* Wrong id */
      return -1;
    }
  if (rinoo_dns_checkshort(&response, 0x8180) == 0)
    {
      /* Wrong flags */
      return -1;
    }
  if (rinoo_dns_checkshort(&response, 0x0001) == 0)
    {
      /* Question number should be 1 */
      return -1;
    }
  if (rinoo_dns_getshort(&response, &tmp) != 0 || tmp == 0)
    {
      /* No response */
      return -1;
    }
  /* Skip Authority RRs & Additional RRs */
  if (rinoo_dns_getshort(&response, &tmp) != 0)
    {
      return -1;
    }
  if (rinoo_dns_getshort(&response, &tmp) != 0)
    {
      return -1;
    }

  if (rinoo_dns_checkhost(&response, host) == 0)
    {
      /* Wrong question domain name */
      return -1;
    }

  if (rinoo_dns_checkdnsa(&response) == 0)
    {
      /* Wrong question type */
      return -1;
    }

  offset = 12;
  while (buffer_len(&response) > 0)
    {
      skip = 0;
      if (rinoo_dns_checkshort(&response, 0xc000 | offset) == 0)
	{
	  skip = 1;
	}
      if (rinoo_dns_checkdnsa(&response))
	{
	  /* Skip TTL */
	  if (rinoo_dns_getshort(&response, &tmp) != 0)
	    {
	      return -1;
	    }
	  if (rinoo_dns_getshort(&response, &tmp) != 0)
	    {
	      return -1;
	    }

	  if (rinoo_dns_checkshort(&response, 4) == 0 ||
	      buffer_len(&response) < 4)
	    {
	      /* Wrong IP size */
	      return -1;
	    }
	  if (skip == 0)
	    {
	      *ip = *(t_ip *) response.buf;
	      return 0;
	    }
	  response.buf += 4;
	  response.len += 4;
	}
      else if (rinoo_dns_checkdnscname(&response))
	{
	  /* Skip TTL */
	  if (rinoo_dns_getshort(&response, &tmp) != 0)
	    {
	      return -1;
	    }
	  if (rinoo_dns_getshort(&response, &tmp) != 0)
	    {
	      return -1;
	    }

	  if (rinoo_dns_getshort(&response, &tmp) != 0)
	    {
	      return -1;
	    }
	  if (buffer_len(&response) < tmp)
	    {
	      /* Wrong size */
	      return -1;
	    }
	  if (skip == 0)
	    {
	      if (tmp == 2 &&
		  rinoo_dns_getshort(&response, &tmp) == 0 &&
		  (tmp & 0xc000) == 0xc000)
		{
		  offset = (tmp & 0x00ff);
		}
	      else
		{
		  offset = buffer_ptr(&response) - buffer_ptr(udpsock->socket.rdbuf);
		  response.buf += tmp;
		  response.len -= tmp;
		}
	    }
	  else
	    {
	      response.buf += tmp;
	      response.len -= tmp;
	    }
	}
      else
	{
	  return -1;
	}
    }
  return -1;
}
