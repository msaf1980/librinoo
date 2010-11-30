/**
 * @file   sched_addmode.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2010
 * @date   Sun Jan  3 16:06:23 2010
 *
 * @brief  Test file for sched_addmode function.
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
 * This test will check if a scheduler correctly modifies
 * the FD poll_mode.
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
  XTEST(sched_insert(&tcpsock->socket, EVENT_SCHED_IN, 0) == 0);
  XTEST(tcpsock->socket.poll_mode == EVENT_SCHED_IN);
  XTEST(sched_addmode(&tcpsock->socket, EVENT_SCHED_OUT) == 0);
  XTEST(tcpsock->socket.poll_mode == (EVENT_SCHED_IN | EVENT_SCHED_OUT));
  tcp_destroy(tcpsock);
  sched_destroy(sched);
  XPASS();
}
