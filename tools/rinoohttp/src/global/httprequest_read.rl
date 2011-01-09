/**
 * @file   httprequest_read.rl
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Nov 10 14:36:40 2010
 *
 * @brief  Functions which give ability to read http request.
 *
 *
 */

#include	"rinoo/rinoohttp.h"

%%{
  machine httpreq_reader;
  write data;

  action starturi	{ uri_start = fpc; }
  action enduri		{ uri_end = fpc; }
  action startcl	{ cl_start = fpc; }
  action endcl		{ cl_end = fpc; }
  action okac		{
    buffer_add(httpsock->request.uri,
	       uri_start,
	       uri_end - uri_start);
    if (cl_start != NULL && cl_end != NULL)
      {
	tmp = *cl_end;
	*cl_end = 0;
	httpsock->request.contentlength = atoi(cl_start);
	*cl_end = tmp;
      }
    httpsock->request.length = fpc - buffer_ptr(httpsock->tcpsock->socket.rdbuf) + 1;
    return (1);
  }
  action parseerror	{ return (-1); }

  crlf = ('\r\n' | '\n');
  method = ('GET' %{ httpsock->request.method = HTTP_GET; } |
	    'POST' %{ httpsock->request.method = HTTP_POST; });
  uri = (ascii* -- crlf);
  http = 'HTTP/1.' ('0' %{ httpsock->request.version = HTTP_10; } |
		    '1' %{ httpsock->request.version = HTTP_11; });
  contentlength = 'Content-length: 'i (digit+) >startcl %endcl crlf;
  header = (alnum | punct)+ ': ' (ascii* -- crlf) crlf;
  main := (method ' ' uri >starturi %enduri ' ' http crlf
	   header*
	   contentlength?
	   header*
	   crlf
	   @okac) $err(parseerror);
  }%%


int		httprequest_read(t_httpsocket *httpsock)
{
  int		cs = 0;
  char		*p = buffer_ptr(httpsock->tcpsock->socket.rdbuf);
  char		*pe =
    buffer_ptr(httpsock->tcpsock->socket.rdbuf) +
    buffer_len(httpsock->tcpsock->socket.rdbuf);
  char		*eof = NULL;
  char		*uri_start = NULL;
  char		*uri_end = NULL;
  char		*cl_start = NULL;
  char		*cl_end = NULL;
  char		tmp;

  %% write init;
  %% write exec;

  (void) httpreq_reader_en_main;
  (void) httpreq_reader_error;
  (void) httpreq_reader_first_final;
  return (0);
}

int		httprequest_readbody(t_httpsocket *httpsock)
{
  if (httpsock->request.contentlength > 0 &&
      httpsock->request.received < httpsock->request.contentlength)
    {
      httpsock->request.received += buffer_len(httpsock->tcpsock->socket.rdbuf);
      if (httpsock->request.received >= httpsock->request.contentlength)
	{
	  return 1;
	}
    }
  else
    {
      return -1;
    }
  return 0;
}
