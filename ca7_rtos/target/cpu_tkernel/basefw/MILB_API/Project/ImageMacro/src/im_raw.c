/**
 * @file		im_raw.c
 * @brief		RAW3A Driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "jdsraw.h"
#include "im_raw.h"

#include "dd_top.h"

//#include "dd_user_custom.h"
#include "ddim_user_custom.h"
#include "driver_common.h"

// #define CO_IM_RAW_DEBUG

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Low power consumption option

#ifdef CO_ACT_CLOCK
#define CO_ACT_RAWCLK
#endif
#ifdef CO_ACT_ICLOCK
#define CO_ACT_RAW_ICLK
#endif
#ifdef CO_ACT_PCLOCK
#define CO_ACT_RAW_PCLK
#endif

#ifdef CO_ACT_RAWCLK
#define D_IM_RAW_RAWCK_BIT		(D_DD_TOP_RAWCK_BIT)
#endif
#ifdef CO_ACT_RAW_PCLK
#define D_IM_RAW_RAWAP_BIT		(D_DD_TOP_RAWAP_BIT)
#endif
#ifdef CO_ACT_RAW_ICLK
#define D_IM_RAW_RAWAX_BIT		(D_DD_TOP_RAWAX_BIT)
#endif


#define	D_IM_RAW_FALSE			(0)
#define	D_IM_RAW_TRUE			(1)

#define	D_IM_RAW_RTRG_STOP		(0)
#define	D_IM_RAW_RTRG_START		(1)
#define	D_IM_RAW_RTRG_STOPPED	(2)
#define	D_IM_RAW_RTRG_RUNNING	(3)

#define	D_IM_RAW_FLG_END		(0x00000001)
#define	D_IM_RAW_FLG_STOP		(0x00000002)
#define	D_IM_RAW_FLG_R_AXI_ERR	(0x00000004)
#define	D_IM_RAW_FLG_W_AXI_ERR	(0x00000008)

#define	D_IM_RAW_INT_R_BIT		(0x00000001)	// RE, RF bit
#define	D_IM_RAW_INT_RAXER_BIT	(0x00000010)	// RAXERE, RAXERF bit
#define	D_IM_RAW_INT_WAXER_BIT	(0x00000020)	// WAXERE, WAXERF bit

#define D_IM_RAW_SRAM_WAIT_USEC	(1)

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#ifdef CO_PARAM_CHECK
#define im_raw_check_pointer(ptr)			(( ptr != NULL ) ? D_IM_RAW_TRUE : D_IM_RAW_FALSE)
#define im_raw_check_wait_time(wait_time)	(( wait_time >= D_DDIM_USER_SEM_WAIT_FEVR ) ? D_IM_RAW_TRUE : D_IM_RAW_FALSE)
#endif

#define Im_RAW_Dsb()				Dd_ARM_Dsb_Pou()
#define Im_RAW_Wait_usec( usec )	Dd_ARM_Wait_ns( (usec * 1000) )

/*----------------------------------------------------------------------*/
/* Global variables														*/
/*----------------------------------------------------------------------*/
#ifdef CO_ACT_RAWCLK
static volatile UCHAR gIM_RAW_RAWCLK_Counter = 0;
#endif
#ifdef CO_ACT_RAW_ICLK
static volatile UCHAR gIM_RAW_ICLK_Counter = 0;
#endif
#ifdef CO_ACT_RAW_PCLK
static volatile UCHAR gIM_RAW_PCLK_Counter = 0;
#endif

static volatile UINT32 gIM_RAW_COND;

static VOID (*gIM_RAW_Callback)(UINT32) = NULL;

/*----------------------------------------------------------------------*/
/* Local function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief			RAWCLK ON
 * @param[in]		None
 * @param[out]		None
 * @retval			None
 * @note			None
 */
static VOID im_raw_on_rawclk( VOID )
{
#ifdef CO_ACT_RAWCLK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_RAW_RAWCLK_Counter, &Dd_Top_Get_CLKSTOP7(), ~((ULONG)D_IM_RAW_RAWCK_BIT) );
#endif
}

/**
 * @brief			RAWCLK OFF
 * @param[in]		None
 * @param[out]		None
 * @retval			None
 * @note			None
 */
