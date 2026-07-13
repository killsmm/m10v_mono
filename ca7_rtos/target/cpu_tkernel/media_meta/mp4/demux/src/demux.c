/**
 * @file		demux.c
 * @brief		Demux Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "os_user_custom.h"
#include "demux.h"
#include "mp4_common.h"
#include "fj_decoder.h"
#include "fj_host.h"
#include "fj_filesystem.h"
#if ( CO_MOVIE_FF_AUDIO_PATTERN == 0 )
#include "libmp4f.h"
#else	// CO_MOVIE_FF_AUDIO_PATTERN
#include "libmovf.h"
#endif	// CO_MOVIE_FF_AUDIO_PATTERN

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
//#define CO_DEBUG_DEMUX_SHOW_HVCC_DETAILS

#define D_BF_DEMUX_TRACK_INVALID						(0xFFFFFFFF)
#define D_BF_DEMUX_TRACK_MAX							(2)
#define D_BF_DEMUX_HVCC_NAL_NUM_MAX						(15)
#define D_BF_DEMUX_H265_NALTYPE_AUD						(35)

#define FLG_DEMUX_VIDEO_SETUP_REQUEST					(0x00000001)
#define FLG_DEMUX_VIDEO_SETUP_COMPLETE					(0x00000002)
#define FLG_DEMUX_AUDIO_SETUP_REQUEST					(0x00000004)
#define FLG_DEMUX_AUDIO_SETUP_COMPLETE					(0x00000008)
#define FLG_DEMUX_FREE_ATOM_SIZE_GET_REQUEST			(0x00000010)
#define FLG_DEMUX_FREE_ATOM_SIZE_GET_COMPLETE			(0x00000020)
#define FLG_DEMUX_FREE_ATOM_GET_REQUEST					(0x00000040)
#define FLG_DEMUX_FREE_ATOM_GET_COMPLETE				(0x00000080)
#define FLG_DEMUX_VIDEO_STREAM_READ_REQUEST				(0x00000100)
//#define FLG_DEMUX_VIDEO_STREAM_READ_COMPLETE			(0x00000200)	// Currently unused.
#define FLG_DEMUX_AUDIO_STREAM_READ_REQUEST				(0x00000400)
#define FLG_DEMUX_AUDIO_STREAM_READ_COMPLETE			(0x00000800)
#define FLG_DEMUX_VIDEO_STREAM_SYNC_READ_REQUEST		(0x00001000)
#define FLG_DEMUX_VIDEO_STREAM_SYNC_READ_COMPLETE		(0x00002000)
#define FLG_DEMUX_AUDIO_STREAM_SYNC_READ_REQUEST		(0x00004000)
#define FLG_DEMUX_AUDIO_STREAM_SYNC_READ_COMPLETE		(0x00008000)
#define FLG_DEMUX_FINALIZE_REQUEST						(0x00010000)
#define FLG_DEMUX_FINALIZE_COMPLETE						(0x00020000)

#define D_BF_DEMUX_FEED_VIDEO_STREAM_MAX				(120)
#define D_BF_DEMUX_FEED_AUDIO_STREAM_MAX				(24)

#define D_BF_DEMUX_HEADER_READ_AREA_SIZE				(0x80000)	// 512KB

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
typedef struct{
	// --- track information ---
	ULONG						track_id;
	ULONG						handler_type;
	ULONG						track_flags;
	ULONG						time_scale;
	ULONG						time_delta;
	ULONG						frame_rate_multiply_100;
	ULONG						total_sample_num;
	ULONG						total_duration;
#if ( CO_MOVIE_FF_AUDIO_PATTERN == 0 )
	UCHAR						language[3];
#else
	UINT16						language;
#endif
	// --- sample description ---
	ULONG						dref_entry_id;
	ULONG						dref_type;
	ULONG						stsd_entry_count;
	ULONG						stsd_entry_id;
	ULONG						descr_type;
} T_BF_DEMUX_TRACK_INFO;


// SPS/PPS Infomation by H.264 avcC
typedef struct {
	ULONG						sps_size;
	UCHAR*						sps_address;
	ULONG						pps_size;
	UCHAR*						pps_address;
} T_BF_DEMUX_H264_SPS_PPS_INFO;


// NAL Infomation by H.265 hvcC
typedef struct {
	UINT32						total_nal_num;
	UINT32						total_nal_size;
	UINT32						nal_size[D_BF_DEMUX_HVCC_NAL_NUM_MAX];
	UINT8*						nal_address[D_BF_DEMUX_HVCC_NAL_NUM_MAX];
} T_BF_DEMUX_H265_PS_INFO;


// Message
typedef struct {
	BOOL					use_flg;
	ULLONG					start_pts;
	E_FJ_DEMUX_DIRECTION	direction;
	E_FJ_DEMUX_SPEED		speed;
} T_BF_DEMUX_VIDEO_SETUP_MESSAGE;

typedef struct {
	BOOL					use_flg;
	ULLONG					start_pts;
} T_BF_DEMUX_AUDIO_SETUP_MESSAGE;

typedef struct {
	BOOL					use_flg;
	UINT32					data_size;
} T_BF_DEMUX_FREE_ATOM_SIZE_GET_MESSAGE;

typedef struct {
	BOOL					use_flg;
	UINT32					data_address;
} T_BF_DEMUX_FREE_ATOM_GET_MESSAGE;

typedef struct {
	BOOL					use_flg;
	E_FJ_DEMUX_CODEC		codec;
	UINT32					read_stream_num;
} T_BF_DEMUX_VIDEO_STREAM_READ_MESSAGE;

typedef struct {
	BOOL					use_flg;
	E_FJ_DEMUX_CODEC		codec;
	UINT32					read_stream_num;
} T_BF_DEMUX_AUDIO_STREAM_READ_MESSAGE;

typedef struct {
	BOOL					use_flg;
	E_FJ_DEMUX_CODEC		codec;
	UINT32					read_stream_num;
	T_FJ_DEMUX_STREAM_PARAM	stream_param;
} T_BF_DEMUX_VIDEO_STREAM_SYNC_READ_MESSAGE;

typedef struct {
	BOOL					use_flg;
	E_FJ_DEMUX_CODEC		codec;
	UINT32					read_stream_num;
	T_FJ_DEMUX_STREAM_PARAM	stream_param;
} T_BF_DEMUX_AUDIO_STREAM_SYNC_READ_MESSAGE;


typedef struct {
	UINT32										file_id;
	VOID*										mp4f_handle;
	VOID*										header_area_p;
	UINT32										video_id;
	UINT32										audio_id;
	T_BF_DEMUX_TRACK_INFO						track_info[D_BF_DEMUX_TRACK_MAX];
	// Video Parameter
	E_FJ_DEMUX_CODEC							video_codec;
	ULONG										track_width;
	ULONG										track_height;
	USHORT										stream_width;
	USHORT										stream_height;
	T_BF_DEMUX_H264_SPS_PPS_INFO				sps_pps_info;
	T_BF_DEMUX_H265_PS_INFO						ps_info;
	// Audio Parameter
	E_FJ_DEMUX_CODEC							audio_codec;
	UINT32										sampling_rate;
	UINT32										audio_bit_rate;
	UINT32										audio_ch;
	// Control Data
	E_FJ_DEMUX_DIRECTION						direction;
	E_FJ_DEMUX_SPEED							speed;
	BOOL										first_video_flag;
	UINT32										video_pts_offset;
	ULLONG										get_video_stream_pts;
	ULLONG										get_audio_stream_pts;
	UINT32										video_gop_num;
	BOOL										b_frame_exist;
	BOOL										only_i_frame;
	ULLONG										video_last_pts;
	ULLONG										video_play_last_pts;
	ULLONG										video_ff_last_pts;
	ULLONG										video_rwd_last_pts;
	ULLONG										video_reverse_last_pts;
	ULLONG										audio_last_pts;
	// Message Data
	T_BF_DEMUX_VIDEO_SETUP_MESSAGE				video_setup_message;
	T_BF_DEMUX_AUDIO_SETUP_MESSAGE				audio_setup_message;
	T_BF_DEMUX_FREE_ATOM_SIZE_GET_MESSAGE		free_atom_size_get_message;
	T_BF_DEMUX_FREE_ATOM_GET_MESSAGE			free_atom_get_message;
	T_BF_DEMUX_VIDEO_STREAM_READ_MESSAGE		video_stream_read_message;
	T_BF_DEMUX_AUDIO_STREAM_READ_MESSAGE		audio_stream_read_message;
	T_BF_DEMUX_VIDEO_STREAM_SYNC_READ_MESSAGE	video_stream_sync_read_message;
	T_BF_DEMUX_AUDIO_STREAM_SYNC_READ_MESSAGE	audio_stream_sync_read_message;
} T_BF_DEMUX_INFO;

// SPS Information
typedef struct{
	INT32 profile_idc;
	INT32 constraint_set0_flag;
	INT32 constraint_set1_flag;
	INT32 constraint_set2_flag;
	INT32 constraint_set3_flag;
	INT32 reserved_zero_4bits;
	INT32 level_idc;
	INT32 seq_parameter_set_id;
	INT32 chroma_format_idc;
	INT32 separate_colour_plane_flag;
	INT32 bit_depth_luma_minus8;
	INT32 bit_depth_chroma_minus8;
	INT32 qpprime_y_zero_transform_bypass_flag;
	INT32 seq_scaling_matrix_present_flag;
	INT32 seq_scaling_list_present_flag[8];
	INT32 ScalingList4x4[6][16];
	INT32 UseDefaultScalingMatrix4x4Flag[6];
	INT32 ScalingList8x8[2][64];
	INT32 UseDefaultScalingMatrix8x8Flag[2];
	INT32 log2_max_frame_num_minus4;
	INT32 pic_order_cnt_type;
	INT32 log2_max_pic_order_cnt_lsb_minus4;
	INT32 delta_pic_order_always_zero_flag;
	INT32 offset_for_non_ref_pic;
	INT32 offset_for_top_to_bottom_field;
	INT32 num_ref_frames_in_pic_order_cnt_cycle;
	INT32 offset_for_ref_frame[256];
	INT32 num_ref_frames;
	INT32 gaps_in_frame_num_value_allowed_flag;
	INT32 pic_width_in_mbs_minus1;
	INT32 pic_height_in_map_units_minus1;
	INT32 frame_mbs_only_flag;
} T_BF_DEMUX_H264_SPS;


/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
static	volatile	T_BF_DEMUX_INFO		gDmux_info = { .file_id = 0 };


/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
// memory control
static	VOID	bf_demux_memory_close( T_BF_DEMUX_INFO* demux_info );
static	VOID	bf_demux_library_close( MP4FHANDLE mp4f_handle );

static	BOOL	bf_demux_video_setup( ULLONG start_pts, E_FJ_DEMUX_DIRECTION direction, E_FJ_DEMUX_SPEED speed );
static	BOOL	bf_demux_audio_setup( ULLONG start_pts );
static	BOOL	bf_demux_freeatom_size_get( UINT32* data_size );
static	BOOL	bf_demux_freeatom_get( UINT32 data_address );
static	BOOL	bf_demux_stream_read( E_FJ_DEMUX_CODEC codec, T_FJ_DEMUX_STREAM_PARAM* stream_param );

// header/stream analyze
static	BOOL	bf_demux_mp4_avcc_analyze( UCHAR* avcc_adr, ULONG avcc_size, T_BF_DEMUX_H264_SPS_PPS_INFO* sps_pps_info );
#if 0	// use sps analyze
static	VOID	bf_demux_h264_sps_analyze( T_BF_DEMUX_H264_SPS* sps, ULONG sps_addr );
static	VOID	bf_demux_h264_bs_read_scaling_list( T_MP4_COMMON_STREAM_CONTROL* bit_stream, INT32* scalingList, INT32 sizeOfScalingList, INT32* useDefaultScalingMatrixFlag );
#endif
static	BOOL	bf_demux_mp4_hvcc_analyze( UINT8* hvcc_adr, UINT32 hvcc_size, T_BF_DEMUX_H265_PS_INFO* ps_info );

// PS NAL attach
static	BOOL	bf_demux_h264_sps_pps_attach( UCHAR* video_sample_addr_base, UCHAR* video_sample_addr );
static	BOOL	bf_demux_h265_ps_attach( UCHAR* video_sample_addr_base, UCHAR* video_sample_addr, T_BF_DEMUX_H265_PS_INFO* ps_info );

// StartCode Replace
static	BOOL	bf_demux_h264_startcode_replace( UCHAR* video_sample_addr, ULONG video_sample_size );
static	BOOL	bf_demux_h265_startcode_replace( UCHAR* video_sample_addr, ULONG video_sample_size );



/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

/**
 * @brief		Demuxer initialize
 * @param		UINT32	file_id
 *				T_FJ_DEMUX_HEADER_INFO*  header_info
 * @return		Success/Fail
 * @note		None
 * @attention	None
 */
