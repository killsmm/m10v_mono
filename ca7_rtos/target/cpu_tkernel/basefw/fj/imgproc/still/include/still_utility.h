/**
 * @file		still_utility.h
 * @brief		Utility API for Still
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _STILL_UTILITY_H_
#define _STILL_UTILITY_H_

#include "ddim_typedef.h"
#include "fj_std.h"
#include "fj_still.h"
#include "still_img_info.h"
#include "im_jpeg.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// --- REMOVE_AHA BEGIN ---
//#define D_STILL_UTILITY_GYRO_DATA_AHA_CNT		(66)
#define D_STILL_UTILITY_GYRO_DATA_AHA_CNT		(336)	// Gyro:8KHz_imgpath:24fps.
// --- REMOVE_AHA END ---

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// --- REMOVE_AHA BEGIN ---
typedef struct {
	INT32			cnt;
	FJ_GYRO_DATA	data[ D_STILL_UTILITY_GYRO_DATA_AHA_CNT ];
} BF_GYRO_DATA_AHA;
// --- REMOVE_AHA END ---

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
extern FJ_ERR_CODE fj_imageconvert( FJ_IMG_CONV_CTRL* conv_info );
extern FJ_ERR_CODE fj_imagecrop( FJ_IMG_CROP_CTRL* crop_info );
extern FJ_ERR_CODE fj_imagejpegencode( FJ_IMG_JPEG_CTRL* enc_info );
extern FJ_ERR_CODE fj_configgyro( FJ_GYRO_CONFIG* gyro_config );
extern FJ_ERR_CODE fj_addgyrodata( ULLONG frame_no, FJ_GYRO_DATA* gyro_data );

extern VOID BF_Still_Get_Gyro_Config( FJ_GYRO_CONFIG* const gyro_config );
extern ULONG BF_Still_Encode_Jpeg_Debug( FJ_IMG_INFO* jpeg_info, ULONG dst_addr );
extern ULONG BF_Still_Encode_Jpeg( UCHAR still_encode_id, FJ_IMG_INFO* jpeg_info, ULONG dst_addr, INT32 wait_time );
extern SHORT BF_Still_Utility_Get_Jpeg_Encode_Quality( const USHORT cr, const UCHAR thumb_flg );


extern FJ_ERR_CODE BF_Still_Image_Convert_2_Init( FJ_IMG_CONV_CTRL* conv_info, const T_IMAGE_SIZE* src_correct_aspect_ratio );
extern FJ_ERR_CODE BF_Still_Image_Convert_2_Execute( FJ_IMG_CONV_CTRL* conv_info );
extern FJ_ERR_CODE BF_Still_Image_Convert_2_Close( VOID );
extern FJ_ERR_CODE BF_Still_FJ_ImageConvert( FJ_IMG_CONV_CTRL* conv_info, const T_IMAGE_SIZE* src_correct_aspect_ratio );


#endif	// _STILL_UTILITY_H_

