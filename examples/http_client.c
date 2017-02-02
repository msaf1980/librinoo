/**
 * @file   http_client.c
 * @author Reginald Lips <reginald.l@gmail.com>
 * @date   Wed Feb  1 18:56:27 2017
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
	rn_http_t http;
	rn_socket_t *socket;

	for (i = 0; i < 10; i++) {
		socket = rn_tcp_client(sched, IP_LOOPBACK, port, 0);
		if (socket == NULL) {
			rn_log("Error while creating socket %d: %s", port, strerror(errno));
			return;
		}
		rn_http_init(socket, &http);
		for (j = 0; j < 10; j++) {
			if (rn_http_request_send(&http, RINOO_HTTP_METHOD_GET, "/", NULL) != 0) {
				goto client_error;
			}
			if (!rn_http_response_get(&http)) {
				goto client_error;
			}
			rn_http_reset(&http);
		}
		rn_http_destroy(&http);
		rn_socket_destroy(socket);
	}
	return;
client_error:
	rn_http_destroy(&http);
	rn_socket_destroy(socket);
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
	rn_sched_t *cur;
	rn_sched_t *sched;

	if (argc != 2) {
		printf("Usage: %s <port>\n", argv[0]);
		return -1;
	}
	port = atoi(argv[1]);
	sched = rn_scheduler();
	if (sched == NULL) {
		return -1;
	}
	if (rn_spawn(sched, 9) != 0) {
		rn_log("Could not spawn threads.");
		rn_scheduler_destroy(sched);
		return -1;
	}
	for (i = 0; i <= 9; i++) {
		cur = rn_spawn_get(sched, i);
		/* 100 parallel connections per thread */
		for (j = 0; j < 100; j++) {
			rn_task_start(cur, process_http_client, cur);
		}
	}
	rn_scheduler_loop(sched);
	rn_scheduler_destroy(sched);
	return 0;
}
