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

static int	rinoo_sched_clock(t_rinoosched *sched);

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
  sched->poller = &pollers[DEFAULT_POLLER];
  sched->timeoutq = list_create(rinoo_socket_timeout_cmp);
  if (sched->timeoutq == NULL)
    {
      xfree(sched);
      XASSERTSTR(0, NULL, "Timeout queue init failed");
    }
  sched->jobq = jobqueue_create();
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
  rinoo_sched_clock(sched);
  return (sched);
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

  /**
   * Destroying all pending sockets.
   */
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
 * Returns the socket corresponding to a file descriptor.
 *
 * @param sched Pointer to the scheduler to use.
 * @param fd File descriptor (< RINOO_SCHED_MAXFDS).
 *
 * @return A pointer to the corresponding socket if found, else NULL.
 */
t_rinoosocket	*rinoo_sched_getsocket(t_rinoosched *sched, int fd)
{
  XDASSERT(fd < RINOO_SCHED_MAXFDS, NULL);

  return (sched->sock_pool[fd]);
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
 * Insert a socket into the scheduler.
 *
 * @param socket Pointer to the socket to use.
 * @param timeout Maximum number of milliseconds before raising a timeout.
 * @param mode Polling mode to add in the scheduler for this socket.
 *
 * @return 0 if succeeds, else -1.
 */
int		rinoo_sched_insert(t_rinoosocket *socket,
				   t_rinoosched_event mode,
				   u32 timeout)
{
  XDASSERT(socket != NULL, -1);
  XASSERT(socket->fd < RINOO_SCHED_MAXFDS, -1);

  if (rinoo_socket_timeout_set(socket, timeout) != 0)
    {
      return (-1);
    }
  if (socket->sched->poller->insert(socket, mode) != 0)
    return (-1);
  socket->sched->sock_pool[socket->fd] = socket;
  return (0);
}

/**
 * Add a polling mode for a socket in the scheduler.
 *
 * @param socket Pointer to the socket to use.
 * @param mode Polling mode to add in the scheduler for this socket.
 *
 * @return 0 if succeeds, else -1.
 */
int		rinoo_sched_addmode(t_rinoosocket *socket,
				    t_rinoosched_event mode)
{
  return (socket->sched->poller->addmode(socket, mode));
}

/**
 * Delete a polling mode for a socket in the scheduler.
 *
 * @param socket Pointer to the socket to remove from the scheduler.
 * @param mode Polling mode to remove from the scheduler for this socket.
 *
 * @return 0 if succeeds, else -1.
 */
int		rinoo_sched_delmode(t_rinoosocket *socket,
				    t_rinoosched_event mode)
{
  return (socket->sched->poller->delmode(socket, mode));
}

/**
 * Remove a socket from the scheduler.
 *
 * @param socket Pointer to the socket to remove from the scheduler.
 *
 * @return 0 if succeeds, else -1.
 */
int		rinoo_sched_remove(t_rinoosocket *socket)
{
  if (socket->sched->poller->remove(socket) != 0)
    return (-1);
  socket->sched->sock_pool[socket->fd] = NULL;
  rinoo_socket_timeout_remove(socket);
  return (0);
}

/**
 * Set the scheduler clock with gettimeofday.
 *
 * @param sched Pointer to the scheduler to use.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
static int	rinoo_sched_clock(t_rinoosched *sched)
{
  return gettimeofday(&sched->curtime, NULL);
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
  return (0);
}
