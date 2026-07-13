/**
 * @file		dd_relc.c
 * @brief		RELC macro driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "dd_relc.h"
#include "_relc.h"
#include "dd_arm.h"
#include "dd_exs.h"
#include "dd_gpv.h"
#include "ddim_user_custom.h"
#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Input buffer size */
#define D_DD_RELC_IN_BUF_64		(64)	/**< Input buffer size 64B 		*/
#define D_DD_RELC_IN_BUF_128	(128)	/**< Input buffer size 128B 	*/
#define D_DD_RELC_IN_BUF_256	(256)	/**< Input buffer size 256B 	*/
#define D_DD_RELC_IN_BUF_512	(512)	/**< Input buffer size 512B		*/
#define D_DD_RELC_IN_BUF_1024	(1024)	/**< Input buffer size 1024B	*/
#define D_DD_RELC_IN_BUF_2048	(2048)	/**< Input buffer size 2048B	*/

/* Ref. buffer size */
#define D_DD_RELC_REF_NO_BUF	(0)		/**< * Ref. buffer is mot exist */
#define D_DD_RELC_REF_BUF_8K	(8196)	/**< * Ref. buffer size 8kbyte	*/

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------




#endif	/* CO_DDIM_UTILITY_USE */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static VP_CALLBACK gRELC_CallBack = NULL;		/**< Callback function pointer */
static volatile T_DD_RELC_DEC_INFO gRELC_Dec_Info;

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
 * @brief  RELC initialize.
 * @param[in] write_hprot			RELC data write side HPROT (default:0x0B)
 * @param[in] read_hprot			RELC data read side HPROT (default:0x0B)
*/
VOID Dd_RELC_Init( UCHAR write_hprot, UCHAR read_hprot )
{
	Dd_Exs_Set_RAM_PD_Ctrl_RELC( 1 );
	Dd_Exs_Set_RELC_Soft_Reset( 1 );
	Dd_GPV_Set_ARC_Remap( D_DD_GPV_REMAP_SD_RELC_HDMAC_USB2, 1 );

#ifdef CO_ACT_CLOCK
	Dd_Exs_Set_RELC_BUS_CLOCK_Stop_Ctrl(0);
#endif //CO_ACT_CLOCK

	Dd_Exs_Set_RELC_HPROT_Write( write_hprot );	/* Set the RELC expanded data write side HPROT setting */
	Dd_Exs_Set_RELC_HPROT_Read( read_hprot );	/* Set the RELC expanded data read side HPROT setting */

	IO_RELC.RELC_RESET.bit.SRST	= 0x00000001;	/* SRST:soft reset on */
	Dd_ARM_Dsb_Pou();
	gRELC_CallBack = NULL;

	while ( 1 ) {
		if( IO_RELC.RELC_RESET.bit.SRST == 0 ) {
			break;
		}
	}

	IO_RELC.RELC_INT_EN.word = 0x0000000F;		/* Interrupt permission */
	Dd_ARM_Dsb_Pou();

	gRELC_Dec_Info.read_byte				= 0;	/* Number of bytes read							*/
	gRELC_Dec_Info.write_byte				= 0;	/* Number of bytes written						*/
	gRELC_Dec_Info.proc_block				= 0;	/* Number of processing blocks					*/
	gRELC_Dec_Info.dec_total_read_bytes		= 0;	/* Decode the total number of bytes to read		*/
	gRELC_Dec_Info.dec_total_write_bytes	= 0;	/* Decode the total number of bytes to write	*/
	gRELC_Dec_Info.seq_cnt					= 0;	/* Sequential run counter						*/

#ifdef CO_ACT_CLOCK
	Dd_Exs_Set_RELC_BUS_CLOCK_Stop_Ctrl(1);
#endif //CO_ACT_CLOCK
}


/**
 * @brief  RELC is exclusively controlled.
 * @return D_DD_RELC_OK					OK
 * @return D_DD_RELC_EXT_LOCK_NG		Lock Error (System Using RELC)
*/
INT32 Dd_RELC_Open(VOID)
{
	INT32 ret = D_DD_RELC_OK;

	if ( DDIM_User_Twai_Sem(SID_DD_RELC, D_DDIM_USER_SEM_WAIT_POL) != D_DDIM_USER_E_OK ) {
		ret = D_DD_RELC_EXT_LOCK_NG;
	}
	
	Dd_Exs_Set_RAM_PD_Ctrl_RELC( 0 );

#ifdef CO_ACT_CLOCK
	Dd_Exs_Set_RELC_BUS_CLOCK_Stop_Ctrl(0);
#endif //CO_ACT_CLOCK

	return ret;
}


/**
 * @brief  RELC is exclusively closed.
 * @return D_DD_RELC_OK				OK
 * @return D_DD_RELC_EXT_LOCK_NG	Lock Error (System Using RELC
*/
INT32 Dd_RELC_Close(VOID)
{
	INT32 ret = D_DD_RELC_OK;

	if ( DDIM_User_Sig_Sem(SID_DD_RELC) != D_DDIM_USER_E_OK ) {
		ret = D_DD_RELC_EXT_LOCK_NG;
	}

	Dd_Exs_Set_RAM_PD_Ctrl_RELC( 1 );

#ifdef CO_ACT_CLOCK
	Dd_Exs_Set_RELC_BUS_CLOCK_Stop_Ctrl(1);
#endif //CO_ACT_CLOCK

	return ret;
}


