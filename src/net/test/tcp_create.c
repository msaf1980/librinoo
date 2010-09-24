/**
 * @file   tcp_create.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2010
 * @date   Wed Dec 30 19:58:33 2009
 *
 * @brief  tcp_create unit test
 *
 *
 */
#include	"rinoo/rinoo.h"

void		event_fsm(t_tcpsocket *unused(tcpsock), t_tcpevent event)
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
  t_sched	*sched;
  t_tcpsocket	*tcpsock;

  sched = sched_create();
  XTEST(sched != NULL);
  tcpsock = tcp_create(sched, 0, 4242, MODE_TCP_CLIENT, event_fsm);
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
  XTEST(tcpsock->mode == MODE_TCP_CLIENT);
  XTEST(tcpsock->event_fsm == event_fsm);
  XTEST(tcpsock->errorstep == 0);
  tcp_destroy(tcpsock);
  sched_destroy(sched);
  XPASS();
}
