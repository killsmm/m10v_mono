/**
 * @file		ct_im_xch.c
 * @brief		ct code for im_xch.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "ct_im_xch.h"

#include "im_xch.h"


#include <string.h>
#include <stdlib.h>

#include "ddim_user_custom.h"

#include "jdsxch.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_CT_XCH_TOUT		(3000)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_CT_XCH_PRINT_PTN_COMM = 0,
	E_CT_XCH_PRINT_PTN_THIN,
	E_CT_XCH_PRINT_PTN_HALF_THIN,
	E_CT_XCH_PRINT_PTN_HIST,
	E_CT_XCH_PRINT_PTN_COPY,
	E_CT_XCH_PRINT_PTN_FILL
} E_CT_XCH_PRINT_PTN;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static UCHAR gCT_Xch_Cb0_End_Flag;
static UCHAR gCT_Xch_Cb1_End_Flag;
static ULONG gCT_Xch_Hist_Buffer[256];

/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/
#ifdef D_IM_XCH_PRINT_ST
extern VOID Im_Xch_Print_ClockStatus( VOID );
#endif

static VOID ct_xch_ctrl_print( E_CT_XCH_PRINT_PTN ptn, VOID* ctrl )
{
	T_IM_XCH_CTRL_THIN thin;
	T_IM_XCH_CTRL_HALF_THIN half_thin;
	T_IM_XCH_CTRL_HIST hist;
	T_IM_XCH_CTRL_COPY copy;
	T_IM_XCH_CTRL_CMN com;

	switch( ptn ) {
		case E_CT_XCH_PRINT_PTN_THIN:
			memcpy( &thin, ctrl, sizeof(T_IM_XCH_CTRL_THIN) );
			Ddim_Print(("Thin Setting print start\n"));
			Ddim_Print(("xdtype     [0x%X]\n", thin.xdtype));
			Ddim_Print(("hcyc       [0x%X]\n", thin.hcyc));
			Ddim_Print(("vcyc       [0x%X]\n", thin.vcyc));
			Ddim_Print(("hen        [0x%X]\n", thin.hen));
			Ddim_Print(("ven        [0x%X]\n", thin.ven));
			Ddim_Print(("s_hsize    [%d]\n", thin.s_hsize));
			Ddim_Print(("src_addr   [0x%X]\n", thin.src_addr));
			Ddim_Print(("Thin Setting print end\n"));
			break;
		case E_CT_XCH_PRINT_PTN_HALF_THIN:
			memcpy( &half_thin, ctrl, sizeof(T_IM_XCH_CTRL_HALF_THIN) );
			Ddim_Print(("Half Thin Setting print start\n"));
			Ddim_Print(("vcyc       [0x%X]\n", half_thin.vcyc));
			Ddim_Print(("ven        [0x%X]\n", half_thin.ven));
			Ddim_Print(("s_hsize    [%d]\n", half_thin.s_hsize));
			Ddim_Print(("src_addr   [0x%X]\n", half_thin.src_addr));
			Ddim_Print(("Half Thin Setting print end\n"));
			break;
		case E_CT_XCH_PRINT_PTN_HIST:
			memcpy( &hist, ctrl, sizeof(T_IM_XCH_CTRL_HIST) );
			Ddim_Print(("Hist Setting print start\n"));
			Ddim_Print(("xdtype     [0x%X]\n", hist.xdtype));
			Ddim_Print(("hcyc       [0x%X]\n", hist.hcyc));
			Ddim_Print(("vcyc       [0x%X]\n", hist.vcyc));
			Ddim_Print(("hen        [0x%X]\n", hist.hen));
			Ddim_Print(("ven        [0x%X]\n", hist.ven));
			Ddim_Print(("s_hsize    [%d]\n", hist.s_hsize));
			Ddim_Print(("src_addr   [0x%X]\n", hist.src_addr));
			Ddim_Print(("hcnt       [0x%X]\n", hist.hcnt));
			Ddim_Print(("Hist Setting print end\n"));
			break;
		case E_CT_XCH_PRINT_PTN_COPY:
			memcpy( &copy, ctrl, sizeof(T_IM_XCH_CTRL_COPY) );
			Ddim_Print(("Copy Setting print start\n"));
			Ddim_Print(("vcyc       [0x%X]\n", copy.vcyc));
			Ddim_Print(("ven        [0x%X]\n", copy.ven));
			Ddim_Print(("s_hsize    [%d]\n", copy.s_hsize));
			Ddim_Print(("src_addr   [0x%X]\n", copy.src_addr));
			Ddim_Print(("Copy Setting print end\n"));
			break;
		case E_CT_XCH_PRINT_PTN_COMM:
		default:
			memcpy( &com, ctrl, sizeof(T_IM_XCH_CTRL_CMN) );
			Ddim_Print(("Common Setting print start\n"));
			Ddim_Print(("mode       [0x%X]\n", com.mode));
			Ddim_Print(("g_hsize    [%d]\n", com.g_hsize));
			Ddim_Print(("d_hsize    [%d]\n", com.d_hsize));
			Ddim_Print(("d_vsize    [%d]\n", com.d_vsize));
			Ddim_Print(("dst_addr   [0x%X]\n", com.dst_addr));
			Ddim_Print(("Common Setting print end\n"));
			break;
	}
	return;
}

VOID ct_xch_callback_0( VOID )
{
	gCT_Xch_Cb0_End_Flag = 1;		// process normal end
	Ddim_Print(("----- ct_x0ch_callback -----\n"));
	return;
}

VOID ct_xch_callback_1( VOID )
{
	gCT_Xch_Cb1_End_Flag = 1;		// process normal end
	Ddim_Print(("----- ct_x1ch_callback -----\n"));
	return;
}


/**
 * @brief  Command main function for Xch test.
 * @param  INT32 argc	:The number of parameters
 *		   CGAR** argv	:The value of parameters
 */
