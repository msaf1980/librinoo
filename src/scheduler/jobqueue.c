/**
 * @file   jobqueue.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Apr 28 19:36:07 2010
 *
 * @brief  Functions to manage a job queue.
 *
 *
 */

#include	"rinoo/rinoo.h"

/**
 * Hash function for timeval structure.
 * This is used to hash a job and to get the hash of curtime.
 *
 * @param tv Pointer to the timeval structure to hash.
 *
 * @return The hash as an unsigned integer.
 */
static inline u32	jobqueue_hashtime(struct timeval *tv)
{
  return ((tv->tv_sec * RINOO_JOBQUEUE_NBSTEPS) +
	  (tv->tv_usec / (RINOO_JOBQUEUE_STEPMS * 1000)));
}

/**
 * Hash function for job.
 *
 * @param node Pointer to the job to hash.
 *
 * @return The hash as an unsigned integer.
 */
static u32	jobqueue_hash(void *node)
{
  t_rinoojob	*job;

  XDASSERT(node != NULL, 1);

  job = (t_rinoojob *) node;
  return jobqueue_hashtime(&job->exectime);
}

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
  t_rinoojob	*job1 = (t_rinoojob *) node1;
  t_rinoojob	*job2 = (t_rinoojob *) node2;

  XDASSERT(node1 != NULL, 1);
  XDASSERT(node2 != NULL, 1);

  if (node1 == node2)
    {
      return 0;
    }
  if (job1->exectime.tv_sec == job2->exectime.tv_sec)
    {
      return job1->exectime.tv_usec - job2->exectime.tv_usec;
    }
  return job1->exectime.tv_sec - job2->exectime.tv_sec;
}

/**
 * Creates a new job queue.
 *
 *
 * @return Pointer to the new job queue.
 */
t_rinoojob_queue	*jobqueue_create(t_rinoosched *sched)
{
  t_rinoojob_queue	*jobq;

  jobq = xcalloc(1, sizeof(*jobq));
  XASSERT(jobq != NULL, NULL);
  jobq->jobtab = hashtable_create(LIST_SORTED_TAIL,
				  RINOO_JOBQUEUE_HASHSIZE,
				  jobqueue_hash,
				  jobqueue_cmp);
  XASSERT(jobq->jobtab != NULL, NULL);
  jobq->nexttime = sched->curtime;
  return jobq;
}

/**
 * Destroys a job queue.
 *
 * @param ptr Pointer to the job queue to destroy.
 */
void			jobqueue_destroy(void *ptr)
{
  t_rinoojob_queue	*jobq;

  XDASSERTN(ptr != NULL);

  jobq = (t_rinoojob_queue *) ptr;
  hashtable_destroy(jobq->jobtab);
  xfree(jobq);
}

/**
 * Destroys a job. This function is used internally only.
 *
 * @param ptr Pointer to the job to destroy.
 */
static void	jobqueue_destroyjob(void *ptr)
{
  t_rinoojob  	*job = (t_rinoojob *) ptr;

  XDASSERTN(job != NULL);

  xfree(job);
}

/**
 * Remove a job from the main jobqueue.
 *
 * @param job Pointer to the job to remove.
 */
