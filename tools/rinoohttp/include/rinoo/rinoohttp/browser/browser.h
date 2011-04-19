/**
 * @file   browser.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Sun Feb 20 16:12:15 2011
 *
 * @brief  Header file which describes the HTTP browser interface.
 *
 *
 */

#ifndef		RINOOHTTP_BROWSER_BROWSER_H_
# define	RINOOHTTP_BROWSER_BROWSER_H_

# define	RINOO_BROWSER_MAX_SUBSTRINGS	30 /* Should be a multiple of 3 (see pcre man page) */

typedef enum	e_rinoobrowser_property
  {
    RINOOBROWSER_TIMEOUT = 0,
    RINOOBROWSER_USERAGENT
  }		t_rinoobrowser_property;

typedef enum	e_rinoobrowser_event
  {
    EVENT_BROWSER_ERROR = 0,
    EVENT_BROWSER_NOMATCH,
    EVENT_BROWSER_MATCH,
    EVENT_BROWSER_MATCH_END
  }		t_rinoobrowser_event;

typedef struct	s_rinoobrowser
{
  u32		timeout;
  char		*useragent;
  const char	*url;
  t_buffer	host;
  t_ip		ip;
  u32		port;
  t_buffer	uri;
  u32		done;
  t_rinoosched	*sched;
  t_rinoohttp	*http;
  t_list	*regexps;
  char		*post_data;
  t_hashtable	*headers;
  void		(*event_fsm)(struct s_rinoobrowser *rb,
			     t_rinoobrowser_event event,
			     t_buffer *result,
			     u32 nbresults,
			     void *arg);
}		t_rinoobrowser;

typedef struct	s_rinoobrowser_search
{
  pcre		*regexp;
  void		*arg;
}		t_rinoobrowser_search;

t_rinoobrowser		*rinoo_browser(t_rinoosched *sched);
void			rinoo_browser_destroy(t_rinoobrowser *rb);
int			rinoo_browser_setproperty(t_rinoobrowser *rb,
						  t_rinoobrowser_property prop,
						  ...);
int			rinoo_browser_get(t_rinoobrowser *rb,
					  const char *url,
					  void (*event_fsm)(t_rinoobrowser *rb,
							    t_rinoobrowser_event event,
							    t_buffer *result,
							    u32 nbresults,
							    void *arg));
int			rinoo_browser_post(t_rinoobrowser *rb,
					   const char *url,
					   const char *post_data,
					   void (*event_fsm)(t_rinoobrowser *rb,
							     t_rinoobrowser_event event,
							     t_buffer *result,
							     u32 nbresults,
							     void *arg));
int			rinoo_browser_search(t_rinoobrowser *rb,
					     const char *regexp,
					     void *arg);

#endif		/* !RINOOHTTP_BROWSER_BROWSER_H_ */
