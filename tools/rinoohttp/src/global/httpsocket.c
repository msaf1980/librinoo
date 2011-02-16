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
t_rinoohttp	*rinoo_http_socket(t_rinootcp *tcpsock,
				   void (*event_fsm)(t_rinoohttp *httpsock,
						     t_rinoohttp_event event))
{
  t_rinoohttp	*httpsock;

  XASSERT(tcpsock != NULL, NULL);

  httpsock = xcalloc(1, sizeof(*httpsock));
  XASSERT(httpsock != NULL, NULL);
  httpsock->request.uri = buffer_create(URI_INITSIZE, URI_MAXSIZE);
  if (httpsock->request.uri == NULL)
    {
      rinoo_http_socket_free(httpsock);
      XASSERT(0, NULL);
    }
  httpsock->request.headers = rinoo_http_header_createtable();
  if (httpsock->request.headers == NULL)
    {
      rinoo_http_socket_free(httpsock);
      XASSERT(0, NULL);
    }
  httpsock->response.headers = rinoo_http_header_createtable();
  if (httpsock->response.headers == NULL)
    {
      rinoo_http_socket_free(httpsock);
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
void		rinoo_http_socket_free(t_rinoohttp *httpsock)
{
  if (httpsock->request.uri != NULL)
    {
      buffer_destroy(httpsock->request.uri);
      httpsock->request.uri = NULL;
    }
  if (httpsock->request.headers != NULL)
    {
      rinoo_http_header_destroytable(httpsock->request.headers);
      httpsock->request.headers = NULL;
    }
  if (httpsock->response.headers != NULL)
    {
      rinoo_http_header_destroytable(httpsock->response.headers);
      httpsock->response.headers = NULL;
    }
  xfree(httpsock);
}

/**
 * Destroy an httpsocket and the underlying tcpsocket.
 *
 * @param httpsock Pointer to the httpsocket to destroy.
 */
void		rinoo_http_socket_destroy(t_rinoohttp *httpsock)
{
  if (httpsock->tcpsock != NULL)
    {
      httpsock->tcpsock->data = NULL;
      rinoo_tcp_destroy(httpsock->tcpsock);
      httpsock->tcpsock = NULL;
    }
  rinoo_http_socket_free(httpsock);
}

/**
 * Reset all request and response members of an HTTP socket.
 * This function is useful for keep-alive connections.
 *
 * @param httpsock Pointer to the HTTP socket to reset.
 */
void		rinoo_http_socket_reset(t_rinoohttp *httpsock)
{
  rinoo_http_request_reset(httpsock);
  rinoo_http_response_reset(httpsock);
  httpsock->last_event = EVENT_HTTP_CONNECT;
}
