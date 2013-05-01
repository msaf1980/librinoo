/**
 * @file   http_easy.h
 * @author reginaldl <reginald.l@gmail.com> - Copyright 2013
 * @date   Tue Apr 30 10:14:57 2013
 *
 * @brief  Easy HTTP interface
 *
 *
 */

#ifndef RINOO_PROTO_HTTP_EASY_H_
#define RINOO_PROTO_HTTP_EASY_H_

#define RINOO_HTTP_ERROR_500	"<div style=\"display: inline-block; border-radius: 4px; border: 1px solid red; width: 16px; height: 16px; color: red; font-size: 14px; text-align: center;\">&#10060;</div> <span style=\"font-family: Arial;\">500 - Internal server error</span>"
#define RINOO_HTTP_ERROR_404	"<div style=\"display: inline-block; border-radius: 4px; border: 1px solid orange; width: 16px; height: 16px; color: orange; font-size: 14px; text-align: center;\">?</div> <span style=\"font-family: Arial;\">404 - Not found</span>"

typedef enum e_rinoohttp_route_type {
	RINOO_HTTP_ROUTE_STATIC = 0,
	RINOO_HTTP_ROUTE_FUNC,
	RINOO_HTTP_ROUTE_FILE,
	RINOO_HTTP_ROUTE_REDIRECT,
} t_rinoohttp_route_type;

typedef struct s_rinoohttp_route {
	const char *uri;
	int code;
	t_rinoohttp_route_type type;
	union {
		const char *file;
		const char *content;
		const char *location;
		int (*func)(t_rinoohttp *http, struct s_rinoohttp_route *route);
	};
} t_rinoohttp_route;

typedef struct s_rinoohttp_easy_context {
	int nbroutes;
	t_rinoosocket *socket;
	t_rinoohttp_route *routes;
} t_rinoohttp_easy_context;

int rinoohttp_easy_server(t_rinoosched *sched, t_ip ip, uint16_t port, t_rinoohttp_route *routes, int size);

#endif /* !RINOO_PROTO_HTTP_EASY_H_ */
