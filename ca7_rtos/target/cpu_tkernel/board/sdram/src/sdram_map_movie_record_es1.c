/**
 * @file		sdram_map_movie_record.c
 * @brief		movie "H.264 Record" memory map.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_common.h"
#include "fj_encoder.h"
#include "sdram_map_movie_record.h"
#include "dd_arm.h"
#include "debug.h"
#include "h265_sirius.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// The maximum which can be taken in each mode is set up.
#define	D_AUDIO_STREAM_BANK_MAX				(SDRAM_BNK_MOVIE_RECORD0_AUDIO_STREAM / 2)
#define	D_H264_STREAM_WORK_BANK_MAX			2
#define	D_H264_STREAM_BANK_MAX				1
#define	D_SIRIUS_VEB_WORK_BANK_MAX			2

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// map control structure
typedef struct {
	BYTE	index;
	BYTE	max_bank;
	ULONG	addr[D_AUDIO_STREAM_BANK_MAX];
} T_AUDIO_STREAM;

typedef struct {
	USHORT						g_width;
	UINT32						bank_size;
	T_SDRAM_MAP_MOVIE_RECORD_YC	addr;
} T_SDRAM_MAP_MOVIE_RECORD_DUMMY_YC;

typedef struct {
	ULONG								unused_area_addr;
	T_SDRAM_MAP_MOVIE_RECORD_DUMMY_YC	yc_dummy[E_H264_IF_STREAM_CH_MAX];
} T_SDRAM_MAP_MOVIE_RECORD_RIBEYR_DUMMY_YC_INFO;

typedef struct {
	T_SDRAM_MAP_MOVIE_RECORD_DUMMY_YC	yc_dummy[E_H265_IF_STREAM_CH_MAX];
} T_SDRAM_MAP_MOVIE_RECORD_SIRIUS_DUMMY_YC_INFO;

typedef struct {
	BYTE	index;
	BYTE	max_bank;
	ULONG	addr[D_H264_STREAM_WORK_BANK_MAX];
} T_SDRAM_MAP_MOVIE_RECORD_RIBERY_STREAM_WORK;

typedef struct {
	ULONG	start_addr;
	ULONG	end_addr;
} T_SDRAM_MAP_MOVIE_RECORD_RIBERY_STREAM;

typedef struct{
	T_SDRAM_MAP_MOVIE_RECORD_RIBERY_REDUCTION	reduction;
	T_SDRAM_MAP_MOVIE_RECORD_RIBERY_LOCALDEC	local_dec;
	T_SDRAM_MAP_MOVIE_RECORD_RIBERY_YC			yc;
} T_SDRAM_MAP_MOVIE_RECORD_RIBERY;

// movie map structure on SDRAM
typedef struct {
	ULONG											ts_work0;
	ULONG											ts_work1;
	T_AUDIO_STREAM									audio_stream0;
	T_AUDIO_STREAM									audio_stream1;
	T_SDRAM_MAP_MOVIE_RECORD_SIRIUS_REMAP_CONFIG	sirius_remap_config;
	ULONG											sirius_code_addr;
	ULONG											sirius_work_addr;
	T_SDRAM_MAP_MOVIE_RECORD_SIRIUS					sirius[E_H265_IF_STREAM_CH_MAX];
	T_SDRAM_MAP_MOVIE_RECORD_SIRIUS_DUMMY_YC_INFO	sirius_dummy_yc;
	ULONG											ribery_work0_addr;
	ULONG											ribery_work1_addr;
	ULONG											ribery_work2_addr;
	T_SDRAM_MAP_MOVIE_RECORD_RIBERY_STREAM			ribery_stream0;
	T_SDRAM_MAP_MOVIE_RECORD_RIBERY_STREAM			ribery_stream1;
	T_SDRAM_MAP_MOVIE_RECORD_RIBERY_STREAM_WORK		ribery_stream_work;
	T_SDRAM_MAP_MOVIE_RECORD_RIBERY					ribery[E_H264_IF_STREAM_CH_MAX];
	T_SDRAM_MAP_MOVIE_RECORD_RIBEYR_DUMMY_YC_INFO	ribery_dummy_yc;
	T_SDRAM_MAP_MOVIE_RECORD_BASE_INFORMATION		sdram_map_base_information;
	T_SDRAM_MAP_MOVIE_RECORD_INFORMATION			sdram_map_information;
} T_SDRAM_MAP_MOVIE_RECORD;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// record  memory map on SDRAM
static T_SDRAM_MAP_MOVIE_RECORD	gSdram_Map_Movie_Record;

// dummy memory table for h264
static volatile INT32 gSdram_Map_Movie_Record_Dummy_Use_Flag_H264[E_H264_IF_STREAM_CH_MAX] = {H264_OFF,H264_OFF,H264_OFF};

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
 *  @brief		Initialize SDRAM Index for Movie Mode.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 Sdram_Map_Movie_Record_Initialize( E_SDRAM_MAP_MOVIE_RECORD_PATTERN pattern )
{
	INT32 result = 0;
	
	INT32 count;
	UINT32 y_lines;
	UINT32 c_lines;
	
	// Init
	memset( &gSdram_Map_Movie_Record, 0, sizeof(gSdram_Map_Movie_Record) );
	
	switch( pattern ){
		case E_SDRAM_MAP_MOVIE_RECORD_PATTERN_0 :
			gSdram_Map_Movie_Record.ts_work0							= SDRAM_ADR_MOVIE_RECORD0_TS_WORK0;
			gSdram_Map_Movie_Record.ts_work1							= SDRAM_ADR_MOVIE_RECORD0_TS_WORK1;
			
			// audio_stream0
			gSdram_Map_Movie_Record.audio_stream0.index					= 0;
			gSdram_Map_Movie_Record.audio_stream0.max_bank				= SDRAM_BNK_MOVIE_RECORD0_AUDIO_STREAM / 2;
			for( count=0; count < (SDRAM_BNK_MOVIE_RECORD0_AUDIO_STREAM / 2); count++ ){
				gSdram_Map_Movie_Record.audio_stream0.addr[count] = SDRAM_ADR_MOVIE_RECORD0_AUDIO_STREAM_0 + SDRAM_SIZ_MOVIE_RECORD0_AUDIO_STREAM * count;
			}
			// audio_stream1
			gSdram_Map_Movie_Record.audio_stream1.index					= 0;
			gSdram_Map_Movie_Record.audio_stream1.max_bank				= SDRAM_BNK_MOVIE_RECORD0_AUDIO_STREAM / 2;
			for( count=0; count < (SDRAM_BNK_MOVIE_RECORD0_AUDIO_STREAM / 2); count++ ){
				gSdram_Map_Movie_Record.audio_stream1.addr[count] = SDRAM_ADR_MOVIE_RECORD0_AUDIO_STREAM_192 + SDRAM_SIZ_MOVIE_RECORD0_AUDIO_STREAM * count;
			}
			
			// sirius  remap config		sirius code have to exist of SDRAM bottom.
			gSdram_Map_Movie_Record.sirius_remap_config.remap_ch0_top	= SDRAM_ADR_CH0_END - D_SIRIUS_REMAP_SIZE;
			gSdram_Map_Movie_Record.sirius_remap_config.remap_ch0_size	= D_SIRIUS_REMAP_SIZE;
			gSdram_Map_Movie_Record.sirius_remap_config.remap_ch1_top	= SDRAM_ADR_CH1_END - D_SIRIUS_REMAP_SIZE;
			gSdram_Map_Movie_Record.sirius_remap_config.remap_ch1_size	= D_SIRIUS_REMAP_SIZE;
			
			// sirius  code & work
			gSdram_Map_Movie_Record.sirius_code_addr					= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_CODE;
			gSdram_Map_Movie_Record.sirius_work_addr					= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_WORK;
			
			// sirius  0ch
			gSdram_Map_Movie_Record.sirius[0].map_mode					= D_H265_ENC_MAP_MODE_3K_NUM;
			
			gSdram_Map_Movie_Record.sirius[0].dpb_cmprs.start_addr		= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_DPB_CMPRS_0;
			gSdram_Map_Movie_Record.sirius[0].dpb_cmprs.end_addr		= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_DPB_CMPRS_END;
			
			gSdram_Map_Movie_Record.sirius[0].cmprs.start_addr			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_0;
			gSdram_Map_Movie_Record.sirius[0].cmprs.end_addr			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_END;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[0].y			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_0;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[0].c			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_0 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[1].y			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_1;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[1].c			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_1 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[2].y			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_2;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[2].c			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_2 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[3].y			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_3;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[3].c			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_3 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[4].y			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_4;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[4].c			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_4 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[5].y			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_5;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[5].c			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_5 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[6].y			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_6;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[6].c			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_6 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[7].y			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_7;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[7].c			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_7 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[8].y			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_8;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[8].c			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_8 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[9].y			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_9;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[9].c			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_9 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[10].y			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_10;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[10].c			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_10 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[11].y			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_11;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[11].c			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_11 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[12].y			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_12;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[12].c			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_12 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[13].y			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_13;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[13].c			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_13 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[14].y			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_14;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[14].c			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_14 + D_SIRIUS_CMPRS_Y_SIZE;
			
			gSdram_Map_Movie_Record.sirius[0].dpb.start_addr			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_DPB_0;
			gSdram_Map_Movie_Record.sirius[0].dpb.end_addr				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_DPB_END;
			
			gSdram_Map_Movie_Record.sirius[0].veb.start_addr			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_VEB;
			gSdram_Map_Movie_Record.sirius[0].veb.end_addr				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_VEB + SDRAM_SIZ_MOVIE_RECORD0_SIRIUS_0_VEB;
			
			gSdram_Map_Movie_Record.sirius[0].veb_work					= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_VEB_WORK;
			gSdram_Map_Movie_Record.sirius[0].veb_work_size				= SDRAM_SIZ_MOVIE_RECORD0_SIRIUS_0_VEB_WORK;
			
			gSdram_Map_Movie_Record.sirius[0].yc.index					= 0;
			gSdram_Map_Movie_Record.sirius[0].yc.max_bank				= SDRAM_BNK_MOVIE_RECORD0_SIRIUS_0_Y;
			gSdram_Map_Movie_Record.sirius[0].yc.g_width				= SDRAM_WIDTH_MOVIE_RECORD0_SIRIUS_0_Y;
			gSdram_Map_Movie_Record.sirius[0].yc.g_lines				= SDRAM_LINES_MOVIE_RECORD0_SIRIUS_0_Y;
			gSdram_Map_Movie_Record.sirius[0].yc.y_bank					= SDRAM_SIZ_MOVIE_RECORD0_SIRIUS_0_Y;
			gSdram_Map_Movie_Record.sirius[0].yc.c_bank					= SDRAM_SIZ_MOVIE_RECORD0_SIRIUS_0_C;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[0].y				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_Y_0;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[0].c				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_C_0;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[1].y				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_Y_1;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[1].c				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_C_1;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[2].y				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_Y_2;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[2].c				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_C_2;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[3].y				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_Y_3;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[3].c				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_C_3;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[4].y				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_Y_4;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[4].c				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_C_4;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[5].y				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_Y_5;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[5].c				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_C_5;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[6].y				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_Y_6;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[6].c				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_C_6;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[7].y				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_Y_7;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[7].c				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_C_7;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[8].y				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_Y_8;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[8].c				= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_C_8;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[9].y				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[9].c				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[10].y				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[10].c				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[11].y				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[11].c				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[12].y				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[12].c				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[13].y				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[13].c				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[14].y				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[14].c				= (UINT32)NULL;
			
			// sirius yc_dummy
			gSdram_Map_Movie_Record.sirius_dummy_yc.yc_dummy[0].g_width		= SDRAM_WIDTH_MOVIE_RECORD0_SIRIUS_0_Y;
			
			
			// h264_ribery_work0 / h264_ribery_work1 / h264_ribery_work2
			gSdram_Map_Movie_Record.ribery_work0_addr					= SDRAM_ADR_MOVIE_RECORD0_RIBERY_WORK0;
			gSdram_Map_Movie_Record.ribery_work1_addr					= SDRAM_ADR_MOVIE_RECORD0_RIBERY_WORK1;
			gSdram_Map_Movie_Record.ribery_work2_addr					= SDRAM_ADR_MOVIE_RECORD0_RIBERY_WORK2;
			
			// h264_ribery_stream
			gSdram_Map_Movie_Record.ribery_stream0.start_addr			= SDRAM_ADR_MOVIE_RECORD0_RIBERY_STREAM0;
			gSdram_Map_Movie_Record.ribery_stream0.end_addr				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_STREAM0_END;
			
			gSdram_Map_Movie_Record.ribery_stream1.start_addr			= SDRAM_ADR_MOVIE_RECORD0_RIBERY_STREAM1;
			gSdram_Map_Movie_Record.ribery_stream1.end_addr				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_STREAM1_END;
			
			// h264_stream_work
			gSdram_Map_Movie_Record.ribery_stream_work.index			= 0;
			gSdram_Map_Movie_Record.ribery_stream_work.max_bank			= SDRAM_BNK_MOVIE_RECORD0_RIBERY_STREAM_WORK;
			gSdram_Map_Movie_Record.ribery_stream_work.addr[0]			= SDRAM_ADR_MOVIE_RECORD0_RIBERY_STREAM_WORK_0;
			gSdram_Map_Movie_Record.ribery_stream_work.addr[1]			= SDRAM_ADR_MOVIE_RECORD0_RIBERY_STREAM_WORK_1;
			
			// h264_ribery_reduction
			gSdram_Map_Movie_Record.ribery[0].reduction.max_bank		= SDRAM_BNK_MOVIE_RECORD0_RIBERY_YC_REDUCTION;
			gSdram_Map_Movie_Record.ribery[0].reduction.g_width			= (((SDRAM_WIDTH_MOVIE_RECORD0_RIBERY_YC/4) + D_RIBERY_WIDTH_ALIGNMENT - 1) / D_RIBERY_WIDTH_ALIGNMENT) * D_RIBERY_WIDTH_ALIGNMENT;
			y_lines														= (((SDRAM_LINES_MOVIE_RECORD0_RIBERY_YC/4) + D_RIBERY_LINES_ALIGNMENT - 1) / D_RIBERY_LINES_ALIGNMENT) * D_RIBERY_LINES_ALIGNMENT;
			c_lines														= (((y_lines/2) + D_RIBERY_LINES_ALIGNMENT - 1) / D_RIBERY_LINES_ALIGNMENT) * D_RIBERY_LINES_ALIGNMENT;
			gSdram_Map_Movie_Record.ribery[0].reduction.y_bank			= (((gSdram_Map_Movie_Record.ribery[0].reduction.g_width * y_lines) + D_RIBERY_BANK_ALIGNMENT - 1) / D_RIBERY_BANK_ALIGNMENT) * D_RIBERY_BANK_ALIGNMENT;
			gSdram_Map_Movie_Record.ribery[0].reduction.c_bank			= (((gSdram_Map_Movie_Record.ribery[0].reduction.g_width * c_lines) + D_RIBERY_BANK_ALIGNMENT - 1) / D_RIBERY_BANK_ALIGNMENT) * D_RIBERY_BANK_ALIGNMENT;
			gSdram_Map_Movie_Record.ribery[0].reduction.y_addr			= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_REDUCTION_0;
			gSdram_Map_Movie_Record.ribery[0].reduction.c_addr			= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_REDUCTION_0;	// if same SDRAM ch, c_addr is same at y_addr;
			
			// h264_ribery_local_dec
			gSdram_Map_Movie_Record.ribery[0].local_dec.max_bank		= SDRAM_BNK_MOVIE_RECORD0_RIBERY_YC_LOCALDEC;
			gSdram_Map_Movie_Record.ribery[0].local_dec.g_width			= (((SDRAM_WIDTH_MOVIE_RECORD0_RIBERY_YC_LOCALDEC) + D_RIBERY_WIDTH_ALIGNMENT - 1) / D_RIBERY_WIDTH_ALIGNMENT) * D_RIBERY_WIDTH_ALIGNMENT;
			y_lines														= (((SDRAM_LINES_MOVIE_RECORD0_RIBERY_YC_LOCALDEC) + D_RIBERY_LINES_ALIGNMENT - 1) / D_RIBERY_LINES_ALIGNMENT) * D_RIBERY_LINES_ALIGNMENT;
			c_lines														= (((y_lines/2) + D_RIBERY_LINES_ALIGNMENT - 1) / D_RIBERY_LINES_ALIGNMENT) * D_RIBERY_LINES_ALIGNMENT;
			gSdram_Map_Movie_Record.ribery[0].local_dec.y_bank			= (((gSdram_Map_Movie_Record.ribery[0].local_dec.g_width * y_lines) + D_RIBERY_BANK_ALIGNMENT - 1) / D_RIBERY_BANK_ALIGNMENT) * D_RIBERY_BANK_ALIGNMENT;
			gSdram_Map_Movie_Record.ribery[0].local_dec.c_bank			= (((gSdram_Map_Movie_Record.ribery[0].local_dec.g_width * c_lines) + D_RIBERY_BANK_ALIGNMENT - 1) / D_RIBERY_BANK_ALIGNMENT) * D_RIBERY_BANK_ALIGNMENT;
			gSdram_Map_Movie_Record.ribery[0].local_dec.y_addr			= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_LOCALDEC_0;
			gSdram_Map_Movie_Record.ribery[0].local_dec.c_addr			= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_LOCALDEC_0;	// if same SDRAM ch, c_addr is same at y_addr;
			
			// h264_ribery_yc
			gSdram_Map_Movie_Record.ribery[0].yc.index					= 0;
			gSdram_Map_Movie_Record.ribery[0].yc.max_bank				= SDRAM_BNK_MOVIE_RECORD0_RIBERY_YC;
			gSdram_Map_Movie_Record.ribery[0].yc.g_width				= (((SDRAM_WIDTH_MOVIE_RECORD0_RIBERY_YC) + D_RIBERY_WIDTH_ALIGNMENT - 1) / D_RIBERY_WIDTH_ALIGNMENT) * D_RIBERY_WIDTH_ALIGNMENT;
			y_lines														= (((SDRAM_LINES_MOVIE_RECORD0_RIBERY_YC) + D_RIBERY_LINES_ALIGNMENT - 1) / D_RIBERY_LINES_ALIGNMENT) * D_RIBERY_LINES_ALIGNMENT;
			c_lines														= (((y_lines/2) + D_RIBERY_LINES_ALIGNMENT - 1) / D_RIBERY_LINES_ALIGNMENT) * D_RIBERY_LINES_ALIGNMENT;
			gSdram_Map_Movie_Record.ribery[0].yc.y_bank					= (((gSdram_Map_Movie_Record.ribery[0].yc.g_width * y_lines) + D_RIBERY_BANK_ALIGNMENT - 1) / D_RIBERY_BANK_ALIGNMENT) * D_RIBERY_BANK_ALIGNMENT;
			gSdram_Map_Movie_Record.ribery[0].yc.c_bank					= (((gSdram_Map_Movie_Record.ribery[0].yc.g_width * c_lines) + D_RIBERY_BANK_ALIGNMENT - 1) / D_RIBERY_BANK_ALIGNMENT) * D_RIBERY_BANK_ALIGNMENT;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[0].y				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_0;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[0].c				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_0 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[1].y				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_1;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[1].c				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_1 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[2].y				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_2;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[2].c				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_2 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[3].y				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_3;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[3].c				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_3 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
#if defined(CO_MEMORY_2GB)
			gSdram_Map_Movie_Record.ribery[0].yc.addr[4].y				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_4;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[4].c				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_4 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[5].y				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_5;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[5].c				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_5 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[6].y				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_6;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[6].c				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_6 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[7].y				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_7;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[7].c				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_7 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[8].y				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_8;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[8].c				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_8 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[9].y				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_9;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[9].c				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_9 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[10].y				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_10;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[10].c				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_10 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[11].y				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_11;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[11].c				= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_11 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
#endif
			// ribery yc_dummy
			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[0].g_width		= SDRAM_WIDTH_MOVIE_RECORD0_RIBERY_YC_DUMMY;
			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[0].bank_size	= E_H264_IF_IMAGE_SIZE_4096_2160;
			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[0].addr.y		= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_DUMMY;
			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[0].addr.c		= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_DUMMY + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
//			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[1].g_width		= SDRAM_WIDTH_MOVIE_RECORD0_SIRIUS_YC_DUMMY;
//			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[1].addr.y		= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_YC_DUMMY_1;
//			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[1].addr.c		= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_YC_DUMMY_1 + D_H265_ENC_MAP_MODE_4K2K_Y_BANK;
//			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[2].g_width		= SDRAM_WIDTH_MOVIE_RECORD0_SIRIUS_YC_DUMMY;
//			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[2].addr.y		= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_YC_DUMMY_1;
//			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[2].addr.c		= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_YC_DUMMY_1 + D_H265_ENC_MAP_MODE_4K2K_Y_BANK;
			
			
			// map base information
			gSdram_Map_Movie_Record.sdram_map_base_information.sirius_y_top_addr[0]			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_Y_0;
			gSdram_Map_Movie_Record.sdram_map_base_information.sirius_y_bottom_addr[0]		= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_Y_8 + SDRAM_SIZ_MOVIE_RECORD0_SIRIUS_0_Y;
			gSdram_Map_Movie_Record.sdram_map_base_information.sirius_c_top_addr[0]			= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_C_0;
			gSdram_Map_Movie_Record.sdram_map_base_information.sirius_c_bottom_addr[0]		= SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_C_8 + SDRAM_SIZ_MOVIE_RECORD0_SIRIUS_0_C;
			
			gSdram_Map_Movie_Record.sdram_map_base_information.ribery_reduction_top_addr	= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_REDUCTION_0;
#if defined(CO_MEMORY_2GB)
			gSdram_Map_Movie_Record.sdram_map_base_information.ribery_reduction_bottom_addr	= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_REDUCTION_5 + SDRAM_SIZ_MOVIE_RECORD0_RIBERY_YC_REDUCTION;
#else
			gSdram_Map_Movie_Record.sdram_map_base_information.ribery_reduction_bottom_addr	= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_REDUCTION_3 + SDRAM_SIZ_MOVIE_RECORD0_RIBERY_YC_REDUCTION;
#endif
			gSdram_Map_Movie_Record.sdram_map_base_information.ribery_localdec_top_addr		= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_LOCALDEC_0;
			gSdram_Map_Movie_Record.sdram_map_base_information.ribery_localdec_bottom_addr	= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_LOCALDEC_1  + SDRAM_SIZ_MOVIE_RECORD0_RIBERY_YC_LOCALDEC;
			gSdram_Map_Movie_Record.sdram_map_base_information.ribery_yc_top_addr			= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_0;
#if defined(CO_MEMORY_2GB)
			gSdram_Map_Movie_Record.sdram_map_base_information.ribery_yc_bottom_addr		= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_11 + SDRAM_SIZ_MOVIE_RECORD0_RIBERY_YC;
#else
			gSdram_Map_Movie_Record.sdram_map_base_information.ribery_yc_bottom_addr		= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_3 + SDRAM_SIZ_MOVIE_RECORD0_RIBERY_YC;
#endif
			gSdram_Map_Movie_Record.sdram_map_base_information.ribery_dummy_top_addr		= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_DUMMY;
			gSdram_Map_Movie_Record.sdram_map_base_information.ribery_dummy_bottom_addr		= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_DUMMY_END;
			
			// map information initialize
			memset( &gSdram_Map_Movie_Record.sdram_map_information, 0, sizeof(T_SDRAM_MAP_MOVIE_RECORD_INFORMATION) );
			gSdram_Map_Movie_Record.sdram_map_information.ribery_map_size[0]				= E_H264_IF_IMAGE_SIZE_MAX;
			gSdram_Map_Movie_Record.sdram_map_information.ribery_map_size[1]				= E_H264_IF_IMAGE_SIZE_MAX;
			gSdram_Map_Movie_Record.sdram_map_information.ribery_map_size[2]				= E_H264_IF_IMAGE_SIZE_MAX;
			gSdram_Map_Movie_Record.sdram_map_information.ribery_map_reduction_use			= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_REDUCTION_0;
			gSdram_Map_Movie_Record.sdram_map_information.ribery_map_reduction_free_size	= SDRAM_SIZ_MOVIE_RECORD0_RIBERY_YC_REDUCTION * SDRAM_BNK_MOVIE_RECORD0_RIBERY_YC_REDUCTION;
			gSdram_Map_Movie_Record.sdram_map_information.ribery_map_localdec_use			= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_LOCALDEC_0;
			gSdram_Map_Movie_Record.sdram_map_information.ribery_map_localdec_free_size		= SDRAM_SIZ_MOVIE_RECORD0_RIBERY_YC_LOCALDEC * SDRAM_BNK_MOVIE_RECORD0_RIBERY_YC_LOCALDEC;
			gSdram_Map_Movie_Record.sdram_map_information.ribery_map_yc_use					= SDRAM_ADR_MOVIE_RECORD0_RIBERY_YC_0;
			gSdram_Map_Movie_Record.sdram_map_information.ribery_map_yc_free_size			= SDRAM_SIZ_MOVIE_RECORD0_RIBERY_YC * SDRAM_BNK_MOVIE_RECORD0_RIBERY_YC;
			
			result = 0;
			break;
#if 0
		case E_SDRAM_MAP_MOVIE_RECORD_PATTERN_STITCH :
			gSdram_Map_Movie_Record.ts_work0							= SDRAM_ADR_MOVIE_RECORDS_TS_WORK0;
			gSdram_Map_Movie_Record.ts_work1							= SDRAM_ADR_MOVIE_RECORDS_TS_WORK1;
			
			// audio_stream0
			gSdram_Map_Movie_Record.audio_stream0.index					= 0;
			gSdram_Map_Movie_Record.audio_stream0.max_bank				= SDRAM_BNK_MOVIE_RECORDS_AUDIO_STREAM / 2;
			for( count=0; count < (SDRAM_BNK_MOVIE_RECORDS_AUDIO_STREAM / 2); count++ ){
				gSdram_Map_Movie_Record.audio_stream0.addr[count] = SDRAM_ADR_MOVIE_RECORDS_AUDIO_STREAM_0 + SDRAM_SIZ_MOVIE_RECORDS_AUDIO_STREAM * count;
			}
			// audio_stream1
			gSdram_Map_Movie_Record.audio_stream1.index					= 0;
			gSdram_Map_Movie_Record.audio_stream1.max_bank				= SDRAM_BNK_MOVIE_RECORDS_AUDIO_STREAM / 2;
			for( count=0; count < (SDRAM_BNK_MOVIE_RECORDS_AUDIO_STREAM / 2); count++ ){
				gSdram_Map_Movie_Record.audio_stream1.addr[count] = SDRAM_ADR_MOVIE_RECORDS_AUDIO_STREAM_192 + SDRAM_SIZ_MOVIE_RECORDS_AUDIO_STREAM * count;
			}
			
			// sirius  remap config		sirius code have to exist of SDRAM bottom.
			gSdram_Map_Movie_Record.sirius_remap_config.remap_ch0_top	= SDRAM_ADR_CH0_END - D_SIRIUS_REMAP_SIZE;
			gSdram_Map_Movie_Record.sirius_remap_config.remap_ch0_size	= D_SIRIUS_REMAP_SIZE;
			gSdram_Map_Movie_Record.sirius_remap_config.remap_ch1_top	= SDRAM_ADR_CH1_END - D_SIRIUS_REMAP_SIZE;
			gSdram_Map_Movie_Record.sirius_remap_config.remap_ch1_size	= D_SIRIUS_REMAP_SIZE;
			
			// sirius  code & work
			gSdram_Map_Movie_Record.sirius_code_addr					= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_CODE;
			gSdram_Map_Movie_Record.sirius_work_addr					= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_WORK;
			
			// sirius  0ch
			gSdram_Map_Movie_Record.sirius[0].map_mode					= D_H265_ENC_MAP_MODE_3K_NUM;
			
			gSdram_Map_Movie_Record.sirius[0].dpb_cmprs.start_addr		= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_DPB_CMPRS_0;
			gSdram_Map_Movie_Record.sirius[0].dpb_cmprs.end_addr		= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_DPB_CMPRS_END;
			
			gSdram_Map_Movie_Record.sirius[0].cmprs.start_addr			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_0;
			gSdram_Map_Movie_Record.sirius[0].cmprs.end_addr			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_END;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[0].y			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_0;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[0].c			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_0 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[1].y			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_1;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[1].c			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_1 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[2].y			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_2;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[2].c			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_2 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[3].y			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_3;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[3].c			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_3 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[4].y			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_4;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[4].c			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_4 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[5].y			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_5;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[5].c			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_5 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[6].y			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_6;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[6].c			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_6 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[7].y			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_7;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[7].c			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_7 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[8].y			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_8;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[8].c			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_8 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[9].y			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_9;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[9].c			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_9 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[10].y			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_10;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[10].c			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_10 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[11].y			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_11;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[11].c			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_11 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[12].y			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_12;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[12].c			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_12 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[13].y			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_13;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[13].c			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_13 + D_SIRIUS_CMPRS_Y_SIZE;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[14].y			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_14;
			gSdram_Map_Movie_Record.sirius[0].cmprs.addr[14].c			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_CMPRS_14 + D_SIRIUS_CMPRS_Y_SIZE;
			
			gSdram_Map_Movie_Record.sirius[0].dpb.start_addr			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_DPB_0;
			gSdram_Map_Movie_Record.sirius[0].dpb.end_addr				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_DPB_END;
			
			gSdram_Map_Movie_Record.sirius[0].veb.start_addr			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_VEB;
			gSdram_Map_Movie_Record.sirius[0].veb.end_addr				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_VEB + SDRAM_SIZ_MOVIE_RECORDS_SIRIUS_0_VEB;
			
			gSdram_Map_Movie_Record.sirius[0].veb_work					= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_VEB_WORK;
			gSdram_Map_Movie_Record.sirius[0].veb_work_size				= SDRAM_SIZ_MOVIE_RECORDS_SIRIUS_0_VEB_WORK;
			
			gSdram_Map_Movie_Record.sirius[0].yc.index					= 0;
			gSdram_Map_Movie_Record.sirius[0].yc.max_bank				= SDRAM_BNK_MOVIE_RECORDS_SIRIUS_0_Y;
			gSdram_Map_Movie_Record.sirius[0].yc.g_width				= SDRAM_WIDTH_MOVIE_RECORDS_SIRIUS_0_Y;
			gSdram_Map_Movie_Record.sirius[0].yc.g_lines				= SDRAM_LINES_MOVIE_RECORDS_SIRIUS_0_Y;
			gSdram_Map_Movie_Record.sirius[0].yc.y_bank					= SDRAM_SIZ_MOVIE_RECORDS_SIRIUS_0_Y;
			gSdram_Map_Movie_Record.sirius[0].yc.c_bank					= SDRAM_SIZ_MOVIE_RECORDS_SIRIUS_0_C;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[0].y				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_Y_0;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[0].c				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_C_0;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[1].y				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_Y_1;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[1].c				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_C_1;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[2].y				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_Y_2;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[2].c				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_C_2;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[3].y				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_Y_3;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[3].c				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_C_3;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[4].y				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_Y_4;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[4].c				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_C_4;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[5].y				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_Y_5;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[5].c				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_C_5;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[6].y				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_Y_6;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[6].c				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_C_6;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[7].y				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_Y_7;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[7].c				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_C_7;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[8].y				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_Y_8;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[8].c				= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_C_8;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[9].y				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[9].c				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[10].y				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[10].c				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[11].y				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[11].c				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[12].y				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[12].c				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[13].y				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[13].c				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[14].y				= (UINT32)NULL;
			gSdram_Map_Movie_Record.sirius[0].yc.addr[14].c				= (UINT32)NULL;
			
			// sirius yc_dummy
			gSdram_Map_Movie_Record.sirius_dummy_yc.yc_dummy[0].g_width		= SDRAM_WIDTH_MOVIE_RECORDS_SIRIUS_0_Y;
			
			// h264_ribery_work0 / h264_ribery_work1 / h264_ribery_work2
			gSdram_Map_Movie_Record.ribery_work0_addr					= SDRAM_ADR_MOVIE_RECORDS_RIBERY_WORK0;
			gSdram_Map_Movie_Record.ribery_work1_addr					= SDRAM_ADR_MOVIE_RECORDS_RIBERY_WORK1;
			gSdram_Map_Movie_Record.ribery_work2_addr					= SDRAM_ADR_MOVIE_RECORDS_RIBERY_WORK2;
			
			// h264_ribery_stream
			gSdram_Map_Movie_Record.ribery_stream0.start_addr			= SDRAM_ADR_MOVIE_RECORDS_RIBERY_STREAM0;
			gSdram_Map_Movie_Record.ribery_stream0.end_addr				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_STREAM0_END;
			
			gSdram_Map_Movie_Record.ribery_stream1.start_addr			= SDRAM_ADR_MOVIE_RECORDS_RIBERY_STREAM1;
			gSdram_Map_Movie_Record.ribery_stream1.end_addr				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_STREAM1_END;
			
			// h264_stream_work
			gSdram_Map_Movie_Record.ribery_stream_work.index			= 0;
			gSdram_Map_Movie_Record.ribery_stream_work.max_bank			= SDRAM_BNK_MOVIE_RECORDS_RIBERY_STREAM_WORK;
			gSdram_Map_Movie_Record.ribery_stream_work.addr[0]			= SDRAM_ADR_MOVIE_RECORDS_RIBERY_STREAM_WORK_0;
			gSdram_Map_Movie_Record.ribery_stream_work.addr[1]			= SDRAM_ADR_MOVIE_RECORDS_RIBERY_STREAM_WORK_1;
			
			// h264_ribery_reduction
			gSdram_Map_Movie_Record.ribery[0].reduction.max_bank		= SDRAM_BNK_MOVIE_RECORDS_RIBERY_YC_REDUCTION;
			gSdram_Map_Movie_Record.ribery[0].reduction.g_width			= (((SDRAM_WIDTH_MOVIE_RECORDS_RIBERY_YC/4) + D_RIBERY_WIDTH_ALIGNMENT - 1) / D_RIBERY_WIDTH_ALIGNMENT) * D_RIBERY_WIDTH_ALIGNMENT;
			y_lines														= (((SDRAM_LINES_MOVIE_RECORDS_RIBERY_YC/4) + D_RIBERY_LINES_ALIGNMENT - 1) / D_RIBERY_LINES_ALIGNMENT) * D_RIBERY_LINES_ALIGNMENT;
			c_lines														= (((y_lines/2) + D_RIBERY_LINES_ALIGNMENT - 1) / D_RIBERY_LINES_ALIGNMENT) * D_RIBERY_LINES_ALIGNMENT;
			gSdram_Map_Movie_Record.ribery[0].reduction.y_bank			= (((gSdram_Map_Movie_Record.ribery[0].reduction.g_width * y_lines) + D_RIBERY_BANK_ALIGNMENT - 1) / D_RIBERY_BANK_ALIGNMENT) * D_RIBERY_BANK_ALIGNMENT;
			gSdram_Map_Movie_Record.ribery[0].reduction.c_bank			= (((gSdram_Map_Movie_Record.ribery[0].reduction.g_width * c_lines) + D_RIBERY_BANK_ALIGNMENT - 1) / D_RIBERY_BANK_ALIGNMENT) * D_RIBERY_BANK_ALIGNMENT;
			gSdram_Map_Movie_Record.ribery[0].reduction.y_addr			= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_REDUCTION_0;
			gSdram_Map_Movie_Record.ribery[0].reduction.c_addr			= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_REDUCTION_0;	// if same SDRAM ch, c_addr is same at y_addr;
			
			// h264_ribery_local_dec
			gSdram_Map_Movie_Record.ribery[0].local_dec.max_bank		= SDRAM_BNK_MOVIE_RECORDS_RIBERY_YC_LOCALDEC;
			gSdram_Map_Movie_Record.ribery[0].local_dec.g_width			= (((SDRAM_WIDTH_MOVIE_RECORDS_RIBERY_YC_LOCALDEC) + D_RIBERY_WIDTH_ALIGNMENT - 1) / D_RIBERY_WIDTH_ALIGNMENT) * D_RIBERY_WIDTH_ALIGNMENT;
			y_lines														= (((SDRAM_LINES_MOVIE_RECORDS_RIBERY_YC_LOCALDEC) + D_RIBERY_LINES_ALIGNMENT - 1) / D_RIBERY_LINES_ALIGNMENT) * D_RIBERY_LINES_ALIGNMENT;
			c_lines														= (((y_lines/2) + D_RIBERY_LINES_ALIGNMENT - 1) / D_RIBERY_LINES_ALIGNMENT) * D_RIBERY_LINES_ALIGNMENT;
			gSdram_Map_Movie_Record.ribery[0].local_dec.y_bank			= (((gSdram_Map_Movie_Record.ribery[0].local_dec.g_width * y_lines) + D_RIBERY_BANK_ALIGNMENT - 1) / D_RIBERY_BANK_ALIGNMENT) * D_RIBERY_BANK_ALIGNMENT;
			gSdram_Map_Movie_Record.ribery[0].local_dec.c_bank			= (((gSdram_Map_Movie_Record.ribery[0].local_dec.g_width * c_lines) + D_RIBERY_BANK_ALIGNMENT - 1) / D_RIBERY_BANK_ALIGNMENT) * D_RIBERY_BANK_ALIGNMENT;
			gSdram_Map_Movie_Record.ribery[0].local_dec.y_addr			= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_LOCALDEC_0;
			gSdram_Map_Movie_Record.ribery[0].local_dec.c_addr			= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_LOCALDEC_0;	// if same SDRAM ch, c_addr is same at y_addr;
			
			// h264_ribery_yc
			gSdram_Map_Movie_Record.ribery[0].yc.index					= 0;
			gSdram_Map_Movie_Record.ribery[0].yc.max_bank				= SDRAM_BNK_MOVIE_RECORDS_RIBERY_YC;
			gSdram_Map_Movie_Record.ribery[0].yc.g_width				= (((SDRAM_WIDTH_MOVIE_RECORDS_RIBERY_YC) + D_RIBERY_WIDTH_ALIGNMENT - 1) / D_RIBERY_WIDTH_ALIGNMENT) * D_RIBERY_WIDTH_ALIGNMENT;
			y_lines														= (((SDRAM_LINES_MOVIE_RECORDS_RIBERY_YC) + D_RIBERY_LINES_ALIGNMENT - 1) / D_RIBERY_LINES_ALIGNMENT) * D_RIBERY_LINES_ALIGNMENT;
			c_lines														= (((y_lines/2) + D_RIBERY_LINES_ALIGNMENT - 1) / D_RIBERY_LINES_ALIGNMENT) * D_RIBERY_LINES_ALIGNMENT;
			gSdram_Map_Movie_Record.ribery[0].yc.y_bank					= (((gSdram_Map_Movie_Record.ribery[0].yc.g_width * y_lines) + D_RIBERY_BANK_ALIGNMENT - 1) / D_RIBERY_BANK_ALIGNMENT) * D_RIBERY_BANK_ALIGNMENT;
			gSdram_Map_Movie_Record.ribery[0].yc.c_bank					= (((gSdram_Map_Movie_Record.ribery[0].yc.g_width * c_lines) + D_RIBERY_BANK_ALIGNMENT - 1) / D_RIBERY_BANK_ALIGNMENT) * D_RIBERY_BANK_ALIGNMENT;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[0].y				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_0;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[0].c				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_0 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[1].y				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_1;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[1].c				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_1 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[2].y				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_2;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[2].c				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_2 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[3].y				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_3;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[3].c				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_3 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[4].y				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_4;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[4].c				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_4 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[5].y				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_5;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[5].c				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_5 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[6].y				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_6;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[6].c				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_6 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[7].y				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_7;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[7].c				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_7 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[8].y				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_8;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[8].c				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_8 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[9].y				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_9;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[9].c				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_9 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[10].y				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_10;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[10].c				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_10 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[11].y				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_11;
			gSdram_Map_Movie_Record.ribery[0].yc.addr[11].c				= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_11 + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
			
			// ribery yc_dummy
			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[0].g_width		= SDRAM_WIDTH_MOVIE_RECORDS_RIBERY_YC_DUMMY;
			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[0].bank_size	= E_H264_IF_IMAGE_SIZE_4096_2160;
			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[0].addr.y		= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_DUMMY;
			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[0].addr.c		= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_DUMMY + gSdram_Map_Movie_Record.ribery[0].yc.y_bank;
//			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[1].g_width		= SDRAM_WIDTH_MOVIE_RECORDS_SIRIUS_YC_DUMMY;
//			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[1].addr.y		= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_YC_DUMMY_1;
//			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[1].addr.c		= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_YC_DUMMY_1 + D_H265_ENC_MAP_MODE_4K2K_Y_BANK;
//			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[2].g_width		= SDRAM_WIDTH_MOVIE_RECORDS_SIRIUS_YC_DUMMY;
//			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[2].addr.y		= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_YC_DUMMY_1;
//			gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[2].addr.c		= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_YC_DUMMY_1 + D_H265_ENC_MAP_MODE_4K2K_Y_BANK;
			
			
			// map base information
			gSdram_Map_Movie_Record.sdram_map_base_information.sirius_y_top_addr[0]			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_Y_0;
			gSdram_Map_Movie_Record.sdram_map_base_information.sirius_y_bottom_addr[0]		= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_Y_8 + SDRAM_SIZ_MOVIE_RECORDS_SIRIUS_0_Y;
			gSdram_Map_Movie_Record.sdram_map_base_information.sirius_c_top_addr[0]			= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_C_0;
			gSdram_Map_Movie_Record.sdram_map_base_information.sirius_c_bottom_addr[0]		= SDRAM_ADR_MOVIE_RECORDS_SIRIUS_0_C_8 + SDRAM_SIZ_MOVIE_RECORDS_SIRIUS_0_C;
			
			gSdram_Map_Movie_Record.sdram_map_base_information.ribery_reduction_top_addr	= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_REDUCTION_0;
			gSdram_Map_Movie_Record.sdram_map_base_information.ribery_reduction_bottom_addr	= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_REDUCTION_5 + SDRAM_SIZ_MOVIE_RECORDS_RIBERY_YC_REDUCTION;
			gSdram_Map_Movie_Record.sdram_map_base_information.ribery_localdec_top_addr		= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_LOCALDEC_0;
			gSdram_Map_Movie_Record.sdram_map_base_information.ribery_localdec_bottom_addr	= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_LOCALDEC_1  + SDRAM_SIZ_MOVIE_RECORDS_RIBERY_YC_LOCALDEC;
			gSdram_Map_Movie_Record.sdram_map_base_information.ribery_yc_top_addr			= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_0;
			gSdram_Map_Movie_Record.sdram_map_base_information.ribery_yc_bottom_addr		= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_11 + SDRAM_SIZ_MOVIE_RECORDS_RIBERY_YC;
			gSdram_Map_Movie_Record.sdram_map_base_information.ribery_dummy_top_addr		= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_DUMMY;
			gSdram_Map_Movie_Record.sdram_map_base_information.ribery_dummy_bottom_addr		= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_DUMMY_END;
			
			// map information initialize
			memset( &gSdram_Map_Movie_Record.sdram_map_information, 0, sizeof(T_SDRAM_MAP_MOVIE_RECORD_INFORMATION) );
			gSdram_Map_Movie_Record.sdram_map_information.ribery_map_size[0]				= E_H264_IF_IMAGE_SIZE_MAX;
			gSdram_Map_Movie_Record.sdram_map_information.ribery_map_size[1]				= E_H264_IF_IMAGE_SIZE_MAX;
			gSdram_Map_Movie_Record.sdram_map_information.ribery_map_size[2]				= E_H264_IF_IMAGE_SIZE_MAX;
			gSdram_Map_Movie_Record.sdram_map_information.ribery_map_reduction_use			= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_REDUCTION_0;
			gSdram_Map_Movie_Record.sdram_map_information.ribery_map_reduction_free_size	= SDRAM_SIZ_MOVIE_RECORDS_RIBERY_YC_REDUCTION * SDRAM_BNK_MOVIE_RECORDS_RIBERY_YC_REDUCTION;
			gSdram_Map_Movie_Record.sdram_map_information.ribery_map_localdec_use			= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_LOCALDEC_0;
			gSdram_Map_Movie_Record.sdram_map_information.ribery_map_localdec_free_size		= SDRAM_SIZ_MOVIE_RECORDS_RIBERY_YC_LOCALDEC * SDRAM_BNK_MOVIE_RECORDS_RIBERY_YC_LOCALDEC;
			gSdram_Map_Movie_Record.sdram_map_information.ribery_map_yc_use					= SDRAM_ADR_MOVIE_RECORDS_RIBERY_YC_0;
			gSdram_Map_Movie_Record.sdram_map_information.ribery_map_yc_free_size			= SDRAM_SIZ_MOVIE_RECORDS_RIBERY_YC * SDRAM_BNK_MOVIE_RECORDS_RIBERY_YC;
			
			result = 0;
			break;
#endif
		case E_SDRAM_MAP_MOVIE_RECORD_PATTERN_MAX :
		default :
#ifndef DEBUG_ON_PC
			BF_Debug_Print_Error(("Sdram_Map_Movie_Record_Initialize: input parameter error. (%d)\n", pattern));
#else
			printf("Sdram_Map_Movie_Record_Initialize: input parameter error. (%d)\n", pattern);
#endif
			result = 1;
			break;
	}
	
	USHORT loop;
	for(loop = 0; loop < E_H264_IF_STREAM_CH_MAX; loop++){
		gSdram_Map_Movie_Record_Dummy_Use_Flag_H264[loop] = H264_OFF;
	}
	gSdram_Map_Movie_Record.ribery_dummy_yc.unused_area_addr = gSdram_Map_Movie_Record.sdram_map_base_information.ribery_dummy_top_addr;
	
	return result;
}

/**
 *	@brief		Get TS Work0 address.\n
 *	@param		None
 *	@return		TS Work0 address
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_TS_Work0( VOID )
{
	return gSdram_Map_Movie_Record.ts_work0;
}

/**
 *	@brief		Get TS Work1 address.\n
 *	@param		None
 *	@return		TS Work1 address
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_TS_Work1( VOID )
{
	return gSdram_Map_Movie_Record.ts_work1;
}


/**
 *	@brief		Get Audio Stream address.\n
 *	@param		None
 *	@return		Audio Stream address
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_Audio_Stream0( VOID )
{
	return gSdram_Map_Movie_Record.audio_stream0.addr[ gSdram_Map_Movie_Record.audio_stream0.index ];
}

/**
 *	@brief		Get Audio Stream address.\n
 *	@param		None
 *	@return		Audio Stream address
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_Audio_Stream1( VOID )
{
	return gSdram_Map_Movie_Record.audio_stream1.addr[ gSdram_Map_Movie_Record.audio_stream1.index ];
}

/**
 *	@brief		Change Audio Stream address.\n
 *	@param		None
 *	@return		MOVIE_OK	Normal End
 *	@return		MOVIE_NG	Abnormal End
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Chg_Audio_Stream0( VOID )
{
	++gSdram_Map_Movie_Record.audio_stream0.index ;
	gSdram_Map_Movie_Record.audio_stream0.index = SDRAM_BNK_LIMIT( gSdram_Map_Movie_Record.audio_stream0.index, gSdram_Map_Movie_Record.audio_stream0.max_bank );
}

/**
 *	@brief		Change Audio Stream address.\n
 *	@param		None
 *	@return		MOVIE_OK	Normal End
 *	@return		MOVIE_NG	Abnormal End
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Chg_Audio_Stream1( VOID )
{
	++gSdram_Map_Movie_Record.audio_stream1.index ;
	gSdram_Map_Movie_Record.audio_stream1.index = SDRAM_BNK_LIMIT( gSdram_Map_Movie_Record.audio_stream1.index, gSdram_Map_Movie_Record.audio_stream1.max_bank );
}

/**
 *	@brief		Get Audio Max Bank.\n
 *	@param		None
 *	@return		Audio Max Bank
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_Audio_Stream0_Max_Bank( VOID )
{
	return gSdram_Map_Movie_Record.audio_stream0.max_bank;
}

/**
 *	@brief		Get Audio Max Bank.\n
 *	@param		None
 *	@return		Audio Max Bank
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_Audio_Stream1_Max_Bank( VOID )
{
	return gSdram_Map_Movie_Record.audio_stream1.max_bank;
}
/*------------------------------------------------------------------*/
/* H265 Buffer Ctrl													*/
/*------------------------------------------------------------------*/
/**
 *	@brief		Get H.265 Remap Config.\n
 *	@param		None
 *	@return		H.265 Remap Config
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Get_H265_Remap_Config( T_SDRAM_MAP_MOVIE_RECORD_SIRIUS_REMAP_CONFIG* remap_config )
{
	memcpy( remap_config, &gSdram_Map_Movie_Record.sirius_remap_config, sizeof(T_SDRAM_MAP_MOVIE_RECORD_SIRIUS_REMAP_CONFIG) );
	return;
}

/**
 *	@brief		Get H.265 Code address.\n
 *	@param		None
 *	@return		H.265 Code address
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_H265_Code( VOID )
{
	return gSdram_Map_Movie_Record.sirius_code_addr;
}

/**
 *	@brief		Get H.265 Work address.\n
 *	@param		None
 *	@return		H.265 Work address
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_H265_Work( VOID )
{
	return gSdram_Map_Movie_Record.sirius_work_addr;
}

/**
 *	@brief		Get H.265 CH address.\n
 *	@param		None
 *	@return		H.265 CH address
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Get_H265_Map( E_H265_IF_STREAM_CH stream_ch, T_SDRAM_MAP_MOVIE_RECORD_SIRIUS* map_sirius )
{
	memcpy( map_sirius, &gSdram_Map_Movie_Record.sirius[stream_ch], sizeof(T_SDRAM_MAP_MOVIE_RECORD_SIRIUS) );
	return;
}

/**
 *	@brief		Get YC dummy address.\n
 *	@param		None
 *	@return		YC dummy address
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Get_H265_Vin_Dummy( E_H265_IF_STREAM_CH stream_ch, E_H265_IF_IMAGE_SIZE size, T_H265_IF_GET_IMAGE_ADDR_PARAM* addr_param )
{

#ifndef DEBUG_ON_PC
	switch( size ){
		case E_H265_IF_IMAGE_SIZE_4000_3000 :
		case E_H265_IF_IMAGE_SIZE_4096_2160 :
		case E_H265_IF_IMAGE_SIZE_4096_2048 :
		case E_H265_IF_IMAGE_SIZE_3840_2160 :
		case E_H265_IF_IMAGE_SIZE_3840_1920 :
		case E_H265_IF_IMAGE_SIZE_2704_2028 :
		case E_H265_IF_IMAGE_SIZE_2704_1520 :
		case E_H265_IF_IMAGE_SIZE_2560_1920 :
		case E_H265_IF_IMAGE_SIZE_2560_1440 :
		case E_H265_IF_IMAGE_SIZE_1920_1440 :
		case E_H265_IF_IMAGE_SIZE_1920_1080 :
		case E_H265_IF_IMAGE_SIZE_1504_1504 :
		case E_H265_IF_IMAGE_SIZE_1440_1080 :
		case E_H265_IF_IMAGE_SIZE_1440_720 :
		case E_H265_IF_IMAGE_SIZE_1280_960 :
		case E_H265_IF_IMAGE_SIZE_1280_720 :
		case E_H265_IF_IMAGE_SIZE_960_540 :
		case E_H265_IF_IMAGE_SIZE_960_480 :
		case E_H265_IF_IMAGE_SIZE_864_480 :
		case E_H265_IF_IMAGE_SIZE_848_480 :
		case E_H265_IF_IMAGE_SIZE_800_600 :
		case E_H265_IF_IMAGE_SIZE_640_480 :
		case E_H265_IF_IMAGE_SIZE_640_360 :
		case E_H265_IF_IMAGE_SIZE_480_270 :
		case E_H265_IF_IMAGE_SIZE_432_240 :
		case E_H265_IF_IMAGE_SIZE_320_240 :
		case E_H265_IF_IMAGE_SIZE_720_480 :
		case E_H265_IF_IMAGE_SIZE_720_576 :
		case E_H265_IF_IMAGE_SIZE_1024_768 :
		case E_H265_IF_IMAGE_SIZE_1280_768 :
		case E_H265_IF_IMAGE_SIZE_1280_1024:
		case E_H265_IF_IMAGE_SIZE_1600_896:
		case E_H265_IF_IMAGE_SIZE_1024_576:
		case E_H265_IF_IMAGE_SIZE_800_448:
		case E_H265_IF_IMAGE_SIZE_352_288:
		case E_H265_IF_IMAGE_SIZE_2592_1944:
		case E_H265_IF_IMAGE_SIZE_2304_1296:
		case E_H265_IF_IMAGE_SIZE_1920_1280:
		case E_H265_IF_IMAGE_SIZE_640_512:
		case E_H265_IF_IMAGE_SIZE_384_288:
			addr_param->g_width = 4096;
			break;
		case E_H265_IF_IMAGE_SIZE_3000_3000 :
			addr_param->g_width = 3072;
			break;
		default:
			addr_param->g_width = 4096;
			break;
	}
	
	addr_param->y_addr		= (unsigned long)NULL;
	addr_param->c_addr		= (unsigned long)NULL;
	addr_param->y_cmp_addr	= (unsigned long)NULL;
	addr_param->c_cmp_addr	= (unsigned long)NULL;
	addr_param->dummy_flg	= TRUE;
	
	return;
#else
	static unsigned char dummy_img_y[4000 * 2000];
	static unsigned char dummy_img_c[4000 * 2000];
	addr_param->y_addr = (unsigned long)dummy_img_y;
	addr_param->c_addr = (unsigned long)dummy_img_c;
	addr_param->dummy_flg	= TRUE;
	return;
#endif
}

/**
 *	@brief		Get H.265 input address.\n
 *	@param		i:idx		FL Bank Empty Idx.
 *	@return		Sirius input address
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Get_H265_Vin( UINT32 stream_ch, UINT32 idx, T_H265_IF_GET_IMAGE_ADDR_PARAM* addr_param )
{
	if( idx >= gSdram_Map_Movie_Record.sirius[stream_ch].yc.max_bank ){
#ifndef DEBUG_ON_PC
		BF_Debug_Print_Error(("Sdram_Map_Movie_Record_Get_H265_Vin: index Error. idx=(%d)\n", idx));
#else
		printf("Sdram_Map_Movie_Record_Get_H265_Vin: index Error. idx=(%d)\n", idx);
#endif
		return;
	}
	
	addr_param->y_addr		= gSdram_Map_Movie_Record.sirius[stream_ch].yc.addr[idx].y;
	addr_param->c_addr		= gSdram_Map_Movie_Record.sirius[stream_ch].yc.addr[idx].c;
	addr_param->y_cmp_addr	= gSdram_Map_Movie_Record.sirius[stream_ch].cmprs.addr[idx].y;
	addr_param->c_cmp_addr	= gSdram_Map_Movie_Record.sirius[stream_ch].cmprs.addr[idx].c;
	addr_param->g_width		= gSdram_Map_Movie_Record.sirius[stream_ch].yc.g_width;
	addr_param->dummy_flg	= FALSE;
	
	return;
}

/**
 *	@brief		Get H.265 Stream Work address.\n
 *	@param		None
 *	@return		H.265 Stream Work address
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_H265_Stream_Work( UINT32 stream_ch, UINT32* work_size )
{
	*work_size = gSdram_Map_Movie_Record.sirius[stream_ch].veb_work_size;
	
	return gSdram_Map_Movie_Record.sirius[stream_ch].veb_work;
}

/**
 *	@brief		Get H.265 Sirius Stream Start address.\n
 *	@param		None
 *	@return		H.265 Sirius Stream Start address
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_H265_Stream_Start_Address( UINT32 stream_ch )
{
	return gSdram_Map_Movie_Record.sirius[stream_ch].veb.start_addr;
}

/**
 *	@brief		Get H.265 Sirius Stream End address.\n
 *	@param		None
 *	@return		H.264 Ribery Stream0 End address
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_H265_Stream_End_Address( UINT32 stream_ch )
{
	return gSdram_Map_Movie_Record.sirius[stream_ch].veb.end_addr;
}

/**
 *	@brief		Get H265 Map YC Information.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Get_H265_Map_YC_Information( E_H265_IF_STREAM_CH stream_ch, T_SDRAM_MAP_MOVIE_RECORD_SIRIUS_YC* map_yc_information )
{
	memcpy( map_yc_information, &gSdram_Map_Movie_Record.sirius[stream_ch].yc, sizeof(T_SDRAM_MAP_MOVIE_RECORD_SIRIUS_YC) );
	return;
}

/**
 *	@brief		Set H265 Map YC Information.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Set_H265_Map_YC_Information( E_H265_IF_STREAM_CH stream_ch, UINT32 map_mode, T_SDRAM_MAP_MOVIE_RECORD_SIRIUS_YC* map_yc_information )
{
	gSdram_Map_Movie_Record.sirius[stream_ch].map_mode = map_mode;
	memcpy( &gSdram_Map_Movie_Record.sirius[stream_ch].yc, map_yc_information, sizeof(T_SDRAM_MAP_MOVIE_RECORD_SIRIUS_YC) );
	return;
}
/*------------------------------------------------------------------*/
/* H264 Buffer Ctr													*/
/*------------------------------------------------------------------*/
/**
 *	@brief		Get H.264 Stream Work0 address.\n
 *	@param		None
 *	@return		H.264 Stream Work address
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_H264_Stream_Work0( VOID )
{
	return gSdram_Map_Movie_Record.ribery_stream_work.addr[ 0 ];
}

/**
 *	@brief		Get H.264 Stream Work0 address.\n
 *	@param		None
 *	@return		H.264 Stream Work address
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_H264_Stream_Work1( VOID )
{
	return gSdram_Map_Movie_Record.ribery_stream_work.addr[ 1 ];
}

/**
 *	@brief		Get H.264 Stream Work Size.\n
 *	@param		None
 *	@return		H.264 Stream Work address
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_H264_Stream_Work_Size( VOID )
{
	return SDRAM_SIZ_MOVIE_RECORD0_RIBERY_STREAM_WORK;
}


/**
 *	@brief		Get H.264 Ribery Stream0 Start address.\n
 *	@param		None
 *	@return		H.264 Ribery Stream0 Start address
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_Start_Address( VOID )
{
	return gSdram_Map_Movie_Record.ribery_stream0.start_addr;
}

/**
 *	@brief		Get H.264 Ribery Stream0 End address.\n
 *	@param		None
 *	@return		H.264 Ribery Stream0 End address
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_End_Address( VOID )
{
	return gSdram_Map_Movie_Record.ribery_stream0.end_addr;
}

/**
 *	@brief		Get H.264 Ribery Stream0 Start address.\n
 *	@param		None
 *	@return		H.264 Ribery Stream0 Start address
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_H264_Ribery_Stream1_Start_Address( VOID )
{
	return gSdram_Map_Movie_Record.ribery_stream1.start_addr;
}

/**
 *	@brief		Get H.264 Ribery Stream0 End address.\n
 *	@param		None
 *	@return		H.264 Ribery Stream0 End address
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_H264_Ribery_Stream1_End_Address( VOID )
{
	return gSdram_Map_Movie_Record.ribery_stream1.end_addr;
}

/**
 *	@brief		Get H.264 Ribery Work0 address.\n
 *	@param		None
 *	@return		H.264 Ribery Work0 address
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_H264_Ribery_Work0( VOID )
{
	return gSdram_Map_Movie_Record.ribery_work0_addr;
}

/**
 *	@brief		Get H.264 Ribery Work1 address.\n
 *	@param		None
 *	@return		H.264 Ribery Work1 address
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_H264_Ribery_Work1( VOID )
{
	return gSdram_Map_Movie_Record.ribery_work1_addr;
}

/**
 *	@brief		Get H.264 Ribery Work2 address.\n
 *	@param		None
 *	@return		H.264 Ribery Work2 address
 *	@note		None
 *	@attention	None
 */
