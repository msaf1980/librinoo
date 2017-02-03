/**
 * @file   http_easy.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Easy HTTP interface
 *
 *
 */

#ifndef RINOO_PROTO_HTTP_EASY_H_
#define RINOO_PROTO_HTTP_EASY_H_

#define RN_HTTP_ERROR_500	"<div style=\"display: inline-block; border-radius: 4px; border: 1px solid red; width: 16px; height: 16px; color: red; font-size: 14px; text-align: center;\">&#10060;</div> <span style=\"font-family: Arial;\">500 - Internal server error</span>"
#define RN_HTTP_ERROR_404	"<div style=\"display: inline-block; border-radius: 4px; border: 1px solid orange; width: 16px; height: 16px; color: orange; font-size: 14px; text-align: center;\">?</div> <span style=\"font-family: Arial;\">404 - Not found</span>"

typedef enum rn_http_route_type_e {
	RN_HTTP_ROUTE_STATIC = 0,
	RN_HTTP_ROUTE_FUNC,
	RN_HTTP_ROUTE_FILE,
	RN_HTTP_ROUTE_DIR,
	RN_HTTP_ROUTE_REDIRECT,
} rn_http_route_type_t;

typedef struct rn_http_route_s {
	const char *uri;
	int code;
	rn_http_route_type_t type;
	union {
		const char *file;
		const char *path;
		const char *content;
		const char *location;
		int (*func)(rn_http_t *http, struct rn_http_route_s *route);
	};
} rn_http_route_t;

typedef struct rn_http_easy_context_s {
	int nbroutes;
	rn_socket_t *socket;
	rn_http_route_t *routes;
} rn_http_easy_context_t;

int rn_http_easy_server(rn_sched_t *sched, rn_addr_t *dst, rn_http_route_t *routes, int size);

#endif /* !RINOO_PROTO_HTTP_EASY_H_ */
