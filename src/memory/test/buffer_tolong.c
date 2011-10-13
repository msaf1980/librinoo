/**
 * @file   buffer_tolong.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Thu Apr  7 18:20:26 2011
 *
 * @brief  buffer_tolong unit test
 *
 *
 */

#include	"rinoo/rinoo.h"

void check_buf(t_buffer * buf, long int expected_result)
{
	size_t len;
	long int result;

	result = buffer_tolong(buf, NULL, 0);
	XTEST(result == expected_result);
	len = 0;
	result = buffer_tolong(buf, &len, 0);
	XTEST(result == expected_result);
	XTEST(len == buffer_len(buf));
	result = buffer_tolong(buf, NULL, 10);
	XTEST(result == expected_result);
	len = 0;
	result = buffer_tolong(buf, &len, 10);
	XTEST(result == expected_result);
	XTEST(len == buffer_len(buf));
}

/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int main()
{
	t_buffer *buf;
	t_buffer buf2;

	buf = buffer_create(10);
	XTEST(buf != NULL);
	XTEST(buffer_add(buf, "123456789", 9) == 9);
	check_buf(buf, 123456789);
	buffer_erase(buf, buffer_len(buf));
	XTEST(buffer_add(buf, "1", 1) == 1);
	check_buf(buf, 1);
	buffer_erase(buf, buffer_len(buf));
	XTEST(buffer_add(buf, "-12345678", 9) == 9);
	check_buf(buf, -12345678);
	buffer_erase(buf, buffer_len(buf));
	buffer_destroy(buf);
	strtobuffer(buf2, "987654321");
	check_buf(&buf2, 987654321);
	strtobuffer(buf2, "0");
	check_buf(&buf2, 0);
	strtobuffer(buf2, "-987654321");
	check_buf(&buf2, -987654321);
	XPASS();
}
