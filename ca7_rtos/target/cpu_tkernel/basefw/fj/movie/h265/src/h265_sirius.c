/**
 * @file		h265_sirius.c
 * @brief		H.265 SIRIUS function.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "h265_sirius.h"

#ifdef CO_H265_SECTION_ON
//#pragma arm section code="H265_CODE"
//#pragma arm section rodata="H265_CONST"
//#pragma arm section rwdata="H265_INIT"
#endif

//#define CO_H265_DEBUG_MBA

#ifdef CO_H265_DEBUG_MBA
#include "mba_if.h"
#endif

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_H265_INT_TYPE_SYS				(0)
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define D_H265_INT_TYPE_ENC				(1)
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define D_H265_INT_TYPE_DEC				(2)

#define D_H265_INT_SR_CONT_COUNT_MAX	(32)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
/* Encode */
typedef struct{
	ULONG	index[D_H265_SIRIUS_FM_EMPTY_FIFO_MAX_NUM];
	ULONG	idx_get;
	ULONG	idx_set;
	ULONG	full_flg;
	ULONG	emp_flg;
} T_H265_SIRIUS_FM_EMPTY_FIFO;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

typedef struct {
	UINT32		type;
	UINT32		data;
} T_H265_SR_CONT_MSG;

typedef struct {
	OS_USER_VP		_os_reserve[2];	// OS reserve
//	OS_USER_VP		blf_addr;		// blf address
	OS_USER_VB		msg[16];		// message
} T_H265_SR_CONT_MBOX;


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
static	volatile	T_H265_SIRIUS_FM_EMPTY_FIFO	gFm_Empty_Fifo[SR_ENCODE_NUM];
volatile UINT32					g_sr_stop_done_flg[SR_ENCODE_NUM];
volatile T_SR_ENC_ERROR_FACTOR	errfactor;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

volatile T_H265_SR_CONT_MBOX	h265_sr_cont_mbox[D_H265_INT_SR_CONT_COUNT_MAX];
volatile UINT32					sr_cont_count;

volatile UINT32					g_sr_stop_done_flg[SR_ENCODE_NUM];

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static INT32 H265_Sirius_Set_Msg( T_H265_SR_CONT_MSG* msg );
static INT32 H265_Sirius_Get_Msg( T_H265_SR_CONT_MSG* msg );
static INT32 H264_Sirius_Clear_Msg( VOID );
static VOID h265_sirius_system_intHandler( UINT32 ch, UINT32 status );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
static VOID h265_sirius_enc_intHandler( UINT32 ch, UINT32 status );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
static VOID h265_sirius_dec_intHandler( UINT32 ch, UINT32 status );
//static VOID h265_sirius_clean( ULONG addr, ULONG size );
//static VOID h265_sirius_flush( ULONG addr, ULONG size );
static VOID h265_sirius_remap_address_set( UINT32 mode, T_SR_BRIDGE_DDR_CONFIG* ddr_config );
static VOID h265_sirius_sdramc_clock_enable( UINT32 mode, E_H265_SDRAMC_CLOCK_EN enable );

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
static INT32 H265_Sirius_Set_Msg( T_H265_SR_CONT_MSG* msg )
{
	OS_USER_ER		ercd;
	T_H265_SR_CONT_MBOX	*mbox_buff;
	
	if( sr_cont_count >= D_H265_INT_SR_CONT_COUNT_MAX ){
		sr_cont_count = 0;
	}
	mbox_buff = (T_H265_SR_CONT_MBOX *)&h265_sr_cont_mbox[sr_cont_count];
	sr_cont_count++;
	
	memcpy( mbox_buff->msg, msg, sizeof(T_H265_SR_CONT_MSG) );
	
	/* [mbox] */
//	H265_Info_Print(("[MBOX] message send! ID: %d (%x[byte])\n", MID_H265_SR_CONT, sizeof(T_H265_SR_CONT_MBOX)));
	ercd = OS_User_Snd_Mbx( MID_H265_SR_CONT, (T_OS_USER_MSG*)mbox_buff );
	if( ercd != D_OS_USER_E_OK ){
		H265_Err_Print(("H265 OS_User_Snd_Mbx(): ercd=%x, mboxid=%d\n", ercd, MID_H265_SR_CONT ));
	}
	return ( ercd );
}

static INT32 H265_Sirius_Get_Msg( T_H265_SR_CONT_MSG* msg )
{
	OS_USER_ER		ercd;
	T_H265_SR_CONT_MBOX	*mbox_buff;
	
	/* [mbox] */
	ercd = OS_User_Rcv_Mbx( MID_H265_SR_CONT, (T_OS_USER_MSG**)&mbox_buff);
	if( ercd != D_OS_USER_E_OK ){
		if( ercd != D_OS_USER_E_TMOUT ){
			H265_Err_Print(("H265 OS_User_Rcv_Mbx/OS_User_Prcv_Mbx(): ercd=%x, mboxid=%d\n", ercd, MID_H265_SR_CONT ));
		}
		return ( ercd );
	}
#if 0	// for Debug
	H265_Info_Print(("[MBOX] message received! ID: %d (%x[byte])\n", MID_H265_SR_CONT, sizeof(T_H265_SR_CONT_MBOX)));
#endif
	
	/* [copy] */
	memcpy( msg, mbox_buff->msg, sizeof(T_H265_SR_CONT_MSG) );
	
	return ( ercd );
}

static INT32 H264_Sirius_Clear_Msg( VOID )
{
	OS_USER_ER			ercd;
	T_H265_SR_CONT_MBOX	*mbox_buff;
	
	sr_cont_count = 0;
	
	while(1){
		ercd = OS_User_Prcv_Mbx(MID_H265_SR_CONT, (T_OS_USER_MSG**)&mbox_buff);
		if( ercd != D_OS_USER_E_OK ){
			if( ercd != D_OS_USER_E_TMOUT ){
				H265_Err_Print(("H265 OS_User_Prcv_Mbx(): ercd=%x, mboxid=%d\n", ercd, MID_H265_SR_CONT ));
			}
			break;
		}
	}
	return ( ercd );
}


