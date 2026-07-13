/**
 * @file		DPS.h
 * @brief		DPS header
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef _DPS_H_
#define _DPS_H_
#include "MessageQueue.h"
#include "media_state.h"
#include "PTP.h"
#include "fj_pictbridge.h"

//file handler
#define DIR_DCIM	0xfff10000
#define DIR_MISC	0xfff20000
#define FIL_DPOF	0xfff20001
#define FILE_DREQUEST	0x0000FF11
#define FILE_DRSPONSE	0x0000FF12
#define FILE_HREQUEST	0x0000FF21
#define FILE_HRSPONSE	0x0000FF22

//job abort style
#define ABORT_IMM		0x90000000
#define ABORT_AFT		0x90010000

//operationType
#define DSC_REQUEST			0x0000FF11
#define DSC_RESPONSE		0x0000FF12
#define PRT_REQUEST			0x0000FF21
#define PRT_RESPONSE		0x0000FF22
#define DSC_SET				0x10
#define PRT_SET				0x20
#define REQUEST_SET			0x01
#define RESPONSE_SET		0x02



#define DPS_BUFFER_SIZE		    1024
#define NUMBER_OF_PAPERSIZE		35
#define	NUMBER_OF_LAYOUTS		253
#define NUMBER_OF_QUALITIES     4
#define NUMBER_OF_PAPERTYPE     4
#define NUMBER_OF_FILETYPE      31
#define NUMBER_OF_DATEPRINT     3
#define NUMBER_OF_FILENAMEPRINT 3
#define NUMBER_OF_IMAGEOPTIMIZE 3
#define NUMBER_OF_PAPERFIXSIZE  19
#define NUMBER_OF_CROPPING      3

#define SIZE_OF_VENDORNAMESTR   66
#define SIZE_OF_VSPECIFICVERION 10
#define SIZE_OF_PRODUCTNAME	    66

#define RESULT_OK			            0
#define RESULT_NOT_EXECUTED		        1
#define RESULT_OPERATION_NOT_SUPPORTED	2
#define RESULT_OPERATION_NOT_RECOGNIZED	3

//operationState
#define WAIT_REQUEST			1
#define WAIT_PRT_RESPONSE		2
#define WAIT_DSC_RESPONSE		3
#define GETTING_CAPABILITY		4
#define CONFIG_SERVICE		    5

#define OPERATION_START			1
#define OPERATION_END			2

#define PRINT_REQUEST_END		    6+1
#define PRINT_RESPONSE_END		    10+1
#define PRINT_REQUEST_START		    1
#define PRINT_RESPONSE_START		7
#define NUMBER_OF_REQUEST_OPERATION	6

//CopyString headerType
#define	NO_HEADER	0
#define ARROW2		0x90	// <xxx>
#define S_ARROR2	0xd0	// </xxx>
#define ARROR2_S	0xb0	// <xxx/>
#define ARROW2_R	0x98	// <xxx>\n
#define S_ARROR2_R	0xd8	// </xxx>\n
#define ARROR2_S_R	0xb8	// <xxx/>\n

//operationID
#define DPS_GETFILEID			    0x01
#define DPS_NOTIFYJOBSTATUS			0x02
#define DPS_NOTIFYDEVICESTATUS		0x03
#define DPS_CONFIGUREPRINTSERVICE	0x04
#define DPS_GETCAPABILITY			0x05
#define DPS_GETJOBSTATUS			0x06
#define DPS_GETDEVICESTATUS			0x07
#define DPS_STARTJOB				0x08
#define DPS_ABORTJOB				0x09
#define DPS_CONTINUEJOB				0x0a

#define	GETDIR					0x0001
#define	GETOPERATIONID			0x0002
#define GETFILEID				0x0004
#define	GETBASEPATHID			0x0008
#define	GETFILEPATH				0x0010
#define	GETOFFSET				0x0020
#define	GETMAXSIZE				0x0040
#define	GETFILETYPE				0x0080
#define	GETPARENTFILEID			0x0100
#define	GETMAXNUMIDS			0x0200

//response result
#define REQUEST_OK 				0x10000000
#define REQUEST_NOTEXECUTED 	0x10010000
#define REQUEST_NOTSUPPORTED 	0x10020000
#define REQUEST_NOTRECOGNIZED	0x10030000

//parameter type
#define TYPE_C	0x01
#define	TYPE_S	0x02
#define	TYPE_L	0x03
#define	TYPE_D	0x04

//printer parameter
#define	NUMBER_OF_PRT_PARA	30

#define FILEID					 0 
#define BASEPATHID               1 
#define JOBENDREASON             2 
#define ERRORSTATUS              3 
#define ERRORREASON              4 
#define PAPERSIZES               5
#define LAYOUTS                  6
#define DPSPRINTSERVICESTATUS    7
#define PRTPID                   8
#define COPYID                   9
#define IMAGESPRINTED           10
#define PROGRESS                11
#define DISCONNECTENABLE        12
#define CAPABILITYCHANGED       13
#define NEWJOBOK                14
#define PRINTSERVICEAVAILABLE   15
#define DATEPRINTS              16
#define FILENAMEPRINTS          17
#define IMAGEOPTIMIZES          18
#define CROPPINGS               19
#define FILETYPES               20
#define FILEPATH                21
#define DPSVERSIONS             22
#define VENDORNAME              23
#define VENDORSPECIFICVERSION   24
#define SERIALNO                25
#define PRODUCTNAME             26
#define QUALITIES               27
#define PAPERTYPE               28
#define PAPERFIXEDSIZE          29
#define XML_COMMENT				30

//DSC parameter
#define	D_ABORTSTYLE			0 
#define	D_FIXEDSIZE				1 
#define	D_CROPPING				2 
#define	D_DATEPRINT				3 
#define	D_FILENAMEPRINT	        4 
#define	D_IMAGEOPTIMIZE	        5 
#define	D_LAYOUT				6 
#define	D_QUALITY				7 
#define	D_PAPERTYPE				8 
#define	D_PAPERSIZE				9 
#define	D_FILETYPE				10
#define	D_FILEID				11
#define	D_COPYID				12
#define	D_COPIES				13
#define	D_PRTPID				14
#define	D_CROPPINGAREA	        15
#define	D_FILEPATH				16
#define	D_FILENAME				17
#define	D_DATE					18
#define	D_DPSVERSIONS			19
#define	D_VENDORNAME			20
#define	D_VENDORSPECIFICVERSION 21
#define	D_PRODUCTNAME			22
#define	D_SERIALNO				23

//abortStyle
#define ABORT_IMM	0x90000000
#define	ABORT_AFT	0x90010000

//newJobOK
#define	NEWJOB_NOTOK	0x76000000
#define	NEWJOB_OK		0x76010000	

//capabilityChanged
#define	CAP_NOCHANGE	0x75000000
#define	CAP_CHANGED		0x75010000

//print mode
#define PRINT_SINGLE		0x01
#define PRINT_ALL			0x02
#define PRINT_MULTI			0x03
#define PRINT_ALL_INDEX		0x04
#define PRINT_DPOF			0x05

//dps key action
#define	DPS_LINKED					0x01
#define RCV_DPS_GETFILEID			0x02
#define RCV_DPS_NOTIFYJOBSTATUS		0x03
#define RCV_DPS_NOTIFYDEVICESTATUS	0x04
#define UNRECOGNIZED_OPERATION		0x05

// dpsPrintServiceStatus
#define PRINT_PRINTING				(0x70000000)
#define PRINT_IDLE					(0x70010000)
#define PRINT_PAUSED				(0x70020000)

//  JobEndReason
#define JOBEND_REASON_NOT_ENDED		(0x71000000)
#define JOBEND_REASON_NORMALLY		(0x71010000)
#define JOBEND_REASON_ABORT9000		(0x71020000)
#define JOBEND_REASON_ABORT9001		(0x71030000)
#define JOBEND_REASON_OTHERS		(0x71040000)

//  errorStatus
#define ERROR_STATUS_NONE			(0x72000000)
#define ERROR_STATUS_WARNING		(0x72010000)
#define ERROR_STATUS_ERROR			(0x72020000)

//  errorReason
#define ERROR_REASON_NONE			(0x73000000)
#define ERROR_REASON_PAPER			(0x73010000)
#define ERROR_REASON_INK			(0x73020000)
#define ERROR_REASON_HARDWARE		(0x73030000)
#define ERROR_REASON_FILE			(0x73040000)

//USB protocol
#define	MAS_MODE		0x1
#define PTP_MODE		0x2

#define max_marked_files	1200

typedef struct {
	SHORT		file_cnt;	/* Modified variable type from CHAR to SHORT by Andy Jing 070306 */		// at most 32767 files 
	//SHORT		dir_no[max_marked_file];
	//SHORT		file_no[max_marked_file];
	//CHAR		isJPG[max_marked_file];
	//CHAR        mark[max_marked_file];
	LONG		FATData[max_marked_files];
	ImagePrintInfo	PrintInfo[max_marked_files];
	//SHORT		Media_Status[max_marked_file];
} Marked_File_DPS;