static VOID im_raw_off_rawclk( VOID )
{
#ifdef CO_ACT_RAWCLK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_RAW_RAWCLK_Counter, &Dd_Top_Get_CLKSTOP7(), ((ULONG)D_IM_RAW_RAWCK_BIT) );
#endif
}

/**
 * @brief			RAWICLK ON
 * @param[in]		None
 * @param[out]		None
 * @retval			None
 * @note			None
 */
static VOID im_raw_on_rawiclk( VOID )
{
#ifdef CO_ACT_RAW_ICLK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_RAW_ICLK_Counter, &Dd_Top_Get_CLKSTOP7(), ~((ULONG)D_IM_RAW_RAWAX_BIT) );
#endif
}

/**
 * @brief			RAWICLK OFF
 * @param[in]		None
 * @param[out]		None
 * @retval			None
 * @note			None
 */
static VOID im_raw_off_rawiclk( VOID )
{
#ifdef CO_ACT_RAW_ICLK
	Dd_Top_Stop_Clock( (UCHAR*)&gIM_RAW_ICLK_Counter, &Dd_Top_Get_CLKSTOP7(), ((ULONG)D_IM_RAW_RAWAX_BIT) );
#endif
}

/**
 * @brief			Setup parameters
 * @param[in]   	T_IM_RAW_CTRL_PARAM* const cfg	: control data
 * @param[out]		None
 * @retval			None
 * @note			None
 */
static VOID im_raw_setup_param( const T_IM_RAW_CTRL_PARAM* const cfg )
{
	INT32 i;
	volatile UINT32 *pLUT;

	// Common
	IO_RAW.RINTE.bit.RE		= 1;
	IO_RAW.RINTE.bit.RAXERE	= 1;
	IO_RAW.RINTE.bit.WAXERE	= 1;

	IO_RAW.MSA.bit.MSA			= cfg->src_addr;
	IO_RAW.MDA.bit.MDA			= cfg->dst_addr;
	IO_RAW.RCTL1.bit.RFMT		= cfg->data_format;
	IO_RAW.RCTL1.bit.BYTS		= cfg->byte_stuffing;
	IO_RAW.DEFINIT.bit.DEFINIT	= cfg->diff_fixed_value;

	IO_RAW.MRAXCTL.bit.MRCACHE	= cfg->axi_param.r_cache_type;
	IO_RAW.MRAXCTL.bit.MRPROT	= cfg->axi_param.r_protection_type;
	IO_RAW.MWAXCTL.bit.MWCACHE	= cfg->axi_param.w_cache_type;
	IO_RAW.MWAXCTL.bit.MWPROT	= cfg->axi_param.w_protection_type;

	gIM_RAW_Callback = cfg->callback;

	// Only 12/14/16 bit
	if( (cfg->data_format == D_IM_RAW_RFMT_14_OR_16_BIT)||(cfg->data_format == D_IM_RAW_RFMT_12_BIT_PACK) ){
		IO_RAW.HSIZE.bit.HSIZE	= cfg->width;
		IO_RAW.VSIZE.bit.VSIZE	= cfg->lines;
		IO_RAW.RCTL1.bit.DEFFC	= cfg->mcu_size;
		if( cfg->data_format == D_IM_RAW_RFMT_12_BIT_PACK ){
			IO_RAW.RCTL1.bit.BITSEL = D_IM_RAW_BITSEL_EXCEPT_16_BIT;
		}
		else{
			IO_RAW.RCTL1.bit.BITSEL = cfg->bit_select;
		}
		IO_RAW.RCTL1.bit.LUTEN		= D_IM_RAW_OFF;
		IO_RAW.RCTL1.bit.DEFOP		= D_IM_RAW_DEFOP_DIFF_PREV_VALUE;
	}
	else{	// D_IM_RAW_RFMT_8_BIT
		IO_RAW.DSIZE.bit.DSIZE	= cfg->data_size;
		IO_RAW.RCTL1.bit.DEFOP	= cfg->diff_mode;
		IO_RAW.RCTL1.bit.BITSEL	= D_IM_RAW_BITSEL_EXCEPT_16_BIT;

		if( cfg->lut == NULL ){
			IO_RAW.RCTL1.bit.LUTEN		= D_IM_RAW_OFF;
		}
		else{
			// set RAW macro enable (active mode)
			IO_RAW.TBLAEN.bit.TBLAEN	= D_IM_RAW_TBLAEN_ON;
			// wait 1us
			Im_RAW_Wait_usec( D_IM_RAW_SRAM_WAIT_USEC );

			IO_RAW.RCTL1.bit.LUTEN		= D_IM_RAW_ON;
			pLUT = (volatile UINT32*)&IO_RAW.LUT.LUT1.word;
			for( i = 0; i < 256; i+=4 ){
				*pLUT++ = (  (cfg->lut[i+3] << 24)
						   | (cfg->lut[i+2] << 16)
						   | (cfg->lut[i+1] <<  8)
						   | (cfg->lut[i+0] <<  0) );
			}

			// RAW macro disable (power save mode)
			IO_RAW.TBLAEN.bit.TBLAEN	= D_IM_RAW_TBLAEN_OFF;
		}
	}
}

