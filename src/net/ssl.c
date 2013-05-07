/**
 * @file   ssl.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Tue Jun  5 10:26:46 2012
 *
 * @brief  Secure connection management
 *
 *
 */

#include"rinoo/rinoo.h"

extern const t_rinoosocket_class socket_class_ssl;

/**
 * Creates a simple SSL context.
 *
 *
 * @return SSL context pointer
 */
t_rinoossl_ctx *rinoo_ssl_context(void)
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

/**
 * Destroys a SSL context.
 *
 * @param ctx SSL contextt pointer
 */
void rinoo_ssl_context_destroy(t_rinoossl_ctx *ctx)
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
t_rinoossl *rinoo_ssl_get(t_rinoosocket *socket)
{
	return container_of(socket, t_rinoossl, socket);
}

/**
 * Creates a SSL client and tries to connect to the specified IP and port.
 *
 * @param sched Scheduler pointer
 * @param ctx SSL context
 * @param ip Destination IP
 * @param port Destination port
 * @param timeout Socket timeout
 *
 * @return Socket pointer on success or NULL if an error occurs
 */
t_rinoosocket *rinoo_ssl_client(t_rinoosched *sched, t_rinoossl_ctx *ctx, t_ip ip, uint32_t port, uint32_t timeout)
{
	t_rinoossl *ssl;
	t_rinoosocket *socket;
	struct sockaddr_in addr;

	socket = rinoo_socket(sched, &socket_class_ssl);
	if (unlikely(socket == NULL)) {
		return NULL;
	}
	ssl = rinoo_ssl_get(socket);
	ssl->ctx = ctx;
	if (timeout != 0 && rinoo_socket_timeout(socket, timeout) != 0) {
		rinoo_socket_destroy(socket);
		return NULL;
	}
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	if (rinoo_socket_connect(socket, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
		rinoo_socket_destroy(socket);
		return NULL;
	}
	return socket;
}

/**
 * Creates a SSL server listening to a specific port, on specific IP.
 *
 * @param sched Scheduler pointer
 * @param ctx SSL context
 * @param ip IP to bind
 * @param port Port to bind
 *
 * @return Socket pointer on success or NULL if an error occurs
 */
t_rinoosocket *rinoo_ssl_server(t_rinoosched *sched, t_rinoossl_ctx *ctx, t_ip ip, uint32_t port)
{
	t_rinoossl *ssl;
	t_rinoosocket *socket;
	struct sockaddr_in addr;

	socket = rinoo_socket(sched, &socket_class_ssl);
	if (unlikely(socket == NULL)) {
		return NULL;
	}
	ssl = rinoo_ssl_get(socket);
	ssl->ctx = ctx;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	if (rinoo_socket_listen(socket, (struct sockaddr *) &addr, sizeof(addr), RINOO_TCP_BACKLOG) != 0) {
		rinoo_socket_destroy(socket);
		return NULL;
	}
	return socket;
}

/**
 * Accepts a new connection from a listening socket.
 *
 * @param socket Pointer to the socket which is listening to
 * @param fromip Pointer to a t_ip where to store the from_ip
 * @param fromport Pointer to a uint32_t where to store the from_port
 *
 * @return A pointer to the new socket on success or NULL if an error occurs
 */
t_rinoosocket *rinoo_ssl_accept(t_rinoosocket *socket, t_ip *fromip, uint32_t *fromport)
{
	t_rinoosocket *new;
	struct sockaddr_in addr;

	new = rinoo_socket_accept(socket, (struct sockaddr *) &addr, (socklen_t *)(int[]){(sizeof(struct sockaddr))});
	if (fromip != NULL) {
		*fromip = addr.sin_addr.s_addr;
	}
	if (fromport != NULL) {
		*fromport = ntohs(addr.sin_port);
	}
	return new;
}

