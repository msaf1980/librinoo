/**
 * @file   rinoo_socket_write.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Sun Jan  3 15:34:47 2010
 *
 * @brief  Test file for read/write functions.
 *
 *
 */
#include "rinoo/rinoo.h"

t_rinoosched *sched;

void process_client(t_rinoosocket *s)
{
	char b;

	rinoo_log("server - client accepted");
	rinoo_log("server - sending 'abcdef'");
	XTEST(rinoo_socket_write(s, "abcdef", 6) == 6);
	rinoo_log("server - receiving 'b'");
	XTEST(rinoo_socket_read(s, &b, 1) == 1);
	XTEST(b == 'b');
	rinoo_log("server - receiving nothing");
	XTEST(rinoo_socket_read(s, &b, 1) == 0);
	rinoo_sched_stop(sched);
}

void server_func(t_rinoosocket *s)
{
	t_ip fromip;
	u32 fromport;
	t_rinoosocket *new;

	XTEST(rinoo_tcp_listen(s, 0, 4242) == 0);
	new = rinoo_tcp_accept(s, process_client, 1000, &fromip, &fromport);
	rinoo_log("server - accepting client (%s:%d)",
		  inet_ntoa(*(struct in_addr *) &fromip),
		  fromport);
	XTEST(new != NULL);
}

void client_func(t_rinoosocket *s)
{
	char a;
	char cur;

	if (rinoo_tcp_connect(s, 0, 4242, 1000) != 0) {
		perror("tcp_connect");
		XFAIL();
	}
	rinoo_log("client - connected");
	for (cur = 'a'; cur <= 'f'; cur++) {
		rinoo_log("client - receiving '%c'", cur);
		XTEST(rinoo_socket_read(s, &a, 1) == 1);
		XTEST(a == cur);
	}
	rinoo_log("client - sending 'b'");
	XTEST(rinoo_socket_write(s, "b", 1) == 1);
}

/**
 * This test will check if a scheduler is well initialized.
 *
 * @return 0 if test passed
 */
int main()
{
	t_rinoosocket *server;
	t_rinoosocket *client;

	sched = rinoo_sched();
	XTEST(sched != NULL);
	server = rinoo_tcp(sched, server_func);
	XTEST(server != NULL);
	client = rinoo_tcp(sched, client_func);
	XTEST(client != NULL);
	XTEST(rinoo_socket_schedule(server, 0) == 0);
	XTEST(rinoo_socket_schedule(client, 10) == 0);
	rinoo_sched_loop(sched);
	rinoo_sched_destroy(sched);
	XPASS();
}
