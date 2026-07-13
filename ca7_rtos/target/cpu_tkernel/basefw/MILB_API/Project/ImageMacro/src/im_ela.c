/**
 * @file		im_ela.c
 * @brief		JDSELA2D Driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include <string.h>
#include "dd_top.h"
#include "im_ela.h"
#include "jdsela.h"
#include "dd_arm.h"
#include "ddim_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
//#define CO_ELA_REG_TYPE_CHECK
//#define CO_ELA_DEBUG_PRINT		// Debug
#define CO_ELA_OPTIMUM_MEM			// Memory consumption optimization

#define D_IM_ELA_SYNC				(0)				/* Sync Processing  			*/
#define D_IM_ELA_ASYNC				(1)				/* Async Processing 			*/

// interrupt flag
#define D_IM_ELA_FLG_ELA			(0x00000001)	/**< Interrupt flag */

#define D_IM_ELA_STEP_S				(0xFF)			/**< Step = Synthesis */


#define D_IM_ELA_ELAINTF_AXWERF		(0x0020)		// __AXWERF
#define D_IM_ELA_ELAINTF_AXRERF		(0x0010)		// __AXRERF
#define D_IM_ELA_ELAINTF_ENWF		(0x0002)		// __ENWF

#define D_IM_ELA_EXECUTE_MIN		(1)				/* ELA macro execute cnt min	*/

/** Operation mode
*/
#define	D_IM_ELA_ELAACT_EXTRACT		(0x00)			/**< Operation mode : Extraction mode 		*/
#define	D_IM_ELA_ELAACT_COMBINE		(0x01)			/**< Operation mode : Combine mode 			*/

#define D_IM_ELA_SRAM_WAIT_USEC		(1)

//#define D_IM_ELA_WAIT_END_TIME		(D_DDIM_WAIT_END_TIME)
#define D_IM_ELA_WAIT_END_TIME		(1000)


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#define Im_ELA_Dsb()	Dd_ARM_Dsb_Pou()

#define im_ela_conv_stage_to_level(stage)	(stage + 1)

#define im_ela_wait_usec( usec )	Dd_ARM_Wait_ns( (usec * 1000) )
#ifdef CO_ELA_REG_TYPE_CHECK
#define im_ela_check_reg_type( dst, src )	(dst) = (src);
#else
#define im_ela_check_reg_type( dst, src )
#endif

#define im_ela_set_reg_signed( reg, type, member, val )		\
		{ \
			type work = { .word = 0UL }; \
			work.bit.member = (val); \
			(reg).bit.member = work.bit.member; \
			im_ela_check_reg_type( work, (reg) ); \
		}

#define im_ela_get_reg_signed( val, reg, type, member )		\
		{ \
			type work = { .word = 0UL }; \
			work.bit.member = -1L; \
			if( ((work.bit.member) == 0xFFFFFFFFUL) || ((reg.bit.member) <= ((work.bit.member) /2)) ) { \
				(val) = reg.bit.member; \
			} \
			else { \
				(val) = -(work.bit.member - reg.bit.member + 1); \
			} \
			im_ela_check_reg_type( (reg), work ); \
		}
#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
/* Nothing Special */

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
// ELA input bayer management table
typedef struct {
	UCHAR	eiw_dtype;			/* EIWTYP	: EIWch data type								@ELABYR	 */
	UCHAR	eiw_sft;			/* EIWSFT	: EIWch data type								@ELABYR	 */
	USHORT	vline;				/* EIILINE	: Input bayer data vertical line			 	@EIILINE */
	USHORT	hsize;				/* EIIHSIZ	: Input bayer data horizontal size			 	@EIIHSIZ */
} T_IM_ELA_BAYER_MNG;

// Parameter adjustment method table
typedef struct {
	E_IM_ELA_QELFLVL	ela_elf_level;
	E_IM_ELA_QELAMD		ela_density_mode[E_IM_ELA_ELF_LEVEL_MAX];
} T_IM_ELA_QELATMD;

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
static volatile UCHAR							gIM_ELA_Stage_Cnt = 0;
static volatile UCHAR							gIM_ELA_Step_Cnt = 0;
static volatile UCHAR							gIM_ELA_Next_Ctrl_Flag = 0;
static volatile T_IM_ELA_BAYER_MNG				gIM_ELA_Bayer;
static volatile T_IM_ELA_CURTAIL_BAYER_DATA		gIM_ELA_Curtail_Bayer[D_IM_ELA_STAGE_MAX];
static volatile T_IM_ELA_EXTRACT_NOISE_DATA		gIM_ELA_Extract_Noise[D_IM_ELA_STAGE_MAX];
static volatile T_IM_ELA_CTRL_COMMON			gIM_ELA_Ctrl_Common_Info;
static volatile T_IM_ELA_QELATMD				gIM_ELA_Qelatmd_Info;
volatile ULONG									gIM_ELA_Axi_Err = D_IM_ELA_NORMAL_END;
volatile T_IM_ELA_MULTIPLE_TIMES_INFO			gIM_ELA_Multiple_Exec_Info;
static volatile T_IM_ELA_CALLBACK				gIM_ELA_CallBack_Func = NULL;


#ifdef CO_ACT_ICLOCK
// ELACLK setting task counter (0~255)
volatile UCHAR gIM_ELA_CLK_Counter	= 0;
// ICLK_ELA setting task counter (0~255)
volatile UCHAR gIM_ELA_ICLK_Counter	= 0;
#endif //CO_ACT_ICLOCK
#ifdef CO_ACT_PCLOCK
// PCLK_ELA setting task counter (0~255)
volatile UCHAR gIM_ELA_PCLK_Counter	= 0;
#endif //CO_ACT_PCLOCK

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
static VOID im_ela_on_pclk( VOID );
static VOID im_ela_off_pclk( VOID );
static VOID im_ela_on_iclk( VOID );
static VOID im_ela_off_iclk( VOID );
static VOID im_ela_on_clk( VOID );
static VOID im_ela_off_clk( VOID );
static VOID im_ela_calc_curtail_bayer_data( UINT32 out_bayer_addr );
static VOID im_ela_calc_extract_noise_data( UINT32 extract_noise_addr, UINT32 noise_suppress_addr );
static INT32 im_ela_start_core( UINT32 sync );
static INT32 im_ela_start_next( VOID );
static VOID im_ela_ctrl_next_register( VOID );
static VOID im_ela_ctrl_next_eirch_register( VOID );
static VOID im_ela_ctrl_eirch_register( T_IM_ELA_CTRL_EIRCH ctrl_eirch );
static VOID im_ela_ctrl_eiwch_register( VOID );
static VOID im_ela_ctrl_enwch_register( VOID );
static VOID im_ela_ctrl_enrech_register( VOID );
static VOID im_ela_ctrl_enrach_enwmch_register( VOID );
static VOID im_ela_ctrl_noise_limit_register( const T_IM_ELA_CTRL_LIMIT* ctrl_noise_limit );
static VOID im_ela_ctrl_core_register( const T_IM_ELA_CTRL_CORE* const ctrl_ela_core );

#ifdef CO_ELA_DEBUG_PRINT	// Debug
static VOID im_ela_qelatmd_info_print( VOID );
static VOID im_ela_ctrl_reg_print( VOID );
#endif

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief		ELA PCLK change to ON
 * @param[in]   None
 * @param[out]  None
 * @return		None
 * @note		None
 */
static VOID im_ela_on_pclk( VOID )
{
#ifdef CO_ACT_PCLOCK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_ELA_PCLK_Counter, &Dd_Top_Get_CLKSTOP6(), ~D_DD_TOP_ELAAP_BIT );
#endif	// CO_ACT_PCLOCK
}

/**
 * @brief		ELA PCLK change to OFF
 * @param[in]   None
 * @param[out]  None
 * @return		None
 * @note		None
 */
static VOID im_ela_off_pclk( VOID )
{
#ifdef CO_ACT_PCLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_ELA_PCLK_Counter, &Dd_Top_Get_CLKSTOP6(), D_DD_TOP_ELAAP_BIT );
#endif	// CO_ACT_PCLOCK
}

/**
 * @brief		ELA ICLK change to ON
 * @param[in]   None
 * @param[out]  None
 * @return		None
 * @note		None
 */
static VOID im_ela_on_iclk( VOID )
{
#ifdef CO_ACT_ICLOCK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_ELA_ICLK_Counter, &Dd_Top_Get_CLKSTOP6(), ~D_DD_TOP_ELAAX_BIT );
#endif	// CO_ACT_ICLOCK
}

/**
 * @brief		ELA ICLK change to OFF
 * @param[in]   None
 * @param[out]  None
 * @return		None
 * @note		None
 */
static VOID im_ela_off_iclk( VOID )
{
#ifdef CO_ACT_ICLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_ELA_ICLK_Counter, &Dd_Top_Get_CLKSTOP6(), D_DD_TOP_ELAAX_BIT );
#endif	// CO_ACT_ICLOCK
}

/**
 * @brief		ELA CLK change to ON
 * @param[in]   None
 * @param[out]  None
 * @return		None
 * @note		None
 */
static VOID im_ela_on_clk( VOID )
{
#ifdef CO_ACT_CLOCK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_ELA_CLK_Counter, &Dd_Top_Get_CLKSTOP6(), ~D_DD_TOP_ELACK_BIT );
#endif	// CO_ACT_CLOCK
}

/**
 * @brief		ELA CLK change to OFF
 * @param[in]   None
 * @param[out]  None
 * @return		None
 * @note		None
 */
static VOID im_ela_off_clk( VOID )
{
#ifdef CO_ACT_CLOCK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_ELA_CLK_Counter, &Dd_Top_Get_CLKSTOP6(), D_DD_TOP_ELACK_BIT );
#endif	// CO_ACT_CLOCK
}

//---------------------------- driver section ----------------------------
/**
 * @brief		calculate curtail bayer data.
 * @param[in]	UINT32 out_bayer_addr
 * @return		VOID
 * @note		None
 * @attention	None
 */
static VOID im_ela_calc_curtail_bayer_data( UINT32 out_bayer_addr )
{
	UCHAR	level = E_IM_ELA_ELF_LEVEL_1;
	UCHAR	stage = 0;
	UCHAR	step;
	UCHAR	step_max;
	UCHAR	pd;
	UCHAR	ph;
	UCHAR	pv;
	USHORT	padding_hpixel;		// Number of padding horizontal pixels
	USHORT	padding_vline;		// Number of padding vertical pixels
	UINT32	padding_addr = 0;	// Padding bayer data address
	UINT32	calc_gl_hbyte;		// Calculate global horizontal size
	USHORT	type;

	padding_hpixel	= gIM_ELA_Bayer.hsize;
	padding_vline	= gIM_ELA_Bayer.vline;

	if( gIM_ELA_Bayer.eiw_dtype == D_IM_ELA_DATATYP_8BIT ){
		type = 8;
	}
	else{
		type = 12;
	}

	for( level = E_IM_ELA_ELF_LEVEL_1; level <= gIM_ELA_Qelatmd_Info.ela_elf_level; level++ ){
		if( level == E_IM_ELA_ELF_LEVEL_1 ){
			pd = 16;
		}
		else{
			pd = 8;
		}

		if( (padding_hpixel % 4) == 0 ){
			ph = 2;
		}
		else{
			ph = 6 - (padding_hpixel % 4);
		}

		if( (padding_vline % 4) == 0 ){
			pv = 2;
		}
		else{
			pv = 6 - (padding_vline % 4);
		}

		padding_hpixel	= (padding_hpixel + pd + ph - 2) / 2;
		padding_vline	= (padding_vline + pd + pv - 2) / 2;

		gIM_ELA_Curtail_Bayer[stage].vline = padding_vline;
		gIM_ELA_Curtail_Bayer[stage].hsize = padding_hpixel;
		calc_gl_hbyte = padding_hpixel * type / 8;
#ifdef CO_IM_ELA_OUT_BAY_DEBUG
		if( gIM_ELA_Bayer.eiw_dtype == D_IM_ELA_EIODTYP_12PACK ){
			if( (calc_gl_hbyte % 64) != 0 ){
				// for display of dump bayer file easy on PC tool(for debug)
			 	calc_gl_hbyte = ((calc_gl_hbyte + 47) /(64*3)) *(64*3);
			}
		}
		else{
#endif //CO_IM_ELA_OUT_BAY_DEBUG
			if( (calc_gl_hbyte % 64) != 0 ){
			 	calc_gl_hbyte = calc_gl_hbyte + (64 - (calc_gl_hbyte % 64));
			}
#ifdef CO_IM_ELA_OUT_BAY_DEBUG
		}
#endif //CO_IM_ELA_OUT_BAY_DEBUG
	 	gIM_ELA_Curtail_Bayer[stage].gl_hbyte = calc_gl_hbyte;

		if( level == gIM_ELA_Qelatmd_Info.ela_elf_level ){
			break;
		}
		switch( gIM_ELA_Qelatmd_Info.ela_density_mode[level + 1] ){
			case E_IM_ELA_DENSITY_MODE_0:
			case E_IM_ELA_DENSITY_MODE_1:
				step_max = 1;
				break;
				
			case E_IM_ELA_DENSITY_MODE_2:
				step_max = 4;
				break;
				
//			case E_IM_ELA_DENSITY_MODE_3:
			default:
				step_max = 16;
				break;
		}
		for( step = 0; step < step_max; step ++ ){
#ifdef CO_ELA_OPTIMUM_MEM	//Memory consumption optimization
			if( (gIM_ELA_Qelatmd_Info.ela_elf_level <= E_IM_ELA_ELF_LEVEL_2) ||
			   (gIM_ELA_Qelatmd_Info.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] != E_IM_ELA_DENSITY_MODE_3 ) ){
				if( ((level == E_IM_ELA_ELF_LEVEL_1) && (step == 0)) ||
				   ((level == E_IM_ELA_ELF_LEVEL_3) && (step == 0)) ){
					padding_addr = out_bayer_addr;
				}
				else{
					if( step == 0 ){
						padding_addr += ((UINT32)gIM_ELA_Curtail_Bayer[stage - 1].gl_hbyte * (UINT32)gIM_ELA_Curtail_Bayer[stage - 1].vline);
					}
					else{
						padding_addr += ((UINT32)calc_gl_hbyte * (UINT32)padding_vline);
					}
				}
			}
			else{
				if( ((level == E_IM_ELA_ELF_LEVEL_1) && (step == 0)) ||
				   ((level == E_IM_ELA_ELF_LEVEL_2) && (step == 0)) ||
				   ((level == E_IM_ELA_ELF_LEVEL_4) && (step == 0)) ){
					padding_addr = out_bayer_addr;
				}
				else{
					padding_addr += ((UINT32)calc_gl_hbyte * (UINT32)padding_vline);
				}
			}
#else
			if( (level == E_IM_ELA_ELF_LEVEL_1) && (step == 0) ){
				padding_addr = out_bayer_addr;
			}
			else{
				if( step == 0 ){
					padding_addr += ((UINT32)gIM_ELA_Curtail_Bayer[stage - 1].gl_hbyte * (UINT32)gIM_ELA_Curtail_Bayer[stage - 1].vline);
				}
				else {
					padding_addr += ((UINT32)calc_gl_hbyte * (UINT32)padding_vline);
				}
			}
#endif
			if( (padding_addr % 64) == 0 ){
			 	gIM_ELA_Curtail_Bayer[stage].bayer_addr[step] = padding_addr;
			}
			else{
			 	padding_addr = padding_addr + (64 - (padding_addr % 64));
			 	gIM_ELA_Curtail_Bayer[stage].bayer_addr[step] = padding_addr;
			}
		}
		stage++;
	}
