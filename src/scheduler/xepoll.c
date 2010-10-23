/**
 * @file   xepoll.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2010
 * @date   Mon Dec 28 01:27:39 2009
 *
 * @brief  This file manages calls to epoll.
 *
 *
 */
#include	"rinoo/rinoo.h"

/**
 * Epoll initialization. It calls epoll_create and
 * initializes internal structures.
 *
 * @param sched Pointer to the scheduler to use.
 *
 * @return 0 if succeeds, else -1.
 */
int		xepoll_init(t_sched *sched)
{
  t_epolldata	*data;

  data = xcalloc(1, sizeof(*data));
  XASSERT(data != NULL, -1);
  data->fd = epoll_create(42); /* Size does not matter any more ;) */
  XASSERT(data->fd != -1, -1);
  if (sigemptyset(&data->sigmask) < 0)
    {
      close(data->fd);
      xfree(data);
      XASSERT(0, -1);
    }
  /* if (sigaddset(&data->sigmask, SIGINT) < 0) */
  /*   { */
  /*     close(data->fd); */
  /*     xfree(data); */
  /*     XASSERT(0, -1); */
  /*   } */
  if (sigaddset(&data->sigmask, SIGPIPE) < 0)
    {
      close(data->fd);
      xfree(data);
      XASSERT(0, -1);
    }
  sigaction(SIGPIPE, &(struct sigaction){ .sa_handler = SIG_IGN }, NULL);
  sched->poller_data = data;
  return (0);
}

/**
 * Destroys the poller in a scheduler. Closes the epoll fd and free's memory.
 *
 * @param sched Pointer to the scheduler to use.
 */
void		xepoll_destroy(t_sched *sched)
{
  XDASSERTN(sched != NULL);
  XDASSERTN(sched->poller_data != NULL);

  close(((t_epolldata *) sched->poller_data)->fd);
  xfree(sched->poller_data);
}

/**
 * Insert a socket into epoll. It calls epoll_ctl.
 *
 * @param socket Pointer to the socket to add.
 * @param mode Polling mode to use to add.
 *
 * @return 0 if succeeds, else -1.
 */
int		xepoll_insert(t_socket *socket, t_schedevent mode)
{
  struct epoll_event	ev = { 0, { 0 } };

  XDASSERT(socket != NULL, -1);

  if ((socket->poll_mode & mode) == mode)
    return (0);
  if ((mode & EVENT_SCHED_IN) == EVENT_SCHED_IN)
    ev.events |= EPOLLIN;
  if ((mode & EVENT_SCHED_OUT) == EVENT_SCHED_OUT)
    ev.events |= EPOLLOUT;
  ev.events |= EPOLLRDHUP;
  ev.data.fd = socket->fd;
  XASSERT(epoll_ctl(((t_epolldata *) socket->sched->poller_data)->fd,
		    EPOLL_CTL_ADD, socket->fd, &ev) >= 0, -1);
  socket->poll_mode |= mode;
  return (0);
}

/**
 * Adds a polling mode for a socket to epoll. It calls epoll_ctl.
 *
 * @param socket Pointer to the socket to add.
 * @param mode Polling mode to use to add.
 *
 * @return 0 if succeeds, else -1.
 */
int		xepoll_addmode(t_socket *socket, t_schedevent mode)
{
  struct epoll_event	ev = { 0, { 0 } };

  XDASSERT(socket != NULL, -1);

  if ((socket->poll_mode & mode) == mode)
    return (0);
  mode |= socket->poll_mode;
  if ((mode & EVENT_SCHED_IN) == EVENT_SCHED_IN)
    ev.events |= EPOLLIN;
  if ((mode & EVENT_SCHED_OUT) == EVENT_SCHED_OUT)
    ev.events |= EPOLLOUT;
  ev.events |= EPOLLRDHUP;
  ev.data.fd = socket->fd;
  XASSERT(epoll_ctl(((t_epolldata *) socket->sched->poller_data)->fd,
		    EPOLL_CTL_MOD, socket->fd, &ev) >= 0, -1);
  socket->poll_mode = mode;
  return (0);
}

