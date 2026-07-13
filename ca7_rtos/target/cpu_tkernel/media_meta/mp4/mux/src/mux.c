/**
 * @file		mux.c
 * @brief		Mux API.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*----------------------------------------------------------------------*/
/* Include Files														*/
/*----------------------------------------------------------------------*/
#include "fj_encoder.h"
#include "fj_host.h"
#include "debug.h"

#include <string.h>
#include <stdlib.h>
#include "mux.h"
#include "mp4_common.h"
#include "audio_encoder.h"
#if ( CO_MOVIE_FF_AUDIO_PATTERN == 0 )
#include "libmp4f.h"
#else	// CO_MOVIE_FF_AUDIO_PATTERN
#include "libmovf.h"
#endif	// CO_MOVIE_FF_AUDIO_PATTERN
#include "sdram_map_movie_record.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* semaphore */
#define D_BF_MUX_LOCK()									OS_User_Wai_Sem( SID_MUX )
#define D_BF_MUX_UNLOCK()								OS_User_Sig_Sem( SID_MUX )

/* Header Informations Size */
#define D_BF_MUX_MP4_ATOM_VIDEO_SIZE					(4096)
#define D_BF_MUX_MP4_ATOM_AUDIO_SIZE					(4096)
#define D_BF_MUX_MP4_ATOM_VIDEO_CHUNK(gopnum)			((4*gopnum)+4+(8*10)+4)	/* stsz:4*gopnum, stco:4, ctts:8*10, stss:4 ((4*gopnum)+4+(8*10)+4) */
#define D_BF_MUX_MP4_ATOM_AUDIO_AAC_CHUNK				(112)					/* stsz:4*24, stco:4, stsc:12 ((4*24)+4+12) for AAC */
#define D_BF_MUX_MP4_ATOM_AUDIO_PCM_CHUNK				(8)						/* stsz:4, stco:4 (4+4) for Linear PCM */

/* Number of Track */
#define D_BF_MUX_MP4_TRACK_MAX_NUM						(2)

/* End of Sequence and End of Stream size */
#define D_BF_MUX_EOS_SIZE								(10)

#define D_BF_MUX_H265_STARTCODE_SIZE					(4)	// 4byte
#define D_BF_MUX_H265_NAL_HEADER_SIZE					(2)	// 2byte
#define D_BF_MUX_H265_SUBLAYER_MAX_NUM					(7)
#define D_BF_MUX_H265_SHORT_TERM_RPS_MAX_NUM			(64)
#define D_BF_MUX_H265_LONG_TERM_RPS_SPS_MAX_NUM			(32)
#define D_BF_MUX_H265_SPATIAL_SEGMENTATION_MAX_RANGE	(4095)
#define D_BF_MUX_H265_ASPECT_RATIO_IDC_UNSPECIFIED		(0)
#define D_BF_MUX_H265_ASPECT_RATIO_IDC_1_1				(1)
#define D_BF_MUX_H265_ASPECT_RATIO_IDC_12_11			(2)
#define D_BF_MUX_H265_ASPECT_RATIO_IDC_10_11			(3)
#define D_BF_MUX_H265_ASPECT_RATIO_IDC_16_11			(4)
#define D_BF_MUX_H265_ASPECT_RATIO_IDC_40_33			(5)
#define D_BF_MUX_H265_ASPECT_RATIO_IDC_24_11			(6)
#define D_BF_MUX_H265_ASPECT_RATIO_IDC_20_11			(7)
#define D_BF_MUX_H265_ASPECT_RATIO_IDC_32_11			(8)
#define D_BF_MUX_H265_ASPECT_RATIO_IDC_80_33			(9)
#define D_BF_MUX_H265_ASPECT_RATIO_IDC_18_11			(10)
#define D_BF_MUX_H265_ASPECT_RATIO_IDC_15_11			(11)
#define D_BF_MUX_H265_ASPECT_RATIO_IDC_64_33			(12)
#define D_BF_MUX_H265_ASPECT_RATIO_IDC_160_99			(13)
#define D_BF_MUX_H265_ASPECT_RATIO_IDC_4_3				(14)
#define D_BF_MUX_H265_ASPECT_RATIO_IDC_3_2				(15)
#define D_BF_MUX_H265_ASPECT_RATIO_IDC_2_1				(16)
#define D_BF_MUX_H265_ASPECT_RATIO_IDC_EXTENDEC_SAR		(255)

#define D_BF_MUX_EVENT_CODE_STOP_4G_LIMIT				(1)
#define D_BF_MUX_EVENT_CODE_STOP_2H_LIMIT				(2)
#define D_BF_MUX_EVENT_CODE_STOP_MEDIA_FULL				(3)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct{
	UINT8						codec_type;					// Codec type.
	ULONG						media_timescale;			// for MDHD atom
	USHORT						track_parm[2];				// Video:[0]width,[1]height
	USHORT						descr_param[2];				// Video:[0]stream width,[1]stream height	Audio:[0]ch,[1]sample_size
	ULONG						mp4_dec_config_parm[5];		// for STSD atom
	ULONG						dec_specific_info_size;		// for STSD atom
	UCHAR						dec_specific_info[1000];	// for STSD atom
} T_BF_MUX_MP4_TRACK_INFO;

typedef volatile struct{
	VOID*						pre_mp4f_handle;							// pre mp4f handle
	VOID*						mp4f_handle;								// mp4f handle
	T_BF_MUX_MP4_TRACK_INFO		audio_track;								// audio track parameter
	T_BF_MUX_MP4_TRACK_INFO		video_track;								// video track parameter
	ULONG						pre_track_id[D_BF_MUX_MP4_TRACK_MAX_NUM];	//  pre track Id
	ULONG						track_id[D_BF_MUX_MP4_TRACK_MAX_NUM];		// track Id
	ULONG						pre_stsd_entry_id[D_BF_MUX_MP4_TRACK_MAX_NUM];	// stsd atom entry Id
	ULONG						stsd_entry_id[D_BF_MUX_MP4_TRACK_MAX_NUM];	// stsd atom entry Id
	ULONG						dref_entry_id[D_BF_MUX_MP4_TRACK_MAX_NUM];	// dref atom entry Id
} T_BF_MUX_FORMAT_CTRL;

typedef volatile struct{
	BOOL						new_file_request;
	T_BF_MUX_FORMAT_CTRL		format_ctrl;
	UINT8						first_flag;
	UINT8						force_stop;
	ULLONG						pre_total_duration;
	ULLONG						total_duration;
	UINT32						pre_gop_loop_cnt;
	UINT32						gop_loop_cnt;
	DOUBLE						duration_convert_ratio;
	T_FJ_MUX_VIDEO_CONFIG*		video;
} T_BF_MUX_CHANNEL_CTRL;

typedef volatile struct {
	ULONG						sample_size[SDRAM_BNK_MOVIE_RECORD0_AUDIO_STREAM];						// Audio Stream Size
	UCHAR*						sample_data[SDRAM_BNK_MOVIE_RECORD0_AUDIO_STREAM];						// Audio Stream Pointer
	ULONG						duration[SDRAM_BNK_MOVIE_RECORD0_AUDIO_STREAM];							// Audio Stream Duration
	ULONG						area_index[SDRAM_BNK_MOVIE_RECORD0_AUDIO_STREAM];						// Audio Stream Area Index
	UINT8						area_use_flg[E_FJ_MUX_CH_MAX][SDRAM_BNK_MOVIE_RECORD0_AUDIO_STREAM];	// Audio Read Index
	UINT8						end_flg[E_FJ_MUX_CH_MAX];												// Audio End Flag
	ULONG						read_index[E_FJ_MUX_CH_MAX];											// Audio Read Index
	ULONG						write_index;															// Audio Write Index
	UCHAR						sample_max_count;														// Audio Sample MAX Count depend on samplingrate
	ULLONG						audio_pre_duration[E_FJ_MUX_CH_MAX];									// Audio pre Total Duration
	ULLONG						audio_total_duration[E_FJ_MUX_CH_MAX];									// Audio Total Duration
} T_BF_MUX_AUDIO_CTRL;

typedef struct{
	ULLONG 						media_size;
	UCHAR						chunk_per_sec;
	T_BF_MUX_AUDIO_CTRL			audio_ctrl[2];
	T_BF_MUX_CHANNEL_CTRL		ch_ctrl[E_FJ_MUX_CH_MAX];
} T_BF_MUX_CTRL;

typedef struct{
	UINT32						general_profile_space;
	UINT32						general_tier_flag;
	UINT32						general_profile_idc;
	UINT32						general_profile_compatibility_flags;	// 32bit Collectively.
	UINT64						general_constraint_indicator_flags;
	UINT32						general_level_idc;
#if 0	// skip
	UINT32						sub_layer_profile_present_flag[ i ];
	UINT32						sub_layer_level_present_flag[ i ];
	UINT32						sub_layer_profile_space[ i ];
	UINT32						sub_layer_tier_flag[ i ];
	UINT32						sub_layer_profile_idc[ i ];
	UINT32						sub_layer_profile_compatibility_flag[ i ][ j ];
	UINT32						sub_layer_level_idc[ i ];
#endif	// skip
} T_BF_MUX_H265_PTL;

#if 0	// skip
typedef struct{
	UINT32						scaling_list_pred_mode_flag[ sizeId ][ matrixId ];
	UINT32						scaling_list_pred_matrix_id_delta[ sizeId ][ matrixId ];
	UINT32						scaling_list_dc_coef_minus8[ sizeId - 2 ][ matrixId ];
	UINT32						scaling_list_delta_coef;
	UINT32						ScalingList[ sizeId ][ matrixId ][ i ];
} T_BF_MUX_H265_SCALING_LIST;
#endif	// skip

#if 0	// skip
typedef struct{
	UINT32						inter_ref_pic_set_prediction_flag;
	UINT32						delta_idx_minus1;
	UINT32						delta_rps_sign;
	UINT32						abs_delta_rps_minus1;
	UINT32						used_by_curr_pic_flag[ j ];
	UINT32						use_delta_flag[ j ];
	UINT32						delta_poc_s0_minus1[ i ];
	UINT32						used_by_curr_pic_s0_flag[ i ];
	UINT32						delta_poc_s1_minus1[ i ];
	UINT32						used_by_curr_pic_s1_flag[ i ];
} T_BF_MUX_H265_ST_RPS;
#endif	// skip

typedef struct{
	UINT32						aspect_ratio_info_present_flag;
	UINT32						aspect_ratio_idc;
	UINT32						sar_width;
	UINT32						sar_height;
	UINT32						overscan_info_present_flag;
	UINT32						overscan_appropriate_flag;
	UINT32						video_signal_type_present_flag;
	UINT32						video_format;
	UINT32						video_full_range_flag;
	UINT32						colour_description_present_flag;
	UINT32						colour_primaries;
	UINT32						transfer_characteristics;
	UINT32						matrix_coeffs;
	UINT32						chroma_loc_info_present_flag;
	UINT32						chroma_sample_loc_type_top_field;
	UINT32						chroma_sample_loc_type_bottom_field;
	UINT32						neutral_chroma_indication_flag;
	UINT32						field_seq_flag;
	UINT32						frame_field_info_present_flag;
	UINT32						default_display_window_flag;
	UINT32						def_disp_win_left_offset;
	UINT32						def_disp_win_right_offset;
	UINT32						def_disp_win_top_offset;
	UINT32						def_disp_win_bottom_offset;
	UINT32						vui_timing_info_present_flag;
	UINT32						vui_num_units_in_tick;
	UINT32						vui_time_scale;
	UINT32						vui_poc_proportional_to_timing_flag;
	UINT32						vui_num_ticks_poc_diff_one_minus1;
	UINT32						vui_hrd_parameters_present_flag;
	UINT32						bitstream_restriction_flag;
	UINT32						tiles_fixed_structure_flag;
	UINT32						motion_vectors_over_pic_boundaries_flag;
	UINT32						restricted_ref_pic_lists_flag;
	UINT32						min_spatial_segmentation_idc;
	UINT32						max_bytes_per_pic_denom;
	UINT32						max_bits_per_min_cu_denom;
	UINT32						log2_max_mv_length_horizontal;
	UINT32						log2_max_mv_length_vertical;
} T_BF_MUX_H265_VUI;

typedef struct{
	UINT32						vps_video_parameter_set_id;
	UINT32						vps_base_layer_internal_flag;
	UINT32						vps_base_layer_available_flag;
	UINT32						vps_max_layers_minus1;
	UINT32						vps_max_sub_layers_minus1;
	UINT32						vps_temporal_id_nesting_flag;
	T_BF_MUX_H265_PTL			profile_tier_level;
} T_BF_MUX_H265_VPS;

typedef struct{
	UINT32						sps_video_parameter_set_id;
	UINT32						sps_max_sub_layers_minus1;
	UINT32						sps_temporal_id_nesting_flag;
	T_BF_MUX_H265_PTL			profile_tier_level;
	UINT32						sps_seq_parameter_set_id;
	UINT32						chroma_format_idc;
	UINT32						separate_colour_plane_flag;
	UINT32						pic_width_in_luma_samples;
	UINT32						pic_height_in_luma_samples;
	UINT32						conformance_window_flag;
	UINT32						conf_win_left_offset;
	UINT32						conf_win_right_offset;
	UINT32						conf_win_top_offset;
	UINT32						conf_win_bottom_offset;
	UINT32						bit_depth_luma_minus8;
	UINT32						bit_depth_chroma_minus8;
	UINT32						log2_max_pic_order_cnt_lsb_minus4;
	UINT32						sps_sub_layer_ordering_info_present_flag;
#if 0	// skip
	UINT32						sps_max_dec_pic_buffering[ i ];
	UINT32						sps_max_num_reorder_pics[ i ];
	UINT32						sps_max_latency_increase[ i ];
#endif
	UINT32						log2_min_luma_coding_block_size_minus3;
	UINT32						log2_diff_max_min_luma_coding_block_size;
	UINT32						log2_min_luma_transform_block_size_minus2;
	UINT32						log2_diff_max_min_luma_transform_block_size;
	UINT32						max_transform_hierarchy_depth_inter;
	UINT32						max_transform_hierarchy_depth_intra;
	UINT32						scaling_list_enable_flag;
	UINT32						sps_scaling_list_data_present_flag;
#if 0	// skip
	T_BF_MUX_H265_SCALING_LIST	scaling_list_data;
#endif	// skip
	UINT32						amp_enabled_flag;
	UINT32						sample_adaptive_offset_enabled_flag;
	UINT32						pcm_enabled_flag;
	UINT32						pcm_sample_bit_depth_luma_minus1;
	UINT32						pcm_sample_bit_depth_chroma_minus1;
	UINT32						log2_min_pcm_luma_coding_block_size_minus3;
	UINT32						log2_diff_max_min_pcm_luma_coding_block_size;
	UINT32						pcm_loop_filter_disable_flag;
	UINT32						num_short_term_ref_pic_sets;
#if 0	// skip
	T_BF_MUX_H265_ST_RPS		short_term_ref_pic_set;
#endif
	UINT32						long_term_ref_pics_present_flag;
	UINT32						num_long_term_ref_pics_sps;
#if 0	// skip
	UINT32						lt_ref_pic_poc_lsb_sps[ i ];
	UINT32						used_by_curr_pic_lt_sps_flag[ i ];
#endif	// skip
	UINT32						sps_temporal_mvp_enabled_flag;
	UINT32						strong_intra_smoothing_enabled_flag;
	UINT32						vui_parameters_present_flag;
	T_BF_MUX_H265_VUI			vui_parameters;
} T_BF_MUX_H265_SPS;


typedef struct{
	UINT32						pps_pic_parameter_set_id;
	UINT32						pps_seq_parameter_set_id;
	UINT32						dependent_slice_segments_enabled_flag;
	UINT32						output_flag_present_flag;
	UINT32						num_extra_slice_header_bits;
	UINT32						sign_data_hiding_enabled_flag;
	UINT32						cabac_init_present_flag;
	UINT32						num_ref_idx_l0_default_active_minus1;
	UINT32						num_ref_idx_l1_default_active_minus1;
	UINT32						init_qp_minus26;
	UINT32						constrained_intra_pred_flag;
	UINT32						transform_skip_enabled_flag;
	UINT32						cu_qp_delta_enabled_flag;
	UINT32						diff_cu_qp_delta_depth;
	UINT32						pps_cb_qp_offset;
	UINT32						pps_cr_qp_offset;
	UINT32						pps_slice_chroma_qp_offsets_present_flag;
	UINT32						weighted_pred_flag;
	UINT32						weighted_bipred_flag;
	UINT32						transquant_bypass_enabled_flag;
	UINT32						tiles_enabled_flag;
	UINT32						entropy_coding_sync_enabled_flag;
} T_BF_MUX_H265_PPS;


typedef struct{
	UINT32						configurationVersion;
	UINT32						general_profile_space;
	UINT32						general_tier_flag;
	UINT32						general_profile_idc;
	UINT32						general_profile_compatibility_flags;
	UINT64						general_constraint_indicator_flags;
	UINT32						general_level_idc;
	UINT32						min_spatial_segmentation_idc;
	UINT32						parallelismType;
	UINT32						chromaFormat;
	UINT32						bitDepthLumaMinus8;
	UINT32						bitDepthChromaMinus8;
	UINT32						avgFrameRate;
	UINT32						constantFrameRate;
	UINT32						numTemporalLayers;
	UINT32						temporalIdNested;
	UINT32						lengthSizeMinusOne;
} T_BF_MUX_H265_HVCC;


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static	volatile UCHAR				eosCode[D_BF_MUX_EOS_SIZE] = {0x00,0x00,0x00,0x01,0x0A,0x00,0x00,0x00,0x01,0x0B};

static	volatile T_FJ_MUX_CONFIG	gMux_Config;
static	volatile T_BF_MUX_CTRL		gMux_Ctrl;
/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define MUX_MAX( a, b ) ((a) > (b) ? (a) : (b))
#define MUX_MIN( a, b ) ((a) > (b) ? (b) : (a))