/**
 * @brief  RELC decode is started.(Sync)
 * @return D_DD_RELC_OK				OK
 * @return D_DD_RELC_FLG_CLR_NG		Clear flag is NG
 * @return D_DD_RELC_TIMEOUT		twai_flg is time out
*/
INT32 Dd_RELC_Start_Sync(VOID)
{
	DDIM_USER_FLGPTN flgptn;
	DDIM_USER_FLGPTN waiptn;
	int ercd = 0;

	waiptn = ( D_DD_RELC_INT_END_FLG | D_DD_RELC_INT_ERR_FLG | D_DD_RELC_INT_SLP_FLG );

	ercd = DDIM_User_Clr_Flg( FID_DD_RELC, ~waiptn );
	if( ercd != D_DDIM_USER_E_OK ) {
		Ddim_Print(("I:RELC_Start_Sync() clr_flg error\n"));
		return D_DD_RELC_FLG_CLR_NG;
	}

	IO_RELC.RELC_MODE1.bit.PRUN	= 0x00000001;	/* RELC start */
	Dd_ARM_Dsb_Pou();

	ercd = DDIM_User_Twai_Flg( FID_DD_RELC, waiptn, D_DDIM_USER_TWF_ORW, &flgptn, D_DDIM_WAIT_END_TIME );
	if ( ercd == D_DDIM_USER_E_TMOUT ) {
		Ddim_Print(("I:RELC_Start_Sync() RELC twai_flg timeout\n"));
		return D_DD_RELC_TIMEOUT;
	}

	return D_DD_RELC_OK;
}


/**
 * @brief  RELC decode is started.(Async)
 * @return D_DD_RELC_OK				OK
 * @return D_DD_RELC_FLG_CLR_NG		Clear flag is NG
*/
INT32 Dd_RELC_Start_Async(VOID)
{
	DDIM_USER_FLGPTN waiptn;
	int ercd = 0;
	waiptn = (D_DD_RELC_INT_END_FLG | D_DD_RELC_INT_ERR_FLG | D_DD_RELC_INT_SLP_FLG );

	ercd = DDIM_User_Clr_Flg( FID_DD_RELC, ~waiptn );
	if(ercd != D_DDIM_USER_E_OK){
		Ddim_Print(("I:RELC_Start_ASync() clr_flg error\n"));
		return D_DD_RELC_FLG_CLR_NG;
	}

	IO_RELC.RELC_MODE1.bit.PRUN	= 0x00000001;	/* RELC start */
	Dd_ARM_Dsb_Pou();

	return D_DD_RELC_OK;
}


/**
 * @brief  Wait for the completion of the RELC decode.
 * @return D_DD_RELC_OK				OK
 * @return D_DD_RELC_TIMEOUT		twai_flg is time out
*/
INT32 Dd_RELC_Wait_End(VOID)
{
	DDIM_USER_FLGPTN flgptn;
	DDIM_USER_FLGPTN waiptn;
	int ercd = 0;

	waiptn = ( D_DD_RELC_INT_END_FLG | D_DD_RELC_INT_ERR_FLG | D_DD_RELC_INT_SLP_FLG );

	ercd = DDIM_User_Twai_Flg( FID_DD_RELC, waiptn, D_DDIM_USER_TWF_ORW, &flgptn, D_DDIM_WAIT_END_TIME );
	if ( ercd == D_DDIM_USER_E_TMOUT ) {
		Ddim_Print(("I:RELC_Wait_End() twai_flg timeout\n"));
		return D_DD_RELC_TIMEOUT;
	}

	gRELC_Dec_Info.read_byte				= (USHORT)IO_RELC.RELC_READ_BYTE.bit.DRNM;	/* Number of bytes read							*/
	gRELC_Dec_Info.write_byte				= (USHORT)IO_RELC.RELC_WRITE_BYTE.bit.DWNM;	/* Number of bytes written						*/
	gRELC_Dec_Info.proc_block				= (USHORT)IO_RELC.RELC_BLOCK.bit.BLKN;		/* Number of processing blocks					*/
	gRELC_Dec_Info.dec_total_read_bytes		= IO_RELC.RELC_INPUT_BYTE;					/* Decode the total number of bytes to read		*/
	gRELC_Dec_Info.dec_total_write_bytes	= IO_RELC.RELC_OUTPUT_BYTE;					/* Decode the total number of bytes to write	*/
	gRELC_Dec_Info.seq_cnt					= IO_RELC.RELC_SEQ_CNT;						/* Sequential run counter						*/

	return D_DD_RELC_OK;
}


/**
 * @brief  RELC decode is stopped.(both Sync and Async)
*/
VOID Dd_RELC_Stop( VOID )
{
	IO_RELC.RELC_MODE1.bit.PRUN	&= 0x00000000;	/* RELC stop */
	Dd_ARM_Dsb_Pou();
}