//----------------------
//	Dummy Handler Process
//----------------------
VOID H265_Sirius_dummy_system_inHandler( VOID ){
	T_H265_SR_CONT_MSG	msg;
	INT32				ret;
	
	msg.type = D_H265_INT_TYPE_SYS;
	msg.data = SR_Int_CpueintRead_Sys();	// read & clear flag
	
	// send msg
	ret = H265_Sirius_Set_Msg( &msg );
	if( ret != D_OS_USER_E_OK ){
		H265_Err_Print(( "H265_Sirius_dummy_system_inHandler : H265_Sirius_Set_Msg ng(%d)\n", ret ));
	}
}

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
VOID H265_Sirius_dummy_enc_inHandler( VOID ){
	T_H265_SR_CONT_MSG	msg;
	INT32				ret;
	
	msg.type = D_H265_INT_TYPE_ENC;
	msg.data = SR_Int_CpueintRead_Enc();	// read & clear flag
	
	// send msg
	ret = H265_Sirius_Set_Msg( &msg );
	if( ret != D_OS_USER_E_OK ){
		H265_Err_Print(( "H265_Sirius_dummy_enc_inHandler : H265_Sirius_Set_Msg ng(%d)\n", ret ));
	}
}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

VOID H265_Sirius_dummy_dec_inHandler( VOID ){
	T_H265_SR_CONT_MSG	msg;
	INT32				ret;
	
	msg.type = D_H265_INT_TYPE_DEC;
	msg.data = SR_Int_CpueintRead_Dec();	// read & clear flag
	
	// send msg
	ret = H265_Sirius_Set_Msg( &msg );
	if( ret != D_OS_USER_E_OK ){
		H265_Err_Print(( "H265_Sirius_dummy_dec_inHandler : H265_Sirius_Set_Msg ng(%d)\n", ret ));
	}
}

VOID H265_Sirius_Cont_Process( VOID ){
	T_H265_SR_CONT_MSG	msg;
	INT32				ret;
	
	while( 1 ){
		ret = H265_Sirius_Get_Msg( &msg );
		if( ret != D_OS_USER_E_OK ){
			H265_Err_Print(( "H265_Sirius_Cont_Process : H265_Sirius_Get_Msg ng(%d)\n", ret ));
			continue;
		}
		if( msg.type == D_H265_INT_TYPE_SYS ){
			SR_Int_Cpueint_Sys( msg.data );
		}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		if( msg.type == D_H265_INT_TYPE_ENC ){
			SR_Int_Cpueint_Enc( msg.data );
		}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		if( msg.type == D_H265_INT_TYPE_DEC ){
			SR_Int_Cpueint_Dec( msg.data );
		}
	}
}

