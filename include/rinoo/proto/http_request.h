/**
 * @file   http_request.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Tue Apr 17 17:55:30 2012
 *
 * @brief  Header file for HTTP request
 *
 *
 */

#ifndef RINOO_PROTO_HTTP_REQUEST_H_
#define RINOO_PROTO_HTTP_REQUEST_H_

/* Defined in http.h */
struct s_rinoohttp;

typedef enum e_rinoohttp_method {
	RINOO_HTTP_METHOD_UNKNOWN = 0,
	RINOO_HTTP_METHOD_OPTIONS = 1,
	RINOO_HTTP_METHOD_GET = 2,
	RINOO_HTTP_METHOD_HEAD = 4,
	RINOO_HTTP_METHOD_POST = 8,
	RINOO_HTTP_METHOD_PUT = 16,
	RINOO_HTTP_METHOD_DELETE = 32,
	RINOO_HTTP_METHOD_TRACE = 64,
	RINOO_HTTP_METHOD_CONNECT = 128,
} t_rinoohttp_method;

typedef struct s_rinoohttp_request {
	t_buffer uri;
	size_t length;
	size_t content_length;
	t_buffer *buffer;
	t_rinoorbtree headers;
	t_rinoohttp_method method;
} t_rinoohttp_request;

int rinoohttp_request_parse(struct s_rinoohttp *http);
int rinoohttp_request_get(struct s_rinoohttp *http);
void rinoohttp_request_setdefaultheaders(struct s_rinoohttp *http);
int rinoohttp_request_send(struct s_rinoohttp *http, t_rinoohttp_method method, const char *uri, t_buffer *body);

#endif /* !RINOO_PROTO_HTTP_REQUEST_H_ */
