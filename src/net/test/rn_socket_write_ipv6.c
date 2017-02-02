/**
 * @file rinoo_socket_write_ipv6.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2014
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief Test file for read/write functions.
 *
 *
 */
#include "rinoo/rinoo.h"

extern const rn_socket_class_t socket_class_tcp6;

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
	rn_socket_t *server;
	rn_socket_t *client;
	struct sockaddr_in6 addr = { 0 };
	rn_sched_t *sched = arg;

	server = rn_socket(sched, &socket_class_tcp6);
	XTEST(server != NULL);
	addr.sin6_port = htons(4242);
	addr.sin6_family = AF_INET6;
	addr.sin6_addr = in6addr_any;
	XTEST(rn_socket_bind(server, (struct sockaddr *) &addr, sizeof(addr), 42) == 0);
	rn_log("server listening...");
	client = rn_socket_accept(server, NULL, NULL);
	XTEST(client != NULL);
	rn_task_start(sched, process_client, client);
	rn_socket_destroy(server);
}

void client_func(void *arg)
{
	char a;
	char cur;
	struct sockaddr_in6 addr = { 0 };
	rn_socket_t *socket;
	rn_sched_t *sched = arg;

	socket = rn_socket(sched, &socket_class_tcp6);
	XTEST(socket != NULL);
	addr.sin6_port = htons(4242);
	addr.sin6_family = AF_INET6;
	addr.sin6_addr = in6addr_loopback;
	XTEST(rn_socket_connect(socket, (struct sockaddr *) &addr, sizeof(addr)) == 0);
	rn_log("client - connected");
	for (cur = 'a'; cur <= 'f'; cur++) {
		rn_log("client - receiving '%c'", cur);
		XTEST(rn_socket_read(socket, &a, 1) == 1);
		XTEST(a == cur);
	}
	rn_log("client - sending 'b'");
	XTEST(rn_socket_write(socket, "b", 1) == 1);
	rn_socket_destroy(socket);
}

/**
 * Main function for this unit test.
 *
 * @return 0 if test passed
 */
int main()
{
	rn_sched_t *sched;

	sched = rn_scheduler();
	XTEST(sched != NULL);
	XTEST(rn_task_start(sched, server_func, sched) == 0);
	XTEST(rn_task_start(sched, client_func, sched) == 0);
	rn_scheduler_loop(sched);
	rn_scheduler_destroy(sched);
	XPASS();
}
