/**
 * @file rn_socket_writev.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2014
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief Test file for writev function.
 *
 *
 */
#include "rinoo/rinoo.h"

#define BUFFER_SIZE 2097152

extern const rn_socket_class_t socket_class_tcp;

static char *big_buffer;

void process_client(void *arg)
{
	int i;
	char b;
	rn_buffer_t *buffers[4];
	rn_buffer_t buffer[4];
	rn_socket_t *socket = arg;

	rn_buffer_static(&buffer[0], big_buffer, BUFFER_SIZE);
	buffers[0] = &buffer[0];
	rn_log("server - client accepted");
	rn_log("server - sending %d bytes", BUFFER_SIZE);
	XTEST(rn_socket_writev(socket, buffers, 1) == BUFFER_SIZE);
	rn_log("server - receiving 'b'");
	XTEST(rn_socket_read(socket, &b, 1) == 1);
	XTEST(b == 'b');
	for (i = 0; i < 4; i++) {
		rn_buffer_static(&buffer[i], big_buffer + (i * (BUFFER_SIZE / 4)), BUFFER_SIZE / 4);
		buffers[i] = &buffer[i];
	}
	rn_log("server - sending %d bytes", BUFFER_SIZE);
	XTEST(rn_socket_writev(socket, buffers, 4) == BUFFER_SIZE);
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
	rn_socket_t *server;
	rn_socket_t *client;
	rn_sched_t *sched = arg;

	server = rn_socket(sched, &socket_class_tcp);
	XTEST(server != NULL);
	rn_addr4(&addr, "127.0.0.1", 4242);
	XTEST(rn_socket_bind(server, &addr, 42) == 0);
	rn_log("server listening...");
	client = rn_socket_accept(server, &addr);
	XTEST(client != NULL);
	rn_log("server - accepting client (%s:%d)", rn_addr_getip(&addr, buf, sizeof(buf)), rn_addr_getport(&addr));
	rn_task_start(sched, process_client, client);
	rn_socket_destroy(server);
}

void client_func(void *arg)
{
	int i;
	char tmp[8];
	ssize_t res;
	ssize_t total;
	rn_addr_t addr;
	rn_socket_t *socket;
	rn_sched_t *sched = arg;

	socket = rn_socket(sched, &socket_class_tcp);
	XTEST(socket != NULL);
	rn_addr4(&addr, "127.0.0.1", 4242);
	XTEST(rn_socket_connect(socket, &addr) == 0);
	rn_log("client - connected");
	rn_log("client - reading %d bytes", BUFFER_SIZE);
	for (i = 0; i < BUFFER_SIZE / 8; i++) {
		res = 0;
		total = 0;
		while (total < 8 && (res = rn_socket_read(socket, tmp + total, 8 - total)) > 0) {
			total += res;
		}
		if (res < 0) {
			rn_log("Error: %s", strerror(rn_error));
		}
		XTEST(res > 0);
		XTEST(memcmp(tmp, "xxxxxxxx", 8) == 0);
	}
	rn_log("client - sending 'b'");
	rn_log("client - reading %d bytes", BUFFER_SIZE);
	XTEST(rn_socket_write(socket, "b", 1) == 1);
	for (i = 0; i < BUFFER_SIZE / 8; i++) {
		res = 0;
		total = 0;
		while (total < 8 && (res = rn_socket_read(socket, tmp + total, 8 - total)) > 0) {
			total += res;
		}
		if (res < 0) {
			rn_log("Error: %s", strerror(rn_error));
		}
		XTEST(res > 0);
		XTEST(memcmp(tmp, "xxxxxxxx", 8) == 0);
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

	big_buffer = malloc(sizeof(*big_buffer) * BUFFER_SIZE);
	XTEST(big_buffer != NULL);
	memset(big_buffer, 'x', sizeof(*big_buffer) * BUFFER_SIZE);
	sched = rn_scheduler();
	XTEST(sched != NULL);
	XTEST(rn_task_start(sched, server_func, sched) == 0);
	XTEST(rn_task_start(sched, client_func, sched) == 0);
	rn_scheduler_loop(sched);
	rn_scheduler_destroy(sched);
	free(big_buffer);
	XPASS();
}
