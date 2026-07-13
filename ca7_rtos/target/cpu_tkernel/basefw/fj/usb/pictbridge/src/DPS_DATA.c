/**
 * @file		DPS_DATA.c
 * @brief		DPS DATA Module
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

//#pragma arm section rwdata="BASEFW_COMM_INIT"
//#pragma arm section rodata="BASEFW_COMM_CONST"
//#pragma arm section code="BASEFW_COMM_CODE"

#include "ddim_typedef.h"	
#include "Global.h"

#include "mwusb_driver4.h"
#include "PTP.h"
#include "DPS.h" 


CHAR dpsInBuffer[DPS_BUFFER_SIZE]; 
CHAR dscReqBuf[2*1024];
CHAR dscResBuf[DPS_BUFFER_SIZE];

CHAR line1String[] = "<?xml version=\"1.0\"?>\r\n \0";
CHAR line2String[] = "<dps xmlns=\"http://www.cipa.jp/dps/schema/\">\r\n \0";
CHAR line3String[] = "<output>\r\n \0";
CHAR line4String[] = "</output>\r\n \0";
CHAR line6String[] = "<input>\r\n \0";
CHAR line7String[] = "</input>\r\n \0";
CHAR line5String[] = "</dps>\r\n \0";
CHAR capabilityString[] = "capability\0";
CHAR jobConfigString[] = "jobConfig\0";
CHAR printInfoString[] = "printInfo\0";
CHAR stringResult[] = "result\0";
CHAR *stringResultPtr = stringResult;
ULONG resultCodeArray[4] = {0x10000000,0x10010000,0x10020000,0x10030000};

PRTPARAVALUE prtParaValue;
DPSSTATUS dpsStatus;

OPSTRINGS opString[] = {
	{"",0},
	{"getFileID\0",			DPS_GETFILEID			},		
	{"notifyJobStatus\0",		DPS_NOTIFYJOBSTATUS		},
	{"notifyDeviceStatus\0",	DPS_NOTIFYDEVICESTATUS		},
	{"configurePrintService\0",	DPS_CONFIGUREPRINTSERVICE	},
	{"getCapability\0",		DPS_GETCAPABILITY		},
	{"getJobStatus\0",		DPS_GETJOBSTATUS		},
	{"getDeviceStatus\0",		DPS_GETDEVICESTATUS		},
	{"startJob\0",			DPS_STARTJOB			},
	{"abortJob\0",			DPS_ABORTJOB			},
	{"continueJob\0",		DPS_CONTINUEJOB			}
};

PRTPARASTRING prtParaString[] = {
	{"fileID\0",			TYPE_L,  1,   0},//0 
	{"basePathID\0",		TYPE_L,  1,   4},//1 
	{"jobEndReason\0",		TYPE_L,  1,   8},//2 
	{"errorStatus\0",		TYPE_L,  1,  12},//3 
	{"errorReason\0",		TYPE_L,  1,  16},//4 
	{"paperSizes\0",		TYPE_L,  1,  20},//5 
	{"layouts\0",			TYPE_L,  1, 160},//6 
	{"dpsPrintServiceStatus\0",	TYPE_L,  1,35580},//7 
	{"prtPID\0",			TYPE_D,  1,35584},//8 
	{"copyID\0",			TYPE_D,  1,35586},//9 
	{"imagesPrinted\0",		TYPE_D,  1,35588},//10
	{"progress\0",			TYPE_D,  2,35590},//11
	{"disconnectEnable\0",		TYPE_L,  1,35594},//12
	{"capabilityChanged\0",		TYPE_L,  1,35595},//13
	{"newJobOK\0",			TYPE_L,  1,35596},//14
	{"printServiceAvailable\0",	TYPE_L,  1,35597},//15
	{"datePrints\0",		TYPE_L,  1,35600},//16
	{"fileNamePrints\0",		TYPE_L,  1,35612},//17
	{"imageOptimizes\0",		TYPE_L,  1,35624},//18
	{"croppings\0",			TYPE_L,  1,35636},//19
	{"fileTypes\0",			TYPE_L,  1,35650},//20
	{"filePath\0",			TYPE_C,  1,35772},//21
	{"dpsVersions\0",		TYPE_C,  1,35838},//22
	{"vendorName\0",         	TYPE_C,  1,35858},//23
	{"vendorSpecificVersion\0", 	TYPE_C,  1,35924},//24
	{"serialNo\0",           	TYPE_C,  1,35934},//25
	{"productName\0",        	TYPE_C,  1,36000},//26
	{"qualities\0",        	TYPE_L,  1,36068},//27 Andy Jing
	{"paperTypes\0",        TYPE_L,  1,36084},//28 Andy Jing
	{"fixedSizes\0",        TYPE_L,  1,/*36100*/36644},//29 Andy Jing
	{"!--\0",        		TYPE_L,  1,0} 	  //30
};

