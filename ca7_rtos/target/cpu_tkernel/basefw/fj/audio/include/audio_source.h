/**
 * @file		audio_source.h
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _AUDIO_SOURCE_H_
#define _AUDIO_SOURCE_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_AUDIO_SRC_INST_NUM				(3)

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/

/**	Audio Source Instance */
typedef struct {
	UCHAR					audio_src_id;			// AudioSourceID
	UCHAR					enable;					// Enable/disable
	E_FJ_AUDIO_SRC_ENUM		type;					// SRC Type
	UCHAR					connect_cap_id;			// Connected AudioCaptureID
} T_AUDIO_SOURCE_INSTANCE;

/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/

extern INT32 BF_AudioSrc_Open( E_FJ_AUDIO_SRC_ENUM audio_src, UCHAR* audio_src_id );
extern INT32 BF_AudioSrc_Close( UCHAR audio_src_id );
extern INT32 BF_AudioSrc_Connect( UCHAR audio_src_id, UCHAR audio_capture_id );
extern INT32 BF_AudioSrc_Disconnect( UCHAR audio_src_id, UCHAR audio_capture_id );
extern T_AUDIO_SOURCE_INSTANCE BF_AudioSrc_Get_InstanceInfo( UINT8 index );

#endif		//_AUDIO_SOURCE_H_
