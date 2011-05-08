/**
 * @file   buffer.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Tue Dec 15 02:20:19 2009
 *
 * @brief  Header file for buffer management
 *
 *
 */

#ifndef		RINOO_BUFFER_H_
# define	RINOO_BUFFER_H_

# define	BUFFER_INCREMENT	2048
# define	BUFFER_DEFAULT_MAXSIZE	1024 * 1024

typedef struct	s_buffer
{
  char		*buf;		/**< Pointer to the data */
  u32		len;		/**< Data length */
  u32		size;		/**< Buffer size */
  u32		max_size;	/**< Buffer maximum size */
}		t_buffer;

# define	buffer_ptr(ptr)				(ptr)->buf
# define	buffer_len(ptr)				(ptr)->len
# define	buffer_size(ptr)			(ptr)->size
# define	buffer_isfull(ptr)			((ptr)->len == (ptr)->size)
# define	buffer_setlen(ptr, length)		(ptr)->len = length
# define	buffer_static(buffer, ptr, length)	do { buffer.buf = (char *) (ptr); buffer.len = (length); buffer.size = buffer.len; buffer.max_size = 0; } while (0)
# define	strtobuffer(buffer, str)		do { buffer.buf = (char *) (str); buffer.len = strlen((str)); buffer.size = buffer.len; buffer.max_size = 0; } while (0)

t_buffer		*buffer_create(u32 init_size, u32 maxsize);
void			buffer_destroy(t_buffer *ptr);
int			buffer_extend(t_buffer *buf, size_t size);
int			buffer_vprint(t_buffer *buf, const char *format, va_list ap);
int			buffer_print(t_buffer *buf, const char *format, ...);
int			buffer_add(t_buffer *buf, const char *data, size_t size);
int			buffer_addstr(t_buffer *buf, const char *str);
int			buffer_addnull(t_buffer *buf);
int			buffer_erase(t_buffer *buf, u32 len);
t_buffer		*buffer_dup(t_buffer *buf);
int			buffer_strcmp(t_buffer *buf, const char *str);
int			buffer_strncmp(t_buffer *buf, const char *str, size_t len);
long int		buffer_tolong(t_buffer *buf, size_t *len, int base);
unsigned long int	buffer_toulong(t_buffer *buf, size_t *len, int base);
float			buffer_tofloat(t_buffer *buf, size_t *len);
double			buffer_todouble(t_buffer *buf, size_t *len);
char			*buffer_tostr(t_buffer *buf);

#endif		/* !RINOO_BUFFER_H_ */
