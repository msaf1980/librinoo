/**
 * @file   rinoo_socket_run.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Tue Mar 20 16:59:54 2012
 *
 * @brief  Test file for rinoo_socket_run function
 *
 *
 */

#include	"rinoo/rinoo.h"

t_rinoosched *sched;
t_rinoosocket *sock1;

void socket1_func(t_rinoosocket *unused(s))
{
	rinoo_sched_stop(sched);
}

/**
 * This test will check if a scheduler is well initialized.
 *
 * @return 0 if test passed
 */
int main()
{
	sched = rinoo_sched();
	XTEST(sched != NULL);
	sock1 = rinoo_socket(sched, AF_INET, SOCK_STREAM, socket1_func);
	XTEST(sock1 != NULL);
	XTEST(rinoo_socket_schedule(sock1, 0) == 0);
	rinoo_sched_loop(sched);
	rinoo_sched_destroy(sched);
	XPASS();
}
