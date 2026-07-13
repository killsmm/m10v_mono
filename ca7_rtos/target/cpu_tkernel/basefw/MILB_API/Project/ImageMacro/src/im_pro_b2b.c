/**
 * @file		im_pro_b2b.c
 * @brief		SRO unit controler of PRO macro.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "jdspro.h"
#include "im_pro.h"
#include "im_pro_common.h"
#include "dd_top.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define	D_IM_B2B1_STATUS_NONE					(0x00000000UL)				/* All stopped		*/
#define	D_IM_B2B1_STATUS_ELF					((0x00000000UL)&(1<< 0))	/* ELF				*/
#define	D_IM_B2B1_STATUS_ELF_NSL				((0x00000000UL)&(1<< 1))	/* ELF NSL			*/
#define	D_IM_B2B1_STATUS_FSHD					((0x00000000UL)&(1<< 2))	/* FSHD				*/

#define	D_IM_B2B2_STATUS_NONE					(0x00000000UL)				/* All stopped		*/
#define	D_IM_B2B2_STATUS_ELF					((0x10000000UL)&(1<< 0))	/* ELF				*/
#define	D_IM_B2B2_STATUS_ELF_NSL				((0x10000000UL)&(1<< 1))	/* ELF NSL			*/
#define	D_IM_B2B2_STATUS_FSHD					((0x10000000UL)&(1<< 2))	/* FSHD				*/

#define	D_IM_B2B_STATUS_ELF_BOTH				( D_IM_B2B1_STATUS_ELF		| D_IM_B2B2_STATUS_ELF		)	/* Image Pipe1&2 ELF		*/
#define	D_IM_B2B_STATUS_ELF_NSL_BOTH			( D_IM_B2B1_STATUS_ELF_NSL	| D_IM_B2B2_STATUS_ELF_NSL	)	/* Image Pipe1&2 ELF NSL	*/
#define	D_IM_B2B_STATUS_FSHD_BOTH				( D_IM_B2B1_STATUS_FSHD		| D_IM_B2B2_STATUS_FSHD		)	/* Image Pipe1&2 FSHD		*/

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// PRO Channel/Macro start/stop status
#define im_pro_b2b_set_start_status(st, ch) 	(gIM_B2B_START_Status[((st) >> 28) & 0xf] |=  (((st) & 0x0fffffff) << (ch)))							/* B2B clock status ON	*/
#define im_pro_b2b_set_stop_status(st, ch)		(gIM_B2B_START_Status[((st) >> 28) & 0xf] &= ~(((st) & 0x0fffffff) << (ch)))							/* B2B clock status OFF	*/
#define im_pro_b2b_get_start_status(st, ch)		(((gIM_B2B_START_Status[((st) >> 28) & 0xf] & (((st) & 0x0fffffff) << (ch))) != 0) ? TRUE : FALSE)		/* started check	*/

#define im_pro_b2b_get_stop_all()				(((gIM_B2B_START_Status[0] | gIM_B2B_START_Status[1] | gIM_B2B_START_Status[2]) != 0) ? FALSE : TRUE)	/* started check	*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static	volatile	ULONG	gIM_B2B_START_Status[1] = {0x00000000};

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
static volatile T_IM_PRO_B2BTOP_INFO	gIM_PRO_B2BTOP_Reg_Ptr_Tbl[E_IM_PRO_UNIT_NUM_MAX] = {
	// E_IM_PRO_UNIT_NUM_1
	{
		&IO_PRO.IMG_PIPE[0].B2B.B2BTOP
	},
	// E_IM_PRO_UNIT_NUM_2
	{
		&IO_PRO.IMG_PIPE[1].B2B.B2BTOP
	},
	// E_IM_PRO_BOTH_UNIT
	{
		&IO_PRO.IMG_PIPE[2].B2B.B2BTOP
	},
};

static const UINT32	gIM_PRO_ELF_Status_Tbl[E_IM_PRO_UNIT_NUM_MAX] = {
	D_IM_B2B1_STATUS_ELF,	D_IM_B2B2_STATUS_ELF,	D_IM_B2B_STATUS_ELF_BOTH,
};

static const UINT32	gIM_PRO_ELF_NSL_Status_Tbl[E_IM_PRO_UNIT_NUM_MAX] = {
	D_IM_B2B1_STATUS_ELF_NSL,	D_IM_B2B2_STATUS_ELF_NSL,	D_IM_B2B_STATUS_ELF_NSL_BOTH,
};

static const T_IM_PRO_FSHD_INFO	gIM_PRO_FSHD_Status_Tbl[E_IM_PRO_UNIT_NUM_MAX] = {
	{	&IO_PRO_TBL.B2B1_TBL.FSHDCTBL[0],	D_IM_B2B1_STATUS_FSHD,		{	D_IM_PRO_COMMON_STATUS_B2B1_M2P2,	D_IM_PRO_COMMON_STATUS_B2B1_M2P3,	D_IM_PRO_COMMON_STATUS_B2B1_PRCH2,	D_IM_PRO_COMMON_STATUS_B2B1_PRCH3,
																			D_IM_PRO_COMMON_STATUS_NONE,		D_IM_PRO_COMMON_STATUS_NONE,		D_IM_PRO_COMMON_STATUS_NONE,		D_IM_PRO_COMMON_STATUS_NONE			}	},
	{	&IO_PRO_TBL.B2B2_TBL.FSHDCTBL[0],	D_IM_B2B2_STATUS_FSHD,		{	D_IM_PRO_COMMON_STATUS_B2B2_M2P2,	D_IM_PRO_COMMON_STATUS_B2B2_M2P3,	D_IM_PRO_COMMON_STATUS_B2B2_PRCH2,	D_IM_PRO_COMMON_STATUS_B2B2_PRCH3,
																			D_IM_PRO_COMMON_STATUS_NONE,		D_IM_PRO_COMMON_STATUS_NONE,		D_IM_PRO_COMMON_STATUS_NONE,		D_IM_PRO_COMMON_STATUS_NONE			}	},
	{	&IO_PRO_TBL.B2B3_TBL.FSHDCTBL[0],	D_IM_B2B_STATUS_FSHD_BOTH,	{	D_IM_PRO_COMMON_STATUS_B2B1_M2P2,	D_IM_PRO_COMMON_STATUS_B2B1_M2P3,	D_IM_PRO_COMMON_STATUS_B2B1_PRCH2,	D_IM_PRO_COMMON_STATUS_B2B1_PRCH3,
																			D_IM_PRO_COMMON_STATUS_B2B2_M2P2,	D_IM_PRO_COMMON_STATUS_B2B2_M2P3,	D_IM_PRO_COMMON_STATUS_B2B2_PRCH2,	D_IM_PRO_COMMON_STATUS_B2B2_PRCH3	}	},
};

