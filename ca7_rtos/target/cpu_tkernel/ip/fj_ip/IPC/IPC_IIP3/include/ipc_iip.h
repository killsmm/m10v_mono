/**
 * @file		ipc_iip.h
 * @brief		IPC_IIP Header
 * @author		hata
 * @since		2014/04/20
 * @date		2016/06/10
 * @note		IPC_IIP Header
 * @attention	None
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; Socionext Inc. 2015-2016</I></B>
 */

#ifndef _IPC_IIP_H_
#define _IPC_IIP_H_

#include "ipc_if.h"

/*--------------------------------------------------------------------------------------*/
/* Definition																			*/
/*--------------------------------------------------------------------------------------*/
#define D_IPC_IIP_OK				(D_IPC_OK)					/* OK */
#define D_IPC_IIP_NG				(D_IPC_NG|0x000E0000)		/* NG */

#define D_IPC_IIP_NG_ARG			(D_IPC_IIP_NG|0x00000001)	/* Argument Error		*/
#define D_IPC_IIP_NG_RESOURCE		(D_IPC_IIP_NG|0x00000002)	/* Resource Error		*/
#define D_IPC_IIP_NG_MEMORY			(D_IPC_IIP_NG|0x00000004)	/* Memory   Error		*/
#define D_IPC_IIP_NG_TIMEOUT		(D_IPC_IIP_NG|0x00000008)	/* TimeOut  Error		*/
#define D_IPC_IIP_NG_UNKNOWN		(D_IPC_IIP_NG|0x00008000)	/* Unknown  Error		*/

#define D_IPC_IIP_MASTER_IF0		(0x20)						/* MasterIF0			*/
#define D_IPC_IIP_MASTER_IF1		(0x40)						/* MasterIF1			*/
#define D_IPC_IIP_MASTER_IF2		(0x80)						/* MasterIF2			*/
#define D_IPC_IIP_MASTER_IFX		(0x00)						/* default				*/

/* UNIT ID */
typedef enum {
	E_IPC_IIP_UNIT_ID_1D0			= 0,			/* 1D Loader0 (0x00000000 00000001)	*/
	E_IPC_IIP_UNIT_ID_1D1			= 1,			/* 1D Loader1 (0x00000000 00000002)	*/
	E_IPC_IIP_UNIT_ID_1D2			= 2,			/* 1D Loader2 (0x00000000 00000004)	*/
	E_IPC_IIP_UNIT_ID_1D3			= 3,			/* 1D Loader3 (0x00000000 00000008)	*/
	E_IPC_IIP_UNIT_ID_1D4			= 4,			/* 1D Loader4 (0x00000000 00000010)	*/
	E_IPC_IIP_UNIT_ID_1D5			= 5,			/* 1D Loader5 (0x00000000 00000020)	*/
	E_IPC_IIP_UNIT_ID_1D6			= 6,			/* 1D Loader6 (0x00000000 00000040)	*/
	E_IPC_IIP_UNIT_ID_1D7			= 7,			/* 1D Loader7 (0x00000000 00000080)	*/
	E_IPC_IIP_UNIT_ID_FLT			= 8,			/* FLT		  (0x00000000 00000100)	*/
	E_IPC_IIP_UNIT_ID_AFN0			= 9,			/* AFN 0	  (0x00000000 00000200)	*/
	E_IPC_IIP_UNIT_ID_AFN1			= 10,			/* AFN 1	  (0x00000000 00000400)	*/
	E_IPC_IIP_UNIT_ID_AFN2			= 11,			/* AFN 2	  (0x00000000 00000800)	*/
	E_IPC_IIP_UNIT_ID_AFN3			= 12,			/* AFN 3	  (0x00000000 00001000)	*/
	E_IPC_IIP_UNIT_ID_FRECT0		= 13,			/* FRECT 0	  (0x00000000 00002000)	*/
	E_IPC_IIP_UNIT_ID_FRECT1		= 14,			/* FRECT 1	  (0x00000000 00004000)	*/
	E_IPC_IIP_UNIT_ID_SL0			= 15,			/* Store 0	  (0x00000000 00008000)	*/
	E_IPC_IIP_UNIT_ID_SL1			= 16,			/* Store 1	  (0x00000000 00010000)	*/
	E_IPC_IIP_UNIT_ID_SL2			= 17,			/* Store 2	  (0x00000000 00020000)	*/
	E_IPC_IIP_UNIT_ID_SL3			= 18,			/* Store 3	  (0x00000000 00040000)	*/
	E_IPC_IIP_UNIT_ID_SL4			= 19,			/* Store 4	  (0x00000000 00080000)	*/
	E_IPC_IIP_UNIT_ID_SL5			= 20,			/* Store 5	  (0x00000000 00100000)	*/
	E_IPC_IIP_UNIT_ID_SL6			= 21,			/* Store 6	  (0x00000000 00200000)	*/
	E_IPC_IIP_UNIT_ID_SL7			= 22,			/* Store 7	  (0x00000000 00400000)	*/
	E_IPC_IIP_UNIT_ID_SL8			= 23,			/* Store 8	  (0x00000000 00800000)	*/
	E_IPC_IIP_UNIT_ID_CSC0			= 24,			/* CSC 0	  (0x00000000 01000000)	*/
	E_IPC_IIP_UNIT_ID_CSC1			= 25,			/* CSC 1	  (0x00000000 02000000)	*/
	E_IPC_IIP_UNIT_ID_CSC2			= 26,			/* CSC 2	  (0x00000000 04000000)	*/
	E_IPC_IIP_UNIT_ID_CSC3			= 27,			/* CSC 3	  (0x00000000 08000000)	*/
	E_IPC_IIP_UNIT_ID_LUT			= 28,			/* LUT		  (0x00000000 10000000)	*/
	E_IPC_IIP_UNIT_ID_GPC			= 29,			/* GPC		  (0x00000000 20000000)	*/
	E_IPC_IIP_UNIT_ID_BLEND0		= 30,			/* Blend 0	  (0x00000000 40000000)	*/
	E_IPC_IIP_UNIT_ID_BLEND1		= 31,			/* Blend 1	  (0x00000000 80000000)	*/
	E_IPC_IIP_UNIT_ID_MON			= 32,			/* Moniter	  (0x00000001 00000000)	*/
	E_IPC_IIP_UNIT_ID_CFC			= 33,			/* Cache Flow (0x00000002 00000000)	*/
	E_IPC_IIP_UNIT_ID_MFT			= 34,			/* MFT		  (0x00000004 00000000)	*/
	E_IPC_IIP_UNIT_ID_MAX
} E_IPC_IIP_UNIT_ID;

