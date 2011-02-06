/**
 * @file   rinoo_sched_destroy.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Tue Nov 30 13:57:00 2010
 *
 * @brief  Test file for rinoo_sched_destroy function
 *
 *
 */
#include	"rinoo/rinoo.h"

void		event_fsm(t_rinootcp *unused(tcpsock),
			  t_rinootcp_event unused(event))
{
}

/**
 * This test will check if a scheduler is destroyed correctly.
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
  rinoo_sched_destroy(sched);
  XPASS();
}
