/**
 * @file   tcp_serverinput.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Dec 30 19:58:33 2009
 *
 * @brief  tcp server input unit test
 *
 *
 */
#include	"rinoo/rinoo.h"

static int	passed = 1;

void		client_event_fsm(t_tcpsocket *tcpsock, t_tcpevent event)
{
  static int	step = 0;

  switch (event)
    {
    case EVENT_TCP_CONNECT:
      printf("Client: connected!\n");
      tcp_print(tcpsock, "a");
      break;
    case EVENT_TCP_OUT:
      step++;
      switch (step)
	{
	case 1:
	  tcp_print(tcpsock, "b");
	  break;
	case 2:
	  tcp_print(tcpsock, "c");
	  break;
	case 3:
	  tcp_print(tcpsock, "d");
	  break;
	}
      break;
    case EVENT_TCP_CLOSE:
      printf("Client: connection closed.\n");
      rinoo_sched_stop(tcpsock->socket.sched);
      break;
    case EVENT_TCP_IN:
    case EVENT_TCP_ERROR:
    case EVENT_TCP_TIMEOUT:
      passed = 0;
      rinoo_sched_stop(tcpsock->socket.sched);
      break;
    }
}

void		server_event_fsm(t_tcpsocket *tcpsock, t_tcpevent event)
{
  switch (event)
    {
    case EVENT_TCP_CONNECT:
      printf("Server: new client connected!\n");
      break;
    case EVENT_TCP_IN:
      printf("Server: input buffer = \"%.*s\"\n",
	     buffer_len(tcpsock->socket.rdbuf),
	     buffer_ptr(tcpsock->socket.rdbuf));
      if (buffer_len(tcpsock->socket.rdbuf) == 4 &&
	  strncmp(buffer_ptr(tcpsock->socket.rdbuf),
		  "abcd",
		  buffer_len(tcpsock->socket.rdbuf)) == 0)
	{
	  tcp_destroy(tcpsock);
	}
      break;
    case EVENT_TCP_CLOSE:
      printf("Server: shuting down...\n");
      break;
    case EVENT_TCP_OUT:
    case EVENT_TCP_ERROR:
    case EVENT_TCP_TIMEOUT:
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
  ctcpsock = tcp_create(sched, 0, 4242, MODE_TCP_CLIENT, 0, client_event_fsm);
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
  rinoo_sched_loop(sched);
  rinoo_sched_destroy(sched);
  if (passed != 1)
    XFAIL();
  XPASS();
}
