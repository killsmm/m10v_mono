/******************************************************************************
 *
 *  @file   pmgr_debug.h
 *  @brief  Parameter Manager
 *
 *  Copyright 2018 Socionext Inc.
 ******************************************************************************/
#ifndef __PMGR_DEBUG_H
#define __PMGR_DEBUG_H
#include <stdio.h>
#include <assert.h>

#define ERROR_LOG(fmt, ...) err_msg(__FILE__, __LINE__, "ERROR", fmt, ##__VA_ARGS__)
#define CHECK_POINTER(x) assert(x != NULL)
#define ASSERT(x) assert(x)
#define UNUSED_VARIABLE(x) (void)(x)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void err_msg(const char *file, int line, const char *type, const char *fmt, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PMGR_DEBUG_H */
