/**
 * @file   stdmem.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2010
 * @date   Mon Oct 26 13:04:23 2009
 *
 * @brief  Contains functions to allocate and free memory
 *
 *
 */

#include "rinoo/rinoo.h"

/**
 * Allocates size bytes and returns a pointer to the allocated memory
 *
 * @param size Size of the memory to allocate
 *
 * @return A pointer to the allocated memory
 */
void	*xmalloc(size_t size)
{
  void	*ptr;

  XDASSERT(size > 0, NULL);

  ptr = malloc(size);
  XASSERTSTR(ptr != NULL, NULL, "malloc returned NULL");
  return (ptr);
}

/**
 * allocates memory for an array of nmemb elements of size bytes each
 * and returns a pointer to the allocated memory. The memory is set to zero.
 *
 * @param nmemb Number of elements in the array
 * @param size Size of each element
 *
 * @return Pointer to the allocated memory
 */
void	*xcalloc(size_t nmemb, size_t size)
{
  void	*ptr;

  XDASSERT(nmemb > 0, NULL);
  XDASSERT(size > 0, NULL);

  ptr = calloc(nmemb, size);
  XASSERTSTR(ptr != NULL, NULL, "calloc returned NULL");
  return (ptr);
}

/**
 * Frees an allocated memory
 *
 * @param ptr Pointer to the allocated memory
 */
void	xfree(void *ptr)
{
  free(ptr);
}

/**
 * Calls realloc function and checks returned value.
 *
 * @param ptr Pointer to the memory to realloc.
 * @param size Size of the new memory segment.
 *
 * @return Pointer to the reallocated memory.
 */
void		*xrealloc(void *ptr, size_t size)
{
  void		*new_ptr;

  XDASSERT(ptr != NULL, NULL);
  XDASSERT(size > 0, NULL);

  new_ptr = realloc(ptr, size);
  XASSERTSTR(new_ptr != NULL, NULL, "realloc returned NULL");
  return (new_ptr);
}
