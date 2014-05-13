/**
 * @file rinoo_socket_wait.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2014
 * @date Sun Jan 3 15:34:47 2010
 *
 * @brief Test file for waitin/waitout functions.
 *
 *
 */
#include "rinoo/rinoo.h"

extern const t_rinoosocket_class socket_class_tcp;

void *server_thread(void *unused(arg))
{
	char a;
	t_rinoosched *sched;
	t_rinoosocket *server;
	t_rinoosocket *client;
	struct sockaddr_in addr;

	sched = rinoo_sched();
	XTEST(sched != NULL);
	server = rinoo_socket(sched, &socket_class_tcp);
	XTEST(server != NULL);
	addr.sin_port = htons(4242);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = 0;
	XTEST(rinoo_socket_bind(server, (struct sockaddr *) &addr, sizeof(addr), 42) == 0);
	rinoo_log("server listening...");
	client = rinoo_socket_accept(server, (struct sockaddr *) &addr, (socklen_t *)(int[]){(sizeof(struct sockaddr))});
	XTEST(client != NULL);
	rinoo_log("server - accepting client (%s:%d)", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	rinoo_log("server - sending 'a'");
	XTEST(rinoo_socket_write(client, "a", 1) == 1);
	XTEST(rinoo_socket_read(client, &a, 1) == 1);
	XTEST(a == 'b');
	rinoo_log("server - received '%c'", a);
	rinoo_log("server - waiting");
	sleep(1);
	rinoo_log("server - sending 'c'");
	XTEST(rinoo_socket_write(client, "c", 1) == 1);
	rinoo_socket_destroy(client);
	rinoo_socket_destroy(server);
	rinoo_sched_destroy(sched);
	return NULL;
}

void task_client(void *sched)
{
	char a;
	t_rinoosocket *socket;
	struct sockaddr_in addr;

	socket = rinoo_socket(sched, &socket_class_tcp);
	XTEST(socket != NULL);
	addr.sin_port = htons(4242);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = 0;
	rinoo_log("client - connecting");
	XTEST(rinoo_socket_connect(socket, (struct sockaddr *) &addr, sizeof(addr)) == 0);
	rinoo_log("client - connected");
	XTEST(rinoo_socket_read(socket, &a, 1) == 1);
	rinoo_log("client - received: '%c'", a);
	XTEST(a == 'a');
	rinoo_log("client - sending 'b'");
	XTEST(rinoo_socket_write(socket, "b", 1) == 1);
	rinoo_log("client - receiving");
	XTEST(rinoo_socket_read(socket, &a, 1) == 1);
	rinoo_log("client - received: '%c'", a);
	XTEST(a == 'c');
	rinoo_socket_destroy(socket);
}

/**
 * Main function for this unit test.
 *
 * @return 0 if test passed
 */
int main()
{
	pthread_t thread;
	t_rinoosched *sched;

	pthread_create(&thread, NULL, server_thread, NULL);
	sleep(1);
	sched = rinoo_sched();
	XTEST(sched != NULL);
	rinoo_task_start(sched, task_client, sched);
	rinoo_sched_loop(sched);
	rinoo_sched_destroy(sched);
	pthread_join(thread, NULL);
	XPASS();
}
