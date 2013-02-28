/**
 * @file   http_file.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2013
 * @date   Wed Feb 16 23:36:42 2011
 *
 * @brief  HTTP Server file sending
 *
 *
 */

#include	"rinoo/rinoo.h"

int rinoohttp_send_dir(t_rinoohttp *http, const char *path)
{
	int ret;
	int flag;
	DIR *dir;
	char *hl;
	char *de;
	t_buffer *result;
	struct stat stats;
	struct dirent *curentry;

	XASSERT(http != NULL, -1);
	XASSERT(path != NULL, -1);

	if (stat(path, &stats) != 0) {
		return -1;
	}
	if (S_ISDIR(stats.st_mode) == 0) {
		return -1;
	}
	dir = opendir(path);
	if (dir == NULL) {
		return -1;
	}
	result = buffer_create(NULL);
	if (result == NULL) {
		closedir(dir);
		errno = ENOMEM;
		return -1;
	}
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
	while ((curentry = readdir(dir)) != NULL) {
		if (flag == 0) {
			hl = " class=\"dl_hl\"";
		} else {
			hl = "";
		}
		if (curentry->d_type == DT_DIR) {
			de = "/";
		} else {
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

	http->response.code = 200;
	ret = rinoohttp_response_send(http, result);
	buffer_destroy(result);
	return ret;
}

int rinoohttp_send_file(t_rinoohttp *http, const char *path)
{
	int ret;
	int fd;
	void *ptr;
	t_buffer dummy;
	struct stat stats;

	XASSERT(http != NULL, -1);
	XASSERT(path != NULL, -1);

	if (stat(path, &stats) != 0) {
		return -1;
	}
	if (S_ISDIR(stats.st_mode)) {
		return rinoohttp_send_dir(http, path);
	}
	if (S_ISREG(stats.st_mode) == 0) {
		return -1;
	}
	fd = open(path, O_RDONLY);
	if (fd < 0) {
		return -1;
	}
	ptr = mmap(NULL, stats.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (ptr == MAP_FAILED) {
		ret = errno;
		close(fd);
		errno = ret;
		return -1;
	}
	http->response.code = 200;
	buffer_static(&dummy, ptr, stats.st_size);
	ret = rinoohttp_response_send(http, &dummy);
	munmap(ptr, stats.st_size);
	close(fd);
	return ret;
}
