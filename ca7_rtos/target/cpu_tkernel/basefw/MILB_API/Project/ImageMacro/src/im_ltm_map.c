/**
 * @file		im_ltm_map.c
 * @brief		Local Tone Mapping process(Map Block)
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

// MSRAMEN value
#define D_IM_LTM_MAP_MSRAMEN_ENABLE		(0x000000B1)
#define D_IM_LTM_MAP_MSRAMEN_DISABLE	(0x00000000)

// PIPE count
#define D_IM_LTM_PIPE_COUNT				(2)

// MINT0 value
#define D_IM_LTM_MAP_MINT0_ENABLE		(0x02311001)
#define D_IM_LTM_MAP_MINT0_DISABLE		(0x00000000)
// MINT1 value
#define D_IM_LTM_MAP_MINT1_ALL_CLR		(0x02301001)

// register bit pattern
#define D_IM_LTM_MAP_TRG_OFF			(0)
#define D_IM_LTM_MAP_TRG_ON				(1)
#define D_IM_LTM_MAP_TRG_IDLE			(2)
#define D_IM_LTM_MAP_TRG_BUSY			(3)

// Input mode
#define D_IM_LTM_MAP_MODE_RBK_DIRECT	(0)
#define D_IM_LTM_MAP_MODE_SDRAM_INPUT	(1)

#define D_IM_LTM_MAP_SRAM_WAIT_USEC		(1)

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
#define im_ltm_map_check_target_pipe_no_1(a)		(((a) + 1) & (D_IM_LTM_PIPE1 + 1))
#define im_ltm_map_check_target_pipe_no_2(a)		(((a) + 1) & (D_IM_LTM_PIPE2 + 1))


// wait usec
#define im_ltm_map_wait_usec( usec )			Dd_ARM_Wait_ns( (usec) * 1000 )


#ifdef CO_ASSERT_ON
#define M_IM_LTM_MAP_ASSETION_MSG( msg )		(msg)
#else
#define M_IM_LTM_MAP_ASSETION_MSG( msg )		(NULL)
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
	VOID					(*map_user_handler)(UINT32* int_status, UINT32 user_param);	// Interrupt Handler
	UINT32					user_param;						// return to callback argument value when interrupt occurs
} T_IM_LTM_MAP_MNG;

// Interrupt parameter table
typedef struct {
	ULONG				flg_bitmask;		// Bitmask of interrupt flag.
	ULONG				ena_bitmask;		// Bitmask of inaerrupt enable.
	ULONG				waitflg;			// Value of set_flg.
	ULONG				int_status;			// Value of gIM_LTM_MAP_Mng.int_status.
} T_IM_LTM_MAP_INTFLG_TBL;


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
static volatile T_IM_LTM_MAP_MNG gIM_LTM_MAP_Mng[D_IM_LTM_PIPE_COUNT];
static volatile struct io_ltm_ch_map* gIM_Io_Ltm_Map_Reg_Ptr[D_IM_LTM_PIPE_MAX] = { &(IO_LTM_P1.MAP), &(IO_LTM_P2.MAP), &(IO_LTM_P3.MAP) };
static volatile struct io_ltm_map_sram* gIM_Io_Ltm_Map_Tbl_Ptr[D_IM_LTM_PIPE_MAX] = { &(IO_LTM_TBL_P1.MAP_SRAM), &(IO_LTM_TBL_P2.MAP_SRAM), &(IO_LTM_TBL_P3.MAP_SRAM) };


//
// Interrupt parameter table
//
static const T_IM_LTM_MAP_INTFLG_TBL gim_ltm_map_inttbl[D_IM_LTM_PIPE_COUNT][6] = {
	{
		{ 0x00000001, 0x00000001, D_IM_LTM_MAP1_INT_STATE_MAP_END,      D_IM_LTM_MAP1_INT_FLG_MAP_END      },	// FINEN
		{ 0x00001000, 0x00001000, D_IM_LTM_MAP1_INT_STATE_LINE_END,     D_IM_LTM_MAP1_INT_FLG_LINE_END     },	// LCNTEN
		{ 0x00010000, 0x00010000, D_IM_LTM_MAP1_INT_STATE_HISTINI_END,  D_IM_LTM_MAP1_INT_FLG_HISTINI_END  },	// LCNTEN
		{ 0x00100000, 0x00100000, D_IM_LTM_MAP1_INT_STATE_AXR_ERR,      D_IM_LTM_MAP1_INT_FLG_AXR_ERR      },	// AXREEN
		{ 0x00200000, 0x00200000, D_IM_LTM_MAP1_INT_STATE_AXW_ERR,      D_IM_LTM_MAP1_INT_FLG_AXW_ERR      },	// AXWEEN
		{ 0x02000000, 0x02000000, D_IM_LTM_MAP1_INT_STATE_YLOG_ERR,     D_IM_LTM_MAP1_INT_FLG_YLOG_ERR     },	// RAMEEN
	},
	{
		{ 0x00000001, 0x00000001, D_IM_LTM_MAP2_INT_STATE_MAP_END,      D_IM_LTM_MAP2_INT_FLG_MAP_END      },	// FINEN
		{ 0x00001000, 0x00001000, D_IM_LTM_MAP2_INT_STATE_LINE_END,     D_IM_LTM_MAP2_INT_FLG_LINE_END     },	// LCNTEN
		{ 0x00010000, 0x00010000, D_IM_LTM_MAP2_INT_STATE_HISTINI_END,  D_IM_LTM_MAP2_INT_FLG_HISTINI_END  },	// LCNTEN
		{ 0x00100000, 0x00100000, D_IM_LTM_MAP2_INT_STATE_AXR_ERR,      D_IM_LTM_MAP2_INT_FLG_AXR_ERR      },	// AXREEN
		{ 0x00200000, 0x00200000, D_IM_LTM_MAP2_INT_STATE_AXW_ERR,      D_IM_LTM_MAP2_INT_FLG_AXW_ERR      },	// AXWEEN
		{ 0x02000000, 0x02000000, D_IM_LTM_MAP2_INT_STATE_YLOG_ERR,     D_IM_LTM_MAP2_INT_FLG_YLOG_ERR     },	// RAMEEN
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
static VOID im_ltm_map_manager_init( UCHAR pipe_no )
{
	UCHAR size_coef = 1;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "im_ltm_map_manager_init error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return;
	}
#endif	// CO_PARAM_CHECK

	if( pipe_no == D_IM_LTM_PIPE12 ){
		size_coef = 2;
		pipe_no = 0;
	}

	// Initialize of State & Handler management information
	memset( (USHORT*)&gIM_LTM_MAP_Mng[pipe_no], 0, sizeof(T_IM_LTM_MAP_MNG) * size_coef );

	return;
}

static VOID im_ltm_map_get_loop_val( UCHAR pipe_no, UCHAR* loop_sta, UCHAR* loop_end )
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

/* LTM_MAP Initialize
 */
