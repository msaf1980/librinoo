/**
 * @file   rinoo.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2012
 * @date   Tue Dec 22 17:29:14 2009
 *
 * @brief  This is the global header file.
 *
 *
 */

#ifndef RINOO_H_
#define RINOO_H_

#define _GNU_SOURCE

#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <openssl/ssl.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/x509v3.h>

/* GLOBAL */
#include "rinoo/global/module.h"

/* DEBUG */
#include "rinoo/debug/module.h"

/* MEMORY */
#include "rinoo/memory/module.h"

/* STRUCT */
#include "rinoo/struct/module.h"

/* SCHEDULER */
#include "rinoo/scheduler/module.h"

/* NET */
#include "rinoo/net/module.h"

/* PROTO */
//#include "rinoo/proto/module.h"

#endif /* !RINOO_H_ */
