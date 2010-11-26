/**
 * @file   utils.c
 * @author Reginald <reginald.l@gmail.com>
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
  char			logline[2048];
  char			timestamp[23];
  va_list		ap;
  struct tm		tmp;
  struct timeval	tv;

  XASSERTN(gettimeofday(&tv, NULL) == 0);
  XASSERTN(localtime_r(&tv.tv_sec, &tmp) != NULL);
  if (strftime(timestamp, sizeof(timestamp), "%Y/%m/%d %T", &tmp) == 0)
    {
      XASSERTN(0);
    }
  printf("[%s.%03d] ", timestamp, (int) (tv.tv_usec / 1000));
  va_start(ap, format);
  res = vsnprintf(logline, 2048, format, ap);
  va_end(ap);
  res = (res > sizeof(logline) ? sizeof(logline) - 1 : res);
  for (i = 0; i < res; i++)
    {
      switch (logline[i])
	{
	case '\a':
	  printf("\\a");
	  break;
	case '\b':
	  printf("\\b");
	  break;
	case '\t':
	  printf("\\t");
	  break;
	case '\n':
	  printf("\\n");
	  break;
	case '\v':
	  printf("\\v");
	  break;
	case '\f':
	  printf("\\f");
	  break;
	case '\r':
	  printf("\\r");
	  break;
	case '"':
	  printf("\\\"");
	  break;
	case '\\':
	  printf("\\\\");
	  break;
	default:
	  if (isprint(logline[i]) == 0)
	    {
	      /*
	       * Not printable characters are represented
	       * by their octal value
	       */
	      printf("\\%03o", (unsigned char) logline[i]);
	    }
	  else
	    {
	      printf("%c", logline[i]);
	    }
	  break;

	}
    }
  printf("\n");
}
