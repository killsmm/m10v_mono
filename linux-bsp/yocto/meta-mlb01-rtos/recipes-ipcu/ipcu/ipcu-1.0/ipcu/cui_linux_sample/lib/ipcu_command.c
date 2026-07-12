/******************************************************************************/
/*
 *  @file  ipcu.c
 *  @brief sample
 *
 *
 *  Copyright 2017 Socionext Inc.
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include "cmfwk_mm.h"
#include "cmfwk_ipcu.h"
#include "cmfwk_std.h"
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sched.h>
#include <errno.h>
#include <semaphore.h>
#include <stdarg.h>
#include "ipcu.h"
#include "camera_if.h"

/* SC2000 SDK Camera IF specification ver.2.31 */

/********************************************************************
 *			Global Variable
 ********************************************************************/
volatile UCHAR gFlag_video_capture_complete = 0;
volatile UCHAR gFlag_single_capture_complete = 0;
volatile UINT32 gStatus_sample_program = BOOT_MODE;

/********************************************************************
 *			Local Variable
 ********************************************************************/
/* communication control */
static volatile UINT8 camera_if_cmd_send_id = 0xFF;
static volatile UINT8 camera_if_cmd_rcv_id = 0xFF;
static volatile UINT8 camera_if_event_id = 0xFF;

static T_CPUCMD_COMMAND_REQUEST command_req;
static T_CPUCMD_COMMAND_ACK command_ack;
static T_CPUCMD_NOTIFY_EVENT event;

static volatile UCHAR camera_if_cmd_complete_flag = 0;

/********************************************************************
 * Local Function
 ********************************************************************/
/* Command Response */
static void camera_if_cmd_rcv_callback(UINT8 id,
				       UINT32 pdata,
				       UINT32 length,
				       UINT32 cont,
				       UINT32 total_length)
{

	/* read response*/
	memcpy(&command_ack, (void*)pdata, sizeof(command_ack));

	/* response check */
	if(camera_if_cmd_rcv_id != id){
		printf("error: ID unmatch.\n");
		return;
	}

	if(command_ack.t_head.cmd_set != command_req.t_head.cmd_set){
		printf("error: command unmatch.\n");
		return;
	}

	if(command_ack.t_head.cmd != command_req.t_head.cmd){
		printf("error: sub command unmatch.\n");
		return;
	}

	if(command_ack.t_head.magic_code != D_CPU_IF_MCODE_COOMNAD_ACK){
		printf("error: this response not ack.\n");
		return;
	}

	/* poling flag set */
	camera_if_cmd_complete_flag = 1;
}

/* Event notify */
static void camera_if_event_callback(UINT8 id,
				       UINT32 pdata,
				       UINT32 length,
				       UINT32 cont,
				       UINT32 total_length)
{

	/* read response*/
	memcpy(&event, (void*)pdata, sizeof(event));

	/* response check */
	if(camera_if_event_id != id){
		printf("error: ID unmatch.\n");
		return;
	}

	if(event.t_head.magic_code != D_CPU_IF_MCODE_EVENT_NOTIFY){
		printf("wornig: this is not event notify.\n");
		return;
	}

	if(event.t_head.cmd_set == E_CAM_IF_COM_SET_NOTIFY_EVENT_PROC_FINISH){
		if(event.t_head.cmd == E_CAM_IF_SUB_COM_NOTIFY_EVENT_VIDEO_CAPTURE_FINISH){
			if(event.notify1 == E_CAMERA_IF_RES_CODE_OK){
				/* poling flag set */
				gFlag_video_capture_complete = 1;
			}
		}
	}

	if(event.t_head.cmd_set == E_CAM_IF_COM_SET_NOTIFY_EVENT_PROC_FINISH){
		if(event.t_head.cmd == E_CAM_IF_SUB_COM_NOTIFY_EVENT_IMAGE_CAPTURE_FINISH){
			if(event.notify1 == E_CAMERA_IF_RES_CODE_OK){
				/* poling flag set */
				gFlag_single_capture_complete = 1;
			}
		}
	}

}

/* Initialize IPCU */
static int ipcu_init(void)
{
	INT32 ret = 0;

	/* Initialize IPCU */
	ret = FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_1, (UINT8*)&camera_if_cmd_send_id);
	if (ret != FJ_ERR_OK)
		goto send_err;
	ret = FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_0, (UINT8*)&camera_if_cmd_rcv_id);
	if (ret != FJ_ERR_OK)
		goto recv_err;
	ret = FJ_IPCU_SetReceiverCB(camera_if_cmd_rcv_id, camera_if_cmd_rcv_callback);
	if (ret != FJ_ERR_OK)
		goto cam_if_cb_err;
	ret = FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_4, (UINT8*)&camera_if_event_id);
	if (ret != FJ_ERR_OK)
		goto event_err;
	ret = FJ_IPCU_SetReceiverCB(camera_if_event_id, camera_if_event_callback);
	if (ret != FJ_ERR_OK)
		goto cam_if_event_cb_err;

	return 0;

send_err:
	camera_if_cmd_send_id = 0xFF;
recv_err:
	FJ_IPCU_Close(camera_if_cmd_send_id);
	camera_if_cmd_send_id = 0xFF;
	camera_if_cmd_rcv_id = 0xFF;
