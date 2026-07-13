/**
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 * @file		: camera_focus_zc525.h
 * @brief 		: focus driver for ZC525
 * @note		: None
 * @attention	: None
 */
 
#ifndef _CAMERA_FOCUS_M43_H
#define _CAMERA_FOCUS_M43_H
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
extern const T_CAMERA_FOCUS_DOF		M43_DOF[E_CAMERA_FOCUS_DOF_MAX];
extern T_CAMERA_FOCUS_BASIC	M43_BASIC;

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/

extern E_CAMERA_ERR_CODE	Focus_VCM_M43_Init(E_CAMERA_ID focus_id, T_CAMERA_FOCUS_PARAM* prm);
extern E_CAMERA_ERR_CODE	Focus_VCM_M43_Move(E_CAMERA_ID focus_id, E_CAMERA_FOCUS_MOVE_DIR dir, BOOL is_coarse, USHORT* value);
extern E_CAMERA_ERR_CODE	Focus_VCM_M43_SetDAC(E_CAMERA_ID focus_id, USHORT dac);
extern E_CAMERA_ERR_CODE	Focus_VCM_M43_GetDAC(E_CAMERA_ID focus_id, USHORT* value);

extern E_CAMERA_ERR_CODE	Focus_VCM_M43_CalcDAC(E_CAMERA_ID focus_id, T_CAMERA_FOCUS_PARAM* prm, UCHAR convert);

extern E_CAMERA_ERR_CODE	Focus_VCM_M43_StartMF(bool mf_stat);
extern E_CAMERA_ERR_CODE	Focus_VCM_M43_ContAPapxABS(uint16_t targetAperure);
extern E_CAMERA_ERR_CODE	Focus_VCM_M43_ContLDdstABS(uint16_t abs_distance);
#endif

