/**
 * @file   http_header.c
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Functions to manager HTTP headers
 *
 *
 */

#include "rinoo/proto/http/module.h"

/**
 * Compare function used in the HTTP header tree to sort entries.
 *
 * @param node1 Pointer to node1
 * @param node2 Pointer to node2
 *
 * @return -1 if they are different (no need to sort them), 0 if they are equal
 */
static int rn_http_header_cmp(rn_rbtree_node_t *node1, rn_rbtree_node_t *node2)
{

	rn_http_header_t *header1 = container_of(node1, rn_http_header_t, node);
	rn_http_header_t *header2 = container_of(node2, rn_http_header_t, node);

	return rn_buffer_cmp(&header1->key, &header2->key);
}

/**
 * Free's a HTTP header structure.
 *
 * @param node Pointer to the HTTP header.
 */
static void rn_http_header_free(rn_rbtree_node_t *node)
{
	rn_http_header_t *header = container_of(node, rn_http_header_t, node);

	free(rn_buffer_ptr(&header->key));
	free(rn_buffer_ptr(&header->value));
	free(header);
}

/**
 * Initializes an HTTP header set.
 *
 * @param headers HTTP header set to initialize
 *
 * @return 0 on success, otherwise -1
 */
int rn_http_headers_init(rn_http_header_set_t *headers)
{
	return rn_rbtree(&headers->tree, rn_http_header_cmp, rn_http_header_free);
}

/**
 * Flushes a HTTP header set.
 *
 * @param headers Pointer to the header set to destroy.
 */
void rn_http_headers_flush(rn_http_header_set_t *headers)
{
	headers->length = 0;
	headers->content_length = 0;
	rn_rbtree_flush(&headers->tree);
}

/**
 *  Adds a new HTTP header to the header set.
 *
 * @param headers Pointer to the header set where to store new header.
 * @param key HTTP header key.
 * @param value HTTP header value.
 * @param size Size of the HTTP header value.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int rn_http_header_setdata(rn_http_header_set_t *headers, const char *key, const char *value, uint32_t size)
{
	char *new_value;
	rn_http_header_t *new;
	rn_http_header_t dummy;
	rn_rbtree_node_t *found;

	XASSERT(headers != NULL, -1);
	XASSERT(key != NULL, -1);
	XASSERT(value != NULL, -1);
	XASSERT(size > 0, -1);

	rn_buffer_set(&dummy.key, key);
	new_value = strndup(value, size);
	found = rn_rbtree_find(&headers->tree, &dummy.node);
	if (found != NULL) {
		new = container_of(found, rn_http_header_t, node);
		free(new->value.ptr);
		rn_buffer_set(&new->value, new_value);
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
	rn_buffer_set(&new->key, key);
	rn_buffer_set(&new->value, new_value);
	if (rn_rbtree_put(&headers->tree, &new->node) != 0) {
		rn_http_header_free(&new->node);
		return -1;
	}
	return 0;
}

/**
 * Adds a new HTTP header to the header set.
 *
 * @param headers Pointer to the header set where to store new header.
 * @param key HTTP header key.
 * @param value HTTP header value.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int rn_http_header_set(rn_http_header_set_t *headers, const char *key, const char *value)
{
	return rn_http_header_setdata(headers, key, value, strlen(value));
}

/**
 * Removes a HTTP header from the header set.
 *
 * @param headers Pointer to the header set to use.
 * @param key HTTP header key.
 */
void rn_http_header_remove(rn_http_header_set_t *headers, const char *key)
{
	rn_http_header_t dummy;
	rn_rbtree_node_t *toremove;

	XASSERTN(headers != NULL);
	XASSERTN(key != NULL);

	rn_buffer_set(&dummy.key, key);
	toremove = rn_rbtree_find(&headers->tree, &dummy.node);
	if (toremove != NULL) {
		rn_rbtree_remove(&headers->tree, toremove);
	}
}

/**
 * Looks for a HTTP header and returns the corresponding structure.
 *
 * @param headers Pointer to the header set to use.
 * @param key HTTP header key.
 *
 * @return A pointer to an HTTP header structure, or NULL if not found.
 */
rn_http_header_t *rn_http_header_get(rn_http_header_set_t *headers, const char *key)
{
	rn_http_header_t dummy;
	rn_rbtree_node_t *node;

	XASSERT(headers != NULL, NULL);
	XASSERT(key != NULL, NULL);

	rn_buffer_set(&dummy.key, key);
	node = rn_rbtree_find(&headers->tree, &dummy.node);
	if (node == NULL) {
		return NULL;
	}
	return container_of(node, rn_http_header_t, node);
}

/**
 * Retrieve first header from header set.
 *
 * @param headers Pointer to the header set to use.
 *
 * @return A pointer to an HTTP header structure, or NULL if none.
 */
rn_http_header_t *rn_http_header_first(rn_http_header_set_t *headers)
{
	rn_rbtree_node_t *node;

	node = rn_rbtree_head(&headers->tree);
	if (node == NULL) {
		return NULL;
	}
	return container_of(node, rn_http_header_t, node);
}

/**
 * Retrieve the next header.
 *
 * @param header Pointer to the header.
 *
 * @return A pointer to an HTTP header structure, or NULL if header is the last.
 */
rn_http_header_t *rn_http_header_next(rn_http_header_t *header)
{
	rn_rbtree_node_t *node;

	node = rn_rbtree_next(&header->node);
	if (node == NULL) {
		return NULL;
	}
	return container_of(node, rn_http_header_t, node);
}
