/**
 * @file   http_response.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file for HTTP response
 *
 *
 */

#ifndef RINOO_PROTO_HTTP_RESPONSE_H_
#define RINOO_PROTO_HTTP_RESPONSE_H_

/* Defined in http.h */
struct rn_http_s;

typedef struct rn_http_response_s {
	int code;
	rn_buffer_t msg;
	rn_buffer_t content;
	rn_buffer_t *buffer;
	rn_http_header_set_t headers;
} rn_http_response_t;

int rn_http_response_parse(struct rn_http_s *http);
bool rn_http_response_get(struct rn_http_s *http);
void rn_http_response_setmsg(struct rn_http_s *http, const char *msg);
void rn_http_response_setdefaultmsg(struct rn_http_s *http);
void rn_http_response_setdefaultheaders(struct rn_http_s *http);
int rn_http_response_prepare(struct rn_http_s *http, size_t body_length);
int rn_http_response_send(struct rn_http_s *http, rn_buffer_t *body);

#endif /* !RINOO_PROTO_HTTP_RESPONSE_H_ */
