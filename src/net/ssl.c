/**
 * @file   ssl.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Secure connection management
 *
 *
 */

#include "rinoo/net/module.h"

extern const rn_socket_class_t socket_class_ssl;
extern const rn_socket_class_t socket_class_ssl6;

/**
 * Creates a simple SSL context.
 *
 *
 * @return SSL context pointer
 */
rn_ssl_ctx_t *rn_ssl_context(void)
{
	RSA *rsa;
	X509 *x509;
	SSL_CTX *ctx;
	EVP_PKEY *pkey;
	X509_NAME *name;
	rn_ssl_ctx_t *ssl;

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
		rn_ssl_context_destroy(ssl);
		return NULL;
	}
	if (SSL_CTX_use_PrivateKey(ctx, pkey) == 0) {
		rn_ssl_context_destroy(ssl);
		return NULL;
	}
	return ssl;
}

/**
 * Destroys a SSL context.
 *
 * @param ctx SSL contextt pointer
 */
void rn_ssl_context_destroy(rn_ssl_ctx_t *ctx)
{
	if (ctx != NULL) {
		X509_free(ctx->x509);
		EVP_PKEY_free(ctx->pkey);
		SSL_CTX_free(ctx->ctx);
		free(ctx);
	}
}

/**
 * Gets a SSL socket from a rinoosocket.
 *
 * @param socket Socket pointer
 *
 * @return SSL socket pointer
 */
rn_ssl_t *rn_ssl_get(rn_socket_t *socket)
{
	return container_of(socket, rn_ssl_t, socket);
}

/**
 * Creates a SSL client and tries to connect to the specified address.
 *
 * @param sched Scheduler pointer
 * @param ctx SSL context
 * @param dst Destination address
 * @param timeout Socket timeout
 *
 * @return Socket pointer on success or NULL if an error occurs
 */
rn_socket_t *rn_ssl_client(rn_sched_t *sched, rn_ssl_ctx_t *ctx, rn_addr_t *dst, uint32_t timeout)
{
	rn_ssl_t *ssl;
	rn_socket_t *socket;

	socket = rn_socket(sched, (IS_IPV6(dst) ? &socket_class_ssl6 : &socket_class_ssl));
	if (unlikely(socket == NULL)) {
		return NULL;
	}
	ssl = rn_ssl_get(socket);
	ssl->ctx = ctx;
	if (timeout != 0 && rn_socket_timeout(socket, timeout) != 0) {
		rn_socket_destroy(socket);
		return NULL;
	}
	if (rn_socket_connect(socket, dst) != 0) {
		rn_socket_destroy(socket);
		return NULL;
	}
	return socket;
}

/**
 * Creates a SSL server listening to a specific address.
 *
 * @param sched Scheduler pointer
 * @param ctx SSL context
 * @param dst Address to bind
 *
 * @return Socket pointer on success or NULL if an error occurs
 */
rn_socket_t *rn_ssl_server(rn_sched_t *sched, rn_ssl_ctx_t *ctx, rn_addr_t *dst)
{
	rn_ssl_t *ssl;
	rn_socket_t *socket;

	socket = rn_socket(sched, (IS_IPV6(dst) ? &socket_class_ssl6 : &socket_class_ssl));
	if (unlikely(socket == NULL)) {
		return NULL;
	}
	ssl = rn_ssl_get(socket);
	ssl->ctx = ctx;
	if (rn_socket_bind(socket, dst, RN_TCP_BACKLOG) != 0) {
		rn_socket_destroy(socket);
		return NULL;
	}
	return socket;
}
