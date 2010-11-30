/**
 * @file   sched_remove.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2010
 * @date   Sun Jan  3 16:06:23 2010
 *
 * @brief  Test file for sched_remove function.
 *
 *
 */
#include	"rinoo/rinoo.h"

void		event_fsm(t_tcpsocket *unused(tcpsock), t_tcpevent event)
{
  switch (event)
    {
    default:
      break;
    }
}

/**
 * This test will check if a scheduler removes a FD correctly.
 *
 * @return 0 if test passed
 */
int		main()
{
  t_sched	*sched;
  t_tcpsocket	*tcpsock;

  sched = sched_create();
  XTEST(sched != NULL);
  tcpsock = tcp_create(sched, 0, 42422, MODE_TCP_SERVER, 0, event_fsm);
  XTEST(tcpsock != NULL);
  XTEST(sched_remove(&tcpsock->socket) == 0);
  XTEST(sched_getsocket(sched, tcpsock->socket.fd) == NULL);
  tcp_destroy(tcpsock);
  sched_destroy(sched);
  XPASS();
}
