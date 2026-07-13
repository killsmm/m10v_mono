/**
 * @file		DPS.c
 * @brief		DPS Module
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

//#pragma arm section rwdata="BASEFW_COMM_INIT"
//#pragma arm section rodata="BASEFW_COMM_CONST"
//#pragma arm section code="BASEFW_COMM_CODE"

#include "os_user_custom.h"
#include "Global.h"
#include "mwusb_driver4.h"
#include "DPS.h"  
#include "fs_if.h"
#include "MessageQueue.h"
#include "fj_pictbridge.h"
#include "fj_custom.h"
#include "PTP.h"
#ifdef CO_DCF_EXIF_ON
#include "dcf_if.h"
#endif
#include <string.h>
#define	MAX_DPS_BUF		64*1024//64K
#define	MIN_DPS_WARPER	320//307

extern VOID SendDSPCmdFile(VOID);	
extern CHAR dpsInBuffer[1024];
extern CHAR dscReqBuf[1024];
extern CHAR dscResBuf[DPS_BUFFER_SIZE];//1024
CHAR *dpsInBufferPtr = dpsInBuffer;
CHAR *dscReqBufPtr = dscReqBuf;
CHAR *dscResBufPtr = dscResBuf;
Marked_File_DPS dps_MarkFile; 
extern CHAR line1String[];
extern CHAR line2String[];
extern CHAR line3String[];
extern CHAR line4String[];
extern CHAR line6String[];
extern CHAR line7String[];
extern CHAR line5String[];
extern CHAR capabilityString[];
extern CHAR jobConfigString[];
extern CHAR printInfoString[];
extern CHAR stringResult[];
extern CHAR *stringResultPtr;
extern ULONG resultCodeArray[4];
ULONG *resultCodeArrayPtr = resultCodeArray;


extern OPSTRINGS opString[];

extern PRTPARASTRING prtParaString[];

extern PRTPARASTRING dscParaString[];

PRTPARAVALUE *prtParaValuePtr = &prtParaValue;
extern DSCPARAVALUE dscParaValue;


DSCPARAVALUE *dscParaValuePtr = &dscParaValue;
static FJ_DPS_DATE_FORMAT DPSdateType = FJ_DPS_DATE_FORMAT_TYPE0;
#ifdef CO_DCF_EXIF_ON
static lpfnDpsCbFuncEntry aryfnDpsCbFuncEntry[FJ_DPS_CB_FUNCTION_SIZE];
#endif
DPS_mode DPSselection; 

extern UCHAR vSessionOpen;

UINT32	needCheckJobAgain=0;//Need again start job to continue endless job
							//if needCheckJobAgain had index of pictureDatabase.

UCHAR	InDpsStarting=0;			
UCHAR	GetCapabilityEntryCnt=0;	
UCHAR	needShowCapabilityChange=0;
UCHAR	collisionCnt=0;
UCHAR requeststartjob=0;
UCHAR Unrecognizedflag = 0;
extern E_BF_MEDIA_STATE_MEDIA_TYPE PictBridgeUI_MediaSelected;
extern UINT32 StorageIndicator;//paul 20060821
extern ULONG ObjectCnt[], JpgCnt[];//paul

#if 0 // RAM sharing
extern OBJECT_HANDLE_INFO Object_NF[], Object_SD[];//paul MD
extern OBJECT_HANDLE_INFO Object_DPOF;
extern OBJECT_SEARCH_TBL  ObjectSearchTbl[];
#endif // RAM sharing
extern OBJECT_HANDLE_INFO *Object;//paul MD

extern T_BF_PTP_BUFF* gBF_Ptp_Buff;

extern ULONG  ulTotalDPSDir;
CHAR *CopyString(CHAR *des,CHAR *src, CHAR stopChar, UCHAR headerType)
{
	//headerType
	//0. NORMAL
	//1. <xxx>	0x90
	//2. </xxx>	0xd0
	//3. <xxx/>	0xb0
	//4. <xxx>\n	0x98
	//5. </xxx>\n	0xd8
	//6. <xxx/>\n	0xb8
	
	if(headerType & 0x80)
		*des++ = '<';
	//if(headerType == 2 || headerType == 5)
	if(headerType & 0x40)
		*des++ = '/';
		
	while(*src != stopChar)
		*des++ = *src++;
		
	if(headerType & 0x20)
		*des++ = '/';
	if(headerType & 0x10)
		*des++ = '>';
	if(headerType & 0x08)
	{
		*des++ = '\r';
		*des++ = '\n';
		*des++ = ' ';
	}		
        return des;
}

CHAR *ResponsePara(CHAR *ptr,CHAR *string,CHAR *para, USHORT type,USHORT count)
{
        ULONG ulongPara;
        USHORT ushortPara;
    
	ptr = CopyString(ptr,string,0,ARROW2);

	switch(type)
	{
		case TYPE_L:
                	ulongPara = *(ULONG *)para;
                	sprintf(ptr,"%08lX",ulongPara);
                	ptr = ptr + 8;
			
		break;
		case TYPE_D:
                	ushortPara = *(USHORT *)para;
                	sprintf(ptr,"%03d",ushortPara);
                	ptr = ptr + 3;
		break;
		case TYPE_S:
                	ushortPara = *(USHORT *)para;
                	sprintf(ptr,"%04X",ushortPara);
                	ptr = ptr + 4;
		break;		
		case TYPE_C:
			ptr = CopyString(ptr,para,0,0);
		break;
	}
	
	
	
	ptr = CopyString(ptr,string,0,S_ARROR2_R); 	
	return ptr;	
	
}	
	
CHAR *StringIdentical(CHAR *buff,CHAR* string)
{	
	while(*string)
	{
		if(*buff++ != *string++)
			return 0;
	}
        return buff;
}	

CHAR	*DpsLocateStr(CHAR *matchStr, CHAR *srcStr)
{
//this function return the location just behind the founded string, not start of the string.
        CHAR *cmpStrPtr = matchStr;

	while(*srcStr)
	{
              	cmpStrPtr = matchStr;
		while(*srcStr++ == *cmpStrPtr++)
		{
			if(!*cmpStrPtr)
				return srcStr;
		}
	}
	return 0;
	
}

VOID DpsInitPrtParaValue(UCHAR full)
{
	if(full){
		memset((CHAR *)prtParaValuePtr, 0, sizeof(PRTPARAVALUE));
	}else{//for Bug-156 , alway keep printer status in session
		memset((CHAR *)prtParaValuePtr->paperSize, 0, sizeof(ULONG)*NUMBER_OF_PAPERSIZE);
		memset((CHAR *)prtParaValuePtr->layout, 0, sizeof(ULONG)*NUMBER_OF_PAPERSIZE*NUMBER_OF_LAYOUTS);
		memset((CHAR *)prtParaValuePtr->datePrints, 0, sizeof(ULONG)*NUMBER_OF_DATEPRINT);
		memset((CHAR *)prtParaValuePtr->fileNamePrints, 0, sizeof(ULONG)*NUMBER_OF_FILENAMEPRINT);
		memset((CHAR *)prtParaValuePtr->imageOptimizes, 0, sizeof(ULONG)*NUMBER_OF_IMAGEOPTIMIZE);
		memset((CHAR *)prtParaValuePtr->croppings, 0, sizeof(ULONG)*NUMBER_OF_CROPPING);
		memset((CHAR *)prtParaValuePtr->fileTypes, 0, sizeof(ULONG)*NUMBER_OF_FILETYPE);
		memset((CHAR *)prtParaValuePtr->qualities, 0, sizeof(ULONG)*NUMBER_OF_QUALITIES);
		memset((CHAR *)prtParaValuePtr->paperType, 0, sizeof(ULONG)*NUMBER_OF_PAPERTYPE*NUMBER_OF_PAPERSIZE);
		memset((CHAR *)prtParaValuePtr->paperFixedSize, 0, sizeof(ULONG)*NUMBER_OF_PAPERFIXSIZE);
		memset((CHAR *)prtParaValuePtr->indexPrint, 0, NUMBER_OF_PAPERSIZE);
		memset((CHAR *)prtParaValuePtr->borderlessPrint, 0, NUMBER_OF_PAPERSIZE);
		memset((CHAR *)prtParaValuePtr->borderlessOff, 0, NUMBER_OF_PAPERSIZE);
	}
}	
	
VOID DpsInitDscParaValue()
{
	//dpsVersions, vendorName, vendorSpecificVersion, productName, and serilNo
	//will NOT be cleared
	memset((CHAR *)dscParaValuePtr, 0, sizeof(DSCPARAVALUE) - 220);	
}


VOID InitDpsStatus(VOID)
{
	dpsStatus.lockOutputBuf = FALSE;
	//dpsStatus.lockInputBuf = FALSE;
	//dpsStatus.inOperationType = 0;
	//dpsStatus.outOperationType = 0;
	//dpsStatus.operationId = 0;
	dpsStatus.waitState = CONFIG_SERVICE;
	dpsStatus.waitState2 = WAIT_REQUEST;
	//dpsStatus.operationResult = 0;	
	dpsStatus.printerError = 0;	
	InDpsStarting=0;
	GetCapabilityEntryCnt = 0;
}		

CHAR	HEX2DEC(CHAR ch)
{
		if(ch>='0' && ch<='9') return ch-'0';
		if(ch>='A' && ch<='F') return ch-'A'+10;
		if(ch>='a' && ch<='f') return ch-'a'+10;
		return 0;
}

//return TRUE if found match
BOOL dataFilter(ULONG data,USHORT XMLType){
	BOOL found = FALSE;
	ULONG code;
	ULONG temp;
	temp = data;
	code = ((temp>>16)<<16);
	switch (XMLType){
		case FILETYPES:
			switch (code){
				case XML_FILETYPE_DEFULT:
				case XML_FILETYPE_EXIF_JPEG:
				case XML_FILETYPE_EXIF:	
				case XML_FILETYPE_JPEG:
				case XML_FILETYPE_TIFF_EP:
				case XML_FILETYPE_FLASHPIX:
				case XML_FILETYPE_BMP:
				case XML_FILETYPE_CIFF:
				case XML_FILETYPE_GIF:
				case XML_FILETYPE_JFIF:
				case XML_FILETYPE_PCD:
				case XML_FILETYPE_PICT:
				case XML_FILETYPE_PNG:
				case XML_FILETYPE_TIFF:
				case XML_FILETYPE_TIFF_IT:
				case XML_FILETYPE_JP2:
				case XML_FILETYPE_JPX:
				case XML_FILETYPE_UNDEF_NONIMAGE:
				case XML_FILETYPE_ASSOCIATION:
				case XML_FILETYPE_SCRIPT:
				case XML_FILETYPE_EXECUTABLE:
				case XML_FILETYPE_TEXT:
				case XML_FILETYPE_HTML:
				case XML_FILETYPE_XHTML_PRINT:
				case XML_FILETYPE_DPOF:
				case XML_FILETYPE_AIFF:
				case XML_FILETYPE_WAV:
				case XML_FILETYPE_MP3:
				case XML_FILETYPE_AVI:
				case XML_FILETYPE_MPEG:
				case XML_FILETYPE_ASF:
					found = TRUE;			
					break;
				default:
					found = FALSE;
					break;				
			}			
			break;				
		case CROPPINGS:
			switch (code){
				case XML_CROPPING_DEFAULT:
				case XML_CROPPING_OFF:
				case XML_CROPPING_ON:	
					found = TRUE;			
					break;
				default:
					found = FALSE;
					break;				
			}			
			break;			
		case FILENAMEPRINTS:
			switch (code){
				case XML_FILENAME_PRINT_DEFAULT:
				case XML_FILENAME_PRINT_OFF:
				case XML_FILENAME_PRINT_ON:					
					found = TRUE;			
					break;
				default:
					found = FALSE;
					break;				
			}			
			break;			
		case IMAGEOPTIMIZES:
			switch (code){
				case XML_IMAGEOPTIMIZE_DEFAULT:
				case XML_IMAGEOPTIMIZE_OFF:
				case XML_IMAGEOPTIMIZE_ON:	
					found = TRUE;			
					break;
				default:
					found = FALSE;
					break;				
			}			
			break;			
		case DATEPRINTS:
			switch (code){
				case XML_DATEPRINT_DEFAULT:
				case XML_DATEPRINT_OFF:
				case XML_DATEPRINT_ON:	
					found = TRUE;			
					break;
				default:
					found = FALSE;
					break;				
			}			
			break;
		case QUALITIES:
			switch (code){
				case XML_QUALITY_DEFAULT:
				case XML_QUALITY_NORMAL:
				case XML_QUALITY_DRAFT:
				case XML_QUALITY_FINE:	
					found = TRUE;			
					break;
				default:
					found = FALSE;
					break;				
			}
			break;
		case PAPERTYPE:
			switch (code){
				case XML_PAPERTYPE_DEFAULT:
				case XML_PAPERTYPE_PLAIN:
				case XML_PAPERTYPE_PHOTO:
				case XML_PAPERTYPE_FAST_PHOTO:	
					found = TRUE;			
					break;
				default:
					found = FALSE;
					break;						
			}
			break;
		case LAYOUTS:
			switch (code){
				case XML_LAYOUT_DEFAULT:
				case XML_LAYOUT_1UP:
				case XML_LAYOUT_2UP:
				case XML_LAYOUT_3UP:
				case XML_LAYOUT_4UP:
				case XML_LAYOUT_5UP:
				case XML_LAYOUT_6UP:
				case XML_LAYOUT_7UP:		
				case XML_LAYOUT_8UP:			
				case XML_LAYOUT_9UP:			
				case XML_LAYOUT_10UP:		
				case XML_LAYOUT_11UP:		
				case XML_LAYOUT_12UP:		
				case XML_LAYOUT_13UP:		
				case XML_LAYOUT_14UP:		
				case XML_LAYOUT_15UP:		
				case XML_LAYOUT_16UP:		
				case XML_LAYOUT_17UP:		
				case XML_LAYOUT_18UP:		
				case XML_LAYOUT_19UP:		
				case XML_LAYOUT_20UP:		
				case XML_LAYOUT_21UP:		
				case XML_LAYOUT_22UP:		
				case XML_LAYOUT_23UP:		
				case XML_LAYOUT_24UP:		
				case XML_LAYOUT_25UP:		
				case XML_LAYOUT_26UP:		
				case XML_LAYOUT_27UP:		
				case XML_LAYOUT_28UP:		
				case XML_LAYOUT_29UP:		
				case XML_LAYOUT_30UP:		
				case XML_LAYOUT_31UP:		
				case XML_LAYOUT_32UP:		
				case XML_LAYOUT_33UP:		
				case XML_LAYOUT_34UP:		
				case XML_LAYOUT_35UP:		
				case XML_LAYOUT_36UP:		
				case XML_LAYOUT_37UP:		
				case XML_LAYOUT_38UP:		
				case XML_LAYOUT_39UP:		
				case XML_LAYOUT_40UP:		
				case XML_LAYOUT_41UP:		
				case XML_LAYOUT_42UP:		
				case XML_LAYOUT_43UP:		
				case XML_LAYOUT_44UP:		
				case XML_LAYOUT_45UP:		
				case XML_LAYOUT_46UP:		
				case XML_LAYOUT_47UP:		
				case XML_LAYOUT_48UP:		
				case XML_LAYOUT_49UP:		
				case XML_LAYOUT_50UP:		
				case XML_LAYOUT_51UP:		
				case XML_LAYOUT_52UP:		
				case XML_LAYOUT_53UP:		
				case XML_LAYOUT_54UP:		
				case XML_LAYOUT_55UP:		
				case XML_LAYOUT_56UP:		
				case XML_LAYOUT_57UP:		
				case XML_LAYOUT_58UP:		
				case XML_LAYOUT_59UP:		
				case XML_LAYOUT_60UP:		
				case XML_LAYOUT_61UP:		
				case XML_LAYOUT_62UP:		
				case XML_LAYOUT_63UP:		
				case XML_LAYOUT_64UP:		
				case XML_LAYOUT_65UP:		
				case XML_LAYOUT_66UP:		
				case XML_LAYOUT_67UP:		
				case XML_LAYOUT_68UP:		
				case XML_LAYOUT_69UP:		
				case XML_LAYOUT_70UP:		
				case XML_LAYOUT_71UP:		
				case XML_LAYOUT_72UP:		
				case XML_LAYOUT_73UP:		
				case XML_LAYOUT_74UP:		
				case XML_LAYOUT_75UP:		
				case XML_LAYOUT_76UP:		
				case XML_LAYOUT_77UP:		
				case XML_LAYOUT_78UP:		
				case XML_LAYOUT_79UP:		
				case XML_LAYOUT_80UP:		
				case XML_LAYOUT_81UP:		
				case XML_LAYOUT_82UP:		
				case XML_LAYOUT_83UP:		
				case XML_LAYOUT_84UP:		
				case XML_LAYOUT_85UP:		
				case XML_LAYOUT_86UP:		
				case XML_LAYOUT_87UP:		
				case XML_LAYOUT_88UP:		
				case XML_LAYOUT_89UP:		
				case XML_LAYOUT_90UP:		
				case XML_LAYOUT_91UP:		
				case XML_LAYOUT_92UP:		
				case XML_LAYOUT_93UP:		
				case XML_LAYOUT_94UP:		
				case XML_LAYOUT_95UP:		
				case XML_LAYOUT_96UP:		
				case XML_LAYOUT_97UP:		
				case XML_LAYOUT_98UP:		
				case XML_LAYOUT_99UP:		
				case XML_LAYOUT_100UP:		
				case XML_LAYOUT_101UP:		
				case XML_LAYOUT_102UP:		
				case XML_LAYOUT_103UP:		
				case XML_LAYOUT_104UP:		
				case XML_LAYOUT_105UP:		
				case XML_LAYOUT_106UP:		
				case XML_LAYOUT_107UP:		
				case XML_LAYOUT_108UP:		
				case XML_LAYOUT_109UP:		
				case XML_LAYOUT_110UP:		
				case XML_LAYOUT_111UP:		
				case XML_LAYOUT_112UP:		
				case XML_LAYOUT_113UP:		
				case XML_LAYOUT_114UP:		
				case XML_LAYOUT_115UP:		
				case XML_LAYOUT_116UP:		
				case XML_LAYOUT_117UP:		
				case XML_LAYOUT_118UP:		
				case XML_LAYOUT_119UP:		
				case XML_LAYOUT_120UP:		
				case XML_LAYOUT_121UP:		
				case XML_LAYOUT_122UP:		
				case XML_LAYOUT_123UP:		
				case XML_LAYOUT_124UP:		
				case XML_LAYOUT_125UP:		
				case XML_LAYOUT_126UP:		
				case XML_LAYOUT_127UP:		
				case XML_LAYOUT_128UP:		
				case XML_LAYOUT_129UP:		
				case XML_LAYOUT_130UP:		
				case XML_LAYOUT_131UP:		
				case XML_LAYOUT_132UP:		
				case XML_LAYOUT_133UP:		
				case XML_LAYOUT_134UP:		
				case XML_LAYOUT_135UP:		
				case XML_LAYOUT_136UP:		
				case XML_LAYOUT_137UP:		
				case XML_LAYOUT_138UP:		
				case XML_LAYOUT_139UP:		
				case XML_LAYOUT_140UP:		
				case XML_LAYOUT_141UP:		
				case XML_LAYOUT_142UP:		
				case XML_LAYOUT_143UP:		
				case XML_LAYOUT_144UP:		
				case XML_LAYOUT_145UP:		
				case XML_LAYOUT_146UP:		
				case XML_LAYOUT_147UP:		
				case XML_LAYOUT_148UP:		
				case XML_LAYOUT_149UP:		
				case XML_LAYOUT_150UP:		
				case XML_LAYOUT_151UP:		
				case XML_LAYOUT_152UP:		
				case XML_LAYOUT_153UP:		
				case XML_LAYOUT_154UP:		
				case XML_LAYOUT_155UP:		
				case XML_LAYOUT_156UP:		
				case XML_LAYOUT_157UP:		
				case XML_LAYOUT_158UP:		
				case XML_LAYOUT_159UP:		
				case XML_LAYOUT_160UP:		
				case XML_LAYOUT_161UP:		
				case XML_LAYOUT_162UP:		
				case XML_LAYOUT_163UP:		
				case XML_LAYOUT_164UP:		
				case XML_LAYOUT_165UP:		
				case XML_LAYOUT_166UP:		
				case XML_LAYOUT_167UP:		
				case XML_LAYOUT_168UP:		
				case XML_LAYOUT_169UP:		
				case XML_LAYOUT_170UP:		
				case XML_LAYOUT_171UP:		
				case XML_LAYOUT_172UP:		
				case XML_LAYOUT_173UP:		
				case XML_LAYOUT_174UP:		
				case XML_LAYOUT_175UP:		
				case XML_LAYOUT_176UP:		
				case XML_LAYOUT_177UP:		
				case XML_LAYOUT_178UP:		
				case XML_LAYOUT_179UP:		
				case XML_LAYOUT_180UP:		
				case XML_LAYOUT_181UP:		
				case XML_LAYOUT_182UP:		
				case XML_LAYOUT_183UP:		
				case XML_LAYOUT_184UP:		
				case XML_LAYOUT_185UP:		
				case XML_LAYOUT_186UP:		
				case XML_LAYOUT_187UP:		
				case XML_LAYOUT_188UP:		
				case XML_LAYOUT_189UP:		
				case XML_LAYOUT_190UP:		
				case XML_LAYOUT_191UP:		
				case XML_LAYOUT_192UP:		
				case XML_LAYOUT_193UP:		
				case XML_LAYOUT_194UP:		
				case XML_LAYOUT_195UP:		
				case XML_LAYOUT_196UP:		
				case XML_LAYOUT_197UP:		
				case XML_LAYOUT_198UP:		
				case XML_LAYOUT_199UP:		
				case XML_LAYOUT_200UP:		
				case XML_LAYOUT_201UP:		
				case XML_LAYOUT_202UP:		
				case XML_LAYOUT_203UP:		
				case XML_LAYOUT_204UP:		
				case XML_LAYOUT_205UP:		
				case XML_LAYOUT_206UP:		
				case XML_LAYOUT_207UP:		
				case XML_LAYOUT_208UP:		
				case XML_LAYOUT_209UP:		
				case XML_LAYOUT_210UP:		
				case XML_LAYOUT_211UP:		
				case XML_LAYOUT_212UP:		
				case XML_LAYOUT_213UP:		
				case XML_LAYOUT_214UP:		
				case XML_LAYOUT_215UP:		
				case XML_LAYOUT_216UP:		
				case XML_LAYOUT_217UP:		
				case XML_LAYOUT_218UP:		
				case XML_LAYOUT_219UP:		
				case XML_LAYOUT_220UP:		
				case XML_LAYOUT_221UP:		
				case XML_LAYOUT_222UP:		
				case XML_LAYOUT_223UP:		
				case XML_LAYOUT_224UP:		
				case XML_LAYOUT_225UP:		
				case XML_LAYOUT_226UP:		
				case XML_LAYOUT_227UP:		
				case XML_LAYOUT_228UP:		
				case XML_LAYOUT_229UP:		
				case XML_LAYOUT_230UP:		
				case XML_LAYOUT_231UP:		
				case XML_LAYOUT_232UP:		
				case XML_LAYOUT_233UP:		
				case XML_LAYOUT_234UP:		
				case XML_LAYOUT_235UP:		
				case XML_LAYOUT_236UP:		
				case XML_LAYOUT_237UP:		
				case XML_LAYOUT_238UP:		
				case XML_LAYOUT_239UP:		
				case XML_LAYOUT_240UP:		
				case XML_LAYOUT_241UP:		
				case XML_LAYOUT_242UP:		
				case XML_LAYOUT_243UP:		
				case XML_LAYOUT_244UP:		
				case XML_LAYOUT_245UP:		
				case XML_LAYOUT_246UP:		
				case XML_LAYOUT_247UP:		
				case XML_LAYOUT_248UP:		
				case XML_LAYOUT_249UP:		
				case XML_LAYOUT_250UP:		
				case XML_LAYOUT_INDEX:	
				case XML_LAYOUT_BORDERLESS:
					found = TRUE;			
					break;
				default:
					found = FALSE;
					break;				
			}
			break;
		case PAPERSIZES:
			switch (code){
				case XML_PAPERSIZE_DEFAULT:
				case XML_PAPERSIZE_L:	
				case XML_PAPERSIZE_2L:
				case XML_PAPERSIZE_HAGAKI:	
				case XML_PAPERSIZE_CARD:
				case XML_PAPERSIZE_100X150:	
				case XML_PAPERSIZE_4X6:
				case XML_PAPERSIZE_8X10:		
				case XML_PAPERSIZE_LETTER:
				case XML_PAPERSIZE_11X17:	
				case XML_PAPERSIZE_A0:
				case XML_PAPERSIZE_A1:	
				case XML_PAPERSIZE_A2:
				case XML_PAPERSIZE_A3:	
				case XML_PAPERSIZE_A4:
				case XML_PAPERSIZE_A5:	
				case XML_PAPERSIZE_A6:
				case XML_PAPERSIZE_A7:	
				case XML_PAPERSIZE_A8:
				case XML_PAPERSIZE_A9:	
				case XML_PAPERSIZE_B0:
				case XML_PAPERSIZE_B1:	
				case XML_PAPERSIZE_B2:
				case XML_PAPERSIZE_B3:	
				case XML_PAPERSIZE_B4:
				case XML_PAPERSIZE_B5:	
				case XML_PAPERSIZE_B6:
				case XML_PAPERSIZE_B7:	
				case XML_PAPERSIZE_B8:
				case XML_PAPERSIZE_B9:	
				case XML_PAPERSIZE_89ROLL:
				case XML_PAPERSIZE_127ROLL:	
				case XML_PAPERSIZE_100ROLL:	
				case XML_PAPERSIZE_210ROLL:	
					found = TRUE;			
					break;
				default:
					found = FALSE;
					break;
			}
			break;
		case PAPERFIXEDSIZE:
			switch (code){
				case XML_FIXEDSIZE_DEFAULT:
				case XML_FIXEDSIZE_2_5X3_25:
				case XML_FIXEDSIZE_3_5X5:
				case XML_FIXEDSIZE_4X6:
				case XML_FIXEDSIZE_5X7:
				case XML_FIXEDSIZE_8X10:
				case XML_FIXEDSIZE_254X178:
				case XML_FIXEDSIZE_110X74:
				case XML_FIXEDSIZE_89X55:
				case XML_FIXEDSIZE_100X148:
				case XML_FIXEDSIZE_6X8:
				case XML_FIXEDSIZE_7X10:
				case XML_FIXEDSIZE_9X13:
				case XML_FIXEDSIZE_10X15:
				case XML_FIXEDSIZE_13X18:
				case XML_FIXEDSIZE_15X21:
				case XML_FIXEDSIZE_18X24:
				case XML_FIXEDSIZE_A4:
				case XML_FIXEDSIZE_LETTER:
					found = TRUE;			
					break;
				default:
					found = FALSE;
					break;
			}
			break;
		default:
			found = TRUE;	//no filter
			break;
	}
	return found;
}

CHAR *ReadXmlDataFilter(CHAR *data, CHAR *buff, USHORT dataType,  USHORT XMLType)
{	
//	UCHAR tempData[8]={0};
	union {
		ULONG ulValue;
		UCHAR ucValue[8];
	} tempData;
	ULONG tempValue = 0;
	ULONG totalCount = 0;
	ULONG maxCount = 0;
	BOOL overFlow=FALSE;
	
	memset(&tempData, 0, sizeof(tempData));
	
	if(*buff == '>')
		buff++;
	
	switch(dataType)
	{
		case TYPE_L:
			while(*buff!='<') {
				while(	!(*buff>='0' && *buff<='9') && 
						!(*buff>='A' && *buff<='F') &&
						!(*buff>='a' && *buff<='f')) {
						if(*buff=='>' || *buff=='<')return buff;
						buff++;
						}
//				*((ULONG *)tempData) = HEX2DEC(*buff)<<28 |
//									HEX2DEC(*(buff+1))<<24 |
//									HEX2DEC(*(buff+2))<<20 |
//									HEX2DEC(*(buff+3))<<16 |
//									HEX2DEC(*(buff+4))<<12 |
//									HEX2DEC(*(buff+5))<<8 |
//									HEX2DEC(*(buff+6))<<4 |
//									HEX2DEC(*(buff+7));
//				tempValue = *((ULONG *)tempData);

				tempData.ucValue[0] = HEX2DEC(buff[6]) << 4 | HEX2DEC(buff[7]);
				tempData.ucValue[1] = HEX2DEC(buff[4]) << 4 | HEX2DEC(buff[5]);
				tempData.ucValue[2] = HEX2DEC(buff[2]) << 4 | HEX2DEC(buff[3]);
				tempData.ucValue[3] = HEX2DEC(buff[0]) << 4 | HEX2DEC(buff[1]);
				tempValue = tempData.ulValue;

				switch(XMLType){
					/*
					case LAYOUTS:
						maxCount = NUMBER_OF_LAYOUTS;
						if (totalCount >= maxCount)
							overFlow = TRUE;						
						break;
					*/	
					case FILETYPES:
						maxCount = NUMBER_OF_FILETYPE;
						if (totalCount >= maxCount)
							overFlow = TRUE;						
						break;						
					case CROPPINGS:
						maxCount = NUMBER_OF_CROPPING;
						if (totalCount >= maxCount)
							overFlow = TRUE;						
						break;							
					case FILENAMEPRINTS:
						maxCount = NUMBER_OF_FILENAMEPRINT;
						if (totalCount >= maxCount)
							overFlow = TRUE;						
						break;						
					case IMAGEOPTIMIZES:
						maxCount = NUMBER_OF_IMAGEOPTIMIZE;
						if (totalCount >= maxCount)
							overFlow = TRUE;						
						break;							
					case DATEPRINTS:
						maxCount = NUMBER_OF_DATEPRINT;
						if (totalCount >= maxCount)
							overFlow = TRUE;						
						break;						
					case QUALITIES:
						maxCount = NUMBER_OF_QUALITIES;
						if (totalCount >= maxCount)
							overFlow = TRUE;						
						break;
					case PAPERSIZES:
						maxCount = NUMBER_OF_PAPERSIZE;
						if (totalCount >= maxCount)
							overFlow = TRUE;
						break;
					case PAPERFIXEDSIZE:
						maxCount = NUMBER_OF_PAPERFIXSIZE;
						if (totalCount >= maxCount)
							overFlow = TRUE;
						break;
					default:
						overFlow = FALSE;
						break;	
				}
				if ((!dataFilter(tempValue,XMLType))||overFlow){
					buff += 8;	//no save
				}else{
					*((ULONG *)data) =  tempValue;
					buff += 8;
					data += 4;	//want save
					totalCount++;
				}
			}
	    	break;
	    	case TYPE_D:
                        do
	    		{
	    			*((USHORT *)data) = (*buff - 0x30)*100 + (*(buff+1) - 0x30)*10 + *(buff+2) - 0x30;
	    			buff = buff + 4;
	    			data = data + 2;
	    		} while(*(buff-1) == ' ' || *(buff-1) == '/');
	    	break;
		case TYPE_S:
			do
			{
				*((USHORT *)data) = (*buff - (*buff > 0x39 ? 0x37:0x30))<<12 | (*(buff+1) - (*(buff+1) > 0x39 ? 0x37:0x30))<<8 | (*(buff+2) - (*(buff+2) > 0x39 ? 0x37:0x30))<<4 | (*(buff+3) - *(buff+3) > 0x39 ? 0x37:0x30);
	    			buff = buff + 5;
	    			data = data + 2;
	    		} while(*(buff-1) == ' ');
	    	break;	    	
	    	case TYPE_C:
	    		CopyString(data, buff, '<',0);
	    	break;
	}
        return DpsLocateStr(">\0",buff);
}


