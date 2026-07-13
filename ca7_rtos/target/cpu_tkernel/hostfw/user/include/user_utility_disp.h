/**
 * @file		user_utility_disp.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _USER_UTILITY_DISP_H_
#define _USER_UTILITY_DISP_H_

#include "share_data.h"
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


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

INT32 User_Utility_HDMI_Yuv( const T_SHARE_LAYOUT* yuv );
INT32 User_Utility_LCD_Yuv( const T_SHARE_LAYOUT* yuv );
VOID  User_Utility_disp_switch(E_SHARE_DISP_IF disp);
INT32 User_Utility_Disp_Enable( E_SHARE_DISP_IF disp );
INT32 User_Utility_Disp_Set_Window( E_SHARE_DISP_IF disp, UINT32 in_width, UINT32 in_lines, FJ_DISP_ROTATE_DEGREE rotate );
INT32 User_Utility_Disp_Get_Resolution_Ratio( E_SHARE_DISP_IF disp, UINT32 * disp_width, UINT32 * disp_line );
INT32 User_Utility_Disp_Render( E_SHARE_DISP_IF disp, FJ_DISP_YCC_ADDR addr, UINT32 global_width );
INT32 User_Utility_Disp_Disable( E_SHARE_DISP_IF disp );
INT32 User_Utility_Disp_OSD_Enable( E_SHARE_DISP_IF disp );
INT32 User_Utility_Disp_OSD_Set_Window( E_SHARE_DISP_IF disp );
INT32 User_Utility_Disp_OSD_Render( E_SHARE_DISP_IF disp, UINT32 addr );
INT32 User_Utility_Disp_OSD_Disable( E_SHARE_DISP_IF disp );
INT32 User_Utility_Disp_OSD_Set_Size( E_SHARE_DISP_IF disp, UINT32 osd_width, UINT32 osd_line );
INT32 User_Utility_Disp_OSD_Get_Size( E_SHARE_DISP_IF disp, UINT32 *osd_width, UINT32 *osd_line );
INT32 User_Utility_Disp_OSD_Set_Config( E_SHARE_DISP_IF disp, UINT32 out_width, UINT32 out_line );
INT32 User_Utility_Disp_OSD_Clear( E_SHARE_DISP_IF disp, UINT32 osd_addr, SHORT sta_x, SHORT sta_y, UINT32 clr_width, UINT32 clr_line  );
INT32 User_Utility_Disp_OSD_Print( E_SHARE_DISP_IF disp, UINT32 osd_addr, UINT32 color[FJ_OSD_PRINT_COLOR_INDEX_END], SHORT sta_x, SHORT sta_y, CHAR *string );

INT32 User_Utility_HDMI_Debug_Output( VOID );

#ifdef __cplusplus
}
#endif

#endif	// _USER_UTILITY_DISP_H_

