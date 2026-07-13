/**
 * @file		h265_wave.h
 * @brief		H.265 WAVE function Header.
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; FUJITSU SEMICONDUCTOR LIMITED 2014</I></B>
 */

#ifndef _H265_WAVE_H_
#define _H265_WAVE_H_

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---

#include "h265_movie.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Y/C or dpb bank size
// ENC
// NHD map
// 960p:1280x960
// 720p:1280x720
// 480p: 848x480
#define D_H265_ENC_MAP_MODE_NHD_NUM			(0)
#define D_H265_ENC_MAP_MODE_NHD_WIDTH		(1280)
#define D_H265_ENC_MAP_MODE_NHD_Y_LINES		(960)
#define D_H265_ENC_MAP_MODE_NHD_C_LINES		(480)
#define D_H265_ENC_MAP_MODE_NHD_Y_BANK		0x0012C000
#define D_H265_ENC_MAP_MODE_NHD_C_BANK		0x00098000

// HD map
// 1.5K(1:1):1504x1504
// 1.4K(4:3):1920x1440
// 1080p:1920x1080
#define D_H265_ENC_MAP_MODE_HD_NUM			(1)
#define D_H265_ENC_MAP_MODE_HD_WIDTH		(1920)
#define D_H265_ENC_MAP_MODE_HD_Y_LINES		(1504)
#define D_H265_ENC_MAP_MODE_HD_C_LINES		(768)
#define D_H265_ENC_MAP_MODE_HD_Y_BANK		0x002D0000
#define D_H265_ENC_MAP_MODE_HD_C_BANK		0x00168000

// 4K2K map
// 4K Cinema:4096x2160
// 4K:3840x2160
// 2.7K(16:9): 2704x1520
// 2.7K(4:3): 2704x2028
// 2.5K(4:3): 2560x1920
#define D_H265_ENC_MAP_MODE_4K2K_NUM		(2)
#define D_H265_ENC_MAP_MODE_4K2K_WIDTH		(4096)
#define D_H265_ENC_MAP_MODE_4K2K_Y_LINES	(2176)
#define D_H265_ENC_MAP_MODE_4K2K_C_LINES	(1088)
#define D_H265_ENC_MAP_MODE_4K2K_Y_BANK		0x00880000
#define D_H265_ENC_MAP_MODE_4K2K_C_BANK		0x00440000

// 3K map
// 3K:3000x3000
#define D_H265_ENC_MAP_MODE_3K_NUM			(3)
#define D_H265_ENC_MAP_MODE_3K_WIDTH		(3072)
#define D_H265_ENC_MAP_MODE_3K_Y_LINES		(3008)
#define D_H265_ENC_MAP_MODE_3K_C_LINES		(1504)
#define D_H265_ENC_MAP_MODE_3K_Y_BANK		0x008d0000
#define D_H265_ENC_MAP_MODE_3K_C_BANK		0x00468000

// 4K3K map
// 4K3K:4000x3000
#define D_H265_ENC_MAP_MODE_4K3K_NUM		(4)
#define D_H265_ENC_MAP_MODE_4K3K_WIDTH		(4096)
#define D_H265_ENC_MAP_MODE_4K3K_Y_LINES	(3008)
#define D_H265_ENC_MAP_MODE_4K3K_C_LINES	(1504)
#define D_H265_ENC_MAP_MODE_4K3K_Y_BANK		0x00BC0000
#define D_H265_ENC_MAP_MODE_4K3K_C_BANK		0x005E0000

// 6K3K map
// 6K:6000x3000
#define D_H265_ENC_MAP_MODE_6K3K_NUM		(5)
#define D_H265_ENC_MAP_MODE_6K3K_WIDTH		(6016)
#define D_H265_ENC_MAP_MODE_6K3K_Y_LINES	(3008)
#define D_H265_ENC_MAP_MODE_6K3K_C_LINES	(1504)
#define D_H265_ENC_MAP_MODE_6K3K_Y_BANK		0x01144000
#define D_H265_ENC_MAP_MODE_6K3K_C_BANK		0x008A4000



