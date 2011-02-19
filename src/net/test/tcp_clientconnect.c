/**
 * @file   tcp_clientconnect.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Dec 30 19:58:33 2009
 *
 * @brief  tcp client connection unit test
 *
 *
 */
#include	"rinoo/rinoo.h"

void		client_event_fsm(t_rinootcp *tcpsock, t_rinootcp_event event)
{
  switch (event)
    {
    case EVENT_TCP_CONNECT:
      printf("Client: connected!\n");
      rinoo_tcp_destroy(tcpsock);
      break;
    case EVENT_TCP_IN:
    case EVENT_TCP_OUT:
    case EVENT_TCP_ERROR:
    case EVENT_TCP_CLOSE:
    case EVENT_TCP_TIMEOUT:
      XFAIL();
      break;
    }
}

void		server_event_fsm(t_rinootcp *tcpsock, t_rinootcp_event event)
{
  switch (event)
    {
    case EVENT_TCP_CONNECT:
      printf("Server: new client connected!\n");
      break;
    case EVENT_TCP_CLOSE:
      if (tcpsock->mode == RINOO_TCP_CLIENT)
	{
	  printf("Server: client connection closed.\n");
	  rinoo_sched_stop(tcpsock->socket.sched);
	}
      else
	printf("Server: shuting down...\n");
      break;
    case EVENT_TCP_IN:
    case EVENT_TCP_OUT:
    case EVENT_TCP_ERROR:
    case EVENT_TCP_TIMEOUT:
      XFAIL();
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
  t_rinootcp	*stcpsock;
  t_rinootcp	*ctcpsock;

  sched = rinoo_sched();
  XTEST(sched != NULL);
  stcpsock = rinoo_tcp_server(sched, 0, 4242, 0, server_event_fsm);
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
  XTEST(stcpsock->mode == RINOO_TCP_SERVER);
  XTEST(stcpsock->event_fsm == server_event_fsm);
  XTEST(stcpsock->errorstep == 0);
  ctcpsock = rinoo_tcp_client(sched, 0, 4242, 0, client_event_fsm);
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
  XTEST(ctcpsock->mode == RINOO_TCP_CLIENT);
  XTEST(ctcpsock->event_fsm == client_event_fsm);
  XTEST(ctcpsock->errorstep == 0);
  rinoo_sched_loop(sched);
  rinoo_sched_destroy(sched);
  XPASS();
}
