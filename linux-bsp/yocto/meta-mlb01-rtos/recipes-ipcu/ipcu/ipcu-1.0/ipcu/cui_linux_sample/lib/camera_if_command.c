/******************************************************************************/
/*
 *  @file  sample.c
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
#include "camera_if.h"
#include "ipcu.h"

/********************************************************************
 * Global Function
 ********************************************************************/
/* Finish Boot */
int Finish_Boot(UINT32* result, UINT32* err_code)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[Finish Boot][req]\n");
#endif

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_INIT_SETTING, E_CAM_IF_SUB_COM_FINISH_BOOT) != 0){
		printf("error: Finish Boot.\n");
		return -1;
	}

	*result = ack.ret1;
	*err_code = ack.ret2;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[Finish Boot][res]result   = 0x%08X\n", *result);
	printf("[Finish Boot][res]err_code = 0x%08X\n", *err_code);
#endif

	gStatus_sample_program = IDLE_MODE;

	return 0;
}

/* set EV Compenstion */
int Set_Ev_Compenstion(UINT32* result, UINT32* err_code, UINT32 ev)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set EV Compenstion][req]ev = 0x%08X\n", ev);
#endif

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_SET_IMG_CFG, E_CAM_IF_SUB_COM_EV, ev) != 0){
		printf("error: set EV Compenstion.\n");
		return -1;
	}

	*result = ack.ret1;
	*err_code = ack.ret2;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set EV Compenstion][res]result   = 0x%08X\n", *result);
	printf("[set EV Compenstion][res]err_code = 0x%08X\n", *err_code);
#endif

	return 0;
}

/* set White Balance */
int Set_White_Balance(UINT32* result, UINT32* err_code, UINT32 wb, UINT32 r_gain, UINT32 b_gain)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set White Balance][req]wb     = 0x%08X\n", wb);
	printf("[set White Balance][req]r_gain = 0x%08X\n", r_gain);
	printf("[set White Balance][req]b_gain = 0x%08X\n", b_gain);
#endif

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_SET_IMG_CFG, E_CAM_IF_SUB_COM_WB, wb, r_gain, b_gain) != 0){
		printf("error: set White Balance.\n");
		return -1;
	}

	*result = ack.ret1;
	*err_code = ack.ret2;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set White Balance][res]result   = 0x%08X\n", *result);
	printf("[set White Balance][res]err_code = 0x%08X\n", *err_code);
#endif

	return 0;
}

/* set Color */
int Set_Color(UINT32* result, UINT32* err_code, UINT32 color)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set Color][req]color = 0x%08X\n", color);
#endif

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_SET_IMG_CFG, E_CAM_IF_SUB_COM_COLOR, color) != 0){
		printf("error: set Color.\n");
		return -1;
	}

	*result = ack.ret1;
	*err_code = ack.ret2;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set Color][res]result   = 0x%08X\n", *result);
	printf("[set Color][res]err_code = 0x%08X\n", *err_code);
#endif

	return 0;
}

/* set CC Matrix */
int Set_CC_Matrix(UINT32* result, UINT32* err_code, UINT32 matrix_line, UINT32 cc_matrix_0, UINT32 cc_matrix_1, UINT32 cc_matrix_2)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set CC Matrix][req]matrix_line = 0x%08X\n", matrix_line);
	printf("[set CC Matrix][req]cc_matrix_0 = 0x%08X\n", cc_matrix_0);
	printf("[set CC Matrix][req]cc_matrix_1 = 0x%08X\n", cc_matrix_1);
	printf("[set CC Matrix][req]cc_matrix_2 = 0x%08X\n", cc_matrix_2);
#endif

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_SET_IMG_CFG, E_CAM_IF_SUB_COM_CC_MATRIX, matrix_line, cc_matrix_0, cc_matrix_1, cc_matrix_2) != 0){
		printf("error: set White Balance.\n");
		return -1;
	}

	*result = ack.ret1;
	*err_code = ack.ret2;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set Color][res]result   = 0x%08X\n", *result);
	printf("[set Color][res]err_code = 0x%08X\n", *err_code);
#endif

	return 0;
}

/* set Sharpness */
int Set_Sharpness(UINT32* result, UINT32* err_code, UINT32 sharpness, UINT32 edge_strength)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set Sharpness][req]sharpness     = 0x%08X\n", sharpness);
	printf("[set Sharpness][req]edge_strength = 0x%08X\n", edge_strength);
#endif

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_SET_IMG_CFG, E_CAM_IF_SUB_COM_SHARPNESS, sharpness, edge_strength) != 0){
		printf("error: set Sharpness.\n");
		return -1;
	}

	*result = ack.ret1;
	*err_code = ack.ret2;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set Sharpness][res]result   = 0x%08X\n", *result);
	printf("[set Sharpness][res]err_code = 0x%08X\n", *err_code);
#endif

	return 0;
}

/* set ISO */
int Set_ISO(UINT32* result, UINT32* err_code, UINT32 iso)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set ISO][req]iso = 0x%08X\n", iso);
#endif

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_SET_IMG_CFG, E_CAM_IF_SUB_COM_ISO, iso) != 0){
		printf("error: set ISO.\n");
		return -1;
	}

	*result = ack.ret1;
	*err_code = ack.ret2;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set ISO][res]result   = 0x%08X\n", *result);
	printf("[set ISO][res]err_code = 0x%08X\n", *err_code);
#endif

	return 0;
}

/* set AE Coverage Speed */
int Set_AE_Coverage_Speed(UINT32* result, UINT32* err_code, UINT32 ae_coverage_speed)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set AE Coverage Speed][req]ae_coverage_speed = 0x%08X\n", ae_coverage_speed);
#endif

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_SET_IMG_CFG, E_CAM_IF_SUB_COM_AE_COVERAGE_SPEED, ae_coverage_speed) != 0){
		printf("error: set AE Coverage Speed.\n");
		return -1;
	}

	*result = ack.ret1;
	*err_code = ack.ret2;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set AE Coverage Speed][res]result   = 0x%08X\n", *result);
	printf("[set AE Coverage Speed][res]err_code = 0x%08X\n", *err_code);
