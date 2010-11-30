/**
 * @file   scheduler.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2010
 * @date   Tue Dec 22 17:30:27 2009
 *
 * @brief  Functions to manage a scheduler.
 *
 *
 */

#include	"rinoo/rinoo.h"

extern t_pollerapi	pollers[NB_POLLERS];

static int	sched_clock(t_sched *sched);

/**
 * Creates a new scheduler.
 *
 *
 * @return A pointer to the new scheduler if succeeds, else NULL.
 */
t_sched		*sched_create()
{
  t_sched	*sched;

  sched = xcalloc(1, sizeof(*sched));
  XASSERTSTR(sched != NULL, NULL, "Cannot create a new scheduler");
  sched->poller = &pollers[DEFAULT_POLLER];
  sched->expirequeue = socket_expirequeue_create();
  if (sched->expirequeue == NULL)
    {
      xfree(sched);
      XASSERTSTR(0, NULL, "Timeout queue init failed");
    }
  sched->jobqueue = jobqueue_create();
  if (sched->jobqueue == NULL)
    {
      xfree(sched);
      XASSERTSTR(0, NULL, "Job queue init failed");
    }
  if (sched->poller->init(sched) == -1)
    {
      xfree(sched);
      XASSERTSTR(0, NULL, "Poller init failed");
    }
  sched_clock(sched);
  return (sched);
}

/**
 * Destroys a scheduler.
 *
 * @param sched Pointer to the scheduler to destroy.
 */
void		sched_destroy(t_sched *sched)
{
  int		i;

  XASSERTN(sched != NULL);

  /**
   * Destroying all pending sockets.
   */
  for (i = 0; i < SCHED_MAXFDS; i++)
    {
      if (sched->sock_pool[i] != NULL)
	{
	  sched->sock_pool[i]->event_fsm(sched->sock_pool[i],
					 EVENT_SCHED_STOP);
	}
    }
  socket_expirequeue_destroy(sched->expirequeue);
  jobqueue_destroy(sched->jobqueue);
  sched->poller->destroy(sched);
  xfree(sched);
}

/**
 * Returns the socket corresponding to a file descriptor.
 *
 * @param sched Pointer to the scheduler to use.
 * @param fd File descriptor (< SCHED_MAXFDS).
 *
 * @return A pointer to the corresponding socket if found, else NULL.
 */
t_socket	*sched_getsocket(t_sched *sched, int fd)
{
  XDASSERT(fd < SCHED_MAXFDS, NULL);

  return (sched->sock_pool[fd]);
}

/**
 * Stops the scheduler. It actually sets the stop flag
 * to end the scheduler loop.
 *
 * @param sched Pointer to the scheduler to stop.
 */
void		sched_stop(t_sched *sched)
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
int		sched_insert(t_socket *socket, t_schedevent mode, u32 timeout)
{
  XDASSERT(socket != NULL, -1);
  XASSERT(socket->fd < SCHED_MAXFDS, -1);

  if (socket_settimeout(socket, timeout) != 0)
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
int		sched_addmode(t_socket *socket, t_schedevent mode)
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
int		sched_delmode(t_socket *socket, t_schedevent mode)
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
int		sched_remove(t_socket *socket)
{
  if (socket->sched->poller->remove(socket) != 0)
    return (-1);
  socket->sched->sock_pool[socket->fd] = NULL;
  socket_removetimeout(socket);
  return (0);
}

/**
 * Set the scheduler clock with gettimeofday.
 *
 * @param sched Pointer to the scheduler to use.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
static int	sched_clock(t_sched *sched)
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
int		sched_loop(t_sched *sched)
{
  u32		t1;
  u32		t2;
  t_socket	*cursocket;

  XDASSERT(sched != NULL, -1);
  t1 = socket_gettimeout(sched);
  t2 = jobqueue_gettimeout(sched);
  while (sched->stop == 0 &&
	 sched->poller->poll(sched, MIN(t1, t2)) == 0)
    {
      sched_clock(sched);
      while ((cursocket = socket_getexpired(sched)) != NULL)
	{
	  cursocket->event_fsm(cursocket, EVENT_SCHED_TIMEOUT);
	}
      jobqueue_exec(sched);
      t1 = socket_gettimeout(sched);
      t2 = jobqueue_gettimeout(sched);
    }
  return (0);
}
