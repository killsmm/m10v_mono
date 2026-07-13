/**
 * @file		iip_resmng.c
 * @brief		IIP resource management processing core
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "dd_arm.h"

#include "iip_resmng.h"
#include "iip_resmng_debug.h"
#include "iip_resmng_secret.h"
#include "iip_res_user_custom.h"
#include <stdlib.h>
#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define D_D_IIP_RES_DEBUG

/** Shift Bit */
#define D_IIP_RES_LEFT_SHIFT_0			(0x0000000000000001)
#define D_IIP_RES_LEFT_SHIFT_1			(0x0000000000000002)
#define D_IIP_RES_LEFT_SHIFT_2			(0x0000000000000004)
#define D_IIP_RES_LEFT_SHIFT_3			(0x0000000000000008)
#define D_IIP_RES_LEFT_SHIFT_4			(0x0000000000000010)
#define D_IIP_RES_LEFT_SHIFT_5			(0x0000000000000020)
#define D_IIP_RES_LEFT_SHIFT_6			(0x0000000000000040)
#define D_IIP_RES_LEFT_SHIFT_7			(0x0000000000000080)
#define D_IIP_RES_LEFT_SHIFT_8			(0x0000000000000100)
#define D_IIP_RES_LEFT_SHIFT_9			(0x0000000000000200)
#define D_IIP_RES_LEFT_SHIFT_10			(0x0000000000000400)
#define D_IIP_RES_LEFT_SHIFT_11			(0x0000000000000800)
#define D_IIP_RES_LEFT_SHIFT_12			(0x0000000000001000)
#define D_IIP_RES_LEFT_SHIFT_13			(0x0000000000002000)
#define D_IIP_RES_LEFT_SHIFT_14			(0x0000000000004000)
#define D_IIP_RES_LEFT_SHIFT_15			(0x0000000000008000)
#define D_IIP_RES_LEFT_SHIFT_16			(0x0000000000010000)
#define D_IIP_RES_LEFT_SHIFT_17			(0x0000000000020000)
#define D_IIP_RES_LEFT_SHIFT_18			(0x0000000000040000)
#define D_IIP_RES_LEFT_SHIFT_19			(0x0000000000080000)
#define D_IIP_RES_LEFT_SHIFT_20			(0x0000000000100000)
#define D_IIP_RES_LEFT_SHIFT_21			(0x0000000000200000)
#define D_IIP_RES_LEFT_SHIFT_22			(0x0000000000400000)
#define D_IIP_RES_LEFT_SHIFT_23			(0x0000000000800000)
#define D_IIP_RES_LEFT_SHIFT_24			(0x0000000001000000)
#define D_IIP_RES_LEFT_SHIFT_25			(0x0000000002000000)
#define D_IIP_RES_LEFT_SHIFT_26			(0x0000000004000000)
#define D_IIP_RES_LEFT_SHIFT_27			(0x0000000008000000)
#define D_IIP_RES_LEFT_SHIFT_28			(0x0000000010000000)
#define D_IIP_RES_LEFT_SHIFT_29			(0x0000000020000000)
#define D_IIP_RES_LEFT_SHIFT_30			(0x0000000040000000)
#define D_IIP_RES_LEFT_SHIFT_31			(0x0000000080000000)
#define D_IIP_RES_LEFT_SHIFT_32			(0x0000000100000000)
#define D_IIP_RES_LEFT_SHIFT_33			(0x0000000200000000)
#define D_IIP_RES_LEFT_SHIFT_34			(0x0000000400000000)
#define D_IIP_RES_LEFT_SHIFT_35			(0x0000000800000000)
#define D_IIP_RES_LEFT_SHIFT_36			(0x0000001000000000)
#define D_IIP_RES_LEFT_SHIFT_37			(0x0000002000000000)
#define D_IIP_RES_LEFT_SHIFT_38			(0x0000004000000000)
#define D_IIP_RES_LEFT_SHIFT_39			(0x0000008000000000)
#define D_IIP_RES_LEFT_SHIFT_40			(0x0000010000000000)
#define D_IIP_RES_LEFT_SHIFT_41			(0x0000020000000000)
#define D_IIP_RES_LEFT_SHIFT_42			(0x0000040000000000)
#define D_IIP_RES_LEFT_SHIFT_43			(0x0000080000000000)
#define D_IIP_RES_LEFT_SHIFT_44			(0x0000100000000000)
#define D_IIP_RES_LEFT_SHIFT_45			(0x0000200000000000)
#define D_IIP_RES_LEFT_SHIFT_46			(0x0000400000000000)
#define D_IIP_RES_LEFT_SHIFT_47			(0x0000800000000000)
#define D_IIP_RES_LEFT_SHIFT_48			(0x0001000000000000)
#define D_IIP_RES_LEFT_SHIFT_49			(0x0002000000000000)
#define D_IIP_RES_LEFT_SHIFT_50			(0x0004000000000000)
#define D_IIP_RES_LEFT_SHIFT_51			(0x0008000000000000)
#define D_IIP_RES_LEFT_SHIFT_52			(0x0010000000000000)
#define D_IIP_RES_LEFT_SHIFT_53			(0x0020000000000000)
#define D_IIP_RES_LEFT_SHIFT_54			(0x0040000000000000)
#define D_IIP_RES_LEFT_SHIFT_55			(0x0080000000000000)
#define D_IIP_RES_LEFT_SHIFT_56			(0x0100000000000000)
#define D_IIP_RES_LEFT_SHIFT_57			(0x0200000000000000)
#define D_IIP_RES_LEFT_SHIFT_58			(0x0400000000000000)
#define D_IIP_RES_LEFT_SHIFT_59			(0x0800000000000000)
#define D_IIP_RES_LEFT_SHIFT_60			(0x1000000000000000)
#define D_IIP_RES_LEFT_SHIFT_61			(0x2000000000000000)
#define D_IIP_RES_LEFT_SHIFT_62			(0x4000000000000000)
#define D_IIP_RES_LEFT_SHIFT_63			(0x8000000000000000)

/** IIP Unit ID */
#define D_IIP_RES_UNIT_ID_1D0			D_IIP_RES_LEFT_SHIFT_0
#define D_IIP_RES_UNIT_ID_1D1			D_IIP_RES_LEFT_SHIFT_1
#define D_IIP_RES_UNIT_ID_1D2			D_IIP_RES_LEFT_SHIFT_2
#define D_IIP_RES_UNIT_ID_1D3			D_IIP_RES_LEFT_SHIFT_3
#define D_IIP_RES_UNIT_ID_FLT			D_IIP_RES_LEFT_SHIFT_4
#define D_IIP_RES_UNIT_ID_AFN0			D_IIP_RES_LEFT_SHIFT_5
#define D_IIP_RES_UNIT_ID_AFN1			D_IIP_RES_LEFT_SHIFT_6
#define D_IIP_RES_UNIT_ID_AFN2			D_IIP_RES_LEFT_SHIFT_7
#define D_IIP_RES_UNIT_ID_FRECT0		D_IIP_RES_LEFT_SHIFT_8
#define D_IIP_RES_UNIT_ID_MON			D_IIP_RES_LEFT_SHIFT_9
#define D_IIP_RES_UNIT_ID_SL0			D_IIP_RES_LEFT_SHIFT_10
#define D_IIP_RES_UNIT_ID_SL1			D_IIP_RES_LEFT_SHIFT_11
#define D_IIP_RES_UNIT_ID_SL2			D_IIP_RES_LEFT_SHIFT_12
#define D_IIP_RES_UNIT_ID_SL3			D_IIP_RES_LEFT_SHIFT_13
#define D_IIP_RES_UNIT_ID_SL4			D_IIP_RES_LEFT_SHIFT_14
#define D_IIP_RES_UNIT_ID_SL5			D_IIP_RES_LEFT_SHIFT_15
#define D_IIP_RES_UNIT_ID_CSC0			D_IIP_RES_LEFT_SHIFT_16
#define D_IIP_RES_UNIT_ID_CSC1			D_IIP_RES_LEFT_SHIFT_17
#define D_IIP_RES_UNIT_ID_LUT 			D_IIP_RES_LEFT_SHIFT_18
#define D_IIP_RES_UNIT_ID_GPC 			D_IIP_RES_LEFT_SHIFT_19
#define D_IIP_RES_UNIT_ID_BLEND0		D_IIP_RES_LEFT_SHIFT_20
#define D_IIP_RES_UNIT_ID_BLEND1		D_IIP_RES_LEFT_SHIFT_21
#define D_IIP_RES_UNIT_ID_FRECT1		D_IIP_RES_LEFT_SHIFT_22
#define D_IIP_RES_UNIT_ID_CFL0			D_IIP_RES_LEFT_SHIFT_23
#define D_IIP_RES_UNIT_ID_CSC2			D_IIP_RES_LEFT_SHIFT_27
#define D_IIP_RES_UNIT_ID_CSC3			D_IIP_RES_LEFT_SHIFT_28
#define D_IIP_RES_UNIT_ID_SL6			D_IIP_RES_LEFT_SHIFT_29
#define D_IIP_RES_UNIT_ID_SL7			D_IIP_RES_LEFT_SHIFT_30
#define D_IIP_RES_UNIT_ID_SL8			D_IIP_RES_LEFT_SHIFT_31
#define D_IIP_RES_UNIT_ID_1D4			D_IIP_RES_LEFT_SHIFT_32
#define D_IIP_RES_UNIT_ID_1D5			D_IIP_RES_LEFT_SHIFT_33
#define D_IIP_RES_UNIT_ID_1D6			D_IIP_RES_LEFT_SHIFT_34
#define D_IIP_RES_UNIT_ID_1D7			D_IIP_RES_LEFT_SHIFT_35
#define D_IIP_RES_UNIT_ID_AFN3			D_IIP_RES_LEFT_SHIFT_36
#define D_IIP_RES_UNIT_ID_MFT			D_IIP_RES_LEFT_SHIFT_37

