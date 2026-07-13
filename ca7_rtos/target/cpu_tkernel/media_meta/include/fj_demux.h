/**
 * @file		fj_demux.h
 * @brief		Demux Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_DEMUX_H
#define _FJ_DEMUX_H

#include "fj_std.h"
#include "ddim_typedef.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @addtogroup fj_demux_overview
@{
	The DEMUX controls can be achieved by using this API.<br>
@}*//* --- end of fj_demux_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @addtogroup fj_demux_sequence
@{
	Please refer BaseFWPF Specification.
@}*//* --- end of fj_demux_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_demux_definition
@{*/
#define FJ_DEMUX_READ_MAX_NUM	120	/**< The maximum number to be read at one time. */


///////////////////////////////////////////////////////////////////////////////
// Enumeration
///////////////////////////////////////////////////////////////////////////////
/**
Description for FJ_DEMUX_CODEC.
*/
typedef enum {
	E_FJ_DEMUX_CODEC_VIDEO_H264	= 0,	/**< Codec is H.264 */
	E_FJ_DEMUX_CODEC_VIDEO_H265	= 1,	/**< Codec is H.265 */
	E_FJ_DEMUX_CODEC_AUDIO_AAC	= 2,	/**< Codec is AAC */
	E_FJ_DEMUX_CODEC_AUDIO_PCM	= 3,	/**< Codec is PCM */
	E_FJ_DEMUX_CODEC_INVALID	= 4,	/**< Video or Audio Codec is invalid. */
} E_FJ_DEMUX_CODEC;

/**
Description for FJ_DEMUX_DIRECTION.
*/
typedef enum {
	E_FJ_DEMUX_DIRECTION_FWD	= 0,	/**< Direction is Forward */
	E_FJ_DEMUX_DIRECTION_RWD	= 1,	/**< Direction is Rewind */
} E_FJ_DEMUX_DIRECTION;

/**
Description for FJ_DEMUX_SPEED.
*/
typedef enum {
	E_FJ_DEMUX_SPEED_1X			= 0,	/**< Normal speed */
	E_FJ_DEMUX_SPEED_2X			= 1,	/**<     2x speed */
	E_FJ_DEMUX_SPEED_4X			= 2,	/**<     4x speed */
	E_FJ_DEMUX_SPEED_8X			= 3,	/**<     8x speed */
	E_FJ_DEMUX_SPEED_1_2X		= 4,	/**<   1/2x speed */
	E_FJ_DEMUX_SPEED_1_4X		= 5,	/**<   1/4x speed */
	E_FJ_DEMUX_SPEED_1_8X		= 6,	/**<   1/8x speed */
} E_FJ_DEMUX_SPEED;


///////////////////////////////////////////////////////////////////////////////
// typedef
///////////////////////////////////////////////////////////////////////////////
/**
Description for T_FJ_DEMUX_HEADER_INFO.
*/
typedef struct {
	// video
	E_FJ_DEMUX_CODEC	video_codec;				/**< Video codec type */
	ULONG				track_width;				/**< Track width(display width) */
	ULONG				track_height;				/**< Track height(display height) */
	USHORT				stream_width;				/**< Stream width */
	USHORT				stream_height;				/**< Stream height */
	UINT32				video_time_scale;			/**< Video time scale */
	UINT32				video_time_delta;			/**< Video time delta */
	UINT32				video_total_sample_num;		/**< Video total number of samples */
	UINT32				video_total_duration;		/**< Video total number of duration */
	UINT32				video_play_last_pts;		/**< Last pts at the normal play */
	UINT32				video_ff_last_pts;			/**< Last pts at the fast forward */
	UINT32				offset_of_start_frame;		/**< Offset of the playback start frame. */
	// audio
	E_FJ_DEMUX_CODEC	audio_codec;				/**< Audio codec type */
	UINT32				sampling_rate;				/**< Sampling rate */
	UINT32				audio_bit_rate;				/**< Bit rate */
	UINT32				audio_ch;					/**< Audio channel : mono(1)/stereo(2) */
	UINT32				audio_time_scale;			/**< Audio time scale */
	UINT32				audio_time_delta;			/**< Audio time delta */
	UINT32				audio_total_sample_num;		/**< Audio total number of sample */
	UINT32				audio_total_duration;		/**< Audio total number of duration */
	UINT32				audio_last_pts;				/**< Last pts */
} T_FJ_DEMUX_HEADER_INFO;

