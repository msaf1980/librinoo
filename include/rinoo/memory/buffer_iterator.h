/**
 * @file   buffer_iterator.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2014
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Buffer iterator header file
 *
 *
 */

#ifndef RINOO_MEMORY_BUFFER_ITERATOR_H_
#define RINOO_MEMORY_BUFFER_ITERATOR_H_

typedef struct rn_buffer_iterator_s {
	size_t offset;
	rn_buffer_t *buffer;
} rn_buffer_iterator_t;

#define RN_BUFFER_ITERATOR_GETTER(name, type)	\
static inline int buffer_iterator_get##name(rn_buffer_iterator_t *iterator, type *value) \
{ \
	if (iterator == NULL) { \
		return -1; \
	} \
	if (iterator->buffer == NULL) {\
		return -1; \
	} \
	if (rn_buffer_ptr(iterator->buffer) == NULL) { \
		return -1; \
	} \
	if (iterator->offset + sizeof(type) > rn_buffer_size(iterator->buffer)) { \
		return -1; \
	} \
	if (value != NULL) { \
		*value = *(type *) (rn_buffer_ptr(iterator->buffer) + iterator->offset); \
	} \
	iterator->offset += sizeof(type); \
	return 0; \
}

static inline void rn_buffer_iterator_set(rn_buffer_iterator_t *iterator, rn_buffer_t *buffer)
{
	iterator->offset = 0;
	iterator->buffer = buffer;
}

static inline int rn_buffer_iterator_position_set(rn_buffer_iterator_t *iterator, size_t pos)
{
	if (pos > rn_buffer_size(iterator->buffer)) {
		return -1;
	}
	iterator->offset = pos;
	return 0;
}

static inline int rn_buffer_iterator_position_inc(rn_buffer_iterator_t *iterator, size_t inc)
{
	if (iterator->offset + inc > rn_buffer_size(iterator->buffer)) {
		return -1;
	}
	iterator->offset += inc;
	return 0;
}

static inline size_t rn_buffer_iterator_position_get(rn_buffer_iterator_t *iterator)
{
	return iterator->offset;
}

static inline void *rn_buffer_iterator_ptr(rn_buffer_iterator_t *iterator)
{
	return rn_buffer_ptr(iterator->buffer) + iterator->offset;
}

static inline bool rn_buffer_iterator_end(rn_buffer_iterator_t *iterator)
{
	return (iterator->offset >= rn_buffer_size(iterator->buffer));
}

RN_BUFFER_ITERATOR_GETTER(short, short)
RN_BUFFER_ITERATOR_GETTER(ushort, unsigned short)
RN_BUFFER_ITERATOR_GETTER(int, int)
RN_BUFFER_ITERATOR_GETTER(uint, unsigned int)
RN_BUFFER_ITERATOR_GETTER(char, char)

static inline int rn_buffer_iterator_gethshort(rn_buffer_iterator_t *iterator, short *value)
{
	if (buffer_iterator_getshort(iterator, value) != 0) {
		return -1;
	}
	*value = ntohs(*value);
	return 0;
}

static inline int rn_buffer_iterator_gethushort(rn_buffer_iterator_t *iterator, unsigned short *value)
{
	if (buffer_iterator_getushort(iterator, value) != 0) {
		return -1;
	}
	*value = ntohs(*value);
	return 0;
}

static inline int rn_buffer_iterator_gethint(rn_buffer_iterator_t *iterator, int *value)
{
	if (buffer_iterator_getint(iterator, value) != 0) {
		return -1;
	}
	*value = ntohl(*value);
	return 0;
}

static inline int rn_buffer_iterator_gethuint(rn_buffer_iterator_t *iterator, unsigned int *value)
{
	if (buffer_iterator_getuint(iterator, value) != 0) {
		return -1;
	}
	*value = ntohl(*value);
	return 0;
}

#endif /* !RINOO_MEMORY_BUFFER_ITERATOR_H_ */
