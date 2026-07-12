/******************************************************************************/
/**
 *  @file   save_snapshot.h
 *  @brief  snapshot save support application
 */
/*
 *  Copyright 2016 Socionext Inc.
 ******************************************************************************/
#ifndef __SAVE_SNAPSHOT_H__
#define __SAVE_SNAPSHOT_H__

#include <linux/ipcu_userland.h>

#define D_SAVE_SNAPSHOT_NF_UNMOUNT_REQUEST				(0x3000)
#define D_SAVE_SNAPSHOT_NF_MOUNT_REQUEST				(0x3001)

#define D_SAVE_SNAPSHOT_OPERATION_NONE					(0)
#define D_SAVE_SNAPSHOT_OPERATION_START					(1)
#define D_SAVE_SNAPSHOT_OPERATION_COMPLETE				(2)

#define D_SAVE_SNAPSHOT_TARGET_NF_0					(0x0000)
#define D_SAVE_SNAPSHOT_TARGET_EMMC_0					(0x0001)
#define D_SAVE_SNAPSHOT_TARGET_NF_1					(0x0002)
#define D_SAVE_SNAPSHOT_TARGET_EMMC_1					(0x0003)

/******************************************************************** 
 *  IPCU IF structure between RTOS
 ********************************************************************/
typedef struct {
	INT32		command;		// 0x3000: unmount request 0x3001: mount request
	INT32		target;			// 0x0000: NF 0x0001:emmc
	INT32		reserved2;		// 0x08-0x0B
	INT32		reserved3;		// 0x0C-0x0F
	INT32		reserved4;		// 0x10-0x13
	INT32		reserved5;		// 0x14-0x17
	INT32		reserved6;		// 0x18-0x1B
	INT32		reserved7;		// 0x1C-0x1F
	INT32		error_code;		// error code
} T_SAVE_SNAPSHOT_IPCU_IF;

#endif /* __SAVE_SNAPSHOT_H__ */
