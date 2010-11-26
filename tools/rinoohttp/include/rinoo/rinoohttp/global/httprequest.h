/**
 * @file   httprequest.h
 * @author Reginald <reginald.l@gmail.com>
 * @date   Mon Oct 25 17:48:47 2010
 *
 * @brief  Header file which describes http request structures
 *
 *
 */

#ifndef		RINOOHTTP_GLOBAL_HTTPREQUEST_H_
# define	RINOOHTTP_GLOBAL_HTTPREQUEST_H_

typedef struct	s_httprequest
{
  t_httpmethod	method;
  t_buffer	*uri;
  t_httpversion	version;
  t_hashtable	*headers;
  u64		length;
  u64		contentlength;
  u64		received;
}		t_httprequest;

struct		s_httpsocket;

int		httprequest_read(struct s_httpsocket *httpsock);
int		httprequest_readbody(struct s_httpsocket *httpsock);
void		httprequest_reset(t_httprequest *req);

#endif		/* !RINOOHTTP_GLOBAL_HTTPREQUEST_H_ */
