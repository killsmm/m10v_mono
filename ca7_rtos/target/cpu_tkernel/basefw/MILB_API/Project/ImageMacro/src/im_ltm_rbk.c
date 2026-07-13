/**
 * @file		im_ltm_rbk.c
 * @brief		Local Tone Mapping process(Resize Block)
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


#include "im_ltm.h"

#include <string.h>
#include "dd_arm.h"

#include "jdsltm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

#define Im_LTM_Dsb()	Dd_ARM_Dsb_Pou()

// SW reset
#define	D_IM_LTM_SR_RELEASE				(0)						/**< SW reset release				*/
#define	D_IM_LTM_SR_RESET				(1)						/**< SW reset state					*/

// RSRAMEN value
#define D_IM_LTM_RBK_RSRAMEN_ENABLE		(0x000003B1)
#define D_IM_LTM_RBK_RSRAMEN_DISABLE	(0x00000000)

// PIPE count
#define D_IM_LTM_PIPE_COUNT				(2)

// RINT0 value
#define D_IM_LTM_RBK_RINT0_ENABLE		(0x00311011)
#define D_IM_LTM_RBK_RINT0_DISABLE		(0x00000000)
// RINT1 value
#define D_IM_LTM_RBK_RINT1_ALL_CLR		(0x00311011)

// register bit pattern
#define D_IM_LTM_RBK_TRG_OFF			(0)
#define D_IM_LTM_RBK_TRG_ON				(1)
#define D_IM_LTM_RBK_TRG_IDLE			(2)
#define D_IM_LTM_RBK_TRG_BUSY			(3)

// RTHSTA register value
#define D_IM_LTM_RBK_RTHSTA_MIN			(0)
#define D_IM_LTM_RBK_RTHSTA_MAX			(845)

// RTVSTA register value
#define D_IM_LTM_RBK_RTVSTA_MIN			(0)
#define D_IM_LTM_RBK_RTVSTA_MAX			(1014)

// RTHSIZ register value
#define D_IM_LTM_RBK_RTHSIZ_MIN			(8)
#define D_IM_LTM_RBK_RTHSIZ_MAX			(854)

// RTVSIZ register value
#define D_IM_LTM_RBK_RTVSIZ_MIN			(8)
#define D_IM_LTM_RBK_RTVSIZ_MAX			(1022)

// Input mode
#define D_IM_LTM_RBK_MODE_SRO_DIRECT	(0)
#define D_IM_LTM_RBK_MODE_SDRAM_INPUT	(2)

#define D_IM_LTM_RBK_SRAM_WAIT_USEC		(1)

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#define im_ltm_rbk_max( a, b )						(((a) > (b)) ? (a):(b))
#define im_ltm_rbk_min( a, b )						(((a) < (b)) ? (a):(b))

#define im_ltm_rbk_check_target_pipe_no_1(a)		(((a) + 1) & (D_IM_LTM_PIPE1 + 1))
#define im_ltm_rbk_check_target_pipe_no_2(a)		(((a) + 1) & (D_IM_LTM_PIPE2 + 1))


// wait usec
#define im_ltm_rbk_wait_usec( usec )			Dd_ARM_Wait_ns( (usec) * 1000 )


#ifdef CO_ASSERT_ON
#define M_IM_LTM_RBK_ASSETION_MSG( msg )		(msg)
#else
#define M_IM_LTM_RBK_ASSETION_MSG( msg )		(NULL)
#endif


#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
// Nothing Special

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

// control information
typedef struct {
	T_IM_LTM_RECT			input_size;						// input size
	UINT32					int_status;						// Interrupt status
	UCHAR					resolution_conversion;			// 0:off,1:on
	VOID					(*rbk_user_handler)(UINT32* int_status, UINT32 user_param);	// Interrupt Handler
	UINT32					user_param;						// return to callback argument value when interrupt occurs
} T_IM_LTM_RBK_MNG;

// Interrupt parameter table
typedef struct {
	ULONG				flg_bitmask;		// Bitmask of interrupt flag.
	ULONG				ena_bitmask;		// Bitmask of inaerrupt enable.
	ULONG				waitflg;			// Value of set_flg.
	ULONG				int_status;			// Value of gIM_LTM_RBK_Mng.int_status.
} T_IM_LTM_RBK_INTFLG_TBL;


#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
static volatile T_IM_LTM_RBK_MNG gIM_LTM_RBK_Mng[D_IM_LTM_PIPE_COUNT];
static volatile struct io_ltm_ch_rbk* gIM_Io_Ltm_Rbk_Reg_Ptr[D_IM_LTM_PIPE_MAX] = { &(IO_LTM_P1.RBK), &(IO_LTM_P2.RBK), &(IO_LTM_P3.RBK) };


//
// Interrupt parameter table
//
static const T_IM_LTM_RBK_INTFLG_TBL gim_ltm_rbk_inttbl[D_IM_LTM_PIPE_COUNT][6] = {
	{
		{ 0x00000001, 0x00000001, D_IM_LTM_RBK1_INT_STATE_RBK_END,   D_IM_LTM_RBK1_INT_FLG_RBK_END   },	// FINEN
		{ 0x00000010, 0x00000010, D_IM_LTM_RBK1_INT_STATE_FRAME_END, D_IM_LTM_RBK1_INT_FLG_FRAME_END },	// FRMEN
		{ 0x00001000, 0x00001000, D_IM_LTM_RBK1_INT_STATE_LINE_END,  D_IM_LTM_RBK1_INT_FLG_LINE_END  },	// LCNTEN
		{ 0x00010000, 0x00010000, D_IM_LTM_RBK1_INT_STATE_WAIT_ERR,  D_IM_LTM_RBK1_INT_FLG_WAIT_ERR  },	// WAITEN
		{ 0x00100000, 0x00100000, D_IM_LTM_RBK1_INT_STATE_AXR_ERR,   D_IM_LTM_RBK1_INT_FLG_AXR_ERR   },	// AXREEN
		{ 0x00200000, 0x00200000, D_IM_LTM_RBK1_INT_STATE_AXW_ERR,   D_IM_LTM_RBK1_INT_FLG_AXW_ERR   },	// AXWEEN
	},
	{
		{ 0x00000001, 0x00000001, D_IM_LTM_RBK2_INT_STATE_RBK_END,   D_IM_LTM_RBK2_INT_FLG_RBK_END   },	// FINEN
		{ 0x00000010, 0x00000010, D_IM_LTM_RBK2_INT_STATE_FRAME_END, D_IM_LTM_RBK2_INT_FLG_FRAME_END },	// FRMEN
		{ 0x00001000, 0x00001000, D_IM_LTM_RBK2_INT_STATE_LINE_END,  D_IM_LTM_RBK2_INT_FLG_LINE_END  },	// LCNTEN
		{ 0x00010000, 0x00010000, D_IM_LTM_RBK2_INT_STATE_WAIT_ERR,  D_IM_LTM_RBK2_INT_FLG_WAIT_ERR  },	// WAITEN
		{ 0x00100000, 0x00100000, D_IM_LTM_RBK2_INT_STATE_AXR_ERR,   D_IM_LTM_RBK2_INT_FLG_AXR_ERR   },	// AXREEN
		{ 0x00200000, 0x00200000, D_IM_LTM_RBK2_INT_STATE_AXW_ERR,   D_IM_LTM_RBK2_INT_FLG_AXW_ERR   },	// AXWEEN
	},
};


