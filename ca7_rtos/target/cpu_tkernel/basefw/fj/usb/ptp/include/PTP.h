/**
 * @file		PTP.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef _PTP_
#define _PTP_
#include "Global.h"
#include "fj_custom.h"

//#define PTP_VENDOR_CMD_EN

#define PTP_StorageID_NF		0x00010001
#define PTP_StorageID_SD		0x00020001
#define PTP_StorageID_MS		0x00030001
#define PTP_StorageID_EM		0x00040001
#define PTP_StorageID_FW		0x40000001

//Andy modified for buffering jpeg file for PTP transfer
//#define PTP_JPEG_BUFFER			0x41600000
//jeff ended
/*----------- Operation Request Code ---------------*/
#define PTP_OP_GetDeviceInfo    0x1001
#define PTP_OP_OpenSession      0x1002
#define PTP_OP_CloseSession     0x1003
#define PTP_OP_GetStorageIDs    0x1004
#define PTP_OP_GetStorageInfo   0x1005
#define PTP_OP_GetNumObjects    0x1006
#define PTP_OP_Getobjecthandles 0x1007
#define PTP_OP_GetObjectInfo    0x1008
#define PTP_OP_GetObject        0x1009
#define PTP_OP_GetThumb         0x100A
#define PTP_OP_SendObjectInfo   0x100C
#define PTP_OP_SendObject       0x100D
#define PTP_OP_FormatStore       0x100F
#define PTP_OP_GetDevicePropDesc       0x1014
#define PTP_OP_GetDevicePropValue       0x1015
#define PTP_OP_SetDevicePropValue       0x1016
#define PTP_OP_GetPartialObject 0x101B

#define PTP_OP_GetFWVersion	0x9006
#define PTP_OP_UpdateFirmwareAll 0x9116
#define PTP_OP_SendObjectSplit	0x9119
#define PTP_OP_GetProgressValue	0x911A
/*----------- Response Code ---------------*/
//#define PTP_RS_Undefined                             0x2000 //
#define PTP_RS_OK                                    0x2001 //[USE]+add
#define PTP_RS_GeneralError                          0x2002 //
#define PTP_RS_SessionNotOpen                        0x2003 //had+add
#define PTP_RS_InvalidTransactionID                  0x2004 //had+[....]
#define PTP_RS_OperationNotSupported                 0x2005 //InSwitchOP+add
#define PTP_RS_ParameterNotSupported                 0x2006 //had+add
#define PTP_RS_IncompleteTransfer                    0x2007 //(Need on Store Removed event happen)
#define PTP_RS_InvalidStorageID                      0x2008 //had+add
#define PTP_RS_InvalidObjectHandle                   0x2009 //had+add
//#define PTP_RS_DevicePropNotSupported                0x200A //
#define PTP_RS_InvalidObjectFormatCode               0x200B //had+add(PS:need 11 in add) 
#define PTP_RS_StoreFull                             0x200C //
#define PTP_RS_ObjectWriteProtected                  0x200D //
#define PTP_RS_StoreReadOnly                         0x200E //had+add
#define PTP_RS_AccessDenied                          0x200F //?????(5/11)<Not Busy,is Can't read write>
#define PTP_RS_NoThumbnailPresent                    0x2010 //?????(?)
//#define PTP_RS_SelfTestFailed                        0x2011 //
//#define PTP_RS_PartialDeletion                       0x2012 //
#define PTP_RS_StoreNotAvailable                     0x2013 //(5~~13)<Not Busy,is Eject>
#define PTP_RS_SpecificationByFormatUnsupported      0x2014 //had+add
#define PTP_RS_NoValidObjectInfo                     0x2015 //had+add
#define PTP_RS_InvalidCodeFormat                     0x2016 //????(6/7)
//#define PTP_RS_UnknownVendorCode                     0x2017 //
//#define PTP_RS_CaptureAlreadyTerminated              0x2018 //
#define PTP_RS_DeviceBusy                            0x2019 //(2/12/13)[USE]
#define PTP_RS_InvalidParentObject                   0x201A //had+add
//#define PTP_RS_InvalidDevicePropFormat               0x201B //
//#define PTP_RS_InvalidDevicePropValue                0x201C //
#define PTP_RS_InvalidParameter                      0x201D //
#define PTP_RS_SessionAlreadyOpen                    0x201E //
//#define PTP_RS_TransactionCancelled                  0x201F //[USE]
//#define PTP_RS_SpecificationOfDestinationUnsupported 0x2020 //
/*------------ Event Code ---------------*/

