/**
 * @file   socket.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2010
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
static int	socket_expirecmp(void *node1, void *node2)
{
  t_socket	*sock1 = (t_socket *) node1;
  t_socket	*sock2 = (t_socket *) node2;

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
 * Create a new list called 'expire queue' to store socket
 * sorted by their expiration time.
 *
 *
 * @return Pointer to the new list, or NULL if an error occurs.
 */
t_list	*socket_expirequeue_create()
{
  return (list_create(socket_expirecmp));
}

/**
 * Destroy an expire queue.
 *
 * @param expirequeue Pointer to the expire queue to destroy
 */
void	socket_expirequeue_destroy(t_list *expirequeue)
{
  XDASSERTN(expirequeue != NULL);

  list_destroy(expirequeue);
}

/**
 * Set the socket timeout (or expiration time).
 *
 * @param socket Pointer to the socket to modify.
 * @param timeout Number of milliseconds representing the timeout.
 *
 * @return 0 on success, or -1 if an error occured.
 */
int	socket_settimeout(t_socket *socket, u32 timeout)
{
  XDASSERT(socket != NULL, -1);
  XDASSERT(socket->sched != NULL, -1);

  if (socket->timeout.node != NULL &&
      unlikely(list_removenode(socket->sched->expirequeue,
			       socket->timeout.node, FALSE) == FALSE))
    {
      return (-1);
    }
  socket->timeout.ms = timeout;
  if (timeout != 0)
    {
      timeraddms(&socket->sched->curtime, timeout, &socket->timeout.expire);
      socket->timeout.node = list_add(socket->sched->expirequeue,
				      socket,
				      NULL);
      if (unlikely(socket->timeout.node == NULL))
	{
	  return (-1);
	}
    }
  return (0);
}

/**
 * Reset the socket timeout (or expiration time).
 * This function uses the previously declared timeout (with socket_settimeout)
 * and re-compute the next expiration time.
 *
 * @param socket Pointer to the socket to modify.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int	socket_resettimeout(t_socket *socket)
{
  return (socket_settimeout(socket, socket->timeout.ms));
}

/**
 * Remove the timeout of a socket. It remove the socket from the expire queue.
 *
 * @param socket Pointer to the socket to remove from expire queue.
 */
void	socket_removetimeout(t_socket *socket)
{
  XDASSERTN(socket != NULL);

  if (socket->timeout.node != NULL)
    {
      list_removenode(socket->sched->expirequeue, socket->timeout.node, TRUE);
    }
}

/**
 * Get the smallest socket timeout from the expire queue.
 *
 * @param sched Pointer to the scheduler to use.
 *
 * @return The number of milliseconds before a socket expires.
 */
u32		socket_gettimeout(t_sched *sched)
{
  t_socket	*socket;

  XDASSERT(sched != NULL, DEFAULT_TIMEOUT);
  XDASSERT(sched->expirequeue != NULL, DEFAULT_TIMEOUT);

  socket = list_gethead(sched->expirequeue);
  if (socket == NULL)
    return (DEFAULT_TIMEOUT);
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
t_socket	*socket_getexpired(t_sched *sched)
{
  t_socket	*socket;

  XDASSERT(sched != NULL, NULL);
  XDASSERT(sched->expirequeue != NULL, NULL);

  socket = list_gethead(sched->expirequeue);
  if (socket != NULL &&
      timercmp(&sched->curtime, &socket->timeout.expire, >=) != 0)
    {
      socket = list_pophead(sched->expirequeue);
      if (socket == NULL)
	{
	  return NULL;
	}
      socket->timeout.node = NULL;
      return socket;
    }
  return NULL;
}
