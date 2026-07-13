/**
 * @file		ct_im_bmh.c
 * @brief		BMH ct code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "ct_im_bmh.h"
#include "im_bmh.h"
#include "dd_top.h"
#include "driver_common.h"
#include "ddim_user_custom.h"
#include "jmlbmh.h"
#include <string.h>
#include <stdlib.h>


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_IM_BMH_TEST_CUR_IMG_ADDR	(0x7C000000)
#define D_IM_BMH_TEST_REF_IMG_ADDR	(0x7D000000)

#define D_IM_BMH_TEST_IMG_HSIZE		(320)
#define D_IM_BMH_TEST_IMG_VSIZE		(240)

#define D_IM_BMH_TEST_RESULT_OUT_ADDR	(0x7E000000)


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// nothing special.

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// nothing special.

/*----------------------------------------------------------------------*/
/* Local Method															*/
/*----------------------------------------------------------------------*/
#ifdef CO_MSG_PRINT_ON
#define D_IM_BMH_RESULT(statement)		result = statement
#else
#define D_IM_BMH_RESULT
#endif

static VOID bmh_callback_output_result( ULONG interrupt_flag_mask );

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static T_IM_BMH_CTRL gct_im_bmh_ctrl = {
	.lselcnt = 0,
	.rbs = 0,
	.pshift = 0,
	.outmd = 0,
	.lutmd = 0,
	.offmd = 0,
	.mode = 1,
	.poffset = 0,
	.sadlv = 0,
	.pCallBack = bmh_callback_output_result,
};

static VOID ct_im_bmh_print_debug_reg( VOID )
{
	UCHAR loopcnt;
	for( loopcnt = 0; loopcnt < 2; loopcnt++ ){
		Ddim_Print(( "IO_BMH[%d]\n",                  loopcnt ));
		Ddim_Print(( "\tBMHSR-SR             = 0x%x\n",  IO_BMH[loopcnt].BMHSR.bit.SR ));
		Ddim_Print(( "\tBMHTRG-BMHTRG        = 0x%x\n",  IO_BMH[loopcnt].BMHTRG.bit.BMHTRG ));
		Ddim_Print(( "\tBMHRE-BMHRE          = 0x%x\n",  IO_BMH[loopcnt].BMHRE.bit.BMHRE ));
		Ddim_Print(( "\tBMHCONT-CONT         = 0x%x\n",  IO_BMH[loopcnt].BMHCONT.bit.CONT ));
		Ddim_Print(( "\tBMHINTFS-__RSST      = 0x%x\n",  IO_BMH[loopcnt].BMHINTFS.bit.__RSST ));
		Ddim_Print(( "\tBMHINTFS-__AXIF      = 0x%x\n",  IO_BMH[loopcnt].BMHINTFS.bit.__AXIF ));
		Ddim_Print(( "\tBMHINTFS-__BMHF      = 0x%x\n",  IO_BMH[loopcnt].BMHINTFS.bit.__BMHF ));
		Ddim_Print(( "\tBMHINTE-AXIE         = 0x%x\n",  IO_BMH[loopcnt].BMHINTE.bit.AXIE ));
		Ddim_Print(( "\tBMHINTE-BMHE         = 0x%x\n",  IO_BMH[loopcnt].BMHINTE.bit.BMHE ));
		Ddim_Print(( "\tBMHMD-MODE           = 0x%x\n",  IO_BMH[loopcnt].BMHMD.bit.MODE ));
		Ddim_Print(( "\tBMHMD-OFFMD          = 0x%x\n",  IO_BMH[loopcnt].BMHMD.bit.OFFMD ));
		Ddim_Print(( "\tBMHMD-LUTMD          = 0x%x\n",  IO_BMH[loopcnt].BMHMD.bit.LUTMD ));
		Ddim_Print(( "\tBMHMD-OUTMD          = 0x%x\n",  IO_BMH[loopcnt].BMHMD.bit.OUTMD ));
		Ddim_Print(( "\tBMHMD-PSHIFT         = 0x%x\n",  IO_BMH[loopcnt].BMHMD.bit.PSHIFT ));
		Ddim_Print(( "\tBMHMD-RBS            = 0x%x\n",  IO_BMH[loopcnt].BMHMD.bit.RBS ));
		Ddim_Print(( "\tBMHMD-LSELCNT        = 0x%x\n",  IO_BMH[loopcnt].BMHMD.bit.LSELCNT ));
		Ddim_Print(( "\tBMHPRM-SADLV         = 0x%x\n",  IO_BMH[loopcnt].BMHPRM.bit.SADLV ));
		Ddim_Print(( "\tBMHPRM-POFFSET       = 0x%x\n",  IO_BMH[loopcnt].BMHPRM.bit.POFFSET ));
		Ddim_Print(( "\tBMHHCNT-RCNT         = 0x%x\n",  IO_BMH[loopcnt].BMHHCNT.bit.RCNT ));
		Ddim_Print(( "\tBMHHCNT-HCNT         = 0x%x\n",  IO_BMH[loopcnt].BMHHCNT.bit.HCNT ));
		Ddim_Print(( "\tBMHSAD_WR-SAD_WR     = 0x%x\n",  IO_BMH[loopcnt].BMHSAD_WR.bit.SAD_WR ));
		Ddim_Print(( "\tBMHDMA-DMAMD         = 0x%x\n",  IO_BMH[loopcnt].BMHDMA.bit.DMAMD ));
		Ddim_Print(( "\tBMHGHSIZE-GHSIZE_REF = 0x%x\n",  IO_BMH[loopcnt].BMHGHSIZE.bit.GHSIZE_REF ));
		Ddim_Print(( "\tBMHGHSIZE-GHSIZE_CUR = 0x%x\n",  IO_BMH[loopcnt].BMHGHSIZE.bit.GHSIZE_CUR ));
		Ddim_Print(( "\tBMHHSIZE-HSIZE_REF   = 0x%x\n",  IO_BMH[loopcnt].BMHHSIZE.bit.HSIZE_REF ));
		Ddim_Print(( "\tBMHVSIZE-VSIZE_REF   = 0x%x\n",  IO_BMH[loopcnt].BMHVSIZE.bit.VSIZE_REF ));
		Ddim_Print(( "\tBMHCURAD-CURSTAD     = 0x%lx\n", IO_BMH[loopcnt].BMHCURAD.bit.CURSTAD ));
		Ddim_Print(( "\tBMHREFAD-REFSTAD     = 0x%lx\n", IO_BMH[loopcnt].BMHREFAD.bit.REFSTAD ));
		Ddim_Print(( "\tBMHSADAD-SADSTAD     = 0x%lx\n", IO_BMH[loopcnt].BMHSADAD.bit.SADSTAD ));
		Ddim_Print(( "\tBMHSADGH-SADGHSIZE   = 0x%x\n",  IO_BMH[loopcnt].BMHSADGH.bit.SADGHSIZE ));
		Ddim_Print(( "\tBMHAXICTRL-ARCACHE   = 0x%x\n",  IO_BMH[loopcnt].BMHAXICTRL.bit.ARCACHE ));
		Ddim_Print(( "\tBMHAXICTRL-ARPROT    = 0x%x\n",  IO_BMH[loopcnt].BMHAXICTRL.bit.ARPROT ));
		Ddim_Print(( "\tBMHAXICTRL-AWCACHE   = 0x%x\n",  IO_BMH[loopcnt].BMHAXICTRL.bit.AWCACHE ));
		Ddim_Print(( "\tBMHAXICTRL-AWPROT    = 0x%x\n",  IO_BMH[loopcnt].BMHAXICTRL.bit.AWPROT ));
		Ddim_Print(( "\tBMHAXIERR-RRESP      = 0x%x\n",  IO_BMH[loopcnt].BMHAXIERR.bit.RRESP ));
		Ddim_Print(( "\tBMHAXIERR-WRESP      = 0x%x\n",  IO_BMH[loopcnt].BMHAXIERR.bit.WRESP ));
		Ddim_Print(( "\n" ));
	}
}

static VOID ct_im_bmh_print_debug_sadr( VOID )
{
	UCHAR loop0;
	ULONG loop1;
	ULONG loop2;

	for( loop0 = 0; loop0 < 2; loop0++ ){
		Ddim_Print(( "IO_BMH[%d]\n", loop0 ));
		for( loop1 = 0; loop1 < 2; loop1++ ){
			for( loop2 = 0; loop2 < 128; loop2++ ){
				Ddim_Print(( "[%lu][%lu] = 0x%lx\n", loop1, loop2, IO_BMH[loop0].SADR.word[loop1][loop2] ));
			}
		}
	}
}

VOID bmh_callback( ULONG interrupt_flag_mask )
{
	Ddim_Print(( "bmh_callback interrupt_flag_mask = 0x%lx\n", interrupt_flag_mask ));
}

static VOID bmh_callback_output_result( ULONG interrupt_flag_mask )
{
	Ddim_Print(( "bmh_callback_output_result interrupt_flag_mask = 0x%lx\n", interrupt_flag_mask ));
	ct_im_bmh_print_debug_reg();
}



/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/