//#define PTP_EV_Undefined                0x4000
//#define PTP_EV_CancelTransaction        0x4001	//(C350).		COOL.
#define PTP_EV_ObjectAdded              0x4002	//SPEC.
#define PTP_EV_ObjectRemoved            0x4003	//SPEC.
#define PTP_EV_StoreAdded               0x4004	//SPEC.(C350).		COOL.
#define PTP_EV_StoreRemoved             0x4005	//SPEC.(C350).		COOL.
//#define PTP_EV_DevicePropChanged        0x4006
#define PTP_EV_ObjectInfoChanged        0x4007	//SPEC.(C350).
//#define PTP_EV_DeviceInfoChanged        0x4008	//(C350).
#define PTP_EV_RequestObjectTransfer    0x4009	//SPEC.(C350).
//#define PTP_EV_StoreFull                0x400A	//(C350).
//#define PTP_EV_DeviceReset              0x400B
//#define PTP_EV_StorageInfoChanged       0x400C
//#define PTP_EV_CaptureComplete          0x400D
//#define PTP_EV_UnreportedStatus         0x400E
/*------------- Object format code --------------*/
#define	PTP_OFC_None		0x0000
#define PTP_OFC_AllPicture	0xFFFF
#define PTP_OFC_Directory	0x3001
#define PTP_OFC_Script		0x3002
#define PTP_OFC_DPOF		0x3006
#define PTP_OFC_WAV			0x3008
#define PTP_OFC_EXIFJPEG	0x3801
#define	PTP_OFC_JPEG		0x3808	//apply from James(NOT IN CUSTOM SPEC)
#define PTP_OFC_MOV			0xB980
/*------------- DevicePropCode --------------*/
#define PTP_DPC_ImageSize				0x5003
#define PTP_DPC_CompressionSetting		0x5004
#define PTP_DPC_FocalLength				0x5008
#define PTP_DPC_FlashMode				0x500C
#define PTP_DPC_IsoMode				0x501E
/*------------- Handle Define -------------------*/
#define HANDLE_FILE_DDISCVRY	0x0000FFF1
#define HANDLE_FILE_HDISCVRY	0x0000FFF2
#define HANDLE_DIR_DCIM			0xFFF10000
#define HANDLE_DIR_MISC			0xFFF20000
#define HANDLE_FILE_AUTO		0xFFF20001

#define HANDLE_FILE_DREQUEST	0x0000FF11
#define HANDLE_FILE_DRSPONSE	0x0000FF12
#define HANDLE_FILE_HREQUEST	0x0000FF21
#define HANDLE_FILE_HRSPONSE	0x0000FF22
/*------------ ProtectStatus -------------------*/
#define PROTECT_NoProtect		0x0000
#define PROTECT_ReadOnly		0x0001

#define	VIRTUAL_HANDLE_MAX		6
#define htonl(x) ((((x) & 0xff000000) >> 24) | \
                  (((x) & 0x00ff0000) >> 8) | \
                  (((x) & 0x0000ff00) << 8) | \
                  (((x) & 0x000000ff) << 24))
/*-------------PTP mode string setting ID-------*/
#define PRMID_PTP_MODEL_STRING       0
#define PRMID_PTP_SERIAL_NUMBER      1
#define	PRMID_DPS_VENDOR_NAME_STRING 2
/*-------------PTP mode string length    -------*/
//#define PTP_MODEL_STRING_LEN         0x1A
#define PTP_MODEL_STRING_LEN         FJ_CUSTOM_USB_PTP_MODEL_LEN
#define PTP_SERIAL_NUMBER_LEN        0x0D
//#define	PTP_VENDOR_NAME_STRING_LEN   0x11
#define	PTP_VENDOR_NAME_STRING_LEN   FJ_CUSTOM_USB_PTP_MANUFACTURER_LEN

// FID_PTPTSK
#define FLG_PTP_TERMINATE (0x00000001)
// FID_MTPTSK
#define FLG_MTP_READWRITE (0x00000001)
#define FLG_MTP_TERMINATE (0x00000008)


