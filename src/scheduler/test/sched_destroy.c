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

void		event_fsm(t_tcpsocket *unused(tcpsock),
			  t_tcpevent unused(event))
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
  t_tcpsocket	*tcpsock;

  sched = rinoo_sched();
  XTEST(sched != NULL);
  tcpsock = tcp_create(sched, 0, 42422, MODE_TCP_SERVER, 0, event_fsm);
  XTEST(tcpsock != NULL);
  rinoo_sched_destroy(sched);
  XPASS();
}
