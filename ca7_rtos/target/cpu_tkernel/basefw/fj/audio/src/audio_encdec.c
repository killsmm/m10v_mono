/**
 * @file		audio_encdec.c
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "os_user_custom.h"
#include "fj_audio.h"
#include "fj_encoder.h"
#include "fj_decoder.h"
#include "audio.h"
#include "audio_encdec.h"
#include "audio_encoder.h"
#include "audio_decoder.h"
#include "dd_arm.h"
#include "debug.h"
#include "mba_if.h"


/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/
// Nothing Special

#define D_BF_TRANSFER_DATA_SIZE				(9U)
#define D_SIZE_OF_REC_RESUME_ADD			(32U)

#define D_AUDIO_ENCDEC_NUM_AUDIO_PROCESS	(14U)

/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function Prototype Definition                                  */
/*----------------------------------------------------------------------*/

static VOID bf_audio_encdec_init_encoder0( VOID );
static VOID bf_audio_encdec_init_encoder1( VOID );
static VOID bf_audio_encdec_tx_stream_data0( VOID );
static VOID bf_audio_encdec_tx_stream_data1( VOID );
static VOID bf_audio_encdec_start_encode0( VOID );
static VOID bf_audio_encdec_start_encode1( VOID );
static VOID bf_audio_encdec_stop_encode0( VOID );
static VOID bf_audio_encdec_stop_encode1( VOID );
static VOID bf_audio_encdec_init_decoder( VOID );
static VOID bf_audio_encdec_decoder_cb( VOID );
static VOID bf_audio_encdec_dec_req_stream( VOID );
static VOID bf_audio_encdec_start_decode( VOID );
static VOID bf_audio_encdec_stop_decode( VOID );
static VOID bf_audio_encdec_stop_decode_notify( VOID );


/*----------------------------------------------------------------------*/
/* Function Table Definition                                            */
/*----------------------------------------------------------------------*/
static const T_AUDIO_TASK_PROCESS gBF_Audio_EncDec_Process[D_AUDIO_ENCDEC_NUM_AUDIO_PROCESS] = {
	{	FLG_AUDIO_ENCDEC_ENC0_INIT,			bf_audio_encdec_init_encoder0		},
	{	FLG_AUDIO_ENCDEC_ENC1_INIT,			bf_audio_encdec_init_encoder1		},
	{	FLG_AUDIO_ENCDEC_ENC0_TX_DATA,		bf_audio_encdec_tx_stream_data0		},
	{	FLG_AUDIO_ENCDEC_ENC1_TX_DATA,		bf_audio_encdec_tx_stream_data1		},
	{	FLG_AUDIO_ENCDEC_ENC0_START,		bf_audio_encdec_start_encode0		},
	{	FLG_AUDIO_ENCDEC_ENC1_START,		bf_audio_encdec_start_encode1		},
	{	FLG_AUDIO_ENCDEC_ENC0_STOP,			bf_audio_encdec_stop_encode0		},
	{	FLG_AUDIO_ENCDEC_ENC1_STOP,			bf_audio_encdec_stop_encode1		},
	{	FLG_AUDIO_ENCDEC_DEC_INIT,			bf_audio_encdec_init_decoder		},
	{	FLG_AUDIO_ENCDEC_DEC_DEC_CB,		bf_audio_encdec_decoder_cb			},
	{	FLG_AUDIO_ENCDEC_DEC_REQ_STRM,		bf_audio_encdec_dec_req_stream		},
	{	FLG_AUDIO_ENCDEC_DEC_START,			bf_audio_encdec_start_decode		},
	{	FLG_AUDIO_ENCDEC_DEC_STOP,			bf_audio_encdec_stop_decode			},
	{	FLG_AUDIO_ENCDEC_DEC_STOP_NTFY,		bf_audio_encdec_stop_decode_notify	},
};

/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/