#endif

	return 0;
}

/* set AE Lock */
int Set_AE_Lock(UINT32* result, UINT32* err_code, UINT32 ae_lock)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set AE Lock][req]ae_lock = 0x%08X\n", ae_lock);
#endif

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_SET_IMG_CFG, E_CAM_IF_SUB_COM_AE_LOCK, ae_lock) != 0){
		printf("error: set AE Lock.\n");
		return -1;
	}

	*result = ack.ret1;
	*err_code = ack.ret2;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set AE Lock][res]result   = 0x%08X\n", *result);
	printf("[set AE Lock][res]err_code = 0x%08X\n", *err_code);
#endif

	return 0;
}

/* set Size & Framerate */
int Set_Size_Framerate(UINT32* result, UINT32* err_code, UINT32 size, UINT32 frame_rate, UINT32 codec_type)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set Size & Framerate][req]size       = 0x%08X\n", size);
	printf("[set Size & Framerate][req]frame_rate = 0x%08X\n", frame_rate);
	printf("[set Size & Framerate][req]codec_type = 0x%08X\n", codec_type);
#endif

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_SET_VIDEO_CFG, E_CAM_IF_SUB_COM_V_SIZE_FRAMERATE, size, frame_rate, codec_type) != 0){
		printf("error: set Size & Framerate.\n");
		return -1;
	}

	*result = ack.ret1;
	*err_code = ack.ret2;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set Size & Framerate][res]result   = 0x%08X\n", *result);
	printf("[set Size & Framerate][res]err_code = 0x%08X\n", *err_code);
#endif

	return 0;
}

/* set Image_Type */
int Set_Image_Type(UINT32* result, UINT32* err_code, UINT32 format)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set Image_Type][req]format = 0x%08X\n", format);
#endif

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_SET_SINGLE_CAP_CFG, E_CAM_IF_SUB_COM_CAP_TYPE, format) != 0){
		printf("error: set Image_Type.\n");
		return -1;
	}

	*result = ack.ret1;
	*err_code = ack.ret2;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set Image_Type][res]result   = 0x%08X\n", *result);
	printf("[set Image_Type][res]err_code = 0x%08X\n", *err_code);
#endif

	return 0;
}

/* set Store_Channel */
int Set_Store_Channel(UINT32* result, UINT32* err_code, UINT32 sd_card_slot)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set Store_Channel][req]sd_card_slot = 0x%08X\n", sd_card_slot);
#endif

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_SET_SINGLE_CAP_CFG, E_CAM_IF_SUB_COM_CAP_CHANNEL, sd_card_slot) != 0){
		printf("error: set Image_Type.\n");
		return -1;
	}

	*result = ack.ret1;
	*err_code = ack.ret2;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set Store_Channel][res]result   = 0x%08X\n", *result);
	printf("[set Store_Channel][res]err_code = 0x%08X\n", *err_code);
#endif

	return 0;
}

/* set Capture_Size */
int Set_Capture_Size(UINT32* result, UINT32* err_code, UINT32 capture_size)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set Capture_Size][req]capture_size = 0x%08X\n", capture_size);
#endif

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_SET_SINGLE_CAP_CFG, E_CAM_IF_SUB_COM_CAP_SIZE, capture_size) != 0){
		printf("error: set Capture_Size.\n");
		return -1;
	}

	*result = ack.ret1;
	*err_code = ack.ret2;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set Capture_Size][res]result   = 0x%08X\n", *result);
	printf("[set Capture_Size][res]err_code = 0x%08X\n", *err_code);
#endif

	return 0;
}

/* set Quality */
int Set_Quality(UINT32* result, UINT32* err_code, UINT32 quality)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set Quality][req]quality = 0x%08X\n", quality);
#endif

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_SET_SINGLE_CAP_CFG, E_CAM_IF_SUB_COM_CAP_QUALITY, quality) != 0){
		printf("error: set Quality.\n");
		return -1;
	}

	*result = ack.ret1;
	*err_code = ack.ret2;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set Quality][res]result   = 0x%08X\n", *result);
	printf("[set Quality][res]err_code = 0x%08X\n", *err_code);
#endif

	return 0;
}

/* set Shooting_Condition */
int Set_Shooting_Condition(UINT32* result, UINT32* err_code, UINT32 shooting_number, UINT32 frame_rate)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set Shooting_Condition][req]shooting_number = 0x%08X\n", shooting_number);
	printf("[set Shooting_Condition][req]frame_rate      = 0x%08X\n", frame_rate);
#endif

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_SET_BURST_CAP_CFG, E_CAM_IF_SUB_COM_BURST_CAP_CONDITION, shooting_number, frame_rate) != 0){
		printf("error: set Shooting_Condition.\n");
		return -1;
	}

	*result = ack.ret1;
	*err_code = ack.ret2;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[set Quality][res]result   = 0x%08X\n", *result);
	printf("[set Quality][res]err_code = 0x%08X\n", *err_code);
#endif

	return 0;
}

/* Change Camera Mode */
int Change_Camera_Mode(UINT32* result, UINT32* err_code, UINT32 change_mode)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[Change Camera Mode][req]change_mode = 0x%08X\n", change_mode);
#endif

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_SET_MODE_CHANGE, E_CAM_IF_SUB_COM_CHANGE_MODE, change_mode) != 0){
		printf("error: Change Camera Mode.\n");
		return -1;
	}

	*result = ack.ret1;
	*err_code = ack.ret2;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[Change_Camera_Mode][res]result   = 0x%08X\n", *result);
	printf("[Change_Camera_Mode][res]err_code = 0x%08X\n", *err_code);
#endif

	gStatus_sample_program = change_mode;

	return 0;
}