/**
 * @brief			Wait that the asynchronization processing ends
 * @param[in]   	DDIM_USER_FLGPTN* flgptn	: wait flag pattern
 * @param[in]   	INT32 wait_time				: wait time
 * @param[out]		None
 * @retval			When the executed asynchronization processing ends, it is a return.
 * @note			None
 */
static INT32 im_raw_wait_end( DDIM_USER_FLGPTN* flgptn, INT32 wait_time )
{
	DDIM_USER_ER		ercd;
	DDIM_USER_FLGPTN	waitptn;

	waitptn = ( D_IM_RAW_FLG_END | D_IM_RAW_FLG_STOP | D_IM_RAW_FLG_R_AXI_ERR | D_IM_RAW_FLG_W_AXI_ERR );
	ercd = DDIM_User_Twai_Flg( FID_IM_RAW, waitptn, D_DDIM_USER_TWF_ORW, flgptn, (DDIM_USER_TMO)wait_time );
	if( ercd == D_DDIM_USER_E_TMOUT ){
		return D_IM_RAW_RETVAL_TIME_OUT;
	}
	if( ercd != D_DDIM_USER_E_OK ){
		return D_IM_RAW_RETVAL_SYSTEM_ERROR;
	}

	ercd = DDIM_User_Clr_Flg( FID_IM_RAW, ~(waitptn & (*flgptn)) );
	if( ercd != D_DDIM_USER_E_OK ){
		return D_IM_RAW_RETVAL_SYSTEM_ERROR;
	}

	// Read AXI Error Check
	if( ((*flgptn) & (DDIM_USER_FLGPTN)D_IM_RAW_FLG_R_AXI_ERR) != 0 ){
		return D_IM_RAW_RETVAL_R_AXI_ERR;
	}
	// Write AXI Error Check
	if( ((*flgptn) & (DDIM_USER_FLGPTN)D_IM_RAW_FLG_W_AXI_ERR) != 0 ){
		return D_IM_RAW_RETVAL_W_AXI_ERR;
	}

	return D_IM_RAW_RETVAL_OK;
}

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
 * @brief			APB Bus clock ON for RAW
 * @param[in]   	None
 * @param[out]		None
 * @retval			None
 * @note			None
 */
VOID Im_RAW_On_Pclk( VOID )
{
#ifdef CO_ACT_RAW_PCLK
	Dd_Top_Start_Clock( (UCHAR*)&gIM_RAW_PCLK_Counter, &Dd_Top_Get_CLKSTOP7(), ~((ULONG)D_IM_RAW_RAWAP_BIT) );

	// wait
	IO_RAW.CCNT.word;	/* pgr0695 */
#endif
}

/**
 * @brief			APB Bus clock OFF for RAW
 * @param[in]   	None
 * @param[out]		None
 * @retval			None
 * @note			None
 */
VOID Im_RAW_Off_Pclk( VOID )
{
#ifdef CO_ACT_RAW_PCLK
	// wait
	IO_RAW.CCNT.word;	/* pgr0695 */

	Dd_Top_Stop_Clock( (UCHAR*)&gIM_RAW_PCLK_Counter, &Dd_Top_Get_CLKSTOP7(), ((ULONG)D_IM_RAW_RAWAP_BIT) );
#endif
}

/**
 * @brief			Software-reset and release software reset.
 * @param[in]   	None
 * @param[out]		None
 * @retval			D_IM_RAW_RETVAL_OK, D_IM_RAW_RETVAL_MACRO_BUSY
 * @note			None
 */
