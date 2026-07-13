/**
 * @file		audio_dma.c
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_audio.h"
#include "fj_encoder.h"
#include "fj_decoder.h"
#include "audio.h"
#include "audio_dma.h"
#include "audio_auif.h"
#include "audio_data.h"
#include "dd_arm.h"
#include "dd_hdmac0.h"
#include "debug.h"

/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/
#define D_AUDIO_DMA_INVALID			(0xFFU)

/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static VOID (*gAudio_DMA_Int_Callback0)( UCHAR dma_status );
static VOID (*gAudio_DMA_Int_Callback1)( UCHAR dma_status );
static VOID (*gAudio_DMA_Int_Callback2)( UCHAR dma_status );
static VOID (*gAudio_DMA_Int_Callback3)( UCHAR dma_status );
static VOID (*gAudio_DMA_Int_Callback4)( UCHAR dma_status );
static VOID (*gAudio_DMA_Int_Callback5)( UCHAR dma_status );
static VOID (*gAudio_DMA_Int_Callback6)( UCHAR dma_status );
static VOID (*gAudio_DMA_Int_Callback7)( UCHAR dma_status );

static volatile UINT32 gBF_Audio_Dma_RemainTrnsSize[D_AUDIO_DMA_CH_NUM] = {0};
static volatile UINT32 gBF_Audio_Dma_TrnsSize[D_AUDIO_DMA_CH_NUM] = {0};
static volatile UINT8 gBF_Audio_Dma_LastInitCh[D_AUDIO_DMA_ID_NUM] = {D_AUDIO_DMA_INVALID, D_AUDIO_DMA_INVALID, D_AUDIO_DMA_INVALID, D_AUDIO_DMA_INVALID};
static volatile UINT8 gBF_Audio_Dma_Status[D_AUDIO_DMA_ID_NUM] = {FJ_AUDIO_DISABLE, FJ_AUDIO_DISABLE, FJ_AUDIO_DISABLE, FJ_AUDIO_DISABLE};



/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function                                                             */
/*----------------------------------------------------------------------*/
static VOID bf_audio_dma_int_handler0(CHAR ch, USHORT * status);
static VOID bf_audio_dma_int_handler1(CHAR ch, USHORT * status);
static VOID bf_audio_dma_int_handler2(CHAR ch, USHORT * status);
static VOID bf_audio_dma_int_handler3(CHAR ch, USHORT * status);
static VOID bf_audio_dma_int_handler4(CHAR ch, USHORT * status);
static VOID bf_audio_dma_int_handler5(CHAR ch, USHORT * status);
static VOID bf_audio_dma_int_handler6(CHAR ch, USHORT * status);
static VOID bf_audio_dma_int_handler7(CHAR ch, USHORT * status);
static INT32 bf_audio_dma0_start_select_callback( UINT8 dma_id, UINT8* dma_ch, VP_CALLBACK* callback );
static INT32 bf_audio_dma1_start_select_callback( UINT8 dma_id, UINT8* dma_ch, VP_CALLBACK* callback );

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/

/**
 * DMA Interrupt Hander0
 * @param   CHAR ch
 * @param   USHORT* status
 * @return  VOID
 */
static VOID bf_audio_dma_int_handler0(CHAR ch, USHORT* status)
{
	VOID (*callback)(UCHAR dma_status);
	
	gBF_Audio_Dma_LastInitCh[D_AUDIO_DMA_ID_0] = D_AUDIO_DMA_CH_0;
	
	callback = (VOID*)gAudio_DMA_Int_Callback0;
	
	if (callback != NULL) {
		callback(*status);
	}
}

/**
 * DMA Interrupt Hander1
 * @param   CHAR ch
 * @param   USHORT* status
 * @return  VOID
 */
static VOID bf_audio_dma_int_handler1(CHAR ch, USHORT* status)
{
	VOID (*callback)(UCHAR dma_status);
	
	gBF_Audio_Dma_LastInitCh[D_AUDIO_DMA_ID_0] = D_AUDIO_DMA_CH_1;
	
	callback = (VOID*)gAudio_DMA_Int_Callback1;
	
	if (callback != NULL) {
		callback(*status);
	}
}

