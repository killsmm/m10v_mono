/**
 * @file		im_ltm_ltm.c
 * @brief		Local Tone Mapping process(LTM Block)
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

// LSRAMEN value
#define D_IM_LTM_LTM_LSRAMEN_ENABLE		(0x00000031)
#define D_IM_LTM_LTM_LSRAMEN_DISABLE	(0x00000000)

// PIPE count
#define D_IM_LTM_PIPE_COUNT				(2)

// LINT0 value
#define D_IM_LTM_LTM_LINT0_ENABLE		(0x02301011)
#define D_IM_LTM_LTM_LINT0_DISABLE		(0x00000000)
// LINT1 value
#define D_IM_LTM_LTM_LINT1_ALL_CLR		(0x02301011)

// register bit pattern
#define D_IM_LTM_LTM_TRG_OFF			(0)
#define D_IM_LTM_LTM_TRG_ON				(1)
#define D_IM_LTM_LTM_TRG_IDLE			(2)
#define D_IM_LTM_LTM_TRG_BUSY			(3)

// Input mode
#define D_IM_LTM_LTM_MODE_B2R_DIRECT	(0)
#define D_IM_LTM_LTM_MODE_SDRAM_INPUT	(1)

#define D_IM_LTM_LTM_SRAM_WAIT_USEC		(1)

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
#define im_ltm_ltm_max( a, b )						(((a) > (b)) ? (a):(b))
#define im_ltm_ltm_min( a, b )						(((a) < (b)) ? (a):(b))

#define im_ltm_ltm_check_target_pipe_no_1(a)		(((a) + 1) & (D_IM_LTM_PIPE1 + 1))
#define im_ltm_ltm_check_target_pipe_no_2(a)		(((a) + 1) & (D_IM_LTM_PIPE2 + 1))


// The structure type matching check.
#ifdef IM_LTM_LTM_REG_TYPE_CHECK
#define im_ltm_ltm_check_reg_type( dst, src )	(dst) = (src);
#else
#define im_ltm_ltm_check_reg_type( dst, src )
#endif


// Signed register access macros.
#define im_ltm_ltm_set_reg_signed( reg, type, member, val )		\
		{ \
			type work = { .word = 0UL }; \
			work.bit.member = (val); \
			(reg).bit.member = work.bit.member; \
			im_ltm_ltm_check_reg_type( work, (reg) );	/* The structure type matching check. */ \
		}

#define im_ltm_ltm_set_reg_signed_a( reg, type, member, val )		\
		{ \
			type work = { .word[0] = 0xFFFFFFFFUL }; \
			work.bit.member = (val); \
			(reg).bit.member = work.bit.member; \
			im_ltm_ltm_check_reg_type( work, (reg) );	/* The structure type matching check. */ \
		}


// wait usec
#define im_ltm_ltm_wait_usec( usec )			Dd_ARM_Wait_ns( (usec) * 1000 )


#ifdef CO_ASSERT_ON
#define M_IM_LTM_LTM_ASSETION_MSG( msg )		(msg)
#else
#define M_IM_LTM_LTM_ASSETION_MSG( msg )		(NULL)
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
	T_IM_LTM_RECT			input_size_rgb;					// input size(RGB(Main image))
	T_IM_LTM_RECT			input_size_y;					// input size(Y(Map image))
	UINT32					int_status;						// Interrupt status
	VOID					(*ltm_user_handler)(UINT32* int_status, UINT32 user_param);	// Interrupt Handler
	UINT32					user_param;						// return to callback argument value when interrupt occurs
} T_IM_LTM_LTM_MNG;

// Interrupt parameter table
typedef struct {
	ULONG				flg_bitmask;		// Bitmask of interrupt flag.
	ULONG				ena_bitmask;		// Bitmask of inaerrupt enable.
	ULONG				waitflg;			// Value of set_flg.
	ULONG				int_status;			// Value of gIM_LTM_LTM_Mng.int_status.
} T_IM_LTM_LTM_INTFLG_TBL;


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
static volatile T_IM_LTM_LTM_MNG gIM_LTM_LTM_Mng[D_IM_LTM_PIPE_COUNT];
static volatile struct io_ltm_ch_ltm* gIM_Io_Ltm_Ltm_Reg_Ptr[D_IM_LTM_PIPE_MAX] = { &(IO_LTM_P1.LTM), &(IO_LTM_P2.LTM), &(IO_LTM_P3.LTM) };
static volatile struct io_ltm_ltm_sram* gIM_Io_Ltm_Ltm_Tbl_Ptr[D_IM_LTM_PIPE_MAX] = { &(IO_LTM_TBL_P1.LTM_SRAM), &(IO_LTM_TBL_P2.LTM_SRAM), &(IO_LTM_TBL_P3.LTM_SRAM) };


