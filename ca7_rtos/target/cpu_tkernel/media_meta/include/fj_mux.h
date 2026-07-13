/**
 * @file		fj_mux.h
 * @brief		Mux Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_MUX_H_
#define _FJ_MUX_H_

#include "ddim_typedef.h"
#include "fj_std.h"
#include "fj_encoder.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @addtogroup fj_mux_overview
@{
	The MUX controls can be achieved by using this API.<br>
@}*//* --- end of fj_mux_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @addtogroup fj_mux_sequence
@{
	Please refer BaseFWPF Specification.
@}*//* --- end of fj_mux_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_mux_definition
@{*/
// --- REMOVE_WIFI_EMMC_SAVE BEGIN ---
#ifndef CO_WIFI_EMMC_SAVE
#define	CO_WIFI_EMMC_SAVE
#endif
// --- REMOVE_NO_WIFI_EMMC_SAVE BEGIN ---
#ifdef CO_WIFI_EMMC_SAVE
#undef	CO_WIFI_EMMC_SAVE
#endif
// --- REMOVE_NO_WIFI_EMMC_SAVE END ---
// --- REMOVE_WIFI_EMMC_SAVE END ---

/* ON/OFF flag */
#define FJ_MUX_OFF						0	/**< flag OFF. */
#define FJ_MUX_ON						1	/**< flag OFF. */

#define D_BF_MUX_HVCC_ARRAY_NUM_MAX		(3)		// VPS + SPS + PPS = 3.
#define D_BF_MUX_HVCC_NAL_NUM_MAX		(10)

#define D_BF_MUX_H265_NALTYPE_AUD		(35)
#define D_BF_MUX_H265_NALTYPE_VPS		(32)
#define D_BF_MUX_H265_NALTYPE_SPS		(33)
#define D_BF_MUX_H265_NALTYPE_PPS		(34)

/* Function Result*/
#define FJ_OK_MP4_COMMON_OK			( 0)		// OK
#define FJ_OK_MAX_TIME_END			( 1)		// Normal End(MaxTime)
#define FJ_OK_MAX_SIZE_END			( 2)		// Normal End(MaxSize)
#define FJ_OK_MEDIAFULL_END			( 3)		// Normal End(Media Full)
#define FJ_ERROR_MP4_COMMON_NG		( 4)		// NG

//Memory Controller Update Opt
#define FJ_MUX_MEMOPT_ADD			( 0)		// Add New File
#define FJ_MUX_MEMOPT_DELETE		( 1)		// Delete 

///////////////////////////////////////////////////////////////////////////////
// enumeration
///////////////////////////////////////////////////////////////////////////////
/** Mux CH (for multi encoding) */
typedef enum {
	E_FJ_MUX_CH_0	= 0,					/**< Mux CH0 */
	E_FJ_MUX_CH_1,							/**< Mux CH1 */
	E_FJ_MUX_CH_2,							/**< Mux CH2 */
	E_FJ_MUX_CH_3,							/**< Mux CH3 */	
	E_FJ_MUX_CH_MAX							/**< Mux CH MAX */
} E_FJ_MUX_CH;


///////////////////////////////////////////////////////////////////////////////
// typedef
///////////////////////////////////////////////////////////////////////////////
/** Mux Video config parameter */
typedef struct{
	BOOL						enable;						/**< Mux Enable Flag. */
	INT32						pre_file_id;				/**< Mux PreFileID. */
	INT32						file_id;					/**< Mux FileID. */
	UINT32						stream_id;					/**< Mux StreamID */
	UINT8						codec_type;					/**< Mux Codec type. <br>
																 FJ_MOVIE_CODEC_H264 : H264 Codec. <br>
																 FJ_MOVIE_CODEC_H265 : H265 Codec. <br> */
	E_FJ_MOVIE_VIDEO_SIZE		image_size;					/**< Mux stream video size <br> */
	E_FJ_MOVIE_VIDEO_SIZE		display_size;				/**< Mux stream video display size <br> */
	ULONG						bitrate;					/**< Bitrate. <br>
																 Please Set Value of Bitrate.<br>
																 (ex. 24Mbps -> D'24,000,000) *However, the values under 1Kbps is truncated.<br>
																 Case of CBR, this parameter means "target bitrate".<br>
																 Case of VBR, this parameter means "MAX bitrate". <br> */
    ULONG   					time_increment;				/**< Video TimeIncrement Value. */
	ULONG   					time_scale;					/**< Video TimeScale Value. */
	UINT32						gop_struct;					/**< Stream GOP Struct.<br><br>
																 FJ_MOVIE_VIDEO_GOP_STRUCT_IPPP : 0 (B-frame Distance = 0)<br>
																 FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP : 1 (B-frame Distance = 2)<br>
																 FJ_MOVIE_VIDEO_GOP_STRUCT_IIII : 2 (I-frame Only) (It support only when GOP num is FJ_MOVIE_VIDEO_GOP_NUM_1 or FJ_MOVIE_VIDEO_GOP_NUM_2.)<br> */
	UINT32						frame_num_of_gop;			/**< The number of frames of GOP. */
	ULONG						max_time;					/**< Maximum Mux Time Limitation. (sec) */
	ULONG						max_size;					/**< Maximum Mux Size Limitation. (byte) */
	ULONG						compatible_brand_count;		/**< Compatible Brand count (0~3) */
	UINT32						compatible_brand[3];		/**< Compatible Brand string for FTYP atom. (4byte) */
} T_FJ_MUX_VIDEO_CONFIG;

