/**
 * @file		avilib.c
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
#include "ddim_typedef.h"
#include "debug.h"
#include "avilib.h"   
#include "fj_std.h"

AVI  	*g_oAVIData=NULL;

//#define MTP_AVI_SUPPORT 
#ifdef MTP_AVI_SUPPORT
int avi_init(void)
{
	memset(&g_oAVIData->aviheader,0,sizeof(AVIHEADER));
	return (0);
}

// parse:
// parse the AVI file
// returns:
//    0 if success, otherwise following error code returned.
//    AVI_ERR_read        if _read error
//    AVI_ERR_NOT_RIFF    if not RIFF chunk
//    AVI_ERR_NOT_AVI    if not AVI file
//    AVI_ERR_MEM        if memory error
//    AVI_ERR_BROKEN    if AVI file is damaged

int avi_parse(FW_CHAR * file,AVIPROP *oAVIProp)

{
	AVISTREAMHEADER    avi_streamheader;
	//AVISUPERINDEX * s;
	char 		byBuff[256];

	char 		byNeedValue[4];
	char 		byNeedValue1[2];	 
	DWORD 		dwMicroSecPerFrame;     
	DWORD 		dwMaxByesPerSec;
	DWORD 		dwVideoBitRare;	 
	DWORD 		dwTotalFrames;	 	 
	DWORD 		dwDuration;
	DWORD 		dwWidth;
	DWORD 		dwHeight;
	FOURCC1 	oFccHandler;
	WORD 		wChannels;
	//DWORD  	dwSamplesPerSec;
	DWORD  	dwAvgBytesPerSec;
	DWORD  	dwAudioBitRate;
	FW_INT32 	nErrCode;
	FW_ULONG 	ulFileLen;
	FW_ULONG 	ulLength;
	FW_ULONG 	ulseek_result;
	FW_ULONG 	ulread_result;	 

	//char 				id[4];
	AVI_FOURCC_TYPE 	fourcc_type = FOURCC_UNDEF;     
	//AVIIDX1ENTRY * 	curidx;
	T_BF_FS_IF_STAT		file_stat;

	memset(oAVIProp,0,sizeof(AVIPROP));

	if (avi_new(file)== NULL)
		return AVI_PARSE_FAIL;

	avi_init();

	nErrCode = BF_Fs_If_Stat((const char*)file, (T_BF_FS_IF_STAT *)&file_stat);	
	if (nErrCode != FJ_ERR_OK)
	{
		BF_Debug_Print_Error(("Get FSIF Stat fail.\r\n"));
	}

	nErrCode=BF_Fs_If_Lseek(g_oAVIData->fd,0,SEEK_END,&ulFileLen);
	if (nErrCode !=0)
	{
		avi_del();
		return (AVI_ERR_SEEK);
	}

	nErrCode = BF_Fs_If_Lseek(g_oAVIData->fd,0,SEEK_SET,&ulseek_result);
	if (nErrCode !=0)
	{
		avi_del();
		return (AVI_ERR_SEEK);
	}

	nErrCode=BF_Fs_If_Read(g_oAVIData->fd,byBuff,12,&ulread_result);
	if (ulread_result !=12)
	{
		avi_del();
		return (AVI_ERR_READ);
	}

	if (memcmp(byBuff,"RIFF",4) != 0)
	{
		avi_del();
		return (AVI_ERR_NOT_RIFF);
	}

	if (memcmp(byBuff+8,"AVI",3) != 0)
	{
		avi_del();
		return (AVI_ERR_NOT_AVI);
	}
	
	do {
         // clear data and _read
         ulLength = 0;
         memset(byBuff, 0, 256);

		 nErrCode=BF_Fs_If_Read(g_oAVIData->fd,byBuff,8,&ulread_result);		 
		 if (ulread_result !=8)
		 {
		 	avi_del();
	 		return(AVI_ERR_READ);
		 }
		 
         fourcc_type = FOURCC_UNDEF;
         if (memcmp(byBuff,"LIST",4) ==0)
         {
            fourcc_type = FOURCC_LIST;
         }
		 else if ( memcmp(byBuff, "avih", 4) == 0 )          
     	 {
          fourcc_type = FOURCC_avih;
     	 }
		 else if ( memcmp(byBuff, "strh", 4) == 0 )          
         {
             fourcc_type = FOURCC_strh;
         }         
		 else if ( memcmp(byBuff, "strf", 4) == 0 )          
         {
             fourcc_type = FOURCC_strf;
         }            
         else if ( memcmp(byBuff, "strd", 4) == 0 )
         {
             fourcc_type = FOURCC_strd;
         }           
         else if ( memcmp(byBuff, "strn", 4) == 0 )
         {
             fourcc_type = FOURCC_strn;
         }   		 
         else if ( memcmp(byBuff, "idx1", 4) == 0 )
         {
             fourcc_type = FOURCC_idx1;
         } 		 
         else if ( memcmp(byBuff, "indx", 4) == 0 )
         {
             fourcc_type = FOURCC_indx;
         }          
         else if ( memcmp(byBuff, "JUNK", 4) == 0 )
         {
             fourcc_type = FOURCC_JUNK;
         }         
         else if ( memcmp(byBuff, "RIFF", 4) == 0 )
         {
             fourcc_type = FOURCC_RIFF;
         }  

         switch (fourcc_type) {
         	case FOURCC_LIST:
				// reserved chunk size, used by movi handler branch
				memcpy(&ulLength,byBuff+4,4);
				ulLength = ENDIAN_SWAP32(ulLength);
				ALIGN_EVEN(ulLength);            // align chunk size to even border

				BF_Fs_If_Read(g_oAVIData->fd,byBuff,4,&ulread_result);
				if (ulread_result != 4)
				{
					avi_del();
					return (AVI_ERR_READ);
				}

				if ( memcmp(byBuff, "hdrl", 4) == 0 )
				 g_oAVIData->aviheader.have_hdrl = 1;

				if ( memcmp(byBuff, "strl", 4) == 0 )
				 g_oAVIData->aviheader.have_strl = 1;

				if ( memcmp(byBuff, "rec ", 4) == 0 )
				 g_oAVIData->aviheader.have_rec = 1;

				if ( memcmp(byBuff, "movi", 4) == 0 ) {
					g_oAVIData->aviheader.have_movi = 1;             

					// get movi offset, aligned
					if(!g_oAVIData->aviheader.movi_offset){
					g_oAVIData->aviheader.movi_offset = ulseek_result;
					ALIGN_EVEN(g_oAVIData->aviheader.movi_offset);
					}
					// get movi length

					g_oAVIData->aviheader.movi_length = ulLength;
					if ( g_oAVIData->aviheader.have_idx1 )
					{
					 	nErrCode=BF_Fs_If_Lseek(g_oAVIData->fd,ulLength - 4, SEEK_CUR,&ulseek_result);
						 if (nErrCode !=0)
						 {
						 	avi_del();
							return(AVI_ERR_SEEK);
						}
					}
				}
				
				if (memcmp(byBuff, "INFO", 4) == 0) {
					nErrCode = BF_Fs_If_Lseek(g_oAVIData->fd,-12,SEEK_CUR,&ulseek_result);
					if (nErrCode !=0)
					{	
						avi_del();
						return(AVI_ERR_SEEK);
					}
					BF_Fs_If_Read(g_oAVIData->fd,byBuff,8,&ulread_result);
					if (ulread_result != 8)
					{
						avi_del();
						return (AVI_ERR_READ);
					}
					memcpy(&ulLength,byBuff+4,4);
					ulLength = ENDIAN_SWAP32(ulLength);
					ALIGN_EVEN(ulLength);            // align chunk size to even border
					nErrCode = BF_Fs_If_Lseek(g_oAVIData->fd,ulLength,SEEK_CUR,&ulseek_result);
					 if (nErrCode !=0)
					 {		
					 	avi_del();
						return(AVI_ERR_SEEK);
					 }
					}
				break;
				
			case FOURCC_avih:
             if ( g_oAVIData->aviheader.have_hdrl ) {
				// back trace 8 bytes

				BF_Fs_If_Read(g_oAVIData->fd,byNeedValue,4,&ulread_result);	
				if (ulread_result != 4)
				{
					avi_del();
					return (AVI_ERR_READ);
				}
			  
				memcpy(&dwMicroSecPerFrame, byNeedValue,4); 		  
				dwMicroSecPerFrame = ENDIAN_SWAP32(dwMicroSecPerFrame);
				BF_Fs_If_Read(g_oAVIData->fd,byNeedValue,4,&ulread_result);
				if (ulread_result != 4)
				{
					avi_del();
					return (AVI_ERR_READ);
				}
	  
				memcpy(&dwMaxByesPerSec, byNeedValue,4);
				dwMaxByesPerSec = ENDIAN_SWAP32(dwMaxByesPerSec);
				oAVIProp->dwSampleRate = dwMaxByesPerSec;	// 0xDE93

				dwVideoBitRare=dwMaxByesPerSec*8;
				oAVIProp->dwVideoBitRate = dwVideoBitRare;	//0xDE9C
			  
				nErrCode=BF_Fs_If_Lseek(g_oAVIData->fd,8,SEEK_CUR,&ulseek_result);
				if (nErrCode !=0)
				{
					avi_del();
					return (AVI_ERR_SEEK);
				}			  
				  
				BF_Fs_If_Read(g_oAVIData->fd,byNeedValue,4,&ulread_result);
				if (ulread_result != 4)
				{
					avi_del();
					return (AVI_ERR_READ);
				}

				memcpy(&dwTotalFrames, byNeedValue,4);
				dwTotalFrames = ENDIAN_SWAP32(dwTotalFrames);
              			  
				dwDuration=dwTotalFrames*dwMicroSecPerFrame/1000000;

				oAVIProp->dwDuration = dwDuration*1000;		// 0xDE89

				nErrCode=BF_Fs_If_Lseek(g_oAVIData->fd,12,SEEK_CUR,&ulseek_result);
				if (nErrCode !=0)
				{	
					avi_del();
					return(AVI_ERR_SEEK);
				}

				BF_Fs_If_Read(g_oAVIData->fd,byNeedValue,4,&ulread_result);
				if (ulread_result != 4)
				{
					avi_del();
				  	return (AVI_ERR_READ);
				}				
			  
				memcpy(&dwWidth, byNeedValue,4);
				dwWidth = ENDIAN_SWAP32(dwWidth);
				oAVIProp->dwWidth = dwWidth;		// 0xDC87		  
				BF_Fs_If_Read(g_oAVIData->fd,byNeedValue,4,&ulread_result);
				if (ulread_result != 4)
				{
					avi_del();
				  	return (AVI_ERR_READ);
				}			  
				memcpy(&dwHeight, byNeedValue,4);
				dwHeight = ENDIAN_SWAP32(dwHeight);
				oAVIProp->dwHeight = dwHeight;	//0xDC88

				nErrCode=BF_Fs_If_Lseek(g_oAVIData->fd,40,SEEK_CUR,&ulseek_result);
				if (nErrCode !=0)
				{	
					avi_del();
				  	return (AVI_ERR_SEEK);
				}

				BF_Fs_If_Read(g_oAVIData->fd,byNeedValue,4,&ulread_result);
				if (ulread_result != 4)
				{
					avi_del();
					return (AVI_ERR_READ);
				}			  
				memcpy(&oFccHandler, byNeedValue,4);
				oFccHandler[4] ='\0';

				memcpy(&(oAVIProp->dwVideoFourCCCodec),&oFccHandler,4);	//0xDE9B
				nErrCode=BF_Fs_If_Lseek(g_oAVIData->fd,-84,SEEK_CUR,&ulseek_result);
				if (nErrCode !=0)
				{		
					avi_del();
				     	return (AVI_ERR_SEEK);
				}

				  nErrCode=BF_Fs_If_Lseek(g_oAVIData->fd,-8,SEEK_CUR,&ulseek_result);
				if (nErrCode !=0)
				{		
					avi_del();
					return (AVI_ERR_SEEK);
				}
  
				ulLength = sizeof(AVIMAINHEADER);
				ALIGN_EVEN(ulLength);            // align chunk size to even border

				BF_Fs_If_Read(g_oAVIData->fd,(char*)&g_oAVIData->aviheader.mainheader,ulLength,&ulread_result);
				if (ulread_result != ulLength)
				{
					avi_del();
					return (AVI_ERR_READ);
				}			  
				g_oAVIData->aviheader.mainheader.dwFlags = ENDIAN_SWAP32(g_oAVIData->aviheader.mainheader.dwFlags);
				if ( g_oAVIData->aviheader.mainheader.dwFlags & AMHF_HASINDEX)
				 g_oAVIData->aviheader.have_idx1 = 1;
				}
				break;
				
         case FOURCC_strh:
             if ( g_oAVIData->aviheader.have_strl ) {
				// back trace 8 bytes				 
				nErrCode=BF_Fs_If_Lseek(g_oAVIData->fd,-8,SEEK_CUR,&ulseek_result);
				if (nErrCode !=0)
				{
					avi_del();
				 	return (AVI_ERR_SEEK);
				}

				ulLength = sizeof(AVISTREAMHEADER);
				ALIGN_EVEN(ulLength);            // align chunk size to even border
				BF_Fs_If_Read(g_oAVIData->fd,(char *)&avi_streamheader, ulLength,&ulread_result);
				if (ulread_result != ulLength)
				{
					avi_del();
					return (AVI_ERR_READ);
				}		
				//nErrCode=BF_Fs_If_Lseek(g_oAVIData->fd,0,SEEK_CUR,&ulseek_result);
				  
				if ( memcmp(avi_streamheader.fccType, "vids", 4) == 0 )
					g_oAVIData->aviheader.vid_streamheader = avi_streamheader;
				else if ( memcmp(avi_streamheader.fccType, "auds", 4) == 0 )
				{	
					g_oAVIData->aviheader.aud_streamheader = avi_streamheader;
					nErrCode=BF_Fs_If_Lseek(g_oAVIData->fd,10,SEEK_CUR,&ulseek_result);
					if (nErrCode !=0)
					{
						avi_del();
						return (AVI_ERR_SEEK);					
					}

					BF_Fs_If_Read(g_oAVIData->fd,byNeedValue1, 2,&ulread_result);
					if (ulread_result != 2)
					{
						avi_del();
					  	return (AVI_ERR_READ);
					}	  
						
					memcpy(&wChannels, byNeedValue1,2);
					wChannels = ENDIAN_SWAP16(wChannels);

					oAVIProp->dwNumberOfChannels=wChannels;		// 0xDE94
					BF_Fs_If_Read(g_oAVIData->fd,byNeedValue1, 4,&ulread_result);
					if (ulread_result != 4)
					{
						avi_del();
					  	return (AVI_ERR_READ);
					}	
						
					memcpy(&dwAvgBytesPerSec, byNeedValue1,4);
					dwAvgBytesPerSec = ENDIAN_SWAP32(dwAvgBytesPerSec);
					oAVIProp->dwSampleRate = dwAvgBytesPerSec;	// 0xDE93

					BF_Fs_If_Read(g_oAVIData->fd,byNeedValue1, 4,&ulread_result);
					if (ulread_result != 4)
					{
						avi_del();
						return (AVI_ERR_READ);
					}	
						
					memcpy(&dwAvgBytesPerSec, byNeedValue1,4);
					dwAvgBytesPerSec = ENDIAN_SWAP32(dwAvgBytesPerSec);

					dwAudioBitRate=dwAvgBytesPerSec*8;	
					oAVIProp->dwAudioBitRate = dwAudioBitRate;			// 0xDE9A

					nErrCode=BF_Fs_If_Lseek(g_oAVIData->fd,-20,SEEK_CUR,&ulseek_result);
					if (nErrCode !=0)
					{
						avi_del();
						return (AVI_ERR_SEEK);
					}
				}
             }
             break;
            
         case FOURCC_RIFF:
             // another RIFF List, for OpenDML

             	BF_Fs_If_Read(g_oAVIData->fd,byBuff, 4,&ulread_result);
		if (ulread_result != 4)
		{
			avi_del();
			return (AVI_ERR_READ);
		}	
			 
             if (strncmp(byBuff, "AVIX", 4))
             {
             		avi_del();
                 	return (AVI_ERR_NOT_RIFF);
             }
             else{
           		/* We got an extended AVI header, so we need to switch to
                   		  * OpenDML to get seeking to work, provided we got indx chunks
                   		 * in the header(have_indx == 1)*/
                 if( g_oAVIData->aviheader.have_indx)
                     g_oAVIData->aviheader.isodml = 1 ;
             }
             break;
         case FOURCC_strf:
         case FOURCC_strd:
         case FOURCC_strn:
         case FOURCC_JUNK:    
         default:
			// skipped
			memcpy(&ulLength,byBuff+4,4);
			ulLength = ENDIAN_SWAP32(ulLength);
			ALIGN_EVEN(ulLength);            // align chunk size to even border

			nErrCode=BF_Fs_If_Lseek( g_oAVIData->fd,ulLength, SEEK_CUR,&ulseek_result);
			if (nErrCode !=0)
			{
				avi_del();
				return (AVI_ERR_SEEK);
			}
			break;
         }
		 nErrCode=BF_Fs_If_Lseek(g_oAVIData->fd,0,SEEK_CUR,&ulseek_result);
		 if (nErrCode !=0)
		 {
		 	avi_del();
	 	 	return (AVI_ERR_SEEK);
		 }
     } while (ulseek_result < ulFileLen );
     
	avi_del();

	return AVI_PARSE_OK;
 }


