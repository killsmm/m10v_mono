/******************************************************************************/
/**
 *  @file   ts.c
 *  @brief  test sample for IPCU Smart Command
 */
/*
 *  Copyright 2015 Socionext Inc.
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "cmfwk_mm.h"
#include "cmfwk_ipcu.h"
#include <string.h>

static char  OUTPUT_PATH[256] = {0};

static INT32 reassembled_size = 0;
static INT32 out_fd = -1;

static INT32 app_abort = 0;
static void signal_handler(INT32 signal)
{
	app_abort = 1;
}

const char* help_msg[]=
{
	"Usage:",
	"  test_ipcu send [sync|async] [shm=ADDRESS] id=N f=PATH [num=N]",
	"  test_ipcu recv id=N f=PATH",
	" ",
	"Options:",
	"  send or recv : IPCU direction",
	"  sync         : Synchronous type function (default)",
	"  async        : Asynchronous type function",
	"  shm=ADDRESS  : Shared SDRAM address (HEX)",
	"                   ADDRESS=0: Non-shared mode (default)",
	"                       Other: Shared mode",
	"  id=N         : IPCU channel id (N=0..15)",
	"  f=PATH       : File path of send/recieve data (PATH=Full path)",
	"  num=N        : Number of times to continue (default: 1)",
	NULL,
};


/******************************************************************************/
/**
 *  @brief  Callback function for asynchronous sending
 *  @fn void send_callback(UINT8 id, UINT32 pdata, UINT32 length, UINT32 cont, UINT32 total_length)
 *  @param  [in]    id           : instance ID
 *  @param  [in]    pdata        : top address of the received data table
 *  @param  [in]    length       : byte size of data
 *  @param  [in]    cont         : continue flag: continue=1, no-continue(last)=0 for separeted communitcation by 32KB size
 *  @param  [in]    total_length : total length of data when separeted communitcation
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
void send_callback(UINT8 id, UINT32 pdata, UINT32 length, UINT32 cont, UINT32 total_length)
{
	LOGI("[callback] IPCU#%d: pdata=0x%08x length=%d cont=%d total_length=%d \n", id, pdata, length, cont, total_length);
	app_abort = 1;
}

/******************************************************************************/
/**
 *  @brief  Callback function for receiving
 *  @fn void recv_callback(UINT8 id, UINT32 pdata, UINT32 length, UINT32 cont, UINT32 total_length)
 *  @param  [in]    id           : instance ID
 *  @param  [in]    pdata        : top address of the received data table
 *  @param  [in]    length       : byte size of data
 *  @param  [in]    cont         : continue flag: continue=1, no-continue(last)=0 for separeted communitcation by 32KB size
 *  @param  [in]    total_length : total length of data when separeted communitcation
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
void recv_callback(UINT8 id, UINT32 pdata, UINT32 length, UINT32 cont, UINT32 total_length)
{
	void *buf_vaddr = (void *)pdata;
	
	LOGI("[callback] IPCU#%d: pdata=0x%08x length=%d cont=%d total_length=%d \n", id, pdata, length, cont, total_length);
	
#if 1   /* Write to SD card */
	if (out_fd < 0) {
		out_fd = open(OUTPUT_PATH, (O_RDWR | O_CREAT | O_TRUNC), 0666);
		if (out_fd < 0) {
			LOGE("%s: failed to open %s \n", __func__, OUTPUT_PATH);
			return;
		}
		reassembled_size = 0;
	}
	
	if (write(out_fd, buf_vaddr, length) != length) {
		LOGE("%s: failed to write %s \n", __func__, OUTPUT_PATH);
	}
	reassembled_size += length;
	
	if (cont == D_NOT_CONTINUE) {
		if (out_fd >= 0) {
			close(out_fd);
			out_fd = -1;
		}
		if (reassembled_size != total_length) {
			LOGE("IPCU#%d: failed to reassemble data. Only %d/%d bytes received \n", id, reassembled_size, total_length);
		}
		
		app_abort = 1;
	}
#endif
}


/******************************************************************************/
/**
 *  @brief  Test for send function
 *  @fn int send_test(UINT8 id, UINT8 sync, UINT32 paddr, const char *path)
 *  @param  [in]    id        : IPCU channel id
 *  @param  [in]    sync      : Synchronous/Asynchronous
 *  @param  [in]    paddr     : Shared SDRAM address
 *  @param  [in]    path      : File path of send data
 *  @param  [in]    cont_num  : Number of times to continue
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
int send_test(UINT8 id, UINT8 sync, UINT32 paddr, const char *path, UINT32 cont_num)
{
	INT32  i, ret;
	INT32  fd;
	UINT32 vaddr, length;
	struct stat sb = {0};
	void   *map_p;
	
	/* Open and read a send file */
	fd = open(path, (O_RDWR | O_SYNC));
	if (fd < 0) {
		LOGE("%s: failed to open %s \n", __func__, path);
		return 1;
	}
	ret = fstat(fd, &sb);
	if (ret < 0) {
		return 1;
	}
	length = sb.st_size;
	map_p = mmap(0, length, PROT_READ, MAP_PRIVATE, fd, 0);
	
	if (paddr) {
		vaddr = FJ_MM_phys_to_virt(paddr);
		if (vaddr == (UINT32)MAP_FAILED) {
			LOGE("%s: failed to access to 0x%08x \n", __func__, paddr);
			goto cleanup;
		}
		memcpy((void*)vaddr, map_p, length);
	} else {
		vaddr = (UINT32)map_p;
	}
	
	if (sync == IPCU_SEND_ASYNC) {
		ret = FJ_IPCU_SetReceiverCB(id, send_callback);
		if (ret != FJ_ERR_OK) {
			return 1;
		}
		
		for(i = 0; i < cont_num; i++) {
			app_abort = 0;
			LOGI("FJ_IPCU_Send: id=%d pdata=0x%08x(0x%08x) length=%d sync=%d \n", id, vaddr, paddr, length, sync);
			ret = FJ_IPCU_Send(id, vaddr, length, sync);
			if (ret != FJ_ERR_OK) {
				goto cleanup;
			}
			
			/* Wait callback */
			while (!app_abort) {
				usleep(100000);
			}
		}
	}
	else {
		for(i = 0; i < cont_num; i++) {
			LOGI("FJ_IPCU_Send: id=%d pdata=0x%08x(0x%08x) length=%d sync=%d \n", id, vaddr, paddr, length, sync);
			ret = FJ_IPCU_Send(id, vaddr, length, sync);
			if (ret != FJ_ERR_OK) {
				goto cleanup;
			}
		}
	}
	