CHAR *ReadXmlData(CHAR *data, CHAR *buff, USHORT dataType)
{	
	if(*buff == '>')
		buff++;
	
	switch(dataType)
	{
		case TYPE_L:
			while(*buff!='<') {
				while(	!(*buff>='0' && *buff<='9') && 
						!(*buff>='A' && *buff<='F') &&
						!(*buff>='a' && *buff<='f')) {
						if(*buff=='>' || *buff=='<')return buff;
						buff++;
						}
				*((ULONG *)data) =  HEX2DEC(*buff)<<28 |
									HEX2DEC(*(buff+1))<<24 |
									HEX2DEC(*(buff+2))<<20 |
									HEX2DEC(*(buff+3))<<16 |
									HEX2DEC(*(buff+4))<<12 |
									HEX2DEC(*(buff+5))<<8 |
									HEX2DEC(*(buff+6))<<4 |
									HEX2DEC(*(buff+7));
				buff += 8;
				data += 4;
				}
    		
	    	break;
	    	case TYPE_D:
                        do
	    		{
	    			*((USHORT *)data) = (*buff - 0x30)*100 + (*(buff+1) - 0x30)*10 + *(buff+2) - 0x30;
	    			buff = buff + 4;
	    			data = data + 2;
	    		} while(*(buff-1) == ' ' || *(buff-1) == '/');
	    	break;
		case TYPE_S:
			do
			{
				*((USHORT *)data) = (*buff - (*buff > 0x39 ? 0x37:0x30))<<12 | (*(buff+1) - (*(buff+1) > 0x39 ? 0x37:0x30))<<8 | (*(buff+2) - (*(buff+2) > 0x39 ? 0x37:0x30))<<4 | (*(buff+3) - *(buff+3) > 0x39 ? 0x37:0x30);
	    			buff = buff + 5;
	    			data = data + 2;
	    		} while(*(buff-1) == ' ');
	    	break;	    	
	    	case TYPE_C:
	    		CopyString(data, buff, '<',0);
	    	break;
	}
        return DpsLocateStr(">\0",buff);
}

