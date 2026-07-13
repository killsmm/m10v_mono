/**
 * @file		mtp.h
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */
#ifndef _MTP_
#define _MTP_

#define MTP_SERIAL_NUMBER_LEN 			32	
#define DOS_FILENAME_LEN 				13
#define JPG_SUPPPROP_NUM				16
#define AVI_SUPPPROP_NUM 				18 
#define ASSOCIATION_SUPPPROP_NUM 		 8
#define DPOF_SUPPPROP_NUM				 8
#define WAV_SUPPPROP_NUM				12
#define MP3_SUPPPROP_NUM				12
#define BMP_SUPPPROP_NUM				11
#define GIF_SUPPPROP_NUM				11
#define TIFF_SUPPPROP_NUM				11
#define MOV_SUPPPROP_NUM				19
#define MP4_SUPPPROP_NUM				19

//#define MTP_NUM_OBJECT_SD		20000

/*----------- MTP Operation Request Code ---------------*/

#define MTP_OP_DeleteObject					0x100B
#define MTP_OP_SendObjectInfo 				0x100C
#define MTP_OP_SendObject 					0x100D
#define MTP_OP_GetDevicePropValue			0x1015

#define MTP_OP_GetObjectPropsSupported 		0x9801
#define MTP_OP_GetObjectPropDesc 			0x9802
#define MTP_OP_GetObjectPropValue			0x9803
#define MTP_OP_GetObjectPropList 			0x9805

#define MTP_OP_SetObjectPropValue			0x9804
#define MTP_OP_SetObjectPropList				0x9806
#define MTP_OP_SendObjectPropList			0x9808
#define MTP_OP_GetObjectReferences 			0x9810
#define MTP_OP_SetObjectReferences			0x9811

#define MTP_OP_GetInterdependentPropDesc 	0x9807


/*----------- MTP Response Code ---------------*/
#define MTP_RS_InvalidObjectHandle					0x2009
#define MTP_RS_DevicePropNotSupported				0x200A
#define MTP_RS_InvalidObjectFormatCode               		0x200B
#define MTP_RS_InvalidDevicePropValue               		0x201C 
#define MTP_RS_InvalidObjectPropCode				0xA801
#define MTP_RS_InvalidDataSet						0xA806

#define MTP_RS_Group_Not_Supported					0xA805
#define MTP_RS_Specification_By_Group_Unsupported 	0xA807
#define MTP_RS_Specification_By_Depth_Unsupported   0xA808
#define MTP_RS_ObjectPropNotSupported				0xA80A

/*----------- MTP Object Format Code ---------------*/
#define MTP_OFC_UNDEFINED 			0x3000
#define MTP_OFC_ASSOCIATION 		0x3001
#define MTP_OFC_SCRIPT				0x3002
#define MTP_OFC_TEXT				0x3004
#define MTP_OFC_HTML				0x3005
#define MTP_OFC_DPOF				0x3006
#define MTP_OFC_WAV					0x3008
#define MTP_OFC_MP3					0x3009
#define MTP_OFC_AVI					0x300A
#define MTP_OFC_MPEG				0x300B
#define MTP_OFC_EXIFJPEG			0x3801
#define MTP_OFC_BMP					0x3804
#define MTP_OFC_GIF					0x3807
#define MTP_OFC_TIFF				0x380D
#define MTP_OFC_MOV					0xB980
#define MTP_OFC_MP4					0xB982

