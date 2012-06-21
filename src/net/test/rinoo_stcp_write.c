/**
 * @file   rinoo_stcp_write.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Thu Jun 21 14:52:08 2012
 *
 * @brief  Test file for rinoossl function
 *
 *
 */

#include	"rinoo/rinoo.h"

t_rinoosched *sched;

void process_client(t_rinoossl *s)
{
	char b;

	rinoo_log("server - client accepted");
	rinoo_log("server - sending 'abcdef'");
	XTEST(rinoo_ssl_write(s, "abcdef", 6) == 6);
	rinoo_log("server - receiving 'b'");
	XTEST(rinoo_ssl_read(s, &b, 1) == 1);
	XTEST(b == 'b');
	rinoo_log("server - receiving nothing");
	XTEST(rinoo_ssl_read(s, &b, 1) == 0);
	rinoo_sched_stop(sched);
}

void server_func(t_rinoossl *s)
{
	t_ip fromip;
	u32 fromport;
	t_rinoossl *new;

	XTEST(rinoo_ssl_listen(s, 0, 4242) == 0);
	rinoo_log("server listening...");
	new = rinoo_ssl_accept(s, process_client, &fromip, &fromport);
	rinoo_log("server - accepting client (%s:%d)",
		  inet_ntoa(*(struct in_addr *) &fromip),
		  fromport);
	XTEST(new != NULL);
}

void client_func(t_rinoossl *s)
{
	char a;
	char cur;

	rinoo_log("client - connecting...");
	if (rinoo_ssl_connect(s, 0, 4242, 1000) != 0) {
		perror("tcp_connect");
		XFAIL();
	}
	rinoo_log("client - connected");
	for (cur = 'a'; cur <= 'f'; cur++) {
		rinoo_log("client - receiving '%c'", cur);
		XTEST(rinoo_ssl_read(s, &a, 1) == 1);
		XTEST(a == cur);
	}
	rinoo_log("client - sending 'b'");
	XTEST(rinoo_ssl_write(s, "b", 1) == 1);
}


/**
 * This test will check if a scheduler is well initialized.
 *
 * @return 0 if test passed
 */
int main()
{
	t_rinoossl_ctx *ssl;
	t_rinoossl *sslserver;
	t_rinoossl *sslclient;

	sched = rinoo_sched();
	XTEST(sched != NULL);
	ssl = rinoo_ssl_context();
	XTEST(ssl != NULL);
	sslserver = rinoo_ssl(sched, ssl, server_func);
	XTEST(sslserver != NULL);
	sslclient = rinoo_ssl(sched, ssl, client_func);
	XTEST(sslclient != NULL);
	XTEST(rinoo_socket_schedule(&sslserver->socket, 0) == 0);
	XTEST(rinoo_socket_schedule(&sslclient->socket, 1) == 0);
	rinoo_sched_loop(sched);
	rinoo_ssl_context_destroy(ssl);
	rinoo_sched_destroy(sched);
	XPASS();
}