BOOL BF_Demux_Initialize( UINT32 file_id, T_FJ_DEMUX_HEADER_INFO* header_info )
{
	MP4FErr								mp4_ret;
	BOOL								ret;
	UINT32								track_loop;
	UINT32								nal_loop;
	ULONG								duration, track_count;
	ULONG								*track_id_list;
	ULONG								creation_time, modification_time;
	ULONG								time_scale;
	CHAR*								handler_name;
	ULONG								trackDuration;
	ULONG								mediaTime=0;
	ULONG								mediaRate=0;
	ULONG								duration_by_tkhd=0;
	UCHAR								samplingFrequencyIndex;
	ULONG								dec_specific_info_size;
	UCHAR*								dec_specific_info;
	ULONG								mp4_dec_config_parm[5];
	USHORT								descr_parm[2];
	UINT32								edts_list_count;
	T_BF_DEMUX_H264_SPS_PPS_INFO		sps_pps_info;
	T_BF_DEMUX_H265_PS_INFO				ps_info;
	T_MP4_COMMON_STREAM_CONTROL			bit_stream;
#if 0	// use sps analyze
	T_BF_DEMUX_H264_SPS*				sps;
#endif
	
	if( gDmux_info.file_id != 0 ){
		printf("BF_Demux_Initialize : Error!! Already initialized.\n");
		return FALSE;
	}
	memset( (VOID*)&gDmux_info,   0, sizeof(gDmux_info) );
	memset( (VOID*)&sps_pps_info, 0, sizeof(sps_pps_info) );
	memset( (VOID*)&ps_info,      0, sizeof(ps_info) );
	
	// Initialize Buffer Management Data for File Format
	gDmux_info.header_area_p = malloc( D_BF_DEMUX_HEADER_READ_AREA_SIZE );
	
	gDmux_info.video_id = D_BF_DEMUX_TRACK_INVALID;
	gDmux_info.audio_id = D_BF_DEMUX_TRACK_INVALID;
	
	gDmux_info.file_id = file_id;
	
	mp4_ret = MP4F_OpenFile(
			(MP4FHANDLE *)&gDmux_info.mp4f_handle,
			MP4F_OPEN_MODE_READ,
			file_id,
			0,	// don't care. (write mode only)
			0,	// don't care. (write mode only)
			0	// don't care. (write mode only)
			);
	if( mp4_ret != MP4F_OK ){
		printf("BF_Demux_Initialize : MP4F_OpenFile error!\n");
		bf_demux_memory_close( (T_BF_DEMUX_INFO *)&gDmux_info );
		return FALSE;
	}
	
	mp4_ret = MP4F_Read( gDmux_info.mp4f_handle );
	if( mp4_ret != MP4F_OK ){
		printf("BF_Demux_Initialize : MP4F_Read error! ret = %lx\n", mp4_ret);
		bf_demux_memory_close( (T_BF_DEMUX_INFO *)&gDmux_info );
		return FALSE;
	}
	
	// get movie information
	mp4_ret = MP4F_GetMovieInformation(
			gDmux_info.mp4f_handle,
			&time_scale,
			&duration,
			&track_count,
			&track_id_list,
			&creation_time,
			&modification_time
			);
	if( mp4_ret != MP4F_OK ){
		printf("BF_Demux_Initialize : MP4F_GetMovieInformation error!\n");
		bf_demux_memory_close( (T_BF_DEMUX_INFO *)&gDmux_info );
		return FALSE;
	}
	
	if( track_count > D_BF_DEMUX_TRACK_MAX ){
		// Not Supportted(Only support Video/Audio track)
		printf("BF_Demux_Initialize : track num error! track_count=%ld\n", track_count);
		bf_demux_memory_close( (T_BF_DEMUX_INFO *)&gDmux_info );
		return FALSE;
	}
	
	for( track_loop=0; track_loop<track_count; track_loop++ ){
		// Set Track ID
		gDmux_info.track_info[track_loop].track_id = track_id_list[track_loop];
	}
	
	for( track_loop=0; track_loop<track_count; track_loop++ ){
		// Get Track Information
		mp4_ret = MP4F_GetTrackInformation(
				gDmux_info.mp4f_handle,
				gDmux_info.track_info[track_loop].track_id,
				(ULONG *)&gDmux_info.track_info[track_loop].handler_type,
				(ULONG *)&gDmux_info.track_info[track_loop].track_flags,
				&duration_by_tkhd,
				(ULONG *)&gDmux_info.track_info[track_loop].time_scale,
				(ULONG *)&gDmux_info.track_info[track_loop].total_duration,
#if ( CO_MOVIE_FF_AUDIO_PATTERN == 0 )
				(UCHAR *)&gDmux_info.track_info[track_loop].language,
#else
				(UINT16 *)&gDmux_info.track_info[track_loop].language,
#endif
				(CHAR **)&handler_name
			);
		if( mp4_ret != MP4F_OK ){
			printf("BF_Demux_Initialize : MP4F_GetTrackInformation error! track_loop=%d\n", track_loop);
			bf_demux_memory_close( (T_BF_DEMUX_INFO *)&gDmux_info );
			return FALSE;
		}
		
		if( gDmux_info.track_info[track_loop].handler_type == MP4F_HANDLER_TYPE_VIDEO ){
			gDmux_info.video_id = track_loop;
			
			mp4_ret = MP4F_GetWidthHeightFromVideoTrack(
					gDmux_info.mp4f_handle,
					gDmux_info.track_info[track_loop].track_id,
					(ULONG *)&gDmux_info.track_width,
					(ULONG *)&gDmux_info.track_height
					);
			if( mp4_ret != MP4F_OK ){
				printf("BF_Demux_Initialize : MP4F_GetWidthHeightFromVideoTrack error! track_loop=%d\n", track_loop);
				bf_demux_memory_close( (T_BF_DEMUX_INFO *)&gDmux_info );
				return FALSE;
			}
			
			gDmux_info.track_width  >>= 16;
			gDmux_info.track_height >>= 16;
			
			mp4_ret = MP4F_GetEditList(gDmux_info.mp4f_handle, gDmux_info.track_info[track_loop].track_id, 1, &trackDuration, &mediaTime, &mediaRate);
			if( mp4_ret == MP4F_OK ){
				gDmux_info.video_pts_offset = mediaTime;
			}
			else{
				gDmux_info.video_pts_offset = 0;
			}
		}
		else if( gDmux_info.track_info[track_loop].handler_type == MP4F_HANDLER_TYPE_SOUND ){
			gDmux_info.audio_id = track_loop;
		}
		else{
			continue;
		}
		
		mp4_ret = MP4F_GetSampleCount(
				gDmux_info.mp4f_handle,
				gDmux_info.track_info[track_loop].track_id,
				(ULONG *)&gDmux_info.track_info[track_loop].total_sample_num
				);
		if( mp4_ret != MP4F_OK ){
			printf("BF_Demux_Initialize : MP4F_GetSampleCount error! track_loop=%d\n", track_loop);
			bf_demux_memory_close( (T_BF_DEMUX_INFO *)&gDmux_info );
			return FALSE;
		}
		
		gDmux_info.track_info[track_loop].time_delta				= gDmux_info.track_info[track_loop].total_duration / gDmux_info.track_info[track_loop].total_sample_num;
		gDmux_info.track_info[track_loop].frame_rate_multiply_100	= (gDmux_info.track_info[track_loop].time_scale * 100) / gDmux_info.track_info[track_loop].time_delta;
		
		// get number of entry for sample description
		mp4_ret = MP4F_GetSampleDescriptionCount(
				gDmux_info.mp4f_handle,
				gDmux_info.track_info[track_loop].track_id,
				(ULONG *)&gDmux_info.track_info[track_loop].stsd_entry_count
				);
		if( mp4_ret != MP4F_OK ){
			printf("BF_Demux_Initialize : MP4F_GetSampleDescriptionCount error! track_loop=%d\n", track_loop);
			bf_demux_memory_close( (T_BF_DEMUX_INFO *)&gDmux_info );
			return FALSE;
		}
		
		if( gDmux_info.track_info[track_loop].stsd_entry_count != 1 ){
			printf("BF_Demux_Initialize : Multiple stsd does not support. track_loop=%d\n", track_loop);
			bf_demux_memory_close( (T_BF_DEMUX_INFO *)&gDmux_info );
			return FALSE;
		}
		
		// Get Sample Description
		mp4_ret = MP4F_GetSampleDescription(
				gDmux_info.mp4f_handle,
				gDmux_info.track_info[track_loop].track_id,
				(ULONG *)&gDmux_info.track_info[track_loop].dref_entry_id,
				(ULONG *)&gDmux_info.track_info[track_loop].descr_type,
				mp4_dec_config_parm,
				&dec_specific_info_size,
				&dec_specific_info,
				1	// stsdEntryId
				);
		if( mp4_ret != MP4F_OK ){
			printf("BF_Demux_Initialize : MP4F_GetSampleDescription error! track_loop=%d\n", track_loop);
			bf_demux_memory_close( (T_BF_DEMUX_INFO *)&gDmux_info );
			return FALSE;
		}

		if( gDmux_info.track_info[track_loop].descr_type == MP4F_SAMPLE_DESCR_TYPE_AVC1 ){
			// 'avc1'
			gDmux_info.video_codec = E_FJ_DEMUX_CODEC_VIDEO_H264;
			
			// get sps/pps address and size from avcc
			ret = bf_demux_mp4_avcc_analyze( (UCHAR*)dec_specific_info, dec_specific_info_size, &sps_pps_info);
			if( ret != TRUE ){
				printf("bf_demux_mp4_avcc_analyze error!!\n");
			}
			
			gDmux_info.sps_pps_info.sps_size = sps_pps_info.sps_size;
			gDmux_info.sps_pps_info.pps_size = sps_pps_info.pps_size;
			
			gDmux_info.sps_pps_info.sps_address = (UCHAR*)malloc( gDmux_info.sps_pps_info.sps_size );
			gDmux_info.sps_pps_info.pps_address = (UCHAR*)malloc( gDmux_info.sps_pps_info.pps_size );
			
			// set sps/pps
			memcpy( gDmux_info.sps_pps_info.sps_address, sps_pps_info.sps_address, gDmux_info.sps_pps_info.sps_size );
			memcpy( gDmux_info.sps_pps_info.pps_address, sps_pps_info.pps_address, gDmux_info.sps_pps_info.pps_size );
#if 0	// use sps analyze
			// get sps information
			sps = malloc( sizeof( T_BF_DEMUX_H264_SPS ) );
			if( sps != NULL ){
				// Get "frame_mbs_only_flag" (Since "NAL haeder" is contained in "sps_address", it is "+1")
				bf_demux_h264_sps_analyze(sps, (ULONG)(gDmux_info.sps_pps_info.sps_address + 1));
#if 1		// for Debug
				printf("profile_idc                           = %d\n", sps->profile_idc);
				printf("constraint_set0_flag                  = %d\n", sps->constraint_set0_flag);
				printf("constraint_set1_flag                  = %d\n", sps->constraint_set1_flag);
				printf("constraint_set2_flag                  = %d\n", sps->constraint_set2_flag);
				printf("constraint_set3_flag                  = %d\n", sps->constraint_set3_flag);
				printf("reserved_zero_4bits                   = %d\n", sps->reserved_zero_4bits);
				printf("level_idc                             = %d\n", sps->level_idc);
				printf("seq_parameter_set_id                  = %d\n", sps->seq_parameter_set_id);
				printf("chroma_format_idc                     = %d\n", sps->chroma_format_idc);
				printf("separate_colour_plane_flag            = %d\n", sps->separate_colour_plane_flag);
				printf("bit_depth_luma_minus8                 = %d\n", sps->bit_depth_luma_minus8);
				printf("bit_depth_chroma_minus8               = %d\n", sps->bit_depth_chroma_minus8);
				printf("qpprime_y_zero_transform_bypass_flag  = %d\n", sps->qpprime_y_zero_transform_bypass_flag);
				printf("seq_scaling_matrix_present_flag       = %d\n", sps->seq_scaling_matrix_present_flag);
				printf("log2_max_frame_num_minus4             = %d\n", sps->log2_max_frame_num_minus4);
				printf("pic_order_cnt_type                    = %d\n", sps->pic_order_cnt_type);
				printf("log2_max_pic_order_cnt_lsb_minus4     = %d\n", sps->log2_max_pic_order_cnt_lsb_minus4);
				printf("delta_pic_order_always_zero_flag      = %d\n", sps->delta_pic_order_always_zero_flag);
				printf("offset_for_non_ref_pic                = %d\n", sps->offset_for_non_ref_pic);
				printf("offset_for_top_to_bottom_field        = %d\n", sps->offset_for_top_to_bottom_field);
				printf("num_ref_frames_in_pic_order_cnt_cycle = %d\n", sps->num_ref_frames_in_pic_order_cnt_cycle);
				printf("num_ref_frames                        = %d\n", sps->num_ref_frames);
				printf("gaps_in_frame_num_value_allowed_flag  = %d\n", sps->gaps_in_frame_num_value_allowed_flag);
				printf("pic_width_in_mbs_minus1               = %d\n", sps->pic_width_in_mbs_minus1);
				printf("pic_height_in_map_units_minus1        = %d\n", sps->pic_height_in_map_units_minus1);
				printf("frame_mbs_only_flag                   = %d\n", sps->frame_mbs_only_flag);
#endif	// for Debug
#if 0
				if( sps->frame_mbs_only_flag ){
					gDmux_info.scan_mode = H264_PROGRESSIVE;
				}
				else{
					gDmux_info.scan_mode = H264_INTERLACE;
				}
#endif
				free(sps);
			}
			else{
				printf("CAN NOT ALLOCATE NEW MEMORY(SPS analyze).\n");
			}
#endif
			// get Max. visual width and height of streams from
			mp4_ret = MP4F_GetParmFromMp4VideoSampleDescr(
					gDmux_info.mp4f_handle,
					gDmux_info.track_info[track_loop].track_id,
					1,	// stsdEntryId
					(USHORT *)&gDmux_info.stream_width,
					(USHORT *)&gDmux_info.stream_height
					);
			if( mp4_ret != MP4F_OK ){
				printf("BF_Demux_Initialize : MP4F_GetParmFromMp4VideoSampleDescr error\n");
				bf_demux_memory_close( (T_BF_DEMUX_INFO *)&gDmux_info );
				return FALSE;
			}
		}
		else if( gDmux_info.track_info[track_loop].descr_type == MP4F_SAMPLE_DESCR_TYPE_HVC1 ){
			// 'hvc1'
			gDmux_info.video_codec = E_FJ_DEMUX_CODEC_VIDEO_H265;
			ret = bf_demux_mp4_hvcc_analyze( (UCHAR*)dec_specific_info, dec_specific_info_size, &ps_info );
			if( ret != TRUE ){
				printf("bf_demux_mp4_hvcc_analyze error!!\n");
			}
			
			gDmux_info.ps_info.total_nal_num	= ps_info.total_nal_num;
			gDmux_info.ps_info.total_nal_size	= ps_info.total_nal_size;
			
			for( nal_loop = 0; nal_loop < gDmux_info.ps_info.total_nal_num; nal_loop++ ){
				
				gDmux_info.ps_info.nal_size[nal_loop]		= ps_info.nal_size[nal_loop];
				gDmux_info.ps_info.nal_address[nal_loop]	= (UINT8*)malloc( gDmux_info.ps_info.nal_size[nal_loop] );
				memcpy( gDmux_info.ps_info.nal_address[nal_loop], ps_info.nal_address[nal_loop], gDmux_info.ps_info.nal_size[nal_loop] );
			}
			
			// get Max. visual width and height of streams from
			mp4_ret = MP4F_GetParmFromMp4VideoSampleDescr(
					gDmux_info.mp4f_handle,
					gDmux_info.track_info[track_loop].track_id,
					1,	// stsdEntryId
					(USHORT *)&gDmux_info.stream_width,
					(USHORT *)&gDmux_info.stream_height
					);
			if( mp4_ret != MP4F_OK ){
				printf("BF_Demux_Initialize : MP4F_GetParmFromMp4VideoSampleDescr error\n");
				bf_demux_memory_close( (T_BF_DEMUX_INFO *)&gDmux_info );
				return FALSE;
			}
			
			// Check Edit list for Bframe
			mp4_ret = MP4F_GetEditListCount(
					gDmux_info.mp4f_handle,
					gDmux_info.track_info[track_loop].track_id,
					(uInt32 *)&edts_list_count
					);
			if( mp4_ret == MP4F_ATOM_STRUCTURE_ERR ){
				gDmux_info.b_frame_exist = FALSE;
			}
			else if( mp4_ret == MP4F_OK ){
				gDmux_info.b_frame_exist = TRUE;
			}
			else{
				printf("BF_Demux_Initialize : MP4F_GetEditListCount error\n");
				bf_demux_memory_close( (T_BF_DEMUX_INFO *)&gDmux_info );
			}
		}
		else if( gDmux_info.track_info[track_loop].descr_type == MP4F_SAMPLE_DESCR_TYPE_MP4A ){
			// get audio format
			if( (mp4_dec_config_parm[0] == 0x40) || (mp4_dec_config_parm[0] == 0x67) ){
				gDmux_info.audio_codec = E_FJ_DEMUX_CODEC_AUDIO_AAC;
			}
			else {
				printf("BF_Demux_Initialize : Not Support Codec of Audio.\n");
				gDmux_info.audio_id = D_BF_DEMUX_TRACK_INVALID;
				continue;
			}
			
			// get audio max bit rate
			gDmux_info.audio_bit_rate = mp4_dec_config_parm[3];
			BF_MP4_Common_BS_Set_Pointer_Address( &bit_stream, dec_specific_info );
			BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( &bit_stream, 5 );	// skip "audioObjectType"
			samplingFrequencyIndex = BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 4 );	// samplingFrequencyIndex:4bit
			
			switch( samplingFrequencyIndex ){
				case 11 :
					gDmux_info.sampling_rate = 8000;
					break;
				case 10 :
					gDmux_info.sampling_rate = 11025;
					break;
				case 9 :
					gDmux_info.sampling_rate = 12000;
					break;
				case 8 :
					gDmux_info.sampling_rate = 16000;
					break;
				case 7 :
					gDmux_info.sampling_rate = 22500;
					break;
				case 6 :
					gDmux_info.sampling_rate = 24000;
					break;
				case 5 :
					gDmux_info.sampling_rate = 32000;
					break;
				case 4 :
					gDmux_info.sampling_rate = 44100;
					break;
				case 3 :
					gDmux_info.sampling_rate = 48000;
					break;
				default :
					printf("BF_Demux_Initialize : Not Support sampling_rate of Audio.\n");
					gDmux_info.audio_id = D_BF_DEMUX_TRACK_INVALID;
					continue;
			}
			// get channel count, sample size in sample description
			mp4_ret = MP4F_GetParmFromMp4AudioSampleDescr(
					gDmux_info.mp4f_handle,
					gDmux_info.track_info[track_loop].track_id,
					1,	// stsdEntryId
					&descr_parm[0],
					&descr_parm[1]
					);
			if( mp4_ret != MP4F_OK ){
				printf("BF_Demux_Initialize : MP4F_GetParmFromMp4AudioSampleDescr error.(Audio)\n");
				gDmux_info.audio_id = D_BF_DEMUX_TRACK_INVALID;
				continue;
			}
			
			// get audio channel
			gDmux_info.audio_ch = descr_parm[0];
		}
#if ( CO_MOVIE_FF_AUDIO_PATTERN == 0 )
#else
		else if( gDmux_info.track_info[track_loop].descr_type == MP4F_SAMPLE_DESCR_TYPE_SOWT ){
			mp4_ret = MP4F_GetParmFromQTAudioSampleDescr(
					gDmux_info.mp4f_handle,
					gDmux_info.track_info[track_loop].track_id,
					1,	// stsdEntryId
					&descr_parm[0],	// audio channel
					&descr_parm[1],	// audio sample size
					&mp4_dec_config_parm[0],	// samplesPerPacket
					&mp4_dec_config_parm[1],	// bytesPerPacket
					&mp4_dec_config_parm[2],	// bytesPerFrame
					&mp4_dec_config_parm[3]	// bytesPerSample
					);
			if( mp4_ret != MP4F_OK ){
				printf("BF_Demux_Initialize : MP4F_GetParmFromQTAudioSampleDescr error.(Audio)\n");
				gDmux_info.audio_id = D_BF_DEMUX_TRACK_INVALID;
				continue;
			}

			gDmux_info.audio_codec = E_FJ_DEMUX_CODEC_AUDIO_PCM;
			gDmux_info.audio_ch = descr_parm[0];
			gDmux_info.sampling_rate = gDmux_info.track_info[track_loop].time_scale;
			gDmux_info.audio_bit_rate = gDmux_info.sampling_rate * gDmux_info.audio_ch * descr_parm[1];
		}
#endif
		else{
			// Unsupported description type.
			if( gDmux_info.track_info[track_loop].handler_type == MP4F_HANDLER_TYPE_VIDEO ){
				gDmux_info.video_id = D_BF_DEMUX_TRACK_INVALID;
			}
			else if( gDmux_info.track_info[track_loop].handler_type == MP4F_HANDLER_TYPE_SOUND ){
				gDmux_info.audio_id = D_BF_DEMUX_TRACK_INVALID;
			}
			printf("Unsupported description type=%d\n", (UINT32)gDmux_info.track_info[track_loop].descr_type );
		}
	}
	// control data setting.
	if( gDmux_info.track_info[ gDmux_info.video_id ].frame_rate_multiply_100 % 200 ){
		gDmux_info.video_gop_num	= (gDmux_info.track_info[ gDmux_info.video_id ].frame_rate_multiply_100 / 200) + 1;
	}
	else{
		gDmux_info.video_gop_num	= gDmux_info.track_info[ gDmux_info.video_id ].frame_rate_multiply_100 / 200;
	}
	if( gDmux_info.video_gop_num == 0 ){
		gDmux_info.video_gop_num	= 1;
	}
	
	// Normal Play(Video).
	gDmux_info.video_play_last_pts		= (ULLONG)gDmux_info.track_info[ gDmux_info.video_id ].total_duration - gDmux_info.track_info[ gDmux_info.video_id ].time_delta;
	// Fast Rewind(Video)
	gDmux_info.video_rwd_last_pts		= 0;
	// Fast Forward(Video)
	gDmux_info.video_ff_last_pts		= ((gDmux_info.video_play_last_pts
											/ (gDmux_info.video_gop_num * gDmux_info.track_info[ gDmux_info.video_id ].time_delta))
											* (gDmux_info.video_gop_num * gDmux_info.track_info[ gDmux_info.video_id ].time_delta));
	// Reverse Play(Video)
	if( gDmux_info.video_play_last_pts < ((ULLONG)gDmux_info.video_gop_num * gDmux_info.track_info[ gDmux_info.video_id ].time_delta) ){
		gDmux_info.video_reverse_last_pts	= gDmux_info.video_play_last_pts;
	}
	else{
		gDmux_info.video_reverse_last_pts	= (ULLONG)(gDmux_info.video_gop_num - 1) * gDmux_info.track_info[ gDmux_info.video_id ].time_delta;
	}
	// Normal Play(Audio).
	if( gDmux_info.audio_id != D_BF_DEMUX_TRACK_INVALID ){
		gDmux_info.audio_last_pts	= (ULLONG)gDmux_info.track_info[ gDmux_info.audio_id ].total_duration - gDmux_info.track_info[ gDmux_info.audio_id ].time_delta;
	}
	
	if( gDmux_info.video_id != D_BF_DEMUX_TRACK_INVALID ){
		header_info->video_codec			= gDmux_info.video_codec;
		header_info->track_width			= gDmux_info.track_width;
		header_info->track_height			= gDmux_info.track_height;
		header_info->stream_width			= gDmux_info.stream_width;
		header_info->stream_height			= gDmux_info.stream_height;
		header_info->video_time_scale		= gDmux_info.track_info[ gDmux_info.video_id ].time_scale;
		header_info->video_time_delta		= gDmux_info.track_info[ gDmux_info.video_id ].time_delta;
		header_info->video_total_sample_num	= gDmux_info.track_info[ gDmux_info.video_id ].total_sample_num;
		header_info->video_total_duration	= gDmux_info.track_info[ gDmux_info.video_id ].total_duration;
		header_info->video_play_last_pts	= gDmux_info.video_play_last_pts;
		header_info->video_ff_last_pts		= gDmux_info.video_ff_last_pts;
		header_info->offset_of_start_frame	= gDmux_info.video_pts_offset / gDmux_info.track_info[ gDmux_info.video_id ].time_delta;
	}
	else{
		header_info->video_codec	= E_FJ_DEMUX_CODEC_INVALID;
	}
	
	if( gDmux_info.audio_id != D_BF_DEMUX_TRACK_INVALID ){
		header_info->audio_codec			= gDmux_info.audio_codec;
		header_info->sampling_rate			= gDmux_info.sampling_rate;
		header_info->audio_bit_rate			= gDmux_info.audio_bit_rate;
		header_info->audio_ch				= gDmux_info.audio_ch;
		header_info->audio_time_scale		= gDmux_info.track_info[ gDmux_info.audio_id ].time_scale;
		header_info->audio_time_delta		= gDmux_info.track_info[ gDmux_info.audio_id ].time_delta;
		header_info->audio_total_sample_num	= gDmux_info.track_info[ gDmux_info.audio_id ].total_sample_num;
		header_info->audio_total_duration	= gDmux_info.track_info[ gDmux_info.audio_id ].total_duration;
		header_info->audio_last_pts			= gDmux_info.audio_last_pts;
	}
	else{
		header_info->audio_codec	= E_FJ_DEMUX_CODEC_INVALID;
	}
	
	// Initialization of the inter-task communication flag and the start of the task.
	OS_User_Clr_Flg( FID_DEMUX, 0 );
	OS_User_Sta_Tsk( TID_DEMUX, 0 );
	
	return TRUE;
}


