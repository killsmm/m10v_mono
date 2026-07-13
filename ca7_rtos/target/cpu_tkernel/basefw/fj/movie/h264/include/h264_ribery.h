/**
 * @file		h264_ribery.h
 * @brief		H.264 RIBERY function Header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _H264_RIBERY_H_
#define _H264_RIBERY_H_

#include "h264_movie.h"
#include "RB.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Table Number
#define D_H264_RIBERY_VRAW_EMPTY_FIFO_MAX_NUM		RB_VRAW_INFO_TABLE_NUM	// VRAW_INFO_EMPTY_IDX_FIFO table num
#define D_H264_RIBERY_VCPB_EMPTY_FIFO_MAX_NUM		RB_VCPB_INFO_TABLE_NUM	// VCPB_INFO_EMPTY_IDX_FIFO table num

// Decode Use VDPB MAX Number
#define D_H264_RIBERY_VDPB_MAX_NUM					(256)
#define D_H264_RIBERY_VDPB_MIN_NUM					(9)

// Ribery Wait Time
#define D_H264_RIBERY_BOOT_TIMEOUT					(1000)
#define D_H264_RIBERY_START_TIMEOUT					(1000)
#define D_H264_RIBERY_STOP_TIMEOUT					(1000)
#define D_H264_RIBERY_FIFO_TIMEOUT					(100)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Interrupt Pattern to Ribery (request)
typedef enum {
	E_H264_RIBERY_IRQ_REQUEST_ENC_START = 0,		// ENC: encode start request
	E_H264_RIBERY_IRQ_REQUEST_ENC_RESET,			//      reset ready request
	E_H264_RIBERY_IRQ_REQUEST_ENC_VRAW_ENCREQ,		//      VRAW encode request
	E_H264_RIBERY_IRQ_REQUEST_ENC_VSTRM_FREE,		//      VSTRM free request
	
	E_H264_RIBERY_IRQ_REQUEST_ENC_FIFO_VRAW_ENCREQ_SET,
	E_H264_RIBERY_IRQ_REQUEST_ENC_FIFO_VSTRM_FREE_SET,
	E_H264_RIBERY_IRQ_REQUEST_ENC_FIFO_VRAW_FREE_GET,
	E_H264_RIBERY_IRQ_REQUEST_ENC_FIFO_VSTRM_ENCDONE_GET,
	
	E_H264_RIBERY_IRQ_REQUEST_DEC_START,			// DEC: decode start request
	E_H264_RIBERY_IRQ_REQUEST_DEC_RESET,			//      reset ready request
	E_H264_RIBERY_IRQ_REQUEST_DEC_STOP,				//      decode stop request
	E_H264_RIBERY_IRQ_REQUEST_DEC_VCPB_DECREQ,		//      VCPB decode request
	E_H264_RIBERY_IRQ_REQUEST_DEC_VDPB_FREE,		//      VDPB free request
	
	E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VCPB_DECREQ_SET,
	E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VDPB_FREE_SET,
	E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VCPB_FREE_GET,
	E_H264_RIBERY_IRQ_REQUEST_DEC_FIFO_VDPB_DECDONE_GET

} E_H264_RIBERY_IRQ_REQUEST;


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Bridge parameter
typedef struct{
	int						work0_ch;			// Ribery work0 area of SDRAM0/1/2				*RB_SDRAM_CH0/1/2
	int						work1_ch;			// Ribery work1 area of SDRAM0/1/2				*RB_SDRAM_CH0/1/2
	int						work2_ch;			// Ribery work2 area of SDRAM0/1/2				*RB_SDRAM_CH0/1/2
	int						stream_ch;			// Ribery stream area of SDRAM0/1/2				*RB_SDRAM_CH0/1/2
} T_H264_RIBERY_BRIDGE_COMMON_CH;

typedef struct{
	unsigned long			work0_addr;			// Ribery work0 area top address	*1KB(1024Byte)alignment
	unsigned long			work1_addr;			// Ribery work1 area top address	*1KB(1024Byte)alignment
	unsigned long			work2_addr;			// Ribery work2 area top address	*1KB(1024Byte)alignment
	unsigned long			stream_addr;		// Ribery stream area top address	*1KB(1024Byte)alignment
} T_H264_RIBERY_BRIDGE_COMMON_ADDR;

typedef struct{
	ULONG								ch0_top_addr;	// ch0 top addr
	ULONG								ch1_top_addr;	// ch1 top addr
	ULONG								strm_bank;		// stream bank size(1MB align)
	T_H264_RIBERY_BRIDGE_COMMON_CH		common_ch;		// common ch
	T_H264_RIBERY_BRIDGE_COMMON_ADDR	common_addr;	// common addr
} T_H264_RIBERY_BRIDGE_COMMON;

typedef struct{
	int				width;				//                width  size	*128  align
	int				y_bank;				//                y_bank size	*16KB align(tb_flg=ON : y_bank/2 16KB align)
	int				c_bank;				//                c_bank size	*16KB align(tb_flg=ON : c_bank/2 16KB align)
	int				localdec_width;		// localdec       width  size	*128  align
	int				localdec_y_bank;	// localdec       y_bank size	*16KB align(tb_flg=ON : y_bank/2 16KB align)
	int				localdec_c_bank;	// localdec       c_bank size	*16KB align(tb_flg=ON : c_bank/2 16KB align)
	int				reduction_width;	// 1/16 reduction width  size	*128  align
	int				reduction_y_bank;	// 1/16 reduction y_bank size	*16KB align
	int				reduction_c_bank;	// 1/16 reduction c_bank size	*16KB align
} T_H264_RIBERY_BRIDGE_MAP_YC_SIZE;

typedef struct{
	int				y_ch;				//                Y area of SDRAM0/1	*RB_SDRAM_CH0/1
	int				c_ch;				//                C area of SDRAM0/1	*RB_SDRAM_CH0/1
	int				localdec_y_ch;		// local decode   Y area of SDRAM0/1	*RB_SDRAM_CH0/1
	int				localdec_c_ch;		// local decode   C area of SDRAM0/1	*RB_SDRAM_CH0/1
	int				reduction_y_ch;		// 1/16 reduction Y area of SDRAM0/1	*RB_SDRAM_CH0/1
	int				reduction_c_ch;		// 1/16 reduction C area of SDRAM0/1	*RB_SDRAM_CH0/1
} T_H264_RIBERY_BRIDGE_MAP_YC_CH;

typedef struct{
	unsigned long	y_addr;				//                Y area top address	*16KB align
	unsigned long	c_addr;				//                C area top address	*16KB align
	unsigned long	localdec_y_addr;	// local decode   Y area top address	*16KB align
	unsigned long	localdec_c_addr;	// local decode   C area top address	*16KB align
	unsigned long	reduction_y_addr;	// 1/16 reduction Y area top address	*16KB align
	unsigned long	reduction_c_addr;	// 1/16 reduction C area top address	*16KB align
} T_H264_RIBERY_BRIDGE_MAP_YC_ADDR;

typedef struct{
	int									tb_flag;			// Top/Bottom Separator flag	* RB_ON/OFF
	T_H264_RIBERY_BRIDGE_MAP_YC_SIZE	map_yc_size;		// map yc size
	T_H264_RIBERY_BRIDGE_MAP_YC_CH		map_yc_ch;			// map yc ch
	T_H264_RIBERY_BRIDGE_MAP_YC_ADDR	map_yc_addr;		// map yc address
} T_H264_RIBERY_BRIDGE_YC;

typedef struct{
	int		PME_Vector_Read;					// set default RB_SDRAM_IF_CHB
	int		PME_Vector_Write;					// set default RB_SDRAM_IF_CHB
	int		EDGE_Read;							// set default RB_SDRAM_IF_CHB
	int		EDGE_Write;							// set default RB_SDRAM_IF_CHB
	int		UBG_Read;							// set default RB_SDRAM_IF_CHB
	int		UBG_Write;							// set default RB_SDRAM_IF_CHB
	int		Activity_Read;						// set default RB_SDRAM_IF_CHB
	int		Activity_Write;						// set default RB_SDRAM_IF_CHB
	int		Stream0_Read_Acore;					// set default RB_SDRAM_IF_CHB
	int		Stream0_Write_Acore;				// set default RB_SDRAM_IF_CHB
	int		Stream1_Read_Acore;					// set default RB_SDRAM_IF_CHB
	int		Stream1_Write_Acore;				// set default RB_SDRAM_IF_CHB
	int		CME_Vector_Read_Acore;				// set default RB_SDRAM_IF_CHB
	int		CME_Vector_Write_Acore;				// set default RB_SDRAM_IF_CHB
	int		COEF_Read_Acore;					// set default RB_SDRAM_IF_CHB
	int		COEF_Write_Acore;					// set default RB_SDRAM_IF_CHB
	int		Stream0_Read_Bcore;					// set default RB_SDRAM_IF_CHB
	int		Stream0_Write_Bcore;				// set default RB_SDRAM_IF_CHB
	int		Stream1_Read_Bcore;					// set default RB_SDRAM_IF_CHB
	int		Stream1_Write_Bcore;				// set default RB_SDRAM_IF_CHB
	int		CME_Vector_Read_Bcore;				// set default RB_SDRAM_IF_CHB
	int		CME_Vector_Write_Bcore;				// set default RB_SDRAM_IF_CHB
	int		COEF_Read_Bcore;					// set default RB_SDRAM_IF_CHB
	int		COEF_Write_Bcore;					// set default RB_SDRAM_IF_CHB
	int		SEQ_Read;							// set default RB_SDRAM_IF_CHB
	int		SEQ_Write;							// set default RB_SDRAM_IF_CHB
	int		CBUS_Read;							// set default RB_SDRAM_IF_CHB
	int		CBUS_Write;							// set default RB_SDRAM_IF_CHB
	int		CC_1_1_Ref_Picture_Y_Read_Acore;	// set default RB_SDRAM_IF_CHC
	int		CC_1_1_Ref_Picture_C_Read_Acore;	// set default RB_SDRAM_IF_CHC
	int		CC_1_1_Ref_Picture_Y_Write_Acore;	// set default RB_SDRAM_IF_CHA
	int		CC_1_1_Ref_Picture_C_Write_Acore;	// set default RB_SDRAM_IF_CHA
	int		CC_1_1_Org_Picture_Y_Read_Acore;	// set default RB_SDRAM_IF_CHA
	int		CC_1_1_Org_Picture_C_Read_Acore;	// set default RB_SDRAM_IF_CHA
	int		PP_1_1_Org_Picture_Y_Read_Acore;	// set default RB_SDRAM_IF_CHA
	int		PP_1_1_Org_Picture_C_Read_Acore;	// set default RB_SDRAM_IF_CHA
	int		PIPP_Red_Picture_Y_Read_Acore;		// set default RB_SDRAM_IF_CHA
	int		PIPP_Red_Picture_C_Read_Acore;		// set default RB_SDRAM_IF_CHA
	int		PIPP_Red_Picture_Y_Write_Acore;		// set default RB_SDRAM_IF_CHA
	int		PIPP_Red_Picture_C_Write_Acore;		// set default RB_SDRAM_IF_CHA
	int		PME_1_16_Org_Picture_Y_Read_Acore;	// set default RB_SDRAM_IF_CHA
	int		PME_1_16_Org_Picture_C_Read_Acore;	// set default RB_SDRAM_IF_CHA
	int		PME_1_16_Ref_Picture_Y_Read_Acore;	// set default RB_SDRAM_IF_CHA
	int		PME_1_16_Ref_Picture_C_Read_Acore;	// set default RB_SDRAM_IF_CHA
	int		CC_1_1_Ref_Picture_Y_Read_Bcore;	// set default RB_SDRAM_IF_CHC
	int		CC_1_1_Ref_Picture_C_Read_Bcore;	// set default RB_SDRAM_IF_CHC
	int		CC_1_1_Ref_Picture_Y_Write_Bcore;	// set default RB_SDRAM_IF_CHA
	int		CC_1_1_Ref_Picture_C_Write_Bcore;	// set default RB_SDRAM_IF_CHA
	int		CC_1_1_Org_Picture_Y_Read_Bcore;	// set default RB_SDRAM_IF_CHA
	int		CC_1_1_Org_Picture_C_Read_Bcore;	// set default RB_SDRAM_IF_CHA
	int		PP_1_1_Org_Picture_Y_Read_Bcore;	// set default RB_SDRAM_IF_CHA
	int		PP_1_1_Org_Picture_C_Read_Bcore;	// set default RB_SDRAM_IF_CHA
	int		PIPP_Red_Picture_Y_Read_Bcore;		// set default RB_SDRAM_IF_CHA
	int		PIPP_Red_Picture_C_Read_Bcore;		// set default RB_SDRAM_IF_CHA
	int		PIPP_Red_Picture_Y_Write_Bcore;		// set default RB_SDRAM_IF_CHA
	int		PIPP_Red_Picture_C_Write_Bcore;		// set default RB_SDRAM_IF_CHA
	int		PME_1_16_Org_Picture_Y_Read_Bcore;	// set default RB_SDRAM_IF_CHA
	int		PME_1_16_Org_Picture_C_Read_Bcore;	// set default RB_SDRAM_IF_CHA
	int		PME_1_16_Ref_Picture_Y_Read_Bcore;	// set default RB_SDRAM_IF_CHA
	int		PME_1_16_Ref_Picture_C_Read_Bcore;	// set default RB_SDRAM_IF_CHA
} T_H264_RIBERY_BRIDGE_PORT;

// Encode parameter
#ifdef CO_RB_VUI_ON
//  Encode: VUI (Video Usability Information)
typedef struct{
	int					video_signal_type_present_flag;		// video flag					*RB_ON/RB_OFF
	unsigned long		video_format;						//  + video_format				* 3bit data (0~5)
	int					video_full_range_flag;				//  + video_full_range_flag		*RB_ON/RB_OFF
	int					colour_description_present_flag;	//  + color flag				*RB_ON/RB_OFF
	unsigned long		colour_primaries;					//     + colour_primaries		* 8bit data
	unsigned long		transfer_characteristics;			//     + transfer_chara			* 8bit data
	unsigned long		matrix_coefficients;				//     + matrix_coefficients	* 8bit data
	int					aspect_ratio_info_present_flag;		// aspect_ratio_info_present_flag	*RB_ON/RB_OFF
	int					aspect_ratio_idc;					//  + aspect_ratio_idc				* 0		Unspecified
															//									* 1		1:1("square")
															//									* 2		12:11
															//									* 3		10:11
															//									* 4		16:11
															//									* 5		40:33
															//									* 6		24:11
															//									* 7		20:11
															//									* 8		32:11
															//									* 9		80:33
															//									*10		18:11
															//									*11		15:11
															//									*12		64:33
															//									*13		160:99
															//									*14		4:3
															//									*15		3:2
															//									*16		2:1
															//									*17-254	reserved
															//									*255	Extended_SAR
	int					sar_width;							//  + sar_width						*16bit
	int					sar_height;							//  + sar_height					*16bit
	int					chroma_loc_info_present_flag;		// chroma_loc_info_present_flag flag	*RB_ON/RB_OFF
	int					chroma_sample_loc_top_field;		// chroma_sample_loc_top_field flag		*3bit
	int					chroma_sample_loc_bottom_field;		// chroma_sample_loc_bottom_field flag	*3bit
} T_H264_RIBERY_ENC_VUI;
#endif

#ifdef CO_RB_SCALINGLIST_ON
//  Encode: External ScalingList
typedef struct{
	int	ScalingListFlag;							// External ScalingList enable		*RB_ON/OFF
	int	ScalingList4x4_Y1[16];						// Y  ScalingList4x4(1)
	int	ScalingList4x4_Cb1[16];						// Cb ScalingList4x4(1)
	int	ScalingList4x4_Cr1[16];						// Cr ScalingList4x4(1)
	int	ScalingList4x4_Y2[16];						// Y  ScalingList4x4(2)
	int	ScalingList4x4_Cb2[16];						// Cb ScalingList4x4(2)
	int	ScalingList4x4_Cr2[16];						// Cr ScalingList4x4(2)
	int	ScalingList8x8_Y1[64];						// Y  ScalingList8x8(1)
	int	ScalingList8x8_Y2[64];						// Y  ScalingList8x8(2)
} T_H264_RIBERY_ENC_SCALINGLIST;
#endif

#ifdef CO_V_Q_DFVLD_ON
// Encode: DeblockingFilter
typedef struct{
	int	V_Q_DfVld;									// DeblockingFilter enable			*RB_ON/OFF
	int	V_Q_DfAlpha;								// Alpha param(4bit)
	int	V_Q_DfBeta;									// Beta  param(4bit)
} T_H264_RIBERY_ENC_V_Q_DFVLD;
#endif

typedef struct{
	UINT32	size;
	UINT32	rate_mode;
	UINT32	enc_mode;
	ULONG	bitrate;
	ULONG	avebitrate;
	UINT32	frame_rate;
	UINT32	scan_mode;
	UINT32	gop_struct;
	UINT32	cl_gop;
	ULONG	gop_size;
	UINT32	profile;
	UINT32	level;
	UINT32	vlcmode;
	UINT32	insert_pic_struct;
	UINT32	insert_buf_ctrl_param;
	UINT32	insert_recovery_point_sei;
	UINT32	end_of_stream_en;
	UINT32	end_of_sequence_en;
	UINT32	cpb_over_en;
	UINT32	ubg_en;
	UINT32	filler_en;
	UINT32	max_qp;
	UINT32	min_qp;
	UINT32	initial_cpb_removal_delay;
	UINT32	vec_info_en;
	UINT32	frame_crop_top_offset;
	UINT32	frame_crop_bottom_offset;
	UINT32	frame_crop_right_offset;
	UINT32	frame_crop_left_offset;
#ifdef CO_RB_SEARCH_RANGE_ON
	UINT32	search_range_sel;
#endif
#ifdef CO_RB_SLICE_ON
	UINT32	slice;
#endif
#ifdef CO_IMAGE_QUALITY_MODE_ON
	UINT32	image_quality_mode;
	INT32	qp_lambda_offset;
	INT32	inter_intra_weight_ctrl;
#endif
#ifdef CO_MB_LIGHT_MODE
	UINT32	mb_light_mode;
#endif
#ifdef CO_FIXQP_EN_ON
	UINT32	fixqp_en;
#endif
#ifdef CO_RB_VUI_ON
	T_H264_RIBERY_ENC_VUI*			vui;				// Video Usability Information
#endif
#ifdef CO_RB_SCALINGLIST_ON
	T_H264_RIBERY_ENC_SCALINGLIST*	scalinglist;		// External ScalingList
#endif
	UINT32	noirq_mode;
	UINT32	vrawinfo_interval;
	UINT32	vstrminfo_interval;
} T_H264_RIBERY_ENC_PARAM;

// Decode parameter
typedef struct{
	UINT32 size;
	UINT32 frame_rate;
	UINT32 scan_mode;
	UINT32 trick;
	UINT32 keep_idx;
	UINT32 bank_num;
	UINT32 self_rec_play;
} T_H264_RIBERY_DEC_PARAM;

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
} T_H264_RIBERY_DEC_SPS;


/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/
/* Common */
extern	VOID	H264_Ribery_Int_Request( USHORT request );
extern	VOID	H264_Ribery_Int_Request2( USHORT request );
extern	INT32	H264_Ribery_Initialize( ULONG codec, ULONG core_mode, T_H264_RIBERY_BRIDGE_COMMON* bridge_common, T_H264_RIBERY_BRIDGE_PORT* bridge_port );
extern	INT32	H264_Ribery_Finalize( ULONG codec );
extern	VOID	H264_Ribery_Clock_Start( VOID );
extern	VOID	H264_Ribery_Clock_Stop( VOID );
extern	VOID	H264_Ribery_AHclock_Set( UCHAR clk );
extern	INT32	H264_Ribery_Boot( VOID );
extern	INT32	H264_Ribery_Boot2( VOID );

