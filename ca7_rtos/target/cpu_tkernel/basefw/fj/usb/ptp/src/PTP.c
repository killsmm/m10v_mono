/**
 * @file		PTP.c
 * @brief		PTP MODULE
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

//#pragma arm section rwdata="BASEFW_COMM_INIT"
//#pragma arm section rodata="BASEFW_COMM_CONST"
//#pragma arm section code="BASEFW_COMM_CODE"

#include "ddim_typedef.h"
#include "os_user_custom.h"
#ifdef CO_DCF_EXIF_ON
#include "dcf_if.h"
#endif
#include "mwusb_driver4.h"
#include "PTP.h"
#include "DPS.h"  
#include "fs_if.h"
#include "control_def.h" //tonyho
#include "MessageQueue.h" //tonyho
#include "ChipTest.H"
#include "fj_pictbridge.h"
#include "sdram_map_common.h"
#include "sdram_map_play.h"
#include "mtp.h"
#include <stddef.h>
#include <string.h>
#include "mdf_wrapper.h"

//===============================================================================

#define Center			(0)
#define UR				(1)
#define LR				(2)
#define LL				(3)
#define UL				(4)
#define DCF_EC_ERROR					0x0
CHAR    MiscPath[64];
ULONG	vTransactionID;
UINT32	vSessionID;
UINT32 PtpStorageID;

extern DEVICE_INFO DeviceInfo;
extern STORAGE_INFO StorageInfo, StorageInfo_SD, StorageInfo_MS;
extern OBJECT_INFO DPSDiscovery_ObjectInfo;
extern OBJECT_INFO ObjectInfo;
extern DeviceProp_ImageSize DevicePropImageSize;
extern DeviceProp_CompressionSetting DevicePropCompressionSetting;
extern DeviceProp_FocalLength DevicePropFocalLength;
extern DeviceProp_FlashMode DevicePropFlashMode;
extern DeviceProp_IsoMode DevicePropIsoMode;
extern INT32 curPageIndex;

INT32 IN_BUFFER_Write( UCHAR *buf_ptr, UINT32 buf_len );

static VOID PTP_InitializeStorageID(VOID);


/*------------ Virtual storage handles --------*/
RAM_FILE_MNG rfm={
	{
		{0,HANDLE_FILE_HREQUEST,"HREQUEST.DPS\0"},
		{0,HANDLE_FILE_HRSPONSE,"HRSPONSE.DPS\0"},
		{0,HANDLE_FILE_DREQUEST,"DREQUEST.DPS\0"},
		{0,HANDLE_FILE_DRSPONSE,"DREPONSE.DPS\0"},
		{0,HANDLE_FILE_HDISCVRY,"HDISCVRY.DPS\0"},
		{1,HANDLE_FILE_DDISCVRY,"DDISCVRY.DPS\0"},
	}
};
UINT32	RecvScriptHandle;//Set by SetObjectInfo , use for SetObject
//UINT32	SendScriptHandle;//Set by DPS,use for GetObjectInfo/GetObject

/*
	Access on virtual RAM space

	Printer			<=====>		DSC
	============				============
	HDISCVRY.DPS				DDISCVRY.DPS

	HREQUEST.DPS				DREQUEST.DPS
	HRSPONSE.DPS				DRSPONSE.DPS
*/


UCHAR			RecvFileName[120];
OBJECT_INFO		RecvObjInfo;

#ifdef USBPF_BIG_ENDIAN_ON // Endian
STORAGE_IDS	StorageIDs_MS = {0x01000000, 0x01000300};
STORAGE_IDS	StorageIDs_SD = {0x01000000, 0x01000200};
STORAGE_IDS StorageIDs_NF = {0x01000000, 0x01000100};
#else // Endian
STORAGE_IDS	StorageIDs_EM = {0x00000001, 0x00040001};
STORAGE_IDS	StorageIDs_MS = {0x00000001, 0x00030001};
STORAGE_IDS	StorageIDs_SD = {0x00000001, 0x00020001};
STORAGE_IDS StorageIDs_NF = {0x00000001, 0x00010001};
#endif // Endian

#ifdef CO_USB_MTP_ON
UCHAR	DPSDiscovery=0;//Flag for now 
#endif

UCHAR   vSessionOpen=0;


#if 0	// RAM sharing
UCHAR	Buffers1[2*1024],	//Send			
		Buffers2[1024],	//Receive
		Buffers3[12+3*4],   //Interrupt
		Buffers4[64],	//Send:Response
		PTP_JPEG_BUFFER[65536]; //Image Bulk In by Andy Jing
#endif	// RAM sharing

// Definition for RAM sharing
T_BF_PTP_BUFF* gBF_Ptp_Buff = (T_BF_PTP_BUFF *) (SDRAM_ADR_PLAY_PTP_BUFFER_AREA);

#if 0 //  RAM sharing
UCHAR   *IN_Buffers=(UCHAR *)PTP_JPEG_BUFFER,
		*OUT_Buffers=Buffers2,
		*INT_Buffers=Buffers3,
		*Resp_Buffers=Buffers4;
#endif

#if 0
// Send data buffer
UCHAR* IN_Buffers = (UCHAR*)(D_SDRAM_MAP_COMMON_DCACHE_OR | SDRAM_ADR_PLAY_PTP_BUFFER_AREA)+offsetof(T_BF_PTP_BUFF,PTP_JPEG_BUFFER);	/*	pgr0448	*/
// Receive data buffer
UCHAR* OUT_Buffers = (UCHAR*)(D_SDRAM_MAP_COMMON_DCACHE_OR | SDRAM_ADR_PLAY_PTP_BUFFER_AREA)+offsetof(T_BF_PTP_BUFF,Buffers2);		/*	pgr0448	*/
// Interrupt send data buffer
UCHAR* INT_Buffers = (UCHAR*)(D_SDRAM_MAP_COMMON_DCACHE_OR | SDRAM_ADR_PLAY_PTP_BUFFER_AREA)+offsetof(T_BF_PTP_BUFF,Buffers3);		/*	pgr0448	*/
// Send:Response data buffer
UCHAR* Resp_Buffers = (UCHAR*)(D_SDRAM_MAP_COMMON_DCACHE_OR | SDRAM_ADR_PLAY_PTP_BUFFER_AREA)+offsetof(T_BF_PTP_BUFF,Buffers4);		/*	pgr0448	*/
#else
// Send data buffer
UCHAR* IN_Buffers = (UCHAR*)(SDRAM_ADR_PLAY_PTP_BUFFER_AREA)+offsetof(T_BF_PTP_BUFF,PTP_JPEG_BUFFER);	/*	pgr0448	*/
// Receive data buffer
UCHAR* OUT_Buffers = (UCHAR*)(SDRAM_ADR_PLAY_PTP_BUFFER_AREA)+offsetof(T_BF_PTP_BUFF,Buffers2);		/*	pgr0448	*/
// Interrupt send data buffer
UCHAR* INT_Buffers = (UCHAR*)(SDRAM_ADR_PLAY_PTP_BUFFER_AREA)+offsetof(T_BF_PTP_BUFF,Buffers3);		/*	pgr0448	*/
// Send:Response data buffer
UCHAR* Resp_Buffers = (UCHAR*)(SDRAM_ADR_PLAY_PTP_BUFFER_AREA)+offsetof(T_BF_PTP_BUFF,Buffers4);		/*	pgr0448	*/
#endif


USHORT  vOperationCode;
UCHAR	vBO=0;
extern ULONG	FocalLength[17];
extern UCHAR	dirMISCname[];
extern UCHAR	dirDCIMname[];
UCHAR	TerTag=0;
UINT32    SendNumber;//Actual number of send bytes
UCHAR *dpsOutBufferPtr;
USHORT outFileSize;
extern CHAR *dscResBufPtr;
extern CHAR *dscReqBufPtr;
//--------------------------- for external process ----------------------
USHORT KeepPTPCmd=0;
ULONG	FWGetSize=0 ,GetFWByPTP=0;
INT32 PTP_fp;
extern DPSSTATUS dpsStatus;
DEVICE_EVENT	DeviceEvent={0x4000,{0,0,0,0},{0,0}};
INT32 total_prints = 0;
UCHAR PICT_DPOF_All_Image_flg = 0x00;//paul 20060727 dpof
//==================================================================================
UINT32 posSY=50;

//paul PTP begin
E_BF_MEDIA_STATE_MEDIA_TYPE PictBridgeUI_MediaSelected;
UINT32 StorageIndicator;//paul 20060821
ULONG ObjectCnt[7], JpgCnt[7];//paul MD

OBJECT_HANDLE_INFO *Object;//paul MD
#if 0	// RAM sharing
OBJECT_HANDLE_INFO Object_SD[NUM_OBJECT_SD], Object_DPOF;//paul MD
OBJECT_SEARCH_TBL  ObjectSearchTbl[900];/* 100 ~ 999 */
#endif	// RAM sharing

ULONG  ulTotalDPSDir = 0;
FW_LONG  ulCurrentDir = 0;
//paul PTP end
ULONG DeviceProperty_ImageSize = e3072x2304;//0x5004 (0 - 5) UINT8
ULONG DeviceProperty_CompressionSetting = eJpegGood;//0x5004 (0 - 2) Best, Better, Good (UINT8)
ULONG DeviceProperty_FocalLength = 1;//460;//0x5008 (1 - 15) UINT32
ULONG DeviceProperty_FlashMode = eStrobeOff;//0x500c (0:Auto, 1:Off, 2:Fill, 3:Auto(Red)) UINT16
ULONG DeviceProperty_IsoMode = eISOAuto;//0x501e UINT8
static E_BF_MEDIA_STATE_MEDIA_TYPE media_Type;
#ifdef CO_USB_MTP_ON
E_BF_MEDIA_STATE_MEDIA_TYPE MTP_media_Type;
#endif
extern MTP_DEVICE_INFO MTPDeviceInfo;

// Softronix tony +
extern UCHAR USBMtpCmd_GetDeviceInfo(VOID);
extern UCHAR USBMtpCmd_GetObjectPropsSupported(VOID);
extern UCHAR USBMtpCmd_GetObjectPropDesc(VOID);
extern UCHAR USBMtpCmd_GetObjectPropValue(VOID);
extern UCHAR USBMtpCmd_GetObjectPropList(VOID);
extern UCHAR USBMtpCmd_DeleteObject(VOID);
extern UCHAR USBMtpCmd_SendObjectInfo(VOID);
extern UCHAR USBMtpCmd_SendObject(VOID);
extern INT32   MTP_GetAllObjectInfo(E_BF_MEDIA_STATE_MEDIA_TYPE mediaType);
extern UCHAR USBMtpCmd_GetObjectInfo(VOID);
extern UCHAR USBMtpCmd_GetObject(VOID);
extern UCHAR USBMtpCmd_GetThumb(VOID);
extern UCHAR USBMtpCmd_FormatStore(VOID);
extern UCHAR USBMtpCmd_GetPartialObject(VOID);

LONG BULK_Read( UCHAR *buf, ULONG Size);
INT32 PTP_GetAllObjectInfo(E_BF_MEDIA_STATE_MEDIA_TYPE mediaType, ULONG *ObjectCount, ULONG *JpgCount);
INT32 PTP_GetDpofObjectInfo(E_BF_MEDIA_STATE_MEDIA_TYPE mediaType);
OS_USER_ER USBPtpCmd_Check_InvalidParameter(UINT32 parm);
OS_USER_ER USBPtpCmd_Check_SessionNotOpen(UINT32 SessionID);
OS_USER_ER USBPtpCmd_Check_InvalidStorageID(UINT32 sID);
OS_USER_ER USBPtpCmd_Check_NoValidObjectInfo( VOID );
OS_USER_ER USBPtpCmd_Check_DeviceBusy( VOID );
//////////////////////////////////////////////////////////////////
VOID	WriteDouble(UCHAR *Addr,UINT64 value)
{
	*Addr=(UCHAR)((value&0x00000000000000FF));
	*(Addr+1)=(UCHAR)((value&0x000000000000FF00)>>8);	
	*(Addr+2)=(UCHAR)((value&0x0000000000FF0000)>>16);	
	*(Addr+3)=(UCHAR)((value&0x00000000FF000000)>>24);	
	*(Addr+4)=(UCHAR)((value&0x000000FF00000000)>>32);
	*(Addr+5)=(UCHAR)((value&0x0000FF0000000000)>>40);
	*(Addr+6)=(UCHAR)((value&0x00FF000000000000)>>48);
	*(Addr+7)=(UCHAR)((value&0xFF00000000000000)>>56);
}

