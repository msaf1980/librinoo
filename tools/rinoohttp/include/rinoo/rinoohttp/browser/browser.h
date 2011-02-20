/**
 * @file   browser.h
 * @author Reginald LIPS <reginald.l@gmail.com>
 * @date   Sun Feb 20 16:12:15 2011
 *
 * @brief  Header file which describes the HTTP browser interface.
 *
 *
 */

#ifndef		RINOOHTTP_BROWSER_BROWSER_H_
# define	RINOOHTTP_BROWSER_BROWSER_H_

typedef enum	e_rinoobrowser_property
  {
    RINOOBROWSER_TIMEOUT = 0,
    RINOOBROWSER_USERAGENT
  }		t_rinoobrowser_property;

typedef struct	s_rinoobrowser
{
  u32		timeout;
  char		*useragent;
  const char	*url;
  t_buffer	host;
  t_ip		ip;
  u32		port;
  t_buffer	uri;
  t_rinoosched	*sched;
  t_rinoohttp	*httpsock;
  void		(*callback)(struct s_rinoobrowser *rb,
			    t_buffer *result);
}		t_rinoobrowser;

t_rinoobrowser		*rinoo_browser(t_rinoosched *sched);
void			rinoo_browser_destroy(t_rinoobrowser *rb);
int			rinoo_browser_setproperty(t_rinoobrowser *rb,
						  t_rinoobrowser_property prop,
						  ...);
int			rinoo_browser_get(t_rinoobrowser *rb,
					  const char *url,
					  void (*callback)(t_rinoobrowser *rb,
							   t_buffer *result));

#endif		/* !RINOOHTTP_BROWSER_BROWSER_H_ */