BOOL BF_Demux_VideoSetup( UINT32 file_id, ULLONG start_pts, E_FJ_DEMUX_DIRECTION direction, E_FJ_DEMUX_SPEED speed )
{
	OS_USER_FLGPTN	flgptn;
	
	if( file_id != gDmux_info.file_id ){
		printf("BF_Demux_VideoSetup : file_id error.\n");
		return FALSE;
	}
	if( gDmux_info.video_id == D_BF_DEMUX_TRACK_INVALID ){
		printf("BF_Demux_VideoSetup : Video track is invalid.\n");
		return FALSE;
	}
	if( start_pts % gDmux_info.track_info[ gDmux_info.video_id ].time_delta ){
		printf("BF_Demux_VideoSetup : The specified PTS is the value that is not divisible by time_delta.\n");
	}
	
	// Set message table.
	if( gDmux_info.video_setup_message.use_flg == TRUE ){
		printf("BF_Demux_VideoSetup : Video setup process is running.\n");
		return FALSE;
	}
	gDmux_info.video_setup_message.use_flg		= TRUE;
	gDmux_info.video_setup_message.start_pts	= start_pts;
	gDmux_info.video_setup_message.direction	= direction;
	gDmux_info.video_setup_message.speed		= speed;
	
	// flag clear(setup complete)
	OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_VIDEO_SETUP_COMPLETE );
	
	// flag set(setup request)
	OS_User_Set_Flg( FID_DEMUX, FLG_DEMUX_VIDEO_SETUP_REQUEST);
	
	// flag wait(setup complete)
	OS_User_Twai_Flg( FID_DEMUX, FLG_DEMUX_VIDEO_SETUP_COMPLETE, D_OS_USER_TWF_ORW, &flgptn, 1000 );
	
	// flag clear(setup complete)
	OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_VIDEO_SETUP_COMPLETE );
	
	gDmux_info.video_setup_message.use_flg	= FALSE;
	gDmux_info.first_video_flag				= TRUE;
	
	return TRUE;
}

