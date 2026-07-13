/**
 * @file		sdram_map_movie_record.h
 * @brief		SocioNext SDRAM Movie (Movie Record) Header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _SDRAM_MAP_MOVIE_RECORD_ES3_H_
#define _SDRAM_MAP_MOVIE_RECORD_ES3_H_

#include "sdram_map_common.h"
#include "sdram_map_movie_record_pattern0_define.h"
#include "sdram_map_movie_record_pattern_stitch_define.h"
#include "h264_if.h"
#include "h265_if.h"

#ifdef CO_MOVIE_PORTING_BLOCK
	// SDRAM_MAP is using the excel tool. When a TOP address is changed, Re-create SDRAM_MAP
#endif

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define SDRAM_ADR_REL2ABS(relative_addr)	(SDRAM_ADR_TOP|(ULONG)(relative_addr))
#define SDRAM_BNK_LIMIT(pos, total) ((pos < total) ? pos : 0)
#define RIBERY_OFFSET(addr)		(((addr & 0x000003FF) == 0) ? addr : ((addr & ~(0x000003FF)) + 0x400))	// 1k(1024byte) alignment

#define	D_RIBERY_REDUCTION_BANK_MAX			SDRAM_BNK_MOVIE_RECORD0_RIBERY_YC_REDUCTION
#define	D_RIBERY_LOCALDEC_BANK_MAX			2
#define	D_RIBERY_YC_BANK_MAX				SDRAM_BNK_MOVIE_RECORD0_RIBERY_YC
#define	D_RIBERY_WIDTH_ALIGNMENT			256
#define	D_RIBERY_LINES_ALIGNMENT			32
#define	D_RIBERY_BANK_ALIGNMENT				16384

#define	D_WAVE_CMPRS_BANK_MAX				SDRAM_BNK_MOVIE_RECORD0_WAVE_0_CMPRS
#define	D_WAVE_LOCALDEC_BANK_MAX			SDRAM_BNK_MOVIE_RECORD0_WAVE_0_LOCALDEC_C
#define	D_WAVE_YC_BANK_MAX					SDRAM_BNK_MOVIE_RECORD0_WAVE_0_Y
#define	D_WAVE_STREAM_BANK_MAX				32
#define	D_WAVE_WIDTH_ALIGNMENT				256
#define	D_WAVE_LINES_ALIGNMENT				64
#define	D_WAVE_LOCALDEC_WIDTH_ALIGNMENT		16
#define	D_WAVE_LOCALDEC_LINES_ALIGNMENT		32
#define	D_WAVE_BANK_ALIGNMENT				16384
#define	D_WAVE_CMPRS_Y_SIZE					18432	// 18Kbyte
#define	D_WAVE_CMPRS_C_SIZE					9216	// 9byte



/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	UINT32					y;
	UINT32					c;
} T_SDRAM_MAP_MOVIE_RECORD_YC;

typedef struct {
	BYTE					max_bank;
	USHORT					g_width;
	UINT32					y_bank;
	UINT32					c_bank;
	UINT32					y_addr;
	UINT32					c_addr;
} T_SDRAM_MAP_MOVIE_RECORD_RIBERY_REDUCTION;

typedef struct {
	BYTE					max_bank;
	USHORT					g_width;
	UINT32					y_bank;
	UINT32					c_bank;
	UINT32					y_addr;
	UINT32					c_addr;
} T_SDRAM_MAP_MOVIE_RECORD_RIBERY_LOCALDEC;

typedef struct {
	BYTE							index;
	BYTE							max_bank;
	USHORT							g_width;
	UINT32							y_bank;
	UINT32							c_bank;
	T_SDRAM_MAP_MOVIE_RECORD_YC		addr[D_RIBERY_YC_BANK_MAX];
} T_SDRAM_MAP_MOVIE_RECORD_RIBERY_YC;


typedef struct{
	UINT32							addr;
	UINT32							size;
} T_SDRAM_MAP_MOVIE_RECORD_WAVE_HEADER;

typedef struct{
	UINT32							start_addr;
	UINT32							end_addr;
	T_SDRAM_MAP_MOVIE_RECORD_YC		addr[D_WAVE_YC_BANK_MAX];
} T_SDRAM_MAP_MOVIE_RECORD_WAVE_CMPRS;

typedef struct {
	BYTE							max_bank;
	USHORT							g_width;
	USHORT							g_lines;
	ULONG							size;
	ULONG							addr[D_WAVE_LOCALDEC_BANK_MAX];
} T_SDRAM_MAP_MOVIE_RECORD_WAVE_LOCALDEC;

typedef struct {
	BYTE							index;
	BYTE							max_bank;
	USHORT							g_width;
	USHORT							g_lines;
	UINT32							y_bank;
	UINT32							c_bank;
	T_SDRAM_MAP_MOVIE_RECORD_YC		addr[D_WAVE_YC_BANK_MAX];
} T_SDRAM_MAP_MOVIE_RECORD_WAVE_YC;

typedef struct {
	BYTE							index;
	BYTE							max_bank;
	ULONG							size;
	ULONG							addr[D_WAVE_STREAM_BANK_MAX];
} T_SDRAM_MAP_MOVIE_RECORD_WAVE_STREAM;

typedef struct {
	ULONG							size;
	ULONG							addr[D_WAVE_STREAM_BANK_MAX];
} T_SDRAM_MAP_MOVIE_RECORD_WAVE_STREAM_WORK;

typedef struct {
	T_SDRAM_MAP_MOVIE_RECORD_WAVE_HEADER		nal_report[2];
	T_SDRAM_MAP_MOVIE_RECORD_WAVE_HEADER		sei[D_WAVE_YC_BANK_MAX];
	T_SDRAM_MAP_MOVIE_RECORD_WAVE_HEADER		vui;
	T_SDRAM_MAP_MOVIE_RECORD_WAVE_HEADER		header;
	T_SDRAM_MAP_MOVIE_RECORD_WAVE_CMPRS			cmprs;
	T_SDRAM_MAP_MOVIE_RECORD_WAVE_LOCALDEC		localdec;
	T_SDRAM_MAP_MOVIE_RECORD_WAVE_YC			yc;
	T_SDRAM_MAP_MOVIE_RECORD_WAVE_STREAM		stream;
	T_SDRAM_MAP_MOVIE_RECORD_WAVE_STREAM_WORK	stream_work;
} T_SDRAM_MAP_MOVIE_RECORD_WAVE;

typedef struct {
	UINT32					wave_localdec_top_addr;
	UINT32					wave_localdec_bottom_addr;
	UINT32					wave_yc_top_addr;
	UINT32					wave_yc_bottom_addr;
	UINT32					wave_stream_top_addr;
	UINT32					wave_stream_bottom_addr;

	UINT32					ribery_reduction_top_addr;
	UINT32					ribery_reduction_bottom_addr;
	UINT32					ribery_localdec_top_addr;
	UINT32					ribery_localdec_bottom_addr;
	UINT32					ribery_yc_top_addr;
	UINT32					ribery_yc_bottom_addr;
	UINT32					ribery_dummy_top_addr;
	UINT32					ribery_dummy_bottom_addr;
} T_SDRAM_MAP_MOVIE_RECORD_BASE_INFORMATION;

typedef struct {
	UINT32					wave_map_count;
	E_H265_IF_IMAGE_SIZE	wave_map_size[E_H265_IF_STREAM_CH_MAX];
	UINT32					wave_map_localdec_total_size[E_H265_IF_STREAM_CH_MAX];
	UINT32					wave_map_localdec_top_addr[E_H265_IF_STREAM_CH_MAX];
	UINT32					wave_map_localdec_bottom_addr[E_H265_IF_STREAM_CH_MAX];
	UINT32					wave_map_localdec_free_size;
	UINT32					wave_map_localdec_use;
	UINT32					wave_map_yc_total_size[E_H265_IF_STREAM_CH_MAX];
	UINT32					wave_map_yc_top_addr[E_H265_IF_STREAM_CH_MAX];
	UINT32					wave_map_yc_bottom_addr[E_H265_IF_STREAM_CH_MAX];
	UINT32					wave_map_yc_free_size;
	UINT32					wave_map_yc_use;
	UINT32					wave_map_stream_total_size[E_H265_IF_STREAM_CH_MAX];
	UINT32					wave_map_stream_top_addr[E_H265_IF_STREAM_CH_MAX];
	UINT32					wave_map_stream_bottom_addr[E_H265_IF_STREAM_CH_MAX];
	UINT32					wave_map_stream_free_size;
	UINT32					wave_map_stream_use;

	UINT32					ribery_map_count;
	E_H264_IF_IMAGE_SIZE	ribery_map_size[E_H264_IF_STREAM_CH_MAX];
	UINT32					ribery_map_reduction_total_size[E_H264_IF_STREAM_CH_MAX];
	UINT32					ribery_map_reduction_top_addr[E_H264_IF_STREAM_CH_MAX];
	UINT32					ribery_map_reduction_bottom_addr[E_H264_IF_STREAM_CH_MAX];
	UINT32					ribery_map_reduction_free_size;
	UINT32					ribery_map_reduction_use;
	UINT32					ribery_map_localdec_total_size[E_H264_IF_STREAM_CH_MAX];
	UINT32					ribery_map_localdec_top_addr[E_H264_IF_STREAM_CH_MAX];
	UINT32					ribery_map_localdec_bottom_addr[E_H264_IF_STREAM_CH_MAX];
	UINT32					ribery_map_localdec_free_size;
	UINT32					ribery_map_localdec_use;
	UINT32					ribery_map_yc_total_size[E_H264_IF_STREAM_CH_MAX];
	UINT32					ribery_map_yc_top_addr[E_H264_IF_STREAM_CH_MAX];
	UINT32					ribery_map_yc_bottom_addr[E_H264_IF_STREAM_CH_MAX];
	UINT32					ribery_map_yc_free_size;
	UINT32					ribery_map_yc_use;
} T_SDRAM_MAP_MOVIE_RECORD_INFORMATION;

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum{
	E_SDRAM_MAP_MOVIE_RECORD_PATTERN_0 = 0,
	E_SDRAM_MAP_MOVIE_RECORD_PATTERN_STITCH,
	E_SDRAM_MAP_MOVIE_RECORD_PATTERN_MAX,
} E_SDRAM_MAP_MOVIE_RECORD_PATTERN;


/*----------------------------------------------------------------------*/
/* Function Prototype Definition										*/
/*----------------------------------------------------------------------*/

