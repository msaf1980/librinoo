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
void		httpresponse_reset(t_httpsocket *httpsock)
{
  httpheader_destroytable(httpsock->response.headers);
  memset(&httpsock->response, 0, sizeof(httpsock->response));
  httpsock->response.headers = httpheader_createtable();
}

/**
 * Set the HTTP message of a HTTP response.
 *
 * @param httpsock Pointer to the HTTP socket to use.
 * @param msg HTTP response message to use.
 */
void		httpresponse_setmsg(struct s_httpsocket *httpsock, const char *msg)
{
  strtobuffer(httpsock->response.msg, msg);
}

/**
 * Set the default HTTP response message according to the response code.
 *
 * @param httpsock Pointer to the HTTP socket to use.
 */
void		httpresponse_setdefaultmsg(struct s_httpsocket *httpsock)
{
  switch (httpsock->response.code)
    {
    case 100:
      httpresponse_setmsg(httpsock, "Continue");
      break;
    case 101:
      httpresponse_setmsg(httpsock, "Switching Protocols");
      break;
    case 200:
      httpresponse_setmsg(httpsock, "OK");
      break;
    case 201:
      httpresponse_setmsg(httpsock, "Created");
      break;
    case 202:
      httpresponse_setmsg(httpsock, "Accepted");
      break;
    case 203:
      httpresponse_setmsg(httpsock, "Non-Authoritative Information");
      break;
    case 204:
      httpresponse_setmsg(httpsock, "No Content");
      break;
    case 205:
      httpresponse_setmsg(httpsock, "Reset Content");
      break;
    case 206:
      httpresponse_setmsg(httpsock, "Partial Content");
      break;
    case 300:
      httpresponse_setmsg(httpsock, "Multiple Choices");
      break;
    case 301:
      httpresponse_setmsg(httpsock, "Moved Permanently");
      break;
    case 302:
      httpresponse_setmsg(httpsock, "Found");
      break;
    case 303:
      httpresponse_setmsg(httpsock, "See Other");
      break;
    case 304:
      httpresponse_setmsg(httpsock, "Not Modified");
      break;
    case 305:
      httpresponse_setmsg(httpsock, "Use Proxy");
      break;
    case 306:
      httpresponse_setmsg(httpsock, "(Unused)");
      break;
    case 307:
      httpresponse_setmsg(httpsock, "Temporary Redirect");
    case 400:
      httpresponse_setmsg(httpsock, "Bad Request");
    case 401:
      httpresponse_setmsg(httpsock, "Unauthorized");
      break;
    case 402:
      httpresponse_setmsg(httpsock, "Payment Required");
      break;
    case 403:
      httpresponse_setmsg(httpsock, "Forbidden");
      break;
    case 404:
      httpresponse_setmsg(httpsock, "Not Found");
      break;
    case 405:
      httpresponse_setmsg(httpsock, "Method Not Allowed");
      break;
    case 406:
      httpresponse_setmsg(httpsock, "Not Acceptable");
      break;
    case 407:
      httpresponse_setmsg(httpsock, "Proxy Authentication Required");
      break;
    case 408:
      httpresponse_setmsg(httpsock, "Request Timeout");
      break;
    case 409:
      httpresponse_setmsg(httpsock, "Conflict");
      break;
    case 410:
      httpresponse_setmsg(httpsock, "Gone");
      break;
    case 411:
      httpresponse_setmsg(httpsock, "Length Required");
      break;
    case 412:
      httpresponse_setmsg(httpsock, "Precondition Failed");
      break;
    case 413:
      httpresponse_setmsg(httpsock, "Request Entity Too Large");
      break;
    case 414:
      httpresponse_setmsg(httpsock, "Request-URI Too Long");
      break;
    case 415:
      httpresponse_setmsg(httpsock, "Unsupported Media Type");
      break;
    case 416:
      httpresponse_setmsg(httpsock, "Requested Range Not Satisfiable");
      break;
    case 417:
      httpresponse_setmsg(httpsock, "Expectation Failed");
      break;
    case 500:
      httpresponse_setmsg(httpsock, "Internal Server Error");
      break;
    case 501:
      httpresponse_setmsg(httpsock, "Not Implemented");
      break;
    case 502:
      httpresponse_setmsg(httpsock, "Bad Gateway");
      break;
    case 503:
      httpresponse_setmsg(httpsock, "Service Unavailable");
      break;
    case 504:
      httpresponse_setmsg(httpsock, "Gateway Timeout");
      break;
    case 505:
      httpresponse_setmsg(httpsock, "HTTP Version Not Supported");
      break;
    default:
      httpsock->response.code = 500;
      httpresponse_setmsg(httpsock, "Internal Error");
      break;
    }
}

void		httpresponse_setdefaultheaders(struct s_httpsocket *httpsock)
{
  char		tmp[24];

  if (httpheader_get(httpsock->response.headers, "Content-Length") == NULL)
    {
      if (snprintf(tmp, 42, "%llu", httpsock->response.contentlength) > 0)
	{
	  httpheader_add(httpsock->response.headers, "Content-Length", tmp);
	}
    }
  if (httpheader_get(httpsock->response.headers, "Server") == NULL)
    {
      httpheader_add(httpsock->response.headers, "Server", HTTPHEADER_DEFAULT_SERVER);
    }
}
