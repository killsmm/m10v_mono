/**
 * @file		dd_pdm.h
 * @brief		Header file of PDM driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/**
@addtogroup lsi_peri
@{
	@addtogroup dd_pdm
	@{
		- @ref PDM_I2S_Slave_Interface
		- @ref PDM_AHB
*/

#ifndef _DD_PDM_H_
#define _DD_PDM_H_

#include "driver_common.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
/* Return Value */
#define D_DD_PDM_INPUT_PARAM_ERROR		(D_DD_PDM | D_DDIM_INPUT_PARAM_ERROR)	/**< Input Parameter Error */
#define D_DD_PDM_MACRO_BUSY_NG			(D_DD_PDM | D_DDIM_MACRO_BUSY_NG)		/**< Macro busy */
#define D_DD_PDM_SEM_TIMEOUT			(D_DD_PDM | D_DDIM_SEM_TIMEOUT)			/**< Semaphore acquisition Time Out */
#define D_DD_PDM_SEM_NG					(D_DD_PDM | D_DDIM_SEM_NG)				/**< Semaphore acquisition NG */
#define D_DD_PDM_SYSTEM_ERROR			(D_DD_PDM | D_DDIM_SYSTEM_ERROR)		/**< System error */

#define D_DD_PDM_CH0				(0x00)										/**< PDM Channel 0  */
#define D_DD_PDM_CH1				(0x01)										/**< PDM Channel 1 */
#define D_DD_PDM_CH_NUM_MAX			(0x02)										/**< Channel Max Number */

/* Function Enable */
#define	D_DD_PDM_DISABLE			(0)											/**< Disable	*/
#define	D_DD_PDM_ENABLE				(1)											/**< Enable		*/

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------

/** LR Channel Swap Setting */
typedef enum {
	E_DD_PDM_CORE_LR_SWAP_NO_SWAP	= 0,							/**< No Swap */
	E_DD_PDM_CORE_LR_SWAP_SWAP										/**< Right/Left Channel Recording Swap */
} E_DD_PDM_CORE_LR_SWAP;

/** PGA Gain */
typedef enum {
	E_DD_PDM_CORE_PGA_0_0_DB		= 0,							/**< PGA Gain:  0.0[dB] */
	E_DD_PDM_CORE_PGA_P_1_5_DB,										/**< PGA Gain: +1.5[dB] */
	E_DD_PDM_CORE_PGA_P_3_0_DB,										/**< PGA Gain: +3.0[dB] */
	E_DD_PDM_CORE_PGA_P_4_5_DB,										/**< PGA Gain: +4.5[dB] */
	E_DD_PDM_CORE_PGA_P_6_0_DB,										/**< PGA Gain: +6.0[dB] */
	E_DD_PDM_CORE_PGA_P_7_5_DB,										/**< PGA Gain: +7.5[dB] */
	E_DD_PDM_CORE_PGA_P_9_0_DB,										/**< PGA Gain: +9.0[dB] */
	E_DD_PDM_CORE_PGA_P_10_5_DB,									/**< PGA Gain: +10.5[dB] */
	E_DD_PDM_CORE_PGA_M_12_0_DB,									/**< PGA Gain: -12.0[dB] */
	E_DD_PDM_CORE_PGA_M_10_5_DB,									/**< PGA Gain: -10.5[dB] */
	E_DD_PDM_CORE_PGA_M_9_0_DB,										/**< PGA Gain: -9.0[dB] */
	E_DD_PDM_CORE_PGA_M_7_5_DB,										/**< PGA Gain: -7.5[dB] */
	E_DD_PDM_CORE_PGA_M_6_0_DB,										/**< PGA Gain: -6.0[dB] */
	E_DD_PDM_CORE_PGA_M_4_5_DB,										/**< PGA Gain: -4.5[dB] */
	E_DD_PDM_CORE_PGA_M_3_0_DB,										/**< PGA Gain: -3.0[dB] */
	E_DD_PDM_CORE_PGA_M_1_5_DB,										/**< PGA Gain: -1.5[dB] */
} E_DD_PDM_CORE_PGA;

