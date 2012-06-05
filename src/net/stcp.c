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

t_rinoossl *rinoossl()
{
	RSA *rsa;
	X509 *x509;
	SSL_CTX *ctx;
	EVP_PKEY *pkey;
	X509_NAME *name;
	t_rinoossl *ssl;

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
		rinoossl_destroy(ssl);
		return NULL;
	}
	if (SSL_CTX_use_PrivateKey(ctx, pkey) == 0) {
		rinoossl_destroy(ssl);
		return NULL;
	}
	return ssl;
}

void rinoossl_destroy(t_rinoossl *ssl)
{
	if (ssl != NULL) {
		X509_free(ssl->x509);
		EVP_PKEY_free(ssl->pkey);
		SSL_CTX_free(ssl->ctx);
		free(ssl);
	}
}
