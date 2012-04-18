/**
 * @file   http_request_parse.rl
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Sun Apr 15 22:29:07 2012
 *
 * @brief  HTTP request parsing
 *
 *
 */

#include	"rinoo/rinoo.h"

%%{
  machine httpreq_reader;
  write data;

  action starturi	{ uri_start = fpc; }
  action enduri		{ uri_end = fpc; }
  action startcl	{ cl_start = fpc; }
  action endcl		{ cl_end = fpc; }
  action okac		{
    buffer_static(req->uri, uri_start, uri_end - uri_start);
    if (cl_start != NULL && cl_end != NULL)
      {
	tmp = *cl_end;
	*cl_end = 0;
	req->content_length = atoi(cl_start);
	*cl_end = tmp;
      }
    req->length = fpc - buffer_ptr(buffer) + 1;
    return 1;
  }
  action parseerror	{ return -1; }

  crlf = '\r\n' | '\n';
  method = ('GET' %{ req->method = RINOO_HTTP_METHOD_GET; } |
	    'POST' %{ req->method = RINOO_HTTP_METHOD_POST; });
  uri = (ascii* -- crlf);
  http = 'HTTP/1.' ('0' %{ req->version = RINOO_HTTP_VERSION_10; } |
		    '1' %{ req->version = RINOO_HTTP_VERSION_11; });
  contentlength = 'Content-length: 'i (digit+) >startcl %endcl crlf;
  header = (alnum | punct)+ ': ' (ascii* -- crlf) crlf;
  main := (method ' ' uri >starturi %enduri ' ' http crlf
	   header*
	   contentlength?
	   header*
	   crlf
	   @okac) $err(parseerror);
  }%%


int rinoohttp_request_parse(t_rinoohttp_request *req, t_buffer *buffer)
{
	int cs = 0;
	char *p = buffer_ptr(buffer);
	char *pe = buffer_ptr(buffer) + buffer_len(buffer);
	char *eof = NULL;
	char *uri_start = NULL;
	char *uri_end = NULL;
	char *cl_start = NULL;
	char *cl_end = NULL;
	char tmp;

	%% write init;
	%% write exec;

	(void) httpreq_reader_en_main;
	(void) httpreq_reader_error;
	(void) httpreq_reader_first_final;
	return 0;
}
