/**
 * @file   http_response.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  HTTP response functions
 *
 *
 */

#include "rinoo/proto/http/module.h"

/**
 * Reads a http response.
 *
 * @param http Pointer to a http structure
 *
 * @return true if a response has been correctly read, otherwise false
 */
bool rn_http_response_get(rn_http_t *http)
{
	int ret;

	rn_http_reset(http);
	while (rn_socket_readb(http->socket, http->response.buffer) > 0) {
		ret = rn_http_response_parse(http);
		if (ret == 1) {
			while (rn_buffer_size(http->response.buffer) < http->response.headers.length + http->response.headers.content_length) {
				if (rn_socket_readb(http->socket, http->response.buffer) <= 0) {
					return false;
				}
			}
			rn_buffer_static(&http->response.content, rn_buffer_ptr(http->response.buffer) + http->response.headers.length, http->response.headers.content_length);
			return true;
		} else if (ret == -1) {
			rn_http_reset(http);
			return false;
		}
	}
	return false;
}

/**
 * Set the HTTP message of a HTTP response.
 *
 * @param http Pointer to the HTTP context to use.
 * @param msg HTTP response message to use.
 */
void rn_http_response_setmsg(rn_http_t *http, const char *msg)
{
	rn_buffer_set(&http->response.msg, msg);
}

/**
 * Set the default HTTP response message according to the response code.
 *
 * @param http Pointer to the HTTP context to use
 */
void rn_http_response_setdefaultmsg(rn_http_t *http)
{
	switch (http->response.code)
	{
	case 100:
		rn_http_response_setmsg(http, "Continue");
		break;
	case 101:
		rn_http_response_setmsg(http, "Switching Protocols");
		break;
	case 200:
		rn_http_response_setmsg(http, "OK");
		break;
	case 201:
		rn_http_response_setmsg(http, "Created");
		break;
	case 202:
		rn_http_response_setmsg(http, "Accepted");
		break;
	case 203:
		rn_http_response_setmsg(http, "Non-Authoritative Information");
		break;
	case 204:
		rn_http_response_setmsg(http, "No Content");
		break;
	case 205:
		rn_http_response_setmsg(http, "Reset Content");
		break;
	case 206:
		rn_http_response_setmsg(http, "Partial Content");
		break;
	case 300:
		rn_http_response_setmsg(http, "Multiple Choices");
		break;
	case 301:
		rn_http_response_setmsg(http, "Moved Permanently");
		break;
	case 302:
		rn_http_response_setmsg(http, "Found");
		break;
	case 303:
		rn_http_response_setmsg(http, "See Other");
		break;
	case 304:
		rn_http_response_setmsg(http, "Not Modified");
		break;
	case 305:
		rn_http_response_setmsg(http, "Use Proxy");
		break;
	case 306:
		rn_http_response_setmsg(http, "(Unused)");
		break;
	case 307:
		rn_http_response_setmsg(http, "Temporary Redirect");
		break;
	case 400:
		rn_http_response_setmsg(http, "Bad Request");
		break;
	case 401:
		rn_http_response_setmsg(http, "Unauthorized");
		break;
	case 402:
		rn_http_response_setmsg(http, "Payment Required");
		break;
	case 403:
		rn_http_response_setmsg(http, "Forbidden");
		break;
	case 404:
		rn_http_response_setmsg(http, "Not Found");
		break;
	case 405:
		rn_http_response_setmsg(http, "Method Not Allowed");
		break;
	case 406:
		rn_http_response_setmsg(http, "Not Acceptable");
		break;
	case 407:
		rn_http_response_setmsg(http, "Proxy Authentication Required");
		break;
	case 408:
		rn_http_response_setmsg(http, "Request Timeout");
		break;
	case 409:
		rn_http_response_setmsg(http, "Conflict");
		break;
	case 410:
		rn_http_response_setmsg(http, "Gone");
		break;
	case 411:
		rn_http_response_setmsg(http, "Length Required");
		break;
	case 412:
		rn_http_response_setmsg(http, "Precondition Failed");
		break;
	case 413:
		rn_http_response_setmsg(http, "Request Entity Too Large");
		break;
	case 414:
		rn_http_response_setmsg(http, "Request-URI Too Long");
		break;
	case 415:
		rn_http_response_setmsg(http, "Unsupported Media Type");
		break;
	case 416:
		rn_http_response_setmsg(http, "Requested Range Not Satisfiable");
		break;
	case 417:
		rn_http_response_setmsg(http, "Expectation Failed");
		break;
	case 500:
		rn_http_response_setmsg(http, "Internal Server Error");
		break;
	case 501:
		rn_http_response_setmsg(http, "Not Implemented");
		break;
	case 502:
		rn_http_response_setmsg(http, "Bad Gateway");
		break;
	case 503:
		rn_http_response_setmsg(http, "Service Unavailable");
		break;
	case 504:
		rn_http_response_setmsg(http, "Gateway Timeout");
		break;
	case 505:
		rn_http_response_setmsg(http, "HTTP Version Not Supported");
		break;
	default:
		http->response.code = 500;
		rn_http_response_setmsg(http, "Internal Error");
		break;
	}
}