UCHAR GetPaperSizeIndex(ULONG paperSize)
{
	UCHAR i=0;
	
	while(prtParaValue.paperSize[i])
	{
		if(paperSize == prtParaValue.paperSize[i])
			return i;
		i++;	
	}
	return 0xff;
}	

VOID SortParameter(ULONG *src)
{
	USHORT ttlQty=0;	
	ULONG *tempPtr = src;
	
	while(*tempPtr++)
		ttlQty++;
		
       SortNumber((LONG *)src, ttlQty);	
}

VOID	SortNumber(
LONG	*a,
SHORT	number)
{
#if 1	// [2009/06/29] Softune Bug
UINT32	incr, i;
INT32 j;
#else
INT32	incr, i, j;	
#endif
    for (incr = (number>>1); incr > 0; incr >>= 1) {
      	for(i = incr; i < number; i++) {
			j = i - incr;
			while(j >= 0) {  
			  if (a[j] > a[j+incr]) {
			    LONG T = a[j];
			    a[j] = a[j+incr];
			    a[j+incr] = T;
			    j -= incr;
			  }
			  else j = -1;
			}
      	}
    }
}

VOID ParseLayoutXml(UCHAR paperSizeIndex, CHAR *ptr)
{
	ULONG layout;	
	UCHAR layoutIndex = 0;
	BOOL isFound = FALSE;

// Warning	while(ptr = DpsLocateStr("57",ptr))
	while((ptr = DpsLocateStr("57",ptr)) != 0)
	{
		ptr = ptr - 2;
		
		layout = (*ptr - (*ptr > 0x39 ? 0x37:0x30))<<28 | (*(ptr+1) - (*(ptr+1) > 0x39 ? 0x37:0x30))<<24 | (*(ptr+2) - (*(ptr+2) > 0x39 ? 0x37:0x30))<<20 | (*(ptr+3) - (*(ptr+3) > 0x39 ? 0x37:0x30))<<16 |
	    	 	 (*(ptr+4) - (*(ptr+4) > 0x39 ? 0x37:0x30))<<12 | (*(ptr+5) - (*(ptr+5) > 0x39 ? 0x37:0x30))<< 8 | (*(ptr+6) - (*(ptr+6) > 0x39 ? 0x37:0x30))<< 4 | (*(ptr+7) - (*(ptr+7) > 0x39 ? 0x37:0x30));
		
#if 0 // for PGRelief
		switch(layout)
		{
			//case LAYOUT_1UP:
			//	prtParaValue.borderlessOff[paperSizeIndex] = 1;//Borderless OFF
			//case LAYOUT_DEFAULT:
			//	break;
			//case LAYOUT_INDEX:
			//	prtParaValue.indexPrint[paperSizeIndex] = 1;
			//	prtParaValue.supportIndexPrint = 1;
			//	break;	
			//case LAYOUT_BORDERLESS:
			//	prtParaValue.borderlessPrint[paperSizeIndex] = 1;//Borderless ON(1-Up Borderless)
			//	break;
			default:
				{
					isFound = dataFilter(layout, LAYOUTS);
					if ((layoutIndex < NUMBER_OF_LAYOUTS)&&(isFound)){						
				prtParaValue.layout[paperSizeIndex][layoutIndex++] = layout;
				prtParaValue.supportNLayout = 1;
					}
				}
				break;
		}	
#else
		isFound = dataFilter(layout, LAYOUTS);
		if ((layoutIndex < NUMBER_OF_LAYOUTS)&&(isFound)){
			prtParaValue.layout[paperSizeIndex][layoutIndex++] = layout;
			prtParaValue.supportNLayout = 1;
		}
#endif
		ptr = ptr + 9;
	}
}	

VOID ParsePaperTypeXml(UCHAR paperSizeIndex, CHAR *ptr)
{
	ULONG type;	
	UCHAR typeIndex = 0;
	BOOL isFound = FALSE;
	
// Warning	while(ptr = DpsLocateStr("52",ptr))
	while((ptr = DpsLocateStr("52",ptr)) != 0)
	{
		ptr = ptr - 2;
		
		type = (*ptr - (*ptr > 0x39 ? 0x37:0x30))<<28 | (*(ptr+1) - (*(ptr+1) > 0x39 ? 0x37:0x30))<<24 | (*(ptr+2) - (*(ptr+2) > 0x39 ? 0x37:0x30))<<20 | (*(ptr+3) - (*(ptr+3) > 0x39 ? 0x37:0x30))<<16 |
	    	 	 (*(ptr+4) - (*(ptr+4) > 0x39 ? 0x37:0x30))<<12 | (*(ptr+5) - (*(ptr+5) > 0x39 ? 0x37:0x30))<< 8 | (*(ptr+6) - (*(ptr+6) > 0x39 ? 0x37:0x30))<< 4 | (*(ptr+7) - (*(ptr+7) > 0x39 ? 0x37:0x30));
		
#if 0 // for PGRelief
		switch(type)
		{
			default:
				{
					isFound = dataFilter(type, PAPERTYPE);	
					if ((typeIndex < NUMBER_OF_PAPERTYPE)&&(isFound)){	
				prtParaValue.paperType[paperSizeIndex][typeIndex++] = type;
					}
				}
				break;
		}	
#else
		isFound = dataFilter(type, PAPERTYPE);
		if ((typeIndex < NUMBER_OF_PAPERTYPE)&&(isFound)){
			prtParaValue.paperType[paperSizeIndex][typeIndex++] = type;
		}
#endif
		ptr = ptr + 9;
	}
}	

CHAR *ReadLayoutCap(CHAR *ptr)
{
	ULONG paperSize;
	UCHAR paperSizeIndex;
	
// Warning	if(!(ptr = DpsLocateStr("paperSize=\"",ptr))) return 0;
// Warning	if(!(ptr = ReadXmlData((CHAR *)&paperSize, ptr, TYPE_L))) return 0;
// Warning	if(!(ptr = DpsLocateStr("5",ptr))) return 0;
	if((ptr = DpsLocateStr("paperSize=\"",ptr)) == 0) return 0;
	if((ptr = ReadXmlData((CHAR *)&paperSize, ptr, TYPE_L)) == 0) return 0;
	if((ptr = DpsLocateStr("5",ptr)) == 0) return 0;
	ptr--;
	
	paperSizeIndex = GetPaperSizeIndex(paperSize);
	if(paperSizeIndex == 0xff) return 0;
			
	ParseLayoutXml(paperSizeIndex,ptr);   
	SortParameter(prtParaValue.layout[paperSizeIndex]);
	return ptr;		
}	

CHAR *ReadPaperTypeCap(CHAR *ptr)
{
	ULONG paperSize;
	UCHAR paperSizeIndex;
	
// Warning	if(!(ptr = DpsLocateStr("paperSize=\"",ptr))) return 0;
// Warning	if(!(ptr = ReadXmlData((CHAR *)&paperSize, ptr, TYPE_L))) return 0;
// Warning	if(!(ptr = DpsLocateStr("5",ptr))) return 0;
	if((ptr = DpsLocateStr("paperSize=\"",ptr)) == 0) return 0;
	if((ptr = ReadXmlData((CHAR *)&paperSize, ptr, TYPE_L)) == 0) return 0;
	if((ptr = DpsLocateStr("5",ptr)) == 0) return 0;
	ptr--;
	
	paperSizeIndex = GetPaperSizeIndex(paperSize);
	if(paperSizeIndex == 0xff) return 0;
			
	ParsePaperTypeXml(paperSizeIndex,ptr);   
	SortParameter(prtParaValue.paperType[paperSizeIndex]);
	return ptr;		
}	

BOOL ParseDpsParameter(CHAR *buff)
{
	USHORT i;
	CHAR *paraStartPtr=buff;
	CHAR *paraTempPtr;
//	FILE *fp;
// Warning	while(paraStartPtr = DpsLocateStr("<",paraStartPtr))
	while((paraStartPtr = DpsLocateStr("<",paraStartPtr)) != 0)
	{	
		if(!StringIdentical(paraStartPtr,opString[dpsStatus.operationId].operationString)) //end of parameter block
//			for(i=0;i<NUMBER_OF_PRT_PARA;i++)
			for(i=0;i<=NUMBER_OF_PRT_PARA;i++)
			{
// Warning				if(paraTempPtr = StringIdentical(paraStartPtr,prtParaString[i].paraString))
				if((paraTempPtr = StringIdentical(paraStartPtr,prtParaString[i].paraString)) != 0)
				{
					switch(i)
					{
						case QUALITIES:
							ReadXmlDataFilter((CHAR *)((ULONG)prtParaValuePtr + (ULONG)prtParaString[i].saveOffset),paraTempPtr,prtParaString[i].type,QUALITIES);						
							SortParameter(prtParaValue.qualities);
							break;
						case PAPERSIZES:
							ReadXmlDataFilter((CHAR *)((ULONG)prtParaValuePtr + (ULONG)prtParaString[i].saveOffset),paraTempPtr,prtParaString[i].type,PAPERSIZES);						
							SortParameter(prtParaValue.paperSize);
							break;
						case PAPERTYPE:
							if(!ReadPaperTypeCap(paraTempPtr)) return FALSE;
							break;
							//ReadXmlData((CHAR *)((ULONG)prtParaValuePtr + (ULONG)prtParaString[i].saveOffset),paraTempPtr,prtParaString[i].type);						
							//SortParameter(prtParaValue.paperType);
						case FILETYPES:
							ReadXmlDataFilter((CHAR *)((ULONG)prtParaValuePtr + (ULONG)prtParaString[i].saveOffset),paraTempPtr,prtParaString[i].type,FILETYPES);						
							SortParameter(prtParaValue.fileTypes);
							break;
						case DATEPRINTS://
							ReadXmlDataFilter((CHAR *)((ULONG)prtParaValuePtr + (ULONG)prtParaString[i].saveOffset),paraTempPtr,prtParaString[i].type,DATEPRINTS);						
							SortParameter(prtParaValue.datePrints);
							break;
						case FILENAMEPRINTS:          
							ReadXmlDataFilter((CHAR *)((ULONG)prtParaValuePtr + (ULONG)prtParaString[i].saveOffset),paraTempPtr,prtParaString[i].type,FILENAMEPRINTS);						
							SortParameter(prtParaValue.fileNamePrints);
							break;	
						case IMAGEOPTIMIZES:
							ReadXmlDataFilter((CHAR *)((ULONG)prtParaValuePtr + (ULONG)prtParaString[i].saveOffset),paraTempPtr,prtParaString[i].type,IMAGEOPTIMIZES);						
							SortParameter(prtParaValue.imageOptimizes);
							break;
						case PAPERFIXEDSIZE:
							ReadXmlDataFilter((CHAR *)((ULONG)prtParaValuePtr + (ULONG)prtParaString[i].saveOffset),paraTempPtr,prtParaString[i].type,PAPERFIXEDSIZE);						
							SortParameter(prtParaValue.paperFixedSize);
							break;
						case CROPPINGS:
							ReadXmlDataFilter((CHAR *)((ULONG)prtParaValuePtr + (ULONG)prtParaString[i].saveOffset),paraTempPtr,prtParaString[i].type,CROPPINGS);						
							SortParameter(prtParaValue.croppings);
							break;
						case LAYOUTS:                 
							if(!ReadLayoutCap(paraTempPtr)) return FALSE;
							break;	
						case DISCONNECTENABLE:        
							if(DpsLocateStr("74010000",paraTempPtr))
								prtParaValue.disconnectEnable = 1;												
							else
								prtParaValue.disconnectEnable = 0;
							break;
						case CAPABILITYCHANGED:       
							if(DpsLocateStr("75010000",paraTempPtr))
								prtParaValue.capabilityChanged = 1;												
							else
								prtParaValue.capabilityChanged = 0;
							break;
						case NEWJOBOK:                
							if(DpsLocateStr("76010000",paraTempPtr))
								prtParaValue.newJobOK = 1;												
							else
								prtParaValue.newJobOK = 0;
							break;
						case PRINTSERVICEAVAILABLE:   
							if(DpsLocateStr("30010000",paraTempPtr))
								prtParaValue.printServiceAvailable = 1;
							else
								prtParaValue.printServiceAvailable = 0;												
							break;	
						case XML_COMMENT:
						continue;
						//break;	
						case JOBENDREASON:           
							ReadXmlData((CHAR *)((ULONG)prtParaValuePtr + (ULONG)prtParaString[i].saveOffset),paraTempPtr,prtParaString[i].type);
							break;	
						case ERRORREASON:
							ReadXmlData((CHAR *)((ULONG)prtParaValuePtr + (ULONG)prtParaString[i].saveOffset),paraTempPtr,prtParaString[i].type);
							break;
						case PROGRESS:
							prtParaValue.progress[0]= str2num(paraTempPtr+1, 3);
							prtParaValue.progress[1]= str2num(paraTempPtr+5, 3);
							//ReadXmlData((CHAR *)((ULONG)prtParaValuePtr + (ULONG)prtParaString[i].saveOffset),paraTempPtr,prtParaString[i].type);
							break;
						case DPSPRINTSERVICESTATUS: 
							ReadXmlData((CHAR *)((ULONG)prtParaValuePtr + (ULONG)prtParaString[i].saveOffset),paraTempPtr,prtParaString[i].type);
							break;                       
						case ERRORSTATUS:             
							ReadXmlData((CHAR *)((ULONG)prtParaValuePtr + (ULONG)prtParaString[i].saveOffset),paraTempPtr,prtParaString[i].type);
							break; 
						case FILEID:			
						case BASEPATHID:  
						case PRTPID:                  
						case COPYID:                  
						case IMAGESPRINTED:                       
						case FILEPATH:                
						case DPSVERSIONS:             
						case VENDORNAME:              
						case VENDORSPECIFICVERSION:   
						case SERIALNO:                
						case PRODUCTNAME:             						
							 ReadXmlData((CHAR *)((ULONG)prtParaValuePtr + (ULONG)prtParaString[i].saveOffset),paraTempPtr,prtParaString[i].type);
							break;
					}
					continue;
				}	
			}
	}
	return TRUE;
}

CHAR *ParseResultCode(CHAR *buff)
{
	CHAR *ptr;
	
// Warning	if(!(ptr = DpsLocateStr("<result>",buff))) return 0;
	if((ptr = DpsLocateStr("<result>",buff)) == 0) return 0;
	
	ptr = ReadXmlData((CHAR *)&(dpsStatus.operationResult),ptr,TYPE_L);
		
	if(dpsStatus.operationResult != REQUEST_OK// &&
	   //dpsStatus.operationResult != REQUEST_NOTEXECUTED &&
	   //dpsStatus.operationResult != REQUEST_NOTSUPPORTED &&
	   /*dpsStatus.operationResult != REQUEST_NOTRECOGNIZED*/)
	   	return 0;	
		   			
	if(!StringIdentical(ptr-9,"</result>")) return 0;
		
        return ptr;
}

CHAR *ParseOperationID(CHAR *buff)
{
	CHAR *ptr;
        CHAR *paraTempPtr;
	USHORT i;
	
	ptr = DpsLocateStr("<",buff);
	if (ptr == 0) {
		return 0;
	}
	
	for(i=1;i<PRINT_RESPONSE_END;i++)
// Warning		if(paraTempPtr = StringIdentical(ptr,opString[i].operationString))
		if((paraTempPtr = StringIdentical(ptr,opString[i].operationString)) != 0)
			{dpsStatus.operationId = opString[i].operationId;return paraTempPtr;}
	return 0;		
}

CHAR *ParseOperationType(CHAR *buff)
{
	CHAR *ptr = buff;

// Warning	if(!(ptr = DpsLocateStr("put>",ptr))) return 0;
	if((ptr = DpsLocateStr("put>",ptr)) == 0) return 0;

	if(StringIdentical(ptr-7,"<input>"))
	{
		//operationType = REQUEST;
		dpsStatus.inOperationType = PRT_REQUEST;
		if(!(DpsLocateStr("</input>",ptr))) return 0;
	}
	else if(StringIdentical(ptr-8,"<output>"))
	{
		dpsStatus.inOperationType = PRT_RESPONSE;
		if(!(DpsLocateStr("</output>",ptr))) return 0;
	}	
	else
		return 0;
		
	return ptr;	
}	