void		jobqueue_removejob(t_rinoojob *job)
{
  XDASSERTN(job != NULL);
  XDASSERTN(job->sched != NULL);
  XDASSERTN(job->sched->jobq != NULL);
  XDASSERTN(job->sched->jobq->jobtab != NULL);

  hashtable_removenode(job->sched->jobq->jobtab, job->listnode, TRUE);
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
t_rinoojob	*jobqueue_add(t_rinoosched *sched,
			      t_rinoojob_state (*func)(t_rinoojob *job),
			      void *args,
			      const struct timeval *tv)
{
  t_rinoojob	*new;

  XDASSERT(sched != NULL, NULL);
  XDASSERT(sched->jobq != NULL, NULL);
  XDASSERT(func != NULL, NULL);

  new = xcalloc(1, sizeof(*new));
  XASSERT(new != NULL, FALSE);
  new->func = func;
  new->args = args;
  if (unlikely(jobqueue_schedule(sched, new, tv) != 0))
    {
      xfree(new);
      return NULL;
    }
  return new;
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
t_rinoojob	*jobqueue_addms(t_rinoosched *sched,
				t_rinoojob_state (*func)(t_rinoojob *job),
				void *args,
				const u32 msec)
{
  struct timeval	tmp;

  timeraddms(&sched->curtime, msec, &tmp);
  return jobqueue_add(sched, func, args, &tmp);
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
int		jobqueue_schedule(t_rinoosched *sched,
				  t_rinoojob *job,
				  const struct timeval *tv)
{
  XDASSERT(sched != NULL, -1);
  XDASSERT(sched->jobq != NULL, -1);
  XDASSERT(job != NULL, -1);

  if (job->listnode != NULL &&
      unlikely(hashtable_popnode(job->sched->jobq->jobtab, job->listnode) != 0))
    {
      return -1;
    }
  job->creatime = sched->curtime;
  job->exectime = *tv;
  if (job->listnode == NULL)
    {
      job->listnode = hashtable_add(sched->jobq->jobtab, job, jobqueue_destroyjob);
      if (unlikely(job->listnode == NULL))
	{
	  return -1;
	}
    }
  else
    {
      if (unlikely(hashtable_addnode(sched->jobq->jobtab, job->listnode) != 0))
	{
	  return -1;
	}
    }
  job->sched = sched;
  if (sched->jobq->jobtab->size == 1 ||
      timercmp(&sched->jobq->nexttime, &job->exectime, >))
    {
      sched->jobq->nexttime = job->exectime;
    }
  return 0;
}

/**
 * Schedule a job in msec milliseconds.
 *
 * @param sched Pointer to the scheduler to use.
 * @param job Pointer to the job to modify.
 * @param msec Number of milliseconds to wait before executing this job.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int		jobqueue_schedule_ms(t_rinoosched *sched,
				     t_rinoojob *job,
				     u32 msec)
{
  struct timeval	tmp;

  XDASSERT(sched != NULL, -1);
  XDASSERT(job != NULL, -1);

  timeraddms(&sched->curtime, msec, &tmp);
  return jobqueue_schedule(sched, job, &tmp);
}

/**
 * Gets the next job to be executed.
 *
 * @param sched Pointer to the scheduler to use.
 *
 * @return Pointer to the job or NULL if the queue is empty
 */
static t_rinoojob	*jobqueue_getnext(t_rinoosched *sched)
{
  u32			i;
  u32			end;
  u32			start;
  struct timeval	tv_tmp;
  t_rinoojob		*curjob = NULL;

  XDASSERT(sched != NULL, NULL);
  XDASSERT(sched->jobq != NULL, NULL);
  XDASSERT(sched->jobq->jobtab != NULL, NULL);
  XDASSERT(sched->jobq->jobtab->table != NULL, NULL);

  if (sched->jobq->jobtab->size == 0)
    {
      sched->jobq->nexttime = sched->curtime;
      return NULL;
    }
  start = jobqueue_hashtime(&sched->jobq->nexttime);
  end = jobqueue_hashtime(&sched->curtime) + RINOO_JOBQUEUE_NBSTEPS;
  for (i = start; i <= end && curjob == NULL; i++)
    {
      curjob = list_gethead(sched->jobq->jobtab->table[i % RINOO_JOBQUEUE_HASHSIZE]);
      if (curjob != NULL)
	{
	  timeraddms(&sched->curtime, RINOO_JOBQUEUE_TIMEOUT, &tv_tmp);
	  if (unlikely(timercmp(&tv_tmp, &curjob->exectime, <)))
	    {
	      curjob = NULL;
	    }
	}
    }
  if (curjob == NULL)
    {
      sched->jobq->nexttime = sched->curtime;
    }
  else
    {
      sched->jobq->nexttime = curjob->exectime;
    }
  return curjob;
}

/**
 * Get the time (ms) before the next job.
 *
 * @param sched Pointer to the scheduler to use.
 *
 * @return The number of milliseconds before the next job.
 */
u32		jobqueue_gettimeout(t_rinoosched *sched)
{
  t_rinoojob	*job;

  XDASSERT(sched != NULL, RINOO_JOBQUEUE_TIMEOUT);
  XDASSERT(sched->jobq != NULL, RINOO_JOBQUEUE_TIMEOUT);

  job = jobqueue_getnext(sched);
  if (job == NULL)
    {
      return RINOO_JOBQUEUE_TIMEOUT;
    }
  if (timercmp(&sched->curtime, &job->exectime, >=))
    {
      return 0;
    }
  return ((job->exectime.tv_sec - sched->curtime.tv_sec) * 1000 +
	  (job->exectime.tv_usec - sched->curtime.tv_usec) / 1000);
}

/**
 * Execute the first job where ptime <= now.
 *
 * @param sched Pointer to the scheduler to use.
 */
void		jobqueue_exec(t_rinoosched *sched)
{
  t_rinoojob	       	*job;
  t_rinoojob_state	jobres;
  struct timeval	tmp1;
  struct timeval	tmp2;

  XDASSERTN(sched != NULL);
  XDASSERTN(sched->jobq != NULL);

  job = jobqueue_getnext(sched);
  if (job != NULL && timercmp(&sched->curtime, &job->exectime, >=))
    {
      tmp1 = job->creatime;
      tmp2 = job->exectime;
      jobres = job->func(job);
      /* Check if the job has been removed or rescheduled */
      if (jobqueue_getnext(sched) == job &&
	  timercmp(&job->creatime, &tmp1, ==) &&
	  timercmp(&job->exectime, &tmp2, ==))
	{
	  switch (jobres)
	    {
	    case JOB_LOOP:
	      jobqueue_schedule(sched, job, &sched->curtime);
	      break;
	    case JOB_REDO:
	      timersub(&job->exectime, &job->creatime, &tmp1);
	      timeradd(&sched->curtime, &tmp1, &tmp2);
	      jobqueue_schedule(sched, job, &tmp2);
	      break;
	    case JOB_DONE:
	      hashtable_removenode(sched->jobq->jobtab, job->listnode, TRUE);
	      break;
	    }
	}
    }
}
