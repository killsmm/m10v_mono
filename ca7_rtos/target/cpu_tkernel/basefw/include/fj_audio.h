/**
 * @file		fj_audio.h
 * @brief		Audio function API
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _FJ_AUDIO_H
#define _FJ_AUDIO_H

#include "ddim_typedef.h"
#include "fj_std.h"
#include "fj_mode.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_audio_overview
@{
<span style="font-weight: bold;">Introduction</span><br>
&nbsp; <br>
The Audio controls can be achieved by using this API.<br>
<br>
<span style="font-weight: bold;">Supported function</span><br>
<br>
BaseFW supports following functions.<br>
<ul>
 <li>@ref audio_capture</li>
 <li>@ref buzzer</li>
 <li>@ref audio_debug</li>
</ul><br>

@section audio_capture Audio Capture
AudioCapture is a function to capture audio data from the audio device.<br>
<br>

@section buzzer Buzzer
Buzzer is is used in all mode.<br>
For example, Key sound, Shutter sound and Startup sound.<br>
<br>



@section audio_debug Audio Debug Function
This functio is used in UART Command<br>
- AudioCapture (Dump PCM Data of the PlugIn0 Input)<br>
    Please use this command during VideoRecord Processing.<br>
    The Data sent to Muxer are the PCM data(not encoded) of PlugIn0 Input Data.<br>
    
    <br>
    @image html audio_overview_df_cap1.png
    <br>

- AudioCapture (Dump PCM Data of the PlugIn0 Output)<br>
    Please use this command during VideoRecord Processing.<br>
    The Data sent to Muxer are are the PCM data(not encoded) of PlugIn0 Output Data.<br>

    <br>
    @image html audio_overview_df_cap2.png
    <br>

- AudioOut (Dump PCM Data of the PlugIn Input)<br>
    Please use this command during AudioOut Processing.<br>
    Copy the PCM data of PlugIn Input Data to debug area.<br>
    After copy completion, the address and the size of debug areas are notified of by UART message.<br>

    <br>
    @image html audio_overview_df_out1.png
    <br>

- AudioOut (Dump PCM Data of the PlugIn Output)<br>
    Please use this command during AudioOut Processing.<br>
    Copy the PCM data of PlugIn Output Data to debug area.<br>
    After copy completion, the address and the size of debug area are notified of by UART message.<br>

    <br>
    @image html audio_overview_df_out2.png
    <br>
<br>


<hr>
<span style="font-weight: bold;">Block Diagram</span><br>
&nbsp; <br>

<br>
	@image html audio_block_diagram.png
<br>

<hr>
<span style="font-weight: bold;">Sound Connectivity Overview</span><br>
&nbsp; <br>

<br>
	Base Firmware provides flexible connectivity of hardware module and Software.<br>
	It can connect multi hardware module to multi sound processing.<br>
	Each connect configuration API is following.<br>
	For more detailed of configuration APIs, please refer to description of each configuration APIs.<br>
	@image html fj_audio_open.png
<br>
<br>
<br>
	@image html fj_audio_connect.png
<br>
<br>
<br>

@}*//* --- end of fj_audio_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_audio_sequence
@{
	- Audio Capture
	-# @ref audio_capture_seq
    <BR>
	-# @ref audio_capture_stop_seq
    <BR><BR>
	
	- Buzzer
	-# @ref audio_play_beep_without_pb
    <BR><BR>
	-# @ref audio_play_beep_pb
    <BR><BR>
	-# @ref audio_play_beep_stop
    <BR><BR>
	<hr>
	
	@section audio_capture_seq Audio Capture Sequence
	@image html audio_capture.png
    <BR>
    
	@section audio_capture_stop_seq Audio Capture Stop Sequence
	@image html audio_capture_stop.png
    <BR><BR>
	
	@section audio_play_beep_without_pb Buzzer(Other than the Playback mode)
	@image html audio_out_buzzer_LiveView.png
    <BR>
	
	@section audio_play_beep_pb Buzzer(Playback mode)
	@image html audio_out_buzzer_PlayBack.png
    <BR>
    
	@section audio_play_beep_stop Buzzer Stop
	@image html audio_out_buzzer_stop_req.png
    <BR><BR>

@}*//* --- end of fj_audio_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////

/** @weakgroup fj_audio_definition
@{*/

