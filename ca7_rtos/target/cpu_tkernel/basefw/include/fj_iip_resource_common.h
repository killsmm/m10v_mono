/**
 * @file		fj_iip_resource_common.h
 * @brief		IIP Resource Management API.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_IIP_RESOURCE_COMMON_H_
#define _FJ_IIP_RESOURCE_COMMON_H_

#include "ddim_typedef.h"

///////////////////////////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_iip_resource_definition
@{*/
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/** Other resource bitmask */
#define D_FJ_IIP_RES_OTHER_NONE				(0)					/**< Nothing */
#define D_FJ_IIP_RES_OTHER_LUT_A			(1 << 0)			/**< LUT A */
#define D_FJ_IIP_RES_OTHER_LUT_B			(1 << 1)			/**< LUT B */
#define D_FJ_IIP_RES_OTHER_LUT_C			(1 << 2)			/**< LUT C */
#define D_FJ_IIP_RES_OTHER_LUT_D			(1 << 3)			/**< LUT D */
#define D_FJ_IIP_RES_OTHER_LUT_E			(1 << 4)			/**< LUT E */
#define D_FJ_IIP_RES_OTHER_LUT_F			(1 << 5)			/**< LUT F */
#define D_FJ_IIP_RES_OTHER_HIST_0			(1 << 6)			/**< Histogram 0 */
#define D_FJ_IIP_RES_OTHER_HIST_1			(1 << 7)			/**< Histogram 1 */
#define D_FJ_IIP_RES_OTHER_HIST_2			(1 << 8)			/**< Histogram 2 */
#define D_FJ_IIP_RES_OTHER_HIST_3			(1 << 9)			/**< Histogram 3 */
#define D_FJ_IIP_RES_OTHER_BICUBIC_0		(1 << 13)			/**< Bi-Cubic calculator 0 */
#define D_FJ_IIP_RES_OTHER_BICUBIC_1		(1 << 14)			/**< Bi-Cubic calculator 1 */
#define D_FJ_IIP_RES_OTHER_BICUBIC_0_OR_1	(1 << 15)			/**< Bi-Cubic calculator 0 or 1 */


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** Lock requested number by unit */
typedef struct {
	UCHAR		oned;								/**< 1D-Loader */
	UCHAR		flt;								/**< FLT */
	UCHAR		afn_0;								/**< AFN0 only */
	UCHAR		afn_1;								/**< AFN1 only */
	UCHAR		afn_2;								/**< AFN2 only */
	UCHAR		afn;								/**< AFN 3~0 */
	UCHAR		frect;								/**< FRECT */
	UCHAR		mon;								/**< MON */
	UCHAR		csc;								/**< CSC */
	UCHAR		lut;								/**< LUT */
	UCHAR		gpc;								/**< GPC */
	UCHAR		blend;								/**< BLEND */
	UCHAR		cfl;								/**< CFL */
	UCHAR		mft;								/**< MFT */
	UCHAR		sl;									/**< SL 8, 7, 6, 4, 3, 5, 2, 1 or 0 */
	UCHAR		sl_0_2;								/**< SL 0~2 (Full function) */
	UCHAR		sl_0_2_master_if_0;					/**< SL 0 (Full function) Master I/F 0 supported */
	UCHAR		sl_0_2_master_if_1;					/**< SL 1 or 2 (Full function) Master I/F 1 supported */
	UCHAR		sl_0_2_master_if_2;					/**< SL 2 (Full function) Master I/F 2 supported */
	UCHAR		sl_3_8;								/**< SL 8, 7, 6, 4, 3 or 5 (Rectangle fill is unsupported) */
	UCHAR		sl_master_if_0;						/**< SL 6, 3, 5 or 0 (6,3,0:Master I/F 0 only  5:Master I/F0 or 2) */
	UCHAR		sl_master_if_1;						/**< SL 7, 4, 1 or 2 (7,4,1:Master I/F 1 only  2:Master I/F1 or 2) */
	UCHAR		sl_master_if_2;						/**< SL 8, 5 or 2 (8:Master I/F 2 only  2:Master I/F0 or 2  5:Master I/F0 or 2) */
	UCHAR		sl0;								/**< SL0 only */
	UCHAR		sl1;								/**< SL1 only */
	UCHAR		sl2;								/**< SL2 only */
	UCHAR		sl3;								/**< SL3 only */
	UCHAR		sl4;								/**< SL4 only */
	UCHAR		sl5;								/**< SL5 only */
	UCHAR		sl6;								/**< SL6 only */
	UCHAR		sl7;								/**< SL7 only */
	UCHAR		sl8;								/**< SL8 only */
	UCHAR		sl_mon;								/**< SL 7, 6, 4, 3, 5, 2, 1 or 0 (for MON or CFL) */
	UCHAR		sl_mon_master_if_0;					/**< SL 6, 3, 5 or 0 (for MON or CFL) Master I/F 0 supported */
	UCHAR		sl_mon_master_if_1;					/**< SL 7, 4, 1 or 2 (for MON or CFL) Master I/F 1 supported */
	UCHAR		sl_mon_master_if_2;					/**< SL 5 or 2 (for MON or CFL) Master I/F 2 supported */
} T_FJ_IIP_RES_UNIT_CONFIG_ARG;

/** Lock requested number */
typedef struct {
	T_FJ_IIP_RES_UNIT_CONFIG_ARG	unit;			/**< Lock requested number by unit */
	UCHAR							pixid;			/**< Lock requested number by PIXFMTTBL(PIXID) */
	UINT32							other;			/**< Lock requested resource bitmask (D_FJ_IIP_RES_OTHER_XXXX) */
} T_FJ_IIP_RES_CONFIG_ARG;

/** IIP resource bitmask */
typedef struct {
	ULLONG						unitid_bitmask64;	/**< Locked unit bitmask */
	UINT32						pixid_bitmask;		/**< Locked PIXFMTTBL(PIXID) bitmask */
	UINT32						other_bitmask;		/**< Locked resource bitmask (bitmask of D_FJ_IIP_RES_OTHER_XXXX other than D_FJ_IIP_RES_OTHER_BICUBIC_0_OR_1) */
} T_FJ_IIP_RES_CONFIG_OUT;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special
/*@} --- end of fj_iip_resource_definition */
/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
// Nothing Special

#endif	// _FJ_IIP_RESOURCE_COMMON_H_

