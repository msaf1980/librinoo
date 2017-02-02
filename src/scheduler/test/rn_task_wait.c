/**
 * @file   rn_task_wait.c
 * @author Reginad Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  rn_task_wait unit test
 *
 *
 */

#include "rinoo/rinoo.h"

#ifdef RINOO_DEBUG
#include <valgrind/valgrind.h>
#define LATENCY			10 + RUNNING_ON_VALGRIND * 110
#else
#define LATENCY			10
#endif /* !RINOO_DEBUG */

int check_time(struct timeval *prev, uint32_t ms)
{
	uint32_t diffms;
	struct timeval cur;
	struct timeval diff;

	if (gettimeofday(&cur, NULL) != 0) {
		return -1;
	}
	timersub(&cur, prev, &diff);
	diffms = diff.tv_sec * 1000;
	diffms += diff.tv_usec / 1000;
	rn_log("Time diff found: %u, expected: %u - %u", diffms, ms, LATENCY);
	if (diffms > ms) {
		diffms = diffms - ms;
	} else {
		diffms = ms - diffms;
	}
	if (diffms > LATENCY) {
		return -1;
	}
	*prev = cur;
	return 0;
}

void task_func(void *sched)
{
	struct timeval prev;

	printf("%s start\n", __FUNCTION__);
	XTEST(gettimeofday(&prev, NULL) == 0);
	rn_task_wait(sched, 100);
	XTEST(check_time(&prev, 100) == 0);
	rn_task_wait(sched, 200);
	XTEST(check_time(&prev, 200) == 0);
	rn_task_wait(sched, 500);
	XTEST(check_time(&prev, 500) == 0);
	rn_task_wait(sched, 1000);
	XTEST(check_time(&prev, 1000) == 0);
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
	XTEST(rn_task_start(sched, task_func, sched) == 0);
	rn_scheduler_loop(sched);
	rn_scheduler_destroy(sched);
	XPASS();
}