/** Mux Audio config parameter */
typedef struct{
	BOOL						enable;						/**< Mux Enable Flag. */
	UINT8						codec_type;					/**< Mux Codec type. (Now is not use.)*/
	UINT8						channel;					/**< Channel num(Mono is 1. Stereo is 2.)*/
	UINT32						samplingrate;				/**< Sampling rate */
	UINT32						bitrate;					/**< Bit rate */
} T_FJ_MUX_AUDIO_CONFIG;

/** MP4 atom string parameter */
typedef struct{
	UINT32						major_brand;				/**< Major Brand string for FTYP atom. (4byte) */
	USHORT						major_version;				/**< Major Version number for FTYP atom. (2byte) . */
	USHORT						minor_version;				/**< Minor Version number for FTYP atom. (2byte) . */
	CHAR						video_handler_name[64];		/**< Video Handler Name string for HDLR atom. (Max 64byte) */
	CHAR						audio_handler_name[64];		/**< Audio Handler Name string for HDLR atom. (Max 64byte) */
} T_FJ_MUX_MP4_STRING_CONFIG;

/** Mux parameter */
typedef struct{
	T_FJ_MUX_AUDIO_CONFIG		audio[2];						/**< Audio Mux Config */
	T_FJ_MUX_VIDEO_CONFIG		video[E_FJ_MUX_CH_MAX];		/**< Video Mux Config */
	T_FJ_MUX_MP4_STRING_CONFIG	string;						/**< MP4 string information */
} T_FJ_MUX_CONFIG;

/** Stream SPS/PPS parameter for H.264 */
typedef struct {
	UINT32						sps_size;					/**< SPS info Size of Video Stream Header. */
	UCHAR*						sps_address;				/**< SPS info Address of Video Stream Header. */
	UINT32						pps_size;					/**< PPS info Size of Video Stream Header. */
	UCHAR*						pps_address;				/**< PPS info Address of Video Stream Header. */
} T_FJ_MUX_H264_SPS_PPS_INFO;

/** Stream PS parameter for H.265 */

typedef struct {
	BOOL						array_completeness;
	UINT32						nal_type;
	UINT32						nal_num;
	UINT32						nal_address[D_BF_MUX_HVCC_NAL_NUM_MAX];
	UINT32						nal_size[D_BF_MUX_HVCC_NAL_NUM_MAX];
} T_FJ_MUX_H265_PS_INFO_ARRAY;

typedef struct {
	UINT32						array_num;
	T_FJ_MUX_H265_PS_INFO_ARRAY	array_elements[D_BF_MUX_HVCC_ARRAY_NUM_MAX];
} T_FJ_MUX_H265_PS_INFO;

/*@} --- end of fj_mux_definition */