//----------------------
//	Handler Process 
//----------------------
/**
 *	@brief		H265(SIRIUS Macro) Interrupt Handller for WDT(Watch dog timer).\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h265_sirius_system_intHandler( UINT32 ch, UINT32 status )
{
//	H265_Info_Print(( "Int h265_sirius_system_intHandler(%x)(%x)\n",ch ,status ));
	
	if( status & SR_SYSTEM_SYSTEMBOOT ){
		OS_User_Set_Flg( FID_SIRIUS, FLG_SIRIUS_SYSTEM_BOOT_COMP );
	}
	if( status & SR_SYSTEM_VIDEOBOOT ){
		OS_User_Set_Flg( FID_SIRIUS, FLG_SIRIUS_VIDEO_BOOT_COMP );
	}
	
	return;
}

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
/**
 *	@brief		H265(SIRIUS Macro) Interrupt Handller for Encode.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h265_sirius_enc_intHandler( UINT32 ch, UINT32 status )
{
//	H265_Info_Print(( "Int h265_sirius_enc_intHandler(%x)(%x)\n",ch ,status ));
	
	if( status & SR_ENCODE_START_DONE ){
		if( ch == 0 ){
			OS_User_Set_Flg( FID_SIRIUS, FLG_SIRIUS_CODEC_ENC_START_COMP0 );
		}
		else{
			OS_User_Set_Flg( FID_SIRIUS, FLG_SIRIUS_CODEC_ENC_START_COMP1 );
		}
	}
	if( status & SR_ENCODE_STOP_DONE ){
		if( ch == 0 ){
#ifdef CO_H265_DEBUG_MBA
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_HEVC, FJ_MBALOG_TYPE_POINT, "stop_done" ));
#endif
			g_sr_stop_done_flg[0] = 1;
			OS_User_Set_Flg( FID_H265, FLG_H265_CODEC_STOP_COMP0 );
		}
		else{
			g_sr_stop_done_flg[1] = 1;
			OS_User_Set_Flg( FID_H265, FLG_H265_CODEC_STOP_COMP1 );
		}
	}
	if( status & SR_ENCODE_SLICE_DONE ){
		if( ch == 0 ){
#ifdef CO_H265_DEBUG_MBA
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_HEVC, FJ_MBALOG_TYPE_POINT, "slice_done" ));
#endif
			OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VSLICE_COMP0 );
		}
		else{
			OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VSLICE_COMP1 );
		}
	}
	if( status & SR_ENCODE_FRAME_DONE ){
		if( ch == 0 ){
#ifdef CO_H265_DEBUG_MBA
			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_HEVC, FJ_MBALOG_TYPE_POINT, "frame_done" ));
#endif
			OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VFRAME_COMP0_TO_CC );
			OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VFRAME_COMP0_TO_ENT );
		}
		else{
			OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VFRAME_COMP1_TO_CC );
			OS_User_Set_Flg( FID_H265, FLG_H265_ENC_VFRAME_COMP1_TO_ENT );
		}
	}
	if( status & SR_ENCODE_ERROR ){
#ifdef CO_H265_DEBUG_MBA
		M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_HEVC, FJ_MBALOG_TYPE_POINT, "error" ));
#endif
		H265_Err_Print(( "Error Interrupt(%d)\n",ch));
		SR_Encode_Error_Info_Get( ch, (T_SR_ENC_ERROR_FACTOR *)&errfactor );
		H265_Err_Print(( "encctlrrlevel   = %#x\n", errfactor.encctlrrlevel   ));
		H265_Err_Print(( "vencerrlevel    = %#x\n", errfactor.vencerrlevel    ));
		H265_Err_Print(( "encctlerrfactor = %#x\n", errfactor.encctlerrfactor ));
		H265_Err_Print(( "vencerrfactor   = %#x\n", errfactor.vencerrfactor   ));
		OS_User_Set_Flg( FID_H265, FLG_H265_FORCE_STOP );
	}
	
	return;
}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

/**
 *	@brief		H265(SIRIUS Macro) Interrupt Handller for Decode.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID h265_sirius_dec_intHandler( UINT32 ch, UINT32 status )
{
	T_SR_DEC_ERROR_FACTOR errfactor;
	
//	H265_Info_Print(( "Int h265_sirius_dec_intHandler(%x)(%x)\n",ch ,status ));
	
	if( status & SR_DECODE_START_DONE ){
		OS_User_Set_Flg( FID_SIRIUS, FLG_SIRIUS_CODEC_DEC_START_COMP );
	}
	if( status & SR_DECODE_STOP_DONE ){
		OS_User_Set_Flg( FID_SIRIUS, FLG_SIRIUS_CODEC_DEC_STOP_COMP );
		OS_User_Set_Flg( FID_H265, FLG_H265_CODEC_FLUSH_COMP );
	}
	if( status & SR_DECODE_FRAME_DONE ){
		OS_User_Set_Flg( FID_H265, FLG_H265_DEC_FRAMEEND_COMP );
		OS_User_Set_Flg( FID_H265, FLG_H265_DEC_STREAM_INPUT );
	}
	if( status & SR_DECODE_TIMEOUT ){
		H265_Err_Print(( "Error SR_DECODE_TIMEOUT(%d)\n",ch));
		OS_User_Set_Flg( FID_H265, FLG_H265_DEC_ERROR );
	}
	if( status & SR_DECODE_EOB_DETECT ){
		;
	}
	if( status & SR_DECODE_ERROR ){
		H265_Err_Print(( "Error Interrupt(%d)\n",ch));
		SR_Decode_Error_Info_Get ( 0, (T_SR_DEC_ERROR_FACTOR *)&errfactor );
		H265_Err_Print(( "decctlerrfactor = %#x\n", errfactor.decctlerrfactor ));
		OS_User_Set_Flg( FID_H265, FLG_H265_DEC_ERROR );
	}
	
	return;
}

static VOID h265_sirius_remap_address_set( UINT32 mode, T_SR_BRIDGE_DDR_CONFIG* ddr_config )
{
	LLONG	remap_address;
/*
---------------------------------------------------------------------
	bridge_param
	----
	ddr_ch0_top_addr        <-- [1]
	ddr_ch0_top_common_addr <-- [3]
	ddr_ch0_common_size
	ddr_ch1_top_addr        <-- [4]
	ddr_ch1_top_common_addr <-- [6]
	ddr_ch1_common_size
	----
	
	sirius view
	-----
	0x80000000- ch0  <-- [2]
	0xBFFFFFFF
	0xC0000000- ch1  <-- [5]
	0xFFFFFFFF
	-----
	
	ch0 UMC REMAP(DS0/DS1) = [1] + [2] - [3]
	ch1 UMC REMAP(DS0/DS1) = [4] + [5] - [6]
	
	ch0 UMC REMAP(SDM/VE0/VE1/VE2/VD0/VD1/VD2) =  [1] - [3]
	ch1 UMC REMAP(SDM/VE0/VE1/VE2/VD0/VD1/VD2) =  [4] - [6]
---------------------------------------------------------------------
*/
	// ch0 UMC REMAP(DS0/DS1)
	remap_address = (LLONG)ddr_config->ddr_ch0_top_addr + 0x80000000 - ddr_config->ddr_ch0_top_common_addr;
	
	H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_DS0, (LONG)remap_address );
	H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_DS1, (LONG)remap_address );
	
	// ch1 UMC REMAP(DS0/DS1)
	remap_address = (LLONG)ddr_config->ddr_ch1_top_addr + 0xC0000000 - ddr_config->ddr_ch1_top_common_addr;
	
	H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_DS0, (LONG)remap_address );
	H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_DS1, (LONG)remap_address );

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	// ch0 UMC REMAP(SDM/VE0/VE1/VE2/VD0/VD1/VD2)
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	// ch0 UMC REMAP(SDM/VD0/VD1/VD2)
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	remap_address = (LLONG)ddr_config->ddr_ch0_top_addr - ddr_config->ddr_ch0_top_common_addr;
	
	H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_SDM, (LONG)remap_address );
	
	if( mode == H265_MODE_PLAY ){
		// Play
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VD0, (LONG)remap_address );
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VD1, (LONG)remap_address );
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VD2, (LONG)remap_address );
	}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	else if( mode == H265_MODE_RECORD ){
		// Record
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VE0, (LONG)remap_address );
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VE1, (LONG)remap_address );
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VE2, (LONG)remap_address );
	}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	else{
		// Transcode
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VE0, (LONG)remap_address );
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VE1, (LONG)remap_address );
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VE2, (LONG)remap_address );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VD0, (LONG)remap_address );
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VD1, (LONG)remap_address );
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VD2, (LONG)remap_address );
	}
	
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	// ch1 UMC REMAP(SDM/VE0/VE1/VE2/VD0/VD1/VD2)
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	// ch1 UMC REMAP(SDM/VD0/VD1/VD2)
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	remap_address = (LLONG)ddr_config->ddr_ch1_top_addr - ddr_config->ddr_ch1_top_common_addr;
	
	H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_SDM, (LONG)remap_address );
	
	if( mode == H265_MODE_PLAY ){
		// Play
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VD0, (LONG)remap_address );
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VD1, (LONG)remap_address );
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VD2, (LONG)remap_address );
	}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	else if( mode == H265_MODE_RECORD ){
		// Record
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VE0, (LONG)remap_address );
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VE1, (LONG)remap_address );
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VE2, (LONG)remap_address );
	}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	else{
		// Transcode
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VE0, (LONG)remap_address );
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VE1, (LONG)remap_address );
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VE2, (LONG)remap_address );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VD0, (LONG)remap_address );
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VD1, (LONG)remap_address );
		H265_Movie_Sdramc_Remap_Address_Set( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VD2, (LONG)remap_address );
	}
	
	return;
}