/* UNIT ID Flags */
#define D_IPC_IIP_UNIT_FLG_1D0		(1ULL << E_IPC_IIP_UNIT_ID_1D0)
#define D_IPC_IIP_UNIT_FLG_1D1		(1ULL << E_IPC_IIP_UNIT_ID_1D1)
#define D_IPC_IIP_UNIT_FLG_1D2		(1ULL << E_IPC_IIP_UNIT_ID_1D2)
#define D_IPC_IIP_UNIT_FLG_1D3		(1ULL << E_IPC_IIP_UNIT_ID_1D3)
#define D_IPC_IIP_UNIT_FLG_1D4		(1ULL << E_IPC_IIP_UNIT_ID_1D4)
#define D_IPC_IIP_UNIT_FLG_1D5		(1ULL << E_IPC_IIP_UNIT_ID_1D5)
#define D_IPC_IIP_UNIT_FLG_1D6		(1ULL << E_IPC_IIP_UNIT_ID_1D6)
#define D_IPC_IIP_UNIT_FLG_1D7		(1ULL << E_IPC_IIP_UNIT_ID_1D7)
#define D_IPC_IIP_UNIT_FLG_FLT		(1ULL << E_IPC_IIP_UNIT_ID_FLT)
#define D_IPC_IIP_UNIT_FLG_AFN0		(1ULL << E_IPC_IIP_UNIT_ID_AFN0)
#define D_IPC_IIP_UNIT_FLG_AFN1		(1ULL << E_IPC_IIP_UNIT_ID_AFN1)
#define D_IPC_IIP_UNIT_FLG_AFN2		(1ULL << E_IPC_IIP_UNIT_ID_AFN2)
#define D_IPC_IIP_UNIT_FLG_AFN3		(1ULL << E_IPC_IIP_UNIT_ID_AFN3)
#define D_IPC_IIP_UNIT_FLG_FRECT0	(1ULL << E_IPC_IIP_UNIT_ID_FRECT0)
#define D_IPC_IIP_UNIT_FLG_FRECT1	(1ULL << E_IPC_IIP_UNIT_ID_FRECT1)
#define D_IPC_IIP_UNIT_FLG_SL0		(1ULL << E_IPC_IIP_UNIT_ID_SL0)
#define D_IPC_IIP_UNIT_FLG_SL1		(1ULL << E_IPC_IIP_UNIT_ID_SL1)
#define D_IPC_IIP_UNIT_FLG_SL2		(1ULL << E_IPC_IIP_UNIT_ID_SL2)
#define D_IPC_IIP_UNIT_FLG_SL3		(1ULL << E_IPC_IIP_UNIT_ID_SL3)
#define D_IPC_IIP_UNIT_FLG_SL4		(1ULL << E_IPC_IIP_UNIT_ID_SL4)
#define D_IPC_IIP_UNIT_FLG_SL5		(1ULL << E_IPC_IIP_UNIT_ID_SL5)
#define D_IPC_IIP_UNIT_FLG_SL6		(1ULL << E_IPC_IIP_UNIT_ID_SL6)
#define D_IPC_IIP_UNIT_FLG_SL7		(1ULL << E_IPC_IIP_UNIT_ID_SL7)
#define D_IPC_IIP_UNIT_FLG_SL8		(1ULL << E_IPC_IIP_UNIT_ID_SL8)
#define D_IPC_IIP_UNIT_FLG_CSC0		(1ULL << E_IPC_IIP_UNIT_ID_CSC0)
#define D_IPC_IIP_UNIT_FLG_CSC1		(1ULL << E_IPC_IIP_UNIT_ID_CSC1)
#define D_IPC_IIP_UNIT_FLG_CSC2		(1ULL << E_IPC_IIP_UNIT_ID_CSC2)
#define D_IPC_IIP_UNIT_FLG_CSC3		(1ULL << E_IPC_IIP_UNIT_ID_CSC3)
#define D_IPC_IIP_UNIT_FLG_LUT		(1ULL << E_IPC_IIP_UNIT_ID_LUT)
#define D_IPC_IIP_UNIT_FLG_GPC		(1ULL << E_IPC_IIP_UNIT_ID_GPC)
#define D_IPC_IIP_UNIT_FLG_BLEND0	(1ULL << E_IPC_IIP_UNIT_ID_BLEND0)
#define D_IPC_IIP_UNIT_FLG_BLEND1	(1ULL << E_IPC_IIP_UNIT_ID_BLEND1)
#define D_IPC_IIP_UNIT_FLG_MON		(1ULL << E_IPC_IIP_UNIT_ID_MON)
#define D_IPC_IIP_UNIT_FLG_CFC		(1ULL << E_IPC_IIP_UNIT_ID_CFC)
#define D_IPC_IIP_UNIT_FLG_MFT		(1ULL << E_IPC_IIP_UNIT_ID_MFT)
#define D_IPC_IIP_UNIT_FLG_MAX		(1ULL << E_IPC_IIP_UNIT_ID_MAX)

