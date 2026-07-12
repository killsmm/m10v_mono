/*
 * warpd.h
 *
 *  Copyright (C) 2014 Lineo Solutions, Inc.
 */

#ifndef _WARPD_H

#define _WARPD_H
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// default configration
#define CONF_FILE		"/etc/warpd.conf"
#define IDENT_FILE		"/tmp/warpd_add"
#define SNAPSHOT_WRAPPER	"/usr/sbin/snapshot_wrapper"
#define IDENT_RID		'W'
#define IDENT_SID		'w'
#define NUM_OF_PROCS		10

// message strings
// warpd -> each procs
#define SNAPSHOT_DONE_STR	"w01"	/* snapshot success */
#define WARP_BOOT_DONE_STR	"w02"	/* warp boot success */
#define SNAPSHOT_FAIL1_STR	"w10"	/* snapshot failed */
#define SNAPSHOT_FAIL2_STR	"w11"	/* failed to execute snapshot wrapper */
#define SNAPSHOT_FAIL3_STR	"w12"	/* unknown error */
// each procs ->  warpd
#define PROC_READY_STR		"W01"	/* The process is ready to exec snapshot */

// warp basic info
#define WARP_ERROR_FILE		"/proc/warp/error"
#define WARP_STAT_FILE		"/proc/warp/stat"

// message queue structure
struct msgbuf{
	long mtype;
	char mdata[3];
};
#endif