/* Set the compression mode of audio record/play */
#define	FJ_COMPRESS_TYPE_PCM		(0x01)		/**< PCM */
#define	FJ_COMPRESS_TYPE_ALAW		(0x02)		/**< ALAW */
#define	FJ_COMPRESS_TYPE_ULAW		(0x03)		/**< ULAW */
#define	FJ_COMPRESS_TYPE_AAC		(0x05)		/**< AAC */

/* Set the number of channels */
#define	FJ_AUDIO_NUM_CHANNEL_1		(1)			/**< 1 channel(mono) */
#define	FJ_AUDIO_NUM_CHANNEL_2		(2)			/**< 2 channel(stereo or 2 mono) */
#define	FJ_AUDIO_NUM_CHANNEL_3		(3)			/**< 3 channel(3 mono) */
#define	FJ_AUDIO_NUM_CHANNEL_4		(4)			/**< 4 channel(2 stereo or 4 mono) */

/* Set AudioIF Master/Slave Mode of I2S Clock */
#define FJ_AUDIO_IF_MASTER			(0x00)			/**< AudioIF-Master(I2S Communication Setting)<br>
														AudioIF:Master, ExternalAudioCodec:Slave<br> */
#define FJ_AUDIO_IF_SLAVE			(0x01)			/**< AudioIF-Slave(I2S Communication Setting)<br>
														AudioIF:Slave, ExternalAudioCodec:Master<br> */

/* Audio Enable / Disable */
#define FJ_AUDIO_ENABLE				(1)				/**< enable.  */
#define FJ_AUDIO_DISABLE			(0)				/**< disable. */

/*	AudioInput Channel(HW) */
#define FJ_AUDIO_INPUT_CH_0			(0x00000001)		/**< AudioInputChannel 0 */
#define FJ_AUDIO_INPUT_CH_1			(0x00000002)		/**< AudioInputChannel 1 */
#define FJ_AUDIO_INPUT_CH_2			(0x00000003)		/**< AudioInputChannel 2 */

/*	AudioCapture Instance ID */
#define FJ_AUDIO_CAP_ID_0			(0x00000001)		/**< AudioCapture Instance ID 0 */
#define FJ_AUDIO_CAP_ID_1			(0x00000002)		/**< AudioCapture Instance ID 1 */

/*	AudioEncoder Instance ID */
#define FJ_AUDIO_ENC_ID_0			(0x00000001)		/**< AudioEncoder Instance ID 0 */
#define FJ_AUDIO_ENC_ID_1			(0x00000002)		/**< AudioEncoder Instance ID 1 */

/*	AudioOut Instance ID */
#define FJ_AUDIO_OUT_ID_0			(0x00000001)		/**< AudioOut Instance ID 0 */
#define FJ_AUDIO_OUT_ID_1			(0x00000002)		/**< AudioOut Instance ID 1 */

/*	AudioDecoder Instance ID */
#define FJ_AUDIO_DEC_ID_0			(0x00000001)		/**< AudioDecoder Instance ID 0 */


/* Configure Type */
#define FJ_AUDIO_AAC_CFG					(0)			/**< AAC Codec configration.<br>
															This definition is used as an argument of FJ_AudioEncoder_Config().<br>	 */


/* Stop Callback factor */
#define FJ_AUDIO_CB_OUT_STOP				(1)			/**< Factor of the stop when FJ_AudioOut_DirectStopWav() is called.<br>
															This definition is used as an argument of FJ_Host_AudioOut_StopWavCB().<br>	 */
#define FJ_AUDIO_CB_OUT_AUTO_STOP			(2)			/**< Factor of the stop when WAV output operation is completed.<br>
															This definition is used as an argument of FJ_Host_AudioOut_StopWavCB().<br>	 */

///////////////////////////////////////////////////////////////////////////////
// typedef
///////////////////////////////////////////////////////////////////////////////


/** Audio Source Type */
typedef enum {
  E_FJ_AUDIO_SRC_0 = 0x00, 			/**< Audio Source Type 0. */
  E_FJ_AUDIO_SRC_1,					/**< Audio Source Type 1. */
  E_FJ_AUDIO_SRC_2,					/**< Audio Source Type 2. */
} E_FJ_AUDIO_SRC_ENUM;

