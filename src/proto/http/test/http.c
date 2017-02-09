/**
 * @file   http.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  http client/server unit test
 *
 *
 */

#include "rinoo/rinoo.h"

#define HTTP_CONTENT	"This content is for test purpose\n"

void http_client(void *sched)
{
	rn_addr_t addr;
	rn_http_t http;
	rn_socket_t *client;

	rn_addr4(&addr, "127.0.0.1", 4242);
	client = rn_tcp_client(sched, &addr, 0);
	XTEST(client != NULL);
	XTEST(rn_http_init(client, &http) == 0);
	XTEST(rn_http_request_send(&http, RN_HTTP_METHOD_GET, "/", NULL) == 0);
	XTEST(rn_http_response_get(&http));
	XTEST(rn_buffer_size(&http.response.content) == strlen(HTTP_CONTENT));
	XTEST(http.response.code == 200);
	rn_http_destroy(&http);
	rn_socket_destroy(client);
}

void http_server_process(void *socket)
{
	rn_buffer_t content;
	rn_http_t http;

	XTEST(rn_http_init(socket, &http) == 0);
	XTEST(rn_http_request_get(&http));
	http.response.code = 200;
	rn_buffer_set(&content, HTTP_CONTENT);
	XTEST(rn_http_response_send(&http, &content) == 0);
	rn_http_destroy(&http);
	rn_socket_destroy(socket);
}

void http_server(void *sched)
{
	char buf[42];
	rn_addr_t addr;
	rn_socket_t *server;
	rn_socket_t *client;

	rn_addr4(&addr, "127.0.0.1", 4242);
	server = rn_tcp_server(sched, &addr);
	XTEST(server != NULL);
	client = rn_socket_accept(server, &addr);
	XTEST(client != NULL);
	rn_log("server - accepting client (%s:%d)", rn_addr_getip(&addr, buf, sizeof(buf)), rn_addr_getport(&addr));
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
