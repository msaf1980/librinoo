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

void rn_ssl_init();
rn_ssl_ctx_t *rn_ssl_context(unsigned short keysize);
rn_ssl_ctx_t *rn_ssl_context_load(const char *cert_file, const char *key_file);
void rn_ssl_context_destroy(rn_ssl_ctx_t *ctx);
rn_ssl_t *rn_ssl_get(rn_socket_t *socket);
rn_socket_t *rn_ssl_client(rn_sched_t *sched, rn_ssl_ctx_t *ctx, rn_addr_t *dst, uint32_t timeout);
rn_socket_t *rn_ssl_server(rn_sched_t *sched, rn_ssl_ctx_t *ctx, rn_addr_t *dst);

#endif /* !RINOO_NET_SSL_H_ */
