/**
 * @file		giflib.c
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
#include "giflib.h"
#include "debug.h"

#define 	GIF_PARSE_OK		0x00000000
#define 	GIF_ERR_READ        0x00000001    	
#define		GIF_ERR_NOT_RIFF	0x00000002   
#define 	GIF_ERR_OPEN		0x00000003
#define 	GIF_ERR_SEEK		0x00000006 

//#define 	GIFPARSE_ERR_PRINT
//#define MTP_GIF_SUPPORT 
#ifdef MTP_GIF_SUPPORT
int  gif_parse(char* file,GIFPROP* poGIFProp)
{
	char TwoByte[2];
	char OneByte[1];
	int  fd;   
	char Temp;
	FW_INT32 nErrorCode=0;
	unsigned long ulseek_result;
	unsigned long ulread_result;	

	memset(poGIFProp,0,sizeof(GIFPROP));
	fd = BF_Fs_If_Open(file, O_RDONLY,0);
	if (fd ==0)
		return	GIF_ERR_OPEN;	

	nErrorCode=BF_Fs_If_Lseek(fd,6,SEEK_CUR,&ulseek_result);
	if (nErrorCode !=0)
	{
#ifdef GIFPARSE_ERR_PRINT
		BF_Debug_Print_Error(("gif parse seek error.\r\n"));
#endif	
		BF_Fs_If_Close(fd);	
		return (GIF_ERR_SEEK);					
	}

	nErrorCode=BF_Fs_If_Read(fd,TwoByte,2,&ulread_result);													
	if (ulread_result !=2)	
	{
#ifdef GIFPARSE_ERR_PRINT
		BF_Debug_Print_Error(("gif parse read error.\r\n"));
#endif	
		BF_Fs_If_Close(fd);		
		return	 GIF_ERR_READ;	
	}
	
	memcpy(&(poGIFProp->wWidth), TwoByte,2);

	nErrorCode=BF_Fs_If_Read(fd,TwoByte,2,&ulread_result);													
	if (ulread_result !=2)		
	{
#ifdef GIFPARSE_ERR_PRINT
		BF_Debug_Print_Error(("gif parse read error.\r\n"));
#endif	
		BF_Fs_If_Close(fd); 
		return	 GIF_ERR_READ;	
	}

	memcpy(&(poGIFProp->wHeight), TwoByte,2);

	nErrorCode=BF_Fs_If_Read(fd,OneByte,1,&ulread_result);													
	if (ulread_result !=1)			
	{
#ifdef GIFPARSE_ERR_PRINT
		BF_Debug_Print_Error(("gif parse read error.\r\n"));
#endif	
		BF_Fs_If_Close(fd);		
		return	 GIF_ERR_READ;	
	}
	
	memcpy(&(poGIFProp->cImageBitDepth), OneByte,1);
	Temp=poGIFProp->cImageBitDepth;

	if (Temp&0x80) 
	{
		Temp=Temp&0x07;


		switch(Temp) {

		case 0x00 :
			poGIFProp->cImageBitDepth=1;
			break;
		case 0x01 :
			poGIFProp->cImageBitDepth=2;
			break;
		case 0x02 :
			poGIFProp->cImageBitDepth=3;
			break;
		case 0x03 :
			poGIFProp->cImageBitDepth=4;
			break;
		case 0x04 :
			poGIFProp->cImageBitDepth=5;
			break;
		case 0x05 :
		    poGIFProp->cImageBitDepth=6;
			break;
		case 0x06 :
		    poGIFProp->cImageBitDepth=7;
			break;
		case 0x07 :
			poGIFProp->cImageBitDepth=8;
			break;
		default:
		 	//BF_Debug_Print_Error(("GIF Error!!! \n"));
#ifdef GIFPARSE_ERR_PRINT
			BF_Debug_Print_Error(("gif parse error.\r\n"));
#endif	
			break;			 	
		}
	}
//	else
//	{
		//BF_Debug_Print_Error(("I don't know \n"));
//#ifdef GIFPARSE_ERR_PRINT
//		BF_Debug_Print_Error(("not known type.\r\n"));
//#endif				
//	}
	BF_Fs_If_Close(fd);
	
	return 0;
}
#else
int  gif_parse(char* file,GIFPROP* poGIFProp)
{
	BF_Debug_Print_Error(("gif_parsej dummy function.\r\n"));
	return 0;
}
#endif