typedef struct {
	CHAR operationString[26];
	USHORT operationId;
} OPSTRINGS;

typedef struct {
	CHAR 	paraString[24];
	CHAR 	type;
	CHAR 	quantity;
	USHORT 	saveOffset;
} PRTPARASTRING;

typedef struct {
	ULONG paperSize;
	ULONG layout[NUMBER_OF_LAYOUTS];
} PAPERLAYOUTCAP;
	
typedef struct {
	ULONG	fileID;
	ULONG 	basePathID;
	ULONG 	jobEndReason;
	ULONG	errorStatus;
	ULONG	errorReason;
//	PAPERLAYOUTCAP paperLayoutCap[NUMBER_OF_PAPERSIZE];
	ULONG   paperSize[NUMBER_OF_PAPERSIZE];
	ULONG   layout[NUMBER_OF_PAPERSIZE][NUMBER_OF_LAYOUTS];
	ULONG	dpsPrintServiceStatus;
	USHORT	prtPID;
	USHORT	copyID;
	USHORT	imagesPrinted;
	USHORT	progress[2];
	UCHAR	disconnectEnable;
	UCHAR	capabilityChanged;
	UCHAR	newJobOK;
	UCHAR 	printServiceAvailable;
	ULONG	datePrints[NUMBER_OF_DATEPRINT];
	ULONG	fileNamePrints[NUMBER_OF_FILENAMEPRINT];
	ULONG	imageOptimizes[NUMBER_OF_IMAGEOPTIMIZE];
	ULONG	croppings[NUMBER_OF_CROPPING];
	ULONG   fileTypes[NUMBER_OF_FILETYPE];
	CHAR	filePath[66];
	CHAR	dpsVersions[2][10];
	CHAR	vendorName[66];
	CHAR	vendorSpecificVersion[10];
	CHAR	serialNo[66];
	CHAR 	productName[66];
	ULONG   qualities[NUMBER_OF_QUALITIES];
	ULONG   paperType[NUMBER_OF_PAPERSIZE][NUMBER_OF_PAPERTYPE];
	//ULONG   paperType[NUMBER_OF_PAPERTYPE];
	ULONG   paperFixedSize[NUMBER_OF_PAPERFIXSIZE];
	UCHAR	supportIndexPrint;
	UCHAR  	supportNLayout;
	UCHAR 	indexPrint[NUMBER_OF_PAPERSIZE];
	UCHAR 	borderlessPrint[NUMBER_OF_PAPERSIZE];
	UCHAR	borderlessOff[NUMBER_OF_PAPERSIZE];
} PRTPARAVALUE;

