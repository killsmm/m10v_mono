/**
 * @file		audio_source.c
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "ddim_typedef.h"
#include "fj_audio.h"
#include "fj_encoder.h"
#include "fj_decoder.h"
#include "audio.h"
#include "audio_source.h"
#include "debug.h"


/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/

static T_AUDIO_SOURCE_INSTANCE gBF_Audio_Source_Inst[D_AUDIO_SRC_INST_NUM] = 
{
	{
		FJ_AUDIO_INPUT_CH_0,		// audio_src_id
		FJ_AUDIO_DISABLE,			// enable
		E_FJ_AUDIO_SRC_0,			// type
		0xFF						// connect_cap_id
	},
	{
		FJ_AUDIO_INPUT_CH_1,
		FJ_AUDIO_DISABLE,
		E_FJ_AUDIO_SRC_1,
		0xFF
	},
	{
		FJ_AUDIO_INPUT_CH_2,
		FJ_AUDIO_DISABLE,
		E_FJ_AUDIO_SRC_2,
		0xFF
	}
};

/*----------------------------------------------------------------------*/
/* Loacal Function                                                      */
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/

/**
 *  @brief		Open Audio Source instance
 *	@param		E_FJ_AUDIO_SRC_ENUM	Audio Source
 *	@param		UCHAR*				Audio Source ID
 *	@return		FJ_ERR_CODE			Result(OK/AUD_PARAM_NG)
 *	@note		None
 *	@attention	None
 */
INT32 BF_AudioSrc_Open( E_FJ_AUDIO_SRC_ENUM audio_src, UCHAR* audio_src_id )
{
	UCHAR src_id;
	
	switch (audio_src) {
		case E_FJ_AUDIO_SRC_0:
			src_id = 0;
			break;
		case E_FJ_AUDIO_SRC_1:
			src_id = 1;
			break;
		default:
		// case E_FJ_AUDIO_SRC_2;
			src_id = 2;
			break; 
	}

	if (gBF_Audio_Source_Inst[src_id].enable == FJ_AUDIO_DISABLE){
		gBF_Audio_Source_Inst[src_id].enable = FJ_AUDIO_ENABLE;
	}
	else {
		BF_Debug_Print_Error(("[AUDIO_SRC]BF_AudioSrc_Open() already use. L=%d\n", __LINE__));
		return D_AUDIO_ERR;
	}
	
	*audio_src_id = gBF_Audio_Source_Inst[src_id].audio_src_id;
	gBF_Audio_Source_Inst[src_id].type = audio_src;
	
	return D_AUDIO_OK;
}


INT32 BF_AudioSrc_Close( UCHAR audio_src_id )
{
	INT32 loop;
	
	for (loop = 0; loop < D_AUDIO_SRC_INST_NUM; loop++){
		if (gBF_Audio_Source_Inst[loop].audio_src_id == audio_src_id){
			gBF_Audio_Source_Inst[loop].enable = FJ_AUDIO_DISABLE;
			break;
		}
	}
	
	return D_AUDIO_OK;
}

INT32 BF_AudioSrc_Connect( UCHAR audio_src_id, UCHAR audio_capture_id )
{
	INT32 loop;
	
	for (loop = 0; loop < D_AUDIO_SRC_INST_NUM; loop++){
		if (gBF_Audio_Source_Inst[loop].audio_src_id == audio_src_id){
			gBF_Audio_Source_Inst[loop].connect_cap_id = audio_capture_id;
			break;
		}
	}
	
	return D_AUDIO_OK;
}

INT32 BF_AudioSrc_Disconnect( UCHAR audio_src_id, UCHAR audio_capture_id )
{
	INT32 loop;
	
	for (loop = 0; loop < D_AUDIO_SRC_INST_NUM; loop++){
		if (gBF_Audio_Source_Inst[loop].audio_src_id == audio_src_id){
			gBF_Audio_Source_Inst[loop].connect_cap_id = 0xFF;
			break;
		}
	}
	
	return D_AUDIO_OK;
}

T_AUDIO_SOURCE_INSTANCE BF_AudioSrc_Get_InstanceInfo( UINT8 index )
{
	return gBF_Audio_Source_Inst[index];
}