// y/c or dpb compression mode
#define D_H265_MAP_COMPRESS_ON			(1)		// for LossLess compression
#define D_H265_MAP_COMPRESS_OFF			(0)		// for throuh

// compression table size
#define D_H265_MAP_SIZE_COMPRESSION_Y	(18*1024)
#define D_H265_MAP_SIZE_COMPRESSION_C	(9*1024)

// support size
// FHD (1920x1080)
#define D_H265_MAP_SIZE_FHD_CODEC_WIDTH				(1920)
#define D_H265_MAP_SIZE_FHD_CODEC_LINES				(1080)

// 1440(1440x1080)
#define D_H265_MAP_SIZE_1440_CODEC_WIDTH			(1440)
#define D_H265_MAP_SIZE_1440_CODEC_LINES			(1080)

// 1440P(1920x1440)
#define D_H265_MAP_SIZE_1440P_CODEC_WIDTH			(1920)
#define D_H265_MAP_SIZE_1440P_CODEC_LINES			(1440)

// HD(1280x720)
#define D_H265_MAP_SIZE_HD_CODEC_WIDTH				(1280)
#define D_H265_MAP_SIZE_HD_CODEC_LINES				(720)

// SD-NTSC(720x480)
#define D_H265_MAP_SIZE_SD_NTSC_CODEC_WIDTH			(720)
#define D_H265_MAP_SIZE_SD_NTSC_CODEC_LINES			(480)

// SD-PAL(720x576)
#define D_H265_MAP_SIZE_SD_PAL_CODEC_WIDTH			(720)
#define D_H265_MAP_SIZE_SD_PAL_CODEC_LINES			(576)

// VGA(640x480)
#define D_H265_MAP_SIZE_VGA_CODEC_WIDTH				(640)
#define D_H265_MAP_SIZE_VGA_CODEC_LINES				(480)

// QVGA(320x240)
#define D_H265_MAP_SIZE_QVGA_CODEC_WIDTH			(320)
#define D_H265_MAP_SIZE_QVGA_CODEC_LINES			(240)

// SXVGA(1280x960)
#define D_H265_MAP_SIZE_SXVGA_CODEC_WIDTH			(1280)
#define D_H265_MAP_SIZE_SXVGA_CODEC_LINES			(960)

// HVGAW(640x360)
#define D_H265_MAP_SIZE_HVGAW_CODEC_WIDTH			(640)
#define D_H265_MAP_SIZE_HVGAW_CODEC_LINES			(360)

// FWVGA(848x480)
#define D_H265_MAP_SIZE_FWVGA_CODEC_WIDTH			(848)
#define D_H265_MAP_SIZE_FWVGA_CODEC_LINES			(480)

// FWQVGA(432x240)
#define D_H265_MAP_SIZE_FWQVGA_CODEC_WIDTH			(432)
#define D_H265_MAP_SIZE_FWQVGA_CODEC_LINES			(240)

// 4K2K(16:9)(4096x2304)
#define D_H265_MAP_SIZE_4K2K_16_9_CODEC_WIDTH		(4096)
#define D_H265_MAP_SIZE_4K2K_16_9_CODEC_LINES		(2304)

// 4K2K(1.90:1)(4096x2160)
#define D_H265_MAP_SIZE_4K2K_1_9_1_CODEC_WIDTH		(4096)
#define D_H265_MAP_SIZE_4K2K_1_9_1_CODEC_LINES		(2160)

// 4K2K(2:1)(4096x2048)
#define D_H265_MAP_SIZE_4K2K_2_1_CODEC_WIDTH		(4096)
#define D_H265_MAP_SIZE_4K2K_2_1_CODEC_LINES		(2048)

// 4K2K(QFHD)(3840x2160)
#define D_H265_MAP_SIZE_4K2K_QFHD_CODEC_WIDTH		(3840)
#define D_H265_MAP_SIZE_4K2K_QFHD_CODEC_LINES		(2160)

// 2.7K(4:3)(2704x2028)
#define D_H265_MAP_SIZE_2_7K_4_3_CODEC_WIDTH		(2704)
#define D_H265_MAP_SIZE_2_7K_4_3_CODEC_LINES		(2028)

