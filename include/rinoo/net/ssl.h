/**
 * @file   ssl.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file for Secure TCP socket function declaractions
 *
 *
 */

#ifndef RINOO_NET_SSL_H_
#define RINOO_NET_SSL_H_

typedef struct rn_ssl_ctx_s {
	X509 *x509;
	EVP_PKEY *pkey;
	SSL_CTX *ctx;
} rn_ssl_ctx_t;

typedef struct rn_ssl_s {
	SSL *ssl;
	rn_ssl_ctx_t *ctx;
	rn_socket_t socket;
} rn_ssl_t;

rn_ssl_ctx_t *rn_ssl_context(void);
void rn_ssl_context_destroy(rn_ssl_ctx_t *ctx);
rn_ssl_t *rn_ssl_get(rn_socket_t *socket);
rn_socket_t *rn_ssl_client(rn_sched_t *sched, rn_ssl_ctx_t *ctx, rn_ip_t *ip, uint32_t port, uint32_t timeout);
rn_socket_t *rn_ssl_server(rn_sched_t *sched, rn_ssl_ctx_t *ctx, rn_ip_t *ip, uint32_t port);
rn_socket_t *rn_ssl_accept(rn_socket_t *socket, rn_ip_t *fromip, uint32_t *fromport);

#endif /* !RINOO_NET_SSL_H_ */
