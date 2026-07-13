/**
 * @file		mtp.c
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
#ifdef CO_DCF_EXIF_ON
#include "dcf_if.h"
#include "exif_controller.h"
#endif
#include "mwusb_driver4.h"
#include "PTP.h"
#include "DPS.h"
#include "fs_if.h"
#include "control_def.h"
#include "MessageQueue.h"
#include "CalibrationFunction.H"
#include "ChipTest.H"
#include "fj_pictbridge.h"
#include "mtp.h"
#include "fj_filesystem.h"

#include "avilib.h"
#include "wavlib.h"
#include "jfiflib.h"
#include "mp3lib.h"
#include "bmplib.h"
#include "giflib.h"
#include "tifflib.h"
#include "movlib.h"
#include "sdram_map_play.h"
#include "debug.h"
#include "fj_custom.h"
#include "fj_media.h"
#include "mdf_wrapper.h"

#define DEVICEPROP_EXTENSION
#define MTP_FOLDER_SUPPORT

#define SDRAM_AND					(0x4FFFFFFF)
#define DCIM_DIR_NAME				FJ_CUSTOM_DCF_ROOT_DIR_NAME
#define DCF_DIR_NAME				FJ_CUSTOM_DCF_DIR_NAME

//char dcim_dir_name[8]	= DCIM_DIR_NAME;
//char dcf_dir_name[6]	= DCF_DIR_NAME;

#define MTP_OBJECT_NUMBER			10000
#define NUM_DIR_LEVEL				3
#define DCF_DIR_NAME_LENGTH			8
#define MTP_DATETIME_LENGTH			16
#define MTP_MAX_LENGTH_FIELD_SIZE	4

#define MTP_ASSOCIATION_GROUP1	8
#define MTP_ASSOCIATION_GROUP2	0
#define MTP_ASSOCIATION_GROUP3	0
#define MTP_ASSOCIATION_GROUP4	0
#define MTP_JPG_GROUP1			13
#define MTP_JPG_GROUP2			3
#define MTP_JPG_GROUP3			0
#define MTP_JPG_GROUP4			0
#define MTP_TIFF_GROUP1			8
#define MTP_TIFF_GROUP2			3
#define MTP_TIFF_GROUP3			0
#define MTP_TIFF_GROUP4			0
#define MTP_BMP_GROUP1			8
#define MTP_BMP_GROUP2			3
#define MTP_BMP_GROUP3			0
#define MTP_BMP_GROUP4			0
#define MTP_GIF_GROUP1			8
#define MTP_GIF_GROUP2			3
#define MTP_GIF_GROUP3			0
#define MTP_GIF_GROUP4			0
//#define MTP_AVI_GROUP1		8
#define MTP_AVI_GROUP1			9
#define MTP_AVI_GROUP2			6
#define MTP_AVI_GROUP3			0
#define MTP_AVI_GROUP4			3
#define MTP_WAV_GROUP1			8
#define MTP_WAV_GROUP2			4
#define MTP_WAV_GROUP3			0
#define MTP_WAV_GROUP4			0
#define MTP_MP3_GROUP1			8
#define MTP_MP3_GROUP2			4
#define MTP_MP3_GROUP3			0
#define MTP_MP3_GROUP4			0
#define MTP_DPOF_GROUP1			8
#define MTP_DPOF_GROUP2			0
#define MTP_DPOF_GROUP3			0
#define MTP_DPOF_GROUP4			0
#define MTP_MOV_GROUP1			13
#define MTP_MOV_GROUP2			6
#define MTP_MOV_GROUP3			0
#define MTP_MOV_GROUP4			0
#define MTP_MP4_GROUP1			13
#define MTP_MP4_GROUP2			6
#define MTP_MP4_GROUP3			0
#define MTP_MP4_GROUP4			0

#define MTP_MAX_PATH			64

//#define CO_BF_USB_MTP_DEBUG_ALL
//#define CO_BF_USB_MTP_DEBUG_READ
//#define CO_BF_USB_MTP_DEBUG_WRITE

#ifdef CO_BF_USB_MTP_DEBUG_ALL
static	ULONG sys_time_before_all;
static	ULONG sys_time_after_all;
#endif

extern char								MiscPath[MTP_MAX_PATH];
extern OBJECT_INFO						DPSDiscovery_ObjectInfo;
extern OBJECT_INFO						ObjectInfo;
extern DeviceProp_ImageSize				DevicePropImageSize;
extern DeviceProp_CompressionSetting	DevicePropCompressionSetting;
extern DeviceProp_FocalLength			DevicePropFocalLength;
extern DeviceProp_FlashMode				DevicePropFlashMode;
extern DeviceProp_IsoMode				DevicePropIsoMode;
extern STORAGE_INFO						StorageInfo, StorageInfo_SD,StorageInfo_MS;

extern UCHAR	Buffers1[2*1024],									// Send
				Buffers2[512*1024*MTP_DATA_MNG_INFO_NUM],			// Receive
				Buffers3[12+3*4],									// Interrupt
				Buffers4[64],										// Send:Response
				PTP_JPEG_BUFFER[512*1024*MTP_DATA_MNG_INFO_NUM];	// Image Bulk In by Andy Jing

extern UCHAR	*IN_Buffers,
				*OUT_Buffers,
				*INT_Buffers,
				*Resp_Buffers;

extern UCHAR	vBO;

extern UINT32	SendNumber;		//Actual number of send bytes

extern UCHAR *dpsOutBufferPtr;
extern USHORT outFileSize;

extern int PTP_fp;
extern DPSSTATUS dpsStatus;
extern int total_prints;
extern unsigned char PICT_DPOF_All_Image_flg;
//==================================================================================
extern E_BF_MEDIA_STATE_MEDIA_TYPE PictBridgeUI_MediaSelected;
extern ULONG ObjectCnt[], JpgCnt[];

extern UINT32	RecvScriptHandle;
UCHAR	ptpUpdateFW = 0;
#ifdef CO_USB_MTP_ON
extern UCHAR	DPSDiscovery;
#endif

FW_CHAR 		g_szMTPRecvFileName[120];
OBJECT_INFO		MtpRecvObjInfo;
extern UCHAR	RecvFileName[120];

ULONG MTPDeviceProperty_IsoMode = eISOAuto;//0x501e UINT8
ULONG MTPDeviceProperty_ImageSize = 0; //0x5004 (2008/09/10) tony +
ULONG MTPDeviceProperty_CompressionSetting = eJpegGood;//0x5004 (0 - 2) Best, Better, Good (UINT8) 
ULONG MTPDeviceProperty_FocalLength = 0;//460;//0x5008 (1 - 15) UINT32 
ULONG MTPDeviceProperty_FlashMode = 0;//0x500c (0:Auto, 1:Off, 2:Fill, 3:Auto(Red)) UINT16

extern long BULK_Read( unsigned char *buf, unsigned long Size);

ULONG	DeviceProperty_PerceivedDeviceType=0x01;
ULONG	DeviceProperty_DeviceIcon=0x01;
UCHAR	DeviceProperty_BatteryLevel=0x00;
ULONG	DeviceProperty_DateTime=0x00;

extern E_BF_MEDIA_STATE_MEDIA_TYPE MTP_media_Type;
extern UINT32 PtpStorageID;
extern T_BF_PTP_BUFF* gBF_Ptp_Buff;
extern long MtpEnd_Flg;

typedef enum {
	//MTP_FILE_NO_TYPE	= 0x00,
	//MTP_FOLDER_TYPE 	= 0x01,
	//MTP_FILE_TYPE		= 0x02,
	MTP_FILE_TYPE_JPG	= 0x00,
	MTP_FILE_TYPE_WAV	= 0x01,
	MTP_FILE_TYPE_AVI	= 0x02,
	MTP_FILE_TYPE_ARC	= 0x03,
	MTP_FILE_TYPE_MP3	= 0x04,
	MTP_FILE_TYPE_BMP   = 0x05,
	MTP_FILE_TYPE_GIF	= 0x06,
	MTP_FILE_TYPE_TIFF	= 0x07,
	MTP_FILE_TYPE_DPOF	= 0x08,
	MTP_FILE_TYPE_MOV	= 0x09,
	MTP_FILE_TYPE_MP4	= 0x0A,
}FILE_EXTTYPE;

typedef enum {
	MTP_FILE_NO_TYPE	= 0x00,
	MTP_FOLDER_TYPE 	= 0x01,
	MTP_FILE_TYPE		= 0x02,
}FILE_ATTR;

typedef struct T_MTP_DB_FILE_INFO{
	struct{
		UCHAR file_exttype:4;
		UCHAR file_attr:2;
		UCHAR fDelete:1;
		UCHAR reserve:1;
	}bit;
	UINT32 	nFileIndex;
	FW_CHAR	szDosFileName[MTP_MAX_PATH];
	UINT32 	nParentIndex;
}T_MTP_DB_FILE_INFO;

typedef struct {
	ULONG				epno;
	ULONG				addr;
	ULONG				size;
	ULONG				opt;
} T_MTP_DATA_MNG_INFO;

typedef struct {
	UINT32				read;
	UINT32				write;
	T_MTP_DATA_MNG_INFO	info[ MTP_DATA_MNG_INFO_NUM ];
} T_MTP_DATA_MNG;

static volatile T_MTP_DATA_MNG MtpDataMng;

#define MTP_SUPPORT_JPG_FILE 			0x00000001 
#define MTP_SUPPORT_WAV_FILE			0x00000002 
#define MTP_SUPPORT_AVI_FILE			0x00000004 
#define MTP_SUPPORT_TIFF_FILE			0x00000008 
#define MTP_SUPPORT_BMP_FILE			0x00000010 
#define MTP_SUPPORT_GIF_FILE			0x00000020 
#define MTP_SUPPORT_MP3_FILE			0x00000040 
#define MTP_SUPPORT_DPOF_FILE			0x00000080 
#define MTP_SUPPORT_MOV_FILE			0x00000100 
#define MTP_SUPPORT_MP4_FILE			0x00000200 

typedef struct {
	FW_USHORT jpg_flg : 1;	// 0:
	FW_USHORT wav_flg : 1;	// 1:
	FW_USHORT avi_flg : 1;	// 2:
	FW_USHORT tif_flg : 1;	// 3:
	FW_USHORT bmp_flg : 1;	// 4:
	FW_USHORT gif_flg : 1;	// 5:
	FW_USHORT mp3_flg : 1;	// 6:
	FW_USHORT dpof_flg: 1;	// 7:DPOF
	FW_USHORT mov_flg : 1;	// 8:MOV
	FW_USHORT mp4_flg : 1;	// 9:MP4
	FW_USHORT reserve : 6 ;	// 10:Reserve
} T_DCF_EXT_TYPE;

#define ALL_FILTER_DISABLE  		0x00000000 
#define ROOT_FOLDER_ENABLE			0x00000001
#define ROOT_FILE_ENABLE   			0x00000002
#define DCIM_FILE_ENABLE   			0x00000004
#define DCF_SET_FOLDER_NAME			0x00000008
#define DCF_FILE_NAME_RULE_ENABLE 	0x00000010

// Folder Filter
// 1. Root entry enable (exclusive DCIM) ??? 	Root_Folder_flg
// 2. File Enable in root Entry				Root_File_flg
// 3. File enable in DCIM					DCIM_File_flg
// 4. File enable in DCIM sub entry			DCIM_subFolder_File_flg
// 5. spec sub DCIM folder (nnnxxxxx)              DCIM_subFolder_SpecFolder_flg

typedef struct{
	FW_USHORT Root_Folder_flg : 1;
	FW_USHORT Root_File_flg : 1;
	FW_USHORT DCIM_File_flg : 1;
	FW_USHORT Set_DCF_DirName_flg : 1;
	FW_USHORT DCF_Folder_Flg : 1;
	FW_USHORT reserve : 11;
}T_FOLDER_FILTER;

#define NULL_STRING			""

static T_FOLDER_FILTER gFolder_Filter = { 1,1,1,1,1,1 };	//{ 1,1,1,0,0,0 };
static T_DCF_EXT_TYPE gFile_Filter = { 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0 }; // Enabel:jpg, mov, mp4
static T_MTP_DB_FILE_INFO* oMTPDBFileInfo=(T_MTP_DB_FILE_INFO*)(SDRAM_ADR_PLAY_MSC_BUFFER_AREA);
//static T_MTP_DB_FILE_INFO* oMTPDBFileInfo=(T_MTP_DB_FILE_INFO*)(SDRAM_AND & SDRAM_ADR_PLAY_MSC_BUFFER_AREA);	// tmp
FW_CHAR 		g_szRootFolder[MTP_MAX_PATH]=NULL_STRING;
FW_CHAR 		g_szDCFDir[MTP_MAX_PATH]=NULL_STRING;
static 	INT32 	g_nObjectCnt=0;

// 0x3001
MTPFormatCode_AssociationSupportProp MTPFormatCodeAssociationSupportProp={
	{0x08,0x00,0x00,0x00},
	{0x01,0xDC},
	{0x02,0xDC},
	{0x03,0xDC},
	{0x04,0xDC},
	{0x07,0xDC},
	{0x0B,0xDC},
	{0x41,0xDC},
	{0x44,0xDC},
};

// 0x3006
MTPFormatCode_DPOFSupportProp MTPFormatCodeDPOFSupportProp={
	{DPOF_SUPPPROP_NUM,0x00,0x00,0x00},
	{0x01,0xDC},
	{0x02,0xDC},
	{0x03,0xDC},
	{0x04,0xDC},
	{0x07,0xDC},
	{0x0B,0xDC},
	{0x41,0xDC},
	{0x44,0xDC},
};

//0x3008 (ObjectPropsCodeArray)
MTPFormatCode_WAVSupportProp MTPFormatCodeWAVSupportProp={
	{WAV_SUPPPROP_NUM,0x00,0x00,0x00},
	{0x01,0xDC},
	{0x02,0xDC},
	{0x03,0xDC},
	{0x04,0xDC},
	{0x07,0xDC},
	{0x0B,0xDC},
	{0x41,0xDC},
	{0x44,0xDC},
	{0x89,0xDC},
	{0x93,0xDE},
	{0x94,0xDE},
	{0x9A,0xDE},
};

//0x3009 (ObjectPropsCodeArray)
MTPFormatCode_MP3SupportProp MTPFormatCodeMP3SupportProp={
	{MP3_SUPPPROP_NUM,0x00,0x00,0x00},
	{0x01,0xDC},
	{0x02,0xDC},
	{0x03,0xDC},
	{0x04,0xDC},
	{0x07,0xDC},
	{0x0B,0xDC},
	{0x41,0xDC},
	{0x44,0xDC},
	{0x89,0xDC},
	{0x93,0xDE},
	{0x94,0xDE},
	{0x9A,0xDE},
};

//0x300A (ObjectPropsCodeArray)
MTPFormatCode_AVISupportProp MTPFormatCodeAVISupportProp={
	{AVI_SUPPPROP_NUM,0x00,0x00,0x00},
	{0x01,0xDC},
	{0x02,0xDC},
	{0x03,0xDC},
	{0x04,0xDC},
	{0x07,0xDC},
	{0x09,0xDC}, // 2009/04/08 tony + (for modify date)
	{0x0B,0xDC},
	{0x41,0xDC},
	{0x44,0xDC},
	{0x87,0xDC},
	{0x88,0xDC},
	{0x89,0xDC},
	{0x93,0xDE},
	{0x94,0xDE},
	{0x97,0xDE},
	{0x9A,0xDE},
	{0x9B,0xDE},
	{0x9C,0xDE},
};

//0x3801 (ObjectPropsCodeArray)
MTPFormatCode_JPEGSupportProp MTPFormatCodeJPEGSupportProp={
	{JPG_SUPPPROP_NUM,0x00,0x00,0x00},
	{0x01,0xDC},
	{0x02,0xDC},
	{0x03,0xDC},
	{0x04,0xDC},
	//{0x05,0xDC},
	//{0x06,0xDC},
	{0x07,0xDC},
	//{0x08,0xDC},
	//{0x09,0xDC},
	//{0x0A,0xDC},
	{0x0B,0xDC},
	{0x41,0xDC},
	{0x44,0xDC},
	{0x81,0xDC},
	{0x82,0xDC},
	{0x83,0xDC},
	{0x84,0xDC},
	{0x86,0xDC},
	{0x87,0xDC},
	{0x88,0xDC},
	{0xD3,0xDC},
};

MTPFormatCode_ImageSupportProp MTPFormatCodeBMPSupportProp={
	{BMP_SUPPPROP_NUM,0x00,0x00,0x00},
	{0x01,0xDC},
	{0x02,0xDC},
	{0x03,0xDC},
	{0x04,0xDC},
	{0x07,0xDC},
	{0x0B,0xDC},
	{0x41,0xDC},
	{0x44,0xDC},
	{0x87,0xDC},
	{0x88,0xDC},
	{0xD3,0xDC},
};

MTPFormatCode_ImageSupportProp MTPFormatCodeGIFSupportProp={
	{GIF_SUPPPROP_NUM,0x00,0x00,0x00},
	{0x01,0xDC},
	{0x02,0xDC},
	{0x03,0xDC},
	{0x04,0xDC},
	{0x07,0xDC},
	{0x0B,0xDC},
	{0x41,0xDC},
	{0x44,0xDC},
	{0x87,0xDC},
	{0x88,0xDC},
	{0xD3,0xDC},
};

MTPFormatCode_ImageSupportProp MTPFormatCodeTIFFSupportProp={
	{TIFF_SUPPPROP_NUM,0x00,0x00,0x00},
	{0x01,0xDC},
	{0x02,0xDC},
	{0x03,0xDC},
	{0x04,0xDC},
	{0x07,0xDC},
	{0x0B,0xDC},
	{0x41,0xDC},
	{0x44,0xDC},
	{0x87,0xDC},
	{0x88,0xDC},
	{0xD3,0xDC},
};

MTPFormatCode_MOVSupportProp MTPFormatCodeMOVSupportProp={
	{MOV_SUPPPROP_NUM,0x00,0x00,0x00},
	{0x01,0xDC},
	{0x02,0xDC},
	{0x03,0xDC},
	{0x04,0xDC},
	{0x07,0xDC},
	{0x0B,0xDC},
	{0x41,0xDC},
	{0x44,0xDC},
	{0x81,0xDC},
	{0x82,0xDC},
	{0x83,0xDC},
	{0x84,0xDC},
	{0x86,0xDC},
	{0x87,0xDC},
	{0x88,0xDC},
	{0x89,0xDC},
	{0x93,0xDE},
	{0x94,0xDE},
	{0x9A,0xDE},
};

MTPFormatCode_MP4SupportProp MTPFormatCodeMP4SupportProp={
	{MP4_SUPPPROP_NUM,0x00,0x00,0x00},
	{0x01,0xDC},
	{0x02,0xDC},
	{0x03,0xDC},
	{0x04,0xDC},
	{0x07,0xDC},
	{0x0B,0xDC},
	{0x41,0xDC},
	{0x44,0xDC},
	{0x81,0xDC},
	{0x82,0xDC},
	{0x83,0xDC},
	{0x84,0xDC},
	{0x86,0xDC},
	{0x87,0xDC},
	{0x88,0xDC},
	{0x89,0xDC},
	{0x93,0xDE},
	{0x94,0xDE},
	{0x9A,0xDE},
};

MTPObjectProperty_StorageID MTPobjectPropertyStorageID={
	{0x01,0xDC},				//PropertyCode
	{0x06,0x00},				//DataTyep
	{0x00},					//GetSet
	{0x00,0x00,0x00,0x00},	//Default
	{0x01,0x00,0x00,0x00},	//GroupCode
	{0x00},					//FormFlag
};

//MTPObjectProperty_StorageID MTPobjectPropertyStorageID_NF={
//	{0x01,0xDC},				//PropertyCode
//	{0x06,0x00},				//DataTyep
//	{0x00},					//GetSet
//	{0x00,0x00,0x00,0x00},	//Default
//	{0x02,0x00,0x00,0x00},	//GroupCode
//	{0x00},					//FormFlag
//};
//
//MTPObjectProperty_StorageID MTPobjectPropertyStorageID_MC={
//	{0x01,0xDC},				//PropertyCode
//	{0x06,0x00},				//DataTyep
//	{0x00},					//GetSet
//	{0x00,0x00,0x00,0x00},	//Default
//	{0x03,0x00,0x00,0x00},	//GroupCode
//	{0x00},					//FormFlag
//};

MTPObjectProperty_ObjectFormat MTPObjectPropertyObjectFormat={
	{0x02,0xDC},
	{0x04,0x00},
	{0x00},
	{0x00,0x00},
	{0x01,0x00,0x00,0x00},
	{0x00},
};

MTPObjectProperty_ProtectionStatus MTPObjectPropertyProtectionStatus={
	{0x03,0xDC},
	{0x04,0x00},
	{0x00},
	{0x00,0x00},
	{0x01,0x00,0x00,0x00},
	{0x02},
	{0x04,0x00},
	{0x00,0x00},
	{0x01,0x00},
	{0x02,0x80},
	{0x03,0x80},
};

MTPObjectProperty_ObjectSize MTPObjectPropertyObjectSize={
	{0x04,0xDC},
	{0x08,0x00},
	{0x00},
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x01,0x00,0x00,0x00},
	{0x00},
};

MTPObjectProperty_AssociationType MTPObjectPropertyAssociationType={
	{0x05,0xDC},
	{0x04,0x00},
	{0x00},
	{0x00,0x00},
	{0x01,0x00,0x00,0x00},
	{0x02},
	{0x00,0x00},
	{0x01,0x00},
};

MTPObjectProperty_AssociationDesc MTPObjectPropertyAssociationDesc={
	{0x06,0xDC},
	{0x06,0x00},
	{0x00},
	{0x00,0x00,0x00,0x00},
	{0x01,0x00,0x00,0x00},
	{0x00},
};

MTPObjectProperty_ObjectFileName MTPObjectPropertyObjectFileName={
	{0x07,0xDC},
	{0xFF,0xFF},
	{0x01},
	{0x00},
	{0x01,0x00,0x00,0x00},
	{0x00},
};

MTPObjectProperty_DateCreated MTPObjectPropertyDateCreated={
	{0x08,0xDC},
	{0xFF,0xFF},
	{0x00},
	{0x00},
	{0x01,0x00,0x00,0x00},
	{0x03},
};

MTPObjectProperty_DateModified MTPObjectPropertyDateModified={
	{0x09,0xDC},
	{0xFF,0xFF},
	{0x00},
	{0x00},
	{0x01,0x00,0x00,0x00},
	{0x03},
};

MTPObjectProperty_Keywords MTPObjectPropertyKeywords={
	{0x0A,0xDC},
	{0xFF,0xFF},
	{0x00},
	{0x01,0x00,0x00,0x00},
	{0x00},
};

MTPObjectProperty_ParentObject MTPObjectPropertyParentObject={
	{0x0B,0xDC},
	{0x06,0x00},
	{0x00},
	{0x00,0x00,0x00,0x00},
	{0x01,0x00,0x00,0x00},
	{0x00},
};

MTPObjectProperty_PUOI MTPObjectPropertyPUOI={
	{0x41,0xDC},
	{0x0A,0x00},
	{0x00},
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x01,0x00,0x00,0x00},
	{0x00},
};

MTPObjectProperty_Name MTPObjectPropertyName={
	{0x44,0xDC},
	{0xFF,0xFF},
	{0x01},
	{0x00},
	{0x01,0x00,0x00,0x00},
	{0x00},
};

MTPObjectProperty_RepresentativeSampleFormat MTPObjectPropRepFormat={
	{0x81,0xDC},
	{0x04,0x00},
	{0x00},
	{0x00,0x00},
	{0x01,0x00,0x00,0x00},
	{0x02},
	{0x01,0x00},
	{0x08,0x38},
};

MTPObjectProperty_RepresentativeSampleSize MTPObjectPropRepSize={
	{0x82,0xDC},
	{0x06,0x00},
	{0x00},
	{0x00,0x00,0x00,0x00},
	{0x01,0x00,0x00,0x00},
	{0x01},
	{0x00,0x00,0x00,0x00},
	{0xFF,0xFF,0xFF,0xFF},
	{0x01,0x00,0x00,0x00},
};

MTPObjectProperty_RepresentativeSampleHeight MTPObjectPropRepHeight={
	{0x83,0xDC},
	{0x06,0x00},
	{0x00},
	{0x00,0x00,0x00,0x00},
	{0x01,0x00,0x00,0x00},
	{0x01},
	{0x00,0x00,0x00,0x00},
	{0xFF,0xFF,0xFF,0xFF},
	{0x01,0x00,0x00,0x00},
};

MTPObjectProperty_RepresentativeSampleWidth MTPObjectPropRepWidth={
	{0x84,0xDC},
	{0x06,0x00},
	{0x00},
	{0x00,0x00,0x00,0x00},
	{0x01,0x00,0x00,0x00},
	{0x01},
	{0x00,0x00,0x00,0x00},
	{0xFF,0xFF,0xFF,0xFF},
	{0x01,0x00,0x00,0x00},
};

MTPObjectProperty_RepresentativeSampleData MTPObjectPropRepData={
	{0x86,0xDC},					//PropertyCode
	{0x02,0x40},					//DataTyep
	{0x00},						//GetSet
	{0x00,0x00,0x00,0x00},		//Default
	{0x01,0x00,0x00,0x00},		//GroupCode
	{0x06},						//FormFlag
	{0x00,0x00,0x10,0x00},		//MaxLength
};

MTPObjectProperty_ImageWidth MTPObjectPropertyImageWidth={
	{0x87,0xDC},
	{0x06,0x00},
	{0x00},
	{0x00,0x00,0x00,0x00},
	{0x02,0x00,0x00,0x00},
	{0x01},
	{0x00,0x00,0x00,0x00},
	{0xFF,0xFF,0xFF,0xFF},
	{0x01,0x00,0x00,0x00},
};

MTPObjectProperty_ImageHeight MTPObjectPropertyImageHeight={
	{0x88,0xDC},
	{0x06,0x00},
	{0x00},
	{0x00,0x00,0x00,0x00},
	{0x02,0x00,0x00,0x00},
	{0x01},
	{0x00,0x00,0x00,0x00},
	{0xFF,0xFF,0xFF,0xFF},
	{0x01,0x00,0x00,0x00},
};

MTPObjectProperty_ImageBitDepth MTPObjectPropertyImageBitDepth={
	{0xD3,0xDC},
	{0x06,0x00},
	{0x00},
	{0x00,0x00,0x00,0x00},
	{0x02,0x00,0x00,0x00},
	{0x02},
	{0x01,0x00},
	{0x00,0x00,0x00,0x00},
};

MTPObjectProperty_Duration MTPObjectPropertyDuration={
	{0x89,0xDC},
	{0x06,0x00},
	{0x00},
	{0x00,0x00,0x00,0x00},
	{0x02,0x00,0x00,0x00},
	{0x01},
	{0x00,0x00,0x00,0x00},
	{0xFF,0xFF,0xFF,0xFF},
	{0x01,0x00,0x00,0x00},
};

MTPObjectProperty_SampleRate MTPObjectPropertySampleRate={
	{0x93,0xDE},
	{0x06,0x00},
	{0x00},
	{0x00,0x00,0x00,0x00},
	{0x02,0x00,0x00,0x00},
	{0x02},
	{0x01,0x00},
	{0x80,0xBB,0x00,0x00},
};

MTPObjectProperty_NumberOfChannel MTPObjectPropertyNumberOfChannel={
	{0x94,0xDE},
	{0x04,0x00},
	{0x01},
	{0x02,0x00},
	{0x02,0x00,0x00,0x00},
	{0x02},
	{0x01,0x00},
	{0x02,0x00},
};

MTPObjectProperty_ScanType MTPObjectPropertyScanType={
	{0x97,0xDE},
	{0x04,0x00},
	{0x01},
	{0x00,0x00},
	{0x04,0x00,0x00,0x00},
	{0x02},
	{0x02,0x00},
	{0x00,0x00},
	{0x01,0x00},
};

MTPObjectProperty_AudioBitRate MTPObjectPropertyAudioBitRate={
	{0x9A,0xDE},
	{0x06,0x00},
	{0x00},
	{0x00,0x00,0x00,0x00},
	{0x02,0x00,0x00,0x00},
	{0x01},
	{0x00,0x00,0x00,0x00},
	{0xFF,0xFF,0xFF,0xFF},
	{0x01,0x00,0x00,0x00},
};

MTPObjectProperty_VideoFourCC_Code MTPObjectPropertyVideoFourCCCode={
	{0x9B,0xDE},
	{0x06,0x00},
	{0x01},
	{0x00,0x00,0x00,0x00},
	{0x04,0x00,0x00,0x00},
	{0x02},
	{0x02,0x00},
	{0x00,0x00,0x00,0x00},
	{0x6E,0x6A,0x70,0x67},
};

MTPObjectProperty_VideoBitRate MTPObjectPropertyVideoBitRate={
	{0x9C,0xDE},
	{0x06,0x00},
	{0x00},
	{0x00,0x00,0x00,0x00},
	{0x04,0x00,0x00,0x00},
	{0x01},	
	{0x00,0x00,0x00,0x00},
	{0xFF,0xFF,0xFF,0xFF},
	{0x01,0x00,0x00,0x00},
};

MTP_DEVICE_INFO MTPDeviceInfo={
	{0x64,0x00},				// StandardVersion
	{0x06,0x00,0x00,0x00},	//MTP Vensor Extension ID
	{0x64,0x00},				//MTP Version
	23,						//VendorExtersionDesc
	{'F',0,'U',0,'J',0,'I',0,'T',0,'S',0,'U',0,' ',0,'P',0,'T',0,'P',0,' ',0,
	 'E',0,'x',0,'t',0,'e',0,'n',0,'s',0,'i',0,'o',0,'n',0,'s',0,0,0},

	{0,0},				//FunctionMode

	{22,0,0,0},			//OperationsSupported
	{
		0x01,0x10,
		0x02,0x10,
		0x03,0x10,
		0x04,0x10,
		0x05,0x10,
		0x06,0x10,
		0x07,0x10,
		0x08,0x10,
		0x09,0x10,
		0x0A,0x10,
		0x0B,0x10,
		0x0C,0x10,
		0x0D,0x10,
		0x0F,0x10,
		0x14,0x10,
		0x15,0x10,
		0x16,0x10,
		0x1B,0x10,
		0x01,0x98,
		0x02,0x98,
		0x03,0x98,
		0x05,0x98,
		//0x15,0x10,
	},

	{6,0,0,0},			//EventsSupported
	{
		0x02,0x40,
		0x03,0x40,
		0x04,0x40,
		0x05,0x40,
		0x07,0x40,
		0x09,0x40,
	},

	{1,0,0,0},			//DevicePropertiesSupported
	//0x01,0x50,
	//0x03,0x50,
	//0x04,0x50,
	//0x08,0x50,
	//0x0c,0x50,
	//0x11,0x50,
	//0x05,0xD4,
	{0x07,0xD4},

	{4,0,0,0},			// CaptureFormats
	{
		0x01, 0x30,			// Association(e.g folder)
		0x01, 0x38,			// Exif/JPEG
		0x80, 0xB9,			// MOV File
		0x82, 0xB9,			// MP4 File
	},

	{4,0,0,0},			// ImageFormats
	{
		0x01, 0x30,			// Association(e.g folder)
		//0x0A, 0x30,		// AVI   File
		//0x08, 0x30,		// WAV File
		//0x09, 0x30,		// MP3 File
		0x01, 0x38,			// JPEG  File
		//0x04, 0x38,		// BMP File
		//0x07, 0x38,		// GIF File
		//0x0D, 0x38,		// TIFF File
		0x80, 0xB9,			// MOV File
		0x82, 0xB9,			// MP4 File
	},

//	PTP_VENDOR_NAME_STRING_LEN,	//Manufacturer
//	{'F',0,'U',0,'J',0,'I',0,'T',0,'S',0,'U',0,' ',0,'C',0,'o',0,
//	'm',0,'p',0,'.',0,0,0,0,0},

	PTP_VENDOR_NAME_STRING_LEN,	//Manufacturer
	FJ_CUSTOM_USB_MTP_MANUFACTURER_STR,

//	PTP_MODEL_STRING_LEN,	//Model
//	{'M',0,'T',0,'P',0,' ',0,'M',0,'O',0,'D',0,'E',0,'L',0,' ',0,' ',0,' ',0,
//	' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,0,0},

	PTP_MODEL_STRING_LEN,	//Model
	FJ_CUSTOM_USB_MTP_MODEL_STR,

	0x04,	//DeviceVersion
	{'1',0,'.',0,'0',0,0,0},

	PTP_SERIAL_NUMBER_LEN,	//SerialNumber
	{'0',0,'1',0,'2',0,'3',0,'4',0,'5',0,'6',0,'7',0,'8',0,'9',0,' ',0,' ',0,0,0},
};

// 0x5001 : Battely Level
MTP_DeviceProp_BatteryLevel MTPDevicePropBatteryLevel={
	{0x01,0x50},	//DevicePropCode
	{0x02,0x00},	//DataType : UINT16
	{0x00},		//Get/Set
	{0x64},		//Factory Default
	{0x64},		//Current Value
	{0x01},		//Form Flag : Range-Form
	{0x01},		//Min Value
	{0x64},		//Max Value
	{0x01},		//Step Value
};

// 0x5003 : Device Property ImageSize
MTP_DeviceProp_ImageSize MTPDevicePropImageSize={
	{0x03,0x50},	//DevicePropCode
	{0xFF,0xFF},	//DataType : STRING
	{0x01},		//Get/Set
	{0x00},		//Factory Default
	{0x00},		//Current Value
	{0x02},		//Form Flag : Enumeration-Form
	{0x06,0x00},	//Number
	{10},			//Length
	{'3',0,'0',0,'7',0,'2',0,'*',0,'2',0,'3',0,'0',0,'4',0,0,0},		// String
	{10},			//Length
	{'3',0,'0',0,'7',0,'2',0,'*',0,'2',0,'0',0,'4',0,'8',0,0,0},		// String
	{10},			//Length
	{'2',0,'5',0,'7',0,'6',0,'*',0,'1',0,'9',0,'3',0,'2',0,0,0},		// String
	{10},			//Length
	{'2',0,'0',0,'4',0,'8',0,'*',0,'1',0,'5',0,'3',0,'6',0,0,0},		// String
	{10},			//Length
	{'1',0,'6',0,'0',0,'0',0,'*',0,'1',0,'2',0,'0',0,'0',0,0,0},		// String
	{8},			//Length
	{'6',0,'4',0,'0',0,'*',0,'4',0,'8',0,'0',0,0,0},					// String
};

// 0x5005 : White Balance
MTP_DeviceProp_WhiteBalance MTPDevicePropWhitebalance={
	{0x05,0x50},	//DevicePropCode
	{0x04,0x00},	//DataType : UINT16
	{0x01},		//Get/Set
	{0x00},		//Factory Default
	{0x00},		//Current Value
	{0x02},		//Form Flag : Enumeration-Form
};

// 0x5006 : RGB Gain
MTP_DeviceProp_RGBGain MTPDevicePropRGBGain={
	{0x06,0x50},	//DevicePropCode
	{0xFF,0xFF},	//DataType : STRING
	{0x01},		//Get/Set
	{0x00},		//Factory Default
	{0x00},		//Current Value
	{0x02},		//Form Flag : Enumeration-Form
};

// 0x5007 : F-Number
MTP_DeviceProp_FNumber MTPDevicePropFNumber={
	{0x07,0x50},	//DevicePropCode
	{0x04,0x00},	//DataType : UINT16
	{0x01},		//Get/Set
	{0x00,0x00},	//Factory Default
	{0x00,0x00},	//Current Value
	{0x02},		//Form Flag : Enumeration-Form
};

// 0x5008 : Focal Length
MTP_DeviceProp_FocalLength MTPDevicePropFocalLength={
	{0x08,0x50},					//DevicePropCode
	{0x06,0x00},					//DataType : UINT32
	{0x01},						//Get/Set
	{0x00,0x00,0x00,0x00},		//Factory Default
	{0x00,0x00,0x00,0x00},		//Current Value
	{0x02},						//Form Flag : Enumeration-Form
};

// 0x5009 : Focus Distance
MTP_DeviceProp_FocusDistance MTPDevicePropFocusDistance={
	{0x09,0x50},	//DevicePropCode
	{0x04,0x00},	//DataType : UINT16
	{0x01},		//Get/Set
	{0x00,0x00},	//Factory Default
	{0x00,0x00},	//Current Value
	{0x02},		//Form Flag : Enumeration-Form
};

// 0x500A : Focus Mode
MTP_DeviceProp_FocusMode MTPDevicePropFocusMode={
	{0x0A,0x50},	//DevicePropCode
	{0x04,0x00},	//DataType : UINT16
	{0x01},		//Get/Set
	{0x00,0x00},	//Factory Default
	{0x00,0x00},	//Current Value
	{0x02},		//Form Flag : Enumeration-Form
};

// 0x500B : Exposure Metering Mode
MTP_DeviceProp_ExposureMeteringMode MTPDevicePropExposureMeteringMode={
	{0x0B,0x50},	//DevicePropCode
	{0x04,0x00},	//DataType : UINT16
	{0x01},		//Get/Set
	{0x00,0x00},	//Factory Default
	{0x00,0x00},	//Current Value
	{0x02},		//Form Flag : Enumeration-Form
};

// 0x500C : Flash Mode
MTP_DeviceProp_FlashMode MTPDevicePropFlashMode={
	{0x0C,0x50},	//DevicePropCode
	{0x04,0x00},	//DataType : UINT16
	{0x01},		//Get/Set
	{0x00,0x00},	//Factory Default
	{0x00,0x00},	//Current Value
	{0x02},		//Form Flag : Enumeration-Form
};

// 0x500D : Exposure Time
MTP_DeviceProp_ExposureTime MTPDevicePropExposureTime={
	{0x0D,0x50},					//DevicePropCode
	{0x06,0x00},					//DataType : UINT32
	{0x01},						//Get/Set
	{0x00,0x00,0x00,0x00},		//Factory Default
	{0x00,0x00,0x00,0x00},		//Current Value
	{0x02},						//Form Flag : Enumeration-Form
};

// 0x500E : ExposureProgramMode
MTP_DeviceProp_ExposureProgramMode MTPDevicePropExposureProgramMode={
	{0x0E,0x50},	//DevicePropCode
	{0x04,0x00},	//DataType : UINT16
	{0x01},		//Get/Set
	{0x00,0x00},	//Factory Default
	{0x00,0x00},	//Current Value
	{0x02},		//Form Flag : Enumeration-Form
};

// 0x500F : ExposureIndex
MTP_DeviceProp_ExposureIndex MTPDevicePropExposureIndex={
	{0x0F,0x50},	//DevicePropCode
	{0x04,0x00},	//DataType : UINT16
	{0x01},		//Get/Set
	{0x00,0x00},	//Factory Default
	{0x00,0x00},	//Current Value
	{0x02},		//Form Flag : Enumeration-Form
};

// 0x5010 : ExposureBiasCompensation
MTP_DeviceProp_ExposureBiasCompensation MTPDevicePropExposureBiasCompensation={
	{0x10,0x50},	//DevicePropCode
	{0x03,0x00},	//DataType : INT16
	{0x01},		//Get/Set
	{0x00,0x00},	//Factory Default
	{0x00,0x00},	//Current Value
	{0x02},		//Form Flag : Enumeration-Form
};

// 0x5011 : DateTime
MTP_DeviceProp_DateTime MTPDevicePropDateTime={
	{0x11,0x50},	//DevicePropCode
	{0xFF,0xFF},	//DataType : STRING
	{0x01},		//Get/Set
	{0x10},		//Length
	{'2',0,'0',0,'0',0,'6',0,'0',0,'1',0,'0',0,'1',0,'T',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,0,0},	// String
	{0x10},		//Length
	{'2',0,'0',0,'9',0,'9',0,'1',0,'2',0,'3',0,'1',0,'T',0,'2',0,'3',0,'5',0,'9',0,'5',0,'9',0,0,0},	// String
	{0x00},		//Form Flag : None
};

// 0x5012 : CaptureDelay
MTP_DeviceProp_CaptureDelay MTPDevicePropCaptureDelay={
	{0x12,0x50},					//DevicePropCode
	{0x06,0x00},					//DataType : UINT32
	{0x01},						//Get/Set
	{0x00,0x00,0x00,0x00},		//Factory Default
	{0x00,0x00,0x00,0x00},		//Current Value
	{0x02},						//Form Flag : Enumeration-Form
};

// 0x5013 :  StillCaptureMode
MTP_DeviceProp_StillCaptureMode MTPDevicePropStillCaptureMode={
	{0x13,0x50},	//DevicePropCode
	{0x04,0x00},	//DataType : UINT16
	{0x01},		//Get/Set
	{0x00,0x00},	//Factory Default
	{0x00,0x00},	//Current Value
	{0x02},		//Form Flag : Enumeration-Form
};

// 0x5014 : Contrast
MTP_DeviceProp_Contrast MTPDevicePropContrast={
	{0x14,0x50},	//DevicePropCode
	{0x02,0x00},	//DataType : UINT8
	{0x01},		//Get/Set
	{0x00},		//Factory Default
	{0x00},		//Current Value
	{0x02},		//Form Flag : Enumeration-Form
};

// 0x5015 : Sharpness
MTP_DeviceProp_Sharpness MTPDevicePropSharpness={
	{0x15,0x50},	//DevicePropCode
	{0x02,0x00},	//DataType : UINT8
	{0x01},		//Get/Set
	{0x00},		//Factory Default
	{0x00},		//Current Value
	{0x02},		//Form Flag : Enumeration-Form
};

// 0x5016 : DigitalZoom
MTP_DeviceProp_DigitalZoom MTPDevicePropDigitalZoom={
	{0x16,0x50},	//DevicePropCode
	{0x02,0x00},	//DataType : UINT8
	{0x01},		//Get/Set
	{0x00},		//Factory Default
	{0x00},		//Current Value
	{0x02},		//Form Flag : Enumeration-Form
};

// 0x5017 : EffectMode
MTP_DeviceProp_EffectMode MTPDevicePropEffectMode={
	{0x17,0x50},	//DevicePropCode
	{0x04,0x00},	//DataType : UINT16
	{0x01},		//Get/Set
	{0x00,0x00},	//Factory Default
	{0x00,0x00},	//Current Value
	{0x02},		//Form Flag : Enumeration-Form
};

// 0x5018 : BurstNumber
MTP_DeviceProp_BurstNumber MTPDevicePropBurstNumber={
	{0x18,0x50},	//DevicePropCode
	{0x04,0x00},	//DataType : UINT16
	{0x01},		//Get/Set
	{0x00},		//Factory Default
	{0x00},		//Current Value
	{0x02},		//Form Flag : Enumeration-Form
};

// 0x5019 : BurstInterval
MTP_DeviceProp_BurstInterval MTPDevicePropBurstInterval={
	{0x19,0x50},	//DevicePropCode
	{0x04,0x00},	//DataType : STRING
	{0x01},		//Get/Set
	{0x00,0x00},	//Factory Default
	{0x00,0x00},	//Current Value
	{0x02},		//Form Flag : Enumeration-Form
};

// 0x501A : TimelapseNumber
MTP_DeviceProp_TimelapseNumber MTPDevicePropTimelapseNumber={
	{0x1A,0x50},	//DevicePropCode
	{0x04,0x00},	//DataType : UINT16
	{0x01},		//Get/Set
	{0x00,0x00},	//Factory Default
	{0x00,0x00},	//Current Value
	{0x02},		//Form Flag : Enumeration-Form
};

// 0x501B : TimelapseInterval
MTP_DeviceProp_TimelapseInterval TimelapseInterval={
	{0x1B,0x50},					//DevicePropCode
	{0x06,0x00},					//DataType : UINT32
	{0x01},						//Get/Set
	{0x00,0x00,0x00,0x00},		//Factory Default
	{0x00,0x00,0x00,0x00},		//Current Value
	{0x02},						//Form Flag : Enumeration-Form
};

// 0x501C : FocusMeteringMode
MTP_DeviceProp_FocusMeteringMode MTPDevicePropFocusMeteringMode={
	{0x1C,0x50},					//DevicePropCode
	{0x06,0x00},					//DataType : UINT32
	{0x01},						//Get/Set
	{0x00,0x00,0x00,0x00},		//Factory Default
	{0x00,0x00,0x00,0x00},		//Current Value
	{0x02},						//Form Flag : Enumeration-Form
};

// 0xD405 : DeviceIcon
MTPDeviceProp_DeviceIcon MTPDevicePropDeviceIcon={
	{0x05,0xD4},				// DevicePropCode
	{0x02,0x40},				// DataType: AUINT8
	{0x00},					// Get
//	{0x01,0x00,0x00,0x00},	// Default Value
//	{0x01,0x00,0x00,0x00},	// Current Value
//	{0x00},					// None
};

// 0xD407 : PerceivedDeviceType
MTPDeviceProp_PerceiveDeviceType DevicePropPerceiveDeviceType={
	{0x07,0xD4},				// DevicePropCode
	{0x06,0x00},				// DataType: UINT32
	{0x00},					// Get
	{0x01,0x00,0x00,0x00},	// Default Value
	{0x01,0x00,0x00,0x00},	// Group Code
	{0x00},					// None
};

//INT32 MTP_SetFolderFilter (FW_CHAR *szRootFolder,FW_CHAR *szDCFDir,INT32 nFilterFlag);
//INT32 MTP_SetFileTypeFilter (INT32 nFilterFlag);

extern void	WriteDouble(UCHAR *Addr, UINT64 value);
extern void WriteShort(UCHAR *Addr, USHORT value);
extern void WriteInt(UCHAR *Addr, UINT32 value);
extern void SendResponseSimple(UINT32 Code);
extern void SendData(UINT32 DataLen);
extern void SendDataDetail2(UINT32 LenField, UINT32 RealSendSize);
extern int strcmpi( const char* a, const char* b);
extern UINT32 CalcSendAndRest(UINT32 wantSend,UINT32 *RestSend);
extern USHORT ScriptFile_Use(UINT32 hdl);
extern long BULK_Read2( unsigned char *buf, unsigned long Size);
extern long BULK_Read3( unsigned char *buf, unsigned long Size);
extern OS_USER_ER USBPtpCmd_Check_DeviceBusy( void );

extern int wav_parse(FW_CHAR * file,WAVPROP * oWavProp);

INT32 xIsJpgFile(FW_CHAR* strFile);
INT32 xIsDCFFile(FW_CHAR* strFile);
INT32 xIsBmpFile(FW_CHAR* strFile);
INT32 xIsGifFile(FW_CHAR* strFile);
INT32 xIsTiffFile(FW_CHAR* strFile);
INT32 xIsAviFile(FW_CHAR* strFile);
INT32 xIsWavFile(FW_CHAR* strFile);
int xIsMp3File(FW_CHAR* strFile);
INT32 xIsMovFile(FW_CHAR* strFile);
INT32 xIsMp4File(FW_CHAR* strFile);
INT32 xIsDpofFile(FW_CHAR* strFile);
INT32 xGetFileName(FW_CHAR* szFullPath,FW_CHAR *szFileName);
INT32 xGetPathName(FW_CHAR* szFullPath,FW_CHAR* szPathName);
INT32 xIsFolderInDcimFolder(FW_CHAR *szDCFFolderName,BOOL *bIsDCFFolder);

static VOID MTP_InitData( VOID )
{
	memset( (VOID*)&MtpDataMng, 0, sizeof(MtpDataMng) );
}

static INT32 MTP_PutData( ULONG epno, ULONG addr, ULONG size, ULONG opt )
{
	UINT32 next		= 0;
	UINT32 write	= 0;
	UINT32 read		= 0;

	OS_User_Wai_Sem( SID_USB_MTP );

	write	= MtpDataMng.write;
	read	= MtpDataMng.read;
	next	= ( write + 1 ) % MTP_DATA_MNG_INFO_NUM;

	if ( next == read ) {
#if defined(CO_BF_USB_MTP_DEBUG_READ) || defined(CO_BF_USB_MTP_DEBUG_WRITE)
		BF_Debug_Print_Information(( "MTP_PutData() buffer full\n" ));
#endif
		OS_User_Sig_Sem( SID_USB_MTP );
		return FJ_ERR_NG;
	}

	if ( ( addr !=0 ) && ( size != 0 ) ) {
		if ( epno == BULK_WRITE_FIFO ) {
			(VOID)PTP_fread( addr, size );
		}
		else if ( epno == BULK_READ_FIFO ) {
			(VOID)BULK_Read3( (UCHAR*)addr, size );
		}
		else {
			// Normal root.
		}
	}

	MtpDataMng.info[ write ].epno	= epno;
	MtpDataMng.info[ write ].addr	= addr;
	MtpDataMng.info[ write ].size	= size;
	MtpDataMng.info[ write ].opt	= opt;

	MtpDataMng.write = next;

	OS_User_Sig_Sem( SID_USB_MTP );

	return FJ_ERR_OK;
}

static INT32 MTP_GetData( ULONG* epno, ULONG* addr, ULONG* size, ULONG* opt )
{
	UINT32	write	= 0;
	UINT32	read	= 0;
	UINT32	next	= 0;

	OS_User_Wai_Sem( SID_USB_MTP );

	write	= MtpDataMng.write;
	read	= MtpDataMng.read;
	next	= ( read + 1 ) % MTP_DATA_MNG_INFO_NUM;

	if ( read == write ) {
#if defined(CO_BF_USB_MTP_DEBUG_READ) || defined(CO_BF_USB_MTP_DEBUG_WRITE)
		BF_Debug_Print_Information(( "MTP_GetData() buffer empty\n" ));
#endif
		OS_User_Sig_Sem( SID_USB_MTP );
		return FJ_ERR_NG;
	}

	*addr	= MtpDataMng.info[ read ].addr;
	*size	= MtpDataMng.info[ read ].size;
	*epno	= MtpDataMng.info[ read ].epno;
	*opt	= MtpDataMng.info[ read ].opt;

	MtpDataMng.read = next;

	OS_User_Sig_Sem( SID_USB_MTP );

	return FJ_ERR_OK;
}

INT32 MTP_SetFolderFilter (FW_CHAR *szRootFolder,FW_CHAR *szDCFDir,INT32 nFilterFlag)
{
	memset(g_szRootFolder,0,MTP_MAX_PATH);
	strncpy(g_szRootFolder,szRootFolder,sizeof(g_szRootFolder));
	memset(g_szDCFDir,0,MTP_MAX_PATH);
	strncpy(g_szDCFDir,szDCFDir,sizeof(g_szDCFDir));

	gFolder_Filter.Root_Folder_flg 	= 0;
	gFolder_Filter.Root_File_flg    = 0;
	gFolder_Filter.DCIM_File_flg 	= 0;
	gFolder_Filter.Set_DCF_DirName_flg 	= 0;
	gFolder_Filter.DCF_Folder_Flg     = 0;

	if ( (nFilterFlag & ROOT_FOLDER_ENABLE) == ROOT_FOLDER_ENABLE)
	{
		gFolder_Filter.Root_Folder_flg = 1;
	}

	if ( (nFilterFlag & ROOT_FILE_ENABLE) == ROOT_FILE_ENABLE)
	{
		gFolder_Filter.Root_File_flg = 1;
	}

	if ( (nFilterFlag & DCIM_FILE_ENABLE) == DCIM_FILE_ENABLE)
	{
		gFolder_Filter.DCIM_File_flg = 1;
	}

	if ((nFilterFlag & DCF_SET_FOLDER_NAME) == DCF_SET_FOLDER_NAME)
	{
		gFolder_Filter.Set_DCF_DirName_flg =1;
	}

	if ( (nFilterFlag & DCF_FILE_NAME_RULE_ENABLE) == DCF_FILE_NAME_RULE_ENABLE)
	{
		gFolder_Filter.DCF_Folder_Flg =1;
	}

	strncpy(g_szRootFolder,szRootFolder,sizeof(g_szRootFolder));
	strncpy(g_szDCFDir,szDCFDir,sizeof(g_szDCFDir));

	return 0;
}

INT32 MTP_SetFileTypeFilter (INT32 nFilterFlag)
{
	gFile_Filter.jpg_flg = 0;
    gFile_Filter.wav_flg = 0;
	gFile_Filter.avi_flg = 0;
	gFile_Filter.tif_flg = 0;
	gFile_Filter.bmp_flg = 0;
	gFile_Filter.gif_flg = 0;
	gFile_Filter.mp3_flg = 0;
	gFile_Filter.dpof_flg= 0;
	gFile_Filter.mov_flg = 0;
	gFile_Filter.mp4_flg = 0;

	if ( (nFilterFlag & MTP_SUPPORT_JPG_FILE) == MTP_SUPPORT_JPG_FILE)
	{
		gFile_Filter.jpg_flg = 1;
	}
	
	if ( (nFilterFlag & MTP_SUPPORT_WAV_FILE) == MTP_SUPPORT_WAV_FILE)
	{
		gFile_Filter.wav_flg = 1;
	}
	if ( (nFilterFlag & MTP_SUPPORT_AVI_FILE) == MTP_SUPPORT_AVI_FILE)
	{
		gFile_Filter.avi_flg = 1;
	}
	if ( (nFilterFlag & MTP_SUPPORT_TIFF_FILE) == MTP_SUPPORT_TIFF_FILE)
	{
		gFile_Filter.tif_flg = 1;
	}
	if ( (nFilterFlag & MTP_SUPPORT_BMP_FILE) == MTP_SUPPORT_BMP_FILE)
	{
		gFile_Filter.bmp_flg = 1;
	}
	if ( (nFilterFlag & MTP_SUPPORT_GIF_FILE) == MTP_SUPPORT_GIF_FILE)
	{
		gFile_Filter.gif_flg = 1;
	}
	if ( (nFilterFlag & MTP_SUPPORT_MP3_FILE) == MTP_SUPPORT_MP3_FILE)
	{
		gFile_Filter.mp3_flg = 1;
	}
	if ( (nFilterFlag & MTP_SUPPORT_DPOF_FILE) == MTP_SUPPORT_DPOF_FILE)
	{
		gFile_Filter.dpof_flg = 1;
	}
	if ( (nFilterFlag & MTP_SUPPORT_MOV_FILE) == MTP_SUPPORT_MOV_FILE)
	{
		gFile_Filter.mov_flg = 1;
	}
	if ( (nFilterFlag & MTP_SUPPORT_MP4_FILE) == MTP_SUPPORT_MP4_FILE)
	{
		gFile_Filter.mp4_flg = 1;
	}
	return 0;
}

INT32 GetJpegPicInfo(UINT32* pThumbSize, UINT32* pThumbWidth, UINT32* pThumbHeight, UINT32* pPicWidth, UINT32* pPicHeight, char* pszDateCreated)
{
	FW_INT32 err = FJ_ERR_OK;
#ifdef CO_DCF_EXIF_ON
	T_BF_DCF_IF_IMAGE_DETAIL img_detail;
	T_BF_DCF_IF_LOAD_IMAGE_DATA load_image_data;
	char date[32];
	memset(date, 0, sizeof(date));
	memset(&img_detail, 0, sizeof(T_BF_DCF_IF_IMAGE_DETAIL));
	memset(&load_image_data, 0, sizeof(T_BF_DCF_IF_LOAD_IMAGE_DATA));
	MtpEnd_Flg = 1; //lock
	err = BF_Dcf_Exif_Analyze_Jpeg(PTP_fp, &load_image_data, &img_detail);
	MtpEnd_Flg = 0; //unlock
	if(err != 0)
	{
		return err;
	}
	if(pszDateCreated != NULL)
	{
		snprintf(date, sizeof(date), "%04d%02d%02d%c%02d%02d%02d", img_detail.DateTimeOriginal.ad_year, img_detail.DateTimeOriginal.month, img_detail.DateTimeOriginal.day, \
		0x54, img_detail.DateTimeOriginal.hour, img_detail.DateTimeOriginal.min, img_detail.DateTimeOriginal.sec);
	}
	else {
		return FJ_ERR_NG;
	}
	strcpy(pszDateCreated, (const char*)date);	
	*pThumbSize = load_image_data.thumb_size;
	if(load_image_data.thumb_size)
	{
		*pThumbWidth = WIDTH_THUMB;
		*pThumbHeight = LINES_THUMB;
	}
	else
	{
		*pThumbWidth = 0;
		*pThumbHeight = 0;
	}
	*pPicWidth = img_detail.width;
	*pPicHeight = img_detail.lines;
#endif
	return err;
}

#define MTP_AVI_TH_WIDTH      160
#define MTP_AVI_TH_LINES      120
		
INT32 GetAviInfo(UINT32* pThumbSize, UINT32* pThumbWidth, UINT32* pThumbHeight, UINT32* pAviWidth, UINT32* pAviHeight, char* szObjPath)
{
/*
	INT32	err = 0;
	T_MJPCTL_UI  movie_play_info;
	UINT32	file_format_type = MJP_FF_AVI;
	CHAR	file_path[MJP_FNAME_MAX];
	ULONG	play_total_time = 0;
	strcpy((char*)file_path, szObjPath);
	// Initialize Movie Control Data
	err = MJP_Usr_Dec_play_init();
	if( err != MOVIE_OK )
	{
		return err;
	}
	//gMjp_play_fid[table_index].file_id = file_id ;
	//gMjp_play_fid[table_index].file_index = 1;
	
	err = MJP_Ctl_Dec_play_top_get( PTP_fp, file_path, file_format_type, (CHAR*)NULL, &play_total_time, 0, MOVIE_OFF);
	if( err != MOVIE_OK )
	{
		return err;
	}
	err = MJP_Ctl_Dec_play_info_get(PTP_fp, &movie_play_info);
	if( err != MOVIE_OK )
	{
		return err;
	}	
	switch( movie_play_info.movieSize )
	{
		case MJP_SIZE_VGA :
			*pThumbWidth = *pAviWidth = WIDTH_VGA ;
			*pThumbHeight = *pAviHeight = LINES_VGA ;
			break ;
		case MJP_SIZE_VGA_16_9 :
			*pThumbWidth = *pAviWidth = WIDTH_VGA ;
			*pThumbHeight = *pAviHeight = LINES_VGA_16_9;
			break ;
		case MJP_SIZE_QVGA :
			*pThumbWidth = *pAviWidth = WIDTH_QVGA;
			*pThumbHeight = *pAviHeight = LINES_QVGA;
			break ;
		case MJP_SIZE_QQVGA :
			*pThumbWidth = *pAviWidth = WIDTH_THUMB;
			*pThumbHeight = *pAviHeight = LINES_THUMB;
			break ;
		default :
			break ;
	}
	*pThumbSize = movie_play_info.top_size;

	return err;
*/
	INT32	err = 0;
	char	file_path[MTP_MAX_PATH];
	T_BF_FS_IF_STAT file_stat;
	strncpy(file_path, szObjPath, sizeof(file_path));
	strncpy(file_path + strlen(file_path) - 3, "THM", sizeof(file_path)-strlen(file_path)+3);

	err = BF_Fs_If_Stat((const char*)file_path, &file_stat);
	if(err == FJ_ERR_OK)	
	{
		*pThumbSize = file_stat.fileSize;
		*pThumbWidth = *pAviWidth = MTP_AVI_TH_WIDTH ;
		*pThumbHeight = *pAviHeight = MTP_AVI_TH_LINES;
	}
	return err;			
}

#define MTP_MOV_TH_WIDTH      160
#define MTP_MOV_TH_LINES      120

INT32 GetMovInfo(UINT32* pThumbSize, UINT32* pThumbWidth, UINT32* pThumbHeight, UINT32* pAviWidth, UINT32* pAviHeight, char* szObjPath)
{
	INT32	err = 0;
	char	file_path[MTP_MAX_PATH];
//	ULONG	ulThumbAddr = 0;
//	ULONG	ulThumbSize = 0;
	INT32	iBackPTP_fp = 0;

	strncpy(file_path, szObjPath, sizeof(file_path));

	*pThumbWidth = *pAviWidth = MTP_MOV_TH_WIDTH ;
	*pThumbHeight = *pAviHeight = MTP_MOV_TH_LINES;

	if(PTP_fp != 0)
	{
		iBackPTP_fp = PTP_fp;
		BF_Fs_If_Close(PTP_fp);
		PTP_fp = 0;
	}

//	BF_Play_Common_Get_Movie_Top_Thumb((CHAR*)file_path, &ulThumbAddr, &ulThumbSize);
//	if(ulThumbSize > 0)
//	{
//		*pThumbSize = (UINT32)ulThumbSize;
//		err = FJ_ERR_OK;
//	}
//	else
//	{
		// Cannot get thumbnail information.
		*pThumbSize = 0;
		err = FJ_ERR_NG;
//	}
	
	if(iBackPTP_fp != 0)
	{
		PTP_fp = BF_Fs_If_Open(file_path, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
	}

	return err;
}

INT32 GetJpegThumbnail(char* szObjPath, FW_ULONG buf, ULONG size, FW_ULONG* length)
{
	FW_INT32 err = FJ_ERR_OK;
#ifdef CO_DCF_EXIF_ON
	T_BF_DCF_IF_IMAGE_LOAD_DATA image_load_data;
	T_BF_DCF_IF_LOAD_IMAGE_DATA load_image_data;
	T_BF_DCF_IF_IMAGE_DETAIL img_detail;
	T_BF_DCF_IF_FILE_INDEX dcf_file_index;
	FW_ULONG result = 0; 
	dcf_file_index.index = 0;
	image_load_data.file_index = dcf_file_index;
	image_load_data.img_addr = buf;
	image_load_data.img_size = size;
	image_load_data.file_type = E_BF_DCF_IF_FILE_TYPE_JPG;
	*length = 0;
	INT32			iBackPTP_fp = 0;

	if(PTP_fp != 0)
	{
		iBackPTP_fp = PTP_fp;
		BF_Fs_If_Close(PTP_fp);
		PTP_fp = 0;
	}

	// open file 
	
	err = PTP_fp = BF_Fs_If_Open(szObjPath, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
	if(PTP_fp == 0)
	{
		PTP_fp = 0; 
		return err;
	}
	MtpEnd_Flg = 1; //lock
	err = BF_Dcf_Exif_Analyze_Jpeg(PTP_fp, &load_image_data, &img_detail);	
	MtpEnd_Flg = 0; //unlock
	if(err != 0)
	{
		BF_Fs_If_Close(PTP_fp);
		/*  20120822 Add PTP_fp set to zero if PTP_fp is Close  */
		PTP_fp = 0;
		return err;
	}
	
	// buffer overflow 
	if(load_image_data.thumb_size > image_load_data.img_size)
	{
		BF_Fs_If_Close(PTP_fp);
		/*  20120822 Add PTP_fp set to zero if PTP_fp is Close  */
		PTP_fp = 0;
		err = FSDCF_ERRNO_THUMB_SIZE_OVER;
		return err;
	}

	// not exif format 
	if(img_detail.jpeg_format != E_BF_DCF_IF_JPEG_FORMAT_EXIF) {
		BF_Fs_If_Close(PTP_fp);
		/*  20120822 Add PTP_fp set to zero if PTP_fp is Close  */
		PTP_fp = 0;
		err = FSDCF_ERRNO_NOT_SUPPORT_FILE;
		return err;
	}
	
	// seek thumbnail top 
	err = BF_Fs_If_Lseek(PTP_fp, (FW_INT32)load_image_data.thumb_offset, D_BF_FS_IF_SEEK_SET, &result);
	if(err != 0)
	{
		BF_Fs_If_Close(PTP_fp);
		/*  20120822 Add PTP_fp set to zero if PTP_fp is Close  */
		PTP_fp = 0;
		return err;
	}
	
	// load thumbnail image 
	err = BF_Fs_If_Read(PTP_fp, (FW_VOID*)image_load_data.img_addr, load_image_data.thumb_size, &result);
	if(err != 0)
	{
		BF_Fs_If_Close(PTP_fp);
		/*  20120822 Add PTP_fp set to zero if PTP_fp is Close  */
		PTP_fp = 0;
		return err;
	}
	*length = result;
	// close file 
	err = BF_Fs_If_Close(PTP_fp);
	PTP_fp = 0;

	if(iBackPTP_fp != 0)
	{
		PTP_fp = BF_Fs_If_Open(szObjPath, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
	}
#endif

	return err;
}

INT32 GetAviThumbnail(char* szObjPath, FW_ULONG buf, ULONG size, FW_ULONG* length)
{
	FW_INT32 file_id = 0, err = FJ_ERR_OK;
	//FW_ULONG rlength = 0;
	char	file_path[MTP_MAX_PATH];

	FW_ULONG result = 0; //
	
	strncpy(file_path, szObjPath, sizeof(file_path));
	strncpy(file_path + strlen(file_path) - 3, "THM", sizeof(file_path)-strlen(file_path)+3);

	
	/*
	file_id = BF_Fs_If_Open( file_path, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL );
	if(!file_id)
	{
		err = FJ_ERR_NG;
		return err;
	}
	err = BF_Fs_If_Read(file_id, (FW_VOID*)buf, size, &rlength);
	BF_Fs_If_Close(file_id);
	*/
	err = file_id = BF_Fs_If_Open(file_path, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
	if(file_id == 0)
	{
		file_id = 0; 
		return err;
	}	
	err = BF_Fs_If_Read(file_id, (FW_VOID*)buf, size, &result);
	if(err != 0)
	{
		BF_Fs_If_Close(file_id);
		return err;
	}
	*length = result;
	/* close file */
	err = BF_Fs_If_Close(file_id);
	file_id = 0;	
	return err;
}


INT32 GetMovThumbnail(char* szObjPath, FW_ULONG buf, ULONG size, FW_ULONG* length)
{
	FW_INT32	err = FJ_ERR_OK;
	char		file_path[MTP_MAX_PATH];
//	ULONG		ulThumbAddr = 0;
//	ULONG		ulThumbSize = 0;
	int			iBackPTP_fp = 0;
	
	strncpy(file_path, szObjPath, sizeof(file_path));

	// buffer overflow 
//	if(ulThumbSize > (size - sizeof(UINT32)))
//	{
//		err = FSDCF_ERRNO_THUMB_SIZE_OVER;
//		return err;
//	}
	
	if(PTP_fp != 0)
	{
		iBackPTP_fp = PTP_fp;
		BF_Fs_If_Close(PTP_fp);
		PTP_fp = 0;
	}

//	BF_Play_Common_Get_Movie_Top_Thumb((CHAR*)file_path, &ulThumbAddr, &ulThumbSize);
//	if(ulThumbSize > 0)
//	{
//		memcpy((char*)buf, (char*)ulThumbAddr, ulThumbSize);
//		err = FJ_ERR_OK;
//	}
//	else
//	{
		// Cannot get thumbnail information.
		err = FJ_ERR_NG;
//	}
//
//	*length = ulThumbSize;
	*length = 0;

	if(iBackPTP_fp != 0)
	{
		PTP_fp = BF_Fs_If_Open(file_path, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
	}

	return err;
}

INT32 GetMovThumbnailAndNumElements(char* szObjPath, FW_ULONG buf, ULONG size, FW_ULONG* length)
{
	FW_INT32	err = FJ_ERR_OK;
	char		file_path[MTP_MAX_PATH];
//	ULONG		ulThumbAddr = 0;
	ULONG		ulThumbSize = 0;
	UCHAR		*Addr;
	int			iBackPTP_fp = 0;
	
	strncpy(file_path, szObjPath, sizeof(file_path));

	// buffer overflow 
//	if(ulThumbSize > (size - sizeof(UINT32)))
//	{
//		err = FSDCF_ERRNO_THUMB_SIZE_OVER;
//		return err;
//	}
	
	if(PTP_fp != 0)
	{
		iBackPTP_fp = PTP_fp;
		BF_Fs_If_Close(PTP_fp);
		PTP_fp = 0;
	}

	// Skip NumElements(4byte)
	buf = buf + sizeof(UINT32);

//	BF_Play_Common_Get_Movie_Top_Thumb((CHAR*)file_path, &ulThumbAddr, &ulThumbSize);
//	if(ulThumbSize > 0)
//	{
//		memcpy((char*)buf, (char*)ulThumbAddr, ulThumbSize);
//		err = FJ_ERR_OK;
//	}
//	else
//	{
		// Cannot get thumbnail information.
		err = FJ_ERR_NG;
//	}

	*length = sizeof(UINT32) + ulThumbSize;

	// Set NumElements(4byte)
	Addr = (UCHAR*)(buf - sizeof(UINT32));
	*Addr=(UCHAR)(ulThumbSize&0x000000FF);
	*(Addr+1)=(UCHAR)((ulThumbSize&0x0000FF00)>>8);
	*(Addr+2)=(UCHAR)((ulThumbSize&0x00FF0000)>>16);
	*(Addr+3)=(UCHAR)((ulThumbSize&0xFF000000)>>24);

	if(iBackPTP_fp != 0)
	{
		PTP_fp = BF_Fs_If_Open(file_path, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
	}

	return err;
}

INT32 xGetParentPath(FW_CHAR* szFullPath,FW_CHAR* szPathName)
{
	static FW_CHAR str = '\\';
	FW_CHAR szTempString[MTP_MAX_PATH];
	FW_CHAR *pDest;
	INT32 nStringLength;
	INT32 nPosition1=0;

	memset(szTempString,0,MTP_MAX_PATH);
	memset(szPathName,0,MTP_MAX_PATH);
	nStringLength = strlen(szFullPath);
	pDest  = strrchr(szFullPath,str);

	if (pDest == NULL)
		return 1;
	
	nPosition1 = pDest - szFullPath;

	if (nPosition1 == (nStringLength-1) )  // end character is '\\'
	{
		if (nStringLength-1 > 0)
			strncpy(szTempString,szFullPath,nStringLength-1);
		else
			return 1;
		
		pDest = strrchr(szTempString,str);

		if (pDest == NULL)
			return 1;
		
		if ( (pDest-szTempString) > 0)
			strncpy(szPathName,szFullPath,pDest-szTempString);
		else
			return 1;
	}
	else	// end character is not '\\'
	{
		pDest = strrchr(szFullPath,str);
		if (pDest == NULL) {
			return 1;
		}
		strncpy(szPathName,szFullPath,(pDest-szFullPath)-1);
		strncpy(szPathName,szFullPath,(pDest-szFullPath));
	}

	return 0;
}


INT32 xIsDcimFolder(FW_CHAR *szFolderName,BOOL *bIsDcimFolder)
{
	INT32 nFolderLen=0;
	FW_CHAR szTmpDcimPath[MTP_MAX_PATH];
	FW_CHAR szDcimFolder[MTP_MAX_PATH];
	FW_INT32 ercd;

	// Edit folder path to compare
	strcpy(szTmpDcimPath, szFolderName);
	*bIsDcimFolder = FALSE;

	nFolderLen = strlen(szTmpDcimPath);
	if( szTmpDcimPath[nFolderLen - 1] == '\\' )
	{
		szTmpDcimPath[nFolderLen - 1] = 0x00;
		nFolderLen = nFolderLen - 1;
	}

	// Edit DCIM folder path
	memset(szDcimFolder, 0, MTP_MAX_PATH);
	ercd = BF_Fs_If_Get_Cur_Drv(szDcimFolder);
	if (ercd != 0) {
		
		BF_Debug_Print_Error(("xIsDcimFolder(): Fail to get current drive.\n"));
		return ercd;
	}
	
	strncat(szDcimFolder, ":\\" DCIM_DIR_NAME, 2 + 4);
	
	// Compare
	if ((strcmp(szTmpDcimPath, szDcimFolder)) == 0)
	{
		*bIsDcimFolder = TRUE;
	}

	return 0;
}

INT32 xIsDCFFolderName(FW_CHAR *szFolderName,BOOL *bIsDCFFolder)
{
	*bIsDCFFolder = FALSE;
	
	if ( ( (szFolderName[0] >= '0') &&	(szFolderName[0]<='9') )  &&
     	 ( (szFolderName[1] >= '0') &&	(szFolderName[1]<='9') )  &&
     	 ( (szFolderName[2] >= '0') &&	(szFolderName[2]<='9') )  &&		 	
		 ( (szFolderName[3] >= 'A') &&	(szFolderName[3]<='Z') )  &&
		 ( (szFolderName[4] >= 'A') &&	(szFolderName[4]<='Z') )  &&
		 ( (szFolderName[5] >= 'A') &&	(szFolderName[5]<='Z') )  &&
		 ( (szFolderName[6] >= 'A') &&	(szFolderName[6]<='Z') )  &&
		 ( (szFolderName[7] >= 'A') &&	(szFolderName[7]<='Z') )  &&
		 ( strlen(szFolderName) == DCF_DIR_NAME_LENGTH )
	)
	{
		*bIsDCFFolder = TRUE;
	}
	else
	{
		*bIsDCFFolder = FALSE;
	}

	return 0;
}

INT32 xIsRootFolder(FW_CHAR *szRootFolderName,BOOL *bIsRootFolder)
{
	char cBackslash = '\\';
	char *szDest;
	INT32 nFolderLen=0;

	*bIsRootFolder = FALSE;

	szDest = strchr(szRootFolderName,cBackslash);
	
	if (szDest == NULL)
		*bIsRootFolder = TRUE;
	else
	{
		nFolderLen = strlen(szRootFolderName);

		if ( nFolderLen == (szDest-szRootFolderName+1) )
			*bIsRootFolder = TRUE;
	}

	return 0;
}

INT32 xIsFileInRootFolder(FW_CHAR *szRootFolderName,BOOL *bIsFileInRootFolder)
{
	char cBackslash = '\\';
	char cColon = ':';	
	char *szDest1;
	char *szDest2;

	*bIsFileInRootFolder = FALSE;

	szDest1 = strchr(szRootFolderName,cColon);
	szDest2 = strrchr(szRootFolderName,cBackslash);

	if ((szDest1 == NULL) || (szDest2 == NULL)) {
		return 1;
	}

	if ( (szDest2-szDest1) == 1)
	{
		*bIsFileInRootFolder = TRUE;
	}
	else
	{
		*bIsFileInRootFolder = FALSE;
	}	

	return 0;
}

INT32 xIsFileInDcimFolder(FW_CHAR *szFolderName,BOOL *bIsFileInDcimFolder)
{
	char cBackslash = '\\';
	char cColon = ':';	
	char *szDest1;
	char *szDest2;
	char szPathName[MTP_MAX_PATH];
	char szParentPath[MTP_MAX_PATH];
	INT32 nErrorCode=0;

	*bIsFileInDcimFolder = FALSE;

	szDest1 = strchr(szFolderName,cColon);
	szDest2 = strrchr(szFolderName,cBackslash);

	if ((szDest1 == NULL) || (szDest2 == NULL)) {
		return 1;
	}

	if ((szDest2-szDest1) == 1)
		return 0;
	
	nErrorCode = xGetParentPath(szFolderName,szParentPath);
	if (nErrorCode==1)
		return 1;
	nErrorCode = xGetPathName(szParentPath,szPathName);
	if (nErrorCode==1)
		return 1;
	
	if (strcmpi(szPathName,DCIM_DIR_NAME)==0)
	{
		*bIsFileInDcimFolder = TRUE;
	}
	else
	{
		*bIsFileInDcimFolder = FALSE;
	}

	return 0;
}

INT32 xIsDCFFolder(FW_CHAR *szDCFFolderName,BOOL *bIsDCFFolder)
{
	FW_CHAR szFileName[MTP_MAX_PATH];
	BOOL bInDcimFolder = FALSE;
	BOOL bIsDCFFolderName = FALSE;

	*bIsDCFFolder = FALSE;
	xIsFolderInDcimFolder(szDCFFolderName,&bInDcimFolder);

	if (bInDcimFolder == TRUE)
	{
		xGetPathName(szDCFFolderName,szFileName);
		xIsDCFFolderName(szFileName,&bIsDCFFolderName);

		if (bIsDCFFolderName == TRUE)
		{
			*bIsDCFFolder = TRUE;
		}
		else
		{
			*bIsDCFFolder = FALSE;
		}	
	}

	return 0;
}

INT32 xIsFileInDCFFolder(FW_CHAR *szDCFFolderName,BOOL *bIsDCFFolder)
{
	FW_CHAR szPathName[MTP_MAX_PATH];
	FW_CHAR szFileName[MTP_MAX_PATH];
	FW_CHAR szParentPath[MTP_MAX_PATH];
	FW_CHAR szDcimPath[MTP_MAX_PATH];
	BOOL    bIsDcimFolder;
	INT32 	nErrorCode=0;

	*bIsDCFFolder = FALSE;

	xGetPathName(szDCFFolderName,szPathName);
	xGetFileName(szDCFFolderName,szFileName);
	nErrorCode = xGetParentPath(szDCFFolderName,szParentPath);
	if (nErrorCode == 1)
		return 1;
	nErrorCode = xGetParentPath(szParentPath,szDcimPath);
	if (nErrorCode == 1)
		return 1;

	xIsDcimFolder(szDcimPath,&bIsDcimFolder);

	if (bIsDcimFolder == TRUE)
	{
		*bIsDCFFolder = TRUE;
	}
	
	return 0;
}

INT32 xIsFolderInDcimFolder(FW_CHAR *szDCFFolderName,BOOL *bIsDCFFolder)
{
	FW_CHAR szParentPath[MTP_MAX_PATH];
	BOOL    bIsDcimFolder;

	*bIsDCFFolder = FALSE;

	xGetParentPath(szDCFFolderName,szParentPath);
	xIsDcimFolder(szParentPath,&bIsDcimFolder);

	if (bIsDcimFolder == TRUE)
	{
		*bIsDCFFolder = TRUE;
	}
	
	return 0;
}

INT32 xIsSpecDCFFolder(FW_CHAR *szDCFFolderName,BOOL *bIsDCFFolder)
{
	char *szDest;

	*bIsDCFFolder = FALSE;

	szDest = strstr(szDCFFolderName,DCF_DIR_NAME);
	
	if (szDest != NULL)
		*bIsDCFFolder = TRUE;

	return 0;
}

INT32 xGetParentObjectHandle(const char* srcfilepath,INT32* ulParentObject)
{
	INT32 j=0;
	*ulParentObject = 0;

	for (j=1;j<=g_nObjectCnt;j++)
	{	 
		if(strcmpi(srcfilepath,(const char*)oMTPDBFileInfo[j].szDosFileName) == 0) 
		{
			*ulParentObject = oMTPDBFileInfo[j].nFileIndex;
		}
	}	

	return 0;
}

INT32 ResetMTPAllObjectInfo()
{
	E_BF_MEDIA_STATE_MEDIA_TYPE eMediaType = BF_Fs_If_Get_Cur_Media();	
//	FW_CHAR		szDirPath[MTP_MAX_PATH];
//	E_BF_MEDIA_STATE_MEDIA_TYPE mediaType;
//	INT32		ercd;

	memset(oMTPDBFileInfo,0,MTP_OBJECT_NUMBER*sizeof(T_MTP_DB_FILE_INFO));
	memset(g_szRootFolder,0,MTP_MAX_PATH);
	g_nObjectCnt=0;	

//	mediaType = BF_Fs_If_Get_Cur_Media();
//	ercd = BF_Media_State_Exchage_Type_To_Drive_Name(mediaType, szDirPath);
//	if (ercd != 0){
//		BF_Debug_Print_Error(("exchange type to drive name error!\n"));
//		return 1;
//	}	

	BF_Debug_Print_Information(("ResetMTPAllObjectInfo eMediaType = %d\r\n", eMediaType));
	MTP_GetAllObjectInfo(eMediaType);
	return 0;
}


INT32 MTP_GetAllObjectInfo(E_BF_MEDIA_STATE_MEDIA_TYPE mediaType)
{		
	INT32 	nObjectIndex=0;
	INT32 	nFolderIndex=0;
	INT32	ercd;
	INT32      nFilterFlag=0;
	short   Level, i;
	UINT32	hdlcnt=0;
	FW_INT32        dNo[NUM_DIR_LEVEL];  //dNo[0]: Root dir, dNo[1]: 1st level Dir(1), dNo[2]: 2nd level Dir
	FW_INT32 		  ndNo;
	T_BF_FS_IF_STAT  dir;

	FW_CHAR 	szTempString[MTP_MAX_PATH];
	FW_CHAR       szTempString2[MTP_MAX_PATH];
	FW_CHAR		DirPath[NUM_DIR_LEVEL][MTP_MAX_PATH];	
	FW_CHAR		szDirPath[MTP_MAX_PATH];	
	FW_CHAR		szCurDrv[D_BF_FS_IF_DRVNAME_MAX];

	BOOL bIsDcimFolder=FALSE;
	BOOL bIsRootFolder=FALSE;
	BOOL bIsDcfFolder=FALSE;
	BOOL bIsDcfFile=FALSE;
	BOOL bIsFileInRootFolder=FALSE;
	BOOL bIsFileInDcimFolder=FALSE;
	BOOL bIsFolderInDcimFolder=FALSE;

	memset(szCurDrv, 0, sizeof(szCurDrv));
	ercd = BF_Fs_If_Get_Cur_Drv(szCurDrv);
	if (ercd != 0) {
		BF_Debug_Print_Error(("MTP_GetAllObjectInfo(): Fail to get current drive.\n"));
		return 1;
	}

 	if (strlen(g_szRootFolder)==0)
 	{
		snprintf(szTempString, sizeof(szTempString), "%s:\\", szCurDrv);

		if (gFolder_Filter.Root_Folder_flg == 1)
			nFilterFlag |= ROOT_FOLDER_ENABLE;

		if (gFolder_Filter.Root_File_flg == 1)
			nFilterFlag |= ROOT_FILE_ENABLE;

		if (gFolder_Filter.DCIM_File_flg == 1)
			nFilterFlag |= DCIM_FILE_ENABLE;	
		
		if (gFolder_Filter.Set_DCF_DirName_flg == 1)
			nFilterFlag |= DCF_SET_FOLDER_NAME;

		if (gFolder_Filter.DCF_Folder_Flg == 1)
			nFilterFlag |= DCF_FILE_NAME_RULE_ENABLE;

		strcpy(szTempString2,g_szDCFDir);
		MTP_SetFolderFilter(szTempString,szTempString2,nFilterFlag);
 	}

	// Change Root directory
	snprintf(szTempString, sizeof(szTempString), "%s", szCurDrv);
	
	Level = 0;
	strcpy(DirPath[Level],g_szRootFolder);	
		
	xIsRootFolder(DirPath[Level],&bIsRootFolder); // for Root Entry File Filter	////
	xIsDcimFolder(DirPath[Level],&bIsDcimFolder); // for DCIM File Filter		////

	xIsFolderInDcimFolder(DirPath[Level],&bIsFolderInDcimFolder);		
	xIsDCFFolder(DirPath[Level],&bIsDcfFolder);
	
	ercd = BF_Fs_If_Opendir(DirPath[Level], &dNo[Level]);

	if(ercd != 0) 
	{
		return 0 ;
	}
	
	// Root Folder 
#ifdef MTP_FOLDER_SUPPORT	
	else if ( (bIsRootFolder == FALSE) && (gFolder_Filter.Root_Folder_flg == 1))
	{
		nObjectIndex++;
		g_nObjectCnt=nObjectIndex;
		
		oMTPDBFileInfo[nObjectIndex].bit.file_attr = MTP_FOLDER_TYPE;
		oMTPDBFileInfo[nObjectIndex].bit.file_exttype = MTP_FOLDER_TYPE;
		oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
		oMTPDBFileInfo[nObjectIndex].nParentIndex = nFolderIndex;
		strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,DirPath[Level],sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));
		nFolderIndex=nObjectIndex;
	}
#endif
	while(1)
	{
		ercd = BF_Fs_If_Readdir(dNo[Level], &dir) ;
		if (ercd == 1) 
		{
			/* Last entry in DCIM folder */
			ercd = BF_Fs_If_Closedir(dNo[Level]);
			if(ercd != 0) 
			{
				return(0);
			}
			Level--;

			xGetParentObjectHandle(DirPath[Level],&nFolderIndex);
			if (Level >= 0/*1*/)
				continue;
			else
				break;
		}
		else if (ercd < 0) 
		{
			/* error */
			for (i = Level; i >= 0; i--)
			{
				ercd = BF_Fs_If_Closedir(dNo[Level]);
				if (ercd != 0) 
				{
					return(0);
				}
			}
			return(0);
		}	

		if ((dir.attributes & 0x10) == 0x10)//Dir
		{
			// skip current & parent dir entry item
			if (dir.fname[0] == '.')
			{
				continue;
			}
			
			if (Level == (NUM_DIR_LEVEL-1))
				continue;
				
			Level++;

			memset(DirPath[Level], 0 , MTP_MAX_PATH);
			strncpy(DirPath[Level], DirPath[Level-1], sizeof(DirPath[Level]));
			strncpy(DirPath[Level] + strlen(DirPath[Level]), dir.fname, sizeof(DirPath[Level])-strlen(DirPath[Level]));
			strncpy(DirPath[Level] + strlen(DirPath[Level]), "\\", sizeof(DirPath[Level])-strlen(DirPath[Level]));

			xIsDcimFolder(DirPath[Level],&bIsDcimFolder);
			xIsFolderInDcimFolder(DirPath[Level],&bIsFolderInDcimFolder);
			xIsDCFFolder(DirPath[Level],&bIsDcfFolder);

			// skip another DCF Dir entry item
			if ((strcmp("DCIM", dir.fname) != 0) && (strcmp("MISC", dir.fname) != 0)
				&& (bIsFolderInDcimFolder == 0) && (bIsDcfFolder == 0))
			{
				Level--;
				continue;
			}

#ifdef MTP_FOLDER_SUPPORT
			if (hdlcnt < MTP_OBJECT_NUMBER)
			{				
				if ( ( bIsFolderInDcimFolder == FALSE)
					|| ( ((gFolder_Filter.Set_DCF_DirName_flg == 0) || ((gFolder_Filter.Set_DCF_DirName_flg == 1) && (strstr(DirPath[Level],g_szDCFDir) != NULL)) )
					        &&  ((gFolder_Filter.DCF_Folder_Flg == 0) || ((gFolder_Filter.DCF_Folder_Flg == 1) && (bIsDcfFolder== TRUE))) ) )
				{
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;			
		           		oMTPDBFileInfo[nObjectIndex].bit.file_attr = MTP_FOLDER_TYPE;
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype = MTP_FOLDER_TYPE;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = nFolderIndex;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,DirPath[Level],sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));
					nFolderIndex=nObjectIndex;
				}
			}
#endif			
			ercd = BF_Fs_If_Opendir(DirPath[Level], &dNo[Level]);
			if(ercd != 0) 
			{
				return 0 ;
			}			
			continue;
		}
		else
		{	// File -> Assign File Object Handle
			if (hdlcnt < MTP_OBJECT_NUMBER)
			{
				memset(szTempString,0,MTP_MAX_PATH);
				strncpy(szTempString, DirPath[Level], sizeof(szTempString));
				strncpy(szTempString + strlen(DirPath[Level]), dir.fname, sizeof(szTempString)-strlen(DirPath[Level])) ;				
				xIsFileInDCFFolder(szTempString,&bIsDcfFile);	
				xIsFileInRootFolder(szTempString,&bIsFileInRootFolder); 
				xIsFileInDcimFolder(szTempString,&bIsFileInDcimFolder);	

				if (     (xIsJpgFile(szTempString) == 0) && (gFile_Filter.jpg_flg == 1) 
					  	  && ((bIsDcfFolder== FALSE) || (gFolder_Filter.Set_DCF_DirName_flg == 0) ||
					          ((bIsDcfFolder== TRUE) && (strstr(szTempString,g_szDCFDir) != NULL) ) )
					  	  &&  ((bIsFileInDcimFolder == FALSE) || ( (gFolder_Filter.DCIM_File_flg == 1) && (bIsFileInDcimFolder == TRUE)) )       
					  	  &&  ((bIsFileInRootFolder == FALSE) || ( (gFolder_Filter.Root_File_flg == 1) && (bIsFileInRootFolder == TRUE)) )
						 &&  ( (bIsDcfFile == FALSE) || (gFolder_Filter.DCF_Folder_Flg == 0) || ( (bIsDcfFile == TRUE) && (gFolder_Filter.DCF_Folder_Flg == 1) && (bIsDcfFolder == TRUE)) )	
					 )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype 	= MTP_FILE_TYPE_JPG;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = nFolderIndex;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}

				if (     (xIsBmpFile(szTempString) == 0) && (gFile_Filter.bmp_flg == 1) 
					  	  && ((bIsDcfFolder== FALSE) || (gFolder_Filter.Set_DCF_DirName_flg == 0) ||
					          ((bIsDcfFolder== TRUE) && (strstr(szTempString,g_szDCFDir) != NULL) ) )
					  	  &&  ((bIsFileInDcimFolder == FALSE) || ( (gFolder_Filter.DCIM_File_flg == 1) && (bIsFileInDcimFolder == TRUE)) )       
					  	  &&  ((bIsFileInRootFolder == FALSE) || ( (gFolder_Filter.Root_File_flg == 1) && (bIsFileInRootFolder == TRUE)) )
						 &&  ( (bIsDcfFile == FALSE) || (gFolder_Filter.DCF_Folder_Flg == 0) || ( (bIsDcfFile == TRUE) && (gFolder_Filter.DCF_Folder_Flg == 1) && (bIsDcfFolder == TRUE)) )	
					 )				
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype 	= MTP_FILE_TYPE_BMP;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = nFolderIndex;					
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}

				if (     (xIsGifFile(szTempString) == 0) && (gFile_Filter.gif_flg == 1) 
					  	  && ((bIsDcfFolder== FALSE) || (gFolder_Filter.Set_DCF_DirName_flg == 0) ||
					          ((bIsDcfFolder== TRUE) && (strstr(szTempString,g_szDCFDir) != NULL) ) )
					  	  &&  ((bIsFileInDcimFolder == FALSE) || ( (gFolder_Filter.DCIM_File_flg == 1) && (bIsFileInDcimFolder == TRUE)) )       
					  	  &&  ((bIsFileInRootFolder == FALSE) || ( (gFolder_Filter.Root_File_flg == 1) && (bIsFileInRootFolder == TRUE)) )
						 &&  ( (bIsDcfFile == FALSE) || (gFolder_Filter.DCF_Folder_Flg == 0) || ( (bIsDcfFile == TRUE) && (gFolder_Filter.DCF_Folder_Flg == 1) && (bIsDcfFolder == TRUE)) )	
					 )				
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype 	= MTP_FILE_TYPE_GIF;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = nFolderIndex;					
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}

				if (     (xIsTiffFile(szTempString) == 0) && (gFile_Filter.tif_flg == 1) 
					  	  && ((bIsDcfFolder== FALSE) || (gFolder_Filter.Set_DCF_DirName_flg == 0) ||
					          ((bIsDcfFolder== TRUE) && (strstr(szTempString,g_szDCFDir) != NULL) ) )
					  	  &&  ((bIsFileInDcimFolder == FALSE) || ( (gFolder_Filter.DCIM_File_flg == 1) && (bIsFileInDcimFolder == TRUE)) )       
					  	  &&  ((bIsFileInRootFolder == FALSE) || ( (gFolder_Filter.Root_File_flg == 1) && (bIsFileInRootFolder == TRUE)) )
						 &&  ( (bIsDcfFile == FALSE) || (gFolder_Filter.DCF_Folder_Flg == 0) || ( (bIsDcfFile == TRUE) && (gFolder_Filter.DCF_Folder_Flg == 1) && (bIsDcfFolder == TRUE)) )	
					 )				
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;				
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype 	= MTP_FILE_TYPE_TIFF;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = nFolderIndex;					
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}

				if (     (xIsAviFile(szTempString) == 0) && (gFile_Filter.avi_flg == 1) 
					  	  && ((bIsDcfFolder== FALSE) || (gFolder_Filter.Set_DCF_DirName_flg == 0) ||
					          ((bIsDcfFolder== TRUE) && (strstr(szTempString,g_szDCFDir) != NULL) ) )
					  	  &&  ((bIsFileInDcimFolder == FALSE) || ( (gFolder_Filter.DCIM_File_flg == 1) && (bIsFileInDcimFolder == TRUE)) )       
					  	  &&  ((bIsFileInRootFolder == FALSE) || ( (gFolder_Filter.Root_File_flg == 1) && (bIsFileInRootFolder == TRUE)) )
						 &&  ( (bIsDcfFile == FALSE) || (gFolder_Filter.DCF_Folder_Flg == 0) || ( (bIsDcfFile == TRUE) && (gFolder_Filter.DCF_Folder_Flg == 1) && (bIsDcfFolder == TRUE)) )	
					 )					 
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype 	= MTP_FILE_TYPE_AVI;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = nFolderIndex;					
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}				

				if (     (xIsWavFile(szTempString) == 0) && (gFile_Filter.wav_flg == 1) 
					  	  && ((bIsDcfFolder== FALSE) || (gFolder_Filter.Set_DCF_DirName_flg == 0) ||
					          ((bIsDcfFolder== TRUE) && (strstr(szTempString,g_szDCFDir) != NULL) ) )
					  	  &&  ((bIsFileInDcimFolder == FALSE) || ( (gFolder_Filter.DCIM_File_flg == 1) && (bIsFileInDcimFolder == TRUE)) )       
					  	  &&  ((bIsFileInRootFolder == FALSE) || ( (gFolder_Filter.Root_File_flg == 1) && (bIsFileInRootFolder == TRUE)) )
						 &&  ( (bIsDcfFile == FALSE) || (gFolder_Filter.DCF_Folder_Flg == 0) || ( (bIsDcfFile == TRUE) && (gFolder_Filter.DCF_Folder_Flg == 1) && (bIsDcfFolder == TRUE)) )	
					 )					 
				{
					hdlcnt++;
					//objhdl = hdlcnt;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype 	= MTP_FILE_TYPE_WAV;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = nFolderIndex;					
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}	
	
				if (     (xIsMp3File(szTempString) == 0) && (gFile_Filter.mp3_flg == 1) 
					  	  && ((bIsDcfFolder== FALSE) || (gFolder_Filter.Set_DCF_DirName_flg == 0) ||
					          ((bIsDcfFolder== TRUE) && (strstr(szTempString,g_szDCFDir) != NULL) ) )
					  	  &&  ((bIsFileInDcimFolder == FALSE) || ( (gFolder_Filter.DCIM_File_flg == 1) && (bIsFileInDcimFolder == TRUE)) )       
					  	  &&  ((bIsFileInRootFolder == FALSE) || ( (gFolder_Filter.Root_File_flg == 1) && (bIsFileInRootFolder == TRUE)) )	
						 &&  ( (bIsDcfFile == FALSE) || (gFolder_Filter.DCF_Folder_Flg == 0) || ( (bIsDcfFile == TRUE) && (gFolder_Filter.DCF_Folder_Flg == 1) && (bIsDcfFolder == TRUE)) )	
					 )					 
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype 	= MTP_FILE_TYPE_MP3;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = nFolderIndex;					
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));					
				}	

				if (     (xIsMovFile(szTempString) == 0) && (gFile_Filter.mov_flg == 1) 
					  	  && ((bIsDcfFolder== FALSE) || (gFolder_Filter.Set_DCF_DirName_flg == 0) ||
					          ((bIsDcfFolder== TRUE) && (strstr(szTempString,g_szDCFDir) != NULL) ) )
					  	  &&  ((bIsFileInDcimFolder == FALSE) || ( (gFolder_Filter.DCIM_File_flg == 1) && (bIsFileInDcimFolder == TRUE)) )       
					  	  &&  ((bIsFileInRootFolder == FALSE) || ( (gFolder_Filter.Root_File_flg == 1) && (bIsFileInRootFolder == TRUE)) )	
						 &&  ( (bIsDcfFile == FALSE) || (gFolder_Filter.DCF_Folder_Flg == 0) || ( (bIsDcfFile == TRUE) && (gFolder_Filter.DCF_Folder_Flg == 1) && (bIsDcfFolder == TRUE)) )	
					 )					 
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype 	= MTP_FILE_TYPE_MOV;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = nFolderIndex;					
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));
				}

				if ( ( xIsMp4File(szTempString) == 0 ) && ( gFile_Filter.mp4_flg == 1 )
					&& ( ( bIsDcfFolder== FALSE ) || ( gFolder_Filter.Set_DCF_DirName_flg == 0 ) || ( ( bIsDcfFolder == TRUE ) && ( strstr( szTempString,g_szDCFDir) != NULL ) ) )
					&& ( ( bIsFileInDcimFolder == FALSE ) || ( ( gFolder_Filter.DCIM_File_flg == 1 ) && ( bIsFileInDcimFolder == TRUE ) ) )
					&& ( ( bIsFileInRootFolder == FALSE ) || ( ( gFolder_Filter.Root_File_flg == 1 ) && ( bIsFileInRootFolder == TRUE ) ) )
					&& ( ( bIsDcfFile == FALSE ) || ( gFolder_Filter.DCF_Folder_Flg == 0 ) || ( ( bIsDcfFile == TRUE ) && ( gFolder_Filter.DCF_Folder_Flg == 1 ) && ( bIsDcfFolder == TRUE ) ) )
					)
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_MP4;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = nFolderIndex;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));
				}

				if (     (xIsDpofFile(szTempString) == 0) && (gFile_Filter.dpof_flg == 1) 
					  	  && ((bIsDcfFolder== FALSE) || (gFolder_Filter.Set_DCF_DirName_flg == 0) ||
					          ((bIsDcfFolder== TRUE) && (strstr(szTempString,g_szDCFDir) != NULL) ) )
					  	  &&  ((bIsFileInDcimFolder == FALSE) || ( (gFolder_Filter.DCIM_File_flg == 1) && (bIsFileInDcimFolder == TRUE)) )       
					  	  &&  ((bIsFileInRootFolder == FALSE) || ( (gFolder_Filter.Root_File_flg == 1) && (bIsFileInRootFolder == TRUE)) )	
						 &&  ( (bIsDcfFile == FALSE) || (gFolder_Filter.DCF_Folder_Flg == 0) || ( (bIsDcfFile == TRUE) && (gFolder_Filter.DCF_Folder_Flg == 1) && (bIsDcfFolder == TRUE)) )	
					 )					 
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype 	= MTP_FILE_TYPE_DPOF;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = nFolderIndex;					
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));					
				}	

			}
		}
	};

	Level++;//Level -> 0 again
	
	if ( (bIsRootFolder == FALSE) && (gFolder_Filter.Root_File_flg == 1) )
	{
		snprintf(szDirPath, sizeof(szDirPath), "%s:\\", szCurDrv);

		ercd = BF_Fs_If_Opendir(szDirPath, &ndNo);
		while(1)
		{
			ercd = BF_Fs_If_Readdir(ndNo, &dir) ;
			if (ercd == 1) 
			{
				ercd = BF_Fs_If_Closedir(ndNo);
				break;
			}

			if ((dir.attributes & 0x10) != 0x10)//Not Dir				
			{
				strncpy(szTempString, szDirPath, sizeof(szTempString));
				strncpy(szTempString + strlen(szDirPath), dir.fname, sizeof(szTempString)-strlen(szDirPath)) ;								
			
				if (  (xIsJpgFile(szTempString) == 0) && (gFile_Filter.jpg_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_JPG;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}
				else if (  (xIsBmpFile(szTempString) == 0) && (gFile_Filter.bmp_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_BMP;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}
				else if (  (xIsGifFile(szTempString) == 0) && (gFile_Filter.gif_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_GIF;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}	
				else if (  (xIsTiffFile(szTempString) == 0) && (gFile_Filter.tif_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_TIFF;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}
				else if (  (xIsWavFile(szTempString) == 0) && (gFile_Filter.wav_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_WAV;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}
				else if (  (xIsMp3File(szTempString) == 0) && (gFile_Filter.mp3_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_MP3;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}
				else if (  (xIsAviFile(szTempString) == 0) && (gFile_Filter.avi_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_AVI;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}					
				else if (  (xIsMovFile(szTempString) == 0) && (gFile_Filter.mov_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_MOV;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}
				else if (  (xIsMp4File(szTempString) == 0) && (gFile_Filter.mp4_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_MP4;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));
				}
				else if (  (xIsDpofFile(szTempString) == 0) && (gFile_Filter.dpof_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_DPOF;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}
			}
		}
	}

	if ( (bIsRootFolder == FALSE) && (bIsDcimFolder == FALSE) && (gFolder_Filter.DCIM_File_flg== 1)) 
	{
		snprintf(szDirPath, sizeof(szDirPath), "%s:\\" DCIM_DIR_NAME "\\", szCurDrv);
		
		ercd = BF_Fs_If_Opendir(szDirPath, &ndNo);
		while(1)
		{
			ercd = BF_Fs_If_Readdir(ndNo, &dir) ;
			if (ercd == 1) 
			{
				ercd = BF_Fs_If_Closedir(ndNo);
				break;
			}

			if ((dir.attributes & 0x10) != 0x10)//Not Dir
			{
				strncpy(szTempString, szDirPath, sizeof(szTempString));
				strncpy(szTempString + strlen(szDirPath), dir.fname, sizeof(szTempString)-strlen(szDirPath)) ;				

				if (  (xIsJpgFile(szTempString) == 0) && (gFile_Filter.jpg_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_JPG;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}
				else if (  (xIsBmpFile(szTempString) == 0) && (gFile_Filter.bmp_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_BMP;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}
				else if (  (xIsGifFile(szTempString) == 0) && (gFile_Filter.gif_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_GIF;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}
				else if (  (xIsTiffFile(szTempString) == 0) && (gFile_Filter.tif_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_TIFF;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}
				else if (  (xIsWavFile(szTempString) == 0) && (gFile_Filter.wav_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_WAV;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}
				else if (  (xIsMp3File(szTempString) == 0) && (gFile_Filter.mp3_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_MP3;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}
				else if (  (xIsAviFile(szTempString) == 0) && (gFile_Filter.avi_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_AVI;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}					
				else if (  (xIsMovFile(szTempString) == 0) && (gFile_Filter.mov_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_MOV;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}
				else if (  (xIsMp4File(szTempString) == 0) && (gFile_Filter.mp4_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_MP4;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));
				}
				else if (  (xIsDpofFile(szTempString) == 0) && (gFile_Filter.dpof_flg == 1) )
				{
					hdlcnt++;
					nObjectIndex++;
					g_nObjectCnt=nObjectIndex;					
					oMTPDBFileInfo[nObjectIndex].bit.file_attr		= MTP_FILE_TYPE;						
					oMTPDBFileInfo[nObjectIndex].bit.file_exttype	= MTP_FILE_TYPE_DPOF;
					oMTPDBFileInfo[nObjectIndex].nFileIndex = nObjectIndex;
					oMTPDBFileInfo[nObjectIndex].nParentIndex = 0;
					strncpy(oMTPDBFileInfo[nObjectIndex].szDosFileName,szTempString,sizeof(oMTPDBFileInfo[nObjectIndex].szDosFileName));						
				}
			}
		}
	}

	return 0;
}


//INT32 MTP_GetObjectHandle(E_BF_MEDIA_STATE_MEDIA_TYPE mediaType, USHORT ObjectFormatCode,USHORT nParentHandle, UINT32 *ptr)
INT32 MTP_GetObjectHandle(USHORT ObjectFormatCode,USHORT nParentHandle, UINT32 *ptr)
{
	ULONG 			i;
	UINT32			hdlcnt=0;
	WORD			wObjectFormatCode = 0;
	FW_CHAR 		szParentName[MTP_MAX_PATH];
	
	if ((ObjectFormatCode == PTP_OFC_AllPicture) || (ObjectFormatCode == PTP_OFC_None) || (ObjectFormatCode == PTP_OFC_EXIFJPEG) || (ObjectFormatCode == PTP_OFC_JPEG) ||
		(ObjectFormatCode == MTP_OFC_AVI)	|| (ObjectFormatCode == MTP_OFC_ASSOCIATION)  || (ObjectFormatCode == MTP_OFC_WAV) || (ObjectFormatCode == MTP_OFC_MP3) 
		|| (ObjectFormatCode == MTP_OFC_BMP) || (ObjectFormatCode == MTP_OFC_GIF) 
		|| (ObjectFormatCode == MTP_OFC_TIFF) || (ObjectFormatCode == MTP_OFC_DPOF)  || (ObjectFormatCode == MTP_OFC_MOV) || (ObjectFormatCode == MTP_OFC_MP4) )
	{
		if (nParentHandle == 0xFFFF)
		{
			wObjectFormatCode = 0x3000;
			for (i=1;i<=g_nObjectCnt;i++)
			{
				if (oMTPDBFileInfo[i].nParentIndex == 0)
				{
					if (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE)
						wObjectFormatCode = MTP_OFC_ASSOCIATION;
					else if (xIsJpgFile(oMTPDBFileInfo[i].szDosFileName) == 0)
						wObjectFormatCode = MTP_OFC_EXIFJPEG;
					else if (xIsBmpFile(oMTPDBFileInfo[i].szDosFileName) == 0)
						wObjectFormatCode = MTP_OFC_BMP;
					else if (xIsGifFile(oMTPDBFileInfo[i].szDosFileName) == 0)
						wObjectFormatCode = MTP_OFC_GIF;
					else if (xIsTiffFile(oMTPDBFileInfo[i].szDosFileName) == 0)
						wObjectFormatCode = MTP_OFC_TIFF;
					else if (xIsAviFile(oMTPDBFileInfo[i].szDosFileName) == 0)
						wObjectFormatCode = MTP_OFC_AVI;
					else if (xIsWavFile(oMTPDBFileInfo[i].szDosFileName) == 0)
						wObjectFormatCode = MTP_OFC_WAV;
					else if (xIsMp3File(oMTPDBFileInfo[i].szDosFileName) == 0)
						wObjectFormatCode = MTP_OFC_MP3;
					else if (xIsDpofFile(oMTPDBFileInfo[i].szDosFileName) == 0)
						wObjectFormatCode = MTP_OFC_DPOF;
					else if (xIsMovFile(oMTPDBFileInfo[i].szDosFileName) == 0)
						wObjectFormatCode = MTP_OFC_MOV;
					else if (xIsMp4File(oMTPDBFileInfo[i].szDosFileName) == 0)
						wObjectFormatCode = MTP_OFC_MP4;
						
					if ((ObjectFormatCode == PTP_OFC_AllPicture) || (ObjectFormatCode == PTP_OFC_None) || (ObjectFormatCode == PTP_OFC_JPEG) ||
						( (ObjectFormatCode == PTP_OFC_EXIFJPEG) && (wObjectFormatCode == PTP_OFC_EXIFJPEG) ) || 					
						( (ObjectFormatCode == MTP_OFC_AVI)  && (wObjectFormatCode == MTP_OFC_AVI) ) || 
						( (ObjectFormatCode == MTP_OFC_WAV)  && (wObjectFormatCode == MTP_OFC_WAV) ) || 					
						( (ObjectFormatCode == MTP_OFC_ASSOCIATION)  && (wObjectFormatCode == MTP_OFC_ASSOCIATION) ) ||
						( (ObjectFormatCode == MTP_OFC_MP3)  && (wObjectFormatCode == MTP_OFC_MP3) ) ||
						( (ObjectFormatCode == MTP_OFC_MOV)  && (wObjectFormatCode == MTP_OFC_MOV) ) ||
						( (ObjectFormatCode == MTP_OFC_MP4)  && (wObjectFormatCode == MTP_OFC_MP4) ) ||
						( (ObjectFormatCode == MTP_OFC_DPOF)  && (wObjectFormatCode == MTP_OFC_DPOF) ) ||
						( (ObjectFormatCode == MTP_OFC_BMP)  && (wObjectFormatCode == MTP_OFC_BMP) ) ||
						( (ObjectFormatCode == MTP_OFC_GIF)  && (wObjectFormatCode == MTP_OFC_GIF) ) ||
						( (ObjectFormatCode == MTP_OFC_TIFF)  && (wObjectFormatCode == MTP_OFC_TIFF) ) )						
					{
						if (oMTPDBFileInfo[i].bit.fDelete != 1)
						{
							hdlcnt++;
							if (ptr)
							{					
								*(ptr++) = ENDIAN_SWAP32(oMTPDBFileInfo[i].nFileIndex);	// pgr0689
							}			
						}
					}
				}

			}
		}
		else if (nParentHandle == 0x0000)
		{
			for (i=1;i<=g_nObjectCnt;i++)
			{


				if (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE)
					wObjectFormatCode = MTP_OFC_ASSOCIATION;
				else if (xIsJpgFile(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_EXIFJPEG;
				else if (xIsBmpFile(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_BMP;
				else if (xIsGifFile(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_GIF;
				else if (xIsTiffFile(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_TIFF;
				else if (xIsAviFile(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_AVI;
				else if (xIsWavFile(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_WAV;
				else if (xIsMp3File(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_MP3;
				else if (xIsDpofFile(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_DPOF;
				else if (xIsMovFile(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_MOV;
				else if (xIsMp4File(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_MP4;
					
				if ( ( (ObjectFormatCode == PTP_OFC_AllPicture) || (ObjectFormatCode == PTP_OFC_None)) ||
						( (ObjectFormatCode == MTP_OFC_ASSOCIATION) && (wObjectFormatCode ==  MTP_OFC_ASSOCIATION) ) ||
						( (ObjectFormatCode == MTP_OFC_MP3) && (wObjectFormatCode 	== MTP_OFC_MP3) ) ||
						( (ObjectFormatCode == MTP_OFC_BMP) && (wObjectFormatCode 	== MTP_OFC_BMP) ) ||						
						( (ObjectFormatCode == MTP_OFC_EXIFJPEG) && (wObjectFormatCode 	== MTP_OFC_EXIFJPEG) ) ||						
						( (ObjectFormatCode == MTP_OFC_AVI) && (wObjectFormatCode 	== MTP_OFC_AVI) ) ||						
						( (ObjectFormatCode == MTP_OFC_MOV) && (wObjectFormatCode 	== MTP_OFC_MOV) ) ||
						( (ObjectFormatCode == MTP_OFC_MP4) && (wObjectFormatCode 	== MTP_OFC_MP4) ) ||
						( (ObjectFormatCode == MTP_OFC_DPOF) && (wObjectFormatCode 	== MTP_OFC_DPOF) ) ||
						( (ObjectFormatCode == MTP_OFC_WAV) && (wObjectFormatCode 	== MTP_OFC_WAV) ) ||						
						( (ObjectFormatCode == MTP_OFC_TIFF) && (wObjectFormatCode 	== MTP_OFC_TIFF) ) ||						
						( (ObjectFormatCode == MTP_OFC_GIF) && (wObjectFormatCode 	== MTP_OFC_GIF) ) ) 
				{
					if (oMTPDBFileInfo[i].bit.fDelete != 1)
					{
						hdlcnt++;
						if (ptr)
						{					
							*(ptr++) = ENDIAN_SWAP32(oMTPDBFileInfo[i].nFileIndex);	// pgr0689
						}			
					}
				}			
			}			
		}
		else if ((nParentHandle != 0x0000) && (nParentHandle != 0xFFFF))
		{

			for (i=1;i<=g_nObjectCnt;i++)
			{
				if (oMTPDBFileInfo[i].nFileIndex == nParentHandle)
				{
					strncpy(szParentName,(const char*)oMTPDBFileInfo[i].szDosFileName, sizeof(szParentName));
				}			
			}

			for (i=1;i<=g_nObjectCnt;i++)
			{
				if (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE)
					wObjectFormatCode = MTP_OFC_ASSOCIATION;
				else if (xIsJpgFile(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_EXIFJPEG;
				else if (xIsBmpFile(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_BMP;
				else if (xIsGifFile(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_GIF;
				else if (xIsTiffFile(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_TIFF;
				else if (xIsAviFile(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_AVI;
				else if (xIsWavFile(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_WAV;
				else if (xIsMp3File(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_MP3;
				else if (xIsMovFile(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_MOV;
				else if (xIsMp4File(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_MP4;
				else if (xIsDpofFile(oMTPDBFileInfo[i].szDosFileName) == 0)
					wObjectFormatCode = MTP_OFC_DPOF;
					
				if ( (oMTPDBFileInfo[i].nParentIndex == nParentHandle) &&
					  ( ( (ObjectFormatCode == PTP_OFC_AllPicture) || (ObjectFormatCode == PTP_OFC_None)) ||
						( (ObjectFormatCode == MTP_OFC_ASSOCIATION) && (wObjectFormatCode ==  MTP_OFC_ASSOCIATION) ) ||
						( (ObjectFormatCode == MTP_OFC_MP3) && (wObjectFormatCode==  MTP_OFC_MP3) ) ||
						( (ObjectFormatCode == MTP_OFC_BMP) && (wObjectFormatCode ==  MTP_OFC_BMP) ) ||
						( (ObjectFormatCode == MTP_OFC_EXIFJPEG) && (wObjectFormatCode ==  MTP_OFC_EXIFJPEG) ) ||
						( (ObjectFormatCode == MTP_OFC_AVI) && (wObjectFormatCode ==  MTP_OFC_AVI) ) ||
						( (ObjectFormatCode == MTP_OFC_MOV) && (wObjectFormatCode==  MTP_OFC_MOV) ) ||
						( (ObjectFormatCode == MTP_OFC_MP4) && (wObjectFormatCode==  MTP_OFC_MP4) ) ||
						( (ObjectFormatCode == MTP_OFC_DPOF) && (wObjectFormatCode==  MTP_OFC_DPOF) ) ||
						( (ObjectFormatCode == MTP_OFC_WAV) && (wObjectFormatCode ==  MTP_OFC_WAV) ) ||
						( (ObjectFormatCode == MTP_OFC_TIFF) && (wObjectFormatCode ==  MTP_OFC_TIFF) ) ||
						( (ObjectFormatCode == MTP_OFC_GIF) && (wObjectFormatCode ==  MTP_OFC_GIF) )))
				{

					if (oMTPDBFileInfo[i].bit.fDelete != 1)
					{
						hdlcnt++;
						if (ptr)
						{					
							*(ptr++) = ENDIAN_SWAP32(oMTPDBFileInfo[i].nFileIndex);	// pgr0689
						}			
					}
				}
			}
		}
	}		
//	else if(ObjectFormatCode == PTP_OFC_DPOF)
//	{
//		if(gBF_Ptp_Buff->Object_DPOF.DcfHandle != 0)
//		{
//			hdlcnt++;
//			if (ptr)
//			{
//				*(ptr++) = ENDIAN_SWAP32(gBF_Ptp_Buff->Object_DPOF.DcfHandle);
//			}
//		}
//	}	
	return hdlcnt;
}


int xAddObjectPorpValueListNoValue(UINT32 dwObjectHandle,UINT16 wOperationCode,UINT16 wDataType,ULONG *ulSize)
{
    ULONG  ulTempValue = 0;
   	USHORT usTempValue = 0;
	UCHAR			*Addr;	

	ulTempValue = dwObjectHandle;

	Addr = IN_Buffers+(*ulSize);
	*Addr=(UCHAR)(ulTempValue&0x000000FF);
	*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
	*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
	*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);	
	*ulSize += sizeof(UINT32);

	usTempValue = wOperationCode;;	
	Addr = IN_Buffers+(*ulSize);	
	*Addr=(UCHAR)(usTempValue&0x00FF);
	*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);	
	*ulSize += sizeof(UINT16);
	
	usTempValue = wDataType;
	Addr = IN_Buffers+(*ulSize);	
	*Addr=(UCHAR)(usTempValue&0x00FF);
	*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);
	*ulSize += sizeof(UINT16);	

	return 0;
}

int xAddObjectPorpValueListForVarLenString(UINT32 dwObjectHandle, UINT16 wOperationCode, UINT16 wDataType, VOID* pValue, ULONG *ulSize, UINT32 nStringlength)
{
    ULONG  ulTempValue = 0;
   	USHORT usTempValue = 0;
	UCHAR			*Addr;	

	ulTempValue = dwObjectHandle;

	Addr = IN_Buffers+(*ulSize);
	*Addr=(UCHAR)(ulTempValue&0x000000FF);
	*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
	*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
	*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);	
	*ulSize += sizeof(UINT32);

	usTempValue = wOperationCode;;	
	Addr = IN_Buffers+(*ulSize);	
	*Addr=(UCHAR)(usTempValue&0x00FF);
	*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);	
	*ulSize += sizeof(UINT16);
	
	usTempValue = wDataType;
	Addr = IN_Buffers+(*ulSize);	
	*Addr=(UCHAR)(usTempValue&0x00FF);
	*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);
	*ulSize += sizeof(UINT16);	
	switch (wDataType)
	{
		case MTP_DATATYPE_UINT16:
			memcpy(&usTempValue,pValue,sizeof(UINT16));
			Addr = IN_Buffers+(*ulSize);	
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);								
			*ulSize += sizeof(UINT16);
		break;
	
		case MTP_DATATYPE_UINT32:
			memcpy(&ulTempValue,pValue,sizeof(UINT32));
			Addr = IN_Buffers+(*ulSize);	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);	
			*ulSize += sizeof(UINT32);
			
		break;
		
		case MTP_DATATYPE_UINT64:
			memcpy(&ulTempValue,pValue,sizeof(UINT32));
			memset(IN_Buffers+(*ulSize),0,sizeof(UINT32)*2);
			Addr = IN_Buffers+(*ulSize);	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			*ulSize += sizeof(UINT32)*2;	
		break;		

		case MTP_DATATYPE_UINT128:
			memcpy(&ulTempValue,pValue,sizeof(UINT32));
			memset(IN_Buffers+(*ulSize),0,sizeof(UINT32)*4);
			Addr = IN_Buffers+(*ulSize);	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			*ulSize += sizeof(UINT32)*4;				
		break;			

		case MTP_DATATYPE_STR:
			IN_Buffers[(*ulSize)] = nStringlength;
			memcpy(IN_Buffers+(*ulSize)+1,pValue,nStringlength*2);
			//Addr = pValue;
			//Addr = IN_Buffers+(*ulOffset)+1;	
			//for(i=0;i<DOS_FILENAME_LEN*2;i++)
			//{
			//	*Addr=(*pValue);
			//	Addr++;
			//}			
			(*ulSize) += nStringlength*2+1;
		break;		
	}
	return 0;
}


int xAddObjectPorpValueList(UINT32 dwObjectHandle,UINT16 wOperationCode,UINT16 wDataType,VOID* pValue,ULONG *ulSize)
{
    ULONG  ulTempValue = 0;
   	USHORT usTempValue = 0;
	UCHAR			*Addr;	

	ulTempValue = dwObjectHandle;

	Addr = IN_Buffers+(*ulSize);
	*Addr=(UCHAR)(ulTempValue&0x000000FF);
	*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
	*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
	*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);	
	*ulSize += sizeof(UINT32);

	usTempValue = wOperationCode;;	
	Addr = IN_Buffers+(*ulSize);	
	*Addr=(UCHAR)(usTempValue&0x00FF);
	*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);	
	*ulSize += sizeof(UINT16);
	
	usTempValue = wDataType;
	Addr = IN_Buffers+(*ulSize);	
	*Addr=(UCHAR)(usTempValue&0x00FF);
	*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);
	*ulSize += sizeof(UINT16);	
	switch (wDataType)
	{
		case MTP_DATATYPE_UINT16:
			memcpy(&usTempValue,pValue,sizeof(UINT16));
			Addr = IN_Buffers+(*ulSize);	
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);								
			*ulSize += sizeof(UINT16);
		break;
	
		case MTP_DATATYPE_UINT32:
			memcpy(&ulTempValue,pValue,sizeof(UINT32));
			Addr = IN_Buffers+(*ulSize);	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);	
			*ulSize += sizeof(UINT32);
			
		break;
		
		case MTP_DATATYPE_UINT64:
			memcpy(&ulTempValue,pValue,sizeof(UINT32));
			memset(IN_Buffers+(*ulSize),0,sizeof(UINT32)*2);
			Addr = IN_Buffers+(*ulSize);	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			*ulSize += sizeof(UINT32)*2;	
		break;		

		case MTP_DATATYPE_UINT128:
			memcpy(&ulTempValue,pValue,sizeof(UINT32));
			memset(IN_Buffers+(*ulSize),0,sizeof(UINT32)*4);
			Addr = IN_Buffers+(*ulSize);	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			*ulSize += sizeof(UINT32)*4;				
		break;			

		case MTP_DATATYPE_STR:
			IN_Buffers[(*ulSize)] = DOS_FILENAME_LEN;
			memcpy(IN_Buffers+(*ulSize)+1,pValue,DOS_FILENAME_LEN*2);
			//Addr = pValue;
			//Addr = IN_Buffers+(*ulOffset)+1;	
			//for(i=0;i<DOS_FILENAME_LEN*2;i++)
			//{
			//	*Addr=(*pValue);
			//	Addr++;
			//}			
			(*ulSize) += DOS_FILENAME_LEN*2+1;
		break;		
	}
	return 0;
}

int ASCIIToUniCode(FW_CHAR *szASCIIString,FW_CHAR *szUniCodeSting)
{
	while( *szASCIIString != 0x00 ) 
	{
		*szUniCodeSting = *szASCIIString;
		szUniCodeSting++;
		*szUniCodeSting = 0x00;
		szASCIIString++;
		szUniCodeSting++;
	}
	*szUniCodeSting=0;
	szUniCodeSting++;
	*szUniCodeSting = 0;
		
	return 0;
}

OS_USER_ER USBMtpProcessObjectOpen( USHORT ProcKind, UINT32 objhdl, UINT32 parm1, UINT32 parm2 )
{
	OS_USER_ER		ret;
	UINT32			size = 0, dataSize = 0, cnt = 0, idx = 0;
	char			fName[ 200 ];
	UINT32			picX = 0, picY = 0, thumbX = 0, thumbY = 0,
					ObjCompressSize = 0, ThumbcompressSize = 0, RestSize;
	UCHAR			*Addr, *AddrW;
	OS_USER_FLGPTN	nFlag;
	FW_INT32		Err;
	FW_CHAR 		szTempString[ MTP_MAX_PATH ];
	char			szDate[ 32 ];
	ULONG			length = 0;
	T_BF_FS_IF_STAT 		file_stat;
	INT32			i;
	BMPPROP			oBmpProp;
	GIFPROP			oGifProp;
	TIFFPROP		oTifProp;
	MOVPROP			oMOVProp;
	JFIFPROP		oJFIFProp;
	BOOL			bHitFlg = FALSE;
	ULONG			ulImageBitDepth = 0;
#ifdef CO_BF_USB_MTP_DEBUG_WRITE
	ULONG ltime;
	ULONG sys_time_before, sys_time_after;
#endif
	
	memset(fName, 0, sizeof(fName));

	switch( ProcKind ) {
		case 	PTP_OP_GetObjectInfo:
			if(objhdl==HANDLE_FILE_DDISCVRY)
			{
				memcpy(IN_Buffers+12,&DPSDiscovery_ObjectInfo,sizeof(DPSDiscovery_ObjectInfo));
				SendData(DPSDiscovery_ObjectInfo.FCMK-DPSDiscovery_ObjectInfo.StorageID+1+(13*2)+3);//FileName has 13 word			  
				break;
			}
			else if((dpsStatus.outOperationType) && (objhdl==dpsStatus.outOperationType))
			{				
//				if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//					WriteInt(ObjectInfo.StorageID,PTP_StorageID_SD);
//				}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//					WriteInt(ObjectInfo.StorageID,PTP_StorageID_NF);
//				}else{
//					WriteInt(ObjectInfo.StorageID,PTP_StorageID_MS);
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
						snprintf(fName, sizeof(fName), "DREQUEST.DPS");
						break;
					case HANDLE_FILE_DRSPONSE:
						snprintf(fName, sizeof(fName), "DRSPONSE.DPS");
						break;
				}
				BF_Debug_Print_Information(("fName is %s\r\n",fName));
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
				PTP_fp = BF_Fs_If_Open(MiscPath, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
				ObjCompressSize = file_stat.fileSize;
				BF_Fs_If_Close(PTP_fp);
				/*  20120822 Add PTP_fp set to zero if PTP_fp is Close  */
				PTP_fp = 0;
				WriteInt(ObjectInfo.ObjectCompressSize,ObjCompressSize);
//				if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//					WriteInt(ObjectInfo.StorageID,PTP_StorageID_SD);
//				}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//					WriteInt(ObjectInfo.StorageID,PTP_StorageID_NF);
//				}else{
//					WriteInt(ObjectInfo.StorageID,PTP_StorageID_MS);
//				}
				WriteInt(ObjectInfo.StorageID,PtpStorageID);
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
				snprintf(fName, sizeof(fName), "AUTPRINT.MRK");
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
				for (i=1;i<=g_nObjectCnt;i++)
				{
					if (oMTPDBFileInfo[i].nFileIndex == objhdl)
					{
						if (oMTPDBFileInfo[i].bit.fDelete == 1)
						{
								SendResponseSimple(PTP_RS_InvalidObjectHandle);
								return 1;
						}

						if (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE)
						{
							memset((void*)&ObjectInfo,0,sizeof(ObjectInfo));
//							if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//								WriteInt(ObjectInfo.StorageID,PTP_StorageID_SD);
//							}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//								WriteInt(ObjectInfo.StorageID,PTP_StorageID_NF);
//							}else{
//								WriteInt(ObjectInfo.StorageID,PTP_StorageID_MS);
//							}
							WriteInt(ObjectInfo.StorageID,PtpStorageID);
							WriteShort(ObjectInfo.ObjectFormat,MTP_OFC_ASSOCIATION);
							WriteInt(ObjectInfo.ParentObject,oMTPDBFileInfo[i].nParentIndex);
							WriteInt(ObjectInfo.ObjectCompressSize,0);
							WriteShort(ObjectInfo.AssociationType,1);	// 2009/03/27 tony+
							dataSize=(ObjectInfo.FCMK-ObjectInfo.StorageID);
							Addr=ObjectInfo.FCMK;
							AddrW=Addr+1;

							xGetPathName(oMTPDBFileInfo[i].szDosFileName,szTempString);
							strncpy(fName,(const char*)szTempString,sizeof(fName));

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
						else if ( ( oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_MP3) ||
								  ( oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_BMP) ||
								  ( oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_JPG) ||
								  ( oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_AVI) ||
								  ( oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_DPOF)||
								  ( oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_MOV) ||
								  ( oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_MP4) ||
								  ( oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_WAV) ||
								  ( oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_TIFF)||
								  ( oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_GIF) )
						{
//							if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//								WriteInt(ObjectInfo.StorageID,PTP_StorageID_SD);
//							}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//								WriteInt(ObjectInfo.StorageID,PTP_StorageID_NF);
//							}else{
//								WriteInt(ObjectInfo.StorageID,PTP_StorageID_MS);
//							}
							WriteInt(ObjectInfo.StorageID,PtpStorageID);
							if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_MP3)
								WriteShort(ObjectInfo.ObjectFormat,MTP_OFC_MP3);
							else if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_BMP)
								WriteShort(ObjectInfo.ObjectFormat,MTP_OFC_BMP);
							else if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_GIF)
								WriteShort(ObjectInfo.ObjectFormat,MTP_OFC_GIF);
							else if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_JPG)
								WriteShort(ObjectInfo.ObjectFormat,MTP_OFC_EXIFJPEG);
							else if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_AVI)
								WriteShort(ObjectInfo.ObjectFormat,MTP_OFC_AVI);
							else if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_WAV)
								WriteShort(ObjectInfo.ObjectFormat,MTP_OFC_WAV);
							else if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_TIFF)
								WriteShort(ObjectInfo.ObjectFormat,MTP_OFC_TIFF);
							else if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_DPOF)
								WriteShort(ObjectInfo.ObjectFormat,MTP_OFC_DPOF);
							else if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_MOV)
								WriteShort(ObjectInfo.ObjectFormat,MTP_OFC_MOV);
							else if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_MP4)
								WriteShort(ObjectInfo.ObjectFormat,MTP_OFC_MP4);

							WriteShort(ObjectInfo.AssociationType,0); // 2009/03/27 tony+
							WriteShort(ObjectInfo.ThumbFormat,0);
							Err = BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);
							if(Err == FJ_ERR_OK)
							{
								PTP_fp = BF_Fs_If_Open(oMTPDBFileInfo[i].szDosFileName, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
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

							ObjCompressSize = file_stat.fileSize;
							WriteInt(ObjectInfo.ObjectCompressSize,ObjCompressSize);

							if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_BMP)
							{
								bmp_parse(oMTPDBFileInfo[i].szDosFileName,&oBmpProp);

								picX = oBmpProp.dwWidth;
								picY = oBmpProp.dwHeight;

								WriteInt(ObjectInfo.ImagePixWidth,picX);
								WriteInt(ObjectInfo.ImagePixHeight,picY);
							}

							if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_GIF)
							{
								gif_parse(oMTPDBFileInfo[i].szDosFileName,&oGifProp);
								picX = oGifProp.wWidth;
								picY = oGifProp.wHeight;

								WriteInt(ObjectInfo.ImagePixWidth,picX);
								WriteInt(ObjectInfo.ImagePixHeight,picY);
							}

							if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_JPG)
							{
								if(GetJpegPicInfo(&ThumbcompressSize, &thumbX, &thumbY, &picX, &picY, szDate) == FJ_ERR_OK)
								{
									WriteShort(ObjectInfo.ThumbFormat,0x3808);
								}

								jfif_parse(oMTPDBFileInfo[i].szDosFileName,&oJFIFProp);
								ulImageBitDepth = oJFIFProp.cImageBitDepth;
								ulImageBitDepth = ulImageBitDepth * 3;
							}

							if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_AVI)
							{
								if(GetAviInfo(&ThumbcompressSize, &thumbX, &thumbY, &thumbX, &thumbY, oMTPDBFileInfo[i].szDosFileName) == FJ_ERR_OK)
								{
									WriteShort(ObjectInfo.ThumbFormat,0x3808);
								}
							}

							if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_MOV)
							{
								if(GetMovInfo(&ThumbcompressSize, &thumbX, &thumbY, &thumbX, &thumbY, oMTPDBFileInfo[i].szDosFileName) == FJ_ERR_OK)
								{
									WriteShort(ObjectInfo.ThumbFormat,0x3808);
								}

								mov_parse(oMTPDBFileInfo[i].szDosFileName,&oMOVProp);
								picX = oMOVProp.dwWidth;
								picY = oMOVProp.dwHeight;
							}

							if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_MP4)
							{
								if(GetMovInfo(&ThumbcompressSize, &thumbX, &thumbY, &thumbX, &thumbY, oMTPDBFileInfo[i].szDosFileName) == FJ_ERR_OK)
								{
									WriteShort(ObjectInfo.ThumbFormat,0x3808);
								}

								mov_parse(oMTPDBFileInfo[i].szDosFileName,&oMOVProp);
								picX = oMOVProp.dwWidth;
								picY = oMOVProp.dwHeight;
							}

							if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_TIFF)
							{
								tiff_parse(oMTPDBFileInfo[i].szDosFileName,&oTifProp);
								picX = oTifProp.dwWidth;
								picY = oTifProp.dwHeight;
							}
							//WriteShort(ObjectInfo.ProtectionStatus,PROTECT_NoProtect);
							// 2009/03/31 STX tony +
							BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
							if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
							{
								//usTempValue = PROTECT_ReadOnly;
								WriteShort(ObjectInfo.ProtectionStatus,PROTECT_ReadOnly);
							}
							else
							{
								//usTempValue = PROTECT_NoProtect;
								WriteShort(ObjectInfo.ProtectionStatus,PROTECT_NoProtect);
							}
							// 2009/03/31 STX tony -							
							WriteInt(ObjectInfo.ObjectCompressSize,ObjCompressSize);
							WriteInt(ObjectInfo.ThumbCompressSize,ThumbcompressSize);
							WriteInt(ObjectInfo.ThumbPixWidth,thumbX);
							WriteInt(ObjectInfo.ThumbPixHeight,thumbY);
							WriteInt(ObjectInfo.ImagePixWidth,picX);
							WriteInt(ObjectInfo.ImagePixHeight,picY);
							WriteInt(ObjectInfo.ImageBitDepth,ulImageBitDepth);
							WriteInt(ObjectInfo.ParentObject,oMTPDBFileInfo[i].nParentIndex);
							//WriteShort(ObjectInfo.AssociationType,0); // 2009/03/27 tony mark +

							dataSize=(ObjectInfo.FCMK-ObjectInfo.StorageID);
							Addr=ObjectInfo.FCMK;
							AddrW=Addr+1;

							xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
							strncpy(fName,(const char*)szTempString,sizeof(fName));

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
							if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_JPG)
							{
								if(strlen(szDate) > 0)
								{
									*Addr = (UCHAR)0x10;
									AddrW = Addr + 1;
									//ObjectInfo.FCMK[1+cnt*2]=(UCHAR)0x10;
									strncpy(fName, (const char*)szDate, sizeof(fName));	
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
									Addr=Addr+1+cnt*2;
								}
								else
								{	
									*Addr = 0;
									Addr = Addr + 1;
									dataSize=cnt*2+1+dataSize;
								}
							}
							//_____________write CreatedDate in ObjectInfo________________
							else if (oMTPDBFileInfo[i].bit.file_exttype != MTP_FILE_TYPE_JPG)
							{
								*Addr = (UCHAR)0x10;
								AddrW = Addr + 1;
								//ObjectInfo.FCMK[1+cnt*2]=(UCHAR)0x10;
								snprintf(fName, sizeof(fName), "%04d%02d%02d%c%02d%02d%02d", ((file_stat.m_date >> 9) + 1980), (file_stat.m_date >> 5) & 0x0f, (file_stat.m_date) & 0x1f, \
								0x54, (file_stat.m_time >> 11) & 0x1f, (file_stat.m_time >> 5) & 0x3f, (file_stat.m_time & 0x1f) * 2);
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
								//AddrW++;
								//*AddrW=0;
								//dataSize=cnt*2+1+dataSize;
								//__________
								AddrW++;
								*AddrW=0;
								dataSize=1+dataSize;
								Addr=Addr+1+cnt*2;
//								BF_Fs_If_Close(PTP_fp);
//								PTP_fp=0;
							}
							else
							{
								*Addr = 0;
								Addr = Addr + 1;
								dataSize=cnt*2+1+dataSize;
							}

							//_____________write ModificationDate in ObjectInfo________________
							*Addr = (UCHAR)0x10;
							AddrW = Addr + 1;
							//ObjectInfo.FCMK[1+cnt*2]=(UCHAR)0x10;
							snprintf(fName, sizeof(fName), "%04d%02d%02d%c%02d%02d%02d", ((file_stat.date >> 9) + 1980), (file_stat.date >> 5) & 0x0f, (file_stat.date) & 0x1f, \
							0x54, (file_stat.time >> 11) & 0x1f, (file_stat.time >> 5) & 0x3f, (file_stat.time & 0x1f) * 2);
							for(cnt=0;cnt<sizeof(fName) && fName[cnt] != 0;cnt++)
							{
								*AddrW=fName[cnt];
								AddrW++;
								*AddrW=0;
								AddrW++;
							}

							if(cnt>0) {
								*AddrW=0;
								AddrW++;
								*AddrW=0;
							}
							cnt++;

							AddrW++;
							*AddrW=0;
							dataSize=cnt*2+1+dataSize;
							//AddrW++;
							//*AddrW=0;
							//dataSize=cnt*2+1+dataSize;
							//__________
							AddrW++;
							*AddrW=0;
							dataSize=1+dataSize;
							BF_Fs_If_Close(PTP_fp);
							PTP_fp=0;
						}
						bHitFlg = TRUE;
					}
				}

				if(!bHitFlg)
				{
					SendResponseSimple(PTP_RS_InvalidObjectHandle);
					return 1;
				}
			}

			memcpy(IN_Buffers+12,&ObjectInfo,sizeof(ObjectInfo));
			SendData(dataSize);
			break;

		case PTP_OP_GetThumb:
			if((objhdl&0x0000FFFF)!=0)
			{
				if (oMTPDBFileInfo[objhdl].bit.fDelete == 1)
				{
						SendResponseSimple(PTP_RS_InvalidObjectHandle);
						return 1;
				}

				if ( (oMTPDBFileInfo[objhdl].bit.file_exttype != MTP_FILE_TYPE_JPG)
					&& (oMTPDBFileInfo[objhdl].bit.file_exttype != MTP_FILE_TYPE_MOV)
					&& (oMTPDBFileInfo[objhdl].bit.file_exttype != MTP_FILE_TYPE_MP4)
					&& (oMTPDBFileInfo[objhdl].bit.file_exttype != MTP_FILE_TYPE_AVI) )
				{
					SendResponseSimple(PTP_RS_NoThumbnailPresent);
					return 1;
				}

				Err = BF_Fs_If_Stat((const char*)oMTPDBFileInfo[objhdl].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);
				if(Err == FJ_ERR_OK)
				{
					switch(oMTPDBFileInfo[objhdl].bit.file_exttype)
					{
						case MTP_FILE_TYPE_JPG:
							Err = GetJpegThumbnail(oMTPDBFileInfo[objhdl].szDosFileName, (FW_ULONG)(IN_Buffers+12), 65536 - 12, &length);
							break;
						case MTP_FILE_TYPE_AVI:
							Err = GetAviThumbnail(oMTPDBFileInfo[objhdl].szDosFileName, (FW_ULONG)(IN_Buffers+12), 65536 - 12, &length);
							break;
						case MTP_FILE_TYPE_MOV:
							Err = GetMovThumbnail(oMTPDBFileInfo[objhdl].szDosFileName, (FW_ULONG)(IN_Buffers+12), 65536 - 12, &length);
							break;
						case MTP_FILE_TYPE_MP4:
							Err = GetMovThumbnail(oMTPDBFileInfo[objhdl].szDosFileName, (FW_ULONG)(IN_Buffers+12), 65536 - 12, &length);
							break;
						default:
							Err = -1;
					}
				}
				else
				{
					SendResponseSimple(PTP_RS_InvalidObjectHandle);
					return 1;
				}

				if(Err != FJ_ERR_OK)
				{
					SendResponseSimple(PTP_RS_NoThumbnailPresent);
					return 1;
				}

				size=RestSize=ThumbcompressSize=length;
				SendDataDetail2(ThumbcompressSize,size);
				if(((size+12) % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
				{
					OS_User_Tslp_Tsk(1);
					MWUSB_SendNullPacket(BULK_WRITE_FIFO);
				}
			}
			break;

		case PTP_OP_GetObject:	//<<<BigSize>>>
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
				BF_Fs_If_Stat(MiscPath, (T_BF_FS_IF_STAT *)&file_stat);
				PTP_fp = BF_Fs_If_Open(MiscPath, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
				ObjCompressSize = file_stat.fileSize;
				//_________________________________Loop Sending___________________________

				BF_Fs_If_Lseek(PTP_fp, 0L, SEEK_SET, &length);
				RestSize=ObjCompressSize;

				//jeff: read a very large data block each time for optimization
				size = PTP_fread((ULONG)(IN_Buffers+12), CalcSendAndRest(65536-12,&RestSize));
				SendDataDetail2(ObjCompressSize,size);

				if ( RestSize > 0 )
				{
					while( RestSize )
					{
						size = PTP_fread((ULONG)IN_Buffers, CalcSendAndRest(65536, &RestSize));
						MWUSB_SetMode(0);	
						MWUSB_Write((UCHAR*)IN_Buffers, size, BULK_WRITE_FIFO);
					}

					if(((size) % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
					{
						OS_User_Tslp_Tsk(1);
						MWUSB_SendNullPacket(BULK_WRITE_FIFO);
					}
				}
				else
				{
					if(((size+12) % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
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
				if (oMTPDBFileInfo[objhdl].bit.fDelete == 1)
				{
						SendResponseSimple(PTP_RS_InvalidObjectHandle);
						return 1;
				}

				Err = BF_Fs_If_Stat((const char*)oMTPDBFileInfo[objhdl].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);
				if(Err == FJ_ERR_OK)
				{
					PTP_fp = BF_Fs_If_Open(oMTPDBFileInfo[objhdl].szDosFileName, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
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

				ObjCompressSize = file_stat.fileSize;
				//_________________________________Loop Sending___________________________

				BF_Fs_If_Lseek(PTP_fp, 0L, SEEK_SET, &length);

				RestSize = ObjCompressSize;
				//jeff: read a very large data block each time for optimization
				size = PTP_fread( (ULONG)(IN_Buffers+12), CalcSendAndRest( 65536 - 12, &RestSize ) );

#ifdef CO_BF_USB_MTP_DEBUG_WRITE
				sys_time_before = BF_Get_Time_us();
#endif

				SendDataDetail2( ObjCompressSize, size );

#ifdef CO_BF_USB_MTP_DEBUG_WRITE
				sys_time_after = BF_Get_Time_us();
				ltime = sys_time_after - sys_time_before;
				printf("***** Write Data with header ***** Size = %u, Time = %lu\n", size+12, ltime);
#endif

				USHORT 	sMaxPaketSize;
				sMaxPaketSize = MWUSB_GetMaxPacketSize( BULK_WRITE_FIFO );

				MTP_InitData();

				if ( RestSize > 0 )
				{
					while( RestSize )
					{
						idx = cnt * sMaxPaketSize * 1024;

						cnt++;

						if ( cnt >= MTP_DATA_MNG_INFO_NUM ) {
							cnt = 0;
						}

						size = CalcSendAndRest( sMaxPaketSize * 1024, &RestSize );

#ifdef CO_BF_USB_MTP_DEBUG_WRITE
						sys_time_before = BF_Get_Time_us();
#endif

						// USB write.
						while ( 1 ) {

							if ( MTP_PutData( BULK_WRITE_FIFO, (ULONG)&IN_Buffers[ idx ], size, 0 ) == FJ_ERR_OK ) {

								OS_User_Set_Flg( FID_MTPTSK, FLG_MTP_READWRITE );

								break;
							}

							OS_User_Tslp_Tsk( 1 );
						}

#ifdef CO_BF_USB_MTP_DEBUG_WRITE
						sys_time_after = BF_Get_Time_us();
						ltime = sys_time_after - sys_time_before;
						printf("***** Write Data ***** Size = %u, Time = %lu\n", size, ltime);
#endif
						OS_User_Tslp_Tsk( 1 );
					}

					if ( ( size % sMaxPaketSize ) == 0 )
					{
						if ( MTP_PutData( BULK_WRITE_FIFO, 0, 0, 0 ) == FJ_ERR_OK ) {
							OS_User_Set_Flg( FID_MTPTSK, FLG_MTP_READWRITE );
						}
					}
				}
				else
				{
					if ( ( ( size + 12 ) % sMaxPaketSize ) == 0 )
					{
						OS_User_Tslp_Tsk( 1 );
						MWUSB_SendNullPacket( BULK_WRITE_FIFO );
					}
				}

				// OK response.
				while ( 1 ) {

					if ( MTP_PutData( VEND_WRITE_FIFO, 0, 0, 0 ) == FJ_ERR_OK ) {
						OS_User_Set_Flg( FID_MTPTSK, FLG_MTP_READWRITE );
						break;
					}

					OS_User_Tslp_Tsk( 1 );
				}

				BF_Fs_If_Close( PTP_fp );
				PTP_fp = 0;
			}
			else
			{
				SendResponseSimple( PTP_RS_InvalidObjectHandle );
				return( 1 );
			}

			break;

		case PTP_OP_GetPartialObject:	//<<<BigSize>>>
			if(objhdl == HANDLE_FILE_AUTO)
			{
				BF_Fs_If_Stat(MiscPath, (T_BF_FS_IF_STAT *)&file_stat);
				PTP_fp = BF_Fs_If_Open(MiscPath, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);

				ObjCompressSize = file_stat.fileSize;
				//_________________________________Loop Sending___________________________
				BF_Fs_If_Lseek(PTP_fp, parm1, SEEK_SET, &length);
				RestSize=parm2;

				if(RestSize>(ObjCompressSize-parm1)){
					RestSize=(ObjCompressSize-parm1);
				}

				ObjCompressSize = RestSize;

				size = PTP_fread((ULONG)(IN_Buffers+12), CalcSendAndRest(1024-12,&RestSize));
				SendDataDetail2(ObjCompressSize,size);
				if ( RestSize > 0 )
				{
					while(RestSize)
					{
						size = PTP_fread((ULONG)IN_Buffers, CalcSendAndRest(1024,&RestSize));
						MWUSB_SetMode(0);
						MWUSB_Write((UCHAR*)IN_Buffers, size, BULK_WRITE_FIFO);
					}

					if(((size+12) % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
					{
						OS_User_Tslp_Tsk(1);
						MWUSB_SendNullPacket(BULK_WRITE_FIFO);
					}
				}
				else
				{
					if(((size+12) % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
					{
						OS_User_Tslp_Tsk(1);
						MWUSB_SendNullPacket(BULK_WRITE_FIFO);
					}
				}
				BF_Fs_If_Close(PTP_fp);
				PTP_fp=0;
			}
			else
			{
				if((objhdl&0x0000FFFF)!=0)
				{
					if (oMTPDBFileInfo[objhdl].bit.fDelete == 1)
					{
						SendResponseSimple(PTP_RS_InvalidObjectHandle);
						return 1;
					}

					if (oMTPDBFileInfo[objhdl].bit.fDelete == 1)
					{
						SendResponseSimple(PTP_RS_InvalidObjectHandle);
						return 1;
					}

					Err = BF_Fs_If_Stat((const char*)oMTPDBFileInfo[objhdl].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);
					if(Err == FJ_ERR_OK)
					{
						PTP_fp = BF_Fs_If_Open(oMTPDBFileInfo[objhdl].szDosFileName, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
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

					ObjCompressSize = file_stat.fileSize;
					//_________________________________Loop Sending___________________________
					BF_Fs_If_Lseek(PTP_fp, parm1, SEEK_SET, &length);
					RestSize=parm2;

					if(RestSize>(ObjCompressSize-parm1)) {
						RestSize=(ObjCompressSize-parm1);
					}
					ObjCompressSize=RestSize;

					size = PTP_fread((ULONG)(IN_Buffers+12), CalcSendAndRest(1024-12,&RestSize));
					SendDataDetail2(ObjCompressSize,size);
					if(RestSize > 0)
					{
						while(RestSize)
						{
							size = PTP_fread((ULONG)IN_Buffers, CalcSendAndRest(1024,&RestSize));
							MWUSB_SetMode(0);
							MWUSB_Write((UCHAR*)IN_Buffers, size, BULK_WRITE_FIFO);
						}
						if(((size) % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
						{
							OS_User_Tslp_Tsk(1);
							MWUSB_SendNullPacket(BULK_WRITE_FIFO);
						}
					}
					else
					{
						if(((size+12) % MWUSB_GetMaxPacketSize(BULK_WRITE_FIFO)) == 0)
						{
							OS_User_Tslp_Tsk(1);
							MWUSB_SendNullPacket(BULK_WRITE_FIFO);
						}
					}
					BF_Fs_If_Close(PTP_fp);
					PTP_fp=0;
				}
				else
				{
					SendResponseSimple(PTP_RS_InvalidObjectHandle);
					return 1;
				}
			}
			break;
	}

	return( 0 );
}

int xGetObjectLevel(FW_CHAR *szPath,BOOL bIsFolder,INT32 *nLevel)
{
	char cBackslash = '\\';
	char *szDest1;
	char szTempString[MTP_MAX_PATH];
	INT32 nStrLength=0;
	INT32 i;
	INT32 nRootLevel=0;

	*nLevel = 0;
	nStrLength = strlen((const char*)szPath);

	for (i=0;i<MTP_MAX_PATH;i++)
	{
		szTempString[i] = 0x00;
	}
		
	if (bIsFolder == TRUE)
	{
		strncpy(szTempString,(const char*)szPath,nStrLength-1);
		szDest1 = strrchr(szTempString,cBackslash); 
	}
	else
	{
		strncpy(szTempString,(const char*)szPath,nStrLength);
		szDest1 = strrchr(szTempString,cBackslash);
	}

	while(szDest1 != NULL)
	{
		(*nLevel)++;

		for (i=0;i<MTP_MAX_PATH;i++)
		{
			szTempString[i] = 0x00;
		}	
		strncpy(szTempString,(const char*)szPath,szDest1-szTempString);

		szDest1 = strrchr(szTempString,cBackslash);
	}

	// Root Folder Level

	nRootLevel = 0;
	nStrLength = strlen((const char*)g_szRootFolder);
	
	for (i=0;i<MTP_MAX_PATH;i++)
	{
		szTempString[i] = 0x00;
	}
		
	strncpy(szTempString,(const char*)g_szRootFolder,nStrLength-1);
	szDest1 = strrchr(szTempString,cBackslash); 

	while(szDest1 != NULL)
	{
		nRootLevel++;

		for (i=0;i<MTP_MAX_PATH;i++)
		{
			szTempString[i] = 0x00;
		}	
		strncpy(szTempString,(const char*)g_szRootFolder,szDest1-szTempString);

		szDest1 = strrchr(szTempString,cBackslash);
	}	
	
	szDest1 = strstr(szPath,g_szRootFolder);

	if (szDest1 != NULL)
	{
        if ( gFolder_Filter.Root_Folder_flg == 1 ) // & ROOT_FOLDER_ENABLE) == ROOT_FOLDER_ENABLE)
        {
			if ( nRootLevel == 0 )				
			{
				*nLevel = *nLevel-nRootLevel;
			}
			else
			{
				*nLevel = *nLevel-nRootLevel+1;
			}
        }
		else
			*nLevel = *nLevel-nRootLevel;
	}

	return 0;
}

INT32 xIsDCFFile(FW_CHAR* strFile)
{
	INT32  result;
	INT32  length;
	INT32  i;
	FW_CHAR szFileName[12]= "\0";	
	FW_CHAR *pdest;
	//FW_CHAR *pattern="0123456789";
	length = strlen(strFile);
	pdest = strstr(strFile,FJ_CUSTOM_DCF_FILE_NAME);	//DSC0
	if (pdest != NULL)
	{
		result = pdest - strFile + 1;		
		if ((length-result+1)!=12)
			return 1;
		strncpy(szFileName,pdest,length-result+1);
		for (i=0;i<4;i++){
			if (!((szFileName[4+i]=='0')||
				(szFileName[4+i]=='1')||
				(szFileName[4+i]=='2')||
				(szFileName[4+i]=='3')||
				(szFileName[4+i]=='4')||
				(szFileName[4+i]=='5')||
				(szFileName[4+i]=='6')||
				(szFileName[4+i]=='7')||
				(szFileName[4+i]=='8')||
				(szFileName[4+i]=='9')))
			{
				return 1;
			}
		}
		if ((szFileName[4]=='0')&&
			(szFileName[4+1]=='0')&&
			(szFileName[4+2]=='0')&&
			(szFileName[4+3]=='0'))
		{
			return 1;
		}
	}
	else 
		return 1;	
	return 0;	//is DCFFile
}

INT32 xIsJpgFile(FW_CHAR* strFile)
{
	INT32  result;
	INT32  length;
	FW_CHAR szDot[] = ".";
	FW_CHAR szExt[4]= "\0";
	FW_CHAR *pdest;

	length = strlen(strFile);
	pdest = strstr(strFile,szDot);
	
	if (pdest != NULL)
	{
		result = pdest - strFile + 1;
		strncpy(szExt,strFile+result,length-result);
	}
	else 
		return 1;

	if (strcmpi(szExt,"JPG") != 0)
		return -1;

	return 0;	//is JPGFile
}

INT32 xIsBmpFile(FW_CHAR* strFile)
{
	INT32  result;
	INT32  length;
	FW_CHAR szDot[] = ".";
	FW_CHAR szExt[4]= "\0";
	FW_CHAR *pdest;

	length = strlen(strFile);
	pdest = strstr(strFile,szDot);
	
	if (pdest != NULL)
	{
		result = pdest - strFile + 1;
		strncpy(szExt,strFile+result,length-result);
	}
	else 
		return 1;

	if (strcmpi(szExt,"BMP") != 0)
		return -1;

	return 0;
}

INT32 xIsGifFile(FW_CHAR* strFile)
{
	INT32  result;
	INT32  length;
	FW_CHAR szDot[] = ".";
	FW_CHAR szExt[4]= "\0";
	FW_CHAR *pdest;

	length = strlen(strFile);
	pdest = strstr(strFile,szDot);
	
	if (pdest != NULL)
	{
		result = pdest - strFile + 1;
		strncpy(szExt,strFile+result,length-result);
	}
	else 
		return 1;

	if (strcmpi(szExt,"GIF") != 0)
		return -1;

	return 0;
}

INT32 xIsTiffFile(FW_CHAR* strFile)
{
	INT32  result;
	INT32  length;
	FW_CHAR szDot[] = ".";
	FW_CHAR szExt[4]= "\0";
	FW_CHAR *pdest;
	
	length = strlen(strFile);
	pdest = strstr(strFile,szDot);
	
	if (pdest != NULL)
	{
		result = pdest - strFile + 1;
		strncpy(szExt,strFile+result,length-result);
	}
	else 
		return 1;

	if (strcmpi(szExt,"TIF") != 0)
		return -1;

	return 0;
}

INT32 xIsAviFile(FW_CHAR* strFile)
{
	INT32  result;
	INT32  length;
	FW_CHAR szDot[] = ".";
	FW_CHAR szExt[4]= "\0";
	FW_CHAR *pdest;

	length = strlen(strFile);
	pdest = strstr(strFile,szDot);
	
	if (pdest != NULL)
	{
		result = pdest - strFile + 1;
		strncpy(szExt,strFile+result,length-result);
	}
	else 
		return 1;

	if (strcmpi(szExt,"AVI") != 0)
		return -1;

	return 0;
}


INT32 xIsMovFile(FW_CHAR* strFile)
{
	INT32  result;
	INT32  length;
	FW_CHAR szDot[] = ".";
	FW_CHAR szExt[4]= "\0";
	FW_CHAR *pdest;

	length = strlen(strFile);
	pdest = strstr(strFile,szDot);

	if (pdest != NULL)
	{
		result = pdest - strFile + 1;
		strncpy(szExt,strFile+result,length-result);
	}
	else 
		return 1;

	if (strcmpi(szExt,"MOV") != 0)
		return -1;

	return 0;
}

INT32 xIsMp4File(FW_CHAR* strFile)
{
	INT32  result;
	INT32  length;
	FW_CHAR szDot[] = ".";
	FW_CHAR szExt[4]= "\0";
	FW_CHAR *pdest;

	length = strlen(strFile);
	pdest = strstr(strFile,szDot);

	if (pdest != NULL)
	{
		result = pdest - strFile + 1;
		strncpy(szExt,strFile+result,length-result);
	}
	else 
		return 1;

	if (strcmpi(szExt,"MP4") != 0)
		return -1;

	return 0;
}

INT32 xIsWavFile(FW_CHAR* strFile)
{
	INT32  result;
	INT32  length;
	FW_CHAR szDot[] = ".";
	FW_CHAR szExt[4]= "\0";
	FW_CHAR* pdest;
	
	length = strlen(strFile);
	pdest = strstr(strFile,szDot);
	
	if (pdest != NULL)
	{
		result = pdest - strFile + 1;
		strncpy(szExt,strFile+result,length-result);
	}
	else 
		return 1;

	if (strcmpi(szExt,"WAV") != 0)
		return -1;

	return 0;
}

int xIsMp3File(FW_CHAR* strFile)
{
	INT32  result;
	INT32  length;
	FW_CHAR szDot[] = ".";
	FW_CHAR szExt[4]= "\0";
	FW_CHAR* pdest;

	length = strlen(strFile);
	pdest = strstr(strFile,szDot);
	
	if (pdest != NULL)
	{
		result = pdest - strFile + 1;
		strncpy(szExt,strFile+result,length-result);
	}
	else 
		return 1;

	if (strcmpi(szExt,"MP3") != 0)
		return -1;

	return 0;
}


INT32 xIsDpofFile(FW_CHAR* strFile)
{
	INT32  result;
	INT32  length;
	FW_CHAR szDot[13] = "AUTPRINT.MRK";
	FW_CHAR szExt[13]= "\0";
	FW_CHAR *pdest;

	length = strlen(strFile);
	pdest = strstr(strFile,szDot);
	
	if (pdest != NULL)
	{
		result = pdest - strFile;
		strncpy(szExt,strFile+result,length-result);
	}
	else 
		return 1;

	if (strcmpi(szExt,"AUTPRINT.MRK") != 0)
		return -1;

	return 0;	//is JPGFile
}

INT32 xGetPathName(FW_CHAR* szFullPath,FW_CHAR* szPathName)
{
	static FW_CHAR str = '\\';
	FW_CHAR szTempString[MTP_MAX_PATH];
	FW_CHAR *pDest;
	INT32 	nStringLength;
	INT32 	 nPos1;

	memset(szTempString,0,MTP_MAX_PATH);
	nStringLength = strlen(szFullPath);
	pDest  = strrchr(szFullPath,str);
	if (pDest == NULL)
		return 1;
	
	nPos1  = pDest-szFullPath;	
	if (nPos1 == (nStringLength-1) )  // end character is '\\'
	{
		if (nStringLength > 0)
		{
			strncpy(szTempString,szFullPath,nStringLength-1);
			pDest = strrchr(szTempString,str);
			if (pDest == NULL) {
				return 1;
			}
			if ( (nStringLength-1-(pDest-szTempString)) > 0)					
				strncpy(szPathName,pDest+1,nStringLength-1-(pDest-szTempString));
			else 
				return 1;
		}
		else
			return 1;
	}
	else
	{
		if (nStringLength > 0)
		{
			pDest = strrchr(szFullPath,str);
			if (pDest == NULL)
				return 1;
			if ((nStringLength-(pDest-szFullPath)) > 0)
				strncpy(szPathName,pDest+1,nStringLength-(pDest-szFullPath));
			else 
				return 1;
		}
		else
			return 1;		
	}

	return 0;
	
}

INT32 xGetFileName(FW_CHAR* szFullPath,FW_CHAR *szFileName)
{
	FW_CHAR* pDest;
	FW_CHAR  szTempString[MTP_MAX_PATH];
	static FW_CHAR  str='\\';	
	INT32 	 nStringLength;

	nStringLength = strlen(szFullPath);
	memset(szTempString,0,MTP_MAX_PATH);
	memset(szFileName,0,MTP_MAX_PATH);
  	pDest = strrchr(szFullPath,str);
	if (pDest == NULL)
		return 1;
	
	if ( ( nStringLength-1-(pDest-szFullPath) ) > 0)
		strncpy(szFileName,pDest+1,nStringLength-1-(pDest-szFullPath));
	else 
		return 1;
	
	return 0;
}

INT32 xCheckParentObject(INT32 nObjectHandle,INT32 nParentHandle,BOOL *bIsFind)
{
	INT32 nIndex=0;

	*bIsFind = FALSE;
	nIndex = nObjectHandle;

	if (nObjectHandle == nParentHandle)
		*bIsFind = TRUE;

	while (oMTPDBFileInfo[nIndex].nParentIndex != 0)
	{
		if (oMTPDBFileInfo[nIndex].nParentIndex == nParentHandle)
		{
			*bIsFind = TRUE;
			nIndex = oMTPDBFileInfo[nIndex].nParentIndex;
		}
		else 
		{
			nIndex = oMTPDBFileInfo[nIndex].nParentIndex;
		}
	}
	
	return 0;
}

INT32 xAddAllAssociationObjectPorpValueList(INT32 nObjectHandle,ULONG dwObjectSize,INT32 nParentObject,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	
//	if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//		ulTempValue = PTP_StorageID_SD;	
//	}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//		ulTempValue = PTP_StorageID_NF;	
//	}else{
//		ulTempValue = PTP_StorageID_MS;	
//	}	
	ulTempValue = PtpStorageID;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);																			
	usTempValue = MTP_OFC_ASSOCIATION; 
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 														
	usTempValue = PROTECT_NoProtect;	
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);									
	ulTempValue = nParentObject;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);	

	return 0;	
}

INT32 xAddAllJPGObjectPorpValueList(INT32 nObjectHandle,ULONG dwObjectSize,INT32 nParentObject,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 		ulTempValue=0;
	USHORT 		usTempValue=0;
	JFIFPROP	oJFIFProp;
	//char 		szTempString[16];
	//char        szDate[32];
	char 		szAscDate[16];
	char 		szUniDate[32];
	T_BF_FS_IF_STAT 	file_stat;	
	UINT32		picX=0,picY=0,thumbX=0,thumbY=0,ThumbcompressSize=0;	
	ULONG 		length = 0;
	UCHAR		*Addr;

//	if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//		ulTempValue = PTP_StorageID_SD;	
//	}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//		ulTempValue = PTP_StorageID_NF;	
//	}else{
//		ulTempValue = PTP_StorageID_MS;	
//	}	
	ulTempValue = PtpStorageID;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);																	
	usTempValue = MTP_OFC_EXIFJPEG; 
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 												
	//usTempValue = PROTECT_NoProtect;	
	// 2009/03/31 STX tony +
	BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
	if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
	{
		usTempValue = PROTECT_ReadOnly;
	}
	else
	{
		usTempValue = PROTECT_NoProtect;
	}
	// 2009/03/31 STX tony -	
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 																
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);		
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize); 							

	memset(szAscDate, 0, sizeof(szAscDate));
	// 2009/04/09 STX tony +
	PTP_fp = BF_Fs_If_Open(oMTPDBFileInfo[nObjectHandle].szDosFileName, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
	GetJpegPicInfo(&ThumbcompressSize, &thumbX, &thumbY, &picX, &picY, szAscDate);
	if(strlen(szAscDate) > 0)
	{
		ASCIIToUniCode(szAscDate,szUniDate); 	
	}
	else
	{
		memset(szUniDate,0,MTP_DATETIME_LENGTH*2);
	}
	xAddObjectPorpValueListForVarLenString(nObjectHandle,MTP_OPC_DATECREATED,MTP_DATATYPE_STR,szUniDate,ulSize,MTP_DATETIME_LENGTH);	
	BF_Fs_If_Close(PTP_fp);
	PTP_fp=0;	
	// 2009/04/09 STX tony -

	// 2009/03/31 STX tony +
	BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);
	snprintf(szAscDate, sizeof(szAscDate), "%04d%02d%02d%c%02d%02d%02d", ((file_stat.date >> 9) + 1980), (file_stat.date >> 5) & 0x0f, (file_stat.date) & 0x1f, \
				   0x54, (file_stat.time >> 11) & 0x1f, (file_stat.time >> 5) & 0x3f, (file_stat.time & 0x1f) * 2);
	
	ASCIIToUniCode(szAscDate,szUniDate); 	
	xAddObjectPorpValueListForVarLenString(nObjectHandle,MTP_OPC_DATEMODIFIED,MTP_DATATYPE_STR,szUniDate,ulSize,MTP_DATETIME_LENGTH);
	// 2009/03/31 STX tony -

	ulTempValue = nParentObject;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize); 			
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);	
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);						

	PTP_fp = BF_Fs_If_Open(oMTPDBFileInfo[nObjectHandle].szDosFileName, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
	GetJpegPicInfo(&ThumbcompressSize, &thumbX, &thumbY, &picX, &picY, szAscDate);
	usTempValue = MTP_OFC_EXIFJPEG;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleFormat,MTP_DATATYPE_UINT16,&usTempValue,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleSize,MTP_DATATYPE_UINT32,&ThumbcompressSize,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleHeigth,MTP_DATATYPE_UINT32,&thumbY,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleWidth,MTP_DATATYPE_UINT32,&thumbX,ulSize);
	BF_Fs_If_Close(PTP_fp);
	PTP_fp=0;	

	xAddObjectPorpValueListNoValue(nObjectHandle,MTP_OPC_REPSampleData,MTP_DATATYPE_AUINT8,ulSize);
	*ulSize = *ulSize + sizeof(UINT32);
	if(GetJpegThumbnail(oMTPDBFileInfo[nObjectHandle].szDosFileName, (FW_ULONG)(IN_Buffers+*ulSize), 65536 - 12, &length) != FJ_ERR_OK)
	{
		// Error 
		BF_Debug_Print_Error(("xAddAllJPGObjectPorpValueList GetJpegThumbnail error"));
	}
	else
	{
		*ulSize = *ulSize + length;
	}

	// Write Thumbnail Size
	ulTempValue = length;
	Addr = IN_Buffers + *ulSize - length - sizeof(UINT32);
	*Addr=(UCHAR)(ulTempValue&0x000000FF);
	*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
	*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);
	*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);

	jfif_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oJFIFProp);
	ulTempValue = oJFIFProp.wWidth;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);							
	ulTempValue = oJFIFProp.wHeight;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
	ulTempValue = oJFIFProp.cImageBitDepth;
	ulTempValue = ulTempValue * 3;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_IMAGEBITDEPTH,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);

	return 0;
}

INT32 xAddAllAVIObjectPorpValueList(INT32 nObjectHandle,ULONG dwObjectSize,INT32 nParentObject,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	AVIPROP			oAVIProp;
	T_BF_FS_IF_STAT			file_stat;
	char 			szAscDate[16];
	char 			szUniDate[32];	

//	if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//		ulTempValue = PTP_StorageID_SD;	
//	}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//		ulTempValue = PTP_StorageID_NF;	
//	}else{
//		ulTempValue = PTP_StorageID_MS;	
//	}
	ulTempValue = PtpStorageID;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);																	
	usTempValue = MTP_OFC_AVI; 
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 												
	//usTempValue = PROTECT_NoProtect;	
	// 2009/03/31 STX tony +
	BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
	if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
	{
		usTempValue = PROTECT_ReadOnly;
	}
	else
	{
		usTempValue = PROTECT_NoProtect;
	}
	// 2009/03/31 STX tony -	
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 																
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);													
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);	


	memset(szAscDate, 0, sizeof(szAscDate));
	// 2009/04/08 STX tony +
	BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);
	snprintf(szAscDate, sizeof(szAscDate), "%04d%02d%02d%c%02d%02d%02d", ((file_stat.date >> 9) + 1980), (file_stat.date >> 5) & 0x0f, (file_stat.date) & 0x1f, \
				   0x54, (file_stat.time >> 11) & 0x1f, (file_stat.time >> 5) & 0x3f, (file_stat.time & 0x1f) * 2);
	
	ASCIIToUniCode(szAscDate,szUniDate); 	
	xAddObjectPorpValueListForVarLenString(nObjectHandle,MTP_OPC_DATEMODIFIED,MTP_DATATYPE_STR,szUniDate,ulSize,16);
	// 2009/04/08 STX tony -

	
	ulTempValue = nParentObject;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize); 			
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);	
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);						
	avi_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oAVIProp);				
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&oAVIProp.dwWidth,ulSize);											
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&oAVIProp.dwHeight,ulSize); 										
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&oAVIProp.dwDuration,ulSize); 											
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&oAVIProp.dwSampleRate,ulSize); 					
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&oAVIProp.dwNumberOfChannels,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_SCANTYPE,MTP_DATATYPE_UINT16,&oAVIProp.dwScanType,ulSize); 					
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&oAVIProp.dwAudioBitRate,ulSize); 	
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_VIDEOFOURCCCODEC,MTP_DATATYPE_UINT32,&oAVIProp.dwVideoFourCCCodec,ulSize); 					
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_VIDEOBITRATE,MTP_DATATYPE_UINT32,&oAVIProp.dwVideoBitRate,ulSize); 		
	ulTempValue = ENDIAN_SWAP32(oAVIProp.dwVideoBitRate);
	
	return 0;
}


INT32 xAddAllDPOFObjectPorpValueList(INT32 nObjectHandle,ULONG dwObjectSize,INT32 nParentObject,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	T_BF_FS_IF_STAT			file_stat;

//	if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//		ulTempValue = PTP_StorageID_SD;	
//	}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//		ulTempValue = PTP_StorageID_NF;	
//	}else{
//		ulTempValue = PTP_StorageID_MS;	
//	}
	ulTempValue = PtpStorageID;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);																	
	usTempValue = MTP_OFC_DPOF; 
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 												
	BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);
	if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
	{
		usTempValue = PROTECT_ReadOnly;
	}
	else
	{
		usTempValue = PROTECT_NoProtect;
	}
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 																
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);													
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);	
	ulTempValue = nParentObject;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize); 			
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);	
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);						

	return 0;
}

INT32 xAddAllMOVObjectPorpValueList(INT32 nObjectHandle,ULONG dwObjectSize,INT32 nParentObject,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	MOVPROP			oMOVProp;
	T_BF_FS_IF_STAT			file_stat;
	UINT32			thumbX=0,thumbY=0,ThumbcompressSize=0;
	ULONG 			length = 0;

//	if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//		ulTempValue = PTP_StorageID_SD;	
//	}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//		ulTempValue = PTP_StorageID_NF;	
//	}else{
//		ulTempValue = PTP_StorageID_MS;	
//	}
	ulTempValue = PtpStorageID;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);																	
	usTempValue = MTP_OFC_MOV; 
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 												
	BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);
	if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
	{
		usTempValue = PROTECT_ReadOnly;
	}
	else
	{
		usTempValue = PROTECT_NoProtect;
	}
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);
	ulTempValue = nParentObject;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);

	// 0xDC81,0xDC82,0xDC83,0xDC84,0xDC86
	if(GetMovInfo(&ThumbcompressSize, &thumbX, &thumbY, &thumbX, &thumbY, oMTPDBFileInfo[nObjectHandle].szDosFileName) != FJ_ERR_OK)
	{
		// Error 
		BF_Debug_Print_Error(("xAddAllMOVObjectPorpValueList GetMovInfo error!\n"));
	}

	usTempValue = MTP_OFC_EXIFJPEG;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleFormat,MTP_DATATYPE_UINT16,&usTempValue,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleSize,MTP_DATATYPE_UINT32,&ThumbcompressSize,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleHeigth,MTP_DATATYPE_UINT32,&thumbY,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleWidth,MTP_DATATYPE_UINT32,&thumbX,ulSize);

	xAddObjectPorpValueListNoValue(nObjectHandle,MTP_OPC_REPSampleData,MTP_DATATYPE_AUINT8,ulSize);
	if(GetMovThumbnailAndNumElements(oMTPDBFileInfo[nObjectHandle].szDosFileName, (FW_ULONG)(IN_Buffers+*ulSize), 65536 - 12, &length) != FJ_ERR_OK)
	{
		// Error 
		BF_Debug_Print_Error(("xAddAllMOVObjectPorpValueList GetMovThumbnail error!\n"));
	}
	*ulSize = *ulSize + length;

	mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);				
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&oMOVProp.dwWidth,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&oMOVProp.dwHeight,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&oMOVProp.dwDuration,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&oMOVProp.dwSampleRate,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&oMOVProp.dwNumberOfChannels,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&oMOVProp.dwAudioBitRate,ulSize);
	
	return 0;
}

INT32 xAddAllMP4ObjectPorpValueList(INT32 nObjectHandle,ULONG dwObjectSize,INT32 nParentObject,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 	ulTempValue = 0;
	USHORT 	usTempValue = 0;
	MOVPROP	oMOVProp;
	T_BF_FS_IF_STAT	file_stat;
	UINT32	thumbX = 0, thumbY = 0, ThumbcompressSize = 0;
	ULONG 	length = 0;

//	if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1) {
//		ulTempValue = PTP_StorageID_SD;
//	}
//	else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF) {
//		ulTempValue = PTP_StorageID_NF;	
//	}
//	else{
//		ulTempValue = PTP_StorageID_MS;	
//	}
	ulTempValue = PtpStorageID;

	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
	usTempValue = MTP_OFC_MP4; 
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize);
	BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);

	if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
	{
		usTempValue = PROTECT_ReadOnly;
	}
	else
	{
		usTempValue = PROTECT_NoProtect;
	}

	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);
	ulTempValue = nParentObject;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);

	// 0xDC81,0xDC82,0xDC83,0xDC84,0xDC86
	if(GetMovInfo(&ThumbcompressSize, &thumbX, &thumbY, &thumbX, &thumbY, oMTPDBFileInfo[nObjectHandle].szDosFileName) != FJ_ERR_OK)
	{
		// Error 
		BF_Debug_Print_Error(("xAddAllMP4ObjectPorpValueList GetMovInfo error!\n"));
	}

	usTempValue = MTP_OFC_EXIFJPEG;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleFormat,MTP_DATATYPE_UINT16,&usTempValue,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleSize,MTP_DATATYPE_UINT32,&ThumbcompressSize,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleHeigth,MTP_DATATYPE_UINT32,&thumbY,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleWidth,MTP_DATATYPE_UINT32,&thumbX,ulSize);

	xAddObjectPorpValueListNoValue(nObjectHandle,MTP_OPC_REPSampleData,MTP_DATATYPE_AUINT8,ulSize);
	if(GetMovThumbnailAndNumElements(oMTPDBFileInfo[nObjectHandle].szDosFileName, (FW_ULONG)(IN_Buffers+*ulSize), 65536 - 12, &length) != FJ_ERR_OK)
	{
		// Error 
		BF_Debug_Print_Error(("xAddAllMP4ObjectPorpValueList GetMovThumbnail error!\n"));
	}

	*ulSize = *ulSize + length;

	mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&oMOVProp.dwWidth,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&oMOVProp.dwHeight,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&oMOVProp.dwDuration,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&oMOVProp.dwSampleRate,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&oMOVProp.dwNumberOfChannels,ulSize);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&oMOVProp.dwAudioBitRate,ulSize);

	return 0;
}

INT32 xAddAllWAVObjectPorpValueList(INT32 nObjectHandle,ULONG dwObjectSize,INT32 nParentObject,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	WAVPROP			oWavProp;
	T_BF_FS_IF_STAT			file_stat;

//	if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//		ulTempValue = PTP_StorageID_SD;	
//	}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//		ulTempValue = PTP_StorageID_NF;	
//	}else{
//		ulTempValue = PTP_StorageID_MS;	
//	}
	ulTempValue = PtpStorageID;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);																	
	usTempValue = MTP_OFC_WAV; 
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 												
	//usTempValue = PROTECT_NoProtect;	
	// 2009/03/31 STX tony +
	BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
	if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
	{
		usTempValue = PROTECT_ReadOnly;
	}
	else
	{
		usTempValue = PROTECT_NoProtect;
	}
	// 2009/03/31 STX tony -	
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 																
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);												
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);								
	ulTempValue = nParentObject;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize); 					
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);																	
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);									
	wav_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oWavProp);
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&oWavProp.dwDuration,ulSize); 					
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&oWavProp.dwSampleRate,ulSize); 					
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&oWavProp.dwNumberOfChannels,ulSize);						
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&oWavProp.dwAudioBitRate,ulSize); 									

	return 0;
}

INT32 xAddAllBMPObjectPorpValueList(INT32 nObjectHandle,ULONG dwObjectSize,INT32 nParentObject,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	BMPPROP			oBmpProp;
	T_BF_FS_IF_STAT			file_stat;

//	if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//		ulTempValue = PTP_StorageID_SD;	
//	}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//		ulTempValue = PTP_StorageID_NF;	
//	}else{
//		ulTempValue = PTP_StorageID_MS;	
//	}
	ulTempValue = PtpStorageID;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);																	
	usTempValue = MTP_OFC_BMP; 
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 												
	//usTempValue = PROTECT_NoProtect;	
	// 2009/03/31 STX tony +
	BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
	if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
	{
		usTempValue = PROTECT_ReadOnly;
	}
	else
	{
		usTempValue = PROTECT_NoProtect;
	}
	// 2009/03/31 STX tony -	
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 																
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);												
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);								
	ulTempValue = nParentObject;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize); 					
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);																	
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);								

	bmp_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oBmpProp);
	ulTempValue = oBmpProp.dwWidth;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);							
	ulTempValue = oBmpProp.dwHeight;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
	usTempValue = oBmpProp.wImageBitDepth;
	usTempValue = ENDIAN_SWAP16(usTempValue);
	ulTempValue = usTempValue;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_IMAGEBITDEPTH,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);

	return 0;	
}

INT32 xAddAllGIFObjectPorpValueList(INT32 nObjectHandle,ULONG dwObjectSize,INT32 nParentObject,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	GIFPROP			oGifProp;
	T_BF_FS_IF_STAT			file_stat;

//	if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//		ulTempValue = PTP_StorageID_SD;	
//	}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//		ulTempValue = PTP_StorageID_NF;	
//	}else{
//		ulTempValue = PTP_StorageID_MS;	
//	}
	ulTempValue = PtpStorageID;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);																	
	usTempValue = MTP_OFC_GIF; 
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 												
	//usTempValue = PROTECT_NoProtect;	
	// 2009/03/31 STX tony +
	BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
	if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
	{
		usTempValue = PROTECT_ReadOnly;
	}
	else
	{
		usTempValue = PROTECT_NoProtect;
	}
	// 2009/03/31 STX tony -	
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 																
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);												
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);								
	ulTempValue = nParentObject;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize); 					
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);																	
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);								

	gif_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oGifProp);
	usTempValue = oGifProp.wWidth;
	usTempValue = ENDIAN_SWAP16(usTempValue);
	ulTempValue = usTempValue;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);							
	usTempValue = oGifProp.wHeight;
	usTempValue = ENDIAN_SWAP16(usTempValue);
	ulTempValue = usTempValue;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
	ulTempValue = oGifProp.cImageBitDepth;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_IMAGEBITDEPTH,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);

	return 0;	
}

INT32 xAddAllTIFFObjectPorpValueList(INT32 nObjectHandle,ULONG dwObjectSize,INT32 nParentObject,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	TIFFPROP		oTiffProp;
	T_BF_FS_IF_STAT			file_stat;

//	if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//		ulTempValue = PTP_StorageID_SD;	
//	}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//		ulTempValue = PTP_StorageID_NF;	
//	}else{
//		ulTempValue = PTP_StorageID_MS;	
//	}
	ulTempValue = PtpStorageID;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);																	
	usTempValue = MTP_OFC_TIFF; 
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 												
	//usTempValue = PROTECT_NoProtect;	
	// 2009/03/31 STX tony +
	BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
	if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
	{
		usTempValue = PROTECT_ReadOnly;
	}
	else
	{
		usTempValue = PROTECT_NoProtect;
	}
	// 2009/03/31 STX tony -	
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 																
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);												
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);								
	ulTempValue = nParentObject;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize); 					
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);																	
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);								
	tiff_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oTiffProp);
	ulTempValue = oTiffProp.dwWidth;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);							
	ulTempValue = oTiffProp.dwHeight;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
	ulTempValue = oTiffProp.wImageBitDepth;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_IMAGEBITDEPTH,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);

	return 0;	
}

INT32 xAddAllMP3ObjectPorpValueList(INT32 nObjectHandle,ULONG dwObjectSize,INT32 nParentObject,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	MP3PROP			oMP3Prop;
	T_BF_FS_IF_STAT			file_stat;

//	if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//		ulTempValue = PTP_StorageID_SD;	
//	}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//		ulTempValue = PTP_StorageID_NF;	
//	}else{
//		ulTempValue = PTP_StorageID_MS;	
//	}
	ulTempValue = PtpStorageID;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);																	
	usTempValue = MTP_OFC_MP3; 
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 												
	//usTempValue = PROTECT_NoProtect;	
	// 2009/03/31 STX tony +
	BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
	if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
	{
		usTempValue = PROTECT_ReadOnly;
	}
	else
	{
		usTempValue = PROTECT_NoProtect;
	}
	// 2009/03/31 STX tony -	
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 																
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);												
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);								
	ulTempValue = nParentObject;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize); 					
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);																	
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);								
	mp3_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMP3Prop);
	ulTempValue = oMP3Prop.nDuration;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);		
	ulTempValue = oMP3Prop.fMP3SampleRate;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);					
	usTempValue = oMP3Prop.nNumberOfChannels;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&usTempValue,ulSize);
	ulTempValue = oMP3Prop.fMP3BitRate;
	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);	
	return 0;
}

INT32 xAddSingleAssociationObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;

	switch (usObjectPropCode)
	{		
		case MTP_OPC_STORAGEID:// 0xDC01 : StorageID
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);							
			break;	
			
		case MTP_OPC_OBJECTFORMAT:// 0xDC02 : ObjectFormat						
			usTempValue = MTP_OFC_ASSOCIATION;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 						
			break;		
			
		case MTP_OPC_PROTECTIONSTATUS:// 0xDC03 : ProtectStatus 			
			usTempValue = PROTECT_NoProtect;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 						
			break;		
			
		case MTP_OPC_OBJECTSIZE:// 0xDC04 : Object Size 				
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);							
			break;	
			
		case MTP_OPC_OBJECTFILENAME:// 0xDC07 : File Name
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;								
			
		case MTP_OPC_PARENTOBJECT:// 0xDC0B : Parnet Object 
			ulTempValue = nParentObject;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;		
			
		case MTP_OPC_PUOI:// 0xDC0B : PUOI
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);							
			break;	
			
		case MTP_OPC_NAME:// 0xDC44 : Object  name
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;								
	}

	return 0;
}

INT32 xAddSendSingleJPGObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	JFIFPROP		oJFIFProp;
	UCHAR			*Addr;
	//char 			szTempString[16];
	char 			szDate[32];
	char 			szAscDate[16];
	char 			szUniDate[32];		
	//char 			szAscDate[16];
	//char 			szUniDate[32];		
	T_BF_FS_IF_STAT	file_stat;	
	UINT32			picX=0,picY=0,thumbX=0,thumbY=0,ThumbcompressSize=0;
	ULONG 			length = 0;

	memset(szAscDate, 0, sizeof(szAscDate));
	
	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID: 				
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			Addr = IN_Buffers+12;	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG));					
			break;
	
		case MTP_OPC_OBJECTFORMAT:
			usTempValue = MTP_OFC_EXIFJPEG;
			Addr = IN_Buffers+12;	
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);					
			SendData(sizeof(USHORT));					
			break;
			
		case MTP_OPC_PROTECTIONSTATUS:
			//usTempValue = 0x0000;
			// 2009/03/31 STX tony +
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			// 2009/03/31 STX tony -			
			Addr = IN_Buffers+12;	
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);			
			SendData(sizeof(USHORT));					
			break;
					
		case MTP_OPC_OBJECTSIZE:	
			ulTempValue = dwObjectSize; 
			Addr = IN_Buffers+12;	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG)*2);					
			break;
			
		// 2009/03/31 STX tony +
		//case MTP_OPC_ASSOCIATIONTYPE:
		//	usTempValue = 0x0000;
		//	Addr = IN_Buffers+12;	
		//	*Addr=(UCHAR)(usTempValue&0x00FF);
		//	*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);			
		//	SendData(sizeof(USHORT));					
		//	break;
		//
		//case MTP_OPC_ASSOCIATIONDESC:
		//	usTempValue = 0x0000;
		//	Addr = IN_Buffers+12;	
		//	*Addr=(UCHAR)(usTempValue&0x00FF);
		//	*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);			
		//	SendData(sizeof(USHORT));					
		//	break;	
		// 2009/03/31 STX tony +
		
		case MTP_OPC_OBJECTFILENAME:
			IN_Buffers[12] = DOS_FILENAME_LEN;					
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 				
			break;

		case MTP_OPC_DATECREATED:
		// 2009/04/09 STX tony +
		IN_Buffers[12] = MTP_DATETIME_LENGTH;
		PTP_fp = BF_Fs_If_Open(oMTPDBFileInfo[nObjectHandle].szDosFileName, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
		GetJpegPicInfo(&ThumbcompressSize, &thumbX, &thumbY, &picX, &picY, szAscDate);

		if(strlen(szAscDate) > 0)
		{
			ASCIIToUniCode(szAscDate,szUniDate);	
		}
		else
		{
			memset(szUniDate,0,MTP_DATETIME_LENGTH*2);
		}	
		memcpy(IN_Buffers+12+1,szUniDate,MTP_DATETIME_LENGTH*2);
		SendData(MTP_DATETIME_LENGTH*2+1); 
		BF_Fs_If_Close(PTP_fp);
		PTP_fp=0;
		// 2009/04/09 STX tony -
			break;

		case MTP_OPC_DATEMODIFIED:
			IN_Buffers[12] = MTP_DATETIME_LENGTH;			
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);
			snprintf(szAscDate, sizeof(szAscDate), "%04d%02d%02d%c%02d%02d%02d", ((file_stat.date >> 9) + 1980), (file_stat.date >> 5) & 0x0f, (file_stat.date) & 0x1f, \
						   0x54, (file_stat.time >> 11) & 0x1f, (file_stat.time >> 5) & 0x3f, (file_stat.time & 0x1f) * 2);			
			ASCIIToUniCode(szAscDate,szUniDate); 		
			memcpy(IN_Buffers+12+1,szUniDate,MTP_DATETIME_LENGTH*2);			
			SendData(MTP_DATETIME_LENGTH*2+1); 				
			break;			

		//case MTP_OPC_KEYWORDS:
		//	IN_Buffers[12] = 0;					
			//memcpy(IN_Buffers+0,cFileName,0*2);
		//	SendData(0*2+1); 				
		//	break;
		// 2009/03/31 STX tony -
			
		case MTP_OPC_PARENTOBJECT:			
			ulTempValue = nParentObject;			
			Addr = IN_Buffers+12;	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ulTempValue));					
			break;

		case MTP_OPC_PUOI: // 0xDC41
			ulTempValue = nObjectHandle; 
			memset(IN_Buffers+12,0,sizeof(ulTempValue) * 4);
			Addr = IN_Buffers+12;	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG) * 4);	
			break;
		
		case MTP_OPC_NAME:	 // 0xDC44
			IN_Buffers[12] = DOS_FILENAME_LEN;		
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 
			break;		


	// 03/26/09 tony +
		case MTP_OPC_REPSampleFormat:
			usTempValue = 14344;
			Addr = IN_Buffers+12;	
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);			
			SendData(sizeof(USHORT));					
			break;

		case MTP_OPC_REPSampleSize:			
			Addr = IN_Buffers+12;	
			PTP_fp = BF_Fs_If_Open(oMTPDBFileInfo[nObjectHandle].szDosFileName, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
			if(GetJpegPicInfo(&ThumbcompressSize, &thumbX, &thumbY, &picX, &picY, szDate) != FJ_ERR_OK)
			{
				BF_Debug_Print_Error(("xAddSendSingleJPGObjectPorpValueList GetJpegPicInfo error\n"));
				// Error 
				ulTempValue = 0x00000000;
			}
			else
			{
				ulTempValue = ThumbcompressSize;
			}
			BF_Fs_If_Close(PTP_fp);
			PTP_fp=0;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ulTempValue));					
			break;
			
		case MTP_OPC_REPSampleHeigth:			
			ulTempValue = 120;			
			Addr = IN_Buffers+12;	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ulTempValue));					
			break;

		case MTP_OPC_REPSampleWidth:			
			ulTempValue = 160;			
			Addr = IN_Buffers+12;	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ulTempValue));					
			break;			
	// 03/26/09 tony -

		case MTP_OPC_REPSampleData:
			Addr = IN_Buffers+12;
			// Size
			PTP_fp = BF_Fs_If_Open(oMTPDBFileInfo[nObjectHandle].szDosFileName, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
			if(GetJpegPicInfo(&ThumbcompressSize, &thumbX, &thumbY, &picX, &picY, szDate) == FJ_ERR_OK)
			{
				*Addr=(UCHAR)(ThumbcompressSize&0x000000FF);
				*(Addr+1)=(UCHAR)((ThumbcompressSize&0x0000FF00)>>8);
				*(Addr+2)=(UCHAR)((ThumbcompressSize&0x00FF0000)>>16);
				*(Addr+3)=(UCHAR)((ThumbcompressSize&0xFF000000)>>24);
			
				// Data
				if(GetJpegThumbnail(oMTPDBFileInfo[nObjectHandle].szDosFileName,
									(FW_ULONG)(Addr+MTP_MAX_LENGTH_FIELD_SIZE), 65536 - 12, &length) != FJ_ERR_OK )
				{
					// Error 
					BF_Debug_Print_Error(("xAddSendSingleJPGObjectPorpValueList GetJpegThumbnail error\n"));
					memset(Addr, 0x00, MTP_MAX_LENGTH_FIELD_SIZE);
					ThumbcompressSize = 0x00000000;
				}
				else
				{
					ThumbcompressSize = length;
				}
			}
			else
			{
				BF_Debug_Print_Error(("xAddSendSingleJPGObjectPorpValueList GetJpegPicInfo error\n"));
				memset(Addr, 0x00, MTP_MAX_LENGTH_FIELD_SIZE);
			}
			BF_Fs_If_Close(PTP_fp);
			PTP_fp=0;
			SendData(ThumbcompressSize + MTP_MAX_LENGTH_FIELD_SIZE);
			break;

		case MTP_OPC_WIDTH: 
			jfif_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oJFIFProp);
			ulTempValue = oJFIFProp.wWidth;	
			Addr = IN_Buffers+12;	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));
			break;
		
		case MTP_OPC_HEIGHT: 
			jfif_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oJFIFProp);		
			ulTempValue = oJFIFProp.wHeight;	
			Addr = IN_Buffers+12;	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));					
			break;
		
		case MTP_OPC_IMAGEBITDEPTH: 
			jfif_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oJFIFProp);			
			ulTempValue = oJFIFProp.cImageBitDepth;
			ulTempValue = ulTempValue * 3;
			
			Addr = IN_Buffers+12;	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);	

			SendData(sizeof(ULONG));					
			break;			
	}

	return 0;
}

INT32 xAddSendSingleAVIObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName)
{
	ULONG			ulTempValue=0;
	USHORT			usTempValue=0;
	AVIPROP			oAVIProp;
	UCHAR			*Addr;
	T_BF_FS_IF_STAT			file_stat;
	char 			szTempString[16];
	char 			szDate[32];	

	memset(szTempString, 0, sizeof(szTempString));
	
	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID: 				
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));					
			break;

		case MTP_OPC_OBJECTFORMAT:
			usTempValue = MTP_OFC_AVI;		
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);				
			SendData(sizeof(USHORT));					
			break;
			
		case MTP_OPC_PROTECTIONSTATUS:
			//usTempValue = 0x0000;		
			// 2009/03/31 STX tony +
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			// 2009/03/31 STX tony -			
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);				
			SendData(sizeof(USHORT));					
			break;
					
		case MTP_OPC_OBJECTSIZE:	
			ulTempValue = dwObjectSize; 
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG)*2);					
			break;

		case MTP_OPC_OBJECTFILENAME:
			IN_Buffers[12] = DOS_FILENAME_LEN;					
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 				
			break;

		case MTP_OPC_DATEMODIFIED:
			IN_Buffers[12] = 16;			
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);
			snprintf(szTempString, sizeof(szTempString), "%04d%02d%02d%c%02d%02d%02d", ((file_stat.date >> 9) + 1980), (file_stat.date >> 5) & 0x0f, (file_stat.date) & 0x1f, \
						   0x54, (file_stat.time >> 11) & 0x1f, (file_stat.time >> 5) & 0x3f, (file_stat.time & 0x1f) * 2);
			
			ASCIIToUniCode(szTempString,szDate); 		
			memcpy(IN_Buffers+12+1,szDate,16*2);			
			//memcpy(IN_Buffers+16,cFileName,16*2);
			SendData(16*2+1); 				
			break;	
			
		case MTP_OPC_PARENTOBJECT:
			ulTempValue = nParentObject;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ulTempValue));					
			break;

		case MTP_OPC_PUOI:
			ulTempValue = nObjectHandle; 
			memset(IN_Buffers+12,0,sizeof(ulTempValue) * 4);
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG) * 4);	
			break;
		
		case MTP_OPC_NAME:	
			IN_Buffers[12] = DOS_FILENAME_LEN;		
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 
			break;		

		case MTP_OPC_WIDTH: 
			avi_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oAVIProp);
			ulTempValue = oAVIProp.dwWidth;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));
			break;
		
		case MTP_OPC_HEIGHT: 
			avi_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oAVIProp);
			ulTempValue = oAVIProp.dwHeight;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));					
			break;
		
		case MTP_OPC_DURATION: 
			avi_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oAVIProp);
			ulTempValue = oAVIProp.dwDuration;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));
			break;

		case MTP_OPC_SAMPLERATE: 
			avi_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oAVIProp);
			ulTempValue = oAVIProp.dwSampleRate;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));
			break;

		case MTP_OPC_NUMBEROFCHANNEL: 
			avi_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oAVIProp);
			usTempValue = oAVIProp.dwNumberOfChannels;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);				
			SendData(sizeof(USHORT));					
			break;

		case MTP_OPC_SCANTYPE: 
			avi_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oAVIProp);
			usTempValue = oAVIProp.dwScanType;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);				
			SendData(sizeof(USHORT));					
			break;

		case MTP_OPC_AUDIOBITRATE: 
			avi_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oAVIProp);
			ulTempValue = oAVIProp.dwAudioBitRate;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));
			break;

		case MTP_OPC_VIDEOFOURCCCODEC: 
			avi_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oAVIProp);
			ulTempValue = oAVIProp.dwVideoFourCCCodec;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));					
			break;

		case MTP_OPC_VIDEOBITRATE: 
			avi_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oAVIProp);
			ulTempValue = oAVIProp.dwVideoBitRate;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));						
			break;
	}

	return 0;
}


INT32 xAddSendSingleDPOFObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName)
{
	ULONG			ulTempValue=0;
	USHORT			usTempValue=0;
	UCHAR			*Addr;
	T_BF_FS_IF_STAT			file_stat;

	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID: 	// 0xDC01
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			Addr = IN_Buffers+12;	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG));					
			break;

		case MTP_OPC_OBJECTFORMAT: // 0xDC02
			usTempValue = MTP_OFC_DPOF;		
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);					
			SendData(sizeof(USHORT));					
			break;
			
		case MTP_OPC_PROTECTIONSTATUS: // 0xDC03
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);				
			SendData(sizeof(USHORT));					
			break;
					
		case MTP_OPC_OBJECTSIZE: // 0xDC04
			ulTempValue = dwObjectSize; 
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG)*2);					
			break;

		case MTP_OPC_OBJECTFILENAME: // 0xDC07
			IN_Buffers[12] = DOS_FILENAME_LEN;					
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 				
			break;
			
		case MTP_OPC_PARENTOBJECT:			
			ulTempValue = nParentObject;			
			Addr = IN_Buffers+12;	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ulTempValue));					
			break;

		case MTP_OPC_PUOI: // 0xDC41
			ulTempValue = nObjectHandle; 
			memset(IN_Buffers+12,0,sizeof(ulTempValue) * 4);
			Addr = IN_Buffers+12;	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG) * 4);	
			break;
		
		case MTP_OPC_NAME:	 // 0xDC44
			IN_Buffers[12] = DOS_FILENAME_LEN;		
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 
			break;		
	}

	return 0;
}

INT32 xAddSendSingleMOVObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName)
{
	ULONG			ulTempValue=0;
	USHORT			usTempValue=0;
	MOVPROP			oMOVProp;
	UCHAR			*Addr;
	T_BF_FS_IF_STAT			file_stat;
	UINT32			thumbX=0,thumbY=0,ThumbcompressSize=0;
	ULONG 			length = 0;

	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID: 	// 0xDC01
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));					
			break;

		case MTP_OPC_OBJECTFORMAT: // 0xDC02
			usTempValue = MTP_OFC_MOV;		
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);				
			SendData(sizeof(USHORT));					
			break;
			
		case MTP_OPC_PROTECTIONSTATUS: // 0xDC03
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);				
			SendData(sizeof(USHORT));					
			break;
					
		case MTP_OPC_OBJECTSIZE: // 0xDC04
			ulTempValue = dwObjectSize; 
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG)*2);					
			break;

		case MTP_OPC_OBJECTFILENAME: // 0xDC07
			IN_Buffers[12] = DOS_FILENAME_LEN;					
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 				
			break;
			
		case MTP_OPC_PARENTOBJECT: // 0xDC0B
			ulTempValue = nParentObject;						
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ulTempValue));					
			break;

		case MTP_OPC_PUOI: // 0xDC41
			ulTempValue = nObjectHandle; 
			memset(IN_Buffers+12,0,sizeof(ulTempValue) * 4);
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG) * 4);	
			break;
		
		case MTP_OPC_NAME:	 // 0xDC44
			IN_Buffers[12] = DOS_FILENAME_LEN;		
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 
			break;		

		case MTP_OPC_REPSampleFormat: // 0xDC81
			usTempValue = 0x3808;
			Addr = IN_Buffers+12;	
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);			
			SendData(sizeof(USHORT));					
			break;

		case MTP_OPC_REPSampleSize: // 0xDC82
			Addr = IN_Buffers+12;
			if(GetMovInfo(&ThumbcompressSize, &thumbX, &thumbY, &thumbX, &thumbY, oMTPDBFileInfo[nObjectHandle].szDosFileName) != FJ_ERR_OK)
			{
				// Error 
				BF_Debug_Print_Error(("xAddSendSingleMOVObjectPorpValueList GetMovInfo error\n"));
				ulTempValue = 0x00000000;
			}
			else
			{
				ulTempValue = ThumbcompressSize;
			}
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ulTempValue));					
			break;

		case MTP_OPC_REPSampleHeigth: // 0xDC83
			ulTempValue = 120;			
			Addr = IN_Buffers+12;	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ulTempValue));					
			break;

		case MTP_OPC_REPSampleWidth: // 0xDC84
			ulTempValue = 160;			
			Addr = IN_Buffers+12;	
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);	
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ulTempValue));					
			break;			

		case MTP_OPC_REPSampleData: // 0xDC86
			// Write Length
			Addr = IN_Buffers+12;
			// Write Data
			if(GetMovThumbnailAndNumElements(oMTPDBFileInfo[nObjectHandle].szDosFileName, (FW_ULONG)(IN_Buffers+12), 65536 - 12, &length) != FJ_ERR_OK)
			{
				// Error 
				BF_Debug_Print_Error(("xAddSendSingleMOVObjectPorpValueList GetMovThumbnail error\n"));
			}
			ThumbcompressSize = length;
			SendData(ThumbcompressSize);
			break;

		case MTP_OPC_WIDTH:  // 0xDC87
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			ulTempValue = oMOVProp.dwWidth;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));
			break;
		
		case MTP_OPC_HEIGHT:  // 0xDC88
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			ulTempValue = oMOVProp.dwHeight;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));					
			break;
		
		case MTP_OPC_DURATION:  // 0xDC89
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			ulTempValue = oMOVProp.dwDuration;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));
			break;

		case MTP_OPC_SAMPLERATE:  // 0xDE93
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			ulTempValue = oMOVProp.dwSampleRate;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));
			break;

		case MTP_OPC_NUMBEROFCHANNEL:  // 0xDE94
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			usTempValue = oMOVProp.dwNumberOfChannels;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);				
			SendData(sizeof(USHORT));					
			break;

		case MTP_OPC_AUDIOBITRATE:  // 0xDE9A
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			ulTempValue = oMOVProp.dwAudioBitRate;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));
			break;
	}

	return 0;
}

INT32 xAddSendSingleMP4ObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName)
{
	ULONG			ulTempValue=0;
	USHORT			usTempValue=0;
	MOVPROP			oMOVProp;
	UCHAR			*Addr;
	T_BF_FS_IF_STAT			file_stat;
	UINT32			thumbX=0,thumbY=0,ThumbcompressSize=0;
	ULONG 			length = 0;

	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID: 	// 0xDC01
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1) {
//				ulTempValue = PTP_StorageID_SD;	
//			}
//			else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF) {
//				ulTempValue = PTP_StorageID_NF;	
//			}
//			else {
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);
			SendData(sizeof(ULONG));
			break;

		case MTP_OPC_OBJECTFORMAT: // 0xDC02
			usTempValue = MTP_OFC_MP4;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);
			SendData(sizeof(USHORT));
			break;

		case MTP_OPC_PROTECTIONSTATUS: // 0xDC03
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);
			SendData(sizeof(USHORT));
			break;

		case MTP_OPC_OBJECTSIZE: // 0xDC04
			ulTempValue = dwObjectSize; 
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);
			SendData(sizeof(ULONG)*2);
			break;

		case MTP_OPC_OBJECTFILENAME: // 0xDC07
			IN_Buffers[12] = DOS_FILENAME_LEN;
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1);
			break;

		case MTP_OPC_PARENTOBJECT: // 0xDC0B
			ulTempValue = nParentObject;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);
			SendData(sizeof(ulTempValue));
			break;

		case MTP_OPC_PUOI: // 0xDC41
			ulTempValue = nObjectHandle;
			memset(IN_Buffers+12,0,sizeof(ulTempValue) * 4);
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);
			SendData(sizeof(ULONG) * 4);
			break;

		case MTP_OPC_NAME:	 // 0xDC44
			IN_Buffers[12] = DOS_FILENAME_LEN;
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1);
			break;

		case MTP_OPC_REPSampleFormat: // 0xDC81
			usTempValue = 0x3808;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);
			SendData(sizeof(USHORT));
			break;

		case MTP_OPC_REPSampleSize: // 0xDC82
			Addr = IN_Buffers+12;
			if(GetMovInfo(&ThumbcompressSize, &thumbX, &thumbY, &thumbX, &thumbY, oMTPDBFileInfo[nObjectHandle].szDosFileName) != FJ_ERR_OK)
			{
				// Error 
				BF_Debug_Print_Error(("xAddSendSingleMP4ObjectPorpValueList GetMovInfo error\n"));
				ulTempValue = 0x00000000;
			}
			else
			{
				ulTempValue = ThumbcompressSize;
			}
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);
			SendData(sizeof(ulTempValue));
			break;

		case MTP_OPC_REPSampleHeigth: // 0xDC83
			ulTempValue = 120;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);
			SendData(sizeof(ulTempValue));
			break;

		case MTP_OPC_REPSampleWidth: // 0xDC84
			ulTempValue = 160;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);
			SendData(sizeof(ulTempValue));
			break;

		case MTP_OPC_REPSampleData: // 0xDC86
			// Write Length
			Addr = IN_Buffers+12;
			// Write Data
			if(GetMovThumbnailAndNumElements(oMTPDBFileInfo[nObjectHandle].szDosFileName, (FW_ULONG)(IN_Buffers+12), 65536 - 12, &length) != FJ_ERR_OK)
			{
				// Error 
				BF_Debug_Print_Error(("xAddSendSingleMP4ObjectPorpValueList GetMovThumbnail error\n"));
			}
			ThumbcompressSize = length;
			SendData(ThumbcompressSize);
			break;

		case MTP_OPC_WIDTH:  // 0xDC87
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			ulTempValue = oMOVProp.dwWidth;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);
			SendData(sizeof(ULONG));
			break;

		case MTP_OPC_HEIGHT:  // 0xDC88
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			ulTempValue = oMOVProp.dwHeight;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);
			SendData(sizeof(ULONG));
			break;

		case MTP_OPC_DURATION:  // 0xDC89
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			ulTempValue = oMOVProp.dwDuration;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);
			SendData(sizeof(ULONG));
			break;

		case MTP_OPC_SAMPLERATE:  // 0xDE93
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			ulTempValue = oMOVProp.dwSampleRate;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);
			SendData(sizeof(ULONG));
			break;

		case MTP_OPC_NUMBEROFCHANNEL:  // 0xDE94
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			usTempValue = oMOVProp.dwNumberOfChannels;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);
			SendData(sizeof(USHORT));
			break;

		case MTP_OPC_AUDIOBITRATE:  // 0xDE9A
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			ulTempValue = oMOVProp.dwAudioBitRate;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8);
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);
			SendData(sizeof(ULONG));
			break;
	}

	return 0;
}

INT32 xAddSendSingleWAVObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName)
{
	ULONG			ulTempValue=0;
	USHORT			usTempValue=0;
	WAVPROP 		oWavProp;
	UCHAR			*Addr;
	T_BF_FS_IF_STAT			file_stat;

	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID: 				
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG));					
			break;
	
		case MTP_OPC_OBJECTFORMAT:
			usTempValue = MTP_OFC_WAV;							
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);			
			SendData(sizeof(USHORT));					
			break;
			
		case MTP_OPC_PROTECTIONSTATUS:
			//usTempValue = 0x0000;		
			// 2009/03/31 STX tony +
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			// 2009/03/31 STX tony -			
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);			
			SendData(sizeof(USHORT));					
			break;
					
		case MTP_OPC_OBJECTSIZE:	
			ulTempValue = dwObjectSize;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG)*2);					
			break;
	
		case MTP_OPC_OBJECTFILENAME:
			IN_Buffers[12] = DOS_FILENAME_LEN;					
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 				
			break;
			
		case MTP_OPC_PARENTOBJECT:			
			ulTempValue = nParentObject;					
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ulTempValue));					
			break;
	
		case MTP_OPC_PUOI:
			ulTempValue = nObjectHandle; 
			memset(IN_Buffers+12,0,sizeof(ulTempValue) * 4);
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG) * 4);	
			break;
		
		case MTP_OPC_NAME:	
			IN_Buffers[12] = DOS_FILENAME_LEN;		
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 
			break;		
	
		case MTP_OPC_DURATION: 
			wav_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oWavProp);
			ulTempValue = oWavProp.dwDuration;					
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG));
			break;
	
		case MTP_OPC_SAMPLERATE: 
			wav_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oWavProp);
			ulTempValue = oWavProp.dwSampleRate;					
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG));
			break;
	
		case MTP_OPC_NUMBEROFCHANNEL: 
			wav_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oWavProp);
			usTempValue = oWavProp.dwNumberOfChannels;					
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);			
			SendData(sizeof(USHORT));					
			break;
			
		case MTP_OPC_AUDIOBITRATE: 
			wav_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oWavProp);
			ulTempValue = oWavProp.dwAudioBitRate;					

			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG));
			break;
	}

	return 0;
}

INT32 xAddSendSingleBMPObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName)
{
	UCHAR 			*Addr;
	ULONG			ulTempValue=0;
	USHORT			usTempValue=0;
	BMPPROP			oBmpProp;
	T_BF_FS_IF_STAT			file_stat;
	
	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID: 				
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG));					
			break;
	
		case MTP_OPC_OBJECTFORMAT:
			usTempValue = MTP_OFC_BMP;						
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);				
			SendData(sizeof(USHORT));					
			break;
			
		case MTP_OPC_PROTECTIONSTATUS:
			//usTempValue = 0x0000;	
			// 2009/03/31 STX tony +
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			// 2009/03/31 STX tony -			
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);				
			SendData(sizeof(USHORT));					
			break;
					
		case MTP_OPC_OBJECTSIZE:	
			ulTempValue = dwObjectSize;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG)*2);					
			break;
	
		case MTP_OPC_OBJECTFILENAME:
			IN_Buffers[12] = DOS_FILENAME_LEN;					
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 				
			break;
			
		case MTP_OPC_PARENTOBJECT:
			ulTempValue = nParentObject;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ulTempValue));					
			break;
	
		case MTP_OPC_PUOI:
			ulTempValue = nObjectHandle; 
			memset(IN_Buffers+12,0,sizeof(ulTempValue) * 4);
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG) * 4);	
			break;
		
		case MTP_OPC_NAME:	
			IN_Buffers[12] = DOS_FILENAME_LEN;		
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 
			break;			
	
		case MTP_OPC_WIDTH: 
			bmp_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oBmpProp);
			ulTempValue = oBmpProp.dwWidth;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG));
			break;
		
		case MTP_OPC_HEIGHT: 
			bmp_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oBmpProp);//
			ulTempValue = oBmpProp.dwHeight;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG));					
			break;
		
		case MTP_OPC_IMAGEBITDEPTH: 
			bmp_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oBmpProp);
			usTempValue = oBmpProp.wImageBitDepth;
			usTempValue = ENDIAN_SWAP16(usTempValue);
			ulTempValue = usTempValue;	
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG));					
			break;		
	}

	return 0;
}

INT32 xAddSendSingleGIFObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName)
{
	UCHAR 			*Addr;
	ULONG			ulTempValue=0;
	USHORT			usTempValue=0;	
	GIFPROP			oGifProp;
	T_BF_FS_IF_STAT			file_stat;
	
	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID: 				
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG));					
			break;
	
		case MTP_OPC_OBJECTFORMAT:
			usTempValue = MTP_OFC_GIF;						
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);			
			SendData(sizeof(USHORT));					
			break;
			
		case MTP_OPC_PROTECTIONSTATUS:
			//usTempValue = 0x0000;	
			// 2009/03/31 STX tony +
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			// 2009/03/31 STX tony -			
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);			
			SendData(sizeof(USHORT));					
			break;
					
		case MTP_OPC_OBJECTSIZE:	
			ulTempValue = dwObjectSize;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG)*2);					
			break;
	
		case MTP_OPC_OBJECTFILENAME:
			IN_Buffers[12] = DOS_FILENAME_LEN;					

			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 				
			break;
			
		case MTP_OPC_PARENTOBJECT:
			ulTempValue = nParentObject;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ulTempValue));					
			break;
	
		case MTP_OPC_PUOI:
			ulTempValue = nObjectHandle; 
			memset(IN_Buffers+12,0,sizeof(ulTempValue) * 4);
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG) * 4);	
			break;
		
		case MTP_OPC_NAME:	
			IN_Buffers[12] = DOS_FILENAME_LEN;		
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 
			break;			
	
		case MTP_OPC_WIDTH: 
			gif_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oGifProp);
			usTempValue =oGifProp.wWidth;
			usTempValue = ENDIAN_SWAP16(usTempValue);
			ulTempValue = usTempValue;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG));
			break;
		
		case MTP_OPC_HEIGHT: 
			gif_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oGifProp);
			usTempValue = oGifProp.wHeight;
			usTempValue = ENDIAN_SWAP16(usTempValue);
			ulTempValue = usTempValue;			 
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG));					
			break;
		
		case MTP_OPC_IMAGEBITDEPTH: 
			gif_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oGifProp);
			ulTempValue = oGifProp.cImageBitDepth;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG));					
			break;		
	}

	return 0;
}

INT32 xAddSendSingleTIFFObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName)
{
	UCHAR 			*Addr;
	ULONG			ulTempValue=0;
	USHORT			usTempValue=0;	
	TIFFPROP		oTiffProp;
	T_BF_FS_IF_STAT			file_stat;
	
	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID: 				
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);						
			SendData(sizeof(ULONG));					
			break;
	
		case MTP_OPC_OBJECTFORMAT:
			usTempValue = MTP_OFC_TIFF;						
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);			
			SendData(sizeof(USHORT));					
			break;
			
		case MTP_OPC_PROTECTIONSTATUS:
			//usTempValue = 0x0000;
			// 2009/03/31 STX tony +
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			// 2009/03/31 STX tony -			
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);			
			SendData(sizeof(USHORT));					
			break;
					
		case MTP_OPC_OBJECTSIZE:	
			ulTempValue = dwObjectSize;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);						
			SendData(sizeof(ULONG)*2);					
			break;
	
		case MTP_OPC_OBJECTFILENAME:
			IN_Buffers[12] = DOS_FILENAME_LEN;					

			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 				
			break;
			
		case MTP_OPC_PARENTOBJECT:	
			ulTempValue = nParentObject;				
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ulTempValue));					
			break;
	
		case MTP_OPC_PUOI:
			ulTempValue = nObjectHandle; 
			memset(IN_Buffers+12,0,sizeof(ulTempValue) * 4);
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);						
			SendData(sizeof(ULONG) * 4);	
			break;
		
		case MTP_OPC_NAME:	//0xDC44
			IN_Buffers[12] = DOS_FILENAME_LEN;		
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 
			break;			
	
		case MTP_OPC_WIDTH: // 0xDC87
			tiff_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oTiffProp);
			ulTempValue = oTiffProp.dwWidth;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);						
			SendData(sizeof(ULONG));
			break;
		
		case MTP_OPC_HEIGHT: // 0xDC88
			tiff_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oTiffProp);
			ulTempValue = oTiffProp.dwHeight;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG));					
			break;
		
		case MTP_OPC_IMAGEBITDEPTH: // 0xDCD3
			tiff_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oTiffProp);
			ulTempValue = oTiffProp.wImageBitDepth;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);			
			SendData(sizeof(ULONG));					
			break;		
	}
	return 0;
}

INT32 xAddSendSingleMP3ObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName)
{
	UCHAR 			*Addr;
	ULONG			ulTempValue=0;
	USHORT			usTempValue=0;
	MP3PROP 		omp3Prop;	
	T_BF_FS_IF_STAT			file_stat;
	
	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID: 				
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;

			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));					
			break;
	
		case MTP_OPC_OBJECTFORMAT:
			usTempValue = MTP_OFC_MP3;							
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);				
			SendData(sizeof(USHORT));					
			break;
			
		case MTP_OPC_PROTECTIONSTATUS:
			//usTempValue = 0x0000;		
			// 2009/03/31 STX tony +
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			// 2009/03/31 STX tony -			
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);				
			SendData(sizeof(USHORT));					
			break;
					
		case MTP_OPC_OBJECTSIZE:	
			ulTempValue = dwObjectSize;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG)*2);					
			break;
	
		case MTP_OPC_OBJECTFILENAME:
			IN_Buffers[12] = DOS_FILENAME_LEN;					
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 				
			break;
			
		case MTP_OPC_PARENTOBJECT:
			ulTempValue = nParentObject;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ulTempValue));					
			break;
	
		case MTP_OPC_PUOI:
			ulTempValue = nObjectHandle; 
			memset(IN_Buffers+12,0,sizeof(ulTempValue) * 4);
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG) * 4);	
			break;
		
		case MTP_OPC_NAME:	
			IN_Buffers[12] = DOS_FILENAME_LEN;		
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 
			break;		
	
		case MTP_OPC_DURATION: 
			mp3_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&omp3Prop);
			ulTempValue = omp3Prop.nDuration;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));
			break;
	
		case MTP_OPC_SAMPLERATE: 
			mp3_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&omp3Prop);
			ulTempValue = (unsigned long)omp3Prop.fMP3SampleRate;						
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));
			break;
	
		case MTP_OPC_NUMBEROFCHANNEL: 
			mp3_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&omp3Prop);
			usTempValue = (unsigned short)omp3Prop.nNumberOfChannels;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);				
			SendData(sizeof(USHORT));					
			break;
			
		case MTP_OPC_AUDIOBITRATE: 
			mp3_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&omp3Prop);
			ulTempValue = (unsigned long)omp3Prop.fMP3BitRate;						
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG));
			break;
	}

	return 0;
}

INT32 xAddSendSingleAssociationObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName)
{
	UCHAR 			*Addr;
	ULONG			ulTempValue=0;
	USHORT			usTempValue=0;	
	
	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID: 				
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);					
			SendData(sizeof(ULONG));					
			break;

		case MTP_OPC_OBJECTFORMAT:
			usTempValue = MTP_OFC_ASSOCIATION;		
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);					
			SendData(sizeof(USHORT));					
			break;
			
		case MTP_OPC_PROTECTIONSTATUS:
			//usTempValue = 0x0000;	
			// 2009/03/31 STX tony +
			usTempValue = PROTECT_NoProtect;			
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(usTempValue&0x00FF);
			*(Addr+1)=(UCHAR)((usTempValue&0xFF00)>>8);					
			SendData(sizeof(USHORT));					
			break;
					
		case MTP_OPC_OBJECTSIZE:	
			ulTempValue = 0;
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);				
			SendData(sizeof(ULONG)*2);					
			break;

		case MTP_OPC_OBJECTFILENAME:
			IN_Buffers[12] = DOS_FILENAME_LEN;					
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 				
			break;
			
		case MTP_OPC_PARENTOBJECT:							
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(nParentObject&0x000000FF);
			*(Addr+1)=(UCHAR)((nParentObject&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((nParentObject&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((nParentObject&0xFF000000)>>24);				
			SendData(sizeof(ULONG));					
			break;

		case MTP_OPC_PUOI:
			ulTempValue = nObjectHandle;	
			memset(IN_Buffers+12,0,sizeof(ulTempValue) * 4);
			Addr = IN_Buffers+12;
			*Addr=(UCHAR)(nObjectHandle&0x000000FF);
			*(Addr+1)=(UCHAR)((nObjectHandle&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((nObjectHandle&0x00FF0000)>>16);	
			*(Addr+3)=(UCHAR)((nObjectHandle&0xFF000000)>>24);				
			SendData(sizeof(ULONG) * 4);	
			break;
		
		case MTP_OPC_NAME:	//0xDC44
			IN_Buffers[12] = DOS_FILENAME_LEN;		
			memcpy(IN_Buffers+12+1,cFileName,DOS_FILENAME_LEN*2);
			SendData(DOS_FILENAME_LEN*2+1); 
			break;		
	}

	return 0;
}

INT32 xAddSingleJPGObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName,ULONG* ulSize)	
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	JFIFPROP		oJFIFProp;
	char 			szDate[32];
	//char 			szTempString[16];
	char 			szAscDate[16];
	char 			szUniDate[32];	
	T_BF_FS_IF_STAT	file_stat;
	UINT32			picX=0,picY=0,thumbX=0,thumbY=0,ThumbcompressSize=0;
	ULONG 			length = 0;
	UCHAR			*Addr;

	memset(szAscDate, 0, sizeof(szAscDate));

	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID:
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);							
			break;	
			
		case MTP_OPC_OBJECTFORMAT:				
			usTempValue = MTP_OFC_EXIFJPEG;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 						
			break;	
			
		case MTP_OPC_PROTECTIONSTATUS:		
			//usTempValue = PROTECT_NoProtect;
			// 2009/03/31 STX tony +
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			// 2009/03/31 STX tony -			
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 						
			break;	
			
		case MTP_OPC_OBJECTSIZE:		
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);
			break;		

		// 2009/03/31 STX tony +
		//case MTP_OPC_ASSOCIATIONTYPE:
		//	usTempValue = 0;
		//	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_ASSOCIATIONTYPE,MTP_DATATYPE_UINT16,&usTempValue,ulSize);
		//	break;	
		//case MTP_OPC_ASSOCIATIONDESC:	
		//	usTempValue = 0;
		//	xAddObjectPorpValueList(nObjectHandle,MTP_OPC_ASSOCIATIONDESC,MTP_DATATYPE_UINT16,&usTempValue,ulSize);
		//	break;		
		// 2009/03/31 STX tony -
			
		case MTP_OPC_OBJECTFILENAME:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;	


		// 2009/04/09 STX tony +
		case MTP_OPC_DATECREATED:
			PTP_fp = BF_Fs_If_Open(oMTPDBFileInfo[nObjectHandle].szDosFileName, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
			GetJpegPicInfo(&ThumbcompressSize, &thumbX, &thumbY, &picX, &picY, szAscDate);
			if(strlen(szAscDate) > 0)
			{
				ASCIIToUniCode(szAscDate,szUniDate);	
			}
			else
			{
				memset(szUniDate,0,MTP_DATETIME_LENGTH*2);
			}
			xAddObjectPorpValueListForVarLenString(nObjectHandle,MTP_OPC_DATECREATED,MTP_DATATYPE_STR,szUniDate,ulSize,MTP_DATETIME_LENGTH);	
			BF_Fs_If_Close(PTP_fp);
			PTP_fp=0;
			break;
		// 2009/04/09 STX tony -

		// 2009/03/31 STX tony +
		case MTP_OPC_DATEMODIFIED:
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);
			snprintf(szAscDate, sizeof(szAscDate), "%04d%02d%02d%c%02d%02d%02d", ((file_stat.date >> 9) + 1980), (file_stat.date >> 5) & 0x0f, (file_stat.date) & 0x1f, \
						   0x54, (file_stat.time >> 11) & 0x1f, (file_stat.time >> 5) & 0x3f, (file_stat.time & 0x1f) * 2);
			
			ASCIIToUniCode(szAscDate,szUniDate); 
			xAddObjectPorpValueListForVarLenString(nObjectHandle,MTP_OPC_DATEMODIFIED,MTP_DATATYPE_STR,szUniDate,ulSize,16);
			break;

		//case MTP_OPC_KEYWORDS:			
		//	xAddObjectPorpValueListForVarLenString(nObjectHandle,MTP_OPC_KEYWORDS,MTP_DATATYPE_STR,szDate,ulSize,0);
		//	break;
		// 2009/03/31 STX tony -
					
		case MTP_OPC_PARENTOBJECT:	
			ulTempValue = nParentObject;		
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;	
			
		case MTP_OPC_PUOI:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);							
			break;	
			
		case MTP_OPC_NAME:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;	


	// 03/26/09 tony +
		case MTP_OPC_REPSampleFormat:
			usTempValue = 14344;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleFormat,MTP_DATATYPE_UINT16,&usTempValue,ulSize);
			break;
		case MTP_OPC_REPSampleSize:			
			PTP_fp = BF_Fs_If_Open(oMTPDBFileInfo[nObjectHandle].szDosFileName, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
			if(GetJpegPicInfo(&ThumbcompressSize, &thumbX, &thumbY, &picX, &picY, szDate) != FJ_ERR_OK)
			{
				// Error 
				BF_Debug_Print_Error(("xAddSingleJPGObjectPorpValueList GetJpegPicInfo error\n"));
				ulTempValue = 0x00000000;
			}
			else
			{
				ulTempValue = ThumbcompressSize;
			}
			BF_Fs_If_Close(PTP_fp);
			PTP_fp=0;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleSize,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);				
			break;
			
		case MTP_OPC_REPSampleHeigth:			
			ulTempValue = 120;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleHeigth,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);				
			break;

		case MTP_OPC_REPSampleWidth:			
			ulTempValue = 160;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleWidth,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);				
			break;			
	// 03/26/09 tony -

		case MTP_OPC_REPSampleData:
			xAddObjectPorpValueListNoValue(nObjectHandle,MTP_OPC_REPSampleData,MTP_DATATYPE_AUINT8,ulSize);
			*ulSize = *ulSize + sizeof(UINT32);
			if(GetJpegThumbnail(oMTPDBFileInfo[nObjectHandle].szDosFileName, (FW_ULONG)(IN_Buffers+*ulSize), 65536 - 12, &length) != FJ_ERR_OK)
			{
				// Error 
				BF_Debug_Print_Error(("xAddSingleJPGObjectPorpValueList GetJpegThumbnail error\n"));
			}
			else
			{
				*ulSize = *ulSize + length;
			}
			
			// Write Thumbnail Size
			ulTempValue = length;
			Addr = IN_Buffers + *ulSize - length - sizeof(UINT32);
			*Addr=(UCHAR)(ulTempValue&0x000000FF);
			*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
			*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);
			*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);
			
			break;
		case MTP_OPC_WIDTH:
			jfif_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oJFIFProp);		
			ulTempValue = oJFIFProp.wWidth;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);							
			break;
			
		case MTP_OPC_HEIGHT:
			jfif_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oJFIFProp);	
			ulTempValue = oJFIFProp.wHeight;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;		
								
		case MTP_OPC_IMAGEBITDEPTH:
			jfif_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oJFIFProp);		
			ulTempValue = oJFIFProp.cImageBitDepth;
			ulTempValue = ulTempValue * 3;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_IMAGEBITDEPTH,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);																		
			break;
			
		default:
			SendResponseSimple(PTP_RS_ParameterNotSupported);
			return 1;
	}

	return 0;
}

INT32 xAddSingleAVIObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	AVIPROP			oAVIProp;
	T_BF_FS_IF_STAT			file_stat;
	char 			szTempString[16];
	char 			szDate[32];	
	
	memset(szTempString, 0, sizeof(szTempString));
	
	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID:
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;
			
		case MTP_OPC_OBJECTFORMAT:
			usTempValue = MTP_OFC_AVI; 
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize);
			break;	
			
		case MTP_OPC_PROTECTIONSTATUS:
			//usTempValue = PROTECT_NoProtect;
			// 2009/03/31 STX tony +
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			// 2009/03/31 STX tony -			
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize);
			break;
			
		case MTP_OPC_OBJECTSIZE:					
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);
			break;
	
		case MTP_OPC_OBJECTFILENAME:	
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;

		case MTP_OPC_DATEMODIFIED:
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);
			snprintf(szTempString, sizeof(szTempString), "%04d%02d%02d%c%02d%02d%02d", ((file_stat.date >> 9) + 1980), (file_stat.date >> 5) & 0x0f, (file_stat.date) & 0x1f, \
						   0x54, (file_stat.time >> 11) & 0x1f, (file_stat.time >> 5) & 0x3f, (file_stat.time & 0x1f) * 2);
			
			ASCIIToUniCode(szTempString,szDate); 
			xAddObjectPorpValueListForVarLenString(nObjectHandle,MTP_OPC_DATEMODIFIED,MTP_DATATYPE_STR,szDate,ulSize,16);
			break;
			
		case MTP_OPC_PARENTOBJECT:		
			ulTempValue = nParentObject;			
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;
			
		case MTP_OPC_PUOI: 
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);
			break;
	
		case MTP_OPC_NAME: 
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;
			
		case MTP_OPC_WIDTH:
			avi_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oAVIProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&oAVIProp.dwWidth,ulSize);											
			break;
		
		case MTP_OPC_HEIGHT:
			avi_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oAVIProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&oAVIProp.dwHeight,ulSize); 										
			break;
		
		case MTP_OPC_DURATION:
			avi_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oAVIProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&oAVIProp.dwDuration,ulSize); 													
			break;
			
		case MTP_OPC_SAMPLERATE:
			avi_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oAVIProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&oAVIProp.dwSampleRate,ulSize); 								
			break;
			
		case MTP_OPC_NUMBEROFCHANNEL:
			avi_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oAVIProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&oAVIProp.dwNumberOfChannels,ulSize);			
			break;
		
		case MTP_OPC_SCANTYPE: 
			avi_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oAVIProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_SCANTYPE,MTP_DATATYPE_UINT16,&oAVIProp.dwScanType,ulSize); 								
			break;
			
		case MTP_OPC_AUDIOBITRATE:
			avi_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oAVIProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&oAVIProp.dwAudioBitRate,ulSize); 				
		break;
			
		case MTP_OPC_VIDEOFOURCCCODEC:
			avi_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oAVIProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_VIDEOFOURCCCODEC,MTP_DATATYPE_UINT32,&oAVIProp.dwVideoFourCCCodec,ulSize); 								
			break;
		
		case MTP_OPC_VIDEOBITRATE:
			avi_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oAVIProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_VIDEOBITRATE,MTP_DATATYPE_UINT32,&oAVIProp.dwVideoBitRate,ulSize);			
			break;
			
		default:
			SendResponseSimple(PTP_RS_ParameterNotSupported);
			return 1;							
	}
	
	return 0;
}

INT32 xAddSingleDPOFObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	T_BF_FS_IF_STAT			file_stat;
	
	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID:
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;
			
		case MTP_OPC_OBJECTFORMAT:
			usTempValue = MTP_OFC_DPOF; 
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize);
			break;	
			
		case MTP_OPC_PROTECTIONSTATUS:
			//usTempValue = PROTECT_NoProtect;
			// 2009/03/31 STX tony +
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			// 2009/03/31 STX tony -			
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize);
			break;
			
		case MTP_OPC_OBJECTSIZE:					
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);
			break;
	
		case MTP_OPC_OBJECTFILENAME:	
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;

		case MTP_OPC_PARENTOBJECT:		
			ulTempValue = nParentObject;			
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;
			
		case MTP_OPC_PUOI: 
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);
			break;
	
		case MTP_OPC_NAME: 
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;

		default:
			SendResponseSimple(PTP_RS_ParameterNotSupported);
			return 1;							
	}
	
	return 0;
}


INT32 xAddSingleMOVObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	MOVPROP			oMOVProp;
	T_BF_FS_IF_STAT			file_stat;
	ULONG 			length = 0;
	UINT32			thumbX=0,thumbY=0,ThumbcompressSize=0;

	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID:
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;
			
		case MTP_OPC_OBJECTFORMAT:
			usTempValue = MTP_OFC_MOV; 
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize);
			break;	
			
		case MTP_OPC_PROTECTIONSTATUS:
			//usTempValue = PROTECT_NoProtect;
			// 2009/03/31 STX tony +
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			// 2009/03/31 STX tony -			
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize);
			break;
			
		case MTP_OPC_OBJECTSIZE:					
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);
			break;
	
		case MTP_OPC_OBJECTFILENAME:	
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;
			
		case MTP_OPC_PARENTOBJECT:		
			ulTempValue = nParentObject;			
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;
			
		case MTP_OPC_PUOI: 
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);
			break;
	
		case MTP_OPC_NAME: 
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;
		
		case MTP_OPC_REPSampleFormat:
			usTempValue = 14344;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleFormat,MTP_DATATYPE_UINT16,&usTempValue,ulSize);				
			break;

		case MTP_OPC_REPSampleSize:
			if(GetMovInfo(&ThumbcompressSize, &thumbX, &thumbY, &thumbX, &thumbY, oMTPDBFileInfo[nObjectHandle].szDosFileName) != FJ_ERR_OK)
			{
				// Error 
				BF_Debug_Print_Error(("xAddSingleMOVObjectPorpValueList GetMovInfo error\n"));
				ulTempValue = 0;
			}
			else
			{
				ulTempValue = ThumbcompressSize;
			}
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleSize,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;
			
		case MTP_OPC_REPSampleHeigth:			
			ulTempValue = 120;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleHeigth,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);				
			break;

		case MTP_OPC_REPSampleWidth:			
			ulTempValue = 160;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleWidth,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);				
			break;			

		case MTP_OPC_REPSampleData:
			xAddObjectPorpValueListNoValue(nObjectHandle,MTP_OPC_REPSampleData,MTP_DATATYPE_AUINT8,ulSize);
			if(GetMovThumbnailAndNumElements(oMTPDBFileInfo[nObjectHandle].szDosFileName, (FW_ULONG)(IN_Buffers+*ulSize), 65536 - 12, &length) != FJ_ERR_OK)
			{
				// Error 
				BF_Debug_Print_Error(("xAddSingleMOVObjectPorpValueList GetMovThumbnail error\n"));
			}
			*ulSize = *ulSize + length;
			break;

		case MTP_OPC_WIDTH:
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&oMOVProp.dwWidth,ulSize);											
			break;
		
		case MTP_OPC_HEIGHT:
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&oMOVProp.dwHeight,ulSize); 										
			break;
		
		case MTP_OPC_DURATION:
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&oMOVProp.dwDuration,ulSize); 													
			break;
			
		case MTP_OPC_SAMPLERATE:
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&oMOVProp.dwSampleRate,ulSize); 								
			break;
			
		case MTP_OPC_NUMBEROFCHANNEL:
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&oMOVProp.dwNumberOfChannels,ulSize);			
			break;
			
		case MTP_OPC_AUDIOBITRATE:
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&oMOVProp.dwAudioBitRate,ulSize); 				
		break;

		default:
			SendResponseSimple(PTP_RS_ParameterNotSupported);
			return 1;							
	}
	
	return 0;
}

INT32 xAddSingleMP4ObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	MOVPROP			oMOVProp;
	T_BF_FS_IF_STAT			file_stat;
	ULONG 			length = 0;
	UINT32			thumbX=0,thumbY=0,ThumbcompressSize=0;

	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID:
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1) {
//				ulTempValue = PTP_StorageID_SD;	
//			}
//			else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF) {
//				ulTempValue = PTP_StorageID_NF;	
//			}
//			else {
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;

		case MTP_OPC_OBJECTFORMAT:
			usTempValue = MTP_OFC_MP4; 
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize);
			break;

		case MTP_OPC_PROTECTIONSTATUS:
			//usTempValue = PROTECT_NoProtect;
			// 2009/03/31 STX tony +
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			// 2009/03/31 STX tony -
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize);
			break;

		case MTP_OPC_OBJECTSIZE:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);
			break;

		case MTP_OPC_OBJECTFILENAME:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;

		case MTP_OPC_PARENTOBJECT:
			ulTempValue = nParentObject;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;

		case MTP_OPC_PUOI: 
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);
			break;

		case MTP_OPC_NAME: 
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;

		case MTP_OPC_REPSampleFormat:
			usTempValue = 14344;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleFormat,MTP_DATATYPE_UINT16,&usTempValue,ulSize);				
			break;

		case MTP_OPC_REPSampleSize:
			if(GetMovInfo(&ThumbcompressSize, &thumbX, &thumbY, &thumbX, &thumbY, oMTPDBFileInfo[nObjectHandle].szDosFileName) != FJ_ERR_OK)
			{
				// Error 
				BF_Debug_Print_Error(("xAddSingleMP4ObjectPorpValueList GetMovInfo error\n"));
				ulTempValue = 0;
			}
			else
			{
				ulTempValue = ThumbcompressSize;
			}
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleSize,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;
			
		case MTP_OPC_REPSampleHeigth:
			ulTempValue = 120;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleHeigth,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;

		case MTP_OPC_REPSampleWidth:
			ulTempValue = 160;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_REPSampleWidth,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;

		case MTP_OPC_REPSampleData:
			xAddObjectPorpValueListNoValue(nObjectHandle,MTP_OPC_REPSampleData,MTP_DATATYPE_AUINT8,ulSize);
			if(GetMovThumbnailAndNumElements(oMTPDBFileInfo[nObjectHandle].szDosFileName, (FW_ULONG)(IN_Buffers+*ulSize), 65536 - 12, &length) != FJ_ERR_OK)
			{
				// Error 
				BF_Debug_Print_Error(("xAddSingleMP4ObjectPorpValueList GetMovThumbnail error\n"));
			}
			*ulSize = *ulSize + length;
			break;

		case MTP_OPC_WIDTH:
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&oMOVProp.dwWidth,ulSize);
			break;

		case MTP_OPC_HEIGHT:
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&oMOVProp.dwHeight,ulSize);
			break;

		case MTP_OPC_DURATION:
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&oMOVProp.dwDuration,ulSize);
			break;

		case MTP_OPC_SAMPLERATE:
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&oMOVProp.dwSampleRate,ulSize);
			break;

		case MTP_OPC_NUMBEROFCHANNEL:
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&oMOVProp.dwNumberOfChannels,ulSize);
			break;

		case MTP_OPC_AUDIOBITRATE:
			mov_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMOVProp);
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&oMOVProp.dwAudioBitRate,ulSize);
		break;

		default:
			SendResponseSimple(PTP_RS_ParameterNotSupported);
			return 1;
	}

	return 0;
}

INT32 xAddSingleWAVObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	WAVPROP			oWavProp;
	T_BF_FS_IF_STAT			file_stat;

	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID:
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);							
			break;		
			
		case MTP_OPC_OBJECTFORMAT:						
			usTempValue = MTP_OFC_WAV;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 						
			break;		
			
		case MTP_OPC_PROTECTIONSTATUS:		
			//usTempValue = PROTECT_NoProtect;
			// 2009/03/31 STX tony +
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			// 2009/03/31 STX tony -			
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 						
			break;		
			
		case MTP_OPC_OBJECTSIZE:	
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);							
			break;	
			
		case MTP_OPC_OBJECTFILENAME:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;	
			
		case MTP_OPC_PARENTOBJECT:		
			ulTempValue = nParentObject;		
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;		
			
		case MTP_OPC_PUOI:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);							
			break;	
			
		case MTP_OPC_NAME:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;				
		
		case MTP_OPC_DURATION: 	
			wav_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oWavProp);	
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&oWavProp.dwDuration,ulSize); 													
			break;	
			
		case MTP_OPC_SAMPLERATE: 
			wav_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oWavProp);	
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&oWavProp.dwSampleRate,ulSize); 								
			break;	
		
		case MTP_OPC_NUMBEROFCHANNEL: 	
			wav_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oWavProp);	
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&oWavProp.dwNumberOfChannels,ulSize);														
			break;						
				
		case MTP_OPC_AUDIOBITRATE: 
			wav_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oWavProp);	
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&oWavProp.dwAudioBitRate,ulSize); 																	
			break;
	
		default:
			SendResponseSimple(PTP_RS_ParameterNotSupported);
			return 1;							
	}

	return 0;
}

INT32 xAddSingleBMPObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	BMPPROP			oBmpProp;
	T_BF_FS_IF_STAT			file_stat;

	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID:
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);							
			break;	
			
		case MTP_OPC_OBJECTFORMAT:					
			usTempValue = MTP_OFC_BMP;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize);							
			break;	
			
		case MTP_OPC_PROTECTIONSTATUS:		
			//usTempValue = PROTECT_NoProtect;
			// 2009/03/31 STX tony +
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			// 2009/03/31 STX tony -			
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 						
			break;	
			
		case MTP_OPC_OBJECTSIZE:		
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);							
			break;	
			
		case MTP_OPC_OBJECTFILENAME:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;	
			
		case MTP_OPC_PARENTOBJECT:	
			ulTempValue = nParentObject;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;	
			
		case MTP_OPC_PUOI:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);							
			break;	
			
		case MTP_OPC_NAME:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;				

		case MTP_OPC_WIDTH:
			bmp_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oBmpProp);
			ulTempValue = oBmpProp.dwWidth;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);							
			break;	
			
		case MTP_OPC_HEIGHT:	
			bmp_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oBmpProp);
			ulTempValue = oBmpProp.dwHeight;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;		
								
		case MTP_OPC_IMAGEBITDEPTH:
			bmp_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oBmpProp);
			usTempValue = oBmpProp.wImageBitDepth;
			usTempValue = ENDIAN_SWAP16(usTempValue);
			ulTempValue = usTempValue;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_IMAGEBITDEPTH,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);																		
			break;
			
	}

	return 0;
}

INT32 xAddSingleGIFObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	GIFPROP			oGifProp;
	T_BF_FS_IF_STAT			file_stat;

	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID:
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);							
			break;	
			
		case MTP_OPC_OBJECTFORMAT:					
			usTempValue = MTP_OFC_GIF;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize);							
			break;		
			
		case MTP_OPC_PROTECTIONSTATUS:			
			//usTempValue = PROTECT_NoProtect;
			// 2009/03/31 STX tony +
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			// 2009/03/31 STX tony -			
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 						
			break;	
			
		case MTP_OPC_OBJECTSIZE:			
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);							
			break;	
			
		case MTP_OPC_OBJECTFILENAME:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;	
			
		case MTP_OPC_PARENTOBJECT:
			ulTempValue = nParentObject;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;	
			
		case MTP_OPC_PUOI:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);							
			break;
			
		case MTP_OPC_NAME:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;				

		case MTP_OPC_WIDTH:
			gif_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oGifProp);
			usTempValue = oGifProp.wWidth;
			usTempValue = ENDIAN_SWAP16(usTempValue);
			ulTempValue = usTempValue;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);							
			break;	
			
		case MTP_OPC_HEIGHT:	
			gif_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oGifProp);
			usTempValue = oGifProp.wHeight;
			usTempValue = ENDIAN_SWAP16(usTempValue);
			ulTempValue = usTempValue;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;		
								
		case MTP_OPC_IMAGEBITDEPTH:
			gif_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oGifProp);
			ulTempValue = oGifProp.cImageBitDepth;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_IMAGEBITDEPTH,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);																		
			break;			
	}

	return 0;
}

INT32 xAddSingleTIFFObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	TIFFPROP		oTiffProp;
	T_BF_FS_IF_STAT 		file_stat;

	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID:
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);							
			break;			
			
		case MTP_OPC_OBJECTFORMAT:						
			usTempValue = MTP_OFC_TIFF;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize);							
			break;		
			
		case MTP_OPC_PROTECTIONSTATUS:		
			//usTempValue = PROTECT_NoProtect;
			// 2009/03/31 STX tony +
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			// 2009/03/31 STX tony -				
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 						
			break;							
			
		case MTP_OPC_OBJECTSIZE:			
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);							
			break;			
			
		case MTP_OPC_OBJECTFILENAME:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;				
			
		case MTP_OPC_PARENTOBJECT:
			ulTempValue = nParentObject;				
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;			
			
		case MTP_OPC_PUOI:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);							
			break;		
			
		case MTP_OPC_NAME:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;				

		case MTP_OPC_WIDTH:
			tiff_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oTiffProp);
			ulTempValue = oTiffProp.dwWidth;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);							
			break;	
			
		case MTP_OPC_HEIGHT:	
			tiff_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oTiffProp);
			ulTempValue = oTiffProp.dwHeight;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;		
								
		case MTP_OPC_IMAGEBITDEPTH:
			tiff_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oTiffProp);
			ulTempValue = oTiffProp.wImageBitDepth;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_IMAGEBITDEPTH,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);																		
			break;			
	}

	return 0;
}

INT32 xAddSingleMP3ObjectPorpValueList(INT32 nObjectHandle,USHORT usObjectPropCode,INT32 nParentObject,ULONG dwObjectSize,FW_CHAR* cFileName,ULONG* ulSize)
{
	ULONG 			ulTempValue=0;
	USHORT 			usTempValue=0;
	MP3PROP			oMP3Prop;
	T_BF_FS_IF_STAT 		file_stat;

	switch (usObjectPropCode)
	{
		case MTP_OPC_STORAGEID:
//			if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//				ulTempValue = PTP_StorageID_SD;	
//			}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//				ulTempValue = PTP_StorageID_NF;	
//			}else{
//				ulTempValue = PTP_StorageID_MS;	
//			}
			ulTempValue = PtpStorageID;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);							
			break;			
			
		case MTP_OPC_OBJECTFORMAT:						
			usTempValue = MTP_OFC_MP3;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,ulSize);							
			break;			
			
		case MTP_OPC_PROTECTIONSTATUS: 			
			//usTempValue = PROTECT_NoProtect;
			// 2009/03/31 STX tony +
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[nObjectHandle].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	// 2009/03/30 tony +
			if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
			{
				usTempValue = PROTECT_ReadOnly;
			}
			else
			{
				usTempValue = PROTECT_NoProtect;
			}
			// 2009/03/31 STX tony -			
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,ulSize); 						
			break;							
			
		case MTP_OPC_OBJECTSIZE:				
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,ulSize);							
			break;			
			
		case MTP_OPC_OBJECTFILENAME:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;				
			
		case MTP_OPC_PARENTOBJECT:	
			ulTempValue = nParentObject;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);
			break;	
			
		case MTP_OPC_PUOI:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&nObjectHandle,ulSize);							
			break;	
			
		case MTP_OPC_NAME:
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,ulSize);
			break;				

		case MTP_OPC_DURATION: 	
			mp3_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMP3Prop);
			ulTempValue = oMP3Prop.nDuration;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&ulTempValue,ulSize); 								
			break;	
			
		case MTP_OPC_SAMPLERATE: 
			mp3_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMP3Prop);	
			ulTempValue = oMP3Prop.fMP3SampleRate;

			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&ulTempValue,ulSize);	
			break;	

		case MTP_OPC_NUMBEROFCHANNEL: 
			mp3_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMP3Prop);	
			usTempValue = oMP3Prop.nNumberOfChannels;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&usTempValue,ulSize);													
			break;						

		case MTP_OPC_AUDIOBITRATE: 
			mp3_parse(oMTPDBFileInfo[nObjectHandle].szDosFileName,&oMP3Prop);	
			ulTempValue = oMP3Prop.fMP3BitRate;
			xAddObjectPorpValueList(nObjectHandle,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&ulTempValue,ulSize); 												
			break;
	}

	return 0;
}

INT32 xAddOneHandleOneProperty(ULONG ulObjectHandle,ULONG ulObjectFormatCode,ULONG ulObjectPropCode,ULONG ulObjectPropGroupCode,ULONG ulDepth)
{
	UINT32			i;
//	ULONG   		ulTempValue;
	ULONG   		ulNumOfElements=0;
	WORD			wObjectFormatCode = 0;
	T_BF_FS_IF_STAT	file_stat;
	DWORD 			dwObjectSize;
	FW_CHAR 		cFileName[DOS_FILENAME_LEN*2];
	FW_CHAR 			szTempString[MTP_MAX_PATH];
	UCHAR			*Addr;
	ULONG  			ulSize = 0;	

	ulNumOfElements = 1;
	ulSize = 12+sizeof(UINT32); 	

	for (i=1;i<=g_nObjectCnt;i++)
	{
		if (oMTPDBFileInfo[i].nFileIndex == ulObjectHandle)
		{
			if (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE)
				wObjectFormatCode = MTP_OFC_ASSOCIATION;
			else if (xIsJpgFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_EXIFJPEG;
			else if (xIsBmpFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_BMP;
			else if (xIsGifFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_GIF;
			else if (xIsTiffFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_TIFF;
			else if (xIsAviFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_AVI;
			else if (xIsDpofFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_DPOF;
			else if (xIsMovFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MOV;
			else if (xIsMp4File(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MP4;
			else if (xIsWavFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_WAV;
			else if (xIsMp3File(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MP3;
		
			if ( ( (ulObjectFormatCode == MTP_OFC_ASSOCIATION) || (ulObjectFormatCode == 0x0000) )	&&
				(wObjectFormatCode == MTP_OFC_ASSOCIATION) && (oMTPDBFileInfo[i].bit.fDelete != 1) )								
			{	
//				memcpy(IN_Buffers+12,&ulTempValue,sizeof(ULONG));
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetPathName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 
				dwObjectSize = 0;	
				xAddSingleAssociationObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);									
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_MP3) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_MP3) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 								
				dwObjectSize = file_stat.fileSize;								
				xAddSingleMP3ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_BMP) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_BMP) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 							
				dwObjectSize = file_stat.fileSize;								
				xAddSingleBMPObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_GIF) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_GIF) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName);
				dwObjectSize = file_stat.fileSize;								
				xAddSingleGIFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_EXIFJPEG) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_EXIFJPEG) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 							
				dwObjectSize = file_stat.fileSize;								
				xAddSingleJPGObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_AVI) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_AVI) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 							
				dwObjectSize = file_stat.fileSize;								
				xAddSingleAVIObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_DPOF) || (ulObjectFormatCode == 0x0000) ) &&		// DPOF
				(wObjectFormatCode == MTP_OFC_DPOF) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddSingleDPOFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);		
			}	
			else if ( ( (ulObjectFormatCode == MTP_OFC_MOV) || (ulObjectFormatCode == 0x0000) ) &&		// MOV
				(wObjectFormatCode == MTP_OFC_MOV) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddSingleMOVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);		
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_MP4) || (ulObjectFormatCode == 0x0000) ) &&		// MP4
				(wObjectFormatCode == MTP_OFC_MP4) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName);
				dwObjectSize = file_stat.fileSize;
				xAddSingleMP4ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);		
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_WAV) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_WAV) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 							
				dwObjectSize = file_stat.fileSize;								
				xAddSingleWAVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_TIFF) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_TIFF) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 							
				dwObjectSize = file_stat.fileSize;								
				xAddSingleTIFFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
		}		
	}

	Addr = IN_Buffers+12;
	*Addr=(UCHAR)(ulNumOfElements&0x000000FF);
	*(Addr+1)=(UCHAR)((ulNumOfElements&0x0000FF00)>>8); 
	*(Addr+2)=(UCHAR)((ulNumOfElements&0x00FF0000)>>16);	
	*(Addr+3)=(UCHAR)((ulNumOfElements&0xFF000000)>>24);			
	SendData(ulSize-12);
	SendResponseSimple(PTP_RS_OK);
	return 0;
}

INT32 xAddOneHandleOnePropertyByDepth(ULONG ulObjectHandle,ULONG ulObjectFormatCode,ULONG ulObjectPropCode,ULONG ulObjectPropGroupCode,ULONG ulDepth)
{
	UINT32			i;
	ULONG   		ulNumOfElements=0;
	WORD			wObjectFormatCode = 0;
	T_BF_FS_IF_STAT	file_stat;
	DWORD 			dwObjectSize;
	FW_CHAR 		cFileName[DOS_FILENAME_LEN*2];
	FW_CHAR 		szTempString[MTP_MAX_PATH];
	UCHAR			*Addr;
	INT32 			nLevel=0;
	ULONG  			ulSize = 0;	

	ulNumOfElements = 1;
	ulSize = 12+sizeof(UINT32); 	

	for (i=1;i<=g_nObjectCnt;i++)
	{
		if ( (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
		{
			xGetObjectLevel(oMTPDBFileInfo[i].szDosFileName,TRUE,&nLevel);
		}
		else if ( (oMTPDBFileInfo[i].bit.file_attr == MTP_FILE_TYPE) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
		{
			xGetObjectLevel(oMTPDBFileInfo[i].szDosFileName,FALSE,&nLevel);
		}
	
		if (oMTPDBFileInfo[i].nFileIndex == ulObjectHandle)
		{
			if (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE)
				wObjectFormatCode = MTP_OFC_ASSOCIATION;
			else if (xIsJpgFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_EXIFJPEG;
			else if (xIsBmpFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_BMP;
			else if (xIsGifFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_GIF;
			else if (xIsTiffFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_TIFF;
			else if (xIsAviFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_AVI;
			else if (xIsDpofFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_DPOF;
			else if (xIsMovFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MOV;
			else if (xIsMp4File(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MP4;
			else if (xIsWavFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_WAV;
			else if (xIsMp3File(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MP3;
		
			if ( ( (ulObjectFormatCode == MTP_OFC_ASSOCIATION) || (ulObjectFormatCode == 0x0000) )	&&
				(wObjectFormatCode == MTP_OFC_ASSOCIATION) && (oMTPDBFileInfo[i].bit.fDelete != 1)
				&& (nLevel<=ulDepth))								
			{	
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetPathName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 
				dwObjectSize = 0;	
				xAddSingleAssociationObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);									
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_MP3) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_MP3) && (oMTPDBFileInfo[i].bit.fDelete != 1)
				&& (nLevel<=ulDepth))
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 								
				dwObjectSize = file_stat.fileSize;								
				xAddSingleMP3ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_BMP) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_BMP) && (oMTPDBFileInfo[i].bit.fDelete != 1)
				&& (nLevel<=ulDepth))
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 							
				dwObjectSize = file_stat.fileSize;								
				xAddSingleBMPObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_GIF) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_GIF) && (oMTPDBFileInfo[i].bit.fDelete != 1)
				&& (nLevel<=ulDepth))
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName);
				dwObjectSize = file_stat.fileSize;								
				xAddSingleGIFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_EXIFJPEG) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_EXIFJPEG) && (oMTPDBFileInfo[i].bit.fDelete != 1)
				&& (nLevel<=ulDepth))
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 							
				dwObjectSize = file_stat.fileSize;								
				xAddSingleJPGObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_AVI) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_AVI) && (oMTPDBFileInfo[i].bit.fDelete != 1)
				&& (nLevel<=ulDepth))
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 							
				dwObjectSize = file_stat.fileSize;								
				xAddSingleAVIObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_DPOF) || (ulObjectFormatCode == 0x0000) ) &&		// DPOF
				(wObjectFormatCode == MTP_OFC_DPOF) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				&& (nLevel<=ulDepth)) 				
			{
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddSingleDPOFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);		
			}	
			else if ( ( (ulObjectFormatCode == MTP_OFC_MOV) || (ulObjectFormatCode == 0x0000) ) &&		// MOV
				(wObjectFormatCode == MTP_OFC_MOV) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				&& (nLevel<=ulDepth)) 				
			{
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddSingleMOVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);		
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_MP4) || (ulObjectFormatCode == 0x0000) ) &&		// MP4
				(wObjectFormatCode == MTP_OFC_MP4) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				&& (nLevel<=ulDepth))
			{
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat);
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName);
				dwObjectSize = file_stat.fileSize;
				xAddSingleMP4ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_WAV) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_WAV) && (oMTPDBFileInfo[i].bit.fDelete != 1)
				&& (nLevel<=ulDepth))
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 							
				dwObjectSize = file_stat.fileSize;								
				xAddSingleWAVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_TIFF) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_TIFF) && (oMTPDBFileInfo[i].bit.fDelete != 1)
				&& (nLevel<=ulDepth))
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 							
				dwObjectSize = file_stat.fileSize;								
				xAddSingleTIFFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
		}		
	}

	Addr = IN_Buffers+12;
	*Addr=(UCHAR)(ulNumOfElements&0x000000FF);
	*(Addr+1)=(UCHAR)((ulNumOfElements&0x0000FF00)>>8); 
	*(Addr+2)=(UCHAR)((ulNumOfElements&0x00FF0000)>>16);	
	*(Addr+3)=(UCHAR)((ulNumOfElements&0xFF000000)>>24);			
	SendData(ulSize-12);
	SendResponseSimple(PTP_RS_OK);
	return 0;
}

INT32 xAddOneHandleAllProperty(ULONG ulObjectHandle,ULONG ulObjectFormatCode,ULONG ulObjectPropCode,ULONG ulObjectPropGroupCode,ULONG ulDepth)
{
	UINT32			i;
//	ULONG   		ulTempValue;
	ULONG   		ulNumOfElements=0;
	WORD			wObjectFormatCode = 0;
	T_BF_FS_IF_STAT	file_stat;
	DWORD 			dwObjectSize;
	FW_CHAR 		cFileName[DOS_FILENAME_LEN*2];
	FW_CHAR 		szTempString[MTP_MAX_PATH];
	UCHAR			*Addr;
	INT32 			nLevel=0;
	ULONG  			ulSize = 0;	

	ulSize = 12+sizeof(UINT32); 
	for (i=1;i<=g_nObjectCnt;i++)
	{	
		if (oMTPDBFileInfo[i].nFileIndex == ulObjectHandle)
		{		
			if (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE)
				wObjectFormatCode = MTP_OFC_ASSOCIATION;
			else if (xIsJpgFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_EXIFJPEG;
			else if (xIsBmpFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_BMP;
			else if (xIsGifFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_GIF;
			else if (xIsTiffFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_TIFF;
			else if (xIsAviFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_AVI;
			else if (xIsDpofFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_DPOF;
			else if (xIsMovFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MOV;
			else if (xIsMp4File(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MP4;
			else if (xIsWavFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_WAV;
			else if (xIsMp3File(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MP3;

			if ( ( (ulObjectFormatCode == MTP_OFC_ASSOCIATION) || (ulObjectFormatCode == 0x0000) )	&&
				(wObjectFormatCode == MTP_OFC_ASSOCIATION) &&(oMTPDBFileInfo[i].bit.fDelete != 1) 
				 )							
			{							
				ulNumOfElements += ASSOCIATION_SUPPPROP_NUM;
				
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetPathName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = 0;				
				xAddAllAssociationObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);
			}				
			else if ( ( (ulObjectFormatCode == MTP_OFC_MP3) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_MP3) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				 )				
			{

				ulNumOfElements += MP3_SUPPPROP_NUM;				 
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllMP3ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);		
			}		
			else if ( ( (ulObjectFormatCode == MTP_OFC_BMP) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_BMP) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				&& (nLevel<=ulDepth) )				
			{
				ulNumOfElements += BMP_SUPPPROP_NUM;				
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
//				ulTempValue = ENDIAN_SWAP32(ulTempValue);
				xAddAllBMPObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);		
			}					
			else if ( ( (ulObjectFormatCode == MTP_OFC_GIF) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_GIF) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				)				
			{
				ulNumOfElements += GIF_SUPPPROP_NUM;				
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllGIFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);		
			}		
			else if ( ( (ulObjectFormatCode == MTP_OFC_EXIFJPEG) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_EXIFJPEG) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				)				
			{
				ulNumOfElements += JPG_SUPPPROP_NUM;					
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllJPGObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);		
			}	
			else if ( ( (ulObjectFormatCode == MTP_OFC_AVI) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_AVI) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				)				
			{
				ulNumOfElements += AVI_SUPPPROP_NUM;			
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllAVIObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);		
			}	
			else if ( ( (ulObjectFormatCode == MTP_OFC_DPOF) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_DPOF) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
			{
				ulNumOfElements += DPOF_SUPPPROP_NUM;				
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllDPOFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
			}	
			else if ( ( (ulObjectFormatCode == MTP_OFC_MOV) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_MOV) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
			{
				ulNumOfElements += MOV_SUPPPROP_NUM;				
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllMOVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_MP4) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_MP4) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 
			{
				ulNumOfElements += MP4_SUPPPROP_NUM;				
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName);
				dwObjectSize = file_stat.fileSize;
				xAddAllMP4ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_WAV) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_WAV) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				 )				
			{
				ulNumOfElements += WAV_SUPPPROP_NUM;				 
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllWAVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);		
			}	
			else if ( ( (ulObjectFormatCode == MTP_OFC_TIFF) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_TIFF) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				)				
			{
				ulNumOfElements += TIFF_SUPPPROP_NUM;				
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllTIFFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);		
			}	
		}

	}

	Addr = IN_Buffers+12;
	*Addr=(UCHAR)(ulNumOfElements&0x000000FF);
	*(Addr+1)=(UCHAR)((ulNumOfElements&0x0000FF00)>>8); 
	*(Addr+2)=(UCHAR)((ulNumOfElements&0x00FF0000)>>16);	
	*(Addr+3)=(UCHAR)((ulNumOfElements&0xFF000000)>>24);

	SendData(ulSize-12);
	SendResponseSimple(PTP_RS_OK);
	return 0;
}

INT32 xAddOneHandleAllPropertyByGroup(ULONG ulObjectHandle,ULONG ulObjectFormatCode,ULONG ulObjectPropCode,ULONG ulObjectPropGroupCode,ULONG ulDepth)
{
	UINT32			i;
	ULONG   		ulTempValue;
	USHORT			usTempValue;
	ULONG   		ulNumOfElements=0;
	WORD			wObjectFormatCode = 0;
	T_BF_FS_IF_STAT	file_stat;
	DWORD 			dwObjectSize;
	FW_CHAR 		cFileName[DOS_FILENAME_LEN*2];
	FW_CHAR 		szTempString[MTP_MAX_PATH];
	INT32			nFolderParent=0;
	MP3PROP			oMP3Prop;
	BMPPROP			oBmpProp;
	GIFPROP			oGifProp = {0};
	JFIFPROP		oJFIFProp;
	AVIPROP			oAVIProp;
	MOVPROP			oMOVProp;
	TIFFPROP		oTiffProp;
	WAVPROP			oWavProp;
	UCHAR			*Addr;
	ULONG  			ulSize = 0;
	//char 			szDate[32];
	char 			szAscDate[16];
	char 			szUniDate[32];		
	UINT32			picX=0,picY=0,thumbX=0,thumbY=0,ThumbcompressSize=0;
	ULONG 			length = 0;

	memset(szAscDate, 0, sizeof(szAscDate));
	ulSize = 12+sizeof(UINT32); 	

	for (i=1;i<=g_nObjectCnt;i++)
	{			
		if (oMTPDBFileInfo[i].nFileIndex == ulObjectHandle)
		{
			if (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE)
				wObjectFormatCode = MTP_OFC_ASSOCIATION;
			else if (xIsJpgFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_EXIFJPEG;
			else if (xIsBmpFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_BMP;
			else if (xIsGifFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_GIF;
			else if (xIsTiffFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_TIFF;
			else if (xIsAviFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_AVI;
			else if (xIsDpofFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_DPOF;
			else if (xIsMovFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MOV;
			else if (xIsMp4File(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MP4;
			else if (xIsWavFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_WAV;
			else if (xIsMp3File(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MP3;
		
			if ( ( (ulObjectFormatCode == MTP_OFC_ASSOCIATION) || (ulObjectFormatCode == 0x0000) )	&&
				(wObjectFormatCode == MTP_OFC_ASSOCIATION) && (oMTPDBFileInfo[i].bit.fDelete != 1) )								
			{	
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetPathName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 
				dwObjectSize = 0;	
				if (ulObjectPropGroupCode == 1)
				{
					ulNumOfElements = MTP_ASSOCIATION_GROUP1;					
					xAddAllAssociationObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,nFolderParent,cFileName,&ulSize);
				}											
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_MP3) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_MP3) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 							
				dwObjectSize = file_stat.fileSize;
				
				if (ulObjectPropGroupCode == 0x00000001)
				{
					ulNumOfElements =  MTP_MP3_GROUP1;															
//					if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//						ulTempValue = PTP_StorageID_SD;	
//					}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//						ulTempValue = PTP_StorageID_NF;	
//					}else{
//						ulTempValue = PTP_StorageID_MS;	
//					}
					ulTempValue = PtpStorageID;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																	
					usTempValue = MTP_OFC_MP3; 
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 											
					//usTempValue = PROTECT_NoProtect;	
					// 2009/03/31 STX tony +
					if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
					{
						usTempValue = PROTECT_ReadOnly;
					}
					else
					{
						usTempValue = PROTECT_NoProtect;
					}
					// 2009/03/31 STX tony -					
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 															
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);												
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize); 							
					ulTempValue = oMTPDBFileInfo[i].nParentIndex;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 				
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);																	
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);							
				}
				else if (ulObjectPropGroupCode == 0x00000002)
				{
					ulNumOfElements = MTP_MP3_GROUP2;																															
					mp3_parse(oMTPDBFileInfo[i].szDosFileName,&oMP3Prop);
					ulTempValue = oMP3Prop.nDuration;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);		
					ulTempValue = oMP3Prop.fMP3SampleRate;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);				
					usTempValue = oMP3Prop.nNumberOfChannels;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&usTempValue,&ulSize);
					ulTempValue = oMP3Prop.fMP3BitRate;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 
		
				}
				else if (ulObjectPropGroupCode == 0x00000003)
				{
					ulNumOfElements = MTP_MP3_GROUP3;															
					
				}
				else if (ulObjectPropGroupCode	== 0x00000004)
				{
					ulNumOfElements = MTP_MP3_GROUP4;															
				}															
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_BMP) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_BMP) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 							 
				dwObjectSize = file_stat.fileSize;								

				if (ulObjectPropGroupCode == 0x00000001)
				{
					ulNumOfElements = MTP_BMP_GROUP1;													
//					if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//						ulTempValue = PTP_StorageID_SD;	
//					}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//						ulTempValue = PTP_StorageID_NF;	
//					}else{
//						ulTempValue = PTP_StorageID_MS;	
//					}	
					ulTempValue = PtpStorageID;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																	
					usTempValue = MTP_OFC_BMP; 
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 											
					//usTempValue = PROTECT_NoProtect;	
					// 2009/03/31 STX tony +
					if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
					{
						usTempValue = PROTECT_ReadOnly;
					}
					else
					{
						usTempValue = PROTECT_NoProtect;
					}
					// 2009/03/31 STX tony -					
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 															
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);												
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize); 							
					ulTempValue = oMTPDBFileInfo[i].nParentIndex;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 				
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);																	
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);							
				}
				else if (ulObjectPropGroupCode == 0x00000002)
				{						
					ulNumOfElements = MTP_BMP_GROUP2;							
					bmp_parse(oMTPDBFileInfo[i].szDosFileName,&oBmpProp);
					ulTempValue = oBmpProp.dwWidth;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);							
					ulTempValue = oBmpProp.dwHeight;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);														
					usTempValue = oBmpProp.wImageBitDepth;
					usTempValue = ENDIAN_SWAP16(usTempValue);
					ulTempValue = usTempValue;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_IMAGEBITDEPTH,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																					
				}
				else if (ulObjectPropGroupCode == 0x00000003)
				{																						
					ulNumOfElements = MTP_BMP_GROUP3;																
				}
				else if (ulObjectPropGroupCode	== 0x00000004)
				{
					ulNumOfElements = MTP_BMP_GROUP4;																
				}								
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_GIF) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_GIF) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName);
				dwObjectSize = file_stat.fileSize;								

				if (ulObjectPropGroupCode == 0x00000001)
				{							
						ulNumOfElements = MTP_GIF_GROUP1;							
//						if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//							ulTempValue = PTP_StorageID_SD;	
//						}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//							ulTempValue = PTP_StorageID_NF;	
//						}else{
//							ulTempValue = PTP_StorageID_MS;	
//						}
						ulTempValue = PtpStorageID;
						xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																	
						usTempValue = MTP_OFC_GIF; 
						xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 											
						//usTempValue = PROTECT_NoProtect;	
						// 2009/03/31 STX tony +
						if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
						{
							usTempValue = PROTECT_ReadOnly;
						}
						else
						{
							usTempValue = PROTECT_NoProtect;
						}
						// 2009/03/31 STX tony -						
						xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 															
						xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);												
						xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize); 							
						ulTempValue = oMTPDBFileInfo[i].nParentIndex;
						xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 				
						xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);																	
						xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);							
				}
				else if (ulObjectPropGroupCode == 0x00000002)
				{
					ulNumOfElements = MTP_GIF_GROUP2;
					gif_parse(oMTPDBFileInfo[i].szDosFileName,&oGifProp);
					usTempValue = oGifProp.wWidth;
					usTempValue = ENDIAN_SWAP16(usTempValue);
					ulTempValue = usTempValue;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);							
					usTempValue = oGifProp.wHeight;
					usTempValue = ENDIAN_SWAP16(usTempValue);
					ulTempValue = usTempValue;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);									
					ulTempValue = oGifProp.cImageBitDepth;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_IMAGEBITDEPTH,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);											
				}
				else if (ulObjectPropGroupCode == 0x00000003)
				{					
					ulTempValue = oGifProp.cImageBitDepth;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_IMAGEBITDEPTH,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);				
					ulNumOfElements = MTP_GIF_GROUP3;																
				}
				else if (ulObjectPropGroupCode	== 0x00000004)
				{
					ulNumOfElements = MTP_GIF_GROUP4;														
				}								
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_EXIFJPEG) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_EXIFJPEG) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 							
				dwObjectSize = file_stat.fileSize;								
				if (ulObjectPropGroupCode == 0x00000001)
				{								
					ulNumOfElements = MTP_JPG_GROUP1;							
//					if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//						ulTempValue = PTP_StorageID_SD;	
//					}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//						ulTempValue = PTP_StorageID_NF;	
//					}else{
//						ulTempValue = PTP_StorageID_MS;	
//					}
					ulTempValue = PtpStorageID;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																	
					usTempValue = MTP_OFC_EXIFJPEG; 
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 											
					//usTempValue = PROTECT_NoProtect;	
					// 2009/03/31 STX tony +
					if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
					{
						usTempValue = PROTECT_ReadOnly;
					}
					else
					{
						usTempValue = PROTECT_NoProtect;
					}
					// 2009/03/31 STX tony -					
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 															
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);							
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize);

					ulTempValue = oMTPDBFileInfo[i].nParentIndex;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 		
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);	
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);	

					PTP_fp = BF_Fs_If_Open(oMTPDBFileInfo[i].szDosFileName, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
					GetJpegPicInfo(&ThumbcompressSize, &thumbX, &thumbY, &picX, &picY, szAscDate);
					if(strlen(szAscDate) > 0)
					{
						ASCIIToUniCode(szAscDate,szUniDate);	
					}
					else
					{
						memset(szUniDate,0,MTP_DATETIME_LENGTH*2);
					}
//					xAddObjectPorpValueListForVarLenString(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_DATECREATED,MTP_DATATYPE_STR,szUniDate,&ulSize,MTP_DATETIME_LENGTH);	
					BF_Fs_If_Close(PTP_fp);
					PTP_fp=0;


					
					// 2009/03/31 STX tony +
//					BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);
//					snprintf(szAscDate, sizeof(szAscDate), "%04d%02d%02d%c%02d%02d%02d", ((file_stat.date >> 9) + 1980), (file_stat.date >> 5) & 0x0f, (file_stat.date) & 0x1f, 
//								   0x54, (file_stat.time >> 11) & 0x1f, (file_stat.time >> 5) & 0x3f, (file_stat.time & 0x1f) * 2);
//					
//					ASCIIToUniCode(szAscDate,szUniDate); 
//					xAddObjectPorpValueListForVarLenString(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_DATEMODIFIED,MTP_DATATYPE_STR,szUniDate,&ulSize,MTP_DATETIME_LENGTH);
					// 2009/03/31 STX tony -

	
					usTempValue = MTP_OFC_EXIFJPEG;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleFormat,MTP_DATATYPE_UINT16,&usTempValue,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleSize,MTP_DATATYPE_UINT32,&ThumbcompressSize,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleHeigth,MTP_DATATYPE_UINT32,&thumbY,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleWidth,MTP_DATATYPE_UINT32,&thumbX,&ulSize);

					xAddObjectPorpValueListNoValue(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleData,MTP_DATATYPE_AUINT8,&ulSize);
					
					ulSize = ulSize + sizeof(UINT32);
					if(GetJpegThumbnail(oMTPDBFileInfo[i].szDosFileName, (FW_ULONG)(IN_Buffers+ulSize), 65536 - 12, &length) != FJ_ERR_OK)
					{
						// Error 
						BF_Debug_Print_Error(("xAddOneHandleAllPropertyByGroup GetJpegThumbnail error\n"));
					}
					else
					{
						ulSize = ulSize + length;
					}
					// Write Thumbnail Size
					ulTempValue = length;
					Addr = IN_Buffers + ulSize - length - sizeof(UINT32);
					*Addr=(UCHAR)(ulTempValue&0x000000FF);
					*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
					*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);
					*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);

				}
				else if (ulObjectPropGroupCode ==  0x00000002)
				{
					ulNumOfElements = MTP_JPG_GROUP2;	
				
					jfif_parse(oMTPDBFileInfo[i].szDosFileName,&oJFIFProp);
				
					ulTempValue = oJFIFProp.wWidth;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);							
					ulTempValue = oJFIFProp.wHeight;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);
					ulTempValue = oJFIFProp.cImageBitDepth;
					ulTempValue = ulTempValue * 3;
					//ulTempValue = 0; // 2009/03/27 tony +
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_IMAGEBITDEPTH,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);							
				}
				else if (ulObjectPropGroupCode == 0x00000003)
				{
					ulNumOfElements = MTP_JPG_GROUP3;												
				}
				else if (ulObjectPropGroupCode == 0x00000004)
				{
					ulNumOfElements = MTP_JPG_GROUP4;												
				}								
			}

			else if ( ( (ulObjectFormatCode == MTP_OFC_AVI) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_AVI) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 							
				dwObjectSize = file_stat.fileSize;								
				
			
				if (ulObjectPropGroupCode == 0x00000001)
				{				
					ulNumOfElements = MTP_AVI_GROUP1;						
//					if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//						ulTempValue = PTP_StorageID_SD;	
//					}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//						ulTempValue = PTP_StorageID_NF;	
//					}else{
//						ulTempValue = PTP_StorageID_MS;	
//					}
					ulTempValue = PtpStorageID;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																	
					usTempValue = MTP_OFC_AVI; 
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 											
					//usTempValue = PROTECT_NoProtect;	
					// 2009/03/31 STX tony +
					if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
					{
						usTempValue = PROTECT_ReadOnly;
					}
					else
					{
						usTempValue = PROTECT_NoProtect;
					}
					// 2009/03/31 STX tony -					
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 															
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);													
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize); 

					// 2009/04/08 STX tony + (for avi modify date)
					BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);
					snprintf(szAscDate, sizeof(szAscDate), "%04d%02d%02d%c%02d%02d%02d", ((file_stat.date >> 9) + 1980), (file_stat.date >> 5) & 0x0f, (file_stat.date) & 0x1f, \
								   0x54, (file_stat.time >> 11) & 0x1f, (file_stat.time >> 5) & 0x3f, (file_stat.time & 0x1f) * 2);
					
					ASCIIToUniCode(szAscDate,szUniDate); 
					xAddObjectPorpValueListForVarLenString(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_DATEMODIFIED,MTP_DATATYPE_STR,szUniDate,&ulSize,16);
					// 2009/04/08 STX tony -


					
					ulTempValue = oMTPDBFileInfo[i].nParentIndex;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 		
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);	
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);					
				}
				else if (ulObjectPropGroupCode == 0x00000002)
				{
					ulNumOfElements = MTP_AVI_GROUP2;							
					avi_parse(oMTPDBFileInfo[i].szDosFileName,&oAVIProp);												
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&oAVIProp.dwWidth,&ulSize);										
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&oAVIProp.dwHeight,&ulSize); 									
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&oAVIProp.dwDuration,&ulSize); 										
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&oAVIProp.dwSampleRate,&ulSize); 				
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&oAVIProp.dwNumberOfChannels,&ulSize);			
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&oAVIProp.dwAudioBitRate,&ulSize); 												
				}
				else if (ulObjectPropGroupCode == 0x00000003)
				{
					ulNumOfElements = MTP_AVI_GROUP3;													
				}
				else if (ulObjectPropGroupCode	== 0x00000004)
				{
					ulNumOfElements = MTP_AVI_GROUP4;				
					avi_parse(oMTPDBFileInfo[i].szDosFileName,&oAVIProp);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_SCANTYPE,MTP_DATATYPE_UINT16,&oAVIProp.dwScanType,&ulSize); 				
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_VIDEOFOURCCCODEC,MTP_DATATYPE_UINT32,&oAVIProp.dwVideoFourCCCodec,&ulSize); 				
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_VIDEOBITRATE,MTP_DATATYPE_UINT32,&oAVIProp.dwVideoBitRate,&ulSize); 							
				}								
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_DPOF) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_DPOF) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 							
				dwObjectSize = file_stat.fileSize;								

				if (ulObjectPropGroupCode == 0x00000001)
				{				
					ulNumOfElements = MTP_DPOF_GROUP1;						
//					if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//						ulTempValue = PTP_StorageID_SD;	
//					}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//						ulTempValue = PTP_StorageID_NF;	
//					}else{
//						ulTempValue = PTP_StorageID_MS;	
//					}
					ulTempValue = PtpStorageID;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																	
					usTempValue = MTP_OFC_DPOF; 
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 											

					if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
					{
						usTempValue = PROTECT_ReadOnly;
					}
					else
					{
						usTempValue = PROTECT_NoProtect;
					}
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 															
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);													
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize); 

					ulTempValue = oMTPDBFileInfo[i].nParentIndex;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 		
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);	
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);					
				}
				else if (ulObjectPropGroupCode == 0x00000002)
				{
					ulNumOfElements = MTP_DPOF_GROUP2;
				}
				else if (ulObjectPropGroupCode == 0x00000003)
				{
					ulNumOfElements = MTP_DPOF_GROUP3;
				}
				else if (ulObjectPropGroupCode	== 0x00000004)
				{
					ulNumOfElements = MTP_DPOF_GROUP4;
				}
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_MOV) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_MOV) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 							
				dwObjectSize = file_stat.fileSize;								
				
			
				if (ulObjectPropGroupCode == 0x00000001)
				{				
					ulNumOfElements = MTP_MOV_GROUP1;						
//					if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//						ulTempValue = PTP_StorageID_SD;	
//					}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//						ulTempValue = PTP_StorageID_NF;	
//					}else{
//						ulTempValue = PTP_StorageID_MS;	
//					}
					ulTempValue = PtpStorageID;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																	
					usTempValue = MTP_OFC_MOV; 
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 											
					//usTempValue = PROTECT_NoProtect;	
					// 2009/03/31 STX tony +
					if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
					{
						usTempValue = PROTECT_ReadOnly;
					}
					else
					{
						usTempValue = PROTECT_NoProtect;
					}
					// 2009/03/31 STX tony -					
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 															
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);													
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize); 
					
					ulTempValue = oMTPDBFileInfo[i].nParentIndex;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 		
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);	
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);					

					// 0xDC81,0xDC82,0xDC83,0xDC84,0xDC86
					if(GetMovInfo(&ThumbcompressSize, &thumbX, &thumbY, &thumbX, &thumbY, oMTPDBFileInfo[i].szDosFileName) != FJ_ERR_OK)
					{
						BF_Debug_Print_Error(("xAddOneHandleAllPropertyByGroup GetMovInfo error\n"));
					}
					usTempValue = MTP_OFC_EXIFJPEG;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleFormat,MTP_DATATYPE_UINT16,&usTempValue,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleSize,MTP_DATATYPE_UINT32,&ThumbcompressSize,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleHeigth,MTP_DATATYPE_UINT32,&thumbY,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleWidth,MTP_DATATYPE_UINT32,&thumbX,&ulSize);

					xAddObjectPorpValueListNoValue(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleData,MTP_DATATYPE_AUINT8,&ulSize);
					
					if(GetMovThumbnailAndNumElements(oMTPDBFileInfo[i].szDosFileName, (FW_ULONG)(IN_Buffers+ulSize), 65536 - 12, &length) != FJ_ERR_OK)
					{
						// Error 
						BF_Debug_Print_Error(("xAddOneHandleAllPropertyByGroup GetMovThumbnail error\n"));
					}
					ulSize = ulSize + length;
					
				}
				else if (ulObjectPropGroupCode == 0x00000002)
				{
					ulNumOfElements = MTP_MOV_GROUP2;
					mov_parse(oMTPDBFileInfo[i].szDosFileName,&oMOVProp);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&oMOVProp.dwWidth,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&oMOVProp.dwHeight,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&oMOVProp.dwDuration,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&oMOVProp.dwSampleRate,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&oMOVProp.dwNumberOfChannels,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&oMOVProp.dwAudioBitRate,&ulSize);
				}
				else if (ulObjectPropGroupCode == 0x00000003)
				{
					ulNumOfElements = MTP_MOV_GROUP3;													
				}
				else if (ulObjectPropGroupCode	== 0x00000004)
				{
					ulNumOfElements = MTP_MOV_GROUP4;				
				}								
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_MP4) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_MP4) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName);
				dwObjectSize = file_stat.fileSize;

				if (ulObjectPropGroupCode == 0x00000001)
				{
					ulNumOfElements = MTP_MP4_GROUP1;
//					if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1) {
//						ulTempValue = PTP_StorageID_SD;	
//					}
//					else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF) {
//						ulTempValue = PTP_StorageID_NF;	
//					}
//					else {
//						ulTempValue = PTP_StorageID_MS;	
//					}
					ulTempValue = PtpStorageID;

					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);
					usTempValue = MTP_OFC_MP4;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize);
					//usTempValue = PROTECT_NoProtect;
					// 2009/03/31 STX tony +
					if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
					{
						usTempValue = PROTECT_ReadOnly;
					}
					else
					{
						usTempValue = PROTECT_NoProtect;
					}
					// 2009/03/31 STX tony -
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize);
					
					ulTempValue = oMTPDBFileInfo[i].nParentIndex;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);

					// 0xDC81,0xDC82,0xDC83,0xDC84,0xDC86
					if(GetMovInfo(&ThumbcompressSize, &thumbX, &thumbY, &thumbX, &thumbY, oMTPDBFileInfo[i].szDosFileName) != FJ_ERR_OK)
					{
						BF_Debug_Print_Error(("xAddOneHandleAllPropertyByGroup GetMovInfo error[MP4]\n"));
					}
					usTempValue = MTP_OFC_EXIFJPEG;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleFormat,MTP_DATATYPE_UINT16,&usTempValue,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleSize,MTP_DATATYPE_UINT32,&ThumbcompressSize,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleHeigth,MTP_DATATYPE_UINT32,&thumbY,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleWidth,MTP_DATATYPE_UINT32,&thumbX,&ulSize);

					xAddObjectPorpValueListNoValue(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleData,MTP_DATATYPE_AUINT8,&ulSize);
					
					if(GetMovThumbnailAndNumElements(oMTPDBFileInfo[i].szDosFileName, (FW_ULONG)(IN_Buffers+ulSize), 65536 - 12, &length) != FJ_ERR_OK)
					{
						// Error 
						BF_Debug_Print_Error(("xAddOneHandleAllPropertyByGroup GetMovThumbnail error[MP4]\n"));
					}
					ulSize = ulSize + length;
				}
				else if (ulObjectPropGroupCode == 0x00000002)
				{
					ulNumOfElements = MTP_MP4_GROUP2;
					mov_parse(oMTPDBFileInfo[i].szDosFileName,&oMOVProp);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&oMOVProp.dwWidth,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&oMOVProp.dwHeight,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&oMOVProp.dwDuration,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&oMOVProp.dwSampleRate,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&oMOVProp.dwNumberOfChannels,&ulSize);
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&oMOVProp.dwAudioBitRate,&ulSize);
				}
				else if (ulObjectPropGroupCode == 0x00000003)
				{
					ulNumOfElements = MTP_MP4_GROUP3;
				}
				else if (ulObjectPropGroupCode	== 0x00000004)
				{
					ulNumOfElements = MTP_MP4_GROUP4;
				}
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_WAV) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_WAV) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 							
				dwObjectSize = file_stat.fileSize;								

				if (ulObjectPropGroupCode == 0x00000001)
				{
					ulNumOfElements = MTP_WAV_GROUP1;					
//					if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//						ulTempValue = PTP_StorageID_SD;	
//					}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//						ulTempValue = PTP_StorageID_NF;	
//					}else{
//						ulTempValue = PTP_StorageID_MS;	
//					}
					ulTempValue = PtpStorageID;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																	
					usTempValue = MTP_OFC_WAV; 
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 											
					//usTempValue = PROTECT_NoProtect;	
					// 2009/03/31 STX tony +
					if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
					{
						usTempValue = PROTECT_ReadOnly;
					}
					else
					{
						usTempValue = PROTECT_NoProtect;
					}
					// 2009/03/31 STX tony -					
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 															
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);												
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize); 							
					ulTempValue = oMTPDBFileInfo[i].nParentIndex;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 				
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);																	
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);							
				}				
				else if (ulObjectPropGroupCode == 0x00000002)
				{
					ulNumOfElements = MTP_WAV_GROUP2;									
				
					wav_parse(oMTPDBFileInfo[i].szDosFileName,&oWavProp); 
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&oWavProp.dwDuration,&ulSize); 				
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&oWavProp.dwSampleRate,&ulSize); 				
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&oWavProp.dwNumberOfChannels,&ulSize);						
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&oWavProp.dwAudioBitRate,&ulSize); 								
				}
				else if (ulObjectPropGroupCode == 0x00000003)
				{
					ulNumOfElements = MTP_WAV_GROUP3;									
				
				}
				else if (ulObjectPropGroupCode	== 0x00000004)
				{
					ulNumOfElements = MTP_WAV_GROUP4;									
				
				}								
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_TIFF) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_TIFF) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{							
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 							
				dwObjectSize = file_stat.fileSize;								

				if (ulObjectPropGroupCode == 0x00000001)
				{	
					ulNumOfElements = MTP_TIFF_GROUP1;
//					if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//						ulTempValue = PTP_StorageID_SD;	
//					}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//						ulTempValue = PTP_StorageID_NF;	
//					}else{
//						ulTempValue = PTP_StorageID_MS;	
//					}
					ulTempValue = PtpStorageID;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																	
					usTempValue = MTP_OFC_TIFF; 
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 											
					//usTempValue = PROTECT_NoProtect;	
					// 2009/03/31 STX tony +
					if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
					{
						usTempValue = PROTECT_ReadOnly;
					}
					else
					{
						usTempValue = PROTECT_NoProtect;
					}
					// 2009/03/31 STX tony -					
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 															
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);												
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize); 							
					ulTempValue = oMTPDBFileInfo[i].nParentIndex;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 				
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);																	
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);										
				}
				else if (ulObjectPropGroupCode == 0x00000002)
				{
					ulNumOfElements = MTP_TIFF_GROUP2;	
					
					tiff_parse(oMTPDBFileInfo[i].szDosFileName,&oTiffProp);
					ulTempValue = oTiffProp.dwWidth;
				
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);							
					ulTempValue = oTiffProp.dwHeight;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);															
					ulTempValue = oTiffProp.wImageBitDepth;
					xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_IMAGEBITDEPTH,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																
				}
				else if (ulObjectPropGroupCode == 0x00000003)
				{
					ulNumOfElements = MTP_TIFF_GROUP3;									
					
				}
				else if (ulObjectPropGroupCode	== 0x00000004)
				{
					ulNumOfElements = MTP_TIFF_GROUP4;													
				}
				
			}

		}				
	}

	Addr = IN_Buffers+12;
	*Addr=(UCHAR)(ulNumOfElements&0x000000FF);
	*(Addr+1)=(UCHAR)((ulNumOfElements&0x0000FF00)>>8); 
	*(Addr+2)=(UCHAR)((ulNumOfElements&0x00FF0000)>>16);	
	*(Addr+3)=(UCHAR)((ulNumOfElements&0xFF000000)>>24);
	
	SendData(ulSize-12);
	SendResponseSimple(PTP_RS_OK);
	return 0;
}

INT32 xAddOneHandleAllPropertyByDepth(ULONG ulObjectHandle,ULONG ulObjectFormatCode,ULONG ulObjectPropCode,ULONG ulObjectPropGroupCode,ULONG ulDepth)
{
	UINT32			i;
	ULONG   		ulNumOfElements=0;
	WORD			wObjectFormatCode = 0;
	T_BF_FS_IF_STAT	file_stat;
	DWORD 			dwObjectSize;
	FW_CHAR 		cFileName[DOS_FILENAME_LEN*2];
	FW_CHAR 		szTempString[MTP_MAX_PATH];
	UCHAR			*Addr;
	INT32 			nLevel=0;
	ULONG  			ulSize = 0;	

	ulSize = 12 + sizeof(ULONG);
	for (i=1;i<=g_nObjectCnt;i++)
	{

		if (oMTPDBFileInfo[i].nFileIndex == ulObjectHandle)
		{
			if ( (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{
				xGetObjectLevel(oMTPDBFileInfo[i].szDosFileName,TRUE,&nLevel);
			}
			else if ( (oMTPDBFileInfo[i].bit.file_attr == MTP_FILE_TYPE) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{
				xGetObjectLevel(oMTPDBFileInfo[i].szDosFileName,FALSE,&nLevel);
			}

			if (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE)
				wObjectFormatCode = MTP_OFC_ASSOCIATION;
			else if (xIsJpgFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_EXIFJPEG;
			else if (xIsBmpFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_BMP;
			else if (xIsGifFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_GIF;
			else if (xIsTiffFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_TIFF;
			else if (xIsAviFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_AVI;
			else if (xIsDpofFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_DPOF;
			else if (xIsMovFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MOV;
			else if (xIsMp4File(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MP4;
			else if (xIsWavFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_WAV;
			else if (xIsMp3File(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MP3;

			if ( ( (ulObjectFormatCode == MTP_OFC_ASSOCIATION) || (ulObjectFormatCode == 0x0000) )	&&
				(wObjectFormatCode == MTP_OFC_ASSOCIATION) &&(oMTPDBFileInfo[i].bit.fDelete != 1) 
				&& (nLevel<=ulDepth) ) 							
			{							
				ulNumOfElements += ASSOCIATION_SUPPPROP_NUM;
				
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetPathName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = 0;	 			
				xAddAllAssociationObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);
			}					
			else if ( ( (ulObjectFormatCode == MTP_OFC_MP3) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_MP3) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				&& (nLevel<=ulDepth) ) 				
			{

				ulNumOfElements += MP3_SUPPPROP_NUM;				 
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllMP3ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);		
			}		
			else if ( ( (ulObjectFormatCode == MTP_OFC_BMP) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_BMP) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				&& (nLevel<=ulDepth) ) 				
			{
				ulNumOfElements += BMP_SUPPPROP_NUM;					
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllBMPObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);		
			}						
			else if ( ( (ulObjectFormatCode == MTP_OFC_GIF) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_GIF) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				&& (nLevel<=ulDepth)) 				
			{
				ulNumOfElements += GIF_SUPPPROP_NUM;					
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllGIFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);		
			}		
			else if ( ( (ulObjectFormatCode == MTP_OFC_EXIFJPEG) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_EXIFJPEG) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				&& (nLevel<=ulDepth))				
			{
				ulNumOfElements += JPG_SUPPPROP_NUM;					
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllJPGObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);		
			}	
			else if ( ( (ulObjectFormatCode == MTP_OFC_AVI) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_AVI) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				&& (nLevel<=ulDepth)) 				
			{
				ulNumOfElements += AVI_SUPPPROP_NUM;					
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllAVIObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);		
			}	
			else if ( ( (ulObjectFormatCode == MTP_OFC_DPOF) || (ulObjectFormatCode == 0x0000) ) &&		// DPOF
				(wObjectFormatCode == MTP_OFC_DPOF) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				&& (nLevel<=ulDepth)) 				
			{
				ulNumOfElements += DPOF_SUPPPROP_NUM;					
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllDPOFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);		
			}	
			else if ( ( (ulObjectFormatCode == MTP_OFC_MOV) || (ulObjectFormatCode == 0x0000) ) &&		// MOV
				(wObjectFormatCode == MTP_OFC_MOV) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				&& (nLevel<=ulDepth)) 				
			{
				ulNumOfElements += MOV_SUPPPROP_NUM;					
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllMOVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);		
			}	
			else if ( ( (ulObjectFormatCode == MTP_OFC_MP4) || (ulObjectFormatCode == 0x0000) ) &&		// MP4
				(wObjectFormatCode == MTP_OFC_MP4) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				&& (nLevel<=ulDepth)) 
			{
				ulNumOfElements += MP4_SUPPPROP_NUM;
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 
				dwObjectSize = file_stat.fileSize;
				xAddAllMP4ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);
			}	
			else if ( ( (ulObjectFormatCode == MTP_OFC_WAV) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_WAV) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				&& (nLevel<=ulDepth) ) 				
			{
				ulNumOfElements += WAV_SUPPPROP_NUM;				 
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllWAVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);		
			}	
			else if ( ( (ulObjectFormatCode == MTP_OFC_TIFF) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_TIFF) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
				&& (nLevel<=ulDepth))				
			{
				ulNumOfElements += TIFF_SUPPPROP_NUM;					
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllTIFFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);		
			}						
		}
	}

	Addr = IN_Buffers+12;
	*Addr=(UCHAR)(ulNumOfElements&0x000000FF);
	*(Addr+1)=(UCHAR)((ulNumOfElements&0x0000FF00)>>8); 
	*(Addr+2)=(UCHAR)((ulNumOfElements&0x00FF0000)>>16);	
	*(Addr+3)=(UCHAR)((ulNumOfElements&0xFF000000)>>24);

	SendData(ulSize-12);
	SendResponseSimple(PTP_RS_OK);
	return 0;
}

INT32 xAddAllHandleOneProperty(ULONG ulObjectHandle,ULONG ulObjectFormatCode,ULONG ulObjectPropCode,ULONG ulObjectPropGroupCode,ULONG ulDepth)
{
	UINT32			i;
	ULONG   		ulTempValue=0;
	ULONG   		ulNumOfElements=0;
	WORD			wObjectFormatCode = 0;
	T_BF_FS_IF_STAT	file_stat;
	DWORD 			dwObjectSize;
	FW_CHAR 		cFileName[DOS_FILENAME_LEN*2];
	FW_CHAR 		szTempString[MTP_MAX_PATH];
	UCHAR			*Addr;
	ULONG  			ulSize = 0;	

	ulSize=12+sizeof(UINT32);	
	for (i=1;i<=g_nObjectCnt;i++)
	{
		if (oMTPDBFileInfo[i].bit.fDelete == 1) 
			break;

		if (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE)
			wObjectFormatCode = MTP_OFC_ASSOCIATION;
		else if (xIsJpgFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_EXIFJPEG;
		else if (xIsBmpFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_BMP;
		else if (xIsGifFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_GIF;
		else if (xIsTiffFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_TIFF;
		else if (xIsAviFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_AVI;
		else if (xIsDpofFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_DPOF;
		else if (xIsMovFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_MOV;
		else if (xIsMp4File(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_MP4;
		else if (xIsWavFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_WAV;
		else if (xIsMp3File(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_MP3;
		
		if ( ( (ulObjectFormatCode == MTP_OFC_ASSOCIATION) || (ulObjectFormatCode == 0x0000) )	&&
			(wObjectFormatCode == MTP_OFC_ASSOCIATION) && (oMTPDBFileInfo[i].bit.fDelete != 1) )								
		{	
			ulNumOfElements++;			
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetPathName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = 0;	
			ulTempValue =  ASSOCIATION_SUPPPROP_NUM;
			ulTempValue = ENDIAN_SWAP32(ulTempValue);
			xAddSingleAssociationObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
		}
		else if ( ( (ulObjectFormatCode == MTP_OFC_MP3) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_MP3) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;			
			xAddSingleMP3ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
		}
		else if ( ( (ulObjectFormatCode == MTP_OFC_BMP) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_BMP) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;			
			xAddSingleBMPObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
		}
		else if ( ( (ulObjectFormatCode == MTP_OFC_GIF) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_GIF) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 										
			dwObjectSize = file_stat.fileSize;	

			xAddSingleGIFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
		}		
		else if ( ( (ulObjectFormatCode == MTP_OFC_EXIFJPEG) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_EXIFJPEG) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 										
			dwObjectSize = file_stat.fileSize;	

			xAddSingleJPGObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
		}
		else if ( ( (ulObjectFormatCode == MTP_OFC_AVI) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_AVI) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 										
			dwObjectSize = file_stat.fileSize;	

			xAddSingleAVIObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
		}
		else if ( ( (ulObjectFormatCode == MTP_OFC_DPOF) || (ulObjectFormatCode == 0x0000) ) &&		// DPOF
			(wObjectFormatCode == MTP_OFC_DPOF) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddSingleDPOFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);		
		}	
		else if ( ( (ulObjectFormatCode == MTP_OFC_MOV) || (ulObjectFormatCode == 0x0000) ) &&		// MOV
			(wObjectFormatCode == MTP_OFC_MOV) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddSingleMOVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);		
		}
		else if ( ( (ulObjectFormatCode == MTP_OFC_MP4) || (ulObjectFormatCode == 0x0000) ) &&		// MP4
			(wObjectFormatCode == MTP_OFC_MP4) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddSingleMP4ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);		
		}
		else if ( ( (ulObjectFormatCode == MTP_OFC_WAV) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_WAV) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 										
			dwObjectSize = file_stat.fileSize;	

			xAddSingleWAVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
		}
		else if ( ( (ulObjectFormatCode == MTP_OFC_TIFF) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_TIFF) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 										
			dwObjectSize = file_stat.fileSize;			
			xAddSingleTIFFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
		}			
	}

	if (ulNumOfElements==0)
	{
		memset(IN_Buffers+12,0,sizeof(ULONG)); 
		SendData(4);
		SendResponseSimple(PTP_RS_OK);
		return 0;
	}
	else
	{			
		Addr = IN_Buffers+12;
		*Addr=(UCHAR)(ulNumOfElements&0x000000FF);
		*(Addr+1)=(UCHAR)((ulNumOfElements&0x0000FF00)>>8); 
		*(Addr+2)=(UCHAR)((ulNumOfElements&0x00FF0000)>>16);	
		*(Addr+3)=(UCHAR)((ulNumOfElements&0xFF000000)>>24);			
		SendData(ulSize-12);
		SendResponseSimple(PTP_RS_OK);	
		return 0;
	}
}

INT32 xAddAllHandleOnePropertyByFolder(ULONG ulObjectHandle,ULONG ulObjectFormatCode,ULONG ulObjectPropCode,ULONG ulObjectPropGroupCode,ULONG ulDepth)
{
	UINT32			i;
	ULONG   		ulTempValue=0;
	ULONG   		ulNumOfElements=0;
	WORD			wObjectFormatCode = 0;
	T_BF_FS_IF_STAT	file_stat;
	DWORD 			dwObjectSize;
	FW_CHAR 		cFileName[DOS_FILENAME_LEN*2];
	FW_CHAR 		szTempString[MTP_MAX_PATH];
	FW_UCHAR		*Addr;
	ULONG  			ulSize = 0;	
	BOOL 			bIsFind=FALSE;

	ulSize=12+sizeof(UINT32);	
	for (i=1;i<=g_nObjectCnt;i++)
	{
		if (oMTPDBFileInfo[i].bit.fDelete == 1) 
			break;

		xCheckParentObject(i,ulObjectHandle,&bIsFind);

		if (bIsFind == TRUE)
		{
			bIsFind = FALSE;		
		
			if (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE)
				wObjectFormatCode = MTP_OFC_ASSOCIATION;
			else if (xIsJpgFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_EXIFJPEG;
			else if (xIsBmpFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_BMP;
			else if (xIsGifFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_GIF;
			else if (xIsTiffFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_TIFF;
			else if (xIsAviFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_AVI;
			else if (xIsDpofFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_DPOF;
			else if (xIsMovFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MOV;
			else if (xIsMp4File(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MP4;
			else if (xIsWavFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_WAV;
			else if (xIsMp3File(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MP3;
			
			if ( ( (ulObjectFormatCode == MTP_OFC_ASSOCIATION) || (ulObjectFormatCode == 0x0000) )	&&
				(wObjectFormatCode == MTP_OFC_ASSOCIATION) && (oMTPDBFileInfo[i].bit.fDelete != 1) )								
			{	
				ulNumOfElements++;		
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetPathName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = 0;	
				ulTempValue =  ASSOCIATION_SUPPPROP_NUM;
				ulTempValue = ENDIAN_SWAP32(ulTempValue);
				xAddSingleAssociationObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_MP3) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_MP3) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{
				ulNumOfElements++;
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;			
				xAddSingleMP3ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_BMP) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_BMP) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{
				ulNumOfElements++;
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;			
				xAddSingleBMPObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_GIF) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_GIF) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{
				ulNumOfElements++;
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 										
				dwObjectSize = file_stat.fileSize;	

				xAddSingleGIFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}		
			else if ( ( (ulObjectFormatCode == MTP_OFC_EXIFJPEG) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_EXIFJPEG) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{
				ulNumOfElements++;
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 										
				dwObjectSize = file_stat.fileSize;	

				xAddSingleJPGObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_AVI) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_AVI) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{
				ulNumOfElements++;
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 										
				dwObjectSize = file_stat.fileSize;	

				xAddSingleAVIObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_DPOF) || (ulObjectFormatCode == 0x0000) ) &&		// DPOF
				(wObjectFormatCode == MTP_OFC_DPOF) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{
				ulNumOfElements++;
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddSingleDPOFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);		
			}	
			else if ( ( (ulObjectFormatCode == MTP_OFC_MOV) || (ulObjectFormatCode == 0x0000) ) &&		// MOV
				(wObjectFormatCode == MTP_OFC_MOV) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{
				ulNumOfElements++;
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddSingleMOVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);		
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_MP4) || (ulObjectFormatCode == 0x0000) ) &&		// MP4
				(wObjectFormatCode == MTP_OFC_MP4) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{
				ulNumOfElements++;
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat);
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName);
				dwObjectSize = file_stat.fileSize;
				xAddSingleMP4ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_WAV) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_WAV) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{
				ulNumOfElements++;
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 										
				dwObjectSize = file_stat.fileSize;	

				xAddSingleWAVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}
			else if ( ( (ulObjectFormatCode == MTP_OFC_TIFF) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_TIFF) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
			{
				ulNumOfElements++;	
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 										
				dwObjectSize = file_stat.fileSize;			
				xAddSingleTIFFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
			}			
		}
	}

	if (ulNumOfElements==0)
	{
		memset(IN_Buffers+12,0,sizeof(ULONG)); 
		SendData(4);
		SendResponseSimple(PTP_RS_OK);
		return 0;
	}
	else
	{			
		Addr = IN_Buffers+12;
		*Addr=(UCHAR)(ulNumOfElements&0x000000FF);
		*(Addr+1)=(UCHAR)((ulNumOfElements&0x0000FF00)>>8); 
		*(Addr+2)=(UCHAR)((ulNumOfElements&0x00FF0000)>>16);	
		*(Addr+3)=(UCHAR)((ulNumOfElements&0xFF000000)>>24);			
		SendData(ulSize-12);
		SendResponseSimple(PTP_RS_OK);	
		return 0;
	}

}

INT32 xAddAllHandleOnePropertyByDepth(ULONG ulObjectHandle,ULONG ulObjectFormatCode,ULONG ulObjectPropCode,ULONG ulObjectPropGroupCode,ULONG ulDepth)
{
	UINT32			i;
	ULONG   		ulTempValue=0;
	ULONG   		ulNumOfElements=0;
	WORD			wObjectFormatCode = 0;
	T_BF_FS_IF_STAT	file_stat;
	DWORD 			dwObjectSize;
	FW_CHAR 		cFileName[DOS_FILENAME_LEN*2];
	FW_CHAR 		szTempString[MTP_MAX_PATH];
	UCHAR			*Addr;
	INT32 			nLevel=0;
	ULONG  			ulSize = 0;	

	ulSize=12+sizeof(UINT32);	
	for (i=1;i<=g_nObjectCnt;i++)
	{
		if (oMTPDBFileInfo[i].bit.fDelete == 1) 
			break;

		if ( (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
		{
			xGetObjectLevel(oMTPDBFileInfo[i].szDosFileName,TRUE,&nLevel);
		}
		else if ( (oMTPDBFileInfo[i].bit.file_attr == MTP_FILE_TYPE) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
		{
			xGetObjectLevel(oMTPDBFileInfo[i].szDosFileName,FALSE,&nLevel);
		}

		if (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE)
			wObjectFormatCode = MTP_OFC_ASSOCIATION;
		else if (xIsJpgFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_EXIFJPEG;
		else if (xIsBmpFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_BMP;
		else if (xIsGifFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_GIF;
		else if (xIsTiffFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_TIFF;
		else if (xIsAviFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_AVI;
		else if (xIsDpofFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_DPOF;
		else if (xIsMovFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_MOV;
		else if (xIsMp4File(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_MP4;
		else if (xIsWavFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_WAV;
		else if (xIsMp3File(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_MP3;
		
		if ( ( (ulObjectFormatCode == MTP_OFC_ASSOCIATION) || (ulObjectFormatCode == 0x0000) )	&&
			(wObjectFormatCode == MTP_OFC_ASSOCIATION) && (oMTPDBFileInfo[i].bit.fDelete != 1)
			&& (nLevel<=ulDepth) )								
		{	
			ulNumOfElements++;			
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetPathName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = 0;	
			ulTempValue =  ASSOCIATION_SUPPPROP_NUM;
			ulTempValue = ENDIAN_SWAP32(ulTempValue);
			xAddSingleAssociationObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
		}
		else if ( ( (ulObjectFormatCode == MTP_OFC_MP3) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_MP3) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
			&& (nLevel<=ulDepth) )
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;			
			xAddSingleMP3ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
		}
		else if ( ( (ulObjectFormatCode == MTP_OFC_BMP) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_BMP) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
			&& (nLevel<=ulDepth))
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;			
			xAddSingleBMPObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
		}
		else if ( ( (ulObjectFormatCode == MTP_OFC_GIF) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_GIF) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
			&& (nLevel<=ulDepth) )
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 										
			dwObjectSize = file_stat.fileSize;	

			xAddSingleGIFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
		}		
		else if ( ( (ulObjectFormatCode == MTP_OFC_EXIFJPEG) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_EXIFJPEG) && (oMTPDBFileInfo[i].bit.fDelete != 1)
			&& (nLevel<=ulDepth) )
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 										
			dwObjectSize = file_stat.fileSize;	

			xAddSingleJPGObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
		}
		else if ( ( (ulObjectFormatCode == MTP_OFC_AVI) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_AVI) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
			&& (nLevel<=ulDepth) )
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 										
			dwObjectSize = file_stat.fileSize;	

			xAddSingleAVIObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
		}
		else if ( ( (ulObjectFormatCode == MTP_OFC_DPOF) || (ulObjectFormatCode == 0x0000) ) &&		// DPOF
			(wObjectFormatCode == MTP_OFC_DPOF) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
			&& (nLevel<=ulDepth)) 				
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddSingleDPOFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);		
		}	
		else if ( ( (ulObjectFormatCode == MTP_OFC_MOV) || (ulObjectFormatCode == 0x0000) ) &&		// MOV
			(wObjectFormatCode == MTP_OFC_MOV) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
			&& (nLevel<=ulDepth)) 				
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddSingleMOVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);		
		}	
		else if ( ( (ulObjectFormatCode == MTP_OFC_MP4) || (ulObjectFormatCode == 0x0000) ) &&		// MP4
			(wObjectFormatCode == MTP_OFC_MP4) && (oMTPDBFileInfo[i].bit.fDelete != 1)
			&& (nLevel<=ulDepth))
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddSingleMP4ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
		}	
		else if ( ( (ulObjectFormatCode == MTP_OFC_WAV) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_WAV) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
			&& (nLevel<=ulDepth) )
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 										
			dwObjectSize = file_stat.fileSize;	

			xAddSingleWAVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
		}
		else if ( ( (ulObjectFormatCode == MTP_OFC_TIFF) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_TIFF) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
			&& (nLevel<=ulDepth) )
		{
			ulNumOfElements++;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 										
			dwObjectSize = file_stat.fileSize;			
			xAddSingleTIFFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,ulObjectPropCode,oMTPDBFileInfo[i].nParentIndex,dwObjectSize,cFileName,&ulSize);
		}			
	}

	if (ulNumOfElements==0)
	{
		memset(IN_Buffers+12,0,sizeof(ULONG)); 
		SendData(4);
		SendResponseSimple(PTP_RS_OK);
		return 0;
	}
	else
	{			
		Addr = IN_Buffers+12;
		*Addr=(UCHAR)(ulNumOfElements&0x000000FF);
		*(Addr+1)=(UCHAR)((ulNumOfElements&0x0000FF00)>>8); 
		*(Addr+2)=(UCHAR)((ulNumOfElements&0x00FF0000)>>16);	
		*(Addr+3)=(UCHAR)((ulNumOfElements&0xFF000000)>>24);			
		SendData(ulSize-12);
		SendResponseSimple(PTP_RS_OK);	
		return 0;
	}
}

INT32 xAddAllHandleAllProperty(ULONG ulObjectHandle,ULONG ulObjectFormatCode,ULONG ulObjectPropCode,ULONG ulObjectPropGroupCode,ULONG ulDepth)
{
	UINT32			i;
	ULONG   		ulNumOfElements=0;
	WORD			wObjectFormatCode = 0;
	T_BF_FS_IF_STAT	file_stat;
	DWORD 			dwObjectSize;
	FW_CHAR 		cFileName[DOS_FILENAME_LEN*2];
	FW_CHAR 		szTempString[MTP_MAX_PATH];
	FW_UCHAR		*Addr;
	ULONG  			ulSize = 0;	

	ulSize = 12+sizeof(UINT32);
	for (i=1;i<=g_nObjectCnt;i++)
	{

		if (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE)
			wObjectFormatCode = MTP_OFC_ASSOCIATION;
		else if (xIsJpgFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_EXIFJPEG;
		else if (xIsBmpFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_BMP;
		else if (xIsGifFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_GIF;
		else if (xIsTiffFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_TIFF;
		else if (xIsAviFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_AVI;
		else if (xIsDpofFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_DPOF;
		else if (xIsMovFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_MOV;
		else if (xIsMp4File(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_MP4;
		else if (xIsWavFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_WAV;
		else if (xIsMp3File(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_MP3;

		if ( ( (ulObjectFormatCode == MTP_OFC_ASSOCIATION) || (ulObjectFormatCode == 0x0000) )	&&
			(wObjectFormatCode == MTP_OFC_ASSOCIATION) &&(oMTPDBFileInfo[i].bit.fDelete != 1) ) 							
		{							
			ulNumOfElements += ASSOCIATION_SUPPPROP_NUM;
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetPathName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = 0;								
			xAddAllAssociationObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);		
			
		}				
		else if ( ( (ulObjectFormatCode == MTP_OFC_MP3) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_MP3) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
		{		
			ulNumOfElements += MP3_SUPPPROP_NUM;				
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddAllMP3ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
		}		
		else if ( ( (ulObjectFormatCode == MTP_OFC_BMP) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_BMP) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
		{		
			ulNumOfElements += BMP_SUPPPROP_NUM;				 
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddAllBMPObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
		}				
		else if ( ( (ulObjectFormatCode == MTP_OFC_GIF) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_GIF) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
		{
			ulNumOfElements += GIF_SUPPPROP_NUM;					
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddAllGIFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
		}		
		else if ( ( (ulObjectFormatCode == MTP_OFC_EXIFJPEG) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_EXIFJPEG) && (oMTPDBFileInfo[i].bit.fDelete != 1))				
		{
			ulNumOfElements += JPG_SUPPPROP_NUM;			
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat);
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			dwObjectSize = file_stat.fileSize;
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 
			xAddAllJPGObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);	
		}	
		else if ( ( (ulObjectFormatCode == MTP_OFC_AVI) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_AVI) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
		{
			ulNumOfElements += AVI_SUPPPROP_NUM;				
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddAllAVIObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
		}	
		else if ( ( (ulObjectFormatCode == MTP_OFC_DPOF) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_DPOF) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
		{
			ulNumOfElements += DPOF_SUPPPROP_NUM;				
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddAllDPOFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
		}	
		else if ( ( (ulObjectFormatCode == MTP_OFC_MOV) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_MOV) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
		{
			ulNumOfElements += MOV_SUPPPROP_NUM;				
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddAllMOVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
		}	
		else if ( ( (ulObjectFormatCode == MTP_OFC_MP4) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_MP4) && (oMTPDBFileInfo[i].bit.fDelete != 1))
		{
			ulNumOfElements += MP4_SUPPPROP_NUM;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName);
			dwObjectSize = file_stat.fileSize;
			xAddAllMP4ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);
		}	
		else if ( ( (ulObjectFormatCode == MTP_OFC_WAV) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_WAV) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
		{
			ulNumOfElements += WAV_SUPPPROP_NUM;					
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddAllWAVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
		}	
		else if ( ( (ulObjectFormatCode == MTP_OFC_TIFF) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_TIFF) && (oMTPDBFileInfo[i].bit.fDelete != 1))				
		{
			ulNumOfElements += TIFF_SUPPPROP_NUM;					
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddAllTIFFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);					
		}								
	}
	
	Addr = IN_Buffers+12;
	*Addr=(UCHAR)(ulNumOfElements&0x000000FF);
	*(Addr+1)=(UCHAR)((ulNumOfElements&0x0000FF00)>>8); 
	*(Addr+2)=(UCHAR)((ulNumOfElements&0x00FF0000)>>16);	
	*(Addr+3)=(UCHAR)((ulNumOfElements&0xFF000000)>>24);

	SendData(ulSize-12);
	SendResponseSimple(PTP_RS_OK);	
	return 0;
}

INT32 xAddAllHandleAllPropertyByGroup(ULONG ulObjectHandle,ULONG ulObjectFormatCode,ULONG ulObjectPropCode,ULONG ulObjectPropGroupCode,ULONG ulDepth)
{
	UINT32			i;
	ULONG   		ulTempValue = 0;
	USHORT			usTempValue;
	ULONG   		ulNumOfElements=0;
	WORD			wObjectFormatCode = 0;
	T_BF_FS_IF_STAT	file_stat;
	DWORD 			dwObjectSize;
	FW_CHAR 		cFileName[DOS_FILENAME_LEN*2];
	FW_CHAR 		szTempString[MTP_MAX_PATH];
	INT32			nFolderParent=0;
	MP3PROP			oMP3Prop;
	BMPPROP			oBmpProp;
	GIFPROP			oGifProp;
	JFIFPROP		oJFIFProp;
	AVIPROP			oAVIProp;
	MOVPROP			oMOVProp;
	TIFFPROP		oTiffProp;
	WAVPROP			oWavProp;
	UCHAR			*Addr;
	ULONG  			ulSize = 0;
	//char 			szDate[32];
	char 			szAscDate[16];
	char 			szUniDate[32];		
	UINT32			picX=0,picY=0,thumbX=0,thumbY=0,ThumbcompressSize=0;
	ULONG 			length = 0;

	memset(szAscDate, 0, sizeof(szAscDate));
	ulSize = 12+sizeof(UINT32); 

	for (i=1;i<=g_nObjectCnt;i++)
	{
		if (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE)
			wObjectFormatCode = MTP_OFC_ASSOCIATION;
		else if (xIsJpgFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_EXIFJPEG;
		else if (xIsBmpFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_BMP;
		else if (xIsGifFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_GIF;
		else if (xIsTiffFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_TIFF;
		else if (xIsAviFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_AVI;
		else if (xIsDpofFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_DPOF;
		else if (xIsMovFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_MOV;
		else if (xIsMp4File(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_MP4;
		else if (xIsWavFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_WAV;
		else if (xIsMp3File(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_MP3;

		if ( ( (ulObjectFormatCode == MTP_OFC_ASSOCIATION) || (ulObjectFormatCode == 0x0000) )	&&
			(wObjectFormatCode == MTP_OFC_ASSOCIATION) &&(oMTPDBFileInfo[i].bit.fDelete != 1) ) 							
		{											
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetPathName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = 0;	
			if (ulObjectPropGroupCode  == 0x00000001)
			{					
				ulNumOfElements += ASSOCIATION_SUPPPROP_NUM;
				ulTempValue = ENDIAN_SWAP32(ulTempValue);
				xAddAllAssociationObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,nFolderParent,cFileName,&ulSize);
			}					
		}					
		else if ( ( (ulObjectFormatCode == MTP_OFC_MP3) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_MP3) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
		{
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					 
			dwObjectSize = file_stat.fileSize;									

			if (ulObjectPropGroupCode == 0x00000001)
			{					
				ulNumOfElements += MTP_MP3_GROUP1;				
//				if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//					ulTempValue = PTP_StorageID_SD;	
//				}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//					ulTempValue = PTP_StorageID_NF;	
//				}else{
//					ulTempValue = PTP_StorageID_MS;	
//				}
				ulTempValue = PtpStorageID;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																	
				usTempValue = MTP_OFC_MP3; 
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 											
				//usTempValue = PROTECT_NoProtect;	
				// 2009/03/31 STX tony +
				if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
				{
					usTempValue = PROTECT_ReadOnly;
				}
				else
				{
					usTempValue = PROTECT_NoProtect;
				}
				// 2009/03/31 STX tony -				
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 															
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);												
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize); 							
				ulTempValue = oMTPDBFileInfo[i].nParentIndex;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 				
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);																	
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);										
			}
			else if (ulObjectPropGroupCode == 0x00000002)
			{
				ulNumOfElements += MTP_MP3_GROUP2;
				mp3_parse(oMTPDBFileInfo[i].szDosFileName,&oMP3Prop);
				ulTempValue = oMP3Prop.nDuration;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);		
				ulTempValue = oMP3Prop.fMP3SampleRate;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);				
				usTempValue = oMP3Prop.nNumberOfChannels;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&usTempValue,&ulSize);
				ulTempValue = oMP3Prop.fMP3BitRate;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 
			}					
		}		
		else if ( ( (ulObjectFormatCode == MTP_OFC_BMP) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_BMP) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
		{
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									

			if (ulObjectPropGroupCode == 0x00000001)
			{					
				ulNumOfElements += MTP_BMP_GROUP1;					
//					if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//						ulTempValue = PTP_StorageID_SD;	
//					}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//						ulTempValue = PTP_StorageID_NF;	
//					}else{
//						ulTempValue = PTP_StorageID_MS;	
//					}
				ulTempValue = PtpStorageID;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																	
				usTempValue = MTP_OFC_BMP; 
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 											
				//usTempValue = PROTECT_NoProtect;	
				// 2009/03/31 STX tony +
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);	
				if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
				{
					usTempValue = PROTECT_ReadOnly;
				}
				else
				{
					usTempValue = PROTECT_NoProtect;
				}
				// 2009/03/31 STX tony -				
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 															
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);												
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize); 							
				ulTempValue = oMTPDBFileInfo[i].nParentIndex;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 				
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);																	
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);							
			}
			else if (ulObjectPropGroupCode == 0x00000002)
			{
				ulNumOfElements += MTP_BMP_GROUP2;										
				bmp_parse(oMTPDBFileInfo[i].szDosFileName,&oBmpProp);
				ulTempValue = oBmpProp.dwWidth;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);							
				ulTempValue = oBmpProp.dwHeight;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);
				usTempValue = oBmpProp.wImageBitDepth;
				usTempValue = ENDIAN_SWAP16(usTempValue);
				ulTempValue = usTempValue;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_IMAGEBITDEPTH,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);						
			
			}					
		}						
		else if ( ( (ulObjectFormatCode == MTP_OFC_GIF) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_GIF) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
		{
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			ulTempValue = ENDIAN_SWAP32(ulTempValue);

			if (ulObjectPropGroupCode == 0x00000001)
			{
				ulNumOfElements += MTP_GIF_GROUP1;				
//					if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//						ulTempValue = PTP_StorageID_SD;	
//					}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//						ulTempValue = PTP_StorageID_NF;	
//					}else{
//						ulTempValue = PTP_StorageID_MS;	
//					}
				ulTempValue = PtpStorageID;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																	
				usTempValue = MTP_OFC_GIF; 
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 											
				//usTempValue = PROTECT_NoProtect;	
				// 2009/03/31 STX tony +
				if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
				{
					usTempValue = PROTECT_ReadOnly;
				}
				else
				{
					usTempValue = PROTECT_NoProtect;
				}
				// 2009/03/31 STX tony -				
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 															
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);												
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize); 							
				ulTempValue = oMTPDBFileInfo[i].nParentIndex;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 				
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);																	
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);							
			}
			else if (ulObjectPropGroupCode == 0x00000002)
			{
				ulNumOfElements += MTP_GIF_GROUP2;		
				gif_parse(oMTPDBFileInfo[i].szDosFileName,&oGifProp);
				usTempValue = oGifProp.wWidth;
				usTempValue = ENDIAN_SWAP16(usTempValue);
				ulTempValue = usTempValue;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);							
				usTempValue = oGifProp.wHeight;
				usTempValue = ENDIAN_SWAP16(usTempValue);
				ulTempValue = usTempValue;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);
				ulTempValue = oGifProp.cImageBitDepth;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_IMAGEBITDEPTH,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);						

			}					
		}		
		else if ( ( (ulObjectFormatCode == MTP_OFC_EXIFJPEG) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_EXIFJPEG) && (oMTPDBFileInfo[i].bit.fDelete != 1))				
		{
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									

			if (ulObjectPropGroupCode == 0x00000001)
			{
				ulNumOfElements += MTP_JPG_GROUP1;						
//					if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//						ulTempValue = PTP_StorageID_SD;	
//					}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//						ulTempValue = PTP_StorageID_NF;	
//					}else{
//						ulTempValue = PTP_StorageID_MS;	
//					}	
				ulTempValue = PtpStorageID;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																	
				usTempValue = MTP_OFC_EXIFJPEG; 
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 											
				//usTempValue = PROTECT_NoProtect;	
				// 2009/03/31 STX tony +
				if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
				{
					usTempValue = PROTECT_ReadOnly;
				}
				else
				{
					usTempValue = PROTECT_NoProtect;
				}
				// 2009/03/31 STX tony -				
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 															
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);													
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize); 							

				PTP_fp = BF_Fs_If_Open(oMTPDBFileInfo[i].szDosFileName, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
				GetJpegPicInfo(&ThumbcompressSize, &thumbX, &thumbY, &picX, &picY, szAscDate);
				if(strlen(szAscDate) > 0)
				{
					ASCIIToUniCode(szAscDate,szUniDate);	
				}
				else
				{
					memset(szUniDate,0,MTP_DATETIME_LENGTH*2);
				}
//				xAddObjectPorpValueListForVarLenString(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_DATECREATED,MTP_DATATYPE_STR,szUniDate,&ulSize,MTP_DATETIME_LENGTH);	
				BF_Fs_If_Close(PTP_fp);
				/*  20120822 Add PTP_fp set to zero if PTP_fp is Close  */
				PTP_fp = 0;

//				// 2009/03/31 STX tony +
//				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);
//				snprintf(szAscDate, sizeof(szAscDate), "%04d%02d%02d%c%02d%02d%02d", ((file_stat.date >> 9) + 1980), (file_stat.date >> 5) & 0x0f, (file_stat.date) & 0x1f, 
//							   0x54, (file_stat.time >> 11) & 0x1f, (file_stat.time >> 5) & 0x3f, (file_stat.time & 0x1f) * 2);
//				
//				ASCIIToUniCode(szAscDate,szUniDate); 				
//				xAddObjectPorpValueListForVarLenString(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_DATEMODIFIED,MTP_DATATYPE_STR,szUniDate,&ulSize,MTP_DATETIME_LENGTH);
				// 2009/03/31 STX tony -
		
				ulTempValue = oMTPDBFileInfo[i].nParentIndex; 
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 		
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);	
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);					
				usTempValue = MTP_OFC_EXIFJPEG;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleFormat,MTP_DATATYPE_UINT16,&usTempValue,&ulSize);
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleSize,MTP_DATATYPE_UINT32,&ThumbcompressSize,&ulSize);
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleHeigth,MTP_DATATYPE_UINT32,&thumbY,&ulSize);
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleWidth,MTP_DATATYPE_UINT32,&thumbX,&ulSize);

				xAddObjectPorpValueListNoValue(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleData,MTP_DATATYPE_AUINT8,&ulSize);
				ulSize = ulSize + sizeof(UINT32);
				if(GetJpegThumbnail(oMTPDBFileInfo[i].szDosFileName, (FW_ULONG)(IN_Buffers+ulSize), 65536 - 12, &length) != FJ_ERR_OK)
				{
					// Error 
					BF_Debug_Print_Error(("xAddAllHandleAllPropertyByGroup GetJpegThumbnail error\n"));
				}
				else
				{
					ulSize = ulSize + length;
				}
				// Write Thumbnail Size
				ulTempValue = length;
				Addr = IN_Buffers + ulSize - length - sizeof(UINT32);
				*Addr=(UCHAR)(ulTempValue&0x000000FF);
				*(Addr+1)=(UCHAR)((ulTempValue&0x0000FF00)>>8); 
				*(Addr+2)=(UCHAR)((ulTempValue&0x00FF0000)>>16);
				*(Addr+3)=(UCHAR)((ulTempValue&0xFF000000)>>24);
			}
			else if (ulObjectPropGroupCode == 0x00000002)
			{
				ulNumOfElements += MTP_JPG_GROUP2;									
				jfif_parse(oMTPDBFileInfo[i].szDosFileName,&oJFIFProp); 					
				ulTempValue = oJFIFProp.wWidth; 					
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);							
				ulTempValue = oJFIFProp.wHeight;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);
				ulTempValue = oJFIFProp.cImageBitDepth;
				ulTempValue = ulTempValue * 3;
				//ulTempValue = 0; // 2009/03/27 tony +
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_IMAGEBITDEPTH,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);											
			}				
		}	
		else if ( ( (ulObjectFormatCode == MTP_OFC_AVI) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_AVI) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
		{
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									

			if (ulObjectPropGroupCode == 0x00000001)
			{										
				ulNumOfElements += MTP_AVI_GROUP1;			
//					if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//						ulTempValue = PTP_StorageID_SD;	
//					}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//						ulTempValue = PTP_StorageID_NF;	
//					}else{
//						ulTempValue = PTP_StorageID_MS;	
//					}
				ulTempValue = PtpStorageID;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																	
				usTempValue = MTP_OFC_AVI; 
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 											
				//usTempValue = PROTECT_NoProtect;	
				// 2009/03/31 STX tony +
				if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
				{
					usTempValue = PROTECT_ReadOnly;
				}
				else
				{
					usTempValue = PROTECT_NoProtect;
				}
				// 2009/03/31 STX tony -				
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 															
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);													
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize); 
				
				// 2009/04/08 STX tony + (for avi modify date)
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName, (T_BF_FS_IF_STAT *)&file_stat);
				snprintf(szAscDate, sizeof(szAscDate), "%04d%02d%02d%c%02d%02d%02d", ((file_stat.date >> 9) + 1980), (file_stat.date >> 5) & 0x0f, (file_stat.date) & 0x1f, \
							   0x54, (file_stat.time >> 11) & 0x1f, (file_stat.time >> 5) & 0x3f, (file_stat.time & 0x1f) * 2);
				
				ASCIIToUniCode(szAscDate,szUniDate); 				
				xAddObjectPorpValueListForVarLenString(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_DATEMODIFIED,MTP_DATATYPE_STR,szUniDate,&ulSize,16);
				// 2009/04/08 STX tony -
				
				ulTempValue = oMTPDBFileInfo[i].nParentIndex;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 		
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);	
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);					
			
			}
			else if (ulObjectPropGroupCode == 0x00000002)
			{
				ulNumOfElements += MTP_AVI_GROUP2;			
				avi_parse(oMTPDBFileInfo[i].szDosFileName,&oAVIProp);							
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&oAVIProp.dwWidth,&ulSize);										
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&oAVIProp.dwHeight,&ulSize); 									
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&oAVIProp.dwDuration,&ulSize); 										
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&oAVIProp.dwSampleRate,&ulSize); 				
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&oAVIProp.dwNumberOfChannels,&ulSize);
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&oAVIProp.dwAudioBitRate,&ulSize); 		
			}
			else if (ulObjectPropGroupCode	== 0x00000004)
			{
				ulNumOfElements += 3;					
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_SCANTYPE,MTP_DATATYPE_UINT16,&oAVIProp.dwScanType,&ulSize); 				
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_VIDEOFOURCCCODEC,MTP_DATATYPE_UINT32,&oAVIProp.dwVideoFourCCCodec,&ulSize);
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_VIDEOBITRATE,MTP_DATATYPE_UINT32,&oAVIProp.dwVideoBitRate,&ulSize); 
			}
			
		}	
		else if ( ( (ulObjectFormatCode == MTP_OFC_DPOF) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_DPOF) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
		{
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									

			if (ulObjectPropGroupCode == 0x00000001)
			{										
				ulNumOfElements += MTP_DPOF_GROUP1;			
//					if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//						ulTempValue = PTP_StorageID_SD;	
//					}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//						ulTempValue = PTP_StorageID_NF;	
//					}else{
//						ulTempValue = PTP_StorageID_MS;	
//					}
				ulTempValue = PtpStorageID;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																	
				usTempValue = MTP_OFC_DPOF; 
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 											

				if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
				{
					usTempValue = PROTECT_ReadOnly;
				}
				else
				{
					usTempValue = PROTECT_NoProtect;
				}

				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 															
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);													
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize); 
				
				ulTempValue = oMTPDBFileInfo[i].nParentIndex;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 		
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);	
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);					
			
			}
		}
		else if ( ( (ulObjectFormatCode == MTP_OFC_MOV) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_MOV) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
		{
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									

			if (ulObjectPropGroupCode == 0x00000001)
			{										
				ulNumOfElements += MTP_MOV_GROUP1;			
//					if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//						ulTempValue = PTP_StorageID_SD;	
//					}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//						ulTempValue = PTP_StorageID_NF;	
//					}else{
//						ulTempValue = PTP_StorageID_MS;	
//					}
				ulTempValue = PtpStorageID;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																	
				usTempValue = MTP_OFC_MOV; 
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 											

				if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
				{
					usTempValue = PROTECT_ReadOnly;
				}
				else
				{
					usTempValue = PROTECT_NoProtect;
				}

				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 															
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);													
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize); 
				
				ulTempValue = oMTPDBFileInfo[i].nParentIndex;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 		
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);	
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);					

				usTempValue = 14344;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleFormat,MTP_DATATYPE_UINT16,&usTempValue,&ulSize);
				if(GetMovInfo(&ThumbcompressSize, &thumbX, &thumbY, &thumbX, &thumbY, oMTPDBFileInfo[i].szDosFileName) != FJ_ERR_OK)
				{
					// Error 
					BF_Debug_Print_Error(("xAddAllHandleAllPropertyByGroup GetMovInfo error\n"));
					ulTempValue = 0x00000000;
				}
				else
				{
					ulTempValue = ThumbcompressSize;
				}
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleSize,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);
				ulTempValue = 120;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleHeigth,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);
				ulTempValue = 160;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleWidth,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);
				xAddObjectPorpValueListNoValue(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleData,MTP_DATATYPE_AUINT8,&ulSize);
				if(GetMovThumbnailAndNumElements(oMTPDBFileInfo[i].szDosFileName, (FW_ULONG)(IN_Buffers+ulSize), 65536 - 12, &length) != FJ_ERR_OK)
				{
					// Error 
					BF_Debug_Print_Error(("xAddAllHandleAllPropertyByGroup GetMovThumbnail error[MOV]\n"));
				}
				ulSize = ulSize + length;

			
			}
			else if (ulObjectPropGroupCode == 0x00000002)
			{
				ulNumOfElements += MTP_MOV_GROUP2;			
				mov_parse(oMTPDBFileInfo[i].szDosFileName,&oMOVProp);							
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&oMOVProp.dwWidth,&ulSize);
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&oMOVProp.dwHeight,&ulSize);
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&oMOVProp.dwDuration,&ulSize);
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&oMOVProp.dwSampleRate,&ulSize);
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&oMOVProp.dwNumberOfChannels,&ulSize);
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&oMOVProp.dwAudioBitRate,&ulSize);
			}
		}
		else if ( ( (ulObjectFormatCode == MTP_OFC_MP4) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_MP4) && (oMTPDBFileInfo[i].bit.fDelete != 1))
		{
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName);
			dwObjectSize = file_stat.fileSize;

			if (ulObjectPropGroupCode == 0x00000001)
			{
				ulNumOfElements += MTP_MP4_GROUP1;
//					if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//						ulTempValue = PTP_StorageID_SD;	
//					}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//						ulTempValue = PTP_StorageID_NF;	
//					}else{
//						ulTempValue = PTP_StorageID_MS;	
//					}
				ulTempValue = PtpStorageID;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																	
				usTempValue = MTP_OFC_MP4; 
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 											

				if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
				{
					usTempValue = PROTECT_ReadOnly;
				}
				else
				{
					usTempValue = PROTECT_NoProtect;
				}

				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 															
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);													
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize); 
				
				ulTempValue = oMTPDBFileInfo[i].nParentIndex;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 		
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);	
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);					

				usTempValue = 14344;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleFormat,MTP_DATATYPE_UINT16,&usTempValue,&ulSize);
				if(GetMovInfo(&ThumbcompressSize, &thumbX, &thumbY, &thumbX, &thumbY, oMTPDBFileInfo[i].szDosFileName) != FJ_ERR_OK)
				{
					// Error 
					BF_Debug_Print_Error(("xAddAllHandleAllPropertyByGroup GetMovInfo error[MP4]\n"));
					ulTempValue = 0x00000000;
				}
				else
				{
					ulTempValue = ThumbcompressSize;
				}
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleSize,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);
				ulTempValue = 120;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleHeigth,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);
				ulTempValue = 160;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleWidth,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);
				xAddObjectPorpValueListNoValue(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_REPSampleData,MTP_DATATYPE_AUINT8,&ulSize);
				if(GetMovThumbnailAndNumElements(oMTPDBFileInfo[i].szDosFileName, (FW_ULONG)(IN_Buffers+ulSize), 65536 - 12, &length) != FJ_ERR_OK)
				{
					// Error 
					BF_Debug_Print_Error(("xAddAllHandleAllPropertyByGroup GetMovThumbnail error[MP4]\n"));
				}
				ulSize = ulSize + length;

			
			}
			else if (ulObjectPropGroupCode == 0x00000002)
			{
				ulNumOfElements += MTP_MP4_GROUP2;
				mov_parse(oMTPDBFileInfo[i].szDosFileName,&oMOVProp);
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&oMOVProp.dwWidth,&ulSize);
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&oMOVProp.dwHeight,&ulSize);
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&oMOVProp.dwDuration,&ulSize);
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&oMOVProp.dwSampleRate,&ulSize);
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&oMOVProp.dwNumberOfChannels,&ulSize);
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&oMOVProp.dwAudioBitRate,&ulSize);
			}
		}
		else if ( ( (ulObjectFormatCode == MTP_OFC_WAV) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_WAV) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
		{
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			if (ulObjectPropGroupCode == 0x00000001)
			{					
				ulNumOfElements += MTP_WAV_GROUP1;					
//					if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//						ulTempValue = PTP_StorageID_SD;	
//					}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//						ulTempValue = PTP_StorageID_NF;	
//					}else{
//						ulTempValue = PTP_StorageID_MS;	
//					}
				ulTempValue = PtpStorageID;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																	
				usTempValue = MTP_OFC_WAV; 
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 											
				//usTempValue = PROTECT_NoProtect;	
				// 2009/03/31 STX tony +
				if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
				{
					usTempValue = PROTECT_ReadOnly;
				}
				else
				{
					usTempValue = PROTECT_NoProtect;
				}
				// 2009/03/31 STX tony -				
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 															
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);												
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize); 							
				ulTempValue = oMTPDBFileInfo[i].nParentIndex;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 				
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);																	
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);												
			}
			else if (ulObjectPropGroupCode == 0x00000002)
			{			
				ulNumOfElements += MTP_WAV_GROUP2;						
				wav_parse(oMTPDBFileInfo[i].szDosFileName,&oWavProp); 
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_DURATION,MTP_DATATYPE_UINT32,&oWavProp.dwDuration,&ulSize); 				
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_SAMPLERATE,MTP_DATATYPE_UINT32,&oWavProp.dwSampleRate,&ulSize); 				
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NUMBEROFCHANNEL,MTP_DATATYPE_UINT16,&oWavProp.dwNumberOfChannels,&ulSize);						
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_AUDIOBITRATE,MTP_DATATYPE_UINT32,&oWavProp.dwAudioBitRate,&ulSize); 														
			}					
		}	
		else if ( ( (ulObjectFormatCode == MTP_OFC_TIFF) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_TIFF) && (oMTPDBFileInfo[i].bit.fDelete != 1))				
		{
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									

			if (ulObjectPropGroupCode == 0x00000001)
			{			
				ulNumOfElements += MTP_TIFF_GROUP1;				
//					if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
//						ulTempValue = PTP_StorageID_SD;	
//					}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
//						ulTempValue = PTP_StorageID_NF;	
//					}else{
//						ulTempValue = PTP_StorageID_MS;	
//					}
				ulTempValue = PtpStorageID;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_STORAGEID,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);																	
				usTempValue = MTP_OFC_TIFF; 
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFORMAT,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 											
				//usTempValue = PROTECT_NoProtect;	
				// 2009/03/31 STX tony +
				if ((file_stat.attributes & D_BF_FS_IF_ATTR_READONLY) == D_BF_FS_IF_ATTR_READONLY)
				{
					usTempValue = PROTECT_ReadOnly;
				}
				else
				{
					usTempValue = PROTECT_NoProtect;
				}
				// 2009/03/31 STX tony -				
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PROTECTIONSTATUS,MTP_DATATYPE_UINT16,&usTempValue,&ulSize); 															
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTSIZE,MTP_DATATYPE_UINT64,&dwObjectSize,&ulSize);												
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_OBJECTFILENAME,MTP_DATATYPE_STR,cFileName,&ulSize); 							
				ulTempValue = oMTPDBFileInfo[i].nParentIndex;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PARENTOBJECT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize); 				
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_PUOI,MTP_DATATYPE_UINT128,&oMTPDBFileInfo[i].nFileIndex,&ulSize);																	
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_NAME,MTP_DATATYPE_STR,cFileName,&ulSize);										
			}
			else if (ulObjectPropGroupCode == 0x00000002)
			{	
				ulNumOfElements += MTP_TIFF_GROUP2; 					
				tiff_parse(oMTPDBFileInfo[i].szDosFileName,&oTiffProp);
				ulTempValue = oTiffProp.dwWidth;					
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_WIDTH,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);							
				ulTempValue = oTiffProp.dwHeight;					
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_HEIGHT,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);
				ulTempValue = oTiffProp.wImageBitDepth;
				xAddObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,MTP_OPC_IMAGEBITDEPTH,MTP_DATATYPE_UINT32,&ulTempValue,&ulSize);						
			}				
		}						

	}

	Addr = IN_Buffers+12;
	*Addr=(UCHAR)(ulNumOfElements&0x000000FF);
	*(Addr+1)=(UCHAR)((ulNumOfElements&0x0000FF00)>>8); 
	*(Addr+2)=(UCHAR)((ulNumOfElements&0x00FF0000)>>16);	
	*(Addr+3)=(UCHAR)((ulNumOfElements&0xFF000000)>>24);		

	SendData(ulSize-12);
	SendResponseSimple(PTP_RS_OK);	
	return 0;

}

INT32 xAddAllHandleAllPropertyByDepth(ULONG ulObjectHandle,ULONG ulObjectFormatCode,ULONG ulObjectPropCode,ULONG ulObjectPropGroupCode,ULONG ulDepth)
{
	UINT32			i;
	ULONG   		ulNumOfElements=0;
	WORD			wObjectFormatCode = 0;
	T_BF_FS_IF_STAT	file_stat;
	DWORD 			dwObjectSize;
	FW_CHAR 		cFileName[DOS_FILENAME_LEN*2];
	FW_CHAR 		szTempString[MTP_MAX_PATH];
	FW_UCHAR		*Addr;
	INT32 			nLevel=0;
	ULONG  			ulSize = 0;	

	ulSize = 12+sizeof(UINT32);
	for (i=1;i<=g_nObjectCnt;i++)
	{
		if ( (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
		{
			xGetObjectLevel(oMTPDBFileInfo[i].szDosFileName,TRUE,&nLevel);
		}
		else if ( (oMTPDBFileInfo[i].bit.file_attr == MTP_FILE_TYPE) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
		{
			xGetObjectLevel(oMTPDBFileInfo[i].szDosFileName,FALSE,&nLevel);
		}
	
		if (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE)
			wObjectFormatCode = MTP_OFC_ASSOCIATION;
		else if (xIsJpgFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_EXIFJPEG;
		else if (xIsBmpFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_BMP;
		else if (xIsGifFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_GIF;
		else if (xIsTiffFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_TIFF;
		else if (xIsAviFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_AVI;
		else if (xIsDpofFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_DPOF;
		else if (xIsMovFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_MOV;
		else if (xIsMp4File(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_MP4;
		else if (xIsWavFile(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_WAV;
		else if (xIsMp3File(oMTPDBFileInfo[i].szDosFileName) == 0)
			wObjectFormatCode = MTP_OFC_MP3;
	
		if ( ( (ulObjectFormatCode == MTP_OFC_ASSOCIATION) || (ulObjectFormatCode == 0x0000) )	&&
			(wObjectFormatCode == MTP_OFC_ASSOCIATION) &&(oMTPDBFileInfo[i].bit.fDelete != 1) 
			&& (nLevel<=ulDepth) )							
		{							
			ulNumOfElements += ASSOCIATION_SUPPPROP_NUM;
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetPathName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = 0;								
			xAddAllAssociationObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);		
			
		}				
		else if ( ( (ulObjectFormatCode == MTP_OFC_MP3) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_MP3) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
			&& (nLevel<=ulDepth) )				
		{		
			ulNumOfElements += MP3_SUPPPROP_NUM;				
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddAllMP3ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
		}			
		else if ( ( (ulObjectFormatCode == MTP_OFC_BMP) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_BMP) && (oMTPDBFileInfo[i].bit.fDelete != 1)
			&& (nLevel<=ulDepth) )				
		{		
			ulNumOfElements += BMP_SUPPPROP_NUM;					 
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddAllBMPObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
		}				
		else if ( ( (ulObjectFormatCode == MTP_OFC_GIF) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_GIF) && (oMTPDBFileInfo[i].bit.fDelete != 1)
			&& (nLevel<=ulDepth) )				
		{
			ulNumOfElements += GIF_SUPPPROP_NUM;				
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddAllGIFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
		}			
		else if ( ( (ulObjectFormatCode == MTP_OFC_EXIFJPEG) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_EXIFJPEG) && (oMTPDBFileInfo[i].bit.fDelete != 1)
			&& (nLevel<=ulDepth))				
		{
			ulNumOfElements += JPG_SUPPPROP_NUM;			
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat);
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			dwObjectSize = file_stat.fileSize;
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 			
			xAddAllJPGObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);	
		}	
		else if ( ( (ulObjectFormatCode == MTP_OFC_AVI) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_AVI) && (oMTPDBFileInfo[i].bit.fDelete != 1)
			&& (nLevel<=ulDepth) )				
		{
			ulNumOfElements += AVI_SUPPPROP_NUM;					
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddAllAVIObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
		}	
		else if ( ( (ulObjectFormatCode == MTP_OFC_DPOF) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_DPOF) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
		{
			ulNumOfElements += DPOF_SUPPPROP_NUM;				
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddAllDPOFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
		}	
		else if ( ( (ulObjectFormatCode == MTP_OFC_MOV) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_MOV) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
		{
			ulNumOfElements += MOV_SUPPPROP_NUM;				
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddAllMOVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
		}	
		else if ( ( (ulObjectFormatCode == MTP_OFC_MP4) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_MP4) && (oMTPDBFileInfo[i].bit.fDelete != 1))
		{
			ulNumOfElements += MP4_SUPPPROP_NUM;
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName);
			dwObjectSize = file_stat.fileSize;
			xAddAllMP4ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);
		}	
		else if ( ( (ulObjectFormatCode == MTP_OFC_WAV) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_WAV) && (oMTPDBFileInfo[i].bit.fDelete != 1) 
			&& (nLevel<=ulDepth))				
		{
			ulNumOfElements += WAV_SUPPPROP_NUM;					
			 BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddAllWAVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
		}	
		else if ( ( (ulObjectFormatCode == MTP_OFC_TIFF) || (ulObjectFormatCode == 0x0000) ) &&
			(wObjectFormatCode == MTP_OFC_TIFF) && (oMTPDBFileInfo[i].bit.fDelete != 1)
			&& (nLevel<=ulDepth) )				
		{
			ulNumOfElements += TIFF_SUPPPROP_NUM;				
			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
			ASCIIToUniCode(szTempString,cFileName); 					
			dwObjectSize = file_stat.fileSize;									
			xAddAllTIFFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);					
		}								
	}	

	Addr = IN_Buffers+12;
	*Addr=(UCHAR)(ulNumOfElements&0x000000FF);
	*(Addr+1)=(UCHAR)((ulNumOfElements&0x0000FF00)>>8); 
	*(Addr+2)=(UCHAR)((ulNumOfElements&0x00FF0000)>>16);	
	*(Addr+3)=(UCHAR)((ulNumOfElements&0xFF000000)>>24);

	SendData(ulSize-12);
	SendResponseSimple(PTP_RS_OK);	
	return 0;		

}

INT32 xAddAllHandleAllPropertyByFolder(ULONG ulObjectHandle,ULONG ulObjectFormatCode,ULONG ulObjectPropCode,ULONG ulObjectPropGroupCode,ULONG ulDepth)
{
	UINT32			i;
	ULONG   		ulNumOfElements=0;
	WORD			wObjectFormatCode = 0;
	T_BF_FS_IF_STAT	file_stat;
	DWORD 			dwObjectSize;
	FW_CHAR 		cFileName[DOS_FILENAME_LEN*2];
	FW_CHAR 		szTempString[MTP_MAX_PATH];
	FW_UCHAR		*Addr;
	ULONG  			ulSize = 0;	
	BOOL 			bIsFind=FALSE;

	ulSize = 12+sizeof(UINT32);
	for (i=1;i<=g_nObjectCnt;i++)
	{
		xCheckParentObject(i,ulObjectHandle,&bIsFind);

		if (bIsFind == TRUE)
		{
			bIsFind = FALSE;
			if (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE)
				wObjectFormatCode = MTP_OFC_ASSOCIATION;
			else if (xIsJpgFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_EXIFJPEG;
			else if (xIsBmpFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_BMP;
			else if (xIsGifFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_GIF;
			else if (xIsTiffFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_TIFF;
			else if (xIsAviFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_AVI;
			else if (xIsDpofFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_DPOF;
			else if (xIsMovFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MOV;
			else if (xIsMp4File(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MP4;
			else if (xIsWavFile(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_WAV;
			else if (xIsMp3File(oMTPDBFileInfo[i].szDosFileName) == 0)
				wObjectFormatCode = MTP_OFC_MP3;

			if ( ( (ulObjectFormatCode == MTP_OFC_ASSOCIATION) || (ulObjectFormatCode == 0x0000) )	&&
				(wObjectFormatCode == MTP_OFC_ASSOCIATION) &&(oMTPDBFileInfo[i].bit.fDelete != 1) ) 							
			{							
				ulNumOfElements += ASSOCIATION_SUPPPROP_NUM;
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetPathName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = 0;								
				xAddAllAssociationObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);		
				
			}				
			else if ( ( (ulObjectFormatCode == MTP_OFC_MP3) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_MP3) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
			{		
				ulNumOfElements += MP3_SUPPPROP_NUM;					
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllMP3ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
			}		
			else if ( ( (ulObjectFormatCode == MTP_OFC_BMP) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_BMP) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
			{		
				ulNumOfElements += BMP_SUPPPROP_NUM;				 
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllBMPObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
			}					
			else if ( ( (ulObjectFormatCode == MTP_OFC_GIF) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_GIF) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
			{
				ulNumOfElements += GIF_SUPPPROP_NUM;						
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllGIFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
			}		
			else if ( ( (ulObjectFormatCode == MTP_OFC_EXIFJPEG) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_EXIFJPEG) && (oMTPDBFileInfo[i].bit.fDelete != 1))				
			{
				ulNumOfElements += JPG_SUPPPROP_NUM;				
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat);
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				dwObjectSize = file_stat.fileSize;
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName);				
				xAddAllJPGObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);	
			}	
			else if ( ( (ulObjectFormatCode == MTP_OFC_AVI) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_AVI) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
			{
				ulNumOfElements += AVI_SUPPPROP_NUM;				
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllAVIObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
			}	
			else if ( ( (ulObjectFormatCode == MTP_OFC_DPOF) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_DPOF) && (oMTPDBFileInfo[i].bit.fDelete != 1))				
			{
				ulNumOfElements += DPOF_SUPPPROP_NUM;				
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat);
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				dwObjectSize = file_stat.fileSize;
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName);				
				xAddAllDPOFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);	
			}	
			else if ( ( (ulObjectFormatCode == MTP_OFC_MOV) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_MOV) && (oMTPDBFileInfo[i].bit.fDelete != 1))				
			{
				ulNumOfElements += MOV_SUPPPROP_NUM;				
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat);
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				dwObjectSize = file_stat.fileSize;
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName);				
				xAddAllMOVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);	
			}	
			else if ( ( (ulObjectFormatCode == MTP_OFC_MP4) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_MP4) && (oMTPDBFileInfo[i].bit.fDelete != 1))
			{
				ulNumOfElements += MP4_SUPPPROP_NUM;
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat);
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				dwObjectSize = file_stat.fileSize;
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName);
				xAddAllMP4ObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);
			}	
			else if ( ( (ulObjectFormatCode == MTP_OFC_WAV) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_WAV) && (oMTPDBFileInfo[i].bit.fDelete != 1)) 				
			{
				ulNumOfElements += WAV_SUPPPROP_NUM;					
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllWAVObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);						
			}	
			else if ( ( (ulObjectFormatCode == MTP_OFC_TIFF) || (ulObjectFormatCode == 0x0000) ) &&
				(wObjectFormatCode == MTP_OFC_TIFF) && (oMTPDBFileInfo[i].bit.fDelete != 1))				
			{
				ulNumOfElements += TIFF_SUPPPROP_NUM;					
				BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat); 
				memset(cFileName,0x00,DOS_FILENAME_LEN*2);
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName); 					
				dwObjectSize = file_stat.fileSize;									
				xAddAllTIFFObjectPorpValueList(oMTPDBFileInfo[i].nFileIndex,dwObjectSize,oMTPDBFileInfo[i].nParentIndex,cFileName,&ulSize);					
			}				
		}
	}

	Addr = IN_Buffers+12;
	*Addr=(UCHAR)(ulNumOfElements&0x000000FF);
	*(Addr+1)=(UCHAR)((ulNumOfElements&0x0000FF00)>>8); 
	*(Addr+2)=(UCHAR)((ulNumOfElements&0x00FF0000)>>16);	
	*(Addr+3)=(UCHAR)((ulNumOfElements&0xFF000000)>>24);

	SendData(ulSize-12);
	SendResponseSimple(PTP_RS_OK);	
	return 0;
}


UCHAR USBMtpCmd_GetObjectInfo(void)
{
    UINT32  ObjectHandle,Code;
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;

    Code = (*CmdDS).parm1;
    ObjectHandle = ENDIAN_SWAP32(Code);

    if(USBMtpProcessObjectOpen(PTP_OP_GetObjectInfo,ObjectHandle,0,0))
    {
		BF_Debug_Print_Error(("USBMtpCmd_GetObjectInfo error ObjectHandle[0x%08x]\n", ObjectHandle));
    	return 1;
    }
    SendResponseSimple(PTP_RS_OK); 
    BF_Debug_Print_Information(("USBMtpCmd_GetObjectInfo\n"));
    return 0;
}

UCHAR USBMtpCmd_GetObject(void)
{
    UINT32  ObjectHandle;
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;

    ObjectHandle=(*CmdDS).parm1;
	ObjectHandle = ENDIAN_SWAP32(ObjectHandle);
    
    if(USBMtpProcessObjectOpen(PTP_OP_GetObject,ObjectHandle,0,0))	return 1;
//  SendResponseSimple(PTP_RS_OK);
	BF_Debug_Print_Information(("USBMtpCmd_GetObject\n"));
    return 0;
}

UCHAR USBMtpCmd_GetThumb(void)
{
    UINT32  ObjectHandle;
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;

    ObjectHandle=(*CmdDS).parm1;
	ObjectHandle = ENDIAN_SWAP32(ObjectHandle);
    
    if(USBMtpProcessObjectOpen(PTP_OP_GetThumb,ObjectHandle,0,0))	return 1;
    SendResponseSimple(PTP_RS_OK);
	BF_Debug_Print_Information(("USBMtpCmd_GetThumb\n"));
    return 0;
}

UCHAR USBMtpCmd_FormatStore(void)
{
    UINT32  StorageID;
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
	FW_INT32  ercd;
	UINT32  rtn = PTP_RS_OK;
	FW_CHAR	szDirPath[MTP_MAX_PATH];

	ercd = BF_Fs_If_Get_Cur_Drv(szDirPath);
	if (ercd != 0) {
		BF_Debug_Print_Error(("USBMtpCmd_FormatStore(): Fail to get current drive.\n"));
		rtn = PTP_RS_DeviceBusy;
	}
	else
	{
		StorageID=(*CmdDS).parm1;
		StorageID=ENDIAN_SWAP32(StorageID);

		if ((StorageID == PTP_StorageID_NF) ||	// Internal NF
			(StorageID == PTP_StorageID_SD) ||	// SD
			(StorageID == PTP_StorageID_EM))	// eMMC
		{
			ercd = BF_Fs_If_QuickFormat(szDirPath);
			if (ercd != D_OS_USER_E_OK)
			{
				BF_Debug_Print_Error(("FS_Qformat error! ercd = %d\r\n", ercd));
				if(ercd == FSDCF_ERRNO_ERR_ROFS)
				{
					// SD status is ptotected
					rtn = PTP_RS_StoreReadOnly;
				}
				else
				{
					rtn = PTP_RS_DeviceBusy;
				}
			}

			ResetMTPAllObjectInfo();
	  	}else{
			BF_Debug_Print_Error(("StorageID error!\r\n"));
			rtn = PTP_RS_InvalidStorageID;
	 	}
	}

	//DCF_Status.gLastSavedFileNo = 0;//paul 20060728
	SendResponseSimple(rtn);
	BF_Debug_Print_Information(("USBMtpCmd_FormatStore\n"));
	return 0;
}

UCHAR USBMtpCmd_GetPartialObject(void)//13=====================================
{
    UINT32  ObjectHandle,OffsetInBytes,MaxRcvNum;
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;

    ObjectHandle=(*CmdDS).parm1;
    OffsetInBytes=(*CmdDS).parm2;
    MaxRcvNum=(*CmdDS).parm3;
    
	ObjectHandle = ENDIAN_SWAP32(ObjectHandle);
	OffsetInBytes = ENDIAN_SWAP32(OffsetInBytes);
	MaxRcvNum = ENDIAN_SWAP32(MaxRcvNum);
    
    if(USBMtpProcessObjectOpen(PTP_OP_GetPartialObject,ObjectHandle,OffsetInBytes,MaxRcvNum))return 1;
    SendResponse(1,PTP_RS_OK, SendNumber ,0,0,0,0);
	BF_Debug_Print_Information(("USBMtpCmd_GetPartialObject\n"));
    return 0;
}

UCHAR USBMtpCmd_GetDeviceInfo(void)
{
    memcpy(IN_Buffers+12,&MTPDeviceInfo,sizeof(MTPDeviceInfo));
    SendData(sizeof(MTPDeviceInfo)-sizeof(MTPDeviceInfo.fill));
    SendResponseSimple(PTP_RS_OK);
	BF_Debug_Print_Information(("USBMtpCmd_GetDeviceInfo\n"));
    return 0;
}

UCHAR USBMtpCmd_GetStorageInfo(void)
{
	GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
	UINT32	storageId;
	UINT64 allSpace,freeSpace;
	unsigned long blksize, blocks, bfree;
    char     media[12];
	T_BF_FS_IF_STATVFS	media_info;
	USHORT bytePerSector=0;
	USHORT sectorPerCluster=0;
	USHORT protectStatus=0;
	UINT32 totalClusters=0;
	UINT32 freeClusters=0;
	UCHAR  wp_status;
	FJ_ERR_CODE ercd;
	FW_CHAR	szDirPath[MTP_MAX_PATH];
	INT32	fs_ercd;
	FW_INT32 fsErcd;

	fs_ercd = BF_Fs_If_Get_Cur_Drv(szDirPath);
	if (fs_ercd != 0) {
		BF_Debug_Print_Error(("USBMtpCmd_GetStorageInfo(): Fail to get current drive.\n"));
		return 0;
	}

    storageId = (*CmdDS).parm1;  
    storageId = ENDIAN_SWAP32(storageId);
	if (storageId == PTP_StorageID_SD){	//SD
		BF_Fs_If_Statvfs(szDirPath, &media_info);
		bytePerSector = 512;
		sectorPerCluster = (USHORT)(media_info.blksize / 512);
		totalClusters = (UINT32)media_info.blocks;
		freeClusters = (UINT32)media_info.bfree;
		allSpace = (UINT64)totalClusters*(UINT64)sectorPerCluster*(UINT64)bytePerSector;
		freeSpace = (UINT64)freeClusters*(UINT64)sectorPerCluster*(UINT64)bytePerSector;	
		WriteDouble(StorageInfo_SD.MaxCapacity,allSpace);
		WriteDouble(StorageInfo_SD.FreeSpaceInBytes/*+4*/,freeSpace);
		// Get write protect status of SD
		ercd = FJ_GetWriteProtectStatus(&wp_status);
		if( ercd == FJ_ERR_NG ) {
			SendResponseSimple(PTP_RS_StoreNotAvailable);
			BF_Debug_Print_Error(("USBMtpCmd_GetStorageInfo StoreNotAvailable[0x%08x]\n", storageId));
			return 0;
		}
		else {
			if( wp_status == FJ_MEDIA_WP ){
				// Protected
				protectStatus = 0x0001;
			}else{
				// Not Protected
				protectStatus = 0x0000;
			}
		}
		WriteShort(StorageInfo_SD.AccessCapability/*+2*/,protectStatus);
		memcpy(IN_Buffers+12,&StorageInfo_SD,sizeof(StorageInfo_SD)-1);
		SendData(sizeof(StorageInfo_SD)-1);		
	}else if (storageId == PTP_StorageID_NF || storageId == PTP_StorageID_EM){	//NF, eMMC
		strncat(szDirPath,":", sizeof(szDirPath)-strlen(szDirPath)-1);
		fsErcd = BF_Fs_If_Chdir(szDirPath);
		if (fsErcd < 0) {
			BF_Debug_Print_Information(("USBMtpCmd_GetStorageInfo szDirPath = %s, fsErcd = %d\n", szDirPath, fsErcd));
		}
		fsErcd = BF_Fs_If_Get_Cur_Drv(media);
		if (fsErcd < 0) {
			BF_Debug_Print_Information(("USBMtpCmd_GetStorageInfo media = %s, fsErcd = %d\n", media, fsErcd));
		}
		BF_Mdf_Wrapper_Get_Free_Space((const char*)media, &blksize, &blocks, &bfree);
		allSpace = (UINT64)blocks*(UINT64)blksize;
		freeSpace = (UINT64)bfree*(UINT64)blksize;
		WriteDouble(StorageInfo.MaxCapacity/*+4*/,allSpace);
		WriteDouble(StorageInfo.FreeSpaceInBytes/*+4*/,freeSpace);	
		memcpy(IN_Buffers+12,&StorageInfo,sizeof(StorageInfo)-1);
		SendData(sizeof(StorageInfo)-1);		
	}else if (storageId == PTP_StorageID_MS){	//MS
		BF_Fs_If_Statvfs(szDirPath, &media_info);
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
	}else{
		SendResponseSimple(PTP_RS_InvalidStorageID);
		BF_Debug_Print_Error(("USBMtpCmd_GetStorageInfo InvalidStorageID[0x%08x]\n", storageId));
		return 0;
	}
    SendResponseSimple(PTP_RS_OK);
	BF_Debug_Print_Information(("USBMtpCmd_GetStorageInfo\n"));

#ifdef CO_BF_USB_MTP_DEBUG_ALL
	ULONG ltime;
	sys_time_after_all = BF_Get_Time_us();
	ltime = sys_time_after_all - sys_time_before_all;
	printf("USB MTP ALL time=%u(us)\n",
									(FW_UINT32)ltime);
#endif


    return 0;
}

UCHAR USBMtpCmd_GetNumObjects(void)
{
    UINT32	StorageID, ObjectFormatCode;
    UINT32	NumObj=0;
    GCS_T	*CmdDS=(GCS_T *)OUT_Buffers;
    UINT32	nParentHandle=0;
    
    StorageID = (*CmdDS).parm1; 
    StorageID = ENDIAN_SWAP32(StorageID);
    
    ObjectFormatCode = (*CmdDS).parm2;	
    ObjectFormatCode = ENDIAN_SWAP32(ObjectFormatCode);
    
    nParentHandle = (*CmdDS).parm3;	
    nParentHandle = ENDIAN_SWAP32(nParentHandle);
    
    switch(ObjectFormatCode)
    {		
       	case PTP_OFC_Script://for DPS discovery 
       		PictBridgeUI_MediaSelected = E_BF_MEDIA_STATE_MEDIA_TYPE_SD1;
       		NumObj = ScriptFile_Handles(0);
       		break;
       	case PTP_OFC_None://Not Use(All Object) 		
       	case PTP_OFC_AllPicture://AllPicture		
       	default:   
//       		if (StorageID == PTP_StorageID_MS){
//       			NumObj = MTP_GetObjectHandle(E_BF_MEDIA_STATE_MEDIA_TYPE_MS, ObjectFormatCode,nParentHandle, 0);
//       		}
//       		else if (StorageID == PTP_StorageID_SD){
//       			NumObj = MTP_GetObjectHandle(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1, ObjectFormatCode,nParentHandle, 0);
//       		}
//       		else if (StorageID == PTP_StorageID_NF){
//       			NumObj = MTP_GetObjectHandle(E_BF_MEDIA_STATE_MEDIA_TYPE_NF, ObjectFormatCode,nParentHandle, 0);
//       		}
//       		else if (StorageID == 0xffffffff){
//       			NumObj = MTP_GetObjectHandle(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1, ObjectFormatCode,nParentHandle, 0);
//       		}			
			NumObj = MTP_GetObjectHandle(ObjectFormatCode,nParentHandle, 0);
       		break;
    }
    
    SendResponse(1,PTP_RS_OK, NumObj ,0,0,0,0);
    BF_Debug_Print_Information(("USBMtpCmd_GetNumObjects\n"));
    return 0;
}

UCHAR USBMtpCmd_GetObjecthandles(void)
{
    UINT32	StorageID, ObjectFormatCode;
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
    UINT32	hdlcnt=0;
    UINT32	*ptr=(UINT32 *)(IN_Buffers+12+4);
    UINT32	nParentHandle=0;
    
    StorageID = (*CmdDS).parm1; 
    StorageID = ENDIAN_SWAP32(StorageID);
    ObjectFormatCode = (*CmdDS).parm2; 
    ObjectFormatCode = ENDIAN_SWAP32(ObjectFormatCode);
    nParentHandle = (*CmdDS).parm3; 
    nParentHandle = ENDIAN_SWAP32(nParentHandle);

    switch(ObjectFormatCode)
    {
		case PTP_OFC_Script:
			hdlcnt = ScriptFile_Handles(ptr);
			break;	
		case PTP_OFC_None://Not Use(All Object)
		case PTP_OFC_AllPicture://All Picture
		default:
			//PictBridgeUI_Enable = 0;
//			if (StorageID == PTP_StorageID_SD){
//				hdlcnt = MTP_GetObjectHandle(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1, ObjectFormatCode,nParentHandle, ptr);
//			}else if (StorageID == PTP_StorageID_NF){
//				hdlcnt = MTP_GetObjectHandle(E_BF_MEDIA_STATE_MEDIA_TYPE_NF, ObjectFormatCode,nParentHandle, ptr);
//			}else if (StorageID == PTP_StorageID_MS){
//				hdlcnt = MTP_GetObjectHandle(E_BF_MEDIA_STATE_MEDIA_TYPE_MS, ObjectFormatCode,nParentHandle, ptr);				
//			}else if (StorageID == 0xffffffff){
//				hdlcnt = MTP_GetObjectHandle(E_BF_MEDIA_STATE_MEDIA_TYPE_SD1, ObjectFormatCode,nParentHandle, ptr);
//			}			
			hdlcnt = MTP_GetObjectHandle(ObjectFormatCode,nParentHandle, ptr);
			break;
    }
	
    *((UINT32 *)(IN_Buffers+12))=ENDIAN_SWAP32(hdlcnt);

    SendData(4+4*hdlcnt);
    //...... Send .....

    SendResponseSimple(PTP_RS_OK);
	 BF_Debug_Print_Information(("USBMtpCmd_GetObjecthandles\n"));
    return 0;
}

UCHAR USBMtpCmd_DeleteObject(void)
{
    INT32 			i=0;
    GCS_T   		*CmdDS=(GCS_T *)OUT_Buffers;
    ULONG			ulParam1;
    ULONG 			ulObjectHandle;		
    ULONG			ulResponseCode = PTP_RS_OK;

    FW_INT32 		Err;
    BOOL			bIsFind=FALSE;
    
    ulParam1=(*CmdDS).parm1;
    ulParam1 = ENDIAN_SWAP32(ulParam1);
    ulObjectHandle = ulParam1;
	UCHAR			wp_status;
	FJ_ERR_CODE		ercd;

	// Check protection status of SD card
	ercd = FJ_GetWriteProtectStatus(&wp_status);
	if( ercd == FJ_ERR_NG ) {
		ulResponseCode = PTP_RS_StoreNotAvailable;
	    SendResponseSimple(ulResponseCode);
		BF_Debug_Print_Error(("USBMtpCmd_DeleteObject Error 0x%04lX\n", ulResponseCode));
		return 0;
	}
	else {
		if( wp_status == FJ_MEDIA_WP ){
			// Protected
			ulResponseCode = PTP_RS_StoreReadOnly;
		    SendResponseSimple(ulResponseCode);
			BF_Debug_Print_Information(("USBMtpCmd_DeleteObject Error 0x%04lX\n", ulResponseCode));
			return 0;
		}
	}

    if (ulObjectHandle == 0xFFFFFFFF)	// Delete All Object
    {
    	for (i=g_nObjectCnt;i>0;i--)
    	{
    		if ( (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
    		{
    			Err=BF_Fs_If_Rmdir(oMTPDBFileInfo[i].szDosFileName);						
    			if (Err == 0)
    			{
    				oMTPDBFileInfo[i].bit.fDelete = 1;
    			}
    			else
    			{
					BF_Debug_Print_Information(("USBMtpCmd_DeleteObject AllObj Rmdir Err = %d, FileName = %s\n", Err, oMTPDBFileInfo[i].szDosFileName));
					ulResponseCode = PTP_RS_AccessDenied;
				}
    		}
    		else if ( (oMTPDBFileInfo[i].bit.file_attr == MTP_FILE_TYPE) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
    		{
    			Err=BF_Fs_If_Remove(oMTPDBFileInfo[i].szDosFileName);	
    			if (Err == FSDCF_ERRNO_OK)
    			{
    				oMTPDBFileInfo[i].bit.fDelete = 1;
    			}
    			else
    			{
					BF_Debug_Print_Information(("USBMtpCmd_DeleteObject AllBbj Err = %d, FileName = %s\n", Err, oMTPDBFileInfo[i].szDosFileName));
					if (Err == FSDCF_ERRNO_ERR_ROFS)
					{
						ulResponseCode = PTP_RS_ObjectWriteProtected;
					}
					else
					{
						ulResponseCode = PTP_RS_AccessDenied;
					}
				}
    		}			
    	}
    }	// end of if (ulObjectHandle == 0xFFFFFFFF)
    else  // (ulObjectHandle != 0xFFFFFFFF)     // Delete Single Object
    {
    	if (oMTPDBFileInfo[ulObjectHandle].bit.file_attr == MTP_FOLDER_TYPE)
    	{
    		for (i=g_nObjectCnt;i>0;i--)
    		{
    			xCheckParentObject(i,ulObjectHandle,&bIsFind);
    
    			if (bIsFind == TRUE)
    			{
    				if ( (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE) && (oMTPDBFileInfo[i].bit.fDelete != 1) )
    				{
    					Err=BF_Fs_If_Rmdir(oMTPDBFileInfo[i].szDosFileName);						
		    			if (Err == 0)
		    			{
		    				oMTPDBFileInfo[i].bit.fDelete = 1;
		    			}
		    			else
		    			{
							BF_Debug_Print_Information(("USBMtpCmd_DeleteObject SingleFol Rmdir Err = %d, FileName = %s\n", Err, oMTPDBFileInfo[i].szDosFileName));
							ulResponseCode = PTP_RS_AccessDenied;
						}
    				}
    				else if ( (oMTPDBFileInfo[i].bit.file_attr == MTP_FILE_TYPE) && (oMTPDBFileInfo[i].bit.fDelete != 1))
    				{
    					Err=BF_Fs_If_Remove(oMTPDBFileInfo[i].szDosFileName);
    					if (Err == FSDCF_ERRNO_OK)
    					{
    						oMTPDBFileInfo[i].bit.fDelete = 1;
    					}
    					else
    					{
							BF_Debug_Print_Information(("USBMtpCmd_DeleteObject SingleFolder Err = %d, FileName = %s\n", Err, oMTPDBFileInfo[i].szDosFileName));
							if (Err == FSDCF_ERRNO_ERR_ROFS)
							{
								ulResponseCode = PTP_RS_ObjectWriteProtected;
							}
							else
							{
								ulResponseCode = PTP_RS_AccessDenied;
							}
						}
    				}
    			}				
    		}			
    	}
    	else if (oMTPDBFileInfo[ulObjectHandle].bit.file_attr == MTP_FILE_TYPE)
    	{
    		Err=BF_Fs_If_Remove( oMTPDBFileInfo[ulObjectHandle].szDosFileName);	
    		if (Err == FSDCF_ERRNO_OK)
    		{
    			oMTPDBFileInfo[ulObjectHandle].bit.fDelete = 1;
    		}
			else
			{
				BF_Debug_Print_Information(("USBMtpCmd_DeleteObject SingleFile Err = %d, FileName = %s\n", Err, oMTPDBFileInfo[ulObjectHandle].szDosFileName));
				if (Err == FSDCF_ERRNO_ERR_ROFS)
				{
					ulResponseCode = PTP_RS_ObjectWriteProtected;
				}
				else
				{
					ulResponseCode = PTP_RS_AccessDenied;
				}
			}
    	}	
    }	// end of else
    
    SendResponseSimple(ulResponseCode);
	BF_Debug_Print_Information(("USBMtpCmd_DeleteObject ulResponseCode = 0x%04lx\n", ulResponseCode));
    return 0;
}

UCHAR USBMtpCmd_GetDevicePropDesc(void)
{
	GCS_T	*CmdDS=(GCS_T *)OUT_Buffers;
	USHORT 	DevicePropCode;
	UINT32	nSendDataSize=0;
	UINT32	nSupportedDataSize=0;
	ULONG   Param1;
	ULONG	ulDevicePropertyValue=0;
	USHORT	usDevicePropertyValue;
//	BOOL    bRet=0;
	UCHAR	ucTimeString[32];
	FW_ULONG	length = 0;
	INT32	ercd;
    UCHAR	*Addr;

	memset(ucTimeString,0x00,sizeof(ucTimeString));

	Param1 = (*CmdDS).parm1;
	Param1 = ENDIAN_SWAP32(Param1);
	DevicePropCode = (USHORT)Param1;
	   
	switch(DevicePropCode)
	{
		case PTP_DPC_ImageSize:			
			if (MTPDeviceProperty_ImageSize == 0)
			{
				memcpy(IN_Buffers+12,&MTPDevicePropImageSize,sizeof(MTPDevicePropImageSize));
				SendData(sizeof(MTPDevicePropImageSize));
			}
			else
			{
				nSupportedDataSize = (INT32)(*(MTPDevicePropImageSize.SupportedValue1Length)) * 2 + 1 +
									 (INT32)(*(MTPDevicePropImageSize.SupportedValue2Length)) * 2 + 1 +
									 (INT32)(*(MTPDevicePropImageSize.SupportedValue3Length)) * 2 + 1 +
									 (INT32)(*(MTPDevicePropImageSize.SupportedValue4Length)) * 2 + 1 + 
									 (INT32)(*(MTPDevicePropImageSize.SupportedValue5Length)) * 2 + 1 +
									 (INT32)(*(MTPDevicePropImageSize.SupportedValue6Length)) * 2 + 1;

				memcpy(IN_Buffers+12,&(MTPDevicePropImageSize),6);	//6 : sizeof(Property Code(2) + DataType(2) + Get/Set(1) + Default Value (1 : UINT8) )
				nSendDataSize = 6;	
								
				switch (MTPDeviceProperty_ImageSize)
				{
					case 1:
						memcpy(IN_Buffers+12+6,&(MTPDevicePropImageSize.SupportedValue1Length),sizeof(MTPDevicePropImageSize.SupportedValue1Length)+sizeof(MTPDevicePropImageSize.SupportedValue1));	// pgr0548
						nSendDataSize += sizeof(MTPDevicePropImageSize.SupportedValue1Length)+sizeof(MTPDevicePropImageSize.SupportedValue1);
						break;
					case 2:
						memcpy(IN_Buffers+12+6,&(MTPDevicePropImageSize.SupportedValue2Length),sizeof(MTPDevicePropImageSize.SupportedValue2Length)+sizeof(MTPDevicePropImageSize.SupportedValue2));	// pgr0548
						nSendDataSize += sizeof(MTPDevicePropImageSize.SupportedValue2Length)+sizeof(MTPDevicePropImageSize.SupportedValue2);
						break;
					case 3:
						memcpy(IN_Buffers+12+6,&(MTPDevicePropImageSize.SupportedValue3Length),sizeof(MTPDevicePropImageSize.SupportedValue3Length)+sizeof(MTPDevicePropImageSize.SupportedValue3));	// pgr0548
						nSendDataSize += sizeof(MTPDevicePropImageSize.SupportedValue3Length)+sizeof(MTPDevicePropImageSize.SupportedValue3);
						break;
					case 4:
						memcpy(IN_Buffers+12+6,&(MTPDevicePropImageSize.SupportedValue4Length),sizeof(MTPDevicePropImageSize.SupportedValue4Length)+sizeof(MTPDevicePropImageSize.SupportedValue4));	// pgr0548
						nSendDataSize += sizeof(MTPDevicePropImageSize.SupportedValue4Length)+sizeof(MTPDevicePropImageSize.SupportedValue4);
						break;
					case 5:
						memcpy(IN_Buffers+12+6,&(MTPDevicePropImageSize.SupportedValue5Length),sizeof(MTPDevicePropImageSize.SupportedValue5Length)+sizeof(MTPDevicePropImageSize.SupportedValue5));	// pgr0548
						nSendDataSize += sizeof(MTPDevicePropImageSize.SupportedValue5Length)+sizeof(MTPDevicePropImageSize.SupportedValue5);
						break;
					case 6:
						memcpy(IN_Buffers+12+6,&(MTPDevicePropImageSize.SupportedValue6Length),sizeof(MTPDevicePropImageSize.SupportedValue6Length)+sizeof(MTPDevicePropImageSize.SupportedValue6));	// pgr0548
						nSendDataSize += sizeof(MTPDevicePropImageSize.SupportedValue6Length)+sizeof(MTPDevicePropImageSize.SupportedValue6);
						break;
				}

				memcpy(IN_Buffers+12+nSendDataSize,&(MTPDevicePropImageSize.FormFlag),1+2+nSupportedDataSize);	// 1 ; sizeof (FormFlag) , 2 : sizeof(NumberOfValue)
				
				nSendDataSize += 1+2+nSupportedDataSize;
				SendData(12+nSendDataSize);				
			}										
			break;
			
		case PTP_DPC_CompressionSetting:				
			switch (MTPDeviceProperty_CompressionSetting)
			{
				case 1:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting,sizeof(DevicePropCompressionSetting));
					IN_Buffers[12+6] = *(DevicePropCompressionSetting.SupportedValue1);	// 6 : sizeof(Property Code(2) + DataType(2) + Get/Set(1) + Default Value (1 : UINT8) )
					break;
				case 2:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting,sizeof(DevicePropCompressionSetting));
					IN_Buffers[12+6] =  *(DevicePropCompressionSetting.SupportedValue2);
					break;							
				case 3:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting,sizeof(DevicePropCompressionSetting));
					IN_Buffers[12+6] =  *(DevicePropCompressionSetting.SupportedValue3);
					break;							
				case 4:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting,sizeof(DevicePropCompressionSetting));
					IN_Buffers[12+6] =  *(DevicePropCompressionSetting.SupportedValue4);
					break;
				case 5:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting,sizeof(DevicePropCompressionSetting));
					IN_Buffers[12+6] =  *(DevicePropCompressionSetting.SupportedValue5);
					break;							
				case 6:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting,sizeof(DevicePropCompressionSetting));
					IN_Buffers[12+6] =  *(DevicePropCompressionSetting.SupportedValue6);
					break;		
				case 7:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting,sizeof(DevicePropCompressionSetting));
					IN_Buffers[12+6] =  *(DevicePropCompressionSetting.SupportedValue7);
					break;
				case 8:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting,sizeof(DevicePropCompressionSetting));
					IN_Buffers[12+6] =  *(DevicePropCompressionSetting.SupportedValue8);
					break;							
				case 9:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting,sizeof(DevicePropCompressionSetting));
					IN_Buffers[12+6] =  *(DevicePropCompressionSetting.SupportedValue9);
					break;		
				case 10:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting,sizeof(DevicePropCompressionSetting));
					IN_Buffers[12+6] =  *(DevicePropCompressionSetting.SupportedValue10);
					break;
				case 11:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting,sizeof(DevicePropCompressionSetting));
					IN_Buffers[12+6] =  *(DevicePropCompressionSetting.SupportedValue11);
					break;							
				case 12:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting,sizeof(DevicePropCompressionSetting));
					IN_Buffers[12+6] =  *(DevicePropCompressionSetting.SupportedValue12);
					break;							
				case 13:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting,sizeof(DevicePropCompressionSetting));
					IN_Buffers[12+6] =  *(DevicePropCompressionSetting.SupportedValue13);
					break;
				case 14:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting,sizeof(DevicePropCompressionSetting));
					IN_Buffers[12+6] =  *(DevicePropCompressionSetting.SupportedValue14);
					break;							
				case 15:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting,sizeof(DevicePropCompressionSetting));
					IN_Buffers[12+6] =  *(DevicePropCompressionSetting.SupportedValue15);
					break;		
				case 16:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting,sizeof(DevicePropCompressionSetting));
					IN_Buffers[12+6] =  *(DevicePropCompressionSetting.SupportedValue16);
					break;
				case 17:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting,sizeof(DevicePropCompressionSetting));
					IN_Buffers[12+6] =  *(DevicePropCompressionSetting.SupportedValue17);
					break;							
				case 18:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting,sizeof(DevicePropCompressionSetting));
					IN_Buffers[12+6] =  *(DevicePropCompressionSetting.SupportedValue18);
					break;					
			}
			
			SendData(sizeof(DevicePropCompressionSetting));
			break;
		case PTP_DPC_FocalLength:
			memcpy(IN_Buffers+12,&DevicePropFocalLength,sizeof(DevicePropFocalLength));
			switch(MTPDeviceProperty_FocalLength)
			{
				case 0:
					memset(IN_Buffers+12+9,0,sizeof(UINT32));	// 9 : sizeof(Property Code(2) + DataType(2) + Get/Set(1) + Default Value (4 : UINT32) )
					break;
				case 1:	
					memcpy(IN_Buffers+12+9,&DevicePropFocalLength.SupportedValue1,sizeof(UINT32));
					break;
				case 2: 
					memcpy(IN_Buffers+12+9,&DevicePropFocalLength.SupportedValue2,sizeof(UINT32));
					break;
				case 3:	
					memcpy(IN_Buffers+12+9,&DevicePropFocalLength.SupportedValue3,sizeof(UINT32));
					break;								
				case 4: 
					memcpy(IN_Buffers+12+9,&DevicePropFocalLength.SupportedValue4,sizeof(UINT32));
					break;
				case 5: 
					memcpy(IN_Buffers+12+9,&DevicePropFocalLength.SupportedValue5,sizeof(UINT32));
					break;
				case 6: 
					memcpy(IN_Buffers+12+9,&DevicePropFocalLength.SupportedValue6,sizeof(UINT32));
					break;	
				case 7:	
					memcpy(IN_Buffers+12+9,&DevicePropFocalLength.SupportedValue7,sizeof(UINT32));
					break;
				case 8: 
					memcpy(IN_Buffers+12+9,&DevicePropFocalLength.SupportedValue8,sizeof(UINT32));
					break;
				case 9:	
					memcpy(IN_Buffers+12+9,&DevicePropFocalLength.SupportedValue9,sizeof(UINT32));
					break;	
				case 10:	
					memcpy(IN_Buffers+12+9,&DevicePropFocalLength.SupportedValue10,sizeof(UINT32));
					break;
				case 11: 
					memcpy(IN_Buffers+12+9,&DevicePropFocalLength.SupportedValue11,sizeof(UINT32));
					break;
				case 12:	
					memcpy(IN_Buffers+12+9,&DevicePropFocalLength.SupportedValue12,sizeof(UINT32));
					break;								
				case 13: 
					memcpy(IN_Buffers+12+9,&DevicePropFocalLength.SupportedValue13,sizeof(UINT32));
					break;
				case 14: 
					memcpy(IN_Buffers+12+9,&DevicePropFocalLength.SupportedValue14,sizeof(UINT32));
					break;
				case 15: 
					memcpy(IN_Buffers+12+9,&DevicePropFocalLength.SupportedValue15,sizeof(UINT32));
					break;	
				case 16:	
					memcpy(IN_Buffers+12+9,&DevicePropFocalLength.SupportedValue16,sizeof(UINT32));
					break;
				case 17: 
					memcpy(IN_Buffers+12+9,&DevicePropFocalLength.SupportedValue17,sizeof(UINT32));
					break;					
			}
			SendData(sizeof(DevicePropFocalLength));
			break;
		case PTP_DPC_FlashMode:
			memcpy(IN_Buffers+12,&DevicePropFlashMode,sizeof(DevicePropFlashMode));

			switch (MTPDeviceProperty_FlashMode)
			{
				case 0:
					memset(IN_Buffers+12+7,0,sizeof(usDevicePropertyValue));	// 7 : sizeof(Property Code(2) + DataType(2) + Get/Set(1) + Default Value (2 : UINT16) )
					break;
				case 1:	
					memcpy(IN_Buffers+12+7,&DevicePropFlashMode.SupportedValue1,sizeof(usDevicePropertyValue));	
					break;
				case 2:	
					memcpy(IN_Buffers+12+7,&DevicePropFlashMode.SupportedValue2,sizeof(usDevicePropertyValue));	
					break;
				case 3: 
					memcpy(IN_Buffers+12+7,&DevicePropFlashMode.SupportedValue3,sizeof(usDevicePropertyValue)); 
					break;
				case 4: 
					memcpy(IN_Buffers+12+7,&DevicePropFlashMode.SupportedValue4,sizeof(usDevicePropertyValue)); 
					break;				
			}				
			SendData(sizeof(DevicePropFlashMode));
			break;
		case MTP_DPC_DEVICEICON:	// 0xD405
			memcpy(IN_Buffers+12,&MTPDevicePropDeviceIcon,sizeof(MTPDevicePropDeviceIcon));
			nSendDataSize = sizeof(MTPDevicePropDeviceIcon);	// Desc
			// Icon Data Read
//			PTP_fp = BF_Fs_If_Open(FJ_CUSTOM_USB_MTP_ICON_FILEPATH, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
			if(PTP_fp != 0)
			{
				nSendDataSize = nSendDataSize + sizeof(nSendDataSize);	// Desc + Length
				// Write Length Data
				length = 0x00000000;
				Addr = IN_Buffers + 12 + sizeof(MTPDevicePropDeviceIcon);
				*Addr=(UCHAR)(length&0x000000FF);
				*(Addr+1)=(UCHAR)((length&0x0000FF00)>>8);
				*(Addr+2)=(UCHAR)((length&0x00FF0000)>>16);
				*(Addr+3)=(UCHAR)((length&0xFF000000)>>24);

				ercd = BF_Fs_If_Read(PTP_fp, (FW_VOID*)(IN_Buffers + 12 + nSendDataSize + 4), 65536 - 12 - nSendDataSize - 4, &length);
				if(ercd != 0)
				{
					// error
					BF_Debug_Print_Error(("USBMtpCmd_GetDevicePropDesc() Icon Data BF_Fs_If_Read() error!\r\n"));

					// Write Length Data 0byte
					Addr = IN_Buffers + 12 + nSendDataSize;
					length = 0x00000000;
					*Addr=(UCHAR)(length&0x000000FF);
					*(Addr+1)=(UCHAR)((length&0x0000FF00)>>8);
					*(Addr+2)=(UCHAR)((length&0x00FF0000)>>16);
					*(Addr+3)=(UCHAR)((length&0xFF000000)>>24);
					nSendDataSize = nSendDataSize + sizeof(nSendDataSize);	// Desc + Length + Length

					// Write Form Flag Data
					Addr = IN_Buffers + 12 + nSendDataSize;
					*Addr = 0x00;
					nSendDataSize = nSendDataSize + 1;						// Desc + Length + Length + Fom Flag
				}
				else
				{
					// Write Length Data
					Addr = IN_Buffers + 12 + nSendDataSize;
					*Addr=(UCHAR)(length&0x000000FF);
					*(Addr+1)=(UCHAR)((length&0x0000FF00)>>8);
					*(Addr+2)=(UCHAR)((length&0x00FF0000)>>16);
					*(Addr+3)=(UCHAR)((length&0xFF000000)>>24);
					nSendDataSize = nSendDataSize + sizeof(nSendDataSize);	// Desc + Length + Length

					nSendDataSize = nSendDataSize + length;					// Desc + Length + Length + Current Data

					// Write Form Flag Data
					Addr = IN_Buffers + 12 + nSendDataSize;
					*Addr = 0x00;
					nSendDataSize = nSendDataSize + 1;						// Desc + Length + Length + Current Data + Form Flag
				}

				// Icon Data Close
				BF_Fs_If_Close(PTP_fp);
				PTP_fp=0;
			}
			else
			{
				PTP_fp=0;	// Initialize Error Code

				// error
				BF_Debug_Print_Error(("USBMtpCmd_GetDevicePropDesc() Icon Data BF_Fs_If_Open() error!\r\n"));

				// Write Length Data 0byte
				Addr = IN_Buffers + 12 + nSendDataSize;
				length = 0x00000000;
				*Addr=(UCHAR)(length&0x000000FF);
				*(Addr+1)=(UCHAR)((length&0x0000FF00)>>8);
				*(Addr+2)=(UCHAR)((length&0x00FF0000)>>16);
				*(Addr+3)=(UCHAR)((length&0xFF000000)>>24);
				nSendDataSize = nSendDataSize + sizeof(nSendDataSize);	// Desc + Length

				// Write Length Data 0byte
				Addr = IN_Buffers + 12 + nSendDataSize;
				length = 0x00000000;
				*Addr=(UCHAR)(length&0x000000FF);
				*(Addr+1)=(UCHAR)((length&0x0000FF00)>>8);
				*(Addr+2)=(UCHAR)((length&0x00FF0000)>>16);
				*(Addr+3)=(UCHAR)((length&0xFF000000)>>24);
				nSendDataSize = nSendDataSize + sizeof(nSendDataSize);	// Desc + Length + Length

				// Write Form Flag Data
				Addr = IN_Buffers + 12 + nSendDataSize;
				*Addr = 0x00;
				nSendDataSize = nSendDataSize + 1;						// Desc + Length + Length + Form Flag

			}

			SendData(nSendDataSize);
			break;	
		case MTP_DPC_PERCEIVED:
			memcpy(IN_Buffers+12,&DevicePropPerceiveDeviceType,sizeof(DevicePropPerceiveDeviceType));
			ulDevicePropertyValue = DeviceProperty_PerceivedDeviceType;
			ulDevicePropertyValue = ENDIAN_SWAP32(ulDevicePropertyValue);
			memcpy(IN_Buffers+12+5,&ulDevicePropertyValue,sizeof(ulDevicePropertyValue));	// 5 : sizeof(Property Code(2) + DataType(2) + Get/Set(1))
			SendData(sizeof(DevicePropPerceiveDeviceType));
			break;	

#ifdef DEVICEPROP_EXTENSION
		case MTP_DPC_BATTERYLEVEL:// 0x5001
			memcpy(IN_Buffers+12,&MTPDevicePropBatteryLevel,sizeof(MTPDevicePropBatteryLevel));
//			bRet = FJ_Battery_get_CB(&DeviceProperty_BatteryLevel);
//			if(bRet != TRUE)
//			{
//				// error
//				BF_Debug_Print_Error(("USBMtpCmd_GetDevicePropDesc() FJ_Battery_get_CB() error!\r\n"));
				DeviceProperty_BatteryLevel = MTPDevicePropBatteryLevel.FactoryDefaultValue[0];
//			}
			memcpy(IN_Buffers+12+6,&DeviceProperty_BatteryLevel,sizeof(DeviceProperty_BatteryLevel));	// 6 : sizeof(Property Code(2) + DataType(2) + Get/Set(1)) + Default Value(1:UINT8)
			SendData(sizeof(MTPDevicePropBatteryLevel));
			break;	
		case MTP_DPC_WHITEBALANCE:// 0x5005
			// ...
			SendData(nSendDataSize);
			break;
		case MTP_DPC_RGBGAIN:// 0x5006
			// ...
			SendData(nSendDataSize);
			break;
		case MTP_DPC_FNUMBER:// 0x5007
			// ...
			SendData(nSendDataSize);
			break;
//		case MTP_DPC_FOCALLENGTH:// 0x5008
//			// ...
//			SendData(nSendDataSize);
//			break;			
		case MTP_DPC_FOCUSDISTANCE:// 0x5009
			// ...
			SendData(nSendDataSize);
			break;
		case MTP_DPC_FOCUSMODE:// 0x500A
			// ...
			SendData(nSendDataSize);
			break;		
		case MTP_DPC_EXPOSUREMETERINGMODE:// 0x500B
			// ...
			SendData(nSendDataSize);
			break;
//		case MTP_DPC_FLASHMODE:// 0x500C
//			// ...
//			SendData(nSendDataSize);
//			break;				
		case MTP_DPC_EXPOSURETIME:// 0x500D
			// ...
			SendData(nSendDataSize);
			break;
		case MTP_DPC_EXPOSUREPROGRAMMODE:// 0x500E
			// ...
			SendData(nSendDataSize);
			break;		
		case MTP_DPC_EXPOSUREINDEX:// 0x500F
			// ...
			SendData(nSendDataSize);
			break;
		case MTP_DPC_EXPOSRUEBIASCOMPENSATION:// 0x5010
			// ...
			SendData(nSendDataSize);
			break;					
		case MTP_DPC_DATETIME:// 0x5011
			memcpy(IN_Buffers+12,&MTPDevicePropDateTime,sizeof(MTPDevicePropDateTime));
//			bRet = FJ_Time_Get_CB(ucTimeString);
//			if(bRet != TRUE)
//			{
//				// error
//				BF_Debug_Print_Error(("USBMtpCmd_GetDevicePropDesc() FJ_Time_Get_CB() error!\r\n"));
//			}
//			else
//			{
				memcpy(&MTPDevicePropDateTime.SupportedValue2,&ucTimeString,sizeof(ucTimeString));
//			}
			memcpy(IN_Buffers+12+39,&MTPDevicePropDateTime.SupportedValue2,sizeof(MTPDevicePropDateTime.SupportedValue2));
								// 39 : sizeof(Property Code(2) + DataType(2) + Get/Set(1) + NumChars(1) + Default Value(32) + NumChars(1))
			SendData(sizeof(MTPDevicePropDateTime));
			break;
		case MTP_DPC_CAPTUREDELAY:// 0x5012
			// ...
			SendData(nSendDataSize);
			break;		
		case MTP_DPC_STILLCAPTUREMODE:// 0x5013
			// ...
			SendData(nSendDataSize);
			break;
		case MTP_DPC_CONTRAST:// 0x5014
			// ...
			SendData(nSendDataSize);
			break;
		case MTP_DPC_SHARPNESS:// 0x5015
			// ...
			SendData(nSendDataSize);
			break;
		case MTP_DPC_DIGITALZOOM:// 0x5016
			// ...
			SendData(nSendDataSize);
			break;
		case MTP_DPC_EFFECTMODE:// 0x5017
			// ...
			SendData(nSendDataSize);
			break;
		case MTP_DPC_BURSTNUMBER:// 0x5018
			// ...
			SendData(nSendDataSize);
			break;
		case MTP_DPC_BURSTINTERVAL:// 0x5019
			// ...
			SendData(nSendDataSize);
			break;
		case MTP_DPC_TIMELAPSENUMBER:// 0x501A
			// ...
			SendData(nSendDataSize);
			break;
		case MTP_DPC_TIMELAPSEINTERVAL:// 0x501B
			// ...
			SendData(nSendDataSize);
			break;
		case MTP_DPC_FOCUSMETERINGMODE:// 0x501C
			// ...
			SendData(nSendDataSize);
			break;
#endif 			
		default:
			SendResponseSimple(MTP_RS_DevicePropNotSupported);
			return 1;
	}

	SendResponseSimple(PTP_RS_OK);
	BF_Debug_Print_Information(("USBMtpCmd_GetDevicePropDesc\n"));
	return 0;
}

UCHAR USBMtpCmd_GetDevicePropValue(void)
{
	GCS_T	*CmdDS=(GCS_T *)OUT_Buffers;
	USHORT 	DevicePropCode;
	ULONG 	ulParam1;
	UCHAR	ucTimeString[32];
	BOOL	bRet=TRUE;
	UINT32	nSendDataSize = 0;
	INT32	ercd;
	FW_ULONG	length = 0;
    UCHAR	*Addr;
	
	memset(ucTimeString,0x00,sizeof(ucTimeString));

	ulParam1 = (*CmdDS).parm1;
	ulParam1 = ENDIAN_SWAP32(ulParam1);
	DevicePropCode = (USHORT)ulParam1;
		   
	switch(DevicePropCode)
	{
		case PTP_DPC_ImageSize:
		{
			switch (MTPDeviceProperty_ImageSize)
			{
				case 0:
					IN_Buffers[12] = 0x00;
					SendData(1);
					break;	
				case 1:
					memcpy(IN_Buffers+12,&MTPDevicePropImageSize.SupportedValue1Length,21);	// pgr0548
					SendData(21);
					break;
				case 2:
					memcpy(IN_Buffers+12,&MTPDevicePropImageSize.SupportedValue2Length,21);	// pgr0548
					SendData(21);
					break;
				case 3:
					memcpy(IN_Buffers+12,&MTPDevicePropImageSize.SupportedValue3Length,21);	// pgr0548
					SendData(21);
					break;
				case 4:
					memcpy(IN_Buffers+12,&MTPDevicePropImageSize.SupportedValue4Length,21);	// pgr0548
					SendData(21);
					break;
				case 5:
					memcpy(IN_Buffers+12,&MTPDevicePropImageSize.SupportedValue5Length,21);	// pgr0548
					SendData(21);
					break;
				case 6:
					memcpy(IN_Buffers+12,&MTPDevicePropImageSize.SupportedValue6Length,17);	// pgr0548
					SendData(17);
					break;
				default:
					SendResponseSimple(PTP_RS_InvalidParameter);
					break;
			}
		}

		SendResponseSimple(PTP_RS_OK);
		break;

		case PTP_DPC_CompressionSetting:
		{
			switch (MTPDeviceProperty_CompressionSetting)
			{
				case 0:
					IN_Buffers[12] = 0x00;
					SendData(1);
					break;			
				case 1:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting.SupportedValue1,sizeof(CHAR));
					break;
				case 2:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting.SupportedValue2,sizeof(CHAR));
					break;
				case 3:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting.SupportedValue3,sizeof(CHAR));
					break;
				case 4:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting.SupportedValue4,sizeof(CHAR));
					break;
				case 5:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting.SupportedValue5,sizeof(CHAR));
					break;
				case 6:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting.SupportedValue6,sizeof(CHAR));
					break;
				case 7:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting.SupportedValue7,sizeof(CHAR));
					break;
				case 8:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting.SupportedValue8,sizeof(CHAR));
					break;
				case 9:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting.SupportedValue9,sizeof(CHAR));
					break;
				case 10:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting.SupportedValue10,sizeof(CHAR));
					break;
				case 11:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting.SupportedValue11,sizeof(CHAR));
					break;
				case 12:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting.SupportedValue12,sizeof(CHAR));
					break;
				case 13:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting.SupportedValue13,sizeof(CHAR));
					break;
				case 14:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting.SupportedValue14,sizeof(CHAR));
					break;
				case 15:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting.SupportedValue15,sizeof(CHAR));
					break;
				case 16:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting.SupportedValue16,sizeof(CHAR));
					break;
				case 17:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting.SupportedValue17,sizeof(CHAR));
					break;
				case 18:
					memcpy(IN_Buffers+12,&DevicePropCompressionSetting.SupportedValue18,sizeof(CHAR));
					break;						
				default:
					SendResponseSimple(PTP_RS_InvalidParameter);
					break;
			}
		}
		SendData(sizeof(CHAR));
		SendResponseSimple(PTP_RS_OK);
		break;
		
		case PTP_DPC_FocalLength:
		{
			switch (MTPDeviceProperty_FocalLength)
			{
				case 0:
					memcpy(IN_Buffers+12,&DevicePropFocalLength.CurrentValue,sizeof(UINT32));
					break;			
				case 1:
					memcpy(IN_Buffers+12,&DevicePropFocalLength.SupportedValue1,sizeof(UINT32));
					break;
				case 2:
					memcpy(IN_Buffers+12,&DevicePropFocalLength.SupportedValue2,sizeof(UINT32));
					break;
				case 3:
					memcpy(IN_Buffers+12,&DevicePropFocalLength.SupportedValue3,sizeof(UINT32));
					break;
					
				case 4:
					memcpy(IN_Buffers+12,&DevicePropFocalLength.SupportedValue4,sizeof(UINT32));
					break;
				case 5:
					memcpy(IN_Buffers+12,&DevicePropFocalLength.SupportedValue5,sizeof(UINT32));
					break;
				case 6:
					memcpy(IN_Buffers+12,&DevicePropFocalLength.SupportedValue6,sizeof(UINT32));
					break;
				case 7:
					memcpy(IN_Buffers+12,&DevicePropFocalLength.SupportedValue7,sizeof(UINT32));
					break;
				case 8:
					memcpy(IN_Buffers+12,&DevicePropFocalLength.SupportedValue8,sizeof(UINT32));
					break;
				case 9:
					memcpy(IN_Buffers+12,&DevicePropFocalLength.SupportedValue9,sizeof(UINT32));
					break;							

				case 10:
					memcpy(IN_Buffers+12,&DevicePropFocalLength.SupportedValue10,sizeof(UINT32));
					break;
				case 11:
					memcpy(IN_Buffers+12,&DevicePropFocalLength.SupportedValue11,sizeof(UINT32));
					break;
				case 12:
					memcpy(IN_Buffers+12,&DevicePropFocalLength.SupportedValue12,sizeof(UINT32));
					break;						
				
				case 13:
					memcpy(IN_Buffers+12,&DevicePropFocalLength.SupportedValue13,sizeof(UINT32));
					break;
				case 14:
					memcpy(IN_Buffers+12,&DevicePropFocalLength.SupportedValue14,sizeof(UINT32));
					break;
				case 15:
					memcpy(IN_Buffers+12,&DevicePropFocalLength.SupportedValue15,sizeof(UINT32));
					break;

				case 16:
					memcpy(IN_Buffers+12,&DevicePropFocalLength.SupportedValue16,sizeof(UINT32));
					break;
				case 17:
					memcpy(IN_Buffers+12,&DevicePropFocalLength.SupportedValue17,sizeof(UINT32));
					break;
					
				default:
					SendResponseSimple(PTP_RS_InvalidParameter);
					break;
			}

			SendData(sizeof(UINT32));
			SendResponseSimple(PTP_RS_OK);
			break;
		}
		
		case PTP_DPC_FlashMode:
		{
			switch (MTPDeviceProperty_FlashMode)
			{
				case 0:
					memset(IN_Buffers+12,0,sizeof(USHORT));	
					break;
				case 1: 
					memcpy(IN_Buffers+12,&DevicePropFlashMode.SupportedValue1,sizeof(USHORT)); 
					break;
				case 2: 
					memcpy(IN_Buffers+12,&DevicePropFlashMode.SupportedValue2,sizeof(USHORT)); 
					break;
				case 3: 
					memcpy(IN_Buffers+12,&DevicePropFlashMode.SupportedValue3,sizeof(USHORT)); 
					break;
				case 4: 
					memcpy(IN_Buffers+12,&DevicePropFlashMode.SupportedValue4,sizeof(USHORT)); 
					break;
			}

			SendData(sizeof(USHORT));
			SendResponseSimple(PTP_RS_OK);
			break;
		}
				
		case PTP_DPC_IsoMode:
		{
			switch (MTPDeviceProperty_IsoMode)
			{
				case eISOAuto:
					memcpy(IN_Buffers+12,DevicePropIsoMode.SupportedValue1,sizeof(UCHAR));
					break;
				case eISO100:
					memcpy(IN_Buffers+12,DevicePropIsoMode.SupportedValue2,sizeof(UCHAR));					
					break;
				case eISO200:
					memcpy(IN_Buffers+12,DevicePropIsoMode.SupportedValue3,sizeof(UCHAR));
					break;
				case eISO400:
					memcpy(IN_Buffers+12,DevicePropIsoMode.SupportedValue4,sizeof(UCHAR));
					break;
				default:
					SendResponseSimple(PTP_RS_InvalidParameter);
					break;
			}

			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;
		}

#ifdef DEVICEPROP_EXTENSION	
		case MTP_DPC_BATTERYLEVEL:// 0x5001
//			bRet = FJ_Battery_get_CB(&DeviceProperty_BatteryLevel);
			if(bRet != TRUE)
			{
				// error
				BF_Debug_Print_Error(("USBMtpCmd_GetDevicePropValue() FJ_Battery_get_CB() error!\r\n"));
				DeviceProperty_BatteryLevel = MTPDevicePropBatteryLevel.FactoryDefaultValue[0];
			}
			memcpy(IN_Buffers+12,&DeviceProperty_BatteryLevel,sizeof(DeviceProperty_BatteryLevel));
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;				

		case MTP_DPC_WHITEBALANCE:// 0x5005
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;
		case MTP_DPC_RGBGAIN:// 0x5006
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;
		case MTP_DPC_FNUMBER:// 0x5007
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;
//		case MTP_DPC_FOCALLENGTH:// 0x5008
//			// ...
//			SendData(sizeof(UCHAR));
//			SendResponseSimple(PTP_RS_OK);
//			break;			
		case MTP_DPC_FOCUSDISTANCE:// 0x5009
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;
		case MTP_DPC_FOCUSMODE:// 0x500A
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;		
		case MTP_DPC_EXPOSUREMETERINGMODE:// 0x500B
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;
//		case MTP_DPC_FLASHMODE:// 0x500C
//			// ...
//			SendData(sizeof(UCHAR));
//			SendResponseSimple(PTP_RS_OK);
//			break;				
		case MTP_DPC_EXPOSURETIME:// 0x500D
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;
		case MTP_DPC_EXPOSUREPROGRAMMODE:// 0x500E
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;		
		case MTP_DPC_EXPOSUREINDEX:// 0x500F
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;
		case MTP_DPC_EXPOSRUEBIASCOMPENSATION:// 0x5010
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;					
		case MTP_DPC_DATETIME:// 0x5011
//			bRet = FJ_Time_Get_CB(ucTimeString);
			if(bRet != TRUE)
			{
				// error
				BF_Debug_Print_Error(("USBMtpCmd_GetDevicePropValue() FJ_Time_Get_CB() error!\r\n"));
			}
			else
			{
				memcpy(&MTPDevicePropDateTime.SupportedValue2,&ucTimeString,sizeof(ucTimeString));
			}
			memcpy(IN_Buffers+12,&MTPDevicePropDateTime.SupportedValue2Length,sizeof(MTPDevicePropDateTime.SupportedValue2Length));		// 1 : sizeof(NumChars(1))
			memcpy(IN_Buffers+12+1,&MTPDevicePropDateTime.SupportedValue2,sizeof(MTPDevicePropDateTime.SupportedValue2));
			SendData(sizeof(MTPDevicePropDateTime.SupportedValue2Length)+sizeof(MTPDevicePropDateTime.SupportedValue2));
			SendResponseSimple(PTP_RS_OK);
			break;
		case MTP_DPC_CAPTUREDELAY:// 0x5012
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;		
		case MTP_DPC_STILLCAPTUREMODE:// 0x5013
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;
		case MTP_DPC_CONTRAST:// 0x5014
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);			
			break;
		case MTP_DPC_SHARPNESS:// 0x5015
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;
		case MTP_DPC_DIGITALZOOM:// 0x5016
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;
		case MTP_DPC_EFFECTMODE:// 0x5017
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;
		case MTP_DPC_BURSTNUMBER:// 0x5018
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;
		case MTP_DPC_BURSTINTERVAL:// 0x5019
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;
		case MTP_DPC_TIMELAPSENUMBER:// 0x501A
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;
		case MTP_DPC_TIMELAPSEINTERVAL:// 0x501B
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;
		case MTP_DPC_FOCUSMETERINGMODE:// 0x501C
			// ...
			SendData(sizeof(UCHAR));
			SendResponseSimple(PTP_RS_OK);
			break;		
#endif 
		case MTP_DPC_DEVICEICON:	// 0xD405
		{
			// Icon Data Read
			nSendDataSize = sizeof(nSendDataSize);
//			PTP_fp = BF_Fs_If_Open(FJ_CUSTOM_USB_MTP_ICON_FILEPATH, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL);
			if(PTP_fp != 0)
			{
				ercd = BF_Fs_If_Read(PTP_fp, (FW_VOID*)(IN_Buffers + 12 + nSendDataSize), 65536 - 12 - nSendDataSize, &length);
				if(ercd != 0)
				{
					// error
					BF_Debug_Print_Error(("USBMtpCmd_GetDevicePropValue() Icon Data BF_Fs_If_Read() error!\r\n"));

					// Write Length Data 0byte
					Addr = IN_Buffers + 12;
					length = 0x00000000;
					*Addr=(UCHAR)(length&0x000000FF);
					*(Addr+1)=(UCHAR)((length&0x0000FF00)>>8);
					*(Addr+2)=(UCHAR)((length&0x00FF0000)>>16);
					*(Addr+3)=(UCHAR)((length&0xFF000000)>>24);
				}
				else
				{
					// Add Icon Data Length
					nSendDataSize = nSendDataSize + length;

					// Write Length Data
					Addr = IN_Buffers + 12;
					*Addr=(UCHAR)(length&0x000000FF);
					*(Addr+1)=(UCHAR)((length&0x0000FF00)>>8);
					*(Addr+2)=(UCHAR)((length&0x00FF0000)>>16);
					*(Addr+3)=(UCHAR)((length&0xFF000000)>>24);
				}
				// Icon Data Close
				BF_Fs_If_Close(PTP_fp);
				PTP_fp=0;
			}
			else
			{
				PTP_fp=0;	// Initialized Error Code

				// error
				BF_Debug_Print_Error(("USBMtpCmd_GetDevicePropValue() Icon Data BF_Fs_If_Open() error!\r\n"));

				// Write Length Data 0byte
				Addr = IN_Buffers + 12;
				length = 0x00000000;
				*Addr=(UCHAR)(length&0x000000FF);
				*(Addr+1)=(UCHAR)((length&0x0000FF00)>>8);
				*(Addr+2)=(UCHAR)((length&0x00FF0000)>>16);
				*(Addr+3)=(UCHAR)((length&0xFF000000)>>24);
			}

			SendData(nSendDataSize);
			SendResponseSimple(PTP_RS_OK);
			break;				
		}	
		case MTP_DPC_PERCEIVED:	// 0xD407
		{
			memcpy(IN_Buffers+12,DevicePropPerceiveDeviceType.GroupCode,sizeof(UINT32));
			SendData(sizeof(UINT32));
			SendResponseSimple(PTP_RS_OK);
			break;				
		}	
	}
	BF_Debug_Print_Information(("USBMtpCmd_GetDevicePropValue\n"));
	return 0;
}

UCHAR USBMtpCmd_SetDevicePropValue(void)
{	
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
    USHORT DevicePropCode;
    UCHAR  *Data = OUT_Buffers+12;
    UCHAR ucDevicePropertyValue;
    USHORT usDevicePropertyValue;
    ULONG Param1;
    UCHAR szDevicePropValue[10*2];
    ULONG ulResponseCode = PTP_RS_OK;
	UCHAR ucTimeString[32];
	BOOL  bRet=TRUE;
//	ULONG	ulDevicePropertyValue=0;

	memset(ucTimeString,0x00,sizeof(ucTimeString));

    Param1 = (*CmdDS).parm1;
    Param1 = ENDIAN_SWAP32(Param1);
    DevicePropCode = (USHORT)Param1;
    
    BULK_Read((UCHAR*)OUT_Buffers, 0);
       
    switch(DevicePropCode)
    {
    	case PTP_DPC_ImageSize:
    		ucDevicePropertyValue = *((UCHAR *)Data);
    		MTPDeviceProperty_ImageSize  = ucDevicePropertyValue;
    		memcpy(szDevicePropValue,(Data+1),20);
    
    		if (memcmp(szDevicePropValue,MTPDevicePropImageSize.SupportedValue1,20) == 0)
    		{
    			MTPDeviceProperty_ImageSize = 1;	
    		}
    		else if (memcmp(szDevicePropValue,MTPDevicePropImageSize.SupportedValue2,20) == 0)
    		{
    			MTPDeviceProperty_ImageSize = 2;
    		}
    		else if (memcmp(szDevicePropValue,MTPDevicePropImageSize.SupportedValue3,20) == 0)
    		{
    			MTPDeviceProperty_ImageSize = 3;
    		}
    
    		else if (memcmp(szDevicePropValue,MTPDevicePropImageSize.SupportedValue4,20) == 0)
    		{
    			MTPDeviceProperty_ImageSize = 4;
    		}
    
    		else if (memcmp(szDevicePropValue,MTPDevicePropImageSize.SupportedValue5,20) == 0)
    		{
    			MTPDeviceProperty_ImageSize = 5;
    		}
    		else if (memcmp(szDevicePropValue,MTPDevicePropImageSize.SupportedValue6,16) == 0)
    		{
    			MTPDeviceProperty_ImageSize = 6;
    		}
    		break;
    		
    	case PTP_DPC_CompressionSetting:
    		ucDevicePropertyValue = *((UCHAR *)Data);
    		MTPDeviceProperty_CompressionSetting = ucDevicePropertyValue+1;
    		break;
    		
    	case PTP_DPC_FocalLength:
    		ucDevicePropertyValue = *((UCHAR *)Data);
    		memcpy(szDevicePropValue,(Data),4);
    
    
    		if (memcmp(szDevicePropValue,DevicePropFocalLength.SupportedValue1,4) == 0)
    		{
    			MTPDeviceProperty_FocalLength = 1;	
    		}
    		else if (memcmp(szDevicePropValue,DevicePropFocalLength.SupportedValue2,4) == 0)
    		{
    			MTPDeviceProperty_FocalLength = 2;
    		}
    		else if (memcmp(szDevicePropValue,DevicePropFocalLength.SupportedValue3,4) == 0)
    		{
    			MTPDeviceProperty_FocalLength = 3;
    		}
    
    		else if (memcmp(szDevicePropValue,DevicePropFocalLength.SupportedValue4,4) == 0)
    		{
    			MTPDeviceProperty_FocalLength = 4;
    		}
    
    		else if (memcmp(szDevicePropValue,DevicePropFocalLength.SupportedValue5,4) == 0)
    		{
    			MTPDeviceProperty_FocalLength = 5;
    		}
    		else if (memcmp(szDevicePropValue,DevicePropFocalLength.SupportedValue6,4) == 0)
    		{
    			MTPDeviceProperty_FocalLength = 6;
    		}
    
    		else if (memcmp(szDevicePropValue,DevicePropFocalLength.SupportedValue7,4) == 0)
    		{
    			MTPDeviceProperty_FocalLength = 7;
    		}
    		else if (memcmp(szDevicePropValue,DevicePropFocalLength.SupportedValue8,4) == 0)
    		{
    			MTPDeviceProperty_FocalLength = 8;
    		}
    
    		else if (memcmp(szDevicePropValue,DevicePropFocalLength.SupportedValue9,4) == 0)
    		{
    			MTPDeviceProperty_FocalLength = 9;
    		}
    
    		else if (memcmp(szDevicePropValue,DevicePropFocalLength.SupportedValue10,4) == 0)
    		{
    			MTPDeviceProperty_FocalLength = 10;
    		}
    		else if (memcmp(szDevicePropValue,DevicePropFocalLength.SupportedValue11,4) == 0)
    		{
    			MTPDeviceProperty_FocalLength = 11;
    		}
    		else if (memcmp(szDevicePropValue,DevicePropFocalLength.SupportedValue12,4) == 0)
    		{
    			MTPDeviceProperty_FocalLength = 12;
    		}
    		else if (memcmp(szDevicePropValue,DevicePropFocalLength.SupportedValue13,4) == 0)
    		{
    			MTPDeviceProperty_FocalLength = 13;
    		}
    
    		else if (memcmp(szDevicePropValue,DevicePropFocalLength.SupportedValue14,4) == 0)
    		{
    			MTPDeviceProperty_FocalLength = 14;
    		}
    
    		else if (memcmp(szDevicePropValue,DevicePropFocalLength.SupportedValue15,4) == 0)
    		{
    			MTPDeviceProperty_FocalLength = 15;
    		}
    		else if (memcmp(szDevicePropValue,DevicePropFocalLength.SupportedValue16,4) == 0)
    		{
    			MTPDeviceProperty_FocalLength = 16;
    		}
    		else if (memcmp(szDevicePropValue,DevicePropFocalLength.SupportedValue17,4) == 0)
    		{
    			MTPDeviceProperty_FocalLength = 17;
    		}
    		else 
    		{
    			//BF_Debug_Print_Error(("===Set Device Prop Value is fail===\r\n"));
    		}
    		
    		break;
    		
    	case PTP_DPC_FlashMode:
    		usDevicePropertyValue = *((USHORT *)Data);
			usDevicePropertyValue = ENDIAN_SWAP16(usDevicePropertyValue);
    		MTPDeviceProperty_FlashMode=usDevicePropertyValue+1;
    		break;	
    
    #ifdef DEVICEPROP_EXTENSION
    	case MTP_DPC_BATTERYLEVEL:// 0x5001
    		// Cannot set property value.
		    ulResponseCode = PTP_RS_AccessDenied;
    		break;

    	case MTP_DPC_WHITEBALANCE:// 0x5005
    		// ...
    
    		break;
    	case MTP_DPC_RGBGAIN:// 0x5006
    		// ...
    
    		break;
    	case MTP_DPC_FNUMBER:// 0x5007
    		// ...
    
    		break;
//    	case MTP_DPC_FOCALLENGTH:// 0x5008
//  		// ...
//
//    		break;			
    	case MTP_DPC_FOCUSDISTANCE:// 0x5009
    		// ...
    
    		break;
    	case MTP_DPC_FOCUSMODE:// 0x500A
    		// ...
    
    		break;		
    	case MTP_DPC_EXPOSUREMETERINGMODE:// 0x500B
    		// ...
    
    		break;
//    	case MTP_DPC_FLASHMODE:// 0x500C
//    		// ...
//    
//    		break;				
    	case MTP_DPC_EXPOSURETIME:// 0x500D
    		// ...
    
    		break;
    	case MTP_DPC_EXPOSUREPROGRAMMODE:// 0x500E
    		// ...
    
    		break;		
    	case MTP_DPC_EXPOSUREINDEX:// 0x500F
    		// ...
    
    		break;
    	case MTP_DPC_EXPOSRUEBIASCOMPENSATION:// 0x5010
    		// ...
    
    		break;					
    	case MTP_DPC_DATETIME:	// 0x5011
    		memcpy(ucTimeString,(Data+1),sizeof(ucTimeString));
//			bRet = FJ_Time_Set_CB(ucTimeString);
			if(bRet != TRUE)
			{
				// error
				BF_Debug_Print_Error(("USBMtpCmd_SetDevicePropValue() FJ_Time_Set_CB() error!\r\n"));
			}
    		break;

    	case MTP_DPC_CAPTUREDELAY:// 0x5012
    		// ...
    
    		break;		
    	case MTP_DPC_STILLCAPTUREMODE:// 0x5013
    		// ...
    
    		break;
    	case MTP_DPC_CONTRAST:// 0x5014
    		// ...
    	
    		break;
    	case MTP_DPC_SHARPNESS:// 0x5015
    		// ...
    
    		break;
    	case MTP_DPC_DIGITALZOOM:// 0x5016
    		// ...
    
    		break;
    	case MTP_DPC_EFFECTMODE:// 0x5017
    		// ...
    
    		break;
    	case MTP_DPC_BURSTNUMBER:// 0x5018
    		// ...
    
    		break;
    	case MTP_DPC_BURSTINTERVAL:// 0x5019
    		// ...
    
    		break;
    	case MTP_DPC_TIMELAPSENUMBER:// 0x501A
    		// ...
    
    		break;
    	case MTP_DPC_TIMELAPSEINTERVAL:// 0x501B
    		// ...
    
    		break;
    	case MTP_DPC_FOCUSMETERINGMODE:// 0x501C
    		// ...
    
    		break;		
    #endif 
    	case MTP_DPC_DEVICEICON:	// 0xD405
    		// Cannot set property value.
		    ulResponseCode = PTP_RS_AccessDenied;
    		break;
    	case MTP_DPC_PERCEIVED:	// 0xD407
    		// Cannot set property value.
		    ulResponseCode = PTP_RS_AccessDenied;
    		break;
    }
    
    SendResponseSimple(ulResponseCode);
	BF_Debug_Print_Information(("USBMtpCmd_SetDevicePropValue ulResponseCode = 0x%04lX\n", ulResponseCode));
    return 0;
}

UCHAR USBMtpCmd_GetObjectPropsSupported(void)
{ 
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
    ULONG 	ulParam1;
    USHORT	usObjectFormatCode;
    
    ulParam1=(*CmdDS).parm1; 
    	ulParam1 = ENDIAN_SWAP32(ulParam1);
    usObjectFormatCode = (USHORT)ulParam1;
    
    switch (usObjectFormatCode)
    {					   
    	case MTP_OFC_AVI:	 
    		memcpy(IN_Buffers+12,&MTPFormatCodeAVISupportProp,sizeof(MTPFormatCodeAVISupportProp));	   
    		SendData(sizeof(MTPFormatCodeAVISupportProp));
    	   break;			   
    
    	case MTP_OFC_DPOF:	 
    		memcpy(IN_Buffers+12,&MTPFormatCodeDPOFSupportProp,sizeof(MTPFormatCodeDPOFSupportProp));	   
    		SendData(sizeof(MTPFormatCodeDPOFSupportProp));
    	   break;			   
    
    	case MTP_OFC_MOV:	 
    		memcpy(IN_Buffers+12,&MTPFormatCodeMOVSupportProp,sizeof(MTPFormatCodeMOVSupportProp));	   
    		SendData(sizeof(MTPFormatCodeMOVSupportProp));
    	   break;			   

    	case MTP_OFC_MP4:
    		memcpy(IN_Buffers+12,&MTPFormatCodeMP4SupportProp,sizeof(MTPFormatCodeMP4SupportProp));
    		SendData(sizeof(MTPFormatCodeMP4SupportProp));
    	   break;

    	case MTP_OFC_EXIFJPEG:	 
    		memcpy(IN_Buffers+12,&MTPFormatCodeJPEGSupportProp,sizeof(MTPFormatCodeJPEGSupportProp));		
    		SendData(sizeof(MTPFormatCodeJPEGSupportProp));
    	   break;	
    	   
    	case MTP_OFC_ASSOCIATION:
    		memcpy(IN_Buffers+12,&MTPFormatCodeAssociationSupportProp,sizeof(MTPFormatCodeAssociationSupportProp));		
    		SendData(sizeof(MTPFormatCodeAssociationSupportProp));
    	   break;
    	   
    	case MTP_OFC_WAV:	 
    		memcpy(IN_Buffers+12,&MTPFormatCodeWAVSupportProp,sizeof(MTPFormatCodeWAVSupportProp));	   
    		SendData(sizeof(MTPFormatCodeWAVSupportProp));
    	   break;	
    		   
    	case MTP_OFC_MP3:
    		memcpy(IN_Buffers+12,&MTPFormatCodeMP3SupportProp,sizeof(MTPFormatCodeMP3SupportProp));	   
    		SendData(sizeof(MTPFormatCodeMP3SupportProp));
    	   break;		
    		   
    	case MTP_OFC_BMP:
    		memcpy(IN_Buffers+12,&MTPFormatCodeBMPSupportProp,sizeof(MTPFormatCodeBMPSupportProp));	   
    		SendData(sizeof(MTPFormatCodeBMPSupportProp));
    	   break;		
    	   
    	case MTP_OFC_GIF:
    		memcpy(IN_Buffers+12,&MTPFormatCodeGIFSupportProp,sizeof(MTPFormatCodeGIFSupportProp));	   
    		SendData(sizeof(MTPFormatCodeGIFSupportProp));
    	   break;		
    	   
    	case MTP_OFC_TIFF:
    		memcpy(IN_Buffers+12,&MTPFormatCodeTIFFSupportProp,sizeof(MTPFormatCodeTIFFSupportProp));	   
    		SendData(sizeof(MTPFormatCodeTIFFSupportProp));
    	   break;		 
    		
    }
    SendResponseSimple(PTP_RS_OK);
    BF_Debug_Print_Information(("USBMtpCmd_GetObjectPropsSupported\n"));
    return 0;
}


UCHAR USBMtpCmd_GetObjectPropDesc(void)
{
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
    ULONG	ulParam1;
    ULONG	ulParam2;
    
    USHORT 	usObjectPropCode;
    USHORT	usObjectFormatCode;
    
    ulParam1=(*CmdDS).parm1;
    ulParam1 = ENDIAN_SWAP32(ulParam1);
    usObjectPropCode = (USHORT)ulParam1;
    
    ulParam2=(*CmdDS).parm2;
    ulParam2 = ENDIAN_SWAP32(ulParam2);
    usObjectFormatCode= (USHORT)ulParam2;
    
    switch (usObjectFormatCode)
    {
    	case MTP_OFC_EXIFJPEG:
    	case MTP_OFC_AVI:
    	case MTP_OFC_ASSOCIATION:
    	case MTP_OFC_WAV:
    	case MTP_OFC_MP3:
    	case MTP_OFC_BMP:
    	case MTP_OFC_GIF:
    	case MTP_OFC_TIFF:
    	case MTP_OFC_DPOF:
    	case MTP_OFC_MOV:
    	case MTP_OFC_MP4:
    		break;
    
    	default:
    		SendResponseSimple(MTP_RS_ObjectPropNotSupported);
    		return 1;
    }

/*    
    if (usObjectFormatCode == MTP_OFC_UNDEFINED)
    {
    	SendResponseSimple(PTP_RS_InvalidObjectFormatCode);
    	return 1;
    }
*/

    if (usObjectFormatCode == MTP_OFC_EXIFJPEG)
    {
    	switch (usObjectPropCode)
    	{
    		case MTP_OPC_STORAGEID:	// 0xDC01
    				/*
				if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID_MC,sizeof(MTPobjectPropertyStorageID));		 
				}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID_NF,sizeof(MTPobjectPropertyStorageID));		 
				}else{
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID,sizeof(MTPobjectPropertyStorageID));		 
				}
				*/
				memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID,sizeof(MTPobjectPropertyStorageID));
    				SendData(sizeof(MTPobjectPropertyStorageID));
    		  	break;
    	  	case MTP_OPC_OBJECTFORMAT: // 0xDC02
    		  		memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFormat,sizeof(MTPObjectPropertyObjectFormat));	   
    		  		SendData(sizeof(MTPObjectPropertyObjectFormat));
    		  	break;
    		case MTP_OPC_PROTECTIONSTATUS: // 0xDC03
    				memcpy(IN_Buffers+12,&MTPObjectPropertyProtectionStatus,sizeof(MTPObjectPropertyProtectionStatus));	   
    				SendData(sizeof(MTPObjectPropertyProtectionStatus));
    			break;
    		case MTP_OPC_OBJECTSIZE: // 0xDC04
    				memcpy(IN_Buffers+12,&MTPObjectPropertyObjectSize,sizeof(MTPObjectPropertyObjectSize));	   
    				SendData(sizeof(MTPObjectPropertyObjectSize));
    			break;	

//2009/03/31 STX tony mark +
/*
// 2009/03/26 tony +
    		case MTP_OPC_ASSOCIATIONTYPE: // 0xDC05
    				memcpy(IN_Buffers+12,&MTPObjectPropertyAssociationType,sizeof(MTPObjectPropertyAssociationType));	   
    				SendData(sizeof(MTPObjectPropertyAssociationType));
    			break;
    		case MTP_OPC_ASSOCIATIONDESC: // 0xDC06
    				memcpy(IN_Buffers+12,&MTPObjectPropertyAssociationDesc,sizeof(MTPObjectPropertyAssociationDesc));	   
    				SendData(sizeof(MTPObjectPropertyAssociationDesc));
    			break;
// 2009/03/26 tony -
*/
//2009/03/31 STX tony mark -

				
    		case MTP_OPC_OBJECTFILENAME: // 0xDC07
    				memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFileName,sizeof(MTPObjectPropertyObjectFileName));    
    				SendData(sizeof(MTPObjectPropertyObjectFileName));
    			break;

//2009/03/31 STX tony mark +
// 2009/03/26 tony +

//    		case MTP_OPC_DATECREATED: // 0xDC08
//    				memcpy(IN_Buffers+12,&MTPObjectPropertyDateCreated,sizeof(MTPObjectPropertyDateCreated));	   
//    				//SendData(sizeof(MTPObjectPropertyDateCreated));
//    				SendData(sizeof(MTPObjectPropertyDateCreated));
//    			break;

//2009/03/31 STX tony mark -				
//    		case MTP_OPC_DATEMODIFIED: // 0xDC09
//    				memcpy(IN_Buffers+12,&MTPObjectPropertyDateModified,sizeof(MTPObjectPropertyDateModified));	   
//    				SendData(sizeof(MTPObjectPropertyDateModified));
//    			break;

//2009/03/31 STX tony mark +
/*
    		case MTP_OPC_KEYWORDS: // 0xDC0A
    				memcpy(IN_Buffers+12,&MTPObjectPropertyKeywords,sizeof(MTPObjectPropertyKeywords));	   
    				SendData(sizeof(MTPObjectPropertyKeywords));
    			break;		
*/
//2009/03/31 STX tony mark -			
// 2009/03/26 tony -


				
    		case MTP_OPC_PARENTOBJECT: // 0xDC0B
    				memcpy(IN_Buffers+12,&MTPObjectPropertyParentObject,sizeof(MTPObjectPropertyParentObject));    
    				SendData(sizeof(MTPObjectPropertyParentObject));
    			break;
    		case MTP_OPC_PUOI: // 0xDC41
    				memcpy(IN_Buffers+12,&MTPObjectPropertyPUOI,sizeof(MTPObjectPropertyPUOI));    
    				SendData(sizeof(MTPObjectPropertyPUOI));
    			break;
    		case MTP_OPC_NAME: // 0xDC44
    				memcpy(IN_Buffers+12,&MTPObjectPropertyName,sizeof(MTPObjectPropertyName));    
    				SendData(sizeof(MTPObjectPropertyName));
    			break;

    		case MTP_OPC_REPSampleFormat: // 0xDC81
    				memcpy(IN_Buffers+12,&MTPObjectPropRepFormat,sizeof(MTPObjectPropRepFormat));    
    				SendData(sizeof(MTPObjectPropRepFormat));
    			break;
    		case MTP_OPC_REPSampleSize: // 0xDC82
    				memcpy(IN_Buffers+12,&MTPObjectPropRepSize,sizeof(MTPObjectPropRepSize));    
    				SendData(sizeof(MTPObjectPropRepSize));
    			break;
    		case MTP_OPC_REPSampleHeigth: // 0xDC83
    				memcpy(IN_Buffers+12,&MTPObjectPropRepHeight,sizeof(MTPObjectPropRepHeight));    
    				SendData(sizeof(MTPObjectPropRepHeight));
    			break;
    		case MTP_OPC_REPSampleWidth: // 0xDC84
    				memcpy(IN_Buffers+12,&MTPObjectPropRepWidth,sizeof(MTPObjectPropRepWidth));    
    				SendData(sizeof(MTPObjectPropRepWidth));
    			break;				
    		case MTP_OPC_REPSampleData: // 0xDC86
    				memcpy(IN_Buffers+12,&MTPObjectPropRepData,sizeof(MTPObjectPropRepData));    
    				SendData(sizeof(MTPObjectPropRepData));
    			break;				
				
    		case MTP_OPC_WIDTH: // 0xDC87
    				memcpy(IN_Buffers+12,&MTPObjectPropertyImageWidth,sizeof(MTPObjectPropertyImageWidth));	 
    				SendData(sizeof(MTPObjectPropertyImageWidth));
    			break;
    
    		case MTP_OPC_HEIGHT: // 0xDC88
    				memcpy(IN_Buffers+12,&MTPObjectPropertyImageHeight,sizeof(MTPObjectPropertyImageHeight));    
    				SendData(sizeof(MTPObjectPropertyImageHeight));
    		break;
    
    		case MTP_OPC_IMAGEBITDEPTH: // 0xDCD3
    				memcpy(IN_Buffers+12,&MTPObjectPropertyImageBitDepth,sizeof(MTPObjectPropertyImageBitDepth));	 
    				SendData(sizeof(MTPObjectPropertyImageBitDepth));
    			break;	
    			
    		default:
    			SendResponseSimple(MTP_RS_ObjectPropNotSupported);
    			return 1;
    	}
    }
    else if (usObjectFormatCode == MTP_OFC_AVI)
    {
    	switch (usObjectPropCode)
    	{
    		case MTP_OPC_STORAGEID:	// 0xDC01
    				/*
				if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID_MC,sizeof(MTPobjectPropertyStorageID));		 
				}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID_NF,sizeof(MTPobjectPropertyStorageID));		 
				}else{
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID,sizeof(MTPobjectPropertyStorageID));		 
				}	
				*/
				memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID,sizeof(MTPobjectPropertyStorageID));
    				SendData(sizeof(MTPobjectPropertyStorageID));
    		  	break;
    	  	case MTP_OPC_OBJECTFORMAT: // 0xDC02
    		  		memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFormat,sizeof(MTPObjectPropertyObjectFormat));	   
    		  		SendData(sizeof(MTPObjectPropertyObjectFormat));
    		  	break;
    		case MTP_OPC_PROTECTIONSTATUS: // 0xDC03
    				memcpy(IN_Buffers+12,&MTPObjectPropertyProtectionStatus,sizeof(MTPObjectPropertyProtectionStatus));	   
    				SendData(sizeof(MTPObjectPropertyProtectionStatus));
    			break;
    		case MTP_OPC_OBJECTSIZE: // 0xDC04
    				memcpy(IN_Buffers+12,&MTPObjectPropertyObjectSize,sizeof(MTPObjectPropertyObjectSize));	   
    				SendData(sizeof(MTPObjectPropertyObjectSize));
    			break;			
    		case MTP_OPC_OBJECTFILENAME: // 0xDC07
    				memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFileName,sizeof(MTPObjectPropertyObjectFileName));    
    				SendData(sizeof(MTPObjectPropertyObjectFileName));
    			break;

    		//case MTP_OPC_DATECREATED: // 0xDC09
    		//		memcpy(IN_Buffers+12,&MTPObjectPropertyDateCreated,sizeof(MTPObjectPropertyDateCreated));	   
    		//		SendData(sizeof(MTPObjectPropertyDateCreated));
    		//	break;				
//2009/03/31 STX tony mark -				
    		case MTP_OPC_DATEMODIFIED: // 0xDC09
    				memcpy(IN_Buffers+12,&MTPObjectPropertyDateModified,sizeof(MTPObjectPropertyDateModified));	   
    				SendData(sizeof(MTPObjectPropertyDateModified));
    			break;

//2009/03/31 STX tony mark +				
    		case MTP_OPC_PARENTOBJECT: // 0xDC0B
    				memcpy(IN_Buffers+12,&MTPObjectPropertyParentObject,sizeof(MTPObjectPropertyParentObject));    
    				SendData(sizeof(MTPObjectPropertyParentObject));
    			break;
    		case MTP_OPC_PUOI: // 0xDC41
    				memcpy(IN_Buffers+12,&MTPObjectPropertyPUOI,sizeof(MTPObjectPropertyPUOI));    
    				SendData(sizeof(MTPObjectPropertyPUOI));
    			break;
    		case MTP_OPC_NAME: // 0xDC44
    				memcpy(IN_Buffers+12,&MTPObjectPropertyName,sizeof(MTPObjectPropertyName));    
    				SendData(sizeof(MTPObjectPropertyName));
    			break;
    		case MTP_OPC_WIDTH: // 0xDC87
    				memcpy(IN_Buffers+12,&MTPObjectPropertyImageWidth,sizeof(MTPObjectPropertyImageWidth));	 
    				SendData(sizeof(MTPObjectPropertyImageWidth));
    			break;
    
    		case MTP_OPC_HEIGHT: // 0xDC88
    				memcpy(IN_Buffers+12,&MTPObjectPropertyImageHeight,sizeof(MTPObjectPropertyImageHeight));    
    				SendData(sizeof(MTPObjectPropertyImageHeight));
    		break;
    
    		case MTP_OPC_DURATION: // 0xDC89
    				memcpy(IN_Buffers+12,&MTPObjectPropertyDuration,sizeof(MTPObjectPropertyDuration));	 
    				SendData(sizeof(MTPObjectPropertyDuration));
    			break;	
    
    		case MTP_OPC_SAMPLERATE: // 0xDE93
    				memcpy(IN_Buffers+12,&MTPObjectPropertySampleRate,sizeof(MTPObjectPropertySampleRate));	 
    				SendData(sizeof(MTPObjectPropertySampleRate));
    			break;	
    
    		case MTP_OPC_NUMBEROFCHANNEL: // 0xDE94
    				memcpy(IN_Buffers+12,&MTPObjectPropertyNumberOfChannel,sizeof(MTPObjectPropertyNumberOfChannel));	 
    				SendData(sizeof(MTPObjectPropertyNumberOfChannel));
    			break;	
    
    		case MTP_OPC_SCANTYPE: // 0xDE97
    				memcpy(IN_Buffers+12,&MTPObjectPropertyScanType,sizeof(MTPObjectPropertyScanType));	 
    				SendData(sizeof(MTPObjectPropertyScanType));
    			break;
    
    		case MTP_OPC_AUDIOBITRATE: // 0xDE9A
    				memcpy(IN_Buffers+12,&MTPObjectPropertyAudioBitRate,sizeof(MTPObjectPropertyAudioBitRate));	 
    				SendData(sizeof(MTPObjectPropertyAudioBitRate));
    			break;	
    		case MTP_OPC_VIDEOFOURCCCODEC: // 0xDE9B
    				memcpy(IN_Buffers+12,&MTPObjectPropertyVideoFourCCCode,sizeof(MTPObjectPropertyVideoFourCCCode));	 
    				SendData(sizeof(MTPObjectPropertyVideoFourCCCode));
    			break;
    		case MTP_OPC_VIDEOBITRATE: // 0xDE9C
    				memcpy(IN_Buffers+12,&MTPObjectPropertyVideoBitRate,sizeof(MTPObjectPropertyVideoBitRate));	 
    				SendData(sizeof(MTPObjectPropertyVideoBitRate));
    			break;	
    		default:
    			SendResponseSimple(MTP_RS_ObjectPropNotSupported);
    			return 1;			
    	}
    }
    else if (usObjectFormatCode == MTP_OFC_WAV)
    {
    	switch (usObjectPropCode)
    	{
    		case MTP_OPC_STORAGEID:	// 0xDC01
    				/*
				if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID_MC,sizeof(MTPobjectPropertyStorageID));		 
				}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID_NF,sizeof(MTPobjectPropertyStorageID));		 
				}else{
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID,sizeof(MTPobjectPropertyStorageID));		 
				}
				*/
				memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID,sizeof(MTPobjectPropertyStorageID));
    				SendData(sizeof(MTPobjectPropertyStorageID));
    		  	break;
    	  	case MTP_OPC_OBJECTFORMAT: // 0xDC02
    		  		memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFormat,sizeof(MTPObjectPropertyObjectFormat));	   
    		  		SendData(sizeof(MTPObjectPropertyObjectFormat));
    		  	break;
    		case MTP_OPC_PROTECTIONSTATUS: // 0xDC03
    				memcpy(IN_Buffers+12,&MTPObjectPropertyProtectionStatus,sizeof(MTPObjectPropertyProtectionStatus));	   
    				SendData(sizeof(MTPObjectPropertyProtectionStatus));
    			break;
    		case MTP_OPC_OBJECTSIZE: // 0xDC04
    				memcpy(IN_Buffers+12,&MTPObjectPropertyObjectSize,sizeof(MTPObjectPropertyObjectSize));	   
    				SendData(sizeof(MTPObjectPropertyObjectSize));
    			break;			
    		case MTP_OPC_OBJECTFILENAME: // 0xDC07
    				memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFileName,sizeof(MTPObjectPropertyObjectFileName));    
    				SendData(sizeof(MTPObjectPropertyObjectFileName));
    			break;
    		case MTP_OPC_PARENTOBJECT: // 0xDC0B
    				memcpy(IN_Buffers+12,&MTPObjectPropertyParentObject,sizeof(MTPObjectPropertyParentObject));    
    				SendData(sizeof(MTPObjectPropertyParentObject));
    			break;
    		case MTP_OPC_PUOI: // 0xDC41
    				memcpy(IN_Buffers+12,&MTPObjectPropertyPUOI,sizeof(MTPObjectPropertyPUOI));    
    				SendData(sizeof(MTPObjectPropertyPUOI));
    			break;
    		case MTP_OPC_NAME: // 0xDC44
    				memcpy(IN_Buffers+12,&MTPObjectPropertyName,sizeof(MTPObjectPropertyName));    
    				SendData(sizeof(MTPObjectPropertyName));
    			break;
    
    		case MTP_OPC_DURATION: // 0xDC89
    				memcpy(IN_Buffers+12,&MTPObjectPropertyDuration,sizeof(MTPObjectPropertyDuration));	 
    				SendData(sizeof(MTPObjectPropertyDuration));
    			break;	
    
    		case MTP_OPC_SAMPLERATE: // 0xDE93
    				memcpy(IN_Buffers+12,&MTPObjectPropertySampleRate,sizeof(MTPObjectPropertySampleRate));	 
    				SendData(sizeof(MTPObjectPropertySampleRate));
    			break;	
    
    		case MTP_OPC_NUMBEROFCHANNEL: // 0xDE94
    				memcpy(IN_Buffers+12,&MTPObjectPropertyNumberOfChannel,sizeof(MTPObjectPropertyNumberOfChannel));	 
    				SendData(sizeof(MTPObjectPropertyNumberOfChannel));
    			break;	
    
    		case MTP_OPC_AUDIOBITRATE: // 0xDE9A
    				memcpy(IN_Buffers+12,&MTPObjectPropertyAudioBitRate,sizeof(MTPObjectPropertyAudioBitRate));	 
    				SendData(sizeof(MTPObjectPropertyAudioBitRate));
    			break;	
    
    		default:
    			SendResponseSimple(MTP_RS_ObjectPropNotSupported);
    			return 1;			
    	}
    }
    else if (usObjectFormatCode == MTP_OFC_MP3)
    {
    	switch (usObjectPropCode)
    	{
    		case MTP_OPC_STORAGEID:	// 0xDC01
    				/*
				if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID_MC,sizeof(MTPobjectPropertyStorageID));		 
				}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID_NF,sizeof(MTPobjectPropertyStorageID));		 
				}else{
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID,sizeof(MTPobjectPropertyStorageID));		 
				}
				*/
				memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID,sizeof(MTPobjectPropertyStorageID));
    				SendData(sizeof(MTPobjectPropertyStorageID));
    		  	break;
    	  	case MTP_OPC_OBJECTFORMAT: // 0xDC02
    		  		memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFormat,sizeof(MTPObjectPropertyObjectFormat));	   
    		  		SendData(sizeof(MTPObjectPropertyObjectFormat));
    		  	break;
    		case MTP_OPC_PROTECTIONSTATUS: // 0xDC03
    				memcpy(IN_Buffers+12,&MTPObjectPropertyProtectionStatus,sizeof(MTPObjectPropertyProtectionStatus));	   
    				SendData(sizeof(MTPObjectPropertyProtectionStatus));
    			break;
    		case MTP_OPC_OBJECTSIZE: // 0xDC04
    				memcpy(IN_Buffers+12,&MTPObjectPropertyObjectSize,sizeof(MTPObjectPropertyObjectSize));	   
    				SendData(sizeof(MTPObjectPropertyObjectSize));
    			break;			
    		case MTP_OPC_OBJECTFILENAME: // 0xDC07
    				memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFileName,sizeof(MTPObjectPropertyObjectFileName));    
    				SendData(sizeof(MTPObjectPropertyObjectFileName));
    			break;
    		case MTP_OPC_PARENTOBJECT: // 0xDC0B
    				memcpy(IN_Buffers+12,&MTPObjectPropertyParentObject,sizeof(MTPObjectPropertyParentObject));    
    				SendData(sizeof(MTPObjectPropertyParentObject));
    			break;
    		case MTP_OPC_PUOI: // 0xDC41
    				memcpy(IN_Buffers+12,&MTPObjectPropertyPUOI,sizeof(MTPObjectPropertyPUOI));    
    				SendData(sizeof(MTPObjectPropertyPUOI));
    			break;
    		case MTP_OPC_NAME: // 0xDC44
    				memcpy(IN_Buffers+12,&MTPObjectPropertyName,sizeof(MTPObjectPropertyName));    
    				SendData(sizeof(MTPObjectPropertyName));
    			break;
    
    		case MTP_OPC_DURATION: // 0xDC89
    				memcpy(IN_Buffers+12,&MTPObjectPropertyDuration,sizeof(MTPObjectPropertyDuration));	 
    				SendData(sizeof(MTPObjectPropertyDuration));
    			break;	
    
    		case MTP_OPC_SAMPLERATE: // 0xDE93
    				memcpy(IN_Buffers+12,&MTPObjectPropertySampleRate,sizeof(MTPObjectPropertySampleRate));	 
    				SendData(sizeof(MTPObjectPropertySampleRate));
    			break;	
    
    		case MTP_OPC_NUMBEROFCHANNEL: // 0xDE94
    				memcpy(IN_Buffers+12,&MTPObjectPropertyNumberOfChannel,sizeof(MTPObjectPropertyNumberOfChannel));	 
    				SendData(sizeof(MTPObjectPropertyNumberOfChannel));
    			break;	
    
    		case MTP_OPC_AUDIOBITRATE: // 0xDE9A
    				memcpy(IN_Buffers+12,&MTPObjectPropertyAudioBitRate,sizeof(MTPObjectPropertyAudioBitRate));	 
    				SendData(sizeof(MTPObjectPropertyAudioBitRate));
    			break;	
    
    		default:
    			SendResponseSimple(MTP_RS_ObjectPropNotSupported);
    			return 1;			
    	}
    }	
    else if (usObjectFormatCode == MTP_OFC_BMP)
    {
    	switch (usObjectPropCode)
    	{
    		case MTP_OPC_STORAGEID: // 0xDC01
    				/*
				if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID_MC,sizeof(MTPobjectPropertyStorageID));		 
				}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID_NF,sizeof(MTPobjectPropertyStorageID));		 
				}else{
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID,sizeof(MTPobjectPropertyStorageID));		 
				}
				*/
				memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID,sizeof(MTPobjectPropertyStorageID));
    				SendData(sizeof(MTPobjectPropertyStorageID));
    			break;
    		case MTP_OPC_OBJECTFORMAT: // 0xDC02
    				memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFormat,sizeof(MTPObjectPropertyObjectFormat));	   
    				SendData(sizeof(MTPObjectPropertyObjectFormat));
    			break;
    		case MTP_OPC_PROTECTIONSTATUS: // 0xDC03
    				memcpy(IN_Buffers+12,&MTPObjectPropertyProtectionStatus,sizeof(MTPObjectPropertyProtectionStatus));	   
    				SendData(sizeof(MTPObjectPropertyProtectionStatus));
    			break;
    		case MTP_OPC_OBJECTSIZE: // 0xDC04
    				memcpy(IN_Buffers+12,&MTPObjectPropertyObjectSize,sizeof(MTPObjectPropertyObjectSize));	   
    				SendData(sizeof(MTPObjectPropertyObjectSize));
    			break;			
    		case MTP_OPC_OBJECTFILENAME: // 0xDC07
    				memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFileName,sizeof(MTPObjectPropertyObjectFileName));	 
    				SendData(sizeof(MTPObjectPropertyObjectFileName));
    			break;
    		case MTP_OPC_PARENTOBJECT: // 0xDC0B
    				memcpy(IN_Buffers+12,&MTPObjectPropertyParentObject,sizeof(MTPObjectPropertyParentObject));	 
    				SendData(sizeof(MTPObjectPropertyParentObject));
    			break;
    		case MTP_OPC_PUOI: // 0xDC41
    				memcpy(IN_Buffers+12,&MTPObjectPropertyPUOI,sizeof(MTPObjectPropertyPUOI));	 
    				SendData(sizeof(MTPObjectPropertyPUOI));
    			break;
    		case MTP_OPC_NAME: // 0xDC44
    				memcpy(IN_Buffers+12,&MTPObjectPropertyName,sizeof(MTPObjectPropertyName));	 
    				SendData(sizeof(MTPObjectPropertyName));
    			break;
    			
    		case MTP_OPC_WIDTH: // 0xDC87
    				memcpy(IN_Buffers+12,&MTPObjectPropertyImageWidth,sizeof(MTPObjectPropertyImageWidth));	 
    				SendData(sizeof(MTPObjectPropertyImageWidth));
    			break;
    	
    		case MTP_OPC_HEIGHT: // 0xDC88
    				memcpy(IN_Buffers+12,&MTPObjectPropertyImageHeight,sizeof(MTPObjectPropertyImageHeight));    
    				SendData(sizeof(MTPObjectPropertyImageHeight));
    		break;
    	
    		case MTP_OPC_IMAGEBITDEPTH: // 0xDCD3
    				memcpy(IN_Buffers+12,&MTPObjectPropertyImageBitDepth,sizeof(MTPObjectPropertyImageBitDepth));  
    				SendData(sizeof(MTPObjectPropertyImageBitDepth));
    			break;	
    			
    		default:
    			SendResponseSimple(MTP_RS_ObjectPropNotSupported);
    			return 1;
    			
    	}
    }	
    else if (usObjectFormatCode == MTP_OFC_GIF)
    {
    	switch (usObjectPropCode)
    	{
    		case MTP_OPC_STORAGEID: // 0xDC01
    				/*
				if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID_MC,sizeof(MTPobjectPropertyStorageID));		 
				}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID_NF,sizeof(MTPobjectPropertyStorageID));		 
				}else{
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID,sizeof(MTPobjectPropertyStorageID));		 
				}
				*/
				memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID,sizeof(MTPobjectPropertyStorageID));
    				SendData(sizeof(MTPobjectPropertyStorageID));
    			break;
    		case MTP_OPC_OBJECTFORMAT: // 0xDC02
    				memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFormat,sizeof(MTPObjectPropertyObjectFormat));	   
    				SendData(sizeof(MTPObjectPropertyObjectFormat));
    			break;
    		case MTP_OPC_PROTECTIONSTATUS: // 0xDC03
    				memcpy(IN_Buffers+12,&MTPObjectPropertyProtectionStatus,sizeof(MTPObjectPropertyProtectionStatus));	   
    				SendData(sizeof(MTPObjectPropertyProtectionStatus));
    			break;
    		case MTP_OPC_OBJECTSIZE: // 0xDC04
    				memcpy(IN_Buffers+12,&MTPObjectPropertyObjectSize,sizeof(MTPObjectPropertyObjectSize));	   
    				SendData(sizeof(MTPObjectPropertyObjectSize));
    			break;			
    		case MTP_OPC_OBJECTFILENAME: // 0xDC07
    				memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFileName,sizeof(MTPObjectPropertyObjectFileName));	 
    				SendData(sizeof(MTPObjectPropertyObjectFileName));
    			break;
    		case MTP_OPC_PARENTOBJECT: // 0xDC0B
    				memcpy(IN_Buffers+12,&MTPObjectPropertyParentObject,sizeof(MTPObjectPropertyParentObject));	 
    				SendData(sizeof(MTPObjectPropertyParentObject));
    			break;
    		case MTP_OPC_PUOI: // 0xDC41
    				memcpy(IN_Buffers+12,&MTPObjectPropertyPUOI,sizeof(MTPObjectPropertyPUOI));	 
    				SendData(sizeof(MTPObjectPropertyPUOI));
    			break;
    		case MTP_OPC_NAME: // 0xDC44
    				memcpy(IN_Buffers+12,&MTPObjectPropertyName,sizeof(MTPObjectPropertyName));	 
    				SendData(sizeof(MTPObjectPropertyName));
    			break;
    			
    		case MTP_OPC_WIDTH: // 0xDC87
    				memcpy(IN_Buffers+12,&MTPObjectPropertyImageWidth,sizeof(MTPObjectPropertyImageWidth));	 
    				SendData(sizeof(MTPObjectPropertyImageWidth));
    			break;
    	
    		case MTP_OPC_HEIGHT: // 0xDC88
    				memcpy(IN_Buffers+12,&MTPObjectPropertyImageHeight,sizeof(MTPObjectPropertyImageHeight));    
    				SendData(sizeof(MTPObjectPropertyImageHeight));
    		break;
    	
    		case MTP_OPC_IMAGEBITDEPTH: // 0xDCD3
    				memcpy(IN_Buffers+12,&MTPObjectPropertyImageBitDepth,sizeof(MTPObjectPropertyImageBitDepth));  
    				SendData(sizeof(MTPObjectPropertyImageBitDepth));
    			break;	
    			
    		default:
    			SendResponseSimple(MTP_RS_ObjectPropNotSupported);
    			return 1;
    	}
    }	
    else if (usObjectFormatCode == MTP_OFC_TIFF)
    {
    	switch (usObjectPropCode)
    	{
    		case MTP_OPC_STORAGEID: // 0xDC01
    				/*
				if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID_MC,sizeof(MTPobjectPropertyStorageID));		 
				}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID_NF,sizeof(MTPobjectPropertyStorageID));		 
				}else{
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID,sizeof(MTPobjectPropertyStorageID));		 
				}
				*/
				memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID,sizeof(MTPobjectPropertyStorageID));
    				SendData(sizeof(MTPobjectPropertyStorageID));
    			break;
    		case MTP_OPC_OBJECTFORMAT: // 0xDC02
    				memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFormat,sizeof(MTPObjectPropertyObjectFormat));	   
    				SendData(sizeof(MTPObjectPropertyObjectFormat));
    			break;
    		case MTP_OPC_PROTECTIONSTATUS: // 0xDC03
    				memcpy(IN_Buffers+12,&MTPObjectPropertyProtectionStatus,sizeof(MTPObjectPropertyProtectionStatus));	   
    				SendData(sizeof(MTPObjectPropertyProtectionStatus));
    			break;
    		case MTP_OPC_OBJECTSIZE: // 0xDC04
    				memcpy(IN_Buffers+12,&MTPObjectPropertyObjectSize,sizeof(MTPObjectPropertyObjectSize));	   
    				SendData(sizeof(MTPObjectPropertyObjectSize));
    			break;			
    		case MTP_OPC_OBJECTFILENAME: // 0xDC07
    				memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFileName,sizeof(MTPObjectPropertyObjectFileName));	 
    				SendData(sizeof(MTPObjectPropertyObjectFileName));
    			break;
    		case MTP_OPC_PARENTOBJECT: // 0xDC0B
    				memcpy(IN_Buffers+12,&MTPObjectPropertyParentObject,sizeof(MTPObjectPropertyParentObject));	 
    				SendData(sizeof(MTPObjectPropertyParentObject));
    			break;
    		case MTP_OPC_PUOI: // 0xDC41
    				memcpy(IN_Buffers+12,&MTPObjectPropertyPUOI,sizeof(MTPObjectPropertyPUOI));	 
    				SendData(sizeof(MTPObjectPropertyPUOI));
    			break;
    		case MTP_OPC_NAME: // 0xDC44
    				memcpy(IN_Buffers+12,&MTPObjectPropertyName,sizeof(MTPObjectPropertyName));	 
    				SendData(sizeof(MTPObjectPropertyName));
    			break;
    			
    		case MTP_OPC_WIDTH: // 0xDC87
    				memcpy(IN_Buffers+12,&MTPObjectPropertyImageWidth,sizeof(MTPObjectPropertyImageWidth));	 
    				SendData(sizeof(MTPObjectPropertyImageWidth));
    			break;
    	
    		case MTP_OPC_HEIGHT: // 0xDC88
    				memcpy(IN_Buffers+12,&MTPObjectPropertyImageHeight,sizeof(MTPObjectPropertyImageHeight));    
    				SendData(sizeof(MTPObjectPropertyImageHeight));
    		break;
    	
    		case MTP_OPC_IMAGEBITDEPTH: // 0xDCD3
    				memcpy(IN_Buffers+12,&MTPObjectPropertyImageBitDepth,sizeof(MTPObjectPropertyImageBitDepth));  
    				SendData(sizeof(MTPObjectPropertyImageBitDepth));
    			break;	
    			
    		default:
    			SendResponseSimple(MTP_RS_ObjectPropNotSupported);
    			return 1;
    	}
    }	
    else if (usObjectFormatCode == MTP_OFC_ASSOCIATION)
    {
    	switch (usObjectPropCode)
    	{
    		case MTP_OPC_STORAGEID:	// 0xDC01
    				/*
				if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_SD1){
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID_MC,sizeof(MTPobjectPropertyStorageID));		 
				}else if (MTP_media_Type == E_BF_MEDIA_STATE_MEDIA_TYPE_NF){
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID_NF,sizeof(MTPobjectPropertyStorageID));		 
				}else{
					memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID,sizeof(MTPobjectPropertyStorageID));		 
				}
				*/
				memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID,sizeof(MTPobjectPropertyStorageID));
    				SendData(sizeof(MTPobjectPropertyStorageID));
    		  	break;
    	  	case MTP_OPC_OBJECTFORMAT: // 0xDC02
    		  		memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFormat,sizeof(MTPObjectPropertyObjectFormat));	   
    		  		SendData(sizeof(MTPObjectPropertyObjectFormat));
    		  	break;
    		case MTP_OPC_PROTECTIONSTATUS: // 0xDC03
    				memcpy(IN_Buffers+12,&MTPObjectPropertyProtectionStatus,sizeof(MTPObjectPropertyProtectionStatus));	   
    				SendData(sizeof(MTPObjectPropertyProtectionStatus));
    			break;
    		case MTP_OPC_OBJECTSIZE: // 0xDC04
    				memcpy(IN_Buffers+12,&MTPObjectPropertyObjectSize,sizeof(MTPObjectPropertyObjectSize));	   
    				SendData(sizeof(MTPObjectPropertyObjectSize));
    			break;			
    		case MTP_OPC_OBJECTFILENAME: // 0xDC07
    				memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFileName,sizeof(MTPObjectPropertyObjectFileName));    
    				SendData(sizeof(MTPObjectPropertyObjectFileName));
    			break;
    		case MTP_OPC_PARENTOBJECT: // 0xDC0B
    				memcpy(IN_Buffers+12,&MTPObjectPropertyParentObject,sizeof(MTPObjectPropertyParentObject));    
    				SendData(sizeof(MTPObjectPropertyParentObject));
    			break;
    		case MTP_OPC_PUOI: // 0xDC41
    				memcpy(IN_Buffers+12,&MTPObjectPropertyPUOI,sizeof(MTPObjectPropertyPUOI));    
    				SendData(sizeof(MTPObjectPropertyPUOI));
    			break;
    		case MTP_OPC_NAME: // 0xDC44
    				memcpy(IN_Buffers+12,&MTPObjectPropertyName,sizeof(MTPObjectPropertyName));    
    				SendData(sizeof(MTPObjectPropertyName));
    			break;
    
    		default:
    			SendResponseSimple(MTP_RS_ObjectPropNotSupported);
    			return 1;			
    	}
    
    }
	else if (usObjectFormatCode == MTP_OFC_DPOF)
	{
		switch (usObjectPropCode)
		{
			case MTP_OPC_STORAGEID:	// 0xDC01
				memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID,sizeof(MTPobjectPropertyStorageID));
					SendData(sizeof(MTPobjectPropertyStorageID));
			  	break;
		  	case MTP_OPC_OBJECTFORMAT: // 0xDC02
			  		memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFormat,sizeof(MTPObjectPropertyObjectFormat));	   
			  		SendData(sizeof(MTPObjectPropertyObjectFormat));
			  	break;
			case MTP_OPC_PROTECTIONSTATUS: // 0xDC03
					memcpy(IN_Buffers+12,&MTPObjectPropertyProtectionStatus,sizeof(MTPObjectPropertyProtectionStatus));	   
					SendData(sizeof(MTPObjectPropertyProtectionStatus));
				break;
			case MTP_OPC_OBJECTSIZE: // 0xDC04
					memcpy(IN_Buffers+12,&MTPObjectPropertyObjectSize,sizeof(MTPObjectPropertyObjectSize));	   
					SendData(sizeof(MTPObjectPropertyObjectSize));
				break;			
			case MTP_OPC_OBJECTFILENAME: // 0xDC07
					memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFileName,sizeof(MTPObjectPropertyObjectFileName));    
					SendData(sizeof(MTPObjectPropertyObjectFileName));
				break;
			case MTP_OPC_PARENTOBJECT: // 0xDC0B
					memcpy(IN_Buffers+12,&MTPObjectPropertyParentObject,sizeof(MTPObjectPropertyParentObject));    
					SendData(sizeof(MTPObjectPropertyParentObject));
				break;
			case MTP_OPC_PUOI: // 0xDC41
					memcpy(IN_Buffers+12,&MTPObjectPropertyPUOI,sizeof(MTPObjectPropertyPUOI));    
					SendData(sizeof(MTPObjectPropertyPUOI));
				break;
			case MTP_OPC_NAME: // 0xDC44
					memcpy(IN_Buffers+12,&MTPObjectPropertyName,sizeof(MTPObjectPropertyName));    
					SendData(sizeof(MTPObjectPropertyName));
				break;
			default:
				SendResponseSimple(MTP_RS_ObjectPropNotSupported);
				return 1;			
		}
	}
	else if ( (usObjectFormatCode == MTP_OFC_MOV) || (usObjectFormatCode == MTP_OFC_MP4) )
	{
		switch (usObjectPropCode)
		{
			case MTP_OPC_STORAGEID:	// 0xDC01
				memcpy(IN_Buffers+12,&MTPobjectPropertyStorageID,sizeof(MTPobjectPropertyStorageID));
					SendData(sizeof(MTPobjectPropertyStorageID));
			  	break;
		  	case MTP_OPC_OBJECTFORMAT: // 0xDC02
			  		memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFormat,sizeof(MTPObjectPropertyObjectFormat));	   
			  		SendData(sizeof(MTPObjectPropertyObjectFormat));
			  	break;
			case MTP_OPC_PROTECTIONSTATUS: // 0xDC03
					memcpy(IN_Buffers+12,&MTPObjectPropertyProtectionStatus,sizeof(MTPObjectPropertyProtectionStatus));	   
					SendData(sizeof(MTPObjectPropertyProtectionStatus));
				break;
			case MTP_OPC_OBJECTSIZE: // 0xDC04
					memcpy(IN_Buffers+12,&MTPObjectPropertyObjectSize,sizeof(MTPObjectPropertyObjectSize));	   
					SendData(sizeof(MTPObjectPropertyObjectSize));
				break;			
			case MTP_OPC_OBJECTFILENAME: // 0xDC07
					memcpy(IN_Buffers+12,&MTPObjectPropertyObjectFileName,sizeof(MTPObjectPropertyObjectFileName));    
					SendData(sizeof(MTPObjectPropertyObjectFileName));
				break;
			case MTP_OPC_PARENTOBJECT: // 0xDC0B
					memcpy(IN_Buffers+12,&MTPObjectPropertyParentObject,sizeof(MTPObjectPropertyParentObject));    
					SendData(sizeof(MTPObjectPropertyParentObject));
				break;
			case MTP_OPC_PUOI: // 0xDC41
					memcpy(IN_Buffers+12,&MTPObjectPropertyPUOI,sizeof(MTPObjectPropertyPUOI));    
					SendData(sizeof(MTPObjectPropertyPUOI));
				break;
			case MTP_OPC_NAME: // 0xDC44
					memcpy(IN_Buffers+12,&MTPObjectPropertyName,sizeof(MTPObjectPropertyName));    
					SendData(sizeof(MTPObjectPropertyName));
				break;

			case MTP_OPC_REPSampleFormat: // 0xDC81
					memcpy(IN_Buffers+12,&MTPObjectPropRepFormat,sizeof(MTPObjectPropRepFormat));	 
					SendData(sizeof(MTPObjectPropRepFormat));
				break;

			case MTP_OPC_REPSampleSize: // 0xDC82
					memcpy(IN_Buffers+12,&MTPObjectPropRepSize,sizeof(MTPObjectPropRepSize));	 
					SendData(sizeof(MTPObjectPropRepSize));
				break;

			case MTP_OPC_REPSampleHeigth: // 0xDC83
					memcpy(IN_Buffers+12,&MTPObjectPropRepHeight,sizeof(MTPObjectPropRepHeight));	 
					SendData(sizeof(MTPObjectPropRepHeight));
				break;

			case MTP_OPC_REPSampleWidth: // 0xDC84
					memcpy(IN_Buffers+12,&MTPObjectPropRepWidth,sizeof(MTPObjectPropRepWidth));    
					SendData(sizeof(MTPObjectPropRepWidth));
				break;

			case MTP_OPC_REPSampleData: // 0xDC86
					memcpy(IN_Buffers+12,&MTPObjectPropRepData,sizeof(MTPObjectPropRepData));	 
					SendData(sizeof(MTPObjectPropRepData));
				break;

			case MTP_OPC_WIDTH: // 0xDC87
					memcpy(IN_Buffers+12,&MTPObjectPropertyImageWidth,sizeof(MTPObjectPropertyImageWidth));	 
					SendData(sizeof(MTPObjectPropertyImageWidth));
				break;
	
			case MTP_OPC_HEIGHT: // 0xDC88
					memcpy(IN_Buffers+12,&MTPObjectPropertyImageHeight,sizeof(MTPObjectPropertyImageHeight));	 
					SendData(sizeof(MTPObjectPropertyImageHeight));
			break;
	
			case MTP_OPC_DURATION: // 0xDC89
					memcpy(IN_Buffers+12,&MTPObjectPropertyDuration,sizeof(MTPObjectPropertyDuration));	 
					SendData(sizeof(MTPObjectPropertyDuration));
				break;	
	
			case MTP_OPC_SAMPLERATE: // 0xDE93
					memcpy(IN_Buffers+12,&MTPObjectPropertySampleRate,sizeof(MTPObjectPropertySampleRate));	 
					SendData(sizeof(MTPObjectPropertySampleRate));
				break;	
	
			case MTP_OPC_NUMBEROFCHANNEL: // 0xDE94
					memcpy(IN_Buffers+12,&MTPObjectPropertyNumberOfChannel,sizeof(MTPObjectPropertyNumberOfChannel));	 
					SendData(sizeof(MTPObjectPropertyNumberOfChannel));
				break;	
			case MTP_OPC_AUDIOBITRATE: // 0xDE9A
					memcpy(IN_Buffers+12,&MTPObjectPropertyAudioBitRate,sizeof(MTPObjectPropertyAudioBitRate));	 
					SendData(sizeof(MTPObjectPropertyAudioBitRate));
				break;	
			default:
				SendResponseSimple(MTP_RS_ObjectPropNotSupported);
				return 1;			
		}
	}


    SendResponseSimple(PTP_RS_OK);
	BF_Debug_Print_Information(("USBMtpCmd_GetObjectPropDesc\n"));
    return 0;
}

UCHAR USBMtpCmd_GetObjectPropValue(void)
{
    UINT32			i=0;
    GCS_T   		*CmdDS=(GCS_T *)OUT_Buffers;
    ULONG 			ulParam1;
    ULONG 			ulParam2;
    USHORT 			usObjectHandle;	
    USHORT			usObjectPropCode;	
    WORD			wObjectFormatCode = 0;
    T_BF_FS_IF_STAT	file_stat;
    DWORD 			dwObjectSize;
    FW_CHAR 		cFileName[DOS_FILENAME_LEN*2];
    FW_CHAR 		szTempString[MTP_MAX_PATH];
    UINT32 			nParentObject=0;	
    
    ulParam1=(*CmdDS).parm1;
    ulParam1 = ENDIAN_SWAP32(ulParam1);
    usObjectHandle = (USHORT)ulParam1;
    
    ulParam2=(*CmdDS).parm2;
    ulParam2 = ENDIAN_SWAP32(ulParam2);	
    usObjectPropCode = (USHORT)ulParam2;
    
    for (i=1;i<=g_nObjectCnt;i++)
    {
    	if (oMTPDBFileInfo[i].nFileIndex == usObjectHandle)
    	{			
    		if (oMTPDBFileInfo[i].bit.fDelete == 1)
    		{
    			SendResponseSimple(PTP_RS_InvalidObjectHandle);
    			return 1;
    		}
    		
    		if (oMTPDBFileInfo[i].bit.file_attr== MTP_FOLDER_TYPE) 
    		{
    			wObjectFormatCode = MTP_OFC_ASSOCIATION;
    			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
    			xGetPathName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);						
    		}
    		else if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_MP3)
    		{
    			wObjectFormatCode = MTP_OFC_MP3;
    			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);							
    		}
    		else if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_BMP)
    		{
    			wObjectFormatCode = MTP_OFC_BMP;
    			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);						 
    		}
    		else if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_GIF)
    		{
    			wObjectFormatCode = MTP_OFC_GIF;
    			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);						
    		}				
    		else if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_JPG)
    		{
    			wObjectFormatCode = MTP_OFC_EXIFJPEG;
    			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);						
    		}
    		else if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_AVI)
    		{
    			wObjectFormatCode = MTP_OFC_AVI;
    			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);						
    		}
    		else if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_MOV)
    		{
    			wObjectFormatCode = MTP_OFC_MOV;
    			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);						
    		}
			else if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_MP4)
			{
				wObjectFormatCode = MTP_OFC_MP4;
				xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
				ASCIIToUniCode(szTempString,cFileName);
			}
    		else if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_DPOF)
    		{
    			wObjectFormatCode = MTP_OFC_DPOF;
    			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);						
    		}
       		else if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_WAV)
    		{
    			wObjectFormatCode = MTP_OFC_WAV;
    			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);							
    		}
    		else if (oMTPDBFileInfo[i].bit.file_exttype == MTP_FILE_TYPE_TIFF)
    		{
    			wObjectFormatCode = MTP_OFC_TIFF;
    			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);							
    		}					
    		
    		if (oMTPDBFileInfo[i].bit.file_attr == MTP_FOLDER_TYPE)
    		{
    			dwObjectSize=0;
    			nParentObject = oMTPDBFileInfo[i].nParentIndex;
    			memset(cFileName,0x00,DOS_FILENAME_LEN*2);
    			xGetPathName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);								
    			xAddSendSingleAssociationObjectPorpValueList(usObjectHandle,usObjectPropCode,nParentObject,dwObjectSize,cFileName);
    		}
    
    		if (wObjectFormatCode == MTP_OFC_MP3)
    		{	
    			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat);
    			memset(cFileName,0x00,DOS_FILENAME_LEN*2);	
    			dwObjectSize = file_stat.fileSize;	
    			nParentObject = oMTPDBFileInfo[i].nParentIndex;
    			wObjectFormatCode = MTP_OFC_MP3;
    			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);					
    			xAddSendSingleMP3ObjectPorpValueList(usObjectHandle,usObjectPropCode,nParentObject,dwObjectSize,cFileName);				
    		}
    
    		if (wObjectFormatCode == MTP_OFC_BMP)
    		{	
    			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat);					
    			memset(cFileName,0x00,DOS_FILENAME_LEN*2);	
    			dwObjectSize = file_stat.fileSize;	
    			nParentObject = oMTPDBFileInfo[i].nParentIndex;
    			wObjectFormatCode = MTP_OFC_BMP;
    			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);						
    			xAddSendSingleBMPObjectPorpValueList(usObjectHandle,usObjectPropCode,nParentObject,dwObjectSize,cFileName);
    		}
    
    		if (wObjectFormatCode == MTP_OFC_GIF)
    		{	
    			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat);					
    			memset(cFileName,0x00,DOS_FILENAME_LEN*2);	
    			dwObjectSize = file_stat.fileSize;	
    			nParentObject = oMTPDBFileInfo[i].nParentIndex;
    			wObjectFormatCode = MTP_OFC_GIF;
    			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);						
    			xAddSendSingleGIFObjectPorpValueList(usObjectHandle,usObjectPropCode,nParentObject,dwObjectSize,cFileName);
    		}
    
    		if (wObjectFormatCode == MTP_OFC_EXIFJPEG)
    		{	
    			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat);
    			memset(cFileName,0x00,DOS_FILENAME_LEN*2);	
    			dwObjectSize = file_stat.fileSize;	
    			nParentObject = oMTPDBFileInfo[i].nParentIndex;
    			wObjectFormatCode = MTP_OFC_EXIFJPEG;
    			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);						
    			xAddSendSingleJPGObjectPorpValueList(usObjectHandle,usObjectPropCode,nParentObject,dwObjectSize,cFileName);
    		}
    
    		if (wObjectFormatCode == MTP_OFC_AVI)
    		{	
    			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat);
    			memset(cFileName,0x00,DOS_FILENAME_LEN*2);	
    			dwObjectSize = file_stat.fileSize;	
    			nParentObject = oMTPDBFileInfo[i].nParentIndex;
    			wObjectFormatCode = MTP_OFC_AVI;
    			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);						
    			xAddSendSingleAVIObjectPorpValueList(usObjectHandle,usObjectPropCode,nParentObject,dwObjectSize,cFileName);
    		}

    		if (wObjectFormatCode == MTP_OFC_DPOF)
    		{	
    			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat);
    			memset(cFileName,0x00,DOS_FILENAME_LEN*2);	
    			dwObjectSize = file_stat.fileSize;	
    			nParentObject = oMTPDBFileInfo[i].nParentIndex;
    			wObjectFormatCode = MTP_OFC_DPOF;
    			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);						
    			xAddSendSingleDPOFObjectPorpValueList(usObjectHandle,usObjectPropCode,nParentObject,dwObjectSize,cFileName);
    		}

    		if (wObjectFormatCode == MTP_OFC_MOV)
    		{	
    			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat);
    			memset(cFileName,0x00,DOS_FILENAME_LEN*2);	
    			dwObjectSize = file_stat.fileSize;	
    			nParentObject = oMTPDBFileInfo[i].nParentIndex;
    			wObjectFormatCode = MTP_OFC_MOV;
    			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);						
    			xAddSendSingleMOVObjectPorpValueList(usObjectHandle,usObjectPropCode,nParentObject,dwObjectSize,cFileName);
    		}
    
    		if (wObjectFormatCode == MTP_OFC_MP4)
    		{
    			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat);
    			memset(cFileName,0x00,DOS_FILENAME_LEN*2);	
    			dwObjectSize = file_stat.fileSize;	
    			nParentObject = oMTPDBFileInfo[i].nParentIndex;
    			wObjectFormatCode = MTP_OFC_MP4;
    			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);						
    			xAddSendSingleMP4ObjectPorpValueList(usObjectHandle,usObjectPropCode,nParentObject,dwObjectSize,cFileName);
    		}

    		if (wObjectFormatCode == MTP_OFC_WAV)
    		{	
    			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat);
    			memset(cFileName,0x00,DOS_FILENAME_LEN*2);	
    			dwObjectSize = file_stat.fileSize;	
    			nParentObject = oMTPDBFileInfo[i].nParentIndex;
    			wObjectFormatCode = MTP_OFC_WAV;
    			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);						
    			xAddSendSingleWAVObjectPorpValueList(usObjectHandle,usObjectPropCode,nParentObject,dwObjectSize,cFileName);
    		}
    
    		if (wObjectFormatCode == MTP_OFC_TIFF)
    		{	
    			BF_Fs_If_Stat((const char*)oMTPDBFileInfo[i].szDosFileName,(T_BF_FS_IF_STAT*)&file_stat);
    			memset(cFileName,0x00,DOS_FILENAME_LEN*2);	
    			dwObjectSize = file_stat.fileSize;	
    			nParentObject = oMTPDBFileInfo[i].nParentIndex;
    			wObjectFormatCode = MTP_OFC_TIFF;
    			xGetFileName(oMTPDBFileInfo[i].szDosFileName,szTempString);
    			ASCIIToUniCode(szTempString,cFileName);						
    			xAddSendSingleTIFFObjectPorpValueList(usObjectHandle,usObjectPropCode,nParentObject,dwObjectSize,cFileName);
    		}		
    	}
    }
    
    SendResponseSimple(PTP_RS_OK);	
	BF_Debug_Print_Information(("USBMtpCmd_GetObjectPropValue\n"));
    return 0;
}

UCHAR USBMtpCmd_GetObjectPropList(void)
{
    GCS_T   *CmdDS=(GCS_T *)OUT_Buffers;
    ULONG  	ulObjectHandle,ulObjectFormatCode,ulObjectPropCode;
    ULONG  	ulObjectPropGroupCode,ulDepth;		
    	
    ulObjectHandle=(*CmdDS).parm1;
    ulObjectHandle = ENDIAN_SWAP32(ulObjectHandle);	
    
    ulObjectFormatCode = (*CmdDS).parm2;
    ulObjectFormatCode = ENDIAN_SWAP32(ulObjectFormatCode);
    
    ulObjectPropCode = (*CmdDS).parm3;
    ulObjectPropCode = ENDIAN_SWAP32(ulObjectPropCode);
    
    ulObjectPropGroupCode = (*CmdDS).parm4;
    ulObjectPropGroupCode = ENDIAN_SWAP32(ulObjectPropGroupCode);
    
    ulDepth = (*CmdDS).parm5;
    ulDepth = ENDIAN_SWAP32(ulDepth);

	BF_Debug_Print_Information(("USBMtpCmd_GetObjectPropList Start\n"));
	BF_Debug_Print_Information(("ulObjectHandle = 0x%08lX, ulObjectFormatCode = 0x%08lX\n", ulObjectHandle, ulObjectFormatCode));
	BF_Debug_Print_Information(("ulObjectPropCode = 0x%08lX, ulObjectPropGroupCode = 0x%08lX\n", ulObjectPropCode, ulObjectPropGroupCode));
	BF_Debug_Print_Information(("ulDepth = 0x%08lX\n", ulDepth));

    if ( (ulObjectHandle == 0x00000000) && (ulDepth == 0x00000000) )
    {
    	memset(IN_Buffers+12,0,sizeof(ULONG));
    	SendData(sizeof(ULONG));
    	SendResponseSimple(PTP_RS_OK);
		BF_Debug_Print_Information(("USBMtpCmd_GetObjectPropList\n"));
    	return 0;
    }
    
    if ( (ulObjectPropCode == 0x00000000) && (ulObjectPropGroupCode == 0x00000000))
    {
    	SendResponseSimple(PTP_RS_ParameterNotSupported);
		BF_Debug_Print_Information(("USBMtpCmd_GetObjectPropList\n"));
    	return 1;	
    }
	
    if ( (ulDepth != 0x00000000) && (ulDepth != 0xFFFFFFFF))
    {    
		if ( (ulObjectHandle != 0x00000000) && (ulObjectHandle != 0xFFFFFFFF) && (ulObjectPropCode == 0xFFFFFFFF)&& ( (ulObjectPropCode == 0xFFFFFFFF) || (ulObjectPropCode == 0x00000000) ) )
    	{	
			xAddOneHandleAllPropertyByDepth(ulObjectHandle,ulObjectFormatCode,ulObjectPropCode,ulObjectPropGroupCode,ulDepth);
			BF_Debug_Print_Information(("USBMtpCmd_GetObjectPropList\n"));
			return 0;
    	}    
    	else if ( ( (ulObjectHandle== 0x00000000) || (ulObjectHandle== 0xFFFFFFFF))  && (ulObjectPropCode == 0xFFFFFFFF) )
    	{
    		xAddAllHandleAllPropertyByDepth(ulObjectHandle,ulObjectFormatCode,ulObjectPropCode,ulObjectPropGroupCode,ulDepth);
			BF_Debug_Print_Information(("USBMtpCmd_GetObjectPropList\n"));
    		return 0;
    	}
    
    	else if ( (ulObjectHandle != 0x00000000) && (ulObjectPropCode!= 0x00000000) && (ulObjectPropCode!= 0xFFFFFFFF) )
    	{
    		xAddOneHandleOnePropertyByDepth(ulObjectHandle,ulObjectFormatCode,ulObjectPropCode,ulObjectPropGroupCode,ulDepth);
			BF_Debug_Print_Information(("USBMtpCmd_GetObjectPropList\n"));
    		return 0;
    	}
    
    	else if ( (ulObjectHandle == 0x00000000) && (ulObjectPropCode!= 0x00000000) && (ulObjectPropCode!= 0xFFFFFFFF) )
    	{
    		xAddAllHandleOnePropertyByDepth(ulObjectHandle,ulObjectFormatCode,ulObjectPropCode,ulObjectPropGroupCode,ulDepth);
			BF_Debug_Print_Information(("USBMtpCmd_GetObjectPropList\n"));
    		return 0;
    	}	
		else
		{
    		SendResponseSimple(MTP_RS_Specification_By_Depth_Unsupported);
			BF_Debug_Print_Information(("USBMtpCmd_GetObjectPropList\n"));
	    	return 1;
		}
    }
    
    if ((ulDepth == 0xFFFFFFFF) &&(ulObjectHandle!=0x00000000) &&(ulObjectHandle!=0xFFFFFFFF))
    {
    	
    	if ( (ulObjectPropCode == 0xFFFFFFFF) || (ulObjectPropCode == 0x00000000) ) // get all object prop code for one handle
    	{
    		
    		xAddAllHandleAllPropertyByFolder(ulObjectHandle,ulObjectFormatCode,ulObjectPropCode,ulObjectPropGroupCode,ulDepth);	
			BF_Debug_Print_Information(("USBMtpCmd_GetObjectPropList [get all object prop code for one handle]\n"));
    		return 0;
    	}
    	//  one object prop code for one object handle
    	else if ( (ulObjectPropCode!= 0x00000000) && (ulObjectPropCode!= 0xFFFFFFFF) ) // get one object porp code
    	{
    		xAddAllHandleOnePropertyByFolder(ulObjectHandle,ulObjectFormatCode,ulObjectPropCode,ulObjectPropGroupCode,ulDepth);
			BF_Debug_Print_Information(("USBMtpCmd_GetObjectPropList [get one object prop code]\n"));
    		return 0;			
    	}
    }
    
	// for Group
	if ( (ulObjectHandle != 0x00000000) && (ulObjectHandle != 0xFFFFFFFF) && (ulObjectPropGroupCode !=0) )
	{	
		if ( (ulObjectPropCode == 0xFFFFFFFF) || (ulObjectPropCode == 0x00000000) ) // get all object prop code for one handle
		{

			xAddOneHandleAllPropertyByGroup(ulObjectHandle,ulObjectFormatCode,ulObjectPropCode,ulObjectPropGroupCode,ulDepth);
			BF_Debug_Print_Information(("USBMtpCmd_GetObjectPropList[for Group]\n"));
			return 0;			
		}
	}
	
	if ( (ulObjectHandle == 0xFFFFFFFF) && (ulObjectFormatCode == 0x0000000000) && (ulObjectPropCode == 0x00000000) && (ulObjectPropGroupCode  != 0x00000000) && (ulDepth == 0x00000000))
	{

		xAddAllHandleAllPropertyByGroup(ulObjectHandle,ulObjectFormatCode,ulObjectPropCode,ulObjectPropGroupCode,ulDepth);
		BF_Debug_Print_Information(("USBMtpCmd_GetObjectPropList\n"));
		return 0;		
	}
	
    // for get all object all propcode
    if ( ((ulObjectHandle== 0x00000000) && (ulObjectPropCode == 0xFFFFFFFF) && ( ulDepth == 0xFFFFFFFF)) ||
    	 ((ulObjectHandle== 0xFFFFFFFF) && (ulObjectPropCode == 0xFFFFFFFF) && ( ulDepth == 0x00000000))	||
    	 ((ulObjectHandle== 0xFFFFFFFF) && (ulObjectPropCode == 0x00000000) && ( ulDepth == 0x00000000)) )		 
    {
    
    	xAddAllHandleAllProperty(ulObjectHandle,ulObjectFormatCode,ulObjectPropCode,ulObjectPropGroupCode,ulDepth);
		BF_Debug_Print_Information(("USBMtpCmd_GetObjectPropList\n"));
    	return 0;
    }	
    else if ( (ulObjectHandle == 0x00000000) && (ulObjectPropCode != 0x00000000))
    {		
    	xAddAllHandleOneProperty(ulObjectHandle,ulObjectFormatCode,ulObjectPropCode,ulObjectPropGroupCode,ulDepth);
		BF_Debug_Print_Information(("USBMtpCmd_GetObjectPropList\n"));
    	return 0;
    }    
    else if ( (ulObjectHandle != 0x00000000) && (ulObjectHandle != 0xFFFFFFFF))
    {     
    
    	if ( (ulObjectPropCode == 0xFFFFFFFF) || (ulObjectPropCode == 0x00000000) ) // get all object prop code for one handle
    	{
    		xAddOneHandleAllProperty(ulObjectHandle,ulObjectFormatCode,ulObjectPropCode,ulObjectPropGroupCode,ulDepth);	
			BF_Debug_Print_Information(("USBMtpCmd_GetObjectPropList\n"));
    		return 0;
    	}
    	//  one object prop code for one object handle
    	else if ( (ulObjectPropCode!= 0x00000000) && (ulObjectPropCode!= 0xFFFFFFFF) ) // get one object porp code
    	{
    		xAddOneHandleOneProperty(ulObjectHandle,ulObjectFormatCode,ulObjectPropCode,ulObjectPropGroupCode,ulDepth);
    	}
    }	
	BF_Debug_Print_Information(("USBMtpCmd_GetObjectPropList\n"));
    return 0;
	
}

UCHAR USBMtpCmd_SendObjectInfo(void)
{
	UINT32		ObjHdl;
	GCS_T   	*CmdDS=(GCS_T *)OUT_Buffers;
	UINT32		StorageID;
	UINT32		ParObjHdl;
	UINT32		allSize;
	UCHAR		len,cnt,*Addr;		
	char 		szFileName[MTP_MAX_PATH];
	INT32		ercd;
		
	StorageID=(*CmdDS).parm1;
	ParObjHdl=(*CmdDS).parm2;
	StorageID=ENDIAN_SWAP32(StorageID);
	ParObjHdl=ENDIAN_SWAP32(ParObjHdl);

	BULK_Read((UCHAR*)OUT_Buffers, 0);
	allSize=(*CmdDS).ContainLen;
	allSize=ENDIAN_SWAP32(allSize);

#ifdef CO_BF_USB_MTP_DEBUG_ALL
	sys_time_before_all = BF_Get_Time_us();
#endif

	memcpy(&MtpRecvObjInfo,OUT_Buffers+12,allSize-12);
	len=MtpRecvObjInfo.FCMK[0];
	Addr=MtpRecvObjInfo.FCMK+1;

	if  ( (len > 0) && ((*(Addr+1)) != 0x00 ) )
	{
		SendResponseSimple(MTP_RS_InvalidDataSet);
		return 1;
	}
		
	for(cnt=0;cnt<len;cnt++)
	{
		RecvFileName[cnt]=*Addr;
		Addr=Addr+2;	
	}

	if(strcmp("HDISCVRY.DPS",(const char*)RecvFileName)==0)
	{
		RecvScriptHandle=HANDLE_FILE_HDISCVRY;
#ifdef CO_USB_MTP_ON
		if(DPSDiscovery != 2)
		{
			DPSDiscovery=1;
		}
#endif

		if(RecvScriptHandle)	
			ScriptFile_Use(RecvScriptHandle);

		if(!ptpUpdateFW){	
			ObjHdl=RecvScriptHandle;
		}else{
			ObjHdl=0xAAAAAAAA;
		}
		
		SendResponse(3,PTP_RS_OK,StorageID,ParObjHdl,ObjHdl,0,0);

		if(!ptpUpdateFW)
		{
#ifdef CO_USB_MTP_ON
			if(DPSDiscovery==1)
			{
		    		DPSDiscovery=2;
		    		DSP_Discovery();
			}
#endif
		}
		return 0;
	}

	
	memset(szFileName,0,MTP_MAX_PATH);

	if ( (ParObjHdl != 0x00000000) && (ParObjHdl != 0xFFFFFFFF) )
	{
		strncat(szFileName,oMTPDBFileInfo[ParObjHdl].szDosFileName, sizeof(szFileName)-strlen(szFileName)-1);
		strncat(szFileName,(const char*)RecvFileName, sizeof(szFileName)-strlen(szFileName)-1);
	}
	else
	{
		ercd = BF_Fs_If_Get_Cur_Drv(szFileName);
		if (ercd != 0) {
			BF_Debug_Print_Error(("USBMtpCmd_SendObjectInfo(): Fail to get current drive.\n"));
			return 1;
		}
	
		strncat(szFileName,":\\", sizeof(szFileName)-strlen(szFileName)-1);
		strncat(szFileName,(const char*)RecvFileName, sizeof(szFileName)-strlen(szFileName)-1);
	}

	strncpy(g_szMTPRecvFileName,szFileName,sizeof(g_szMTPRecvFileName));

	g_nObjectCnt++;
	ObjHdl = g_nObjectCnt;

	if ( (MtpRecvObjInfo.ObjectFormat[0] == 0x01) && (MtpRecvObjInfo.ObjectFormat[1] == 0x30) )
	{
		oMTPDBFileInfo[ObjHdl].bit.file_attr		= MTP_FOLDER_TYPE;
		oMTPDBFileInfo[ObjHdl].bit.file_exttype 	= MTP_FOLDER_TYPE;
		ercd = BF_Fs_If_Mkdir(szFileName);
		strncat(szFileName,"\\", sizeof(szFileName)-strlen(szFileName)-1);	
	}
	else
	{
		oMTPDBFileInfo[ObjHdl].bit.file_attr		= MTP_FILE_TYPE;	

		if (xIsJpgFile(g_szMTPRecvFileName) == 0)
			oMTPDBFileInfo[ObjHdl].bit.file_exttype 	= MTP_FILE_TYPE_JPG;
		else if (xIsBmpFile(g_szMTPRecvFileName) == 0)
			oMTPDBFileInfo[ObjHdl].bit.file_exttype 	= MTP_FILE_TYPE_BMP;
		else if (xIsGifFile(g_szMTPRecvFileName) == 0)
			oMTPDBFileInfo[ObjHdl].bit.file_exttype 	= MTP_FILE_TYPE_GIF;
		else if (xIsTiffFile(g_szMTPRecvFileName) == 0)
			oMTPDBFileInfo[ObjHdl].bit.file_exttype 	= MTP_FILE_TYPE_TIFF;
		else if (xIsWavFile(g_szMTPRecvFileName) == 0)
			oMTPDBFileInfo[ObjHdl].bit.file_exttype 	= MTP_FILE_TYPE_WAV;
		else if (xIsMp3File(g_szMTPRecvFileName) == 0)
			oMTPDBFileInfo[ObjHdl].bit.file_exttype 	= MTP_FILE_TYPE_MP3;
		else if (xIsAviFile(g_szMTPRecvFileName) == 0)
			oMTPDBFileInfo[ObjHdl].bit.file_exttype 	= MTP_FILE_TYPE_AVI;
		else if (xIsMovFile(g_szMTPRecvFileName) == 0)
			oMTPDBFileInfo[ObjHdl].bit.file_exttype 	= MTP_FILE_TYPE_MOV;
		else if (xIsMp4File(g_szMTPRecvFileName) == 0)
			oMTPDBFileInfo[ObjHdl].bit.file_exttype 	= MTP_FILE_TYPE_MP4;
		else
		{
			SendResponseSimple(MTP_RS_InvalidObjectFormatCode);
			return 1;
		}
	}
	
	oMTPDBFileInfo[ObjHdl].nFileIndex = ObjHdl;
	oMTPDBFileInfo[ObjHdl].nParentIndex = ParObjHdl;
	strncpy(oMTPDBFileInfo[ObjHdl].szDosFileName,szFileName,sizeof(oMTPDBFileInfo[ObjHdl].szDosFileName));
	SendResponse(3,PTP_RS_OK,StorageID,ParObjHdl,ObjHdl,0,0);
	BF_Debug_Print_Information(("USBMtpCmd_SendObjectInfo\n"));
	return 0;
}

UCHAR USBMtpCmd_SendObject( VOID )
{
	ULONG			recvSize;
	ULONG			ulReSize = 0;
	GCS_T			*CmdDS=(GCS_T *)OUT_Buffers;
	UINT32			allSize;
	unsigned long	Readsize;
	FW_INT32		fileHdl = 0;
	FW_ULONG		nWriteSize = 0;
	FW_INT32		nRetCode = 0;
	SHORT			sMaxPaketSize;
	INT32			cnt = 0;
	INT32			idx = 0;

#ifdef CO_BF_USB_MTP_DEBUG_READ
	ULONG ltime=0;
	ULONG sys_time_before, sys_time_after;
	sys_time_before = BF_Get_Time_us();
#endif

	MTP_InitData();

	sMaxPaketSize = MWUSB_GetMaxPacketSize( BULK_READ_FIFO );

	Readsize = BULK_Read2( (UCHAR*)&OUT_Buffers[ 0 ], 0 );

	allSize = (*CmdDS).ContainLen;
	allSize = ENDIAN_SWAP32(allSize);

	if ( USBPtpCmd_Check_DeviceBusy() ) {
		return 1;
	}

	recvSize = Readsize - 12;

#ifdef FSIF_ATTR_ATTR_ARCHIVE_ON
	fileHdl = BF_Fs_If_Open( g_szMTPRecvFileName, D_BF_FS_IF_O_RDWR|D_BF_FS_IF_O_CREAT, ( D_BF_FS_IF_ATTR_NORMAL | D_BF_FS_IF_ATTR_ARCHIVE ) );
#else
	fileHdl = BF_Fs_If_Open( g_szMTPRecvFileName, D_BF_FS_IF_O_RDWR|D_BF_FS_IF_O_CREAT, D_BF_FS_IF_ATTR_NORMAL );
#endif

	if ( fileHdl == 0 )
	{
		memset( &oMTPDBFileInfo[g_nObjectCnt], 0, sizeof(T_MTP_DB_FILE_INFO) );
		g_nObjectCnt--;
		SendResponseSimple( MTP_RS_InvalidDataSet );
		return 1;
	}

	BF_Fs_If_Lseek( fileHdl, 0, SEEK_SET, &nWriteSize );

	if ( recvSize < ( sMaxPaketSize - 12 ) )
	{
		nRetCode = BF_Fs_If_Write( fileHdl, (VOID*)(OUT_Buffers+12), recvSize, &nWriteSize );

		if ( nRetCode != 0 )
		{
			memset( &oMTPDBFileInfo[g_nObjectCnt], 0, sizeof(T_MTP_DB_FILE_INFO) );
			g_nObjectCnt--;
			BF_Fs_If_Close( fileHdl );
			SendResponseSimple( MTP_RS_InvalidDataSet );
			return 1;
		}
		BF_Fs_If_Close( fileHdl );
	}
	else
	{
		nRetCode = BF_Fs_If_Write( fileHdl, (VOID*)(OUT_Buffers+12), sMaxPaketSize - 12, &nWriteSize );

		if ( nRetCode != 0 )
		{
			memset( &oMTPDBFileInfo[g_nObjectCnt], 0, sizeof(T_MTP_DB_FILE_INFO) );
			g_nObjectCnt--;
			BF_Fs_If_Close( fileHdl );
			SendResponseSimple( MTP_RS_InvalidDataSet );
			return 1;
		}

		ulReSize = recvSize - ( sMaxPaketSize - 12 );

		while ( ulReSize > 0 )
		{
			idx = cnt * sMaxPaketSize * 1024;
			cnt++;

			if ( cnt >= MTP_DATA_MNG_INFO_NUM ) {
				cnt = 0;
			}

#ifdef CO_BF_USB_MTP_DEBUG_READ
			sys_time_before = BF_Get_Time_us();
#endif

			if ( ulReSize < ( sMaxPaketSize * 1024 ) )
			{
				// File write.
				while ( 1 ) {

					if ( MTP_PutData( BULK_READ_FIFO, (ULONG)&OUT_Buffers[ idx ], ulReSize, (ULONG)fileHdl ) == FJ_ERR_OK ) {
						OS_User_Set_Flg( FID_MTPTSK, FLG_MTP_READWRITE );
						break;
					}

					OS_User_Tslp_Tsk( 1 );
				}

#ifdef CO_BF_USB_MTP_DEBUG_READ
				sys_time_after = BF_Get_Time_us();
				ltime = sys_time_after - sys_time_before;
				printf("USB MTP Read Size1=%u(byte), time=%u(us), speed=%.3f(MB/s)\n",
												(FW_UINT32)Readsize, (FW_UINT32)ltime, ((float)(Readsize) * 1000 * 1000) / (ltime * 1024.0 * 1024.0));
#endif

				ulReSize = 0;
			}
			else
			{
				// File write.
				while ( 1 ) {

					if ( MTP_PutData( BULK_READ_FIFO, (ULONG)&OUT_Buffers[ idx ],  sMaxPaketSize * 1024, (ULONG)fileHdl ) == FJ_ERR_OK ) {
						OS_User_Set_Flg( FID_MTPTSK, FLG_MTP_READWRITE );
						break;
					}

					OS_User_Tslp_Tsk( 1 );
				}

#ifdef CO_BF_USB_MTP_DEBUG_READ
				sys_time_after = BF_Get_Time_us();
				ltime = sys_time_after - sys_time_before;
				printf("USB MTP Read Size2=%u(byte), time=%u(us), speed=%.3f(MB/s)\n",
												(FW_UINT32)Readsize, (FW_UINT32)ltime, ((float)(Readsize) * 1000 * 1000) / (ltime * 1024.0 * 1024.0));
#endif

				ulReSize = ulReSize - ( sMaxPaketSize * 1024 );
			}

			OS_User_Tslp_Tsk( 1 );
		}

		// File close.
		while ( 1 ) {

			if ( MTP_PutData( BULK_READ_FIFO, 0, 0, (ULONG)fileHdl ) == FJ_ERR_OK ) {
				OS_User_Set_Flg( FID_MTPTSK, FLG_MTP_READWRITE );
				break;
			}

			OS_User_Tslp_Tsk( 1 );
		}
	}

	// OK response.
	while ( 1 ) {

		if ( MTP_PutData( VEND_WRITE_FIFO, 0, 0, 0 ) == FJ_ERR_OK ) {
			OS_User_Set_Flg( FID_MTPTSK, FLG_MTP_READWRITE );
			break;
		}

		OS_User_Tslp_Tsk( 1 );
	}

	BF_Debug_Print_Information(("USBMtpCmd_SendObject\n"));

	return 0;
}

VOID USBMtpTsk_ReadWrite( INT32 stacd )
{
	OS_USER_FLGPTN	flgptn, retptn;
	ULONG			addr, size, epno, opt;
	FW_INT32		nRetCode	= 0;
	FW_ULONG		nWriteSize	= 0;

	BF_Debug_Print_Information(("MTP Read/Write Task Start.....\r\n"));

	flgptn = FLG_MTP_READWRITE | FLG_MTP_TERMINATE;

	while ( 1 ) {

		if ( OS_User_Twai_Flg( FID_MTPTSK, flgptn, D_OS_USER_TWF_ORW, &retptn, D_OS_USER_TMO_FEVR ) == D_OS_USER_E_OK ) {

			OS_User_Clr_Flg( FID_MTPTSK, ~(retptn) );

			// MTP read/write.
			if ( ( retptn & FLG_MTP_READWRITE ) == FLG_MTP_READWRITE ) {

				while ( 1 ) {

					// Get MTP data.
					if ( MTP_GetData( &epno, &addr, &size, &opt ) == FJ_ERR_OK ) {
						// USB write.
						if ( epno == BULK_WRITE_FIFO ) {

							if ( ( addr != 0 ) && ( size != 0 ) ) {
								MWUSB_SetMode( 0 );
								MWUSB_Write( (UCHAR*)addr, size, BULK_WRITE_FIFO );
							}
							else {
								MWUSB_SendNullPacket( BULK_WRITE_FIFO );
								break;
							}
						}
						// USB write.
						else if ( epno == BULK_READ_FIFO ) {
							// File write.
							if ( ( addr != 0 ) && ( size != 0 ) ) {

								nRetCode = BF_Fs_If_Write( (FW_INT32)opt, (VOID*)addr, size, &nWriteSize );

								if ( nRetCode != 0 ) {
									memset( &oMTPDBFileInfo[ g_nObjectCnt ], 0, sizeof( T_MTP_DB_FILE_INFO ) );
									g_nObjectCnt--;
									BF_Fs_If_Close( (FW_INT32)opt );
									SendResponseSimple( MTP_RS_InvalidDataSet );
									break;
								}
							}
							// File close.
							else {
								BF_Fs_If_Close( (FW_INT32)opt );
								break;
							}
						}
						// MTP OK response..
						else if ( epno == VEND_WRITE_FIFO ) {
							SendResponseSimple( PTP_RS_OK );
							break;
						}
						else {
							BF_Debug_Print_Information(("MTP Read/Write Task epno is unknown.\r\n"));
						}
					}
					else {
						// Buffer empty.
						OS_User_Tslp_Tsk( 1 );
						break;
					}

					OS_User_Tslp_Tsk( 1 );
				}

				continue;
			}

			// MTP read/write task end.
			if ( ( retptn & FLG_MTP_TERMINATE ) == FLG_MTP_TERMINATE ) {
				break;
			}
		}
		else {
			BF_Debug_Print_Error(("MTP Read/Write Task WaitFlg error.\r\n"));
		}
	}
	
	OS_User_Clr_Flg(FID_MTPTSK, 0);
	
	// Release mdf resource
	BF_Fs_If_End_Task_Mdf();
	
	BF_Debug_Print_Information(("MTP Read/Write Task End.....\r\n"));

	OS_User_Ext_Tsk();
}