#define D_IIP_RES_FLG_NOT_USED			(0)
#define D_IIP_RES_FLG_USED				(1)
#define D_IIP_RES_SETFLG_PTN			(0xFFFFFFFF)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Wait Pattern Value */
typedef enum {
	E_IIP_RES_WAIT_PTN_NONE		= 0,		
	E_IIP_RES_WAIT_PTN_1		= 1 << 0,	
	E_IIP_RES_WAIT_PTN_2		= 1 << 1,	
	E_IIP_RES_WAIT_PTN_3		= 1 << 2,	
	E_IIP_RES_WAIT_PTN_4		= 1 << 3,	
	E_IIP_RES_WAIT_PTN_5		= 1 << 4,	
	E_IIP_RES_WAIT_PTN_6		= 1 << 5,	
	E_IIP_RES_WAIT_PTN_7		= 1 << 6,	
	E_IIP_RES_WAIT_PTN_8		= 1 << 7,	
	E_IIP_RES_WAIT_PTN_9		= 1 << 8,	
	E_IIP_RES_WAIT_PTN_10		= 1 << 9,	
	E_IIP_RES_WAIT_PTN_11		= 1 << 10,	
	E_IIP_RES_WAIT_PTN_12		= 1 << 11,	
	E_IIP_RES_WAIT_PTN_13		= 1 << 12,	
	E_IIP_RES_WAIT_PTN_14		= 1 << 13,	
	E_IIP_RES_WAIT_PTN_15		= 1 << 14,	
	E_IIP_RES_WAIT_PTN_16		= 1 << 15,	
	E_IIP_RES_WAIT_PTN_17		= 1 << 16,	
	E_IIP_RES_WAIT_PTN_18		= 1 << 17,	
	E_IIP_RES_WAIT_PTN_19		= 1 << 18,	
	E_IIP_RES_WAIT_PTN_20		= 1 << 19,	
	E_IIP_RES_WAIT_PTN_21		= 1 << 20,	
	E_IIP_RES_WAIT_PTN_22		= 1 << 21,	
	E_IIP_RES_WAIT_PTN_23		= 1 << 22,	
	E_IIP_RES_WAIT_PTN_24		= 1 << 23,	
	E_IIP_RES_WAIT_PTN_25		= 1 << 24,	
	E_IIP_RES_WAIT_PTN_26		= 1 << 25,	
	E_IIP_RES_WAIT_PTN_27		= 1 << 26,	
	E_IIP_RES_WAIT_PTN_28		= 1 << 27,	
	E_IIP_RES_WAIT_PTN_29		= 1 << 28,	
	E_IIP_RES_WAIT_PTN_30		= 1 << 29,	
	E_IIP_RES_WAIT_PTN_31		= 1 << 30,	
	E_IIP_RES_WAIT_PTN_MAX		= 32		
} E_IIP_RES_WAIT_PTN;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	UCHAR				used;
	E_IIP_RES_WAIT_PTN	ptn;
} T_IIP_RES_WAIT_MNG;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Resource manager table
static T_IIP_RES_CONFIG_TBL gIIP_Config_Tbl = {
	{	// T_IIP_UNIT_RES_TBL
		.oned0  = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// oned0;
		.oned1  = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// oned1;
		.oned2  = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// oned2;
		.oned3  = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// oned3;
		.flt    = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// flt;
		.afn0   = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// afn0;
		.afn1   = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// afn1;
		.afn2   = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// afn2;
		.frect0 = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// frect0;
		.mon    = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// mon;
		.csc0   = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// csc0;
		.csc1   = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// csc1;
		.lut    = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// lut;
		.gpc    = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// gpc;
		.blend0 = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// blend0;
		.blend1 = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// blend1;
		.frect1 = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// frect1;
		.cfl0   = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// cfl0;
		.csc2   = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// csc2;
		.csc3   = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// csc3;
		.oned4  = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// oned4;
		.oned5  = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// oned5;
		.oned6  = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// oned6;
		.oned7  = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// oned7;
		.afn3   = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// afn3;
		.mft    = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// mft;
		.sl = {
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// sl0;
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// sl1;
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// sl2;
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// sl3;
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// sl4;
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// sl5;
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// sl6;
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// sl7;
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// sl8;
		},
	},
	{	// T_IIP_PIXID_RES_TBL
		{
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// PIXID 0
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// PIXID 1
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// PIXID 2
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// PIXID 3
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// PIXID 4
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// PIXID 5
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// PIXID 6
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// PIXID 7
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// PIXID 8
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// PIXID 9
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// PIXID 10
			{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}}	// PIXID 11
		}
	},
	{	// T_IIP_OTHER_RES_TBL
		{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// lut_a;
		{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// lut_b;
		{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// lut_c;
		{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// lut_d;
		{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// lut_e;
		{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// lut_f;
		{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// hist_0;
		{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// hist_1;
		{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// hist_2;
		{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// hist_3;
		{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}},	// bi_cubic_0;
		{ D_IIP_RES_CONFIG_STS_FREE, { 0, 0}}	// bi_cubic_1;
	}
};

static T_IIP_RES_WAIT_MNG gIIP_Resource_Wait_Tbl[E_IIP_RES_WAIT_PTN_MAX] = {
		{ 0, E_IIP_RES_WAIT_PTN_1},
		{ 0, E_IIP_RES_WAIT_PTN_2},
		{ 0, E_IIP_RES_WAIT_PTN_3},
		{ 0, E_IIP_RES_WAIT_PTN_4},
		{ 0, E_IIP_RES_WAIT_PTN_5},
		{ 0, E_IIP_RES_WAIT_PTN_6},
		{ 0, E_IIP_RES_WAIT_PTN_7},
		{ 0, E_IIP_RES_WAIT_PTN_8},
		{ 0, E_IIP_RES_WAIT_PTN_9},
		{ 0, E_IIP_RES_WAIT_PTN_10},
		{ 0, E_IIP_RES_WAIT_PTN_11},
		{ 0, E_IIP_RES_WAIT_PTN_12},
		{ 0, E_IIP_RES_WAIT_PTN_13},
		{ 0, E_IIP_RES_WAIT_PTN_14},
		{ 0, E_IIP_RES_WAIT_PTN_15},
		{ 0, E_IIP_RES_WAIT_PTN_16},
		{ 0, E_IIP_RES_WAIT_PTN_17},
		{ 0, E_IIP_RES_WAIT_PTN_18},
		{ 0, E_IIP_RES_WAIT_PTN_19},
		{ 0, E_IIP_RES_WAIT_PTN_20},
		{ 0, E_IIP_RES_WAIT_PTN_21},
		{ 0, E_IIP_RES_WAIT_PTN_22},
		{ 0, E_IIP_RES_WAIT_PTN_23},
		{ 0, E_IIP_RES_WAIT_PTN_24},
		{ 0, E_IIP_RES_WAIT_PTN_25},
		{ 0, E_IIP_RES_WAIT_PTN_26},
		{ 0, E_IIP_RES_WAIT_PTN_27},
		{ 0, E_IIP_RES_WAIT_PTN_28},
		{ 0, E_IIP_RES_WAIT_PTN_29},
		{ 0, E_IIP_RES_WAIT_PTN_30},
		{ 0, E_IIP_RES_WAIT_PTN_31}
};

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
 *	@brief		get the wait pattern
 *	@param		O: wai_ptn
 *	@return		INT32 Error code
 *	@note		None
 *	@attention	None
 */
static INT32 iip_res_lock_waiptn( ULONG *wai_ptn )
{
	UINT32 i;
	UCHAR find = 0;
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	Dd_ARM_Critical_Section_Start( critseq );

	for ( i = 0; i < E_IIP_RES_WAIT_PTN_MAX; i++ ) {
		if ( gIIP_Resource_Wait_Tbl[i].used == D_IIP_RES_FLG_NOT_USED ) {
			 gIIP_Resource_Wait_Tbl[i].used = D_IIP_RES_FLG_USED;
			 *wai_ptn = gIIP_Resource_Wait_Tbl[i].ptn;
			 find = 1;
			 break;
		}
	}

	Dd_ARM_Critical_Section_End( critseq );

	if ( !find ) {
		return D_IIP_ERR_NG;
	}

	return D_IIP_ERR_OK;
}

/**
 *	@brief		free the wait pattern
 *	@param		I: waiptn
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID iip_res_free_waiptn( ULONG waiptn )
{
	UINT32 i;
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));

	Dd_ARM_Critical_Section_Start( critseq );

	for ( i = 0; i < E_IIP_RES_WAIT_PTN_MAX; i++ ) {
		if ( gIIP_Resource_Wait_Tbl[i].ptn == waiptn ) {
			 gIIP_Resource_Wait_Tbl[i].used = D_IIP_RES_FLG_NOT_USED;
			 break;
		}
	}

	Dd_ARM_Critical_Section_End( critseq );

	return;
}

/**
 *	@brief		Wait flag
 *	@param		I: tsk_id
 *	@param		I: tout
 *	@return		INT32 Error code
 *	@note		None
 *	@attention	None
 */
static INT32 iip_res_wai_flg( UINT32 tout )
{
	INT32	ret;
	ULONG	huntedptn;
	ULONG	waiptn = 0;

	ret = iip_res_lock_waiptn( &waiptn );
	if ( ret != D_IIP_ERR_OK ) {
		return ret;
	}

	IIP_Res_Custom_Clr_Flg( ~waiptn );

	ret = IIP_Res_Custom_Twai_Flg( waiptn, 1, &huntedptn, tout );

	iip_res_free_waiptn( waiptn );

	return ret;
}

/**
 *	@brief		Get the total number of requests lock unit
 *	@param		I: T_IIP_RES_UNIT_CONFIG_ARG*	config
 *	@return		INT32: the total number of requests lock unit
 *	@note		None
 *	@attention	None
 */
static INT32 iip_res_get_request_unit_cnt( T_IIP_RES_UNIT_CONFIG_ARG* config )
{
	INT32 count = 0;

	count += config->oned;
	count += config->flt;
	count += config->afn_0;
	count += config->afn_1;
	count += config->afn_2;
	count += config->afn;
	count += config->frect;
	count += config->mon;
	count += config->csc;
	count += config->lut;
	count += config->gpc;
	count += config->blend;
	count += config->cfl;
	count += config->mft;
	count += config->sl;
	count += config->sl_0_2;
	count += config->sl_0_2_master_if_0;
	count += config->sl_0_2_master_if_1;
	count += config->sl_0_2_master_if_2;
	count += config->sl_3_8;
	count += config->sl_master_if_0;
	count += config->sl_master_if_1;
	count += config->sl_master_if_2;
	count += config->sl0;
	count += config->sl1;
	count += config->sl2;
	count += config->sl3;
	count += config->sl4;
	count += config->sl5;
	count += config->sl6;
	count += config->sl7;
	count += config->sl8;
	count += config->sl_mon;
	count += config->sl_mon_master_if_0;
	count += config->sl_mon_master_if_1;
	count += config->sl_mon_master_if_2;

	return count;
}

static UINT32 iip_res_lock_sl_unit( UCHAR* const unit_cnt, ULLONG* const bit_mask, T_IIP_RES_INFO unit_tbl_sl[], const UCHAR sl_no_tbl[], const T_IIP_RES_USER user_info )
{
	UINT32	loopcnt;
	UCHAR	unit_no;
	UINT32	lock_cnt = 0;
	static const ULLONG	sl_bit_mask_tbl[] = {
		D_IIP_RES_UNIT_ID_SL0,
		D_IIP_RES_UNIT_ID_SL1,
		D_IIP_RES_UNIT_ID_SL2,
		D_IIP_RES_UNIT_ID_SL3,
		D_IIP_RES_UNIT_ID_SL4,
		D_IIP_RES_UNIT_ID_SL5,
		D_IIP_RES_UNIT_ID_SL6,
		D_IIP_RES_UNIT_ID_SL7,
		D_IIP_RES_UNIT_ID_SL8,
	};

	for( loopcnt = 0; (sl_no_tbl[loopcnt] != 0xFF) && ((*unit_cnt) > 0); loopcnt++ ) {
		unit_no = sl_no_tbl[loopcnt];
		if ( unit_tbl_sl[unit_no].stat == D_IIP_RES_CONFIG_STS_FREE ) {
			unit_tbl_sl[unit_no].stat = D_IIP_RES_CONFIG_STS_USING;
			unit_tbl_sl[unit_no].user = user_info;
			*bit_mask |= sl_bit_mask_tbl[unit_no];
			(*unit_cnt)--;
			lock_cnt++;
		}
	}

	return lock_cnt;
}

/**
 *	@brief		Processing which locks an IIP unit
 *	@param		I: T_IIP_RES_UNIT_CONFIG_ARG*	unit_cnt
 *	@param		O: T_IIP_RES_UNIT_TBL*			unit_tbl
 *	@param		O: ULLONG*						bit_mask
 *	@param		I: T_IIP_RES_USER				user_info
 *	@return		Success: D_IIP_ERR_OK / Failure: D_IIP_ERR_NG
 *	@note		None
 *	@attention	None
 */
static INT32 iip_res_lock_unit( T_IIP_RES_UNIT_CONFIG_ARG* unit_cnt, T_IIP_RES_UNIT_TBL* unit_tbl, ULLONG* bit_mask, T_IIP_RES_USER user_info )
{
	INT32 total_cnt;
	UCHAR afn0_flg = 0;
	UCHAR afn1_flg = 0;
	UCHAR afn2_flg = 0;

	// get request total count
	total_cnt = iip_res_get_request_unit_cnt( unit_cnt );

	// OneD
	if ( unit_cnt->oned > 0 && unit_tbl->oned0.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->oned0.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->oned0.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_1D0;
		unit_cnt->oned--;
		total_cnt--;
	}
	if ( unit_cnt->oned > 0 && unit_tbl->oned1.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->oned1.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->oned1.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_1D1;
		unit_cnt->oned--;
		total_cnt--;
	}
	if ( unit_cnt->oned > 0 && unit_tbl->oned2.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->oned2.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->oned2.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_1D2;
		unit_cnt->oned--;
		total_cnt--;
	}
	if ( unit_cnt->oned > 0 && unit_tbl->oned3.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->oned3.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->oned3.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_1D3;
		unit_cnt->oned--;
		total_cnt--;
	}
	if ( unit_cnt->oned > 0 && unit_tbl->oned4.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->oned4.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->oned4.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_1D4;
		unit_cnt->oned--;
		total_cnt--;
	}
	if ( unit_cnt->oned > 0 && unit_tbl->oned5.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->oned5.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->oned5.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_1D5;
		unit_cnt->oned--;
		total_cnt--;
	}
	if ( unit_cnt->oned > 0 && unit_tbl->oned6.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->oned6.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->oned6.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_1D6;
		unit_cnt->oned--;
		total_cnt--;
	}
	if ( unit_cnt->oned > 0 && unit_tbl->oned7.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->oned7.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->oned7.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_1D7;
		unit_cnt->oned--;
		total_cnt--;
	}

	// FLT
	if ( unit_cnt->flt > 0 && unit_tbl->flt.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->flt.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->flt.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_FLT;
		unit_cnt->flt--;
		total_cnt--;
	}

	// AFN0
	if ( unit_cnt->afn_0 > 0 && unit_tbl->afn0.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->afn0.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->afn0.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_AFN0;
		unit_cnt->afn_0--;
		total_cnt--;
		afn0_flg = 1;
	}

	// AFN1
	if ( unit_cnt->afn_1 > 0 && unit_tbl->afn1.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->afn1.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->afn1.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_AFN1;
		unit_cnt->afn_1--;
		total_cnt--;
		afn1_flg = 1;
	}

	// AFN2
	if ( unit_cnt->afn_2 > 0 && unit_tbl->afn2.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->afn2.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->afn2.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_AFN2;
		unit_cnt->afn_2--;
		total_cnt--;
		afn2_flg = 1;
	}

	// AFN	(Search 3 >>2 >>1 >>0)
	if ( unit_cnt->afn > 0 && unit_tbl->afn3.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->afn3.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->afn3.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_AFN3;
		unit_cnt->afn--;
		total_cnt--;
	}
	
	if ( unit_cnt->afn > 0 && unit_tbl->afn2.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->afn2.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->afn2.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_AFN2;
		unit_cnt->afn--;
		total_cnt--;
	}
	else if ( afn2_flg && (unit_cnt->afn > 0) ) {
		unit_cnt->afn--;
		total_cnt--;
	}
	
	if ( unit_cnt->afn > 0 && unit_tbl->afn1.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->afn1.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->afn1.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_AFN1;
		unit_cnt->afn--;
		total_cnt--;
	}
	else if ( afn1_flg && (unit_cnt->afn > 0) ) {
		unit_cnt->afn--;
		total_cnt--;
	}

	if ( unit_cnt->afn > 0 && unit_tbl->afn0.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->afn0.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->afn0.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_AFN0;
		unit_cnt->afn--;
		total_cnt--;
	}
	else if ( afn0_flg && (unit_cnt->afn > 0) ) {
		unit_cnt->afn--;
		total_cnt--;
	}

	// FRECT
	if ( unit_cnt->frect > 0 &&  unit_tbl->frect0.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->frect0.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->frect0.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_FRECT0;
		unit_cnt->frect--;
		total_cnt--;
	}
	if ( unit_cnt->frect > 0 &&  unit_tbl->frect1.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->frect1.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->frect1.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_FRECT1;
		unit_cnt->frect--;
		total_cnt--;
	}

	// MON
	if ( unit_cnt->mon > 0 &&  unit_tbl->mon.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->mon.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->mon.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_MON;
		unit_cnt->mon--;
		total_cnt--;
	}

	// CSC
	if ( unit_cnt->csc > 0 &&  unit_tbl->csc0.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->csc0.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->csc0.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_CSC0;
		unit_cnt->csc--;
		total_cnt--;
	}
	if ( unit_cnt->csc > 0 &&  unit_tbl->csc1.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->csc1.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->csc1.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_CSC1;
		unit_cnt->csc--;
		total_cnt--;
	}
	if ( unit_cnt->csc > 0 &&  unit_tbl->csc2.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->csc2.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->csc2.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_CSC2;
		unit_cnt->csc--;
		total_cnt--;
	}
	if ( unit_cnt->csc > 0 &&  unit_tbl->csc3.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->csc3.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->csc3.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_CSC3;
		unit_cnt->csc--;
		total_cnt--;
	}

	// LUT
	if ( unit_cnt->lut > 0 && unit_tbl->lut.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->lut.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->lut.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_LUT;
		unit_cnt->lut--;
		total_cnt--;
	}
	
	// GPC
	if ( unit_cnt->gpc > 0 && unit_tbl->gpc.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->gpc.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->gpc.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_GPC;
		unit_cnt->gpc--;
		total_cnt--;
	}
	
	// BLEND
	if ( unit_cnt->blend > 0 && unit_tbl->blend0.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->blend0.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->blend0.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_BLEND0;
		unit_cnt->blend--;
		total_cnt--;
	}
	if ( unit_cnt->blend > 0 && unit_tbl->blend1.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->blend1.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->blend1.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_BLEND1;
		unit_cnt->blend--;
		total_cnt--;
	}

	// CFL
	if ( unit_cnt->cfl > 0 && unit_tbl->cfl0.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->cfl0.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->cfl0.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_CFL0;
		unit_cnt->cfl--;
		total_cnt--;
	}

	// MFT
	if ( unit_cnt->mft > 0 && unit_tbl->mft.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		unit_tbl->mft.stat = D_IIP_RES_CONFIG_STS_USING;
		unit_tbl->mft.user = user_info;
		*bit_mask |= D_IIP_RES_UNIT_ID_MFT;
		unit_cnt->mft--;
		total_cnt--;
	}

	// SL
	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl0, bit_mask, unit_tbl->sl, (const UCHAR[]) { 0, 0xFF }, user_info );
	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl1, bit_mask, unit_tbl->sl, (const UCHAR[]) { 1, 0xFF }, user_info );
	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl2, bit_mask, unit_tbl->sl, (const UCHAR[]) { 2, 0xFF }, user_info );
	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl3, bit_mask, unit_tbl->sl, (const UCHAR[]) { 3, 0xFF }, user_info );
	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl4, bit_mask, unit_tbl->sl, (const UCHAR[]) { 4, 0xFF }, user_info );
	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl5, bit_mask, unit_tbl->sl, (const UCHAR[]) { 5, 0xFF }, user_info );
	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl6, bit_mask, unit_tbl->sl, (const UCHAR[]) { 6, 0xFF }, user_info );
	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl7, bit_mask, unit_tbl->sl, (const UCHAR[]) { 7, 0xFF }, user_info );
	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl8, bit_mask, unit_tbl->sl, (const UCHAR[]) { 8, 0xFF }, user_info );

	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl_0_2_master_if_0, bit_mask, unit_tbl->sl, (const UCHAR[]) { 0, 0xFF }, user_info );
	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl_0_2_master_if_1, bit_mask, unit_tbl->sl, (const UCHAR[]) { 1, 2, 0xFF }, user_info );
	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl_0_2_master_if_2, bit_mask, unit_tbl->sl, (const UCHAR[]) { 2, 0xFF }, user_info );
	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl_0_2, bit_mask, unit_tbl->sl, (const UCHAR[]) { 0, 1, 2, 0xFF }, user_info );

	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl_mon_master_if_0, bit_mask, unit_tbl->sl, (const UCHAR[]) { 6, 3, 5, 0, 0xFF }, user_info );
	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl_mon_master_if_1, bit_mask, unit_tbl->sl, (const UCHAR[]) { 7, 4, 1, 2, 0xFF }, user_info );
	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl_mon_master_if_2, bit_mask, unit_tbl->sl, (const UCHAR[]) { 5, 2, 0xFF }, user_info );
	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl_mon, bit_mask, unit_tbl->sl, (const UCHAR[]) { 7, 6, 4, 3, 5, 2, 1, 0, 0xFF }, user_info );

	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl_master_if_0, bit_mask, unit_tbl->sl, (const UCHAR[]) { 6, 3, 5, 0, 0xFF }, user_info );
	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl_master_if_1, bit_mask, unit_tbl->sl, (const UCHAR[]) { 7, 4, 1, 2, 0xFF }, user_info );
	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl_master_if_2, bit_mask, unit_tbl->sl, (const UCHAR[]) { 8, 5, 2, 0xFF }, user_info );

	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl_3_8, bit_mask, unit_tbl->sl, (const UCHAR[]) { 8, 7, 6, 4, 3, 5, 0xFF }, user_info );

	total_cnt -= iip_res_lock_sl_unit( &unit_cnt->sl, bit_mask, unit_tbl->sl, (const UCHAR[]) { 8, 7, 6, 4, 3, 5, 2, 1, 0, 0xFF }, user_info );

	// Check locked all of requests
	if ( total_cnt != 0 ) {
		return D_IIP_ERR_NG;
	}

	return D_IIP_ERR_OK;
}

