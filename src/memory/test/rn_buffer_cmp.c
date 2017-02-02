/**
 * @file   rn_buffer_cmp.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  rn_buffer_cmp unit test
 *
 *
 */

#include "rinoo/rinoo.h"

char *strings[] = {
	"aabb",
	"aac",
	"ba",
	"baa",
	"bb",
	"c",
	"cccccccccccccccccc"
};

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int main()
{
	uint32_t i;
	rn_buffer_t *buffer1;
	rn_buffer_t *buffer2;

	buffer1 = rn_buffer_create(NULL);
	XTEST(buffer1 != NULL);
	buffer2 = rn_buffer_create(NULL);
	XTEST(buffer2 != NULL);
	for (i = 0; i < ARRAY_SIZE(strings) - 1; i++) {
		rn_buffer_add(buffer1, strings[i], strlen(strings[i]));
		rn_buffer_add(buffer2, strings[i + 1], strlen(strings[i + 1]));
		XTEST(rn_buffer_cmp(buffer1, buffer2) < 0);
		rn_buffer_erase(buffer1, rn_buffer_size(buffer1));
		rn_buffer_erase(buffer2, rn_buffer_size(buffer2));
	}
	rn_buffer_destroy(buffer1);
	rn_buffer_destroy(buffer2);
	XPASS();
}
