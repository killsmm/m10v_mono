/**
 * @file		ct_im_iip_frecttbl.h
 * @brief		This is ct header for im_iip_frecttbl.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _CT_IM_IIP_FRECTTBL_H_
#define _CT_IM_IIP_FRECTTBL_H_


// im_iip header
#include "im_iip_param.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

// FRECT table number (for FullHD 2340x1296)
#define D_CT_IM_IIP_FRECT_TBL_FULLHD_VIS_0_X_NUM	(18)		// (2304 / 64 /2)
#define D_CT_IM_IIP_FRECT_TBL_FULLHD_VIS_0_Y_NUM	(162)		// (1296 / 8)
#define D_CT_IM_IIP_FRECT_TBL_FULLHD_VIS_0_NUM		(D_CT_IM_IIP_FRECT_TBL_FULLHD_VIS_0_X_NUM * D_CT_IM_IIP_FRECT_TBL_FULLHD_VIS_0_Y_NUM)
#define D_CT_IM_IIP_FRECT_TBL_FULLHD_VIS_0_X_OFS	(0)
#define D_CT_IM_IIP_FRECT_TBL_FULLHD_VIS_1_X_NUM	(18)		// (2304 / 64 /2)
#define D_CT_IM_IIP_FRECT_TBL_FULLHD_VIS_1_Y_NUM	(162)		// (1296 / 8)
#define D_CT_IM_IIP_FRECT_TBL_FULLHD_VIS_1_NUM		(D_CT_IM_IIP_FRECT_TBL_FULLHD_VIS_1_X_NUM * D_CT_IM_IIP_FRECT_TBL_FULLHD_VIS_1_Y_NUM)
#define D_CT_IM_IIP_FRECT_TBL_FULLHD_VIS_1_X_OFS	(D_CT_IM_IIP_FRECT_TBL_FULLHD_VIS_1_X_NUM * D_CT_IM_IIP_FRECT_TBL_FULLHD_VIS_X_SZ)


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern T_IM_IIP_PARAM_FRECT_OPCOL_0 g_ct_im_iip_frecttbl_0_fullhd_vis[D_CT_IM_IIP_FRECT_TBL_FULLHD_VIS_0_NUM];
extern T_IM_IIP_PARAM_FRECT_OPCOL_0 g_ct_im_iip_frecttbl_1_fullhd_vis[D_CT_IM_IIP_FRECT_TBL_FULLHD_VIS_1_NUM];

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus


#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _CT_IM_IIP_FRECTTBL_H_

