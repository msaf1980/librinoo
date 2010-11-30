/**
 * @file   debug.c
 * @author Reginald LIPS <reginald.l@gmail.com>
 * @date   Fri Nov  5 14:47:27 2010
 *
 * @brief  Enables debug information
 *
 *
 */

#include	"rinoo/rinoo.h"

/**
 * Signal callback which generates a backtrace of the current stack.
 *
 * @param signal Signal number
 */
void	rinoo_generate_backtrace(int signum)
{
  int	fd;
  char	gdbcmd[1024];
  char	backtrace_file[] = "/tmp/.rinoocrash_XXXXXX";

  fd = mkstemp(backtrace_file);
  if (fd == -1)
    {
      return;
    }
  close(fd);
  snprintf(gdbcmd,
	   1024,
	   "gdb -batch -p %d -x /usr/include/rinoo/backtrace.gdb > %s 2>&1",
	   getpid(),
	   backtrace_file);
  system(gdbcmd);
  signal(signum, SIG_DFL);
  raise(signum);
}

/**
 * Enables backtracing in case of crash.
 *
 */
void	rinoo_enable_backtrace()
{
  signal(SIGSEGV, rinoo_generate_backtrace);
  signal(SIGFPE, rinoo_generate_backtrace);
  signal(SIGABRT, rinoo_generate_backtrace);
  signal(SIGILL, rinoo_generate_backtrace);
  signal(SIGBUS, rinoo_generate_backtrace);
}
