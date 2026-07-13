/**
 * @file		h264_if_record_api.c
 * @brief		This file is a movie record api function.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "h264_if.h"
#include "h264_if_common.h"
#include "h264_movie.h"
#include "h264_encode.h"
#include "sdram_map_movie_record.h"

#ifdef CO_H264_SECTION_ON
//#pragma arm section code="H264_CODE"
//#pragma arm section rodata="H264_CONST"
//#pragma arm section rwdata="H264_INIT"
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
static	volatile	E_H264_IF_RECORD_STATUS		gH264_If_Rec_Stetus;

// Movie Condition Informations
static	volatile	T_H264_MOVIE_UI				gH264_If_Rec_UI ;

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
VOID H264_If_Record_Api_Set_Status( E_H264_IF_RECORD_STATUS status )
{
	gH264_If_Rec_Stetus = status;
}

/**
 * get current status of movie record function
 * @param	nothing
 * @return	current status. please refer to E_H264_IF_RECORD_STATUS.
 */
E_H264_IF_RECORD_STATUS H264_If_Record_Api_Get_Status( VOID )
{
	return gH264_If_Rec_Stetus;
}

/*----------------------------------------------------------------------*/
/* Main API Function													*/
/*----------------------------------------------------------------------*/
/**
 * Initialize.
 * @param	nothing
 * @return	nothing
 */
VOID H264_If_Record_Api_Initialize( VOID )
{
	H264_Movie_Memset( (VOID*)&gH264_If_Rec_UI, 0, sizeof(gH264_If_Rec_UI) );
	
	return ;
}

/**
 * Finalize.
 * @param	nothing
 * @return	nothing
 */
VOID H264_If_Record_Api_Finalize( VOID )
{
	H264_Encode_Finalize();
	return ;
}

/**
 * Param Set. (call by "ready")
 * @param	nothing
 * @return	nothing
 */
