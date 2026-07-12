/******************************************************************************/
/**
 *  @file   camera_if_direct.c
 *  @brief communication with rtos.
 */
/*
 *  Copyright 2017 Socionext Inc.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "cmfwk_mm.h"
#include "cmfwk_ipcu.h"
#include "cmfwk_std.h"
#include <stdlib.h>
//#define D_TEST_LINUX_RTOS_COMMON_STRING_BUF		(0x45E40028)
#define D_TEST_LINUX_RTOS_COMMON_STRING_BUF_AREA	(0x8000)
#define D_TEST_LINUX_RTOS_COMMON_STRING_BUF_SIZE	(256)
#define D_TEST_LINUX_RTOS_COMMON_STRING_BUF_OFFSET	(0x28)
#define D_TEST_LINUX_RTOS_COMMON_STRING_BUF_PAIR_CH	(8)

#define CAMERA_IF_CMDREQ_MAX (sizeof(camera_if_req) / sizeof(camera_if_cmd_request_data))

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

typedef struct {
	E_CAM_IF_COM_SET cmd_set;
	E_CAM_IF_SUB_COM cmd;
	ULONG param[4];
	CHAR *param_str[4];
} cmd_req_pack;

//command information
typedef struct {
	const char *cmd_str;
	ULONG cmd_set;
	ULONG cmd;
	ULONG param[4];
} camera_if_cmd_request_data;


/******************************************************************** 
 *			Local Variable
 ********************************************************************/
/* communication control */
static volatile UINT8 camera_if_cmd_send_id = 0xFF;
static volatile UINT8 camera_if_cmd_rcv_id = 0xFF;

static volatile E_CAM_IF_COM_SET camera_if_current_cmd_set;
static volatile E_CAM_IF_SUB_COM camera_if_current_cmd;

static volatile UCHAR camera_if_cmd_complete_flag = 0;
static volatile  struct camera_if_return camera_if_cmd_ret_value;


/******************************************************************** 
 *	Local Function
 ********************************************************************/
static void camera_if_cmd_rcv_callback(UINT8 id,
				       UINT32 pdata,
				       UINT32 length,
				       UINT32 cont,
				       UINT32 total_length)
{
	T_CPUCMD_COMMAND_ACK ack;


	if (camera_if_cmd_rcv_id != id ||
	    length != sizeof(ack) ||
	    length != total_length ||
	    cont == D_CONTINUE) {
		camera_if_cmd_ret_value.ret = E_CAMERA_IF_RES_CODE_ERROR;
		camera_if_cmd_complete_flag = 1;
		printf("Receive response error.\n");
		return;
	}

	memcpy(&ack, (void*)pdata, sizeof(ack));

	if (ack.t_head.magic_code != D_CPU_IF_MCODE_COOMNAD_ACK ||
	    ack.t_head.format_version != D_CPU_IF_COM_FORMAT_VERSION ||
	    ack.t_head.cmd_set != camera_if_current_cmd_set ||
	    ack.t_head.cmd != camera_if_current_cmd) {
		camera_if_cmd_ret_value.ret = E_CAMERA_IF_RES_CODE_ERROR;
		camera_if_cmd_complete_flag = 1;
		printf("Receive response error. code=0x%08X, version=0x%08X, cmd_set=0x%08X, cmd=0x%08X\n",
			ack.t_head.magic_code, ack.t_head.format_version,
			ack.t_head.cmd_set, ack.t_head.cmd);
		return;
	}

	camera_if_cmd_ret_value.ret = ack.ret1;
	camera_if_cmd_ret_value.ret_code = ack.ret2;
	camera_if_cmd_complete_flag = 1;
	printf("Receive response.\n");
	printf("        Return 1 = 0x%08X\n", ack.ret1);
	printf("        Return 2 = 0x%08X\n", ack.ret2);
	printf("        Return 3 = 0x%08X\n", ack.ret3);
	printf("        Return 4 = 0x%08X\n", ack.ret4);
	return;
}

static int camera_if_ipcu_initialize(void)
{
	INT32 ret;


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

	return 0;

cam_if_cb_err:
	FJ_IPCU_Close(camera_if_cmd_rcv_id);
recv_err:
	FJ_IPCU_Close(camera_if_cmd_send_id);
	camera_if_cmd_rcv_id = 0xFF;
send_err:
	camera_if_cmd_send_id = 0xFF;
	return -1;
}

static void camera_if_ipcu_uninitialize(void)
{
	if (camera_if_cmd_send_id != 0xFF)
		FJ_IPCU_Close(camera_if_cmd_send_id);
	if (camera_if_cmd_rcv_id != 0xFF)
		FJ_IPCU_Close(camera_if_cmd_rcv_id);

	camera_if_cmd_send_id = 0xFF;
	camera_if_cmd_rcv_id = 0xFF;
}

