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

char *strings_lg[] = {
	"aabb",
	"aac",
	"ba",
	"baa",
	"bb",
	"c",
	"cccccccccccccccccc"
};

char *strings_case_eq[] = {
	"aabb",
	"aaBB",
	"Ba",
	"ba",
	"bb",
	"Bb",
	"abcde",
	"abcde"
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
	for (i = 0; i < ARRAY_SIZE(strings_lg) - 1; i++) {
		rn_buffer_add(buffer1, strings_lg[i], strlen(strings_lg[i]));
		rn_buffer_add(buffer2, strings_lg[i + 1], strlen(strings_lg[i + 1]));
		XTEST(rn_buffer_cmp(buffer1, buffer2) < 0);
		XTEST(rn_buffer_casecmp(buffer1, buffer2) < 0);
		rn_buffer_reset(buffer1);
		rn_buffer_reset(buffer2);
	}
	for (i = 0; i < ARRAY_SIZE(strings_case_eq) - 1; i += 2) {
		rn_buffer_add(buffer1, strings_case_eq[i], strlen(strings_case_eq[i]));
		rn_buffer_add(buffer2, strings_case_eq[i + 1], strlen(strings_case_eq[i + 1]));
		XTEST(rn_buffer_casecmp(buffer1, buffer2) == 0);
		rn_buffer_reset(buffer1);
		rn_buffer_reset(buffer2);
	}
	rn_buffer_destroy(buffer1);
	rn_buffer_destroy(buffer2);
	XPASS();
}
