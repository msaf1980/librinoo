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

static void	rinoo_resolv_fsm(t_udpsocket *udpsock, t_udpevent event);
static int	rinoo_dns_query_a(t_udpsocket *udpsock, const char *host);
static int	rinoo_dns_parse_a(t_udpsocket *udpsock,
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
 * @return 0 on success, or -1 if an error occurs.
 */
int		rinoo_resolv(t_sched *sched,
			     const char *host,
			     t_dnstype type,
			     u32 timeout,
			     void (*result_cb)(t_dnsevent event, t_ip ip))
{
  t_dnscontext	*ctx;
  t_udpsocket	*udpsock;

  XASSERT(sched != NULL, -1);
  XASSERT(host != NULL, -1);
  XASSERT(result_cb != NULL, -1);

  /* Using resolv library to get resolver information (see man resolver) */
  XASSERT(res_init() == 0, -1);

  ctx = xcalloc(1, sizeof(*ctx));
  XASSERT(ctx != NULL, -1);
  ctx->host = host;
  ctx->query_type = type;
  ctx->result_cb = result_cb;
  if (timeout == 0)
    {
      timeout = _res.retrans * 1000;
    }
  udpsock = udp_create(sched,
		       _res.nsaddr_list[0].sin_addr.s_addr,
		       ntohs(_res.nsaddr_list[0].sin_port),
		       MODE_UDP_CLIENT,
		       timeout,
		       rinoo_resolv_fsm);
  if (udpsock == NULL)
    {
      xfree(ctx);
      XASSERT(0, -1);
    }
  udpsock->data = ctx;
  return 0;
}

static void	rinoo_resolv_destroy(t_dnscontext *ctx)
{
  xfree(ctx);
}

static void	rinoo_resolv_fsm(t_udpsocket *udpsock, t_udpevent event)
{
  t_ip		ip;
  t_dnscontext	*ctx;

  ctx = (t_dnscontext *) udpsock->data;
  XASSERTN(ctx != NULL);
  switch (event)
    {
    case EVENT_UDP_IN:
      if (rinoo_dns_parse_a(udpsock, ctx->host, &ip) == 0)
	{
	  ctx->result_cb(EVENT_DNS_OK, ip);
	}
      else
	{
	  ctx->result_cb(EVENT_DNS_ERROR, 0);
	}
      udp_destroy(udpsock);
      rinoo_resolv_destroy(ctx);
      break;
    case EVENT_UDP_OUT:
      if (ctx->state != STATE_DNS_SENT)
	{
	  rinoo_dns_query_a(udpsock, ctx->host);
	  ctx->state = STATE_DNS_SENT;
	  sched_addmode(&udpsock->socket, EVENT_SCHED_IN);
	}
      break;
    case EVENT_UDP_TIMEOUT:
      ctx->result_cb(EVENT_DNS_ERROR, 0);
    case EVENT_UDP_CLOSE:
    case EVENT_UDP_ERROR:
      rinoo_resolv_destroy(ctx);
      break;
    }
}

static int	rinoo_dns_query_a(t_udpsocket *udpsock, const char *host)
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

  qid = (unsigned short) ((u64) udpsock % USHRT_MAX);
  udp_printdata(udpsock, (const char *) &qid, 2);
  udp_printdata(udpsock, dns_header, 10);
  while (*host != 0)
    {
      ptr = strchrnul(host, '.');
      tmp = ptr - host;
      udp_printdata(udpsock, &tmp, 1);
      udp_printdata(udpsock, host, (size_t) tmp);
      host += tmp + 1;
    }
  udp_printdata(udpsock, dns_qfooter, 5);
  return 0;
}

static int	rinoo_dns_parse_a(t_udpsocket *udpsock,
				  const char *host,
				  t_ip *ip)
{
  char			*ptr;
  char			*response;
  u32			host_len;
  unsigned short	tmp;
  const char		dns_ainet[] =
    {
      0x00, 0x01, 0x00, 0x01
    };

  host_len = strlen(host);
  if (buffer_len(udpsock->socket.rdbuf) < 12 + host_len + 6 + 16)
    {
      return -1;
    }
  response = buffer_ptr(udpsock->socket.rdbuf);
  tmp = *(unsigned short *) response;
  response += sizeof(tmp);
  if (tmp != (unsigned short) ((u64) udpsock % USHRT_MAX))
    {
      /* Wrong id */
      return -1;
    }
  tmp = ntohs(*(unsigned short *) response);
  response += sizeof(tmp);
  if (tmp != (unsigned short) 0x8180)
    {
      /* Wrong flags */
      return -1;
    }
  tmp = ntohs(*(unsigned short *) response);
  response += sizeof(tmp);
  if (tmp != (unsigned short) 0x0001)
    {
      /* Question number should be 1 */
      return -1;
    }
  tmp = ntohs(*(unsigned short *) response);
  response += sizeof(tmp);
  if (tmp == 0)
    {
      /* No response */
      return -1;
    }
  /* Skip Authority RRs & Additional RRs */
  response += sizeof(tmp) * 2;
  /* Parse question */
  while (*host != 0)
    {
      ptr = strchrnul(host, '.');
      tmp = ptr - host;
      if (*response != tmp)
	{
	  /* Wrong string size */
	  return -1;
	}
      response++;
      if (strncmp(host, response, tmp) != 0)
	{
	  /* Wrong question domain name */
	  return -1;
	}
      response += tmp;
      host += tmp + 1;
    }
  if (*response != 0)
    {
      return -1;
    }
  response++;
  if (memcmp(response, dns_ainet, sizeof(dns_ainet)) != 0)
    {
      return -1;
    }
  response += sizeof(dns_ainet);
  tmp = ntohs(*(unsigned short *) response);
  response += sizeof(tmp);
  if (tmp != (unsigned short) 0xc00c)
    {
      /* Wrong name offset */
      return -1;
    }
  if (memcmp(response, dns_ainet, sizeof(dns_ainet)) != 0)
    {
      return -1;
    }
  response += sizeof(dns_ainet);
  /* Skip TTL */
  response += 4;
  tmp = ntohs(*(unsigned short *) response);
  response += sizeof(tmp);
  if (tmp != 4)
    {
      /* Wrong IP size */
      return -1;
    }
  *ip = *(t_ip *) response;
  return 0;
}