/* PIXID */
typedef enum{
	E_IPC_IIP_PIXID_0				= 0,			/* PIXFMTTBL 0						*/
    E_IPC_IIP_PIXID_1,								/* PIXFMTTBL 1						*/
    E_IPC_IIP_PIXID_2,								/* PIXFMTTBL 2						*/
    E_IPC_IIP_PIXID_3,								/* PIXFMTTBL 3						*/
    E_IPC_IIP_PIXID_4,								/* PIXFMTTBL 4						*/
    E_IPC_IIP_PIXID_5,								/* PIXFMTTBL 5						*/
    E_IPC_IIP_PIXID_6,								/* PIXFMTTBL 6						*/
    E_IPC_IIP_PIXID_7,								/* PIXFMTTBL 7						*/
    E_IPC_IIP_PIXID_8,								/* PIXFMTTBL 8						*/
    E_IPC_IIP_PIXID_9,								/* PIXFMTTBL 9						*/
    E_IPC_IIP_PIXID_10,								/* PIXFMTTBL 10						*/
    E_IPC_IIP_PIXID_11,								/* PIXFMTTBL 11						*/
    E_IPC_IIP_PIXID_MAX,
} E_IPC_IIP_PIXID;

/* PIXID Flags */
#define D_IPC_IIP_PIXID_FLG_0 		(1 << E_IPC_IIP_PIXID_0)
#define D_IPC_IIP_PIXID_FLG_1 		(1 << E_IPC_IIP_PIXID_1)
#define D_IPC_IIP_PIXID_FLG_2 		(1 << E_IPC_IIP_PIXID_2)
#define D_IPC_IIP_PIXID_FLG_3 		(1 << E_IPC_IIP_PIXID_3)
#define D_IPC_IIP_PIXID_FLG_4 		(1 << E_IPC_IIP_PIXID_4)
#define D_IPC_IIP_PIXID_FLG_5 		(1 << E_IPC_IIP_PIXID_5)
#define D_IPC_IIP_PIXID_FLG_6 		(1 << E_IPC_IIP_PIXID_6)
#define D_IPC_IIP_PIXID_FLG_7 		(1 << E_IPC_IIP_PIXID_7)
#define D_IPC_IIP_PIXID_FLG_8 		(1 << E_IPC_IIP_PIXID_8)
#define D_IPC_IIP_PIXID_FLG_9 		(1 << E_IPC_IIP_PIXID_9)
#define D_IPC_IIP_PIXID_FLG_10		(1 << E_IPC_IIP_PIXID_10)
#define D_IPC_IIP_PIXID_FLG_11		(1 << E_IPC_IIP_PIXID_11)
#define D_IPC_IIP_PIXID_FLG_MAX		(1 << E_IPC_IIP_PIXID_MAX)