/* Media Format */
int Format_SD_Card(UINT32* result, UINT32* err_code, UINT32 format)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_FORMAT, E_CAM_IF_SUB_COM_SD_CARD, format) != 0){
		printf("error: Media Format.\n");
		return -1;
	}

	*result = ack.ret1;
	*err_code = ack.ret2;

	return 0;
}

/* get EV Compenstion */
int Get_Ev_Compenstion(UINT32* ev)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_GET_IMG_CFG, E_CAM_IF_SUB_COM_EV) != 0){
		printf("error: get EV Compenstion.\n");
		return -1;
	}

	*ev = ack.ret1;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[get EV Compenstion][res]ev = 0x%08X\n", *ev);
#endif

	return 0;
}

/* get white balance */
int Get_White_Balance(UINT32* wb, UINT32* r_gain, UINT32* b_gain)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_GET_IMG_CFG, E_CAM_IF_SUB_COM_WB) != 0){
		printf("error: get white balance.\n");
		return -1;
	}

	*wb = ack.ret1;
	*r_gain = ack.ret2;
	*b_gain = ack.ret3;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[get white balance][res]wb     = 0x%08X\n", *wb);
	printf("[get white balance][res]r_gain = 0x%08X\n", *r_gain);
	printf("[get white balance][res]b_gain = 0x%08X\n", *b_gain);
#endif

	return 0;
}

/* get Color */
int Get_Color(UINT32* color)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_GET_IMG_CFG, E_CAM_IF_SUB_COM_COLOR) != 0){
		printf("error: get color.\n");
		return -1;
	}

	*color = ack.ret1;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[get color][res]color = 0x%08X\n", *color);
#endif

	return 0;
}

/* get CC Matrix */
int Get_CC_Matrix(UINT32* matrix_line, UINT32* cc_matrix_0, UINT32* cc_matrix_1, UINT32* cc_matrix_2)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_GET_IMG_CFG, E_CAM_IF_SUB_COM_CC_MATRIX) != 0){
		printf("error: get cc matrix.\n");
		return -1;
	}

	*matrix_line = ack.ret1;
	*cc_matrix_0 = ack.ret2;
	*cc_matrix_1 = ack.ret3;
	*cc_matrix_2 = ack.ret4;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[get cc matrix][res]matrix_line = 0x%08X\n", *matrix_line);
	printf("[get cc matrix][res]cc_matrix_0 = 0x%08X\n", *cc_matrix_0);
	printf("[get cc matrix][res]cc_matrix_1 = 0x%08X\n", *cc_matrix_1);
	printf("[get cc matrix][res]cc_matrix_2 = 0x%08X\n", *cc_matrix_2);
#endif

	return 0;
}

/* get Sharpness */
int Get_Sharpness(UINT32* sharpness, UINT32* edge_strength)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_GET_IMG_CFG, E_CAM_IF_SUB_COM_SHARPNESS) != 0){
		printf("error: get Sharpness.\n");
		return -1;
	}

	*sharpness = ack.ret1;
	*edge_strength = ack.ret2;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[get Sharpness][res]sharpness     = 0x%08X\n", *sharpness);
	printf("[get Sharpness][res]edge_strength = 0x%08X\n", *edge_strength);
#endif

	return 0;
}

/* get ISO */
int Get_ISO(UINT32* iso)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_GET_IMG_CFG, E_CAM_IF_SUB_COM_ISO) != 0){
		printf("error: get ISO.\n");
		return -1;
	}

	*iso = ack.ret1;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[get ISO][res]iso = 0x%08X\n", *iso);
#endif

	return 0;
}

/* get AE Coverage Speed */
int Get_AE_Coverage_Speed(UINT32* ae_coverage_speed)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_GET_IMG_CFG, E_CAM_IF_SUB_COM_AE_COVERAGE_SPEED) != 0){
		printf("error: get AE Coverage Speed.\n");
		return -1;
	}

	*ae_coverage_speed = ack.ret1;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[get AE Coverage Speed][res]ae_coverage_speed = 0x%08X\n", *ae_coverage_speed);
#endif

	return 0;
}

/* get AE Lock */
int Get_AE_Lock(UINT32* ae_lock)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_GET_IMG_CFG, E_CAM_IF_SUB_COM_AE_LOCK) != 0){
		printf("error: get AE Lock.\n");
		return -1;
	}

	*ae_lock = ack.ret1;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[get AE Lock][res]ae_lock = 0x%08X\n", *ae_lock);
#endif

	return 0;
}

/* get Size & Framerate */
int Get_Size_Framerate(UINT32* size, UINT32* frame_rate, UINT32* codec_type)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_GET_VIDEO_CFG, E_CAM_IF_SUB_COM_V_SIZE_FRAMERATE) != 0){
		printf("error: get Size & Framerate.\n");
		return -1;
	}

	*size = ack.ret1;
	*frame_rate = ack.ret2;
	*codec_type = ack.ret3;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[get Size & Framerate][res]size       = 0x%08X\n", *size);
	printf("[get Size & Framerate][res]frame_rate = 0x%08X\n", *frame_rate);
	printf("[get Size & Framerate][res]codec_type = 0x%08X\n", *codec_type);
#endif

	return 0;
}

/* get Image Type */
int Get_Image_Type(UINT32* format)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_GET_SINGLE_CAP_CFG, E_CAM_IF_SUB_COM_CAP_TYPE) != 0){
		printf("error: get Image Type.\n");
		return -1;
	}

	*format = ack.ret1;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[get Image Type][res]format = 0x%08X\n", *format);
#endif

	return 0;
}

/* get Store Channel */
int Get_Store_Channel(UINT32* sd_card_slot)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_GET_SINGLE_CAP_CFG, E_CAM_IF_SUB_COM_CAP_CHANNEL) != 0){
		printf("error: get Store Channel.\n");
		return -1;
	}

	*sd_card_slot = ack.ret1;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[get Store Channel][res]sd_card_slot = 0x%08X\n", *sd_card_slot);
#endif

	return 0;
}

