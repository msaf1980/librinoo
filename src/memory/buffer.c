/**
 * @file   buffer.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Thu Dec  3 23:23:17 2009
 *
 * @brief  Contains functions to create a buffer (pointer & size)
 *
 *
 */

#include	"rinoo/rinoo.h"

/**
 * Creates a new buffer. It allocates the needed memory.
 *
 * @param init_size Initial size of the buffer.
 * @param maxsize Maximum size of the buffer.
 *
 * @return Pointer to the created buffer.
 */
t_buffer	*buffer_create(u32 init_size, u32 maxsize)
{
  t_buffer	*buf;

  XDASSERT(init_size <= maxsize || maxsize == 0, NULL);
  XDASSERT(init_size > 0, NULL);

  buf = xcalloc(1, sizeof(*buf));
  XASSERT(buf != NULL, NULL);
  buf->buf = xcalloc(1, init_size);
  if (buf->buf == NULL)
    {
      xfree(buf);
      XASSERT(0, NULL);
    }
  buf->len = 0;
  buf->size = init_size;
  if (likely(maxsize != 0))
    {
      buf->max_size = maxsize;
    }
  else
    {
      buf->max_size = BUFFER_DEFAULT_MAXSIZE;
    }
  return (buf);
}

/**
 * Destroys a buffer.
 *
 * @param buffer Pointer to the buffer to destroy.
 */
void		buffer_destroy(t_buffer *buf)
{
  XDASSERTN(buf != NULL);
  XDASSERTN(buf->buf != NULL);
  /* Means buf->buf has been allocated, see strtobuffer macro in buffer.h */
  XDASSERTN(buf->max_size > 0);

  xfree(buf->buf);
  xfree(buf);
}

/**
 * Extends a buffer. It trys to add 'size' + BUFFER_INCREMENT of memory.
 * This function fails if the buffer maximum size is reached.
 *
 * @param buf Pointer to the buffer to extend.
 * @param size New desired size.
 *
 * @return 1 if succeeds, 0 if the maximum size is reached, -1 on error.
 */