typedef struct {
	ULONG	abortStyle;
	ULONG	fixedSize;
	ULONG	cropping;	
	ULONG   datePrint;
	ULONG   fileNamePrint;
	ULONG   imageOptimize;
	ULONG   layout;
	ULONG   quality;
	ULONG   paperType;
	ULONG   paperSize;
	ULONG   fileType;
	ULONG	fileID;
	USHORT	copyID;
	USHORT  copies;
	USHORT  prtPID;
	USHORT  croppingArea[4];
	CHAR	filePath[66];
	CHAR 	fileName[26];
	CHAR	date[26];		
	CHAR 	dpsVersions[10];
	CHAR   	vendorName[SIZE_OF_VENDORNAMESTR];
	CHAR	vendorSpecificVersion[SIZE_OF_VSPECIFICVERION];
	CHAR 	productName[SIZE_OF_PRODUCTNAME];
	CHAR	serialNo[66];
} DSCPARAVALUE;	

typedef struct {
 UCHAR lockOutputBuf;
 UCHAR lockInputBuf;
 UCHAR operationId;
 UCHAR waitState;
 UCHAR waitState2;
 UCHAR printerError;
 UCHAR capabilityChanged;
 USHORT dscRequestFileSize;
 USHORT dscResponseFileSize;
 USHORT inFileSize;
 USHORT DPSKeyAction;
 USHORT DPSEntryUIMode;
 ULONG outOperationType;
 ULONG inOperationType; 
 ULONG operationResult;
 ULONG dpsPrintServiceStatus;
 ULONG dpsErrorStatus;
 ULONG dpsErrorReason;
 ULONG dpsjobEndReason;
 UCHAR dpsDisconnctEnabled;
 UCHAR dpsCapabilityChanged;
 UCHAR dpsNewJobOK; 
 USHORT dpsCurrentPage;
 USHORT dpsTotalPage;
} DPSSTATUS; 

