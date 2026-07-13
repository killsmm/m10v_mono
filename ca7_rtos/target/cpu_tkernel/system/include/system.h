/**
 * @file		system.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "share.h"
#include "share_data.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_SYSTEM_EVENT_NONE = 0,
	E_SYSTEM_EVENT_ERR,
	E_SYSTEM_EVENT_MODE_VIEW,
	E_SYSTEM_EVENT_MODE_YUVIN_VIEW,
	E_SYSTEM_EVENT_STOP_MODE,
	E_SYSTEM_EVENT_STOP_YUVIN_MODE,
	E_SYSTEM_EVENT_MODE_END,
	E_SYSTEM_EVENT_AE_END,
	E_SYSTEM_EVENT_AWB_END,
	E_SYSTEM_EVENT_AF_UPDATE,
	
	E_SYSTEM_EVENT_DMIPI0_FS,
	E_SYSTEM_EVENT_DMIPI0_FE,
	E_SYSTEM_EVENT_DMIPI1_FS,
	E_SYSTEM_EVENT_DMIPI1_FE,
	E_SYSTEM_EVENT_DMIPI2_FS,
	E_SYSTEM_EVENT_DMIPI2_FE,
	E_SYSTEM_EVENT_DMIPI3_FS,
	E_SYSTEM_EVENT_DMIPI3_FE,
	
	E_SYSTEM_EVENT_SENCORE_VD00,
	E_SYSTEM_EVENT_SENCORE_VD10,
	E_SYSTEM_EVENT_SENCORE_VD20,
	E_SYSTEM_EVENT_SENCORE_VD30,
	
	E_SYSTEM_EVENT_VD,
	E_SYSTEM_EVENT_LDIV_PWCH,
	E_SYSTEM_EVENT_LDIV_PWCH_DUMMY,
	E_SYSTEM_EVENT_PWCH,
	E_SYSTEM_EVENT_SRO_PRCH,
	E_SYSTEM_EVENT_R2YLINE,
	E_SYSTEM_EVENT_R2YLINE1,
	E_SYSTEM_EVENT_R2YCNR,
	E_SYSTEM_EVENT_AEAWB_PWCH,
	E_SYSTEM_EVENT_PRO_HIST,
	E_SYSTEM_EVENT_AF,
	E_SYSTEM_EVENT_LTMCNR,
	E_SYSTEM_EVENT_CNR_OFL,
	
	E_SYSTEM_EVENT_PIPE_YUV,
	E_SYSTEM_EVENT_VHDR_YUV,
	E_SYSTEM_EVENT_SGDE_YUV,
	E_SYSTEM_EVENT_QVIEW_YUV,
	E_SYSTEM_EVENT_STITCH_YUV,
	E_SYSTEM_EVENT_SHDR_YUV,
	E_SYSTEM_EVENT_VIS,
	E_SYSTEM_EVENT_EIS,

	E_SYSTEM_EVENT_PIPE_DOL_YUV,
	E_SYSTEM_EVENT_DOL_COMBINE,
	E_SYSTEM_EVENT_DOL_COMBINE_RBK,
	E_SYSTEM_EVENT_DOL_COMBINE_MAP,
	E_SYSTEM_EVENT_DEBUG_DOL_COPY,
	E_SYSTEM_EVENT_DEBUG_DOL_COPY2,

	E_SYSTEM_EVENT_LTM_MAP,
	E_SYSTEM_EVENT_LTM_RGB,
	E_SYSTEM_EVENT_LTM,
	
	E_SYSTEM_EVENT_IQ_DBG2_SAVE_REG,
	
	E_SYSTEM_EVENT_CAP_ENC_REL,
	E_SYSTEM_EVENT_CAP_MEDIA_REL,
	E_SYSTEM_EVENT_CAP_PIPE_DUMMY,
	
	E_SYSTEM_EVENT_VIEW_RECONFIG,

	E_SYSTEM_EVENT_CAP_WITHOUT_RESET,
	
	E_SYSTEM_EVENT_MAX,
} E_SYSTEM_EVENT;

typedef enum {
	D_SENSOR_VIEW_MODE	= 1,
	D_YUV_VIEW_MODE 	= 2,
} E_SYSTEM_VIEW_MODE;

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

void Snd_Event_to_System( E_SYSTEM_EVENT event, ULLONG frame_no );
void Wait_Queue_is_Empty_in_System( UINT32 wait_ms_pre, UINT32 timeout_ms );
void Wait_Specified_Event( E_SYSTEM_EVENT wait_event );
void System_Estimate_AE_ROI( ULLONG frame_no, const T_SHARE_AE_ROI_IN* in, T_SHARE_AE_ROI_OUT* out );
E_SYSTEM_VIEW_MODE System_get_view_mode(void);

#ifdef __cplusplus
}
#endif

#endif	// _SYSTEM_H_