/**
 *	@brief		Processing which locks an IIP PIXFMTID
 *	@param		I: UCHAR*							pixid_cnt
 *	@param		O: T_IIP_RES_PIXID_TBL*				pixid_tbl
 *	@param		O: UINT32*							bit_mask
 *	@param		I: T_IIP_RES_USER					user_info
 *	@return		Success: D_IIP_ERR_OK / Failure: D_IIP_ERR_NG
 *	@note		None
 *	@attention	None
 */
static INT32 iip_res_lock_pixid( UCHAR* pixid_cnt, T_IIP_RES_PIXID_TBL* pixid_tbl, UINT32* bit_mask, T_IIP_RES_USER user_info )
{
	UINT32	i;

	// PIXFMTID
	for ( i = 0; i < D_IIP_RES_PIXID_MAXNUM; i++ ) {
		if ( pixid_tbl->pixid[i].stat == D_IIP_RES_CONFIG_STS_FREE ) {
			pixid_tbl->pixid[i].stat = D_IIP_RES_CONFIG_STS_USING;
			pixid_tbl->pixid[i].user = user_info;
			*bit_mask |= (1 << i);
			*pixid_cnt -= 1;
		}
		if ( *pixid_cnt == 0 ) {
			break;
		}
	}

	if ( *pixid_cnt != 0 ) {
		return D_IIP_ERR_NG;
	}

	return D_IIP_ERR_OK;
}

