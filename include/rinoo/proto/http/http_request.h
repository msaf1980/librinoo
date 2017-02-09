/**
 * @file   http_request.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file for HTTP request
 *
 *
 */

#ifndef RINOO_PROTO_HTTP_REQUEST_H_
#define RINOO_PROTO_HTTP_REQUEST_H_

/* Defined in http.h */
struct rn_http_s;

typedef enum rn_http_method_e {
	RN_HTTP_METHOD_UNKNOWN = 0,
	RN_HTTP_METHOD_OPTIONS = 1,
	RN_HTTP_METHOD_GET = 2,
	RN_HTTP_METHOD_HEAD = 4,
	RN_HTTP_METHOD_POST = 8,
	RN_HTTP_METHOD_PUT = 16,
	RN_HTTP_METHOD_DELETE = 32,
	RN_HTTP_METHOD_TRACE = 64,
	RN_HTTP_METHOD_CONNECT = 128,
} rn_http_method_t;

typedef struct rn_http_request_s {
	rn_buffer_t uri;
	rn_buffer_t content;
	rn_buffer_t *buffer;
	rn_http_method_t method;
	rn_http_header_set_t headers;
} rn_http_request_t;

int rn_http_request_parse(struct rn_http_s *http);
bool rn_http_request_get(struct rn_http_s *http);
void rn_http_request_setdefaultheaders(struct rn_http_s *http);
int rn_http_request_send(struct rn_http_s *http, rn_http_method_t method, const char *uri, rn_buffer_t *body);

#endif /* !RINOO_PROTO_HTTP_REQUEST_H_ */
