/**
 * @file   rinoo_sched_stop.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Sun Jan  3 16:02:21 2010
 *
 * @brief  Test file for rinoo_sched_stop function
 *
 *
 */
#include	"rinoo/rinoo.h"

t_rinoojob_state	stop_callback(t_rinoojob *job)
{
  rinoo_sched_stop(job->sched);
  XTEST(job->sched->stop == 1);
  return JOB_DONE;
}

/**
 * This test will check if a scheduler stops correctly.
 *
 * @return 0 if test passed
 */
int		main()
{
  t_rinoosched	*sched;
  t_rinoojob	*job;

  sched = rinoo_sched();
  XTEST(sched != NULL);
  job = jobqueue_addms(sched, stop_callback, NULL, 0);
  XTEST(job != NULL);
  rinoo_sched_loop(sched);
  rinoo_sched_destroy(sched);
  XPASS();
}
