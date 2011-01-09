/**
 * @file   sched_create.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Sun Jan  3 15:34:47 2010
 *
 * @brief  Test file for sched_create function.
 *
 *
 */
#include	"rinoo/rinoo.h"

extern t_pollerapi	pollers[NB_POLLERS];

/**
 * This test will check if a scheduler is well initialized.
 *
 * @return 0 if test passed
 */
int		main()
{
  int		i;
  t_sched	*sched;

  sched = sched_create();
  XTEST(sched != NULL);
  XTEST(sched->poller == &pollers[DEFAULT_POLLER]);
  XTEST(sched->poller_data != NULL);
  XTEST(sched->stop == 0);
  XTEST(sched->sock_pool != NULL);
  for (i = SCHED_MAXFDS - 1; i >= 0; i--)
    XTEST(sched->sock_pool[i] == NULL);
  sched_destroy(sched);
  XPASS();
}
