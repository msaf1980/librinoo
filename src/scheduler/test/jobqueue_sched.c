/**
 * @file   jobqueue_sched.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Mon May 10 15:32:30 2010
 *
 * @brief  jobqueue scheduling unit test
 *
 *
 */
#include	"rinoo/rinoo.h"

#define		DIFF_TOLERANCE	25

static u32		last_timeout;
static t_rinoosched	*sched;
static struct timeval	lasttv;

static void		set_timeout(u32 timeout)
{
  if (gettimeofday(&lasttv, NULL) != 0)
    {
      XFAIL();
    }
  last_timeout = timeout;
}

static int		check_timeout()
{
  struct timeval	tv;

  if (gettimeofday(&tv, NULL) != 0)
    {
      XFAIL();
    }
  rinoo_log("Expected timeout: %ums - Recorded timeout: %ldms",
	    last_timeout,
	    ((tv.tv_sec - lasttv.tv_sec) * 1000 + (tv.tv_usec - lasttv.tv_usec) / 1000));
  if (((tv.tv_sec - lasttv.tv_sec) * 1000 +
       (tv.tv_usec - lasttv.tv_usec) / 1000) < 0 ||
      ((tv.tv_sec - lasttv.tv_sec) * 1000 +
       (tv.tv_usec - lasttv.tv_usec) / 1000) > last_timeout + DIFF_TOLERANCE)
    {
      rinoo_log("Job scheduling was too slow (more than %dms)", DIFF_TOLERANCE);
      XFAIL();
    }
  return 0;
}

t_rinoojob_state	job_callback(t_rinoojob *job)
{
  int		counter = PTR_TO_INT(job->args);
  u32		curtimeout;

  if (check_timeout() != 0)
    return JOB_DONE;
  if (counter == 4)
    {
      rinoo_sched_stop(sched);
      return JOB_DONE;
    }
  if (counter > 4)
    {
      XFAIL();
    }
  counter++;
  job->args = INT_TO_PTR(counter);
  switch (counter)
    {
    case 1:
      curtimeout = 100;
      break;
    case 2:
      curtimeout = 300;
      break;
    case 3:
      curtimeout = 600;
      break;
    case 4:
      curtimeout = 50;
      break;
    }
  set_timeout(curtimeout);
  if (jobqueue_schedule_ms(sched, job, curtimeout) == -1)
    {
      XFAIL();
    }
  return JOB_REDO;
}

void		server_event_fsm(t_rinootcp *unused(tcpsock), t_rinootcp_event event)
{
  switch (event)
    {
    case EVENT_TCP_CONNECT:
      break;
    case EVENT_TCP_CLOSE:
      break;
    case EVENT_TCP_IN:
    case EVENT_TCP_OUT:
    case EVENT_TCP_ERROR:
    case EVENT_TCP_TIMEOUT:
      XFAIL();
      break;
    }
}

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int		main()
{
  t_rinootcp	*stcpsock;
  struct timeval	tv1;
  struct timeval	tv2;

  sched = rinoo_sched();
  XTEST(sched != NULL);
  stcpsock = rinoo_tcp_server(sched, 0, 4242, 0, server_event_fsm);
  XTEST(stcpsock != NULL);
  set_timeout(100);
  XTEST(jobqueue_addms(sched,
		       job_callback,
		       NULL,
		       100) != NULL)
  XTEST(gettimeofday(&tv1, NULL) == 0);
  rinoo_sched_loop(sched);
  XTEST(gettimeofday(&tv2, NULL) == 0);
  XTEST((tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec - tv1.tv_usec) / 1000 < 1500);
  rinoo_sched_destroy(sched);
  XPASS();
}