INT32 H264_If_Record_Api_Param_Set( T_H264_IF_MSG* msg )
{
	UINT32					reduction_total_size;
	UINT32					localdec_total_size;
	UINT32					yc_total_size;
	UINT32					reduction_y_bank_size;
	UINT32					reduction_c_bank_size;
	UINT32					reduction_g_width;
	UINT32					reduction_g_y_lines;
	UINT32					reduction_g_c_lines;
	UINT32					y_bank_size;
	UINT32					c_bank_size;
	UINT32					g_width;
	UINT32					g_y_lines;
	UINT32					g_c_lines;
	UINT32					i;
	E_H264_IF_STREAM_CH		stream_ch;
	E_H264_IF_STREAM_CH		first_stream_ch = E_H264_IF_STREAM_CH_MAX;
	UINT32					free1, free2;
	
	T_SDRAM_MAP_MOVIE_RECORD_BASE_INFORMATION	map_base_info;
	T_SDRAM_MAP_MOVIE_RECORD_INFORMATION		map_info;
	
	T_SDRAM_MAP_MOVIE_RECORD_RIBERY_REDUCTION	map_reduction;
	T_SDRAM_MAP_MOVIE_RECORD_RIBERY_LOCALDEC	map_localdec;
	T_SDRAM_MAP_MOVIE_RECORD_RIBERY_YC			map_yc;
	
	if( msg == NULL ){
		return H264_PARAM_ERR;
	}
	
	stream_ch = msg->data.pRecordParam->stream_ch;
	memcpy( (VOID *)&gH264_If_Rec_UI.codec_param[stream_ch], msg->data.pRecordParam, sizeof(T_H264_IF_PARAM_RECORD) );
	
	Sdram_Map_Movie_Record_Get_Map_Information( &map_info );
	Sdram_Map_Movie_Record_Get_Map_Base_Information( &map_base_info );
	
	reduction_g_width		= (((msg->data.pRecordParam->width/4) + D_RIBERY_WIDTH_ALIGNMENT - 1) / D_RIBERY_WIDTH_ALIGNMENT) * D_RIBERY_WIDTH_ALIGNMENT;
	reduction_g_y_lines		= (((msg->data.pRecordParam->lines/4) + D_RIBERY_LINES_ALIGNMENT - 1) / D_RIBERY_LINES_ALIGNMENT) * D_RIBERY_LINES_ALIGNMENT;
	reduction_g_c_lines		= (((msg->data.pRecordParam->lines/8) + D_RIBERY_LINES_ALIGNMENT - 1) / D_RIBERY_LINES_ALIGNMENT) * D_RIBERY_LINES_ALIGNMENT;
	reduction_y_bank_size	= (((reduction_g_width * reduction_g_y_lines) + D_RIBERY_BANK_ALIGNMENT - 1) / D_RIBERY_BANK_ALIGNMENT) * D_RIBERY_BANK_ALIGNMENT;
	reduction_c_bank_size	= (((reduction_g_width * reduction_g_c_lines) + D_RIBERY_BANK_ALIGNMENT - 1) / D_RIBERY_BANK_ALIGNMENT) * D_RIBERY_BANK_ALIGNMENT;
	
	g_width					= ((msg->data.pRecordParam->width + D_RIBERY_WIDTH_ALIGNMENT - 1) / D_RIBERY_WIDTH_ALIGNMENT) * D_RIBERY_WIDTH_ALIGNMENT;
	g_y_lines				= ((msg->data.pRecordParam->lines + D_RIBERY_LINES_ALIGNMENT - 1) / D_RIBERY_LINES_ALIGNMENT) * D_RIBERY_LINES_ALIGNMENT;
	g_c_lines				= (((msg->data.pRecordParam->lines/2) + D_RIBERY_LINES_ALIGNMENT - 1) / D_RIBERY_LINES_ALIGNMENT) * D_RIBERY_LINES_ALIGNMENT;
	y_bank_size				= (((g_width * g_y_lines) + D_RIBERY_BANK_ALIGNMENT - 1) / D_RIBERY_BANK_ALIGNMENT) * D_RIBERY_BANK_ALIGNMENT;
	c_bank_size				= (((g_width * g_c_lines) + D_RIBERY_BANK_ALIGNMENT - 1) / D_RIBERY_BANK_ALIGNMENT) * D_RIBERY_BANK_ALIGNMENT;
	
	reduction_total_size	= (reduction_y_bank_size + reduction_c_bank_size) * D_RIBERY_REDUCTION_BANK_MAX;
	localdec_total_size		= (y_bank_size + c_bank_size) * D_RIBERY_LOCALDEC_BANK_MAX;
	yc_total_size			= (y_bank_size + c_bank_size) * D_RIBERY_YC_BANK_MAX;
	
	if( ( reduction_total_size > map_info.ribery_map_reduction_free_size ) ||
		( localdec_total_size  > map_info.ribery_map_localdec_free_size  ) ||
		( yc_total_size        > map_info.ribery_map_yc_free_size        ) ){
		H264_Err_Print(("H264 bank size over error!\n"));
		return H264_NG;
	}
	
	Sdram_Map_Movie_Record_Get_H264_Map_Reduction_Information( stream_ch, &map_reduction );

	map_reduction.max_bank	= D_RIBERY_REDUCTION_BANK_MAX;
	map_reduction.g_width	= reduction_g_width;
	map_reduction.y_bank	= reduction_y_bank_size;
	map_reduction.c_bank	= reduction_c_bank_size;
	map_reduction.y_addr	= map_info.ribery_map_reduction_use;
	map_reduction.c_addr	= map_info.ribery_map_reduction_use;
	
	Sdram_Map_Movie_Record_Set_H264_Map_Reduction_Information( stream_ch, &map_reduction );
	
	
	Sdram_Map_Movie_Record_Get_H264_Map_Local_Dec_Information( stream_ch, &map_localdec );
	
	map_localdec.max_bank	= D_RIBERY_LOCALDEC_BANK_MAX;
	map_localdec.g_width	= g_width;
	map_localdec.y_bank		= y_bank_size;
	map_localdec.c_bank		= c_bank_size;
	map_localdec.y_addr		= map_info.ribery_map_localdec_use;
	map_localdec.c_addr		= map_info.ribery_map_localdec_use;
	
	Sdram_Map_Movie_Record_Set_H264_Map_Local_Dec_Information( stream_ch, &map_localdec );
	
	
	Sdram_Map_Movie_Record_Get_H264_Map_YC_Information( stream_ch, &map_yc );
	
	map_yc.max_bank		= D_RIBERY_YC_BANK_MAX;
	map_yc.g_width		= g_width;
	map_yc.y_bank		= y_bank_size;
	map_yc.c_bank		= c_bank_size;
	map_yc.addr[0].y	= map_info.ribery_map_yc_use;
	map_yc.addr[0].c	= map_yc.addr[0].y + y_bank_size;
	
	for (i = 1; i < D_RIBERY_YC_BANK_MAX; i++) {
		map_yc.addr[i].y	= map_yc.addr[i-1].c + c_bank_size;
		map_yc.addr[i].c	= map_yc.addr[i].y + y_bank_size;
	}
	Sdram_Map_Movie_Record_Set_H264_Map_YC_Information( stream_ch, &map_yc );
	
	
	map_info.ribery_map_count++;
	map_info.ribery_map_size[stream_ch]						= msg->data.pRecordParam->size;
	map_info.ribery_map_reduction_total_size[stream_ch]		= reduction_total_size;
	map_info.ribery_map_reduction_top_addr[stream_ch]		= map_reduction.y_addr;
	map_info.ribery_map_reduction_bottom_addr[stream_ch]	= map_reduction.y_addr + reduction_total_size;
	map_info.ribery_map_localdec_total_size[stream_ch]		= localdec_total_size;
	map_info.ribery_map_localdec_top_addr[stream_ch]		= map_localdec.y_addr;
	map_info.ribery_map_localdec_bottom_addr[stream_ch]		= map_localdec.y_addr + localdec_total_size;
	map_info.ribery_map_yc_total_size[stream_ch]			= yc_total_size;
	map_info.ribery_map_yc_top_addr[stream_ch]				= map_yc.addr[0].y;
	map_info.ribery_map_yc_bottom_addr[stream_ch]			= map_yc.addr[D_RIBERY_YC_BANK_MAX-1].c + c_bank_size;
	
	if( map_info.ribery_map_count == 1 ){
//		 -------- 		 -------- 
//		|        |		|   1    |
//		|        |		|--------|
//		|  free  |	->	|  free  |
//		|        |		|        |
//		|        |		|        |
//		 -------- 		 -------- 
		map_info.ribery_map_reduction_use		+= reduction_total_size;
		map_info.ribery_map_reduction_free_size	-= reduction_total_size;
		map_info.ribery_map_localdec_use		+= localdec_total_size;
		map_info.ribery_map_localdec_free_size	-= localdec_total_size;
		map_info.ribery_map_yc_use				+= yc_total_size;
		map_info.ribery_map_yc_free_size		-= yc_total_size;
	}
	else if( map_info.ribery_map_count == 2 ){
		for( i=0; i<E_H264_IF_STREAM_CH_MAX; i++ ){
			if( i != stream_ch ){
				if( map_info.ribery_map_size[i] != E_H264_IF_IMAGE_SIZE_MAX ){
					first_stream_ch = i;
					break;
				}
			}
		}
		if( first_stream_ch == E_H264_IF_STREAM_CH_MAX ){
			H264_Err_Print(("H264_If_Record_Api_Param_Set : stream ch error!\n"));		// This route does not pass through.
			return H264_NG;
		}
		
		if( map_info.ribery_map_reduction_bottom_addr[first_stream_ch] == map_info.ribery_map_reduction_use ){
			if( map_info.ribery_map_reduction_top_addr[first_stream_ch] == map_base_info.ribery_reduction_top_addr ){
//			 -------- 		 -------- 
//			|    1   |		|   1    |
//			|--------|		|--------|
//			|  free  |	->	|   2    |
//			|        |		|--------|
//			|        |		|  free  |
//			 -------- 		 -------- 
				map_info.ribery_map_reduction_use		+= reduction_total_size;
				map_info.ribery_map_reduction_free_size	-= reduction_total_size;
			}
			else{
				free1 = map_info.ribery_map_reduction_top_addr[first_stream_ch] - map_base_info.ribery_reduction_top_addr;
				free2 = map_base_info.ribery_reduction_bottom_addr - map_info.ribery_map_reduction_bottom_addr[stream_ch];
				
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
					map_info.ribery_map_reduction_use		= map_base_info.ribery_reduction_top_addr;
					map_info.ribery_map_reduction_free_size	= free1;
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
					map_info.ribery_map_reduction_use		= map_info.ribery_map_reduction_bottom_addr[stream_ch];
					map_info.ribery_map_reduction_free_size	= free2;
				}
			}
		}
		else{
			free1 = map_info.ribery_map_reduction_top_addr[first_stream_ch] - map_info.ribery_map_reduction_bottom_addr[stream_ch];
			free2 = map_base_info.ribery_reduction_bottom_addr - map_info.ribery_map_reduction_bottom_addr[first_stream_ch];
			
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
				map_info.ribery_map_reduction_use		= map_info.ribery_map_reduction_bottom_addr[stream_ch];
				map_info.ribery_map_reduction_free_size	= free1;
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
				map_info.ribery_map_reduction_use		= map_info.ribery_map_reduction_bottom_addr[first_stream_ch];
				map_info.ribery_map_reduction_free_size	= free2;
			}
		}
		
		
		if( map_info.ribery_map_yc_bottom_addr[first_stream_ch] == map_info.ribery_map_yc_use ){
			if( map_info.ribery_map_yc_top_addr[first_stream_ch] == map_base_info.ribery_yc_top_addr ){
//			 -------- 		 -------- 
//			|    1   |		|   1    |
//			|--------|		|--------|
//			|  free  |	->	|   2    |
//			|        |		|--------|
//			|        |		|  free  |
//			 -------- 		 -------- 
				map_info.ribery_map_localdec_use		+= localdec_total_size;
				map_info.ribery_map_localdec_free_size	-= localdec_total_size;
				map_info.ribery_map_yc_use				+= yc_total_size;
				map_info.ribery_map_yc_free_size		-= yc_total_size;
			}
			else{
				free1 = map_info.ribery_map_yc_top_addr[first_stream_ch] - map_base_info.ribery_yc_top_addr;
				free2 = map_base_info.ribery_yc_bottom_addr - map_info.ribery_map_yc_bottom_addr[stream_ch];
				
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
					map_info.ribery_map_localdec_use		= map_base_info.ribery_localdec_top_addr;
					map_info.ribery_map_localdec_free_size	= map_info.ribery_map_localdec_top_addr[first_stream_ch] - map_base_info.ribery_localdec_top_addr;;
					map_info.ribery_map_yc_use				= map_base_info.ribery_yc_top_addr;
					map_info.ribery_map_yc_free_size		= free1;
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
					map_info.ribery_map_localdec_use		= map_info.ribery_map_localdec_bottom_addr[stream_ch];
					map_info.ribery_map_localdec_free_size	= map_base_info.ribery_localdec_bottom_addr - map_info.ribery_map_localdec_bottom_addr[stream_ch];;
					map_info.ribery_map_yc_use				= map_info.ribery_map_yc_bottom_addr[stream_ch];
					map_info.ribery_map_yc_free_size		= free2;
				}
			}
		}
		else{
			free1 = map_info.ribery_map_yc_top_addr[first_stream_ch] - map_info.ribery_map_yc_bottom_addr[stream_ch];
			free2 = map_base_info.ribery_yc_bottom_addr - map_info.ribery_map_yc_bottom_addr[first_stream_ch];
			
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
				map_info.ribery_map_localdec_use		= map_info.ribery_map_localdec_bottom_addr[stream_ch];
				map_info.ribery_map_localdec_free_size	= map_info.ribery_map_localdec_top_addr[first_stream_ch] - map_info.ribery_map_localdec_bottom_addr[stream_ch];
				map_info.ribery_map_yc_use				= map_info.ribery_map_yc_bottom_addr[stream_ch];
				map_info.ribery_map_yc_free_size		= free1;
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
				map_info.ribery_map_localdec_use		= map_info.ribery_map_localdec_bottom_addr[first_stream_ch];
				map_info.ribery_map_localdec_free_size	= map_base_info.ribery_localdec_bottom_addr - map_info.ribery_map_localdec_bottom_addr[first_stream_ch];
				map_info.ribery_map_yc_use				= map_info.ribery_map_yc_bottom_addr[first_stream_ch];
				map_info.ribery_map_yc_free_size		= free2;
			}
		}
	}
	else{
//			not update free area information. becase not allowed next stream.
	}
	
	Sdram_Map_Movie_Record_Set_H264_Map_Information( &map_info );
	
	return H264_OK;
}