#ifdef CO_ELA_OPTIMUM_MEM	// Memory consumption optimization.
	if( (gIM_ELA_Qelatmd_Info.ela_elf_level <= E_IM_ELA_ELF_LEVEL_2) ||
	   (gIM_ELA_Qelatmd_Info.ela_density_mode[E_IM_ELA_ELF_LEVEL_3] != E_IM_ELA_DENSITY_MODE_3 ) ){
		// No processing.
	}
	else{
		padding_addr = (gIM_ELA_Curtail_Bayer[1].bayer_addr[15] - gIM_ELA_Curtail_Bayer[0].bayer_addr[3])
					   + ((UINT32)(gIM_ELA_Curtail_Bayer[1].gl_hbyte) * (UINT32)(gIM_ELA_Curtail_Bayer[1].vline));
		for( step = 0; step < 4; step ++ ){
			gIM_ELA_Curtail_Bayer[0].bayer_addr[step] += padding_addr;
		}
	}
#endif

}

/**
 * @brief		calculate curtail bayer data.
 * @param[in]	UINT32 out_bayer_addr
 * @param[in]	UINT32 noise_suppress_addr
 * @return		VOID
 * @note		None
 * @attention	None
 */
static VOID im_ela_calc_extract_noise_data( UINT32 extract_noise_addr, UINT32 noise_suppress_addr )
{
	UCHAR	level = E_IM_ELA_ELF_LEVEL_1;
	UCHAR	stage = 0;
	UCHAR 	step;
	UCHAR 	step_max;
	UCHAR 	pd;
	UCHAR 	rd;
	UCHAR 	ph;
	UCHAR 	pv;
	USHORT	padding_hpixel;		// Number of padding horizontal pixels
	USHORT	padding_vline;		// Number of padding vertical pixels
	UINT32	padding_addr;		// Padding bayer data address
	UINT32	calc_gl_hbyte;		// Calculate global horizontal size
	USHORT	in_bayer_hpixel;	// Number of input bayer horizontal pixels
	USHORT	in_bayer_vline;		// Number of input bayer vertical pixels

	padding_addr = extract_noise_addr;

	for( level = E_IM_ELA_ELF_LEVEL_1; level <= gIM_ELA_Qelatmd_Info.ela_elf_level; level++ ){
		if( level == E_IM_ELA_ELF_LEVEL_1 ){
			in_bayer_hpixel = gIM_ELA_Bayer.hsize;
			in_bayer_vline = gIM_ELA_Bayer.vline;
		}
		else{
			in_bayer_hpixel = gIM_ELA_Curtail_Bayer[stage - 1].hsize;
			in_bayer_vline = gIM_ELA_Curtail_Bayer[stage - 1].vline;
		}

		if( level == E_IM_ELA_ELF_LEVEL_1 ){
			pd = 16;
		}
		else{
			pd = 8;
		}
		if( gIM_ELA_Qelatmd_Info.ela_density_mode[level] == E_IM_ELA_DENSITY_MODE_0 ){
			rd = 2;
		}
		else{
			rd = 1;
		}

		if( (in_bayer_hpixel % 4) == 0 ){
			ph = 2;
		}
		else{
			ph = 6 - (in_bayer_hpixel % 4);
		}

		if( (in_bayer_vline % 4) == 0 ){
			pv = 2;
		}
		else{
			pv = 6 - (in_bayer_vline % 4);
		}

		padding_hpixel = (in_bayer_hpixel + pd + ph - 14) / rd;
		padding_vline  = (in_bayer_vline + pd + pv - 14) / rd;
		gIM_ELA_Extract_Noise[stage].hsize = padding_hpixel;
		gIM_ELA_Extract_Noise[stage].vline = padding_vline;
		if( (padding_hpixel % 64) != 0 ){
		 	calc_gl_hbyte = padding_hpixel + (64 - (padding_hpixel % 64));
		}
		else{
			calc_gl_hbyte = padding_hpixel;
		}
	 	gIM_ELA_Extract_Noise[stage].gl_hbyte = calc_gl_hbyte;

		switch( gIM_ELA_Qelatmd_Info.ela_density_mode[level] ){
			case E_IM_ELA_DENSITY_MODE_0:
			case E_IM_ELA_DENSITY_MODE_1:
				step_max = 1;
				break;
				
			case E_IM_ELA_DENSITY_MODE_2:
				step_max = 4;
				break;
				
//			case E_IM_ELA_DENSITY_MODE_3:
			default:
				step_max = 16;
				break;
		}
		for( step = 0; step < step_max; step ++ ){
#ifdef CO_ELA_OPTIMUM_MEM	// Memory consumption optimization.
			if( step == 0 && level == E_IM_ELA_ELF_LEVEL_1 ){
				padding_addr = noise_suppress_addr;
			}
			else if( step == 0 && level >= E_IM_ELA_ELF_LEVEL_2 ){
				padding_addr = extract_noise_addr;
			}
			else{
				padding_addr += (calc_gl_hbyte * padding_vline * 8 / 8);
			}
#else
			if( step == 0 && level == E_IM_ELA_ELF_LEVEL_1 ){
				padding_addr = noise_suppress_addr;
			}
			else if( step == 0 && level == E_IM_ELA_ELF_LEVEL_2 ){
				padding_addr = extract_noise_addr;
			}
			else{
				padding_addr += (calc_gl_hbyte * padding_vline * 8 / 8);
			}
#endif
			if( (padding_addr % 64) == 0 ){
			 	gIM_ELA_Extract_Noise[stage].noise_data_addr[step] = padding_addr;
			}
			else {
			 	padding_addr = padding_addr + (64 - (padding_addr % 64));
			 	gIM_ELA_Extract_Noise[stage].noise_data_addr[step] = padding_addr;
			}
		}
		stage++;
	}
}

/**
 * @brief		Start the operaion of ELA
 * @param[in]	D_IM_ELA_SYNC, D_IM_ELA_ASYNC
 * @return		INT32 D_DDIM_OK / D_IM_ELA_TIMEOUT / D_IM_ELA_NG / D_IM_ELA_MACRO_BUSY
 * @note		None
 * @attention	None
 */
static INT32 im_ela_start_core( UINT32 sync )
{
	DDIM_USER_FLGPTN flg_ptn;
	DDIM_USER_ER ercd;
	INT32 ret = D_DDIM_OK;

	im_ela_on_pclk();

	if( IO_ELA.ELATRG.bit.ELATRG == 0x3 ){
		Ddim_Print(("im_ela_start_core() macro busy.\n"));

		im_ela_off_pclk();

		return D_IM_ELA_MACRO_BUSY;
	}

	ercd = DDIM_User_Clr_Flg( FID_IM_ELA, ~D_IM_ELA_FLG_ELA );
	if( ercd != D_DDIM_USER_E_OK ){
		Ddim_Print(("im_ela_start_core() system error. ercd = %d\n", ercd));

		im_ela_off_pclk();

		return D_IM_ELA_NG;
	}

	im_ela_on_iclk();
	im_ela_on_clk();

	// Enable SRAM ACCESS
	IO_ELA.SRAMACT.bit.SRAMACT = 1;

	im_ela_wait_usec( D_IM_ELA_SRAM_WAIT_USEC );

	IO_ELA.ELATRG.bit.ELATRG = 1;

#ifdef CO_ELA_DEBUG_PRINT	// Debug
	im_ela_ctrl_reg_print();
#endif

	if( sync == D_IM_ELA_SYNC ){
		/* ELA wait end */
		do{
			ercd = DDIM_User_Twai_Flg( FID_IM_ELA, D_IM_ELA_FLG_ELA, D_DDIM_USER_TWF_ORW, &flg_ptn, D_IM_ELA_WAIT_END_TIME );
			if( ercd == D_DDIM_USER_E_TMOUT ){
				Ddim_Print(("im_ela_start_core() time out. ercd = %d\n", ercd));
				ret = D_IM_ELA_TIMEOUT;
				break;
			}
			else if( ercd != D_DDIM_USER_E_OK ){
				Ddim_Print(("im_ela_start_core() system error. ercd = %d\n", ercd));
				ret = D_IM_ELA_NG;
				break;
			}
			else{
				(VOID)DDIM_User_Clr_Flg( FID_IM_ELA, ~D_IM_ELA_FLG_ELA );
			}
		}
		while( im_ela_start_next() != D_IM_ELA_PROCESS_END );

#ifdef CO_ELA_DEBUG_PRINT	// Debug
		Ddim_Print(("im_ela_start_core() Normal End. ercd = %d\n", ercd));
#endif

		if( IO_ELA.ELATRG.bit.ELATRG == 0x3 ){
			/* ELA stop */
			IO_ELA.ELATRG.bit.ELATRG = 0;
		}

		// Disable SRAM ACCESS
		IO_ELA.SRAMACT.bit.SRAMACT = 0;

		im_ela_off_pclk();
		im_ela_off_iclk();
		im_ela_off_clk();
	}
	else{	// sync == D_IM_ELA_ASYNC
		im_ela_off_pclk();
	}

	return ret;
}

/**
 * @brief		Start the next operaion of ELA
 * @param[in]	None
 * @return		INT32 D_DDIM_OK / D_IM_ELA_PROCESS_END
 * @note		None
 * @attention	None
 */

static INT32 im_ela_start_next( VOID )
{
	static UCHAR stop_wait_cnt;

	if( gIM_ELA_Axi_Err != D_IM_ELA_NORMAL_END ){
		if( IO_ELA.ELATRG.bit.ELATRG == 0x3 ){
			/* ELA stop */
			IO_ELA.ELATRG.bit.ELATRG = 0;
		}
		Ddim_Print(("im_ela_start_next() AXI error. ercd = %lu\n", gIM_ELA_Axi_Err));
		Ddim_Print(("RRESP[0] = %d\n", IO_ELA.EARAXSTS.bit.RRESP_0));
		Ddim_Print(("RRESP[1] = %d\n", IO_ELA.EARAXSTS.bit.RRESP_1));
		Ddim_Print(("RRESP[2] = %d\n", IO_ELA.EARAXSTS.bit.RRESP_2));
		Ddim_Print(("BRESP[0] = %d\n", IO_ELA.EAWAXSTS.bit.BRESP_0));
		Ddim_Print(("BRESP[1] = %d\n", IO_ELA.EAWAXSTS.bit.BRESP_1));
		Ddim_Print(("BRESP[2] = %d\n", IO_ELA.EAWAXSTS.bit.BRESP_2));

		return D_IM_ELA_PROCESS_END;
	}
	else{
		stop_wait_cnt = 0;
		while( IO_ELA.ELATRG.bit.ELATRG == 3 ){
			DDIM_User_Dly_Tsk( 1 );

			stop_wait_cnt++;

			if( stop_wait_cnt > 100 ){
				Ddim_Print(("im_ela_start_next() Time Out Error. ELATRG = %d\n", IO_ELA.ELATRG.bit.ELATRG ));
				return D_IM_ELA_PROCESS_END;
			}
		}

		if( gIM_ELA_Qelatmd_Info.ela_elf_level != E_IM_ELA_ELF_LEVEL_1 ){
			if( gIM_ELA_Stage_Cnt == 0 ){
				gIM_ELA_Stage_Cnt++;
				gIM_ELA_Step_Cnt = 0;
			}
			else{
				if( gIM_ELA_Step_Cnt != D_IM_ELA_STEP_S ){
					if( gIM_ELA_Qelatmd_Info.ela_density_mode[im_ela_conv_stage_to_level(gIM_ELA_Stage_Cnt)] == E_IM_ELA_DENSITY_MODE_2 ){
						if( gIM_ELA_Step_Cnt < 3 ){
							gIM_ELA_Step_Cnt++;
						}
						else if( gIM_ELA_Step_Cnt == 3 ){
							gIM_ELA_Step_Cnt = D_IM_ELA_STEP_S;
						}
						else{
							gIM_ELA_Stage_Cnt++;
							gIM_ELA_Step_Cnt = 0;
						}
					}
					else if( gIM_ELA_Qelatmd_Info.ela_density_mode[im_ela_conv_stage_to_level(gIM_ELA_Stage_Cnt)] == E_IM_ELA_DENSITY_MODE_3 ){
						if( gIM_ELA_Step_Cnt < 15 ){
							gIM_ELA_Step_Cnt++;
						}
						else if( gIM_ELA_Step_Cnt == 15 ){
							gIM_ELA_Step_Cnt = D_IM_ELA_STEP_S;
						}
						else{
							gIM_ELA_Stage_Cnt++;
							gIM_ELA_Step_Cnt = 0;
						}
					}
					else{
						gIM_ELA_Step_Cnt = D_IM_ELA_STEP_S;
					}
				}
				else{
					gIM_ELA_Stage_Cnt++;
					gIM_ELA_Step_Cnt = 0;
				}
			}
#ifdef CO_ELA_DEBUG_PRINT	// Debug
			Ddim_Print(("im_ela_start_next() : Next Stage = %d, Next Step = %d\n", gIM_ELA_Stage_Cnt, gIM_ELA_Step_Cnt));
#endif

			if( (im_ela_conv_stage_to_level(gIM_ELA_Stage_Cnt) > gIM_ELA_Qelatmd_Info.ela_elf_level) ||
			   ((gIM_ELA_Step_Cnt == 0) && (gIM_ELA_Next_Ctrl_Flag == D_IM_ELA_ENABLE)) ){
				return D_IM_ELA_PROCESS_END;
			}
			else{
				/* Common register & EIIch register & ENIch/ENOch register setting */
				im_ela_ctrl_next_register();

#ifdef CO_ELA_DEBUG_PRINT	// Debug
				Ddim_Print(("im_ela_start_next() : Start the next process\n"));
				im_ela_ctrl_reg_print();
#endif
				/* ELA start */
				IO_ELA.ELATRG.bit.ELATRG = 1;
			}
		}
		else{
#ifdef CO_ELA_DEBUG_PRINT	// Debug
			Ddim_Print(("im_ela_start_next(): Process End.(E_IM_ELA_ELF_LEVEL_1)\n"));
#endif
			return D_IM_ELA_PROCESS_END;
		}
	}

	return D_DDIM_OK;
}

/**
 * @brief		Common register & EIRch register & ENWch/ENWch register setting
 * @param[in]	None
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID im_ela_ctrl_next_register( VOID )
{
	/* common register setting */
	if( gIM_ELA_Step_Cnt == 0 ){
		IO_ELA.ELACTL.bit.ELACNT += 1;													// ELA count
	}
	IO_ELA.ELACTL.bit.ELAMD = gIM_ELA_Qelatmd_Info.ela_density_mode[im_ela_conv_stage_to_level(gIM_ELA_Stage_Cnt)];	// ELA density mode

	if( gIM_ELA_Step_Cnt == D_IM_ELA_STEP_S ){
		IO_ELA.ELACTL.bit.ELAACT = D_IM_ELA_ELAACT_COMBINE;								// Synthesis mode
		im_ela_ctrl_enrech_register();													// ENRAch/ENWMch register setting
		im_ela_ctrl_enrach_enwmch_register();
	}
	else{
		IO_ELA.ELACTL.bit.ELAACT = D_IM_ELA_ELAACT_EXTRACT;								// Synthesis mode
		IO_ELA.ELABYR.bit.EIRTYP = gIM_ELA_Bayer.eiw_dtype;								// Input data type
		IO_ELA.ELABYR.bit.EIRSFT = gIM_ELA_Bayer.eiw_sft;								// Input bit shift mode

		if( gIM_ELA_Qelatmd_Info.ela_elf_level == im_ela_conv_stage_to_level(gIM_ELA_Stage_Cnt) ){
			IO_ELA.ELABYR.bit.EIWOUT = D_IM_ELA_EIWOUT_NONE;
		}
		else{
			switch( gIM_ELA_Qelatmd_Info.ela_density_mode[im_ela_conv_stage_to_level(gIM_ELA_Stage_Cnt) + 1] ){
				case E_IM_ELA_DENSITY_MODE_0:
				case E_IM_ELA_DENSITY_MODE_1:
					if( gIM_ELA_Step_Cnt == 0 ){
						IO_ELA.ELABYR.bit.EIWOUT = D_IM_ELA_EIWOUT_1;
					}
					else{
						IO_ELA.ELABYR.bit.EIWOUT = D_IM_ELA_EIWOUT_NONE;
					}
					break;
				case E_IM_ELA_DENSITY_MODE_2:
					if( gIM_ELA_Step_Cnt == 0 ){
						IO_ELA.ELABYR.bit.EIWOUT = D_IM_ELA_EIWOUT_4;
					}
					else{
						IO_ELA.ELABYR.bit.EIWOUT = D_IM_ELA_EIWOUT_NONE;
					}
					break;
//				case E_IM_ELA_DENSITY_MODE_3:
				default:
					if( gIM_ELA_Step_Cnt < 4 ){
						IO_ELA.ELABYR.bit.EIWOUT = D_IM_ELA_EIWOUT_4;
					}
					else{
						IO_ELA.ELABYR.bit.EIWOUT = D_IM_ELA_EIWOUT_NONE;
					}
					break;
			}
		}

		IO_ELA.ELABYR.bit.DKNEN = 0;	// Deknee disable
		IO_ELA.ELABYR.bit.DKNACT = 0;
		IO_ELA.ELABYR.bit.EOBAEN = 0;	// "OB correction"/"Optical zero shift" disable

		/* for multiple times execution parameter */
		if( gIM_ELA_Step_Cnt == 0 ){
			if( gIM_ELA_Multiple_Exec_Info.core[gIM_ELA_Stage_Cnt] != NULL ){
				im_ela_ctrl_core_register( gIM_ELA_Multiple_Exec_Info.core[gIM_ELA_Stage_Cnt] );
			}
		}

		// EIRch register setting
		im_ela_ctrl_next_eirch_register();
		// EIWch register setting
		im_ela_ctrl_eiwch_register();
		// ENWch register setting
		im_ela_ctrl_enwch_register();
	}
}

