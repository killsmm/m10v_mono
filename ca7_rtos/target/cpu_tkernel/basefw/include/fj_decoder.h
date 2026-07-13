/**
 * @file		fj_decoder.h
 * @brief		Decoder API Header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_DECODER_H
#define _FJ_DECODER_H

#include "fj_mode.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_decoder_overview
@{

	-# @ref Video_codec_specification
	-# @ref Audio_codec_specification
	-# @ref fj_decoder_api_configuration_connectivity_overview
	-# @ref fj_decoder_api_configuration_connectivity_video_playback

	<hr>
	@section Video_codec_specification Video codec specification
	<table style="text-align:left;" border="1" cellpadding="1" cellspacing="2">
		<tr>
			<th style="background-color: rgb(204, 255, 255);">Item</th>
			<th style="background-color: rgb(204, 255, 255);">Specification of BaseFW for H264</th>
			<th style="background-color: rgb(204, 255, 255);">Specification of BaseFW for H265</th>
			<th style="background-color: rgb(204, 255, 255);">Changeable</th>
			<th style="background-color: rgb(204, 255, 255);">Remarks</th>
		</tr>
		<tr>
			<th valign=top>Stream Format</th>
			<td valign=top>ES (Elementary Stream)</td>
			<td valign=top>ES (Elementary Stream)</td>
			<td valign=top>-</td>
			<td valign=top>-</td>
		</tr>
		<tr>
			<th valign=top>Image Format</th>
			<td valign=top>ALL YCC420 (Video Format)</td>
			<td valign=top>ALL YCC420 (Video Format)</td>
			<td valign=top>-</td>
			<td valign=top>-</td>
		</tr>
		<tr>
			<th valign=top>Playing Size</th>
			<td valign=top>
				4096x2304<br>
				4096x2160<br>
				4096x2048<br>
				3840x2160<br>
				2704x2028<br>
				2704x1520<br>
				1920x1440<br>
				1920x1080<br>
				1440x1080<br>
				1280x960<br>
				1280x720<br>
				848x480<br>
				720x480<br>
				640x480<br>
				640x360<br>
				432x240<br>
				320x240
			</td>
			<td valign=top>
				4096x2160<br>
				3840x2160<br>
				3000x3000<br>
				2704x2028<br>
				2704x1520<br>
				2560x1920<br>
				1920x1440<br>
				1920x1080<br>
				1504x1504<br>
				1440x1080<br>
				1280x960<br>
				1280x720<br>
				848x480
			</td>
			<td valign=top>@ref FJ_VideoPlayback_SetVideoConfig().</td>
			<td valign=top>-</td>
		</tr>
		<tr>
			<th valign=top>Frame Rate</th>
			<td valign=top>
				240fps <br>
				120fps <br>
				 60fps <br>
				 50fps <br>
				 48fps <br>
				 30fps <br>
				 25fps <br>
				 24fps <br>
				 15fps <br>
				239.76fps <br>
				119.88fps <br>
				 59.94fps <br>
				 47.95fps <br>
				 29.97fps <br>
				 23.97fps <br>
				 14.98fps <br>
			<td valign=top>
				240fps <br>
				120fps <br>
				 60fps <br>
				 30fps <br>
				 15fps <br>
				239.76fps <br>
				119.88fps <br>
				 59.94fps <br>
				 29.97fps <br>
				 14.98fps <br>
			<td valign=top>@ref FJ_VideoPlayback_SetVideoConfig().</td>
			<td valign=top>The combination of the allowed image size and frame rate, please refer to the following.<br>
						   @ref FrameRate_and_Size_List_H264 and @ref FrameRate_and_Size_List_H265 </td>
		</tr>
		<tr>
			<th valign=top>Pause</th>
			<td valign=top>Possible</td>
			<td valign=top>Possible</td>
			<td valign=top>@ref FJ_VideoPlayback_PauseVideo()<br>
						   @ref FJ_VideoPlayback_ResumeVideo</td>
			<td valign=top>-</td>
		</tr>
		<tr>
			<th valign=top>FWD/RWD</th>
			<td valign=top>7Step (Speed : x1/8, x1/4, x1/2, x1, x2, x4, x8)</td>
			<td valign=top>7Step (Speed : x1/8, x1/4, x1/2, x1, x2, x4, x8)</td>
			<td valign=top>@ref FJ_VideoPlayback_SetStartPTS()<br>
						   @ref FJ_VideoPlayback_SetSpeedDirection()</td>
			<td valign=top>
				- FF/REW display cycle is as following.<br>
				2x:250ms, 4x:125ms, 8x:62.5ms.<br>
				Please Feed the Stream of SyncFrame in accordance with the display cycle of the above.<br>
				<br>
				- For the function of reverse playback(direction=RWD speed=x1/8, x1/4, x1/2, x1),<br>
				Interval of SyncFrame must be within 15frame.
			</td>
		</tr>
		<tr>
			<th valign=top>Decode for screennail</th>
			<td valign=top>Possible</td>
			<td valign=top>Possible</td>
			<td valign=top>@ref FJ_VideoPlayback_StartIFrameDecoding()</td>
			<td valign=top>Only decode, or decode and display.</td>
		</tr>
		<tr>
			<th valign=top>Get display image during video playback</th>
			<td valign=top>Possible</td>
			<td valign=top>Possible</td>
			<td valign=top>@ref FJ_VideoPlayback_GetDisplayFrame()</td>
			<td valign=top>It is possible only when paused.</td>
		</tr>
	</table>

	<hr>
	@section Audio_codec_specification Audio codec specification
	<table style="text-align:left;" border="1" cellpadding="1" cellspacing="2">
		<tr>
			<th style="background-color: rgb(204, 255, 255);">Item</th>
			<th style="background-color: rgb(204, 255, 255);">Specification of BaseFW for AAC</th>
			<th style="background-color: rgb(204, 255, 255);">Changeable</th>
			<th style="background-color: rgb(204, 255, 255);">Remarks</th>
		</tr>
		<tr>
			<th valign=top>Stream Format</th>
			<td valign=top>ES (Elementary Stream)</td>
			<td valign=top>-</td>
			<td valign=top>-</td>
		</tr>
		<tr>
			<th valign=top>Sampling Rate</th>
			<td valign=top>8000Hz<br>
						   12000Hz<br>
						   16000Hz<br>
						   24000Hz<br>
						   32000Hz<br>
						   48000Hz</td>
			<td valign=top>@ref FJ_AudioDecoder_Config()</td>
			<td valign=top>Please set the sampling rate of 32000Hz or more to output HDMI.</td>
		</tr>
		<tr>
			<th valign=top>Bit Rate</th>
			<td valign=top>sampling rate 8000[Hz]:8000-48000[bps/ch]<br>
						   sampling rate 12000[Hz]:8000-72000[bps/ch]<br>
						   sampling rate 16000[Hz]:8000-96000[bps/ch]<br>
						   sampling rate 24000[Hz]:8000-144000[bps/ch]<br>
						   sampling rate 32000[Hz]:8000-192000[bps/ch]<br>
						   sampling rate 48000[Hz]:8000-288000[bps/ch]</td>
			<td valign=top>-</td>
			<td valign=top>-</td>
		</tr>
		<tr>
			<th valign=top>Channel(Stereo/Mono)</th>
			<td valign=top>Stereo, Mono</td>
			<td valign=top>@ref FJ_AudioDecoder_Config()</td>
			<td valign=top>-</td>
		</tr>
	</table>

	<hr>
	@section fj_decoder_api_configuration_connectivity_overview	Image Connectivity Overview
	Base Firmware provides flexible connectivity of hardware module and Software.<br>
	Each connect configuration API is following.<br>
	@image html fj_decoder_connectivity_overview.png

	<hr>
	@section fj_decoder_api_configuration_connectivity_video_playback	Image Connectivity Video PlayBack Sample
	Sample connection setting of Video playback is as following.<br>
	@image html fj_decoder_connectivity_video_playback_sample.png

@}*//* --- end of fj_decoder_overview */


