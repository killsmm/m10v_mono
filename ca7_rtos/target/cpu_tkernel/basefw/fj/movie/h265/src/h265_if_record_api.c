/**
 * @file		h265_if_record_api.c
 * @brief		This file is a movie record api function.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "h265_if.h"
#include "h265_if_common.h"
#include "h265_movie.h"
#include "sdram_map_movie_record.h"
#include "h265_encode.h"

#ifdef CO_H265_SECTION_ON
//#pragma arm section code="H265_CODE"
//#pragma arm section rodata="H265_CONST"
//#pragma arm section rwdata="H265_INIT"
#endif

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Movie Record Infromation(status and time informations)
static	volatile	E_H265_IF_RECORD_STATUS		gH265_If_Rec_Status = E_H265_IF_RECORD_STATUS_END;

// Movie Condition Informations
static	volatile	T_H265_MOVIE_UI				gH265_If_Rec_UI ;

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */


/*----------------------------------------------------------------------*/
/* Sub Function															*/
/*----------------------------------------------------------------------*/
/**
 * set current status of movie record function
 * @param	status
 * @return	nothing
 */
VOID H265_If_Record_Api_Set_Status( E_H265_IF_RECORD_STATUS status )
{
	gH265_If_Rec_Status = status;
	return;
}

/**
 * get current status of movie record function
 * @param	nothing
 * @return	current status. please refer to E_H265_IF_RECORD_STATUS.
 */
E_H265_IF_RECORD_STATUS H265_If_Record_Api_Get_Status( VOID )
{
	return gH265_If_Rec_Status;
}


/*----------------------------------------------------------------------*/
/* Main API Function													*/
/*----------------------------------------------------------------------*/
/**
 * Initialize.
 * @param	nothing
 * @return	nothing
 */
VOID H265_If_Record_Api_Initialize( VOID )
{
	H265_Movie_Memset( (VOID*)&gH265_If_Rec_UI, 0, sizeof(gH265_If_Rec_UI) );
	return;
}

/**
 * Finalize.
 * @param	nothing
 * @return	nothing
 */
VOID H265_If_Record_Api_Finalize( VOID )
{
	H265_Encode_Finalize();
	return;
}

/**
 * Param Set. (call by "ready")
 * @param	nothing
 * @return	nothing
 */