/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static	INT32	bf_mux_convert_endian( VOID *input_data, ULONG convert_size );
static	T_BF_MUX_CHANNEL_CTRL*	bf_mux_channel_ctrl_get( INT32 stream_id, INT32* stream_ch );
static	INT32	bf_mux_init_param( VOID );
static	INT32	bf_mux_default_palam_set( T_BF_MUX_FORMAT_CTRL* format_ctrl, T_FJ_MUX_VIDEO_CONFIG* video_param, T_FJ_MUX_AUDIO_CONFIG* audio_param );
static	INT32	bf_mux_fixed_atom_create( INT32 stream_ch, T_BF_MUX_FORMAT_CTRL* format_ctrl );
static	INT32	bf_mux_fixed_atom_create2( INT32 stream_ch, T_BF_MUX_FORMAT_CTRL* format_ctrl, T_FJ_MUX_H264_SPS_PPS_INFO* sps_pps_info, T_FJ_MUX_H265_PS_INFO* ps_info );
static	INT32	bf_mux_avcc_create( T_FJ_MUX_H264_SPS_PPS_INFO* sps_pps_info, UCHAR* avcc_adr, ULONG* avcc_size );
static	INT32	bf_mux_hvcc_create( T_FJ_MUX_H265_PS_INFO* ps_info, UCHAR* hvcc_adr, ULONG* hvcc_size );
static	VOID	bf_mux_hvcc_create_ps_free( UINT8* vps_address, UINT8* sps_address, UINT8* pps_address );
static	BOOL	bf_mux_mp4_ptl_analyze( UINT32 profilePresentFlag, UINT32 maxNumSubLayersMinus1, T_BF_MUX_H265_PTL* ptl_info, T_MP4_COMMON_STREAM_CONTROL* bit_stream );
static	BOOL	bf_mux_mp4_scaling_list_analyze( T_MP4_COMMON_STREAM_CONTROL* bit_stream );
static	BOOL	bf_mux_mp4_st_rps_analyze( UINT32 stRpsIdx, UINT32 num_short_term_ref_pic_sets, UINT32 NumDeltaPocs[D_BF_MUX_H265_SHORT_TERM_RPS_MAX_NUM], T_MP4_COMMON_STREAM_CONTROL* bit_stream );
static	BOOL	bf_mux_mp4_vui_analyze( UINT32 sps_max_sub_layers_minus1, T_BF_MUX_H265_VUI* vui_info, T_MP4_COMMON_STREAM_CONTROL* bit_stream );
static	BOOL	bf_mux_mp4_hrd_analyze( UINT32 commonInfPresentFlag, UINT32 maxNumSubLayersMinus1, T_MP4_COMMON_STREAM_CONTROL* bit_stream );
static	BOOL	bf_mux_mp4_sub_layer_hrd_analyze( UINT32 cpb_cnt_minus1, UINT32 sub_pic_hrd_params_present_flag, T_MP4_COMMON_STREAM_CONTROL* bit_stream );
static	INT32	bf_mux_video_sample_time_check( T_BF_MUX_CHANNEL_CTRL* pCh_Ctrl, UINT32 frame_type );
static	INT32	bf_mux_video_sample_size_check( INT32 stream_ch, T_BF_MUX_CHANNEL_CTRL* pCh_Ctrl, ULONG sample_size );
static	LONG	bf_mux_record_maxtime_calculate( INT32 stream_ch, T_BF_MUX_CHANNEL_CTRL* pCh_Ctrl, ULLONG media_size, ULLONG* recordable_size, ULONG* size_per_sec );
static	LONG	bf_mux_record_maxtime_get_static( INT32 stream_ch, T_BF_MUX_CHANNEL_CTRL* pCh_Ctrl, ULLONG media_size, ULLONG* recordable_size, ULONG* size_per_sec );
static	VOID	bf_mux_file_close_all( VOID );


/**
 *	@brief		Convert Endian.\n
 *	@param		i/o:input data   :input data
 *	@param		i:convert_size   :convert size
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
static INT32 bf_mux_convert_endian( VOID *input_data, ULONG convert_size )
{
	ULONG	loop_count;
	CHAR	work_area[64]={0};
	
	if( (convert_size < 4) || (convert_size > 64) ){
		BF_Debug_Print_Error(("bf_mux_convert_endian: Covert Size Error!! %lu\n", convert_size));
		return MP4_COMMON_NG;
	}
	
	for( loop_count=0; loop_count<convert_size; loop_count++ ){
		work_area[loop_count] = ((CHAR *)input_data)[loop_count];
	}
	
	for( loop_count=1; loop_count<=convert_size; loop_count++ ){
		((CHAR *)input_data)[loop_count - 1] = work_area[convert_size - loop_count];
	}
	
	return MP4_COMMON_OK;
}

/**
 *	@brief		Mux Ctrl Get.\n
 *	@param		i:stream_id
 *	@param		o:stream_ch
 *	@return		T_BF_MUX_CHANNEL_CTRL
 *	@note		None
 *	@attention	None
 */
static T_BF_MUX_CHANNEL_CTRL* bf_mux_channel_ctrl_get( INT32 stream_id, INT32* stream_ch )
{
	INT32 tmp_stream_ch;
	T_BF_MUX_CHANNEL_CTRL* pCh_Ctrl = NULL;
	
	for( tmp_stream_ch=0; tmp_stream_ch<E_FJ_MUX_CH_MAX; tmp_stream_ch++ ){
		pCh_Ctrl = (T_BF_MUX_CHANNEL_CTRL *)&(gMux_Ctrl.ch_ctrl[tmp_stream_ch]);
		if(pCh_Ctrl->video->stream_id == stream_id){
			*stream_ch = tmp_stream_ch;
			break;
		}
	}
	
	if( tmp_stream_ch == E_FJ_MUX_CH_MAX ){
		return NULL;
	}
	else{
		return pCh_Ctrl;
	}
}

/**
 *	@brief		param init.\n
 *	@param		i:mem_config
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
static INT32 bf_mux_init_param( VOID )
{
	INT32	ret;
	INT32	enable_count = 0;
	UCHAR	stream_ch;
	ULLONG	recordable_size;
	ULONG	size_per_sec;
	ULONG	recordable_time;
	INT32	file_id[E_FJ_MUX_CH_MAX];
	INT32   file_id_count = 0;
	ULONG	brand_count_loop;
	T_BF_MUX_CHANNEL_CTRL* pCh_Ctrl;
	INT32	i;
	UCHAR	aud_id = 0;
	
	for( stream_ch=0; stream_ch<E_FJ_MUX_CH_MAX; stream_ch++ ){
		if( gMux_Config.video[stream_ch].enable == TRUE ){
			enable_count++;
		}
		gMux_Ctrl.audio_ctrl[0].end_flg[stream_ch] = FJ_MUX_ON;
		gMux_Ctrl.audio_ctrl[1].end_flg[stream_ch] = FJ_MUX_ON;
	}
	if( enable_count == 0 ){
		return MP4_COMMON_NG;
	}
	
	//init param
	gMux_Ctrl.media_size		= BF_MP4_Common_Fs_Mediasize_Get();
	gMux_Ctrl.chunk_per_sec		= 2;	// [Interim]

#if 1 // for LITTLE ENDIAN
	// Convert Endian(major brand, compatible brand)
	bf_mux_convert_endian( (VOID*)&gMux_Config.string.major_brand, sizeof(UINT32) );
	for( stream_ch=0; stream_ch<E_FJ_MUX_CH_MAX; stream_ch++ ){
		if( gMux_Config.video[stream_ch].enable == TRUE ){
			for( brand_count_loop=0; brand_count_loop<gMux_Config.video[stream_ch].compatible_brand_count; brand_count_loop++ ){
				bf_mux_convert_endian( (VOID*)&gMux_Config.video[stream_ch].compatible_brand[brand_count_loop], sizeof(ULONG) );
			}
		}
	}
#endif
	for( i=0; i<2; i++ ){
		if( gMux_Config.audio[i].enable == TRUE ){
			gMux_Ctrl.audio_ctrl[i].sample_max_count = (gMux_Config.audio[i].samplingrate/1024/gMux_Ctrl.chunk_per_sec) + 1;
			
			if( gMux_Ctrl.media_size < (D_BF_MUX_MP4_ATOM_VIDEO_SIZE + D_BF_MUX_MP4_ATOM_AUDIO_SIZE) ){
				BF_Debug_Print_Error(("--- MEDIA SIZE=%llu\n",gMux_Ctrl.media_size ));
				return MP4_COMMON_NG_MEDIA_AREA_NOTHING;
			}
		}
	}

	if( gMux_Ctrl.media_size < D_BF_MUX_MP4_ATOM_VIDEO_SIZE ){
		BF_Debug_Print_Error(("--- MEDIA SIZE=%llu\n",gMux_Ctrl.media_size ));
		return MP4_COMMON_NG_MEDIA_AREA_NOTHING;
	}

	for( stream_ch=0; stream_ch<E_FJ_MUX_CH_MAX; stream_ch++ ){
		pCh_Ctrl					= &(gMux_Ctrl.ch_ctrl[stream_ch]);
		pCh_Ctrl->video				= (T_FJ_MUX_VIDEO_CONFIG*)&(gMux_Config.video[stream_ch]);
		if( pCh_Ctrl->video->enable == TRUE ){
			pCh_Ctrl->first_flag		= D_BF_MUX_ON;
			pCh_Ctrl->force_stop		= D_BF_MUX_OFF;
			pCh_Ctrl->total_duration	= 0;
			pCh_Ctrl->gop_loop_cnt		= 0;

			if( gMux_Config.audio[0].enable == TRUE && gMux_Config.audio[1].enable == TRUE && stream_ch == E_FJ_MUX_CH_1) {
				// if 2 video streams, and 2 audio streams, we assign audio stream1 to video stream1
				aud_id = 1;
			} else {
				aud_id = 0;
			}
			
			if( gMux_Config.audio[aud_id].enable == TRUE ) {
				pCh_Ctrl->duration_convert_ratio = (DOUBLE)pCh_Ctrl->video->time_scale / gMux_Config.audio[aud_id].samplingrate;
				gMux_Ctrl.audio_ctrl[aud_id].end_flg[stream_ch] = FJ_MUX_OFF;
			}
			
			// Set Total Record Time and initialize recording time
			recordable_time = bf_mux_record_maxtime_get_static( stream_ch, pCh_Ctrl, gMux_Ctrl.media_size, &recordable_size, &size_per_sec );
			if( recordable_time < 1 ){
				// if total time is small than 1sec, it cannot record
				BF_Debug_Print_Error(("--- MEDIA SIZE=%llu\n",gMux_Ctrl.media_size ));
				return MP4_COMMON_NG_MEDIA_AREA_NOTHING;
			}
			
			// --- Set default parameter ---
			ret = bf_mux_default_palam_set( &(pCh_Ctrl->format_ctrl), 
											pCh_Ctrl->video,
											(T_FJ_MUX_AUDIO_CONFIG*)&gMux_Config.audio[aud_id] );
			if( ret != MP4_COMMON_OK ){
				BF_Debug_Print_Error(("--- Default Parameter Error(MOV). ret=%d\n", ret));
				return MP4_COMMON_NG_SYSTEM;
			}
			
			// --- Set fixed ATOM Informations ---
			ret = bf_mux_fixed_atom_create( stream_ch, &(pCh_Ctrl->format_ctrl) );
			if( ret != MP4_COMMON_OK ){
				BF_Debug_Print_Error(("--- bf_mux_fixed_atom_create. ret=%d\n", ret));
				// Free the memory, and close file
				bf_mux_file_close_all();
				return ret;
			}
		}
	}
	
	// Init fs memory
	for( stream_ch=0; stream_ch<E_FJ_MUX_CH_MAX; stream_ch++ ){
		if( gMux_Ctrl.ch_ctrl[stream_ch].video->enable == TRUE ){
			file_id[file_id_count] = gMux_Ctrl.ch_ctrl[stream_ch].video->file_id;
			file_id_count++;
		}
	}
	ret = BF_MP4_Common_Fs_Memory_Initialize( file_id, file_id_count );
	if( ret != MP4_COMMON_OK ){
		BF_Debug_Print_Error(("--- BF_MP4_Common_Fs_Memory_Initialize. ret=%d\n", ret));
		// Free the memory, and close file
		bf_mux_file_close_all();
		return ret;
	}
	
	return MP4_COMMON_OK; 
}

/**
 *	@brief		Mux Defalt Parameter Set.\n
 *	@param		i/o:format_ctrl
 *	@param		i:video_param
 *	@param		i:audio_param
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
static INT32 bf_mux_default_palam_set( T_BF_MUX_FORMAT_CTRL* format_ctrl, 
									   T_FJ_MUX_VIDEO_CONFIG* video_param,
									   T_FJ_MUX_AUDIO_CONFIG* audio_param )
{
	UCHAR	audioSpecificInfo[2];
	UCHAR	samplingFrequencyIndex;
	T_MP4_COMMON_STREAM_CONTROL	bit_stream;
	
	audioSpecificInfo[0] = 0;
	audioSpecificInfo[1] = 0;
	
	if( audio_param->enable == TRUE ){
		format_ctrl->audio_track.codec_type = audio_param->codec_type;					// codec type
		
		// --- Set MOV deafult parameters ---
		format_ctrl->audio_track.media_timescale = audio_param->samplingrate;			// mdhd->timescale
		format_ctrl->audio_track.descr_param[0]	 = audio_param->channel;				/* channel count
																						   1: mono, 2: stereo
																						   stsd->(sampleEntry)->channelCount */
		format_ctrl->audio_track.descr_param[1]	 = 16;									/* sample size
																						   SOWT(TWOS)/ULAW : 16/16bit
																						   stsd->(sampleEntry)->sample_size */
		if( BF_AudioEnc_GetTestMode() == 0 ){
			format_ctrl->audio_track.mp4_dec_config_parm[0] = 0x40;							/* esds->objectTypeIndication
																							   0x40 :Audio ISO/IEC 14496-3
																							   0x67 :Audio ISO/IEC 13818-7 LC
																							   (Attention of cannot reproduction in ISO/IEC 13818-7 LC with QT) */
			format_ctrl->audio_track.mp4_dec_config_parm[1] = 0x05;							/* esds->streamType
																							   0x05 :AudioStream */
			format_ctrl->audio_track.mp4_dec_config_parm[2] = 0x300 * audio_param->channel;	// esds->(decconfig)->bufferSizeDB (S)
			format_ctrl->audio_track.mp4_dec_config_parm[3] = audio_param->bitrate;			// esds->(decconfig)->maxBitrate
			format_ctrl->audio_track.mp4_dec_config_parm[4] = audio_param->bitrate;			// esds->(decconfig)->avgBitrate	
			format_ctrl->audio_track.dec_specific_info_size = 2;							// esds->decording specific info size
			
			BF_MP4_Common_BS_Set_Pointer_Address( &bit_stream, audioSpecificInfo );
			
			BF_MP4_Common_BS_Write_U_Nbit( &bit_stream, 5, 2 );								// audioObjectType:5bit
			switch( audio_param->samplingrate ){
				case D_BF_MUX_AUDIO_SR_8 :
					samplingFrequencyIndex	 = 11;
					break;
				case D_BF_MUX_AUDIO_SR_11025 :
					samplingFrequencyIndex	 = 10;
					break;
				case D_BF_MUX_AUDIO_SR_12 :
					samplingFrequencyIndex	 = 9;
					break;
				case D_BF_MUX_AUDIO_SR_16 :
					samplingFrequencyIndex	 = 8;
					break;
				case D_BF_MUX_AUDIO_SR_225 :
					samplingFrequencyIndex	 = 7;
					break;
				case D_BF_MUX_AUDIO_SR_24 :
					samplingFrequencyIndex	 = 6;
					break;
				case D_BF_MUX_AUDIO_SR_32 :
					samplingFrequencyIndex	 = 5;
					break;
				case D_BF_MUX_AUDIO_SR_441 :
					samplingFrequencyIndex	 = 4;
					break;
				case D_BF_MUX_AUDIO_SR_48 :
					samplingFrequencyIndex	 = 3;
					break;
				default :
					BF_Debug_Print_Error(("+++ fj_mux_mp4_default_set: NG!!\n"));
					return MP4_COMMON_NG_INPUT_PARAM;
			}
			BF_MP4_Common_BS_Write_U_Nbit( &bit_stream, 4, samplingFrequencyIndex );		// samplingFrequencyIndex:4bit
			BF_MP4_Common_BS_Write_U_Nbit( &bit_stream, 4, audio_param->channel );			// channelConfiguration  :4bit
			BF_MP4_Common_BS_Write_U_1bit( &bit_stream, 0 );								// frameLengthFlag       :1bit (value is fixed by the AAC library specification.)
			BF_MP4_Common_BS_Write_U_1bit( &bit_stream, 0 );								// dependsOnCoreCoder    :1bit (value is fixed by the AAC library specification.)
			BF_MP4_Common_BS_Write_U_1bit( &bit_stream, 0 );								// extensionFlag         :1bit (value is fixed by the AAC library specification.)
			
			format_ctrl->audio_track.dec_specific_info[0] = audioSpecificInfo[0];
			format_ctrl->audio_track.dec_specific_info[1] = audioSpecificInfo[1];
		}
		else {
			format_ctrl->audio_track.mp4_dec_config_parm[0] = 0x01;				/* stsd->samplesPerPacket
																				   SOWT(TWOS)/ULAW : 1/1 */
			format_ctrl->audio_track.mp4_dec_config_parm[1] = 0x02;				/* stsd->bytesPerPacket
																				   SOWT(TWOS)/ULAW : 2/1byte */
			if( audio_param->channel == D_BF_MUX_AUDIO_CH_STEREO ){
				format_ctrl->audio_track.mp4_dec_config_parm[2] = 0x04;			/* stsd->bytesPerFrame
																				   SOWT(TWOS)(streo)/SOWT(TWOS)(mono)/ULAW : 4/2/1byte */
			}
			else{
				format_ctrl->audio_track.mp4_dec_config_parm[2] = 0x02;
			}
			
			format_ctrl->audio_track.mp4_dec_config_parm[3] = 0x02;				/* stsd->bytesPerSample
																				   SOWT(TWOS)/ULAW : 2/2byte */
			format_ctrl->audio_track.mp4_dec_config_parm[4] = audio_param->bitrate;
			format_ctrl->audio_track.dec_specific_info_size = 0;				// No used
		}
	}
	
	format_ctrl->video_track.codec_type			= video_param->codec_type;		// codec type
	format_ctrl->video_track.media_timescale	= video_param->time_scale;		// TimeScale   mdhd->timescale
	
	switch( video_param->image_size ){
// --- REMOVE_COMPILE_OPT BEGIN ---
#ifdef CO_SUPPORT_60FPS
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_60FPS BEGIN ---
		case E_FJ_MOVIE_VIDEO_SIZE_4000_3000 :
			format_ctrl->video_track.descr_param[0] = 4000;		// width for track
			format_ctrl->video_track.descr_param[1] = 3000;		// height for track
			break;
// --- REMOVE_60FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#endif // CO_SUPPORT_60FPS
// --- REMOVE_COMPILE_OPT END ---
		case E_FJ_MOVIE_VIDEO_SIZE_4096_2304 :
			format_ctrl->video_track.descr_param[0] = 4096;		// width for track
			format_ctrl->video_track.descr_param[1] = 2304;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_4096_2160 :
			format_ctrl->video_track.descr_param[0] = 4096;		// width for track
			format_ctrl->video_track.descr_param[1] = 2160;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_4096_2048 :
			format_ctrl->video_track.descr_param[0] = 4096;		// width for track
			format_ctrl->video_track.descr_param[1] = 2048;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_3840_2160 :
			format_ctrl->video_track.descr_param[0] = 3840;		// width for track
			format_ctrl->video_track.descr_param[1] = 2160;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_3840_1920 :
			format_ctrl->video_track.descr_param[0] = 3840;		// width for track
			format_ctrl->video_track.descr_param[1] = 1920;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_3000_3000 :
			format_ctrl->video_track.descr_param[0] = 3000;		// width for track
			format_ctrl->video_track.descr_param[1] = 3000;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2704_2028 :
			format_ctrl->video_track.descr_param[0] = 2704;		// width for track
			format_ctrl->video_track.descr_param[1] = 2028;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2704_1520 :
			format_ctrl->video_track.descr_param[0] = 2704;		// width for track
			format_ctrl->video_track.descr_param[1] = 1520;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2560_1920 :
			format_ctrl->video_track.descr_param[0] = 2560;		// width for track
			format_ctrl->video_track.descr_param[1] = 1920;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2560_1440 :
			format_ctrl->video_track.descr_param[0] = 2560;		// width for track
			format_ctrl->video_track.descr_param[1] = 1440;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_1440 :
			format_ctrl->video_track.descr_param[0] = 1920;		// width for track
			format_ctrl->video_track.descr_param[1] = 1440;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_1080 :
			format_ctrl->video_track.descr_param[0] = 1920;		// width for track
			format_ctrl->video_track.descr_param[1] = 1080;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_960 :
			format_ctrl->video_track.descr_param[0] = 1920;		// width for track
			format_ctrl->video_track.descr_param[1] = 960;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1504_1504 :
			format_ctrl->video_track.descr_param[0] = 1504;		// width for track
			format_ctrl->video_track.descr_param[1] = 1504;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1440_1080 :
			format_ctrl->video_track.descr_param[0] = 1440;		// width for track
			format_ctrl->video_track.descr_param[1] = 1080;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1440_720 :
			format_ctrl->video_track.descr_param[0] = 1440;		// width for track
			format_ctrl->video_track.descr_param[1] = 720;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_1024 :
			format_ctrl->video_track.descr_param[0] = 1280;		// width for track
			format_ctrl->video_track.descr_param[1] = 1024;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_960 :
			format_ctrl->video_track.descr_param[0] = 1280;		// width for track
			format_ctrl->video_track.descr_param[1] = 960;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_768 :
			format_ctrl->video_track.descr_param[0] = 1280;		// width for track
			format_ctrl->video_track.descr_param[1] = 768;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_720 :
			format_ctrl->video_track.descr_param[0] = 1280;		// width for track
			format_ctrl->video_track.descr_param[1] = 720;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1024_768 :
			format_ctrl->video_track.descr_param[0] = 1024;		// width for track
			format_ctrl->video_track.descr_param[1] = 768;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_864_480 :
			format_ctrl->video_track.descr_param[0] = 864;		// width for track
			format_ctrl->video_track.descr_param[1] = 480;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_960_540 :
			format_ctrl->video_track.descr_param[0] = 960;		// width for track
			format_ctrl->video_track.descr_param[1] = 540;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_960_480 :
			format_ctrl->video_track.descr_param[0] = 960;		// width for track
			format_ctrl->video_track.descr_param[1] = 480;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_848_480 :
			format_ctrl->video_track.descr_param[0] = 848;		// width for track
			format_ctrl->video_track.descr_param[1] = 480;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_800_600 :
			format_ctrl->video_track.descr_param[0] = 800;		// width for track
			format_ctrl->video_track.descr_param[1] = 600;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_720_576 :
			format_ctrl->video_track.descr_param[0] = 720;		// width for track
			format_ctrl->video_track.descr_param[1] = 576;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_720_480 :
			format_ctrl->video_track.descr_param[0] = 720;		// width for track
			format_ctrl->video_track.descr_param[1] = 480;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_640_480 :
			format_ctrl->video_track.descr_param[0] = 640;		// width for track
			format_ctrl->video_track.descr_param[1] = 480;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_640_360 :
			format_ctrl->video_track.descr_param[0] = 640;		// width for track
			format_ctrl->video_track.descr_param[1] = 360;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_480_270 :
			format_ctrl->video_track.descr_param[0] = 480;		// width for track
			format_ctrl->video_track.descr_param[1] = 270;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_432_240 :
			format_ctrl->video_track.descr_param[0] = 432;		// width for track
			format_ctrl->video_track.descr_param[1] = 240;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_320_240 :
			format_ctrl->video_track.descr_param[0] = 320;		// width for track
			format_ctrl->video_track.descr_param[1] = 240;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1600_896:
			format_ctrl->video_track.descr_param[0] = 1600;		// width for track
			format_ctrl->video_track.descr_param[1] = 896;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1024_576:
			format_ctrl->video_track.descr_param[0] = 1024;		// width for track
			format_ctrl->video_track.descr_param[1] = 576;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_800_448:
			format_ctrl->video_track.descr_param[0] = 800;		// width for track
			format_ctrl->video_track.descr_param[1] = 448;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_352_288:
			format_ctrl->video_track.descr_param[0] = 352;		// width for track
			format_ctrl->video_track.descr_param[1] = 288;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2592_1944:
			format_ctrl->video_track.descr_param[0] = 2592;		// width for track
			format_ctrl->video_track.descr_param[1] = 1944;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2304_1296:
			format_ctrl->video_track.descr_param[0] = 2304;		// width for track
			format_ctrl->video_track.descr_param[1] = 1296;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_1280:
			format_ctrl->video_track.descr_param[0] = 1920;		// width for track
			format_ctrl->video_track.descr_param[1] = 1280;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_640_512:
			format_ctrl->video_track.descr_param[0] = 640;		// width for track
			format_ctrl->video_track.descr_param[1] = 512;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_384_288:
			format_ctrl->video_track.descr_param[0] = 384;		// width for track
			format_ctrl->video_track.descr_param[1] = 288;		// height for track
			break;
		default :
			BF_Debug_Print_Error(("+++ fj_mux_mp4_default_set: NG!!\n"));
			return MP4_COMMON_NG_INPUT_PARAM;
	}
	
	switch( video_param->display_size ){
// --- REMOVE_COMPILE_OPT BEGIN ---
#ifdef CO_SUPPORT_60FPS
// --- REMOVE_COMPILE_OPT END ---
// --- REMOVE_60FPS BEGIN ---
		case E_FJ_MOVIE_VIDEO_SIZE_4000_3000 :
			format_ctrl->video_track.track_parm[0] = 4000;		// width for track
			format_ctrl->video_track.track_parm[1] = 3000;		// height for track
			break;
// --- REMOVE_60FPS END ---
// --- REMOVE_COMPILE_OPT BEGIN ---
#endif // CO_SUPPORT_60FPS
// --- REMOVE_COMPILE_OPT END ---
		case E_FJ_MOVIE_VIDEO_SIZE_4096_2304 :
			format_ctrl->video_track.track_parm[0] = 4096;		// width for track
			format_ctrl->video_track.track_parm[1] = 2304;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_4096_2160 :
			format_ctrl->video_track.track_parm[0] = 4096;		// width for track
			format_ctrl->video_track.track_parm[1] = 2160;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_4096_2048 :
			format_ctrl->video_track.track_parm[0] = 4096;		// width for track
			format_ctrl->video_track.track_parm[1] = 2048;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_3840_2160 :
			format_ctrl->video_track.track_parm[0] = 3840;		// width for track
			format_ctrl->video_track.track_parm[1] = 2160;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_3840_1920 :
			format_ctrl->video_track.track_parm[0] = 3840;		// width for track
			format_ctrl->video_track.track_parm[1] = 1920;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_3000_3000 :
			format_ctrl->video_track.track_parm[0] = 3000;		// width for track
			format_ctrl->video_track.track_parm[1] = 3000;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2704_2028 :
			format_ctrl->video_track.track_parm[0] = 2704;		// width for track
			format_ctrl->video_track.track_parm[1] = 2028;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2704_1520 :
			format_ctrl->video_track.track_parm[0] = 2704;		// width for track
			format_ctrl->video_track.track_parm[1] = 1520;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2560_1920 :
			format_ctrl->video_track.track_parm[0] = 2560;		// width for track
			format_ctrl->video_track.track_parm[1] = 1920;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2560_1440 :
			format_ctrl->video_track.track_parm[0] = 2560;		// width for track
			format_ctrl->video_track.track_parm[1] = 1440;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_1440 :
			format_ctrl->video_track.track_parm[0] = 1920;		// width for track
			format_ctrl->video_track.track_parm[1] = 1440;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_1080 :
			format_ctrl->video_track.track_parm[0] = 1920;		// width for track
			format_ctrl->video_track.track_parm[1] = 1080;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_960 :
			format_ctrl->video_track.track_parm[0] = 1920;		// width for track
			format_ctrl->video_track.track_parm[1] = 960;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1504_1504 :
			format_ctrl->video_track.track_parm[0] = 1504;		// width for track
			format_ctrl->video_track.track_parm[1] = 1504;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1440_1080 :
			format_ctrl->video_track.track_parm[0] = 1440;		// width for track
			format_ctrl->video_track.track_parm[1] = 1080;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1440_720 :
			format_ctrl->video_track.track_parm[0] = 1440;		// width for track
			format_ctrl->video_track.track_parm[1] = 720;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_1024 :
			format_ctrl->video_track.track_parm[0] = 1280;		// width for track
			format_ctrl->video_track.track_parm[1] = 1024;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_960 :
			format_ctrl->video_track.track_parm[0] = 1280;		// width for track
			format_ctrl->video_track.track_parm[1] = 960;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_768 :
			format_ctrl->video_track.track_parm[0] = 1280;		// width for track
			format_ctrl->video_track.track_parm[1] = 768;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1280_720 :
			format_ctrl->video_track.track_parm[0] = 1280;		// width for track
			format_ctrl->video_track.track_parm[1] = 720;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1024_768 :
			format_ctrl->video_track.track_parm[0] = 1024;		// width for track
			format_ctrl->video_track.track_parm[1] = 768;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_864_480 :
			format_ctrl->video_track.track_parm[0] = 864;		// width for track
			format_ctrl->video_track.track_parm[1] = 480;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_960_540 :
			format_ctrl->video_track.track_parm[0] = 960;		// width for track
			format_ctrl->video_track.track_parm[1] = 540;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_960_480 :
			format_ctrl->video_track.track_parm[0] = 960;		// width for track
			format_ctrl->video_track.track_parm[1] = 480;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_848_480 :
			format_ctrl->video_track.track_parm[0] = 848;		// width for track
			format_ctrl->video_track.track_parm[1] = 480;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_800_600 :
			format_ctrl->video_track.track_parm[0] = 800;		// width for track
			format_ctrl->video_track.track_parm[1] = 600;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_720_576 :
			format_ctrl->video_track.track_parm[0] = 720;		// width for track
			format_ctrl->video_track.track_parm[1] = 576;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_720_480 :
			format_ctrl->video_track.track_parm[0] = 720;		// width for track
			format_ctrl->video_track.track_parm[1] = 480;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_640_480 :
			format_ctrl->video_track.track_parm[0] = 640;		// width for track
			format_ctrl->video_track.track_parm[1] = 480;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_640_360 :
			format_ctrl->video_track.track_parm[0] = 640;		// width for track
			format_ctrl->video_track.track_parm[1] = 360;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_480_270 :
			format_ctrl->video_track.track_parm[0] = 480;		// width for track
			format_ctrl->video_track.track_parm[1] = 270;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_432_240 :
			format_ctrl->video_track.track_parm[0] = 432;		// width for track
			format_ctrl->video_track.track_parm[1] = 240;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_320_240 :
			format_ctrl->video_track.track_parm[0] = 320;		// width for track
			format_ctrl->video_track.track_parm[1] = 240;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1600_896:
			format_ctrl->video_track.track_parm[0] = 1600;		// width for track
			format_ctrl->video_track.track_parm[1] = 896;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1024_576:
			format_ctrl->video_track.track_parm[0] = 1024;		// width for track
			format_ctrl->video_track.track_parm[1] = 576;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_800_448:
			format_ctrl->video_track.track_parm[0] = 800;		// width for track
			format_ctrl->video_track.track_parm[1] = 448;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_352_288:
			format_ctrl->video_track.track_parm[0] = 352;		// width for track
			format_ctrl->video_track.track_parm[1] = 288;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2592_1944:
			format_ctrl->video_track.track_parm[0] = 2592;		// width for track
			format_ctrl->video_track.track_parm[1] = 1944;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_2304_1296:
			format_ctrl->video_track.track_parm[0] = 2304;		// width for track
			format_ctrl->video_track.track_parm[1] = 1296;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_1920_1280:
			format_ctrl->video_track.track_parm[0] = 1920; 	// width for track
			format_ctrl->video_track.track_parm[1] = 1280; 	// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_640_512:
			format_ctrl->video_track.track_parm[0] = 640;		// width for track
			format_ctrl->video_track.track_parm[1] = 512;		// height for track
			break;
		case E_FJ_MOVIE_VIDEO_SIZE_384_288:
			format_ctrl->video_track.track_parm[0] = 384;		// width for track
			format_ctrl->video_track.track_parm[1] = 288;		// height for track
			break;
		default :
			BF_Debug_Print_Error(("+++ fj_mux_mp4_default_set: NG!!\n"));
			return MP4_COMMON_NG_INPUT_PARAM;
	}
	
	return MP4_COMMON_OK;
}

