/**
 * @file   rn_task_pause.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  rn_task_pause unit test
 *
 *
 */

#include "rinoo/rinoo.h"

int check = 0;

void task1(void *sched)
{
	int i;

	printf("%s start\n", __FUNCTION__);
	for (i = 0; i < 10; i++) {
		check++;
		printf("%s - %d\n", __FUNCTION__, check);
		XTEST(check == 1);
		rn_task_pause(sched);
	}
	printf("%s end\n", __FUNCTION__);
}

void task2(void *sched)
{
	int i;

	printf("%s start\n", __FUNCTION__);
	for (i = 0; i < 10; i++) {
		check++;
		printf("%s - %d\n", __FUNCTION__, check);
		XTEST(check == 2);
		rn_task_pause(sched);
	}
	printf("%s end\n", __FUNCTION__);
}

void task3(void *sched)
{
	int i;

	printf("%s start\n", __FUNCTION__);
	for (i = 0; i < 10; i++) {
		check++;
		printf("%s - %d\n", __FUNCTION__, check);
		XTEST(check == 3);
		check = 0;
		rn_task_pause(sched);
	}
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
	XTEST(rn_task_start(sched, task1, sched) == 0);
	XTEST(rn_task_start(sched, task2, sched) == 0);
	XTEST(rn_task_start(sched, task3, sched) == 0);
	rn_sched_loop(sched);
	rn_sched_destroy(sched);
	XPASS();
}