/*------------ SICD Generic Container Structure -----------*/
//   for Operation/Response/Event
typedef struct {
        UCHAR ContainLen[4];    // Container Size
        UCHAR ContainType[2];   // 1:Command 2:Data 3:Response 4:Event
        UCHAR Code[2];          // PIMA 15740 Operation/Event Code
        UCHAR TransactionID[4];
        UCHAR PayLoad[4*5];     // Max Use for Parameter.
}GCS_B;//for Byte

typedef struct {
        UINT32    ContainLen;  
        USHORT  ContainType;
        USHORT  Code;
        UINT32    TransactionID; 
        UINT32    parm1;
        UINT32    parm2;
        UINT32    parm3;
        UINT32    parm4;
        UINT32    parm5;
}GCS_T;//for Type
/*-----------------------------*/
//typedef struct {
//       UINT32    Num;
//        UINT32    Data[2];//paul MD1
//}STORAGE_IDS;
typedef struct {
        UINT32    Num;
        UINT32    Data;
}STORAGE_IDS;
typedef struct {
        UINT32    Num;
        UINT32    Data[1];
}OBJECTHANDLE_IDS;
//len:217
typedef struct {
        UCHAR   StandardVersion[2];
        UCHAR   VendorExtensionID[4];
        UCHAR   VendorExtensionVersion[2];
        UCHAR   VendorExtensionDesc_Len;
//        UCHAR   VendorExtensionDesc_Data[23*2];
        
//        UCHAR   VendorExtensionDesc_Data[22*2];			//paul 20060406 mark out // howard
        
        
        UCHAR   FunctionMode[2];

        UCHAR   OperationsSupported_Num[4];
        UCHAR   OperationsSupported_Data[14*2];//paul 20060417 16 <- 13
        UCHAR   EventsSupported_Num[4];
        UCHAR   EventsSupported_Data[1*2];
        UCHAR   DevicePropertiesSupported_Num[4];
        //UCHAR   DevicePropertiesSupported_Data[4*2];
        UCHAR   CaptureFormats_Num[4];
        //UCHAR  CaptureFormats_Data[?*2];
        UCHAR   ImageFormats_Num[4];
        //UCHAR   ImageFormats_Data[3*2];
		UCHAR   ImageFormats_Data[11*2];//paul 20060417 11*2 <-2*2

        UCHAR   Manufacturer_Len;
        UCHAR   Manufacturer_Data[PTP_VENDOR_NAME_STRING_LEN*2];
        UCHAR   Model_Len;
        UCHAR   Model_Data[PTP_MODEL_STRING_LEN*2];
        UCHAR   DeviceVersion_Len;
        //UCHAR   DeviceVersion_Data[0x04*2];
        UCHAR   SerialNumber_Len;
        //UCHAR   SerialNumber_Data[PTP_SERIAL_NUMBER_LEN*2];
        UCHAR	fill[1];
}DEVICE_INFO;

//************************ F/W Update **********************
//len:145
typedef struct {
        UCHAR   StandardVersion[2];
        UCHAR   VendorExtensionID[4];
        UCHAR   VendorExtensionVersion[2];
        UCHAR   VendorExtensionDesc_Len;
        //UCHAR   VendorExtensionDesc_Data[22*2];
        UCHAR   FunctionMode[2];

        UCHAR   OperationsSupported_Num[4];
        UCHAR   OperationsSupported_Data[17*2];//////////////////////
        UCHAR   EventsSupported_Num[4];
        //UCHAR   EventsSupported_Data[6*2];
        UCHAR   DevicePropertiesSupported_Num[4];
        //UCHAR  DevicePropertiesSupported_Data[?*2];
        UCHAR   CaptureFormats_Num[4];
        UCHAR   CaptureFormats_Data[3*2];
        UCHAR   ImageFormats_Num[4];
        UCHAR   ImageFormats_Data[3*2];

        UCHAR   Manufacturer_Len;
        UCHAR   Manufacturer_Data[8*2];
        UCHAR   Model_Len;
        UCHAR   Model_Data[10*2];
        UCHAR   DeviceVersion_Len;
        UCHAR   DeviceVersion_Data[5*2];
        UCHAR   SerialNumber_Len;
        UCHAR   SerialNumber_Data[13*2];
        UCHAR	fill[3];
        
}DEVICE_INFO_FW_UPDATE;

