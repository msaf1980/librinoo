/**
 * @file   spawn.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief RiNOO scheduler spawning functions
 *
 *
 */

#include "rinoo/scheduler/module.h"

/**
 * Spawns a number of children schedulers.
 *
 * @param sched Parent scheduler
 * @param count Number of children to create
 *
 * @return 0 on success, otherwise -1
 */
int rn_spawn(rn_sched_t *sched, int count)
{
	int i;
	rn_sched_t *child;
	rn_thread_t *thread;

	if (sched->spawns.count == 0) {
		thread = calloc(count, sizeof(*thread));
		if (thread == NULL) {
			return -1;
		}
	} else {
		thread = realloc(sched->spawns.thread, sizeof(*thread) * (sched->spawns.count + count));
		if (thread == NULL) {
			return -1;
		}
	}
	sched->spawns.thread = thread;
	for (i = sched->spawns.count; i < sched->spawns.count + count; i++) {
		child = rn_scheduler();
		if (child == NULL) {
			sched->spawns.count = i;
			return -1;
		}
		child->id = i + 1;
		sched->spawns.thread[i].id = 0;
		sched->spawns.thread[i].sched = child;
	}
	sched->spawns.count = i;
	return 0;
}

/**
 * Destroy all scheduler spawns
 *
 * @param sched
 */
void rn_spawn_destroy(rn_sched_t *sched)
{
	if (sched->spawns.thread != NULL) {
		free(sched->spawns.thread);
	}
	sched->spawns.count = 0;
}

/**
 * Get a scheduler spawn from its id.
 * Id 0 returns sched itself.
 *
 * @param sched Scheduler to use
 * @param id Spawn id
 *
 * @return Pointer to the spawn or NULL if an error occured
 */
rn_sched_t *rn_spawn_get(rn_sched_t *sched, int id)
{
	if (id < 0 || id > sched->spawns.count) {
		return NULL;
	}
	if (id == 0) {
		return sched;
	}
	return sched->spawns.thread[id - 1].sched;
}

/**
 * Main spawn loop. This function should be executed in a thread.
 *
 * @param sched Scheduler running the loop
 *
 * @return NULL
 */
static void *rn_spawn_loop(void *sched)
{
	rn_scheduler_loop(sched);
	rn_scheduler_destroy(sched);
	return NULL;
}

/**
 * Signal handler called when killing threads.
 * This will stop the running scheduler.
 *
 * @param unused Signal number
 */
static void rn_spawn_handler_stop(int unused(sig))
{
	rn_sched_t *sched = rn_scheduler_self();

	if (sched != NULL) {
		rn_scheduler_stop(sched);
	}
}

/**
 * Starts spawns. It creates a thread for each spawn.
 *
 * @param sched Main scheduler
 *
 * @return 0 on success otherwise -1
 */
int rn_spawn_start(rn_sched_t *sched)
{
	int i;
	sigset_t oldset;
	sigset_t newset;

	sigemptyset(&newset);
	if (sigaddset(&newset, SIGINT) < 0) {
		return -1;
	}
	if (sigaction(SIGUSR2, &(struct sigaction){ .sa_handler = rn_spawn_handler_stop }, NULL) != 0) {
		return -1;
	}
	pthread_sigmask(SIG_BLOCK, &newset, &oldset);
	for (i = 0; i < sched->spawns.count; i++) {
		if (pthread_create(&sched->spawns.thread[i].id, NULL, rn_spawn_loop, sched->spawns.thread[i].sched) != 0) {
			pthread_sigmask(SIG_SETMASK, &oldset, NULL);
			return -1;
		}
	}
	pthread_sigmask(SIG_SETMASK, &oldset, NULL);
	return 0;
}

/**
 * Stops all schedule spawns.
 *
 * @param sched Main scheduler
 */
void rn_spawn_stop(rn_sched_t *sched)
{
	int i;

	for (i = 0; i < sched->spawns.count; i++) {
		if (sched->spawns.thread[i].id != 0) {
			sched->spawns.thread[i].sched = NULL;
			pthread_kill(sched->spawns.thread[i].id, SIGUSR2);
		}
	}
}

/**
 * Waits for spawns to finish.
 *
 * @param sched Main scheduler
 */
void rn_spawn_join(rn_sched_t *sched)
{
	int i;

	for (i = 0; i < sched->spawns.count; i++) {
		if (sched->spawns.thread[i].id != 0) {
			pthread_join(sched->spawns.thread[i].id, NULL);
		}
	}
}