#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE


/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/* Reset internal value
 */
static VOID im_ltm_rbk_manager_init( UCHAR pipe_no )
{
	UCHAR size_coef = 1;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "im_ltm_rbk_manager_init error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return;
	}
#endif	// CO_PARAM_CHECK

	if( pipe_no == D_IM_LTM_PIPE12 ){
		size_coef = 2;
		pipe_no = 0;
	}

	// Initialize of State & Handler management information
	memset( (USHORT*)&gIM_LTM_RBK_Mng[pipe_no], 0, sizeof(T_IM_LTM_RBK_MNG) * size_coef );

	return;
}

#ifdef IM_LTM_RBK_STATUS_PRINT
// Calculate LTM RBK output image width
static UINT32 im_ltm_rbk_calc_out_width( UCHAR pipe_no )
{
	UINT32 in_pixs;
	UINT32 out_pixs;
	UINT32 ref_pixs;

	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Print(( "im_ltm_rbk_calc_out_width:pipe_no err\n" ));
		return 0;
	}

	Im_LTM_On_Pclk(pipe_no);
	if( gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RIOCTL.bit.INSEL == D_IM_LTM_RBK_MODE_SRO_DIRECT ) {
		if( gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RIOCTL.bit.SROMD == D_IM_LTM_RBK_SRO_DIRECT_1 ){
			in_pixs = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->BTHSIZ0.word;
		}
		else{
			in_pixs = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->BTHSIZ0.word + gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->BTHSIZ1.word;
		}
	}
	else {
		in_pixs = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RSDRHSZ.word;
	}

	if( gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->HVEN.bit.HVEN == D_IM_LTM_ENABLE_ON ){
		pixs = pixs >> 1;
	}

	if( gIM_LTM_RBK_Mng[pipe_no].resolution_conversion == D_IM_LTM_ENABLE_ON ){
		pixs = pixs >> 1;
	}

	if( gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RTEN.bit.RTEN == D_IM_LTM_ENABLE_ON ){
		if( gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RTHSIZ.word > pixs ){
			pixs = 0xFFFF;
		}
		else{
			pixs = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RTHSIZ.word;
		}
	}

	Im_LTM_Off_Pclk(pipe_no);

	return out_pixs;
}

// Calculate LTM RBK output image lines
static UINT32 im_ltm_rbk_calc_out_lines( UCHAR pipe_no )
{
	UINT32 pixs;

	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Print(( "im_ltm_rbk_calc_out_lines:pipe_no err\n" ));
		return 0;
	}

	Im_LTM_On_Pclk(pipe_no);
	if( gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RIOCTL.bit.INSEL == D_IM_LTM_RBK_MODE_SRO_DIRECT ) {
		if( gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RIOCTL.bit.SROMD == D_IM_LTM_RBK_SRO_DIRECT_1 ){
			in_pixs = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->BTVSIZ0.word;
		}
		else{
			in_pixs = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->BTVSIZ0.word;	//  = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->BTVSIZ1.word
		}
	}
	else {
		in_pixs = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RSDRVSZ.word;
	}

	if( gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->HVEN.bit.HVEN == D_IM_LTM_ENABLE_ON ){
		pixs = pixs >> 1;
	}

	if( gIM_LTM_RBK_Mng[pipe_no].resolution_conversion == D_IM_LTM_ENABLE_ON ){
		pixs = pixs >> 1;
	}

	if( gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RDCMD.bit.RDCEN == D_IM_LTM_ENABLE_ON ){
		pixs = pixs >> ( gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RDCMD.bit.RDCMD + 1 );
	}

	if( gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RTEN.bit.RTEN == D_IM_LTM_ENABLE_ON ){
		if( gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RTVSIZ.word > pixs ){
			pixs = 0xFFFF;
		}
		else{
			pixs = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RTVSIZ.word;
		}
	}

	Im_LTM_Off_Pclk(pipe_no);

	return pixs;
}

#endif