INT32 H265_If_Record_Api_Param_Set( T_H265_IF_MSG* msg )
{
// --- REMOVE_ES1_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	UINT32					map_mode;
	UINT32					y_bank_size;
	UINT32					c_bank_size;
	UINT32					g_width;
	UINT32					g_lines;
	E_H265_IF_STREAM_CH		stream_ch;
	UINT32					max_bank_num;
	UINT32					y_pointer = 0;
	UINT32					c_pointer = 0;
	UINT32					y_limit_size;
	UINT32					c_limit_size;
	UINT32					i;
	
	T_SDRAM_MAP_MOVIE_RECORD_BASE_INFORMATION	map_base_info;
	
	T_SDRAM_MAP_MOVIE_RECORD_SIRIUS_YC	map_yc;
	
	if( msg == NULL ){
		return H265_PARAM_ERR;
	}
	
	stream_ch = msg->data.pRecordParam->stream_ch;
	memcpy( (VOID *)&gH265_If_Rec_UI.codec_param[stream_ch], msg->data.pRecordParam, sizeof(T_H265_IF_PARAM_RECORD) );
	
	Sdram_Map_Movie_Record_Get_Map_Base_Information( &map_base_info );
	
	switch( msg->data.pRecordParam->size ){
		case E_H265_IF_IMAGE_SIZE_4000_3000:
			g_width					= 4096;
			g_lines					= 3008;
			map_mode				= D_H265_ENC_MAP_MODE_4K3K_NUM;
			break;
		case E_H265_IF_IMAGE_SIZE_4096_2160:
		case E_H265_IF_IMAGE_SIZE_4096_2048:
		case E_H265_IF_IMAGE_SIZE_3840_2160:
		case E_H265_IF_IMAGE_SIZE_3840_1920:
		case E_H265_IF_IMAGE_SIZE_2704_2028:
		case E_H265_IF_IMAGE_SIZE_2704_1520:
		case E_H265_IF_IMAGE_SIZE_2560_1920:
		case E_H265_IF_IMAGE_SIZE_2560_1440:
		case E_H265_IF_IMAGE_SIZE_1920_960:
		case E_H265_IF_IMAGE_SIZE_1440_720:
		case E_H265_IF_IMAGE_SIZE_2592_1944:
		case E_H265_IF_IMAGE_SIZE_2304_1296:
			g_width					= 4096;
			g_lines					= 2176;
			map_mode				= D_H265_ENC_MAP_MODE_4K2K_NUM;
			break;
		case E_H265_IF_IMAGE_SIZE_3000_3000:
			g_width					= 3072;
			g_lines					= 3008;
			map_mode				= D_H265_ENC_MAP_MODE_3K_NUM;
			break;
		case E_H265_IF_IMAGE_SIZE_1920_1440:
		case E_H265_IF_IMAGE_SIZE_1920_1080:
		case E_H265_IF_IMAGE_SIZE_1504_1504:
		case E_H265_IF_IMAGE_SIZE_1440_1080:
		case E_H265_IF_IMAGE_SIZE_1280_1024:
		case E_H265_IF_IMAGE_SIZE_1600_896:
		case E_H265_IF_IMAGE_SIZE_1920_1280:
		//case E_H265_IF_IMAGE_SIZE_1440_720:
			g_width					= 2048;
			g_lines					= 1536;
			map_mode				= 0xFF;
			break;
		case E_H265_IF_IMAGE_SIZE_1280_960:
		case E_H265_IF_IMAGE_SIZE_1280_720:
		case E_H265_IF_IMAGE_SIZE_960_540:
		case E_H265_IF_IMAGE_SIZE_960_480:
		case E_H265_IF_IMAGE_SIZE_864_480:
		case E_H265_IF_IMAGE_SIZE_848_480:
		case E_H265_IF_IMAGE_SIZE_800_600:
		case E_H265_IF_IMAGE_SIZE_640_480:
		case E_H265_IF_IMAGE_SIZE_640_360:
		case E_H265_IF_IMAGE_SIZE_480_270:
		case E_H265_IF_IMAGE_SIZE_432_240:
		case E_H265_IF_IMAGE_SIZE_320_240:
		case E_H265_IF_IMAGE_SIZE_720_576:
		case E_H265_IF_IMAGE_SIZE_720_480:
		case E_H265_IF_IMAGE_SIZE_1280_768:
		case E_H265_IF_IMAGE_SIZE_1024_768:
		case E_H265_IF_IMAGE_SIZE_1024_576:
		case E_H265_IF_IMAGE_SIZE_800_448:
		case E_H265_IF_IMAGE_SIZE_352_288:
		case E_H265_IF_IMAGE_SIZE_640_512:
		case E_H265_IF_IMAGE_SIZE_384_288:
			g_width					= 1280;
			g_lines					= 960;
			map_mode				= D_H265_ENC_MAP_MODE_NHD_NUM;
			break;
		default:
			/* it already checked by BF_Movie_Video_Attr_Set_H265_Config */
			return H265_NG;
	}
	y_bank_size		= (((g_width * g_lines) + D_SIRIUS_BANK_ALIGNMENT - 1) / D_SIRIUS_BANK_ALIGNMENT) * D_SIRIUS_BANK_ALIGNMENT;
	c_bank_size		= (((g_width * g_lines / 2) + D_SIRIUS_BANK_ALIGNMENT - 1) / D_SIRIUS_BANK_ALIGNMENT) * D_SIRIUS_BANK_ALIGNMENT;
	
	y_limit_size = map_base_info.sirius_y_bottom_addr[stream_ch] - map_base_info.sirius_y_top_addr[stream_ch];
	c_limit_size = map_base_info.sirius_c_bottom_addr[stream_ch] - map_base_info.sirius_c_top_addr[stream_ch];
	
	for( max_bank_num=0; max_bank_num < D_SIRIUS_YC_BANK_MAX; max_bank_num++ ){
		
		if( (y_pointer + y_bank_size) > y_limit_size ){
			break;
		}
		
		if( (c_pointer + c_bank_size) > c_limit_size ){
			break;
		}
		
		y_pointer += y_bank_size;
		c_pointer += c_bank_size;
	}
	
	Sdram_Map_Movie_Record_Get_H265_Map_YC_Information( stream_ch, &map_yc );
	
	map_yc.max_bank		= max_bank_num;
	map_yc.g_width		= g_width;
	map_yc.g_lines		= g_lines;
	map_yc.y_bank		= y_bank_size;
	map_yc.c_bank		= c_bank_size;
	
	for( i=0; i<D_SIRIUS_YC_BANK_MAX; i++ ){
		map_yc.addr[i].y	= map_yc.addr[0].y + (y_bank_size * i);
		map_yc.addr[i].c	= map_yc.addr[0].c + (c_bank_size * i);
		if( i >= max_bank_num ){
			map_yc.addr[i].y	= (ULONG)NULL;
			map_yc.addr[i].c	= (ULONG)NULL;
		}
	}
	
	Sdram_Map_Movie_Record_Set_H265_Map_YC_Information( stream_ch, map_mode, &map_yc );
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	UINT32					yc_total_size;
	UINT32					y_bank_size;
	UINT32					c_bank_size;
	UINT32					g_width;
	UINT32					g_y_lines;
	UINT32					g_c_lines;
	UINT32					localdec_total_size;
	UINT32					local_y_bank_size;
	UINT32					local_c_bank_size;
	UINT32					local_width;
	UINT32					local_width_for_c;
	UINT32					local_lines;
	UINT32					local_lines_for_c;
	UINT32					i;
	E_H265_IF_STREAM_CH		stream_ch;
	E_H265_IF_STREAM_CH		first_stream_ch = E_H265_IF_STREAM_CH_MAX;
	UINT32					free1, free2;
	
	T_SDRAM_MAP_MOVIE_RECORD_BASE_INFORMATION	map_base_info;
	T_SDRAM_MAP_MOVIE_RECORD_INFORMATION		map_info;
	T_SDRAM_MAP_MOVIE_RECORD_WAVE				map_wave;
	
	if( msg == NULL ){
		return H265_PARAM_ERR;
	}
	
	stream_ch = msg->data.pRecordParam->stream_ch;
	memcpy( (VOID *)&gH265_If_Rec_UI.codec_param[stream_ch], msg->data.pRecordParam, sizeof(T_H265_IF_PARAM_RECORD) );
	
	Sdram_Map_Movie_Record_Get_Map_Information( &map_info );
	Sdram_Map_Movie_Record_Get_Map_Base_Information( &map_base_info );
	
	g_width					= ((msg->data.pRecordParam->width + D_WAVE_WIDTH_ALIGNMENT - 1) / D_WAVE_WIDTH_ALIGNMENT) * D_WAVE_WIDTH_ALIGNMENT;
	g_y_lines				= ((msg->data.pRecordParam->lines + D_WAVE_LINES_ALIGNMENT - 1) / D_WAVE_LINES_ALIGNMENT) * D_WAVE_LINES_ALIGNMENT;
	g_c_lines				= (((msg->data.pRecordParam->lines/2) + D_WAVE_LINES_ALIGNMENT - 1) / D_WAVE_LINES_ALIGNMENT) * D_WAVE_LINES_ALIGNMENT;
	y_bank_size				= (((g_width * g_y_lines) + D_WAVE_BANK_ALIGNMENT - 1) / D_WAVE_BANK_ALIGNMENT) * D_WAVE_BANK_ALIGNMENT;
	c_bank_size				= (((g_width * g_c_lines) + D_WAVE_BANK_ALIGNMENT - 1) / D_WAVE_BANK_ALIGNMENT) * D_WAVE_BANK_ALIGNMENT;
	yc_total_size			= (y_bank_size + c_bank_size) * D_WAVE_YC_BANK_MAX;
	
	local_width				= (((msg->data.pRecordParam->width + 16) + D_WAVE_LOCALDEC_WIDTH_ALIGNMENT - 1) / D_WAVE_LOCALDEC_WIDTH_ALIGNMENT ) * D_WAVE_LOCALDEC_WIDTH_ALIGNMENT;
	local_lines				= (((msg->data.pRecordParam->lines + 4) + D_WAVE_LOCALDEC_LINES_ALIGNMENT - 1) / D_WAVE_LOCALDEC_LINES_ALIGNMENT ) * D_WAVE_LOCALDEC_LINES_ALIGNMENT;
	local_y_bank_size		= (((local_width * local_lines) + D_WAVE_BANK_ALIGNMENT - 1) / D_WAVE_BANK_ALIGNMENT) * D_WAVE_BANK_ALIGNMENT;
	local_width_for_c		= (((local_width / 2 + 16) + D_WAVE_LOCALDEC_WIDTH_ALIGNMENT - 1) / D_WAVE_LOCALDEC_WIDTH_ALIGNMENT ) * D_WAVE_LOCALDEC_WIDTH_ALIGNMENT;
	local_lines_for_c		= (((local_lines / 2 + 4) + D_WAVE_LOCALDEC_LINES_ALIGNMENT - 1) / D_WAVE_LOCALDEC_LINES_ALIGNMENT ) * D_WAVE_LOCALDEC_LINES_ALIGNMENT;
	local_c_bank_size		= ((((local_width_for_c * local_lines_for_c) + D_WAVE_BANK_ALIGNMENT - 1) / D_WAVE_BANK_ALIGNMENT) * D_WAVE_BANK_ALIGNMENT) * 2;
	localdec_total_size		= (local_y_bank_size + local_c_bank_size) * D_WAVE_LOCALDEC_BANK_MAX;

	
	if( ( localdec_total_size	> map_info.wave_map_localdec_free_size	) ||
		( yc_total_size			> map_info.wave_map_yc_free_size		) ){
		H265_Err_Print(("H265 bank size over error!\n"));
		H265_Err_Print(("local_width:%d, local_lines:%d\n", local_width, local_lines));
		H265_Err_Print(("local_width_for_c:%d, local_lines_for_c:%d\n", local_width_for_c, local_lines_for_c));
		H265_Err_Print(("local_y_bank_size:%d, local_lines:%d\n", local_y_bank_size, local_c_bank_size));
		H265_Err_Print(("localdec_total_size:%d, map_info.wave_map_localdec_free_size:%d\n", localdec_total_size, map_info.wave_map_localdec_free_size));
		H265_Err_Print(("yc_total_size:%d, map_info.wave_map_yc_free_size:%d\n", yc_total_size, map_info.wave_map_yc_free_size));
		return H265_NG;
	}
	
	Sdram_Map_Movie_Record_Get_H265_Map( stream_ch, &map_wave );
	
	map_wave.localdec.max_bank	= D_WAVE_LOCALDEC_BANK_MAX;
	map_wave.localdec.g_width	= local_width;
	map_wave.localdec.g_lines	= local_lines;
	map_wave.localdec.size		= local_y_bank_size + local_c_bank_size;
	map_wave.localdec.addr[0]	= map_info.wave_map_localdec_use;
	for( i=1; i<map_wave.localdec.max_bank; i++ ){
		map_wave.localdec.addr[i]	= map_wave.localdec.addr[i-1] + map_wave.localdec.size;
	}
	
	map_wave.yc.max_bank		= D_WAVE_YC_BANK_MAX;
	map_wave.yc.g_width			= g_width;
	map_wave.yc.g_lines			= g_y_lines;
	map_wave.yc.y_bank			= y_bank_size;
	map_wave.yc.c_bank			= c_bank_size;
	map_wave.yc.addr[0].y		= map_info.wave_map_yc_use;
	map_wave.yc.addr[0].c		= map_wave.yc.addr[0].y + y_bank_size;
	for( i=1; i<map_wave.yc.max_bank; i++ ){
		map_wave.yc.addr[i].y		= map_wave.yc.addr[i-1].c + c_bank_size;
		map_wave.yc.addr[i].c		= map_wave.yc.addr[i].y + y_bank_size;
	}
	
	Sdram_Map_Movie_Record_Set_H265_Map( stream_ch, &map_wave );
	
	map_info.wave_map_count++;
	map_info.wave_map_size[stream_ch]					= msg->data.pRecordParam->size;
	map_info.wave_map_localdec_total_size[stream_ch]	= localdec_total_size;
	map_info.wave_map_localdec_top_addr[stream_ch]		= map_wave.localdec.addr[0];
	map_info.wave_map_localdec_bottom_addr[stream_ch]	= map_wave.localdec.addr[D_WAVE_LOCALDEC_BANK_MAX-1] + map_wave.localdec.size;
	map_info.wave_map_yc_total_size[stream_ch]			= yc_total_size;
	map_info.wave_map_yc_top_addr[stream_ch]			= map_wave.yc.addr[0].y;
	map_info.wave_map_yc_bottom_addr[stream_ch]			= map_wave.yc.addr[D_WAVE_YC_BANK_MAX-1].c + map_wave.yc.c_bank;
	
	if( map_info.wave_map_count == 1 ){
//		 -------- 		 -------- 
//		|        |		|   1    |
//		|        |		|--------|
//		|  free  |	->	|  free  |
//		|        |		|        |
//		|        |		|        |
//		 -------- 		 -------- 
		map_info.wave_map_localdec_use			+= localdec_total_size;
		map_info.wave_map_localdec_free_size	-= localdec_total_size;
		map_info.wave_map_yc_use				+= yc_total_size;
		map_info.wave_map_yc_free_size			-= yc_total_size;
	}
	else if( map_info.wave_map_count == 2 ){
		for( i=0; i<E_H265_IF_STREAM_CH_MAX; i++ ){
			if( i != stream_ch ){
				if( map_info.wave_map_size[i] != E_H265_IF_IMAGE_SIZE_MAX ){
					first_stream_ch = i;
					break;
				}
			}
		}
		if( first_stream_ch == E_H265_IF_STREAM_CH_MAX ){
			H265_Err_Print(("stream ch error!\n"));		// This route does not pass through.
			return H265_NG;
		}
		
		if( map_info.wave_map_yc_bottom_addr[first_stream_ch] == map_info.wave_map_yc_use ){
			if( map_info.wave_map_yc_top_addr[first_stream_ch] == map_base_info.wave_yc_top_addr ){
//			 -------- 		 -------- 
//			|    1   |		|   1    |
//			|--------|		|--------|
//			|  free  |	->	|   2    |
//			|        |		|--------|
//			|        |		|  free  |
//			 -------- 		 -------- 
				map_info.wave_map_localdec_use			+= localdec_total_size;
				map_info.wave_map_localdec_free_size	-= localdec_total_size;
				map_info.wave_map_yc_use				+= yc_total_size;
				map_info.wave_map_yc_free_size			-= yc_total_size;
			}
			else{
				free1 = map_info.wave_map_yc_top_addr[first_stream_ch] - map_base_info.wave_yc_top_addr;
				free2 = map_base_info.wave_yc_bottom_addr - map_info.wave_map_yc_bottom_addr[stream_ch];
				
				if( free1 > free2 ){
//			 -------- 		 -------- 
//			|  free1 |		|  free1 |	case of free1 > free2
//			|--------|		|--------|
//			|   1    |	->	|   1    |
//			|--------|		|--------|
//			|  free2 |		|   2    |
//			|        |		|--------|
//			|        |		|  free2 |
//			 -------- 		 -------- 
					map_info.wave_map_localdec_use			= map_base_info.wave_localdec_top_addr;
					map_info.wave_map_localdec_free_size	= map_info.wave_map_localdec_top_addr[first_stream_ch] - map_base_info.wave_localdec_top_addr;;
					map_info.wave_map_yc_use				= map_base_info.wave_yc_top_addr;
					map_info.wave_map_yc_free_size			= free1;
				}
				else{
//			 -------- 		 -------- 
//			|  free1 |		|  free1 |	case of free1 < free2
//			|--------|		|--------|
//			|   1    |	->	|   1    |
//			|--------|		|--------|
//			|  free2 |		|   2    |
//			|        |		|--------|
//			|        |		|  free2 |
//			 -------- 		 -------- 
					map_info.wave_map_localdec_use		= map_info.wave_map_localdec_bottom_addr[stream_ch];
					map_info.wave_map_localdec_free_size	= map_base_info.wave_localdec_bottom_addr - map_info.wave_map_localdec_bottom_addr[stream_ch];;
					map_info.wave_map_yc_use				= map_info.wave_map_yc_bottom_addr[stream_ch];
					map_info.wave_map_yc_free_size		= free2;
				}
			}
		}
		else{
			free1 = map_info.wave_map_yc_top_addr[first_stream_ch] - map_info.wave_map_yc_bottom_addr[stream_ch];
			free2 = map_base_info.wave_yc_bottom_addr - map_info.wave_map_yc_bottom_addr[first_stream_ch];
			
			if( free1 > free2 ){
//			 -------- 		 -------- 
//			|  free1 |		|   2    |	case of free1 > free2
//			|        |		|--------|
//			|        |	->	|  free1 |
//			|--------|		|--------|
//			|   1    |		|   1    |
//			|--------|		|--------|
//			|  free2 |		|  free2 |
//			 -------- 		 -------- 
				map_info.wave_map_localdec_use			= map_info.wave_map_localdec_bottom_addr[stream_ch];
				map_info.wave_map_localdec_free_size	= map_info.wave_map_localdec_top_addr[first_stream_ch] - map_info.wave_map_localdec_bottom_addr[stream_ch];
				map_info.wave_map_yc_use				= map_info.wave_map_yc_bottom_addr[stream_ch];
				map_info.wave_map_yc_free_size			= free1;
			}
			else{
//			 -------- 		 -------- 
//			|  free1 |		|   2    |	case of free1 < free2
//			|        |		|--------|
//			|        |	->	|  free1 |
//			|--------|		|--------|
//			|   1    |		|   1    |
//			|--------|		|--------|
//			|  free2 |		|  free2 |
//			 -------- 		 -------- 
				map_info.wave_map_localdec_use			= map_info.wave_map_localdec_bottom_addr[first_stream_ch];
				map_info.wave_map_localdec_free_size	= map_base_info.wave_localdec_bottom_addr - map_info.wave_map_localdec_bottom_addr[first_stream_ch];
				map_info.wave_map_yc_use				= map_info.wave_map_yc_bottom_addr[first_stream_ch];
				map_info.wave_map_yc_free_size			= free2;
			}
		}
	}
	else{
//			not update free area information. becase not allowed next stream.
	}
	
	Sdram_Map_Movie_Record_Set_H265_Map_Information( &map_info );

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---
	
	return H265_OK;
}

