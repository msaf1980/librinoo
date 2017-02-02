/**
 * @file   rinoo_scheduler_stop.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  RiNOO Scheduler stop unit test
 *
 *
 */

#include "rinoo/rinoo.h"

int checker = 0;

void task(void *unused(arg))
{
	printf("%s start\n", __FUNCTION__);
	XTEST(checker == 0);
	checker = 1;
	printf("%s end\n", __FUNCTION__);
}

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int main()
{
	rn_sched_t *sched;

	sched = rn_sched();
	XTEST(sched != NULL);
	XTEST(rn_task_start(sched, task, sched) == 0);
	rn_sched_loop(sched);
	rn_sched_destroy(sched);
	XTEST(checker == 1);
	XPASS();
}
