/**
 * @file		share.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#ifndef _SHARE_H_
#define _SHARE_H_

#include "ddim_typedef.h"
#include "fj_debug.h"
#include "fj_debug_mba.h"
#include "fj_host_debug.h"
#include "mba_if.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define OK		(0)
#define NG		(-1)
#define ON		(1)
#define OFF		(0)

#define D_FJ_DISP_ID_LCD	(0)
#define D_FJ_DISP_ID_HDMI	(1)

// Flag Pattern for FID_HOST_VD
#define FLG_HOST_VD_TRIG		0x00000001
// Flag Pattern for FID_HOST_READOUT
#define FLG_HOST_READOUT_MIPI_CH0	0x00000000
#define FLG_HOST_READOUT_MIPI_CH1	0x01000000

#define FLG_HOST_READOUT_SLVS_CH0	0x10000000

#define FLG_HOST_READOUT_LVDS_CH0	0x20000000
#define FLG_HOST_READOUT_LVDS_CH1	0x21000000

#define FLG_HOST_READOUT_START	0x00000001
#define FLG_HOST_READOUT_END	0x00000002
/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_MEM_BANK_SET = 0,
	E_MEM_BANK_ING,
	E_MEM_BANK_RCV,
	E_MEM_BANK_MAX,
} E_BANK;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#ifdef CO_BF_MBALOG_ON
#define MPRINTF(arg)		(BF_MBA_IF_Printf_LineID arg)
#else
#define MPRINTF(arg)
#endif
#define UPRINTF				FJ_Debug_Print_Dbg
#define UPRINTF_ERR(ercd) \
	do { \
		if( ercd ){ \
			FJ_Debug_Print_Err( "(x o x) %s, %d, ercd=0x%x\n", __FUNCTION__, __LINE__, ercd ); \
		} \
	}while(0);

#define UPRINTF_WRN(ercd) \
	do { \
		if( ercd ){ \
			FJ_Debug_Print_Wrn( "(x o x) %s, %d, ercd=0x%x\n", __FUNCTION__, __LINE__, ercd ); \
		} \
	}while(0);

#define COUNT_SET_FLG( flg, n) \
	do { \
		n = flg; \
		n = ((n&0xaaaaaaaa) >> 1) + (n&0x55555555); \
		n = ((n&0xcccccccc) >> 2) + (n&0x33333333); \
		n = ((n&0xf0f0f0f0) >> 4) + (n&0x0f0f0f0f); \
		n = ((n&0xff00ff00) >> 8) + (n&0x00ff00ff); \
		n = ((n&0xffff0000) >> 16) + (n&0x0000ffff); \
	}while(0);


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif	// _SHARE_H_

