/******************************************************************************/
/**
 *  @file   camera_if.c
 *  @brief communication with rtos.
 */
/*
 *  Copyright 2016 Socionext Inc.
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

#define D_TEST_CPU_IF_UDTA_ADDR				(0x7E40E000)
#define D_TEST_CPU_IF_UDTA_SIZE_MAX			(4 * 1024 * 1024)
#define MSG_FILE 					"."

#define STR_MSG_QUE_MAX					(300)

#define D_TEST_CPU_IF_REQ_CMD_ASSEMBLY_MAX		(10)

#define D_TEST_LINUX_RTOS_COMMON_STRING_BUF		(0x4FE40028)
#define D_TEST_LINUX_RTOS_COMMON_STRING_BUF_SIZE	(256)

#define CAMERA_IF_CMDREQ_MAX (sizeof(camera_if_req) / sizeof(camera_if_cmd_request_data))

/* compile switch for unit test */
//#define CAMERA_IF_RCV_TEST

/******************************************************************** 
 *	Structure
 ********************************************************************/
 //communication control
struct camera_if_return {
	E_CAMERA_IF_RES_CODE ret;
	ULONG ret_code;
};

struct msgtype {
	ULONG mtype;
	T_CPUCMD_STREAM msg_stream;
};

struct stream_msg_queue {
	USHORT read;
	USHORT write;
	struct msgtype msgs[STR_MSG_QUE_MAX];
};

typedef struct {
	E_CAM_IF_COM_SET cmd_set;
	E_CAM_IF_SUB_COM cmd;
	ULONG param[5];
	CHAR *param_str[5];
} cmd_req_pack;

//command information
typedef struct {
	const char *cmd_str;
	ULONG cmd_set;
	ULONG cmd;
	ULONG param_count;
	UCHAR ptype[5];		/* 0 - int; 1 - string */
	ULONG param[5];
} camera_if_cmd_request_data;

/******************************************************************** 
 *			Local Variable
 ********************************************************************/
/* communication control */
static volatile UINT8 camera_event_rcv_id = 0xFF;

static volatile E_CAM_IF_COM_SET camera_if_current_cmd_set;
static volatile E_CAM_IF_SUB_COM camera_if_current_cmd;

static volatile UCHAR camera_if_cmd_complete_flag = 0;
static volatile  struct camera_if_return camera_if_cmd_ret_value;

static INT32 ev_rec_abort = 0;

static T_CPUCMD_NOTIFY_EVENT rec_event;
static int event_flag;
/******************************************************************** 
 *	Local Function
 ********************************************************************/
static void camera_event_rcv_callback(UINT8 id,
				      UINT32 pdata,
				      UINT32 length,
				      UINT32 cont,
				      UINT32 total_length)
{
	if (camera_event_rcv_id != id ||
	    length != sizeof(rec_event) ||
	    length != total_length ||
	    cont == D_CONTINUE) {
		printf("camera_event_rcv_callback error id(%d), length(%u), rcv_id(%d), size(%d)!\n",
			id, length, camera_event_rcv_id, sizeof(rec_event));
		return;
	}

	memcpy(&rec_event, (void*)pdata, sizeof(rec_event));

	if (rec_event.t_head.magic_code != D_CPU_IF_MCODE_EVENT_NOTIFY ||
	    rec_event.t_head.format_version != D_CPU_IF_COM_FORMAT_VERSION) {
		printf("event head error!\n");
		return;
	}

	switch(rec_event.t_head.cmd_set) {
		case E_CAM_IF_COM_SET_NOTIFY_EVENT_STATUS_CHANGED:
			printf("Command=0x%08x Sub-Command=0x%08x\n",
			       rec_event.t_head.cmd_set, rec_event.t_head.cmd);
			printf("Parameter1=0x%8x\n", rec_event.notify1);
			event_flag = 1;
			break;

		case E_CAM_IF_COM_SET_NOTIFY_EVENT_PROC_FINISH:
			printf("Command=0x%08x Sub-Command=0x%08x\n",
			       rec_event.t_head.cmd_set, rec_event.t_head.cmd);

			switch(rec_event.t_head.cmd) {
				case E_CAM_IF_SUB_COM_NOTIFY_EVENT_IMAGE_CAPTURE_FINISH:
					printf("Parameter1=0x%08x\n", rec_event.notify1);
					break;
				default:
					printf("Parameter1=0x%08x Parameter2=0x%08x\n",
					       rec_event.notify1, rec_event.notify2);
					break;
			}
			event_flag = 1;
			break;

		case E_CAM_IF_COM_SET_NOTIFY_EVENT_PROC_ERROR:
			printf("Command=0x%08x Sub-Command=0x%08x\n",
			       rec_event.t_head.cmd_set, rec_event.t_head.cmd);
			printf("Parameter1=0x%08x\n", rec_event.notify1);
			event_flag = 1;
			break;

		case E_CAM_IF_COM_SET_NOTIFY_EVENT_EDID_RECEIVED:
			printf("Command=0x%08x Sub-Command=0x%08x\n",
			       rec_event.t_head.cmd_set, rec_event.t_head.cmd);
			printf("Parameter1=0x%08x, Parameter2=0x%08x\
				Parameter3=0x%08x, Parameter4=0x%08x\n",
				rec_event.notify1, rec_event.notify2,
				rec_event.notify3, rec_event.notify4);
			event_flag = 1;
			break;

		case E_CAM_IF_COM_SET_NOTIFY_EVENT_ISSUE_CMD:
 			printf("E_CAM_IF_COM_SET_NOTIFY_EVENT_ISSUE_CMD, CMD:%s\n",(char*) &rec_event.evt );
			system((char*)&rec_event.evt);
			event_flag = 0;
			break;
		default:
			printf("event command error!\n");
			break;
	}

}

