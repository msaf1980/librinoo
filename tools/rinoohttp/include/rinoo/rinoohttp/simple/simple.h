/**
 * @file   simple.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Sun Jan 23 22:14:24 2011
 *
 * @brief  Header file which describes the simple http interface
 *
 *
 */

#ifndef		RINOOHTTP_SIMPLE_SIMPLE_H
# define	RINOOHTTP_SIMPLE_SIMPLE_H

typedef struct	s_rinoohttp
{
  t_sched	*sched;
  t_httpsocket	*httpsock;
}		t_rinoohttp;

t_rinoohttp	*rinoo_simplehttp_init();
void		rinoo_simplehttp_destroy(t_rinoohttp *rsh);

int		rinoo_simplehttp_get(t_rinoohttp *rsh, const char *url);

#endif		/* !RINOOHTTP_SIMPLE_SIMPLE_H */
