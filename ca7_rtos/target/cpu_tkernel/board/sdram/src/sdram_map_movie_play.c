/**
 * @file		sdram_map_movie_play.c
 * @brief		movie play memory map.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_common.h"
#include "sdram_map_movie_play.h"
#include "h264_decode.h"
#include "h265_decode.h"

#ifdef CO_MOVIE_SECTION_ON
//#pragma arm section code="MOVIE_CODE"
//#pragma arm section rodata="MOVIE_CONST"
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

// movie play control structure
// H.264
typedef struct {
	UINT32	bank_num;
	UINT32	y_addr[D_H264_DEC_VDPB_MAX_NUM];
	UINT32	c_addr[D_H264_DEC_VDPB_MAX_NUM];
} T_SDRAM_MAP_MOVIE_PLAY_H264_YUV_CONTROL;

typedef struct {
	UINT32	write_pointer;
	UINT32	read_pointer;
	UINT32	start_address;
	UINT32	end_address;
	UINT32	remain_size;
} T_SDRAM_MAP_MOVIE_PLAY_H264_STREAM_CONTROL;


typedef struct {
	T_SDRAM_MAP_MOVIE_PLAY_H264_YUV_CONTROL		yuv_ctrl;
	T_SDRAM_MAP_MOVIE_PLAY_H264_STREAM_CONTROL	stream_ctrl;
	BYTE										last_yuv_index;
} T_SDRAM_MAP_MOVIE_PLAY_H264_CONTROL;


// H.265
typedef struct {
	UINT32	bank_num;
	UINT32	y_bank_size;
	UINT32	c_bank_size;
	UINT32	y_addr[D_H265_DEC_VDPB_MAX_NUM];
	UINT32	c_addr[D_H265_DEC_VDPB_MAX_NUM];
} T_SDRAM_MAP_MOVIE_PLAY_H265_YUV_CONTROL;

typedef struct {
	UINT32	write_pointer;
	UINT32	read_pointer;
	UINT32	start_address;
	UINT32	end_address;
	UINT32	remain_size;
} T_SDRAM_MAP_MOVIE_PLAY_H265_STREAM_CONTROL;


typedef struct {
	T_SDRAM_MAP_MOVIE_PLAY_H265_YUV_CONTROL		yuv_ctrl;
	T_SDRAM_MAP_MOVIE_PLAY_H265_STREAM_CONTROL	stream_ctrl;
	BYTE										last_yuv_index;
} T_SDRAM_MAP_MOVIE_PLAY_H265_CONTROL;


// Audio
typedef struct {
	UINT8	astream_in;
	UINT8	astream_out;
	UINT8	astream_count;
} T_SDRAM_MAP_MOVIE_PLAY_AUDIO_INDEX;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// movie play memory map index control
static T_SDRAM_MAP_MOVIE_PLAY_H264_CONTROL	gSdram_Map_Movie_Play_H264_Ctl;
static T_SDRAM_MAP_MOVIE_PLAY_H265_CONTROL	gSdram_Map_Movie_Play_H265_Ctl;

static T_SDRAM_MAP_MOVIE_PLAY_AUDIO_INDEX	gSdram_Map_Movie_Play_Ctl_Audio;

static const UINT32	gSdram_Map_Movie_Play_Audio[SDRAM_BNK_MOVIE_PLAY_AUDIO_STREAM] = {
	SDRAM_ADR_MOVIE_PLAY_AUDIO_STREAM_0,	SDRAM_ADR_MOVIE_PLAY_AUDIO_STREAM_1,	SDRAM_ADR_MOVIE_PLAY_AUDIO_STREAM_2,	SDRAM_ADR_MOVIE_PLAY_AUDIO_STREAM_3
};



/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/*--------------*/
/* H.264		*/
/*--------------*/
/**
 *  @brief		Initialize SDRAM Index for Movie Play(H.264) Mode.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Play_H264_Initialize( UINT32 y_bank_size, UINT32 c_bank_size, UINT32* bank_num )
{
	UINT32	yuv_loop;
	UINT32	bank_pointer;
	UINT32	yuv_bank_end;
	
	OS_User_Wai_Sem(SID_H264_STREAM);
	
	memset( &gSdram_Map_Movie_Play_H264_Ctl, 0, sizeof(gSdram_Map_Movie_Play_H264_Ctl) );
	
	gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.start_address	= SDRAM_ADR_MOVIE_PLAY_H264_STREAM;
	gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.end_address		= SDRAM_ADR_MOVIE_PLAY_H264_STREAM_END;
	
	gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.write_pointer	= SDRAM_ADR_MOVIE_PLAY_H264_STREAM;
	gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.read_pointer		= SDRAM_ADR_MOVIE_PLAY_H264_STREAM;
	
	gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.remain_size		= SDRAM_SIZ_MOVIE_PLAY_H264_STREAM;
	
	bank_pointer	= SDRAM_ADR_MOVIE_PLAY_YCC;
	yuv_bank_end	= SDRAM_ADR_MOVIE_PLAY_YCC_END;
	
	for( yuv_loop=0; yuv_loop < D_H264_DEC_VDPB_MAX_NUM; yuv_loop++ ){
		
		if( (bank_pointer + y_bank_size + c_bank_size) > yuv_bank_end ){
			break;
		}
		
		gSdram_Map_Movie_Play_H264_Ctl.yuv_ctrl.y_addr[yuv_loop]	= bank_pointer;
		gSdram_Map_Movie_Play_H264_Ctl.yuv_ctrl.c_addr[yuv_loop]	= bank_pointer + y_bank_size;
		gSdram_Map_Movie_Play_H264_Ctl.yuv_ctrl.bank_num++;
		bank_pointer += y_bank_size + c_bank_size;
		
		if( gSdram_Map_Movie_Play_H264_Ctl.yuv_ctrl.bank_num >= *bank_num ){
			break;
		}
	}
	
	*bank_num = gSdram_Map_Movie_Play_H264_Ctl.yuv_ctrl.bank_num;
	
	OS_User_Sig_Sem(SID_H264_STREAM);
	
	return;
}


/**
 *  @brief		Initialize Stream Control for Movie Play(H.264) Mode.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Play_H264_Stream_Ctrl_Initialize( VOID )
{
	gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.write_pointer	= SDRAM_ADR_MOVIE_PLAY_H264_STREAM;
	gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.read_pointer		= SDRAM_ADR_MOVIE_PLAY_H264_STREAM;
	gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.remain_size		= SDRAM_SIZ_MOVIE_PLAY_H264_STREAM;
	return;
}


/**
 *  @brief		Get Decode Image address.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Play_H264_Get_Vdec( UINT32 idx, UINT32* y_addr, UINT32* c_addr )
{
	if( idx >= gSdram_Map_Movie_Play_H264_Ctl.yuv_ctrl.bank_num ){
		BF_Debug_Print_Error(("Sdram_Map_Movie_Play_H264_Get_Vdec : The value of the index is invalid.(%d)\n", idx ));
		*y_addr = 0;
		*c_addr = 0;
		return;
	}
	
	gSdram_Map_Movie_Play_H264_Ctl.last_yuv_index = idx ;
	
	*y_addr = gSdram_Map_Movie_Play_H264_Ctl.yuv_ctrl.y_addr[ idx ];
	*c_addr = gSdram_Map_Movie_Play_H264_Ctl.yuv_ctrl.c_addr[ idx ];
	
	return;
}

/**
 *  @brief		Get Decode Image address (last).\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Play_H264_Get_LastVdec( UINT32* y_addr, UINT32* c_addr )
{
	Sdram_Map_Movie_Play_H264_Get_Vdec( gSdram_Map_Movie_Play_H264_Ctl.last_yuv_index, y_addr, c_addr );
	return;
}


/**
 *  @brief		Get Stream address (last).\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 Sdram_Map_Movie_Play_H264_Get_Stream_Area( UINT32* size, UINT32* address )
{
	INT32	ret=0;
	UINT32	bank_remain_size_1;
	UINT32	bank_remain_size_2;
	
	OS_User_Wai_Sem(SID_H264_STREAM);
	
	// positional relationship check of write_pointer and read_pointer
	if( gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.write_pointer >= gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.read_pointer ){
		// write_pointer're behind than read_pointer.
		bank_remain_size_1 = gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.end_address - gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.write_pointer;
		bank_remain_size_2 = gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.read_pointer - gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.start_address;
		
		if( *size < bank_remain_size_1 ){
			// Size fits.
			*address = gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.write_pointer;
			gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.write_pointer = *address + *size;
			bank_remain_size_1 -= *size;
			if( bank_remain_size_1 >= bank_remain_size_2 ){
				gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.remain_size = bank_remain_size_1;
			}
			else{
				gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.remain_size = bank_remain_size_2;
			}
			ret = 0;
		}
		else{
			// Size does not fit.
			if( *size < bank_remain_size_2 ){
				// Size fits.
				*address = gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.start_address;
				gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.write_pointer = *address + *size;
				bank_remain_size_2 -= *size;
				// Because it uses as a ring buffer, bank_remain_size_1 is not used.
				gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.remain_size = bank_remain_size_2;
				ret = 0;
			}
			else{
				// Size does not fit. -> Full!!
				address = NULL;
				ret = -1;
			}
		}
	}
	else{
		// If write_pointer is in front than read_pointer (if it has been around a ring buffer)
		bank_remain_size_1 = gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.read_pointer - gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.write_pointer;
		
		if( *size < bank_remain_size_1 ){
			// Size fits.
			*address = gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.write_pointer;
			gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.write_pointer = *address + *size;
			bank_remain_size_1 -= *size;
			gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.remain_size = bank_remain_size_1;
			ret = 0;
		}
		else{
			// Size does not fit.
			address = NULL;
			ret = -1;
		}
	}
	
	OS_User_Sig_Sem(SID_H264_STREAM);
	
	return ret;
}


/**
 *  @brief		Check Stream Address.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
BOOL Sdram_Map_Movie_Play_H264_Check_Stream_Area( UINT32 size, UINT32 address )
{
	UINT32	stream_start_address;
	UINT32	stream_end_address;
	
	stream_start_address	= address;
	stream_end_address		= address + size;
	
	if( stream_start_address < SDRAM_ADR_MOVIE_PLAY_H264_STREAM ){
		BF_Debug_Print_Error(("Sdram_Map_Movie_Play_H264_Check_Stream_Area : stream address error!!(start address) [%#x]\n", address ));
		return FALSE;
	}
	if( stream_end_address >= (SDRAM_ADR_MOVIE_PLAY_H264_STREAM + SDRAM_SIZ_MOVIE_PLAY_H264_STREAM) ){
		BF_Debug_Print_Error(("Sdram_Map_Movie_Play_H264_Check_Stream_Area : stream address error!!(end address) [%d] [%#x]\n", size, address ));
		return FALSE;
	}
	if( size == 0 ){
		BF_Debug_Print_Error(("Sdram_Map_Movie_Play_H264_Check_Stream_Area : stream size error!!(size is 0)\n" ));
		return FALSE;
	}
	
	return TRUE;
}


/**
 *  @brief		Release Stream Area.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Play_H264_Release_Stream_Area( UINT32 address )
{
	UINT32	bank_remain_size_1;
	UINT32	bank_remain_size_2;
	
	OS_User_Wai_Sem(SID_H264_STREAM);
	
	// read_pointer update.
	gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.read_pointer = address;
	
	// positional relationship check of write_pointer and read_pointer
	if( gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.write_pointer >= gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.read_pointer ){
		
		bank_remain_size_1 = gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.end_address - gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.write_pointer;
		bank_remain_size_2 = gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.read_pointer - gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.start_address;
		
		if( bank_remain_size_1 >= bank_remain_size_2 ){
			gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.remain_size = bank_remain_size_1;
		}
		else{
			gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.remain_size = bank_remain_size_2;
		}
	}
	else{
		// If write_pointer is in front than read_pointer (if it has been around a ring buffer)
		gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.remain_size = gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.read_pointer - gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.write_pointer;
	}
	
	OS_User_Sig_Sem(SID_H264_STREAM);
	
	return;
}


/**
 *  @brief		Get Empty Size for Stream Area.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Play_H264_Get_Stream_Empty_Size( UINT32* size )
{
	OS_User_Wai_Sem(SID_H264_STREAM);
	
	*size = gSdram_Map_Movie_Play_H264_Ctl.stream_ctrl.remain_size;
	
	OS_User_Sig_Sem(SID_H264_STREAM);
	
	return;
}



/*--------------*/
/* H.265		*/
/*--------------*/
/**
 *  @brief		Initialize SDRAM Index for Movie Play(H.265) Mode.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Play_H265_Initialize( UINT32 y_bank_size, UINT32 c_bank_size, UINT32* bank_num )
{
	UINT32	yuv_loop;
	UINT32	bank_pointer;
	UINT32	yuv_bank_end;
	
	OS_User_Wai_Sem(SID_H265_STREAM);
	
	memset( &gSdram_Map_Movie_Play_H265_Ctl, 0, sizeof(gSdram_Map_Movie_Play_H265_Ctl) );
	
	gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.y_bank_size			= y_bank_size;
	gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.c_bank_size			= c_bank_size;
	
	gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.start_address	= SDRAM_ADR_MOVIE_PLAY_H265_STREAM;
	gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.end_address		= SDRAM_ADR_MOVIE_PLAY_H265_STREAM + SDRAM_SIZ_MOVIE_PLAY_H265_STREAM;
	
	gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.write_pointer	= SDRAM_ADR_MOVIE_PLAY_H265_STREAM;
	gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.read_pointer		= SDRAM_ADR_MOVIE_PLAY_H265_STREAM;
	
	gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.remain_size		= SDRAM_SIZ_MOVIE_PLAY_H265_STREAM;
	
	bank_pointer	= SDRAM_ADR_MOVIE_PLAY_YCC_1;
	yuv_bank_end	= SDRAM_ADR_MOVIE_PLAY_YCC_END_1;
	
	// Calculation of the number of banks that can be configured
	for( yuv_loop=0; yuv_loop < D_H265_DEC_VDPB_MAX_NUM; yuv_loop++ ){
		
		if( (bank_pointer + y_bank_size + c_bank_size) > yuv_bank_end ){
			break;
		}
		
		gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.bank_num++;
		bank_pointer += y_bank_size + c_bank_size;
		
		if( gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.bank_num >= *bank_num ){
			break;
		}
	}
	
	bank_pointer	= SDRAM_ADR_MOVIE_PLAY_YCC_1;
	
	// Y Bank address setting
	for( yuv_loop=0; yuv_loop < gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.bank_num; yuv_loop++ ){
		
		gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.y_addr[yuv_loop]	= bank_pointer;
		bank_pointer += y_bank_size;
	}
	// C Bank address setting
	for( yuv_loop=0; yuv_loop < gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.bank_num; yuv_loop++ ){
		
		gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.c_addr[yuv_loop]	= bank_pointer;
		bank_pointer += c_bank_size;
	}
	
	*bank_num = gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.bank_num;
	
	OS_User_Sig_Sem(SID_H265_STREAM);
	
	return;
}


/**
 *  @brief		Initialize Stream Control for Movie Play(H.265) Mode.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Play_H265_Stream_Ctrl_Initialize( VOID )
{
	gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.write_pointer	= SDRAM_ADR_MOVIE_PLAY_H265_STREAM;
	gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.read_pointer		= SDRAM_ADR_MOVIE_PLAY_H265_STREAM;
	gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.remain_size		= SDRAM_SIZ_MOVIE_PLAY_H265_STREAM;
	return;
}


/**
 *  @brief		Get Decode Image address.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Play_H265_Get_Vdec( UINT32 idx, UINT32* y_addr, UINT32* c_addr )
{
	if( idx >= gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.bank_num ){
		BF_Debug_Print_Error(("Sdram_Map_Movie_Play_H265_Get_Vdec : The value of the index is invalid.(%d)\n", idx ));
		*y_addr = 0;
		*c_addr = 0;
		return;
	}
	
	gSdram_Map_Movie_Play_H265_Ctl.last_yuv_index = idx ;
	
	*y_addr = gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.y_addr[ idx ];
	*c_addr = gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.c_addr[ idx ];
	
	return;
}

/**
 *  @brief		Get Decode Image address (last).\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Play_H265_Get_LastVdec( UINT32* y_addr, UINT32* c_addr )
{
	Sdram_Map_Movie_Play_H265_Get_Vdec( gSdram_Map_Movie_Play_H265_Ctl.last_yuv_index, y_addr, c_addr );
	return;
}

/**
 *  @brief		Get Decode Image Bank Area.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Play_H265_Get_Vdec_Area( UINT32* y_start_addr, UINT32* y_end_addr, UINT32* c_start_addr, UINT32* c_end_addr )
{
	*y_start_addr	= gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.y_addr[0];
	*y_end_addr		= gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.y_addr[gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.bank_num - 1] + gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.y_bank_size;
	*c_start_addr	= gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.c_addr[0];
	*c_end_addr		= gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.c_addr[gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.bank_num - 1] + gSdram_Map_Movie_Play_H265_Ctl.yuv_ctrl.c_bank_size;
	
	return;
}




/**
 *  @brief		Get Stream address (last).\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
INT32 Sdram_Map_Movie_Play_H265_Get_Stream_Area( UINT32* size, UINT32* address )
{
	INT32	ret=0;
	UINT32	bank_remain_size_1;
	UINT32	bank_remain_size_2;
	
	OS_User_Wai_Sem(SID_H265_STREAM);
	
	// positional relationship check of write_pointer and read_pointer
	if( gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.write_pointer >= gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.read_pointer ){
		// write_pointer're behind than read_pointer.
		bank_remain_size_1 = gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.end_address - gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.write_pointer;
		bank_remain_size_2 = gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.read_pointer - gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.start_address;
		
		if( *size < bank_remain_size_1 ){
			// Size fits.
			*address = gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.write_pointer;
			gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.write_pointer = *address + *size;
			bank_remain_size_1 -= *size;
			if( bank_remain_size_1 >= bank_remain_size_2 ){
				gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.remain_size = bank_remain_size_1;
			}
			else{
				gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.remain_size = bank_remain_size_2;
			}
			ret = 0;
		}
		else{
			// Size does not fit.
			if( *size < bank_remain_size_2 ){
				// Size fits.
				*address = gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.start_address;
				gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.write_pointer = *address + *size;
				bank_remain_size_2 -= *size;
				// Because it uses as a ring buffer, bank_remain_size_1 is not used.
				gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.remain_size = bank_remain_size_2;
				ret = 0;
			}
			else{
				// Size does not fit. -> Full!!
				address = NULL;
				ret = -1;
			}
		}
	}
	else{
		// If write_pointer is in front than read_pointer (if it has been around a ring buffer)
		bank_remain_size_1 = gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.read_pointer - gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.write_pointer;
		
		if( *size < bank_remain_size_1 ){
			// Size fits.
			*address = gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.write_pointer;
			gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.write_pointer = *address + *size;
			bank_remain_size_1 -= *size;
			gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.remain_size = bank_remain_size_1;
			ret = 0;
		}
		else{
			// Size does not fit.
			address = NULL;
			ret = -1;
		}
	}
	
	OS_User_Sig_Sem(SID_H265_STREAM);
	
	return ret;
}


/**
 *  @brief		Check Stream Address.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
BOOL Sdram_Map_Movie_Play_H265_Check_Stream_Area( UINT32 size, UINT32 address )
{
	UINT32	stream_start_address;
	UINT32	stream_end_address;
	
	stream_start_address	= address;
	stream_end_address		= address + size;
	
	if( stream_start_address < SDRAM_ADR_MOVIE_PLAY_H265_STREAM ){
		BF_Debug_Print_Error(("Sdram_Map_Movie_Play_H265_Check_Stream_Area : stream address error!!(start address) [%#x]\n", address ));
		return FALSE;
	}
	if( stream_end_address >= (SDRAM_ADR_MOVIE_PLAY_H265_STREAM + SDRAM_SIZ_MOVIE_PLAY_H265_STREAM) ){
		BF_Debug_Print_Error(("Sdram_Map_Movie_Play_H265_Check_Stream_Area : stream address error!!(end address) [%d] [%#x]\n", size, address ));
		return FALSE;
	}
	if( size == 0 ){
		BF_Debug_Print_Error(("Sdram_Map_Movie_Play_H265_Check_Stream_Area : stream size error!!(size is 0)\n" ));
		return FALSE;
	}
	
	return TRUE;
}


/**
 *  @brief		Release Stream Area.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Play_H265_Release_Stream_Area( UINT32 address )
{
	UINT32	bank_remain_size_1;
	UINT32	bank_remain_size_2;
	
	OS_User_Wai_Sem(SID_H265_STREAM);
	
	// read_pointer update.
	gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.read_pointer = address;
	
	// positional relationship check of write_pointer and read_pointer
	if( gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.write_pointer >= gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.read_pointer ){
		
		bank_remain_size_1 = gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.end_address - gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.write_pointer;
		bank_remain_size_2 = gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.read_pointer - gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.start_address;
		
		if( bank_remain_size_1 >= bank_remain_size_2 ){
			gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.remain_size = bank_remain_size_1;
		}
		else{
			gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.remain_size = bank_remain_size_2;
		}
	}
	else{
		// If write_pointer is in front than read_pointer (if it has been around a ring buffer)
		gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.remain_size = gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.read_pointer - gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.write_pointer;
	}
	
	OS_User_Sig_Sem(SID_H265_STREAM);
	
	return;
}


/**
 *  @brief		Get Empty Size for Stream Area.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Play_H265_Get_Stream_Empty_Size( UINT32* size )
{
	OS_User_Wai_Sem(SID_H265_STREAM);
	
	*size = gSdram_Map_Movie_Play_H265_Ctl.stream_ctrl.remain_size;
	
	OS_User_Sig_Sem(SID_H265_STREAM);
	
	return;
}



/*--------------*/
/* Audio		*/
/*--------------*/
/**
 *  @brief		Initialize SDRAM Index for Movie Play Mode.\n
 *	@param		None
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID Sdram_Map_Movie_Play_Audio_Initialize( VOID )
{
	gSdram_Map_Movie_Play_Ctl_Audio.astream_in = 0;
	gSdram_Map_Movie_Play_Ctl_Audio.astream_out = 0;
	gSdram_Map_Movie_Play_Ctl_Audio.astream_count = 0;
}

/**
 *  @brief		Get AudioStream_In address.\n
 *	@param		None
 *	@return		PCM_in address
 *	@note		None
 *	@attention	None
 */