// Common
extern	INT32	Sdram_Map_Movie_Record_Initialize( E_SDRAM_MAP_MOVIE_RECORD_PATTERN pattern );

extern	ULONG	Sdram_Map_Movie_Record_Get_TS_Work0( VOID );
extern	ULONG	Sdram_Map_Movie_Record_Get_TS_Work1( VOID );

// Audio
extern	ULONG	Sdram_Map_Movie_Record_Get_Audio_Stream0( VOID );
extern	ULONG	Sdram_Map_Movie_Record_Get_Audio_Stream1( VOID );
extern	VOID	Sdram_Map_Movie_Record_Chg_Audio_Stream0( VOID );
extern	VOID	Sdram_Map_Movie_Record_Chg_Audio_Stream1( VOID );
extern	ULONG	Sdram_Map_Movie_Record_Get_Audio_Stream0_Max_Bank( VOID );
extern	ULONG	Sdram_Map_Movie_Record_Get_Audio_Stream1_Max_Bank( VOID );

// H265
extern	VOID	Sdram_Map_Movie_Record_Get_H265_Code( UINT32* addr_ch0, UINT32* size_ch0, UINT32* addr_ch1, UINT32* size_ch1 );
extern	VOID	Sdram_Map_Movie_Record_Get_H265_Map( E_H265_IF_STREAM_CH stream_ch, T_SDRAM_MAP_MOVIE_RECORD_WAVE* map_wave );
extern	VOID	Sdram_Map_Movie_Record_Get_H265_Vin_Dummy( E_H265_IF_STREAM_CH stream_ch, E_H265_IF_IMAGE_SIZE size, T_H265_IF_GET_IMAGE_ADDR_PARAM* addr_param );
extern	VOID	Sdram_Map_Movie_Record_Get_H265_SEI( E_H265_IF_STREAM_CH stream_ch, UINT32 index, UINT32* addr, UINT32* size );
extern	VOID	Sdram_Map_Movie_Record_Get_H265_VUI( E_H265_IF_STREAM_CH stream_ch, UINT32* addr, UINT32* size );
extern	VOID	Sdram_Map_Movie_Record_Get_H265_NAL_Report( E_H265_IF_STREAM_CH stream_ch, UINT32 index, UINT32* addr, UINT32* size );
extern	VOID	Sdram_Map_Movie_Record_Get_H265_Header( E_H265_IF_STREAM_CH stream_ch, UINT32* addr, UINT32* size );
extern	VOID	Sdram_Map_Movie_Record_Get_H265_Vin( UINT32 stream_ch, UINT32 idx, T_H265_IF_GET_IMAGE_ADDR_PARAM* addr_param );
extern	VOID	Sdram_Map_Movie_Record_Get_H265_Stream_Bank( UINT32 stream_ch, UINT32 idx, UINT32* addr, UINT32* size );
extern	VOID	Sdram_Map_Movie_Record_Get_H265_Stream_Index( UINT32 stream_ch, UINT32* idx, UINT32 addr );
extern	UINT32	Sdram_Map_Movie_Record_Get_H265_Stream_Work( UINT32 stream_ch, UINT32* work_size, UINT32 index );
extern	UINT8	Sdram_Map_Movie_Record_Get_H265_Stream_Bank_Num( UINT32 stream_ch );