BOOL BF_Demux_AudioSetup( UINT32 file_id, ULLONG start_pts )
{
	OS_USER_FLGPTN	flgptn;
	
	if( file_id != gDmux_info.file_id ){
		printf("BF_Demux_AudioSetup : file_id error.\n");
		return FALSE;
	}
	if( gDmux_info.audio_id == D_BF_DEMUX_TRACK_INVALID ){
		printf("BF_Demux_AudioSetup : Audio track is invalid.\n");
		return FALSE;
	}
	
	// Set message table.
	if( gDmux_info.audio_setup_message.use_flg == TRUE ){
		printf("BF_Demux_AudioSetup : Audio setup process is running.\n");
		return FALSE;
	}
	gDmux_info.audio_setup_message.use_flg		= TRUE;
	gDmux_info.audio_setup_message.start_pts	= start_pts;
	
	// flag clear(setup complete)
	OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_AUDIO_SETUP_COMPLETE );
	
	// flag set(setup request)
	OS_User_Set_Flg( FID_DEMUX, FLG_DEMUX_AUDIO_SETUP_REQUEST);
	
	// flag wait(setup complete)
	OS_User_Twai_Flg( FID_DEMUX, FLG_DEMUX_AUDIO_SETUP_COMPLETE, D_OS_USER_TWF_ORW, &flgptn, 1000 );
	
	// flag clear(setup complete)
	OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_AUDIO_SETUP_COMPLETE );
	
	gDmux_info.audio_setup_message.use_flg = FALSE;
	
	return TRUE;
}

BOOL BF_Demux_FreeAtomSizeGet( UINT32 file_id, UINT32* data_size )
{
	OS_USER_FLGPTN	flgptn;
	
	if( file_id != gDmux_info.file_id ){
		printf("BF_Demux_FreeAtomSizeGet : file_id error.\n");
		return FALSE;
	}
	if( data_size == NULL ){
		printf("BF_Demux_FreeAtomSizeGet : data_size error.\n");
		return FALSE;
	}
	
	// Set message table.
	if( gDmux_info.free_atom_size_get_message.use_flg == TRUE ){
		printf("BF_Demux_FreeAtomSizeGet : Free atom size get process is running.\n");
		return FALSE;
	}
	gDmux_info.free_atom_size_get_message.use_flg = TRUE;
	
	// flag clear(setup complete)
	OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_FREE_ATOM_SIZE_GET_COMPLETE );
	
	// flag set(setup request)
	OS_User_Set_Flg( FID_DEMUX, FLG_DEMUX_FREE_ATOM_SIZE_GET_REQUEST);
	
	// flag wait(setup complete)
	OS_User_Twai_Flg( FID_DEMUX, FLG_DEMUX_FREE_ATOM_SIZE_GET_COMPLETE, D_OS_USER_TWF_ORW, &flgptn, 1000 );
	
	// flag clear(setup complete)
	OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_FREE_ATOM_SIZE_GET_COMPLETE );
	
	// set read stream information.
	*data_size = gDmux_info.free_atom_size_get_message.data_size;
	
	gDmux_info.free_atom_size_get_message.use_flg = FALSE;
	
	return TRUE;
}

BOOL BF_Demux_FreeAtomGet( UINT32 file_id, UINT32 data_address )
{
	OS_USER_FLGPTN	flgptn;
	
	if( file_id != gDmux_info.file_id ){
		printf("BF_Demux_FreeAtomGet : file_id error.\n");
		return FALSE;
	}
	if( data_address == 0 ){
		printf("BF_Demux_FreeAtomGet : data_address error.\n");
		return FALSE;
	}
	
	// Set message table.
	if( gDmux_info.free_atom_get_message.use_flg == TRUE ){
		printf("BF_Demux_FreeAtomGet : Free atom size get process is running.\n");
		return FALSE;
	}
	gDmux_info.free_atom_get_message.use_flg		= TRUE;
	gDmux_info.free_atom_get_message.data_address	= data_address;
	
	// flag clear(setup complete)
	OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_FREE_ATOM_GET_COMPLETE );
	
	// flag set(setup request)
	OS_User_Set_Flg( FID_DEMUX, FLG_DEMUX_FREE_ATOM_GET_REQUEST);
	
	// flag wait(setup complete)
	OS_User_Twai_Flg( FID_DEMUX, FLG_DEMUX_FREE_ATOM_GET_COMPLETE, D_OS_USER_TWF_ORW, &flgptn, 1000 );
	
	// flag clear(setup complete)
	OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_FREE_ATOM_GET_COMPLETE );
	
	gDmux_info.free_atom_get_message.use_flg = FALSE;
	
	return TRUE;
}

BOOL BF_Demux_StreamRead( UINT32 file_id, E_FJ_DEMUX_CODEC codec, UINT32 read_stream_num )
{
	if( file_id != gDmux_info.file_id ){
		printf("BF_Demux_StreamRead : file_id error.\n");
		return FALSE;
	}
	
	if( codec == gDmux_info.video_codec ){
		if( read_stream_num > D_BF_DEMUX_FEED_VIDEO_STREAM_MAX ){
			printf("BF_Demux_StreamRead : [VIDEO] Over the feed possible number(%d). feed_num=%d\n", D_BF_DEMUX_FEED_VIDEO_STREAM_MAX, read_stream_num);
			return FALSE;
		}
		// Set message table.
		if( gDmux_info.video_stream_read_message.use_flg == TRUE ){
//			printf("BF_Demux_StreamRead : Video stream read process is running.\n");
			return FALSE;
		}
		gDmux_info.video_stream_read_message.use_flg					= TRUE;
		gDmux_info.video_stream_read_message.codec						= codec;
		gDmux_info.video_stream_read_message.read_stream_num			= read_stream_num;
		
		// flag set(read request)
		OS_User_Set_Flg( FID_DEMUX, FLG_DEMUX_VIDEO_STREAM_READ_REQUEST);
	}
	else if( codec == gDmux_info.audio_codec ){
		if( read_stream_num > D_BF_DEMUX_FEED_AUDIO_STREAM_MAX ){
			printf("BF_Demux_StreamRead : [AUDIO] Over the feed possible number(%d). feed_num=%d\n", D_BF_DEMUX_FEED_AUDIO_STREAM_MAX, read_stream_num);
			return FALSE;
		}
		// Set message table.
		if( gDmux_info.audio_stream_read_message.use_flg == TRUE ){
//			printf("BF_Demux_StreamRead : Audio stream read process is running.\n");
			return FALSE;
		}
		gDmux_info.audio_stream_read_message.use_flg					= TRUE;
		gDmux_info.audio_stream_read_message.codec						= codec;
		gDmux_info.audio_stream_read_message.read_stream_num			= read_stream_num;
		
		// flag set(read request)
		OS_User_Set_Flg( FID_DEMUX, FLG_DEMUX_AUDIO_STREAM_READ_REQUEST);
	}
	else{
		printf("BF_Demux_StreamRead : codec error[%d].\n", codec);
		return FALSE;
	}
	
	return TRUE;
}

BOOL BF_Demux_StreamReadSync( UINT32 file_id, E_FJ_DEMUX_CODEC codec, UINT32 read_stream_num, T_FJ_DEMUX_STREAM_PARAM* stream_param )
{
	OS_USER_FLGPTN	flgptn;
	
	if( file_id != gDmux_info.file_id ){
		printf("BF_Demux_StreamReadSync : file_id error.\n");
		return FALSE;
	}
	
	if( codec == gDmux_info.video_codec ){
		// Set message table.
		if( gDmux_info.video_stream_sync_read_message.use_flg == TRUE ){
			printf("BF_Demux_StreamReadSync : Video stream read process is running.\n");
			return FALSE;
		}
		gDmux_info.video_stream_sync_read_message.use_flg			= TRUE;
		gDmux_info.video_stream_sync_read_message.codec				= codec;
		gDmux_info.video_stream_sync_read_message.read_stream_num	= read_stream_num;
		
		// flag clear(read complete)
		OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_VIDEO_STREAM_SYNC_READ_COMPLETE );
		
		// flag set(read request)
		OS_User_Set_Flg( FID_DEMUX, FLG_DEMUX_VIDEO_STREAM_SYNC_READ_REQUEST);
		
		// flag wait(read complete)
		OS_User_Twai_Flg( FID_DEMUX, FLG_DEMUX_VIDEO_STREAM_SYNC_READ_COMPLETE, D_OS_USER_TWF_ORW, &flgptn, 1000 );
		
		// flag clear(read complete)
		OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_VIDEO_STREAM_SYNC_READ_COMPLETE );
		
		// set read stream information.
		*stream_param = gDmux_info.video_stream_sync_read_message.stream_param;
		
		gDmux_info.video_stream_sync_read_message.use_flg = FALSE;
	}
	else if( codec == gDmux_info.audio_codec ){
		// Set message table.
		if( gDmux_info.audio_stream_sync_read_message.use_flg == TRUE ){
			printf("BF_Demux_StreamReadSync : Audio stream read process is running.\n");
			return FALSE;
		}
		gDmux_info.audio_stream_sync_read_message.use_flg			= TRUE;
		gDmux_info.audio_stream_sync_read_message.codec				= codec;
		gDmux_info.audio_stream_sync_read_message.read_stream_num	= read_stream_num;
		
		// flag clear(read complete)
		OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_AUDIO_STREAM_SYNC_READ_COMPLETE );
		
		// flag set(read request)
		OS_User_Set_Flg( FID_DEMUX, FLG_DEMUX_AUDIO_STREAM_SYNC_READ_REQUEST);
		
		// flag wait(read complete)
		OS_User_Twai_Flg( FID_DEMUX, FLG_DEMUX_AUDIO_STREAM_SYNC_READ_COMPLETE, D_OS_USER_TWF_ORW, &flgptn, 1000 );
		
		// flag clear(read complete)
		OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_AUDIO_STREAM_SYNC_READ_COMPLETE );
		
		// set read stream information.
		*stream_param = gDmux_info.audio_stream_sync_read_message.stream_param;
		
		gDmux_info.audio_stream_sync_read_message.use_flg = FALSE;
	}
	else{
		printf("BF_Demux_StreamReadSync : codec error[%d].\n", codec);
		return FALSE;
	}
	
	return TRUE;
}

BOOL BF_Demux_Finalize( UINT32 file_id )
{
	OS_USER_FLGPTN		flgptn;
	
	if( file_id != gDmux_info.file_id ){
		printf("BF_Demux_Finalize : file_id error.\n");
		return FALSE;
	}
	
	// flag clear(finalize complete)
	OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_FINALIZE_COMPLETE );
	
	// flag set(finalize request)
	OS_User_Set_Flg( FID_DEMUX, FLG_DEMUX_FINALIZE_REQUEST);
	
	// flag wait(finalize complete)
	OS_User_Twai_Flg( FID_DEMUX, FLG_DEMUX_FINALIZE_COMPLETE, D_OS_USER_TWF_ORW, &flgptn, 1000 );
	
	// flag clear(finalize complete)
	OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_FINALIZE_COMPLETE );
	
	gDmux_info.file_id = 0;
	
	return TRUE;
}

static VOID bf_demux_memory_close( T_BF_DEMUX_INFO* demux_info )
{
	UINT32	nal_loop;
	
	// H.264 sps/pps address free.
	if( demux_info->sps_pps_info.sps_address != NULL ){
		free( demux_info->sps_pps_info.sps_address );
		demux_info->sps_pps_info.sps_address = NULL;
		demux_info->sps_pps_info.sps_size = 0;
	}
	if( demux_info->sps_pps_info.pps_address != NULL ){
		free( demux_info->sps_pps_info.pps_address );
		demux_info->sps_pps_info.pps_address = NULL;
		demux_info->sps_pps_info.pps_size = 0;
	}
	
	// H.265 ps address free.
	for( nal_loop = 0; nal_loop < gDmux_info.ps_info.total_nal_num; nal_loop++ ){
		if( gDmux_info.ps_info.nal_address[nal_loop] != NULL ){
			free( gDmux_info.ps_info.nal_address[nal_loop] );
			gDmux_info.ps_info.nal_address[nal_loop]	= NULL;
			gDmux_info.ps_info.nal_size[nal_loop]		= 0;
		}
		else{
			printf("bf_demux_memory_close : Error. There may be a problem with the information in the ps_info. \n");
		}
	}
	
	// Close MP4 memory.
	if( demux_info->mp4f_handle != NULL ){
		bf_demux_library_close( demux_info->mp4f_handle );
		demux_info->mp4f_handle = NULL;
	}
	
	return;
}