/*-------------- StorageInfo Dataset ---------------*/
typedef struct {
        UCHAR  StorageType[2];        //1:Fixed ROM 2:Removable ROM 3:Fixed RAM 4:Removable RAM
        UCHAR  FilesystemType[2];     //1:Generic Flat 2:Generic Hierarchical 3:DCF
        UCHAR  AccessCapability[2];   //1:Read-Only whthout Object Deletion
                                    //2:Read-Only with Object Deletion
        UCHAR   MaxCapacity[8];
        UCHAR   FreeSpaceInBytes[8];
        UCHAR   FreeSpaceImages[4];
        UCHAR   StorageDescription_Len;
//        UCHAR   StorageDescription_Data[26];
        UCHAR   VolumnLabel_Len;
        //UCHAR   VolumnLabel_Data[];
        UCHAR	fill[1];
}STORAGE_INFO;
/*-------------- ObjectInfo Dataset ----------------*/
typedef struct {
        UCHAR   StorageID[4];
        UCHAR   ObjectFormat[2];
        UCHAR   ProtectionStatus[2];//0:No Protection 1:Read-Only
        UCHAR   ObjectCompressSize[4];
        UCHAR   ThumbFormat[2];
        UCHAR   ThumbCompressSize[4];
        UCHAR   ThumbPixWidth[4];
        UCHAR   ThumbPixHeight[4];
        UCHAR   ImagePixWidth[4];
        UCHAR   ImagePixHeight[4];
        UCHAR   ImageBitDepth[4];
        UCHAR   ParentObject[4];
        UCHAR   AssociationType[2];
        UCHAR   AssociationDesc[4];
        UCHAR   SequenceNumber[4];
        
      	UCHAR    FCMK[200];   //=>FileName/CaptureDate/ModificationDate/Keywords
}OBJECT_INFO;
/*-------------- DeviceProp_ImageSize Dataset ----------------*/
typedef struct {//paul add for Altek - Mars Vista Command
        UCHAR   DevicePropCode[2];
        UCHAR   DataType[2];
        UCHAR   GetSet[1];//0x00:Get 0x01:Get/Set
        UCHAR   FactoryDefaultValue[1];
        UCHAR   CurrentValue[1];
        UCHAR   FormFlag[1];
        UCHAR   NumberOfValue[2];
        UCHAR   SupportedValue1[1];
        UCHAR   SupportedValue2[1];
        UCHAR   SupportedValue3[1];
        UCHAR   SupportedValue4[1];
        UCHAR   SupportedValue5[1];
        UCHAR   SupportedValue6[1];
 }DeviceProp_ImageSize;
/*-------------- DeviceProp_CompressionSetting Dataset ----------------*/
typedef struct {//paul add for Altek - Mars Vista Command
        UCHAR   DevicePropCode[2];
        UCHAR   DataType[2];
        UCHAR   GetSet[1];//0x00:Get 0x01:Get/Set
        UCHAR   FactoryDefaultValue[1];
        UCHAR   CurrentValue[1];
        UCHAR   FormFlag[1];
        UCHAR   NumberOfValue[2];
        UCHAR   SupportedValue1[1];
        UCHAR   SupportedValue2[1];
        UCHAR   SupportedValue3[1];
         UCHAR   SupportedValue4[1];
        UCHAR   SupportedValue5[1];
        UCHAR   SupportedValue6[1];
        UCHAR   SupportedValue7[1];
        UCHAR   SupportedValue8[1];
        UCHAR   SupportedValue9[1];
        UCHAR   SupportedValue10[1];
        UCHAR   SupportedValue11[1];
        UCHAR   SupportedValue12[1];
        UCHAR   SupportedValue13[1];
        UCHAR   SupportedValue14[1];
        UCHAR   SupportedValue15[1];
        UCHAR   SupportedValue16[1];
        UCHAR   SupportedValue17[1];
        UCHAR   SupportedValue18[1];
}DeviceProp_CompressionSetting;
/*-------------- DeviceProp_FocalLength Dataset ----------------*/
typedef struct {//paul add for Altek - Mars Vista Command
        UCHAR   DevicePropCode[2];
        UCHAR   DataType[2];
        UCHAR   GetSet[1];//0x00:Get 0x01:Get/Set
        UCHAR   FactoryDefaultValue[4];
        UCHAR   CurrentValue[4];
        UCHAR   FormFlag[1];
        UCHAR   NumberOfValue[17];
        UCHAR   SupportedValue1[4];
        UCHAR   SupportedValue2[4];
        UCHAR   SupportedValue3[4];
        UCHAR   SupportedValue4[4];
        UCHAR   SupportedValue5[4];
        UCHAR   SupportedValue6[4];
        UCHAR   SupportedValue7[4];
         UCHAR   SupportedValue8[4];
        UCHAR   SupportedValue9[4];
        UCHAR   SupportedValue10[4];
        UCHAR   SupportedValue11[4];
        UCHAR   SupportedValue12[4];
        UCHAR   SupportedValue13[4];
        UCHAR   SupportedValue14[4];
        UCHAR   SupportedValue15[4];
        UCHAR   SupportedValue16[4];
        UCHAR   SupportedValue17[4];
}DeviceProp_FocalLength;
/*-------------- DeviceProp_FlashMode Dataset ----------------*/
typedef struct {//paul add for Altek - Mars Vista Command
        UCHAR   DevicePropCode[2];
        UCHAR   DataType[2];
        UCHAR   GetSet[1];//0x00:Get 0x01:Get/Set
        UCHAR   FactoryDefaultValue[2];
        UCHAR   CurrentValue[2];
        UCHAR   FormFlag[1];
        UCHAR   NumberOfValue[2];
        UCHAR   SupportedValue1[2];
        UCHAR   SupportedValue2[2];
        UCHAR   SupportedValue3[2];
        UCHAR   SupportedValue4[2];
 }DeviceProp_FlashMode;
