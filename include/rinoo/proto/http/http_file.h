/**
 * @file   http_file.h
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb  1 18:56:27 2017
 *
 * @brief  Header file for HTTP file
 *
 *
 */

#ifndef		RINOO_PROTO_HTTP_FILE_H_
# define	RINOO_PROTO_HTTP_FILE_H_

int rn_http_send_dir(rn_http_t *http, const char *path);
int rn_http_send_file(rn_http_t *http, const char *path);

#endif		/* !RINOO_PROTO_HTTP_FILE_H_ */

