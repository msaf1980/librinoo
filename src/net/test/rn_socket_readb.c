/**
 * @file   rn_socket_readb.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Test file for rn_socket_readb.
 *
 *
 */

#include "rinoo/rinoo.h"

#define TRANSFER_SIZE	(1024 * 1000)

static char *str;

void process_client(void *socket)
{
	rn_buffer_t *buffer;

	buffer = rn_buffer_create(NULL);
	XTEST(buffer != NULL);
	while (rn_socket_readb(socket, buffer) > 0) {
		rn_log("receiving...");
	}
	XTEST(rn_buffer_size(buffer) == TRANSFER_SIZE);
	XTEST(rn_buffer_strncmp(buffer, str, TRANSFER_SIZE) == 0);
	rn_buffer_destroy(buffer);
	free(str);
	rn_socket_destroy(socket);
}

void server_func(void *unused(arg))
{
	rn_socket_t *server;
	rn_socket_t *client;

	server = rn_tcp_server(rn_scheduler_self(), IP_ANY, 4242);
	XTEST(server != NULL);
	client = rn_tcp_accept(server, NULL, NULL);
	XTEST(client != NULL);
	rn_log("client accepted");
	rn_task_start(rn_scheduler_self(), process_client, client);
	rn_socket_destroy(server);
}

void client_func(void *unused(arg))
{
	rn_buffer_t buffer;
	rn_socket_t *client;

	client = rn_tcp_client(rn_scheduler_self(), IP_LOOPBACK, 4242, 0);
	XTEST(client != NULL);
	str = malloc(sizeof(*str) * TRANSFER_SIZE);
	XTEST(str != NULL);
	memset(str, 'a', TRANSFER_SIZE);
	rn_buffer_static(&buffer, str, TRANSFER_SIZE);
	XTEST(rn_socket_writeb(client, &buffer) == TRANSFER_SIZE);
	rn_socket_destroy(client);
}

/**
 * Main function for this unit test.
 *
 *
 * @return 0 if test passed
 */
int main()
{
	rn_sched_t *sched;

	sched = rn_scheduler();
	XTEST(sched != NULL);
	XTEST(rn_task_start(sched, server_func, NULL) == 0);
	XTEST(rn_task_start(sched, client_func, NULL) == 0);
	rn_scheduler_loop(sched);
	rn_scheduler_destroy(sched);
	XPASS();
}
