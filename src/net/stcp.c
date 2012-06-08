/**
 * @file   stcp.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Tue Jun  5 10:26:46 2012
 *
 * @brief  Secure connection management
 *
 *
 */

#include	"rinoo/rinoo.h"

static void rinoo_ssl_autodestroy(t_rinoosocket *socket)
{
	rinoo_ssl_destroy(container_of(socket, t_rinoossl, socket));
}

static void rinoo_ssl_run(t_rinoosocket *socket)
{
	t_rinoossl *ssl;

	ssl = container_of(socket, t_rinoossl, socket);
	ssl->run(ssl);
}

t_rinoossl_ctx *rinoo_ssl_context()
{
	RSA *rsa;
	X509 *x509;
	SSL_CTX *ctx;
	EVP_PKEY *pkey;
	X509_NAME *name;
	t_rinoossl_ctx *ssl;

	/* SSL_library_init is not reentrant! */
	SSL_library_init();

	pkey = EVP_PKEY_new();
	if (pkey == NULL) {
		return NULL;
	}

	x509 = X509_new();
	if (x509 == NULL) {
		EVP_PKEY_free(pkey);
		return NULL;
	}
	rsa = RSA_generate_key(512, RSA_F4, NULL,NULL);
	if (rsa == NULL || EVP_PKEY_assign_RSA(pkey, rsa) == 0) {
		X509_free(x509);
		EVP_PKEY_free(pkey);
		return NULL;
	}

	X509_set_version(x509, 3);
	ASN1_INTEGER_set(X509_get_serialNumber(x509), 0);
	X509_gmtime_adj(X509_get_notBefore(x509), 0);
	X509_gmtime_adj(X509_get_notAfter(x509), 60 * 60 * 24 * 360);
	X509_set_pubkey(x509, pkey);

	name = X509_get_subject_name(x509);

	if (X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (const unsigned char *) "US", -1, -1, 0) == 0) {
		X509_free(x509);
		EVP_PKEY_free(pkey);
		return NULL;
	}
	if (X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (const unsigned char *) "RiNOO", -1, -1, 0) == 0) {
		X509_free(x509);
		EVP_PKEY_free(pkey);
		return NULL;
	}

	X509_set_issuer_name(x509, name);

	if (X509_sign(x509, pkey, EVP_md5()) == 0) {
		X509_free(x509);
		EVP_PKEY_free(pkey);
		return NULL;
	}

	ssl = malloc(sizeof(*ssl));
	if (ssl == NULL) {
		X509_free(x509);
		EVP_PKEY_free(pkey);
		return NULL;
	}
	ctx = SSL_CTX_new(SSLv23_method());
	if (ctx == NULL) {
		X509_free(x509);
		EVP_PKEY_free(pkey);
		free(ssl);
		return NULL;
	}
	ssl->x509 = x509;
	ssl->pkey = pkey;
	ssl->ctx = ctx;
	if (SSL_CTX_use_certificate(ctx, x509) == 0) {
		rinoo_ssl_context_destroy(ssl);
		return NULL;
	}
	if (SSL_CTX_use_PrivateKey(ctx, pkey) == 0) {
		rinoo_ssl_context_destroy(ssl);
		return NULL;
	}
	return ssl;
}

void rinoo_ssl_context_destroy(t_rinoossl_ctx *ctx)
{
	if (ctx != NULL) {
		X509_free(ctx->x509);
		EVP_PKEY_free(ctx->pkey);
		SSL_CTX_free(ctx->ctx);
		free(ctx);
	}
}

t_rinoossl *rinoo_ssl(t_rinoosched *sched, t_rinoossl_ctx *ctx, void (*run)(t_rinoossl *ssl))
{
	t_rinoossl *ssl;

	XASSERT(sched != NULL && ctx != NULL, NULL);

	ssl = malloc(sizeof(*ssl));
	if (ssl == NULL) {
		return NULL;
	}
	ssl->ctx = ctx;
	ssl->run = run;
	if (unlikely(rinoo_socket_set(sched, &ssl->socket, AF_INET, SOCK_STREAM, rinoo_ssl_run, rinoo_ssl_autodestroy) != 0)) {
		free(ssl);
		return NULL;
	}
	return ssl;
}

void rinoo_ssl_destroy(t_rinoossl *ssl)
{
	rinoo_socket_close(&ssl->socket);
	free(ssl);
}

int rinoo_ssl_connect(t_rinoossl *ssl, t_ip ip, u32 port, u32 timeout)
{
	int ret;
	BIO *sbio;

	if (unlikely(rinoo_tcp_connect(&ssl->socket, ip, port, timeout) != 0)) {
		return -1;
	}
	ssl->ssl = SSL_new(ssl->ctx->ctx);
	if (unlikely(ssl->ssl == NULL)) {
		return -1;
	}
	sbio = BIO_new_socket(ssl->socket.fd, BIO_NOCLOSE);
	if (unlikely(sbio == NULL)) {
		return -1;
	}
	SSL_set_bio(ssl->ssl, sbio, sbio);
	ret = SSL_connect(ssl->ssl);
	if (ret == 0) {
		return -1;
	}
	if (ret == 1) {
		return 0;
	}

	return 0;
}
