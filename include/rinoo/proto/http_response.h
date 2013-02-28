/**
 * @file   http_response.h
 * @author Reginald Lips <reginald.l@gmail.com>
 * @date   Tue Apr 17 17:57:34 2012
 *
 * @brief  Header file for HTTP response
 *
 *
 */

#ifndef		RINOO_PROTO_HTTP_RESPONSE_H_
# define	RINOO_PROTO_HTTP_RESPONSE_H_

/* Defined in http.h */
struct s_rinoohttp;

typedef struct s_rinoohttp_response
{
	t_rinoosocket		*socket;
	t_buffer		*buffer;
	int			code;
	t_buffer		msg;
	size_t			length;
	size_t			content_length;
	t_rinoorbtree		headers;
} t_rinoohttp_response;

int rinoohttp_response_parse(struct s_rinoohttp *http);
int rinoohttp_response_get(struct s_rinoohttp *http);
void rinoohttp_response_setmsg(struct s_rinoohttp *http, const char *msg);
void rinoohttp_response_setdefaultmsg(struct s_rinoohttp *http);
void rinoohttp_response_setdefaultheaders(struct s_rinoohttp *http);
int rinoohttp_response_send(struct s_rinoohttp *http, t_buffer *body);

#endif		/* !RINOO_PROTO_HTTP_RESPONSE_H_ */
