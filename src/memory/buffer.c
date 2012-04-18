/**
 * @file   buffer.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Thu Dec  3 23:23:17 2009
 *
 * @brief  Contains functions to create a buffer (pointer & size)
 *
 *
 */

#include "rinoo/rinoo.h"

/**
 * Creates a new buffer. It allocates the needed memory.
 *
 * @param init_size Initial size of the buffer.
 *
 * @return Pointer to the created buffer.
 */
t_buffer *buffer_create(u32 init_size)
{
	t_buffer *buf;

	XASSERT(init_size > 0, NULL);

	buf = calloc(1, sizeof(*buf));
	XASSERT(buf != NULL, NULL);
	buf->buf = calloc(1, init_size);
	if (buf->buf == NULL) {
		free(buf);
		XASSERT(0, NULL);
	}
	buf->len = 0;
	buf->size = init_size;
	buf->alloc = 1;
	return buf;
}

/**
 * Destroys a buffer.
 *
 * @param buffer Pointer to the buffer to destroy.
 */
void buffer_destroy(t_buffer *buf)
{
	XASSERTN(buf != NULL);
	XASSERTN(buf->buf != NULL);

	if (buf->alloc != 0) {
		buf->alloc = 0;
		free(buf->buf);
	}
	buf->buf = NULL;
	free(buf);
}

/**
 * Extends a buffer. It tries to set new size to (size * 2).
 *
 * @param buf Pointer to the buffer to extend.
 * @param size New desired size.
 *
 * @return 0 if succeeds, -1 if an error occurs.
 */
int buffer_extend(t_buffer *buf, size_t size)
{
	void *newbuf;

	XASSERT(buf != NULL, -1);
	XASSERT(buf->buf != NULL, -1);
	XASSERT(buf->alloc == 1, -1);

	if (buf->size >= size) {
		return 0;
	}
	size = 2 * (size > RINOO_BUFFER_INCREMENT ? size : RINOO_BUFFER_INCREMENT);
	newbuf = realloc(buf->buf, size);
	/* if realloc fails, buf->buf is untouched, we'll keep it */
	if (unlikely(newbuf == NULL)) {
		return -1;
	}
	buf->buf = newbuf;
	buf->size = size;
	return 0;
}

/**
 * It is vprintf-like function which tries to add data to the buffer
 * and it will try to extend this buffer if it is to small.
 * This function uses vsnprintf, you can find how to use the format
 * string with man vsnprintf.
 *
 * @param buf Pointer to the buffer to add data to.
 * @param format Format string which defines subsequent arguments.
 * @param ap Vararg list.
 *
 * @return Number of bytes printed if succeeds, else -1.
 */
int buffer_vprint(t_buffer *buf, const char *format, va_list ap)
{
	int res;
	va_list ap2;

	XASSERT(buf != NULL, -1);
	XASSERT(buf->buf != NULL, -1);
	XASSERT(buf->alloc == 1, -1);

	va_copy(ap2, ap);
	while (((u32) (res = vsnprintf(buf->buf + buf->len,
				       buf->size - buf->len,
				       format, ap2)) >= buf->size - buf->len) &&
	       buffer_extend(buf, buf->len + res + 1) == 0) {
		va_end(ap2);
		va_copy(ap2, ap);
	}
	va_end(ap2);
	buf->len += res;
	return res;
}

/**
 * It is printf-like function which tries to add data to the buffer
 * and it will try to extend this buffer if it is to small.
 * This function uses buffer_vprint.
 *
 * @param buf Pointer to the buffer to add data to.
 * @param format Format string which defines subsequent arguments.
 *
 * @return Number of bytes printed if succeeds, else -1.
 */
int buffer_print(t_buffer *buf, const char *format, ...)
{
	int res;
	va_list ap;

	va_start(ap, format);
	res = buffer_vprint(buf, format, ap);
	va_end(ap);
	return res;
}

/**
 * Adds data to a buffer. If the buffer is to small, this function
 * will try to extend it.
 *
 * @param buf Buffer where the data will be added.
 * @param data Data to add to the buffer.
 * @param size Size of data.
 *
 * @return size if data is added to the buffer, else -1.
 */
int buffer_add(t_buffer *buf, const char *data, size_t size)
{
	XASSERT(buf != NULL, -1);
	XASSERT(buf->buf != NULL, -1);
	XASSERT(buf->alloc == 1, -1);
	XASSERT(data != NULL, -1);

	if (size + buf->len > buf->size &&
	    buffer_extend(buf, size + buf->len) < 0) {
		return -1;
	}
	memcpy(buf->buf + buf->len, data, size);
	buf->len += size;
	return size;
}