static const T_IM_PRO_RDMA_ELF_ADDR gIM_PRO_ELF_Addr[E_IM_PRO_UNIT_NUM_MAX] = {
	{
		0x28410420,0x28410424,0x28410428,0x2841042C,
		0x28410430,0x28410434,0x28410438,0x2841043C,
		0x28410440,0x28410444,0x28410448,0x2841044C,
		0x28410450,0x28410454,0x28410458,0x2841045C,
		0x28410460,0x28410464,0x28410468,0x2841046C,
		0x28410470,0x28410474,0x28410478,0x2841047C,
		0x28410480,0x28410484,0x28410488,0x2841048C,
		0x28410490,0x28410494,0x28410498,0x2841049C,
		0x284104A0,0x284104A4,0x284104A8,0x284104AC,
		0x284104B0,0x284104B8,0x284104BC,0x284104C0,
		0x284104C4,0x284104C8,0x284104CC,0x284104D0,
		0x284104D4,0x284104D8,0x284104DC,0x284104E0,
		0x284104E4,0x284104E8,0x284104F0,0x284104F4,
		0x284104F8,0x284104FC,0x28410500,0x28410504,
		0x28410508,0x2841050C,0x28410510,0x28410514,
		0x28410518,0x2841051C,0x28410520,0x28410528,
		0x2841052C,0x28410540,0x28410544,0x28410548,
		0x2841054C,0x28410550,0x28410560,0x28410564,
		0x28410568,0x2841056C,0x28410570,0x28410574,
		0x28410578,0x2841057C,0x28410580,0x28410584,
		0x28410588,0x2841058C,0x28410590,0x28410594,
		0x28410598,0x2841059C,0x284105A0,0x284105A4,
		0x284105A8,0x284105AC,0x284105B0,0x284105B4,
		0x284105B8,0x284105BC,0x284105C0,0x284105C8,
		0x284105CC,0x284105D0,0x284105D4,0x284105D8,
		0x284105DC,0x284105E0,0x284105E4,0x284105E8,
		0x284105EC,0x284105F0,0x284105F4,0x284105F8,
		0x284105FC,0x28410600,0x28410604,0x28410608,
		0x2841060C,0x28410610,0x28410614,0x28410618,
		0x2841061C,0x28410620,0x28410624,0x28410628,
		0x2841062C,0x28410630,0x28410634,0x28410640,
		0x28410644,0x28410648,0x2841064C,0x28410650,
		0x28410654,0x28410660,0x28410664,0x28410668,
		0x2841066C,0x28410684,
	},
	{
		0x28510420,0x28510424,0x28510428,0x2851042C,
		0x28510430,0x28510434,0x28510438,0x2851043C,
		0x28510440,0x28510444,0x28510448,0x2851044C,
		0x28510450,0x28510454,0x28510458,0x2851045C,
		0x28510460,0x28510464,0x28510468,0x2851046C,
		0x28510470,0x28510474,0x28510478,0x2851047C,
		0x28510480,0x28510484,0x28510488,0x2851048C,
		0x28510490,0x28510494,0x28510498,0x2851049C,
		0x285104A0,0x285104A4,0x285104A8,0x285104AC,
		0x285104B0,0x285104B8,0x285104BC,0x285104C0,
		0x285104C4,0x285104C8,0x285104CC,0x285104D0,
		0x285104D4,0x285104D8,0x285104DC,0x285104E0,
		0x285104E4,0x285104E8,0x285104F0,0x285104F4,
		0x285104F8,0x285104FC,0x28510500,0x28510504,
		0x28510508,0x2851050C,0x28510510,0x28510514,
		0x28510518,0x2851051C,0x28510520,0x28510528,
		0x2851052C,0x28510540,0x28510544,0x28510548,
		0x2851054C,0x28510550,0x28510560,0x28510564,
		0x28510568,0x2851056C,0x28510570,0x28510574,
		0x28510578,0x2851057C,0x28510580,0x28510584,
		0x28510588,0x2851058C,0x28510590,0x28510594,
		0x28510598,0x2851059C,0x285105A0,0x285105A4,
		0x285105A8,0x285105AC,0x285105B0,0x285105B4,
		0x285105B8,0x285105BC,0x285105C0,0x285105C8,
		0x285105CC,0x285105D0,0x285105D4,0x285105D8,
		0x285105DC,0x285105E0,0x285105E4,0x285105E8,
		0x285105EC,0x285105F0,0x285105F4,0x285105F8,
		0x285105FC,0x28510600,0x28510604,0x28510608,
		0x2851060C,0x28510610,0x28510614,0x28510618,
		0x2851061C,0x28510620,0x28510624,0x28510628,
		0x2851062C,0x28510630,0x28510634,0x28510640,
		0x28510644,0x28510648,0x2851064C,0x28510650,
		0x28510654,0x28510660,0x28510664,0x28510668,
		0x2851066C,0x28510684,
	},
	{
		0x28610420,0x28610424,0x28610428,0x2861042C,
		0x28610430,0x28610434,0x28610438,0x2861043C,
		0x28610440,0x28610444,0x28610448,0x2861044C,
		0x28610450,0x28610454,0x28610458,0x2861045C,
		0x28610460,0x28610464,0x28610468,0x2861046C,
		0x28610470,0x28610474,0x28610478,0x2861047C,
		0x28610480,0x28610484,0x28610488,0x2861048C,
		0x28610490,0x28610494,0x28610498,0x2861049C,
		0x286104A0,0x286104A4,0x286104A8,0x286104AC,
		0x286104B0,0x286104B8,0x286104BC,0x286104C0,
		0x286104C4,0x286104C8,0x286104CC,0x286104D0,
		0x286104D4,0x286104D8,0x286104DC,0x286104E0,
		0x286104E4,0x286104E8,0x286104F0,0x286104F4,
		0x286104F8,0x286104FC,0x28610500,0x28610504,
		0x28610508,0x2861050C,0x28610510,0x28610514,
		0x28610518,0x2861051C,0x28610520,0x28610528,
		0x2861052C,0x28610540,0x28610544,0x28610548,
		0x2861054C,0x28610550,0x28610560,0x28610564,
		0x28610568,0x2861056C,0x28610570,0x28610574,
		0x28610578,0x2861057C,0x28610580,0x28610584,
		0x28610588,0x2861058C,0x28610590,0x28610594,
		0x28610598,0x2861059C,0x286105A0,0x286105A4,
		0x286105A8,0x286105AC,0x286105B0,0x286105B4,
		0x286105B8,0x286105BC,0x286105C0,0x286105C8,
		0x286105CC,0x286105D0,0x286105D4,0x286105D8,
		0x286105DC,0x286105E0,0x286105E4,0x286105E8,
		0x286105EC,0x286105F0,0x286105F4,0x286105F8,
		0x286105FC,0x28610600,0x28610604,0x28610608,
		0x2861060C,0x28610610,0x28610614,0x28610618,
		0x2861061C,0x28610620,0x28610624,0x28610628,
		0x2861062C,0x28610630,0x28610634,0x28610640,
		0x28610644,0x28610648,0x2861064C,0x28610650,
		0x28610654,0x28610660,0x28610664,0x28610668,
		0x2861066C,0x28610684,
	},
};

static const T_IM_PRO_RDMA_FSHD_ADDR gIM_PRO_FSHD_Addr[E_IM_PRO_UNIT_NUM_MAX] = {
	{
		0x28410A08,0x28410A1C,0x28410A20,0x28410A24,
		0x28410A28,0x28410A30,0x28410A34,0x28410A38,
		0x28410A3C,0x28410A40,0x28410A44,0x28410A48,
		0x28410A50,0x28410A54,0x28410A58,0x28410A5C,
		0x28410A60,0x28410A64,0x28410A68,0x28410A6C,
		0x28410A70,0x28410A80,0x28410A84,0x28410A88,
	},
	{
		0x28510A08,0x28510A1C,0x28510A20,0x28510A24,
		0x28510A28,0x28510A30,0x28510A34,0x28510A38,
		0x28510A3C,0x28510A40,0x28510A44,0x28510A48,
		0x28510A50,0x28510A54,0x28510A58,0x28510A5C,
		0x28510A60,0x28510A64,0x28510A68,0x28510A6C,
		0x28510A70,0x28510A80,0x28510A84,0x28510A88,
	},
	{
		0x28610A08,0x28610A1C,0x28610A20,0x28610A24,
		0x28610A28,0x28610A30,0x28610A34,0x28610A38,
		0x28610A3C,0x28610A40,0x28610A44,0x28610A48,
		0x28610A50,0x28610A54,0x28610A58,0x28610A5C,
		0x28610A60,0x28610A64,0x28610A68,0x28610A6C,
		0x28610A70,0x28610A80,0x28610A84,0x28610A88,
	},
};

static const T_IM_PRO_RDMA_FSHD_CTBL_ADDR gIM_PRO_FSHD_Ctbl_Addr[E_IM_PRO_UNIT_NUM_MAX][D_IM_PRO_B2B_FSHD_CTBL_CNT] = {
	{
		{ 0x29480000 },
		{ 0x29480800 },
	},
	{
		{ 0x29580000 },
		{ 0x29580800 },
	},
	{
		{ 0x29680000 },
		{ 0x29680800 },
	},
};

VOID im_pro_comm_get_fshd_reg_info( E_IM_PRO_UNIT_NUM unit_no, const T_IM_PRO_FSHD_INFO** fshdl_info )
{
	*fshdl_info = &gIM_PRO_FSHD_Status_Tbl[unit_no];
}

/********************************************************/
/*	B2B Top												*/
/********************************************************/

/**
B2BTOP Macro initialize
@param[in]	unit_no : Unit number.
@remarks	Please Call at the time of system starting.
*/
VOID Im_PRO_B2BTOP_Init( E_IM_PRO_UNIT_NUM unit_no )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	// Software release
	gIM_PRO_B2BTOP_Reg_Ptr_Tbl[unit_no].reg_ptr->SR.bit.SR = D_IM_PRO_SR_RELEASE;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock
}

