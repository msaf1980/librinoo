/**
 * @file   vector.h
 * @author Reginald Lips <reginald.@gmail.com> - Copyright 2014
 * @date   Wed Apr 30 18:48:12 2014
 *
 * @brief  Vector structure
 *
 *
 */

#ifndef RINOO_STRUCT_VECTOR_H_
#define RINOO_STRUCT_VECTOR_H_

typedef struct s_rinoovector {
	size_t size;
	size_t msize;
	void **ptr;
} t_rinoovector;

int rinoovector_add(t_rinoovector *vector, void *item);
void rinoovector_destroy(t_rinoovector *vector);
int rinoovector_remove(t_rinoovector *vector, uint32_t i);
void *rinoovector_get(t_rinoovector *vector, uint32_t i);
size_t rinoovector_size(t_rinoovector *vector);

#endif /* !RINOO_STRUCT_VECTOR_H_ */

