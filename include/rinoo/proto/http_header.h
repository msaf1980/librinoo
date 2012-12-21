/**
 * @file   http_header.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Sun Jan  9 19:57:32 2011
 *
 * @brief  Header file which describes HTTP header functions
 *
 *
 */

#ifndef RINOO_PROTO_HTTP_HEADER_H_
#define RINOO_PROTO_HTTP_HEADER_H_

typedef struct	s_rinoohttp_header {
	t_buffer key;
	t_buffer value;
	t_rinoorbtree_node node;
} t_rinoohttp_header;

int rinoohttp_headers_init(void);
void rinoohttp_headers_flush(t_rinoorbtree *headertree);
int rinoohttp_header_setdata(t_rinoorbtree *headertree, const char *key, const char *value, u32 size);
int rinoohttp_header_set(t_rinoorbtree *headertree, const char *key, const char *value);
void rinoohttp_header_remove(t_rinoorbtree *headertree, const char *key);
t_rinoohttp_header *rinoohttp_header_get(t_rinoorbtree *headertab, const char *key);

#endif /* !RINOO_PROTO_HTTP_HEADER_H_ */