///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_decoder_sequence
@{
	- Video Play
	-# @ref fj_movie_play_seq_1
	-# @ref fj_movie_play_seq_2
	-# @ref fj_movie_play_seq_3
	-# @ref fj_movie_play_seq_4
	-# @ref fj_movie_play_seq_5
	-# @ref fj_movie_play_seq_6
	-# @ref fj_movie_play_seq_7
	-# @ref fj_movie_play_seq_8
	-# @ref fj_movie_play_seq_9
	-# @ref fj_movie_play_seq_10
	-# @ref fj_movie_play_seq_11
	<hr>

	@section fj_movie_play_seq_1 Video Play (Start-Loop-Stop)
	@image html fj_movie_play.png

	@section fj_movie_play_seq_2 Video Play (Stop File End)
	@image html fj_movie_play_file_end.png

	@section fj_movie_play_seq_3 Video Play Fwd
	@image html fj_movie_play_fwd.png

	@section fj_movie_play_seq_4 Video Play Rwd
	@image html fj_movie_play_rwd.png

	@section fj_movie_play_seq_5 Video Play Pause
	@image html fj_movie_play_pause.png

	@section fj_movie_play_seq_6 Video Play Resume (from pause)
	@image html fj_movie_play_resume_from_pause.png

	@section fj_movie_play_seq_7 Video Play Resume (from fast forward/backward -> pause)
	@image html fj_movie_play_resume_from_ff_rew-_pause.png

	@section fj_movie_play_seq_8 Video Play Resume (from fwd/rwd)
	@image html fj_movie_play_resume_from_fwdrwd.png

	@section fj_movie_play_seq_9 Video Play Step
	@image html fj_movie_play_step_playback.png

	@section fj_movie_play_seq_10 Video Time Search
	@image html fj_movie_play_time_search.png

	@section fj_movie_play_seq_11 Video Play Error
	@image html fi_movie_play_error.png

@}*//* --- end of fj_decoder_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_decoder_definition
@{*/
/* ON/OFF flag */
#define FJ_MOVIE_PLAY_OFF							0	/**< flag OFF. */
#define FJ_MOVIE_PLAY_ON							1	/**< flag ON. */

/* Codec type */
#define FJ_MOVIE_PLAY_CODEC_H264					0	/**< H264. */
#define FJ_MOVIE_PLAY_CODEC_H265					1	/**< H265. */
#define FJ_MOVIE_PLAY_CODEC_AAC						2	/**< AAC. */


/* For wOperation in FJ_VideoEncoder_Config() */
#define FJ_MOVIE_PLAY_VIDEO_SCAN_MODE_PROGRESSIVE	0	/**< progressive. */
#define FJ_MOVIE_PLAY_VIDEO_SCAN_MODE_INTERLACE		1	/**< interlace.(Not Supported) */

/* For conceal_error in T_FJ_MOVIE_PLAY_VIDEO_CONFIG_PARAM */
#define FJ_MOVIE_PLAY_VIDEO_CONCEAL_ERROR_STOP		0	/**< play stop with an error. */
#define FJ_MOVIE_PLAY_VIDEO_CONCEAL_ERROR_SKIP		1	/**< frame skip with an error. */


///////////////////////////////////////////////////////////////////////////////
// enumeration
///////////////////////////////////////////////////////////////////////////////
/** [connectivity] Video Decoder Type */
typedef enum {
	E_FJ_VIDEO_DEC_H264 = 0, 	/**< H.264. */
	E_FJ_VIDEO_DEC_H265 = 1,	/**< H.265. */
	E_FJ_VIDEO_DEC_NONE = 0xFFFF,
} E_FJ_VIDEO_DEC_ENUM;

/** Image Size */
typedef enum {
// --- REMOVE_60FPS BEGIN ---
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_4000_3000	= 0,	/**< 4000x3000 */
// --- REMOVE_60FPS END ---
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_4096_2304	= 1,	/**< 4096x2304 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_4096_2160	= 2,	/**< 4096x2160 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_4096_2048	= 3,	/**< 4096x2048 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_3840_2160	= 4,	/**< 3840x2160 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_3840_1920	= 5,	/**< 3840x1920 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_3000_3000	= 6,	/**< 3000x3000 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_2704_2028	= 7,	/**< 2704x2028 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_2704_1520	= 8,	/**< 2704x1520 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_2560_1920	= 9,	/**< 2560x1920 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_1440	= 10,	/**< 1920x1440 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_1080	= 11,	/**< 1920x1080 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_960		= 12,	/**< 1920x 960 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_1504_1504	= 13,	/**< 1504x1504 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_1440_1080	= 14,	/**< 1440x1080 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_1440_720		= 15,	/**< 1440x 720 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_960		= 16,	/**< 1280x 960 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_720		= 17,	/**< 1280x 720 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_864_480		= 18,	/**<  864x 480 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_848_480		= 19,	/**<  848x 480 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_720_480		= 20,	/**<  720x 480 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_640_480		= 21,	/**<  640x 480 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_640_360		= 22,	/**<  640x 360 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_432_240		= 23,	/**<  432x 240 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_320_240		= 24,	/**<  320x 240 */

	E_FJ_MOVIE_PLAY_VIDEO_SIZE_720_576		= 25,
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_1024_768		= 26,
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_768		= 27,
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_1024	= 28,
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_800_600		= 29,
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_2560_1440	= 30,	/**< 2560x1440 */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_960_540		= 31,
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_480_270		= 32,
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_1600_896		= 33,
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_1024_576		= 34,
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_800_448		= 35,
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_352_288		= 36,
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_2592_1944	= 37,
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_2304_1296	= 38,
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_960_480		= 39,
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_1280	= 40,
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_640_512		= 41,
	E_FJ_MOVIE_PLAY_VIDEO_SIZE_384_288		= 42,
} E_FJ_MOVIE_PLAY_VIDEO_SIZE;

/** For speed ing FJ_VideoPlayback_SetSpeedDirection() */
typedef enum {
	E_FJ_MOVIE_PLAY_SPEED_1X		= 0,	/**< Normal speed */
	E_FJ_MOVIE_PLAY_SPEED_2X		= 1,	/**<     2x speed */
	E_FJ_MOVIE_PLAY_SPEED_4X		= 2,	/**<     4x speed */
	E_FJ_MOVIE_PLAY_SPEED_8X		= 3,	/**<     8x speed */
	E_FJ_MOVIE_PLAY_SPEED_1_2X		= 4,	/**<   1/2x speed */
	E_FJ_MOVIE_PLAY_SPEED_1_4X		= 5,	/**<   1/4x speed */
	E_FJ_MOVIE_PLAY_SPEED_1_8X		= 6,	/**<   1/8x speed */
} E_FJ_MOVIE_PLAY_SPEED;

/** For direction in FJ_VideoPlayback_SetSpeedDirection() */
typedef enum {
	E_FJ_MOVIE_PLAY_DIRECTION_FWD	= 0,	/**< Direction is Forward */
	E_FJ_MOVIE_PLAY_DIRECTION_RWD	= 1,	/**< Direction is Rewind */
} E_FJ_MOVIE_PLAY_DIRECTION;

/** For last_picture in FJ_VideoPlayback_StopVideoDecode() */
typedef enum {
	E_FJ_MOVIE_PLAY_LAST_PICTURE_KEEP		= 0,	/**< Last picture is keep [fwd/rwd]*/
	E_FJ_MOVIE_PLAY_LAST_PICTURE_DONOT_KEEP	= 1,	/**< Last picture is do not keep(black display) [stop]*/
	E_FJ_MOVIE_PLAY_LAST_PICTURE_KEEP_STOP	= 2,	/**< Last picture is keep, but stop play [stop] */
} E_FJ_MOVIE_PLAY_LAST_PICTURE;


/** For buffer_type in FJ_VideoPlayback_MemAlloc() */
typedef enum {
	E_FJ_MOVIE_PLAY_BUFFER_TYPE_H264_STREAM		= 0,	/**< Buffer type is H.264 stream */
	E_FJ_MOVIE_PLAY_BUFFER_TYPE_H265_STREAM		= 1,	/**< Buffer type is H.265 stream */
	E_FJ_MOVIE_PLAY_BUFFER_TYPE_AAC_STREAM		= 2,	/**< Buffer type is AAC stream */
	E_FJ_MOVIE_PLAY_BUFFER_TYPE_PCM_STREAM		= 3,	/**< Buffer type is PCM stream */
} E_FJ_MOVIE_PLAY_BUFFER_TYPE;

/** Decoder Status */
typedef enum {
	E_FJ_MOVIE_PLAY_DECODER_STATUS_IDLE		= 0,	/**< Decoder status is idle */
	E_FJ_MOVIE_PLAY_DECODER_STATUS_NORMAL	= 1,	/**< Decoder status is normal */
	E_FJ_MOVIE_PLAY_DECODER_STATUS_LAST		= 2,	/**< Decoder status is last stream decoded */
} E_FJ_MOVIE_PLAY_DECODER_STATUS;

/** YUV format */
typedef enum {
	E_FJ_MOVIE_PLAY_YUV_FORMAT_VIDEO		= 0,	/**< Video format(420 fixed) */
	E_FJ_MOVIE_PLAY_YUV_FORMAT_YUV420		= 1,	/**< YUV420 format (Currently not used.) */
} E_FJ_MOVIE_PLAY_YUV_FORMAT;


/* AudioDecoder */
/** Audio Decode Type */
typedef enum {
	E_FJ_AUDIO_DEC_NONE_0 = 0x00, 		/**< Non-Decoding. */
	E_FJ_AUDIO_DEC_AAC,					/**< AAC-LC. */
} E_FJ_AUDIO_DEC_ENUM;

/** Audio Codec(Compress) type */
typedef enum {
	E_FJ_AUDIO_DEC_CODEC_PCM	= 0,	/**< PCM */
	E_FJ_AUDIO_DEC_CODEC_AAC,			/**< AAC */
} E_FJ_AUDIO_DEC_CODEC_TYPE;

///////////////////////////////////////////////////////////////////////////////
// typedef
///////////////////////////////////////////////////////////////////////////////

/** For video_config in FJ_VideoPlayback_SetVideoConfig() */
typedef struct{
	UINT8						codec_type;			/**< Select of Codec Type.<br><br>
													     FJ_MOVIE_PLAY_CODEC_H264         : 0   (H264) <br>
													     FJ_MOVIE_PLAY_CODEC_H265         : 1   (H265) <br> */
	E_FJ_MOVIE_PLAY_VIDEO_SIZE	image_size;			/**< Select of Image Size.<br><br>
// --- REMOVE_60FPS BEGIN ---
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_4000_3000  : 0   (4000x3000) <br>
// --- REMOVE_60FPS END ---
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_4096_2304  : 1   (4096x2304) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_4096_2160  : 2   (4096x2160) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_4096_2048  : 3   (4096x2048) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_3840_2160  : 4   (3840x2160) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_3000_3000  : 5   (3000x3000) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_2704_2028  : 6   (2704x2028) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_2704_1520  : 7   (2704x1520) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_2560_1920  : 8   (2560x1920) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_1440  : 9   (1920x1440) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_1080  : 10  (1920x1080) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_960   : 11  (1920x 960) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_1504_1504  : 12  (1504x1504) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_1440_1080  : 13  (1440x1080) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_1440_720   : 14  (1440x 720) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_960   : 15  (1280x 960) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_720   : 16  (1280x 720) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_864_480    : 17  ( 864x 480) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_848_480    : 18  ( 848x 480) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_720_480    : 19  ( 720x 480) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_640_480    : 20  ( 640x 480) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_640_360    : 21  ( 640x 360) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_432_240    : 22  ( 432x 240) <br>
														 E_FJ_MOVIE_PLAY_VIDEO_SIZE_320_240    : 23  ( 320x 240) <br> */
	UINT8						scan_mode;			/**< Select of scan mode.<br><br>
														 FJ_MOVIE_PLAY_VIDEO_SCAN_MODE_PROGRESSIVE : 0 (Progressive) <br>
														 FJ_MOVIE_PLAY_VIDEO_SCAN_MODE_INTERLACE   : 1 (Interlace (Not Supported)) <br> */
	UINT8						conceal_error;		/**< Select of operation at the time of the error.<br><br>
														FJ_MOVIE_PLAY_VIDEO_CONCEAL_ERROR_STOP : 0 (play stop with an error.) <br>
														FJ_MOVIE_PLAY_VIDEO_CONCEAL_ERROR_SKIP : 1 (frame skip with an error.) <br> */
	UINT32						time_scale;			/**< Time Scale. (ex. 29.97fps is 30000) <br> */
	UINT32						time_delta;			/**< Time Delta. (ex. 29.97fps is 1001)  <br> */
	BOOL						trick;				/**< Whether the trick play.<br>
														If set to FALSE, because it does not get the image bank necessary to reverse playback,<br>
														BaseFW can save the SDRAM.<br> */
	UINT8						compatibility_mode;	/**< Set of compatible playback mode of the codec.<br>
														FJ_MOVIE_PLAY_ON  : Can play other than the self-recording streams.
														FJ_MOVIE_PLAY_OFF : Can only play self-recording streams. <br> */
} T_FJ_MOVIE_PLAY_VIDEO_CONFIG_PARAM;


/** For video_stream_param in FJ_VideoPlayback_SetVideoStream() / FJ_VideoPlayback_StartIFrameDecoding() */
typedef struct{
	UINT8		stream_end_flag;	/**< Last stream = 1, Otherwise = 0. <br> */
	UINT8		stss;				/**< It exists in MP4 of stss atom = 1, Not exist = 0. <br> */
	UINT32		stream_size;		/**< Stream size. <br> */
	UINT32		stream_address;		/**< Stored stream address. <br> */
	ULLONG		pts;				/**< Presentation Time Stamp (ex. 1001) <br> */
	ULLONG		dts;				/**< Decoding Time Stamp (ex. 1001) <br> */
} T_FJ_MOVIE_PLAY_VIDEO_STREAM_PARAM;


/** For audio_stream_param in FJ_VideoPlayback_SetAudioStream() */
typedef struct{
	UINT8		stream_end_flag;	/**< Last stream = 1, Otherwise = 0. <br> */
	UINT32		stream_size;		/**< Stream size. <br> */
	UINT32		stream_address;		/**< Stored stream address. <br> */
	ULLONG		pts;				/**< Presentation Time Stamp (ex. 1001) <br> */
} T_FJ_MOVIE_PLAY_AUDIO_STREAM_PARAM;


/** YUV parameter */
typedef struct{
	UINT32						y_addr;			/**< Y    data stored address. <br> */
	UINT32						cbcr_addr;		/**< CbCr data stored address. <br> */
	UINT32						g_width;		/**< global horizontal size. <br> */
	UINT32						g_lines;		/**< global vertical size. <br> */
	UINT32						width;			/**< horizontal size. <br> */
	UINT32						lines;			/**< vertical size. <br> */
	E_FJ_MOVIE_PLAY_YUV_FORMAT	yuv_format;		/**< YUV format. <br> */
} T_FJ_MOVIE_PLAY_VIDEO_YUV_INFO;

/* AudioDecoder */
/** AudioDecoder config parameter */
typedef struct{
	E_FJ_AUDIO_DEC_CODEC_TYPE		codec_type;		/**< Select of Codec Type.<br>*/
	UINT8		audio_ch;		/**< Channel type(Mono is FJ_AUDIO_NUM_CHANNEL_1. Stereo is FJ_AUDIO_NUM_CHANNEL_2.).<br> */
	UINT32		sampling_rate;	/**< Sampling rate. (ex. 48KHz is 48000) <br> */
} T_FJ_AUDIO_DEC_CONFIG_PARAM;


/*@} --- end of fj_decoder_definition */

///////////////////////////////////////////////////////////////////////////////
// macro
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_decoder_api
@{*/

///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function open Video Decoder instance.<br>
@param[in]	video_dec		Video Decoder.<br>
							[E_FJ_VIDEO_DEC_H264, E_FJ_VIDEO_DEC_H265]<br>
@param[out]	video_decoder_id	Video Decoder ID (SW instance ID).<br>
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred. *it already open instance, we return this error.
*/
extern	FJ_ERR_CODE	FJ_VideoDecoder_Open( E_FJ_VIDEO_DEC_ENUM video_dec, UCHAR* video_decoder_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function close Video Decoder instance.<br>
@param[in]	video_decoder_id	Video Decoder ID (SW instance ID).<br>
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred.
*/
extern	FJ_ERR_CODE	FJ_VideoDecoder_Close( UCHAR video_decoder_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function connects Video Decoder Instance with Display Instance.
@param[in]	video_decoder_id	VideoDecoder Instance ID.<br>
@param[in]	display_id			Display Instance ID.<br>
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
extern	FJ_ERR_CODE	FJ_VideoDecoder_Connect( UCHAR video_decoder_id, UCHAR display_id, BOOL sub_inst );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function Disconnects Video Decoder Instance with Display Instance.
@param[in]	video_decoder_id	VideoDecoder Instance ID.<br>
@param[in]	display_id			Display Instance ID.<br>
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
extern	FJ_ERR_CODE	FJ_VideoDecoder_Disconnect( UCHAR video_decoder_id, UCHAR display_id, BOOL sub_inst );



/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Initialize the Video Process. (global variable, Management information of Video stream area, etc...)

@retval FJ_ERR_OK : Success.
@retval FJ_ERR_NG : Error occurred.
@remarks Please call this API before Video Codec Setup by all means.
*/
extern	FJ_ERR_CODE	FJ_VideoPlayback_VideoInit( VOID );


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Initialize the Audio Process. (global variable, Management information of Audio stream area, etc...)

@retval FJ_ERR_OK : Success.
@retval FJ_ERR_NG : Error occurred.
@remarks Please call this API before Audio Codec Setup by all means.
*/
extern	FJ_ERR_CODE	FJ_VideoPlayback_AudioInit( VOID );


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Configure the Video decoder parameter.

@param [in] video_config			Set the configuration of Video Decoder.
@retval FJ_ERR_OK : Success.
@retval FJ_ERR_NG : Error occurred.
*/
extern	FJ_ERR_CODE	FJ_VideoPlayback_SetVideoConfig( T_FJ_MOVIE_PLAY_VIDEO_CONFIG_PARAM* video_config );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set the PTS to start playback. <br>
Do the AV sync using this PTS.

@param [in] start_pts				PTS to start playback.
@retval FJ_ERR_OK : Success.
@retval FJ_ERR_NG : Error occurred.
*/
extern	FJ_ERR_CODE	FJ_VideoPlayback_SetStartPTS( ULLONG start_pts );


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set the speed and direction for the playback. <br>
Using this setting, determine the playback methods as normal play / fast-forward / rewind.

@param [in] speed					Playback speed.
@param [in] direction				Playback direction.
@param [in] pause					If set TRUE, display the only 1frame after decode start.
									After that, the state of the decoder will transition to Pause.
									(Use in frame playback.)
@retval FJ_ERR_OK : Success.
@retval FJ_ERR_NG : Error occurred.
*/
extern	FJ_ERR_CODE	FJ_VideoPlayback_SetSpeedDirection( E_FJ_MOVIE_PLAY_SPEED speed, E_FJ_MOVIE_PLAY_DIRECTION direction, BOOL pause );


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set the Video stream that Host has read from the media to BaseFW. <br>

@param [in] store_num				The number of stream parameter that is stored.
@param [in] video_stream_param		Parameters of the video stream.
@retval FJ_ERR_OK : Success.
@retval FJ_ERR_NG : Error occurred.
@retval FJ_ERR_OVERFLOW : No management table space available.
*/
extern	FJ_ERR_CODE	FJ_VideoPlayback_SetVideoStream( E_FJ_VIDEO_DEC_ENUM codec, UINT32 store_num, T_FJ_MOVIE_PLAY_VIDEO_STREAM_PARAM* video_stream_param );


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set the Audio stream that Host has read from the media to BaseFW.

@param [in] store_num				The number of stream parameter that is stored.
@param [in] audio_stream_param		Parameters of the audio stream.
@retval FJ_ERR_OK : Success.
@retval FJ_ERR_NG : Error occurred.
@retval FJ_ERR_OVERFLOW : No management table space available.
*/
extern	FJ_ERR_CODE	FJ_VideoPlayback_SetAudioStream( UINT32 store_num, T_FJ_MOVIE_PLAY_AUDIO_STREAM_PARAM* audio_stream_param );


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Start the Decode & Display of Video.

@retval FJ_ERR_OK : Success.
@retval FJ_ERR_NG : Error occurred.
*/
extern	FJ_ERR_CODE	FJ_VideoPlayback_StartVideoDecode( E_FJ_VIDEO_DEC_ENUM codec );


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Start the Decode of Audio.

@retval FJ_ERR_OK : Success.
@retval FJ_ERR_NG : Error occurred.
*/
extern	FJ_ERR_CODE	FJ_VideoPlayback_StartAudioDecode( VOID );


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Start the Output of Audio.

@retval FJ_ERR_OK : Success.
@retval FJ_ERR_NG : Error occurred.
*/
extern	FJ_ERR_CODE	FJ_VideoPlayback_StartAudioOutput( VOID );


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Stop the Output of Audio.

@retval FJ_ERR_OK : Success.
@retval FJ_ERR_NG : Error occurred.
*/
extern	FJ_ERR_CODE	FJ_VideoPlayback_StopAudioOutput( VOID );


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Stop the Decode & Display of Video.

@param [in] last_picture			Display method of the last picture after stop. (Remains of the last picture, or black image)
@retval FJ_ERR_OK : Success.
@retval FJ_ERR_NG : Error occurred.
@retval FJ_ERR_MOVIE_ALREADY_STOP_PLAY : VideoPlayback is already stopped.
*/
extern	FJ_ERR_CODE	FJ_VideoPlayback_StopVideoDecode( E_FJ_VIDEO_DEC_ENUM codec, E_FJ_MOVIE_PLAY_LAST_PICTURE last_picture );


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Stop the Decode of Audio.

@retval FJ_ERR_OK : Success.
@retval FJ_ERR_NG : Error occurred.
*/
extern	FJ_ERR_CODE	FJ_VideoPlayback_StopAudioDecode( VOID );


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Pause the Decode & Display of Video.

@retval FJ_ERR_OK : Success.
@retval FJ_ERR_NG : Error occurred.
*/
extern	FJ_ERR_CODE	FJ_VideoPlayback_PauseVideo( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Resume Decode & Display of Video.

@retval FJ_ERR_OK : Success.
@retval FJ_ERR_NG : Error occurred.
*/
extern	FJ_ERR_CODE	FJ_VideoPlayback_ResumeVideo( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get the display frame(YUV).

@param [in,out] display_yuv_info		Information of the YUV .
@retval FJ_ERR_OK : Success.
@retval FJ_ERR_NG : Error occurred.
*/
extern	FJ_ERR_CODE	FJ_VideoPlayback_GetDisplayFrame( T_FJ_MOVIE_PLAY_VIDEO_YUV_INFO* display_yuv_info );


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Decode the Iframe.

@param [in]  display				Display control after decoding.<br>
									TRUE : display. (Display by BaseFW)<br>
									FALSE : not display. (Display by HostFW)
@param [in]  req_pts				The PTS of the request frame.
@param [in]  store_num				The number of stream parameter that is stored.
@param [in]  video_stream_param		Parameters of the video stream.
@param [in,out] decode_yuv_info		Information of the YUV from display image.
@retval FJ_ERR_OK : Success.
@retval FJ_ERR_NG : Error occurred.
*/
extern	FJ_ERR_CODE	FJ_VideoPlayback_StartIFrameDecoding( BOOL display, ULLONG req_pts, UINT32 store_num, T_FJ_MOVIE_PLAY_VIDEO_STREAM_PARAM* video_stream_param, T_FJ_MOVIE_PLAY_VIDEO_YUV_INFO* decode_yuv_info);


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get the stream area of Video / Audio.

@param [in]      buffer_type		Type of stream buffer.
@param [in]      size				Request buffer size.
@param [out]     address			Address of the acquired buffer.

@retval FJ_ERR_OK : Success.
@retval FJ_ERR_NG : There is no free space in the buffer.
*/
extern	FJ_ERR_CODE	FJ_VideoPlayback_MemAlloc( E_FJ_MOVIE_PLAY_BUFFER_TYPE buffer_type, UINT32* size, UINT32* address );


/* AudioDecoder */
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function open Audio Decoder instance.<br>
@param[in]	audio_dec		Audio Decoder.<br>
							[E_FJ_AUDIO_DEC_NONE_0, E_FJ_AUDIO_DEC_AAC]<br>
@param[out]	audio_dec_id	Audio Decoder ID (SW instance ID).<br>
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred. *it already open instance, we return this error.
*/
extern	FJ_ERR_CODE	FJ_AudioDecoder_Open( E_FJ_AUDIO_DEC_ENUM audio_dec, UCHAR * audio_dec_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function close Audio Decoder instance.<br>
@param[in]	audio_dec_id	Audio Decoder ID (SW instance ID).<br>
@retval		FJ_ERR_OK	Normal end.
@retval		FJ_ERR_NG	Error occurred.
*/
extern	FJ_ERR_CODE	FJ_AudioDecoder_Close( UCHAR audio_dec_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function connects Audio Decoder Instance with Audio Out Instance.
@param[in]	audio_dec_id	AudioDecoder Instance ID.<br>
@param[in]	audio_out_id		AudioOut Instance ID.<br>
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
extern	FJ_ERR_CODE	FJ_AudioDecoder_Connect( UCHAR audio_dec_id, UCHAR audio_out_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function Disconnects Audio Decoder Instance with Audio Out Instance.
@param[in]	audio_dec_id		AudioDecoder Instance ID.<br>
@param[in]	audio_out_id		AudioOut Instance ID.<br>
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
*/
extern	FJ_ERR_CODE	FJ_AudioDecoder_Disconnect( UCHAR audio_dec_id, UCHAR audio_out_id );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function configure the audio decoder parameter.
@param[in]	audio_dec_id	Audio Decoder ID.<br>
@param[in]	audio_dec_config	audio decoder parameter<br>
@retval		FJ_ERR_OK Normal End
@retval		FJ_ERR_NG Abnormal End
@retval		FJ_ERR_NG_INPUT_PARAM Parameter error.
@remarks This API is the same function as the FJ_VideoPlayback_SetAudioConfig().<br><br>

Sample code is as follows.<br>
@code
extern FJ_ERR_CODE error_code;
T_FJ_AUDIO_DEC_CONFIG_PARAM dec_config;

dec_config.codec_type = E_MOVIE_AUDIO_CODEC_AAC;
dec_config.audio_ch = FJ_AUDIO_NUM_CHANNEL_2;
dec_config.sampling_rate = 44100;

error_code = FJ_AudioDecoder_Config(audio_dec_id, &dec_config);
@endcode
*/
extern	FJ_ERR_CODE	FJ_AudioDecoder_Config( UCHAR audio_dec_id, T_FJ_AUDIO_DEC_CONFIG_PARAM * audio_dec_config );


/*@} --- end of fj_decoder_api */

// @cond
///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_decoder_message
@{*/
/**
@def FJ_HM_MOVIE_H264_CODEC_ERROR
This message means that there are any issues in the h264 codec.<br>
*/


/*@} --- end of fj_decoder_message */
// @endcond

///////////////////////////////////////////////////////////////////////////////
// callback
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_decoder_callback
@{*/
/*@} --- end of fj_decoder_callback */

#endif
