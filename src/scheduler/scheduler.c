/**
 * @file   scheduler.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Scheduler functions
 *
 *
 */

#include "rinoo/scheduler/module.h"

/**
 * Create a new scheduler.
 *
 *
 * @return Pointer to the new scheduler, or NULL if an error occurs
 */
rn_sched_t *rn_scheduler(void)
{
	rn_sched_t *sched;

	sched = calloc(1, sizeof(*sched));
	if (sched == NULL) {
		return NULL;
	}
	if (rn_task_driver_init(sched) != 0) {
		free(sched);
		return NULL;
	}
	if (rn_epoll_init(sched) != 0) {
		rn_scheduler_destroy(sched);
		return NULL;
	}
	if (rn_list(&sched->nodes, NULL) != 0) {
		rn_scheduler_destroy(sched);
		return NULL;
	}
	gettimeofday(&sched->clock, NULL);
	return sched;
}

static void rn_sched_cancel_task(rn_list_node_t *node)
{
	rn_sched_node_t *sched_node;

	sched_node = container_of(node, rn_sched_node_t, lnode);
	sched_node->error = ECANCELED;
	errno = sched_node->error;
	if (rn_task_resume(sched_node->task) != 0 && sched_node->task != NULL) {
		rn_task_destroy(sched_node->task);
	}
}

/**
 * Destroy a scheduler
 *
 * @param sched Pointer to the scheduler to destroy
 */
void rn_scheduler_destroy(rn_sched_t *sched)
{
	XASSERTN(sched != NULL);

	rn_spawn_destroy(sched);
	rn_scheduler_stop(sched);
	/* Destroying all pending tasks. */
	rn_task_driver_stop(sched);
	rn_list_flush(&sched->nodes, rn_sched_cancel_task);
	rn_task_driver_destroy(sched);
	rn_epoll_destroy(sched);
	free(sched);
}

/**
 * Get active scheduler.
 *
 *
 * @return Pointer to the active scheduler or NULL if none.
 */
rn_sched_t *rn_scheduler_self(void)
{
	rn_task_t *task;

	task = rn_task_self();
	if (task == NULL) {
		return NULL;
	}
	return task->sched;
}

/**
 * Register a file descriptor in the scheduler and wait for IO.
 *
 * @param node Scheduler node to monitor.
 * @param mode Mode to enable (IN/OUT).
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int rn_scheduler_waitfor(rn_sched_node_t *node, rn_sched_mode_t mode)
{
	int error;

	XASSERT(mode != RN_MODE_NONE, -1);

	if (node->error != 0) {
		error = node->error;
		rn_scheduler_remove(node);
		errno = error;
		return -1;
	}
	if ((node->received & mode) == mode) {
		node->received -= mode;
		return 0;
	}
	if ((node->waiting & mode) != mode) {
		if (node->waiting == RN_MODE_NONE) {
			if (unlikely(rn_epoll_insert(node, mode) != 0)) {
				return -1;
			}
			rn_list_put(&node->sched->nodes, &node->lnode);
		} else {
			if (unlikely(rn_epoll_addmode(node, node->waiting | mode) != 0)) {
				return -1;
			}
		}
	}
	node->mode = mode;
	node->waiting |= mode;
	node->task = rn_task_driver_getcurrent(node->sched);
	node->sched->nbpending++;
	if (unlikely(node->task == &node->sched->driver.main)) {
		while ((node->received & mode) != mode) {
			rn_scheduler_poll(node->sched);
			if (node->error != 0) {
				error = node->error;
				rn_scheduler_remove(node);
				errno = error;
				node->sched->nbpending--;
				return -1;
			}
		}
		node->sched->nbpending--;
		return 0;
	}
	if (rn_task_release(node->sched) != 0 && node->error == 0) {
		node->error = errno;
	}
	node->sched->nbpending--;
	/* Detach task */
	node->task = NULL;
	if (node->error != 0) {
		error = node->error;
		rn_scheduler_remove(node);
		errno = error;
		return -1;
	}
	if ((node->received & mode) != mode) {
		rn_scheduler_remove(node);
		/* Task has been resumed but no event received, this is a timeout */
		errno = ETIMEDOUT;
		return -1;
	}
	node->received -= mode;
	return 0;
}

/**
 * Unregister a file descriptor from the scheduler.
 *
 * @param node Scheduler node to remove.
 *
 * @return 0 on success, otherwise -1.
 */
int rn_scheduler_remove(rn_sched_node_t *node)
{
	if (rn_list_remove(&node->sched->nodes, &node->lnode) != 0) {
		/* Node already removed */
		return -1;
	}
	if (rn_epoll_remove(node) != 0) {
		return -1;
	}
	node->task = NULL;
	return 0;
}

/**
 * Wake up a scheduler node task.
 * This function should be called by the file descriptor monitoring layer (epoll).
 *
 * @param node Scheduler node which received IO event.
 * @param mode IO Event.
 * @param error Error flag.
 */
void rn_scheduler_wakeup(rn_sched_node_t *node, rn_sched_mode_t mode, int error)
{
	if (node->error == 0) {
		node->error = error;
	}
	node->received |= mode;
	if (node->task == NULL || node->task == &node->sched->driver.main) {
		return;
	}
	if (node->mode == mode || node->error != 0) {
		rn_task_resume(node->task);
	}
}

/**
 * Stops the scheduler. It actually sets the stop flag
 * to end the scheduler loop.
 *
 * @param sched Pointer to the scheduler to stop.
 */
void rn_scheduler_stop(rn_sched_t *sched)
{
	XASSERTN(sched != NULL);

	if (sched->stop == false) {
		sched->stop = true;
		rn_spawn_stop(sched);
	}
}

/**
 * Check whether a scheduler has processed all tasks or stop has been requested.
 *
 * @param sched Pointer to the scheduler.
 *
 * @return true if scheduling is over, otherwise false.
 */
static bool rn_sched_end(rn_sched_t *sched)
{
	return (sched->stop == true || (sched->nbpending == 0 && rn_task_driver_nbpending(sched) == 0));
}

/**
 * Check for any task to be executed and poll hte file descriptor monitoring layer (epoll).
 *
 * @param sched Pointer to the scheduler.
 *
 * @return 0 on success, otherwise -1.
 */
int rn_scheduler_poll(rn_sched_t *sched)
{
	int timeout;

	gettimeofday(&sched->clock, NULL);
	timeout = rn_task_driver_run(sched);
	if (!rn_sched_end(sched)) {
		return rn_epoll_poll(sched, timeout);
	}
	return 0;
}

/**
 * Main scheduler loop.
 * This calls rn_scheduler_poll in a loop until the scheduler gets stopped.
 *
 * @param sched Pointer to the scheduler to use.
 *
 */
void rn_scheduler_loop(rn_sched_t *sched)
{
	sched->stop = false;
	if (rn_spawn_start(sched) != 0) {
		goto loop_stop;
	}
	while (!rn_sched_end(sched)) {
		rn_scheduler_poll(sched);
	}
loop_stop:
	rn_spawn_join(sched);
}