/** Setting Cycle(during gain setting / soft mute) */
typedef enum {
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	E_DD_PDM_CORE_S_CYCLE_167US		= 0,							/**< Setting Cycle:  167[us] */
	E_DD_PDM_CORE_S_CYCLE_333US,									/**< Setting Cycle:  333[us] */
	E_DD_PDM_CORE_S_CYCLE_500US,									/**< Setting Cycle:  500[us] */
	E_DD_PDM_CORE_S_CYCLE_667US,									/**< Setting Cycle:  667[us] */
	E_DD_PDM_CORE_S_CYCLE_833US,									/**< Setting Cycle:  833[us] */
	E_DD_PDM_CORE_S_CYCLE_1000US,									/**< Setting Cycle:  1000[us] */
	E_DD_PDM_CORE_S_CYCLE_1167US,									/**< Setting Cycle:  1167[us] */
	E_DD_PDM_CORE_S_CYCLE_1333US,									/**< Setting Cycle:  1333[us] */
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	E_DD_PDM_CORE_S_CYCLE_0		= 0,								/**< Setting Cycle:  FS=48kHz/32kHz:167[us], FS=24kHz/16kHz:333[us] */
	E_DD_PDM_CORE_S_CYCLE_1,										/**< Setting Cycle:  FS=48kHz/32kHz:333[us], FS=24kHz/16kHz:667[us] */
	E_DD_PDM_CORE_S_CYCLE_2,										/**< Setting Cycle:  FS=48kHz/32kHz:500[us], FS=24kHz/16kHz:1000[us] */
	E_DD_PDM_CORE_S_CYCLE_3,										/**< Setting Cycle:  FS=48kHz/32kHz:667[us], FS=24kHz/16kHz:1333[us] */
	E_DD_PDM_CORE_S_CYCLE_4,										/**< Setting Cycle:  FS=48kHz/32kHz:833[us], FS=24kHz/16kHz:1667[us] */
	E_DD_PDM_CORE_S_CYCLE_5,										/**< Setting Cycle:  FS=48kHz/32kHz:1000[us], FS=24kHz/16kHz:2000[us] */
	E_DD_PDM_CORE_S_CYCLE_6,										/**< Setting Cycle:  FS=48kHz/32kHz:1167[us], FS=24kHz/16kHz:2333[us] */
	E_DD_PDM_CORE_S_CYCLE_7,										/**< Setting Cycle:  FS=48kHz/32kHz:1333[us], FS=24kHz/16kHz:2667[us] */
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
} E_DD_PDM_CORE_S_CYCLE;

/** DMA Burst Length Setting */
typedef enum {
	E_DD_PDM_DMA_BURST_LEN_1	= 0,								/**< DMA burst length: 1 word */
	E_DD_PDM_DMA_BURST_LEN_4,										/**< DMA burst length: 4 word */
	E_DD_PDM_DMA_BURST_LEN_8,										/**< DMA burst length: 8 word */
	E_DD_PDM_DMA_BURST_LEN_16,										/**< DMA burst length: 16 word */
} E_DD_PDM_DMA_BURST_LEN;

/** PCM Output Channel Setting(DMA Transfer Mode) */
typedef enum {
	E_DD_PDM_DMA_PCM_CH_DISABLE	= 0,								/**< PCM Channel: Disable */
	E_DD_PDM_DMA_PCM_CH_LEFT,										/**< PCM Channel: Mono(Left Channel) */
	E_DD_PDM_DMA_PCM_CH_RIGHT,										/**< PCM Channel: Mono(Right Channel) */
	E_DD_PDM_DMA_PCM_CH_STEREO,										/**< PCM Channel: Stereo */
} E_DD_PDM_DMA_PCM_CH;


