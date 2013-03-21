/**
 * @file   scheduler.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2013
 * @date   Mon Dec 28 00:14:20 2009
 *
 * @brief  Header file for scheduler function declarations.
 *
 *
 */

#ifndef RINOO_SCHEDULER_SCHEDULER_H_
#define RINOO_SCHEDULER_SCHEDULER_H_

#define RINOO_SCHEDULER_MAXFDS		1000000

typedef enum e_rinoosched_mode {
	RINOO_MODE_NONE = 0,
	RINOO_MODE_IN = 1,
	RINOO_MODE_OUT = 2,
} t_rinoosched_mode;

typedef struct s_rinoosched_event {
	int error;
	t_rinootask *task;
	t_rinoosched_mode waiting;
	t_rinoosched_mode received;
} t_rinoosched_event;

typedef struct s_rinoosched {
	int stop;
	uint32_t nbpending;
	struct timeval clock;
	t_rinootask_driver driver;
	struct s_rinooepoll epoll;
	t_rinoosched_event event_map[RINOO_SCHEDULER_MAXFDS];
} t_rinoosched;

t_rinoosched *rinoo_sched();
void rinoo_sched_destroy(t_rinoosched *sched);
void rinoo_sched_stop(t_rinoosched *sched);
int rinoo_sched_waitfor(t_rinoosched *sched, int fd,  t_rinoosched_mode mode);
int rinoo_sched_remove(t_rinoosched *sched, int fd);
struct s_rinoosocket *rinoo_sched_get(t_rinoosched *sched, int fd);
void rinoo_sched_wakeup(t_rinoosched *sched, int fd, t_rinoosched_mode mode, int error);
int rinoo_sched_poll(t_rinoosched *sched);
void rinoo_sched_loop(t_rinoosched *sched);

#endif /* !RINOO_SCHEDULER_SCHEDULER_H */
