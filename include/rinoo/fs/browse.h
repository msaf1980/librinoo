/**
 * @file   browse.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2014
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file for easy directory browsing.
 *
 *
 */

#ifndef RINOO_FS_BROWSE_H_
#define RINOO_FS_BROWSE_H_

typedef struct rn_fs_directory_s {
	DIR *fd;
	char *path;
	rn_list_node_t stack_node;
} rn_fs_directory_t;

typedef struct rn_fs_entry_s {
	rn_buffer_t *path;
	struct stat stat;
	struct dirent *entry;
	rn_list_t stack;
} rn_fs_entry_t;

int rn_fs_browse(const char *path, rn_fs_entry_t **last_entry);

#endif /* !RINOO_FS_BROWSE_H_ */