cleanup:
	munmap(map_p, length);
	close(fd);
	
	return 0;
}

/******************************************************************************/
/**
 *  @brief  Test for receive function
 *  @fn int recv_test(UINT8 id, const char *path)
 *  @param  [in]    id        : IPCU channel id
 *  @param  [in]    path      : File path of receive data
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
int recv_test(UINT8 id, const char *path)
{
	INT32  ret;
	
	strncpy(OUTPUT_PATH, path, sizeof(OUTPUT_PATH));
	
	ret = FJ_IPCU_SetReceiverCB(id, recv_callback);
	if (ret != FJ_ERR_OK) {
		return 1;
	}
	
	/* Main Loop */
	while (!app_abort) {
		usleep(100000);
	}
	
	return 0;
}

/******************************************************************************/
/**
 *  @brief  Test application for ipcu communication
 *  @fn int main( int argc, int8_t* argv[] )
 *  @param  [in]    argc      : The number of arguments passed
 *  @param  [in]    argv      : String of arguments passed
 *  @note
 *  @author
 *  @version
 */
/******************************************************************************/
int main(int argc, char *argv[])
{
	INT32  ret;
	UINT8  test_id;
	UINT32 cont_num = 1;
	
	INT32  loop;
	char   path[256] = {0};
	char   argstr[32] = {0};
	UINT32 direction = 0;
	UINT32 paddr = 0;
	INT32  chid = -1;
	UINT32 sync = IPCU_SEND_SYNC;
	UINT32 help = 0;
	
	if (argc == 1) {
		help = 1;
	} else {
		for(loop = 1; loop < argc; loop++) {
			if( (*((int*)argv[loop]) & 0x0000FFFF) == 0x00003d66 ) // "f="
			{
				strncpy(path, &argv[loop][2], sizeof(path));
				LOGV("File path=%s\n", path);
			}
			else if( (*((int*)argv[loop]) & 0x00FFFFFF) == 0x003d6469 ) // "id="
			{
				strncpy(argstr, &argv[loop][3], 8);
				chid = strtoul(argstr, NULL, 10);
				LOGV("chid=%d\n", chid);
			}
			else if( (*((int*)argv[loop]) & 0xFFFFFFFF) == 0x3d6d6873 ) // "shm="
			{
				strncpy(argstr, &argv[loop][4], 8);
				paddr = strtoul(argstr, NULL, 16);
				LOGV("SDRAM address=%x\n", paddr);
			}
			else if( (*((int*)argv[loop]) & 0xFFFFFFFF) == 0x3d6d756e ) // "num="
			{
				strncpy(argstr, &argv[loop][4], 8);
				cont_num = strtoul(argstr, NULL, 10);
				LOGV("Number of times to continue=%d\n", cont_num);
			}
			else if( strcmp(argv[loop], "send") == 0 )
			{
				direction = IPCU_DIR_SEND;
				LOGV("Send %d\n", direction);
			}
			else if( strcmp(argv[loop], "recv") == 0 )
			{
				direction = IPCU_DIR_RECV;
				LOGV("Recv %d\n", direction);
			}
			else if( strcmp(argv[loop], "sync") == 0 )
			{
				sync = IPCU_SEND_SYNC;
				LOGV("Sync %d\n", sync);
			}
			else if( strcmp(argv[loop], "async") == 0 )
			{
				sync = IPCU_SEND_ASYNC;
				LOGV("Sync %d\n", sync);
			}
			else
			{
				help = 1;
			}
		}
	}
	
	/* Check parameters */
	if ((path[0] == '\0') || (direction == 0) || (chid < 0)) {
		help = 1;
	}
	
	if (help) {
		for(loop = 0; help_msg[loop] != NULL; loop++) {
			printf("%s\n",help_msg[loop]);
		}
		return 0;
	}
	
	/* Register Ctrl-C event handler */
	signal(SIGINT, signal_handler);
	
	/* Preparation */
	ret = FJ_IPCU_Open(chid, &test_id);
	if (ret != FJ_ERR_OK) {
		return 1;
	}
	
	/* Main */
	if (direction == IPCU_DIR_SEND) {
		send_test(test_id, sync, paddr, path, cont_num);
	} else if (direction == IPCU_DIR_RECV) {
		recv_test(test_id, path);
	}
	
	/* Clean up */
	if (out_fd >= 0) {
		close(out_fd);
	}
	ret = FJ_IPCU_Close(test_id);
	if (ret != FJ_ERR_OK) {
		// DO NOTHING
	}
	
	return 0;
}
