/**
 * @file   http.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Sun Apr 15 00:07:08 2012
 *
 * @brief
 *
 *
 */

#include "rinoo/rinoo.h"

int rinoohttp_init(t_rinoosocket *socket, t_rinoohttp *http)
{
	memset(http, 0, sizeof(*http));
	http->socket = socket;
	http->request.buffer = buffer_create(NULL);
	if (http->request.buffer == NULL) {
		return -1;
	}
	http->response.buffer = buffer_create(NULL);
	if (http->response.buffer == NULL) {
		buffer_destroy(http->request.buffer);
		return -1;
	}
	if (rinoohttp_headers_init(&http->request.headers) != 0) {
		buffer_destroy(http->request.buffer);
		buffer_destroy(http->response.buffer);
		return -1;
	}
	if (rinoohttp_headers_init(&http->response.headers) != 0) {
		buffer_destroy(http->request.buffer);
		buffer_destroy(http->response.buffer);
		return -1;
	}
	http->version = RINOO_HTTP_VERSION_11;
	return 0;
}

void rinoohttp_destroy(t_rinoohttp *http)
{
	if (http->request.buffer != NULL) {
		buffer_destroy(http->request.buffer);
		http->request.buffer = NULL;
	}
	if (http->response.buffer != NULL) {
		buffer_destroy(http->response.buffer);
		http->response.buffer = NULL;
	}
	rinoohttp_headers_flush(&http->request.headers);
	rinoohttp_headers_flush(&http->response.headers);
}

void rinoohttp_reset(t_rinoohttp *http)
{
	buffer_erase(http->request.buffer, buffer_size(http->request.buffer));
	buffer_erase(http->response.buffer, buffer_size(http->response.buffer));
	rinoohttp_headers_flush(&http->request.headers);
	rinoohttp_headers_flush(&http->response.headers);
}
