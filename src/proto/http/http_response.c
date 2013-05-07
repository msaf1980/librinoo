/**
 * @file   http_response.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2013
 * @date   Fri Nov 19 12:49:12 2010
 *
 * @brief  HTTP response functions
 *
 *
 */

#include "rinoo/rinoo.h"

/**
 * Reads a http response.
 *
 * @param http Pointer to a http structure
 *
 * @return true if a response has been correctly read, otherwise false
 */
bool rinoohttp_response_get(t_rinoohttp *http)
{
	int ret;

	rinoohttp_reset(http);
	while (rinoo_socket_readb(http->socket, http->response.buffer) > 0) {
		ret = rinoohttp_response_parse(http);
		if (ret == 1) {
			while (buffer_size(http->response.buffer) < http->response.headers_length + http->response.content_length) {
				if (rinoo_socket_readb(http->socket, http->response.buffer) <= 0) {
					return false;
				}
			}
			buffer_static(&http->response.content, buffer_ptr(http->response.buffer) + http->response.headers_length, http->response.content_length);
			return true;
		} else if (ret == -1) {
			rinoohttp_reset(http);
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
void rinoohttp_response_setmsg(t_rinoohttp *http, const char *msg)
{
	strtobuffer(&http->response.msg, msg);
}

/**
 * Set the default HTTP response message according to the response code.
 *
 * @param http Pointer to the HTTP context to use
 */
void rinoohttp_response_setdefaultmsg(t_rinoohttp *http)
{
	switch (http->response.code)
	{
	case 100:
		rinoohttp_response_setmsg(http, "Continue");
		break;
	case 101:
		rinoohttp_response_setmsg(http, "Switching Protocols");
		break;
	case 200:
		rinoohttp_response_setmsg(http, "OK");
		break;
	case 201:
		rinoohttp_response_setmsg(http, "Created");
		break;
	case 202:
		rinoohttp_response_setmsg(http, "Accepted");
		break;
	case 203:
		rinoohttp_response_setmsg(http, "Non-Authoritative Information");
		break;
	case 204:
		rinoohttp_response_setmsg(http, "No Content");
		break;
	case 205:
		rinoohttp_response_setmsg(http, "Reset Content");
		break;
	case 206:
		rinoohttp_response_setmsg(http, "Partial Content");
		break;
	case 300:
		rinoohttp_response_setmsg(http, "Multiple Choices");
		break;
	case 301:
		rinoohttp_response_setmsg(http, "Moved Permanently");
		break;
	case 302:
		rinoohttp_response_setmsg(http, "Found");
		break;
	case 303:
		rinoohttp_response_setmsg(http, "See Other");
		break;
	case 304:
		rinoohttp_response_setmsg(http, "Not Modified");
		break;
	case 305:
		rinoohttp_response_setmsg(http, "Use Proxy");
		break;
	case 306:
		rinoohttp_response_setmsg(http, "(Unused)");
		break;
	case 307:
		rinoohttp_response_setmsg(http, "Temporary Redirect");
		break;
	case 400:
		rinoohttp_response_setmsg(http, "Bad Request");
		break;
	case 401:
		rinoohttp_response_setmsg(http, "Unauthorized");
		break;
	case 402:
		rinoohttp_response_setmsg(http, "Payment Required");
		break;
	case 403:
		rinoohttp_response_setmsg(http, "Forbidden");
		break;
	case 404:
		rinoohttp_response_setmsg(http, "Not Found");
		break;
	case 405:
		rinoohttp_response_setmsg(http, "Method Not Allowed");
		break;
	case 406:
		rinoohttp_response_setmsg(http, "Not Acceptable");
		break;
	case 407:
		rinoohttp_response_setmsg(http, "Proxy Authentication Required");
		break;
	case 408:
		rinoohttp_response_setmsg(http, "Request Timeout");
		break;
	case 409:
		rinoohttp_response_setmsg(http, "Conflict");
		break;
	case 410:
		rinoohttp_response_setmsg(http, "Gone");
		break;
	case 411:
		rinoohttp_response_setmsg(http, "Length Required");
		break;
	case 412:
		rinoohttp_response_setmsg(http, "Precondition Failed");
		break;
	case 413:
		rinoohttp_response_setmsg(http, "Request Entity Too Large");
		break;
	case 414:
		rinoohttp_response_setmsg(http, "Request-URI Too Long");
		break;
	case 415:
		rinoohttp_response_setmsg(http, "Unsupported Media Type");
		break;
	case 416:
		rinoohttp_response_setmsg(http, "Requested Range Not Satisfiable");
		break;
	case 417:
		rinoohttp_response_setmsg(http, "Expectation Failed");
		break;
	case 500:
		rinoohttp_response_setmsg(http, "Internal Server Error");
		break;
	case 501:
		rinoohttp_response_setmsg(http, "Not Implemented");
		break;
	case 502:
		rinoohttp_response_setmsg(http, "Bad Gateway");
		break;
	case 503:
		rinoohttp_response_setmsg(http, "Service Unavailable");
		break;
	case 504:
		rinoohttp_response_setmsg(http, "Gateway Timeout");
		break;
	case 505:
		rinoohttp_response_setmsg(http, "HTTP Version Not Supported");
		break;
	default:
		http->response.code = 500;
		rinoohttp_response_setmsg(http, "Internal Error");
		break;
	}
}

/**
 * Sets default http response headers.
 *
 * @param http Pointer to a http structure
 */
void rinoohttp_response_setdefaultheaders(t_rinoohttp *http)
{
	char tmp[24];

	if (rinoohttp_header_get(&http->response.headers, "Content-Length") == NULL) {
		if (snprintf(tmp, 24, "%lu", http->response.content_length) > 0) {
			rinoohttp_header_set(&http->response.headers, "Content-Length", tmp);
		}
	}
	if (rinoohttp_header_get(&http->response.headers, "Server") == NULL) {
		rinoohttp_header_set(&http->response.headers, "Server", RINOO_HTTP_SIGNATURE);
	}
}

/**
 * Sends a http response.
 *
 * @param http Pointer to a http structure
 * @param body Pointer to http response body, if any
 *
 * @return 0 on success, otherwise -1
 */
int rinoohttp_response_send(t_rinoohttp *http, t_buffer *body)
{
	ssize_t ret;
	t_rinoorbtree_node *cur_node;
	t_rinoohttp_header *cur_header;

	XASSERT(http != NULL, -1);
	XASSERT(buffer_size(http->response.buffer) == 0, -1);

	if (body != NULL) {
		http->response.content_length = buffer_size(body);
	}
	rinoohttp_response_setdefaultheaders(http);
	rinoohttp_response_setdefaultmsg(http);
	switch (http->version) {
	case RINOO_HTTP_VERSION_10:
		buffer_add(http->response.buffer, "HTTP/1.0", 8);
		break;
	default:
		buffer_add(http->response.buffer, "HTTP/1.1", 8);
		break;
	}
	buffer_print(http->response.buffer, " %d %.*s\r\n", http->response.code, buffer_size(&http->response.msg), buffer_ptr(&http->response.msg));
	for (cur_node = rinoorbtree_head(&http->response.headers);
	     cur_node != NULL;
	     cur_node = rinoorbtree_next(cur_node)) {
		cur_header = container_of(cur_node, t_rinoohttp_header, node);
		buffer_print(http->response.buffer,
			     "%.*s: %.*s\r\n",
			     buffer_size(&cur_header->key),
			     buffer_ptr(&cur_header->key),
			     buffer_size(&cur_header->value),
			     buffer_ptr(&cur_header->value));
	}
	buffer_add(http->response.buffer, "\r\n", 2);
	if (body != NULL && buffer_size(http->response.buffer) > buffer_size(body)) {
		buffer_add(http->response.buffer, buffer_ptr(body), buffer_size(body));
		ret = rinoo_socket_writeb(http->socket, http->response.buffer);
		if (ret != (ssize_t) buffer_size(http->response.buffer)) {
			return -1;
		}
	} else {
		ret = rinoo_socket_writeb(http->socket, http->response.buffer);
		if (ret != (ssize_t) buffer_size(http->response.buffer)) {
			return -1;
		}
		if (body != NULL && rinoo_socket_writeb(http->socket, body) != (ssize_t) buffer_size(body)) {
			return -1;
		}
	}
	return 0;
}