INT32 Im_RAW_Init( VOID )
{
	INT32	retval;

	im_raw_on_rawclk();
	im_raw_on_rawiclk();
	Im_RAW_On_Pclk();
	Im_RAW_Dsb();

	if( IO_RAW.RCTL1.bit.RTRG == D_IM_RAW_RTRG_STOPPED ){
		// Software-Reset ON
		IO_RAW.RCTL0.word = 1;

		// RAWCLK        = MAX200MHz	MIN50MHz
		// APB clock     = 50MHz
		// -->   ( (1/RAWCLK) / (1/APB clock) ) * 3 + 1
		//     = ( (1/25)   / (1/50)     ) * 3 + 1
		//     = 7 APB cycle

		IO_RAW.RCTL0.word;	/* pgr0695 */
		IO_RAW.RCTL0.word;	/* pgr0695 */
		IO_RAW.RCTL0.word;	/* pgr0695 */
		IO_RAW.RCTL0.word;	/* pgr0695 */

		// Software-Reset OFF
		IO_RAW.RCTL0.word = 0;

		// RAW macro disable (power save mode)
		IO_RAW.TBLAEN.bit.TBLAEN	= D_IM_RAW_TBLAEN_OFF;

		retval = D_IM_RAW_RETVAL_OK;
	}
	else{
		retval = D_IM_RAW_RETVAL_MACRO_BUSY;
	}

	Im_RAW_Off_Pclk();
	im_raw_off_rawiclk();
	im_raw_off_rawclk();
	Im_RAW_Dsb();

	return retval;
}

/**
   Open driver.
   */

/**
 * @brief			The semaphore of RAW is acquired
 * @param[in]		wait_time	: Time-out period
 * @param[out]		None
 * @retval			D_IM_RAW_RETVAL_OK, D_IM_RAW_RETVAL_PARAM_ERROR, D_IM_RAW_RETVAL_SYSTEM_ERROR
 * @note			None
 */
INT32 Im_RAW_Open( INT32 wait_time )
{
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if( im_raw_check_wait_time(wait_time) == D_IM_RAW_FALSE ){
		Ddim_Assertion(("I:Im_RAW_Open INVALID_ARG_ERR\n"));
		return D_IM_RAW_RETVAL_PARAM_ERROR;
	}
#endif

	if( D_DDIM_USER_SEM_WAIT_POL == wait_time ){
		ercd = DDIM_User_Pol_Sem( SID_IM_RAW );
	}
	else{
		ercd = DDIM_User_Twai_Sem( SID_IM_RAW, (DDIM_USER_TMO)wait_time );
	}

	if( D_DDIM_USER_E_OK == ercd ){
		return D_IM_RAW_RETVAL_OK;
	}
	else{
		return D_IM_RAW_RETVAL_SYSTEM_ERROR;
	}
}

/**
 * @brief		The semaphore of RAW is returned
 * @param[in]	None
 * @param[out]	None
 * @return		D_IM_RAW_RETVAL_OK, D_IM_RAW_RETVAL_SYSTEM_ERROR
 * @note		None
 */
INT32 Im_RAW_Close( VOID )
{
	DDIM_USER_ER	ercd;

	ercd = DDIM_User_Sig_Sem( SID_IM_RAW );

	if( D_DDIM_USER_E_OK == ercd ){
		return D_IM_RAW_RETVAL_OK;
	}
	else{
		return D_IM_RAW_RETVAL_SYSTEM_ERROR;
	}
}

/**
 * @brief		Setup CODEx and LENx table registers.
 * @param[in]	T_IM_RAW_CTRL_CODE_TBL* tbl	: Pointer of code table
 * @param[in]	UCHAR data_format			: D_IM_RAW_RFMT_14_OR_16_BIT, D_IM_RAW_RFMT_12_BIT_PACK, D_IM_RAW_RFMT_8_BIT
 * @param[out]	None
 * @return		D_IM_RAW_RETVAL_OK, D_IM_RAW_RETVAL_PARAM_ERROR
 * @note		None
 */
