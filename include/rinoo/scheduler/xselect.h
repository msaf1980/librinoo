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

typedef struct	s_selectdata
{
  fd_set	readfds;
  fd_set	writefds;
  int		nfds;
  sigset_t	sigmask;
}		t_selectdata;

int		xselect_init(t_sched *sched);
void		xselect_destroy(t_sched *sched);
int		xselect_insert(t_socket *socket, t_schedevent mode);
int		xselect_addmode(t_socket *socket, t_schedevent mode);
int		xselect_delmode(t_socket *socket, t_schedevent mode);
int		xselect_remove(t_socket *socket);
int		xselect_poll(t_sched *sched, u32 timeout);

#endif		/* !RINOO_XSELECT_H_ */
