/**
 * @file   browse.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2014
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Easy directory browsing.
 *
 *
 */

#include "rinoo/fs/module.h"

static int rn_fs_stack_push(rn_fs_entry_t *entry, DIR *dirfd, const char *path)
{
	rn_fs_directory_t *directory;

	directory = calloc(1, sizeof(*directory));
	if (directory == NULL) {
		return -1;
	}
	directory->path = strdup(path);
	if (directory->path == NULL) {
		free(directory);
		return -1;
	}
	directory->fd = dirfd;
	rn_list_put(&entry->stack, &directory->stack_node);
	return 0;
}

static int rn_fs_stack_pop(rn_fs_entry_t *entry)
{
	rn_list_node_t *node;
	rn_fs_directory_t *directory;

	node = rn_list_pop(&entry->stack);
	if (node == NULL) {
		return -1;
	}
	directory = container_of(node, rn_fs_directory_t, stack_node);
	free(directory->path);
	free(directory);
	return 0;
}

static rn_fs_directory_t *rn_fs_stack_head(rn_fs_entry_t *entry)
{
	rn_list_node_t *node;
	rn_fs_directory_t *directory;

	node = rn_list_head(&entry->stack);
	if (node == NULL) {
		return NULL;
	}
	directory = container_of(node, rn_fs_directory_t, stack_node);
	return directory;
}

static void rn_fs_stack_destroy_node(rn_list_node_t *node)
{
	rn_fs_directory_t *directory;

	directory = container_of(node, rn_fs_directory_t, stack_node);
	closedir(directory->fd);
	free(directory->path);
	free(directory);
}

static void rn_fs_entry_destroy(rn_fs_entry_t *entry)
{
	if (entry == NULL) {
		return;
	}
	if (entry->entry != NULL) {
		free(entry->entry);
	}
	if (entry->path != NULL) {
		rn_buffer_destroy(entry->path);
	}
	rn_list_flush(&entry->stack, rn_fs_stack_destroy_node);
	free(entry);
}

static rn_fs_entry_t *rn_fs_entry(const char *path)
{
	long size;
	rn_fs_entry_t *entry;

	entry = calloc(1, sizeof(*entry));
	if (entry == NULL) {
		return NULL;
	}
	rn_list(&entry->stack, NULL);
	size = pathconf(path, _PC_NAME_MAX);
	if (size < 0) {
		size = 256;
	}
	size = offsetof(struct dirent, d_name) + size + 1;
	entry->entry = calloc(1, size);
	if (entry->entry == NULL) {
		goto entry_error;
	}
	entry->path = rn_buffer_create(NULL);
	if (entry->path == NULL) {
		goto entry_error;
	}
	if (stat(path, &entry->stat) != 0) {
		goto entry_error;
	}
	if (!S_ISDIR(entry->stat.st_mode)) {
		goto entry_error;
	}
	return entry;
entry_error:
	rn_fs_entry_destroy(entry);
	return NULL;
}

int rn_fs_browse(const char *path, rn_fs_entry_t **last_entry)
{
	DIR *dirfd;
	rn_fs_entry_t *curentry;
	struct dirent *result;
	rn_fs_directory_t *directory;

	if (last_entry == NULL) {
		return -1;
	}
	curentry = *last_entry;
	if (curentry == NULL) {
		curentry = rn_fs_entry(path);
		if (curentry == NULL) {
			return -1;
		}
		dirfd = opendir(path);
		if (dirfd == NULL) {
			goto browse_error;
		}
		if (rn_fs_stack_push(curentry, dirfd, path) != 0) {
			closedir(dirfd);
			goto browse_error;
		}
	}
	directory = rn_fs_stack_head(curentry);
	if (directory == NULL) {
		goto browse_error;
	}
	for (result = NULL; result == NULL && directory != NULL;) {
		if (readdir_r(directory->fd, curentry->entry, &result) != 0) {
			goto browse_error;
		}
		if (result == NULL) {
			closedir(directory->fd);
			rn_fs_stack_pop(curentry);
			directory = rn_fs_stack_head(curentry);
			result = NULL;
			continue;
		}
		if (curentry->entry->d_name[0] == '.' && (curentry->entry->d_name[1] == 0 || curentry->entry->d_name[1] == '.')) {
			result = NULL;
			continue;
		}
		rn_buffer_erase(curentry->path, 0);
		rn_buffer_addstr(curentry->path, directory->path);
		if (((char *) rn_buffer_ptr(curentry->path))[curentry->path->size - 1] != '/') {
			rn_buffer_addstr(curentry->path, "/");
		}
		rn_buffer_addstr(curentry->path, curentry->entry->d_name);
		rn_buffer_addnull(curentry->path);
		if (stat(rn_buffer_ptr(curentry->path), &curentry->stat) != 0) {
			/* Try next entry */
			result = NULL;
			continue;
		}
		if (S_ISDIR(curentry->stat.st_mode)) {
			dirfd = opendir(rn_buffer_ptr(curentry->path));
			if (dirfd == NULL) {
				/* Try next entry */
				result = NULL;
				continue;
			}
			if (rn_fs_stack_push(curentry, dirfd, rn_buffer_ptr(curentry->path)) != 0) {
				goto browse_error;
			}
		}
	}
	if (result == NULL && directory == NULL) {
		/* End of browsing */
		rn_fs_entry_destroy(curentry);
		*last_entry = NULL;
		return 0;
	}
	*last_entry = curentry;
	return 0;
browse_error:
	if (curentry != NULL) {
		rn_fs_entry_destroy(curentry);
	}
	return -1;
}
