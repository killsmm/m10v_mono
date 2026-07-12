/******************************************************************************/
/**
 *  @file   pmic_com.c
 *  @brief communication with Power Manager.
 */
/*
 *  Copyright 2018 Socionext Inc.
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

#define D_PMIC_RESPONSE_CODE_OFFSET	0x01000000
#define D_PMIC_NOTIFY_CODE_OFFSET	0x02000000

#define STR_MAX 255

/******************************************************************** 
 *	Structure
 ********************************************************************/
//communication control
struct pmic_com_cmd {
	ULONG data[10];
};

/******************************************************************** 
 *			Local Variable
 ********************************************************************/
/* communication control */
static volatile UINT8 g_cmd_send_id = 0xFF;
static volatile UINT8 g_cmd_rcv_id = 0xFF;

static volatile UCHAR g_complete_flag = 0;

static volatile struct pmic_com_cmd g_cmd_ret;


/******************************************************************** 
 *	Local Function
 ********************************************************************/
static void pmic_com_cmd_rcv_callback(UINT8 id,
									UINT32 pdata,
									UINT32 length,
									UINT32 cont,
									UINT32 total_length)
{
	INT32 i;
	INT32 num;
	struct pmic_com_cmd ack;
	
	// if (g_exit_flag == 1) return;

	if ( 40 < length ) {
		g_complete_flag = 1;
		printf("Receive response error.\n");
		return;
	}

	num = length / sizeof(UINT32);
	memcpy(&ack.data[0], (void*)pdata, length);

	for (i = 0; i < num; ++i) {
		g_cmd_ret.data[i] = ack.data[i];
	}

	if (g_cmd_ret.data[0] >= D_PMIC_NOTIFY_CODE_OFFSET) {
		printf("Receive notify.\n");
	}
	else if (g_cmd_ret.data[0] >= D_PMIC_RESPONSE_CODE_OFFSET) {
			printf("Receive response.\n");
			g_complete_flag = 1;
	}
	else {
			printf("Receive Unknown.\n");
			g_complete_flag = 1;
	}
	
	for (i = 0; i < num; ++i) {
		printf("        Return %2d = 0x%08X\n", i, g_cmd_ret.data[i]);
	}
	
	
	return;
}

static int pmic_com_ipcu_initialize(void)
{
	INT32 ret;

	/* Initialize IPCU */
	ret = FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_13, (UINT8*)&g_cmd_send_id);
	if (ret != FJ_ERR_OK)
		goto send_err;
	ret = FJ_IPCU_Open(E_FJ_IPCU_MAILBOX_TYPE_12, (UINT8*)&g_cmd_rcv_id);
	if (ret != FJ_ERR_OK)
		goto recv_err;
	ret = FJ_IPCU_SetReceiverCB(g_cmd_rcv_id, pmic_com_cmd_rcv_callback);
	if (ret != FJ_ERR_OK)
		goto cam_if_cb_err;
	return 0;

cam_if_cb_err:
	FJ_IPCU_Close(g_cmd_rcv_id);
recv_err:
	FJ_IPCU_Close(g_cmd_send_id);
	g_cmd_rcv_id = 0xFF;
send_err:
	g_cmd_send_id = 0xFF;
	return -1;
}

static void pmic_com_ipcu_uninitialize(void)
{
	// g_exit_flag = 1;
	
	if (g_cmd_send_id != 0xFF)
		FJ_IPCU_Close(g_cmd_send_id);
	if (g_cmd_rcv_id != 0xFF)
		FJ_IPCU_Close(g_cmd_rcv_id);

	g_cmd_send_id = 0xFF;
	g_cmd_rcv_id = 0xFF;
}