/* Encode */
extern	VOID	H264_Ribery_Enc_InitParam_Set( UINT32 ch, T_H264_RIBERY_ENC_PARAM* param, T_H264_RIBERY_BRIDGE_YC* bridge_yc );
extern	INT32	H264_Ribery_Enc_Start( UINT32 ch );
extern	VOID	H264_Ribery_Enc_Vtable_Init( VOID );
extern	INT32	H264_Ribery_Enc_VRAW_Set( T_RB_FIFO_INDEX index, T_RB_INFOTABLE_VRAW* vraw_info );
extern	INT32	H264_Ribery_Enc_VRAW_Req( UINT32 count, T_RB_FIFO_INDEX* index );
extern	INT32	H264_Ribery_Enc_VRAW_Free( UINT32* free_count, T_RB_FIFO_INDEX* free_index, UINT32* free_bank );
extern	INT32	H264_Ribery_Enc_VSTREAM_Get( UINT32* count, T_RB_FIFO_INDEX* index, T_RB_INFOTABLE_VSTRM* vstrm_info );
extern	INT32	H264_Ribery_Enc_VSTREAM_Free( UINT32 count, T_RB_FIFO_INDEX* index );
extern	INT32	H264_Ribery_Enc_Y_VEC_Get( UINT32* index, T_RB_INFOTABLE_Y_VEC* y_vec_info );
extern	VOID	H264_Ribery_Enc_Vraw_Empty_Init( UINT32 ch, UINT32 table_offset );
extern	INT32	H264_Ribery_Enc_Vraw_Empty_Field_Set( UINT32 ch, UINT32 empty_index );
extern	INT32	H264_Ribery_Enc_Vraw_Empty_Field_Get( UINT32 ch, UINT32* empty_index );

