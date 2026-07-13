/**
 * @file		detect_task.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _DETECT_TASK_H_
#define _DETECT_TASK_H_

#include "share.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define FLG_DETECT_HDMI_SETUP		0x00000001
#define FLG_DETECT_HDMI_UNSET		0x00000002
#define FLG_DETECT_SD1_MOUNT		0x00000004
#define FLG_DETECT_SD1_UNMOUNT		0x00000008
#define FLG_DETECT_WAITPTN			( FLG_DETECT_HDMI_SETUP	|\
									  FLG_DETECT_HDMI_UNSET	|\
									  FLG_DETECT_SD1_MOUNT	|\
									  FLG_DETECT_SD1_UNMOUNT	)

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

VOID hdmi_get_video_id_reso(UINT32 *width, UINT32 *height);
FLOAT hdmi_get_video_id_fps( VOID );
INT32 Linux_Get_SD1( VOID );
INT32 Linux_Rel_SD1( VOID );
VOID Hdmi_ForceChangeVid( UINT32 vid);

#ifdef __cplusplus
}
#endif

#endif	// _DETECT_TASK_H_

