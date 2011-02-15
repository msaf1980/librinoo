/**
 * @file   tcp.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Mon Dec 28 22:29:31 2009
 *
 * @brief  Manages a TCP connection based on asynchronous event system as epoll.
 *
 *
 */

#include	"rinoo/rinoo.h"

static inline void	rinoo_tcp_error(t_rinootcp *tcpsock,
					t_rinootcp_event errorstep);
static void		rinoo_tcp_accept(t_rinoosocket *socket,
					 t_rinoosched_event event);
static void		rinoo_tcp_connect(t_rinoosocket *socket,
					  t_rinoosched_event event);
static void		rinoo_tcp_fsm(t_rinoosocket *socket,
				      t_rinoosched_event event);


/**
 * Creates a new tcp structure.
 *
 * @param sched Pointer to the scheduler to use.
 * @param fd File descriptor of the tcp socket.
 * @param ip If server mode, IP to bind, if client mode, destination IP.
 * @param port If server mode, port to bind, if client mode, destination port.
 * @param mode Mode to use for this connection (server/client).
 *
 * @return Pointer to the new rinootcp structure, or NULL if an error occurs.
 */
static t_rinootcp	*rinoo_tcp_init(t_rinoosched *sched,
					int fd,
					t_ip ip,
					u16 port,
					t_rinootcp_mode mode)
{
  int			enabled;
  t_rinootcp		*tcpsock;

  XDASSERT(sched != NULL, NULL);

  tcpsock = xcalloc(1, sizeof(*tcpsock));
  XASSERT(tcpsock != NULL, NULL);
  tcpsock->socket.fd = -1;
  tcpsock->socket.sched = sched;
  tcpsock->socket.rdbuf = buffer_create(RDBUF_INITSIZE, RDBUF_MAXSIZE);
  if (tcpsock->socket.rdbuf == NULL)

    {
      rinoo_tcp_destroy(tcpsock);
      XASSERT(0, NULL);
    }
  tcpsock->socket.wrbuf = buffer_create(WRBUF_INITSIZE, WRBUF_MAXSIZE);
  if (tcpsock->socket.wrbuf == NULL)
    {
      rinoo_tcp_destroy(tcpsock);
      XASSERT(0, NULL);
    }
  tcpsock->socket.data = tcpsock;
  tcpsock->ip = ip;
  tcpsock->port = port;
  tcpsock->mode = mode;
  tcpsock->socket.fd = fd;
  enabled = 1;
  if (ioctl(tcpsock->socket.fd, FIONBIO, &enabled) == -1)
    {
      rinoo_tcp_destroy(tcpsock);
      XASSERT(0, NULL);
    }
  return tcpsock;
}

/**
 * Creates a new tcp connection and add it to a scheduler.
 *
 * @param sched Pointer to the scheduler to use.
 * @param ip If server mode, IP to bind, if client mode, destination IP.
 * @param port If server mode, port to bind, if client mode, destination port.
 * @param timeout Max number of milliseconds without activity.
 * @param event_fsm Pointer to the function to call for each event.
 *
 * @return Pointer to the new tcp structure, or NULL if an error occurs.
 */