/** PCM Data Word Length Setting(DMA Transfer Mode) */
typedef enum {
	E_DD_PDM_DMA_PCM_WD_16	= 0,									/**< PCM Data Word Length: 16bit */
	E_DD_PDM_DMA_PCM_WD_18,											/**< PCM Data Word Length: 18bit */
	E_DD_PDM_DMA_PCM_WD_20,											/**< PCM Data Word Length: 20bit */
	E_DD_PDM_DMA_PCM_WD_24,											/**< PCM Data Word Length: 24bit */
} E_DD_PDM_DMA_PCM_WD;

/** Type of Interrupt */
typedef enum {
	E_DD_PDM_INT_TYPE_FFOVF	= 0,									/**< FIFO OverFlow Interrupt*/
	E_DD_PDM_INT_TYPE_DMA0,											/**< DMA0 Interrupt */
	E_DD_PDM_INT_TYPE_DMA1,											/**< DMA1 Interrupt */
} E_DD_PDM_INT_TYPE;


#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
typedef VOID (*T_DD_PDM_CALLBACK)( VOID );								/**< Callback function pointer	*/

/** PDM Core Config */
typedef struct {
	E_DD_PDM_CORE_LR_SWAP		swap;									/**< Right/Left Channel Swap Setting<BR><BR>
																			 Default value : E_DD_PDM_CORE_LR_SWAP_NO_SWAP */
	E_DD_PDM_CORE_PGA			pga_r;									/**< Right Channel PGA Gain<BR><BR>
																			 Default value : E_DD_PDM_CORE_PGA_0_0_DB */
	E_DD_PDM_CORE_PGA			pga_l;									/**< Left Channel PGA Gain<BR><BR>
																			 Default value : E_DD_PDM_CORE_PGA_0_0_DB */
	UINT16						mclk_div;								/**< PDM_CLK Frequency divisor<BR><BR>
																			 value Range: [0x2-0x3F]<BR>
																			 Default value : 0x4 */
	UINT16						sinc_rate;								/**< SINC Decimation Rate<BR><BR>
																			 value Range: [0x1-0x7F]<BR>
																			 Default value : 0xd10 */
	UINT8						adc_hpd;								/**< High Pass Filter<BR><BR>
																			 value Range: D_DD_PDM_ENABLE / D_DD_PDM_DISABLE<BR>
																			 Default value : D_DD_PDM_DISABLE */
	UINT8						hp_cutoff;								/**< High Pass Filter Coefficients<BR><BR>
																			 Default value : 0xB */
	E_DD_PDM_CORE_S_CYCLE		s_cycle;								/**< Set number of PDM_CLK during gain-setting changes or soft mute<BR><BR>
// --- REMOVE_ES1_HARDWARE BEGIN ---
																			 Default value : E_DD_PDM_CORE_S_CYCLE_333US
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
																			 Default value : E_DD_PDM_CORE_S_CYCLE_1
// --- REMOVE_ES3_HARDWARE END ---
																			  */
	UINT8						soft_mute;								/**< Soft Mute<BR><BR>
																			 value Range: D_DD_PDM_ENABLE / D_DD_PDM_DISABLE<BR>
																			 Default value : D_DD_PDM_DISABLE */
} T_DD_PDM_CORE_CFG;

/** PDM DMAC Config */
typedef struct {
	UINT8						dmick_dly;								/**< Input Data Sampling with number of PDM clock cycle delay<BR><BR>
																			 value Range: [0-3]<BR>
																			 Default value : 0x0 */
	E_DD_PDM_DMA_BURST_LEN		dma_burstlen;							/**< DMA Burst Length<BR><BR>
																			 Default value : E_DD_PDM_DMA_BURST_LEN_4 */
	E_DD_PDM_DMA_PCM_CH			pcm_chset;								/**< Set PCM Output Channels as Mono or Stereo<BR><BR>
																			 Default value : E_DD_PDM_DMA_PCM_CH_LEFT */
	E_DD_PDM_DMA_PCM_WD			pcm_wdlen;								/**< PCM Data Word Length<BR><BR>
																			 Default value : E_DD_PDM_DMA_PCM_WD_16 */
	UINT8						dma_en;									/**< DMA Transfer<BR><BR>
																			 value Range: D_DD_PDM_ENABLE / D_DD_PDM_DISABLE<BR>
																			 Default value : D_DD_PDM_DISABLE */
} T_DD_PDM_DMA_CFG;

