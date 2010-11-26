/**
 * @file   http.h
 * @author Reginald <reginald.l@gmail.com>
 * @date   Mon Oct 25 18:01:12 2010
 *
 * @brief  Header file which describes global http structures
 *
 *
 */

#ifndef		RINOOHTTP_GLOBAL_HTTP_H_
# define	RINOOHTTP_GLOBAL_HTTP_H_

typedef enum	e_httpmethod
  {
    HTTP_GET = 0,
    HTTP_POST,
    HTTP_NOTIMPLEM
  }		t_httpmethod;

typedef enum	e_httpversion
  {
    HTTP_10 = 0,
    HTTP_11
  }		t_httpversion;

typedef struct	s_httpheader
{
  t_buffer	key;
  t_buffer	value;
}		t_httpheader;

#endif		/* !RINOOHTTP_GLOBAL_HTTP_H_ */
