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

typedef struct	s_rinoohttp_file
{
  int		fd;
  u64		offset;
  t_buffer	data;
  void		*orig_data;
  void		(*orig_event_fsm)(t_rinoohttp *httpsock,
				  t_rinoohttp_event event);
}		t_rinoohttp_file;

int		rinoo_http_file_send(t_rinoohttp *httpsock, const char *file);

#endif		/* !RINOOHTTP_SERVER_HTTPFILE_H_ */