/**
 * @brief  The common control setting for RELC is set.
 * @param[in] relc_ctrl_cmn			RELC common control parameter
 * @return D_DD_RELC_OK				OK
 * @return D_DD_RELC_PARAM_ERR		Parameter error
*/
INT32 Dd_RELC_Ctrl_Common( T_DD_RELC_CTRL_CMN const* const relc_ctrl_cmn )
{
#ifdef CO_PARAM_CHECK
	if ( relc_ctrl_cmn == NULL ) {
		Ddim_Assertion(("RELC: input param error. [*relc_ctrl_cmn] NULL\n"));
		return D_DD_RELC_PARAM_ERR;
	}
#endif

	if ( relc_ctrl_cmn->desc_mode > 0 ) {
		IO_RELC.RELC_MODE1.word |= (relc_ctrl_cmn->desc_mode << 2);
	}

	IO_RELC.RELC_MODE1.bit.DSMD	= relc_ctrl_cmn->desc_mode;			/* Descriptor Mode					*/
	IO_RELC.RELC_MODE1.bit.CRMD	= relc_ctrl_cmn->cont_run_mode;		/* Continuous Run Mode				*/
	IO_RELC.RELC_SEQ_NUM		= relc_ctrl_cmn->seq_num;			/* Sequential Run Number of Times	*/
	Dd_ARM_Dsb_Pou();
	gRELC_CallBack = relc_ctrl_cmn->callback;						/* Callback function pointer		*/

	return D_DD_RELC_OK;
}


/**
 * @brief  The register control setting for RELC is set.
 * @param[in] relc_ctrl_cmn			RELC register control parameter
 * @return D_DD_RELC_OK				OK
 * @return D_DD_RELC_PARAM_ERR		Parameter error
*/
INT32 Dd_RELC_Ctrl_Register( T_DD_RELC_CTRL_REG const* const relc_ctrl_reg )
{
#ifdef CO_PARAM_CHECK
	if ( relc_ctrl_reg == NULL ) {
		Ddim_Assertion(("RELC: input param error. [*relc_ctrl_reg] NULL\n"));
		return D_DD_RELC_PARAM_ERR;
	}
#endif

	IO_RELC.RELC_MODE2.bit.IMAX	= relc_ctrl_reg->in_end_addr_en;	/* Input End Address Enable			*/
	IO_RELC.RELC_MODE2.bit.OMAX	= relc_ctrl_reg->out_end_addr_en;	/* Output End Address Enable		*/
	IO_RELC.RELC_MODE2.bit.IAAI	= relc_ctrl_reg->in_addr_auto_inc;	/* Input Address Auto Increment		*/
	IO_RELC.RELC_MODE2.bit.OAAI	= relc_ctrl_reg->out_addr_auto_inc;	/* Output Address Auto Increment	*/

	IO_RELC.RELC_IN_START_ADDR	= relc_ctrl_reg->in_start_addr;		/* Input start address				*/
	IO_RELC.RELC_IN_END_ADDR	= relc_ctrl_reg->in_end_addr;		/* Input end address				*/
	IO_RELC.RELC_OUT_START_ADDR	= relc_ctrl_reg->out_start_addr;	/* Output start address				*/
	IO_RELC.RELC_OUT_END_ADDR	= relc_ctrl_reg->out_end_addr;		/* Output end address				*/
	Dd_ARM_Dsb_Pou();

	return D_DD_RELC_OK;
}


/**
 * @brief  The descriptor control setting for RELC is set.
 * @param[in] relc_ctrl_cmn			RELC descriptor control parameter
 * @return D_DD_RELC_OK				OK
 * @return D_DD_RELC_PARAM_ERR		Parameter error
*/
INT32 Dd_RELC_Ctrl_Descriptor( T_DD_RELC_CTRL_DESC const* const relc_ctrl_desc )
{
#ifdef CO_PARAM_CHECK
	if ( relc_ctrl_desc == NULL ) {
		Ddim_Assertion(("RELC: input param error. [*relc_ctrl_desc] NULL\n"));
		return D_DD_RELC_PARAM_ERR;
	}
#endif

	IO_RELC.RELC_DESCRIPTOR_ADDR = relc_ctrl_desc->descriptor_addr;	/* descriptor address */
	Dd_ARM_Dsb_Pou();

	return D_DD_RELC_OK;
}


/**
 * @preif	The input start address for RELC is set.
 * @param[in] in_start_addr			Input start address
 * @return D_DD_RELC_OK				OK
 * @retval D_DD_RELC_PARAM_ERR		Parameter error
*/
INT32 Dd_RELC_Set_In_Start_Addr( ULONG in_start_addr )
{
#ifdef CO_PARAM_CHECK
	if ( in_start_addr == 0 ) {
		Ddim_Assertion(("RELC: input param error. [*in_start_addr] NULL\n"));
		return D_DD_RELC_PARAM_ERR;
	}
#endif

	IO_RELC.RELC_IN_START_ADDR = in_start_addr;	/* Input start address */
	Dd_ARM_Dsb_Pou();

	return D_DD_RELC_OK;
}


/**
 * @preif	The input end address for RELC is set.
 * @param[in] in_end_addr			Input end address
 * @return D_DD_RELC_OK				OK
 * @retval D_DD_RELC_PARAM_ERR		Parameter error
*/
INT32 Dd_RELC_Set_In_End_Addr( ULONG in_end_addr )
{
#ifdef CO_PARAM_CHECK
	if ( in_end_addr == 0 ) {
		Ddim_Assertion(("RELC: input param error. [*in_end_addr] NULL\n"));
		return D_DD_RELC_PARAM_ERR;
	}
#endif

	IO_RELC.RELC_IN_END_ADDR = in_end_addr;		/* Input end address */
	Dd_ARM_Dsb_Pou();

	return D_DD_RELC_OK;
}


