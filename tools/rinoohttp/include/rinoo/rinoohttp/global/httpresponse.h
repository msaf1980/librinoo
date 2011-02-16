/**
 * @file   httpresponse.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Mon Oct 25 18:05:40 2010
 *
 * @brief  Header file which describes http response structures
 *
 *
 */

#ifndef		RINOOHTTP_GLOBAL_HTTPRESPONSE_H_
# define	RINOOHTTP_GLOBAL_HTTPRESPONSE_H_

typedef struct		s_rinoohttp_response
{
  t_rinoohttp_version	version;
  u32			code;
  t_buffer		msg;
  t_hashtable		*headers;
  u64			length;
  u64			received;
  u64			contentlength;
}			t_rinoohttp_response;

struct s_rinoohttp;

int		rinoo_http_response_read(struct s_rinoohttp *httpsock);
int		rinoo_http_response_readbody(struct s_rinoohttp *httpsock);
void		rinoo_http_response_reset(struct s_rinoohttp *httpsock);
void		rinoo_http_response_setmsg(struct s_rinoohttp *httpsock,
					   const char *msg);
void		rinoo_http_response_setdefaultmsg(struct s_rinoohttp *httpsock);
void		rinoo_http_response_setdefaultheaders(struct s_rinoohttp *httpsock);

#endif		/* !RINOOHTTP_GLOBAL_RINOO_HTTP_RESPONSE_H_ */
