/**
 * @file		audio_auif.c
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_audio.h"
#include "fj_encoder.h"
#include "fj_decoder.h"
#include "audio_auif.h"
#include "dd_audio.h"
#include "audio.h"
#include "dd_arm.h"
#include "debug.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_AUDIO_AUIF_ON					(1)
#define D_AUDIO_AUIF_OFF				(0)

#define D_AUDIO_AUIF_NUM_LR_CLOCK		(10U)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	UINT32	lr_clock;
	INT32	(*process)(T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle);
} T_AUDIO_AUIF_LR_CLOCK;


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static volatile UINT8 gBF_Auif_InGain_Tbl[D_AUDIO_AUIF_CH_NUM] = {
	D_AUDIO_AUIF_GAIN_0DB,
	D_AUDIO_AUIF_GAIN_0DB,
	D_AUDIO_AUIF_GAIN_0DB,
	D_AUDIO_AUIF_GAIN_0DB,
	D_AUDIO_AUIF_GAIN_0DB,
	D_AUDIO_AUIF_GAIN_0DB
};

static volatile UINT8 gBF_Auif_OutGain_Tbl[(D_AUDIO_AUIF_CH_NUM - 2)] = {
	D_AUDIO_AUIF_GAIN_0DB,
	D_AUDIO_AUIF_GAIN_0DB,
	D_AUDIO_AUIF_GAIN_0DB,
	D_AUDIO_AUIF_GAIN_0DB,
};


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Local Function Prototype Definition									*/
/*----------------------------------------------------------------------*/
static INT32 bf_audio_auif_set_lrclock_8khz(T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle);
static INT32 bf_audio_auif_set_lrclock_11khz(T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle);
static INT32 bf_audio_auif_set_lrclock_12khz(T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle);
static INT32 bf_audio_auif_set_lrclock_16khz(T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle);
static INT32 bf_audio_auif_set_lrclock_22khz(T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle);
static INT32 bf_audio_auif_set_lrclock_24khz(T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle);
static INT32 bf_audio_auif_set_lrclock_32khz(T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle);
static INT32 bf_audio_auif_set_lrclock_44khz(T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle);
static INT32 bf_audio_auif_set_lrclock_48khz(T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle);
static INT32 bf_audio_auif_set_lrclock_96khz(T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle);
static INT32 bf_audio_auif_set_clock( UINT8 dev_ch, const T_FJ_AUDIO_I2S_CFG* i2s_cfg );

/*----------------------------------------------------------------------*/
/* Function Table Definition											*/
/*----------------------------------------------------------------------*/
static const T_AUDIO_AUIF_LR_CLOCK gBF_Audio_Auif_LRClock_Tbl[D_AUDIO_AUIF_NUM_LR_CLOCK] = {
	{	D_AUDIO_SAMPLE_RATE_8000,		bf_audio_auif_set_lrclock_8khz		},
	{	D_AUDIO_SAMPLE_RATE_11025,		bf_audio_auif_set_lrclock_11khz		},
	{	D_AUDIO_SAMPLE_RATE_12000,		bf_audio_auif_set_lrclock_12khz		},
	{	D_AUDIO_SAMPLE_RATE_16000,		bf_audio_auif_set_lrclock_16khz		},
	{	D_AUDIO_SAMPLE_RATE_22050,		bf_audio_auif_set_lrclock_22khz		},
	{	D_AUDIO_SAMPLE_RATE_24000,		bf_audio_auif_set_lrclock_24khz		},
	{	D_AUDIO_SAMPLE_RATE_32000,		bf_audio_auif_set_lrclock_32khz		},
	{	D_AUDIO_SAMPLE_RATE_44100,		bf_audio_auif_set_lrclock_44khz		},
	{	D_AUDIO_SAMPLE_RATE_48000,		bf_audio_auif_set_lrclock_48khz		},
	{	D_AUDIO_SAMPLE_RATE_96000,		bf_audio_auif_set_lrclock_96khz		},
};


/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/

