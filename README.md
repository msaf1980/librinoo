<p align="center">
  <img src="https://gitlab.com/reginaldl/librinoo/raw/master/doc/rinoo.svg" alt="RiNOO Logo" width="50%"/>
</p>

# Overview

RiNOO is an asynchronous socket management library which leverages coroutines to make socket "appear" synchronous.
This is possible by using fast-contexts (see fcontext project or https://github.com/msaf1980/fcontext). Code looks simple. The complexity
of asynchronous sockets is hidden.
RiNOO is a simple way to create high scalability client/server applications.


# Build

Build with builtin fcontext
```
$ mkdir _build && cd _build
$ cmake ..
$ make
$ make install
```

Build with external fcontext (https://github.com/msaf1980/fcontext)
```
$ mkdir _build && cd _build
$ cmake -DRINOO_JUMP=boost ..
$ make
$ make install
```

# Test

This repository contains a number of tests. After building RiNOO, tests can be executed by running either `make test` or `ctest` (provided by CMake).
Each test is a standalone binary which will be executed twice:
 1. To validate the test
 2. To check memory leaks and overflows using `valgrind`

# Coroutines

Coroutines allow collaborative multitasking in a process. Each coroutine runs on its own stack and may decide to suspend its processing to allow another coroutine to run.
RiNOO provides a simple API to create and schedule coroutines, named *tasks*.

## Create tasks

```C
#include "rinoo/rinoo.h"

void task1(void *sched)
{
	printf("Task1 - Hello\n");
	rn_task_pause(sched);
	printf("Task1 - World\n");
}

void task2(void *sched)
{
	printf("Task2 - Hello\n");
	rn_task_pause(sched);
	printf("Task2 - World\n");
}

int main(int argc, char **argv)
{
	rn_sched_t *sched;

	sched = rn_scheduler();
	rn_task_start(sched, task1, sched);
	rn_task_start(sched, task2, sched);
	rn_scheduler_loop();
	rn_scheduler_destroy(sched);
	return 0;
}
```
The above example starts 2 tasks, each prints *Hello*, pauses and prints *World*:
```
$ ./a.out
Task1 - Hello
Task2 - Hello
Task1 - World
Task2 - World
```

# Coroutines combined with asynchronous sockets

## Hello world!

```C
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
```

## Multi-threading

```C
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
	while ((client = rn_socket_accept(server, NULL)) != NULL) {
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
```

## HTTP

```C
#include "rinoo/rinoo.h"

void http_client(void *sched)
{
    rn_addr_t addr;
    rn_http_t http;
    rn_socket_t *client;

    rn_addr4(&addr, "127.0.0.1", 4242);
    client = rn_tcp_client(sched, &addr, 0);
    rn_http_init(client, &http);
    rn_http_request_send(&http, RN_HTTP_METHOD_GET, "/", NULL);
    rn_http_response_get(&http);
    rn_log("client - %.*s", rn_buffer_size(http.response.buffer), rn_buffer_ptr(http.response.buffer));
    rn_http_destroy(&http);
    rn_socket_destroy(client);
}

int main()
{
    rn_sched_t *sched;

    sched = rn_scheduler();
    rn_task_start(sched, http_client, sched);
    rn_scheduler_loop(sched);
    rn_scheduler_destroy(sched);
    return 0;
}
```
