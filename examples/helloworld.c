#include "rinoo/rinoo.h"

void task_client(void *socket)
{
	char a;

	rn_socket_write(socket, "Hello world!\n", 13);
	rn_socket_read(socket, &a, 1);
	rn_socket_destroy(socket);
}

void task_server(void *sched)
{
	rn_addr_t addr;
	rn_socket_t *server;
	rn_socket_t *client;

	rn_addr4(&addr, "127.0.0.1", 4242);
	server = rn_tcp_server(sched, &addr);
	while ((client = rn_socket_accept(server, NULL)) != NULL) {
		rn_task_start(sched, task_client, client);
	}
	rn_socket_destroy(server);
}

int main()
{
	rn_sched_t *sched;

	sched = rn_scheduler();
	rn_task_start(sched, task_server, sched);
	rn_scheduler_loop(sched);
	rn_scheduler_destroy(sched);
	return 0;
}
