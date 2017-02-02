/**
 * @file   buffer_class.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file for buffer class
 *
 *
 */

#ifndef RINOO_MEMORY_BUFFER_CLASS_H_
#define RINOO_MEMORY_BUFFER_CLASS_H_

struct rn_buffer_s;

typedef struct rn_buffer_class_s {
	size_t inisize;
	size_t maxsize;
	int (*init)(struct rn_buffer_s *buffer);
	size_t (*growthsize)(struct rn_buffer_s *buffer, size_t newsize);
	void *(*malloc)(struct rn_buffer_s *buffer, size_t size);
	void *(*realloc)(struct rn_buffer_s *buffer, size_t newsize);
	int (*free)(struct rn_buffer_s *buffer);
} rn_buffer_class_t;

#endif /* !RINOO_MEMORY_BUFFER_CLASS_H_ */
