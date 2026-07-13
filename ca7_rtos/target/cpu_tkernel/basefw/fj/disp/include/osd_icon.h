/**
 * @file		osd_icon.h
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _OSD_ICON_H_
#define _OSD_ICON_H_

#ifndef INCLUDE_MADE
#include "fj_disp.h"
#include "osd_icon_comm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
Draw icon on VGR
@param[in]	disp_no_data	Draw layer
@param[in]	draw_positon	display position
@param[in]	src_addr		icon data
@param[in]	DrawMode		draw mode. See E_DrawMode.
@param[in]	alpha			attribute level for blending when a icon is drawn.<br>
							It's valid when DrawMode is E_DrawMode_Effect_Optimized only.<br>
							And it's valid on GR layer only. It can be set 0~255.
@retval		FJ_ERR_OK OK
*/
FJ_ERR_CODE BF_Osd_Icon_Draw_GR_Icon(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, const T_POS* draw_positon, const T_BF_OSD_ICON_COMM_ICON_INFO* src_addr
									,E_DrawMode DrawMode, BYTE alpha);


#endif // INCLUDE_MADE
#endif // _OSD_ICON_H_