/**
 * @preif	The output start address for RELC is set.
 * @param[in] out_start_addr		Output start address
 * @return D_DD_RELC_OK				OK
 * @retval D_DD_RELC_PARAM_ERR		Parameter error
*/
INT32 Dd_RELC_Set_Out_Start_Addr( ULONG out_start_addr )
{
#ifdef CO_PARAM_CHECK
	if ( out_start_addr == 0 ) {
		Ddim_Assertion(("RELC: input param error. [*out_start_addr] NULL\n"));
		return D_DD_RELC_PARAM_ERR;
	}
#endif

	IO_RELC.RELC_OUT_START_ADDR = out_start_addr;	/* Output start address */
	Dd_ARM_Dsb_Pou();

	return D_DD_RELC_OK;
}


/**
 * @preif	The output end address for RELC is set.
 * @param[in] out_end_addr			Output end address
 * @return D_DD_RELC_OK				OK
 * @return D_DD_RELC_PARAM_ERR		Parameter error
*/
INT32 Dd_RELC_Set_Out_End_Addr(ULONG out_end_addr)
{
#ifdef CO_PARAM_CHECK
	if ( out_end_addr == 0 ) {
		Ddim_Assertion(("RELC: input param error. [*out_end_addr] NULL\n"));
		return D_DD_RELC_PARAM_ERR;
	}
#endif
	
	IO_RELC.RELC_OUT_END_ADDR = out_end_addr;	/* Output end address */
	Dd_ARM_Dsb_Pou();

	return D_DD_RELC_OK;
}


/**
 * @preif	The common control setting for RELC is get.
 * @param[out] relc_ctrl_cmn		RELC common control parameter
 * @return D_DD_RELC_OK				OK
 * @return D_DD_RELC_PARAM_ERR			Parameter error
*/
INT32 Dd_RELC_Get_Ctrl_Common( T_DD_RELC_CTRL_CMN* const relc_ctrl_cmn )
{
#ifdef CO_PARAM_CHECK
	if ( relc_ctrl_cmn == NULL ) {
		Ddim_Assertion(("RELC: input param error. [*relc_ctrl_cmn] NULL\n"));
		return D_DD_RELC_PARAM_ERR;
	}
#endif

	relc_ctrl_cmn->desc_mode		= (UCHAR)IO_RELC.RELC_MODE1.bit.DSMD;	/* Descriptor Mode					*/
	relc_ctrl_cmn->cont_run_mode	= (UCHAR)IO_RELC.RELC_MODE1.bit.CRMD;	/* Continuous Run Mode				*/
	relc_ctrl_cmn->seq_num			= IO_RELC.RELC_SEQ_NUM;					/* Sequential Run Number of Times	*/
	relc_ctrl_cmn->callback			= gRELC_CallBack;						/* Callback function pointer		*/

	return D_DD_RELC_OK;
}


/**
 * @preif	The register control setting for RELC is get.
 * @param[out] relc_ctrl_reg		RELC register control parameter
 * @return D_DD_RELC_OK				OK
 * @return D_DD_RELC_PARAM_ERR		Parameter error
*/
INT32 Dd_RELC_Get_Ctrl_Register( T_DD_RELC_CTRL_REG* const relc_ctrl_reg )
{
#ifdef CO_PARAM_CHECK
	if ( relc_ctrl_reg == NULL ) {
		Ddim_Assertion(("RELC: input param error. [*relc_ctrl_reg] NULL\n"));
		return D_DD_RELC_PARAM_ERR;
	}
#endif

	relc_ctrl_reg->in_end_addr_en		= (UCHAR)IO_RELC.RELC_MODE2.bit.IMAX;	/* Input End Address Enable			*/
	relc_ctrl_reg->out_end_addr_en		= (UCHAR)IO_RELC.RELC_MODE2.bit.OMAX;	/* Output End Address Enable		*/
	relc_ctrl_reg->in_addr_auto_inc		= (UCHAR)IO_RELC.RELC_MODE2.bit.IAAI;	/* Input Address Auto Increment		*/
	relc_ctrl_reg->out_addr_auto_inc	= (UCHAR)IO_RELC.RELC_MODE2.bit.OAAI;	/* Output Address Auto Increment	*/

	relc_ctrl_reg->in_start_addr		= IO_RELC.RELC_IN_START_ADDR;			/* Input start address				*/
	relc_ctrl_reg->in_end_addr			= IO_RELC.RELC_IN_END_ADDR;				/* Input end address				*/
	relc_ctrl_reg->out_start_addr		= IO_RELC.RELC_OUT_START_ADDR;			/* Output start address				*/
	relc_ctrl_reg->out_end_addr			= IO_RELC.RELC_OUT_END_ADDR;			/* Output end address				*/

	return D_DD_RELC_OK;
}


/**
 * @preif	The descriptor control setting for RELC is get.
 * @param[out] relc_ctrl_desc		RELC descriptor control parameter
 * @return D_DD_RELC_OK				OK
 * @return D_DD_RELC_PARAM_ERR		Parameter error
*/
INT32 Dd_RELC_Get_Ctrl_Descriptor( T_DD_RELC_CTRL_DESC* const relc_ctrl_desc )
{
#ifdef CO_PARAM_CHECK
	if ( relc_ctrl_desc == NULL) {
		Ddim_Assertion(("RELC: input param error. [*relc_ctrl_desc] NULL\n"));
		return D_DD_RELC_PARAM_ERR;
	}
#endif

	relc_ctrl_desc->descriptor_addr = IO_RELC.RELC_DESCRIPTOR_ADDR;			/* descriptor address */

	return D_DD_RELC_OK;
}


