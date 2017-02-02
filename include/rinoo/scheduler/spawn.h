/**
 * @file   spawn.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  RiNOO scheduler spawning functions
 *
 *
 */

#ifndef RINOO_SCHEDULER_SPAWN_H_
#define RINOO_SCHEDULER_SPAWN_H_

/* Defined in scheduler.h */
struct rn_sched_s;

typedef struct rn_thread_s {
	pthread_t id;
	struct rn_sched_s *sched;
} rn_thread_t;

typedef struct rn_sched_spawns_s {
	int count;
	rn_thread_t *thread;
} rn_sched_spawns_t;

int rn_spawn(struct rn_sched_s *sched, int count);
void rn_spawn_destroy(struct rn_sched_s *sched);
struct rn_sched_s *rn_spawn_get(struct rn_sched_s *sched, int id);
int rn_spawn_start(struct rn_sched_s *sched);
void rn_spawn_stop(struct rn_sched_s *sched);
void rn_spawn_join(struct rn_sched_s *sched);

#endif /* !RINOO_SCHEDULER_SPAWN_H_ */
