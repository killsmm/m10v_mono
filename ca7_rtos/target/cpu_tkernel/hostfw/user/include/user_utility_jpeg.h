/**
 * @file		user_utility_jpeg.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _USER_UTILITY_JPEG_H_
#define _USER_UTILITY_JPEG_H_

#ifdef CO_DCF_EXIF_ON
#include "dcf_if.h"
#endif

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
typedef struct {
	E_SHARE_YUV_FORMAT	format;	
	USHORT	width;
	USHORT	lines;
	ULONG	g_y_width;
	ULONG	g_c_width;
	ULONG	addr_y;
	ULONG	addr_c;
	ULONG	addr_code;
	ULLONG	limit_size;
	UCHAR	quality;
} T_JPEG_ENC;

#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
typedef struct {
	struct {
		ULONG  addr;
		ULONG  size;
		USHORT width;
		USHORT lines;
	} mpf_enc[2];
} T_JPEG_MPF_DATA;
#endif

typedef struct {
	ULONG					thumb_addr;
	ULONG					thumb_size;
	ULONG					main_addr;
	ULONG					main_size;
	ULONG					main_width;
	ULONG					main_lines;
#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
	T_JPEG_MPF_DATA			mpf;
#endif
	T_BF_DCF_IF_EXIF_INFO*	exif;
} T_JPEG_SAVE_FILE2;


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

INT32 User_Utility_JPEG_Enc( ULLONG frame_no, const T_JPEG_ENC *in, ULONG thumb_addr, ULONG thumb_size );
INT32 User_Utility_JPEG_Enc_Thumb( ULLONG frame_no, const T_SHARE_YUV* yuv, E_SHARE_YUV_OUT index, ULONG addr_code, ULONG limit_size, ULONG thumb_yuv_addr, UCHAR quality );
#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
INT32 User_Utility_JPEG_Enc_MPF( ULLONG frame_no, const T_SHARE_YUV* yuv, E_SHARE_YUV_OUT index, ULONG addr_code, ULONG limit_size, UCHAR quality );
VOID User_Utility_JPEG_Set_MPF( const T_JPEG_MPF_DATA* mpf_data );
#endif
INT32 User_Utility_JPEG_Get_Result( ULONG* addr, ULONG* size );
INT32 User_Utility_JPEG_ENC( FW_USHORT width, FW_USHORT lines, const ULLONG frame_no );
#ifdef CO_DCF_EXIF_ON
INT32 User_Utility_JPEG_Save_File( ULONG addr, ULONG size, const T_BF_DCF_IF_EXIF_INFO* exif );
INT32 User_Utility_JPEG_Get_Exif( ULLONG frame_no, T_JPEG_SAVE_FILE2* save );
INT32 User_Utility_JPEG_Save_File2( const T_JPEG_SAVE_FILE2* save );
BOOL User_Utility_ExifUpdateCB( VOID );
INT32 User_Utility_CreateMakerNoteCB( CHAR * byBuffer, INT32 Size, INT32 Offset, INT32 * ParamArray, INT32 ParamNum );
#endif
void User_Utility_IPCU_JPEG_Enable(BOOL en);
BOOL User_Utility_is_IPCU_JPEG_Enable(void);
void User_Utility_Send_Jpeg_To_Linux(ULONG addr, ULONG size, T_BF_DCF_IF_EXIF_INFO *exif, FW_USHORT width, FW_USHORT lines, const ULLONG frame_no);



#ifdef __cplusplus
}
#endif

#endif	// _USER_UTILITY_JPEG_H_

