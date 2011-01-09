/**
 * @file   httprequest.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Nov 10 15:42:36 2010
 *
 * @brief  HTTP request functions
 *
 *
 */

#include	"rinoo/rinoohttp.h"

/**
 * Reset all member of an HTTP request.
 * This function is useful for keep-alive connections.
 *
 * @param httpsock Pointer to the HTTP socket to use.
 */
void		httprequest_reset(struct s_httpsocket *httpsock)
{
  httpsock->request.length = 0;
  httpsock->request.received = 0;
  httpsock->request.contentlength = 0;
  buffer_erase(httpsock->request.uri, buffer_len(httpsock->request.uri));
}
