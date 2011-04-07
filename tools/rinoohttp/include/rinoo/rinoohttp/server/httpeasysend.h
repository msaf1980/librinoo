/**
 * @file   httpeasysend.h
 * @author Reginald LIPS <reginald.l@gmail.com>
 * @date   Mon Mar  7 00:03:27 2011
 *
 * @brief  HTTP Server easy send interface header file.
 *
 *
 */

#ifndef		RINOOHTTP_SERVER_HTTPEASYSEND_H_
# define	RINOOHTTP_SERVER_HTTPEASYSEND_H_

typedef struct	t_rinoohttp_send_ctx
{
  u32		offset;
  t_rinoohttp	*httpsock;
  t_buffer	*tosend;
  void		*data;
  void		(*free_func)(void *data);
  void		*orig_data;
  void		(*orig_event_fsm)(t_rinoohttp *httpsock,
				  t_rinoohttp_event event);
}		t_rinoohttp_send_ctx;

t_rinoohttp_send_ctx	*rinoo_http_easysend(t_rinoohttp *httpsock);
void			rinoo_http_easysend_destroy(t_rinoohttp_send_ctx *sctx);
int			rinoo_http_easysend_buffer(t_rinoohttp *httpsock,
						   t_buffer *buffer);
int			rinoo_http_easysend_print(t_rinoohttp *httpsock,
						  const char *format, ...);
int			rinoo_http_easysend_file(t_rinoohttp *httpsock,
						 const char *path);

#endif		/* !RINOOHTTP_SERVER_HTTPEASYSEND_H_ */
