/******************************************************************************/
/**
 *  @file   warp_notify.h
 *  @brief  warp result notify application
 */
/*
 *  Copyright 2017 Socionext Inc.
 ******************************************************************************/
#ifndef __WARP_NOTIFY_H__
#define __WARP_NOTIFY_H__

#include <linux/ipcu_userland.h>

#define D_WARP_NOTIFY_HIB_RESULT_OK		(0x46423557)
#define D_WARP_NOTIFY_HIB_RESULT_NG		(0x4F525245)

#define D_WARP_NOTIFY_GET_HIB_RESULT_REQUEST	(0x3002)

#define D_WARP_NOTIFY_HIB_RESULT_HIB_BOOT_OK	(0)
#define D_WARP_NOTIFY_HIB_RESULT_NORMAL_BOOT	(1)

#define D_WARP_NOTIFY_BF_ADDRESS_FILE_PATH	"/tmp/warp_bf_address"

/******************************************************************** 
 *  IPCU IF structure between RTOS
 ********************************************************************/
typedef struct {
	INT32		command;		// 0x3002: get hibernation result
	INT32		result;			// offset 0x0 from boot_flag
	INT32		error_no;		// offset 0x3F8 from boot_flag
	INT32		bf_address;		// address of bootflag stored
	INT32		reserved4;		// 0x10-0x13
	INT32		reserved5;		// 0x14-0x17
	INT32		reserved6;		// 0x18-0x1B
	INT32		reserved7;		// 0x1C-0x1F
	INT32		error_code;		// error code
} T_WARP_NOTIFY_IPCU_IF;

#endif /* __WARP_NOTIFY_H__ */