/** Audio Capure Type */
typedef enum {
  E_FJ_AUDIO_CAPTURE_0 = 0x00, 		/**< Audio Capture Type 0. */
  E_FJ_AUDIO_CAPTURE_1,				/**< Audio Capture Type 1. */
  E_FJ_AUDIO_CAPTURE_2,				/**< Audio Capture Type 2. */
} E_FJ_AUDIO_CAPTURE_ENUM;

/** Audio Out Type */
typedef enum {
  E_FJ_AUDIO_OUT_0 = 0x00, 			/**< Audio Out Type 0. */
  E_FJ_AUDIO_OUT_1,					/**< Audio Out Type 1. */
} E_FJ_AUDIO_OUT_ENUM;

/** Audio In port Type */
typedef enum {
  E_FJ_AUDIO_IN_PORT_I2S0 = 0x00, 		/**< Audio In Port I2S0. */
  E_FJ_AUDIO_IN_PORT_I2S2 = 0x02,		/**< Audio In Port I2S2. */
  E_FJ_AUDIO_IN_PORT_I2S4 = 0x04,		/**< Audio In Port I2S4(PDM). */
} E_FJ_AUDIO_IN_PORT_ENUM;

/** Audio OutDevice Type */
typedef enum {
  E_FJ_AUDIO_OUT_PORT_I2S0 = 0x00, 		/**< Audio Out Port I2S0. */
  E_FJ_AUDIO_OUT_PORT_I2S2 = 0x02,		/**< Audio Out Port I2S2. */
  E_FJ_AUDIO_OUT_PORT_I2S3,				/**< Audio Out Port I2S3. */
} E_FJ_AUDIO_OUT_PORT_ENUM;

/**	WAV control type */
typedef enum {
  E_FJ_AUDIO_WAV_CTL_NONE = 0x00,		/**< Do not play WAV in the case of Movie sound playing. */
  E_FJ_AUDIO_WAV_CTL_MIX, 				/**< Mix WAV and  Movie sound. */
  E_FJ_AUDIO_WAV_CTL_REPLACE,			/**< WAV replaces a Movie sound. */
} E_FJ_AUDIO_WAV_CTL;

/**	I2S format Type */
typedef enum {
	E_FJ_AUDIO_I2S_Data_FORMAT_LEFT		= 0x00,	/**< Left-justified */
	E_FJ_AUDIO_I2S_Data_FORMAT_RIGHT	= 0x01,	/**< Right-justified */
	E_FJ_AUDIO_I2S_Data_FORMAT_I2S		= 0x02,	/**< I2s format */
} E_FJ_AUDIO_I2S_DATA_FORMAT;

/**I2S configuration informations */
typedef struct {
	UCHAR						master_slave;		/**< Master/Slave of I2S Clock (FJ_AUDIO_IF_MASTER or FJ_AUDIO_IF_SLAVE) */
	ULONG						lr_clock;			/**< LR Clock Frequency (8000,12000,16000,24000,32000,48000,96000[Hz]) */
	UCHAR						data_cycle;			/**< Data input/output cycle (32, 64[FS]) */
	E_FJ_AUDIO_I2S_DATA_FORMAT	i2s_format;			/**< I2S Data Format */
	BYTE						bit_count;			/**< Valid bit count (8, 16, 24[bit]) */
} T_FJ_AUDIO_I2S_CFG;

/** audio interface configuration informations(in case of AudioCapture) */
typedef struct {
	E_FJ_AUDIO_IN_PORT_ENUM		audio_in_port;		/**< Input Port Configuration */
	T_FJ_AUDIO_I2S_CFG			i2s_cfg;			/**< I2S Configuration Information */
} T_FJ_AUDIO_CAPTURE_CFG;

/** audio interface configuration informations(in case of AudioPlay) */
typedef struct {
	E_FJ_AUDIO_OUT_PORT_ENUM	audio_out_port;		/**< Output Port Configuration */
	T_FJ_AUDIO_I2S_CFG			i2s_cfg;			/**< I2S Configuration Information */
} T_FJ_AUDIO_OUT_CFG;


