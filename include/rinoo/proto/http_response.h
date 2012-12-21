/**
 * @file   http_response.h
 * @author Reginald Lips <reginald.l@gmail.com>
 * @date   Tue Apr 17 17:57:34 2012
 *
 * @brief  Header file for HTTP response
 *
 *
 */

#ifndef RINOO_PROTO_HTTP_RESPONSE_H_
#define RINOO_PROTO_HTTP_RESPONSE_H_

/* Defined in http.h */
struct s_rinoohttp;

typedef struct s_rinoohttp_response {
	int code;
	t_buffer msg;
	t_buffer *buffer;
	t_rinoosocket *socket;
	size_t content_length;
	t_rinoorbtree headers;
} t_rinoohttp_response;

void rinoohttp_response_setmsg(struct s_rinoohttp *http, const char *msg);
void rinoohttp_response_setdefaultmsg(struct s_rinoohttp *http);
void rinoohttp_response_setdefaultheaders(struct s_rinoohttp *http);
int rinoohttp_response_send(struct s_rinoohttp *http, t_buffer *body);

#endif /* !RINOO_PROTO_HTTP_RESPONSE_H_ */
