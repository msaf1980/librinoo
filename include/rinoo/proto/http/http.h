/**
 * @file   http.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file for http service
 *
 *
 */

#ifndef RINOO_PROTO_HTTP_H_
#define RINOO_PROTO_HTTP_H_

#define RN_HTTP_SIGNATURE	"RiNOO/" VERSION

typedef enum rn_http_version_e {
	RINOO_HTTP_VERSION_10 = 0,
	RINOO_HTTP_VERSION_11,
	RINOO_HTTP_VERSION_UNKNOWN
} rn_http_version_t;

typedef struct rn_http_s {
	rn_socket_t *socket;
	rn_http_version_t version;
	rn_http_request_t request;
	rn_http_response_t response;
} rn_http_t;

int rn_http_init(rn_socket_t *socket, rn_http_t *http);
void rn_http_destroy(rn_http_t *http);
void rn_http_reset(rn_http_t *http);

#endif /* !RINOO_PROTO_HTTP_H_ */
