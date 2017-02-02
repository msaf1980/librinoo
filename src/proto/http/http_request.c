/**
 * @file   http_request.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  HTTP request functions
 *
 *
 */

#include "rinoo/proto/http/module.h"

/**
 * Reads a http request.
 *
 * @param http Pointer to a http structure
 *
 * @return 1 if a request has been correctly read, otherwise 0
 */
bool rn_http_request_get(rn_http_t *http)
{
	int ret;

	rn_http_reset(http);
	errno = 0;
	while (rn_socket_readb(http->socket, http->request.buffer) > 0) {
		ret = rn_http_request_parse(http);
		if (ret == 1) {
			while (rn_buffer_size(http->request.buffer) < http->request.headers_length + http->request.content_length) {
				if (rn_socket_readb(http->socket, http->request.buffer) <= 0) {
					return false;
				}
			}
			rn_buffer_static(&http->request.content, rn_buffer_ptr(http->request.buffer) + http->request.headers_length, http->request.content_length);
			return true;
		} else if (ret == -1) {
			http->response.code = 400;
			if (rn_http_response_send(http, NULL) != 0 && errno == ESHUTDOWN) {
				return false;
			}
			rn_http_reset(http);
		}
		errno = 0;
	}
	return false;
}

/**
 * Sets default http request headers.
 *
 * @param http Pointer to a http structure
 */
void rn_http_request_setdefaultheaders(rn_http_t *http)
{
	char tmp[24];

	if (rn_http_header_get(&http->request.headers, "Content-Length") == NULL) {
		if (snprintf(tmp, 24, "%lu", http->request.content_length) > 0) {
			rn_http_header_set(&http->request.headers, "Content-Length", tmp);
		}
	}
}

/**
 * Sends a http request.
 *
 * @param http Pointer to a http structure
 * @param method HTTP method to be used
 * @param uri URI to use
 * @param body Pointer to http request body, if any
 *
 * @return 0 on success, otherwise -1
 */
int rn_http_request_send(rn_http_t *http, rn_http_method_t method, const char *uri, rn_buffer_t *body)
{
	ssize_t ret;
	rn_rbtree_node_t *cur_node;
	rn_http_header_t *cur_header;

	XASSERT(http != NULL, -1);
	XASSERT(rn_buffer_size(http->request.buffer) == 0, -1);

	if (body != NULL) {
		http->request.content_length = rn_buffer_size(body);
	}
	http->request.method = method;
	switch (http->request.method) {
	case RINOO_HTTP_METHOD_OPTIONS:
		rn_buffer_add(http->request.buffer, "OPTIONS ", 8);
		break;
	case RINOO_HTTP_METHOD_GET:
		rn_buffer_add(http->request.buffer, "GET ", 4);
		break;
	case RINOO_HTTP_METHOD_HEAD:
		rn_buffer_add(http->request.buffer, "HEAD ", 5);
		break;
	case RINOO_HTTP_METHOD_POST:
		rn_buffer_add(http->request.buffer, "POST ", 5);
		break;
	case RINOO_HTTP_METHOD_PUT:
		rn_buffer_add(http->request.buffer, "PUT ", 4);
		break;
	case RINOO_HTTP_METHOD_DELETE:
		rn_buffer_add(http->request.buffer, "DELETE ", 7);
		break;
	case RINOO_HTTP_METHOD_TRACE:
		rn_buffer_add(http->request.buffer, "TRACE ", 6);
		break;
	case RINOO_HTTP_METHOD_CONNECT:
		rn_buffer_add(http->request.buffer, "CONNECT ", 8);
		break;
	case RINOO_HTTP_METHOD_UNKNOWN:
		return -1;
	}
	rn_buffer_set(&http->request.uri, uri);
	rn_buffer_add(http->request.buffer, rn_buffer_ptr(&http->request.uri), rn_buffer_size(&http->request.uri));
	rn_http_request_setdefaultheaders(http);
	switch (http->version) {
	case RINOO_HTTP_VERSION_10:
		rn_buffer_add(http->request.buffer, " HTTP/1.0\r\n", 11);
		break;
	default:
		rn_buffer_add(http->request.buffer, " HTTP/1.1\r\n", 11);
		break;
	}
	for (cur_node = rn_rbtree_head(&http->request.headers);
	     cur_node != NULL;
	     cur_node = rn_rbtree_next(cur_node)) {
		cur_header = container_of(cur_node, rn_http_header_t, node);
		rn_buffer_print(http->request.buffer,
			     "%.*s: %.*s\r\n",
			     rn_buffer_size(&cur_header->key),
			     rn_buffer_ptr(&cur_header->key),
			     rn_buffer_size(&cur_header->value),
			     rn_buffer_ptr(&cur_header->value));
	}
	rn_buffer_add(http->request.buffer, "\r\n", 2);
	ret = rn_socket_writeb(http->socket, http->request.buffer);
	if (ret != (ssize_t) rn_buffer_size(http->request.buffer)) {
		return -1;
	}
	if (body != NULL && rn_socket_writeb(http->socket, body) != (ssize_t) rn_buffer_size(body)) {
		return -1;
	}
	return 0;
}