/**
 * Ready.
 * @param	nothing
 * @return	nothing
 */
INT32 H265_If_Record_Api_Ready( T_H265_IF_MSG* msg )
{
	INT32	ret;
	
	/* Encode Ready Request */
	ret = H265_Encode_Main_Ready( msg->data.pRecordParam->stream_ch, (T_H265_MOVIE_UI*)&gH265_If_Rec_UI );
	if(      ret == H265_OK					){	;}
	else if( ret == H265_ERR_INPUT_PARAM	){	return H265_PARAM_ERR;	}
	else{										return H265_FATAL_ERR;	}
	
	return H265_OK;
}

/**
 * Stop.
 * @param	nothing
 * @return	nothing
 */
INT32 H265_If_Record_Api_Stop( T_H265_IF_MSG* msg )
{
	if( msg == NULL ){
		return H265_PARAM_ERR;
	}
	
	/* Encode Stop Request */
	H265_Encode_Main_End( (E_H265_IF_STREAM_CH)msg->data.info, (T_H265_MOVIE_UI*)&gH265_If_Rec_UI );
	
	return H265_OK;
}


/*----------------------------------------------------------------------*/
/* Sub Function															*/
/*----------------------------------------------------------------------*/
/**
 * normal end.
 * @param	nothing
 * @return	nothing
 */
INT32 H265_If_Record_Api_Check_End( VOID )
{
	INT32	ret;
	
	// check end
	ret = H265_Encode_Main_Check_End( (T_H265_MOVIE_UI*)&gH265_If_Rec_UI );
	
	return ret;
}

