/**
 * @file   rinoo_stcp_gen.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Tue Mar 20 16:59:54 2012
 *
 * @brief  Test file for rinoossl function
 *
 *
 */

#include	"rinoo/rinoo.h"

/**
 * This test will check if a scheduler is well initialized.
 *
 * @return 0 if test passed
 */
int main()
{
	t_rinoossl *ssl;

	ssl = rinoossl();
	XTEST(ssl != NULL);
	rinoossl_destroy(ssl);
	XPASS();
}
