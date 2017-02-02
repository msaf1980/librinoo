/**
 * @file   rn_buffer_tofloat.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  rn_buffer_tofloat unit test
 *
 *
 */

#include "rinoo/rinoo.h"

void check_buf(rn_buffer_t *buffer, float expected_result)
{
	size_t len;
	float result;

	result = rn_buffer_tofloat(buffer, NULL);
	XTEST(result == expected_result);
	len = 0;
	result = rn_buffer_tofloat(buffer, &len);
	XTEST(result == expected_result);
	XTEST(len == rn_buffer_size(buffer));
	result = rn_buffer_tofloat(buffer, NULL);
	XTEST(result == expected_result);
	len = 0;
	result = rn_buffer_tofloat(buffer, &len);
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
	XTEST(rn_buffer_add(buffer, "12345.6789", 10) == 10);
	check_buf(buffer, 12345.6789);
	rn_buffer_erase(buffer, rn_buffer_size(buffer));
	XTEST(rn_buffer_add(buffer, "1", 1) == 1);
	check_buf(buffer, 1);
	rn_buffer_erase(buffer, rn_buffer_size(buffer));
	XTEST(rn_buffer_add(buffer, "-12345.678", 10) == 10);
	check_buf(buffer, -12345.678);
	rn_buffer_erase(buffer, rn_buffer_size(buffer));
	rn_buffer_destroy(buffer);
	rn_strtobuffer(&buffer2, "98765.4321");
	check_buf(&buffer2, 98765.4321);
	rn_strtobuffer(&buffer2, "0");
	check_buf(&buffer2, 0);
	rn_strtobuffer(&buffer2, "-9876543.21");
	check_buf(&buffer2, -9876543.21);
	XPASS();
}
