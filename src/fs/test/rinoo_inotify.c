/**
 * @file   rinoo_socket_inotify.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2014
 * @date   Wed Jul 16 22:18:10 2014
 *
 * @brief  Test file for rinoo inotify.
 *
 *
 */

#include "rinoo/rinoo.h"

void check_file(void *sched)
{
	t_rinoo_inotify *inotify;
	t_rinoo_inotify_event *event;

	inotify = rinoo_inotify(sched);
	rinoo_inotify_add_watch(inotify, "/tmp", INOTIFY_CREATE | INOTIFY_DELETE, false);
	while ((event = rinoo_inotify_event(inotify)) != NULL) {
		if (event->type & INOTIFY_CREATE) {
			rinoo_log("File created.");
		} else if (event->type & INOTIFY_DELETE) {
			rinoo_log("File deleted.");
		}
	}
	rinoo_log("%s", strerror(errno));
	rinoo_inotify_destroy(inotify);
}

/**
 * Main function for this unit test.
 *
 *
 * @return 0 if test passed
 */
int main()
{
	t_rinoosched *sched;

	sched = rinoo_sched();
	XTEST(sched != NULL);
	XTEST(rinoo_task_start(sched, check_file, sched) == 0);
	rinoo_sched_loop(sched);
	rinoo_sched_destroy(sched);
	XPASS();
}
