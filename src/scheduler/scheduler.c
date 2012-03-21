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

extern t_rinoopoll pollers[RINOO_NB_POLLS];

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
	sched->poll = &pollers[RINOO_DEFAULT_POLL];
	if (sched->poll->init(sched) != 0) {
		free(sched);
		return NULL;
	}
	if (rinoo_task_driver_init(sched) != 0) {
		rinoo_sched_destroy(sched);
		return NULL;
	}
	return sched;
}

/**
 * Destroy a scheduler
 *
 * @param sched Pointer to the scheduler to destroy
 */
void rinoo_sched_destroy(t_rinoosched *sched)
{
	XASSERTN(sched != NULL);

	sched->poll->destroy(sched);
	rinoo_task_driver_destroy(sched);
	free(sched);
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
 * Controls socket mode registration in the scheduler.
 *
 * @param socket Pointer to the socket to change in the scheduler.
 * @param action Action to perform: enable/disable socket event.
 * @param mode Mode to enable (IN/OUT).
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int rinoo_sched_socket(t_rinoosocket *socket, t_rinoosched_action action, t_rinoosched_mode mode)
{
	XASSERT(socket != NULL, -1);
	XASSERT(socket->sched != NULL, -1);
	XASSERT(socket->fd < RINOO_SCHEDULER_MAXFDS, -1);

	switch (action)
	{
	case RINOO_SCHED_ADD:
		if (socket->sched->sock_pool[socket->fd] != socket) {
			if (unlikely(socket->sched->poll->insert(socket, mode) != 0)) {
				return -1;
			}
			socket->sched->sock_pool[socket->fd] = socket;
		} else {
			if (unlikely(socket->sched->poll->addmode(socket, mode) != 0)) {
				return -1;
			}
		}
		break;
	case RINOO_SCHED_REMOVE:
		if (socket->sched->poll->remove(socket) != 0)
		{
			return -1;
		}
		socket->sched->sock_pool[socket->fd] = NULL;
		break;
	}
	return 0;
}

/**
 * Returns the socket corresponding to a file descriptor.
 *
 * @param sched Pointer to the scheduler to use.
 * @param fd File descriptor (< RINOO_SCHEDULER_MAXFDS).
 *
 * @return A pointer to the corresponding socket if found, else NULL.
 */
t_rinoosocket *rinoo_sched_get(t_rinoosched *sched, int fd)
{
	XASSERT(sched != NULL, NULL);
	XASSERT(fd >= 0 && fd < RINOO_SCHEDULER_MAXFDS, NULL);

	return sched->sock_pool[fd];
}

/**
 * Main scheduler loop.
 *
 * @param sched Pointer to the scheduler to use.
 *
 */
void rinoo_sched_loop(t_rinoosched *sched)
{
	u32 timeout;

	while (sched->stop == 0) {
		gettimeofday(&sched->clock, NULL);
		timeout = rinoo_task_driver_run(sched);
		if (sched->stop != 0) {
			break;
		}
		rinoo_epoll_poll(sched, timeout);
	}
}