INT32 Im_RAW_Ctrl_CodeTbl( const T_IM_RAW_CTRL_CODE_TBL* const tbl, UCHAR data_format )
{
	INT32			i, num;
	volatile UINT32	*pCODE, *pLEN;

#ifdef CO_PARAM_CHECK
	if( (im_raw_check_pointer(tbl) == D_IM_RAW_FALSE) || ( data_format > D_IM_RAW_RFMT_8_BIT ) ){
		Ddim_Assertion(("I:Im_RAW_Ctrl_CodeTbl : INVALID_ARG_ERR\n"));
		return D_IM_RAW_RETVAL_PARAM_ERROR;
	}
#endif

	Im_RAW_On_Pclk();
	Im_RAW_Dsb();

	if( IO_RAW.RCTL1.bit.RTRG == D_IM_RAW_RTRG_RUNNING ){
		Im_RAW_Off_Pclk();
		Im_RAW_Dsb();
		return D_IM_RAW_RETVAL_MACRO_BUSY;
	}

	// CODEx / LENx
	pCODE = (UINT32*)&IO_RAW.CODE.CODE1.word;
	pLEN  = (UINT32*)&IO_RAW.LEN.LEN1.word;
	if( data_format == D_IM_RAW_RFMT_14_OR_16_BIT ){
		num = 14;	// CODE0 - CODEE, LEN0 - LENE
	}
	else if( data_format == D_IM_RAW_RFMT_12_BIT_PACK ){
		num = 12;	// CODE0 - CODEC, LEN0 - LENC (Can't Set D - E)
	}
	else{			// D_IM_RAW_RFMT_8_BIT
		num = 8;	// CODE0 - CODE8, LEN0 - LEN8 (Can't Set 9 - E)
	}
	for( i = 0; i < num; i+=2 ){
		*pCODE++ = (UINT32)( (tbl->code[i+1] << 16)        | (tbl->code[i] << 0)        );
		*pLEN++  = (UINT32)( (tbl->code_length[i+1] << 16) | (tbl->code_length[i] << 0) );
	}
	*pCODE = (UINT32)(tbl->code[i] << 0);
	*pLEN  = (UINT32)(tbl->code_length[i] << 0);

	Im_RAW_Off_Pclk();
	Im_RAW_Dsb();

	return D_IM_RAW_RETVAL_OK;
}

/**
 * @brief		Setup encoding parameters except for CODEx and LENx.
 * @param[in]	T_IM_RAW_CTRL_PARAM* cfg	: Pointer of encoding parameter table.
 * @param[in]	UINT32 code_limit			: The limitation of the size of encoded data
 * @param[out]	None
 * @return		D_IM_RAW_RETVAL_OK, D_IM_RAW_RETVAL_PARAM_ERROR, D_IM_RAW_RETVAL_MACRO_BUSY
 * @note		None
 */
INT32 Im_RAW_Ctrl_Enc( const T_IM_RAW_CTRL_PARAM* const cfg, UINT32 code_limit )
{
#ifdef CO_PARAM_CHECK
	if( im_raw_check_pointer(cfg) == D_IM_RAW_FALSE ){
		Ddim_Assertion(("I:Im_RAW_Ctrl_Enc : INVALID_ARG_ERR\n"));
		return D_IM_RAW_RETVAL_PARAM_ERROR;
	}
#endif

	Im_RAW_On_Pclk();
	Im_RAW_Dsb();

	if( IO_RAW.RCTL1.bit.RTRG == D_IM_RAW_RTRG_RUNNING ){
		Im_RAW_Off_Pclk();
		Im_RAW_Dsb();
		return D_IM_RAW_RETVAL_MACRO_BUSY;
	}

	im_raw_setup_param( cfg );

	IO_RAW.RCTL1.bit.OPMD = 0;			// Encode
	IO_RAW.CLMT.bit.CLMT = code_limit;

	Im_RAW_Off_Pclk();
	Im_RAW_Dsb();

	return D_IM_RAW_RETVAL_OK;
}

/**
 * @brief		Setup decoding parameters except for CODEx and LENx.
 * @param[in]	T_IM_RAW_CTRL_PARAM*  cfg	: Pointer of decoding parameter table.
 * @param[out]	None
 * @return		D_IM_RAW_RETVAL_OK, D_IM_RAW_RETVAL_PARAM_ERROR, D_IM_RAW_RETVAL_MACRO_BUSY
 * @note		None
 */
