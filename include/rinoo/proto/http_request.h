/**
 * @file   http_request.h
 * @author Reginald Lips <reginald.l@gmail.com>
 * @date   Tue Apr 17 17:55:30 2012
 *
 * @brief  Header file for HTTP request
 *
 *
 */

#ifndef		RINOO_PROTO_HTTP_REQUEST_H_
# define	RINOO_PROTO_HTTP_REQUEST_H_

typedef struct s_rinoohttp_request
{
	t_rinoosocket		*socket;
	t_rinoohttp_method	method;
	t_rinoohttp_version	version;
	t_buffer		uri;
	size_t			length;
	t_rinoorbtree		header;
	size_t			content_length;
} t_rinoohttp_request;

int rinoohttp_request_parse(t_rinoohttp_request *req, t_buffer *buffer);

#endif		/* !RINOO_PROTO_HTTP_REQUEST_H_ */
