/**
 * @file   jobqueue.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2010
 * @date   Wed Apr 28 19:36:07 2010
 *
 * @brief  Functions to manage a job queue.
 *
 *
 */

#include	"rinoo/rinoo.h"

/**
 * Compares two jobs depending on their execution time.
 *
 * @param node1 Pointer to the first job.
 * @param node2 Pointer to the second job.
 *
 * @return Returns an integer less than, equal to, or greater than zero if node1, respectively, is less than, matches, or is greater than node2.
 */
static int	jobqueue_cmp(void *node1, void *node2)
{
  t_job		*job1 = (t_job *) node1;
  t_job		*job2 = (t_job *) node2;

  XDASSERT(node1 != NULL, 1);
  XDASSERT(node2 != NULL, 1);

  if (node1 == node2)
    return 0;
  if (job1->exectime.tv_sec > job2->exectime.tv_sec)
    return 1;
  if (job1->exectime.tv_sec < job2->exectime.tv_sec)
    return -1;
  if (job1->exectime.tv_usec > job2->exectime.tv_usec)
    return 1;
  if (job1->exectime.tv_usec < job2->exectime.tv_usec)
    return -1;

  /* Always make a difference between jobs */
  return 1;
}

/**
 * Creates a new job queue.
 *
 *
 * @return Pointer to the new job queue.
 */
t_list		*jobqueue_create()
{
  return (list_create(jobqueue_cmp));
}

/**
 * Destroys a job queue.
 *
 * @param ptr Pointer to the job queue to destroy.
 */
void		jobqueue_destroy(void *ptr)
{
  XDASSERTN(ptr != NULL);

  list_destroy(ptr);
}

/**
 * Destroys a job. This function is used internally only.
 *
 * @param ptr Pointer to the job to destroy.
 */
static void	jobqueue_destroyjob(void *ptr)
{
  t_job		*job = (t_job *) ptr;

  XDASSERTN(job != NULL);

  xfree(job);
}

/**
 * Remove a job from the main jobqueue.
 *
 * @param sched Pointer to the scheduler to use.
 * @param job Pointer to the job to remove.
 */
void		jobqueue_removejob(t_job *job)
{
  XDASSERTN(job != NULL);
  XDASSERTN(job->sched != NULL);

  list_removenode(job->sched->jobqueue, job->listnode, TRUE);
}

/**
 * Schedule a job to a certain a time.
 *
 * @param sched Pointer to the scheduler to use.
 * @param job Pointer to the job to modify.
 * @param tv New time to set up.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int		jobqueue_schedule(struct s_sched *sched,
				  t_job *job,
				  const struct timeval *tv)
{
  XDASSERT(sched != NULL, -1);
  XDASSERT(sched->jobqueue != NULL, -1);
  XDASSERT(job != NULL, -1);

  job->creatime = sched->curtime;
  job->exectime = *tv;
  if (job->listnode != NULL &&
      unlikely(list_removenode(job->sched->jobqueue, job->listnode, FALSE) == FALSE))
    {
      return (-1);
    }
  job->listnode = list_add(sched->jobqueue, job, jobqueue_destroyjob);
  if (unlikely(job->listnode == NULL))
    return (-1);
  job->sched = sched;
  return (0);
}

/**
 * Reschedule a job.
 *
 * @param sched Pointer to the scheduler to use.
 * @param job Pointer to the job to modify.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int		jobqueue_reschedule(struct s_sched *sched, t_job *job)
{
  struct timeval	tmp;

  XDASSERT(sched != NULL, -1);
  XDASSERT(sched->jobqueue != NULL, -1);
  XDASSERT(job != NULL, -1);

  timersub(&job->exectime, &job->creatime, &tmp);
  timeradd(&sched->curtime, &tmp, &job->exectime);
  return jobqueue_schedule(sched, job, &job->exectime);
}

/**
 * Creates and adds a new job in a job queue.
 *
 * @param sched Pointer to the scheduler to use.
 * @param func Function to call for this job.
 * @param args Arguments to pass to the function.
 * @param tm Time at which the function will be called.
 *
 * @return Pointer to the created job or NULL if an error occurs.
 */
t_job		*jobqueue_add(struct s_sched *sched,
			      t_jobstate (*func)(t_job *job),
			      void *args,
			      const struct timeval *tv)
{
  t_job		*new;

  XDASSERT(sched != NULL, NULL);
  XDASSERT(sched->jobqueue != NULL, NULL);
  XDASSERT(func != NULL, NULL);

  new = xcalloc(1, sizeof(*new));
  XASSERT(new != NULL, FALSE);
  new->func = func;
  new->args = args;
  if (unlikely(jobqueue_schedule(sched, new, tv) != 0))
    {
      xfree(new);
      return (NULL);
    }
  return (new);
}

