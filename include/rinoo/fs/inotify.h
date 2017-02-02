/**
 * @file   inotify.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2014
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  inotify structures and functions.
 *
 *
 */

#ifndef RINOO_FS_INOTIFY_H_
#define RINOO_FS_INOTIFY_H_

typedef enum rn_inotify_type_e {
	INOTIFY_ACCESS = IN_ACCESS,
	INOTIFY_ATTRIB = IN_ATTRIB,
	INOTIFY_CLOSE = IN_CLOSE,
	INOTIFY_CLOSE_WRITE = IN_CLOSE_WRITE,
	INOTIFY_CLOSE_NOWRITE = IN_CLOSE_NOWRITE,
	INOTIFY_CREATE = IN_CREATE,
	INOTIFY_DELETE = IN_DELETE,
	INOTIFY_DELETE_SELF = IN_DELETE_SELF,
	INOTIFY_MODIFY = IN_MODIFY,
	INOTIFY_MOVE = IN_MOVE,
	INOTIFY_MOVE_SELF = IN_MOVE_SELF,
	INOTIFY_MOVED_FROM = IN_MOVED_FROM,
	INOTIFY_MOVED_TO = IN_MOVED_TO,
	INOTIFY_OPEN = IN_OPEN,
	INOTIFY_DONT_FOLLOW = IN_DONT_FOLLOW,
	INOTIFY_EXCL_UNLINK = IN_EXCL_UNLINK,
	INOTIFY_MASK_ADD = IN_MASK_ADD,
	INOTIFY_ONESHOT = IN_ONESHOT,
	INOTIFY_ONLYDIR = IN_ONLYDIR,
	INOTIFY_IGNORED = IN_IGNORED,
	INOTIFY_ISDIR = IN_ISDIR,
	INOTIFY_Q_OVERFLOW = IN_Q_OVERFLOW,
	INOTIFY_UNMOUNT = IN_UNMOUNT
} rn_inotify_type_t;

typedef struct rn_inotify_watch_s {
	int wd;
	char *path;
} rn_inotify_watch_t;

typedef struct rn_inotify_event_s {
	rn_buffer_t *path;
	rn_inotify_type_t type;
	rn_inotify_watch_t *watch;
} rn_inotify_event_t;

typedef struct rn_inotify_s {
	rn_sched_node_t node;
	size_t io_calls;
	size_t nb_watches;
	rn_inotify_watch_t *watches[500];
	char read_buffer[4096];
	rn_inotify_event_t event;
} rn_inotify_t;

rn_inotify_t *rn_inotify(rn_sched_t *sched);
void rn_inotify_destroy(rn_inotify_t *notify);
rn_inotify_watch_t *rn_inotify_add_watch(rn_inotify_t *inotify, const char *path, rn_inotify_type_t type, bool recursive);
int rn_inotify_rm_watch(rn_inotify_t *inotify, rn_inotify_watch_t *watch);
rn_inotify_event_t *rn_inotify_event(rn_inotify_t *inotify);

#endif /* !RINOO_FS_INOTIFY_H_ */