/**
 *	@brief		Fixed atom Create 1st Timing.\n
 *	@param		i:stream_ch
 *	@param		i:format_ctrl
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
static INT32 bf_mux_fixed_atom_create( INT32 stream_ch, T_BF_MUX_FORMAT_CTRL* format_ctrl )
{
	MP4FErr		mp4err;		// MP4F File Format Error
	ULONG		track_flags;
	ULONG		track_matrix[9];
#if ( CO_MOVIE_FF_AUDIO_PATTERN == 0 )
	UCHAR		lang[3] = {'u', 'n', 'd'}; // mdhd-language ('und')
#endif	// CO_MOVIE_FF_AUDIO_PATTERN
	UCHAR		aud_id = 0;
	
// --- 1. Initialize parameter ---
	format_ctrl->mp4f_handle = NULL;
	format_ctrl->track_id[0] = 0;
	format_ctrl->track_id[1] = 0;
	format_ctrl->dref_entry_id[0] = 0;
	format_ctrl->dref_entry_id[1] = 0;
	format_ctrl->stsd_entry_id[1] = 0;
#if ( CO_MOVIE_FF_AUDIO_PATTERN != 0 )
	track_flags = MP4F_TRACK_ENABLE|MP4F_TRACK_IN_MOVIE|MP4F_TRACK_IN_PREVIEW|MP4F_TRACK_IN_POSTER;	// MOV
#else	// CO_MOVIE_FF_AUDIO_PATTERN
	track_flags = MP4F_TRACK_ENABLE;						// MP4
#endif	// CO_MOVIE_FF_AUDIO_PATTERN
	track_matrix[0] = 0x00010000;
	track_matrix[1] = 0x00000000;
	track_matrix[2] = 0x00000000;
	track_matrix[3] = 0x00000000;
	track_matrix[4] = 0x00010000;
	track_matrix[5] = 0x00000000;
	track_matrix[6] = 0x00000000;
	track_matrix[7] = 0x00000000;
	track_matrix[8] = 0x40000000;
	
// --- 2. Open MP4 Handler ---
	// open mp4 handler
	mp4err = MP4F_OpenFile(
				(MP4FHANDLE*)&format_ctrl->mp4f_handle,
				MP4F_OPEN_MODE_WRITE,
				gMux_Config.video[stream_ch].file_id,
				MP4F_BEFORE_MOOV,
				MP4F_AT_CHUNK_UPDATE,
				gMux_Config.video[stream_ch].time_scale
				);
	if( mp4err != MP4F_OK ){
		BF_Debug_Print_Error(("Open MP4Handler Error. mp4err=%lx\n", mp4err));
		MP4F_Close( format_ctrl->mp4f_handle );
		return MP4_COMMON_NG_SYSTEM;
	}
	
// --- 3. Create video track ---
#if ( CO_MOVIE_FF_AUDIO_PATTERN != 0 )
	mp4err = MP4F_AddTrack(
				format_ctrl->mp4f_handle,
				(uInt32*)&format_ctrl->track_id[0],
				MP4F_HANDLER_TYPE_VIDEO,
				track_flags,
				format_ctrl->video_track.media_timescale,
				track_matrix,
				(USHORT)MP4F_MOV_LANG_ENGLISH,
				(CHAR*)gMux_Config.string.video_handler_name
				);
#else	// CO_MOVIE_FF_AUDIO_PATTERN
	mp4err = MP4F_AddTrack(
				format_ctrl->mp4f_handle,
				(uInt32*)&format_ctrl->track_id[0],
				MP4F_HANDLER_TYPE_VIDEO,
				track_flags,
				format_ctrl->video_track.media_timescale,
				track_matrix,
				lang,
				(CHAR*)gMux_Config.string.video_handler_name
				);
#endif	// CO_MOVIE_FF_AUDIO_PATTERN
	if( mp4err != MP4F_OK ){
		BF_Debug_Print_Error(("Video AddTrack Error. mp4err=%lx\n", mp4err));
		MP4F_Close( format_ctrl->mp4f_handle );
		return MP4_COMMON_NG_SYSTEM;
	}
	// Set width,height to video track
	mp4err = MP4F_SetWidthHeightToVideoTrack(
				format_ctrl->mp4f_handle,
				format_ctrl->track_id[0],
				(ULONG)format_ctrl->video_track.track_parm[0]<<16,	// width
				(ULONG)format_ctrl->video_track.track_parm[1]<<16	// height
				);
	if( mp4err != MP4F_OK ){
		BF_Debug_Print_Error(("Video Width Error. mp4err=%lx\n", mp4err));
		MP4F_Close( format_ctrl->mp4f_handle );
		return MP4_COMMON_NG_SYSTEM;
	}
	// Add data reference
	mp4err = MP4F_AddDataReference(
				format_ctrl->mp4f_handle,
				format_ctrl->track_id[0],
				MP4F_DATA_TYPE_SELF_CONTAINED,
				NULL,
				NULL,
				(uInt32*)&format_ctrl->dref_entry_id[0]
				);
	if( mp4err != MP4F_OK ){
		BF_Debug_Print_Error(("Video DataReference Error. mp4err=%lx\n", mp4err));
		MP4F_Close( format_ctrl->mp4f_handle );
		return MP4_COMMON_NG_SYSTEM;
	}
	
	if( gMux_Config.audio[0].enable == TRUE && gMux_Config.audio[1].enable == TRUE && stream_ch == E_FJ_MUX_CH_1) {
		// if 2 video streams, and 2 audio streams, we assign audio stream1 to video stream1
		aud_id = 1;
	} else {
		aud_id = 0;
	}
	if( gMux_Config.audio[aud_id].enable == TRUE ){
// --- 4. Create audio track ---
#if ( CO_MOVIE_FF_AUDIO_PATTERN != 0 )
		mp4err = MP4F_AddTrack(
					format_ctrl->mp4f_handle,
					(uInt32*)&format_ctrl->track_id[1],
					MP4F_HANDLER_TYPE_SOUND,
					track_flags,
					format_ctrl->audio_track.media_timescale,
					track_matrix,
					(USHORT)MP4F_MOV_LANG_ENGLISH,
					(CHAR*)gMux_Config.string.audio_handler_name
					);
#else	// CO_MOVIE_FF_AUDIO_PATTERN
		mp4err = MP4F_AddTrack(
					format_ctrl->mp4f_handle,
					(uInt32*)&format_ctrl->track_id[1],
					MP4F_HANDLER_TYPE_SOUND,
					track_flags,
					format_ctrl->audio_track.media_timescale,
					track_matrix,
					lang,
					(CHAR*)gMux_Config.string.audio_handler_name
					);
#endif	// CO_MOVIE_FF_AUDIO_PATTERN
		if( mp4err != MP4F_OK ){
			BF_Debug_Print_Error(("Audio AddTrack Error. mp4err=%lx\n", mp4err));
			MP4F_Close( format_ctrl->mp4f_handle );
			return MP4_COMMON_NG_SYSTEM;
		}
		// Add data reference
		mp4err = MP4F_AddDataReference(
					format_ctrl->mp4f_handle,
					format_ctrl->track_id[1],
					MP4F_DATA_TYPE_SELF_CONTAINED,
					NULL,
					NULL,
					(uInt32*)&format_ctrl->dref_entry_id[1]
		        	);
		if( mp4err != MP4F_OK ){
			BF_Debug_Print_Error(("Audio DataReference Error. mp4err=%lx\n", mp4err));
			MP4F_Close( format_ctrl->mp4f_handle );
			return MP4_COMMON_NG_SYSTEM;
		}
		// Add sample description
		if( BF_AudioEnc_GetTestMode() == 0 ){
			mp4err = MP4F_AddSampleDescription(
						format_ctrl->mp4f_handle,
						format_ctrl->track_id[1],
						format_ctrl->dref_entry_id[1],
						MP4F_SAMPLE_DESCR_TYPE_MP4A,
						(uInt32*)format_ctrl->audio_track.mp4_dec_config_parm,
						format_ctrl->audio_track.dec_specific_info_size,
						(uInt8*)format_ctrl->audio_track.dec_specific_info,
						(uInt32*)&format_ctrl->stsd_entry_id[1]
						);
		}
		else{
			mp4err = MP4F_AddSampleDescription(
						format_ctrl->mp4f_handle,
						format_ctrl->track_id[1],
						format_ctrl->dref_entry_id[1],
						MP4F_SAMPLE_DESCR_TYPE_SOWT,
	//					MP4F_SAMPLE_DESCR_TYPE_TWOS,
						NULL,
						format_ctrl->audio_track.dec_specific_info_size,
						NULL,
						(uInt32*)&format_ctrl->stsd_entry_id[1]
						);
		}
		if( mp4err != MP4F_OK ){
			BF_Debug_Print_Error(("Audio SampleDescription Error. mp4err=%lx\n", mp4err));
			MP4F_Close( format_ctrl->mp4f_handle );
			return MP4_COMMON_NG_SYSTEM;
		}
		
#if ( CO_MOVIE_FF_AUDIO_PATTERN == 0 )
		mp4err = MP4F_SetParmToMp4AudioSampleDescr(
					format_ctrl->mp4f_handle,
					format_ctrl->track_id[1],
					format_ctrl->stsd_entry_id[1],
					format_ctrl->audio_track.descr_param[0],	// channel count (1: Monaural, 2: stereo)
					format_ctrl->audio_track.descr_param[1]		// sample size
					);
#else	// CO_MOVIE_FF_AUDIO_PATTERN
		if( BF_AudioEnc_GetTestMode() == 0 ){
			// In the case of MOV / AAC, for the setting of QTAudioSampleDescr, re-set the dec_config_param.
			format_ctrl->audio_track.mp4_dec_config_parm[0] = 0x400;	/* stsd->samplesPerPacket
																			   AAC 1sample = PCM 1024sample. set 1024*/
			format_ctrl->audio_track.mp4_dec_config_parm[1] = 0x0;		/* stsd->bytesPerPacket
																			   According to the specification is set to 0. */
			format_ctrl->audio_track.mp4_dec_config_parm[2] = 0x0;		/* stsd->bytesPerFrame
																			   According to the specification is set to 0. */
			format_ctrl->audio_track.mp4_dec_config_parm[3] = 0x02;		/* stsd->bytesPerSample
																			   Other than the 8bit audio is set to 2. */
		}
		mp4err = MP4F_SetParmToQTAudioSampleDescr(
					format_ctrl->mp4f_handle,
					format_ctrl->track_id[1],
					format_ctrl->stsd_entry_id[1],
					format_ctrl->audio_track.descr_param[0],	// channel count (1: Monaural, 2: stereo)
					format_ctrl->audio_track.descr_param[1],	// sample size
					format_ctrl->audio_track.mp4_dec_config_parm[0],
					format_ctrl->audio_track.mp4_dec_config_parm[1],
					format_ctrl->audio_track.mp4_dec_config_parm[2],
					format_ctrl->audio_track.mp4_dec_config_parm[3]
					);
		if( mp4err != MP4F_OK ){
			BF_Debug_Print_Error(("Audio Parameter Error. mp4err=%lx\n", mp4err));
			MP4F_Close( format_ctrl->mp4f_handle );
			return MP4_COMMON_NG_SYSTEM;
		}