#define MTP_OPC_STORAGEID			0xDC01
#define MTP_OPC_OBJECTFORMAT		0xDC02
#define MTP_OPC_PROTECTIONSTATUS 	0xDC03
#define MTP_OPC_OBJECTSIZE			0xDC04
#define MTP_OPC_ASSOCIATIONTYPE		0xDC05
#define MTP_OPC_ASSOCIATIONDESC		0xDC06
#define MTP_OPC_OBJECTFILENAME		0xDC07
#define MTP_OPC_DATECREATED			0xDC08
#define MTP_OPC_DATEMODIFIED 		0xDC09
#define MTP_OPC_KEYWORDS			0xDC0A
#define MTP_OPC_PARENTOBJECT		0xDC0B
#define MTP_OPC_PUOI				0xDC41
#define MTP_OPC_NAME				0xDC44
#define MTP_OPC_REPSampleFormat		0xDC81
#define MTP_OPC_REPSampleSize		0xDC82
#define MTP_OPC_REPSampleHeigth		0xDC83
#define MTP_OPC_REPSampleWidth		0xDC84
#define MTP_OPC_REPSampleData		0xDC86
#define MTP_OPC_WIDTH				0xDC87
#define MTP_OPC_HEIGHT				0xDC88
#define MTP_OPC_DURATION			0xDC89
#define MTP_OPC_IMAGEBITDEPTH		0xDCD3
#define MTP_OPC_SAMPLERATE			0xDE93
#define MTP_OPC_NUMBEROFCHANNEL 	0xDE94
#define MTP_OPC_SCANTYPE			0xDE97
#define MTP_OPC_AUDIOBITRATE		0xDE9A
#define MTP_OPC_VIDEOFOURCCCODEC    0xDE9B
#define MTP_OPC_VIDEOBITRATE		0xDE9C

/*------------- DevicePropCode --------------*/
#define MTP_DPC_BATTERYLEVEL				0x5001
#define MTP_DPC_WHITEBALANCE				0x5005
#define MTP_DPC_RGBGAIN						0x5006
#define MTP_DPC_FNUMBER						0x5007
#define MTP_DPC_FOCALLENGTH					0x5008
#define MTP_DPC_FOCUSDISTANCE       		0x5009
#define MTP_DPC_FOCUSMODE					0x500A
#define MTP_DPC_EXPOSUREMETERINGMODE		0x500B 
#define MTP_DPC_FLASHMODE					0x500C
#define MTP_DPC_EXPOSURETIME 				0x500D
#define MTP_DPC_EXPOSUREPROGRAMMODE			0x500E
#define MTP_DPC_EXPOSUREINDEX				0x500F
#define MTP_DPC_EXPOSRUEBIASCOMPENSATION 	0x5010
#define MTP_DPC_DATETIME					0x5011
#define MTP_DPC_CAPTUREDELAY				0x5012
#define MTP_DPC_STILLCAPTUREMODE			0x5013
#define MTP_DPC_CONTRAST					0x5014
#define MTP_DPC_SHARPNESS					0x5015
#define MTP_DPC_DIGITALZOOM					0x5016
#define MTP_DPC_EFFECTMODE					0x5017
#define MTP_DPC_BURSTNUMBER					0x5018
#define MTP_DPC_BURSTINTERVAL				0x5019
#define MTP_DPC_TIMELAPSENUMBER				0x501A
#define MTP_DPC_TIMELAPSEINTERVAL			0x501B
#define MTP_DPC_FOCUSMETERINGMODE			0x501C

#define MTP_DPC_DEVICEICON					0xD405
#define MTP_DPC_PERCEIVED					0xD407

/*----------- MTP DATA TYPE  ---------------*/
#define MTP_DATATYPE_UNDEF			0x0000
#define MTP_DATATYPE_INT8			0x0001
#define MTP_DATATYPE_UINT8			0x0002
#define MTP_DATATYPE_INT16 			0x0003
#define MTP_DATATYPE_UINT16			0x0004
#define MTP_DATATYPE_INT32			0x0005
#define MTP_DATATYPE_UINT32			0x0006
#define MTP_DATATYPE_INT64			0x0007
#define MTP_DATATYPE_UINT64			0x0008
#define MTP_DATATYPE_INT128			0x0009
#define MTP_DATATYPE_UINT128		0x000A
#define MTP_DATATYPE_AUINT8			0x4002
#define MTP_DATATYPE_STR			0xFFFF

extern int 		PTP_fp;
extern ULONG	PTP_fread(ULONG buff,ULONG fsize); 	

UCHAR USBMtpCmd_GetDeviceInfo(void);
UCHAR USBMtpCmd_GetStorageIDs(void);
UCHAR USBMtpCmd_GetStorageInfo(void);
UCHAR USBMtpCmd_GetNumObjects(void);
UCHAR USBMtpCmd_GetObjecthandles(void);