/* RESOURCE */
typedef enum{
	E_IPC_IIP_RES_NONE				= 0,			/* None								*/
    E_IPC_IIP_RES_LUT_A				= 0,			/* LUT A							*/
    E_IPC_IIP_RES_LUT_B,							/* LUT B							*/
    E_IPC_IIP_RES_LUT_C,							/* LUT C							*/
    E_IPC_IIP_RES_LUT_D,							/* LUT D							*/
    E_IPC_IIP_RES_LUT_E,							/* LUT E							*/
    E_IPC_IIP_RES_LUT_F,							/* LUT F							*/
    E_IPC_IIP_RES_HIST0,							/* Histgram 0						*/
    E_IPC_IIP_RES_HIST1,							/* Histgram 1						*/
    E_IPC_IIP_RES_HIST2,							/* Histgram 2						*/
    E_IPC_IIP_RES_HIST3,							/* Histgram 3						*/
	E_IPC_IIP_RES_CACHE0,							/* Image Cache0(Resource獲得時無効)	*/
    E_IPC_IIP_RES_CACHE1,							/* Image Cache1(Resource獲得時無効)	*/
    E_IPC_IIP_RES_CACHE2,							/* Image Cache2(Resource獲得時無効)	*/
	E_IPC_IIP_RES_BICUBIC,							/* Bi-Cubic0or1(Internal利用時無効)	*/
    E_IPC_IIP_RES_BICUBIC0,							/* Bi-Cubic0						*/
    E_IPC_IIP_RES_BICUBIC1,							/* Bi-Cubic1						*/
    E_IPC_IIP_RES_MAX
} E_IPC_IIP_RES;

/* RESOURCE Flags */
#define D_IPC_IIP_OPEN_RES_NONE		(0 << E_IPC_IIP_RES_NONE)
#define D_IPC_IIP_OPEN_RES_LUT_A	(1 << E_IPC_IIP_RES_LUT_A)
#define D_IPC_IIP_OPEN_RES_LUT_B	(1 << E_IPC_IIP_RES_LUT_B)
#define D_IPC_IIP_OPEN_RES_LUT_C	(1 << E_IPC_IIP_RES_LUT_C)
#define D_IPC_IIP_OPEN_RES_LUT_D	(1 << E_IPC_IIP_RES_LUT_D)
#define D_IPC_IIP_OPEN_RES_LUT_E	(1 << E_IPC_IIP_RES_LUT_E)
#define D_IPC_IIP_OPEN_RES_LUT_F	(1 << E_IPC_IIP_RES_LUT_F)
#define D_IPC_IIP_OPEN_RES_HIST0	(1 << E_IPC_IIP_RES_HIST0)
#define D_IPC_IIP_OPEN_RES_HIST1	(1 << E_IPC_IIP_RES_HIST1)
#define D_IPC_IIP_OPEN_RES_HIST2	(1 << E_IPC_IIP_RES_HIST2)
#define D_IPC_IIP_OPEN_RES_HIST3	(1 << E_IPC_IIP_RES_HIST3)
#define D_IPC_IIP_OPEN_RES_CACHE0	(1 << E_IPC_IIP_RES_CACHE0)	/* Resource獲得時無効	*/
#define D_IPC_IIP_OPEN_RES_CACHE1	(1 << E_IPC_IIP_RES_CACHE1)	/* Resource獲得時無効	*/
#define D_IPC_IIP_OPEN_RES_CACHE2	(1 << E_IPC_IIP_RES_CACHE2)	/* Resource獲得時無効	*/
#define D_IPC_IIP_OPEN_RES_BICUBIC	(1 << E_IPC_IIP_RES_BICUBIC)/* Internal利用時無効	*/
#define D_IPC_IIP_OPEN_RES_BICUBIC0	(1 << E_IPC_IIP_RES_BICUBIC0)
#define D_IPC_IIP_OPEN_RES_BICUBIC1	(1 << E_IPC_IIP_RES_BICUBIC1)
#define D_IPC_IIP_OPEN_RES_MAX		(1 << E_IPC_IIP_RES_MAX)