/**
 *  @brief		Set LR Clock(8kHz)
 *	@param		None
 *	@return		INT32		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_auif_set_lrclock_8khz( T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle )
{
	if (clock == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_auif_set_lrclock_8khz() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	clock->aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	clock->div_aumclko = E_DD_AUDIO_AUMCLKO_DIV_12;		// 2048kHz
	
	if (data_cycle == D_AUDIO_I2S_CYCLE_32FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_8;		// 256kHz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_32;		// 8kHz
	}
	else if (data_cycle == D_AUDIO_I2S_CYCLE_64FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_4;		// 512Hz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_64;		// 8kHz
	}
	else {
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set LR Clock(11.025kHz)
 *	@param		None
 *	@return		INT32		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_auif_set_lrclock_11khz( T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle )
{
	if (clock == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_auif_set_lrclock_11khz() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	clock->aumclki = E_DD_AUDIO_MASTER_CLOCK_22_5792;
	clock->div_aumclko = E_DD_AUDIO_AUMCLKO_DIV_8;		// 2822.4kHz
	
	if (data_cycle == D_AUDIO_I2S_CYCLE_32FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_8;		// 352.8kHz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_32;		// 11.025kHz
	}
	else if (data_cycle == D_AUDIO_I2S_CYCLE_64FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_4;		// 705.6kHz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_64;		// 11.025kHz
	}
	else {
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set LR Clock(12kHz)
 *	@param		None
 *	@return		INT32		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_auif_set_lrclock_12khz( T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle )
{
	if (clock == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_auif_set_lrclock_12khz() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	clock->aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	clock->div_aumclko = E_DD_AUDIO_AUMCLKO_DIV_8;		// 3072kHz
	
	if (data_cycle == D_AUDIO_I2S_CYCLE_32FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_8;		// 384kHz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_32;		// 12kHz
	}
	else if (data_cycle == D_AUDIO_I2S_CYCLE_64FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_4;		// 768kHz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_64;		// 12kHz
	}
	else {
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set LR Clock(16kHz)
 *	@param		None
 *	@return		INT32		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_auif_set_lrclock_16khz( T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle )
{
	if (clock == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_auif_set_lrclock_16khz() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	clock->aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	clock->div_aumclko = E_DD_AUDIO_AUMCLKO_DIV_6;		// 4096kHz
	
	if (data_cycle == D_AUDIO_I2S_CYCLE_32FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_8;		// 512kHz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_32;		// 16kHz
	}
	else if (data_cycle == D_AUDIO_I2S_CYCLE_64FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_4;		// 1024kHz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_64;		// 16kHz
	}
	else {
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set LR Clock(22.05kHz)
 *	@param		None
 *	@return		INT32		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_auif_set_lrclock_22khz( T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle )
{
	if (clock == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_auif_set_lrclock_22khz() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	clock->aumclki = E_DD_AUDIO_MASTER_CLOCK_22_5792;
	clock->div_aumclko = E_DD_AUDIO_AUMCLKO_DIV_4;		// 5644.8kHz
	
	if (data_cycle == D_AUDIO_I2S_CYCLE_32FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_8;		// 705.6kHz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_32;		// 22.05kHz
	}
	else if (data_cycle == D_AUDIO_I2S_CYCLE_64FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_4;		// 1411.2kHz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_64;		// 22.05kHz
	}
	else {
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set LR Clock(24kHz)
 *	@param		None
 *	@return		INT32		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_auif_set_lrclock_24khz( T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle )
{
	if (clock == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_auif_set_lrclock_24khz() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	clock->aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	clock->div_aumclko = E_DD_AUDIO_AUMCLKO_DIV_4;		// 6144kHz
	
	if (data_cycle == D_AUDIO_I2S_CYCLE_32FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_8;		// 768kHz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_32;		// 24kHz
	}
	else if (data_cycle == D_AUDIO_I2S_CYCLE_64FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_4;		// 1536kHz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_64;		// 24kHz
	}
	else {
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set LR Clock(32kHz)
 *	@param		None
 *	@return		INT32		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_auif_set_lrclock_32khz( T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle )
{
	if (clock == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_auif_set_lrclock_32khz() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	clock->aumclki = E_DD_AUDIO_MASTER_CLOCK_8_192;
	clock->div_aumclko = E_DD_AUDIO_AUMCLKO_DIV_1;		// 8192kHz
	
	if (data_cycle == D_AUDIO_I2S_CYCLE_32FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_8;		// 1024kHz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_32;		// 32kHz
	}
	else if (data_cycle == D_AUDIO_I2S_CYCLE_64FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_4;		// 2048kHz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_64;		// 32kHz
	}
	else {
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set LR Clock(44.1kHz)
 *	@param		None
 *	@return		INT32		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_auif_set_lrclock_44khz( T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle )
{
	if (clock == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_auif_set_lrclock_44khz() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	clock->aumclki = E_DD_AUDIO_MASTER_CLOCK_22_5792;
	clock->div_aumclko = E_DD_AUDIO_AUMCLKO_DIV_2;		// 11289.6kHz
	
	if (data_cycle == D_AUDIO_I2S_CYCLE_32FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_8;		// 1411.2kHz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_32;		// 44.1kHz
	}
	else if (data_cycle == D_AUDIO_I2S_CYCLE_64FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_4;		// 2822.4kHz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_64;		// 44.1kHz
	}
	else {
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set LR Clock(48kHz)
 *	@param		None
 *	@return		INT32		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_auif_set_lrclock_48khz( T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle )
{
	if (clock == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_auif_set_lrclock_48khz() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	clock->aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	clock->div_aumclko = E_DD_AUDIO_AUMCLKO_DIV_2;		// 12288kHz
	
	if (data_cycle == D_AUDIO_I2S_CYCLE_32FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_8;		// 1536kHz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_32;		// 48kHz
	}
	else if (data_cycle == D_AUDIO_I2S_CYCLE_64FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_4;		// 3072kHz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_64;		// 48kHz
	}
	else {
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set LR Clock(96kHz)
 *	@param		None
 *	@return		INT32		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_auif_set_lrclock_96khz( T_DD_AUDIO_I2S_CMMN* clock, UCHAR data_cycle )
{
	if (clock == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_auif_set_lrclock_96khz() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	clock->aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	clock->div_aumclko = E_DD_AUDIO_AUMCLKO_DIV_1;		// 24576kHz
	
	if (data_cycle == D_AUDIO_I2S_CYCLE_32FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_8;		// 3072kHz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_32;		// 96kHz
	}
	else if (data_cycle == D_AUDIO_I2S_CYCLE_64FS){
		clock->div_auclk = E_DD_AUDIO_AUCLK_DIV_4;		// 6144kHz
		clock->div_lrclk = E_DD_AUDIO_AULR_DIV_64;		// 96kHz
	}
	else {
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set Audio Clock and Divider
 *	@param		None
 *	@return		INT32		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
static INT32 bf_audio_auif_set_clock( UINT8 dev_ch, const T_FJ_AUDIO_I2S_CFG* i2s_cfg )
{
	T_DD_AUDIO_I2S_CMMN i2s_common;
	INT32 ret;
	UINT32 loop;
	
	ret = Dd_Audio_Get_Ctrl_I2sCmmn(dev_ch, &i2s_common);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Get_Ctrl_I2sCmmn L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	
	if (i2s_cfg->master_slave == FJ_AUDIO_IF_MASTER){
		i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
		
		ret = D_AUDIO_ERR;
		for (loop = 0; loop < D_AUDIO_AUIF_NUM_LR_CLOCK; loop++){
			if (i2s_cfg->lr_clock == gBF_Audio_Auif_LRClock_Tbl[loop].lr_clock){
				ret = gBF_Audio_Auif_LRClock_Tbl[loop].process(&i2s_common, i2s_cfg->data_cycle);
			}
		}
		
		if (ret != D_AUDIO_OK){
			BF_Debug_Print_Error(("[AUDIO_AUIF]Set LR Clock Error. L=%d\n", __LINE__));
			return D_AUDIO_ERR;
		}
	}
	else {
		i2s_common.master_slave = E_DD_AUDIO_CLK_SLAVE;
	}
	
	ret = Dd_Audio_Ctrl_I2sCmmn(dev_ch, &i2s_common);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Ctrl_I2sCmmn L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}


/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/

/**
 *  @brief		Initialize AudioMacoro
 *	@param		None
 *	@return		INT32		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Auif_Init_AudioMacro( VOID )
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	static volatile UINT8 dd_init_flg = D_AUDIO_AUIF_OFF;
	
	Dd_ARM_Critical_Section_Start( critseq );
	
	if (dd_init_flg == D_AUDIO_AUIF_OFF){
		dd_init_flg = D_AUDIO_AUIF_ON;
		Dd_Audio_Init();
	}
	
	Dd_ARM_Critical_Section_End( critseq );
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Open input channel
 *	@param		UINT8				Channel of AudioMacro
 *	@return		INT32				Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Auif_OpenInput( UINT8 dev_ch )
{
	INT32 ret;

//	printf("BF_Audio_Auif_OpenInput() ch=%d\n",dev_ch);
	
	ret = Dd_Audio_Open_Input(dev_ch, D_DDIM_USER_SEM_WAIT_POL);
	if (ret != D_DDIM_OK) {
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Open_Input error(%d) L=%d\n", ret, __LINE__));
		ret = D_AUDIO_ERR;
	}
	else {
		ret = D_AUDIO_OK;
	}
	
	return ret;
}

/**
 *  @brief		Close input channel
 *	@param		UINT8				Channel of AudioMacro
 *	@return		INT32				Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Auif_CloseInput( UINT8 dev_ch )
{
	INT32 ret;
	
//	printf("BF_Audio_Auif_CloseInput() ch=%d\n",dev_ch);

	ret = Dd_Audio_Close_Input(dev_ch);
	if (ret != D_DDIM_OK) {
		// BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Close_Input error(%d) L=%d\n", ret, __LINE__));
		ret = D_AUDIO_ERR;
	}
	else {
		ret = D_AUDIO_OK;
	}
	
	return ret;
}

/**
 *  @brief		Open output channel
 *	@param		UINT8				Channel of AudioMacro
 *	@return		INT32				Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Auif_OpenOutput( UINT8 dev_ch )
{
	INT32 ret;
	
	ret = Dd_Audio_Open_Output(dev_ch, D_DDIM_USER_SEM_WAIT_POL);
	if (ret != D_DDIM_OK) {
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Open_Output error(%d) L=%d\n", ret, __LINE__));
		ret = D_AUDIO_ERR;
	}
	else {
		ret = D_AUDIO_OK;
	}
	
	return ret;
}

/**
 *  @brief		Close output channel
 *	@param		UINT8				Channel of AudioMacro
 *	@return		INT32				Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Auif_CloseOutput( UINT8 dev_ch )
{
	INT32 ret;
	
	ret = Dd_Audio_Close_Output(dev_ch);
	if (ret != D_DDIM_OK) {
		// BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Close_Output error(%d) L=%d\n", ret, __LINE__));
		ret = D_AUDIO_ERR;
	}
	else {
		ret = D_AUDIO_OK;
	}
	
	return ret;
}


/**
 *  @brief		Set I2S Input Configuration.
 *	@param		UINT8				Channel of AudioMacro
 *	@param		T_FJ_AUDIO_I2S_CFG	I2S Configuration
 *	@return		INT32				Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Auif_Set_I2sIn_Config( UINT8 dev_ch, T_FJ_AUDIO_I2S_CFG* i2s_cfg )
{
	T_DD_AUDIO_I2S_IN i2s_in;
	INT32 ret;
	
//	printf("BF_Audio_Auif_Set_I2sIn_Config() ch=%d\n",dev_ch);
	
	if (i2s_cfg == NULL){
		BF_Debug_Print_Error(("[AUDIO_AUIF]BF_Audio_Auif_Set_I2sIn_Config() param err. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	ret = Dd_Audio_Get_Ctrl_I2sIn(dev_ch, &i2s_in);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Get_Ctrl_I2sIn L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (i2s_cfg->i2s_format == E_FJ_AUDIO_I2S_Data_FORMAT_LEFT){
		i2s_in.mode_in = E_DD_AUDIO_MODE_L_JUST;
	}
	else if (i2s_cfg->i2s_format == E_FJ_AUDIO_I2S_Data_FORMAT_RIGHT){
		i2s_in.mode_in = E_DD_AUDIO_MODE_R_JUST;
	}
	else {
		// E_FJ_AUDIO_I2S_Data_FORMAT_I2S
		i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	}
	
	if (i2s_cfg->bit_count == D_AUDIO_PCM_16_BIT){
		i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	}
	else if (i2s_cfg->bit_count == D_AUDIO_PCM_24_BIT){
		i2s_in.bit_in = E_DD_AUDIO_BIT_24BIT;
	}
	else {
		// 8bit
		i2s_in.bit_in = E_DD_AUDIO_BIT_8BIT;
	}
	
	if ((dev_ch == D_DD_AUDIO_IF_CH4) ||
		(dev_ch == D_DD_AUDIO_IF_CH5) ){
		i2s_cfg->master_slave = FJ_AUDIO_IF_MASTER;
		i2s_cfg->data_cycle = D_AUDIO_I2S_CYCLE_64FS;		// work around for pdm
		i2s_in.mode_in = E_DD_AUDIO_MODE_I2S;
	}
	
	ret = bf_audio_auif_set_clock(dev_ch, i2s_cfg);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_auif_set_clock L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	ret = Dd_Audio_Ctrl_I2sIn(dev_ch, &i2s_in);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Ctrl_I2sIn L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (dev_ch == D_DD_AUDIO_IF_CH0){
		ret = Dd_Audio_Ctrl_I2sIn(D_DD_AUDIO_IF_CH1, &i2s_in);
		if (ret != D_DDIM_OK){
			BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Ctrl_I2sIn L=%d\n", __LINE__));
			return D_AUDIO_ERR;
		}
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set I2S Output Configuration.
 *	@param		UINT8				Channel of AudioMacro
 *	@param		T_FJ_AUDIO_I2S_CFG	I2S Configuration
 *	@return		INT32				Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Auif_Set_I2sOut_Config( UINT8 dev_ch, const T_FJ_AUDIO_I2S_CFG* i2s_cfg )
{
	T_DD_AUDIO_I2S_OUT i2s_out;
	INT32 ret;
	
	ret = Dd_Audio_Get_Ctrl_I2sOut(dev_ch, &i2s_out);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Get_Ctrl_I2sOut L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (i2s_cfg->i2s_format == E_FJ_AUDIO_I2S_Data_FORMAT_LEFT){
		i2s_out.mode_out = E_DD_AUDIO_MODE_L_JUST;
	}
	else if (i2s_cfg->i2s_format == E_FJ_AUDIO_I2S_Data_FORMAT_RIGHT){
		i2s_out.mode_out = E_DD_AUDIO_MODE_R_JUST;
	}
	else {
		// E_FJ_AUDIO_I2S_Data_FORMAT_I2S
		i2s_out.mode_out = E_DD_AUDIO_MODE_I2S;
	}
	
	if (i2s_cfg->bit_count == D_AUDIO_PCM_16_BIT){
		i2s_out.bit_out = E_DD_AUDIO_BIT_16BIT;
	}
	else if (i2s_cfg->bit_count == D_AUDIO_PCM_24_BIT){
		i2s_out.bit_out = E_DD_AUDIO_BIT_24BIT;
	}
	else {
		// 8bit
		i2s_out.bit_out = E_DD_AUDIO_BIT_8BIT;
	}
	
	if (i2s_cfg->data_cycle == D_AUDIO_I2S_CYCLE_32FS){
		i2s_out.fs = E_DD_AUDIO_FS_32FS;
	}
	else if (i2s_cfg->data_cycle == D_AUDIO_I2S_CYCLE_64FS){
		// 64fs
		i2s_out.fs = E_DD_AUDIO_FS_64FS;
	}
	else {
		return D_AUDIO_ERR;
	}
	
	ret = bf_audio_auif_set_clock(dev_ch, i2s_cfg);
	if (ret != D_AUDIO_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]bf_audio_auif_set_clock L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	ret = Dd_Audio_Ctrl_I2sOut(dev_ch, &i2s_out);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Ctrl_I2sOut L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (dev_ch == D_DD_AUDIO_IF_CH0){
		ret = Dd_Audio_Ctrl_I2sOut(D_DD_AUDIO_IF_CH1, &i2s_out);
		if (ret != D_DDIM_OK){
			BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Ctrl_I2sIn L=%d\n", __LINE__));
			return D_AUDIO_ERR;
		}
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set Channel Configuration.
 *	@param		UINT8		Channel of AudioMacro
 *	@param		UINT8		Channel Type(Stereo/Mono)
 *	@return		INT32		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Auif_Set_Channel_Config( UINT8 dev_ch, UINT8 ch_type )
{
	INT32 ret;
	T_DD_AUDIO_CTRL_COMMON ctrl_common;
	
	ret = Dd_Audio_Get_Ctrl_Common(dev_ch, &ctrl_common);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Get_Ctrl_Common Error L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if ( ch_type == FJ_AUDIO_NUM_CHANNEL_1 ){
		ctrl_common.channel = E_DD_AUDIO_CHANNEL_MONO;
	}
	else{
		ctrl_common.channel = E_DD_AUDIO_CHANNEL_STEREO;
	}
	
	ctrl_common.fifo_usage = E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	
	ret = Dd_Audio_Ctrl_Common(dev_ch, &ctrl_common);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Ctrl_Common Error L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (dev_ch == D_DD_AUDIO_IF_CH0){
		ret = Dd_Audio_Ctrl_Common(D_DD_AUDIO_IF_CH1, &ctrl_common);
		if (ret != D_DDIM_OK){
			BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Ctrl_Common L=%d\n", __LINE__));
			return D_AUDIO_ERR;
		}
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set Input Controll Configuration.
 *	@param		UINT8		Channel of AudioMacro
 *	@return		INT32		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Auif_Input_Ctrl( UINT8 dev_ch, UINT8 format )
{
	T_DD_AUDIO_CTRL_IN ctrl_input;
	INT32 ret;
	E_DD_AUDIO_BIT_SHIFT bit_shift;
	
//	printf("BF_Audio_Auif_Input_Ctrl() ch=%d, format=%d\n",dev_ch,format);
	
	ret = Dd_Audio_Get_Ctrl_Input(dev_ch, &ctrl_input);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Get_Ctrl_Input Error:%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (format == D_AUDIO_AUIF_AHB_FORMAT_R){
		ctrl_input.format = E_DD_AUDIO_DATA_REG_FRMT_R_JUST;
	}
	else {
		ctrl_input.format = E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	}
	
	switch (gBF_Auif_InGain_Tbl[dev_ch]){
		case D_AUDIO_AUIF_GAIN_0DB :
			bit_shift = E_DD_AUDIO_BIT_SHIFT_0;
			break;
		case D_AUDIO_AUIF_GAIN_6DB :
			bit_shift = E_DD_AUDIO_BIT_SHIFT_1;
			break;
		case D_AUDIO_AUIF_GAIN_12DB :
			bit_shift = E_DD_AUDIO_BIT_SHIFT_2;
			break;
		case D_AUDIO_AUIF_GAIN_18DB :
			bit_shift = E_DD_AUDIO_BIT_SHIFT_3;
			break;
		default:
			bit_shift = E_DD_AUDIO_BIT_SHIFT_0;
			break;
	}
	
	ctrl_input.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_input.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_input.bit_shift	= bit_shift;
	
	ret = Dd_Audio_Ctrl_Input(dev_ch, &ctrl_input);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Ctrl_Input Error:%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set Output Controll Configuration.
 *	@param		UINT8		Channel of AudioMacro
 *	@param		UINT8		format of transfer to P2S
 *	@return		INT32		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Auif_Output_Ctrl( UINT8 dev_ch, UINT8 format )
{
	T_DD_AUDIO_CTRL_OUT ctrl_output;
	INT32 ret;
	E_DD_AUDIO_BIT_SHIFT bit_shift;
	
	ret = Dd_Audio_Get_Ctrl_Output(dev_ch, &ctrl_output);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Get_Ctrl_Output Error L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (format == D_AUDIO_AUIF_AHB_FORMAT_R){
		ctrl_output.format = E_DD_AUDIO_DATA_REG_FRMT_R_JUST;
	}
	else {
		ctrl_output.format = E_DD_AUDIO_DATA_REG_FRMT_L_JUST;
	}
	
	switch (gBF_Auif_OutGain_Tbl[dev_ch]){
		case D_AUDIO_AUIF_GAIN_0DB :
			bit_shift = E_DD_AUDIO_BIT_SHIFT_0;
			break;
		case D_AUDIO_AUIF_GAIN_6DB :
			bit_shift = E_DD_AUDIO_BIT_SHIFT_1;
			break;
		case D_AUDIO_AUIF_GAIN_12DB :
			bit_shift = E_DD_AUDIO_BIT_SHIFT_2;
			break;
		case D_AUDIO_AUIF_GAIN_18DB :
			bit_shift = E_DD_AUDIO_BIT_SHIFT_3;
			break;
		default:
			bit_shift = E_DD_AUDIO_BIT_SHIFT_0;
			break;
	}
	
	ctrl_output.fifo_stages	= E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_output.ahb_format	= E_DD_AUDIO_AHB_FRMT_L_JUST;
	ctrl_output.lr_copy		= D_DD_AUDIO_DISABLE;
	ctrl_output.mix_play	= D_DD_AUDIO_DISABLE;
	ctrl_output.bit_shift	= bit_shift;
	
	ret = Dd_Audio_Ctrl_Output(dev_ch, &ctrl_output);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Ctrl_Output Error L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set Input DMA Mode
 *	@param		UINT8				Channel of AudioMacro
 *	@param		E_AUDIO_DMA_MODE	DMA Mode(1ch/2sh)
 *	@param		USHORT				The number of the transfer change samples
 *	@return		INT32				Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Auif_Set_Dma_ModeIn( UINT8 dev_ch, E_AUDIO_DMA_MODE mode, USHORT sample )
{
	T_DD_AUDIO_DMA dma_setting;
	INT32 ret;
	
	ret = Dd_Audio_Get_Ctrl_DmaIn(dev_ch, &dma_setting);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Get_Ctrl_DmaIn Error L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (mode == E_AUDIO_DMA_MODE_IN_1CH){
		dma_setting.dma_2ch = D_DD_AUDIO_DISABLE;
	}
	else if (mode == E_AUDIO_DMA_MODE_IN_2CH){
		dma_setting.dma_2ch = D_DD_AUDIO_ENABLE;
		dma_setting.dma_trnsf_cnt = sample;
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_AUIF]BF_Audio_Auif_Set_Dma_ModeIn Error mode=%d\n", mode));
		return D_AUDIO_ERR;
	}
	
	ret = Dd_Audio_Ctrl_DmaIn(dev_ch, &dma_setting);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Ctrl_DmaIn Error L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *  @brief		Set Output DMA Mode
 *	@param		UINT8				Channel of AudioMacro
 *	@param		E_AUDIO_DMA_MODE	DMA Mode(1ch/2sh)
 *	@param		USHORT				The number of the transfer change samples
 *	@return		INT32				Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Auif_Set_Dma_ModeOut( UINT8 dev_ch, E_AUDIO_DMA_MODE mode, USHORT sample )
{
	T_DD_AUDIO_DMA dma_setting;
	INT32 ret;
	
	ret = Dd_Audio_Get_Ctrl_DmaOut(dev_ch, &dma_setting);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Get_Ctrl_DmaOut Error L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (mode == E_AUDIO_DMA_MODE_OUT_1CH){
		dma_setting.dma_2ch = D_DD_AUDIO_DISABLE;
	}
	else if (mode == E_AUDIO_DMA_MODE_OUT_2CH){
		dma_setting.dma_2ch = D_DD_AUDIO_ENABLE;
		dma_setting.dma_trnsf_cnt = sample;
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_AUIF]BF_Audio_Auif_Set_Dma_ModeIn Error mode=%d\n", mode));
		return D_AUDIO_ERR;
	}
	
	ret = Dd_Audio_Ctrl_DmaOut(dev_ch, &dma_setting);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Ctrl_DmaIn Error L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	return D_AUDIO_OK;
}

/**
 *	@brief		Get Input Transfer Samples
 *	@param		UINT8		Channel of AudioMacro
 *	@return		UINT16		The number of Transfer Samples
 *	@note		None
 *	@attention	None
 */
