/**
 * @file   udp.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Mon Dec 28 22:29:31 2009
 *
 * @brief  Manages a UDP socket based on asynchronous event system as epoll.
 *
 *
 */

#include	"rinoo/rinoo.h"

static inline void	rinoo_udp_error(t_rinooudp *udpsock,
					t_rinooudp_event errorstep);
static void		rinoo_udp_fsm(t_rinoosocket *socket,
				      t_rinoosched_event event);

/**
 * Creates a new udp structure.
 *
 * @param sched Pointer to the scheduler to use.
 * @param ip If server mode, IP to bind, if client mode, destination IP.
 * @param port If server mode, port to bind, if client mode, destination port.
 * @param mode Mode to use for this socket (server/client).
 *
 * @return Pointer to the new udp structure, or NULL if an error occurs.
 */
static t_rinooudp	*rinoo_udp_init(t_rinoosched *sched,
					t_ip ip,
					u32 port,
					t_rinooudp_mode mode)
{
  int			enabled;
  t_rinooudp		*udpsock;

  XDASSERT(sched != NULL, NULL);

  udpsock = xcalloc(1, sizeof(*udpsock));
  XASSERT(udpsock != NULL, NULL);
  udpsock->socket.fd = -1;
  udpsock->socket.sched = sched;
  udpsock->socket.rdbuf = buffer_create(RDBUF_INITSIZE, RDBUF_MAXSIZE);
  if (udpsock->socket.rdbuf == NULL)
    {
      rinoo_udp_destroy(udpsock);
      XASSERT(0, NULL);
    }
  udpsock->socket.wrbuf = buffer_create(WRBUF_INITSIZE, WRBUF_MAXSIZE);
  if (udpsock->socket.wrbuf == NULL)
    {
      rinoo_udp_destroy(udpsock);
      XASSERT(0, NULL);
    }
  udpsock->socket.data = udpsock;
  udpsock->ip = ip;
  udpsock->port = port;
  udpsock->mode = mode;
  if ((udpsock->socket.fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
      rinoo_udp_destroy(udpsock);
      XASSERT(0, NULL);
    }
  enabled = 1;
  if (ioctl(udpsock->socket.fd, FIONBIO, &enabled) == -1)
    {
      rinoo_udp_destroy(udpsock);
      XASSERT(0, NULL);
    }
  return udpsock;
}

/**
 * Creates a new udp client socket and add it to a scheduler.
 *
 * @param sched Pointer to the scheduler to use.
 * @param ip If server mode, IP to bind, if client mode, destination IP.
 * @param port If server mode, port to bind, if client mode, destination port.
 * @param timeout Max number of milliseconds without activity.
 * @param event_fsm Pointer to the function to call for each event.
 *
 * @return Pointer to the new udp structure, or NULL if an error occurs.
 */
t_rinooudp	*rinoo_udp_client(t_rinoosched *sched,
				  t_ip ip,
				  u32 port,
				  u32 timeout,
				  void (*event_fsm)(t_rinooudp *udpsock,
						    t_rinooudp_event event))
{
  t_rinooudp		*udpsock;
  struct sockaddr_in    addr;

  XDASSERT(sched != NULL, NULL);

  udpsock = rinoo_udp_init(sched, ip, port, RINOO_UDP_CLIENT);
  if (unlikely(udpsock == NULL))
    {
      return NULL;
    }
  addr.sin_family = AF_INET;
  addr.sin_port = htons(udpsock->port);
  addr.sin_addr.s_addr = udpsock->ip;
  /* Yes, connect can be used on UDP socket to set the default destination */
  if (connect(udpsock->socket.fd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
    {
      rinoo_udp_destroy(udpsock);
      XASSERT(0, NULL);
    }
  udpsock->socket.event_fsm = rinoo_udp_fsm;
  if (rinoo_sched_socket(RINOO_SCHED_INSERT,
			 &udpsock->socket,
			 EVENT_SCHED_OUT) == -1)
    {
      rinoo_udp_destroy(udpsock);
      XASSERT(0, NULL);
    }
  if (unlikely(rinoo_socket_timeout_set(&udpsock->socket, timeout) != 0))
    {
      rinoo_udp_destroy(udpsock);
      XASSERT(0, NULL);
    }
  udpsock->event_fsm = event_fsm;
  return udpsock;
}

/**
 * Creates a new udp server socket and add it to a scheduler.
 *
 * @param sched Pointer to the scheduler to use.
 * @param ip If server mode, IP to bind, if client mode, destination IP.
 * @param port If server mode, port to bind, if client mode, destination port.
 * @param timeout Max number of milliseconds without activity.
 * @param event_fsm Pointer to the function to call for each event.
 *
 * @return Pointer to the new udp structure, or NULL if an error occurs.
 */
t_rinooudp	*rinoo_udp_server(t_rinoosched *sched,
				  t_ip ip,
				  u32 port,
				  u32 timeout,
				  void (*event_fsm)(t_rinooudp *udpsock,
						    t_rinooudp_event event))
{
  int			enabled;
  t_rinooudp		*udpsock;
  struct sockaddr_in    addr;

  XDASSERT(sched != NULL, NULL);

  udpsock = rinoo_udp_init(sched, ip, port, RINOO_UDP_SERVER);
  if (unlikely(udpsock == NULL))
    {
      return NULL;
    }
  addr.sin_family = AF_INET;
  addr.sin_port = htons(udpsock->port);
  addr.sin_addr.s_addr = udpsock->ip;
  enabled = 1;
  if (setsockopt(udpsock->socket.fd,
		 SOL_SOCKET,
		 SO_REUSEADDR,
		 &enabled,
		 sizeof(enabled)) == -1 ||
      bind(udpsock->socket.fd,
	   (struct sockaddr *) &addr,
	   sizeof(addr)) == -1)
    {
      rinoo_udp_destroy(udpsock);
      XASSERT(0, NULL);
    }
  udpsock->socket.event_fsm = rinoo_udp_fsm;
  if (rinoo_sched_socket(RINOO_SCHED_INSERT,
			 &udpsock->socket,
			 EVENT_SCHED_IN) == -1)
    {
      rinoo_udp_destroy(udpsock);
      XASSERT(0, NULL);
    }
  /*
   * We just keep the timeout value.
   * Scheduler is not aware about any timeout for this socket.
   */
  udpsock->socket.timeout.ms = timeout;
  udpsock->event_fsm = event_fsm;
  return udpsock;
}

/**
 * Destroys a udp socket. This function removes the corresponding
 * file descriptor from the scheduler, closes the fd, and free's memory used.
 *
 * @param udpsock Pointer to the udp socket.
 */
void	rinoo_udp_destroy(t_rinooudp *udpsock)
{
  XDASSERTN(udpsock != NULL);

  if (udpsock->socket.fd > -1)
    {
      if (rinoo_sched_get(udpsock->socket.sched, udpsock->socket.fd) != NULL)
	{
	  rinoo_sched_socket(RINOO_SCHED_REMOVE, &udpsock->socket, 0);
	}
      close(udpsock->socket.fd);
    }
  if (udpsock->socket.rdbuf != NULL)
    {
      buffer_destroy(udpsock->socket.rdbuf);
    }
  if (udpsock->socket.wrbuf != NULL)
    {
      buffer_destroy(udpsock->socket.wrbuf);
    }
  xfree(udpsock);
}

/**
 * Generates a udp error. It simply sets the error step (where the error
 * occured), calls the child fsm to raise the error, and finally destroys
 * the socket.
 *
 * @param udpsock Pointer to the udp socket.
 * @param errorstep Error step, to specify where the error occured.
 */
static inline void	rinoo_udp_error(t_rinooudp *udpsock,
					t_rinooudp_event errorstep)
{
  XDASSERTN(udpsock != NULL);

  udpsock->errorstep = errorstep;
  udpsock->event_fsm(udpsock, EVENT_UDP_ERROR);
  rinoo_udp_destroy(udpsock);
}

/**
 * Global udp fsm. This function is called to raise every event on the socket.
 *
 * @param socket Pointer to the socket to use.
 * @param event Scheduler event which is raised.
 */
static void	rinoo_udp_fsm(t_rinoosocket *socket,
			      t_rinoosched_event event)
{
  int		res;
  t_rinooudp	*udpsock;

  XDASSERTN(socket != NULL);
  XDASSERTN(socket->data != NULL);

  udpsock = socket->data;
  switch (event)
    {
    case EVENT_SCHED_IN:
      if (buffer_len(socket->rdbuf) == RDBUF_MAXSIZE)
	{
	  rinoo_udp_error(udpsock, EVENT_UDP_IN);
	  return;
	}
      res = read(socket->fd,
		 buffer_ptr(socket->rdbuf) + buffer_len(socket->rdbuf),
		 buffer_size(socket->rdbuf) - buffer_len(socket->rdbuf));
      if (unlikely(res < 0))
	{
	  rinoo_udp_error(udpsock, EVENT_UDP_IN);
	  return;
	}
      else if (res == 0)
	{
	  udpsock->event_fsm(udpsock, EVENT_UDP_CLOSE);
	  rinoo_udp_destroy(udpsock);
	  return;
	}
      buffer_setlen(socket->rdbuf, buffer_len(socket->rdbuf) + res);
      if (buffer_isfull(socket->rdbuf))
	buffer_extend(socket->rdbuf, 0);
      udpsock->event_fsm(udpsock, EVENT_UDP_IN);
      break;
    case EVENT_SCHED_OUT:
      if (buffer_len(socket->wrbuf) > 0)
	{
	  res = write(socket->fd,
		      buffer_ptr(socket->wrbuf),
		      buffer_len(socket->wrbuf));
	  if (res < 0)
	    {
	      rinoo_udp_error(udpsock, EVENT_UDP_OUT);
	      return;
	    }
	  buffer_erase(socket->wrbuf, res);
	}
      if (buffer_len(socket->wrbuf) <= 0 &&
	  rinoo_sched_socket(RINOO_SCHED_MODDEL, socket, EVENT_SCHED_OUT) == -1)
	{
	  rinoo_udp_error(udpsock, EVENT_UDP_OUT);
	  return;
	}
      udpsock->event_fsm(udpsock, EVENT_UDP_OUT);
      break;
    case EVENT_SCHED_TIMEOUT:
      udpsock->event_fsm(udpsock, EVENT_UDP_TIMEOUT);
      rinoo_udp_destroy(udpsock);
      break;
    case EVENT_SCHED_ERROR:
      rinoo_udp_error(udpsock, EVENT_UDP_ERROR);
      break;
    case EVENT_SCHED_STOP:
      udpsock->event_fsm(udpsock, EVENT_UDP_CLOSE);
      rinoo_udp_destroy(udpsock);
      break;
    }
}

/**
 * Prints a string into the write buffer of a udp socket.
 *
 * @param udpsock Pointer to the socket to write into.
 * @param format printf-like format string
 *
 * @return Number of bytes printed if succeeds, else -1.
 */
int		rinoo_udp_print(t_rinooudp *udpsock, const char *format, ...)
{
  int		res;
  va_list	ap;

  XDASSERT(udpsock != NULL, -1);

  va_start(ap, format);
  res = buffer_vprint(udpsock->socket.wrbuf, format, ap);
  va_end(ap);
  if (res > -1)
    {
      rinoo_sched_socket(RINOO_SCHED_MODADD, &udpsock->socket, EVENT_SCHED_OUT);
    }
  return res;
}

/**
 * Adds data into the write buffer of a udp socket.
 *
 * @param udpsock Pointer to the socket to write into.
 * @param format printf-like format string
 *
 * @return Number of bytes printed if succeeds, else -1.
 */
int		rinoo_udp_printdata(t_rinooudp *udpsock,
				    const char *data,
				    size_t size)
{
  int		res;

  XDASSERT(udpsock != NULL, -1);

  res = buffer_add(udpsock->socket.wrbuf, data, size);
  if (res > -1)
    {
      rinoo_sched_socket(RINOO_SCHED_MODADD, &udpsock->socket, EVENT_SCHED_OUT);
    }
  return res;
}

/**
 * This function says if a udp socket is set to server mode or not.
 *
 * @param udpsock Pointer to the socket to check.
 *
 * @return A non-zero value if udpsock is a server socket, otherwise 0.
 */
int		rinoo_udp_isserver(t_rinooudp *udpsock)
{
  return (udpsock->mode == RINOO_UDP_SERVER);
}