static VOID bf_demux_library_close( MP4FHANDLE mp4f_handle )
{
    MP4FErr	mp4err;
	
	if( mp4f_handle != NULL ){
		mp4err = MP4F_Close( mp4f_handle );
		if( mp4err != MP4F_OK ){
			printf("MP4F_Close Error.mp4err=%d\n", (UINT32)mp4err);
		}
	}
	return;
}


static BOOL bf_demux_video_setup( ULLONG start_pts, E_FJ_DEMUX_DIRECTION direction, E_FJ_DEMUX_SPEED speed )
{
	ULLONG	temp_get_stream_pts;
	UINT32	time_delta;
	UINT32	gop_num;
	
	time_delta	= gDmux_info.track_info[ gDmux_info.video_id ].time_delta;
	gop_num		= gDmux_info.video_gop_num;
	
	if( start_pts % time_delta ){
		printf("bf_demux_video_setup : The specified PTS is the value that is not divisible by time_delta.\n");
	}
	
	switch( speed ){
		case E_FJ_DEMUX_SPEED_1X :
		case E_FJ_DEMUX_SPEED_1_2X :
		case E_FJ_DEMUX_SPEED_1_4X :
		case E_FJ_DEMUX_SPEED_1_8X :
			gDmux_info.only_i_frame = FALSE;
			break;
		
		case E_FJ_DEMUX_SPEED_2X :
		case E_FJ_DEMUX_SPEED_4X :
		case E_FJ_DEMUX_SPEED_8X :
			gDmux_info.only_i_frame = TRUE;
			break;
		default :
			printf("bf_demux_video_setup : parameter(speed) error!\n");
			return FALSE;
	}
	
	temp_get_stream_pts			= (ULLONG)((( start_pts / time_delta ) / gop_num) * gop_num) * time_delta;
	
	if( (gDmux_info.only_i_frame == TRUE) && (direction == E_FJ_DEMUX_DIRECTION_FWD) ){
		// case of Fast Forward.
		gDmux_info.get_video_stream_pts = temp_get_stream_pts + ( gop_num * time_delta );
		if( gDmux_info.get_video_stream_pts > gDmux_info.video_ff_last_pts ){
			printf("bf_demux_video_setup : There is no stream where acquisition is possible.\n");
			return FALSE;
		}
		
		gDmux_info.video_last_pts = gDmux_info.video_ff_last_pts;
	}
	else if( (gDmux_info.only_i_frame == TRUE) && (direction == E_FJ_DEMUX_DIRECTION_RWD) ){
		// case of Fast Rewind.
		if( (start_pts == temp_get_stream_pts) && ( start_pts != 0 ) ){
			gDmux_info.get_video_stream_pts = temp_get_stream_pts - ( gop_num * time_delta );
		}
		else{
			gDmux_info.get_video_stream_pts = temp_get_stream_pts;
		}
		
		gDmux_info.video_last_pts = gDmux_info.video_rwd_last_pts;
	}
	else if( (gDmux_info.only_i_frame == FALSE) && (direction == E_FJ_DEMUX_DIRECTION_FWD) ){
		// case of Normal Play.
		gDmux_info.get_video_stream_pts	= temp_get_stream_pts;
		gDmux_info.video_last_pts		= gDmux_info.video_play_last_pts;
	}
	else{
		// case of Reverse Play.
		gDmux_info.get_video_stream_pts	= temp_get_stream_pts;
		gDmux_info.video_last_pts		= gDmux_info.video_reverse_last_pts;
	}
	
	gDmux_info.direction				= direction;
	gDmux_info.speed					= speed;
	
	printf("bf_demux_video_setup : start PTS is %ld\n", (ULONG)start_pts);
	printf("bf_demux_video_setup : Feed start PTS is %ld\n", (ULONG)gDmux_info.get_video_stream_pts);
	
	return TRUE;
}

static BOOL bf_demux_audio_setup( ULLONG start_pts )
{
	gDmux_info.get_audio_stream_pts = start_pts;
	
	printf("bf_demux_audio_setup : Feed start audio PTS is %ld\n", (ULONG)gDmux_info.get_audio_stream_pts);
	
	return TRUE;
}

static BOOL bf_demux_freeatom_size_get( UINT32* data_size )
{
	MP4FErr err;
	
	if( data_size == NULL ){
		printf("bf_demux_freeatom_size_get : data_size error\n");
		return FALSE;
	}
	
	err = MP4F_GetFreeSize( gDmux_info.mp4f_handle, (ULONG *)data_size );
	if( err != MP4F_OK ){
		if( (err == MP4F_FILE_TYPE_NOT_EXIST_ERR) ||
			(err == MP4F_TRACK_NOT_FOUND_ERR)     ){
			*data_size = 0;
			return TRUE;
		}
		else{
			printf("MP4F_GetFreeSize error\n");
			return FALSE;
		}
	}
	
	return TRUE;
}

static BOOL bf_demux_freeatom_get( UINT32 data_address )
{
	MP4FErr err;
	
	if( data_address == 0 ){
		printf("bf_demux_freeatom_get : data_address error\n");
		return FALSE;
	}
	
	err = MP4F_GetFree( gDmux_info.mp4f_handle, (UCHAR *)data_address );
	if( err != MP4F_OK ){
		printf("MP4F_GetFree error\n");
		return FALSE;
	}
	
	return TRUE;
}

static BOOL bf_demux_stream_read( E_FJ_DEMUX_CODEC codec, T_FJ_DEMUX_STREAM_PARAM* stream_param )
{
	FJ_ERR_CODE					fj_ret;
	BOOL						ret;
	MP4FErr						err;
	ULONG						track_id;
	ULONG						stsd_id;
	ULONG						sample_id=0;	// sample_id = 0 (Get sample from the value of sampleDT.)
	ULONG						sample_size;
	ULONG						tmp_sample_size=0;
	ULONG						sample_duration;
	ULONG						sample_ct_offset;
	ULONG						sync_sample_flag;
	ULONG						sample_offset;
	ULONG						get_pts;
	INT64						position64;
	UINT32						buffer_address;
	UINT32						tmp_buffer_address=0;
	ULONG						read_size;
	ULLONG						last_pts;
	ULLONG						gop_pts;
	
	switch( codec ){
		case E_FJ_DEMUX_CODEC_VIDEO_H264 :
		case E_FJ_DEMUX_CODEC_VIDEO_H265 :
			track_id	= gDmux_info.track_info[ gDmux_info.video_id ].track_id;
			get_pts		= (ULONG)gDmux_info.get_video_stream_pts;
			last_pts	= gDmux_info.video_last_pts;
			break;
		case E_FJ_DEMUX_CODEC_AUDIO_AAC :
		case E_FJ_DEMUX_CODEC_AUDIO_PCM :
			track_id	= gDmux_info.track_info[ gDmux_info.audio_id ].track_id;
			get_pts		= (ULONG)gDmux_info.get_audio_stream_pts;
			last_pts	= gDmux_info.audio_last_pts;
			break;
			
		default :
			printf("bf_demux_stream_read : codec error!\n");
			return FALSE;
	}
	
	err = MP4F_GetSampleOffset(
			gDmux_info.mp4f_handle,
			track_id,
			&sample_id,
			&stsd_id,
			&sample_size,
			&sample_duration,
			&get_pts,
			&sample_ct_offset,
			&sync_sample_flag,
			&sample_offset
			);
	if( err != MP4F_OK ){
		printf("bf_demux_stream_read : MP4F_GetSampleOffset error! codec is %d\n", codec);
		return FALSE;
	}
	
	if( (codec == E_FJ_DEMUX_CODEC_VIDEO_H264) && (gDmux_info.first_video_flag == TRUE) ){
		// Plus the size of the SPS / PPS.
		tmp_sample_size = sample_size;
		// sample_size += start code(4byte) + SPS + start code(4byte) + PPS
		sample_size += 4 + gDmux_info.sps_pps_info.sps_size + 4 + gDmux_info.sps_pps_info.pps_size;
	}
	if( (codec == E_FJ_DEMUX_CODEC_VIDEO_H265) && (gDmux_info.first_video_flag == TRUE) ){
		// Plus the size of the PS NAL.
		tmp_sample_size = sample_size;
		// sample_size += "Total NAL size" + "StartCode size(Assigned to each NAL(4byte fix))"
		sample_size += gDmux_info.ps_info.total_nal_size + (gDmux_info.ps_info.total_nal_num * 4);
	}

	if( codec == E_FJ_DEMUX_CODEC_AUDIO_PCM){
		// Max 4096Bytes every time
		if(sample_size >= 4096){
			sample_size = 4096;	
		}
	}
	
	ret = FJ_Host_Demux_GetStreamAreaCB( gDmux_info.file_id, codec, (UINT32*)&sample_size, &buffer_address );
	if( ret != TRUE ){
//		printf("bf_demux_stream_read: FJ_Host_Demux_GetStreamAreaCB error!!\n");
		return FALSE;
	}
	
	fj_ret = FJ_Fs_Lseek64( gDmux_info.file_id, sample_offset, FJ_FS_SEEK_SET, &position64 );
	if( fj_ret != FJ_ERR_OK ){
		printf("bf_demux_stream_read : FJ_Fs_Lseek error!!\n");
		return FALSE;
	}
	
	if( (codec == E_FJ_DEMUX_CODEC_VIDEO_H264) && (gDmux_info.first_video_flag == TRUE) ){
		// Performs stream acquisition to the address plus the size of SPS / PPS.
		sample_size			= tmp_sample_size;
		tmp_buffer_address	= buffer_address;
		buffer_address		+= 4 + gDmux_info.sps_pps_info.sps_size + 4 + gDmux_info.sps_pps_info.pps_size;
	}
	if( (codec == E_FJ_DEMUX_CODEC_VIDEO_H265) && (gDmux_info.first_video_flag == TRUE) ){
		// Performs stream acquisition to the address plus the size of PS NAL.
		sample_size			= tmp_sample_size;
		tmp_buffer_address	= buffer_address;
		buffer_address		+= gDmux_info.ps_info.total_nal_size + (gDmux_info.ps_info.total_nal_num * 4);
	}
	
	FJ_Fs_Read( gDmux_info.file_id, (UINT32 *)buffer_address, sample_size, &read_size);
	if( sample_size != read_size ){
		printf("bf_demux_stream_read : FJ_Fs_Read error! sample_size=%ld, read_size=%ld\n", sample_size, read_size);
		return FALSE;
	}
	
	FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (ULONG)buffer_address, sample_size);
	
	if( codec == E_FJ_DEMUX_CODEC_VIDEO_H264 ){
		if( gDmux_info.first_video_flag == TRUE ){
			// Place the SPS / PPS.
			bf_demux_h264_sps_pps_attach( (UCHAR*)tmp_buffer_address, (UCHAR*)buffer_address );
			sample_size		+= 4 + gDmux_info.sps_pps_info.sps_size + 4 + gDmux_info.sps_pps_info.pps_size;
			buffer_address	= tmp_buffer_address;
		}
		// Implement the replacement to the start code.
		bf_demux_h264_startcode_replace( (UCHAR*)buffer_address, sample_size );
	}
	if( codec == E_FJ_DEMUX_CODEC_VIDEO_H265 ){
		if( gDmux_info.first_video_flag == TRUE ){
#if 0	// tentative code. delete NAL(SEI).
			if( (((UCHAR*)buffer_address)[0] == 0) && (((UCHAR*)buffer_address)[1] == 0) && (((UCHAR*)buffer_address)[2] == 0) && (((UCHAR*)buffer_address)[3] == 6) ){
				buffer_address		+= 10;
				tmp_buffer_address	+= 10;
				sample_size			-= 10;
			}
#endif
			// Place the PS NAL.
			bf_demux_h265_ps_attach( (UCHAR*)tmp_buffer_address, (UCHAR*)buffer_address, (T_BF_DEMUX_H265_PS_INFO *)&gDmux_info.ps_info );
			sample_size		+= gDmux_info.ps_info.total_nal_size + (gDmux_info.ps_info.total_nal_num * 4);
			buffer_address	= tmp_buffer_address;
		}
		// Implement the replacement to the start code.
		bf_demux_h265_startcode_replace( (UCHAR*)buffer_address, sample_size );
	}
	
	stream_param->stream_size[stream_param->stream_num]		= sample_size;
	stream_param->stream_address[stream_param->stream_num]	= buffer_address;
	stream_param->pts[stream_param->stream_num]				= (ULLONG)get_pts + sample_ct_offset;
	
	if( (codec == E_FJ_DEMUX_CODEC_VIDEO_H264) || (codec == E_FJ_DEMUX_CODEC_VIDEO_H265) ){
		if( stream_param->pts[stream_param->stream_num] >= gDmux_info.video_pts_offset ){
			stream_param->pts[stream_param->stream_num] -= gDmux_info.video_pts_offset;
		}
		else{
			// Route that should not pass through.
			printf("bf_demux_stream_read : There was abnormal in the process of PTS offset.");
			stream_param->pts[stream_param->stream_num] = 0;
		}
	}
	
	stream_param->dts[stream_param->stream_num]				= get_pts;
	if( sync_sample_flag == MP4F_SYNC_SAMPLE ){
		stream_param->stss[stream_param->stream_num]		= 1;
	}
	else{
		stream_param->stss[stream_param->stream_num]		= 0;
	}

	if( codec == E_FJ_DEMUX_CODEC_AUDIO_PCM){
		if( (DOUBLE)get_pts + sample_size / 4 >= last_pts ){
			stream_param->stream_end_flag[stream_param->stream_num]	= 1;
		}
		else{
			stream_param->stream_end_flag[stream_param->stream_num]	= 0;
		}
	}
	else{
		if( last_pts == get_pts ){
			stream_param->stream_end_flag[stream_param->stream_num]	= 1;
#if 0	// for Debug
			if( codec == E_FJ_DEMUX_CODEC_VIDEO_H264 ){
				printf("last feed dts is %ld\n", get_pts);
				printf("last stream size is %d\n", stream_param->stream_size[stream_param->stream_num]);
			}
#endif
		}
		else{
			stream_param->stream_end_flag[stream_param->stream_num]	= 0;
		}
	}
	
	gDmux_info.first_video_flag = FALSE;
	
	if( stream_param->stream_end_flag[stream_param->stream_num] == 1 ){
		stream_param->stream_num++;
		return TRUE;
	}
	stream_param->stream_num++;
	
	// Ready to the next feed.
	switch( codec ){
		case E_FJ_DEMUX_CODEC_VIDEO_H264 :
		case E_FJ_DEMUX_CODEC_VIDEO_H265 :
			
			gop_pts = (ULLONG)gDmux_info.video_gop_num * gDmux_info.track_info[ gDmux_info.video_id ].time_delta;
			
			if( (gDmux_info.only_i_frame == TRUE) && (gDmux_info.direction == E_FJ_DEMUX_DIRECTION_FWD) ){
				// case of Fast Forward.
				gDmux_info.get_video_stream_pts += gop_pts;
			}
			else if( (gDmux_info.only_i_frame == TRUE) && (gDmux_info.direction == E_FJ_DEMUX_DIRECTION_RWD) ){
				// case of Fast Rewind.
				if( gDmux_info.get_video_stream_pts >= gop_pts ){
					gDmux_info.get_video_stream_pts -= gop_pts;
				}
				else{
					gDmux_info.get_video_stream_pts = 0;
				}
			}
			else if( (gDmux_info.only_i_frame == FALSE) && (gDmux_info.direction == E_FJ_DEMUX_DIRECTION_FWD) ){
				// case of Normal Play.
				gDmux_info.get_video_stream_pts += gDmux_info.track_info[ gDmux_info.video_id ].time_delta;
			}
			else{
				// case of Reverse Play.
				gDmux_info.get_video_stream_pts += gDmux_info.track_info[ gDmux_info.video_id ].time_delta;
				if( !(gDmux_info.get_video_stream_pts % gop_pts) ){
					
					if( gDmux_info.get_video_stream_pts >= (gop_pts * 2) ){
						gDmux_info.get_video_stream_pts -= (gop_pts * 2);
					}
				}
				else{
					// Case of reverse playback in the last frame.
					if( gDmux_info.get_video_stream_pts > gDmux_info.video_play_last_pts ){
						gDmux_info.get_video_stream_pts = (gDmux_info.get_video_stream_pts / gop_pts) * gop_pts;
						if( gDmux_info.get_video_stream_pts >= gop_pts ){
							gDmux_info.get_video_stream_pts -= gop_pts;
						}
					}
				}
			}
			break;
		case E_FJ_DEMUX_CODEC_AUDIO_AAC :
			gDmux_info.get_audio_stream_pts += gDmux_info.track_info[ gDmux_info.audio_id ].time_delta;
			break;
		case E_FJ_DEMUX_CODEC_AUDIO_PCM :
			// When PCM,4Bytes = 1PTS
			gDmux_info.get_audio_stream_pts += sample_size / 4;
			break;
			
		default :
			printf("bf_demux_stream_read : codec error!\n");
			return FALSE;
	}
	
	return TRUE;
}



