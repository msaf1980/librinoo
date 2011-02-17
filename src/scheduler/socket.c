/**
 * @file   socket.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Dec 30 17:25:49 2009
 *
 * @brief  All functions needed to manage sockets.
 *
 *
 */

#include	"rinoo/rinoo.h"

/**
 * Compares to sockets depending on their expiration time.
 *
 * @param node1 Pointer to the first socket.
 * @param node2 Pointer to the second socket.
 *
 * @return Returns an integer less than, equal to, or greater than zero if node1, respectively, is less than, matches, or is greater than node2.
 */
int		rinoo_socket_timeout_cmp(void *node1, void *node2)
{
  t_rinoosocket	*sock1 = (t_rinoosocket *) node1;
  t_rinoosocket	*sock2 = (t_rinoosocket *) node2;

  XDASSERT(node1 != NULL, 1);
  XDASSERT(node2 != NULL, 1);

  if (node1 == node2)
    return 0;
  if (sock1->timeout.expire.tv_sec > sock2->timeout.expire.tv_sec)
    return 1;
  if (sock1->timeout.expire.tv_sec < sock2->timeout.expire.tv_sec)
    return -1;
  if (sock1->timeout.expire.tv_usec > sock2->timeout.expire.tv_usec)
    return 1;
  if (sock1->timeout.expire.tv_usec < sock2->timeout.expire.tv_usec)
    return -1;

  /* Always make a difference between socket timeouts */
  return 1;
}

/**
 * Set the socket timeout (or expiration time).
 *
 * @param socket Pointer to the socket to modify.
 * @param timeout Number of milliseconds representing the timeout.
 *
 * @return 0 on success, or -1 if an error occured.
 */
int	rinoo_socket_timeout_set(t_rinoosocket *socket, u32 timeout)
{
  XDASSERT(socket != NULL, -1);
  XDASSERT(socket->sched != NULL, -1);

  if (socket->timeout.node != NULL &&
      unlikely(list_popnode(socket->sched->timeoutq,
			    socket->timeout.node) != 0))
    {
      return -1;
    }
  socket->timeout.ms = timeout;
  if (timeout != 0)
    {
      timeraddms(&socket->sched->curtime, timeout, &socket->timeout.expire);
      if (socket->timeout.node == NULL)
	{
	  socket->timeout.node = list_add(socket->sched->timeoutq, socket, NULL);
	  if (unlikely(socket->timeout.node == NULL))
	    {
	      return -1;
	    }
	}
      else
	{
	  if (unlikely(list_addnode(socket->sched->timeoutq,
				    socket->timeout.node) != 0))
	    {
	      return -1;
	    }
	}
    }
  return 0;
}

/**
 * Reset the socket timeout (or expiration time).
 * This function uses the previously declared timeout (with rinoo_socket_settimeout)
 * and re-compute the next expiration time.
 *
 * @param socket Pointer to the socket to modify.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int	rinoo_socket_timeout_reset(t_rinoosocket *socket)
{
  return (rinoo_socket_timeout_set(socket, socket->timeout.ms));
}

/**
 * Remove the timeout of a socket. It remove the socket from the expire queue.
 *
 * @param socket Pointer to the socket to remove from expire queue.
 */
void	rinoo_socket_timeout_remove(t_rinoosocket *socket)
{
  XDASSERTN(socket != NULL);

  if (socket->timeout.node != NULL)
    {
      list_removenode(socket->sched->timeoutq, socket->timeout.node, TRUE);
      socket->timeout.node = NULL;
    }
}

/**
 * Get the smallest socket timeout from the expire queue.
 *
 * @param sched Pointer to the scheduler to use.
 *
 * @return The number of milliseconds before a socket expires.
 */
u32		rinoo_socket_timeout_getmin(t_rinoosched *sched)
{
  t_rinoosocket	*socket;

  XDASSERT(sched != NULL, RINOO_JOBQUEUE_TIMEOUT);
  XDASSERT(sched->timeoutq != NULL, RINOO_JOBQUEUE_TIMEOUT);

  socket = list_gethead(sched->timeoutq);
  if (socket == NULL)
    return (RINOO_JOBQUEUE_TIMEOUT);
  if (sched->curtime.tv_sec > socket->timeout.expire.tv_sec)
    return (0);
  return ((socket->timeout.expire.tv_sec - sched->curtime.tv_sec) * 1000 +
	  (socket->timeout.expire.tv_usec - sched->curtime.tv_usec) / 1000);
}

/**
 * Get the next expired socket.
 *
 * @param sched Pointer to the scheduler to use.
 *
 * @return A pointer to an expired socket, or NULL all sockets are not expired yet.
 */
t_rinoosocket	*rinoo_socket_getexpired(t_rinoosched *sched)
{
  t_rinoosocket	*socket;

  XDASSERT(sched != NULL, NULL);
  XDASSERT(sched->timeoutq != NULL, NULL);

  socket = list_gethead(sched->timeoutq);
  if (socket != NULL &&
      timercmp(&sched->curtime, &socket->timeout.expire, >=) != 0)
    {
      socket = list_pophead(sched->timeoutq);
      if (socket == NULL)
	{
	  return NULL;
	}
      socket->timeout.node = NULL;
      return socket;
    }
  return NULL;
}
