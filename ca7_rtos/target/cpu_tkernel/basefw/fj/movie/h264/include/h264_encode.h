/**
 * @file		h264_encode.h
 * @brief		H.264 Encode function Header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _H264_ENCODE_H_
#define _H264_ENCODE_H_

#include "movie_common.h"
#include "h264_movie.h"
#include "h264_ribery.h"
#include "sdram_map_movie_record.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Recording Status
#define H264_ENC_STOP_END				(0)
#define H264_ENC_RECORDING				(1)

// Vin Address Control Result
#define H264_ENC_VIN_CTL_LASTDATA		(-1)
#define H264_ENC_VIN_CTL_NONEDATA		(-2)

// Vin Idx Control
#define H264_ENC_VIN_IDX_NODATA			(-1)

// semaphore wait time
#define H264_ENC_SEMAPHORE_TIMEOUT		(300)
// Number of ENC Request timing
#define D_H264_ENC_FIFO_REQ_COUNT		(6)		// MAX = RB_FIFO_MAX_COUNT_NUM - 1

#define D_H264_ENCODE_DECIDE_BANK_FIFO_MAX	(4)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// --- Record Control Informations ---
// VIDEO
typedef struct {
	ULONG					time_scale;									// Video Time Scale
	ULLONG					time_increment;								// Video Time Increment
	ULLONG					total_time_increment;						// Total Time Increment
	DOUBLE					coefficient;								// Video Time Coefficient (for Vstream)
} T_H264_ENC_VIDEO_CTRL;

typedef struct {
	UCHAR					rec_status;									// Recording Status
																		// 	H264_ENC_STOP_END(0)   : Normal Stop record
																		// 	H264_ENC_RECORDING(1)  : Recording status
	BYTE					task_result;								// Result of Task Control Process
																		// 	(Error Code)
	BYTE					task_status;								// Task status
	UCHAR					vector_status;								// Vector Task Status
	T_H264_ENC_VIDEO_CTRL	video[E_H264_IF_STREAM_CH_MAX];				// Video Control
} T_H264_ENC_CTL;

// vin address structure
typedef struct {
	UINT32					free_bank_tbl[RB_MAP_BNK_ENC_YCC];
	UINT32					write_point;
	UINT32					read_point;
	UINT32					decide_bank[RB_MAP_BNK_ENC_YCC];
	ULLONG					decide_frame_no[RB_MAP_BNK_ENC_YCC];
	UINT32					decide_write_point;
	UINT32					decide_read_point;
	UINT32					hold_max_no;
	INT32					next_decide_bank[D_H264_ENCODE_DECIDE_BANK_FIFO_MAX];
	ULLONG					next_decide_frame_no[D_H264_ENCODE_DECIDE_BANK_FIFO_MAX];
	UINT32					next_decide_write_point;
	UINT32					next_decide_read_point;
	UINT32					duplicate_check_tbl[RB_MAP_BNK_ENC_YCC];
} T_H264_ENC_VIN_ADDRESS_CONTROL;

typedef struct{
	UCHAR*						sample_address;								// Sample address
	UINT32						sample_size;								// Sample size
	UINT32						sample_type;								// Sample type (I,B,P)
	UINT32						bitrate;									// Bitrate
	UINT32						VBV_buffer;									// VBV buffer
	UINT32						nal_unit_num;								// NUL unit Number
	ULONG						nal_unit_size[H264_NAL_UNIT_MAX_NUM];		// NAL unit size
	LLONG						sample_pts;									// Sample PTS
	LLONG						sample_dts;									// Sample DTS
	UINT32						ave_qp;										// Average QP
	UINT32						cpb_over_flag;								// CPB Buffer Over flag
} T_H264_ENC_SAMPLE_PARAM;

// --- Visual Stream informations (RECORD/VIDEO) ---
typedef struct{
	UINT32						sample_num;															// Sample Number
	ULONG						stream_ch[D_H264_ENC_FIFO_REQ_COUNT];								// Stream ch
	ULLONG						frame_no[D_H264_ENC_FIFO_REQ_COUNT];								// FrameNo (VRAW Serial Number)
	ULLONG						vstrem_serial_num[D_H264_ENC_FIFO_REQ_COUNT];						// VSTRM Serial Number
	ULONG						frame_count_in_gop[D_H264_ENC_FIFO_REQ_COUNT];						// Frame Count In GOP
	LONG						poc[D_H264_ENC_FIFO_REQ_COUNT];										// POC
	T_H264_ENC_SAMPLE_PARAM		first_param[D_H264_ENC_FIFO_REQ_COUNT];								// First Sample Info
	T_H264_ENC_SAMPLE_PARAM		second_param[D_H264_ENC_FIFO_REQ_COUNT];							// Second Sample Info (Only Interlace)
	UCHAR						stream_end_flg[D_H264_ENC_FIFO_REQ_COUNT];							// Stream End Flag
	UINT32						vstream_free_index[D_H264_ENC_FIFO_REQ_COUNT];						// Vstream Free Index
	UINT32						ave_bitrate[D_H264_ENC_FIFO_REQ_COUNT];								// Averate Bitrate
	ULONG						stream_buffer0_limit[D_H264_ENC_FIFO_REQ_COUNT];					// StreamBuffer0 Limit Address
	ULONG						stream_buffer1_limit[D_H264_ENC_FIFO_REQ_COUNT];					// StreamBuffer1 Limit Address
} T_H264_ENC_VIDEO_INFO;

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
extern	INT32	H264_Encode_Main_Ready( E_H264_IF_STREAM_CH stream_ch, T_H264_MOVIE_UI* record_info );
extern	INT32	H264_Encode_Main_Check_End( T_H264_MOVIE_UI* record_info );
extern	VOID	H264_Encode_Main_End( E_H264_IF_STREAM_CH stream_ch, T_H264_MOVIE_UI* record_info );
extern	VOID	H264_Encode_ENT_Main( VOID );
extern	VOID	H264_Encode_CC_Main( VOID );
extern	VOID	H264_Encode_VECTOR_Main( VOID )	;
#ifdef CO_H264_ENC_DEBUG_VIEW_DUMMY
extern	VOID	H264_Encode_DISP_Main( VOID );
#endif
extern	INT32	H264_Encode_Free_Video_Index( E_H264_IF_STREAM_CH stream_ch, UINT32 free_vstream_index, BOOL last_free_flg );

extern	INT32	H264_Encode_Initialize( E_H264_IF_STREAM_CH stream_ch, T_H264_MOVIE_UI* record_info );
extern	VOID	H264_Encode_Finalize( VOID );
extern	INT32	H264_Encode_Image_Address_Get( E_H264_IF_STREAM_CH stream_ch, E_H264_IF_IMAGE_SIZE size, ULLONG frame_no, T_H264_IF_GET_IMAGE_ADDR_PARAM* addr_param );
extern	VOID	H264_Encode_Image_Address_Decide( E_H264_IF_STREAM_CH stream_ch, ULLONG frame_no, BOOL dummy_flg );
extern	VOID	H264_Encode_Frame_Enabled( E_H264_IF_STREAM_CH stream_ch, ULLONG frame_no, BOOL dummy_flg, BOOL enabled );


extern	VOID	H264_Encode_Vin_Address_Valid( E_H264_IF_STREAM_CH stream_ch );
extern	VOID	H264_Encode_Dynamic_Parameter_Set( E_H264_IF_STREAM_CH stream_ch, T_H264_IF_DYNAMIC_PARAM_RECORD* dynamic_param );
extern	VOID	H264_Encode_Insert_IDR_Trriger_Set( E_H264_IF_STREAM_CH stream_ch );

#endif
