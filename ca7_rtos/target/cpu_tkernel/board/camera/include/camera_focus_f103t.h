/**
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 * @file		: camera_focus_zc525.h
 * @brief 		: focus driver for ZC525
 * @note		: None
 * @attention	: None
 */
 
#ifndef _CAMERA_FOCUS_F103T_H
#define _CAMERA_FOCUS_F103T_H
#include <stdint.h>
#include <stdbool.h>
#include "ddim_typedef.h"
#include "camera_common.h"


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern const T_CAMERA_FOCUS_DOF		F103T_DOF[E_CAMERA_FOCUS_DOF_MAX];
extern T_CAMERA_FOCUS_BASIC	F103T_BASIC;

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
typedef enum
{
    /* data */
    F130T_PARAM_ZOOM1 = 0x00,
    F130T_PARAM_ZOOM2,
    F130T_PARAM_FOCUS_NEAR,
    F130T_PARAM_FOCUS_FAR,
    F130T_PARAM_ENABLE,
}F130T_PARAM_MODE;


extern E_CAMERA_ERR_CODE	Focus_VCM_F103T_Init(E_CAMERA_ID focus_id, T_CAMERA_FOCUS_PARAM* prm);
extern E_CAMERA_ERR_CODE	Focus_VCM_F103T_Move(E_CAMERA_ID focus_id, E_CAMERA_FOCUS_MOVE_DIR dir, BOOL is_coarse, USHORT* value);
extern E_CAMERA_ERR_CODE	Focus_VCM_F103T_SetDAC(E_CAMERA_ID focus_id, USHORT dac);
extern E_CAMERA_ERR_CODE	Focus_VCM_F103T_GetDAC(E_CAMERA_ID focus_id, USHORT* value);

extern E_CAMERA_ERR_CODE	Focus_VCM_F103T_CalcDAC(E_CAMERA_ID focus_id, T_CAMERA_FOCUS_PARAM* prm, UCHAR convert);

extern E_CAMERA_ERR_CODE	Focus_VCM_F103T_StartMF(bool mf_stat);
extern E_CAMERA_ERR_CODE	Focus_VCM_F103T_ContAPapxABS(uint16_t targetAperure);
extern E_CAMERA_ERR_CODE	Focus_VCM_F103T_FOUCS(uint16_t abs_distance);

// extern E_CAMERA_ERR_CODE    Focus_VCM_F103T_ZOOM1(uint16_t abs_setups);
// extern E_CAMERA_ERR_CODE    Focus_VCM_F103T_ZOOM2(uint16_t abs_setups);
extern E_CAMERA_ERR_CODE    Focus_VCM_F103T_Set_Focal_Length(int32_t flen);

extern E_CAMERA_ERR_CODE	Focus_VCM_F103T_FOUCS_ABS(uint16_t abs_distance);
extern E_CAMERA_ERR_CODE    Focus_VCM_F103T_Set_F130T_PARAM (int zoom1 , int zoom2 , int focusNear, int focusFar);
// 1 . 黑夜模式 ， 0. 白天模式
extern E_CAMERA_ERR_CODE    Focus_VCM_F103T_Set_IR_CUT (int mode);

extern E_CAMERA_ERR_CODE    Focus_VCM_F103T_Set_PARAM (F130T_PARAM_MODE mode, int param);

extern E_CAMERA_ERR_CODE    Focus_VCM_F103T_Get_Cur_Focus_Value(uint16_t* focus_Value);

#endif

