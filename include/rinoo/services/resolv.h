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

typedef enum	e_rinoodns_type
  {
    RINOODNS_TYPE_A = 1,
    RINOODNS_TYPE_NS,
    RINOODNS_TYPE_MD,
    RINOODNS_TYPE_MF,
    RINOODNS_TYPE_CNAME,
    RINOODNS_TYPE_SOA,
    RINOODNS_TYPE_MB,
    RINOODNS_TYPE_MG,
    RINOODNS_TYPE_MR,
    RINOODNS_TYPE_NULL,
    RINOODNS_TYPE_WKS,
    RINOODNS_TYPE_PTR,
    RINOODNS_TYPE_HINFO,
    RINOODNS_TYPE_MINFO,
    RINOODNS_TYPE_MX,
    RINOODNS_TYPE_TXT
  }		t_rinoodns_type;

typedef enum	e_rinoodns_event
  {
    RINOODNS_EVENT_OK = 0,
    RINOODNS_EVENT_ERROR
  }		t_rinoodns_event;

typedef enum	e_rinoodns_state
  {
    RINOODNS_STATE_NONE = 0,
    RINOODNS_STATE_SENT
  }		t_rinoodns_state;

typedef struct		s_rinoodns
{
  t_rinoodns_state	state;
  char			*host;
  t_rinoodns_type	query_type;
  void			*data;
  void			(*result_cb)(struct s_rinoodns *rdns,
				     t_rinoodns_event event,
				     t_ip ip);
}			t_rinoodns;

t_rinoodns	*rinoo_resolv(t_rinoosched *sched,
			      const char *host,
			      t_rinoodns_type type,
			      u32 timeout,
			      void (*result_cb)(t_rinoodns *rdns,
						t_rinoodns_event event,
						t_ip ip));

#endif		/* !RINOO_RESOLV_H_ */
