/**
 * @file   folding.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
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

void http_client_send(rn_http_t *http, const char *header_value, int expected_code)
{
	rn_buffer_t *buffer;
	rn_http_header_t *header;

	rn_log("Sending: %s", header_value);
	buffer = rn_buffer_create(NULL);
	rn_buffer_print(buffer,
		     "GET / HTTP/1.1\r\n"
		     "Host: test\r\n"
		     "X-Test: %s\r\n"
		     "Content-Length: 0\r\n"
		     "\r\n", header_value);
	XTEST(rn_socket_writeb(http->socket, buffer) > 0);
	XTEST(rn_http_response_get(http));
	XTEST(http->response.code == expected_code);
	if (expected_code == 200) {
		header = rn_http_header_get(&http->response.headers, "X-Test");
		XTEST(header != NULL);
		XTEST(rn_buffer_strcmp(&header->value, header_value) == 0);
	}
	rn_http_reset(http);
	rn_buffer_destroy(buffer);
}

void http_client(void *sched)
{
	rn_http_t http;
	rn_socket_t *client;

	client = rn_tcp_client(sched, IP_LOOPBACK, 4242, 0);
	XTEST(client != NULL);
	XTEST(rn_http_init(client, &http) == 0);
	http_client_send(&http, HEADER1, 200);
	http_client_send(&http, HEADER2, 200);
	http_client_send(&http, HEADER3, 200);
	http_client_send(&http, HEADER4, 200);
	http_client_send(&http, HEADER5, 400);
	rn_http_destroy(&http);
	rn_socket_destroy(client);
}

void http_server_recv(rn_http_t *http, const char *header_value)
{
	rn_http_header_t *header;

	rn_log("Receiving: %s", header_value);
	XTEST(rn_http_request_get(http));
	header = rn_http_header_get(&http->request.headers, "X-Test");
	XTEST(header != NULL);
	XTEST(rn_buffer_strcmp(&header->value, header_value) == 0);
	http->response.code = 200;
	rn_http_header_set(&http->response.headers, "X-Test", header_value);
	XTEST(rn_http_response_send(http, NULL) == 0);
	rn_http_reset(http);
}

void http_server_process(void *socket)
{
	rn_http_t http;

	XTEST(rn_http_init(socket, &http) == 0);
	http_server_recv(&http, HEADER1);
	http_server_recv(&http, HEADER2);
	http_server_recv(&http, HEADER3);
	http_server_recv(&http, HEADER4);
	XTEST(rn_http_request_get(&http) == false);
	rn_http_destroy(&http);
	rn_socket_destroy(socket);
}

void http_server(void *sched)
{
	rn_ip_t ip;
	uint16_t port;
	rn_socket_t *server;
	rn_socket_t *client;

	server = rn_tcp_server(sched, IP_ANY, 4242);
	XTEST(server != NULL);
	client = rn_tcp_accept(server, &ip, &port);
	XTEST(client != NULL);
	rn_log("server - accepting client (%s:%d)", inet_ntoa(*(struct in_addr *) &ip), port);
	rn_task_start(sched, http_server_process, client);
	rn_socket_destroy(server);
}

/**
 * Main function for this unit test.
 *
 *
 * @return 0 if test passed
 */
int main()
{
	rn_sched_t *sched;

	sched = rn_scheduler();
	XTEST(sched != NULL);
	XTEST(rn_task_start(sched, http_server, sched) == 0);
	XTEST(rn_task_start(sched, http_client, sched) == 0);
	rn_scheduler_loop(sched);
	rn_scheduler_destroy(sched);
	XPASS();
}