/**
 *	@brief		AudioEncoder Initialize Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_encdec_init_encoder0( VOID )
{
	BF_AudioEnc_Init_Encoder(D_AUDIO_ENC_CH_0);
}

/**
 *	@brief		AudioEncoder Initialize Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_encdec_init_encoder1( VOID )
{
	BF_AudioEnc_Init_Encoder(D_AUDIO_ENC_CH_1);
}

/**
 *	@brief		AudioEncoder Transfer stream data
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_encdec_tx_stream_data0( VOID )
{
//	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_START, "TxData"));
	BF_AudioEnc_TxStreamData(D_AUDIO_ENC_CH_0);
//	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_END, "TxData"));
}

/**
 *	@brief		AudioEncoder Transfer stream data
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_encdec_tx_stream_data1( VOID )
{
	BF_AudioEnc_TxStreamData(D_AUDIO_ENC_CH_1);
}

/**
 *	@brief		AudioEncoder Start Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_encdec_start_encode0( VOID )
{
//	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_START, "StartEnc"));
	BF_AudioEnc_StartEncode(D_AUDIO_ENC_CH_0);
//	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_END, "StartEnc"));
}

/**
 *	@brief		AudioEncoder Start Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_encdec_start_encode1( VOID )
{
	BF_AudioEnc_StartEncode(D_AUDIO_ENC_CH_1);
}

/**
 *	@brief		AudioEncoder Stop Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_encdec_stop_encode0( VOID )
{
	BF_AudioEnc_StopEncode(D_AUDIO_ENC_CH_0);
}
/**
 *	@brief		AudioEncoder Stop Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_encdec_stop_encode1( VOID )
{
	BF_AudioEnc_StopEncode(D_AUDIO_ENC_CH_1);
}

/**
 *	@brief		AudioDecoder Initialize Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_encdec_init_decoder( VOID )
{
	BF_AudioDec_Init_Decode();
}

/**
 *	@brief		AudioDecoder Request AudioStream data
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_encdec_decoder_cb( VOID )
{
	BF_AudioDec_Ntfy_DecoderInfo();
}

/**
 *	@brief		AudioDecoder Request AudioStream data
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_encdec_dec_req_stream( VOID )
{
	BF_AudioDec_Req_AudioStream();
}

/**
 *	@brief		AudioDecoder Start Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_encdec_start_decode( VOID )
{
	BF_AudioDec_Start_Decode();
}

/**
 *	@brief		AudioDecoder Stop Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_encdec_stop_decode( VOID )
{
	BF_AudioDec_Stop_Decode();
}

/**
 *	@brief		AudioDecoder Stop Notification Process
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_audio_encdec_stop_decode_notify( VOID )
{
	BF_AudioDec_Stop_Decode_Ntfy();
}

/**
 *	@brief		AudioEncDec Task
 *	@param		INT32
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Audio_EncDec_Process( INT32 stacd )
{
	OS_USER_FLGPTN flgptn = 0;
	ULONG wai_flg_ptn;
	UINT32 loop;
	OS_USER_ER ercd;
	
	wai_flg_ptn = FLG_AUDIO_ENCDEC_ENC0_INIT		|
				  FLG_AUDIO_ENCDEC_ENC1_INIT		|
				  FLG_AUDIO_ENCDEC_ENC0_TX_DATA		|
				  FLG_AUDIO_ENCDEC_ENC1_TX_DATA		|
				  FLG_AUDIO_ENCDEC_ENC0_START		|
				  FLG_AUDIO_ENCDEC_ENC1_START		|
				  FLG_AUDIO_ENCDEC_ENC0_STOP		|
				  FLG_AUDIO_ENCDEC_ENC1_STOP		|
				  FLG_AUDIO_ENCDEC_DEC_INIT			|
				  FLG_AUDIO_ENCDEC_DEC_DEC_CB		|
				  FLG_AUDIO_ENCDEC_DEC_REQ_STRM		|
				  FLG_AUDIO_ENCDEC_DEC_START		|
				  FLG_AUDIO_ENCDEC_DEC_STOP			|
				  FLG_AUDIO_ENCDEC_DEC_STOP_NTFY;

	while (1){
		ercd = OS_User_Wai_Flg( FID_AUDIO_ENCDEC, wai_flg_ptn, D_OS_USER_TWF_ORW, &flgptn);
		if (ercd != D_OS_USER_E_OK) {
			BF_Debug_Print_Error(("[AUDIO_ENCDEC]wai_flg error(%d) L=%d\n", ercd, __LINE__));
		}
		
		M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_START, "f=%lx", flgptn ));
		flgptn &= wai_flg_ptn;
		
		for (loop = 0; loop < D_AUDIO_ENCDEC_NUM_AUDIO_PROCESS; loop++){
			if ((flgptn & gBF_Audio_EncDec_Process[loop].event_flag) != 0){
				ercd = OS_User_Clr_Flg(FID_AUDIO_ENCDEC, ~(gBF_Audio_EncDec_Process[loop].event_flag));
				if (ercd != D_OS_USER_E_OK) {
					BF_Debug_Print_Error(("[AUDIO_ENCDEC]clr_flg error(%d) L=%d\n", ercd, __LINE__));
				}
				
				(VOID)BF_AudioEncDec_Wai_Sem();
				
				gBF_Audio_EncDec_Process[loop].process();
				
				(VOID)BF_AudioEncDec_Sig_Sem();
			}
		}
		M_MBA_PRINTF((D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_AUDIO, FJ_MBALOG_TYPE_END, "" ));
	}
}

/**
 *	@brief		AudioEncDec Task Start
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioEncDec_TaskStart( VOID )
{
	OS_USER_ER ercd_sta;
	OS_USER_ER ercd_ter;
	OS_USER_ER ercd;
	T_OS_USER_RTSK pk_rtsk;

	OS_User_Ref_Tsk(TID_AUDIO_ENCDEC, &pk_rtsk);
	if (pk_rtsk.tskstat == D_OS_USER_TTS_DMT){
		// clear flag
		ercd = OS_User_Clr_Flg(FID_AUDIO_ENCDEC, 0);
		if (ercd != D_OS_USER_E_OK) {
			BF_Debug_Print_Error(("[AUDIO_ENCDEC]clr_flg error(%d) Line=%d\n", ercd, __LINE__));
		}
		
		// Create Task process
		do {
			ercd_sta = OS_User_Sta_Tsk(TID_AUDIO_ENCDEC, 0);
			if (ercd_sta == D_OS_USER_E_OBJ){
				ercd_ter = OS_User_Ter_Tsk(TID_AUDIO_ENCDEC);
				if (ercd_ter != D_OS_USER_E_OK){
					BF_Debug_Print_Information(("[AUDIO_ENCDEC] ter_tsk Error! ercd(%d), Line=%d\n", ercd, __LINE__));
				}
			}
		}
		while (ercd_sta != 0);
	}
}

/**
 *	@brief		AudioEncDec Task Stop
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_AudioEncDec_TaskStop( VOID )
{
	OS_USER_ER ercd;

	ercd = OS_User_Ter_Tsk(TID_AUDIO_ENCDEC);
	if (ercd != D_OS_USER_E_OK){
		BF_Debug_Print_Information(("[AUDIO_ENCDEC] ter_tsk Error! ercd(%d), Line=%d\n", ercd, __LINE__));
	}
}

/**
 *  @brief		Wait on Semaphore
 *	@param		None
 *	@return		INT32
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioEncDec_Wai_Sem( VOID )
{
	OS_USER_ER ercd;
	
	//TimeOut:66ms(22*3). To work in a period for 21.3ms(48kHz)
	ercd = OS_User_Twai_Sem(SID_AUDIO_ENCDEC, 100);
	if (ercd != D_OS_USER_E_OK){
		BF_Debug_Print_Error(("[AUDIO_ENCDEC]twai_sem error(%d)\n", ercd));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Signal Semaphore
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioEncDec_Sig_Sem( VOID )
{
	OS_USER_ER ercd;
	
	ercd = OS_User_Sig_Sem(SID_AUDIO_ENCDEC);
	if (ercd != D_OS_USER_E_OK){
		BF_Debug_Print_Error(("[AUDIO_ENCDEC]sig_sem error(%d)\n", ercd));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