//
// Interrupt parameter table
//
static const T_IM_LTM_LTM_INTFLG_TBL gim_ltm_ltm_inttbl[D_IM_LTM_PIPE_COUNT][6] = {
	{
		{ 0x00000001, 0x00000001, D_IM_LTM_LTM1_INT_STATE_LTM_END,   D_IM_LTM_LTM1_INT_FLG_LTM_END  },	// FINEN
		{ 0x00000010, 0x00000010, D_IM_LTM_LTM1_INT_STATE_EXIP_END,  D_IM_LTM_LTM1_INT_FLG_EXIP_END },	// EXIPEN
		{ 0x00001000, 0x00001000, D_IM_LTM_LTM1_INT_STATE_LINE_END,  D_IM_LTM_LTM1_INT_FLG_LINE_END },	// LCNTEN
		{ 0x00100000, 0x00100000, D_IM_LTM_LTM1_INT_STATE_AXR_ERR,   D_IM_LTM_LTM1_INT_FLG_AXR_ERR  },	// AXREEN
		{ 0x00200000, 0x00200000, D_IM_LTM_LTM1_INT_STATE_AXW_ERR,   D_IM_LTM_LTM1_INT_FLG_AXW_ERR  },	// AXWEEN
		{ 0x02000000, 0x02000000, D_IM_LTM_LTM1_INT_STATE_YLOG_ERR,  D_IM_LTM_LTM1_INT_FLG_YLOG_ERR },	// RAMEEN
	},
	{
		{ 0x00000001, 0x00000001, D_IM_LTM_LTM2_INT_STATE_LTM_END,   D_IM_LTM_LTM2_INT_FLG_LTM_END  },	// FINEN
		{ 0x00000010, 0x00000010, D_IM_LTM_LTM2_INT_STATE_EXIP_END,  D_IM_LTM_LTM2_INT_FLG_EXIP_END },	// EXIPEN
		{ 0x00001000, 0x00001000, D_IM_LTM_LTM2_INT_STATE_LINE_END,  D_IM_LTM_LTM2_INT_FLG_LINE_END },	// LCNTEN
		{ 0x00100000, 0x00100000, D_IM_LTM_LTM2_INT_STATE_AXR_ERR,   D_IM_LTM_LTM2_INT_FLG_AXR_ERR  },	// AXREEN
		{ 0x00200000, 0x00200000, D_IM_LTM_LTM2_INT_STATE_AXW_ERR,   D_IM_LTM_LTM2_INT_FLG_AXW_ERR  },	// AXWEEN
		{ 0x02000000, 0x02000000, D_IM_LTM_LTM2_INT_STATE_YLOG_ERR,  D_IM_LTM_LTM2_INT_FLG_YLOG_ERR },	// RAMEEN
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
static VOID im_ltm_ltm_manager_init( UCHAR pipe_no )
{
	UCHAR size_coef = 1;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "im_ltm_ltm_manager_init error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return;
	}
#endif	// CO_PARAM_CHECK

	if( pipe_no == D_IM_LTM_PIPE12 ){
		size_coef = 2;
		pipe_no = 0;
	}

	// Initialize of State & Handler management information
	memset( (USHORT*)&gIM_LTM_LTM_Mng[pipe_no], 0, sizeof(T_IM_LTM_LTM_MNG) * size_coef );

	return;
}

static VOID im_ltm_ltm_get_loop_val( UCHAR pipe_no, UCHAR* loop_sta, UCHAR* loop_end )
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

/* LTM_LTM Initialize
 */
INT32 Im_LTM_LTM_Init( UCHAR pipe_no )
{
	ULONG clksel[2] = { 0, 0 };
	UCHAR clkstop[2][5] = {{ 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0 }};	// SROxCLK/SROxCLK_2/B2BxCLK/B2RxCLK/LTMRBKxCLK

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Init error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	if( pipe_no == D_IM_LTM_PIPE1 || pipe_no == D_IM_LTM_PIPE12 ){
		clksel[0] = Dd_Top_Get_CLKSEL4_LTM1CLK();
		switch( clksel[0] ){
			case 8:		// SRO1CLK
			case 16:	// SRO1CLK_2
			case 24:	// B2B1CLK
			case 32:	// B2R1CLK
				if( Dd_Top_Get_CLKSTOP10_SRO1CK() == 0 ){
					clkstop[0][0] = 1;
					Dd_Top_Set_CLKSTOP10_SRO1CK( 1 );
				}
				if( Dd_Top_Get_CLKSTOP10_SRO1CK_2() == 0 ){
					clkstop[0][1] = 1;
					Dd_Top_Set_CLKSTOP10_SRO1CK_2( 1 );
				}
				if( Dd_Top_Get_CLKSTOP10_B2B1CK() == 0 ){
					clkstop[0][2] = 1;
					Dd_Top_Set_CLKSTOP10_B2B1CK( 1 );
				}
				if( Dd_Top_Get_CLKSTOP10_B2R1CK() == 0 ){
					clkstop[0][3] = 1;
					Dd_Top_Set_CLKSTOP10_B2R1CK( 1 );
				}
				if( Dd_Top_Get_CLKSTOP11_LTM1CK() == 0 ){
					clkstop[0][4] = 1;
					Dd_Top_Set_CLKSTOP11_LTM1CK( 1 );
				}
				Dd_Top_Set_CLKSEL4_LTM1CLK( 0 );		// 300MHz
				break;
			default:
				if( Dd_Top_Get_CLKSTOP11_LTM1CK() == 0 ){
					clkstop[0][4] = 1;
					Dd_Top_Set_CLKSTOP11_LTM1CK( 1 );
				}
				break;
		}
	}
	if( pipe_no == D_IM_LTM_PIPE2 || pipe_no == D_IM_LTM_PIPE12 ){
		clksel[1] = Dd_Top_Get_CLKSEL6_LTM2CLK();
		switch( clksel[1] ){
			case 8:		// SRO1CLK
			case 16:	// SRO1CLK_2
			case 24:	// B2B1CLK
			case 32:	// B2R1CLK
				if( Dd_Top_Get_CLKSTOP12_SRO2CK() == 0 ){
					clkstop[1][0] = 1;
					Dd_Top_Set_CLKSTOP12_SRO2CK( 1 );
				}
				if( Dd_Top_Get_CLKSTOP12_SRO2CK_2() == 0 ){
					clkstop[1][1] = 1;
					Dd_Top_Set_CLKSTOP12_SRO2CK_2( 1 );
				}
				if( Dd_Top_Get_CLKSTOP12_B2B2CK() == 0 ){
					clkstop[1][2] = 1;
					Dd_Top_Set_CLKSTOP12_B2B2CK( 1 );
				}
				if( Dd_Top_Get_CLKSTOP12_B2R2CK() == 0 ){
					clkstop[1][3] = 1;
					Dd_Top_Set_CLKSTOP12_B2R2CK( 1 );
				}
				if( Dd_Top_Get_CLKSTOP13_LTM2CK() == 0 ){
					clkstop[1][4] = 1;
					Dd_Top_Set_CLKSTOP13_LTM2CK( 1 );
				}
				Dd_Top_Set_CLKSEL6_LTM2CLK( 0 );		// 300MHz
				break;
			default:
				if( Dd_Top_Get_CLKSTOP13_LTM2CK() == 0 ){
					clkstop[1][4] = 1;
					Dd_Top_Set_CLKSTOP13_LTM2CK( 1 );
				}
				break;
		}
	}

	im_ltm_ltm_manager_init( pipe_no );

	Im_LTM_On_Pclk( pipe_no );
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LTMSR.bit.SR = D_IM_LTM_SR_RELEASE;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSRAMEN.word = D_IM_LTM_LTM_LSRAMEN_ENABLE;
	Im_LTM_Off_Pclk( pipe_no );

	switch( clksel[0] ){
		case 8:		// SRO1CLK
		case 16:	// SRO1CLK_2
		case 24:	// B2B1CLK
		case 32:	// B2R1CLK
			Dd_Top_Set_CLKSEL4_LTM1CLK( clksel[0] );		// original settings
			break;
		default:
			// DO NOTHING.
			break;
	}
	switch( clksel[1] ){
		case 8:		// SRO1CLK
		case 16:	// SRO1CLK_2
		case 24:	// B2B1CLK
		case 32:	// B2R1CLK
			Dd_Top_Set_CLKSEL6_LTM2CLK( clksel[1] );		// original settings
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
		Dd_Top_Set_CLKSTOP10_B2B1CK( 0 );
	}
	if( clkstop[0][3] ){
		Dd_Top_Set_CLKSTOP10_B2R1CK( 0 );
	}
	if( clkstop[0][4] ){
		Dd_Top_Set_CLKSTOP11_LTM1CK( 0 );
	}
	if( clkstop[1][0] ){
		Dd_Top_Set_CLKSTOP12_SRO2CK( 0 );
	}
	if( clkstop[1][1] ){
		Dd_Top_Set_CLKSTOP12_SRO2CK_2( 0 );
	}
	if( clkstop[1][2] ){
		Dd_Top_Set_CLKSTOP12_B2B2CK( 0 );
	}
	if( clkstop[1][3] ){
		Dd_Top_Set_CLKSTOP12_B2R2CK( 0 );
	}
	if( clkstop[1][4] ){
		Dd_Top_Set_CLKSTOP13_LTM2CK( 0 );
	}

	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* LTM_LTM SW reset
 */
INT32 Im_LTM_LTM_SW_Reset( UCHAR pipe_no )
{
	UCHAR	loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_SW_Reset error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_ltm_ltm_get_loop_val( pipe_no, &loop_sta, &loop_end );

	Im_LTM_On_Pclk( pipe_no );
	Im_LTM_On_Clk( pipe_no );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		if( gIM_Io_Ltm_Ltm_Reg_Ptr[loop_cnt]->LTRG.bit.TRG == D_IM_LTM_LTM_TRG_BUSY ){
			return D_IM_LTM_MACRO_BUSY;
		}
	}
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LTMSR.bit.SR = D_IM_LTM_SR_RESET;

	Im_LTM_Off_Clk( pipe_no );
	Im_LTM_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* set AXI bus I/F Control
 */
INT32 Im_LTM_LTM_Ctrl_Axi( UCHAR pipe_no, const T_IM_LTM_LTM_CTRL_AXI* const ctrl_axi )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( ctrl_axi == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Ctrl_Axi error. ctrl_axi = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Ctrl_Axi error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXICTLR1.bit.ARCACHE_R  = ctrl_axi->rch_rgb[D_IM_LTM_PORT_R].cache_type;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXICTLR1.bit.ARPROT_R   = ctrl_axi->rch_rgb[D_IM_LTM_PORT_R].protect_type;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXICTLR1.bit.ARCACHE_G  = ctrl_axi->rch_rgb[D_IM_LTM_PORT_G].cache_type;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXICTLR1.bit.ARPROT_G   = ctrl_axi->rch_rgb[D_IM_LTM_PORT_G].protect_type;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXICTLR1.bit.ARCACHE_B  = ctrl_axi->rch_rgb[D_IM_LTM_PORT_B].cache_type;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXICTLR1.bit.ARPROT_B   = ctrl_axi->rch_rgb[D_IM_LTM_PORT_B].protect_type;

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXICTLR2.bit.ARCACHE_YP = ctrl_axi->rch_y[D_IM_LTM_PORT_Y_YP].cache_type;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXICTLR2.bit.ARPROT_YP  = ctrl_axi->rch_y[D_IM_LTM_PORT_Y_YP].protect_type;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXICTLR2.bit.ARCACHE_YN = ctrl_axi->rch_y[D_IM_LTM_PORT_Y_YN].cache_type;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXICTLR2.bit.ARPROT_YN  = ctrl_axi->rch_y[D_IM_LTM_PORT_Y_YN].protect_type;

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXICTLW.bit.AWCACHE_R   = ctrl_axi->wch[D_IM_LTM_PORT_R].cache_type;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXICTLW.bit.AWPROT_R    = ctrl_axi->wch[D_IM_LTM_PORT_R].protect_type;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXICTLW.bit.AWCACHE_G   = ctrl_axi->wch[D_IM_LTM_PORT_G].cache_type;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXICTLW.bit.AWPROT_G    = ctrl_axi->wch[D_IM_LTM_PORT_G].protect_type;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXICTLW.bit.AWCACHE_B   = ctrl_axi->wch[D_IM_LTM_PORT_B].cache_type;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXICTLW.bit.AWPROT_B    = ctrl_axi->wch[D_IM_LTM_PORT_B].protect_type;

	Im_LTM_Off_Pclk( pipe_no );

	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* get AXI bus read channel status
 */
INT32 Im_LTM_LTM_Get_AxiReadStat( UCHAR pipe_no, T_IM_LTM_LTM_AXI_RCH_STAT* const axi_read_stat )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( axi_read_stat == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Get_AxiReadStat error. axi_read_stat = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Get_AxiReadStat error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	axi_read_stat->rch_rgb_axi_stat[D_IM_LTM_PORT_1] = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIRERR.bit.RRESP_R;
	axi_read_stat->rch_rgb_axi_stat[D_IM_LTM_PORT_2] = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIRERR.bit.RRESP_G;
	axi_read_stat->rch_rgb_axi_stat[D_IM_LTM_PORT_3] = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIRERR.bit.RRESP_B;

	axi_read_stat->rch_y_axi_stat[D_IM_LTM_PORT_1]   = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIRERR.bit.RRESP_YP;
	axi_read_stat->rch_y_axi_stat[D_IM_LTM_PORT_2]   = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIRERR.bit.RRESP_YN;

	Im_LTM_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* get AXI bus write channel status
 */
INT32 Im_LTM_LTM_Get_AxiWriteStat( UCHAR pipe_no, T_IM_LTM_LTM_AXI_WCH_STAT* const axi_write_stat )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( axi_write_stat == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Get_AxiWriteStat error. axi_write_stat = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Get_AxiWriteStat error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	axi_write_stat->wch_axi_stat[D_IM_LTM_PORT_1] = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIWERR.bit.WRESP_R;
	axi_write_stat->wch_axi_stat[D_IM_LTM_PORT_2] = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIWERR.bit.WRESP_G;
	axi_write_stat->wch_axi_stat[D_IM_LTM_PORT_3] = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIWERR.bit.WRESP_B;

	Im_LTM_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* get AXI bus ctrl param
 */
INT32 Im_LTM_LTM_Get_AxiCtrlParam( UCHAR pipe_no, T_IM_LTM_LTM_AXI_CTRL_PARAM* const axi_ctrl_prm )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( axi_ctrl_prm == NULL ){
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Get_AxiCtrlParam error. axi_ctrl_prm = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Get_AxiCtrlParam error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	axi_ctrl_prm->rch_burst_length_r  = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTRR;
	axi_ctrl_prm->rch_burst_length_g  = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTRG;
	axi_ctrl_prm->rch_burst_length_b  = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTRB;
	axi_ctrl_prm->rch_burst_length_yp = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTRYP;
	axi_ctrl_prm->rch_burst_length_yn = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTRYN;
	axi_ctrl_prm->wch_burst_length_r  = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTWR;
	axi_ctrl_prm->wch_burst_length_g  = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTWG;
	axi_ctrl_prm->wch_burst_length_b  = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTWB;
	axi_ctrl_prm->mask_write_r        = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.WSTBMSKR;
	axi_ctrl_prm->mask_write_g        = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.WSTBMSKG;
	axi_ctrl_prm->mask_write_b        = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.WSTBMSKB;
	axi_ctrl_prm->read_select_yp      = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->AXIRSEL.bit.AXIRSEL1;
	axi_ctrl_prm->read_select_yn      = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->AXIRSEL.bit.AXIRSEL2;

	Im_LTM_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* set AXI bus ctrl param
 */
INT32 Im_LTM_LTM_Set_AxiCtrlParam( UCHAR pipe_no, const T_IM_LTM_LTM_AXI_CTRL_PARAM* const axi_ctrl_prm )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( axi_ctrl_prm == NULL ){
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Set_AxiCtrlParam error. axi_ctrl_prm = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Set_AxiCtrlParam error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTRR  = axi_ctrl_prm->rch_burst_length_r;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTRG  = axi_ctrl_prm->rch_burst_length_g;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTRB  = axi_ctrl_prm->rch_burst_length_b;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTRYP = axi_ctrl_prm->rch_burst_length_yp;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTRYN = axi_ctrl_prm->rch_burst_length_yn;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTWR  = axi_ctrl_prm->wch_burst_length_r;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTWG  = axi_ctrl_prm->wch_burst_length_g;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTWB  = axi_ctrl_prm->wch_burst_length_b;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.WSTBMSKR = axi_ctrl_prm->mask_write_r;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.WSTBMSKG = axi_ctrl_prm->mask_write_g;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.WSTBMSKB = axi_ctrl_prm->mask_write_b;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->AXIRSEL.bit.AXIRSEL1  = axi_ctrl_prm->read_select_yp;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->AXIRSEL.bit.AXIRSEL2  = axi_ctrl_prm->read_select_yn;

	Im_LTM_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* LTM_LTM Control (each mode common control)
 */
INT32 Im_LTM_LTM_Ctrl_Common( UCHAR pipe_no, const T_IM_LTM_LTM_CTRL_COMMON* const ltm_ctrl )
{
	UCHAR	loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( ltm_ctrl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Ctrl_Common error. ltm_ctrl = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Ctrl_Common error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_ltm_ltm_get_loop_val( pipe_no, &loop_sta, &loop_end );

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LLINTLV.bit.LINTLV     = ltm_ctrl->line_intr;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTRYP  = ltm_ctrl->rch_burst_length_yp;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTRYN  = ltm_ctrl->rch_burst_length_yn;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTWR   = ltm_ctrl->wch_burst_length_r;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTWG   = ltm_ctrl->wch_burst_length_g;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTWB   = ltm_ctrl->wch_burst_length_b;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.WSTBMSKR  = ltm_ctrl->mask_write_r;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.WSTBMSKG  = ltm_ctrl->mask_write_g;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.WSTBMSKB  = ltm_ctrl->mask_write_b;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->AXIRSEL.bit.AXIRSEL1   = ltm_ctrl->read_select_yp;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->AXIRSEL.bit.AXIRSEL2   = ltm_ctrl->read_select_yn;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LIOCTL.bit.R3DTYP      = ltm_ctrl->in_data_type_yp;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LIOCTL.bit.R4DTYP      = ltm_ctrl->in_data_type_yn;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LIOCTL.bit.OUTSEL      = ltm_ctrl->out_select;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LIOCTL.bit.W1DTYP      = ltm_ctrl->out_data_type;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDRHSZR3.bit.SDRHSZR3 = ltm_ctrl->input_rect.img_width;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDRDEFR3.bit.SDRDEFR3 = ltm_ctrl->global_width_yp;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDRVSZR3.bit.SDRVSZR3 = ltm_ctrl->input_rect.img_lines;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDRDEFR4.bit.SDRDEFR4 = ltm_ctrl->global_width_yn;

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_LTM_LTM_Mng[loop_cnt].input_size_y = ltm_ctrl->input_rect;
		// Set Callback pointer
		gIM_LTM_LTM_Mng[loop_cnt].ltm_user_handler = ltm_ctrl->ltm_user_handler;
		gIM_LTM_LTM_Mng[loop_cnt].user_param = ltm_ctrl->user_param;
	}

	Im_LTM_Off_Pclk( pipe_no );

	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* LTM_LTM Control (SDRAM input mode control)
 */
INT32 Im_LTM_LTM_Ctrl_ModeSDRAMInput( UCHAR pipe_no, const T_IM_LTM_LTM_CTRL_SDRAM_INPUT* const ltm_ctrl_sdram_input )
{
	UCHAR	loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( ltm_ctrl_sdram_input == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Ctrl_ModeSDRAMInput error. ltm_ctrl_sdram_input = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Ctrl_ModeSDRAMInput error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_ltm_ltm_get_loop_val( pipe_no, &loop_sta, &loop_end );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_LTM_LTM_Mng[loop_cnt].input_size_rgb = ltm_ctrl_sdram_input->input_rect;
	}

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTRR   = ltm_ctrl_sdram_input->rch_burst_length_r;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTRG   = ltm_ctrl_sdram_input->rch_burst_length_g;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LAXIBSIZ.bit.BURSTRB   = ltm_ctrl_sdram_input->rch_burst_length_b;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDRDEFR2.bit.SDRDEFR2 = ltm_ctrl_sdram_input->input_global_rgb;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDRHSZR2.bit.SDRHSZR2 = ltm_ctrl_sdram_input->input_rect.img_width;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDRVSZR2.bit.SDRVSZR2 = ltm_ctrl_sdram_input->input_rect.img_lines;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LIOCTL.bit.INSEL       = D_IM_LTM_LTM_MODE_SDRAM_INPUT;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LIOCTL.bit.BYPASS      = D_IM_LTM_ENABLE_OFF;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Set Managed Input Address information(Main RGB images)
 */
INT32 Im_LTM_LTM_Set_InAddr_Info_Rgb( UCHAR pipe_no, const T_IM_LTM_INADDR_INFO* const in_addr )
{
	UINT32 offset;

#ifdef CO_PARAM_CHECK
	if( in_addr == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Set_InAddr_Info_Rgb error. in_addr = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Set_InAddr_Info_Rgb error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	offset = ( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDRDEFR2.bit.SDRDEFR2 * gIM_LTM_LTM_Mng[pipe_no].input_size_rgb.img_top ) + ( gIM_LTM_LTM_Mng[pipe_no].input_size_rgb.img_left * 2 );

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDRADR2R.bit.SDRADR2R = (UINT32)in_addr->addr_r + offset;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDRADR2G.bit.SDRADR2G = (UINT32)in_addr->addr_g + offset;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDRADR2B.bit.SDRADR2B = (UINT32)in_addr->addr_b + offset;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Set Managed Input Address/Offset information(Map Y images)
 */
INT32 Im_LTM_LTM_Set_InData_Info( UCHAR pipe_no, const T_IM_LTM_LTM_INDATA_INFO* const in_data_info )
{
	UINT32 offset_yp = 0;
	UINT32 offset_yn = 0;
	UCHAR bit_offset_yp = 0;
	UCHAR bit_offset_yn = 0;
	double coef_yp, coef_yn;

#ifdef CO_PARAM_CHECK
	if( in_data_info == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Set_InData_Info error. in_data_info = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Set_InData_Info error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	coef_yp = ( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LIOCTL.bit.R3DTYP == D_IM_LTM_DTYP_PACK12 ) ? ( 3.0 / 2.0 ) : ( 2.0 );
	coef_yn = ( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LIOCTL.bit.R4DTYP == D_IM_LTM_DTYP_PACK12 ) ? ( 3.0 / 2.0 ) : ( 2.0 );

	offset_yp = ( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDRDEFR3.bit.SDRDEFR3 * gIM_LTM_LTM_Mng[pipe_no].input_size_y.img_top );
	offset_yp += (UINT32)( gIM_LTM_LTM_Mng[pipe_no].input_size_y.img_left * coef_yp );
	if(( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LIOCTL.bit.R3DTYP == D_IM_LTM_DTYP_PACK12 )
	&& ( gIM_LTM_LTM_Mng[pipe_no].input_size_y.img_left & 1 )){
		bit_offset_yp = ( in_data_info->top_bit_offset_yp == D_IM_LTM_ENABLE_ON ) ? 0 : 1;
	}
	else{
		bit_offset_yp = in_data_info->top_bit_offset_yp;
	}

	offset_yn = ( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDRDEFR4.bit.SDRDEFR4 * gIM_LTM_LTM_Mng[pipe_no].input_size_y.img_top );
	offset_yn += (UINT32)( gIM_LTM_LTM_Mng[pipe_no].input_size_y.img_left * coef_yn );
	if(( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LIOCTL.bit.R4DTYP == D_IM_LTM_DTYP_PACK12 )
	&& ( gIM_LTM_LTM_Mng[pipe_no].input_size_y.img_left & 1 )){
		bit_offset_yn = ( in_data_info->top_bit_offset_yn == D_IM_LTM_ENABLE_ON ) ? 0 : 1;
	}
	else{
		bit_offset_yn = in_data_info->top_bit_offset_yn;
	}

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDRADR3.bit.SDRADR3 = (UINT32)in_data_info->addr_yp + offset_yp;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDROFS.bit.SDROFSR3 = bit_offset_yp;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDRADR4.bit.SDRADR4 = (UINT32)in_data_info->addr_yn + offset_yn;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDROFS.bit.SDROFSR4 = bit_offset_yn;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Set Managed output Address/Global width information
 */
INT32 Im_LTM_LTM_Set_OutData_Info( UCHAR pipe_no, const T_IM_LTM_LTM_OUTDATA_INFO* const out_data_info )
{
#ifdef CO_PARAM_CHECK
	if( out_data_info == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Set_OutData_Info error. out_data_info = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Set_OutData_Info error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDWADR.bit.SDWADR   = (UINT32)out_data_info->addr[D_IM_LTM_PORT_R];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDWDEFR.bit.SDWDEFR = out_data_info->global_width[D_IM_LTM_PORT_R];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDWADG.bit.SDWADG   = (UINT32)out_data_info->addr[D_IM_LTM_PORT_G];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDWDEFG.bit.SDWDEFG = out_data_info->global_width[D_IM_LTM_PORT_G];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDWADB.bit.SDWADB   = (UINT32)out_data_info->addr[D_IM_LTM_PORT_B];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDWDEFB.bit.SDWDEFB = out_data_info->global_width[D_IM_LTM_PORT_B];

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* LTM_LTM Control (B2R direct connection mode control)
 */
INT32 Im_LTM_LTM_Ctrl_ModeB2RDirect( UCHAR pipe_no, const T_IM_LTM_LTM_CTRL_B2R_DIRECT* const ltm_ctrl_b2r_direct )
{
	UCHAR	loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( ltm_ctrl_b2r_direct == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Ctrl_ModeB2RDirect error. ltm_ctrl_b2r_direct = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Ctrl_ModeB2RDirect error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_ltm_ltm_get_loop_val( pipe_no, &loop_sta, &loop_end );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_LTM_LTM_Mng[loop_cnt].input_size_rgb = ltm_ctrl_b2r_direct->input_rect;
	}

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LIOCTL.bit.INSEL       = D_IM_LTM_LTM_MODE_B2R_DIRECT;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDRHSZR2.bit.SDRHSZR2 = ltm_ctrl_b2r_direct->input_rect.img_width;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSDRVSZR2.bit.SDRVSZR2 = ltm_ctrl_b2r_direct->input_rect.img_lines;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LIOCTL.bit.BYPASS      = ltm_ctrl_b2r_direct->bypass;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* LTM LTM resize pitch Control
 */
INT32 Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto( UCHAR pipe_no )
{
	UCHAR tmp_pipe_no;
	USHORT rgb_width, rgb_lines, map_width, map_lines;
	UINT32 hsta, hpit, vsta, vpit;
	UINT32 tmp_hsz, tmp_vsz;


#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Set_MapImage_Resize_Pitch_Auto error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	tmp_pipe_no = ( pipe_no == D_IM_LTM_PIPE12 ) ? D_IM_LTM_PIPE1: pipe_no;

	Im_LTM_On_Pclk( pipe_no );

	rgb_width = gIM_Io_Ltm_Ltm_Reg_Ptr[tmp_pipe_no]->LSDRHSZR2.bit.SDRHSZR2;
	rgb_lines = gIM_Io_Ltm_Ltm_Reg_Ptr[tmp_pipe_no]->LSDRVSZR2.bit.SDRVSZR2;
	map_width = gIM_Io_Ltm_Ltm_Reg_Ptr[tmp_pipe_no]->LSDRHSZR3.bit.SDRHSZR3;
	map_lines = gIM_Io_Ltm_Ltm_Reg_Ptr[tmp_pipe_no]->LSDRVSZR3.bit.SDRVSZR3;

	hpit = 16384 * map_width / rgb_width;
	vpit = 16384 * map_lines / rgb_lines;

	tmp_hsz = ( map_width - 1 ) * 16384 / hpit + 1;
	tmp_vsz = ( map_lines - 1 ) * 16384 / vpit + 1;

	hsta = hpit * ( tmp_hsz - rgb_width ) / 2;
	vsta = vpit * ( tmp_vsz - rgb_lines ) / 2;

	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LHSTA, union io_ltm_ch_ltm_lhsta, HSTA, hsta );
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LHPIT.bit.HPIT = hpit;
	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LVSTA, union io_ltm_ch_ltm_lvsta, VSTA, vsta );
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LVPIT.bit.VPIT = vpit;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* LTM LTM resize pitch Control
 */
INT32 Im_LTM_LTM_Set_MapImage_Resize_Pitch( UCHAR pipe_no, const T_IM_LTM_LTM_MAP_RESIZE_PITCH* const resize_pitch )
{
#ifdef CO_PARAM_CHECK
	if( resize_pitch == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Set_MapImage_Resize_Pitch error. ltm_ctrl_b2r_direct = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Set_MapImage_Resize_Pitch error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LHSTA, union io_ltm_ch_ltm_lhsta, HSTA, resize_pitch->hsta );
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LHPIT.bit.HPIT = resize_pitch->hpit;
	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LVSTA, union io_ltm_ch_ltm_lvsta, VSTA, resize_pitch->vsta );
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LVPIT.bit.VPIT = resize_pitch->vpit;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Set Frame stop
 */
INT32 Im_LTM_LTM_Set_FrameStop( UCHAR pipe_no, UCHAR onoff )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Set_FrameStop error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LFRSTP.bit.FRSTP = onoff;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Set LTM parameter hold
*/
INT32 Im_LTM_LTM_Set_ParamHold( UCHAR pipe_no, const UCHAR hold_enable )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Set_ParamHold error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LPARMSK.bit.PARMSK = hold_enable;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Get LTM busy status.
 */
INT32 Im_LTM_LTM_Get_LtmBusy( UCHAR pipe_no, BOOL* const busy_status )
{
	ULONG ltmtrg;

#ifdef CO_PARAM_CHECK
	if( busy_status == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Get_LtmBusy error. busy_status = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Get_LtmBusy error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	ltmtrg = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LTRG.bit.TRG;

	Im_LTM_Off_Pclk( pipe_no );

	if( ltmtrg == D_IM_LTM_LTM_TRG_BUSY ) {
		*busy_status = TRUE;
	}
	else {
		*busy_status = FALSE;
	}
	return D_DDIM_OK;
}

/* Restart LTM process.
 */
INT32 Im_LTM_LTM_ContStart( UCHAR pipe_no )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_ContStart error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	// P-clock control
#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
	Ddim_Print(( "Im_LTM_LTM_ContStart on pclk\n" ));
#endif
	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSRAMEN.word = D_IM_LTM_LTM_LSRAMEN_ENABLE;

	im_ltm_ltm_wait_usec( D_IM_LTM_LTM_SRAM_WAIT_USEC );

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LTRG.bit.TRG = D_IM_LTM_LTM_TRG_ON;

#ifdef CO_DEBUG_ON_PC
	if( im_ltm_ltm_check_target_pipe_no_1(pipe_no) ){
		gIM_Io_Ltm_Ltm_Reg_Ptr[D_IM_LTM_PIPE1]->LTRG.bit.TRG = D_IM_LTM_LTM_TRG_BUSY;
	}
	if( im_ltm_ltm_check_target_pipe_no_2(pipe_no) ){
		gIM_Io_Ltm_Ltm_Reg_Ptr[D_IM_LTM_PIPE2]->LTRG.bit.TRG = D_IM_LTM_LTM_TRG_BUSY;
	}
#endif

#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
	Ddim_Print(( "Im_LTM_LTM_ContStart off pclk\n" ));
#endif
	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Start LTM process.
 */
INT32 Im_LTM_LTM_Start( UCHAR pipe_no )
{
	DDIM_USER_ER		ercd;
	INT32				retcd;
	UCHAR				loop_cnt, loop_sta, loop_end;
	DDIM_USER_FLGPTN	flgptn = 0;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Start error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
	Ddim_Print(( "Im_LTM_LTM_Start begin\n" ));
#endif

	// P-clock control
#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
	Ddim_Print(( "Im_LTM_LTM_Start on pclk\n" ));
#endif

	im_ltm_ltm_get_loop_val( pipe_no, &loop_sta, &loop_end );

	Im_LTM_On_Pclk( pipe_no );

	while( 1 ) {	// Pclk enable section
		// Clear of All Interrupt enable & Interrupt status
#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
		Ddim_Print(( "Im_LTM_LTM_Start begin initialize\n" ));
#endif

#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
		Ddim_Print(( "Im_LTM_LTM_Start initialize interrupt\n" ));
#endif
		// Initialized Interrupt
		gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LINT1.word = D_IM_LTM_LTM_LINT1_ALL_CLR;

		for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
			gIM_LTM_LTM_Mng[loop_cnt].int_status = 0;
		}

#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
		Ddim_Print(( "Im_LTM_LTM_Start enable interrupt\n" ));
#endif
		gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LINT0.word = D_IM_LTM_LTM_LINT0_ENABLE;

#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
		Ddim_Print(( "Im_LTM_LTM_Start on clk\n" ));
#endif
		Im_LTM_On_Hclk( pipe_no );
		Im_LTM_On_Aclk( pipe_no );
		Im_LTM_On_Clk( pipe_no );

		im_ltm_ltm_wait_usec( D_IM_LTM_LTM_SRAM_WAIT_USEC );


#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
		Ddim_Print(( "Im_LTM_LTM_Start clear event flag\n" ));
#endif

		// Clear of Event Flag
		if( im_ltm_ltm_check_target_pipe_no_1( pipe_no ) ){
			flgptn |= D_IM_LTM_LTM1_INT_FLG_ALL;
		}
		if( im_ltm_ltm_check_target_pipe_no_2( pipe_no ) ){
			flgptn |= D_IM_LTM_LTM2_INT_FLG_ALL;
		}

		ercd = DDIM_User_Clr_Flg( FID_IM_LTM_LTM, ~flgptn );
		if( ercd != D_DDIM_USER_E_OK ) {
			Ddim_Print(( "Im_LTM_LTM_Start: invalid FID\n" ));
			retcd = D_IM_LTM_SYSTEM_ERROR;
			break;
		}

		gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSRAMEN.word = D_IM_LTM_LTM_LSRAMEN_ENABLE;

		im_ltm_ltm_wait_usec( D_IM_LTM_LTM_SRAM_WAIT_USEC );

		gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LTRG.bit.TRG = D_IM_LTM_LTM_TRG_ON;
#ifdef CO_DEBUG_ON_PC
		for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
			gIM_Io_Ltm_Ltm_Reg_Ptr[loop_cnt]->LTRG.bit.TRG = D_IM_LTM_LTM_TRG_BUSY;
		}
#endif
		retcd = D_DDIM_OK;
		break;
	}

#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
	Ddim_Print(( "Im_LTM_LTM_Start off pclk\n" ));
#endif
	Im_LTM_Off_Pclk( pipe_no );

#ifdef D_IM_LTM_LTM_DEBUG_ON_PC
	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_Io_Ltm_Ltm_Reg_Ptr[loop_cnt]->LINT1.word = 0;
		gIM_Io_Ltm_Ltm_Reg_Ptr[loop_cnt]->LINT1.bit.__FINFL = 1;
		if( gIM_Io_Ltm_Ltm_Reg_Ptr[loop_cnt]->LLINTLV.bit.LINTLV != 0 ){
			gIM_Io_Ltm_Ltm_Reg_Ptr[loop_cnt]->LINT1.bit.__LCNTFL = 1;
		}
		Im_LTM_LTM_Int_Handler( loop_cnt );
		gIM_Io_Ltm_Ltm_Reg_Ptr[loop_cnt]->LINT1.word = 0;
	}
#endif

	Im_LTM_Dsb();

#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
	Ddim_Print(( "Im_LTM_LTM_Start end\n" ));
#endif

	return retcd;
}

/* Stop LTM LTM process
 */
INT32 Im_LTM_LTM_Stop( UCHAR pipe_no )
{
	UCHAR loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Stop error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_ltm_ltm_get_loop_val( pipe_no, &loop_sta, &loop_end );

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LTRG.bit.TRG = D_IM_LTM_LTM_TRG_OFF;

	Im_LTM_Dsb();
	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		while( gIM_Io_Ltm_Ltm_Reg_Ptr[loop_cnt]->LTRG.bit.TRG != D_IM_LTM_LTM_TRG_IDLE ) {
#ifdef CO_DEBUG_ON_PC
//			Ddim_Print(( "Im_LTM_LTM_Stop wait loop. %u\n", gIM_Io_Ltm_Ltm_Reg_Ptr[loop_cnt]->LTRG.bit.TRG ));
			gIM_Io_Ltm_Ltm_Reg_Ptr[loop_cnt]->LTRG.bit.TRG = D_IM_LTM_LTM_TRG_IDLE;
#endif
			;	// DO NOTHING
		}
	}

	// Clear of All Interrupt disable
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LINT0.word = D_IM_LTM_LTM_LINT0_DISABLE;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LINT1.word = D_IM_LTM_LTM_LINT1_ALL_CLR;

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSRAMEN.word = D_IM_LTM_LTM_LSRAMEN_DISABLE;

	Im_LTM_Off_Clk( pipe_no );
	Im_LTM_Off_Aclk( pipe_no );
	Im_LTM_Off_Hclk( pipe_no );
	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Wait end of LTM LTM macro.
 */
INT32 Im_LTM_LTM_WaitEnd( DDIM_USER_FLGPTN* const p_flgptn, DDIM_USER_FLGPTN waiptn, DDIM_USER_TMO tmout )
{
	DDIM_USER_FLGPTN	flgptn;
	DDIM_USER_ER		ercd;

#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
	Ddim_Print(( "Im_LTM_LTM_WaitEnd begin\n" ));
#endif

#ifdef CO_PARAM_CHECK
	if( (waiptn & D_IM_LTM_LTM_INT_FLG_ALL) != waiptn ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_WaitEnd error. waiptn\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif
	Im_LTM_Dsb();

	switch( DDIM_User_Twai_Flg( FID_IM_LTM_LTM, waiptn, D_DDIM_USER_TWF_ORW, &flgptn, tmout ) ) {
		case D_DDIM_USER_E_OK:
#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
		Ddim_Print(( "Im_LTM_LTM_WaitEnd twai_flg OK return\n" ));
#endif
			flgptn = waiptn & flgptn;
			ercd = DDIM_User_Clr_Flg( FID_IM_LTM_LTM, ~flgptn );
			if( ercd != D_DDIM_USER_E_OK ) {
				Ddim_Print(( "Im_LTM_LTM_WaitEnd: invalid FID.\n" ));
				return D_IM_LTM_PARAM_ERROR;
			}
			if( p_flgptn != NULL ) {
				*p_flgptn = flgptn;
			}
			return D_DDIM_OK;
		case D_DDIM_USER_E_TMOUT:
#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
		Ddim_Print(( "Im_LTM_LTM_WaitEnd twai_flg TimeOut return\n" ));
#endif
			return D_IM_LTM_TIMEOUT;
		default:
#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
		Ddim_Print(( "Im_LTM_LTM_WaitEnd twai_flg NG return\n" ));
#endif
			return D_IM_LTM_PARAM_ERROR;
	}
}

/* LTM_LTM Interrupt Handler
 */
VOID Im_LTM_LTM_Int_Handler( UCHAR pipe_no )
{
	UINT32			loopcnt;
	DDIM_USER_ER	ercd;
	ULONG			lint0;
	ULONG			lint1;
	ULONG			lint1_clr;

#ifdef CO_PARAM_CHECK
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Int_Handler error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return;
	}
#endif	// CO_PARAM_CHECK

#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
	Ddim_Print(( "Im_LTM_LTM_Int_Handler begin\n" ));
#endif

#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
	Ddim_Print(( "Im_LTM_LTM_Int_Handler on pclk\n" ));
#endif
	Im_LTM_On_Pclk( pipe_no );

	lint0 = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LINT0.word;
	lint1 = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LINT1.word;
	lint1_clr = 0;

#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
	Ddim_Print(( "Im_LTM_LTM_Int_Handler lint0=0x%lx, lint1=0x%lx\n", lint0, lint1 ));
#endif

	// Call of Callback API for interrupt status

	for( loopcnt = 0; loopcnt < (sizeof(gim_ltm_ltm_inttbl[0]) / sizeof(gim_ltm_ltm_inttbl[0][0])); loopcnt++ ) {
		if( ((lint1 & gim_ltm_ltm_inttbl[pipe_no][loopcnt].flg_bitmask) != 0)
		 && ((lint0 & gim_ltm_ltm_inttbl[pipe_no][loopcnt].ena_bitmask) != 0) ) {

#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
			Ddim_Print(( "Im_LTM_LTM_Int_Handler begin 0x%x\n", gim_ltm_ltm_inttbl[pipe_no][loopcnt].flg_bitmask ));
#endif

			gIM_LTM_LTM_Mng[pipe_no].int_status = gim_ltm_ltm_inttbl[pipe_no][loopcnt].int_status;

			lint1_clr |= gim_ltm_ltm_inttbl[pipe_no][loopcnt].flg_bitmask;

			ercd = DDIM_User_Set_Flg( FID_IM_LTM_LTM, gim_ltm_ltm_inttbl[pipe_no][loopcnt].waitflg );
			if( ercd != D_DDIM_USER_E_OK ) {
				Ddim_Print(( "Im_LTM_LTM_Int_Handler: invalid FID. 0x%lx\n", gim_ltm_ltm_inttbl[pipe_no][loopcnt].waitflg ));
			}

			if (gIM_LTM_LTM_Mng[pipe_no].ltm_user_handler) {
				gIM_LTM_LTM_Mng[pipe_no].ltm_user_handler( (UINT32*)&gIM_LTM_LTM_Mng[pipe_no].int_status, gIM_LTM_LTM_Mng[pipe_no].user_param );
			}
		}
	}

	// Clear interrupt
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LINT1.word = lint1_clr;


#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
	Ddim_Print(( "Im_LTM_LTM_Int_Handler lint1=0x%lx\n", gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LINT1.word ));
#endif

#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
	Ddim_Print(( "Im_LTM_LTM_Int_Handler off pclk\n" ));
#endif
	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

#ifdef CO_DEBUG_PRINT_IM_LTM_LTM
	Ddim_Print(( "Im_LTM_LTM_Int_Handler end\n" ));
#endif
}

/* Set LTM High frequency strength
*/
INT32 Im_LTM_LTM_Set_High_Frequency_Strength( UCHAR pipe_no, const T_IM_LTM_LTM_CTRL_HF_STRENGTH* hf_ctrl )
{
#ifdef CO_PARAM_CHECK
	if( hf_ctrl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Set_High_Frequency_Strength error. hf_ctrl = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Set_High_Frequency_Strength error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->NLVTHOF7.bit.NLVTHOF_7 = hf_ctrl->strength;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->PLVTHOF7.bit.PLVTHOF_7 = hf_ctrl->strength;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Set internal parameter
 */
INT32 Im_LTM_LTM_Set_Internal_Param( UCHAR pipe_no, T_IM_LTM_INTERNAL_PARAM* const param )
{
#ifdef CO_PARAM_CHECK
	if( param == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Set_Internal_Param error. param = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Set_Internal_Param error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LYSHADOW.bit.YSHADOW       = param->yshadow;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LYHIGHLIGHT.bit.YHIGHLIGHT = param->yhighlight;

	Im_LTM_Off_Pclk( pipe_no );
	return D_DDIM_OK;
}

/* LTM Blend control
*/
INT32 Im_LTM_LTM_Ctrl_Blend( UCHAR pipe_no, const T_IM_LTM_LTM_CTRL_BLEND* blend_ctrl )
{
#ifdef CO_PARAM_CHECK
	if( blend_ctrl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Ctrl_Blend error. blend_ctrl = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Ctrl_Blend error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDYRT.bit.BLDYRT        = blend_ctrl->ratio;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDOF.BLDOF1.bit.BLDOF_0 = blend_ctrl->offset[0];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDOF.BLDOF1.bit.BLDOF_1 = blend_ctrl->offset[1];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDOF.BLDOF2.bit.BLDOF_2 = blend_ctrl->offset[2];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDOF.BLDOF2.bit.BLDOF_3 = blend_ctrl->offset[3];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDOF.BLDOF3.bit.BLDOF_4 = blend_ctrl->offset[4];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDOF.BLDOF3.bit.BLDOF_5 = blend_ctrl->offset[5];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDOF.BLDOF4.bit.BLDOF_6 = blend_ctrl->offset[6];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDOF.BLDOF4.bit.BLDOF_7 = blend_ctrl->offset[7];

	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDGA.BLDGA1, union io_ltm_ch_ltm_bldga_1, BLDGA_0, blend_ctrl->gain[0] );
	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDGA.BLDGA1, union io_ltm_ch_ltm_bldga_1, BLDGA_1, blend_ctrl->gain[1] );
	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDGA.BLDGA2, union io_ltm_ch_ltm_bldga_2, BLDGA_2, blend_ctrl->gain[2] );
	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDGA.BLDGA2, union io_ltm_ch_ltm_bldga_2, BLDGA_3, blend_ctrl->gain[3] );
	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDGA.BLDGA3, union io_ltm_ch_ltm_bldga_3, BLDGA_4, blend_ctrl->gain[4] );
	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDGA.BLDGA3, union io_ltm_ch_ltm_bldga_3, BLDGA_5, blend_ctrl->gain[5] );
	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDGA.BLDGA4, union io_ltm_ch_ltm_bldga_4, BLDGA_6, blend_ctrl->gain[6] );
	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDGA.BLDGA4, union io_ltm_ch_ltm_bldga_4, BLDGA_7, blend_ctrl->gain[7] );

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDBD.BLDBD1.bit.BLDBD_0 = blend_ctrl->border[0];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDBD.BLDBD1.bit.BLDBD_1 = blend_ctrl->border[1];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDBD.BLDBD2.bit.BLDBD_2 = blend_ctrl->border[2];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDBD.BLDBD2.bit.BLDBD_3 = blend_ctrl->border[3];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDBD.BLDBD3.bit.BLDBD_4 = blend_ctrl->border[4];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDBD.BLDBD3.bit.BLDBD_5 = blend_ctrl->border[5];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDBD.BLDBD4.bit.BLDBD_6 = blend_ctrl->border[6];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->BLDBD.BLDBD4.bit.BLDBD_7 = blend_ctrl->border[7];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LTMMD.bit.LTMMD          = blend_ctrl->mode;

	if( blend_ctrl->mode == 1 ){
		gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->RCGMD.bit.YSHMD = 1;
		gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->RCGMD.bit.RCGMD = 0;
	}
	else{
		gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->RCGMD.bit.YSHMD = 0;
		gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->RCGMD.bit.RCGMD = 1;
	}
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->RCGMD.bit.RCGAMD = 0;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* LTM RCG table control
*/
INT32 Im_LTM_LTM_Set_RCG_Table( UCHAR pipe_no, const USHORT* const src_tbl )
{
	UINT32 loop_cnt;

#ifdef CO_PARAM_CHECK
	// check on input pointer
	if(src_tbl == NULL) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Set_RCG_Table error. src_tbl = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Set_RCG_Table error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif

	Im_LTM_On_Pclk( pipe_no );

	for( loop_cnt = 0; loop_cnt < D_IM_LTM_TABLE_MAX_RCG; loop_cnt++ ) {
		gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->RCGTBL.hword[loop_cnt] = src_tbl[loop_cnt];
	}

	Im_LTM_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* LTM chroma correction strength control
*/
INT32 Im_LTM_LTM_Ctrl_Chroma_Correction_Strength( UCHAR pipe_no, const T_IM_LTM_LTM_CTRL_CHROMA* chroma_ctrl )
{
#ifdef CO_PARAM_CHECK
	if( chroma_ctrl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Ctrl_Chroma_Correction_Strength error. chroma_ctrl = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Ctrl_Chroma_Correction_Strength error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGOF.CGOF1.bit.CGOF_0 = chroma_ctrl->offset[0];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGOF.CGOF1.bit.CGOF_1 = chroma_ctrl->offset[1];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGOF.CGOF2.bit.CGOF_2 = chroma_ctrl->offset[2];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGOF.CGOF2.bit.CGOF_3 = chroma_ctrl->offset[3];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGOF.CGOF3.bit.CGOF_4 = chroma_ctrl->offset[4];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGOF.CGOF3.bit.CGOF_5 = chroma_ctrl->offset[5];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGOF.CGOF4.bit.CGOF_6 = chroma_ctrl->offset[6];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGOF.CGOF4.bit.CGOF_7 = chroma_ctrl->offset[7];

	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGGA.CGGA1, union io_ltm_ch_ltm_cgga_1, CGGA_0, chroma_ctrl->gain[0] );
	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGGA.CGGA1, union io_ltm_ch_ltm_cgga_1, CGGA_1, chroma_ctrl->gain[1] );
	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGGA.CGGA2, union io_ltm_ch_ltm_cgga_2, CGGA_2, chroma_ctrl->gain[2] );
	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGGA.CGGA2, union io_ltm_ch_ltm_cgga_2, CGGA_3, chroma_ctrl->gain[3] );
	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGGA.CGGA3, union io_ltm_ch_ltm_cgga_3, CGGA_4, chroma_ctrl->gain[4] );
	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGGA.CGGA3, union io_ltm_ch_ltm_cgga_3, CGGA_5, chroma_ctrl->gain[5] );
	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGGA.CGGA4, union io_ltm_ch_ltm_cgga_4, CGGA_6, chroma_ctrl->gain[6] );
	im_ltm_ltm_set_reg_signed( gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGGA.CGGA4, union io_ltm_ch_ltm_cgga_4, CGGA_7, chroma_ctrl->gain[7] );

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGBD.CGBD1.bit.CGBD_1 = chroma_ctrl->border[0];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGBD.CGBD2.bit.CGBD_2 = chroma_ctrl->border[1];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGBD.CGBD2.bit.CGBD_3 = chroma_ctrl->border[2];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGBD.CGBD3.bit.CGBD_4 = chroma_ctrl->border[3];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGBD.CGBD3.bit.CGBD_5 = chroma_ctrl->border[4];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGBD.CGBD4.bit.CGBD_6 = chroma_ctrl->border[5];
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->CGBD.CGBD4.bit.CGBD_7 = chroma_ctrl->border[6];

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* YLOG table control
 */
INT32 Im_LTM_LTM_Ctrl_Ylog_Tbl( UCHAR pipe_no, const T_IM_LTM_CTRL_YLOG* const ylog_ctrl )
{
#ifdef CO_PARAM_CHECK
	if( ylog_ctrl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Ctrl_Ylog_Tbl error. ylog_ctrl = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Ctrl_Ylog_Tbl error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LYLCTL.bit.YLOGSL      = ylog_ctrl->use_tbl;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LYLOG0EP0.bit.YLOG0EP0 = ylog_ctrl->end_point0_tbl0;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LYLOG0EP1.bit.YLOG0EP1 = ylog_ctrl->end_point1_tbl0;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LYLOG1EP0.bit.YLOG1EP0 = ylog_ctrl->end_point0_tbl1;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LYLOG1EP1.bit.YLOG1EP1 = ylog_ctrl->end_point1_tbl1;

	Im_LTM_Off_Pclk( pipe_no );
	return D_DDIM_OK;
}

/* Get active YLOG table No.
 */
INT32 Im_LTM_LTM_Get_Active_Ylog_Tbl_No( UCHAR pipe_no, UCHAR* const tbl_no )
{
#ifdef CO_PARAM_CHECK
	if( tbl_no == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Get_Active_Ylog_Tbl_No error. tbl_no = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Get_Active_Ylog_Tbl_No error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	*tbl_no = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LYLCTL.bit.YLOGSTA;

	Im_LTM_Off_Pclk( pipe_no );
	return D_DDIM_OK;
}

/* Set YLOG table.
 */
INT32 Im_LTM_LTM_Set_YLOG_Table( UCHAR pipe_no, UCHAR tbl_sel, const USHORT* const src_tbl )
{
	UINT32 sram_en;
#ifndef CO_LTM_RDMA_ON
	UINT32 loop_cnt;
	volatile USHORT* dst_tbl;
#endif	// CO_LTM_RDMA_ON

#ifdef CO_PARAM_CHECK
	// check on input pointer
	if(src_tbl == NULL) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_LTM_Set_YLOG_Table error. src_tbl = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_LTM_Set_YLOG_Table error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif

	Im_LTM_On_Pclk( pipe_no );

	sram_en = gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSRAMEN.word;
	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSRAMEN.word = D_IM_LTM_LTM_LSRAMEN_ENABLE;

#ifdef CO_LTM_RDMA_ON
	im_ltm_ltm_set_rdma_val_ylog_table( pipe_no, src_tbl );
#else	// CO_LTM_RDMA_ON
	Im_LTM_On_Hclk( pipe_no );

	switch( tbl_sel ){
		case D_LTM_YLOG_TBL_00:
			dst_tbl = gIM_Io_Ltm_Ltm_Tbl_Ptr[pipe_no]->LYLOG0TBL0.hword;
			break;
		case D_LTM_YLOG_TBL_10:
			dst_tbl = gIM_Io_Ltm_Ltm_Tbl_Ptr[pipe_no]->LYLOG1TBL0.hword;
			break;
		case D_LTM_YLOG_TBL_01:
			dst_tbl = gIM_Io_Ltm_Ltm_Tbl_Ptr[pipe_no]->LYLOG0TBL1.hword;
			break;
		case D_LTM_YLOG_TBL_11:
			dst_tbl = gIM_Io_Ltm_Ltm_Tbl_Ptr[pipe_no]->LYLOG1TBL1.hword;
			break;
		default:
			return D_IM_LTM_PARAM_ERROR;
	}

	for( loop_cnt = 0; loop_cnt < D_IM_LTM_TABLE_MAX_YLOG; loop_cnt++ ) {
		dst_tbl[loop_cnt] = src_tbl[loop_cnt];
	}

	Im_LTM_Off_Hclk( pipe_no );
#endif	// CO_LTM_RDMA_ON

	gIM_Io_Ltm_Ltm_Reg_Ptr[pipe_no]->LSRAMEN.word = sram_en;

	Im_LTM_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

#ifdef IM_LTM_LTM_STATUS_PRINT
VOID Im_LTM_LTM_Print_Status( VOID )
{
	UINT32 loopcnt;

	for( loopcnt = 0; loopcnt < 2; loopcnt++ ) {
		Ddim_Print(( "PIPE%u:\n", loopcnt ));
		Ddim_Print(( "\tinput_size_rgb\n" ));
		Ddim_Print(( "\t\timg_left            = 0x%x\n", gIM_LTM_LTM_Mng[loopcnt].input_size_rgb.img_left ));
		Ddim_Print(( "\t\timg_top             = 0x%x\n", gIM_LTM_LTM_Mng[loopcnt].input_size_rgb.img_top ));
		Ddim_Print(( "\t\timg_width           = 0x%x\n", gIM_LTM_LTM_Mng[loopcnt].input_size_rgb.img_width ));
		Ddim_Print(( "\t\timg_lines           = 0x%x\n", gIM_LTM_LTM_Mng[loopcnt].input_size_rgb.img_lines ));
		Ddim_Print(( "\tinput_size_y\n" ));
		Ddim_Print(( "\t\timg_left            = 0x%x\n", gIM_LTM_LTM_Mng[loopcnt].input_size_y.img_left ));
		Ddim_Print(( "\t\timg_top             = 0x%x\n", gIM_LTM_LTM_Mng[loopcnt].input_size_y.img_top ));
		Ddim_Print(( "\t\timg_width           = 0x%x\n", gIM_LTM_LTM_Mng[loopcnt].input_size_y.img_width ));
		Ddim_Print(( "\t\timg_lines           = 0x%x\n", gIM_LTM_LTM_Mng[loopcnt].input_size_y.img_lines ));
		Ddim_Print(( "\tint_status            = 0x%x\n", gIM_LTM_LTM_Mng[loopcnt].int_status ));
		Ddim_Print(( "\tltm_user_handler      = 0x%x\n", (UINT32)gIM_LTM_LTM_Mng[loopcnt].ltm_user_handler ));
		Ddim_Print(( "\tuser_param            = 0x%x\n", (UINT32)gIM_LTM_LTM_Mng[loopcnt].user_param ));
	}
}

VOID Im_LTM_LTM_Print_AccEnStatus( VOID )
{
	UINT32 loopcnt;

	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	for( loopcnt = 0; loopcnt < 2; loopcnt++ ) {
		Ddim_Print(( "SRAMAEN  = 0x%x\n", gIM_Io_Ltm_Ltm_Reg_Ptr[loopcnt]->LSRAMEN.bit.SRAMAEN ));
		Ddim_Print(( "RMAENRGB = 0x%x\n", gIM_Io_Ltm_Ltm_Reg_Ptr[loopcnt]->LSRAMEN.bit.RMAENEGB ));
		Ddim_Print(( "RMAENYN  = 0x%x\n", gIM_Io_Ltm_Ltm_Reg_Ptr[loopcnt]->LSRAMEN.bit.RMAENYN ));
	}
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
}
#endif

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
#endif	// CO_DDIM_UTILITY_USE
