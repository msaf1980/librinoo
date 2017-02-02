/**
 * @file   rn_vector_add.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2014
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  rn_vector_add unit test
 *
 *
 */

#include "rinoo/rinoo.h"

#if __WORDSIZE == 64
# define INT_TO_PTR(p) ((void *)(uint64_t)(p))
#else
# define INT_TO_PTR(p) ((void *)(p))
#endif


/**
 * Main function for this unit test
 *
 *
 * @return 0 if test passed
 */
int main()
{
	int i;
	size_t prev = 4;
	rn_vector_t vector = { 0 };

	for (i = 0; i < 1000; i++) {
		XTEST(rn_vector_add(&vector, INT_TO_PTR(i)) == 0);
		XTEST(vector.size == (size_t) i + 1);
		XTEST(vector.msize == prev * 2);
		if ((size_t) (i + 1) >= vector.msize) {
			prev = vector.msize;
		}
	}
	for (i = 0; i < 1000; i++) {
		XTEST(rn_vector_get(&vector, (uint32_t) i) == INT_TO_PTR(i));
	}
	XTEST(rn_vector_get(&vector, 1000) == NULL);
	rn_vector_destroy(&vector);
	XPASS();
}