extern	VOID	Sdram_Map_Movie_Record_Set_H265_Map( E_H265_IF_STREAM_CH stream_ch, T_SDRAM_MAP_MOVIE_RECORD_WAVE* map_wave );
extern	VOID	Sdram_Map_Movie_Record_Get_H265_Map_YC_Information( E_H265_IF_STREAM_CH stream_ch, T_SDRAM_MAP_MOVIE_RECORD_WAVE_YC* map_yc_information );
extern	VOID	Sdram_Map_Movie_Record_Set_H265_Map_YC_Information( E_H265_IF_STREAM_CH stream_ch, T_SDRAM_MAP_MOVIE_RECORD_WAVE_YC* map_yc_information );


// H264
extern	ULONG	Sdram_Map_Movie_Record_Get_H264_Stream_Work0( VOID );
extern	ULONG	Sdram_Map_Movie_Record_Get_H264_Stream_Work1( VOID );
extern	ULONG	Sdram_Map_Movie_Record_Get_H264_Stream_Work_Size( VOID );
extern	ULONG	Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_Start_Address( VOID );
extern	ULONG	Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_End_Address( VOID );
extern	ULONG	Sdram_Map_Movie_Record_Get_H264_Ribery_Stream1_Start_Address( VOID );
extern	ULONG	Sdram_Map_Movie_Record_Get_H264_Ribery_Stream1_End_Address( VOID );
extern	ULONG	Sdram_Map_Movie_Record_Get_H264_Ribery_Work0( VOID );
extern	ULONG	Sdram_Map_Movie_Record_Get_H264_Ribery_Work1( VOID );
extern	ULONG	Sdram_Map_Movie_Record_Get_H264_Ribery_Work2( VOID );
extern	VOID	Sdram_Map_Movie_Record_Get_H264_Vin_Dummy( E_H264_IF_STREAM_CH stream_ch, E_H264_IF_IMAGE_SIZE size, T_H264_IF_GET_IMAGE_ADDR_PARAM* addr_param );
extern	VOID	Sdram_Map_Movie_Record_Get_H264_Vin( UINT32 stream_ch, UINT32 idx, T_H264_IF_GET_IMAGE_ADDR_PARAM* addr_param );
extern	VOID	Sdram_Map_Movie_Record_Get_H264_Map_Reduction_Information( E_H264_IF_STREAM_CH stream_ch, T_SDRAM_MAP_MOVIE_RECORD_RIBERY_REDUCTION* map_reduction_information );
extern	VOID	Sdram_Map_Movie_Record_Set_H264_Map_Reduction_Information( E_H264_IF_STREAM_CH stream_ch, T_SDRAM_MAP_MOVIE_RECORD_RIBERY_REDUCTION* map_reduction_information );
extern	VOID	Sdram_Map_Movie_Record_Get_H264_Map_Local_Dec_Information( E_H264_IF_STREAM_CH stream_ch, T_SDRAM_MAP_MOVIE_RECORD_RIBERY_LOCALDEC* map_local_dec_information );
extern	VOID	Sdram_Map_Movie_Record_Set_H264_Map_Local_Dec_Information( E_H264_IF_STREAM_CH stream_ch, T_SDRAM_MAP_MOVIE_RECORD_RIBERY_LOCALDEC* map_local_dec_information );
extern	VOID	Sdram_Map_Movie_Record_Get_H264_Map_YC_Information( E_H264_IF_STREAM_CH stream_ch, T_SDRAM_MAP_MOVIE_RECORD_RIBERY_YC* map_yc_information );
extern	VOID	Sdram_Map_Movie_Record_Set_H264_Map_YC_Information( E_H264_IF_STREAM_CH stream_ch, T_SDRAM_MAP_MOVIE_RECORD_RIBERY_YC* map_yc_information );

