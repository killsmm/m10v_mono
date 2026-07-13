/**
 * @file		dd_audio.h
 * @brief		Header file of AudioI/F driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/**
@addtogroup lsi_peri
@{
	@addtogroup dd_audio
	@{
		- @ref Audio_Input_Sample
		- @ref Audio_Output_Sample

*/

#ifndef _DD_AUDIO_H_
#define _DD_AUDIO_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
/* Return Value */
#define D_DD_AUDIO_INPUT_PARAM_ERROR	(D_DD_AUDIO | D_DDIM_INPUT_PARAM_ERROR)	/**< Input Parameter Error */
#define D_DD_AUDIO_MACRO_BUSY_NG		(D_DD_AUDIO | D_DDIM_MACRO_BUSY_NG)		/**< Macro busy */
#define D_DD_AUDIO_SEM_TIMEOUT			(D_DD_AUDIO | D_DDIM_SEM_TIMEOUT)		/**< Semaphore acquisition Time Out */
#define D_DD_AUDIO_SEM_NG				(D_DD_AUDIO | D_DDIM_SEM_NG)			/**< Semaphore acquisition NG */
#define D_DD_AUDIO_SYSTEM_ERROR			(D_DD_AUDIO | D_DDIM_SYSTEM_ERROR)		/**< System error */

#define D_DD_AUDIO_IF_CH0				(0x00)									/**< Audio IF Channel 0  */
#define D_DD_AUDIO_IF_CH1				(0x01)									/**< Audio IF Channel 1 */
#define D_DD_AUDIO_IF_CH2				(0x02)									/**< Audio IF Channel 2 */
#define D_DD_AUDIO_IF_CH3				(0x03)									/**< Audio IF Channel 3 */
#define D_DD_AUDIO_IF_CH4				(0x04)									/**< Audio IF Channel 4 */
#define D_DD_AUDIO_IF_CH5				(0x05)									/**< Audio IF Channel 5 */
#define D_DD_AUDIO_IF_CH0_CH1			(0x10)									/**< Audio IF Channel 0 & 1<br>
																					 This define is available only in following functions.<br>
																					 - Dd_Audio_Start_Input()<br>
																					 - Dd_Audio_Stop_Input()<br>
																					 - Dd_Audio_Start_Output()<br>
																					 - Dd_Audio_Stop_Output()<br>
																					 - Dd_Audio_Start_LoopBack()<br>
																					 - Dd_Audio_Stop_LoopBack() */
#define D_DD_AUDIO_IF_CH4_CH5			(0x11)									/**< Audio IF Channel 4 & 5<br>
																					 This define is available only in following functions.<br>
																					 - Dd_Audio_Start_Input()<br>
																					 - Dd_Audio_Stop_Input() */

#define D_DD_AUDIO_IF_CH_NUM_MAX		(0x06)									/**< Channel Max Number */
#define D_DD_AUDIO_IF_IN_CH_NUM_MAX		(D_DD_AUDIO_IF_CH_NUM_MAX)			/**< Input Channel Max Number */
#define D_DD_AUDIO_IF_OUT_CH_NUM_MAX	(0x04)									/**< Output Channel Max Number */
#define D_DD_AUDIO_IF_LB_CH_NUM_MAX		(0x03)									/**< LoopBack Channel Max Number */

/* Function Enable */
#define	D_DD_AUDIO_DISABLE				(0)									/**< Disable	*/
#define	D_DD_AUDIO_ENABLE				(1)									/**< Enable		*/

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

/** Data Output/Input Register Format */
typedef enum {
	E_DD_AUDIO_DATA_REG_FRMT_R_JUST	= 0,								/**< Left justified */
	E_DD_AUDIO_DATA_REG_FRMT_L_JUST										/**< Right justified */
} E_DD_AUDIO_DATA_REG_FRMT;

/** FIFO Stages */
typedef enum {
	E_DD_AUDIO_FIFO_STAGES_1	= 0,									/**< FIFO Stages 1 */
	E_DD_AUDIO_FIFO_STAGES_16											/**< FIFO Stages 16 */
} E_DD_AUDIO_FIFO_STAGES;

/** FIFO Usage */
typedef enum {
	E_DD_AUDIO_FIFO_USAGE_STAGES_1	= 0,								/**< FIFO Usages Status 1 stage */
	E_DD_AUDIO_FIFO_USAGE_STAGES_12,									/**< FIFO Usages Status 12 stages */
	E_DD_AUDIO_FIFO_USAGE_STAGES_8,										/**< FIFO Usages Status 8 stages */
	E_DD_AUDIO_FIFO_USAGE_STAGES_4,										/**< FIFO Usages Status 4 stages */
	E_DD_AUDIO_FIFO_USAGE_STAGES_16										/**< FIFO Usages Status 16 stages */
} E_DD_AUDIO_FIFO_USAGE;


/** AHB Bus Format */
typedef enum {	
	E_DD_AUDIO_AHB_FRMT_R_JUST	= 0,									/**< Right justified */
	E_DD_AUDIO_AHB_FRMT_L_JUST											/**< Left justified */
} E_DD_AUDIO_AHB_FRMT;


/** I2S Data Format */
typedef enum {
	E_DD_AUDIO_MODE_L_JUST	= 0,										/**< Left justified */
	E_DD_AUDIO_MODE_R_JUST,												/**< Right justified */
	E_DD_AUDIO_MODE_I2S													/**< I2S format */
} E_DD_AUDIO_MODE;