static BOOL bf_demux_mp4_avcc_analyze( UCHAR* avcc_adr, ULONG avcc_size, T_BF_DEMUX_H264_SPS_PPS_INFO* sps_pps_info )
{
#if 0 // Unnecessary information
	UCHAR	configurationVersion;		// 8bit
	UCHAR	AVCProfileIndication;		// 8bit
	UCHAR	profile_compatibility;		// 8bit
	UCHAR	AVCLevelIndication;			// 8bit
#endif
	UCHAR	lengthSizeMinusOne;			// 2bit
	UCHAR	numOfSequenceParameterSets;	// 5bit
	UCHAR	sequenceParameterSetLength;	// 16bit
	UCHAR*	sps_address;
	UCHAR	numOfPictureParameterSets;	// 5bit
	UCHAR	pictureParameterSetLength;	// 16bit
	UCHAR*	pps_address;
	T_MP4_COMMON_STREAM_CONTROL	bit_stream;
	
	BF_MP4_Common_BS_Set_Pointer_Address( &bit_stream, avcc_adr );
	
	
#if 0 // Unnecessary information
	configurationVersion		= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 8 );
	AVCProfileIndication		= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 8 );
	profile_compatibility		= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 8 );
	AVCLevelIndication			= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 8 );
#else
	BF_MP4_Common_BS_Skip_Pointer_Address( &bit_stream, 4 );	// 4byte(32bit) skip
#endif
	BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( &bit_stream, 6 );	// reserved
	lengthSizeMinusOne			= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 2 );
	if( lengthSizeMinusOne != 3 ){
		return FALSE;
	}
	BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( &bit_stream, 3 );	// reserved
	
	numOfSequenceParameterSets	= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 5 );
	if(numOfSequenceParameterSets > 1){
		return FALSE;
	}
	sequenceParameterSetLength	= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 16 );
	sps_address					= BF_MP4_Common_BS_Get_Pointer( &bit_stream );
	BF_MP4_Common_BS_Skip_Pointer_Address( &bit_stream, sequenceParameterSetLength );
	
	numOfPictureParameterSets	= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 8 );
	if(numOfPictureParameterSets > 1){
		return FALSE;
	}
	pictureParameterSetLength	= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 16 );
	pps_address					= BF_MP4_Common_BS_Get_Pointer( &bit_stream );
	BF_MP4_Common_BS_Skip_Pointer_Address( &bit_stream, pictureParameterSetLength );
	
	// set SPS/PPS information
	sps_pps_info->sps_size    = sequenceParameterSetLength;
	sps_pps_info->sps_address = sps_address;
	sps_pps_info->pps_size    = pictureParameterSetLength;
	sps_pps_info->pps_address = pps_address;
	
	// input data check
	if(avcc_size != ((ULONG)BF_MP4_Common_BS_Get_Pointer( &bit_stream ) - (ULONG)avcc_adr)){
		sps_pps_info->sps_size    = 0;
		sps_pps_info->sps_address = 0;
		sps_pps_info->pps_size    = 0;
		sps_pps_info->pps_address = 0;
		return FALSE;
	}
	
	return TRUE;
}

#if 0	// use sps analyze
static VOID bf_demux_h264_sps_analyze( T_BF_DEMUX_H264_SPS* sps, ULONG sps_addr )
{
	T_MP4_COMMON_STREAM_CONTROL	bit_stream;
	ULONG						loop;
	
	BF_MP4_Common_BS_Set_Pointer_Address( &bit_stream, (UCHAR*)sps_addr );
	
	// SPS Analyze start
	sps->profile_idc			= BF_MP4_Common_BS_Read_U_Nbit(&bit_stream, 8);
	sps->constraint_set0_flag	= BF_MP4_Common_BS_Read_U_1bit(&bit_stream);
	sps->constraint_set1_flag	= BF_MP4_Common_BS_Read_U_1bit(&bit_stream);
	sps->constraint_set2_flag	= BF_MP4_Common_BS_Read_U_1bit(&bit_stream);
	sps->constraint_set3_flag	= BF_MP4_Common_BS_Read_U_1bit(&bit_stream);
	sps->reserved_zero_4bits	= BF_MP4_Common_BS_Read_U_Nbit(&bit_stream,4);
	sps->level_idc				= BF_MP4_Common_BS_Read_U_Nbit(&bit_stream, 8);
	sps->seq_parameter_set_id	= BF_MP4_Common_BS_Read_U_E(&bit_stream);
	
	if( sps->profile_idc == 100 || sps->profile_idc == 110 ||
		sps->profile_idc == 122 || sps->profile_idc == 144 ){
		
		sps->chroma_format_idc	= BF_MP4_Common_BS_Read_U_E(&bit_stream);
		
		if( sps->chroma_format_idc == 3 ){
			sps->separate_colour_plane_flag	= BF_MP4_Common_BS_Read_U_1bit(&bit_stream);
		}
		
		sps->bit_depth_luma_minus8					= BF_MP4_Common_BS_Read_U_E(&bit_stream);
		sps->bit_depth_chroma_minus8				= BF_MP4_Common_BS_Read_U_E(&bit_stream);
		sps->qpprime_y_zero_transform_bypass_flag	= BF_MP4_Common_BS_Read_U_1bit(&bit_stream);
		sps->seq_scaling_matrix_present_flag		= BF_MP4_Common_BS_Read_U_1bit(&bit_stream);
		
		if( sps->seq_scaling_matrix_present_flag ){
			for( loop = 0; loop < 8; loop++ ){
				sps->seq_scaling_list_present_flag[loop] = BF_MP4_Common_BS_Read_U_1bit(&bit_stream);
				if( sps->seq_scaling_list_present_flag[loop] ){
					if( loop < 6 ){
						bf_demux_h264_bs_read_scaling_list( &bit_stream, sps->ScalingList4x4[loop],		/* pgr0013 */ /* pgr0862 */
						16, &sps->UseDefaultScalingMatrix4x4Flag[loop]);								/* pgr0013 */ /* pgr0862 */
					}
					else{
						bf_demux_h264_bs_read_scaling_list( &bit_stream, sps->ScalingList8x8[loop - 6],
						64, &sps->UseDefaultScalingMatrix8x8Flag[loop - 6] );
					}
				}
			}
		}
	}
	
	sps->log2_max_frame_num_minus4	= BF_MP4_Common_BS_Read_U_E(&bit_stream);
	sps->pic_order_cnt_type			= BF_MP4_Common_BS_Read_U_E(&bit_stream);
	
	if( sps->pic_order_cnt_type == 0 ){
		sps->log2_max_pic_order_cnt_lsb_minus4	= BF_MP4_Common_BS_Read_U_E(&bit_stream);
	}
	else if( sps->pic_order_cnt_type == 1 ){
		sps->delta_pic_order_always_zero_flag		= BF_MP4_Common_BS_Read_U_1bit(&bit_stream);
		sps->offset_for_non_ref_pic					= BF_MP4_Common_BS_Read_S_E(&bit_stream);
		sps->offset_for_top_to_bottom_field			= BF_MP4_Common_BS_Read_S_E(&bit_stream);
		sps->num_ref_frames_in_pic_order_cnt_cycle	= BF_MP4_Common_BS_Read_U_E(&bit_stream);
		
		for( loop = 0; loop < sps->num_ref_frames_in_pic_order_cnt_cycle; loop++ ){
			sps->offset_for_ref_frame[loop]			= BF_MP4_Common_BS_Read_S_E(&bit_stream);
		}
	}
	
	sps->num_ref_frames							= BF_MP4_Common_BS_Read_U_E(&bit_stream);
	sps->gaps_in_frame_num_value_allowed_flag	= BF_MP4_Common_BS_Read_U_1bit(&bit_stream);
	sps->pic_width_in_mbs_minus1				= BF_MP4_Common_BS_Read_U_E(&bit_stream);
	sps->pic_height_in_map_units_minus1			= BF_MP4_Common_BS_Read_U_E(&bit_stream);
	sps->frame_mbs_only_flag					= BF_MP4_Common_BS_Read_U_1bit(&bit_stream);
}