/** AudioCapture callback(PlugIn0) informations */
typedef struct {
	UINT32	in_addr;				/**< Address of the pcm data */
	ULONG	in_size;				/**< Size of the pcm data */
	UCHAR	in_ch_type;				/**< Number of Channels<br>
										(FJ_AUDIO_NUM_CHANNEL_1 or FJ_AUDIO_NUM_CHANNEL_2 or FJ_AUDIO_NUM_CHANNEL_3 or FJ_AUDIO_NUM_CHANNEL_4) */
	ULONG	in_sampling_rate;		/**< Sampling rate */
	UINT32	out_addr;				/**< Address of the processed pcm data */
	ULONG	out_size;				/**< Size of the processed pcm data */
	UCHAR	out_ch_type;			/**< Number of Channels of the processed pcm data */
	ULONG	out_sampling_rate;		/**< Sampling rate of the processed pcm data */
} T_FJ_AUDIO_CAP_PLUGIN0_CB_PARAM;

/** AudioCapture callback(PlugIn1) informations */
typedef struct {
	UCHAR	area_index;		/**< Area index for free */
	UINT32	addr;			/**< Address of the pcm data */
	ULONG	size;			/**< Size of the pcm data */
	UCHAR	ch_type;		/**< Number of Channels<br>
								(FJ_AUDIO_NUM_CHANNEL_1 or FJ_AUDIO_NUM_CHANNEL_2 or FJ_AUDIO_NUM_CHANNEL_3 or FJ_AUDIO_NUM_CHANNEL_4) */
	ULONG	sampling_rate;	/**< Sampling rate */
} T_FJ_AUDIO_CAP_PLUGIN1_CB_PARAM;

/** AudioOut callback(PlugIn) function parameter */
typedef struct {
	UINT32	in_addr;				/**< Address of the pcm data */
	ULONG	in_size;				/**< Size of the pcm data */
	UCHAR	in_ch_type;				/**< Number of Channels<br>
										(FJ_AUDIO_NUM_CHANNEL_1 or FJ_AUDIO_NUM_CHANNEL_2 or FJ_AUDIO_NUM_CHANNEL_3 or FJ_AUDIO_NUM_CHANNEL_4) */
	ULONG	in_sampling_rate;		/**< Sampling rate */
	UINT32	out_addr;				/**< Address of the processed pcm data */
	ULONG	out_size;				/**< Size of the processed pcm data */
	UCHAR	out_ch_type;			/**< Number of Channels of the processed pcm data<br>
										(FJ_AUDIO_NUM_CHANNEL_1 or FJ_AUDIO_NUM_CHANNEL_2 or FJ_AUDIO_NUM_CHANNEL_3 or FJ_AUDIO_NUM_CHANNEL_4) */
	ULONG	out_sampling_rate;		/**< Sampling rate of the processed pcm data */
} T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM;


/** AudioCaputure Sample config parameter */
typedef struct {
	ULONG	in_sampling_rate;		/**< SamplingRate of input sample data form device. */
	UCHAR	in_ch_type;				/**< Number of Channels of input sample data form device.<br>
										(FJ_AUDIO_NUM_CHANNEL_1 or FJ_AUDIO_NUM_CHANNEL_2 or FJ_AUDIO_NUM_CHANNEL_3 or FJ_AUDIO_NUM_CHANNEL_4) */
	ULONG	out_sampling_rate;		/**< SamplingRate of output sample data to Connected Block(AudioEncoder or AudioOut). */
	UCHAR	out_ch_type;			/**< Number of Channels of output sample data to Connected Block(AudioEncoder or AudioOut).<br>
										(FJ_AUDIO_NUM_CHANNEL_1 or FJ_AUDIO_NUM_CHANNEL_2 or FJ_AUDIO_NUM_CHANNEL_3 or FJ_AUDIO_NUM_CHANNEL_4) */
} T_FJ_AUDIO_CAP_SMPL_CFG;

/*@} --- end of fj_audio_definition */

