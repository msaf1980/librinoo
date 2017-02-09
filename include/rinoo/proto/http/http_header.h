/**
 * @file   http_header.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file which describes HTTP header functions
 *
 *
 */

#ifndef RINOO_PROTO_HTTP_HEADER_H_
#define RINOO_PROTO_HTTP_HEADER_H_

typedef struct rn_http_header_set_s {
	size_t length;
	size_t content_length;
	rn_rbtree_t tree;
} rn_http_header_set_t;

typedef struct rn_http_header_s {
	rn_buffer_t key;
	rn_buffer_t value;
	rn_rbtree_node_t node;
} rn_http_header_t;


int rn_http_headers_init(rn_http_header_set_t *headers);
void rn_http_headers_flush(rn_http_header_set_t *headers);
int rn_http_header_setdata(rn_http_header_set_t *headers, const char *key, const char *value, uint32_t size);
int rn_http_header_set(rn_http_header_set_t *headers, const char *key, const char *value);
void rn_http_header_remove(rn_http_header_set_t *headers, const char *key);
rn_http_header_t *rn_http_header_get(rn_http_header_set_t *headers, const char *key);
rn_http_header_t *rn_http_header_first(rn_http_header_set_t *headers);
rn_http_header_t *rn_http_header_next(rn_http_header_t *header);

#endif /* !RINOO_PROTO_HTTP_HEADER_H_ */
