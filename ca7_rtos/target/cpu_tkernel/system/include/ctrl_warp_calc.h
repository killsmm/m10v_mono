/**
 * @file		ctrl_warp_calc.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _CTRL_WARP_CALC_H_
#define _CTRL_WARP_CALC_H_

#include "ip_ge.h"
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_LOCAL_GRID_H_MAX_NUM	(8192/64)
#define D_LOCAL_GRID_V_MAX_NUM	(8192/32)

#define FLG_LDC_CALC_START		0x00000001
#define FLG_LDC_CALC_END		0x80000000
#define FLG_LDC_CALC_WAITPTN	(FLG_LDC_CALC_START | FLG_LDC_CALC_END)

#define FLG_LDC_CALC_DONE			0x00000001
#define FLG_LDC_CALC_DONE_WAITPTN	(FLG_LDC_CALC_DONE)
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	unsigned long	tbl_address;
	unsigned short	in_img_h;
	unsigned short	in_img_v;
	unsigned short	in_num_cell_h;
	unsigned short	in_num_cell_v;
    T_IP_GE_LGRID 	*in_local;
	T_IP_GE_LGRID 	in_local_work;
    T_IP_GE_LNODE 	*in_nodes_coor;	
	T_IP_GE_LNODE 	*in_work_nodes_coor;
	T_IP_GE_LNODE 	*in_work_nodes;
	unsigned short	out_img_h;
	unsigned short	out_img_v;
	unsigned short	out_num_cell_h;
	unsigned short	out_num_cell_v;
    T_IP_GE_LGRID   *rsz_local;
    T_IP_GE_LNODE 	*rsz_nodes_coor;	
    T_IP_GE_LGRID   *inter_local;
    T_IP_GE_LNODE 	*inter_nodes_coor;
	T_IP_GE_LGRID	out_inter_local;
		
	FLOAT			zoom_ratio;
	FLOAT			zoom_pos_x;
	FLOAT			zoom_pos_y;
	FLOAT 			zoom_s_x;
	FLOAT 			zoom_s_y;
	unsigned char	_1st_ldc_calc;
} T_LDC_IMG_INFO;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

extern void Ctrl_Warp_Calculation(T_LDC_IMG_INFO* img_info);

#ifdef __cplusplus
}
#endif

#endif	// _CTRL_WARP_CALC_H_