static VOID h265_sirius_sdramc_clock_enable( UINT32 mode, E_H265_SDRAMC_CLOCK_EN enable )
{
	if( enable == E_H265_SDRAMC_CLOCK_EN_ENABLE ){
		// Common
		// SDRAM ch0
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_DS0, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_DS1, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_SDM, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_DS01, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_SDM, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		// SDRAM ch1
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_DS0, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_DS1, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_SDM, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_DS01, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_SDM, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		
		if( mode == H265_MODE_PLAY ){
			// Play
			// SDRAM ch0
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VD0, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VD1, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VD2, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_VD01, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_VD2, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			// SDRAM ch1
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VD0, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VD1, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VD2, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_VD01, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_VD2, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		else if( mode == H265_MODE_RECORD ){
			// Record
			// SDRAM ch0
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VE0, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VE1, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VE2, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_VE012, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			// SDRAM ch1
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VE0, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VE1, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VE2, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_VE012, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		else{
			// Transcode
			// SDRAM ch0
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VE0, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VE1, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VE2, E_H265_SDRAMC_CLOCK_EN_ENABLE );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VD0, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VD1, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VD2, E_H265_SDRAMC_CLOCK_EN_ENABLE );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
			H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_VE012, E_H265_SDRAMC_CLOCK_EN_ENABLE );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
			H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_VD01, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_VD2, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			// SDRAM ch1
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VE0, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VE1, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VE2, E_H265_SDRAMC_CLOCK_EN_ENABLE );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VD0, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VD1, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VD2, E_H265_SDRAMC_CLOCK_EN_ENABLE );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
			H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_VE012, E_H265_SDRAMC_CLOCK_EN_ENABLE );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
			H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_VD01, E_H265_SDRAMC_CLOCK_EN_ENABLE );
			H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_VD2, E_H265_SDRAMC_CLOCK_EN_ENABLE );
		}
	}
	else{
		// SDRAM ch0
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_DS0, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_DS1, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_SDM, E_H265_SDRAMC_CLOCK_EN_DISABLE );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VE0, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VE1, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VE2, E_H265_SDRAMC_CLOCK_EN_DISABLE );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VD0, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VD1, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_PORT_VD2, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_DS01, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_SDM, E_H265_SDRAMC_CLOCK_EN_DISABLE );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_VE012, E_H265_SDRAMC_CLOCK_EN_DISABLE );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_VD01, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_0, E_H265_SDRAMC_DATABUF_VD2, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		
		// SDRAM ch1
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_DS0, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_DS1, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_SDM, E_H265_SDRAMC_CLOCK_EN_DISABLE );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VE0, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VE1, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VE2, E_H265_SDRAMC_CLOCK_EN_DISABLE );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VD0, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VD1, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_BusIf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_PORT_VD2, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_DS01, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_SDM, E_H265_SDRAMC_CLOCK_EN_DISABLE );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_VE012, E_H265_SDRAMC_CLOCK_EN_DISABLE );
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_VD01, E_H265_SDRAMC_CLOCK_EN_DISABLE );
		H265_Movie_Sdramc_DataBuf_Clock_Enable( E_H265_SDRAMC_CH_1, E_H265_SDRAMC_DATABUF_VD2, E_H265_SDRAMC_CLOCK_EN_DISABLE );
	}
	
	return;
}


/**
 *	@brief		SIRIUS init Process.
 *	@param		IN :codec
 *				   :bridge_param
 *				OUT:None
 *	@return		H265_OK	Normal End
 *	@return		H265_NG	Abnormal End
 *	@note		None
 *	@attention	None
 */
INT32 H265_Sirius_Initialize( UINT32 mode, T_SR_BRIDGE_MAP_PARAM *bridge_map )
{
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	INT32			ret;
	
	H265_Info_Print(( "H265_Sirius_Initialize Start\n" ));
	
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	g_sr_stop_done_flg[0] = 0;
	g_sr_stop_done_flg[1] = 0;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	
	g_sr_stop_done_flg[0] = 0;
	g_sr_stop_done_flg[1] = 0;
	
	// flag clear (Sirius)
	OS_User_Clr_Flg( FID_SIRIUS, 0 );
	
	// clear sirius msg box
	H264_Sirius_Clear_Msg();
	
	// start sirius control task
	OS_User_Sta_Tsk( TID_H265_SIRIUS_CONT, 0 );
	
	// clock stop
	H265_Sirius_Clock_Stop();
	
	// SDRAMC clock enable
	h265_sirius_sdramc_clock_enable( mode, E_H265_SDRAMC_CLOCK_EN_ENABLE );
	
	// remap address set.
	h265_sirius_remap_address_set( mode, &bridge_map->ddr_config );
	
	// init Bridge
	ret = SR_Bridge_Set_MemoryMap( bridge_map );
	if( ret != SR_OK ){
		H265_Err_Print(( "Sirius Bridge NG\n" ));
		H265_Sirius_Clock_Stop();
		return H265_NG;
	}
	
	// for system boot
	if( mode == H265_MODE_PLAY ){
		ret = SR_Cpu_System_Boot( h265_sirius_system_intHandler, NULL, h265_sirius_dec_intHandler, SR_OFF );
	}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	else if( mode == H265_MODE_RECORD ){
		ret = SR_Cpu_System_Boot( h265_sirius_system_intHandler, h265_sirius_enc_intHandler, NULL, SR_OFF );
	}
	else{
		ret = SR_Cpu_System_Boot( h265_sirius_system_intHandler, h265_sirius_enc_intHandler, h265_sirius_dec_intHandler, SR_OFF );
	}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	
#ifndef CO_DEBUG_ON_PC
	// wait system boot complete
	ercd = OS_User_Twai_Flg( FID_SIRIUS, FLG_SIRIUS_SYSTEM_BOOT_COMP, D_OS_USER_TWF_ORW, &flgptn, D_H265_SIRIUS_BOOT_TIMEOUT );
	if( ercd == D_OS_USER_E_TMOUT ){
		H265_Err_Print(( "Sirius System Boot Time Out\n" ));
		return H265_NG;
	}
	
	// for video boot
	ret = SR_Cpu_Video_Boot();
	
	// wait video boot complete
	ercd = OS_User_Twai_Flg( FID_SIRIUS, FLG_SIRIUS_VIDEO_BOOT_COMP, D_OS_USER_TWF_ORW, &flgptn, D_H265_SIRIUS_BOOT_TIMEOUT );
	if( ercd == D_OS_USER_E_TMOUT ){
		H265_Err_Print(( "Sirius Video Boot Time Out\n" ));
		return H265_NG;
	}
#endif
	
	H265_Info_Print(( "H265_Sirius_Initialize End\n" ));
	
	return H265_OK;
}

