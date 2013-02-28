/**
 * @file   rinoo_stcp_gen.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Tue Mar 20 16:59:54 2012
 *
 * @brief  Test file for rinoossl function
 *
 *
 */

#include "rinoo/rinoo.h"

/**
 * This test will check if a scheduler is well initialized.
 *
 * @return 0 if test passed
 */
int main()
{
	t_rinoossl_ctx *ssl;

	ssl = rinoo_ssl_context();
	XTEST(ssl != NULL);
	rinoo_ssl_context_destroy(ssl);
	XPASS();
}
