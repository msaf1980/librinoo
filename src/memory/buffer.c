/**
 * @file   buffer.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2010
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

  XDASSERT(init_size <= maxsize, NULL);

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
  buf->max_size = maxsize;
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

  xfree(buf->buf);
  xfree(buf);
}

/**
 * Extends a buffer. It trys to add BUFFER_INCREMENT memory.
 * This function fails if the buffer maximum size is reached.
 *
 * @param buf Pointer to the buffer to extend.
 *
 * @return 1 if succeeds, 0 if the maximum size is reached, -1 on error.
 */
int		buffer_extend(t_buffer *buf)
{
  int		newsize;
  void		*newbuf;

  XDASSERT(buf != NULL, -1);
  XDASSERT(buf->buf != NULL, -1);

  if (buf->size == buf->max_size)
    return (0);
  newsize = (buf->size + BUFFER_INCREMENT > buf->max_size) ? buf->max_size : buf->size + BUFFER_INCREMENT;
  newbuf = xrealloc(buf->buf, newsize);
  if (newbuf == NULL) /* if realloc fails, buf->buf is untouched, we'll keep it */
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
	 buffer_extend(buf) == 1)
    {
      va_end(ap2);
      va_copy(ap2, ap);
    }
  va_end(ap2);
  buf->len += res;
  return (res);
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

  if (size + buf->len > buf->size && buffer_extend(buf) != 1)
    return (-1);
  memcpy(buf->buf + buf->len, data, size);
  buf->len += size;
  return (size);
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
      memmove(buf->buf, buf->buf + len, buf->size - len);
      bzero(buf->buf + (buf->size - len), len);
      buf->len -= len;
      return (1);
    }
  return (0);
}