// 2.7K(16:9)(2704x1520)
#define D_H265_MAP_SIZE_2_7K_16_9_CODEC_WIDTH		(2704)
#define D_H265_MAP_SIZE_2_7K_16_9_CODEC_LINES		(1520)

// 2.7K(1.90:1)(2880x1524)
#define D_H265_MAP_SIZE_2_7K_1_9_1_CODEC_WIDTH		(2880)
#define D_H265_MAP_SIZE_2_7K_1_9_1_CODEC_LINES		(1524)		// for 1528

#define EXTENDED_SAR			255
#define MAX_VPS_MAX_SUB_LAYERS	16
#define MAX_CPB_COUNT			16
#define MAX_NUM_DECODING_UNIT	20
#define MAX_NUM_SEQ_ID			20

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	BUFFERING_PERIOD						= 0,
	PICTURE_TIMING,							// = 1
	FILLER_PAYLOAD							= 3,
	USER_DATA_REGISTERED_ITU_T_T35,			// = 4
	USER_DATA_UNREGISTERED,					// = 5
	RECOVERY_POINT,							// = 6
	SCENE_INFO								= 9,
	FULL_FRAME_SNAPSHOT						= 15,
	PROGRESSIVE_REFINEMENT_SEGMENT_START,	// = 16
	PROGRESSIVE_REFINEMENT_SEGMENT_END,		// = 17
	FILM_GRAIN_CHARACTERISTICS				= 19,
	POST_FILTER_HINT						= 22,
	TONE_MAPPING_INFO,						// = 23
	KNEE_FUNCTION_INFO,						// = 24
	FRAME_PACKING							= 45,
	DISPLAY_ORIENTATION						= 47,
	SOP_DESCRIPTION,						// = 48
	ACTIVE_PARAMETER_SETS					= 129,
	DECODING_UNIT_INFO,						// = 130
	TEMPORAL_LEVEL0_INDEX,					// = 131
	DECODED_PICTURE_HASH,					// = 132
	SCALABLE_NESTING						= 133,
	REGION_REFRESH_INFO,					// = 134
	NO_DISPLAY,								// = 135
	TIME_CODE,								// = 136
	MASTERING_DISPLAY_COLOUR_VOLUME,		// = 137
	SEGM_RECT_FRAME_PACKING,				// = 138
	TEMP_MOTION_CONSTRAINED_TILE_SETS,		// = 139
	CHROMA_SAMPLING_FILTER_HINT,			// = 140
} SeiPayloadType;

enum {
	SNT_TRAIL_N         = 0,
	SNT_TRAIL_R         = 1,
	SNT_TSA_N           = 2,
	SNT_TSA_R           = 3,
	SNT_STSA_N          = 4,
	SNT_STSA_R          = 5,
	SNT_RADL_N          = 6,
	SNT_RADL_R          = 7,
	SNT_RASL_N          = 8,
	SNT_RASL_R          = 9,
	SNT_RSV_N10         = 10,
	SNT_RSV_R11         = 11,
	SNT_RSV_N12         = 12,
	SNT_RSV_R13         = 13,
	SNT_RSV_N14         = 14,
	SNT_RSV_R15         = 15,
	SNT_BLA_W_LP        = 16,
	SNT_BLA_W_RADL      = 17,
	SNT_BLA_N_LP        = 18,
	SNT_IDR_W_RADL      = 19,
	SNT_IDR_N_LP        = 20,
	SNT_CRA             = 21,
	SNT_RSV_IRAP22      = 22,
	SNT_RSV_IRAP23      = 23,
	SNT_VPS             = 32,
	SNT_SPS             = 33,
	SNT_PPS             = 34,
	SNT_AUD             = 35,
	SNT_EOS             = 36,
	SNT_EOB             = 37,
	SNT_FD              = 38,
	SNT_PREFIX_SEI      = 39,
	SNT_SUFFIX_SEI      = 40,
};

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct{
	UINT32		addr_ch0;
	UINT32		size_ch0;
	UINT32		addr_ch1;
	UINT32		size_ch1;
} T_H265_WAVE_CODE;

