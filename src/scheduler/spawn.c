/**
 * @file   spawn.c
 * @author reginaldl <reginald.l@gmail.com>
 * @date   Tue Sep 22 16:52:09 2013
 *
 * @brief RiNOO scheduler spawning functions
 *
 *
 */

#include "rinoo/rinoo.h"

/**
 * Spawns a number of children schedulers.
 *
 * @param sched Parent scheduler
 * @param count Number of children to create
 *
 * @return 0 on success, otherwise -1
 */
int rinoo_spawn(t_rinoosched *sched, int count)
{
	int i;
	pthread_t *thread;
	t_rinoosched *child;
	t_rinoosched **children;

	if (sched->spawns.count == 0) {
		children = calloc(count, sizeof(*children));
		if (children == NULL) {
			return -1;
		}
		thread = calloc(count, sizeof(*thread));
		if (thread == NULL) {
			free(children);
			return -1;
		}
	} else {
		children = realloc(sched->spawns.sched, sizeof(*children) * (sched->spawns.count + count));
		if (children == NULL) {
			return -1;
		}
		thread = realloc(sched->spawns.thread, sizeof(*thread) * (sched->spawns.count + count));
		if (thread == NULL) {
			/* Hopefully, this won't be lost */
			sched->spawns.sched = children;
			return -1;
		}
	}
	sched->spawns.sched = children;
	sched->spawns.thread = thread;
	for (i = sched->spawns.count; i < sched->spawns.count + count; i++) {
		child = rinoo_sched();
		if (child == NULL) {
			sched->spawns.count = i;
			return -1;
		}
		child->id = i + 1;
		sched->spawns.sched[i] = child;
		sched->spawns.thread[i] = 0;
	}
	sched->spawns.count = i;
	return 0;
}

/**
 * Destroy all scheduler spawns
 *
 * @param sched
 */
void rinoo_spawn_destroy(t_rinoosched *sched)
{
	if (sched->spawns.sched != NULL) {
		free(sched->spawns.sched);
	}
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
t_rinoosched *rinoo_spawn_get(t_rinoosched *sched, int id)
{
	if (id < 0 || id > sched->spawns.count) {
		return NULL;
	}
	if (id == 0) {
		return sched;
	}
	return sched->spawns.sched[id - 1];
}

/**
 * Main spawn loop. This function should be executed in a thread.
 *
 * @param sched Scheduler running the loop
 *
 * @return NULL
 */
static void *rinoo_spawn_loop(void *sched)
{
	rinoo_sched_loop(sched);
	rinoo_sched_destroy(sched);
	return NULL;
}

/**
 * Starts spawns. It creates a thread for each spawn.
 *
 * @param sched Main scheduler
 *
 * @return 0 on success otherwise -1
 */
int rinoo_spawn_start(t_rinoosched *sched)
{
	int i;
	sigset_t oldset;
        sigset_t newset;

        sigemptyset(&newset);
        if (sigaddset(&newset, SIGINT) < 0) {
		return -1;
	}
        pthread_sigmask(SIG_BLOCK, &newset, &oldset);
	for (i = 0; i < sched->spawns.count; i++) {
		if (pthread_create(&sched->spawns.thread[i], NULL, rinoo_spawn_loop, sched->spawns.sched[i]) != 0) {
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
void rinoo_spawn_stop(t_rinoosched *sched)
{
	int i;

	for (i = 0; i < sched->spawns.count; i++) {
		if (sched->spawns.thread[i] != 0) {
			rinoo_sched_stop(sched->spawns.sched[i]);
			pthread_kill(sched->spawns.thread[i], SIGUSR2);
		}
	}
}

/**
 * Waits for spawns to finish.
 *
 * @param sched Main scheduler
 */
void rinoo_spawn_join(t_rinoosched *sched)
{
	int i;

	for (i = 0; i < sched->spawns.count; i++) {
		if (sched->spawns.thread[i] != 0) {
			pthread_join(sched->spawns.thread[i], NULL);
		}
	}
}
