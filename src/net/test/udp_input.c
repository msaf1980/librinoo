/**
 * @file   udp_clientinput.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Dec 30 19:58:33 2009
 *
 * @brief  udp client input unit test
 *
 *
 */
#include	"rinoo/rinoo.h"

static int	passed = 1;

void		client_event_fsm(t_udpsocket *udpsock, t_udpevent event)
{
  static int	step = 0;

  switch (event)
    {
    case EVENT_UDP_OUT:
      step++;
      switch (step)
	{
	case 1:
	  udp_print(udpsock, "a");
	  break;
	case 2:
	  udp_print(udpsock, "b");
	  break;
	case 3:
	  udp_print(udpsock, "c");
	  break;
	}
      break;
    case EVENT_UDP_CLOSE:
      printf("Client: close\n");
      break;
    case EVENT_UDP_IN:
    case EVENT_UDP_ERROR:
    case EVENT_UDP_TIMEOUT:
      passed = 0;
      sched_stop(udpsock->socket.sched);
      break;
    }
}

void		server_event_fsm(t_udpsocket *udpsock, t_udpevent event)
{


  switch (event)
    {
    case EVENT_UDP_IN:
      printf("Server: input buffer = \"%.*s\"\n",
	     buffer_len(udpsock->socket.rdbuf),
	     buffer_ptr(udpsock->socket.rdbuf));
      if (buffer_len(udpsock->socket.rdbuf) == 3 &&
	  strncmp(buffer_ptr(udpsock->socket.rdbuf),
		  "abc",
		  buffer_len(udpsock->socket.rdbuf)) == 0)
	{
	  sched_stop(udpsock->socket.sched);
	}
      break;
    case EVENT_UDP_CLOSE:
      printf("Server: close\n");
      break;
    case EVENT_UDP_OUT:
    case EVENT_UDP_ERROR:
    case EVENT_UDP_TIMEOUT:
      passed = 0;
      sched_stop(udpsock->socket.sched);
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
  t_udpsocket	*sudpsock;
  t_udpsocket	*cudpsock;

  sched = sched_create();
  XTEST(sched != NULL);
  sudpsock = udp_create(sched, 0, 4242, MODE_UDP_SERVER, 0, server_event_fsm);
  XTEST(sudpsock != NULL);
  XTEST(sudpsock->socket.fd != 0);
  XTEST(sudpsock->socket.sched == sched);
  XTEST(sudpsock->socket.poll_mode != 0);
  XTEST(sudpsock->socket.event_fsm != NULL);
  XTEST(sudpsock->socket.parent == NULL);
  XTEST(sudpsock->socket.rdbuf != NULL);
  XTEST(sudpsock->socket.wrbuf != NULL);
  XTEST(sudpsock->ip == 0);
  XTEST(sudpsock->port == 4242);
  XTEST(sudpsock->mode == MODE_UDP_SERVER);
  XTEST(sudpsock->event_fsm == server_event_fsm);
  XTEST(sudpsock->errorstep == 0);
  cudpsock = udp_create(sched, 0, 4242, MODE_UDP_CLIENT, 0, client_event_fsm);
  XTEST(cudpsock != NULL);
  XTEST(cudpsock->socket.fd != 0);
  XTEST(cudpsock->socket.sched == sched);
  XTEST(cudpsock->socket.poll_mode != 0);
  XTEST(cudpsock->socket.event_fsm != NULL);
  XTEST(cudpsock->socket.parent == NULL);
  XTEST(cudpsock->socket.rdbuf != NULL);
  XTEST(cudpsock->socket.wrbuf != NULL);
  XTEST(cudpsock->ip == 0);
  XTEST(cudpsock->port == 4242);
  XTEST(cudpsock->mode == MODE_UDP_CLIENT);
  XTEST(cudpsock->event_fsm == client_event_fsm);
  XTEST(cudpsock->errorstep == 0);
  sched_loop(sched);
  sched_destroy(sched);
  if (passed != 1)
    XFAIL();
  XPASS();
}
