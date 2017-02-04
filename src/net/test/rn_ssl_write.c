/**
 * @file   rinoo_ssl_write.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Test file for rinoossl function
 *
 *
 */

#include	"rinoo/rinoo.h"

rn_sched_t *sched;

void process_client(void *arg)
{
	char b;
	rn_socket_t *socket = arg;

	rn_log("server - client accepted");
	rn_log("server - sending 'abcdef'");
	XTEST(rn_socket_write(socket, "abcdef", 6) == 6);
	rn_log("server - receiving 'b'");
	XTEST(rn_socket_read(socket, &b, 1) == 1);
	XTEST(b == 'b');
	rn_log("server - receiving nothing");
	XTEST(rn_socket_read(socket, &b, 1) == -1);
	rn_socket_destroy(socket);
}

void server_func(void *arg)
{
	char buf[42];
	rn_addr_t addr;
	rn_socket_t *client;
	rn_socket_t *server;
	rn_ssl_ctx_t *ctx = arg;

	rn_addr4(&addr, "127.0.0.1", 4242);
	server = rn_ssl_server(sched, ctx, &addr);
	rn_log("server listening...");
	client = rn_socket_accept(server, &addr);
	XTEST(client != NULL);
	rn_log("server - accepting client (%s:%d)", rn_addr_getip(&addr, buf, sizeof(buf)), rn_addr_getport(&addr));
	rn_task_start(sched, process_client, client);
	rn_socket_destroy(server);
}

void client_func(void *arg)
{
	char a;
	char cur;
	rn_addr_t addr;
	rn_socket_t *client;
	rn_ssl_ctx_t *ctx = arg;

	rn_log("client - connecting...");
	rn_addr4(&addr, "127.0.0.1", 4242);
	client = rn_ssl_client(sched, ctx, &addr, 0);
	XTEST(client != NULL);
	rn_log("client - connected");
	for (cur = 'a'; cur <= 'f'; cur++) {
		rn_log("client - receiving '%c'", cur);
		XTEST(rn_socket_read(client, &a, 1) == 1);
		XTEST(a == cur);
	}
	rn_log("client - sending 'b'");
	XTEST(rn_socket_write(client, "b", 1) == 1);
	rn_socket_destroy(client);
}


/**
 * Main function for this unit test.
 *
 * @return 0 if test passed
 */
int main()
{
	rn_ssl_ctx_t *ssl;

	sched = rn_scheduler();
	XTEST(sched != NULL);
	ssl = rn_ssl_context();
	XTEST(ssl != NULL);
	rn_task_start(sched, server_func, ssl);
	rn_task_start(sched, client_func, ssl);
	rn_scheduler_loop(sched);
	rn_ssl_context_destroy(ssl);
	rn_scheduler_destroy(sched);
	XPASS();
}