/**
Description for T_FJ_DEMUX_STREAM_PARAM.
*/
typedef struct {
	UINT32		stream_num;									/**< Number of stream */
	UINT32		stream_size[FJ_DEMUX_READ_MAX_NUM];			/**< Stream size */
	UINT32		stream_address[FJ_DEMUX_READ_MAX_NUM];		/**< Stream address */
	ULLONG		pts[FJ_DEMUX_READ_MAX_NUM];					/**< Presentation Time Stamp */
	ULLONG		dts[FJ_DEMUX_READ_MAX_NUM];					/**< Decoding Time Stamp */
	UINT32		stss[FJ_DEMUX_READ_MAX_NUM];				/**< Sync Sample : true(1)/false(0) */
	UINT32		stream_end_flag[FJ_DEMUX_READ_MAX_NUM];		/**< Flag indicating the end of the stream */
} T_FJ_DEMUX_STREAM_PARAM;


/*@} --- end of fj_demux_definition */

///////////////////////////////////////////////////////////////////////////////
// macro
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_demux_api
@{*/

///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function initialize the Demux.<br>
@param[in]	file_id				File ID.<br>
@param[out]	header_info			Header information.<br>
@retval		FJ_ERR_OK			Normal end.
@retval		FJ_ERR_NG			Error occurred.
*/
FJ_ERR_CODE	FJ_Demux_Initialize( UINT32 file_id, T_FJ_DEMUX_HEADER_INFO* header_info );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function will be the setup of video.<br>
@param[in]	file_id				File ID.<br>
@param[in]	start_pts			PTS to start playback.<br>
@param[in]	direction			Direction to start playback.
@param[in]	speed				Speed to start playback.
@retval		FJ_ERR_OK			Normal end.
@retval		FJ_ERR_NG			Error occurred.
*/
FJ_ERR_CODE	FJ_Demux_VideoSetup( UINT32 file_id, ULLONG start_pts, E_FJ_DEMUX_DIRECTION direction, E_FJ_DEMUX_SPEED speed );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function will be the setup of audio.<br>
@param[in]	file_id				File ID.<br>
@param[in]	start_pts			PTS to start playback.<br>
@retval		FJ_ERR_OK			Normal end.
@retval		FJ_ERR_NG			Error occurred.
*/
FJ_ERR_CODE	FJ_Demux_AudioSetup( UINT32 file_id, ULLONG start_pts );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get free atom data size.<br>
@param[in]	file_id				File ID.<br>
@param[out]	data_size			Free atom data size.<br>
@retval		FJ_ERR_OK			Normal end.
@retval		FJ_ERR_NG			Error occurred.
@remarks If the free atom is not in the file, it will be "retval = FJ_ERR_OK" and "data_size = 0".
*/
FJ_ERR_CODE	FJ_Demux_FreeAtomSizeGet( UINT32 file_id, UINT32* data_size );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function get free atom data.<br>
@param[in]	file_id				File ID.<br>
@param[in]	data_address		It is the address to store the data of the free atom.<br>
@retval		FJ_ERR_OK			Normal end.
@retval		FJ_ERR_NG			Error occurred.
*/
FJ_ERR_CODE	FJ_Demux_FreeAtomGet( UINT32 file_id, UINT32 data_address );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function reads the stream (async).<br>
@param[in]	file_id				File ID.<br>
@param[in]	codec				Codec type.<br>
@param[in]	read_stream_num		The number of streams to read.<br>
@retval		FJ_ERR_OK			Normal end.
@retval		FJ_ERR_NG			Error occurred.
*/
FJ_ERR_CODE	FJ_Demux_StreamRead( UINT32 file_id, E_FJ_DEMUX_CODEC codec, UINT32 read_stream_num );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function reads the stream (sync).<br>
@param[in]	file_id				File ID.<br>
@param[in]	codec				Codec type.<br>
@param[in]	read_stream_num		The number of streams to read.<br>
@param[out]	stream_param		Parameters of the read stream.<br>
@retval		FJ_ERR_OK			Normal end.
@retval		FJ_ERR_NG			Error occurred.
*/
FJ_ERR_CODE	FJ_Demux_StreamReadSync( UINT32 file_id, E_FJ_DEMUX_CODEC codec, UINT32 read_stream_num, T_FJ_DEMUX_STREAM_PARAM* stream_param );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function finalize the Demux.<br>
@param[in]	file_id				File ID.<br>
@retval		FJ_ERR_OK			Normal end.
@retval		FJ_ERR_NG			Error occurred.
*/
FJ_ERR_CODE	FJ_Demux_Finalize( UINT32 file_id );

/*@} --- end of fj_demux_api */

///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
#endif	// _FJ_DEMUX_H
