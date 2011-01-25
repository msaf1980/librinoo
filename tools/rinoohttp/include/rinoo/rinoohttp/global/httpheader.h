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

typedef struct	s_httpheader
{
  t_buffer	key;
  t_buffer	value;
}		t_httpheader;

# define	HTTPHEADER_HASHSIZE		128

# define	HTTPHEADER_DEFAULT_SERVER	"RiNOO/" VERSION


t_hashtable	*httpheader_createtable();
void		httpheader_destroytable(t_hashtable *headertab);
int		httpheader_adddata(t_hashtable *headertab,
				   const char *key,
				   const char *value,
				   u32 size);
int		httpheader_add(t_hashtable *headertab, const char *key, const char *value);
int		httpheader_remove(t_hashtable *headertab, const char *key);
t_httpheader	*httpheader_get(t_hashtable *headertab, const char *key);

#endif		/* !RINOOHTTP_GLOBAL_HTTPHEADER_H_ */
