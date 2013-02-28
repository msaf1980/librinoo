/**
 * @file   http_request.c
 * @author Reginald Lips <reginald.l@gmail.com>
 * @date   Sun Apr 22 22:52:38 2012
 *
 * @brief  HTTP request functions
 *
 *
 */

#include "rinoo/rinoo.h"

/**
 * Reads a http request.
 *
 * @param http Pointer to a http structure
 *
 * @return 1 if a request has been correctly read, otherwise 0
 */
int rinoohttp_request_get(t_rinoohttp *http)
{
	int ret;

	rinoohttp_reset(http);
	while (rinoo_socket_readb(http->socket, http->request.buffer) > 0) {
		ret = rinoohttp_request_parse(http);
		if (ret == 1) {
			return 1;
		} else if (ret == -1) {
			http->response.code = 400;
			if (rinoohttp_response_send(http, NULL) != 0 && errno == ESHUTDOWN) {
				return 0;
			}
			rinoohttp_reset(http);
		}
	}
	return 0;
}

/**
 * Sets default http request headers.
 *
 * @param http Pointer to a http structure
 */
void rinoohttp_request_setdefaultheaders(t_rinoohttp *http)
{
	char tmp[24];

	if (rinoohttp_header_get(&http->request.headers, "Content-Length") == NULL) {
		if (snprintf(tmp, 24, "%lu", http->request.content_length) > 0) {
			rinoohttp_header_set(&http->request.headers, "Content-Length", tmp);
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
int rinoohttp_request_send(t_rinoohttp *http, t_rinoohttp_method method, const char *uri, t_buffer *body)
{
	ssize_t ret;
	t_rinoorbtree_node *cur_node;
	t_rinoohttp_header *cur_header;

	XASSERT(http != NULL, -1);
	XASSERT(buffer_size(http->request.buffer) == 0, -1);

	if (body != NULL) {
		http->request.content_length = buffer_size(body);
	}
	http->request.method = method;
	switch (http->request.method) {
	case RINOO_HTTP_METHOD_GET:
		buffer_add(http->request.buffer, "GET ", 4);
		break;
	case RINOO_HTTP_METHOD_POST:
		buffer_add(http->request.buffer, "POST ", 5);
		break;
	case RINOO_HTTP_METHOD_UNKNOWN:
		return -1;
	}
	strtobuffer(&http->request.uri, uri);
	buffer_add(http->request.buffer, buffer_ptr(&http->request.uri), buffer_size(&http->request.uri));
	rinoohttp_request_setdefaultheaders(http);
	switch (http->version) {
	case RINOO_HTTP_VERSION_10:
		buffer_add(http->request.buffer, " HTTP/1.0\r\n", 11);
		break;
	default:
		buffer_add(http->request.buffer, " HTTP/1.1\r\n", 11);
		break;
	}
	for (cur_node = rinoorbtree_head(&http->request.headers);
	     cur_node != NULL;
	     cur_node = rinoorbtree_next(cur_node)) {
		cur_header = container_of(cur_node, t_rinoohttp_header, node);
		buffer_print(http->request.buffer,
			     "%.*s: %.*s\r\n",
			     buffer_size(&cur_header->key),
			     buffer_ptr(&cur_header->key),
			     buffer_size(&cur_header->value),
			     buffer_ptr(&cur_header->value));
	}
	buffer_add(http->request.buffer, "\r\n", 2);
	ret = rinoo_socket_writeb(http->socket, http->request.buffer);
	if (ret != (ssize_t) buffer_size(http->request.buffer)) {
		return -1;
	}
	if (body != NULL && rinoo_socket_writeb(http->socket, body) != (ssize_t) buffer_size(body)) {
		return -1;
	}
	return 0;
}
