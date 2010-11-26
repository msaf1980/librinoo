/**
 * @file   httpresponse.c
 * @author Reginald LIPS <rlips@cloudmark.com>
 * @date   Fri Nov 19 12:49:12 2010
 *
 * @brief  HTTP response functions
 *
 *
 */

#include	"rinoo/rinoohttp.h"

/**
 * Reset all members of an HTTP response.
 * This function is useful for keep-alive connections.
 *
 * @param resp Pointer to the response to reset.
 */
void		httpresponse_reset(t_httpresponse *resp)
{
  resp->version = 0;
  resp->code = 0;
  resp->msg.buf = NULL;
  resp->msg.len = 0;
  resp->contentlength = 0;
  resp->body.buf = NULL;
  resp->body.len = 0;
}
