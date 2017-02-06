/**
 * @file   inotify.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2014
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Implementation of inotify for rinoo.
 *
 *
 */

#include "rinoo/fs/module.h"

rn_inotify_t *rn_inotify(rn_sched_t *sched)
{
	int fd;
	rn_inotify_t *notify;

	fd = inotify_init1(IN_NONBLOCK);
	if (fd < 0) {
		return NULL;
	}
	notify = calloc(1, sizeof(*notify));
	if (notify == NULL) {
		close(fd);
		return NULL;
	}
	notify->event.path = rn_buffer_create(NULL);
	if (notify->event.path == NULL) {
		close(fd);
		free(notify);
		return NULL;
	}
	notify->node.fd = fd;
	notify->node.sched = sched;
	return notify;
}

void rn_inotify_destroy(rn_inotify_t *notify)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(notify->watches); i++) {
		if (notify->watches[i] != NULL) {
			rn_inotify_rm_watch(notify, notify->watches[i]);
		}
	}
	rn_scheduler_remove(&notify->node);
	close(notify->node.fd);
	rn_buffer_destroy(notify->event.path);
	free(notify);
}

rn_inotify_watch_t *rn_inotify_add_watch(rn_inotify_t *inotify, const char *path, rn_inotify_type_t type, bool recursive)
{
	int wd;
	int nb;
	rn_fs_entry_t *entry;
	rn_inotify_watch_t *watch;

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
	if (recursive) {
		nb = 0;
		entry = NULL;
		while (rn_fs_browse(path, &entry) == 0 && entry != NULL) {
			nb++;
			if (nb > 100) {
				nb = 0;
				rn_task_pause(inotify->node.sched);
			}
			if (S_ISDIR(entry->stat.st_mode)) {
				rn_inotify_add_watch(inotify, rn_buffer_ptr(entry->path), type, false);
			}
		}
	}
	inotify->watches[wd] = watch;
	inotify->nb_watches++;
	return watch;
}

int rn_inotify_rm_watch(rn_inotify_t *inotify, rn_inotify_watch_t *watch)
{
	inotify_rm_watch(inotify->node.fd, watch->wd);
	inotify->watches[watch->wd] = NULL;
	inotify->nb_watches--;
	free(watch->path);
	free(watch);
	return 0;
}

static int rn_inotify_waitio(rn_inotify_t *inotify)
{
	inotify->io_calls++;
	if (inotify->io_calls > 10) {
		inotify->io_calls = 0;
		if (rn_task_pause(inotify->node.sched) != 0) {
				return -1;
		}
	}
	return 0;
}

rn_inotify_event_t *rn_inotify_event(rn_inotify_t *inotify)
{
	ssize_t ret;
	struct inotify_event *ievent;

	if (rn_inotify_waitio(inotify) != 0) {
		return NULL;
	}
	while ((ret = read(inotify->node.fd, inotify->read_buffer, sizeof(inotify->read_buffer))) < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			rn_error_set(errno);
			return NULL;
		}
		inotify->io_calls = 0;
		if (rn_scheduler_waitfor(&inotify->node, RN_MODE_IN) != 0) {
			return NULL;
		}
	}
	ievent = (struct inotify_event *) inotify->read_buffer;
	inotify->event.type = ievent->mask;
	inotify->event.watch = inotify->watches[ievent->wd];
	rn_buffer_erase(inotify->event.path, 0);
	rn_buffer_addstr(inotify->event.path, inotify->event.watch->path);
	if (ievent->name[0] != 0) {
		rn_buffer_addstr(inotify->event.path, "/");
		rn_buffer_addstr(inotify->event.path, ievent->name);
	}
	rn_buffer_addnull(inotify->event.path);
	return &inotify->event;
}
