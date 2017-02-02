/**
 * @file   rn_spawn_stop.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  RiNOO spawn stop unit test
 *
 *
 */

#include "rinoo/rinoo.h"

#define NBSPAWNS	10

int checker[NBSPAWNS + 1];

void task(void *unused(arg))
{
	rn_sched_t *cur;

	rn_log("%s start %d", __FUNCTION__, rn_sched_self()->id);
	/* sleep should return as soon as we get killed by the main scheduler */
	sleep(1000);
	cur = rn_sched_self();
	XTEST(cur != NULL);
	XTEST(cur->id >= 0 && cur->id <= NBSPAWNS);
	XTEST(checker[cur->id] == 0);
	checker[cur->id] = 1;
	rn_log("%s end %d", __FUNCTION__, rn_sched_self()->id);
}

/**
 * Wait 1 second and stop schedulers.
 * This needs to be called from the main scheduler.
 *
 * @param sched Pointer to the main scheduler
 */
void wait_and_stop(void *sched)
{
	rn_sched_t *cur;

	cur = rn_sched_self();
	XTEST(cur == sched);
	XTEST(cur->id == 0);
	checker[cur->id] = 1;
	rn_log("wait & stop");
	rn_task_wait(sched, 1000);
	rn_log("stopping");
	rn_sched_stop(sched);
}

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int main()
{
	int i;
	rn_sched_t *cur;
	rn_sched_t *sched;

	memset(checker, 0, sizeof(*checker) * NBSPAWNS);
	sched = rn_sched();
	XTEST(sched != NULL);
	XTEST(rn_spawn(sched, NBSPAWNS) == 0);
	for (i = 1; i <= NBSPAWNS; i++) {
		cur = rn_spawn_get(sched, i);
		XTEST(cur != NULL);
		XTEST(rn_task_start(cur, task, NULL) == 0);
	}
	XTEST(rn_task_start(sched, wait_and_stop, sched) == 0);
	rn_sched_loop(sched);
	rn_sched_destroy(sched);
	for (i = 0; i <= NBSPAWNS; i++) {
		XTEST(checker[i] == 1);
	}
	XPASS();
}
