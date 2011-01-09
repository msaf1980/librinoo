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

#define		DIFF_TOLERANCE	20

static int		passed = 1;

static u32		last_timeout;
static t_sched		*sched;
static struct timeval	lasttv;

static void		set_timeout(u32 timeout)
{
  if (gettimeofday(&lasttv, NULL) != 0)
    {
      passed = 0;
      sched_stop(sched);
      return;
    }
  last_timeout = timeout;
}

static int		check_timeout()
{
  struct timeval	tv;

  if (gettimeofday(&tv, NULL) != 0)
    {
      passed = 0;
      sched_stop(sched);
      return (-1);
    }
  printf("Recorded timeout: %ldms\n", ((tv.tv_sec - lasttv.tv_sec) * 1000 +
				      (tv.tv_usec - lasttv.tv_usec) / 1000));
  if (((tv.tv_sec - lasttv.tv_sec) * 1000 +
       (tv.tv_usec - lasttv.tv_usec) / 1000) < 0 ||
      ((tv.tv_sec - lasttv.tv_sec) * 1000 +
       (tv.tv_usec - lasttv.tv_usec) / 1000) > last_timeout + DIFF_TOLERANCE)
    {
      printf("Job scheduling was too slow (more than %dms)\n", DIFF_TOLERANCE);
      passed = 0;
      sched_stop(sched);
      return (-1);
    }
  return (0);
}

t_jobstate	job_callback(t_job *job)
{
  int		counter = PTR_TO_INT(job->args);
  u32		curtimeout;

  if (check_timeout() != 0)
    return JOB_DONE;
  if (counter == 4)
    {
      sched_stop(sched);
      return JOB_DONE;
    }
  if (counter > 4)
    {
      passed = 0;
      sched_stop(sched);
      return JOB_DONE;
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
  if (jobqueue_resettimems(sched, job, curtimeout) == -1)
    {
      passed = 0;
      sched_stop(sched);
    }
  return JOB_REDO;
}

void		server_event_fsm(t_tcpsocket *tcpsock, t_tcpevent event)
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
      passed = 0;
      sched_stop(tcpsock->socket.sched);
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
  t_tcpsocket	*stcpsock;
  struct timeval	tv1;
  struct timeval	tv2;

  sched = sched_create();
  XTEST(sched != NULL);
  stcpsock = tcp_create(sched, 0, 4242, MODE_TCP_SERVER, 0, server_event_fsm);
  XTEST(stcpsock != NULL);
  set_timeout(100);
  XTEST(jobqueue_addms(sched,
		       job_callback,
		       NULL,
		       100) != NULL)
  XTEST(gettimeofday(&tv1, NULL) == 0);
  sched_loop(sched);
  XTEST(gettimeofday(&tv2, NULL) == 0);
  XTEST((tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec - tv1.tv_usec) / 1000 < 1500);
  sched_destroy(sched);
  if (passed != 1)
    XFAIL();
  XPASS();
}
