/**
 * @file   rinoo_socket_timeout.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Sun Jan  3 15:34:47 2010
 *
 * @brief  Test file for socket timeouts.
 *
 *
 */
#include	"rinoo/rinoo.h"

t_rinoosched *sched;

void process_client(t_rinoosocket *s)
{
	char b;

	rinoo_log("server - triggering timeout");
	rinoo_socket_timeout(s, 1000);
	XTEST(rinoo_socket_read(s, &b, 1) == -1);
	rinoo_sched_stop(sched);
	rinoo_socket_release(s);
}

void server_func(t_rinoosocket *s)
{
	t_ip fromip;
	u32 fromport;
	t_rinoosocket *new;

	XTEST(rinoo_tcp_listen(s, 0, 4242) == 0);
	new = rinoo_tcp_accept(s, process_client, &fromip, &fromport);
	rinoo_log("server - accepting client (%s:%d)",  inet_ntoa(*(struct in_addr *) &fromip), fromport);
	XTEST(new != NULL);
}

void client_func(t_rinoosocket *s)
{
	char a;

	XTEST(rinoo_tcp_connect(s, 0, 4242, 1000) == 0);
	rinoo_log("client - connected");
	rinoo_log("client - waiting timeout");
	XTEST(rinoo_socket_read(s, &a, 1) == -1);
	rinoo_log("client - %s", strerror(rinoo_socket_error_get(s)));
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
	XTEST(rinoo_socket_schedule(client, 1) == 0);
	rinoo_sched_loop(sched);
	rinoo_sched_destroy(sched);
	XPASS();
}