/*-------------- DeviceProp_IsoMode Dataset ----------------*/
typedef struct {//paul add for Altek - Mars Vista Command
        UCHAR   DevicePropCode[2];
        UCHAR   DataType[2];
        UCHAR   GetSet[1];//0x00:Get 0x01:Get/Set
        UCHAR   FactoryDefaultValue[1];
        UCHAR   CurrentValue[1];
        UCHAR   FormFlag[1];
        UCHAR   NumberOfValue[2];
        UCHAR   SupportedValue1[1];
        UCHAR   SupportedValue2[1];
        UCHAR   SupportedValue3[1];
        UCHAR   SupportedValue4[1];
}DeviceProp_IsoMode;
/*------------ class command struct ----------*/
typedef struct{
	UCHAR	Value[4];
}PARAM;
typedef struct{
	USHORT	wLength;
	USHORT	Code;	
	PARAM	Params[10];
}DEVICE_STATUS;
//
typedef struct{
	USHORT	EventCode;
	UCHAR	TransactionID[4];
	UCHAR	SizeOfParam[2];
}DEVICE_EVENT;
/*------------ Virtual Storage Handles -------*/
typedef struct{
		UCHAR	flg;
		UINT32	hdl;
		CHAR	fileName[14];
}RAM_FILE_FCB;

typedef struct{
		RAM_FILE_FCB	RFCs[VIRTUAL_HANDLE_MAX];
}RAM_FILE_MNG;		

typedef struct{
	USHORT   usX;
	USHORT   usY;
	USHORT   usW;
	USHORT   usH;
	USHORT   usCopies;
	UCHAR    ucPrintOption;
}ImagePrintInfo; // Andy Jing

typedef struct{
	//UCHAR	SrcPath[64];
	//UCHAR   Date[14]; //YYYYMMDD
	//ULONG	Handle;	
	ULONG	DcfHandle;	
	//ULONG	ParentHandle;
	//USHORT	ObjectFormatCode;//
	//UCHAR	Attribute;//File Attribute: 0x10->Dir, 0x20->File
	ImagePrintInfo  PrintInfo;
} OBJECT_HANDLE_INFO;

typedef struct{
	SHORT  usObjPos;
	SHORT  usObjCnt;
} OBJECT_SEARCH_TBL;

#define NUM_OBJECT_NF	100
#define NUM_OBJECT_SD	FJ_CUSTOM_DCFDB_MAX
#define SD_DRIVE_TAG       ("I:\\")
#define NF_DRIVE_TAG       ("J:\\")
#define PTP_ObjectHandle_NF_Offset	1000//paul MD: 1001, 1002. ..., 1100
#define PTP_ObjectHandle_SD_Offset	0//paul MD: 1, 2, ..., 1000
//paul PTP end

#define MTP_DATA_MNG_INFO_NUM	4

