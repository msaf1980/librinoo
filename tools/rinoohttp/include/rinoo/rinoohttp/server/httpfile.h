/**
 * @file   httpfile.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Feb 16 23:40:13 2011
 *
 * @brief  Header file for http server file sending
 *
 *
 */

#ifndef		RINOOHTTP_SERVER_HTTPFILE_H_
# define	RINOOHTTP_SERVER_HTTPFILE_H_

typedef struct		s_rinoohttp_file
{
  int			fd;
  t_buffer		data;
}			t_rinoohttp_file;

t_rinoohttp_file	*rinoo_http_file_open(t_rinoohttp_send_ctx *sctx,
					      const char *path);
void			rinoo_http_file_destroy(void *data);

#endif		/* !RINOOHTTP_SERVER_HTTPFILE_H_ */
