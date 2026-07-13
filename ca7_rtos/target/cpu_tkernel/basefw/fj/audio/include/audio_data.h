/**
 * @file		audio_data.h
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _AUDIO_DATA_H_
#define _AUDIO_DATA_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration  														*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/

extern VOID BF_Audio_Data_Clean_Flush_Dcache( const VOID* const addr, const UINT32 bytes );
extern VOID BF_Audio_Data_Flush_Dcache( const VOID* const addr, const UINT32 bytes );
extern VOID BF_Audio_Data_Clean_Dcache( const VOID* const addr, const UINT32 bytes );
extern VOID BF_Audio_Data_Reverse2Byte( UINT16* data2 );
extern VOID BF_Audio_Data_ChangeEndian( UINT8* pcm, UINT32 data_bytes );
extern VOID BF_Audio_Data_4ByteTo3Byte( UINT8* in_4byte, UINT8* out_3byte, UINT32 in_data_size );
extern VOID BF_Audio_Data_3ByteTo4Byte( UINT8* in_3byte, UINT8* out_4byte, UINT32 in_data_size );
extern VOID BF_Audio_Data_Pcm_24bitTo16bit( UINT8* in_24bit, UINT8* out_16bit, UINT32 in_data_size );
extern VOID BF_Audio_Data_Pcm_16bitTo24bit( UINT8* in_16bit, UINT8* out_24bit, UINT32 in_data_size );
extern VOID BF_Audio_Data_Pcm_8to16( INT8* src_addr, INT16* dst_addr, UINT32 smpl_cnt, UINT8 channel);
extern VOID BF_Audio_Data_Pcm_16to8( INT16* src_addr, INT8* dst_addr, UINT32 smpl_cnt, UINT8 channel);
extern VOID BF_Audio_Data_24bitPcm_MonoToStereo( UINT8* in_mono, UINT8* out_stereo, UINT32 in_data_size );
extern VOID BF_Audio_Data_24bitPcm_StereoToMono( UINT8* in_stereo, UINT8* out_mono, UINT32 in_data_size );
extern VOID BF_Audio_Data_Add_16bitPcm( INT16* in_data_1, INT16* in_data_2, INT16* out_data, UINT32 data_size );
extern VOID BF_Audio_Data_Add_24bitPcm( UINT8* in_data_1, UINT8* in_data_2, UINT8* out_data, UINT32 data_size );
extern VOID BF_Audio_Data_Swp_16bitPcm_2chTo4ch( UINT32* in_data_1, UINT32* in_data_2, UINT32* out_data, UINT32 in_data_size );
extern VOID BF_Audio_Data_Swp_32bitPcm_2chTo4ch( UINT32* in_data_1, UINT32* in_data_2, UINT32* out_data, UINT32 in_data_size );
extern VOID BF_Audio_Data_Swp_16bitPcm_2chTo3ch( UINT16* in_data_1, UINT16* in_data_2, UINT16* out_data, UINT32 in_data_size );
extern VOID BF_Audio_Data_Swp_32bitPcm_2chTo3ch( UINT32* in_data_1, UINT32* in_data_2, UINT32* out_data, UINT32 in_data_size );
extern VOID BF_Audio_Data_Swp_16bitPcm_4chTo2ch( UINT32* in_data, UINT32* out_data_1, UINT32* out_data_2, UINT32 in_data_size );
extern VOID BF_Audio_Data_Swp_32bitPcm_4chTo2ch( UINT32* in_data, UINT32* out_data_1, UINT32* out_data_2, UINT32 in_data_size );
extern VOID BF_Audio_Data_Swp_16bitPcm_3chTo2ch( UINT16* in_data, UINT16* out_data_1, UINT16* out_data_2, UINT32 in_data_size );
extern VOID BF_Audio_Data_Swp_32bitPcm_3chTo2ch( UINT32* in_data, UINT32* out_data_1, UINT32* out_data_2, UINT32 in_data_size );
extern VOID BF_Audio_Data_24bitPcm_Cnv2chTo3ch( UINT8* in_data, UINT8* out_data, UINT32 in_data_size );
extern VOID BF_Audio_Data_24bitPcm_Cnv2chTo4ch( UINT8* in_data, UINT8* out_data, UINT32 in_data_size );
extern VOID BF_Audio_Data_16bitPcm_Cnv2chTo3ch( UINT16* in_data, UINT16* out_data, UINT32 in_data_size );
extern VOID BF_Audio_Data_16bitPcm_Cnv2chTo4ch( UINT16* in_data, UINT16* out_data, UINT32 in_data_size );
extern VOID BF_Audio_Data_16bitPcm_BitShift_L( UINT16* pcm, UINT32 data_size );
extern VOID BF_Audio_Data_32bitPcm_BitShift_L( UINT32* pcm, UINT32 data_size );
extern UINT32 BF_Audio_Data_Get_NullData_Addr( VOID );

#endif	// _AUDIO_DATA_H_