/** PDM DMA Transfer Length */
typedef struct {
	UINT16						ttsize;								/**< DMA total fransfer length in word<BR><BR>
																			 value Range: [0x0001-0xFFFF]<BR>
																			 Default value : 0x78 */
	UINT8						tsize;								/**< DMA block transfer length in word<BR><BR>
																			 value Range: [0x01-0xFF]<BR>
																			 Default value : 0x18 */
} T_DD_PDM_DMA_LEN;



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

/**
Initialize PDM macro<br>
@retval D_DDIM_OK					OK
*@retval D_DD_PDM_SYSTEM_ERROR		System Error
*/
extern	INT32	Dd_Pdm_Init( VOID );

/**
Execute exclusive control for PDM macro<br>
@param[in]	ch	channel<br>
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@param[in]	tmout	Time of timeout<br>
						<ul><li>Positive Value(Time of timeout)
							<li>@ref D_DDIM_USER_SEM_WAIT_POL
							<li>@ref D_DDIM_USER_SEM_WAIT_FEVR</ul>
@retval D_DDIM_OK					OK
@retval D_DD_PDM_INPUT_PARAM_ERR	Input Parameter Error
@retval D_DD_PDM_SEM_NG				Semaphore acquisition NG
@retval D_DD_PDM_SEM_TIMEOUT		Semaphore acquisition Time Out
@remarks This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
		 This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern	INT32	Dd_Pdm_Open( UINT8 ch, INT32 tmout );

/**
Cancel exclusive control for PDM macro<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@retval D_DDIM_OK				OK
@retval D_DD_PDM_SEM_NG			Semaphore release NG 
@remarks This API uses DDIM_User_Sig_Sem().
*/
extern	INT32	Dd_Pdm_Close( UINT8 ch );

/**
DMA0 Interrupt handler for PDM macro<br>
@param[in]	ch	channel<br>
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
*/
extern	VOID	Dd_Pdm_Dma0_Int_Handler( UINT8 ch );

/**
DMA1 Interrupt handler for PDM macro<br>
@param[in]	ch	channel<br>
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
*/
extern	VOID	Dd_Pdm_Dma1_Int_Handler( UINT8 ch );

/**
OverFlow Interrupt handler for PDM macro<br>
@param[in]	ch	channel<br>
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
*/
extern	VOID	Dd_Pdm_OverFlow_Int_Handler( UINT8 ch );

/**
Set pdm core control information<br>
@param[in]	ch	channel<br>
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@param[in]	core_cfg	pcm core control information. See @ref T_DD_PDM_CORE_CFG
@retval D_DDIM_OK						OK
@retval D_DD_PDM_INPUT_PARAM_ERROR		Input Parameter Error
*/
extern	INT32	Dd_Pdm_Ctrl_Core( UINT8 ch, T_DD_PDM_CORE_CFG* core_cfg );

/**
Get pcm core control information(Common Setting)<br>
@param[in]	ch	channel<br>
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@param[out]	core_cfg	pcm core control information. See @ref T_DD_PDM_CORE_CFG
@retval D_DDIM_OK						OK
@retval D_DD_PDM_INPUT_PARAM_ERROR		Input Parameter Error
*/
extern INT32 Dd_Pdm_Get_Ctrl_Core( UINT8 ch, T_DD_PDM_CORE_CFG* core_cfg );

/**
Start Data Streaming<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@retval D_DDIM_OK						OK
@retval D_DD_PDM_INPUT_PARAM_ERROR		Input Parameter Error
*/
extern	INT32	Dd_Pdm_Start_Streaming( UINT8 ch );

