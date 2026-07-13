/**
 * @file		user_utility_disp.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#include "user_utility_disp.h"
#include "fj_disp.h"
#include "disp.h"
//#include "dd_top.h"
#include "drv_lcd.h"
#include "lcd_config.h"
#include "detect_task.h"
#include "im_disp.h"
#include "user.h"
#include "fj_filesystem.h"
#include "l1l2cache.h"
#include "sdram_map_define.h"
#include "osd_draw_comm.h"
/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/

#define D_USER_UTILITY_DISP_LCD_IMG_H				(480)
#define D_USER_UTILITY_DISP_LCD_IMG_V				(320)

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static UINT32 g_User_Disp_Lcd_Osd_Width = 0;
static UINT32 g_User_Disp_Lcd_Osd_Line = 0;
static UINT32 g_User_Disp_Hdmi_Osd_Width = 0;
static UINT32 g_User_Disp_Hdmi_Osd_Line = 0;

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static void lcd_config(void);
static void user_utility_disp_size_calc(FJ_DISP_WINDOW_CONFIG* disp_win);
static void hdmi_vsync_callback( void );
static void lcd_vsync_callback( void );

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
INT32 User_Utility_HDMI_Yuv( const T_SHARE_LAYOUT* yuv )
{
	FJ_DISP_WINDOW_CONFIG	disp_window;
	FJ_DISP_YCC_ADDR		addr;
	E_BF_DISP_IMAGE_FORMAT	fmt;
	ULONG					global_width;
	FLOAT					in_aspect, out_aspect;
	T_SHARE_SYSTEM_CONFIG 	syscfg;
	FJ_ERR_CODE 			ercd = 0;
	U_IM_DISP_YR_MATRIX_COEFFICIENT yr_matrix_to_709[D_IM_DISP_MATRIX_SIZE] = {
		{ 0x000064CA00004000 },	// 0x4000,0x0000,0x64CA (1.0,  0.0,      1.5748)
		{ 0x0000E20BF4034000 },	// 0x4000,0xF403,0xE20B (1.0, -0.1873,  -0.4681)
		{ 0x0000000076C24000 }	// 0x4000,0x76C2,0x0000 (1.0,  1.85565,  0.0)
	};
	U_IM_DISP_YR_MATRIX_COEFFICIENT yr_matrix_to_601[D_IM_DISP_MATRIX_SIZE] = {
		{ 0x000059BA00004000 },	// 0x4000,0x0000,0x59BA (1.0,  0.0,     1.402)
		{ 0x0000D24CE9FA4000 },	// 0x4000,0xE9FA,0xD24C (1.0, -0.3441,  -0.7141)
		{ 0x0000000071684000 }	// 0x4000,0x7168,0x0000 (1.0,  1.772,   0.0)
	};
	U_IM_DISP_YR_MATRIX_COEFFICIENT ry_matrix_to_709[D_IM_DISP_MATRIX_SIZE] = {
		{ 0x0000049F2DC60D9B },	// 0xD9B, 0x2DC6,0x49F  (0.2126,  0.7152,   0.0722)
		{ 0x00002000E756F8AA },	// 0xF8AA,0xE756,0x2000 (-0.1146, -0.3854,  0.5)
		{ 0x0000FD12E2EE2000 }	// 0x2000,0xE2EE,0xFD12 (0.5,	  -0.4542,  -0.0458)
	};
	U_IM_DISP_YR_MATRIX_COEFFICIENT ry_matrix_to_601[D_IM_DISP_MATRIX_SIZE] = {
		{ 0x0000074C25911323 },	// 0x1323,0x2591,0x074C (0.299,   0.587,    0.114)
		{ 0x00002000EACDF533 },	// 0xF533,0xEACD,0x2000 (-0.1687, -0.3313,  0.5)
		{ 0x0000FAD1E5342000 }	// 0x2000,0xE534,0xFAD1 (0.5,     -0.4187,  -0.081)
	};
	T_IM_DISP_CLIP_CAL clip_cal;

	if(FJ_Disp_HDMI_Get_Detect_Status() == FJ_DISP_HDMI_DETECT_DISCONNECTED){
		return NG;
	}
	
	//MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_DISP, FJ_MBALOG_FUNC_ID_YUV, FJ_MBALOG_TYPE_POINT, "HDMI Set %08lx", yuv->addr_y ));
	
	addr.y_addr    = yuv->addr_y;
	addr.cbcr_addr = yuv->addr_c;
	global_width = yuv->h_byte;

	if (yuv->video == E_SHARE_YUV_VIDEO_NONE) {
		switch( yuv->format ){
		case E_SHARE_YUV_FORMAT_422_SEMI:
			fmt = E_BF_DISP_IMAGE_FORMAT_422;
			break;
		case E_SHARE_YUV_FORMAT_420_SEMI:
		default:
			fmt = E_BF_DISP_IMAGE_FORMAT_420;
			break;
		}
	} else {
		fmt = E_BF_DISP_IMAGE_FORMAT_VIDEO;
	}

	disp_window.in_x_pos = 0;
	disp_window.in_y_pos = 0;
	disp_window.in_width = yuv->h_size;
	disp_window.in_lines = yuv->v_size;
	disp_window.out_x_pos = 0;
	disp_window.out_y_pos = 0;
	hdmi_get_video_id_reso(&disp_window.out_width, &disp_window.out_lines);
	if( disp_window.out_width == 0 || disp_window.out_lines == 0) {
		UPRINTF_ERR(-1);
		return -1;
	}

	in_aspect = (FLOAT)disp_window.in_width / disp_window.in_lines;
	out_aspect = (FLOAT)disp_window.out_width / disp_window.out_lines;

	if(in_aspect != out_aspect) {
		user_utility_disp_size_calc(&disp_window);
	}

	// disp resize limit
	if( disp_window.out_width * 2 < disp_window.in_width ){
		disp_window.in_width = disp_window.out_width * 2;
		disp_window.in_x_pos = (yuv->h_size - disp_window.in_width ) / 2;
	}
	if( disp_window.out_lines * 2 < disp_window.in_lines ){
		disp_window.in_lines = disp_window.out_lines * 2;
		disp_window.in_y_pos = (yuv->v_size - disp_window.in_lines ) / 2;
	}
	FJ_Set_Display_Window( D_FJ_DISP_ID_HDMI, &disp_window );

	Get_Share_System_Config(&syscfg);

	if (syscfg.yc_range == E_YC_RANGE_FULL) {
		clip_cal.y_cal.ygain	= 0x10000;
		clip_cal.y_cal.yofs		= 0;
		clip_cal.y_clip.cph		= 0xFFFF;
		clip_cal.y_clip.cpl		= 0;

		clip_cal.cb_cal.cgain	= 0x8000;
		clip_cal.cb_cal.cofs	= 0;
		clip_cal.cb_clip.cph	= 0xFFFF;
		clip_cal.cb_clip.cpl	= 0;

		clip_cal.cr_cal.cgain	= 0x8000;
		clip_cal.cr_cal.cofs	= 0;
		clip_cal.cr_clip.cph	= 0xFFFF;
		clip_cal.cr_clip.cpl	= 0;
	} else if (syscfg.yc_range == E_YC_RANGE_LIMITED) {
		clip_cal.y_cal.ygain	= 0x1B800;
		clip_cal.y_cal.yofs		= 0x1000;
		clip_cal.y_clip.cph		= 0xEB00;
		clip_cal.y_clip.cpl		= 0x1000;

		clip_cal.cb_cal.cgain	= 0x71E3;
		clip_cal.cb_cal.cofs	= 0x1000;
		clip_cal.cb_clip.cph	= 0xF000;
		clip_cal.cb_clip.cpl	= 0x1000;

		clip_cal.cr_cal.cgain	= 0x71E3;
		clip_cal.cr_cal.cofs	= 0x1000;
		clip_cal.cr_clip.cph	= 0xF000;
		clip_cal.cr_clip.cpl	= 0x1000;
	}

	ercd = Im_DISP_Set_Clip(E_IM_DISP_HDMI, &clip_cal);
	UPRINTF_ERR(ercd);

	if (syscfg.yc_bt == E_YC_BT_601 || syscfg.yc_bt == E_YC_BT_IQ) {
		ercd = Im_DISP_Set_Output_Matrix(E_IM_DISP_HDMI, E_IM_DISP_MATRIX_KIND_Y2R, yr_matrix_to_601);
		UPRINTF_ERR(ercd);
		ercd = Im_DISP_Set_Output_Matrix(E_IM_DISP_HDMI, E_IM_DISP_MATRIX_KIND_R2Y, ry_matrix_to_601);
		UPRINTF_ERR(ercd);
		ercd = BF_Disp_Set_Main_Addr( D_FJ_DISP_ID_HDMI, &addr, fmt, E_BF_DISP_YCC_COLOR_SPACE_601, global_width ); // BF_ should not use here?
		UPRINTF_ERR(ercd);
	} else if (syscfg.yc_bt == E_YC_BT_709) {
		ercd = Im_DISP_Set_Output_Matrix(E_IM_DISP_HDMI, E_IM_DISP_MATRIX_KIND_Y2R, yr_matrix_to_709);
		UPRINTF_ERR(ercd);
		ercd = Im_DISP_Set_Output_Matrix(E_IM_DISP_HDMI, E_IM_DISP_MATRIX_KIND_R2Y, ry_matrix_to_709);
		UPRINTF_ERR(ercd);
		ercd = BF_Disp_Set_Main_Addr( D_FJ_DISP_ID_HDMI, &addr, fmt, E_BF_DISP_YCC_COLOR_SPACE_709, global_width ); // BF_ should not use here?
		UPRINTF_ERR(ercd);
	}

	ercd = FJ_Disp_Video_Enable( D_FJ_DISP_ID_HDMI );
	UPRINTF_ERR(ercd);

	return ercd;
}


INT32 User_Utility_LCD_Yuv( const T_SHARE_LAYOUT* yuv)
{
	FJ_DISP_WINDOW_CONFIG	disp_window;
	FJ_DISP_YCC_ADDR		addr;
	E_BF_DISP_IMAGE_FORMAT	fmt;
	ULONG					global_width;
	FJ_ERR_CODE 			ercd = 0;
	T_SHARE_SYSTEM_CONFIG 	syscfg;
	U_IM_DISP_YR_MATRIX_COEFFICIENT yr_matrix_to_709[D_IM_DISP_MATRIX_SIZE] = {
		{ 0x000064CA00004000 },	// 0x4000,0x0000,0x64CA (1.0,  0.0,      1.5748)
		{ 0x0000E20BF4034000 },	// 0x4000,0xF403,0xE20B (1.0, -0.1873,  -0.4681)
		{ 0x0000000076C24000 }	// 0x4000,0x76C2,0x0000 (1.0,  1.85565,  0.0)
	};
	U_IM_DISP_YR_MATRIX_COEFFICIENT yr_matrix_to_601[D_IM_DISP_MATRIX_SIZE] = {
		{ 0x000059BA00004000 },	// 0x4000,0x0000,0x59BA (1.0,  0.0,     1.402)
		{ 0x0000D24CE9FA4000 },	// 0x4000,0xE9FA,0xD24C (1.0, -0.3441,  -0.7141)
		{ 0x0000000071684000 }	// 0x4000,0x7168,0x0000 (1.0,  1.772,   0.0)
	};
	U_IM_DISP_YR_MATRIX_COEFFICIENT ry_matrix_to_709[D_IM_DISP_MATRIX_SIZE] = {
		{ 0x0000049F2DC60D9B },	// 0xD9B, 0x2DC6,0x49F  (0.2126,  0.7152,   0.0722)
		{ 0x00002000E756F8AA },	// 0xF8AA,0xE756,0x2000 (-0.1146, -0.3854,  0.5)
		{ 0x0000FD12E2EE2000 }	// 0x2000,0xE2EE,0xFD12 (0.5,	  -0.4542,  -0.0458)
	};
	U_IM_DISP_YR_MATRIX_COEFFICIENT ry_matrix_to_601[D_IM_DISP_MATRIX_SIZE] = {
		{ 0x0000074C25911323 },	// 0x1323,0x2591,0x074C (0.299,   0.587,    0.114)
		{ 0x00002000EACDF533 },	// 0xF533,0xEACD,0x2000 (-0.1687, -0.3313,  0.5)
		{ 0x0000FAD1E5342000 }	// 0x2000,0xE534,0xFAD1 (0.5,     -0.4187,  -0.081)
	};
	T_IM_DISP_CLIP_CAL clip_cal;
	UINT32	rotation, disp_h, disp_v;
	FLOAT	in_aspect, out_aspect;

	addr.y_addr   	= yuv->addr_y;
	addr.cbcr_addr	= yuv->addr_c;
	global_width	= yuv->h_byte;

	switch( yuv->format ){
	case E_SHARE_YUV_FORMAT_422_SEMI:
		fmt = E_BF_DISP_IMAGE_FORMAT_422;
		break;
	case E_SHARE_YUV_FORMAT_420_SEMI:
	default:
		fmt = E_BF_DISP_IMAGE_FORMAT_420;
		break;
	}
	
	drv_lcd_get_resolution(&disp_h, &disp_v, &rotation);
	
	disp_window.out_x_pos = 0;
	disp_window.out_y_pos = 0;
	disp_window.out_width = disp_h;
	disp_window.out_lines = disp_v;
	disp_window.in_x_pos = 0;
	disp_window.in_y_pos = 0;
	disp_window.in_width = yuv->h_size;
	disp_window.in_lines = yuv->v_size;

	in_aspect = (FLOAT)disp_window.in_width / disp_window.in_lines;
	out_aspect = (FLOAT)disp_window.out_width / disp_window.out_lines;

	if(in_aspect != out_aspect) {
		user_utility_disp_size_calc(&disp_window);
	}

	// disp resize limit
	if( disp_window.out_width * 2 < disp_window.in_width ){
		disp_window.in_width = disp_window.out_width * 2;
		disp_window.in_x_pos = (yuv->h_size - disp_window.in_width ) / 2;
	}
	if( disp_window.out_lines * 2 < disp_window.in_lines ){
		disp_window.in_lines = disp_window.out_lines * 2;
		disp_window.in_y_pos = (yuv->v_size - disp_window.in_lines ) / 2;
	}
	FJ_Set_Display_Window( FJ_DISP_TYPE_LCD, &disp_window );

	Get_Share_System_Config(&syscfg);

	if (syscfg.yc_range == E_YC_RANGE_FULL) {
		clip_cal.y_cal.ygain	= 0x10000;
		clip_cal.y_cal.yofs		= 0;
		clip_cal.y_clip.cph		= 0xFFFF;
		clip_cal.y_clip.cpl		= 0;

		clip_cal.cb_cal.cgain	= 0x8000;
		clip_cal.cb_cal.cofs	= 0;
		clip_cal.cb_clip.cph	= 0xFFFF;
		clip_cal.cb_clip.cpl	= 0;

		clip_cal.cr_cal.cgain	= 0x8000;
		clip_cal.cr_cal.cofs	= 0;
		clip_cal.cr_clip.cph	= 0xFFFF;
		clip_cal.cr_clip.cpl	= 0;
	} else if (syscfg.yc_range == E_YC_RANGE_LIMITED) {
		clip_cal.y_cal.ygain	= 0x1B800;
		clip_cal.y_cal.yofs		= 0x1000;
		clip_cal.y_clip.cph		= 0xEB00;
		clip_cal.y_clip.cpl		= 0x1000;

		clip_cal.cb_cal.cgain	= 0x71E3;
		clip_cal.cb_cal.cofs	= 0x1000;
		clip_cal.cb_clip.cph	= 0xF000;
		clip_cal.cb_clip.cpl	= 0x1000;

		clip_cal.cr_cal.cgain	= 0x71E3;
		clip_cal.cr_cal.cofs	= 0x1000;
		clip_cal.cr_clip.cph	= 0xF000;
		clip_cal.cr_clip.cpl	= 0x1000;
	}

	ercd = Im_DISP_Set_Clip(E_IM_DISP_LCD_MIPI, &clip_cal);
	UPRINTF_ERR(ercd);

	if (syscfg.yc_bt == E_YC_BT_601 || syscfg.yc_bt == E_YC_BT_IQ) {
		ercd = Im_DISP_Set_Output_Matrix(E_IM_DISP_LCD_MIPI, E_IM_DISP_MATRIX_KIND_Y2R, yr_matrix_to_601);
		UPRINTF_ERR(ercd);
		ercd = Im_DISP_Set_Output_Matrix(E_IM_DISP_LCD_MIPI, E_IM_DISP_MATRIX_KIND_R2Y, ry_matrix_to_601);
		UPRINTF_ERR(ercd);
		ercd = BF_Disp_Set_Main_Addr( D_FJ_DISP_ID_LCD, &addr, fmt, E_BF_DISP_YCC_COLOR_SPACE_601, global_width ); // BF_ should not use here?
		UPRINTF_ERR(ercd);
	} else if (syscfg.yc_bt == E_YC_BT_709) {
		ercd = Im_DISP_Set_Output_Matrix(E_IM_DISP_LCD_MIPI, E_IM_DISP_MATRIX_KIND_Y2R, yr_matrix_to_709);
		UPRINTF_ERR(ercd);
		ercd = Im_DISP_Set_Output_Matrix(E_IM_DISP_LCD_MIPI, E_IM_DISP_MATRIX_KIND_R2Y, ry_matrix_to_709);
		UPRINTF_ERR(ercd);
		ercd = BF_Disp_Set_Main_Addr( D_FJ_DISP_ID_LCD, &addr, fmt, E_BF_DISP_YCC_COLOR_SPACE_709, global_width ); // BF_ should not use here?
		UPRINTF_ERR(ercd);
	}

	ercd = FJ_Disp_Video_Enable( D_FJ_DISP_ID_LCD );
	UPRINTF_ERR(ercd);

	return ercd;
}

VOID User_Utility_disp_switch(E_SHARE_DISP_IF disp)
{
	FJ_ERR_CODE ercd = 0;

	switch ( disp ) {
	case E_SHARE_DISP_IF_LCD:
		if (FJ_Disp_LCD_Get_State() == FJ_DISP_LCD_CONNECT_STATE_OFF) {
			lcd_config();
			ercd = FJ_Disp_LCD_Start_Sync(FJ_DISP_TYPE_LCD);
			UPRINTF_ERR(ercd);
		}
		ercd = FJ_Disp_Video_Disable( FJ_DISP_TYPE_HDMI);
		UPRINTF_ERR(ercd);
		FJ_Set_OSD_Vsync_Callback( FJ_DISP_TYPE_LCD, TRUE, lcd_vsync_callback );
	break;
	case E_SHARE_DISP_IF_HDMI:
		if (FJ_Disp_LCD_Get_State() == FJ_DISP_LCD_CONNECT_STATE_ON) {
			ercd = FJ_Disp_LCD_Stop_Sync(FJ_DISP_TYPE_LCD);
			UPRINTF_ERR(ercd);
			ercd = FJ_Disp_Video_Disable( FJ_DISP_TYPE_LCD );
			UPRINTF_ERR(ercd);
			FJ_Set_OSD_Vsync_Callback( FJ_DISP_TYPE_HDMI, TRUE, hdmi_vsync_callback );
		}
	break;
	default:
	break;
	}
}

INT32 User_Utility_Disp_Enable( E_SHARE_DISP_IF disp )
{
	INT32 ret;

	ret = FJ_Disp_Video_Enable( disp );
	UPRINTF_ERR(ret);

	if( ret != FJ_ERR_OK ) {
		return NG;
	}

	return OK;
}

INT32 User_Utility_Disp_Set_Window( E_SHARE_DISP_IF disp, UINT32 in_width, UINT32 in_lines, FJ_DISP_ROTATE_DEGREE rotate )
{
	INT32 ret;
	FJ_DISP_WINDOW_CONFIG window_config;
	FLOAT in_aspect, out_aspect;

	if( disp == E_SHARE_DISP_IF_LCD ) {
		window_config.in_x_pos	= 0;
		window_config.in_y_pos	= 0;
		window_config.in_width	= in_width;
		window_config.in_lines	= in_lines;
		window_config.out_x_pos	= 0;
		window_config.out_y_pos	= 0;
		window_config.out_width	= D_USER_UTILITY_DISP_LCD_IMG_H;
		window_config.out_lines	= D_USER_UTILITY_DISP_LCD_IMG_V;

		if( window_config.out_width * 2 < window_config.in_width ) {
			window_config.in_width = window_config.out_width * 2;
			window_config.in_x_pos = ( in_lines - window_config.in_width ) / 2;
		}
		if( window_config.out_lines * 2 < window_config.in_lines ) {
			window_config.in_lines = window_config.out_lines * 2;
			window_config.in_y_pos = ( in_lines - window_config.in_lines ) / 2;
		}
	}
	else {
		window_config.in_x_pos	= 0;
		window_config.in_y_pos	= 0;
		window_config.in_width	= in_width;
		window_config.in_lines	= in_lines;
		window_config.out_x_pos	= 0;
		window_config.out_y_pos	= 0;
		hdmi_get_video_id_reso(&window_config.out_width, &window_config.out_lines);

		in_aspect = ( FLOAT )window_config.in_width / window_config.in_lines;
		out_aspect = ( FLOAT )window_config.out_width / window_config.out_lines;

		if( in_aspect != out_aspect ) {
			user_utility_disp_size_calc( &window_config );
		}

		if( window_config.out_width * 2 < window_config.in_width ) {
			window_config.in_width = window_config.out_width * 2;
			window_config.in_x_pos = ( in_width - window_config.in_width ) / 2;
		}
		if( window_config.out_lines * 2 < window_config.in_lines ) {
			window_config.in_lines = window_config.out_lines * 2;
			window_config.in_y_pos = ( in_lines - window_config.in_lines ) / 2;
		}
	}

	ret = FJ_Set_Display_Window( disp, &window_config );
	UPRINTF_ERR(ret);

	if( ret != FJ_ERR_OK ) {
		return NG;
	}

	ret = FJ_Disp_Video_Set_Rotate( disp, rotate );
	UPRINTF_ERR(ret);

	if( ret != FJ_ERR_OK ) {
		return NG;
	}

	return OK;
}

INT32 User_Utility_Disp_Get_Resolution_Ratio( E_SHARE_DISP_IF disp, UINT32 * disp_width, UINT32 * disp_line )
{
	if( ( disp_width == NULL )
			|| ( disp_line == NULL ) ) {
		return NG;
	}

	if( disp == E_SHARE_DISP_IF_LCD ) {
		*disp_width = D_USER_UTILITY_DISP_LCD_IMG_H;
		*disp_line = D_USER_UTILITY_DISP_LCD_IMG_V;
	}
	else {
		hdmi_get_video_id_reso(disp_width, disp_line);
	}

	return OK;
}

INT32 User_Utility_Disp_Render( E_SHARE_DISP_IF disp, FJ_DISP_YCC_ADDR addr, UINT32 global_width )
{
	INT32 ret;

	ret = FJ_Disp_Video_Render( disp, ( FJ_DISP_YCC_ADDR * )&addr, global_width );
	UPRINTF_ERR(ret);

	if( ret != FJ_ERR_OK ) {
		return NG;
	}

	return OK;
}

INT32 User_Utility_Disp_Disable( E_SHARE_DISP_IF disp )
{
	INT32 ret;

	ret = FJ_Disp_Video_Disable( disp );
	UPRINTF_ERR(ret);

	if( ret != FJ_ERR_OK ) {
		return NG;
	}

	return OK;
}

INT32 User_Utility_Disp_OSD_Enable( E_SHARE_DISP_IF disp )
{
	INT32 ret;

	ret = FJ_Disp_OSD_Enable( disp, FJ_DISP_LAYERID_TYPE_OSD0 );
	UPRINTF_ERR(ret);

	if( ret != FJ_ERR_OK ) {
		return NG;
	}

	return OK;
}

INT32 User_Utility_Disp_OSD_Set_Window( E_SHARE_DISP_IF disp )
{
	INT32 ret;
	FJ_DISP_OSD_CONFIG osd_config;

	if( disp == E_SHARE_DISP_IF_LCD ) {
		osd_config.in_osd.Position.StartX	= 0;
		osd_config.in_osd.Position.StartY	= 0;
		osd_config.in_osd.Width				= D_USER_UTILITY_DISP_LCD_IMG_H;
		osd_config.in_osd.Height			= D_USER_UTILITY_DISP_LCD_IMG_V;
		osd_config.in_pitch					= D_USER_UTILITY_DISP_LCD_IMG_H;
		osd_config.in_fmt					= FJ_DISP_OSD_FORMAT_RGBA8888;
		osd_config.out_osd.Position.StartX	= 0;
		osd_config.out_osd.Position.StartY	= 0;
		osd_config.out_osd.Width			= D_USER_UTILITY_DISP_LCD_IMG_H;
		osd_config.out_osd.Height			= D_USER_UTILITY_DISP_LCD_IMG_V;
		osd_config.out_pitch				= D_USER_UTILITY_DISP_LCD_IMG_H;
		osd_config.out_fmt					= FJ_DISP_OSD_FORMAT_RGBA8888;
	}
	else {
		osd_config.in_osd.Position.StartX	= 0;
		osd_config.in_osd.Position.StartY	= 0;
		hdmi_get_video_id_reso(&osd_config.in_osd.Width, &osd_config.in_osd.Height);

		osd_config.in_pitch					= osd_config.in_osd.Width;
		osd_config.in_fmt					= FJ_DISP_OSD_FORMAT_RGBA8888;
		osd_config.out_osd.Position.StartX	= 0;
		osd_config.out_osd.Position.StartY	= 0;
		hdmi_get_video_id_reso(&osd_config.out_osd.Width, &osd_config.out_osd.Height);

		osd_config.out_pitch				= osd_config.out_osd.Width;
		osd_config.out_fmt					= FJ_DISP_OSD_FORMAT_RGBA8888;
	}

	ret = FJ_Disp_OSD_Set_Config( disp, FJ_DISP_LAYERID_TYPE_OSD0, &osd_config );
	UPRINTF_ERR(ret);

	if( ret != FJ_ERR_OK ) {
		return NG;
	}

	return OK;
}

INT32 User_Utility_Disp_OSD_Render( E_SHARE_DISP_IF disp, UINT32 addr )
{
	INT32 ret;

	ret = FJ_Disp_OSD_Render( disp, FJ_DISP_LAYERID_TYPE_OSD0, addr );
	UPRINTF_ERR(ret);

	if( ret != FJ_ERR_OK ) {
		return NG;
	}

	return OK;
}

INT32 User_Utility_Disp_OSD_Disable( E_SHARE_DISP_IF disp )
{
	INT32 ret;

	ret = FJ_Disp_OSD_Disable( disp, FJ_DISP_LAYERID_TYPE_OSD0 );
	UPRINTF_ERR(ret);

	if( ret != FJ_ERR_OK ) {
		return NG;
	}

	return OK;
}

INT32 User_Utility_Disp_OSD_Set_Size( E_SHARE_DISP_IF disp, UINT32 osd_width, UINT32 osd_line )
{

	if( disp == E_SHARE_DISP_IF_LCD ) {
		g_User_Disp_Lcd_Osd_Width = osd_width;
		g_User_Disp_Lcd_Osd_Line = osd_line;
	}
	else {
		g_User_Disp_Hdmi_Osd_Width = osd_width;
		g_User_Disp_Hdmi_Osd_Line = osd_line;
	}

	return OK;
}

INT32 User_Utility_Disp_OSD_Get_Size( E_SHARE_DISP_IF disp, UINT32 *osd_width, UINT32 *osd_line )
{
	
	if( ( osd_width == NULL )
			|| ( osd_line == NULL ) ) {
		return NG;
	}

	if( disp == E_SHARE_DISP_IF_LCD ) {
		*osd_width = g_User_Disp_Lcd_Osd_Width;
		*osd_line = g_User_Disp_Lcd_Osd_Line;
	}
	else {
		*osd_width = g_User_Disp_Hdmi_Osd_Width;
		*osd_line = g_User_Disp_Hdmi_Osd_Line;
	}

	return OK;
}

INT32 User_Utility_Disp_OSD_Set_Config( E_SHARE_DISP_IF disp, UINT32 out_width, UINT32 out_line )
{
	INT32 ret;
	FJ_DISP_OSD_CONFIG osd_config;
	UINT32 disp_width = out_width;
	UINT32 disp_line = out_line;

	if((out_width == 0) || (out_line == 0)){
		User_Utility_Disp_Get_Resolution_Ratio(disp, &disp_width, &disp_line);
	}

	User_Utility_Disp_OSD_Get_Size(disp, &osd_config.in_osd.Width, &osd_config.in_osd.Height);
	osd_config.in_osd.Position.StartX	= 0;
	osd_config.in_osd.Position.StartY	= 0;
	osd_config.in_pitch 				= osd_config.in_osd.Width;
	osd_config.in_fmt					= FJ_DISP_OSD_FORMAT_RGBA8888;
	
	osd_config.out_osd.Width			= disp_width;
	osd_config.out_osd.Height			= disp_line;
	osd_config.out_osd.Position.StartX	= 0;
	osd_config.out_osd.Position.StartY	= 0;
	osd_config.out_pitch				= osd_config.out_osd.Width;
	osd_config.out_fmt					= FJ_DISP_OSD_FORMAT_RGBA8888;

	ret = FJ_Disp_OSD_Set_Config( disp, FJ_DISP_LAYERID_TYPE_OSD0, &osd_config );
	UPRINTF_ERR(ret);

	if( ret != FJ_ERR_OK ) {
		return NG;
	}

	return OK;
}

INT32 User_Utility_Disp_OSD_Clear( E_SHARE_DISP_IF disp, UINT32 osd_addr, SHORT sta_x, SHORT sta_y, UINT32 clr_width, UINT32 clr_line  )
{
	ULONG  mem_addr = osd_addr;
	ULONG  addr;
	UINT32 osd_width, osd_line, i;

	User_Utility_Disp_OSD_Get_Size( disp, &osd_width, &osd_line );

	if((clr_width >= osd_width) && (clr_line >= osd_line)){
		memset( (char *)mem_addr, 0, ( osd_width * osd_line * 4 ) );
	}
	else{
		addr = mem_addr + (sta_x + sta_y * osd_width) * 4;
		for (i = 0; i < clr_line; i++) {
			memset( (char *)addr, 0, ( clr_width * 4 ) );
			addr = addr + osd_width * 4;
		}
	}
	
	FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, osd_addr, osd_width * osd_line * 4 );
	return OK;
}

INT32 User_Utility_Disp_OSD_Print( E_SHARE_DISP_IF disp, UINT32 osd_addr, UINT32 color[FJ_OSD_PRINT_COLOR_INDEX_END], SHORT sta_x, SHORT sta_y, CHAR *string )
{
	INT32  ret = OK;
	ULONG  addr;
	UINT32 osd_width, osd_line;
	UINT32 gr_color[FJ_OSD_PRINT_COLOR_INDEX_END];
	T_POS postion = {0, 0};

	ret = User_Utility_Disp_OSD_Get_Size( disp, &osd_width, &osd_line );
	UPRINTF_ERR( ret );	

	addr = osd_addr + (sta_x + sta_y * osd_width) * 4;
	gr_color[FJ_OSD_PRINT_COLOR_INDEX_BACK] = color[FJ_OSD_PRINT_COLOR_INDEX_BACK];
	gr_color[FJ_OSD_PRINT_COLOR_INDEX_TEXT] = color[FJ_OSD_PRINT_COLOR_INDEX_TEXT];
	BF_Osd_Draw_Comm_Print_String_GR(addr, gr_color, string, osd_width, osd_line, postion);

	FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, osd_addr, osd_width * osd_line * 4 );
	return ret;
}

INT32 User_Utility_HDMI_Debug_Output( VOID )
{
	FJ_ERR_CODE 				ercd = 0;
	FJ_DISP_WINDOW_CONFIG		dwindow;
	FJ_DISP_YCC_ADDR			daddr;
	E_BF_DISP_IMAGE_FORMAT		dformat;
	E_BF_DISP_YCC_COLOR_SPACE	dcolor_space;
	ULONG						dglobal_bytes;
	ULONG						dglobal_lines;
	
	// Force Change Disp Setting
	{
		T_IM_DISP_CTRL_OUTPUT_TRG_LIMIT ctrl_output_trglmt;
		T_IM_DISP_CTRL_OUTPUT			ctrl_output;
		
		ercd = Im_DISP_Stop( E_IM_DISP_SEL_LAYER_ALL, E_IM_DISP_SEL_LAYER_NONE, 0, E_IM_DISP_STOP_TYPE_WITH_WAIT );
		UPRINTF_ERR(ercd);
		
		ercd = Im_DISP_Wait_Stop( E_IM_DISP_SEL_LAYER_ALL, E_IM_DISP_SEL_LAYER_NONE );
		UPRINTF_ERR(ercd);
		
		ercd = Im_DISP_Get_Ctrl_Output( E_IM_DISP_HDMI, &ctrl_output_trglmt, &ctrl_output );
		UPRINTF_ERR(ercd);
		
		ctrl_output_trglmt.domd.bit.MODE	= D_IM_DISP_MODE_GENERAL_10BIT_2;
		ctrl_output.doctl2.bit.CTOH			= D_IM_DISP_CTOH_LEFT;
		
		ercd = Im_DISP_Ctrl_Output( E_IM_DISP_HDMI, &ctrl_output_trglmt, &ctrl_output );
		UPRINTF_ERR(ercd);
		
		ercd = Im_DISP_Start( E_IM_DISP_SEL_LAYER_ALL, E_IM_DISP_SEL_LAYER_NONE, 0 );
		UPRINTF_ERR(ercd);
	}
	// Disp Setting
	dwindow.in_x_pos = 0;
	dwindow.in_y_pos = 0;
	dwindow.in_width = 3840;
	dwindow.in_lines = 2160;
	dwindow.out_x_pos = 0;
	dwindow.out_y_pos = 0;
	dwindow.out_width = dwindow.in_width;
	dwindow.out_lines = dwindow.in_lines;
	FJ_Set_Display_Window( D_FJ_DISP_ID_HDMI, &dwindow );
	
	dformat = E_BF_DISP_IMAGE_FORMAT_GENERAL_10BITx2;
	dcolor_space  = E_BF_DISP_YCC_COLOR_SPACE_601;
	dglobal_bytes = dwindow.in_width * 2; // 16bit
	dglobal_lines = dwindow.in_lines;
	daddr.y_addr    = SDRAM_ADR_CH0_RTOS_IMAGE_WORK;
	daddr.cbcr_addr = SDRAM_ADR_CH0_RTOS_IMAGE_WORK + ( dglobal_bytes * dglobal_lines );
	
	// Get Image from SD, Copy to Disp Address
	{
		UCHAR*	dst_y_buf;
		UCHAR*	dst_c_buf;
		UINT32	dst_y_add = daddr.y_addr;
		UINT32	dst_c_add = daddr.cbcr_addr;
		UINT32	dst_width = dwindow.out_width;
		UINT32	dst_lines = dwindow.out_lines;
		UINT32	dst_global_bytes = dglobal_bytes;
		UINT32	dst_bitwidth = 10; // 12, 14
		
		// file work
		ULONG	fid;
		CHAR	frpath[64] = "I:\\bay_14unpack_3840x2160.RAW";
		CHAR	fwpath[64] = "I:\\bay_14unpack_3840x2160(Transfer).RAW";
		ULONG	fbay_width = 3840;
		ULONG	fbay_lines = 2160;
		ULONG	fbay_global_bytes = fbay_width * 2;
		ULONG	frbay_add = daddr.cbcr_addr + ( dglobal_bytes * dglobal_lines );
		ULONG	fwbay_add = frbay_add + ( fbay_global_bytes * fbay_lines );
		USHORT*	frbay_buf;
		USHORT*	fwbay_buf;
		ULONG	flength;
		ULONG	fret_length;
		// work
		USHORT	pix[16*2];
		USHORT	raw[20*2];
		INT32	pix_num;
		INT32	raw_num;
		INT32	d10_num;
		INT32	pix_cnt;
		INT32	raw_cnt;
		INT32	d10_cnt;
		INT32	x, y;
		
		// Read File
		frbay_buf = (USHORT*)( frbay_add );
		flength = fbay_global_bytes * fbay_lines;
		fid = FJ_Fs_Open( (CHAR*)frpath, FJ_FS_OPEN_RDONLY );
		ercd = FJ_Fs_Read( fid, (UINT32*)frbay_buf, flength, &fret_length);
		UPRINTF_ERR(ercd);
		FJ_Fs_Close( fid );
		
		// for safety
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)frbay_buf, fret_length );
		
#if 0
		// Write Same Data Back for Debug to check frpath = "write_back"
		fwbay_buf = frbay_buf;
		flength = fbay_global_bytes * fbay_lines;
		fid = FJ_Fs_Open( "I:\\write_back.raw", (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
		ercd = FJ_Fs_Write( fid, (UINT32*)fwbay_buf, flength, &fret_length);
		UPRINTF_ERR(ercd);
		FJ_Fs_Close( fid );
#endif

#if 1
		// Write Transfer Area for Debug to check fwpath = data got by receiver
		for( y = 0; y < dst_lines; y ++ ){
			fwbay_buf = (USHORT*)( fwbay_add + (dst_width*2) * y ); // 16bit
			frbay_buf = (USHORT*)( frbay_add + fbay_global_bytes * y );
			for( x = 0; x < dst_width; x ++ ){
				*fwbay_buf = (*frbay_buf) & 0x3FF0; // bit[3:0] = 0;
				fwbay_buf ++;
				frbay_buf ++;
			}
		}
		
		fwbay_buf = (USHORT*)fwbay_add;
		flength = (dst_width*2) * dst_lines; // 16bit
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)fwbay_add, flength );// for safety
		fid = FJ_Fs_Open( fwpath, (FJ_FS_OPEN_WRONLY | FJ_FS_OPEN_CREAT | FJ_FS_OPEN_TRUNC) );
		ercd = FJ_Fs_Write( fid, (UINT32*)fwbay_buf, flength, &fret_length);
		UPRINTF_ERR(ercd);
		FJ_Fs_Close( fid );
#endif

		// Convert 14bit unpack Disp Format
		frbay_buf = (USHORT*)( frbay_add );
		for( y = 0; y < dst_lines; y ++ ){
			
			frbay_buf = (USHORT*)( frbay_add + fbay_global_bytes * y );
			dst_y_buf = (UCHAR*)( dst_y_add + dst_global_bytes * y );
			dst_c_buf = (UCHAR*)( dst_c_add + dst_global_bytes * y );
			
			// 10bit only
			pix_num = 16*2;
			raw_num = 20*2;
			d10_num = 25*2;
			x = 0;
			while( x < dst_width ){
				// Get Pix and Shift and Clip
				for( pix_cnt = 0; pix_cnt < pix_num; pix_cnt ++ ){
					// Pix Get, Shift, Clip
					pix[ pix_cnt ] =  ( frbay_buf[ x + pix_cnt ] >> (14-dst_bitwidth) ) & ( (2 << dst_bitwidth) - 1 );
				}
				
				// Pix -> Raw 10
				pix_cnt = 0;
				for( raw_cnt = 0; raw_cnt < raw_num; raw_cnt ++ ){
					if( (raw_cnt % 5) != 4 ){
						raw[ raw_cnt ] = (pix[ pix_cnt ] >> 2) & 0xFF;
						pix_cnt ++;
					}else{
						raw[ raw_cnt ] = (  ((pix[ pix_cnt-1 ] & 0x3) << 6)
										  | ((pix[ pix_cnt-2 ] & 0x3) << 4)
										  | ((pix[ pix_cnt-3 ] & 0x3) << 2)
										  | ((pix[ pix_cnt-4 ] & 0x3) << 0)
										 ) & 0xFF;
					}
					raw[ raw_cnt ] <<= 2; // [9:2] is valid
				}
				
				// Raw -> Disp10
				// Disp Format 10bit ( 4pix = 5bytes )
				// Current Receiver use only [9:2]
				//          b7        b0
				// byte0 D0[7:0]
				// byte1 D1[5:0] D0[9:8]
				// byte2 D2[3:0] D1[9:6]
				// byte3 D3[1:0] D2[9:4]
				// byte4         D3[9:2]
				raw_cnt = 0;
				for( d10_cnt = 0; d10_cnt < d10_num/2; d10_cnt+=5 ){
					*dst_c_buf++ = (UCHAR)(  (raw[ (raw_cnt+0)*2+0 ] & 0xFF) );
					*dst_c_buf++ = (UCHAR)( ((raw[ (raw_cnt+1)*2+0 ] & 0x3F) << 2) | (raw[ (raw_cnt+0)*2+0 ] >> 8) );
					*dst_c_buf++ = (UCHAR)( ((raw[ (raw_cnt+2)*2+0 ] & 0x0F) << 4) | (raw[ (raw_cnt+1)*2+0 ] >> 6) );
					*dst_c_buf++ = (UCHAR)( ((raw[ (raw_cnt+3)*2+0 ] & 0x03) << 6) | (raw[ (raw_cnt+2)*2+0 ] >> 4) );
					*dst_c_buf++ = (UCHAR)(                                          (raw[ (raw_cnt+3)*2+0 ] >> 2) );
					
					*dst_y_buf++ = (UCHAR)(  (raw[ (raw_cnt+0)*2+1 ] & 0xFF) );
					*dst_y_buf++ = (UCHAR)( ((raw[ (raw_cnt+1)*2+1 ] & 0x3F) << 2) | (raw[ (raw_cnt+0)*2+1 ] >> 8) );
					*dst_y_buf++ = (UCHAR)( ((raw[ (raw_cnt+2)*2+1 ] & 0x0F) << 4) | (raw[ (raw_cnt+1)*2+1 ] >> 6) );
					*dst_y_buf++ = (UCHAR)( ((raw[ (raw_cnt+3)*2+1 ] & 0x03) << 6) | (raw[ (raw_cnt+2)*2+1 ] >> 4) );
					*dst_y_buf++ = (UCHAR)(                                          (raw[ (raw_cnt+3)*2+1 ] >> 2) );
					
					raw_cnt += 4;
				}
				// Update
				x += pix_num;
			}
		}
		
		// for safety
		BF_L1l2cache_Clean_Flush_Addr( dst_y_add, ( dst_global_bytes * dst_lines ) );
		BF_L1l2cache_Clean_Flush_Addr( dst_c_add, ( dst_global_bytes * dst_lines ) );
	}
	
	// Disp Address
	ercd = BF_Disp_Set_Main_Addr( D_FJ_DISP_ID_HDMI, 
								 &daddr, 
								 dformat, 
								 dcolor_space, 
								 dglobal_bytes );
	UPRINTF_ERR(ercd);
	
	ercd = FJ_Disp_Video_Enable( D_FJ_DISP_ID_HDMI );
	UPRINTF_ERR(ercd);
	
	return OK;
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static void lcd_config(void)
{
	FJ_ERR_CODE ercd = 0;
	FJ_DISP_LCD_CONFIG lcd_config = {0};
	// depends on lcd
	lcd_config.lcd_clk						= D_LCD_CONFIG_CLOCK;
	lcd_config.lcd_polarity.clock			= (FJ_DISP_LCD_EDGE_TYPE)D_LCD_CONFIG_POLARITY_CLOCK;
	lcd_config.lcd_polarity.vsync			= (FJ_DISP_LCD_POLARITY_TYPE)D_LCD_CONFIG_POLARITY_VSYNC;
	lcd_config.lcd_polarity.hsync			= (FJ_DISP_LCD_POLARITY_TYPE)D_LCD_CONFIG_POLARITY_HSYNV;
	lcd_config.lcd_polarity.venable			= (FJ_DISP_LCD_POLARITY_TYPE)D_LCD_CONFIG_POLARITY_VENABLE;
	lcd_config.lcd_polarity.henable			= (FJ_DISP_LCD_POLARITY_TYPE)D_LCD_CONFIG_POLARITY_HENABLE;
	lcd_config.lcd_timing.scan_mode			= (FJ_DISP_LCD_SCAN_MODE)D_LCD_CONFIG_SCAN_MODE;
	lcd_config.lcd_timing.vcyc				= D_LCD_CONFIG_VCYC;
	lcd_config.lcd_timing.vcyc_shift		= D_LCD_CONFIG_VCYC_SHIFT;
	lcd_config.lcd_timing.hcyc				= D_LCD_CONFIG_HCYC;
	lcd_config.lcd_timing.vpw				= D_LCD_CONFIG_VPW;
	lcd_config.lcd_timing.hpw				= D_LCD_CONFIG_HPW;
	lcd_config.lcd_timing.vblk				= D_LCD_CONFIG_VBLK;
	lcd_config.lcd_timing.hblk				= D_LCD_CONFIG_HBLK;
	lcd_config.lcd_timing.vsize				= D_LCD_CONFIG_VSIZE;
	lcd_config.lcd_timing.hsize				= D_LCD_CONFIG_HSIZE;

	lcd_config.lcd_format.output_mode		= (FJ_DISP_LCD_OUTPUT_MODE)D_LCD_CONFIG_OUTPUT_MODE;
	lcd_config.lcd_format.output_order[0]	= (FJ_DISP_LCD_OUTPUT_ORDER)D_LCD_CONFIG_OUTPUT_ORDER_1;
	lcd_config.lcd_format.output_order[1]	= (FJ_DISP_LCD_OUTPUT_ORDER)D_LCD_CONFIG_OUTPUT_ORDER_2;
	lcd_config.lcd_format.output_order[2]	= (FJ_DISP_LCD_OUTPUT_ORDER)D_LCD_CONFIG_OUTPUT_ORDER_3;
	lcd_config.lcd_format.output_order[3]	= (FJ_DISP_LCD_OUTPUT_ORDER)D_LCD_CONFIG_OUTPUT_ORDER_4;
	lcd_config.lcd_format.output_order[4]	= (FJ_DISP_LCD_OUTPUT_ORDER)D_LCD_CONFIG_OUTPUT_ORDER_5;
	lcd_config.lcd_format.output_order[5]	= (FJ_DISP_LCD_OUTPUT_ORDER)D_LCD_CONFIG_OUTPUT_ORDER_6;
	lcd_config.lcd_format.pix_fmt			= (FJ_DISP_LCD_PIX_FMT)D_LCD_CONFIG_PIXEL_FORMAT;

	ercd = FJ_Disp_LCD_Config( FJ_DISP_TYPE_LCD, &lcd_config );
	UPRINTF_ERR(ercd);
}

static void user_utility_disp_size_calc(FJ_DISP_WINDOW_CONFIG* disp_win)
{
	static UINT32 in_width = 0, in_lines = 0, out_width = 0, out_lines = 0, find_m = 0, find_n = 0;
	UCHAR m = 0, n = 0;
	UINT32 size, isize, osize, diff_size;
	FLOAT	ratio, ratio1;

	if((in_width == disp_win->in_width) 
		&& (in_lines == disp_win->in_lines)
		&& (out_width == disp_win->out_width) 
		&& (out_lines == disp_win->out_lines))
	{
		disp_win->out_width = in_width * find_m / find_n;
		disp_win->out_lines = in_lines * find_m / find_n;
		disp_win->out_x_pos = ((out_width - disp_win->out_width) >> 1) & 0xFFFFFFFE;
		disp_win->out_y_pos = ((out_lines - disp_win->out_lines) >> 1) & 0xFFFFFFFE;
		return;
	}

	in_width = disp_win->in_width;
	in_lines = disp_win->in_lines;
	out_width = disp_win->out_width; 
	out_lines = disp_win->out_lines;
	
	ratio = (FLOAT)disp_win->out_width / disp_win->in_width;
	ratio1 = (FLOAT)disp_win->out_lines / disp_win->in_lines;
	if(ratio < ratio1)
	{
		isize =disp_win->in_width;
		osize =disp_win->out_width;
	}
	else
	{
		isize = disp_win->in_lines;
		osize = disp_win->out_lines;
	}
		
	diff_size = 0xFFFFFFFF;
	// Find N
	for(n = 31; n > 0; n--)
	{
		if(((disp_win->in_width % n) == 0) && ((disp_win->in_lines % n) == 0))
		{
			// Find M
			for(m = 31; m > 0; m--)
			{
				size = isize * m / n;
				if(size <= osize)
				{
					if(size == osize)
					{
						find_m = m;
						find_n = n;
						
						diff_size = 0;
						break;
					}

					if(diff_size > (osize - size))
					{
						find_m = m;
						find_n = n;
						
						diff_size = osize - size;
						break;
					}
				}
			}
			
			if(diff_size == 0)
			{
				break;
			}
		}
	}
		
	if(diff_size != 0xFFFFFFFF)
	{
	
	disp_win->out_width = disp_win->in_width * find_m / find_n;
	
	disp_win->out_lines = disp_win->in_lines * find_m / find_n;
		disp_win->out_x_pos = ((out_width - disp_win->out_width) >> 1) & 0xFFFFFFFE;
		disp_win->out_y_pos = ((out_lines - disp_win->out_lines) >> 1) & 0xFFFFFFFE;
	}

	UPRINTF("\n hdmi_size_calc: [%d/%d] = %d x %d ", find_m, find_n , disp_win->out_width, disp_win->out_lines);

}

static void hdmi_vsync_callback( void )
{
	T_SHARE_SYSTEM_PARAM param;
	ULONG addr[3];

	FJ_GetMainDispBuffAddr( E_OSDLayer_HDMI_MAIN, addr );
	Get_Share_System_Param(&param);
	if(User_VIew_Get_Stat() == E_USER_STATE_IDLE && User_YUVin_VIew_Get_Stat() == E_USER_STATE_IDLE) {
		param.lock_ipu_address_y = D_ADDR_UNLOCK;
		param.lock_ipu_address_c = D_ADDR_UNLOCK;
	} else {
		param.lock_ipu_address_y = addr[0];
		param.lock_ipu_address_c = addr[1];
	}
	Set_Share_System_Param(&param);
	//MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_DISP, FJ_MBALOG_FUNC_ID_VSYNC_INT, FJ_MBALOG_TYPE_POINT, "HDMI Get %08lx", addr[0] ));
}

static void lcd_vsync_callback( void )
{
	T_SHARE_SYSTEM_PARAM param;
	ULONG addr[3];

	FJ_GetMainDispBuffAddr( E_OSDLayer_LCD_MAIN, addr );
	Get_Share_System_Param(&param);
	if(User_VIew_Get_Stat() == E_USER_STATE_IDLE && User_YUVin_VIew_Get_Stat() == E_USER_STATE_IDLE) {
		param.lock_ipu_address_y = D_ADDR_UNLOCK;
		param.lock_ipu_address_c = D_ADDR_UNLOCK;
	} else {
		param.lock_ipu_address_y = addr[0];
		param.lock_ipu_address_c = addr[1];
	}
	Set_Share_System_Param(&param);
	MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_USER_DISP, FJ_MBALOG_FUNC_ID_VSYNC_INT, FJ_MBALOG_TYPE_POINT, "LCD Get %08lx", addr[0] ));
}