/* get Capture Size */
int Get_Capture_Size(UINT32* capture_size)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_GET_SINGLE_CAP_CFG, E_CAM_IF_SUB_COM_CAP_SIZE) != 0){
		printf("error: get Capture Size.\n");
		return -1;
	}

	*capture_size = ack.ret1;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[get Capture Size][res]capture_size = 0x%08X\n", *capture_size);
#endif

	return 0;
}

/* get Quality */
int Get_Quality(UINT32* quality)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_GET_SINGLE_CAP_CFG, E_CAM_IF_SUB_COM_CAP_QUALITY) != 0){
		printf("error: get Quality.\n");
		return -1;
	}

	*quality = ack.ret1;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[get Quality][res]quality = 0x%08X\n", *quality);
#endif

	return 0;
}

/* get Shooting Condition */
int Get_Shooting_Condition(UINT32* shooting_number, UINT32* frame_rate)
{
	T_CPUCMD_COMMAND_ACK ack;

	/* Camera_IF Command Request */
	if(IPCU_Command_Send(&ack, E_CAM_IF_COM_GET_BURST_CAP_CFG, E_CAM_IF_SUB_COM_BURST_CAP_CONDITION) != 0){
		printf("error: get Shooting Condition.\n");
		return -1;
	}

	*shooting_number = ack.ret1;
	*frame_rate = ack.ret2;

	/* debug */
#ifdef CAMERA_IF_DEBUG
	printf("[get Shooting Condition][res]shooting_number = 0x%08X\n", *shooting_number);
	printf("[get Shooting Condition][res]frame_rate      = 0x%08X\n", *frame_rate);
#endif

	return 0;
}

/* console EV Compenstion */
static void console_ev_compenstion(void)
{
	INT8 str[16];
	UINT32 ev;
	UINT32 result;
	UINT32 err_code;

	/* get EV Compenstion */
	Get_Ev_Compenstion(&ev);

	printf("--- EV Compenstion (current:%x)---\n", ev);
	printf("[number]   [description]\n");
	printf("      0  : +3.0 EV\n");
	printf("      1  : +2.7 EV\n");
	printf("      2  : +2.3 EV\n");
	printf("      3  : +2.0 EV\n");
	printf("      4  : +1.7 EV\n");
	printf("      5  : +1.3 EV\n");
	printf("      6  : +1.0 EV\n");
	printf("      7  : +0.7 EV\n");
	printf("      8  : +0.3 EV\n");
	printf("      9  :  0.0 EV\n");
	printf("      a  : -0.3 EV\n");
	printf("      b  : -0.7 EV\n");
	printf("      c  : -1.0 EV\n");
	printf("      d  : -1.3 EV\n");
	printf("      e  : -1.7 EV\n");
	printf("      f  : -2.0 EV\n");
	printf("     10  : -2.3 EV\n");
	printf("     11  : -2.7 EV\n");
	printf("     12  : -3.0 EV\n");
	printf("----------------------------------\n");
	printf("Select Number? :");

	fgets(str, 16, stdin);
	ev = strtol(str, NULL, 16);

	/* EV Compenstion */
	Set_Ev_Compenstion(&result, &err_code, ev);
}

/* console White Balance */
static void console_white_balance(void)
{
	INT8 str[16];
	UINT32 result;
	UINT32 err_code;
	UINT32 wb;
	UINT32 r_gain;
	UINT32 b_gain;

	/* get White Balance */
	Get_White_Balance(&wb, &r_gain, &b_gain);

	printf("--- White Balance (current:%x)-------\n", wb);
	printf("[number]   [description]\n");
	printf("       0 :   Auto\n");
	printf("       1 : 12000K :Daylight Sky\n");
	printf("       2 :  8000K :Shade of Daylight\n");
	printf("       3 :  6500K :Cloudy Sky\n");
	printf("       4 :  5400K :Daylight\n");
	printf("       5 :  5000K :Fluorescent1\n");
	printf("       6 :  4200K :Fluorescent2\n");
	printf("       7 :  3500K :Fluorescent3\n");
	printf("       8 :  3000K :Incandescent\n");
	printf("       9 :  2000K :Setting sun\n");
	printf("       a :  Flash\n");
	printf("       b : Manual\n");
	printf("-------------------------------------\n");
	printf("Select Number? :");

	fgets(str, 16, stdin);
	wb = strtol(str, NULL, 16);

	if(wb == 0xb){
		printf("R gain? :");
		fgets(str, 16, stdin);
		r_gain = strtol(str, NULL, 16);
		printf("B gain? :");
		fgets(str, 16, stdin);
		b_gain = strtol(str, NULL, 16);
	}

	/* White Balance */
	Set_White_Balance(&result, &err_code, wb, r_gain, b_gain);
}

/* console Color */
static void console_color(void)
{
	INT8 str[16];
	UINT32 result;
	UINT32 err_code;
	UINT32 color;
	UINT32 matrix_line;
	UINT32 cc_matrix_0;
	UINT32 cc_matrix_1;
	UINT32 cc_matrix_2;

	/* get Color */
	Get_Color(&color);

	printf("--- Color (current:%x)-------\n", color);
	printf("[number]   [description]\n");
	printf("       0 :  Auto\n");
	printf("       1 :  Manual\n");
	printf("-------------------------------------\n");
	printf("Select Number? :");

	fgets(str, 16, stdin);
	color = strtol(str, NULL, 16);

	if(color == 0){
		/* set Color */
		Set_Color(&result, &err_code, color);
	}
	else if(color == 1){
		/* set Color */
		Set_Color(&result, &err_code, color);

		/* get CC Matrix */
		Get_CC_Matrix(&matrix_line, &cc_matrix_0, &cc_matrix_1, &cc_matrix_2);

		printf("--- Matrix Line (current:%x)-------\n", matrix_line);
		printf("[number]   [description]\n");
		printf("       0 :  1 line matrix\n");
		printf("       1 :  2 line matrix\n");
		printf("       2 :  3 line matrix\n");
		printf("-------------------------------------\n");
		printf("Select Number? :");

		fgets(str, 16, stdin);
		matrix_line = strtol(str, NULL, 16);

		printf("CC matrix_0? :");
		fgets(str, 16, stdin);
		cc_matrix_0 = strtol(str, NULL, 16);
		printf("CC matrix_1? :");
		fgets(str, 16, stdin);
		cc_matrix_1 = strtol(str, NULL, 16);
		printf("CC matrix_2? :");
		fgets(str, 16, stdin);
		cc_matrix_2 = strtol(str, NULL, 16);

		/* set CC Matrix */
		Set_CC_Matrix(&result, &err_code, matrix_line, cc_matrix_0, cc_matrix_1, cc_matrix_2);

	}
	else{
		printf("warning: inputted the wrong number\n");
	}
}

