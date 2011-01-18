/**
 * @file   httpheader.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Sun Jan  9 18:59:50 2011
 *
 * @brief  Functions to manager HTTP headers
 *
 *
 */

#include	"rinoo/rinoohttp.h"

/**
 * Hash function used in the HTTP header hashtable.
 *
 * @param node Pointer to the node on which to compute hash
 *
 * @return Resulting hash
 */
static u32	httpheader_hash(void *node)
{
  t_httpheader	*header = (t_httpheader *) node;

  XDASSERT(header != NULL, 0);
  XDASSERT(buffer_ptr(&header->key) != NULL, 0);

  return murmurhash(buffer_ptr(&header->key),
		    buffer_len(&header->key),
		    buffer_len(&header->key));
}

/**
 * Compare function used in the HTTP header hashtable to sort entries.
 *
 * @param node1 Pointer to node1
 * @param node2 Pointer to node2
 *
 * @return -1 if they are different (no need to sort them), 0 if they are equal
 */
static int	httpheader_cmp(void *node1, void *node2)
{
  t_httpheader	*header1 = (t_httpheader *) node1;
  t_httpheader	*header2 = (t_httpheader *) node2;

  XDASSERT(header1 != NULL, -1);
  XDASSERT(header2 != NULL, -1);

  if (buffer_len(&header1->key) != buffer_len(&header2->key))
    {
      return -1;
    }
  if (strncasecmp(buffer_ptr(&header1->key),
		  buffer_ptr(&header2->key),
		  buffer_len(&header1->key)) != 0)
    {
      return -1;
    }
  return 0;
}

/**
 * Creates a new HTTP header hashtable.
 *
 *
 * @return A pointer to the created table or NULL if an error occurs.
 */
t_hashtable	*httpheader_createtable()
{
  return hashtable_create(HTTPHEADER_HASHSIZE,
			  httpheader_hash,
			  httpheader_cmp);
}

/**
 * Destroys a HTTP header hashtable.
 *
 * @param headertab Pointer to the hashtable to destroy.
 */
void		httpheader_destroytable(t_hashtable *headertab)
{
  hashtable_destroy(headertab);
}

/**
 * Free's a HTTP header structure.
 *
 * @param node Pointer to the HTTP header.
 */
void		httpheader_free(void *node)
{
  t_httpheader	*header = (t_httpheader *) node;

  XDASSERTN(header != NULL);

  xfree(header->key.buf);
  xfree(header->value.buf);
  xfree(header);
}

/**
 * Adds a new HTTP header to the hashtable.
 *
 * @param headertab Pointer to the hashtable where to store new header.
 * @param key HTTP header key.
 * @param value HTTP header value.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int		httpheader_add(t_hashtable *headertab, char *key, char *value)
{
  t_httpheader	dummy;
  t_httpheader	*new;

  XDASSERT(headertab != NULL, -1);
  XDASSERT(key != NULL, -1);
  XDASSERT(value != NULL, -1);

  dummy.key.buf = key;
  dummy.key.len = strlen(key);
  new = (t_httpheader *) hashtable_find(headertab, &dummy);
  if (new != NULL)
    {
      xfree(new->value.buf);
      new->value.buf = strdup(value);
      new->value.len = strlen(value);
      return 0;
    }

  new = xcalloc(1, sizeof(*new));
  new->key.buf = strdup(key);
  new->key.len = dummy.key.len;
  new->value.buf = strdup(value);
  new->value.len = strlen(value);
  if (hashtable_add(headertab, new, httpheader_free) == 0)
    {
      xfree(new);
      return -1;
    }
  return 0;
}

/**
 * Removes a HTTP header from the hashtable.
 *
 * @param headertab Pointer to the hashtable to use.
 * @param key HTTP header key.
 *
 * @return 0 on sucess, or -1 if an error occurs.
 */
int		httpheader_remove(t_hashtable *headertab, char *key)
{
  t_httpheader	dummy;

  XDASSERT(headertab != NULL, -1);
  XDASSERT(key != NULL, -1);

  dummy.key.buf = key;
  dummy.key.len = strlen(key);
  if (hashtable_remove(headertab, &dummy, TRUE) == FALSE)
    {
      return -1;
    }
  return 0;
}

/**
 * Looks for a HTTP header and returns the corresponding structure.
 *
 * @param headertab Pointer to the hashtable to use.
 * @param key HTTP header key.
 *
 * @return A pointer to a HTTP header structure, or NULL if not found.
 */
t_httpheader	*httpheader_get(t_hashtable *headertab, char *key)
{
  t_httpheader	dummy;

  XDASSERT(headertab != NULL, NULL);
  XDASSERT(key != NULL, NULL);

  dummy.key.buf = key;
  dummy.key.len = strlen(key);
  return (t_httpheader *) hashtable_find(headertab, &dummy);
}
