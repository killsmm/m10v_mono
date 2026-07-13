/*************************************************************************
 *
 *    Multi Device File Access Library
 *    Copyright 2015 Socionext Inc.
 *
 * 1. Fime name
 *        FS_SystemDependence.h
 * 2. Outline of source file
 *        System dependence part definition
 *
 ************************************************************************/

#ifndef _FS_SYSTEMDEPENDENCE_H
#define _FS_SYSTEMDEPENDENCE_H

#include "FS_lib.h"

/*
 * 1. Function name
 *    FS_SD_getTaskID
 * 2. Outline
 *    Task ID is acquired
 * 3. Argument
 *    FS_DWORD    *Taskid : [O] Address in area where task ID is stored
 * 4. Return
 *        None
 */
FS_VOID FS_SD_getTaskID(FS_DWORD *Taskid);

/*
 * 1. Function name
 *    FS_SD_malloc
 * 2. Outline
 *    The memory area of the specified size is acquired, and the first address is returned
 * 3. Argument
 *    FS_DWORD size : [I] Size of acquired memory area
 * 4. Return
 *        The first address in acquired memory area
 *        NULL when failing in acquisition of memory area
 */
FS_VOID *FS_SD_malloc(FS_DWORD size);

/*
 * 1. Function name
 *    FS_SD_free
 * 2. Outline
 *    A specified memory area is released
 *    When NULL or an address outside management is specified, it doesn't process it
 * 3. Argument
 *    FS_VOID *address : [I] The first address in released memory area
 * 4. Return
 *        None
 */
FS_VOID FS_SD_free(FS_VOID *address);

FS_VOID FS_memcpy(FS_VOID *d_buf, const FS_VOID *s_buf, FS_DWORD size);

#endif