/* console Sharpness */
static void console_sharpness(void)
{
	INT8 str[16];
	UINT32 result;
	UINT32 err_code;
	UINT32 sharpness;
	UINT32 edge_strength;

	/* get Sharpness */
	Get_Sharpness(&sharpness, &edge_strength);

	printf("--- Sharpness (current:%x)---\n", sharpness);
	printf("[number]   [description]\n");
	printf("       0 :  0 (Week)\n");
	printf("       1 :  1\n");
	printf("       2 :  2\n");
	printf("       3 :  3\n");
	printf("       4 :  4\n");
	printf("       5 :  5\n");
	printf("       6 :  6\n");
	printf("       7 :  7 (default)\n");
	printf("       8 :  8\n");
	printf("       9 :  9\n");
	printf("       a : 10\n");
	printf("       b : 11\n");
	printf("       c : 12\n");
	printf("       d : 13\n");
	printf("       e : 14 (Strong)\n");
	printf("-----------------------------\n");
	printf("Select Number? :");

	fgets(str, 16, stdin);
	sharpness = strtol(str, NULL, 16);

	printf("--- Edge Strength (current:%x)---\n", edge_strength);
	printf("[number]   [description]\n");
	printf("       0 :  0 (Week)\n");
	printf("       1 :  1\n");
	printf("       2 :  2\n");
	printf("       3 :  3\n");
	printf("       4 :  4\n");
	printf("       5 :  5\n");
	printf("       6 :  6\n");
	printf("       7 :  7 (default)\n");
	printf("       8 :  8\n");
	printf("       9 :  9\n");
	printf("       a : 10\n");
	printf("       b : 11\n");
	printf("       c : 12\n");
	printf("       d : 13\n");
	printf("       e : 14 (Strong)\n");
	printf("--------------------------------\n");
	printf("Select Number? :");

	fgets(str, 16, stdin);
	edge_strength = strtol(str, NULL, 16);

	/* set Sharpness */
	Set_Sharpness(&result, &err_code, sharpness, edge_strength);

	/* Idle */
	Idle();

	/* Preview */
	Preview();
}

/* console ISO */
static void console_iso(void)
{
	INT8 str[16];
	UINT32 result;
	UINT32 err_code;
	UINT32 iso;

	/* get ISO */
	Get_ISO(&iso);

	printf("--- ISO (current:%x)-----\n", iso);
	printf("[number]   [description]\n");
	printf("       0 : Auto\n");
	printf("       1 : ISO100\n");
	printf("       2 : ISO200\n");
	printf("       3 : ISO400\n");
	printf("       4 : ISO800\n");
	printf("       5 : ISO1600\n");
	printf("       6 : ISO3200\n");
	printf("       7 : ISO6400\n");
	printf("-------------------------\n");
	printf("Select Number? :");

	fgets(str, 16, stdin);
	iso = strtol(str, NULL, 16);

	/* set ISO */
	Set_ISO(&result, &err_code, iso);
}

/* console AE Coverage Speed */
static void console_ae_coverage_speed(void)
{
	INT8 str[16];
	UINT32 result;
	UINT32 err_code;
	UINT32 ae_coverage_speed;

	/* get AE Coverage Speed */
	Get_AE_Coverage_Speed(&ae_coverage_speed);

	printf("AE Coverage Speed?(current:%x) :", ae_coverage_speed);

	fgets(str, 16, stdin);
	ae_coverage_speed = strtol(str, NULL, 16);

	/* set AE Coverage Speed */
	Set_AE_Coverage_Speed(&result, &err_code, ae_coverage_speed);
}

/* console AE Lock */
static void console_ae_lock(void)
{
	INT8 str[16];
	UINT32 result;
	UINT32 err_code;
	UINT32 ae_lock;

	/* get AE Lock */
	Get_AE_Lock(&ae_lock);

	printf("--- AE Lock (current:%x)---\n", ae_lock);
	printf("[number]    [description]\n");
	printf("       0  : Unlock\n");
	printf("       1  : Lock\n");
	printf("----------------------\n");
	printf("Select Number? :");

	fgets(str, 16, stdin);
	ae_lock = strtol(str, NULL, 16);

	/* set AE Lock */
	Set_AE_Lock(&result, &err_code, ae_lock);
}

/* console size & framerate */
static void console_size_framerate(void)
{
	INT8 str[16];
	UINT32 result;
	UINT32 err_code;
	UINT32 size;
	UINT32 frame_rate;
	UINT32 codec_type;

	/* get size & framerate */
	Get_Size_Framerate(&size, &frame_rate, &codec_type);

	printf("--- Video Size (current:%x)---\n", size);
	printf("[number]   [description]\n");
	printf("       0 :  3840x2160\n");
	printf("-----------------------------\n");
	printf("Select Number? :");

	fgets(str, 16, stdin);
	size = strtol(str, NULL, 16);

	printf("--- Frame rate (current:%x)---\n", frame_rate);
	printf("[number]   [description]\n");
	printf("       2 :   60fps\n");
	printf("--------------------------------\n");
	printf("Select Number? :");

	fgets(str, 16, stdin);
	frame_rate = strtol(str, NULL, 16);

	printf("--- Codec type (current:%x)---\n", codec_type);
	printf("[number]   [description]\n");
	printf("       0 :  HEVC\n");
	printf("--------------------------------\n");
	printf("Select Number? :");

	fgets(str, 16, stdin);
	codec_type = strtol(str, NULL, 16);

	/* set size & framerate */
	Set_Size_Framerate(&result, &err_code, size, frame_rate, codec_type);
}

