# RiNOO
[![Build Status](https://drone.io/github.com/reginaldl/librinoo/status.png)](https://drone.io/github.com/reginaldl/librinoo/latest)
[![Coverity](https://scan.coverity.com/projects/2835/badge.svg)](https://scan.coverity.com/projects/2835)

RiNOO is a socket management library. RiNOO sockets are asynchronous but "appear" synchronous.
This is possible by using fast-contexts (see fcontext project). Code looks simple. The complexity
of asynchronous sockets is hidden.
RiNOO is a simple way to create high scalability client/server applications.

## Documentation

* [Using librinoo for fun and profit](https://github.com/reginaldl/librinoo/wiki/Using-librinoo-for-fun-and-profit)
* [Libevent vs. RiNOO](https://github.com/reginaldl/librinoo/wiki/Libevent-vs.-RiNOO)

## Examples

### Hello world!

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
    	rn_socket_t *server;
    	rn_socket_t *client;

    	server = rn_tcp_server(sched, IP_ANY, 4242);
    	while ((client = rn_tcp_accept(server, NULL, NULL)) != NULL) {
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

### Multi-threading

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
    	while ((client = rn_tcp_accept(server, NULL, NULL)) != NULL) {
                rn_log("Accepted connection on thread %d", sched->id);
                rn_task_start(sched, task_client, client);
    	}
    	rn_socket_destroy(server);
    }

    int main()
    {
        int i;
    	rn_sched_t *spawn;
    	rn_sched_t *sched;
    	rn_socket_t *server;

    	sched = rn_scheduler();
        /* Spawning 10 schedulers, each running in a separate thread */
        rn_spawn(sched, 10);
        for (i = 0; i <= 10; i++) {
                spawn = rn_spawn_get(sched, i);
                server = rn_tcp_server(spawn, IP_ANY, 4242);
                rn_task_start(spawn, task_server, server);
        }
    	rn_scheduler_loop(sched);
    	rn_scheduler_destroy(sched);
    	return 0;
    }

### HTTP

    #include "rinoo/rinoo.h"

    void http_client(void *sched)
    {
        rn_http_t http;
        rn_socket_t *client;

        client = rn_tcp_client(sched, IP_LOOPBACK, 80, 0);
        rn_http_init(client, &http);
        rn_http_request_send(&http, RINOO_HTTP_METHOD_GET, "/", NULL);
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

### HTTP easy server

    #include "rinoo/rinoo.h"

    rn_http_route_t routes[] = {
        { "/", 200, RINOO_HTTP_ROUTE_STATIC, .content = "<html><body><center>Welcome to RiNOO HTTP server!<br/><br/><a href=\"/motd\">motd</a></center><body></html>" },
        { "/motd", 200, RINOO_HTTP_ROUTE_FILE, .file = "/etc/motd" },
        { NULL, 302, RINOO_HTTP_ROUTE_REDIRECT, .location = "/" }
    };

    int main()
    {
        rn_sched_t *sched;

        sched = rn_scheduler();
        rn_http_easy_server(sched, 0, 4242, routes, sizeof(routes) / sizeof(*routes));
        rn_scheduler_loop(sched);
        rn_scheduler_destroy(sched);
        return 0;
    }
