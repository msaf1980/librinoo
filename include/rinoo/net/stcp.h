/**
 * @file   stcp.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Tue Jun  5 10:45:19 2012
 *
 * @brief  Header file for Secure TCP socket function declaractions
 *
 *
 */

#ifndef RINOO_NET_STCP_H_
#define RINOO_NET_STCP_H_

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
t_rinoossl *rinoo_ssl(t_rinoosched *sched, t_rinoossl_ctx *ctx);
void rinoo_ssl_destroy(t_rinoossl *ssl);
int rinoo_ssl_connect(t_rinoossl *ssl, t_ip ip, u32 port);
int rinoo_ssl_listen(t_rinoossl *ssl, t_ip ip, u32 port);
t_rinoossl *rinoo_ssl_accept(t_rinoossl *ssl, t_ip *fromip, u32 *fromport);
ssize_t rinoo_ssl_read(t_rinoossl *ssl, void *buf, size_t count);
ssize_t rinoo_ssl_write(t_rinoossl *ssl, const void *buf, size_t count);
ssize_t rinoo_ssl_readb(t_rinoossl *ssl, t_buffer *buffer);
ssize_t rinoo_ssl_writeb(t_rinoossl *ssl, t_buffer *buffer);

#endif /* !RINOO_NET_STCP_H_ */
