/**
 * @file		jfiflib.c
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
#include "jfiflib.h"
#include "fs_if.h"
#include "ddim_typedef.h"
//#include "fj_common.h"
#include "ChipTest.H"

UINT32 nCount=0;
void  jfif_parse(char * file, JFIFPROP* poJFIFProp)
{
	int  fd;
	//unsigned char FourByte[4];
	//unsigned char TwoByte[2];
	unsigned char Temp[4];
	unsigned char Temp2[2];
	unsigned short block_length;
	FW_INT32  	nErrorCode;
	FW_ULONG  	ulRead_Result;
	FW_ULONG  	ulSeek_Result;

	memset(poJFIFProp,0,sizeof(JFIFPROP));
	fd = BF_Fs_If_Open(file, D_BF_FS_IF_O_RDONLY,0);

	//nErrorCode = BF_Fs_If_Read(fd,FourByte,4,&ulRead_Result);
	//memcpy(Temp, FourByte,4);
	nErrorCode = BF_Fs_If_Read(fd,&Temp,4,&ulRead_Result);
	//if ((FourByte[0]!=0xff)||(FourByte[1]!=0xd8)||(FourByte[2]!=0xff)||(FourByte[3]!=0xe0)) 
	//	printf("Error File!!! \n");

	//nErrorCode = BF_Fs_If_Read(fd,TwoByte,2,&ulRead_Result);
	//memcpy(&block_length, TwoByte,2);
	nErrorCode = BF_Fs_If_Read(fd,&block_length,2,&ulRead_Result);
	block_length = ENDIAN_SWAP16_PARSE(block_length);

	//nErrorCode = BF_Fs_If_Read(fd,FourByte,4,&ulRead_Result);
	//memcpy(&Temp, FourByte,4);
	nErrorCode = BF_Fs_If_Read(fd,&Temp,4,&ulRead_Result);

	//if ( strnicmp(Temp, "JFIF", 4) != 0 )
	//	printf("Eorrr File!!! \n");

	nErrorCode = BF_Fs_If_Lseek(fd,-4,SEEK_CUR,&ulSeek_Result);

	while (1) 
	{
	    nErrorCode = BF_Fs_If_Lseek(fd,block_length-2,SEEK_CUR,&ulSeek_Result);
		if (nErrorCode != 0)
			goto err;

		nErrorCode = BF_Fs_If_Read(fd,Temp2,2,&ulRead_Result);
		if (nErrorCode != 0)
			goto err;

		if((Temp2[0]==0xFF)&&(Temp2[1]==0xC0))
		{
			nErrorCode = BF_Fs_If_Lseek(fd,2,SEEK_CUR,&ulSeek_Result);
			if (nErrorCode != 0)
				goto err;				

			nErrorCode = BF_Fs_If_Read(fd,&(poJFIFProp->cImageBitDepth),sizeof(char),&ulRead_Result);
			if (nErrorCode != 0)
				goto err;	
			
			nErrorCode = BF_Fs_If_Read(fd,&(poJFIFProp->wHeight),sizeof(unsigned short),&ulRead_Result);
			if (nErrorCode != 0)
				goto err;

			nErrorCode = BF_Fs_If_Read(fd,&(poJFIFProp->wWidth),sizeof(unsigned short),&ulRead_Result);
			if (nErrorCode != 0)
				goto err;

			break;
		}
		else
		{
			nErrorCode = BF_Fs_If_Read(fd,&block_length,2,&ulRead_Result);
			block_length = ENDIAN_SWAP16_PARSE(block_length);
		}
	} 

err:
	if (fd)
		BF_Fs_If_Close(fd);

	poJFIFProp->wHeight = ENDIAN_SWAP16_PARSE(poJFIFProp->wHeight);
	poJFIFProp->wWidth = ENDIAN_SWAP16_PARSE(poJFIFProp->wWidth);

	return;
} 