PRTPARASTRING dscParaString[] = {
	{"abortStyle",			TYPE_L,	1,0},
	{"fixedSize",			TYPE_L,	1,4},
	{"cropping",			TYPE_L,	1,8},
	{"datePrint",			TYPE_L,	1,12},
	{"fileNamePrint",		TYPE_L,	1,16},
	{"imageOptimize",		TYPE_L,	1,20},
	{"layout",			TYPE_L,	1,24},
	{"quality",			TYPE_L,	1,28},
	{"paperType",			TYPE_L,	1,32},
	{"paperSize",			TYPE_L,	1,36},
	{"fileType",			TYPE_L,	1,40},
	{"fileID",			TYPE_L,	1,44},
	{"copyID",			TYPE_D, 1,48},
	{"copies",			TYPE_D,	1,50},
	{"prtPID",			TYPE_D,	1,52},
	{"croppingArea",		TYPE_S,	4,54},
	{"filePath",			TYPE_C,	1, 62},	//66
	{"fileName",			TYPE_C,	1,128},	//26
	{"date",			TYPE_C,	1,154},	//26
	{"dpsVersions",			TYPE_C,	1,180},	//10
	{"vendorName",			TYPE_C,	1,190},	//66
	{"vendorSpecificVersion",	TYPE_C,	1,256},	//10
	{"productName",			TYPE_C,	1,266},	//66
	{"serialNo",			TYPE_C,	1,332}	//66
};	


DSCPARAVALUE dscParaValue = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 				//LONG data
	0, 0, 0, {0, 0, 0, 0},								//SHORT data
	"../DCIM/100FUJIT/MLNA0001.JPG\0",					//filePath
	"MLNA0001.JPG\0",									//fileName
	"2005/06/01\0",										//date
	"1.0\0",											//dpsVersions
	"Socionext Inc.\0",							//vendorName
	"1.0\0",											//vendorSpecificVersion
	"Socionext DSC\0",									//productName
	"00000000001\0",									//serial No
};

