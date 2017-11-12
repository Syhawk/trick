#ifndef INCLUDE_ERROR_H
#define INCLUDE_ERROR_H

#include "unp.h"

#include <stdarg.h>
#include <syslog.h>

void err_sys(const char* fmt, ...);
void err_doit(int errnoflag, int level, const char* fmt, va_list ap);
void err_quit(const char* fmt, ...);

#endif	// INCLUDE_ERROR_H