UINT16 BF_Audio_Auif_Get_Input_Trns_Smaple( UINT8 dev_ch )
{
	return Dd_Audio_Get_InDmaSample(dev_ch);
}

/**
 *	@brief		Get Output Transfer Samples
 *	@param		UINT8		Channel of AudioMacro
 *	@return		UINT16		The number of Transfer Samples
 *	@note		None
 *	@attention	None
 */
UINT16 BF_Audio_Auif_Get_Output_Trns_Smaple( UINT8 dev_ch )
{
	return Dd_Audio_Get_OutDmaSample(dev_ch);
}

/*
 *	@brief		Get address of registar AUIDLR
 *	@param		UINT8		Channel of AudioMacro
 *	@return		UINT32		address of register AUIDLR
 *	@note		None
 *	@attention	None
 */
UINT32 BF_Audio_Auif_Get_Addr_Reg_AUIDLR( UINT8 dev_ch )
{
	return Dd_Audio_Get_Addr_Reg_AUIDLR(dev_ch);
}

/*
 *	@brief		Get address of registar AUODLR
 *	@param		UINT8		Channel of AudioMacro
 *	@return		UINT32		address of register AUODLR
 *	@note		None
 *	@attention	None
 */
UINT32 BF_Audio_Auif_Get_Addr_Reg_AUODLR( UINT8 dev_ch )
{
	return Dd_Audio_Get_Addr_Reg_AUODLR(dev_ch);
}