UCHAR USBMtpCmd_DeleteObject(void);
UCHAR USBMtpCmd_GetDevicePropDesc(void);
UCHAR USBMtpCmd_GetDevicePropValue(void);
UCHAR USBMtpCmd_SetDevicePropValue(void);
UCHAR USBMtpCmd_GetObjectPropsSupported(void);
UCHAR USBMtpCmd_GetObjectPropDesc(void);
UCHAR USBMtpCmd_GetObjectPropValue(void);
UCHAR USBMtpCmd_GetObjectPropList(void);
UCHAR USBMtpCmd_SendObjectInfo(void);
UCHAR USBMtpCmd_SendObject(void);


OS_USER_ER   USBMtpProcessObjectOpen(USHORT ProcKind, UINT32 objhdl, UINT32 parm1, UINT32 parm2);
//INT32        MTP_GetObjectHandle(E_BF_MEDIA_STATE_MEDIA_TYPE mediaType, USHORT ObjectFormatCode,USHORT nParentHandle, UINT32 *ptr);
INT32        MTP_GetObjectHandle(USHORT ObjectFormatCode,USHORT nParentHandle, UINT32 *ptr);
INT32        ResetMTPAllObjectInfo(void);
INT32        MTP_GetAllObjectInfo(E_BF_MEDIA_STATE_MEDIA_TYPE mediaType);

// ObjectPropsCodeArray (for 0x9801)
typedef struct{
	UCHAR ObjectProp_Len[4];
	UCHAR ObjectProp1[2];
	UCHAR ObjecjProp2[2];
	UCHAR ObjectProp3[2];
	UCHAR ObjectProp4[2];
	UCHAR ObjectProp5[2];
	UCHAR ObjectProp6[2];
	UCHAR ObjectProp7[2];
	UCHAR ObjectProp8[2];
	UCHAR ObjectProp9[2];
	UCHAR ObjectProp10[2];	
	UCHAR ObjectProp11[2];	
	UCHAR ObjectProp12[2];
	UCHAR ObjectProp13[2];
	UCHAR ObjectProp14[2];
	UCHAR ObjectProp15[2];
	UCHAR ObjectProp16[2];
	UCHAR ObjectProp17[2];
	UCHAR ObjectProp18[2];
	UCHAR ObjectProp19[2];
	//UCHAR ObjectProp20[2];	
}FormatCode_SupportProp;

//FormatCodeAVISupportProp

// ObjectPropsCodeArray (for 0x9801)
typedef struct{
	UCHAR ObjectProp_Len[4];
	UCHAR ObjectProp1[2];
	UCHAR ObjecjProp2[2];
	UCHAR ObjectProp3[2];
	UCHAR ObjectProp4[2];
	UCHAR ObjectProp5[2];
	UCHAR ObjectProp6[2];
	UCHAR ObjectProp7[2];
	UCHAR ObjectProp8[2];
	UCHAR ObjectProp9[2];
	UCHAR ObjectProp10[2];	
	UCHAR ObjectProp11[2];	
	UCHAR ObjectProp12[2];
	UCHAR ObjectProp13[2];
	UCHAR ObjectProp14[2];
	UCHAR ObjectProp15[2];
	UCHAR ObjectProp16[2];
	UCHAR ObjectProp17[2];
	UCHAR ObjectProp18[2]; // 2009/04/08 tony + (for modify date)
}MTPFormatCode_AVISupportProp;

//FormatCodeDPOFSupportProp
// ObjectPropsCodeArray (for 0x9801)
typedef struct{
	UCHAR ObjectProp_Len[4];
	UCHAR ObjectProp1[2];
	UCHAR ObjecjProp2[2];
	UCHAR ObjectProp3[2];
	UCHAR ObjectProp4[2];
	UCHAR ObjectProp5[2];
	UCHAR ObjectProp6[2];
	UCHAR ObjectProp7[2];
	UCHAR ObjectProp8[2];
}MTPFormatCode_DPOFSupportProp;