/**
 *	@brief		SIRIUS Fainaraiz Process.
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H265_Sirius_Finalize( VOID )
{
	H265_Info_Print(( "H265_Sirius_Finalize Start\n" ));
	
	SR_Cpu_System_Reset();
	
	// end sirius control task
	OS_User_Ter_Tsk( TID_H265_SIRIUS_CONT );
	
	// SDRAMC clock disable
	h265_sirius_sdramc_clock_enable( H265_MODE_COMMON, E_H265_SDRAMC_CLOCK_EN_DISABLE );
	
	H265_Info_Print(( "H265_Sirius_Finalize End\n" ));
	return;
}

/**
 *	@brief		Stop MPCKST and execute Soft Reset(H265SR).\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID H265_Sirius_Clock_Stop( VOID )
{
	H265_Movie_Sirius_IPP_Clock_Stop( E_H265_TOP_CLOCK_STOP_ON );			// clock Stop
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	H265_Movie_Sirius_PXF_Clock_Stop( E_H265_TOP_CLOCK_STOP_ON );			// clock Stop
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	H265_Movie_Sirius_VDF_Clock_Stop( E_H265_TOP_CLOCK_STOP_ON );			// clock Stop
	
	OS_User_Tslp_Tsk(1);
	
	H265_Movie_Sirius_IPP_ASYNC_Reset( E_H265_TOP_RESET_ON );				// HEIPPASYNCSR reset
	H265_Movie_Sirius_IPP_SYNC_Reset( E_H265_TOP_RESET_ON );				// HEIPPSYNCSR  reset
	H265_Movie_Sirius_VDF_Reset( E_H265_TOP_RESET_ON );						// HEVDFSR      reset
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	H265_Movie_Sirius_PXF_Reset( E_H265_TOP_RESET_ON );						// HEPXFSR      reset
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	
	return;
}

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
//----------------------
//	Encode Function
//----------------------
INT32 H265_Sirius_Enc_Start( UINT32 ch, T_SR_ENC_INITPARAM* initparam, T_SR_ENC_FM_TABLE *fmtable )
{
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	INT32			ret;
	
	// enc start request
	ret = SR_Encode_Start( ch, initparam, fmtable );
	if( ret != SR_OK ){
		H265_Err_Print(( "SR_Encode_Start(%d) error!!\n",ch ));
		return H265_NG;
	}
	
#ifndef CO_DEBUG_ON_PC
	// wait response
	if( ch == 0 ){
		ercd = OS_User_Twai_Flg( FID_SIRIUS, FLG_SIRIUS_CODEC_ENC_START_COMP0, D_OS_USER_TWF_ORW, &flgptn, D_H265_SIRIUS_START_TIMEOUT );
	}
	else{
		ercd = OS_User_Twai_Flg( FID_SIRIUS, FLG_SIRIUS_CODEC_ENC_START_COMP1, D_OS_USER_TWF_ORW, &flgptn, D_H265_SIRIUS_START_TIMEOUT );
	}
	
	if( ercd == D_OS_USER_E_OK ){
		H265_Warning_Print(( "Enc_Start Complete(%d)!!\n", ch ));
	}
	else{
		H265_Warning_Print(( "Enc_Start Complete NG(%d)!!\n", ch ));
		return H265_NG;
	}
#endif
	return H265_OK;
}

INT32 H265_Sirius_Enc_Stop( UINT32 ch )
{
	INT32			ret;
	
	H265_Info_Print(( "SR_Encode_Stop() ch(%d)\n", ch ));
	
#ifdef CO_H265_DEBUG_MBA
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_HEVC, FJ_MBALOG_TYPE_POINT, "stop" ));
#endif
	// enc stop request
	ret = SR_Encode_Stop( ch );
	if( ret != SR_OK ){
		H265_Err_Print(( "SR_Encode_Stop(%d) error!!\n",ch ));
		return H265_NG;
	}
	
	return H265_OK;
}

INT32 H265_Sirius_Enc_Fm_Free_Get( UINT32 ch, T_SR_ENC_FM_TABLE *fmtable )
{
	INT32			ret;
//	INT32 i;
	
#ifdef CO_H265_DEBUG_MBA
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_HEVC, FJ_MBALOG_TYPE_POINT, "free_get" ));
#endif
	ret = SR_Encode_Fm_Free_Get( ch,  fmtable );
	if( ret != SR_OK ){
		H265_Err_Print(( "SR_Encode_Fm_Free_Get(%d) error!!\n",ch ));
		return H265_NG;
	}
	
//	for(i=0; i<ENC_FM_NUM; i++){
//		if( fmtable->use_flag[i] == 0 ){
//			M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_HEVC, FJ_MBALOG_TYPE_POINT, "free%d", i ));
//		}
//	}
	
	return H265_OK;
}

INT32 H265_Sirius_Enc_Frame( UINT32 ch, T_SR_ENC_PARAM *vecparam, T_SR_ENC_FM_TABLE *fmtable )
{
	INT32			ret;
	
#ifdef CO_H265_DEBUG_MBA
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_HEVC, FJ_MBALOG_TYPE_POINT, "enc_f %d", vecparam->serial_num ));
#endif
	ret = SR_Encode_Frame( ch, vecparam, fmtable );
	if( ret != SR_OK ){
#ifdef CO_H265_DEBUG_MBA
		H265_Err_Print(( "SR_Encode_Frame(%d) %d error!!\n",ch, vecparam->serial_num ));
#else
		H265_Err_Print(( "SR_Encode_Frame(%d) error!!\n",ch ));
#endif
		return H265_NG;
	}
	
	return H265_OK;
}

INT32 H265_Sirius_Enc_Veb_Info_Set( UINT32 ch, UINT32 picture_size )
{
	INT32			ret;
	
#ifdef CO_H265_DEBUG_MBA
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_HEVC, FJ_MBALOG_TYPE_POINT, "veb %d", picture_size ));
#endif
	ret = SR_Encode_Veb_Info_Set( ch, picture_size );
	if( ret != SR_OK ){
		if( g_sr_stop_done_flg[ch] != 1 ){
#ifdef CO_H265_DEBUG_MBA
			H265_Err_Print(( "SR_Encode_Veb_Info_Set(%d) %d error!!\n",ch, picture_size ));
#else
			H265_Err_Print(( "SR_Encode_Veb_Info_Set(%d) error!!\n",ch ));
#endif
			return H265_NG;
		}
	}
	
	return H265_OK;
}

INT32 H265_Sirius_Enc_Frame_Info_Get( UINT32 ch, T_SR_ENC_VINFO *vinfo )
{
	INT32			ret;
	
	ret = SR_Encode_Frame_Info_Get( ch, vinfo );
	if( ret != SR_OK ){
		H265_Err_Print(( "SR_Encode_Frame_Info_Get(%d) error!!\n",ch ));
		return H265_NG;
	}
	
#ifdef CO_H265_DEBUG_MBA
//	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_HEVC, FJ_MBALOG_TYPE_POINT, "Enc2 %d", vinfo->serial_num ));
	M_MBA_PRINTF(( D_MBA_KIND_MOVIE, FJ_MBALOG_FUNC_ID_HEVC, FJ_MBALOG_TYPE_POINT, "frame_get %d, %d", vinfo->serial_num, vinfo->picture_size ));
#endif
	
	return H265_OK;
}

VOID H265_Sirius_Enc_Fm_Empty_Init( UINT32 ch )
{
	UINT32 idx = 0;
	
//	H265_Info_Print(( "H265_Sirius_Enc_Fm_Empty_Init Start\n" ));
	
	for( idx = 0 ; idx < ENC_FM_NUM ; idx++ ){
		gFm_Empty_Fifo[ch].index[idx] = idx;
	}
	gFm_Empty_Fifo[ch].idx_get  = 0;			/* for FieldGet */
	gFm_Empty_Fifo[ch].idx_set  = 0;			/* for FieldSet */
	gFm_Empty_Fifo[ch].full_flg = H265_ON;	/* empty table is full */
	gFm_Empty_Fifo[ch].emp_flg  = H265_OFF;	/* empty table is full */
	
	return;
}

