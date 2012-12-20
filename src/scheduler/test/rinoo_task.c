/**
 * @file   rinoo_task.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Fri Jan 13 18:07:14 2012
 *
 * @brief  rinoo_task unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

int checker = 0;

void task3(void *unused(arg))
{
	printf("%s start\n", __FUNCTION__);
	checker = 3;
	printf("%s end\n", __FUNCTION__);
}

void task2(void *arg)
{
	t_rinootask *t3;
	t_rinoosched *sched = arg;

	printf("%s start\n", __FUNCTION__);
	XTEST(checker == 1);
	t3 = rinoo_task(sched, task3, sched);
	XTEST(t3 != NULL);
	checker = 2;
	rinoo_task_run(t3);
	XTEST(checker == 3);
	printf("%s end\n", __FUNCTION__);
}

void task1(void *arg)
{
	t_rinootask *t2;
	t_rinoosched *sched = arg;

	printf("%s start\n", __FUNCTION__);
	XTEST(checker == 0);
	t2 = rinoo_task(sched, task2, sched);
	XTEST(t2 != NULL);
	checker = 1;
	rinoo_task_run(t2);
	XTEST(checker == 3);
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
	t_rinootask *t1;
	t_rinoosched *sched;

	sched = rinoo_sched();
	XTEST(sched != NULL);
	t1 = rinoo_task(sched, task1, sched);
	XTEST(t1 != NULL);
	rinoo_task_run(t1);
	rinoo_sched_destroy(sched);
	XTEST(checker == 3);
	XPASS();
}