/** Valid Bit Count */
typedef enum {
	E_DD_AUDIO_BIT_8BIT		= 0,										/**< 8bit */
	E_DD_AUDIO_BIT_16BIT,												/**< 16bit */
	E_DD_AUDIO_BIT_24BIT												/**< 24bit */
} E_DD_AUDIO_BIT;

/** I2S master/slave mode */
typedef enum {
	E_DD_AUDIO_CLK_MASTER = 0,											/**< MASTER */
	E_DD_AUDIO_CLK_SLAVE												/**< SLAVE */
} E_DD_AUDIO_CLK_MODE;

/** Audio Master Clock Setting */
typedef enum {
	E_DD_AUDIO_MASTER_CLOCK_8_192 = 0,									/**< 8.192MHz */
	E_DD_AUDIO_MASTER_CLOCK_22_5792,									/**< 22.5792MHz */
	E_DD_AUDIO_MASTER_CLOCK_24_576,										/**< 24.576MHz */
	E_DD_AUDIO_MASTER_CLOCK_36_864,										/**< 36.864MHz */
	E_DD_AUDIO_MASTER_CLOCK_AUMCLKI										/**< AUMCLKI */
} E_DD_AUDIO_MASTER_CLOCK;

/** AUMCLKO Clock Division Ratio */
typedef enum {
	E_DD_AUDIO_AUMCLKO_DIV_1	= 0,									/**< Divide reference clock by 1 */
	E_DD_AUDIO_AUMCLKO_DIV_2	= 1,									/**< Divide reference clock by 2 */
	E_DD_AUDIO_AUMCLKO_DIV_4	= 3,									/**< Divide reference clock by 4 */
	E_DD_AUDIO_AUMCLKO_DIV_6	= 5,									/**< Divide reference clock by 6 */
	E_DD_AUDIO_AUMCLKO_DIV_8	= 7,									/**< Divide reference clock by 8 */
	E_DD_AUDIO_AUMCLKO_DIV_10	= 9,									/**< Divide reference clock by 10 */
	E_DD_AUDIO_AUMCLKO_DIV_12	= 11,									/**< Divide reference clock by 12 */
	E_DD_AUDIO_AUMCLKO_DIV_14	= 13,									/**< Divide reference clock by 14 */
	E_DD_AUDIO_AUMCLKO_DIV_16	= 15,									/**< Divide reference clock by 16 */
	E_DD_AUDIO_AUMCLKO_DIV_18	= 17,									/**< Divide reference clock by 18 */
	E_DD_AUDIO_AUMCLKO_DIV_20	= 19,									/**< Divide reference clock by 20 */
	E_DD_AUDIO_AUMCLKO_DIV_22	= 21,									/**< Divide reference clock by 22 */
	E_DD_AUDIO_AUMCLKO_DIV_24	= 23,									/**< Divide reference clock by 24 */
	E_DD_AUDIO_AUMCLKO_DIV_26	= 25,									/**< Divide reference clock by 26 */
	E_DD_AUDIO_AUMCLKO_DIV_28	= 27,									/**< Divide reference clock by 28 */
	E_DD_AUDIO_AUMCLKO_DIV_30	= 29,									/**< Divide reference clock by 30 */
	E_DD_AUDIO_AUMCLKO_DIV_32	= 31									/**< Divide reference clock by 32 */
}E_DD_AUDIO_AUMCLKO_DIV_CLK;

/** AUCLK Clock Division Ratio */
typedef enum {
	E_DD_AUDIO_AUCLK_DIV_4	= 0,										/**< Divide AUMCLKO clock by 4 */
	E_DD_AUDIO_AUCLK_DIV_8,												/**< Divide AUMCLKO clock by 8 */
	E_DD_AUDIO_AUCLK_DIV_12,											/**< Divide AUMCLKO clock by 12 */
	E_DD_AUDIO_AUCLK_DIV_16,											/**< Divide AUMCLKO clock by 16 */
	E_DD_AUDIO_AUCLK_DIV_20,											/**< Divide AUMCLKO clock by 20 */
	E_DD_AUDIO_AUCLK_DIV_24,											/**< Divide AUMCLKO clock by 24 */
	E_DD_AUDIO_AUCLK_DIV_28,											/**< Divide AUMCLKO clock by 28 */
	E_DD_AUDIO_AUCLK_DIV_32												/**< Divide AUMCLKO clock by 32 */
}E_DD_AUDIO_AUCLK_DIV_CLK;

/** AULR Clock Division Ratio */
typedef enum {
	E_DD_AUDIO_AULR_DIV_16	= 0,										/**< Divide AUCLK clock by 16 */
	E_DD_AUDIO_AULR_DIV_32,												/**< Divide AUCLK clock by 32 */
	E_DD_AUDIO_AULR_DIV_48,												/**< Divide AUCLK clock by 48 */
	E_DD_AUDIO_AULR_DIV_64												/**< Divide AUCLK clock by 64 */
}E_DD_AUDIO_AULR_DIV_CLK;

/** Serial Clock vs LR Channel Clock */
typedef enum {
	E_DD_AUDIO_FS_32FS = 0,												/**< 32fs */
	E_DD_AUDIO_FS_48FS,													/**< 48fs */
	E_DD_AUDIO_FS_64FS,													/**< 64fs */
	E_DD_AUDIO_FS_128FS													/**< 128fs */
} E_DD_AUDIO_FS;

/** Audio Channel */
typedef enum {
	E_DD_AUDIO_CHANNEL_MONO		= 0,									/**< Mono */
	E_DD_AUDIO_CHANNEL_STEREO	= 1										/**< Stereo */
} E_DD_AUDIO_CHANNEL;

