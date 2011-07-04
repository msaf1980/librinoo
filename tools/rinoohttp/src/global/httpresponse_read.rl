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

void	rinoo_http_response_addheader(t_rinoohttp *httpsock,
				      char *key_start, char *key_end,
				      char *val_start, char *val_end)
{
  char	*key;

  if (key_start >= key_end || val_start >= val_end)
    {
      return;
    }
  key = strndup(key_start, key_end - key_start);
  rinoo_http_header_adddata(httpsock->response.headers,
			    key,
			    val_start,
			    val_end - val_start);
  xfree(key);
}

%%{
  machine httpres_reader;
  write data;

  action startcode	{ code_start = fpc; }
  action endcode	{ code_end = fpc; }
  action startmsg	{ msg_start = fpc; }
  action endmsg		{ msg_end = fpc; }
  action startcl	{ cl_start = fpc; }
  action endcl		{ cl_end = fpc; }
  action startheaderkey	{ headerkey_start = fpc; }
  action endheaderkey	{ headerkey_end = fpc; }
  action startheaderval	{ headerval_start = fpc; }
  action endheaderval	{
    headerval_end = fpc;
    rinoo_http_response_addheader(httpsock,
				  headerkey_start,
				  headerkey_end,
				  headerval_start,
				  headerval_end);
    headerkey_start = NULL;
    headerkey_end = NULL;
    headerval_start = NULL;
    headerval_end = NULL;
  }
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
  header = (alnum | punct)+ >startheaderkey %endheaderkey ': ' (ascii* -- crlf) >startheaderval %endheaderval crlf;
  main := (http ' ' code ' ' msg crlf
	   header*
	   contentlength?
	   header*
	   crlf
	   @okac) $err(parseerror);
  }%%


int		rinoo_http_response_read(t_rinoohttp *httpsock)
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
  char		*headerkey_start = NULL;
  char		*headerkey_end = NULL;
  char		*headerval_start = NULL;
  char		*headerval_end = NULL;
  char		tmp;

  %% write init;
  %% write exec;

  (void) httpres_reader_en_main;
  (void) httpres_reader_error;
  (void) httpres_reader_first_final;
  return (0);
}

int		rinoo_http_response_readbody(t_rinoohttp *httpsock)
{
  if (httpsock->response.contentlength > 0 &&
      httpsock->response.received < httpsock->response.contentlength)
    {
      httpsock->response.received = buffer_len(httpsock->tcpsock->socket.rdbuf);
#warning socket rdbuf could have been erased in a previous step
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
