/**
 * @file   rinoo_stcp_connect.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Thu Jun 21 14:52:08 2012
 *
 * @brief  Test file for rinoossl connect function
 *
 *
 */

#include	"rinoo/rinoo.h"

t_rinoosched *sched;

void ssl_run(t_rinoossl *ssl)
{
	XTEST(rinoo_ssl_connect(ssl, 0, 443, 5) == 0);
	rinoo_sched_stop(sched);
}

/**
 * This test will check if a scheduler is well initialized.
 *
 * @return 0 if test passed
 */
int main()
{
	t_rinoossl *sslsock;
	t_rinoossl_ctx *ssl;

	sched = rinoo_sched();
	XTEST(sched != NULL);
	ssl = rinoo_ssl_context();
	XTEST(ssl != NULL);
	sslsock = rinoo_ssl(sched, ssl, ssl_run);
	XTEST(sslsock != NULL);
	XTEST(rinoo_socket_start(&sslsock->socket) == 0);
	rinoo_sched_loop(sched);
	rinoo_ssl_context_destroy(ssl);
	rinoo_sched_destroy(sched);
	XPASS();
}
