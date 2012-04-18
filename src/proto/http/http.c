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

void rinoohttp_process_client(t_rinoosocket *s)
{
	int ret;
	t_buffer *buffer;
	t_rinoohttp_request query;
	t_rinoohttp_response response;
	char path[512];

	buffer = buffer_create(1024);
	while (rinoo_socket_readb(s, buffer) > 0) {
		ret = rinoohttp_request_parse(&query, buffer);
		if (ret == 1) {
			snprintf(path, 512, "%.*s", (int) buffer_len(&query.uri), buffer_ptr(&query.uri));
			rinoohttp_response_init(s, &response);
			rinoohttp_send_file(&response, path);
			rinoohttp_response_destroy(&response);
			buffer_erase(buffer, buffer_len(buffer));
		} else if (ret == -1) {
			rinoo_socket_write(s, "HTTP/1.1 400 Bad Request\r\n\r\n", 28);
			buffer_erase(buffer, buffer_len(buffer));
		}
	}
	buffer_destroy(buffer);
}

void rinoohttp_process_server(t_rinoosocket *s)
{
	t_ip fromip;
	u32 fromport;
	t_rinoosocket *new;

	rinoo_tcp_listen(s, 0, 4242);
	while ((new = rinoo_tcp_accept(s, rinoohttp_process_client, &fromip, &fromport)) != NULL) {
		rinoo_log("server - accepting client (%s:%d)",
			  inet_ntoa(*(struct in_addr *) &fromip),
			  fromport);
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
