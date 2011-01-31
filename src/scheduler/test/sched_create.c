/**
 * @file   rinoo_sched.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Sun Jan  3 15:34:47 2010
 *
 * @brief  Test file for rinoo_sched function.
 *
 *
 */
#include	"rinoo/rinoo.h"

extern t_rinoopoller	pollers[NB_POLLERS];

/**
 * This test will check if a scheduler is well initialized.
 *
 * @return 0 if test passed
 */
int		main()
{
  int		i;
  t_rinoosched	*sched;

  sched = rinoo_sched();
  XTEST(sched != NULL);
  XTEST(sched->poller == &pollers[DEFAULT_POLLER]);
  XTEST(sched->poller_data != NULL);
  XTEST(sched->stop == 0);
  XTEST(sched->sock_pool != NULL);
  for (i = RINOO_SCHED_MAXFDS - 1; i >= 0; i--)
    XTEST(sched->sock_pool[i] == NULL);
  rinoo_sched_destroy(sched);
  XPASS();
}