UINT32 Sdram_Map_Movie_Play_Get_AStream_in( VOID )
{
	return gSdram_Map_Movie_Play_Audio[gSdram_Map_Movie_Play_Ctl_Audio.astream_in];
}

/**
 *  @brief		Change AudioStream_In area.\n
 *	@param		None
 *	@return		MOVIE_OK	Normal End
 *	@return		MOVIE_NG	Abnormal End\n
 *								It cannot set audio stream data, because audio process is not play\n
 *								audio sample yet.\n
 *	@note		None
 *	@attention	None
 */
INT32 Sdram_Map_Movie_Play_Chg_AStream_in( VOID )
{
	if (gSdram_Map_Movie_Play_Ctl_Audio.astream_count < SDRAM_BNK_MOVIE_PLAY_AUDIO_STREAM){
		gSdram_Map_Movie_Play_Ctl_Audio.astream_in++;
		gSdram_Map_Movie_Play_Ctl_Audio.astream_in = SDRAM_BNK_LIMIT(gSdram_Map_Movie_Play_Ctl_Audio.astream_in, SDRAM_BNK_MOVIE_PLAY_AUDIO_STREAM);
	}
	else{
		// It can not reference buffer, so system treat as error.
		return 1;
	}
	gSdram_Map_Movie_Play_Ctl_Audio.astream_count++;
	return 0;
}

