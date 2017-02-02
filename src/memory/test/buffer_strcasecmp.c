/**
 * @file   rn_buffer_strcasecmp.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  rn_buffer_strcasecmp unit test
 *
 *
 */

#include "rinoo/rinoo.h"

char *strings[] = {
	"AAbb",
	"aac",
	"ba",
	"bAa",
	"bb",
	"c",
	"ccCCCCCCCCCCCCCCCCCccc"
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

	buffer1 = rn_buffer_create(NULL);
	XTEST(buffer1 != NULL);
	for (i = 0; i < ARRAY_SIZE(strings) - 1; i++) {
		rn_buffer_add(buffer1, strings[i], strlen(strings[i]));
		XTEST(rn_buffer_strcasecmp(buffer1, strings[i + 1]) < 0);
		rn_buffer_erase(buffer1, rn_buffer_size(buffer1));
	}
	rn_buffer_destroy(buffer1);
	XPASS();
}
