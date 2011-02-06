/**
 * @file   rinoo_sched_remove.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Sun Jan  3 16:06:23 2010
 *
 * @brief  Test file for rinoo_sched_remove function.
 *
 *
 */
#include	"rinoo/rinoo.h"

void		event_fsm(t_rinootcp *unused(tcpsock), t_rinootcp_event unused(event))
{
}

/**
 * This test will check if a scheduler removes a FD correctly.
 *
 * @return 0 if test passed
 */
int		main()
{
  t_rinoosched	*sched;
  t_rinootcp	*tcpsock;

  sched = rinoo_sched();
  XTEST(sched != NULL);
  tcpsock = rinoo_tcp_server(sched, 0, 42422, 0, event_fsm);
  XTEST(tcpsock != NULL);
  XTEST(rinoo_sched_socket(RINOO_SCHED_REMOVE, &tcpsock->socket, 0) == 0);
  XTEST(rinoo_sched_get(sched, tcpsock->socket.fd) == NULL);
  rinoo_tcp_destroy(tcpsock);
  rinoo_sched_destroy(sched);
  XPASS();
}