/**
 *	@brief		Bit Stream Read(Scaling List).\n
 *	@param		IN :Bit Stream Information
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
static VOID bf_demux_h264_bs_read_scaling_list( T_MP4_COMMON_STREAM_CONTROL* bit_stream, INT32* scalingList, INT32 sizeOfScalingList, INT32* useDefaultScalingMatrixFlag )
{
	INT32	loop;
	INT32	delta_scale;
	INT32	lastScale = 8;
	INT32	nextScale = 8;
	
	for( loop = 0; loop < sizeOfScalingList; loop++ ){
		if( nextScale != 0 ){
			delta_scale = BF_MP4_Common_BS_Read_S_E(bit_stream);
			nextScale = ( lastScale + delta_scale + 256 ) % 256;
			*useDefaultScalingMatrixFlag = ( loop == 0 && nextScale == 0 );
		}
		scalingList[loop] = ( nextScale == 0 ) ? lastScale : nextScale;
		lastScale = scalingList[loop];
	}
}
#endif


static BOOL bf_demux_h264_sps_pps_attach( UCHAR* video_sample_addr_base, UCHAR* video_sample_addr )
{
	UCHAR*			video_sample_addr_work;
	ULONG			aud_size=0;
	ULONG			nal_size=0;
	UCHAR*			nal_addr;
	
	// SPS/PPS attach
	/*
             [1st step]
             +-------------------+-+-----+-+-----------------+
             |                   | | AUD | | Slice           |
             +-------------------+-+-----+-+-----------------+
             |                   |                            
          addr_base             addr                          
                                                              
             [2nd step]
             +-+-----+ ------------------+-+-----------------+
             | | AUD |                   | | Slice           |
             +-+-----+-------------------+-+-----------------+
                                                              
             [3rd step]
             +-+------+-+-------+-+------+-+-----------------+
             | | AUD  | | SPS   | | PPS  | | Slice           |
             +-+------+-+-------+-+------+-+-----------------+
             4+aud    4+sps      4+pps      4+slice size
	 */
	
	// 2nd step: AUD -> top area
	video_sample_addr_work = video_sample_addr_base;
	{
		ULONG	value;
		ULONG	result;
		memcpy( &value, video_sample_addr, 4 );	// get AUD size (2byte)
		result =	(((value) & 0x000000ff) << 24) | (((value) & 0x0000ff00) <<  8) | 
					(((value) & 0x00ff0000) >>  8) | (((value) & 0xff000000) >> 24);
		aud_size =	result;
	}
	aud_size += 4;															// add address [start code(4byte) + AUD size(2byte)]
	memcpy( video_sample_addr_work, video_sample_addr, aud_size );			// AUD -> top area
	memset( video_sample_addr, 0x00, aud_size );							// OLD AUD Zero Fill
	
	video_sample_addr += aud_size;											// move Slice top address(for address check after.)
	
	// 3rd step: SPS/PPS copy
	video_sample_addr_work += aud_size;										// move SPS top address
	nal_size = gDmux_info.sps_pps_info.sps_size;
	nal_addr = gDmux_info.sps_pps_info.sps_address;
	{
		ULONG	value;
		ULONG	result;
		value  =	nal_size;
		result =	(((value) & 0x000000ff) << 24) | (((value) & 0x0000ff00) <<  8) | 
					(((value) & 0x00ff0000) >>  8) | (((value) & 0xff000000) >> 24);
		memcpy( video_sample_addr_work, &result, 4 );						// SPS size (4byte)
	}
	video_sample_addr_work += 4;
	memcpy( video_sample_addr_work, nal_addr, nal_size );
	
	video_sample_addr_work += nal_size;										// move PPS top address
	nal_size = gDmux_info.sps_pps_info.pps_size;
	nal_addr = gDmux_info.sps_pps_info.pps_address;
	{
		ULONG	value;
		ULONG	result;
		value  =	nal_size;
		result =	(((value) & 0x000000ff) << 24) | (((value) & 0x0000ff00) <<  8) | 
					(((value) & 0x00ff0000) >>  8) | (((value) & 0xff000000) >> 24);
		memcpy( video_sample_addr_work, &result, 4 );						// SPS size (4byte)
	}
	video_sample_addr_work += 4;
	memcpy( video_sample_addr_work, nal_addr, nal_size );
	
	video_sample_addr_work += nal_size;										// move Slice top address
	// check 
	if( video_sample_addr != video_sample_addr_work ){
		printf("bf_demux_h264_sps_pps_attach : SPS/PPS Attach Error\n");
		return FALSE;
	}
	
	return TRUE;
}


static BOOL bf_demux_h264_startcode_replace( UCHAR* video_sample_addr, ULONG video_sample_size )
{
	UCHAR*				nal_addr;
	ULONG				nal_size;
	UCHAR				start_code[4] = {0x00,0x00,0x00,0x01};
	
	//  NAL size <-> NAL header replace
	/*
      +-+-----+-+------------------+
      | | AUD | | Slice            | 
      +-+-----+-+------------------+ 
	*/
	nal_addr = video_sample_addr;
	while(1){
		// AUD,(SPS,PPS,)Slice,,,
		{
			ULONG	value;
			ULONG	result;
			memcpy( &value, nal_addr, 4 );	// get NAL size
			result =	(((value) & 0x000000ff) << 24) | (((value) & 0x0000ff00) <<  8) | 
						(((value) & 0x00ff0000) >>  8) | (((value) & 0xff000000) >> 24);
			nal_size =	result;
		}
		memcpy( nal_addr, &start_code, 4 );	// NAL size -> start code (4byte) replace
		nal_addr += (4 + nal_size);			// add address [start code(4byte) + NAL size(XXbyte)]
		
		if( nal_size >= video_sample_size){
			printf("bf_demux_h264_startcode_replace : Start code replase error \n");
			return FALSE;
		}
		if( nal_addr >= ((UCHAR*)(video_sample_addr + video_sample_size)) ){
			break;
		}
	}
	
	return TRUE;
}



static BOOL bf_demux_mp4_hvcc_analyze( UINT8* hvcc_adr, UINT32 hvcc_size, T_BF_DEMUX_H265_PS_INFO* ps_info )
{
	UINT16		array_loop;
	UINT16		nal_loop;
#ifdef CO_DEBUG_DEMUX_SHOW_HVCC_DETAILS
	UINT8 		configurationVersion;					// 8bit
	UINT8 		general_profile_space;					// 2bit
	UINT8 		general_tier_flag;						// 1bit
	UINT8 		general_profile_idc;					// 5bit
	UINT32 		general_profile_compatibility_flags;	// 32bit
	UINT64 		general_constraint_indicator_flags;		// 48bit
	UINT8 		general_level_idc;						// 8bit
	UINT16 		min_spatial_segmentation_idc;			// 12bit
	UINT8 		parallelismType;						// 2bit
	UINT8 		chromaFormat;							// 2bit
	UINT8 		bitDepthLumaMinus8;						// 3bit
	UINT8 		bitDepthChromaMinus8;					// 3bit
	UINT16 		avgFrameRate;							// 16bit
	UINT8 		constantFrameRate;						// 2bit
	UINT8 		numTemporalLayers;						// 3bit
	UINT8 		temporalIdNested;						// 1bit
#endif
	UINT8 		lengthSizeMinusOne;						// 2bit
	UINT8 		numOfArrays;							// 8bit
#ifdef CO_DEBUG_DEMUX_SHOW_HVCC_DETAILS
	UINT8 		array_completeness;						// 1bit
	UINT8 		NAL_unit_type;							// 6bit
#endif
	UINT16 		numNalus;								// 16bit
	UINT16	 	nalUnitLength;							// 16bit
	T_MP4_COMMON_STREAM_CONTROL	bit_stream;
	
	BF_MP4_Common_BS_Set_Pointer_Address( &bit_stream, hvcc_adr );
	
#ifdef CO_DEBUG_DEMUX_SHOW_HVCC_DETAILS
	configurationVersion				= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 8 );
	general_profile_space				= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 2 );
	general_tier_flag					= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 1 );
	general_profile_idc					= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 5 );
	general_profile_compatibility_flags	= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 32 );
	general_constraint_indicator_flags	= BF_MP4_Common_BS_Read_64_U_Nbit( &bit_stream, 48 );
	general_level_idc					= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 8 );
	BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( &bit_stream, 4 );	// reserved 4bit
	min_spatial_segmentation_idc		= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 12 );
	BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( &bit_stream, 6 );	// reserved 6bit
	parallelismType						= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 2 );
	BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( &bit_stream, 6 );	// reserved 6bit
	chromaFormat						= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 2 );
	BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( &bit_stream, 5 );	// reserved 5bit
	bitDepthLumaMinus8					= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 3 );
	BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( &bit_stream, 5 );	// reserved 5bit
	bitDepthChromaMinus8				= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 3 );
	avgFrameRate						= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 16 );
	constantFrameRate					= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 2 );
	numTemporalLayers					= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 3 );
	temporalIdNested					= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 1 );
#else
	BF_MP4_Common_BS_Skip_Pointer_Address( &bit_stream, 21 );	// 21byte
	BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( &bit_stream, 6 );	// 6bit
#endif
	lengthSizeMinusOne					= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 2 );
	if( lengthSizeMinusOne != 3 ){
		printf("bf_demux_mp4_hvcc_analyze : Error. lengthSizeMinusOne is 3 other value.\n");
		return FALSE;
	}
	numOfArrays							= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 8 );
	
#ifdef CO_DEBUG_DEMUX_SHOW_HVCC_DETAILS
	printf("\n");
	printf("configurationVersion                = %d\n",   configurationVersion                );
	printf("general_profile_space               = %d\n",   general_profile_space               );
	printf("general_tier_flag                   = %d\n",   general_tier_flag                   );
	printf("general_profile_idc                 = %d\n",   general_profile_idc                 );
	printf("general_profile_compatibility_flags = %d\n",   general_profile_compatibility_flags );
	printf("general_constraint_indicator_flags  = %lld\n", general_constraint_indicator_flags  );
	printf("general_level_idc                   = %d\n",   general_level_idc                   );
	printf("min_spatial_segmentation_idc        = %d\n",   min_spatial_segmentation_idc        );
	printf("parallelismType                     = %d\n",   parallelismType                     );
	printf("chromaFormat                        = %d\n",   chromaFormat                        );
	printf("bitDepthLumaMinus8                  = %d\n",   bitDepthLumaMinus8                  );
	printf("bitDepthChromaMinus8                = %d\n",   bitDepthChromaMinus8                );
	printf("avgFrameRate                        = %d\n",   avgFrameRate                        );
	printf("constantFrameRate                   = %d\n",   constantFrameRate                   );
	printf("numTemporalLayers                   = %d\n",   numTemporalLayers                   );
	printf("temporalIdNested                    = %d\n",   temporalIdNested                    );
	printf("lengthSizeMinusOne                  = %d\n",   lengthSizeMinusOne                  );
	printf("----------------------------------------\n");
	printf("numOfArrays                         = %d\n",   numOfArrays                         );
#endif	// for Debug
	
	ps_info->total_nal_num	= 0;
	ps_info->total_nal_size	= 0;
	
	for( array_loop = 0; array_loop < numOfArrays; array_loop++ ){
#ifdef CO_DEBUG_DEMUX_SHOW_HVCC_DETAILS
		array_completeness				= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 1 );
		BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( &bit_stream, 1 );	// reserved 1bit
		NAL_unit_type					= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 6 );
#else
		BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( &bit_stream, 8 );
#endif
		numNalus						= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 16 );
		
#ifdef CO_DEBUG_DEMUX_SHOW_HVCC_DETAILS
		printf("array_completeness                  = %d\n",   array_completeness                  );
		printf("NAL_unit_type                       = %d\n",   NAL_unit_type                       );
		printf("--------------------\n");
		printf("numNalus                            = %d\n",   numNalus                            );
#endif	// for Debug
		
		for( nal_loop = 0; nal_loop < numNalus; nal_loop++ ){
			if( ps_info->total_nal_num >= D_BF_DEMUX_HVCC_NAL_NUM_MAX ){
				printf("bf_demux_mp4_hvcc_analyze : Error. It has exceeded the maximum number of save of the NAL unit.\n");
				return FALSE;
			}
			nalUnitLength									= BF_MP4_Common_BS_Read_U_Nbit( &bit_stream, 16 );
			ps_info->nal_size[ps_info->total_nal_num]		= nalUnitLength;
			ps_info->nal_address[ps_info->total_nal_num]	= BF_MP4_Common_BS_Get_Pointer( &bit_stream );
			ps_info->total_nal_size							+= ps_info->nal_size[ps_info->total_nal_num];
			ps_info->total_nal_num++;
			
			BF_MP4_Common_BS_Skip_Pointer_Address( &bit_stream, nalUnitLength );	// skip byte
			
#ifdef CO_DEBUG_DEMUX_SHOW_HVCC_DETAILS
			printf("nalUnitLength                       = %d\n",   nalUnitLength                       );
		printf("--------------------\n");
#endif	// for Debug
		
		}
	}
#ifdef CO_DEBUG_DEMUX_SHOW_HVCC_DETAILS
	printf("----------------------------------------\n");
#endif	// for Debug
	
	// input data check
	if(hvcc_size != ((UINT32)BF_MP4_Common_BS_Get_Pointer( &bit_stream ) - (UINT32)hvcc_adr)){
		printf("bf_demux_mp4_hvcc_analyze : Error. Read size and the data length does not match.\n");
		return FALSE;
	}
	
	return TRUE;
}

