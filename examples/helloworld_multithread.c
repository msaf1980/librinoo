#include "rinoo/rinoo.h"

void task_client(void *socket)
{
	char a;

	rn_socket_write(socket, "Hello world!\n", 13);
	rn_socket_read(socket, &a, 1);
	rn_socket_destroy(socket);
}

void task_server(void *server)
{
	rn_sched_t *sched;
	rn_socket_t *client;

	sched = rn_scheduler_self();
	while ((client = rn_tcp_accept(server, NULL)) != NULL) {
		rn_log("Accepted connection on thread %d", sched->id);
		rn_task_start(sched, task_client, client);
	}
	rn_socket_destroy(server);
}

int main()
{
	int i;
	rn_addr_t addr;
	rn_sched_t *spawn;
	rn_sched_t *sched;
	rn_socket_t *server;

	sched = rn_scheduler();
	rn_addr4(&addr, "127.0.0.1", 4242);
	/* Spawning 10 schedulers, each running in a separate thread */
	rn_spawn(sched, 10);
	for (i = 0; i <= 10; i++) {
		spawn = rn_spawn_get(sched, i);
		server = rn_tcp_server(spawn, &addr);
		rn_task_start(spawn, task_server, server);
	}
	rn_scheduler_loop(sched);
	rn_scheduler_destroy(sched);
	return 0;
}