// Common
extern	VOID	Sdram_Map_Movie_Record_Get_Map_Base_Information( T_SDRAM_MAP_MOVIE_RECORD_BASE_INFORMATION* map_base_information );
extern	VOID	Sdram_Map_Movie_Record_Get_Map_Information( T_SDRAM_MAP_MOVIE_RECORD_INFORMATION* map_information );
extern	VOID	Sdram_Map_Movie_Record_Set_H264_Map_Information( T_SDRAM_MAP_MOVIE_RECORD_INFORMATION* map_information );
extern	VOID	Sdram_Map_Movie_Record_Set_H265_Map_Information( T_SDRAM_MAP_MOVIE_RECORD_INFORMATION* map_information );
extern	ULONG 	Sdram_Map_Movie_Record_Get_H265_Temp_Addr_Y( UCHAR ch );
extern	ULONG 	Sdram_Map_Movie_Record_Get_H265_Temp_Size_Y( UCHAR ch );
extern	ULONG 	Sdram_Map_Movie_Record_Get_H265_Temp_Addr_C( UCHAR ch );
extern	ULONG 	Sdram_Map_Movie_Record_Get_H265_Temp_Size_C( UCHAR ch );
extern	ULONG 	Sdram_Map_Movie_Record_Get_H265_Temp_Addr_CMP( UCHAR ch );
extern	ULONG 	Sdram_Map_Movie_Record_Get_H265_Temp_Size_CMP( UCHAR ch );

// Debug
extern	VOID	Sdram_Map_Movie_Record_Check_Address( VOID );

#endif