CHAR *ParseXML(CHAR *buff)
{
	CHAR *ptr = buff;
//	BOOL parseFail = FALSE;
	
// Warning	if(!(ptr = DpsLocateStr("<?xml",ptr))) parseFail = TRUE;
// Warning	if(!(ptr = DpsLocateStr("<dps",ptr))) parseFail = TRUE;
	if((ptr = DpsLocateStr("<?xml",ptr)) == 0){
//		parseFail = TRUE;
		return 0;
	}

	if((ptr = DpsLocateStr("<dps",ptr)) == 0){
//		parseFail = TRUE;
		return 0;
	}
	
// Warning	if(!(ptr = ParseOperationType(ptr))) parseFail = TRUE;
	if((ptr = ParseOperationType(ptr)) == 0){
//		parseFail = TRUE;
		return 0;
	}

	if(dpsStatus.inOperationType == PRT_RESPONSE){
// Warning		if(!(ptr = ParseResultCode(ptr))) parseFail = TRUE;
		if((ptr = ParseResultCode(ptr)) == 0){
//			parseFail = TRUE;
			return 0;
		}
	}

// Warning	if(!(ptr = ParseOperationID(ptr))) parseFail = TRUE;
	if((ptr = ParseOperationID(ptr)) == 0){
//		parseFail = TRUE;
		return 0;
	}

// Warning	if(!ParseDpsParameter(ptr)) parseFail = TRUE;
	if(ParseDpsParameter(ptr) == 0){
//		parseFail = TRUE;
		return 0;
	}
	
// Warning	if(!(ptr = DpsLocateStr("</dps>",ptr))) parseFail = TRUE;
	if((ptr = DpsLocateStr("</dps>",ptr)) == 0){
//		parseFail = TRUE;
		return 0;
	}
	
//	dpsStatus.lockInputBuf = FALSE;
// Warning	if(parseFail)
	//if(parseFail != 0)
	//	return 0;
	//else	
	//	return ptr;
	return ptr;
}

CHAR *ResponseXmlHead(CHAR *buff, USHORT operationType)
{
	CHAR *ptr=buff;
	
	ptr = CopyString(ptr,line1String,0,0);
	ptr = CopyString(ptr,line2String,0,0);
	switch(operationType)
	{
		case DSC_REQUEST:
		ptr = CopyString(ptr,line6String,0,0);
		break;
		case DSC_RESPONSE:
		ptr = CopyString(ptr,line3String,0,0);
		break;
	}
	return ptr;
}	


CHAR *ResponseXmlTail(CHAR *buff, USHORT operationType)
{	
	CHAR *ptr = buff;
	switch(operationType)
	{
		case DSC_REQUEST:
			ptr = CopyString(ptr,line7String,0,0);
		break;
		case DSC_RESPONSE:
			ptr = CopyString(ptr,line4String,0,0);
		break;	
	}
	ptr = CopyString(ptr,line5String,0,0);
	return ptr;
}	

CHAR GetDpsOutputBuf(ULONG operationType) 
{

	return TRUE;
}

CHAR GetDpsInputBuf()  
{

	memset(dpsInBufferPtr,0,DPS_BUFFER_SIZE);

	return TRUE;
}

/*UCHAR IssueRequestEventOnly(USHORT fileSize)
{
	UINT32 nFlag;
	UINT32	cnt;
	UINT32 count;
	OS_USER_ER ret;
	UINT32 counter;

	OS_User_Set_Flg(FID_DPSTSK, FLG_DPSDSC_EVENT_ONLY);
	
	return 0;
	
}*/

UINT32	DpsWaitTime;	
UCHAR IssueRequest(USHORT fileSize)
{
	OS_USER_FLGPTN nFlag;
	//UINT32	cnt;
	OS_USER_ER ret;
	//UINT32 counter;

	//cnt=0;

//	ret = OS_User_Pol_Flg(&nFlag, FID_DPSTSK, FLG_DPSPRT2, D_OS_USER_TWF_ORW);
	ret = OS_User_Pol_Flg(FID_DPSTSK, FLG_DPSPRT2, D_OS_USER_TWF_ORW, &nFlag);

       while(ret != D_OS_USER_E_OK){

              //for(counter=0;counter<1000;counter++) {;}
		//cnt++;
		//if(!vSessionOpen)	return 2;

//		ret = OS_User_Pol_Flg(&nFlag, FID_DPSTSK, FLG_DPSPRT2, D_OS_USER_TWF_ORW);
		ret = OS_User_Pol_Flg(FID_DPSTSK, FLG_DPSPRT2, D_OS_USER_TWF_ORW, &nFlag);
		OS_User_Tslp_Tsk(1);//paul 20060629 fix bug print hang			
#if 0	// This process is workable when USB Polling functions is used.  
		if (FJ_GetUsbCableStatus()==FALSE){
			BF_Debug_Print_Error(("FJ_GetUsbCableStatus()==FALSE\n"));
			break;
		}				
#endif
	}
	OS_User_Clr_Flg(FID_DPSTSK, ~FLG_DPSPRT2);
	dpsStatus.outOperationType = DSC_REQUEST;
	dpsStatus.dscRequestFileSize = fileSize;

       
	OS_User_Set_Flg(FID_DPSTSK, FLG_DPSDSC);
	
	if(!vSessionOpen)	return 2;

//	ret = OS_User_Pol_Flg(&nFlag, FID_DPSTSK, (FLG_DPSPRT2), D_OS_USER_TWF_ORW);
	ret = OS_User_Pol_Flg(FID_DPSTSK, (FLG_DPSPRT2), D_OS_USER_TWF_ORW, &nFlag);


       while(ret != D_OS_USER_E_OK)
       {
             //for(counter=0;counter<1000;counter++) {;}
//	      ret = OS_User_Pol_Flg(&nFlag, FID_DPSTSK, (FLG_DPSPRT2), D_OS_USER_TWF_ORW);
	      ret = OS_User_Pol_Flg(FID_DPSTSK, (FLG_DPSPRT2), D_OS_USER_TWF_ORW, &nFlag);
		OS_User_Tslp_Tsk(1);//paul 20060612 fix bug USB Connecting ...			
#if 0	// This process is workable when USB Polling functions is used. 
		if (FJ_GetUsbCableStatus()==FALSE){
			BF_Debug_Print_Error(("FJ_GetUsbCableStatus()==FALSE\n"));
			break;
		}			
#endif
	}
      
	if(!vSessionOpen)	return 2;
	
	if(nFlag & FLG_DPSPRT2){	
	       return 0;//Succeed		
	}
	else{
		return 0;//Time-out
	}
}

UCHAR IssueResponse(USHORT fileSize)  
{	
	dpsStatus.waitState2 = WAIT_REQUEST;
	dpsStatus.outOperationType = DSC_RESPONSE;
	dpsStatus.dscResponseFileSize = fileSize;
	SendDSPCmdFile();
	return TRUE;		
}
			
USHORT ResponseGetFileID()
{
#ifdef CO_DCF_EXIF_ON
	CHAR *ptr = 0;
	CHAR dirID[9]={0},fileID[9],tmpPath[64];
	LONG ulFileID = 0;
	LONG ulDirID = 0;
	FW_INT32 ret = 0;
	
	if(prtParaValue.basePathID != FIL_DPOF) return 0;
	if(strlen(prtParaValue.filePath) < 27 ) return 0;
	strncpy(tmpPath, prtParaValue.filePath, sizeof(tmpPath));
	//memcpy(tmpPath, "I:", 2);
	//tmpPath[2] = 0x5C;
	//tmpPath[7] = 0x5C;
	//tmpPath[16] = 0x5C;
	memcpy(dirID, (FW_CHAR*)tmpPath+8,8);
	dirID[8] = '\0';	
	ret = BF_Dcf_If_Get_Index_By_Dir_Name((const FW_CHAR*)dirID, &ulDirID);
	if(0 != ret) return 0;
	ret = BF_Dcf_If_Set_Current_Dir(NULL, D_BF_DCF_IF_PRM_NORMAL_ACTION, (ulDirID & 0xFFFF0000));
	if(0 != ret) {return 0;}
	BF_Dcf_If_Get_File_Id((FW_CHAR*)tmpPath, (FW_INT32*)&ulFileID);
	if(ulFileID <= 0)return 0;
	snprintf(fileID,sizeof(fileID),"%08lX",ulFileID);
	ptr = ResponseXmlHead(dscResBufPtr,DSC_RESPONSE);
	ptr = ResponsePara(ptr,stringResultPtr,(CHAR *)(resultCodeArrayPtr),TYPE_L,1);
	ptr = CopyString(ptr, (CHAR *)(&opString[DPS_GETFILEID]), 0,ARROW2_R);
	ptr = ResponsePara(ptr,prtParaString[FILEID].paraString,fileID,TYPE_C,1);
	ptr = CopyString(ptr, (CHAR *)(&opString[DPS_GETFILEID]), 0,S_ARROR2_R);
	ptr = ResponseXmlTail(ptr,DSC_RESPONSE);
	
	IssueResponse(ptr - dscResBufPtr);
#endif
	return TRUE;
}

USHORT ResponseNotifyJobStatus()
{
	CHAR *ptr;
	
	ptr = ResponseXmlHead(dscResBufPtr,DSC_RESPONSE);
	ptr = ResponsePara(ptr,stringResultPtr,(CHAR *)(resultCodeArrayPtr),TYPE_L,1);
	ptr = CopyString(ptr, (CHAR *)(&opString[DPS_NOTIFYJOBSTATUS]), 0,ARROR2_S_R);
	ptr = ResponseXmlTail(ptr,DSC_RESPONSE);
			
	IssueResponse(ptr - dscResBufPtr);
	return TRUE;
}

USHORT ResponseNotifyDeviceStatus()
{
	CHAR *ptr;
	ptr = ResponseXmlHead(dscResBufPtr,DSC_RESPONSE);
	ptr = ResponsePara(ptr,stringResultPtr,(CHAR *)(resultCodeArrayPtr),TYPE_L,1);
	ptr = CopyString(ptr, (CHAR *)(&opString[DPS_NOTIFYDEVICESTATUS]), 0,ARROR2_S_R);
	ptr = ResponseXmlTail(ptr,DSC_RESPONSE);

	IssueResponse(ptr - dscResBufPtr);
	return TRUE;
}

USHORT ResponseUnrecognizedOperation(UCHAR result)
{
	CHAR *ptr;
	MSG outmsg = {ePictBridgeSubState_Unrecognized_Operation, 0, (VOID*)0, {0}};
	FJ_PictBridgeSendHostMessage(outmsg);
	ptr = ResponseXmlHead(dscResBufPtr,DSC_RESPONSE);
	ptr = ResponsePara(ptr,stringResultPtr,(CHAR *)(resultCodeArrayPtr+result),TYPE_L,1);
	ptr = ResponseXmlTail(ptr,DSC_RESPONSE);

	IssueResponse(ptr - dscResBufPtr);
	return TRUE;
}
	
USHORT RequestConfigurePrintService()
{
	CHAR *ptr;
	ULONG ulCount = 0;
	GetDpsOutputBuf(DSC_REQUEST);
	
	ptr = ResponseXmlHead(dscReqBufPtr,DSC_REQUEST);

	ptr = CopyString(ptr,(CHAR *)(&opString[DPS_CONFIGUREPRINTSERVICE]),0,ARROW2_R);
	ptr = ResponsePara(ptr,dscParaString[D_DPSVERSIONS].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_DPSVERSIONS].saveOffset),dscParaString[D_DPSVERSIONS].type,dscParaString[D_DPSVERSIONS].quantity);
	ptr = ResponsePara(ptr,dscParaString[D_VENDORNAME].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_VENDORNAME].saveOffset),dscParaString[D_VENDORNAME].type,dscParaString[D_VENDORNAME].quantity);
	ptr = ResponsePara(ptr,dscParaString[D_VENDORSPECIFICVERSION].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_VENDORSPECIFICVERSION].saveOffset),dscParaString[D_VENDORSPECIFICVERSION].type,dscParaString[D_VENDORSPECIFICVERSION].quantity);
	ptr = ResponsePara(ptr,dscParaString[D_PRODUCTNAME].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_PRODUCTNAME].saveOffset),dscParaString[D_PRODUCTNAME].type,dscParaString[D_PRODUCTNAME].quantity);
	//ptr = ResponsePara(ptr,dscParaString[D_SERIALNO].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_SERIALNO].saveOffset),dscParaString[D_SERIALNO].type,dscParaString[D_SERIALNO].quantity);
	ptr = CopyString(ptr,(CHAR *)(&opString[DPS_CONFIGUREPRINTSERVICE]),0,S_ARROR2_R);
	ptr = ResponseXmlTail(ptr,DSC_REQUEST);

	IssueRequest(ptr - dscReqBufPtr);
	while((dpsStatus.waitState == CONFIG_SERVICE) && (ulCount < 5000))
	{
		OS_User_Tslp_Tsk(1);
		ulCount++;
	}
	return 0;
}

USHORT RequestGetCapability(UCHAR item, UCHAR papersizeIndex)
{
	CHAR *ptr;

	GetDpsOutputBuf(DSC_REQUEST);

	ptr = ResponseXmlHead(dscReqBufPtr,DSC_REQUEST);
	
	//add "<getCapability>" to buffer
	ptr = CopyString(ptr,opString[DPS_GETCAPABILITY].operationString, 0,ARROW2_R);

	//add "<capability>" to buffer
	ptr = CopyString(ptr,capabilityString, 0,ARROW2_R);

	//add "<layouts paperSize="0x12345678"/>" to buffer
	*ptr++ = '<';	
	ptr = CopyString(ptr,prtParaString[item].paraString, 0,0);	
	if((item == LAYOUTS) || (item == PAPERTYPE))
	{
	*ptr++ = ' ';	
	ptr = CopyString(ptr,dscParaString[D_PAPERSIZE].paraString, 0,0);
	*ptr++ = '=';	
	*ptr++ = '"';
        sprintf(ptr,"%08lX",prtParaValue.paperSize[papersizeIndex]);
	ptr+=8;
	*ptr++ = '"';
	}
	*ptr++ = '/';
	*ptr++ = '>';
	
        *ptr++ = '\r';		
        *ptr++ = '\n';		
        *ptr++ = ' ';		
	
	//add "</capability>" to buffer
	ptr = CopyString(ptr,capabilityString, 0,S_ARROR2_R);

	//add "</getCapability>" to buffer		
	ptr = CopyString(ptr,opString[DPS_GETCAPABILITY].operationString, 0,S_ARROR2_R);

	ptr = ResponseXmlTail(ptr,DSC_REQUEST);

	return	IssueRequest(ptr - dscReqBufPtr);

}
UCHAR	SettingChange=0;
UCHAR	IsSetChanging( VOID )
{
	return SettingChange;
}

UCHAR GetNecessaryCapability()
{
	UCHAR i = 0;
	UCHAR	ret;

	if(!vSessionOpen)return 2;//like time-out
	dpsStatus.capabilityChanged = 1;
	prtParaValue.capabilityChanged=0; 
	if(GetCapabilityEntryCnt){//repeate
		return 2;
	}else{//first
		GetCapabilityEntryCnt=1;
		if(!InDpsStarting)	
			needShowCapabilityChange=1;
	}	
	DpsInitPrtParaValue(FALSE);
	ret=0;//OK
	
	while((dpsStatus.capabilityChanged == 1)&&(vSessionOpen)&&(!ret))
	{
		i=0;
		if(!vSessionOpen)	break;		
		dpsStatus.capabilityChanged = 0;
		if(!ret)
// Warning			if(ret=RequestGetCapability(PAPERSIZES, i))  break;
			if((ret=RequestGetCapability(PAPERSIZES, i)) != 0)  break;
		while((prtParaValue.paperSize[i] != 0) && (i < NUMBER_OF_PAPERSIZE))
		{
			if(!ret)
			{
// Warning				if(ret=RequestGetCapability(LAYOUTS, i))	break;	
				if((ret=RequestGetCapability(LAYOUTS, i)) != 0)	break;	
				i++;
			}	
			else break;
		}
		i = 0;
		while((prtParaValue.paperSize[i] != 0) && (i < NUMBER_OF_PAPERSIZE))
		{
			if(!ret)
			{
// Warning				if(ret=RequestGetCapability(PAPERTYPE, i))	break;	
				if((ret=RequestGetCapability(PAPERTYPE, i)) != 0)	break;	
				i++;
			}	
			else break;
		}
		if(!ret)
// Warning			if(ret=RequestGetCapability(DATEPRINTS, i))  break;	
			if((ret=RequestGetCapability(DATEPRINTS, i)) != 0)  break;	
		if(!ret)
// Warning			if(ret=RequestGetCapability(FILENAMEPRINTS, i))  break;
			if((ret=RequestGetCapability(FILENAMEPRINTS, i)) != 0)  break;
		if(!ret)
// Warning			if(ret=RequestGetCapability(IMAGEOPTIMIZES, i))  break;
			if((ret=RequestGetCapability(IMAGEOPTIMIZES, i)) != 0)  break;
		if(!ret)
// Warning			if(ret=RequestGetCapability(CROPPINGS, i))  break;	
			if((ret=RequestGetCapability(CROPPINGS, i)) != 0)  break;	
		if(!ret)
// Warning			if(ret=RequestGetCapability(FILETYPES, i))  break;
			if((ret=RequestGetCapability(FILETYPES, i)) != 0)  break;
		if(!ret)
// Warning			if(ret=RequestGetCapability(QUALITIES, i))  break;
			if((ret=RequestGetCapability(QUALITIES, i)) != 0)  break;
		if(!ret)
// Warning			if(ret=RequestGetCapability(PAPERFIXEDSIZE, i))  break;
			if((ret=RequestGetCapability(PAPERFIXEDSIZE, i)) != 0)  break;
			
		if(needShowCapabilityChange){
			SettingChange=1;
	
			SettingChange=0;
		}	
	}
	needShowCapabilityChange=0;
			
	GetCapabilityEntryCnt=0;
	
	if(ret)	return ret;
	return 0;
}