static void signal_handler(INT32 signal)
{
	ev_rec_abort = 1;
}

static int camera_event_rcv_initialize(void)
{
	INT32 ret;

	/* Initialize IPCU */
	ret = FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_4, (UINT8*)&camera_event_rcv_id);
	if (ret != FJ_ERR_OK)
		goto ev_rcv_err;
	ret = FJ_IPCU_SetReceiverCB(camera_event_rcv_id, camera_event_rcv_callback);
	if (ret != FJ_ERR_OK)
		goto cam_cb_err;
	return 0;

cam_cb_err:
	FJ_IPCU_Close(camera_event_rcv_id);
ev_rcv_err:
	camera_event_rcv_id = 0xFF;
	return -1;
}

static void camera_event_rcv_uninitialize(void)
{
	if (camera_event_rcv_id != 0xFF){
		FJ_IPCU_SetReceiverCB(camera_event_rcv_id, NULL);
		FJ_IPCU_Close(camera_event_rcv_id);
	}
	camera_event_rcv_id = 0xFF;
}
#if 0
static void camera_event_rcv_test(int value)
{
	rec_event.notify1 = value;
	rec_event.notify2 = value;
	rec_event.notify3 = value;
	rec_event.notify4 = value;

	switch (value) {
		case 100:
			event_flag = 1;
			rec_event.t_head.cmd_set = E_CAM_IF_COM_SET_NOTIFY_EVENT_STATUS_CHANGED;
			rec_event.t_head.cmd = E_CAM_IF_SUB_COM_NOTIFY_EVENT_HDMI_STATUS_CHANGED;
			break;

		case 200:
			event_flag = 1;
			rec_event.t_head.cmd_set = E_CAM_IF_COM_SET_NOTIFY_EVENT_STATUS_CHANGED;
			rec_event.t_head.cmd = E_CAM_IF_SUB_COM_NOTIFY_EVENT_SD_STATUS_CHANGED;
			break;

		case 300:
			event_flag = 1;
			rec_event.t_head.cmd_set = E_CAM_IF_COM_SET_NOTIFY_EVENT_STATUS_CHANGED;
			rec_event.t_head.cmd = E_CAM_IF_SUB_COM_NOTIFY_EVENT_BAD_BLOCK_EXCEEDED;
			break;

		case 400:
			event_flag = 1;
			rec_event.t_head.cmd_set = E_CAM_IF_COM_SET_NOTIFY_EVENT_PROC_FINISH;
			rec_event.t_head.cmd = E_CAM_IF_SUB_COM_NOTIFY_EVENT_IMAGE_CAPTURE_FINISH;
			break;

		case 500:
			event_flag = 1;
			rec_event.t_head.cmd_set = E_CAM_IF_COM_SET_NOTIFY_EVENT_PROC_FINISH;
			rec_event.t_head.cmd = E_CAM_IF_SUB_COM_NOTIFY_EVENT_VIDEO_CAPTURE_FINISH;
			break;

		case 600:
			event_flag = 1;
			rec_event.t_head.cmd_set = E_CAM_IF_COM_SET_NOTIFY_EVENT_PROC_FINISH;
			rec_event.t_head.cmd = E_CAM_IF_SUB_COM_NOTIFY_EVENT_MOVIE_RESTORE_FINISH;
			break;

		case 700:
			event_flag = 1;
			rec_event.t_head.cmd_set = E_CAM_IF_COM_SET_NOTIFY_EVENT_STATUS_CHANGED;
			rec_event.t_head.cmd = E_CAM_IF_SUB_COM_NOTIFY_EVENT_SENSOR_STATUS_CHANGED;
			break;

		case 800:
			event_flag = 1;
			rec_event.t_head.cmd_set = E_CAM_IF_COM_SET_NOTIFY_EVENT_PROC_ERROR;
			rec_event.t_head.cmd = E_CAM_IF_SUB_COM_NOTIFY_EVENT_PREVIEW_WIFI_ERROR;
			break;

		case 900:
			event_flag = 1;
			rec_event.t_head.cmd_set = E_CAM_IF_COM_SET_NOTIFY_EVENT_PROC_ERROR;
			rec_event.t_head.cmd = E_CAM_IF_SUB_COM_NOTIFY_EVENT_IMAGE_CAPTURE_ERROR;
			break;

		case 1000:
			event_flag = 1;
			rec_event.t_head.cmd_set = E_CAM_IF_COM_SET_NOTIFY_EVENT_PROC_ERROR;
			rec_event.t_head.cmd = E_CAM_IF_SUB_COM_NOTIFY_EVENT_VIDEO_CAPTURE_ERROR;
			break;

		case 1100:
			event_flag = 1;
			rec_event.t_head.cmd_set = E_CAM_IF_COM_SET_NOTIFY_EVENT_PROC_ERROR;
			rec_event.t_head.cmd = E_CAM_IF_SUB_COM_NOTIFY_EVENT_PLAY_ERROR;
			break;

		case 1200:
			event_flag = 1;
			rec_event.t_head.cmd_set = E_CAM_IF_COM_SET_NOTIFY_EVENT_PROC_ERROR;
			rec_event.t_head.cmd = E_CAM_IF_SUB_COM_NOTIFY_EVENT_MOVIE_RESTORE_ERROR;
			break;

		case 1300:
			event_flag = 1;
			rec_event.t_head.cmd_set = E_CAM_IF_COM_SET_NOTIFY_EVENT_PROC_ERROR;
			rec_event.t_head.cmd = value;
			break;

		case 1400:
			event_flag = 1;
			rec_event.t_head.cmd_set = E_CAM_IF_COM_SET_NOTIFY_EVENT_EDID_RECEIVED;
			rec_event.t_head.cmd = E_CAM_IF_SUB_COM_NOTIFY_EVENT_EDID_INFO;
			break;

		case 1500:
			event_flag = 1;
			rec_event.t_head.cmd_set = E_CAM_IF_COM_SET_NOTIFY_EVENT_EDID_RECEIVED;
			rec_event.t_head.cmd = value;
			break;

		case 1600:
			event_flag = 1;
			rec_event.t_head.cmd_set = value;
			rec_event.t_head.cmd = value;
			break;

		default:
			break;
	}
}
#endif
/******************************************************************************/
/**
 *  @brief  sample_app
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
	char* default_str = "/usr/bin/event/";
	char str[80] = "";
	char script_str[80] = "";
	char arg_str[80] = "";
	char error_str[80] = "";

	int ret;

#ifdef CAMERA_IF_RCV_TEST
	int i = 0;
#endif
	printf("camera_EV_rec main().\n");

	/* Register Ctrl-C event handler */
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGKILL, signal_handler);

	/* Initialize IPCU */
	if (camera_event_rcv_initialize() != 0) {
		printf("camera_event_rcv_initialize error.\n");
		return 0;
	}

	/* Main Loop */
	while (1) {
		if (ev_rec_abort)
			break;

		if (event_flag) {
			printf("event_receive\n");

			/* reset event_flag */
			event_flag =0;

			/* set default path */
			strcpy(str, default_str);

			sprintf(script_str, "%08X/%08X.sh", rec_event.t_head.cmd_set, rec_event.t_head.cmd);

			strcat(str, script_str);

			printf("%s\n", str);

			sprintf(arg_str, " %d %d %d %d",
				rec_event.notify1, rec_event.notify2,
				rec_event.notify3, rec_event.notify4);

			strcat(str, arg_str);

			printf("%s\n", str);

			ret = system(str);
			if (WEXITSTATUS(ret) != 0) {
				/* error occurred */
				strcpy(str, default_str);

				strcat(str, "error.sh");

				sprintf(error_str, " %d %d",
					rec_event.t_head.cmd_set, rec_event.t_head.cmd);

				strcat(str, error_str);

				strcat(str, arg_str);

				printf("%s\n", str);

				system(str);
			}
		}
#ifdef CAMERA_IF_RCV_TEST
		i++;

		camera_event_rcv_test(i);

		printf("cnt=%d\n", i);
#endif

		usleep(100000);
	}

	/* Uninitialize IPCU */
	camera_event_rcv_uninitialize();

	printf("Event recive end.\n\n");
	
	return 0;
}

