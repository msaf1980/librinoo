/**
 * @file   rn_fs_browse.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2014
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Test file for rinoo fs browse.
 *
 *
 */

#include "rinoo/rinoo.h"

/**
 * Main function for this unit test.
 *
 *
 * @return 0 if test passed
 */
int main()
{
	rn_fs_entry_t *entry = NULL;

	while (rn_fs_browse(".", &entry) == 0 && entry != NULL) {
		rn_log("%s", rn_buffer_ptr(entry->path));
	}
	XPASS();
}
