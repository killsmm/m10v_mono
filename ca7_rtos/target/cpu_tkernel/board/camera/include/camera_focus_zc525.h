/**
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 * @file		: camera_focus_zc525.h
 * @brief 		: focus driver for ZC525
 * @note		: None
 * @attention	: None
 */
 
#ifndef _CAMERA_FOCUS_ZC525_H
#define _CAMERA_FOCUS_ZC525_H

#include "ddim_typedef.h"
#include "camera_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern const T_CAMERA_FOCUS_DOF		Z525_DOF[E_CAMERA_FOCUS_DOF_MAX];
extern const T_CAMERA_FOCUS_BASIC	ZC525_BASIC;

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
extern E_CAMERA_ERR_CODE	Focus_VCM_ZC525_Init(E_CAMERA_ID focus_id, T_CAMERA_FOCUS_PARAM* prm);
extern E_CAMERA_ERR_CODE	Focus_VCM_ZC525_SetDAC(E_CAMERA_ID focus_id, USHORT dac);
extern E_CAMERA_ERR_CODE	Focus_VCM_ZC525_GetDAC(E_CAMERA_ID focus_id, USHORT* value);
extern E_CAMERA_ERR_CODE	Focus_VCM_ZC525_Move(E_CAMERA_ID focus_id, E_CAMERA_FOCUS_MOVE_DIR dir, BOOL is_coarse, USHORT* value);
extern E_CAMERA_ERR_CODE	Focus_VCM_ZC525_CalcDAC(E_CAMERA_ID focus_id, T_CAMERA_FOCUS_PARAM* prm, UCHAR convert);

#endif

