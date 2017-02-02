/**
 * @file   rn_sched_self.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  rn_sched_self unit test
 *
 *
 */

#include "rinoo/rinoo.h"

rn_sched_t *sched;

void task3(void *unused(arg))
{
	printf("%s start\n", __FUNCTION__);
	XTEST(rn_sched_self() == sched);
	printf("%s end\n", __FUNCTION__);
}

void task2(void *unused(arg))
{
	printf("%s start\n", __FUNCTION__);
	XTEST(rn_sched_self() == sched);
	rn_task_run(sched, task3, sched);
	XTEST(rn_sched_self() == sched);
	printf("%s end\n", __FUNCTION__);
}

void task1(void *unused(arg))
{
	printf("%s start\n", __FUNCTION__);
	XTEST(rn_sched_self() == sched);
	rn_task_run(sched, task2, sched);
	XTEST(rn_sched_self() == sched);
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
	XTEST(rn_sched_self() == NULL);
	sched = rn_sched();
	XTEST(sched != NULL);
	XTEST(rn_sched_self() == sched);
	XTEST(rn_task_run(sched, task1, sched) == 0);
	rn_sched_destroy(sched);
	XPASS();
}