static BOOL bf_demux_h265_ps_attach( UCHAR* video_sample_addr_base, UCHAR* video_sample_addr, T_BF_DEMUX_H265_PS_INFO* ps_info )
{
	UINT8			nal_type;
	UINT32			nal_loop;
	UINT8*			video_sample_addr_work;
#if 0
	ULONG			aud_size=0;
	ULONG			nal_size=0;
	UCHAR*			nal_addr;
#endif
	
	
	nal_type = video_sample_addr[4] >> 1;
	video_sample_addr_work = video_sample_addr_base;
	
	if( nal_type != D_BF_DEMUX_H265_NALTYPE_AUD ){
		
		// If the beginning of the NAL is not in AUD.
		for( nal_loop = 0; nal_loop < ps_info->total_nal_num; nal_loop++ ){
			{
				ULONG	value;
				value =		(((ps_info->nal_size[nal_loop]) & 0x000000ff) << 24) | (((ps_info->nal_size[nal_loop]) & 0x0000ff00) <<  8) | 
							(((ps_info->nal_size[nal_loop]) & 0x00ff0000) >>  8) | (((ps_info->nal_size[nal_loop]) & 0xff000000) >> 24);
				memcpy( video_sample_addr_work, &value, 4 );
			}
			
			video_sample_addr_work += 4;
			memcpy( video_sample_addr_work, ps_info->nal_address[nal_loop], ps_info->nal_size[nal_loop] );
			video_sample_addr_work += ps_info->nal_size[nal_loop];
		}
	}
	else{
		// If the beginning of the NAL is AUD.
		
#if 0	// Not maintenance.
		// PS NAL attach
		/*
	             [1st step]
	             +-------------------+-+-----+-+-----------------+
	             |                   | | AUD | | Slice           |
	             +-------------------+-+-----+-+-----------------+
	             |                   |                            
	          addr_base             addr                          
	                                                              
	             [2nd step]
	             +-+-----+ ------------------+-+-----------------+
	             | | AUD |                   | | Slice           |
	             +-+-----+-------------------+-+-----------------+
	                                                              
	             [3rd step]
	             +-+------+-+-------+-+------+-+-----------------+
	             | | AUD  | | SPS   | | PPS  | | Slice           |
	             +-+------+-+-------+-+------+-+-----------------+
	             4+aud    4+sps      4+pps      4+slice size
		 */
		
		// 2nd step: AUD -> top area
		{
			ULONG	value;
			ULONG	result;
			memcpy( &value, video_sample_addr, 4 );	// get AUD size (2byte)
			result =	(((value) & 0x000000ff) << 24) | (((value) & 0x0000ff00) <<  8) | 
						(((value) & 0x00ff0000) >>  8) | (((value) & 0xff000000) >> 24);
			aud_size =	result;
		}
		aud_size += 4;															// add address [start code(4byte) + AUD size(2byte)]
		memcpy( video_sample_addr_work, video_sample_addr, aud_size );			// AUD -> top area
		memset( video_sample_addr, 0x00, aud_size );							// OLD AUD Zero Fill
		
		video_sample_addr += aud_size;											// move Slice top address(for address check after.)
		
		// 3rd step: SPS/PPS copy
		video_sample_addr_work += aud_size;										// move SPS top address
		nal_size = gDmux_info.sps_pps_info.sps_size;
		nal_addr = gDmux_info.sps_pps_info.sps_address;
		{
			ULONG	value;
			ULONG	result;
			value  =	nal_size;
			result =	(((value) & 0x000000ff) << 24) | (((value) & 0x0000ff00) <<  8) | 
						(((value) & 0x00ff0000) >>  8) | (((value) & 0xff000000) >> 24);
			memcpy( video_sample_addr_work, &result, 4 );						// SPS size (4byte)
		}
		video_sample_addr_work += 4;
		memcpy( video_sample_addr_work, nal_addr, nal_size );
		
		video_sample_addr_work += nal_size;										// move PPS top address
		nal_size = gDmux_info.sps_pps_info.pps_size;
		nal_addr = gDmux_info.sps_pps_info.pps_address;
		{
			ULONG	value;
			ULONG	result;
			value  =	nal_size;
			result =	(((value) & 0x000000ff) << 24) | (((value) & 0x0000ff00) <<  8) | 
						(((value) & 0x00ff0000) >>  8) | (((value) & 0xff000000) >> 24);
			memcpy( video_sample_addr_work, &result, 4 );						// SPS size (4byte)
		}
		video_sample_addr_work += 4;
		memcpy( video_sample_addr_work, nal_addr, nal_size );
		
		video_sample_addr_work += nal_size;										// move Slice top address
		
		
		// check
		if( video_sample_addr != video_sample_addr_work ){
			printf("bf_demux_h265_ps_attach : SPS/PPS Attach Error\n");
			return FALSE;
		}
#endif
	}
	
	// check
	if( video_sample_addr != video_sample_addr_work ){
		printf("bf_demux_h265_ps_attach : PS NAL Attach Error\n");
		return FALSE;
	}
	
	return TRUE;
}

static BOOL bf_demux_h265_startcode_replace( UCHAR* video_sample_addr, ULONG video_sample_size )
{
	return bf_demux_h264_startcode_replace( video_sample_addr, video_sample_size );
}





VOID BF_Demux_Controller( INT32 stacd )
{
	BOOL					ret;
	OS_USER_FLGPTN			flgptn;
	OS_USER_FLGPTN			waiptn;
	UINT32					stream_loop;
	T_FJ_DEMUX_STREAM_PARAM	async_video_stream_param;
	T_FJ_DEMUX_STREAM_PARAM	async_audio_stream_param;
	
	memset( (VOID*)&async_video_stream_param, 0, sizeof(async_video_stream_param) );
	memset( (VOID*)&async_audio_stream_param, 0, sizeof(async_audio_stream_param) );
	
	printf("BF_Demux_Controller Start.\n");
	
	waiptn = 	(FLG_DEMUX_VIDEO_SETUP_REQUEST |
				FLG_DEMUX_AUDIO_SETUP_REQUEST |
				FLG_DEMUX_FREE_ATOM_SIZE_GET_REQUEST |
				FLG_DEMUX_FREE_ATOM_GET_REQUEST |
				FLG_DEMUX_VIDEO_STREAM_READ_REQUEST |
				FLG_DEMUX_AUDIO_STREAM_READ_REQUEST |
				FLG_DEMUX_VIDEO_STREAM_SYNC_READ_REQUEST |
				FLG_DEMUX_AUDIO_STREAM_SYNC_READ_REQUEST |
				FLG_DEMUX_FINALIZE_REQUEST);
	
	while(1){
		
		OS_User_Wai_Flg( FID_DEMUX, waiptn, D_OS_USER_TWF_ORW, &flgptn );
		
		// Audio Setup
		if( flgptn & FLG_DEMUX_AUDIO_SETUP_REQUEST ){
#if 0	// for Debug
			printf("Demux : Audio Setup.\n");
#endif	// for Debug
			OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_AUDIO_SETUP_REQUEST );
			OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_AUDIO_STREAM_SYNC_READ_REQUEST );
			OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_AUDIO_STREAM_READ_REQUEST );
			
			memset( (VOID*)&async_audio_stream_param, 0, sizeof(async_audio_stream_param) );
			bf_demux_audio_setup( gDmux_info.audio_setup_message.start_pts );
			OS_User_Set_Flg( FID_DEMUX, FLG_DEMUX_AUDIO_SETUP_COMPLETE);
		}
		// Video Setup
		else if( flgptn & FLG_DEMUX_VIDEO_SETUP_REQUEST ){
#if 0	// for Debug
			printf("Demux : Video Setup.\n");
#endif	// for Debug
			OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_VIDEO_SETUP_REQUEST );
			OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_VIDEO_STREAM_SYNC_READ_REQUEST );
			OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_VIDEO_STREAM_READ_REQUEST );
			
			memset( (VOID*)&async_video_stream_param, 0, sizeof(async_video_stream_param) );
			bf_demux_video_setup( gDmux_info.video_setup_message.start_pts, gDmux_info.video_setup_message.direction, gDmux_info.video_setup_message.speed );
			OS_User_Set_Flg( FID_DEMUX, FLG_DEMUX_VIDEO_SETUP_COMPLETE);
		}
		// Free Atom Size Get.
		else if( flgptn & FLG_DEMUX_FREE_ATOM_SIZE_GET_REQUEST ){
#if 0	// for Debug
			printf("Demux : Free Atom Size Get.\n");
#endif	// for Debug
			OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_FREE_ATOM_SIZE_GET_REQUEST );
			
			bf_demux_freeatom_size_get( (UINT32 *)&gDmux_info.free_atom_size_get_message.data_size );
			
			OS_User_Set_Flg( FID_DEMUX, FLG_DEMUX_FREE_ATOM_SIZE_GET_COMPLETE);
		}
		// Free Atom Get.
		else if( flgptn & FLG_DEMUX_FREE_ATOM_GET_REQUEST ){
#if 0	// for Debug
			printf("Demux : Free Atom Get.\n");
#endif	// for Debug
			OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_FREE_ATOM_GET_REQUEST );
			
			bf_demux_freeatom_get( gDmux_info.free_atom_get_message.data_address );
			
			OS_User_Set_Flg( FID_DEMUX, FLG_DEMUX_FREE_ATOM_GET_COMPLETE);
		}
		// Sync Read Audio Stream.
		else if( flgptn & FLG_DEMUX_AUDIO_STREAM_SYNC_READ_REQUEST ){
#if 0	// for Debug
			printf("Demux : Sync Read Audio Stream.\n");
#endif	// for Debug
			OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_AUDIO_STREAM_SYNC_READ_REQUEST );
			
			gDmux_info.audio_stream_sync_read_message.stream_param.stream_num	= 0;
			for( stream_loop=0; stream_loop<gDmux_info.audio_stream_sync_read_message.read_stream_num; stream_loop++ ){
				if( FALSE == bf_demux_stream_read( gDmux_info.audio_codec, (T_FJ_DEMUX_STREAM_PARAM*)&gDmux_info.audio_stream_sync_read_message.stream_param ) ){
					break;
				}
				if( gDmux_info.audio_stream_sync_read_message.stream_param.stream_end_flag[stream_loop] == TRUE ){
					break;
				}
			}
			OS_User_Set_Flg( FID_DEMUX, FLG_DEMUX_AUDIO_STREAM_SYNC_READ_COMPLETE);
		}
		// Sync Read Video Stream.
		else if( flgptn & FLG_DEMUX_VIDEO_STREAM_SYNC_READ_REQUEST ){
#if 0	// for Debug
			printf("Demux : Sync Read Video Stream.\n");
#endif	// for Debug
			OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_VIDEO_STREAM_SYNC_READ_REQUEST );
			
			gDmux_info.video_stream_sync_read_message.stream_param.stream_num	= 0;
			for( stream_loop=0; stream_loop<gDmux_info.video_stream_sync_read_message.read_stream_num; stream_loop++ ){
				if( FALSE == bf_demux_stream_read( gDmux_info.video_codec, (T_FJ_DEMUX_STREAM_PARAM*)&gDmux_info.video_stream_sync_read_message.stream_param ) ){
					break;
				}
				if( gDmux_info.video_stream_sync_read_message.stream_param.stream_end_flag[stream_loop] == TRUE ){
					break;
				}
			}
			OS_User_Set_Flg( FID_DEMUX, FLG_DEMUX_VIDEO_STREAM_SYNC_READ_COMPLETE);
		}
		// Async Read Audio Stream.
		else if( flgptn & FLG_DEMUX_AUDIO_STREAM_READ_REQUEST ){
#if 0	// for Debug
			printf("Demux : Async Read Audio Stream.\n");
#endif	// for Debug
			OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_AUDIO_STREAM_READ_REQUEST );
			
			if( async_audio_stream_param.stream_num == 0 ){
				for( stream_loop=0; stream_loop<gDmux_info.audio_stream_read_message.read_stream_num; stream_loop++ ){
					if( FALSE == bf_demux_stream_read( gDmux_info.audio_codec, &async_audio_stream_param ) ){
						break;
					}
					if( async_audio_stream_param.stream_end_flag[stream_loop] == TRUE ){
						break;
					}
				}
			}
			if( async_audio_stream_param.stream_num != 0 ){
				ret = FJ_Host_Demux_StreamReadCompleteCB( gDmux_info.file_id, gDmux_info.audio_codec, &async_audio_stream_param );
				if(ret == TRUE){
					async_audio_stream_param.stream_num = 0;
				}
				else{
					// DO NOTHING (It will be a retry of feed.)
				}
			}
			gDmux_info.audio_stream_read_message.use_flg = FALSE;
		}
		// Async Read Video Stream.
		else if( flgptn & FLG_DEMUX_VIDEO_STREAM_READ_REQUEST ){
#if 0	// for Debug
			printf("Demux : Async Read Video Stream.\n");
#endif	// for Debug
			OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_VIDEO_STREAM_READ_REQUEST );
			
			if( async_video_stream_param.stream_num == 0 ){
				for( stream_loop=0; stream_loop<gDmux_info.video_stream_read_message.read_stream_num; stream_loop++ ){
					if( FALSE == bf_demux_stream_read( gDmux_info.video_codec, &async_video_stream_param ) ){
						break;
					}
					if( async_video_stream_param.stream_end_flag[stream_loop] == TRUE ){
						break;
					}
				}
			}
			if( async_video_stream_param.stream_num != 0 ){
				ret = FJ_Host_Demux_StreamReadCompleteCB( gDmux_info.file_id, gDmux_info.video_codec, &async_video_stream_param );
				if( ret == TRUE ){
					async_video_stream_param.stream_num = 0;
				}
				else{
					// DO NOTHING (It will be a retry of feed.)
				}
			}
			gDmux_info.video_stream_read_message.use_flg = FALSE;
		}
		// Finalize
		else if( flgptn & FLG_DEMUX_FINALIZE_REQUEST ){
#if 0	// for Debug
			printf("Demux : Finalize.\n");
#endif	// for Debug
			OS_User_Clr_Flg( FID_DEMUX, ~FLG_DEMUX_FINALIZE_REQUEST );
			
			bf_demux_memory_close( (T_BF_DEMUX_INFO *)&gDmux_info );
			OS_User_Set_Flg( FID_DEMUX, FLG_DEMUX_FINALIZE_COMPLETE);
			break;
		}
		else{
			printf("BF_Demux_Controller : Error!! Undefined flag has been set.\n");
			break;
		}
	}
	
	printf("BF_Demux_Controller End.\n");
	OS_User_Ext_Tsk();
}

