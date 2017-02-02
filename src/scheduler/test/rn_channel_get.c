/**
 * @file   rn_channel_get.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2015
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  rn_channel_get/rn_channel_put unit test
 *
 *
 */

#include "rinoo/rinoo.h"

void task1(void *channel)
{
	int i;
	int *x;

	for (i = 0; i < 10; i++) {
		rn_channel_put(channel, &i);
		x = rn_channel_get(channel);
		XTEST(i == *x);
	}
}

void task2(void *channel)
{
	int i;
	int *x;

	for (i = 0; i < 10; i++) {
		x = rn_channel_get(channel);
		XTEST(i == *x);
		rn_channel_put(channel, &i);
	}
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
	rn_channel_t *channel;


	sched = rn_scheduler();
	XTEST(sched != NULL);
	channel = rn_channel(sched);
	XTEST(channel != NULL);
	XTEST(rn_task_start(sched, task1, channel) == 0);
	XTEST(rn_task_start(sched, task2, channel) == 0);
	rn_scheduler_loop(sched);
	rn_channel_destroy(channel);
	rn_scheduler_destroy(sched);
	XPASS();
}