#ifdef CO_BMH_DEBUG
extern VOID Im_BMH_Print_ClockStatus( VOID );
#endif

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_1: "
INT32 CT_Im_Bmh_1_1( BYTE channel_no )
{
	INT32 im_ercd;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open0 error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_BMH_Open( channel_no, 100 );
	if( im_ercd != D_IM_BMH_SEM_TIMEOUT_ERR ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open1 error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_2: "
INT32 CT_Im_Bmh_1_2( BYTE channel_no )
{
	INT32 im_ercd;
	DDIM_USER_ID sid_back;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	sid_back = SID_IM_BMH( channel_no );
	SID_IM_BMH( channel_no ) = 0xfff;

	im_ercd = Im_BMH_Open( channel_no, 100 );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		SID_IM_BMH( channel_no ) = sid_back;
		return im_ercd;
	}

	SID_IM_BMH( channel_no ) = sid_back;

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_3: "
INT32 CT_Im_Bmh_1_3( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 i, j;

	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;
	for( i = 0; i < 8; i++ ){
		for( j = 0; j < 32; j++ ){
			cur_write_data.cur_write_flg[i][j] = 1;
			cur_write_data.cur_data[i][j] = j + 2;
			ref_write_data.ref_write_flg[i][j] = 1;
			ref_write_data.ref_data[i][j] = j;
		}
	}

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 10 );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_4: "
INT32 CT_Im_Bmh_1_4( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 wait_factor;
	UINT32 i, j;
	T_IM_BMH_CTRL bmh_ctrl = {
		.lselcnt = 0,
		.rbs = 0,
		.pshift = 0,
		.outmd = 0,
		.lutmd = 0,
		.offmd = 0,
		.mode = 1,
		.poffset = 0,
		.sadlv = 0,
		.pCallBack = bmh_callback,
	};
	T_IM_BMH_BMHHCNT cnt = {
		.hcnt = 0,
		.rcnt = 0,
	};

	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;
	for( i = 0; i < 9; i++ ){
		for( j = 0; j < 256; j++ ){
			cur_write_data.cur_write_flg[i][j] = 1;
			cur_write_data.cur_data[i][j] = j;
			ref_write_data.ref_write_flg[i][j] = 1;
			ref_write_data.ref_data[i][j] = j / 2;
		}
	}

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_BMH_Init( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Init error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Interrupt( channel_no, D_IM_BMH_FLG_BMH_END | D_IM_BMH_FLG_BMH_AXI_ERR, 1 );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Interrupt error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Bmhhcnt( channel_no, &cnt );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Bmhhcnt error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Async( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Async error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
	IO_BMH[channel_no].BMHINTFS.bit.__BMHF = 1;
	Im_BMH_Int_Handler( channel_no );
#endif //CO_DEBUG_ON_PC

	im_ercd = Im_BMH_Wait_End( channel_no, &wait_factor, 1000 );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Wait End error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_5: "
INT32 CT_Im_Bmh_1_5( BYTE channel_no )
{
	INT32 im_ercd;
#ifdef CO_DEBUG_ON_PC
	UINT32 wait_factor;
#endif //CO_DEBUG_ON_PC

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

#ifdef CO_DEBUG_ON_PC
	im_ercd = Im_BMH_Open( channel_no, 100 );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Async( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Async error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Wait_End( channel_no, &wait_factor, 10 );
	if( im_ercd != D_IM_BMH_SEM_TIMEOUT_ERR ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Wait End error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
#else //!CO_DEBUG_ON_PC
	Ddim_Print(( "%s: Only debug on PC\n", D_IM_BMH_FUNC_NAME ));
	im_ercd = D_DDIM_SYSTEM_ERROR;
#endif //!CO_DEBUG_ON_PC

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_6: "
INT32 CT_Im_Bmh_1_6( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 wait_factor;
	DDIM_USER_ID fid_back;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Async( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Async error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	fid_back = FID_IM_BMH;
	FID_IM_BMH = 0xfff;
	im_ercd = Im_BMH_Wait_End( channel_no, &wait_factor, 10 );
	if( im_ercd != D_IM_BMH_SEM_NG ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Wait End error = 0x%x\n", im_ercd ));
		FID_IM_BMH = fid_back;
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	FID_IM_BMH = fid_back;

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_7: "
INT32 CT_Im_Bmh_1_7( BYTE channel_no )
{
	INT32 im_ercd;
	DDIM_USER_ID sid_back;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	sid_back = SID_IM_BMH( channel_no );
	SID_IM_BMH( channel_no ) = 0xfff;

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_IM_BMH_SEM_NG ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	SID_IM_BMH( channel_no ) = sid_back;

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_8: "
INT32 CT_Im_Bmh_1_8( BYTE channel_no )
{
	INT32 im_ercd;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_9: "
INT32 CT_Im_Bmh_1_9( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl[2] = {
		{	// min
			.lselcnt = 0,
			.rbs = 0,
			.pshift = 0,
			.outmd = 0,
			.lutmd = 0,
			.offmd = 0,
			.mode = 1,
			.poffset = 0,
			.sadlv = 0,
			.pCallBack = NULL,
		},
		{	// max
			.lselcnt = 8,
			.rbs = 7,
			.pshift = 4,
			.outmd = 6,
			.lutmd = 1,
			.offmd = 1,
			.mode = 15,
			.poffset = 0xFF,
			.sadlv = 0xFFFF,
			.pCallBack = NULL,
		},
	};

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	for( loop = 0; loop < 2; loop++ ){
		im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl[loop] );
		if( im_ercd != D_DDIM_OK ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_im_bmh_print_debug_reg();
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_10: "
INT32 CT_Im_Bmh_1_10( BYTE channel_no )
{
	INT32 im_ercd;

	T_IM_BMH_CTRL bmh_ctrl = {
		.lselcnt = 2,
		.rbs = 3,
		.pshift = 4,
		.outmd = 5,
		.lutmd = 1,
		.offmd = 1,
		.mode = 15,
		.poffset = 63,
		.sadlv = 0x7FFF,
		.pCallBack = bmh_callback,
	};
	T_IM_BMH_CTRL bmh_ctrl_get;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Im_BMH_Ctrl error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_BMH_Get_Ctrl( channel_no, &bmh_ctrl_get );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Im_BMH_Get_Ctrl error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
	ct_im_bmh_print_debug_reg();
	Ddim_Print(( D_IM_BMH_FUNC_NAME "bmh_ctrl_get.lselcnt   = 0x%x\n",  bmh_ctrl_get.lselcnt ));
	Ddim_Print(( D_IM_BMH_FUNC_NAME "bmh_ctrl_get.rbs       = 0x%x\n",  bmh_ctrl_get.rbs ));
	Ddim_Print(( D_IM_BMH_FUNC_NAME "bmh_ctrl_get.pshift    = 0x%x\n",  bmh_ctrl_get.pshift ));
	Ddim_Print(( D_IM_BMH_FUNC_NAME "bmh_ctrl_get.outmd     = 0x%x\n",  bmh_ctrl_get.outmd ));
	Ddim_Print(( D_IM_BMH_FUNC_NAME "bmh_ctrl_get.lutmd     = 0x%x\n",  bmh_ctrl_get.lutmd ));
	Ddim_Print(( D_IM_BMH_FUNC_NAME "bmh_ctrl_get.offmd     = 0x%x\n",  bmh_ctrl_get.offmd ));
	Ddim_Print(( D_IM_BMH_FUNC_NAME "bmh_ctrl_get.mode      = 0x%x\n",  bmh_ctrl_get.mode ));
	Ddim_Print(( D_IM_BMH_FUNC_NAME "bmh_ctrl_get.poffset   = 0x%lx\n", bmh_ctrl_get.poffset ));
	Ddim_Print(( D_IM_BMH_FUNC_NAME "bmh_ctrl_get.sadlv     = 0x%lx\n", bmh_ctrl_get.sadlv ));
	Ddim_Print(( D_IM_BMH_FUNC_NAME "bmh_ctrl_get.pCallBack = 0x%p\n",  bmh_ctrl_get.pCallBack ));

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_11: "
INT32 CT_Im_Bmh_1_11( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_BMHRE sram;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	for( loop = 0; loop < 2; loop++ ){
		sram.bmhre = loop;
		im_ercd = Im_BMH_Set_Bmhre( channel_no, &sram );
		if( im_ercd != D_DDIM_OK ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Bmhre error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_im_bmh_print_debug_reg();
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_12: "
INT32 CT_Im_Bmh_1_12( BYTE channel_no )
{
	INT32 im_ercd;

	T_IM_BMH_BMHRE sram;
	T_IM_BMH_BMHRE sram_get;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	sram.bmhre = 1;
	im_ercd = Im_BMH_Set_Bmhre( channel_no, &sram );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Bmhre error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
	im_ercd = Im_BMH_Get_Bmhre( channel_no, &sram_get );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Get Bmhre error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
	Ddim_Print(( D_IM_BMH_FUNC_NAME "sram_get.bmhre = 0x%x\n", sram_get.bmhre ));

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_13: "
INT32 CT_Im_Bmh_1_13( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;
	UINT32 wait_factor;
	UINT32 i, j;
	T_IM_BMH_CTRL bmh_ctrl = {
		.lselcnt = 0,
		.rbs = 0,
		.pshift = 0,
		.outmd = 0,
		.lutmd = 0,
		.offmd = 0,
		.mode = 1,
		.poffset = 0,
		.sadlv = 0,
		.pCallBack = bmh_callback,
	};
	T_IM_BMH_RSST rsst;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;
	for( i = 0; i < 9; i++ ){
		for( j = 0; j < 256; j++ ){
			cur_write_data.cur_write_flg[i][j] = 1;
			cur_write_data.cur_data[i][j] = j;
			ref_write_data.ref_write_flg[i][j] = 1;
			ref_write_data.ref_data[i][j] = j / 2;
		}
	}

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	for( loop = 0; loop < 2; loop++ ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "loop[%d]\n", loop ));
		im_ercd = Im_BMH_Open( channel_no, 100 );
		if( im_ercd != D_DDIM_OK ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
			return im_ercd;
		}

		im_ercd = Im_BMH_Init( channel_no );
		if( im_ercd != D_DDIM_OK ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "Init error = 0x%x\n", im_ercd ));
			Im_BMH_Close( channel_no );
			return im_ercd;
		}

		im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
		if( im_ercd != D_DDIM_OK ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
			Im_BMH_Close( channel_no );
			return im_ercd;
		}

		im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
		if( im_ercd != D_DDIM_OK ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Cur error = 0x%x\n", im_ercd ));
			Im_BMH_Close( channel_no );
			return im_ercd;
		}

		im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
		if( im_ercd != D_DDIM_OK ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Ref error = 0x%x\n", im_ercd ));
			Im_BMH_Close( channel_no );
			return im_ercd;
		}

		im_ercd = Im_BMH_Start_Async( channel_no );
		if( im_ercd != D_DDIM_OK ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Async error = 0x%x\n", im_ercd ));
			Im_BMH_Close( channel_no );
			return im_ercd;
		}

#ifdef CO_DEBUG_ON_PC
		IO_BMH[channel_no].BMHINTFS.bit.__BMHF = 1;
		IO_BMH[channel_no].BMHINTFS.bit.__RSST = ( IO_BMH[channel_no].BMHINTFS.bit.__RSST + 1 ) & 1;
		Im_BMH_Int_Handler( channel_no );
#endif //CO_DEBUG_ON_PC

		im_ercd = Im_BMH_Wait_End( channel_no, &wait_factor, 1000 );
		if( im_ercd != D_DDIM_OK ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "Wait End error = 0x%x\n", im_ercd ));
			Im_BMH_Close( channel_no );
			return im_ercd;
		}

		im_ercd = Im_BMH_Get_Rsst( channel_no, &rsst );
		if( im_ercd != D_DDIM_OK ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "Get Rsst error = 0x%x\n", im_ercd ));
			Im_BMH_Close( channel_no );
			return im_ercd;
		}
		Ddim_Print(( "rsst.rsst = %u\n", rsst.rsst ));

		im_ercd = Im_BMH_Close( channel_no );
		if( im_ercd != D_DDIM_OK ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_14: "
INT32 CT_Im_Bmh_1_14( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_BMHCONT cont;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	for( loop = 0; loop < 2; loop++ ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "loop[%d]\n", loop ));
		cont.cont = loop;
		im_ercd = Im_BMH_Set_Bmhcont( channel_no, &cont );
		if( im_ercd != D_DDIM_OK ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Bmhcont error = 0x%x\n", im_ercd ));
			return im_ercd;
		}

		ct_im_bmh_print_debug_reg();
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_15: "
INT32 CT_Im_Bmh_1_15( BYTE channel_no )
{
	INT32 im_ercd;

	T_IM_BMH_BMHCONT cont;
	T_IM_BMH_BMHCONT cont_get;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	cont.cont = 1;
	im_ercd = Im_BMH_Set_Bmhcont( channel_no, &cont );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Bmhcont error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_BMH_Get_Bmhcont( channel_no, &cont_get );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Get Bmhcont error = 0x%x\n", im_ercd ));
		return im_ercd;
	}
	Ddim_Print(( D_IM_BMH_FUNC_NAME "cont_get.cont = 0x%x\n", cont_get.cont ));

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_16: "
INT32 CT_Im_Bmh_1_16( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_AXI axi_ctrl[2] = {
		{	// min
			.read_cache_type = 0,
			.read_protection_type = 0,
			.write_cache_type = 0,
			.write_protection_type = 0,
		},
		{	// max
			.read_cache_type = 0xf,
			.read_protection_type = 0x7,
			.write_cache_type = 0xf,
			.write_protection_type = 0x7,
		},
	};

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	for( loop = 0; loop < 2; loop++ ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "loop[%d]\n", loop ));
		im_ercd = Im_BMH_Set_Bmhaxi( channel_no, &axi_ctrl[loop] );
		if( im_ercd != D_DDIM_OK ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Bmhaxi error = 0x%x\n", im_ercd ));
			return im_ercd;
		}
		ct_im_bmh_print_debug_reg();
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_17: "
INT32 CT_Im_Bmh_1_17( BYTE channel_no )
{
	INT32 im_ercd;

	T_IM_BMH_AXI_STATUS sts;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

#ifdef CO_DEBUG_ON_PC
	IO_BMH[channel_no].BMHAXIERR.bit.WRESP = 1;
	IO_BMH[channel_no].BMHAXIERR.bit.RRESP = 1;
#endif // CO_DEBUG_ON_PC

	im_ercd = Im_BMH_Get_Bmhaxierr( channel_no, &sts );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Get Bmhaxierr error = 0x%x\n", im_ercd ));
	}
	Ddim_Print(( D_IM_BMH_FUNC_NAME "sts.read_channel_response  = 0x%x\n", sts.read_channel_response ));
	Ddim_Print(( D_IM_BMH_FUNC_NAME "sts.write_channel_response = 0x%x\n", sts.write_channel_response ));

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_18: "
INT32 CT_Im_Bmh_1_18( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_BMHHCNT cnt[2] = {
		{	// min
			.hcnt = 0,
			.rcnt = 0,
		},
		{	// max
			.hcnt = 0xff,
			.rcnt = 0xf,
		},
	};

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	for( loop = 0; loop < 2; loop++ ){
		im_ercd = Im_BMH_Set_Bmhhcnt( channel_no, &cnt[loop] );
		if( im_ercd != D_DDIM_OK ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Bmhhcnt error = 0x%x\n", im_ercd ));
		}
		ct_im_bmh_print_debug_reg();
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_19: "
INT32 CT_Im_Bmh_1_19( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 i, j;

	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;
	for( i = 0; i < 8; i++ ){
		for( j = 0; j < 32; j++ ){
			cur_write_data.cur_write_flg[i][j] = 1;
			cur_write_data.cur_data[i][j] = j + 2;
			ref_write_data.ref_write_flg[i][j] = 1;
			ref_write_data.ref_data[i][j] = j;
		}
	}

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_BMH_Init( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Init error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
		IO_BMH[channel_no].BMHINTFS.bit.__BMHF = 1;
#endif //CO_DEBUG_ON_PC

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_20: "
INT32 CT_Im_Bmh_1_20( BYTE channel_no )
{
	INT32 im_ercd;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_21: "
INT32 CT_Im_Bmh_1_21( BYTE channel_no )
{
	INT32 im_ercd;

	im_ercd = CT_Im_Bmh_1_8( channel_no );

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_22: "
INT32 CT_Im_Bmh_1_22( BYTE channel_no )
{
	INT32 im_ercd;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Init( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Init error = 0x%x\n", im_ercd ));
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_23: "
INT32 CT_Im_Bmh_1_23( BYTE channel_no )
{
	INT32 im_ercd;

	T_IM_BMH_BMHHCNT cnt;
	T_IM_BMH_BMHHCNT cnt_get;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	cnt.hcnt = 1;
	cnt.rcnt = 1;
	im_ercd = Im_BMH_Set_Bmhhcnt( channel_no, &cnt );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Bmhhcnt error = 0x%x\n", im_ercd ));
	}
	im_ercd = Im_BMH_Get_Bmhhcnt( channel_no, &cnt_get );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Bmhhcnt error = 0x%x\n", im_ercd ));
	}
	Ddim_Print(( D_IM_BMH_FUNC_NAME "cnt_get.hcnt = 0x%x\n", cnt_get.hcnt ));
	Ddim_Print(( D_IM_BMH_FUNC_NAME "cnt_get.rcnt = 0x%x\n", cnt_get.rcnt ));

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_24: "
INT32 CT_Im_Bmh_1_24( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_BMHSAD_WR sad_wr[2] = {
		{	// min
			.sad_wr = 0,
		},
		{	// max
			.sad_wr = 0x1f,
		},
	};

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	for( loop = 0; loop < 2; loop++ ){
		im_ercd = Im_BMH_Set_Bmhsad_wr( channel_no, &sad_wr[loop] );
		if( im_ercd != D_DDIM_OK ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Bmhsad_wr error = 0x%x\n", im_ercd ));
		}
		ct_im_bmh_print_debug_reg();
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_25: "
INT32 CT_Im_Bmh_1_25( BYTE channel_no )
{
	INT32 im_ercd;

	T_IM_BMH_BMHSAD_WR sad_wr;
	T_IM_BMH_BMHSAD_WR sad_wr_get;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	sad_wr.sad_wr = 1;
	im_ercd = Im_BMH_Set_Bmhsad_wr( channel_no, &sad_wr );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Bmhsad_wr error = 0x%x\n", im_ercd ));
	}
	im_ercd = Im_BMH_Get_Bmhsad_wr( channel_no, &sad_wr_get );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Get Bmhsad_wr error = 0x%x\n", im_ercd ));
	}
	Ddim_Print(( D_IM_BMH_FUNC_NAME "sad_wr_get.sad_wr = 0x%x\n", sad_wr_get.sad_wr ));

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_26: "
INT32 CT_Im_Bmh_1_26( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL_DMA bmh_ctrl_dma[2] = {
		{	// min
			.dmamd = 0,
			.ghsize_cur = 16,
			.ghsize_ref = 32,
			.hsize_ref = 32,
			.vsize_ref = 8,
			.curstad = 0,
			.refstad = 0,
			.sadstad = 0,
			.sadghsize = 0,
		},
		{	// max
			.dmamd = 1,
			.ghsize_cur = 0xFFF8,
			.ghsize_ref = 0xFFF8,
			.hsize_ref = 0xFFFF,
			.vsize_ref = 0xFFFF,
			.curstad = 0xFFFFFFF8,
			.refstad = 0xFFFFFFF8,
			.sadstad = 0xFFFFFFF8,
			.sadghsize = 0xFFF8,
		},
	};

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	for( loop = 0; loop < 2; loop++ ){
		im_ercd = Im_BMH_Ctrl_Dma( channel_no, &bmh_ctrl_dma[loop] );
		if( im_ercd != D_DDIM_OK ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl Dma error = 0x%x\n", im_ercd ));
		}
		ct_im_bmh_print_debug_reg();
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_27: "
INT32 CT_Im_Bmh_1_27( BYTE channel_no )
{
	INT32 im_ercd;

	T_IM_BMH_CTRL_DMA bmh_ctrl_dma;
	T_IM_BMH_CTRL_DMA bmh_ctrl_dma_get;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	bmh_ctrl_dma.dmamd = 1;
	bmh_ctrl_dma.ghsize_cur = 0x10;
	bmh_ctrl_dma.ghsize_ref = 0x20;
	bmh_ctrl_dma.hsize_ref = 0x30;
	bmh_ctrl_dma.vsize_ref = 0x40;
	bmh_ctrl_dma.curstad = 0x50;
	bmh_ctrl_dma.refstad = 0x60;
	bmh_ctrl_dma.sadstad = 0x70;
	bmh_ctrl_dma.sadghsize = 0x80;
	im_ercd = Im_BMH_Ctrl_Dma( channel_no, &bmh_ctrl_dma );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl Dma error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Get_Ctrl_Dma( channel_no, &bmh_ctrl_dma_get );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Get Ctrl_Dma error = 0x%x\n", im_ercd ));
	}

	Ddim_Print(( D_IM_BMH_FUNC_NAME "bmh_ctrl_dma_get.dmamd      = 0x%lx\n", bmh_ctrl_dma_get.dmamd ));
	Ddim_Print(( D_IM_BMH_FUNC_NAME "bmh_ctrl_dma_get.ghsize_cur = 0x%lx\n", bmh_ctrl_dma_get.ghsize_cur ));
	Ddim_Print(( D_IM_BMH_FUNC_NAME "bmh_ctrl_dma_get.ghsize_ref = 0x%lx\n", bmh_ctrl_dma_get.ghsize_ref ));
	Ddim_Print(( D_IM_BMH_FUNC_NAME "bmh_ctrl_dma_get.hsize_ref  = 0x%lx\n", bmh_ctrl_dma_get.hsize_ref ));
	Ddim_Print(( D_IM_BMH_FUNC_NAME "bmh_ctrl_dma_get.vsize_ref  = 0x%lx\n", bmh_ctrl_dma_get.vsize_ref ));
	Ddim_Print(( D_IM_BMH_FUNC_NAME "bmh_ctrl_dma_get.curstad    = 0x%lx\n", bmh_ctrl_dma_get.curstad ));
	Ddim_Print(( D_IM_BMH_FUNC_NAME "bmh_ctrl_dma_get.refstad    = 0x%lx\n", bmh_ctrl_dma_get.refstad ));
	Ddim_Print(( D_IM_BMH_FUNC_NAME "bmh_ctrl_dma_get.sadstad    = 0x%lx\n", bmh_ctrl_dma_get.sadstad ));
	Ddim_Print(( D_IM_BMH_FUNC_NAME "bmh_ctrl_dma_get.sadghsize  = 0x%lx\n", bmh_ctrl_dma_get.sadghsize ));

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_28: "
INT32 CT_Im_Bmh_1_28( BYTE channel_no )
{
	INT32 im_ercd;
	ULONG count;
	UINT32 loop;

	T_IM_BMH_LUT lut_data;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	for( loop = 0; loop < 3; loop++ ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "loop[%d]\n", loop ));
		switch( loop ){
			case 1:	// max
				for( count = 0; count < 2048; count++ ){
					lut_data.lut[count] = 0xFFF;
				}
				break;

			case 2:	// individual
				for( count = 0; count < 2048; count++ ){
					lut_data.lut[count] = count;
				}
				break;

//			case 0:	// min
			default:
				for( count = 0; count < 2048; count++ ){
					lut_data.lut[count] = 0;
				}
				break;
		}

		im_ercd = Im_BMH_Set_Lut( channel_no, &lut_data );
		if( im_ercd != D_DDIM_OK ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Lut error = 0x%x\n", im_ercd ));
		}
#ifdef CO_DEBUG_ON_PC
		for(count = 0; count < 1024; count++ ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "IO_BMH[%d].LUT.word[%lu] = 0x%lx\n", channel_no, count, IO_BMH[channel_no].LUT.word[count] ));
		}
#endif // CO_DEBUG_ON_PC
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_29: "
INT32 CT_Im_Bmh_1_29( BYTE channel_no )
{
	INT32 im_ercd;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Set_Interrupt( channel_no, 0, 1 );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Interrupt error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	ct_im_bmh_print_debug_reg();

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_30: "
INT32 CT_Im_Bmh_1_30( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 i, j;
	UINT32 wait_factor;

	T_IM_BMH_CTRL bmh_ctrl = {
		.lselcnt = 0,
		.rbs = 0,
		.pshift = 0,
		.outmd = 0,
		.lutmd = 0,
		.offmd = 0,
		.mode = 1,
		.poffset = 0,
		.sadlv = 0,
		.pCallBack = bmh_callback_output_result,
	};
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;
	for( i = 0; i < 9; i++ ){
		for( j = 0; j < 256; j++ ){
			cur_write_data.cur_write_flg[i][j] = 1;
			cur_write_data.cur_data[i][j] = j;
			ref_write_data.ref_write_flg[i][j] = 1;
			ref_write_data.ref_data[i][j] = j / 2;
		}
	}

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_BMH_Init( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Init error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Interrupt( channel_no, D_IM_BMH_FLG_BMH_END | D_IM_BMH_FLG_BMH_AXI_ERR, 1 );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Interrupt error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Async( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Async error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
	IO_BMH[channel_no].BMHINTFS.bit.__BMHF = 1;
	Im_BMH_Int_Handler( channel_no );
#endif //CO_DEBUG_ON_PC

	im_ercd = Im_BMH_Wait_End( channel_no, &wait_factor, 1000 );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Wait End error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_31: "
INT32 CT_Im_Bmh_1_31( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop1, loop2, loop3;

	T_IM_BMH_CUR_BLOCK cur_write_data;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	for( loop1 = 0; loop1 < 3; loop1++ ){
		switch( loop1 ){
			case 1:	// max
				for( loop2 = 0; loop2 < 9; loop2++ ){
					for( loop3 = 0; loop3 < 256; loop3++ ){
						cur_write_data.cur_write_flg[loop2][loop3] = 1;
						cur_write_data.cur_data[loop2][loop3] = 0xFF;
					}
				}
				break;

			case 2:	// individual
				for( loop2 = 0; loop2 < 9; loop2++ ){
					for( loop3 = 0; loop3 < 256; loop3++ ){
						cur_write_data.cur_write_flg[loop2][loop3] = ( loop3 & 1 );
						cur_write_data.cur_data[loop2][loop3] = ( loop3 + 2 ) & 0xFF;
					}
				}
				break;

//			case 0:	// min
			default:
				for( loop2 = 0; loop2 < 9; loop2++ ){
					for( loop3 = 0; loop3 < 256; loop3++ ){
						cur_write_data.cur_write_flg[loop2][loop3] = 0;
						cur_write_data.cur_data[loop2][loop3] = 0;
					}
				}
				break;
		}

		im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
		if( im_ercd != D_DDIM_OK ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Cur error = 0x%x\n", im_ercd ));
			return im_ercd;
		}

#ifdef CO_DEBUG_ON_PC
		for( loop2 = 0; loop2 < 9; loop2++ ){
			for( loop3 = 0; loop3 < 64; loop3++ ){
				Ddim_Print(( D_IM_BMH_FUNC_NAME "IO_BMH[%d].CUR.word[%d][%d] = 0x%lx\n", channel_no, loop2, loop3, IO_BMH[channel_no].CUR.word[loop2][loop3] ));
			}
		}
#endif // CO_DEBUG_ON_PC
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_32: "
INT32 CT_Im_Bmh_1_32( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop1, loop2, loop3;

	T_IM_BMH_REF_BLOCK ref_write_data;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	for( loop1 = 0; loop1 < 3; loop1++ ){
		switch( loop1 ){
			case 1:	// max
				for( loop2 = 0; loop2 < 9; loop2++ ){
					for( loop3 = 0; loop3 < 256; loop3++ ){
						ref_write_data.ref_write_flg[loop2][loop3] = 1;
						ref_write_data.ref_data[loop2][loop3] = 0xFF;
					}
				}
				break;

			case 2:	// individual
				for( loop2 = 0; loop2 < 9; loop2++ ){
					for( loop3 = 0; loop3 < 256; loop3++ ){
						ref_write_data.ref_write_flg[loop2][loop3] = ( loop3 & 1 );
						ref_write_data.ref_data[loop2][loop3] = ( loop3 + 2 ) & 0xFF;
					}
				}
				break;

//			case 0:	// min
			default:
				for( loop2 = 0; loop2 < 9; loop2++ ){
					for( loop3 = 0; loop3 < 256; loop3++ ){
						ref_write_data.ref_write_flg[loop2][loop3] = 0;
						ref_write_data.ref_data[loop2][loop3] = 0;
					}
				}
				break;
		}

		im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
		if( im_ercd != D_DDIM_OK ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Ref error = 0x%x\n", im_ercd ));
			return im_ercd;
		}

#ifdef CO_DEBUG_ON_PC
		for( loop2 = 0; loop2 < 9; loop2++ ){
			for( loop3 = 0; loop3 < 64; loop3++ ){
				Ddim_Print(( D_IM_BMH_FUNC_NAME "IO_BMH[%d].REF.word[%d][%d] = 0x%lx\n", channel_no, loop2, loop3, IO_BMH[channel_no].REF.word[loop2][loop3] ));
			}
		}
#endif // CO_DEBUG_ON_PC
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_33: "
INT32 CT_Im_Bmh_1_33( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 i, j;

	T_IM_BMH_SADR_000 sadr_read_data;
	ULONG loop1;
	ULONG loop2;

	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;
	for( i = 0; i < 8; i++ ){
		for( j = 0; j < 32; j++ ){
			cur_write_data.cur_write_flg[i][j] = 1;
			cur_write_data.cur_data[i][j] = j + 2;
			ref_write_data.ref_write_flg[i][j] = 1;
			ref_write_data.ref_data[i][j] = j;
		}
	}

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_BMH_Init( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Init error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
		IO_BMH[channel_no].BMHINTFS.bit.__BMHF = 1;
#endif //CO_DEBUG_ON_PC

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
	for(loop1 = 0; loop1 < 2; loop1++ ){
		for(loop2 = 0; loop2 < 56; loop2++ ){
			IO_BMH[channel_no].SADR.bit000[loop1][loop2].POS_0    = loop2;
			IO_BMH[channel_no].SADR.bit000[loop1][loop2].SADcmp_0 = loop1;
			IO_BMH[channel_no].SADR.bit000[loop1][loop2].POS_1    = loop2 + 1;
			IO_BMH[channel_no].SADR.bit000[loop1][loop2].SADcmp_1 = loop1;
			IO_BMH[channel_no].SADR.bit000[loop1][loop2].POS_2    = loop2 + 2;
			IO_BMH[channel_no].SADR.bit000[loop1][loop2].SADcmp_2 = loop1;
			IO_BMH[channel_no].SADR.bit000[loop1][loop2].POS_3    = loop2 + 3;
			IO_BMH[channel_no].SADR.bit000[loop1][loop2].SADcmp_3 = loop1;
		}
		IO_BMH[channel_no].SADR.bit000[loop1][56].POS_0    = loop2;
		IO_BMH[channel_no].SADR.bit000[loop1][56].SADcmp_0 = loop1;
	}
#endif // CO_DEBUG_ON_PC

	im_ercd = Im_BMH_Get_Sadr_000( channel_no, &sadr_read_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Get Sadr000 error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	for(loop1 = 0; loop1 < 2; loop1++ ){
		for(loop2 = 0; loop2 < 228; loop2++ ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.sadcmp[%lu][%lu] = 0x%x\n",loop1,loop2, sadr_read_data.sadcmp[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.pos[%lu][%lu]    = 0x%x\n",loop1,loop2, sadr_read_data.pos[loop1][loop2] ));
		}
	}

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_34: "
INT32 CT_Im_Bmh_1_34( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 i, j;

	T_IM_BMH_CTRL bmh_ctrl = {
		.lselcnt = 0,
		.rbs = 0,
		.pshift = 0,
		.outmd = 1,
		.lutmd = 0,
		.offmd = 0,
		.mode = 1,
		.poffset = 0,
		.sadlv = 0,
		.pCallBack = bmh_callback,
	};
	T_IM_BMH_SADR_001 sadr_read_data;
	ULONG loop1;

	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;
	for( i = 0; i < 8; i++ ){
		for( j = 0; j < 32; j++ ){
			cur_write_data.cur_write_flg[i][j] = 1;
			cur_write_data.cur_data[i][j] = j + 2;
			ref_write_data.ref_write_flg[i][j] = 1;
			ref_write_data.ref_data[i][j] = j;
		}
	}

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_BMH_Init( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Init error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
		IO_BMH[channel_no].BMHINTFS.bit.__BMHF = 1;
#endif //CO_DEBUG_ON_PC

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
	for(loop1 = 0; loop1 < 225; loop1++ ){
		IO_BMH[channel_no].SADR.bit001[loop1].SADmin = loop1;
		IO_BMH[channel_no].SADR.bit001[loop1].POS    = ( loop1 & 0x7F );
		IO_BMH[channel_no].SADR.bit001[loop1].SADcmp = ( loop1 & 1 );
	}
#endif // CO_DEBUG_ON_PC

	im_ercd = Im_BMH_Get_Sadr_001( channel_no, &sadr_read_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Get Sadr001 error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	for(loop1 = 0; loop1 < 225; loop1++ ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.sadcmp[%lu] = 0x%x\n",loop1, sadr_read_data.sadcmp[loop1] ));
		Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.pos[%lu]    = 0x%x\n",loop1, sadr_read_data.pos[loop1] ));
		Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.sadmin[%lu] = 0x%x\n",loop1, sadr_read_data.sadmin[loop1] ));
	}

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_35: "
INT32 CT_Im_Bmh_1_35( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 i, j;

	T_IM_BMH_CTRL bmh_ctrl = {
		.lselcnt = 0,
		.rbs = 0,
		.pshift = 0,
		.outmd = 2,
		.lutmd = 0,
		.offmd = 0,
		.mode = 1,
		.poffset = 0,
		.sadlv = 0,
		.pCallBack = bmh_callback,
	};
	T_IM_BMH_SADR_010 sadr_read_data;
	ULONG loop1;
	ULONG loop2;

	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;
	for( i = 0; i < 8; i++ ){
		for( j = 0; j < 32; j++ ){
			cur_write_data.cur_write_flg[i][j] = 1;
			cur_write_data.cur_data[i][j] = j + 2;
			ref_write_data.ref_write_flg[i][j] = 1;
			ref_write_data.ref_data[i][j] = j;
		}
	}

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_BMH_Init( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Init error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
		IO_BMH[channel_no].BMHINTFS.bit.__BMHF = 1;
#endif //CO_DEBUG_ON_PC

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
	for(loop1 = 0; loop1 < 2; loop1++ ){
		for(loop2 = 0; loop2 < 112; loop2++ ){
			IO_BMH[channel_no].SADR.bit010[loop1][loop2].POS_fraction_0 = (( 0xFF - loop2 ) & 0x3F );
			IO_BMH[channel_no].SADR.bit010[loop1][loop2].POS_Integer_0  = loop2 + 1;
			IO_BMH[channel_no].SADR.bit010[loop1][loop2].SADcmp_0       = loop1;
			IO_BMH[channel_no].SADR.bit010[loop1][loop2].POS_fraction_1 = (( 0xFE - loop2 ) & 0x3F );
			IO_BMH[channel_no].SADR.bit010[loop1][loop2].POS_Integer_1  = loop2 + 2;
			IO_BMH[channel_no].SADR.bit010[loop1][loop2].SADcmp_1       = loop1;
		}
		IO_BMH[channel_no].SADR.bit010[loop1][112].POS_fraction_0 = (( 0xFF - loop2 ) & 0x3F );
		IO_BMH[channel_no].SADR.bit010[loop1][112].POS_Integer_0  = loop2 + 1;
		IO_BMH[channel_no].SADR.bit010[loop1][112].SADcmp_0       = loop1;
	}
#endif // CO_DEBUG_ON_PC

	im_ercd = Im_BMH_Get_Sadr_010( channel_no, &sadr_read_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Get Sadr010 error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
	}

	for(loop1 = 0; loop1 < 2; loop1++ ){
		for(loop2 = 0; loop2 < 226; loop2++ ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.sadcmp[%lu][%lu]       = 0x%x\n",loop1,loop2, sadr_read_data.sadcmp[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.pos_integer[%lu][%lu]  = 0x%x\n",loop1,loop2, sadr_read_data.pos_integer[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.pos_fraction[%lu][%lu] = 0x%x\n",loop1,loop2, sadr_read_data.pos_fraction[loop1][loop2] ));
		}
	}

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_36: "
INT32 CT_Im_Bmh_1_36( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 i, j;

	T_IM_BMH_CTRL bmh_ctrl = {
		.lselcnt = 0,
		.rbs = 0,
		.pshift = 0,
		.outmd = 3,
		.lutmd = 0,
		.offmd = 0,
		.mode = 1,
		.poffset = 0,
		.sadlv = 0,
		.pCallBack = bmh_callback,
	};
	T_IM_BMH_SADR_011 sadr_read_data;
	ULONG loop1;

	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;
	for( i = 0; i < 8; i++ ){
		for( j = 0; j < 32; j++ ){
			cur_write_data.cur_write_flg[i][j] = 1;
			cur_write_data.cur_data[i][j] = j + 2;
			ref_write_data.ref_write_flg[i][j] = 1;
			ref_write_data.ref_data[i][j] = j;
		}
	}

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_BMH_Init( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Init error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
		IO_BMH[channel_no].BMHINTFS.bit.__BMHF = 1;
#endif //CO_DEBUG_ON_PC

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
	for(loop1 = 0; loop1 < 225; loop1++ ){
		IO_BMH[channel_no].SADR.bit011[loop1].SADmin       = loop1;
		IO_BMH[channel_no].SADR.bit011[loop1].POS_fraction = (( 0xFF - loop1 ) & 0x3F );
		IO_BMH[channel_no].SADR.bit011[loop1].POS_Integer  = ( loop1 & 0x7F );
		IO_BMH[channel_no].SADR.bit011[loop1].SADcmp       = ( loop1 & 1 );
	}
#endif // CO_DEBUG_ON_PC

	im_ercd = Im_BMH_Get_Sadr_011( channel_no, &sadr_read_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Im_BMH_Get_Sadr_011 error = 0x%x\n", im_ercd ));
	}

	for(loop1 = 0; loop1 < 225; loop1++ ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.sadcmp[%lu]       = 0x%x\n",loop1, sadr_read_data.sadcmp[loop1] ));
		Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.pos_integer[%lu]  = 0x%x\n",loop1, sadr_read_data.pos_integer[loop1] ));
		Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.pos_fraction[%lu] = 0x%x\n",loop1, sadr_read_data.pos_fraction[loop1] ));
		Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.sadmin[%lu]       = 0x%x\n",loop1, sadr_read_data.sadmin[loop1] ));
	}

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_37: "
INT32 CT_Im_Bmh_1_37( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 i, j;

	T_IM_BMH_CTRL bmh_ctrl = {
		.lselcnt = 0,
		.rbs = 0,
		.pshift = 0,
		.outmd = 4,
		.lutmd = 0,
		.offmd = 0,
		.mode = 1,
		.poffset = 0,
		.sadlv = 0,
		.pCallBack = bmh_callback,
	};
	T_IM_BMH_SADR_100 sadr_read_data;
	ULONG loop1;
	ULONG loop2;

	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;
	for( i = 0; i < 8; i++ ){
		for( j = 0; j < 32; j++ ){
			cur_write_data.cur_write_flg[i][j] = 1;
			cur_write_data.cur_data[i][j] = j + 2;
			ref_write_data.ref_write_flg[i][j] = 1;
			ref_write_data.ref_data[i][j] = j;
		}
	}

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_BMH_Init( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Init error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
		IO_BMH[channel_no].BMHINTFS.bit.__BMHF = 1;
#endif //CO_DEBUG_ON_PC

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
	for(loop1 = 0; loop1 < 2; loop1++ ){
		for(loop2 = 0; loop2 < 112; loop2++ ){
			IO_BMH[channel_no].SADR.bit100[loop1][loop2].SADmin       = loop2;
			IO_BMH[channel_no].SADR.bit100[loop1][loop2].POS_fraction = (( 0xFF - loop2 ) & 0x3F );
			IO_BMH[channel_no].SADR.bit100[loop1][loop2].POS_Integer  = loop2 + 1;
			IO_BMH[channel_no].SADR.bit100[loop1][loop2].SADcmp       = loop1;
		}
	}
#endif // CO_DEBUG_ON_PC

	im_ercd = Im_BMH_Get_Sadr_100( channel_no, &sadr_read_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Get Sadr100 error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
	}

	for(loop1 = 0; loop1 < 2; loop1++ ){
		for(loop2 = 0; loop2 < 112; loop2++ ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.sadcmp[%lu][%lu]       = 0x%x\n",loop1,loop2, sadr_read_data.sadcmp[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.pos_integer[%lu][%lu]  = 0x%x\n",loop1,loop2, sadr_read_data.pos_integer[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.pos_fraction[%lu][%lu] = 0x%x\n",loop1,loop2, sadr_read_data.pos_fraction[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.sadmin[%lu][%lu]       = 0x%x\n",loop1,loop2, sadr_read_data.sadmin[loop1][loop2] ));
		}
	}

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_38: "
INT32 CT_Im_Bmh_1_38( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 i, j;

	T_IM_BMH_CTRL bmh_ctrl = {
		.lselcnt = 0,
		.rbs = 0,
		.pshift = 0,
		.outmd = 5,
		.lutmd = 0,
		.offmd = 0,
		.mode = 1,
		.poffset = 0,
		.sadlv = 0,
		.pCallBack = bmh_callback,
	};
	T_IM_BMH_SADR_101 sadr_read_data;
	ULONG loop1;
	ULONG loop2;

	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;
	for( i = 0; i < 8; i++ ){
		for( j = 0; j < 32; j++ ){
			cur_write_data.cur_write_flg[i][j] = 1;
			cur_write_data.cur_data[i][j] = j + 2;
			ref_write_data.ref_write_flg[i][j] = 1;
			ref_write_data.ref_data[i][j] = j;
		}
	}

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_BMH_Init( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Init error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
		IO_BMH[channel_no].BMHINTFS.bit.__BMHF = 1;
#endif //CO_DEBUG_ON_PC

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
	for(loop1 = 0; loop1 < 2; loop1++ ){
		for(loop2 = 0; loop2 < 56; loop2++ ){
			IO_BMH[channel_no].SADR.bit101[loop1][loop2].SADmin       = loop2;
			IO_BMH[channel_no].SADR.bit101[loop1][loop2].POS_fraction = 0x3F - loop2;
			IO_BMH[channel_no].SADR.bit101[loop1][loop2].POS_Integer  = loop2 + 1;
			IO_BMH[channel_no].SADR.bit101[loop1][loop2].SADcmp       = loop1;
			IO_BMH[channel_no].SADR.bit101[loop1][loop2].CUR_MIN      = loop2 + 2;
			IO_BMH[channel_no].SADR.bit101[loop1][loop2].CUR_MAX      = loop2 + 3;
			IO_BMH[channel_no].SADR.bit101[loop1][loop2].Deviation    = loop2 + 4;
			IO_BMH[channel_no].SADR.bit101[loop1][loop2].Weight       = loop2 + 5;
		}
	}
#endif // CO_DEBUG_ON_PC

	im_ercd = Im_BMH_Get_Sadr_101( channel_no, &sadr_read_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Im_BMH_Get_Sadr_101 error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
	}

	for(loop1 = 0; loop1 < 2; loop1++ ){
		for(loop2 = 0; loop2 < 56; loop2++ ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.sadcmp[%lu][%lu]       = 0x%x\n",loop1,loop2, sadr_read_data.sadcmp[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.pos_integer[%lu][%lu]  = 0x%x\n",loop1,loop2, sadr_read_data.pos_integer[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.pos_fraction[%lu][%lu] = 0x%x\n",loop1,loop2, sadr_read_data.pos_fraction[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.sadmin[%lu][%lu]       = 0x%x\n",loop1,loop2, sadr_read_data.sadmin[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.weight[%lu][%lu]       = 0x%x\n",loop1,loop2, sadr_read_data.weight[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.deviation[%lu][%lu]    = 0x%x\n",loop1,loop2, sadr_read_data.deviation[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.cur_max[%lu][%lu]      = 0x%x\n",loop1,loop2, sadr_read_data.cur_max[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.cur_min[%lu][%lu]      = 0x%x\n",loop1,loop2, sadr_read_data.cur_min[loop1][loop2] ));
		}
	}

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}

	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_1_39: "
INT32 CT_Im_Bmh_1_39( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 i, j;

	T_IM_BMH_CTRL bmh_ctrl = {
		.lselcnt = 0,
		.rbs = 0,
		.pshift = 0,
		.outmd = 6,
		.lutmd = 0,
		.offmd = 0,
		.mode = 1,
		.poffset = 0,
		.sadlv = 0,
		.pCallBack = bmh_callback,
	};
	T_IM_BMH_SADR_110 sadr_read_data;
	ULONG loop1;
	ULONG loop2;

	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;
	for( i = 0; i < 8; i++ ){
		for( j = 0; j < 32; j++ ){
			cur_write_data.cur_write_flg[i][j] = 1;
			cur_write_data.cur_data[i][j] = j + 2;
			ref_write_data.ref_write_flg[i][j] = 1;
			ref_write_data.ref_data[i][j] = j;
		}
	}

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	im_ercd = Im_BMH_Init( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Init error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
		IO_BMH[channel_no].BMHINTFS.bit.__BMHF = 1;
#endif //CO_DEBUG_ON_PC

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

#ifdef CO_DEBUG_ON_PC
	for(loop1 = 0; loop1 < 2; loop1++ ){
		for(loop2 = 0; loop2 < 28; loop2++ ){
			IO_BMH[channel_no].SADR.bit110[loop1][loop2].SADmin        = loop2;
			IO_BMH[channel_no].SADR.bit110[loop1][loop2].POS           = loop2 + 1;
			IO_BMH[channel_no].SADR.bit110[loop1][loop2].SADcmp        = loop1;
			IO_BMH[channel_no].SADR.bit110[loop1][loop2].SADmin1after  = loop2 + 2;
			IO_BMH[channel_no].SADR.bit110[loop1][loop2].SADmin1before = loop2 + 3;
			IO_BMH[channel_no].SADR.bit110[loop1][loop2].SADmin2after  = loop2 + 4;
			IO_BMH[channel_no].SADR.bit110[loop1][loop2].SADmin2before = loop2 + 5;
			IO_BMH[channel_no].SADR.bit110[loop1][loop2].CUR_MIN       = loop2 + 6;
			IO_BMH[channel_no].SADR.bit110[loop1][loop2].CUR_MAX       = loop2 + 7;
			IO_BMH[channel_no].SADR.bit110[loop1][loop2].Deviation     = loop2 + 8;
			IO_BMH[channel_no].SADR.bit110[loop1][loop2].Weight        = loop2 + 9;
		}
	}
#endif // CO_DEBUG_ON_PC

	im_ercd = Im_BMH_Get_Sadr_110( channel_no, &sadr_read_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Im_BMH_Get_Sadr_110 error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
	}

	for(loop1 = 0; loop1 < 2; loop1++ ){
		for(loop2 = 0; loop2 < 28; loop2++ ){
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.sadcmp[%lu][%lu]        = 0x%x\n",loop1,loop2, sadr_read_data.sadcmp[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.pos[%lu][%lu]           = 0x%x\n",loop1,loop2, sadr_read_data.pos[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.sadmin[%lu][%lu]        = 0x%x\n",loop1,loop2, sadr_read_data.sadmin[loop1][loop2] ));

			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.sadmin1before[%lu][%lu] = 0x%x\n",loop1,loop2, sadr_read_data.sadmin1before[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.sadmin1after[%lu][%lu]  = 0x%x\n",loop1,loop2, sadr_read_data.sadmin1after[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.sadmin2before[%lu][%lu] = 0x%x\n",loop1,loop2, sadr_read_data.sadmin2before[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.sadmin2after[%lu][%lu]  = 0x%x\n",loop1,loop2, sadr_read_data.sadmin2after[loop1][loop2] ));

			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.weight[%lu][%lu]        = 0x%x\n",loop1,loop2, sadr_read_data.weight[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.deviation[%lu][%lu]     = 0x%x\n",loop1,loop2, sadr_read_data.deviation[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.cur_max[%lu][%lu]       = 0x%x\n",loop1,loop2, sadr_read_data.cur_max[loop1][loop2] ));
			Ddim_Print(( D_IM_BMH_FUNC_NAME "sadr_read_data.cur_min[%lu][%lu]       = 0x%x\n",loop1,loop2, sadr_read_data.cur_min[loop1][loop2] ));
		}
	}

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}

	return im_ercd;
}

static INT32 ct_im_bmh_run_1( const UINT32 ct_no_2nd, const BYTE channel_no )
{
	switch( ct_no_2nd ){
		// Test-1-1
		case 1:
			return CT_Im_Bmh_1_1( channel_no );

		// Test-1-2
		case 2:
			return CT_Im_Bmh_1_2( channel_no );

		// Test-1-3
		case 3:
			return CT_Im_Bmh_1_3( channel_no );

		// Test-1-4
		case 4:
			return CT_Im_Bmh_1_4( channel_no );

		// Test-1-5
		case 5:
			return CT_Im_Bmh_1_5( channel_no );

		// Test-1-6
		case 6:
			return CT_Im_Bmh_1_6( channel_no );

		// Test-1-7
		case 7:
			return CT_Im_Bmh_1_7( channel_no );

		// Test-1-8
		case 8:
			return CT_Im_Bmh_1_8( channel_no );

		// Test-1-9
		case 9:
			return CT_Im_Bmh_1_9( channel_no );

		// Test-1-10
		case 10:
			return CT_Im_Bmh_1_10( channel_no );

		// Test-1-11
		case 11:
			return CT_Im_Bmh_1_11( channel_no );

		// Test-1-12
		case 12:
			return CT_Im_Bmh_1_12( channel_no );

		// Test-1-13
		case 13:
			return CT_Im_Bmh_1_13( channel_no );

		// Test-1-14
		case 14:
			return CT_Im_Bmh_1_14( channel_no );

		// Test-1-15
		case 15:
			return CT_Im_Bmh_1_15( channel_no );

		// Test-1-16
		case 16:
			return CT_Im_Bmh_1_16( channel_no );

		// Test-1-17
		case 17:
			return CT_Im_Bmh_1_17( channel_no );

		// Test-1-18
		case 18:
			return CT_Im_Bmh_1_18( channel_no );

		// Test-1-19
		case 19:
			return CT_Im_Bmh_1_19( channel_no );

		// Test-1-20
		case 20:
			return CT_Im_Bmh_1_20( channel_no );

		// Test-1-21
		case 21:
			return CT_Im_Bmh_1_21( channel_no );

		// Test-1-22
		case 22:
			return CT_Im_Bmh_1_22( channel_no );

		// Test-1-23
		case 23:
			return CT_Im_Bmh_1_23( channel_no );

		// Test-1-24
		case 24:
			return CT_Im_Bmh_1_24( channel_no );

		// Test-1-25
		case 25:
			return CT_Im_Bmh_1_25( channel_no );

		// Test-1-26
		case 26:
			return CT_Im_Bmh_1_26( channel_no );

		// Test-1-27
		case 27:
			return CT_Im_Bmh_1_27( channel_no );

		// Test-1-28
		case 28:
			return CT_Im_Bmh_1_28( channel_no );

		// Test-1-29
		case 29:
			return CT_Im_Bmh_1_29( channel_no );

		// Test-1-30
		case 30:
			return CT_Im_Bmh_1_30( channel_no );

		// Test-1-31
		case 31:
			return CT_Im_Bmh_1_31( channel_no );

		// Test-1-32
		case 32:
			return CT_Im_Bmh_1_32( channel_no );

		// Test-1-33
		case 33:
			return CT_Im_Bmh_1_33( channel_no );

		// Test-1-34
		case 34:
			return CT_Im_Bmh_1_34( channel_no );

		// Test-1-35
		case 35:
			return CT_Im_Bmh_1_35( channel_no );

		// Test-1-36
		case 36:
			return CT_Im_Bmh_1_36( channel_no );

		// Test-1-37
		case 37:
			return CT_Im_Bmh_1_37( channel_no );

		// Test-1-38
		case 38:
			return CT_Im_Bmh_1_38( channel_no );

		// Test-1-39
		case 39:
			return CT_Im_Bmh_1_39( channel_no );

		default:
			Ddim_Print(( "Error: Unknown command.\n" ));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_1: "
INT32 CT_Im_Bmh_2_1( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Ctrl( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_2: "
INT32 CT_Im_Bmh_2_2( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Ctrl( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_3: "
INT32 CT_Im_Bmh_2_3( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Set_Bmhre( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_4: "
INT32 CT_Im_Bmh_2_4( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Bmhre( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_5: "
INT32 CT_Im_Bmh_2_5( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Rsst( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_6: "
INT32 CT_Im_Bmh_2_6( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Set_Bmhcont( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_7: "
INT32 CT_Im_Bmh_2_7( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Bmhcont( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_8: "
INT32 CT_Im_Bmh_2_8( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Set_Bmhaxi( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_9: "
INT32 CT_Im_Bmh_2_9( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Bmhaxierr( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_10: "
INT32 CT_Im_Bmh_2_10( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Set_Bmhhcnt( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_11: "
INT32 CT_Im_Bmh_2_11( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Bmhhcnt( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_12: "
INT32 CT_Im_Bmh_2_12( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Set_Bmhsad_wr( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_13: "
INT32 CT_Im_Bmh_2_13( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Bmhsad_wr( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_14: "
INT32 CT_Im_Bmh_2_14( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Ctrl_Dma( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_15: "
INT32 CT_Im_Bmh_2_15( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Ctrl_Dma( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_16: "
INT32 CT_Im_Bmh_2_16( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Set_Lut( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_17: "
INT32 CT_Im_Bmh_2_17( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;
	USHORT interrupt_bit = 0;
	UCHAR permission_flg = 2;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Set_Interrupt( channel_no, interrupt_bit, permission_flg);
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_18: "
INT32 CT_Im_Bmh_2_18( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Set_Cur( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_19: "
INT32 CT_Im_Bmh_2_19( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Set_Ref( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_20: "
INT32 CT_Im_Bmh_2_20( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Sadr_000( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_21: "
INT32 CT_Im_Bmh_2_21( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Sadr_001( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_22: "
INT32 CT_Im_Bmh_2_22( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Sadr_010( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_23: "
INT32 CT_Im_Bmh_2_23( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Sadr_011( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_24: "
INT32 CT_Im_Bmh_2_24( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Sadr_100( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_25: "
INT32 CT_Im_Bmh_2_25( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Sadr_101( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_26: "
INT32 CT_Im_Bmh_2_26( VOID )
{
	INT32 ercd;
	BYTE channel_no = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Sadr_110( channel_no, NULL );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_27: "
INT32 CT_Im_Bmh_2_27( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_CTRL bmh_ctrl;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_28: "
INT32 CT_Im_Bmh_2_28( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_CTRL bmh_ctrl;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Ctrl( channel_no, &bmh_ctrl );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_29: "
INT32 CT_Im_Bmh_2_29( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_BMHRE sram;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Set_Bmhre( channel_no, &sram );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_30: "
INT32 CT_Im_Bmh_2_30( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_BMHRE sram;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Bmhre( channel_no, &sram );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_31: "
INT32 CT_Im_Bmh_2_31( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_RSST rsst;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Rsst( channel_no, &rsst );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_32: "
INT32 CT_Im_Bmh_2_32( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_BMHCONT cont;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Set_Bmhcont( channel_no, &cont );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_33: "
INT32 CT_Im_Bmh_2_33( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_BMHCONT cont;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Bmhcont( channel_no, &cont );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_34: "
INT32 CT_Im_Bmh_2_34( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_AXI axi_ctrl;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Set_Bmhaxi( channel_no, &axi_ctrl );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_35: "
INT32 CT_Im_Bmh_2_35( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_AXI_STATUS sts;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Bmhaxierr( channel_no, &sts );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_36: "
INT32 CT_Im_Bmh_2_36( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_BMHHCNT cnt;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Set_Bmhhcnt( channel_no, &cnt );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_37: "
INT32 CT_Im_Bmh_2_37( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_BMHHCNT cnt;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Bmhhcnt( channel_no, &cnt);
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_38: "
INT32 CT_Im_Bmh_2_38( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_BMHSAD_WR sad_wr;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Set_Bmhsad_wr( channel_no, &sad_wr );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_39: "
INT32 CT_Im_Bmh_2_39( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_BMHSAD_WR sad_wr;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Bmhsad_wr( channel_no, &sad_wr );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_40: "
INT32 CT_Im_Bmh_2_40( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_CTRL_DMA bmh_ctrl_dma;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Ctrl_Dma( channel_no, &bmh_ctrl_dma );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_41: "
INT32 CT_Im_Bmh_2_41( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_CTRL_DMA bmh_ctrl_dma;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Ctrl_Dma( channel_no, &bmh_ctrl_dma );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_42: "
INT32 CT_Im_Bmh_2_42( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_LUT lut_data;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Set_Lut( channel_no, &lut_data );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_43: "
INT32 CT_Im_Bmh_2_43( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	USHORT interrupt_bit = 0;
	UCHAR permission_flg = 0;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Set_Interrupt( channel_no, interrupt_bit, permission_flg);
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_44: "
INT32 CT_Im_Bmh_2_44( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_CUR_BLOCK cur_write_data;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_45: "
INT32 CT_Im_Bmh_2_45( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_REF_BLOCK ref_write_data;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_46: "
INT32 CT_Im_Bmh_2_46( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_SADR_000 sadr_read_data;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Sadr_000( channel_no, &sadr_read_data );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_47: "
INT32 CT_Im_Bmh_2_47( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_SADR_001 sadr_read_data;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Sadr_001( channel_no, &sadr_read_data );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_48: "
INT32 CT_Im_Bmh_2_48( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_SADR_010 sadr_read_data;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Sadr_010( channel_no, &sadr_read_data );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_49: "
INT32 CT_Im_Bmh_2_49( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_SADR_011 sadr_read_data;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Sadr_011( channel_no, &sadr_read_data );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_50: "
INT32 CT_Im_Bmh_2_50( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_SADR_100 sadr_read_data;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Sadr_100( channel_no, &sadr_read_data );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_51: "
INT32 CT_Im_Bmh_2_51( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_SADR_101 sadr_read_data;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Sadr_101( channel_no, &sadr_read_data );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_2_52: "
INT32 CT_Im_Bmh_2_52( VOID )
{
	INT32 ercd;
	BYTE channel_no = 2;
	T_IM_BMH_SADR_110 sadr_read_data;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	ercd = Im_BMH_Get_Sadr_110( channel_no, &sadr_read_data );
	Ddim_Print(( D_IM_BMH_FUNC_NAME "result = 0x%x\n", ercd ));

	return D_DDIM_OK;
}



static INT32 ct_im_bmh_run_2( const UINT32 ct_no_2nd )
{
	switch( ct_no_2nd ){
		// Test-2-1
		case 1:
			return CT_Im_Bmh_2_1();

		// Test-2-2
		case 2:
			return CT_Im_Bmh_2_2();

		// Test-2-3
		case 3:
			return CT_Im_Bmh_2_3();

		// Test-2-4
		case 4:
			return CT_Im_Bmh_2_4();

		// Test-2-5
		case 5:
			return CT_Im_Bmh_2_5();

		// Test-2-6
		case 6:
			return CT_Im_Bmh_2_6();

		// Test-2-7
		case 7:
			return CT_Im_Bmh_2_7();

		// Test-2-8
		case 8:
			return CT_Im_Bmh_2_8();

		// Test-2-9
		case 9:
			return CT_Im_Bmh_2_9();

		// Test-2-10
		case 10:
			return CT_Im_Bmh_2_10();

		// Test-2-11
		case 11:
			return CT_Im_Bmh_2_11();

		// Test-2-12
		case 12:
			return CT_Im_Bmh_2_12();

		// Test-2-13
		case 13:
			return CT_Im_Bmh_2_13();

		// Test-2-14
		case 14:
			return CT_Im_Bmh_2_14();

		// Test-2-15
		case 15:
			return CT_Im_Bmh_2_15();

		// Test-2-16
		case 16:
			return CT_Im_Bmh_2_16();

		// Test-2-17
		case 17:
			return CT_Im_Bmh_2_17();

		// Test-2-18
		case 18:
			return CT_Im_Bmh_2_18();

		// Test-2-19
		case 19:
			return CT_Im_Bmh_2_19();

		// Test-2-20
		case 20:
			return CT_Im_Bmh_2_20();

		// Test-2-21
		case 21:
			return CT_Im_Bmh_2_21();

		// Test-2-22
		case 22:
			return CT_Im_Bmh_2_22();

		// Test-2-23
		case 23:
			return CT_Im_Bmh_2_23();

		// Test-2-24
		case 24:
			return CT_Im_Bmh_2_24();

		// Test-2-25
		case 25:
			return CT_Im_Bmh_2_25();

		// Test-2-26
		case 26:
			return CT_Im_Bmh_2_26();

		// Test-2-27
		case 27:
			return CT_Im_Bmh_2_27();

		// Test-2-28
		case 28:
			return CT_Im_Bmh_2_28();

		// Test-2-29
		case 29:
			return CT_Im_Bmh_2_29();

		// Test-2-30
		case 30:
			return CT_Im_Bmh_2_30();

		// Test-2-31
		case 31:
			return CT_Im_Bmh_2_31();

		// Test-2-32
		case 32:
			return CT_Im_Bmh_2_32();

		// Test-2-33
		case 33:
			return CT_Im_Bmh_2_33();

		// Test-2-34
		case 34:
			return CT_Im_Bmh_2_34();

		// Test-2-35
		case 35:
			return CT_Im_Bmh_2_35();

		// Test-2-36
		case 36:
			return CT_Im_Bmh_2_36();

		// Test-2-37
		case 37:
			return CT_Im_Bmh_2_37();

		// Test-2-38
		case 38:
			return CT_Im_Bmh_2_38();

		// Test-2-39
		case 39:
			return CT_Im_Bmh_2_39();

		// Test-2-40
		case 40:
			return CT_Im_Bmh_2_40();

		// Test-2-41
		case 41:
			return CT_Im_Bmh_2_41();

		// Test-2-42
		case 42:
			return CT_Im_Bmh_2_42();

		// Test-2-43
		case 43:
			return CT_Im_Bmh_2_43();

		// Test-2-44
		case 44:
			return CT_Im_Bmh_2_44();

		// Test-2-45
		case 45:
			return CT_Im_Bmh_2_45();

		// Test-2-46
		case 46:
			return CT_Im_Bmh_2_46();

		// Test-2-47
		case 47:
			return CT_Im_Bmh_2_47();

		// Test-2-48
		case 48:
			return CT_Im_Bmh_2_48();

		// Test-2-49
		case 49:
			return CT_Im_Bmh_2_49();

		// Test-2-50
		case 50:
			return CT_Im_Bmh_2_50();

		// Test-2-51
		case 51:
			return CT_Im_Bmh_2_51();

		// Test-2-52
		case 52:
			return CT_Im_Bmh_2_52();

		default:
			Ddim_Print(( "Error: Unknown command.\n" ));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_1: "
INT32 CT_Im_Bmh_3_1( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_2: "
INT32 CT_Im_Bmh_3_2( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.lselcnt = 1;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}

#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_3: "
INT32 CT_Im_Bmh_3_3( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.lselcnt = 2;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_4: "
INT32 CT_Im_Bmh_3_4( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.lselcnt = 3;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_5: "
INT32 CT_Im_Bmh_3_5( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.lselcnt = 4;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_6: "
INT32 CT_Im_Bmh_3_6( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.lselcnt = 5;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_7: "
INT32 CT_Im_Bmh_3_7( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.lselcnt = 6;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_8: "
INT32 CT_Im_Bmh_3_8( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.lselcnt = 7;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_9: "
INT32 CT_Im_Bmh_3_9( BYTE channel_no )
{
	INT32 im_ercd;

	im_ercd = CT_Im_Bmh_3_1( channel_no );

	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_10: "
INT32 CT_Im_Bmh_3_10( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.rbs = 1;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_11: "
INT32 CT_Im_Bmh_3_11( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.rbs = 2;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_12: "
INT32 CT_Im_Bmh_3_12( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.rbs = 3;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_13: "
INT32 CT_Im_Bmh_3_13( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.rbs = 4;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_14: "
INT32 CT_Im_Bmh_3_14( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.rbs = 5;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_15: "
INT32 CT_Im_Bmh_3_15( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.rbs = 6;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_16: "
INT32 CT_Im_Bmh_3_16( BYTE channel_no )
{
	INT32 im_ercd;

	im_ercd = CT_Im_Bmh_3_1( channel_no );

	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_17: "
INT32 CT_Im_Bmh_3_17( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.pshift = 1;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_18: "
INT32 CT_Im_Bmh_3_18( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.pshift = 2;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_19: "
INT32 CT_Im_Bmh_3_19( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.pshift = 3;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_20: "
INT32 CT_Im_Bmh_3_20( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.pshift = 4;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_21: "
INT32 CT_Im_Bmh_3_21( BYTE channel_no )
{
	INT32 im_ercd;

	im_ercd = CT_Im_Bmh_3_1( channel_no );

	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_22: "
INT32 CT_Im_Bmh_3_22( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.outmd = 1;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_23: "
INT32 CT_Im_Bmh_3_23( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.outmd = 2;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_24: "
INT32 CT_Im_Bmh_3_24( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.outmd = 3;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_25: "
INT32 CT_Im_Bmh_3_25( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.outmd = 4;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_26: "
INT32 CT_Im_Bmh_3_26( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.outmd = 5;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_27: "
INT32 CT_Im_Bmh_3_27( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.outmd = 6;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_28: "
INT32 CT_Im_Bmh_3_28( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;
	T_IM_BMH_BMHSAD_WR sad_wr;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.outmd = 6;
	sad_wr.sad_wr = 0x1F;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Bmhsad_wr( channel_no, &sad_wr );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Bmhsad_wr error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_29: "
INT32 CT_Im_Bmh_3_29( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.lutmd = 1;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_30: "
INT32 CT_Im_Bmh_3_30( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.offmd = 1;
	bmh_ctrl.poffset = 10;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_31: "
INT32 CT_Im_Bmh_3_31( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	// top line
	memcpy( cur_write_data.cur_data[0], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
	memcpy( ref_write_data.ref_data[0], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
	memset( cur_write_data.cur_write_flg[0], 1, 256 );
	memset( ref_write_data.ref_write_flg[0], 1, 256 );

	// 2~8 line(last 16pix)
	for( loop = 1; loop < 8; loop++ ){
		memcpy( &cur_write_data.cur_data[loop][256 - 16], (VOID*)(D_IM_BMH_TEST_CUR_IMG_ADDR + D_IM_BMH_TEST_IMG_HSIZE * loop - 16), 16 );
		memcpy( &ref_write_data.ref_data[loop][256 - 16], (VOID*)(D_IM_BMH_TEST_REF_IMG_ADDR + D_IM_BMH_TEST_IMG_HSIZE * loop - 16), 16 );
		memset( &cur_write_data.cur_write_flg[loop][256 - 16], 1, 16 );
		memset( &ref_write_data.ref_write_flg[loop][256 - 16], 1, 16 );
	}

	bmh_ctrl.mode = 5;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_32: "
INT32 CT_Im_Bmh_3_32( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.mode = 11;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_33: "
INT32 CT_Im_Bmh_3_33( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;
	T_IM_BMH_BMHCONT cont;

	// top line
	memcpy( cur_write_data.cur_data[0], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
	memcpy( ref_write_data.ref_data[0], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
	memset( cur_write_data.cur_write_flg[0], 1, 256 );
	memset( ref_write_data.ref_write_flg[0], 1, 256 );

	// 2~8 line(last 16pix)
	for( loop = 1; loop < 8; loop++ ){
		memcpy( &cur_write_data.cur_data[loop][256 - 16], (VOID*)(D_IM_BMH_TEST_CUR_IMG_ADDR + D_IM_BMH_TEST_IMG_HSIZE * loop - 16), 16 );
		memcpy( &ref_write_data.ref_data[loop][256 - 16], (VOID*)(D_IM_BMH_TEST_REF_IMG_ADDR + D_IM_BMH_TEST_IMG_HSIZE * loop - 16), 16 );
		memset( &cur_write_data.cur_write_flg[loop][256 - 16], 1, 16 );
		memset( &ref_write_data.ref_write_flg[loop][256 - 16], 1, 16 );
	}

	bmh_ctrl.mode = 7;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	memcpy( &cur_write_data.cur_data[8][256 - 16], (VOID*)(D_IM_BMH_TEST_CUR_IMG_ADDR + D_IM_BMH_TEST_IMG_HSIZE * 8 - 16), 16 );
	memcpy( &ref_write_data.ref_data[8][256 - 16], (VOID*)(D_IM_BMH_TEST_REF_IMG_ADDR + D_IM_BMH_TEST_IMG_HSIZE * 8 - 16), 16 );
	memset( &cur_write_data.cur_write_flg[8][256 - 16], 1, 16 );
	memset( &ref_write_data.ref_write_flg[8][256 - 16], 1, 16 );

	cont.cont = 1;
	im_ercd = Im_BMH_Set_Bmhcont( channel_no, &cont );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Bmhcont error = 0x%x\n", im_ercd ));
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_34: "
INT32 CT_Im_Bmh_3_34( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.sadlv = 0xFFFF;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_35: "
INT32 CT_Im_Bmh_3_35( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;
	T_IM_BMH_BMHHCNT cnt;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.sadlv = 0xFFFF;
	cnt.hcnt = 10;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Bmhhcnt( channel_no, &cnt );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Bmhhcnt error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_36: "
INT32 CT_Im_Bmh_3_36( BYTE channel_no )
{
	INT32 im_ercd;
	UINT32 loop;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CUR_BLOCK cur_write_data;
	T_IM_BMH_REF_BLOCK ref_write_data;
	T_IM_BMH_BMHHCNT cnt;

	for( loop = 0; loop < 8; loop++ ){
		memcpy( cur_write_data.cur_data[loop], (VOID*)D_IM_BMH_TEST_CUR_IMG_ADDR, 256 );
		memcpy( ref_write_data.ref_data[loop], (VOID*)D_IM_BMH_TEST_REF_IMG_ADDR, 256 );
		memset( cur_write_data.cur_write_flg[loop], 1, 256 );
		memset( ref_write_data.ref_write_flg[loop], 1, 256 );
	}

	bmh_ctrl.rbs = 3;
	bmh_ctrl.sadlv = 0xFFFF;
	cnt.hcnt = 200;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Bmhhcnt( channel_no, &cnt );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Set Bmhhcnt error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Cur( channel_no, &cur_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Cur error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Set_Ref( channel_no, &ref_write_data );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ref error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


#undef D_IM_BMH_FUNC_NAME
#define D_IM_BMH_FUNC_NAME "CT_Im_Bmh_3_37: "
INT32 CT_Im_Bmh_3_37( BYTE channel_no )
{
	INT32 im_ercd;

	T_IM_BMH_CTRL bmh_ctrl = gct_im_bmh_ctrl;
	T_IM_BMH_CTRL_DMA bmh_dma_ctrl;

	bmh_dma_ctrl.dmamd = 1;
	bmh_dma_ctrl.ghsize_cur = D_IM_BMH_TEST_IMG_HSIZE;
	bmh_dma_ctrl.ghsize_ref = D_IM_BMH_TEST_IMG_HSIZE;
	bmh_dma_ctrl.hsize_ref = D_IM_BMH_TEST_IMG_HSIZE;
	bmh_dma_ctrl.vsize_ref = D_IM_BMH_TEST_IMG_VSIZE;
	bmh_dma_ctrl.curstad = D_IM_BMH_TEST_CUR_IMG_ADDR;
	bmh_dma_ctrl.refstad = D_IM_BMH_TEST_REF_IMG_ADDR;
	bmh_dma_ctrl.sadstad = D_IM_BMH_TEST_RESULT_OUT_ADDR;
	bmh_dma_ctrl.sadghsize = 0x400;

	Ddim_Print(( "%s\n", D_IM_BMH_FUNC_NAME ));

	im_ercd = Im_BMH_Open( channel_no, 100 );	// wai_sem timeout 100 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Open error = 0x%x\n", im_ercd ));
		return im_ercd;
	}

	Im_BMH_Init( channel_no );
	// don't forget Im_BMH_Close() after this line

	im_ercd = Im_BMH_Ctrl( channel_no, &bmh_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Ctrl_Dma( channel_no, &bmh_dma_ctrl );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Ctrl Dma error = 0x%x\n", im_ercd ));
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	im_ercd = Im_BMH_Start_Sync( channel_no, 1000 );	// wait timeout 1000 ticks
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Start Sync error = 0x%x\n", im_ercd ));
		Im_BMH_Stop( channel_no );
		Im_BMH_Close( channel_no );
		return im_ercd;
	}

	ct_im_bmh_print_debug_sadr();

	im_ercd = Im_BMH_Stop( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Stop error = 0x%x\n", im_ercd ));
	}

	im_ercd = Im_BMH_Close( channel_no );
	if( im_ercd != D_DDIM_OK ){
		Ddim_Print(( D_IM_BMH_FUNC_NAME "Close error = 0x%x\n", im_ercd ));
	}
	return im_ercd;
}


static INT32 ct_im_bmh_run_3( const UINT32 ct_no_2nd, const BYTE channel_no )
{
	DDIM_User_L1l2cache_Clean_Flush_All();

	switch( ct_no_2nd ){
		// Test-3-1
		case 1:
			return CT_Im_Bmh_3_1( channel_no );

		// Test-3-2
		case 2:
			return CT_Im_Bmh_3_2( channel_no );

		// Test-3-3
		case 3:
			return CT_Im_Bmh_3_3( channel_no );

		// Test-3-4
		case 4:
			return CT_Im_Bmh_3_4( channel_no );

		// Test-3-5
		case 5:
			return CT_Im_Bmh_3_5( channel_no );

		// Test-3-6
		case 6:
			return CT_Im_Bmh_3_6( channel_no );

		// Test-3-7
		case 7:
			return CT_Im_Bmh_3_7( channel_no );

		// Test-3-8
		case 8:
			return CT_Im_Bmh_3_8( channel_no );

		// Test-3-9
		case 9:
			return CT_Im_Bmh_3_9( channel_no );

		// Test-3-10
		case 10:
			return CT_Im_Bmh_3_10( channel_no );

		// Test-3-11
		case 11:
			return CT_Im_Bmh_3_11( channel_no );

		// Test-3-12
		case 12:
			return CT_Im_Bmh_3_12( channel_no );

		// Test-3-13
		case 13:
			return CT_Im_Bmh_3_13( channel_no );

		// Test-3-14
		case 14:
			return CT_Im_Bmh_3_14( channel_no );

		// Test-3-15
		case 15:
			return CT_Im_Bmh_3_15( channel_no );

		// Test-3-16
		case 16:
			return CT_Im_Bmh_3_16( channel_no );

		// Test-3-17
		case 17:
			return CT_Im_Bmh_3_17( channel_no );

		// Test-3-18
		case 18:
			return CT_Im_Bmh_3_18( channel_no );

		// Test-3-19
		case 19:
			return CT_Im_Bmh_3_19( channel_no );

		// Test-3-20
		case 20:
			return CT_Im_Bmh_3_20( channel_no );

		// Test-3-21
		case 21:
			return CT_Im_Bmh_3_21( channel_no );

		// Test-3-22
		case 22:
			return CT_Im_Bmh_3_22( channel_no );

		// Test-3-23
		case 23:
			return CT_Im_Bmh_3_23( channel_no );

		// Test-3-24
		case 24:
			return CT_Im_Bmh_3_24( channel_no );

		// Test-3-25
		case 25:
			return CT_Im_Bmh_3_25( channel_no );

		// Test-3-26
		case 26:
			return CT_Im_Bmh_3_26( channel_no );

		// Test-3-27
		case 27:
			return CT_Im_Bmh_3_27( channel_no );

		// Test-3-28
		case 28:
			return CT_Im_Bmh_3_28( channel_no );

		// Test-3-29
		case 29:
			return CT_Im_Bmh_3_29( channel_no );

		// Test-3-30
		case 30:
			return CT_Im_Bmh_3_30( channel_no );

		// Test-3-31
		case 31:
			return CT_Im_Bmh_3_31( channel_no );

		// Test-3-32
		case 32:
			return CT_Im_Bmh_3_32( channel_no );

		// Test-3-33
		case 33:
			return CT_Im_Bmh_3_33( channel_no );

		// Test-3-34
		case 34:
			return CT_Im_Bmh_3_34( channel_no );

		// Test-3-35
		case 35:
			return CT_Im_Bmh_3_35( channel_no );

		// Test-3-36
		case 36:
			return CT_Im_Bmh_3_36( channel_no );

		// Test-3-37
		case 37:
			return CT_Im_Bmh_3_37( channel_no );

		default:
			Ddim_Print(( "Error: Unknown command.\n" ));
			break;
	}

	DDIM_User_L1l2cache_Clean_Flush_All();

	return D_DDIM_INPUT_PARAM_ERROR;
}


VOID CT_Im_BMH_Run( const UINT32 ct_idx_1st, const UINT32 ct_idx_2nd, const UINT32 ct_idx_3rd )
{
#ifdef CO_MSG_PRINT_ON
	INT32 result = D_DDIM_INPUT_PARAM_ERROR;
#endif
	static BOOL is_init = 0;

	Ddim_Print(( "CT_Im_BMH_Run( %u, %u, %u ) Begin\n", ct_idx_1st, ct_idx_2nd, ct_idx_3rd ));

	if( is_init == 0 ){
#ifdef CO_DEBUG_ON_PC
		memset( (VOID*)&IO_BMH, '\0', sizeof(IO_BMH) );
//		Dd_Top_Set_CLKSTOP14_BMHAX( 1 );
//		Dd_Top_Set_CLKSTOP14_BMHCK( 1 );
#endif // CO_DEBUG_ON_PC

		if( !(( ct_idx_1st == 1) && (ct_idx_2nd == 1)) ){
			Ddim_Print(( "Im_BMH_Init() Begin\n" ));
			Im_BMH_Init( 0 );
			Im_BMH_Init( 1 );

			Ddim_Print(( "Im_BMH_Init() End\n" ));
			is_init = 1;
		}
	}

#ifdef CO_BMH_DEBUG
	Im_BMH_Print_ClockStatus();
#endif

	switch( ct_idx_1st ){
		case 1:
			D_IM_BMH_RESULT( ct_im_bmh_run_1( ct_idx_2nd, ct_idx_3rd ) );
			break;

		case 2:
			D_IM_BMH_RESULT( ct_im_bmh_run_2( ct_idx_2nd ) );
			break;

		case 3:
			D_IM_BMH_RESULT( ct_im_bmh_run_3( ct_idx_2nd, ct_idx_3rd ) );
			break;

		default:
			Ddim_Print(( "Error: Unknown command.\n" ));
			break;
	}


#ifdef CO_MSG_PRINT_ON
	Ddim_Print(( "result = 0x%x\n", result ));
#endif

#ifdef CO_BMH_DEBUG
	Im_BMH_Print_ClockStatus();
#endif

	Ddim_Print(( "CT_Im_BMH_Run( %u, %u, %u ) End\n", ct_idx_1st, ct_idx_2nd, ct_idx_3rd ));

	return;
}

VOID CT_Im_Bmh_Main( INT32 argc, CHAR** argv )
{
	if( argc < 3 ){
		Ddim_Print(( "CT_Im_Bmh_Main: parameter error.\n" ));
		return;
	}

	CT_Im_BMH_Run( (UINT32)atoi((const char*)argv[1]),
				   (UINT32)atoi((const char*)argv[2]),
				   (UINT32)atoi((const char*)argv[3]));
}

