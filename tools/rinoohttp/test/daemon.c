/**
 * @file   daemon.c
 * @author Reginald <reginald.l@gmail.com> - Copyright 2010
 * @date   Wed Nov 10 14:58:24 2010
 *
 * @brief  HTTP server daemon for test purpose.
 *
 *
 */
#include	"rinoo/rinoohttp.h"

enum		e_daemonmsg
  {
    DAEMON_400 = 0,
    DAEMON_404,
    DAEMON_200
  }		t_daemonmsg;

char		*daemon_msgs[] =
  {
    "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
    "<html>\n"
    "  <head>\n"
    "    <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n"
    "    <title>400 Bad Request</title>\n"
    "  </head>\n"
    "  <body>\n"
    "    <h1>Bad Request</h1>\n"
    "    <p>Your browser sent a request that this server could not understand.</p><br />\n"
    "    <hr /><address>rinoohttp/0.1</address>\n"
    "  </body>\n"
    "</html>\n",
    "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
    "<html>\n"
    "  <head>\n"
    "    <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n"
    "    <title>404 Not Found</title>\n"
    "  </head>\n"
    "  <body>\n"
    "    <h1>Not found</h1>\n"
    "    <p>Your browser sent a request that this server could not understand.</p><br />\n"
    "    <hr /><address>rinoohttp/0.1</address>\n"
    "  </body>\n"
    "</html>\n",
    "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
    "<html>\n"
    "  <head>\n"
    "    <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n"
    "    <title>Welcome to RinOo!</title>\n"
    "  </head>\n"
    "  <body>\n"
    "    <h1>rinoo 0.1</h1><hr/>\n"
    "    <p>Wow! what a webserver! :)</p><br/><br/>\n"
    "    <address>rinoohttp/0.1</address>\n"
    "  </body>\n"
    "</html>\n"
  };

t_sched		*sched;

void		sig_handler(int unused(signal))
{
  sched_stop(sched);
}

void		myhttp_fsm(t_httpsocket *httpsock, t_httpevent event)
{
  switch (event)
    {
    case EVENT_HTTP_CONNECT:
      rinoo_log("httpd - connect");
      break;
    case EVENT_HTTP_REQUEST:
      rinoo_log("httpd - request uri: -%.*s- version: %d",
		buffer_len(httpsock->request.uri),
		buffer_ptr(httpsock->request.uri),
		httpsock->request.version);
      break;
    case EVENT_HTTP_REQBODY:
      rinoo_log("httpd - requestbody - received=%d", httpsock->request.received);
      break;
    case EVENT_HTTP_RESPONSE:
      rinoo_log("httpd - response");
      httpsock->response.code = 200;
      httpsock->response.msg.buf = "OK";
      httpsock->response.msg.len = 2;
      httpsock->response.body.buf = daemon_msgs[DAEMON_200];
      httpsock->response.body.len = strlen(daemon_msgs[DAEMON_200]);
      break;
    case EVENT_HTTP_RESPBODY:
      if (httpsock->data == NULL)
	{
	  rinoo_log("httpd - responsebody");
	  tcp_printdata(httpsock->tcpsock,
			daemon_msgs[DAEMON_200],
			strlen(daemon_msgs[DAEMON_200]));
	  httpsock->data = INT_TO_PTR(1);
	}
      break;
    case EVENT_HTTP_ERROR:
      rinoo_log("httpd - error");
      break;
    case EVENT_HTTP_CLOSE:
      rinoo_log("httpd - close");
      break;
    case EVENT_HTTP_TIMEOUT:
      rinoo_log("httpd - timeout");
      break;
    }
}

/**
 * Main function for this test
 *
 *
 * @return Should be 0.
 */
int		main()
{
  sched = sched_create();
  rinoo_enable_backtrace((struct s_sched *) sched);
  signal(SIGINT, sig_handler);
  httpserver_create(sched, 0, 4242, 15000, myhttp_fsm);
  sched_loop(sched);
  sched_destroy(sched);
  return (0);
}
