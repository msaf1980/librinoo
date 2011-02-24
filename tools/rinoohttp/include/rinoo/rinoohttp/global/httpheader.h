/**
 * @file   httpheader.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Sun Jan  9 19:57:32 2011
 *
 * @brief  Header file which describes HTTP header functions
 *
 *
 */

#ifndef		RINOOHTTP_GLOBAL_HTTPHEADER_H_
# define	RINOOHTTP_GLOBAL_HTTPHEADER_H_

typedef struct	s_rinoohttp_header
{
  t_buffer	key;
  t_buffer	value;
}		t_rinoohttp_header;

# define	RINOOHTTP_HEADER_HASHSIZE		128

# define	RINOOHTTP_HEADER_DEFAULT_SERVER	"RiNOO/" VERSION


t_hashtable	*rinoo_http_header_createtable();
void		rinoo_http_header_destroytable(t_hashtable *headertab);
int		rinoo_http_header_copytable(t_hashtable *htab_dest,
					    t_hashtable *htab_src);
int		rinoo_http_header_adddata(t_hashtable *headertab,
					  const char *key,
					  const char *value,
					  u32 size);
int		rinoo_http_header_add(t_hashtable *headertab,
				      const char *key,
				      const char *value);
int		rinoo_http_header_remove(t_hashtable *headertab, const char *key);
t_rinoohttp_header	*rinoo_http_header_get(t_hashtable *headertab,
					       const char *key);

#endif		/* !RINOOHTTP_GLOBAL_RINOO_HTTP_HEADER_H_ */
