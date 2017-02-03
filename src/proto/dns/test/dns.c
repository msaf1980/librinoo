/**
 * @file   dns.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  DNS unit test
 *
 *
 */

#include "rinoo/rinoo.h"

void dns_test(void *arg)
{
	rn_addr_t addr;

	XTEST(rn_dns_addr_get(arg, "google.com", &addr) == 0);
	rn_log("IP: %s", inet_ntoa(addr.v4.sin_addr));
}

/**
 * Main function for this unit test.
 *
 *
 * @return 0 if test passed
 */
int main()
{
	rn_sched_t *sched;

	sched = rn_scheduler();
	XTEST(sched != NULL);
	XTEST(rn_task_start(sched, dns_test, sched) == 0);
	rn_scheduler_loop(sched);
	rn_scheduler_destroy(sched);
	XPASS();
}
