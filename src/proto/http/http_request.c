/**
 * @file   http_request.c
 * @author Reginald Lips <reginald.l@gmail.com>
 * @date   Sun Apr 22 22:52:38 2012
 *
 * @brief  HTTP request functions
 *
 *
 */

#include "rinoo/rinoo.h"

int rinoohttp_request_get(t_rinoohttp *http)
{
	int ret;

	rinoohttp_reset(http);
	while (rinoo_socket_readb(http->socket, http->request.buffer) > 0) {
		ret = rinoohttp_request_parse(http);
		if (ret == 1) {
			return 1;
		} else if (ret == -1) {
			http->response.code = 400;
			rinoohttp_response_send(http, NULL);
			rinoohttp_reset(http);
		}
	}
	return 0;
}

