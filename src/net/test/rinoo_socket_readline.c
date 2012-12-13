/**
 * @file   rinoo_socket_readline.c
 * @author Reginald LIPS <reginald.@gmail.com> - Copyright 2012
 * @date   Wed Oct 24 23:54:16 2012
 *
 * @brief  Test file for socket readline.
 *
 *
 */

#include "rinoo/rinoo.h"

t_rinoosched *sched;

void process_client(t_rinoosocket *s)
{
	char b;
	char *str = "line1\nline2\nline3\n";

	rinoo_log("server - client accepted");
	rinoo_log("server - sending '%s'", str);
	XTEST(rinoo_socket_write(s, str, strlen(str)) == (ssize_t) strlen(str));
	rinoo_log("server - receiving 'b'");
	XTEST(rinoo_socket_read(s, &b, 1) == 1);
	XTEST(b == 'b');
	rinoo_log("server - receiving nothing");
	XTEST(rinoo_socket_read(s, &b, 1) == 0);
	rinoo_sched_stop(sched);
}

void client_func(t_rinoosocket *s)
{
	t_buffer *buffer;

	if (rinoo_tcp_connect(s, 0, 4242, 1000) != 0) {
		perror("tcp_connect");
		XFAIL();
	}
	buffer = buffer_create(NULL);
	rinoo_log("client - connected");
	XTEST(rinoo_socket_readline(s, buffer, "\n", 30)  == 6);
	buffer_erase(buffer, 6);
	XTEST(rinoo_socket_readline(s, buffer, "\n", 30)  == 6);
	buffer_erase(buffer, 6);
	XTEST(rinoo_socket_readline(s, buffer, "\n", 30)  == 6);
	buffer_destroy(buffer);
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
	server = rinoo_tcp_server(sched, 0, 4242, process_client);
	XTEST(server != NULL);
	client = rinoo_tcp(sched, client_func);
	XTEST(client != NULL);
	XTEST(rinoo_socket_start(client) == 0);
	rinoo_sched_loop(sched);
	rinoo_sched_destroy(sched);
	XPASS();
}