/*--------------------------------------------------------------------------------------*/
/* Enumeration																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Structure																			*/
/*--------------------------------------------------------------------------------------*/
typedef struct {
	IPC_UCHAR	oned;								/* 1D Loader						*/
	IPC_UCHAR	flt;								/* FLT								*/
	IPC_UCHAR	afn0;								/* AFN 0(YCbCr14bit)				*/
	IPC_UCHAR	afnx;								/* AFN 0 or AFN 1 or AFN 2			*/
	IPC_UCHAR	frect;								/* FRECT							*/
	IPC_UCHAR	mon;								/* MON								*/
	IPC_UCHAR	slm;								/* SL 0-2(MASK機能付き)				*/
	IPC_UCHAR	slh;								/* SL 0-7(Histgram取得付き)			*/
	IPC_UCHAR	slx;								/* SL 0-8							*/
	IPC_UCHAR	csc;								/* CSC								*/
	IPC_UCHAR	lut;								/* LUT								*/
	IPC_UCHAR	gpc;								/* GPC								*/
	IPC_UCHAR	blend;								/* blend							*/
	IPC_UCHAR	cfc;								/* CFC								*/
	IPC_UCHAR	mft;								/* MFT								*/
} T_IPC_IIP_REQ_UNIT;

typedef struct {
	T_IPC_IIP_REQ_UNIT	unit;						/* 要求するUnitとその数				*/
	IPC_UCHAR			pixid;						/* 要求するPixelID数				*/
	IPC_UINT			other;						/* 要求するLUT/Histgram/Bi-Cubic
													   (D_IPC_IIP_OPEN_RES_xxx形式)		*/
} T_IPC_IIP_RES_REQ;

typedef struct {
	IPC_LLONG unit_flags;							/* D_IPC_IIP_UNIT_FLG_xxx 形式		*/
	IPC_UINT  pixid_flags;							/* D_IPC_IIP_PIXID_FLG_xxx形式		*/
	IPC_UINT  other_flags;							/* D_IPC_IIP_OPEN_RES_xxx 形式		*/
} T_IPC_IIP_RES;

/*--------------------------------------------------------------------------------------*/
/* Global Data																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Macro																				*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Function  																			*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* API		  																			*/
/*--------------------------------------------------------------------------------------*/
/* IPC_IIP Lib Get Version */
extern IPC_VOID IPC_IIP_Get_Version( 
	IPC_CHAR**		str 
);

/*--------------------------------------------------------------------------------------*/
/* Version  																			*/
/*--------------------------------------------------------------------------------------*/
#define V_Ipc_Iip_Version		"flib_version:V03L00;\r\n"

#endif	// _IPC_IIP_H_

/* V00L01 2014.04.25 IPC_IIP2 ver0.1 */
/* V01L00 2014.05.30 IPC_IIP2 ver1.0 */
/* V01L01 2014.07.10 IPC_IIP2 ver1.1 */
/* V01L02 2014.07.14 IPC_IIP2 ver1.2 */
/* V01L03 2014.07.30 IPC_IIP2 ver1.3 */
/* V01L04 2014.08.11 IPC_IIP2 ver1.4 */
/* V01L05 2014.08.21 IPC_IIP2 ver1.5 */
/* Socionext Inc. */
/* V01L05 2015.03.17 IPC_IIP2 ver1.5 */
/* V01L05 2016.06.10 IPC_IIP3 ver3.0 */

