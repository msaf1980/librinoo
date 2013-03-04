/**
 * @file   rinoo_stcp_write.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Thu Jun 21 14:52:08 2012
 *
 * @brief  Test file for rinoossl function
 *
 *
 */

#include	"rinoo/rinoo.h"

t_rinoosched *sched;

void process_client(void *arg)
{
	char b;
	t_rinoossl *ssl = arg;

	rinoo_log("server - client accepted");
	rinoo_log("server - sending 'abcdef'");
	XTEST(rinoo_ssl_write(ssl, "abcdef", 6) == 6);
	rinoo_log("server - receiving 'b'");
	XTEST(rinoo_ssl_read(ssl, &b, 1) == 1);
	XTEST(b == 'b');
	rinoo_log("server - receiving nothing");
	XTEST(rinoo_ssl_read(ssl, &b, 1) == 0);
	rinoo_ssl_destroy(ssl);
}

void server_func(void *arg)
{
	t_ip fromip;
	u32 fromport;
	t_rinoossl *client;
	t_rinoossl *server;
	t_rinoossl_ctx *ctx = arg;

	server = rinoo_ssl(sched, ctx);
	XTEST(rinoo_ssl_listen(server, 0, 4242) == 0);
	rinoo_log("server listening...");
	client = rinoo_ssl_accept(server, &fromip, &fromport);
	XTEST(client != NULL);
	rinoo_log("server - accepting client (%s:%d)", inet_ntoa(*(struct in_addr *) &fromip), fromport);
	rinoo_task_start(sched, process_client, client);
	rinoo_ssl_destroy(server);
}

void client_func(void *arg)
{
	char a;
	char cur;
	t_rinoossl *ssl;
	t_rinoossl_ctx *ctx = arg;

	ssl = rinoo_ssl(sched, ctx);
	XTEST(ssl != NULL);
	rinoo_log("client - connecting...");
	if (rinoo_ssl_connect(ssl, 0, 4242) != 0) {
		perror("tcp_connect");
		XFAIL();
	}
	rinoo_log("client - connected");
	for (cur = 'a'; cur <= 'f'; cur++) {
		rinoo_log("client - receiving '%c'", cur);
		XTEST(rinoo_ssl_read(ssl, &a, 1) == 1);
		XTEST(a == cur);
	}
	rinoo_log("client - sending 'b'");
	XTEST(rinoo_ssl_write(ssl, "b", 1) == 1);
	rinoo_ssl_destroy(ssl);
}


/**
 * Main function for this unit test.
 *
 * @return 0 if test passed
 */
int main()
{
	t_rinoossl_ctx *ssl;

	sched = rinoo_sched();
	XTEST(sched != NULL);
	ssl = rinoo_ssl_context();
	XTEST(ssl != NULL);
	rinoo_task_start(sched, server_func, ssl);
	rinoo_task_start(sched, client_func, ssl);
	rinoo_sched_loop(sched);
	rinoo_ssl_context_destroy(ssl);
	rinoo_sched_destroy(sched);
	XPASS();
}
