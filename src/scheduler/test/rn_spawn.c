/**
 * @file   rn_spawn.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  RiNOO spawn unit test
 *
 *
 */

#include "rinoo/rinoo.h"

#define NBSPAWNS	10

int checker[NBSPAWNS + 1];

void task(void *unused(arg))
{
	rn_sched_t *cur;

	printf("%s start\n", __FUNCTION__);
	cur = rn_sched_self();
	XTEST(cur != NULL);
	XTEST(cur->id >= 0 && cur->id <= NBSPAWNS);
	XTEST(checker[cur->id] == 0);
	checker[cur->id] = 1;
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
	int i;
	rn_sched_t *cur;
	rn_sched_t *sched;

	memset(checker, 0, sizeof(*checker) * NBSPAWNS);
	sched = rn_sched();
	XTEST(sched != NULL);
	XTEST(rn_spawn(sched, NBSPAWNS) == 0);
	for (i = 0; i <= NBSPAWNS; i++) {
		cur = rn_spawn_get(sched, i);
		XTEST(cur != NULL);
		if (i == 0) {
			XTEST(cur == sched);
		}
		XTEST(rn_task_start(cur, task, sched) == 0);
	}
	rn_sched_loop(sched);
	rn_sched_destroy(sched);
	for (i = 0; i <= NBSPAWNS; i++) {
		XTEST(checker[i] == 1);
	}
	XPASS();
}