///////////////////////////////////////////////////////////////////////////////
// macro
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_audio_api
@{*/

///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////
/* AudioSRC Block (Dummy) */
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function open Audio Source instance.<br>
@param[in]	audio_src		Audio Source.<br>
								[E_FJ_AUDIO_SRC_0, E_FJ_AUDIO_SRC_1, E_FJ_AUDIO_SRC_2]<br>
@param[out]	audio_src_id	Audio Source ID (SW instance ID).<br>
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred. *it already open instance, we return this error.
@remarks
This function is used in SDK.<br><br>
*/
extern FJ_ERR_CODE FJ_AudioSRC_Open( E_FJ_AUDIO_SRC_ENUM audio_src, UCHAR * audio_src_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function close Audio Source instance.<br>
@param[in]	audio_src_id	Audio Source ID (SW instance ID).<br>
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred.
@remarks
This function is used in SDK.<br><br>
*/
extern FJ_ERR_CODE FJ_AudioSRC_Close( UCHAR audio_src_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function connects Audio Source with Audio Capture Instance.
@param[in]	audio_src_id		AudioSRC Instance ID.<br>
@param[in]	audio_capture_id	AudioCapture Instance ID.<br>
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
@remarks
This function is used in SDK.<br><br>
*/
extern FJ_ERR_CODE FJ_AudioSRC_Connect( UCHAR audio_src_id, UCHAR audio_capture_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function Disconnects Audio Source from Audio Capture Instance.
@param[in]	audio_src_id		AudioSRC Instance ID.<br>
@param[in]	audio_capture_id	AudioCapture Instance ID.<br>
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
@remarks
This function is used in SDK.<br><br>
*/
extern FJ_ERR_CODE FJ_AudioSRC_Disconnect( UCHAR audio_src_id, UCHAR audio_capture_id );


/* AudioCapture Block */
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function open Audio Capture instance.<br>
@param[in]	audio_cap			Audio Capture.<br>
								[E_FJ_AUDIO_CAPTURE_0, E_FJ_AUDIO_CAPTURE_1, E_FJ_AUDIO_CAPTURE_2]<br>
@param[out]	audio_capture_id	Audio Capture ID (SW instance ID).<br>
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred. *it already open instance, we return this error.
*/
extern FJ_ERR_CODE FJ_AudioCapture_Open( E_FJ_AUDIO_CAPTURE_ENUM audio_cap,  UCHAR * audio_capture_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function close Audio Capture instance.<br>
@param[in]	audio_capture_id	Audio Capture ID (SW instance ID).<br>
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE FJ_AudioCapture_Close( UCHAR audio_capture_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function connects Audio Capture Instance with Audio Out Instance.
@param[in]	audio_capture_id		AudioCapture Instance ID.<br>
@param[in]	audio_out_id			AudioOut Instance ID.<br>
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
extern FJ_ERR_CODE FJ_AudioCapture_ConnectOut( UCHAR audio_capture_id, UCHAR audio_out_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function Disconnects Audio Capture Instance from Audio Out Instance.
@param[in]	audio_capture_id		AudioCapture Instance ID.<br>
@param[in]	audio_out_id			AudioOut Instance ID.<br>
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
extern FJ_ERR_CODE FJ_AudioCapture_DisconnectOut( UCHAR audio_capture_id, UCHAR audio_out_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function connects Audio Capture Instance with Audio Encoder Instance.
@param[in]	audio_capture_id		AudioCapture Instance ID.<br>
@param[in]	audio_enc_id			AudioEnc Instance ID.<br>
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
extern FJ_ERR_CODE FJ_AudioCapture_ConnectEnc( UCHAR audio_capture_id, UCHAR audio_enc_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function connects Audio Capture Instance with Audio Encoder Instance.
@param[in]	audio_capture_id		AudioCapture Instance ID.<br>
@param[in]	audio_enc_id			AudioEnc Instance ID.<br>
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
extern FJ_ERR_CODE FJ_AudioCapture_DisconnectEnc( UCHAR audio_capture_id, UCHAR audio_enc_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets the communication between the audio capture and the audio device.<br>
@param[in]	audio_capture_id	AudioCapture Instance ID.<br>
@param[in]	audio_capture_config  Audio interface configuration informations
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred.
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
@remarks
@remarks
This function performs communication setting between the audio capture and the audio input device.<br><br>
When AudioCapture is processing, This function is not used.<br>
When AudioCapture was stoped, This function is used.<br>

The combinations of setable LR Clock and Data Cycle are as follows.<br>
- audio_in_port = E_FJ_AUDIO_IN_PORT_I2S0 / E_FJ_AUDIO_IN_PORT_I2S0<br>
<br>
<table>
	<tr>
		<th colspan="2" rowspan="2"></th>
		<th colspan="2" style="text-align:center;">Data Cycle</th>
	</tr>
	<tr>
		<td style="text-align:center;">32FS</td>
		<td style="text-align:center;">64FS</td>
	</tr>
	<tr>
		<th rowspan="9">LR Clock</th>
		<td style="text-align:center;">8000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">12000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">16000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">24000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">32000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">48000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">96000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
</table>
<br><br>

<br>
- audio_in_port = E_FJ_AUDIO_IN_PORT_I2S4<br>
(E_FJ_AUDIO_IN_PORT_I2S4 is the ports for the PDM.)<br>
<br>
<table>
	<tr>
		<th colspan="2" rowspan="2"></th>
		<th colspan="2" style="text-align:center;">Data Cycle</th>
	</tr>
	<tr>
		<td style="text-align:center;">32FS</td>
		<td style="text-align:center;">64FS</td>
	</tr>
	<tr>
		<th rowspan="9">LR Clock</th>
		<td style="text-align:center;">8000Hz</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">12000Hz</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">16000Hz</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
// --- REMOVE_ES3_HARDWARE BEGIN ---
		<td style="font-size:80%;text-align:center;">Support</td>
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		<td style="font-size:80%;text-align:center;">Not Support</td>
// --- REMOVE_ES1_HARDWARE END ---
	</tr>
	<tr>
		<td style="text-align:center;">24000Hz</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
// --- REMOVE_ES3_HARDWARE BEGIN ---
		<td style="font-size:80%;text-align:center;">Support</td>
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		<td style="font-size:80%;text-align:center;">Not Support</td>
// --- REMOVE_ES1_HARDWARE END ---
	</tr>
	<tr>
		<td style="text-align:center;">32000Hz</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">48000Hz</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">96000Hz</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
	</tr>
</table>
<br><br>

<br>
- Combination of setable frequency of LR Clock<br>
<br>
<table>
	<tr>
		<th colspan="2" rowspan="2"></th>
		<th colspan="7" style="text-align:center;">2nd instance</th>
	</tr>
	<tr>
		<td style="text-align:center;">8000Hz</td>
		<td style="text-align:center;">12000Hz</td>
		<td style="text-align:center;">16000Hz</td>
		<td style="text-align:center;">24000Hz</td>
		<td style="text-align:center;">32000Hz</td>
		<td style="text-align:center;">48000Hz</td>
		<td style="text-align:center;">96000Hz</td>
	</tr>
	<tr>
		<th rowspan="9">1st Instance</th>
		<td style="text-align:center;">8000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">12000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">16000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">24000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">32000Hz</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">48000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">96000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
</table>
<br><br>

For example.<br>
The capture audio data are stereos.<br>
I2S Setting of "I2S0" is as follows.<br>
- Master/Slave: Master<br>
- LR Clock: 48kHz<br>
- data input cycle: 32FS<br>
- data format: Left justified<br>
- valid bit count: 16bit<br>
<br>
Sample code is as follows.<BR>
@code
extern FJ_ERR_CODE error_code;
T_FJ_AUDIO_CAPTURE_CFG audio_capture_config;

audio_capture_config.audio_in_port = E_FJ_AUDIO_IN_PORT_I2S0;
audio_capture_config.i2s_cfg.master_slave = FJ_AUDIO_IF_MASTER;
audio_capture_config.i2s_cfg.lr_clock = 48000;
audio_capture_config.i2s_cfg.data_cycle = 32;
audio_capture_config.i2s_cfg.i2s_format = E_FJ_AUDIO_I2S_Data_FORMAT_LEFT;
audio_capture_config.i2s_cfg.bit_count = 16;

error_code = FJ_AudioCapture_Config(audio_capture_id, &audio_capture_config);
@endcode
*/
extern FJ_ERR_CODE FJ_AudioCapture_Config( UCHAR audio_capture_id, T_FJ_AUDIO_CAPTURE_CFG * audio_capture_config );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets audio capture input/output samples<br>
This function sets the format(SamplingRate,ChannelType) of the data before and after the PlugIn0.<br>
@param[in]	audio_capture_id	AudioCapture Instance ID.<br>
@param[in]	audio_cap_smpl_config	input/output sample infomation(SamplingRate, ChannelType).<br>
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred.
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
@remarks
When AudioCapture is processing, This function is not used.<br>
When AudioCapture was stoped, This function is used.<br>
*/
extern FJ_ERR_CODE FJ_AudioCapture_SampleConfig( UCHAR audio_capture_id, T_FJ_AUDIO_CAP_SMPL_CFG * audio_cap_smpl_config);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function starts capture from audio device connected to AudioCapture instance.<br>
@param[in]	audio_capture_id	AudioCapture Instance ID.<br>
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred.
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
extern FJ_ERR_CODE FJ_AudioCapture_Start( UCHAR audio_capture_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function stops capture from audio device connected to AudioCapture instance.<br>
@param[in]	audio_capture_id	AudioCapture Instance ID.<br>
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred.
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
@remarks
If this function is executed after FJ_AudioEncoder_Stop(), Please execute this function after having encoded the last frame.<br>
*/
extern FJ_ERR_CODE FJ_AudioCapture_Stop( UCHAR audio_capture_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set enable(disable) callback function(FJ_Host_AudioCapturePlugIn1CB()).<br>
@param[in]	audio_capture_id	AudioCapture Instance ID.<br>
@param[in]	enable		FJ_AUDIO_ENABLE:Enable callback function(FJ_Host_AudioCapturePlugIn1CB()).<br>
						FJ_AUDIO_DISABLE:Disable callback function(FJ_Host_AudioCapturePlugIn1CB())<br>
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred.
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
extern FJ_ERR_CODE FJ_AudioCapture_EnablePlugIn1( UCHAR audio_capture_id, UCHAR enable );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function notifies the completion of pcm data processing(FJ_Host_AudioCapturePlugIn1CB()).<br>
A buffer area appointed in "free_area_index" is released.<br>
@param[in]	audio_capture_id	AudioCapture Instance ID.<br>
@param[in]	free_area_index	Area index for free.<br>
							Set a value of "area_index" notified of in FJ_Host_AudioCapturePlugIn1CB().<br>
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred.
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
extern FJ_ERR_CODE FJ_AudioCapture_CompletePlugIn1( UCHAR audio_capture_id, UCHAR free_area_index );


/* AudioOut Block */
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function open Audio Out instance.<br>
@param[in]	audio_out			Audio Out.<br>
								[E_FJ_AUDIO_OUT_0, E_FJ_AUDIO_OUT_1]<br>
@param[out]	audio_out_id	Audio Out ID (SW instance ID).<br>
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred. *it already open instance, we return this error.
*/
extern FJ_ERR_CODE FJ_AudioOut_Open( E_FJ_AUDIO_OUT_ENUM audio_out,  UCHAR * audio_out_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function close Audio Out instance.<br>
@param[in]	audio_out_id	Audio Out ID (SW instance ID).<br>
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE FJ_AudioOut_Close( UCHAR audio_out_id );
extern FJ_ERR_CODE FJ_AudioOut_ForceClose( UCHAR audio_out_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets the communication of the audio interface corresponding to the AudioChannel.<br>
@param[in]	audio_out_id	AudioOut Instance ID.<br>
@param[in]	audio_out_config  audio interface configuration informations
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred.
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
@remarks
This function performs communication setting between the audio interface and the audio device.<br><br>

The combinations of setable LR Clock and Data Cycle are as follows.<br>
<table>
	<tr>
		<th colspan="2" rowspan="2"></th>
		<th colspan="2" style="text-align:center;">Data Cycle</th>
	</tr>
	<tr>
		<td style="text-align:center;">32FS</td>
		<td style="text-align:center;">64FS</td>
	</tr>
	<tr>
		<th rowspan="9">LR Clock</th>
		<td style="text-align:center;">8000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">12000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">16000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">24000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">32000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">48000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">96000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
</table>
<br><br>

<br>
- Combination of setable frequency of LR Clock<br>
<br>
<table>
	<tr>
		<th colspan="2" rowspan="2"></th>
		<th colspan="7" style="text-align:center;">2nd instance</th>
	</tr>
	<tr>
		<td style="text-align:center;">8000Hz</td>
		<td style="text-align:center;">12000Hz</td>
		<td style="text-align:center;">16000Hz</td>
		<td style="text-align:center;">24000Hz</td>
		<td style="text-align:center;">32000Hz</td>
		<td style="text-align:center;">48000Hz</td>
		<td style="text-align:center;">96000Hz</td>
	</tr>
	<tr>
		<th rowspan="9" style="text-align:center;">1st Instance</th>
		<td style="text-align:center;">8000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">12000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">16000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">24000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">32000Hz</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">48000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
	<tr>
		<td style="text-align:center;">96000Hz</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Not Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
		<td style="font-size:80%;text-align:center;">Support</td>
	</tr>
</table>
<br><br>

For example.<br>
The output audio data are monaural.<br>
I2S Setting of "I2S" is as follows.<br>
- Master/Slave: Master<br>
- LR Clock: 32kHz<br>
- data output cycle: 64FS<br>
- data format: I2s<br>
- valid bit count: 24bit<br>
<br>
Sample code is as follows.<br>
@code
extern FJ_ERR_CODE error_code;
T_FJ_AUDIO_OUT_CFG audio_out_config;

audio_out_config.audio_out_port = E_FJ_AUDIO_OUT_PORT_I2S0;
audio_out_config.i2s_cfg.master_slave = FJ_AUDIO_IF_MASTER;
audio_out_config.i2s_cfg.lr_clock = 32000;
audio_out_config.i2s_cfg.data_cycle = 64;
audio_out_config.i2s_cfg.i2s_format = E_FJ_AUDIO_I2S_Data_FORMAT_I2S;
audio_out_config.i2s_cfg.bit_count = 24;

error_code = FJ_AudioOut_Config(audio_out_id, &audio_out_config);
@endcode
*/
extern FJ_ERR_CODE FJ_AudioOut_Config( UCHAR audio_out_id, T_FJ_AUDIO_OUT_CFG * audio_out_config );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
The WAV file is loaded from SDRAM to one of 30 segments of SDRAM.<br>
@param[in]	addr	Address of the file to be loaded
@param[in]	size 	Size of the file to be loaded
@param[in]	wav_id 	ID of the loaded file(0:Error)
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred. *it already open instance, we return this error.
@remarks The maximum number of WAV files that Load is 30.<br><br>
The waveFormatType to support is as follows.<br>
- PCM(0x0001)<br>
- A-Law(0x0006)<br>
- mu-Law(0x0007)<br>
*/
extern FJ_ERR_CODE FJ_AudioOut_DirectLoadWav( UINT32 addr, ULONG size, UINT32 * wav_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function unload audio file from the segment of SDRAM by wave_id
@param[in]	wav_id		Number of wave id for output (Wav ID)
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
*/
extern FJ_ERR_CODE FJ_AudioOut_DirectUnloadWav( UINT32 wav_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function reads audio file from the segment of SDRAM, and Output it.
@param[in]	audio_out_id	AudioOut Instance ID.<br>
@param[in]	wav_id			Number of wave id for output (Wav ID)
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
extern FJ_ERR_CODE FJ_AudioOut_DirectPlayWav( UCHAR audio_out_id, UINT32 wav_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function stops WAV output operation.
@param[in]	audio_out_id	AudioOut Instance ID.<br>
@param[in]	wav_id			Number of wave id for output (Wav ID)
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
extern FJ_ERR_CODE FJ_AudioOut_DirectStopWav( UCHAR audio_out_id, UINT32 wav_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function controls operation of WAV.<br>
This function sets behavior in case of playing Buzzer and Movie sound at the same time.<br>
@param[in]	audio_out_id	AudioOut Instance ID.<br>
@param[in]	control			WAV control type<br>
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
extern FJ_ERR_CODE FJ_AudioOut_ControlWav( UCHAR audio_out_id, E_FJ_AUDIO_WAV_CTL control );


/*@} --- end of fj_audio_api */

///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_audio_message
@{*/

/**
@def FJ_HM_AUDIO_OUT_BEGIN
Audio output start notification message.<br>
This message is used in SDK.<br><br>

*/

/**
@def FJ_HM_AUDIO_OUT_AUTO_STOP
Audio output automatic stop notification message.<br>
This message is used in SDK.<br><br>
*/

/*@} --- end of fj_audio_message */

#endif
