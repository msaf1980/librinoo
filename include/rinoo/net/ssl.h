/**
 * @file   ssl.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Tue Jun  5 10:45:19 2012
 *
 * @brief  Header file for Secure TCP socket function declaractions
 *
 *
 */

#ifndef RINOO_NET_SSL_H_
#define RINOO_NET_SSL_H_

typedef struct s_rinoossl_ctx {
	X509 *x509;
	EVP_PKEY *pkey;
	SSL_CTX *ctx;
} t_rinoossl_ctx;

typedef struct s_rinoossl {
	SSL *ssl;
	t_rinoossl_ctx *ctx;
	t_rinoosocket socket;
} t_rinoossl;

t_rinoossl_ctx *rinoo_ssl_context();
void rinoo_ssl_context_destroy(t_rinoossl_ctx *ctx);
t_rinoossl *rinoo_ssl_get(t_rinoosocket *socket);
t_rinoosocket *rinoo_ssl_client(t_rinoosched *sched, t_rinoossl_ctx *ctx, t_ip ip, uint32_t port, uint32_t timeout);
t_rinoosocket *rinoo_ssl_server(t_rinoosched *sched, t_rinoossl_ctx *ctx, t_ip ip, uint32_t port);
t_rinoosocket *rinoo_ssl_accept(t_rinoosocket *socket, t_ip *fromip, uint32_t *fromport);

#endif /* !RINOO_NET_SSL_H_ */
