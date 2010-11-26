/**
 * @file   smtpsocket.c
 * @author Reginald <reginald.l@gmail.com>
 * @date   Wed Nov 24 16:08:09 2010
 *
 * @brief  Functions to create and manage SMTP sockets.
 *
 *
 */

#include	"rinoo/rinoosmtp.h"

/**
 * Create a new smtpsocket over a tcpsocket.
 *
 * @param tcpsock Pointer to the tcpsocket to use.
 * @param event_fsm Pointer to a SMTP event fsm.
 *
 * @return A pointer to the new smtpsocket, or NULL if an error occurs.
 */
t_smtpsocket	*smtpsocket_create(t_tcpsocket *tcpsock,
				   void (*event_fsm)(t_smtpsocket *smtpsock,
						     t_smtpevent event))
{
  t_smtpsocket	*smtpsock;

  XASSERT(tcpsock != NULL, NULL);

  smtpsock = xcalloc(1, sizeof(*smtpsock));
  XASSERT(smtpsock != NULL, NULL);
  smtpsock->tcpsock = tcpsock;
  smtpsock->event_fsm = event_fsm;
  tcpsock->data = smtpsock;
  return smtpsock;
}

/**
 * Free a smtpsocket but does not destroy the underlying tcpsocket.
 *
 * @param smtpsock Pointer to the smtpsocket to free.
 */
void		smtpsocket_free(t_smtpsocket *smtpsock)
{
  xfree(smtpsock);
}

/**
 * Destroy a smtpsocket and the underlying tcpsocket.
 *
 * @param smtpsock Pointer to the smtpsocket to destroy.
 */
void		smtpsocket_destroy(t_smtpsocket *smtpsock)
{
  if (smtpsock->tcpsock != NULL)
    {
      smtpsock->tcpsock->data = NULL;
      tcp_destroy(smtpsock->tcpsock);
      smtpsock->tcpsock = NULL;
    }
  smtpsocket_free(smtpsock);
}
