/*
 * Warp helper library
 *
 *  Copyright (C) 2014 Lineo Solutions, Inc.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <getopt.h>
#include <signal.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>

#include "warpd.h"
#include "warp_helper_config.h"
#include "warp_helper_lib.h"

static int msq_rid, msq_sid;
static char *ident_file_path = IDENT_FILE;
static int ident_rid = IDENT_RID;
static int ident_sid = IDENT_SID;
static long num_procs = NUM_OF_PROCS;
static int proc_no = 0;

int warp_helper_libinit( char *full_path, int pno)
{
	int timeout;
	int ret;
	key_t ident_rkey, ident_skey;

	/**
	 * load config file setting
	 */
	if (!full_path)
		full_path = CONF_FILE;
	ret = read_conffile( full_path);
	if ( ret == 0){
		if ( strlen( warp_ipc_key)){
			ident_file_path =warp_ipc_key;
		}

		if ( warp_daemon_id){
			ident_rid = warp_daemon_id;
		}

		if ( warp_client_id){
			ident_sid = warp_client_id;
		}

		if ( warp_processes){
			num_procs = warp_processes;
		}
	}else{
		// init error
		return ERR_CONFIG_FILE;
	}

	if ( pno == 0){
		return ERR_PROCNUM_ZERO;
	}else
	if (num_procs < pno){
		return ERR_PROCNUM_OVER;
	}

	/**
	 * get warpd rx message queue
	 */
	ident_rkey = ftok( ident_file_path, ident_rid);
	if (ident_rkey == -1) {
		ret = errno;
		return ERR_IDENT_RX_FILE;
	}

	timeout = warp_wait_time;
	while( 1) {
		msq_rid = msgget(ident_rkey, 0666);
		if (msq_rid == -1) {
			ret = errno;
			if (ret == ENOENT) {
				sleep(1);
				if ( timeout){
					timeout--;
					if (timeout == 0){
						return ERR_WAPRD_RX_TIMEOUT;
					}
				}
				continue;
			}
//			printf("failed to get message queue(%d).\n", ret);
			return ERR_WAPRD_RX_Q;
		}
		break;
	}

	/**
	 * get warpd tx message queue
	 */
	ident_skey = ftok(ident_file_path, ident_sid);
	if (ident_skey == -1) {
		ret = errno;
		return ERR_IDENT_TX_FILE;
	}

	timeout = warp_wait_time;
	while(1) {
		msq_sid = msgget(ident_skey, 0666);
		if (msq_sid == -1) {
			ret = errno;
			if (ret == ENOENT) {
				sleep(1);
				if ( timeout){
					timeout--;
					if (timeout == 0){
						return ERR_WAPRD_TX_TIMEOUT;
					}
				}
				continue;
			}
//			printf("failed to get message queue(%d).\n", ret);
			return ERR_WAPRD_TX_Q;
		}
		break;
	}

	proc_no = pno;

	return ERR_SUCCESS;

}

int warp_helper_enter_freeze( void)
{
	int ret;
	struct msgbuf msq;

	/**
	 * send message to warpd
	 */
	msq.mtype = proc_no;
	memcpy(msq.mdata, PROC_READY_STR, sizeof(PROC_READY_STR));
	ret = msgsnd(msq_rid, &msq, sizeof(msq.mdata), 0);
	if (ret == -1) {
		return ERR_SEND_FAIL;
	}

	/**
	 * receive message from warpd
	 */
	while (1){
		ret = msgrcv(msq_sid, &msq, sizeof(msq.mdata), proc_no, 0);
		if (ret == -1) {
			if (errno == EINTR){
				// retry
				continue;
			}
			// error
			return ERR_RECV_FAIL;
		}else{
			// success
			break;
		}
	}

	if (!strncmp(msq.mdata, SNAPSHOT_DONE_STR, ret)) {
		ret = ERR_SUCCESS;
	} else if (!strncmp(msq.mdata, WARP_BOOT_DONE_STR, ret)) {
		ret = ERR_WARP_SUCCESS;
	} else {
		ret = ERR_SNAP_FAIL;
	}
	return ret;
}

void warp_helper_err_to_message( int err)
{
	switch( err){
		case ERR_SUCCESS:
			printf("Success.\n");
			break;

		case ERR_WARP_SUCCESS:
			printf("Warp boot success.\n");
			break;

		case ERR_CONFIG_FILE:
			printf("Config file error.\n");
			break;

		case ERR_IDENT_RX_FILE:
			printf("failed to create RX System V IPC key.(errono=%d)\n", errno);
			break;

		case ERR_WAPRD_RX_TIMEOUT:
			printf("Timeout : failed to get RX message queue.(errono=%d)\n", errno);
			break;

		case ERR_WAPRD_RX_Q:
			printf("failed to get RX message queue.(errono=%d)\n", errno);
			break;

		case ERR_IDENT_TX_FILE:
			printf("failed to create TX System V IPC key.(errono=%d)\n",errno);
			break;

		case ERR_WAPRD_TX_TIMEOUT:
			printf("Timeout : failed to get TX message queue.(errono=%d)\n", errno);
			break;

		case ERR_WAPRD_TX_Q:
			printf("failed to get TX message queue.(errono=%d)\n", errno);
			break;

		case ERR_SEND_FAIL:
			printf("msgsnd failed.(errono=%d)\n", errno);
			break;

		case ERR_RECV_FAIL:		
			printf("msgrcv failed.(errono=%d)\n", errno);
			break;

		case ERR_SNAP_FAIL:
			printf("exec snapshot failed.\n");
			break;

		case ERR_PROCNUM_OVER:
			printf("process number exceeded the limit.\n");
			break;

		case ERR_PROCNUM_ZERO:
			printf("invalid process number '0'.\n");
			break;

		default:
			printf("Unkown error.\n");
			break;
	}
}