/**
Stop Data Streaming<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@retval D_DDIM_OK						OK
@retval D_DD_PDM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32	Dd_Pdm_Stop_Streaming( UINT8 ch );

/**
Set pdm dma control information<br>
@param[in]	ch	channel<br>
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@param[in]	dma_cfg		pcm dma control information. See @ref T_DD_PDM_DMA_CFG
@retval D_DDIM_OK						OK
@retval D_DD_PDM_INPUT_PARAM_ERROR		Input Parameter Error
*/
extern	INT32	Dd_Pdm_Ctrl_Dma( UINT8 ch, T_DD_PDM_DMA_CFG* dma_cfg );

/**
Get pcm core control information(Common Setting)<br>
@param[in]	ch	channel<br>
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@param[out]	dma_cfg		pcm dma control information. See @ref T_DD_PDM_DMA_CFG
@retval D_DDIM_OK						OK
@retval D_DD_PDM_INPUT_PARAM_ERROR		Input Parameter Error
*/
extern INT32 Dd_Pdm_Get_Ctrl_Dma( UINT8 ch, T_DD_PDM_DMA_CFG* dma_cfg );

/**
Set callback function of Dma0 Interrupt<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@param[in]	callback	Callback function pointer
@retval D_DDIM_OK					OK
@retval D_DD_PDM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32 Dd_Pdm_Set_CallbackDma0Intr( UINT8 ch, T_DD_PDM_CALLBACK callback );

/**
Set enable Dma0 Interrupt<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@param[in]	enable	[1:enable interrupt request, 0:disable interrupt request]
@retval D_DDIM_OK					OK
@retval D_DD_PDM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32 Dd_Pdm_Set_EnableDma0Intr( UINT8 ch, UINT8 enable );

/**
Set callback function of Dma1 Interrupt<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@param[in]	callback	Callback function pointer
@retval D_DDIM_OK					OK
@retval D_DD_PDM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32 Dd_Pdm_Set_CallbackDma1Intr( UINT8 ch, T_DD_PDM_CALLBACK callback );

/**
Set callback function of Dma1 Interrupt<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@param[in]	enable	[1:enable interrupt request, 0:disable interrupt request]
@retval D_DDIM_OK					OK
@retval D_DD_PDM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32 Dd_Pdm_Set_EnableDma1Intr( UINT8 ch, UINT8 enable );

/**
Set callback function of OverFlow Interrupt<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@param[in]	callback	Callback function pointer
@retval D_DDIM_OK					OK
@retval D_DD_PDM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32 Dd_Pdm_Set_CallbackOverFlowIntr( UINT8 ch, T_DD_PDM_CALLBACK callback );

/**
Set callback function of OverFlow Interrupt<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@param[in]	enable	[1:enable interrupt request, 0:disable interrupt request]
@retval D_DDIM_OK					OK
@retval D_DD_PDM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32 Dd_Pdm_Set_EnableOverFlowIntr( UINT8 ch, UINT8 enable );

/**
Set DMA0 distination address<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@param[in]	addr	distination address(4byte unit)
@retval D_DDIM_OK					OK
@retval D_DD_PDM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32	Dd_Pdm_Set_DMA0_Dst_Addr( UINT8 ch, UINT32 addr );


/**
Set DMA1 distination address<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@param[in]	addr	distination address(4byte unit)
@retval D_DDIM_OK					OK
@retval D_DD_PDM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32	Dd_Pdm_Set_DMA1_Dst_Addr( UINT8 ch, UINT32 addr );


/**
Set DMA Transfer Size<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@param[in]	dma_len		dma transfer length information. See @ref T_DD_PDM_DMA_LEN
@retval D_DDIM_OK					OK
@retval D_DD_PDM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32 Dd_Pdm_Set_Dma_TransLength( UINT8 ch, T_DD_PDM_DMA_LEN* dma_len );

/**
DMA FIFO Flush<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@retval D_DDIM_OK						OK
@retval D_DD_PDM_INPUT_PARAM_ERROR		Input Parameter Error
*/
extern	INT32	Dd_Pdm_Flush_Dma_Fifo( UINT8 ch );