/**
 *	@brief		Processing which locks an IIP Other
 *	@param		I: UINT32*							other_cnt
 *	@param		O: T_IIP_RES_OTHER_TBL*				other_tbl
 *	@param		O: UINT32*							bit_mask
 *	@param		I: T_IIP_RES_USER					user_info
 *	@return		Success: D_IIP_ERR_OK / Failure: D_IIP_ERR_NG
 *	@note		None
 *	@attention	None
 */
static INT32 iip_res_lock_other( UINT32* other_cnt, T_IIP_RES_OTHER_TBL* other_tbl, UINT32* bit_mask, T_IIP_RES_USER user_info  )
{
	// LUT_A
	if ( (*other_cnt & D_IIP_RES_OTHER_LUT_A) != 0 && other_tbl->lut_a.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		other_tbl->lut_a.stat = D_IIP_RES_CONFIG_STS_USING;
		other_tbl->lut_a.user = user_info;
		*bit_mask |= D_IIP_RES_OTHER_LUT_A;
		*other_cnt &= ~D_IIP_RES_OTHER_LUT_A;
	}
	// LUT_B
	if ( (*other_cnt & D_IIP_RES_OTHER_LUT_B) != 0 && other_tbl->lut_b.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		other_tbl->lut_b.stat = D_IIP_RES_CONFIG_STS_USING;
		other_tbl->lut_b.user = user_info;
		*bit_mask |= D_IIP_RES_OTHER_LUT_B;
		*other_cnt &= ~D_IIP_RES_OTHER_LUT_B;
	}
	// LUT_C
	if ( (*other_cnt & D_IIP_RES_OTHER_LUT_C) != 0 && other_tbl->lut_c.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		other_tbl->lut_c.stat = D_IIP_RES_CONFIG_STS_USING;
		other_tbl->lut_c.user = user_info;
		*bit_mask |= D_IIP_RES_OTHER_LUT_C;
		*other_cnt &= ~D_IIP_RES_OTHER_LUT_C;
	}
	// LUT_D
	if ( (*other_cnt & D_IIP_RES_OTHER_LUT_D) != 0 && other_tbl->lut_d.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		other_tbl->lut_d.stat = D_IIP_RES_CONFIG_STS_USING;
		other_tbl->lut_d.user = user_info;
		*bit_mask |= D_IIP_RES_OTHER_LUT_D;
		*other_cnt &= ~D_IIP_RES_OTHER_LUT_D;
	}
	// LUT_E
	if ( (*other_cnt & D_IIP_RES_OTHER_LUT_E) != 0 && other_tbl->lut_e.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		other_tbl->lut_e.stat = D_IIP_RES_CONFIG_STS_USING;
		other_tbl->lut_e.user = user_info;
		*bit_mask |= D_IIP_RES_OTHER_LUT_E;
		*other_cnt &= ~D_IIP_RES_OTHER_LUT_E;
	}
	// LUT_F
	if ( (*other_cnt & D_IIP_RES_OTHER_LUT_F) != 0 && other_tbl->lut_f.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		other_tbl->lut_f.stat = D_IIP_RES_CONFIG_STS_USING;
		other_tbl->lut_f.user = user_info;
		*bit_mask |= D_IIP_RES_OTHER_LUT_F;
		*other_cnt &= ~D_IIP_RES_OTHER_LUT_F;
	}
	// HIT_0
	if ( (*other_cnt & D_IIP_RES_OTHER_HIST_0) != 0 && other_tbl->hist_0.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		other_tbl->hist_0.stat = D_IIP_RES_CONFIG_STS_USING;
		other_tbl->hist_0.user = user_info;
		*bit_mask |= D_IIP_RES_OTHER_HIST_0;
		*other_cnt &= ~D_IIP_RES_OTHER_HIST_0;
	}
	// HIT_1
	if ( (*other_cnt & D_IIP_RES_OTHER_HIST_1) != 0 && other_tbl->hist_1.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		other_tbl->hist_1.stat = D_IIP_RES_CONFIG_STS_USING;
		other_tbl->hist_1.user = user_info;
		*bit_mask |= D_IIP_RES_OTHER_HIST_1;
		*other_cnt &= ~D_IIP_RES_OTHER_HIST_1;
	}
	// HIT_2
	if ( (*other_cnt & D_IIP_RES_OTHER_HIST_2) != 0 && other_tbl->hist_2.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		other_tbl->hist_2.stat = D_IIP_RES_CONFIG_STS_USING;
		other_tbl->hist_2.user = user_info;
		*bit_mask |= D_IIP_RES_OTHER_HIST_2;
		*other_cnt &= ~D_IIP_RES_OTHER_HIST_2;
	}
	// HIT_3
	if ( (*other_cnt & D_IIP_RES_OTHER_HIST_3) != 0 && other_tbl->hist_3.stat == D_IIP_RES_CONFIG_STS_FREE ) {
		other_tbl->hist_3.stat = D_IIP_RES_CONFIG_STS_USING;
		other_tbl->hist_3.user = user_info;
		*bit_mask |= D_IIP_RES_OTHER_HIST_3;
		*other_cnt &= ~D_IIP_RES_OTHER_HIST_3;
	}

	// BI_CUBIC_0_OR_1
	if ( (*other_cnt & (D_IIP_RES_OTHER_BICUBIC_0 | D_IIP_RES_OTHER_BICUBIC_1 | D_IIP_RES_OTHER_BICUBIC_0_OR_1)) == D_IIP_RES_OTHER_BICUBIC_0_OR_1 ) {
		// BI_CUBIC_0
		if ( other_tbl->bi_cubic_0.stat == D_IIP_RES_CONFIG_STS_FREE ) {
			other_tbl->bi_cubic_0.stat = D_IIP_RES_CONFIG_STS_USING;
			other_tbl->bi_cubic_0.user = user_info;
			*bit_mask |= D_IIP_RES_OTHER_BICUBIC_0;
			*other_cnt &= ~D_IIP_RES_OTHER_BICUBIC_0_OR_1;
		}
		// BI_CUBIC_1
		else if ( other_tbl->bi_cubic_1.stat == D_IIP_RES_CONFIG_STS_FREE ) {
			other_tbl->bi_cubic_1.stat = D_IIP_RES_CONFIG_STS_USING;
			other_tbl->bi_cubic_1.user = user_info;
			*bit_mask |= D_IIP_RES_OTHER_BICUBIC_1;
			*other_cnt &= ~D_IIP_RES_OTHER_BICUBIC_0_OR_1;
		}
	}
	else {
		// BI_CUBIC_0
		if ( (*other_cnt & D_IIP_RES_OTHER_BICUBIC_0) != 0 && other_tbl->bi_cubic_0.stat == D_IIP_RES_CONFIG_STS_FREE ) {
			other_tbl->bi_cubic_0.stat = D_IIP_RES_CONFIG_STS_USING;
			other_tbl->bi_cubic_0.user = user_info;
			*bit_mask |= D_IIP_RES_OTHER_BICUBIC_0;
			*other_cnt &= ~D_IIP_RES_OTHER_BICUBIC_0;
		}
		// BI_CUBIC_1
		if ( (*other_cnt & D_IIP_RES_OTHER_BICUBIC_1) != 0 && other_tbl->bi_cubic_1.stat == D_IIP_RES_CONFIG_STS_FREE ) {
			other_tbl->bi_cubic_1.stat = D_IIP_RES_CONFIG_STS_USING;
			other_tbl->bi_cubic_1.user = user_info;
			*bit_mask |= D_IIP_RES_OTHER_BICUBIC_1;
			*other_cnt &= ~D_IIP_RES_OTHER_BICUBIC_1;
		}
		// BI_CUBIC_0 or BI_CUBIC_1 (Clear Only)
		if ( (*other_cnt & D_IIP_RES_OTHER_BICUBIC_0_OR_1) != 0 ) {
			*other_cnt &= ~D_IIP_RES_OTHER_BICUBIC_0_OR_1;
		}
	}

	if ( *other_cnt != D_IIP_RES_OTHER_NONE ) {
		return D_IIP_ERR_NG;
	}

	return D_IIP_ERR_OK;
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief	Lock the IIP resource.
 * @param	O: T_IIP_RES_CONFIG_OUT		config_out[]
 * @param	I: T_IIP_RES_CONFIG_ARG		config_arg[]
 * @param	I: UCHAR					config_num
 * @param	I: INT32					wait_time
 * @param	I: T_IIP_RES_USER_INFO*		user_info
 * @return	Success: D_IIP_ERR_OK / Failure: D_IIP_ERR_NG / Timeout: D_IIP_ERR_TMOUT
 * @note	None
*/
INT32 IIP_Res_Config_Lock( T_IIP_RES_CONFIG_OUT config_out[], const T_IIP_RES_CONFIG_ARG config_arg[], const UCHAR config_num, const INT32 wait_time, const T_IIP_RES_USER* user_info )
{
	INT32	ret = D_IIP_ERR_OK;
	UCHAR	loop;
	T_IIP_RES_CONFIG_TBL	conf_tbl;
	T_IIP_RES_CONFIG_ARG	conf_arg;
	ULLONG	unit_bit_msk = 0;
	UINT32	pixid_bit_msk = 0;
	UINT32	other_bit_msk = 0;
	ULLONG	base_tm = 0;
	ULLONG	elapsed_tm1 = 0;
	ULLONG	elapsed_tm2 = 0;
	UINT32	remain_time = wait_time;
	UINT32	success_cnt = 0;

	// Get Base time
	IIP_Res_Custom_Get_Tim(&base_tm);

	while (1) {

		// Get semaphore
		ret = IIP_Res_Custom_Open( remain_time );
		if ( ret != D_IIP_ERR_OK ) {
			return ret;
		}

		// Get Resource Mng table
		conf_tbl = gIIP_Config_Tbl;

		for (loop = 0, success_cnt = 0; loop < config_num; loop++) {

			// init variable
			unit_bit_msk = 0;
			pixid_bit_msk = 0;
			other_bit_msk = 0;
			conf_arg = config_arg[loop];

			while ( 1 ) {

				// Get unit resource
				ret = iip_res_lock_unit( &conf_arg.unit, &conf_tbl.unit, &unit_bit_msk, *user_info );
				if ( ret != D_IIP_ERR_OK ) {
					break;
				}

				if ( conf_arg.pixid != 0 ) {
					// Get pixid resource
					ret = iip_res_lock_pixid( &conf_arg.pixid, &conf_tbl.pixid, &pixid_bit_msk, *user_info );
					if ( ret != D_IIP_ERR_OK ) {
						break;
					}
				}

				if ( conf_arg.other != D_IIP_RES_OTHER_NONE ) {
					// Get other resource
					ret = iip_res_lock_other( &conf_arg.other, &conf_tbl.other, &other_bit_msk, *user_info );
					if ( ret != D_IIP_ERR_OK ) {
						break;
					}
				}

				config_out[loop].unitid_bitmask64 = unit_bit_msk;
				config_out[loop].pixid_bitmask = pixid_bit_msk;
				config_out[loop].other_bitmask = other_bit_msk;
				success_cnt++;
				break;
			}
		}

		if ( success_cnt == config_num ) {
		// Success root
			// Set Resource Mng table
			gIIP_Config_Tbl = conf_tbl;
			// Release semaphore
			IIP_Res_Custom_Close();
			break;
		}

		// Release semaphore
		IIP_Res_Custom_Close();

		IIP_Res_Custom_Get_Tim(&elapsed_tm1);

		if ( wait_time > (elapsed_tm1 - base_tm)) {
			remain_time = wait_time - (elapsed_tm1 - base_tm);
		}
		else {
#ifdef D_D_IIP_RES_DEBUG
			printf("TMOUT root1: wait_time[%d] elapsed_tm[%d]\n",wait_time, (UINT32)(elapsed_tm1 - base_tm));
#endif
			ret = D_IIP_ERR_TMOUT;
			break;
		}

		// Wait Flag
		ret = iip_res_wai_flg( remain_time );
		if ( ret != D_IIP_ERR_OK) {
#ifdef D_D_IIP_RES_DEBUG
			printf("TMOUT root2: twai timeout\n"));
#endif
			break;
		}

		IIP_Res_Custom_Get_Tim(&elapsed_tm2);

		if ( wait_time > (elapsed_tm2 - base_tm)) {
			remain_time = wait_time - (elapsed_tm2 - base_tm);
		}
		else {
#ifdef D_D_IIP_RES_DEBUG
			printf("TMOUT root3: wait_time[%d] elapsed_tm[%d]\n",wait_time, (UINT32)(elapsed_tm2 - base_tm));
#endif
			ret = D_IIP_ERR_TMOUT;
			break;
		}
#ifdef D_D_IIP_RES_DEBUG
		printf("Loop\n");
#endif
	}

	if ( ret != D_IIP_ERR_OK ) {
		// initialize
		for (loop = 0; loop < config_num; loop++) {
			config_out[loop].unitid_bitmask64 = 0;
			config_out[loop].pixid_bitmask = 0;
			config_out[loop].other_bitmask = 0;
		}
	}

	return ret;
}

