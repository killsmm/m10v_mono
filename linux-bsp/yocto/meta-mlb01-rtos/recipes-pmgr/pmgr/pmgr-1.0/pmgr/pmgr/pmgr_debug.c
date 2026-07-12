/******************************************************************************
 *
 *  @file   pmgr_debug.c
 *  @brief  Parameter Manager
 *
 *  Copyright 2018 Socionext Inc.
 ******************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include "pmgr_debug.h"

void err_msg(const char *file, int line, const char *type, const char *fmt, ...)
{
    fprintf(stderr, "[%s] %s(%d): ", type, file, line);

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fputc('\n', stderr);
}
