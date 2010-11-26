/**
 * @file   smtpclient_read.c
 * @author Reginald <reginald.l@gmail.com>
 * @date   Wed Nov 24 16:20:09 2010
 *
 * @brief  SMTP client read functions
 *
 *
 */

#include	"rinoo/rinoosmtp.h"

/**
 * Reads SMTP client session. This function just validates
 * the input since we don't need any information from server.
 *
 * @param smtpsock Pointer to the smtpsocket to use.
 *
 * @return 0 if input data is incomplete (need to read more), 1 on success, -1 if an error occurs.
 */
int    	smtpclient_read(t_smtpsocket *smtpsock)
{
  int	ret = -1;
  u32	code = 0;

  if (memrchr(buffer_ptr(smtpsock->tcpsock->socket.rdbuf),
	      '\n',
	      buffer_len(smtpsock->tcpsock->socket.rdbuf)) == NULL)
    {
      return 0;
    }
  if (memcmp(buffer_ptr(smtpsock->tcpsock->socket.rdbuf),
	     "220 ",
	     MIN(buffer_len(smtpsock->tcpsock->socket.rdbuf), 4)) == 0)
    {
      code = 220;
    }
  else if (memcmp(buffer_ptr(smtpsock->tcpsock->socket.rdbuf),
		  "250 ",
		  MIN(buffer_len(smtpsock->tcpsock->socket.rdbuf), 4)) == 0)
    {
      code = 250;
    }
  else if (memcmp(buffer_ptr(smtpsock->tcpsock->socket.rdbuf),
		  "354 ",
		  MIN(buffer_len(smtpsock->tcpsock->socket.rdbuf), 4)) == 0)
    {
      code = 354;
    }
  else if (memcmp(buffer_ptr(smtpsock->tcpsock->socket.rdbuf),
		  "221 ",
		  MIN(buffer_len(smtpsock->tcpsock->socket.rdbuf), 4)) == 0)
    {
      code = 221;
    }
  else
    {
      return -1;
    }

  switch (smtpsock->event)
    {
    case EVENT_SMTP_BANNER:
      if (code == 220)
	{
	  ret = 1;
	}
      break;
    case EVENT_SMTP_HELO:
    case EVENT_SMTP_MAILFROM:
    case EVENT_SMTP_RCPTTO:
    case EVENT_SMTP_DATAIO:
      if (code == 250)
	{
	  ret = 1;
	}
      break;
    case EVENT_SMTP_DATA:
      if (code == 354)
	{
	  ret = 1;
	}
      break;
    case EVENT_SMTP_DATAEND:
      if (code == 221)
	{
	  ret = 1;
	}
      break;
    case EVENT_SMTP_CONNECT:
    case EVENT_SMTP_QUIT:
    case EVENT_SMTP_ERROR:
    case EVENT_SMTP_TIMEOUT:
    case EVENT_SMTP_CLOSE:
      ret = -1;
      break;
    }
  return ret;
}
