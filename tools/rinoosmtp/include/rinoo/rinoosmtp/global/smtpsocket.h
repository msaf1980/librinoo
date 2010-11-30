/**
 * @file   smtpsocket.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2010
 * @date   Wed Nov 24 15:23:19 2010
 *
 * @brief  Header file for SMTP socket declarations.
 *
 *
 */

#ifndef		RINOOSMTP_GLOBAL_SMTPSOCKET_H_
# define	RINOOSTMP_GLOBAL_SMTPSOCKET_H_

typedef enum	e_smtpevent
  {
    EVENT_SMTP_CONNECT = 0,
    EVENT_SMTP_BANNER,
    EVENT_SMTP_HELO,
    EVENT_SMTP_MAILFROM,
    EVENT_SMTP_RCPTTO,
    EVENT_SMTP_DATA,
    EVENT_SMTP_DATAIO,
    EVENT_SMTP_DATAEND,
    EVENT_SMTP_QUIT,
    EVENT_SMTP_ERROR,
    EVENT_SMTP_TIMEOUT,
    EVENT_SMTP_CLOSE
  }		t_smtpevent;

typedef struct	s_smtpsocket
{
  t_tcpsocket	*tcpsock;
  t_smtpevent	event;
  void		(*event_fsm)(struct s_smtpsocket *smtpsocket,
			     t_smtpevent event);
  void		*data;
}		t_smtpsocket;

t_smtpsocket	*smtpsocket_create(t_tcpsocket *tcpsock,
				   void (*event_fsm)(t_smtpsocket *smtpsock,
						     t_smtpevent event));
void		smtpsocket_free(t_smtpsocket *smtpsock);
void		smtpsocket_destroy(t_smtpsocket *smtpsock);
int	    	smtpclient_read(t_smtpsocket *smtpsock);

#endif		/* !RINOOSMTP_GLOBAL_SMTPSOCKET_H_ */
