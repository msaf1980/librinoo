/**
 * @file   tcp_create.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Dec 30 19:58:33 2009
 *
 * @brief  tcp_create unit test
 *
 *
 */
#include	"rinoo/rinoo.h"

void		event_fsm(t_rinootcp *unused(tcpsock), t_rinootcp_event event)
{
  switch (event)
    {
    case EVENT_TCP_CONNECT:
      printf("CONNECTED!\n");
      break;
    case EVENT_TCP_IN:
    case EVENT_TCP_OUT:
      break;
    case EVENT_TCP_ERROR:
      printf("Error: %s\n", strerror(errno));
      break;
    case EVENT_TCP_CLOSE:
    case EVENT_TCP_TIMEOUT:
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
  t_rinootcp	*tcpsock;

  sched = rinoo_sched();
  XTEST(sched != NULL);
  tcpsock = rinoo_tcp_client(sched, 0, 4242, 0, event_fsm);
  XTEST(tcpsock != NULL);
  XTEST(tcpsock->socket.fd != 0);
  XTEST(tcpsock->socket.sched == sched);
  XTEST(tcpsock->socket.poll_mode != 0);
  XTEST(tcpsock->socket.event_fsm != NULL);
  XTEST(tcpsock->socket.parent == NULL);
  XTEST(tcpsock->socket.rdbuf != NULL);
  XTEST(tcpsock->socket.wrbuf != NULL);
  XTEST(tcpsock->ip == 0);
  XTEST(tcpsock->port == 4242);
  XTEST(tcpsock->mode == RINOO_TCP_CLIENT);
  XTEST(tcpsock->event_fsm == event_fsm);
  XTEST(tcpsock->errorstep == 0);
  rinoo_tcp_destroy(tcpsock);
  tcpsock = rinoo_tcp_server(sched, 0, 4242, 0, event_fsm);
  XTEST(tcpsock != NULL);
  XTEST(tcpsock->socket.fd != 0);
  XTEST(tcpsock->socket.sched == sched);
  XTEST(tcpsock->socket.poll_mode != 0);
  XTEST(tcpsock->socket.event_fsm != NULL);
  XTEST(tcpsock->socket.parent == NULL);
  XTEST(tcpsock->socket.rdbuf != NULL);
  XTEST(tcpsock->socket.wrbuf != NULL);
  XTEST(tcpsock->ip == 0);
  XTEST(tcpsock->port == 4242);
  XTEST(tcpsock->mode == RINOO_TCP_SERVER);
  XTEST(tcpsock->event_fsm == event_fsm);
  XTEST(tcpsock->errorstep == 0);
  rinoo_tcp_destroy(tcpsock);
  rinoo_sched_destroy(sched);
  XPASS();
}