USHORT RequestGetJobStatus()
{
	CHAR *ptr;

	GetDpsOutputBuf(DSC_REQUEST);

	ptr = ResponseXmlHead(dscReqBufPtr,DSC_REQUEST);
	ptr = CopyString(ptr,opString[DPS_GETJOBSTATUS].operationString, 0,ARROR2_S_R);
	ptr = ResponseXmlTail(ptr,DSC_REQUEST);

	IssueRequest(ptr - dscReqBufPtr);

	return 1;	
}
	
USHORT RequestGetDeviceStatus()
{
	CHAR *ptr;

	GetDpsOutputBuf(DSC_REQUEST);
	
	ptr = ResponseXmlHead(dscReqBufPtr,DSC_REQUEST);
	ptr = CopyString(ptr,opString[DPS_GETDEVICESTATUS].operationString, 0,ARROR2_S_R);
	ptr = ResponseXmlTail(ptr,DSC_REQUEST);


	IssueRequest(ptr - dscReqBufPtr);

	return 1;	
}

VOID  ResetObjectPrintInfo()
{
#if 1	// [2009/06/29] Softune Bug
	UINT32 i = 0;
#else
	INT32 i = 0;
#endif
	for(i = 0; i < NUM_OBJECT_SD; i++)
	{
		if((Object[i].PrintInfo).usCopies > 0)  //has setting
		{
			if (Object[i].DcfHandle != 0)
			{
				memset(&(Object[i].PrintInfo), 0, sizeof(ImagePrintInfo));
			}
		}
	}	
}

USHORT RequestStartJob()
{ 
#ifdef CO_DCF_EXIF_ON
	CHAR *ptr;
	ULONG i = 0;
	UINT32 k = 0;
	USHORT j = 0, n = 0;
	FW_LONG Index = -1;
	FW_CHAR srcfilepath[64];
	//T_BF_FS_IF_STAT fsstat;
	UCHAR   DateBuf[15];
	T_BF_DCF_IF_IMAGE_DETAIL img_detail;
	T_BF_DCF_IF_LOAD_IMAGE_DATA img_data;
	GetDpsOutputBuf(DSC_REQUEST);
	i=needCheckJobAgain;
	needCheckJobAgain=0;
	for(i = 0;i < ulTotalDPSDir; i++)
	{
		if(gBF_Ptp_Buff->ObjectSearchTbl[i].usObjPos == -1) /* empty dir */
		{
			continue;
		}
		else
		{
			for(k = gBF_Ptp_Buff->ObjectSearchTbl[i].usObjPos; k < (gBF_Ptp_Buff->ObjectSearchTbl[i].usObjPos) + (gBF_Ptp_Buff->ObjectSearchTbl[i].usObjCnt); k++)
			{
				if((Object[k].PrintInfo).usCopies > 0)  //has setting
				{	
					if (Object[k].DcfHandle != 0)
					{
						dps_MarkFile.FATData[j] = Object[k].DcfHandle;
						dps_MarkFile.PrintInfo[j] = Object[k].PrintInfo;
						j++;
					}
				}
			}
		}
		
	}
	needCheckJobAgain=0;
	//for (i = 0, j = 0; i < NUM_OBJECT_SD; i++)
/*
	for (i = (NUM_OBJECT_SD - 1), j = 0; i >= 0; i--)
	{
		if (Object[i].DcfHandle != 0)
		{
			if((Object[i].PrintInfo).usCopies > 0)  //has setting
			{
				dps_MarkFile.FATData[j] = Object[i].DcfHandle;
				dps_MarkFile.PrintInfo[j] = Object[i].PrintInfo;
				j++;
			}
		}
	}
*/
	if(j == 0)/* No file is set */
	{
		return 1;	
	}
	dps_MarkFile.file_cnt = j;
	ptr = ResponseXmlHead(dscReqBufPtr,DSC_REQUEST);
	ptr = CopyString(ptr,(CHAR *)(&opString[DPS_STARTJOB]),0,ARROW2_R);
	//jobConfig
	ptr = CopyString(ptr,jobConfigString,0,ARROW2_R);
	if(dscParaValue.quality)
	{
		ptr = ResponsePara(ptr,dscParaString[D_QUALITY].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_QUALITY].saveOffset),dscParaString[D_QUALITY].type,dscParaString[D_QUALITY].quantity);
	}
	if(dscParaValue.paperSize)
	{
		ptr = ResponsePara(ptr,dscParaString[D_PAPERSIZE].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_PAPERSIZE].saveOffset),dscParaString[D_PAPERSIZE].type,dscParaString[D_PAPERSIZE].quantity);
	}
	if(dscParaValue.paperType)
	{
		ptr = ResponsePara(ptr,dscParaString[D_PAPERTYPE].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_PAPERTYPE].saveOffset),dscParaString[D_PAPERTYPE].type,dscParaString[D_PAPERTYPE].quantity);
	}
	if(dscParaValue.fileType)
	{
		ptr = ResponsePara(ptr,dscParaString[D_FILETYPE].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_FILETYPE].saveOffset),dscParaString[D_FILETYPE].type,dscParaString[D_FILETYPE].quantity);
	}
	//dscParaValue.datePrint = (dps_MarkFile.PrintInfo[i]).uDatePrint;
	if(dscParaValue.datePrint)
	{
		ptr = ResponsePara(ptr,dscParaString[D_DATEPRINT].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_DATEPRINT].saveOffset),dscParaString[D_DATEPRINT].type,dscParaString[D_DATEPRINT].quantity);
	}
	//dscParaValue.fileNamePrint = (dps_MarkFile.PrintInfo[i]).uFileNamePrint;
	if(dscParaValue.fileNamePrint) 
	{
		ptr = ResponsePara(ptr,dscParaString[D_FILENAMEPRINT].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_FILENAMEPRINT].saveOffset),dscParaString[D_FILENAMEPRINT].type,dscParaString[D_FILENAMEPRINT].quantity);	
	}	
	if(dscParaValue.imageOptimize)
	{
		ptr = ResponsePara(ptr,dscParaString[D_IMAGEOPTIMIZE].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_IMAGEOPTIMIZE].saveOffset),dscParaString[D_IMAGEOPTIMIZE].type,dscParaString[D_IMAGEOPTIMIZE].quantity);	
	}
	if(dscParaValue.layout)
	{
		ptr = ResponsePara(ptr,dscParaString[D_LAYOUT].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_LAYOUT].saveOffset),dscParaString[D_LAYOUT].type,dscParaString[D_LAYOUT].quantity);	
	}
	if(dscParaValue.fixedSize)
	{
		ptr = ResponsePara(ptr,dscParaString[D_FIXEDSIZE].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_FIXEDSIZE].saveOffset),dscParaString[D_FIXEDSIZE].type,dscParaString[D_FIXEDSIZE].quantity);	
	}
	//dscParaValue.cropping = (dps_MarkFile.PrintInfo[i]).uCrop;
	if(dscParaValue.cropping) 
	{
		ptr = ResponsePara(ptr,dscParaString[D_CROPPING].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_CROPPING].saveOffset),dscParaString[D_CROPPING].type,dscParaString[D_CROPPING].quantity);	
	}	
	ptr = CopyString(ptr,jobConfigString,0,S_ARROR2_R);
    for(i = 0;i < dps_MarkFile.file_cnt;i++)
	{			
        dscParaValue.fileID = dps_MarkFile.FATData[i];
        if(!(((dps_MarkFile.PrintInfo[i]).ucPrintOption) && FJ_PICT_PRINT_OPTION_DUPLICATE_FILEID))		/*	pgr0006	*/
        {
			ptr = CopyString(ptr,printInfoString,0,ARROW2_R);
			ptr = ResponsePara(ptr,dscParaString[D_FILEID].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_FILEID].saveOffset),dscParaString[D_FILEID].type,dscParaString[D_FILEID].quantity);			
			dscParaValue.copies = (dps_MarkFile.PrintInfo[i]).usCopies;
			ptr = ResponsePara(ptr,dscParaString[D_COPIES].paraString,(CHAR *)&(dscParaValue.copies),TYPE_D,1);						
			if(dscParaValue.fileNamePrint == XML_FILENAME_PRINT_ON) 
			{	
				Index = PTP_FindObjectIndex(PictBridgeUI_MediaSelected, dps_MarkFile.FATData[i]);
				BF_Dcf_If_Set_Current_Dir(NULL, D_BF_DCF_IF_PRM_NORMAL_ACTION, (Index & 0xFFFF0000));
				BF_Dcf_If_Get_Index_File_Path(Index, "JPG", srcfilepath);		
				//memcpy(srcfilepath, DRIVE_TAG, strlen(DRIVE_TAG));
				//srcfilepath[7] = '\\';
				//srcfilepath[16] = '\\';
				strncpy(dscParaValue.fileName, (const CHAR*)(srcfilepath + strlen((const CHAR*)srcfilepath) - 12), sizeof(dscParaValue.fileName));
				ptr = ResponsePara(ptr,dscParaString[D_FILENAME].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_FILENAME].saveOffset),dscParaString[D_FILENAME].type,dscParaString[D_FILENAME].quantity);	
			}	
		
			if(dscParaValue.datePrint == XML_DATEPRINT_ON)
			{
				CHAR szTmpDate[26];
				Index = PTP_FindObjectIndex(PictBridgeUI_MediaSelected, dps_MarkFile.FATData[i]);
				BF_Dcf_If_Set_Current_Dir(NULL, D_BF_DCF_IF_PRM_NORMAL_ACTION, (Index & 0xFFFF0000));	
				//memcpy(srcfilepath, DRIVE_TAG, strlen(DRIVE_TAG));
				//srcfilepath[7] = '\\';
				//srcfilepath[16] = '\\';
				BF_Dcf_If_Get_Exif_Info_Jpeg(Index, &img_detail, &img_data);
				snprintf((CHAR *)DateBuf, sizeof(DateBuf), "%04d%02d%02d%02d%02d%02d", img_detail.DateTimeOriginal.ad_year, img_detail.DateTimeOriginal.month, img_detail.DateTimeOriginal.day, \
				img_detail.DateTimeOriginal.hour, img_detail.DateTimeOriginal.min, img_detail.DateTimeOriginal.sec);
				//BF_Dcf_If_Get_Index_File_Path(Index, "JPG", srcfilepath);
				//BF_Fs_If_Stat((const FW_CHAR *)srcfilepath, &fsstat);
				//sprintf(DateBuf, "%04d%02d%02d%02d%02d%02d", (fsstat.date >> 9) + 1980, (fsstat.date >> 5) & 0x0f, (fsstat.date) & 0x1f, 
				//(fsstat.time >> 11) & 0x1f, (fsstat.time >> 5) & 0x3f, (fsstat.time & 0x1f) * 2);
				switch(DPSdateType)
				{
					case FJ_DPS_DATE_FORMAT_TYPE0:// YY/MM/DD
						snprintf(dscParaValue.date, sizeof(dscParaValue.date), "%c%c/%c%c/%c%c", DateBuf[2], DateBuf[3], DateBuf[4], DateBuf[5], \
						DateBuf[6], DateBuf[7]);
						dscParaValue.date[8]=0;
						break;
					case FJ_DPS_DATE_FORMAT_TYPE1:// YYYY/MM/DD
						snprintf(dscParaValue.date, sizeof(dscParaValue.date),"%c%c%c%c/%c%c/%c%c", DateBuf[0], DateBuf[1], DateBuf[2], \
						DateBuf[3], DateBuf[4], DateBuf[5], \
						DateBuf[6], DateBuf[7]);
						dscParaValue.date[10]=0;
						break;
					case FJ_DPS_DATE_FORMAT_TYPE2:// DD hh:mm:ss
						snprintf(dscParaValue.date, sizeof(dscParaValue.date),"%c%c %c%c:%c%c:%c%c", DateBuf[6], DateBuf[7], DateBuf[8], \
						DateBuf[9], DateBuf[10], DateBuf[11], DateBuf[12], DateBuf[13]);
						dscParaValue.date[11]=0;
						break;
					case FJ_DPS_DATE_FORMAT_TYPE3:// YYYY/MM/DD hh:mm:ss
						snprintf(dscParaValue.date, sizeof(dscParaValue.date),"%c%c%c%c/%c%c/%c%c %c%c:%c%c:%c%c", DateBuf[0], DateBuf[1], DateBuf[2], \
						DateBuf[3], DateBuf[4], DateBuf[5], DateBuf[6], DateBuf[7], DateBuf[8], \
						DateBuf[9], DateBuf[10], DateBuf[11], DateBuf[12], DateBuf[13]);
						dscParaValue.date[19]=0;
						break;
					case FJ_DPS_DATE_FORMAT_TYPE4:// DD/MM/YY
						snprintf(dscParaValue.date, sizeof(dscParaValue.date),"%c%c/%c%c/%c%c", DateBuf[6], DateBuf[7], DateBuf[4], \
						DateBuf[5], DateBuf[2], DateBuf[3]);
						dscParaValue.date[8]=0;
						break;
					case FJ_DPS_DATE_FORMAT_TYPE5:// DD/MM/YYYY
						snprintf(dscParaValue.date, sizeof(dscParaValue.date),"%c%c/%c%c/%c%c%c%c", DateBuf[6], DateBuf[7], DateBuf[4], \
						DateBuf[5], DateBuf[0], DateBuf[1], DateBuf[2], DateBuf[3]);
						dscParaValue.date[10]=0;
						break;
					case FJ_DPS_DATE_FORMAT_TYPE6:// DD/MM/YYYY hh:mm:ss
						snprintf(dscParaValue.date, sizeof(dscParaValue.date),"%c%c/%c%c/%c%c%c%c %c%c:%c%c:%c%c", DateBuf[6], DateBuf[7], DateBuf[4], \
						DateBuf[5], DateBuf[0], DateBuf[1], DateBuf[2], DateBuf[3], \
						DateBuf[8], DateBuf[9], DateBuf[10], DateBuf[11], DateBuf[12], \
						DateBuf[13]);
						dscParaValue.date[19]=0;
						break;
					case FJ_DPS_DATE_FORMAT_TYPE7:// MM/DD/YY
						snprintf(dscParaValue.date, sizeof(dscParaValue.date),"%c%c/%c%c/%c%c", DateBuf[4], DateBuf[5], DateBuf[6], \
						DateBuf[7], DateBuf[2], DateBuf[3]);
						dscParaValue.date[8]=0;
						break;
					case FJ_DPS_DATE_FORMAT_TYPE8:// MM/DD/YYYY
						snprintf(dscParaValue.date, sizeof(dscParaValue.date),"%c%c/%c%c/%c%c%c%c", DateBuf[4], DateBuf[5], DateBuf[6], \
						DateBuf[7], DateBuf[0], DateBuf[1], DateBuf[2], DateBuf[3]);
						dscParaValue.date[10]=0;
						break;
					case FJ_DPS_DATE_FORMAT_TYPE9:// MM/DD/YYYY hh:mm:ss
						snprintf(dscParaValue.date, sizeof(dscParaValue.date),"%c%c/%c%c/%c%c%c%c %c%c:%c%c:%c%c", DateBuf[4], DateBuf[5], DateBuf[6], \
						DateBuf[7], DateBuf[0], DateBuf[1], DateBuf[2], DateBuf[3], \
						DateBuf[8], DateBuf[9], DateBuf[10], DateBuf[11], DateBuf[12], \
						DateBuf[13]);
						dscParaValue.date[19]=0;
						break;
					default:
						snprintf(dscParaValue.date, sizeof(dscParaValue.date),"%c%c/%c%c/%c%c", DateBuf[2], DateBuf[3], DateBuf[4], DateBuf[5], \
						DateBuf[6], DateBuf[7]);
						dscParaValue.date[8]=0;
						break;
				}
				if(aryfnDpsCbFuncEntry[FJ_DPS_CB_FUNCTION_DATETIME])
				{
					memcpy(szTmpDate, dscParaValue.date, sizeof(szTmpDate)); 
					aryfnDpsCbFuncEntry[FJ_DPS_CB_FUNCTION_DATETIME]((VOID*)szTmpDate, (VOID*)0); /* Callback to user area */
					/* If user area has modify szTmpDate value, replace the dscParaValue.date */
					if(memcmp(dscParaValue.date,szTmpDate, sizeof(szTmpDate))){memcpy(dscParaValue.date, szTmpDate, sizeof(szTmpDate));}
					//if(strlen(szTmpDate) > 0){memcpy(dscParaValue.date, szTmpDate, sizeof(szTmpDate));}
				}
				ptr = ResponsePara(ptr,dscParaString[D_DATE].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_DATE].saveOffset),dscParaString[D_DATE].type,dscParaString[D_DATE].quantity);	
			}
			if(dscParaValue.cropping == XML_CROPPING_ON) 
			{
				dscParaValue.croppingArea[0] = dps_MarkFile.PrintInfo[i].usX;
				dscParaValue.croppingArea[1] = dps_MarkFile.PrintInfo[i].usY;
				dscParaValue.croppingArea[2] = dps_MarkFile.PrintInfo[i].usW;
				dscParaValue.croppingArea[3] = dps_MarkFile.PrintInfo[i].usH;
				ptr = ResponsePara(ptr,dscParaString[D_CROPPINGAREA].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_CROPPINGAREA].saveOffset),dscParaString[D_CROPPINGAREA].type,dscParaString[D_CROPPINGAREA].quantity);	
			}	
			ptr = CopyString(ptr,printInfoString,0,S_ARROR2_R);
		}
		else
		{
			dscParaValue.copies = 1; /* Fix to 1 for duplicate option */
			if(dscParaValue.fileNamePrint == XML_FILENAME_PRINT_ON) 
			{	
				Index = PTP_FindObjectIndex(PictBridgeUI_MediaSelected, dps_MarkFile.FATData[i]);
				BF_Dcf_If_Set_Current_Dir(NULL, D_BF_DCF_IF_PRM_NORMAL_ACTION, (Index & 0xFFFF0000));
				BF_Dcf_If_Get_Index_File_Path(Index, "JPG", srcfilepath);		
				//memcpy(srcfilepath, DRIVE_TAG, strlen(DRIVE_TAG));
				//srcfilepath[7] = '\\';
				//srcfilepath[16] = '\\';
				strncpy(dscParaValue.fileName, (const CHAR*)(srcfilepath + strlen((const CHAR*)srcfilepath) - 12), sizeof(dscParaValue.fileName));
			}	
			if(dscParaValue.datePrint == XML_DATEPRINT_ON)
			{
				CHAR szTmpDate[26];
				Index = PTP_FindObjectIndex(PictBridgeUI_MediaSelected, dps_MarkFile.FATData[i]);
				BF_Dcf_If_Set_Current_Dir(NULL, D_BF_DCF_IF_PRM_NORMAL_ACTION, (Index & 0xFFFF0000));	
				//memcpy(srcfilepath, DRIVE_TAG, strlen(DRIVE_TAG));
				//srcfilepath[7] = '\\';
				//srcfilepath[16] = '\\';
				BF_Dcf_If_Get_Exif_Info_Jpeg(Index, &img_detail, &img_data);
				snprintf((CHAR *)DateBuf, sizeof(DateBuf), "%04d%02d%02d%02d%02d%02d", img_detail.DateTimeOriginal.ad_year, img_detail.DateTimeOriginal.month, img_detail.DateTimeOriginal.day, \
				img_detail.DateTimeOriginal.hour, img_detail.DateTimeOriginal.min, img_detail.DateTimeOriginal.sec);
				//BF_Dcf_If_Get_Index_File_Path(Index, "JPG", srcfilepath);
				//BF_Fs_If_Stat((const FW_CHAR *)srcfilepath, &fsstat);
				//sprintf(DateBuf, "%04d%02d%02d%02d%02d%02d", (fsstat.date >> 9) + 1980, (fsstat.date >> 5) & 0x0f, (fsstat.date) & 0x1f, 
				//(fsstat.time >> 11) & 0x1f, (fsstat.time >> 5) & 0x3f, (fsstat.time & 0x1f) * 2);
				switch(DPSdateType)
				{
					case FJ_DPS_DATE_FORMAT_TYPE0:// YY/MM/DD
						snprintf(dscParaValue.date, sizeof(dscParaValue.date), "%c%c/%c%c/%c%c", DateBuf[2], DateBuf[3], DateBuf[4], DateBuf[5], \
						DateBuf[6], DateBuf[7]);
						dscParaValue.date[8]=0;
						break;
					case FJ_DPS_DATE_FORMAT_TYPE1:// YYYY/MM/DD
						snprintf(dscParaValue.date, sizeof(dscParaValue.date), "%c%c%c%c/%c%c/%c%c", DateBuf[0], DateBuf[1], DateBuf[2], \
						DateBuf[3], DateBuf[4], DateBuf[5], \
						DateBuf[6], DateBuf[7]);
						dscParaValue.date[10]=0;
						break;
					case FJ_DPS_DATE_FORMAT_TYPE2:// DD hh:mm:ss
						snprintf(dscParaValue.date, sizeof(dscParaValue.date), "%c%c %c%c:%c%c:%c%c", DateBuf[6], DateBuf[7], DateBuf[8], \
						DateBuf[9], DateBuf[10], DateBuf[11], DateBuf[12], DateBuf[13]);
						dscParaValue.date[11]=0;
						break;
					case FJ_DPS_DATE_FORMAT_TYPE3:// YYYY/MM/DD hh:mm:ss
						snprintf(dscParaValue.date, sizeof(dscParaValue.date), "%c%c%c%c/%c%c/%c%c %c%c:%c%c:%c%c", DateBuf[0], DateBuf[1], DateBuf[2], \
						DateBuf[3], DateBuf[4], DateBuf[5], DateBuf[6], DateBuf[7], DateBuf[8], \
						DateBuf[9], DateBuf[10], DateBuf[11], DateBuf[12], DateBuf[13]);
						dscParaValue.date[19]=0;
						break;
					case FJ_DPS_DATE_FORMAT_TYPE4:// DD/MM/YY
						snprintf(dscParaValue.date, sizeof(dscParaValue.date), "%c%c/%c%c/%c%c", DateBuf[6], DateBuf[7], DateBuf[4], \
						DateBuf[5], DateBuf[2], DateBuf[3]);
						dscParaValue.date[8]=0;
						break;
					case FJ_DPS_DATE_FORMAT_TYPE5:// DD/MM/YYYY
						snprintf(dscParaValue.date, sizeof(dscParaValue.date), "%c%c/%c%c/%c%c%c%c", DateBuf[6], DateBuf[7], DateBuf[4], \
						DateBuf[5], DateBuf[0], DateBuf[1], DateBuf[2], DateBuf[3]);
						dscParaValue.date[10]=0;
						break;
					case FJ_DPS_DATE_FORMAT_TYPE6:// DD/MM/YYYY hh:mm:ss
						snprintf(dscParaValue.date, sizeof(dscParaValue.date), "%c%c/%c%c/%c%c%c%c %c%c:%c%c:%c%c", DateBuf[6], DateBuf[7], DateBuf[4], \
						DateBuf[5], DateBuf[0], DateBuf[1], DateBuf[2], DateBuf[3], \
						DateBuf[8], DateBuf[9], DateBuf[10], DateBuf[11], DateBuf[12], \
						DateBuf[13]);
						dscParaValue.date[19]=0;
						break;
					case FJ_DPS_DATE_FORMAT_TYPE7:// MM/DD/YY
						snprintf(dscParaValue.date, sizeof(dscParaValue.date), "%c%c/%c%c/%c%c", DateBuf[4], DateBuf[5], DateBuf[6], \
						DateBuf[7], DateBuf[2], DateBuf[3]);
						dscParaValue.date[8]=0;
						break;
					case FJ_DPS_DATE_FORMAT_TYPE8:// MM/DD/YYYY
						snprintf(dscParaValue.date, sizeof(dscParaValue.date), "%c%c/%c%c/%c%c%c%c", DateBuf[4], DateBuf[5], DateBuf[6], \
						DateBuf[7], DateBuf[0], DateBuf[1], DateBuf[2], DateBuf[3]);
						dscParaValue.date[10]=0;
						break;
					case FJ_DPS_DATE_FORMAT_TYPE9:// MM/DD/YYYY hh:mm:ss
						snprintf(dscParaValue.date, sizeof(dscParaValue.date), "%c%c/%c%c/%c%c%c%c %c%c:%c%c:%c%c", DateBuf[4], DateBuf[5], DateBuf[6], \
						DateBuf[7], DateBuf[0], DateBuf[1], DateBuf[2], DateBuf[3], \
						DateBuf[8], DateBuf[9], DateBuf[10], DateBuf[11], DateBuf[12], \
						DateBuf[13]);
						dscParaValue.date[19]=0;
						break;
					default:
						snprintf(dscParaValue.date, sizeof(dscParaValue.date), "%c%c/%c%c/%c%c", DateBuf[2], DateBuf[3], DateBuf[4], DateBuf[5], \
						DateBuf[6], DateBuf[7]);
						dscParaValue.date[8]=0;
						break;
				}
				if(aryfnDpsCbFuncEntry[FJ_DPS_CB_FUNCTION_DATETIME])
				{
					memcpy(szTmpDate, dscParaValue.date, sizeof(szTmpDate)); 
					aryfnDpsCbFuncEntry[FJ_DPS_CB_FUNCTION_DATETIME]((VOID*)szTmpDate, (VOID*)0); /* Callback to user area */
					/* If user area has modify szTmpDate value, replace the dscParaValue.date */
					if(memcmp(dscParaValue.date,szTmpDate, sizeof(szTmpDate))){memcpy(dscParaValue.date, szTmpDate, sizeof(szTmpDate));}
					//if(strlen(szTmpDate) > 0){memcpy(dscParaValue.date, szTmpDate, sizeof(szTmpDate));}
				}
			}
			if(dscParaValue.cropping == XML_CROPPING_ON) 
			{
				dscParaValue.croppingArea[0] = dps_MarkFile.PrintInfo[i].usX;
				dscParaValue.croppingArea[1] = dps_MarkFile.PrintInfo[i].usY;
				dscParaValue.croppingArea[2] = dps_MarkFile.PrintInfo[i].usW;
				dscParaValue.croppingArea[3] = dps_MarkFile.PrintInfo[i].usH;
			}	
			for(n = 0;n < (dps_MarkFile.PrintInfo[i]).usCopies; n++)
			{
				ptr = CopyString(ptr,printInfoString,0,ARROW2_R);
				ptr = ResponsePara(ptr,dscParaString[D_FILEID].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_FILEID].saveOffset),dscParaString[D_FILEID].type,dscParaString[D_FILEID].quantity);
				ptr = ResponsePara(ptr,dscParaString[D_COPIES].paraString,(CHAR *)&(dscParaValue.copies),TYPE_D,1);		
				if(dscParaValue.fileNamePrint == XML_FILENAME_PRINT_ON)
				{
					ptr = ResponsePara(ptr,dscParaString[D_FILENAME].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_FILENAME].saveOffset),dscParaString[D_FILENAME].type,dscParaString[D_FILENAME].quantity);	
				}	
				if(dscParaValue.datePrint == XML_DATEPRINT_ON)
				{
					ptr = ResponsePara(ptr,dscParaString[D_DATE].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_DATE].saveOffset),dscParaString[D_DATE].type,dscParaString[D_DATE].quantity);
				}			
				if(dscParaValue.cropping == XML_CROPPING_ON)
				{
					ptr = ResponsePara(ptr,dscParaString[D_CROPPINGAREA].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_CROPPINGAREA].saveOffset),dscParaString[D_CROPPINGAREA].type,dscParaString[D_CROPPINGAREA].quantity);
				}
				ptr = CopyString(ptr,printInfoString,0,S_ARROR2_R);
			}
		}
	}
	ptr = CopyString(ptr,(CHAR *)(&opString[DPS_STARTJOB]),0,S_ARROR2_R);
	ptr = ResponseXmlTail(ptr,DSC_REQUEST);
	ResetObjectPrintInfo(); /* Clear object print info */
	if((prtParaValue.newJobOK) && GetBusReadySignal() && (InDpsStarting == 0))
	{
		IssueRequest(ptr - dscReqBufPtr);
	}
	else
	{
		return 1;
	}