/**
 *  @brief		Get AudioStream_Out address.\n
 *	@param		None
 *	@return		PCM_out address
 *	@note		None
 *	@attention	None
 */
UINT32 Sdram_Map_Movie_Play_Get_AStream_out( VOID )
{
	return gSdram_Map_Movie_Play_Audio[gSdram_Map_Movie_Play_Ctl_Audio.astream_out];
}

/**
 *  @brief		Change AudioStream_Out area.\n
 *	@param		None
 *	@return		MOVIE_OK	Normal End
 *	@return		MOVIE_NG	Abnormal End\n
 *								It cannot found audio stream data, because movie process was not set\n
 *								audio sample yet.\n
 *	@note		None
 *	@attention	None
 */
INT32 Sdram_Map_Movie_Play_Chg_AStream_out( VOID )
{
	if( gSdram_Map_Movie_Play_Ctl_Audio.astream_count > 0 ){
		gSdram_Map_Movie_Play_Ctl_Audio.astream_out++;
		gSdram_Map_Movie_Play_Ctl_Audio.astream_out = SDRAM_BNK_LIMIT(gSdram_Map_Movie_Play_Ctl_Audio.astream_out, SDRAM_BNK_MOVIE_PLAY_AUDIO_STREAM);
	}
	else{
		// It can not reference buffer, so system treat as error.
		return 1;
	}
	gSdram_Map_Movie_Play_Ctl_Audio.astream_count--;
	return 0;
}


/**
 *  @brief		Get AudioStream_Out address.\n
 *	@param		None
 *	@return		PCM_out address
 *	@note		None
 *	@attention	None
 */
UINT32 Sdram_Map_Movie_Play_Get_AStream_Area_Size( VOID )
{
	return SDRAM_SIZ_MOVIE_PLAY_AUDIO_STREAM;
}

