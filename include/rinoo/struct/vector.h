/**
 * @file   vector.h
 * @author Reginald Lips <reginald.@gmail.com> - Copyright 2014
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Vector structure
 *
 *
 */

#ifndef RINOO_STRUCT_VECTOR_H_
#define RINOO_STRUCT_VECTOR_H_

typedef struct rn_vector_s {
	size_t size;
	size_t msize;
	void **ptr;
} rn_vector_t;

int rn_vector_add(rn_vector_t *vector, void *item);
void rn_vector_destroy(rn_vector_t *vector);
int rn_vector_remove(rn_vector_t *vector, uint32_t i);
void *rn_vector_get(rn_vector_t *vector, uint32_t i);
size_t rn_vector_size(rn_vector_t *vector);

#endif /* !RINOO_STRUCT_VECTOR_H_ */

