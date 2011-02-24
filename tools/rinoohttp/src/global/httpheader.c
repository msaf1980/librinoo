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
static u32	rinoo_http_header_hash(void *node)
{
  t_rinoohttp_header	*header = (t_rinoohttp_header *) node;

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
static int	rinoo_http_header_cmp(void *node1, void *node2)
{
  t_rinoohttp_header	*header1 = (t_rinoohttp_header *) node1;
  t_rinoohttp_header	*header2 = (t_rinoohttp_header *) node2;

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
t_hashtable	*rinoo_http_header_createtable()
{
  return hashtable_create(LIST_SORTED_HEAD,
			  RINOOHTTP_HEADER_HASHSIZE,
			  rinoo_http_header_hash,
			  rinoo_http_header_cmp);
}

/**
 * Destroys a HTTP header hashtable.
 *
 * @param headertab Pointer to the hashtable to destroy.
 */
void		rinoo_http_header_destroytable(t_hashtable *headertab)
{
  hashtable_destroy(headertab);
}

/**
 * Free's a HTTP header structure.
 *
 * @param node Pointer to the HTTP header.
 */
void		rinoo_http_header_free(void *node)
{
  t_rinoohttp_header	*header = (t_rinoohttp_header *) node;

  XDASSERTN(header != NULL);

  xfree(buffer_ptr(&header->key));
  xfree(buffer_ptr(&header->value));
  xfree(header);
}

/**
 * Copy a HTTP header hashtable content to another hashtable.
 *
 * @param htab_dest Pointer to the hashtable where to copy elements.
 * @param htab_src Pointer to the source hashtable.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int		rinoo_http_header_copytable(t_hashtable *htab_dest,
					    t_hashtable *htab_src)
{
  t_rinoohttp_header	*new;
  t_rinoohttp_header	*curheader;
  t_hashiterator	iterator = { 0, 0 };

  XASSERT(htab_dest != NULL, -1);
  XASSERT(htab_src != NULL, -1);

  while ((curheader = (t_rinoohttp_header *) hashtable_getnext(htab_src,
							       &iterator)) != NULL)
    {
      new = xcalloc(1, sizeof(*new));
      new->key.buf = xcalloc(1, sizeof(*new->key.buf) * curheader->key.len);
      new->key.len = curheader->key.len;
      memcpy(new->key.buf, curheader->key.buf, new->key.len);
      new->value.buf = xcalloc(1, sizeof(*new->value.buf) * curheader->value.len);
      new->value.len = curheader->value.len;
      memcpy(new->value.buf, curheader->value.buf, new->value.len);
      if (hashtable_add(htab_dest, new, rinoo_http_header_free) == 0)
	{
	  return -1;
	}
    }
  return 0;
}

/**
 *  Adds a new HTTP header to the hashtable.
 *
 * @param headertab Pointer to the hashtable where to store new header.
 * @param key HTTP header key.
 * @param value HTTP header value.
 * @param size Size of the HTTP header value.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int		rinoo_http_header_adddata(t_hashtable *headertab,
					  const char *key,
					  const char *value,
					  u32 size)
{
  t_rinoohttp_header	dummy;
  t_rinoohttp_header	*new;

  XASSERT(headertab != NULL, -1);
  XASSERT(key != NULL, -1);
  XASSERT(value != NULL, -1);
  XASSERT(size > 0, -1);

  strtobuffer(dummy.key, key);
  value = strndup(value, size);
  new = (t_rinoohttp_header *) hashtable_find(headertab, &dummy);
  if (new != NULL)
    {
      xfree(new->value.buf);
      strtobuffer(new->value, value);
      return 0;
    }

  new = xcalloc(1, sizeof(*new));
  key = strdup(key);
  strtobuffer(new->key, key);
  strtobuffer(new->value, value);
  if (hashtable_add(headertab, new, rinoo_http_header_free) == 0)
    {
      rinoo_http_header_free(new);
      return -1;
    }
  return 0;
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
int		rinoo_http_header_add(t_hashtable *headertab,
				      const char *key,
				      const char *value)
{
  return rinoo_http_header_adddata(headertab, key, value, strlen(value));
}

/**
 * Removes a HTTP header from the hashtable.
 *
 * @param headertab Pointer to the hashtable to use.
 * @param key HTTP header key.
 *
 * @return 0 on sucess, or -1 if an error occurs.
 */
int		rinoo_http_header_remove(t_hashtable *headertab, const char *key)
{
  t_rinoohttp_header	dummy;

  XDASSERT(headertab != NULL, -1);
  XDASSERT(key != NULL, -1);

  strtobuffer(dummy.key, key);
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
t_rinoohttp_header	*rinoo_http_header_get(t_hashtable *headertab,
					       const char *key)
{
  t_rinoohttp_header	dummy;

  XDASSERT(headertab != NULL, NULL);
  XDASSERT(key != NULL, NULL);

  strtobuffer(dummy.key, key);
  return (t_rinoohttp_header *) hashtable_find(headertab, &dummy);
}
