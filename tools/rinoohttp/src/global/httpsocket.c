/**
 * @file   httpsocket.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
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
t_httpsocket	*httpsocket_create(t_rinootcp *tcpsock,
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
      httpsocket_free(httpsock);
      XASSERT(0, NULL);
    }
  httpsock->request.headers = httpheader_createtable();
  if (httpsock->request.headers == NULL)
    {
      httpsocket_free(httpsock);
      XASSERT(0, NULL);
    }
  httpsock->response.headers = httpheader_createtable();
  if (httpsock->response.headers == NULL)
    {
      httpsocket_free(httpsock);
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
  if (httpsock->request.headers != NULL)
    {
      httpheader_destroytable(httpsock->request.headers);
      httpsock->request.headers = NULL;
    }
  if (httpsock->response.headers != NULL)
    {
      httpheader_destroytable(httpsock->response.headers);
      httpsock->response.headers = NULL;
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
      rinoo_tcp_destroy(httpsock->tcpsock);
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
  httprequest_reset(httpsock);
  httpresponse_reset(httpsock);
  httpsock->last_event = EVENT_HTTP_CONNECT;
}
