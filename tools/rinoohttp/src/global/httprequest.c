/**
 * @file   httprequest.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2010
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
 * @param req Pointer to the request to reset.
 */
void		httprequest_reset(t_httprequest *req)
{
  req->length = 0;
  req->received = 0;
  req->contentlength = 0;
  buffer_erase(req->uri, buffer_len(req->uri));
}
