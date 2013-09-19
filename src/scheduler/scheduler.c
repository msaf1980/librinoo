/**
 * @file   scheduler.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
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
t_rinoosched *rinoo_sched(void)
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
	t_rinoolist_node *cur;
	t_rinoosched_node *node;

	XASSERTN(sched != NULL);

	if (sched->spawns.sched != NULL) {
		for (i = 0; i < sched->spawns.count; i++) {
			rinoo_sched_destroy(sched->spawns.sched[i]);
		}
		free(sched->spawns.sched);
	}
	if (sched->spawns.thread != NULL) {
		free(sched->spawns.thread);
	}
	/* Destroying all pending tasks. */
	while ((cur = rinoolist_pop(&sched->nodes)) != NULL) {
		node = container_of(cur, t_rinoosched_node, lnode);
		errno = ECANCELED;
		if (rinoo_task_resume(node->task) != 0) {
			rinoo_task_destroy(node->task);
		}
	}
	rinoo_task_driver_destroy(sched);
	rinoo_epoll_destroy(sched);
	free(sched);
}

/**
 * Spawns a number of children schedulers.
 *
 * @param sched Parent scheduler
 * @param count Number of children to create
 *
 * @return 0 on success, otherwise -1
 */
int rinoo_sched_spawn(t_rinoosched *sched, int count)
{
	int i;
	pthread_t *thread;
	t_rinoosched *child;
	t_rinoosched **children;

	if (sched->spawns.count == 0) {
		children = calloc(count, sizeof(*children));
		if (children == NULL) {
			return -1;
		}
		thread = calloc(count, sizeof(*thread));
		if (thread == NULL) {
			free(children);
			return -1;
		}
	} else {
		children = realloc(sched->spawns.sched, sizeof(*children) * (sched->spawns.count + count));
		if (children == NULL) {
			return -1;
		}
		thread = realloc(sched->spawns.thread, sizeof(*thread) * (sched->spawns.count + count));
		if (thread == NULL) {
			/* Hopefullu, this won't be lost */
			sched->spawns.sched = children;
			return -1;
		}
	}
	sched->spawns.sched = children;
	sched->spawns.thread = thread;
	for (i = sched->spawns.count; i < sched->spawns.count + count; i++) {
		child = rinoo_sched();
		if (child == NULL) {
			sched->spawns.count = i;
			return -1;
		}
		child->id = i + 1;
		sched->spawns.sched[i] = child;
		sched->spawns.thread[i] = 0;
	}
	sched->spawns.count = i;
	return 0;
}

/**
 * Get a scheduler spawn from its id.
 * Id 0 returns sched itself.
 *
 * @param sched Scheduler to use
 * @param id Spawn id
 *
 * @return Pointer to the spawn or NULL if an error occured
 */
t_rinoosched *rinoo_sched_spawn_get(t_rinoosched *sched, int id)
{
	if (id < 0 || id > sched->spawns.count) {
		return NULL;
	}
	if (id == 0) {
		return sched;
	}
	return sched->spawns.sched[id];
}

/**
 * Get active scheduler.
 *
 *
 * @return Pointer to the active scheduler or NULL if none.
 */
t_rinoosched *rinoo_sched_self(void)
{
	t_rinootask *task;

	task = rinoo_task_self();
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
int rinoo_sched_waitfor(t_rinoosched_node *node, t_rinoosched_mode mode)
{
	int error;

	XASSERT(mode != RINOO_MODE_NONE, -1);

	if (node->error != 0) {
		error = node->error;
		rinoo_sched_remove(node);
		errno = error;
		return -1;
	}
	if ((node->received & mode) == mode) {
		node->received -= mode;
		return 0;
	}
	if ((node->waiting & mode) != mode) {
		if (node->waiting == RINOO_MODE_NONE) {
			if (unlikely(rinoo_epoll_insert(node, mode) != 0)) {
				return -1;
			}
			rinoolist_add(&node->sched->nodes, &node->lnode);
		} else {
			if (unlikely(rinoo_epoll_addmode(node, mode) != 0)) {
				return -1;
			}
		}
	}
	node->waiting |= mode;
	node->task = rinoo_task_driver_getcurrent(node->sched);
	if (unlikely(node->task == &node->sched->driver.main)) {
		return rinoo_sched_poll(node->sched);
	}
	node->sched->nbpending++;
	rinoo_task_release(node->sched);
	node->sched->nbpending--;
	/* Detach task */
	node->task = NULL;
	if (node->error != 0) {
		error = node->error;
		rinoo_sched_remove(node);
		errno = error;
		return -1;
	}
	if ((node->received & mode) != mode) {
		rinoo_sched_remove(node);
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
int rinoo_sched_remove(t_rinoosched_node *node)
{
	if (node->sched == NULL) {
		return -1;
	}
	if (rinoo_epoll_remove(node) != 0) {
		return -1;
	}
	rinoolist_remove(&node->sched->nodes, &node->lnode);
	node->task = NULL;
	node->sched = NULL;
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
void rinoo_sched_wakeup(t_rinoosched_node *node, t_rinoosched_mode mode, int error)
{
	if (node->error == 0) {
		node->error = error;
	}
	node->received |= mode;
	if (node->task != NULL && node->task != &node->sched->driver.main && (node->waiting & mode) == mode) {
		rinoo_task_resume(node->task);
	}
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
	int timeout;

	gettimeofday(&sched->clock, NULL);
	timeout = rinoo_task_driver_run(sched);
	if (sched->stop == 0 && sched->nbpending > 0) {
		return rinoo_epoll_poll(sched, timeout);
	}
	return 0;
}

static void *rinoo_sched_spawn_loop(void *arg)
{
	rinoo_sched_loop(arg);
	return NULL;
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
	int i;

	sched->stop = 0;
	for (i = 0; i < sched->spawns.count; i++) {
		if (pthread_create(&sched->spawns.thread[i], NULL, rinoo_sched_spawn_loop, sched->spawns.sched[i]) != 0) {
			goto start_stop;
		}
	}
	while (sched->stop == 0 && (sched->nbpending > 0 || rinoo_task_driver_nbpending(sched) > 0)) {
		rinoo_sched_poll(sched);
	}
start_stop:
	for (i = 0; i < sched->spawns.count; i++) {
		if (sched->spawns.thread[i] != 0) {
			rinoo_sched_stop(sched->spawns.sched[i]);
			pthread_join(sched->spawns.thread[i], NULL);
		}
	}
}
