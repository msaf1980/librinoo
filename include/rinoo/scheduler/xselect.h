/**
 * @file   xselect.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Jan 27 11:38:57 2010
 *
 * @brief  Header file for the select functions declarations.
 *
 *
 */

#ifndef		RINOO_XSELECT_H_
# define	RINOO_XSELECT_H_

# include	<sys/select.h>

typedef struct	s_rinooselect
{
  int		nfds;
  fd_set	readfds;
  fd_set	writefds;
  sigset_t	sigmask;
}		t_rinooselect;

int		xselect_init(t_rinoosched *sched);
void		xselect_destroy(t_rinoosched *sched);
int		xselect_insert(t_rinoosocket *socket,
			       t_rinoosched_event mode);
int		xselect_addmode(t_rinoosocket *socket,
				t_rinoosched_event mode);
int		xselect_delmode(t_rinoosocket *socket,
				t_rinoosched_event mode);
int		xselect_remove(t_rinoosocket *socket);
int		xselect_poll(t_rinoosched *sched, u32 timeout);

#endif		/* !RINOO_XSELECT_H_ */
