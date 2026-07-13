/**
 * @file		movlib.c
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

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ddim_typedef.h"
#include "debug.h"
#include "movlib.h"   
#include "fj_std.h"

MOV  	*g_oMOVData=NULL;

#define MTP_MOV_SUPPORT 
#ifdef MTP_MOV_SUPPORT

int mov_parse(FW_CHAR * file,MOVPROP *oMOVProp)

{
	char 		byBuff[256];
	unsigned int 		dwDuration = 0;
	unsigned short 		dwWidth = 0;
	unsigned short 		dwHeight = 0;
	unsigned short 		wChannels = 0;
	unsigned short		dwSampleSize = 0;	// Sample Size
	unsigned int		dwSampleDelta = 0;	// Sample Delta
	unsigned int		dwTimeScale = 0;	// Time Scale
	unsigned int		dwSampleRate = 0;	// Sample Rate
	unsigned int		dwSize;
	INT32		nSeekSize;
	FW_INT32 	nErrCode;
	FW_ULONG 	ulFileLen;
	FW_ULONG 	ulseek_result;
	FW_ULONG 	ulread_result;
	double		dTmpWork;
	double		dTmpLower;

	MOV_ATOM_TYPE 	atom_type = ATOM_INVALID;
	MOV_MINF_STREAM_TYPE 	stream_type = MOV_MINF_VIDEO;
	T_BF_FS_IF_STAT		file_stat;

	memset(oMOVProp,0,sizeof(MOVPROP));

	if (mov_new(file)== NULL)
		return MOV_PARSE_FAIL;

	nErrCode = BF_Fs_If_Stat((const char*)file, (T_BF_FS_IF_STAT *)&file_stat);	
	if (nErrCode != FJ_ERR_OK)
	{
		BF_Debug_Print_Error(("Get FSIF Stat fail.\r\n"));
	}

	// Get file length
	nErrCode=BF_Fs_If_Lseek(g_oMOVData->fd,0,SEEK_END,&ulFileLen);
	if (nErrCode !=0)
	{
		mov_del();
		return (MOV_ERR_SEEK);
	}

//	BF_Debug_Print_Error(("ulFileLen = %d \r\n", ulFileLen));

	nErrCode = BF_Fs_If_Lseek(g_oMOVData->fd,0,SEEK_SET,&ulseek_result);
	if (nErrCode !=0)
	{
		mov_del();
		return (MOV_ERR_SEEK);
	}

	// ftyp
	nErrCode=BF_Fs_If_Read(g_oMOVData->fd,byBuff,4,&ulread_result);
	if (ulread_result !=4)
	{
		mov_del();
		return (MOV_ERR_READ);
	}

	memcpy(&dwSize, byBuff, 4);
	nSeekSize = dwSize - 4;

	nErrCode = BF_Fs_If_Lseek(g_oMOVData->fd,nSeekSize,SEEK_CUR,&ulseek_result);
	if (nErrCode !=0)
	{	
		mov_del();
		return(MOV_ERR_SEEK);
	}

	do {
		
		// Get Atom Size
		nErrCode=BF_Fs_If_Read(g_oMOVData->fd,byBuff,4,&ulread_result);
		if (ulread_result !=4)
		{
			mov_del();
			return (MOV_ERR_READ);
		}

		memcpy(&dwSize, byBuff, 4);

		// clear data and _read
		memset(byBuff, 0, 256);

		// Get Atom Type
		nErrCode=BF_Fs_If_Read(g_oMOVData->fd,byBuff,4,&ulread_result);
		if (ulread_result !=4)
		{
		 	mov_del();
	 		return(MOV_ERR_READ);
		}

//		BF_Debug_Print_Information(("byBuff = %s\r\n", byBuff));

		atom_type = ATOM_INVALID;
		if (memcmp(byBuff,"moov",4) ==0)
		{
			atom_type = ATOM_MOOV;
		}
		else if ( memcmp(byBuff, "mvhd", 4) == 0 )
		{
			atom_type = ATOM_MVHD;
		}
		else if ( memcmp(byBuff, "trak", 4) == 0 )
		{
			atom_type = ATOM_TRAK;
		}
		else if ( memcmp(byBuff, "mdia", 4) == 0 )
		{
			atom_type = ATOM_MDIA;
		}
		else if ( memcmp(byBuff, "hdlr", 4) == 0 )
		{
			atom_type = ATOM_HDLR;
		}
		else if ( memcmp(byBuff, "minf", 4) == 0 )
		{
			atom_type = ATOM_MINF;
		}
		else if ( memcmp(byBuff, "stbl", 4) == 0 )
		{
			atom_type = ATOM_STBL;
		}
		else if ( memcmp(byBuff, "stsd", 4) == 0 )
		{
			atom_type = ATOM_STSD;
		}
		else if ( memcmp(byBuff, "avc1", 4) == 0 )
		{
			atom_type = ATOM_AVC1;
		}
		else if ( memcmp(byBuff, "stts", 4) == 0 )
		{
			atom_type = ATOM_STTS;
		}
		else if ( memcmp(byBuff, "twos", 4) == 0 )
		{
			atom_type = ATOM_TWOS;
		}

		switch (atom_type) {
			case ATOM_MOOV:
			case ATOM_TRAK:
			case ATOM_MDIA:
			case ATOM_MINF:
			case ATOM_STBL:
				break;
			case ATOM_MVHD:
			
				// Seek
				nErrCode = BF_Fs_If_Lseek(g_oMOVData->fd,12,SEEK_CUR,&ulseek_result);
				if (nErrCode !=0)
				{	
					mov_del();
					return(MOV_ERR_SEEK);
				}

				// Get TimeScale
				nErrCode=BF_Fs_If_Read(g_oMOVData->fd,byBuff,4,&ulread_result);
				if (ulread_result !=4)
				{
					mov_del();
					return (MOV_ERR_READ);
				}
				
				memcpy(&dwTimeScale, byBuff, 4);
				
				BF_Debug_Print_Information(("ATOM_MVHD dwTimeScale = %d\r\n", dwTimeScale));

				// Get Duration
				nErrCode=BF_Fs_If_Read(g_oMOVData->fd,byBuff,4,&ulread_result);
				if (ulread_result !=4)
				{
					mov_del();
					return (MOV_ERR_READ);
				}

				memcpy(&dwDuration, byBuff, 4);
				BF_Debug_Print_Information(("ATOM_MVHD dwDuration = %d\r\n", dwDuration));

				// Seek
				nSeekSize = dwSize - 28;
				nErrCode = BF_Fs_If_Lseek(g_oMOVData->fd,nSeekSize,SEEK_CUR,&ulseek_result);
				if (nErrCode !=0)
				{	
					mov_del();
					return(MOV_ERR_SEEK);
				}
				break;
			case ATOM_HDLR:
				// Seek
				nErrCode = BF_Fs_If_Lseek(g_oMOVData->fd,8,SEEK_CUR,&ulseek_result);
				if (nErrCode !=0)
				{	
					mov_del();
					return(MOV_ERR_SEEK);
				}

				// Get StreamType
				nErrCode=BF_Fs_If_Read(g_oMOVData->fd,byBuff,4,&ulread_result);
				if (ulread_result !=4)
				{
					mov_del();
					return (MOV_ERR_READ);
				}

				if ( memcmp(byBuff, "vide", 4) == 0 )
				{
					stream_type = MOV_MINF_VIDEO;
				}
				else
				{
					stream_type = MOV_MINF_SOUND;
				}

//				BF_Debug_Print_Information(("ATOM_HDLR stream_type = %d\r\n", stream_type));

				// Seek
				nSeekSize = dwSize - 20;
				nErrCode = BF_Fs_If_Lseek(g_oMOVData->fd,nSeekSize,SEEK_CUR,&ulseek_result);
				if (nErrCode !=0)
				{	
					mov_del();
					return(MOV_ERR_SEEK);
				}
				break;
			case ATOM_AVC1:

				// Seek
				nErrCode = BF_Fs_If_Lseek(g_oMOVData->fd,24,SEEK_CUR,&ulseek_result);
				if (nErrCode !=0)
				{	
					mov_del();
					return(MOV_ERR_SEEK);
				}

				// Get Width
				nErrCode=BF_Fs_If_Read(g_oMOVData->fd,byBuff,2,&ulread_result);
				if (ulread_result !=2)
				{
					mov_del();
					return (MOV_ERR_READ);
				}
				
				memcpy(&dwWidth, byBuff, 2);

//				BF_Debug_Print_Information(("ATOM_AVC1 dwWidth = %d\r\n", dwWidth));

				// Get Height
				nErrCode=BF_Fs_If_Read(g_oMOVData->fd,byBuff,2,&ulread_result);
				if (ulread_result !=2)
				{
					mov_del();
					return (MOV_ERR_READ);
				}
				
				memcpy(&dwHeight, byBuff, 2);

//				BF_Debug_Print_Information(("ATOM_AVC1 dwHeight = %d\r\n", dwHeight));

				// Seek
				nSeekSize = dwSize - 36;
				nErrCode = BF_Fs_If_Lseek(g_oMOVData->fd,nSeekSize,SEEK_CUR,&ulseek_result);
				if (nErrCode !=0)
				{	
					mov_del();
					return(MOV_ERR_SEEK);
				}
				break;
			case ATOM_STSD:
				// Seek
				nSeekSize = 8;
				nErrCode = BF_Fs_If_Lseek(g_oMOVData->fd,nSeekSize,SEEK_CUR,&ulseek_result);
				if (nErrCode !=0)
				{	
					mov_del();
					return(MOV_ERR_SEEK);
				}
				break;
			case ATOM_STTS:
				if (stream_type == MOV_MINF_VIDEO)
				{
					// Seek
					nErrCode = BF_Fs_If_Lseek(g_oMOVData->fd,12,SEEK_CUR,&ulseek_result);
					if (nErrCode !=0)
					{	
						mov_del();
						return(MOV_ERR_SEEK);
					}

					// Get sample-delta
					nErrCode=BF_Fs_If_Read(g_oMOVData->fd,byBuff,4,&ulread_result);
					if (ulread_result !=4)
					{
						mov_del();
						return (MOV_ERR_READ);
					}
					
					memcpy(&dwSampleDelta, byBuff, 4);
//					BF_Debug_Print_Information(("dwSampleDelta = %d\r\n", dwSampleDelta));

				}
				else
				{
					// Seek
					nErrCode = BF_Fs_If_Lseek(g_oMOVData->fd,16,SEEK_CUR,&ulseek_result);
					if (nErrCode !=0)
					{	
						mov_del();
						return(MOV_ERR_SEEK);
					}
				}
				break;

			case ATOM_TWOS:
				// Seek -> General Structure of a Sample Description
				nErrCode = BF_Fs_If_Lseek(g_oMOVData->fd,8,SEEK_CUR,&ulseek_result);
				if (nErrCode !=0)
				{	
					mov_del();
					return(MOV_ERR_SEEK);
				}

				// Seek -> Sound Sample Description (Version 0)
				nErrCode = BF_Fs_If_Lseek(g_oMOVData->fd,8,SEEK_CUR,&ulseek_result);
				if (nErrCode !=0)
				{	
					mov_del();
					return(MOV_ERR_SEEK);
				}

				// Get Number of channels
				nErrCode=BF_Fs_If_Read(g_oMOVData->fd,byBuff,2,&ulread_result);
				if (ulread_result !=2)
				{
					mov_del();
					return (MOV_ERR_READ);
				}
				
				memcpy(&wChannels, byBuff, 2);
//				BF_Debug_Print_Information(("ATOM_TWOS wChannels = %d\r\n", wChannels));

				// Get Sample size
				nErrCode=BF_Fs_If_Read(g_oMOVData->fd,byBuff,2,&ulread_result);
				if (ulread_result !=2)
				{
					mov_del();
					return (MOV_ERR_READ);
				}
				
				memcpy(&dwSampleSize, byBuff, 2);
//				BF_Debug_Print_Information(("ATOM_TWOS dwSampleSize = %d\r\n", dwSampleSize));

				// Seek
				nErrCode = BF_Fs_If_Lseek(g_oMOVData->fd,4,SEEK_CUR,&ulseek_result);
				if (nErrCode !=0)
				{	
					mov_del();
					return(MOV_ERR_SEEK);
				}

				// Get Sample rate
				nErrCode=BF_Fs_If_Read(g_oMOVData->fd,byBuff,4,&ulread_result);
				if (ulread_result !=4)
				{
					mov_del();
					return (MOV_ERR_READ);
				}
				
				memcpy(&dwSampleRate, byBuff, 4);
//				BF_Debug_Print_Information(("ATOM_TWOS dwSampleRate = 0x%X\r\n", dwSampleRate));

				// Seek
				nSeekSize = dwSize - 36;
				nErrCode = BF_Fs_If_Lseek(g_oMOVData->fd,nSeekSize,SEEK_CUR,&ulseek_result);
				if (nErrCode !=0)
				{	
					mov_del();
					return(MOV_ERR_SEEK);
				}
				break;
			default:
				// skipped
				nSeekSize = dwSize - 8;
				nErrCode=BF_Fs_If_Lseek( g_oMOVData->fd,nSeekSize,SEEK_CUR,&ulseek_result);
				if (nErrCode !=0)
				{
					mov_del();
					return (MOV_ERR_SEEK);
				}
				break;
		}

		nErrCode=BF_Fs_If_Lseek(g_oMOVData->fd,0,SEEK_CUR,&ulseek_result);
		if (nErrCode !=0)
		{
			mov_del();
			return (MOV_ERR_SEEK);
		}

		if(dwSize == 0)
		{
			BF_Debug_Print_Information(("dwSize == 0!!\r\n"));
			ulseek_result = ulFileLen;
		}

//		BF_Debug_Print_Information(("Seek Position ulseek_result = 0x%X\r\n", ulseek_result));

	} while (ulseek_result < ulFileLen );
	
	if (dwTimeScale == 0)
	{
		BF_Debug_Print_Error(("dwTimeScale value fail.\r\n"));
		return (MOV_PARSE_FAIL);
	}
	
	oMOVProp->dwWidth = dwWidth;
	oMOVProp->dwHeight = dwHeight;
	oMOVProp->dwDuration = (dwDuration * 1000) / dwTimeScale;
	dTmpLower = modf((dwDuration * 1000) / dwTimeScale, &dTmpWork);
	if(0.5 <= dTmpLower)
	{
		oMOVProp->dwDuration++;
	}

	oMOVProp->dwSampleRate = (dwSampleRate >> 16) & 0x0000FFFF;
	oMOVProp->dwNumberOfChannels = wChannels;
	oMOVProp->dwAudioBitRate = ((dwSampleRate >> 16) & 0x0000FFFF) * dwSampleSize * wChannels;

#if 1
	BF_Debug_Print_Information(("MOV Duration     = %d\r\n", dwDuration));
	BF_Debug_Print_Information(("MOV TimeScale    = %d\r\n", dwTimeScale));
	BF_Debug_Print_Information(("MOV Width        = 0x%X\r\n", oMOVProp->dwWidth));
	BF_Debug_Print_Information(("MOV Height       = 0x%X\r\n", oMOVProp->dwHeight));
	BF_Debug_Print_Information(("MOV DurationTime = 0x%X\r\n", oMOVProp->dwDuration));
	BF_Debug_Print_Information(("MOV SampleRate   = 0x%X\r\n", oMOVProp->dwSampleRate));
	BF_Debug_Print_Information(("MOV NumberOfChannels = 0x%X\r\n", oMOVProp->dwNumberOfChannels));
	BF_Debug_Print_Information(("MOV AudioBitRate = 0x%X\r\n", oMOVProp->dwAudioBitRate));
#endif

	mov_del();

	return MOV_PARSE_OK;
 }


/************ constructor & deconstructor ******************/
// constructor:
//	  param: file - input file's name
//	  return: an MOV object if success, otherwise NULL
MOV * mov_new(char * file)
{
	g_oMOVData = (MOV *)malloc(sizeof(MOV));

	if (g_oMOVData) {
	 g_oMOVData->file_name = file;
	 g_oMOVData->fd = BF_Fs_If_Open(file, D_BF_FS_IF_O_RDONLY,0);		 
	}

	if (g_oMOVData->fd ==0 ){
		BF_Debug_Print_Error(("[MOVLIB]Open File Fail.\r\n"));
	}
	  
	return (g_oMOVData?g_oMOVData:NULL);
}

// deconstructor
//	  param: an MOV object
void mov_del(void)
{
	if (g_oMOVData->fd) {
		BF_Fs_If_Close(g_oMOVData->fd);
		FREE(g_oMOVData);
	}	 
}

#else	//dummy function
int mov_init(void)
{
	BF_Debug_Print_Error(("MOV_init dummy function.\r\n"));
	return (0);
}

int mov_parse(FW_CHAR * file,MOVPROP *oMOVProp)

{
	BF_Debug_Print_Error(("MOV_parse dummy function.\r\n"));
	return MOV_PARSE_OK;
 }

MOV * mov_new(char * file)
{
	BF_Debug_Print_Error(("MOV_new dummy function.\r\n"));
	return (NULL);
}

void mov_del(void)
{ 
	BF_Debug_Print_Error(("mov_del dummy function.\r\n"));
}
#endif


