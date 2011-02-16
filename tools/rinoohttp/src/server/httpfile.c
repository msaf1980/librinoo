/**
 * @file   httpfile.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Feb 16 23:36:42 2011
 *
 * @brief  HTTP Server file sending
 *
 *
 */
#include	"rinoo/rinoohttp.h"

static void			rinoo_http_file_fsm(t_rinoohttp *httpsock,
						    t_rinoohttp_event event);

static int	rinoo_http_file_open(t_rinoohttp_file *httpfile, const char *path)
{
  int		fd;
  void		*ptr;
  struct stat	stats;

  if (stat(path, &stats) != 0)
    {
      return -1;
    }
  fd = open(path, O_RDONLY);
  if (fd < 0)
    {
      return -1;
    }
  ptr = mmap(NULL, stats.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (ptr == NULL)
    {
      close(fd);
      return -1;
    }
  httpfile->fd = fd;
  httpfile->data.buf = ptr;
  httpfile->data.len = stats.st_size;
  return 0;
}

int		rinoo_http_file_send(t_rinoohttp *httpsock, const char *file)
{
  t_rinoohttp_file	*httpfile;

  XASSERT(httpsock != NULL, -1);
  XASSERT(file != NULL, -1);

  httpfile = xcalloc(1, sizeof(*httpfile));
  XASSERT(httpfile != NULL, -1);
  if (rinoo_http_file_open(httpfile, file) != 0)
    {
      xfree(httpfile);
      return -1;
    }
  httpfile->orig_data = httpsock->data;
  httpfile->orig_event_fsm = httpsock->event_fsm;
  httpsock->data = httpfile;
  httpsock->event_fsm = rinoo_http_file_fsm;
  httpsock->response.code = 200;
  httpsock->response.contentlength = buffer_len(&httpfile->data);
  return 0;
}

void		rinoo_http_file_destroy(t_rinoohttp *httpsock,
					t_rinoohttp_file *httpfile)
{
  XASSERTN(httpsock != NULL);
  XASSERTN(httpfile != NULL);

  httpsock->data = httpfile->orig_data;
  httpsock->event_fsm = httpfile->orig_event_fsm;
  munmap(buffer_ptr(&httpfile->data), buffer_len(&httpfile->data));
  close(httpfile->fd);
  xfree(httpfile);
}

static void			rinoo_http_file_fsm(t_rinoohttp *httpsock,
						    t_rinoohttp_event event)
{
  t_rinoohttp_file	*httpfile;

  httpfile = (t_rinoohttp_file *) httpsock->data;
  switch (event)
    {
    case EVENT_HTTP_CONNECT:
    case EVENT_HTTP_REQUEST:
    case EVENT_HTTP_REQBODY:
    case EVENT_HTTP_RESPONSE:
      break;
    case EVENT_HTTP_RESPBODY:
      if (httpfile->offset >= buffer_len(&httpfile->data))
	{
	  rinoo_http_file_destroy(httpsock, httpfile);
	  return;
	}
      if (buffer_len(&httpfile->data) - httpfile->offset > 2048)
	{
	  rinoo_tcp_printdata(httpsock->tcpsock,
			      buffer_ptr(&httpfile->data) + httpfile->offset,
			      2048);
	  httpfile->offset += 2048;
	}
      else
	{
	  rinoo_tcp_printdata(httpsock->tcpsock,
			      buffer_ptr(&httpfile->data) + httpfile->offset,
			      buffer_len(&httpfile->data) - httpfile->offset);
	  httpfile->offset = buffer_len(&httpfile->data);
	}
      break;
    case EVENT_HTTP_ERROR:
    case EVENT_HTTP_CLOSE:
    case EVENT_HTTP_TIMEOUT:
      rinoo_http_file_destroy(httpsock, httpfile);
      httpsock->event_fsm(httpsock, event);
      break;
    }
}
