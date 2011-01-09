/**
 * @file   xselect.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Jan 27 11:36:30 2010
 *
 * @brief  This file manages calls to select.
 *
 *
 */
#include	"rinoo/rinoo.h"

/**
 * Select initialization. It initializes internal fds sets
 * for reading and writing.
 *
 * @param sched Pointer to the scheduler to use.
 *
 * @return 0 if succeeds, else -1.
 */
int		xselect_init(t_sched *sched)
{
  t_selectdata	*data;

  XDASSERT(sched != NULL, -1);

  data = xcalloc(1, sizeof(*data));
  XASSERT(data != NULL, -1);
  FD_ZERO(&data->readfds);
  FD_ZERO(&data->writefds);
  if (sigemptyset(&data->sigmask) < 0)
    {
      xfree(data);
      XASSERT(0, -1);
    }
  if (sigaddset(&data->sigmask, SIGINT) < 0)
    {
      xfree(data);
      XASSERT(0, -1);
    }
/*   if (sigaddset(&data->sigmask, SIGPIPE) < 0) */
/*     { */
/*       xfree(data); */
/*       XASSERT(0, -1); */
/*     } */
  sched->poller_data = data;
  return (0);
}

/**
 * Destroys the poller in a scheduler. It just free's memory.
 *
 * @param sched Pointer to the scheduler to use.
 */
void		xselect_destroy(t_sched *sched)
{
  XDASSERTN(sched != NULL);
  XDASSERTN(sched->poller_data != NULL);

  xfree(sched->poller_data);
}

static void	xselect_findnfds(t_sched *sched)
{
  int		nfds;
  t_selectdata	*data;

  data = (t_selectdata *) sched->poller_data;
  for (nfds = data->nfds; nfds >= 0; nfds--)
    {
      if (FD_ISSET(nfds, &data->readfds) ||
	  FD_ISSET(nfds, &data->writefds))
	{
	  data->nfds = nfds;
	  return;
	}
    }
  data->nfds = 0;
}

/**
 * Insert a socket into the select sets. It calls FD_SET.
 *
 * @param socket Pointer to the socket to add.
 * @param mode Polling mode to use to add.
 *
 * @return 0 if succeeds, else -1.
 */
int		xselect_insert(t_socket *socket, t_schedevent mode)
{
  t_selectdata	*data;

  XDASSERT(socket != NULL, -1);
  XASSERTSTR(socket->fd < FD_SETSIZE, -1, "Too many file descriptors for select()");

  if ((socket->poll_mode & mode) == mode)
    return (0);
  data = (t_selectdata *) socket->sched->poller_data;
  if ((mode & EVENT_SCHED_IN) == EVENT_SCHED_IN)
    FD_SET(socket->fd, &data->readfds);
  if ((mode & EVENT_SCHED_OUT) == EVENT_SCHED_OUT)
    FD_SET(socket->fd, &data->writefds);
  socket->poll_mode |= mode;
  if (data->nfds < socket->fd)
    data->nfds = socket->fd;
  return (0);
}

/**
 * Adds a polling mode for a socket to the select sets. It calls FD_SET.
 *
 * @param socket Pointer to the socket to add.
 * @param mode Polling mode to use to add.
 *
 * @return 0 if succeeds, else -1.
 */
int		xselect_addmode(t_socket *socket, t_schedevent mode)
{
  XDASSERT(socket != NULL, -1);
  XASSERTSTR(socket->fd < FD_SETSIZE, -1, "Too many file descriptors for select()");

  return (xselect_insert(socket, mode));
}

/**
 * Deletes a polling mode for a socket from the select sets. It calls FD_CLR.
 *
 * @param socket Pointer to the socket to use.
 * @param mode Polling mode to delete.
 *
 * @return 0 if succeeds, else -1.
 */
int		xselect_delmode(t_socket *socket, t_schedevent mode)
{
  t_selectdata	*data;

  XDASSERT(socket != NULL, -1);
  XASSERTSTR(socket->fd < FD_SETSIZE, -1, "Too many file descriptors for select()");

  if ((socket->poll_mode & mode) == 0)
    return (0);
  data = (t_selectdata *) socket->sched->poller_data;
  if ((mode & EVENT_SCHED_IN) == EVENT_SCHED_IN)
    FD_CLR(socket->fd, &data->readfds);
  if ((mode & EVENT_SCHED_OUT) == EVENT_SCHED_OUT)
    FD_CLR(socket->fd, &data->writefds);
  socket->poll_mode -= mode;
  xselect_findnfds(socket->sched);
  return (0);
}

/**
 * Removes a socket from select sets. It calls FD_CLR.
 *
 * @param socket Pointer to the socket to remove from the select sets.
 *
 * @return 0 if succeeds, else -1.
 */
int		xselect_remove(t_socket *socket)
{
  t_selectdata	*data;

  XDASSERT(socket != NULL, -1);
  XASSERTSTR(socket->fd < FD_SETSIZE, -1, "Too many file descriptors for select()");

  if (socket->poll_mode == 0)
    return (0);
  data = (t_selectdata *) socket->sched->poller_data;
  FD_CLR(socket->fd, &data->readfds);
  FD_CLR(socket->fd, &data->writefds);
  socket->poll_mode = 0;
  xselect_findnfds(socket->sched);
  return (0);
}

/**
 * Start polling. It calls select.
 *
 * @param sched Pointer to the scheduler to use.
 *
 * @return 0 if succeeds, else -1.
 */
int		xselect_poll(t_sched *sched, u32 timeout)
{
  int		i;
  int		nbevents;
  t_socket	*cursocket;
  t_selectdata	*data;
  fd_set	readfds;
  fd_set	writefds;
  struct timespec	tv;

  XDASSERT(sched != NULL, -1);

  data = (t_selectdata *) sched->poller_data;
  memcpy(&readfds, &data->readfds, sizeof(readfds));
  memcpy(&writefds, &data->readfds, sizeof(writefds));
  tv.tv_sec = timeout / 1000;
  tv.tv_nsec = timeout % 1000 * 100;
  nbevents = pselect(data->nfds + 1, &readfds, &writefds, NULL, &tv, &data->sigmask);
  XASSERT(nbevents != -1, -1);
  for (i = 0; i <= data->nfds; i++)
    {
      if (FD_ISSET(i, &readfds))
	{
	  cursocket = sched_getsocket(sched, i);
	  cursocket->event_fsm(cursocket, EVENT_SCHED_IN);
	}
      if (FD_ISSET(i, &writefds))
	{
	  cursocket = sched_getsocket(sched, i);
	  /**
	   * If cursocket has been removed in the readfds step,
	   * sched_getsocket will return NULL.
	   */
	  if (cursocket != NULL)
	    cursocket->event_fsm(cursocket, EVENT_SCHED_OUT);
	}
    }
  return (0);
}
