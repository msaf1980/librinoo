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

t_rinoohttp_file	*rinoo_http_file_open(t_rinoohttp_send_ctx *sctx,
					      const char *path)
{
  int			fd;
  void			*ptr;
  struct stat		stats;
  t_rinoohttp_file	*httpfile;

  XASSERT(sctx != NULL, NULL);
  XASSERT(path != NULL, NULL);

  if (stat(path, &stats) != 0)
    {
      return NULL;
    }
  fd = open(path, O_RDONLY);
  if (fd < 0)
    {
      return NULL;
    }
  ptr = mmap(NULL, stats.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (ptr == NULL)
    {
      close(fd);
      return NULL;
    }
  httpfile = xcalloc(1, sizeof(*httpfile));
  if (httpfile == NULL)
    {
      close(fd);
      return NULL;
    }
  httpfile->fd = fd;
  httpfile->data.buf = ptr;
  httpfile->data.len = stats.st_size;
  sctx->tosend = &httpfile->data;
  return httpfile;
}

void			rinoo_http_file_destroy(void *data)
{
  t_rinoohttp_file	*httpfile;

  XASSERTN(data != NULL);

  httpfile = (t_rinoohttp_file *) data;
  munmap(buffer_ptr(&httpfile->data), buffer_len(&httpfile->data));
  close(httpfile->fd);
  xfree(httpfile);
}
