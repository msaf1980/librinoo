/**
 * @file   inotify.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2014
 * @date   Tue Jul 15 19:35:23 2014
 *
 * @brief  Implementation of inotify for rinoo.
 *
 *
 */

#include "rinoo/rinoo.h"

t_rinoo_inotify *rinoo_inotify(t_rinoosched *sched)
{
	int fd;
	t_rinoo_inotify *notify;

	fd = inotify_init1(IN_NONBLOCK);
	if (fd < 0) {
		return NULL;
	}
	notify = calloc(1, sizeof(*notify));
	if (notify == NULL) {
		close(fd);
		return NULL;
	}
	notify->node.fd = fd;
	notify->node.sched = sched;
	return notify;
}

void rinoo_inotify_destroy(t_rinoo_inotify *notify)
{
	rinoo_sched_remove(&notify->node);
	close(notify->node.fd);
	free(notify);
}

t_rinoo_inotify_watch *rinoo_inotify_add_watch(t_rinoo_inotify *inotify, const char *path, t_rinoo_inotify_type type, bool recursive)
{
	int wd;
	t_rinoo_inotify_watch *watch;

	if (inotify->nb_watches >= ARRAY_SIZE(inotify->watches)) {
		return NULL;
	}
	wd = inotify_add_watch(inotify->node.fd, path, type);
	if (wd < 0) {
		return NULL;
	}
	if ((unsigned int) wd >= ARRAY_SIZE(inotify->watches)) {
		inotify_rm_watch(inotify->node.fd, wd);
		return NULL;
	}
	watch = calloc(1, sizeof(*watch));
	if (watch == NULL) {
		inotify_rm_watch(inotify->node.fd, wd);
		return NULL;
	}
	watch->wd = wd;
	watch->path = strdup(path);
	if (watch->path == NULL) {
		inotify_rm_watch(inotify->node.fd, wd);
		free(watch);
		return NULL;
	}
	inotify->watches[wd] = watch;
	inotify->nb_watches++;
	return watch;
}

int rinoo_inotify_rm_watch(t_rinoo_inotify *inotify, t_rinoo_inotify_watch *watch)
{
	inotify_rm_watch(inotify->node.fd, watch->wd);
	inotify->watches[watch->wd] = NULL;
	inotify->nb_watches--;
	free(watch->path);
	free(watch);
	return 0;
}

static int rinoo_inotify_waitio(t_rinoo_inotify *inotify)
{
	inotify->io_calls++;
	if (inotify->io_calls > 10) {
		inotify->io_calls = 0;
		if (rinoo_task_pause(inotify->node.sched) != 0) {
				return -1;
		}
	}
	return 0;
}

t_rinoo_inotify_event *rinoo_inotify_event(t_rinoo_inotify *inotify)
{
	static __thread t_rinoo_inotify_event event;
	ssize_t ret;
	char buffer[1024];
	struct inotify_event *ievent;

	if (rinoo_inotify_waitio(inotify) != 0) {
		return NULL;
	}
	errno = 0;
	while ((ret = read(inotify->node.fd, buffer, sizeof(buffer))) < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			return NULL;
		}
		inotify->io_calls = 0;
		if (rinoo_sched_waitfor(&inotify->node, RINOO_MODE_IN) != 0) {
			return NULL;
		}
		errno = 0;
	}
	ievent = (struct inotify_event *) buffer;
	event.type = ievent->mask;
	event.watch = inotify->watches[ievent->wd];
	return &event;
}
