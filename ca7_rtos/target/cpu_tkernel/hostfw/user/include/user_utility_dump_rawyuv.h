/**
 * @file		user_utility_dump_rawyuv.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _USER_UTILITY_DUMP_RAWYUV_H_
#define _USER_UTILITY_DUMP_RAWYUV_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_DUMP_STORAGE = 0,
	E_DUMP_IPCU
} T_DUMP_INTERFACE;
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

void User_Utility_SaveFile_RAW_Divided( void );
void User_Utility_SaveFile_DOL_YUV( void );
void User_Utility_Dump_RAW_One_Sensor( const ULLONG frame_no, T_DUMP_INTERFACE interface );
void User_Utility_Dump_YUV( const E_SHARE_YUV_OUT yuv_out, const ULLONG frame_no, T_DUMP_INTERFACE interface );
void User_Utility_Dump_RAW_YUV(const ULONG aval_mem, const ULLONG frame_no, T_DUMP_INTERFACE interface_yuv, T_DUMP_INTERFACE interface_raw );

void User_Utility_IPCU_RAW_Enable(BOOL en);
BOOL User_Utility_is_IPCU_RAW_Enable(void);
void User_Utility_IPCU_YUV_Enable(BOOL en);
BOOL User_Utility_is_IPCU_YUV_Enable(void);

VOID User_Utility_Dump_Raw_Printf(UINT32 image_addr, USHORT image_width, USHORT pos_x, USHORT pos_y, ULONG img_size_byte, char *fmt, ... );
VOID User_Utility_Dump_YUV_Printf( const T_SHARE_YUV* yuv, E_SHARE_YUV_OUT index, USHORT pos_x, USHORT pos_y, char *fmt, ... );
VOID User_Utility_Dump_YUV_SetFontSize( UCHAR font_size );
#ifdef __cplusplus
}
#endif

#endif	// _USER_UTILITY_DUMP_RAWYUV_H_