/************ constructor & deconstructor ******************/
// constructor:
//    param: file - input file's name
//    return: an AVI object if success, otherwise NULL
AVI * avi_new(char * file)
{
	g_oAVIData = (AVI *)malloc(sizeof(AVI));

	if (g_oAVIData) {
	 g_oAVIData->file_name = file;
	 g_oAVIData->fd = BF_Fs_If_Open(file, O_RDONLY,0);		 
	}

	if (g_oAVIData->fd ==0 ){
		BF_Debug_Print_Error(("[AVILIB]Open File Fail.\r\n"));
	}
	  
	return (g_oAVIData?g_oAVIData:NULL);
}

// deconstructor
//    param: an AVI object
void avi_del(void)
{
	if (g_oAVIData->fd) {
		BF_Fs_If_Close(g_oAVIData->fd);
		FREE(g_oAVIData);
	}	 
}

#else	//dummy function
int avi_init(void)
{
	BF_Debug_Print_Error(("avi_init dummy function.\r\n"));
	return (0);
}

int avi_parse(FW_CHAR * file,AVIPROP *oAVIProp)

{
	BF_Debug_Print_Error(("avi_parse dummy function.\r\n"));
	return AVI_PARSE_OK;
 }

AVI * avi_new(char * file)
{
	BF_Debug_Print_Error(("avi_new dummy function.\r\n"));
	return (NULL);
}

void avi_del(void)
{ 
	BF_Debug_Print_Error(("avi_del dummy function.\r\n"));
}
#endif


