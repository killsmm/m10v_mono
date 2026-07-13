/**
 * @file		still_basic.h
 * @brief		Basic API for Still
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _STILL_BASIC_H_
#define _STILL_BASIC_H_

#include "ddim_typedef.h"

#include "fj_std.h"
#include "fj_still.h"
#include "fj_encoder.h"

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
extern FJ_ERR_CODE fj_set_sg( UCHAR sen_id, FJ_SG_CTRL* sg_ctrl );
extern FJ_ERR_CODE fj_sg_start( UCHAR sen_id );
extern FJ_ERR_CODE fj_sg_stop( UCHAR sen_id, UCHAR stop_mode );
extern FJ_ERR_CODE fj_sen_control_sensorclk( UCHAR on_off );
extern FJ_ERR_CODE fj_getlinerhistogram( UCHAR sen_id, USHORT* pHistogramBuff );
extern FJ_ERR_CODE fj_get_capture_stitch_status( FJ_STILL_STITCH_TYPE * stitch_status ) ;
extern FJ_ERR_CODE fj_getdemosaicflipmirrorenabled( UCHAR demosaic_id, BOOL* flip_enabled, BOOL* mirror_enabled );

extern FJ_ERR_CODE BF_Still_Basic_Unlock_JpegBuffer( ULONG frame_no, ULONG bunk_num );
// delete\basefw\fj\imgproc\still
extern VOID BF_Still_PRO_Ctrl_Sg_Set( const UCHAR sen_id, const FJ_SG_CTRL* const sg_ctrl );
extern VOID BF_Still_PRO_Ctrl_SG_Start( const UCHAR sen_id );
extern VOID BF_Still_PRO_Ctrl_SG_Stop( const UCHAR sen_id, const UCHAR stop_mode );


#endif	// _STILL_UTILITY_H_