//FormatCodeMOVSupportProp
// ObjectPropsCodeArray (for 0x9801)
typedef struct{
	UCHAR ObjectProp_Len[4];
	UCHAR ObjectProp1[2];
	UCHAR ObjecjProp2[2];
	UCHAR ObjectProp3[2];
	UCHAR ObjectProp4[2];
	UCHAR ObjectProp5[2];
	UCHAR ObjectProp6[2];
	UCHAR ObjectProp7[2];
	UCHAR ObjectProp8[2];
	UCHAR ObjectProp9[2];
	UCHAR ObjectProp10[2];
	UCHAR ObjectProp11[2];
	UCHAR ObjectProp12[2];
	UCHAR ObjectProp13[2];
	UCHAR ObjectProp14[2];
	UCHAR ObjectProp15[2];
	UCHAR ObjectProp16[2];
	UCHAR ObjectProp17[2];
	UCHAR ObjectProp18[2];
	UCHAR ObjectProp19[2];
}MTPFormatCode_MOVSupportProp;

//FormatCodeMP4SupportProp
// ObjectPropsCodeArray (for 0x9801)
typedef struct{
	UCHAR ObjectProp_Len[4];
	UCHAR ObjectProp1[2];
	UCHAR ObjecjProp2[2];
	UCHAR ObjectProp3[2];
	UCHAR ObjectProp4[2];
	UCHAR ObjectProp5[2];
	UCHAR ObjectProp6[2];
	UCHAR ObjectProp7[2];
	UCHAR ObjectProp8[2];
	UCHAR ObjectProp9[2];
	UCHAR ObjectProp10[2];
	UCHAR ObjectProp11[2];
	UCHAR ObjectProp12[2];
	UCHAR ObjectProp13[2];
	UCHAR ObjectProp14[2];
	UCHAR ObjectProp15[2];
	UCHAR ObjectProp16[2];
	UCHAR ObjectProp17[2];
	UCHAR ObjectProp18[2];
	UCHAR ObjectProp19[2];
}MTPFormatCode_MP4SupportProp;

// ObjectPropsCodeArray (for 0x9801)
typedef struct{
	UCHAR ObjectProp_Len[4];
	UCHAR ObjectProp1[2];
	UCHAR ObjecjProp2[2];
	UCHAR ObjectProp3[2];
	UCHAR ObjectProp4[2];
	UCHAR ObjectProp5[2];
	UCHAR ObjectProp6[2];
	UCHAR ObjectProp7[2];
	UCHAR ObjectProp8[2];
	UCHAR ObjectProp9[2];
	UCHAR ObjectProp10[2];	
	UCHAR ObjectProp11[2];	
	UCHAR ObjectProp12[2];	
}MTPFormatCode_WAVSupportProp;


typedef struct{
	UCHAR ObjectProp_Len[4];
	UCHAR ObjectProp1[2];
	UCHAR ObjecjProp2[2];
	UCHAR ObjectProp3[2];
	UCHAR ObjectProp4[2];
	UCHAR ObjectProp5[2];
	UCHAR ObjectProp6[2];
	UCHAR ObjectProp7[2];
	UCHAR ObjectProp8[2];
	UCHAR ObjectProp9[2];
	UCHAR ObjectProp10[2];	
	UCHAR ObjectProp11[2];	
	UCHAR ObjectProp12[2];	
}MTPFormatCode_MP3SupportProp;



// ObjectPropsCodeArray (for 0x9801)
typedef struct{
	UCHAR ObjectProp_Len[4];
	UCHAR ObjectProp1[2];
	UCHAR ObjecjProp2[2];
	UCHAR ObjectProp3[2];
	UCHAR ObjectProp4[2];
	UCHAR ObjectProp5[2];
	UCHAR ObjectProp6[2];
	UCHAR ObjectProp7[2];
	UCHAR ObjectProp8[2];
}MTPFormatCode_AssociationSupportProp;

// 2009/03/27  tony +
// ObjectPropsCodeArray (for 0x9801)
typedef struct{
	UCHAR ObjectProp_Len[4];
	UCHAR ObjectProp1[2];
	UCHAR ObjecjProp2[2];
	UCHAR ObjectProp3[2];
	UCHAR ObjectProp4[2];
	UCHAR ObjectProp5[2];	//08/15/08 tony mark +
	UCHAR ObjectProp6[2];	//08/15/08 tony mark +
	UCHAR ObjectProp7[2];	//08/15/08 tony mark +
	UCHAR ObjectProp8[2];	//08/15/08 tony mark +	
	UCHAR ObjectProp9[2];
	UCHAR ObjectProp10[2];
	UCHAR ObjectProp11[2];
	UCHAR ObjectProp12[2];
	UCHAR ObjectProp13[2];
	UCHAR ObjectProp14[2];
	UCHAR ObjectProp15[2];
	UCHAR ObjectProp16[2];
	//UCHAR ObjectProp17[2];
	//UCHAR ObjectProp18[2];
	//UCHAR ObjectProp19[2];
	//UCHAR ObjectProp20[2];	
}MTPFormatCode_JPEGSupportProp;
// 2009/03/27  tony -


