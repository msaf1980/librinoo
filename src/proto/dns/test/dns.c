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
	rn_ip_t ip;

	XTEST(rn_dns_ip_get(arg, "google.com", &ip) == 0);
	rn_log("IP: %s", inet_ntoa(ip.v4.sin_addr));
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