DEVICE_INFO DeviceInfo={
#if 0 // Big endian
    {0,0x64},				//StandardVersion
    {0,0,0,0},			//VendorExtensionID
    {0,0},				//VendorExtensionVersion
    
//    0,//paul 20060406
	0,				//VendorExtersionDesc
    //{'F',0,'U',0,'J',0,'I',0,'T',0,'S',0,'U',0,' ',0,'P',0,'T',0,'P',0,' ',0,
    // 'E',0,'x',0,'t',0,'e',0,'n',0,'s',0,'i',0,'o',0,'n',0,'s',0,
    // 0,0},
    
    {0,0},				//FunctionMode
    
    {0,0,0,14},			//OperationsSupported
    {
		0x10,0x01, 0x10,0x02, 0x10,0x03, 0x10,0x04, 0x10,0x05, 0x10,0x06,
		0x10,0x07, 0x10,0x08, 0x10,0x09, 0x10,0x0A, 0x10,0x0C, 0x10,0x0D,
		0x10,0x1B, 0x00,0x00,
	},

    
	{0,0,0,1},			//EventsSupported
    {0x40,0x09},//0x03,0x40,0x04,0x40,0x05,0x40,0x07,0x40,0x09,0x40,
    
	{0,0,0,0},			//DevicePropertiesSupported
    /*0x03,0x50,*///0x04,0x50,0x08,0x50,0x0c,0x50,0x1E,0x50,	//paul for Altek Mars Vista Command
    {0,0,0,0},			//CaptureFormats
    //,
    //3,0,0,0,			//ImageFormats : Association/DPOF/JPEG(EXIF)
    //0x01,0x30, 0x06,0x30, 0x01,0x38,

//paul PTP begin
	{0,0,0,0x0b},			//ImageFormats(Num) 11
	{
		0x30, 0x00, 0x30, 0x04, 0x30, 0x0A, 0x30, 0x08, 0x30, 0x01, 0x38, 0x01,//Undefined/TXT/AVI/WAV/Association/JPEG(EXIF)
		0x38, 0x04, 0x30, 0x05, 0x30, 0x0b, 0x30, 0x09, 0x30, 0x02,///BMP/HTML/MPEG/MP3/Script(DPS,...)
	},

//	2,0,0,0,			//ImageFormats : Association/JPEG(EXIF)
//	0x01,0x30, 0x01,0x38,
//paul PTP end

	PTP_VENDOR_NAME_STRING_LEN,	//Manufacturer
//	{'F',0,'o',0,'x',0,'c',0,'o',0,'n',0,'n',0,' ',0,'C',0,'o',0,
//	'm',0,'p',0,'a',0,'n',0,'y',0,' ',0,0,0},
	FJ_CUSTOM_USB_PTP_MANUFACTURER_STR,
	
	PTP_MODEL_STRING_LEN,	//Model
//	{'S',0,'O',0,'A',0,'3',0,'5',0,'0',0,' ',0,'D',0,'S',0,'C',0,' ',0,' ',0,
//	' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,0,0},
	FJ_CUSTOM_USB_PTP_MODEL_STR,
     
	0,	//DeviceVersion
	//{'1',0,'.',0,'0',0,0,0},

	0,//PTP_SERIAL_NUMBER_LEN,	//SerialNumber
	//{'0',0,'1',0,'2',0,'3',0,'4',0,'5',0,'6',0,'7',0,'8',0,'9',0,' ',0,' ',0,0,0},
#else
    {100,0},				//StandardVersion
    {0,0,0,0},			//VendorExtensionID
    {0,0},				//VendorExtensionVersion
    
//    0,//paul 20060406
	0,				//VendorExtersionDesc
    //{'F',0,'U',0,'J',0,'I',0,'T',0,'S',0,'U',0,' ',0,'P',0,'T',0,'P',0,' ',0,
    // 'E',0,'x',0,'t',0,'e',0,'n',0,'s',0,'i',0,'o',0,'n',0,'s',0,
    // 0,0},
    
    {0,0},				//FunctionMode
    
    {14,0,0,0},			//OperationsSupported
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
		0x0C,0x10, 
		0x0D,0x10, 
		0x1B,0x10, 
		0x00,0x00, 
	},
    
	{1,0,0,0},			//EventsSupported
    {0x09,0x40},//0x03,0x40,0x04,0x40,0x05,0x40,0x07,0x40,0x09,0x40,
    
	{0,0,0,0},			//DevicePropertiesSupported
    /*0x03,0x50,*///0x04,0x50,0x08,0x50,0x0c,0x50,0x1E,0x50,	//paul for Altek Mars Vista Command
    {0,0,0,0},			//CaptureFormats
    //,
    //3,0,0,0,			//ImageFormats : Association/DPOF/JPEG(EXIF)
    //0x01,0x30, 0x06,0x30, 0x01,0x38,

//paul PTP begin
	{11,0,0,0},			//ImageFormats(Num) 11
	{
		0x00, 0x30, 
		0x04, 0x30, 
		0x0A, 0x30, 
		0x08, 0x30, 
		0x01, 0x30, 
		0x01, 0x38, //Undefined/TXT/AVI/WAV/Association/JPEG(EXIF)
		0x04, 0x38, 
		0x05, 0x30, 
		0x0b, 0x30, 
		0x09, 0x30, 
		0x02, 0x30, ///BMP/HTML/MPEG/MP3/Script(DPS,...)
	},

//	2,0,0,0,			//ImageFormats : Association/JPEG(EXIF)
//	0x01,0x30, 0x01,0x38,
//paul PTP end

	PTP_VENDOR_NAME_STRING_LEN,	//Manufacturer
//	{'F',0,'o',0,'x',0,'c',0,'o',0,'n',0,'n',0,' ',0,'C',0,'o',0,
//	'm',0,'p',0,'a',0,'n',0,'y',0,' ',0,0,0},
	FJ_CUSTOM_USB_PTP_MANUFACTURER_STR,
	
	PTP_MODEL_STRING_LEN,	//Model
//	{'S',0,'O',0,'A',0,'3',0,'5',0,'0',0,' ',0,'D',0,'S',0,'C',0,' ',0,' ',0,
//	' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,0,0},
	FJ_CUSTOM_USB_PTP_MODEL_STR,
     
	0,	//DeviceVersion
	//{'1',0,'.',0,'0',0,0,0},

	0,//PTP_SERIAL_NUMBER_LEN,	//SerialNumber
	//{'0',0,'1',0,'2',0,'3',0,'4',0,'5',0,'6',0,'7',0,'8',0,'9',0,' ',0,' ',0,0,0},
