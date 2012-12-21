# RiNOO

RiNOO is a socket management libray. RiNOO sockets are asynchronous but "appear" synchronous.
This is possible by using fast-contexts (see fcontext project). Code looks simple. The complexity
of asynchronous sockets is hidden.
RiNOO is a simple way to create high scalability client/server applications.

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

