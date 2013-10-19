/**
 * @file   http_client.c
 * @author reginaldl <reginald.l@gmail.com>
 * @date   Tue Sep 24 22:27:06 2013
 *
 * @brief  Example of a multi-threaded HTTP client.
 *	   $ gcc ./examples/http_client.c -o rinoo_httpcli -I./include -L. -lrinoo_static -lpthread
 *
 */

#include "rinoo/rinoo.h"

uint16_t port;

/**
 * Opens 10 consecutive connections.
 * Each connection sends 10 HTTP requests and wait for response.
 *
 * @param sched Scheduler
 */
void process_http_client(void *sched)
{
	int i;
	int j;
	t_rinoohttp http;
	t_rinoosocket *socket;

	for (i = 0; i < 10; i++) {
		socket = rinoo_tcp_client(sched, 0, port, 0);
		if (socket == NULL) {
			rinoo_log("Error while creating socket %d: %s", port, strerror(errno));
			return;
		}
		rinoohttp_init(socket, &http);
		for (j = 0; j < 10; j++) {
			if (rinoohttp_request_send(&http, RINOO_HTTP_METHOD_GET, "/", NULL) != 0) {
				goto client_error;
			}
			if (!rinoohttp_response_get(&http)) {
				goto client_error;
			}
			rinoohttp_reset(&http);
		}
		rinoohttp_destroy(&http);
		rinoo_socket_destroy(socket);
	}
	return;
client_error:
	rinoohttp_destroy(&http);
	rinoo_socket_destroy(socket);
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
	int j;
	t_rinoosched *cur;
	t_rinoosched *sched;

	if (argc != 2) {
		printf("Usage: %s <port>\n", argv[0]);
		return -1;
	}
	port = atoi(argv[1]);
	sched = rinoo_sched();
	if (sched == NULL) {
		return -1;
	}
	if (rinoo_spawn(sched, 9) != 0) {
		rinoo_log("Could not spawn threads.");
		rinoo_sched_destroy(sched);
		return -1;
	}
	for (i = 0; i <= 9; i++) {
		cur = rinoo_spawn_get(sched, i);
		/* 100 parallel connections per thread */
		for (j = 0; j < 100; j++) {
			rinoo_task_start(cur, process_http_client, cur);
		}
	}
	rinoo_sched_loop(sched);
	rinoo_sched_destroy(sched);
	return 0;
}