#endif
};

STORAGE_INFO StorageInfo={
    {0x03,0x00},//paul MD StorageType (3 = Fixed RAM)
    {0x03,0x00},//FilesystemType (3 = DCF)
//    1,0,//AccessCapability (1 = Read-Only without Object Deletion)
    {0,0},//AccessCapability (0 = Read-Write)
    {0,0,0,0,0,0,0,0},//MaxCapacity   <<<============
    //0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,//MaxCapacity   <<<============
    {0,0,0,0,0,0,0,0},//FreeSpaceinBytes   <<<==========
    //0,0,0,0,//FreeSpaceInImages  <<<============
    {0xFF,0xFF,0xFF,0xFF},//FreeSpaceInImages  <<<============
    FJ_CUSTOM_USB_PTP_DESCRIPTION_LEN_NF,//StorageDescription
    0 //VolumeLabel
};

STORAGE_INFO StorageInfo_SD={
    {0x04,0x00},//StorageType (4 = Removable RAM)
    {0x03,0x00},//FilesystemType (3 = DCF)
//    1,0,//AccessCapability (1 = Read-Only without Object Deletion)
    {0,0},//AccessCapability (0 = Read-Write)
    {0,0,0,0,0,0,0,0},//MaxCapacity   <<<============
    //0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,//MaxCapacity   <<<============
    {0,0,0,0,0,0,0,0},//FreeSpaceinBytes   <<<==========
    //0,0,0,0,//FreeSpaceInImages  <<<============
    {0xFF,0xFF,0xFF,0xFF},//FreeSpaceInImages  <<<============
    FJ_CUSTOM_USB_PTP_DESCRIPTION_LEN_MS,//StorageDescription Len
    0 //VolumeLabel
};

STORAGE_INFO StorageInfo_MS={
    {0x04,0x00},//StorageType (4 = Removable RAM)
    {0x03,0x00},//FilesystemType (3 = DCF)
//    1,0,//AccessCapability (1 = Read-Only without Object Deletion)
    {0,0},//AccessCapability (0 = Read-Write)
    {0,0,0,0,0,0,0,0},//MaxCapacity   <<<============
    //0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,//MaxCapacity   <<<============
    {0,0,0,0,0,0,0,0},//FreeSpaceinBytes   <<<==========
    //0,0,0,0,//FreeSpaceInImages  <<<============
    {0xFF,0xFF,0xFF,0xFF},//FreeSpaceInImages  <<<============
    FJ_CUSTOM_USB_PTP_DESCRIPTION_LEN_SD,//StorageDescription Len
    0 //VolumeLabel
};

OBJECT_INFO DPSDiscovery_ObjectInfo={
    {0,0x01,0,0x01},//StorageID
    {0x30,0x02},//ObjectFormat<<<<<<<<<<
    {0,0},//ProtectionStatus
    {0,0,0,0},//OjbectCompressedSize<<<<<<<<<<
    {0,0},//ThumbFormat
    {0,0,0,0},//ThumbCompressedSize
    {0,0,0,0},//ThumbPixWidth
    {0,0,0,0},//ThumbPixHeight
    {0,0,0,0},//ImagePixWidth
    {0,0,0,0},//ImagePixHeight
    {0,0,0,0},//ImageBitDepth
    {0,0,0,0},//ParentObject
    {0,0},//AssociationType
    {0,0,0,0},//AssociationDesc
    {0,0,0,0},//SequenceNumber
    
    {
		13,//file name len
		'D',0,'D',0,'I',0,'S',0,'C',0,'V',0,'R',0,'Y',0,'.',0,'D',0,
		'P',0,'S',0,0,0,//FileName
		0,//CaptureDate
		0,//ModificationDate
		0 //Keywords
		,0//fill<<<<<<<<<
	}
};

OBJECT_INFO ObjectInfo={
    {0,0x01,0,0x01},//StorageID
    {0x30,0x00},//paul 20060411 default to Undefined
    //{0x01,0x30},//ObjectFormat
    {0,0},//ProtectionStatus
    {0,0,0,0},//OjbectCompressedSize
    {0,0},//ThumbFormat
    {0,0,0,0},//ThumbCompressedSize
    {0,0,0,0},//ThumbPixWidth
    {0,0,0,0},//ThumbPixHeight
    {0,0,0,0},//ImagePixWidth
    {0,0,0,0},//ImagePixHeight
    {0,0,0,0},//ImageBitDepth
    {0,0,0,0},//ParentObject
    {0,0},//AssociationType
    {0,0,0,0},//AssociationDesc
    {0,0,0,0},//SequenceNumber
    
    {
		5,//file name len
		'M',0,'I',0,'S',0,'C',0,//FileName
		0,//CaptureDate
		0,//ModificationDate
		0 //Keywords
	}
};

