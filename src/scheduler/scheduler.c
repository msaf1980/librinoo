/**
 * @file   scheduler.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Tue Dec 22 17:30:27 2009
 *
 * @brief  Functions to manage a scheduler.
 *
 *
 */

#include	"rinoo/rinoo.h"

extern t_rinoopoller	pollers[NB_POLLERS];

/**
 * Creates a new scheduler.
 *
 *
 * @return A pointer to the new scheduler if succeeds, else NULL.
 */
t_rinoosched	*rinoo_sched()
{
  t_rinoosched	*sched;

  sched = xcalloc(1, sizeof(*sched));
  XASSERTSTR(sched != NULL, NULL, "Cannot create a new scheduler");
  if (rinoo_sched_clock(sched) != 0)
    {
      xfree(sched);
      XASSERTSTR(0, NULL, "Scheduler clock init failed");
    }
  sched->poller = &pollers[DEFAULT_POLLER];
  sched->timeoutq = list_create(LIST_SORTED_TAIL, rinoo_socket_timeout_cmp);
  if (sched->timeoutq == NULL)
    {
      xfree(sched);
      XASSERTSTR(0, NULL, "Timeout queue init failed");
    }
  sched->jobq = jobqueue_create(sched);
  if (sched->jobq == NULL)
    {
      xfree(sched);
      XASSERTSTR(0, NULL, "Job queue init failed");
    }
  if (sched->poller->init(sched) == -1)
    {
      xfree(sched);
      XASSERTSTR(0, NULL, "Poller init failed");
    }
  return sched;
}

/**
 * Destroys a scheduler.
 *
 * @param sched Pointer to the scheduler to destroy.
 */
void		rinoo_sched_destroy(t_rinoosched *sched)
{
  int		i;

  XASSERTN(sched != NULL);

  /* Destroying all pending sockets. */
  for (i = 0; i < RINOO_SCHED_MAXFDS; i++)
    {
      if (sched->sock_pool[i] != NULL)
	{
	  sched->sock_pool[i]->event_fsm(sched->sock_pool[i],
					 EVENT_SCHED_STOP);
	}
    }
  list_destroy(sched->timeoutq);
  jobqueue_destroy(sched->jobq);
  sched->poller->destroy(sched);
  xfree(sched);
}

/**
 * Controls socket event registration in the scheduler.
 *
 * @param action Action to perform: insert/remove socket, enable/disable socket event.
 * @param socket Pointer to the socket to change in the scheduler.
 * @param mode Socket event to enable.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int		rinoo_sched_socket(t_rinoosched_action action,
				   t_rinoosocket *socket,
				   t_rinoosched_event mode)
{
  XDASSERT(socket != NULL, -1);
  XASSERT(socket->fd < RINOO_SCHED_MAXFDS, -1);

  switch (action)
    {
    case RINOO_SCHED_INSERT:
      if (unlikely(socket->sched->poller->insert(socket, mode) != 0))
	{
	  return -1;
	}
      socket->sched->sock_pool[socket->fd] = socket;
      break;
    case RINOO_SCHED_MODADD:
      if (unlikely(socket->sched->poller->addmode(socket, mode) != 0))
	{
	  return -1;
	}
      break;
    case RINOO_SCHED_MODDEL:
      if (unlikely(socket->sched->poller->delmode(socket, mode) != 0))
	{
	  return -1;
	}
      break;
    case RINOO_SCHED_REMOVE:
      if (socket->sched->poller->remove(socket) != 0)
	{
	  return -1;
	}
      socket->sched->sock_pool[socket->fd] = NULL;
      rinoo_socket_timeout_remove(socket);
      break;
    }
  return 0;
}

/**
 * Returns the socket corresponding to a file descriptor.
 *
 * @param sched Pointer to the scheduler to use.
 * @param fd File descriptor (< RINOO_SCHED_MAXFDS).
 *
 * @return A pointer to the corresponding socket if found, else NULL.
 */
t_rinoosocket	*rinoo_sched_get(t_rinoosched *sched, int fd)
{
  XDASSERT(sched != NULL, NULL);
  XDASSERT(fd >= 0 && fd < RINOO_SCHED_MAXFDS, NULL);

  return sched->sock_pool[fd];
}

/**
 * Set the scheduler clock with gettimeofday.
 *
 * @param sched Pointer to the scheduler to use.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int		rinoo_sched_clock(t_rinoosched *sched)
{
  return gettimeofday(&sched->curtime, NULL);
}

/**
 * Stops the scheduler. It actually sets the stop flag
 * to end the scheduler loop.
 *
 * @param sched Pointer to the scheduler to stop.
 */
void		rinoo_sched_stop(t_rinoosched *sched)
{
  XDASSERTN(sched != NULL);

  sched->stop = 1;
}

/**
 * Main scheduler loop.
 *
 * @param sched Pointer to the scheduler to use.
 *
 * @return -1 if an error occurs, else 0.
 */
int		rinoo_sched_loop(t_rinoosched *sched)
{
  u32		t1;
  u32		t2;
  t_rinoosocket	*cursocket;

  XDASSERT(sched != NULL, -1);
  t1 = rinoo_socket_timeout_getmin(sched);
  t2 = jobqueue_gettimeout(sched);
  sched->stop = 0;
  while (sched->stop == 0 &&
	 sched->poller->poll(sched, MIN(t1, t2)) == 0)
    {
      rinoo_sched_clock(sched);
      while ((cursocket = rinoo_socket_getexpired(sched)) != NULL)
	{
	  cursocket->event_fsm(cursocket, EVENT_SCHED_TIMEOUT);
	}
      jobqueue_exec(sched);
      t1 = rinoo_socket_timeout_getmin(sched);
      t2 = jobqueue_gettimeout(sched);
    }
  return 0;
}
