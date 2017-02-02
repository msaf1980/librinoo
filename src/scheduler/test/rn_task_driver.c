/**
 * @file   rn_task.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  rn_task unit test
 *
 *
 */

#include "rinoo/rinoo.h"

int checker = 0;

void task3(void *arg)
{
	rn_sched_t *sched = arg;
	printf("%s start\n", __FUNCTION__);
	XTEST(checker == 2);
	checker = 3;
	rn_scheduler_stop(sched);
	printf("%s end\n", __FUNCTION__);
}

void task2(void *arg)
{
	rn_sched_t *sched = arg;

	printf("%s start\n", __FUNCTION__);
	XTEST(checker == 1);
	rn_task_start(sched, task3, sched);
	XTEST(checker == 1);
	checker = 2;
	printf("%s end\n", __FUNCTION__);
}

void task1(void *arg)
{
	rn_sched_t *sched = arg;

	printf("%s start\n", __FUNCTION__);
	XTEST(checker == 0);
	rn_task_start(sched, task2, sched);
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

	sched = rn_scheduler();
	XTEST(sched != NULL);
	XTEST(rn_task_start(sched, task1, sched) == 0);
	rn_scheduler_loop(sched);
	rn_scheduler_destroy(sched);
	XTEST(checker == 3);
	XPASS();
}