/** Data Shift */
typedef enum {
	E_DD_AUDIO_BIT_SHIFT_0	= 0,										/**< Left shift of 0 bit */
	E_DD_AUDIO_BIT_SHIFT_1,												/**< Left shift of 1 bit */
	E_DD_AUDIO_BIT_SHIFT_2,												/**< Left shift of 2 bit */
	E_DD_AUDIO_BIT_SHIFT_3												/**< Left shift of 3 bit */
} E_DD_AUDIO_BIT_SHIFT;


#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
typedef VOID (*T_DD_AUDIO_CALLBACK)( VOID );	/**< Callback function pointer	*/

/** Audio Control(Common) */
typedef struct {
	E_DD_AUDIO_FIFO_USAGE		fifo_usage;									/**< FIFO Usage<BR><BR>
																				 Default value : E_DD_AUDIO_FIFO_USAGE_STAGES_8 */
	E_DD_AUDIO_CHANNEL			channel;									/**< channel<BR><BR>
																				 Default value : E_DD_AUDIO_CHANNEL_MONO */
} T_DD_AUDIO_CTRL_COMMON;

/** Audio Control(Input) */
typedef struct {
	E_DD_AUDIO_DATA_REG_FRMT	format;										/**< Data Input Register Format<BR><BR>
																				 Default value : E_DD_AUDIO_DATA_REG_FRMT_R_JUST */
	E_DD_AUDIO_FIFO_STAGES		fifo_stages;								/**< input FIFO Stages of output register<BR><BR>
																				 Default value : E_DD_AUDIO_FIFO_STAGES_1 */
	E_DD_AUDIO_AHB_FRMT			ahb_format;									/**< Input Data AHB Bus Format<BR><BR>
																				 Default value : E_DD_AUDIO_AHB_FRMT_R_JUST */
	E_DD_AUDIO_BIT_SHIFT		bit_shift;									/**< Audio Input Data Shift<BR><BR>
																				 Default value : E_DD_AUDIO_BIT_SHIFT_0 */
} T_DD_AUDIO_CTRL_IN;

/** Audio Control(Output) */
typedef struct {
	E_DD_AUDIO_DATA_REG_FRMT	format;										/**< Data Output Register Format<BR><BR>
																				 Default value : E_DD_AUDIO_DATA_REG_FRMT_R_JUST */
	E_DD_AUDIO_FIFO_STAGES		fifo_stages;								/**< Output FIFO Stages of output register<BR><BR>
																				 Default value : E_DD_AUDIO_FIFO_STAGES_1 */
	E_DD_AUDIO_AHB_FRMT			ahb_format;									/**< Output Data AHB Bus Format<BR><BR>
																				 Default value : E_DD_AUDIO_AHB_FRMT_R_JUST */
	UINT8						lr_copy;									/**< Lch Data Rch Copy Enable <BR><BR>
																				 Value Range:D_DD_AUDIO_DISABLE / D_DD_AUDIO_ENABLE<BR>
																				 In the case of a audio interface slave mode, It is prohibited to set D_DD_AUDIO_ENABLE. <BR><BR>
																				 Default value : D_DD_AUDIO_DISABLE */
	UINT8						mix_play;									/**< Mix Output Enable <BR><BR>
																				 D_DD_AUDIO_ENABLE: The data mixed Lch / Rch is output.<BR>
																				 D_DD_AUDIO_DISABLE: The normal data is output.<BR>
																				 Value Range:D_DD_AUDIO_DISABLE / D_DD_AUDIO_ENABLE<BR>
																				 Default value : D_DD_AUDIO_DISABLE */
	E_DD_AUDIO_BIT_SHIFT		bit_shift;									/**< Audio Output Data Shift<BR><BR>
																				 Default value : E_DD_AUDIO_BIT_SHIFT_0 */
} T_DD_AUDIO_CTRL_OUT;

/** Audio I2S Control(Common) */
typedef struct {
	E_DD_AUDIO_MASTER_CLOCK		aumclki;									/**< Audio Master Clock(input)<BR><BR>
																				 Default value : @ref E_DD_AUDIO_MASTER_CLOCK_8_192 */
	E_DD_AUDIO_AUMCLKO_DIV_CLK	div_aumclko;								/**< AUMCLKO Output Clock Division Ratio<BR><BR>
																				 Default value : @ref E_DD_AUDIO_AUMCLKO_DIV_2 */
	E_DD_AUDIO_AUCLK_DIV_CLK	div_auclk;									/**< AUCLK Output Clock Division Ratio<BR><BR>
																				 Default value : E_DD_AUDIO_AUCLK_DIV_16 */
	E_DD_AUDIO_AULR_DIV_CLK		div_lrclk;									/**< AULR Output Clock Division Ratio<BR><BR>
																				 Default value : E_DD_AUDIO_AULR_DIV_32 */
	UINT8						clk_div_enable;								/**< Clock division enable <BR><BR>
																				 Value Range:D_DD_AUDIO_DISABLE / D_DD_AUDIO_ENABLE<BR>
																			     <ul><li>@ref D_DD_AUDIO_DISABLE
																				 <li>@ref D_DD_AUDIO_ENABLE</ul>	*/
	E_DD_AUDIO_CLK_MODE			master_slave;								/**< Master/Slave setting of clock<BR><BR>
																				 Default value : E_DD_AUDIO_CLK_SLAVE */
}  T_DD_AUDIO_I2S_CMMN;

