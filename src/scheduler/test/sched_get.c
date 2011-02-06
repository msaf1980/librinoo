/**
 * @file   rinoo_sched_get.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Sun Jan  3 15:35:15 2010
 *
 * @brief  Test file for rinoo_sched_get function.
 *
 *
 */
#include	"rinoo/rinoo.h"

extern t_rinoopoller	pollers[NB_POLLERS];

void		event_fsm(t_rinootcp *unused(tcpsock), t_rinootcp_event unused(event))
{
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
  t_rinoosched	*sched;
  t_rinootcp	*tcpsock;

  sched = rinoo_sched();
  XTEST(sched != NULL);
  tcpsock = rinoo_tcp_server(sched, 0, 42422, 0, event_fsm);
  XTEST(tcpsock != NULL);
  XTEST(rinoo_sched_get(sched, tcpsock->socket.fd) == &tcpsock->socket);
  found = 0;
  for(i = RINOO_SCHED_MAXFDS - 1; i >= 0; i--)
    {
      XTEST(sched->sock_pool[i] == NULL || (found == 0 && (sched->sock_pool[i] == &tcpsock->socket)));
      if (sched->sock_pool[i] == &tcpsock->socket)
	found = 1;
    }
  XTEST(found == 1);
  rinoo_tcp_destroy(tcpsock);
  for(i = 0; i < RINOO_SCHED_MAXFDS; i++)
    {
      XTEST(sched->sock_pool[i] == NULL);
    }
  rinoo_sched_destroy(sched);
  XPASS();
}