INT32 Im_RAW_Ctrl_Dec( const T_IM_RAW_CTRL_PARAM* const cfg )
{
#ifdef CO_PARAM_CHECK
	if( im_raw_check_pointer(cfg) == D_IM_RAW_FALSE ){
		Ddim_Assertion(("I:Im_RAW_Ctrl_Dec : INVALID_ARG_ERR\n"));
		return D_IM_RAW_RETVAL_PARAM_ERROR;
	}
#endif

	Im_RAW_On_Pclk();
	Im_RAW_Dsb();

	if( IO_RAW.RCTL1.bit.RTRG == D_IM_RAW_RTRG_RUNNING ){
		Im_RAW_Off_Pclk();
		Im_RAW_Dsb();

		return D_IM_RAW_RETVAL_MACRO_BUSY;
	}

	im_raw_setup_param( cfg );

	IO_RAW.RCTL1.bit.OPMD = 1;			// Decode

	Im_RAW_Off_Pclk();
	Im_RAW_Dsb();

	return D_IM_RAW_RETVAL_OK;
}

/**
 * @brief		Start JDSRAW3A.
 * @param[in]	None
 * @param[out]	None
 * @return		D_IM_RAW_RETVAL_OK, D_IM_RAW_RETVAL_SYSTEM_ERROR, D_IM_RAW_RETVAL_MACRO_BUSY
 * @note		None
 */
INT32 Im_RAW_Start( VOID )
{
	DDIM_USER_ER	ercd;

	im_raw_on_rawclk();
	im_raw_on_rawiclk();
	Im_RAW_On_Pclk();
	Im_RAW_Dsb();

	if( IO_RAW.RCTL1.bit.RTRG == D_IM_RAW_RTRG_RUNNING ){
		Im_RAW_Off_Pclk();
		im_raw_off_rawiclk();
		im_raw_off_rawclk();
		Im_RAW_Dsb();
		return D_IM_RAW_RETVAL_MACRO_BUSY;
	}

	ercd = DDIM_User_Clr_Flg( FID_IM_RAW, (~(D_IM_RAW_FLG_END | D_IM_RAW_FLG_STOP | D_IM_RAW_FLG_R_AXI_ERR | D_IM_RAW_FLG_W_AXI_ERR)) );
	if( D_DDIM_USER_E_OK != ercd ){
		Im_RAW_Off_Pclk();
		im_raw_off_rawiclk();
		im_raw_off_rawclk();
		Im_RAW_Dsb();
		return D_IM_RAW_RETVAL_SYSTEM_ERROR;
	}

	// set RAW macro enable (active mode)
	IO_RAW.TBLAEN.bit.TBLAEN	= D_IM_RAW_TBLAEN_ON;
	// wait 1us
	Im_RAW_Wait_usec( D_IM_RAW_SRAM_WAIT_USEC );

	IO_RAW.RCTL1.bit.RTRG = D_IM_RAW_RTRG_START;

	Im_RAW_Off_Pclk();
	Im_RAW_Dsb();

	return D_IM_RAW_RETVAL_OK;
}

/**
 * @brief		Wait end at encoding.
 * @param[out]	UINT32*  condition	: D_IM_RAW_COND_OK, D_IM_RAW_COND_ENC_LIMIT_OVER
 * @param[out]	UINT32* byte		: The size of encoded data
 * @param[in]	INT32 wait_time		: Wait time [msec]. The valid range is -1, 0, ... . 
 * @return		D_IM_RAW_RETVAL_OK, D_IM_RAW_RETVAL_PARAM_ERROR, D_IM_RAW_RETVAL_FORCE_STOP, D_IM_RAW_RETVAL_TIME_OUT
 * @note		None
 */
