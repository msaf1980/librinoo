/**
 * @file   stcp.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Tue Jun  5 10:26:46 2012
 *
 * @brief  Secure connection management
 *
 *
 */

#include"rinoo/rinoo.h"

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

t_rinoossl *rinoo_ssl(t_rinoosched *sched, t_rinoossl_ctx *ctx)
{
	t_rinoossl *ssl;

	XASSERT(sched != NULL && ctx != NULL, NULL);

	ssl = calloc(1, sizeof(*ssl));
	if (ssl == NULL) {
		return NULL;
	}
	ssl->ctx = ctx;
	if (unlikely(rinoo_socket_set(sched, &ssl->socket, AF_INET, SOCK_STREAM) != 0)) {
		free(ssl);
		return NULL;
	}
	return ssl;
}

void rinoo_ssl_destroy(t_rinoossl *ssl)
{
	if (ssl->ssl != NULL) {
		SSL_free(ssl->ssl);
	}
	rinoo_socket_close(&ssl->socket);
	free(ssl);
}

int rinoo_ssl_connect(t_rinoossl *ssl, t_ip ip, u32 port)
{
	int ret;
	BIO *sbio;

	if (unlikely(rinoo_tcp_connect(&ssl->socket, ip, port) != 0)) {
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
	while ((ret = SSL_connect(ssl->ssl)) < 0) {
		switch(SSL_get_error(ssl->ssl, ret)) {
		case SSL_ERROR_NONE:
			return 0;
		case SSL_ERROR_ZERO_RETURN:
		case SSL_ERROR_WANT_X509_LOOKUP:
		case SSL_ERROR_SYSCALL:
		case SSL_ERROR_SSL:
			return -1;
		case SSL_ERROR_WANT_READ:
			if (rinoo_socket_waitin(&ssl->socket) != 0) {
				return -1;
			}
			break;
		case SSL_ERROR_WANT_WRITE:
		case SSL_ERROR_WANT_CONNECT:
		case SSL_ERROR_WANT_ACCEPT:
			if (rinoo_socket_waitout(&ssl->socket) != 0) {
				return -1;
			}
			break;

		}
	}
	if (ret == 0) {
		return -1;
	}
	return 0;
}

int rinoo_ssl_listen(t_rinoossl *ssl, t_ip ip, u32 port)
{
	return rinoo_tcp_listen(&ssl->socket, ip, port);
}

t_rinoossl *rinoo_ssl_accept(t_rinoossl *ssl, t_ip *fromip, u32 *fromport)
{
	int fd;
	int ret;
	BIO *sbio;
	int enabled;
	t_rinoossl *new;
	struct sockaddr_in addr;

	if (rinoo_socket_waitin(&ssl->socket) != 0) {
		return NULL;
	}
	fd = accept(ssl->socket.fd, (struct sockaddr *) &addr, (socklen_t *)(int[]){(sizeof(struct sockaddr))});
	if (fd == -1) {
		return NULL;
	}
	if (fromip != NULL) {
		*fromip = addr.sin_addr.s_addr;
	}
	if (fromport != NULL) {
		*fromport = ntohs(addr.sin_port);
	}
	new = calloc(1, sizeof(*new));
	if (unlikely(new == NULL)) {
		close(fd);
		return NULL;
	}
	new->ctx = ssl->ctx;
	new->socket.fd = fd;
	new->socket.sched = ssl->socket.sched;
	enabled = 1;
	if (unlikely(ioctl(fd, FIONBIO, &enabled) == -1)) {
		close(fd);
		free(new);
		return NULL;
	}
	new->socket.parent = &ssl->socket;
	if (rinoo_sched_waitfor(new->socket.sched, new->socket.fd, RINOO_MODE_OUT) != 0) {
		rinoo_ssl_destroy(new);
		return NULL;
	}
	new->ssl = SSL_new(new->ctx->ctx);
	if (unlikely(new->ssl == NULL)) {
		rinoo_ssl_destroy(new);
		return new;
	}
	sbio = BIO_new_socket(new->socket.fd, BIO_NOCLOSE);
	if (unlikely(sbio == NULL)) {
		rinoo_ssl_destroy(new);
		return NULL;
	}
	SSL_set_bio(new->ssl, sbio, sbio);
	while ((ret = SSL_accept(new->ssl)) <= 0) {
		switch(SSL_get_error(new->ssl, ret)) {
		case SSL_ERROR_NONE:
		case SSL_ERROR_ZERO_RETURN:
		case SSL_ERROR_WANT_X509_LOOKUP:
		case SSL_ERROR_SYSCALL:
		case SSL_ERROR_SSL:
			rinoo_ssl_destroy(new);
			return NULL;
		case SSL_ERROR_WANT_READ:
			if (rinoo_socket_waitin(&new->socket) != 0) {
				rinoo_ssl_destroy(new);
				return NULL;
			}
			break;
		case SSL_ERROR_WANT_WRITE:
		case SSL_ERROR_WANT_CONNECT:
		case SSL_ERROR_WANT_ACCEPT:
			if (rinoo_socket_waitout(&new->socket) != 0) {
				rinoo_ssl_destroy(new);
				return NULL;
			}
			break;

		}
	}
	return new;
}

ssize_t rinoo_ssl_read(t_rinoossl *ssl, void *buf, size_t count)
{
	int ret;

	/* Don't need to wait for input here as SSL is buffered */
	while ((ret = SSL_read(ssl->ssl, buf, count)) < 0) {
		switch(SSL_get_error(ssl->ssl, ret)) {
		case SSL_ERROR_NONE:
			return 0;
		case SSL_ERROR_ZERO_RETURN:
		case SSL_ERROR_WANT_X509_LOOKUP:
		case SSL_ERROR_SYSCALL:
		case SSL_ERROR_SSL:
			return -1;
		case SSL_ERROR_WANT_READ:
			if (rinoo_socket_waitin(&ssl->socket) != 0) {
				return -1;
			}
			break;
		case SSL_ERROR_WANT_WRITE:
		case SSL_ERROR_WANT_CONNECT:
		case SSL_ERROR_WANT_ACCEPT:
			if (rinoo_socket_waitout(&ssl->socket) != 0) {
				return -1;
			}
			break;

		}
	}
	if (ret <= 0) {
		return -1;
	}
	return ret;
}

ssize_t rinoo_ssl_write(t_rinoossl *ssl, const void *buf, size_t count)
{
	int ret;

	if (rinoo_socket_waitout(&ssl->socket) != 0) {
		return -1;
	}
	while ((ret = SSL_write(ssl->ssl, buf, count)) < 0) {
		switch(SSL_get_error(ssl->ssl, ret)) {
		case SSL_ERROR_NONE:
			return 0;
		case SSL_ERROR_ZERO_RETURN:
		case SSL_ERROR_WANT_X509_LOOKUP:
		case SSL_ERROR_SYSCALL:
		case SSL_ERROR_SSL:
			return -1;
		case SSL_ERROR_WANT_READ:
			if (rinoo_socket_waitin(&ssl->socket) != 0) {
				return -1;
			}
			break;
		case SSL_ERROR_WANT_WRITE:
		case SSL_ERROR_WANT_CONNECT:
		case SSL_ERROR_WANT_ACCEPT:
			if (rinoo_socket_waitout(&ssl->socket) != 0) {
				return -1;
			}
			break;

		}
	}
	if (ret <= 0) {
		return -1;
	}
	return ret;
}

ssize_t rinoo_ssl_readb(t_rinoossl *ssl, t_buffer *buffer)
{
	ssize_t res;

	if (buffer_isfull(buffer) && buffer_extend(buffer, 0) <= 0) {
		return -1;
	}
	res = rinoo_ssl_read(ssl, buffer_ptr(buffer) + buffer_size(buffer), buffer_msize(buffer) - buffer_size(buffer));
	if (res <= 0) {
		return -1;
	}
	buffer_setsize(buffer, buffer_size(buffer) + res);
	return res;
}

ssize_t rinoo_ssl_writeb(t_rinoossl *ssl, t_buffer *buffer)
{
	size_t len;
	size_t total;
	ssize_t res;

	total = 0;
	len = buffer_size(buffer);
	while (len > 0) {
		res = rinoo_ssl_write(ssl, buffer_ptr(buffer) + buffer_size(buffer) - len, len);
		if (res <= 0) {
			return -1;
		}
		total += res;
		len -= res;
	}
	return total;
}