#endif
	return 0;
}


USHORT RequestStartJobDpof()
{ 
	CHAR *ptr;
	ULONG ulOriFileType = 0;
	GetDpsOutputBuf(DSC_REQUEST);
	
	if(gBF_Ptp_Buff->Object_DPOF.DcfHandle != FIL_DPOF)
	{
		MSG outmsg = {ePictBridgeSubState_Unrecognized_Operation, 0, (VOID*)0, {0}};
		FJ_PictBridgeSendHostMessage(outmsg);
		return 1;
	}
	else
	{
		ptr = ResponseXmlHead(dscReqBufPtr,DSC_REQUEST);
		ptr = CopyString(ptr,(CHAR *)(&opString[DPS_STARTJOB]),0,ARROW2_R);
		//jobConfig
		ptr = CopyString(ptr,jobConfigString,0,ARROW2_R);
#if 0
		if(dscParaValue.quality)
		{
			ptr = ResponsePara(ptr,dscParaString[D_QUALITY].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_QUALITY].saveOffset),dscParaString[D_QUALITY].type,dscParaString[D_QUALITY].quantity);
		}
		if(dscParaValue.paperSize)
		{
			ptr = ResponsePara(ptr,dscParaString[D_PAPERSIZE].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_PAPERSIZE].saveOffset),dscParaString[D_PAPERSIZE].type,dscParaString[D_PAPERSIZE].quantity);
		}
		if(dscParaValue.paperType)
		{
			ptr = ResponsePara(ptr,dscParaString[D_PAPERTYPE].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_PAPERTYPE].saveOffset),dscParaString[D_PAPERTYPE].type,dscParaString[D_PAPERTYPE].quantity);
		}
#endif
		ulOriFileType = dscParaValue.fileType;
		dscParaValue.fileType = XML_FILETYPE_DPOF;
		if(dscParaValue.fileType)
		{
			ptr = ResponsePara(ptr,dscParaString[D_FILETYPE].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_FILETYPE].saveOffset),dscParaString[D_FILETYPE].type,dscParaString[D_FILETYPE].quantity);
		}
		dscParaValue.fileType = ulOriFileType;
#if 0
		if(dscParaValue.datePrint)
		{
			ptr = ResponsePara(ptr,dscParaString[D_DATEPRINT].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_DATEPRINT].saveOffset),dscParaString[D_DATEPRINT].type,dscParaString[D_DATEPRINT].quantity);
		}
		if(dscParaValue.fileNamePrint) 
		{
			ptr = ResponsePara(ptr,dscParaString[D_FILENAME].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_FILENAME].saveOffset),dscParaString[D_FILENAME].type,dscParaString[D_FILENAME].quantity);	
		}	
		if(dscParaValue.imageOptimize)
		{
			ptr = ResponsePara(ptr,dscParaString[D_IMAGEOPTIMIZE].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_IMAGEOPTIMIZE].saveOffset),dscParaString[D_IMAGEOPTIMIZE].type,dscParaString[D_IMAGEOPTIMIZE].quantity);	
		}
		if(dscParaValue.layout)
		{
			ptr = ResponsePara(ptr,dscParaString[D_LAYOUT].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_LAYOUT].saveOffset),dscParaString[D_LAYOUT].type,dscParaString[D_LAYOUT].quantity);	
		}
		if(dscParaValue.fixedSize)
		{
			ptr = ResponsePara(ptr,dscParaString[D_FIXEDSIZE].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_FIXEDSIZE].saveOffset),dscParaString[D_FIXEDSIZE].type,dscParaString[D_FIXEDSIZE].quantity);	
		}
		//dscParaValue.cropping = (dps_MarkFile.PrintInfo[i]).uCrop;
		if(dscParaValue.cropping) 
		{
			ptr = ResponsePara(ptr,dscParaString[D_CROPPING].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_CROPPING].saveOffset),dscParaString[D_CROPPING].type,dscParaString[D_CROPPING].quantity);	
		}	