VOID Ct_Im_Xch_Main( INT32 argc, CHAR** argv )
{
	INT32 ret;
	INT32 ct_arg_1;
	E_IM_XCH_CH_SEL xch = E_IM_XCH_CH_SEL_0;

	ct_arg_1 = atoi((const char *)argv[1]);

	if( ct_arg_1 != 0
	&&	ct_arg_1 != 10
	&&	ct_arg_1 != 11
	&&	ct_arg_1 != 18
	&&	ct_arg_1 != 20
	&&	ct_arg_1 != 26
	&&	ct_arg_1 != 27
	&&	ct_arg_1 != 28
	&&	ct_arg_1 != 35
	&&	ct_arg_1 != 36
	&&	ct_arg_1 != 37
	 ){
		if( atoi((const char *)argv[2]) == 0 ){
			xch = E_IM_XCH_CH_SEL_0;
		}
		else{
			xch = E_IM_XCH_CH_SEL_1;
		}
	}

	if( argc > 1 ){
		// Im_Xch_Init
		if( ct_arg_1 == 0 ){

#ifdef CO_DEBUG_ON_PC
		memset( (USHORT*)&IO_XCH, 0, sizeof(IO_XCH) );
#endif

#ifdef CO_DEBUG_ON_PC
			IO_XCH.SR.bit.SR = 1;
#endif
			Im_Xch_Init();
			Ddim_Print(( "Im_Xch_Init()\n"));

		}
		// Im_Xch_Open
		else if( ct_arg_1 == 1 ){
			INT32 tmout;

			tmout = atoi((const char *)argv[3]);

			ret = Im_Xch_Open( xch, tmout );

			Ddim_Print(( "Im_Xch_Open() ret = 0x%X\n", ret));

		}
		// Im_Xch_Close
		else if( ct_arg_1 == 2 ){

			ret = Im_Xch_Close( xch );

			Ddim_Print(( "Im_Xch_Close() ret = 0x%X\n", ret));

		}
		// Im_Xch_Ctrl_Common
		else if( ct_arg_1 == 3 ){
			T_IM_XCH_CTRL_CMN ctrl_cmn;

			ctrl_cmn.mode		= atoi((const char *)argv[3]);
			ctrl_cmn.g_hsize	= atoi((const char *)argv[4]);
			ctrl_cmn.d_hsize	= atoi((const char *)argv[5]);
			ctrl_cmn.d_vsize	= atoi((const char *)argv[6]);
			ctrl_cmn.dst_addr	= D_CT_XCH_DST_TOP_ADDR;
			ctrl_cmn.pCallBack	= NULL;

			if( atoi((const char *)argv[7]) == 1 ){
			// Async
				if( xch == E_IM_XCH_CH_SEL_0 ){
					gCT_Xch_Cb0_End_Flag	= 0;
					ctrl_cmn.pCallBack		= (VP_CALLBACK)ct_xch_callback_0;
				}
				else{
					gCT_Xch_Cb1_End_Flag	= 0;
					ctrl_cmn.pCallBack		= (VP_CALLBACK)ct_xch_callback_1;
				}
			}

			// degug print
			ct_xch_ctrl_print( E_CT_XCH_PRINT_PTN_COMM, (VOID *)&ctrl_cmn );

			ret = Im_Xch_Ctrl_Common( xch, &ctrl_cmn );

			Ddim_Print(( "Im_Xch_Ctrl_Common() ret = 0x%X\n", ret));

		}
		// Im_Xch_Ctrl_Thin
		else if( ct_arg_1 == 4 ){
			T_IM_XCH_CTRL_THIN ctrl_thin;
			UINT32 cyc;
			UINT32 ptn;

			ctrl_thin.xdtype	= atoi((const char *)argv[3]);

			// cycle
			cyc = atoi((const char *)argv[4]);
			ctrl_thin.hcyc		= cyc;
			ctrl_thin.vcyc		= cyc;

			// dsp pattern
			ptn = strtoul((const char *)argv[5], NULL, 0);
			ctrl_thin.hen		= ptn;		/* pgr0539 */
			ctrl_thin.ven		= ptn;		/* pgr0539 */

			ctrl_thin.s_hsize	= strtoul((const char *)argv[6], NULL, 0);
			ctrl_thin.src_addr	= D_CT_XCH_SRC_TOP_ADDR;

			// degug print
			ct_xch_ctrl_print( E_CT_XCH_PRINT_PTN_THIN, (VOID *)&ctrl_thin );

			ret = Im_Xch_Ctrl_Thin( xch, &ctrl_thin );

			Ddim_Print(( "Im_Xch_Ctrl_Thin() ret = 0x%X\n", ret));

		}
		// Im_Xch_Ctrl_Hist
		else if( ct_arg_1 == 5 ){
			T_IM_XCH_CTRL_HIST ctrl_hist;
			UINT32 cyc;
			UINT32 ptn;

			ctrl_hist.xdtype	= atoi((const char *)argv[3]);

			// cycle
			cyc = atoi((const char *)argv[4]);
			ctrl_hist.hcyc		= cyc;
			ctrl_hist.vcyc		= cyc;

			// dsp pattern
			ptn = strtoul((const char *)argv[5], NULL, 0);
			ctrl_hist.hen		= ptn;		/* pgr0539 */
			ctrl_hist.ven		= ptn;		/* pgr0539 */

			ctrl_hist.s_hsize		= atoi((const char *)argv[6]);
			ctrl_hist.src_addr		= D_CT_XCH_SRC_TOP_ADDR;
			ctrl_hist.hcnt			= atoi((const char *)argv[7]);

			// degug print
			ct_xch_ctrl_print( E_CT_XCH_PRINT_PTN_HIST, (VOID *)&ctrl_hist );

			ret = Im_Xch_Ctrl_Hist( xch, &ctrl_hist );

			Ddim_Print(( "Im_Xch_Ctrl_Hist() ret = 0x%X\n", ret));

		}
		// Im_Xch_Ctrl_Half_Thin
		else if( ct_arg_1 == 6 ){
			T_IM_XCH_CTRL_HALF_THIN	ctrl_half_thin;
			UINT32 cyc;
			UINT32 ptn;

			// cycle
			cyc = atoi((const char *)argv[3]);
			ctrl_half_thin.vcyc		= cyc;

			// dsp pattern
			ptn = strtoul((const char *)argv[4], NULL, 0);
			ctrl_half_thin.ven		= ptn;	/* pgr0539 */

			ctrl_half_thin.s_hsize	= atoi((const char *)argv[5]);
			ctrl_half_thin.src_addr	= D_CT_XCH_SRC_TOP_ADDR;

			// degug print
			ct_xch_ctrl_print( E_CT_XCH_PRINT_PTN_HALF_THIN, (VOID *)&ctrl_half_thin );

			ret = Im_Xch_Ctrl_Half_Thin( xch, &ctrl_half_thin );

			Ddim_Print(( "Im_Xch_Ctrl_Half_Thin() ret = 0x%X\n", ret));

		}
		// Im_Xch_Ctrl_Copy
		else if( ct_arg_1 == 7 ){
			T_IM_XCH_CTRL_COPY ctrl_cpy;
			UINT32 cyc;
			UINT32 ptn;

			// cycle
			cyc = atoi((const char *)argv[3]);
			ctrl_cpy.vcyc		= cyc;

			// dsp pattern
			ptn = strtoul((const char *)argv[4], NULL, 10);
			ctrl_cpy.ven		= ptn;		/* pgr0539 */

			ctrl_cpy.s_hsize	= atoi((const char *)argv[5]);
			ctrl_cpy.src_addr	= D_CT_XCH_SRC_TOP_ADDR;

			// degug print
			ct_xch_ctrl_print( E_CT_XCH_PRINT_PTN_COPY, (VOID *)&ctrl_cpy );

			ret = Im_Xch_Ctrl_Copy( xch, &ctrl_cpy );

			Ddim_Print(( "Im_Xch_Ctrl_Copy() ret = 0x%X\n", ret));

		}
		// Im_Xch_Ctrl_Fill
		else if( ct_arg_1 == 8 ){
			UCHAR data;

			data = (UCHAR)atoi((const char *)argv[3]);

			ret = Im_Xch_Ctrl_Fill( xch, data );

			Ddim_Print(( "Im_Xch_Ctrl_Fill() ret = 0x%X\n", ret));

		}
		// Im_Xch_Start_Sync
		// Im_Xch_Start_Async
		// Im_Xch_Wait_End
		else if( ct_arg_1 == 9 ){
			UINT32 sync_ptn;

			sync_ptn = atoi((const char *)argv[3]);

			if( sync_ptn == 0 ){
			// Sync
				ret = Im_Xch_Start_Sync( xch );
				Ddim_Print(( "Im_Xch_Start_Sync() ret = 0x%X\n", ret));
			}
			else if( sync_ptn == 1 ){
			// Async wait end
				ret = Im_Xch_Start_Async( xch );
				Ddim_Print(( "Im_Xch_Start_Async() ret = 0x%X\n", ret));

				if( ret == D_IM_XCH_OK ){
#ifdef CO_DEBUG_ON_PC
					if( xch == 0 ){
						IO_XCH.XCHICE.bit.XE0 = 1;
						IO_XCH.XCHICF.bit.__XF0 = 1;
					}
					else{
						IO_XCH.XCHICE.bit.XE1 = 1;
						IO_XCH.XCHICF.bit.__XF1 = 1;
					}
					Im_Xch_Int_Handler();
#endif

					ret = Im_Xch_Wait_End( xch );
					Ddim_Print(( "Im_Xch_Wait_End() ret = 0x%X\n", ret));
				}
			}
			else if( sync_ptn == 2 ){
			// Async callback
				ret = Im_Xch_Start_Async( xch );
				Ddim_Print(( "Im_Xch_Start_Async() ret = 0x%X\n", ret));

				if( ret == D_IM_XCH_OK ){
#ifdef CO_DEBUG_ON_PC
					if( xch == 0 ){
						IO_XCH.XCHICE.bit.XE0 = 1;
						IO_XCH.XCHICF.bit.__XF0 = 1;
					}
					else{
						IO_XCH.XCHICE.bit.XE1 = 1;
						IO_XCH.XCHICF.bit.__XF1 = 1;
					}
					Im_Xch_Int_Handler();
#endif

					if( xch == E_IM_XCH_CH_SEL_0 ){
						while( !gCT_Xch_Cb0_End_Flag ){
							DDIM_User_Tslp_Tsk( 10 );		// wait 10ms
						}
						Ddim_Print(("----- Callback 0ch End -----\n"));
					}
					else{
						while( !gCT_Xch_Cb1_End_Flag ){
							DDIM_User_Tslp_Tsk( 10 );		// wait 10ms
						}
						Ddim_Print(("----- Callback 1ch End -----\n"));
					}
				}
			}
			else{
				Ddim_Print(("input parameter error\n"));
			}
		}
		// Im_Xch_Get_Histogram_Max
		else if( ct_arg_1 == 10 ){
			UCHAR max;

			max = Im_Xch_Get_Histogram_Max();

			Ddim_Print(( "Im_Xch_Get_Histogram_Max() Max = %d\n", max ));

		}
		// Im_Xch_Get_Histogram
		else if( ct_arg_1 == 11 ){

			memset( &gCT_Xch_Hist_Buffer, 0, sizeof(gCT_Xch_Hist_Buffer) );

			ret = Im_Xch_Get_Histogram( gCT_Xch_Hist_Buffer );
			Ddim_Print(( "Im_Xch_Get_Histogram() ret = 0x%X\n", ret ));

			if( ret == D_IM_XCH_OK ){
				Ddim_Print(("***** gCT_Xch_Hist_Buffer[  0] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[0]));
				Ddim_Print(("***** gCT_Xch_Hist_Buffer[ 50] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[50]));
				Ddim_Print(("***** gCT_Xch_Hist_Buffer[100] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[100]));
				Ddim_Print(("***** gCT_Xch_Hist_Buffer[150] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[150]));
				Ddim_Print(("***** gCT_Xch_Hist_Buffer[200] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[200]));
				Ddim_Print(("***** gCT_Xch_Hist_Buffer[250] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[250]));
				Ddim_Print(("***** gCT_Xch_Hist_Buffer[255] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[255]));
			}
		}
		// Im_Xch_Get_Ctrl_Common
		else if( ct_arg_1 == 12 ){
			T_IM_XCH_CTRL_CMN ctrl_cmn;

			memset( &ctrl_cmn, 0, sizeof(ctrl_cmn) );

			ret = Im_Xch_Get_Ctrl_Common( xch, &ctrl_cmn );
			Ddim_Print(( "Im_Xch_Get_Ctrl_Common() ret = 0x%X\n", ret ));

			if( xch == E_IM_XCH_CH_SEL_0 ){
				Ddim_Print(( "IO_XCH.XCHCTL0.XMD    = %d\n"  , ctrl_cmn.mode ));
				Ddim_Print(( "IO_XCH.XCHCTL0.XGHW   = 0x%X\n", ctrl_cmn.g_hsize ));
				Ddim_Print(( "IO_XCH.XCHCTL0.XDHW   = 0x%X\n", ctrl_cmn.d_hsize ));
				Ddim_Print(( "IO_XCH.XCHCTL0.XDVW   = 0x%X\n", ctrl_cmn.d_vsize ));
				Ddim_Print(( "IO_XCH.XCHCTL0.XDA    = 0x%X\n", ctrl_cmn.dst_addr ));
			}
			else{
				Ddim_Print(( "IO_XCH.XCHCTL1.XMD    = %d\n"  , ctrl_cmn.mode ));
				Ddim_Print(( "IO_XCH.XCHCTL1.XGHW   = 0x%X\n", ctrl_cmn.g_hsize ));
				Ddim_Print(( "IO_XCH.XCHCTL1.XDHW   = 0x%X\n", ctrl_cmn.d_hsize ));
				Ddim_Print(( "IO_XCH.XCHCTL1.XDVW   = 0x%X\n", ctrl_cmn.d_vsize ));
				Ddim_Print(( "IO_XCH.XCHCTL1.XDA    = 0x%X\n", ctrl_cmn.dst_addr ));
			}
		}
		// Im_Xch_Get_Ctrl_Thin
		else if( ct_arg_1 == 13 ){
			T_IM_XCH_CTRL_THIN ctrl_thin;

			memset( &ctrl_thin, 0, sizeof(ctrl_thin) );

			ret = Im_Xch_Get_Ctrl_Thin( xch, &ctrl_thin );
			Ddim_Print(( "Im_Xch_Get_Ctrl_Thin() ret = 0x%X\n", ret ));

			if( xch == E_IM_XCH_CH_SEL_0 ){
				Ddim_Print(( "IO_XCH.XCHCTL0.XDTYPE      = %d\n"  , ctrl_thin.xdtype ));
				Ddim_Print(( "IO_XCH.XCHCTL0.XCYC.XHCYC  = 0x%X\n", ctrl_thin.hcyc ));
				Ddim_Print(( "IO_XCH.XCHCTL0.XCYC.XVCYC  = 0x%X\n", ctrl_thin.vcyc ));
				Ddim_Print(( "IO_XCH.XCHCTL0.XHEN        = 0x%X\n", ctrl_thin.hen ));
				Ddim_Print(( "IO_XCH.XCHCTL0.XVEN        = 0x%X\n", ctrl_thin.ven ));
				Ddim_Print(( "IO_XCH.XCHCTL0.XSHW        = 0x%X\n", ctrl_thin.s_hsize ));
				Ddim_Print(( "IO_XCH.XCHCTL0.XSA         = 0x%X\n", ctrl_thin.src_addr ));
			}
			else{
				Ddim_Print(( "IO_XCH.XCHCTL1.XDTYPE      = %d\n"  , ctrl_thin.xdtype ));
				Ddim_Print(( "IO_XCH.XCHCTL1.XCYC.XHCYC  = 0x%X\n", ctrl_thin.hcyc ));
				Ddim_Print(( "IO_XCH.XCHCTL1.XCYC.XVCYC  = 0x%X\n", ctrl_thin.vcyc ));
				Ddim_Print(( "IO_XCH.XCHCTL1.XHEN        = 0x%X\n", ctrl_thin.hen ));
				Ddim_Print(( "IO_XCH.XCHCTL1.XVEN        = 0x%X\n", ctrl_thin.ven ));
				Ddim_Print(( "IO_XCH.XCHCTL1.XSHW        = 0x%X\n", ctrl_thin.s_hsize ));
				Ddim_Print(( "IO_XCH.XCHCTL1.XSA         = 0x%X\n", ctrl_thin.src_addr ));
			}
		}
		// Im_Xch_Get_Ctrl_Hist
		else if( ct_arg_1 == 14 ){
			T_IM_XCH_CTRL_HIST ctrl_hist;

			memset( &ctrl_hist, 0, sizeof(ctrl_hist) );

			ret = Im_Xch_Get_Ctrl_Hist( xch, &ctrl_hist );
			Ddim_Print(( "Im_Xch_Get_Ctrl_Hist() ret = 0x%X\n", ret ));

			Ddim_Print(( "IO_XCH.XCHCTL0.XDTYPE      = %d\n"  , ctrl_hist.xdtype ));
			Ddim_Print(( "IO_XCH.XCHCTL0.XCYC.XHCYC  = 0x%X\n", ctrl_hist.hcyc ));
			Ddim_Print(( "IO_XCH.XCHCTL0.XCYC.XVCYC  = 0x%X\n", ctrl_hist.vcyc ));
			Ddim_Print(( "IO_XCH.XCHCTL0.XHEN        = 0x%X\n", ctrl_hist.hen ));
			Ddim_Print(( "IO_XCH.XCHCTL0.XVEN        = 0x%X\n", ctrl_hist.ven ));
			Ddim_Print(( "IO_XCH.XCHCTL0.XSHW        = 0x%X\n", ctrl_hist.s_hsize ));
			Ddim_Print(( "IO_XCH.XCHCTL0.XSA         = 0x%X\n", ctrl_hist.src_addr ));
			Ddim_Print(( "IO_XCH.XHISTG0.XHCNT0      = %d\n"  , ctrl_hist.hcnt ));
		}
		// Im_Xch_Get_Ctrl_Half_Thin
		else if( ct_arg_1 == 15 ){
			T_IM_XCH_CTRL_HALF_THIN ctrl_half_thin;

			memset( &ctrl_half_thin, 0, sizeof(ctrl_half_thin) );

			ret = Im_Xch_Get_Ctrl_Half_Thin( xch, &ctrl_half_thin );
			Ddim_Print(( "Im_Xch_Get_Ctrl_Half_Thin() ret = 0x%X\n", ret ));

			if( xch == E_IM_XCH_CH_SEL_0 ){
				Ddim_Print(( "IO_XCH.XCHCTL0.XCYC.XVCYC  = 0x%X\n", ctrl_half_thin.vcyc ));
				Ddim_Print(( "IO_XCH.XCHCTL0.XVEN        = 0x%X\n", ctrl_half_thin.ven ));
				Ddim_Print(( "IO_XCH.XCHCTL0.XSHW        = 0x%X\n", ctrl_half_thin.s_hsize ));
				Ddim_Print(( "IO_XCH.XCHCTL0.XSA         = 0x%X\n", ctrl_half_thin.src_addr ));
			}
			else{
				Ddim_Print(( "IO_XCH.XCHCTL1.XCYC.XVCYC  = 0x%X\n", ctrl_half_thin.vcyc ));
				Ddim_Print(( "IO_XCH.XCHCTL1.XVEN        = 0x%X\n", ctrl_half_thin.ven ));
				Ddim_Print(( "IO_XCH.XCHCTL1.XSHW        = 0x%X\n", ctrl_half_thin.s_hsize ));
				Ddim_Print(( "IO_XCH.XCHCTL1.XSA         = 0x%X\n", ctrl_half_thin.src_addr ));
			}

		}
		// Im_Xch_Get_Ctrl_Copy
		else if( ct_arg_1 == 16 ){
			T_IM_XCH_CTRL_COPY ctrl_cpy;

			memset( &ctrl_cpy, 0, sizeof(ctrl_cpy) );

			ret = Im_Xch_Get_Ctrl_Copy( xch, &ctrl_cpy );
			Ddim_Print(( "Im_Xch_Get_Ctrl_Copy() ret = 0x%X\n", ret ));

			if( xch == E_IM_XCH_CH_SEL_0 ){
				Ddim_Print(( "IO_XCH.XCHCTL0.XCYC.XVCYC  = 0x%X\n", ctrl_cpy.vcyc ));
				Ddim_Print(( "IO_XCH.XCHCTL0.XVEN        = 0x%X\n", ctrl_cpy.ven ));
				Ddim_Print(( "IO_XCH.XCHCTL0.XSHW        = 0x%X\n", ctrl_cpy.s_hsize ));
				Ddim_Print(( "IO_XCH.XCHCTL0.XSA         = 0x%X\n", ctrl_cpy.src_addr ));
			}
			else{
				Ddim_Print(( "IO_XCH.XCHCTL1.XCYC.XVCYC  = 0x%X\n", ctrl_cpy.vcyc ));
				Ddim_Print(( "IO_XCH.XCHCTL1.XVEN        = 0x%X\n", ctrl_cpy.ven ));
				Ddim_Print(( "IO_XCH.XCHCTL1.XSHW        = 0x%X\n", ctrl_cpy.s_hsize ));
				Ddim_Print(( "IO_XCH.XCHCTL1.XSA         = 0x%X\n", ctrl_cpy.src_addr ));
			}

		}
		// Im_Xch_Get_Ctrl_Fill
		else if( ct_arg_1 == 17 ){
			UCHAR data;

			ret = Im_Xch_Get_Ctrl_Fill( xch, &data );
			Ddim_Print(( "Im_Xch_Get_Ctrl_Fill() ret = 0x%X\n", ret ));

			if( xch == E_IM_XCH_CH_SEL_0 ){
				Ddim_Print(( "IO_XCH.XCHCTL0.XFDT  = 0x%X\n", data ));
			}
			else{
				Ddim_Print(( "IO_XCH.XCHCTL1.XFDT  = 0x%X\n", data ));
			}

		}
		// Im_Xch_Stop
		else if( ct_arg_1 == 18 ){

			ret = Im_Xch_Stop();
			Ddim_Print(( "Im_Xch_Stop() ret = 0x%X\n", ret ));

		}
		// Im_Xch_Get_Xtrg
		else if( ct_arg_1 == 19 ){
			E_IM_XCH_ST st;

			if( atoi((const char *)argv[3]) == 1 ){
				ret = Im_Xch_Start_Async( xch );
				Ddim_Print(( "Im_Xch_Start_Async() ret = 0x%X\n", ret));
			}

			ret = Im_Xch_Get_Xtrg( xch, &st );
			Ddim_Print(( "Im_Xch_Get_Xtrg() ret = 0x%X\n", ret ));

			if( st == E_IM_XCH_ST_RUNNING ){
				Ddim_Print(( "X%dch Running\n", xch ));
			}
			else{
				Ddim_Print(( "X%dch Stopped\n", xch ));
			}
		}
		// Im_Xch_Int_Handler
		else if( ct_arg_1 == 20 ){

			Im_Xch_Int_Handler();

#ifdef CO_DEBUG_ON_PC
			Ddim_Print(( "IO_XCH.XCHICF.bit.__XF0 = 0x%X\n", IO_XCH.XCHICF.bit.__XF0 ));
			Ddim_Print(( "IO_XCH.XCHICF.bit.__XF1 = 0x%X\n", IO_XCH.XCHICF.bit.__XF1 ));
#endif
		}
		// Im_Xch_Copy
		else if( ct_arg_1 == 21 ){
			UINT32 src_addr;
			UINT32 dst_addr;
			UINT32 src_gl_width;
			UINT32 dst_gl_width;
			UINT32 width;
			UINT32 lines;

			src_addr		= D_CT_XCH_SRC_TOP_ADDR;
			dst_addr		= D_CT_XCH_DST_TOP_ADDR;

			width			= atoi((const char *)argv[3]);
			lines			= atoi((const char *)argv[4]);
			src_gl_width	= width;
			dst_gl_width	= width;

			ret = Im_Xch_Copy( xch, src_addr, src_gl_width, dst_gl_width, dst_addr, width, lines );
			Ddim_Print(( "Im_Xch_Copy() ret = 0x%X\n", ret ));

		}
		// Im_Xch_Copy_Ex_Sync
		// Im_Xch_Copy_Ex_Async
		else if( ct_arg_1 == 22 ){
			T_IM_XCH_COPY_EX	ctrl_cpy_ex;
			E_IM_XCH_COPY_SEL	type;
			UINT32				g_width;
			UINT32				width;
			UINT32				lines;
			UINT32				sync_type;

			if( atoi((const char *)argv[3]) == 0 ){
				type = E_IM_XCH_COPY_SEL_HI;
			}
			else{
				type = E_IM_XCH_COPY_SEL_NORMAL;
			}

			g_width	= atoi((const char *)argv[4]);
			width	= atoi((const char *)argv[5]);
			lines	= atoi((const char *)argv[6]);

			sync_type = atoi((const char *)argv[7]);

			ctrl_cpy_ex.xch					= xch;
			ctrl_cpy_ex.copy_type			= type;
			ctrl_cpy_ex.size.width			= width;
			ctrl_cpy_ex.size.lines			= lines;
			ctrl_cpy_ex.size.src_gl_width	= width;
			ctrl_cpy_ex.size.dst_gl_width	= g_width;
			ctrl_cpy_ex.addr.src			= D_CT_XCH_SRC_TOP_ADDR;
			ctrl_cpy_ex.addr.dst			= D_CT_XCH_DST_TOP_ADDR;

			if( sync_type == 0 ){
			// Sync
				ret = Im_Xch_Copy_Ex_Sync( &ctrl_cpy_ex );
				Ddim_Print(( "Im_Xch_Copy_Ex_Sync() ret = 0x%X\n", ret ));
			}
			else{
			// Async
				ret = Im_Xch_Open( xch, D_CT_XCH_TOUT );
				Ddim_Print(( "Im_Xch_Open() ret = 0x%X\n", ret ));

				ret = Im_Xch_Copy_Ex_Async( &ctrl_cpy_ex, NULL );
				Ddim_Print(( "Im_Xch_Copy_Ex_Async() ret = 0x%X\n", ret ));

#ifdef CO_DEBUG_ON_PC
				if( xch == 0 ){
					IO_XCH.XCHICE.bit.XE0 = 1;
					IO_XCH.XCHICF.bit.__XF0 = 1;
				}
				else{
					IO_XCH.XCHICE.bit.XE1 = 1;
					IO_XCH.XCHICF.bit.__XF1 = 1;
				}
				Im_Xch_Int_Handler();
#endif
				ret = Im_Xch_Wait_End( xch );
				Ddim_Print(( "Im_Xch_Wait_End() ret = 0x%X\n", ret ));

				ret = Im_Xch_Close( xch );
				Ddim_Print(( "Im_Xch_Close() ret = 0x%X\n", ret ));
			}
		}
		// Im_Xch_Fill
		else if( ct_arg_1 == 23 ){
			UINT32 fill_data;
			UINT32 gl_width;
			UINT32 width;
			UINT32 lines;
			UINT32 dst_addr;

			gl_width	= atoi((const char *)argv[3]);
			width		= atoi((const char *)argv[4]);
			lines		= atoi((const char *)argv[5]);
			fill_data	= atoi((const char *)argv[6]);
			dst_addr	= D_CT_XCH_DST_TOP_ADDR;

			ret = Im_Xch_Fill( xch, fill_data, gl_width, dst_addr, width, lines );
			Ddim_Print(( "Im_Xch_Fill() ret = 0x%X\n", ret ));

		}
		// Im_Xch_Fill_Ex_Sync
		// Im_Xch_Fill_Ex_Async
		else if( ct_arg_1 == 24 ){
			T_IM_XCH_FILL_EX ctrl_fill_ex;
			UINT32			 sync_type;

			ctrl_fill_ex.xch			= xch;
			ctrl_fill_ex.size.gl_width	= atoi((const char *)argv[3]);
			ctrl_fill_ex.size.width		= atoi((const char *)argv[4]);
			ctrl_fill_ex.size.lines		= atoi((const char *)argv[5]);
			ctrl_fill_ex.fill_data		= atoi((const char *)argv[6]);
			ctrl_fill_ex.fill_addr		= D_CT_XCH_DST_TOP_ADDR;

			sync_type = atoi((const char *)argv[7]);

			if( sync_type == 0 ){
			// Sync
				ret = Im_Xch_Fill_Ex_Sync( &ctrl_fill_ex );
				Ddim_Print(( "Im_Xch_Fill_Ex_Sync() ret = 0x%X\n", ret ));
			}
			else{
			// Async
				ret = Im_Xch_Open( xch, D_CT_XCH_TOUT );
				Ddim_Print(( "Im_Xch_Open() ret = 0x%X\n", ret ));

				ret = Im_Xch_Fill_Ex_Async( &ctrl_fill_ex, NULL );
				Ddim_Print(( "Im_Xch_Fill_Ex_Async() ret = 0x%X\n", ret ));

#ifdef CO_DEBUG_ON_PC
				if( xch == 0 ){
					IO_XCH.XCHICE.bit.XE0 = 1;
					IO_XCH.XCHICF.bit.__XF0 = 1;
				}
				else{
					IO_XCH.XCHICE.bit.XE1 = 1;
					IO_XCH.XCHICF.bit.__XF1 = 1;
				}
				Im_Xch_Int_Handler();
#endif

				ret = Im_Xch_Wait_End( xch );
				Ddim_Print(( "Im_Xch_Wait_End() ret = 0x%X\n", ret ));

				ret = Im_Xch_Close( xch );
				Ddim_Print(( "Im_Xch_Close() ret = 0x%X\n", ret ));
			}
		}
		// Im_Xch_Convert_Thinning_Sync
		// Im_Xch_Convert_Thinning_Async
		else if( ct_arg_1 == 25 ){
			T_IM_XCH_CONV_THIN	ctrl_thin_ex;
			UINT32				src_gl_width;
			UINT32				dst_gl_width;
			UINT32				width;
			UINT32				lines;
			UINT32			 	sync_type;

			ctrl_thin_ex.xch				= xch;

			switch( atoi((const char *)argv[3]) ){
				case 0:
					ctrl_thin_ex.thin_width	= D_IM_XCH_THIN_SEL_1_1;
					break;
				case 1:
					ctrl_thin_ex.thin_width	= D_IM_XCH_THIN_SEL_1_2;
					break;
				case 2:
					ctrl_thin_ex.thin_width	= D_IM_XCH_THIN_SEL_1_4;
					break;
				case 3:
					ctrl_thin_ex.thin_width	= D_IM_XCH_THIN_SEL_3_4;
					break;
				default:
					Ddim_Print(("input parameter error\n"));
					return;
			}

			switch( atoi((const char *)argv[4]) ){
				case 0:
					ctrl_thin_ex.thin_lines	= D_IM_XCH_THIN_SEL_1_1;
					break;
				case 1:
					ctrl_thin_ex.thin_lines	= D_IM_XCH_THIN_SEL_1_2;
					break;
				case 2:
					ctrl_thin_ex.thin_lines	= D_IM_XCH_THIN_SEL_1_4;
					break;
				case 3:
					ctrl_thin_ex.thin_lines	= D_IM_XCH_THIN_SEL_3_4;
					break;
				default:
					Ddim_Print(("input parameter error\n"));
					return;
			}

			dst_gl_width	= atoi((const char *)argv[5]);
			width			= atoi((const char *)argv[6]);
			lines			= atoi((const char *)argv[7]);
			src_gl_width	= width;

			ctrl_thin_ex.size.width			= width;
			ctrl_thin_ex.size.lines			= lines;
			ctrl_thin_ex.size.src_gl_width	= src_gl_width;
			ctrl_thin_ex.size.dst_gl_width	= dst_gl_width;
			ctrl_thin_ex.addr.src			= D_CT_XCH_SRC_TOP_ADDR;
			ctrl_thin_ex.addr.dst			= D_CT_XCH_DST_TOP_ADDR;

			sync_type = atoi((const char *)argv[8]);

			if( sync_type == 0 ){
			// Sync
				ret = Im_Xch_Convert_Thinning_Sync( &ctrl_thin_ex );
				Ddim_Print(( "Im_Xch_Convert_Thinning_Sync() ret = 0x%X\n", ret ));
			}
			else{
			// Async
				ret = Im_Xch_Open( xch, D_CT_XCH_TOUT );
				Ddim_Print(( "Im_Xch_Open() ret = 0x%X\n", ret ));

				ret = Im_Xch_Convert_Thinning_Async( &ctrl_thin_ex, NULL );
				Ddim_Print(( "Im_Xch_Convert_Thinning_Async() ret = 0x%X\n", ret ));

#ifdef CO_DEBUG_ON_PC
				if( xch == 0 ){
					IO_XCH.XCHICE.bit.XE0 = 1;
					IO_XCH.XCHICF.bit.__XF0 = 1;
				}
				else{
					IO_XCH.XCHICE.bit.XE1 = 1;
					IO_XCH.XCHICF.bit.__XF1 = 1;
				}
				Im_Xch_Int_Handler();
#endif

				ret = Im_Xch_Wait_End( xch );
				Ddim_Print(( "Im_Xch_Wait_End() ret = 0x%X\n", ret ));

				ret = Im_Xch_Close( xch );
				Ddim_Print(( "Im_Xch_Close() ret = 0x%X\n", ret ));
			}
		}
		// Im_Xch_Examine_Histogram_Sync
		// Im_Xch_Examine_Histogram_Async
		else if( ct_arg_1 == 26 ){
			T_IM_XCH_EXAMINE_HIST	ctrl_hist_ex;
			UCHAR					hist_max;
			UINT32					sync_type;

			ctrl_hist_ex.hist_addr	=  D_CT_XCH_SRC_TOP_ADDR;

			ctrl_hist_ex.size.width	= atoi((const char *)argv[2]);
			ctrl_hist_ex.size.lines	= atoi((const char *)argv[3]);

			sync_type	= atoi((const char *)argv[4]);

			memset( &gCT_Xch_Hist_Buffer, 0, sizeof(gCT_Xch_Hist_Buffer) );

			if( sync_type == 0 ){
			// Sync
				ret = Im_Xch_Examine_Histogram_Sync( &ctrl_hist_ex, gCT_Xch_Hist_Buffer, &hist_max );
				Ddim_Print(( "Im_Xch_Examine_Histogram_Sync() ret = 0x%X\n", ret ));

				Ddim_Print(( "Histogram Max = %d\n", hist_max ));
				Ddim_Print(( "***** gCT_Xch_Hist_Buffer[  0] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[0] ));
				Ddim_Print(( "***** gCT_Xch_Hist_Buffer[ 50] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[50] ));
				Ddim_Print(( "***** gCT_Xch_Hist_Buffer[100] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[100] ));
				Ddim_Print(( "***** gCT_Xch_Hist_Buffer[150] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[150] ));
				Ddim_Print(( "***** gCT_Xch_Hist_Buffer[200] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[200] ));
				Ddim_Print(( "***** gCT_Xch_Hist_Buffer[250] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[250] ));
				Ddim_Print(( "***** gCT_Xch_Hist_Buffer[255] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[255] ));
			}
			else{
			// Async
				ret = Im_Xch_Open( E_IM_XCH_CH_SEL_0, D_CT_XCH_TOUT );
				Ddim_Print(( "Im_Xch_Open() ret = 0x%X\n", ret ));

				ret = Im_Xch_Examine_Histogram_Async( &ctrl_hist_ex, NULL );
				Ddim_Print(( "Im_Xch_Examine_Histogram_Async() ret = 0x%X\n", ret ));

#ifdef CO_DEBUG_ON_PC
				if( xch == 0 ){
					IO_XCH.XCHICE.bit.XE0 = 1;
					IO_XCH.XCHICF.bit.__XF0 = 1;
				}
				else{
					IO_XCH.XCHICE.bit.XE1 = 1;
					IO_XCH.XCHICF.bit.__XF1 = 1;
				}
				Im_Xch_Int_Handler();
#endif

				ret = Im_Xch_Wait_End( E_IM_XCH_CH_SEL_0 );
				Ddim_Print(( "Im_Xch_Wait_End() ret = 0x%X\n", ret ));

#ifdef CO_DEBUG_ON_PC
				IO_XCH.XCHCTL0.XTRG.bit.XTRG = 2;
#endif
				hist_max = Im_Xch_Get_Histogram_Max();
				Ddim_Print(( "Im_Xch_Get_Histogram_Max() Histogram Max = %d\n", hist_max ));

				ret = Im_Xch_Get_Histogram( gCT_Xch_Hist_Buffer );
				Ddim_Print(( "Im_Xch_Get_Histogram() ret = 0x%X\n", ret ));

				Ddim_Print(( "***** gCT_Xch_Hist_Buffer[  0] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[0] ));
				Ddim_Print(( "***** gCT_Xch_Hist_Buffer[ 50] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[50] ));
				Ddim_Print(( "***** gCT_Xch_Hist_Buffer[100] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[100] ));
				Ddim_Print(( "***** gCT_Xch_Hist_Buffer[150] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[150] ));
				Ddim_Print(( "***** gCT_Xch_Hist_Buffer[200] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[200] ));
				Ddim_Print(( "***** gCT_Xch_Hist_Buffer[250] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[250] ));
				Ddim_Print(( "***** gCT_Xch_Hist_Buffer[255] = 0x%lX *****\n", gCT_Xch_Hist_Buffer[255] ));

				ret = Im_Xch_Close( E_IM_XCH_CH_SEL_0 );
				Ddim_Print(( "Im_Xch_Close() ret = 0x%X\n", ret ));
			}

		}
		// Im_Xch_Copy_Dual_Channel
		else if( ct_arg_1 == 27 ){
			UINT32	width;
			UINT32	lines;
			UINT32	src_addr;
			UINT32	dst_addr;
			INT32	size;

			width		= atoi((const char *)argv[2]);
			lines		= atoi((const char *)argv[3]);
			size		= width * lines;

			src_addr	= D_CT_XCH_SRC_TOP_ADDR;
			dst_addr	= D_CT_XCH_DST_TOP_ADDR;

			ret = Im_Xch_Copy_Dual_Channel( (VOID *)dst_addr, (VOID *)src_addr, size );
			Ddim_Print(( "Im_Xch_Copy_Dual_Channel() ret = 0x%X\n", ret ));
		}
		// Parameter Check
		else if( ct_arg_1 == 28 ){
			T_IM_XCH_CTRL_CMN		xch_ctrl_cmn;
			T_IM_XCH_CTRL_THIN		xch_ctrl_thin;
			T_IM_XCH_CTRL_HIST		xch_ctrl_hist;
			T_IM_XCH_CTRL_HALF_THIN	xch_ctrl_half_thin;
			T_IM_XCH_CTRL_COPY		xch_ctrl_copy;
			T_IM_XCH_EXAMINE_HIST	xch_hist_param;
			T_IM_XCH_CONV_THIN		xch_thin_param;
			UCHAR					hist_max;

			memset( &xch_ctrl_cmn, 0, sizeof(T_IM_XCH_CTRL_CMN) );
			memset( &xch_ctrl_thin, 0, sizeof(T_IM_XCH_CTRL_THIN) );
			memset( &xch_ctrl_hist, 0, sizeof(T_IM_XCH_CTRL_HIST) );
			memset( &xch_ctrl_half_thin, 0, sizeof(T_IM_XCH_CTRL_HALF_THIN) );
			memset( &xch_ctrl_copy, 0, sizeof(T_IM_XCH_CTRL_COPY) );
			memset( &xch_hist_param, 0, sizeof(T_IM_XCH_EXAMINE_HIST) );
			memset( &xch_thin_param, 0, sizeof(T_IM_XCH_CONV_THIN) );

			// Im_Xch_Open
			ret = Im_Xch_Open( E_IM_XCH_CH_SEL_0, -2 );
			Ddim_Print(( "Im_Xch_Open():                    ret = 0x%X\n", ret ));

			// Im_Xch_Ctrl_Common
			ret = Im_Xch_Ctrl_Common( E_IM_XCH_CH_SEL_0, NULL );
			Ddim_Print(( "Im_Xch_Ctrl_Common():             ret = 0x%X\n", ret ));

			xch_ctrl_cmn.mode = D_IM_XCH_MD_HISTGRAM_DOWN;
			ret = Im_Xch_Ctrl_Common( E_IM_XCH_CH_SEL_1, &xch_ctrl_cmn );
			Ddim_Print(( "Im_Xch_Ctrl_Common():             ret = 0x%X\n", ret ));

			xch_ctrl_cmn.mode = D_IM_XCH_MD_HISTGRAM;
			ret = Im_Xch_Ctrl_Common( E_IM_XCH_CH_SEL_1, &xch_ctrl_cmn );
			Ddim_Print(( "Im_Xch_Ctrl_Common():             ret = 0x%X\n", ret ));

			// Im_Xch_Ctrl_Thin
			ret = Im_Xch_Ctrl_Thin( E_IM_XCH_CH_SEL_0, NULL );
			Ddim_Print(( "Im_Xch_Ctrl_Thin():               ret = 0x%X\n", ret ));

			// Im_Xch_Ctrl_Hist
			ret = Im_Xch_Ctrl_Hist( E_IM_XCH_CH_SEL_0, NULL );
			Ddim_Print(( "Im_Xch_Ctrl_Hist():               ret = 0x%X\n", ret ));

			ret = Im_Xch_Ctrl_Hist( E_IM_XCH_CH_SEL_1, &xch_ctrl_hist );
			Ddim_Print(( "Im_Xch_Ctrl_Hist():               ret = 0x%X\n", ret ));

			// Im_Xch_Ctrl_Half_Thin
			ret = Im_Xch_Ctrl_Half_Thin( E_IM_XCH_CH_SEL_0, NULL );
			Ddim_Print(( "Im_Xch_Ctrl_Half_Thin():          ret = 0x%X\n", ret ));

			// Im_Xch_Ctrl_Copy
			ret = Im_Xch_Ctrl_Copy( E_IM_XCH_CH_SEL_0, NULL );
			Ddim_Print(( "Im_Xch_Ctrl_Copy():               ret = 0x%X\n", ret ));

			// Im_Xch_Ctrl_Copy
			ret = Im_Xch_Get_Histogram( NULL );
			Ddim_Print(( "Im_Xch_Get_Histogram():           ret = 0x%X\n", ret ));

			// Im_Xch_Get_Ctrl_Common
			ret = Im_Xch_Get_Ctrl_Common( E_IM_XCH_CH_SEL_0, NULL );
			Ddim_Print(( "Im_Xch_Get_Ctrl_Common():         ret = 0x%X\n", ret ));

			// Im_Xch_Get_Ctrl_Thin
			ret = Im_Xch_Get_Ctrl_Thin( E_IM_XCH_CH_SEL_0, NULL );
			Ddim_Print(( "Im_Xch_Get_Ctrl_Thin():           ret = 0x%X\n", ret ));

			// Im_Xch_Get_Ctrl_Hist
			ret = Im_Xch_Get_Ctrl_Hist( E_IM_XCH_CH_SEL_0, NULL );
			Ddim_Print(( "Im_Xch_Get_Ctrl_Hist():           ret = 0x%X\n", ret ));

			ret = Im_Xch_Get_Ctrl_Hist( E_IM_XCH_CH_SEL_1, &xch_ctrl_hist );
			Ddim_Print(( "Im_Xch_Get_Ctrl_Hist():           ret = 0x%X\n", ret ));

			// Im_Xch_Get_Ctrl_Half_Thin
			ret = Im_Xch_Get_Ctrl_Half_Thin( E_IM_XCH_CH_SEL_0, NULL );
			Ddim_Print(( "Im_Xch_Get_Ctrl_Half_Thin():      ret = 0x%X\n", ret ));

			// Im_Xch_Get_Ctrl_Copy
			ret = Im_Xch_Get_Ctrl_Copy( E_IM_XCH_CH_SEL_0, NULL );
			Ddim_Print(( "Im_Xch_Get_Ctrl_Copy():           ret = 0x%X\n", ret ));

			// Im_Xch_Get_Ctrl_Half_Thin
			ret = Im_Xch_Get_Ctrl_Half_Thin( E_IM_XCH_CH_SEL_0, NULL );
			Ddim_Print(( "Im_Xch_Get_Ctrl_Half_Thin():      ret = 0x%X\n", ret ));

			// Im_Xch_Get_Ctrl_Fill
			ret = Im_Xch_Get_Ctrl_Fill( E_IM_XCH_CH_SEL_0, NULL );
			Ddim_Print(( "Im_Xch_Get_Ctrl_Fill():           ret = 0x%X\n", ret ));

			// Im_Xch_Get_Xtrg
			ret = Im_Xch_Get_Xtrg( E_IM_XCH_CH_SEL_0, NULL );
			Ddim_Print(( "Im_Xch_Get_Xtrg():                ret = 0x%X\n", ret ));

			// Im_Xch_Copy_Ex_Sync
			ret = Im_Xch_Copy_Ex_Sync( NULL );
			Ddim_Print(( "Im_Xch_Copy_Ex_Sync():            ret = 0x%X\n", ret ));

			// Im_Xch_Copy_Ex_Async
			ret = Im_Xch_Copy_Ex_Async( NULL, NULL );
			Ddim_Print(( "Im_Xch_Copy_Ex_Async():           ret = 0x%X\n", ret ));

			// Im_Xch_Fill_Ex_Sync
			ret = Im_Xch_Fill_Ex_Sync( NULL );
			Ddim_Print(( "Im_Xch_Fill_Ex_Sync():            ret = 0x%X\n", ret ));

			// Im_Xch_Fill_Ex_Async
			ret = Im_Xch_Fill_Ex_Async( NULL, NULL );
			Ddim_Print(( "Im_Xch_Fill_Ex_Async():           ret = 0x%X\n", ret ));

			// Im_Xch_Examine_Histogram_Sync
			ret = Im_Xch_Examine_Histogram_Sync( NULL, NULL, &hist_max );
			Ddim_Print(( "Im_Xch_Examine_Histogram_Sync():  ret = 0x%X\n", ret ));

			ret = Im_Xch_Examine_Histogram_Sync( &xch_hist_param, NULL, NULL );
			Ddim_Print(( "Im_Xch_Examine_Histogram_Sync():  ret = 0x%X\n", ret ));

			// Im_Xch_Examine_Histogram_Async
			ret = Im_Xch_Examine_Histogram_Async( NULL, NULL );
			Ddim_Print(( "Im_Xch_Examine_Histogram_Async(): ret = 0x%X\n", ret ));

			// Im_Xch_Convert_Thinning_Sync
			ret = Im_Xch_Convert_Thinning_Sync( NULL );
			Ddim_Print(( "Im_Xch_Convert_Thinning_Sync():   ret = 0x%X\n", ret ));

			// Im_Xch_Convert_Thinning_Async
			ret = Im_Xch_Convert_Thinning_Async( NULL, NULL );
			Ddim_Print(( "Im_Xch_Convert_Thinning_Async():  ret = 0x%X\n", ret ));

		}
		// Im_Xch_Start_Sync
		else if( ct_arg_1 == 29 ){

			ret = Im_Xch_Start_Sync( xch );
			Ddim_Print(( "Im_Xch_Start_Sync() ret = 0x%X\n", ret));

		}
		// Im_Xch_Start_Async
		else if( ct_arg_1 == 30 ){

			ret = Im_Xch_Start_Async( xch );
			Ddim_Print(( "Im_Xch_Start_Async() ret = 0x%X\n", ret));

		}
		// Im_Xch_Wait_End
		else if( ct_arg_1 == 31 ){

			ret = Im_Xch_Wait_End( xch );
			Ddim_Print(( "Im_Xch_Wait_End() ret = 0x%X\n", ret));

		}
		// for Macro busy
		else if( ct_arg_1 == 32 ){
			INT32 ct_arg_2;

			ct_arg_2 = atoi((const char *)argv[3]);

			ret = Im_Xch_Start_Async( xch );
			Ddim_Print(( "Im_Xch_Start_Async() ret = 0x%X\n", ret));

			switch( ct_arg_2 ){
				case 0:
					ret = Im_Xch_Start_Sync( xch );
					Ddim_Print(( "Im_Xch_Start_Sync() ret = 0x%X\n", ret));
					break;
				case 1:
					ret = Im_Xch_Start_Async( xch );
					Ddim_Print(( "Im_Xch_Start_Async() ret = 0x%X\n", ret));
					break;
				case 2:
					ret = Im_Xch_Get_Histogram(gCT_Xch_Hist_Buffer);
					Ddim_Print(( "Im_Xch_Get_Histogram() ret = 0x%X\n", ret));
					break;
				default:
					Ddim_Print(("input parameter error\n"));
					return;
			}
		}
		// src area init
		else if( ct_arg_1 == 35 ){
			T_IM_XCH_FILL_EX ctrl_fill_ex;
			UINT32 ct_arg_2;
			UINT32 ct_arg_3;
			UCHAR dat;

			dat			= atoi((const char *)argv[2]);
			ct_arg_2	= atoi((const char *)argv[3]);
			ct_arg_3	= atoi((const char *)argv[4]);

			ctrl_fill_ex.xch			= E_IM_XCH_CH_SEL_0;

			// Set fill data
			if( dat == 0 ){
				ctrl_fill_ex.fill_data		= 0x00;
			}
			else{
				ctrl_fill_ex.fill_data		= dat;
			}

			if( ct_arg_2 == 0 ){
				ctrl_fill_ex.size.gl_width	= 0x4000;
				ctrl_fill_ex.size.width		= ctrl_fill_ex.size.gl_width;
				ctrl_fill_ex.size.lines		= 0x800;
			}
			else{
				ctrl_fill_ex.size.gl_width	= ct_arg_2;
				ctrl_fill_ex.size.width		= ctrl_fill_ex.size.gl_width;
				ctrl_fill_ex.size.lines		= ct_arg_3;
			}

			ctrl_fill_ex.fill_addr		= D_CT_XCH_SRC_TOP_ADDR;

			// Sync
			ret = Im_Xch_Fill_Ex_Sync( &ctrl_fill_ex );
			Ddim_Print(( "Im_Xch_Fill_Ex_Sync() ret = 0x%X\n", ret ));
			Ddim_Print(("src area clear\n"));
		}
		// dst area init
		else if( ct_arg_1 == 36 ){
			T_IM_XCH_FILL_EX ctrl_fill_ex;
			UINT32 ct_arg_2;
			UINT32 ct_arg_3;
			INT32 dat;

			dat			= atoi((const char *)argv[2]);
			ct_arg_2	= atoi((const char *)argv[3]);
			ct_arg_3	= atoi((const char *)argv[4]);

			ctrl_fill_ex.xch			= E_IM_XCH_CH_SEL_0;

			// Set fill data
			if( dat == 0 ){
				ctrl_fill_ex.fill_data		= 0x00;
			}
			else{
				ctrl_fill_ex.fill_data		= (UCHAR)dat;
			}

			if( ct_arg_2 == 0 ){
				ctrl_fill_ex.size.gl_width	= 0x4000;
				ctrl_fill_ex.size.width		= ctrl_fill_ex.size.gl_width;
				ctrl_fill_ex.size.lines		= 0x800;
			}
			else{
				ctrl_fill_ex.size.gl_width	= ct_arg_2;
				ctrl_fill_ex.size.width		= ctrl_fill_ex.size.gl_width;
				ctrl_fill_ex.size.lines		= ct_arg_3;
			}

			ctrl_fill_ex.fill_addr		= D_CT_XCH_DST_TOP_ADDR;

			// Sync
			ret = Im_Xch_Fill_Ex_Sync( &ctrl_fill_ex );
			Ddim_Print(( "Im_Xch_Fill_Ex_Sync() ret = 0x%X\n", ret ));
			Ddim_Print(("dst area clear\n"));

		}
		// Initialize register for PC debug
		else if( ct_arg_1 == 37 ){
#ifdef CO_DEBUG_ON_PC
			IO_XCH.XCHCTL0.XDTYPE.bit.XSDTYPE	= 1;
			IO_XCH.XCHCTL0.XDTYPE.bit.XDDTYPE	= 1;
			IO_XCH.XCHCTL0.XMD.bit.XMD			= 7;
			IO_XCH.XCHCTL0.XGHW.bit.XGHW		= 0xFFF0;
			IO_XCH.XCHCTL0.XDHW.bit.XDHW		= 0x7FFF;
			IO_XCH.XCHCTL0.XDVW.bit.XDVW		= 0x3FFF;
			IO_XCH.XCHCTL0.XCYC.bit.XHCYC		= 0x1F;
			IO_XCH.XCHCTL0.XCYC.bit.XVCYC		= 0x1F;
			IO_XCH.XCHCTL0.XHEN.bit.XHEN		= 0xFFFFFFFF;
			IO_XCH.XCHCTL0.XVEN.bit.XVEN		= 0xFFFFFFFF;
			IO_XCH.XCHCTL0.XSHW.bit.XSHW		= 0xFFF0;
			IO_XCH.XCHCTL0.XSA.bit.XSA			= 0xFFFFFFFF;
			IO_XCH.XCHCTL0.XDA.bit.XDA			= 0xFFFFFFFF;
			IO_XCH.XCHCTL0.XFDT.bit.XFDT		= 0xFF;
			IO_XCH.XHISTG0.bit.XHCNT0			= 0x1;

			IO_XCH.XCHCTL1.XDTYPE.bit.XSDTYPE	= 0;
			IO_XCH.XCHCTL1.XDTYPE.bit.XDDTYPE	= 0;
			IO_XCH.XCHCTL1.XMD.bit.XMD			= 0;
			IO_XCH.XCHCTL1.XGHW.bit.XGHW		= 0x0;
			IO_XCH.XCHCTL1.XDHW.bit.XDHW		= 0x0;
			IO_XCH.XCHCTL1.XDVW.bit.XDVW		= 0x0;
			IO_XCH.XCHCTL1.XCYC.bit.XHCYC		= 0x0;
			IO_XCH.XCHCTL1.XCYC.bit.XVCYC		= 0x0;
			IO_XCH.XCHCTL1.XHEN.bit.XHEN		= 0x1;
			IO_XCH.XCHCTL1.XVEN.bit.XVEN		= 0x1;
			IO_XCH.XCHCTL1.XSHW.bit.XSHW		= 0x0;
			IO_XCH.XCHCTL1.XSA.bit.XSA			= 0x0;
			IO_XCH.XCHCTL1.XDA.bit.XDA			= 0x0;
			IO_XCH.XCHCTL1.XFDT.bit.XFDT		= 0x0;
#endif
		}
		// Im_Xch_Fill
		else if( ct_arg_1 == 38 ){
			UINT32 fill_data;
			UINT32 gl_width;
			UINT32 width;
			UINT32 lines;
			UINT32 dst_addr;

			gl_width	= atoi((const char *)argv[3]);
			width		= atoi((const char *)argv[4]);
			lines		= atoi((const char *)argv[5]);
			fill_data	= atoi((const char *)argv[6]);
			dst_addr = strtoul((const char *)argv[7], NULL, 0);

			ret = Im_Xch_Fill( xch, fill_data, gl_width, dst_addr, width, lines );		/* pgr0539 */
			Ddim_Print(( "Im_Xch_Fill() ret = 0x%X\n", ret ));

		}
		// Error
		else{
			Ddim_Print(("input parameter error\n"));
		}
	}

#ifdef D_IM_XCH_PRINT_ST
	Im_Xch_Print_ClockStatus();
#endif

	Ddim_Print(("xch command complete\n"));

	DDIM_User_L1l2cache_Clean_Flush_All();
	return;
}

