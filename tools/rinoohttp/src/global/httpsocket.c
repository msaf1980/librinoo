/**
 * @file   httpsocket.c
 * @author Reginald <reginald.l@gmail.com> - Copyright 2010
 * @date   Tue Oct 26 21:20:23 2010
 *
 * @brief  Functions to create and manage http sockets
 *
 *
 */

#include	"rinoo/rinoohttp.h"

/**
 * Create a new httpsocket over a tcpsocket.
 *
 * @param tcpsock Pointer to the tcpsocket to use.
 * @param event_fsm Pointer to an HTTP event fsm.
 *
 * @return A pointer to the new httpsocket, or NULL if an error occurs.
 */
t_httpsocket	*httpsocket_create(t_tcpsocket *tcpsock,
				   void (*event_fsm)(t_httpsocket *httpsock,
						     t_httpevent event))
{
  t_httpsocket	*httpsock;

  XASSERT(tcpsock != NULL, NULL);

  httpsock = xcalloc(1, sizeof(*httpsock));
  XASSERT(httpsock != NULL, NULL);
  httpsock->request.uri = buffer_create(URI_INITSIZE, URI_MAXSIZE);
  if (httpsock->request.uri == NULL)
    {
      xfree(httpsock);
      XASSERT(0, NULL);
    }
  httpsock->tcpsock = tcpsock;
  httpsock->event_fsm = event_fsm;
  tcpsock->data = httpsock;
  return httpsock;
}

/**
 * Free an httpsocket but does not destroy the underlying tcpsocket.
 *
 * @param httpsock Pointer to the httpsocket to free.
 */
void		httpsocket_free(t_httpsocket *httpsock)
{
  if (httpsock->request.uri != NULL)
    {
      buffer_destroy(httpsock->request.uri);
      httpsock->request.uri = NULL;
    }
  xfree(httpsock);
}

/**
 * Destroy an httpsocket and the underlying tcpsocket.
 *
 * @param httpsock Pointer to the httpsocket to destroy.
 */
void		httpsocket_destroy(t_httpsocket *httpsock)
{
  if (httpsock->tcpsock != NULL)
    {
      httpsock->tcpsock->data = NULL;
      tcp_destroy(httpsock->tcpsock);
      httpsock->tcpsock = NULL;
    }
  httpsocket_free(httpsock);
}

/**
 * Reset all request and response members of an HTTP socket.
 * This function is useful for keep-alive connections.
 *
 * @param httpsock Pointer to the HTTP socket to reset.
 */
void		httpsocket_reset(t_httpsocket *httpsock)
{
  httprequest_reset(&httpsock->request);
  httpresponse_reset(&httpsock->response);
}
