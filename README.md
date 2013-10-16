# RiNOO

RiNOO is a socket management library. RiNOO sockets are asynchronous but "appear" synchronous.
This is possible by using fast-contexts (see fcontext project). Code looks simple. The complexity
of asynchronous sockets is hidden.
RiNOO is a simple way to create high scalability client/server applications.

## Documentation

* [Using librinoo for fun and profit](https://github.com/reginaldl/librinoo/wiki/Using-librinoo-for-fun-and-profit)
* [Libevent vs. RiNOO](https://github.com/reginaldl/librinoo/wiki/Libevent-vs.-RiNOO)

## Example


    #include "rinoo/rinoo.h"

    void task_client(void *socket)
    {
    	char a;

    	rinoo_socket_write(socket, "Hello world!\n", 13);
    	rinoo_socket_read(socket, &a, 1);
    	rinoo_socket_destroy(socket);
    }

    void task_server(void *sched)
    {
    	t_rinoosocket *server;
    	t_rinoosocket *client;

    	server = rinoo_tcp_server(sched, 0, 4242);
    	while ((client = rinoo_tcp_accept(server, NULL, NULL)) != NULL) {
    		rinoo_task_start(sched, task_client, client);
    	}
    	rinoo_socket_destroy(server);
    }

    int main()
    {
    	t_rinoosched *sched;

    	sched = rinoo_sched();
    	rinoo_task_start(sched, task_server, sched);
    	rinoo_sched_loop(sched);
    	rinoo_sched_destroy(sched);
    	return 0;
    }

## Example with HTTP

    #include "rinoo/rinoo.h"

    void http_client(void *sched)
    {
        t_rinoohttp http;
        t_rinoosocket *client;

        client = rinoo_tcp_client(sched, 0, 80, 0);
        rinoohttp_init(client, &http);
        rinoohttp_request_send(&http, RINOO_HTTP_METHOD_GET, "/", NULL);
        rinoohttp_response_get(&http);
        rinoo_log("client - %.*s", buffer_size(http.response.buffer), buffer_ptr(http.response.buffer));
        rinoohttp_destroy(&http);
        rinoo_socket_destroy(client);
    }

    int main()
    {
        t_rinoosched *sched;

        sched = rinoo_sched();
        rinoo_task_start(sched, http_client, sched);
        rinoo_sched_loop(sched);
        rinoo_sched_destroy(sched);
        return 0;
    }

## Example with HTTP easy server

    #include "rinoo/rinoo.h"

    t_rinoohttp_route routes[] = {
        { "/", 200, RINOO_HTTP_ROUTE_STATIC, .content = "<html><body><center>Welcome to RiNOO HTTP server!<br/><br/><a href=\"/motd\">motd</a></center><body></html>" },
        { "/motd", 200, RINOO_HTTP_ROUTE_FILE, .file = "/etc/motd" },
        { NULL, 302, RINOO_HTTP_ROUTE_REDIRECT, .location = "/" }
    };

    int main()
    {
        t_rinoosched *sched;

        sched = rinoo_sched();
        rinoohttp_easy_server(sched, 0, 4242, routes, sizeof(routes) / sizeof(*routes));
        rinoo_sched_loop(sched);
        rinoo_sched_destroy(sched);
        return 0;
    }