/**
 * Adds a string to a buffer. It actually calls buffer_add with strlen
 * of str as size parameter.
 *
 * @param buf Buffer where the string will be added
 * @param str String to add to the buffer
 *
 * @return Number of bytes added on success, or -1 if an error occurs
 */
int buffer_addstr(t_buffer *buf, const char *str)
{
	return buffer_add(buf, str, strlen(str));
}

/**
 * Adds a null byte to the end of a buffer.
 *
 * @param buf Buffer where the null byte will be added
 *
 * @return 0 on success, or -1 if an error occurs
 */
int buffer_addnull(t_buffer *buf)
{
	if ((buf->len == 0 || buf->buf[buf->len - 1] != 0) &&
	    buffer_add(buf, "\0", 1) < 0) {
		return -1;
	}
	return 0;
}

/**
 * Erases beginning data in the buffer and moves the rest
 * to the beginning. This function does -not- reduce the buffer.
 *
 * @param buf Buffer where the data will be erased.
 * @param len Amount of data erased ('len' first bytes).
 *
 * @return 0 if data has been erased, else -1.
 */
int buffer_erase(t_buffer *buf, u32 len)
{
	XASSERT(buf != NULL, -1);
	XASSERT(buf->buf != NULL, -1);

	if (buf->len > 0) {
		if (len > buf->len)
			len = buf->len;
		if (len < buf->len) {
			memmove(buf->buf, buf->buf + len, buf->size - len);
		}
		bzero(buf->buf + (buf->size - len), len);
		buf->len -= len;
		return 0;
	}
	return -1;
}

/**
 * Duplicates a buffer.
 *
 * @param buf Pointer to the buffer to duplicate.
 *
 * @return A pointer to the new buffer, or NULL if an error occurs.
 */
t_buffer *buffer_dup(t_buffer *buf)
{
	t_buffer *newbuf;

	XASSERT(buf != NULL, NULL);
	XASSERT(buf->buf != NULL, NULL);

	newbuf = calloc(1, sizeof(*newbuf));
	if (unlikely(newbuf == NULL)) {
		return NULL;
	}
	newbuf->len = buf->len;
	newbuf->size = buf->size;
	if (unlikely(newbuf->size) == 0) {
		newbuf->size = RINOO_BUFFER_INCREMENT;
	}
	newbuf->buf = calloc(1, newbuf->size);
	if (unlikely(newbuf->buf == NULL)) {
		free(newbuf);
		return NULL;
	}
	memcpy(newbuf->buf, buf->buf, buf->len);
	newbuf->alloc = 1;
	return newbuf;
}

/**
 * Compares two buffers
 *
 * @param buf1 Pointer to a buffer.
 * @param buf2 Pointer to a buffer.
 *
 * @return An integer less than, equal to, or greater than zero if buf1 is found, respectively, to be less than, to match, or be greater than buf2
 */
int buffer_cmp(t_buffer *buf1, t_buffer *buf2)
{
	int ret;

	ret = buffer_len(buf1) - buffer_len(buf2);
	if (ret != 0) {
		return ret;
	}
	ret = memcmp(buffer_ptr(buf1), buffer_ptr(buf2), buffer_len(buf1));
	if (ret != 0) {
		return ret;
	}
	return 0;

}

/**
 * Compares a buffer with a string.
 *
 * @param buf Pointer to a buffer.
 * @param str Pointer to a string.
 *
 * @return An integer less than, equal to, or greater than zero if buf is found, respectively, to be less than, to match, or be greater than s2
 */
int buffer_strcmp(t_buffer *buf, const char *str)
{
	u32 size;

	XASSERT(buf != NULL, -1);
	XASSERT(str != NULL, -1);

	size = strlen(str);
	if (buffer_len(buf) < size) {
		return -str[buffer_len(buf)];
	}
	if (buffer_len(buf) > size) {
		return buf->buf[size];
	}
	return memcmp(buffer_ptr(buf), str, size);
}

/**
 * Compares a buffer with n bytes of a string.
 *
 * @param buf Pointer to a buffer.
 * @param str Pointer to a string.
 * @param len Maximum length of the string.
 *
 * @return An integer less than, equal to, or greater than zero if buf is found, respectively, to be less than, to match, or be greater than s2
 */
int buffer_strncmp(t_buffer *buf, const char *str, size_t len)
{
	u32 i;

	XASSERT(buf != NULL, -1);
	XASSERT(str != NULL, -1);

	for (i = 0; i < len && i < buffer_len(buf); i++) {
		if (buf->buf[i] != str[i] || str[i] == 0)
			return buf->buf[i] - str[i];
	}
	if (i < len && i >= buffer_len(buf))
		return -buf->buf[buffer_len(buf) - 1];
	return 0;
}

