/**
 * @file   buffer.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2010
 * @date   Tue Dec 15 02:20:19 2009
 *
 * @brief  Header file for buffer management
 *
 *
 */

#ifndef		RINOO_BUFFER_H_
# define	RINOO_BUFFER_H_

# define	BUFFER_INCREMENT	2048

typedef struct	s_buffer
{
  char		*buf;		/**< Pointer to the data */
  u32		len;		/**< Data length */
  u32		size;		/**< Buffer size */
  u32		max_size;	/**< Buffer maximum size */
}		t_buffer;

# define	buffer_ptr(ptr)			(ptr)->buf
# define	buffer_len(ptr)			(ptr)->len
# define	buffer_size(ptr)		(ptr)->size
# define	buffer_isfull(ptr)		((ptr)->len == (ptr)->size)
# define	buffer_setlen(ptr, length)	(ptr)->len = length

t_buffer	*buffer_create(u32 init_size, u32 maxsize);
void		buffer_destroy(t_buffer *ptr);
int		buffer_extend(t_buffer *buf, size_t size);
int		buffer_vprint(t_buffer *buf, const char *format, va_list ap);
int		buffer_add(t_buffer *buf, const char *data, size_t size);
int		buffer_erase(t_buffer *buf, u32 len);

#endif		/* !RINOO_BUFFER_H_ */
