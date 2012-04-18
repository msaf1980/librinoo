/**
 * @file   http.h
 * @author Reginald Lips <reginald.l@gmail.com>
 * @date   Sun Apr 15 21:59:15 2012
 *
 * @brief  Header file for http service
 *
 *
 */

#ifndef		RINOO_PROTO_HTTP_H_
# define	RINOO_PROTO_HTTP_H_

# define	RINOO_HTTP_SIGNATURE	"RiNOO/" VERSION

typedef enum e_rinoohttp_method
{
	RINOO_HTTP_METHOD_GET = 0,
	RINOO_HTTP_METHOD_POST,
	RINOO_HTTP_METHOD_UNKNOWN
} t_rinoohttp_method;

typedef enum e_rinoohttp_version
{
	RINOO_HTTP_VERSION_10 = 0,
	RINOO_HTTP_VERSION_11,
	RINOO_HTTP_VERSION_UNKNOWN
} t_rinoohttp_version;

int rinoohttp_server(t_rinoosched *sched);

#endif		/* !RINOO_PROTO_HTTP_H_ */
