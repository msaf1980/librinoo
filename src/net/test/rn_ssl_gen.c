/**
 * @file   rinoo_ssl_gen.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Test file for rinoossl function
 *
 *
 */

#include "rinoo/rinoo.h"

/**
 * Main function for this unit test.
 *
 * @return 0 if test passed
 */
int main()
{
	rn_ssl_ctx_t *ssl;

	ssl = rn_ssl_context();
	XTEST(ssl != NULL);
	rn_ssl_context_destroy(ssl);
	XPASS();
}