/* Send command to Power Manager. */
static int pmic_com_send_cmd(ULONG param[10], INT32 num)
{
	INT32 i;
	INT32 ret;
	struct pmic_com_cmd cmd_req;
	UINT32 try_cnt = 0;
	UCHAR bTry_error = 0;
	g_complete_flag = 0;

	for (i = 0; i < num; ++i) {
		cmd_req.data[i] = param[i];
	}

	ret = FJ_IPCU_Send(g_cmd_send_id,
					(UINT32)&cmd_req, num * sizeof(ULONG),
					IPCU_SEND_SYNC);
	if (ret != FJ_ERR_OK) {
		FJ_IPCU_Close(g_cmd_send_id);
		return -1;
	}

	while (1) {
		if (g_complete_flag == 1) {
			break;
		}
		
		if (try_cnt > 30) {
			bTry_error = 1;
			break;
		}
		++try_cnt;
		sleep(1);
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
			ret_param = (ULONG)atoi(param);
	} else {
		ret_param = (ULONG)atoi(param);
	}

	return ret_param;
}

INT32 cmd_arg_split(char *str, ULONG *param)
{
	INT32 i = 0;
	char cmd_name[] = "pmic_com";
	char dlm[] = " ";
	char *tok;
	char *ret;

	if ( str == NULL || param == NULL) {
		return -1;
	}

	tok = strtok( str, dlm );

	if (tok != NULL) {
		ret = strstr( tok, cmd_name );
		if (ret == NULL) {
			return -2;
		}
	}
	else {
		return -3;
	}
	
	while ( (tok = strtok( NULL, dlm )) != NULL ) {
		param[i++] = set_argv(tok);
	}
	
	return i;	// arg number
}

void exit_pmic_com()
{
	/* Uninitialize IPCU */
	pmic_com_ipcu_uninitialize();
	
	printf("PMIC Communication command end.\n\n");

	exit(EXIT_SUCCESS);
}

void usage()
{
	printf("Usage:\n");
	printf("pmic_com <Param1> <Param2> <Param3> <Param4> <Param5> <Param6> <Param7> <Param8> <Param9> <Param10>\n");
	printf("  Param1 : Parameter 1\n");
	printf("  Param2 : Parameter 2\n");
	printf("  Param3 : Parameter 3\n");
	printf("  Param4 : Parameter 4\n");
	printf("  Param5 : Parameter 5\n");
	printf("  Param6 : Parameter 6\n");
	printf("  Param7 : Parameter 7\n");
	printf("  Param8 : Parameter 8\n");
	printf("  Param9 : Parameter 9\n");
	printf("  Param10: Parameter 10\n");
	printf("\n");
	printf("  The number of parameters by the command is changed\n");
	printf("\n");
	printf("Example:\n");
	printf("  pmic_com 0x05 0x3001 0x00 0x01 0x02 0x03 0x04 0x05 0x06\n");
}
static void signal_handler(int signal)
{
	exit_pmic_com();
}
static void sigterm_handler(int signal)
{
	exit_pmic_com();
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
	INT32 prm_num;
	INT32 i;
	ULONG req_prm[10] = {0};
	INT32 ret;

	char cmd_str[STR_MAX];

	if (signal(SIGINT, signal_handler) == SIG_ERR)
	printf("SIGINT erro\n");
	if (signal(SIGTERM, sigterm_handler) == SIG_ERR)
	printf("SIGTERM erro\n");

	// g_exit_flag = 0;

	/* Parameter Check */
	prm_num = argc - 1;
	if (prm_num < 1 || prm_num > 10) {
		usage();
		return 0;
	}

	for (i = 0; i < prm_num; ++i) {
		req_prm[i] = set_argv(argv[1 + i]);
	}

	/* Initialize IPCU */
	if (pmic_com_ipcu_initialize() != 0) {
		printf("pmic_com_ipcu_initialize error.\n");
		return 0;
	}

	/* Send Request */
	while (1) {
		ret = pmic_com_send_cmd(req_prm, prm_num);
		
		if (ret == 0) {
			printf("0x%x:0x%x succesfully.\n", req_prm[0], req_prm[1]);
		} else {
			printf("0x%x:0x%x error.err_code = %d\n", req_prm[0], req_prm[1], ret);
		}

		/* Input Next Request */
		while (1) {
			printf("command > ");

			memset(cmd_str, 0, STR_MAX);
			fgets( cmd_str, STR_MAX, stdin);

			prm_num = cmd_arg_split(cmd_str, req_prm);

			/* Parameter Check */
			if (prm_num < 1 || prm_num > 10) {
				usage();
				continue;
			}else {
				break;
			}
		}
	}

	return 0;
}
