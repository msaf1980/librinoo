/**
 * @file   http_response.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Fri Nov 19 12:49:12 2010
 *
 * @brief  HTTP response functions
 *
 *
 */

#include	"rinoo/rinoo.h"

int rinoohttp_response_init(t_rinoosocket *socket, t_rinoohttp_response *response)
{
	memset(response, 0, sizeof(*response));
	response->socket = socket;
	return rinoohttp_headers_init(&response->headers);
}

void rinoohttp_response_destroy(t_rinoohttp_response *response)
{
	response->socket = NULL;
	rinoohttp_headers_flush(&response->headers);
}

/**
 * Reset all members of the HTTP response from a HTTP socket.
 * This function is useful for keep-alive connections.
 *
 * @param response Pointer to the HTTP response to use.
 */
void rinoohttp_response_reset(t_rinoohttp_response *response)
{
	t_rinoosocket *socket;

	socket = response->socket;
	rinoohttp_response_destroy(response);
	rinoohttp_response_init(socket, response);
}

/**
 * Set the HTTP message of a HTTP response.
 *
 * @param response Pointer to the HTTP response to use.
 * @param msg HTTP response message to use.
 */
void rinoohttp_response_setmsg(t_rinoohttp_response *response, const char *msg)
{
	strtobuffer(response->msg, msg);
}

/**
 * Set the default HTTP response message according to the response code.
 *
 * @param response Pointer to the HTTP response to use.
 */
void rinoohttp_response_setdefaultmsg(t_rinoohttp_response *response)
{
	switch (response->code)
	{
	case 100:
		rinoohttp_response_setmsg(response, "Continue");
		break;
	case 101:
		rinoohttp_response_setmsg(response, "Switching Protocols");
		break;
	case 200:
		rinoohttp_response_setmsg(response, "OK");
		break;
	case 201:
		rinoohttp_response_setmsg(response, "Created");
		break;
	case 202:
		rinoohttp_response_setmsg(response, "Accepted");
		break;
	case 203:
		rinoohttp_response_setmsg(response, "Non-Authoritative Information");
		break;
	case 204:
		rinoohttp_response_setmsg(response, "No Content");
		break;
	case 205:
		rinoohttp_response_setmsg(response, "Reset Content");
		break;
	case 206:
		rinoohttp_response_setmsg(response, "Partial Content");
		break;
	case 300:
		rinoohttp_response_setmsg(response, "Multiple Choices");
		break;
	case 301:
		rinoohttp_response_setmsg(response, "Moved Permanently");
		break;
	case 302:
		rinoohttp_response_setmsg(response, "Found");
		break;
	case 303:
		rinoohttp_response_setmsg(response, "See Other");
		break;
	case 304:
		rinoohttp_response_setmsg(response, "Not Modified");
		break;
	case 305:
		rinoohttp_response_setmsg(response, "Use Proxy");
		break;
	case 306:
		rinoohttp_response_setmsg(response, "(Unused)");
		break;
	case 307:
		rinoohttp_response_setmsg(response, "Temporary Redirect");
		break;
	case 400:
		rinoohttp_response_setmsg(response, "Bad Request");
		break;
	case 401:
		rinoohttp_response_setmsg(response, "Unauthorized");
		break;
	case 402:
		rinoohttp_response_setmsg(response, "Payment Required");
		break;
	case 403:
		rinoohttp_response_setmsg(response, "Forbidden");
		break;
	case 404:
		rinoohttp_response_setmsg(response, "Not Found");
		break;
	case 405:
		rinoohttp_response_setmsg(response, "Method Not Allowed");
		break;
	case 406:
		rinoohttp_response_setmsg(response, "Not Acceptable");
		break;
	case 407:
		rinoohttp_response_setmsg(response, "Proxy Authentication Required");
		break;
	case 408:
		rinoohttp_response_setmsg(response, "Request Timeout");
		break;
	case 409:
		rinoohttp_response_setmsg(response, "Conflict");
		break;
	case 410:
		rinoohttp_response_setmsg(response, "Gone");
		break;
	case 411:
		rinoohttp_response_setmsg(response, "Length Required");
		break;
	case 412:
		rinoohttp_response_setmsg(response, "Precondition Failed");
		break;
	case 413:
		rinoohttp_response_setmsg(response, "Request Entity Too Large");
		break;
	case 414:
		rinoohttp_response_setmsg(response, "Request-URI Too Long");
		break;
	case 415:
		rinoohttp_response_setmsg(response, "Unsupported Media Type");
		break;
	case 416:
		rinoohttp_response_setmsg(response, "Requested Range Not Satisfiable");
		break;
	case 417:
		rinoohttp_response_setmsg(response, "Expectation Failed");
		break;
	case 500:
		rinoohttp_response_setmsg(response, "Internal Server Error");
		break;
	case 501:
		rinoohttp_response_setmsg(response, "Not Implemented");
		break;
	case 502:
		rinoohttp_response_setmsg(response, "Bad Gateway");
		break;
	case 503:
		rinoohttp_response_setmsg(response, "Service Unavailable");
		break;
	case 504:
		rinoohttp_response_setmsg(response, "Gateway Timeout");
		break;
	case 505:
		rinoohttp_response_setmsg(response, "HTTP Version Not Supported");
		break;
	default:
		response->code = 500;
		rinoohttp_response_setmsg(response, "Internal Error");
		break;
	}
}

void rinoohttp_response_setdefaultheaders(t_rinoohttp_response *response)
{
	char tmp[24];

	if (rinoohttp_header_get(&response->headers, "Content-Length") == NULL) {
		if (snprintf(tmp, 24, "%lu", response->content_length) > 0) {
			rinoohttp_header_set(&response->headers, "Content-Length", tmp);
		}
	}
	if (rinoohttp_header_get(&response->headers, "Server") == NULL) {
		rinoohttp_header_set(&response->headers, "Server", RINOO_HTTP_SIGNATURE);
	}
}

int rinoohttp_response_send(t_rinoohttp_response *response, t_buffer *body)
{
	ssize_t ret;
	t_buffer *head;
	t_rinoorbtree_node *cur_node;
	t_rinoohttp_header *cur_header;

	response->content_length = buffer_len(body);
	rinoohttp_response_setdefaultheaders(response);
	rinoohttp_response_setdefaultmsg(response);
	head = buffer_create(1024);
	if (head == NULL) {
		return -1;
	}
	switch (response->version) {
	case RINOO_HTTP_VERSION_10:
		buffer_print(head, "HTTP/1.0");
		break;
	default:
		buffer_print(head, "HTTP/1.1");
		break;
	}
	buffer_print(head, " %d %.*s\r\n", response->code, buffer_len(&response->msg), buffer_ptr(&response->msg));
	for (cur_node = rinoorbtree_head(&response->headers);
	     cur_node != NULL;
	     cur_node = rinoorbtree_next(cur_node)) {
		cur_header = container_of(cur_node, t_rinoohttp_header, node);
		buffer_print(head,
			     "%.*s: %.*s\r\n",
			     buffer_len(&cur_header->key),
			     buffer_ptr(&cur_header->key),
			     buffer_len(&cur_header->value),
			     buffer_ptr(&cur_header->value));
	}
	buffer_print(head, "\r\n");
	ret = rinoo_socket_writeb(response->socket, head);
	if (ret == (ssize_t) buffer_len(head)) {
		ret = rinoo_socket_writeb(response->socket, body);
	}
	buffer_destroy(head);
	return (ret == (ssize_t) buffer_len(body) ? 0 : -1);
}