/**
 * @preif	It is Interrupt Handler of RELC.
			The interruption flag is cleared, and the CallBack function is called.
*/
VOID Dd_RELC_Int_Handler( VOID )
{
	DDIM_USER_FLGPTN waiptn;
	UCHAR int_type = 0;
	DDIM_USER_ER ret = D_DDIM_USER_E_OK;

	waiptn = 0;

	if ( ( IO_RELC.RELC_INT_EN.bit.EINT == 1 ) && ( IO_RELC.RELC_INT_ST.bit.__EINS == 1 ) ) {	/* ERROR INT */
		waiptn |= D_DD_RELC_INT_ERR_FLG;
		int_type = D_DD_RELC_ERR;
	}

	if ( ( IO_RELC.RELC_INT_EN.bit.SINT == 1) && ( IO_RELC.RELC_INT_ST.bit.__SINS == 1 ) ) {	/* SLEEP INT */
		if ( int_type == 0 ) {
	 		waiptn |= D_DD_RELC_INT_SLP_FLG;
			int_type = D_DD_RELC_SLEEP;
		}
	}

	if ( ( IO_RELC.RELC_INT_EN.bit.FINT == 1 ) && ( IO_RELC.RELC_INT_ST.bit.__FINS == 1 ) ) {	/* FINISH INT */
		if ( int_type == 0 ) {
	 		waiptn |= D_DD_RELC_INT_END_FLG;
 			int_type = D_DD_RELC_FINISH;
		}
	}

	if ( (IO_RELC.RELC_INT_EN.bit.CINT == 1 ) && ( IO_RELC.RELC_INT_ST.bit.__CINS == 1 ) ) {	/* CONTINUOUS RUN FINISH */
		if ( int_type == 0 ) {
			waiptn |= D_DD_RELC_INT_END_FLG;
			int_type = D_DD_RELC_CR_FINISH;
		}
	}

	if ( int_type == D_DD_RELC_FINISH ) {
		if ( IO_RELC.RELC_SEQ_NUM == 1 ) {
			ret = DDIM_User_Set_Flg( FID_DD_RELC, waiptn );
			if ( ret != D_DDIM_USER_E_OK ) {
				Ddim_Print(("Error DDIM_User_Set_Flg!! ret = 0x%x\n", ret));
			}
		}
	}
	else {
		ret = DDIM_User_Set_Flg( FID_DD_RELC, waiptn );
		if ( ret != D_DDIM_USER_E_OK ) {
			Ddim_Print(("Error DDIM_User_Set_Flg!! ret = 0x%x\n", ret));
		}
	}

	if ( gRELC_CallBack ) {
		(*gRELC_CallBack)( int_type );			/* Call Back function */
	}

	if ( int_type == D_DD_RELC_ERR ) {
		IO_RELC.RELC_INT_ST.word = 0x0000FFFB;	/* EINS */
	}

	if ( int_type == D_DD_RELC_SLEEP ) {
		IO_RELC.RELC_INT_ST.word = 0x0000FFFE;	/* SINS */
	}

	if ( int_type == D_DD_RELC_FINISH ) {
		IO_RELC.RELC_INT_ST.word = 0x0000FFFD;	/* FINS */
	}

	if ( int_type == D_DD_RELC_CR_FINISH ) {
		IO_RELC.RELC_INT_ST.word = 0x0000FFF5;	/* CINS, FINS */
	}

	Dd_ARM_Dsb_Pou();
}


/**
 * @preif	RELC decode status is get.
 * @return	RELC decode status
*/
INT32 Dd_RELC_Get_Status( VOID )
{
	INT32 ret;

	ret = (IO_RELC.RELC_STATUS.word & 0x00000007);	/* 000b = IDLE, 001b = RUN, 010b = SLEEP, 100b = FINISH  */

	return ret;
}


/**
 * @preif	RELC sleep reason is get.
 * @return	RELC sleep reason
*/
INT32 Dd_RELC_Get_Sleep_Reason( VOID )
{
	INT32 ret;

	ret = IO_RELC.RELC_STATUS.bit.SRES;		/* 100b = User instruction,					*/
											/* 001b = To reach the end address input,	*/
											/* 010b = To reach the end address output	*/
	return 	ret;
}


/**
 * @preif	RELC error status is get.
 * @return	RELC error status
*/
INT32 Dd_RELC_Get_Error_Status( VOID )
{
	INT32 ret;

	ret = IO_RELC.RELC_STATUS.bit.EFLG;

	return ret;
}


/**
 * @preif	RELC error number is get.
 * @return D_DD_RELC_OK							OK
 * @return D_DD_RELC_ERR_INPUT_DATA_x			Input data error
 * @return D_DD_RELC_ERR_BUS_INPUT				Bus error(input)
 * @return D_DD_RELC_ERR_BUS_OUTPUT				Bus error(output)
 * @return D_DD_RELC_ERR_ADDR_MISMATCH_INPUT	Address mismatch error(input)
 * @return D_DD_RELC_ERR_ADDR_MISMATCH_OUTPUT	Address mismatch error(output)
*/
INT32 Dd_RELC_Get_Error( VOID )
{
	INT32 ret;

	ret = (IO_RELC.RELC_ERROR_NUM.word & 0x0000ffff);

	return ret;
}