/**
 * @brief		EIRch register next setting
 * @param[in]	None
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID im_ela_ctrl_next_eirch_register( VOID )
{
	T_IM_ELA_CTRL_EIRCH ctrl_eirch;

	// The EIRA is populated with EIWA data from the previous stage.
	ctrl_eirch.in_bayer_top_offset = 0;
	ctrl_eirch.in_bayer_total_hsize = gIM_ELA_Curtail_Bayer[gIM_ELA_Stage_Cnt - 1].gl_hbyte;
	ctrl_eirch.in_bayer_hsize = gIM_ELA_Curtail_Bayer[gIM_ELA_Stage_Cnt - 1].hsize;
	ctrl_eirch.in_bayer_vline = gIM_ELA_Curtail_Bayer[gIM_ELA_Stage_Cnt - 1].vline;
	ctrl_eirch.in_bayer_top_addr = gIM_ELA_Curtail_Bayer[gIM_ELA_Stage_Cnt - 1].bayer_addr[gIM_ELA_Step_Cnt];

	im_ela_ctrl_eirch_register( ctrl_eirch );
}

/**
 * @brief		EIRch register setting
 * @param[in]	T_IM_ELA_CTRL_EIRCH ctrl_eirch
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID im_ela_ctrl_eirch_register( T_IM_ELA_CTRL_EIRCH ctrl_eirch )
{
	USHORT	hsize;
	USHORT	vline;

	const USHORT	hsize_max_cnt0	= 15360;
	const USHORT	hsize_min_cnt0	= 320;
	const USHORT	hsize_min		= 64;
	const USHORT	vline_max_cnt0	= 8960;
	const USHORT	vline_min_cnt0	= 240;
	const USHORT	vline_min		= 48;

	hsize = ctrl_eirch.in_bayer_hsize;
	vline = ctrl_eirch.in_bayer_vline;

	IO_ELA.EIRA.word = ctrl_eirch.in_bayer_top_addr;				// Input bayer data top address
	IO_ELA.EIROFS.bit.EIROFS = ctrl_eirch.in_bayer_top_offset;		// Input bayer data Top area bit offset
	IO_ELA.EIRDEF.bit.EIRDEF = ctrl_eirch.in_bayer_total_hsize;		// Input bayer data total horizontal size

	if( gIM_ELA_Stage_Cnt == 0 ){
		if( hsize > hsize_max_cnt0 ){
			hsize = hsize_max_cnt0;
		}
		else if( hsize < hsize_min_cnt0 ){
			hsize = hsize_min_cnt0;
		}
		else{
			;	// DO NOTHING
		}

		if( vline > vline_max_cnt0 ){
			vline = vline_max_cnt0;
		}
		else if( vline < vline_min_cnt0 ){
			vline = vline_min_cnt0;
		}
		else{
			;	// DO NOTHING
		}
	}
	else{
		if( hsize < hsize_min ){
			hsize = hsize_min;
		}

		if( vline < vline_min ){
			vline = vline_min;
		}
	}

	IO_ELA.EIRHSIZ.bit.EIRHSIZ = hsize;								// Input bayer data horizontal size
	IO_ELA.EIRVSIZ.bit.EIRVSIZ = vline;								// Input bayer data horizontal size

	gIM_ELA_Bayer.hsize = hsize;
	gIM_ELA_Bayer.vline = vline;
}

/**
 * @brief		EIWch register setting
 * @param		None
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID im_ela_ctrl_eiwch_register( VOID )
{
	UINT32	loop_cnt;
	UINT32	loop_start;
	UINT32	loop_end;
	UINT32	gl_hbyte;

	if( gIM_ELA_Qelatmd_Info.ela_elf_level == E_IM_ELA_ELF_LEVEL_1 ||
	   gIM_ELA_Qelatmd_Info.ela_elf_level == im_ela_conv_stage_to_level(gIM_ELA_Stage_Cnt) ){
		return;
	}
	if( gIM_ELA_Step_Cnt >= 4 ){
		return;
	}

	switch( gIM_ELA_Qelatmd_Info.ela_density_mode[im_ela_conv_stage_to_level(gIM_ELA_Stage_Cnt) + 1] ){
		case E_IM_ELA_DENSITY_MODE_0:
		case E_IM_ELA_DENSITY_MODE_1:
			if( gIM_ELA_Step_Cnt >= 1 ){
				return;
			}
			loop_end = 1;
			break;
		case E_IM_ELA_DENSITY_MODE_2:
			if( gIM_ELA_Step_Cnt >= 1 ){
				return;
			}
			loop_end = 4;
			break;
//		case E_IM_ELA_DENSITY_MODE_3:
		default:
			if( gIM_ELA_Step_Cnt >= 4 ){
				return;
			}
			loop_end = 4;
			break;
	}
	loop_start = 4 * gIM_ELA_Step_Cnt;

	/* EIOch register setting */
	for( loop_cnt = 0; loop_cnt < loop_end; loop_cnt++ ){
		IO_ELA.EIWA.word[loop_cnt] = gIM_ELA_Curtail_Bayer[gIM_ELA_Stage_Cnt].bayer_addr[loop_start + loop_cnt];	// Output bayer data top address
	}

	/* calc. and set output bayer number of horizontal all bytes */
	/*
	 *  refer JDSELA1A section 5.5
	 */
	 gl_hbyte = gIM_ELA_Curtail_Bayer[gIM_ELA_Stage_Cnt].gl_hbyte;
#ifdef CO_IM_ELA_OUT_BAY_DEBUG
	if( gIM_ELA_Bayer.eiw_dtype == D_IM_ELA_EIODTYP_12PACK ){
		if( (gl_hbyte % 64) == 0 ){
			IO_ELA.EIWDEF.word = gl_hbyte;
		}
		else{
			// for display of dump bayer file easy on PC tool(for debug)
			IO_ELA.EIWDEF.word = ((gl_hbyte + 47) / (64 * 3)) * (64 * 3);
		}
	}
	else{
#endif //CO_IM_ELA_OUT_BAY_DEBUG
		if( (gl_hbyte % 64) == 0 ){
			IO_ELA.EIWDEF.word = gl_hbyte;
		}
		else{
			IO_ELA.EIWDEF.word = gl_hbyte + (64 - (gl_hbyte % 64));
		}
#ifdef CO_IM_ELA_OUT_BAY_DEBUG
	}
#endif //CO_IM_ELA_OUT_BAY_DEBUG
}