/**
 * Converts a buffer to a long int accordingly to strtol.
 *
 * @param buf Pointer to a buffer to convert.
 * @param len If not NULL, it stores the buffer length processed for conversion.
 * @param base Conversion base.
 *
 * @return Result of conversion.
 */
long int buffer_tolong(t_buffer *buf, size_t *len, int base)
{
	long int result;
	char *endptr;
	t_buffer *workbuf;

	XASSERT(buf != NULL, 0);
	XASSERT(buf->buf != NULL, 0);

	workbuf = buf;
	if (buf->alloc == 0) {
		/* Considering buf->buf has not been allocated */
		workbuf = buffer_dup(buf);
	}
	result = 0;
	endptr = workbuf->buf;
	if (buffer_addnull(workbuf) == 0) {
		result = strtol(workbuf->buf, &endptr, base);
	}
	if (len != NULL) {
		*len = endptr - workbuf->buf;
	}
	if (workbuf != buf) {
		buffer_destroy(workbuf);
	} else {
		/* Removing null byte */
		buf->len--;
	}
	return result;
}

/**
 * Converts a buffer to an unsigned long int accordingly to strtoul.
 *
 * @param buf Pointer to a buffer to convert.
 * @param len If not NULL, it stores the buffer length processed for conversion.
 * @param base Conversion base.
 *
 * @return Result of conversion.
 */
unsigned long int buffer_toulong(t_buffer *buf, size_t *len, int base)
{
	char *endptr;
	t_buffer *workbuf;
	unsigned long int result;

	XASSERT(buf != NULL, 0);
	XASSERT(buf->buf != NULL, 0);

	workbuf = buf;
	if (buf->alloc == 0) {
		/* Considering buf->buf has not been allocated */
		workbuf = buffer_dup(buf);
	}
	result = 0;
	endptr = workbuf->buf;
	if (buffer_addnull(workbuf) == 0) {
		result = strtoul(workbuf->buf, &endptr, base);
	}
	if (len != NULL) {
		*len = endptr - workbuf->buf;
	}
	if (workbuf != buf) {
		buffer_destroy(workbuf);
	} else {
		/* Removing null byte */
		buf->len--;
	}
	return result;
}

/**
 * Converts a buffer to a float accordingly to strtof.
 *
 * @param buf Pointer to a buffer to convert.
 * @param len If not NULL, it stores the buffer length processed for conversion.
 *
 * @return Result of conversion.
 */
float buffer_tofloat(t_buffer *buf, size_t *len)
{
	float result;
	char *endptr;
	t_buffer *workbuf;

	XASSERT(buf != NULL, 0);
	XASSERT(buf->buf != NULL, 0);

	workbuf = buf;
	if (buf->alloc == 0) {
		/* Considering buf->buf has not been allocated */
		workbuf = buffer_dup(buf);
	}
	result = 0;
	endptr = workbuf->buf;
	if (buffer_addnull(workbuf) == 0) {
		result = strtof(workbuf->buf, &endptr);
	}
	if (len != NULL) {
		*len = endptr - workbuf->buf;
	}
	if (workbuf != buf) {
		buffer_destroy(workbuf);
	} else {
		/* Removing null byte */
		buf->len--;
	}
	return result;
}

/**
 * Converts a buffer to a double accordingly to strtod.
 *
 * @param buf Pointer to a buffer to convert.
 * @param len If not NULL, it stores the buffer length processed for conversion.
 *
 * @return Result of conversion.
 */
double buffer_todouble(t_buffer *buf, size_t *len)
{
	double result;
	char *endptr;
	t_buffer *workbuf;

	XASSERT(buf != NULL, 0);
	XASSERT(buf->buf != NULL, 0);

	workbuf = buf;
	if (buf->alloc == 0) {
		/* Considering buf->buf has not been allocated */
		workbuf = buffer_dup(buf);
	}
	result = 0;
	endptr = workbuf->buf;
	if (buffer_addnull(workbuf) == 0) {
		result = strtod(workbuf->buf, &endptr);
	}
	if (len != NULL) {
		*len = endptr - workbuf->buf;
	}
	if (workbuf != buf) {
		buffer_destroy(workbuf);
	} else {
		/* Removing null byte */
		buf->len--;
	}
	return result;
}

/**
 * Converts a buffer into a string.
 * It makes sure the buffer ends with \0 and returns the internal buffer pointer.
 * Buffer length changes, thus.
 *
 * @param buf Pointer to the buffer to convert.
 *
 * @return A pointer to a string or NULL if an error occurs.
 */
char *buffer_tostr(t_buffer *buf)
{
	if (buffer_addnull(buf) != 0) {
		return NULL;
	}
	return buf->buf;
}