// Definition for RAM sharing
typedef struct{
	UCHAR	Buffers1[2*1024];									// Send
	UCHAR	Buffers2[512*1024*MTP_DATA_MNG_INFO_NUM];			// Receive
	UCHAR	Buffers3[12+3*4];									// Interrupt
	UCHAR	Buffers4[64];										// Send:Response
	UCHAR	PTP_JPEG_BUFFER[512*1024*MTP_DATA_MNG_INFO_NUM];	// Image Bulk In by Andy Jing
	OBJECT_HANDLE_INFO Object_SD[NUM_OBJECT_SD];
	OBJECT_HANDLE_INFO Object_DPOF;								// paul MD
	OBJECT_SEARCH_TBL  ObjectSearchTbl[900];					// 100 ~ 999
} T_BF_PTP_BUFF;

VOID	usb_ptp_cls_tsk( VOID );
OS_USER_ER		callPTPbyMass( VOID );
ULONG	IsGetFWByPTP(VOID);
VOID	setFWGetSize(ULONG	v);

UINT32 CheckMediaFormatType( VOID );
#define PTP_CHECK_MEDIA_OK	0
#define PTP_CHECK_MEDIA_NG	1

/*--------------------------------------
			for DPS
--------------------------------------*/
VOID SendDSPCmdFile( VOID );
VOID DSP_Discovery( VOID );
///////////////////////////////////////////////////////////////////
VOID SendResponse(UINT32 ParmCnt,UINT32 Code,
                 UINT32 parm1,UINT32 parm2,UINT32 parm3,UINT32 parm4,UINT32 parm5);
VOID SendInterrupt(UINT32 ParmCnt,UINT32 Code,
                    UINT32 parm1,UINT32 parm2,UINT32 parm3);
UCHAR USBPtpCmd_GetDeviceInfo(VOID);//1
UCHAR USBPtpCmd_OpenSession(VOID);//2
UCHAR USBPtpCmd_CloseSession(VOID);//3
UCHAR USBPtpCmd_GetStorageIDs(VOID);//4
UCHAR USBPtpCmd_GetStorageInfo(VOID);//5
UCHAR USBPtpCmd_GetNumObjects(VOID);//6
UCHAR USBPtpCmd_GetObjecthandles(VOID);//7
UCHAR USBPtpCmd_GetObjectInfo(VOID);//8
UCHAR USBPtpCmd_GetObject(VOID);//9
UCHAR USBPtpCmd_GetThumb(VOID);//10
UCHAR USBPtpCmd_SendObjectInfo(VOID);//11
UCHAR USBPtpCmd_SendObject(VOID);//12
UCHAR USBPtpCmd_FormatStore(VOID);//paul add
UCHAR USBPtpCmd_GetDevicePropDesc(VOID);//paul add
UCHAR USBPtpCmd_SetDevicePropValue(VOID);//paul add
UCHAR USBPtpCmd_GetPartialObject(VOID);//13
UINT32 ScriptFile_Handles(UINT32 *Addr);
OS_USER_ER USBPtpProcessObjectOpen(USHORT ProcKind,UINT32 objhdl,UINT32 parm1,UINT32 parm2);
OS_USER_ER USBPtpCmd_Check_ParameterNotSupported(UCHAR parmCnt,UCHAR MaxCnt,UINT32 *ptr);
OS_USER_ER USBPtpCmd_Check_SpecificationByFormatUnsupported(ULONG objfmt);
OS_USER_ER USBPtpCmd_Check_InvalidObjectFormatCode(ULONG objfmt);
OS_USER_ER USBPtpProcessRecvObj(USHORT ProcKind,UINT32 StrgID,UINT32 ParentID);
VOID	WriteInt(UCHAR *Addr,UINT32 value);
UCHAR USBPtpCmd_SendObjectSplit( VOID );
VOID USBPtpCmdUnknown ( VOID );
//INT32 PTP_GetObjectHandle(E_BF_MEDIA_STATE_MEDIA_TYPE mediaType, USHORT ObjectFormatCode, UINT32 *ptr);
INT32 PTP_GetObjectHandle(USHORT ObjectFormatCode, UINT32 *ptr);
ULONG PTP_FindObjectIndex(E_BF_MEDIA_STATE_MEDIA_TYPE mediaType, ULONG Handle);
INT32 SetPTPModeString(USHORT wParmID, UCHAR* bBuffer, UINT32 dwSize );
INT32 ResetAllObjectInfo(VOID);
VOID ResetDPS(VOID);
extern INT32 str2num(CHAR* src, INT32 n);  // Positive Intergers Only
extern VOID SendEvent(UINT32 Code,UINT32 objhdl);
extern VOID InitialPTP( VOID );
#endif