#endif	// CO_MOVIE_FF_AUDIO_PATTERN
	}
	
	return MP4_COMMON_OK;
}

/**
 *	@brief		Fixed atom Create 2nd Timing.\n
 *	@param		i:format_ctrl
 *	@param		i:sps_pps_info
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
static INT32 bf_mux_fixed_atom_create2( INT32 stream_ch, T_BF_MUX_FORMAT_CTRL* format_ctrl, T_FJ_MUX_H264_SPS_PPS_INFO* sps_pps_info, T_FJ_MUX_H265_PS_INFO* ps_info )
{
	MP4FErr			mp4err;			// MP4F File Format Error
	UINT32			minor_version;
	UINT32			descrType;
	ULONG			avcc_size = 0;
	ULONG			hvcc_size = 0;
	UCHAR*			compressorname;
	UCHAR			avc_compressorname[32]  = {0x0A,'A','V','C',' ','C','o','d','i','n', 'g',0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0};
	UCHAR			hevc_compressorname[32] = {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0};
	format_ctrl->stsd_entry_id[0] = 0;
#if ( CO_MOVIE_FF_AUDIO_PATTERN != 0 )
	USHORT			color_info[4] = {0,0,0,0};
#endif	// CO_MOVIE_FF_AUDIO_PATTERN
	
	if( format_ctrl->video_track.codec_type == FJ_MOVIE_CODEC_H264 ){
		if( sps_pps_info == NULL ){
			BF_Debug_Print_Error(("- MUX:sps_pps_info Error.\n"));
			MP4F_Close( format_ctrl->mp4f_handle );
			return MP4_COMMON_NG_INPUT_PARAM;
		}
		
		bf_mux_avcc_create( sps_pps_info, (UCHAR*)format_ctrl->video_track.dec_specific_info, &avcc_size);
		format_ctrl->video_track.dec_specific_info_size = avcc_size;
		descrType = MP4F_SAMPLE_DESCR_TYPE_AVC1;
		
		compressorname = avc_compressorname;
	}
	else if( format_ctrl->video_track.codec_type == FJ_MOVIE_CODEC_H265 ){
		if( ps_info == NULL ){
			BF_Debug_Print_Error(("- MUX:ps_info Error.\n"));
			MP4F_Close( format_ctrl->mp4f_handle );
			return MP4_COMMON_NG_INPUT_PARAM;
		}
		
		bf_mux_hvcc_create( ps_info, (UCHAR*)format_ctrl->video_track.dec_specific_info, &hvcc_size);
		format_ctrl->video_track.dec_specific_info_size = hvcc_size;
		descrType = MP4F_SAMPLE_DESCR_TYPE_HVC1;
		
		compressorname = hevc_compressorname;
	}
	else{
		BF_Debug_Print_Error(("bf_mux_fixed_atom_create2 : codec_type Error. codec_type=%x\n", format_ctrl->video_track.codec_type));
		MP4F_Close( format_ctrl->mp4f_handle );
		return MP4_COMMON_NG_SYSTEM;
	}
	
	// Add sample description
	mp4err = MP4F_AddSampleDescription(
				format_ctrl->mp4f_handle,
				format_ctrl->track_id[0],
				format_ctrl->dref_entry_id[0],
				descrType,
				(uInt32*)format_ctrl->video_track.mp4_dec_config_parm,
				format_ctrl->video_track.dec_specific_info_size,
				(uInt8*)format_ctrl->video_track.dec_specific_info,
				(uInt32*)&format_ctrl->stsd_entry_id[0]
				);
	if( mp4err != MP4F_OK ){
		BF_Debug_Print_Error(("Video SampleDescription Error. mp4err=%lx\n", mp4err));
		MP4F_Close( format_ctrl->mp4f_handle );
		return MP4_COMMON_NG_SYSTEM;
	}
	
#if ( CO_MOVIE_FF_AUDIO_PATTERN != 0 )
	// Set vidual width, vidual height to sample description for Video.
	mp4err = MP4F_SetParmToQTVideoSampleDescr(
				format_ctrl->mp4f_handle,
				format_ctrl->track_id[0],
				format_ctrl->stsd_entry_id[0],
				format_ctrl->video_track.descr_param[0],			// width
				format_ctrl->video_track.descr_param[1],			// height
				compressorname,
				color_info
				);
#else	// CO_MOVIE_FF_AUDIO_PATTERN
	// Set vidual width, vidual height to sample description for Video.
	mp4err = MP4F_SetParmToMp4VideoSampleDescr(
				format_ctrl->mp4f_handle,
				format_ctrl->track_id[0],
				format_ctrl->stsd_entry_id[0],
				format_ctrl->video_track.descr_param[0],			// width
				format_ctrl->video_track.descr_param[1],			// height
				compressorname
				);

#endif	// CO_MOVIE_FF_AUDIO_PATTERN
	if( mp4err != MP4F_OK ){
		BF_Debug_Print_Error(("Video Parameter Error. mp4err=%lx\n", mp4err));
		MP4F_Close( format_ctrl->mp4f_handle );
		return MP4_COMMON_NG_SYSTEM;
	}
	
	minor_version = (gMux_Config.string.major_version << 16) | (gMux_Config.string.minor_version);
	
	mp4err = MP4F_SetFileType(
				format_ctrl->mp4f_handle,
				gMux_Config.string.major_brand,
				minor_version,
				gMux_Config.video[stream_ch].compatible_brand_count,
				(uInt32*)gMux_Config.video[stream_ch].compatible_brand
				);
	if( mp4err != MP4F_OK ){
		BF_Debug_Print_Error(("FTYP Setting Error. mp4err=%lx\n", mp4err));
		MP4F_Close( format_ctrl->mp4f_handle );
		return MP4_COMMON_NG_SYSTEM;
	}
	
	return MP4_COMMON_OK;
}

/**
 *	@brief		AVCC atom Create.\n
 *	@param		i:sps_pps_info
 *	@param		o:avcc_adr
 *	@param		o:avcc_size
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
static INT32 bf_mux_avcc_create( T_FJ_MUX_H264_SPS_PPS_INFO* sps_pps_info, UCHAR* avcc_adr, ULONG* avcc_size )
{
	T_MP4_COMMON_STREAM_CONTROL	sps_bit_stream;
	T_MP4_COMMON_STREAM_CONTROL	avcc_bit_stream;
	
	BF_MP4_Common_BS_Set_Pointer_Address( &sps_bit_stream, sps_pps_info->sps_address );
	BF_MP4_Common_BS_Set_Pointer_Address( &avcc_bit_stream, avcc_adr );
	
	// skip nal_unit_type
	BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( &sps_bit_stream, 8 );
	
	BF_MP4_Common_BS_Write_U_Nbit( &avcc_bit_stream, 8, 1 );													// configurationVersion			:8bit
	BF_MP4_Common_BS_Write_U_Nbit( &avcc_bit_stream, 8, BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 8 ) );	// AVCProfileIndication			:8bit
	BF_MP4_Common_BS_Write_U_Nbit( &avcc_bit_stream, 8, BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 8 ) );	// profile_compatibility		:8bit
	BF_MP4_Common_BS_Write_U_Nbit( &avcc_bit_stream, 8, BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 8 ) );	// AVCLevelIndication			:8bit
	BF_MP4_Common_BS_Write_U_Nbit( &avcc_bit_stream, 6, 63 );													// reserved(0b111111)			:6bit
	BF_MP4_Common_BS_Write_U_Nbit( &avcc_bit_stream, 2, 3 );													// lengthSizeMinusOne			:2bit
	BF_MP4_Common_BS_Write_U_Nbit( &avcc_bit_stream, 3, 7 );													// reserved(0b111)				:3bit
	
	BF_MP4_Common_BS_Write_U_Nbit( &avcc_bit_stream, 5, 1 );													// numOfSequenceParameterSets	:5bit
	BF_MP4_Common_BS_Write_U_Nbit( &avcc_bit_stream, 16, sps_pps_info->sps_size );								// sequenceParameterSetLength	:16bit
	// set SPS data
	memcpy(BF_MP4_Common_BS_Get_Pointer( &avcc_bit_stream ), sps_pps_info->sps_address, sps_pps_info->sps_size);
	BF_MP4_Common_BS_Skip_Pointer_Address( &avcc_bit_stream, sps_pps_info->sps_size );
	
	BF_MP4_Common_BS_Write_U_Nbit( &avcc_bit_stream, 8, 1 );													// numOfPictureParameterSets	:8bit
	BF_MP4_Common_BS_Write_U_Nbit( &avcc_bit_stream, 16, sps_pps_info->pps_size );								// pictureParameterSetLength	:16bit
	// set PPS data
	memcpy(BF_MP4_Common_BS_Get_Pointer( &avcc_bit_stream ), sps_pps_info->pps_address, sps_pps_info->pps_size);
	BF_MP4_Common_BS_Skip_Pointer_Address( &avcc_bit_stream, sps_pps_info->pps_size );
	
	// set avcc size
	*avcc_size = (ULONG)BF_MP4_Common_BS_Get_Pointer( &avcc_bit_stream ) - (ULONG)avcc_adr;
	
	return MP4_COMMON_OK;
}


/**
 *	@brief		HVCC atom Create.\n
 *	@param		i:ps_info
 *	@param		o:hvcc_adr
 *	@param		o:hvcc_size
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
static INT32 bf_mux_hvcc_create( T_FJ_MUX_H265_PS_INFO* ps_info, UCHAR* hvcc_adr, ULONG* hvcc_size )
{
	BOOL							ret;
	UINT32							len;
	UINT32							NumDeltaPocs[D_BF_MUX_H265_SHORT_TERM_RPS_MAX_NUM];
	UINT32							i;
	UINT32							array_loop;
	UINT32							nal_loop;
	UINT32							nal_index;
	UINT8*							vps_address = NULL;
	UINT32							vps_size;
	UINT8*							sps_address = NULL;
	UINT32							sps_size;
	UINT8*							pps_address = NULL;
	UINT32							pps_size;
	T_BF_MUX_H265_VPS				vps_info;
	T_BF_MUX_H265_SPS				sps_info;
	T_BF_MUX_H265_PPS				pps_info;
	T_BF_MUX_H265_HVCC				hvcc_info;
	T_MP4_COMMON_STREAM_CONTROL		vps_bit_stream;
	T_MP4_COMMON_STREAM_CONTROL		sps_bit_stream;
	T_MP4_COMMON_STREAM_CONTROL		pps_bit_stream;
	T_MP4_COMMON_STREAM_CONTROL		hvcc_bit_stream;
	
	memset( &vps_info, 0, sizeof(vps_info) );
	memset( &sps_info, 0, sizeof(sps_info) );
	memset( &pps_info, 0, sizeof(pps_info) );
	
	for( array_loop = 0; array_loop < ps_info->array_num ; array_loop++ ){
		if( ps_info->array_elements[array_loop].nal_type == D_BF_MUX_H265_NALTYPE_VPS ){
			// VPS setting.
			if( vps_address == NULL ){
				nal_index	=  ps_info->array_elements[array_loop].nal_num - 1;		// Specify the NAL, which is the last registered.
				
				vps_address	= (UINT8*)malloc( ps_info->array_elements[array_loop].nal_size[nal_index] );
				if( vps_address == NULL ){
					BF_Debug_Print_Error(("bf_mux_hvcc_create : vps area malloc NG!!\n"));
					break;
				}
				
				BF_MP4_Common_Start_Code_Emuration_Convert( (UINT8*)(ps_info->array_elements[array_loop].nal_address[nal_index] + D_BF_MUX_H265_STARTCODE_SIZE + D_BF_MUX_H265_NAL_HEADER_SIZE),	// src address
															(ps_info->array_elements[array_loop].nal_size[nal_index] - D_BF_MUX_H265_STARTCODE_SIZE - D_BF_MUX_H265_NAL_HEADER_SIZE),				// src size
															vps_address,																								// dst address
															&vps_size );																								// dst size
				if( vps_size == 0 ){
					// ps area free.
					bf_mux_hvcc_create_ps_free( vps_address, sps_address, pps_address );
					return FALSE;	/* pgr0524 */
				}
				BF_MP4_Common_BS_Set_Pointer_Address( &vps_bit_stream, vps_address );
			}
		}
		else if( ps_info->array_elements[array_loop].nal_type == D_BF_MUX_H265_NALTYPE_SPS ){
			// SPS setting.
			if( sps_address == NULL ){
				nal_index	=  ps_info->array_elements[array_loop].nal_num - 1;		// Specify the NAL, which is the last registered.
				
				sps_address = (UINT8*)malloc( ps_info->array_elements[array_loop].nal_size[nal_index] );
				if( sps_address == NULL ){
					BF_Debug_Print_Error(("bf_mux_hvcc_create : sps area malloc NG!!\n"));
					break;
				}
				
				BF_MP4_Common_Start_Code_Emuration_Convert( (UINT8*)(ps_info->array_elements[array_loop].nal_address[nal_index] + D_BF_MUX_H265_STARTCODE_SIZE + D_BF_MUX_H265_NAL_HEADER_SIZE),	// src address
															(ps_info->array_elements[array_loop].nal_size[nal_index] - D_BF_MUX_H265_STARTCODE_SIZE - D_BF_MUX_H265_NAL_HEADER_SIZE),				// src size
															sps_address,																								// dst address
															&sps_size );																								// dst size
				if( sps_size == 0 ){
					// ps area free.
					bf_mux_hvcc_create_ps_free( vps_address, sps_address, pps_address );
					return FALSE;	/* pgr0524 */
				}
				BF_MP4_Common_BS_Set_Pointer_Address( &sps_bit_stream, sps_address );
			}
		}
		else if( ps_info->array_elements[array_loop].nal_type == D_BF_MUX_H265_NALTYPE_PPS ){
			// PPS setting.
			if( pps_address == NULL ){
				nal_index	=  ps_info->array_elements[array_loop].nal_num - 1;		// Specify the NAL, which is the last registered.
				
				pps_address = (UINT8*)malloc( ps_info->array_elements[array_loop].nal_size[nal_index] );
				if( pps_address == NULL ){
					BF_Debug_Print_Error(("bf_mux_hvcc_create : pps area malloc NG!!\n"));
					break;
				}
				
				BF_MP4_Common_Start_Code_Emuration_Convert( (UINT8*)(ps_info->array_elements[array_loop].nal_address[nal_index] + D_BF_MUX_H265_STARTCODE_SIZE + D_BF_MUX_H265_NAL_HEADER_SIZE),	// src address
															(ps_info->array_elements[array_loop].nal_size[nal_index] - D_BF_MUX_H265_STARTCODE_SIZE - D_BF_MUX_H265_NAL_HEADER_SIZE),				// src size
															pps_address,																								// dst address
															&pps_size );																								// dst size
				if( pps_size == 0 ){
					// ps area free.
					bf_mux_hvcc_create_ps_free( vps_address, sps_address, pps_address );
					return FALSE;	/* pgr0524 */
				}
				BF_MP4_Common_BS_Set_Pointer_Address( &pps_bit_stream, pps_address );
			}
		}
		else{
			;	// Nothing Special.
		}
	}
	
	if( (vps_address == NULL) || (sps_address == NULL) || (pps_address == NULL) ){
		// ps area free.
		bf_mux_hvcc_create_ps_free( vps_address, sps_address, pps_address );
		return FALSE;	/* pgr0524 */
	}
	
	// VPS : Analysis.
	vps_info.vps_video_parameter_set_id		= BF_MP4_Common_BS_Read_U_Nbit( &vps_bit_stream, 4 );
	vps_info.vps_base_layer_internal_flag	= BF_MP4_Common_BS_Read_U_Nbit( &vps_bit_stream, 1 );
	vps_info.vps_base_layer_available_flag	= BF_MP4_Common_BS_Read_U_Nbit( &vps_bit_stream, 1 );
	vps_info.vps_max_layers_minus1			= BF_MP4_Common_BS_Read_U_Nbit( &vps_bit_stream, 6 );
	vps_info.vps_max_sub_layers_minus1		= BF_MP4_Common_BS_Read_U_Nbit( &vps_bit_stream, 3 );
	vps_info.vps_temporal_id_nesting_flag	= BF_MP4_Common_BS_Read_U_Nbit( &vps_bit_stream, 1 );
	BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( &vps_bit_stream, 16 );	// vps_reserved_0xffff_16bits
	
	bf_mux_mp4_ptl_analyze( 1, vps_info.vps_max_sub_layers_minus1, &vps_info.profile_tier_level, &vps_bit_stream );
	// VPS : The rest is unnecessary to HVCC.
	
	
	// SPS : Analysis.
	sps_info.sps_video_parameter_set_id		= BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 4 );
	sps_info.sps_max_sub_layers_minus1		= BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 3 );
	sps_info.sps_temporal_id_nesting_flag	= BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 1 );
	
	bf_mux_mp4_ptl_analyze( 1, sps_info.sps_max_sub_layers_minus1, &sps_info.profile_tier_level, &sps_bit_stream );
	
	sps_info.sps_seq_parameter_set_id	= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
	sps_info.chroma_format_idc			= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
	if( sps_info.chroma_format_idc == 3 ){
		sps_info.separate_colour_plane_flag	= BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 1 );
	}
	
	sps_info.pic_width_in_luma_samples	= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
	sps_info.pic_height_in_luma_samples	= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
	sps_info.conformance_window_flag	= BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 1 );
	if( sps_info.conformance_window_flag == 1 ){
		sps_info.conf_win_left_offset		= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
		sps_info.conf_win_right_offset		= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
		sps_info.conf_win_top_offset		= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
		sps_info.conf_win_bottom_offset		= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
	}
	sps_info.bit_depth_luma_minus8						= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
	sps_info.bit_depth_chroma_minus8					= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
	sps_info.log2_max_pic_order_cnt_lsb_minus4			= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
	sps_info.sps_sub_layer_ordering_info_present_flag	= BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 1 );
	
	for( i = ( sps_info.sps_sub_layer_ordering_info_present_flag ? 0 : sps_info.sps_max_sub_layers_minus1 ); i <= sps_info.sps_max_sub_layers_minus1; i++ ) {
		
		BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );	// sps_max_dec_pic_buffering_minus1[ i ]
		BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );	// sps_max_num_reorder_pics[ i ]
		BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );	// sps_max_latency_increase_plus1[ i ]
	}
	
	sps_info.log2_min_luma_coding_block_size_minus3			= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
	sps_info.log2_diff_max_min_luma_coding_block_size		= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
	sps_info.log2_min_luma_transform_block_size_minus2		= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
	sps_info.log2_diff_max_min_luma_transform_block_size	= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
	sps_info.max_transform_hierarchy_depth_inter			= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
	sps_info.max_transform_hierarchy_depth_intra			= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
	sps_info.scaling_list_enable_flag						= BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 1 );
	if( sps_info.scaling_list_enable_flag == 1 ){
		sps_info.sps_scaling_list_data_present_flag	= BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 1 );
		if( sps_info.sps_scaling_list_data_present_flag == 1 ){
			
			bf_mux_mp4_scaling_list_analyze( &sps_bit_stream );
		}
	}
	
	sps_info.amp_enabled_flag						= BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 1 );
	sps_info.sample_adaptive_offset_enabled_flag	= BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 1 );
	sps_info.pcm_enabled_flag						= BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 1 );
	if( sps_info.pcm_enabled_flag == 1 ){
		sps_info.pcm_sample_bit_depth_luma_minus1				= BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 4 );
		sps_info.pcm_sample_bit_depth_chroma_minus1				= BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 4 );
		sps_info.log2_min_pcm_luma_coding_block_size_minus3		= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
		sps_info.log2_diff_max_min_pcm_luma_coding_block_size	= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
		sps_info.pcm_loop_filter_disable_flag					= BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 1 );
	}
	
	sps_info.num_short_term_ref_pic_sets	= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
	if( sps_info.num_short_term_ref_pic_sets > D_BF_MUX_H265_SHORT_TERM_RPS_MAX_NUM ){
		BF_Debug_Print_Error(("bf_mux_hvcc_create : num_short_term_ref_pic_sets is over!!\n"));
		// ps area free.
		bf_mux_hvcc_create_ps_free( vps_address, sps_address, pps_address );
		return MP4_COMMON_NG;	/* pgr0524 */
	}
	
	for( i = 0; i < sps_info.num_short_term_ref_pic_sets; i++){
		
		ret = bf_mux_mp4_st_rps_analyze( i, sps_info.num_short_term_ref_pic_sets, NumDeltaPocs, &sps_bit_stream );
		if( ret != TRUE ){
			// ps area free.
			bf_mux_hvcc_create_ps_free( vps_address, sps_address, pps_address );
			return MP4_COMMON_NG;	/* pgr0524 */
		}
	}
	
	sps_info.long_term_ref_pics_present_flag	= BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 1 );
	if( sps_info.long_term_ref_pics_present_flag ) {
		
		sps_info.num_long_term_ref_pics_sps	= BF_MP4_Common_BS_Read_U_E( &sps_bit_stream );
		
		if( sps_info.num_long_term_ref_pics_sps > D_BF_MUX_H265_LONG_TERM_RPS_SPS_MAX_NUM ){
			BF_Debug_Print_Error(("bf_mux_hvcc_create : num_long_term_ref_pics_sps is over!!\n"));
			// ps area free.
			bf_mux_hvcc_create_ps_free( vps_address, sps_address, pps_address );
			return MP4_COMMON_NG;	/* pgr0524 */
		}
		
		for( i = 0; i < sps_info.num_long_term_ref_pics_sps; i++ ) {
			
			len = MUX_MIN( (sps_info.log2_max_pic_order_cnt_lsb_minus4 + 4), 16 );
			
			BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( &sps_bit_stream, len );	// lt_ref_pic_poc_lsb_sps[ i ] (xx bit)
			BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( &sps_bit_stream, 1 );		// used_by_curr_pic_lt_sps_flag[ i ] (1 bit)
		}
	}
	
	sps_info.sps_temporal_mvp_enabled_flag			= BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 1 );
	sps_info.strong_intra_smoothing_enabled_flag	= BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 1 );
	sps_info.vui_parameters_present_flag			= BF_MP4_Common_BS_Read_U_Nbit( &sps_bit_stream, 1 );
	
	if( sps_info.vui_parameters_present_flag == 1 ){
		
		bf_mux_mp4_vui_analyze( sps_info.sps_max_sub_layers_minus1, &sps_info.vui_parameters, &sps_bit_stream );
	}
	// SPS : The rest is unnecessary to HVCC.
	
	
	// PPS : Analysis.
	pps_info.pps_pic_parameter_set_id					= BF_MP4_Common_BS_Read_U_E( &pps_bit_stream );
	pps_info.pps_seq_parameter_set_id					= BF_MP4_Common_BS_Read_U_E( &pps_bit_stream );
	pps_info.dependent_slice_segments_enabled_flag		= BF_MP4_Common_BS_Read_U_Nbit( &pps_bit_stream, 1 );
	pps_info.output_flag_present_flag					= BF_MP4_Common_BS_Read_U_Nbit( &pps_bit_stream, 1 );
	pps_info.num_extra_slice_header_bits				= BF_MP4_Common_BS_Read_U_Nbit( &pps_bit_stream, 3 );
	pps_info.sign_data_hiding_enabled_flag				= BF_MP4_Common_BS_Read_U_Nbit( &pps_bit_stream, 1 );
	pps_info.cabac_init_present_flag					= BF_MP4_Common_BS_Read_U_Nbit( &pps_bit_stream, 1 );
	pps_info.num_ref_idx_l0_default_active_minus1		= BF_MP4_Common_BS_Read_U_E( &pps_bit_stream );
	pps_info.num_ref_idx_l1_default_active_minus1		= BF_MP4_Common_BS_Read_U_E( &pps_bit_stream );
	pps_info.init_qp_minus26							= BF_MP4_Common_BS_Read_S_E( &pps_bit_stream );
	pps_info.constrained_intra_pred_flag				= BF_MP4_Common_BS_Read_U_Nbit( &pps_bit_stream, 1 );
	pps_info.transform_skip_enabled_flag				= BF_MP4_Common_BS_Read_U_Nbit( &pps_bit_stream, 1 );
	pps_info.cu_qp_delta_enabled_flag					= BF_MP4_Common_BS_Read_U_Nbit( &pps_bit_stream, 1 );
	if( pps_info.cu_qp_delta_enabled_flag == 1 ){
		pps_info.diff_cu_qp_delta_depth					= BF_MP4_Common_BS_Read_U_E( &pps_bit_stream );
	}
	pps_info.pps_cb_qp_offset							= BF_MP4_Common_BS_Read_S_E( &pps_bit_stream );
	pps_info.pps_cr_qp_offset							= BF_MP4_Common_BS_Read_S_E( &pps_bit_stream );
	pps_info.pps_slice_chroma_qp_offsets_present_flag	= BF_MP4_Common_BS_Read_U_Nbit( &pps_bit_stream, 1 );
	pps_info.weighted_pred_flag							= BF_MP4_Common_BS_Read_U_Nbit( &pps_bit_stream, 1 );
	pps_info.weighted_bipred_flag						= BF_MP4_Common_BS_Read_U_Nbit( &pps_bit_stream, 1 );
	pps_info.transquant_bypass_enabled_flag				= BF_MP4_Common_BS_Read_U_Nbit( &pps_bit_stream, 1 );
	pps_info.tiles_enabled_flag							= BF_MP4_Common_BS_Read_U_Nbit( &pps_bit_stream, 1 );
	pps_info.entropy_coding_sync_enabled_flag			= BF_MP4_Common_BS_Read_U_Nbit( &pps_bit_stream, 1 );
	// PPS : The rest is unnecessary to HVCC.
	
	
	// - HVCC information create -
	
	// configurationVersion
	hvcc_info.configurationVersion	= 1;	// Fixed value
	
	// general_profile_space
	if( vps_info.profile_tier_level.general_profile_space != sps_info.profile_tier_level.general_profile_space ){
		BF_Debug_Print_Error(("bf_mux_hvcc_create : sps and pps general_profile_space is mismatch.\n"));
		// ps area free.
		bf_mux_hvcc_create_ps_free( vps_address, sps_address, pps_address );
		return MP4_COMMON_NG;	/* pgr0524 */
	}
	else{
		hvcc_info.general_profile_space = vps_info.profile_tier_level.general_profile_space;
	}
	
	// general_tier_flag / general_level_idc
	if( vps_info.profile_tier_level.general_tier_flag > sps_info.profile_tier_level.general_tier_flag ){
		// Set the value of the VPS.
		hvcc_info.general_tier_flag	= vps_info.profile_tier_level.general_tier_flag;
		hvcc_info.general_level_idc	= vps_info.profile_tier_level.general_level_idc;
	}
	else if( vps_info.profile_tier_level.general_tier_flag < sps_info.profile_tier_level.general_tier_flag ){
		// Set the value of the SPS.
		hvcc_info.general_tier_flag	= sps_info.profile_tier_level.general_tier_flag;
		hvcc_info.general_level_idc	= sps_info.profile_tier_level.general_level_idc;
	}
	else{
		// tier_flag is the same.
		if( vps_info.profile_tier_level.general_level_idc > sps_info.profile_tier_level.general_level_idc ){
			// Set the value of the VPS.
			hvcc_info.general_tier_flag	= vps_info.profile_tier_level.general_tier_flag;
			hvcc_info.general_level_idc	= vps_info.profile_tier_level.general_level_idc;
		}
		else{
			// Set the value of the SPS.
			hvcc_info.general_tier_flag	= sps_info.profile_tier_level.general_tier_flag;
			hvcc_info.general_level_idc	= sps_info.profile_tier_level.general_level_idc;
		}
	}
	
	// general_profile_idc
	hvcc_info.general_profile_idc					= MUX_MAX( vps_info.profile_tier_level.general_profile_idc, sps_info.profile_tier_level.general_profile_idc );
	
	// general_profile_compatibility_flags
	hvcc_info.general_profile_compatibility_flags	= vps_info.profile_tier_level.general_profile_compatibility_flags & sps_info.profile_tier_level.general_profile_compatibility_flags;
	
	// general_constraint_indicator_flags
	hvcc_info.general_constraint_indicator_flags	= vps_info.profile_tier_level.general_constraint_indicator_flags & sps_info.profile_tier_level.general_constraint_indicator_flags;
	
	// min_spatial_segmentation_idc
	if( sps_info.vui_parameters.min_spatial_segmentation_idc <= D_BF_MUX_H265_SPATIAL_SEGMENTATION_MAX_RANGE ){
		hvcc_info.min_spatial_segmentation_idc		= sps_info.vui_parameters.min_spatial_segmentation_idc;
	}
	else{
		hvcc_info.min_spatial_segmentation_idc		= 0;
	}
	
	// parallelismType
	if ( (pps_info.entropy_coding_sync_enabled_flag == 1) && (pps_info.tiles_enabled_flag == 1) ){
		hvcc_info.parallelismType = 0; // mixed-type parallel decoding
	}
	else if ( pps_info.entropy_coding_sync_enabled_flag == 1 ){
		hvcc_info.parallelismType = 3; // wavefront-based parallel decoding
	}
	else if ( pps_info.tiles_enabled_flag == 1 ){
		hvcc_info.parallelismType = 2; // tile-based parallel decoding
	}
	else{
		hvcc_info.parallelismType = 1; // slice-based parallel decoding
	}