/* console image type */
static void console_image_type(void)
{
	INT8 str[16];
	UINT32 result;
	UINT32 err_code;
	UINT32 format;

	/* get Image Type */
	Get_Image_Type(&format);

	printf("--- Image Type (current:%x)---\n", format);
	printf("[number]   [description]\n");
	printf("       0 :  JPEG\n");
	printf("-----------------------------\n");
	printf("Select Number? :");

	fgets(str, 16, stdin);
	format = strtol(str, NULL, 16);

	/* set Image Type */
	Set_Image_Type(&result, &err_code, format);
}

/* console Store Channel */
static void console_store_channel(void)
{
	INT8 str[16];
	UINT32 result;
	UINT32 err_code;
	UINT32 sd_card_slot;

	/* get Store Channel */
	Get_Store_Channel(&sd_card_slot);

	printf("--- Image Type (current:%x)---\n", sd_card_slot);
	printf("[number]   [description]\n");
	printf("       1 :  SD1 Card\n");
	printf("-----------------------------\n");
	printf("Select Number? :");

	fgets(str, 16, stdin);
	sd_card_slot = strtol(str, NULL, 16);

	/* set Store Channel */
	Set_Store_Channel(&result, &err_code, sd_card_slot);
}

/* console Capture Size */
static void console_capture_size(void)
{
	INT8 str[16];
	UINT32 result;
	UINT32 err_code;
	UINT32 capture_size;

	/* get Capture Size */
	Get_Capture_Size(&capture_size);

	printf("--- Capture Size (current:%x)---\n", capture_size);
	printf("[number]   [description]\n");
	printf("       1 :  4000x3000	(4:3) 12M\n");
	printf("-----------------------------\n");
	printf("Select Number? :");

	fgets(str, 16, stdin);
	capture_size = strtol(str, NULL, 16);

	/* set Store Channel */
	Set_Capture_Size(&result, &err_code, capture_size);
}

/* console Quality */
static void console_quality(void)
{
	INT8 str[16];
	UINT32 result;
	UINT32 err_code;
	UINT32 quality;

	/* get Quality */
	Get_Quality(&quality);

	printf("--- Quality (current:%x)---\n", quality);
	printf("[number]   [description]\n");
	printf("       0 :  Low\n");
	printf("       1 :  Middle\n");
	printf("       2 :  High\n");
	printf("-----------------------------\n");
	printf("Select Number? :");

	fgets(str, 16, stdin);
	quality = strtol(str, NULL, 16);

	/* set Quality */
	Set_Quality(&result, &err_code, quality);
}

/* console Shooting_Condition */
static void console_shooting_condition(void)
{
	INT8 str[16];
	UINT32 result;
	UINT32 err_code;
	UINT32 shooting_number;
	UINT32 frame_rate;

	/* get Shooting_Condition */
	Get_Shooting_Condition(&shooting_number, &frame_rate);

	printf("--- Max Shooting Number (current:%x)---\n", shooting_number);
	printf("[number]   [description]\n");
	printf("       1 :    1pcs\n");
	printf("     ... :     ...\n");
	printf("      1e :   30pcs\n");
	printf("     ... :     ...\n");
	printf("      64 :  100pcs\n");
	printf("-----------------------------\n");
	printf("Select Number? :");

	fgets(str, 16, stdin);
	shooting_number = strtol(str, NULL, 16);

	printf("--- Frame Rate (current:%x)---\n", frame_rate);
	printf("[number]   [description]\n");
	printf("       1 :    5fps\n");
	printf("       2 :   10fps\n");
	printf("       3 :   15fps\n");
	printf("       4 :   30fps\n");
	printf("-----------------------------\n");
	printf("Select Number? :");

	fgets(str, 16, stdin);
	frame_rate = strtol(str, NULL, 16);

	/* set Shooting_Condition */
	Set_Shooting_Condition(&result, &err_code, shooting_number, frame_rate);
}

/* console Change Camera Mode */
static void console_change_camera_mode(void)
{
	INT8 str[16];
	UINT32 result;
	UINT32 err_code;
	UINT32 mode;

	printf("--- Change Camera Mode ---\n");
	printf("[number]   [description]\n");
	printf("       0 :  Boot\n");
	printf("       1 :  IDLE Mode\n");
	printf("       2 :  Preview Mode\n");
	printf("       3 :  Single Capture Mode\n");
	printf("       4 :  Burst Capture Mode\n");
	printf("       8 :  Video Capture Mode\n");
	printf("-----------------------------\n");
	printf("Select Number? :");

	fgets(str, 16, stdin);
	mode = strtol(str, NULL, 16);

	/* Change Camera Mode */
	Change_Camera_Mode(&result, &err_code, mode);
}

/* console Video Capture */
int console_video_capture(void)
{
	UINT32 event_timeout = 0;
	UINT32 result;
	UINT32 err_code;

	/* Preview */
	Preview();

	/* Video Capture start */
	Change_Camera_Mode(&result, &err_code, VIDEO_MODE);

	/* This sample record 10 seconds */
	sleep(10); /* sec */

	/* Video Captur stop */
	Change_Camera_Mode(&result, &err_code, PREVIEW_MODE);

	/* wait Event for Video Capture Finished */
	if(gFlag_video_capture_complete == 1){
		gFlag_video_capture_complete = 0;
	}
	else{
		printf("waiting for event from RTOS...\n");
		while(1){
			if(gFlag_video_capture_complete == 1){
				gFlag_video_capture_complete = 0;
				break;
			}

			++event_timeout;
			sleep(1); /* sec */

			if(event_timeout > 30){ /* application creator decides */
				/* RTOS not return event */
				printf("error: event timeout.\n");
				return -2;
			}
		}
	}

	return 0;
}