// Queue
typedef struct {
	UCHAR*	buffer;
    UINT32	size;
    UINT32	itemSize;
    UINT32	count;
    UINT32	front;
    UINT32	rear;
	UINT32	lock;	// sem id
//    VpuMutex	lock;
} T_H265_WAVE_QUEUE;

typedef struct sei_buffering_period_tag {
	UINT32	sub_pic_hrd_params_present_flag;	//derived from syntax elements found in the hrd_parameters( )
	UINT32	au_cpb_removal_delay_length_minus1; //derived from syntax elements found in the hrd_parameters( )
	UINT32	initial_cpb_removal_delay_length_minus1; //derived from syntax elements found in the hrd_parameters( )
	UINT32	CpbCnt;	//CpbCnt are derived from syntax elements found in the sub_layer_hrd_parameters( ) syntax structure
	UINT32	NalHrdBpPresentFlag;		// NalHrdBpPresentFlag and VclHrdBpPresentFlag are found in or derived from syntax elements found in the hrd_parameters( )
	UINT32	VclHrdBpPresentFlag;		//NalHrdBpPresentFlag and VclHrdBpPresentFlag are found in or derived from syntax elements found in the hrd_parameters( )

	UINT32	bp_seq_parameter_set_id; 	
	UINT32	irap_cpb_params_present_flag;	
	UINT32	cpb_delay_offset;					//The syntax element has a length in bits given by au_cpb_removal_delay_length_minus1 + 1
	UINT32	dpb_delay_offset;
	UINT32	concatenation_flag;
	UINT32	au_cpb_removal_delay_delta_minus1;
	UINT32	nal_initial_cpb_removal_delay[MAX_CPB_COUNT];
	UINT32	nal_initial_cpb_removal_offset[MAX_CPB_COUNT];
	UINT32	nal_initial_alt_cpb_removal_delay[MAX_CPB_COUNT];
	UINT32	nal_initial_alt_cpb_removal_offset[MAX_CPB_COUNT];

	UINT32	vcl_initial_cpb_removal_delay[MAX_CPB_COUNT];
	UINT32	vcl_initial_cpb_removal_offset[MAX_CPB_COUNT];
	UINT32	vcl_initial_alt_cpb_removal_delay[MAX_CPB_COUNT];
	UINT32	vcl_initial_alt_cpb_removal_offset[MAX_CPB_COUNT];
} sei_buffering_period_t;

typedef struct sei_pic_timing_tag {
	UINT32	duplicate_flag; 
	UINT32	source_scan_type;
	UINT32	pic_struct;

	UINT32	CpbDpbDelaysPresentFlag;
	UINT32	au_cpb_removal_delay_minus1;
	UINT32	pic_dpb_output_delay;
	UINT32	pic_dpb_output_du_delay;
	
	UINT32	num_decoding_units_minus1;
	UINT32	du_common_cpb_removal_delay_flag;
	UINT32	du_common_cpb_removal_delay_increment_minus1;
	UINT32	num_nalus_in_du_minus1[MAX_NUM_DECODING_UNIT];
	UINT32	du_cpb_removal_delay_increment_minus1[MAX_NUM_DECODING_UNIT];
} sei_pic_timing_t;

typedef struct sei_active_parameter_tag {
	UINT32	active_video_parameter_set_id; 
	UINT32	self_contained_cvs_flag;
	UINT32	no_parameter_set_update_flag;
	UINT32	num_sps_ids_minus1;
	UINT32	active_seq_parameter_set_id[MAX_NUM_SEQ_ID];
} sei_active_parameter_t;


typedef struct {
	UINT32	addr;
	UINT32	size;
    sei_buffering_period_t	sbp;
} T_H265_WAVE_SEI_BUFFERING_PERIOD;


typedef struct {
	UINT32	addr;
	UINT32	size;
    sei_pic_timing_t	spt;
} T_H265_WAVE_SEI_PIC_TIMING;


typedef struct {
	UINT32	addr;
	UINT32	size;
    sei_active_parameter_t	sap;
} T_H265_WAVE_SEI_ACTIVE_PARAMETER;


