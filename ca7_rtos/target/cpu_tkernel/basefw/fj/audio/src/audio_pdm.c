/**
 * @file		audio_pdm.c
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_audio.h"
#include "fj_encoder.h"
#include "fj_decoder.h"
#include "audio_pdm.h"
#include "dd_pdm.h"
#include "audio.h"
#include "dd_arm.h"
#include "dd_top.h"
#include "debug.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_AUDIO_PDM_ON		(1)
#define D_AUDIO_PDM_OFF		(0)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

static volatile UINT8 gBF_AudioPdm_Status[D_AUDIO_PDM_CH_NUM] = {D_AUDIO_PDM_STAUS_INIT, D_AUDIO_PDM_STAUS_INIT};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Local Function Prototype Definition                                  */
/*----------------------------------------------------------------------*/
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
static INT32 bf_audio_pdm_sel_pdmclk( UINT8 dev_ch, UINT32 freq_smpl );
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
/**
 *  @brief		Sett PDM Clock
 *	@param		UINT8		pdm channel
 *	@param		UINT32		sampling frequency
 *	@return		INT32		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_pdm_sel_pdmclk( UINT8 dev_ch, UINT32 freq_smpl )
{
	switch (freq_smpl){
		case D_AUDIO_SAMPLE_RATE_16000:			// FALL THROUGH
		case D_AUDIO_SAMPLE_RATE_24000:
			// set divided by 2 AUCLK
			if (dev_ch == D_AUDIO_PDM_CH0){
				Dd_Top_Set_CLKSEL7_PDM0SEL(6);
			}
			else {
				Dd_Top_Set_CLKSEL7_PDM1SEL(6);
			}
			break;
		case D_AUDIO_SAMPLE_RATE_32000:			// FALL THROUGH
		case D_AUDIO_SAMPLE_RATE_48000:
			// set by PDMCLK
			if (dev_ch == D_AUDIO_PDM_CH0){
				Dd_Top_Set_CLKSEL7_PDM0SEL(4);
			}
			else {
				Dd_Top_Set_CLKSEL7_PDM1SEL(4);
			}
			break;
		default :
			return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/

/**
 *  @brief		Initialize PDM Macoro
 *	@param		None
 *	@return		INT32		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Pdm_Init_PdmMacro( VOID )
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	T_DD_PDM_DMA_CFG dma_cfg;
	INT32 ret;
	static volatile UINT8 dd_init_flg = D_AUDIO_PDM_OFF;
	
	Dd_ARM_Critical_Section_Start( critseq );
	
	if (dd_init_flg == D_AUDIO_PDM_OFF){
		dd_init_flg = D_AUDIO_PDM_ON;
		Dd_Pdm_Init();
		
		ret = Dd_Pdm_Get_Ctrl_Dma(D_DD_PDM_CH0, &dma_cfg);
		if (ret != D_DDIM_OK){
			BF_Debug_Print_Error(("[AUDIO_PDM]Dd_Pdm_Open error(%d) L=%d\n", ret, __LINE__));
		}
		
		dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
		dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_24;
		
		ret = Dd_Pdm_Ctrl_Dma(D_DD_PDM_CH0, &dma_cfg);
		if (ret != D_DDIM_OK){
			BF_Debug_Print_Error(("[AUDIO_PDM]Dd_Pdm_Ctrl_Dma error(%d) L=%d\n", ret, __LINE__));
		}
	
		ret = Dd_Pdm_Get_Ctrl_Dma(D_DD_PDM_CH1, &dma_cfg);
		if (ret != D_DDIM_OK){
			BF_Debug_Print_Error(("[AUDIO_PDM]Dd_Pdm_Open error(%d) L=%d\n", ret, __LINE__));
		}
		
		dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
		
		ret = Dd_Pdm_Ctrl_Dma(D_DD_PDM_CH1, &dma_cfg);
		if (ret != D_DDIM_OK){
			BF_Debug_Print_Error(("[AUDIO_PDM]Dd_Pdm_Ctrl_Dma error(%d) L=%d\n", ret, __LINE__));
		}
	}
	
	Dd_ARM_Critical_Section_End( critseq );
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Open channel
 *	@param		UINT8				Channel of PDM Macro
 *	@return		INT32				Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Pdm_Open( UINT8 dev_ch )
{
	INT32 ret;
	
	ret = Dd_Pdm_Open(dev_ch, D_DDIM_USER_SEM_WAIT_POL);
	if (ret == D_DDIM_OK) {
		ret = D_AUDIO_OK;
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_PDM]Dd_Pdm_Open error(%d) L=%d\n", ret, __LINE__));
		ret = D_AUDIO_ERR;
	}
	
	return ret;
}

/**
 *  @brief		Close channel
 *	@param		UINT8				Channel of PDM Macro
 *	@return		INT32				Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Pdm_Close( UINT8 dev_ch )
{
	INT32 ret;

	ret = Dd_Pdm_Close(dev_ch);
	if (ret == D_DDIM_OK) {
		ret = D_AUDIO_OK;
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_PDM]Dd_Pdm_Close error(%d) L=%d\n", ret, __LINE__));
		ret = D_AUDIO_ERR;
	}
	
	return ret;
}


/**
 *  @brief		Set Pdm Configuration.
 *	@param		UINT8				Channel of AudioMacro
 *	@param		UINT32				Sampling Frequency
 *	@return		INT32				Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Pdm_Config( UINT8 dev_ch, UINT32 freq_smpl )
{
	T_DD_PDM_CORE_CFG core_cfg;
	INT32 ret;
	
	ret = Dd_Pdm_Get_Ctrl_Core(dev_ch, &core_cfg);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_PDM]Dd_Pdm_Get_Ctrl_Core L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	ret = bf_audio_pdm_sel_pdmclk(dev_ch, freq_smpl);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_PDM]bf_audio_pdm_sel_pdmclk err. freq=%d L=%d\n", freq_smpl, __LINE__));
		return D_AUDIO_ERR;
	}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	
	// PDM_CLK(24.576[MHz]) = Fs * 4 * SINC_RATE * MCLKDIV
	// PDM_CKO(3.072[MHz]) = Fs * 4 * SINC_RATE
	// MCLKDIV = 8
	// Fs = 768 / SINC_RATE
	
	switch (freq_smpl){
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		case D_AUDIO_SAMPLE_RATE_16000:
			core_cfg.mclk_div = 4;
			core_cfg.sinc_rate = 48;
			break;
		case D_AUDIO_SAMPLE_RATE_24000:
			core_cfg.mclk_div = 4;
			core_cfg.sinc_rate = 32;
			break;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		case D_AUDIO_SAMPLE_RATE_32000:
			core_cfg.mclk_div = 8;
			core_cfg.sinc_rate = 24;
			break;
		case D_AUDIO_SAMPLE_RATE_48000:
			core_cfg.mclk_div = 8;
			core_cfg.sinc_rate = 16;
			break;
		default :
			return D_AUDIO_ERR;
	}
	
	core_cfg.pga_l = E_DD_PDM_CORE_PGA_P_6_0_DB;
	core_cfg.pga_r = E_DD_PDM_CORE_PGA_P_6_0_DB;
	
	ret = Dd_Pdm_Ctrl_Core(dev_ch, &core_cfg);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_PDM]Dd_Pdm_Ctrl_Core L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/*
 *	@brief		Start PDM Streaming
 *	@param		UINT8		Channel of PdmMacro
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Pdm_Start( UINT8 dev_ch )
{
	INT32 ret;
	
	ret = Dd_Pdm_Start_Streaming(dev_ch);
	if (ret == D_DDIM_OK){
		ret = D_AUDIO_OK;
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_PDM]Dd_Pdm_Start_Streaming Error L=%d\n", __LINE__));
		ret = D_AUDIO_ERR;
	}
	
	gBF_AudioPdm_Status[dev_ch] = D_AUDIO_PDM_STAUS_START;
	
	return ret;
}

/*
 *	@brief		Stop Input
 *	@param		UINT8		Channel of AudioMacro
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Pdm_Stop( UINT8 dev_ch )
{
	INT32 ret;
	
	ret = Dd_Pdm_Stop_Streaming(dev_ch);
	if (ret == D_DDIM_OK){
		ret = D_AUDIO_OK;
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_PDM]Dd_Pdm_Stop_Streaming Error L=%d\n", __LINE__));
		ret = D_AUDIO_ERR;
	}
	
	gBF_AudioPdm_Status[dev_ch] = D_AUDIO_PDM_STAUS_STOP;
	
	return ret;
}

/*
 *	@brief		Get Status
 *	@param		UINT8		Channel of AudioMacro
 *	@return		None
 *	@note		None
 *	@attention	None
 */
UINT8 BF_Audio_Pdm_GetStatus( UINT8 dev_ch )
{
	return gBF_AudioPdm_Status[dev_ch];
}
