/**
 * @file   http_header.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2013
 * @date   Sun Jan  9 18:59:50 2011
 *
 * @brief  Functions to manager HTTP headers
 *
 *
 */

#include	"rinoo/rinoo.h"

/**
 * Compare function used in the HTTP header hashtable to sort entries.
 *
 * @param node1 Pointer to node1
 * @param node2 Pointer to node2
 *
 * @return -1 if they are different (no need to sort them), 0 if they are equal
 */
static int rinoohttp_header_cmp(t_rinoorbtree_node *node1, t_rinoorbtree_node *node2)
{

	t_rinoohttp_header *header1 = container_of(node1, t_rinoohttp_header, node);
	t_rinoohttp_header *header2 = container_of(node2, t_rinoohttp_header, node);

	return buffer_cmp(&header1->key, &header2->key);
}

/**
 * Free's a HTTP header structure.
 *
 * @param node Pointer to the HTTP header.
 */
void rinoohttp_header_free(t_rinoorbtree_node *node)
{
	t_rinoohttp_header *header = container_of(node, t_rinoohttp_header, node);

	free(buffer_ptr(&header->key));
	free(buffer_ptr(&header->value));
	free(header);
}

/**
 * Creates a new HTTP header tree.
 *
 *
 * @return A pointer to the created table or NULL if an error occurs.
 */
int rinoohttp_headers_init(t_rinoorbtree *tree)
{
	return rinoorbtree(tree, rinoohttp_header_cmp, rinoohttp_header_free);
}

/**
 * Flushes a HTTP header tree.
 *
 * @param headertree Pointer to the hashtable to destroy.
 */
void rinoohttp_headers_flush(t_rinoorbtree *headertree)
{
	rinoorbtree_flush(headertree);
}

/**
 *  Adds a new HTTP header to the hashtable.
 *
 * @param headertree Pointer to the hashtable where to store new header.
 * @param key HTTP header key.
 * @param value HTTP header value.
 * @param size Size of the HTTP header value.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int rinoohttp_header_setdata(t_rinoorbtree *headertree, const char *key, const char *value, uint32_t size)
{
	char *new_value;
	t_rinoohttp_header *new;
	t_rinoohttp_header dummy;
	t_rinoorbtree_node *found;

	XASSERT(headertree != NULL, -1);
	XASSERT(key != NULL, -1);
	XASSERT(value != NULL, -1);
	XASSERT(size > 0, -1);

	strtobuffer(&dummy.key, key);
	new_value = strndup(value, size);
	found = rinoorbtree_find(headertree, &dummy.node);
	if (found != NULL) {
		new = container_of(found, t_rinoohttp_header, node);
		free(new->value.ptr);
		strtobuffer(&new->value, new_value);
		return 0;
	}

	new = calloc(1, sizeof(*new));
	if (new == NULL) {
		free(new_value);
		return -1;
	}
	key = strdup(key);
	if (key == NULL) {
		free(new_value);
		free(new);
		return -1;
	}
	strtobuffer(&new->key, key);
	strtobuffer(&new->value, new_value);
	if (rinoorbtree_put(headertree, &new->node) != 0) {
		rinoohttp_header_free(&new->node);
		return -1;
	}
	return 0;
}

/**
 * Adds a new HTTP header to the hashtable.
 *
 * @param headertree Pointer to the hashtable where to store new header.
 * @param key HTTP header key.
 * @param value HTTP header value.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int rinoohttp_header_set(t_rinoorbtree *headertree, const char *key, const char *value)
{
	return rinoohttp_header_setdata(headertree, key, value, strlen(value));
}

/**
 * Removes a HTTP header from the hashtable.
 *
 * @param headertree Pointer to the hashtable to use.
 * @param key HTTP header key.
 */
void rinoohttp_header_remove(t_rinoorbtree *headertree, const char *key)
{
	t_rinoohttp_header dummy;
	t_rinoorbtree_node *toremove;

	XASSERTN(headertree != NULL);
	XASSERTN(key != NULL);

	strtobuffer(&dummy.key, key);
	toremove = rinoorbtree_find(headertree, &dummy.node);
	if (toremove != NULL) {
		rinoorbtree_remove(headertree, toremove);
	}
}

/**
 * Looks for a HTTP header and returns the corresponding structure.
 *
 * @param headertree Pointer to the hashtable to use.
 * @param key HTTP header key.
 *
 * @return A pointer to a HTTP header structure, or NULL if not found.
 */
t_rinoohttp_header *rinoohttp_header_get(t_rinoorbtree *headertree, const char *key)
{
	t_rinoohttp_header dummy;
	t_rinoorbtree_node *node;

	XASSERT(headertree != NULL, NULL);
	XASSERT(key != NULL, NULL);

	strtobuffer(&dummy.key, key);
	node = rinoorbtree_find(headertree, &dummy.node);
	if (node == NULL) {
		return NULL;
	}
	return container_of(node, t_rinoohttp_header, node);
}