INT32 Im_RAW_Wait_End_Enc( UINT32* const condition, UINT32* const byte, INT32 wait_time )
{
	INT32 				retval;
	DDIM_USER_FLGPTN	waitptn;

#ifdef CO_PARAM_CHECK
	if( (im_raw_check_pointer(condition) == D_IM_RAW_FALSE)
		|| (im_raw_check_pointer(byte) == D_IM_RAW_FALSE)
		|| (im_raw_check_wait_time(wait_time) == D_IM_RAW_FALSE) ){
		Ddim_Assertion(("I:Im_RAW_Wait_End_Enc : INVALID_ARG_ERR\n"));
		return D_IM_RAW_RETVAL_PARAM_ERROR;
	}
#endif

	retval = im_raw_wait_end( &waitptn, wait_time );

	im_raw_off_rawiclk();
	im_raw_off_rawclk();

	Im_RAW_On_Pclk();
	Im_RAW_Dsb();

	// RAW macro disable (power save mode)
	IO_RAW.TBLAEN.bit.TBLAEN	= D_IM_RAW_TBLAEN_OFF;

	Im_RAW_Off_Pclk();
	Im_RAW_Dsb();

	if( retval != D_IM_RAW_RETVAL_OK ){
		return retval;
	}

	if( (waitptn & D_IM_RAW_FLG_END) == D_IM_RAW_FLG_END ){
		*condition = gIM_RAW_COND;

		Im_RAW_On_Pclk();
		Im_RAW_Dsb();

		*byte = IO_RAW.CCNT.word;

		Im_RAW_Off_Pclk();
		Im_RAW_Dsb();

		return D_IM_RAW_RETVAL_OK;
	}

	return D_IM_RAW_RETVAL_FORCE_STOP;
}

/**
 * @brief		Wait end at decoding.
 * @param[out]	UINT32*  condition	: D_IM_RAW_COND_OK, D_IM_RAW_COND_ENC_LIMIT_OVER
 * @param[in]	INT32 wait_time		: Wait time [msec]. The valid range is -1, 0, ... . 
 * @return		D_IM_RAW_RETVAL_OK, D_IM_RAW_RETVAL_PARAM_ERROR, D_IM_RAW_RETVAL_FORCE_STOP, D_IM_RAW_RETVAL_TIME_OUT
 * @note		None
 */
INT32 Im_RAW_Wait_End_Dec( UINT32* const condition, INT32 wait_time )
{
	INT32 				retval;
	DDIM_USER_FLGPTN	waitptn;

#ifdef CO_PARAM_CHECK
	if( (im_raw_check_pointer(condition) == D_IM_RAW_FALSE)
		|| (im_raw_check_wait_time(wait_time) == D_IM_RAW_FALSE) ){
		Ddim_Assertion(("I:Im_RAW_Wait_End_Dec : INVALID_ARG_ERR\n"));
		return D_IM_RAW_RETVAL_PARAM_ERROR;
	}
#endif

	retval = im_raw_wait_end( &waitptn, wait_time );

	im_raw_off_rawiclk();
	im_raw_off_rawclk();

	Im_RAW_On_Pclk();
	Im_RAW_Dsb();

	// RAW macro disable (power save mode)
	IO_RAW.TBLAEN.bit.TBLAEN	= D_IM_RAW_TBLAEN_OFF;

	Im_RAW_Off_Pclk();
	Im_RAW_Dsb();

	if( retval != D_IM_RAW_RETVAL_OK ){
		return retval;
	}

	if( (waitptn & D_IM_RAW_FLG_END) == D_IM_RAW_FLG_END ){
		*condition = gIM_RAW_COND;
		return D_IM_RAW_RETVAL_OK;
	}

	return D_IM_RAW_RETVAL_FORCE_STOP;
}

/**
 * @brief		Stop JDSRAW3A.
 * @param[in]	None
 * @param[out]	None
 * @return		D_IM_RAW_RETVAL_OK, D_IM_RAW_RTRG_STOP, D_IM_RAW_RETVAL_SYSTEM_ERROR, D_IM_RAW_RETVAL_MACRO_BUSY
 * @note		None
 */
INT32 Im_RAW_Stop( VOID )
{
	INT32 retval = D_IM_RAW_RETVAL_OK;
	DDIM_USER_ER	ercd;

	Im_RAW_On_Pclk();
	Im_RAW_Dsb();

	if( IO_RAW.RCTL1.bit.RTRG == D_IM_RAW_RTRG_RUNNING ){
		IO_RAW.RCTL1.bit.RTRG = D_IM_RAW_RTRG_STOP;
		Im_RAW_Dsb();

		ercd = DDIM_User_Set_Flg( FID_IM_RAW, D_IM_RAW_FLG_STOP );
		if( ercd != D_DDIM_USER_E_OK ){
			Ddim_Print(("I:Im_RAW_Stop : Error DDIM_User_Set_Flg [%d]\n", ercd));
			retval = D_IM_RAW_RETVAL_SYSTEM_ERROR;
		}

		if( IO_RAW.RCTL1.bit.RTRG == D_IM_RAW_RTRG_RUNNING ){
			Ddim_Print(("I:Im_RAW_Stop : JDSRAW3A is still running.\n"));
			retval = D_IM_RAW_RETVAL_MACRO_BUSY;
		}
	}

	Im_RAW_Off_Pclk();
	Im_RAW_Dsb();

	return retval;
}

