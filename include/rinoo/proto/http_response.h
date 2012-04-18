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

typedef struct s_rinoohttp_response
{
	t_rinoosocket		*socket;
	int			code;
	t_rinoohttp_version	version;
	t_buffer		msg;
	size_t			content_length;
	t_rinoorbtree		headers;
} t_rinoohttp_response;

int rinoohttp_response_init(t_rinoosocket *socket, t_rinoohttp_response *response);
void rinoohttp_response_destroy(t_rinoohttp_response *response);
void rinoohttp_response_reset(t_rinoohttp_response *response);
void rinoohttp_response_setmsg(t_rinoohttp_response *response, const char *msg);
void rinoohttp_response_setdefaultmsg(t_rinoohttp_response *response);
void rinoohttp_response_setdefaultheaders(t_rinoohttp_response *response);
int rinoohttp_response_send(t_rinoohttp_response *response, t_buffer *body);

#endif		/* !RINOO_PROTO_HTTP_RESPONSE_H_ */