/**
 * @preif	RELC status is get.
 * @param[out] dec_info				RELC processing status
*/
INT32 Dd_RELC_Get_Process_Status( T_DD_RELC_DEC_INFO* dec_info )
{
#ifdef CO_PARAM_CHECK
	if ( dec_info == NULL ) {
		Ddim_Assertion(("RELC: input param error. [*dec_info] NULL\n"));
		return D_DD_RELC_PARAM_ERR;
	}
#endif

#if 0
	dec_info->read_byte				= (USHORT)IO_RELC.RELC_READ_BYTE.bit.DRNM;	/* Number of bytes read							*/
	dec_info->write_byte			= (USHORT)IO_RELC.RELC_WRITE_BYTE.bit.DWNM;	/* Number of bytes written						*/
	dec_info->proc_block			= (USHORT)IO_RELC.RELC_BLOCK.bit.BLKN;		/* Number of processing blocks					*/
	dec_info->dec_total_read_bytes	= IO_RELC.RELC_INPUT_BYTE;					/* Decode the total number of bytes to read		*/
	dec_info->dec_total_write_bytes	= IO_RELC.RELC_OUTPUT_BYTE;					/* Decode the total number of bytes to write	*/
	dec_info->seq_cnt				= IO_RELC.RELC_SEQ_CNT;						/* Sequential run counter						*/
#else
	dec_info->read_byte				= gRELC_Dec_Info.read_byte;					/* Number of bytes read							*/
	dec_info->write_byte			= gRELC_Dec_Info.write_byte;				/* Number of bytes written						*/
	dec_info->proc_block			= gRELC_Dec_Info.proc_block;				/* Number of processing blocks					*/
	dec_info->dec_total_read_bytes	= gRELC_Dec_Info.dec_total_read_bytes;		/* Decode the total number of bytes to read		*/
	dec_info->dec_total_write_bytes	= gRELC_Dec_Info.dec_total_write_bytes;		/* Decode the total number of bytes to write	*/
	dec_info->seq_cnt				= gRELC_Dec_Info.seq_cnt;					/* Sequential run counter						*/
#endif
	return D_DD_RELC_OK;
}


/**
 * @preifThe input and reference data buffer size for RELC is get.
 * @param[out] in_buf_size			input buffer size
 * @param[out] ref_buf_size			reference buffer size
 * @return D_DD_RELC_OK				OK
 * @return D_DD_RELC_PARAM_ERR		Parameter error
*/
INT32 Dd_RELC_Get_Buf_Size( USHORT* in_buf_size, USHORT* ref_buf_size )
{
	UINT32 buf_size_type;

#ifdef CO_PARAM_CHECK
	if ( in_buf_size == NULL ) {
		Ddim_Assertion(("RELC: input param error. [*in_buf_size] NULL\n"));
		return D_DD_RELC_PARAM_ERR;
	}
	if ( ref_buf_size == NULL ) {
		Ddim_Assertion(("RELC: input param error. [*ref_buf_size] NULL\n"));
		return D_DD_RELC_PARAM_ERR;
	}
#endif

	buf_size_type = IO_RELC.RELC_BUF_SIZE.bit.IBFS;

	switch ( buf_size_type ) {
		case 2:
			*in_buf_size = D_DD_RELC_IN_BUF_64;
			break;
		case 3:
			*in_buf_size = D_DD_RELC_IN_BUF_128;
			break;
		case 4:
			*in_buf_size = D_DD_RELC_IN_BUF_256;
			break;
		case 5:
			*in_buf_size = D_DD_RELC_IN_BUF_512;
			break;
		case 6:
			*in_buf_size = D_DD_RELC_IN_BUF_1024;
			break;
		case 7:
			*in_buf_size = D_DD_RELC_IN_BUF_2048;
			break;
		default:
			*in_buf_size = 0;
	}

	buf_size_type = IO_RELC.RELC_BUF_SIZE.bit.RBFS;

	if ( buf_size_type == 0 ) {
		*ref_buf_size = D_DD_RELC_REF_NO_BUF;
	}
	else {
		*ref_buf_size = D_DD_RELC_REF_BUF_8K;
	}

	return D_DD_RELC_OK;
}


/**
 * @preif	Set the data to the input data buffer(mirror erea).
 * @param[in]	in_data_addr		input data address
 * @param[in]	in_data_size		input data size
 * @return D_DD_RELC_OK				OK
 * @return D_DD_RELC_PARAM_ERR		Parameter error
*/
INT32 Dd_RELC_Set_In_Buf_Data_Mirror( ULONG in_data_addr, UINT32 in_data_size )
{
#ifdef CO_PARAM_CHECK
	if ( in_data_addr == 0 ) {
		Ddim_Assertion(("RELC: input param error. [*in_data_addr] NULL\n"));
		return D_DD_RELC_PARAM_ERR;
	}
#endif

	memcpy( (VOID*)&IO_RELC.RELC_INPUT_BUF_DATA_MIRROR, (VOID*)in_data_addr, in_data_size );
	Dd_ARM_Dsb_Pou();

	return D_DD_RELC_OK;
}


/**
 * @preif	Get the data to the input data buffer(mirror erea).
 * @param[out]	in_data_addr			input data address
 * @param[out]	in_data_size			input data size
 * @return D_DD_RELC_OK					OK
 * @return D_DD_RELC_PARAM_ERR			Parameter error
*/
INT32 Dd_RELC_Get_In_Buf_Data_Mirror( ULONG out_data_addr, UINT32 out_data_size )
{
#ifdef CO_PARAM_CHECK
	if ( out_data_addr == 0 ) {
		Ddim_Assertion(("RELC: input param error. [*out_data_addr] NULL\n"));
		return D_DD_RELC_PARAM_ERR;
	}
#endif

	memcpy( (VOID*)out_data_addr, (VOID*)&IO_RELC.RELC_INPUT_BUF_DATA_MIRROR, out_data_size );

	return D_DD_RELC_OK;
}


