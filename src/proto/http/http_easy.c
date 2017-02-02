/**
 * @file   http_easy.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Easy HTTP interface
 *
 *
 */

#include "rinoo/proto/http/module.h"

/**
 * Calls a HTTP route.
 *
 * @param http Pointer to a HTTP context
 * @param route Pointer to the route to call
 */
static void rn_http_easy_route_call(rn_http_t *http, rn_http_route_t *route)
{
	rn_buffer_t body;
	rn_buffer_t *uri;

	http->response.code = route->code;
	switch (route->type) {
	case RINOO_HTTP_ROUTE_STATIC:
		rn_strtobuffer(&body, route->content);
		rn_http_response_send(http, &body);
		break;
	case RINOO_HTTP_ROUTE_FUNC:
		if (route->func(http, route) != 0) {
			http->response.code = 500;
			rn_strtobuffer(&body, RN_HTTP_ERROR_500);
			rn_http_response_send(http, &body);
		}
		break;
	case RINOO_HTTP_ROUTE_FILE:
		if (rn_http_send_file(http, route->file) != 0) {
			http->response.code = 404;
			rn_strtobuffer(&body, RN_HTTP_ERROR_404);
			rn_http_response_send(http, &body);
		}
		break;
	case RINOO_HTTP_ROUTE_DIR:
		uri = rn_buffer_create(NULL);
		rn_buffer_addstr(uri, route->path);
		rn_buffer_addstr(uri, "/");
		rn_buffer_add(uri, rn_buffer_ptr(&http->request.uri), rn_buffer_size(&http->request.uri));
		rn_buffer_addnull(uri);
		if (rn_http_send_file(http, rn_buffer_ptr(uri)) != 0) {
			http->response.code = 404;
			rn_strtobuffer(&body, RN_HTTP_ERROR_404);
			rn_http_response_send(http, &body);
		}
		rn_buffer_destroy(uri);
		break;
	case RINOO_HTTP_ROUTE_REDIRECT:
		rn_http_header_set(&http->response.headers, "Location", route->location);
		rn_http_response_send(http, NULL);
		break;
	}
}

/**
 * HTTP client processing callback
 *
 * @param context Pointer to a HTTP easy context
 */
static void rn_http_easy_client_process(void *context)
{
	int i;
	bool found;
	rn_buffer_t body;
	rn_http_t http;
	rn_http_easy_context_t *econtext = context;

	rn_http_init(econtext->socket, &http);
	while (rn_http_request_get(&http)) {
		for (i = 0, found = false; i < econtext->nbroutes && found == false; i++) {
			if (econtext->routes[i].uri == NULL ||
			rn_buffer_strcmp(&http.request.uri, econtext->routes[i].uri) == 0 ||
			(econtext->routes[i].type == RINOO_HTTP_ROUTE_DIR && rn_buffer_strncmp(&http.request.uri, econtext->routes[i].uri, strlen(econtext->routes[i].uri)) == 0)) {
				rn_http_easy_route_call(&http, &econtext->routes[i]);
				found = true;
			}
		}
		if (found == false) {
			http.response.code = 404;
			rn_strtobuffer(&body, RN_HTTP_ERROR_404);
			rn_http_response_send(&http, &body);
		}
		rn_http_reset(&http);
	}
	rn_http_destroy(&http);
	rn_socket_destroy(econtext->socket);
	free(econtext);
}

/**
 * HTTP server processing callback
 *
 * @param context Pointer to a HTTP easy context
 */
static void rn_http_easy_server_process(void *context)
{
	rn_socket_t *client;
	rn_http_easy_context_t *c_context;
	rn_http_easy_context_t *s_context = context;

	while ((client = rn_tcp_accept(s_context->socket, NULL, NULL)) != NULL) {
		c_context = malloc(sizeof(*c_context));
		if (c_context == NULL) {
			rn_socket_destroy(client);
			rn_socket_destroy(s_context->socket);
			free(s_context);
			return;
		}
		c_context->socket = client;
		c_context->routes = s_context->routes;
		c_context->nbroutes = s_context->nbroutes;
		rn_task_start(s_context->socket->node.sched, rn_http_easy_client_process, c_context);
	}
	rn_socket_destroy(s_context->socket);
	free(s_context);
}

/**
 * Starts a HTTP server which serves the given HTTP easy routes.
 *
 * @param sched Pointer to a scheduler
 * @param ip Ip address to bind
 * @param port Port to bind
 * @param routes Array of HTTP easy route to server
 * @param size Number of routes
 *
 * @return 0 on success, or -1 if an error occurs
 */
int rn_http_easy_server(rn_sched_t *sched, rn_ip_t *ip, uint16_t port, rn_http_route_t *routes, int size)
{
	rn_socket_t *server;
	rn_http_easy_context_t *context;

	if (routes == NULL) {
		return -1;
	}
	server = rn_tcp_server(sched, ip, port);
	if (server == NULL) {
		return -1;
	}
	context = malloc(sizeof(*context));
	if (context == NULL) {
		return -1;
	}
	context->socket = server;
	context->routes = routes;
	context->nbroutes = size;
	if (rn_task_start(sched, rn_http_easy_server_process, context) != 0) {
		free(context);
		return -1;
	}
	return 0;
}