INT32 H265_Sirius_Enc_Fm_Empty_Field_Set( UINT32 ch, UINT32 empty_index )
{
	if( gFm_Empty_Fifo[ch].full_flg == H265_OFF ){
#if 0
		H265_Info_Print(( "FS.i=[%d]\n", empty_index ));
#endif
		/* empty table is Nothing */
		gFm_Empty_Fifo[ch].index[ gFm_Empty_Fifo[ch].idx_set ] = empty_index;
		gFm_Empty_Fifo[ch].idx_set++;
		
		if( gFm_Empty_Fifo[ch].idx_set >= ENC_FM_NUM ){
			gFm_Empty_Fifo[ch].idx_set = 0;
		}
		
		if( gFm_Empty_Fifo[ch].idx_set == gFm_Empty_Fifo[ch].idx_get ){
			/* empty table is full */
			gFm_Empty_Fifo[ch].full_flg = H265_ON;
		}
		else {
			/* empty table is full */
			gFm_Empty_Fifo[ch].emp_flg = H265_OFF;
		}
	}
	else{
		/* empty table is full */
		H265_Err_Print(( "Fm_Empty is Full\n" ));
		
		return H265_NG;
	}
	
	return H265_OK;
}

INT32 H265_Sirius_Enc_Fm_Empty_Field_Get( UINT32 ch, UINT32* empty_index )
{
	if( gFm_Empty_Fifo[ch].emp_flg == H265_OFF ){
		/* empty table is full */
		if( gFm_Empty_Fifo[ch].idx_get == gFm_Empty_Fifo[ch].idx_set ){
			/* empty table is Nothing */
			gFm_Empty_Fifo[ch].full_flg = H265_OFF;
		}
		
		*empty_index = gFm_Empty_Fifo[ch].index[ gFm_Empty_Fifo[ch].idx_get ];
		gFm_Empty_Fifo[ch].idx_get++;
#if 0
		H265_Info_Print(( "FG.i=[%d]\n", *empty_index ));
#endif		
		
		if( gFm_Empty_Fifo[ch].idx_get >= ENC_FM_NUM ){
			gFm_Empty_Fifo[ch].idx_get = 0;
		}
		
		if( gFm_Empty_Fifo[ch].idx_get == gFm_Empty_Fifo[ch].idx_set ){
			/* empty table is Nothing */
			gFm_Empty_Fifo[ch].emp_flg = H265_ON;
		}
	}
	else{
		/* empty table is Nothing */
		H265_Err_Print(( "Fm_Empty is Nothing\n" ));
		return H265_NG;
	}
	
	return H265_OK;
}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

//----------------------
//	Decode Function
//----------------------
INT32 H265_Sirius_Dec_Start( UINT32 mode )
{
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	INT32			ret;
	
	H265_Info_Print(( "SR_Decode_Start() : mode = (%x)\n", mode ));
	
	// enc start request
	ret = SR_Decode_Start( 0, mode );
	if( ret != SR_OK ){
		H265_Err_Print(( "SR_Decode_Start error!!\n" ));
		return H265_NG;
	}
	
#ifndef CO_DEBUG_ON_PC
	// wait response
	ercd = OS_User_Twai_Flg( FID_SIRIUS, FLG_SIRIUS_CODEC_DEC_START_COMP, D_OS_USER_TWF_ORW, &flgptn, D_H265_SIRIUS_START_TIMEOUT );
	if( ercd == D_OS_USER_E_OK ){
		H265_Warning_Print(( "Dec_Start Complete!!\n" ));
	}
	else{
		H265_Warning_Print(( "Dec_Start Complete NG!!\n" ));
		return H265_NG;
	}
#endif
	
	return H265_OK;
}