#if 0	// ** If min_spatial_segmentation_idc is 0, it may be correct that the parallelismType to 0. **
	if( hvcc_info.min_spatial_segmentation_idc == 0 ){
		hvcc_info.parallelismType = 0;
	}
#endif
	
	// chromaFormat
	hvcc_info.chromaFormat				= sps_info.chroma_format_idc;
	
	// bitDepthLumaMinus8
	hvcc_info.bitDepthLumaMinus8		= sps_info.bit_depth_luma_minus8;
	
	// bitDepthChromaMinus8
	hvcc_info.bitDepthChromaMinus8		= sps_info.bit_depth_chroma_minus8;
	
	// avgFrameRate
	hvcc_info.avgFrameRate				= 0;	// Fixed value.
	
	// constantFrameRate
	hvcc_info.constantFrameRate			= 0;	// Fixed value.
	
	// numTemporalLayers
	hvcc_info.numTemporalLayers			= MUX_MAX( vps_info.vps_max_sub_layers_minus1, sps_info.sps_max_sub_layers_minus1 ) + 1;
	
	// temporalIdNested
	hvcc_info.temporalIdNested			= sps_info.sps_temporal_id_nesting_flag;
	
	// lengthSizeMinusOne
	hvcc_info.lengthSizeMinusOne		= 3;	// Fixed value.
	
	
	// - HVCC write -
	BF_MP4_Common_BS_Set_Pointer_Address( &hvcc_bit_stream, hvcc_adr );
	
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 8, hvcc_info.configurationVersion );						// configurationVersion					:8bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 2, hvcc_info.general_profile_space );						// general_profile_space				:2bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 1, hvcc_info.general_tier_flag );							// general_tier_flag					:1bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 5, hvcc_info.general_profile_idc );						// general_profile_idc					:5bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 32, hvcc_info.general_profile_compatibility_flags );		// general_profile_compatibility_flags	:32bit
	BF_MP4_Common_BS_Write_64_U_Nbit( &hvcc_bit_stream, 48, hvcc_info.general_constraint_indicator_flags );		// general_constraint_indicator_flags	:48bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 8, hvcc_info.general_level_idc );							// general_level_idc					:8bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 4, 0xF );													// reserved(0b1111)						:4bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 12, hvcc_info.min_spatial_segmentation_idc );				// min_spatial_segmentation_idc			:12bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 6, 0x3F );													// reserved(0b111111)					:6bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 2, hvcc_info.parallelismType );							// parallelismType						:2bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 6, 0x3F );													// reserved(0b111111)					:6bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 2, hvcc_info.chromaFormat );								// chromaFormat							:2bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 5, 0x1F );													// reserved(0b11111)					:5bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 3, hvcc_info.bitDepthLumaMinus8 );							// bitDepthLumaMinus8					:3bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 5, 0x1F );													// reserved(0b11111)					:5bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 3, hvcc_info.bitDepthChromaMinus8 );						// bitDepthChromaMinus8					:3bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 16, hvcc_info.avgFrameRate );								// avgFrameRate							:16bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 2, hvcc_info.constantFrameRate );							// constantFrameRate					:2bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 3, hvcc_info.numTemporalLayers );							// numTemporalLayers					:3bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 1, hvcc_info.temporalIdNested );							// temporalIdNested						:1bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 2, hvcc_info.lengthSizeMinusOne );							// lengthSizeMinusOne					:2bit
	BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 8, ps_info->array_num );									// numOfArrays							:8bit
	
	for( array_loop=0; array_loop < ps_info->array_num; array_loop++ ){
		
		BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 1, ps_info->array_elements[array_loop].array_completeness );		// array_completeness			:1bit
		BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 1, 0x0 );															// reserved(0b0)				:1bit
		BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 6, ps_info->array_elements[array_loop].nal_type );					// NAL_unit_type				:6bit
		BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 16, ps_info->array_elements[array_loop].nal_num );					// numNalus						:16bit
		
		for( nal_loop=0; nal_loop < ps_info->array_elements[array_loop].nal_num; nal_loop++ ){
			BF_MP4_Common_BS_Write_U_Nbit( &hvcc_bit_stream, 16, (ps_info->array_elements[array_loop].nal_size[nal_loop] - 4) );	// nalUnitLength				:16bit
			// Write NAL data
			memcpy(BF_MP4_Common_BS_Get_Pointer( &hvcc_bit_stream ), (UINT8*)(ps_info->array_elements[array_loop].nal_address[nal_loop] + 4), (ps_info->array_elements[array_loop].nal_size[nal_loop] - 4) );
			BF_MP4_Common_BS_Skip_Pointer_Address( &hvcc_bit_stream, (ps_info->array_elements[array_loop].nal_size[nal_loop] - 4) );
		}
	}
	
	// set hvcc size
	*hvcc_size = (ULONG)BF_MP4_Common_BS_Get_Pointer( &hvcc_bit_stream ) - (ULONG)hvcc_adr;
	
	// ps area free.
	bf_mux_hvcc_create_ps_free( vps_address, sps_address, pps_address );
	
	return MP4_COMMON_OK;	/* pgr0524 */
}

