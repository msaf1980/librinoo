/**
 * @file   sched_getsocket.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2010
 * @date   Sun Jan  3 15:35:15 2010
 *
 * @brief  Test file for sched_getsocket function.
 *
 *
 */
#include	"rinoo/rinoo.h"

extern t_pollerapi	pollers[NB_POLLERS];

void		event_fsm(t_tcpsocket *unused(tcpsock), t_tcpevent event)
{
  switch (event)
    {
    default:
      break;
    }
}

/**
 * This test will check if a scheduler stores FDs correctly.
 *
 * @return 0 if test passed
 */
int		main()
{
  int		i;
  int		found;
  t_sched	*sched;
  t_tcpsocket	*tcpsock;

  sched = sched_create();
  XTEST(sched != NULL);
  tcpsock = tcp_create(sched, 0, 42422, MODE_TCP_SERVER, 0, event_fsm);
  XTEST(tcpsock != NULL);
  XTEST(sched_getsocket(sched, tcpsock->socket.fd) == &tcpsock->socket);
  found = 0;
  for(i = SCHED_MAXFDS - 1; i >= 0; i--)
    {
      XTEST(sched->sock_pool[i] == NULL || (found == 0 && (sched->sock_pool[i] == &tcpsock->socket)));
      if (sched->sock_pool[i] == &tcpsock->socket)
	found = 1;
    }
  tcp_destroy(tcpsock);
  sched_destroy(sched);
  XPASS();
}