#endif
		ptr = CopyString(ptr,jobConfigString,0,S_ARROR2_R);
		dscParaValue.fileID = gBF_Ptp_Buff->Object_DPOF.DcfHandle;
		ptr = CopyString(ptr,printInfoString,0,ARROW2_R);
		ptr = ResponsePara(ptr,dscParaString[D_FILEID].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_FILEID].saveOffset),dscParaString[D_FILEID].type,dscParaString[D_FILEID].quantity);	
		ptr = CopyString(ptr,printInfoString,0,S_ARROR2_R);
	}
	ptr = CopyString(ptr,(CHAR *)(&opString[DPS_STARTJOB]),0,S_ARROR2_R);
	ptr = ResponseXmlTail(ptr,DSC_REQUEST);
	ResetObjectPrintInfo(); /* Clear object print info */
	if((prtParaValue.newJobOK) && GetBusReadySignal() && (InDpsStarting == 0))
	{
		IssueRequest(ptr - dscReqBufPtr);
	}
	else
	{
		return 1;
	}
	
	return 0;
}

//this function should be moved to callback.c
VOID SetAbortType(ULONG type)
{
	*(ULONG *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_ABORTSTYLE].saveOffset) = type;	
}
		
USHORT RequestAbortJob()
{
	CHAR *ptr;

       dscParaValue.abortStyle = ABORT_IMM; 
       
	//dpsStatus.operationId = DPS_ABORTJOB;
	GetDpsOutputBuf(DSC_REQUEST);
	
	ptr = ResponseXmlHead(dscReqBufPtr,DSC_REQUEST);

	ptr = CopyString(ptr,(CHAR *)(&opString[DPS_ABORTJOB]),0,ARROW2_R);
	ptr = ResponsePara(ptr,dscParaString[D_ABORTSTYLE].paraString,(CHAR *)((ULONG)dscParaValuePtr + (ULONG)dscParaString[D_ABORTSTYLE].saveOffset),dscParaString[D_ABORTSTYLE].type,dscParaString[D_ABORTSTYLE].quantity);
	ptr = CopyString(ptr,(CHAR *)(&opString[DPS_ABORTJOB]),0,S_ARROR2_R);

	ptr = ResponseXmlTail(ptr,DSC_REQUEST);
	if(GetBusReadySignal() && (InDpsStarting == 0))
	{
		IssueRequest(ptr - dscReqBufPtr);
	}
	else
	{
		return 1;
	}

	return 0;
	
}
	
USHORT Asyn_RequestAbortJob()
{
	MSG RequestMessage = {ePictBridgeSubCmd_SetPrintAction, (ULONG)FJ_PICT_ACTION_ABORT, (VOID*)0};
	if(GetBusReadySignal() && (InDpsStarting == 0))
	{
		SendMessage(RequestMessage, eDpsSubSysMQ, kMsgWaitInfinity);
		return 0;
	}
	else
	{
		return 1;
	}
}	

USHORT RequestContinueJob()
{
	CHAR *ptr;

	//dpsStatus.operationId = DPS_CONTINUEJOB;
	GetDpsOutputBuf(DSC_REQUEST);
	
	ptr = ResponseXmlHead(dscReqBufPtr,DSC_REQUEST);
	ptr = CopyString(ptr,opString[DPS_CONTINUEJOB].operationString, 0,ARROR2_S_R);
	ptr = ResponseXmlTail(ptr,DSC_REQUEST);
	
	if(GetBusReadySignal() && (InDpsStarting == 0))
	{
		IssueRequest(ptr - dscReqBufPtr);
	}
	else
	{
		return 1;
	}
	
	return 0;
}

USHORT Asyn_RequestContinueJob()
{
	MSG RequestMessage = {ePictBridgeSubCmd_SetPrintAction, (ULONG)FJ_PICT_ACTION_CONTINUE, (VOID*)0};
	if(GetBusReadySignal() && (InDpsStarting == 0))
	{
		SendMessage(RequestMessage, eDpsSubSysMQ, kMsgWaitInfinity);
		return 0;
	}
	else
	{
		return 1;
	}
}

USHORT RequestStoreAdded()
{
	OS_User_Set_Flg(FID_DPSTSK, FLG_DPSDSC_STORE_ADDED);

	return 1;
}

USHORT RequestStoreRemoved()
{
	OS_User_Set_Flg(FID_DPSTSK, FLG_DPSDSC_STORE_REMOVED);

	return 1;
}
		
UCHAR DpsLinked()
{   
	UCHAR	ret;
	extern VOID CopySerialNumber(CHAR *);        
	
	CopyString(dscParaValue.serialNo,"TEST SERIAL NO\0",0,0);
	
// Warning	if(ret=RequestConfigurePrintService())	return ret;
	if((ret=RequestConfigurePrintService()) != 0)	return ret;
	OS_User_Tslp_Tsk(1000);
	InDpsStarting=1;
// Warning	if((ret=GetNecessaryCapability()==1))
	if((ret=GetNecessaryCapability())==1)
	{
		InDpsStarting=0;
		return ret;
	}
	InDpsStarting=0;
	
	return 0;//Succeed
}

VOID ResetCapabilityValue()
{
	memset(&(prtParaValue.paperSize), 0, sizeof(prtParaValue.paperSize));
	memset(&(prtParaValue.layout), 0, sizeof(prtParaValue.layout));
	memset(&(prtParaValue.datePrints), 0, sizeof(prtParaValue.datePrints));
	memset(&(prtParaValue.fileNamePrints), 0, sizeof(prtParaValue.fileNamePrints));
	memset(&(prtParaValue.imageOptimizes), 0, sizeof(prtParaValue.imageOptimizes));
	memset(&(prtParaValue.croppings), 0, sizeof(prtParaValue.croppings));
	memset(&(prtParaValue.fileTypes), 0, sizeof(prtParaValue.fileTypes));
	memset(&(prtParaValue.qualities), 0, sizeof(prtParaValue.qualities));
	memset(&(prtParaValue.paperType), 0, sizeof(prtParaValue.paperType));
	memset(&(prtParaValue.paperFixedSize), 0, sizeof(prtParaValue.paperFixedSize));
}

ULONG ProcessDPSEvent()
{
volatile	ULONG returnValue=0;
MSG outmsg;

	returnValue = 0;		 
 			switch(dpsStatus.DPSKeyAction)
			{
				case DPS_LINKED:
					dpsStatus.DPSKeyAction = 0;  //Andy Jing 061228
					if(!DpsLinked())
					{
						returnValue = 0x8000;
						outmsg.msgid = ePictBridgeSubState_Printer_Connected;
					}else{
						returnValue = 0xA000;  //0x2000 | 0x8000
						outmsg.msgid = ePictBridgeSubState_Printer_Connected;
					}	
					FJ_PictBridgeSendHostMessage(outmsg);
				break;
				case RCV_DPS_NOTIFYDEVICESTATUS:
					//===============================================================
					//don't change the order of check 
					//decide by prtParaValue
					dpsStatus.DPSKeyAction = 0;
					UpdateDeviceStatus();
					//capability changed	
					if(prtParaValue.capabilityChanged && dpsStatus.capabilityChanged == 0)
					{
						if(!dpsStatus.printerError)
						{							
							dscParaValue.fileID=0;
							OS_User_Tslp_Tsk(600);
							InDpsStarting = 1;
							ResetCapabilityValue();
							if(GetNecessaryCapability()==1) 
							{
								InDpsStarting = 0;
								returnValue = 0xA000;  //0x2000 | 0x8000
							}
							InDpsStarting = 0;
							returnValue = 0x8000;										
						}
					}                
					//print complete,must after check needCheckJobAgain to returnValue=0x8000
					if(prtParaValue.newJobOK){
						if(needCheckJobAgain)
							if(RequestStartJob()) //PS:it will clear need needCheckJobAgain
								returnValue = 0xA000;
					}			
				break;
				case RCV_DPS_NOTIFYJOBSTATUS:
					dpsStatus.DPSKeyAction = 0;
					UpdateJobStatus();
					returnValue = 0x8000;
				break;
			default:
				break;
			}
	return returnValue;		
}

ULONG  UpdateDeviceStatus()
{
	ULONG nReturnValue = 0;
	MSG outmsg = {0,0,(VOID*)0,{0}};
	if(dpsStatus.dpsPrintServiceStatus != prtParaValue.dpsPrintServiceStatus)
	{
		switch(prtParaValue.dpsPrintServiceStatus)
		{
			case PRINT_IDLE:
				if(dpsStatus.dpsPrintServiceStatus == PRINT_PRINTING)/* Printing -> Idle */
				{
					outmsg.msgid = ePictBridgeSubState_Printer_Status;
					outmsg.ulPara1 = (prtParaValue.dpsPrintServiceStatus) >> 8;
					FJ_PictBridgeSendHostMessage(outmsg);
					outmsg.msgid = ePictBridgeSubState_Printer_Job_Complete_Ok;
					FJ_PictBridgeSendHostMessage(outmsg);
				}
				else /* Paused -> Idle */
				{
					outmsg.msgid = ePictBridgeSubState_Printer_Status;
					outmsg.ulPara1 = (prtParaValue.dpsPrintServiceStatus) >> 8;
					FJ_PictBridgeSendHostMessage(outmsg);
					/* For Ability need */
					outmsg.msgid = ePictBridgeSubState_Printer_Status;
					outmsg.ulPara1 = (ERROR_STATUS_NONE) >> 8;
					FJ_PictBridgeSendHostMessage(outmsg);
				}
				if(prtParaValue.errorStatus == ERROR_STATUS_NONE) /* Idle no error */
				{
					outmsg.msgid = ePictBridgeSubState_Printer_Status;
					outmsg.ulPara1 = (prtParaValue.errorStatus) >> 8;
					FJ_PictBridgeSendHostMessage(outmsg);
				}
				dpsStatus.dpsCurrentPage = 0; /* Reset current page value */  
				dpsStatus.dpsTotalPage = 0;   /* Reset total page value */ 
				break;
			case PRINT_PRINTING:
				if(dpsStatus.dpsPrintServiceStatus == PRINT_PAUSED)/* Paused -> Printing */
				{
					outmsg.msgid = ePictBridgeSubState_Printer_Job_Continue;
					FJ_PictBridgeSendHostMessage(outmsg);
				}
				else/* Idle -> Printing */
				{
					outmsg.msgid = ePictBridgeSubState_Printer_Status;
					outmsg.ulPara1 = (prtParaValue.dpsPrintServiceStatus) >> 8;
					FJ_PictBridgeSendHostMessage(outmsg);
				}
				break;
			case PRINT_PAUSED:
				outmsg.msgid = ePictBridgeSubState_Printer_Status;
				outmsg.ulPara1 = (prtParaValue.dpsPrintServiceStatus) >> 8;
				FJ_PictBridgeSendHostMessage(outmsg);
				break;
		}
		dpsStatus.dpsPrintServiceStatus = prtParaValue.dpsPrintServiceStatus;
	}
	if(dpsStatus.dpsjobEndReason != prtParaValue.jobEndReason)
	{
		outmsg.msgid = ePictBridgeSubState_Printer_Status;
		outmsg.ulPara1 = (prtParaValue.jobEndReason) >> 8;
		FJ_PictBridgeSendHostMessage(outmsg);
		dpsStatus.dpsjobEndReason = prtParaValue.jobEndReason;
	}
	if(dpsStatus.dpsErrorStatus != prtParaValue.errorStatus)
	{
		FJ_PRINTER_ERROR_REASON ErrCode = PRINTER_NO_ERROR;
		switch(prtParaValue.errorStatus)
		{
			case ERROR_STATUS_NONE:
				outmsg.msgid = ePictBridgeSubState_Error_Cleared;
				break;
			case ERROR_STATUS_WARNING:
				outmsg.msgid = ePictBridgeSubState_Printer_Recoverable_Error;
				ErrCode = (FJ_PRINTER_ERROR_REASON)((prtParaValue.errorReason) & 0xfffff); 
				outmsg.ulPara1 = ErrCode;
				break;
			case ERROR_STATUS_ERROR:
				outmsg.msgid = ePictBridgeSubState_Printer_Fatal_Error;
				ErrCode = (FJ_PRINTER_ERROR_REASON)((prtParaValue.errorReason) & 0xfffff); 
				outmsg.ulPara1 = ErrCode;
				break;
		}
		dpsStatus.dpsErrorStatus = prtParaValue.errorStatus;
		FJ_PictBridgeSendHostMessage(outmsg);
	}
	if(dpsStatus.dpsErrorReason != prtParaValue.errorReason)
	{
		outmsg.msgid = ePictBridgeSubState_Printer_Status;
		outmsg.ulPara1 = (prtParaValue.errorReason) >> 8;
		FJ_PictBridgeSendHostMessage(outmsg);
		dpsStatus.dpsErrorReason = prtParaValue.errorReason;
	}
	if(dpsStatus.dpsDisconnctEnabled != prtParaValue.disconnectEnable)
	{
		outmsg.msgid = ePictBridgeSubState_Printer_Status;
		outmsg.ulPara1 = ((prtParaValue.disconnectEnable) == 1)?XML_DISCONNECTENABLE_TRUE:XML_DISCONNECTENABLE_FALSE;
		FJ_PictBridgeSendHostMessage(outmsg);
		dpsStatus.dpsDisconnctEnabled = prtParaValue.disconnectEnable;
	}
	if(dpsStatus.dpsCapabilityChanged != prtParaValue.capabilityChanged)
	{
		outmsg.msgid = ePictBridgeSubState_Printer_Status;
		outmsg.ulPara1 = ((prtParaValue.capabilityChanged) == 1)?XML_CAPABILITYCHANGE_TRUE:XML_CAPABILITYCHANGE_FALSE;
		FJ_PictBridgeSendHostMessage(outmsg);
		dpsStatus.dpsCapabilityChanged = prtParaValue.capabilityChanged;
	}
	if(dpsStatus.dpsNewJobOK != prtParaValue.newJobOK)
	{
		outmsg.msgid = ePictBridgeSubState_Printer_Status;
		outmsg.ulPara1 = ((prtParaValue.newJobOK) == 1)?XML_NEWJOBOK_TRUE:XML_NEWJOBOK_FALSE;
		FJ_PictBridgeSendHostMessage(outmsg);
		dpsStatus.dpsNewJobOK = prtParaValue.newJobOK;
	}
	return nReturnValue;
}

ULONG  UpdateJobStatus()
{
	ULONG nReturnValue = 0;
	if((dpsStatus.dpsCurrentPage != prtParaValue.progress[0]) || \
	(dpsStatus.dpsTotalPage != prtParaValue.progress[1]))
	{
		MSG outmsg;
		dpsStatus.dpsCurrentPage = prtParaValue.progress[0];
		dpsStatus.dpsTotalPage = prtParaValue.progress[1];
		outmsg.msgid = ePictBridgeSubState_Printer_Progress;
		outmsg.ulPara1 = (dpsStatus.dpsCurrentPage) | (dpsStatus.dpsTotalPage << 12);
		FJ_PictBridgeSendHostMessage(outmsg);
	}
	return nReturnValue;
}

UCHAR GetDpsPrinterStatus(FJ_DPS_DEVICE_STATUS* pDpsDeviceStatus)
{
	pDpsDeviceStatus->ServiceStatus = (dpsStatus.dpsPrintServiceStatus) >> 8;
	pDpsDeviceStatus->JobEndReadon  = (dpsStatus.dpsjobEndReason) >> 8;
	pDpsDeviceStatus->ErrorStatus   = (dpsStatus.dpsErrorStatus) >> 8;
	pDpsDeviceStatus->ErrorReason   = (dpsStatus.dpsErrorReason) >> 8;
	pDpsDeviceStatus->DisconnectEnable = (((dpsStatus.dpsDisconnctEnabled) == 1)?XML_DISCONNECTENABLE_TRUE:XML_DISCONNECTENABLE_FALSE);
	pDpsDeviceStatus->CapabilityChanged = (((dpsStatus.dpsCapabilityChanged) == 1)?XML_CAPABILITYCHANGE_TRUE:XML_CAPABILITYCHANGE_FALSE);
	pDpsDeviceStatus->NewJobOK = (((dpsStatus.dpsNewJobOK) == 1)?XML_NEWJOBOK_TRUE:XML_NEWJOBOK_FALSE);
	return 0;
}

