/**
 * @file   buffer.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file for buffer management
 *
 *
 */

#ifndef RINOO_MEMORY_BUFFER_H_
#define RINOO_MEMORY_BUFFER_H_

#define RN_BUFFER_INCREMENT	2048

typedef struct rn_buffer_s {
	void *ptr;
	size_t size;
	size_t msize;
	rn_buffer_class_t *class;
} rn_buffer_t;

#define rn_buffer_ptr(buffer)			((buffer)->ptr)
#define rn_buffer_size(buffer)			((buffer)->size)
#define rn_buffer_msize(buffer)			((buffer)->msize)
#define rn_buffer_isfull(buffer)			((buffer)->size == (buffer)->msize || (buffer)->msize == 0)
#define rn_buffer_setsize(buffer, newsize)		do { (buffer)->size = newsize; } while (0)
#define rn_strtobuffer(buffer, str)		do { rn_buffer_static(buffer, (void *)(str), strlen(str)); } while (0)

rn_buffer_t *rn_buffer_create(rn_buffer_class_t *class);
void rn_buffer_static(rn_buffer_t *buffer, void *ptr, size_t size);
void rn_buffer_set(rn_buffer_t *buffer, void *ptr, size_t msize);
int rn_buffer_destroy(rn_buffer_t *buffer);
int rn_buffer_extend(rn_buffer_t *buffer, size_t size);
int rn_buffer_vprint(rn_buffer_t *buffer, const char *format, va_list ap);
int rn_buffer_print(rn_buffer_t *buffer, const char *format, ...);
int rn_buffer_add(rn_buffer_t *buffer, const char *data, size_t size);
int rn_buffer_addstr(rn_buffer_t *buffer, const char *str);
int rn_buffer_addnull(rn_buffer_t *buf);
int rn_buffer_erase(rn_buffer_t *buffer, size_t size);
rn_buffer_t *rn_buffer_dup(rn_buffer_t *buffer);
int rn_buffer_cmp(rn_buffer_t *buffer1, rn_buffer_t *buffer2);
int rn_buffer_strcmp(rn_buffer_t *buffer, const char *str);
int rn_buffer_strncmp(rn_buffer_t *buffer, const char *str, size_t len);
int rn_buffer_strcasecmp(rn_buffer_t *buffer, const char *str);
int rn_buffer_strncasecmp(rn_buffer_t *buffer, const char *str, size_t len);
long int rn_buffer_tolong(rn_buffer_t *buffer, size_t *len, int base);
unsigned long int rn_buffer_toulong(rn_buffer_t *buffer, size_t *len, int base);
float rn_buffer_tofloat(rn_buffer_t *buffer, size_t *len);
double rn_buffer_todouble(rn_buffer_t *buffer, size_t *len);
char *rn_buffer_tostr(rn_buffer_t *buffer);

#endif /* !RINOO_MEMORY_BUFFER_H_ */
