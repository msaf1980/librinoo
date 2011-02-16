/**
 * @file   http.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Mon Oct 25 18:01:12 2010
 *
 * @brief  Header file which describes global http structures
 *
 *
 */

#ifndef		RINOOHTTP_GLOBAL_HTTP_H_
# define	RINOOHTTP_GLOBAL_HTTP_H_

typedef enum	e_rinoohttp_method
  {
    HTTP_METHOD_NONE = 0,
    HTTP_METHOD_GET,
    HTTP_METHOD_POST
  }		t_rinoohttp_method;

typedef enum	e_rinoohttp_version
  {
    HTTP_VERSION_NONE = 0,
    HTTP_VERSION_10,
    HTTP_VERSION_11
  }		t_rinoohttp_version;

#endif		/* !RINOOHTTP_GLOBAL_HTTP_H_ */