typedef struct
{
	UINT32	vui_parameters_presesent_flag;

	// vui_present_flags
	UINT32	neutral_chroma_indication_flag;
	UINT32	field_seq_flag;		// this syntax shall be 1 when pic_struct in sei_pic_timing is 1 or 2
	UINT32	frame_field_info_present_flag;	//this syntax shall be 1 when pic_struct in sei_pic_timing is not 0
	UINT32	aspect_ratio_info_present_flag;
	UINT32	overscan_info_present_flag;
	UINT32	video_signal_type_present_flag;
	UINT32	colour_description_present_flag;
	UINT32	chroma_loc_info_present_flag;
	UINT32	default_display_window_flag;
	UINT32	vui_timing_info_present_flag;
	UINT32	bitstream_restriction_flag;

	// vui_aspect_ratio_idc
	UINT32	aspect_ratio_idc;

	// vui_sar_size
	UINT32	sar_width;
	UINT32	sar_height;

	// vui_overscan_appropriate
	UINT32	overscan_appropriate_flag;

	// vui_video_signal
	UINT32	video_format;
	UINT32	video_full_range_flag;
	UINT32	colour_primaries;
	UINT32	transfer_characteristics;
	UINT32	matrix_coeffs;

	// vui_chroma_sample_loc
	UINT32	chroma_sample_loc_type_top_field;
	UINT32	chroma_sample_loc_type_bottom_field;

	// vui_disp_win_left_right
	UINT32	def_disp_win_left_offset;
	UINT32	def_disp_win_right_offset;

	// vui_disp_win_top_bot
	UINT32	def_disp_win_top_offset;
	UINT32	def_disp_win_bottom_offset;

	UINT32	vui_hrd_parameters_present_flag;


	UINT32	vui_num_units_in_tick;
	UINT32	vui_time_scale;
	UINT32	vui_poc_proportional_to_timing_flag;
	UINT32	vui_num_ticks_poc_diff_one_minus1;

	UINT32	tiles_fixed_structure_flag;
	UINT32	motion_vectors_over_pic_boundaries_flag;
	UINT32	restricted_ref_pic_lists_flag;
	UINT32	min_spatial_segmentation_idc;
	UINT32	max_bytes_per_pic_denom;
	UINT32	max_bits_per_min_cu_denom;
	UINT32	log2_max_mv_length_horizontal;
	UINT32	log2_max_mv_length_vertical;
} vui_t;


typedef struct 
{
	UINT32	encodeRbspHrdInVui;
	UINT32	encodeRbspHrdInVps;

	UINT32	commonInfPresentFlag;
	UINT32	vps_max_sub_layers_minus1; // this parameter is from VPS

	UINT32	nal_hrd_parameters_present_flag;
	UINT32	vcl_hrd_parameters_present_flag;
	UINT32	sub_pic_hrd_params_present_flag;

	UINT32	tick_divisor_minus2;
	UINT32	du_cpb_removal_delay_increment_length_minus1;
	UINT32	sub_pic_cpb_params_in_pic_timing_sei_flag;
	UINT32	dpb_output_delay_du_length_minus1;

	UINT32	bit_rate_scale;
	UINT32	cpb_size_scale;

	UINT32	cpb_size_du_scale;
	UINT32	initial_cpb_removal_delay_length_minus1;
	UINT32	au_cpb_removal_delay_length_minus1;
	UINT32	dpb_output_delay_length_minus1;

	UINT32	fixed_pic_rate_general_flag[MAX_VPS_MAX_SUB_LAYERS];
	UINT32	fixed_pic_rate_within_cvs_flag[MAX_VPS_MAX_SUB_LAYERS];
	UINT32	elemental_duration_in_tc_minus1[MAX_VPS_MAX_SUB_LAYERS];
	UINT32	low_delay_hrd_flag[MAX_VPS_MAX_SUB_LAYERS];
	UINT32	cpb_cnt_minus1[MAX_VPS_MAX_SUB_LAYERS];

	//sub_layer_hrd_parameters(

	UINT32	bit_rate_value_minus1[MAX_CPB_COUNT][MAX_VPS_MAX_SUB_LAYERS];
	UINT32	cpb_size_value_minus1[MAX_CPB_COUNT][MAX_VPS_MAX_SUB_LAYERS];
	UINT32	cpb_size_du_value_minus1[MAX_CPB_COUNT][MAX_VPS_MAX_SUB_LAYERS];
	UINT32	bit_rate_du_value_minus1[MAX_CPB_COUNT][MAX_VPS_MAX_SUB_LAYERS];
	UINT32	cbr_flag[MAX_CPB_COUNT][MAX_VPS_MAX_SUB_LAYERS];	

} hrd_t;