/* Decode */
extern	VOID	H264_Ribery_Dec_InitParam_Set( T_H264_RIBERY_DEC_PARAM* param, T_H264_RIBERY_BRIDGE_YC* bridge_yc );
extern	VOID	H264_Ribery_Dec_InitParam_Set2( T_H264_RIBERY_DEC_PARAM* param, T_H264_RIBERY_BRIDGE_YC* bridge_yc );
extern	INT32	H264_Ribery_Dec_Start( VOID );
extern	INT32	H264_Ribery_Dec_Start2( VOID );
extern	VOID	H264_Ribery_Dec_Vtable_Init( VOID );
extern	VOID	H264_Ribery_Dec_Vtable_Init2( VOID );
extern	INT32	H264_Ribery_Dec_VCPB_Set( UINT32 index, T_RB_INFOTABLE_VCPB* vcpb_info );
extern	INT32	H264_Ribery_Dec_VCPB_Set2( UINT32 index, T_RB_INFOTABLE_VCPB* vcpb_info );
extern	INT32	H264_Ribery_Dec_VCPB_Free( UINT32 *index );
extern	INT32	H264_Ribery_Dec_VCPB_Free2( UINT32 *index );
extern	INT32	H264_Ribery_Dec_VDPB_Get( UINT32* in_index, T_RB_INFOTABLE_VDPB* vdpb_info );
extern	INT32	H264_Ribery_Dec_VDPB_Get2( UINT32* in_index, T_RB_INFOTABLE_VDPB* vdpb_info );
extern	INT32	H264_Ribery_Dec_VDPB_Free( UINT32 index );
extern	INT32	H264_Ribery_Dec_VDPB_Free2( UINT32 index );
extern	VOID	H264_Ribery_Dec_Vcpb_Empty_Init( VOID );
extern	INT32	H264_Ribery_Dec_Vcpb_Empty_Field_Set( UINT32 empty_index );
extern	INT32	H264_Ribery_Dec_Vcpb_Empty_Field_Get( UINT32* empty_index );
extern	VOID	H264_Ribery_Dec_Sps_Analyze( T_H264_RIBERY_DEC_SPS* sps, ULONG sps_addr );
#endif