/* console Single Capture */
int console_single_capture(void)
{
	UINT32 event_timeout = 0;
	UINT32 result;
	UINT32 err_code;

	/* Preview */
	Preview();

	/* Single Capture start */
	Change_Camera_Mode(&result, &err_code, SINGLE_MODE);

	/* wait Event for Burst Capture Finished */
	if(gFlag_single_capture_complete == 1){
		gFlag_single_capture_complete = 0;
	}
	else{
		printf("waiting for event from RTOS...\n");
		while(1){
			if(gFlag_single_capture_complete == 1){
				gFlag_single_capture_complete = 0;
				break;
			}

			++event_timeout;
			sleep(1); /* sec */

			if(event_timeout > 30){ /* application creator decides */
				/* RTOS not return event */
				printf("error: event timeout.\n");
				return -2;
			}
		}
	}

	/* Preview */
	Preview();

	return 0;
}

/* console Media Format */
static void console_media_format(void)
{
	INT8 str[16];
	UINT32 format;

	printf("\nreally sd card format ok? (y/n) :");
	fgets(str, 16, stdin);
	if(strcmp(str, "y\n") == 0){
	}
	else if(strcmp(str, "n\n") == 0){
		return;
	}
	else{
		return;
	}

	printf("--- Format ---\n");
	printf("[number]   [description]\n");
	printf("       1 :  SD1 Card\n");
	printf("-----------------------------\n");
	printf("Select Number? :");

	fgets(str, 16, stdin);
	format = strtol(str, NULL, 16);

	Media_Format(format);
}

/* console menu */
static int console_menu(void)
{
	INT8 str[16];
	UINT32 number = 0;

	printf("### Camera_IF CUI Menu ###########\n");
	printf("# Image  setting                 #\n");
	printf("#   i1: EV Compenstion           #\n");
	printf("#   i2: White Balance            #\n");
	printf("#   i3: Color                    #\n");
	printf("#   i4: Sharpness                #\n");
	printf("#   i5: ISO                      #\n");
	printf("#   i6: AE Coverage Speed        #\n");
	printf("#   i7: AE Lock                  #\n");
	printf("# Video  setting                 #\n");
	printf("#   v1: Video Size/Frame/Codec   #\n");
	printf("# Still  setting                 #\n");
	printf("#   s1: Image Type               #\n");
	printf("#   s2: Store Channel            #\n");
	printf("#   s3: Capture Size             #\n");
	printf("#   s4: Quality                  #\n");
	printf("# Camera  mode                   #\n");
	printf("#   c1: IDLE                     #\n");
	printf("#   c2: Preview                  #\n");
	printf("#   c3: Video Capture            #\n");
	printf("#   c4: Single Capture           #\n");
	printf("#   c5: Media Format             #\n");
	printf("# Exit                           #\n");
	printf("#    e: menu exit                #\n");
	printf("##################################\n");
	printf("Select Number? :");
	fgets(str, 16, stdin);

	if(strcmp(str, "i1\n") == 0){
		console_ev_compenstion();
	}
	else if(strcmp(str, "i2\n") == 0){
		console_white_balance();
	}
	else if(strcmp(str, "i3\n") == 0){
		console_color();
	}
	else if(strcmp(str, "i4\n") == 0){
		console_sharpness();
	}
	else if(strcmp(str, "i5\n") == 0){
		console_iso();
	}
	else if(strcmp(str, "i6\n") == 0){
		console_ae_coverage_speed();
	}
	else if(strcmp(str, "i7\n") == 0){
		console_ae_lock();
	}
	else if(strcmp(str, "v1\n") == 0){
		console_size_framerate();
	}
	else if(strcmp(str, "s1\n") == 0){
		console_image_type();
	}
	else if(strcmp(str, "s2\n") == 0){
		console_store_channel();
	}
	else if(strcmp(str, "s3\n") == 0){
		console_capture_size();
	}
	else if(strcmp(str, "s4\n") == 0){
		console_quality();
	}
	else if(strcmp(str, "c1\n") == 0){
		Idle();
	}
	else if(strcmp(str, "c2\n") == 0){
		Preview();
	}
	else if(strcmp(str, "c3\n") == 0){
		console_video_capture();
	}
	else if(strcmp(str, "c4\n") == 0){
		console_single_capture();
	}
	else if(strcmp(str, "c5\n") == 0){
		console_media_format();
	}
	else if(strcmp(str, "e\n") == 0){
		return -1;
	}
	else{
		printf("Incorrect input\n");
	}

	return 0;
}

/* open menu */
static int open_menu(void)
{
	INT8 str[16];

	printf("open CUI menu?(y/n) :");
	fgets(str, 16, stdin);

	while(1){
		if(strcmp(str, "y\n") == 0){
			return 0;
		}
		else if(strcmp(str, "n\n") == 0){
			return -1;
		}
		else{
			printf("open CUI menu?(y/n) :");
			fgets(str, 16, stdin);
		}
	}

	return 0;
}

/* CUI menu */
void Sample_Menu(void)
{
	UINT32 number = 0;
	INT8 str[16];

	while(1){
		if(console_menu() != 0){
			break;
		}
		if(open_menu() != 0){
			break;
		}
	}
}

/* Boot up */
int Boot_Up(void)
{
	UINT32 result;
	UINT32 err_code;

	if(gStatus_sample_program != BOOT_MODE){
		return 0;
	}

	/* Finish Boot */
	if(Finish_Boot(&result, &err_code) != 0){
		printf("error: Boot up.\n");
		return -1;
	}

	return 0;
}

