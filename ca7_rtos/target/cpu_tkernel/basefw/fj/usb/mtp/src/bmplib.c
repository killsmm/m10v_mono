/**
 * @file		bmplib.c
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
#include "bmplib.h"

#define BMP_PARSE_OK		0x00000000
#define BMP_ERR_READ        0x00000001    	
#define	BMP_ERR_NOT_RIFF    0x00000002   
#define BMP_ERR_OPEN		0x00000003
#define BMP_ERR_SEEK		0x00000006 

//#define BMPPARSE_ERR_PRINT
//#define MTP_BMP_SUPPORT 
#ifdef MTP_BMP_SUPPORT
int  bmp_parse(char* file,BMPPROP* poBmpProp)							   
{	
	char FourByte[4];
	char TwoByte[2];
	int  fd;   
	FW_INT32 nErrorCode=0;
	unsigned long ulseek_result;
	unsigned long ulread_result;

	memset(poBmpProp,0,sizeof(BMPPROP));

	fd = BF_Fs_If_Open(file, O_RDONLY,0);
	if (fd ==0)
	{
#ifdef BMPPARSE_ERR_PRINT
		BF_Debug_Print_Error("bmp parse read error.\r\n");
#endif	
		return	BMP_ERR_OPEN;
	}


	nErrorCode=BF_Fs_If_Lseek(fd,18,SEEK_CUR,&ulseek_result);
	if (nErrorCode !=0)
	{
#ifdef BMPPARSE_ERR_PRINT
		BF_Debug_Print_Error(("bmp parse read error.\r\n"));
#endif	
		BF_Fs_If_Close(fd);	
		return (BMP_ERR_SEEK);					
	}

	nErrorCode=BF_Fs_If_Read(fd,FourByte,4,&ulread_result);													
	if (ulread_result !=4)	
	{
#ifdef BMPPARSE_ERR_PRINT
		BF_Debug_Print_Error(("bmp parse read error.\r\n"));
#endif	
		BF_Fs_If_Close(fd);	
		return	 BMP_ERR_READ;	
	}
	memcpy(&(poBmpProp->dwWidth), FourByte,4);
	poBmpProp->dwWidth = ENDIAN_SWAP32(poBmpProp->dwWidth);

	nErrorCode=BF_Fs_If_Read(fd,FourByte,4,&ulread_result);													
	if (ulread_result !=4)	
	{
#ifdef BMPPARSE_ERR_PRINT
		BF_Debug_Print_Error(("bmp parse read error.\r\n"));
#endif	
		BF_Fs_If_Close(fd);	
		return	 BMP_ERR_READ;		
	}
	memcpy(&(poBmpProp->dwHeight), FourByte,4);
	poBmpProp->dwHeight = ENDIAN_SWAP32(poBmpProp->dwHeight);

	nErrorCode=BF_Fs_If_Lseek(fd,2,SEEK_CUR,&ulseek_result);
	if (nErrorCode !=0)
	{
#ifdef BMPPARSE_ERR_PRINT
		BF_Debug_Print_Error(("bmp parse seek error.\r\n"));
#endif	
		BF_Fs_If_Close(fd);	
		return (BMP_ERR_SEEK);					
	}

	nErrorCode=BF_Fs_If_Read(fd,TwoByte,2,&ulread_result);													
	if (ulread_result !=2)	
	{
#ifdef BMPPARSE_ERR_PRINT
		BF_Debug_Print_Error(("bmp parse read error.\r\n"));
#endif	
		BF_Fs_If_Close(fd);
		return	 BMP_ERR_READ;	
	}

	memcpy(&(poBmpProp->wImageBitDepth), TwoByte,2);

	BF_Fs_If_Close(fd);

	return BMP_PARSE_OK;

}
#else	//dummy function
int  bmp_parse(char* file,BMPPROP* poBmpProp)							   
{	
	BF_Debug_Print_Error(("bmp_parse dummy function.\r\n"));
	return BMP_PARSE_OK;
}
#endif
