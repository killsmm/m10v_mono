/**
 * @file		ip_mfnr.h
 * @brief		Header file of wrapper layer (MFNR Library)
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#ifndef _IP_MFNR_H
#define	_IP_MFNR_H

#include "fj_still.h"
#include "im_shdr.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup ip_mfnr_overview
@{*/

/*@} --- end of ip_mfnr_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup ip_mfnr_sequence
@{*/

/*@} --- end of ip_mfnr_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup ip_mfnr_definition
@{*/
#define D_IP_MFNR_OK					(0)			/**< Success.					*/
#define D_IP_MFNR_NG					(-1)		/**< Error. 					*/

#define D_IP_MFNR_MAX_COUNT				(6)			/**< Max number of input image.	*/

#define D_IP_MFNR_MARGIN				(64)		/**< Margin (32:16*2 (or 64:32*2) )	*/
//#define D_IP_MFNR_MARGIN				(32)		/**< Margin (32:16*2 (or 64:32*2) )	*/

// GMD mode.
typedef enum {
	E_IP_MFNR_GMD_MODE_FPT	= 0,					/**< FPT.				*/
	E_IP_MFNR_GMD_MODE_PRE_HRE,						/**< PRE+HRE.			*/
	E_IP_MFNR_GMD_MODE_GYRO,						/**< Gyro.				*/
} E_IP_MFNR_GMD_MODE;

// filter margin.
typedef enum {
	E_IP_MFNR_FIL_MARGIN_16 = 0,					/**< 16 pixel.			*/
	E_IP_MFNR_FIL_MARGIN_32,						/**< 32 pixel.			*/
} E_IP_MFNR_FIL_MARGIN;

///////////////////////////////////////////////////////////////////////////////
// structure
///////////////////////////////////////////////////////////////////////////////
/** MFNR start parameter. */
typedef struct {
	E_IP_MFNR_FIL_MARGIN		filMargin;				/**< [In]Filter margin(16/32 pixels).		*/
	FJ_STILL_IP_IMAGE			imgSrc_1st[ 3 ];		/**< [In]Source image 1st.<br>
															- Image address is 8byte align.
															<ul>
																<li>[0]:Full image      (YUV422 8bit, expansion margin from SGDE).
																<li>[1]:Reduction image (YUV422 8bit, expansion margin from SGDE).
																<li>[2]:1/4 image       (YUV422 8bit, expansion margin from SGDE).
															</ul>
														*/
	FJ_STILL_IP_IMAGE			imgSrc_2nd[ 2 ];		/**< [In]Source image 2nd.<br>
															- Image address is 8byte align.
															<ul>
																<li>[0]:Full image      (YUV422 8bit, expansion margin from SGDE).
																<li>[1]:Reduction image (YUV422 8bit, expansion margin from SGDE).
															</ul>
														*/
	FJ_STILL_IP_IMAGE			imgMe;					/**< [Out]Vector data.<br>
																- Image address is 8byte align.
														*/
	FJ_STILL_IP_IMAGE			imgDst;					/**< [Out]Destination image.<br>
															- Full image(YUV422 8bit). Image address is 8byte align.
														*/
} IP_MFNR_START_PRM;

/** IP_MFNR_CROP_XY_POS. */
typedef struct {
	FLOAT						pos_x;
	FLOAT						pos_y;
} IP_MFNR_CROP_XY_POS;

/** IP_MFNR_CROP_IMG_SIZE. */
typedef struct {
	USHORT						width;
	USHORT						lines;
} IP_MFNR_CROP_IMG_SIZE;

/** IP_MFNR_CROP_CORNER_POS. */
typedef struct {
	IP_MFNR_CROP_XY_POS			left_top;
	IP_MFNR_CROP_XY_POS			right_top;
	IP_MFNR_CROP_XY_POS			left_bottom;
	IP_MFNR_CROP_XY_POS			right_bottom;
} IP_MFNR_CROP_CORNER_POS;
/*@} --- end of ip_mfnr_definition */

///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

/** @weakgroup ip_mfnr_api
@{*/

