/**
 * @file   http.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
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
	http->request.buffer = buffer_create(1024);
	if (http->request.buffer == NULL) {
		return -1;
	}
	http->response.buffer = buffer_create(1024);
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
	buffer_erase(http->request.buffer, buffer_len(http->request.buffer));
	buffer_erase(http->response.buffer, buffer_len(http->response.buffer));
	rinoohttp_headers_flush(&http->request.headers);
	rinoohttp_headers_flush(&http->response.headers);
}

void rinoohttp_process_client(t_rinoosocket *s)
{
	t_rinoohttp http;
	char path[512];

	rinoohttp_init(s, &http);
	while (rinoohttp_request_get(&http) != 0) {
		snprintf(path, 512, "%.*s", (int) buffer_len(&http.request.uri), buffer_ptr(&http.request.uri));
		rinoohttp_header_set(&http.response.headers, "Connection", "Keep-alive");
		if (rinoohttp_send_file(&http, path) != 0) {
			switch (errno) {
			case EACCES:
				http.response.code = 401;
				break;
			default:
				http.response.code = 500;
				break;
			}
			rinoohttp_response_send(&http, NULL);
		}
	}
	rinoohttp_destroy(&http);
	rinoo_log("closing");
}

void rinoohttp_process_server(t_rinoosocket *s)
{
	t_ip fromip;
	u32 fromport;
	t_rinoosocket *new;

	rinoo_tcp_listen(s, 0, 4242);
	while ((new = rinoo_tcp_accept(s, rinoohttp_process_client, &fromip, &fromport)) != NULL) {
		rinoo_log("server - accepting client (%s:%d)", inet_ntoa(*(struct in_addr *) &fromip), fromport);
	}
}

int rinoohttp_server(t_rinoosched *sched)
{
	t_rinoosocket *server;

	server = rinoo_tcp(sched, rinoohttp_process_server);
	if (server == NULL) {
		return -1;
	}
	return rinoo_socket_schedule(server, 0);
}
