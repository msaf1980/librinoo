/**
 * @file   http_file.h
 * @author Reginald Lips <reginald.l@gmail.com>
 * @date   Tue Apr 17 21:14:27 2012
 *
 * @brief  Header file for HTTP file
 *
 *
 */

#ifndef RINOO_PROTO_HTTP_FILE_H_
#define RINOO_PROTO_HTTP_FILE_H_

int rinoohttp_send_dir(t_rinoohttp *http, const char *path);
int rinoohttp_send_file(t_rinoohttp *http, const char *path);

#endif /* !RINOO_PROTO_HTTP_FILE_H_ */

