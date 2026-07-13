/**
 * @file		still_proc_subr.h
 * @brief		capture image create procedure
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _STILL_PROC_SUBR_H_
#define _STILL_PROC_SUBR_H_

// FJ
#include "fj_still.h"

// Still
#include "still_img_info.h"
#include "still_iip.h"

// DDIM
#include "ddim_typedef.h"


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
// Movie still capture start
typedef struct {
	USHORT cap_count;
	T_IMAGE_ADDR_YCC rec_hd;
	T_IMAGE_ADDR_YCC rec_vga;
	T_IMAGE_ADDR_YCC rec_thumb;
} T_BF_STILL_PROC_SUBR_MOVIE_CAPTURE;
// Movie still capture end

typedef struct {
	T_BF_STILL_IMG			dst;
	T_BF_STILL_IMG_WINDOW_F	crop_rect;
	T_BF_STILL_IMG_WINDOW_L	dst_pos;
	UCHAR					dst_mif;
} T_BF_STILL_IIP_IMG_FILL;


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
#ifdef __cplusplus
extern "C" {
#endif


extern VOID BF_Still_Proc_Subr_Convert_ImgInfo( const FJ_IMG_INFO* const org_img, T_BF_STILL_IMG* conv_img );
extern FJ_ERR_CODE BF_Still_Proc_Execute_AfnConvertImage( const T_BF_STILL_IIP_IMG_CONVERT* const img_info, const UCHAR fill_en, const INT32 timeout );

extern FJ_ERR_CODE BF_Still_Proc_Execute_AfnConvertImage_2_Open_SetData( const T_BF_STILL_IIP_IMG_CONVERT* const img_info, const UCHAR fill_en, const INT32 timeout );
extern FJ_ERR_CODE BF_Still_Proc_Execute_AfnConvertImage_2_Execute( const T_BF_STILL_IIP_IMG_CONVERT* const img_info );
extern FJ_ERR_CODE BF_Still_Proc_Execute_AfnConvertImage_2_Close( VOID );


#ifdef __cplusplus
}
#endif

#endif	// _STILL_PROC_SUBR_H_