/** Audio I2S Control(Input) */
typedef struct {
	E_DD_AUDIO_MODE				mode_in;									/**< Input Data Format<BR><BR>
																				 Default value : @ref E_DD_AUDIO_MODE_L_JUST */
	E_DD_AUDIO_BIT				bit_in;										/**< Valid Bit of Input Data<BR><BR>
																				 Default value : E_DD_AUDIO_BIT_8BIT */
} T_DD_AUDIO_I2S_IN;

/** Audio I2S Control(Input) */
typedef struct {
	E_DD_AUDIO_MODE				mode_out;									/**< Output Data Format<BR><BR>
																				 Default value : @ref E_DD_AUDIO_MODE_L_JUST */
	E_DD_AUDIO_BIT				bit_out;									/**< Valid Bit of Output Data<BR><BR>
																				 Default value : E_DD_AUDIO_BIT_8BIT */
	E_DD_AUDIO_FS				fs;											/**< AUCLK(sirial clock) vs AULRO(LR channel clock)<BR><BR>
																				 Default value : E_DD_AUDIO_FS_32FS */
} T_DD_AUDIO_I2S_OUT;

/** Audio DMA Control */
typedef struct {
	UINT8						dma_2ch;									/**< DMA 2ch Transfer Enable<BR><BR>
																				 Value Range:D_DD_AUDIO_DISABLE / D_DD_AUDIO_ENABLE<BR>
																				 Default value : D_DD_AUDIO_DISABLE */
	UINT16						dma_trnsf_cnt;								/**< DMA Transfer Sample Count<BR><BR>
																				 Value Range:[0x0010 - 0xFFFE]<BR>
																				 Default value : 0x0000 */
} T_DD_AUDIO_DMA;


