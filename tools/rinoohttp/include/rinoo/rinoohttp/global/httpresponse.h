/**
 * @file   httpresponse.h
 * @author Reginald <reginald.l@gmail.com>
 * @date   Mon Oct 25 18:05:40 2010
 *
 * @brief  Header file which describes http response structures
 *
 *
 */

#ifndef		RINOOHTTP_GLOBAL_HTTPRESPONSE_H_
# define	RINOOHTTP_GLOBAL_HTTPRESPONSE_H_

typedef struct	s_httpresponse
{
  t_httpversion	version;
  u32		code;
  t_buffer	msg;
  t_hashtable	*headers;
  u64		contentlength;
  t_buffer	body;
}		t_httpresponse;

void		httpresponse_reset(t_httpresponse *resp);

#endif		/* !RINOOHTTP_GLOBAL_HTTPRESPONSE_H_ */