/* Idle */
int Idle(void)
{
	UINT32 result;
	UINT32 err_code;

	/* Configurable camera settings */
	  // Finish_Boot(&result, &err_code);
	  // Set_Ev_Compenstion(&result, &err_code, ev);
	  // Set_White_Balance(&result, &err_code, wb, r_gain, b_gain);
	  // Set_Color(&result, &err_code, color);
	  // Set_CC_Matrix(&result, &err_code, matrix_line, cc_matrix_0, cc_matrix_1, cc_matrix_2);
	  // Set_Sharpness(&result, &err_code, sharpness, edge_strength);
	  // Set_ISO(&result, &err_code, iso);
	  // Set_AE_Coverage_Speed(&result, &err_code, ae_coverage_speed);
	  // Set_AE_Lock(&result, &err_code, ae_lock);
	  // Set_Size_Framerate(&result, &err_code, size, frame_rate, codec_type);
	  // Set_Image_Type(&result, &err_code, format);
	  // Set_Store_Channel(&result, &err_code, sd_card_slot);
	  // Set_Capture_Size(&result, &err_code, capture_size);
	  // Set_Quality(&result, &err_code, quality);

	/* Change Camera Mode */
	if(Change_Camera_Mode(&result, &err_code, IDLE_MODE) != 0){
		printf("error: Idle.\n");
		return -1;
	}

	return 0;
}

static int check_camifts(void)
{
	FILE* fp;

	fp = fopen("camifts_done", "r");
	if(fp == NULL){
		fp = fopen("camifts_done", "w");
		if(fp == NULL){
			printf("error: failed fopen camifts_done.\n");
			return -1;
		}
		fclose(fp);
		return 0;
	}
	else{
		fclose(fp);
		return 1;
	}
}

/* Preview */
int Preview(void)
{
	UINT32 result;
	UINT32 err_code;

	if(gStatus_sample_program == BOOT_MODE){
		Boot_Up();
		if(check_camifts() != 1){
			system("camifts &");
		}
	}

	/* Configurable camera settings */
	  // Set_Ev_Compenstion(&result, &err_code, ev);
	  // Set_White_Balance(&result, &err_code, wb, r_gain, b_gain);
	  // Set_Color(&result, &err_code, color);
	  // Set_CC_Matrix(&result, &err_code, matrix_line, cc_matrix_0, cc_matrix_1, cc_matrix_2);
	  // Set_Sharpness(&result, &err_code, sharpness, edge_strength);
	  // Set_ISO(&result, &err_code, iso);
	  // Set_AE_Coverage_Speed(&result, &err_code, ae_coverage_speed);
	  // Set_AE_Lock(&result, &err_code, ae_lock);
	  // Set_Size_Framerate(&result, &err_code, size, frame_rate, codec_type);
	  // Set_Image_Type(&result, &err_code, format);
	  // Set_Store_Channel(&result, &err_code, sd_card_slot);
	  // Set_Capture_Size(&result, &err_code, capture_size);
	  // Set_Quality(&result, &err_code, quality);

	if(gStatus_sample_program != PREVIEW_MODE){
		/* Change Camera Mode */
		if(Change_Camera_Mode(&result, &err_code, PREVIEW_MODE) != 0){
			printf("error: Preview.\n");
			return -1;
		}
		gStatus_sample_program = PREVIEW_MODE;
	}

	return 0;
}

/* Video Capture */
int Video_Capture(UINT32 v_size, UINT32 v_frame_rate, UINT32 v_codec_type)
{
	UINT32 event_timeout = 0;
	UINT32 result;
	UINT32 err_code;

	/* Preview */
	Preview();

	/* Set Video Capture Configuration */
	Set_Size_Framerate(&result, &err_code, v_size, v_frame_rate, v_codec_type);

	/* Video Capture start */
	Change_Camera_Mode(&result, &err_code, VIDEO_MODE);

	/* This sample record 10 seconds */
	sleep(10); /* sec */

	/* Video Captur stop */
	Change_Camera_Mode(&result, &err_code, PREVIEW_MODE);

	/* wait Event for Video Capture Finished */
	if(gFlag_video_capture_complete == 1){
		gFlag_video_capture_complete = 0;
	}
	else{
		printf("waiting for event from RTOS...\n");
		while(1){
			if(gFlag_video_capture_complete == 1){
				gFlag_video_capture_complete = 0;
				break;
			}

			++event_timeout;
			sleep(1); /* sec */

			if(event_timeout > 30){ /* application creator decides */
				/* RTOS not return event */
				printf("error: event timeout.\n");
				return -2;
			}
		}
	}

	return 0;
}

int Single_Capture(	UINT32 s_format,
										UINT32 s_sd_card_slot,
										UINT32 s_capture_size,
										UINT32 s_quality)
{
	UINT32 event_timeout = 0;
	UINT32 result;
	UINT32 err_code;

	/* Preview */
	Preview();

	/* Set Image Capture Configuration */
	Set_Image_Type(&result, &err_code, s_format);
	Set_Store_Channel(&result, &err_code, s_sd_card_slot);
	Set_Capture_Size(&result, &err_code, s_capture_size);
	Set_Quality(&result, &err_code, s_quality);

	/* Single Capture start */
	Change_Camera_Mode(&result, &err_code, SINGLE_MODE);

	/* wait Event for Burst Capture Finished */
	if(gFlag_single_capture_complete == 1){
		gFlag_single_capture_complete = 0;
	}
	else{
		printf("waiting for event from RTOS...\n");
		while(1){
			if(gFlag_single_capture_complete == 1){
				gFlag_single_capture_complete = 0;
				break;
			}

			++event_timeout;
			sleep(1); /* sec */

			if(event_timeout > 30){ /* application creator decides */
				/* RTOS not return event */
				printf("error: event timeout.\n");
				return -2;
			}
		}
	}

	/* Preview */
	Preview();

	return 0;
}

/* Media Format */
int Media_Format(UINT32 format)
{
	UINT32 result;
	UINT32 err_code;

	if(gStatus_sample_program == BOOT_MODE){
		Boot_Up();
	}

	Idle();

	/* Media Format */
	if(Format_SD_Card(&result, &err_code, format) != 0){
		printf("error: Media Format.\n");
		return -1;
	}

	return 0;
}

