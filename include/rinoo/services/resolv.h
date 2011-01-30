/**
 * @file   resolv.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Sun Jan 30 17:29:39 2011
 *
 * @brief  Header file for resolv function declarations
 *
 *
 */

#ifndef		RINOO_RESOLV_H_
# define	RINOO_RESOLV_H_

typedef enum	e_dnstype
  {
    TYPE_DNS_A = 1,
    TYPE_DNS_NS,
    TYPE_DNS_MD,
    TYPE_DNS_MF,
    TYPE_DNS_CNAME,
    TYPE_DNS_SOA,
    TYPE_DNS_MB,
    TYPE_DNS_MG,
    TYPE_DNS_MR,
    TYPE_DNS_NULL,
    TYPE_DNS_WKS,
    TYPE_DNS_PTR,
    TYPE_DNS_HINFO,
    TYPE_DNS_MINFO,
    TYPE_DNS_MX,
    TYPE_DNS_TXT
  }		t_dnstype;

typedef enum	e_dnsevent
  {
    EVENT_DNS_OK = 0,
    EVENT_DNS_ERROR
  }		t_dnsevent;

typedef enum	e_dnsstate
  {
    STATE_DNS_NONE = 0,
    STATE_DNS_SENT
  }		t_dnsstate;

typedef struct	s_dnscontext
{
  t_dnsstate	state;
  const char	*host;
  t_dnstype	query_type;
  void (*result_cb)(t_dnsevent event, t_ip ip);
}		t_dnscontext;

int		rinoo_resolv(t_sched *sched,
			     const char *host,
			     t_dnstype type,
			     void (*result_cb)(t_dnsevent event, t_ip ip));

#endif		/* !RINOO_RESOLV_H_ */
