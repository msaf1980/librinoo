/**
 * @file   server.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2010
 * @date   Sun Dec 27 23:47:50 2009
 *
 * @brief  Test file for libroo, server side.
 *
 *
 */

#include	"rinoo/rinoo.h"

t_sched		*sched;

typedef struct	s_httreq
{
  char		*uri;
  int		fd;
}		t_httpreq;

void		sig_handler(int unused(signal))
{
  sched_stop(sched);
}

static void	httpserver_badrequest(t_tcpsocket *socket)
{
  char		*msg =
    "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
    "<html>\n"
    "  <head>\n"
    "    <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n"
    "    <title>400 Bad Request</title>\n"
    "  </head>\n"
    "  <body>\n"
    "    <h1>Bad Request</h1>\n"
    "    <p>Your browser sent a request that this server could not understand.</p><br />\n"
    "    <hr /><address>rinoohttpd/0.1</address>\n"
    "  </body>\n"
    "</html>\n";

  tcp_print(socket, "HTTP/1.0 400 Bad Request\r\n");
  tcp_print(socket, "Content-length: %d\r\n\r\n", strlen(msg));
  tcp_print(socket, msg);
}

static void	httpserver_ok(t_tcpsocket *socket, t_buffer *unused(uri))
{
  char		*msg =
    "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
    "<html>\n"
    "  <head>\n"
    "    <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n"
    "    <title>RiNOO</title>\n"
    "  </head>\n"
    "  <body>\n"
    "    <h1>It works!</h1>\n"
    "    <p>This page was sent by RiNOO httpd based on librinoo.</p><br />\n"
    "    <hr /><address>rinoohttpd/0.1</address>\n"
    "  </body>\n"
    "</html>\n";

  tcp_print(socket, "HTTP/1.1 200 OK\r\n");
  tcp_print(socket, "Content-Type: text/html\r\n");
  tcp_print(socket, "Content-Length: %d\r\n\r\n", strlen(msg));

  tcp_print(socket, "%s", msg);
}

static int	httpserver_read(t_buffer *buffer, t_buffer *uri)
{
  char		*p = buffer->buf;
  char		*pe = buffer->buf + buffer->len;
  char		*eof = NULL;
  int		cs = 0;

  %%{
    machine httpreader;
    write data;

    action starturi { uri->buf = fpc; }
    action enduri   { uri->len = fpc - uri->buf;}
    action okac	{ return (1); }
    action parseerror { return (-1); }

    crlf = ('\r\n' | '\n');
    method = ('GET' | 'POST' | 'HEAD');
    uri = (ascii* -- crlf);
    http = 'HTTP/1.' [0-1];
    header = (alnum | punct)+ ': ' (ascii* -- crlf) crlf;
    main := (method ' ' uri >starturi %enduri ' ' http crlf header* crlf @okac) $err(parseerror);

    write init;
    write exec;
  }%%
  (void) httpreader_en_main;
  (void) httpreader_error;
  (void) httpreader_first_final;
  return (0);
}

void		event_fsm(t_tcpsocket *tcpsock, t_tcpevent event)
{
  t_buffer	uri;

  switch (event)
    {
    case EVENT_TCP_CONNECT:
      break;
    case EVENT_TCP_IN:
      switch (httpserver_read(tcpsock->socket.rdbuf, &uri))
	{
	case 1:
	  printf("httpd - URI: %.*s\n", uri.len, uri.buf);
	  httpserver_ok(tcpsock, &uri);
	  break;
	case 0:
	  return;
	case -1:
	  httpserver_badrequest(tcpsock);
	  break;
	}
      buffer_erase(tcpsock->socket.rdbuf, buffer_len(tcpsock->socket.rdbuf));
      break;
    case EVENT_TCP_OUT:
      break;
    case EVENT_TCP_ERROR:
      break;
    case EVENT_TCP_CLOSE:
      break;
    case EVENT_TCP_TIMEOUT:
      break;
    }
}

/**
 * Main function for this sample
 *
 *
 * @return Should be 0.
 */
int		main(int argc, char **argv)
{
  t_tcpsocket	*tcpsock;

  if (argc != 2)
    {
      printf("Usage %s <port>\n", argv[0]);
      return (-1);
    }
  sched = sched_create();
  signal(SIGINT, sig_handler);
  tcpsock = tcp_create(sched, 0, atoi(argv[1]), MODE_TCP_SERVER, event_fsm);
  if (tcpsock == NULL)
    {
      return (-1);
    }
  sched_loop(sched);
  sched_destroy(sched);
  return (0);
}
