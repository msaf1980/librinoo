/**
 * @file   jobqueue_create.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Apr 28 16:42:20 2010
 *
 * @brief  jobqueue_create unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int		main()
{
  t_rinoosched		*sched;
  t_rinoojob_queue	*jobqueue;

  sched = rinoo_sched();
  jobqueue = sched->jobq;
  XTEST(jobqueue != NULL);
  XTEST(timercmp(&jobqueue->nexttime, &sched->curtime, ==) == 1);
  XTEST(jobqueue->jobtab != NULL);
  XTEST(jobqueue->jobtab->size == 0);
  rinoo_sched_destroy(sched);
  XPASS();
}
