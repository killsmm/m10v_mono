/**
 * @file		playback2.h
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _PLAYBACK2_H_
#define _PLAYBACK2_H_

#include "ddim_typedef.h"
#include "fj_std.h"
#include "fj_playback.h"
#include "play_macro_wrap.h"


/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function                                                             */
/*----------------------------------------------------------------------*/
//////////////////////////////////
// Playback API
//////////////////////////////////
extern FJ_ERR_CODE fj_stillplayback_getmeminfo( T_FJ_PLAYBACK_MEMINFO* mem_info );
extern FJ_ERR_CODE fj_stillplayback_imagedecode( T_FJ_PLAYBACK_DECODE* dec_info, T_FJ_PLAYBACK_YUV_INFO* yuv_info );
extern FJ_ERR_CODE fj_stillplayback_imageconvert( T_FJ_PLAYBACK_CONVERT* conv_info );

#endif // _PLAYBACK_H_