ULONG Sdram_Map_Movie_Record_Get_H264_Ribery_Work2( VOID )
{
	return gSdram_Map_Movie_Record.ribery_work2_addr;
}


/**
 *	@brief		Get YC dummy address.\n
 *	@param		None
 *	@return		YC dummy address
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Get_H264_Vin_Dummy( E_H264_IF_STREAM_CH stream_ch, E_H264_IF_IMAGE_SIZE size, T_H264_IF_GET_IMAGE_ADDR_PARAM* addr_param )
{

#ifndef DEBUG_ON_PC
	USHORT bank_width;
	UINT32 y_bank, c_bank;
	
	if( gSdram_Map_Movie_Record_Dummy_Use_Flag_H264[stream_ch] == H264_OFF ){
		
		gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[stream_ch].bank_size = size;
		
		switch( size ){
				case E_H264_IF_IMAGE_SIZE_4096_2304 :
					bank_width	= 4096;
					y_bank		= 0x00900000;
					c_bank		= 0x00480000;
					break;
				case E_H264_IF_IMAGE_SIZE_4096_2160 :
					bank_width	= 4096;
					y_bank		= 0x00880000;
					c_bank		= 0x00440000;
					break;
				case E_H264_IF_IMAGE_SIZE_4096_2048 :
					bank_width	= 4096;
					y_bank		= 0x00800000;
					c_bank		= 0x00400000;
					break;
				case E_H264_IF_IMAGE_SIZE_3840_2160 :
					bank_width	= 3840;
					y_bank		= 0x007F8000;
					c_bank		= 0x003FC000;
					break;
                case E_H264_IF_IMAGE_SIZE_3840_1920 :
					bank_width	= 4096;
					y_bank		= 0x00880000;
					c_bank		= 0x00440000;
					break;
				case E_H264_IF_IMAGE_SIZE_3000_3000 :
					bank_width	= 3072;
					y_bank		= 0x008D0000;
					c_bank		= 0x00468000;
					break;					
				case E_H264_IF_IMAGE_SIZE_2704_2028 :
					bank_width	= 2816;
					y_bank		= 0x00580000;
					c_bank		= 0x002C0000;
					break;
				case E_H264_IF_IMAGE_SIZE_2704_1520 :
					bank_width	= 2816;
					y_bank		= 0x00420000;
					c_bank		= 0x00210000;
					break;
				case E_H264_IF_IMAGE_SIZE_2560_1440 :
					bank_width	= 2560;
					y_bank		= 0x00384000;
					c_bank		= 0x001CC000;
					break;
				case E_H264_IF_IMAGE_SIZE_1920_1440 :
					bank_width	= 2048;
					y_bank		= 0x002D0000;
					c_bank		= 0x00170000;
					break;
				case E_H264_IF_IMAGE_SIZE_1920_1080 :
					bank_width	= 2048;
					y_bank		= 0x00220000;
					c_bank		= 0x00110000;
					break;
				case E_H264_IF_IMAGE_SIZE_1920_960 :
					bank_width	= 2048;
					y_bank		= 0x001E0000;
					c_bank		= 0x000F0000;
					break;
				case E_H264_IF_IMAGE_SIZE_1440_1080 :
					bank_width	= 1536;
					y_bank		= 0x00198000;
					c_bank		= 0x000CC000;
					break;
				case E_H264_IF_IMAGE_SIZE_1440_720 :
					bank_width	= 1536;
					y_bank		= 0x00114000;
					c_bank		= 0x00090000;
					break;
				case E_H264_IF_IMAGE_SIZE_1280_1024 :
					bank_width	= 1280;
					y_bank		= 0x00140000;
					c_bank		= 0x000A0000;
					break;
				case E_H264_IF_IMAGE_SIZE_1280_960 :
					bank_width	= 1280;
					y_bank		= 0x0012C000;
					c_bank		= 0x00098000;
					break;
				case E_H264_IF_IMAGE_SIZE_1280_768:
					bank_width	= 1280;
					y_bank		= 0x000F0000;
					c_bank		= 0x00078000;
					break;
				case E_H264_IF_IMAGE_SIZE_1280_720 :
					bank_width	= 1280;
					y_bank		= 0x000E8000;
					c_bank		= 0x00078000;
					break;
				case E_H264_IF_IMAGE_SIZE_1024_768 :
					bank_width	= 1024;
					y_bank		= 0x000C0000;
					c_bank		= 0x00060000;
					break;
				case E_H264_IF_IMAGE_SIZE_864_480 :
					bank_width	= 1024;
					y_bank		= 0x00078000;
					c_bank		= 0x00040000;
					break;
				case E_H264_IF_IMAGE_SIZE_800_600 :
					bank_width	= 1024;
					y_bank		= 0x00078000;
					c_bank		= 0x00040000;
					break;
				case E_H264_IF_IMAGE_SIZE_960_540 :
					bank_width	= 1024;
					y_bank		= 0x00080000;
					c_bank		= 0x00044000;
					break;
				case E_H264_IF_IMAGE_SIZE_960_480 :
					bank_width	= 1024;
					y_bank		= 0x00078000;
					c_bank		= 0x00040000;
					break;
				case E_H264_IF_IMAGE_SIZE_848_480 :
					bank_width	= 1024;
					y_bank		= 0x00078000;
					c_bank		= 0x00040000;
					break;
				case E_H264_IF_IMAGE_SIZE_720_576 :
					bank_width	= 768;
					y_bank		= 0x0006C000;
					c_bank		= 0x00036000;
					break;
				case E_H264_IF_IMAGE_SIZE_720_480 :
					bank_width	= 768;
					y_bank		= 0x0005C000;
					c_bank		= 0x00030000;
					break;
				case E_H264_IF_IMAGE_SIZE_640_480 :
					bank_width	= 768;
					y_bank		= 0x0005C000;
					c_bank		= 0x00030000;
					break;
				case E_H264_IF_IMAGE_SIZE_640_360 :
					bank_width	= 768;
					y_bank		= 0x00048000;
					c_bank		= 0x00024000;
					break;
				case E_H264_IF_IMAGE_SIZE_480_270 :
					bank_width	= 768;
					y_bank		= 0x24000;
					c_bank		= 0x14000;
					break;
				case E_H264_IF_IMAGE_SIZE_432_240 :
					bank_width	= 512;
					y_bank		= 0x00020000;
					c_bank		= 0x00010000;
					break;
				case E_H264_IF_IMAGE_SIZE_320_240 :
					bank_width	= 512;
					y_bank		= 0x00020000;
					c_bank		= 0x00010000;
					break;
				case E_H264_IF_IMAGE_SIZE_1600_896:
					bank_width	= 1792;
					y_bank		= 0x00188000;
					c_bank		= 0x000C4000;
					break;
				case E_H264_IF_IMAGE_SIZE_1024_576:
					bank_width	= 1024;
					y_bank		= 0x00090000;
					c_bank		= 0x00048000;
					break;
				case E_H264_IF_IMAGE_SIZE_800_448:
					bank_width	= 1024;
					y_bank		= 0x00070000;
					c_bank		= 0x00038000;
					break;
				case E_H264_IF_IMAGE_SIZE_352_288:
					bank_width	= 512;
					y_bank		= 0x00024000;
					c_bank		= 0x00012000;
					break;
				case E_H264_IF_IMAGE_SIZE_2592_1944:
					bank_width	= 3072;
					y_bank		= 0x005B2000;
					c_bank		= 0x002DA000;
					break;
				case E_H264_IF_IMAGE_SIZE_2304_1296:
					bank_width	= 2560;
					y_bank		= 0x0032A000;
					c_bank		= 0x00196000;
					break;
				case E_H264_IF_IMAGE_SIZE_1920_1280:
					bank_width	= 2048;
					y_bank		= 0x00280000;
					c_bank		= 0x00140000;
					break;
				case E_H264_IF_IMAGE_SIZE_640_512:
					bank_width	= 768;
					y_bank		= 0x00060000;
					c_bank		= 0x00030000;
					break;
				case E_H264_IF_IMAGE_SIZE_384_288:
					bank_width	= 512;
					y_bank		= 0x00024000;
					c_bank		= 0x00012000;
					break;
				default :
					bank_width	= 4096;
					y_bank		= 0x00880000;
					c_bank		= 0x00440000;
					break;
		}
		
		gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[stream_ch].addr.y = gSdram_Map_Movie_Record.ribery_dummy_yc.unused_area_addr;
		gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[stream_ch].addr.c = gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[stream_ch].addr.y + y_bank;
		
		gSdram_Map_Movie_Record.ribery_dummy_yc.unused_area_addr = gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[stream_ch].addr.c + c_bank;
		if( gSdram_Map_Movie_Record.ribery_dummy_yc.unused_area_addr > gSdram_Map_Movie_Record.sdram_map_base_information.ribery_dummy_bottom_addr ){
			/// Overlap
			BF_Debug_Print_Error(("Sdram_Map_Movie_Record_Get_H264_Vin_Dummy: dummy area overlap error.\n"));
		}
		gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[stream_ch].g_width = bank_width;
		
		gSdram_Map_Movie_Record_Dummy_Use_Flag_H264[stream_ch] = H264_ON;
		
		addr_param->y_addr  = gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[stream_ch].addr.y;
		addr_param->c_addr  = gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[stream_ch].addr.c;
		addr_param->g_width = gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[stream_ch].g_width;
	}
	else{
		if( gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[stream_ch].bank_size != size ){
			BF_Debug_Print_Error(("Sdram_Map_Movie_Record_Get_H264_Vin_Dummy: dummy size unmatch.\n"));
		}
		
		addr_param->y_addr  = gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[stream_ch].addr.y;
		addr_param->c_addr  = gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[stream_ch].addr.c;
		addr_param->g_width = gSdram_Map_Movie_Record.ribery_dummy_yc.yc_dummy[stream_ch].g_width;
	}
	
	addr_param->dummy_flg = TRUE;
	
	return;

#else
	static unsigned char dummy_img_y[4000 * 2000];
	static unsigned char dummy_img_c[4000 * 2000];
	addr_param->y_addr = (unsigned long)dummy_img_y;
	addr_param->c_addr = (unsigned long)dummy_img_c;
	addr_param->dummy_flg	= TRUE;
	return;
#endif
}

/**
 *	@brief		Get Ribery input address.\n
 *	@param		i:idx		FL Bank Empty Idx.
 *	@return		Ribery input address
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Get_H264_Vin( UINT32 stream_ch, UINT32 idx, T_H264_IF_GET_IMAGE_ADDR_PARAM* addr_param )
{
	if( idx >= D_RIBERY_YC_BANK_MAX ){
#ifndef DEBUG_ON_PC
		BF_Debug_Print_Error(("Sdram_Map_Movie_Record_Get_H264_Vin: index Error. idx=(%d)\n", idx));
#else
		printf("Sdram_Map_Movie_Record_Get_H264_Vin: index Error. idx=(%d)\n", idx);
#endif
		return;
	}
	
	addr_param->y_addr		= gSdram_Map_Movie_Record.ribery[stream_ch].yc.addr[idx].y;
	addr_param->c_addr		= gSdram_Map_Movie_Record.ribery[stream_ch].yc.addr[idx].c;
	addr_param->g_width		= gSdram_Map_Movie_Record.ribery[stream_ch].yc.g_width;
	addr_param->dummy_flg	= FALSE;
	return;
}

/**
 *	@brief		Get H264 Map Reduction Information.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Get_H264_Map_Reduction_Information( E_H264_IF_STREAM_CH stream_ch, T_SDRAM_MAP_MOVIE_RECORD_RIBERY_REDUCTION* map_reduction_information )
{
	memcpy( map_reduction_information, &gSdram_Map_Movie_Record.ribery[stream_ch].reduction, sizeof(T_SDRAM_MAP_MOVIE_RECORD_RIBERY_REDUCTION) );
	return;
}

/**
 *	@brief		Set H264 Map Reduction Information.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Set_H264_Map_Reduction_Information( E_H264_IF_STREAM_CH stream_ch, T_SDRAM_MAP_MOVIE_RECORD_RIBERY_REDUCTION* map_reduction_information )
{
	memcpy( &gSdram_Map_Movie_Record.ribery[stream_ch].reduction, map_reduction_information, sizeof(T_SDRAM_MAP_MOVIE_RECORD_RIBERY_REDUCTION) );
	return;
}

/**
 *	@brief		Get H264 Map Local_Dec Information.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Get_H264_Map_Local_Dec_Information( E_H264_IF_STREAM_CH stream_ch, T_SDRAM_MAP_MOVIE_RECORD_RIBERY_LOCALDEC* map_local_dec_information )
{
	memcpy( map_local_dec_information, &gSdram_Map_Movie_Record.ribery[stream_ch].local_dec, sizeof(T_SDRAM_MAP_MOVIE_RECORD_RIBERY_LOCALDEC) );
	return;
}

/**
 *	@brief		Set H264 Map Local_Dec Information.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Set_H264_Map_Local_Dec_Information( E_H264_IF_STREAM_CH stream_ch, T_SDRAM_MAP_MOVIE_RECORD_RIBERY_LOCALDEC* map_local_dec_information )
{
	memcpy( &gSdram_Map_Movie_Record.ribery[stream_ch].local_dec, map_local_dec_information, sizeof(T_SDRAM_MAP_MOVIE_RECORD_RIBERY_LOCALDEC) );
	return;
}


/**
 *	@brief		Get H264 Map YC Information.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Get_H264_Map_YC_Information( E_H264_IF_STREAM_CH stream_ch, T_SDRAM_MAP_MOVIE_RECORD_RIBERY_YC* map_yc_information )
{
	memcpy( map_yc_information, &gSdram_Map_Movie_Record.ribery[stream_ch].yc, sizeof(T_SDRAM_MAP_MOVIE_RECORD_RIBERY_YC) );
	return;
}

/**
 *	@brief		Set H264 Map YC Information.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Set_H264_Map_YC_Information( E_H264_IF_STREAM_CH stream_ch, T_SDRAM_MAP_MOVIE_RECORD_RIBERY_YC* map_yc_information )
{
	memcpy( &gSdram_Map_Movie_Record.ribery[stream_ch].yc, map_yc_information, sizeof(T_SDRAM_MAP_MOVIE_RECORD_RIBERY_YC) );
	return;
}

/**
 *	@brief		Get Map Base Information.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Get_Map_Base_Information( T_SDRAM_MAP_MOVIE_RECORD_BASE_INFORMATION* map_base_information )
{
	memcpy( map_base_information, &gSdram_Map_Movie_Record.sdram_map_base_information, sizeof(T_SDRAM_MAP_MOVIE_RECORD_BASE_INFORMATION) );
	return;
}

/**
 *	@brief		Get Map Information.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Get_Map_Information( T_SDRAM_MAP_MOVIE_RECORD_INFORMATION* map_information )
{
	memcpy( map_information, &gSdram_Map_Movie_Record.sdram_map_information, sizeof(T_SDRAM_MAP_MOVIE_RECORD_INFORMATION) );
	return;
}

/**
 *	@brief		Set H264 Map Base Information.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Set_H264_Map_Information( T_SDRAM_MAP_MOVIE_RECORD_INFORMATION* map_information )
{
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_count		= map_information->ribery_map_count;
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_size[0]	= map_information->ribery_map_size[0];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_size[1]	= map_information->ribery_map_size[1];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_size[2]	= map_information->ribery_map_size[2];
	
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_reduction_total_size[0]	= map_information->ribery_map_reduction_total_size[0];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_reduction_total_size[1]	= map_information->ribery_map_reduction_total_size[1];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_reduction_total_size[2]	= map_information->ribery_map_reduction_total_size[2];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_reduction_top_addr[0]		= map_information->ribery_map_reduction_top_addr[0];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_reduction_top_addr[1]		= map_information->ribery_map_reduction_top_addr[1];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_reduction_top_addr[2]		= map_information->ribery_map_reduction_top_addr[2];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_reduction_bottom_addr[0]	= map_information->ribery_map_reduction_bottom_addr[0];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_reduction_bottom_addr[1]	= map_information->ribery_map_reduction_bottom_addr[1];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_reduction_bottom_addr[2]	= map_information->ribery_map_reduction_bottom_addr[2];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_reduction_free_size		= map_information->ribery_map_reduction_free_size;
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_reduction_use				= map_information->ribery_map_reduction_use;
	
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_localdec_total_size[0]		= map_information->ribery_map_localdec_total_size[0];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_localdec_total_size[1]		= map_information->ribery_map_localdec_total_size[1];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_localdec_total_size[2]		= map_information->ribery_map_localdec_total_size[2];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_localdec_top_addr[0]		= map_information->ribery_map_localdec_top_addr[0];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_localdec_top_addr[1]		= map_information->ribery_map_localdec_top_addr[1];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_localdec_top_addr[2]		= map_information->ribery_map_localdec_top_addr[2];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_localdec_bottom_addr[0]	= map_information->ribery_map_localdec_bottom_addr[0];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_localdec_bottom_addr[1]	= map_information->ribery_map_localdec_bottom_addr[1];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_localdec_bottom_addr[2]	= map_information->ribery_map_localdec_bottom_addr[2];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_localdec_free_size			= map_information->ribery_map_localdec_free_size;
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_localdec_use				= map_information->ribery_map_localdec_use;
	
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_yc_total_size[0]	= map_information->ribery_map_yc_total_size[0];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_yc_total_size[1]	= map_information->ribery_map_yc_total_size[1];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_yc_total_size[2]	= map_information->ribery_map_yc_total_size[2];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_yc_top_addr[0]		= map_information->ribery_map_yc_top_addr[0];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_yc_top_addr[1]		= map_information->ribery_map_yc_top_addr[1];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_yc_top_addr[2]		= map_information->ribery_map_yc_top_addr[2];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_yc_bottom_addr[0]	= map_information->ribery_map_yc_bottom_addr[0];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_yc_bottom_addr[1]	= map_information->ribery_map_yc_bottom_addr[1];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_yc_bottom_addr[2]	= map_information->ribery_map_yc_bottom_addr[2];
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_yc_free_size		= map_information->ribery_map_yc_free_size;
	gSdram_Map_Movie_Record.sdram_map_information.ribery_map_yc_use				= map_information->ribery_map_yc_use;
	
	return;
}

/**
 *	@brief		Set H265 Map Base Information.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Record_Set_H265_Map_Information( T_SDRAM_MAP_MOVIE_RECORD_INFORMATION* map_information )
{
	gSdram_Map_Movie_Record.sdram_map_information.sirius_map_count		= map_information->sirius_map_count;
	gSdram_Map_Movie_Record.sdram_map_information.sirius_map_size[0]	= map_information->sirius_map_size[0];
	gSdram_Map_Movie_Record.sdram_map_information.sirius_map_size[1]	= map_information->sirius_map_size[1];
	return;
}

ULONG Sdram_Map_Movie_Record_Get_H265_Temp_Addr_Y( UCHAR ch )
{
	return SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_Y_0;
}

ULONG Sdram_Map_Movie_Record_Get_H265_Temp_Size_Y( UCHAR ch )
{
	return SDRAM_SIZ_MOVIE_RECORD0_SIRIUS_0_Y;
}

ULONG Sdram_Map_Movie_Record_Get_H265_Temp_Addr_C( UCHAR ch )
{
	return SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_C_0;
}

ULONG Sdram_Map_Movie_Record_Get_H265_Temp_Size_C( UCHAR ch )
{
	return SDRAM_SIZ_MOVIE_RECORD0_SIRIUS_0_C;
}

ULONG Sdram_Map_Movie_Record_Get_H265_Temp_Addr_CMP( UCHAR ch )
{
	return SDRAM_ADR_MOVIE_RECORD0_SIRIUS_0_CMPRS_0;
}

ULONG Sdram_Map_Movie_Record_Get_H265_Temp_Size_CMP( UCHAR ch )
{
	return SDRAM_SIZ_MOVIE_RECORD0_SIRIUS_0_CMPRS;
}

#if 0	// for Debug
VOID Sdram_Map_Movie_Record_Check_Address( VOID )
{
	ULONG	check_loop;
	ULONG	check_loop_stream_ch;
	ULONG	check_address;
	ULONG	check_y_address;
	ULONG	check_c_address;
	USHORT	check_g_width;
	
	// Audio Stream
#ifndef DEBUG_ON_PC
	BF_Debug_Print_Information(("audio stream max bank[%u]\n", Sdram_Map_Movie_Record_Get_Audio_Stream0_Max_Bank() ));
#else
	printf("audio stream max bank[%u]\n", Sdram_Map_Movie_Record_Get_Audio_Stream0_Max_Bank() );
#endif
	
	for( check_loop=0; check_loop<D_AUDIO_STREAM_BANK_MAX+2; check_loop++ ){
		check_address = Sdram_Map_Movie_Record_Get_Audio_Stream0();
#ifndef DEBUG_ON_PC
		BF_Debug_Print_Information(("audio stream addr[%#08x]\n", check_address ));
#else
		printf("audio stream addr[%#08x]\n", check_address );
#endif
		Sdram_Map_Movie_Record_Chg_Audio_Stream0();
	}
	OS_User_Dly_Tsk( 200 );
	
	// H.264 Stream Work
	
#ifndef DEBUG_ON_PC
	BF_Debug_Print_Information(("H.264 Stream Work0 addr [%#08x]\n", Sdram_Map_Movie_Record_Get_H264_Stream_Work0() ));
#else
	printf("H.264 Stream Work0 addr [%#08x]\n", Sdram_Map_Movie_Record_Get_H264_Stream_Work0() );
#endif
#ifndef DEBUG_ON_PC
	BF_Debug_Print_Information(("H.264 Stream Work1 addr [%#08x]\n", Sdram_Map_Movie_Record_Get_H264_Stream_Work1() ));
#else
	printf("H.264 Stream Work1 addr [%#08x]\n", Sdram_Map_Movie_Record_Get_H264_Stream_Work1() );
#endif
#ifndef DEBUG_ON_PC
	BF_Debug_Print_Information(("H.264 Stream Work size [%u]\n", Sdram_Map_Movie_Record_Get_H264_Stream_Work_Size() ));
#else
	printf("H.264 Stream Work size [%u]\n", Sdram_Map_Movie_Record_Get_H264_Stream_Work_Size() );
#endif
	
	// H.264 Ribery
#ifndef DEBUG_ON_PC
	BF_Debug_Print_Information(("H.264 Ribery Stream0 Start addr [%#08x]\n", Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_Start_Address() ));
#else
	printf("H.264 Ribery Stream0 Start addr [%#08x]\n", Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_Start_Address() );
#endif
#ifndef DEBUG_ON_PC
	BF_Debug_Print_Information(("H.264 Ribery Stream0 End addr   [%#08x]\n", Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_End_Address() ));
#else
	printf("H.264 Ribery Stream0 End addr   [%#08x]\n", Sdram_Map_Movie_Record_Get_H264_Ribery_Stream0_End_Address() );
#endif
#ifndef DEBUG_ON_PC
	BF_Debug_Print_Information(("H.264 Ribery Stream1 Start addr [%#08x]\n", Sdram_Map_Movie_Record_Get_H264_Ribery_Stream1_Start_Address() ));
#else
	printf("H.264 Ribery Stream1 Start addr [%#08x]\n", Sdram_Map_Movie_Record_Get_H264_Ribery_Stream1_Start_Address() );
#endif
#ifndef DEBUG_ON_PC
	BF_Debug_Print_Information(("H.264 Ribery Stream1 End addr   [%#08x]\n", Sdram_Map_Movie_Record_Get_H264_Ribery_Stream1_End_Address() ));
#else
	printf("H.264 Ribery Stream1 End addr   [%#08x]\n", Sdram_Map_Movie_Record_Get_H264_Ribery_Stream1_End_Address() );
#endif
#ifndef DEBUG_ON_PC
	BF_Debug_Print_Information(("H.264 Ribery Work0 addr [%#08x]\n", Sdram_Map_Movie_Record_Get_H264_Ribery_Work0() ));
#else
	printf("H.264 Ribery Work0 addr [%#08x]\n", Sdram_Map_Movie_Record_Get_H264_Ribery_Work0() );
#endif
#ifndef DEBUG_ON_PC
	BF_Debug_Print_Information(("H.264 Ribery Work1 addr [%#08x]\n", Sdram_Map_Movie_Record_Get_H264_Ribery_Work1() ));
#else
	printf("H.264 Ribery Work1 addr [%#08x]\n", Sdram_Map_Movie_Record_Get_H264_Ribery_Work1() );
#endif
#ifndef DEBUG_ON_PC
	BF_Debug_Print_Information(("H.264 Ribery Work2 addr [%#08x]\n", Sdram_Map_Movie_Record_Get_H264_Ribery_Work2() ));
#else
	printf("H.264 Ribery Work2 addr [%#08x]\n", Sdram_Map_Movie_Record_Get_H264_Ribery_Work2() );
#endif
	
	OS_User_Dly_Tsk( 200 );
	// H.264 YCC
	// dummy
	for( check_loop_stream_ch=0; check_loop_stream_ch<E_H264_IF_STREAM_CH_MAX; check_loop_stream_ch++ ){
		Sdram_Map_Movie_Record_Get_H264_Vin_Dummy( (E_H264_IF_STREAM_CH)check_loop_stream_ch, E_H264_IF_IMAGE_SIZE_4096_2160, &check_y_address, &check_c_address, &check_g_width );
#ifndef DEBUG_ON_PC
		BF_Debug_Print_Information(("H.264 dummy addr Y[%#08x] C[%#08x] ch[%u]\n", check_y_address, check_c_address, check_loop_stream_ch ));
#else
		printf("H.264 dummy addr Y[%#08x] C[%#08x] ch[%u]\n", check_y_address, check_c_address, check_loop_stream_ch );
#endif
	}
	
	OS_User_Dly_Tsk( 200 );
	// YC
	for( check_loop_stream_ch=0; check_loop_stream_ch<E_H264_IF_STREAM_CH_MAX; check_loop_stream_ch++ ){
		for( check_loop=0; check_loop<D_RIBERY_YC_BANK_MAX; check_loop++ ){
			Sdram_Map_Movie_Record_Get_H264_Vin( check_loop_stream_ch, check_loop, &check_y_address, &check_c_address, &check_g_width );
#ifndef DEBUG_ON_PC
			BF_Debug_Print_Information(("H.264 Vin set addr [%#08x] ch[%u] idx[%u]\n", check_y_address, check_loop_stream_ch, check_loop ));
#else
			printf("H.264 Vin set addr [%#08x] ch[%u] idx[%u]\n", check_y_address, check_loop_stream_ch, check_loop );
#endif
		}
	}
	OS_User_Dly_Tsk( 200 );

}
#endif

