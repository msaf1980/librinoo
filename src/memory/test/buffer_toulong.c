/**
 * @file   rn_buffer_toulong.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  rn_buffer_toulong unit test
 *
 *
 */

#include "rinoo/rinoo.h"

void check_buf(rn_buffer_t *buffer, unsigned long int expected_result)
{
	size_t len;
	unsigned long int result;

	result = rn_buffer_toulong(buffer, NULL, 0);
	XTEST(result == expected_result);
	len = 0;
	result = rn_buffer_toulong(buffer, &len, 0);
	XTEST(result == expected_result);
	XTEST(len == rn_buffer_size(buffer));
	result = rn_buffer_toulong(buffer, NULL, 10);
	XTEST(result == expected_result);
	len = 0;
	result = rn_buffer_toulong(buffer, &len, 10);
	XTEST(result == expected_result);
	XTEST(len == rn_buffer_size(buffer));
}

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int main()
{
	rn_buffer_t *buffer;
	rn_buffer_t buffer2;

	buffer = rn_buffer_create(NULL);
	XTEST(buffer != NULL);
	XTEST(rn_buffer_add(buffer, "123456789", 9) == 9);
	check_buf(buffer, 123456789);
	rn_buffer_erase(buffer, rn_buffer_size(buffer));
	XTEST(rn_buffer_add(buffer, "1", 1) == 1);
	check_buf(buffer, 1);
	rn_buffer_destroy(buffer);
	rn_strtobuffer(&buffer2, "987654321");
	check_buf(&buffer2, 987654321);
	rn_strtobuffer(&buffer2, "0");
	check_buf(&buffer2, 0);
	XPASS();
}