/**
 * @brief		ENWch register setting
 * @param[in]	None
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID im_ela_ctrl_enwch_register( VOID )
{
	IO_ELA.ENWA.word = gIM_ELA_Extract_Noise[gIM_ELA_Stage_Cnt].noise_data_addr[gIM_ELA_Step_Cnt];	// Output noise data data top address
	IO_ELA.ENWDEF.word = gIM_ELA_Extract_Noise[gIM_ELA_Stage_Cnt].gl_hbyte;							// Output noise data data total horizontal size
}

/**
 * @brief		ENREch register setting
 * @param[in]	None
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID im_ela_ctrl_enrech_register( VOID )
{
	UINT32 i;

	for( i =0; i < D_IM_ELA_ENRECH_ADDR_MAX; i++ ){
		IO_ELA.ENRA.word[i] = gIM_ELA_Extract_Noise[gIM_ELA_Stage_Cnt].noise_data_addr[i];
	}
	IO_ELA.ENRHSIZ.bit.ENRHSIZ = gIM_ELA_Extract_Noise[gIM_ELA_Stage_Cnt].hsize;
	IO_ELA.ENRVSIZ.bit.ENRVSIZ = gIM_ELA_Extract_Noise[gIM_ELA_Stage_Cnt].vline;
}

/**
 * @brief		ENRAch/ENWMch register setting
 * @param[in]	None
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID im_ela_ctrl_enrach_enwmch_register( VOID )
{
	IO_ELA.ENSA.word = gIM_ELA_Extract_Noise[0].noise_data_addr[0];		// Input/Output synthetic noise data top address

	IO_ELA.ENSDEF.word = gIM_ELA_Extract_Noise[0].gl_hbyte;				// Input/Output synthetic noise data total horizontal size
	IO_ELA.ENSHSIZ.word = gIM_ELA_Extract_Noise[0].hsize;				// Input/Output synthetic noise data horizontal size
	IO_ELA.ENSVSIZ.word = gIM_ELA_Extract_Noise[0].vline;				// Input/Output synthetic noise data vertical line
}

/**
 * @brief		ELA core register setting
 * @param[in]	T_IM_ELA_CTRL_NOISE_LIMIT ctrl_noise_limit
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID im_ela_ctrl_noise_limit_register( const T_IM_ELA_CTRL_LIMIT* ctrl_noise_limit )
{
	USHORT	plus_limit_work;
	USHORT	minus_limit_work;
	USHORT	limit_chk_val;

	plus_limit_work = ctrl_noise_limit->noise_plus_limit;
	minus_limit_work = ctrl_noise_limit->noise_minus_limit;

	if( gIM_ELA_Ctrl_Common_Info.noise_data_compress_enable == D_IM_ELA_DISABLE ){
		if( gIM_ELA_Ctrl_Common_Info.noise_data_precision == D_IM_ELA_NSLRES_PRECISION_1 ){
			limit_chk_val = 127;
		}
		else if( gIM_ELA_Ctrl_Common_Info.noise_data_precision == D_IM_ELA_NSLRES_PRECISION_2 ){
			limit_chk_val = 254;
		}
		else if( gIM_ELA_Ctrl_Common_Info.noise_data_precision == D_IM_ELA_NSLRES_PRECISION_3 ){
			limit_chk_val = 509;
		}
		else{
			limit_chk_val = 1019;
		}
	}
	else{
		if( gIM_ELA_Ctrl_Common_Info.noise_data_precision == D_IM_ELA_NSLRES_PRECISION_1 ){
			limit_chk_val = 315;
		}
		else if( gIM_ELA_Ctrl_Common_Info.noise_data_precision == D_IM_ELA_NSLRES_PRECISION_2 ){
			limit_chk_val = 631;
		}
		else{
			limit_chk_val = 1023;		// Don't Care (0x3FF)
		}
	}

	if( plus_limit_work > limit_chk_val ){
		plus_limit_work = limit_chk_val;
	}
	if( minus_limit_work > limit_chk_val ){
		minus_limit_work = limit_chk_val;
	}

	IO_ELA.NSLNGLMT.bit.NSLNGLMP = plus_limit_work;		// Noise element plus side limit
	IO_ELA.NSLNGLMT.bit.NSLNGLMM = minus_limit_work;	// Noise element minus side limit
}

/**
 * @brief		ELA core register setting
 * @param[in]	T_IM_ELA_CTRL_COMMON_NOISE ctrl_common_noise
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID im_ela_ctrl_core_register( const T_IM_ELA_CTRL_CORE* const ctrl_ela_core )
{
	// Optical zero level shift parameter
	im_ela_set_reg_signed( IO_ELA.EZSFTA1, union io_ela_ezsfta1, EZSFTA1, ctrl_ela_core->optical_zero_level_shift );

	/* NSLNGLMT register setting */
	im_ela_ctrl_noise_limit_register( &(ctrl_ela_core->limit) );

	IO_ELA.BYRTYP.bit.ELADCMD			= ctrl_ela_core->defect_correction_mode;// Defect pixel correction mode
	IO_ELA.ELFZP.bit.ELFZP				= ctrl_ela_core->zero_point_level;		// Zero point level

	IO_ELA.ELFL.ELFL1.bit.ELFL_1		= ctrl_ela_core->elflt[0];				// ELF luminance threshold 1
	IO_ELA.ELFL.ELFL1.bit.ELFL_2		= ctrl_ela_core->elflt[1];				// ELF luminance threshold 2
	IO_ELA.ELFL.ELFL2.bit.ELFL_3		= ctrl_ela_core->elflt[2];				// ELF luminance threshold 3
	IO_ELA.ELFL.ELFL2.bit.ELFL_4		= ctrl_ela_core->elflt[3];				// ELF luminance threshold 4

	IO_ELA.ANBOR.ANBOR1.bit.ANBOR_0		= ctrl_ela_core->anb_r.offset[0]; 	  	// Noise characteristics of the pixel R : Offset 0
	IO_ELA.ANBOR.ANBOR1.bit.ANBOR_1		= ctrl_ela_core->anb_r.offset[1]; 	  	// Noise characteristics of the pixel R : Offset 1
	IO_ELA.ANBOR.ANBOR2.bit.ANBOR_2		= ctrl_ela_core->anb_r.offset[2]; 	  	// Noise characteristics of the pixel R : Offset 2
	IO_ELA.ANBOR.ANBOR2.bit.ANBOR_3		= ctrl_ela_core->anb_r.offset[3]; 	  	// Noise characteristics of the pixel R : Offset 3
	IO_ELA.ANBKR.ANBKR1.bit.ANBKR_0		= ctrl_ela_core->anb_r.gain[0];		  	// Noise characteristics of the pixel R : Gain 0
	IO_ELA.ANBKR.ANBKR1.bit.ANBKR_1		= ctrl_ela_core->anb_r.gain[1];		  	// Noise characteristics of the pixel R : Gain 1
	IO_ELA.ANBKR.ANBKR2.bit.ANBKR_2		= ctrl_ela_core->anb_r.gain[2];		  	// Noise characteristics of the pixel R : Gain 2
	IO_ELA.ANBKR.ANBKR2.bit.ANBKR_3		= ctrl_ela_core->anb_r.gain[3];		  	// Noise characteristics of the pixel R : Gain 3
	IO_ELA.ANBDR.ANBDR1.bit.ANBDR_0		= ctrl_ela_core->anb_r.boundary[0];	  	// Noise characteristics of the pixel R : boundary 0
	IO_ELA.ANBDR.ANBDR1.bit.ANBDR_1		= ctrl_ela_core->anb_r.boundary[1];	  	// Noise characteristics of the pixel R : boundary 1
	IO_ELA.ANBDR.ANBDR2.bit.ANBDR_2		= ctrl_ela_core->anb_r.boundary[2];	  	// Noise characteristics of the pixel R : boundary 2
	IO_ELA.ANBDR.ANBDR2.bit.ANBDR_3		= ctrl_ela_core->anb_r.boundary[3];	  	// Noise characteristics of the pixel R : boundary 3

	IO_ELA.ANBOG.ANBOG1.bit.ANBOG_0		= ctrl_ela_core->anb_g.offset[0]; 	  	// Noise characteristics of the pixel G : Offset 0
	IO_ELA.ANBOG.ANBOG1.bit.ANBOG_1		= ctrl_ela_core->anb_g.offset[1]; 	  	// Noise characteristics of the pixel G : Offset 1
	IO_ELA.ANBOG.ANBOG2.bit.ANBOG_2		= ctrl_ela_core->anb_g.offset[2]; 	  	// Noise characteristics of the pixel G : Offset 2
	IO_ELA.ANBOG.ANBOG2.bit.ANBOG_3		= ctrl_ela_core->anb_g.offset[3]; 	  	// Noise characteristics of the pixel G : Offset 3
	IO_ELA.ANBKG.ANBKG1.bit.ANBKG_0		= ctrl_ela_core->anb_g.gain[0];		  	// Noise characteristics of the pixel G : Gain 0
	IO_ELA.ANBKG.ANBKG1.bit.ANBKG_1		= ctrl_ela_core->anb_g.gain[1];		  	// Noise characteristics of the pixel G : Gain 1
	IO_ELA.ANBKG.ANBKG2.bit.ANBKG_2		= ctrl_ela_core->anb_g.gain[2];		  	// Noise characteristics of the pixel G : Gain 2
	IO_ELA.ANBKG.ANBKG2.bit.ANBKG_3		= ctrl_ela_core->anb_g.gain[3];		  	// Noise characteristics of the pixel G : Gain 3
	IO_ELA.ANBDG.ANBDG1.bit.ANBDG_0		= ctrl_ela_core->anb_g.boundary[0];	  	// Noise characteristics of the pixel G : boundary 0
	IO_ELA.ANBDG.ANBDG1.bit.ANBDG_1		= ctrl_ela_core->anb_g.boundary[1];	  	// Noise characteristics of the pixel G : boundary 1
	IO_ELA.ANBDG.ANBDG2.bit.ANBDG_2		= ctrl_ela_core->anb_g.boundary[2];	  	// Noise characteristics of the pixel G : boundary 2
	IO_ELA.ANBDG.ANBDG2.bit.ANBDG_3		= ctrl_ela_core->anb_g.boundary[3];	  	// Noise characteristics of the pixel G : boundary 3

	IO_ELA.ANBOB.ANBOB1.bit.ANBOB_0		= ctrl_ela_core->anb_b.offset[0]; 	  	// Noise characteristics of the pixel B : Offset 0
	IO_ELA.ANBOB.ANBOB1.bit.ANBOB_1		= ctrl_ela_core->anb_b.offset[1]; 	  	// Noise characteristics of the pixel B : Offset 1
	IO_ELA.ANBOB.ANBOB2.bit.ANBOB_2		= ctrl_ela_core->anb_b.offset[2]; 	  	// Noise characteristics of the pixel B : Offset 2
	IO_ELA.ANBOB.ANBOB2.bit.ANBOB_3		= ctrl_ela_core->anb_b.offset[3]; 	  	// Noise characteristics of the pixel B : Offset 3
	IO_ELA.ANBKB.ANBKB1.bit.ANBKB_0		= ctrl_ela_core->anb_b.gain[0];		  	// Noise characteristics of the pixel B : Gain 0
	IO_ELA.ANBKB.ANBKB1.bit.ANBKB_1		= ctrl_ela_core->anb_b.gain[1];		  	// Noise characteristics of the pixel B : Gain 1
	IO_ELA.ANBKB.ANBKB2.bit.ANBKB_2		= ctrl_ela_core->anb_b.gain[2];		  	// Noise characteristics of the pixel B : Gain 2
	IO_ELA.ANBKB.ANBKB2.bit.ANBKB_3		= ctrl_ela_core->anb_b.gain[3];		  	// Noise characteristics of the pixel B : Gain 3
	IO_ELA.ANBDB.ANBDB1.bit.ANBDB_0		= ctrl_ela_core->anb_b.boundary[0];	  	// Noise characteristics of the pixel B : boundary 0
	IO_ELA.ANBDB.ANBDB1.bit.ANBDB_1		= ctrl_ela_core->anb_b.boundary[1];	  	// Noise characteristics of the pixel B : boundary 1
	IO_ELA.ANBDB.ANBDB2.bit.ANBDB_2		= ctrl_ela_core->anb_b.boundary[2];	  	// Noise characteristics of the pixel B : boundary 2
	IO_ELA.ANBDB.ANBDB2.bit.ANBDB_3		= ctrl_ela_core->anb_b.boundary[3];	  	// Noise characteristics of the pixel B : boundary 3

	IO_ELA.LGTW.LGTW1.bit.LGTWR			= ctrl_ela_core->lumen_gain_r;			// R Gain for luminance
	IO_ELA.LGTW.LGTW1.bit.LGTWG			= ctrl_ela_core->lumen_gain_g;			// G Gain for luminance
	IO_ELA.LGTW.LGTW2.bit.LGTWB			= ctrl_ela_core->lumen_gain_b;			// B Gain for luminance

	IO_ELA.LGT.bit.LGTR					= ctrl_ela_core->lumen_factor_r;		// R factor for the luminance
	IO_ELA.LGT.bit.LGTG					= ctrl_ela_core->lumen_factor_g;		// G factor for the luminance
	IO_ELA.LGT.bit.LGTB					= ctrl_ela_core->lumen_factor_b;		// B factor for the luminance
	IO_ELA.LGT.bit.LGTC					= ctrl_ela_core->lumen_factor_c;		// C factor for the luminance

	IO_ELA.LGTE.bit.LGTER				= ctrl_ela_core->lumen_edge_r;			// R factor for the degree of weight edge
	IO_ELA.LGTE.bit.LGTEG				= ctrl_ela_core->lumen_edge_g;			// G factor for the degree of weight edge
	IO_ELA.LGTE.bit.LGTEB				= ctrl_ela_core->lumen_edge_b;			// B factor for the degree of weight edge

	IO_ELA.EDG2ANBA.bit.EDG2ANBA		= ctrl_ela_core->edge_adjust_a1;		// Set the degree adjustment factor-1 edge of A
	IO_ELA.EDGSCLA.bit.EDGSCLA			= ctrl_ela_core->edge_adjust_a2;		// Set the degree adjustment factor-2 edge of A
	IO_ELA.EDG2ANBB.bit.EDG2ANBB		= ctrl_ela_core->edge_adjust_b1;		// Set the degree adjustment factor-1 edge of B
	IO_ELA.EDGSCLB.bit.EDGSCLB			= ctrl_ela_core->edge_adjust_b2;		// Set the degree adjustment factor-2 edge of B

	IO_ELA.ALNGE.ALNGE1.bit.ALNGE_1		= ctrl_ela_core->edge_threshold[0];		// Edge degree threshold 1
	IO_ELA.ALNGE.ALNGE1.bit.ALNGE_2		= ctrl_ela_core->edge_threshold[1];		// Edge degree threshold 1
	IO_ELA.ALNGE.ALNGE2.bit.ALNGE_3		= ctrl_ela_core->edge_threshold[2];		// Edge degree threshold 1

	IO_ELA.ALNG.ALNG1.bit.ALNGL1E1N		= ctrl_ela_core->nr[0];					// NR intensity :Luminance L1, Edge E1
	IO_ELA.ALNG.ALNG1.bit.ALNGL1E2N		= ctrl_ela_core->nr[1];					// NR intensity :Luminance L1, Edge E2
	IO_ELA.ALNG.ALNG2.bit.ALNGL1E3N		= ctrl_ela_core->nr[2];					// NR intensity :Luminance L1, Edge E3
	IO_ELA.ALNG.ALNG2.bit.ALNGL2E1N		= ctrl_ela_core->nr[3];					// NR intensity :Luminance L2, Edge E1
	IO_ELA.ALNG.ALNG3.bit.ALNGL2E2N		= ctrl_ela_core->nr[4];					// NR intensity :Luminance L2, Edge E2
	IO_ELA.ALNG.ALNG3.bit.ALNGL2E3N		= ctrl_ela_core->nr[5];					// NR intensity :Luminance L2, Edge E3
	IO_ELA.ALNG.ALNG4.bit.ALNGL3E1N		= ctrl_ela_core->nr[6];					// NR intensity :Luminance L3, Edge E1
	IO_ELA.ALNG.ALNG4.bit.ALNGL3E2N		= ctrl_ela_core->nr[7];					// NR intensity :Luminance L3, Edge E2
	IO_ELA.ALNG.ALNG5.bit.ALNGL3E3N		= ctrl_ela_core->nr[8];					// NR intensity :Luminance L3, Edge E3
	IO_ELA.ALNG.ALNG5.bit.ALNGL4E1N		= ctrl_ela_core->nr[9];					// NR intensity :Luminance L4, Edge E1
	IO_ELA.ALNG.ALNG6.bit.ALNGL4E2N		= ctrl_ela_core->nr[10];				// NR intensity :Luminance L4, Edge E2
	IO_ELA.ALNG.ALNG6.bit.ALNGL4E3N		= ctrl_ela_core->nr[11];				// NR intensity :Luminance L4, Edge E3

	IO_ELA.LFTH.bit.LFTH				= ctrl_ela_core->l_noise; 				// L extraction threshold noise
	IO_ELA.LCOME.bit.LCOME1				= ctrl_ela_core->l_edge[0];				// Edge applicable threshold contour L 1
	IO_ELA.LCOME.bit.LCOME2				= ctrl_ela_core->l_edge[1];				// Edge applicable threshold contour L 2
	IO_ELA.LCOMEN.bit.LCOME1N			= ctrl_ela_core->l_intense[0];			// Applied intensity L 1
	IO_ELA.LCOMEN.bit.LCOME2N			= ctrl_ela_core->l_intense[1];			// Applied intensity L 2
}

#ifdef CO_ELA_DEBUG_PRINT	// Debug
static VOID im_ela_qelatmd_info_print( VOID )
{
	UINT32 i;

	Ddim_Print(("<Im_ELA Qelatmd Info>\n"));
	Ddim_Print(("\t ela_elf_level = %d\n", gIM_ELA_Qelatmd_Info.ela_elf_level));
	for( i = 0; i < E_IM_ELA_ELF_LEVEL_MAX; i++ ){
		Ddim_Print(("\t ela_density_mode[%d] = %d\n", i, gIM_ELA_Qelatmd_Info.ela_density_mode[i]));
	}
}

static VOID im_ela_ctrl_reg_print( VOID )
{
	UINT32 i;

#if 1
	Ddim_Print(( "(Core Reg)\n" ));
	Ddim_Print(( "\tELACNT = %d\n",			IO_ELA.ELACTL.bit.ELACNT ));
	Ddim_Print(( "\tELAACT = %d\n",			IO_ELA.ELACTL.bit.ELAACT ));
	Ddim_Print(( "\tELAMD  = %d\n",			IO_ELA.ELACTL.bit.ELAMD ));
	Ddim_Print(( "\tEIRTYP = %d\n",			IO_ELA.ELABYR.bit.EIRTYP ));
	Ddim_Print(( "\tEIRSFT = %d\n",			IO_ELA.ELABYR.bit.EIRSFT ));
	Ddim_Print(( "\tEIWTYP = %d\n",			IO_ELA.ELABYR.bit.EIWTYP ));
	Ddim_Print(( "\tEIWSFT = %d\n",			IO_ELA.ELABYR.bit.EIWSFT ));
	Ddim_Print(( "\tEIWOUT = %d\n",			IO_ELA.ELABYR.bit.EIWOUT ));
	Ddim_Print(( "\tNSLMD  = %d\n",			IO_ELA.NSLMD.bit.NSLMD ));

	Ddim_Print(( "(EIRch Reg)\n" ));
	Ddim_Print(( "\tEIRA    = 0x%08lx\n",	IO_ELA.EIRA.word ));
	Ddim_Print(( "\tEIROFS  = %d\n",		IO_ELA.EIROFS.bit.EIROFS ));
	Ddim_Print(( "\tEIRDEF  = %d\n",		IO_ELA.EIRDEF.bit.EIRDEF ));
	Ddim_Print(( "\tEIRHSIZ = %d\n",		IO_ELA.EIRHSIZ.bit.EIRHSIZ ));
	Ddim_Print(( "\tEIRVSIZ = %d\n",		IO_ELA.EIRVSIZ.bit.EIRVSIZ ));

	Ddim_Print(( "(EIWch Reg)\n" ));
	for( i = 0; i < 4; i++ ){
		Ddim_Print(( "\tEIWA[%d] = 0x%08lx\n",	i, IO_ELA.EIWA.word[i] ));
	}
	Ddim_Print(( "\tEIWDEF  = %d\n",		IO_ELA.EIWDEF.bit.EIWDEF ));

	Ddim_Print(( "(ENWch Reg)\n" ));
	Ddim_Print(( "\tENWA   = 0x%08lx\n",	IO_ELA.ENWA.word ));
	Ddim_Print(( "\tENWDEF = %d\n",			IO_ELA.ENWDEF.bit.ENWDEF ));

	Ddim_Print(( "(ENREch Reg)\n" ));
	for( i = 0; i < 16; i++ ){
		Ddim_Print(( "\tENRA[%d] = 0x%08lx\n",	i, IO_ELA.ENRA.word[i] ));
	}
	Ddim_Print(( "\tENRHSIZ  = %d\n",		IO_ELA.ENRHSIZ.bit.ENRHSIZ ));
	Ddim_Print(( "\tENRVSIZ  = %d\n",		IO_ELA.ENRVSIZ.bit.ENRVSIZ ));

	Ddim_Print(( "(ENRAch/ENWMch Reg)\n" ));
	Ddim_Print(( "\tENSA    = 0x%08lx\n",	IO_ELA.ENSA.word ));
	Ddim_Print(( "\tENSDEF  = %d\n",		IO_ELA.ENSDEF.bit.ENSDEF ));
	Ddim_Print(( "\tENSHSIZ = %d\n",		IO_ELA.ENSHSIZ.bit.ENSHSIZ ));
	Ddim_Print(( "\tENSVSIZ = %d\n",		IO_ELA.ENSVSIZ.bit.ENSVSIZ ));
#else
	Ddim_Print(( "\tELATRG = 0x%02x\n",		IO_ELA.ELATRG.bit.ELATRG ));
	
	Ddim_Print(( "\tELACTL = 0x%08lx\n",	IO_ELA.ELACTL.word ));
	
	Ddim_Print(( "\tELABYR = 0x%08lx\n",	IO_ELA.ELABYR.word ));
	
	Ddim_Print(( "\tNSLMD = 0x%08lx\n",		IO_ELA.NSLMD.word ));
	
	Ddim_Print(( "\tOBOF = 0x%016llx\n",	IO_ELA.OBOF.dword ));

	Ddim_Print(( "\tEZSFTA1 = 0x%04x\n",	IO_ELA.EZSFTA1.bit.EZSFTA1 ));

	Ddim_Print(( "\tNSLNGLMT = 0x%08lx\n",	IO_ELA.NSLNGLMT.word ));

	Ddim_Print(( "\tEIRA = 0x%08lx\n",		IO_ELA.EIRA.word ));
	Ddim_Print(( "\tEIROFS = 0x%02x\n",		IO_ELA.EIROFS.bit.EIROFS ));
	Ddim_Print(( "\tEIRDEF = 0x%04x\n",		IO_ELA.EIRDEF.bit.EIRDEF ));
	Ddim_Print(( "\tEIRHSIZ = 0x%04x\n",	IO_ELA.EIRHSIZ.bit.EIRHSIZ ));
	Ddim_Print(( "\tEIRVSIZ = 0x%04x\n",	IO_ELA.EIRVSIZ.bit.EIRVSIZ ));

	for( i = 0; i < 4; i++ ){
		Ddim_Print(( "\tEIWA[%d] = 0x%08lx\n",	i, IO_ELA.EIWA.word[i] ));
	}
	Ddim_Print(( "\tEIWDEF = 0x%04x\n",		IO_ELA.EIWDEF.bit.EIWDEF ));

	Ddim_Print(( "\tENWA = 0x%08lx\n",		IO_ELA.ENWA.word ));
	Ddim_Print(( "\tENWDEF = 0x%04x\n",		IO_ELA.ENWDEF.bit.ENWDEF ));

	for( i = 0; i < 16; i++ ){
		Ddim_Print(( "\tENRA[%d] = 0x%08lx\n",	i, IO_ELA.ENRA.word[i] ));
	}
	Ddim_Print(( "\tENRHSIZ = 0x%04x\n",	IO_ELA.ENRHSIZ.bit.ENRHSIZ ));
	Ddim_Print(( "\tENRVSIZ = 0x%04x\n",	IO_ELA.ENRVSIZ.bit.ENRVSIZ ));

	Ddim_Print(( "\tENSA = 0x%08lx\n",		IO_ELA.ENSA.word ));
	Ddim_Print(( "\tENSDEF = 0x%04x\n",		IO_ELA.ENSDEF.bit.ENSDEF ));
	Ddim_Print(( "\tENSHSIZ = 0x%04x\n",	IO_ELA.ENSHSIZ.bit.ENSHSIZ ));
	Ddim_Print(( "\tENSVSIZ = 0x%04x\n",	IO_ELA.ENSVSIZ.bit.ENSVSIZ ));
#endif
}

