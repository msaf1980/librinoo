/**
 * @file   smtpclient.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Nov 24 21:50:44 2010
 *
 * @brief  RiNOO SMTP Client
 *
 *
 */

#include	"rinoo/rinoosmtp.h"

static void	smtpclient_fsm(t_tcpsocket *tcpsock, t_tcpevent event);

t_smtpsocket	*smtpclient_create(t_rinoosched *sched,
				   t_ip ip,
				   u32 port,
				   u32 timeout,
				   void (*event_fsm)(t_smtpsocket *smtpsock,
						     t_smtpevent event))
{
  t_tcpsocket	*tcpsock;
  t_smtpsocket	*smtpsock;

  XDASSERT(sched != NULL, NULL);

  tcpsock = tcp_create(sched,
		       ip,
		       port,
		       MODE_TCP_CLIENT,
		       timeout,
		       smtpclient_fsm);
  XASSERT(tcpsock != NULL, NULL);
  smtpsock = smtpsocket_create(tcpsock, event_fsm);
  XASSERT(smtpsock != NULL, NULL);
  return smtpsock;
}

static void	smtpclient_fsmin(t_smtpsocket *smtpsock)
{
  switch (smtpclient_read(smtpsock))
    {
    case 0:
      return;
    case 1:
      switch (smtpsock->event)
	{
	case EVENT_SMTP_BANNER:
	  smtpsock->event_fsm(smtpsock, EVENT_SMTP_BANNER);
	  smtpsock->event = EVENT_SMTP_BANNER;
	  buffer_erase(smtpsock->tcpsock->socket.rdbuf,
		       buffer_len(smtpsock->tcpsock->socket.rdbuf));
	  smtpsock->event_fsm(smtpsock, EVENT_SMTP_HELO);
	  smtpsock->event = EVENT_SMTP_HELO;
	  break;
	case EVENT_SMTP_HELO:
	  smtpsock->event_fsm(smtpsock, EVENT_SMTP_MAILFROM);
	  smtpsock->event = EVENT_SMTP_MAILFROM;
	  break;
	case EVENT_SMTP_MAILFROM:
	  smtpsock->event_fsm(smtpsock, EVENT_SMTP_RCPTTO);
	  smtpsock->event = EVENT_SMTP_RCPTTO;
	  break;
	case EVENT_SMTP_RCPTTO:
	  smtpsock->event_fsm(smtpsock, EVENT_SMTP_RCPTTO);
	  smtpsock->event = EVENT_SMTP_RCPTTO;
	  if (buffer_len(smtpsock->tcpsock->socket.wrbuf) == 0)
	    {
	      buffer_erase(smtpsock->tcpsock->socket.rdbuf,
			   buffer_len(smtpsock->tcpsock->socket.rdbuf));
	      smtpsock->event_fsm(smtpsock, EVENT_SMTP_DATA);
	      smtpsock->event = EVENT_SMTP_DATA;
	    }
	  break;
	case EVENT_SMTP_DATA:
	  smtpsock->event_fsm(smtpsock, EVENT_SMTP_DATAIO);
	  smtpsock->event = EVENT_SMTP_DATAIO;
	  break;
	case EVENT_SMTP_DATAIO:
	  smtpsock->event_fsm(smtpsock, EVENT_SMTP_DATAEND);
	  if (smtpsock->event == EVENT_SMTP_DATAIO)
	    {
	      /* if event_fsm doesn't jump to somewhere else */
	      smtpsock->event = EVENT_SMTP_DATAEND;
	    }
	  break;
	case EVENT_SMTP_DATAEND:
	  smtpsock->event_fsm(smtpsock, EVENT_SMTP_QUIT);
	  smtpsock->event = EVENT_SMTP_QUIT;
	  break;
	case EVENT_SMTP_QUIT:
	case EVENT_SMTP_CONNECT:
	case EVENT_SMTP_ERROR:
	case EVENT_SMTP_TIMEOUT:
	case EVENT_SMTP_CLOSE:
	  XASSERTN(0);
	  break;
	}
      break;
    case -1:
      smtpsock->event_fsm(smtpsock, EVENT_SMTP_ERROR);
      smtpsocket_destroy(smtpsock);
      return;
    }
  buffer_erase(smtpsock->tcpsock->socket.rdbuf,
	       buffer_len(smtpsock->tcpsock->socket.rdbuf));
}

static void	smtpclient_fsmout(t_smtpsocket *smtpsock)
{
  switch (smtpsock->event)
    {
    case EVENT_SMTP_DATAIO:
      smtpsock->event_fsm(smtpsock, EVENT_SMTP_DATAIO);
      break;
    case EVENT_SMTP_DATAEND:
    case EVENT_SMTP_QUIT:
    case EVENT_SMTP_ERROR:
    case EVENT_SMTP_TIMEOUT:
    case EVENT_SMTP_CLOSE:
    case EVENT_SMTP_CONNECT:
    case EVENT_SMTP_BANNER:
    case EVENT_SMTP_HELO:
    case EVENT_SMTP_MAILFROM:
    case EVENT_SMTP_RCPTTO:
    case EVENT_SMTP_DATA:
      break;
    }
}

static void	smtpclient_fsm(t_tcpsocket *tcpsock, t_tcpevent event)
{
  t_smtpsocket	*smtpsock;

  smtpsock = tcpsock->data;
  XASSERTN(smtpsock != NULL);
  switch (event)
    {
    case EVENT_TCP_CONNECT:
      smtpsock->event_fsm(smtpsock, EVENT_SMTP_CONNECT);
      smtpsock->event = EVENT_SMTP_BANNER;
      break;
    case EVENT_TCP_IN:
      smtpclient_fsmin(smtpsock);
      break;
    case EVENT_TCP_OUT:
      smtpclient_fsmout(smtpsock);
      break;
    case EVENT_TCP_ERROR:
      smtpsock->event_fsm(smtpsock, EVENT_SMTP_ERROR);
      smtpsocket_free(smtpsock);
      break;
    case EVENT_TCP_CLOSE:
      smtpsock->event_fsm(smtpsock, EVENT_SMTP_CLOSE);
      smtpsocket_free(smtpsock);
      break;
    case EVENT_TCP_TIMEOUT:
      smtpsock->event_fsm(smtpsock, EVENT_SMTP_TIMEOUT);
      smtpsocket_free(smtpsock);
      break;
    }
}