// ObjectPropsCodeArray (for 0x9801)
typedef struct{
	UCHAR ObjectProp_Len[4];
	UCHAR ObjectProp1[2];
	UCHAR ObjecjProp2[2];
	UCHAR ObjectProp3[2];
	UCHAR ObjectProp4[2];
	UCHAR ObjectProp5[2];	//08/15/08 tony mark +
	UCHAR ObjectProp6[2];	//08/15/08 tony mark +
	UCHAR ObjectProp7[2];	//08/15/08 tony mark +
	UCHAR ObjectProp8[2];	//08/15/08 tony mark +	
	UCHAR ObjectProp9[2];
	UCHAR ObjectProp10[2];
	UCHAR ObjectProp11[2];
}MTPFormatCode_ImageSupportProp;



//  ObjectPropCode : 0xDC01
typedef struct{
	UCHAR PropertyCode[2];
	UCHAR DataType[2];		
	UCHAR GetSet[1];
	UCHAR Default[4];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
}MTPObjectProperty_StorageID; 



//  ObjectPropCode : 0xDC02
typedef struct {
	UCHAR PropertyCode[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR Default[2];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
}MTPObjectProperty_ObjectFormat;

//  ObjectPropCode : 0xDC03
typedef struct{
	UCHAR Property[2];
	UCHAR Datatype[2];
	UCHAR GetSet[1];
	UCHAR Default[2];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
	UCHAR NumValues[2];
	UCHAR Support1[2];
	UCHAR Support2[2];
	UCHAR Support3[2];
	UCHAR Support4[2];
}MTPObjectProperty_ProtectionStatus;

//  ObjectPropCode : 0xDC04
typedef struct{
	UCHAR 	PropertyCode[2];
	UCHAR  	DataType[2];
	UCHAR 	GetSet[1];
	UCHAR 	DefaultValeu[8];
	UCHAR 	GropuCode[4];
	UCHAR 	FormFlag[1];
}MTPObjectProperty_ObjectSize;

//  ObjectPropCode : 0xDC05
typedef struct{
	UCHAR PropertyCode[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR Default[2];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
	UCHAR Support1[2];
	UCHAR Support2[2];
}MTPObjectProperty_AssociationType;

//  ObjectPropCode : 0xDC06
typedef struct{
	UCHAR PropertyCode[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[4];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
}MTPObjectProperty_AssociationDesc;



//  ObjectPropCode : 0xDC07
typedef struct{
	UCHAR PropertyCode[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[1];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
}MTPObjectProperty_ObjectFileName;


//  ObjectPropCode : 0xDC08
typedef struct{
	UCHAR PropertyCode[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[1];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
}MTPObjectProperty_DateCreated;


//  ObjectPropCode : 0xDC09
typedef struct{
	UCHAR PropertyCode[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[1];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
}MTPObjectProperty_DateModified;


//  ObjectPropCode : 0xDC0A
typedef struct{
	UCHAR PropertyCode[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	//UCHAR DefaultValue[1];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
}MTPObjectProperty_Keywords;


//  ObjectPropCode : 0xDC0B
typedef struct{
	UCHAR PropertyCode[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[4];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
}MTPObjectProperty_ParentObject;

//  ObjectPropCode : 0xDC41
typedef struct{
	UCHAR PropertyCode[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[16];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
}MTPObjectProperty_PUOI;

//  ObjectPropCode : 0xDC44
typedef struct{
	UCHAR PropertyCode[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[1];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
}MTPObjectProperty_Name;

// ObjectPropCode : 0xDC81
typedef struct{
	UCHAR Property[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[2];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
	UCHAR Size[2];
	UCHAR Support1[2];
}MTPObjectProperty_RepresentativeSampleFormat;

// ObjectPropCode : 0xDC82
typedef struct{
	UCHAR PropertyCode[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[4];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
	UCHAR MinValue[4];
	UCHAR MaxValue[4];
	UCHAR StepSize[4];
}MTPObjectProperty_RepresentativeSampleSize;

// ObjectPropCode : 0xDC83
typedef struct{
	UCHAR PropertyCode[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[4];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
	UCHAR MinValue[4];
	UCHAR MaxValue[4];
	UCHAR StepSize[4];
}MTPObjectProperty_RepresentativeSampleHeight;

// ObjectPropCode : 0xDC84
typedef struct{
	UCHAR PropertyCode[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[4];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
	UCHAR MinValue[4];
	UCHAR MaxValue[4];
	UCHAR StepSize[4];
}MTPObjectProperty_RepresentativeSampleWidth;

// ObjectPropCode : 0xDC86
typedef struct{
	UCHAR PropertyCode[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[4];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
	UCHAR MaxLength[4];
}MTPObjectProperty_RepresentativeSampleData;


//  ObjectPropCode : 0xDC87
typedef struct{
	UCHAR PropertyCode[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[4];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
	UCHAR MinValue[4];
	UCHAR MaxValue[4];
	UCHAR StepSize[4];
}MTPObjectProperty_ImageWidth;



//  ObjectPropCode : 0xDC88
typedef struct{
	UCHAR PropertyCode[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[4];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
	UCHAR MinValue[4];
	UCHAR MaxValue[4];
	UCHAR StepSize[4];
}MTPObjectProperty_ImageHeight;

//  ObjectPropCode : 0xDC89
typedef struct{
	UCHAR Property[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[4];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
	UCHAR MinValue[4];
	UCHAR MaxValue[4];
	UCHAR StepSize[4];	
}MTPObjectProperty_Duration;

//  ObjectPropCode : 0xDE95
typedef struct{
	UCHAR PropertyCode[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[4];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
	UCHAR NumValue[2];
	UCHAR SupportValue[4];	
}MTPObjectProperty_ImageBitDepth;

//  ObjectPropCode : 0xDE93
typedef struct{
	UCHAR Property[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[4];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
	UCHAR Size[2];
	UCHAR Support1[4];
}MTPObjectProperty_SampleRate;

//  ObjectPropCode : 0xDE94
typedef struct{
	UCHAR Property[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[2];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
	UCHAR Size[2];
	UCHAR Support1[2];
}MTPObjectProperty_NumberOfChannel;

//  ObjectPropCode : 0xDE97
typedef struct{
	UCHAR Property[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[2];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
	UCHAR NumofVaule[2];
	UCHAR SupportVale1[2];
	UCHAR SupportVale2[2];	
}MTPObjectProperty_ScanType;

//  ObjectPropCode : 0xDE9A
typedef struct{
	UCHAR Property[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[4];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
	UCHAR MinValue[4];
	UCHAR MaxValue[4];
	UCHAR StepSize[4];
}MTPObjectProperty_AudioBitRate;

//  ObjectPropCode : 0xDE9B
typedef struct{
	UCHAR Property[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[4];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
	UCHAR NumofVaule[2];
	UCHAR SupportValue1[4];
	UCHAR SupportValue2[4];
}MTPObjectProperty_VideoFourCC_Code;

//  ObjectPropCode : 0xDE9C
typedef struct{
	UCHAR Property[2];
	UCHAR DataType[2];
	UCHAR GetSet[1];
	UCHAR DefaultValue[4];
	UCHAR GroupCode[4];
	UCHAR FormFlag[1];
	UCHAR MinValue[4];
	UCHAR MaxValue[4];
	UCHAR StepSize[4];
}MTPObjectProperty_VideoBitRate;

//typedef struct _MTPOBJECTPropInfo
//{
//	UINT32 	dwObjectHandle;
//	UCHAR   byDeleteFlag;		
//}MTPOBJECTPropInfo;
#if 0
typedef struct {
        UCHAR   StandardVersion[2];
        UCHAR   VendorExtensionID[4];
        UCHAR   VendorExtensionVersion[2];
        UCHAR   VendorExtensionDesc_Len;
        UCHAR   VendorExtensionDesc_Data[23*2];              
        UCHAR   FunctionMode[2];

        UCHAR   OperationsSupported_Num[4];
		
	 UCHAR   OperationSupported_Data[22*2];
		
        UCHAR   EventsSupported_Num[4];
        UCHAR   EventsSupported_Data[6*2];
        UCHAR   DevicePropertiesSupported_Num[4];
        UCHAR   DevicePropertiesSupported_Data[5*2];
        UCHAR   CaptureFormats_Num[4];
        UCHAR   ImageFormats_Num[4];
	 UCHAR 	ImageFormats_Data[8*2];	
        UCHAR   Manufacturer_Len;
        UCHAR   Manufacturer_Data[PTP_VENDOR_NAME_STRING_LEN*2];
        UCHAR   Model_Len;
        UCHAR   Model_Data[PTP_MODEL_STRING_LEN*2];
        UCHAR   DeviceVersion_Len;
        UCHAR   DeviceVersion_Data[0x04*2];
        UCHAR   SerialNumber_Len;
		UCHAR   SerialNumber_Data[PTP_SERIAL_NUMBER_LEN*2];        
        UCHAR	fill[1];
}MTP_DEVICE_INFO;
#else
typedef struct {
        UCHAR   StandardVersion[2];
        UCHAR   VendorExtensionID[4];
        UCHAR   VendorExtensionVersion[2];
        UCHAR   VendorExtensionDesc_Len;
        UCHAR   VendorExtensionDesc_Data[23*2];              
        UCHAR   FunctionMode[2];
        UCHAR   OperationsSupported_Num[4];		
        UCHAR   OperationSupported_Data[22*2];		
        UCHAR   EventsSupported_Num[4];
        UCHAR   EventsSupported_Data[6*2];
        UCHAR   DevicePropertiesSupported_Num[4];
        UCHAR   DevicePropertiesSupported_Data[1*2];
        UCHAR   CaptureFormats_Num[4];
        UCHAR   CaptureFormats_Data[4*2];
        UCHAR   ImageFormats_Num[4];
        UCHAR   ImageFormats_Data[4*2];	//supported media type * 2 
        UCHAR   Manufacturer_Len;
        UCHAR   Manufacturer_Data[PTP_VENDOR_NAME_STRING_LEN*2];
        UCHAR   Model_Len;
        UCHAR   Model_Data[PTP_MODEL_STRING_LEN*2];
        UCHAR   DeviceVersion_Len;
        UCHAR   DeviceVersion_Data[0x04*2];
        UCHAR   SerialNumber_Len;
        UCHAR   SerialNumber_Data[PTP_SERIAL_NUMBER_LEN*2];        
        UCHAR   fill[1];
}MTP_DEVICE_INFO;
#endif
/*-------------- MTPDeviceProp_PerceiveDeviceType Dataset ----------------*/

// 0x5001 : Battery Level
// SupportValue : 0 - 100
typedef struct {
        UCHAR   DevicePropCode[2];
        UCHAR   DataType[2];
        UCHAR   GetSet[1];
        UCHAR   FactoryDefaultValue[1];
		UCHAR 	CurrnetValue[1];
        UCHAR   FormFlag[1];
		UCHAR	MinValue[1];
		UCHAR	MaxValue[1];
		UCHAR	StepSize[1];
}MTP_DeviceProp_BatteryLevel;

// 0x5003 : Device Property ImageSize
// SupportValue : 3072*2304, 3072*2048, 2576*1932, 2048*1536, 1600*1200, 640*480
typedef struct {
        UCHAR   DevicePropCode[2];
        UCHAR   DataType[2];
        UCHAR   GetSet[1];
        UCHAR   FactoryDefaultValue[1];
		UCHAR 	CurrnetValue[1];
        UCHAR   FormFlag[1];
        UCHAR   NumberOfValue[2];
		UCHAR 	SupportedValue1Length[1];
		UCHAR 	SupportedValue1[10*2];
		UCHAR   SupportedValue2Length[1];
		UCHAR 	SupportedValue2[10*2];
		UCHAR 	SupportedValue3Length[1];
		UCHAR 	SupportedValue3[10*2];
		UCHAR 	SupportedValue4Length[1];
		UCHAR 	SupportedValue4[10*2];
		UCHAR   SupportedValue5Length[1];
		UCHAR 	SupportedValue5[10*2];
		UCHAR 	SupportedValue6Length[1];
		UCHAR   SupportedValue6[8*2];
 }MTP_DeviceProp_ImageSize;

// 0x5005 : White Balance
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[1];
	UCHAR	CurrnetValue[1];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_WhiteBalance;

// 0x5006 : RGB Gain
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[1];
	UCHAR	CurrnetValue[1];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_RGBGain;

// 0x5007 : F-Number
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[2];
	UCHAR	CurrnetValue[2];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_FNumber;

// 0x5008 : Focal Length
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[4];
	UCHAR	CurrnetValue[4];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_FocalLength;

// 0x5009 : Focus Distance
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[2];
	UCHAR	CurrnetValue[2];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_FocusDistance;

// 0x500A : Focus Mode
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[2];
	UCHAR	CurrnetValue[2];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_FocusMode;

// 0x500B : Exposure Metering Mode 
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[2];
	UCHAR	CurrnetValue[2];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_ExposureMeteringMode;

// 0x500C : Flash Mode
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[2];
	UCHAR	CurrnetValue[2];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_FlashMode;

// 0x500D : Exposure Time 
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[4];
	UCHAR	CurrnetValue[4];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_ExposureTime;

// 0x500E : ExposureProgramMode
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[2];
	UCHAR	CurrnetValue[2];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_ExposureProgramMode;

// 0x500F : ExposureIndex
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[2];
	UCHAR	CurrnetValue[2];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_ExposureIndex;

// 0x5010 : ExposureBiasCompensation
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[2];
	UCHAR	CurrnetValue[2];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_ExposureBiasCompensation;

// 0x5011 : DateTime
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR 	SupportedValue1Length[1];
	UCHAR 	SupportedValue1[16*2];
	UCHAR   SupportedValue2Length[1];
	UCHAR 	SupportedValue2[16*2];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_DateTime;

// 0x5012 : CaptureDelay
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[4];
	UCHAR	CurrnetValue[4];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_CaptureDelay;

// 0x5013 :  StillCaptureMode
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[2];
	UCHAR	CurrnetValue[2];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_StillCaptureMode;

// 0x5014 : Contrast
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[1];
	UCHAR	CurrnetValue[1];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_Contrast;

// 0x5015 : Sharpness
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[1];
	UCHAR	CurrnetValue[1];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_Sharpness;

// 0x5016 : DigitalZoom
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[1];
	UCHAR	CurrnetValue[1];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_DigitalZoom;

// 0x5017 : EffectMode
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[2];
	UCHAR	CurrnetValue[2];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_EffectMode;

// 0x5018 : BurstNumber
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[2];
	UCHAR	CurrnetValue[2];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_BurstNumber;

// 0x5019 : BurstInterval
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[2];
	UCHAR	CurrnetValue[2];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_BurstInterval;

// 0x501A : TimelapseNumber
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[2];
	UCHAR	CurrnetValue[2];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_TimelapseNumber;

// 0x501B : TimelapseInterval
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[4];
	UCHAR	CurrnetValue[4];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_TimelapseInterval;

// 0x501C : FocusMeteringMode
typedef struct{
	UCHAR	DevicePropCode[2];
	UCHAR	DataType[2];
	UCHAR	GetSet[1];
	UCHAR	FactoryDefaultValue[4];
	UCHAR	CurrnetValue[4];
	UCHAR	FormFlag[1];
}MTP_DeviceProp_FocusMeteringMode;

// 0xD405 : DeviceIcon
typedef struct {
        UCHAR   DevicePropCode[2];
        UCHAR   DataType[2];
        UCHAR   GetSet[1];//0x00:Get 0x01:Get/Set
//        UCHAR   DefaultValue[4];
//        UCHAR   CurrnetValue[4];
//        UCHAR   FormFlag[1];
}MTPDeviceProp_DeviceIcon;


// 0xD407 : PerceivedDeviceType
typedef struct {
        UCHAR   DevicePropCode[2];
        UCHAR   DataType[2];
        UCHAR   GetSet[1];//0x00:Get 0x01:Get/Set
        UCHAR   DefaultValue[4];
        UCHAR   GroupCode[4];
        UCHAR   FormFlag[1];
}MTPDeviceProp_PerceiveDeviceType;


#endif


