/**
 * @file   http_server.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Example of a multi-threaded HTTP server.
 *	   $ gcc ./examples/http_server.c -o rinoo_httpserv -I./include -L. -lrinoo_static -lpthread
 *
 */

#include "rinoo/rinoo.h"

rn_sched_t *master;

/**
 * Process HTTP requests for each client.
 *
 * @param arg Client socket
 */
void process_http_client(void *arg)
{
	rn_http_t http;
	rn_socket_t *client = arg;

	rn_http_init(client, &http);
	while (rn_http_request_get(&http)) {
		rn_log("Request uri=%.*s thread=%d", rn_buffer_size(&http.request.uri), rn_buffer_ptr(&http.request.uri), rn_scheduler_self()->id);
		http.response.code = 200;
		if (rn_http_response_send(&http, NULL) != 0) {
			goto error;
		}
		rn_http_reset(&http);
	}
	rn_http_destroy(&http);
	rn_socket_destroy(client);
	return;
error:
	rn_log("error");
	rn_http_destroy(&http);
	rn_socket_destroy(client);
}

/**
 * Accepts incoming connection on the HTTP server
 *
 * @param arg Server socket
 */
void process_http_server(void *arg)
{
	rn_socket_t *client;
	rn_socket_t *socket = arg;

	rn_log("Thread %d started.", rn_scheduler_self()->id);
	while ((client = rn_tcp_accept(socket, NULL)) != NULL) {
		rn_task_start(rn_scheduler_self(), process_http_client, client);
	}
	rn_socket_destroy(socket);
	rn_log("Thread %d stopped.", rn_scheduler_self()->id);
}

void signal_handler(int unused(signal))
{
	rn_log("Exiting...");
	rn_scheduler_stop(master);
}

/**
 * Main function
 *
 * @param argc Number of parameters
 * @param argv Parameters
 *
 * @return Should return 0
 */
int main(int argc, char **argv)
{
	int i;
	rn_addr_t addr;
	rn_sched_t *cur;
	rn_socket_t *server;

	if (argc != 2) {
		printf("Usage: %s <port>\n", argv[0]);
		return -1;
	}
	master = rn_scheduler();
	if (master == NULL) {
		return -1;
	}
	rn_addr4(&addr, "127.0.0.1", atoi(argv[1]));
	server = rn_tcp_server(master, &addr);
	if (server == NULL) {
		rn_log("Could not create server socket on port %s.", argv[1]);
		rn_scheduler_destroy(master);
		return -1;
	}
	if (sigaction(SIGINT, &(struct sigaction){ .sa_handler = signal_handler }, NULL) != 0) {
		rn_scheduler_destroy(master);
		return -1;
	}
	/* Spawning 10 threads */
	if (rn_spawn(master, 10) != 0) {
		rn_log("Could not spawn threads.");
		rn_scheduler_destroy(master);
		return -1;
	}
	for (i = 1; i <= 10; i++) {
		cur = rn_spawn_get(master, i);
		rn_task_start(cur, process_http_server, rn_socket_dup(cur, server));
	}
	rn_task_start(master, process_http_server, server);
	rn_scheduler_loop(master);
	rn_scheduler_destroy(master);
	return 0;
}