typedef	struct{
		USHORT	qualityLevel;//DPS_PC
		USHORT  DPSindex;
		
		USHORT  DPSPrintNumber;
		USHORT  DPSPrintFileName;
		USHORT  DPSPrintDate;
		
		USHORT  DPSPaperSetupBorderless;
		
		}DPS_mode ;

extern USHORT ResponseGetFileID(VOID);
extern USHORT ResponseNotifyJobStatus(VOID);
extern USHORT ResponseNotifyDeviceStatus(VOID);
extern USHORT RequestConfigurePrintService(VOID);
extern UCHAR   GetNecessaryCapability(VOID);
extern USHORT RequestGetJobStatus(VOID);
extern USHORT RequestGetDeviceStatus(VOID);
extern USHORT RequestStartJob(VOID);
extern USHORT RequestStartJobDpof(VOID);
extern USHORT RequestAbortJob(VOID);
extern USHORT Asyn_RequestAbortJob(VOID);
extern USHORT RequestContinueJob(VOID);
extern USHORT Asyn_RequestContinueJob(VOID);
extern VOID SetAbortType(ULONG type);
extern VOID SetAbortType(ULONG type);
extern USHORT PrintByDPOF(VOID);
extern UCHAR DpsLinked(VOID);
extern PRTPARAVALUE prtParaValue;
extern DSCPARAVALUE dscParaValue;
extern DPSSTATUS dpsStatus;
extern ULONG ProcessDPSEvent(VOID);
extern CHAR GetDpsInputBuf(VOID);
extern UCHAR GetPaperSizeIndex(ULONG paperSize);
VOID   SortNumber(LONG *a,SHORT number);
SHORT  DpsSubsystemInit(FJ_USB_MODE mode);
SHORT  DpsSubsystemEnd(VOID);
VOID   AppDPSEvent(MSG msg);
ULONG  UpdateDeviceStatus(VOID);
ULONG  UpdateJobStatus(VOID);
UCHAR  GetDpsPrinterStatus(FJ_DPS_DEVICE_STATUS* pDpsDeviceStatus);
UCHAR  GetPrinterCapability(FJ_PICT_CAPABILITY_PARAM uCapabilityId, ULONG* Buffer, ULONG uPaperSize);
UCHAR  ConfigPrinterCapability(FJ_CAPABILITY_PROPERTY* pNewCapabilityProperty);
UCHAR  AddPrintedImage(FJ_PRINT_IMAGE_PROPERTY* pPrintImageProperty);
UCHAR  SetCBFunctionEntry(FJ_DPS_CB_FUNCTION_ID uCbFuncID, lpfnDpsCbFuncEntry fnDpsCbFuncEntry);
UCHAR  SetPICTConfigString(FJ_PRMID_PICT_CONFIGSTRING wParmID, UCHAR* bBuffer, UINT32 dwSize );
UCHAR  InitDataCache(VOID);
extern UCHAR InitThumbnailDataCache(VOID);
extern VOID InitDpsStatus(VOID);
extern VOID wait_Ptp_initialized(VOID);
#endif