/**
Get log of interrupt<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@param[in]	type		type of interrupt. See @ref E_DD_PDM_INT_TYPE
@param[out]	status		status of interrupt.[1:Interrupt done / 0: Interrupt not done]
@retval D_DDIM_OK					OK
@retval D_DD_PDM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32 Dd_Pdm_Get_Status( UINT8 ch, E_DD_PDM_INT_TYPE type, UINT8* status );

/**
Clear log of interrupt<br>
@param[in]	ch	channel
						<ul><li>@ref D_DD_PDM_CH0
							<li>@ref D_DD_PDM_CH1</ul>
@param[in]	type		type of interrupt. See @ref E_DD_PDM_INT_TYPE
@retval D_DDIM_OK					OK
@retval D_DD_PDM_INPUT_PARAM_ERROR	Input Parameter Error
*/
extern	INT32 Dd_Pdm_Clear_Status( UINT8 ch, E_DD_PDM_INT_TYPE type);


#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
//---------------------- colabo  section -------------------------------
#endif

#ifdef __cplusplus
}
#endif

#endif	// _DD_PDM_H_

/*	@}*/
/*@}*/

/************************************************************************/
/* Sample Code															*/
/************************************************************************/
/** @weakgroup dd_pdm
@{
	<hr>
	@section PDM_I2S_Slave_Interface	Sample of I2S Slave Interface Operation
	@image html dd_pdm_i2s.png
	<br>
	The example of I2S Slave Interface Operation is as follows.
	@code
	
	T_DD_PDM_CORE_CFG core_cfg;
	T_DD_AUDIO_I2S_CMMN i2s_common;
	
	// Initialize PDM Macro
	Dd_PDM_Init();
	
	// gets an exclusivity
	Dd_Pdm_Open(D_DD_PDM_CH0, D_DDIM_USER_SEM_WAIT_FEVR);
	
	// Core Setting
	core_cfg.swap = E_DD_PDM_CORE_LR_SWAP_NO_SWAP;
	core_cfg.pga_r = E_DD_PDM_CORE_PGA_0_0_DB;
	core_cfg.pga_l = E_DD_PDM_CORE_PGA_0_0_DB;
	core_cfg.mclk_div = 8;
	core_cfg.sinc_rate = 16;
	core_cfg.adc_hpd = D_DD_PDM_DISABLE;
	core_cfg.hp_cutoff = 11;
// --- REMOVE_ES1_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_333US;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_1;
// --- REMOVE_ES3_HARDWARE END ---
	core_cfg.soft_mute = D_DD_PDM_DISABLE;
	
	Dd_Pdm_Ctrl_Core(D_DD_PDM_CH0, &core_cfg);
	
	// Data Streaming Start
	Dd_Pdm_Start_Streaming(D_DD_PDM_CH0);
	
	// I2S Clock Setting
	Dd_Audio_Open_Input(D_DD_AUDIO_IF_CH4, D_DDIM_USER_SEM_WAIT_FEVR);
	
	Dd_Audio_Reset(D_DD_AUDIO_IF_CH4);
	
	i2s_common.aumclki = E_DD_AUDIO_MASTER_CLOCK_24_576;
	i2s_common.div_aumclko = E_DD_AUDIO_AUMCLKO_DIV_1;
	i2s_common.div_auclk = E_DD_AUDIO_AUCLK_DIV_8;
	i2s_common.div_lrclk = E_DD_AUDIO_AULR_DIV_64;
	i2s_common.clk_div_enable = D_DD_AUDIO_ENABLE;
	i2s_common.master_slave = E_DD_AUDIO_CLK_MASTER;
	
	Dd_Audio_Ctrl_I2sCmmn(D_DD_AUDIO_IF_CH4, &i2s_common);
	
	   :
	   :
	
	// stops data streaming
	Dd_Pdm_Stop_Streaming(D_DD_PDM_CH0);
	
	// releases an exclusivity
	Dd_Pdm_Close(D_DD_PDM_CH0);
	
	@endcode
	
	@section PDM_AHB		Sample of AHB-Lite master interface Operation
	@image html dd_pdm_dma.png
	<br>
	The example of Audio Output Operation is as follows.
	@code
	
	T_DD_PDM_CORE_CFG core_cfg;
	T_DD_PDM_DMA_CFG dma_cfg;
	T_DD_PDM_DMA_LEN dma_len;
	
	// Initialize PDM Macro
	Dd_PDM_Init();
	
	// gets an exclusivity
	Dd_Pdm_Open(D_DD_PDM_CH0, D_DDIM_USER_SEM_WAIT_FEVR);
	
	// Core Setting
	core_cfg.swap = E_DD_PDM_CORE_LR_SWAP_NO_SWAP;
	core_cfg.pga_r = E_DD_PDM_CORE_PGA_0_0_DB;
	core_cfg.pga_l = E_DD_PDM_CORE_PGA_0_0_DB;
	core_cfg.mclk_div = 8;
	core_cfg.sinc_rate = 16;
	core_cfg.adc_hpd = D_DD_PDM_DISABLE;
	core_cfg.hp_cutoff = 11;
// --- REMOVE_ES1_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_333US;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	core_cfg.s_cycle = E_DD_PDM_CORE_S_CYCLE_1;
// --- REMOVE_ES3_HARDWARE END ---
	core_cfg.soft_mute = D_DD_PDM_DISABLE;
	
	Dd_Pdm_Ctrl_Core(D_DD_PDM_CH0, &core_cfg);
	
	// Flush DMA FIFO
	Dd_Pdm_Flush_Dma_Fifo(D_DD_PDM_CH0);
	
	// DMA Setting
	dma_cfg.dmick_dly = 0;
	dma_cfg.dma_burstlen = E_DD_PDM_DMA_BURST_LEN_8;
	dma_cfg.pcm_chset = E_DD_PDM_DMA_PCM_CH_STEREO;
	dma_cfg.pcm_wdlen = E_DD_PDM_DMA_PCM_WD_24;
	dma_cfg.dma_en = D_DD_PDM_ENABLE;
	
	Dd_Pdm_Ctrl_Dma(D_DD_PDM_CH0, &dma_cfg);
	
	// DMA Transfer Length Setting
	dma_len.ttsize = 0xF0;
	dma_len.tsize = 0x18;
	
	Dd_Pdm_Set_Dma_TransLength(D_DD_PDM_CH0, &dma_len);
	
	// Set Dma0 Distination Address
	Dd_Pdm_Set_DMA0_Dst_Addr(D_DD_PDM_CH0, 0xA0000000);
	
	// Set Dma1 Distination Address
	Dd_Pdm_Set_DMA1_Dst_Addr(D_DD_PDM_CH0, 0xA1000000);
	
	// DMA0 Interrupt Setting
	Dd_Pdm_Set_CallbackDma0Intr(D_DD_PDM_CH0, user_function_dma0_int);
	
	// DMA1 Interrupt Setting
	Dd_Pdm_Set_CallbackDma1Intr(D_DD_PDM_CH0, user_function_dma1_int);
	
	// Enable DMA0 Interrupt
	Dd_Pdm_Set_EnableDma0Intr(D_DD_PDM_CH0, D_DD_PDM_ENABLE);
	
	// Enable DMA1 Interrupt
	Dd_Pdm_Set_EnableDma1Intr(D_DD_PDM_CH0, D_DD_PDM_ENABLE);
	
	// Data Streaming Start
	Dd_Pdm_Start_Streaming(D_DD_PDM_CH0);
	
	   :
	   :
	
	// stops data streaming
	Dd_Pdm_Stop_Streaming(D_DD_PDM_CH0);
	
	// releases an exclusivity
	Dd_Pdm_Close(D_DD_PDM_CH0);
	
	@endcode
*/
/*@}*/