t_rinootcp	*rinoo_tcp_client(t_rinoosched *sched,
				  t_ip ip,
				  u16 port,
				  u32 timeout,
				  void (*event_fsm)(t_rinootcp *tcpsock,
						    t_rinootcp_event event))
{
  int			fd;
  t_rinootcp		*tcpsock;
  struct sockaddr_in    addr;

  XDASSERT(sched != NULL, NULL);

  if (unlikely((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1))
    {
      XASSERT(0, NULL);
    }
  tcpsock = rinoo_tcp_init(sched, fd, ip, port, RINOO_TCP_CLIENT);
  if (unlikely(tcpsock == NULL))
    {
      return NULL;
    }
  addr.sin_port = htons(tcpsock->port);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = tcpsock->ip;
  if (connect(tcpsock->socket.fd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
    {
      switch (errno)
	{
	case EINPROGRESS:
	case EALREADY:
	  break;
	default:
	  rinoo_tcp_destroy(tcpsock);
	  XASSERT(0, NULL);
	}
    }
  tcpsock->socket.event_fsm = rinoo_tcp_connect;
  if (rinoo_sched_socket(RINOO_SCHED_INSERT,
			 &tcpsock->socket,
			 EVENT_SCHED_OUT) == -1)
    {
      rinoo_tcp_destroy(tcpsock);
      XASSERT(0, NULL);
    }
  if (unlikely(rinoo_socket_timeout_set(&tcpsock->socket, timeout) != 0))
    {
      rinoo_tcp_destroy(tcpsock);
      XASSERT(0, NULL);
    }
  tcpsock->event_fsm = event_fsm;
  return tcpsock;
}

/**
 * Creates a new tcp server and add it to a scheduler.
 *
 * @param sched Pointer to the scheduler to use.
 * @param ip If server mode, IP to bind, if client mode, destination IP.
 * @param port If server mode, port to bind, if client mode, destination port.
 * @param timeout Max number of milliseconds without activity.
 * @param event_fsm Pointer to the function to call for each event.
 *
 * @return Pointer to the new tcp structure, or NULL if an error occurs.
 */
t_rinootcp	*rinoo_tcp_server(t_rinoosched *sched,
				  t_ip ip,
				  u16 port,
				  u32 timeout,
				  void (*event_fsm)(t_rinootcp *tcpsock,
						    t_rinootcp_event event))
{
  int			fd;
  int			enabled;
  t_rinootcp		*tcpsock;
  struct sockaddr_in    addr;

  XDASSERT(sched != NULL, NULL);

  if (unlikely((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1))
    {
      XASSERT(0, NULL);
    }
  tcpsock = rinoo_tcp_init(sched, fd, ip, port, RINOO_TCP_SERVER);
  if (unlikely(tcpsock == NULL))
    {
      return NULL;
    }
  addr.sin_port = htons(tcpsock->port);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = tcpsock->ip;
  enabled = 1;
  if (setsockopt(tcpsock->socket.fd,
		 SOL_SOCKET,
		 SO_REUSEADDR,
		 &enabled,
		 sizeof(enabled)) == -1 ||
      bind(tcpsock->socket.fd,
	   (struct sockaddr *) &addr,
	   sizeof(addr)) == -1 ||
      listen(tcpsock->socket.fd, RINOO_TCP_LISTENSIZE) == -1)
    {
      rinoo_tcp_destroy(tcpsock);
      XASSERT(0, NULL);
    }
  tcpsock->socket.event_fsm = rinoo_tcp_accept;
  if (rinoo_sched_socket(RINOO_SCHED_INSERT,
			 &tcpsock->socket,
			 EVENT_SCHED_IN) == -1)
    {
      rinoo_tcp_destroy(tcpsock);
      XASSERT(0, NULL);
    }
  /*
   * We just keep the timeout value.
   * Scheduler is not aware about any timeout for this socket.
   */
  tcpsock->socket.timeout.ms = timeout;
  tcpsock->event_fsm = event_fsm;
  return tcpsock;
}

/**
 * Destroys a tcp connection. This function removes the corresponding
 * file descriptor from the scheduler, closes the fd, and free's memory used.
 *
 * @param tcpsock Pointer to the tcp connection.
 */
void	rinoo_tcp_destroy(t_rinootcp *tcpsock)
{
  XDASSERTN(tcpsock != NULL);

  if (tcpsock->socket.fd > -1)
    {
      if (rinoo_sched_get(tcpsock->socket.sched, tcpsock->socket.fd) != NULL)
	{
	  rinoo_sched_socket(RINOO_SCHED_REMOVE, &tcpsock->socket, 0);
	}
      close(tcpsock->socket.fd);
    }
  if (tcpsock->socket.rdbuf != NULL)
    {
      buffer_destroy(tcpsock->socket.rdbuf);
    }
  if (tcpsock->socket.wrbuf != NULL)
    {
      buffer_destroy(tcpsock->socket.wrbuf);
    }
  xfree(tcpsock);
}

/**
 * Generates a tcp error. It simply sets the error step (where the error
 * occured), calls the child fsm to raise the error, and finally destroys
 * the connection.
 *
 * @param tcpsock Pointer to the tcp connection.
 * @param errorstep Error step, to specify where the error occured.
 */
static inline void	rinoo_tcp_error(t_rinootcp *tcpsock, t_rinootcp_event errorstep)
{
  XDASSERTN(tcpsock != NULL);

  tcpsock->errorstep = errorstep;
  tcpsock->event_fsm(tcpsock, EVENT_TCP_ERROR);
  rinoo_tcp_destroy(tcpsock);
}

/**
 * In server mode, this function will accept a new connection
 * by calling accept. This function should be called when the
 * socket is ready for reading, right after the connection creation.
 *
 * @param socket Pointer to the socket to use.
 * @param event Scheduler event which is raised.
 */
static void		rinoo_tcp_accept(t_rinoosocket *socket, t_rinoosched_event event)
{
  int			fd;
  t_rinootcp		*tcpsock;
  t_rinootcp		*newsock;
  struct sockaddr_in	addr;

  XDASSERTN(socket != NULL);
  XDASSERTN(socket->data != NULL);

  tcpsock = (t_rinootcp *) socket->data;
  switch (event)
    {
    case EVENT_SCHED_IN:
      fd = accept(tcpsock->socket.fd, (struct sockaddr_in *) &addr,
		   (socklen_t *)(int[]){(sizeof(struct sockaddr))});
      XASSERTN(fd != -1);
      newsock = rinoo_tcp_init(socket->sched,
			       fd,
			       addr.sin_addr.s_addr,
			       ntohs(addr.sin_port),
			       RINOO_TCP_CLIENT);
      if (unlikely(newsock == NULL))
	{
	  close(fd);
	  XASSERTN(0);
	}
      newsock->socket.event_fsm = rinoo_tcp_fsm;
      newsock->socket.parent = socket;
      newsock->event_fsm = tcpsock->event_fsm;
      if (rinoo_sched_socket(RINOO_SCHED_INSERT,
			     &newsock->socket,
			     EVENT_SCHED_IN) == -1)
	{
	  rinoo_tcp_destroy(newsock);
	  XASSERTN(0);
	}
      if (unlikely(rinoo_socket_timeout_set(&newsock->socket,
					    socket->timeout.ms) != 0))
	{
	  rinoo_tcp_destroy(tcpsock);
	  XASSERTN(0);
	}
      tcpsock->event_fsm(newsock, EVENT_TCP_CONNECT);
      break;
    case EVENT_SCHED_OUT:
      rinoo_tcp_destroy(tcpsock);
      XASSERTN(0);
    case EVENT_SCHED_TIMEOUT:
      rinoo_tcp_destroy(tcpsock);
      XASSERTN(0);
      break;
    case EVENT_SCHED_ERROR:
      rinoo_tcp_error(tcpsock, EVENT_TCP_ERROR);
      break;
    case EVENT_SCHED_STOP:
      tcpsock->event_fsm(tcpsock, EVENT_TCP_CLOSE);
      rinoo_tcp_destroy(tcpsock);
      break;
    }
}

/**
 * In client mode, this function will finalize the new tcp connection.
 * If succeeds, it should call the child fsm with the connected state.
 *
 * @param socket Pointer to the socket to use.
 * @param event Scheduler event which is raised.
 */
static void	rinoo_tcp_connect(t_rinoosocket *socket, t_rinoosched_event event)
{
  int           val;
  socklen_t     size;
  t_rinootcp	*tcpsock;

  XDASSERTN(socket != NULL);
  XDASSERTN(socket->data != NULL);

  tcpsock = (t_rinootcp *) socket->data;
  switch (event)
    {
    case EVENT_SCHED_OUT:
    case EVENT_SCHED_ERROR:
      size = sizeof(val);
      if (getsockopt(tcpsock->socket.fd, SOL_SOCKET, SO_ERROR, (void *) &val, &size) < 0)
	{
	  rinoo_tcp_error(tcpsock, EVENT_TCP_CONNECT);
	  return;
	}
      if (val != 0)
	{
	  errno = val;
	  rinoo_tcp_error(tcpsock, EVENT_TCP_CONNECT);
	  return;
	}
      if (rinoo_sched_socket(RINOO_SCHED_MODDEL, socket, EVENT_SCHED_OUT) == -1 ||
	  rinoo_sched_socket(RINOO_SCHED_MODADD, socket, EVENT_SCHED_IN) == -1)
	{
	  rinoo_tcp_error(tcpsock, EVENT_TCP_CONNECT);
	  return;
	}
      tcpsock->socket.event_fsm = rinoo_tcp_fsm;
      tcpsock->event_fsm(tcpsock, EVENT_TCP_CONNECT);
      break;
    case EVENT_SCHED_TIMEOUT:
      tcpsock->event_fsm(tcpsock, EVENT_TCP_TIMEOUT);
      rinoo_tcp_destroy(tcpsock);
      break;
    case EVENT_SCHED_IN:
      rinoo_tcp_destroy(tcpsock);
      XASSERTN(0);
      break;
    case EVENT_SCHED_STOP:
      rinoo_tcp_error(tcpsock, EVENT_TCP_CONNECT);
      break;
    }
}

/**
 * Global tcp fsm. This function is called when the connection has
 * been established to raise every event on the socket.
 *
 * @param socket Pointer to the socket to use.
 * @param event Scheduler event which is raised.
 */
static void	rinoo_tcp_fsm(t_rinoosocket *socket, t_rinoosched_event event)
{
  int		res;
  t_rinootcp	*tcpsock;

  XDASSERTN(socket != NULL);
  XDASSERTN(socket->data != NULL);

  tcpsock = socket->data;
  switch (event)
    {
    case EVENT_SCHED_IN:
      if (buffer_len(socket->rdbuf) == RDBUF_MAXSIZE)
	{
	  rinoo_tcp_error(tcpsock, EVENT_TCP_IN);
	  return;
	}
      res = read(socket->fd,
		 buffer_ptr(socket->rdbuf) + buffer_len(socket->rdbuf),
		 buffer_size(socket->rdbuf) - buffer_len(socket->rdbuf));
      if (unlikely(res < 0))
	{
	  rinoo_tcp_error(tcpsock, EVENT_TCP_IN);
	  return;
	}
      else if (res == 0)
	{
	  tcpsock->event_fsm(tcpsock, EVENT_TCP_CLOSE);
	  rinoo_tcp_destroy(tcpsock);
	  return;
	}
      buffer_setlen(socket->rdbuf, buffer_len(socket->rdbuf) + res);
      if (buffer_isfull(socket->rdbuf))
	buffer_extend(socket->rdbuf, 0);
      tcpsock->event_fsm(tcpsock, EVENT_TCP_IN);
      break;
    case EVENT_SCHED_OUT:
      if (buffer_len(socket->wrbuf) > 0)
	{
	  res = write(socket->fd,
		      buffer_ptr(socket->wrbuf),
		      buffer_len(socket->wrbuf));
	  if (res < 0)
	    {
	      rinoo_tcp_error(tcpsock, EVENT_TCP_OUT);
	      return;
	    }
	  buffer_erase(socket->wrbuf, res);
	}
      if (buffer_len(socket->wrbuf) <= 0 &&
	  rinoo_sched_socket(RINOO_SCHED_MODDEL, socket, EVENT_SCHED_OUT) == -1)
	{
	  rinoo_tcp_error(tcpsock, EVENT_TCP_OUT);
	  return;
	}
      tcpsock->event_fsm(tcpsock, EVENT_TCP_OUT);
      break;
    case EVENT_SCHED_TIMEOUT:
      tcpsock->event_fsm(tcpsock, EVENT_TCP_TIMEOUT);
      rinoo_tcp_destroy(tcpsock);
      break;
    case EVENT_SCHED_ERROR:
      rinoo_tcp_error(tcpsock, EVENT_TCP_ERROR);
      break;
    case EVENT_SCHED_STOP:
      tcpsock->event_fsm(tcpsock, EVENT_TCP_CLOSE);
      rinoo_tcp_destroy(tcpsock);
      break;
    }
}

/**
 * Prints a string into the write buffer of a tcp socket.
 *
 * @param tcpsock Pointer to the socket to write into.
 * @param format printf-like format string
 *
 * @return Number of bytes printed if succeeds, else -1.
 */
int		rinoo_tcp_print(t_rinootcp *tcpsock, const char *format, ...)
{
  int		res;
  va_list	ap;

  XDASSERT(tcpsock != NULL, -1);

  va_start(ap, format);
  res = buffer_vprint(tcpsock->socket.wrbuf, format, ap);
  va_end(ap);
  if (res > -1)
    {
      rinoo_sched_socket(RINOO_SCHED_MODADD, &tcpsock->socket, EVENT_SCHED_OUT);
    }
  return res;
}

/**
 * Adds data into the write buffer of a tcp socket.
 *
 * @param tcpsock Pointer to the socket to write into.
 * @param format printf-like format string
 *
 * @return Number of bytes printed if succeeds, else -1.
 */
int		rinoo_tcp_printdata(t_rinootcp *tcpsock, const char *data, size_t size)
{
  int		res;

  XDASSERT(tcpsock != NULL, -1);

  res = buffer_add(tcpsock->socket.wrbuf, data, size);
  if (res > -1)
    {
      rinoo_sched_socket(RINOO_SCHED_MODADD, &tcpsock->socket, EVENT_SCHED_OUT);
    }
  return res;
}

/**
 * This function says if a tcp socket is set to server mode or not.
 *
 * @param tcpsock Pointer to the socket to check.
 *
 * @return A non-zero value if tcpsock is a server socket, otherwise 0.
 */
int		rinoo_tcp_isserver(t_rinootcp *tcpsock)
{
  return (tcpsock->mode == RINOO_TCP_SERVER);
}

/**
 * For tcp servers only, this returns the main socket of a child socket.
 *
 * @param tcpsock Pointer to the child socket.
 *
 * @return Pointer to the main server socket.
 */
t_rinootcp	*rinoo_tcp_getparent(t_rinootcp *tcpsock)
{
  XDASSERT(tcpsock != NULL, NULL);

  if (tcpsock->socket.parent == NULL)
    {
      XASSERT(tcpsock->mode == RINOO_TCP_SERVER, NULL);
      /* This is a server mode socket */
      return NULL;
    }

  return (t_rinootcp *) tcpsock->socket.parent->data;
}