/**
 * Ready.
 * @param	nothing
 * @return	nothing
 */
INT32 H264_If_Record_Api_Ready( T_H264_IF_MSG* msg )
{
	INT32	ret;
	
	/* Encode Ready Request */
	ret = H264_Encode_Main_Ready( msg->data.pRecordParam->stream_ch, (T_H264_MOVIE_UI*)&gH264_If_Rec_UI );
	if(      ret == H264_OK					){	;}
	else if( ret == H264_ERR_INPUT_PARAM	){	return H264_PARAM_ERR;	}
	else{										return H264_FATAL_ERR;	}
	
	return H264_OK;
}

/**
 * Stop.
 * @param	nothing
 * @return	nothing
 */
INT32 H264_If_Record_Api_Stop( T_H264_IF_MSG* msg )
{
	if( msg == NULL ){
		return H264_PARAM_ERR;
	}
	
	/* Encode Stop Request */
	H264_Encode_Main_End( (E_H264_IF_STREAM_CH)msg->data.info, (T_H264_MOVIE_UI*)&gH264_If_Rec_UI );
	
	return H264_OK;
}


/*----------------------------------------------------------------------*/
/* Sub Function															*/
/*----------------------------------------------------------------------*/
/**
 * normal end.
 * @param	nothing
 * @return	nothing
 */
INT32 H264_If_Record_Api_Check_End( VOID )
{
	INT32	ret;
	
	// check end
	ret = H264_Encode_Main_Check_End( (T_H264_MOVIE_UI*)&gH264_If_Rec_UI );
	
	return ret;
}

