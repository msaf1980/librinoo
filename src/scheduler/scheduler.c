/**
 * @file   scheduler.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Fri Jan  6 16:10:07 2012
 *
 * @brief  Scheduler functions
 *
 *
 */

#include "rinoo/rinoo.h"

/**
 * Create a new scheduler.
 *
 *
 * @return Pointer to the new scheduler, or NULL if an error occurs
 */
t_rinoosched *rinoo_sched()
{
	t_rinoosched *sched;

	sched = calloc(1, sizeof(*sched));
	if (sched == NULL) {
		return NULL;
	}
	if (rinoo_task_driver_init(sched) != 0) {
		free(sched);
		return NULL;
	}
	if (rinoo_epoll_init(sched) != 0) {
		rinoo_sched_destroy(sched);
		return NULL;
	}
	gettimeofday(&sched->clock, NULL);
	return sched;
}

/**
 * Destroy a scheduler
 *
 * @param sched Pointer to the scheduler to destroy
 */
void rinoo_sched_destroy(t_rinoosched *sched)
{
	int i;

	XASSERTN(sched != NULL);

	/* Destroying all pending tasks. */
	for (i = 0; i < RINOO_SCHEDULER_MAXFDS; i++)
	{
		if (sched->task_pool[i] != NULL)
		{
			errno = ECANCELED;
			rinoo_task_resume(sched->task_pool[i]);
		}
	}
	rinoo_task_driver_destroy(sched);
	rinoo_epoll_destroy(sched);
	free(sched);
}

/**
 * Register a file descriptor in the scheduler and wait for IO.
 *
 * @param sched Pointer to the scheduler.
 * @param fd File descriptor to monitor.
 * @param mode Mode to enable (IN/OUT).
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int rinoo_sched_waitfor(t_rinoosched *sched, int fd, t_rinoosched_mode mode)
{
	XASSERT(fd < RINOO_SCHEDULER_MAXFDS, -1);

	if (sched->task_pool[fd] == NULL) {
		if (unlikely(rinoo_epoll_insert(sched, fd, mode) != 0)) {
			return -1;
		}
	} else {
		if (unlikely(rinoo_epoll_addmode(sched, fd, mode) != 0)) {
			return -1;
		}
	}
	sched->task_pool[fd] = rinoo_task_driver_getcurrent(sched);
	if (unlikely(sched->task_pool[fd] == &sched->driver.main)) {
		return rinoo_sched_poll(sched);
	}
	sched->nbpending++;
	rinoo_task_release(sched);
	sched->nbpending--;
	if (sched->error != 0) {
		rinoo_sched_remove(sched, fd);
		errno = sched->error;
		return -1;
	}
	if (sched->lastmode != mode) {
		rinoo_sched_remove(sched, fd);
		/* Task has been resumed without setting lastmode */
		errno = ETIMEDOUT;
		return -1;
	}
	sched->error = 0;
	sched->lastmode = RINOO_MODE_NONE;
	return 0;
}

/**
 * Unregister a file descriptor from the scheduler.
 *
 * @param sched Pointer to the scheduler.
 * @param fd File descriptor to remove.
 *
 * @return 0 on success, otherwise -1.
 */
int rinoo_sched_remove(t_rinoosched *sched, int fd)
{
	XASSERT(fd < RINOO_SCHEDULER_MAXFDS, -1);

	sched->task_pool[fd] = NULL;
	if (rinoo_epoll_remove(sched, fd) != 0) {
		return -1;
	}
	return 0;
}

/**
 * Wake up a file descriptor task.
 * This function should be called by the file descriptor monitoring layer (epoll).
 *
 * @param sched Pointer to the scheduler.
 * @param fd File descriptor which received IO event.
 * @param mode IO Event.
 * @param error Error flag.
 */
void rinoo_sched_wakeup(t_rinoosched *sched, int fd, t_rinoosched_mode mode, int error)
{
	t_rinootask *task;

	XASSERTN(fd < RINOO_SCHEDULER_MAXFDS);

	task = sched->task_pool[fd];
	sched->error = error;
	sched->lastmode = mode;
	if (task == NULL || task == &sched->driver.main) {
		/* Nothing to wake */
		return;
	}
	rinoo_task_resume(task);
}

/**
 * Stops the scheduler. It actually sets the stop flag
 * to end the scheduler loop.
 *
 * @param sched Pointer to the scheduler to stop.
 */
void rinoo_sched_stop(t_rinoosched *sched)
{
	XASSERTN(sched != NULL);

	sched->stop = 1;
}

/**
 * Check for any task to be executed and poll hte file descriptor monitoring layer (epoll).
 *
 * @param sched Pointer to the scheduler.
 *
 * @return 0 on success, otherwise -1.
 */
int rinoo_sched_poll(t_rinoosched *sched)
{
	u32 timeout;

	gettimeofday(&sched->clock, NULL);
	timeout = rinoo_task_driver_run(sched);
	if (sched->stop == 0) {
		return rinoo_epoll_poll(sched, timeout);
	}
	return 0;
}

/**
 * Main scheduler loop.
 * This calls rinoo_sched_poll in a loop until the scheduler gets stopped.
 *
 * @param sched Pointer to the scheduler to use.
 *
 */
void rinoo_sched_loop(t_rinoosched *sched)
{
	sched->stop = 0;
	while (sched->stop == 0 && (sched->nbpending > 0 || rinoo_task_driver_nbpending(sched) > 0)) {
		rinoo_sched_poll(sched);
	}
}
