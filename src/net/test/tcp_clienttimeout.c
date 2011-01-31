/**
 * @file   tcp_clienttimeout.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Dec 30 19:58:33 2009
 *
 * @brief  tcp client connection unit test
 *
 *
 */
#include	"rinoo/rinoo.h"

static int	passed = 1;

t_rinoojob_state	client_cb(t_rinoojob *job)
{
  static int	counter = 0;
  t_tcpsocket	*tcpsock;

  tcpsock = job->args;
  tcp_print(tcpsock, "ping");
  counter++;
  if (counter < 5)
    {
      return JOB_REDO;
    }
  rinoo_log("Engaging client timeout...");
  return JOB_DONE;
}

void		client_event_fsm(t_tcpsocket *tcpsock, t_tcpevent event)
{
  switch (event)
    {
    case EVENT_TCP_CONNECT:
      rinoo_log("Client: connected!");
      jobqueue_addms(tcpsock->socket.sched, client_cb, tcpsock, 100);
      break;
    case EVENT_TCP_IN:
      rinoo_log("Client: received \"%.*s\"",
		buffer_len(tcpsock->socket.rdbuf),
		tcpsock->socket.rdbuf->buf);
      buffer_erase(tcpsock->socket.rdbuf, buffer_len(tcpsock->socket.rdbuf));
      break;
    case EVENT_TCP_OUT:
      break;
    case EVENT_TCP_ERROR:
    case EVENT_TCP_CLOSE:
      passed = 0;
      rinoo_sched_stop(tcpsock->socket.sched);
      break;
    case EVENT_TCP_TIMEOUT:
      rinoo_log("Client timeout!");
      break;
    }
}

void		server_event_fsm(t_tcpsocket *tcpsock, t_tcpevent event)
{
  switch (event)
    {
    case EVENT_TCP_CONNECT:
      rinoo_log("Server: new client connected!");
      break;
    case EVENT_TCP_CLOSE:
      if (tcpsock->mode == MODE_TCP_CLIENT)
	{
	  rinoo_log("Server: client connection closed.");
	  rinoo_sched_stop(tcpsock->socket.sched);
	}
      else
	rinoo_log("Server: shuting down...");
      break;
    case EVENT_TCP_IN:
      rinoo_log("Server: received \"%.*s\"",
		buffer_len(tcpsock->socket.rdbuf),
		tcpsock->socket.rdbuf->buf);
      buffer_erase(tcpsock->socket.rdbuf, buffer_len(tcpsock->socket.rdbuf));
      tcp_print(tcpsock, "pong");
      break;
    case EVENT_TCP_OUT:
      break;
    case EVENT_TCP_ERROR:
    case EVENT_TCP_TIMEOUT:
      rinoo_log("Server: error or timeout!");
      passed = 0;
      rinoo_sched_stop(tcpsock->socket.sched);
      break;
    }
}

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int		main()
{
  t_rinoosched	*sched;
  t_tcpsocket	*stcpsock;
  t_tcpsocket	*ctcpsock;
  struct timeval	tv1;
  struct timeval	tv2;

  sched = rinoo_sched();
  XTEST(sched != NULL);
  stcpsock = tcp_create(sched, 0, 4242, MODE_TCP_SERVER, 0, server_event_fsm);
  XTEST(stcpsock != NULL);
  XTEST(stcpsock->socket.fd != 0);
  XTEST(stcpsock->socket.sched == sched);
  XTEST(stcpsock->socket.poll_mode != 0);
  XTEST(stcpsock->socket.event_fsm != NULL);
  XTEST(stcpsock->socket.parent == NULL);
  XTEST(stcpsock->socket.rdbuf != NULL);
  XTEST(stcpsock->socket.wrbuf != NULL);
  XTEST(stcpsock->ip == 0);
  XTEST(stcpsock->port == 4242);
  XTEST(stcpsock->mode == MODE_TCP_SERVER);
  XTEST(stcpsock->event_fsm == server_event_fsm);
  XTEST(stcpsock->errorstep == 0);
  ctcpsock = tcp_create(sched, 0, 4242, MODE_TCP_CLIENT, 500, client_event_fsm);
  XTEST(ctcpsock != NULL);
  XTEST(ctcpsock->socket.fd != 0);
  XTEST(ctcpsock->socket.sched == sched);
  XTEST(ctcpsock->socket.poll_mode != 0);
  XTEST(ctcpsock->socket.event_fsm != NULL);
  XTEST(ctcpsock->socket.parent == NULL);
  XTEST(ctcpsock->socket.rdbuf != NULL);
  XTEST(ctcpsock->socket.wrbuf != NULL);
  XTEST(ctcpsock->ip == 0);
  XTEST(ctcpsock->port == 4242);
  XTEST(ctcpsock->mode == MODE_TCP_CLIENT);
  XTEST(ctcpsock->event_fsm == client_event_fsm);
  XTEST(ctcpsock->errorstep == 0);
  XTEST(gettimeofday(&tv1, NULL) == 0);
  rinoo_sched_loop(sched);
  XTEST(gettimeofday(&tv2, NULL) == 0);
  XTEST((tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec - tv1.tv_usec) / 1000 >= 1000);
  XTEST((tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec - tv1.tv_usec) / 1000 < 1200);
  rinoo_sched_destroy(sched);
  if (passed != 1)
    XFAIL();
  XPASS();
}