cam_if_cb_err:
	FJ_IPCU_Close(camera_if_cmd_send_id);
	FJ_IPCU_Close(camera_if_cmd_rcv_id);
	camera_if_cmd_send_id = 0xFF;
	camera_if_cmd_rcv_id = 0xFF;
event_err:
	FJ_IPCU_Close(camera_if_cmd_send_id);
	FJ_IPCU_Close(camera_if_cmd_rcv_id);
	camera_if_cmd_send_id = 0xFF;
	camera_if_cmd_rcv_id = 0xFF;
	camera_if_event_id = 0xFF;
cam_if_event_cb_err:
	FJ_IPCU_Close(camera_if_cmd_send_id);
	FJ_IPCU_Close(camera_if_cmd_rcv_id);
	FJ_IPCU_Close(camera_if_event_id);
	camera_if_cmd_send_id = 0xFF;
	camera_if_cmd_rcv_id = 0xFF;
	camera_if_event_id = 0xFF;
	return -1;
}

/* send command to rtos */
static int camera_if_send_cmd(T_CPUCMD_COMMAND_REQUEST req)
{
	INT32  ret = 0;

	/* poling flag clear */
	camera_if_cmd_complete_flag = 0;

	/* add header */
	req.t_head.magic_code = D_CPU_IF_MCODE_COMMAND_REQ;
	req.t_head.format_version = D_CPU_IF_COM_FORMAT_VERSION;

	command_req = req;

	/* IPCU send data */
	ret = FJ_IPCU_Send(camera_if_cmd_send_id,
				             (UINT32)&command_req,
				             sizeof(req),
				             IPCU_SEND_SYNC);

	if(ret != FJ_ERR_OK){
		FJ_IPCU_Close(camera_if_cmd_send_id);
		return -1;
	}

	return 0;
}

/********************************************************************
 * Global Function
 ********************************************************************/
/* Camera_IF Command Request */
int IPCU_Command_Send(T_CPUCMD_COMMAND_ACK* ack, UINT32 main, UINT32 sub, ...)
{
	va_list ap;
	INT32 ret = 0;
	UINT32 ack_timeout = 0;
	T_CPUCMD_COMMAND_REQUEST req;

	/* The second argument is variable before ... */
	va_start(ap, sub);

	req.t_head.cmd_set = main;
	req.t_head.cmd = sub;
	req.param1 = va_arg(ap, UINT32);
	req.param2 = va_arg(ap, UINT32);
	req.param3 = va_arg(ap, UINT32);
	req.param4 = va_arg(ap, UINT32);

	va_end(ap);

	/* send request to rtos */
	ret = camera_if_send_cmd(req);

#ifdef CAMERA_IF_DEBUG
	printf("[IPCU Command]Command     ID = 0x%08X\n", main);
	printf("[IPCU Command]Sub-Command ID = 0x%08X\n", sub);
	printf("[IPCU Command]param1         = 0x%08X\n", req.param1);
	printf("[IPCU Command]param2         = 0x%08X\n", req.param2);
	printf("[IPCU Command]param3         = 0x%08X\n", req.param3);
	printf("[IPCU Command]param4         = 0x%08X\n", req.param4);
#endif

	if(ret != 0){
		printf("error: send comannd error.err_code = %d\n", ret);
		return -1;
	}

	/* wait ack */
	if(camera_if_cmd_complete_flag == 1){
		camera_if_cmd_complete_flag = 0;
	}
	else{
		printf("info: waiting for ack from RTOS...\n");
		while(1){
			if(camera_if_cmd_complete_flag == 1){
				camera_if_cmd_complete_flag = 0;
				break;
			}

			++ack_timeout;
			sleep(1); /* sec */

			if(ack_timeout > 30){ /* application creator decides */
				/* RTOS not return ack */
				printf("error: ack timeout.\n");
				return -2;
			}
		}
	}

	*ack = command_ack;

	return 0;
}

/* Camera_IF Open */
int Camera_If_Open(void)
{
	/* Initialize IPCU */
	if(ipcu_init() != 0){
		printf("error: ipcu init.\n");
		return -1;
	}

	return 0;
}

/* Camera_IF Close */
void Camera_If_Close(void)
{
	if (camera_if_cmd_send_id != 0xFF){
//		printf("debug: FJ_IPCU_Close(camera_if_cmd_send_id) camera_if_cmd_send_id = %d\n", camera_if_cmd_send_id);
		FJ_IPCU_Close(camera_if_cmd_send_id);
	}
	if (camera_if_cmd_rcv_id != 0xFF){
//		printf("debug: FJ_IPCU_Close(camera_if_cmd_rcv_id) camera_if_cmd_rcv_id = %d\n", camera_if_cmd_rcv_id);
		FJ_IPCU_Close(camera_if_cmd_rcv_id);
	}
	if (camera_if_event_id != 0xFF){
//		printf("debug: FJ_IPCU_Close(camera_if_event_id) camera_if_event_id = %d\n", camera_if_event_id);
		FJ_IPCU_SetReceiverCB(camera_if_event_id, NULL);
		FJ_IPCU_Close(camera_if_event_id);
	}

	camera_if_cmd_send_id = 0xFF;
	camera_if_cmd_rcv_id = 0xFF;
	camera_if_event_id = 0xFF;
}