#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#ifdef __cplusplus
extern "C" {
#endif


/* Common */
/**
Execute exclusive control for AudioI/F macro(Input)<br>
@param[in]	ch	channel<br>
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@param[in]	tmout	Time of timeout<br>
						<ul><li>Positive Value(Time of timeout)
							<li>@ref D_DDIM_USER_SEM_WAIT_POL
							<li>@ref D_DDIM_USER_SEM_WAIT_FEVR</ul>
@retval D_DDIM_OK					OK
@retval D_DD_AUDIO_INPUT_PARAM_ERR	Input Parameter Error
@retval D_DD_AUDIO_SEM_NG			Semaphore acquisition NG
@retval D_DD_AUDIO_SEM_TIMEOUT		Semaphore acquisition Time Out
@remarks This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
		 This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern	INT32	Dd_Audio_Open_Input( UINT8 ch, INT32 tmout );

/**
Execute exclusive control for AudioI/F macro(Output)<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@param[in]	tmout	Time of timeout<br>
						<ul><li>Positive Value(Time of timeout)
							<li>@ref D_DDIM_USER_SEM_WAIT_POL
							<li>@ref D_DDIM_USER_SEM_WAIT_FEVR</ul>
@retval D_DDIM_OK					OK
@retval D_DD_AUDIO_INPUT_PARAM_ERR	Input Parameter Error
@retval D_DD_AUDIO_SEM_NG			Semaphore acquisition NG
@retval D_DD_AUDIO_SEM_TIMEOUT		Semaphore acquisition Time Out
@remarks This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
		 This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern	INT32	Dd_Audio_Open_Output( UINT8 ch, INT32 tmout );

/**
Cancel exclusive control for AudioI/F macro(Input)<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@retval D_DDIM_OK					OK
@retval D_DD_AUDIO_SEM_NG			Semaphore release NG 
@remarks This API uses DDIM_User_Sig_Sem().
*/
extern	INT32	Dd_Audio_Close_Input( UINT8 ch );

/**
Cancel exclusive control for AudioI/F macro(Output)<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@retval D_DDIM_OK					OK
@retval D_DD_AUDIO_SEM_NG			Semaphore release NG 
@remarks This API uses DDIM_User_Sig_Sem().
*/
extern	INT32	Dd_Audio_Close_Output( UINT8 ch );

/**
Interrupt handler for AudioI/F macro<br><br>
Interrupt Cause<br>
- Input register full<br>
- Input FIFO stage sotred<br>
- Input register overflow<br>
- Output register empty<br>
- Output FIFO stage empty<br>
- Output register underflow<br><br>

@param[in]	ch	channel<br>
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
*/
extern	VOID	Dd_Audio_Int_Handler( UINT8 ch );

/**
Initialize AudioI/F macro<br>
Reset all channel of AudioI/F macro<br>
Start Clock(PLL10A)<br>
*/
extern	VOID	Dd_Audio_Init( VOID );

/**
Reset Audio macro<br>
@param[in]	ch	channel<br>
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_AUDIO_SYSTEM_ERROR			System Error
*/
extern INT32 Dd_Audio_Reset( UINT8 ch );

/**
Set audio data control information(Common Setting)<br>
@param[in]	ch	channel<br>
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@param[in]	ctrl_inf audio data control information. See @ref T_DD_AUDIO_CTRL_COMMON
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_AUDIO_SYSTEM_ERROR			System Error
*/
extern	INT32	Dd_Audio_Ctrl_Common( UINT8 ch, T_DD_AUDIO_CTRL_COMMON* ctrl_inf );

/**
Get audio data control information(Common Setting)<br>
@param[in]	ch	channel<br>
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@param[out]	ctrl_inf audio data control information. See @ref T_DD_AUDIO_CTRL_COMMON
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern INT32 Dd_Audio_Get_Ctrl_Common( UINT8 ch, T_DD_AUDIO_CTRL_COMMON* ctrl_inf );

/**
Set audio input data control information<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@param[in]	ctrl_inf audio data control information. See @ref T_DD_AUDIO_CTRL_IN
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_AUDIO_SYSTEM_ERROR			System Error
*/
extern	INT32	Dd_Audio_Ctrl_Input( UINT8 ch, T_DD_AUDIO_CTRL_IN* ctrl_inf );

/**
Get audio input data control information<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@param[out]	ctrl_inf audio data control information. See @ref T_DD_AUDIO_CTRL_IN
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32 Dd_Audio_Get_Ctrl_Input( UINT8 ch, T_DD_AUDIO_CTRL_IN* ctrl_inf );

/**
Set audio output data control information<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@param[in]	ctrl_inf audio data control information. See @ref T_DD_AUDIO_CTRL_OUT
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_AUDIO_SYSTEM_ERROR			System Error
*/
extern	INT32	Dd_Audio_Ctrl_Output( UINT8 ch, T_DD_AUDIO_CTRL_OUT* ctrl_inf );

/**
Get audio output data control information<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@param[out]	ctrl_inf audio data control information. See @ref T_DD_AUDIO_CTRL_OUT
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32 Dd_Audio_Get_Ctrl_Output( UINT8 ch, T_DD_AUDIO_CTRL_OUT* ctrl_inf );

/**
Set I2S control information(Common Clock Setting)<br>
@param[in]	ch	channel<br>
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@param[in]	i2s_setting	I2S control information. See @ref T_DD_AUDIO_I2S_CMMN
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_AUDIO_SYSTEM_ERROR			System Error
@remarks This API uses DDIM_User_Dly_Tsk().
*/
extern	INT32	Dd_Audio_Ctrl_I2sCmmn( UINT8 ch, T_DD_AUDIO_I2S_CMMN* i2s_setting );

/**
Get I2S control information(Common Clock Setting)<br>
@param[in]	ch	channel<br>
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@param[out]	i2s_setting	I2S control information. See @ref T_DD_AUDIO_I2S_CMMN
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32 Dd_Audio_Get_Ctrl_I2sCmmn( UINT8 ch, T_DD_AUDIO_I2S_CMMN* i2s_setting );

/**
Set I2S control information(Input Data Setting)<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@param[in]	i2s_setting	I2S control information. See @ref T_DD_AUDIO_I2S_IN
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_AUDIO_SYSTEM_ERROR			System Error
*/
extern	INT32	Dd_Audio_Ctrl_I2sIn( UINT8 ch, T_DD_AUDIO_I2S_IN* i2s_setting );

/**
Get I2S control information(Input Data Setting)<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@param[out]	i2s_setting	I2S control information. See @ref T_DD_AUDIO_I2S_IN
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32 Dd_Audio_Get_Ctrl_I2sIn( UINT8 ch, T_DD_AUDIO_I2S_IN* i2s_setting );

/**
Set I2S control information(Output Data Setting)<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@param[in]	i2s_setting	I2S control information. See @ref T_DD_AUDIO_I2S_OUT
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_AUDIO_SYSTEM_ERROR			System Error
*/
extern	INT32	Dd_Audio_Ctrl_I2sOut( UINT8 ch, T_DD_AUDIO_I2S_OUT* i2s_setting );

/**
Get I2S control information(Output Data Setting)<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@param[out]	i2s_setting	I2S control information. See @ref T_DD_AUDIO_I2S_OUT
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32 Dd_Audio_Get_Ctrl_I2sOut( UINT8 ch, T_DD_AUDIO_I2S_OUT* i2s_setting );

/**
Set DMA control information of input<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@param[in]	dma_setting	DMA control information. See @ref T_DD_AUDIO_DMA
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_AUDIO_SYSTEM_ERROR			System Error
*/
extern	INT32	Dd_Audio_Ctrl_DmaIn( UINT8 ch, T_DD_AUDIO_DMA* dma_setting );

/**
Get DMA control information of input<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@param[out]	dma_setting	DMA control information. See @ref T_DD_AUDIO_DMA
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32	Dd_Audio_Get_Ctrl_DmaIn( UINT8 ch, T_DD_AUDIO_DMA* dma_setting );

/**
Set DMA control information of output<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@param[in]	dma_setting	DMA control information. See @ref T_DD_AUDIO_DMA
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_AUDIO_SYSTEM_ERROR			System Error
*/
extern	INT32	Dd_Audio_Ctrl_DmaOut( UINT8 ch, T_DD_AUDIO_DMA* dma_setting );

/**
Set DMA control information of output<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@param[out]	dma_setting	DMA control information. See @ref T_DD_AUDIO_DMA
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_AUDIO_SYSTEM_ERROR			System Error
*/
extern	INT32	Dd_Audio_Get_Ctrl_DmaOut( UINT8 ch, T_DD_AUDIO_DMA* dma_setting );

/**
Start input operation<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5
							<li>@ref D_DD_AUDIO_IF_CH0_CH1</ul>
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_AUDIO_SYSTEM_ERROR			System Error
*/
extern	INT32	Dd_Audio_Start_Input( UINT8 ch );

/**
Stop input operation<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5
							<li>@ref D_DD_AUDIO_IF_CH0_CH1</ul>
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_AUDIO_SYSTEM_ERROR			System Error
@remarks This API uses DDIM_User_Dly_Tsk().
*/
extern	INT32	Dd_Audio_Stop_Input( UINT8 ch );

/**
Get status of input operation of audio interface<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@retval	TRUE:input operation is performed.
@retval	FALSE:input operation is not performed.
*/
extern	BOOL	Dd_Audio_Get_Status_Input( UINT8 ch );

/**
Start output operation<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3
							<li>@ref D_DD_AUDIO_IF_CH0_CH1</ul>
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_AUDIO_SYSTEM_ERROR			System Error
*/
extern	INT32	Dd_Audio_Start_Output( UINT8 ch );

/**
Stop output operation<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3
							<li>@ref D_DD_AUDIO_IF_CH0_CH1</ul>
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_AUDIO_SYSTEM_ERROR			System Error
@remarks This API uses DDIM_User_Dly_Tsk().
*/
extern	INT32	Dd_Audio_Stop_Output( UINT8 ch );

/**
Get status of output operation of audio interface<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@retval	TRUE:output operation is performed.
@retval	FALSE:output is fixed to "0".
*/
extern	BOOL	Dd_Audio_Get_Status_Output( UINT8 ch );

/**
Start loopback operation<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH0_CH1</ul>
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_AUDIO_SYSTEM_ERROR			System Error
*/
extern	INT32	Dd_Audio_Start_LoopBack( UINT8 ch );

/**
Stop loopBack operation<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH0_CH1</ul>
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
@retval D_DD_AUDIO_SYSTEM_ERROR			System Error
*/
extern	INT32	Dd_Audio_Stop_LoopBack( UINT8 ch );

/**
Get status of loopback operation<br>
@param[in]	ch channel[0 - 2]
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2</ul>
@retval	TRUE:loopback operation is performed.
@retval	FALSE:loopback operation is not performed.
*/
extern	BOOL	Dd_Audio_Get_Status_LoopBack( UCHAR ch );

/**
Set enable DMA start request when the input register is full<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@param[in]	enable	[1:enable DMA start request, 0:disable DMA start request]
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32	Dd_Audio_Set_EnableFifoFullDmaIn( UINT8 ch, UINT8 enable );

/**
Set enable DMA start request when the output register is empty<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@param[in]	enable	[1:enable DMA start request, 0:disable DMA start request]
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32	Dd_Audio_Set_EnableFifoEmptyDmaOut( UINT8 ch, UINT8 enable );

/**
Set enable DMA start request(input operation) according to FIFO Usage(See @ref T_DD_AUDIO_CTRL_COMMON)<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@param[in]	enable	[1:enable DMA start request, 0:disable DMA start request]
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32	Dd_Audio_Set_InputDMARequestEnable( UINT8 ch, UINT8 enable );

/**
Set enable DMA start request(output operation) according to FIFO Usage(See @ref T_DD_AUDIO_CTRL_COMMON)<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@param[in]	enable	[1:enable DMA start request, 0:disable DMA start request]
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32	Dd_Audio_Set_OutputDMARequestEnable( UINT8 ch, UINT8 enable );

/**
Get input data DMA transfer sample count<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@retval sample count
*/
extern	UINT16	Dd_Audio_Get_InDmaSample( UINT8 ch );

/**
Get output data DMA transfer sample count<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@retval sample count
*/
extern	UINT16	Dd_Audio_Get_OutDmaSample( UINT8 ch );

/**
Replace the write value to the input register(AUIDLR) by 1Byte unit<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@param[in]	enable  [1:Enable replacement, 0:Disable replacement]
@retval D_DDIM_OK					OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32	Dd_Audio_Set_SwapByte_AUIDLR( UINT8 ch, UINT8 enable );

/**
Replace the write value to the input register(AUIDLR) by 2Byte unit<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@param[in]	enable  [1:Enable replacement, 0:Disable replacement]
@retval D_DDIM_OK					OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32	Dd_Audio_Set_SwapHW_AUIDLR( UINT8 ch, UINT8 enable );

/**
Replace the write value to the input register(AUODLR) by 1Byte unit<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@param[in]	enable  [1:Enable replacement, 0:Disable replacement]
@retval D_DDIM_OK					OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32	Dd_Audio_Set_SwapByte_AUODLR( UINT8 ch, UINT8 enable );

/**
Replace the write value to the input register(AUODLR) by 2Byte unit<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@param[in]	enable  [1:Enable replacement, 0:Disable replacement]
@retval D_DDIM_OK					OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32	Dd_Audio_Set_SwapHW_AUODLR( UINT8 ch, UINT8 enable );

/**
Get address of register AUIDLR<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@retval Adress of register AUIDLR
*/
extern	UINT32	Dd_Audio_Get_Addr_Reg_AUIDLR( UINT8 ch );

/**
Get address of register AUODLR<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@retval Adress of register AUODLR
*/
extern	UINT32	Dd_Audio_Get_Addr_Reg_AUODLR( UINT8 ch );

/**
Get address of register AUIDL<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@retval Adress of register AUIDL
*/
extern	UINT32	Dd_Audio_Get_Addr_Reg_AUIDL( UINT8 ch );

/**
Get address of register AUIDR<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@retval Adress of register AUIDR
*/
extern	UINT32	Dd_Audio_Get_Addr_Reg_AUIDR( UINT8 ch );

/**
Get address of register AUODL<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@retval Adress of register AUODL
*/
extern	UINT32	Dd_Audio_Get_Addr_Reg_AUODL( UCHAR ch );

/**
Get address of register AUODR<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@retval Adress of register AUODR
*/
extern	UINT32	Dd_Audio_Get_Addr_Reg_AUODR( UINT8 ch );

/**
Get address of register AUIDDMAPT<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@retval Adress of register AUIDDMAPT
*/
extern	UINT32	Dd_Audio_Get_Addr_Reg_AUIDDMAPT( UINT8 ch );

/**
Get address of register AUODDMAPT<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@retval Adress of register AUODDMAPT
*/
extern	UINT32	Dd_Audio_Get_Addr_Reg_AUODDMAPT( UINT8 ch );

/**
Set enable the interrupt request caused by input register full<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@param[in]	enable	[1:enable interrupt request, 0:disable interrupt request]
@param[in]	callback	Callback function pointer
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32 Dd_Audio_Set_EnableInputIntr( UINT8 ch, UINT8 enable, T_DD_AUDIO_CALLBACK callback );

/**
Set enable the interrupt request caused by output register empty<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@param[in]	enable	[1:enable interrupt request, 0:disable interrupt request]
@param[in]	callback	Callback function pointer
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32 Dd_Audio_Set_EnableOutputIntr( UINT8 ch, UINT8 enable, T_DD_AUDIO_CALLBACK callback );

/**
Set enable the interrupt request caused by input register full<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@param[in]	enable	[1:enable interrupt request, 0:disable interrupt request]
@param[in]	callback	Callback function pointer
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32 Dd_Audio_Set_EnableInputIntr( UINT8 ch, UINT8 enable, T_DD_AUDIO_CALLBACK callback );

/**
Set enable the interrupt request caused by output register empty<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@param[in]	enable	[1:enable interrupt request, 0:disable interrupt request]
@param[in]	callback	Callback function pointer
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32 Dd_Audio_Set_EnableOutputIntr( UINT8 ch, UINT8 enable, T_DD_AUDIO_CALLBACK callback );

/**
Set enable the interrupt request caused by data input register overflow<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@param[in]	enable	[1:enable interrupt request, 0:disable interrupt request]
@param[in]	callback	Callback function pointer
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32	Dd_Audio_Set_EnableInOverFlowIntr( UINT8 ch, UINT8 enable, T_DD_AUDIO_CALLBACK callback );

/**
Set enable the interrupt request caused by data output register underflow<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@param[in]	enable	[1:enable interrupt request, 0:disable interrupt request]
@param[in]	callback	Callback function pointer
@retval D_DDIM_OK						OK
@retval D_DD_AUDIO_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32	Dd_Audio_Set_EnableOutUnderFlowIntr( UINT8 ch, UINT8 enable, T_DD_AUDIO_CALLBACK callback );

/**
Get the operation status of FIFO stages 15-0 of input register<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH4
							<li>@ref D_DD_AUDIO_IF_CH5</ul>
@retval Value of register AUIFST
*/
extern	UINT32	Dd_Audio_Get_InputFifoStatus( UINT8 ch );

/**
Get the operation status of FIFO stages 15-0 of output register<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_AUDIO_IF_CH0
							<li>@ref D_DD_AUDIO_IF_CH1
							<li>@ref D_DD_AUDIO_IF_CH2
							<li>@ref D_DD_AUDIO_IF_CH3</ul>
@retval Value of register AUOFST
*/
extern	UINT32 Dd_Audio_Get_OutputFifoStatus( UINT8 ch );

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

#ifdef __cplusplus
}
#endif