/**
 * Deletes a polling mode for a socket from epoll. It calls epoll_ctl.
 *
 * @param socket Pointer to the socket to use.
 * @param mode Polling mode to delete.
 *
 * @return 0 if succeeds, else -1.
 */
int		xepoll_delmode(t_socket *socket, t_schedevent mode)
{
  struct epoll_event	ev = { 0, { 0 } };

  XDASSERT(socket != NULL, -1);

  if ((socket->poll_mode & mode) == 0)
    return (0);
  mode = socket->poll_mode - mode;
  if ((mode & EVENT_SCHED_IN) == EVENT_SCHED_IN)
    ev.events |= EPOLLIN;
  if ((mode & EVENT_SCHED_OUT) == EVENT_SCHED_OUT)
    ev.events |= EPOLLOUT;
  ev.events |= EPOLLRDHUP;
  ev.data.fd = socket->fd;
  XASSERT(epoll_ctl(((t_epolldata *) socket->sched->poller_data)->fd,
		    EPOLL_CTL_MOD,
		    socket->fd,
		    &ev) >= 0, -1);
  socket->poll_mode = mode;
  return (0);
}

/**
 * Removes a socket from epoll. It calls epoll_ctl.
 *
 * @param socket Pointer to the socket to remove from epoll.
 *
 * @return 0 if succeeds, else -1.
 */
int		xepoll_remove(t_socket *socket)
{
  XDASSERT(socket != NULL, -1);

  if (socket->poll_mode == 0)
    return (0);
  XASSERT(epoll_ctl(((t_epolldata *) socket->sched->poller_data)->fd,
		    EPOLL_CTL_DEL,
		    socket->fd,
		    NULL) >= 0, -1);
  socket->poll_mode = 0;
  return (0);
}

/**
 * Start polling. It calls epoll_wait.
 *
 * @param sched Pointer to the scheduler to use.
 *
 * @return 0 if succeeds, else -1.
 */
int		xepoll_poll(t_sched *sched, u32 timeout)
{
  int		i;
  int		nbevents;
  t_socket	*cursocket;
  t_epolldata	*data;

  XDASSERT(sched != NULL, -1);

  data = (t_epolldata *) sched->poller_data;
  nbevents = epoll_pwait(data->fd, data->events, MAX_EVENTS, timeout, &data->sigmask);
  if (unlikely(nbevents == -1))
    {
      /* We don't want to raise an error in this case */
      return (0);
    }
  for (i = 0; i < nbevents; i++)
    {
      cursocket = sched_getsocket(sched, data->events[i].data.fd);
      if ((data->events[i].events & EPOLLIN) == EPOLLIN)
	{
	  cursocket->event_fsm(cursocket, EVENT_SCHED_IN);
	}
      cursocket = sched_getsocket(sched, data->events[i].data.fd);
      /**
       * If cursocket has been removed in the EPOLLIN step,
       * sched_getsocket will return NULL.
       */
      if (cursocket != NULL &&
	  (data->events[i].events & EPOLLOUT) == EPOLLOUT)
	{
	  cursocket->event_fsm(cursocket, EVENT_SCHED_OUT);
	}
      cursocket = sched_getsocket(sched, data->events[i].data.fd);
      if (cursocket != NULL &&
	  ((data->events[i].events & EPOLLERR) == EPOLLERR ||
	   (data->events[i].events & EPOLLERR) == EPOLLRDHUP ||
	   (data->events[i].events & EPOLLHUP) == EPOLLHUP))
	{
	  cursocket->event_fsm(cursocket, EVENT_SCHED_ERROR);
	}
      cursocket = sched_getsocket(sched, data->events[i].data.fd);
      if (cursocket != NULL)
	{
	  socket_resettimeout(cursocket);
	}
    }
  return (0);
}