INT32 H265_Sirius_Dec_Stop( VOID )
{
#ifndef CO_DEBUG_ON_PC
	OS_USER_FLGPTN	flgptn;
	OS_USER_ER		ercd=0;
#endif
	INT32			ret;
	
	// dec stop request
	ret = SR_Decode_Stop( 0 );
	if( ret != SR_OK ){
		H265_Err_Print(( "SR_Decode_Stop error!!\n" ));
		return H265_NG;
	}
	
#ifndef CO_DEBUG_ON_PC
	// wait response
	ercd = OS_User_Twai_Flg( FID_SIRIUS, FLG_SIRIUS_CODEC_DEC_STOP_COMP, D_OS_USER_TWF_ORW, &flgptn, D_H265_SIRIUS_START_TIMEOUT );
	if( ercd == D_OS_USER_E_OK ){
		H265_Warning_Print(( "Dec_Stop Complete!!\n" ));
	}
	else{
		H265_Warning_Print(( "Dec_Stop Complete NG!!\n" ));
		return H265_NG;
	}
#endif
	
#ifdef CO_DEBUG_ON_PC
	OS_User_Set_Flg( FID_H265, FLG_H265_CODEC_FLUSH_COMP );
#endif
	
	return H265_OK;
}

INT32 H265_Sirius_Dec_Frame( T_SR_DEC_PICTURE_INFO *picture_info, UINT32 *alloc_result )
{
	INT32			ret;
	
	ret = SR_Decode_Frame( 0, picture_info, alloc_result );
	if( ret != SR_OK ){
		H265_Err_Print(( "SR_Decode_Frame error!!\n" ));
		return H265_NG;
	}
	
#ifdef CO_DEBUG_ON_PC
	OS_User_Set_Flg( FID_H265, FLG_H265_DEC_FRAMEEND_COMP );
	OS_User_Set_Flg( FID_H265, FLG_H265_DEC_STREAM_INPUT );
#endif
	
	return H265_OK;
}

INT32 H265_Sirius_Dec_Frame_Alloc( T_SR_FM_OUT_INFO *fminfo, UINT32 *alloc_result )
{
	INT32			ret;
	
	ret = SR_Decode_Frame_Alloc( 0, fminfo, alloc_result );
	if( ret != SR_OK ){
		H265_Err_Print(( "SR_Decode_Frame_Alloc error!!\n" ));
		return H265_NG;
	}
	
	return H265_OK;
}

INT32 H265_Sirius_Dec_Frame_Keepid( UINT32 id )
{
	INT32			ret;
	
	ret = SR_Decode_Frame_Keepid( 0, id );
	if( ret != SR_OK ){
		H265_Err_Print(( "SR_Decode_Frame_Keepid error!!\n" ));
		return H265_NG;
	}
	
	return H265_OK;
}

INT32 H265_Sirius_Dec_Frame_Free( UINT32 id )
{
	INT32			ret;
	
	ret = SR_Decode_Frame_Free( 0, id );
	if( ret != SR_OK ){
		H265_Err_Print(( "SR_Decode_Frame_Free error!!\n" ));
		return H265_NG;
	}
	
	return H265_OK;
}


/*----------------------------------------------------------------------*/
/* User Function for Sirius Driver										*/
/*----------------------------------------------------------------------*/

VOID H265_Sirius_Power_Set( UCHAR arg )
{
	// 0	IPP PMU ON
	// 1	IPP PMU OFF
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	// 2	PXF PMU ON
	// 3	PXF PMU OFF
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	// 4	VDF PMU ON
	// 5	VDF PMU OFF
	
	H265_Info_Print(( "H265_Sirius_Power_Set : arg = (%x)\n",arg ));
	
	if( arg == 0 ){
		H265_Info_Print(( "H265_Sirius_Power_Set : IPP PMU ON\n" ));
		H265_Movie_PMU_Setting( E_H265_PMU_BLOCK_IPP, H265_ON );
	}
	else if( arg == 1 ){
		H265_Info_Print(( "H265_Sirius_Power_Set : IPP PMU OFF\n" ));
		H265_Movie_PMU_Setting( E_H265_PMU_BLOCK_IPP, H265_OFF );
	}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	else if( arg == 2 ){
		H265_Info_Print(( "H265_Sirius_Power_Set : PXF PMU ON\n" ));
		H265_Movie_PMU_Setting( E_H265_PMU_BLOCK_PXF, H265_ON );
	}
	else if( arg == 3 ){
		H265_Info_Print(( "H265_Sirius_Power_Set : PXF PMU OFF\n" ));
		H265_Movie_PMU_Setting( E_H265_PMU_BLOCK_PXF, H265_OFF );
	}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	else if( arg == 4 ){
		H265_Info_Print(( "H265_Sirius_Power_Set : VDF PMU ON\n" ));
		H265_Movie_PMU_Setting( E_H265_PMU_BLOCK_VDF, H265_ON );
	}
	else if( arg == 5 ){
		H265_Info_Print(( "H265_Sirius_Power_Set : VDF PMU OFF\n" ));
		H265_Movie_PMU_Setting( E_H265_PMU_BLOCK_VDF, H265_OFF );
	}
	else{
		;
	}
	
	return;
}
		