INT32 Im_LTM_MAP_Init( UCHAR pipe_no )
{
	ULONG clksel[2] = { 0, 0 };
	UCHAR clkstop[2][3] = {{ 0, 0, 0 }, { 0, 0, 0 }};	// SROxCLK/SROxCLK_2/LTMRBKxCLK

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Init error. pipe_no>D_IM_LTM_PIPE12\n" ));
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

	im_ltm_map_manager_init( pipe_no );

	Im_LTM_On_Pclk( pipe_no );
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAPSR.bit.SR = D_IM_LTM_SR_RELEASE;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MSRAMEN.word = D_IM_LTM_MAP_MSRAMEN_ENABLE;
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

/* LTM_MAP SW reset
 */
INT32 Im_LTM_MAP_SW_Reset( UCHAR pipe_no )
{
	UCHAR	loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_SW_Reset error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_ltm_map_get_loop_val( pipe_no, &loop_sta, &loop_end );

	Im_LTM_On_Pclk( pipe_no );
	Im_LTM_On_RBKClk( pipe_no );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		if( gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MTRG.bit.TRG == D_IM_LTM_MAP_TRG_BUSY ){
			return D_IM_LTM_MACRO_BUSY;
		}
	}
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAPSR.bit.SR = D_IM_LTM_SR_RESET;

	Im_LTM_Off_RBKClk( pipe_no );
	Im_LTM_Off_Pclk( pipe_no );

	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* set AXI bus I/F Control
 */
INT32 Im_LTM_MAP_Ctrl_Axi( UCHAR pipe_no, const T_IM_LTM_MAP_CTRL_AXI* const ctrl_axi )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( ctrl_axi == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_MAP_Ctrl_Axi error. ctrl_axi = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Ctrl_Axi error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXICTLR.bit.ARCACHE = ctrl_axi->rch.cache_type;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXICTLR.bit.ARPROT  = ctrl_axi->rch.protect_type;

	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXICTLW.bit.AWCACHE_YP = ctrl_axi->wch[D_IM_LTM_PORT_Y_YP].cache_type;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXICTLW.bit.AWPROT_YP  = ctrl_axi->wch[D_IM_LTM_PORT_Y_YP].protect_type;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXICTLW.bit.AWCACHE_YN = ctrl_axi->wch[D_IM_LTM_PORT_Y_YN].cache_type;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXICTLW.bit.AWPROT_YN  = ctrl_axi->wch[D_IM_LTM_PORT_Y_YN].protect_type;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXICTLW.bit.AWCACHE_DB = 0;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXICTLW.bit.AWPROT_DB  = 0;
	Im_LTM_Off_Pclk( pipe_no );

	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* get AXI bus read channel status
 */
INT32 Im_LTM_MAP_Get_AxiReadStat( UCHAR pipe_no, T_IM_LTM_MAP_AXI_RCH_STAT* const axi_read_stat )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( axi_read_stat == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_MAP_Get_AxiReadStat error. axi_read_stat = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Get_AxiReadStat error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );
	axi_read_stat->rch_axi_stat = gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXIRERR.bit.RRESP;
	Im_LTM_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* get AXI bus write channel status
 */
INT32 Im_LTM_MAP_Get_AxiWriteStat( UCHAR pipe_no, T_IM_LTM_MAP_AXI_WCH_STAT* const axi_write_stat )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( axi_write_stat == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_MAP_Get_AxiWriteStat error. axi_write_stat = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Get_AxiWriteStat error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );
	axi_write_stat->wch_axi_stat[D_IM_LTM_PORT_1] = gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXIWERR.bit.WRESP_YP;
	axi_write_stat->wch_axi_stat[D_IM_LTM_PORT_2] = gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXIWERR.bit.WRESP_YN;
	Im_LTM_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* get AXI bus ctrl param
 */
INT32 Im_LTM_MAP_Get_AxiCtrlParam( UCHAR pipe_no, T_IM_LTM_MAP_AXI_CTRL_PARAM* const axi_ctrl_prm )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( axi_ctrl_prm == NULL ){
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_MAP_Get_AxiCtrlParam error. axi_ctrl_prm = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Get_AxiCtrlParam error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );
	axi_ctrl_prm->rch_burst_length    = gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXIBSIZ.bit.BURSTR;
	axi_ctrl_prm->wch_burst_length_yp = gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXIBSIZ.bit.BURSTWYP;
	axi_ctrl_prm->wch_burst_length_yn = gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXIBSIZ.bit.BURSTWYN;
	axi_ctrl_prm->mask_write_yp       = gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXIBSIZ.bit.WSTBMSKYP;
	axi_ctrl_prm->mask_write_yn       = gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXIBSIZ.bit.WSTBMSKYN;
	axi_ctrl_prm->write_select_yp     = gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->AXICHSEL.bit.AXIWSEL1;
	axi_ctrl_prm->write_select_yn     = gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->AXICHSEL.bit.AXIWSEL2;
	Im_LTM_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* set AXI bus ctrl param
 */
INT32 Im_LTM_MAP_Set_AxiCtrlParam( UCHAR pipe_no, const T_IM_LTM_MAP_AXI_CTRL_PARAM* const axi_ctrl_prm )
{
#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( axi_ctrl_prm == NULL ){
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_MAP_Set_AxiCtrlParam error. axi_ctrl_prm = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Set_AxiCtrlParam error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXIBSIZ.bit.BURSTR    = axi_ctrl_prm->rch_burst_length;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXIBSIZ.bit.BURSTWYP  = axi_ctrl_prm->wch_burst_length_yp;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXIBSIZ.bit.BURSTWYN  = axi_ctrl_prm->wch_burst_length_yn;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXIBSIZ.bit.WSTBMSKYP = axi_ctrl_prm->mask_write_yp;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXIBSIZ.bit.WSTBMSKYN = axi_ctrl_prm->mask_write_yn;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->AXICHSEL.bit.AXIWSEL1 = axi_ctrl_prm->write_select_yp;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->AXICHSEL.bit.AXIWSEL2 = axi_ctrl_prm->write_select_yn;
	Im_LTM_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

/* LTM_MAP Control (each mode common control)
 */
INT32 Im_LTM_MAP_Ctrl_Common( UCHAR pipe_no, const T_IM_LTM_MAP_CTRL_COMMON* const map_ctrl )
{
	UCHAR	loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( map_ctrl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_MAP_Ctrl_Common error. map_ctrl = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Ctrl_Common error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_ltm_map_get_loop_val( pipe_no, &loop_sta, &loop_end );

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MLINTLV.bit.LINTLV     = map_ctrl->line_intr;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXIBSIZ.bit.BURSTWYP  = map_ctrl->wch_burst_length_yp;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXIBSIZ.bit.BURSTWYN  = map_ctrl->wch_burst_length_yn;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXIBSIZ.bit.WSTBMSKYP = map_ctrl->mask_write_yp;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXIBSIZ.bit.WSTBMSKYN = map_ctrl->mask_write_yn;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->AXICHSEL.bit.AXIWSEL1  = map_ctrl->write_select_yp;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->AXICHSEL.bit.AXIWSEL2  = map_ctrl->write_select_yn;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MIOCTL.bit.W0DTYP      = map_ctrl->data_type_yp;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MIOCTL.bit.W1DTYP      = map_ctrl->data_type_yn;

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		// Set Callback pointer
		gIM_LTM_MAP_Mng[loop_cnt].map_user_handler = map_ctrl->map_user_handler;
		gIM_LTM_MAP_Mng[loop_cnt].user_param = map_ctrl->user_param;
	}

	Im_LTM_Off_Pclk( pipe_no );

	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* LTM_MAP Control (SDRAM input mode control)
 */
INT32 Im_LTM_MAP_Ctrl_ModeSDRAMInput( UCHAR pipe_no, const T_IM_LTM_MAP_CTRL_SDRAM_INPUT* const map_ctrl_sdram_input )
{
	UCHAR	loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( map_ctrl_sdram_input == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_MAP_Ctrl_ModeSDRAMInput error. map_ctrl_sdram_input = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Ctrl_ModeSDRAMInput error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_ltm_map_get_loop_val( pipe_no, &loop_sta, &loop_end );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_LTM_MAP_Mng[loop_cnt].input_size = map_ctrl_sdram_input->input_rect;
	}

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MAXIBSIZ.bit.BURSTR = map_ctrl_sdram_input->burst_length;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MSDRDEF.bit.SDRDEF  = map_ctrl_sdram_input->input_global;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MSDRHSZ.bit.SDRHSZ  = map_ctrl_sdram_input->input_rect.img_width;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MSDRVSZ.bit.SDRVSZ  = map_ctrl_sdram_input->input_rect.img_lines;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MIOCTL.bit.INSEL    = D_IM_LTM_MAP_MODE_SDRAM_INPUT;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MIOCTL.bit.R1DTYP   = map_ctrl_sdram_input->data_type;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Set Managed Input Address/Offset information
 */
INT32 Im_LTM_MAP_Set_InData_Info( UCHAR pipe_no, const T_IM_LTM_MAP_INDATA_INFO* const in_data_info )
{
	UINT32 offset = 0;
	UCHAR bit_offset = 0;
	double coef;

#ifdef CO_PARAM_CHECK
	if( in_data_info == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_MAP_Set_InData_Info error. in_data_info = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Set_InData_Info error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	coef = ( gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MIOCTL.bit.R1DTYP == D_IM_LTM_DTYP_PACK12 ) ? ( 3.0 * 3.0 / 2.0 ) : ( 3.0 * 2.0 );

	offset = ( gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MSDRDEF.bit.SDRDEF * gIM_LTM_MAP_Mng[pipe_no].input_size.img_top );
	offset += (UINT32)( gIM_LTM_MAP_Mng[pipe_no].input_size.img_left * coef );

	if(( gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MIOCTL.bit.R1DTYP == D_IM_LTM_DTYP_PACK12 )
	&& ( gIM_LTM_MAP_Mng[pipe_no].input_size.img_left & 0x1 )){
		bit_offset = ( in_data_info->top_bit_offset == D_IM_LTM_ENABLE_ON ) ? 0 : 1;
	}
	else{
		bit_offset = in_data_info->top_bit_offset;
	}

	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MSDRAD.bit.SDRAD   = (UINT32)in_data_info->addr + offset;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MSDROFS.bit.SDROFS = bit_offset;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Set Managed output Address/Global width information
 */
INT32 Im_LTM_MAP_Set_OutData_Info( UCHAR pipe_no, const T_IM_LTM_MAP_OUTDATA_INFO* const out_data_info )
{
#ifdef CO_PARAM_CHECK
	if( out_data_info == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_MAP_Set_OutData_Info error. out_data_info = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Set_OutData_Info error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MSDWADW0.bit.SDWADW0   = (UINT32)out_data_info->addr_yp;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MSDWDEFW0.bit.SDWDEFW0 = out_data_info->global_width_yp;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MSDWADW1.bit.SDWADW1   = (UINT32)out_data_info->addr_yn;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MSDWDEFW1.bit.SDWDEFW1 = out_data_info->global_width_yn;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* LTM_MAP Control (RBK direct connection mode control)
 */
INT32 Im_LTM_MAP_Ctrl_ModeRBKDirect( UCHAR pipe_no )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Ctrl_ModeRBKDirect error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MIOCTL.bit.INSEL  = D_IM_LTM_MAP_MODE_RBK_DIRECT;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

INT32 Im_LTM_MAP_Set_FrameStop( UCHAR pipe_no, UCHAR onoff )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Set_FrameStop error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MFRSTP.bit.FRSTP = onoff;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Set MAP parameter hold
*/
INT32 Im_LTM_MAP_Set_ParamHold( UCHAR pipe_no, const UCHAR hold_enable )
{
#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Set_ParamHold error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MPARMSK.bit.PARMSK = hold_enable;

	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Get MAP busy status.
 */
INT32 Im_LTM_MAP_Get_MapBusy( UCHAR pipe_no, BOOL* const busy_status )
{
	ULONG maptrg;

#ifdef CO_PARAM_CHECK
	if( busy_status == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_MAP_Get_MapBusy error. busy_status = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Get_MapBusy error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	maptrg = gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MTRG.bit.TRG;

	Im_LTM_Off_Pclk( pipe_no );

	if( maptrg == D_IM_LTM_MAP_TRG_BUSY ) {
		*busy_status = TRUE;
	}
	else {
		*busy_status = FALSE;
	}
	return D_DDIM_OK;
}

/* Restart MAP process.
 */
INT32 Im_LTM_MAP_ContStart( UCHAR pipe_no )
{
	UCHAR	loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_ContStart error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	// P-clock control
#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
	Ddim_Print(( "Im_LTM_MAP_ContStart on pclk\n" ));
#endif

	im_ltm_map_get_loop_val( pipe_no, &loop_sta, &loop_end );

	Im_LTM_On_Pclk( pipe_no );

	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MSRAMEN.bit.SRAMAEN = D_IM_LTM_ENABLE_ON;
		gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MSRAMEN.bit.RMAENHST = D_IM_LTM_ENABLE_ON;
		gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MSRAMEN.bit.RMAENYN = D_IM_LTM_ENABLE_ON;
		gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MSRAMEN.bit.RMAENRCH = (gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MIOCTL.bit.INSEL == D_IM_LTM_MAP_MODE_RBK_DIRECT) ? D_IM_LTM_ENABLE_OFF : D_IM_LTM_ENABLE_ON;
	}

	im_ltm_map_wait_usec( D_IM_LTM_MAP_SRAM_WAIT_USEC );

	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MTRG.bit.TRG = D_IM_LTM_MAP_TRG_ON;

#ifdef CO_DEBUG_ON_PC
	if( im_ltm_map_check_target_pipe_no_1(pipe_no) ){
		gIM_Io_Ltm_Map_Reg_Ptr[D_IM_LTM_PIPE1]->MTRG.bit.TRG = D_IM_LTM_MAP_TRG_BUSY;
	}
	if( im_ltm_map_check_target_pipe_no_2(pipe_no) ){
		gIM_Io_Ltm_Map_Reg_Ptr[D_IM_LTM_PIPE2]->MTRG.bit.TRG = D_IM_LTM_MAP_TRG_BUSY;
	}
#endif

#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
	Ddim_Print(( "Im_LTM_MAP_ContStart off pclk\n" ));
#endif
	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Start MAP process.
 */
INT32 Im_LTM_MAP_Start( UCHAR pipe_no )
{
	DDIM_USER_ER		ercd;
	INT32				retcd;
	UCHAR				loop_cnt, loop_sta, loop_end;
	DDIM_USER_FLGPTN	flgptn = 0;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Start error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
	Ddim_Print(( "Im_LTM_MAP_Start begin\n" ));
#endif

	// P-clock control
#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
	Ddim_Print(( "Im_LTM_MAP_Start on pclk\n" ));
#endif

	im_ltm_map_get_loop_val( pipe_no, &loop_sta, &loop_end );

	Im_LTM_On_Pclk( pipe_no );

	while( 1 ) {	// Pclk enable section
		// Clear of All Interrupt enable & Interrupt status
#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
		Ddim_Print(( "Im_LTM_MAP_Start begin initialize\n" ));
#endif

#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
		Ddim_Print(( "Im_LTM_MAP_Start initialize interrupt\n" ));
#endif
		// Initialized Interrupt
		gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MINT1.word = D_IM_LTM_MAP_MINT1_ALL_CLR;

		for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
			gIM_LTM_MAP_Mng[loop_cnt].int_status = 0;
		}

#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
		Ddim_Print(( "Im_LTM_MAP_Start enable interrupt\n" ));
#endif
		gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MINT0.word = D_IM_LTM_MAP_MINT0_ENABLE;

#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
		Ddim_Print(( "Im_LTM_MAP_Start on clk\n" ));
#endif
		Im_LTM_On_Hclk( pipe_no );
		Im_LTM_On_Aclk( pipe_no );
		Im_LTM_On_RBKClk( pipe_no );

		// Set access enable

		for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
			gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MSRAMEN.bit.SRAMAEN = D_IM_LTM_ENABLE_ON;
			gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MSRAMEN.bit.RMAENHST = D_IM_LTM_ENABLE_ON;
			gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MSRAMEN.bit.RMAENYN = D_IM_LTM_ENABLE_ON;
			gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MSRAMEN.bit.RMAENRCH = (gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MIOCTL.bit.INSEL == D_IM_LTM_MAP_MODE_RBK_DIRECT) ? D_IM_LTM_ENABLE_OFF : D_IM_LTM_ENABLE_ON;
		}

		im_ltm_map_wait_usec( D_IM_LTM_MAP_SRAM_WAIT_USEC );

		for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
			gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MHCPRD.bit.HCPRD = D_IM_LTM_ENABLE_ON;
			gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MHSTINI.bit.HSTINI = D_IM_LTM_ENABLE_ON;
			while( gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MHSTINI.bit.HSTINI == D_IM_LTM_ENABLE_ON
				&& gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MINT1.bit.__HINIFL == 0 ) {
				im_ltm_map_wait_usec( D_IM_LTM_MAP_SRAM_WAIT_USEC );
			}
			gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MHCPRD.bit.HCPRD = D_IM_LTM_ENABLE_OFF;
		}

#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
		Ddim_Print(( "Im_LTM_MAP_Start clear event flag\n" ));
#endif

		// Clear of Event Flag
		if( im_ltm_map_check_target_pipe_no_1( pipe_no ) ){
			flgptn |= D_IM_LTM_MAP1_INT_FLG_ALL;
		}
		if( im_ltm_map_check_target_pipe_no_2( pipe_no ) ){
			flgptn |= D_IM_LTM_MAP2_INT_FLG_ALL;
		}

		ercd = DDIM_User_Clr_Flg( FID_IM_LTM_MAP, ~flgptn );
		if( ercd != D_DDIM_USER_E_OK ) {
			Ddim_Print(( "Im_LTM_MAP_Start: invalid FID\n" ));
			retcd = D_IM_LTM_SYSTEM_ERROR;
			break;
		}

		gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MTRG.bit.TRG = D_IM_LTM_MAP_TRG_ON;
#ifdef CO_DEBUG_ON_PC
		for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
			gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MTRG.bit.TRG = D_IM_LTM_MAP_TRG_BUSY;
		}
#endif
		retcd = D_DDIM_OK;
		break;
	}

#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
	Ddim_Print(( "Im_LTM_MAP_Start off pclk\n" ));
#endif
	Im_LTM_Off_Pclk( pipe_no );

#ifdef D_IM_LTM_MAP_DEBUG_ON_PC
	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MINT1.word = 0;
		gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MINT1.bit.__FINFL = 1;
		if( gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MLINTLV.bit.LINTLV != 0 ){
			gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MINT1.bit.__LCNTFL = 1;
		}
		Im_LTM_MAP_Int_Handler( loop_cnt );
		gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MINT1.word = 0;
	}
#endif

	Im_LTM_Dsb();

#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
	Ddim_Print(( "Im_LTM_MAP_Start end\n" ));
#endif

	return retcd;
}

/* Stop LTM MAP process
 */
INT32 Im_LTM_MAP_Stop( UCHAR pipe_no )
{
	UCHAR loop_cnt, loop_sta, loop_end;

#ifdef CO_PARAM_CHECK
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Stop error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	im_ltm_map_get_loop_val( pipe_no, &loop_sta, &loop_end );

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MTRG.bit.TRG = D_IM_LTM_MAP_TRG_OFF;

	Im_LTM_Dsb();
	for( loop_cnt = loop_sta; loop_cnt <= loop_end; loop_cnt++ ){
		while( gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MTRG.bit.TRG != D_IM_LTM_MAP_TRG_IDLE ) {
#ifdef CO_DEBUG_ON_PC
//			Ddim_Print(( "Im_LTM_MAP_Stop wait loop. %u\n", gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MTRG.bit.TRG ));
			gIM_Io_Ltm_Map_Reg_Ptr[loop_cnt]->MTRG.bit.TRG = D_IM_LTM_MAP_TRG_IDLE;
#endif
			;	// DO NOTHING
		}
	}

	// Clear of All Interrupt disable
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MINT0.word = D_IM_LTM_MAP_MINT0_DISABLE;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MINT1.word = D_IM_LTM_MAP_MINT1_ALL_CLR;

	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MSRAMEN.word = D_IM_LTM_MAP_MSRAMEN_DISABLE;

	Im_LTM_Off_RBKClk( pipe_no );
	Im_LTM_Off_Aclk( pipe_no );
	Im_LTM_Off_Hclk( pipe_no );
	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

	return D_DDIM_OK;
}

/* Wait end of LTM MAP macro.
 */
INT32 Im_LTM_MAP_WaitEnd( DDIM_USER_FLGPTN* const p_flgptn, DDIM_USER_FLGPTN waiptn, DDIM_USER_TMO tmout )
{
	DDIM_USER_FLGPTN	flgptn;
	DDIM_USER_ER		ercd;

#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
	Ddim_Print(( "Im_LTM_MAP_WaitEnd begin\n" ));
#endif

#ifdef CO_PARAM_CHECK
	if( (waiptn & D_IM_LTM_MAP_INT_FLG_ALL) != waiptn ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_MAP_WaitEnd error. waiptn\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif
	Im_LTM_Dsb();

	switch( DDIM_User_Twai_Flg( FID_IM_LTM_MAP, waiptn, D_DDIM_USER_TWF_ORW, &flgptn, tmout ) ) {
		case D_DDIM_USER_E_OK:
#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
		Ddim_Print(( "Im_LTM_MAP_WaitEnd twai_flg OK return\n" ));
#endif
			flgptn = waiptn & flgptn;
			ercd = DDIM_User_Clr_Flg( FID_IM_LTM_MAP, ~flgptn );
			if( ercd != D_DDIM_USER_E_OK ) {
				Ddim_Print(( "Im_LTM_MAP_WaitEnd: invalid FID.\n" ));
				return D_IM_LTM_SYSTEM_ERROR;
			}
			if( p_flgptn != NULL ) {
				*p_flgptn = flgptn;
			}
			return D_DDIM_OK;
		case D_DDIM_USER_E_TMOUT:
#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
		Ddim_Print(( "Im_LTM_MAP_WaitEnd twai_flg TimeOut return\n" ));
#endif
			return D_IM_LTM_TIMEOUT;
		default:
#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
		Ddim_Print(( "Im_LTM_MAP_WaitEnd twai_flg NG return\n" ));
#endif
			return D_IM_LTM_PARAM_ERROR;
	}
}

/* LTM_MAP Interrupt Handler
 */
VOID Im_LTM_MAP_Int_Handler( UCHAR pipe_no )
{
	UINT32			loopcnt;
	DDIM_USER_ER	ercd;
	ULONG			mint0;
	ULONG			mint1;
	ULONG			mint1_clr;

#ifdef CO_PARAM_CHECK
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Int_Handler error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return;
	}
#endif	// CO_PARAM_CHECK

#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
	Ddim_Print(( "Im_LTM_MAP_Int_Handler begin\n" ));
#endif

#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
	Ddim_Print(( "Im_LTM_MAP_Int_Handler on pclk\n" ));
#endif
	Im_LTM_On_Pclk( pipe_no );

	mint0 = gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MINT0.word;
	mint1 = gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MINT1.word;
	mint1_clr = 0;

#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
	Ddim_Print(( "Im_LTM_MAP_Int_Handler mint0=0x%lx, mint1=0x%lx\n", mint0, mint1 ));
#endif

	// Call of Callback API for interrupt status

	for( loopcnt = 0; loopcnt < (sizeof(gim_ltm_map_inttbl[0]) / sizeof(gim_ltm_map_inttbl[0][0])); loopcnt++ ) {
		if( ((mint1 & gim_ltm_map_inttbl[pipe_no][loopcnt].flg_bitmask) != 0)
		 && ((mint0 & gim_ltm_map_inttbl[pipe_no][loopcnt].ena_bitmask) != 0) ) {

#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
			Ddim_Print(( "Im_LTM_MAP_Int_Handler begin 0x%x\n", gim_ltm_map_inttbl[pipe_no][loopcnt].flg_bitmask ));
#endif

			gIM_LTM_MAP_Mng[pipe_no].int_status = gim_ltm_map_inttbl[pipe_no][loopcnt].int_status;

			mint1_clr |= gim_ltm_map_inttbl[pipe_no][loopcnt].flg_bitmask;

			ercd = DDIM_User_Set_Flg( FID_IM_LTM_MAP, gim_ltm_map_inttbl[pipe_no][loopcnt].waitflg );
			if( ercd != D_DDIM_USER_E_OK ) {
				Ddim_Print(( "Im_LTM_MAP_Int_Handler: invalid FID. 0x%lx\n", gim_ltm_map_inttbl[pipe_no][loopcnt].waitflg ));
			}

			if (gIM_LTM_MAP_Mng[pipe_no].map_user_handler) {
				gIM_LTM_MAP_Mng[pipe_no].map_user_handler( (UINT32*)&gIM_LTM_MAP_Mng[pipe_no].int_status, gIM_LTM_MAP_Mng[pipe_no].user_param );
			}
		}
	}

	// Clear interrupt
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MINT1.word = mint1_clr;


#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
	Ddim_Print(( "Im_LTM_MAP_Int_Handler mint1=0x%lx\n", gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MINT1.word ));
#endif

#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
	Ddim_Print(( "Im_LTM_MAP_Int_Handler off pclk\n" ));
#endif
	Im_LTM_Off_Pclk( pipe_no );
	Im_LTM_Dsb();

#ifdef CO_DEBUG_PRINT_IM_LTM_MAP
	Ddim_Print(( "Im_LTM_MAP_Int_Handler end\n" ));
#endif
}

/* WB Gain control
 */
INT32 Im_LTM_MAP_Set_WB_Gain( UCHAR pipe_no, const T_IM_LTM_MAP_GAIN* const wb_gain )
{
#ifdef CO_PARAM_CHECK
	if( wb_gain == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_MAP_Set_WB_Gain error. wb_gain = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Set_WB_Gain error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->GAINR.bit.GAINR = wb_gain->gain_r;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->GAING.bit.GAING = wb_gain->gain_g;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->GAINB.bit.GAINB = wb_gain->gain_b;

	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->GAINCPR.bit.GAINCPR = wb_gain->clip_r;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->GAINCPG.bit.GAINCPG = wb_gain->clip_g;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->GAINCPB.bit.GAINCPB = wb_gain->clip_b;

	Im_LTM_Off_Pclk( pipe_no );
	return D_DDIM_OK;
}

/* Map image Generation mode control
 */
INT32 Im_LTM_MAP_Ctrl_Generation_Mode( UCHAR pipe_no, const T_IM_LTM_MAP_CTRL_GENERATION_MODE* const mode )
{
#ifdef CO_PARAM_CHECK
	if( mode == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_MAP_Ctrl_Generation_Mode error. mode = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Ctrl_Generation_Mode error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->PEPS0M.bit.PEPS0M     = mode->mode0;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->PEPS0OF.bit.PEPS0OF_3 = mode->coef0;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->PEPS1M.bit.PEPS1M     = mode->mode1;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->PEPS1OF.bit.PEPS1OF_3 = mode->coef1;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->PEPS2M.bit.PEPS2M     = mode->mode2;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->PEPS2OF.bit.PEPS2OF_3 = mode->coef2;

	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->NEPS0M.bit.NEPS0M     = mode->mode0;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->NEPS0OF.bit.NEPS0OF_3 = mode->coef0;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->NEPS1M.bit.NEPS1M     = mode->mode1;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->NEPS1OF.bit.NEPS1OF_3 = mode->coef1;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->NEPS2M.bit.NEPS2M     = mode->mode2;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->NEPS2OF.bit.NEPS2OF_3 = mode->coef2;

	Im_LTM_Off_Pclk( pipe_no );
	return D_DDIM_OK;
}

/* Get internal parameter
 */
INT32 Im_LTM_MAP_Get_Internal_Param( UCHAR pipe_no, T_IM_LTM_INTERNAL_PARAM* const param )
{
#ifdef CO_PARAM_CHECK
	if( param == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_MAP_Get_Internal_Param error. param = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Get_Internal_Param error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	param->yshadow    = gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->YSHADOW.bit.YSHADOW;
	param->yhighlight = gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->YHIGHLIGHT.bit.YHIGHLIGHT;

	Im_LTM_Off_Pclk( pipe_no );
	return D_DDIM_OK;
}

/* YLOG table control
 */
INT32 Im_LTM_MAP_Ctrl_Ylog_Tbl( UCHAR pipe_no, const T_IM_LTM_CTRL_YLOG* const ylog_ctrl )
{
#ifdef CO_PARAM_CHECK
	if( ylog_ctrl == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_MAP_Ctrl_Ylog_Tbl error. ylog_ctrl = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Ctrl_Ylog_Tbl error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MYLCTL.bit.YLOGSL      = ylog_ctrl->use_tbl;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MYLOG0EP0.bit.YLOG0EP0 = ylog_ctrl->end_point0_tbl0;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MYLOG0EP1.bit.YLOG0EP1 = ylog_ctrl->end_point1_tbl0;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MYLOG1EP0.bit.YLOG1EP0 = ylog_ctrl->end_point0_tbl1;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MYLOG1EP1.bit.YLOG1EP1 = ylog_ctrl->end_point1_tbl1;

	Im_LTM_Off_Pclk( pipe_no );
	return D_DDIM_OK;
}

/* Get active YLOG table No.
 */
INT32 Im_LTM_MAP_Get_Active_Ylog_Tbl_No( UCHAR pipe_no, UCHAR* const tbl_no )
{
#ifdef CO_PARAM_CHECK
	if( tbl_no == NULL ) {
		// Parameter setting error
		Ddim_Assertion(("Im_LTM_MAP_Get_Active_Ylog_Tbl_No error. tbl_no = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no >= D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Get_Active_Ylog_Tbl_No error. pipe_no>=D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	Im_LTM_On_Pclk( pipe_no );

	*tbl_no = gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MYLCTL.bit.YLOGSTA;

	Im_LTM_Off_Pclk( pipe_no );
	return D_DDIM_OK;
}

/* Set YLOG table.
 */
INT32 Im_LTM_MAP_Set_YLOG_Table( UCHAR pipe_no, UCHAR tbl_sel, const USHORT* const src_tbl )
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
		Ddim_Assertion(("Im_LTM_MAP_Set_YLOG_Table error. src_tbl = NULL\n"));
		return D_IM_LTM_PARAM_ERROR;
	}
	if( pipe_no > D_IM_LTM_PIPE12 ){
		Ddim_Assertion(( "Im_LTM_MAP_Set_YLOG_Table error. pipe_no>D_IM_LTM_PIPE12\n" ));
		return D_IM_LTM_PARAM_ERROR;
	}
#endif

	Im_LTM_On_Pclk( pipe_no );

	sram_en = gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MSRAMEN.word;
	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MSRAMEN.word = D_IM_LTM_MAP_MSRAMEN_ENABLE;

#ifdef CO_LTM_RDMA_ON
	im_ltm_map_set_rdma_val_ylog_table( pipe_no, src_tbl );
#else	// CO_LTM_RDMA_ON
	Im_LTM_On_Hclk( pipe_no );

	switch( tbl_sel ){
		case D_LTM_YLOG_TBL_00:
			dst_tbl = gIM_Io_Ltm_Map_Tbl_Ptr[pipe_no]->MYLOG0TBL0.hword;
			break;
		case D_LTM_YLOG_TBL_10:
			dst_tbl = gIM_Io_Ltm_Map_Tbl_Ptr[pipe_no]->MYLOG1TBL0.hword;
			break;
		case D_LTM_YLOG_TBL_01:
			dst_tbl = gIM_Io_Ltm_Map_Tbl_Ptr[pipe_no]->MYLOG0TBL1.hword;
			break;
//		case D_LTM_YLOG_TBL_11:
		default:
			dst_tbl = gIM_Io_Ltm_Map_Tbl_Ptr[pipe_no]->MYLOG1TBL1.hword;
			break;
	}

	for( loop_cnt = 0; loop_cnt < D_IM_LTM_TABLE_MAX_YLOG; loop_cnt++ ) {
		dst_tbl[loop_cnt] = src_tbl[loop_cnt];
	}

	Im_LTM_Off_Hclk( pipe_no );
#endif	// CO_LTM_RDMA_ON

	gIM_Io_Ltm_Map_Reg_Ptr[pipe_no]->MSRAMEN.word = sram_en;

	Im_LTM_Off_Pclk( pipe_no );

	return D_DDIM_OK;
}

#ifdef IM_LTM_MAP_STATUS_PRINT
VOID Im_LTM_MAP_Print_Status( VOID )
{
	UINT32 loopcnt;

	for( loopcnt = 0; loopcnt < 2; loopcnt++ ) {
		Ddim_Print(( "PIPE%u:\n", loopcnt ));
		Ddim_Print(( "\tinput_size\n" ));
		Ddim_Print(( "\t\timg_left            = 0x%x\n", gIM_LTM_MAP_Mng[loopcnt].input_size.img_left ));
		Ddim_Print(( "\t\timg_top             = 0x%x\n", gIM_LTM_MAP_Mng[loopcnt].input_size.img_top ));
		Ddim_Print(( "\t\timg_width           = 0x%x\n", gIM_LTM_MAP_Mng[loopcnt].input_size.img_width ));
		Ddim_Print(( "\t\timg_lines           = 0x%x\n", gIM_LTM_MAP_Mng[loopcnt].input_size.img_lines ));
		Ddim_Print(( "\tint_status            = 0x%x\n", gIM_LTM_MAP_Mng[loopcnt].int_status ));
		Ddim_Print(( "\tmap_user_handler      = 0x%x\n", (UINT32)gIM_LTM_MAP_Mng[loopcnt].map_user_handler ));
		Ddim_Print(( "\tuser_param            = 0x%x\n", (UINT32)gIM_LTM_MAP_Mng[loopcnt].user_param ));
	}
}

VOID Im_LTM_MAP_Print_AccEnStatus( VOID )
{
	UINT32 loopcnt;

	Im_LTM_On_Pclk( D_IM_LTM_PIPE12 );
	for( loopcnt = 0; loopcnt < 2; loopcnt++ ) {
		Ddim_Print(( "SRAMAEN  = 0x%x\n", gIM_Io_Ltm_Map_Reg_Ptr[loopcnt]->MSRAMEN.bit.SRAMAEN ));
		Ddim_Print(( "RMAENYN  = 0x%x\n", gIM_Io_Ltm_Map_Reg_Ptr[loopcnt]->MSRAMEN.bit.RMAENYN ));
		Ddim_Print(( "RMAENRCH = 0x%x\n", gIM_Io_Ltm_Map_Reg_Ptr[loopcnt]->MSRAMEN.bit.RMAENRCH ));
	}
	Im_LTM_Off_Pclk( D_IM_LTM_PIPE12 );
}
#endif

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
#endif	// CO_DDIM_UTILITY_USE