int		buffer_extend(t_buffer *buf, size_t size)
{
  int		newsize;
  void		*newbuf;

  XDASSERT(buf != NULL, -1);
  XDASSERT(buf->buf != NULL, -1);

  if (buf->size == buf->max_size)
    return (0);
  size = (size > BUFFER_INCREMENT ?
	  size + BUFFER_INCREMENT : BUFFER_INCREMENT);
  newsize = (buf->size + size > buf->max_size ?
	     buf->max_size : buf->size + size);
  newbuf = xrealloc(buf->buf, newsize);
 /* if realloc fails, buf->buf is untouched, we'll keep it */
  if (newbuf == NULL)
    XASSERT(0, -1);
  buf->buf = newbuf;
  buf->size = newsize;
  return (1);
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
int		buffer_vprint(t_buffer *buf, const char *format, va_list ap)
{
  int		res;
  va_list	ap2;

  XDASSERT(buf != NULL, -1);
  XDASSERT(buf->buf != NULL, -1);

  va_copy(ap2, ap);
  while (((u32)(res = vsnprintf(buf->buf + buf->len,
				buf->size - buf->len,
				format, ap2)) >= buf->size - buf->len) &&
	 buffer_extend(buf, res) == 1)
    {
      va_end(ap2);
      va_copy(ap2, ap);
    }
  va_end(ap2);
  buf->len += res;
  return (res);
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
int		buffer_print(t_buffer *buf, const char *format, ...)
{
  int		res;
  va_list	ap;

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
int		buffer_add(t_buffer *buf, const char *data, size_t size)
{
  XDASSERT(buf != NULL, -1);
  XDASSERT(buf->buf != NULL, -1);
  XDASSERT(data != NULL, -1);

  if (size + buf->len > buf->size && buffer_extend(buf, size + buf->len) != 1)
    return (-1);
  memcpy(buf->buf + buf->len, data, size);
  buf->len += size;
  return (size);
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
int		buffer_addstr(t_buffer *buf, const char *str)
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
int		buffer_addnull(t_buffer *buf)
{
  if (buffer_add(buf, "\0", 1) < 0)
    {
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
 * @return 1 if data has been erased, else 0.
 */
int		buffer_erase(t_buffer *buf, u32 len)
{
  XDASSERT(buf != NULL, 0);
  XDASSERT(buf->buf != NULL, 0);

  if (buf->len > 0)
    {
      if (len > buf->len)
	len = buf->len;
      if (len < buf->len)
	{
	  memmove(buf->buf, buf->buf + len, buf->size - len);
	}
      bzero(buf->buf + (buf->size - len), len);
      buf->len -= len;
      return (1);
    }
  return (0);
}

/**
 * Duplicates a buffer.
 *
 * @param buf Pointer to the buffer to duplicate.
 *
 * @return A pointer to the new buffer, or NULL if an error occurs.
 */
t_buffer	*buffer_dup(t_buffer *buf)
{
  t_buffer	*newbuf;

  XDASSERT(buf != NULL, NULL);
  XDASSERT(buf->buf != NULL, NULL);

  newbuf = xcalloc(1, sizeof(*newbuf));
  XASSERT(newbuf != NULL, NULL);
  newbuf->len = buf->len;
  newbuf->size = buf->size;
  newbuf->max_size = buf->max_size;
  if (unlikely(newbuf->size) == 0)
    {
      newbuf->size = BUFFER_INCREMENT;
    }
  if (newbuf->max_size == 0)
    {
      newbuf->max_size = BUFFER_DEFAULT_MAXSIZE;
    }
  newbuf->buf = xcalloc(1, newbuf->size);
  if (newbuf->buf == NULL)
    {
      xfree(newbuf);
      XASSERT(0, NULL);
    }
  memcpy(newbuf->buf, buf->buf, buf->len);
  return newbuf;
}

/**
 * Compares a buffer with a string.
 *
 * @param buf Pointer to a buffer.
 * @param str Pointer to a string.
 *
 * @return An integer less than, equal to, or greater than zero if buf is found, respectively, to be less than, to match, or be greater than s2
 */
int		buffer_strcmp(t_buffer *buf, const char *str)
{
  u32		size;

  XASSERT(buf != NULL, -1);
  XASSERT(str != NULL, -1);

  size = strlen(str);
  if (buffer_len(buf) < size)
    {
      return -str[buffer_len(buf)];
    }
  if (buffer_len(buf) > size)
    {
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
int		buffer_strncmp(t_buffer *buf, const char *str, size_t len)
{
  u32		i;

  XASSERT(buf != NULL, -1);
  XASSERT(str != NULL, -1);

  for (i = 0; i < len && i < buffer_len(buf); i++)
    {
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
long int	buffer_tolong(t_buffer *buf, size_t *len, int base)
{
  long int	result;
  char		*endptr;
  t_buffer	*workbuf;

  workbuf = buf;
  if (buf->max_size == 0)
    {
      /* Considering buf->buf has not been allocated */
      workbuf = buffer_dup(buf);
    }
  result = 0;
  endptr = workbuf->buf;
  if (buffer_addnull(workbuf) == 0)
    {
      result = strtol(workbuf->buf, &endptr, base);
    }
  if (len != NULL)
    {
      *len = endptr - workbuf->buf;
    }
  if (workbuf != buf)
    {
      buffer_destroy(workbuf);
    }
  else
    {
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
unsigned long int	buffer_toulong(t_buffer *buf, size_t *len, int base)
{
  unsigned long int	result;
  char			*endptr;
  t_buffer		*workbuf;

  workbuf = buf;
  if (buf->max_size == 0)
    {
      /* Considering buf->buf has not been allocated */
      workbuf = buffer_dup(buf);
    }
  result = 0;
  endptr = workbuf->buf;
  if (buffer_addnull(workbuf) == 0)
    {
      result = strtoul(workbuf->buf, &endptr, base);
    }
  if (len != NULL)
    {
      *len = endptr - workbuf->buf;
    }
  if (workbuf != buf)
    {
      buffer_destroy(workbuf);
    }
  else
    {
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
float		buffer_tofloat(t_buffer *buf, size_t *len)
{
  float		result;
  char		*endptr;
  t_buffer	*workbuf;

  workbuf = buf;
  if (buf->max_size == 0)
    {
      /* Considering buf->buf has not been allocated */
      workbuf = buffer_dup(buf);
    }
  result = 0;
  endptr = workbuf->buf;
  if (buffer_addnull(workbuf) == 0)
    {
      result = strtof(workbuf->buf, &endptr);
    }
  if (len != NULL)
    {
      *len = endptr - workbuf->buf;
    }
  if (workbuf != buf)
    {
      buffer_destroy(workbuf);
    }
  else
    {
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
double		buffer_todouble(t_buffer *buf, size_t *len)
{
  double	result;
  char		*endptr;
  t_buffer	*workbuf;

  workbuf = buf;
  if (buf->max_size == 0)
    {
      /* Considering buf->buf has not been allocated */
      workbuf = buffer_dup(buf);
    }
  result = 0;
  endptr = workbuf->buf;
  if (buffer_addnull(workbuf) == 0)
    {
      result = strtod(workbuf->buf, &endptr);
    }
  if (len != NULL)
    {
      *len = endptr - workbuf->buf;
    }
  if (workbuf != buf)
    {
      buffer_destroy(workbuf);
    }
  else
    {
      /* Removing null byte */
      buf->len--;
    }
  return result;
}