#endif	// _DD_AUDIO_H_

/*	@}*/
/*@}*/

/************************************************************************/
/* Sample Code															*/
/************************************************************************/
/** @weakgroup dd_audio
@{
	<hr>
	@section Audio_Input_Sample		Sample of Audio Input Operation
	@image html dd_audio_input_1.png
	<br>
	The example of Audio Input Operation is as follows.
	@code
	
	T_DD_AUDIO_I2S_CMMN i2s_common;
	T_DD_AUDIO_I2S_IN i2s_in;
	T_DD_AUDIO_CTRL_COMMON ctrl_common;
	T_DD_AUDIO_CTRL_IN ctrl_in;
	T_DD_AUDIO_DMA dma_in;
	
	// gets an exclusivity
	Dd_Audio_Open_Input(D_DD_AUDIO_IF_CH0, D_DDIM_USER_SEM_WAIT_FEVR);
	
	// Reset
	Dd_Audio_Reset(D_DD_AUDIO_IF_CH0)
	
	// I2S Setting
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko = E_DD_AUDIO_AUMCLKO_DIV_1;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_8;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(D_DD_AUDIO_IF_CH0, &i2s_common);
	
	i2s_in.mode_in = E_DD_AUDIO_MODE_L_JUST;
	i2s_in.bit_in = E_DD_AUDIO_BIT_16BIT;
	
	Dd_Audio_Ctrl_I2sIn(D_DD_AUDIO_IF_CH0, &i2s_in);
	
	// Input Data Setting
	ctrl_common.fifo_usage = E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel = E_DD_AUDIO_CHANNEL_MONO;
	
	Dd_Audio_Ctrl_Common(D_DD_AUDIO_IF_CH0, &ctrl_common);
	
	ctrl_in.format = E_DD_AUDIO_DATA_REG_FRMT_R_JUST;
	ctrl_in.fifo_stages = E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_in.ahb_format = E_DD_AUDIO_AHB_FRMT_R_JUST;
	ctrl_in.bit_shift = E_DD_AUDIO_BIT_SHIFT_0;
	
	Dd_Audio_Ctrl_Input(D_DD_AUDIO_IF_CH0, &ctrl_in);
	
	dma_in.dma_2ch = D_DD_AUDIO_DISABLE;
	dma_in.dma_trnsf_cnt = 0;
	
	Dd_Audio_Ctrl_DmaIn(D_DD_AUDIO_IF_CH0, &dma_in);
	
	// DMA Trasfer Setting(Refer to HDMAC Macro Function)
	Dd_HDMAC0_Open(0, SEM_WAIT_TIME);
	// Audio data are transferred to memory(0xA0000000).
	Dd_HDMAC0_Input_Audio_Async(0, D_DD_HDMAC0_IS_IDREQ_0, Dd_Audio_Get_Addr_Reg_AUIDLR(D_DD_AUDIO_IF_CH0), 0xA0000000, 2048, NULL);
	
	// DMA Trasfer Enable
	Dd_Audio_Set_InputDMARequestEnable(D_DD_AUDIO_IF_CH0, D_DD_AUDIO_ENABLE);
	
	// runs audio macro
	Dd_Audio_Start_Input(D_DD_AUDIO_IF_CH0);
	
	   :
	   :
	
	// stops audio macro
	Dd_Audio_Stop_Input(D_DD_AUDIO_IF_CH0);
	
	Dd_Audio_Set_InputDMARequestEnable(D_DD_AUDIO_IF_CH0, D_DD_AUDIO_DISABLE);
	
	// DMA Trasfer Setting(Refer to HDMAC Macro Function)
	Dd_HDMAC0_Stop(0);
	Dd_HDMAC0_Close(0);
	
	// releases an exclusivity
	Dd_Audio_Close_Input(D_DD_AUDIO_IF_CH0);
	
	@endcode

	@section Audio_Output_Sample		Sample of Audio Output Operation
	@image html dd_audio_output_1.png
	<br>
	The example of Audio Output Operation is as follows.
	@code

	T_DD_AUDIO_I2S_CMMN i2s_common;
	T_DD_AUDIO_I2S_OUT i2s_out;
	T_DD_AUDIO_CTRL_COMMON ctrl_common;
	T_DD_AUDIO_CTRL_OUT ctrl_out;
	T_DD_AUDIO_DMA dma_out;

	// gets an exclusivity
	Dd_Audio_Open_Output(D_DD_AUDIO_IF_CH0, D_DDIM_USER_SEM_WAIT_FEVR);

	// Reset
	Dd_Audio_Reset(D_DD_AUDIO_IF_CH0)

	// I2S Setting
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko = E_DD_AUDIO_AUMCLKO_DIV_1;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_8;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;

	Dd_Audio_Ctrl_I2sCmmn(D_DD_AUDIO_IF_CH0, &i2s_common);

	i2s_out.mode_out = E_DD_AUDIO_MODE_L_JUST;
	i2s_out.bit_out = E_DD_AUDIO_BIT_16BIT;
	i2s_out.fs = E_DD_AUDIO_FS_64FS;

	Dd_Audio_Ctrl_I2sOut(D_DD_AUDIO_IF_CH0, &i2s_out);

	// Output Data Setting
	ctrl_common.fifo_usage = E_DD_AUDIO_FIFO_USAGE_STAGES_1;
	ctrl_common.channel = E_DD_AUDIO_CHANNEL_MONO;
	
	Dd_Audio_Ctrl_Common(D_DD_AUDIO_IF_CH0, &ctrl_common);

	ctrl_out.format = E_DD_AUDIO_DATA_REG_FRMT_R_JUST;
	ctrl_out.fifo_stages = E_DD_AUDIO_FIFO_STAGES_16;
	ctrl_out.ahb_format = E_DD_AUDIO_AHB_FRMT_R_JUST;
	ctrl_out.lr_copy = D_DD_AUDIO_DISABLE;
	ctrl_out.mix_play = D_DD_AUDIO_DISABLE;
	ctrl_out.bit_shift = E_DD_AUDIO_BIT_SHIFT_0;

	Dd_Audio_Ctrl_Output(D_DD_AUDIO_IF_CH0, &ctrl_out);

	dma_out.dma_2ch = D_DD_AUDIO_DISABLE;
	dma_out.dma_trnsf_cnt = 0;

	Dd_Audio_Ctrl_DmaOut(D_DD_AUDIO_IF_CH0, &dma_out);

	// DMA Trasfer Setting(Refer to HDMAC Macro Function)
	Dd_HDMAC0_Open(0, SEM_WAIT_TIME);
	// Audio data are transferred from memory(0xA0000000).
	Dd_HDMAC0_Output_Audio_Async(0, D_DD_HDMAC0_IS_IDREQ_1, 0xA0000000, Dd_Audio_Get_Addr_Reg_AUODLR(D_DD_AUDIO_IF_CH0), 2048, NULL);
	
	// DMA Trasfer Enable
	Dd_Audio_Set_OutputDMARequestEnable(D_DD_AUDIO_IF_CH0, D_DD_AUDIO_ENABLE);
	// runs audio macro
	Dd_Audio_Start_Output(D_DD_AUDIO_IF_CH0);
	
	   :
	   :
	
	Dd_Audio_Set_OutputDMARequestEnable(D_DD_AUDIO_IF_CH0, D_DD_AUDIO_DISABLE);

	// DMA Trasfer Setting(Refer to HDMAC Macro Function)
	Dd_HDMAC0_Stop(0);
	Dd_HDMAC0_Close(0);
	
	// stops audio macro
	Dd_Audio_Stop_Output(D_DD_AUDIO_IF_CH0);

	// releases an exclusivity
	Dd_Audio_Close_Output(D_DD_AUDIO_IF_CH0);

	@endcode
*/
/*@}*/