UCHAR GetPrinterCapability(FJ_PICT_CAPABILITY_PARAM uCapabilityId, ULONG* Buffer, ULONG uPaperSize)
{
	SHORT sElementCount = 0;
	UCHAR uReturn = 0;
	switch(uCapabilityId)
	{
		case FJ_PICT_PARAM_QUALITY:
			for(; sElementCount < NUMBER_OF_QUALITIES; sElementCount++)
			{
				if(prtParaValue.qualities[sElementCount] != 0)
				{
					*(Buffer + sElementCount + 1) = prtParaValue.qualities[sElementCount];
					continue;
				}
				break;
			}
			break;
		case FJ_PICT_PARAM_PAPER_SIZE:
			for(; sElementCount < NUMBER_OF_PAPERSIZE; sElementCount++)
			{
				if(prtParaValue.paperSize[sElementCount] != 0)
				{
					*(Buffer + sElementCount + 1) = prtParaValue.paperSize[sElementCount];
					continue;
				}
				break;
			}
			break;
		case FJ_PICT_PARAM_PAPER_TYPE:
			{
				INT32 iIndex = -1;
		    	for(; sElementCount < NUMBER_OF_PAPERSIZE; sElementCount++)
				{
					if(prtParaValue.paperSize[sElementCount] == uPaperSize)/* Find it */
					{
						iIndex = sElementCount;
						break;
					}
					continue;
				}
				if(iIndex != -1)
				{
					for(sElementCount = 0; sElementCount < NUMBER_OF_PAPERTYPE; sElementCount++)
					{
						if(prtParaValue.paperType[iIndex][sElementCount] != 0)
						{
							*(Buffer + sElementCount + 1) = prtParaValue.paperType[iIndex][sElementCount];
							continue;
						}		
						break;
					}
				}
				else{uReturn = 1;}
			}
				break;
		case FJ_PICT_PARAM_FILE_TYPE:
			for(; sElementCount < NUMBER_OF_FILETYPE; sElementCount++)
			{
				if(prtParaValue.fileTypes[sElementCount] != 0)
				{
					*(Buffer + sElementCount + 1) = prtParaValue.fileTypes[sElementCount];
					continue;
				}	
				break;
			}
			break;
		case FJ_PICT_PARAM_DATE_PRINT:
			for(; sElementCount < NUMBER_OF_DATEPRINT; sElementCount++)
			{
				if(prtParaValue.datePrints[sElementCount] != 0)
				{
					*(Buffer + sElementCount + 1) = prtParaValue.datePrints[sElementCount];
					continue;
				}
				break;
			}
			break;
		case FJ_PICT_PARAM_FILENAME_PRINT:
			for(; sElementCount < NUMBER_OF_FILENAMEPRINT; sElementCount++)
			{
				if(prtParaValue.fileNamePrints[sElementCount] != 0)
				{
					*(Buffer + sElementCount + 1) = prtParaValue.fileNamePrints[sElementCount];
					continue;
				}	
				break;
			}
			break;
		case FJ_PICT_PARAM_IMAGE_OPTIMIZE:
			for(; sElementCount < NUMBER_OF_IMAGEOPTIMIZE; sElementCount++)
			{
				if(prtParaValue.imageOptimizes[sElementCount] != 0)
				{
					*(Buffer + sElementCount + 1) = prtParaValue.imageOptimizes[sElementCount];
					continue;
				}	
				break;
			}
			break;
		case FJ_PICT_PARAM_FIXED_SIZE:
			for(; sElementCount < NUMBER_OF_PAPERFIXSIZE; sElementCount++)
			{
				if(prtParaValue.paperFixedSize[sElementCount] != 0)
				{
					*(Buffer + sElementCount + 1) = prtParaValue.paperFixedSize[sElementCount];
					continue;
				}	
				break;
			}
			break;
		case FJ_PICT_PARAM_CROPPING:
			for(; sElementCount < NUMBER_OF_CROPPING; sElementCount++)
			{
				if(prtParaValue.croppings[sElementCount] != 0)
				{
					*(Buffer + sElementCount + 1) = prtParaValue.croppings[sElementCount];
					continue;
				}	
				break;
			}
			break;
		case FJ_PICT_PARAM_LAYOUT:
		{
			INT32 iIndex = -1;
		    for(; sElementCount < NUMBER_OF_PAPERSIZE; sElementCount++)
			{
				if(prtParaValue.paperSize[sElementCount] == uPaperSize)/* Find it */
				{
					iIndex = sElementCount;
					break;
				}
				continue;
			}
			if(iIndex != -1)
			{
				for(sElementCount = 0; sElementCount < NUMBER_OF_LAYOUTS; sElementCount++)
				{
					if(prtParaValue.layout[iIndex][sElementCount] != 0)
					{
						*(Buffer + sElementCount + 1) = prtParaValue.layout[iIndex][sElementCount];
						continue;
					}	
					break;
				}
			}
			else{uReturn = 1;}
		}
			break;
		case FJ_PICT_PARAM_DATE_FORMAT:
			*(Buffer + 1) = DPSdateType;
			sElementCount = 1;
			break;
		case FJ_PICT_PARAM_TIMEOUT:
		default: /* Not Support */
			uReturn = 1;
			break;
	}
	*Buffer = sElementCount; /* Set total num of elements */
	return uReturn;
}

UCHAR ConfigPrinterCapability(FJ_CAPABILITY_PROPERTY* pNewCapabilityProperty)
{
	UCHAR uReturn = 0;
	switch(pNewCapabilityProperty->CapabilityType)
	{
		case FJ_PICT_PARAM_QUALITY:
			dscParaValue.quality = pNewCapabilityProperty->ulCapabilityValue;
			break;
		case FJ_PICT_PARAM_PAPER_SIZE:
			dscParaValue.paperSize = pNewCapabilityProperty->ulCapabilityValue;
			break;
		case FJ_PICT_PARAM_PAPER_TYPE:
			dscParaValue.paperType = pNewCapabilityProperty->ulCapabilityValue;
			break;
		case FJ_PICT_PARAM_FILE_TYPE:
			dscParaValue.fileType = pNewCapabilityProperty->ulCapabilityValue;
			break;
		case FJ_PICT_PARAM_DATE_PRINT:
			dscParaValue.datePrint = pNewCapabilityProperty->ulCapabilityValue;
			break;
		case FJ_PICT_PARAM_FILENAME_PRINT:
			dscParaValue.fileNamePrint = pNewCapabilityProperty->ulCapabilityValue;
			break;
		case FJ_PICT_PARAM_IMAGE_OPTIMIZE:
			dscParaValue.imageOptimize = pNewCapabilityProperty->ulCapabilityValue;
			break;
		case FJ_PICT_PARAM_LAYOUT:
			dscParaValue.layout = pNewCapabilityProperty->ulCapabilityValue;
			break;
		case FJ_PICT_PARAM_FIXED_SIZE:
			dscParaValue.fixedSize = pNewCapabilityProperty->ulCapabilityValue;
			break;
		case FJ_PICT_PARAM_CROPPING:
			dscParaValue.cropping = pNewCapabilityProperty->ulCapabilityValue;
			break;
		case FJ_PICT_PARAM_DATE_FORMAT:
			DPSdateType = (FJ_DPS_DATE_FORMAT)(pNewCapabilityProperty->ulCapabilityValue);
			break;
		case FJ_PICT_PARAM_TIMEOUT:
		default:
			uReturn = 1;
			break;
	}
	return uReturn;
}

UCHAR AddPrintedImage(FJ_PRINT_IMAGE_PROPERTY* pPrintImageProperty)
{
	UCHAR uReturn = 1;
#ifdef CO_DCF_EXIF_ON
	T_BF_DCF_IF_FILE_INDEX File_Tbl;
#if 1	// [2009/06/29] Softune Bug
	volatile INT32 i;
#else
	INT32 i;
#endif
	File_Tbl.index = 0;
	File_Tbl.index = pPrintImageProperty->ulFileID;
	if((gBF_Ptp_Buff->ObjectSearchTbl[(File_Tbl.pack.dir_index) - 1].usObjPos) == -1) /* empty dir */   /*	pgr0872	*/
	{
		return uReturn;
	}
	i = (gBF_Ptp_Buff->ObjectSearchTbl[(File_Tbl.pack.dir_index) - 1].usObjPos)+(File_Tbl.pack.obj_index) - 1;   /*	pgr0872	*/
	if(i < 0){return uReturn;}
	if(Object[i].DcfHandle == 0){return uReturn;}
	(Object[i].PrintInfo).usX = pPrintImageProperty->usCropCoordinateX;
	(Object[i].PrintInfo).usY = pPrintImageProperty->usCropCoordinateY;
	(Object[i].PrintInfo).usW = pPrintImageProperty->usCropWidth;
	(Object[i].PrintInfo).usH = pPrintImageProperty->usCropHeight;
	(Object[i].PrintInfo).usCopies = pPrintImageProperty->usCopies;
	(Object[i].PrintInfo).ucPrintOption = pPrintImageProperty->ucPrintOption;
	uReturn = 0;			
#endif
	return uReturn;
}

UCHAR SetCBFunctionEntry(FJ_DPS_CB_FUNCTION_ID uCbFuncID, lpfnDpsCbFuncEntry fnDpsCbFuncEntry)
{
#ifdef CO_DCF_EXIF_ON
	if(uCbFuncID >= FJ_DPS_CB_FUNCTION_SIZE){return 1;}
	aryfnDpsCbFuncEntry[uCbFuncID] = fnDpsCbFuncEntry;
#endif
	return 0;
}

UCHAR SetPICTConfigString(FJ_PRMID_PICT_CONFIGSTRING wParmID, UCHAR* bBuffer, UINT32 dwSize )
{
	UCHAR nReturn = 0;
	switch(wParmID)
	{
		case FJ_PRMID_PICT_CONFIG_VENDOR_NAME_STRING:
			if(dwSize >= SIZE_OF_VENDORNAMESTR)
			{
				nReturn = 1;
			}
			else
			{
				memset(dscParaValue.vendorName, 0, SIZE_OF_VENDORNAMESTR);
				memcpy(dscParaValue.vendorName, bBuffer, dwSize);
			}
			break;
		case FJ_PRMID_PICT_CONFIG_VENDOR_SVERSION_STRING:
			if(dwSize >= SIZE_OF_VSPECIFICVERION)
			{
				nReturn = 1;
			}
			else
			{
				memset(dscParaValue.vendorSpecificVersion, 0, SIZE_OF_VSPECIFICVERION);
				memcpy(dscParaValue.vendorSpecificVersion, bBuffer, dwSize);
			}
			break;
		case FJ_PRMID_PICT_CONFIG_PRODUCT_STRING:
			if(dwSize >= SIZE_OF_PRODUCTNAME)
			{
				nReturn = 1;
			}
			else
			{
				memset(dscParaValue.productName, 0, SIZE_OF_PRODUCTNAME);
				memcpy(dscParaValue.productName, bBuffer, dwSize);
			}
			break;
		default:
			nReturn = 1;
			break;
			
	}
	return nReturn;

}

UCHAR InitThumbnailDataCache(VOID)
{
	UCHAR nReturn = 0;
//	ULONG cache_start_addr = SDRAM_ADR_REL2ABS( (ULONG)SDRAM_ADR_PLAY_THUMB_CACHE_AREA ); 
//	Play_Thumbnail_InitCacheInfo( &cache_start_addr );
	return nReturn;
}

VOID dpstsk( INT32 stacd)
{
	OS_USER_FLGPTN nFlag;
	OS_USER_ER ret;
	InitDpsStatus();
	DpsInitPrtParaValue(TRUE);
	DpsInitDscParaValue();
	while(1)
	{   
//		ret=OS_User_Wai_Flg(&nFlag, FID_DPSTSK, (FLG_DPSEVENT|FLG_DPSDSC|FLG_DPSDSC_STORE_ADDED|FLG_DPSDSC_STORE_REMOVED), D_OS_USER_TWF_ORW|TWF_CLR);
		ret=OS_User_Wai_Flg(FID_DPSTSK, (FLG_DPSEVENT|FLG_DPSDSC|FLG_DPSDSC_STORE_ADDED|FLG_DPSDSC_STORE_REMOVED), D_OS_USER_TWF_ORW, &nFlag);
		ret=OS_User_Clr_Flg(FID_DPSTSK, ~(FLG_DPSEVENT|FLG_DPSDSC|FLG_DPSDSC_STORE_ADDED|FLG_DPSDSC_STORE_REMOVED));
		if((nFlag & FLG_DPSEVENT)&&(ret==D_OS_USER_E_OK))
		{
			OS_User_Clr_Flg(FID_DPSTSK, ~FLG_DPSEVENT);
			switch(dpsStatus.inOperationType){
			case PRT_REQUEST:
				//ret = OS_User_Pol_Flg(&nFlag, FID_DPSTSK, (FLG_DPSPRT2), D_OS_USER_TWF_ORW);
				//if(!(nFlag & FLG_DPSPRT2))
				//{
				//	collisionCnt++;  
				//	printf("collision.....\r\n");     
				// 	break;    		
				//}
				if(dpsStatus.waitState2 != WAIT_REQUEST)
				{	
					ParseXML(dpsInBufferPtr);
					break;
				}
				OS_User_Clr_Flg(FID_DPSTSK, ~FLG_DPSPRT2);
				collisionCnt=0;
				dpsStatus.waitState = WAIT_DSC_RESPONSE; /* set Config_Service completed */
				dpsStatus.waitState2 = WAIT_DSC_RESPONSE;
				if(!ParseXML(dpsInBufferPtr)){
					ResponseUnrecognizedOperation(3);
				}
				else if(prtParaValue.dpsPrintServiceStatus != PRINT_PRINTING &&
					   prtParaValue.dpsPrintServiceStatus != PRINT_IDLE &&
					   prtParaValue.dpsPrintServiceStatus != PRINT_PAUSED){
					ResponseUnrecognizedOperation(2);
			       }
				else
				{
					switch(dpsStatus.operationId)
					{	
						case DPS_GETFILEID:
							OS_User_Tslp_Tsk(120);
							if(dpsStatus.inOperationType == HANDLE_FILE_HREQUEST)
							{
								ResponseGetFileID();
							}
						break;
						case DPS_NOTIFYJOBSTATUS:
						{
							MSG outmsg = {ePictBridgeSubCmd_ProcessDpsEvent, ePictBridgeSubCmd_ProcessDpsEvent, (VOID*)0, {0}}; 
							dpsStatus.DPSKeyAction = RCV_DPS_NOTIFYJOBSTATUS;
							SendMessage(outmsg,eDpsSubSysMQ ,10);
							OS_User_Tslp_Tsk(120);
							if(dpsStatus.inOperationType == HANDLE_FILE_HREQUEST)
							{
								ResponseNotifyJobStatus();
							}
						}
						break;
					    case DPS_NOTIFYDEVICESTATUS:
					    {
					    	MSG outmsg = {ePictBridgeSubCmd_ProcessDpsEvent, ePictBridgeSubCmd_ProcessDpsEvent, (VOID*)0, {0}}; 
							dpsStatus.DPSKeyAction = RCV_DPS_NOTIFYDEVICESTATUS;
							SendMessage(outmsg,eDpsSubSysMQ ,10);
							OS_User_Tslp_Tsk(120);
							if(dpsStatus.inOperationType == HANDLE_FILE_HREQUEST)
							{
								ResponseNotifyDeviceStatus();
							}
						}
						break;
					}
				}
//				OS_User_Wai_Flg(&nFlag, FID_DPSTSK, (FLG_DPSPRT2), D_OS_USER_TWF_ORW);
				OS_User_Wai_Flg(FID_DPSTSK, (FLG_DPSPRT2), D_OS_USER_TWF_ORW, &nFlag);
			break;
			case PRT_RESPONSE:
				if(!ParseXML(dpsInBufferPtr)) /* Printer response isn't RESPONSE_OK */
				{
					MSG outmsg = {ePictBridgeSubState_Printer_Abnormal_Response, 0, (VOID*)0, {0}};
					FJ_PictBridgeSendHostMessage(outmsg);
				}
				dpsStatus.waitState2 = WAIT_REQUEST;
				OS_User_Set_Flg(FID_DPSTSK, FLG_DPSPRT2); //inform primary task that response coming
			break;	
			}
		}
		if((nFlag & FLG_DPSDSC)&&(ret==D_OS_USER_E_OK)){
				dpsStatus.waitState2 = WAIT_PRT_RESPONSE;
				SendDSPCmdFile();
				OS_User_Clr_Flg(FID_DPSTSK, ~FLG_DPSDSC);     
		}
		if((nFlag & FLG_DPSDSC_STORE_ADDED)&&(ret==D_OS_USER_E_OK)){
				SendEvent(PTP_EV_StoreAdded, 0x00010001); 
				OS_User_Clr_Flg(FID_DPSTSK, ~FLG_DPSDSC_STORE_ADDED); 
		}
		if((nFlag & FLG_DPSDSC_STORE_REMOVED)&&(ret==D_OS_USER_E_OK)){
				SendEvent(PTP_EV_StoreRemoved, 0x00010001);
				OS_User_Clr_Flg(FID_DPSTSK, ~FLG_DPSDSC_STORE_REMOVED);
				  
		}
	}
}	