/**
Open MFNR.
@return		If the function succeeds, the return value is D_IP_MFNR_OK.
*/
extern INT32 IP_MFNR_Open( VOID );

/**
Initialize MFNR.
@param[in]	maxCount	: Max number of input images (2~6).
@return		If the function succeeds, the return value is D_IP_MFNR_OK.
*/
extern INT32 IP_MFNR_Init( const UCHAR maxCount );

/**
Start MFNR.
@param[in]	startPrm	: MFNR start parameter.
@param[in]	heapAddr	: MFNR Heap address.
@param[in]	sgdeHeap	: SGDE heap.
@param[in]	count		: Processing count (1~maxCount-1).
@return		If the function succeeds, the return value is D_IP_MFNR_OK.
*/
extern INT32 IP_MFNR_Start( IP_MFNR_START_PRM* startPrm, const ULONG heapAddr, FJ_STILL_IP_IMAGE* sgdeHeap, FJ_STILL_IP_IMAGE* shdrOut, const UCHAR count );

/**
Close MFNR.
@return		If the function succeeds, the return value is D_IP_MFNR_OK.
*/
extern INT32 IP_MFNR_Close( VOID );

/**
Get MFNR version.
@param[out]	str	: version.
*/
extern VOID IP_MFNR_Get_Version( CHAR* str );

/**
Get MFNR input previous frame information.
@param[out]	prev_info		: previous image infomation.
@param[out]	prev_info_red	: previous image infomation (reduction).
*/
extern VOID IP_MFNR_Get_Input_Prev_Frame_Info( FJ_STILL_IP_IMAGE* prev_info, FJ_STILL_IP_IMAGE* prev_info_red );

/**
Get MFNR count.
@return		Processing count. (1~maxCount-1).
*/
extern INT32 IP_MFNR_Get_Count();

#if defined(CO_IQBIN_GENERAL)	// --- REMOVE_IQBIN_GENERAL BEGIN ---
/**
Get MFNR register value table pointer.
@param[out]	shdr_ctrl	 : Register value table pointer for IQ parameter set.
@param[out]	shdr_pre	 : Register value table pointer for IQ parameter set.
@param[out]	shdr_fmd	 : Register value table pointer for IQ parameter set.
@param[out]	shdr_spnr	 : Register value table pointer for IQ parameter set.
@param[out]	shdr_msk	 : Register value table pointer for IQ parameter set.
@param[out]	shdr_pmsk	 : Register value table pointer for IQ parameter set.
@param[out]	shdr_in		 : Register value table pointer for IQ parameter set.
*/
extern VOID IP_MFNR_Get_RegTbl( T_IM_SHDR_CTRL** shdr_ctrl, T_IM_SHDR_PRE** shdr_pre, T_IM_SHDR_FMD** shdr_fmd, T_IM_SHDR_SPNR** shdr_spnr, T_IM_SHDR_MSK** shdr_msk, T_IM_SHDR_PMSK** shdr_pmsk, UINT32** shdr_in );
#endif // CO_IQBIN_xxxx  --- REMOVE_IQBIN_GENERAL END ---

/** Calculation corner_pos from image rectangle. */
extern VOID IP_MFNR_Conv_Rect2CornerPos( IP_MFNR_CROP_CORNER_POS* const corner_pos, const IP_MFNR_CROP_IMG_SIZE* const rect );

/** Calculation cropping corner_pos from base_corner_pos and target xform.	*/
extern VOID IP_MFNR_Calc_CropCornerPos( IP_MFNR_CROP_CORNER_POS* const crop_corner_pos, const IP_MFNR_CROP_CORNER_POS* const base_corner_pos, const FLOAT target_rotate_xform[6], const IP_MFNR_CROP_IMG_SIZE* const target_rect );

/** Calculation xform for cropping from crop_corner_pos.	*/
extern VOID IP_MFNR_Calc_AspectCropPos( FLOAT crop_xform[6], const IP_MFNR_CROP_IMG_SIZE* const dst_rect, const IP_MFNR_CROP_CORNER_POS* const crop_corner_pos );

/*@} --- end of ip_mfnr_api */

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// _IP_MFNR_H
