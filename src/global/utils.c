/**
 * @file   utils.c
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Oct  6 21:49:38 2010
 *
 * @brief  Utility functions
 *
 *
 */

#include	"rinoo/rinoo.h"

/**
 * Add milliseconds to a timeval structure.
 *
 * @param in Pointer to the input timeval.
 * @param ms Number of milliseconds to add.
 * @param out Pointer to a timeval where to store the result.
 *
 * @return 0 on success, or -1 if an error occurs.
 */
int		timeraddms(struct timeval *in, u32 ms, struct timeval *out)
{
  struct timeval		tmp;

  XDASSERT(in != NULL, -1);
  XDASSERT(out != NULL, -1);

  tmp.tv_sec = ms / 1000;
  tmp.tv_usec = (ms % 1000) * 1000;
  timeradd(in, &tmp, out);
  return (0);
}

/**
 * Log function
 *
 * @param format printf's like format.
 */
void		rinoo_log(const char *format, ...)
{
  u32			i;
  u32			res;
  u32			offset;
  char			*logline;
  char			*esclogline;
  va_list		ap;
  struct tm		tmp;
  struct timeval	tv;

  logline = xmalloc(sizeof(*logline) * RINOO_LOG_MAXLENGTH);
  XASSERTN(logline != NULL);
  esclogline = xmalloc(sizeof(*esclogline) * RINOO_LOG_MAXLENGTH);
  XASSERTN(esclogline != NULL);
  XASSERTN(gettimeofday(&tv, NULL) == 0);
  XASSERTN(localtime_r(&tv.tv_sec, &tmp) != NULL);
  offset = strftime(logline, RINOO_LOG_MAXLENGTH, "[%Y/%m/%d %T.", &tmp);
  if (offset == 0)
    {
      xfree(logline);
      xfree(esclogline);
      XASSERTN(0);
    }
  offset += snprintf(logline + offset,
		     RINOO_LOG_MAXLENGTH - offset,
		     "%03d] ",
		     (int) (tv.tv_usec / 1000));
  va_start(ap, format);
  res = vsnprintf(logline + offset, RINOO_LOG_MAXLENGTH - offset, format, ap);
  va_end(ap);
  res = (offset + res > RINOO_LOG_MAXLENGTH ? RINOO_LOG_MAXLENGTH : offset + res);
  for (i = 0, offset = 0; i < res && offset < RINOO_LOG_MAXLENGTH - 3; i++, offset++)
    {
      switch (logline[i])
	{
	case '\a':
	  strcpy(esclogline + offset, "\\a");
	  offset++;
	  break;
	case '\b':
	  strcpy(esclogline + offset, "\\b");
	  offset++;
	  break;
	case '\t':
	  strcpy(esclogline + offset, "\\t");
	  offset++;
	  break;
	case '\n':
	  strcpy(esclogline + offset, "\\n");
	  offset++;
	  break;
	case '\v':
	  strcpy(esclogline + offset, "\\v");
	  offset++;
	  break;
	case '\f':
	  strcpy(esclogline + offset, "\\f");
	  offset++;
	  break;
	case '\r':
	  strcpy(esclogline + offset, "\\r");
	  offset++;
	  break;
	case '"':
	  strcpy(esclogline + offset, "\\\"");
	  offset++;
	  break;
	case '\\':
	  strcpy(esclogline + offset, "\\\\");
	  offset++;
	  break;
	default:
	  if (isprint(logline[i]) == 0)
	    {
	      /*
	       * Not printable characters are represented
	       * by their octal value
	       */
	      snprintf(esclogline + offset,
		       RINOO_LOG_MAXLENGTH - offset,
		       "\\%03o",
		       (unsigned char) logline[i]);
	      offset += 2;
	    }
	  else
	    {
	      esclogline[offset] = logline[i];
	    }
	  break;

	}
    }
  esclogline[offset++] = '\n';
  write(1, esclogline, offset);
  xfree(logline);
  xfree(esclogline);
}
