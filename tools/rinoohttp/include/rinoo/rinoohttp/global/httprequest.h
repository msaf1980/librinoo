/**
 * @file   httprequest.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Mon Oct 25 17:48:47 2010
 *
 * @brief  Header file which describes http request structures
 *
 *
 */

#ifndef		RINOOHTTP_GLOBAL_HTTPREQUEST_H_
# define	RINOOHTTP_GLOBAL_HTTPREQUEST_H_

typedef struct		s_rinoohttp_request
{
  t_rinoohttp_method	method;
  t_buffer		*uri;
  t_rinoohttp_version	version;
  t_hashtable		*headers;
  u64			length;
  u64			received;
  u64			contentlength;
}			t_rinoohttp_request;

struct		s_rinoohttp;

int		rinoo_http_request_read(struct s_rinoohttp *httpsock);
int		rinoo_http_request_readbody(struct s_rinoohttp *httpsock);
void		rinoo_http_request_reset(struct s_rinoohttp *httpsock);
void		rinoo_http_request_setdefaultheaders(struct s_rinoohttp *httpsock);

#endif		/* !RINOOHTTP_GLOBAL_HTTPREQUEST_H_ */