VOID SendData(UINT32 DataLen)
{
    GCS_T *Cmd;
    ULONG LENGTH;

    Cmd=(GCS_T *)IN_Buffers;
    LENGTH = 12+DataLen;
    LENGTH=ENDIAN_SWAP32(LENGTH);
    (*Cmd).ContainLen=ENDIAN_SWAP32(LENGTH);
	(*Cmd).ContainType=ENDIAN_SWAP16(2);
    (*Cmd).Code=ENDIAN_SWAP16(vOperationCode);
    (*Cmd).TransactionID=ENDIAN_SWAP32(vTransactionID);
	
	MWUSB_SetMode(0);
	MWUSB_Write((UCHAR*)IN_Buffers, 12+DataLen, BULK_WRITE_FIFO);
	if(((12+DataLen) % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
	{
		OS_User_Tslp_Tsk(1);
		MWUSB_SendNullPacket(BULK_WRITE_FIFO);
	}
}


VOID SendDataDetail(UINT32 LenField,UINT32 RealSendSize)
{
    GCS_T *Cmd;
    ULONG LENGTH;

    Cmd=(GCS_T *)IN_Buffers;
    LENGTH = 12+LenField;
	LENGTH=ENDIAN_SWAP32(LENGTH);
    (*Cmd).ContainLen=ENDIAN_SWAP32(LENGTH);

	(*Cmd).ContainType=ENDIAN_SWAP16(2);
    (*Cmd).Code=ENDIAN_SWAP16(vOperationCode);
    (*Cmd).TransactionID=ENDIAN_SWAP32(vTransactionID);

	MWUSB_SetMode(0);
	MWUSB_Write((UCHAR*)IN_Buffers, 12+RealSendSize, BULK_WRITE_FIFO);
	if(((12+RealSendSize) % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
	{
		OS_User_Tslp_Tsk(1);
		MWUSB_SendNullPacket(BULK_WRITE_FIFO);
	}
}

VOID SendDataDetail2(UINT32 LenField,UINT32 RealSendSize)
{
    GCS_T *Cmd;
    ULONG LENGTH;

    Cmd=(GCS_T *)IN_Buffers;
    LENGTH = 12+LenField;
    (*Cmd).ContainLen=ENDIAN_SWAP32(LENGTH);
    (*Cmd).ContainType=ENDIAN_SWAP16(2);
    (*Cmd).Code=ENDIAN_SWAP16(vOperationCode);
    (*Cmd).TransactionID=ENDIAN_SWAP32(vTransactionID);
	MWUSB_SetMode(0);
	MWUSB_Write((UCHAR*)IN_Buffers, 12+RealSendSize, BULK_WRITE_FIFO);
}

VOID SendResponseSimple(UINT32 Code)
{
	SendResponse(0,Code,0,0,0,0,0);	
}


VOID SendResponse(UINT32 ParmCnt,UINT32 Code,
                 UINT32 parm1,UINT32 parm2,UINT32 parm3,UINT32 parm4,UINT32 parm5)
{
    GCS_T *Cmd;
    ULONG LENGTH;
    ULONG transactonID = 0;
	Cmd=(GCS_T *)IN_Buffers;
    LENGTH = 12+ParmCnt*4;
    (*Cmd).ContainLen=ENDIAN_SWAP32(LENGTH);
    (*Cmd).ContainType=ENDIAN_SWAP16(3);
    (*Cmd).Code=ENDIAN_SWAP16(Code);
    (*Cmd).TransactionID=ENDIAN_SWAP32(vTransactionID);
    transactonID = ENDIAN_SWAP32(vTransactionID);
    DeviceEvent.TransactionID[0]=(UCHAR)((transactonID>>24)&0x000000ff);
    DeviceEvent.TransactionID[1]=(UCHAR)((transactonID>>16)&0x000000ff);
    DeviceEvent.TransactionID[2]=(UCHAR)((transactonID>>8)&0x000000ff);
    DeviceEvent.TransactionID[3]=(UCHAR)(transactonID&0x000000ff);
    (*Cmd).parm1=ENDIAN_SWAP32(parm1);
    (*Cmd).parm2=ENDIAN_SWAP32(parm2);
    (*Cmd).parm3=ENDIAN_SWAP32(parm3);
    (*Cmd).parm4=ENDIAN_SWAP32(parm4);
    (*Cmd).parm5=ENDIAN_SWAP32(parm5);

	MWUSB_SetMode(0);
	MWUSB_Write((UCHAR*)IN_Buffers, 12+ParmCnt*4, BULK_WRITE_FIFO);
}


VOID SendEvent(UINT32 Code,UINT32 objhdl)
{
	SendInterrupt(1,Code,objhdl,0,0);	
}
//----------- Checking PTP Command Valid and Response ------------


VOID SendInterrupt(UINT32 ParmCnt,UINT32 Code,
                    UINT32 parm1,UINT32 parm2,UINT32 parm3)
{
    GCS_T *Cmd;
    ULONG LENGTH;
    Cmd=(GCS_T *)INT_Buffers;

    LENGTH=12+ParmCnt*4;
    (*Cmd).ContainLen=ENDIAN_SWAP32(LENGTH);
    (*Cmd).ContainType=ENDIAN_SWAP16(4);
    (*Cmd).Code=ENDIAN_SWAP16(Code);
    (*Cmd).TransactionID=0xFFFFFFFF;
    
    (*Cmd).parm1=ENDIAN_SWAP32(parm1);
    (*Cmd).parm2=ENDIAN_SWAP32(parm2);

    LENGTH=12+ParmCnt*4;
#if 1 //2012/08/18
	MWUSB_InterruptWrite((UCHAR*)INT_Buffers, LENGTH);
#else
	
	MWUSB_InterruptWrite((UCHAR*)INT_Buffers, 16);
    INT_Buffers = INT_Buffers + 16;
	MWUSB_InterruptWrite((UCHAR*)INT_Buffers, 0);
#endif
    INT_Buffers=gBF_Ptp_Buff->Buffers3;
}


ULONG	PTP_fread(ULONG	buff,ULONG fsize)
{
	FW_ULONG vsize;
	BF_Fs_If_Read(PTP_fp, (VOID *)buff, fsize, &vsize);	    
	return vsize;
}

UINT32 CheckMediaFormatType( VOID )
{
	UINT32	ret = PTP_CHECK_MEDIA_NG;
	INT32	ercd = 0;				// error code
	UCHAR	used_media, format_type;

	ercd = FJ_Get_Media_Format_Type(&used_media, &format_type);
	if (ercd == FJ_ERR_OK)
	{
		switch (used_media)
		{
			case FJ_USED_MEDIA_SD:
				if ((format_type == FJ_FS_TYPE_FAT12) ||
					(format_type == FJ_FS_TYPE_FAT16) ||
					(format_type == FJ_FS_TYPE_FAT16E))
				{
					ret = PTP_CHECK_MEDIA_OK;
				}
				break;
			case FJ_USED_MEDIA_SDHC:
				if ((format_type == FJ_FS_TYPE_FAT32) ||
					(format_type == FJ_FS_TYPE_FAT32X))
				{
					ret = PTP_CHECK_MEDIA_OK;
				}
				break;
			case FJ_USED_MEDIA_SDXC:
				if (format_type == FJ_FS_TYPE_EXFAT)
				{
					ret = PTP_CHECK_MEDIA_OK;
				}
				break;
			default:
				break;
		}
		if (ret == PTP_CHECK_MEDIA_NG)
		{
			printf("CheckMediaFormatType() media error! used_media[%d], format_type[%d]\r\n", used_media, format_type);
		}
	}
	else
	{
		printf("CheckMediaFormatType() FJ_Get_Media_Format_Type() error = %d \r\n", ercd);
	}

	return ret;
}

VOID InitialPTP( VOID )
{
//	E_BF_MEDIA_STATE_MEDIA_TYPE eMediaType;

	/* set path --------------*/
	media_Type = BF_Fs_If_Get_Cur_Media();
#ifdef CO_USB_MTP_ON
	MTP_media_Type = media_Type;
#endif
	PTP_InitializeStorageID();
	switch(media_Type)
	{
		case E_BF_MEDIA_STATE_MEDIA_TYPE_MS:
			Object = gBF_Ptp_Buff->Object_SD;
			break;
		case E_BF_MEDIA_STATE_MEDIA_TYPE_SD1:
		default:
			Object = gBF_Ptp_Buff->Object_SD;
			break;
	}
	//DPSDiscovery=0;//not discovery now
	RecvScriptHandle=0;
	vSessionID=0;
	vSessionOpen=0;
	vTransactionID=0;
	
	rfm.RFCs[0].flg=0;
	rfm.RFCs[1].flg=0;
	rfm.RFCs[2].flg=0;
	rfm.RFCs[3].flg=0;
	rfm.RFCs[4].flg=0;
	
	TerTag=0;	
	KeepPTPCmd=0;
	

}

VOID PTP_InitializeStorageID(VOID) {
	
	if      (media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1)     PtpStorageID = PTP_StorageID_SD;
	else if (media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF)      PtpStorageID = PTP_StorageID_NF;
	else if (media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC)    PtpStorageID = PTP_StorageID_EM;    // pgr0839
	else                                                        PtpStorageID = PTP_StorageID_MS;
	
	return;
}

/*  20120822 Add form here  for GetPartialObect command speed-up */
typedef struct {
	UINT32	PrevObjHandle;			/* OjectHandle of the file processed immediately before  */
	UINT32	PrevObjCompressSize;	/* Size of the file processed immediately before         */
} PTP_CACHE_GETPARTIALOBJECT;

#ifdef CO_DCF_EXIF_ON
static PTP_CACHE_GETPARTIALOBJECT Ptp_Cache_GetPartialObject =
{
	0, 0
};
#endif

static VOID ptp_ClearGetPartialObectCache(VOID)
{
    if( PTP_fp != 0 )
    {   /* maybe file is open  */
        BF_Fs_If_Close(PTP_fp);
       PTP_fp = 0;
    }
#ifdef CO_DCF_EXIF_ON
	Ptp_Cache_GetPartialObject.PrevObjHandle = 0;
	Ptp_Cache_GetPartialObject.PrevObjCompressSize = 0;
#endif
}	
/*  20120822 Add to   here  */

VOID ResetDPS()
{
#ifdef CO_USB_MTP_ON
	DPSDiscovery=0;//not discovery now
#endif
	RecvScriptHandle=0;
	vSessionID=0;
	vSessionOpen=0;
	vTransactionID=0;
	
	rfm.RFCs[0].flg=0;
	rfm.RFCs[1].flg=0;
	rfm.RFCs[2].flg=0;
	rfm.RFCs[3].flg=0;
	rfm.RFCs[4].flg=0;
	
	TerTag=0;	
	KeepPTPCmd=0;
	InitDpsStatus();
	memset(&dpsStatus, 0x00, sizeof(dpsStatus));
    /*  20120822  Add form here for GetPartialObject command speed-up  */
    ptp_ClearGetPartialObectCache();
	/*  20120822 Add to here  */
}

INT32 ResetAllObjectInfo()
{
	E_BF_MEDIA_STATE_MEDIA_TYPE eMediaType = BF_Fs_If_Get_Cur_Media();
	memset(gBF_Ptp_Buff->Object_SD, 0, sizeof(gBF_Ptp_Buff->Object_SD));//Andy Jing
	memset(&gBF_Ptp_Buff->Object_DPOF, 0, sizeof(gBF_Ptp_Buff->Object_DPOF));//Andy Jing
	memset(gBF_Ptp_Buff->ObjectSearchTbl, 0, sizeof(gBF_Ptp_Buff->ObjectSearchTbl));//Andy Jing
	ulTotalDPSDir = 0;
	ulCurrentDir = 0;
	PTP_GetAllObjectInfo(eMediaType, &ObjectCnt[E_BF_MEDIA_STATE_MEDIA_TYPE_SD1], &JpgCnt[E_BF_MEDIA_STATE_MEDIA_TYPE_SD1]);
//	PTP_GetDpofObjectInfo(eMediaType);      delete because it's a processing for PictBridge.
	return 0;
}

//------- Checking Global Structure ---------
OS_USER_ER USBPtpCheckGCS( VOID )
{
    ULONG 	len;
    USHORT 	cmdtype,Code;
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
    UCHAR	parmCnt=0;
    ULONG	transactionID = 0;

    len=OUT_Buffers[3]<<24 | OUT_Buffers[2]<<16 | OUT_Buffers[1]<<8 | OUT_Buffers[0];
    if (len < 12) return 1;
    if (len > sizeof(GCS_B)) return 2;
    //Error on Type 
    cmdtype=(OUT_Buffers[5]<<8 | OUT_Buffers[4]);
    if (!(cmdtype == 1))  return 3;//Error on Containter type
    /////////debug ////////
    Code = (OUT_Buffers[7]<<8 | OUT_Buffers[6]);
    
    transactionID = (*CmdDS).TransactionID;  
    vTransactionID = ENDIAN_SWAP32(transactionID);
    vOperationCode=Code;  
    
    parmCnt=((UCHAR)len)-12;
    if(parmCnt) parmCnt=parmCnt / 4;
    
    
    switch(vOperationCode)
    {
		case PTP_OP_GetDeviceInfo	://1
			//if(USBPtpCmd_Check_ParameterNotSupported(parmCnt,0,&((*CmdDS).parm1)))	return 1;
			break;
		case PTP_OP_OpenSession  	://2 
			if(USBPtpCmd_Check_InvalidParameter((*CmdDS).parm1))	return 1;
//paul test			if(USBPtpCmd_Check_SessionAlreadyOpen())	return 1;
			break;
		case PTP_OP_CloseSession   	://3
			if(USBPtpCmd_Check_SessionNotOpen(0))	return 1;
			
			break;
		case PTP_OP_GetStorageIDs  	://4
			if(USBPtpCmd_Check_SessionNotOpen(0))	return 1;
			if(USBPtpCmd_Check_ParameterNotSupported(parmCnt,0,&((*CmdDS).parm1)))	return 1;
			
			break;
		case PTP_OP_GetStorageInfo 	://5
			if(USBPtpCmd_Check_SessionNotOpen(0))	return 1;
			if(USBPtpCmd_Check_InvalidStorageID((*CmdDS).parm1))	return 1;
			
			break;
		
		case PTP_OP_GetNumObjects  	://6
			if(USBPtpCmd_Check_SessionNotOpen(0))	return 1;
			if(USBPtpCmd_Check_ParameterNotSupported(parmCnt,3,&((*CmdDS).parm1)))	return 1;
			if(USBPtpCmd_Check_InvalidStorageID((*CmdDS).parm1))	return 1;
			
			if((parmCnt>=2) && ((*CmdDS).parm2 != 0))
			{
				if(USBPtpCmd_Check_SpecificationByFormatUnsupported((*CmdDS).parm2))	return 1;
			}
			if(USBPtpCmd_Check_InvalidParameter((*CmdDS).parm1))	return 1;
			break;
		case PTP_OP_Getobjecthandles://7
			if(USBPtpCmd_Check_SessionNotOpen(0))	return 1;			
			if(USBPtpCmd_Check_ParameterNotSupported(parmCnt,3,&((*CmdDS).parm1)))	return 1;			
			if(USBPtpCmd_Check_InvalidStorageID((*CmdDS).parm1))	return 1;
			if((parmCnt>=2) && ((*CmdDS).parm2 != 0))
			{
				if(USBPtpCmd_Check_InvalidObjectFormatCode((*CmdDS).parm2))	return 1;
				if(USBPtpCmd_Check_SpecificationByFormatUnsupported((*CmdDS).parm2))	return 1;
			}
			if(USBPtpCmd_Check_InvalidParameter((*CmdDS).parm1))	return 1;
			break;
		case PTP_OP_GetObjectInfo	://8  
			if(USBPtpCmd_Check_SessionNotOpen(0))	return 1;			
			if(USBPtpCmd_Check_ParameterNotSupported(parmCnt,1,&((*CmdDS).parm1)))	return 1;
			break;
		case PTP_OP_GetObject      	://9
			if(USBPtpCmd_Check_SessionNotOpen(0))	return 1;		
			if(USBPtpCmd_Check_ParameterNotSupported(parmCnt,1,&((*CmdDS).parm1)))	return 1;
			if(USBPtpCmd_Check_InvalidParameter((*CmdDS).parm1))	return 1;
			break;
		case PTP_OP_GetThumb       	://10
			if(USBPtpCmd_Check_SessionNotOpen(0))	return 1;			
			if(USBPtpCmd_Check_ParameterNotSupported(parmCnt,1,&((*CmdDS).parm1)))	return 1;
			break;
		case PTP_OP_SendObjectInfo	://11
			if(USBPtpCmd_Check_SessionNotOpen(0))	return 1;		
			if(USBPtpCmd_Check_ParameterNotSupported(parmCnt,2,&((*CmdDS).parm1)))	return 1;			
			if((parmCnt>=1) && ((*CmdDS).parm1 != 0))
			{
				//if(USBPtpCmd_Check_InvalidStorageID((*CmdDS).parm1))	return 1;
				//if(USBPtpCmd_Check_StoreReadOnly((*CmdDS).parm1))	return 1;
			}
			break;
		case PTP_OP_SendObject		://12
			if(USBPtpCmd_Check_SessionNotOpen(0))	return 1;			
//			if(USBPtpCmd_Check_ParameterNotSupported(parmCnt,0,&((*CmdDS).parm1)))	return 1;
//			if(USBPtpCmd_Check_NoValidObjectInfo())	return 1;
			
			break;

		case PTP_OP_FormatStore		://paul
			if(USBPtpCmd_Check_SessionNotOpen(0))	return 1;			
//			if(USBPtpCmd_Check_ParameterNotSupported(parmCnt,0,&((*CmdDS).parm1)))	return 1;
//			if(USBPtpCmd_Check_NoValidObjectInfo())	return 1;
			
			break;

		case PTP_OP_GetDevicePropDesc://paul
			if(USBPtpCmd_Check_SessionNotOpen(0))	return 1;			
//			if(USBPtpCmd_Check_ParameterNotSupported(parmCnt,0,&((*CmdDS).parm1)))	return 1;
//			if(USBPtpCmd_Check_NoValidObjectInfo())	return 1;
			
			break;

		case PTP_OP_SetDevicePropValue://paul
			if(USBPtpCmd_Check_SessionNotOpen(0))	return 1;			
//			if(USBPtpCmd_Check_ParameterNotSupported(parmCnt,0,&((*CmdDS).parm1)))	return 1;
//			if(USBPtpCmd_Check_NoValidObjectInfo())	return 1;
			
			break;

		case PTP_OP_GetPartialObject://13
			if(USBPtpCmd_Check_SessionNotOpen(0))	return 1;		
			if(USBPtpCmd_Check_ParameterNotSupported(parmCnt,3,&((*CmdDS).parm1)))	return 1;
			if(USBPtpCmd_Check_InvalidParameter((*CmdDS).parm1))	return 1;
			
			break;
		case PTP_OP_UpdateFirmwareAll:	
					
			break;
		case PTP_OP_SendObjectSplit:
			break;
		case MTP_OP_GetObjectPropsSupported:
		 	break;
		case MTP_OP_GetObjectPropDesc:
		 	break;	
		case MTP_OP_GetObjectPropValue: 		
		 	break;
		case MTP_OP_GetObjectPropList:		
		 	break;				
		case MTP_OP_DeleteObject:			
		 	break;		
		case MTP_OP_GetDevicePropValue: 		
			break;
#ifdef PTP_VENDOR_CMD_EN
			case PTP_CT_FJ_GpioGroupSelect:	//		(0x9001)
			case PTP_CT_FJ_GpioGroupConfig:	//		(0x9002)
			case PTP_CT_FJ_GpioGroupSet:		//		(0x9003)
			case PTP_CT_FJ_GpioGroupGet:		//		(0x9004)
			case PTP_CT_FJ_ConfigButtonDrvEx: 	//		(0x9005)
			case PTP_CT_FJ_AdcRead:			//		(0x9006)
#if 0	//<----- unnecessity : start 
			case PTP_CT_FJ_SetMShutterController:	//	(0x900A)
			case PTP_CT_FJ_GetMShutterController:	//	(0x900B)
			case PTP_CT_FJ_MShutterOpen:			//	(0x900C)
			case PTP_CT_FJ_MShutterClose:			//	(0x900D)	
#endif	//<----- unnecessity : end 
			case PTP_CT_FJ_MountStorageMedia:		//	(0x9010)		
			case PTP_CT_FJ_FormatStorageMedia:	//	(0x9011)	
			case PTP_CT_FJ_SetMode:				//(0x9020)
			case PTP_CT_FJ_SetImageSize:	//(0x9040)
			case PTP_CT_FJ_SetCompressionRatio:	//	(0x9041)
			case PTP_CT_FJ_DirectFsFileOpen: 			//0x9053		p
			case PTP_CT_FJ_DirectFsFileClose:			//0x9054		p
			case PTP_CT_FJ_DirectFsFileRead:			//0x9055		p
			case PTP_CT_FJ_DirectFsFileWrite:		
			case PTP_CT_FJ_SetCurrentImage:		//	(0x906F)
			
			case PTP_CT_FJ_GetDCFInfo:			//	(0x9070)
		       case PTP_CT_FJ_SetFileType:			//	(0x9071)	
		       case PTP_CT_FJ_FileOperation:			//	(0x907B)
			case PTP_CT_FJ_AudioRecCfg:		//		(0x90B4)
			case PTP_CT_FJ_AudioParamSet:	//		(0x90B6)
			case PTP_CT_FJ_RecordPlayAudio:	//		(0x90B8)
			case PTP_CT_FJ_StopRecording:		//		(0x90BA)
			case PTP_CT_FJ_SetDisplayMode:	//		(0x90CC)			
			case PTP_CT_FJ_SetLong:			//		(0x90DB)
			case PTP_CT_FJ_GetLong:			//		(0x90DC)	
			case PTP_CT_FJ_GetFixedImage:			//	(0x90DD)
			case PTP_CT_FJ_GetLastImage:			//	(0x90DE)
			case PTP_CT_FJ_GetLastFileName:		//	(0x90DF)
			case PTP_CT_FJ_SetFileName:			//	(0x90E0)
			case PTP_CT_FJ_SendFixedImage:		//	(0x90E1)
			case PTP_CT_FJ_GetParamLong:		//		(0x90A9)	
			case PTP_CT_FJ_EnableRTC:		//		(0x9025)
			case PTP_CT_FJ_GetClock:			//		(0x9026)
			case PTP_CT_FJ_SetClock:			//		(0x9027)	

			case PTP_CT_FJ_DebugDataWrite:	//		(0x90F0)
			case PTP_CT_FJ_DebugDataRead:	//		(0x90F1)
			case PTP_CT_FJ_DebugBulkOut:	//		(0x90F2)
			case PTP_CT_FJ_AEGetComputeInfo:	//		(0x9107)
			case PTP_CT_FJ_SetCaptureExposureTime:	//(0x9111)
			case PTP_CT_FJ_SetPreviewAgcGain:	//		(0x9114)
#if 0	//<----- unnecessity : start 
			case PTP_CT_FJ_Set2ToppolyTPG85:	//			0x9200
			case PTP_CT_FJ_Set2ADDI9000:	//				0x9201
			case PTP_CT_FJ_Lens_Open:	//			0x9202
			case PTP_CT_FJ_Lens_Close:	//				0x9203
			case PTP_CT_FJ_Lens_ZoomMove2Wide:	//		0x9204
			case PTP_CT_FJ_Lens_ZoomMove2Tele:	//		0x9205
			case PTP_CT_FJ_Lens_ZoomMoveByStep:	//		0x9206
			case PTP_CT_FJ_Lens_FocusMove2CCW:	//		0x9207
			case PTP_CT_FJ_Lens_FocusMove2CW:	//		0x9208
			case PTP_CT_FJ_Lens_MShutterOpen:	//		0x9209
			case PTP_CT_FJ_Lens_MShutterClose:	//		0x920A
			case PTP_CT_FJ_Lens_NDOpen:	//				0x920B
			case PTP_CT_FJ_Lens_NDClose:	//				0x920C			
#endif	//<----- unnecessity : end 
			break;
#endif //PTP_VENDOR_CMD_EN
#ifdef CALIBRATION_COMMANMD
		case Calibration_StartSensorSubSystem:
		case Calibration_EndSensorSubSystem:
		case Calibration_GetFirmwareVersion:
		case Calibration_GetSerialNum:  
		case Calibration_SaveSerialNum:
		case Calibration_TakePhoto:
		case Calibration_GetJpeg:
		case Calibration_GetRaw:
		case Calibration_CheckBadPixel:
		case Calibration_SendBadPixel:
		case Calibration_CheckZoomOffset:
		case Calibration_CheckFocusOffset:
		case Calibration_CheckFocusRange:
		case Calibration_CheckShutterDelay:
		case Calibration_CheckIris:
		case Calibration_CheckStrobe:
		case Calibration_CheckZ3GOffset:
		case Calibration_GetZoomOffset:
		case Calibration_GetFocusOffset:
		case Calibration_GetFocusRange:
		case Calibration_GetShutterDelay:
		case Calibration_GetZ3GOffset:
			if(USBPtpCmd_Check_SessionNotOpen(0))	return 1;
			break;
#endif
		default://Error Command
			if((vOperationCode>=0x9500)&&(vOperationCode<=0x95FF))
			{
				USBPtpCmdUnknown();
				return 1;
			} 
			else 
			{
				USBPtpCmdUnknown();
			return 1;
    }
			//break;
    }
    return(0);//__Succeed
}

OS_USER_ER execPTPCmd( VOID )
{
    UCHAR   ret=1;
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
    
    USHORT Code;

    INT32	cnt;
        for(cnt=0;cnt<20;cnt++)	
			*(OUT_Buffers+cnt)=0;

        {
            static INT32 count,count55=0;
            count++;
            count55++;
            //MWUSB_SetMode(1);
            MWUSB_SetMode(0);	//usb normal mode
            if(MWUSB_Read1Packet((UCHAR*)OUT_Buffers) <=0)
            //if(MWUSB_Read((UCHAR*)OUT_Buffers, 0, BULK_READ_FIFO) <= 0)
            {
            	return ret;
            }           
        }
        
        
        if(!USBPtpCheckGCS())
        {
            Code = (*CmdDS).Code;  //debug
            Code = ENDIAN_SWAP16(Code);
            (*CmdDS).Code = Code;
            
            switch ((*CmdDS).Code)  {
                case PTP_OP_GetDeviceInfo:      ret=USBPtpCmd_GetDeviceInfo();		break;
                case PTP_OP_OpenSession:        ret=USBPtpCmd_OpenSession();        break;
                case PTP_OP_CloseSession:       ret=USBPtpCmd_CloseSession();       break;
                case PTP_OP_GetStorageIDs:      ret=USBPtpCmd_GetStorageIDs();      break;
                case PTP_OP_GetStorageInfo:     ret=USBPtpCmd_GetStorageInfo();     break;
                case PTP_OP_GetNumObjects:      ret=USBPtpCmd_GetNumObjects();      break;
                case PTP_OP_Getobjecthandles:   ret=USBPtpCmd_GetObjecthandles();   break;
                case PTP_OP_GetObjectInfo:      ret=USBPtpCmd_GetObjectInfo();      break;
                case PTP_OP_GetObject:          ret=USBPtpCmd_GetObject();          break;
                case PTP_OP_GetThumb:           ret=USBPtpCmd_GetThumb();           break;
                case PTP_OP_SendObjectInfo:	    ret=USBPtpCmd_SendObjectInfo();     break;
                case PTP_OP_SendObject:         ret=USBPtpCmd_SendObject();         break;
                case PTP_OP_FormatStore:         ret=USBPtpCmd_FormatStore();         break;
                case PTP_OP_GetDevicePropDesc:         ret=USBPtpCmd_GetDevicePropDesc();         break;
                case PTP_OP_SetDevicePropValue:         ret=USBPtpCmd_SetDevicePropValue();         break;
                case PTP_OP_GetPartialObject:   ret=USBPtpCmd_GetPartialObject();   break;
                case PTP_OP_SendObjectSplit:	ret=USBPtpCmd_SendObjectSplit();	break; 
		 // Softronix tony +
		 case MTP_OP_GetObjectPropsSupported:	ret=USBMtpCmd_GetObjectPropsSupported();	break;
		 case MTP_OP_GetObjectPropDesc:			ret=USBMtpCmd_GetObjectPropDesc();	break;	
		 case MTP_OP_GetObjectPropValue: 		ret=USBMtpCmd_GetObjectPropValue(); 		break;
		 case MTP_OP_GetObjectPropList:			ret=USBMtpCmd_GetObjectPropList();	break;				
		 case MTP_OP_DeleteObject:				ret=USBMtpCmd_DeleteObject();	break;		
		 case MTP_OP_GetDevicePropValue: 		ret=USBMtpCmd_GetDevicePropValue(); 	break;
		 // Softronix tony -				
#ifdef PTP_VENDOR_CMD_EN
			case PTP_CT_FJ_GpioGroupSelect:		ret=CHIP_TEST_GpioGroupSelect();		break;	//		(0x9001)
			case PTP_CT_FJ_GpioGroupConfig:		ret=CHIP_TEST_GpioGroupConfig(); 		break;	//		(0x9002)
			case PTP_CT_FJ_GpioGroupSet:			ret=CHIP_TEST_GpioGroupSet();			break;	//		(0x9003)
			case PTP_CT_FJ_GpioGroupGet:			ret=CHIP_TEST_GpioGroupGet();			break;	//		(0x9004)
			case PTP_CT_FJ_ConfigButtonDrvEx:		ret=CHIP_TEST_ConfigButtonDrvEx();		break;	//		(0x9005)
			case PTP_CT_FJ_AdcRead:				ret=CHIP_TEST_AdcRead();				break;	//	(0x9006)	
#if 0	//<----- unnecessity : start 
			case PTP_CT_FJ_SetMShutterController:	ret=CHIP_TEST_SetMShutterController(); 	break;	//	(0x900A)
			case PTP_CT_FJ_GetMShutterController:	ret=CHIP_TEST_GetMShutterController(); 	break;	//	(0x900B)
			case PTP_CT_FJ_MShutterOpen:			ret=CHIP_TEST_MShutterOpen(); 			break;	//	(0x900C)
			case PTP_CT_FJ_MShutterClose:			ret=CHIP_TEST_MShutterClose(); 			break;	//	(0x900D)	
#endif	//<----- unnecessity : end 
			case PTP_CT_FJ_MountStorageMedia:		ret=CHIP_TEST_MountStorageMedia();		break;	//	(0x9010)
			case PTP_CT_FJ_FormatStorageMedia:	ret=CHIP_TEST_FormatStorageMedia(); 	break;	//	(0x9011)	
			case PTP_CT_FJ_SetMode:				ret=CHIP_TEST_SetMode();				break;//(0x9020)
			case PTP_CT_FJ_SetImageSize:			ret=CHIP_TEST_SetImageSize();			break;
			case PTP_CT_FJ_SetCompressionRatio:	ret=CHIP_TEST_SetCompressionRatio();	break;	//	(0x9041)
			case PTP_CT_FJ_DirectFsFileOpen: 		ret=CHIP_TEST_DirectFsFileOpen();		break;	//0x9053		p
			case PTP_CT_FJ_DirectFsFileClose:		ret=CHIP_TEST_DirectFsFileClose();		break;	//0x9054		p
			case PTP_CT_FJ_DirectFsFileRead:		ret=CHIP_TEST_DirectFsFileRead();		break;	//0x9055		p
			case PTP_CT_FJ_DirectFsFileWrite:		ret=CHIP_TEST_DirectFsFileWrite();		break;	//0x9056		p
			case PTP_CT_FJ_SetCurrentImage:		ret=CHIP_TEST_SetCurrentImage();			break;	//	(0x906F)
			case PTP_CT_FJ_GetDCFInfo:			ret=CHIP_TEST_GetDCFInfo();				break;	//(0x9070)
			case PTP_CT_FJ_SetFileType:			ret=CHIP_TEST_SetFileType();			break;	//	(0x9071)
			case PTP_CT_FJ_FileOperation:			ret=CHIP_TEST_FileOperation();			break;	//	(0x907B)
			case PTP_CT_FJ_AudioRecCfg:			ret=CHIP_TEST_AudioRecCfg();			break;	//		(0x90B4)
			case PTP_CT_FJ_AudioParamSet:		ret=CHIP_TEST_AudioParamSet();			break;	//		(0x90B6)
			case PTP_CT_FJ_RecordPlayAudio:		ret=CHIP_TEST_RecordPlayAudio();		break;	//		(0x90B8)
			case PTP_CT_FJ_StopRecording:			ret=CHIP_TEST_StopRecording();			break;	//		(0x90BA)
			case PTP_CT_FJ_SetDisplayMode:		ret=CHIP_TEST_SetDisplayMode();			break;	//		(0x90CC)
			case PTP_CT_FJ_SetLong:				ret=CHIP_TEST_SetLong();				break;//		(0x90DB)
			case PTP_CT_FJ_GetLong:				ret=CHIP_TEST_GetLong();				break;//		(0x90DC)
			case PTP_CT_FJ_GetFixedImage:			ret=CHIP_TEST_GetFixedImage();		break;//	(0x90DD)
			case PTP_CT_FJ_GetLastImage:			ret=CHIP_TEST_GetLastImage();		break;//	(0x90DE)
			case PTP_CT_FJ_GetLastFileName:		ret=CHIP_TEST_GetLastFileName();	break;//	(0x90DF)
			case PTP_CT_FJ_SetFileName:			ret=CHIP_TEST_SetFileName();		break;//	(0x90E0)			
			case PTP_CT_FJ_SendFixedImage: 		ret=CHIP_TEST_SendFixedImage();		break;//(0x90E1)		
			case PTP_CT_FJ_EnableRTC:			ret=CHIP_TEST_EnableRTC();				break;	//	(0x9025)
			case PTP_CT_FJ_GetClock:				ret=CHIP_TEST_GetClock();				break;	//	(0x9026)
			case PTP_CT_FJ_SetClock:				ret=CHIP_TEST_SetClock();				break;	//	(0x9027)
			case PTP_CT_FJ_GetParamLong:			ret=CHIP_TEST_GetParamLong();			break;	//	(0x90A9)(0x90A9)
			case PTP_CT_FJ_DebugDataWrite:		ret=CHIP_TEST_DebugDataWrite();		break;	//	(0x90F0)
			case PTP_CT_FJ_DebugDataRead:		ret=CHIP_TEST_DebugDataRead();			break;	//	(0x90F1)
			case PTP_CT_FJ_DebugBulkOut:		ret=CHIP_TEST_DebugBulkOut();	break;	//(0x90F2)

			case PTP_CT_FJ_AESetWindow:			ret=CHIP_TEST_AESetWindow();			break;	//	(0x9102)
			case PTP_CT_FJ_AEGetComputeInfo:		ret=CHIP_TEST_AEGetComputeInfo();			break;//		(0x9107)
			case PTP_CT_FJ_SetCaptureExposureTime: ret=CHIP_TEST_SetCaptureExposureTime(); break; 	//(0x9111)
			case PTP_CT_FJ_SetPreviewAgcGain:		ret=CHIP_TEST_SetPreviewAgcGain();			break;//		(0x9114)	

#if 0	//<----- unnecessity : start 
			case PTP_CT_FJ_Set2ToppolyTPG85:		ret=CHIP_TEST_Set2ToppolyTPG85();	break;
			case PTP_CT_FJ_Set2ADDI9000:			ret=CHIP_TEST_Set2ADDI9000();			break;//				0x9201
			case PTP_CT_FJ_Lens_Open:			ret=CHIP_TEST_Lens_Open();			break;//			0x9202
			case PTP_CT_FJ_Lens_Close:			ret=CHIP_TEST_Lens_Close();			break;//				0x9203
			case PTP_CT_FJ_Lens_ZoomMove2Wide:		ret=CHIP_TEST_Lens_ZoomMove2Wide();			break;//		0x9204
			case PTP_CT_FJ_Lens_ZoomMove2Tele:		ret=CHIP_TEST_Lens_ZoomMove2Tele();			break;//		0x9205
			case PTP_CT_FJ_Lens_ZoomMoveByStep:		ret=CHIP_TEST_Lens_ZoomMoveByStep();			break;//		0x9206
			case PTP_CT_FJ_Lens_FocusMove2CCW:		ret=CHIP_TEST_Lens_FocusMove2CCW();			break;//		0x9207
			case PTP_CT_FJ_Lens_FocusMove2CW:		ret=CHIP_TEST_Lens_FocusMove2CW();			break;//		0x9208
			case PTP_CT_FJ_Lens_MShutterOpen:			ret=CHIP_TEST_Lens_MShutterOpen();			break;//		0x9209
			case PTP_CT_FJ_Lens_MShutterClose:		ret=CHIP_TEST_Lens_MShutterClose();			break;//		0x920A
			case PTP_CT_FJ_Lens_NDOpen:				ret=CHIP_TEST_Lens_NDOpen();			break;//				0x920B
			case PTP_CT_FJ_Lens_NDClose:				ret=CHIP_TEST_Lens_NDClose();			break;//				0x920C	
#endif	//<----- unnecessity : end 

#endif //PTP_VENDOR_CMD_EN
#ifdef CALIBRATION_COMMANMD
				case Calibration_StartSensorSubSystem: ret=Calibration_PTPStartSensorSubSystem();  break;
				case Calibration_EndSensorSubSystem: ret=Calibration_PTPEndSensorSubSystem();  break;
				case Calibration_GetFirmwareVersion:  ret=Calibration_PTPGetFirmwareVersion();  break;
				case Calibration_GetSerialNum:  ret=Calibration_PTPGetSerialNum();  break;
				case Calibration_SaveSerialNum:  ret=Calibration_PTPSaveSerialNum();  break;
				case Calibration_TakePhoto:  ret=Calibration_PTPTakePhoto();  break;
				case Calibration_GetJpeg:  ret=Calibration_PTPGetJpeg();  break;
				case Calibration_GetRaw:  ret=Calibration_PTP_GetRaw();  break;
				case Calibration_CheckBadPixel:  ret=Calibration_PTPCheckBadPixel();  break;
				case Calibration_SendBadPixel:  ret=Calibration_PTPSendBadPixel();  break;
				case Calibration_CheckZoomOffset:  ret=Calibration_PTPCheckZoomOffset();  break;
				case Calibration_CheckFocusOffset:  ret=Calibration_PTPCheckFocusOffset();  break;
				case Calibration_CheckFocusRange:  ret=Calibration_PTPCheckFocusRange();  break;
				case Calibration_CheckShutterDelay: ret=Calibration_PTPCheckShutterDelay();  break;
				case Calibration_CheckIris:  ret=Calibration_PTPCheckIris();  break;
				case Calibration_CheckStrobe:  ret=Calibration_PTPCheckStrobe();  break;
				case Calibration_CheckZ3GOffset:  ret=Calibration_PTPCheckZ3GOffset();  break;
				case Calibration_GetZoomOffset:  ret=Calibration_PTPGetZoomOffset();  break;
				case Calibration_GetFocusOffset:  ret=Calibration_PTPGetFocusOffset();  break;
				case Calibration_GetFocusRange:  ret=Calibration_PTPGetFocusRange();  break;
				case Calibration_GetShutterDelay:  ret=Calibration_PTPGetShutterDelay();  break;
				case Calibration_GetZ3GOffset:  ret=Calibration_PTPGetZ3GOffset();  break;
#endif 		
                default://Error Command
                    #ifdef PTP_VENDOR_CMD_EN
                	if(((*CmdDS).Code>=0x9500)&&((*CmdDS).Code<=0x95FF)){
                		ret=Vender_Chip_Test();
                	} else
                	#endif //PTP_VENDOR_CMD_EN
                	{	
                    USBPtpCmdUnknown();
                    }
                    break;
            }           
        }
        else //Not Correct Command
        {
			printf("execPTPCmd Not Correct Command!! %d\n", (*CmdDS).Code);
        }	

//	}
	return ret;
}


OS_USER_ER USBPtpCmd_Check_InvalidParameter(UINT32 parm)
{
	parm = ENDIAN_SWAP32(parm);
	if(parm==0)
	{
		SendResponseSimple(PTP_RS_InvalidParameter);
		return 1;			
	}	
	return 0;
}

OS_USER_ER USBPtpCmd_Check_SessionAlreadyOpen( VOID )
{
	if(vSessionOpen)
	{
		SendResponseSimple(PTP_RS_SessionAlreadyOpen);
		return 1;	
	}
	return 0;
}


UCHAR USBPtpCmd_GetDeviceInfo(VOID)//1============================================
{
#ifdef CO_USB_MTP_ON
    if(DPSDiscovery)	// Softronix tony +
    {
#endif
	memcpy(IN_Buffers+12,&DeviceInfo,sizeof(DeviceInfo));
	SendData(sizeof(DeviceInfo)-sizeof(DeviceInfo.fill));

    SendResponseSimple(PTP_RS_OK);
#ifdef CO_USB_MTP_ON
    }
    else	// Softronix tony +	
    {
		USBMtpCmd_GetDeviceInfo();	
    }		// Softronix tony -
#endif
    return 0;
}


UCHAR USBPtpCmd_OpenSession(VOID)//2================================================
{
    UINT32 Code;
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;

    Code=(*CmdDS).parm1;
	vSessionID=ENDIAN_SWAP32(Code);
    vSessionID=Code;
    vSessionOpen=1;

    SendResponseSimple(PTP_RS_OK);
    return 0;
}


UCHAR USBPtpCmd_CloseSession(VOID)//3==============================================
{
	MSG outmsg = {ePictBridgeSubState_Ptp_SessionClosed,0,(VOID*)0,{0}};
    InitialPTP(); /* Reset PTP */
    InitDpsStatus(); /* Reset DPS */
    SendResponseSimple(PTP_RS_OK);
    FJ_PictBridgeSendHostMessage(outmsg);
    return 0;
}

UCHAR USBPtpCmd_GetStorageIDs(VOID)//4=========================================================
{
	USHORT	usMediaStatus = 0x0000;		// media status is invalid.
	INT32	iMediaRet;

	if (media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1)		// SD
	{
		iMediaRet = CheckMediaFormatType();
		if (iMediaRet == PTP_CHECK_MEDIA_OK)
		{
			usMediaStatus = 0x0001;
		}
		else
		{
			usMediaStatus = 0x0000;
		}
		usMediaStatus = ENDIAN_SWAP16(usMediaStatus);
		memcpy(&StorageIDs_SD.Data, &usMediaStatus, sizeof(usMediaStatus));
		memcpy(IN_Buffers+12,&StorageIDs_SD,sizeof(StorageIDs_SD));
	}else if (media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){		// NF
		memcpy(IN_Buffers+12,&StorageIDs_NF,sizeof(StorageIDs_NF));
	}else if (media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC){		// pgr0839    eMMC
		memcpy(IN_Buffers+12,&StorageIDs_EM,sizeof(StorageIDs_EM));
	}else{		// MS
		memcpy(IN_Buffers+12,&StorageIDs_MS,sizeof(StorageIDs_MS));
	}
	SendData(sizeof(STORAGE_IDS));

    SendResponseSimple(PTP_RS_OK);   
	BF_Debug_Print_Information(("USBPtpCmd_GetStorageIDs\n"));
    return 0;
}


UCHAR USBPtpCmd_GetStorageInfo(VOID)//5========================================
{
#ifdef CO_USB_MTP_ON
    if(DPSDiscovery)	// Softronix tony +
    {
#endif
	GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
	UINT32	storageId;
	//ULONG	allSpace,freeSpace;
	UINT64 allSpace,freeSpace;
	ULONG blksize, blocks, bfree;
	//E_BF_MEDIA_STATE_MEDIA_TYPE  eMediaType;
    CHAR     media[12];
	T_BF_FS_IF_STATVFS	media_info;
	USHORT bytePerSector=0;
	USHORT sectorPerCluster=0;
	UINT32 totalClusters=0;
	UINT32 freeClusters=0;
	storageId = (*CmdDS).parm1;  
	storageId = ENDIAN_SWAP32(storageId);

	if (storageId == PTP_StorageID_SD){	//SD
		BF_Fs_If_Statvfs("I", &media_info);
		bytePerSector = 512;
		sectorPerCluster = (USHORT)(media_info.blksize / 512);
		totalClusters = (UINT32)media_info.blocks;
		freeClusters = (UINT32)media_info.bfree;
		allSpace = (UINT64)totalClusters*(UINT64)sectorPerCluster*(UINT64)bytePerSector;
		freeSpace = (UINT64)freeClusters*(UINT64)sectorPerCluster*(UINT64)bytePerSector;	
		WriteDouble(StorageInfo_SD.MaxCapacity,allSpace);
		WriteDouble(StorageInfo_SD.FreeSpaceInBytes/*+4*/,freeSpace);			
		memcpy(IN_Buffers+12,&StorageInfo_SD,sizeof(StorageInfo_SD)-1);
		SendData(sizeof(StorageInfo_SD)-1);		
	}else if (storageId == PTP_StorageID_NF || storageId == PTP_StorageID_EM){	//NF, eMMC
		BF_Fs_If_Get_Cur_Drv(media);
		BF_Mdf_Wrapper_Get_Free_Space((const CHAR*)media, &blksize, &blocks, &bfree);
		allSpace = (UINT64)blocks*(UINT64)blksize;
		freeSpace = (UINT64)bfree*(UINT64)blksize;
		WriteDouble(StorageInfo.MaxCapacity/*+4*/,allSpace);
		WriteDouble(StorageInfo.FreeSpaceInBytes/*+4*/,freeSpace);	
		memcpy(IN_Buffers+12,&StorageInfo,sizeof(StorageInfo)-1);
		SendData(sizeof(StorageInfo)-1);		
	}else if (storageId == PTP_StorageID_MS){	//MS
		BF_Fs_If_Statvfs("I", &media_info);
		bytePerSector = 512;
		sectorPerCluster = (USHORT)(media_info.blksize / 512);
		totalClusters = (UINT32)media_info.blocks;
		freeClusters = (UINT32)media_info.bfree;
		allSpace = (UINT64)totalClusters*(UINT64)sectorPerCluster*(UINT64)bytePerSector;
		freeSpace =  (UINT64)freeClusters*(UINT64)sectorPerCluster*(UINT64)bytePerSector;
		WriteDouble(StorageInfo_MS.MaxCapacity/*+4*/,allSpace);
		WriteDouble(StorageInfo_MS.FreeSpaceInBytes/*+4*/,freeSpace);	
		memcpy(IN_Buffers+12,&StorageInfo_MS,sizeof(StorageInfo_MS)-1);
		SendData(sizeof(StorageInfo_MS)-1);
	}
    /*--------- Response -----------*/
    SendResponseSimple(PTP_RS_OK);
	BF_Debug_Print_Information(("USBPtpCmd_GetStorageInfo\n"));
#ifdef CO_USB_MTP_ON
    }
    else
    {
    		USBMtpCmd_GetStorageInfo();
    }	
#endif
    return 0;
}

UCHAR USBPtpCmd_GetNumObjects(VOID)//6===============================================
{
    UINT32	StorageID, ObjectFormatCode;
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
    UINT32 NumObj=0;
        
#ifdef CO_USB_MTP_ON
    if(DPSDiscovery)	// Softronix tony +
    {
#endif
    StorageID = (*CmdDS).parm1; 
    StorageID = ENDIAN_SWAP32(StorageID);

    ObjectFormatCode = (*CmdDS).parm2;  
    ObjectFormatCode = ENDIAN_SWAP32(ObjectFormatCode);


    switch(ObjectFormatCode)
    {    	
    	case PTP_OFC_Script://for DPS discovery	
			//pictBridgeStorage = sysStatReport.imageStorage;//paul MD, uses Draft(Playback) Mode storage selection as PictBridge UI storage selection
			//if ((pictBridgeStorage == eImageStorage_Auto) && IsCardOK()){
				//MediaStatus = MediaStatusSD;
				PictBridgeUI_MediaSelected = E_BF_MEDIA_STATE_MEDIA_TYPE_SD1;
			//}else{
			//	MediaStatus = MediaStatusNF;
			//	PictBridgeUI_MediaSelected = eMEDIA_TYPE_SM;
			//}
			total_prints = JpgCnt[PictBridgeUI_MediaSelected];
			PICT_DPOF_All_Image_flg = 0;//paul 20060727 dpof
			
			//outmsg.msgid = ePictBridgeSubState_Init_PictBridge_UI;
			//SendMessage(outmsg,eUSBMQ,kMsgWaitInfinity);
   			NumObj = ScriptFile_Handles(0);

    			break;
    	case PTP_OFC_None://Not Use(All Object)      	
    	case PTP_OFC_AllPicture://AllPicture      	
    	default:   
			//PictBridgeUI_Enable = 0;
//			if (StorageID == PTP_StorageID_NF){
//				NumObj = PTP_GetObjectHandle(E_BF_MEDIA_STATE_MEDIA_TYPE_NF, ObjectFormatCode, 0);
//			}
//			else if (StorageID == 0xffffffff){
//				NumObj = PTP_GetObjectHandle(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1, ObjectFormatCode, 0);
//			}			
			NumObj = PTP_GetObjectHandle(ObjectFormatCode, 0);
			break;

    }

    SendResponse(1,PTP_RS_OK, NumObj ,0,0,0,0);
#ifdef CO_USB_MTP_ON
    	}
    else	// Softronix tony +	
    {
		USBMtpCmd_GetNumObjects();	
    }		// Softronix tony -
#endif
    return 0;
}

UCHAR USBPtpCmd_GetObjecthandles(VOID)//7=================================
{
    UINT32	StorageID, ObjectFormatCode;
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
    UINT32	hdlcnt=0;
    UINT32	*ptr=(UINT32 *)(IN_Buffers+12+4);
#ifdef CO_USB_MTP_ON
    if(DPSDiscovery)	// Softronix tony +
    {
#endif
    StorageID = (*CmdDS).parm1; 
    StorageID = ENDIAN_SWAP32(StorageID);

    ObjectFormatCode = (*CmdDS).parm2; 
    ObjectFormatCode = ENDIAN_SWAP32(ObjectFormatCode);

	switch(ObjectFormatCode)
	{
		case PTP_OFC_Script:
			hdlcnt = ScriptFile_Handles(ptr);
			break;	
		case PTP_OFC_None://Not Use(All Object)
		case PTP_OFC_AllPicture://All Picture
		default:
			//PictBridgeUI_Enable = 0;
//			if (StorageID == PTP_StorageID_NF){
//				hdlcnt = PTP_GetObjectHandle(E_BF_MEDIA_STATE_MEDIA_TYPE_NF, ObjectFormatCode, ptr);
//			}else if (StorageID == PTP_StorageID_SD){
//				hdlcnt = PTP_GetObjectHandle(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1, ObjectFormatCode, ptr);
//			}else if (StorageID == PTP_StorageID_MS){
//				hdlcnt = PTP_GetObjectHandle(E_BF_MEDIA_STATE_MEDIA_TYPE_MS, ObjectFormatCode, ptr);
//			}else if (StorageID == 0xffffffff){
//				hdlcnt = PTP_GetObjectHandle(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1, ObjectFormatCode, ptr);
//			}			
			hdlcnt = PTP_GetObjectHandle(ObjectFormatCode, ptr);
			break;
	}
	
    *((UINT32 *)(IN_Buffers+12))=ENDIAN_SWAP32(hdlcnt);
    SendData(4+4*hdlcnt);
    //...... Send .....

    SendResponseSimple(PTP_RS_OK);
#ifdef CO_USB_MTP_ON
    }
    else	// Softronix tony +	
    {
		USBMtpCmd_GetObjecthandles();	
    }		// Softronix tony -	
#endif
    return 0;
}


UCHAR USBPtpCmd_GetObjectInfo(VOID)//8=======================================
{
    UINT32    ObjectHandle,Code;
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
#ifdef CO_USB_MTP_ON
    if(DPSDiscovery)	// Softronix tony +
    {
#endif
    Code = (*CmdDS).parm1;  //debug 
    ObjectHandle = ENDIAN_SWAP32(Code);
    
    if(USBPtpProcessObjectOpen(PTP_OP_GetObjectInfo,ObjectHandle,0,0))	return 1;
    SendResponseSimple(PTP_RS_OK); 
#ifdef CO_USB_MTP_ON
    }
    else	// Softronix tony +	
    {
		USBMtpCmd_GetObjectInfo();	
    }		// Softronix tony -	
#endif
    return 0;
}


UCHAR USBPtpCmd_GetObject(VOID)//9============================================
{
    UINT32    ObjectHandle;
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
#ifdef CO_USB_MTP_ON
    if (DPSDiscovery) // Softronix tony +
    {
#endif
    ObjectHandle=(*CmdDS).parm1;
    ObjectHandle = ENDIAN_SWAP32(ObjectHandle);
    
    if(USBPtpProcessObjectOpen(PTP_OP_GetObject,ObjectHandle,0,0))	return 1;
    SendResponseSimple(PTP_RS_OK);
#ifdef CO_USB_MTP_ON
    }
    else	// Softronix tony +	
    {
		USBMtpCmd_GetObject();	
    }		// Softronix tony -	
#endif
    return 0;
}


UCHAR USBPtpCmd_GetThumb(VOID)//10===========================================
{
    UINT32    ObjectHandle;
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
#ifdef CO_USB_MTP_ON
    if (DPSDiscovery) // Softronix tony +
    {
#endif
    ObjectHandle=(*CmdDS).parm1;
    ObjectHandle = ENDIAN_SWAP32(ObjectHandle);
    
    if(USBPtpProcessObjectOpen(PTP_OP_GetThumb,ObjectHandle,0,0))	return 1;
    SendResponseSimple(PTP_RS_OK);
#ifdef CO_USB_MTP_ON
    }
    else	// Softronix tony +	
    {
		USBMtpCmd_GetThumb();	
    }		// Softronix tony -	
#endif
    return 0;
}


UCHAR USBPtpCmd_SendObjectInfo(VOID)//11======================================
{

    UINT32    ObjHdl;     //Object Handle (reverse for incoming object)
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
    UINT32    StorageID;  
    UINT32    ParObjHdl; 
#ifdef CO_USB_MTP_ON
	if (DPSDiscovery) // Softronix tony +
	{
#endif
	StorageID=(*CmdDS).parm1;
	ParObjHdl=(*CmdDS).parm2;
	StorageID=ENDIAN_SWAP32(StorageID);
	ParObjHdl=ENDIAN_SWAP32(ParObjHdl);
	if(USBPtpProcessRecvObj(PTP_OP_SendObjectInfo,StorageID,ParObjHdl))	return 1;

	ObjHdl=RecvScriptHandle;
	
    //.......... Receive Data ........
    
    SendResponse(3,PTP_RS_OK,StorageID,ParObjHdl,ObjHdl,0,0);
    
#ifdef CO_USB_MTP_ON
	if(DPSDiscovery==1)
	{
		DPSDiscovery=2;
		DSP_Discovery();
	}
#endif
#ifdef CO_USB_MTP_ON
	}
	else
	{
		USBMtpCmd_SendObjectInfo();
		//SendResponse(3,PTP_RS_OK,StorageID,ParObjHdl,ObjHdl,0,0);
	}	
#endif
	return 0;
}



UCHAR USBPtpCmd_SendObject(VOID)//12=========================================
{
#ifdef CO_USB_MTP_ON
	if ( DPSDiscovery )
	{
#endif
		if(USBPtpProcessRecvObj(PTP_OP_SendObject,0,0))	return 1;
		SendResponseSimple(PTP_RS_OK);
#ifdef CO_USB_MTP_ON
	}
	else
	{
		(VOID)USBMtpCmd_SendObject();
//		SendResponseSimple(PTP_RS_OK);
	}
#endif
	return 0;
}

UCHAR USBPtpCmd_FormatStore(VOID)//2================================================
{
    UINT32 StorageID;
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
#ifdef CO_USB_MTP_ON
	if (DPSDiscovery) // Softronix tony +
	{
#endif
	StorageID=(*CmdDS).parm1;
	StorageID=ENDIAN_SWAP32(StorageID);
//#ifdef _M2A_
//	if (StorageID == PTP_StorageID_NF){//Internal NandFlash
//		NF_Format();
//		PTP_GetAllObjectInfo(eMEDIA_TYPE_SM, &ObjectCnt[eMEDIA_TYPE_SM], &JpgCnt[eMEDIA_TYPE_SM]);
//  	}else if (StorageID == PTP_StorageID_SD){//SD
//		SD_Format();
//		PTP_GetAllObjectInfo(eMEDIA_TYPE_SDI, &ObjectCnt[eMEDIA_TYPE_SDI], &JpgCnt[eMEDIA_TYPE_SDI]);
//  	}else{
//  	};
//	DCF_Init(eDCF_ALL);
//#endif
#ifdef _M4A_SMALLBOARD_
#ifdef CO_DCF_EXIF_ON
	if (StorageID == PTP_StorageID_NF){//Internal NandFlash
		BF_Dcf_If_Format_As(NULL, E_BF_MEDIA_STATE_MEDIA_TYPE_NF);
		PTP_GetAllObjectInfo(E_BF_MEDIA_STATE_MEDIA_TYPE_NF, &ObjectCnt[E_BF_MEDIA_STATE_MEDIA_TYPE_NF], &JpgCnt[E_BF_MEDIA_STATE_MEDIA_TYPE_NF]);
	}else if (StorageID == PTP_StorageID_EM){//eMMC
		BF_Dcf_If_Format_As(NULL, E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC);
		PTP_GetAllObjectInfo(E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC, &ObjectCnt[E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC], &JpgCnt[E_BF_MEDIA_STATE_MEDIA_TYPE_EMMC]);
  	}else if (StorageID == PTP_StorageID_SD){//SD
		BF_Dcf_If_Format_As(NULL, E_BF_MEDIA_STATE_MEDIA_TYPE_SD1);
		PTP_GetAllObjectInfo(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1, &ObjectCnt[E_BF_MEDIA_STATE_MEDIA_TYPE_SD1], &JpgCnt[E_BF_MEDIA_STATE_MEDIA_TYPE_SD1]);
  	}else{
  	};
#endif
#endif
	
	//DCF_Status.gLastSavedFileNo = 0;//paul 20060728
	SendResponseSimple(PTP_RS_OK);
#ifdef CO_USB_MTP_ON
	}
	else
	{
		USBMtpCmd_FormatStore();
	}
#endif
	return 0;
}

UCHAR USBPtpCmd_GetDevicePropDesc(VOID)//paul========================================
{
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
    USHORT DevicePropCode;
	//UCHAR *DevicePropertyValue = OUT_Buffers+12; /* Marked by Andy Jing 070306 */
    ULONG Param1;
#ifdef CO_USB_MTP_ON
    if(DPSDiscovery)	// Softronix tony +
    {
#endif
	Param1 = (*CmdDS).parm1;
	Param1 = ENDIAN_SWAP32(Param1);
	DevicePropCode = (USHORT)Param1;
       
	switch(DevicePropCode)
	{
		case PTP_DPC_ImageSize:
                    memcpy(IN_Buffers+12,&DevicePropImageSize,sizeof(DevicePropImageSize));
                    SendData(sizeof(DevicePropImageSize));
			break;
		case PTP_DPC_CompressionSetting://0: Best, 1: Better, 2: Good
                    memcpy(IN_Buffers+12,&DevicePropCompressionSetting,sizeof(DevicePropCompressionSetting));
                    SendData(sizeof(DevicePropCompressionSetting));
			break;
		case PTP_DPC_FocalLength:
                    memcpy(IN_Buffers+12,&DevicePropFocalLength,sizeof(DevicePropFocalLength));
                    SendData(sizeof(DevicePropFocalLength));
			break;
		case PTP_DPC_FlashMode:
                    memcpy(IN_Buffers+12,&DevicePropFlashMode,sizeof(DevicePropFlashMode));
                    SendData(sizeof(DevicePropFlashMode));
			break;
		case PTP_DPC_IsoMode:
                    memcpy(IN_Buffers+12,&DevicePropIsoMode,sizeof(DevicePropIsoMode));
                    SendData(sizeof(DevicePropIsoMode));
			break;
	}

    /*--------- Response -----------*/
    SendResponseSimple(PTP_RS_OK);
#ifdef CO_USB_MTP_ON
    }	
    else	// Softronix tony +
    {
		USBMtpCmd_GetDevicePropDesc();
    }		// Softronix tony -	
#endif
    return 0;
}

UCHAR USBPtpCmd_SetDevicePropValue(VOID)//12=========================================
{	
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
    USHORT DevicePropCode;
	UCHAR i, *Data = OUT_Buffers+12;
	UCHAR ucDevicePropertyValue;
	USHORT usDevicePropertyValue;
 	ULONG ulDevicePropertyValue;
    ULONG Param1;
	
#ifdef CO_USB_MTP_ON
    if(DPSDiscovery)	// Softronix tony +
    {
#endif
	Param1 = (*CmdDS).parm1;
	Param1 = ENDIAN_SWAP32(Param1);
	DevicePropCode = (USHORT)Param1;
	//MWUSB_SetMode(1);
    //MWUSB_Read((UCHAR*)OUT_Buffers, 0, BULK_READ_FIFO);
	BULK_Read((UCHAR*)OUT_Buffers, 0);
       
	switch(DevicePropCode)
	{
		case PTP_DPC_CompressionSetting://Resolution+Compression
			ucDevicePropertyValue = *((UCHAR *)Data);
			DeviceProperty_ImageSize = ucDevicePropertyValue%6;
//paul 20060510 for resolution definition change
			if (DeviceProperty_ImageSize == 0)
				DeviceProperty_ImageSize = e3072x2304;
			else if (DeviceProperty_ImageSize == 1)
				DeviceProperty_ImageSize = e3072x2048;
			else if (DeviceProperty_ImageSize == 2)
				DeviceProperty_ImageSize = e2576x1932;
			else if (DeviceProperty_ImageSize == 3)
				DeviceProperty_ImageSize = e2048x1536;
			else if (DeviceProperty_ImageSize == 4)
				DeviceProperty_ImageSize = e1600x1200;
			else if (DeviceProperty_ImageSize == 5)
				DeviceProperty_ImageSize = eVGA;
			else
				DeviceProperty_ImageSize = e3072x2304;
//paul 20060510 for resolution definition change
			DeviceProperty_CompressionSetting = ucDevicePropertyValue/6;
//			DeviceProperty_CompressionSetting = eJpegGood;//paul avoid bug temperarily
			break;
		case PTP_DPC_FocalLength:
			ulDevicePropertyValue = *((ULONG *)Data);
			ulDevicePropertyValue = ENDIAN_SWAP32(ulDevicePropertyValue);

			for (i=0; i<17; i++)
			{//if not found, don't change zoom position
				if (FocalLength[i] == ulDevicePropertyValue)
				{
					DeviceProperty_FocalLength=i+1;
					break;
				}
			}
			break;
		case PTP_DPC_FlashMode:
			usDevicePropertyValue = *((USHORT *)Data);
			usDevicePropertyValue = ENDIAN_SWAP16(usDevicePropertyValue);
			DeviceProperty_FlashMode=usDevicePropertyValue - 1;
			break;
		case PTP_DPC_IsoMode:
			DeviceProperty_IsoMode = *((UCHAR *)Data);
			break;
	}


    SendResponseSimple(PTP_RS_OK);
#ifdef CO_USB_MTP_ON
     }
	
    else	// Softronix tony +
    {
		USBMtpCmd_SetDevicePropValue();
    }	
#endif
    return 0;
}

UCHAR USBPtpCmd_GetPartialObject(VOID)//13=====================================
{
    UINT32    ObjectHandle,OffsetInBytes,MaxRcvNum;
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
#ifdef CO_USB_MTP_ON
    if (DPSDiscovery) // Softronix tony +
    {
#endif
    ObjectHandle=(*CmdDS).parm1;
    OffsetInBytes=(*CmdDS).parm2;
    MaxRcvNum=(*CmdDS).parm3;

    ObjectHandle=ENDIAN_SWAP32(ObjectHandle);
    OffsetInBytes=ENDIAN_SWAP32(OffsetInBytes);
    MaxRcvNum=ENDIAN_SWAP32(MaxRcvNum);

    if(USBPtpProcessObjectOpen(PTP_OP_GetPartialObject,ObjectHandle,OffsetInBytes,MaxRcvNum))return 1;
    SendResponse(1,PTP_RS_OK, SendNumber ,0,0,0,0);
#ifdef CO_USB_MTP_ON
    }
    else	// Softronix tony +
    {
		USBMtpCmd_GetPartialObject();
    }		// Softronix tony -	
#endif
    return 0;
}


OS_USER_ER USBPtpCmd_Check_SessionNotOpen(UINT32 SessionID)
{
	if(vSessionID==0){
		SendResponseSimple(PTP_RS_SessionNotOpen);
		return 1;			
	}
	if((SessionID != 0) && (SessionID != vSessionID)){
		SendResponseSimple(PTP_RS_SessionNotOpen);
		return 1;	
	}
	return 0;
}


OS_USER_ER USBPtpCmd_Check_ParameterNotSupported(UCHAR parmCnt,UCHAR MaxCnt,UINT32 *ptr)
{
	if(parmCnt>MaxCnt)
		for(;MaxCnt<parmCnt;MaxCnt++)
			if((*(ptr+MaxCnt))!=0)
			{
				SendResponseSimple(PTP_RS_ParameterNotSupported);
				return 1;
			}
	return 0;
}


OS_USER_ER USBPtpCmd_Check_InvalidStorageID(UINT32 sID)
{
	sID = ENDIAN_SWAP32(sID);

	if(sID != 0){
		if((sID != PTP_StorageID_NF) && (sID != PTP_StorageID_SD) && (sID != PTP_StorageID_MS) && (sID != PTP_StorageID_EM) && (sID != 0xFFFFFFFF))
		{
			SendResponseSimple(PTP_RS_InvalidStorageID);
			return 1;
		}
	}
	return 0;
}


OS_USER_ER USBPtpCmd_Check_InvalidObjectFormatCode(ULONG objfmt)
{	//Same as USBPtpCmd_Check_SpecificationByFormatUnsupported now !!!!
	objfmt = ENDIAN_SWAP32(objfmt);
	
	switch(objfmt)
	{
		case	PTP_OFC_None:
		case	PTP_OFC_AllPicture:
		case	PTP_OFC_Directory:
		case	PTP_OFC_Script:
		case	PTP_OFC_EXIFJPEG:
		case	PTP_OFC_DPOF:
		case	PTP_OFC_MOV:
		case	PTP_OFC_WAV:
			return 0;
		default:
			SendResponseSimple(PTP_RS_InvalidObjectFormatCode);		
			return 1;
	}
}


OS_USER_ER USBPtpCmd_Check_SpecificationByFormatUnsupported(ULONG objfmt)
{	//Same as USBPtpCmd_Check_InvalidObjectFormatCode now !!!!
	objfmt = ENDIAN_SWAP32(objfmt);
	switch(objfmt)
	{
		case	PTP_OFC_None:
		case	PTP_OFC_AllPicture:
		case	PTP_OFC_Directory:
		case	PTP_OFC_Script:
		case	PTP_OFC_EXIFJPEG:
		case	PTP_OFC_DPOF:
		case	PTP_OFC_MOV:
		case	PTP_OFC_WAV:
			return 0;
		default:
			SendResponseSimple(PTP_RS_InvalidObjectFormatCode);		
			return 1;
	}
}


UINT32 ScriptFile_Handles(UINT32 *Addr)
{
	UCHAR	cnt;
	UINT32	v=0;
	for(cnt=0;cnt<VIRTUAL_HANDLE_MAX;cnt++)	
		if(rfm.RFCs[cnt].flg){
			v++;
			if(Addr != 0){
				*Addr = rfm.RFCs[cnt].hdl;
				*Addr = ENDIAN_SWAP32(*Addr);
				Addr++;		// pgr0689
			}
		}
	return v;
}


USHORT	ScriptFile_Use(UINT32 hdl)
{
	UCHAR	cnt;
	for(cnt=0;cnt<VIRTUAL_HANDLE_MAX;cnt++)
	{
		if(hdl==rfm.RFCs[cnt].hdl){
			if(rfm.RFCs[cnt].flg){
				return	PTP_EV_ObjectInfoChanged;
			}else{
				rfm.RFCs[cnt].flg=1;
				return	PTP_EV_ObjectAdded;
			}	
		}
	}	
	return 0;
}


USHORT	ScriptFile_Rem(UINT32 hdl)
{
	UCHAR cnt;
	for(cnt=0;cnt<VIRTUAL_HANDLE_MAX;cnt++)
	{
		if(hdl==rfm.RFCs[cnt].hdl){
			if(rfm.RFCs[cnt].flg){
				rfm.RFCs[cnt].flg=0;				
				return	PTP_EV_ObjectRemoved;
			}
		}
	}	
	return 0;
}



OS_USER_ER USBPtpCmd_Check_NoValidObjectInfo( VOID )
{
	if(!RecvScriptHandle)
	{
		SendResponseSimple(PTP_RS_NoValidObjectInfo);
		return 1;	
	}
	return 0;
}


VOID	WriteInt(UCHAR *Addr,UINT32 value)
{
	*Addr=(UCHAR)(value&0x000000FF);
	*(Addr+1)=(UCHAR)((value&0x0000FF00)>>8);	
	*(Addr+2)=(UCHAR)((value&0x00FF0000)>>16);	
	*(Addr+3)=(UCHAR)((value&0xFF000000)>>24);	
}


VOID 	WriteShort(UCHAR *Addr,USHORT value)
{
	*Addr=(UCHAR)(value&0x00FF);
	*(Addr+1)=(UCHAR)((value&0xFF00)>>8);		
}

USHORT readTIFF_R(UCHAR *Addr)
{
	return (*Addr)<<8 | (*(Addr+1));	
}

USHORT	readIIMMShort(UCHAR *ptr)
{
	switch(vBO){
		case 1:return ((*(ptr+1)<<8)|(*ptr));
		case 2:return (*ptr<<8) | (*(ptr+1));
	}	  
	return 0;
}

UINT32	readIIMMInt(UCHAR *ptr)
{
	switch(vBO){
		case 1:return (*(ptr+3))<<24 | (*(ptr+2))<<16 | (*(ptr+1))<<8 | (*ptr);
		case 2:return (*ptr)<<24 | (*(ptr+1))<<16 | (*(ptr+2))<<8 | (*(ptr+3));
	}           
	return 0;
}

UCHAR readTagValue(UCHAR *Addr,USHORT vTag,UINT32 *vValue)
{
	USHORT cnt,vCount;
	
	vCount=readIIMMShort(Addr);
	Addr=Addr+2;
	for(cnt=0;cnt<vCount;cnt++,Addr=Addr+12)
		if(readIIMMShort(Addr)==vTag){
			*vValue=readIIMMInt(Addr+8);
			return(0);
		}
	return(1);
}


UINT32 CalcSendAndRest(UINT32 wantSend,UINT32 *RestSend)
{
	UINT32 v;
	
	if(wantSend > *RestSend){
		v=*RestSend;
		*RestSend=0;
		
	}else{
		v=wantSend;
		*RestSend=(*RestSend)-wantSend;
	}	
            
	return v;
}



UCHAR USBPtpCmd_SendObjectSplit( VOID )
{

	INT32		err = 0;
		
	GetFWByPTP=0;
	//..............Get F/W from Buffers here..............
	SendResponse(0,PTP_RS_OK,0,0,0,0,0);
	return err;
	
}
//--------------------------------------------------------
VOID USBPtpCmdUnknown ( VOID )
{
    
    /*---- send Response ----*/
    SendResponseSimple(PTP_RS_OperationNotSupported);
}


OS_USER_ER USBPtpProcessRecvObj(USHORT ProcKind,UINT32 StrgID,UINT32 ParentID)
{
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
    UINT32	allSize;
    UCHAR	len,cnt,*Addr;
#ifdef DeletePictBridgeCode
    ULONG Readsize;
	ULONG   recvSize;	
    extern CHAR *dpsInBufferPtr;
    
	Readsize=
#endif
		BULK_Read((UCHAR*)OUT_Buffers, 0);
	allSize=(*CmdDS).ContainLen;
    allSize=ENDIAN_SWAP32(allSize);

	
	switch(ProcKind)
	{
		case PTP_OP_SendObjectInfo:
			memcpy(&RecvObjInfo,OUT_Buffers+12,allSize-12);
			len=RecvObjInfo.FCMK[0];
			Addr=RecvObjInfo.FCMK+1;
			
			for(cnt=0;cnt<len;cnt++)
			{
				RecvFileName[cnt]=*Addr;
				Addr=Addr+2;	
			}	
			RecvScriptHandle=0;
			
			if(strcmp("HREQUEST.DPS",(const CHAR*)RecvFileName)==0)
			{
				RecvScriptHandle=HANDLE_FILE_HREQUEST;
			}else 
			if(strcmp("HRSPONSE.DPS",(const CHAR*)RecvFileName)==0)
			{
				RecvScriptHandle=HANDLE_FILE_HRSPONSE;
			}else
			if(strcmp("HDISCVRY.DPS",(const CHAR*)RecvFileName)==0)
			{
				RecvScriptHandle=HANDLE_FILE_HDISCVRY;
#ifdef DeletePictBridgeCode
				if(DPSDiscovery != 2)
				{
					DPSDiscovery=1;//Set Tag
				}
#endif
			}else
			{
				
			}
			if(RecvScriptHandle)	
				ScriptFile_Use(RecvScriptHandle);
			break;
		case PTP_OP_SendObject:
				if(USBPtpCmd_Check_DeviceBusy())	return 1;
				
#ifdef DeletePictBridgeCode
                recvSize = Readsize -12;

				if(DPSDiscovery)
				{
					if(GetDpsInputBuf())
					{
						memcpy(dpsInBufferPtr,OUT_Buffers+12,recvSize);
						
						//........ Trans Data to Buffer for DPS .......
						dpsStatus.inFileSize=recvSize;
						dpsStatus.inOperationType = RecvScriptHandle;
						if((dpsStatus.inOperationType == HANDLE_FILE_HRSPONSE) && (dpsStatus.waitState2 == WAIT_DSC_RESPONSE))
						{
					   		OS_User_Set_Flg(FID_DPSTSK, FLG_DPSPRT2); /* To avoid that Host doesn't wait for DSC response */
					   		OS_User_Wup_Tsk(TID_USB_DPSTSK);
						}
						OS_User_Set_Flg(FID_DPSTSK, FLG_DPSEVENT);

					}
					//.........................................
					//	Notify DPS Start Process Script File
					//.........................................
					
					ScriptFile_Rem(RecvScriptHandle);
				}
#endif
				RecvScriptHandle=0;//finish use this handle*/
				
			break;
	}   
	return 0;
}



OS_USER_ER USBPtpCmd_Check_DeviceBusy( VOID )
{                

	UCHAR	i=100;       
	while(dpsStatus.lockInputBuf){
		if(!(i--))break;        
	}
	if(dpsStatus.lockInputBuf){
		SendResponseSimple(PTP_RS_DeviceBusy);
		return 1;
	}
	return 0;
}


/***************************************************************************************/
/*  20120822  The following processing was added for the GetPartialObject command      */
/*    speed-up.                                                                        */
/*    ObjectHandle and the size of the file processed immediately before are retained. */
/*    When the command processing ends, the file is not closed.                        */
/*    The Stat purchase of the file opening and the file is done only when differing   */
/*    from ObjectHandle processed immediately before.                                  */
/***************************************************************************************/

OS_USER_ER USBPtpProcessObjectOpen(USHORT ProcKind,UINT32 objhdl,UINT32 parm1,UINT32 parm2)
{
#ifdef CO_DCF_EXIF_ON
	OS_USER_ER ret;
	UINT32	size=0,dataSize=0,cnt/*,Base=0,Base2=0*/;
	CHAR	fName[766] = "\0";
	UINT32	picX=0,picY=0,thumbX=0,thumbY=0,parentObj=0,
			ObjCompressSize=0,ThumbcompressSize=0,RestSize;		
	//UINT32	ThumbOffset=0;
	UCHAR	*Addr,*AddrW;
	OS_USER_FLGPTN	nFlag;
	FW_INT32 Err; 
	FW_CHAR srcfilepath[64];
	ULONG length;
	T_BF_FS_IF_STAT file_stat;	//jeff
	FW_LONG Index;
	T_BF_DCF_IF_FILE_INDEX File_Tbl;
	T_BF_DCF_IF_IMAGE_DETAIL t_image_detail;	
	T_BF_DCF_IF_LOAD_IMAGE_DATA t_dcf_load_image_detail;
	T_BF_DCF_IF_FILE_INFO fsfileinfo;
	
	memset(fName, 0, sizeof(fName));
	
	switch(ProcKind){
		
		case 	PTP_OP_GetObjectInfo:				
			
			if(objhdl==HANDLE_FILE_DDISCVRY)
			{
				memcpy(IN_Buffers+12,&DPSDiscovery_ObjectInfo,sizeof(DPSDiscovery_ObjectInfo));
				SendData(DPSDiscovery_ObjectInfo.FCMK-DPSDiscovery_ObjectInfo.StorageID+1+(13*2)+3);//FileName has 13 word			  
    				break;
			
			}
			else if((dpsStatus.outOperationType) && (objhdl==dpsStatus.outOperationType))
			{				
//				if (media_Type==E_BF_MEDIA_STATE_MEDIA_TYPE_MS){
//					WriteInt(ObjectInfo.StorageID,PTP_StorageID_MS);
//				}else if (media_Type==E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//					WriteInt(ObjectInfo.StorageID,PTP_StorageID_NF);
//				}else if (media_Type==E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//					WriteInt(ObjectInfo.StorageID,PTP_StorageID_SD);
//				}					
				WriteInt(ObjectInfo.StorageID,PtpStorageID);
				WriteShort(ObjectInfo.ObjectFormat,PTP_OFC_Script);//file
				WriteShort(ObjectInfo.ProtectionStatus,PROTECT_NoProtect);
				WriteInt(ObjectInfo.ObjectCompressSize,outFileSize);
				WriteShort(ObjectInfo.ThumbFormat,0);
				WriteInt(ObjectInfo.ThumbCompressSize,0);
				WriteInt(ObjectInfo.ThumbPixWidth,0);
				WriteInt(ObjectInfo.ThumbPixHeight,0);
				WriteInt(ObjectInfo.ImagePixWidth,0);
				WriteInt(ObjectInfo.ImagePixHeight,0);
				WriteInt(ObjectInfo.ParentObject,0);	
				WriteShort(ObjectInfo.AssociationType,0);
				dataSize=(ObjectInfo.FCMK-ObjectInfo.StorageID);
				//______________write FileName in ObjectInfo___________________
				Addr=ObjectInfo.FCMK;
				AddrW=Addr+1;
				switch(dpsStatus.outOperationType){
					case HANDLE_FILE_DREQUEST:
						sprintf(fName, "DREQUEST.DPS");	
						break;
					case HANDLE_FILE_DRSPONSE:
						sprintf(fName, "DRSPONSE.DPS");	
						break;
				}
				cnt=0;
				for(cnt=0;cnt<sizeof(fName) && fName[cnt] != 0;cnt++)
				{
					*AddrW=fName[cnt];
					AddrW++;
					*AddrW=0;
					AddrW++;
				}
				if(cnt>0){
					*AddrW=0;
					AddrW++;
					*AddrW=0;	
				}
				cnt++;
				ObjectInfo.FCMK[0]=(UCHAR)cnt;//FileName Length
				dataSize=cnt*2+1+dataSize;
				//_____________write CaptureDate in ObjectInfo____________________
				Addr=Addr+1+cnt*2;
				cnt=0;
	
				AddrW++;
				*AddrW=0;
				dataSize=cnt*2+1+dataSize;
				//_____________write ModificationDate in ObjectInfo________________
				Addr=Addr+1+cnt*2;
				cnt=0;
				
				AddrW++;
				*AddrW=0;
				dataSize=cnt*2+1+dataSize;
				//__________
				AddrW++;
				*AddrW=0;
				dataSize=1+dataSize;				
			}
			else if(objhdl==HANDLE_FILE_AUTO)
			{
				BF_Fs_If_Stat(MiscPath, (T_BF_FS_IF_STAT *)&file_stat);
				/*  20120822 Add from here  */
				if( PTP_fp != 0 )
				{	/*  File may be still open.  */
					BF_Fs_If_Close(PTP_fp);
				}
				/*  20120822 Add to here  */
				PTP_fp = BF_Fs_If_Open(MiscPath, D_BF_FS_IF_O_RDONLY, 0);
				ObjCompressSize = file_stat.fileSize;
				BF_Fs_If_Close(PTP_fp);
				/*  20120822 Add PTP_fp set to zero if PTP_fp is Close  */
				PTP_fp = 0;
				WriteInt(ObjectInfo.ObjectCompressSize,ObjCompressSize);
//				if (media_Type==E_BF_MEDIA_STATE_MEDIA_TYPE_MS){
//					WriteInt(ObjectInfo.StorageID,PTP_StorageID_MS);
//				}else if (media_Type==E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//					WriteInt(ObjectInfo.StorageID,PTP_StorageID_NF);
//				}else if (media_Type==E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//					WriteInt(ObjectInfo.StorageID,PTP_StorageID_SD);
//				}			
				WriteInt(ObjectInfo.StorageID,PtpStorageID);
				//WriteInt(ObjectInfo.StorageID,PTP_StorageID);
				WriteShort(ObjectInfo.ObjectFormat,PTP_OFC_DPOF);
				WriteShort(ObjectInfo.ProtectionStatus,PROTECT_ReadOnly);
				WriteShort(ObjectInfo.ThumbFormat,0);
				WriteInt(ObjectInfo.ThumbCompressSize,0);
				WriteInt(ObjectInfo.ThumbPixWidth,0);
				WriteInt(ObjectInfo.ThumbPixHeight,0);
				WriteInt(ObjectInfo.ImagePixWidth,0);
				WriteInt(ObjectInfo.ImagePixHeight,0);	
				WriteShort(ObjectInfo.AssociationType,0);
				WriteInt(ObjectInfo.ParentObject,HANDLE_DIR_MISC);	
				dataSize=(ObjectInfo.FCMK-ObjectInfo.StorageID);
				//______________write FileName in ObjectInfo___________________
				Addr=ObjectInfo.FCMK;
				AddrW=Addr+1;
				sprintf(fName, "AUTPRINT.MRK");	
				cnt=0;
				for(cnt=0;cnt<sizeof(fName) && fName[cnt] != 0;cnt++)
				{
					*AddrW=fName[cnt];
					AddrW++;
					*AddrW=0;
					AddrW++;
				}
				if(cnt>0){
					*AddrW=0;
					AddrW++;
					*AddrW=0;	
				}
				cnt++;
				ObjectInfo.FCMK[0]=(UCHAR)cnt;//FileName Length
				dataSize=cnt*2+1+dataSize;
				//_____________write CaptureDate in ObjectInfo____________________
				Addr=Addr+1+cnt*2;
				cnt=0;
	
				AddrW++;
				*AddrW=0;
				dataSize=cnt*2+1+dataSize;
				//_____________write ModificationDate in ObjectInfo________________
				Addr=Addr+1+cnt*2;
				cnt=0;
				
				AddrW++;
				*AddrW=0;
				dataSize=cnt*2+1+dataSize;
				//__________
				AddrW++;
				*AddrW=0;
				dataSize=1+dataSize;		
			}
			else if((objhdl&0x0000FFFF)!=0)
			{						
				vBO=0;	
				//_____________________Get File Name(with ObjSize)____________________
				File_Tbl.index = objhdl;
				
				if(File_Tbl.index == 0)
				{
					Index = -1;
				}
				else
				{
					Index = Object[(gBF_Ptp_Buff->ObjectSearchTbl[(File_Tbl.pack.dir_index) - 1].usObjPos)+(File_Tbl.pack.obj_index) - 1].DcfHandle; // pgr0872
				}
				
				if (Index != -1) {
					BF_Dcf_If_Set_Current_Dir(NULL, D_BF_DCF_IF_PRM_NORMAL_ACTION, (File_Tbl.index & 0xFFFF0000));
					Err = BF_Dcf_If_Get_Index_File_Path(Index, NULL, srcfilepath);
					if(Err == FJ_ERR_OK)
					{
						BF_Dcf_If_Get_File_Info(Index, &fsfileinfo);
						//memcpy(srcfilepath, DRIVE_TAG, strlen(DRIVE_TAG));
						//srcfilepath[7] = '\\';
						//srcfilepath[16] = '\\';
						Err = BF_Fs_If_Stat((const CHAR*)srcfilepath, (T_BF_FS_IF_STAT *)&file_stat);
						if(Err == FJ_ERR_OK)
						{
							//PTP_fp = BF_Fs_If_Open(srcfilepath, D_BF_FS_IF_O_RDONLY, 0);
							//if(PTP_fp == 0)
							//{
							//	SendResponseSimple(PTP_RS_InvalidObjectHandle);
							//	return 1;
							//}
						}
						else
						{
							SendResponseSimple(PTP_RS_InvalidObjectHandle);
							return 1;
						}
					}
					else
					{
						SendResponseSimple(PTP_RS_InvalidObjectHandle);
						return 1;
					}
				} else {
					SendResponseSimple(PTP_RS_InvalidObjectHandle);
					return 1;
				}
				
				ObjCompressSize = file_stat.fileSize;
				//-----------------------------------------------

//				if (media_Type==E_BF_MEDIA_STATE_MEDIA_TYPE_MS){
//					WriteInt(ObjectInfo.StorageID,PTP_StorageID_MS);
//				}else if (media_Type==E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//					WriteInt(ObjectInfo.StorageID,PTP_StorageID_NF);
//				}else if (media_Type==E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//					WriteInt(ObjectInfo.StorageID,PTP_StorageID_SD);
//				}			
				WriteInt(ObjectInfo.StorageID,PtpStorageID);
				
				WriteShort(ObjectInfo.ProtectionStatus,PROTECT_NoProtect);
				WriteInt(ObjectInfo.ObjectCompressSize,ObjCompressSize);
				WriteInt(ObjectInfo.ThumbCompressSize,ThumbcompressSize);
				WriteInt(ObjectInfo.ThumbPixWidth,thumbX);
				WriteInt(ObjectInfo.ThumbPixHeight,thumbY);
				WriteInt(ObjectInfo.ImagePixWidth,picX);
				WriteInt(ObjectInfo.ImagePixHeight,picY);
				WriteInt(ObjectInfo.ParentObject,parentObj);	
				WriteShort(ObjectInfo.AssociationType,0);
		
				dataSize=(ObjectInfo.FCMK-ObjectInfo.StorageID);
				//______________write FileName in ObjectInfo___________________
				Addr=ObjectInfo.FCMK;
				AddrW=Addr+1;

				//parentObj = Object[Index].DcfHandle;
				WriteInt(ObjectInfo.ParentObject,parentObj);	
				switch(BF_Dcf_If_Convert_File_Type(fsfileinfo.file_type))
				{
					case E_BF_DCF_IF_FILE_TYPE_AVI:
						WriteShort(ObjectInfo.ObjectFormat,0x300A);
						break;
					case E_BF_DCF_IF_FILE_TYPE_JPG:
					default:
						WriteShort(ObjectInfo.ObjectFormat,PTP_OFC_EXIFJPEG);	
						break;	
				}
		        strcpy(fName, file_stat.fname);
				cnt=0;
				for(cnt=0;cnt<sizeof(fName) && fName[cnt] != 0;cnt++)
				{
					*AddrW=fName[cnt];
					AddrW++;
					*AddrW=0;
					AddrW++;
				}
				if(cnt>0){
					*AddrW=0;
					AddrW++;
					*AddrW=0;	
				}
				cnt++;
				ObjectInfo.FCMK[0]=(UCHAR)cnt;//FileName Length
				dataSize=cnt*2+1+dataSize;

#if 1
				/* Set Nodata to CaptureDate, ModificationDate and Keywords. */
				AddrW++;
				*AddrW=0;
				AddrW++;
				*AddrW=0;
				AddrW++;
				*AddrW=0;
				dataSize+=3;
				
#else
				//_____________write CaptureDate in ObjectInfo____________________
				Addr=Addr+1+cnt*2;	
				AddrW = Addr + 1;
				ObjectInfo.FCMK[1+cnt*2]=(UCHAR)0x10;
				switch(BF_Dcf_If_Convert_File_Type(fsfileinfo.file_type))
				{
					case E_BF_DCF_IF_FILE_TYPE_AVI:
						sprintf(fName, "%02d%02d%02d%c%02d%02d%02d", ((file_stat.date >> 9) + 1980), (file_stat.date >> 5) & 0x0f, (file_stat.date) & 0x1f, \
						0x54, (file_stat.time >> 11) & 0x1f, (file_stat.time >> 5) & 0x3f, (file_stat.time & 0x1f) * 2);
						break;
					case E_BF_DCF_IF_FILE_TYPE_JPG:
					default:
						BF_Dcf_If_Get_Exif_Info_Jpeg(Index, &t_image_detail, &t_dcf_load_image_detail);
						sprintf(fName, "%02d%02d%02d%c%02d%02d%02d", t_image_detail.DateTimeOriginal.ad_year, t_image_detail.DateTimeOriginal.month, t_image_detail.DateTimeOriginal.day, \
						0x54, t_image_detail.DateTimeOriginal.hour, t_image_detail.DateTimeOriginal.min, t_image_detail.DateTimeOriginal.sec);	
						break;	
				}
				for(cnt=0;cnt<sizeof(fName) && fName[cnt] != 0;cnt++)
				{
					*AddrW=fName[cnt];
					AddrW++;
					*AddrW=0;
					AddrW++;
				}
				if(cnt>0){
					*AddrW=0;
					AddrW++;
					*AddrW=0;	
				}
				cnt++;

				AddrW++;
				*AddrW=0;
				dataSize=cnt*2+1+dataSize;
				//_____________write ModificationDate in ObjectInfo________________
				Addr=Addr+1+cnt*2;
				cnt=0;
				
				AddrW++;
				*AddrW=0;
				dataSize=cnt*2+1+dataSize;
				//__________
				AddrW++;
				*AddrW=0;
				dataSize=1+dataSize;	
#endif

				switch(BF_Dcf_If_Convert_File_Type(fsfileinfo.file_type))
				{
					case E_BF_DCF_IF_FILE_TYPE_AVI:
						Err = BF_Dcf_If_Get_Index_File_Path( Index, (const CHAR*)"THM", (CHAR*)srcfilepath);
						if( Err == 0 )
						{		
							Err = BF_Fs_If_Stat((const CHAR*)srcfilepath, (T_BF_FS_IF_STAT *)&file_stat);
							if(Err == FJ_ERR_OK)	
							{
								ThumbcompressSize = file_stat.fileSize;
								thumbX = WIDTH_THM;
								thumbY = LINES_THM;
							}			
							else
							{
								ThumbcompressSize = 0;
								thumbX = 0;
								thumbY = 0;
							}
						}
						else
						{
							ThumbcompressSize = 0;
							thumbX = 0;
							thumbY = 0;
						}
						WriteInt(ObjectInfo.ThumbCompressSize,ThumbcompressSize);		
						WriteInt(ObjectInfo.ThumbPixWidth,thumbX);
						WriteInt(ObjectInfo.ThumbPixHeight,thumbY);
						WriteShort(ObjectInfo.ThumbFormat,0x3808);
						break;
					case E_BF_DCF_IF_FILE_TYPE_JPG:
					default:
						Err = BF_Dcf_If_Get_Exif_Info_Jpeg(Index, &t_image_detail, &t_dcf_load_image_detail);
						if(Err == 0)
						{
							ThumbcompressSize = t_dcf_load_image_detail.thumb_size;
							picX = t_image_detail.width;
							picY = t_image_detail.lines;
							if(ThumbcompressSize)
							{
								thumbX = WIDTH_THUMB;
								thumbY = LINES_THUMB;
							}
							else
							{
								thumbX = 0;
								thumbY = 0;
							}
						}
						else
						{
							ThumbcompressSize = 0;
							picX = 0;
							picY = 0;
							thumbX = 0;
							thumbY = 0;
						}
						WriteInt(ObjectInfo.ThumbCompressSize,ThumbcompressSize);
						WriteInt(ObjectInfo.ImagePixWidth,picX);
						WriteInt(ObjectInfo.ImagePixHeight,picY);
						WriteInt(ObjectInfo.ThumbPixWidth,thumbX);
						WriteInt(ObjectInfo.ThumbPixHeight,thumbY);
						WriteShort(ObjectInfo.ThumbFormat,0x3808);
						break;
				}		
			}
			
			memcpy(IN_Buffers+12,&ObjectInfo,sizeof(ObjectInfo));
			SendData(dataSize);
			break;
			
		case	PTP_OP_GetThumb:

			if((objhdl&0x0000FFFF)!=0)
			{
				vBO=0;	
				//_____________________Get File Name(with ObjSize)____________________
				File_Tbl.index = objhdl;
				
				if(File_Tbl.index == 0)
				{
					Index = -1;
				}
				else
				{
					Index = Object[(gBF_Ptp_Buff->ObjectSearchTbl[(File_Tbl.pack.dir_index) - 1].usObjPos)+(File_Tbl.pack.obj_index) - 1].DcfHandle; // pgr0872
				}
				
				if (Index != -1) {
					BF_Dcf_If_Set_Current_Dir(NULL, D_BF_DCF_IF_PRM_NORMAL_ACTION, (File_Tbl.index & 0xFFFF0000));
					Err = BF_Dcf_If_Get_Index_File_Path(Index, NULL, srcfilepath);
					if(Err == FJ_ERR_OK)
					{
						BF_Dcf_If_Get_File_Info(Index, &fsfileinfo);
						//memcpy(srcfilepath, DRIVE_TAG, strlen(DRIVE_TAG));
						//srcfilepath[7] = '\\';
						//srcfilepath[16] = '\\';
						Err = BF_Fs_If_Stat((const CHAR*)srcfilepath, (T_BF_FS_IF_STAT *)&file_stat);
						if(Err == FJ_ERR_OK)
						{
							/*  20120822 Add from here  */
							if( PTP_fp != 0 )
							{	/*  File may be still open.  */
								BF_Fs_If_Close(PTP_fp);
							}
							/*  20120822 Add to here  */
							PTP_fp = BF_Fs_If_Open(srcfilepath, D_BF_FS_IF_O_RDONLY, 0);
							if(PTP_fp == 0)
							{
								/*  20120822 Add  form here  */
								PTP_fp = 0;
								/*  20120822 Add  form here  */
								SendResponseSimple(PTP_RS_InvalidObjectHandle);
								return 1;
							}
						}
						else
						{
							SendResponseSimple(PTP_RS_InvalidObjectHandle);
							return 1;
						}
					}
					else
					{
						SendResponseSimple(PTP_RS_InvalidObjectHandle);
						return 1;
					}
				} else {
					SendResponseSimple(PTP_RS_InvalidObjectHandle);
					return 1;
				}
				#if 0
				if(sizeof(gBF_Ptp_Buff->Buffers1)<2000)
					size = PTP_fread((ULONG)IN_Buffers, sizeof(gBF_Ptp_Buff->Buffers1));
				else
					size = PTP_fread((ULONG)IN_Buffers, 2000);

				if((*(IN_Buffers+0xc)=='I') && (*(IN_Buffers+0xd)=='I')){
					vBO=1;
				}else
				if((*(IN_Buffers+0xc)=='M') && (*(IN_Buffers+0xd)=='M')){
					vBO=2;
				}else		
				{
					//......Err
					BF_Fs_If_Close(PTP_fp);
					PTP_fp=0;
					SendResponseSimple(PTP_RS_StoreNotAvailable);		
					return(1);	
				}
				//____________________Search EXIF ThumbSize/ThumbOffset___________________________
				Addr=IN_Buffers+0xc+readIIMMInt(IN_Buffers+0x10);
				
				Addr=Addr+2+readIIMMShort(Addr)*12;//last in 0th-IFD pointer to 1st-IFD Offset
				Addr=IN_Buffers+0xc+readIIMMInt(Addr);
				
				readTagValue(Addr,0x0201,&ThumbOffset);//use for GET_THUMB//<<<<<<<<<<<<<<
				readTagValue(Addr,0x0202,&ThumbcompressSize);//<<<<<<<<<<<<<<<<<<<<<<<<
				//_________________________________Loop Sending___________________________
				Base=0xc+ThumbOffset;//Thumb place addr 
				RestSize=ThumbcompressSize;
				BF_Fs_If_Lseek(PTP_fp, Base, SEEK_SET, &length);
				size = PTP_fread((ULONG)(IN_Buffers+12), CalcSendAndRest(1024-12,&RestSize));	
				//SendDataDetail(ThumbcompressSize,size);	
				SendDataDetail2(ThumbcompressSize,size);	
				if(RestSize > 0)
				{
					while(RestSize)
					{
						size = PTP_fread((ULONG)IN_Buffers, CalcSendAndRest(1024,&RestSize));
						MWUSB_SetMode(0);	
						MWUSB_Write((UCHAR*)IN_Buffers, size, BULK_WRITE_FIFO);				
					}
					if((size % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
    				{
						OS_User_Tslp_Tsk(1);
						MWUSB_SendNullPacket(BULK_WRITE_FIFO);
    				}		
				}
				else
				{
					if(((size + 12) % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
    				{
						OS_User_Tslp_Tsk(1);
						MWUSB_SendNullPacket(BULK_WRITE_FIFO);
    				}		
				}
				#endif
				BF_Fs_If_Close(PTP_fp);
				PTP_fp=0;
				switch(BF_Dcf_If_Convert_File_Type(fsfileinfo.file_type))
				{
					case E_BF_DCF_IF_FILE_TYPE_AVI:
#if 0
						Err = MJP_Ctl_open_file_THM( Index, &PTP_fp );
						if( Err == 0 )
						{
							BF_Fs_If_Read(PTP_fp, (FW_VOID*)(IN_Buffers+12), 65536-12,(FW_ULONG*)&RestSize);
							size=ThumbcompressSize=RestSize;
							SendDataDetail2(ThumbcompressSize,size);
							if(((size+12) % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
    						{
								OS_User_Tslp_Tsk(1);
								MWUSB_SendNullPacket(BULK_WRITE_FIFO);
    						}			
							Err = BF_Fs_If_Close(PTP_fp);
							PTP_fp = 0;
						}
						else
						{
							ThumbcompressSize = size = 0;
							SendDataDetail2(ThumbcompressSize,size);
							if(((size+12) % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
    						{
								OS_User_Tslp_Tsk(1);
								MWUSB_SendNullPacket(BULK_WRITE_FIFO);
    						}			
						}
#endif
						break;
					case E_BF_DCF_IF_FILE_TYPE_JPG:
					default:
						Err = BF_Dcf_If_Load_Thumb_Jpeg( Index,
								 (FW_ULONG)(IN_Buffers+12),
								 65536 - 12,
								 &length,
								 &t_image_detail );
						if(Err == 0)
						{
							size=RestSize=ThumbcompressSize=length;
							SendDataDetail2(ThumbcompressSize,size);
							if(((size+12) % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
    						{
								OS_User_Tslp_Tsk(1);
								MWUSB_SendNullPacket(BULK_WRITE_FIFO);
    						}			
						}
						else
						{
							ThumbcompressSize = size = 0;
							SendDataDetail2(ThumbcompressSize,size);
							if(((size+12) % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
    						{
								OS_User_Tslp_Tsk(1);
								MWUSB_SendNullPacket(BULK_WRITE_FIFO);
    						}			
						}
						break;
				}
			}
			else
			{
				SendResponseSimple(PTP_RS_InvalidObjectHandle);
				return 1;
			}

			break;
			
		case	PTP_OP_GetObject://<<<BigSize>>>	
			if((dpsStatus.outOperationType) && (objhdl==dpsStatus.outOperationType))
			{
				memcpy(IN_Buffers+12,dpsOutBufferPtr,outFileSize);				
				SendData(outFileSize);
				
				ret = OS_User_Pol_Flg(FID_DPSTSK, (FLG_DPSPRT2), D_OS_USER_TWF_ORW, &nFlag);
				if((ret != D_OS_USER_E_OK) && dpsStatus.outOperationType==HANDLE_FILE_DRSPONSE)    
			    	OS_User_Set_Flg(FID_DPSTSK, FLG_DPSPRT2);    
				
				OS_User_Clr_Flg(FID_DPSTSK,~FLG_DPSDSC);
			}
			else if((objhdl==HANDLE_FILE_AUTO))
			{	
				/*  20120822 Add from here  */
				if( PTP_fp != 0 )
				{	/*  File may be still open.  */
					BF_Fs_If_Close(PTP_fp);
				}
				/*  20120822 Add to here  */
				BF_Fs_If_Stat(MiscPath, (T_BF_FS_IF_STAT *)&file_stat);
				PTP_fp = BF_Fs_If_Open(MiscPath, D_BF_FS_IF_O_RDONLY, 0);
				ObjCompressSize = file_stat.fileSize;
				//_________________________________Loop Sending___________________________

				BF_Fs_If_Lseek(PTP_fp, 0L, SEEK_SET, &length);                     
				RestSize=ObjCompressSize;

				//jeff: read a very large data block each time for optimization
				size = PTP_fread((ULONG)(IN_Buffers+12), CalcSendAndRest(65536-12,&RestSize));
				SendDataDetail2(ObjCompressSize,size);			
				if(RestSize > 0)
				{
					while(RestSize)
					{
						size = PTP_fread((ULONG)IN_Buffers, CalcSendAndRest(65536,&RestSize));
						MWUSB_SetMode(0);	
						MWUSB_Write((UCHAR*)IN_Buffers, size, BULK_WRITE_FIFO);				
					}
					if((size % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
    				{
						OS_User_Tslp_Tsk(1);
						MWUSB_SendNullPacket(BULK_WRITE_FIFO);
    				}		
				}
				else
				{
					if(((size + 12) % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
    				{
						OS_User_Tslp_Tsk(1);
						MWUSB_SendNullPacket(BULK_WRITE_FIFO);
    				}		
				}
				BF_Fs_If_Close(PTP_fp);
				PTP_fp=0;
			}
			else if((objhdl&0x0000FFFF)!=0)
			{	
				vBO=0;	
				
				File_Tbl.index = objhdl;
				
				if(File_Tbl.index == 0)
				{
					Index = -1;
				}
				else
				{
					Index = Object[(gBF_Ptp_Buff->ObjectSearchTbl[(File_Tbl.pack.dir_index) - 1].usObjPos)+(File_Tbl.pack.obj_index) - 1].DcfHandle; // pgr0872
				}
				
				if (Index != -1) {
					BF_Dcf_If_Set_Current_Dir(NULL, D_BF_DCF_IF_PRM_NORMAL_ACTION, (File_Tbl.index & 0xFFFF0000));
					Err = BF_Dcf_If_Get_Index_File_Path(Index, NULL, srcfilepath);
					if(Err == FJ_ERR_OK)
					{
						//memcpy(srcfilepath, DRIVE_TAG, strlen(DRIVE_TAG));
						//srcfilepath[7] = '\\';
						//srcfilepath[16] = '\\';
						Err = BF_Fs_If_Stat((const CHAR*)srcfilepath, (T_BF_FS_IF_STAT *)&file_stat);
						if(Err == FJ_ERR_OK)
						{
							/*  20120822 Add from here  */
							if( PTP_fp != 0 )
							{	/*  File may be still open.  */
								BF_Fs_If_Close(PTP_fp);
							}
							/*  20120822 Add to here  */
							PTP_fp = BF_Fs_If_Open(srcfilepath, D_BF_FS_IF_O_RDONLY, 0);
							if(PTP_fp == 0)
							{
								/*  20120822 Add  form here  */
								PTP_fp = 0;
								/*  20120822 Add  form here  */
								SendResponseSimple(PTP_RS_InvalidObjectHandle);
								return 1;
							}
						}
						else
						{
							SendResponseSimple(PTP_RS_InvalidObjectHandle);
							return 1;
						}
					}
					else
					{
						SendResponseSimple(PTP_RS_InvalidObjectHandle);
						return 1;
					}
				} else {
					SendResponseSimple(PTP_RS_InvalidObjectHandle);
					return 1;
				}
				ObjCompressSize = file_stat.fileSize;
				//_________________________________Loop Sending___________________________

				BF_Fs_If_Lseek(PTP_fp, 0L, SEEK_SET, &length);                       
				RestSize=ObjCompressSize;

				//jeff: read a very large data block each time for optimization
				size = PTP_fread((ULONG)(IN_Buffers+12), CalcSendAndRest(65536-12,&RestSize));
				//SendDataDetail(ObjCompressSize,size);		
				SendDataDetail2(ObjCompressSize,size);			
				if(RestSize > 0)
				{
					while(RestSize)
					{
						size = PTP_fread((ULONG)IN_Buffers, CalcSendAndRest(65536,&RestSize));
						MWUSB_SetMode(0);	
						MWUSB_Write((UCHAR*)IN_Buffers, size, BULK_WRITE_FIFO);				
					}
					if((size % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
    				{
						OS_User_Tslp_Tsk(1);
						MWUSB_SendNullPacket(BULK_WRITE_FIFO);
    				}		
				}
				else
				{
					if(((size + 12) % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
    				{
						OS_User_Tslp_Tsk(1);
						MWUSB_SendNullPacket(BULK_WRITE_FIFO);
    				}		
				}
				/*
				while(RestSize){	
					size = PTP_fread((ULONG)IN_Buffers, CalcSendAndRest(65536,&RestSize));
					MWUSB_SetMode(0);
					MWUSB_Write((UCHAR*)IN_Buffers, size, BULK_WRITE_FIFO);
				}	
				*/
				BF_Fs_If_Close(PTP_fp);
				PTP_fp=0;
			}
			else
			{		
				SendResponseSimple(PTP_RS_InvalidObjectHandle);
				return(1);	
			}
			break;
			
		case	PTP_OP_GetPartialObject://<<<BigSize>>>	
			/*  20120822 Add form here  */
			if( objhdl != Ptp_Cache_GetPartialObject.PrevObjHandle )
			{	/* Change Object  */
	    		ptp_ClearGetPartialObectCache();
			}
			/*	20120822 Add to here    */
			if(objhdl == HANDLE_FILE_AUTO)
			{
#if 1			
				/*  20120822 Add form here  */
				if( (objhdl != Ptp_Cache_GetPartialObject.PrevObjHandle)||(PTP_fp == 0) )
				{	/*  Change Object  */
				BF_Fs_If_Stat(MiscPath, (T_BF_FS_IF_STAT *)&file_stat);
				PTP_fp = BF_Fs_If_Open(MiscPath, D_BF_FS_IF_O_RDONLY, 0);
				ObjCompressSize = file_stat.fileSize;
					Ptp_Cache_GetPartialObject.PrevObjHandle = objhdl;
					Ptp_Cache_GetPartialObject.PrevObjCompressSize = ObjCompressSize;
				}
				else
				{	/*  Same Object */
					ObjCompressSize = Ptp_Cache_GetPartialObject.PrevObjCompressSize;
				}
				/*	20120822 Add to here    */	
#else			
				BF_Fs_If_Stat(MiscPath, (T_BF_FS_IF_STAT *)&file_stat);
				PTP_fp = BF_Fs_If_Open(MiscPath, D_BF_FS_IF_O_RDONLY, 0);
				ObjCompressSize = file_stat.fileSize;
#endif
				//_________________________________Loop Sending___________________________
				BF_Fs_If_Lseek(PTP_fp, parm1, SEEK_SET, &length);   				
				RestSize=parm2;
					
				if(RestSize>(ObjCompressSize-parm1)){
					RestSize=(ObjCompressSize-parm1);					
				}
				ObjCompressSize=RestSize;
				SendNumber=ObjCompressSize;

				size = PTP_fread((ULONG)(IN_Buffers+12),
//20120815							 CalcSendAndRest(1024-12,&RestSize));
							 		CalcSendAndRest(65536-12,&RestSize));
				//SendDataDetail(ObjCompressSize,size);		
				SendDataDetail2(ObjCompressSize,size);			
				if(RestSize > 0)
				{
					while(RestSize)
					{
						size = PTP_fread((ULONG)IN_Buffers,
//20120815									 CalcSendAndRest(1024,&RestSize));
											 CalcSendAndRest(65536,&RestSize));
						MWUSB_SetMode(0);	
						MWUSB_Write((UCHAR*)IN_Buffers, size, BULK_WRITE_FIFO);				
					}
					if((size % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
    				{
						OS_User_Tslp_Tsk(1);
						MWUSB_SendNullPacket(BULK_WRITE_FIFO);
    				}		
				}
				else
				{
					if(((size + 12) % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
    				{
						OS_User_Tslp_Tsk(1);
						MWUSB_SendNullPacket(BULK_WRITE_FIFO);
    				}		
				}
#if 0
				BF_Fs_If_Close(PTP_fp);
				PTP_fp=0;
#endif
			}
			else
			{
				if((objhdl&0x0000FFFF)!=0)
				{
					vBO=0;	
#if 1
					if( (objhdl != Ptp_Cache_GetPartialObject.PrevObjHandle)||(PTP_fp == 0) )
					{	/*  Change Object  */
						//_____________________Get File Name(with ObjSize)____________________
					
					
						File_Tbl.index = objhdl;
				
						if(File_Tbl.index == 0)
						{
							Index = -1;
						}
						else
						{
							Index = Object[(gBF_Ptp_Buff->ObjectSearchTbl[(File_Tbl.pack.dir_index) - 1].usObjPos)+(File_Tbl.pack.obj_index) - 1].DcfHandle; // pgr0872
						}
				
						if (Index != -1) {
							BF_Dcf_If_Set_Current_Dir(NULL, D_BF_DCF_IF_PRM_NORMAL_ACTION, (File_Tbl.index & 0xFFFF0000));
							Err = BF_Dcf_If_Get_Index_File_Path(Index, NULL, srcfilepath);
							if(Err == FJ_ERR_OK)
							{
								//memcpy(srcfilepath, DRIVE_TAG, strlen(DRIVE_TAG));
								//srcfilepath[7] = '\\';
								//srcfilepath[16] = '\\';
								Err = BF_Fs_If_Stat((const CHAR*)srcfilepath, (T_BF_FS_IF_STAT *)&file_stat);
								if(Err == FJ_ERR_OK)
								{
									PTP_fp = BF_Fs_If_Open(srcfilepath, D_BF_FS_IF_O_RDONLY, 0);
									if(PTP_fp == 0)
									{
										SendResponseSimple(PTP_RS_InvalidObjectHandle);
										return 1;
									}
								}
								else
								{
									SendResponseSimple(PTP_RS_InvalidObjectHandle);
									return 1;
								}
							}
							else
							{
								SendResponseSimple(PTP_RS_InvalidObjectHandle);
								return 1;
							}
						} else {
							SendResponseSimple(PTP_RS_InvalidObjectHandle);
							return 1;
						}
						/////////
						ObjCompressSize = file_stat.fileSize;
						Ptp_Cache_GetPartialObject.PrevObjHandle = objhdl;
						Ptp_Cache_GetPartialObject.PrevObjCompressSize = ObjCompressSize;
					}
					else
					{
						ObjCompressSize = Ptp_Cache_GetPartialObject.PrevObjCompressSize;
					}
#else
					//_____________________Get File Name(with ObjSize)____________________
					
					
					File_Tbl.index = objhdl;
				
					if(File_Tbl.index == 0)
					{
						Index = -1;
					}
					else
					{
						Index = Object[(gBF_Ptp_Buff->ObjectSearchTbl[(File_Tbl.pack.dir_index) - 1].usObjPos)+(File_Tbl.pack.obj_index) - 1].DcfHandle;
					}
				
					if (Index != -1) {
						printf("Index :%d\n", Index);
						BF_Dcf_If_Set_Current_Dir(NULL, D_BF_DCF_IF_PRM_NORMAL_ACTION, (File_Tbl.index & 0xFFFF0000));
						Err = BF_Dcf_If_Get_Index_File_Path(Index, NULL, srcfilepath);
						if(Err == FJ_ERR_OK)
						{
							//memcpy(srcfilepath, DRIVE_TAG, strlen(DRIVE_TAG));
							//srcfilepath[7] = '\\';
							//srcfilepath[16] = '\\';
							Err = BF_Fs_If_Stat((const CHAR*)srcfilepath, (T_BF_FS_IF_STAT *)&file_stat);
							if(Err == FJ_ERR_OK)
							{
								PTP_fp = BF_Fs_If_Open(srcfilepath, D_BF_FS_IF_O_RDONLY, 0);
								if(PTP_fp == 0)
								{
									SendResponseSimple(PTP_RS_InvalidObjectHandle);
									return 1;
								}
							}
							else
							{
								SendResponseSimple(PTP_RS_InvalidObjectHandle);
								return 1;
							}
						}
						else
						{
							SendResponseSimple(PTP_RS_InvalidObjectHandle);
							return 1;
						}
					} else {
						SendResponseSimple(PTP_RS_InvalidObjectHandle);
						return 1;
					}
					ObjCompressSize = file_stat.fileSize;
#endif				
					//_________________________________Loop Sending___________________________
					BF_Fs_If_Lseek(PTP_fp, parm1, SEEK_SET, &length);   				
					RestSize=parm2;
					
					if(RestSize>(ObjCompressSize-parm1)){
						RestSize=(ObjCompressSize-parm1);					
					}
					ObjCompressSize=RestSize;
					SendNumber=ObjCompressSize;

//20120815				size = PTP_fread((ULONG)(IN_Buffers+12), CalcSendAndRest(1024-12,&RestSize));	
					size = PTP_fread((ULONG)(IN_Buffers+12), CalcSendAndRest(65536-12,&RestSize));
					SendDataDetail2(ObjCompressSize,size);		
					if(RestSize > 0)
					{
						while(RestSize)
						{
							size = PTP_fread((ULONG)IN_Buffers,
//20120815										 CalcSendAndRest(1024,&RestSize));
												 CalcSendAndRest(65536,&RestSize));
							MWUSB_SetMode(0);	
							MWUSB_Write((UCHAR*)IN_Buffers, size, BULK_WRITE_FIFO);				
						}
						if((size % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
	    				{
#if 1					
							/*  20120822 Add form here  */
							while( MWUSB_CheckFifoEmpty(BULK_WRITE_FIFO) == MWUSB_NG )
							{	/*  Fifo is not empty  */
								if( vSessionID == 0 )
								{	/*  Session Closed */
									break ;
								}
							}
							/*  20120822 Add too here  */
#else
							OS_User_Tslp_Tsk(1);
#endif
							MWUSB_SendNullPacket(BULK_WRITE_FIFO);
	    				}		
					}
					else
					{
						if(((size + 12) % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
	    				{
#if 1
							/*  20120822 Add form here  */
							while( MWUSB_CheckFifoEmpty(BULK_WRITE_FIFO) == MWUSB_NG )
							{	/*  Fifo is not empty  */
								if( vSessionID == 0 )
								{	/*  Session Closed */
									break ;
								}
							}
#else
							OS_User_Tslp_Tsk(1);
#endif
							MWUSB_SendNullPacket(BULK_WRITE_FIFO);
	    				}		
					}
#if 0  //20120822
					BF_Fs_If_Close(PTP_fp);
					PTP_fp=0;
#endif
				}
				else
				{		
					SendResponseSimple(PTP_RS_InvalidObjectHandle);
					return 1;
				}	
			}
			break;
	}
#endif
	return(0);
}



VOID DSP_Discovery( VOID )
// Where PTP had comnunicate with Printer for DPS Layer discovery
// PTP will Call this function for DPS discovery succeed
{
	MSG outmsg = {ePictBridgeSubCmd_ProcessDpsEvent, ePictBridgeSubCmd_ProcessDpsEvent, (VOID*)0, {0}}; 
	dpsStatus.DPSKeyAction=DPS_LINKED;
	OS_User_Set_Flg(FID_DPSTSK, FLG_DPSPRT2);  
	SendMessage(outmsg,eDpsSubSysMQ ,10);
}

VOID SysMode_USB_PTP( INT32 stacd)
{
	OS_USER_ER retCode;
	OS_USER_FLGPTN retptn;
	
	BF_Debug_Print_Information(("PTP Task Start.....\r\n"));
	
	OS_User_Clr_Flg(FID_PTPTSK, 0);
	
	do {
		
		execPTPCmd();
		
		// Check end task flag
		//OS_User_Tslp_Tsk(1);
		retCode = OS_User_Twai_Flg(FID_PTPTSK, FLG_PTP_TERMINATE, D_OS_USER_TWF_ORW, &retptn, 1);   // timeout: 1ms
		if (retCode != D_OS_USER_E_OK &&
			retCode != D_OS_USER_E_TMOUT &&
			retCode != D_OS_USER_E_RLWAI &&
			retCode != D_OS_USER_E_DISWAI) {
			
			BF_Debug_Print_Error(("PTP Task flag error. retCode=%d\n", retCode));
			break;
		}
	} while (retCode != D_OS_USER_E_OK || (retptn & FLG_PTP_TERMINATE) != FLG_PTP_TERMINATE);
	
	OS_User_Clr_Flg(FID_PTPTSK, 0);
	
	// Release mdf resource
	BF_Fs_If_End_Task_Mdf();
	
	BF_Debug_Print_Information(("PTP Task End.....\r\n"));
	OS_User_Ext_Tsk();
}

//========================================================
//	*Addr	:	The Address of Data buffer for send to Printer
//	size 	:	This size of Data in data buffer
//	SendKind:	1:[DREQUEST.DPS]	2:[DRSPONSE.DPS]
VOID SendDSPCmdFile( VOID )
{
	if(!vSessionOpen) return;
	if(ScriptFile_Use(dpsStatus.outOperationType))
	{
		if(dpsStatus.outOperationType == DSC_RESPONSE)
		{
			dpsOutBufferPtr = (UCHAR*)dscResBufPtr;
			outFileSize = dpsStatus.dscResponseFileSize;
		}	
		else
		{
			dpsOutBufferPtr = (UCHAR*)dscReqBufPtr;
			outFileSize = dpsStatus.dscRequestFileSize;
		}
		SendEvent(PTP_EV_RequestObjectTransfer,dpsStatus.outOperationType);
	}	
	// ......................... Process by PTP Layer function call ...............
	//	[PTP] <-- GUI/DPS
}

/*--------------------------------------------------------------------------
<Function>    UINT32 PTP_GetObjectHandle
<Argument>    EXIF_FILE_t pExif : Exif file information structure [in/out]
			  UINT32        ptr	
<Return>      UINT32     : File Handle Count
<Description> 
--------------------------------------------------------------------------*/
//INT32 PTP_GetObjectHandle(E_BF_MEDIA_STATE_MEDIA_TYPE mediaType, USHORT ObjectFormatCode, UINT32 *ptr)
INT32 PTP_GetObjectHandle(USHORT ObjectFormatCode, UINT32 *ptr)
{
	ULONG i;
	UINT32 hdlcnt=0;
	if ((ObjectFormatCode == PTP_OFC_AllPicture) || (ObjectFormatCode == PTP_OFC_None) || (ObjectFormatCode == PTP_OFC_EXIFJPEG) || (ObjectFormatCode == PTP_OFC_JPEG))
	{
		//MSG outmsg;
		FW_LONG cur_dir_index = 0;
		//BF_Dcf_If_Get_Dir_Index( DCF_DB_CURRENT_DIR<<16, &cur_dir_index );
		cur_dir_index = ulCurrentDir >> 16;
		if(cur_dir_index)
		{
			for (i = gBF_Ptp_Buff->ObjectSearchTbl[cur_dir_index - 1].usObjPos; i < ((gBF_Ptp_Buff->ObjectSearchTbl[cur_dir_index - 1].usObjPos) + (gBF_Ptp_Buff->ObjectSearchTbl[cur_dir_index - 1].usObjCnt)); i++)
			{
				if(Object[i].DcfHandle != 0)
				{
					hdlcnt++;
					if (ptr != 0)
					{
						*(ptr++) = ENDIAN_SWAP32(Object[i].DcfHandle);		// pgr0689
					}
				}
			}
		}
	}
	else if(ObjectFormatCode == PTP_OFC_DPOF)
	{
		if(gBF_Ptp_Buff->Object_DPOF.DcfHandle != 0)
		{
			hdlcnt++;
			if (ptr)
			{
				*(ptr++) = ENDIAN_SWAP32(gBF_Ptp_Buff->Object_DPOF.DcfHandle);
			}
		}
	}
	return hdlcnt;
}

INT32 PTP_GetAllObjectInfo(E_BF_MEDIA_STATE_MEDIA_TYPE mediaType, ULONG *ObjectCount, ULONG *JpgCount)
{ 
	FW_INT32 jpgCnt = 0;
#ifdef CO_DCF_EXIF_ON
	FW_INT32 Err;
	FW_INT32 objCnt = 0; 
	T_BF_DCF_IF_FILE_INFO fsfileinfo;
	CHAR dirName[10];
    USHORT date;
    USHORT time;
    E_BF_DCF_IF_FILE_TYPE filttype = E_BF_DCF_IF_FILE_TYPE_NOTHING;
	FW_ULONG file_index;
	T_BF_DCF_IF_DB_INFO ret_data;
	T_BF_DCF_IF_FILE_INDEX File_Tbl;
	memset(&ret_data, 0, sizeof(T_BF_DCF_IF_DB_INFO));
	memset(&File_Tbl, 0, sizeof(T_BF_DCF_IF_FILE_INDEX));
	BF_Dcf_If_Read_Dir_Total_Count(&(File_Tbl.pack.dir_index));
	ret_data.p_objectCnt = (FW_SHORT*)(&(File_Tbl.pack.obj_index));
	ret_data.dirName = (FW_CHAR*)dirName;
	ret_data.p_date = &date;
	ret_data.p_time = &time;
	BF_Dcf_If_Get_Obj_Info(File_Tbl.index, &ret_data);
	BF_Dcf_If_Get_Dir_Index( DCF_DB_CURRENT_DIR<<16, &ulCurrentDir );
	if((File_Tbl.index) <= 0)
	{
		*ObjectCount = *JpgCount = 0;
		return 0;
	}
	while(File_Tbl.pack.dir_index)
	{
		ulTotalDPSDir++;
		BF_Dcf_If_Set_Current_Dir(NULL, D_BF_DCF_IF_PRM_NORMAL_ACTION, File_Tbl.index);
		BF_Dcf_If_Get_Obj_Info(File_Tbl.index, &ret_data);
		if(File_Tbl.pack.obj_index <= 0) /* ignore empty dir */
		{
			gBF_Ptp_Buff->ObjectSearchTbl[(File_Tbl.pack.dir_index) - 1].usObjPos = -1;
			gBF_Ptp_Buff->ObjectSearchTbl[(File_Tbl.pack.dir_index) - 1].usObjCnt = -1;
			(File_Tbl.pack.dir_index)--;
		}
		else
		{	
			gBF_Ptp_Buff->ObjectSearchTbl[(File_Tbl.pack.dir_index) - 1].usObjPos = objCnt;
			gBF_Ptp_Buff->ObjectSearchTbl[(File_Tbl.pack.dir_index) - 1].usObjCnt = File_Tbl.pack.obj_index;
			while(File_Tbl.pack.obj_index)
			{
				objCnt++;
				file_index = File_Tbl.index;
				Err = BF_Dcf_If_Get_File_Info(file_index, &fsfileinfo);
				if(Err == FJ_ERR_OK)
				{
					filttype = BF_Dcf_If_Convert_File_Type(fsfileinfo.file_type);
					if((filttype == E_BF_DCF_IF_FILE_TYPE_JPG) || (filttype == E_BF_DCF_IF_FILE_TYPE_AVI))//JPG or AVI ONLY
					{
						Object[(gBF_Ptp_Buff->ObjectSearchTbl[(File_Tbl.pack.dir_index) - 1].usObjPos)+(File_Tbl.pack.obj_index) - 1].DcfHandle = file_index;
						jpgCnt++;
					}
				}
				(File_Tbl.pack.obj_index)--;
			}
			(File_Tbl.pack.dir_index)--;
		}
	}
	*JpgCount = jpgCnt;
#endif
	return jpgCnt;
}

INT32 PTP_GetDpofObjectInfo(E_BF_MEDIA_STATE_MEDIA_TYPE mediaType)
{	 
	INT32 err ;
	CHAR szDirPath[32];
	
	memset(szDirPath, 0, 32);
	err = BF_Media_State_Exchage_Type_To_Drive_Name(mediaType, szDirPath);
	if (err != 0){
		BF_Debug_Print_Error(("exchange type to drive name error!\n"));
		return err;
	}
	
	strcpy(MiscPath, szDirPath);
	strcpy(MiscPath + strlen(szDirPath),":\\MISC\\AUTPRINT.MRK");

//	switch(mediaType)
//	{
//		case E_BF_MEDIA_STATE_MEDIA_TYPE_SD:
//		case E_BF_MEDIA_STATE_MEDIA_TYPE_SD1:
//			strcpy(MiscPath, SD_DRIVE_TAG);
//			strcpy(MiscPath + strlen(NF_DRIVE_TAG),"MISC\\AUTPRINT.MRK");
//			
//			break;
//		default:
//			strcpy(MiscPath, NF_DRIVE_TAG);
//			strcpy(MiscPath + strlen(SD_DRIVE_TAG),"MISC\\AUTPRINT.MRK");
//			break;
//	}
	
	err = BF_Fs_If_Open(MiscPath, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
	if(err != 0)
	{			
		gBF_Ptp_Buff->Object_DPOF.DcfHandle = FIL_DPOF;
		BF_Fs_If_Close(err);
	}
	return err;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SHORT PTP_FindObjectHandle()
//	Action:
//	1.	Return the index of the Object in the Object_Handle_Info array.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ULONG PTP_FindObjectIndex(E_BF_MEDIA_STATE_MEDIA_TYPE mediaType, ULONG Handle)
{
/*
	if(Handle == 0)
	{
		return 0xFFFF;
	}
	if(Object[Handle - 1].DcfHandle == 0)
	{
		return 0xFFFF;
	}
	return ((SHORT)(Object[Handle].DcfHandle));
*/
	return Handle;
}

LONG BULK_Read( UCHAR *buf, ULONG Size)
{
	ULONG ulTotalSize = 0;
	LONG Readsize = 0;
	UCHAR* TempBuf = buf;
	GCS_T  *CmdDS=(GCS_T *)buf;
	UINT32 tmp, Code32;
	MWUSB_SetMode(0);	//usb normal mode
	Readsize = MWUSB_Read1Packet(TempBuf);
	TempBuf += Readsize;
	Code32 = (*CmdDS).ContainLen;
    ulTotalSize = ENDIAN_SWAP32(Code32);

	LONG ret;
	
#if 1	//20120818
	if(MWUSB_GetSpeed() == MWUSB_HISPEED){
	
		if( (ulTotalSize-Readsize) > 0 )
		{
			ret = MWUSB_Read(TempBuf, (ulTotalSize-Readsize), BULK_READ_FIFO);
			if(ret < 0)
			{
				printf("# MWUSB_Read error\n");
			}
		}
	}
	else
	{
		for(;Readsize < ulTotalSize;)
		{
			tmp = ulTotalSize - Readsize;
			if(tmp>=64)
			{
				Readsize += MWUSB_Read(TempBuf, 64, BULK_READ_FIFO);
				TempBuf += 64;
			}
			else
			{
				Readsize += MWUSB_Read(TempBuf, tmp, BULK_READ_FIFO);
				TempBuf += tmp;
			}
		}
	}
#else
	for(;Readsize < ulTotalSize;){
		tmp = ulTotalSize - Readsize;
		if(tmp>=64)
		{
			Readsize += MWUSB_Read(TempBuf, 64, BULK_READ_FIFO); 
			TempBuf += 64;
		} else { 
			Readsize += MWUSB_Read(TempBuf, tmp, BULK_READ_FIFO);
			TempBuf += tmp;
		}
	}	
#endif
	
    return ulTotalSize;
}
// 04/30 STX tony +
// first 64kb data  for read large data (read > 64k data) 
LONG BULK_Read2( UCHAR *buf, ULONG Size)
{
	ULONG	ulTotalSize	= 0;
	LONG	Readsize	= 0;
	UCHAR*	TempBuf		= buf;
	GCS_T	*CmdDS		= (GCS_T *)buf;
	UINT32	tmp, Code32;
	SHORT	sMaxPaketSize;

	MWUSB_SetMode( 0 );	//usb normal mode
	Readsize = MWUSB_Read1Packet( TempBuf );

	TempBuf		+= Readsize;
	Code32		= (*CmdDS).ContainLen;
	ulTotalSize = ENDIAN_SWAP32(Code32);

	sMaxPaketSize = MWUSB_GetMaxPacketSize( BULK_READ_FIFO );

	if (ulTotalSize > sMaxPaketSize) {
		ulTotalSize = sMaxPaketSize;
	}

	for ( ; Readsize < ulTotalSize ; ) {

		tmp = ulTotalSize - Readsize;

		if ( tmp >= sMaxPaketSize ) {
			Readsize += MWUSB_Read( TempBuf, sMaxPaketSize, BULK_READ_FIFO ); 
			TempBuf  += sMaxPaketSize;
		}
		else { 
			Readsize += MWUSB_Read( TempBuf, tmp, BULK_READ_FIFO );
			TempBuf += tmp;
		}
	}

	ulTotalSize = ENDIAN_SWAP32( Code32 );

	return ulTotalSize;
}

// first other data  for read large data (read > 64k data) 
LONG BULK_Read3( UCHAR *buf, ULONG Size)
{
	ULONG	ulTotalSize	= 0;
	LONG	Readsize	= 0;
	UCHAR*	TempBuf		= buf;
	UINT32	tmp;

	MWUSB_SetMode( 0 );	//usb normal mode

//	Readsize = MWUSB_Read1Packet( TempBuf );
//	TempBuf  += Readsize;

	ulTotalSize		= Size;

	for ( ; Readsize < ulTotalSize; ) {

		tmp = ulTotalSize - Readsize;

		if ( tmp >= Size ) {
			Readsize += MWUSB_Read( TempBuf, Size, BULK_READ_FIFO ); 
			TempBuf  += Size;
		}
		else {
			Readsize += MWUSB_Read( TempBuf, tmp, BULK_READ_FIFO );
			TempBuf  += tmp;
		}
	}

	return ulTotalSize;
}

// case-insensitive
INT32 strcmpi( const CHAR* a, const CHAR* b)
{
	CHAR *src, *dst;
	CHAR src_temp = 0, dst_temp = 0;

	src = (CHAR*)b;
	dst = (CHAR*)a;

	for(;*src != '\0'; src++, dst++) {
		src_temp=*src;
		dst_temp=*dst;

		if (src_temp>='a' && src_temp<='z')
			src_temp=src_temp - 0x20;
		if (dst_temp>='a' && dst_temp<='z')
			dst_temp=dst_temp - 0x20;
		if(src_temp != dst_temp) {
			return(src_temp - dst_temp);
		}
	}

	return(src_temp - dst_temp);
}


INT32 str2num(CHAR* src, INT32 n)  // Positive Intergers Only
{
	INT32 i=0;
	INT32 j;
	INT32 exponent=1;
	INT32 sum=0;
	
	while ((i<n) && (*(UCHAR *)src)>=0x30 && (*(UCHAR *)src)<=0x39/*isdigit(src)*/) {
		for (j=1; j<(n-i); j++)
			exponent=exponent*10;
		sum = sum + (*src-0x30)*exponent;
		exponent=1;
		src++;
		i++;
	}
	if (i==0)
		return -1;
	else
		return sum;
}


INT32 IN_BUFFER_Write( UCHAR *buf_ptr, UINT32 buf_len )
{
	UINT32 size = 0, rest_size;
	MWUSB_SetMode(0);
	rest_size = buf_len;
	size      = CalcSendAndRest( 65536 - 12, &rest_size );
	memcpy((IN_Buffers+12), buf_ptr, size );
	SendDataDetail2( buf_len, size );
	buf_ptr += size;

	while ( rest_size )
	{
		size = CalcSendAndRest( 65536, &rest_size );
		memcpy((IN_Buffers+0), buf_ptr, size );
	MWUSB_SetMode(0);
		MWUSB_Write( (UCHAR*) IN_Buffers, size, BULK_WRITE_FIFO );
		buf_ptr += size;
	}

	//tony ho : Send Null Packet if necessary
	if ((buf_len+12) % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)== 0){	//tonyho:need to removed the constant value
		OS_User_Tslp_Tsk(300);
		MWUSB_SendNullPacket(BULK_WRITE_FIFO);
	}
	
	return TRUE;
}

INT32 SetPTPModeString(USHORT wParmID, UCHAR* bBuffer, UINT32 dwSize )
{
	INT32 nReturn = 0, i = 0, j = 0;
	switch(wParmID)
	{
		case PRMID_PTP_MODEL_STRING:
			if(dwSize >= PTP_MODEL_STRING_LEN)
			{
				nReturn = 1;
			}
			else
			{
				memset(DeviceInfo.Model_Data, 0, PTP_MODEL_STRING_LEN * 2);
				while(i < ((PTP_MODEL_STRING_LEN - 1) * 2))
				{
					
					DeviceInfo.Model_Data[i] = (j >= dwSize)?0x20:bBuffer[j++];
					DeviceInfo.Model_Data[i + 1] = 0x00;
					i += 2;
				}

				i=0;
				j=0;
				memset(MTPDeviceInfo.Model_Data, 0, PTP_MODEL_STRING_LEN * 2);
				while(i < ((PTP_MODEL_STRING_LEN - 1) * 2))
				{
					
					MTPDeviceInfo.Model_Data[i] = (j >= dwSize)?0x20:bBuffer[j++];
					MTPDeviceInfo.Model_Data[i + 1] = 0x00;
					i += 2;
				}					
			}
			break;
		case PRMID_PTP_SERIAL_NUMBER:
			if(dwSize >= PTP_SERIAL_NUMBER_LEN)
			{
				nReturn = 1;
			}
			else
			{
			/*
				memset(DeviceInfo.SerialNumber_Data, 0, PRMID_PTP_SERIAL_NUMBER * 2);
				while(i < ((PTP_SERIAL_NUMBER_LEN - 1) * 2))
				{
					DeviceInfo.SerialNumber_Data[i] = (j >= dwSize)?0x20:bBuffer[j++];
					DeviceInfo.SerialNumber_Data[i + 1] = 0x00;
					i += 2;
				}
			*/
			}
			break;
		case PRMID_DPS_VENDOR_NAME_STRING:
			if(dwSize >= PTP_VENDOR_NAME_STRING_LEN)
			{
				nReturn = 1;
			}
			else
			{
				memset(DeviceInfo.Manufacturer_Data, 0, PTP_VENDOR_NAME_STRING_LEN * 2);
				while(i < ((PTP_VENDOR_NAME_STRING_LEN - 1) * 2))
				{
					DeviceInfo.Manufacturer_Data[i] = (j >= dwSize)?0x20:bBuffer[j++];
					DeviceInfo.Manufacturer_Data[i + 1] = 0x00;
					i += 2;
				}
				i=0;
				j=0;
				memset(MTPDeviceInfo.Manufacturer_Data, 0, PTP_VENDOR_NAME_STRING_LEN * 2);
				while(i < ((PTP_VENDOR_NAME_STRING_LEN - 1) * 2))
				{
					MTPDeviceInfo.Manufacturer_Data[i] = (j >= dwSize)?0x20:bBuffer[j++];
					MTPDeviceInfo.Manufacturer_Data[i + 1] = 0x00;
					i += 2;
				}				
			}
			break;
	}
	return nReturn;

}
