/**
 * @file   folding.c
 * @author reginaldl <reginald.l@gmail.com> - Copyright 2013
 * @date   Mon Apr 29 14:05:27 2013
 *
 * @brief  HTTP header folding uni test
 *
 *
 */

#include "rinoo/rinoo.h"

#define HEADER1		"testing\r\n\tfolding"
#define HEADER2		"testing\r\n\t\t\tfolding"
#define HEADER3		"testing\r\n folding"
#define HEADER4		"testing\r\n   folding"
#define HEADER5		"testing\r\nfolding"

void http_client_send(t_rinoohttp *http, const char *header_value, int expected_code)
{
	t_buffer *buffer;
	t_rinoohttp_header *header;

	rinoo_log("Sending: %s", header_value);
	buffer = buffer_create(NULL);
	buffer_print(buffer,
		     "GET / HTTP/1.1\r\n"
		     "Host: test\r\n"
		     "X-Test: %s\r\n"
		     "Content-Length: 0\r\n"
		     "\r\n", header_value);
	XTEST(rinoo_socket_writeb(http->socket, buffer) > 0);
	XTEST(rinoohttp_response_get(http) == 1);
	XTEST(http->response.code == expected_code);
	if (expected_code == 200) {
		header = rinoohttp_header_get(&http->response.headers, "X-Test");
		XTEST(header != NULL);
		XTEST(buffer_strcmp(&header->value, header_value) == 0);
	}
	rinoohttp_reset(http);
	buffer_destroy(buffer);
}

void http_client(void *sched)
{
	t_rinoohttp http;
	t_rinoosocket *client;

	client = rinoo_tcp_client(sched, 0, 4242, 0);
	XTEST(client != NULL);
	XTEST(rinoohttp_init(client, &http) == 0);
	http_client_send(&http, HEADER1, 200);
	http_client_send(&http, HEADER2, 200);
	http_client_send(&http, HEADER3, 200);
	http_client_send(&http, HEADER4, 200);
	http_client_send(&http, HEADER5, 400);
	rinoohttp_destroy(&http);
	rinoo_socket_destroy(client);
}

void http_server_recv(t_rinoohttp *http, const char *header_value)
{
	t_rinoohttp_header *header;

	rinoo_log("Receiving: %s", header_value);
	XTEST(rinoohttp_request_get(http) == 1);
	header = rinoohttp_header_get(&http->request.headers, "X-Test");
	XTEST(header != NULL);
	XTEST(buffer_strcmp(&header->value, header_value) == 0);
	http->response.code = 200;
	rinoohttp_header_set(&http->response.headers, "X-Test", header_value);
	XTEST(rinoohttp_response_send(http, NULL) == 0);
	rinoohttp_reset(http);
}

void http_server_process(void *socket)
{
	t_rinoohttp http;

	XTEST(rinoohttp_init(socket, &http) == 0);
	http_server_recv(&http, HEADER1);
	http_server_recv(&http, HEADER2);
	http_server_recv(&http, HEADER3);
	http_server_recv(&http, HEADER4);
	XTEST(rinoohttp_request_get(&http) == 0);
	rinoohttp_destroy(&http);
	rinoo_socket_destroy(socket);
}

void http_server(void *sched)
{
	t_ip ip;
	uint32_t port;
	t_rinoosocket *server;
	t_rinoosocket *client;

	server = rinoo_tcp_server(sched, 0, 4242);
	XTEST(server != NULL);
	client = rinoo_tcp_accept(server, &ip, &port);
	XTEST(client != NULL);
	rinoo_log("server - accepting client (%s:%d)", inet_ntoa(*(struct in_addr *) &ip), port);
	rinoo_task_start(sched, http_server_process, client);
	rinoo_socket_destroy(server);
}

/**
 * Main function for this unit test.
 *
 *
 * @return 0 if test passed
 */
int main()
{
	t_rinoosched *sched;

	sched = rinoo_sched();
	XTEST(sched != NULL);
	XTEST(rinoo_task_start(sched, http_server, sched) == 0);
	XTEST(rinoo_task_start(sched, http_client, sched) == 0);
	rinoo_sched_loop(sched);
	rinoo_sched_destroy(sched);
	XPASS();
}
