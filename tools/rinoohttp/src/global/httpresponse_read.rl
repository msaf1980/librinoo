/**
 * @file   httpresponse_read.rl
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Tue Jan 18 22:08:52 2011
 *
 * @brief  Functions which give ability to read http response.
 *
 *
 */


#include	"rinoo/rinoohttp.h"

%%{
  machine httpres_reader;
  write data;

  action startcode	{ code_start = fpc; }
  action endcode	{ code_end = fpc; }
  action startmsg	{ msg_start = fpc; }
  action endmsg		{ msg_end = fpc; }
  action startcl	{ cl_start = fpc; }
  action endcl		{ cl_end = fpc; }
  action okac		{
    if (code_start != NULL && code_end != NULL)
      {
	tmp = *code_end;
	*code_end = 0;
	httpsock->response.code = atoi(code_start);
	*code_end = tmp;
      }
    if (msg_start != NULL && msg_end != NULL)
      {
	httpsock->response.msg.buf = msg_start;
	httpsock->response.msg.len = msg_end - msg_start;
      }
    if (cl_start != NULL && cl_end != NULL)
      {
	tmp = *cl_end;
	*cl_end = 0;
	httpsock->response.contentlength = atoi(cl_start);
	*cl_end = tmp;
      }
    httpsock->response.length = fpc - buffer_ptr(httpsock->tcpsock->socket.rdbuf) + 1;
    return (1);
  }
  action parseerror	{ return (-1); }

  crlf = '\r\n';
  msg = (ascii+ -- crlf) >startmsg %endmsg;
  code = (digit+) >startcode %endcode;
  http = 'HTTP/1.' ('0' %{ httpsock->response.version = HTTP_VERSION_10; } |
		    '1' %{ httpsock->response.version = HTTP_VERSION_11; });
  contentlength = 'Content-length: 'i (digit+) >startcl %endcl crlf;
  header = (alnum | punct)+ ': ' (ascii* -- crlf) crlf;
  main := (http ' ' code ' ' msg crlf
	   header*
	   contentlength?
	   header*
	   crlf
	   @okac) $err(parseerror);
  }%%


int		httpresponse_read(t_httpsocket *httpsock)
{
  int		cs = 0;
  char		*p = buffer_ptr(httpsock->tcpsock->socket.rdbuf);
  char		*pe =
    buffer_ptr(httpsock->tcpsock->socket.rdbuf) +
    buffer_len(httpsock->tcpsock->socket.rdbuf);
  char		*eof = NULL;
  char		*code_start = NULL;
  char		*code_end = NULL;
  char		*msg_start = NULL;
  char		*msg_end = NULL;
  char		*cl_start = NULL;
  char		*cl_end = NULL;
  char		tmp;

  %% write init;
  %% write exec;

  (void) httpres_reader_en_main;
  (void) httpres_reader_error;
  (void) httpres_reader_first_final;
  return (0);
}

int		httpresponse_readbody(t_httpsocket *httpsock)
{
  if (httpsock->response.contentlength > 0 &&
      httpsock->response.received < httpsock->response.contentlength)
    {
      httpsock->response.received = buffer_len(httpsock->tcpsock->socket.rdbuf);
      if (httpsock->response.received >= httpsock->response.contentlength)
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
