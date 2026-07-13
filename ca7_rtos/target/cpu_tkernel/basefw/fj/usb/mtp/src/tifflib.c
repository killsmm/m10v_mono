/**
 * @file		tifflib.c
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
#include "tifflib.h"

#define TIFF_PARSE_OK		0x00000000
#define TIFF_ERR_READ        0x00000001    	
#define	TIFF_ERR_NOT_RIFF    0x00000002   
#define TIFF_ERR_OPEN		0x00000003
#define TIFF_ERR_SEEK		0x00000006 

//#define TIFPARSE_ERR_PRINT

#define TIFF_LITTLEENDIAN 0x1
#define TIFF_BIGENDIAN    0x2

#define TIFF_IFDTAG_IMAGEWIDTH 	0x0100
#define TIFF_IFDTAG_IMAGEHEIGHT 	0x0101
#define TIFF_IFDTAG_IMAGEDEPTH	0x0102

//#define MTP_TIFF_SUPPORT 
#ifdef MTP_TIFF_SUPPORT
int  tiff_parse(char * file,TIFFPROP* poTiffProp)
{
	unsigned char FourByte[4];
	unsigned char TwoByte[2];

	FW_INT32 nErrorCode=0;
	unsigned long ulseek_result;
	unsigned long ulread_result;

	int  fd;

	int  Flagg;

	unsigned short count;
	unsigned long  TempFour;
	unsigned long PixelCount;
	unsigned long PixelLen;
	 
	unsigned short  len;
	unsigned short  PixelTemp;
	unsigned short  PixelValue;
	unsigned short  sIFDTagValue=0;


	unsigned char TempTwo[2];

	unsigned char	cByteOrder=0x00;	// 0x01 : littleendian
										// 0x02 : bigendian
	unsigned short  sWidth=0;
	unsigned short  sHeight=0;

 	
	count=0;
	PixelCount=0;
	PixelValue=0; 
    	Flagg=0;

	memset(poTiffProp,0,sizeof(TIFFPROP));
	fd = BF_Fs_If_Open(file, O_RDONLY,0);
	if (fd ==0)
		return	TIFF_ERR_OPEN;

	nErrorCode=BF_Fs_If_Read(fd,TwoByte,2,&ulread_result);													
	if (ulread_result !=2)	
	{
#ifdef TIFPARSE_ERR_PRINT
		BF_Debug_Print_Error(("tif parse read error.\r\n"));
#endif	
		BF_Fs_If_Close(fd);		
		return	 TIFF_ERR_READ;
	}

	memcpy(TempTwo, TwoByte,2);
	 
	if (((TempTwo[0]==0x49)&&(TempTwo[1]==0x49))||((TempTwo[0]==0x4D)&&(TempTwo[1]==0x4D))) 
	{
		if ( (TempTwo[0] == 0x49) && (TempTwo[1] == 0x49) )
			cByteOrder = TIFF_LITTLEENDIAN;
		else if ( (TempTwo[0] == 0x4D ) && (TempTwo[1] == 0x4D) )
			cByteOrder = TIFF_BIGENDIAN;

#ifdef TIFPARSE_ERR_PRINT
		if (cByteOrder == TIFF_LITTLEENDIAN)
			BF_Debug_Print_Error(("little endian %d,%c%c\r\n",cByteOrder,TempTwo[0],TempTwo[1]));
		else if (cByteOrder == TIFF_BIGENDIAN)
			BF_Debug_Print_Error(("big endian %d,%c%c\r\n",cByteOrder,TempTwo[0],TempTwo[1]));
#endif	

		nErrorCode=BF_Fs_If_Lseek(fd,2,SEEK_CUR,&ulseek_result);
		if (nErrorCode !=0)
		{
#ifdef TIFPARSE_ERR_PRINT
			BF_Debug_Print_Error(("tif parse seek error.\r\n"));
#endif	
			BF_Fs_If_Close(fd);				
			return (TIFF_ERR_SEEK);					
		}		

		nErrorCode=BF_Fs_If_Read(fd,FourByte,4,&ulread_result);													
		if (ulread_result !=4)	
		{
#ifdef TIFPARSE_ERR_PRINT
			BF_Debug_Print_Error(("tif parse read error.\r\n"));
#endif	
			BF_Fs_If_Close(fd);				
			return	 TIFF_ERR_READ;
		}
		memcpy(&TempFour, FourByte,4);

		if (cByteOrder == TIFF_LITTLEENDIAN)
			TempFour = ENDIAN_SWAP32(TempFour);		

#ifdef TIFPARSE_ERR_PRINT
		BF_Debug_Print_Error(("First Offset is 0x%xh\r\n",TempFour));
#endif	

		nErrorCode=BF_Fs_If_Lseek(fd,TempFour,SEEK_SET,&ulseek_result);
		if (nErrorCode !=0)
		{
#ifdef TIFPARSE_ERR_PRINT
			BF_Debug_Print_Error(("tif parse read error.\r\n"));
#endif	
			BF_Fs_If_Close(fd);			
			return (TIFF_ERR_SEEK);					
		}	
		
		nErrorCode=BF_Fs_If_Read(fd,TwoByte,2,&ulread_result);													
		if (ulread_result !=2)	
		{
#ifdef TIFPARSE_ERR_PRINT
			BF_Debug_Print_Error(("tif parse read error.\r\n"));
#endif	
			BF_Fs_If_Close(fd);			
			return	 TIFF_ERR_READ;		
		}
		memcpy(&len, TwoByte,2);

		if (cByteOrder == TIFF_LITTLEENDIAN)
			len = ENDIAN_SWAP16(len);

		while (count<len) 
		{
			nErrorCode=BF_Fs_If_Read(fd,TwoByte,2,&ulread_result);													
			if (ulread_result !=2)		
			{
#ifdef TIFPARSE_ERR_PRINT
				BF_Debug_Print_Error(("tif parse read error.\r\n"));
#endif	
				BF_Fs_If_Close(fd);				
				return	 TIFF_ERR_READ;	
			}

			memcpy(&sIFDTagValue,TwoByte,2);

			if (cByteOrder == TIFF_LITTLEENDIAN)
			{
				sIFDTagValue = ENDIAN_SWAP16(sIFDTagValue);
			}

#ifdef TIFPARSE_ERR_PRINT
			BF_Debug_Print_Error(("Tag1 value is  0x%xh\r\n",sIFDTagValue));
#endif		
 
			if (sIFDTagValue == TIFF_IFDTAG_IMAGEWIDTH)
			{
				nErrorCode=BF_Fs_If_Lseek(fd,6,SEEK_CUR,&ulseek_result);
				if (nErrorCode !=0)
				{
#ifdef TIFPARSE_ERR_PRINT
					BF_Debug_Print_Error(("tif parse seek error.\r\n"));
#endif	
					BF_Fs_If_Close(fd);					
					return (TIFF_ERR_SEEK);					
				}					

				nErrorCode=BF_Fs_If_Read(fd,TwoByte,2,&ulread_result);	
				if (ulread_result !=2)													
					return	 TIFF_ERR_READ;					

				memcpy(&sWidth,TwoByte,2);
				nErrorCode=BF_Fs_If_Lseek(fd,2,SEEK_CUR,&ulseek_result);
				if (cByteOrder == TIFF_LITTLEENDIAN)
					poTiffProp->dwWidth = ENDIAN_SWAP16(sWidth);
				else
					poTiffProp->dwWidth = sWidth;

#ifdef TIFPARSE_ERR_PRINT
				BF_Debug_Print_Error(("Width is 0x%xh\r\n",poTiffProp->dwWidth));
#endif
				
				Flagg+=1;			
			}
	   
			else if (sIFDTagValue == TIFF_IFDTAG_IMAGEHEIGHT)
			{
				nErrorCode=BF_Fs_If_Lseek(fd,6,SEEK_CUR,&ulseek_result);
				if (nErrorCode !=0)
				{
#ifdef TIFPARSE_ERR_PRINT
					BF_Debug_Print_Error(("tif parse seek error.\r\n"));
#endif	
					BF_Fs_If_Close(fd);					
					return (TIFF_ERR_SEEK);					
				}

				nErrorCode=BF_Fs_If_Read(fd,TwoByte,2,&ulread_result);	
				if (ulread_result !=2)													
					return	 TIFF_ERR_READ;					

				memcpy(&sHeight,TwoByte,2);
				nErrorCode=BF_Fs_If_Lseek(fd,2,SEEK_CUR,&ulseek_result);
				if (cByteOrder == TIFF_LITTLEENDIAN)
					poTiffProp->dwHeight = ENDIAN_SWAP16(sHeight);
				else
					poTiffProp->dwHeight = sHeight;

#ifdef TIFPARSE_ERR_PRINT
				BF_Debug_Print_Error(("Height is 0x%xh\r\n",poTiffProp->dwHeight));
#endif
				Flagg+=1;				
			}
	   
			else if (sIFDTagValue == TIFF_IFDTAG_IMAGEDEPTH)
			{

				nErrorCode=BF_Fs_If_Lseek(fd,2,SEEK_CUR,&ulseek_result);
				if (nErrorCode !=0)
				{
#ifdef TIFPARSE_ERR_PRINT
					BF_Debug_Print_Error(("tif parse seek error.\r\n"));
#endif	
					BF_Fs_If_Close(fd);					
					return (TIFF_ERR_SEEK);					
				}				

				nErrorCode=BF_Fs_If_Read(fd,FourByte,4,&ulread_result);													
				if (ulread_result !=4)		
				{
#ifdef TIFPARSE_ERR_PRINT
					BF_Debug_Print_Error(("tif parse read error.\r\n"));
#endif	
					BF_Fs_If_Close(fd);					
					return	 TIFF_ERR_READ;					
				}
				memcpy(&PixelLen, FourByte,4);

				if (cByteOrder == TIFF_LITTLEENDIAN)
					PixelLen= ENDIAN_SWAP32(PixelLen);

#ifdef TIFPARSE_ERR_PRINT
				BF_Debug_Print_Error(("PixNum is %d\r\n",PixelLen));
#endif

				nErrorCode=BF_Fs_If_Read(fd,FourByte,4,&ulread_result);													
				if (ulread_result !=4)	
				{
#ifdef TIFPARSE_ERR_PRINT
					BF_Debug_Print_Error(("tif parse read error.\r\n"));
#endif	
					BF_Fs_If_Close(fd);					
					return	 TIFF_ERR_READ;				    
				}
				memcpy(&TempFour, FourByte,4);
				
				if (cByteOrder == TIFF_LITTLEENDIAN)
					TempFour= ENDIAN_SWAP32(TempFour);

#ifdef TIFPARSE_ERR_PRINT
				BF_Debug_Print_Error(("TempFour(Addr) is %d\r\n",TempFour));
#endif
				
				if (TempFour%2) 
				{
					poTiffProp->wImageBitDepth=TempFour;
					TempFour= ENDIAN_SWAP32(TempFour);
					return;
				}
				else
				{
					nErrorCode=BF_Fs_If_Lseek(fd,TempFour,SEEK_SET,&ulseek_result);
					if (nErrorCode !=0)
					{
#ifdef TIFPARSE_ERR_PRINT
						BF_Debug_Print_Error(("tif parse read error.\r\n"));
#endif					
						BF_Fs_If_Close(fd);
						return (TIFF_ERR_SEEK);					
					}						

					 while (PixelCount<PixelLen)					 
					 {
						nErrorCode=BF_Fs_If_Read(fd,TwoByte,2,&ulread_result);													
						if (ulread_result !=2)													
							return	 TIFF_ERR_READ;								
						memcpy(&PixelTemp, TwoByte,2);
						if (cByteOrder == TIFF_LITTLEENDIAN)
							PixelTemp = ENDIAN_SWAP16(PixelTemp);
						
#ifdef TIFPARSE_ERR_PRINT
				BF_Debug_Print_Error(("PixelTemp(Addr) is %d\r\n",PixelTemp));
#endif
							
						PixelValue+=PixelTemp;
#ifdef TIFPARSE_ERR_PRINT
				BF_Debug_Print_Error(("PixelValue(Addr) is %d\r\n",PixelValue));
#endif
				 		PixelCount+=1;
					 }
                 
					 poTiffProp->wImageBitDepth=PixelValue;					 
					 Flagg+=1;				
				
					if (Flagg==3) 
					{
						BF_Fs_If_Close(fd);
						return;
					}			
				}				
			}	 
			else
			{
				nErrorCode=BF_Fs_If_Lseek(fd,10,SEEK_CUR,&ulseek_result);
				if (nErrorCode !=0)
				{
#ifdef TIFPARSE_ERR_PRINT
					BF_Debug_Print_Error(("tif parse seek error.\r\n"));
#endif	
					BF_Fs_If_Close(fd);					
					return (TIFF_ERR_SEEK);					
				}
			}
			count+=1;
	   }
		
#ifdef TIFPARSE_ERR_PRINT
		BF_Debug_Print_Error(("tif parse read error.\r\n"));
#endif		  		
		BF_Fs_If_Close(fd);
	 }
	else
	{
#ifdef TIFPARSE_ERR_PRINT
		BF_Debug_Print_Error(("tif file type error.\r\n"));
#endif		         
		BF_Fs_If_Close(fd);
	}
   
    return 0;
}
#else
int  tiff_parse(char * file,TIFFPROP* poTiffProp)
{
	BF_Debug_Print_Error(("tiff_parse dummy function.\r\n"));
    return 0;
}
#endif