/**
 * @preif	Set the data to the input data buffer.
 * @param[in] in_data_addr			input data address
 * @param[in] in_data_size			input data size
 * @return D_DD_RELC_OK				OK
 * @return D_DD_RELC_PARAM_ERR			Parameter error
*/
INT32 Dd_RELC_Set_In_Buf_Data( ULONG in_data_addr, UINT32 in_data_size )
{
#ifdef CO_PARAM_CHECK
	if ( in_data_addr == 0 ) {
		Ddim_Assertion(("RELC: input param error. [*in_data_addr] NULL\n"));
		return D_DD_RELC_PARAM_ERR;
	}
#endif

	memcpy( (VOID*)&IO_RELC.RELC_INPUT_BUF_DATA, (VOID*)in_data_addr, in_data_size );
	Dd_ARM_Dsb_Pou();

	return D_DD_RELC_OK;
}


/**
 * @preif	Get the data to the input data buffer.
 * @param[out]	in_data_addr		input data address
 * @param[out]	in_data_size		input data size
 * @return D_DD_RELC_OK				OK
 * @return D_DD_RELC_PARAM_ERR		Parameter error
*/
INT32 Dd_RELC_Get_In_Buf_Data(ULONG out_data_addr, UINT32 out_data_size)
{
#ifdef CO_PARAM_CHECK
	if ( out_data_addr == 0 ) {
		Ddim_Assertion(("RELC: input param error. [*out_data_addr] NULL\n"));
		return D_DD_RELC_PARAM_ERR;
	}
#endif

	memcpy( (VOID*)out_data_addr, (VOID*)&IO_RELC.RELC_INPUT_BUF_DATA, out_data_size );

	return D_DD_RELC_OK;
}


/**
 * @preif	Set the data to the reference data buffer.
 * @param[in]	out_data_addr		output data address
 * @param[in]	out_data_size		output data size
 * @return D_DD_RELC_OK				OK
 * @return D_DD_RELC_PARAM_ERR		Parameter error
*/
INT32 Dd_RELC_Set_Ref_Buf_Data( ULONG in_data_addr, UINT32 in_data_size )
{
#ifdef CO_PARAM_CHECK
	if ( in_data_addr == 0 ) {
		Ddim_Assertion(("RELC: input param error. [*in_data_addr] NULL\n"));
		return D_DD_RELC_PARAM_ERR;
	}
#endif

	memcpy( (VOID*)&IO_RELC.RELC_REF_BUF_DATA, (VOID*)in_data_addr, in_data_size );
	Dd_ARM_Dsb_Pou();

	return D_DD_RELC_OK;
}


/**
 * @preif	Get the data to the reference data buffer.
 * @param[out]	out_data_addr		output data address
 * @param[out]	out_data_size		output data size
 * @return D_DD_RELC_OK				OK
 * @return D_DD_RELC_PARAM_ERR		Parameter error
*/
INT32 Dd_RELC_Get_Ref_Buf_Data( ULONG out_data_addr, UINT32 out_data_size )
{
#ifdef CO_PARAM_CHECK
	if ( out_data_addr == 0 ) {
		Ddim_Assertion(("RELC: input param error. [*out_data_addr] NULL\n"));
		return D_DD_RELC_PARAM_ERR;
	}
#endif

	memcpy( (VOID*)out_data_addr, (VOID*)&IO_RELC.RELC_REF_BUF_DATA, out_data_size );

	return D_DD_RELC_OK;
}




