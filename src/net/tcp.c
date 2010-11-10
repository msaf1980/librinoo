/**
 * @file   tcp.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2010
 * @date   Mon Dec 28 22:29:31 2009
 *
 * @brief  Manages a TCP connection based on asynchronous event system as epoll.
 *
 *
 */

#include	"rinoo/rinoo.h"

static inline void	tcp_error(t_tcpsocket *tcpsock, t_tcpevent errorstep);
static void		tcp_accept(t_socket *socket, t_schedevent event);
static void		tcp_connect(t_socket *socket, t_schedevent event);
static void		tcp_fsm(t_socket *socket, t_schedevent event);

/**
 * Creates a new tcp connection and add it to a scheduler.
 *
 * @param sched Pointer to the scheduler to use.
 * @param ip If server mode, IP to bind, if client mode, destination IP.
 * @param port If server mode, port to bind, if client mode, destination port.
 * @param mode Mode to use for this connection (server/client).
 * @param timeout Max number of milliseconds without activity.
 * @param event_fsm Pointer to the function to call for each event.
 *
 * @return Pointer to the new tcp structure, or NULL if an error occurs.
 */
t_tcpsocket	*tcp_create(t_sched *sched,
			    t_ip ip,
			    u32 port,
			    t_tcpmode mode,
			    u32 timeout,
			    void (*event_fsm)(t_tcpsocket *tcpsock,
					      t_tcpevent event))
{
  int			enabled;
  t_tcpsocket		*tcpsock;
  struct sockaddr_in    addr;

  XDASSERT(sched != NULL, NULL);

  tcpsock = xcalloc(1, sizeof(*tcpsock));
  XASSERT(tcpsock != NULL, NULL);
  tcpsock->socket.fd = -1;
  tcpsock->socket.sched = sched;
  tcpsock->socket.rdbuf = buffer_create(RDBUF_INITSIZE, RDBUF_MAXSIZE);
  if (tcpsock->socket.rdbuf == NULL)
    {
      tcp_destroy(tcpsock);
      XASSERT(0, NULL);
    }
  tcpsock->socket.wrbuf = buffer_create(WRBUF_INITSIZE, WRBUF_MAXSIZE);
  if (tcpsock->socket.wrbuf == NULL)
    {
      tcp_destroy(tcpsock);
      XASSERT(0, NULL);
    }
  tcpsock->socket.data = tcpsock;
  tcpsock->ip = ip;
  tcpsock->port = port;
  tcpsock->mode = mode;
  tcpsock->event_fsm = event_fsm;
  if ((tcpsock->socket.fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
      tcp_destroy(tcpsock);
      XASSERT(0, NULL);
    }
  enabled = 1;
  if (ioctl(tcpsock->socket.fd, FIONBIO, &enabled) == -1)
    {
      tcp_destroy(tcpsock);
      XASSERT(0, NULL);
    }
  addr.sin_port = htons(tcpsock->port);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = tcpsock->ip;
  if (mode == MODE_TCP_SERVER)
    {
      if (setsockopt(tcpsock->socket.fd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)) == -1 ||
	  bind(tcpsock->socket.fd, (struct sockaddr *) &addr, sizeof(addr)) == -1 ||
	  listen(tcpsock->socket.fd, TCP_LISTEN_SIZE) == -1)
	{
	  tcp_destroy(tcpsock);
	  XASSERT(0, NULL);
	}
      tcpsock->socket.event_fsm = tcp_accept;
    }
  else
    {
      if (connect(tcpsock->socket.fd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
	{
	  switch (errno)
	    {
	    case EINPROGRESS:
	    case EALREADY:
	      break;
	    default:
	      tcp_destroy(tcpsock);
	      XASSERT(0, NULL);
	    }
	}
      tcpsock->socket.event_fsm = tcp_connect;
    }
  if (sched_insert(&tcpsock->socket,
		   (mode == MODE_TCP_SERVER ?
		    EVENT_SCHED_IN : EVENT_SCHED_OUT),
		   (mode == MODE_TCP_SERVER ? 0 : timeout)) == -1)
    {
      tcp_destroy(tcpsock);
      XASSERT(0, NULL);
    }
  /* This should be used only in MODE_TCP_SERVER */
  tcpsock->child_timeout = timeout;
  return (tcpsock);
}

/**
 * Destroys a tcp connection. This function removes the corresponding
 * file descriptor from the scheduler, closes the fd, and free's memory used.
 *
 * @param tcpsock Pointer to the tcp connection.
 */
void	tcp_destroy(t_tcpsocket *tcpsock)
{
  XDASSERTN(tcpsock != NULL);

  if (tcpsock->socket.fd > -1)
    {
      if (sched_getsocket(tcpsock->socket.sched, tcpsock->socket.fd) != NULL)
	sched_remove(&tcpsock->socket);
      close(tcpsock->socket.fd);
    }
  if (tcpsock->socket.rdbuf != NULL)
    buffer_destroy(tcpsock->socket.rdbuf);
  if (tcpsock->socket.wrbuf != NULL)
    buffer_destroy(tcpsock->socket.wrbuf);
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
static inline void	tcp_error(t_tcpsocket *tcpsock, t_tcpevent errorstep)
{
  XDASSERTN(tcpsock != NULL);

  tcpsock->errorstep = errorstep;
  tcpsock->event_fsm(tcpsock, EVENT_TCP_ERROR);
  tcp_destroy(tcpsock);
}

/**
 * In server mode, this function will accept a new connection
 * by calling accept. This function should be called when the
 * socket is ready for reading, right after the connection creation.
 *
 * @param socket Pointer to the socket to use.
 * @param event Scheduler event which is raised.
 */
static void		tcp_accept(t_socket *socket, t_schedevent event)
{
  int			fd;
  int			enabled;
  t_tcpsocket		*tcpsock;
  t_tcpsocket		*newsock;
  struct sockaddr_in	addr;

  XDASSERTN(socket != NULL);
  XDASSERTN(socket->data != NULL);

  tcpsock = (t_tcpsocket *) socket->data;
  switch (event)
    {
    case EVENT_SCHED_IN:
      fd = accept(tcpsock->socket.fd, (struct sockaddr_in *) &addr,
		   (socklen_t *)(int[]){(sizeof(struct sockaddr))});
      XASSERTN(fd != -1);
      enabled = 1;
      if (ioctl(fd, FIONBIO, &enabled) == -1)
	{
	  close(fd);
	  XASSERTN(0);
	}
      newsock = xcalloc(1, sizeof(*newsock));
      if (newsock == NULL)
	{
	  close(fd);
	  XASSERTN(0);
	}
      newsock->socket.fd = fd;
      newsock->socket.rdbuf = buffer_create(RDBUF_INITSIZE, RDBUF_MAXSIZE);
      if (newsock->socket.rdbuf == NULL)
	{
	  tcp_destroy(newsock);
	  XASSERTN(0);
	}
      newsock->socket.wrbuf = buffer_create(WRBUF_INITSIZE, WRBUF_MAXSIZE);
      if (newsock->socket.wrbuf == NULL)
	{
	  tcp_destroy(newsock);
	  XASSERTN(0);
	}
      newsock->socket.sched = tcpsock->socket.sched;
      newsock->socket.event_fsm = tcp_fsm;
      newsock->socket.data = newsock;
      newsock->socket.parent = socket;
      newsock->ip = addr.sin_addr.s_addr;
      newsock->port = ntohs(addr.sin_port);
      newsock->mode = MODE_TCP_CLIENT;
      newsock->event_fsm = tcpsock->event_fsm;
      if (sched_insert(&newsock->socket,
		       EVENT_SCHED_IN,
		       tcpsock->child_timeout) == -1)
	{
	  tcp_destroy(newsock);
	  XASSERTN(0);
	}
      tcpsock->event_fsm(newsock, EVENT_TCP_CONNECT);
      break;
    case EVENT_SCHED_OUT:
      tcp_destroy(tcpsock);
      XASSERTN(0);
    case EVENT_SCHED_TIMEOUT:
      tcp_destroy(tcpsock);
      XASSERTN(0);
      break;
    case EVENT_SCHED_ERROR:
      tcp_error(tcpsock, EVENT_TCP_ERROR);
      break;
    case EVENT_SCHED_STOP:
      tcpsock->event_fsm(tcpsock, EVENT_TCP_CLOSE);
      tcp_destroy(tcpsock);
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
static void	tcp_connect(t_socket *socket, t_schedevent event)
{
  int           val;
  socklen_t     size;
  t_tcpsocket	*tcpsock;

  XDASSERTN(socket != NULL);
  XDASSERTN(socket->data != NULL);

  tcpsock = (t_tcpsocket *)socket->data;
  switch (event)
    {
    case EVENT_SCHED_OUT:
    case EVENT_SCHED_ERROR:
      size = sizeof(val);
      if (getsockopt(tcpsock->socket.fd, SOL_SOCKET, SO_ERROR, (void *) &val, &size) < 0)
	{
	  tcp_error(tcpsock, EVENT_TCP_CONNECT);
	  return;
	}
      if (val != 0)
	{
	  errno = val;
	  tcp_error(tcpsock, EVENT_TCP_CONNECT);
	  return;
	}
      if (sched_delmode(socket, EVENT_SCHED_OUT) == -1 ||
	  sched_addmode(socket, EVENT_SCHED_IN) == -1)
	{
	  tcp_error(tcpsock, EVENT_TCP_CONNECT);
	  return;
	}
      tcpsock->socket.event_fsm = tcp_fsm;
      tcpsock->event_fsm(tcpsock, EVENT_TCP_CONNECT);
      break;
    case EVENT_SCHED_TIMEOUT:
      tcpsock->event_fsm(tcpsock, EVENT_TCP_TIMEOUT);
      tcp_destroy(tcpsock);
      break;
    case EVENT_SCHED_IN:
      tcp_destroy(tcpsock);
      XASSERTN(0);
      break;
    case EVENT_SCHED_STOP:
      tcp_error(tcpsock, EVENT_TCP_CONNECT);
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
static void	tcp_fsm(t_socket *socket, t_schedevent event)
{
  int		res;
  t_tcpsocket	*tcpsock;

  XDASSERTN(socket != NULL);
  XDASSERTN(socket->data != NULL);

  tcpsock = socket->data;
  switch (event)
    {
    case EVENT_SCHED_IN:
      if (buffer_len(socket->rdbuf) == RDBUF_MAXSIZE)
	{
	  tcp_error(tcpsock, EVENT_TCP_IN);
	  return;
	}
      res = read(socket->fd,
		 buffer_ptr(socket->rdbuf) + buffer_len(socket->rdbuf),
		 buffer_size(socket->rdbuf) - buffer_len(socket->rdbuf));
      if (unlikely(res < 0))
	{
	  tcp_error(tcpsock, EVENT_TCP_IN);
	  return;
	}
      else if (res == 0)
	{
	  tcpsock->event_fsm(tcpsock, EVENT_TCP_CLOSE);
	  tcp_destroy(tcpsock);
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
	      tcp_error(tcpsock, EVENT_TCP_OUT);
	      return;
	    }
	  buffer_erase(socket->wrbuf, res);
	}
      if (buffer_len(socket->wrbuf) <= 0 &&
	  sched_delmode(socket, EVENT_SCHED_OUT) == -1)
	{
	  tcp_error(tcpsock, EVENT_TCP_OUT);
	  return;
	}
      tcpsock->event_fsm(tcpsock, EVENT_TCP_OUT);
      break;
    case EVENT_SCHED_TIMEOUT:
      tcpsock->event_fsm(tcpsock, EVENT_TCP_TIMEOUT);
      tcp_destroy(tcpsock);
      break;
    case EVENT_SCHED_ERROR:
      tcp_error(tcpsock, EVENT_TCP_ERROR);
      break;
    case EVENT_SCHED_STOP:
      tcpsock->event_fsm(tcpsock, EVENT_TCP_CLOSE);
      tcp_destroy(tcpsock);
      break;
    }
}

/**
 * Prints a string into the write buffer of a tcp socket.
 *
 * @param socket Pointer to the socket to write into.
 * @param format printf-like format string
 *
 * @return Number of bytes printed if succeeds, else -1.
 */
int		tcp_print(t_tcpsocket *socket, const char *format, ...)
{
  int		res;
  va_list	ap;

  XDASSERT(socket != NULL, -1);

  va_start(ap, format);
  res = buffer_vprint(socket->socket.wrbuf, format, ap);
  va_end(ap);
  if (res > -1)
    sched_addmode(&socket->socket, EVENT_SCHED_OUT);
  return (res);
}

/**
 * Adds data into the write buffer of a tcp socket.
 *
 * @param tcpsock Pointer to the socket to write into.
 * @param format printf-like format string
 *
 * @return Number of bytes printed if succeeds, else -1.
 */
int		tcp_printdata(t_tcpsocket *tcpsock, const char *data, size_t size)
{
  int		res;

  XDASSERT(tcpsock != NULL, -1);

  res = buffer_add(tcpsock->socket.wrbuf, data, size);
  if (res > -1)
    sched_addmode(&tcpsock->socket, EVENT_SCHED_OUT);
  return (res);
}

/**
 * This function says if a tcp socket is set to server mode or not.
 *
 * @param tcpsock Pointer to the socket to check.
 *
 * @return A non-zero value if tcpsock is a server socket, otherwise 0.
 */
int		tcp_isserver(t_tcpsocket *tcpsock)
{
  return (tcpsock->mode == MODE_TCP_SERVER);
}

/**
 * For tcp servers only, this returns the main socket of a child socket.
 *
 * @param tcpsock Pointer to the child socket.
 *
 * @return Pointer to the main server socket.
 */
t_tcpsocket	*tcp_getparent(t_tcpsocket *tcpsock)
{
  XDASSERT(tcpsock != NULL, NULL);

  if (tcpsock->socket.parent == NULL)
    {
      XASSERT(tcpsock->mode == MODE_TCP_SERVER, NULL);
      /* This is a server mode socket */
      return (NULL);
    }

  return ((t_tcpsocket *) tcpsock->socket.parent->data);
}
