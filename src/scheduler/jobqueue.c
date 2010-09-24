/**
 * @file   jobqueue.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2010
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
  u32		ms;

  XDASSERT(node1 != NULL, 1);
  XDASSERT(node2 != NULL, 1);

  if (node1 == node2)
    return (0);
  ms = (job1->tv.tv_sec - job2->tv.tv_sec) * 1000 + (job1->tv.tv_usec - job2->tv.tv_usec) / 1000;
  if (ms == 0)
    return (1);
  return (ms);
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
static void	jobqueue_destroy_job(void *ptr)
{
  XDASSERTN(ptr != NULL);

  xfree(ptr);
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
			      struct s_socket *socket,
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
  new->tv = *tv;
  new->socket = socket;
  new->listnode = list_add(sched->jobqueue, new, jobqueue_destroy_job);
  if (unlikely(new->listnode == NULL))
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
 * @param socket Pointer to a socket to which associate the job
 * @param func Pointer to the function to call
 * @param args Pointer to arguments to pass to the function
 * @param msec Number of milliseconds before executing the function
 *
 * @return Pointer to the created job or NULL if an error occurs
 */
t_job		*jobqueue_addms(struct s_sched *sched,
				struct s_socket *socket,
				t_jobstate (*func)(t_job *job),
				void *args,
				const u32 msec)
{
  struct timeval	tv;

  tv.tv_sec = sched->curtime.tv_sec + (msec / 1000) +
    (sched->curtime.tv_usec + (msec % 1000) * 1000) / 1000000;
  tv.tv_usec = (sched->curtime.tv_usec + (msec % 1000) * 1000) % 1000000;
  return (jobqueue_add(sched, socket, func, args, &tv));
}

/**
 * Reschedule a job to a certain a time.
 *
 * @param sched Pointer to the scheduler to use.
 * @param job Pointer to the job to modify.
 * @param tv New time to set up.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int		jobqueue_reschedule(struct s_sched *sched,
				    t_job *job,
				    const struct timeval *tv)
{
  XDASSERT(sched != NULL, -1);
  XDASSERT(sched->jobqueue != NULL, -1);
  XDASSERT(job != NULL, -1);

  job->tv = *tv;
  if (job->listnode != NULL &&
      unlikely(list_removenode(sched->jobqueue, job->listnode, FALSE) == FALSE))
    {
      return (-1);
    }
  job->listnode = list_add(sched->jobqueue, job, jobqueue_destroy_job);
  if (unlikely(job->listnode == NULL))
    return (-1);
  return (0);
}

/**
 * Reset the time at which the job will be done.
 *
 * @param job Pointer to the job to modify.
 * @param tv New time to set up.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int		jobqueue_resettime(t_job *job, const struct timeval *tv)
{
  XDASSERT(job != NULL, -1);

  job->tv = *tv;
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
int		jobqueue_resettimems(t_job *job, const u32 msec)
{
  t_sched		*sched;
  struct timeval	tv;

  XDASSERT(job != NULL, -1);
  XDASSERT(job->socket != NULL, -1);
  XDASSERT(job->socket->sched != NULL, -1);

  sched = job->socket->sched;
  tv.tv_sec = sched->curtime.tv_sec + (msec / 1000) +
    (sched->curtime.tv_usec + (msec % 1000) * 1000) / 1000000;
  tv.tv_usec = (sched->curtime.tv_usec + (msec % 1000) * 1000) % 1000000;
  return (jobqueue_resettime(job, &tv));
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
 * @return Return the number of milliseconds before the next job.
 */
u32		jobqueue_gettimeout(struct s_sched *sched)
{
  t_job			*job;

  XDASSERT(sched != NULL, DEFAULT_TIMEOUT);
  XDASSERT(sched->jobqueue != NULL, DEFAULT_TIMEOUT);

  job = jobqueue_gethead(sched);
  if (job == NULL)
    return (DEFAULT_TIMEOUT);
  if (sched->curtime.tv_sec > job->tv.tv_sec)
    return (0);
  if (sched->curtime.tv_sec == job->tv.tv_sec)
    {
      if (sched->curtime.tv_usec >= job->tv.tv_usec)
	return (0);
      return ((job->tv.tv_usec - sched->curtime.tv_usec > 1000) ?
      	      ((job->tv.tv_usec - sched->curtime.tv_usec) / 1000) : 1);
    }
  return ((job->tv.tv_sec - sched->curtime.tv_sec) * 1000 +
	  (job->tv.tv_usec - sched->curtime.tv_usec) / 1000);
}

/**
 * Execute all jobs where ptime <= now.
 *
 * @param sched Pointer to the scheduler to use.
 */
void		jobqueue_exec(struct s_sched *sched)
{
  t_job	       	*job;

  XDASSERTN(sched != NULL);
  XDASSERTN(sched->jobqueue != NULL);

  job = jobqueue_gethead(sched);
  while (job != NULL &&
	 (sched->curtime.tv_sec > job->tv.tv_sec ||
	  ((sched->curtime.tv_sec == job->tv.tv_sec) &&
	   sched->curtime.tv_usec >= job->tv.tv_usec)))
    {
      job = jobqueue_pop(sched);
      if (job->func(job) == JOB_DONE ||
	  jobqueue_reschedule(sched, job, &job->tv) != 0)
	{
#warning can lead to infinite loop
	  jobqueue_destroy_job(job);
	}
      job = jobqueue_gethead(sched);
    }
}
