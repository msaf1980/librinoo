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

void task3(t_rinootask *unused(task))
{
	printf("%s start\n", __FUNCTION__);
	checker = 3;
	printf("%s end\n", __FUNCTION__);
}

void task2(t_rinootask *task)
{
	t_rinootask *t3;

	printf("%s start\n", __FUNCTION__);
	XTEST(checker == 1);
	t3 = malloc(sizeof(*t3));
	XTEST(rinoo_task(task->sched, t3, task3, (void (*)(t_rinootask *task)) free) == 0);
	checker = 2;
	rinoo_task_run(t3);
	XTEST(checker == 3);
	printf("%s end\n", __FUNCTION__);
}

void task1(t_rinootask *task)
{
	t_rinootask *t2;

	printf("%s start\n", __FUNCTION__);
	XTEST(checker == 0);
	t2 = malloc(sizeof(*t2));
	XTEST(rinoo_task(task->sched, t2, task2, (void (*)(t_rinootask *task)) free) == 0);
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
	t1 = malloc(sizeof(*t1));
	XTEST(rinoo_task(sched, t1, task1, (void (*)(t_rinootask *task)) free) == 0);
	rinoo_task_run(t1);
	rinoo_sched_destroy(sched);
	XTEST(checker == 3);
	XPASS();
}