static VOID bf_mux_hvcc_create_ps_free( UINT8* vps_address, UINT8* sps_address, UINT8* pps_address )
{
	if( vps_address != NULL ){
		free( vps_address );
	}
	if( sps_address != NULL ){
		free( sps_address );
	}
	if( pps_address != NULL ){
		free( pps_address );
	}
	
	return;
}

static BOOL bf_mux_mp4_ptl_analyze( UINT32 profilePresentFlag, UINT32 maxNumSubLayersMinus1, T_BF_MUX_H265_PTL* ptl_info, T_MP4_COMMON_STREAM_CONTROL* bit_stream )
{
	UINT32	i;
	UINT32	sub_layer_profile_present_flag[D_BF_MUX_H265_SUBLAYER_MAX_NUM];
	UINT32	sub_layer_level_present_flag[D_BF_MUX_H265_SUBLAYER_MAX_NUM];
	
	memset( sub_layer_profile_present_flag, 0, sizeof(sub_layer_profile_present_flag) );
	memset( sub_layer_level_present_flag,   0, sizeof(sub_layer_level_present_flag) );
	
	if( profilePresentFlag ){
		ptl_info->general_profile_space					= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 2 );
		ptl_info->general_tier_flag						= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
		ptl_info->general_profile_idc					= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 5 );
		ptl_info->general_profile_compatibility_flags	= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 32 );
		ptl_info->general_constraint_indicator_flags	= BF_MP4_Common_BS_Read_64_U_Nbit( bit_stream, 48 );
	}
	
	ptl_info->general_level_idc = BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 8 );
	
	for( i = 0; i < maxNumSubLayersMinus1; i++ ){
		sub_layer_profile_present_flag[i]	= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
		sub_layer_level_present_flag[i]		= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
	}
	
	if( maxNumSubLayersMinus1 > 0 ){
		for( i = maxNumSubLayersMinus1; i < 8; i++ ){
			BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( bit_stream, 2 );	// reserved_zero_2bits (2 bit)
		}
		
		for( i = 0; i < maxNumSubLayersMinus1; i++ ) {
			if( sub_layer_profile_present_flag[i] ) {
//				sub_layer_profile_space[ i ]                       2 bit
//				sub_layer_tier_flag[ i ]                           1 bit
//				sub_layer_profile_idc[ i ]                         5 bit
//				sub_layer_profile_compatibility_flag[ i ][ j ]    32 bit
//				sub_layer_progressive_source_flag[ i ]             1 bit
//				sub_layer_interlaced_source_flag[ i ]              1 bit
//				sub_layer_non_packed_constraint_flag[ i ]          1 bit
//				sub_layer_frame_only_constraint_flag[ i ]          1 bit
//				sub_layer_reserved_zero_43bits[ i ]               43 bit
//				sub_layer_reserved_zero_bit[ i ]                   1 bit
//                                                          total 88 bit
				BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( bit_stream, 88 );
			}
			if( sub_layer_level_present_flag[i] == 1 ){
				BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( bit_stream, 8 );	// sub_layer_level_idc (8 bit)
			}
		}
	}
	
	return TRUE;
}

static BOOL bf_mux_mp4_scaling_list_analyze( T_MP4_COMMON_STREAM_CONTROL* bit_stream )
{
	UINT32	sizeId;
	UINT32	matrixId;
	UINT32	i;
	UINT32	coefNum;
	UINT32	scaling_list_pred_mode_flag;
	
	for( sizeId = 0; sizeId < 4; sizeId++ ){
		
		for( matrixId = 0; matrixId < 6; matrixId += ( sizeId == 3 ) ? 3 : 1 ){
			
			scaling_list_pred_mode_flag = BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
			
			if( scaling_list_pred_mode_flag == 0 ){
				BF_MP4_Common_BS_Read_U_E( bit_stream );	// scaling_list_pred_matrix_id_delta
			}
			else{
				
				coefNum = MUX_MIN( 64, ( 1 << ( 4 + ( sizeId << 1 ) ) ) );
				
				if( sizeId > 1 ){
					BF_MP4_Common_BS_Read_S_E( bit_stream );	// scaling_list_dc_coef_minus8[ sizeId - 2 ][ matrixId ]
				}
				
				for( i = 0; i < coefNum; i++){
					BF_MP4_Common_BS_Read_S_E( bit_stream );	// scaling_list_delta_coef
				}
			}
		}
	}
	
	return TRUE;
}

static BOOL bf_mux_mp4_st_rps_analyze( UINT32 stRpsIdx, UINT32 num_short_term_ref_pic_sets, UINT32 NumDeltaPocs[D_BF_MUX_H265_SHORT_TERM_RPS_MAX_NUM], T_MP4_COMMON_STREAM_CONTROL* bit_stream )
{
	UINT32	i;
	UINT32	j;
	UINT32	inter_ref_pic_set_prediction_flag = 0;
	UINT32	num_negative_pics=0;
	UINT32	num_positive_pics=0;
	UINT32	used_by_curr_pic_flag=0;
	UINT32	use_delta_flag=0;
	
	if( stRpsIdx != 0 ){
		inter_ref_pic_set_prediction_flag = BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
	}
	
	if( inter_ref_pic_set_prediction_flag == 1 ){
		
		if( stRpsIdx >= num_short_term_ref_pic_sets ){
			BF_Debug_Print_Error(("bf_mux_mp4_st_rps_analyze : stRpsIdx is greater than or num_short_term_ref_pic_sets.\n"));
			return FALSE;
		}
		
		BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( bit_stream, 1 );	// delta_rps_sign
		BF_MP4_Common_BS_Read_U_E( bit_stream );					// abs_delta_rps_minus1
		
		NumDeltaPocs[stRpsIdx] = 0;
		
		for( j = 0; j <= NumDeltaPocs[stRpsIdx - 1]; j++ ){
			
			used_by_curr_pic_flag = BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
			if( used_by_curr_pic_flag == 0 ){
				use_delta_flag = BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
			}
			
			if( (used_by_curr_pic_flag==1) || (use_delta_flag==1) ){
				NumDeltaPocs[stRpsIdx]++;
			}
		}
	}
	else{
		num_negative_pics	= BF_MP4_Common_BS_Read_U_E( bit_stream );
		num_positive_pics	= BF_MP4_Common_BS_Read_U_E( bit_stream );
		
		NumDeltaPocs[stRpsIdx]	= num_negative_pics + num_positive_pics;
		
		for( i = 0; i < num_negative_pics; i++ ){
			BF_MP4_Common_BS_Read_U_E( bit_stream );					// delta_poc_s0_minus1[ i ]
			BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( bit_stream, 1 );	// used_by_curr_pic_s0_flag[ i ]
		}
		
		for( i = 0; i < num_positive_pics; i++ ){
			BF_MP4_Common_BS_Read_U_E( bit_stream );					// delta_poc_s1_minus1[ i ]
			BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( bit_stream, 1 );	// used_by_curr_pic_s1_flag[ i ]
		}
	}
	
	return TRUE;
}

static BOOL bf_mux_mp4_vui_analyze( UINT32 sps_max_sub_layers_minus1, T_BF_MUX_H265_VUI* vui_info, T_MP4_COMMON_STREAM_CONTROL* bit_stream )
{
	vui_info->aspect_ratio_info_present_flag = BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
	if( vui_info->aspect_ratio_info_present_flag == 1 ){
		
		vui_info->aspect_ratio_idc = BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 8 );
		if( vui_info->aspect_ratio_idc == D_BF_MUX_H265_ASPECT_RATIO_IDC_EXTENDEC_SAR ) {
			vui_info->sar_width		= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 16 );
			vui_info->sar_height	= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 16 );
		}
	}
	
	vui_info->overscan_info_present_flag = BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
	if( vui_info->overscan_info_present_flag == 1 ){
		vui_info->overscan_appropriate_flag = BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
	}
	
	vui_info->video_signal_type_present_flag = BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
	if( vui_info->video_signal_type_present_flag == 1 ) {
		
		vui_info->video_format						= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 3 );
		vui_info->video_full_range_flag				= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
		vui_info->colour_description_present_flag	= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
		
		if( vui_info->colour_description_present_flag == 1 ) {
			vui_info->colour_primaries			= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 8 );
			vui_info->transfer_characteristics	= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 8 );
			vui_info->matrix_coeffs				= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 8 );
		}
	}
	
	vui_info->chroma_loc_info_present_flag	= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
	if( vui_info->chroma_loc_info_present_flag == 1 ) {
		vui_info->chroma_sample_loc_type_top_field		= BF_MP4_Common_BS_Read_U_E( bit_stream );
		vui_info->chroma_sample_loc_type_bottom_field	= BF_MP4_Common_BS_Read_U_E( bit_stream );
	}
	vui_info->neutral_chroma_indication_flag	= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
	vui_info->field_seq_flag					= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
	vui_info->frame_field_info_present_flag		= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
	vui_info->default_display_window_flag		= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
	if( vui_info->default_display_window_flag == 1 ) {
		vui_info->def_disp_win_left_offset		= BF_MP4_Common_BS_Read_U_E( bit_stream );
		vui_info->def_disp_win_right_offset		= BF_MP4_Common_BS_Read_U_E( bit_stream );
		vui_info->def_disp_win_top_offset		= BF_MP4_Common_BS_Read_U_E( bit_stream );
		vui_info->def_disp_win_bottom_offset	= BF_MP4_Common_BS_Read_U_E( bit_stream );
	}
	
	vui_info->vui_timing_info_present_flag		= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
	if( vui_info->vui_timing_info_present_flag == 1 ) {
		vui_info->vui_num_units_in_tick					= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 32 );
		vui_info->vui_time_scale						= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 32 );
		vui_info->vui_poc_proportional_to_timing_flag	= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
		if( vui_info->vui_poc_proportional_to_timing_flag == 1 ){
			vui_info->vui_num_ticks_poc_diff_one_minus1	= BF_MP4_Common_BS_Read_U_E( bit_stream );
		}
		
		vui_info->vui_hrd_parameters_present_flag	= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
		if( vui_info->vui_hrd_parameters_present_flag == 1 ){
			bf_mux_mp4_hrd_analyze( 1, sps_max_sub_layers_minus1, bit_stream );
		}
	}
	
	vui_info->bitstream_restriction_flag	= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
	if( vui_info->bitstream_restriction_flag == 1 ) {
		
		vui_info->tiles_fixed_structure_flag				= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
		vui_info->motion_vectors_over_pic_boundaries_flag	= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
		vui_info->restricted_ref_pic_lists_flag				= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
		vui_info->min_spatial_segmentation_idc				= BF_MP4_Common_BS_Read_U_E( bit_stream );
		vui_info->max_bytes_per_pic_denom					= BF_MP4_Common_BS_Read_U_E( bit_stream );
		vui_info->max_bits_per_min_cu_denom					= BF_MP4_Common_BS_Read_U_E( bit_stream );
		vui_info->log2_max_mv_length_horizontal				= BF_MP4_Common_BS_Read_U_E( bit_stream );
		vui_info->log2_max_mv_length_vertical				= BF_MP4_Common_BS_Read_U_E( bit_stream );
	}
	
	return TRUE;
}

static BOOL bf_mux_mp4_hrd_analyze( UINT32 commonInfPresentFlag, UINT32 maxNumSubLayersMinus1, T_MP4_COMMON_STREAM_CONTROL* bit_stream )
{
	UINT32	i;
	UINT32	nal_hrd_parameters_present_flag	= 0;
	UINT32	vcl_hrd_parameters_present_flag	= 0;
	UINT32	sub_pic_hrd_params_present_flag	= 0;
	UINT32	fixed_pic_rate_general_flag		= 0;
	UINT32	fixed_pic_rate_within_cvs_flag	= 0;
	UINT32	low_delay_hrd_flag				= 0;
	UINT32	cpb_cnt_minus1					= 0;
	
	if( commonInfPresentFlag == 1 ) {
		
		nal_hrd_parameters_present_flag	= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
		vcl_hrd_parameters_present_flag	= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
		
		if( (nal_hrd_parameters_present_flag == 1) || (vcl_hrd_parameters_present_flag == 1) ){
			
			sub_pic_hrd_params_present_flag	= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
			if( sub_pic_hrd_params_present_flag == 1 ) {
				
				BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( bit_stream, 8 );	// tick_divisor_minus2
				BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( bit_stream, 5 );	// du_cpb_removal_delay_increment_length_minus1
				BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( bit_stream, 1 );	// sub_pic_cpb_params_in_pic_timing_sei_flag
				BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( bit_stream, 5 );	// dpb_output_delay_du_length_minus1
			}
			BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( bit_stream, 4 );	// bit_rate_scale
			BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( bit_stream, 4 );	// cpb_size_scale
			
			if( sub_pic_hrd_params_present_flag == 1 ){
				BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( bit_stream, 4 );	// cpb_size_du_scale
			}
			
			BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( bit_stream, 5 );	// initial_cpb_removal_delay_length_minus1
			BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( bit_stream, 5 );	// au_cpb_removal_delay_length_minus1
			BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( bit_stream, 5 );	// dpb_output_delay_length_minus1
		}
	}
	
	for( i = 0; i <= maxNumSubLayersMinus1; i++ ) {
		
		fixed_pic_rate_general_flag	= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
		if( fixed_pic_rate_general_flag == 0 ){
			fixed_pic_rate_within_cvs_flag	= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
		}
		if( fixed_pic_rate_within_cvs_flag == 1 ){
			BF_MP4_Common_BS_Read_U_E( bit_stream );	// elemental_duration_in_tc_minus1
		}
		else{
			low_delay_hrd_flag	= BF_MP4_Common_BS_Read_U_Nbit( bit_stream, 1 );
		}
		if( low_delay_hrd_flag == 0 ){
			cpb_cnt_minus1 = BF_MP4_Common_BS_Read_U_E( bit_stream );
			if( cpb_cnt_minus1 > 31 ){
				BF_Debug_Print_Error(("bf_mux_mp4_hrd_analyze : cpb_cnt_minus1 error!!.\n"));
				return FALSE;
			}
		}
		if( nal_hrd_parameters_present_flag == 1 ){
			bf_mux_mp4_sub_layer_hrd_analyze( cpb_cnt_minus1, sub_pic_hrd_params_present_flag, bit_stream );
		}
		if( vcl_hrd_parameters_present_flag == 1 ){
			bf_mux_mp4_sub_layer_hrd_analyze( cpb_cnt_minus1, sub_pic_hrd_params_present_flag, bit_stream );
		}
	}
	
	return TRUE;
}

static BOOL bf_mux_mp4_sub_layer_hrd_analyze( UINT32 cpb_cnt_minus1, UINT32 sub_pic_hrd_params_present_flag, T_MP4_COMMON_STREAM_CONTROL* bit_stream )
{
	UINT32	i;
	
	for( i = 0; i <= cpb_cnt_minus1; i++ ) {
		BF_MP4_Common_BS_Read_U_E( bit_stream );	// bit_rate_value_minus1[ i ]
		BF_MP4_Common_BS_Read_U_E( bit_stream );	// cpb_size_value_minus1[ i ]
		
		if( sub_pic_hrd_params_present_flag == 1 ){
			BF_MP4_Common_BS_Read_U_E( bit_stream );	// cpb_size_du_value_minus1[ i ]
			BF_MP4_Common_BS_Read_U_E( bit_stream );	// bit_rate_du_value_minus1[ i ]
		}
		BF_MP4_Common_BS_Skip_Pointer_Scope_Bit( bit_stream, 1 );	// cbr_flag[ i ]
	}
	
	return TRUE;
}


/**
 *	@brief		Check video sample time.\n
 *	@param		i:pCh_Ctrl
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
static INT32 bf_mux_video_sample_time_check( T_BF_MUX_CHANNEL_CTRL* pCh_Ctrl, UINT32 frame_type )
{
	INT32 ret = MP4_COMMON_OK;
	ULLONG next_duration;
	ULONG next_time;
	
	pCh_Ctrl->total_duration += pCh_Ctrl->video->time_increment;
	
	if( frame_type != FJ_MOVIE_VIDEO_FRAME_TYPE_B ){
		if( pCh_Ctrl->video->gop_struct == FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP ){
			next_duration = pCh_Ctrl->total_duration + (pCh_Ctrl->video->time_increment * 3 );
			next_time = next_duration / pCh_Ctrl->video->time_scale;
		}
		else{
			next_duration = pCh_Ctrl->total_duration + (pCh_Ctrl->video->time_increment );
			next_time = next_duration / pCh_Ctrl->video->time_scale;
		}
		
		if( next_time >= pCh_Ctrl->video->max_time ){
			ret = MP4_COMMON_OK_MAX_TIME_END;
		}
	}
	
	return ret;
}

/**
 *	@brief		Check video sample size in Media Vacant Area.\n
 *	@param		i:pCh_Ctrl
 *	@param		i:sample_size
 *	@return		OK/NG
 *	@note		None
 *	@attention	None
 */
static INT32 bf_mux_video_sample_size_check( INT32 stream_ch, T_BF_MUX_CHANNEL_CTRL* pCh_Ctrl, ULONG sample_size )
{
	ULLONG	temp_media_size;
	ULLONG	temp_first_size;
	UINT32	audio_size;
	UINT32	video_size;
	UINT32	sec_data_size;
	INT32	ret = MP4_COMMON_OK;
	UCHAR	media_judge = 1;
	UCHAR	aud_id = 0;
	
	temp_media_size = gMux_Ctrl.media_size;
	
	if( temp_media_size > pCh_Ctrl->video->max_size ){
		temp_media_size = pCh_Ctrl->video->max_size;
		media_judge = 0;
	}
	
	if( pCh_Ctrl->first_flag == D_BF_MUX_ON ){
		temp_first_size = temp_media_size;
		
		// Media_Size - Fixed_Atom_Size
		if( gMux_Config.audio[0].enable == TRUE ){
			if( temp_media_size < (D_BF_MUX_MP4_ATOM_VIDEO_SIZE + D_BF_MUX_MP4_ATOM_AUDIO_SIZE) ){
				return MP4_COMMON_NG_FILE_SIZE;
			}
			temp_media_size -= (D_BF_MUX_MP4_ATOM_VIDEO_SIZE + D_BF_MUX_MP4_ATOM_AUDIO_SIZE);
		}
		else{
			if( temp_media_size < D_BF_MUX_MP4_ATOM_VIDEO_SIZE ){
				return MP4_COMMON_NG_FILE_SIZE;
			}
			temp_media_size -= D_BF_MUX_MP4_ATOM_VIDEO_SIZE;
		}
		
		D_BF_MUX_LOCK();
		gMux_Ctrl.media_size = gMux_Ctrl.media_size - (temp_first_size - temp_media_size );
		pCh_Ctrl->video->max_size = pCh_Ctrl->video->max_size - (temp_first_size - temp_media_size );
		D_BF_MUX_UNLOCK();
		pCh_Ctrl->first_flag = D_BF_MUX_OFF;
	}
	
	video_size = pCh_Ctrl->video->bitrate/8;
	
	if( gMux_Config.audio[0].enable == TRUE && gMux_Config.audio[1].enable == TRUE && stream_ch == E_FJ_MUX_CH_1) {
		// if 2 video streams, and 2 audio streams, we assign audio stream1 to video stream1
		aud_id = 1;
	} else {
		aud_id = 0;
	}
	if( gMux_Config.audio[aud_id].enable == TRUE ){
		audio_size = gMux_Config.audio[aud_id].bitrate/8;
	} else {
		audio_size = 0;
	}

	if( BF_AudioEnc_GetTestMode() == 0 ){
		sec_data_size = (D_BF_MUX_MP4_ATOM_VIDEO_CHUNK(pCh_Ctrl->video->frame_num_of_gop)+D_BF_MUX_MP4_ATOM_AUDIO_AAC_CHUNK) * gMux_Ctrl.chunk_per_sec;
	}
	else{
		sec_data_size = (D_BF_MUX_MP4_ATOM_VIDEO_CHUNK(pCh_Ctrl->video->frame_num_of_gop)+D_BF_MUX_MP4_ATOM_AUDIO_PCM_CHUNK) * gMux_Ctrl.chunk_per_sec;
	}

	sec_data_size += video_size + audio_size;			// 2 sec (MAX case)
	
	if( temp_media_size < (((ULLONG)sec_data_size*2) + sample_size)){
		// Under 2sec Data
		if(media_judge == 0){
			ret = MP4_COMMON_NG_FILE_SIZE;
		}
		else{
			ret = MP4_COMMON_NG_MEDIA_AREA_NOTHING;
		}
	}
	
	return ret;
}