/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
VOID H265_Wave_Clock_Start( INT32 clock );
VOID H265_Wave_Clock_Stop( VOID );

INT32 H265_Wave_Initialize( UINT32 mode, UINT32 clock, T_H265_WAVE_CODE code );
VOID  H265_Wave_Finalize( VOID );

INT32 H265_Wave_SetDDRChannel( DDRChannelParam ddrCh );
INT32 H265_Wave_EncOpen( UINT32 ch, EncHandle* handle, EncOpenParam* encOpParam );
INT32 H265_Wave_EncClose( UINT32 ch, EncHandle handle );
INT32 H265_Wave_EncGetInitialInfo( UINT32 ch, EncHandle handle, EncInitialInfo* encInitInfo );
INT32 H265_Wave_EncRegisterFrameBuffer( UINT32 ch, EncHandle handle, FrameBuffer* bufArray, int num, int stride, int height, int mapType );
INT32 H265_Wave_EncAllocateFrameBuffer( UINT32 ch, EncHandle handle, FrameBufferAllocInfo info, FrameBuffer *frameBuffer );
INT32 H265_Wave_EncGiveCommand( UINT32 ch, EncHandle handle, CodecCommand cmd, void * parameter );
INT32 H265_Wave_EncStartOneFrame( UINT32 ch, EncHandle handle, EncParam * param );
INT32 H265_Wave_WaitInterrupt( UINT32 ch, Uint32 coreIdx, int timeout );
VOID H265_Wave_ClearInterrupt( UINT32 ch, Uint32 coreIdx );
INT32 H265_Wave_EncGetOutputInfo( UINT32 ch, EncHandle handle, EncOutputInfo * info );
INT32 H265_Wave_EncGetBitstreamBuffer(EncHandle handle, UINT32 *prdPrt, UINT32 *pwrPtr, UINT32 * size);


T_H265_WAVE_QUEUE* H265_Wave_Queue_Create( UINT32 itemCount, UINT32 itemSize );
VOID H265_Wave_Queue_Destroy( T_H265_WAVE_QUEUE* queue );
BOOL H265_Wave_Queue_Enqueue( T_H265_WAVE_QUEUE* queue, VOID* data );
VOID* H265_Wave_Queue_Dequeue( T_H265_WAVE_QUEUE* queue );
UINT32 H265_Wave_Queue_Get_Cnt( T_H265_WAVE_QUEUE* queue );


INT32 H265_Wave_activeParameterSEIEncode(sei_active_parameter_t *sap, hrd_t *hrd, UINT8 *pBuffer);
INT32 H265_Wave_picTimingSEIEncode(sei_pic_timing_t *spt, hrd_t *hrd, UINT8 *pBuffer);
INT32 H265_Wave_bufferingPeriodSeiEncode(sei_buffering_period_t *sbp, hrd_t *hrd, UINT8 *pBuffer);
INT32 H265_Wave_EncodePrefixSEI(BOOL on_pictstruct_flag, T_H265_WAVE_SEI_ACTIVE_PARAMETER *sap, T_H265_WAVE_SEI_PIC_TIMING *spt, BOOL on_buffer_ctrl_flag, T_H265_WAVE_SEI_BUFFERING_PERIOD *sbp, hrd_t *hrd, UINT8 *pBuffer);
BOOL H265_Wave_EncodeHRDInVUI(hrd_t *hrd, vui_t *vui, UINT8 *pBuffer, INT32 *pByteSize, INT32 *pBitSize, double dframeRate);
VOID H265_Wave_VmemSize_Calculation( UINT32 ch, EncHandle handle, EncInitialInfo* encInitInfo );
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---

#endif