/**
 * @brief	Release the IIP resource.
 * @param	I: T_IIP_RES_CONFIG_OUT		config_out[]
 * @param	I: UCHAR					config_num
 * @param	I: INT32					wait_time
 * @return	Success: D_IIP_ERR_OK / Failure: D_IIP_ERR_NG / Timeout: D_IIP_ERR_TMOUT
 * @note	None
*/
INT32 IIP_Res_Config_Free( const T_IIP_RES_CONFIG_OUT config_out[], const UCHAR config_num, const INT32 wait_time )
{
	UINT32	i;
	INT32	ret;
	UCHAR	loop;
	T_IIP_RES_CONFIG_TBL	conf_tbl;
	T_IIP_RES_INFO			init_res_info = { D_IIP_RES_CONFIG_STS_FREE, { 0, 0 }};

	// Get semaphore
	ret = IIP_Res_Custom_Open( wait_time );
	if ( ret != D_IIP_ERR_OK ) {
		return ret;
	}

	conf_tbl = gIIP_Config_Tbl;

	for (loop = 0; loop < config_num; loop++) {

		// UNIT
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_1D0 ) != 0  ) {
			conf_tbl.unit.oned0 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_1D1 ) != 0  ) {
			conf_tbl.unit.oned1 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_1D2 ) != 0  ) {
			conf_tbl.unit.oned2 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_1D3 ) != 0  ) {
			conf_tbl.unit.oned3 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_1D4 ) != 0  ) {
			conf_tbl.unit.oned4 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_1D5 ) != 0  ) {
			conf_tbl.unit.oned5 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_1D6 ) != 0  ) {
			conf_tbl.unit.oned6 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_1D7 ) != 0  ) {
			conf_tbl.unit.oned7 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_FLT ) != 0  ) {
			conf_tbl.unit.flt = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_AFN0 ) != 0  ) {
			conf_tbl.unit.afn0 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_AFN1 ) != 0  ) {
			conf_tbl.unit.afn1 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_AFN2 ) != 0  ) {
			conf_tbl.unit.afn2 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_AFN3 ) != 0  ) {
			conf_tbl.unit.afn3 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_FRECT0 ) != 0  ) {
			conf_tbl.unit.frect0 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_FRECT1 ) != 0  ) {
			conf_tbl.unit.frect1 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_MON ) != 0  ) {
			conf_tbl.unit.mon = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_CSC0 ) != 0  ) {
			conf_tbl.unit.csc0 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_CSC1 ) != 0  ) {
			conf_tbl.unit.csc1 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_CSC2 ) != 0  ) {
			conf_tbl.unit.csc2 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_CSC3 ) != 0  ) {
			conf_tbl.unit.csc3 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_LUT ) != 0  ) {
			conf_tbl.unit.lut = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_GPC ) != 0  ) {
			conf_tbl.unit.gpc = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_BLEND0 ) != 0  ) {
			conf_tbl.unit.blend0 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_BLEND1 ) != 0  ) {
			conf_tbl.unit.blend1 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_CFL0 ) != 0  ) {
			conf_tbl.unit.cfl0 = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_SL0 ) != 0  ) {
			conf_tbl.unit.sl[0] = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_SL1 ) != 0  ) {
			conf_tbl.unit.sl[1] = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_SL2 ) != 0  ) {
			conf_tbl.unit.sl[2] = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_SL3 ) != 0  ) {
			conf_tbl.unit.sl[3] = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_SL4 ) != 0  ) {
			conf_tbl.unit.sl[4] = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_SL5 ) != 0  ) {
			conf_tbl.unit.sl[5] = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_SL6 ) != 0  ) {
			conf_tbl.unit.sl[6] = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_SL7 ) != 0  ) {
			conf_tbl.unit.sl[7] = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_SL8 ) != 0  ) {
			conf_tbl.unit.sl[8] = init_res_info;
		}
		if ( (config_out[loop].unitid_bitmask64 & D_IIP_RES_UNIT_ID_MFT ) != 0  ) {
			conf_tbl.unit.mft = init_res_info;
		}

		// PIXFMTID
		for ( i = 0; i < D_IIP_RES_PIXID_MAXNUM; i++ ) {
			if ( (config_out[loop].pixid_bitmask & (1 << i)) != 0 ) {
				conf_tbl.pixid.pixid[i] = init_res_info;
			} 
		}

		// OTHER
		if ( (config_out[loop].other_bitmask & D_IIP_RES_OTHER_LUT_A ) != 0  ) {
			conf_tbl.other.lut_a = init_res_info;
		}
		if ( (config_out[loop].other_bitmask & D_IIP_RES_OTHER_LUT_B ) != 0  ) {
			conf_tbl.other.lut_b = init_res_info;
		}
		if ( (config_out[loop].other_bitmask & D_IIP_RES_OTHER_LUT_C ) != 0  ) {
			conf_tbl.other.lut_c = init_res_info;
		}
		if ( (config_out[loop].other_bitmask & D_IIP_RES_OTHER_LUT_D ) != 0  ) {
			conf_tbl.other.lut_d = init_res_info;
		}
		if ( (config_out[loop].other_bitmask & D_IIP_RES_OTHER_LUT_E ) != 0  ) {
			conf_tbl.other.lut_e = init_res_info;
		}
		if ( (config_out[loop].other_bitmask & D_IIP_RES_OTHER_LUT_F ) != 0  ) {
			conf_tbl.other.lut_f = init_res_info;
		}
		if ( (config_out[loop].other_bitmask & D_IIP_RES_OTHER_HIST_0 ) != 0  ) {
			conf_tbl.other.hist_0 = init_res_info;
		}
		if ( (config_out[loop].other_bitmask & D_IIP_RES_OTHER_HIST_1 ) != 0  ) {
			conf_tbl.other.hist_1 = init_res_info;
		}
		if ( (config_out[loop].other_bitmask & D_IIP_RES_OTHER_HIST_2 ) != 0  ) {
			conf_tbl.other.hist_2 = init_res_info;
		}
		if ( (config_out[loop].other_bitmask & D_IIP_RES_OTHER_HIST_3 ) != 0  ) {
			conf_tbl.other.hist_3 = init_res_info;
		}
		if ( (config_out[loop].other_bitmask & D_IIP_RES_OTHER_BICUBIC_0 ) != 0  ) {
			conf_tbl.other.bi_cubic_0 = init_res_info;
		}
		if ( (config_out[loop].other_bitmask & D_IIP_RES_OTHER_BICUBIC_1 ) != 0  ) {
			conf_tbl.other.bi_cubic_1 = init_res_info;
		}
	}

	gIIP_Config_Tbl = conf_tbl;

	// Release semaphore
	IIP_Res_Custom_Close();

	IIP_Res_Custom_Set_Flg( D_IIP_RES_SETFLG_PTN );

	return D_IIP_ERR_OK;
}