/**
B2BTOP Macro software reset
@param[in]	unit_no : Unit number.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG		: All macro not stopped NG
*/
INT32 Im_PRO_B2BTOP_SW_Reset( E_IM_PRO_UNIT_NUM unit_no )
{
#ifdef CO_ACT_PRO_CLOCK
	if (im_pro_b2b_get_stop_all() == FALSE){	//not ALL Stopped
		Ddim_Print(("I:Im_PRO_B2BTOP_SW_Reset. macro not stopped error. \n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif	/* CO_ACT_PRO_CLOCK */

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	// Software reset
	gIM_PRO_B2BTOP_Reg_Ptr_Tbl[unit_no].reg_ptr->SR.bit.SR = D_IM_PRO_SR_RESET;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
B2BTOP Macro clock control
@param[in]	unit_no : Unit number.
@param[in]	clk_type	: B2BTOP Macro clock type
@param[in]	on_off		: 0:clock on 1:clock off
@param[in]	wait_skip	: 0:non wait 1:wait 1ms. for wait PROCLK/CDK 5 cycle.
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_B2BTOP_Control_Clock( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_B2BTOP_CLK_TYPE clk_type, UCHAR on_off, UCHAR wait_skip )
{
	UINT32 cpu_clk_hz;
	UINT32 macro_clk_hz;
#ifdef CO_PARAM_CHECK
	if (on_off > D_IM_PRO_CLOCK_OFF){
		Ddim_Assertion(("I:Im_PRO_B2BTOP_Control_Clock error. on_off value over!! (%d)\n", on_off));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else {
		// DO NOTHING
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	switch( clk_type ) {
		case E_IM_PRO_B2BTOP_CLK_TYPE_B2BCLK:
			gIM_PRO_B2BTOP_Reg_Ptr_Tbl[unit_no].reg_ptr->CLKSTP.bit.PSTP1 = ( ( on_off == D_IM_PRO_CLOCK_ON ) ? D_IM_PRO_CLOCK_ON : D_IM_PRO_CLOCK_OFF );
			break;

		default:
			break;
	}
	// waiting for 5 cycle
	if( wait_skip == 0 ) {
		im_pro_get_current_clk_hz( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B, &cpu_clk_hz, &macro_clk_hz );
		im_pro_common_wait_by_clk_cycle( 5, macro_clk_hz, cpu_clk_hz );
#ifndef CO_DEBUG_ON_PC
//			__nop();
#endif
	}
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
B2BTOP macro control data setting
@param[in]	unit_no : Unit number.
@param[in]	ctrl : B2BTOP macro control information structure
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_B2BTOP_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_B2BTOP_CTRL* ctrl )
{
#ifdef CO_PARAM_CHECK
	if (ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_B2BTOP_Ctrl error. ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	gIM_PRO_B2BTOP_Reg_Ptr_Tbl[unit_no].reg_ptr->B2BTOPCTL1.bit.B2BISW	= ctrl->input_mode;
	gIM_PRO_B2BTOP_Reg_Ptr_Tbl[unit_no].reg_ptr->B2BTOPCTL1.bit.CAGSL	= ctrl->cag_target_sel;
	gIM_PRO_B2BTOP_Reg_Ptr_Tbl[unit_no].reg_ptr->B2BTOPCTL2.bit.ORG0	= ctrl->first_pixel[0];
	gIM_PRO_B2BTOP_Reg_Ptr_Tbl[unit_no].reg_ptr->B2BTOPCTL2.bit.ORG1	= ctrl->first_pixel[1];
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Start ELF
@param[in]	unit_no : Unit number.
@retval		D_DDIM_OK				: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PRCH not running NG
*/
INT32 Im_PRO_ELF_Start( E_IM_PRO_UNIT_NUM unit_no )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFTRG.bit.ELFTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	im_pro_b2b_set_start_status(gIM_PRO_ELF_Status_Tbl[unit_no], 0);

	return D_DDIM_OK;
}

/**
Stop ELF
@param[in]	unit_no : Unit number.
@param[in]	force : force stop option
*/
VOID Im_PRO_ELF_Stop( E_IM_PRO_UNIT_NUM unit_no, UCHAR force )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	if(force == 0) {
		IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFTRG.bit.ELFTRG = D_IM_PRO_TRG_FRAME_STOP;		// stop
	}
	else {
		IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFTRG.bit.ELFTRG = D_IM_PRO_TRG_FORCE_STOP;		// force stop
	}
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	im_pro_b2b_set_stop_status(gIM_PRO_ELF_Status_Tbl[unit_no], 0);
}

/**
A setup of enable access to the built-in RAM of ELF.
@param[in]	unit_no : Unit number.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_ELF_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg )
{
#ifdef CO_PARAM_CHECK
	if( ( paen_trg == 0 ) && ( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFTRG.bit.ELFTRG != D_IM_PRO_TRG_STATUS_STOPPED ) ) {
		Ddim_Assertion(("I:Im_PRO_ELF_Set_PAEN. macro has not stopped error.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFPAEN.bit.EPAEN = paen_trg;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
ELF control common setup 
@param[in]	unit_no : Unit number.
@param[in]	elf_common_ctrl : ELF control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_ELF_Ctrl_Common( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_COMMON_CTRL* elf_common_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (elf_common_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_ELF_Ctrl_Common error. elf_common_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFCTL.bit.ELFPDD		= elf_common_ctrl->pix_padding;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFCTL.bit.ELFRNG		= elf_common_ctrl->padding_mode;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFCTL.bit.ELFAFS		= elf_common_ctrl->nr_select;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
ELF control setup 
@param[in]	unit_no : Unit number.
@param[in]	elf_ctrl : ELF control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_ELF_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_CTRL* elf_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (elf_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_ELF_Ctrl error. elf_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFCTL.bit.N1S5EN		= elf_ctrl->cmn1.n1s5_enable;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFCTL.bit.N5S9EN		= elf_ctrl->cmn1.n5s9_enable;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFCTL.bit.NFS9EN		= elf_ctrl->cmn1.f9_enable;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFCTL.bit.NLA0EN		= elf_ctrl->cmn1.a0_enable;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFCTL.bit.F5MD		= elf_ctrl->cmn1.f5_filter_mode;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFCTL.bit.F9MD		= elf_ctrl->cmn1.f9_filter_mode;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFCTL.bit.EFMD		= elf_ctrl->cmn1.e_filter_mode;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFCTL.bit.LFMD		= elf_ctrl->cmn1.l_filter_mode;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFL.ELFL1.bit.ELFL1	= elf_ctrl->cmn2.elfl1;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFL.ELFL1.bit.ELFL2	= elf_ctrl->cmn2.elfl2;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFL.ELFL2.bit.ELFL3	= elf_ctrl->cmn2.elfl3;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFL.ELFL2.bit.ELFL4	= elf_ctrl->cmn2.elfl4;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
A setup of ELF effective area 
@param[in]	unit_no : Unit number.
@param[in]	elf_area : ELF area information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_ELF_Set_Area( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_AREA* elf_area )
{
#ifdef CO_PARAM_CHECK
	if (elf_area == NULL){
		Ddim_Assertion(("I:Im_PRO_ELF_Set_Area error. elf_area=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_ELF_ELFGH_MIN, D_IM_PRO_ELF_ELFGH_MAX, elf_area->g_pos_x, "Im_PRO_ELF_Set_Area : g_pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_ELF_ELFGV_MIN, D_IM_PRO_ELF_ELFGV_MAX, elf_area->g_pos_y, "Im_PRO_ELF_Set_Area : g_pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_ELF_ELFGHW_MIN, D_IM_PRO_ELF_ELFGHW_MAX, elf_area->g_width, "Im_PRO_ELF_Set_Area : g_width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_ELF_ELFGVW_MIN, D_IM_PRO_ELF_ELFGVW_MAX, elf_area->g_lines, "Im_PRO_ELF_Set_Area : g_lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_ELF_ELFH_MIN, D_IM_PRO_ELF_ELFH_MAX, elf_area->pos_x, "Im_PRO_ELF_Set_Area : pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_ELF_ELFV_MIN, D_IM_PRO_ELF_ELFV_MAX, elf_area->pos_y, "Im_PRO_ELF_Set_Area : pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_ELF_ELFHW_MIN, D_IM_PRO_ELF_ELFHW_MAX, elf_area->width, "Im_PRO_ELF_Set_Area : width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_ELF_ELFVW_MIN, D_IM_PRO_ELF_ELFVW_MAX, elf_area->lines, "Im_PRO_ELF_Set_Area : lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFGWS.bit.ELFGHW	= elf_area->g_width;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFGWS.bit.ELFGVW	= elf_area->g_lines;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFGWP.bit.ELFGH	= elf_area->g_pos_x;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFGWP.bit.ELFGV	= elf_area->g_pos_y;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFWS.bit.ELFHW	= elf_area->width;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFWS.bit.ELFVW	= elf_area->lines;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFWP.bit.ELFH		= elf_area->pos_x;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFWP.bit.ELFV		= elf_area->pos_y;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set noise func information.
@param[in]	unit_no : Unit number.
@param[in]	nfunc : ELF noise func information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_ELF_Set_Noisefunc( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_NOISE_FUNC* nfunc )
{
#ifdef CO_PARAM_CHECK
	if (nfunc == NULL){
		Ddim_Assertion(("I:Im_PRO_ELF_Set_Noisefunc error. nfunc=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	// Offset
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBOR.ANBOR1.bit.ANBOR_0	= nfunc[0].offset[ 0 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBOR.ANBOR1.bit.ANBOR_1	= nfunc[0].offset[ 1 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBOR.ANBOR2.bit.ANBOR_2	= nfunc[0].offset[ 2 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBOR.ANBOR2.bit.ANBOR_3	= nfunc[0].offset[ 3 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBOG.ANBOG1.bit.ANBOG_0	= nfunc[1].offset[ 0 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBOG.ANBOG1.bit.ANBOG_1	= nfunc[1].offset[ 1 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBOG.ANBOG2.bit.ANBOG_2	= nfunc[1].offset[ 2 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBOG.ANBOG2.bit.ANBOG_3	= nfunc[1].offset[ 3 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBOB.ANBOB1.bit.ANBOB_0	= nfunc[2].offset[ 0 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBOB.ANBOB1.bit.ANBOB_1	= nfunc[2].offset[ 1 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBOB.ANBOB2.bit.ANBOB_2	= nfunc[2].offset[ 2 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBOB.ANBOB2.bit.ANBOB_3	= nfunc[2].offset[ 3 ];
	// Gain
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBKR.ANBKR1.bit.ANBKR_0	= nfunc[0].gain[ 0 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBKR.ANBKR1.bit.ANBKR_1	= nfunc[0].gain[ 1 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBKR.ANBKR2.bit.ANBKR_2	= nfunc[0].gain[ 2 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBKR.ANBKR2.bit.ANBKR_3	= nfunc[0].gain[ 3 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBKG.ANBKG1.bit.ANBKG_0	= nfunc[1].gain[ 0 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBKG.ANBKG1.bit.ANBKG_1	= nfunc[1].gain[ 1 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBKG.ANBKG2.bit.ANBKG_2	= nfunc[1].gain[ 2 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBKG.ANBKG2.bit.ANBKG_3	= nfunc[1].gain[ 3 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBKB.ANBKB1.bit.ANBKB_0	= nfunc[2].gain[ 0 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBKB.ANBKB1.bit.ANBKB_1	= nfunc[2].gain[ 1 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBKB.ANBKB2.bit.ANBKB_2	= nfunc[2].gain[ 2 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBKB.ANBKB2.bit.ANBKB_3	= nfunc[2].gain[ 3 ];
	// Boundary
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBDR.ANBDR1.bit.ANBDR_0	= nfunc[0].boundary[ 0 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBDR.ANBDR1.bit.ANBDR_1	= nfunc[0].boundary[ 1 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBDR.ANBDR2.bit.ANBDR_2	= nfunc[0].boundary[ 2 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBDR.ANBDR2.bit.ANBDR_3	= nfunc[0].boundary[ 3 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBDG.ANBDG1.bit.ANBDG_0	= nfunc[1].boundary[ 0 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBDG.ANBDG1.bit.ANBDG_1	= nfunc[1].boundary[ 1 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBDG.ANBDG2.bit.ANBDG_2	= nfunc[1].boundary[ 2 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBDG.ANBDG2.bit.ANBDG_3	= nfunc[1].boundary[ 3 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBDB.ANBDB1.bit.ANBDB_0	= nfunc[2].boundary[ 0 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBDB.ANBDB1.bit.ANBDB_1	= nfunc[2].boundary[ 1 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBDB.ANBDB2.bit.ANBDB_2	= nfunc[2].boundary[ 2 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ANBDB.ANBDB2.bit.ANBDB_3	= nfunc[2].boundary[ 3 ];
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set Light/Outlines filter information.
@param[in]	unit_no : Unit number.
@param[in]	lgt_filter : ELF Light/Outlines filter func information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_ELF_Set_LGT_EDG_Filter( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_LGT_EDG_FUNC* lgt_filter )
{
#ifdef CO_PARAM_CHECK
	if (lgt_filter == NULL){
		Ddim_Assertion(("I:Im_PRO_ELF_Set_LGT_EDG_Filter error. lgt_filter=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFZP.bit.ELFZP		= lgt_filter->zero_point;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.LGTW.LGTW1.bit.LGTWR	= lgt_filter->gain_r;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.LGTW.LGTW1.bit.LGTWG	= lgt_filter->gain_g;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.LGTW.LGTW2.bit.LGTWB	= lgt_filter->gain_b;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.LGT.bit.LGTR			= lgt_filter->factor_r;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.LGT.bit.LGTG			= lgt_filter->factor_g;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.LGT.bit.LGTB			= lgt_filter->factor_b;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.LGT.bit.LGTC			= lgt_filter->color_para;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.LGTE.bit.LGTER			= lgt_filter->contour_r;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.LGTE.bit.LGTEG			= lgt_filter->contour_g;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.LGTE.bit.LGTEB			= lgt_filter->contour_b;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EDG2ANBA.bit.EDG2ANBA	= lgt_filter->contour_a_1;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EDGSCLA.bit.EDGSCLA	= lgt_filter->contour_a_2;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EDG2ANBB.bit.EDG2ANBB	= lgt_filter->contour_b_1;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EDGSCLB.bit.EDGSCLB	= lgt_filter->contour_b_2;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EDG2ANBC.bit.EDG2ANBC	= lgt_filter->contour_c_1;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EDGSCLC.bit.EDGSCLC	= lgt_filter->contour_c_2;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set E/L filter information.
@param[in]	unit_no : Unit number.
@param[in]	e_l_filter : ELF E/L filter func information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_ELF_Set_E_L_Filter( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_L_E_FUNC* e_l_filter )
{
#ifdef CO_PARAM_CHECK
	if (e_l_filter == NULL){
		Ddim_Assertion(("I:Im_PRO_ELF_Set_E_L_Filter error. e_l_filter=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EFTH.EFTH1.bit.EFL1TH			= e_l_filter->e_threshold_1;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EFTH.EFTH1.bit.EFL2TH			= e_l_filter->e_threshold_2;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EFTH.EFTH2.bit.EFL3TH			= e_l_filter->e_threshold_3;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EFTH.EFTH2.bit.EFL4TH			= e_l_filter->e_threshold_4;
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.LFTH, union io_lfth, LFTH, e_l_filter->l_threshold );
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELCOME.bit.ELCOME1				= e_l_filter->l_contour_1;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELCOME.bit.ELCOME2				= e_l_filter->l_contour_2;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELCOMEN.bit.ELCOME1N			= e_l_filter->l_cintensity_1;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELCOMEN.bit.ELCOME2N			= e_l_filter->l_cintensity_2;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set F9 filter information.
@param[in]	unit_no : Unit number.
@param[in]	f9_filter : ELF F9 filter func information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_ELF_Set_F9_Filter( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_f9_SAMPLING* f9_filter )
{
#ifdef CO_PARAM_CHECK
	if (f9_filter == NULL){
		Ddim_Assertion(("I:Im_PRO_ELF_Set_F9_Filter error. f9_filter=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9W.FS9W1.bit.FS9WR			= f9_filter->rgb_gain.gain[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9W.FS9W1.bit.FS9WG			= f9_filter->rgb_gain.gain[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9W.FS9W2.bit.FS9WB			= f9_filter->rgb_gain.gain[2];
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9YC.FS9YC1, union io_fs9yc_1, FS9YC00, f9_filter->yc_conv_matrix[0] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9YC.FS9YC1, union io_fs9yc_1, FS9YC01, f9_filter->yc_conv_matrix[1] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9YC.FS9YC2, union io_fs9yc_2, FS9YC02, f9_filter->yc_conv_matrix[2] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9YC.FS9YC2, union io_fs9yc_2, FS9YC10, f9_filter->yc_conv_matrix[3] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9YC.FS9YC3, union io_fs9yc_3, FS9YC11, f9_filter->yc_conv_matrix[4] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9YC.FS9YC3, union io_fs9yc_3, FS9YC12, f9_filter->yc_conv_matrix[5] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9YC.FS9YC4, union io_fs9yc_4, FS9YC20, f9_filter->yc_conv_matrix[6] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9YC.FS9YC4, union io_fs9yc_4, FS9YC21, f9_filter->yc_conv_matrix[7] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9YC.FS9YC5, union io_fs9yc_5, FS9YC22, f9_filter->yc_conv_matrix[8] );

	// groupe A
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FASTR.bit.FASTR				= f9_filter->f9a_group.strength;
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.CRVAF, union io_crvaf, CRVAFX1, f9_filter->f9a_group.e_focus_coord[0] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.CRVAF, union io_crvaf, CRVAFY1, f9_filter->f9a_group.e_focus_coord[1] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.CRVAF, union io_crvaf, CRVAFX2, f9_filter->f9a_group.e_focus_coord[2] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.CRVAF, union io_crvaf, CRVAFY2, f9_filter->f9a_group.e_focus_coord[3] );
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVABTOF.bit.FCRVABTOF1		= f9_filter->f9a_group.e_polygonal_line_offset1;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVABTGA.bit.FCRVABTGA0		= f9_filter->f9a_group.e_polygonal_line_minus_inclination[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVABTGA.bit.FCRVABTGA1		= f9_filter->f9a_group.e_polygonal_line_minus_inclination[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVABTBD.bit.FCRVABTBD1		= f9_filter->f9a_group.e_polygonal_line_boundary1;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVABTCP.bit.FCRVABTCPH		= f9_filter->f9a_group.e_upper_clip;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVABTCP.bit.FCRVABTCPL		= f9_filter->f9a_group.e_lower_clip;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FYABTGA.bit.FYABTGA0			= f9_filter->f9a_group.l_polygonal_line_plus_inclination0;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FYABTGA.bit.FYABTGA1			= f9_filter->f9a_group.l_polygonal_line_minus_inclination1;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FYABTBD.bit.FYABTBD0			= f9_filter->f9a_group.l_polygonal_line_boundary[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FYABTBD.bit.FYABTBD1			= f9_filter->f9a_group.l_polygonal_line_boundary[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FYABTCLP.bit.FYABTCLPH			= f9_filter->f9a_group.l_upper_clip;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FYABTCLP.bit.FYABTCLPL			= f9_filter->f9a_group.l_lower_clip;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FAEHH.bit.FAECHH				= f9_filter->f9a_group.t_f_profile_transition;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FAEHH.bit.FAETHH				= f9_filter->f9a_group.t_f_profile_threshold;

	// groupe B
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FBSTR.bit.FBSTR				= f9_filter->f9b_group.strength;
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.CRVBF, union io_crvbf, CRVBFX1, f9_filter->f9b_group.e_focus_coord[0] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.CRVBF, union io_crvbf, CRVBFY1, f9_filter->f9b_group.e_focus_coord[1] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.CRVBF, union io_crvbf, CRVBFX2, f9_filter->f9b_group.e_focus_coord[2] );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.CRVBF, union io_crvbf, CRVBFY2, f9_filter->f9b_group.e_focus_coord[3] );
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVBBTOF.bit.FCRVBBTOF1		= f9_filter->f9b_group.e_polygonal_line_offset1;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVBBTGA.bit.FCRVBBTGA0		= f9_filter->f9b_group.e_polygonal_line_minus_inclination[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVBBTGA.bit.FCRVBBTGA1		= f9_filter->f9b_group.e_polygonal_line_minus_inclination[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVBBTBD.bit.FCRVBBTBD1		= f9_filter->f9b_group.e_polygonal_line_boundary1;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVBBTCP.bit.FCRVBBTCPL		= f9_filter->f9b_group.e_upper_clip;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FCRVBBTCP.bit.FCRVBBTCPH		= f9_filter->f9b_group.e_lower_clip;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FYBBTGA.bit.FYBBTGA0			= f9_filter->f9b_group.l_polygonal_line_plus_inclination0;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FYBBTGA.bit.FYBBTGA1			= f9_filter->f9b_group.l_polygonal_line_minus_inclination1;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FYBBTBD.bit.FYBBTBD0			= f9_filter->f9b_group.l_polygonal_line_boundary[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FYBBTBD.bit.FYBBTBD1			= f9_filter->f9b_group.l_polygonal_line_boundary[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FYBBTCLP.bit.FYBBTCLPL			= f9_filter->f9b_group.l_upper_clip;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FYBBTCLP.bit.FYBBTCLPH			= f9_filter->f9b_group.l_lower_clip;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FBEHH.bit.FBECHH				= f9_filter->f9b_group.t_f_profile_transition;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FBEHH.bit.FBETHH				= f9_filter->f9b_group.t_f_profile_threshold;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set F9 filter RGB gain.
@param[in]	unit_no : Unit number.
@param[in]	rgb_gain : White balance gain
@retval		D_DDIM_OK					: Seting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Seting NG
*/
INT32 Im_PRO_ELF_Set_F9_RGBGain( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_f9_RGB_GAIN* rgb_gain )
{
#ifdef CO_PARAM_CHECK
	if( rgb_gain == NULL ){
		Ddim_Assertion(("I:Im_PRO_ELF_Set_F9_RGBGain error. rgb_gain=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	//Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9W.FS9W1.bit.FS9WR				= rgb_gain->gain[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9W.FS9W1.bit.FS9WG				= rgb_gain->gain[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.FS9W.FS9W2.bit.FS9WB				= rgb_gain->gain[2];
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}
/**
Set 1s5 sampling information.
@param[in]	unit_no : Unit number.
@param[in]	s1s5_sampling : ELF 1s5 sampling information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_ELF_Set_1s5_Sampling( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_1s5_SAMPLING* s1s5_sampling )
{
#ifdef CO_PARAM_CHECK
	if (s1s5_sampling == NULL){
		Ddim_Assertion(("I:Im_PRO_ELF_Set_1s5_Sampling error. s1s5_sampling=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5E.A1S5E1.bit.A1S5E1		= s1s5_sampling->s1s5_threshold[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5E.A1S5E1.bit.A1S5E2		= s1s5_sampling->s1s5_threshold[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5E.A1S5E2.bit.A1S5E3		= s1s5_sampling->s1s5_threshold[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L1.A1S5L11.bit.A1S5L1E1N	= s1s5_sampling->s1s5_nr_l1_E13[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L1.A1S5L11.bit.A1S5L1E2N	= s1s5_sampling->s1s5_nr_l1_E13[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L1.A1S5L12.bit.A1S5L1E3N	= s1s5_sampling->s1s5_nr_l1_E13[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L2.A1S5L21.bit.A1S5L2E1N	= s1s5_sampling->s1s5_nr_l2_E13[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L2.A1S5L21.bit.A1S5L2E2N	= s1s5_sampling->s1s5_nr_l2_E13[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L2.A1S5L22.bit.A1S5L2E3N	= s1s5_sampling->s1s5_nr_l2_E13[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L3.A1S5L31.bit.A1S5L3E1N	= s1s5_sampling->s1s5_nr_l3_E13[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L3.A1S5L31.bit.A1S5L3E2N	= s1s5_sampling->s1s5_nr_l3_E13[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L3.A1S5L32.bit.A1S5L3E3N	= s1s5_sampling->s1s5_nr_l3_E13[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L4.A1S5L41.bit.A1S5L4E1N	= s1s5_sampling->s1s5_nr_l4_E13[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L4.A1S5L41.bit.A1S5L4E2N	= s1s5_sampling->s1s5_nr_l4_E13[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A1S5L4.A1S5L42.bit.A1S5L4E3N	= s1s5_sampling->s1s5_nr_l4_E13[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NR1S5RGB.NR1S5RGB1.bit.NR1S5R	= s1s5_sampling->s1s5_nr_adjustment[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NR1S5RGB.NR1S5RGB1.bit.NR1S5G	= s1s5_sampling->s1s5_nr_adjustment[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NR1S5RGB.NR1S5RGB2.bit.NR1S5B	= s1s5_sampling->s1s5_nr_adjustment[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NS1S5LM.bit.NS1S5LMP			= s1s5_sampling->s1s5_noise_limit_plus;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NS1S5LM.bit.NS1S5LMM			= s1s5_sampling->s1s5_noise_limit_minus;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set 5s9 sampling information.
@param[in]	unit_no : Unit number.
@param[in]	s5s9_sampling : ELF 5s9 sampling information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_ELF_Set_5s9_Sampling( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_5s9_SAMPLING* s5s9_sampling )
{
#ifdef CO_PARAM_CHECK
	if (s5s9_sampling == NULL){
		Ddim_Assertion(("I:Im_PRO_ELF_Set_5s9_Sampling error. s5s9_sampling=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9E.A5S9E1.bit.A5S9E1		= s5s9_sampling->s5s9_threshold[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9E.A5S9E1.bit.A5S9E2		= s5s9_sampling->s5s9_threshold[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9E.A5S9E2.bit.A5S9E3		= s5s9_sampling->s5s9_threshold[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L1.A5S9L11.bit.A5S9L1E1N	= s5s9_sampling->s5s9_nr_l1_E13[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L1.A5S9L11.bit.A5S9L1E2N	= s5s9_sampling->s5s9_nr_l1_E13[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L1.A5S9L12.bit.A5S9L1E3N	= s5s9_sampling->s5s9_nr_l1_E13[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L2.A5S9L21.bit.A5S9L2E1N	= s5s9_sampling->s5s9_nr_l2_E13[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L2.A5S9L21.bit.A5S9L2E2N	= s5s9_sampling->s5s9_nr_l2_E13[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L2.A5S9L22.bit.A5S9L2E3N	= s5s9_sampling->s5s9_nr_l2_E13[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L3.A5S9L31.bit.A5S9L3E1N	= s5s9_sampling->s5s9_nr_l3_E13[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L3.A5S9L31.bit.A5S9L3E2N	= s5s9_sampling->s5s9_nr_l3_E13[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L3.A5S9L32.bit.A5S9L3E3N	= s5s9_sampling->s5s9_nr_l3_E13[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L4.A5S9L41.bit.A5S9L4E1N	= s5s9_sampling->s5s9_nr_l4_E13[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L4.A5S9L41.bit.A5S9L4E2N	= s5s9_sampling->s5s9_nr_l4_E13[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.A5S9L4.A5S9L42.bit.A5S9L4E3N	= s5s9_sampling->s5s9_nr_l4_E13[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NR5S9RGB.NR5S9RGB1.bit.NR5S9R	= s5s9_sampling->s5s9_nr_adjustment[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NR5S9RGB.NR5S9RGB1.bit.NR5S9G	= s5s9_sampling->s5s9_nr_adjustment[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NR5S9RGB.NR5S9RGB2.bit.NR5S9B	= s5s9_sampling->s5s9_nr_adjustment[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NS5S9LM.bit.NS5S9LMP			= s5s9_sampling->s5s9_noise_limit_plus;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NS5S9LM.bit.NS5S9LMM			= s5s9_sampling->s5s9_noise_limit_minus;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set Adjust the intensity noise suppression information.
@param[in]	unit_no : Unit number.
@param[in]	noise_adj_filter : ELF Adjust the intensity noise suppression information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_ELF_Set_Noise_Adj( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_Noise_Adj* noise_adj_filter )
{
#ifdef CO_PARAM_CHECK
	if (noise_adj_filter == NULL){
		Ddim_Assertion(("I:Im_PRO_ELF_Set_Noise_Adj error. noise_adj_filter=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFMNNRRL.bit.ELFMNNRRL1	= noise_adj_filter->nr_strength_l1[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFMNNRGL.bit.ELFMNNRGL1	= noise_adj_filter->nr_strength_l1[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFMNNRBL.bit.ELFMNNRBL1	= noise_adj_filter->nr_strength_l1[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFMNNRRL.bit.ELFMNNRRL2	= noise_adj_filter->nr_strength_l2[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFMNNRGL.bit.ELFMNNRGL2	= noise_adj_filter->nr_strength_l2[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFMNNRBL.bit.ELFMNNRBL2	= noise_adj_filter->nr_strength_l2[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFMNNRRL.bit.ELFMNNRRL3	= noise_adj_filter->nr_strength_l3[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFMNNRGL.bit.ELFMNNRGL3	= noise_adj_filter->nr_strength_l3[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFMNNRBL.bit.ELFMNNRBL3	= noise_adj_filter->nr_strength_l3[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFMNNRRL.bit.ELFMNNRRL4	= noise_adj_filter->nr_strength_l4[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFMNNRGL.bit.ELFMNNRGL4	= noise_adj_filter->nr_strength_l4[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFMNNRBL.bit.ELFMNNRBL4	= noise_adj_filter->nr_strength_l4[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFAFNR.bit.ELFAFNR		= noise_adj_filter->af_path_nr_strength;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Set A0-function noise suppression information.
@param[in]	unit_no : Unit number.
@param[in]	a0_filter : A0-function noise suppression information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_ELF_Set_A0_Sampling( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_a0_FILTER* a0_filter )
{
#ifdef CO_PARAM_CHECK
	if (a0_filter == NULL){
		Ddim_Assertion(("I:Im_PRO_ELF_Set_A0_Sampling error. a0_filter=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZELADCMD.bit.ZELADCMD			= a0_filter->dpc_mode;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBOR.ZANBOR1.bit.ZANBOR_0	= a0_filter->nce[0].offset[ 0 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBOR.ZANBOR1.bit.ZANBOR_1	= a0_filter->nce[0].offset[ 1 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBOR.ZANBOR2.bit.ZANBOR_2	= a0_filter->nce[0].offset[ 2 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBOR.ZANBOR2.bit.ZANBOR_3	= a0_filter->nce[0].offset[ 3 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBOG.ZANBOG1.bit.ZANBOG_0	= a0_filter->nce[1].offset[ 0 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBOG.ZANBOG1.bit.ZANBOG_1	= a0_filter->nce[1].offset[ 1 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBOG.ZANBOG2.bit.ZANBOG_2	= a0_filter->nce[1].offset[ 2 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBOG.ZANBOG2.bit.ZANBOG_3	= a0_filter->nce[1].offset[ 3 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBOB.ZANBOB1.bit.ZANBOB_0	= a0_filter->nce[2].offset[ 0 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBOB.ZANBOB1.bit.ZANBOB_1	= a0_filter->nce[2].offset[ 1 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBOB.ZANBOB2.bit.ZANBOB_2	= a0_filter->nce[2].offset[ 2 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBOB.ZANBOB2.bit.ZANBOB_3	= a0_filter->nce[2].offset[ 3 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBKR.ZANBKR1.bit.ZANBKR_0	= a0_filter->nce[0].gain[ 0 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBKR.ZANBKR1.bit.ZANBKR_1	= a0_filter->nce[0].gain[ 1 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBKR.ZANBKR2.bit.ZANBKR_2	= a0_filter->nce[0].gain[ 2 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBKR.ZANBKR2.bit.ZANBKR_3	= a0_filter->nce[0].gain[ 3 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBKG.ZANBKG1.bit.ZANBKG_0	= a0_filter->nce[1].gain[ 0 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBKG.ZANBKG1.bit.ZANBKG_1	= a0_filter->nce[1].gain[ 1 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBKG.ZANBKG2.bit.ZANBKG_2	= a0_filter->nce[1].gain[ 2 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBKG.ZANBKG2.bit.ZANBKG_3	= a0_filter->nce[1].gain[ 3 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBKB.ZANBKB1.bit.ZANBKB_0	= a0_filter->nce[2].gain[ 0 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBKB.ZANBKB1.bit.ZANBKB_1	= a0_filter->nce[2].gain[ 1 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBKB.ZANBKB2.bit.ZANBKB_2	= a0_filter->nce[2].gain[ 2 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBKB.ZANBKB2.bit.ZANBKB_3	= a0_filter->nce[2].gain[ 3 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBDR.ZANBDR1.bit.ZANBDR_0	= a0_filter->nce[0].area_boundary[ 0 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBDR.ZANBDR1.bit.ZANBDR_1	= a0_filter->nce[0].area_boundary[ 1 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBDR.ZANBDR2.bit.ZANBDR_2	= a0_filter->nce[0].area_boundary[ 2 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBDR.ZANBDR2.bit.ZANBDR_3	= a0_filter->nce[0].area_boundary[ 3 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBDG.ZANBDG1.bit.ZANBDG_0	= a0_filter->nce[1].area_boundary[ 0 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBDG.ZANBDG1.bit.ZANBDG_1	= a0_filter->nce[1].area_boundary[ 1 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBDG.ZANBDG2.bit.ZANBDG_2	= a0_filter->nce[1].area_boundary[ 2 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBDG.ZANBDG2.bit.ZANBDG_3	= a0_filter->nce[1].area_boundary[ 3 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBDB.ZANBDB1.bit.ZANBDB_0	= a0_filter->nce[2].area_boundary[ 0 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBDB.ZANBDB1.bit.ZANBDB_1	= a0_filter->nce[2].area_boundary[ 1 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBDB.ZANBDB2.bit.ZANBDB_2	= a0_filter->nce[2].area_boundary[ 2 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZANBDB.ZANBDB2.bit.ZANBDB_3	= a0_filter->nce[2].area_boundary[ 3 ];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLGTW.ZLGTW1.bit.ZLGTWR		= a0_filter->lumi_rgb_gain[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLGTW.ZLGTW1.bit.ZLGTWG		= a0_filter->lumi_rgb_gain[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLGTW.ZLGTW2.bit.ZLGTWB		= a0_filter->lumi_rgb_gain[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLGT.bit.ZLGTR					= a0_filter->lumi_rgb_coeff[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLGT.bit.ZLGTG					= a0_filter->lumi_rgb_coeff[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLGT.bit.ZLGTB					= a0_filter->lumi_rgb_coeff[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLGT.bit.ZLGTC					= a0_filter->lumi_c_coeff;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLGTE.bit.ZLGTER				= a0_filter->t_weight[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLGTE.bit.ZLGTEG				= a0_filter->t_weight[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLGTE.bit.ZLGTEB				= a0_filter->t_weight[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZEDG2ANBA.bit.ZEDG2ANBA		= a0_filter->t_adj_a.coeff1;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZEDGSCLA.bit.ZEDGSCLA			= a0_filter->t_adj_a.coeff2;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZEDG2ANBB.bit.ZEDG2ANBB		= a0_filter->t_adj_b.coeff1;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZEDGSCLB.bit.ZEDGSCLB			= a0_filter->t_adj_b.coeff2;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNGE.ZALNGE1.bit.ZALNGE1		= a0_filter->t_long_wavelength_threshold[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNGE.ZALNGE1.bit.ZALNGE2		= a0_filter->t_long_wavelength_threshold[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNGE.ZALNGE2.bit.ZALNGE3		= a0_filter->t_long_wavelength_threshold[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNG.ZALNG1.bit.ZALNGL1E1N	= a0_filter->t_long_wavelength_nr_strength[0][0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNG.ZALNG1.bit.ZALNGL1E2N	= a0_filter->t_long_wavelength_nr_strength[0][1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNG.ZALNG2.bit.ZALNGL1E3N	= a0_filter->t_long_wavelength_nr_strength[0][2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNG.ZALNG2.bit.ZALNGL2E1N	= a0_filter->t_long_wavelength_nr_strength[1][0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNG.ZALNG3.bit.ZALNGL2E2N	= a0_filter->t_long_wavelength_nr_strength[1][1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNG.ZALNG3.bit.ZALNGL2E3N	= a0_filter->t_long_wavelength_nr_strength[1][2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNG.ZALNG4.bit.ZALNGL3E1N	= a0_filter->t_long_wavelength_nr_strength[2][0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNG.ZALNG4.bit.ZALNGL3E2N	= a0_filter->t_long_wavelength_nr_strength[2][1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNG.ZALNG5.bit.ZALNGL3E3N	= a0_filter->t_long_wavelength_nr_strength[2][2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNG.ZALNG5.bit.ZALNGL4E1N	= a0_filter->t_long_wavelength_nr_strength[3][0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNG.ZALNG6.bit.ZALNGL4E2N	= a0_filter->t_long_wavelength_nr_strength[3][1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZALNG.ZALNG6.bit.ZALNGL4E3N	= a0_filter->t_long_wavelength_nr_strength[3][2];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLFTH.bit.ZLFTH				= a0_filter->l_filter_extraction_threshold;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLCOME.bit.ZLCOME1				= a0_filter->l_app_tolerance_threshold[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLCOME.bit.ZLCOME2				= a0_filter->l_app_tolerance_threshold[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLCOMEN.bit.ZLCOME1N			= a0_filter->l_app_strength[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZLCOMEN.bit.ZLCOME2N			= a0_filter->l_app_strength[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZNSLNGLM.bit.ZNSLNGLMP			= a0_filter->long_wavelength_noise_p_limiter;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ZNSLNGLM.bit.ZNSLNGLMM			= a0_filter->long_wavelength_noise_m_limiter;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Start NSL
@param[in]	unit_no : Unit number.
@retval		D_DDIM_OK				: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PRCH not running NG
*/
INT32 Im_PRO_ELF_Start_NSL( E_IM_PRO_UNIT_NUM unit_no )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NSLTRG.bit.NSLTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	im_pro_b2b_set_start_status(gIM_PRO_ELF_NSL_Status_Tbl[unit_no], 0);

	return D_DDIM_OK;
}

/**
Stop NSL
@param[in]	unit_no : Unit number.
@param[in]	force : force stop option
*/
VOID Im_PRO_ELF_Stop_NSL( E_IM_PRO_UNIT_NUM unit_no, UCHAR force )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	if(force == 0) {
		IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NSLTRG.bit.NSLTRG = D_IM_PRO_TRG_FRAME_STOP;		// stop
	}
	else {
		IO_PRO.IMG_PIPE[unit_no].B2B.ELF.NSLTRG.bit.NSLTRG = D_IM_PRO_TRG_FORCE_STOP;		// force stop
	}
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	im_pro_b2b_set_stop_status(gIM_PRO_ELF_NSL_Status_Tbl[unit_no], 0);
}

/**
NSL control setup 
@param[in]	unit_no : Unit number.
@param[in]	nsl_ctrl : ELF control information
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_ELF_Ctrl_NSL( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_NSL_CTRL* nsl_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (nsl_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_ELF_Ctrl_NSL error. nsl_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if (( nsl_ctrl->long_noise_comp == E_IM_PRO_ELF_NSLKEN_EN ) && ( nsl_ctrl->l_noise_precision > E_IM_PRO_ELF_NSLRES_1 )){
		Ddim_Assertion(("I:Im_PRO_ELF_Ctrl_NSL parameter combination error.  When NSLKNE=1, setting NSLRES to 2 or 3 is prohibited. NSLKNE(%u) NSLRES(%u)\n", nsl_ctrl->long_noise_comp, nsl_ctrl->l_noise_precision));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFCTL.bit.NSLMD				= nsl_ctrl->l_noise_mode;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFCTL.bit.NSLRES				= nsl_ctrl->l_noise_precision;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFCTL.bit.NSLKNE				= nsl_ctrl->long_noise_comp;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
A setup of enable access to the built-in RAM of A0 function.
@param[in]	unit_no : Unit number.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_ELF_Set_A0_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg )
{
#ifdef CO_PARAM_CHECK
	if( ( paen_trg == 0 ) && ( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFTRG.bit.ELFTRG != D_IM_PRO_TRG_STATUS_STOPPED ) ) {
		Ddim_Assertion(("I:Im_PRO_ELF_Set_A0_PAEN. macro has not stopped error.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFPAEN.bit.EPAENA0 = paen_trg;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
A setup of enable access to the built-in RAM of NSL.
@param[in]	unit_no : Unit number.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_ELF_Set_NSL_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg )
{
#ifdef CO_PARAM_CHECK
	if( ( paen_trg == 0 ) && ( IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFTRG.bit.ELFTRG != D_IM_PRO_TRG_STATUS_STOPPED ) ) {
		Ddim_Assertion(("I:Im_PRO_ELF_Set_NSL_PAEN. macro has not stopped error.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.ELFPAEN.bit.EPAENNSL = paen_trg;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Get the top address of the address array of ELF control.
@param[in]		unit_no : Unit number.
@param[out]		addr					: Top address of the address array of ELF control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
INT32 Im_PRO_Get_RdmaAddr_ELF_Cntl( E_IM_PRO_UNIT_NUM unit_no, const T_IM_PRO_RDMA_ELF_ADDR** addr )
{
#ifdef CO_PARAM_CHECK
	if( addr == NULL ) {
		Ddim_Assertion(("I:Im_PRO_Get_RdmaAddr_ELF_Cntl. error. addr=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	*addr = &(gIM_PRO_ELF_Addr[unit_no]);

	return D_DDIM_OK;
}

/**
Set Edge level adjustment parameter for high frequency area
@param[in]	unit_no : Unit number.
@param[in]	edge_adj : Edge level adjustment parameter
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_ELF_Set_Edge_Adj( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_ELF_HIGH_FREQ_AREA_EDGE_ADJ* edge_adj )
{
#ifdef CO_PARAM_CHECK
	if( edge_adj == NULL ) {
		Ddim_Assertion(("I:Im_PRO_ELF_Set_Edge_Adj. error. edge_adj=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EDGCATH.bit.EDGCATHA	= edge_adj->high_freq_area_adj_coeff;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EDGCATH.bit.ELFTEST1	= edge_adj->test_para_1;
	IO_PRO.IMG_PIPE[unit_no].B2B.ELF.EDGCATH.bit.ELFTEST2	= edge_adj->test_para_2;

	return D_DDIM_OK;
}

/**
Flat Shading Start
@param[in]	unit_no : Unit number.
@retval		D_DDIM_OK				: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PRCH/M2P not running NG
@remarks	Flat Shading Compensation function is started(enable).
*/
INT32 Im_PRO_FSHD_Start( E_IM_PRO_UNIT_NUM unit_no )
{
	for( UCHAR loopcnt = 0; loopcnt < D_IM_PRO_FSHD_PRE_STARTED_STATUS_MAX; loopcnt++ ) {
		if(gIM_PRO_FSHD_Status_Tbl[unit_no].pre_started_status[loopcnt] != D_IM_PRO_COMMON_STATUS_NONE) {
			if (im_pro_comm_get_start_status(gIM_PRO_FSHD_Status_Tbl[unit_no].pre_started_status[loopcnt], 0) == FALSE){
				Ddim_Print(("I:Im_PRO_FSHD_Start. macro not running error. Pre-Started Macro status:(%u) \n", gIM_PRO_FSHD_Status_Tbl[unit_no].pre_started_status[loopcnt]));
				return D_IM_PRO_MACRO_BUSY_NG;
			}
		}
	}

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDTRG.bit.FSHDTRG = D_IM_PRO_TRG_START;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	im_pro_b2b_set_start_status(gIM_PRO_FSHD_Status_Tbl[unit_no].status, 0);

	return D_DDIM_OK;
}

/**
Flat Shading Stop
@param[in]	unit_no : Unit number.
@param[in]	force : force stop option
@retval		D_DDIM_OK				: Setting OK
@retval		D_IM_PRO_MACRO_BUSY_NG	: PRCH/M2P not stopped NG
@remarks	Flat Shading Compensation function is stopped(disable).
*/
INT32 Im_PRO_FSHD_Stop( E_IM_PRO_UNIT_NUM unit_no, UCHAR force )
{
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	if(force == 0) {
		IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDTRG.bit.FSHDTRG = D_IM_PRO_TRG_FRAME_STOP;		// stop
	}
	else {
		for( UCHAR loopcnt = 0; loopcnt < D_IM_PRO_FSHD_PRE_STARTED_STATUS_MAX; loopcnt++ ) {
			if(gIM_PRO_FSHD_Status_Tbl[unit_no].pre_started_status[loopcnt] != D_IM_PRO_COMMON_STATUS_NONE) {
				if (im_pro_comm_get_start_status(gIM_PRO_FSHD_Status_Tbl[unit_no].pre_started_status[loopcnt], 0) == TRUE){
					Ddim_Print(("I:Im_PRO_FSHD_Stop. macro not stop error. Pre-Stopped Macro status:(%u) \n", gIM_PRO_FSHD_Status_Tbl[unit_no].pre_started_status[loopcnt]));
					return D_IM_PRO_MACRO_BUSY_NG;
				}
			}
		}
		IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDTRG.bit.FSHDTRG = D_IM_PRO_TRG_FORCE_STOP;		// force stop
	}
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	im_pro_b2b_set_stop_status(gIM_PRO_FSHD_Status_Tbl[unit_no].status, 0);

	return D_DDIM_OK;
}

/**
Flat Shading Control
@param[in]	unit_no : Unit number.
@param[in]	fshd_ctrl : Shading Control information
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
INT32 Im_PRO_FSHD_Ctrl( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_FSHD_COMMON_CTRL* fshd_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (fshd_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_FSHD_Ctrl error. fshd_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_FSULRR_MIN, D_IM_PRO_FSHD_FSULRR_MAX, fshd_ctrl->clip_r, "Im_PRO_FSHD_Ctrl : clip_r" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_FSULGR_MIN, D_IM_PRO_FSHD_FSULGR_MAX, fshd_ctrl->clip_gr, "Im_PRO_FSHD_Ctrl : clip_gr" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_FSULGB_MIN, D_IM_PRO_FSHD_FSULGB_MAX, fshd_ctrl->clip_gb, "Im_PRO_FSHD_Ctrl : clip_gb" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_FSULBB_MIN, D_IM_PRO_FSHD_FSULBB_MAX, fshd_ctrl->clip_b, "Im_PRO_FSHD_Ctrl : clip_b" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDCTL.bit.FSMODE		= fshd_ctrl->correction_mode;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDCTL.bit.FWMODE		= fshd_ctrl->correction_mode_awb;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDCTL.bit.FSHDPTMD		= fshd_ctrl->montage_mode;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSUL1.bit.FSULRR			= fshd_ctrl->clip_r;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSUL1.bit.FSULGR			= fshd_ctrl->clip_gr;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSUL2.bit.FSULGB			= fshd_ctrl->clip_gb;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSUL2.bit.FSULBB			= fshd_ctrl->clip_b;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
A setup of enable access to the built-in RAM of FSHD.
@param[in]	unit_no : Unit number.
@param[in]	paen_trg : RAM access control<br>
				 value range :[0:Access inhibit  1:Permissions]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FSHD_Set_PAEN( E_IM_PRO_UNIT_NUM unit_no, UCHAR paen_trg )
{
#ifdef CO_PARAM_CHECK
	if( ( paen_trg == 0 ) && ( IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDTRG.bit.FSHDTRG != D_IM_PRO_TRG_STATUS_STOPPED ) ) {
		Ddim_Assertion(("I:Im_PRO_FSHD_Set_PAEN. macro has not stopped error.\n"));
		return D_IM_PRO_MACRO_BUSY_NG;
	}
#endif
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDPAEN.bit.FSPAEN = paen_trg;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
A Setup is Flat Shading Area
@param[in]	unit_no : Unit number.
@param[in]	fshd_area : Area for Shading compensation See @ref T_IM_PRO_AREA_INFO<br>
 												value range  :pos_x[0 - 4095]<br>
 												target registor  :@@FSHDH<br>
 												value range  :pos_y[2 - 8960]<br>
 												target registor  :@@FSHDV<br>
 												value range  :width[4 - 2496] 2pixel boundary<br>
 												target registor  :@@FSHDHW<br>
 												value range  :lines[4 - 8960] 4pixel boundary<br>
 												target registor  :@@FSHDVW
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
INT32 Im_PRO_FSHD_Set_Area( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_AREA_INFO* fshd_area )
{
#ifdef CO_PARAM_CHECK
	if (fshd_area == NULL){
		Ddim_Assertion(("I:Im_PRO_FSHD_Set_Area error. fshd_area=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_FSHDH_MIN, D_IM_PRO_FSHD_FSHDH_MAX, fshd_area->pos_x, "Im_PRO_FSHD_Set_Area : pos_x" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_FSHDV_MIN, D_IM_PRO_FSHD_FSHDV_MAX, fshd_area->pos_y, "Im_PRO_FSHD_Set_Area : pos_y" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_FSHDHW_MIN, D_IM_PRO_FSHD_FSHDHW_MAX, fshd_area->width, "Im_PRO_FSHD_Set_Area : width" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_FSHDVW_MIN, D_IM_PRO_FSHD_FSHDVW_MAX, fshd_area->lines, "Im_PRO_FSHD_Set_Area : lines" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDH.bit.FSHDH	= fshd_area->pos_x;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDV.bit.FSHDV	= fshd_area->pos_y;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDHW.bit.FSHDHW	= fshd_area->width;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDVW.bit.FSHDVW	= fshd_area->lines;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Flat Shading (Frame Correction) Control 
@param[in]	unit_no : Unit number.
@param[in]	blend_ctrl : Frame correction blend ratio information
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
INT32 Im_PRO_FSHD_Set_Blend( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_FSHD_BLEND_CTRL* blend_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (blend_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_FSHD_Set_Blend error. blend_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDBC, union io_fshdbc, FSHDBC0, blend_ctrl->shading_blend_0 );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDBC, union io_fshdbc, FSHDBC1, blend_ctrl->shading_blend_1 );
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Flat Shading (Frame Correction) Control 
@param[in]	unit_no : Unit number.
@param[in]	fshd_ctrl : Frame Correction Control information
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
INT32 Im_PRO_FSHD_Set_Frame( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_FSHD_FRAME_CTRL* fshd_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (fshd_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_FSHD_Set_Frame error. fshd_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( im_pro_check_val_range( D_IM_PRO_FSHD_FSHDHWPH_MIN, D_IM_PRO_FSHD_FSHDHWPH_MAX, fshd_ctrl->montage_pos_h, "Im_PRO_FSHD_Set_Frame : montage_pos_h" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( (IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDCTL.bit.FSHDPTMD == 1) && (( fshd_ctrl->start_pos_h + fshd_ctrl->montage_pos_h - 2 ) % fshd_ctrl->horizontal_pixel != 0) ) {
		Ddim_Assertion(("I:Im_PRO_FSHD_Set_Frame parameter error. Please satisfy following relationship. start_pos_h + montage_pos_h == n * horizontal_pixel + 2\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_FSSUBPRH_MIN, D_IM_PRO_FSHD_FSSUBPRH_MAX, fshd_ctrl->frame_horizontal_size, "Im_PRO_FSHD_Set_Frame : frame_horizontal_size" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_FSSUBPRV_MIN, D_IM_PRO_FSHD_FSSUBPRV_MAX, fshd_ctrl->frame_vertical_size, "Im_PRO_FSHD_Set_Frame : frame_vertical_size" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_FSSUBSPH_MIN, D_IM_PRO_FSHD_FSSUBSPH_MAX, fshd_ctrl->horizontal_pixel, "Im_PRO_FSHD_Set_Frame : horizontal_pixel" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_FSSUBSPV_MIN, D_IM_PRO_FSHD_FSSUBSPV_MAX, fshd_ctrl->vertical_pixel, "Im_PRO_FSHD_Set_Frame : vertical_pixel" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_FSSUBSTH_MIN, D_IM_PRO_FSHD_FSSUBSTH_MAX, fshd_ctrl->start_pos_h, "Im_PRO_FSHD_Set_Frame : start_pos_h" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_FSSUBSTV_MIN, D_IM_PRO_FSHD_FSSUBSTV_MAX, fshd_ctrl->start_pos_v, "Im_PRO_FSHD_Set_Frame : start_pos_v" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_FSARR_MIN, D_IM_PRO_FSHD_FSARR_MAX, fshd_ctrl->magnification_r, "Im_PRO_FSHD_Set_Frame : magnification_r" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_FSAGR_MIN, D_IM_PRO_FSHD_FSAGR_MAX, fshd_ctrl->magnification_gr, "Im_PRO_FSHD_Set_Frame : magnification_gr" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_FSAGB_MIN, D_IM_PRO_FSHD_FSAGB_MAX, fshd_ctrl->magnification_gb, "Im_PRO_FSHD_Set_Frame : magnification_gb" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_FSABB_MIN, D_IM_PRO_FSHD_FSABB_MAX, fshd_ctrl->magnification_b, "Im_PRO_FSHD_Set_Frame : magnification_b" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSCTL.bit.FSFMT		= fshd_ctrl->fsctl;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSSUBPR.bit.FSSUBPRH	= fshd_ctrl->frame_horizontal_size;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSSUBPR.bit.FSSUBPRV	= fshd_ctrl->frame_vertical_size;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSSUBSP.bit.FSSUBSPH	= fshd_ctrl->horizontal_pixel;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSSUBSP.bit.FSSUBSPV	= fshd_ctrl->vertical_pixel;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSSUBDR.bit.FSSUBDRM	= fshd_ctrl->Inverse_base;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSSUBDR.bit.FSSUBDRE	= fshd_ctrl->Inverse_exponent;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDHWPH.bit.FSHDHWPH	= fshd_ctrl->montage_pos_h;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSSUBST.bit.FSSUBSTH	= fshd_ctrl->start_pos_h;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSSUBST.bit.FSSUBSTV	= fshd_ctrl->start_pos_v;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSA1.bit.FSARR		= fshd_ctrl->magnification_r;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSA1.bit.FSAGR		= fshd_ctrl->magnification_gr;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSA2.bit.FSAGB		= fshd_ctrl->magnification_gb;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSA2.bit.FSABB		= fshd_ctrl->magnification_b;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDHCCTL.bit.FSHCEN	= fshd_ctrl->sat_correction_enabled;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSSLP1.bit.FSSLPRR	= fshd_ctrl->sat_slope_gain[0];
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSSLP1.bit.FSSLPGR	= fshd_ctrl->sat_slope_gain[1];
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSSLP2.bit.FSSLPGB	= fshd_ctrl->sat_slope_gain[2];
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSSLP2.bit.FSSLPBB	= fshd_ctrl->sat_slope_gain[3];
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Flat Shading (Concentric Correction) Control 
@param[in]	unit_no : Unit number.
@param[in]	fshd_ctrl : Concentric Correction Control information
@retval		D_DDIM_OK						: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR		: Setting NG
*/
INT32 Im_PRO_FSHD_Set_Concentric( E_IM_PRO_UNIT_NUM unit_no, T_IM_PRO_FSHD_CONCENTRIC_CTRL* fshd_ctrl )
{
#ifdef CO_PARAM_CHECK
	if (fshd_ctrl == NULL){
		Ddim_Assertion(("I:Im_PRO_FSHD_Set_Concentric error. fshd_ctrl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_CSHDPARV_MIN, D_IM_PRO_FSHD_CSHDPARV_MAX, fshd_ctrl->interval_v, "Im_PRO_FSHD_Set_Concentric : interval_v" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_CSHDPARV_MIN, D_IM_PRO_FSHD_CSHDPARV_MAX, fshd_ctrl->interval_v, "Im_PRO_FSHD_Set_Concentric : interval_v" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_CSARR_MIN, D_IM_PRO_FSHD_CSARR_MAX, fshd_ctrl->magnification_r, "Im_PRO_FSHD_Set_Frame : magnification_r" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_CSAGR_MIN, D_IM_PRO_FSHD_CSAGR_MAX, fshd_ctrl->magnification_gr, "Im_PRO_FSHD_Set_Frame : magnification_gr" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_CSAGB_MIN, D_IM_PRO_FSHD_CSAGB_MAX, fshd_ctrl->magnification_gb, "Im_PRO_FSHD_Set_Frame : magnification_gb" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( im_pro_check_val_range( D_IM_PRO_FSHD_CSABB_MIN, D_IM_PRO_FSHD_CSABB_MAX, fshd_ctrl->magnification_b, "Im_PRO_FSHD_Set_Frame : magnification_b" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSCTL1.bit.CSSTBL			= fshd_ctrl->select_ram;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSCTL2.bit.CSFMT			= fshd_ctrl->select_format;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSCTL2.bit.CSHDLIR		= fshd_ctrl->interpolated_val;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSCTL2.bit.CSHDPARH		= fshd_ctrl->interval_h;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSCTL2.bit.CSHDPARV		= fshd_ctrl->interval_v;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSTBLS1.bit.CSTBLSRR		= fshd_ctrl->start_address_r;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSTBLS1.bit.CSTBLSGR		= fshd_ctrl->start_address_gr;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSTBLS2.bit.CSTBLSGB		= fshd_ctrl->start_address_gb;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSTBLS2.bit.CSTBLSBB		= fshd_ctrl->start_address_b;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSHDD.bit.CSHDDO			= fshd_ctrl->offset;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSHDD.bit.CSHDDM			= fshd_ctrl->clip_value;
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSHDOA, union io_cshdoa, CSHDOAH, fshd_ctrl->pos_h );
	im_pro_set_reg_signed( IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSHDOA, union io_cshdoa, CSHDOAV, fshd_ctrl->pos_v );
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSA1.bit.CSARR			= fshd_ctrl->magnification_r;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSA1.bit.CSAGR			= fshd_ctrl->magnification_gr;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSA2.bit.CSAGB			= fshd_ctrl->magnification_gb;
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSA2.bit.CSABB			= fshd_ctrl->magnification_b;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Flat Shading (Concentric Correction) Table Number Acquisition
@param[in]	unit_no : Unit number.
@retval		Concentric Correction Table Number.<br>
			value range :[0:RAM0 1:RAM1]:@@CSTBST
*/
E_IM_PRO_FSHD_LUT_SEL Im_PRO_FSHD_Get_Ctbl_Number( E_IM_PRO_UNIT_NUM unit_no )
{
	E_IM_PRO_FSHD_LUT_SEL ctbl_num;

	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	ctbl_num = IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.CSTBLST.bit.CSTBST;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return ctbl_num;
}

/**
Flat shading compensation table is registered.
@param[in]	unit_no : Unit number.
@param[in]	shd_tbl		: Shading table info
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
@note		None
*/
INT32 Im_PRO_FSHD_Set_Table( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHD_LUT_SEL tbl_sel, T_IM_PRO_FSHD_TABLE* shd_tbl )
{
#ifdef CO_PARAM_CHECK
	if (shd_tbl == NULL){
		Ddim_Assertion(("I:Im_PRO_FSHD_Set_Table error. shd_tbl=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	if( ( shd_tbl->pshd_tbl == NULL ) || ( shd_tbl->size == 0 ) ) {
		Ddim_Assertion(("I:Im_PRO_FSHD_Set_Table error. shd_tbl->pshd_tbl=%lx shd_tbl->size=%u\n", (ULONG)shd_tbl->pshd_tbl, shd_tbl->size));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	im_pro_on_hclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	im_pro_memcpy( (VOID*)&gIM_PRO_FSHD_Status_Tbl[unit_no].tbl_reg_ptr[tbl_sel].hword[0], shd_tbl->pshd_tbl, shd_tbl->size );
	im_pro_off_hclk( E_IM_PRO_UNIT_NUM_1, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
set the request issue interval when reading the coefficients from PRCH/M2P macro.
@param[in]	unit_no : Unit number.
@param[in]	req_interval_clk : RAM access control<br>
			value range :[4~1023]<br>
@retval		D_DDIM_OK					: Setting OK
@retval		D_IM_PRO_INPUT_PARAM_ERROR	: Setting NG
*/
INT32 Im_PRO_FSHD_Set_Coeff_Read_Line_Wait( E_IM_PRO_UNIT_NUM unit_no, USHORT req_interval_clk )
{
#ifdef CO_PARAM_CHECK
	if( im_pro_check_val_range( D_IM_PRO_FSHD_FSWRIC_MIN, D_IM_PRO_FSHD_FSWRIC_MAX, req_interval_clk, "Im_PRO_FSHD_Set_Coeff_Read_Line_Wait : req_interval_clk" ) == FALSE ) {
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
	else if( req_interval_clk >= ((IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSHDHW.bit.FSHDHW - (4*IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSSUBPR.bit.FSSUBPRH))-10)/3 ) {
		Ddim_Assertion(("I:Im_PRO_FSHDL_Set_Coeff_Read_Line_Wait. parameter error. Please satisfy following relationship. (req_interval_clk  < ((FSHDHW-(4*FSSUBPRH)) -10)/3) (%u)\n", req_interval_clk));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif
	im_pro_on_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Start_Clock
	IO_PRO.IMG_PIPE[unit_no].B2B.FSHD.FSWRIC.bit.FSWRIC			= req_interval_clk;
	im_pro_off_pclk( unit_no, E_IM_PRO_CLK_BLOCK_TYPE_B2B );	// Dd_Top_Stop_Clock

	return D_DDIM_OK;
}

/**
Get the top address of the address array of FSHD control.
@param[in]		unit_no : Unit number.
@param[out]		addr					: Top address of the address array of FSHD control.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
INT32 Im_PRO_Get_RdmaAddr_FSHD_Cntl( E_IM_PRO_UNIT_NUM unit_no, const T_IM_PRO_RDMA_FSHD_ADDR** addr )
{
#ifdef CO_PARAM_CHECK
	if( addr == NULL ) {
		Ddim_Assertion(("I:Im_PRO_Get_RdmaAddr_FSHD_Cntl. error. addr=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	*addr = &gIM_PRO_FSHD_Addr[unit_no];

	return D_DDIM_OK;
}

/**
Get the top address of the address array of FSHD control.
@param[in]		unit_no : Unit number.
@param[out]		addr					: Top address of the address array of FSHD Concentric Circles Correction table.
@retval			D_DDIM_OK				: success.
@retval			D_IM_B2R_PARAM_ERROR	: parameter error.
*/
INT32 Im_PRO_Get_RdmaAddr_FSHD_Ctbl( E_IM_PRO_UNIT_NUM unit_no, E_IM_PRO_FSHD_LUT_SEL tbl_sel, const T_IM_PRO_RDMA_FSHD_CTBL_ADDR** addr )
{
#ifdef CO_PARAM_CHECK
	if( addr == NULL ) {
		Ddim_Assertion(("I:Im_PRO_Get_RdmaAddr_FSHD_Ctbl. error. addr=NULL\n"));
		return D_IM_PRO_INPUT_PARAM_ERROR;
	}
#endif

	*addr = &gIM_PRO_FSHD_Ctbl_Addr[unit_no][tbl_sel];

	return D_DDIM_OK;
}