DeviceProp_ImageSize DevicePropImageSize={
    {0x50,0x03},//DevicePropCode
    {0x00,0x02},//DataType: UINT8
    {0x01},//Get/Set
    {0x00},//Factory Default
    {0x00},//Current Value
    {0x02},//Form Flag
    {0x00,0x06},//3 //6 items
    {0x00},//3072*2304
    {0x01},//3072*2048(3:2)
    {0x02},//2576*1932
    {0x03},//2048*1536
    {0x04},//1600*1200
    {0x05}//640*480
};

DeviceProp_CompressionSetting DevicePropCompressionSetting={
    {0x50,0x04},//DevicePropCode
    {0x00,0x02},//DataType: UINT8
    {0x01},//Get/Set
    {0x00},//Factory Default
    {0x00},//Current Value
    {0x02},//Form Flag
    {0x00,0x12},//18 items
    {0x00},//Best
    {0x01},//Better
    {0x02},//Good
    {0x03},//Best
    {0x04},//Better
    {0x05},//Good
    {0x06},//Best
    {0x07},//Better
    {0x08},//Good
    {0x09},//Best
    {0x0A},//Better
    {0x0B},//Good
    {0x0C},//Best
    {0x0D},//Better
    {0x0E},//Good
    {0x0F},//Best
    {0x10},//Better
    {0x11}//Good
};

DeviceProp_FocalLength DevicePropFocalLength={
    {0x50,0x08},//DevicePropCode
    {0x00,0x06},//DataType: UINT32
    {0x01},//Get/Set
    {0x00,0x00,0x00,0x00},//Factory Default
    {0x00,0x00,0x00,0x00},//Current Value
    {0x02},//Form Flag
    {0x00,0x11},//3 //17 items
    {0x00,0x00,0x01,0xCC},//460mm
    {0x00,0x00,0x01,0xFE},//510mm
    {0x00,0x00,0x02,0x30},//560mm
    {0x00,0x00,0x02,0x6C},//620mm
    {0x00,0x00,0x02,0xB2},//690mm
    {0x00,0x00,0x02,0xF8},//760mm
    {0x00,0x00,0x03,0x48},//840mm
    {0x00,0x00,0x03,0x98},//920mm
    {0x00,0x00,0x03,0xF2},//1010mm
    {0x00,0x00,0x04,0x4C},//1100mm
    {0x00,0x00,0x04,0xC4},//1220mm
    {0x00,0x00,0x05,0x46},//1350mm
    {0x00,0x00,0x05,0xDC},//1500mm
    {0x00,0x00,0x06,0x86},//1670mm
    {0x00,0x00,0x07,0x44},//1860mm
    {0x00,0x00,0x08,0x02},//2050mm
    {0x00,0x00,0x08,0xAC} //2220mm
};

DeviceProp_FlashMode DevicePropFlashMode={
    {0x50,0x0C},//DevicePropCode
    {0x00,0x04},//DataType: UINT16
    {0x01},//Get/Set
    {0x00,0x00},//Factory Default
    {0x00,0x00},//Current Value
    {0x02},//Form Flag
    {0x00,0x4},//3 //4 items
    {0x00,0x00},//Auto
    {0x00,0x01},//Off
    {0x00,0x02},//Fill
    {0x00,0x03}//Auto(Red Eye)
};

DeviceProp_IsoMode DevicePropIsoMode={
    {0x50,0x1E},//DevicePropCode
    {0x00,0x02},//DataType: UINT8
    {0x01},//Get/Set
    {0x00},//Factory Default
    {0x00},//Current Value
    {0x02},//Form Flag
    {0x00,0x04},//3 //4 items
    {0x00},//Auto
    {0x01},//ISO100
    {0x02},//ISO200
    {0x03}//ISO400
};

ULONG	FocalLength[17]={460,510,560,620,690,760,840,920,1010,1100,1220,1350,1500,1670,1860,2050,2220};
UCHAR	dirMISCname[]={'M',0,'I',0,'S',0,'C',0,0,0};
UCHAR	dirDCIMname[]={'D',0,'C',0,'I',0,'M',0,0,0};

