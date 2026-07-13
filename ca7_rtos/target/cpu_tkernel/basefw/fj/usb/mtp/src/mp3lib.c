/**
 * @file		mp3lib.c
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
#include "mp3lib.h"

//#define MP3PARSE_ERR_PRINT

#define MPEG1_MONO_SIDEINFO_SIZE		17
#define MPEG1_NOT_MONO_SIDEINFO_SIZE	32
#define MPEG_2_25_MONO_SIDEINFO_SIZE	 9
#define MPEG_2_25_NOT_MONO_SIDEINFO_SIZE 17
#define MPEG_AUDIO_HEADER_SIZE			 4

//#define MTP_MP3_SUPPORT 
#ifdef MTP_MP3_SUPPORT
extern int strcmpi( const char* a, const char* b);

static int MP3BitRate[4][16] = {
	{0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, -1},
	{0, 32, 40, 48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, -1},
	{0, 32, 48, 56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384, -1},
	{0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, -1},
};

 void  mp3_parse(FW_CHAR* file,MP3PROP *omp3Prop)
 {
	int	IsVBR;
	int	IsMPEG1;
	int	Ismono; 

	unsigned char FourByte[4];
	unsigned char TwoByte[2];
	unsigned char OneByte[1];

	int fd;
	FW_ULONG file_len;
	int m_nFrameNrs;//m_nFrameNrs: 	Number of Frames 
	int Padding;
	int Duration;
	int m_nNumberOfChannels;
	float m_fMP3SampleRate;
	float m_fFrameLen;
	float m_nMP3BitRate;

	int index1;
	int index2;
	int nOffset=0;
	int nReadCount=0;
	int i;

	int nErrorCode;
	
	unsigned char TempFour[4];
	unsigned char TempTwo[2];
 	unsigned char TempOne[1];
	unsigned char byTempBuf[1024];
    	unsigned char LayerValue;
	unsigned long ulseek_result;
	unsigned long ulread_result;
	BOOL bFound=FALSE;

	
	IsVBR=0;
	IsMPEG1=0;
	Ismono=0;

#ifdef MP3PARSE_ERR_PRINT
		BF_Debug_Print_Error(("mp3 file name is %s.\r\n",file));
#endif

	memset(omp3Prop,0,sizeof(MP3PROP));
	fd = BF_Fs_If_Open(file, O_RDONLY,0);
	if (fd ==0)
		return ;

	nErrorCode=BF_Fs_If_Lseek(fd,0,SEEK_END,&file_len);
	if (nErrorCode != 0)
	{
#ifdef MP3PARSE_ERR_PRINT
		BF_Debug_Print_Error(("mp3 parse seek error.\r\n"));
#endif
		if (fd)
			BF_Fs_If_Close(fd);
		
		return;
	}


	nErrorCode=BF_Fs_If_Lseek(fd,0,SEEK_SET,&ulseek_result);
	if (nErrorCode != 0)
	{
#ifdef MP3PARSE_ERR_PRINT
		BF_Debug_Print_Error(("mp3 parse seek error.\r\n"));
#endif
		if (fd)
			BF_Fs_If_Close(fd);	

		return;
	}

	 nErrorCode = BF_Fs_If_Read(fd,TwoByte,2,&ulread_result);
	 if (nErrorCode != 0)
	 {
#ifdef MP3PARSE_ERR_PRINT
		 BF_Debug_Print_Error(("mp3 parse read error.\r\n"));
#endif
		if (fd)
			BF_Fs_If_Close(fd);
		 return;
	 }

	 memcpy(TempTwo, TwoByte,2);

	if ((TempTwo[0]==0xFF)&& ((TempTwo[1]&0xF0)==0xF0))
		bFound = TRUE;
	
	while (bFound == FALSE)
	{
		nErrorCode=BF_Fs_If_Lseek(fd,nOffset,SEEK_SET,&ulseek_result);
		if (nErrorCode != 0)
		{
#ifdef MP3PARSE_ERR_PRINT
					 BF_Debug_Print_Error(("mp3 parse seek error.\r\n"));
#endif
			if (fd)
				BF_Fs_If_Close(fd);
			return;
		}		
		nErrorCode = BF_Fs_If_Read(fd,byTempBuf,1024,&ulread_result);
		if (nErrorCode != 0)
		{
#ifdef MP3PARSE_ERR_PRINT
					 BF_Debug_Print_Error(("mp3 parse read error.\r\n"));
#endif
			if (fd)
				BF_Fs_If_Close(fd);
			return;
		}		
		nReadCount++;
	
		for (i=0;i<1024;i++)
		{
			nOffset++;
			memcpy(TempTwo, byTempBuf+(nOffset-(nReadCount-1) * 1024),2);
			if ((TempTwo[0]==0xFF)&& ((TempTwo[1]&0xF0)==0xF0))
			{
				bFound = TRUE;	
				nErrorCode=BF_Fs_If_Lseek(fd,nOffset+2,SEEK_SET,&ulseek_result);
				if (nErrorCode != 0)
				{
#ifdef MP3PARSE_ERR_PRINT
							 BF_Debug_Print_Error(("mp3 parse read error.\r\n"));
#endif

					if (fd)
						BF_Fs_If_Close(fd);
					return;
				}				
				break;
			}
		}
	}

	if ((TempTwo[0]==0xFF)&& ((TempTwo[1]&0xF0)==0xF0))
	{	
#ifdef MP3PARSE_ERR_PRINT
		BF_Debug_Print_Error(("find mp3 header start.\r\n"));
#endif	
		if (TempTwo[1]&0x08) 
		{
			IsMPEG1=1;
#ifdef MP3PARSE_ERR_PRINT
		BF_Debug_Print_Error(("MPEG1.\r\n"));
#endif				
			//printf("MEPG-1!!!\n");
		}
		else
		{
			IsMPEG1=0;
#ifdef MP3PARSE_ERR_PRINT
			BF_Debug_Print_Error(("not MPEG1.\r\n"));
#endif				
		}
		
	    	LayerValue=(TempTwo[1]&0x06)>>1;
  
        	index1=(int)LayerValue;
		 
		switch(LayerValue) 
		{
			case 0x11:
				//printf("Layer I!!!\n");
				break;
			case 0x10:
				//printf("Layer II!!!\n");
				break;
			case 0x01:
				//printf("Layer III!!!\n");			
				break;
			default:
			   //printf("Serverd!!!\n");
			   break;
		}
	 
		Padding=(int)(TempTwo[1]&0x01);

		if (Padding) 
			Padding=0;
		else
			Padding=1;

		BF_Fs_If_Read(fd,OneByte,1,&ulread_result);
		memcpy(TempOne, OneByte,1);
        	index2=(int)(((*TempOne)&0xF0)>>4);

	    	m_nMP3BitRate=(float)(MP3BitRate[index1][index2]);
		//printf("MP3BitRate!!! %f\n",m_nMP3BitRate);
		omp3Prop->fMP3BitRate = m_nMP3BitRate;

#ifdef MP3PARSE_ERR_PRINT
		BF_Debug_Print_Error(("m_nMP3BitRate is %d.\r\n",m_nMP3BitRate));
#endif
		
		switch((((*TempOne)&0x06)>>1)) 
		{
			case 0x00:
				m_fMP3SampleRate=44.1;
				omp3Prop->fMP3SampleRate = m_fMP3SampleRate;
				 //printf("MP3SampleRate is %.1f!!!\n",m_fMP3SampleRate);
				break;
			case 0x01:
				m_fMP3SampleRate=48.0;
				omp3Prop->fMP3SampleRate = m_fMP3SampleRate;
				 //printf("MP3SampleRate is %.1f!!!\n",m_fMP3SampleRate);
				break;
			case 0x10:
				m_fMP3SampleRate=32.0;
				omp3Prop->fMP3SampleRate = m_fMP3SampleRate;
				 //printf("MP3SampleRate is %.1f!!!\n",m_fMP3SampleRate);
				break;
			default:
				//printf("Serverd!!!\n");
				break;
		}
		omp3Prop->fMP3SampleRate = m_fMP3SampleRate*1000;

		BF_Fs_If_Read(fd,OneByte,1,&ulread_result);
		memcpy(TempOne, OneByte,1);

		switch((((*TempOne)&0xC0)>>6))
		{
			case 0x11:
				Ismono=1;
				m_nNumberOfChannels=1;
			    break;
			default:
				Ismono=0;
				m_nNumberOfChannels=2;
		}
		omp3Prop->nNumberOfChannels=m_nNumberOfChannels;
	
		if (IsMPEG1) 
		{
			if (Ismono) 
			{
				nErrorCode=BF_Fs_If_Lseek(fd,nOffset+MPEG1_MONO_SIDEINFO_SIZE+MPEG_AUDIO_HEADER_SIZE,SEEK_SET,&ulseek_result);
				if (nErrorCode != 0)
				{
#ifdef MP3PARSE_ERR_PRINT
					BF_Debug_Print_Error(("mp3 parse seek error.\r\n"));
#endif
					if (fd)
						BF_Fs_If_Close(fd);
					return;
				}				
				BF_Fs_If_Read(fd,FourByte,4,&ulread_result);
				memcpy(TempFour, (const char*)FourByte,4);
			
				if (!(strcmpi((const char*)TempFour, "Xing")))
					IsVBR=1;
				else
					IsVBR=0;
					
			}
			else
			{
				nErrorCode=BF_Fs_If_Lseek(fd,nOffset+MPEG1_NOT_MONO_SIDEINFO_SIZE+MPEG_AUDIO_HEADER_SIZE,SEEK_SET,&ulseek_result);				
				if (nErrorCode != 0)
				{
#ifdef MP3PARSE_ERR_PRINT
					BF_Debug_Print_Error(("mp3 parse seek error.\r\n"));
#endif
					if (fd)
						BF_Fs_If_Close(fd);
					return;
				}				
				BF_Fs_If_Read(fd,FourByte,4,&ulread_result);
				memcpy(TempFour, FourByte,4);

				if (!(strcmpi((const char*)TempFour, "Xing")))
					IsVBR=1;
				else
					IsVBR=0;
	
			}
		}
		else
		{
			if (Ismono) 
			{
				nErrorCode=BF_Fs_If_Lseek(fd,nOffset+MPEG_2_25_MONO_SIDEINFO_SIZE+MPEG_AUDIO_HEADER_SIZE,SEEK_SET,&ulseek_result);
				if (nErrorCode != 0)
				{
#ifdef MP3PARSE_ERR_PRINT
					BF_Debug_Print_Error(("mp3 parse seek error.\r\n"));
#endif
					if (fd)
						BF_Fs_If_Close(fd);
					return;
				}				
				BF_Fs_If_Read(fd,FourByte,4,&ulread_result);
				memcpy(TempFour, FourByte,4);

				if (!(strcmpi((const char*)TempFour, "Xing")))
					IsVBR=1;
				else
					IsVBR=0;			
			}
			else
			{
				nErrorCode=BF_Fs_If_Lseek(fd,nOffset+MPEG_2_25_NOT_MONO_SIDEINFO_SIZE+MPEG_AUDIO_HEADER_SIZE,SEEK_SET,&ulseek_result);
				if (nErrorCode != 0)
				{
#ifdef MP3PARSE_ERR_PRINT
					BF_Debug_Print_Error(("mp3 parse seek error.\r\n"));
#endif
					if (fd)
						BF_Fs_If_Close(fd);
					return;
				}				
				BF_Fs_If_Read(fd,FourByte,4,&ulread_result);
				memcpy(TempFour, FourByte,4);
			
				if (!(strcmpi((const char *)TempFour, "Xing"))) 
					IsVBR=1;
				else
					IsVBR=0;		

			}

		}
		
		//BF_Debug_Print_Error(("IsVBR is %d !!!\n",IsVBR));

		if (IsVBR) 
		
		{
			nErrorCode=BF_Fs_If_Lseek(fd,4,SEEK_CUR,&ulseek_result);
			if (nErrorCode != 0)
			{
#ifdef MP3PARSE_ERR_PRINT
				BF_Debug_Print_Error(("mp3 parse seek error.\r\n"));
#endif
				if (fd)
					BF_Fs_If_Close(fd);
				return;
			}			

			BF_Fs_If_Read(fd,FourByte,4,&ulread_result);
			memcpy(&m_nFrameNrs, FourByte,4);
			Duration=(m_nFrameNrs*26/1000)+1;
#ifdef MP3PARSE_ERR_PRINT
			BF_Debug_Print_Error(("== VBR Duration is %d ==\r\n",Duration));
#endif			
			//omp3Prop->nDuration = Duration;
			omp3Prop->nDuration = Duration*1000;
			m_nMP3BitRate=file_len/Duration*8/1024;
			omp3Prop->fMP3BitRate = m_nMP3BitRate;
			omp3Prop->fMP3BitRate = m_nMP3BitRate*1000;
#ifdef MP3PARSE_ERR_PRINT
			BF_Debug_Print_Error(("== VBR m_nMP3BitRate is %d ==\r\n",m_nMP3BitRate));
#endif				
			//BF_Debug_Print_Error(("MP3BitRate!!! %f\n",m_nMP3BitRate));
		
		}
		else
		{
			m_fFrameLen = ((144 * m_nMP3BitRate / m_fMP3SampleRate ) + Padding);
			m_nFrameNrs = m_fFrameLen;
			Duration=((file_len/m_nFrameNrs)*26/1000)+1;
#ifdef MP3PARSE_ERR_PRINT
			BF_Debug_Print_Error(("== Not VBR Duration is %d ==\r\n",Duration));
#endif					
			//omp3Prop->nDuration = Duration;
			omp3Prop->nDuration = Duration*1000;
			//BF_Debug_Print_Error(("Duration is %d Sec!!!\n",Duration));
		    
			//BF_Debug_Print_Error(("MP3BitRate!!! %f\n",m_nMP3BitRate));
		}
	}
	else
	{
#ifdef MP3PARSE_ERR_PRINT
		BF_Debug_Print_Error(("not tiff file.\r\n"));
#endif
		if (fd)
			BF_Fs_If_Close(fd);
		return;	 
	}

	if (fd)
		BF_Fs_If_Close(fd);

	return;
   
 }
#else
void  mp3_parse(FW_CHAR* file,MP3PROP *omp3Prop)
{
	BF_Debug_Print_Error(("mp3_parse dummy function.\r\n"));
	return;
}
#endif