/**
 *	@brief		Calculate Movie Record Maximum Time(sec) from designated size.
 *				The Maximum Record Time is computed according to MENU at that time.
 *	@param		i:media_size		Media Size(byte)
 *	@param		i:record_info		Record Condition
 *	@param		i:static_flag		Calculate is Static Flag
 *									H264_ON  : Static Calculate
 *									H264_OFF : Dinamic Calculate
 *	@param		o:recordable_size	Total Recordable Size
 *	@param		o:size_per_sec		Record Size Per Sec
 *	@return		Record Time(sec)
 *	@note		None
 *	@attention	None
*/
static LONG bf_mux_record_maxtime_calculate( INT32 stream_ch, T_BF_MUX_CHANNEL_CTRL* pCh_Ctrl, ULLONG media_size, ULLONG* recordable_size, ULONG* size_per_sec )
{
	LONG		max_time;
	ULONG		video_size;
	ULONG		audio_size;
	UINT32		sec_data_size;
	UCHAR		aud_id = 0;
	
	/* Checks media size with Header Data Size */
	video_size = pCh_Ctrl->video->bitrate/8;
	
	if( gMux_Config.audio[0].enable == TRUE && gMux_Config.audio[1].enable == TRUE && stream_ch == E_FJ_MUX_CH_1) {
		// if 2 video streams, and 2 audio streams, we assign audio stream1 to video stream1
		aud_id = 1;
	} else {
		aud_id = 0;
	}

	if( gMux_Config.audio[aud_id].enable == TRUE ){
		audio_size = (gMux_Config.audio[aud_id].bitrate / 8);
	} else{
		audio_size = 0;
	}
	
	if( BF_AudioEnc_GetTestMode() == 0 ){
		sec_data_size = (D_BF_MUX_MP4_ATOM_VIDEO_CHUNK(pCh_Ctrl->video->frame_num_of_gop)+D_BF_MUX_MP4_ATOM_AUDIO_AAC_CHUNK) * gMux_Ctrl.chunk_per_sec;
	}
	else{
		sec_data_size = (D_BF_MUX_MP4_ATOM_VIDEO_CHUNK(pCh_Ctrl->video->frame_num_of_gop)+D_BF_MUX_MP4_ATOM_AUDIO_PCM_CHUNK) * gMux_Ctrl.chunk_per_sec;
	}
	
	sec_data_size += video_size + audio_size;			// 1 sec (MAX case)
	
	*recordable_size = media_size;
	*size_per_sec = sec_data_size;
	max_time = media_size / sec_data_size;
	
	return max_time;
}

/**
 *	@brief		Get Recordable Maximum Time before record(sec).\n
 *				The Maximum Record Time is computed according to MENU at that time.\n
 *	@param		None
 *	@return		Record Total Time(sec)
 *	@note		None
 *	@attention	None
*/
static LONG bf_mux_record_maxtime_get_static( INT32 stream_ch, T_BF_MUX_CHANNEL_CTRL* pCh_Ctrl, ULLONG media_size, ULLONG* recordable_size, ULONG* size_per_sec )
{
	LONG	recordable_time ;
	ULLONG	tmp_media_size;
	
	if( (recordable_size == NULL) || (size_per_sec == NULL) ){
		BF_Debug_Print_Error(("--- record_info is NULL on bf_mux_record_maxtime_get_static\n"));
		return -1;
	}
	
	tmp_media_size = media_size;
	
	if( gMux_Config.audio[0].enable == TRUE ){
		if( tmp_media_size < ((ULLONG)D_BF_MUX_MP4_ATOM_VIDEO_SIZE + D_BF_MUX_MP4_ATOM_AUDIO_SIZE) ){
			return 0;
		}
		/* ATOM Default Size(Fixed Size) */
		tmp_media_size -= ((ULLONG)D_BF_MUX_MP4_ATOM_VIDEO_SIZE + D_BF_MUX_MP4_ATOM_AUDIO_SIZE);
	}
	else{
		if( tmp_media_size < ((ULLONG)D_BF_MUX_MP4_ATOM_VIDEO_SIZE) ){
			return 0;
		}
		/* ATOM Default Size(Fixed Size) */
		tmp_media_size -= ((ULLONG)D_BF_MUX_MP4_ATOM_VIDEO_SIZE);
	}
	
	recordable_time = bf_mux_record_maxtime_calculate( stream_ch, pCh_Ctrl, tmp_media_size, recordable_size, size_per_sec );
	
	return recordable_time;
}