/**
 * Sets default http response headers.
 *
 * @param http Pointer to a http structure
 */
void rn_http_response_setdefaultheaders(rn_http_t *http)
{
	char tmp[24];

	if (rn_http_header_get(&http->response.headers, "Content-Length") == NULL) {
		if (snprintf(tmp, 24, "%lu", http->response.headers.content_length) > 0) {
			rn_http_header_set(&http->response.headers, "Content-Length", tmp);
		}
	}
	if (rn_http_header_get(&http->response.headers, "Server") == NULL) {
		rn_http_header_set(&http->response.headers, "Server", RN_HTTP_SIGNATURE);
	}
}

/**
 * Prepares HTTP response buffer. This function forges a buffer containing
 * the raw HTTP response (response code, message and headers) with no response
 * content. The result is available in the http response buffer (rn_http_t
 * structure).
 *
 * @param http HTTP structure
 * @param body_length HTTP Content length
 *
 * @result 0 on success, otherwise -1
 */
int rn_http_response_prepare(rn_http_t *http, size_t body_length)
{
	rn_http_header_t *cur_header;

	XASSERT(http != NULL, -1);
	XASSERT(rn_buffer_size(http->response.buffer) == 0, -1);

	http->response.headers.content_length = body_length;
	rn_http_response_setdefaultheaders(http);
	rn_http_response_setdefaultmsg(http);
	switch (http->version) {
	case RN_HTTP_VERSION_10:
		rn_buffer_add(http->response.buffer, "HTTP/1.0", 8);
		break;
	default:
		rn_buffer_add(http->response.buffer, "HTTP/1.1", 8);
		break;
	}
	rn_buffer_print(http->response.buffer, " %d %.*s\r\n", http->response.code, rn_buffer_size(&http->response.msg), rn_buffer_ptr(&http->response.msg));
	for (cur_header = rn_http_header_first(&http->response.headers);
	     cur_header != NULL;
	     cur_header = rn_http_header_next(cur_header)) {
		rn_buffer_print(http->response.buffer,
			     "%.*s: %.*s\r\n",
			     rn_buffer_size(&cur_header->key),
			     rn_buffer_ptr(&cur_header->key),
			     rn_buffer_size(&cur_header->value),
			     rn_buffer_ptr(&cur_header->value));
	}
	rn_buffer_add(http->response.buffer, "\r\n", 2);
	return 0;
}

/**
 * Sends a http response.
 *
 * @param http Pointer to a http structure
 * @param body Pointer to http response body, if any
 *
 * @return 0 on success, otherwise -1
 */
int rn_http_response_send(rn_http_t *http, rn_buffer_t *body)
{
	rn_buffer_t *buffers[2];

	if (rn_http_response_prepare(http, (body != NULL ? rn_buffer_size(body) : 0)) != 0) {
		return -1;
	}
	if (body == NULL) {
		if (rn_socket_writeb(http->socket, http->response.buffer) != (ssize_t) rn_buffer_size(http->response.buffer)) {
			return -1;
		}
	} else {
		buffers[0] = http->response.buffer;
		buffers[1] = body;
		if (rn_socket_writev(http->socket, buffers, 2) != (ssize_t)(rn_buffer_size(http->response.buffer) + rn_buffer_size(body))) {
			return -1;
		}
	}
	return 0;
}
