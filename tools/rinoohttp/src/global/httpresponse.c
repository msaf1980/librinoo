/**
 * @file   httpresponse.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Fri Nov 19 12:49:12 2010
 *
 * @brief  HTTP response functions
 *
 *
 */

#include	"rinoo/rinoohttp.h"

/**
 * Reset all members of the HTTP response from a HTTP socket.
 * This function is useful for keep-alive connections.
 *
 * @param httpsock Pointer to the HTTP socket to use.
 */
void		rinoo_http_response_reset(struct s_rinoohttp *httpsock)
{
  rinoo_http_header_destroytable(httpsock->response.headers);
  memset(&httpsock->response, 0, sizeof(httpsock->response));
  httpsock->response.headers = rinoo_http_header_createtable();
}

/**
 * Set the HTTP message of a HTTP response.
 *
 * @param httpsock Pointer to the HTTP socket to use.
 * @param msg HTTP response message to use.
 */
void		rinoo_http_response_setmsg(struct s_rinoohttp *httpsock,
					   const char *msg)
{
  strtobuffer(httpsock->response.msg, msg);
}

/**
 * Set the default HTTP response message according to the response code.
 *
 * @param httpsock Pointer to the HTTP socket to use.
 */
void		rinoo_http_response_setdefaultmsg(struct s_rinoohttp *httpsock)
{
  switch (httpsock->response.code)
    {
    case 100:
      rinoo_http_response_setmsg(httpsock, "Continue");
      break;
    case 101:
      rinoo_http_response_setmsg(httpsock, "Switching Protocols");
      break;
    case 200:
      rinoo_http_response_setmsg(httpsock, "OK");
      break;
    case 201:
      rinoo_http_response_setmsg(httpsock, "Created");
      break;
    case 202:
      rinoo_http_response_setmsg(httpsock, "Accepted");
      break;
    case 203:
      rinoo_http_response_setmsg(httpsock, "Non-Authoritative Information");
      break;
    case 204:
      rinoo_http_response_setmsg(httpsock, "No Content");
      break;
    case 205:
      rinoo_http_response_setmsg(httpsock, "Reset Content");
      break;
    case 206:
      rinoo_http_response_setmsg(httpsock, "Partial Content");
      break;
    case 300:
      rinoo_http_response_setmsg(httpsock, "Multiple Choices");
      break;
    case 301:
      rinoo_http_response_setmsg(httpsock, "Moved Permanently");
      break;
    case 302:
      rinoo_http_response_setmsg(httpsock, "Found");
      break;
    case 303:
      rinoo_http_response_setmsg(httpsock, "See Other");
      break;
    case 304:
      rinoo_http_response_setmsg(httpsock, "Not Modified");
      break;
    case 305:
      rinoo_http_response_setmsg(httpsock, "Use Proxy");
      break;
    case 306:
      rinoo_http_response_setmsg(httpsock, "(Unused)");
      break;
    case 307:
      rinoo_http_response_setmsg(httpsock, "Temporary Redirect");
    case 400:
      rinoo_http_response_setmsg(httpsock, "Bad Request");
    case 401:
      rinoo_http_response_setmsg(httpsock, "Unauthorized");
      break;
    case 402:
      rinoo_http_response_setmsg(httpsock, "Payment Required");
      break;
    case 403:
      rinoo_http_response_setmsg(httpsock, "Forbidden");
      break;
    case 404:
      rinoo_http_response_setmsg(httpsock, "Not Found");
      break;
    case 405:
      rinoo_http_response_setmsg(httpsock, "Method Not Allowed");
      break;
    case 406:
      rinoo_http_response_setmsg(httpsock, "Not Acceptable");
      break;
    case 407:
      rinoo_http_response_setmsg(httpsock, "Proxy Authentication Required");
      break;
    case 408:
      rinoo_http_response_setmsg(httpsock, "Request Timeout");
      break;
    case 409:
      rinoo_http_response_setmsg(httpsock, "Conflict");
      break;
    case 410:
      rinoo_http_response_setmsg(httpsock, "Gone");
      break;
    case 411:
      rinoo_http_response_setmsg(httpsock, "Length Required");
      break;
    case 412:
      rinoo_http_response_setmsg(httpsock, "Precondition Failed");
      break;
    case 413:
      rinoo_http_response_setmsg(httpsock, "Request Entity Too Large");
      break;
    case 414:
      rinoo_http_response_setmsg(httpsock, "Request-URI Too Long");
      break;
    case 415:
      rinoo_http_response_setmsg(httpsock, "Unsupported Media Type");
      break;
    case 416:
      rinoo_http_response_setmsg(httpsock, "Requested Range Not Satisfiable");
      break;
    case 417:
      rinoo_http_response_setmsg(httpsock, "Expectation Failed");
      break;
    case 500:
      rinoo_http_response_setmsg(httpsock, "Internal Server Error");
      break;
    case 501:
      rinoo_http_response_setmsg(httpsock, "Not Implemented");
      break;
    case 502:
      rinoo_http_response_setmsg(httpsock, "Bad Gateway");
      break;
    case 503:
      rinoo_http_response_setmsg(httpsock, "Service Unavailable");
      break;
    case 504:
      rinoo_http_response_setmsg(httpsock, "Gateway Timeout");
      break;
    case 505:
      rinoo_http_response_setmsg(httpsock, "HTTP Version Not Supported");
      break;
    default:
      httpsock->response.code = 500;
      rinoo_http_response_setmsg(httpsock, "Internal Error");
      break;
    }
}

void		rinoo_http_response_setdefaultheaders(struct s_rinoohttp *httpsock)
{
  char		tmp[24];

  if (rinoo_http_header_get(httpsock->response.headers, "Content-Length") == NULL)
    {
      if (snprintf(tmp, 42, "%llu", httpsock->response.contentlength) > 0)
	{
	  rinoo_http_header_add(httpsock->response.headers, "Content-Length", tmp);
	}
    }
  if (rinoo_http_header_get(httpsock->response.headers, "Server") == NULL)
    {
      rinoo_http_header_add(httpsock->response.headers,
			    "Server",
			    RINOOHTTP_HEADER_DEFAULT_SERVER);
    }
}
