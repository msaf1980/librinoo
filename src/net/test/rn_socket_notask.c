/**
 * @file rinoo_socket_notask.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief Test file for read/write functions without any task.
 *
 *
 */
#include "rinoo/rinoo.h"

extern const rn_socket_class_t socket_class_tcp;

void *server_thread(void *unused(arg))
{
	char b;
	rn_sched_t *sched;
	rn_socket_t *server;
	rn_socket_t *client;
	struct sockaddr_in addr;

	sched = rn_scheduler();
	XTEST(sched != NULL);
	server = rn_socket(sched, &socket_class_tcp);
	XTEST(server != NULL);
	addr.sin_port = htons(4242);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = 0;
	XTEST(rn_socket_bind(server, (struct sockaddr *) &addr, sizeof(addr), 42) == 0);
	rn_log("server listening...");
	client = rn_socket_accept(server, (struct sockaddr *) &addr, (socklen_t *)(int[]){(sizeof(struct sockaddr))});
	XTEST(client != NULL);
	rn_log("server - accepting client (%s:%d)", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	rn_log("server - sending 'abcdef'");
	XTEST(rn_socket_write(client, "abcdef", 6) == 6);
	rn_log("server - receiving 'b'");
	XTEST(rn_socket_read(client, &b, 1) == 1);
	XTEST(b == 'b');
	rn_log("server - receiving nothing");
	XTEST(rn_socket_read(client, &b, 1) == -1);
	rn_socket_destroy(client);
	rn_socket_destroy(server);
	rn_scheduler_destroy(sched);
	return NULL;
}

/**
 * Main function for this unit test.
 *
 * @return 0 if test passed
 */
int main()
{
	char a;
	char cur;
	pthread_t thread;
	rn_sched_t *sched;
	rn_socket_t *socket;
	struct sockaddr_in addr;

	pthread_create(&thread, NULL, server_thread, NULL);
	sleep(1);
	sched = rn_scheduler();
	XTEST(sched != NULL);
	socket = rn_socket(sched, &socket_class_tcp);
	XTEST(socket != NULL);
	addr.sin_port = htons(4242);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = 0;
	rn_log("client - connecting");
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
	rn_scheduler_destroy(sched);
	pthread_join(thread, NULL);
	XPASS();
}
