/**
 * @file   smtpclient.h
 * @author Reginald <reginald.l@gmail.com>
 * @date   Thu Nov 25 14:44:48 2010
 *
 * @brief  Header file for SMTP client function declarations.
 *
 *
 */

#ifndef		RINOOSMTP_CLIENT_SMTPCLIENT_H_
# define	RINOOSMTP_CLIENT_SMTPCLIENT_H_

t_smtpsocket	*smtpclient_create(t_sched *sched,
				   t_ip ip,
				   u32 port,
				   u32 timeout,
				   void (*event_fsm)(t_smtpsocket *smtpsock,
						     t_smtpevent event));


#endif		/* !RINOOSMTP_CLIENT_SMTP_CLIENT_H_ */
