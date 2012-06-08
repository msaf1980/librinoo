/**
 * @file   stcp.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Tue Jun  5 10:45:19 2012
 *
 * @brief  Header file for Secure TCP socket function declaractions
 *
 *
 */

#ifndef		RINOO_NET_STCP_H_
# define	RINOO_NET_STCP_H_

typedef struct s_rinoossl_ctx
{
	X509 *x509;
	EVP_PKEY *pkey;
	SSL_CTX *ctx;
} t_rinoossl_ctx;

typedef struct s_rinoossl
{
	SSL *ssl;
	t_rinoossl_ctx *ctx;
	t_rinoosocket socket;
	void (*run)(struct s_rinoossl *ssl);
} t_rinoossl;

t_rinoossl_ctx *rinoo_ssl_context();
void rinoo_ssl_context_destroy(t_rinoossl_ctx *ctx);
t_rinoossl *rinoo_ssl(t_rinoosched *sched, t_rinoossl_ctx *ctx, void (*run)(t_rinoossl *ssl));
void rinoo_ssl_destroy(t_rinoossl *ssl);

#endif		/* !RINOO_NET_STCP_H_ */