/**
 *	@brief		Get the IIP resource management table. ( for Rewrite table version)
 *	@param		T_IIP_RES_CALLBACK	callback_func
 *	@param		VOID*				callback_arg_pt
 *	@param		INT32				wait_time
 *	@return		Success: D_IIP_ERR_OK / Failure: D_IIP_ERR_NG / Timeout: D_IIP_ERR_TMOUT
 *	@note		None
 *	@attention	None
 */
INT32 IIP_Res_Config( const T_IIP_RES_CALLBACK callback_func, VOID* callback_arg_pt, const INT32 wait_time )
{
	INT32 ret;

	// Get semaphore
	ret = IIP_Res_Custom_Open( wait_time );
	if ( ret != D_IIP_ERR_OK ) {
		return ret;
	}

	// Call back function
	callback_func( callback_arg_pt, &gIIP_Config_Tbl );

	// Release semaphore
	IIP_Res_Custom_Close();

	return D_IIP_ERR_OK;
}

/**
 *	@brief		Print out the resource table values for debug
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID IIP_Res_DebugPrint( VOID )
{
	T_IIP_RES_CONFIG_TBL conf_tbl;
	UINT32 i;

	memcpy( &conf_tbl, &gIIP_Config_Tbl, sizeof(T_IIP_RES_CONFIG_TBL) );

	// UNIT
	printf("UNIT  [oned0 ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.oned0.stat,	conf_tbl.unit.oned0.user.tskid,		conf_tbl.unit.oned0.user.layer );
	printf("UNIT  [oned1 ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.oned1.stat,	conf_tbl.unit.oned1.user.tskid,		conf_tbl.unit.oned1.user.layer );
	printf("UNIT  [oned2 ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.oned2.stat,	conf_tbl.unit.oned2.user.tskid,		conf_tbl.unit.oned2.user.layer );
	printf("UNIT  [oned3 ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.oned3.stat,	conf_tbl.unit.oned3.user.tskid,		conf_tbl.unit.oned3.user.layer );
	printf("UNIT  [oned4 ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.oned4.stat,	conf_tbl.unit.oned4.user.tskid,		conf_tbl.unit.oned4.user.layer );
	printf("UNIT  [oned5 ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.oned5.stat,	conf_tbl.unit.oned5.user.tskid,		conf_tbl.unit.oned5.user.layer );
	printf("UNIT  [oned6 ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.oned6.stat,	conf_tbl.unit.oned6.user.tskid,		conf_tbl.unit.oned6.user.layer );
	printf("UNIT  [oned7 ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.oned7.stat,	conf_tbl.unit.oned7.user.tskid,		conf_tbl.unit.oned7.user.layer );
	printf("UNIT  [flt   ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.flt.stat,		conf_tbl.unit.flt.user.tskid,		conf_tbl.unit.flt.user.layer );
	printf("UNIT  [afn0  ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.afn0.stat,		conf_tbl.unit.afn0.user.tskid,		conf_tbl.unit.afn0.user.layer );
	printf("UNIT  [afn1  ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.afn1.stat,		conf_tbl.unit.afn1.user.tskid,		conf_tbl.unit.afn1.user.layer );
	printf("UNIT  [afn2  ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.afn2.stat,		conf_tbl.unit.afn2.user.tskid,		conf_tbl.unit.afn2.user.layer );
	printf("UNIT  [afn3  ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.afn3.stat,		conf_tbl.unit.afn3.user.tskid,		conf_tbl.unit.afn3.user.layer );
	printf("UNIT  [frect0] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.frect0.stat,	conf_tbl.unit.frect0.user.tskid,	conf_tbl.unit.frect0.user.layer );
	printf("UNIT  [frect1] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.frect1.stat,	conf_tbl.unit.frect1.user.tskid,	conf_tbl.unit.frect1.user.layer );
	printf("UNIT  [mon   ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.mon.stat,		conf_tbl.unit.mon.user.tskid,		conf_tbl.unit.mon.user.layer );
	printf("UNIT  [csc0  ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.csc0.stat,		conf_tbl.unit.csc0.user.tskid,		conf_tbl.unit.csc0.user.layer );
	printf("UNIT  [csc1  ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.csc1.stat,		conf_tbl.unit.csc1.user.tskid,		conf_tbl.unit.csc1.user.layer );
	printf("UNIT  [csc2  ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.csc2.stat,		conf_tbl.unit.csc2.user.tskid,		conf_tbl.unit.csc2.user.layer );
	printf("UNIT  [csc3  ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.csc3.stat,		conf_tbl.unit.csc3.user.tskid,		conf_tbl.unit.csc3.user.layer );
	printf("UNIT  [lut   ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.lut.stat,		conf_tbl.unit.lut.user.tskid,		conf_tbl.unit.lut.user.layer );
	printf("UNIT  [gpc   ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.gpc.stat,		conf_tbl.unit.gpc.user.tskid,		conf_tbl.unit.gpc.user.layer );
	printf("UNIT  [blend0] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.blend0.stat,	conf_tbl.unit.blend0.user.tskid,		conf_tbl.unit.blend0.user.layer );
	printf("UNIT  [blend1] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.blend1.stat,	conf_tbl.unit.blend1.user.tskid,		conf_tbl.unit.blend1.user.layer );
	printf("UNIT  [cfl0  ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.cfl0.stat,		conf_tbl.unit.cfl0.user.tskid,		conf_tbl.unit.cfl0.user.layer );
	printf("UNIT  [sl0   ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.sl[0].stat,		conf_tbl.unit.sl[0].user.tskid,		conf_tbl.unit.sl[0].user.layer );
	printf("UNIT  [sl1   ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.sl[1].stat,		conf_tbl.unit.sl[1].user.tskid,		conf_tbl.unit.sl[1].user.layer );
	printf("UNIT  [sl2   ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.sl[2].stat,		conf_tbl.unit.sl[2].user.tskid,		conf_tbl.unit.sl[2].user.layer );
	printf("UNIT  [sl3   ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.sl[3].stat,		conf_tbl.unit.sl[3].user.tskid,		conf_tbl.unit.sl[3].user.layer );
	printf("UNIT  [sl4   ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.sl[4].stat,		conf_tbl.unit.sl[4].user.tskid,		conf_tbl.unit.sl[4].user.layer );
	printf("UNIT  [sl5   ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.sl[5].stat,		conf_tbl.unit.sl[5].user.tskid,		conf_tbl.unit.sl[5].user.layer );
	printf("UNIT  [sl6   ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.sl[6].stat,		conf_tbl.unit.sl[6].user.tskid,		conf_tbl.unit.sl[6].user.layer );
	printf("UNIT  [sl7   ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.sl[7].stat,		conf_tbl.unit.sl[7].user.tskid,		conf_tbl.unit.sl[7].user.layer );
	printf("UNIT  [sl8   ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.sl[8].stat,		conf_tbl.unit.sl[8].user.tskid,		conf_tbl.unit.sl[8].user.layer );
	printf("UNIT  [mft   ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.unit.mft.stat,		conf_tbl.unit.mft.user.tskid,		conf_tbl.unit.mft.user.layer );

	// PIXFMTID
	for ( i = 0; i < D_IIP_RES_PIXID_MAXNUM; i++ ) {
		printf("PIXFMT[ID%d  ] >> status[%d] tskid[%d] layer[%d]\n", i, conf_tbl.pixid.pixid[i].stat,	conf_tbl.pixid.pixid[i].user.tskid,	conf_tbl.pixid.pixid[i].user.layer );
	}

	// OTHER
	printf("OTHER [lut_a ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.other.lut_a.stat,				conf_tbl.other.lut_a.user.tskid,		conf_tbl.other.lut_a.user.layer );
	printf("OTHER [lut_b ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.other.lut_b.stat,				conf_tbl.other.lut_b.user.tskid,		conf_tbl.other.lut_b.user.layer );
	printf("OTHER [lut_c ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.other.lut_c.stat,				conf_tbl.other.lut_c.user.tskid,		conf_tbl.other.lut_c.user.layer );
	printf("OTHER [lut_d ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.other.lut_d.stat,				conf_tbl.other.lut_d.user.tskid,		conf_tbl.other.lut_d.user.layer );
	printf("OTHER [lut_e ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.other.lut_e.stat,				conf_tbl.other.lut_e.user.tskid,		conf_tbl.other.lut_e.user.layer );
	printf("OTHER [lut_f ] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.other.lut_f.stat,				conf_tbl.other.lut_f.user.tskid,		conf_tbl.other.lut_f.user.layer );
	printf("OTHER [hist_0] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.other.hist_0.stat,			conf_tbl.other.hist_0.user.tskid,		conf_tbl.other.hist_0.user.layer );
	printf("OTHER [hist_1] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.other.hist_1.stat,			conf_tbl.other.hist_1.user.tskid,		conf_tbl.other.hist_1.user.layer );
	printf("OTHER [hist_2] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.other.hist_2.stat,			conf_tbl.other.hist_2.user.tskid,		conf_tbl.other.hist_2.user.layer );
	printf("OTHER [hist_3] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.other.hist_3.stat,			conf_tbl.other.hist_3.user.tskid,		conf_tbl.other.hist_3.user.layer );
	printf("OTHER [bi_cubic_0] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.other.bi_cubic_0.stat,	conf_tbl.other.bi_cubic_0.user.tskid,	conf_tbl.other.bi_cubic_0.user.layer );
	printf("OTHER [bi_cubic_1] >> status[%d] tskid[%d] layer[%d]\n", conf_tbl.other.bi_cubic_1.stat,	conf_tbl.other.bi_cubic_1.user.tskid,	conf_tbl.other.bi_cubic_1.user.layer );

	return;
}