/**
 * @brief		Get AXI bus I/F Status
 * @param[out]	T_IM_RAW_AXI_STATUS* sts	: sts	AXI bus I/F status parameter
 * @return		D_IM_RAW_RETVAL_OK, D_IM_RAW_RETVAL_PARAM_ERROR
 * @note		None
 */
INT32 Im_RAW_Get_AXI_Status( T_IM_RAW_AXI_STATUS* const sts )
{
#ifdef CO_PARAM_CHECK
	if( sts == NULL ){
		// sts setting error
		Ddim_Assertion(("I:Im_RAW_Get_AXI_Status : INVALID_ARG_ERR\n"));
		return D_IM_RAW_RETVAL_PARAM_ERROR;
	}
#endif

	Im_RAW_On_Pclk();
	Im_RAW_Dsb();

	sts->r_ch_resp = IO_RAW.MRAXSTS.bit.RRESP;
	sts->w_ch_resp = IO_RAW.MWAXSTS.bit.BRESP;

	Im_RAW_Off_Pclk();
	Im_RAW_Dsb();

	return D_DDIM_OK;
}

/**
 * @brief		Interrupt handler.
 * @param[in]	None
 * @param[out]	None
 * @return		None
 * @note		None
 */
VOID Im_RAW_Int_Handler( VOID )
{
	UINT32	rinte_reg, rintf_reg;
	DDIM_USER_ER	ercd;

	Im_RAW_On_Pclk();
	Im_RAW_Dsb();

	rintf_reg = IO_RAW.RINTF.word;
	IO_RAW.RINTF.word = rintf_reg;		// Clear interrupt
	rinte_reg = IO_RAW.RINTE.word;
	gIM_RAW_COND = IO_RAW.COND.word;	// Get COND register.

	Im_RAW_Off_Pclk();
	Im_RAW_Dsb();

	// Check Read AXI Error interrupt state
	if( ( (rinte_reg & D_IM_RAW_INT_RAXER_BIT) != 0 ) && ( (rintf_reg & D_IM_RAW_INT_RAXER_BIT) != 0 ) ){
		ercd = DDIM_User_Set_Flg( FID_IM_RAW, D_IM_RAW_FLG_R_AXI_ERR );
		if( ercd != D_DDIM_USER_E_OK ){
			Ddim_Print(("I:Im_RAW_Int_Handler : Read AXI Error DDIM_User_Set_Flg [%d]\n", ercd));
		}

		if( gIM_RAW_Callback != NULL ){
			gIM_RAW_Callback( (UINT32)D_IM_RAW_R_AXI_ERROR );
		}
	}

	// Check Write AXI Error interrupt state
	if( ( (rinte_reg & D_IM_RAW_INT_WAXER_BIT) != 0 ) && ( (rintf_reg & D_IM_RAW_INT_WAXER_BIT) != 0 ) ){
		ercd = DDIM_User_Set_Flg( FID_IM_RAW, D_IM_RAW_FLG_W_AXI_ERR );
		if( ercd != D_DDIM_USER_E_OK ){
			Ddim_Print(("I:Im_RAW_Int_Handler : Write AXI Error DDIM_User_Set_Flg [%d]\n", ercd));
		}

		if( gIM_RAW_Callback != NULL ){
			gIM_RAW_Callback( (UINT32)D_IM_RAW_W_AXI_ERROR );
		}
	}

	// Check Process complete interrupt state
	if( ( (rinte_reg & D_IM_RAW_INT_R_BIT) != 0 ) && ( (rintf_reg & D_IM_RAW_INT_R_BIT) != 0 ) ){
		ercd = DDIM_User_Set_Flg( FID_IM_RAW, D_IM_RAW_FLG_END );
		if( ercd != D_DDIM_USER_E_OK ){
			Ddim_Print(("I:Im_RAW_Int_Handler : Error DDIM_User_Set_Flg [%d]\n", ercd));
		}

		if( gIM_RAW_Callback != NULL ){
			gIM_RAW_Callback( gIM_RAW_COND );
		}
	}
}
