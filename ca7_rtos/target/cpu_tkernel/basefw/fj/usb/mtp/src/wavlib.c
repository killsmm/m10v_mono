/**
 * @file		wavlib.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */
//#pragma arm section rwdata="BASEFW_MTP_INIT"
//#pragma arm section rodata="BASEFW_MTP_CONST"
//#pragma arm section code="BASEFW_MTP_CODE"
//#pragma arm section rwdata="MTP_DATA_AREA"

#include <string.h>
#include "debug.h"
#include "wavlib.h"

#define WAV_PARSE_OK		0x00000000
#define WAV_ERR_READ        0x00000001    	
#define WAV_ERR_NOT_RIFF    0x00000002   
#define WAV_ERR_OPEN		0x00000003
#define WAV_ERR_SEEK		0x00000006    	

//#define WAVPARSE_ERR_PRINT
//#define MTP_WAV_SUPPORT 
#ifdef MTP_WAV_SUPPORT
int wav_parse(FW_CHAR * file,WAVPROP * oWavProp)
{
	int				fd;                
	char				byNeedValue[4];
	char				byNeedValue1[2];
	unsigned int 		dwFileLength;
	unsigned int 		dwSampleRate;
	unsigned int 		dwByteRate;
	unsigned int 		dwAudioBitRate;
	unsigned int 		dwDuration;
	unsigned short 	dwBitsPersample;
	unsigned short 	wChannels;

	FW_INT32 	nErrorCode;
	FW_ULONG 	ulseek_result;
	FW_ULONG 	ulread_result;	 

	memset(oWavProp,0,sizeof(WAVPROP));
	fd = BF_Fs_If_Open(file, O_RDONLY,0);
	if (fd ==0)
		return	WAV_ERR_OPEN;

	nErrorCode=BF_Fs_If_Read(fd,byNeedValue,4,&ulread_result);													
	if (ulread_result !=4)	
	{
#ifdef WAVPARSE_ERR_PRINT
		BF_Debug_Print_Error(("wav parse read error.\r\n"));
#endif	
		BF_Fs_If_Close(fd);		
		return	WAV_ERR_READ;
	}
	
	if (memcmp(byNeedValue,"RIFF",4) != 0)	
	{
#ifdef WAVPARSE_ERR_PRINT
		BF_Debug_Print_Error(("wav parse not riff error.\r\n"));
#endif	
		BF_Fs_If_Close(fd);		
		return (WAV_ERR_NOT_RIFF);							
	}

	nErrorCode=BF_Fs_If_Read(fd,byNeedValue,4,&ulread_result);		
	if (ulread_result !=4)	
	{
#ifdef WAVPARSE_ERR_PRINT
		BF_Debug_Print_Error(("wav parse read error.\r\n"));
#endif	
		BF_Fs_If_Close(fd);		
		return	WAV_ERR_READ;
	}
	
	memcpy(&dwFileLength, byNeedValue,4);
	dwFileLength = ENDIAN_SWAP32(dwFileLength);
	
	nErrorCode=BF_Fs_If_Lseek(fd,14,SEEK_CUR,&ulseek_result);
	if (nErrorCode !=0)
	{
#ifdef WAVPARSE_ERR_PRINT
		BF_Debug_Print_Error(("wav parse seek error.\r\n"));
#endif	
		BF_Fs_If_Close(fd);	
		return (WAV_ERR_SEEK);					
	}
	
	nErrorCode=BF_Fs_If_Read(fd,byNeedValue1,2,&ulread_result);	
	memcpy(&wChannels, byNeedValue1,2);	
       wChannels = ENDIAN_SWAP16(wChannels);
	oWavProp->dwNumberOfChannels = wChannels;
   
	nErrorCode=BF_Fs_If_Read(fd,byNeedValue,4,&ulread_result);	
	memcpy(&dwSampleRate, byNeedValue,4);
	dwSampleRate = ENDIAN_SWAP32(dwSampleRate);
	oWavProp->dwSampleRate = dwSampleRate;

	nErrorCode=BF_Fs_If_Read(fd,byNeedValue,4,&ulread_result);	
	memcpy(&dwByteRate, byNeedValue,4);
    	dwByteRate = ENDIAN_SWAP32(dwByteRate);
	dwAudioBitRate=dwByteRate*8;
	oWavProp->dwAudioBitRate = dwAudioBitRate;

   	nErrorCode=BF_Fs_If_Lseek(fd,2,SEEK_CUR,&ulseek_result);
	if (nErrorCode !=0)
	{
#ifdef WAVPARSE_ERR_PRINT
		BF_Debug_Print_Error(("wav parse seek error.\r\n"));
#endif	
		BF_Fs_If_Close(fd);	
		return (WAV_ERR_SEEK);				
	}

	nErrorCode=BF_Fs_If_Read(fd,byNeedValue1,2,&ulread_result);	
    	memcpy(&dwBitsPersample, byNeedValue1,2);
	dwBitsPersample = ENDIAN_SWAP16(dwBitsPersample);	

	dwDuration=dwFileLength/(dwSampleRate*wChannels*dwBitsPersample/8);
	oWavProp->dwDuration = dwDuration;
	oWavProp->dwDuration = dwDuration*1000;
	BF_Fs_If_Close(fd);

    return WAV_PARSE_OK;
}
#else //dummy function
int wav_parse(FW_CHAR * file,WAVPROP * oWavProp)
{
	BF_Debug_Print_Error(("wav parse dummy function.\r\n"));
    return WAV_PARSE_OK;
}
#endif