///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_mux_api
@{*/

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function muxer initialize and create file(MP4) <br>
@param[in]	config				Mux configuration.<br>
@retval		FJ_ERR_OK			Normal end.
@retval		FJ_ERR_NG			Error occurred.
*/
extern	FJ_ERR_CODE	FJ_Mux_Initialize( T_FJ_MUX_CONFIG* config );


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function muxer fixed atom inforamtion setting(MP4) <br>
@param[in]	stream_id			stream id. (same value as that set in FJ_Mux_Initialize).<br>
@param[in]	sps_pps_info		SPS/PPS information.(use H.264)<br>
@param[in]	ps_info				PS information.(use H.265)<br>
@retval		FJ_ERR_OK			Normal end.
@retval		FJ_ERR_NG			Error occurred.
*/
extern	FJ_ERR_CODE	FJ_Mux_Fixed_Atom_Setting( UINT32 stream_id, T_FJ_MUX_H264_SPS_PPS_INFO* sps_pps_info, T_FJ_MUX_H265_PS_INFO* ps_info );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function video add sample (MP4) <br>
@param[in]	stream_id			stream id. (same value as that set in FJ_Mux_Initialize).<br>
@param[in]	sample_address		sample address.<br>
@param[in]	sample_size			sample size.<br>
@param[in]	frame_type			frame type.<br>
@param[in]	ct_offset			ctts offset value. (pts-dts).<br>
@param[in]	end_chunk_flg		end chunk excute flag.<br>
@param[in]	end_flg				last add sample flag.<br>
@retval		FJ_ERR_OK			Normal end.
@retval		FJ_ERR_NG			Error occurred.
*/
extern	INT32	FJ_Mux_Video_Add_Sample(
		UINT32		stream_id,
		UCHAR*		sample_address,
		ULONG		sample_size,
		ULONG		ct_offset,
		UINT32		frame_type,
		UCHAR		end_chunk_flg,
		UCHAR		end_flg
		);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function audio add sample (MP4) <br>
@param[in]	area_index			audio area index.<br>
@param[in]	sample_address		sample address.<br>
@param[in]	sample_size			sample size.<br>
@param[in]	duration			sample duration. (At a sampling rate conversion).<br>
@retval		FJ_ERR_OK			Normal end.
@retval		FJ_ERR_NG			Error occurred.
*/
extern	FJ_ERR_CODE	FJ_Mux_Audio_Add_Sample( UCHAR stream_id, ULONG area_index, BYTE* sample_address, ULONG sample_size, ULONG duration );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function video add free atom (MP4) <br>
@param[in]	stream_id			stream id. (same value as that set in FJ_Mux_Initialize).<br>
@param[in]	data_address		data address.<br>
@param[in]	data_size			data size.<br>
@retval		FJ_ERR_OK			Normal end.
@retval		FJ_ERR_NG			Error occurred.
@remarks Export of free atom is done in the finalization, but the data that has been set in this API is not copied (protection).<br>
         Please note that there is a need to protect the data until the finalization is complete.<br>
*/
FJ_ERR_CODE FJ_Mux_Add_FreeAtom( UINT32 stream_id, UCHAR* data_address, ULONG data_size );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function mux finalize(MP4) <br>
@param[in]	stream_id			stream id. (same value as that set in FJ_Mux_Initialize).<br>
@retval		FJ_ERR_OK			Normal end.
@retval		FJ_ERR_NG			Error occurred.
*/
extern	FJ_ERR_CODE	FJ_Mux_Finalize( UINT32 stream_id );

// --- REMOVE_WIFI_EMMC_SAVE BEGIN ---
#ifdef CO_WIFI_EMMC_SAVE
extern	FJ_ERR_CODE FJ_Mux_WIFI_Out_EMMC( BOOL enable );
#endif
// --- REMOVE_WIFI_EMMC_SAVE END ---

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function creat a new file(MP4) <br>
@param[in]	stream_id			stream id. (same value as that set in FJ_Mux_Initialize).<br>
@param[in]	file_id				file id.<br>
@retval		FJ_ERR_OK			Normal end.
@retval		FJ_ERR_NG			Error occurred.
*/
extern	FJ_ERR_CODE FJ_Mux_Atom_Creat(UINT32 stream_id , INT32 file_id);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set movie file old(MP4) <br>
@param[in]	stream_id			stream id. (same value as that set in FJ_Mux_Initialize).<br>
*/
extern	FJ_ERR_CODE FJ_Mux_Movie_File_Old_Set(UINT32 stream_id);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function close file(MP4) <br>
@param[in]	stream_id			stream id. (same value as that set in FJ_Mux_Initialize).<br>
@retval		FJ_ERR_OK			Normal end.
@retval		FJ_ERR_NG			Error occurred.
*/
extern	FJ_ERR_CODE FJ_Mux_Close_File( UINT32 stream_id );

/*@} --- end of fj_mux_api */

#endif	// _FJ_MUX_H_