#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/** @name Utility Functions
@{*/
/**
 * @preif	Set and Start RELC normal mode
 * @param[in] relc_set_mod_normal		RELC setting for normal mode
 * @return D_DD_RELC_OK					OK
 * @return D_DD_RELC_PARAM_ERR				Parameter error
*/
INT32 Dd_RELC_Utility_Register( T_DD_RELC_SET_MOD_NORMAL const* const relc_set_mod_normal )
{
	INT32				ret_code = 0;
	T_DD_RELC_CTRL_CMN		ctrl_cmn;
	T_DD_RELC_CTRL_REG		ctrl_reg;

	memset( &ctrl_cmn, 0, sizeof(T_DD_RELC_CTRL_CMN) );
	memset( &ctrl_reg, 0, sizeof(T_DD_RELC_CTRL_REG) );

	ctrl_cmn.write_hprot	= relc_set_mod_normal->write_hprot;
	ctrl_cmn.read_hprot		= relc_set_mod_normal->read_hprot;

	Dd_RELC_Init( ctrl_cmn.write_hprot, ctrl_cmn.read_hprot );	/* After power on and the reset, this is executed only once. */
	
	ret_code = Dd_RELC_Open();
	if ( ret_code != D_DD_RELC_OK ) {
		Ddim_Print( ("Error RELC Open !!\n" ) );
		return ret_code;
	}
	
	ctrl_cmn.desc_mode	= D_DD_RELC_RUN_MODE_NORMAL;
	ctrl_cmn.callback	= relc_set_mod_normal->callback;
	
	if ( ( relc_set_mod_normal->seq_num) > 1 ) {	/* Continuous run mode */
		ctrl_cmn.cont_run_mode		= D_DD_RELC_CONT_MODE_CONT;
		ctrl_cmn.seq_num			= relc_set_mod_normal->seq_num;
		ctrl_reg.in_end_addr_en		= D_DD_RELC_ADDR_DISABLE;
		ctrl_reg.out_end_addr_en	= D_DD_RELC_ADDR_DISABLE;
		ctrl_reg.in_addr_auto_inc	= D_DD_RELC_ADDR_INCRE_ON;
		ctrl_reg.out_addr_auto_inc	= D_DD_RELC_ADDR_INCRE_ON;
	}
	else {											/* Normal mode */
		ctrl_cmn.cont_run_mode		= D_DD_RELC_CONT_MODE_NORMAL;
		ctrl_cmn.seq_num			= 1;
		ctrl_reg.in_end_addr_en		= D_DD_RELC_ADDR_DISABLE;
		ctrl_reg.out_end_addr_en	= D_DD_RELC_ADDR_DISABLE;
		ctrl_reg.in_addr_auto_inc	= D_DD_RELC_ADDR_INCRE_OFF;
		ctrl_reg.out_addr_auto_inc	= D_DD_RELC_ADDR_INCRE_OFF;
	}

	ctrl_reg.in_start_addr	= relc_set_mod_normal->in_start_addr;
	ctrl_reg.in_end_addr	= 0;
	ctrl_reg.out_start_addr	= relc_set_mod_normal->out_start_addr;
	ctrl_reg.out_end_addr	= 0;

	ret_code = Dd_RELC_Ctrl_Common( &ctrl_cmn );

	if ( ret_code != D_DD_RELC_OK ) {
		Ddim_Print( ("Error Dd_RELC_Ctrl_Common() !!\n" ) );
	}

	ret_code = Dd_RELC_Ctrl_Register(&ctrl_reg);

	if ( ret_code != D_DD_RELC_OK ) {
		Ddim_Print( ("Error Dd_RELC_Ctrl_Register() !!\n" ) );
	}

	ret_code = Dd_RELC_Start_Sync();

	if ( ret_code != 0 ) {
		Ddim_Print(("Dd_RELC_Start_Sync() clr_flg error. ret_code=0x%x\n", ret_code));
	}

	ret_code = Dd_RELC_Close();

	if ( ret_code != D_DD_RELC_OK ) {
		Ddim_Print( ("Error Dd_RELC_Close() !!\n" ) );
	}

	return ret_code;
}


/**
 * @preif	Set and Start RELC descriptor mode
 * @param[in] relc_set_mod_desc		RELC setting for descriptor mode
 * @retval D_DD_RELC_OK				OK
 * @retval D_DD_RELC_PARAM_ERR		Parameter error
*/
INT32 Dd_RELC_Utility_Descriptor( T_DD_RELC_SET_MOD_DESC const* const relc_set_mod_desc )
{
	INT32				ret_code = 0;
	T_DD_RELC_CTRL_CMN	ctrl_cmn;
	T_DD_RELC_CTRL_DESC	ctrl_desc;

	memset( &ctrl_cmn, 0, sizeof(T_DD_RELC_CTRL_CMN) );
	memset( &ctrl_desc, 0, sizeof(T_DD_RELC_CTRL_DESC) );

	ctrl_cmn.write_hprot	= relc_set_mod_desc->write_hprot;
	ctrl_cmn.read_hprot		= relc_set_mod_desc->read_hprot;

	Dd_RELC_Init( ctrl_cmn.write_hprot, ctrl_cmn.read_hprot );	/* After power on and the reset, this is executed only once. */

	ret_code = Dd_RELC_Open();

	if ( ret_code != D_DD_RELC_OK ) {
		Ddim_Print( ("Error RELC Open !!\n" ) );
		return ret_code;
	}

	ctrl_cmn.desc_mode	= D_DD_RELC_RUN_MODE_DESC;
	ctrl_cmn.callback	= relc_set_mod_desc->callback;

	if ( relc_set_mod_desc->seq_num > 1 ) {				/* Continuous run mode */
		ctrl_cmn.cont_run_mode	= D_DD_RELC_CONT_MODE_CONT;
		ctrl_cmn.seq_num		= relc_set_mod_desc->seq_num;
	}
	else {												/* Normal mode */
		ctrl_cmn.cont_run_mode	= D_DD_RELC_CONT_MODE_NORMAL;
		ctrl_cmn.seq_num		= 1;
	}

	ctrl_desc.descriptor_addr	= relc_set_mod_desc->relc_descriptor_addr;

	ret_code = Dd_RELC_Ctrl_Common( &ctrl_cmn );

	if ( ret_code != D_DD_RELC_OK ) {
		Ddim_Print( ("Error Dd_RELC_Ctrl_Common() !!\n" ) );
	}

	ret_code = Dd_RELC_Ctrl_Descriptor(&ctrl_desc);

	if ( ret_code != D_DD_RELC_OK ) {
		Ddim_Print( ("Error Dd_RELC_Ctrl_Descriptor() !!\n" ) );
	}

	ret_code = Dd_RELC_Start_Sync();

	if ( ret_code != 0 ) {
		Ddim_Print(("Dd_RELC_Start_Sync() clr_flg error. ret_code=0x%x\n", ret_code));
	}

	ret_code = Dd_RELC_Close();

	if ( ret_code != D_DD_RELC_OK ) {
		Ddim_Print( ("Error Dd_RELC_Close() !!\n" ) );
	}

	return ret_code;
}


#endif	/* CO_DDIM_UTILITY_USE */

/*@}*/