/*
 *	@brief		Get address of registar AUIDDMAPT
 *	@param		UINT8		Channel of AudioMacro
 *	@return		UINT32		address of register AUIDDMAPT
 *	@note		None
 *	@attention	None
 */
UINT32 BF_Audio_Auif_Get_Addr_Reg_AUIDDMAPT( UINT8 dev_ch )
{
	return Dd_Audio_Get_Addr_Reg_AUIDDMAPT(dev_ch);
}

/*
 *	@brief		Get address of registar AUODDMAPT
 *	@param		UINT8		Channel of AudioMacro
 *	@return		UINT32		address of register AUODDMAPT
 *	@note		None
 *	@attention	None
 */
UINT32 BF_Audio_Auif_Get_Addr_Reg_AUODDMAPT( UINT8 dev_ch )
{
	return Dd_Audio_Get_Addr_Reg_AUODDMAPT(dev_ch);
}

/*
 *	@brief		Start Input
 *	@param		UINT8		Channel of AudioMacro
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Input_Start( UINT8 dev_ch )
{
	INT32 ret;
	
	if (dev_ch == D_AUDIO_AUIF_CH0_CH1){
		if ((Dd_Audio_Get_Status_Input(D_AUDIO_AUIF_CH0) == FALSE) &&
			(Dd_Audio_Get_Status_Input(D_AUDIO_AUIF_CH1) == FALSE) ){
			
			ret = Dd_Audio_Set_InputDMARequestEnable(D_AUDIO_AUIF_CH0, D_DD_AUDIO_ENABLE);
			if (ret == D_DDIM_OK){
				ret = Dd_Audio_Set_InputDMARequestEnable(D_AUDIO_AUIF_CH1, D_DD_AUDIO_ENABLE);
				if (ret != D_DDIM_OK){
					BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Set_InputDMARequestEnable Error L=%d\n", __LINE__));
				}
				else {
					BF_Audio_Auif_Set_Enable_OverFlow_Int(D_AUDIO_AUIF_CH0);
					BF_Audio_Auif_Set_Enable_OverFlow_Int(D_AUDIO_AUIF_CH1);
					
					ret = Dd_Audio_Start_Input(D_AUDIO_AUIF_CH0_CH1);
					if (ret != D_DDIM_OK){
						BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Start_Input Error L=%d\n", __LINE__));
					}
				}
			}
			else {
				BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Set_InputDMARequestEnable Error L=%d\n", __LINE__));
			}
		}
		else {
			BF_Debug_Print_Warning(("[AUDIO_AUIF]Input Operation Processing... L=%d\n", __LINE__));
		}
	}
	else if (dev_ch == D_AUDIO_AUIF_CH4_CH5){
		if ((Dd_Audio_Get_Status_Input(D_AUDIO_AUIF_CH4) == FALSE) &&
			(Dd_Audio_Get_Status_Input(D_AUDIO_AUIF_CH5) == FALSE) ){
			
			ret = Dd_Audio_Set_InputDMARequestEnable(D_AUDIO_AUIF_CH4, D_DD_AUDIO_ENABLE);
			if (ret == D_DDIM_OK){
				ret = Dd_Audio_Set_InputDMARequestEnable(D_AUDIO_AUIF_CH5, D_DD_AUDIO_ENABLE);
				if (ret != D_DDIM_OK){
					BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Set_InputDMARequestEnable Error L=%d\n", __LINE__));
				}
				else {
					BF_Audio_Auif_Set_Enable_OverFlow_Int(D_AUDIO_AUIF_CH4);
					BF_Audio_Auif_Set_Enable_OverFlow_Int(D_AUDIO_AUIF_CH5);
					
					ret = Dd_Audio_Start_Input(D_AUDIO_AUIF_CH4_CH5);
					if (ret != D_DDIM_OK){
						BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Start_Input Error L=%d\n", __LINE__));
					}
				}
			}
			else {
				BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Set_InputDMARequestEnable Error L=%d\n", __LINE__));
			}
		}
		else {
			BF_Debug_Print_Warning(("[AUDIO_AUIF]Input Operation Processing... L=%d\n", __LINE__));
		}
	}
	else {
		if (Dd_Audio_Get_Status_Input(dev_ch) == FALSE){
			
			ret = Dd_Audio_Set_InputDMARequestEnable(dev_ch, D_DD_AUDIO_ENABLE);
			if (ret == D_DDIM_OK){
				BF_Audio_Auif_Set_Enable_OverFlow_Int(dev_ch);
				
				ret = Dd_Audio_Start_Input(dev_ch);
				if (ret != D_DDIM_OK){
					BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Start_Input Error L=%d\n", __LINE__));
				}
			}
			else {
				BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Set_InputDMARequestEnable Error L=%d\n", __LINE__));
			}
		}
		else {
			BF_Debug_Print_Warning(("[AUDIO_AUIF]Input Operation Processing... L=%d\n", __LINE__));
		}
	}
}

/*
 *	@brief		Stop Input
 *	@param		UINT8		Channel of AudioMacro
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Input_Stop( UINT8 dev_ch )
{
	if (dev_ch == D_AUDIO_AUIF_CH0_CH1){
		BF_Audio_Auif_Set_Disable_OverFlow_Int(D_AUDIO_AUIF_CH0);
		BF_Audio_Auif_Set_Disable_OverFlow_Int(D_AUDIO_AUIF_CH1);
	}
	else if (dev_ch == D_AUDIO_AUIF_CH4_CH5){
		BF_Audio_Auif_Set_Disable_OverFlow_Int(D_AUDIO_AUIF_CH4);
		BF_Audio_Auif_Set_Disable_OverFlow_Int(D_AUDIO_AUIF_CH5);
	}
	else {
		BF_Audio_Auif_Set_Disable_OverFlow_Int(dev_ch);
	}
	
	Dd_Audio_Stop_Input(dev_ch);
	
	if (dev_ch == D_AUDIO_AUIF_CH0_CH1){
		Dd_Audio_Set_InputDMARequestEnable(D_AUDIO_AUIF_CH0, D_DD_AUDIO_DISABLE);
		Dd_Audio_Set_InputDMARequestEnable(D_AUDIO_AUIF_CH1, D_DD_AUDIO_DISABLE);
	}
	else if (dev_ch == D_AUDIO_AUIF_CH4_CH5){
		Dd_Audio_Set_InputDMARequestEnable(D_AUDIO_AUIF_CH4, D_DD_AUDIO_DISABLE);
		Dd_Audio_Set_InputDMARequestEnable(D_AUDIO_AUIF_CH5, D_DD_AUDIO_DISABLE);
	}
	else {
		Dd_Audio_Set_InputDMARequestEnable(dev_ch, D_DD_AUDIO_DISABLE);
	}
}

/*
 *	@brief		Start Output
 *	@param		UINT8		Channel of AudioMacro
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Output_Start( UINT8 dev_ch )
{
	INT32 ret;
	
	if (dev_ch != D_AUDIO_AUIF_CH0_CH1){
		if (Dd_Audio_Get_Status_Output(dev_ch) == FALSE){
			ret = Dd_Audio_Set_OutputDMARequestEnable(dev_ch, D_DD_AUDIO_ENABLE);
			if (ret == D_DDIM_OK){
				BF_Audio_Auif_Set_Enable_UnderFlow_Int(dev_ch);
				
				ret = Dd_Audio_Start_Output(dev_ch);
				if (ret != D_DDIM_OK){
					BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Start_Input Error L=%d\n", __LINE__));
				}
			}
			else {
				BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Set_OutputDMARequestEnable Error L=%d\n", __LINE__));
			}
		}
		else {
			BF_Debug_Print_Warning(("[AUDIO_AUIF]Output Operation Processing... L=%d\n", __LINE__));
		}
	}
	else {
		if ((Dd_Audio_Get_Status_Output(D_AUDIO_AUIF_CH0) == FALSE)	&&
			(Dd_Audio_Get_Status_Output(D_AUDIO_AUIF_CH1) == FALSE)	){
			
			ret = Dd_Audio_Set_OutputDMARequestEnable(D_AUDIO_AUIF_CH0, D_DD_AUDIO_ENABLE);
			if (ret == D_DDIM_OK){
				ret = Dd_Audio_Set_OutputDMARequestEnable(D_AUDIO_AUIF_CH1, D_DD_AUDIO_ENABLE);
				if (ret == D_DDIM_OK){
					BF_Audio_Auif_Set_Enable_UnderFlow_Int(D_AUDIO_AUIF_CH0);
					BF_Audio_Auif_Set_Enable_UnderFlow_Int(D_AUDIO_AUIF_CH1);
					
					ret = Dd_Audio_Start_Output(D_AUDIO_AUIF_CH0_CH1);
					if (ret != D_DDIM_OK){
						BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Start_Input Error L=%d\n", __LINE__));
					}
				}
				else {
					BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Set_OutputDMARequestEnable Error L=%d\n", __LINE__));
				}
			}
			else {
				BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Set_OutputDMARequestEnable Error L=%d\n", __LINE__));
			}
		}
		else {
			BF_Debug_Print_Warning(("[AUDIO_AUIF]Output Operation Processing... L=%d\n", __LINE__));
		}
	}
}

/*
 *	@brief		Stop Output
 *	@param		UINT8		Channel of AudioMacro
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Output_Stop( UINT8 dev_ch )
{
	if (dev_ch != D_AUDIO_AUIF_CH0_CH1){
		BF_Audio_Auif_Set_Disable_UnderFlow_Int(dev_ch);
	}
	else {
		BF_Audio_Auif_Set_Disable_UnderFlow_Int(D_AUDIO_AUIF_CH0);
		BF_Audio_Auif_Set_Disable_UnderFlow_Int(D_AUDIO_AUIF_CH1);
	}
	
	if (dev_ch != D_AUDIO_AUIF_CH0_CH1){
		Dd_Audio_Set_OutputDMARequestEnable(dev_ch, D_DD_AUDIO_DISABLE);
	}
	else {
		Dd_Audio_Set_OutputDMARequestEnable(D_AUDIO_AUIF_CH0, D_DD_AUDIO_DISABLE);
		Dd_Audio_Set_OutputDMARequestEnable(D_AUDIO_AUIF_CH1, D_DD_AUDIO_DISABLE);
	}
	
	Dd_Audio_Stop_Output(dev_ch);
}

/*
 *	@brief		Interrupt Handler
 *	@param		UCHAR		Channel of AudioMacro
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Int_Handler( UCHAR ch )
{
	Dd_Audio_Int_Handler(ch);
}

/*
 *	@brief		Enable interrupt of overflow
 *	@param		UINT8		Channel of AudioMacro
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Set_Enable_OverFlow_Int( UINT8 dev_ch )
{
	INT32 ret;
	T_DD_AUDIO_CALLBACK callback;
	
	switch(dev_ch){
		case D_DD_AUDIO_IF_CH0:
			callback = (T_DD_AUDIO_CALLBACK)BF_Audio_Auif_Over_Flow_Error_Ch0;
			break;
		case D_DD_AUDIO_IF_CH1:
			callback = (T_DD_AUDIO_CALLBACK)BF_Audio_Auif_Over_Flow_Error_Ch1;
			break;
		case D_DD_AUDIO_IF_CH2:
			callback = (T_DD_AUDIO_CALLBACK)BF_Audio_Auif_Over_Flow_Error_Ch2;
			break;
		case D_DD_AUDIO_IF_CH4:
			callback = (T_DD_AUDIO_CALLBACK)BF_Audio_Auif_Over_Flow_Error_Ch4;
			break;
		case D_DD_AUDIO_IF_CH5:
			callback = (T_DD_AUDIO_CALLBACK)BF_Audio_Auif_Over_Flow_Error_Ch5;
			break;
		default:
			callback = (T_DD_AUDIO_CALLBACK)NULL;
			break;
	}
	
	ret = Dd_Audio_Set_EnableInOverFlowIntr(dev_ch, D_DD_AUDIO_ENABLE, callback);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Set_EnableInOverFlowIntr Error L=%d\n", __LINE__));
	}
}

/*
 *	@brief		Disable interrupt of overflow
 *	@param		UINT8		Channel of AudioMacro
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Set_Disable_OverFlow_Int( UINT8 dev_ch )
{
	INT32 ret;
	
	ret = Dd_Audio_Set_EnableInOverFlowIntr(dev_ch, D_DD_AUDIO_DISABLE, NULL);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Set_EnableInOverFlowIntr Error L=%d\n", __LINE__));
	}
}

/*
 *	@brief		Enable interrupt of underflow
 *	@param		UINT8		Channel of AudioMacro
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Set_Enable_UnderFlow_Int( UINT8 dev_ch )
{
	INT32 ret;
	T_DD_AUDIO_CALLBACK callback;
	
	switch(dev_ch){
		case D_DD_AUDIO_IF_CH0:
			callback = (T_DD_AUDIO_CALLBACK)BF_Audio_Auif_Under_Flow_Error_Ch0;
			break;
		case D_DD_AUDIO_IF_CH1:
			callback = (T_DD_AUDIO_CALLBACK)BF_Audio_Auif_Under_Flow_Error_Ch1;
			break;
		case D_DD_AUDIO_IF_CH2:
			callback = (T_DD_AUDIO_CALLBACK)BF_Audio_Auif_Under_Flow_Error_Ch2;
			break;
		case D_DD_AUDIO_IF_CH3:
			callback = (T_DD_AUDIO_CALLBACK)BF_Audio_Auif_Under_Flow_Error_Ch3;
			break;
		default:
			callback = (T_DD_AUDIO_CALLBACK)NULL;
			break;
	}
	
	ret = Dd_Audio_Set_EnableOutUnderFlowIntr(dev_ch, D_DD_AUDIO_ENABLE, callback);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Set_EnableOutUnderFlowIntr Error L=%d\n", __LINE__));
	}
}

/*
 *	@brief		Disable interrupt of underflow
 *	@param		UINT8		Channel of AudioMacro
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Set_Disable_UnderFlow_Int( UINT8 dev_ch )
{
	INT32 ret;
	
	ret = Dd_Audio_Set_EnableOutUnderFlowIntr(dev_ch, D_DD_AUDIO_DISABLE, NULL);
	if (ret != D_DDIM_OK){
		BF_Debug_Print_Error(("[AUDIO_AUIF]Dd_Audio_Set_EnableOutUnderFlowIntr Error L=%d\n", __LINE__));
	}
}

/*
 *	@brief		Notification OverFlow Error(CH0)
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Over_Flow_Error_Ch0( VOID )
{
//	BF_Audio_Auif_Set_Disable_OverFlow_Int(D_DD_AUDIO_IF_CH0);
//	BF_Debug_Print_Error(("[AUDIO] Over Flow\n"));
}

/*
 *	@brief		Notification UnderFlow Error(CH0)
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Under_Flow_Error_Ch0( VOID )
{
//	BF_Audio_Auif_Set_Disable_UnderFlow_Int(D_DD_AUDIO_IF_CH0);
//	BF_Debug_Print_Error(("[AUDIO] Under Flow\n"));
}

/*
 *	@brief		Notification OverFlow Error(CH0)
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Over_Flow_Error_Ch1( VOID )
{
//	BF_Audio_Auif_Set_Disable_OverFlow_Int(D_DD_AUDIO_IF_CH1);
//	BF_Debug_Print_Error(("[AUDIO] Over Flow\n"));
}

/*
 *	@brief		Notification UnderFlow Error(CH1)
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Under_Flow_Error_Ch1( VOID )
{
//	BF_Audio_Auif_Set_Disable_UnderFlow_Int(D_DD_AUDIO_IF_CH1);
//	BF_Debug_Print_Error(("[AUDIO] Under Flow\n"));
}

/*
 *	@brief		Notification OverFlow Error(CH2)
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Over_Flow_Error_Ch2( VOID )
{
//	BF_Audio_Auif_Set_Disable_OverFlow_Int(D_DD_AUDIO_IF_CH2);
//	BF_Debug_Print_Error(("[AUDIO] Over Flow\n"));
}

/*
 *	@brief		Notification UnderFlow Error(CH2)
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Under_Flow_Error_Ch2( VOID )
{
//	BF_Audio_Auif_Set_Disable_UnderFlow_Int(D_DD_AUDIO_IF_CH2);
//	BF_Debug_Print_Error(("[AUDIO] Under Flow\n"));
}

/*
 *	@brief		Notification UnderFlow Error(CH3)
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Under_Flow_Error_Ch3( VOID )
{
//	BF_Audio_Auif_Set_Disable_UnderFlow_Int(D_DD_AUDIO_IF_CH3);
//	BF_Debug_Print_Error(("[AUDIO] Under Flow\n"));
}

/*
 *	@brief		Notification OverFlow Error(CH4)
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Over_Flow_Error_Ch4( VOID )
{
//	BF_Audio_Auif_Set_Disable_OverFlow_Int(D_DD_AUDIO_IF_CH4);
//	BF_Debug_Print_Error(("[AUDIO] Over Flow\n"));
}

/*
 *	@brief		Notification OverFlow Error(CH5)
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Over_Flow_Error_Ch5( VOID )
{
//	BF_Audio_Auif_Set_Disable_OverFlow_Int(D_DD_AUDIO_IF_CH5);
//	BF_Debug_Print_Error(("[AUDIO] Over Flow\n"));
}

/*
 *	@brief		Swap Input Data
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Swap_InputData( UINT8 dev_ch, UINT8 enable )
{
	Dd_Audio_Set_SwapHW_AUIDLR(dev_ch, enable);
}

/*
 *	@brief		Swap Output Data
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID BF_Audio_Auif_Swap_OutputData( UINT8 dev_ch, UINT8 enable )
{
	Dd_Audio_Set_SwapHW_AUODLR(dev_ch, enable);
}


/*
 *	@brief		Set Input Volume
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Auif_Set_InputGain( UINT8 dev_ch, UINT8 volume )
{
	if (dev_ch > D_AUDIO_AUIF_CH5){
		BF_Debug_Print_Error(("[AUDIO_AUIF]BF_Audio_Auif_Set_InputGain() dev_ch=%d, L=%d\n", dev_ch, __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (volume > D_AUDIO_AUIF_GAIN_18DB){
		BF_Debug_Print_Error(("[AUDIO_AUIF]BF_Audio_Auif_Set_InputGain() volume=%d, L=%d\n", volume, __LINE__));
		return D_AUDIO_ERR;
	}
	
	gBF_Auif_InGain_Tbl[dev_ch] = volume;
	
	return D_AUDIO_OK;
}

/*
 *	@brief		Set Output Volume
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 BF_Audio_Auif_Set_OutputGain( UINT8 dev_ch, UINT8 volume )
{
	if (dev_ch > D_AUDIO_AUIF_CH3){
		BF_Debug_Print_Error(("[AUDIO_AUIF]BF_Audio_Auif_Set_OutputGain() dev_ch=%d, L=%d\n", dev_ch, __LINE__));
		return D_AUDIO_ERR;
	}
	
	if (volume > D_AUDIO_AUIF_GAIN_18DB){
		BF_Debug_Print_Error(("[AUDIO_AUIF]BF_Audio_Auif_Set_OutputGain() volume=%d, L=%d\n", volume, __LINE__));
		return D_AUDIO_ERR;
	}
	
	gBF_Auif_OutGain_Tbl[dev_ch] = volume;
	
	return D_AUDIO_OK;
}