/**
 * Add a job into a job queue
 *
 * @param sched Pointer to the scheduler to use
 * @param func Pointer to the function to call
 * @param args Pointer to arguments to pass to the function
 * @param msec Number of milliseconds before executing the function
 *
 * @return Pointer to the created job or NULL if an error occurs
 */
t_job		*jobqueue_addms(struct s_sched *sched,
				t_jobstate (*func)(t_job *job),
				void *args,
				const u32 msec)
{
  struct timeval	tmp;

  timeraddms(&sched->curtime, msec, &tmp);
  return (jobqueue_add(sched, func, args, &tmp));
}

/**
 * Reset the time at which the job will be done.
 *
 * @param job Pointer to the job to modify.
 * @param tv New time to set up.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int		jobqueue_resettime(struct s_sched *sched,
				   t_job *job,
				   const struct timeval *tv)
{
  XDASSERT(sched != NULL, -1);
  XDASSERT(job != NULL, -1);
  XDASSERT(tv != NULL, -1);

  job->creatime = sched->curtime;
  job->exectime = *tv;
  return (0);
}

/**
 * Reset the time at which the job will be done, based on milliseconds.
 *
 * @param job Pointer to the job to modify.
 * @param msec Number of milliseconds before executing the function
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int		jobqueue_resettimems(struct s_sched *sched,
				     t_job *job,
				     const u32 msec)
{
  struct timeval	tmp;

  XDASSERT(sched != NULL, -1);
  XDASSERT(job != NULL, -1);

  timeraddms(&sched->curtime, msec, &tmp);
  return (jobqueue_resettime(sched, job, &tmp));
}

/**
 * Gets the head of a job queue and removes it.
 * This function is used internally only.
 *
 * @param sched Pointer to the scheduler to use.
 *
 * @return Pointer to the job or NULL if the queue is empty.
 */
static t_job	*jobqueue_pop(struct s_sched *sched)
{
  t_job		*head;

  XDASSERT(sched != NULL, NULL);
  XDASSERT(sched->jobqueue != NULL, NULL);

  head = list_pophead(sched->jobqueue);
  if (head == NULL)
    {
      return NULL;
    }
  head->listnode = NULL;
  return head;
}

/**
 * Gets the head of a job queue, does not remove it.
 *
 * @param sched Pointer to the scheduler to use.
 *
 * @return Pointer to the job or NULL if the queue is empty
 */
static t_job	*jobqueue_gethead(struct s_sched *sched)
{
  XDASSERT(sched != NULL, NULL);
  XDASSERT(sched->jobqueue != NULL, NULL);

  return (list_gethead(sched->jobqueue));
}

/**
 * Get the time (ms) before the next job.
 *
 * @param sched Pointer to the scheduler to use.
 *
 * @return The number of milliseconds before the next job.
 */
u32		jobqueue_gettimeout(struct s_sched *sched)
{
  t_job			*job;

  XDASSERT(sched != NULL, DEFAULT_TIMEOUT);
  XDASSERT(sched->jobqueue != NULL, DEFAULT_TIMEOUT);

  job = jobqueue_gethead(sched);
  if (job == NULL)
    return (DEFAULT_TIMEOUT);
  if (sched->curtime.tv_sec > job->exectime.tv_sec)
    return (0);
  return ((job->exectime.tv_sec - sched->curtime.tv_sec) * 1000 +
	  (job->exectime.tv_usec - sched->curtime.tv_usec) / 1000);
}

/**
 * Execute the first job where ptime <= now.
 *
 * @param sched Pointer to the scheduler to use.
 */
void		jobqueue_exec(struct s_sched *sched)
{
  t_job	       	*job;
  t_jobstate	jobres;

  XDASSERTN(sched != NULL);
  XDASSERTN(sched->jobqueue != NULL);

  job = jobqueue_gethead(sched);
  if (job != NULL &&
      timercmp(&sched->curtime, &job->exectime, >=) != 0)
    {
      jobres = job->func(job);
      /**
       * job could have been destroyed in the callback.
       * if job is still available, then extract it from
       * the queue.
       */
      if (jobqueue_gethead(sched) == job)
	{
	  job = jobqueue_pop(sched);
	  switch (jobres)
	    {
	    case JOB_LOOP:
	      jobqueue_schedule(sched, job, &sched->curtime);
	      break;
	    case JOB_REDO:
	      jobqueue_reschedule(sched, job);
	      break;
	    case JOB_DONE:
	      jobqueue_destroyjob(job);
	      break;
	    }
	}
    }
}