/* Send command to rtos. */
static int camera_if_send_cmd(E_CAM_IF_COM_SET cmd_set,
			      E_CAM_IF_SUB_COM cmd,
			      ULONG param[4])
{
	INT32  ret;
	T_CPUCMD_COMMAND_REQUEST cmd_req;
	UINT32 try_cnt = 0;
	UCHAR bTry_error = 0;
	struct camera_if_return tmp_tst;
	UINT32 vaddr;
	UINT32 phys_adr_top, m_size;

	FJ_MM_getPhysAddrInfo(IPCU_MM_BUFF, &phys_adr_top, &m_size);

	phys_adr_top = phys_adr_top +
			(( camera_if_cmd_send_id + D_TEST_LINUX_RTOS_COMMON_STRING_BUF_PAIR_CH ) *
			 D_TEST_LINUX_RTOS_COMMON_STRING_BUF_AREA ) + D_TEST_LINUX_RTOS_COMMON_STRING_BUF_OFFSET;

	vaddr = FJ_MM_phys_to_virt(phys_adr_top);
	if (vaddr == (UINT32)MAP_FAILED) {
		printf("FJ_MM_phys_to_virt error\n");
		return -3;
	}

	camera_if_cmd_complete_flag = 0;
	memset(&tmp_tst, 0x00, sizeof(tmp_tst));
	camera_if_cmd_ret_value = tmp_tst;
	camera_if_current_cmd_set = cmd_set;
	camera_if_current_cmd = cmd;
	cmd_req.t_head.magic_code = D_CPU_IF_MCODE_COMMAND_REQ;
	cmd_req.t_head.format_version = D_CPU_IF_COM_FORMAT_VERSION;
	cmd_req.t_head.cmd_set = cmd_set;
	cmd_req.t_head.cmd = cmd;

	cmd_req.param1 = param[0];
	cmd_req.param2 = param[1];
	cmd_req.param3 = param[2];
	cmd_req.param4 = param[3];

	ret = FJ_IPCU_Send(camera_if_cmd_send_id,
			   (UINT32)&cmd_req, sizeof(cmd_req),
			   IPCU_SEND_SYNC);
	if (ret != FJ_ERR_OK) {
		FJ_IPCU_Close(camera_if_cmd_send_id);
		return -1;
	}

	while (1) {
		if (camera_if_cmd_complete_flag == 1) {
			break;
		}
		
		if (try_cnt > 30) {
			bTry_error = 1;
			break;
		}
		++try_cnt;
		usleep(100*1000);
	}

	if (bTry_error == 1)
		return -2;
	return 0;
}

ULONG set_argv(char *param)
{
	ULONG ret_param;


	if (strlen(param) > 2) {
		if (param[0] == '0' && (param[1] == 'x' || param[1] == 'X'))
			ret_param = (ULONG)strtoul(param, NULL, 16);
		else
			ret_param = (ULONG)strtoul(param, NULL, 10);
	} else {
		ret_param = (ULONG)strtoul(param, NULL, 10);
	}

	return ret_param;
}

void usage()
{
	printf("Usage:\n");
	printf("camera_if_direct <Param1> <Param2> <Param3> <Param4> <Param5> <Param6>\n");
	printf("  Param1: Command\n");
	printf("  Param2: Sub Command\n");
	printf("  Param3: Parameter 1\n");
	printf("  Param4: Parameter 2\n");
	printf("  Param5: Parameter 3\n");
	printf("  Param6: Parameter 4\n");
	printf("\n");
	printf("  The number of parameters by the command is changed\n");
	printf("\n");
	printf("Example:\n");
	printf("  camera_if_direct 0x10100 0x100 0x09\n");
}
static void signal_handler(int signal)
{
	/* Uninitialize IPCU */
	camera_if_ipcu_uninitialize();
}
static void sigterm_handler(int signal)
{
	/* Uninitialize IPCU */
	camera_if_ipcu_uninitialize();
}

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
	INT32 i;
	cmd_req_pack cmd_req;
	INT32 ret;
	char str[16] = {0};
	float f_data = 0.0;

	if (signal(SIGINT, signal_handler) == SIG_ERR)
	printf("SIGINT erro\n");
	if (signal(SIGTERM, sigterm_handler) == SIG_ERR)
	printf("SIGTERM erro\n");

	/* Parameter Check */
	if (argc <= 2 || argc >= 8) {
		usage();
		return 0;
	}

	memset(&cmd_req, 0x00, sizeof(cmd_req));

	cmd_req.cmd_set = set_argv(argv[1]);
	cmd_req.cmd = set_argv(argv[2]);
	for (i = 0; i < (argc - 3); i++) {
#if 0
		cmd_req.param[i] = set_argv(argv[argv_cnt]);
		argv_cnt++;
#else
		memset(str, 0, sizeof(str));
		strcpy(str, argv[3+i]);
		switch( str[0] ) {
		case 'd':
			cmd_req.param[i] = (ULONG)strtol( &str[1], 0, 10 );
			break;
		case 'h':
			cmd_req.param[i] = (ULONG)strtoul( &str[1], 0, 16 );
			break;
		case 'f':
			f_data = strtof( &str[1], 0 );
			memcpy(&cmd_req.param[i], &f_data, sizeof(f_data));
			printf("f_data:%f\n, cmd_req.param[i]:%08x\n", f_data, cmd_req.param[i]);
			break;
		default:
			cmd_req.param[i] = set_argv(argv[3+i]);
			//cmd_req.param[i] = (ULONG)strtoul( &str[1], 0, 16 );
			break;
		}
#endif
	}

	/* Initialize IPCU */
	if (camera_if_ipcu_initialize() != 0) {
		printf("camera_if_ipcu_initialize error.\n");
		return 0;
	}

	ret = camera_if_send_cmd(cmd_req.cmd_set,
				 cmd_req.cmd,
				 cmd_req.param);
	if (ret == 0) {
		printf("0x%x:0x%x successfully.\n",
			cmd_req.cmd_set, cmd_req.cmd);
	} else {
		printf("0x%x:0x%x error.err_code = %d\n",
			cmd_req.cmd_set, cmd_req.cmd, ret);
	}

	/* Uninitialize IPCU */
	camera_if_ipcu_uninitialize();

	printf("Camera IF command end.\n\n");
	
	return 0;
}

