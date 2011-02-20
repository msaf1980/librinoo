/**
 * @file   simple.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Sun Jan 23 22:14:24 2011
 *
 * @brief  Header file which describes the simple http interface
 *
 *
 */

#ifndef		RINOOHTTP_SIMPLE_SIMPLE_H_
# define	RINOOHTTP_SIMPLE_SIMPLE_H_

typedef struct	s_rinoohttp_s
{
  t_rinoosched	*sched;
  t_rinoohttp	*httpsock;
}		t_rinoohttp_s;

t_rinoohttp_s	*rinoo_simplehttp_init();
void		rinoo_simplehttp_destroy(t_rinoohttp_s *rsh);
int		rinoo_parse_url(const char *url,
				t_buffer *host,
				u32 *port,
				t_buffer *uri);
int		rinoo_simplehttp_get(t_rinoohttp_s *rsh, const char *url);

#endif		/* !RINOOHTTP_SIMPLE_SIMPLE_H_ */
