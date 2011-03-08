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

t_rinoohttp_file	*rinoo_http_dir_open(t_rinoohttp_send_ctx *sctx,
					     const char *path)
{
  int			flag;
  DIR			*dir;
  char			*hl;
  char			*de;
  struct stat		stats;
  t_buffer		*result;
  t_rinoohttp_file	*httpfile;
  struct dirent		*curentry;

  XASSERT(sctx != NULL, NULL);
  XASSERT(path != NULL, NULL);

  if (stat(path, &stats) != 0)
    {
      return NULL;
    }
  if (S_ISDIR(stats.st_mode) == 0)
    {
      return NULL;
    }
  dir = opendir(path);
  if (dir == NULL)
    {
      return NULL;
    }
  result = buffer_create(1024, BUFFER_DEFAULT_MAXSIZE);
  if (result == NULL)
    {
      closedir(dir);
      return NULL;
    }
  httpfile = xcalloc(1, sizeof(*httpfile));
  if (httpfile == NULL)
    {
      closedir(dir);
      return NULL;
    }
  httpfile->type = 1;
  httpfile->fd = -1;
  httpfile->data = result;
  sctx->tosend = httpfile->data;
  buffer_print(result,
	       "<html>\n"
	       "  <head>\n"
	       "    <title>Directory listing</title>\n"
	       "    <style>\n"
	       "      body { font-family: Monospace; }\n"
	       "      #dirlist { width: 800px; border: 1px solid #888; padding: 10px; margin: 0 auto; }\n"
	       "      #dirlist ul { padding: 0px; list-style: none; }\n"
	       "      #dirlist li { padding: 2px; margin: 0px; }\n"
	       "      #dirlist li a { display: block; border: 1px dotted #888; padding: 0px 10px; height: 15px; }\n"
	       "      a { margin: 0px; text-decoration: none; color: #000; }\n"
	       "      .dl_title { font-size: 18px; color: #888; }\n"
	       "      .dl_en { float: left; width: 500px; }\n"
	       "      .dl_ed { float: left; width: 130px; text-align: center; }\n"
	       "      .dl_es { float: left; width: 120px; text-align: right; }\n"
	       "      .dl_hl { background-color: #eee; }\n"
	       "    </style>\n"
	       "  </head>\n"
	       "  <body>\n"
	       "    <div id=\"dirlist\">\n"
	       "      <div class=\"dl_title\">Directory listing</div>\n"
	       "      <ul>\n");
  flag = 0;
  while ((curentry = readdir(dir)) != NULL)
    {
      if (flag == 0)
	{
	  hl = " class=\"dl_hl\"";
	}
      else
	{
	  hl = "";
	}
      if (curentry->d_type == DT_DIR)
      	{
	  de = "/";
	}
      else
	{
	  de = "";
	}
      buffer_print(result,
		   "	<li><a href=\"%s%s\"%s>"
		   "<div class=\"dl_en\">%s%s</div>"
		   "<div class=\"dl_ed\">-</div>"
		   "<div class=\"dl_es\">-</div></a></li>",
		   curentry->d_name,
		   de,
		   hl,
		   curentry->d_name,
		   de);
      flag = ! flag;
    }
  buffer_print(result,
	       "      </ul>\n"
	       "    </div>\n"
	       "  </body>\n"
	       "</html>\n");
  closedir(dir);
  return httpfile;
}

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
  if (S_ISDIR(stats.st_mode))
    {
      return rinoo_http_dir_open(sctx, path);
    }
  if (S_ISREG(stats.st_mode) == 0)
    {
      return NULL;
    }
  fd = open(path, O_RDONLY);
  if (fd < 0)
    {
      return NULL;
    }
  ptr = mmap(NULL, stats.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (ptr == MAP_FAILED)
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
  httpfile->data = xcalloc(1, sizeof(*httpfile->data));
  if (httpfile->data == NULL)
    {
      xfree(httpfile);
      close(fd);
      return NULL;
    }
  httpfile->fd = fd;
  httpfile->data->buf = ptr;
  httpfile->data->len = stats.st_size;
  sctx->tosend = httpfile->data;
  return httpfile;
}

void			rinoo_http_file_destroy(void *data)
{
  t_rinoohttp_file	*httpfile;

  XASSERTN(data != NULL);

  httpfile = (t_rinoohttp_file *) data;
  if (httpfile->type == 0)
    {
      munmap(buffer_ptr(httpfile->data), buffer_len(httpfile->data));
      close(httpfile->fd);
      xfree(httpfile->data);
    }
  else
    {
      buffer_destroy(httpfile->data);
    }
  xfree(httpfile);
}