/**
 *	@brief		Close all file.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
static VOID bf_mux_file_close_all( VOID )
{
	T_BF_MUX_CHANNEL_CTRL* pCh_Ctrl;
	INT32 stream_ch;
	for( stream_ch=0; stream_ch<E_FJ_MUX_CH_MAX; stream_ch++ ){
		pCh_Ctrl = &(gMux_Ctrl.ch_ctrl[stream_ch]);
		if( pCh_Ctrl->video->enable == TRUE )
			MP4F_Close( pCh_Ctrl->format_ctrl.mp4f_handle );
		}
}


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
INT32 BF_Mux_Initialize( T_FJ_MUX_CONFIG* config )
{
	INT32 ret;
	
	if( config == NULL ){
		BF_Debug_Print_Error(("- MUX:BF_Mux_Initialize Input param error\n"));
		return MP4_COMMON_NG;
	}
	
	memset( (VOID*)&gMux_Ctrl, 0, sizeof(gMux_Ctrl) );
	
	memcpy( (VOID*)&gMux_Config, config, sizeof(T_FJ_MUX_CONFIG) );
	
	ret = bf_mux_init_param();
	if( ret != MP4_COMMON_OK ){
		return MP4_COMMON_NG;
	}
	
	return MP4_COMMON_OK;
}


INT32 BF_Mux_FixedAtomSetting( UINT32 stream_id, T_FJ_MUX_H264_SPS_PPS_INFO* sps_pps_info, T_FJ_MUX_H265_PS_INFO* ps_info )
{
	INT32 ret = MP4_COMMON_OK;
	INT32 stream_ch;
	
	T_BF_MUX_CHANNEL_CTRL* pCh_Ctrl = bf_mux_channel_ctrl_get(stream_id, &stream_ch);
	if( pCh_Ctrl == NULL ){
		BF_Debug_Print_Error(("- MUX:BF_Mux_FixedAtomSetting not enable ID\n"));
		return MP4_COMMON_NG;
	}
	
	if( pCh_Ctrl->video->enable == TRUE ){
		ret = bf_mux_fixed_atom_create2( stream_ch, &(pCh_Ctrl->format_ctrl), sps_pps_info, ps_info );
		if( ret != MP4_COMMON_OK ){	// System Error
			BF_Debug_Print_Error(("- MUX:BF_Mux_FixedAtomSetting. atom error\n" ));
			return MP4_COMMON_NG;
		}
	}
	
	return MP4_COMMON_OK;
}


INT32 BF_Mux_VideoAddSample(
		UINT32		stream_id,
		UCHAR*		sample_address,
		ULONG		sample_size,
		ULONG		ct_offset,
		UINT32		frame_type,
		UCHAR		end_chunk_flg,
		UCHAR		end_flg
		)
{
	MP4FErr			mp4err;
	ULONG			loop_index;
	INT32			i;
	BOOL			free_enable;
	INT32			ret = MP4_COMMON_OK;
	INT32			ret_result;
	UINT8			force_stop_flag = D_BF_MUX_OFF;
	INT32			stream_ch;
	BOOL			not_addsample = TRUE;
	ULLONG			next_audio_duration;
	UCHAR			aud_id = 0;
	
	T_BF_MUX_CHANNEL_CTRL* pCh_Ctrl;
	T_BF_MUX_AUDIO_CTRL* pAudio_ctrl;
	
	if( sample_address == NULL ){
		BF_Debug_Print_Error(("- MUX:BF_Mux_VideoAddSample Input param error\n"));
		return MP4_COMMON_NG;
	}
	
	if( frame_type == FJ_MOVIE_VIDEO_FRAME_TYPE_IDR ){
		frame_type = MP4F_SYNC_SAMPLE;
	}
	else{
		frame_type = MP4F_NO_SYNC_SAMPLE;
	}
	
	pCh_Ctrl = bf_mux_channel_ctrl_get( stream_id, &stream_ch );
	if( pCh_Ctrl == NULL ){
		BF_Debug_Print_Error(("- MUX:BF_Mux_VideoAddSample not enable ID\n"));
		return MP4_COMMON_NG;
	}
	
	if( (pCh_Ctrl->video->enable != TRUE) || (pCh_Ctrl->force_stop == D_BF_MUX_ON) ){
		return MP4_COMMON_OK;
	}
	
	if( frame_type != FJ_MOVIE_VIDEO_FRAME_TYPE_B ){
		ret_result = bf_mux_video_sample_size_check( stream_ch, pCh_Ctrl, sample_size );
		if(ret_result != MP4_COMMON_OK){
			if(ret_result == MP4_COMMON_NG_FILE_SIZE){
//				BF_Debug_Print_Information(("MAX file end\n"));
				ret = MP4_COMMON_OK_MAX_SIZE_END;
			}
			else if(ret_result == MP4_COMMON_NG_MEDIA_AREA_NOTHING){
				// Set Event code : Media full
				FJ_Host_Mux_VideoAddSampleCB( D_BF_MUX_EVENT_CODE_STOP_MEDIA_FULL );
				memcpy( (UCHAR*)(sample_address+sample_size), (ULONG*)eosCode, D_BF_MUX_EOS_SIZE );
				sample_size = sample_size + D_BF_MUX_EOS_SIZE;
				force_stop_flag = D_BF_MUX_ON;
				ret = MP4_COMMON_OK_MEDIAFULL_END;
			}
		}
	}
	
	ret_result = bf_mux_video_sample_time_check( pCh_Ctrl, frame_type );
	if(ret_result != MP4_COMMON_OK){
		BF_Debug_Print_Information(("MAX time end\n"));
		ret = MP4_COMMON_OK_MAX_TIME_END;
	}
	
	if( (force_stop_flag == D_BF_MUX_ON) && (end_flg != FJ_MUX_ON) ){
		memcpy( (UCHAR*)(sample_address+sample_size), (ULONG*)eosCode, D_BF_MUX_EOS_SIZE );
		sample_size = sample_size + D_BF_MUX_EOS_SIZE;
		end_flg = FJ_MUX_ON;
	}
	
	D_BF_MUX_LOCK();
	gMux_Ctrl.media_size		-= sample_size;
	pCh_Ctrl->video->max_size	-= sample_size;
	D_BF_MUX_UNLOCK();
	
	mp4err = MP4F_AddSample( pCh_Ctrl->format_ctrl.mp4f_handle,
							 pCh_Ctrl->format_ctrl.track_id[0],
							 sample_size,
							 gMux_Config.video[stream_ch].time_increment,
							 ct_offset,
							 frame_type,
							 sample_address );
	if( mp4err != MP4_COMMON_OK ){
		BF_Debug_Print_Error(("video add sample error. mp4err=%lx\n", mp4err));
		return MP4_COMMON_NG;
	}
	pCh_Ctrl->gop_loop_cnt++;
	
	if( (end_chunk_flg == FJ_MUX_ON) || (end_flg == FJ_MUX_ON) ){
		//Video chunk end
		D_BF_MUX_LOCK();
		gMux_Ctrl.media_size		-= D_BF_MUX_MP4_ATOM_VIDEO_CHUNK(pCh_Ctrl->video->frame_num_of_gop);
		pCh_Ctrl->video->max_size	-= D_BF_MUX_MP4_ATOM_VIDEO_CHUNK(pCh_Ctrl->video->frame_num_of_gop);
		D_BF_MUX_UNLOCK();
		
		mp4err = MP4F_EndCurrentChunk( pCh_Ctrl->format_ctrl.mp4f_handle,
									   pCh_Ctrl->format_ctrl.track_id[0],
									   pCh_Ctrl->format_ctrl.stsd_entry_id[0],
									   NULL );
		if( mp4err != MP4_COMMON_OK ){
			BF_Debug_Print_Error(("videoend chunk error. mp4err=%lx\n", mp4err));
			return MP4_COMMON_NG;
		}
		pCh_Ctrl->gop_loop_cnt = 0;

		if( gMux_Config.audio[0].enable == TRUE && gMux_Config.audio[1].enable == TRUE && stream_ch == E_FJ_MUX_CH_1) {
			// if 2 video streams, and 2 audio streams, we assign audio stream1 to video stream1
			aud_id = 1;
		}
		if( gMux_Config.audio[aud_id].enable == TRUE ) {
			pAudio_ctrl = &gMux_Ctrl.audio_ctrl[aud_id];
			//add audio
			for( loop_index=0; loop_index < SDRAM_BNK_MOVIE_RECORD0_AUDIO_STREAM; loop_index++ ){
				if( BF_AudioEnc_GetTestMode() == 0 ){
					next_audio_duration = pAudio_ctrl->audio_total_duration[stream_ch] + 1024;
				}
				else{
					next_audio_duration = pAudio_ctrl->audio_total_duration[stream_ch] + pAudio_ctrl->sample_size[pAudio_ctrl->read_index[stream_ch]]/2/gMux_Config.audio[aud_id].channel;
				}
				if( (next_audio_duration * pCh_Ctrl->duration_convert_ratio) > pCh_Ctrl->total_duration ){
					// Audio is under of Video
					break;
				}
				if( pAudio_ctrl->read_index[stream_ch] == pAudio_ctrl->write_index ){
					//no sample in buftbl
					break;
				}
				D_BF_MUX_LOCK();
				gMux_Ctrl.media_size		-= pAudio_ctrl->sample_size[pAudio_ctrl->read_index[stream_ch]];
				pCh_Ctrl->video->max_size	-= pAudio_ctrl->sample_size[pAudio_ctrl->read_index[stream_ch]];
				D_BF_MUX_UNLOCK();
				if( BF_AudioEnc_GetTestMode() == 0 ){
					mp4err = MP4F_AddSample( pCh_Ctrl->format_ctrl.mp4f_handle,
											 pCh_Ctrl->format_ctrl.track_id[1],
											 pAudio_ctrl->sample_size[pAudio_ctrl->read_index[stream_ch]],
											 1024,
											 0,
											 MP4F_SYNC_SAMPLE,
											 pAudio_ctrl->sample_data[pAudio_ctrl->read_index[stream_ch]] );
					
					pAudio_ctrl->audio_total_duration[stream_ch] += 1024;
				}
				else{
					mp4err = MP4F_AddSample( pCh_Ctrl->format_ctrl.mp4f_handle,
											 pCh_Ctrl->format_ctrl.track_id[1],
											 pAudio_ctrl->sample_size[pAudio_ctrl->read_index[stream_ch]],
											 pAudio_ctrl->sample_size[pAudio_ctrl->read_index[stream_ch]]/2/gMux_Config.audio[aud_id].channel,
											 0,
											 MP4F_SYNC_SAMPLE,
											 pAudio_ctrl->sample_data[pAudio_ctrl->read_index[stream_ch]] );
					
					pAudio_ctrl->audio_total_duration[stream_ch] += pAudio_ctrl->sample_size[pAudio_ctrl->read_index[stream_ch]]/2/gMux_Config.audio[aud_id].channel;
				}
				if( mp4err != MP4F_OK ){
					BF_Debug_Print_Error(("Audio add sample error. mp4err=%lx\n", mp4err));
					return MP4_COMMON_NG;
				}
				
				not_addsample = FALSE;
				
				pAudio_ctrl->area_use_flg[stream_ch][pAudio_ctrl->read_index[stream_ch]] = FJ_MUX_OFF;
				free_enable = TRUE;
				
				for( i=0; i<E_FJ_MUX_CH_MAX; i++ ){
					if( pAudio_ctrl->area_use_flg[i][pAudio_ctrl->read_index[stream_ch]] == FJ_MUX_ON ){
						free_enable = FALSE;
						// if 2 audios enabled, videos dont share same audio, so we free audio buffer here
						if( gMux_Config.audio[0].enable == TRUE && gMux_Config.audio[1].enable == TRUE) {
							free_enable = TRUE;
						}
						break;
					}
				}
				if( free_enable == TRUE ){
					FJ_Host_Mux_AudioFreeRequestCB( aud_id, pAudio_ctrl->area_index[pAudio_ctrl->read_index[stream_ch]], FALSE );
				}
				
				pAudio_ctrl->read_index[stream_ch]++;
				if( pAudio_ctrl->read_index[stream_ch] >= SDRAM_BNK_MOVIE_RECORD0_AUDIO_STREAM ){
					pAudio_ctrl->read_index[stream_ch] = 0;
				}
			}
//			printf("## %dV %d\n",pCh_Ctrl->stream_ch, loop_index);
			if( not_addsample != TRUE ){
				//Audio chunk end
				D_BF_MUX_LOCK();
				if( BF_AudioEnc_GetTestMode() == 0 ){
					gMux_Ctrl.media_size		-= D_BF_MUX_MP4_ATOM_AUDIO_AAC_CHUNK;
					pCh_Ctrl->video->max_size	-= D_BF_MUX_MP4_ATOM_AUDIO_AAC_CHUNK;
				}
				else{
					gMux_Ctrl.media_size		-= D_BF_MUX_MP4_ATOM_AUDIO_PCM_CHUNK;
					pCh_Ctrl->video->max_size	-= D_BF_MUX_MP4_ATOM_AUDIO_PCM_CHUNK;
				}
				D_BF_MUX_UNLOCK();
				
//				printf("## %lld %lld\n",gMux_Ctrl.media_size, pCh_Ctrl->video->max_size );
				
				mp4err = MP4F_EndCurrentChunk( pCh_Ctrl->format_ctrl.mp4f_handle,
											   pCh_Ctrl->format_ctrl.track_id[1],
											   pCh_Ctrl->format_ctrl.stsd_entry_id[1],
											   NULL );
				if( mp4err != MP4F_OK ){
					BF_Debug_Print_Error(("Audio end chunk error. mp4err=%lx\n", mp4err));
					return MP4_COMMON_NG;
				}
			}
			if( end_flg == FJ_MUX_ON ){
				pAudio_ctrl->end_flg[stream_ch] = FJ_MUX_ON;
				
				free_enable = TRUE;
				for( i=0; i<E_FJ_MUX_CH_MAX; i++ ){
					if( pAudio_ctrl->end_flg[stream_ch] != FJ_MUX_ON ){
						free_enable = FALSE;
						break;
					}
				}
				if( free_enable == TRUE ){
					FJ_Host_Mux_AudioFreeRequestCB( aud_id, 0, TRUE );
				}
			}
		}
		if( end_flg == FJ_MUX_ON ){
			pCh_Ctrl->force_stop = D_BF_MUX_ON;
		}
	}
	
	return ret;

}


INT32 BF_Mux_AudioAddSample( UCHAR stream_id, ULONG area_index, BYTE* sample_address, ULONG sample_size, ULONG duration )
{
	INT32 i;
	
	gMux_Ctrl.audio_ctrl[stream_id].sample_data[gMux_Ctrl.audio_ctrl[stream_id].write_index]	= sample_address;
	gMux_Ctrl.audio_ctrl[stream_id].sample_size[gMux_Ctrl.audio_ctrl[stream_id].write_index]	= sample_size;
	gMux_Ctrl.audio_ctrl[stream_id].area_index[gMux_Ctrl.audio_ctrl[stream_id].write_index]		= area_index;
	gMux_Ctrl.audio_ctrl[stream_id].duration[gMux_Ctrl.audio_ctrl[stream_id].write_index]		= duration;
	
	for( i=0; i<E_FJ_MUX_CH_MAX; i++ ){
		if( gMux_Config.video[i].enable == TRUE ){
			gMux_Ctrl.audio_ctrl[stream_id].area_use_flg[i][gMux_Ctrl.audio_ctrl[stream_id].write_index] = FJ_MUX_ON;
		}
	}
	
	gMux_Ctrl.audio_ctrl[stream_id].write_index++;
	if( gMux_Ctrl.audio_ctrl[stream_id].write_index >= SDRAM_BNK_MOVIE_RECORD0_AUDIO_STREAM ){
		gMux_Ctrl.audio_ctrl[stream_id].write_index = 0;
	}
	return MP4_COMMON_OK;
}

INT32 BF_Mux_AddFreeAtom( UINT32 stream_id, UCHAR* data_address, ULONG data_size )
{
	MP4FErr	mp4err;
	INT32	stream_ch;
	T_BF_MUX_CHANNEL_CTRL* pCh_Ctrl;
	
	if( data_size == 0 ){
		BF_Debug_Print_Error(("BF_Mux_VideoAddSample : error!! data_size is zero\n"));
		return MP4_COMMON_NG;
	}
	if( data_address == NULL ){
		BF_Debug_Print_Error(("BF_Mux_VideoAddSample : error!! data_address is null\n"));
		return MP4_COMMON_NG;
	}
	
	pCh_Ctrl = bf_mux_channel_ctrl_get( stream_id, &stream_ch );
	if( pCh_Ctrl == NULL ){
		BF_Debug_Print_Error(("BF_Mux_VideoAddSample : not enable ID\n"));
		return MP4_COMMON_NG;
	}
	
	if( pCh_Ctrl->video->enable != TRUE ){
		return MP4_COMMON_NG;
	}
	
	mp4err = MP4F_AddFree(
				pCh_Ctrl->format_ctrl.mp4f_handle,
				data_address,
				data_size
				);
	if( mp4err != MP4_COMMON_OK ){
		BF_Debug_Print_Error(("free atom add error. mp4err=%lx\n", mp4err));
		return MP4_COMMON_NG;
	}
	
	return MP4_COMMON_OK;
}

INT32 BF_Mux_Finalize( UINT32 stream_id )
{
	INT32	ret;
	INT32	stream_ch;
	INT32	tmp_stream_ch;
//	BOOL	end_flg;
	MP4FErr	mp4err;
	UCHAR aud_id = 0;
	
	T_BF_MUX_CHANNEL_CTRL* pCh_Ctrl = bf_mux_channel_ctrl_get( stream_id, &stream_ch );
	
	if( pCh_Ctrl->video->enable != TRUE ){
		return MP4_COMMON_OK;
	}
	
	if( pCh_Ctrl->gop_loop_cnt != 0 ){
		//Video chunk end
		mp4err = MP4F_EndCurrentChunk( pCh_Ctrl->format_ctrl.mp4f_handle,
									   pCh_Ctrl->format_ctrl.track_id[0],
									   pCh_Ctrl->format_ctrl.stsd_entry_id[0],
									   NULL );
		if( mp4err != MP4_COMMON_OK ){
			BF_Debug_Print_Error(("videoend chunk error. mp4err=%lx\n", mp4err));
			return MP4_COMMON_NG;
		}
	}
		
	// edts atom create
	if( pCh_Ctrl->video->gop_struct == FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP ){
		mp4err = MP4F_AddEditList( pCh_Ctrl->format_ctrl.mp4f_handle,
								   pCh_Ctrl->format_ctrl.track_id[0],
								   pCh_Ctrl->total_duration,
								   pCh_Ctrl->video->time_increment,
								   0x00010000 );
		if( mp4err != MP4F_OK ){
			BF_Debug_Print_Error(("Video MP4F_AddEditList: error. mp4err=%lx\n", mp4err));
			return MP4_COMMON_NG;
		}

		if( gMux_Config.audio[0].enable == TRUE && gMux_Config.audio[1].enable == TRUE && stream_ch == E_FJ_MUX_CH_1) {
			aud_id = 1;
		}
		if( gMux_Config.audio[aud_id].enable == TRUE ){
			mp4err = MP4F_AddEditList( pCh_Ctrl->format_ctrl.mp4f_handle,
									   pCh_Ctrl->format_ctrl.track_id[1],
									   (UINT32)(gMux_Ctrl.audio_ctrl[aud_id].audio_total_duration[stream_ch] * pCh_Ctrl->duration_convert_ratio),
									   0,
									   0x00010000 );
			if( mp4err != MP4F_OK ){
				BF_Debug_Print_Error(("Audio fj_mux_mp4_edts_data_set: error. mp4err=%lx\n", mp4err));
				return MP4_COMMON_NG;
			}
		}
	}
	
	// Write End
	// Free File Format Memory
	if( pCh_Ctrl->format_ctrl.mp4f_handle != NULL ){
		mp4err = MP4F_WriteToFile( pCh_Ctrl->format_ctrl.mp4f_handle );
		if( mp4err != MP4F_OK ){
			BF_Debug_Print_Error(("MP4F Write to File Error.mp4err=%lx\n",mp4err));
			return MP4_COMMON_NG;
		}
	}
	
	// Write Down FS Memory
	ret = BF_MP4_Common_Fs_Memory_Write_Down( pCh_Ctrl->video->file_id, FJ_MUX_ON );
	if( ret != MP4_COMMON_OK ){
		BF_Debug_Print_Error(("--- FJ_Mux_Finalize:BF_MP4_Common_Fs_Memory_Write_Down Error(%d)\n", ret));
	}
	
	// Close File Format Handller
	if( pCh_Ctrl->video->enable != FALSE ){
		MP4F_Close( pCh_Ctrl->format_ctrl.mp4f_handle );
		pCh_Ctrl->video->enable = FALSE;
	}
	
//	end_flg = TRUE;
	for( tmp_stream_ch=0; tmp_stream_ch<E_FJ_MUX_CH_MAX; tmp_stream_ch++ ){
		if( gMux_Ctrl.ch_ctrl[tmp_stream_ch].video->enable == TRUE ){
//			end_flg = FALSE;
			break;
		}
	}
	
	pCh_Ctrl->first_flag = D_BF_MUX_ON;
	pCh_Ctrl->force_stop = D_BF_MUX_OFF;
	
	return MP4_COMMON_OK;
}

// --- REMOVE_WIFI_EMMC_SAVE BEGIN ---
#ifdef CO_WIFI_EMMC_SAVE
VOID BF_Mux_WIFI_Out_EMMC( BOOL enable )
{
	BF_MP4_Common_WIFI_Out_EMMC(enable);
}
#endif
// --- REMOVE_WIFI_EMMC_SAVE END ---

INT32 BF_Mux_Atom_Creat(UINT32 stream_id, INT32 file_id)
{
	T_BF_MUX_CHANNEL_CTRL* pCh_Ctrl;
	INT32	stream_ch;
	INT32	ret;
	UCHAR	aud_id = 0;
	
	pCh_Ctrl = bf_mux_channel_ctrl_get( stream_id, &stream_ch );
	if( pCh_Ctrl == NULL ){
		BF_Debug_Print_Error(("- MUX:BF_Mux_VideoAddSample not enable ID\n"));
		return MP4_COMMON_NG;
	}
	
		//init param
	gMux_Ctrl.media_size		= BF_MP4_Common_Fs_Mediasize_Get();
	gMux_Ctrl.chunk_per_sec		= 2;	// [Interim]

	if( gMux_Config.audio[0].enable == TRUE && gMux_Config.audio[1].enable == TRUE && stream_ch == E_FJ_MUX_CH_1) {
		// if 2 video streams, and 2 audio streams, we assign audio stream1 to video stream1
		aud_id = 1;
	}
	if( gMux_Config.audio[aud_id].enable == TRUE ){
		gMux_Ctrl.audio_ctrl[aud_id].sample_max_count = (gMux_Config.audio[aud_id].samplingrate/1024/gMux_Ctrl.chunk_per_sec) + 1;
		
		if( gMux_Ctrl.media_size < (D_BF_MUX_MP4_ATOM_VIDEO_SIZE + D_BF_MUX_MP4_ATOM_AUDIO_SIZE) ){
			BF_Debug_Print_Error(("--- MEDIA SIZE=%llu\n",gMux_Ctrl.media_size ));
			return MP4_COMMON_NG_MEDIA_AREA_NOTHING;
		}
	}
	else{
		if( gMux_Ctrl.media_size < D_BF_MUX_MP4_ATOM_VIDEO_SIZE ){
			BF_Debug_Print_Error(("--- MEDIA SIZE=%llu\n",gMux_Ctrl.media_size ));
			return MP4_COMMON_NG_MEDIA_AREA_NOTHING;
		}
	}
	
	gMux_Config.video[stream_ch].pre_file_id = gMux_Config.video[stream_ch].file_id;
	pCh_Ctrl->format_ctrl.pre_mp4f_handle = pCh_Ctrl->format_ctrl.mp4f_handle;
	pCh_Ctrl->video->max_size = 0xFFFFFFFF;
	//pCh_Ctrl->video->max_size = 0x0C800000;
	pCh_Ctrl->video->max_time = 7200;
	pCh_Ctrl->first_flag = D_BF_MUX_ON;
	pCh_Ctrl->force_stop = D_BF_MUX_OFF;
	pCh_Ctrl->format_ctrl.pre_track_id[0] = pCh_Ctrl->format_ctrl.track_id[0];
	pCh_Ctrl->format_ctrl.pre_track_id[1] = pCh_Ctrl->format_ctrl.track_id[1];
	pCh_Ctrl->format_ctrl.pre_stsd_entry_id[0] = pCh_Ctrl->format_ctrl.stsd_entry_id[0];
	pCh_Ctrl->format_ctrl.pre_stsd_entry_id[1] = pCh_Ctrl->format_ctrl.stsd_entry_id[1];
	gMux_Config.video[stream_ch].file_id = file_id;
	
	// --- Set fixed ATOM Informations ---
	ret = bf_mux_fixed_atom_create( stream_ch, &(pCh_Ctrl->format_ctrl) );
	if( ret != MP4_COMMON_OK ){
		BF_Debug_Print_Error(("--- bf_mux_fixed_atom_create. ret=%d\n", ret));
		gMux_Config.video[stream_ch].file_id = gMux_Config.video[stream_ch].pre_file_id;
		gMux_Config.video[stream_ch].pre_file_id = 0;
		// Free the memory, and close file
		bf_mux_file_close_all();
		return ret;
	}
	pCh_Ctrl->new_file_request = FALSE;

	pCh_Ctrl->pre_total_duration = pCh_Ctrl->total_duration;
	pCh_Ctrl->total_duration = 0;
	pCh_Ctrl->pre_gop_loop_cnt = pCh_Ctrl->gop_loop_cnt;
	pCh_Ctrl->gop_loop_cnt = 0;
	gMux_Ctrl.audio_ctrl[aud_id].audio_pre_duration[stream_ch] = gMux_Ctrl.audio_ctrl[aud_id].audio_total_duration[stream_ch];
	gMux_Ctrl.audio_ctrl[aud_id].audio_total_duration[stream_ch] = 0;
	
	//Update memory Controller Param
	BF_MP4_Common_Fs_Memory_Update( file_id, FJ_MUX_MEMOPT_ADD);

	return MP4_COMMON_OK;
}

INT32 BF_Mux_Movie_File_Old_Set(UINT32 stream_id)
{
	T_BF_MUX_CHANNEL_CTRL* pCh_Ctrl;
	INT32	stream_ch;
	UCHAR	aud_id = 0;
	
	pCh_Ctrl = bf_mux_channel_ctrl_get( stream_id, &stream_ch );
	if( pCh_Ctrl == NULL ){
		BF_Debug_Print_Error(("- MUX:BF_Mux_VideoAddSample not enable ID\n"));
		return MP4_COMMON_NG;
	}
	
	if( gMux_Config.audio[0].enable == TRUE && gMux_Config.audio[1].enable == TRUE && stream_ch == E_FJ_MUX_CH_1) {
		// if 2 video streams, and 2 audio streams, we assign audio stream1 to video stream1
		aud_id = 1;
	}
	gMux_Config.video[stream_ch].pre_file_id = gMux_Config.video[stream_ch].file_id;
	pCh_Ctrl->format_ctrl.pre_mp4f_handle = pCh_Ctrl->format_ctrl.mp4f_handle;
	pCh_Ctrl->format_ctrl.pre_track_id[0] = pCh_Ctrl->format_ctrl.track_id[0];
	pCh_Ctrl->format_ctrl.pre_track_id[1] = pCh_Ctrl->format_ctrl.track_id[1];
	pCh_Ctrl->format_ctrl.pre_stsd_entry_id[0] = pCh_Ctrl->format_ctrl.stsd_entry_id[0];
	pCh_Ctrl->format_ctrl.pre_stsd_entry_id[1] = pCh_Ctrl->format_ctrl.stsd_entry_id[1];

	pCh_Ctrl->pre_total_duration = pCh_Ctrl->total_duration;
	pCh_Ctrl->pre_gop_loop_cnt = pCh_Ctrl->gop_loop_cnt;
	gMux_Ctrl.audio_ctrl[aud_id].audio_pre_duration[stream_ch] = gMux_Ctrl.audio_ctrl[aud_id].audio_total_duration[stream_ch];

	return MP4_COMMON_OK;
}

INT32 BF_Mux_Close_File(INT32 stream_id)
{
	INT32	ret;
	INT32	stream_ch;
	MP4FErr	mp4err;
	static T_BF_MUX_CHANNEL_CTRL* pCh_Ctrl;
	UCHAR	aud_id = 0;

	pCh_Ctrl = bf_mux_channel_ctrl_get( stream_id, &stream_ch );

	if( pCh_Ctrl->video->enable != TRUE ){
		return MP4_COMMON_OK;
	}

	if( pCh_Ctrl->pre_gop_loop_cnt != 0 ){
		//Video chunk end
		mp4err = MP4F_EndCurrentChunk( pCh_Ctrl->format_ctrl.pre_mp4f_handle,
									   pCh_Ctrl->format_ctrl.pre_track_id[0],
									   pCh_Ctrl->format_ctrl.pre_stsd_entry_id[0],
									   NULL );
		if( mp4err != MP4_COMMON_OK ){
			BF_Debug_Print_Error(("videoend chunk error. mp4err=%lu\n", mp4err));
			return MP4_COMMON_NG;
		}
	}
	
	if( gMux_Config.audio[0].enable == TRUE && gMux_Config.audio[1].enable == TRUE && stream_ch == E_FJ_MUX_CH_1) {
		// if 2 video streams, and 2 audio streams, we assign audio stream1 to video stream1
		aud_id = 1;
	}

	// edts atom create
	if( pCh_Ctrl->video->gop_struct == FJ_MOVIE_VIDEO_GOP_STRUCT_IBBP ){
		mp4err = MP4F_AddEditList( pCh_Ctrl->format_ctrl.pre_mp4f_handle,
								   pCh_Ctrl->format_ctrl.pre_track_id[0],
								   pCh_Ctrl->pre_total_duration,
								   pCh_Ctrl->video->time_increment,
								   0x00010000 );
		if( mp4err != MP4F_OK ){
			BF_Debug_Print_Error(("Video MP4F_AddEditList: error. mp4err=%lu\n", mp4err));
			return MP4_COMMON_NG;
		}
		
		if( gMux_Config.audio[aud_id].enable == TRUE ){
			mp4err = MP4F_AddEditList( pCh_Ctrl->format_ctrl.pre_mp4f_handle,
									   pCh_Ctrl->format_ctrl.pre_track_id[1],
									   (UINT32)(gMux_Ctrl.audio_ctrl[aud_id].audio_pre_duration[stream_ch] * pCh_Ctrl->duration_convert_ratio),
									   0,
									   0x00010000 );
			if( mp4err != MP4F_OK ){
				BF_Debug_Print_Error(("Audio fj_mux_mp4_edts_data_set: error. mp4err=%lu\n", mp4err));
				return MP4_COMMON_NG;
			}
		}
	}
	
	// Write End
	// Free File Format Memory
	if( pCh_Ctrl->format_ctrl.pre_mp4f_handle!= NULL ){
		mp4err = MP4F_WriteToFile( pCh_Ctrl->format_ctrl.pre_mp4f_handle);
		if( mp4err != MP4F_OK ){
			BF_Debug_Print_Error(("MP4F Write to File Error.mp4err=%lu\n",mp4err));
			return MP4_COMMON_NG;
		}
	}

	// Write Down FS Memory
	ret = BF_MP4_Common_Fs_Memory_Write_Down( pCh_Ctrl->video->pre_file_id, FJ_MUX_ON );
	if( ret != MP4_COMMON_OK ){
		BF_Debug_Print_Error(("--- FJ_Mux_Finalize:BF_MP4_Common_Fs_Memory_Write_Down Error(%d)\n", ret));
	}
	
	// Close File Format Handller
	MP4F_Close( pCh_Ctrl->format_ctrl.pre_mp4f_handle);
	BF_MP4_Common_Fs_Memory_Update( pCh_Ctrl->video->pre_file_id, FJ_MUX_MEMOPT_DELETE );
	pCh_Ctrl->format_ctrl.pre_mp4f_handle= NULL;
	gMux_Ctrl.audio_ctrl[aud_id].audio_pre_duration[stream_ch] = 0;
	gMux_Ctrl.ch_ctrl[stream_ch].pre_total_duration = 0;
	pCh_Ctrl->video->pre_file_id = 0;

	return MP4_COMMON_OK;
}