/**
 * DMA Interrupt Hander2
 * @param   CHAR ch
 * @param   USHORT* status
 * @return  VOID
 */
static VOID bf_audio_dma_int_handler2(CHAR ch, USHORT* status)
{
	VOID (*callback)(UCHAR dma_status);
	
	gBF_Audio_Dma_LastInitCh[D_AUDIO_DMA_ID_1] = D_AUDIO_DMA_CH_2;
	
	callback = (VOID*)gAudio_DMA_Int_Callback2;
	
	if (callback != NULL) {
		callback(*status);
	}
}

/**
 * DMA Interrupt Hander3
 * @param   CHAR ch
 * @param   USHORT* status
 * @return  VOID
 */
static VOID bf_audio_dma_int_handler3(CHAR ch, USHORT* status)
{
	VOID (*callback)(UCHAR dma_status);
	
	gBF_Audio_Dma_LastInitCh[D_AUDIO_DMA_ID_1] = D_AUDIO_DMA_CH_3;
	
	callback = (VOID*)gAudio_DMA_Int_Callback3;
	
	if (callback != NULL) {
		callback(*status);
	}
}

/**
 * DMA Interrupt Hander4
 * @param   CHAR ch
 * @param   USHORT* status
 * @return  VOID
 */
static VOID bf_audio_dma_int_handler4(CHAR ch, USHORT* status)
{
	VOID (*callback)(UCHAR dma_status);

	gBF_Audio_Dma_LastInitCh[D_AUDIO_DMA_ID_2] = D_AUDIO_DMA_CH_4;
	
	callback = (VOID*)gAudio_DMA_Int_Callback4;
	
	if (callback != NULL) {
		callback(*status);
	}
}

/**
 * DMA Interrupt Hander5
 * @param   CHAR ch
 * @param   USHORT* status
 * @return  VOID
 */
static VOID bf_audio_dma_int_handler5(CHAR ch, USHORT* status)
{
	VOID (*callback)(UCHAR dma_status);
	
	gBF_Audio_Dma_LastInitCh[D_AUDIO_DMA_ID_2] = D_AUDIO_DMA_CH_5;
	
	callback = (VOID*)gAudio_DMA_Int_Callback5;
	
	if (callback != NULL) {
		callback(*status);
	}
}

/**
 * DMA Interrupt Hander6
 * @param   CHAR ch
 * @param   USHORT* status
 * @return  VOID
 */
static VOID bf_audio_dma_int_handler6(CHAR ch, USHORT* status)
{
	VOID (*callback)(UCHAR dma_status);
	
	gBF_Audio_Dma_LastInitCh[D_AUDIO_DMA_ID_3] = D_AUDIO_DMA_CH_6;
	
	callback = (VOID*)gAudio_DMA_Int_Callback6;
	
	if (callback != NULL) {
		callback(*status);
	}
}

/**
 * DMA Interrupt Hander7
 * @param   CHAR ch
 * @param   USHORT* status
 * @return  VOID
 */
static VOID bf_audio_dma_int_handler7(CHAR ch, USHORT* status)
{
	VOID (*callback)(UCHAR dma_status);
	
	gBF_Audio_Dma_LastInitCh[D_AUDIO_DMA_ID_3] = D_AUDIO_DMA_CH_7;
	
	callback = (VOID*)gAudio_DMA_Int_Callback7;
	
	if (callback != NULL) {
		callback(*status);
	}
}

/**
 * Select callback function(dma0)
 * @param   UINT8			dma_id
 * @param   UINT8*			dma_ch
 * @param   VP_CALLBACK*	callback
 * @return  None
 */
