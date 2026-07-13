/**
 * @file		h265_encode.h
 * @brief		H.265 Encode function Header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _H265_ENCODE_H_
#define _H265_ENCODE_H_

#include "movie_common.h"
#include "h265_movie.h"
#include "h265_sirius.h"
#include "sdram_map_movie_record.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Recording Status
#define H265_ENC_STOP_END				(0)
#define H265_ENC_RECORDING				(1)

// Vin Address Control Result
#define H265_ENC_VIN_CTL_LASTDATA		(-1)
#define H265_ENC_VIN_CTL_NONEDATA		(-2)

// Vin Idx Control
#define H265_ENC_VIN_IDX_NODATA			(-1)

// semaphore wait time
#define H265_ENC_SEMAPHORE_TIMEOUT		(300)
// Number of ENC Stream Fifo
#define D_H265_ENC_STREAM_FIFO_COUNT	(192)

#define D_H265_ENCODE_VINFO_FIFO_MAX		(20)
#define D_H265_ENCODE_DECIDE_BANK_FIFO_MAX	(3)

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#define D_H265_ENCODE_NAL_MAX_SIZE	(18)
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---


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
} T_H265_ENC_VIDEO_CTRL;

typedef struct {
	UCHAR					rec_status;									// Recording Status
																		// 	H265_ENC_STOP_END(0)   : Normal Stop record
																		// 	H265_ENC_RECORDING(1)  : Recording status
	BYTE					task_result;								// Result of Task Control Process
																		// 	(Error Code)
	BYTE					task_status;								// Task status
	T_H265_ENC_VIDEO_CTRL	video[E_H265_IF_STREAM_CH_MAX];				// Video Control
} T_H265_ENC_CTL;

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
// vin address structure
typedef struct {
	UINT32					max_bank_num;
	UINT32					free_bank_tbl[D_SIRIUS_YC_BANK_MAX];
	UINT32					write_point;
	UINT32					read_point;
	UINT32					decide_bank[D_SIRIUS_YC_BANK_MAX];
	ULLONG					decide_frame_no[D_SIRIUS_YC_BANK_MAX];
	UINT32					decide_write_point;
	UINT32					decide_read_point;
	UINT32					hold_max_no;
	INT32					next_decide_bank[D_H265_ENCODE_DECIDE_BANK_FIFO_MAX];
	ULLONG					next_decide_frame_no[D_H265_ENCODE_DECIDE_BANK_FIFO_MAX];
	UINT32					next_decide_write_point;
	UINT32					next_decide_read_point;
	UINT32					duplicate_check_tbl[D_SIRIUS_YC_BANK_MAX];
} T_H265_ENC_VIN_ADDRESS_CONTROL;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
typedef struct {
	UINT32					max_bank_num;
	UINT32					free_bank_tbl[D_WAVE_YC_BANK_MAX];
	UINT32					write_point;
	UINT32					read_point;
	UINT32					decide_bank[D_WAVE_YC_BANK_MAX];
	ULLONG					decide_frame_no[D_WAVE_YC_BANK_MAX];
	UINT32					decide_write_point;
	UINT32					decide_read_point;
	UINT32					hold_max_no;
	INT32					next_decide_bank[D_H265_ENCODE_DECIDE_BANK_FIFO_MAX];
	ULLONG					next_decide_frame_no[D_H265_ENCODE_DECIDE_BANK_FIFO_MAX];
	UINT32					next_decide_write_point;
	UINT32					next_decide_read_point;
	UINT32					duplicate_check_tbl[D_WAVE_YC_BANK_MAX];
} T_H265_ENC_VIN_ADDRESS_CONTROL;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---

// --- Visual Stream informations (RECORD/VIDEO) ---
typedef struct{
	UINT32						vstream_index;								// Vstream Index
	ULLONG						frame_no;									// FrameNo (VRAW Serial Number)
	ULLONG						vstrem_serial_num;							// VSTRM Serial Number
	ULONG						frame_count_in_gop;							// Frame Count In GOP
	UINT32						sample_address;								// Sample address
	UINT32						sample_size;								// Sample size
	UINT32						sample_type;								// Sample type (I,B,P)
	UINT32						bitrate;									// Bitrate
	UINT32						ave_bitrate;								// Averate Bitrate
	UINT32						nal_unit_num;								// NUL unit Number
	ULONG						nal_unit_size[H265_NAL_UNIT_MAX_NUM];		// NAL unit size
	LLONG						sample_pts;									// Sample PTS
	LLONG						sample_dts;									// Sample DTS
	UINT32						cpb_over_flag;								// CPB Buffer Over flag
	LONG						poc;										// POC
	UCHAR						stream_end_flg;								// Stream End Flag
} T_H265_ENC_VIDEO_INFO;

// Stream Free Control
typedef struct {
	UINT8	use_flg;
	UINT32	ch;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	UINT32	free_size;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	UINT32	free_buf_index;
	UINT32	free_addr;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
} T_H265_ENC_STREAM_FREE_CONTROL;

// FIFO Control
typedef struct {
	UINT32	r_point;
	UINT32	w_point;
	UINT32	fifo_over_flg;
} T_H265_ENC_VSTRM_FIFO_CTRL;

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
typedef struct {
	UINT32		header_addr;
	UINT32		header_size;
	UINT32		AUD_NAL_size;
	UINT32		VPS_NAL_size;
	UINT32		SPS_NAL_size;
	UINT32		PPS_NAL_size;
}T_H265_ENC_HEADER_INFO;

typedef struct {
	UINT32		NAL_size[D_H265_ENCODE_NAL_MAX_SIZE];
}T_H265_ENC_NAL_SIZE;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---

// Slice Info
typedef struct {
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
	UINT32	nal_unit_size[SR_NAL_UNIT_SIZE_TBL_NUM];
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	/*******************  ES3_Porting **********************/ 
	UINT32	nal_unit_size[32];	//dummy
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
} T_H265_ENC_SLICE_INFO;

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
extern	INT32	H265_Encode_Main_Ready( E_H265_IF_STREAM_CH stream_ch, T_H265_MOVIE_UI* record_info );
extern	INT32	H265_Encode_Main_Check_End( T_H265_MOVIE_UI* record_info );
extern	VOID	H265_Encode_Main_End( E_H265_IF_STREAM_CH stream_ch, T_H265_MOVIE_UI* record_info );
extern	VOID	H265_Encode_ENT_Main( VOID );
// --- REMOVE_ES1_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
extern	VOID	H265_Encode_ENT_INT_Main( VOID );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE END ---
extern	VOID	H265_Encode_CC_Main( VOID );
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
extern	VOID	H265_Encode_CC_2_Main( VOID );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---
#ifdef CO_H265_ENC_DEBUG_VIEW_DUMMY
extern	VOID	H265_Encode_DISP_Main( VOID );
#endif
extern	INT32	H265_Encode_Free_Video_Index( E_H265_IF_STREAM_CH stream_ch, UINT32 free_vstream_index, BOOL last_free_flg );
extern	VOID	H265_Encode_Sync_Interrupt_Count_Increment( E_H265_IF_STREAM_CH stream_ch );