#endif


#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------


/**
 * @brief		The ela channel is exclusively controlled.
 * @param[in]	INT32 tmout
 * @return		INT32 D_DDIM_OK/D_IM_ELA_SEM_NG/D_IM_ELA_INPUT_PARAM_ERROR/D_IM_ELA_SEM_TIMEOUT_ERR
 * @note		None
 * @attention	None
 */
INT32 Im_ELA_Open( const INT32 tmout )
{
	DDIM_USER_ER ercd;

#ifdef CO_PARAM_CHECK
	if( tmout < D_DDIM_USER_SEM_WAIT_FEVR ){
		Ddim_Assertion(("Im_ELA_Open() error. input param error. tmout = %x\n", tmout));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	if( D_DDIM_USER_SEM_WAIT_POL == tmout ){
		ercd = DDIM_User_Pol_Sem( SID_IM_ELA );							// pol_sem()
	}
	else{
		ercd = DDIM_User_Twai_Sem( SID_IM_ELA, (DDIM_USER_TMO)tmout );	// twai_sem()
	}

	if( D_DDIM_USER_E_TMOUT == ercd ){
		// A semaphore acquisition processing time out
		Ddim_Print(("Im_ELA_Open() error : Semaphore Get Time Out\n"));
		return D_IM_ELA_SEM_TIMEOUT_ERR;
	}

	if( D_DDIM_USER_E_OK != ercd ){
		// Ela processing error
		Ddim_Print(("Im_ELA_Open() error : Semaphore Get Error\n"));
		return D_IM_ELA_SEM_NG;
	}

	return D_DDIM_OK;
}

/**
 * @brief 		The exclusive control of control Channel it is released.
 * @param[in]	None
 * @return 		INT32 D_DDIM_OK/D_IM_ELA_SEM_NG
 * @note		None
 * @attention	None
 */
INT32 Im_ELA_Close( VOID )
{
	DDIM_USER_ER ercd;

	ercd = DDIM_User_Sig_Sem( SID_IM_ELA );				// sig_sem()
	if( D_DDIM_USER_E_OK != ercd ){
		// SPR processing error
		Ddim_Print(("Im_ELA_Close() Error.\n"));
		return D_IM_ELA_SEM_NG;
	}
	return D_DDIM_OK;
}

/**
 * @brief		ELA Initialize
 * @param[in]	None
 * @return		None
 * @note		None
 * @attention	None
 */
VOID Im_ELA_Init( VOID )
{
	union io_ela_elaintf elaintf;

	gIM_ELA_Axi_Err = D_IM_ELA_NORMAL_END;

	im_ela_on_pclk();

	/* Interrupt setting */
	IO_ELA.ELAINTE.bit.ENWE = 1;		// int enable
	IO_ELA.ELAINTE.bit.AXRERE = 1;		// int enable
	IO_ELA.ELAINTE.bit.AXWERE = 1;		// int enable

	// flag clear
	elaintf.word = 0;
	elaintf.bit.__ENWF = 1;
	elaintf.bit.__AXRERF = 1;
	elaintf.bit.__AXWERF = 1;

	IO_ELA.ELAINTF.word = elaintf.word;

	im_ela_off_pclk();
}

/**
 * @brief		Get ELA executing count
 * @param[in]	None
 * @return		ELA executing count.(0~4)
 * @note		None
 * @attention	None
 */
UCHAR Im_ELA_Get_Exe_Cnt( VOID )
{
	UCHAR ela_count;

	im_ela_on_pclk();

	ela_count = ( IO_ELA.ELACTL.bit.ELACNT );

	im_ela_off_pclk();

	return ela_count;
}

/**
 * @brief		The register at the ELA parameters is set
 * @param[in]	T_IM_ELA_CTRL* ctrl_ela
 * @return		INT32 D_DDIM_OK / D_IM_ELA_INPUT_PARAM_ERROR
 * @note		None
 * @attention	None
 */
UINT32 Im_ELA_Ctrl( const T_IM_ELA_CTRL* const ctrl_ela )
{
	UINT32 loop_cnt;

#ifdef CO_PARAM_CHECK
	/* check on input pointer */
	if( ctrl_ela == NULL ){
		Ddim_Assertion(("Im_ELA_Ctrl() error. ctrl_ela = NULL\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}

	/* check on input elf level */
	if( ctrl_ela->ela_elf_level == E_IM_ELA_ELF_LEVEL_MAX ){
		Ddim_Assertion(("Im_ELA_Ctrl() error. ela_elf_level = %d\n", ctrl_ela->ela_elf_level));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}

	/* check on input ELA density mode */
	for( loop_cnt = 1; loop_cnt <= ctrl_ela->ela_elf_level; loop_cnt++ ){
		if( ctrl_ela->ela_density_mode[loop_cnt] > (ctrl_ela->ela_density_mode[loop_cnt - 1] + 1) ){
			Ddim_Assertion(("Im_ELA_Ctrl() error. ela_density_mode[%d] = %d (ela_density_mode[%d] = %d)\n", 
																						loop_cnt,
																						ctrl_ela->ela_density_mode[loop_cnt],
																						(loop_cnt - 1),
																						ctrl_ela->ela_density_mode[loop_cnt - 1]));
			return D_IM_ELA_INPUT_PARAM_ERROR;
		}
	}
	if( ctrl_ela->ctrl_common.noise_data_compress_enable == D_IM_ELA_ENABLE ){
		if( (ctrl_ela->ctrl_common.noise_data_precision == D_IM_ELA_NSLRES_PRECISION_3) ||
		   (ctrl_ela->ctrl_common.noise_data_precision == D_IM_ELA_NSLRES_PRECISION_4) ){
			Ddim_Assertion(("Im_ELA_Ctrl() error. noise_data_compress_enable = %d, noise_data_precision = %d)\n", 
																						ctrl_ela->ctrl_common.noise_data_compress_enable,
																						ctrl_ela->ctrl_common.noise_data_precision));
			return D_IM_ELA_INPUT_PARAM_ERROR;
		}
	}
#endif // CO_PARAM_CHECK

	memset( (T_IM_ELA_BAYER_MNG*)&gIM_ELA_Bayer, 0, sizeof(T_IM_ELA_BAYER_MNG) );
#ifdef CO_ELA_DEBUG_PRINT	// Debug
	Ddim_Print(("Im_ELA_Ctrl() : gIM_ELA_Bayer clear END\n"));
#endif

	im_ela_on_pclk();

	// Wait ELA Stop
	while( IO_ELA.ELATRG.bit.ELATRG == 3 ){
		DDIM_User_Dly_Tsk(1);
		Ddim_Print(("Im_ELA_Ctrl() : ELA Processing\n"));
	}

	gIM_ELA_Next_Ctrl_Flag = ctrl_ela->next_ctrl_flag;

	gIM_ELA_Stage_Cnt = 0;								// ELA macro stage count
	gIM_ELA_Step_Cnt = 0;								// ELA macro step count

	gIM_ELA_Qelatmd_Info.ela_elf_level = ctrl_ela->ela_elf_level;
	for( loop_cnt = 0; loop_cnt <= ctrl_ela->ela_elf_level; loop_cnt++ ){
		gIM_ELA_Qelatmd_Info.ela_density_mode[loop_cnt] = ctrl_ela->ela_density_mode[loop_cnt];
	}

	/* common register setting */
	IO_ELA.ELACTL.bit.ELACNT = 0;												// ELA count
	IO_ELA.ELACTL.bit.ELAACT = D_IM_ELA_ELAACT_EXTRACT;							// Operation mode
	IO_ELA.ELACTL.bit.ELAMD = ctrl_ela->ela_density_mode[E_IM_ELA_ELF_LEVEL_1];	// ELA density mode

	IO_ELA.ELABYR.bit.EIRTYP = ctrl_ela->ctrl_byr.in_data_type;					// Input data type
	// Input bit shift mode
	switch( ctrl_ela->ctrl_byr.in_data_shift_mode ){
		case D_IM_ELA_BIT_SHIFT_MODE_R2:
			IO_ELA.ELABYR.bit.EIRSFT = 0x00;
			break;
		case D_IM_ELA_BIT_SHIFT_MODE_L2:
			IO_ELA.ELABYR.bit.EIRSFT = 0x02;
			break;
		case D_IM_ELA_BIT_SHIFT_MODE_L4:
			IO_ELA.ELABYR.bit.EIRSFT = 0x03;
			break;
		default:
			// Not Bit Shift
			IO_ELA.ELABYR.bit.EIRSFT = 0x01;
			break;
	}
	IO_ELA.ELABYR.bit.EIWTYP = ctrl_ela->ctrl_byr.out_data_type;				// Output data type
	// Output bit shift mode
	switch( ctrl_ela->ctrl_byr.out_data_shift_mode ){
		case D_IM_ELA_BIT_SHIFT_MODE_R2:
			IO_ELA.ELABYR.bit.EIWSFT = 0x02;
			gIM_ELA_Bayer.eiw_sft = 0x02;
			break;
		case D_IM_ELA_BIT_SHIFT_MODE_R4:
			IO_ELA.ELABYR.bit.EIWSFT = 0x03;
			gIM_ELA_Bayer.eiw_sft = 0x03;
			break;
		default:
			// Not Bit Shift
			IO_ELA.ELABYR.bit.EIWSFT = 0x01;
			gIM_ELA_Bayer.eiw_sft = 0x01;
			break;
	}
	// Output data num
	if( ctrl_ela->ela_elf_level == E_IM_ELA_ELF_LEVEL_1 ){
		IO_ELA.ELABYR.bit.EIWOUT = D_IM_ELA_EIWOUT_NONE;
	}
	else{
		switch( ctrl_ela->ela_density_mode[E_IM_ELA_ELF_LEVEL_2] ){
			case E_IM_ELA_DENSITY_MODE_0:
			case E_IM_ELA_DENSITY_MODE_1:
				IO_ELA.ELABYR.bit.EIWOUT = D_IM_ELA_EIWOUT_1;
				break;
//			case E_IM_ELA_DENSITY_MODE_2:
//			case E_IM_ELA_DENSITY_MODE_3:
			default:
				IO_ELA.ELABYR.bit.EIWOUT = D_IM_ELA_EIWOUT_4;
				break;
		}
	}

	gIM_ELA_Bayer.eiw_dtype = ctrl_ela->ctrl_byr.out_data_type;

	// First set only
	if( ctrl_ela->ctrl_byr.deknee_enable == D_IM_ELA_ENABLE ){
		// Enabling the Deknee processing for bayer data entry
		IO_ELA.ELABYR.bit.DKNEN = ctrl_ela->ctrl_byr.deknee_enable;
		IO_ELA.ELABYR.bit.DKNACT = 1;
	}

	// Enabling the "OB correction"/"Optical zero shift" processing for bayer data entry
	IO_ELA.ELABYR.bit.EOBAEN = ctrl_ela->ctrl_byr.ob_zsf_enable;

	IO_ELA.NSLMD.bit.NSLMD = ctrl_ela->ctrl_common.noise_data_mode;				// Noise data mode
	IO_ELA.NSLMD.bit.NSLRES = ctrl_ela->ctrl_common.noise_data_precision;		// Noise data precision
	IO_ELA.NSLMD.bit.NSLKNE = ctrl_ela->ctrl_common.noise_data_compress_enable;	// Noise data compression enable
	IO_ELA.BYRTYP.bit.ORG = ctrl_ela->ctrl_common.bayer_start_pixel_type;		// Bayer data start-pixel type
	memcpy( (T_IM_ELA_CTRL_COMMON*)&gIM_ELA_Ctrl_Common_Info, &(ctrl_ela->ctrl_common), sizeof(T_IM_ELA_CTRL_COMMON) );
#ifdef CO_ELA_DEBUG_PRINT	// Debug
	Ddim_Print(("Im_ELA_Ctrl() : T_IM_ELA_CTRL_COMMON Copy END\n"));
#endif

	if( ctrl_ela->ctrl_byr.ob_zsf_enable == D_IM_ELA_ENABLE ){
		// OB Correction R Pixel
		im_ela_set_reg_signed( IO_ELA.OBOF.OBOF1, union io_ela_obof_1, EOBRR, ctrl_ela->ob_correction.ob_rr );

		// OB Correction B Pixel
		im_ela_set_reg_signed( IO_ELA.OBOF.OBOF1, union io_ela_obof_1, EOBBB, ctrl_ela->ob_correction.ob_bb );

		// OB Correction Gr Pixel
		im_ela_set_reg_signed( IO_ELA.OBOF.OBOF2, union io_ela_obof_2, EOBGR, ctrl_ela->ob_correction.ob_gr );

		// OB Correction Gb Pixel
		im_ela_set_reg_signed( IO_ELA.OBOF.OBOF2, union io_ela_obof_2, EOBGB, ctrl_ela->ob_correction.ob_gb );
	}

	/* EIRch register setting */
	im_ela_ctrl_eirch_register( ctrl_ela->ctrl_eirch );
#ifdef CO_ELA_DEBUG_PRINT	// Debug
	Ddim_Print(("Im_ELA_Ctrl() : EIRch register setting END\n"));
#endif

	/* calculate curtail bayer data */
	im_ela_calc_curtail_bayer_data( ctrl_ela->out_bayer_addr );
#ifdef CO_ELA_DEBUG_PRINT	// Debug
	Ddim_Print(("Im_ELA_Ctrl() : calculate curtail bayer data END\n"));
#endif

	/* calculate extraction noise data */
	im_ela_calc_extract_noise_data( ctrl_ela->extract_noise_addr, ctrl_ela->noise_suppress_addr );
#ifdef CO_ELA_DEBUG_PRINT	// Debug
	Ddim_Print(("Im_ELA_Ctrl() : calculate extraction noise data END\n"));
#endif

	/* EIWch register setting */
	im_ela_ctrl_eiwch_register();
#ifdef CO_ELA_DEBUG_PRINT	// Debug
	Ddim_Print(("Im_ELA_Ctrl() : EIWch register setting END\n"));
#endif

	/* ENWch register setting */
	im_ela_ctrl_enwch_register();
#ifdef CO_ELA_DEBUG_PRINT	// Debug
	Ddim_Print(("Im_ELA_Ctrl() : ENWch register setting END\n"));
#endif

	/* ENWMch register setting */
	im_ela_ctrl_enrach_enwmch_register();
#ifdef CO_ELA_DEBUG_PRINT	// Debug
	Ddim_Print(("Im_ELA_Ctrl() : ENWMch register setting END\n"));
#endif

	gIM_ELA_CallBack_Func = ctrl_ela->pCallBack;
	im_ela_off_pclk();
	Im_ELA_Dsb();

	return D_DDIM_OK;
}

/**
 * @brief		The register at the ELA common parameters is get
 * @param[out]	T_IM_ELA_CTRL_COMMON* ctrl_ela_common
 * @return		INT32 D_DDIM_OK / D_IM_ELA_INPUT_PARAM_ERROR
 * @note		None
 * @attention	None
 */
UINT32 Im_ELA_Get_Ctrl_Common( T_IM_ELA_CTRL_COMMON* ctrl_ela_common )
{
#ifdef CO_PARAM_CHECK
	/* check on input pointer */
	if( ctrl_ela_common == NULL ){
		Ddim_Assertion(("Im_ELA_Get_Ctrl_Common() error. ctrl_ela_common = NULL\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_ela_on_pclk();

	ctrl_ela_common->noise_data_mode			= IO_ELA.NSLMD.bit.NSLMD;		// Noise data mode
	ctrl_ela_common->noise_data_precision		= IO_ELA.NSLMD.bit.NSLRES;		// Noise data precision
	ctrl_ela_common->noise_data_compress_enable	= IO_ELA.NSLMD.bit.NSLKNE;		// Noise data compression enable
	ctrl_ela_common->bayer_start_pixel_type		= IO_ELA.BYRTYP.bit.ORG;		// Bayer data start-pixel type

	im_ela_off_pclk();

	return D_DDIM_OK;
}

/**
 * @brief		The register is set for the ELA macro processing since the second times. 
 * @param[in]	None
 * @return		INT32 D_DDIM_OK
 * @note		None
 * @attention	None
 */
UINT32 Im_ELA_Ctrl_Next( VOID )
{
	im_ela_on_pclk();

	/* Common register & EIIch register & ENIch/ENOch register setting */
	im_ela_ctrl_next_register();

	im_ela_off_pclk();
	Im_ELA_Dsb();

	return D_DDIM_OK;
}

/**
 * @brief		The register at the EIRch is get
 * @param[out]	T_IM_ELA_CTRL_EIRCH* ctrl_ela_eirch
 * @return		INT32 D_DDIM_OK / D_IM_ELA_INPUT_PARAM_ERROR
 * @note		None
 * @attention	None
 */
UINT32 Im_ELA_Get_Ctrl_Eirch( T_IM_ELA_CTRL_EIRCH* const ctrl_ela_eirch )
{
#ifdef CO_PARAM_CHECK
	/* check on input pointer */
	if( ctrl_ela_eirch == NULL ){
		Ddim_Assertion(("Im_ELA_Get_Ctrl_Eirch() error. ctrl_ela_eirch = NULL\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_ela_on_pclk();

	ctrl_ela_eirch->in_bayer_top_addr = IO_ELA.EIRA.word;			// Input bayer data top address

	ctrl_ela_eirch->in_bayer_top_offset =IO_ELA.EIROFS.bit.EIROFS;	// Input bayer data Top area bit offset

	ctrl_ela_eirch->in_bayer_total_hsize = IO_ELA.EIRDEF.word;		// Input bayer data total horizontal size
	ctrl_ela_eirch->in_bayer_hsize = IO_ELA.EIRHSIZ.word;			// Input bayer data horizontal size
	ctrl_ela_eirch->in_bayer_vline = IO_ELA.EIRVSIZ.word;			// Input bayer data vertical line

	im_ela_off_pclk();

	return D_DDIM_OK;
}

/**
 * @brief		The register at the EIWch is get
 * @param[out]	T_IM_ELA_CTRL_EIWCH* ctrl_ela_eiwch
 * @return		INT32 D_DDIM_OK / D_IM_ELA_INPUT_PARAM_ERROR
 * @note		None
 * @attention	None
 */
UINT32 Im_ELA_Get_Ctrl_Eiwch( T_IM_ELA_CTRL_EIWCH* const ctrl_ela_eiwch )
{
	UINT32	i;
#ifdef CO_PARAM_CHECK
	/* check on input pointer */
	if( ctrl_ela_eiwch == NULL ){
		Ddim_Assertion(("Im_ELA_Get_Ctrl_Eiwch() error. ctrl_ela_eiwch = NULL\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_ela_on_pclk();

	for( i =0; i < D_IM_ELA_EIWCH_ADDR_MAX; i++ ){
		ctrl_ela_eiwch->out_bayer_top_addr[i] = IO_ELA.EIWA.word[i];	// Output bayer data top address
	}
	ctrl_ela_eiwch->out_bayer_total_hsize = IO_ELA.EIWDEF.word;			// Output bayer data total horizontal size

	im_ela_off_pclk();

	return D_DDIM_OK;
}

/**
 * @brief		The register at the ENWch is get
 * @param[out]	T_IM_ELA_CTRL_ENWCH* ctrl_ela_enwch
 * @return		INT32 D_DDIM_OK / D_IM_ELA_INPUT_PARAM_ERROR
 * @note		None
 * @attention	None
 */
UINT32 Im_ELA_Get_Ctrl_Enwch( T_IM_ELA_CTRL_ENWCH* const ctrl_ela_enwch )
{
#ifdef CO_PARAM_CHECK
	/* check on input pointer */
	if( ctrl_ela_enwch == NULL ){
		Ddim_Assertion(("Im_ELA_Get_Ctrl_Enwch() error. ctrl_ela_enwch = NULL\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_ela_on_pclk();

	ctrl_ela_enwch->out_extract_noise_top_addr = IO_ELA.ENWA.word;		// Output extraction noise data top address
	ctrl_ela_enwch->out_extract_noise_total_hsize = IO_ELA.ENWDEF.word;	// Output extraction noise data total horizontal size

	im_ela_off_pclk();

	return D_DDIM_OK;
}

/**
 * @brief		The register at the ENREch is get
 * @param[out]	T_IM_ELA_CTRL_ENRECH* ctrl_ela_enrech
 * @return		INT32 D_DDIM_OK / D_IM_ELA_INPUT_PARAM_ERROR
 * @note		None
 * @attention	None
 */
UINT32 Im_ELA_Get_Ctrl_Enrech( T_IM_ELA_CTRL_ENRECH* const ctrl_ela_enrech )
{
	UINT32	i;
#ifdef CO_PARAM_CHECK
	/* check on input pointer */
	if( ctrl_ela_enrech == NULL ){
		Ddim_Assertion(("Im_ELA_Get_Ctrl_Enrech() error. ctrl_ela_enrech = NULL\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_ela_on_pclk();

	for( i =0; i < D_IM_ELA_ENRECH_ADDR_MAX; i++ ){
		ctrl_ela_enrech->in_extract_noise_top_addr[i] = IO_ELA.ENRA.word[i];	// Input extraction noise data top address
	}
	ctrl_ela_enrech->in_extract_noise_hsize = IO_ELA.ENRHSIZ.word;		// Input extraction noise data horizontal size
	ctrl_ela_enrech->in_extract_noise_vline = IO_ELA.ENRVSIZ.word;	// Input extraction noise data vertical line

	im_ela_off_pclk();

	return D_DDIM_OK;
}

/**
 * @brief		The register at the ENRAch/ENWMch is get
 * @param[out]	T_IM_ELA_CTRL_ENRACH_ENWMCH* ctrl_ela_enrach_enwmch
 * @return		INT32 D_DDIM_OK / D_IM_ELA_INPUT_PARAM_ERROR
 * @note		None
 * @attention	None
 */
UINT32 Im_ELA_Get_Ctrl_Enrach_Enwmch( T_IM_ELA_CTRL_ENRACH_ENWMCH* const ctrl_ela_enrach_enwmch )
{
#ifdef CO_PARAM_CHECK
	/* check on input pointer */
	if( ctrl_ela_enrach_enwmch == NULL ){
		Ddim_Assertion(("Im_ELA_Get_Ctrl_Enrach_Enwmch() error. ctrl_ela_enrach_enwmch = NULL\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_ela_on_pclk();

	ctrl_ela_enrach_enwmch->synthetic_noise_top_addr = IO_ELA.ENSA.word;		// Input/Output synthetic noise data top address
	ctrl_ela_enrach_enwmch->synthetic_noise_total_hsize = IO_ELA.ENSDEF.word;	// Input/Output synthetic noise data total horizontal size
	ctrl_ela_enrach_enwmch->synthetic_noise_hsize = IO_ELA.ENSHSIZ.word;		// Input/Output synthetic noise data horizontal size
	ctrl_ela_enrach_enwmch->synthetic_noise_vline = IO_ELA.ENSVSIZ.word;		// Input/Output synthetic noise data vertical line

	im_ela_off_pclk();

	return D_DDIM_OK;
}

/**
 * @brief		The register at the core information is get
 * @param[out]	T_IM_ELA_CTRL_CORE* ctrl_ela_core
 * @return		INT32 D_DDIM_OK / D_IM_ELA_INPUT_PARAM_ERROR
 * @note		None
 * @attention	None
 */
UINT32 Im_ELA_Get_Ctrl_Core( T_IM_ELA_CTRL_CORE* const ctrl_ela_core )
{
#ifdef CO_PARAM_CHECK
	/* check on input pointer */
	if( ctrl_ela_core == NULL ){
		Ddim_Assertion(("Im_ELA_Get_Ctrl_Core() error. ctrl_ela_core = NULL\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_ela_on_pclk();

	ctrl_ela_core->defect_correction_mode = IO_ELA.BYRTYP.bit.ELADCMD;		// Defect pixel correction mode
	ctrl_ela_core->zero_point_level = IO_ELA.ELFZP.bit.ELFZP;				// Zero point level

	ctrl_ela_core->elflt[0] = IO_ELA.ELFL.ELFL1.bit.ELFL_1;					// ELF luminance threshold 1
	ctrl_ela_core->elflt[1] = IO_ELA.ELFL.ELFL1.bit.ELFL_2;					// ELF luminance threshold 2
	ctrl_ela_core->elflt[2] = IO_ELA.ELFL.ELFL2.bit.ELFL_3;					// ELF luminance threshold 3
	ctrl_ela_core->elflt[3] = IO_ELA.ELFL.ELFL2.bit.ELFL_4;					// ELF luminance threshold 4

	ctrl_ela_core->anb_r.offset[0] = IO_ELA.ANBOR.ANBOR1.bit.ANBOR_0;		// Noise characteristics of the pixel R : Offset 0
	ctrl_ela_core->anb_r.offset[1] = IO_ELA.ANBOR.ANBOR1.bit.ANBOR_1;		// Noise characteristics of the pixel R : Offset 1
	ctrl_ela_core->anb_r.offset[2] = IO_ELA.ANBOR.ANBOR2.bit.ANBOR_2;		// Noise characteristics of the pixel R : Offset 2
	ctrl_ela_core->anb_r.offset[3] = IO_ELA.ANBOR.ANBOR2.bit.ANBOR_3;		// Noise characteristics of the pixel R : Offset 3
	ctrl_ela_core->anb_r.gain[0] = IO_ELA.ANBKR.ANBKR1.bit.ANBKR_0;			// Noise characteristics of the pixel R : Gain 0
	ctrl_ela_core->anb_r.gain[1] = IO_ELA.ANBKR.ANBKR1.bit.ANBKR_1;			// Noise characteristics of the pixel R : Gain 1
	ctrl_ela_core->anb_r.gain[2] = IO_ELA.ANBKR.ANBKR2.bit.ANBKR_2;			// Noise characteristics of the pixel R : Gain 2
	ctrl_ela_core->anb_r.gain[3] = IO_ELA.ANBKR.ANBKR2.bit.ANBKR_3;			// Noise characteristics of the pixel R : Gain 3
	ctrl_ela_core->anb_r.boundary[0] = IO_ELA.ANBDR.ANBDR1.bit.ANBDR_0;		// Noise characteristics of the pixel R : boundary 0
	ctrl_ela_core->anb_r.boundary[1] = IO_ELA.ANBDR.ANBDR1.bit.ANBDR_1;		// Noise characteristics of the pixel R : boundary 1
	ctrl_ela_core->anb_r.boundary[2] = IO_ELA.ANBDR.ANBDR2.bit.ANBDR_2;		// Noise characteristics of the pixel R : boundary 2
	ctrl_ela_core->anb_r.boundary[3] = IO_ELA.ANBDR.ANBDR2.bit.ANBDR_3;		// Noise characteristics of the pixel R : boundary 3

	ctrl_ela_core->anb_g.offset[0] = IO_ELA.ANBOG.ANBOG1.bit.ANBOG_0;		// Noise characteristics of the pixel G : Offset 0
	ctrl_ela_core->anb_g.offset[1] = IO_ELA.ANBOG.ANBOG1.bit.ANBOG_1;		// Noise characteristics of the pixel G : Offset 1
	ctrl_ela_core->anb_g.offset[2] = IO_ELA.ANBOG.ANBOG2.bit.ANBOG_2;		// Noise characteristics of the pixel G : Offset 2
	ctrl_ela_core->anb_g.offset[3] = IO_ELA.ANBOG.ANBOG2.bit.ANBOG_3;		// Noise characteristics of the pixel G : Offset 3
	ctrl_ela_core->anb_g.gain[0] = IO_ELA.ANBKG.ANBKG1.bit.ANBKG_0;			// Noise characteristics of the pixel G : Gain 0
	ctrl_ela_core->anb_g.gain[1] = IO_ELA.ANBKG.ANBKG1.bit.ANBKG_1;			// Noise characteristics of the pixel G : Gain 1
	ctrl_ela_core->anb_g.gain[2] = IO_ELA.ANBKG.ANBKG2.bit.ANBKG_2;			// Noise characteristics of the pixel G : Gain 2
	ctrl_ela_core->anb_g.gain[3] = IO_ELA.ANBKG.ANBKG2.bit.ANBKG_3;			// Noise characteristics of the pixel G : Gain 3
	ctrl_ela_core->anb_g.boundary[0] = IO_ELA.ANBDG.ANBDG1.bit.ANBDG_0;		// Noise characteristics of the pixel G : boundary 0
	ctrl_ela_core->anb_g.boundary[1] = IO_ELA.ANBDG.ANBDG1.bit.ANBDG_1;		// Noise characteristics of the pixel G : boundary 1
	ctrl_ela_core->anb_g.boundary[2] = IO_ELA.ANBDG.ANBDG2.bit.ANBDG_2;		// Noise characteristics of the pixel G : boundary 2
	ctrl_ela_core->anb_g.boundary[3] = IO_ELA.ANBDG.ANBDG2.bit.ANBDG_3;		// Noise characteristics of the pixel G : boundary 3

	ctrl_ela_core->anb_b.offset[0] = IO_ELA.ANBOB.ANBOB1.bit.ANBOB_0;		// Noise characteristics of the pixel B : Offset 0
	ctrl_ela_core->anb_b.offset[1] = IO_ELA.ANBOB.ANBOB1.bit.ANBOB_1;		// Noise characteristics of the pixel B : Offset 1
	ctrl_ela_core->anb_b.offset[2] = IO_ELA.ANBOB.ANBOB2.bit.ANBOB_2;		// Noise characteristics of the pixel B : Offset 2
	ctrl_ela_core->anb_b.offset[3] = IO_ELA.ANBOB.ANBOB2.bit.ANBOB_3;		// Noise characteristics of the pixel B : Offset 3
	ctrl_ela_core->anb_b.gain[0] = IO_ELA.ANBKB.ANBKB1.bit.ANBKB_0;			// Noise characteristics of the pixel B : Gain 0
	ctrl_ela_core->anb_b.gain[1] = IO_ELA.ANBKB.ANBKB1.bit.ANBKB_1;			// Noise characteristics of the pixel B : Gain 1
	ctrl_ela_core->anb_b.gain[2] = IO_ELA.ANBKB.ANBKB2.bit.ANBKB_2;			// Noise characteristics of the pixel B : Gain 2
	ctrl_ela_core->anb_b.gain[3] = IO_ELA.ANBKB.ANBKB2.bit.ANBKB_3;			// Noise characteristics of the pixel B : Gain 3
	ctrl_ela_core->anb_b.boundary[0] = IO_ELA.ANBDB.ANBDB1.bit.ANBDB_0;		// Noise characteristics of the pixel B : boundary 0
	ctrl_ela_core->anb_b.boundary[1] = IO_ELA.ANBDB.ANBDB1.bit.ANBDB_1;		// Noise characteristics of the pixel B : boundary 1
	ctrl_ela_core->anb_b.boundary[2] = IO_ELA.ANBDB.ANBDB2.bit.ANBDB_2;		// Noise characteristics of the pixel B : boundary 2
	ctrl_ela_core->anb_b.boundary[3] = IO_ELA.ANBDB.ANBDB2.bit.ANBDB_3;		// Noise characteristics of the pixel B : boundary 3

	ctrl_ela_core->lumen_gain_r = IO_ELA.LGTW.LGTW1.bit.LGTWR;				// R Gain for luminance
	ctrl_ela_core->lumen_gain_g = IO_ELA.LGTW.LGTW1.bit.LGTWG;				// G Gain for luminance
	ctrl_ela_core->lumen_gain_b = IO_ELA.LGTW.LGTW2.bit.LGTWB;				// B Gain for luminance

	ctrl_ela_core->lumen_factor_r = IO_ELA.LGT.bit.LGTR;					// R factor for the luminance
	ctrl_ela_core->lumen_factor_g = IO_ELA.LGT.bit.LGTG;					// G factor for the luminance
	ctrl_ela_core->lumen_factor_b = IO_ELA.LGT.bit.LGTB;					// B factor for the luminance
	ctrl_ela_core->lumen_factor_c = IO_ELA.LGT.bit.LGTC;					// C factor for the luminance

	ctrl_ela_core->lumen_edge_r = IO_ELA.LGTE.bit.LGTER;					// R factor for the degree of weight edge
	ctrl_ela_core->lumen_edge_g = IO_ELA.LGTE.bit.LGTEG;					// G factor for the degree of weight edge
	ctrl_ela_core->lumen_edge_b = IO_ELA.LGTE.bit.LGTEB;					// B factor for the degree of weight edge

	ctrl_ela_core->edge_adjust_a1 = IO_ELA.EDG2ANBA.bit.EDG2ANBA;			// Set the degree adjustment factor-1 edge of A
	ctrl_ela_core->edge_adjust_a2 = IO_ELA.EDGSCLA.bit.EDGSCLA;				// Set the degree adjustment factor-2 edge of A
	ctrl_ela_core->edge_adjust_b1 = IO_ELA.EDG2ANBB.bit.EDG2ANBB;			// Set the degree adjustment factor-1 edge of B
	ctrl_ela_core->edge_adjust_b2 = IO_ELA.EDGSCLB.bit.EDGSCLB;				// Set the degree adjustment factor-2 edge of B

	ctrl_ela_core->edge_threshold[0] = IO_ELA.ALNGE.ALNGE1.bit.ALNGE_1;		// Edge degree threshold 1
	ctrl_ela_core->edge_threshold[1] = IO_ELA.ALNGE.ALNGE1.bit.ALNGE_2;		// Edge degree threshold 1
	ctrl_ela_core->edge_threshold[2] = IO_ELA.ALNGE.ALNGE2.bit.ALNGE_3;		// Edge degree threshold 1

	ctrl_ela_core->nr[0]  = IO_ELA.ALNG.ALNG1.bit.ALNGL1E1N;				// NR intensity :Luminance L1, Edge E1
	ctrl_ela_core->nr[1]  = IO_ELA.ALNG.ALNG1.bit.ALNGL1E2N;				// NR intensity :Luminance L1, Edge E2
	ctrl_ela_core->nr[2]  = IO_ELA.ALNG.ALNG2.bit.ALNGL1E3N;				// NR intensity :Luminance L1, Edge E3
	ctrl_ela_core->nr[3]  = IO_ELA.ALNG.ALNG2.bit.ALNGL2E1N;				// NR intensity :Luminance L2, Edge E1
	ctrl_ela_core->nr[4]  = IO_ELA.ALNG.ALNG3.bit.ALNGL2E2N;				// NR intensity :Luminance L2, Edge E2
	ctrl_ela_core->nr[5]  = IO_ELA.ALNG.ALNG3.bit.ALNGL2E3N;				// NR intensity :Luminance L2, Edge E3
	ctrl_ela_core->nr[6]  = IO_ELA.ALNG.ALNG4.bit.ALNGL3E1N;				// NR intensity :Luminance L3, Edge E1
	ctrl_ela_core->nr[7]  = IO_ELA.ALNG.ALNG4.bit.ALNGL3E2N;				// NR intensity :Luminance L3, Edge E2
	ctrl_ela_core->nr[8]  = IO_ELA.ALNG.ALNG5.bit.ALNGL3E3N;				// NR intensity :Luminance L3, Edge E3
	ctrl_ela_core->nr[9]  = IO_ELA.ALNG.ALNG5.bit.ALNGL4E1N;				// NR intensity :Luminance L4, Edge E1
	ctrl_ela_core->nr[10] = IO_ELA.ALNG.ALNG6.bit.ALNGL4E2N;				// NR intensity :Luminance L4, Edge E2
	ctrl_ela_core->nr[11] = IO_ELA.ALNG.ALNG6.bit.ALNGL4E3N;				// NR intensity :Luminance L4, Edge E3

	ctrl_ela_core->l_noise = IO_ELA.LFTH.bit.LFTH;							// L extraction threshold noise
	ctrl_ela_core->l_edge[0] = IO_ELA.LCOME.bit.LCOME1;						// Edge applicable threshold contour L 1
	ctrl_ela_core->l_edge[1] = IO_ELA.LCOME.bit.LCOME2;						// Edge applicable threshold contour L 2
	ctrl_ela_core->l_intense[0] = IO_ELA.LCOMEN.bit.LCOME1N;				// Applied intensity L 1
	ctrl_ela_core->l_intense[1] = IO_ELA.LCOMEN.bit.LCOME2N;				// Applied intensity L 2

	im_ela_off_pclk();

	return D_DDIM_OK;
}

/**
 * @brief		The register at OB Correction control is get
 * @param[out]	T_IM_ELA_CTRL_OB_CORRECTION* ctrl_ela_ob
 * @return		INT32 D_DDIM_OK / D_IM_ELA_INPUT_PARAM_ERROR
 * @note		None
 * @attention	None
 */
UINT32 Im_ELA_Get_Ctrl_Ob_Correction( T_IM_ELA_CTRL_OB_CORRECTION* const ctrl_ela_ob )
{
#ifdef CO_PARAM_CHECK
	/* check on input pointer */
	if( ctrl_ela_ob == NULL ){
		Ddim_Assertion(("Im_ELA_Get_Ctrl_Ob_Correction() error. ctrl_ela_ob = NULL\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_ela_on_pclk();

	// OB Correction R Pixel
	im_ela_get_reg_signed(ctrl_ela_ob->ob_rr, IO_ELA.OBOF.OBOF1, union io_ela_obof_1, EOBRR);

	// OB Correction B Pixel
	im_ela_get_reg_signed(ctrl_ela_ob->ob_bb, IO_ELA.OBOF.OBOF1, union io_ela_obof_1, EOBBB);

	// OB Correction Gr Pixel
	im_ela_get_reg_signed(ctrl_ela_ob->ob_gr, IO_ELA.OBOF.OBOF2, union io_ela_obof_2, EOBGR);

	// OB Correction Gb Pixel
	im_ela_get_reg_signed(ctrl_ela_ob->ob_gb, IO_ELA.OBOF.OBOF2, union io_ela_obof_2, EOBGB);

	im_ela_off_pclk();

	return D_DDIM_OK;
}

/**
 * @brief		Get Optical zero level shift parameter
 * @param[in]	None
 * @return		Optical zero level shift parameter.(Two's complement)
 * @note		None
 * @attention	None
 */
USHORT Im_ELA_Get_Optical_Zero_Level_Shift( VOID )
{
	USHORT Optical_Zero_Level_Shift;

	im_ela_on_pclk();

	im_ela_get_reg_signed(Optical_Zero_Level_Shift, IO_ELA.EZSFTA1, union io_ela_ezsfta1, EZSFTA1);

	im_ela_off_pclk();

	return Optical_Zero_Level_Shift;
}


/**
 * @brief		Set address of parameter for multiple times execution
 * @param[in]	T_IM_ELA_MULTIPLE_TIMES_INFO* multiple_exec_info
 * @return		INT32 D_DDIM_OK / D_IM_ELA_INPUT_PARAM_ERROR
 * @note		None
 * @attention	This API calls before Im_ELA_Start() or Im_ELA_Start_Async().
 */
UINT32 Im_ELA_Set_Multiple_Exec_Info( const T_IM_ELA_MULTIPLE_TIMES_INFO* const multiple_exec_info )
{
#ifdef CO_PARAM_CHECK
	/* check on input pointer */
	if( multiple_exec_info == NULL ){
		Ddim_Assertion(("Im_ELA_Set_Multiple_Exec_Info() error. multiple_exec_info = NULL\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_ela_on_pclk();

	memcpy( (T_IM_ELA_MULTIPLE_TIMES_INFO*)&gIM_ELA_Multiple_Exec_Info, multiple_exec_info, sizeof(T_IM_ELA_MULTIPLE_TIMES_INFO) );

	/* multiple times execution parameter for 1st time*/
	if( gIM_ELA_Multiple_Exec_Info.core[0] != NULL ){
		im_ela_ctrl_core_register( gIM_ELA_Multiple_Exec_Info.core[0] );
	}

	im_ela_off_pclk();

	return D_DDIM_OK;
}

/**
 * @brief		Start the operation of ELA's sync
 * @param[in]	None
 * @return		D_DDIM_OK / D_IM_ELA_TIMEOUT / D_IM_ELA_NG / D_IM_ELA_MACRO_BUSY
 * @note		None
 * @attention	None
 */

INT32 Im_ELA_Start_Sync( VOID )
{
	INT32 ret;

	ret = im_ela_start_core( D_IM_ELA_SYNC );

	return ret;
}

/**
 * @brief		Start the operation of ELA's async
 * @param[in]	None
 * @return		D_DDIM_OK / D_IM_ELA_MACRO_BUSY
 * @note		None
 * @attention	None
 */
INT32 Im_ELA_Start_Async( VOID )
{
	INT32 ret;

	ret = im_ela_start_core( D_IM_ELA_ASYNC );

	return ret;
}

/**
 * @brief		The processing of ELA stop
 * @param		None
 * @return		None
 * @note		None
 * @attention	None
 */
VOID Im_ELA_Stop( VOID )
{
	DDIM_USER_ER ercd;

	im_ela_on_pclk();

	/* ELA stop */
	if( IO_ELA.ELATRG.bit.ELATRG == 0x3 ){
		/* ELA stop */
		IO_ELA.ELATRG.bit.ELATRG = 0;
	}

	// Disable SRAM ACCESS
	IO_ELA.SRAMACT.bit.SRAMACT = 0;

	im_ela_off_pclk();
	im_ela_off_iclk();
	im_ela_off_clk();

	ercd = DDIM_User_Clr_Flg( FID_IM_ELA, ~D_IM_ELA_FLG_ELA );
	if( ercd != D_DDIM_USER_E_OK ){
		Ddim_Print(("Im_ELA_Stop() system error. ercd = %d\n", ercd));
	}
}

/**
 * @brief		Wait that the asynchronization processing ends
 * @param[in]	None
 * @return		D_DDIM_OK / D_IM_ELA_TIMEOUT / D_IM_ELA_NG
 * @note		When the executed asynchronization processing ends, it is a return.
 * @attention	None
 */
INT32 Im_ELA_Wait_End( VOID )
{
	DDIM_USER_FLGPTN flg_ptn;
	DDIM_USER_ER ercd;
	INT32 ret = D_DDIM_OK;

	ercd = DDIM_User_Twai_Flg( FID_IM_ELA, D_IM_ELA_FLG_ELA, D_DDIM_USER_TWF_ORW, &flg_ptn, D_IM_ELA_WAIT_END_TIME );
	if( ercd == D_DDIM_USER_E_TMOUT ){
		Ddim_Print(("im_ela_start_core() time out. ercd = %d\n", ercd));
		ret = D_IM_ELA_TIMEOUT;
	}
	else if( ercd != D_DDIM_USER_E_OK ){
		Ddim_Print(("im_ela_start_core() system error. ercd = %d\n", ercd));
		ret = D_IM_ELA_NG;
	}
	else{
		(VOID)DDIM_User_Clr_Flg( FID_IM_ELA, ~D_IM_ELA_FLG_ELA );
	}

	return ret;
}

/**
 * @brief		It is an interrupt handler that starts when the processing of ELA ends
 * @param		None
 * @return		None
 * @note		None
 * @attention	None
 */
VOID Im_ELA_Int_Handler( VOID )
{
	union io_ela_elainte elainte;
	union io_ela_elaintf elaintf;

	gIM_ELA_Axi_Err = D_IM_ELA_NORMAL_END;

	im_ela_on_pclk();
	Im_ELA_Dsb();

#ifdef CO_ELA_DEBUG_PRINT	// Debug
	Ddim_Print(("Im_ELA_Int_Handler() : Stage=%d, Step=%d\n", gIM_ELA_Stage_Cnt, gIM_ELA_Step_Cnt));
	Ddim_Print(("Im_ELA_Int_Handler() : gIM_ELA_Next_Ctrl_Flag = %d\n", gIM_ELA_Next_Ctrl_Flag));
	Ddim_Print(( "\tELACNT = %d\n",			IO_ELA.ELACTL.bit.ELACNT ));
	Ddim_Print(( "\tELAMD  = %d\n",			IO_ELA.ELACTL.bit.ELAMD ));
	Ddim_Print(( "\tELAACT = %d\n",			IO_ELA.ELACTL.bit.ELAACT ));
	Ddim_Print(( "\tEIWOUT = %d\n",			IO_ELA.ELABYR.bit.EIWOUT ));
	Ddim_Print(( "\tEIRA    = 0x%08lx\n",	IO_ELA.EIRA.word ));
	{
		UINT32 i;
		for( i = 0; i < 4; i++ ){
			Ddim_Print(( "\tEIWA[%d] = 0x%08lx\n",	i, IO_ELA.EIWA.word[i] ));
		}
		Ddim_Print(( "\tENWA   = 0x%08lx\n",		IO_ELA.ENWA.word ));
		for( i = 0; i < 16; i++ ){
			Ddim_Print(( "\tENRA[%d] = 0x%08lx\n",	i, IO_ELA.ENRA.word[i] ));
		}
	}
	Ddim_Print(( "\tENSA    = 0x%08lx\n",			IO_ELA.ENSA.word ));

	im_ela_qelatmd_info_print();
#endif
	elainte.word = IO_ELA.ELAINTE.word;
	elaintf.word = IO_ELA.ELAINTF.word;

	/* interrupt AXI Write Ch */
	if( (elainte.bit.AXWERE == 1) && (elaintf.bit.__AXWERF == 1) ){		/* pgr0872 */
		IO_ELA.ELAINTF.word = D_IM_ELA_ELAINTF_AXWERF;	// flag clear
		gIM_ELA_Axi_Err |= D_IM_ELA_AXI_ERR_CH_W;
	}

	/* interrupt AXI Read Ch */
	if( (elainte.bit.AXRERE == 1) && (elaintf.bit.__AXRERF == 1) ){		/* pgr0872 */
		IO_ELA.ELAINTF.word = D_IM_ELA_ELAINTF_AXRERF;	// flag clear
		gIM_ELA_Axi_Err |= D_IM_ELA_AXI_ERR_CH_R;
	}

	/* interrupt Process completed */
	if( (elainte.bit.ENWE == 1) && (elaintf.bit.__ENWF == 1) ){			/* pgr0872 */
		IO_ELA.ELAINTF.word = D_IM_ELA_ELAINTF_ENWF;	// flag clear
	}

	(VOID)DDIM_User_Set_Flg( FID_IM_ELA, D_IM_ELA_FLG_ELA );

	// User Function Check
	if( gIM_ELA_CallBack_Func != NULL ){
		gIM_ELA_CallBack_Func( gIM_ELA_Axi_Err );
	}

	im_ela_off_pclk();
	Im_ELA_Dsb();
}

/**
 * @brief		Cancel the LUT sleep for Deknee.
 * @param[in]	UCHAR deknee_enable
 * @return		None
 * @note		None
 * @attention	None
 */
VOID Im_ELA_Set_Deknee_Lut_Sleep( UCHAR deknee_enable )
{
#ifdef CO_PARAM_CHECK
	if( deknee_enable > D_IM_ELA_ENABLE ){
		Ddim_Assertion(("Im_ELA_Set_Deknee_Lut_Sleep() error. input deknee_enable error : %d\n", deknee_enable));
		return ;
	}
#endif // CO_PARAM_CHECK

	im_ela_on_pclk();

	if( deknee_enable == D_IM_ELA_ENABLE ){
		IO_ELA.ELABYR.bit.DKNACT = 1;
	}
	else{
		IO_ELA.ELABYR.bit.DKNACT = 0;
	}

	im_ela_off_pclk();
}

/**
 * @brief		The register setting of NSLMD is acquired.
 * @param[out]	T_IM_ELA_CTRL_NSLMD* const nslmd
 * @return		INT32 D_DDIM_OK / D_IM_ELA_INPUT_PARAM_ERROR
 * @note		None
 * @attention	None
 */
UINT32  Im_ELA_Get_Nslmd( T_IM_ELA_CTRL_NSLMD* const nslmd )
{
#ifdef CO_PARAM_CHECK
	/* check on input pointer */
	if( nslmd == NULL ){
		Ddim_Assertion(("Im_ELA_Get_Nslmd() error. nslmd = NULL\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK
	im_ela_on_pclk();

	nslmd->noise_data_mode				= IO_ELA.NSLMD.bit.NSLMD;
	nslmd->noise_data_precision			= IO_ELA.NSLMD.bit.NSLRES;
	nslmd->noise_data_compress_enable	= IO_ELA.NSLMD.bit.NSLKNE;

	im_ela_off_pclk();

	return D_DDIM_OK;
}

/**
 * @brief		The register is set for EAch.
 * @param[in]	T_IM_ELA_CTRL_EACH* const ctrl_each
 * @return		INT32 D_DDIM_OK / D_IM_ELA_INPUT_PARAM_ERROR
 * @note		None
 * @attention	None
 */
UINT32 Im_ELA_Set_EAch( const T_IM_ELA_CTRL_EACH* const ctrl_each )
{
#ifdef CO_PARAM_CHECK
	/* check on input pointer */
	if( ctrl_each == NULL ){
		Ddim_Assertion(("Im_ELA_Set_EAch() error. ctrl_each = NULL\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK
	im_ela_on_pclk();

	IO_ELA.EARCACHE.bit.EARCACHE_0		= ctrl_each->cache_type_r[0];
	IO_ELA.EARCACHE.bit.EARCACHE_1		= ctrl_each->cache_type_r[1];
	IO_ELA.EARCACHE.bit.EARCACHE_2		= ctrl_each->cache_type_r[2];

	IO_ELA.EAWCACHE.bit.EAWCACHE_0		= ctrl_each->cache_type_w[0];
	IO_ELA.EAWCACHE.bit.EAWCACHE_1		= ctrl_each->cache_type_w[1];
	IO_ELA.EAWCACHE.bit.EAWCACHE_2		= ctrl_each->cache_type_w[2];

	IO_ELA.EARPROT.bit.EARPROT_0		= ctrl_each->protect_type_r[0];
	IO_ELA.EARPROT.bit.EARPROT_1		= ctrl_each->protect_type_r[1];
	IO_ELA.EARPROT.bit.EARPROT_2		= ctrl_each->protect_type_r[2];

	IO_ELA.EAWPROT.bit.EAWPROT_0		= ctrl_each->protect_type_w[0];
	IO_ELA.EAWPROT.bit.EAWPROT_1		= ctrl_each->protect_type_w[1];
	IO_ELA.EAWPROT.bit.EAWPROT_2		= ctrl_each->protect_type_w[2];

	IO_ELA.EARREQMSK.EARREQMSK1.bit.EARREQMSK_0	= ctrl_each->req_mask_r[0];
	IO_ELA.EARREQMSK.EARREQMSK1.bit.EARREQMSK_1	= ctrl_each->req_mask_r[1];
	IO_ELA.EARREQMSK.EARREQMSK2.bit.EARREQMSK_2	= ctrl_each->req_mask_r[2];

	IO_ELA.EAWREQMSK.EAWREQMSK1.bit.EAWREQMSK_0	= ctrl_each->req_mask_w[0];
	IO_ELA.EAWREQMSK.EAWREQMSK1.bit.EAWREQMSK_1	= ctrl_each->req_mask_w[1];
	IO_ELA.EAWREQMSK.EAWREQMSK2.bit.EAWREQMSK_2	= ctrl_each->req_mask_w[2];

	im_ela_off_pclk();

	return D_DDIM_OK;
}

/**
 * @brief		The register setting of EAch is acquired.
 * @param[out]	T_IM_ELA_CTRL_EACH* const ctrl_each
 * @return		INT32 D_DDIM_OK / D_IM_ELA_INPUT_PARAM_ERROR
 * @note		None
 * @attention	None
 */
UINT32 Im_ELA_Get_EAch( T_IM_ELA_CTRL_EACH* const ctrl_each )
{
#ifdef CO_PARAM_CHECK
	/* check on input pointer */
	if( ctrl_each == NULL ){
		Ddim_Assertion(("Im_ELA_Get_EAch() error. ctrl_each = NULL\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK
	im_ela_on_pclk();

	ctrl_each->cache_type_r[0]		= IO_ELA.EARCACHE.bit.EARCACHE_0;
	ctrl_each->cache_type_r[1]		= IO_ELA.EARCACHE.bit.EARCACHE_1;
	ctrl_each->cache_type_r[2]		= IO_ELA.EARCACHE.bit.EARCACHE_2;

	ctrl_each->cache_type_w[0]		= IO_ELA.EAWCACHE.bit.EAWCACHE_0;
	ctrl_each->cache_type_w[1]		= IO_ELA.EAWCACHE.bit.EAWCACHE_1;
	ctrl_each->cache_type_w[2]		= IO_ELA.EAWCACHE.bit.EAWCACHE_2;

	ctrl_each->protect_type_r[0]	= IO_ELA.EARPROT.bit.EARPROT_0;
	ctrl_each->protect_type_r[1]	= IO_ELA.EARPROT.bit.EARPROT_1;
	ctrl_each->protect_type_r[2]	= IO_ELA.EARPROT.bit.EARPROT_2;

	ctrl_each->protect_type_w[0]	= IO_ELA.EAWPROT.bit.EAWPROT_0;
	ctrl_each->protect_type_w[1]	= IO_ELA.EAWPROT.bit.EAWPROT_1;
	ctrl_each->protect_type_w[2]	= IO_ELA.EAWPROT.bit.EAWPROT_2;

	ctrl_each->req_mask_r[0]		= IO_ELA.EARREQMSK.EARREQMSK1.bit.EARREQMSK_0;
	ctrl_each->req_mask_r[1]		= IO_ELA.EARREQMSK.EARREQMSK1.bit.EARREQMSK_1;
	ctrl_each->req_mask_r[2]		= IO_ELA.EARREQMSK.EARREQMSK2.bit.EARREQMSK_2;

	ctrl_each->req_mask_w[0]		= IO_ELA.EAWREQMSK.EAWREQMSK1.bit.EAWREQMSK_0;
	ctrl_each->req_mask_w[1]		= IO_ELA.EAWREQMSK.EAWREQMSK1.bit.EAWREQMSK_1;
	ctrl_each->req_mask_w[2]		= IO_ELA.EAWREQMSK.EAWREQMSK2.bit.EAWREQMSK_2;

	im_ela_off_pclk();

	return D_DDIM_OK;
}

/**
 * @brief		The register setting of AXI status is acquired.
 * @param[out]	T_IM_ELA_EACH_AXI* const each_axi
 * @return		INT32 D_DDIM_OK / D_IM_ELA_INPUT_PARAM_ERROR
 * @note		None
 * @attention	None
 */
UINT32 Im_ELA_Get_AXI_Status( T_IM_ELA_EACH_AXI* const each_axi )
{
#ifdef CO_PARAM_CHECK
	/* check on input pointer */
	if( each_axi == NULL ){
		Ddim_Assertion(("Im_ELA_Get_AXI_Status() error. each_axi = NULL\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK
	im_ela_on_pclk();

	each_axi->axi_reply_r[0]	= IO_ELA.EARAXSTS.bit.RRESP_0;
	each_axi->axi_reply_r[1]	= IO_ELA.EARAXSTS.bit.RRESP_1;
	each_axi->axi_reply_r[2]	= IO_ELA.EARAXSTS.bit.RRESP_2;

	each_axi->axi_reply_w[0]	= IO_ELA.EAWAXSTS.bit.BRESP_0;
	each_axi->axi_reply_w[1]	= IO_ELA.EAWAXSTS.bit.BRESP_1;
	each_axi->axi_reply_w[2]	= IO_ELA.EAWAXSTS.bit.BRESP_2;

	im_ela_off_pclk();

	return D_DDIM_OK;
}

/**
 * @brief		Set Deknee table.
 * @param[in]	USHORT* const src_tbl
 * @param[in]	USHORT write_ofs_num
 * @param[in]	USHORT array_num
 * @return		INT32 D_DDIM_OK / D_IM_ELA_INPUT_PARAM_ERROR / D_IM_ELA_MACRO_BUSY
 * @note		None
 * @attention	None
 */
UINT32 Im_ELA_Set_Deknee_Tbl( const USHORT* const src_tbl, USHORT write_ofs_num, USHORT array_num )
{
	UINT32 loop_cnt;
	ULONG deknee_tbl[D_IM_ELA_DEKNEE_TBL_MAX / 2];

#ifdef CO_PARAM_CHECK
	// check on input pointer
	if( src_tbl == NULL ){
		// Parameter setting error
		Ddim_Assertion(("I:Im_ELA_Set_Deknee_Tbl error. src_tbl = NULL\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
	if( (write_ofs_num + array_num) > D_IM_ELA_DEKNEE_TBL_MAX ) {
		// Parameter setting error
		Ddim_Assertion(("I:Im_ELA_Set_Deknee_Tbl error. write_ofs_num + array_num > MAX\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	im_ela_on_pclk();
	IO_ELA.ELABYR.bit.DKNACT = 1;

	if( IO_ELA.ELATRG.bit.ELATRG == 0x3 ){
		return D_IM_ELA_MACRO_BUSY;
	}

	for( loop_cnt = 0; loop_cnt < (D_IM_ELA_DEKNEE_TBL_MAX / 2); loop_cnt++ ){
		deknee_tbl[loop_cnt] = IO_ELA.DKNTBL.word[loop_cnt];
	}

	for( loop_cnt = write_ofs_num; loop_cnt < array_num; loop_cnt++ ){
		if( (loop_cnt % 2) == 0 ){
			deknee_tbl[loop_cnt / 2] &= 0xFFFF0000;
			deknee_tbl[loop_cnt / 2] |= src_tbl[loop_cnt];
		}
		else{
			deknee_tbl[loop_cnt / 2] &= 0x0000FFFF;
			deknee_tbl[loop_cnt / 2] |= src_tbl[loop_cnt] << 16;
		}
	}

	for( loop_cnt = 0; loop_cnt < (D_IM_ELA_DEKNEE_TBL_MAX / 2); loop_cnt++ ){
		IO_ELA.DKNTBL.word[loop_cnt] = deknee_tbl[loop_cnt];
	}

	IO_ELA.ELABYR.bit.DKNACT = 0;
	im_ela_off_pclk();

	return D_DDIM_OK;
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
/**
 * @brief		Set curtail bayer data table
 * @param[in]	curtail_bayer[D_IM_ELA_STAGE_MAX]	Pointer of Table data (Type of T_IM_ELA_CURTAIL_BAYER_DATA)
 * @return		INT32 D_DDIM_OK / D_IM_ELA_INPUT_PARAM_ERROR
 * @note		None
 * @attention	None
 */
UINT32 Im_ELA_Set_Curtail_Bayer_Data_Tbl( const T_IM_ELA_CURTAIL_BAYER_DATA curtail_bayer[D_IM_ELA_STAGE_MAX] )
{
#ifdef CO_PARAM_CHECK
	/* check on input pointer */
	if( curtail_bayer == NULL ){
		Ddim_Assertion(("Im_ELA_Set_Curtail_Bayer_Data_Tbl() error. curtail_bayer = NULL\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	memcpy( &gIM_ELA_Curtail_Bayer, curtail_bayer, (sizeof(T_IM_ELA_CURTAIL_BAYER_DATA) * D_IM_ELA_STAGE_MAX) );
	return D_DDIM_OK;
}

/**
 * @brief		Get curtail bayer data table
 * @param[out]	curtail_bayer[D_IM_ELA_STAGE_MAX]	Pointer of Table data (Type of T_IM_ELA_CURTAIL_BAYER_DATA)
 * @return		INT32 D_DDIM_OK / D_IM_ELA_INPUT_PARAM_ERROR
 * @note		None
 * @attention	None
 */
UINT32 Im_ELA_Get_Curtail_Bayer_Data_Tbl( T_IM_ELA_CURTAIL_BAYER_DATA curtail_bayer[D_IM_ELA_STAGE_MAX] )
{
#ifdef CO_PARAM_CHECK
	/* check on input pointer */
	if( curtail_bayer == NULL ){
		Ddim_Assertion(("Im_ELA_Get_Curtail_Bayer_Data_Tbl() error. curtail_bayer = NULL\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	memcpy( curtail_bayer, &gIM_ELA_Curtail_Bayer, (sizeof(T_IM_ELA_CURTAIL_BAYER_DATA) * D_IM_ELA_STAGE_MAX) );
	return D_DDIM_OK;
}

/**
 * @brief		Set extraction noise data table
 * @param[in]	extract_noise[D_IM_ELA_STAGE_MAX]	Pointer of Table data (Type of T_IM_ELA_EXTRACT_NOISE_DATA)
 * @return		INT32 D_DDIM_OK / D_IM_ELA_INPUT_PARAM_ERROR
 * @note		None
 * @attention	None
 */
UINT32 Im_ELA_Set_Extract_Noise_Data_Tbl( const T_IM_ELA_EXTRACT_NOISE_DATA extract_noise[D_IM_ELA_STAGE_MAX] )
{
#ifdef CO_PARAM_CHECK
	/* check on input pointer */
	if( extract_noise == NULL ){
		Ddim_Assertion(("Im_ELA_Set_Extract_Noise_Data_Tbl() error. extract_noise = NULL\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	memcpy( &gIM_ELA_Extract_Noise, extract_noise, (sizeof(T_IM_ELA_EXTRACT_NOISE_DATA) * D_IM_ELA_STAGE_MAX) );
	return D_DDIM_OK;
}


/**
 * @brief		Get extraction noise data table
 * @param[out]	extract_noise[D_IM_ELA_STAGE_MAX]	Pointer of Table data (Type of T_IM_ELA_EXTRACT_NOISE_DATA)
 * @return		INT32 D_DDIM_OK / D_IM_ELA_INPUT_PARAM_ERROR
 * @note		None
 * @attention	None
 */
UINT32 Im_ELA_Get_Extract_Noise_Data_Tbl( T_IM_ELA_EXTRACT_NOISE_DATA extract_noise[D_IM_ELA_STAGE_MAX] )
{
#ifdef CO_PARAM_CHECK
	/* check on input pointer */
	if( extract_noise == NULL ){
		Ddim_Assertion(("Im_ELA_Get_Extract_Noise_Data_Tbl() error. extract_noise = NULL\n"));
		return D_IM_ELA_INPUT_PARAM_ERROR;
	}
#endif // CO_PARAM_CHECK

	memcpy( extract_noise, &gIM_ELA_Extract_Noise, (sizeof(T_IM_ELA_EXTRACT_NOISE_DATA) * D_IM_ELA_STAGE_MAX) );
	return D_DDIM_OK;
}



//---------------------------- colabo section ----------------------------
// Nothing Special
#endif