static VOID im_ltm_rbk_get_loop_val( UCHAR pipe_no, UCHAR* loop_sta, UCHAR* loop_end )
{
	switch( pipe_no ){
		case D_IM_LTM_PIPE1:
			*loop_sta = 0;
			*loop_end = 0;
			break;
		case D_IM_LTM_PIPE2:
			*loop_sta = 1;
			*loop_end = 1;
			break;
//		case D_IM_LTM_PIPE12:
		default:
			*loop_sta = 0;
			*loop_end = 1;
			break;
	}
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif	// CO_DDIM_UTILITY_USE

/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

/* LTM_RBK Initialize
 */
INT32 Im_LTM_RBK_Init( UCHAR pipe_no )
{
	ULONG clksel[2] = { 0, 0 };
	UCHAR clkstop[2][3] = {{ 0, 0, 0 }, { 0, 0, 0 }};	// SROxCLK/SROxCLK_2/LTMRBKxCLK

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_Init error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	if( pipe_no == D_IM_LTM_PIPE1 || pipe_no == D_IM_LTM_PIPE12 ){
		clksel[0] = Dd_Top_Get_CLKSEL5_LTMRBK1CLK();
		switch( clksel[0] ){
			case 8:		// SRO1CLK
			case 16:	// SRO1CLK_2
				if( Dd_Top_Get_CLKSTOP10_SRO1CK() == 0 ){
					clkstop[0][0] = 1;
					Dd_Top_Set_CLKSTOP10_SRO1CK( 1 );
				}
				if( Dd_Top_Get_CLKSTOP10_SRO1CK_2() == 0 ){
					clkstop[0][1] = 1;
					Dd_Top_Set_CLKSTOP10_SRO1CK_2( 1 );
				}
				if( Dd_Top_Get_CLKSTOP10_LTMRBK1CK() == 0 ){
					clkstop[0][2] = 1;
					Dd_Top_Set_CLKSTOP10_LTMRBK1CK( 1 );
				}
				Dd_Top_Set_CLKSEL5_LTMRBK1CLK( 0 );		// 300MHz
				break;
			default:
				if( Dd_Top_Get_CLKSTOP10_LTMRBK1CK() == 0 ){
					clkstop[0][2] = 1;
					Dd_Top_Set_CLKSTOP10_LTMRBK1CK( 1 );
				}
				break;
		}
	}
	if( pipe_no == D_IM_LTM_PIPE2 || pipe_no == D_IM_LTM_PIPE12 ){
		clksel[1] = Dd_Top_Get_CLKSEL7_LTMRBK2CLK();
		switch( clksel[1] ){
			case 8:		// SRO2CLK
			case 16:	// SRO2CLK_2
				if( Dd_Top_Get_CLKSTOP12_SRO2CK() == 0 ){
					clkstop[1][0] = 1;
					Dd_Top_Set_CLKSTOP12_SRO2CK( 1 );
				}
				if( Dd_Top_Get_CLKSTOP12_SRO2CK_2() == 0 ){
					clkstop[1][1] = 1;
					Dd_Top_Set_CLKSTOP12_SRO2CK_2( 1 );
				}
				if( Dd_Top_Get_CLKSTOP12_LTMRBK2CK() == 0 ){
					clkstop[1][2] = 1;
					Dd_Top_Set_CLKSTOP12_LTMRBK2CK( 1 );
				}
				Dd_Top_Set_CLKSEL7_LTMRBK2CLK( 0 );		// 300MHz
				break;
			default:
				if( Dd_Top_Get_CLKSTOP12_LTMRBK2CK() == 0 ){
					clkstop[1][2] = 1;
					Dd_Top_Set_CLKSTOP12_LTMRBK2CK( 1 );
				}
				break;
		}
	}

	im_ltm_rbk_manager_init( pipe_no );

	Im_LTM_On_Pclk( pipe_no );
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RBKSR.bit.SR = D_IM_LTM_SR_RELEASE;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RSRAMEN.word = D_IM_LTM_RBK_RSRAMEN_ENABLE;
	Im_LTM_Off_Pclk( pipe_no );

	switch( clksel[0] ){
		case 8:		// SRO1CLK
		case 16:	// SRO1CLK_2
			Dd_Top_Set_CLKSEL5_LTMRBK1CLK( clksel[0] );		// original settings
			break;
		default:
			// DO NOTHING.
			break;
	}
	switch( clksel[1] ){
		case 8:		// SRO2CLK
		case 16:	// SRO2CLK_2
			Dd_Top_Set_CLKSEL7_LTMRBK2CLK( clksel[1] );		// original settings
			break;
		default:
			// DO NOTHING.
			break;
	}

	if( clkstop[0][0] ){
		Dd_Top_Set_CLKSTOP10_SRO1CK( 0 );
	}
	if( clkstop[0][1] ){
		Dd_Top_Set_CLKSTOP10_SRO1CK_2( 0 );
	}
	if( clkstop[0][2] ){
		Dd_Top_Set_CLKSTOP10_LTMRBK1CK( 0 );
	}
	if( clkstop[1][0] ){
		Dd_Top_Set_CLKSTOP12_SRO2CK( 0 );
	}
	if( clkstop[1][1] ){
		Dd_Top_Set_CLKSTOP12_SRO2CK_2( 0 );
	}
	if( clkstop[1][2] ){
		Dd_Top_Set_CLKSTOP12_LTMRBK2CK( 0 );
	}

	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* LTM_RBK SW reset
 */
INT32 Im_LTM_RBK_SW_Reset( UCHAR pipe_no )
{
	UCHAR	loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_SW_Reset error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_ltm_rbk_get_loop_val( pipe_no, &loop_sta, &loop_end );

	Im_LTM_On_Pclk( pipe_no );
	Im_LTM_On_RBKClk( pipe_no );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		if( gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RTRG.bit.TRG == D_IM_LTM_RBK_TRG_BUSY ){
			return D_IM_LTM_MACRO_BUSY;
		}
	}
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RBKSR.bit.SR = D_IM_LTM_SR_RESET;

	Im_LTM_Off_RBKClk( pipe_no );
	Im_LTM_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* set AXI bus I/F Control
 */
INT32 Im_LTM_RBK_Ctrl_Axi( UCHAR pipe_no, const T_IM_LTM_RBK_CTRL_AXI* const ctrl_axi )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( ctrl_axi == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_RBK_Ctrl_Axi error. ctrl_axi = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_Ctrl_Axi error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXICTLR1.bit.ARCACHE_R = ctrl_axi->rch[D_IM_LTM_PORT_R].cache_type;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXICTLR1.bit.ARPROT_R  = ctrl_axi->rch[D_IM_LTM_PORT_R].protect_type;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXICTLR1.bit.ARCACHE_G = ctrl_axi->rch[D_IM_LTM_PORT_G].cache_type;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXICTLR1.bit.ARPROT_G  = ctrl_axi->rch[D_IM_LTM_PORT_G].protect_type;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXICTLR1.bit.ARCACHE_B = ctrl_axi->rch[D_IM_LTM_PORT_B].cache_type;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXICTLR1.bit.ARPROT_B  = ctrl_axi->rch[D_IM_LTM_PORT_B].protect_type;

	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXICTLW1.bit.AWCACHE = ctrl_axi->wch.cache_type;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXICTLW1.bit.AWPROT  = ctrl_axi->wch.protect_type;
	Im_LTM_Off_Pclk( pipe_no );

	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* get AXI bus read channel status
 */
INT32 Im_LTM_RBK_Get_AxiReadStat( UCHAR pipe_no, T_IM_LTM_RBK_AXI_RCH_STAT* const axi_read_stat )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( axi_read_stat == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_RBK_Get_AxiReadStat error. axi_read_stat = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_Get_AxiReadStat error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );
	axi_read_stat->rch_axi_stat[D_IM_LTM_PORT_1] = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXIRERR.bit.RRESP_R;
	axi_read_stat->rch_axi_stat[D_IM_LTM_PORT_2] = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXIRERR.bit.RRESP_G;
	axi_read_stat->rch_axi_stat[D_IM_LTM_PORT_3] = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXIRERR.bit.RRESP_B;
	Im_LTM_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* get AXI bus write channel status
 */
INT32 Im_LTM_RBK_Get_AxiWriteStat( UCHAR pipe_no, T_IM_LTM_RBK_AXI_WCH_STAT* const axi_write_stat )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( axi_write_stat == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_RBK_Get_AxiWriteStat error. axi_write_stat = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_Get_AxiWriteStat error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );
	axi_write_stat->wch_axi_stat = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXIWERR.bit.WRESP;
	Im_LTM_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* get AXI bus ctrl param
 */
INT32 Im_LTM_RBK_Get_AxiCtrlParam( UCHAR pipe_no, T_IM_LTM_RBK_AXI_CTRL_PARAM* const axi_ctrl_prm )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( axi_ctrl_prm == NULL ){
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_RBK_Get_AxiCtrlParam error. axi_ctrl_prm = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_Get_AxiCtrlParam error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );
	axi_ctrl_prm->rch_burst_length_r = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXIBSIZ.bit.BURSTRR;
	axi_ctrl_prm->rch_burst_length_g = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXIBSIZ.bit.BURSTRG;
	axi_ctrl_prm->rch_burst_length_b = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXIBSIZ.bit.BURSTRB;
	axi_ctrl_prm->wch_burst_length   = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXIBSIZ.bit.BURSTW;
	axi_ctrl_prm->mask_write         = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXIBSIZ.bit.WSTBMSK;
	Im_LTM_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* set AXI bus ctrl param
 */
INT32 Im_LTM_RBK_Set_AxiCtrlParam( UCHAR pipe_no, const T_IM_LTM_RBK_AXI_CTRL_PARAM* const axi_ctrl_prm )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( axi_ctrl_prm == NULL ){
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_RBK_Set_AxiCtrlParam error. axi_ctrl_prm = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_Set_AxiCtrlParam error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXIBSIZ.bit.BURSTRR = axi_ctrl_prm->rch_burst_length_r;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXIBSIZ.bit.BURSTRG = axi_ctrl_prm->rch_burst_length_g;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXIBSIZ.bit.BURSTRB = axi_ctrl_prm->rch_burst_length_b;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXIBSIZ.bit.BURSTW  = axi_ctrl_prm->wch_burst_length;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXIBSIZ.bit.WSTBMSK = axi_ctrl_prm->mask_write;
	Im_LTM_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* LTM_RBK Control (each mode common control)
 */
INT32 Im_LTM_RBK_Ctrl_Common( UCHAR pipe_no, const T_IM_LTM_RBK_CTRL_COMMON* const rbk_ctrl )
{
	UCHAR	loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( rbk_ctrl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_RBK_Ctrl_Common error. rbk_ctrl = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_Ctrl_Common error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_ltm_rbk_get_loop_val( pipe_no, &loop_sta, &loop_end );

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RLINTLV.bit.LINTLV   = rbk_ctrl->line_intr;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXIBSIZ.bit.BURSTW  = rbk_ctrl->burst_length;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXIBSIZ.bit.WSTBMSK = rbk_ctrl->mask_write;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RIOCTL.bit.OUTSEL    = rbk_ctrl->out_select;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RIOCTL.bit.W0DTYP    = rbk_ctrl->data_type;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RDCMD.bit.RDCEN      = rbk_ctrl->reduction_enable;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RDCMD.bit.RDCMD      = rbk_ctrl->reduction_mode;

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		// Set Callback pointer
		gIM_LTM_RBK_Mng[loop_cnt].rbk_user_handler = rbk_ctrl->rbk_user_handler;
		gIM_LTM_RBK_Mng[loop_cnt].user_param = rbk_ctrl->user_param;
	}

	Im_LTM_Off_Pclk( pipe_no );

	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* LTM_RBK Control (SDRAM input mode control)
 */
INT32 Im_LTM_RBK_Ctrl_ModeSDRAMInput( UCHAR pipe_no, const T_IM_LTM_RBK_CTRL_SDRAM_INPUT* const rbk_ctrl_sdram_input )
{
	UCHAR	loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( rbk_ctrl_sdram_input == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_RBK_Ctrl_ModeSDRAMInput error. rbk_ctrl_sdram_input = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_Ctrl_ModeSDRAMInput error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_ltm_rbk_get_loop_val( pipe_no, &loop_sta, &loop_end );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_LTM_RBK_Mng[loop_cnt].input_size = rbk_ctrl_sdram_input->input_rect;
	}

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXIBSIZ.bit.BURSTRR = rbk_ctrl_sdram_input->burst_length_r;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXIBSIZ.bit.BURSTRG = rbk_ctrl_sdram_input->burst_length_g;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RAXIBSIZ.bit.BURSTRB = rbk_ctrl_sdram_input->burst_length_b;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RSDRDEF.bit.SDRDEF   = rbk_ctrl_sdram_input->input_global;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RSDRHSZ.bit.SDRHSZ   = rbk_ctrl_sdram_input->input_rect.img_width;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RSDRVSZ.bit.SDRVSZ   = rbk_ctrl_sdram_input->input_rect.img_lines;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RIOCTL.bit.INSEL     = D_IM_LTM_RBK_MODE_SDRAM_INPUT;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RSFEN.bit.RSFEN      = rbk_ctrl_sdram_input->shift_enable;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->HVEN.bit.HVEN        = D_IM_LTM_ENABLE_ON;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Set Managed Input Address information
 */
INT32 Im_LTM_RBK_Set_InAddr_Info( UCHAR pipe_no, const T_IM_LTM_INADDR_INFO* const in_addr )
{
	UINT32 offset;

#ifdef CO_PARAM_CHECK
	if( in_addr == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_RBK_Set_InAddr_Info error. in_addr = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_Set_InAddr_Info error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	offset = ( gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RSDRDEF.bit.SDRDEF * gIM_LTM_RBK_Mng[pipe_no].input_size.img_top ) + ( gIM_LTM_RBK_Mng[pipe_no].input_size.img_left * 2 );

	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RSDRADR.bit.SDRADR = (UINT32)in_addr->addr_r + offset;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RSDRADG.bit.SDRADG = (UINT32)in_addr->addr_g + offset;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RSDRADB.bit.SDRADB = (UINT32)in_addr->addr_b + offset;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Set Managed output Address/Global width information
 */
INT32 Im_LTM_RBK_Set_OutData_Info( UCHAR pipe_no, const T_IM_LTM_RBK_OUTDATA_INFO* const out_data_info )
{
#ifdef CO_PARAM_CHECK
	if( out_data_info == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_RBK_Set_OutData_Info error. out_data_info = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_Set_OutData_Info error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RSDWAD.bit.SDWAD   = (UINT32)out_data_info->addr;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RSDWDEF.bit.SDWDEF = out_data_info->global_width;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* LTM_RBK Control (SRO direct connection mode control)
 */
INT32 Im_LTM_RBK_Ctrl_ModeSRODirect( UCHAR pipe_no, const T_IM_LTM_RBK_CTRL_SRO_DIRECT* const rbk_ctrl_sro_direct )
{
#ifdef CO_PARAM_CHECK
	if( rbk_ctrl_sro_direct == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_RBK_Ctrl_ModeSRODirect error. rbk_ctrl_sro_direct = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_Ctrl_ModeSRODirect error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RIOCTL.bit.INSEL    = D_IM_LTM_RBK_MODE_SRO_DIRECT;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RIOCTL.bit.BAYER0   = rbk_ctrl_sro_direct->top_pixel0;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RIOCTL.bit.BAYER1   = rbk_ctrl_sro_direct->top_pixel1;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RIOCTL.bit.SROMD    = rbk_ctrl_sro_direct->sro_mode;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RSFEN.bit.RSFEN0    = rbk_ctrl_sro_direct->shift_enable0;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RSFEN.bit.RSFEN1    = rbk_ctrl_sro_direct->shift_enable1;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->HVEN.bit.HVEN       = rbk_ctrl_sro_direct->half_reduction_enable;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->BTHSTA0.bit.BTHSTA0 = rbk_ctrl_sro_direct->bayer_trim0.img_left;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->BTVSTA0.bit.BTVSTA0 = rbk_ctrl_sro_direct->bayer_trim0.img_top;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->BTHSIZ0.bit.BTHSIZ0 = rbk_ctrl_sro_direct->bayer_trim0.img_width;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->BTVSIZ0.bit.BTVSIZ0 = rbk_ctrl_sro_direct->bayer_trim0.img_lines;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->BTHSTA1.bit.BTHSTA1 = rbk_ctrl_sro_direct->bayer_trim1.img_left;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->BTVSTA1.bit.BTVSTA1 = rbk_ctrl_sro_direct->bayer_trim1.img_top;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->BTHSIZ1.bit.BTHSIZ1 = rbk_ctrl_sro_direct->bayer_trim1.img_width;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->BTVSIZ1.bit.BTVSIZ1 = rbk_ctrl_sro_direct->bayer_trim1.img_lines;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* LTM_RBK resolution conversion Control
 */
INT32 Im_LTM_RBK_Ctrl_Resolution_Conversion( UCHAR pipe_no, const UCHAR conversion_on_off )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_Ctrl_Resolution_Conversion error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif

	Im_LTM_On_Pclk( pipe_no );

	if( conversion_on_off == D_IM_LTM_ENABLE_ON ){
		gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RHSTA.bit.HSTA = 0x2000;
		gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RHPIT.bit.HPIT = 0x8000;
		gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RVSTA.bit.VSTA = 0x2000;
		gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RVPIT.bit.VPIT = 0x8000;
	}
	else{
		gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RHSTA.bit.HSTA = 0;
		gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RHPIT.bit.HPIT = 0x4000;
		gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RVSTA.bit.VSTA = 0;
		gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RVPIT.bit.VPIT = 0x4000;
	}

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Set Frame stop
 */
INT32 Im_LTM_RBK_Set_FrameStop( UCHAR pipe_no, UCHAR onoff )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_Set_FrameStop error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RFRSTP.bit.FRSTP = onoff;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Set RBK parameter hold
*/
INT32 Im_LTM_RBK_Set_ParamHold( UCHAR pipe_no, const UCHAR hold_enable )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_Set_ParamHold error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RPARMSK.bit.PARMSK = hold_enable;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Get RBK busy status.
 */
INT32 Im_LTM_RBK_Get_RbkBusy( UCHAR pipe_no, BOOL* const busy_status )
{
	ULONG rbktrg;

#ifdef CO_PARAM_CHECK
	if( busy_status == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_RBK_Get_RbkBusy error. busy_status = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_Get_RbkBusy error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	rbktrg = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RTRG.bit.TRG;

	Im_LTM_Off_Pclk( pipe_no );

	if( rbktrg == D_IM_LTM_RBK_TRG_BUSY ) {
		*busy_status = TRUE;
	}
	else {
		*busy_status = FALSE;
	}
	return D_DDIM_OK;
}

/* Restart RBK process.
 */
INT32 Im_LTM_RBK_ContStart( UCHAR pipe_no )
{
	UCHAR	loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_ContStart error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	// P-clock control
#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
	Ddim_Print(( "Im_LTM_RBK_ContStart on pclk\n" ));
#endif

	im_ltm_rbk_get_loop_val( pipe_no, &loop_sta, &loop_end );

	Im_LTM_On_Pclk( pipe_no );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RSRAMEN.bit.SRAMAEN = D_IM_LTM_ENABLE_ON;
		gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RSRAMEN.bit.RMAENLB1 = (gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->HVEN.bit.HVEN == D_IM_LTM_ENABLE_OFF) ? D_IM_LTM_ENABLE_OFF : D_IM_LTM_ENABLE_ON;
		if(( gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RIOCTL.bit.INSEL == D_IM_LTM_RBK_MODE_SDRAM_INPUT )
		|| ( gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RIOCTL.bit.SROMD != D_IM_LTM_RBK_SRO_DIRECT_2 )){
			gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RSRAMEN.bit.RMAENLB2 = D_IM_LTM_ENABLE_OFF;
			gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RSRAMEN.bit.RMAENSRO = D_IM_LTM_ENABLE_OFF;
		}
		else{
			gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RSRAMEN.bit.RMAENLB2 = D_IM_LTM_ENABLE_ON;
			gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RSRAMEN.bit.RMAENSRO = D_IM_LTM_ENABLE_ON;
		}
		gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RSRAMEN.bit.RMAENWCH = (gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RIOCTL.bit.OUTSEL == D_IM_LTM_RBK_OUT_MAP_DIRECT) ? D_IM_LTM_ENABLE_OFF : D_IM_LTM_ENABLE_ON;
		gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RSRAMEN.bit.RMAENRCH = (gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RIOCTL.bit.INSEL == D_IM_LTM_RBK_MODE_SRO_DIRECT) ? D_IM_LTM_ENABLE_OFF : D_IM_LTM_ENABLE_ON;
	}

	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RTRG.bit.TRG = D_IM_LTM_RBK_TRG_ON;
#ifdef CO_DEBUG_ON_PC
	if( im_ltm_rbk_check_target_pipe_no_1(pipe_no) ){
		gIM_Io_Ltm_Rbk_Reg_Ptr[D_IM_LTM_PIPE1]->RTRG.bit.TRG = D_IM_LTM_RBK_TRG_BUSY;
	}
	if( im_ltm_rbk_check_target_pipe_no_2(pipe_no) ){
		gIM_Io_Ltm_Rbk_Reg_Ptr[D_IM_LTM_PIPE2]->RTRG.bit.TRG = D_IM_LTM_RBK_TRG_BUSY;
	}
#endif

#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
	Ddim_Print(( "Im_LTM_RBK_ContStart off pclk\n" ));
#endif
	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Start RBK process.
 */
INT32 Im_LTM_RBK_Start( UCHAR pipe_no )
{
	DDIM_USER_ER		ercd;
	INT32				retcd;
	UCHAR				loop_cnt, loop_sta, loop_end;
	DDIM_USER_FLGPTN	flgptn = 0;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_Start error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
	Ddim_Print(( "Im_LTM_RBK_Start begin\n" ));
#endif

	// P-clock control
#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
	Ddim_Print(( "Im_LTM_RBK_Start on pclk\n" ));
#endif

	im_ltm_rbk_get_loop_val( pipe_no, &loop_sta, &loop_end );

	Im_LTM_On_Pclk( pipe_no );

	while( 1 ) {	// Pclk enable section
		// Clear of All Interrupt enable & Interrupt status
#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
		Ddim_Print(( "Im_LTM_RBK_Start begin initialize\n" ));
#endif

#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
		Ddim_Print(( "Im_LTM_RBK_Start initialize interrupt\n" ));
#endif
		// Initialized Interrupt
		gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RINT1.word = D_IM_LTM_RBK_RINT1_ALL_CLR;

		for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
			gIM_LTM_RBK_Mng[loop_cnt].int_status = 0;
		}

#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
		Ddim_Print(( "Im_LTM_RBK_Start enable interrupt\n" ));
#endif
		gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RINT0.word = D_IM_LTM_RBK_RINT0_ENABLE;

#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
		Ddim_Print(( "Im_LTM_RBK_Start on clk\n" ));
#endif
		Im_LTM_On_Aclk( pipe_no );
		Im_LTM_On_RBKClk( pipe_no );

		// Set access enable

		for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
			gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RSRAMEN.bit.SRAMAEN = D_IM_LTM_ENABLE_ON;
			gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RSRAMEN.bit.RMAENLB1 = (gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->HVEN.bit.HVEN == D_IM_LTM_ENABLE_OFF) ? D_IM_LTM_ENABLE_OFF : D_IM_LTM_ENABLE_ON;
			if(( gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RIOCTL.bit.INSEL == D_IM_LTM_RBK_MODE_SDRAM_INPUT )
			|| ( gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RIOCTL.bit.SROMD != D_IM_LTM_RBK_SRO_DIRECT_2 )){
				gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RSRAMEN.bit.RMAENLB2 = D_IM_LTM_ENABLE_OFF;
				gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RSRAMEN.bit.RMAENSRO = D_IM_LTM_ENABLE_OFF;
			}
			else{
				gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RSRAMEN.bit.RMAENLB2 = D_IM_LTM_ENABLE_ON;
				gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RSRAMEN.bit.RMAENSRO = D_IM_LTM_ENABLE_ON;
			}
			gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RSRAMEN.bit.RMAENWCH = (gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RIOCTL.bit.OUTSEL == D_IM_LTM_RBK_OUT_MAP_DIRECT) ? D_IM_LTM_ENABLE_OFF : D_IM_LTM_ENABLE_ON;
			gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RSRAMEN.bit.RMAENRCH = (gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RIOCTL.bit.INSEL == D_IM_LTM_RBK_MODE_SRO_DIRECT) ? D_IM_LTM_ENABLE_OFF : D_IM_LTM_ENABLE_ON;
		}

		im_ltm_rbk_wait_usec( D_IM_LTM_RBK_SRAM_WAIT_USEC );


#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
		Ddim_Print(( "Im_LTM_RBK_Start clear event flag\n" ));
#endif

		// Clear of Event Flag
		if( im_ltm_rbk_check_target_pipe_no_1( pipe_no ) ){
			flgptn |= D_IM_LTM_RBK1_INT_FLG_ALL;
		}
		if( im_ltm_rbk_check_target_pipe_no_2( pipe_no ) ){
			flgptn |= D_IM_LTM_RBK2_INT_FLG_ALL;
		}

		ercd = DDIM_User_Clr_Flg( FID_IM_LTM_RBK, ~flgptn );
		if( ercd != D_DDIM_USER_E_OK ) {
			Ddim_Print(( "Im_LTM_RBK_Start: invalid FID\n" ));
			retcd = D_IM_LTM_SYSTEM_ERROR;
			break;
		}

		gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RTRG.bit.TRG = D_IM_LTM_RBK_TRG_ON;
#ifdef CO_DEBUG_ON_PC
		for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
			gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RTRG.bit.TRG = D_IM_LTM_RBK_TRG_BUSY;
		}
#endif
		retcd = D_DDIM_OK;
		break;
	}

#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
	Ddim_Print(( "Im_LTM_RBK_Start off pclk\n" ));
#endif
	Im_LTM_Off_Pclk( pipe_no );

#ifdef D_IM_LTM_RBK_DEBUG_ON_PC
	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RINT1.word = 0;
		gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RINT1.bit.__FINFL = 1;
		if( gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RLINTLV.bit.LINTLV != 0 ){
			gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RINT1.bit.__LCNTFL = 1;
		}
		Im_LTM_RBK_Int_Handler( loop_cnt );
		gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RINT1.word = 0;
	}
#endif

	Im_LTM_Dsb();

#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
	Ddim_Print(( "Im_LTM_RBK_Start end\n" ));
#endif

	return retcd;
}

/* Stop LTM RBK process
 */
INT32 Im_LTM_RBK_Stop( UCHAR pipe_no )
{
	UCHAR loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_Stop error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_ltm_rbk_get_loop_val( pipe_no, &loop_sta, &loop_end );

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RTRG.bit.TRG = D_IM_LTM_RBK_TRG_OFF;

	Im_LTM_Dsb();
	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		while( gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RTRG.bit.TRG != D_IM_LTM_RBK_TRG_IDLE ) {
#ifdef CO_DEBUG_ON_PC
//			Ddim_Print(( "Im_LTM_RBK_Stop wait loop. %u\n", gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RTRG.bit.TRG ));
			gIM_Io_Ltm_Rbk_Reg_Ptr[loop_cnt]->RTRG.bit.TRG = D_IM_LTM_RBK_TRG_IDLE;
#endif
			;	// DO NOTHING
		}
	}

	// Clear of All Interrupt disable
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RINT0.word = D_IM_LTM_RBK_RINT0_DISABLE;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RINT1.word = D_IM_LTM_RBK_RINT1_ALL_CLR;

	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RSRAMEN.word = D_IM_LTM_RBK_RSRAMEN_DISABLE;

	Im_LTM_Off_RBKClk( pipe_no );
	Im_LTM_Off_Aclk( pipe_no );
	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Wait end of LTM RBK macro.
 */
INT32 Im_LTM_RBK_WaitEnd( DDIM_USER_FLGPTN* const p_flgptn, DDIM_USER_FLGPTN waiptn, DDIM_USER_TMO tmout )
{
	DDIM_USER_FLGPTN	flgptn;
	DDIM_USER_ER		ercd;

#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
	Ddim_Print(( "Im_LTM_RBK_WaitEnd begin\n" ));
#endif

#ifdef CO_PARAM_CHECK
	if( (waiptn & D_IM_LTM_RBK_INT_FLG_ALL) != waiptn ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_RBK_WaitEnd error. waiptn\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif
	Im_LTM_Dsb();

	switch( DDIM_User_Twai_Flg( FID_IM_LTM_RBK, waiptn, D_DDIM_USER_TWF_ORW, &flgptn, tmout ) ) {
		case D_DDIM_USER_E_OK:
#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
		Ddim_Print(( "Im_LTM_RBK_WaitEnd twai_flg OK return\n" ));
#endif
			flgptn = waiptn & flgptn;
			ercd = DDIM_User_Clr_Flg( FID_IM_LTM_RBK, ~flgptn );
			if( ercd != D_DDIM_USER_E_OK ) {
				Ddim_Print(( "Im_LTM_RBK_WaitEnd: invalid FID.\n" ));
				return D_IM_LTM_SYSTEM_ERROR;
			}
			if( p_flgptn != NULL ) {
				*p_flgptn = flgptn;
			}
			return D_DDIM_OK;
		case D_DDIM_USER_E_TMOUT:
#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
		Ddim_Print(( "Im_LTM_RBK_WaitEnd twai_flg TimeOut return\n" ));
#endif
			return D_IM_LTM_TIMEOUT;
		default:
#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
		Ddim_Print(( "Im_LTM_RBK_WaitEnd twai_flg NG return\n" ));
#endif
			return D_IM_LTM_PARAM_ERROR;
	}
}

/* LTM_RBK Interrupt Handler
 */
VOID Im_LTM_RBK_Int_Handler( UCHAR pipe_no )
{
	UINT32			loopcnt;
	DDIM_USER_ER	ercd;
	ULONG			rint0;
	ULONG			rint1;
	ULONG			rint1_clr;

#ifdef CO_PARAM_CHECK
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_Int_Handler error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return;
	}
#endif	// CO_PARAM_CHECK

#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
	Ddim_Print(( "Im_LTM_RBK_Int_Handler begin\n" ));
#endif

#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
	Ddim_Print(( "Im_LTM_RBK_Int_Handler on pclk\n" ));
#endif
	Im_LTM_On_Pclk( pipe_no );

	rint0 = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RINT0.word;
	rint1 = gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RINT1.word;
	rint1_clr = 0;

#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
	Ddim_Print(( "Im_LTM_RBK_Int_Handler rint0=0x%lx, rint1=0x%lx\n", rint0, rint1 ));
#endif

	// Call of Callback API for interrupt status

	for( loopcnt = 0; loopcnt < (sizeof(gim_ltm_rbk_inttbl[0]) / sizeof(gim_ltm_rbk_inttbl[0][0])); loopcnt++ ) {
		if( ((rint1 & gim_ltm_rbk_inttbl[pipe_no][loopcnt].flg_bitmask) != 0)
		 && ((rint0 & gim_ltm_rbk_inttbl[pipe_no][loopcnt].ena_bitmask) != 0) ) {

#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
			Ddim_Print(( "Im_LTM_RBK_Int_Handler begin 0x%x\n", gim_ltm_rbk_inttbl[pipe_no][loopcnt].flg_bitmask ));
#endif

			gIM_LTM_RBK_Mng[pipe_no].int_status = gim_ltm_rbk_inttbl[pipe_no][loopcnt].int_status;

			rint1_clr |= gim_ltm_rbk_inttbl[pipe_no][loopcnt].flg_bitmask;

			ercd = DDIM_User_Set_Flg( FID_IM_LTM_RBK, gim_ltm_rbk_inttbl[pipe_no][loopcnt].waitflg );
			if( ercd != D_DDIM_USER_E_OK ) {
				Ddim_Print(( "Im_LTM_RBK_Int_Handler: invalid FID. 0x%lx\n", gim_ltm_rbk_inttbl[pipe_no][loopcnt].waitflg ));
			}

			if (gIM_LTM_RBK_Mng[pipe_no].rbk_user_handler) {
				gIM_LTM_RBK_Mng[pipe_no].rbk_user_handler( (UINT32*)&gIM_LTM_RBK_Mng[pipe_no].int_status, gIM_LTM_RBK_Mng[pipe_no].user_param );
			}
		}
	}

	// Clear interrupt
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RINT1.word = rint1_clr;


#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
	Ddim_Print(( "Im_LTM_RBK_Int_Handler rint1=0x%lx\n", gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RINT1.word ));
#endif

#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
	Ddim_Print(( "Im_LTM_RBK_Int_Handler off pclk\n" ));
#endif
	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

#ifdef CO_DEBUG_PRINT_IM_LTM_RBK
	Ddim_Print(( "Im_LTM_RBK_Int_Handler end\n" ));
#endif
}

/* set LTM_RBK RGB trimming
 */
INT32 Im_LTM_RBK_Ctrl_RGBTrimming( UCHAR pipe_no, const T_IM_LTM_RBK_RGB_TRIMMING* const rbk_trimming )
{
	USHORT img_top;
	USHORT img_left;
	USHORT img_width;
	USHORT img_lines;

#ifdef CO_PARAM_CHECK
	if( rbk_trimming == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_RBK_Ctrl_RGBTrimming error. rbk_trimming = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_RBK_Ctrl_RGBTrimming error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RTEN.bit.RTEN = rbk_trimming->trimming_enable;

	// TRMHSTA
	img_left = rbk_trimming->rgb_trim.img_left;
	img_left = im_ltm_rbk_min( img_left, (USHORT)D_IM_LTM_RBK_RTHSTA_MAX );
	img_left = im_ltm_rbk_max( img_left, (USHORT)D_IM_LTM_RBK_RTHSTA_MIN );

	// TRMVSTA
	img_top = rbk_trimming->rgb_trim.img_top;
	img_top = im_ltm_rbk_min( img_top, (USHORT)D_IM_LTM_RBK_RTVSTA_MAX );
	img_top = im_ltm_rbk_max( img_top, (USHORT)D_IM_LTM_RBK_RTVSTA_MIN );

	// TRMHSIZ (2pixels alignment & Advancing)
	img_width = rbk_trimming->rgb_trim.img_width;
	img_width = im_ltm_rbk_min( img_width, (USHORT)D_IM_LTM_RBK_RTHSIZ_MAX );
	img_width = im_ltm_rbk_max( img_width, (USHORT)D_IM_LTM_RBK_RTHSIZ_MIN );

	// TRMVSIZ
	img_lines = rbk_trimming->rgb_trim.img_lines;
	img_lines = im_ltm_rbk_min( img_lines, (USHORT)D_IM_LTM_RBK_RTVSIZ_MAX );
	img_lines = im_ltm_rbk_max( img_lines, (USHORT)D_IM_LTM_RBK_RTVSIZ_MIN );

	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RTHSTA.bit.RTHSTA = img_left;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RTHSIZ.bit.RTHSIZ = img_width;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RTVSTA.bit.RTVSTA = img_top;
	gIM_Io_Ltm_Rbk_Reg_Ptr[pipe_no]->RTVSIZ.bit.RTVSIZ = img_lines;
	Im_LTM_Off_Pclk( pipe_no );
	return D_DDIM_OK;
}

#ifdef IM_LTM_RBK_STATUS_PRINT
VOID Im_LTM_RBK_Print_Status( VOID )
{
	UINT32 loopcnt;

	for( loopcnt = 0; loopcnt < 2; loopcnt++ ) {
		Ddim_Print(( "PIPE%u:\n", loopcnt ));
		Ddim_Print(( "\tinput_size\n" ));
		Ddim_Print(( "\t\timg_left            = 0x%x\n", gIM_LTM_RBK_Mng[loopcnt].input_size.img_left ));
		Ddim_Print(( "\t\timg_top             = 0x%x\n", gIM_LTM_RBK_Mng[loopcnt].input_size.img_top ));
		Ddim_Print(( "\t\timg_width           = 0x%x\n", gIM_LTM_RBK_Mng[loopcnt].input_size.img_width ));
		Ddim_Print(( "\t\timg_lines           = 0x%x\n", gIM_LTM_RBK_Mng[loopcnt].input_size.img_lines ));
		Ddim_Print(( "\tint_status            = 0x%x\n", gIM_LTM_RBK_Mng[loopcnt].int_status ));
		Ddim_Print(( "\tresolution_conversion = 0x%x\n", gIM_LTM_RBK_Mng[loopcnt].resolution_conversion ));
		Ddim_Print(( "\trbk_user_handler      = 0x%x\n", (UINT32)gIM_LTM_RBK_Mng[loopcnt].rbk_user_handler ));
		Ddim_Print(( "\tuser_param            = 0x%x\n", (UINT32)gIM_LTM_RBK_Mng[loopcnt].user_param ));
	}
}

VOID Im_LTM_RBK_Print_AccEnStatus( VOID )
{
	UINT32 loopcnt;

	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	for( loopcnt = 0; loopcnt < 2; loopcnt++ ) {
		Ddim_Print(( "SRAMAEN  = 0x%x\n", gIM_Io_Ltm_Rbk_Reg_Ptr[loopcnt]->RSRAMEN.bit.SRAMAEN ));
		Ddim_Print(( "RMAENLB1 = 0x%x\n", gIM_Io_Ltm_Rbk_Reg_Ptr[loopcnt]->RSRAMEN.bit.RMAENLB1 ));
		Ddim_Print(( "RMAENLB2 = 0x%x\n", gIM_Io_Ltm_Rbk_Reg_Ptr[loopcnt]->RSRAMEN.bit.RMAENLB2 ));
		Ddim_Print(( "RMAENHST = 0x%x\n", gIM_Io_Ltm_Rbk_Reg_Ptr[loopcnt]->RSRAMEN.bit.RMAENHST ));
		Ddim_Print(( "RMAENSRO = 0x%x\n", gIM_Io_Ltm_Rbk_Reg_Ptr[loopcnt]->RSRAMEN.bit.RMAENSRO ));
		Ddim_Print(( "RMAENWCH = 0x%x\n", gIM_Io_Ltm_Rbk_Reg_Ptr[loopcnt]->RSRAMEN.bit.RMAENWCH ));
		Ddim_Print(( "RMAENRCH = 0x%x\n", gIM_Io_Ltm_Rbk_Reg_Ptr[loopcnt]->RSRAMEN.bit.RMAENRCH ));
	}
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
}
#endif

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
#endif	// CO_DDIM_UTILITY_USE