static INT32 bf_audio_dma0_start_select_callback( UINT8 dma_id, UINT8* dma_ch, VP_CALLBACK* callback )
{
	switch (dma_id){
		case D_AUDIO_DMA_ID_0:
			*dma_ch = D_AUDIO_DMA_CH_0;
			*callback = (VP_CALLBACK)bf_audio_dma_int_handler0;
			break;
		case D_AUDIO_DMA_ID_1:
			*dma_ch = D_AUDIO_DMA_CH_2;
			*callback = (VP_CALLBACK)bf_audio_dma_int_handler2;
			break;
		case D_AUDIO_DMA_ID_2:
			*dma_ch = D_AUDIO_DMA_CH_4;
			*callback = (VP_CALLBACK)bf_audio_dma_int_handler4;
			break;
		case D_AUDIO_DMA_ID_3:
			*dma_ch = D_AUDIO_DMA_CH_6;
			*callback = (VP_CALLBACK)bf_audio_dma_int_handler6;
			break;
		default:
			*dma_ch = D_AUDIO_DMA_CH_0;
			*callback = (VP_CALLBACK)NULL;
			return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 * Select callback function(dma1)
 * @param   UINT8			dma_id
 * @param   UINT8*			dma_ch
 * @param   VP_CALLBACK*	callback
 * @return  None
 */
static INT32 bf_audio_dma1_start_select_callback( UINT8 dma_id, UINT8* dma_ch, VP_CALLBACK* callback )
{
	switch (dma_id){
		case D_AUDIO_DMA_ID_0:
			*dma_ch = D_AUDIO_DMA_CH_1;
			*callback = (VP_CALLBACK)bf_audio_dma_int_handler1;
			break;
		case D_AUDIO_DMA_ID_1:
			*dma_ch = D_AUDIO_DMA_CH_3;
			*callback = (VP_CALLBACK)bf_audio_dma_int_handler3;
			break;
		case D_AUDIO_DMA_ID_2:
			*dma_ch = D_AUDIO_DMA_CH_5;
			*callback = (VP_CALLBACK)bf_audio_dma_int_handler5;
			break;
		case D_AUDIO_DMA_ID_3:
			*dma_ch = D_AUDIO_DMA_CH_7;
			*callback = (VP_CALLBACK)bf_audio_dma_int_handler7;
			break;
		default:
			*dma_ch = D_AUDIO_DMA_CH_1;
			*callback = (VP_CALLBACK)NULL;
			return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/

/**
 * DMA Initialize setting
 * @param   UINT8*	dma_id
 * @return  INT32	OK/ERR
 */
INT32 BF_Audio_DMA_Init( UINT8* dma_id )
{
	UINT32 loop;
	INT32 ret;
	INT32 ret_val = D_AUDIO_OK;
//	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	
//	Dd_ARM_Critical_Section_Start( critseq );
	
	if (dma_id == NULL){
		BF_Debug_Print_Error(("[AUDIO_DMA]BF_Audio_DMA_Init() Param Error. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	*dma_id = D_AUDIO_DMA_INVALID;
	
	for (loop = 0; loop < D_AUDIO_DMA_ID_NUM; loop++){
		if (gBF_Audio_Dma_Status[loop] == FJ_AUDIO_DISABLE){
			gBF_Audio_Dma_Status[loop] = FJ_AUDIO_ENABLE;
			*dma_id = loop;
			break;
		}
	}
	
	if (*dma_id == D_AUDIO_DMA_INVALID){
		BF_Debug_Print_Error(("[AUDIO_DMA]Error. All DMA used. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	switch (*dma_id){
		case D_AUDIO_DMA_ID_0:
			ret = Dd_HDMAC0_Open(D_AUDIO_DMA_CH_0, D_DDIM_USER_SEM_WAIT_POL);
			if (ret != D_DDIM_OK) {
				BF_Debug_Print_Error(("[AUDIO_DMA]Dd_HDMAC0_Open error(%d) L=%d\n", ret, __LINE__));
				ret_val = D_AUDIO_ERR;
			}
			else {
				ret = Dd_HDMAC0_Open(D_AUDIO_DMA_CH_1, D_DDIM_USER_SEM_WAIT_POL);
				if (ret != D_DDIM_OK) {
					BF_Debug_Print_Error(("[AUDIO_DMA]Dd_HDMAC0_Open error(%d) L=%d\n", ret, __LINE__));
					ret_val = D_AUDIO_ERR;
				}
			}
			break;
		case D_AUDIO_DMA_ID_1:
			ret = Dd_HDMAC0_Open(D_AUDIO_DMA_CH_2, D_DDIM_USER_SEM_WAIT_POL);
			if (ret != D_DDIM_OK) {
				BF_Debug_Print_Error(("[AUDIO_DMA]Dd_HDMAC0_Open error(%d) L=%d\n", ret, __LINE__));
				ret_val = D_AUDIO_ERR;
			}
			else {
				ret = Dd_HDMAC0_Open(D_AUDIO_DMA_CH_3, D_DDIM_USER_SEM_WAIT_POL);
				if (ret != D_DDIM_OK) {
					BF_Debug_Print_Error(("[AUDIO_DMA]Dd_HDMAC0_Open error(%d) L=%d\n", ret, __LINE__));
					ret_val = D_AUDIO_ERR;
				}
			}
			break;
		case D_AUDIO_DMA_ID_2:
			ret = Dd_HDMAC0_Open(D_AUDIO_DMA_CH_4, D_DDIM_USER_SEM_WAIT_POL);
			if (ret != D_DDIM_OK) {
				BF_Debug_Print_Error(("[AUDIO_DMA]Dd_HDMAC0_Open error(%d) L=%d\n", ret, __LINE__));
				ret_val = D_AUDIO_ERR;
			}
			else {
				ret = Dd_HDMAC0_Open(D_AUDIO_DMA_CH_5, D_DDIM_USER_SEM_WAIT_POL);
				if (ret != D_DDIM_OK) {
					BF_Debug_Print_Error(("[AUDIO_DMA]Dd_HDMAC0_Open error(%d) L=%d\n", ret, __LINE__));
					ret_val = D_AUDIO_ERR;
				}
			}
			break;
		case D_AUDIO_DMA_ID_3:
			ret = Dd_HDMAC0_Open(D_AUDIO_DMA_CH_6, D_DDIM_USER_SEM_WAIT_POL);
			if (ret != D_DDIM_OK) {
				BF_Debug_Print_Error(("[AUDIO_DMA]Dd_HDMAC0_Open error(%d) L=%d\n", ret, __LINE__));
				ret_val = D_AUDIO_ERR;
			}
			else {
				ret = Dd_HDMAC0_Open(D_AUDIO_DMA_CH_7, D_DDIM_USER_SEM_WAIT_POL);
				if (ret != D_DDIM_OK) {
					BF_Debug_Print_Error(("[AUDIO_DMA]Dd_HDMAC0_Open error(%d) L=%d\n", ret, __LINE__));
					ret_val = D_AUDIO_ERR;
				}
			}
			break;
		default:
			BF_Debug_Print_Error(("[AUDIO_DMA]BF_Audio_DMA_Init() error. invalid dma_id L=%d\n", __LINE__));
			ret_val = D_AUDIO_ERR;
	}
	
//	Dd_ARM_Critical_Section_End( critseq );
	
	return ret_val;
}

/**
 * Set DMA Interrupt callbakc function
 * @param   UINT8*			dma_id
 * @param   *callback_fnc	pointer of callback function
 * @return  None
 */
VOID BF_Audio_DMA_Set_Callback_Fnc0( UINT8 dma_id, VOID (*callback_fnc)(UCHAR dma_status) )
{
	switch (dma_id){
		case D_AUDIO_DMA_ID_0:
			gAudio_DMA_Int_Callback0 = callback_fnc;
			break;
		case D_AUDIO_DMA_ID_1:
			gAudio_DMA_Int_Callback2 = callback_fnc;
			break;
		case D_AUDIO_DMA_ID_2:
			gAudio_DMA_Int_Callback4 = callback_fnc;
			break;
		case D_AUDIO_DMA_ID_3:
			gAudio_DMA_Int_Callback6 = callback_fnc;
			break;
		default:
			BF_Debug_Print_Error(("[AUDIO_DMA]BF_Audio_DMA_Set_Callback_Fnc0() error. invalid dma_id L=%d\n", __LINE__));
			break;
	}
}

/**
 * Set DMA Interrupt callbakc function
 * @param   UINT8*			dma_id
 * @param   *callback_fnc	pointer of callback function
 * @return  None
 */
VOID BF_Audio_DMA_Set_Callback_Fnc1( UINT8 dma_id, VOID (*callback_fnc)(UCHAR dma_status) )
{
	switch (dma_id){
		case D_AUDIO_DMA_ID_0:
			gAudio_DMA_Int_Callback1 = callback_fnc;
			break;
		case D_AUDIO_DMA_ID_1:
			gAudio_DMA_Int_Callback3 = callback_fnc;
			break;
		case D_AUDIO_DMA_ID_2:
			gAudio_DMA_Int_Callback5 = callback_fnc;
			break;
		case D_AUDIO_DMA_ID_3:
			gAudio_DMA_Int_Callback7 = callback_fnc;
			break;
		default:
			BF_Debug_Print_Error(("[AUDIO_DMA]BF_Audio_DMA_Set_Callback_Fnc1() error. invalid dma_id L=%d\n", __LINE__));
			break;
	}
}

/**
 * Start dma Input process
 * @param   T_AUDIO_DMA_START_PARAM*	dma_param
 * @return  None
 */
VOID BF_Audio_DMA_Input_Start0( T_AUDIO_DMA_START_PARAM* dma_param )
{
	UINT8 dma_ch;
	UINT8 in_sel;
	VP_CALLBACK callback;
	INT32 ret;
	
	ret = bf_audio_dma0_start_select_callback(dma_param->dma_id, &dma_ch, &callback);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_DMA]bf_audio_dma0_start_select_callback() error. invalid dma_id L=%d\n", __LINE__));
		return;
	}
	
	switch (dma_param->audio_dev_ch){
		case D_AUDIO_AUIF_CH0:
			in_sel = D_DD_HDMAC0_IS_IDREQ_0;
			break;
		case D_AUDIO_AUIF_CH1:
			in_sel = D_DD_HDMAC0_IS_IDREQ_2;
			break;
		case D_AUDIO_AUIF_CH2:
			in_sel = D_DD_HDMAC0_IS_IDREQ_4;
			break;
		case D_AUDIO_AUIF_CH4:
			in_sel = D_DD_HDMAC0_IS_IDREQ_8;
			break;
		case D_AUDIO_AUIF_CH5:
			in_sel = D_DD_HDMAC0_IS_IDREQ_10;
			break;
		default:
			BF_Debug_Print_Error(("[AUDIO_DMA]BF_Audio_DMA_Input_Start0() error. invalid dev_ch L=%d\n", __LINE__));
			return;
	}
	
//	BF_Audio_Data_Flush_Dcache((VOID*)dma_param->dst_address, dma_param->trans_size);
	
	Dd_ARM_Dsb_Pou();
	
	Dd_HDMAC0_Input_Audio_Async(dma_ch,
								in_sel,
								dma_param->src_address,
								dma_param->dst_address,
								dma_param->trans_size,
								callback);
}

/**
 * Start dma Input process
 * @param   T_AUDIO_DMA_START_PARAM*	dma_id
 * @return  None
 */
VOID BF_Audio_DMA_Input_Start1( T_AUDIO_DMA_START_PARAM* dma_param )
{
	UINT8 dma_ch;
	UINT8 in_sel;
	VP_CALLBACK callback;
	INT32 ret;
	
	ret = bf_audio_dma1_start_select_callback(dma_param->dma_id, &dma_ch, &callback);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_DMA]bf_audio_dma1_start_select_callback() error. invalid dma_id L=%d\n", __LINE__));
		return;
	}
	
	switch (dma_param->audio_dev_ch){
		case D_AUDIO_AUIF_CH0:
			in_sel = D_DD_HDMAC0_IS_IDREQ_12;
			break;
		case D_AUDIO_AUIF_CH1:
			in_sel = D_DD_HDMAC0_IS_IDREQ_14;
			break;
		case D_AUDIO_AUIF_CH2:
			in_sel = D_DD_HDMAC0_IS_IDREQ_16;
			break;
		case D_AUDIO_AUIF_CH4:
			in_sel = D_DD_HDMAC0_IS_IDREQ_20;
			break;
		case D_AUDIO_AUIF_CH5:
			in_sel = D_DD_HDMAC0_IS_IDREQ_22;
			break;
		default:
			BF_Debug_Print_Error(("[AUDIO_DMA]BF_Audio_DMA_Input_Start1() error. invalid dev_ch L=%d\n", __LINE__));
			return;
	}
	
//	BF_Audio_Data_Flush_Dcache((VOID*)dma_param->dst_address, dma_param->trans_size);
	
	Dd_ARM_Dsb_Pou();
	
	Dd_HDMAC0_Input_Audio_Async(dma_ch,
								in_sel,
								dma_param->src_address,
								dma_param->dst_address,
								dma_param->trans_size,
								callback);
}

/**
 * Start dma output process
 * @param   T_AUDIO_DMA_START_PARAM*	dma_id
 * @return  None
 */
VOID BF_Audio_DMA_Output_Start0( T_AUDIO_DMA_START_PARAM* dma_param )
{
	UINT8 dma_ch;
	UINT8 in_sel;
	VP_CALLBACK callback;
	INT32 ret;
	
	ret = bf_audio_dma0_start_select_callback(dma_param->dma_id, &dma_ch, &callback);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_DMA]bf_audio_dma0_start_select_callback() error. invalid dma_id L=%d\n", __LINE__));
		return;
	}
	
	switch (dma_param->audio_dev_ch){
		case D_AUDIO_AUIF_CH0:
			in_sel = D_DD_HDMAC0_IS_IDREQ_1;
			break;
		case D_AUDIO_AUIF_CH1:
			in_sel = D_DD_HDMAC0_IS_IDREQ_3;
			break;
		case D_AUDIO_AUIF_CH2:
			in_sel = D_DD_HDMAC0_IS_IDREQ_5;
			break;
		case D_AUDIO_AUIF_CH3:
			in_sel = D_DD_HDMAC0_IS_IDREQ_7;
			break;
		default:
			BF_Debug_Print_Error(("[AUDIO_DMA]BF_Audio_DMA_Output_Start0() error. invalid dev_ch L=%d\n", __LINE__));
			return;
	}
	
	Dd_HDMAC0_Output_Audio_Async(dma_ch,
								in_sel,
								dma_param->src_address,
								dma_param->dst_address,
								dma_param->trans_size,
								callback);
}

/**
 * Start dma output process
 * @param   T_AUDIO_DMA_START_PARAM*	dma_id
 * @return  None
 */
VOID BF_Audio_DMA_Output_Start1( T_AUDIO_DMA_START_PARAM* dma_param )
{
	UINT8 dma_ch;
	UINT8 in_sel;
	VP_CALLBACK callback;
	INT32 ret;
	
	ret = bf_audio_dma1_start_select_callback(dma_param->dma_id, &dma_ch, &callback);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_DMA]bf_audio_dma1_start_select_callback() error. invalid dma_id L=%d\n", __LINE__));
		return;
	}
	
	switch (dma_param->audio_dev_ch){
		case D_AUDIO_AUIF_CH0:
			in_sel = D_DD_HDMAC0_IS_IDREQ_13;
			break;
		case D_AUDIO_AUIF_CH1:
			in_sel = D_DD_HDMAC0_IS_IDREQ_15;
			break;
		case D_AUDIO_AUIF_CH2:
			in_sel = D_DD_HDMAC0_IS_IDREQ_17;
			break;
		case D_AUDIO_AUIF_CH3:
			in_sel = D_DD_HDMAC0_IS_IDREQ_19;
			break;
		default:
			BF_Debug_Print_Error(("[AUDIO_DMA]BF_Audio_DMA_Output_Start0() error. invalid dev_ch L=%d\n", __LINE__));
			return;
	}
	
	Dd_HDMAC0_Output_Audio_Async(dma_ch,
								in_sel,
								dma_param->src_address,
								dma_param->dst_address,
								dma_param->trans_size,
								callback);
}

/**
 * Stop dma process
 * @param   T_AUDIO_DMA_START_PARAM*	dma_id
 * @return  None
 */
INT32 BF_Audio_DMA_Stop( UINT8 dma_id )
{
	INT32 ret;
	
	switch (dma_id){
		case D_AUDIO_DMA_ID_0:
			ret = Dd_HDMAC0_Stop(D_AUDIO_DMA_CH_0);
			if (ret != D_DDIM_OK){
				BF_Debug_Print_Error(("[AUDIO_DMA]Dd_HDMAC0_Stop Error L=%d\n", __LINE__));
				return D_AUDIO_ERR;
			}
			ret = Dd_HDMAC0_Stop(D_AUDIO_DMA_CH_1);
			if (ret != D_DDIM_OK){
				BF_Debug_Print_Error(("[AUDIO_DMA]Dd_HDMAC0_Stop Error L=%d\n", __LINE__));
				return D_AUDIO_ERR;
			}
			
			gBF_Audio_Dma_RemainTrnsSize[D_AUDIO_DMA_CH_0] = Dd_HDMAC0_Get_Remain_Trns_Size(D_AUDIO_DMA_CH_0);
			gBF_Audio_Dma_TrnsSize[D_AUDIO_DMA_CH_0] = Dd_HDMAC0_Get_Trns_Size(D_AUDIO_DMA_CH_0);
			gBF_Audio_Dma_RemainTrnsSize[D_AUDIO_DMA_CH_1] = Dd_HDMAC0_Get_Remain_Trns_Size(D_AUDIO_DMA_CH_1);
			gBF_Audio_Dma_TrnsSize[D_AUDIO_DMA_CH_1] = Dd_HDMAC0_Get_Trns_Size(D_AUDIO_DMA_CH_1);
			
			Dd_HDMAC0_Close(D_AUDIO_DMA_CH_0);
			Dd_HDMAC0_Close(D_AUDIO_DMA_CH_1);
			break;
		case D_AUDIO_DMA_ID_1:
			ret = Dd_HDMAC0_Stop(D_AUDIO_DMA_CH_2);
			if (ret != D_DDIM_OK){
				BF_Debug_Print_Error(("[AUDIO_DMA]Dd_HDMAC0_Stop Error L=%d\n", __LINE__));
				return D_AUDIO_ERR;
			}
			ret = Dd_HDMAC0_Stop(D_AUDIO_DMA_CH_3);
			if (ret != D_DDIM_OK){
				BF_Debug_Print_Error(("[AUDIO_DMA]Dd_HDMAC0_Stop Error L=%d\n", __LINE__));
				return D_AUDIO_ERR;
			}
			
			gBF_Audio_Dma_RemainTrnsSize[D_AUDIO_DMA_CH_2] = Dd_HDMAC0_Get_Remain_Trns_Size(D_AUDIO_DMA_CH_2);
			gBF_Audio_Dma_TrnsSize[D_AUDIO_DMA_CH_2] = Dd_HDMAC0_Get_Trns_Size(D_AUDIO_DMA_CH_2);
			gBF_Audio_Dma_RemainTrnsSize[D_AUDIO_DMA_CH_3] = Dd_HDMAC0_Get_Remain_Trns_Size(D_AUDIO_DMA_CH_3);
			gBF_Audio_Dma_TrnsSize[D_AUDIO_DMA_CH_3] = Dd_HDMAC0_Get_Trns_Size(D_AUDIO_DMA_CH_3);
			
			Dd_HDMAC0_Close(D_AUDIO_DMA_CH_2);
			Dd_HDMAC0_Close(D_AUDIO_DMA_CH_3);
			break;
		case D_AUDIO_DMA_ID_2:
			ret = Dd_HDMAC0_Stop(D_AUDIO_DMA_CH_4);
			if (ret != D_DDIM_OK){
				BF_Debug_Print_Error(("[AUDIO_DMA]Dd_HDMAC0_Stop Error L=%d\n", __LINE__));
				return D_AUDIO_ERR;
			}
			ret = Dd_HDMAC0_Stop(D_AUDIO_DMA_CH_5);
			if (ret != D_DDIM_OK){
				BF_Debug_Print_Error(("[AUDIO_DMA]Dd_HDMAC0_Stop Error L=%d\n", __LINE__));
				return D_AUDIO_ERR;
			}
			
			gBF_Audio_Dma_RemainTrnsSize[D_AUDIO_DMA_CH_4] = Dd_HDMAC0_Get_Remain_Trns_Size(D_AUDIO_DMA_CH_4);
			gBF_Audio_Dma_TrnsSize[D_AUDIO_DMA_CH_4] = Dd_HDMAC0_Get_Trns_Size(D_AUDIO_DMA_CH_4);
			gBF_Audio_Dma_RemainTrnsSize[D_AUDIO_DMA_CH_5] = Dd_HDMAC0_Get_Remain_Trns_Size(D_AUDIO_DMA_CH_5);
			gBF_Audio_Dma_TrnsSize[D_AUDIO_DMA_CH_5] = Dd_HDMAC0_Get_Trns_Size(D_AUDIO_DMA_CH_5);
			
			Dd_HDMAC0_Close(D_AUDIO_DMA_CH_4);
			Dd_HDMAC0_Close(D_AUDIO_DMA_CH_5);
			break;
		case D_AUDIO_DMA_ID_3:
			ret = Dd_HDMAC0_Stop(D_AUDIO_DMA_CH_6);
			if (ret != D_DDIM_OK){
				BF_Debug_Print_Error(("[AUDIO_DMA]Dd_HDMAC0_Stop Error L=%d\n", __LINE__));
				return D_AUDIO_ERR;
			}
			ret = Dd_HDMAC0_Stop(D_AUDIO_DMA_CH_7);
			if (ret != D_DDIM_OK){
				BF_Debug_Print_Error(("[AUDIO_DMA]Dd_HDMAC0_Stop Error L=%d\n", __LINE__));
				return D_AUDIO_ERR;
			}
			
			gBF_Audio_Dma_RemainTrnsSize[D_AUDIO_DMA_CH_6] = Dd_HDMAC0_Get_Remain_Trns_Size(D_AUDIO_DMA_CH_6);
			gBF_Audio_Dma_TrnsSize[D_AUDIO_DMA_CH_6] = Dd_HDMAC0_Get_Trns_Size(D_AUDIO_DMA_CH_6);
			gBF_Audio_Dma_RemainTrnsSize[D_AUDIO_DMA_CH_7] = Dd_HDMAC0_Get_Remain_Trns_Size(D_AUDIO_DMA_CH_7);
			gBF_Audio_Dma_TrnsSize[D_AUDIO_DMA_CH_7] = Dd_HDMAC0_Get_Trns_Size(D_AUDIO_DMA_CH_7);
			
			Dd_HDMAC0_Close(D_AUDIO_DMA_CH_6);
			Dd_HDMAC0_Close(D_AUDIO_DMA_CH_7);
			break;
		default:
			BF_Debug_Print_Error(("[AUDIO_DMA]BF_Audio_DMA_Init() error. invalid dma_id L=%d\n", __LINE__));
			return D_AUDIO_ERR;
	}
	
	gBF_Audio_Dma_Status[dma_id] = FJ_AUDIO_DISABLE;
	
	return D_AUDIO_OK;
}

/**
 * Get remain transfer size
 * @param   UINT8	dma_id
 * @return  UINT32	size
 */
UINT32 BF_Audio_DMA_Get_Remain_Trns_Size( UINT8 dma_id )
{
	return gBF_Audio_Dma_TrnsSize[gBF_Audio_Dma_LastInitCh[dma_id]];
}

/**
 * Get transfer size
 * @param   UINT8	dma_id
 * @return  UINT32	size
 */
UINT32 BF_Audio_DMA_Get_Trns_Size( UINT8 dma_id )
{
	return gBF_Audio_Dma_RemainTrnsSize[gBF_Audio_Dma_LastInitCh[dma_id]];
}

/**
 * Get Number of Remain DMA ID
 * @param   None
 * @return  UINT32	Number of Remain DMA ID
 */
UINT32 BF_Audio_DMA_Get_RemainNumDmaID( VOID )
{
	UINT32 loop;
	UINT32 count;
	
	count = 0;
	
	for (loop = 0; loop < D_AUDIO_DMA_ID_NUM; loop++){
		if (gBF_Audio_Dma_Status[loop] == FJ_AUDIO_DISABLE){
			count++;
		}
	}
	
	return count;
}

