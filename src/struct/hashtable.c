/**
 * @file   hashtable.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Apr 28 14:42:05 2010
 *
 * @brief  Functions to manage hash table structures.
 *
 *
 */

#include	"rinoo/rinoo.h"

static int	hashtable_cmpfunc(void *unused(node1), void *unused(node2))
{
  return (-1);
}

/**
 * Creates a new hash table.
 *
 * @param hashsize Array size of the hash table.
 * @param hash_func Pointer to a hash function.
 *
 * @return Pointer to the new hash table.
 */
t_hashtable	*hashtable_create(u32 hashsize,
				  u32 (*hash_func)(void *node),
				  int (*cmp_func)(void *node1, void *node2))
{
  u32		i;
  t_hashtable	*new;

  XDASSERT(hashsize > 0, NULL);
  XDASSERT(hash_func != NULL, NULL);

  new = xcalloc(1, sizeof(*new));
  XASSERT(new != NULL, NULL);
  new->hashsize = hashsize;
  new->hash_func = hash_func;
  new->table = xcalloc(hashsize, sizeof(*new->table));
  if (new->table == NULL)
    {
      xfree(new);
      XASSERT(0, NULL);
    }
  if (cmp_func == NULL)
    cmp_func = hashtable_cmpfunc;
  for (i = 0; i < hashsize; i++)
    new->table[i] = list_create(cmp_func);
  return (new);
}

/**
 * Destroys a hashtable and its elements.
 *
 * @param ptr Pointer to the hashtable to destroy.
 */
void		hashtable_destroy(void *ptr)
{
  u32		i;
  t_hashtable	*htab;

  XASSERTN(ptr != NULL);

  htab = (t_hashtable *) ptr;
  for (i = 0; i < htab->hashsize; i++)
    list_destroy(htab->table[i]);
  xfree(htab->table);
  xfree(htab);
}

/**
 * Adds an element to a hash table.
 *
 * @param htab Pointer to the hash table where to add the element.
 * @param node Pointer to the element to add.
 * @param free_func Pointer to a function which can free the element.
 *
 * @return 1 on success, 0 if an error occurs.
 */
int		hashtable_add(t_hashtable *htab,
			      void *node,
			      void (*free_func)(void *node))
{
  XDASSERT(htab != NULL, FALSE);

  if (list_add(htab->table[htab->hash_func(node) % htab->hashsize],
	       node,
	       free_func) == NULL)
    {
      return (FALSE);
    }
  htab->size++;
  return (TRUE);
}

/**
 * Deletes an element from a hash table.
 *
 * @param htab Pointer to the hash table to use.
 * @param node Pointer to the element to remove.
 * @param needfree Boolean which indicates if the node needs to be freed.
 *
 * @return 1 on success, 0 if an error occurs.
 */
int		hashtable_remove(t_hashtable *htab, void *node, u32 needfree)
{
  int		res;

  XDASSERT(htab != NULL, FALSE);

  res = list_remove(htab->table[htab->hash_func(node) % htab->hashsize],
		    node,
		    needfree);
  if (res == TRUE)
    htab->size--;
  return (res);
}

/**
 * Finds an element in a hash table.
 *
 * @param htab Pointer to the hash table to use.
 * @param node Pointer to an element to look for.
 *
 * @return Pointer to the element found or NULL if nothing is found.
 */
void		*hashtable_find(t_hashtable *htab, void *node)
{
  XDASSERT(htab != NULL, NULL);

  return (list_find(htab->table[htab->hash_func(node) % htab->hashsize],
		    node));
}

/**
 * Get next element of a hashtable. The hash iterator stores the current hash
 * and a listiterator to easily access to the next element in the next call.
 *
 * @param htab Pointer to the hashtable to use.
 * @param iterator Pointer to a hashtable iterator where to store the current hash & list iterator.
 *
 * @return A pointer to the current element or NULL if the end is reached.
 */
void		*hashtable_getnext(t_hashtable *htab, t_hashiterator *iterator)
{
  u32		i;
  void		*result;

  XDASSERT(htab != NULL, NULL);
  XDASSERT(iterator != NULL, NULL);

  for (i = iterator->hash; i < htab->hashsize; i++)
    {
      iterator->hash = i;
      result = list_getnext(htab->table[i], &iterator->list_iterator);
      if (result != NULL)
	{
	  return result;
	}
    }

  iterator->hash = 0;
  iterator->list_iterator = NULL;
  return NULL;
}