extern	INT32	H265_Encode_Initialize( E_H265_IF_STREAM_CH stream_ch, T_H265_MOVIE_UI* record_info );
extern	VOID	H265_Encode_Finalize( VOID );
extern	INT32	H265_Encode_Image_Address_Get( E_H265_IF_STREAM_CH stream_ch, E_H265_IF_IMAGE_SIZE size, ULLONG frame_no, T_H265_IF_GET_IMAGE_ADDR_PARAM* addr_param );
extern	VOID	H265_Encode_Image_Address_Decide( E_H265_IF_STREAM_CH stream_ch, ULLONG frame_no, BOOL dummy_flg );
extern	VOID	H265_Encode_Frame_Enabled( E_H265_IF_STREAM_CH stream_ch, ULLONG frame_no, BOOL dummy_flg, BOOL enabled );


extern	VOID	H265_Encode_Vin_Address_Valid( E_H265_IF_STREAM_CH stream_ch );
extern	VOID	H265_Encode_Dynamic_Parameter_Set( E_H265_IF_STREAM_CH stream_ch, T_H265_IF_DYNAMIC_PARAM_RECORD* dynamic_param );
extern	VOID	H265_Encode_Insert_IDR_Trriger_Set( E_H265_IF_STREAM_CH stream_ch );

#endif
