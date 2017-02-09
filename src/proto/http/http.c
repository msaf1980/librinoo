/**
 * @file   http.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief
 *
 *
 */

#include "rinoo/proto/http/module.h"

int rn_http_init(rn_socket_t *socket, rn_http_t *http)
{
	memset(http, 0, sizeof(*http));
	http->socket = socket;
	http->request.buffer = rn_buffer_create(NULL);
	if (http->request.buffer == NULL) {
		return -1;
	}
	http->response.buffer = rn_buffer_create(NULL);
	if (http->response.buffer == NULL) {
		rn_buffer_destroy(http->request.buffer);
		return -1;
	}
	if (rn_http_headers_init(&http->request.headers) != 0) {
		rn_buffer_destroy(http->request.buffer);
		rn_buffer_destroy(http->response.buffer);
		return -1;
	}
	if (rn_http_headers_init(&http->response.headers) != 0) {
		rn_buffer_destroy(http->request.buffer);
		rn_buffer_destroy(http->response.buffer);
		return -1;
	}
	http->version = RN_HTTP_VERSION_11;
	return 0;
}

void rn_http_destroy(rn_http_t *http)
{
	if (http->request.buffer != NULL) {
		rn_buffer_destroy(http->request.buffer);
		http->request.buffer = NULL;
	}
	if (http->response.buffer != NULL) {
		rn_buffer_destroy(http->response.buffer);
		http->response.buffer = NULL;
	}
	rn_http_headers_flush(&http->request.headers);
	rn_http_headers_flush(&http->response.headers);
}

void rn_http_reset(rn_http_t *http)
{
	/* Reset request */
	memset(&http->request.uri, 0, sizeof(http->request.uri));
	http->request.method = RN_HTTP_METHOD_UNKNOWN;
	rn_buffer_reset(http->request.buffer);
	rn_http_headers_flush(&http->request.headers);
	/* Reset response */
	memset(&http->response.msg, 0, sizeof(http->response.msg));
	http->response.code = 0;
	rn_buffer_reset(http->response.buffer);
	rn_http_headers_flush(&http->response.headers);
}
