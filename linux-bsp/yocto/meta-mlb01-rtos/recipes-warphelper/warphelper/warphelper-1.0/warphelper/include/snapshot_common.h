/******************************************************************************/
/**
 *  @file   snapshot_common.h
 *  @brief  snapshot support application common header
 */
/*
 *  Copyright 2016 Socionext Inc.
 ******************************************************************************/
#ifndef __SNAPSHOT_COMMON_H__
#define __SNAPSHOT_COMMON_H__

#include <linux/ipcu_userland.h>

//#define D_SNAPSHOT_COMMON_SYSLOG_ENABLE
//#define D_SNAPSHOT_COMMON_DEBUG_ENABLE

#ifdef D_SNAPSHOT_COMMON_SYSLOG_ENABLE
#define PRINT(fmt, ...)   syslog(LOG_INFO, fmt, __VA_ARGS__)
#else  // D_SNAPSHOT_COMMON_SYSLOG_ENABLE
#define PRINT(fmt, ...)   printf(fmt, __VA_ARGS__)
#endif // D_SNAPSHOT_COMMON_SYSLOG_ENABLE

#ifdef D_SNAPSHOT_COMMON_SYSLOG_ENABLE
#define DEBUG_PRINT(fmt, ...)   syslog(LOG_INFO, fmt, __VA_ARGS__)
#else  // D_SNAPSHOT_COMMON_SYSLOG_ENABLE
#ifdef D_SNAPSHOT_COMMON_DEBUG_ENABLE
#define DEBUG_PRINT(fmt, ...)   printf(fmt, __VA_ARGS__)
#else  // D_SNAPSHOT_COMMON_DEBUG_ENABLE
#define DEBUG_PRINT(fmt, ...)
#endif // D_SNAPSHOT_COMMON_DEBUG_ENABLE
#endif // D_SNAPSHOT_COMMON_SYSLOG_ENABLE

#ifdef D_SNAPSHOT_COMMON_SYSLOG_ENABLE
#define ERROR_PRINT(fmt, ...)   syslog(LOG_ERR, fmt, __VA_ARGS__)
#else  // D_SNAPSHOT_COMMON_SYSLOG_ENABLE
#define ERROR_PRINT(fmt, ...)   printf(fmt, __VA_ARGS__)
#endif // D_SNAPSHOT_COMMON_SYSLOG_ENABLE

#define D_SNAPSHOT_COMMON_MAGIC_NUMBER_OK				(0xBEEFCAFE)
#define D_SNAPSHOT_COMMON_MAGIC_NUMBER_NG				(0xFEEDBACF)

#define D_SNAPSHOT_COMMON_RECV_ERR_OK					(0)
#define D_SNAPSHOT_COMMON_RECV_ERR_NG					(1)

#define D_SNAPSHOT_COMMON_SHARED_COMMEM_NAME				"/proc/device-tree/shared_commem/buf-addr"

#define D_SNAPSHOT_COMMON_IPCU_BUFFER_OFFSET				(0x8000)
#define D_SNAPSHOT_COMMON_IPCU_CH					(2)

#endif /* __SNAPSHOT_COMMON_H__ */