VOID H265_Sirius_Reset_Set( UCHAR arg )
{
	// 0	IPP Asynchronous reset
	// 1	IPP Asynchronous reset release
	// 2	IPP Synchronous reset
	// 3	IPP Synchronous reset release
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	// 4	PXF reset
	// 5	PXF reset release
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	// 6	VDF reset
	// 7	VDF reset release
	
	H265_Info_Print(( "H265_Sirius_Reset_Set : arg = (%x)\n",arg ));
	
	if( arg == 0 ){
		H265_Info_Print(( "H265_Sirius_Reset_Set : IPP_ASYNC_Reset = (reset)\n" ));
		H265_Movie_Sirius_IPP_ASYNC_Reset( E_H265_TOP_RESET_ON );	// HEIPPASYNCSR reset
	}
	else if( arg == 1 ){
		H265_Info_Print(( "H265_Sirius_Reset_Set : IPP_ASYNC_Reset = (reset off)\n" ));
		H265_Movie_Sirius_IPP_ASYNC_Reset( E_H265_TOP_RESET_OFF );	// HEIPPASYNCSR reset off
	}
	else if( arg == 2 ){
		H265_Info_Print(( "H265_Sirius_Reset_Set : IPP_SYNC_Reset = (reset)\n" ));
		H265_Movie_Sirius_IPP_SYNC_Reset( E_H265_TOP_RESET_ON );	// HEIPPSYNCSR  reset
	}
	else if( arg == 3 ){
		H265_Info_Print(( "H265_Sirius_Reset_Set : IPP_SYNC_Reset = (reset off)\n" ));
		H265_Movie_Sirius_IPP_SYNC_Reset( E_H265_TOP_RESET_OFF );	// HEIPPSYNCSR  reset off
	}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	else if( arg == 4 ){
		H265_Info_Print(( "H265_Sirius_Reset_Set : PXF_Reset = (reset)\n" ));
		H265_Movie_Sirius_PXF_Reset( E_H265_TOP_RESET_ON );			// HEPXFSR      reset
	}
	else if( arg == 5 ){
		H265_Info_Print(( "H265_Sirius_Reset_Set : PXF_Reset = (reset off)\n" ));
		H265_Movie_Sirius_PXF_Reset( E_H265_TOP_RESET_OFF );		// HEPXFSR      reset off
	}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	else if( arg == 6 ){
		H265_Info_Print(( "H265_Sirius_Reset_Set : VDF_Reset = (reset)\n" ));
		H265_Movie_Sirius_VDF_Reset( E_H265_TOP_RESET_ON );			// HEPXFSR      reset
	}
	else if( arg == 7 ){
		H265_Info_Print(( "H265_Sirius_Reset_Set : VDF_Reset = (reset off)\n" ));
		H265_Movie_Sirius_VDF_Reset( E_H265_TOP_RESET_OFF );		// HEPXFSR      reset off
	}
	else{
		;
	}
	
//	H265_Info_Print(( "H265_SIRIUS_Reset_Set : reg = (%x)\n", *((volatile ULONG *)0x1d021090) ));
	
	return;
}

VOID H265_Sirius_Clock_Set( UCHAR arg )
{
	// 0	IPP clock supply ON
	// 1	IPP clock supply OFF
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	// 2	PXF clock supply ON
	// 3	PXF clock supply OFF
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	// 4	VDF clock supply ON
	// 5	VDF clock supply OFF
	
	H265_Info_Print(( "H265_Sirius_Clock_Set : arg = (%x)\n",arg ));
	
	if( arg == 0 ){
		H265_Info_Print(( "H265_Sirius_Clock_Set : IPP_Clock_Stop = (OFF)\n" ));
		H265_Movie_Sirius_IPP_Clock_Stop( E_H265_TOP_CLOCK_STOP_OFF );	// clock Start
	}
	else if( arg == 1 ){
		H265_Info_Print(( "H265_Sirius_Clock_Set : IPP_Clock_Stop = (ON)\n" ));
		H265_Movie_Sirius_IPP_Clock_Stop( E_H265_TOP_CLOCK_STOP_ON );	// clock Stop
	}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	else if( arg == 2 ){
		H265_Info_Print(( "H265_Sirius_Clock_Set : PXF_Clock_Stop = (OFF)\n" ));
		H265_Movie_Sirius_PXF_Clock_Stop( E_H265_TOP_CLOCK_STOP_OFF );	// clock Start
	}
	else if( arg == 3 ){
		H265_Info_Print(( "H265_SIRIUS_Clock_Set : PXF_Clock_Stop = (ON)\n" ));
		H265_Movie_Sirius_PXF_Clock_Stop( E_H265_TOP_CLOCK_STOP_ON );	// clock Stop
	}
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	else if( arg == 4 ){
		H265_Info_Print(( "H265_Sirius_Clock_Set : VDF_Clock_Stop = (OFF)\n" ));
		H265_Movie_Sirius_VDF_Clock_Stop( E_H265_TOP_CLOCK_STOP_OFF );	// clock Start
	}
	else if( arg == 5 ){
		H265_Info_Print(( "H265_Sirius_Clock_Set : VDF_Clock_Stop = (ON)\n" ));
		H265_Movie_Sirius_VDF_Clock_Stop( E_H265_TOP_CLOCK_STOP_ON );	// clock Stop
	}
	else{
		;
	}
	
//	H265_Info_Print(( "H265_Sirius_Clock_Set : reg = (%x)\n", *((volatile ULONG *)0x1D021070) ));
	
	return;
}

VOID H265_Sirius_DSB( VOID )
{
	H265_Movie_Arm_Sirius_Dsb_Poc();
	
	return;
}

UINT32 H265_Sirius_Memory_Read( UINT32 address )
{
#if 1
	UINT32	data;
	
	data = *(volatile UINT32*)(H265_Movie_Sirius_Phy_To_Log( address ));
	
 	return data;
	
#else	// for Debug
	UINT32	data;
	UINT32	conv_addr;
	
	conv_addr = H265_Movie_Sirius_Phy_To_Log( address );
	if( conv_addr == address ){
		H265_Err_Print(( "H265_Sirius_Memory_Read : addr = %#x\n", address ));
	}
	
	data = *(volatile UINT32*)( conv_addr );
	
 	return data;
#endif
}

VOID H265_Sirius_Memory_Write( UINT32 address, UINT32 data)
{
#if 1
	*(volatile UINT32*)(H265_Movie_Sirius_Phy_To_Log( address )) = data;
	
	return;
	
#else	// for Debug
	UINT32	conv_addr;
	
	conv_addr = H265_Movie_Sirius_Phy_To_Log( address );
	if( conv_addr == address ){
		H265_Err_Print(( "H265_Sirius_Memory_Write : addr = %#x\n", address ));
	}
	
	*(volatile UINT32*)(conv_addr) = data;
	
	return;
#endif
}

VOID H265_Sirius_Clean( UINT32 address, UINT32 size )
{
	H265_Movie_Cache_Clean( (ULONG)address, (ULONG)size );
	
	return;
}

VOID H265_Sirius_Flush( UINT32 address, UINT32 size )
{
	H265_Movie_Cache_Flush( (ULONG)address, (ULONG)size );
	
	return;
}

UINT32 H265_Sirius_Dpd( VOID ){
	return H265_Movie_DI();
}

VOID H265_Sirius_Dpe( UINT32 status){
	H265_Movie_EI(status);
}

