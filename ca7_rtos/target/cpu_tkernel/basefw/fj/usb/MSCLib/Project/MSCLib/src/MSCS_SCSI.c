/************************************************************************/
/*	<Contents>		USB Mass Storage Sub-class Functions for SCSI		*/
/*	<Filename>		MSCS_SCSI.c											*/
/*	<Enviroment>	Softune V6 for FR-80								*/
/*	<Creation Date/author>												*/
/*		2001/07/06		N.Uenishi										*/
/*	<Latest modify Date/author>											*/
/*		2005/12/28		S.Hibino										*/
/*		2006/06/27		S.Hibino										*/
/*			Moded_Sense(10) Page Code:05hに対しダミーデータを返すように */
/*			した。														*/
/*		2006/12/25		S.Hibino										*/
/*			ユーザーSCSIコマンドへの対応								*/
/*		2007/06/27		S.Hibino										*/
/*			_setSenseKey( DI , 0x06 , 0x28 , 0x00 )を呼び出して関数を	*/
/*			抜ける場合は、DI->MSCS_MEDIUM_CHANGED をクリアするように	*/
/*			した。														*/
/*			応答データ書き込み後、STALLする場合は FIFOが空であるのを	*/
/*			確認してからSTALLするようにした。							*/
/*		2008/07/09		S.Hibino										*/
/*			REALOSのtslp_tskをダイレクトに呼び出さないようにした。		*/
/*			Mode SenseのPage code:20hに対応した。						*/
/* 		2008/08/20		S.Hibino										*/
/*			パーティション以外の領域への書き込みをエラー扱い(Write		*/
/*			Protect)にしてみる											*/
/*		2008/10/27		S.Hibino										*/
/*			サブタスク終了時に待ちタスクがある場合、ダミーメッセージを	*/
/*			書き込むようにした											*/
/*		2008/10/30		S.Hibino										*/
/*			パラレル動作時のメモリ不足への対応を追加。					*/
/*		2010/11/11		J.Tomida										*/
/*			ユーザコマンドのTEST_UNIT_READYが失敗したらEP_STALLを回避	*/
/*		2010/11/16		K.Maeda 										*/
/*			PREVENT ALLOW MEDIUM REMOVALコマンド受信時に無条件で		*/
/*			Failするように変更した。MAC対応のため。						*/
/*			STALLを返さずにFailを行っている。（MACバグ対応）			*/
/*		2011/06/14		J.Tomida										*/
/*			MSCS_AddVendorCmdFunc2のI/FをExternとして公開				*/
/*		2011/08/17		J.Tomida 										*/
/*			PREVENT ALLOW MEDIUM REMOVALコマンド受信時に無条件で		*/
/*			Failするように変更したものを、OKに変更。					*/
/*			NormalEndに変更（WindowsXP対応）							*/
/*		2016/01/26		H.Hata											*/
/*	<Version>    for V2.1												*/
/*							Socionext Middleware Library				*/
/*								Copyright Socionext Inc. 2001-20016		*/
/************************************************************************/
/*      USB_EN_SECTION	Enable Section Definition						*/
/************************************************************************/

/* Section Definition */
#ifdef USB_EN_SECTION
#include "MSC_Section.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <memory.h> /* M-7 */

#include "dd_arm.h"

#include "msc_wrapos.h"
#include "msclib.h"

#include "MSCP_Bulkonly.h"
#include "MSCA.h"
#include "MSCS.h"
#include "MSCS_SCSI.h"

//060210 コンパイルエラーが出たのでDefineした
#include "mwusb_driver4.h"

#ifdef DMA_TCHECK
#include "os_user_custom.h"
#endif

/* USE_UART_MESSAGE : Use UART output  */
//#define USE_UART_MESSAGE

#ifndef USE_UART_MESSAGE
	#ifdef USB_COMPLIANCE_CHECK
		#define USE_UART_MESSAGE
	#endif
#endif

#ifdef USE_UART_MESSAGE
	#define RS_printf printf
#else
    #define RS_printf(...)
#endif

#define MSC_SERIAL_READ
#define MSC_SERIAL_WRITE

/* For Memory release at terminaion task */
static unsigned char *pbuf_rcv_release=(void *)NULL;
static unsigned char *pbuf_snd_release=(void *)NULL;

/* For dummy read */
static unsigned int dummy[256];  /* 1024 byte dummy buffer */


/* Drive Information */
extern MSCS_DriveInfo_t MSCS_DriveInfo[];

extern void MSCA_SelectDrive(unsigned char driveno);

/**************************/
/* Command Function table */
/**************************/
// 2006.11.09 add arg. residue for USBCV 1.3 MSC Test
typedef int (*mscs_cmdtbl_t)(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *DI, Ulong* residue);

static mscs_cmdtbl_t MSCS_CmdTbl[ ][12] = {
	{
		MSCS_Cmd_Inquiry,
		MSCS_Cmd_ModeSense_0x1A,
		MSCS_Cmd_ModeSense_0x5A,
		MSCS_Cmd_PreventAllowMediumRemoval,
		MSCS_Cmd_Read,
		MSCS_Cmd_ReadCapacity,
		MSCS_Cmd_RequestSense,
		MSCS_Cmd_StartStop,
		MSCS_Cmd_TestUnitReady,
		MSCS_Cmd_Verify,
		MSCS_Cmd_Write,
		MSCS_Cmd_Invalid
	},{
		MSCS_Cmd_Inquiry,
		MSCS_Cmd_ModeSense_0x1A,
		MSCS_Cmd_ModeSense_0x5A,
		MSCS_Cmd_PreventAllowMediumRemoval,
		MSCS_Cmd_Read,
		MSCS_Cmd_ReadCapacity,
		MSCS_Cmd_RequestSense,
		MSCS_Cmd_StartStop,
		MSCS_Cmd_TestUnitReady,
		MSCS_Cmd_Verify,
		MSCS_Cmd_Write,
		MSCS_Cmd_Invalid
	},{
		MSCS_Cmd_Inquiry,
		MSCS_Cmd_ModeSense_0x1A,
		MSCS_Cmd_ModeSense_0x5A,
		MSCS_Cmd_PreventAllowMediumRemoval,
		MSCS_Cmd_Read,
		MSCS_Cmd_ReadCapacity,
		MSCS_Cmd_RequestSense,
		MSCS_Cmd_StartStop,
		MSCS_Cmd_TestUnitReady,
		MSCS_Cmd_Verify,
		MSCS_Cmd_Write,
		MSCS_Cmd_Invalid
	},{
		MSCS_Cmd_Inquiry,
		MSCS_Cmd_ModeSense_0x1A,
		MSCS_Cmd_ModeSense_0x5A,
		MSCS_Cmd_PreventAllowMediumRemoval,
		MSCS_Cmd_Read,
		MSCS_Cmd_ReadCapacity,
		MSCS_Cmd_RequestSense,
		MSCS_Cmd_StartStop,
		MSCS_Cmd_TestUnitReady,
		MSCS_Cmd_Verify,
		MSCS_Cmd_Write,
		MSCS_Cmd_Invalid
	},{
		MSCS_Cmd_Inquiry,
		MSCS_Cmd_ModeSense_0x1A,
		MSCS_Cmd_ModeSense_0x5A,
		MSCS_Cmd_PreventAllowMediumRemoval,
		MSCS_Cmd_Read,
		MSCS_Cmd_ReadCapacity,
		MSCS_Cmd_RequestSense,
		MSCS_Cmd_StartStop,
		MSCS_Cmd_TestUnitReady,
		MSCS_Cmd_Verify,
		MSCS_Cmd_Write,
		MSCS_Cmd_Invalid
	},{
		MSCS_Cmd_Inquiry,
		MSCS_Cmd_ModeSense_0x1A,
		MSCS_Cmd_ModeSense_0x5A,
		MSCS_Cmd_PreventAllowMediumRemoval,
		MSCS_Cmd_Read,
		MSCS_Cmd_ReadCapacity,
		MSCS_Cmd_RequestSense,
		MSCS_Cmd_StartStop,
		MSCS_Cmd_TestUnitReady,
		MSCS_Cmd_Verify,
		MSCS_Cmd_Write,
		MSCS_Cmd_Invalid
	},{
		MSCS_Cmd_Inquiry,
		MSCS_Cmd_ModeSense_0x1A,
		MSCS_Cmd_ModeSense_0x5A,
		MSCS_Cmd_PreventAllowMediumRemoval,
		MSCS_Cmd_Read,
		MSCS_Cmd_ReadCapacity,
		MSCS_Cmd_RequestSense,
		MSCS_Cmd_StartStop,
		MSCS_Cmd_TestUnitReady,
		MSCS_Cmd_Verify,
		MSCS_Cmd_Write,
		MSCS_Cmd_Invalid
	},{
		MSCS_Cmd_Inquiry,
		MSCS_Cmd_ModeSense_0x1A,
		MSCS_Cmd_ModeSense_0x5A,
		MSCS_Cmd_PreventAllowMediumRemoval,
		MSCS_Cmd_Read,
		MSCS_Cmd_ReadCapacity,
		MSCS_Cmd_RequestSense,
		MSCS_Cmd_StartStop,
		MSCS_Cmd_TestUnitReady,
		MSCS_Cmd_Verify,
		MSCS_Cmd_Write,
		MSCS_Cmd_Invalid
	}
};

/************ User SCSI Command Table Definition ****************/
// add 2006.12.25 
typedef int (*mscs_user_cmd_t)(void);
typedef int (*mscs_user2_cmd_t)(USBCBW*, unsigned long*);
typedef struct
{
	unsigned char	valid;	// 0:invalid 1:valid
	unsigned char	code;		// Command Code
	unsigned char	type;	// 0: func(void), 1:func(USBCBW*, unsigned long*)
	void*			func;		// Function
} mscs_user_cmdtbl_t;
#define MSCS_MAX_USER_CMD	(64)
static long Mscs_valid_vendcmd = 0;
static mscs_user_cmdtbl_t	MSCS_Uset_CmdTbl[MSCS_MAX_USER_CMD] = {
	{0, 0, 0, NULL}, {0, 0, 0, NULL},{0, 0, 0, NULL}, {0, 0, 0, NULL},
	{0, 0, 0, NULL}, {0, 0, 0, NULL},{0, 0, 0, NULL}, {0, 0, 0, NULL},
	{0, 0, 0, NULL}, {0, 0, 0, NULL},{0, 0, 0, NULL}, {0, 0, 0, NULL},
	{0, 0, 0, NULL}, {0, 0, 0, NULL},{0, 0, 0, NULL}, {0, 0, 0, NULL},
	{0, 0, 0, NULL}, {0, 0, 0, NULL},{0, 0, 0, NULL}, {0, 0, 0, NULL},
	{0, 0, 0, NULL}, {0, 0, 0, NULL},{0, 0, 0, NULL}, {0, 0, 0, NULL},
	{0, 0, 0, NULL}, {0, 0, 0, NULL},{0, 0, 0, NULL}, {0, 0, 0, NULL},
	{0, 0, 0, NULL}, {0, 0, 0, NULL},{0, 0, 0, NULL}, {0, 0, 0, NULL},
	{0, 0, 0, NULL}, {0, 0, 0, NULL},{0, 0, 0, NULL}, {0, 0, 0, NULL},
	{0, 0, 0, NULL}, {0, 0, 0, NULL},{0, 0, 0, NULL}, {0, 0, 0, NULL},
	{0, 0, 0, NULL}, {0, 0, 0, NULL},{0, 0, 0, NULL}, {0, 0, 0, NULL},
	{0, 0, 0, NULL}, {0, 0, 0, NULL},{0, 0, 0, NULL}, {0, 0, 0, NULL},
	{0, 0, 0, NULL}, {0, 0, 0, NULL},{0, 0, 0, NULL}, {0, 0, 0, NULL},
	{0, 0, 0, NULL}, {0, 0, 0, NULL},{0, 0, 0, NULL}, {0, 0, 0, NULL},
	{0, 0, 0, NULL}, {0, 0, 0, NULL},{0, 0, 0, NULL}, {0, 0, 0, NULL},
	{0, 0, 0, NULL}, {0, 0, 0, NULL},{0, 0, 0, NULL}, {0, 0, 0, NULL}
} ;
		
// 2006.12.25
/*  search user cmd table  */
static mscs_user_cmdtbl_t* search_user_cmdtbl(unsigned char code)
{
	mscs_user_cmdtbl_t *tbl;
	int i;
	
	for(i=0, tbl=&MSCS_Uset_CmdTbl[0]; i<Mscs_valid_vendcmd; tbl++)
	{
		if( tbl->valid == 1 )
		{
			if( tbl->code == code )
			{	// Found
				return tbl;
			}
			i++;
		}
	}
	return (mscs_user_cmdtbl_t*)NULL;
}

/* search invalid cmdtbl */
static mscs_user_cmdtbl_t* new_user_cmdtbl(void)
{
	mscs_user_cmdtbl_t *tbl;
	int i;
	
	for(i=0, tbl=&MSCS_Uset_CmdTbl[0]; i<MSCS_MAX_USER_CMD; i++, tbl++)
	{
		if( tbl->valid == 0 )
		{
			// Found npt used table
			return tbl;
		}
	}
	return (mscs_user_cmdtbl_t*)NULL;
}

/******************************************************/
static unsigned char gMedia_become_ready[8] =
{1,1,1,1,1,1,1,1};
void MSCS_AllSetBecomeReady(void)
{
	int i;
	MSCS_DriveInfo_t *DI;

	for( i=0 ; i<MSCS_LUN ; i++ )
	{
		DI=&MSCS_DriveInfo[i];
		DI->MSCS_MEDIUM_EJECT 	= 0;
		DI->MSCS_MEDIUM_REMOVAL	= 0;
		gMedia_become_ready[i] = 1;
	}
}

static void _setSenseKey(MSCS_DriveInfo_t *DI,Uchar SenseKey , Uchar ASC , Uchar ASCQ )
{
	DI->MSCS_SENSEdata.SenseKey = SenseKey;
	DI->MSCS_SENSEdata.ASC = ASC;
	DI->MSCS_SENSEdata.ASCQ = ASCQ;
	DI->MSCS_SENSEdata.AddSenseLength = 0x0A;	/* Bug fix 2002.10.02 */
}

		
static void _setInquiry(MSCS_DriveInfo_t *DI)
{
			
	/* Clear data */
	memset( &(DI->MSCS_INQUIRYdata) , 0 , sizeof(MSCS_INQUIRY_DATA) );
	memset( &(DI->MSCS_INQUIRYdata.Vendor), 0x20 , (8+16+4) );	/* fill blank character */

			
	/* Set Data */
	DI->MSCS_INQUIRYdata.PDT = 0;
	DI->MSCS_INQUIRYdata.byte1.RMB = 1;
	DI->MSCS_INQUIRYdata.byte2.bit.ISO = 0;
	DI->MSCS_INQUIRYdata.byte2.bit.ECMA = 0;
//	DI->MSCS_INQUIRYdata.byte2.bit.ANSI = 2;	/* The device compiles to ANSI X3.131:1994 */
	DI->MSCS_INQUIRYdata.byte2.bit.ANSI = 0;	/* The device compiles to this standard */
//	DI->MSCS_INQUIRYdata.byte2.bit.ANSI = 4;	/* The device compiles to this standard */
	DI->MSCS_INQUIRYdata.byte3.RDF = 2;			/* 2:SCSI-2 standard */
	DI->MSCS_INQUIRYdata.AddLength = 31;
	memcpy( (char *)(DI->MSCS_INQUIRYdata.Vendor) , MSCS_INQ_VENDOR,  10 );
	memcpy( (char *)(DI->MSCS_INQUIRYdata.Product), MSCS_INQ_PRODUCT, 16);
	memcpy( (char *)(DI->MSCS_INQUIRYdata.RevLvl) , MSCS_INQ_REVLVL,  4 );
}

static void _setModePage( MSCS_DriveInfo_t *DI )
{
	MSCS_MODE_DATA_01	*data01;
#if 0
	MSCS_MODE_DATA_05	*data05;
	MSCS_MODE_DATA_08	*data08;
	MSCS_MODE_DATA_1B	*data1B;
	MSCS_MODE_DATA_1C	*data1C;
#endif

	/*********************/
	/* Set Current Value */
	/*********************/
	/* Read-Write Error Recovery Page */
	data01 = &(DI->MSCS_MODEdata01[PC_Current]);
	memset( (Uchar *)data01 , 0 , sizeof(MSCS_MODE_DATA_01) );
	data01->byte0.PS 		= 0;
	data01->byte0.PageCode 	= 0x01;
	data01->PageLength 		= 0x0A;	/* bugfix 2002.03.27 */
	data01->byte2.AWRE		= 1;	/* Automatic write reallocation enable */
	data01->byte2.RC		= 0;
	data01->byte2.PER 		= 0;
	data01->byte2.DCR 		= 0;	/* ECC Error recovery */
	data01->ReadRetryCount 	= DI->MSCS_ReadRetryCount;
	data01->WriteRetryCount = DI->MSCS_WriteRetryCount;

#if 0
	/* Flexible Disk Page */
	data05 = &(DI->MSCS_MODEdata05[PC_Current]);
	memset( (Uchar *)data05 , 0 , sizeof(MSCS_MODE_DATA_05) );
	data05->byte0.PS 		= 0;
	data05->byte0.PageCode 	= 0x05;
	data05->PageLength 		= 0x1E;
	data05->TransferRate	= DI->MSCA_TransferRate;
	data05->NumOfHeads 		= (Uchar)DI->MSCA_IDIdata.current_head;
	data05->SectorPerTrack 	= (Uchar)DI->MSCA_IDIdata.current_sectrack;
	data05->BytePerSector 	= DI->MSCA_IDIdata.bytepersector;
	data05->NumOfCylinders 	= DI->MSCA_IDIdata.current_cylinder;
	data05->MotorOnDelay 	= 0x00;
	data05->MotorOffDelay 	= 0x00;
	data05->RotationRate 	= 0x0000;

	/* Caching Page */
	data08 = &(DI->MSCS_MODEdata08[PC_Current]);
	memset( (Uchar *)data08 , 0 , sizeof(MSCS_MODE_DATA_08) );
	data08->byte0.PS 		= 0;
	data08->byte0.PageCode 	= 0x08;
	data08->PageLength 		= 0x0A;
	data08->byte2.WCE		= 0;	/* Write trough */
	data08->byte2.RCD		= 1;	/* Cache disable */

	/* Removablie Block Access Capabilities Page */
	data1B = &(DI->MSCS_MODEdata1B[PC_Current]);
	memset( (Uchar *)data1B , 0 , sizeof(MSCS_MODE_DATA_1B) );
	data1B->byte0.PS 		= 0;
	data1B->byte0.PageCode 	= 0x1B;
	data1B->PageLength 		= 0x0A;
	data1B->byte2.SRFP		= 0;	/* Not Support reporting format progress */
	data1B->byte3.TLUN		= (MSCS_LUN);	/* Total LUN */

	/* Timer & Protect Page */
	data1C = &(DI->MSCS_MODEdata1C[PC_Current]);
	memset( (Uchar *)data1C , 0 , sizeof(MSCS_MODE_DATA_1C) );
	data1C->byte0.PS 		= 0;
	data1C->byte0.PageCode 	= 0x1C;
	data1C->PageLength 		= 0x06;
	data1C->byte3.ITM 		= 0x03;	/* Inactivity time multiplier = 500ms */
#endif

#if 0	/* Change!! PC is currnet only */
		/************************/
		/* Set Default Value    */
		/************************/
		/* Read-Write Error Recovery Page */
		memcpy( (Uchar *)&(DI->MSCS_MODEdata01[PC_Default]),(Uchar *)&(DI->MSCS_MODEdata01[PC_Current]),sizeof(MSCS_MODE_DATA_01) );
		/* Flexible Disk Page */
		memcpy( (Uchar *)&(DI->MSCS_MODEdata05[PC_Default]),(Uchar *)&(DI->MSCS_MODEdata05[PC_Current]),sizeof(MSCS_MODE_DATA_05) );
		/* Caching Page */
		memcpy( (Uchar *)&(DI->MSCS_MODEdata08[PC_Default]),(Uchar *)&(DI->MSCS_MODEdata08[PC_Current]),sizeof(MSCS_MODE_DATA_08) );
		/* Removablie Block Access Capabilities Page */
		memcpy( (Uchar *)&(DI->MSCS_MODEdata1B[PC_Default]),(Uchar *)&(DI->MSCS_MODEdata1B[PC_Current]),sizeof(MSCS_MODE_DATA_1B) );
		/* Timer & Protect Page */
		memcpy( (Uchar *)&(DI->MSCS_MODEdata1C[PC_Default]),(Uchar *)&(DI->MSCS_MODEdata1C[PC_Current]),sizeof(MSCS_MODE_DATA_1C) );

		/************************/
		/* Set Changeable Value */
		/************************/
		/* Read-Write Error Recovery Page */
		data01 = &(DI->MSCS_MODEdata01[PC_Changeable]);
		memset( (Uchar *)data01 , 0 , sizeof(MSCS_MODE_DATA_01) );
		/* Flexible Disk Page */
		data05 = &(DI->MSCS_MODEdata05[PC_Changeable]);
		memset( (Uchar *)data05 , 0 , sizeof(MSCS_MODE_DATA_05) );
		/* Caching Page */
		data08 = &(DI->MSCS_MODEdata08[PC_Changeable]);
		memset( (Uchar *)data08 , 0 , sizeof(MSCS_MODE_DATA_08) );
		/* Removablie Block Access Capabilities Page */
		data1B = &(DI->MSCS_MODEdata1B[PC_Changeable]);
		memset( (Uchar *)data1B , 0 , sizeof(MSCS_MODE_DATA_1B) );
		/* Timer & Protect Page */
		data1C = &(DI->MSCS_MODEdata1C[PC_Changeable]);
		memset( (Uchar *)data1C , 0 , sizeof(MSCS_MODE_DATA_1C) );
	
		/*******************/
		/* Set Saved Value */
		/*******************/
		/* Read-Write Error Recovery Page */
		data01 = &(DI->MSCS_MODEdata01[PC_Saved]);
		memset( (Uchar *)data01 , 0 , sizeof(MSCS_MODE_DATA_01) );
		/* Flexible Disk Page */
		data05 = &(DI->MSCS_MODEdata05[PC_Saved]);
		memset( (Uchar *)data05 , 0 , sizeof(MSCS_MODE_DATA_05) );
		/* Caching Page */
		data08 = &(DI->MSCS_MODEdata08[PC_Saved]);
		memset( (Uchar *)data08 , 0 , sizeof(MSCS_MODE_DATA_08) );
		/* Removablie Block Access Capabilities Page */
		data1B = &(DI->MSCS_MODEdata1B[PC_Saved]);
		memset( (Uchar *)data1B , 0 , sizeof(MSCS_MODE_DATA_1B) );
		/* Timer & Protect Page */
		data1C = &(DI->MSCS_MODEdata1C[PC_Saved]);
		memset( (Uchar *)data1C , 0 , sizeof(MSCS_MODE_DATA_1C) );
#endif

	return;
}


/********************************/
/* Sub-Class Command Function	*/
/********************************/
int MSCS_Cmd_Inquiry(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *DI, Ulong *residue)
{
	//ER	ret=0;
	unsigned char *buff = NULL;
	// 2006.11.14 Modify for USBCV 1.3 MSC Test
	Ulong size, dat_size;
//	unsigned short sr;
	
	if( *residue < CmdBlk->CmdDat[4] )
		size = *residue;
	else
		size = CmdBlk->CmdDat[4];
	if( size > 0 )
	{
//		sr = Dd_ARM_DI();
		/* Vendor Specifc Data Get */
		/* ret= */ MSC_get_blf( (MSC_VP *)&buff	,(MSC_ID)W_MPFID_EXTBUF_HIGH);
		if (buff == NULL)
		{
			MSCP_EPStalled(BULK_WRITE_FIFO);  /* Bulk-In EndPoint is Stalled */
			MSCP_EPStalled(BULK_READ_FIFO);  /* Bulk-Out EndPoint is Stalled */
			return(bCSW_PhaseError);
		}
		dat_size = MSCA_SetInquiryVendData(DI->MSCS_DriveNo, (buff+sizeof(MSCS_INQUIRY_DATA)));
		
		/* Send Response Data */
		if( dat_size == 0 )
		{	// No vendor data
			if( size > sizeof(MSCS_INQUIRY_DATA) )
				size = sizeof(MSCS_INQUIRY_DATA);
			if( MSCP_SndData( (Uchar *)&(DI->MSCS_INQUIRYdata) , size ) != size )
			{
//				Dd_ARM_EI(sr);
				return( bCSW_Failed );
			}
		}
		else
		{	// Exist vendor data
			memcpy(buff, (Uchar *)&(DI->MSCS_INQUIRYdata), sizeof(MSCS_INQUIRY_DATA));
			dat_size += sizeof(MSCS_INQUIRY_DATA);
			buff[4] = (unsigned char)dat_size - 5;
			if( size > dat_size )
				size = dat_size;
			if( MSCP_SndData( buff , size ) != size )
			{
//				Dd_ARM_EI(sr);
				return( bCSW_Failed );
			}
		}
		// release buffer
		MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH , buff );
//		Dd_ARM_EI(sr);
	}
	*residue -= size;	// 2006.11.09 add for USBCV 1.3 MSC Test
	if( *residue > 0 )
	{
		while( MSCP_CheckEPEmpty( BULK_WRITE_FIFO ) == MSC_ERROR )
		{	// FIFO Not Empty
			MSC_tslp_tsk(10);
		}
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
	}
	gMedia_become_ready[DI->MSCS_DriveNo] = 0;
	return( bCSW_Passed );
}


#if 0
int MSCS_Cmd_ModeSelect(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *DI, Ulong *residue)
{
	/* Page saved is not supported */
	_setSenseKey( DI , 0x05 , 0x24 , 0x00 );
	return( bCSW_Failed );
}
#endif

int MSCS_Cmd_ModeSense_0x1A(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *DI, Ulong *residue)	/* ModeSense(06) */
{
	// 2006.11.14 Modify for USBCV 1.3 MSC Test
	unsigned char *buf = NULL;
	unsigned char *Dat = NULL;
	Ushort	size, size1;
	MSCS_MODE_DATA_HEADER_0x1A	*Header;
#if 0
	MSCS_MODE_BLKDESC			*Blkdesc;
#endif
	Uchar	PC;		/* Page control */
#if 0
	Uchar	DBD;	/* Disable block descriptor */	/* add 2002.07.17 */
#endif

	/* Media changed? */
	if( DI->MSCS_MEDIUM_CHANGED != 0 ) {
		/* Clear Medium change flag */// add.2007.05.25
		DI->MSCS_MEDIUM_CHANGED = 0;
		/* Endpoint Stalled */
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		/* Set Sensekey "Not Ready to Ready transition - MEDIA CHANGED */
		_setSenseKey( DI , 0x06 , 0x28 , 0x00 );
		return(bCSW_Failed);
	}

	/* Check Page Control Field */
	PC = (CmdBlk->CmdDat[2] >> 6 ) & 0x03 ;
	/* Change!! PC is current only */
	if ( PC != PC_Current ) {
		/* Error */
		/* Endpoint Stalled */
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		_setSenseKey( DI , 0x05 , 0x24 , 0x00 ); /* INVALID FIELD IN COMMAND PACKET */
		return(bCSW_Failed);
	}

	/* get allocation length */
	size = (CmdBlk->CmdDat[4]); /*  Response data length depends on allocation length of Command parameter */

	/* Get memory for read data */   
	MSC_get_blf( (MSC_VP *)&(buf) , W_MPFID_EXTBUF_HIGH );

	if (buf == NULL)
	{
		MSCP_EPStalled(BULK_WRITE_FIFO);  /* Bulk-In EndPoint is Stalled */
		MSCP_EPStalled(BULK_READ_FIFO);  /* Bulk-Out EndPoint is Stalled */
		return (bCSW_PhaseError);
	}
	
	/* Clear Buffer data */
	memset( buf , 0 , size );

	/* Set Mode Parameter Header */
	Header = (MSCS_MODE_DATA_HEADER_0x1A *)buf;
	Header->Length = 0 ;
	Header->MediumTypeCode = MSCA_GetMediaType(DI->MSCS_DriveNo); // Modify 2008.07.09

	/* Write Protected ? */ // Modify 2008.07.09
	if( (MSCA_CheckMedia[DI->MSCS_DriveNo]() == MSC_OK)&&(MSCA_CheckWriteProtect[DI->MSCS_DriveNo]() != MSC_OK) ) {
		// Media Exist & Write Protected
		Header->byte2.WP = 1 ;
	}
#if 0
	if( MSCA_CheckWriteProtect[DI->MSCS_DriveNo]() != MSC_OK ) {	/* Bugfix 2002/03/18 */
		Header->byte2.WP = 1 ;
	}
#endif
	
	Dat = (Uchar *)(buf + sizeof(MSCS_MODE_DATA_HEADER_0x1A) );

#if 0	
	/* Check Disable block descriptor */
	DBD = (CmdBlk->CmdDat[1]>>3) & 0x01 ;
	if(DBD == 0) {
		/* return block descriptor */
		Blkdesc = (MSCS_MODE_BLKDESC *)Dat;	/* set pointer address */
		Blkdesc->NumOfBlks 	= (Ulong)(DI->MSCA_IDIdata.lba_sector);
		Blkdesc->BlockLength= (Ulong)(DI->MSCA_IDIdata.bytepersector);
		Dat += sizeof(MSCS_MODE_BLKDESC);	/* increment address */
		
		Header->BLKDESCLENGTH = 8;
	} else {
		/* Not return block descriptor */
		Header->BLKDESCLENGTH = 0;
	}
#endif
	/* Need not block descriptor */
	Header->BLKDESCLENGTH = 0;
	
	/* Make page data */
	switch( CmdBlk->CmdDat[2] & 0x3F ) {	/* Page Code */
	case 0x00:
	case 0x01:	/* Read-Write Error Recovery Page	*/
		Header->Length += sizeof(MSCS_MODE_DATA_01) ;
		memcpy( (Uchar *)Dat , (Uchar *)&(DI->MSCS_MODEdata01[PC]) , sizeof(MSCS_MODE_DATA_01) );
		break;
	case 0x05 :  /* Flexible Discket Page(Duumy Response) */
	case 0x1C :  /* Dummy Response(NO Data) */
		break;
	case 0x20 :	/* Vendor specific for Memory Stick */
		Header->Length += MSCA_SetModeData20(DI->MSCS_DriveNo, Dat);
		break;
	case 0x3F:	/* All pages						*/
		Header->Length += sizeof(MSCS_MODE_DATA_01) ;
		memcpy( (Uchar *)Dat , (Uchar *)&(DI->MSCS_MODEdata01[PC]) , sizeof(MSCS_MODE_DATA_01) );
		Dat += sizeof(MSCS_MODE_DATA_01);
		Header->Length += MSCA_SetModeData20(DI->MSCS_DriveNo, Dat);
		break;
		
	default:
		/* Endpoint Stalled */
		MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH, buf );	/* add 2002.07.15 */
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		_setSenseKey(DI,0x05,0x24,0x00);	/* INVALID FIELD IN COMMAND PACKET */
		return(bCSW_Failed);
	}

	Header->Length +=( Header->BLKDESCLENGTH + sizeof(MSCS_MODE_DATA_HEADER_0x1A)-1 ) ;
	size1 = Header->Length + 1;
	if( size > size1 )
	{
		size = size1;
	}
	if( *residue < size )
		size = *residue;
	if( size > 0 )
	{
		/* Send Response Data */
		if( MSCP_SndData( buf , size ) != size )return(bCSW_Failed);
	}
	MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH, buf );
	*residue -= size;	// 2006.11.09 add for USBCV 1.3 MSC Test
	if( *residue > 0 )
	{
		while( MSCP_CheckEPEmpty( BULK_WRITE_FIFO ) == MSC_ERROR )
		{	// FIFO Not Empty
			MSC_tslp_tsk(10);
		}
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
	}
	return( bCSW_Passed );
}

int MSCS_Cmd_ModeSense_0x5A(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *DI, Ulong *residue)		/* ModeSense(10) */
{
	unsigned char *buf = NULL;
	unsigned char *Dat = NULL;
	Ulong	size, size1;
	MSCS_MODE_DATA_HEADER_0x5A	*Header;
#if 0
	MSCS_MODE_BLKDESC			*Blkdesc;
#endif
	Uchar	PC;		/* Page control */
#if 0
	Uchar	DBD;	/* Disable block descriptor */	/* add 2002.07.17 */
#endif

//#ifndef MSC_BIG_ENDIAN
///* Little Endian for M-7 */
//    Ushort tmp;
//#endif /* MSC_BIG_ENDIAN */

	/* Media changed? */
	if( DI->MSCS_MEDIUM_CHANGED != 0 ) {
		/* Clear Medium change flag */// add.2007.05.25
		DI->MSCS_MEDIUM_CHANGED = 0;
		/* Endpoint Stalled */
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		/* Set Sensekey "Not Ready to Ready transition - MEDIA CHANGED */
		_setSenseKey( DI , 0x06 , 0x28 , 0x00 );
		return(bCSW_Failed);
	}

	/* Check Page Control Field */
	PC = (CmdBlk->CmdDat[2] >> 6 ) & 0x03 ;
	/* Change!! PC is current only */
	if ( PC != PC_Current ) {
		/* Error */
		/* Endpoint Stalled */
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		_setSenseKey( DI , 0x05 , 0x24 , 0x00 ); /* INVALID FIELD IN COMMAND PACKET */
		return(bCSW_Failed);
	}

	/* get allocation length */
	/* Response data length depends on allocation length of Command parameter */
	size = ((CmdBlk->CmdDat[7])*256 + (CmdBlk->CmdDat[8])); 
	
	/* Get memory for read data */   
	MSC_get_blf( (MSC_VP *)&(buf) , W_MPFID_EXTBUF_HIGH );

	if (buf == NULL)
	{
		MSCP_EPStalled(BULK_WRITE_FIFO);  /* Bulk-In EndPoint is Stalled */
		MSCP_EPStalled(BULK_READ_FIFO);  /* Bulk-Out EndPoint is Stalled */
		return (bCSW_PhaseError);
	}
	
	/* Clear Buffer data */
	memset( buf , 0 , size );

	/* Set Mode Parameter Header */
	Header = (MSCS_MODE_DATA_HEADER_0x5A *)buf;
	Header->Length = 0 ;
	Header->MediumTypeCode = MSCA_GetMediaType(DI->MSCS_DriveNo);	// Modify 2008.07.09

	/* Write Protected ? */ // Modify 2008.07.09
	if( (MSCA_CheckMedia[DI->MSCS_DriveNo]() == MSC_OK)&&(MSCA_CheckWriteProtect[DI->MSCS_DriveNo]() != MSC_OK) ) {
		// Media Exist & Write Protected
		Header->byte3.WP = 1 ;
	}
#if 0
	if( MSCA_CheckWriteProtect[DI->MSCS_DriveNo]() != MSC_OK ) {	/* Bugfix 2002/03/18 */
		Header->byte3.WP = 1 ;
	}
#endif
	Dat = (Uchar *)(buf + sizeof(MSCS_MODE_DATA_HEADER_0x5A) );

#if 0	
	/* Check Disable block descriptor */
	DBD = (CmdBlk->CmdDat[1]>>3) & 0x01 ;
	if(DBD == 0) {
		/* return block descriptor */
		Blkdesc = (MSCS_MODE_BLKDESC *)Dat;	/* set pointer address */
		Blkdesc->NumOfBlks 	= (Ulong)(DI->MSCA_IDIdata.lba_sector);
		Blkdesc->BlockLength= (Ulong)(DI->MSCA_IDIdata.bytepersector);
		Dat += sizeof(MSCS_MODE_BLKDESC);	/* increment address */
		
		Header->BLKDESCLENGTH = 8;
	} else {
		/* Not return block descriptor */
		Header->BLKDESCLENGTH = 0;
	}
#endif
	/* Need not block descriptor */
	Header->BLKDESCLENGTH = 0;
	
	/* Make page data */
	switch( CmdBlk->CmdDat[2] & 0x3F ) {
	case 0x00:
	case 0x01:	/* Read-Write Error Recovery Page	*/
		Header->Length += sizeof(MSCS_MODE_DATA_01) ;
		memcpy( (Uchar *)Dat , (Uchar *)&(DI->MSCS_MODEdata01[PC]) , sizeof(MSCS_MODE_DATA_01) );
		break;
	case 0x05 :  /* Flexible Discket Page(Duumy Response) */
	case 0x1C :  /* Dummy Response(NO Data) */
		break;
	case 0x20 :	 /* Vendor specific for Memory Stick */
		Header->Length += MSCA_SetModeData20(DI->MSCS_DriveNo, Dat);
		break;
	case 0x3F:	/* All pages						*/
		Header->Length += sizeof(MSCS_MODE_DATA_01) ;
		memcpy( (Uchar *)Dat , (Uchar *)&(DI->MSCS_MODEdata01[PC]) , sizeof(MSCS_MODE_DATA_01) );
		Dat += sizeof(MSCS_MODE_DATA_01);
		Header->Length += MSCA_SetModeData20(DI->MSCS_DriveNo, Dat);
		break;
	default:
		/* Endpoint Stalled */
		MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH, buf );
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		_setSenseKey(DI,0x05,0x24,0x00);	/* INVALID FIELD IN COMMAND PACKET */
		return(bCSW_Failed);
	}
	
	Header->Length +=( Header->BLKDESCLENGTH + sizeof(MSCS_MODE_DATA_HEADER_0x5A)-2 ) ;
	size1 = Header->Length + 2;
	if( size > size1 )
	{
		size = size1;
	}
	if( *residue < size )
		size = *residue;
	if( size > 0 )
	{
//#ifndef MSC_BIG_ENDIAN
///* Little Endian for M-7 */
//    tmp = Header->Length;
//    Header->Length =  (tmp & 0x00FFu) << 8;
//    Header->Length |= (tmp & 0xFF00u) >> 8;
//    
//    tmp = Header->BLKDESCLENGTH;
//    Header->BLKDESCLENGTH =  (tmp & 0x00FFu) << 8;
//    Header->BLKDESCLENGTH |= (tmp & 0xFF00u) >> 8;
//#endif /* MSC_BIG_ENDIAN */
	
		/* Send Response Data */
		if( MSCP_SndData( buf , size ) != size )return(bCSW_Failed);
	}
	MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH, buf );
	*residue -= size;	// 2006.11.09 add for USBCV 1.3 MSC Test
	if( *residue > 0 )
	{
		while( MSCP_CheckEPEmpty( BULK_WRITE_FIFO ) == MSC_ERROR )
		{	// FIFO Not Empty
			MSC_tslp_tsk(10);
		}
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
	}
	return( bCSW_Passed );
}


int MSCS_Cmd_PreventAllowMediumRemoval(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *DI, Ulong *residue)
{
#if 1	//k.maeda 2010/11/16 for MAC OS
	//_setSenseKey( DI , 0x05 , 0x24 , 0x00 ); /* INVALID FIELD IN COMMAND PACKET */
	//return( bCSW_Failed );
	//J.Tomida 2011/08/17 for WindowsXP
	return( bCSW_Passed );
#else
	
	/* Media changed? */
	if( DI->MSCS_MEDIUM_CHANGED != 0 ) {
		/* Clear Medium change flag */// add.2007.05.25
		DI->MSCS_MEDIUM_CHANGED = 0;
		/* Endpoint Stalled */
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		/* Set Sensekey "Not Ready to Ready transition - MEDIA CHANGED */
		_setSenseKey( DI , 0x06 , 0x28 , 0x00 );
		return(bCSW_Failed);
	}

	/* Set global variable of prevention of medium removal */
	if( (CmdBlk->CmdDat[4] & 0x01) == 0x01 ) {
		DI->MSCS_MEDIUM_REMOVAL = 1; /* Prevent bit = 1 : a Host issue prevention of medium removal */
		if (MSCA_LockMedia[DI->MSCS_DriveNo]() == MSC_OK) {
			return( bCSW_Passed );
		} else {
			MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
			_setSenseKey( DI , 0x05 , 0x24 , 0x00 ); /* INVALID FIELD IN COMMAND PACKET */
			return( bCSW_Failed );
		}
	} else {
		DI->MSCS_MEDIUM_REMOVAL = 0; /* Prevent bit = 0 : a Host allow medium removal */
		if (MSCA_UnlockMedia[DI->MSCS_DriveNo]() == MSC_OK) {
			return( bCSW_Passed );
		} else {
			MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
			_setSenseKey( DI , 0x05 , 0x24 , 0x00 ); /* INVALID FIELD IN COMMAND PACKET */
			return( bCSW_Failed );
		}
	}
#endif
}


#ifndef MSC_SERIAL_READ
static int is_subtask_active(MSC_ID tid)
{
	T_MSC_RTSK rtsk;
	int ret = MSC_ERROR;
	if(MSC_ref_tsk(&rtsk, tid) == MSC_OK )
	{
		if( rtsk.tskstat != MSC_TTS_DMT )
		{	// Task is active
			ret = MSC_OK;
		}
	}
	return ret;
}
#endif


int MSCS_Cmd_Read(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *DI, Ulong *residue)
{
	Ulong	LBA;
	Ulong	Length;
	Uchar	*buf;
#ifndef MSC_SERIAL_READ
	MSCP_USB_MSG	*Request_msg=0,*Response_msg=0;
	MSC_ER	ret=0;
#endif
	Ulong	size;
	//UINT	currentILM;
#ifdef MSC_SERIAL_READ
#ifndef DMA_TCHECK
	long retsize = 0;
#endif
#endif
	
	// 200611.14 Modify for USBCV 1.3 MSC Test
	/* Media changed? */
	if( DI->MSCS_MEDIUM_CHANGED != 0 ) {
		/* Clear Medium change flag */// add.2007.05.25
		DI->MSCS_MEDIUM_CHANGED = 0;
		/* Endpoint Stalled */
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		/* Set Sensekey "Not Ready to Ready transition - MEDIA CHANGED */
		_setSenseKey( DI , 0x06 , 0x28 , 0x00 );
		return(bCSW_Failed);
	}

	if( ((DI->MSCS_MEDIUM_REMOVAL == 0) && (DI->MSCS_MEDIUM_EJECT == 1))  	/* Medium allow removal & eject */
		|| (MSCA_CheckMedia[DI->MSCS_DriveNo]() != MSC_OK) 
	) {
		/* Endpoint Stalled */
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		/* Error */
		_setSenseKey( DI , 0x02 , 0x3A , 0x00 ); /* MEDIUM NOT PRESENT */
		return( bCSW_Failed );
	}

	LBA  = (Ulong)(CmdBlk->CmdDat[2])*0x1000000 + (Ulong)(CmdBlk->CmdDat[3])*0x10000 + (Ulong)(CmdBlk->CmdDat[4])*0x100 + (Ulong)(CmdBlk->CmdDat[5]);
	Length = (Ushort)(CmdBlk->CmdDat[7])*256 + (Ushort)(CmdBlk->CmdDat[8]);

	size = Length * (DI->MSCA_IDIdata.bytepersector);
	if( *residue != size  )
	{	// Bad Size
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		/* Error */
		_setSenseKey( DI , 0x05 , 0x24 , 0x00 ); /* INVALID FIELD IN CDB */
		return( bCSW_Failed );
	}

	/* Validity check of LBA & Length */
	if( (LBA+Length) > (DI->MSCA_IDIdata.lba_sector) ) { /* change 2002.07.15 */
		/* Error */
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */ /* add 2002.07.15 */
		_setSenseKey( DI , 0x05 , 0x21 , 0x00 ); /* LOGICAL BLOCK ADDRESS OUT OF RANGE */
		return(bCSW_Failed);
	}

	if( Length > 0 ) {
#ifndef MSC_SERIAL_READ
/******************/
/* parallel Read  */
/******************/
		
		while( Length > 0 ) {
			/* Get Memory buffer */
			ret=MSC_get_blf( (MSC_VP *)&pbuf_snd_release	,(MSC_ID)W_MPFID_EXTBUF_HIGH);
/* >> add 2008.10.30 */
			if( ret == E_NOMEM_USB )
			{	// メモリ不足
				
				while( is_subtask_active(W_TID_MSCP_SND) == MSC_OK )
				{	// subtask active
					ret = MSC_get_blf( (MSC_VP *)&pbuf_snd_release	,(MSC_ID)W_MPFID_EXTBUF_HIGH);
					if( ret != E_NOMEM_USB )
						break;
					MSC_tslp_tsk(1);
				}
			}
			if( ret != MSC_OK )
			{	// System Error or Term
				pbuf_snd_release = NULL;
				MSCP_SendStop();
				MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */ /* add 2002.07.15 */
				_setSenseKey( DI , 0x03 , 0x11 , 0x00 ); /* UNRECOVERD READ ERROR */
				return(bCSW_Failed);
			}
/* << add 2008.10.30 */
			
			buf = pbuf_snd_release;
			/* Read data from Media by LBA Access */
			if( Length > (W_MPFID_EXTBUF_HIGH_SIZE>>9) ) {
#ifdef DMA_TCHECK
				static OS_USER_SYSTIM st0, ed0;

				OS_User_Get_Tim(&st0);
#endif
				/* The size of Requested Read Sector is MORE than Buffer size */
				if( MSCA_ReadLBA[DI->MSCS_DriveNo]( LBA , (W_MPFID_EXTBUF_HIGH_SIZE>>9) , buf ) != MSC_OK ) {
					/* Error */
					MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH , pbuf_snd_release );
					pbuf_snd_release = NULL;
					/* >> add 2008.07.30 for parallel read */
					MSCP_SendStop();
					/* >> add 2009.12.23 bug fix */
					ret=MSC_get_blf( (MSC_VP *)&Request_msg ,(MSC_ID)W_MPFID_OTHERMSG );
					__DI();
					Request_msg->size = -1;	/* -1 means to Send Stop */
					Request_msg->buf = NULL;
					__EI();
					MSC_snd_msg( W_MID_MSCP_SND_REQUEST , (T_MSC_MSG *)Request_msg );
					/* << 2009.12.22 */
					/* wait data send stop */
					MSC_rcv_msg( (T_MSC_MSG **)&Response_msg , W_MID_MSCP_SND_RESPONSE ) ;
					*residue -= Response_msg->size;
					/* Release buffer */
					MSC_rel_blf( (MSC_ID)W_MPFID_OTHERMSG , Response_msg );
					/*  << */
					MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */ /* add 2002.07.15 */
					_setSenseKey( DI , 0x03 , 0x11 , 0x00 ); /* UNRECOVERD READ ERROR */
					return(bCSW_Failed);
				}
#ifdef DMA_TCHECK
				OS_User_Get_Tim(&ed0);
#endif
				Length -= (W_MPFID_EXTBUF_HIGH_SIZE>>9);
				LBA += (W_MPFID_EXTBUF_HIGH_SIZE>>9) ;
				size = (W_MPFID_EXTBUF_HIGH_SIZE) ;
			} else {
				/* The size of Requested Read Sector is LESS than Buffer size */
				if( MSCA_ReadLBA[DI->MSCS_DriveNo]( LBA , Length , buf ) != MSC_OK ) {
					/* Error */
					MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH , pbuf_snd_release );
					pbuf_snd_release = NULL;
					/* >> add 2008.07.30 for parallel read */
					MSCP_SendStop();
					/* >> add 2009.12.23 bug fix */
					ret=MSC_get_blf( (MSC_VP *)&Request_msg ,(MSC_ID)W_MPFID_OTHERMSG );
					__DI();
					Request_msg->size = -1;	/* -1 means to Send Stop */
					Request_msg->buf = NULL;
					__EI();
					MSC_snd_msg( W_MID_MSCP_SND_REQUEST , (T_MSC_MSG *)Request_msg );
					/* << 2009.12.22 */
					/* wait data send stop */
					MSC_rcv_msg( (T_MSC_MSG **)&Response_msg , W_MID_MSCP_SND_RESPONSE ) ;
					*residue -= Response_msg->size;
					/* Release buffer */
					MSC_rel_blf( (MSC_ID)W_MPFID_OTHERMSG , Response_msg );
					/*  << */
					MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */ /* add 2002.07.15 */
					_setSenseKey( DI , 0x03 , 0x11 , 0x00 ); /* UNRECOVERD READ ERROR */
					return(bCSW_Failed);
				}
				size = Length * (DI->MSCA_IDIdata.bytepersector) ;
				Length = 0 ;
			}
			/* Send Read Data to HostPC via USB */
			ret=MSC_get_blf( (MSC_VP *)&Request_msg ,(MSC_ID)W_MPFID_OTHERMSG );
/* >> Critical point */
			__DI();
			Request_msg->size = size ;
			Request_msg->buf = buf;
			__EI();
/* << Critical point */
			MSC_snd_msg( W_MID_MSCP_SND_REQUEST , (T_MSC_MSG *)Request_msg );
			pbuf_snd_release = NULL;
		}
		/* Send Check message to MSCP_Snd_Request task */
		ret=MSC_get_blf( (MSC_VP *)&Request_msg ,(MSC_ID)W_MPFID_OTHERMSG );
/* >> Critical point */
		__DI();
		Request_msg->size = 0;	/* 0 means to do check */
		Request_msg->buf = NULL;
		__EI();
		MSC_snd_msg( W_MID_MSCP_SND_REQUEST , (T_MSC_MSG *)Request_msg );
/* << Critical point */
		/* Receive check message from MSCP_Snd_Request task */
		MSC_rcv_msg( (T_MSC_MSG **)&Response_msg , W_MID_MSCP_SND_RESPONSE ) ;
		*residue -= Response_msg->size;
		/* Release buffer */
		MSC_rel_blf( (MSC_ID)W_MPFID_OTHERMSG , Response_msg );
		/* ここで転送エラー対策を行う */

#else /* MSC_SERIAL_READ */
/***************/
/* SERIAL READ */
/***************/
		/* Get Memory buffer */
		MSC_get_blf( (MSC_VP *)&pbuf_snd_release	,(MSC_ID)W_MPFID_EXTBUF_HIGH);

		while( Length > 0 ) {
#ifdef DMA_TCHECK
			static OS_USER_SYSTIM st2, ed2, st3, ed3;
			int kk;
#endif
			buf = pbuf_snd_release;
			/* Read data from Media by LBA Access */
			if( Length > (W_MPFID_EXTBUF_HIGH_SIZE>>9) ) {
				/* The size of Requested Read Sector is MORE than Buffer size */
#ifdef DMA_TCHECK

				OS_User_Get_Tim(&st2);
#endif
				if( MSCA_ReadLBA[DI->MSCS_DriveNo]( LBA , (W_MPFID_EXTBUF_HIGH_SIZE>>9) , buf ) != MSC_OK ) {
					/* Error */
					MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH , pbuf_snd_release );
					pbuf_snd_release = NULL;
					MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */ /* add 2002.07.15 */
					_setSenseKey( DI , 0x03 , 0x11 , 0x00 ); /* UNRECOVERD READ ERROR */
					return(bCSW_Failed);
				}
#ifdef DMA_TCHECK
				OS_User_Get_Tim(&ed2);
#endif
				Length -= (W_MPFID_EXTBUF_HIGH_SIZE>>9);	/* sector */
				LBA += (W_MPFID_EXTBUF_HIGH_SIZE>>9) ;		/* sector */
				size = (W_MPFID_EXTBUF_HIGH_SIZE) ;			/* byte */
			} else {
				/* The size of Requested Read Sector is LESS than Buffer size */
				if( MSCA_ReadLBA[DI->MSCS_DriveNo]( LBA , Length , buf ) != MSC_OK ) {
					/* Error */
					MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH , pbuf_snd_release );
					pbuf_snd_release = NULL;
					MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */ /* add 2002.07.15 */
					_setSenseKey( DI , 0x03 , 0x11 , 0x00 ); /* UNRECOVERD READ ERROR */
					return(bCSW_Failed);
				}
				size = Length * (DI->MSCA_IDIdata.bytepersector) ;	/* byte */
				Length = 0 ;
			}
#ifdef DMA_TCHECK
			if( size >= 0x4000 ) {
				OS_User_Get_Tim(&st3);
				MWUSB_Write( buf, size , BULK_WRITE_FIFO);
				OS_User_Get_Tim(&ed3);
				kk=1;
			}
			else {
				MWUSB_Write( buf, size , BULK_WRITE_FIFO);
			}
#else
			/* USB data Send */
			retsize = MWUSB_Write( buf, size , BULK_WRITE_FIFO);
			if( retsize <= 0 )
			{	// USB Write Error
				MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH , pbuf_snd_release );
				pbuf_snd_release = NULL;
				_setSenseKey( DI , 0x03 , 0x11 , 0x00 ); /* UNRECOVERD READ ERROR */
				return(bCSW_Failed);
			}
			*residue -= retsize;	// 2006.11.09 add for USBCV 1.3 MSC Test
#endif
		}
		
		/* Relase memory buffer */
		MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH	, pbuf_snd_release);
		pbuf_snd_release = NULL;

#endif /* MSC_SERIAL_READ */
	}

	return( bCSW_Passed );
}


int MSCS_Cmd_ReadCapacity(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *DI, Ulong *residue)
{
	// 2006.11.14 Modify for USBCV 1.3 MSC Test
	MSCS_READ_CAPACITIES_DATA	buf;
	Ulong size, send_length;
#ifndef MSC_BIG_ENDIAN
/* Little Endian for M-7 */
    Ulong tmp;
#endif /* MSC_BIG_ENDIAN */
	/* Media changed? */
	if( DI->MSCS_MEDIUM_CHANGED != 0 ) {
		/* Clear Medium change flag */// add.2007.05.25
		DI->MSCS_MEDIUM_CHANGED = 0;
		/* Endpoint Stalled */
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		/* Set Sensekey "Not Ready to Ready transition - MEDIA CHANGED */
		_setSenseKey( DI , 0x06 , 0x28 , 0x00 );
		return(bCSW_Failed);
	}

	/* Medium not present? */ /* Medium already eject ? */
	if( ((DI->MSCS_MEDIUM_REMOVAL == 0) && (DI->MSCS_MEDIUM_EJECT == 1)) || 	/* Medium allow removal & eject */
		 (MSCA_CheckMedia[DI->MSCS_DriveNo]() != MSC_OK) ) {
		/* Endpoint Stalled */
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		/* Error */
		_setSenseKey( DI , 0x02 , 0x3A , 0x00 ); /* MEDIUM NOT PRESENT */
		return( bCSW_Failed );
	}
#ifdef MSC_BIG_ENDIAN
#ifndef MSCS_RAM_ReadRetryCount	// Chsnge 2009.03.05	
	buf.NumOfBlks = MSCA_Capacity[DI->MSCS_DriveNo]() - 1;	// Change 2009.03.05
#else	
	buf.NumOfBlks = (DI->MSCA_IDIdata.lba_sector) - 1 ;	/* Bugfix 2002.01.15 */
#endif
	buf.BlockLength = DI->MSCA_IDIdata.bytepersector;

#else /* MSC_BIG_ENDIAN */
/* Little Endian for M-7 */
#ifndef MSCS_RAM_ReadRetryCount	// Chsnge 2009.03.05	
	tmp = MSCA_Capacity[DI->MSCS_DriveNo]() - 1;	// Change 2009.03.05
#else	
	tmp = (DI->MSCA_IDIdata.lba_sector) - 1 ;	/* Bugfix 2002.01.15 */
#endif
    buf.NumOfBlks = (tmp & 0x000000FFu) << 24;
    buf.NumOfBlks |= (tmp & 0x0000FF00u) << 8;
    buf.NumOfBlks |= (tmp & 0x00FF0000u) >> 8;
    buf.NumOfBlks |= (tmp & 0xFF000000u) >> 24;
    
	tmp = DI->MSCA_IDIdata.bytepersector;
    buf.BlockLength = (tmp & 0x000000FFu) << 24;
    buf.BlockLength |= (tmp & 0x0000FF00u) << 8;
    buf.BlockLength |= (tmp & 0x00FF0000u) >> 8;
    buf.BlockLength |= (tmp & 0xFF000000u) >> 24;
#endif /* MSC_BIG_ENDIAN */

	if( *residue < sizeof(MSCS_READ_CAPACITIES_DATA) )
		size = *residue;
	else
		size = sizeof(MSCS_READ_CAPACITIES_DATA);
	if( size > 0 )
	{
		/* Send Response Data */
		send_length = sizeof(MSCS_READ_CAPACITIES_DATA);
		//MSCP_SndData( (unsigned char *)&buf , sizeof(MSCS_READ_CAPACITIES_DATA) );
		if( MSCP_SndData( (unsigned char *)&buf , send_length ) != send_length )return(bCSW_Failed);
	}
	*residue -= size;
	if( *residue > 0 )
	{
		while( MSCP_CheckEPEmpty( BULK_WRITE_FIFO ) == MSC_ERROR )
		{	// FIFO Not Empty
			MSC_tslp_tsk(10);
		}
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
	}
	return( bCSW_Passed );
}

#if 0
int MSCS_Cmd_ReadFormatCapacity(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *DI, Ulong *residue)
{
	MSCS_READ_FORMAT_CAPACITIES_DATA	buf;
	Ushort size;
	
	/* Medium not present? */ /* Medium already eject ? */
	if( ((DI->MSCS_MEDIUM_REMOVAL == 0) && (DI->MSCS_MEDIUM_EJECT == 1)) || 	/* Medium allow removal & eject */
		 (MSCA_CheckMedia[DI->MSCS_DriveNo]() != MSC_OK) ) {
		/* Endpoint Stalled */
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		/* Error */
		_setSenseKey( DI , 0x02 , 0x3A , 0x00 ); /* MEDIUM NOT PRESENT */
		return( bCSW_Failed );
	}

	/* Set Data */
	buf.ListLength = 16;
	buf.Max.NumOfBlks = (DI->MSCA_IDIdata.lba_sector);				/* bugfix 2002.04.08 */
	buf.Max.BlockLength = DI->MSCA_IDIdata.bytepersector;
	buf.Formattable.NumOfBlks = (DI->MSCA_IDIdata.lba_sector);		/* bugfix 2002.04.08 */
	buf.Formattable.BlockLength = DI->MSCA_IDIdata.bytepersector;

	size = (Ushort)((CmdBlk->CmdDat[7]) << 8 ) + (Ushort)(CmdBlk->CmdDat[8] );
	if( size > sizeof(MSCS_READ_FORMAT_CAPACITIES_DATA) ) 
		size = sizeof(MSCS_READ_FORMAT_CAPACITIES_DATA);

	/* Send Response Data */
	if( MSCP_SndData( (unsigned char *)&buf , size ) != size )return( bCSW_Failed );

	*residue -= size;	// 2006.11.09 add for USBCV 1.3 MSC Test
	return( bCSW_Passed );
}
#endif

int MSCS_Cmd_RequestSense(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *DI, Ulong *residue)
{
	Ushort size;

	size = (Ushort)(CmdBlk->CmdDat[4]);
	if( size > sizeof(MSCS_REQUEST_SENSE) ) 
		size = sizeof(MSCS_REQUEST_SENSE);
	if( *residue < size )
		size = *residue;
	if( size > 0 ) {
		/* Send Response Data */
		if( MSCP_SndData( (Uchar *)&(DI->MSCS_SENSEdata) , size ) != size )return( bCSW_Failed );
	}
	/* clear SENSEData */
	/* Set Sensekey NOSENSE */
	_setSenseKey( DI , 0x00 , 0x00 , 0x00 );
	*residue -= size;	// 2006.11.09 add for USBCV 1.3 MSC Test
	if( *residue > 0 )
	{
		while( MSCP_CheckEPEmpty( BULK_WRITE_FIFO ) == MSC_ERROR )
		{	// FIFO Not Empty
			MSC_tslp_tsk(10);
		}
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
	}
	return( bCSW_Passed );
}

int MSCS_Cmd_StartStop(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *DI, Ulong *residue)
{

	/* Media changed? */
	if( DI->MSCS_MEDIUM_CHANGED != 0 ) {
		/* Clear Medium change flag */// add.2007.05.25
		DI->MSCS_MEDIUM_CHANGED = 0;
		/* Set Sensekey "Not Ready to Ready transition - MEDIA CHANGED */
		_setSenseKey( DI , 0x06 , 0x28 , 0x00 );
		return(bCSW_Failed);
	}

	/* Medium not present? */ /* Medium already eject ? */
	if( ((DI->MSCS_MEDIUM_REMOVAL == 0) && (DI->MSCS_MEDIUM_EJECT == 1)) || 	/* Medium allow removal & eject */
		 (MSCA_CheckMedia[DI->MSCS_DriveNo]() != MSC_OK) ) {
		/* Error */
		_setSenseKey( DI , 0x02 , 0x3A , 0x00 ); /* MEDIUM NOT PRESENT */
		return( bCSW_Failed );
	} else {
		/* Set Sensekey NOSENSE */
		_setSenseKey( DI , 0x00 , 0x00 , 0x00 );
	}

	if( CmdBlk->CmdDat[4] == 0x02 ) { 	 	/* LoEj == 1 && Start == 0 */
		DI->MSCS_MEDIUM_EJECT = 1;
	} else {
		DI->MSCS_MEDIUM_EJECT = 0;
	}

	return( bCSW_Passed );
}


int MSCS_Cmd_TestUnitReady(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *DI, Ulong *residue)
{
	/* Medium not present? */ /* Medium already eject ? */
	// SD media are undetectable unless CheckMedia() is called. 
    // Therefore, the order of the conditions of if sentence was changed. 
	if(  (MSCA_CheckMedia[DI->MSCS_DriveNo]() != MSC_OK) || /* Medium not present? */	
		 ((DI->MSCS_MEDIUM_REMOVAL == 0) && (DI->MSCS_MEDIUM_EJECT == 1)) ) {
		/* Error */
		_setSenseKey( DI , 0x02 , 0x3A , 0x00 ); /* MEDIUM NOT PRESENT */
		return( bCSW_Failed );
	} else {
		if( DI->MSCS_MEDIUM_CHANGED == 0 ) {
			if(gMedia_become_ready[DI->MSCS_DriveNo] == 0)
			{	
				/* Set Sensekey NOSENSE */
				_setSenseKey( DI , 0x00 , 0x00 , 0x00 );
				return( bCSW_Passed );
			}
			else
			{	// 1st test unit ready
				/* Set Sensekey NOSENSE */
				_setSenseKey( DI , 0x02 , 0x04 , 0x01 );
				gMedia_become_ready[DI->MSCS_DriveNo] = 0;
				return( bCSW_Failed );
			}
		} else {
			/* Clear Medium change flag */
			DI->MSCS_MEDIUM_CHANGED = 0;
			/* Set Sensekey "Not Ready to Ready transition - MEDIA CHANGED */
			_setSenseKey( DI , 0x06 , 0x28 , 0x00 );
			return( bCSW_Failed );
		}
	}
}

int MSCS_Cmd_Verify(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *DI, Ulong *residue)
{


	return( bCSW_Passed );
}
	


int MSCS_Cmd_Write(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *DI, Ulong *residue)
{
	// 2006.11.14 Modify for USBCV 1.3 MSC Test
	Ulong LBA;
	Ulong Length;
#ifndef MSC_SERIAL_WRITE
	MSCP_USB_MSG	*Request_msg=0,*Response_msg=0;
#endif
	MSC_ER	ret=0;
	LONG	size;
	Uchar	*buf;
#ifdef USE_UART_MESSAGE
	UINT	currentILM=0;
#endif
	LONG retsize=0;
#ifndef MSC_SERIAL_WRITE
	int ret_wlba=0;
#endif
	unsigned int dummy_size = 0;
	Ulong residue_size;
	
	/* Media changed? */
	if( DI->MSCS_MEDIUM_CHANGED != 0 ) {
		/* Clear Medium change flag */// add.2007.05.25
		DI->MSCS_MEDIUM_CHANGED = 0;
		/* Endpoint Stalled */
		if( *residue > 0 )
			MSCP_EPStalled( BULK_READ_FIFO );	/* Bulk-Out EndPoint is Stalled */
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		/* Set Sensekey "Not Ready to Ready transition - MEDIA CHANGED */
		_setSenseKey( DI , 0x06 , 0x28 , 0x00 );
		return(bCSW_Failed);
	}

	if( ((DI->MSCS_MEDIUM_REMOVAL == 0) && (DI->MSCS_MEDIUM_EJECT == 1))  	/* Medium allow removal & eject */
		|| (MSCA_CheckMedia[DI->MSCS_DriveNo]() != MSC_OK) 
	) {
		/* Endpoint Stalled */
		if( *residue > 0 )
			MSCP_EPStalled( BULK_READ_FIFO );	/* Bulk-Out EndPoint is Stalled */
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		/* Error */
		_setSenseKey( DI , 0x02 , 0x3A , 0x00 ); /* MEDIUM NOT PRESENT */
		#ifdef DEBUG
		RS_printf("write media check error\r\n");
		#endif
		return( bCSW_Failed );
	}

	if( MSCA_CheckWriteProtect[DI->MSCS_DriveNo]() == MSC_ERROR ) {
		/* Endpoint Stalled */
		if( *residue > 0 )
			MSCP_EPStalled( BULK_READ_FIFO );	/* Bulk-Out EndPoint is Stalled */
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		/* Error */
		_setSenseKey( DI , 0x07 , 0x27 , 0x00 ); /* WRITE PROTECTED MEDIA */
		#ifdef DEBUG
		RS_printf("write protected check error\r\n");
		#endif
		return(bCSW_Failed);
	}

	LBA = 	(Ulong)(CmdBlk->CmdDat[2])*0x1000000 + 
			(Ulong)(CmdBlk->CmdDat[3])*0x10000 + 
			(Ulong)(CmdBlk->CmdDat[4])*0x100 + 
			(Ulong)(CmdBlk->CmdDat[5]);
	Length = (Ushort)(CmdBlk->CmdDat[7])*256 + (Ushort)(CmdBlk->CmdDat[8]);

	size = Length * (DI->MSCA_IDIdata.bytepersector);
	if( *residue != size )
	{	// Bad Size
		if( *residue < size ) {
			/* Dummy read */
			while (1) {
				if ((*residue) <= dummy_size) {
			    	break;
				} else if (((*residue) - dummy_size) < 1024) {
					MWUSB_Read((unsigned char *)&dummy[0],
					           ((*residue) - dummy_size), BULK_READ_FIFO);
					break;
				} else {
					MWUSB_Read((unsigned char *)&dummy[0], 1024, BULK_READ_FIFO);
					dummy_size = dummy_size + 1024;
				}
			}
			/* Set data size */
			(*residue) = size - (*residue);
			MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
			/* Error */
			return( bCSW_PhaseError );
        }
    }
		    
	
	/* Validity check of LBA & Length */
	if( (LBA+Length) > (DI->MSCA_IDIdata.lba_sector) ) { /* change 2002.07.15 */
		/* Error */
		/* Endpoint Stalled */
		if( *residue > 0 )
			MSCP_EPStalled( BULK_READ_FIFO );	/* Bulk-Out EndPoint is Stalled */
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		_setSenseKey( DI , 0x05 , 0x21 , 0x00 ); /* LOGICAL BLOCK ADDRESS OUT OF RANGE */
		#ifdef DEBUG
		RS_printf("write out of range error\r\n");
		#endif
		return(bCSW_Failed);
	}

	if( Length > 0 ) {
#ifndef MSC_SERIAL_WRITE
/******************/
/* parallel Write */
/******************/
		/* Get Message  buffer */
		ret=MSC_get_blf( (MSC_VP *)&Request_msg ,(MSC_ID)W_MPFID_OTHERMSG );
		if (ret != MSC_OK ) {
#ifdef USE_UART_MESSAGE
			RS_printf("write get message memory error\r\n");
#endif
			return(bCSW_Failed);
		}
/* >> Critical point */
		__DI();
		Request_msg->size = Length * (DI->MSCA_IDIdata.bytepersector);	/* Total size of transfer */
		Request_msg->buf = NULL;
		__EI();
/* << Critical point */
		ret = MSC_snd_msg( W_MID_MSCP_RCV_REQUEST , (T_MSC_MSG *)Request_msg );
		if (ret != MSC_OK ) {
#ifdef USE_UART_MESSAGE
			RS_printf("write snd request error\r\n");
#endif
			return(bCSW_Failed);
		}

		while( Length > 0 ) {
			ret = MSC_rcv_msg( (T_MSC_MSG **)&Response_msg , W_MID_MSCP_RCV_RESPONSE ) ;
			if (ret != MSC_OK ) {
#ifdef USE_UART_MESSAGE
				RS_printf("write rcv response error\r\n");
#endif
				return(bCSW_Failed);
			}
/* >> Critical point */
			__DI();
			pbuf_rcv_release = Response_msg->buf;
			buf = Response_msg->buf;
			__EI();
/* << Critical point */
			if( Response_msg->size > 0 )
			{	// Receive OK
				retsize = Response_msg->size;
				size= (Response_msg->size)>>9;
			}
			else
			{	// Receive ERROR
				size = Response_msg->size;
				if( size == -123 )
				{	// Realos Bug?
					MSC_rel_blf( (MSC_ID)W_MPFID_OTHERMSG		, Request_msg );
				}
			}
			ret = MSC_rel_blf( (MSC_ID)W_MPFID_OTHERMSG		, Response_msg );
			if (ret != MSC_OK ) {
#ifdef USE_UART_MESSAGE
				RS_printf("RspMsg:%x Size:%x ILM:%d\n", Response_msg, Response_msg->size, currentILM);
				RS_printf("write relase message memory error\r\n");
#endif
				MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH	, buf);
				pbuf_rcv_release = NULL;
				return(bCSW_Failed);
			}
			if( size < 0 )
			{	// USB Receive Error
				if( buf != NULL )
				{
					MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH	, buf);
				}
				pbuf_rcv_release = NULL;
				return(bCSW_Failed);
			}
#if 0			
			//if( MSCA_WriteLBA[DI->MSCS_DriveNo]( LBA , size , buf ) != MSC_OK ) {
#else
			ret_wlba = MSCA_WriteLBA[DI->MSCS_DriveNo]( LBA , size , buf );
			if( ret_wlba != MSC_OK ) {
				
#endif				
				/* Error */
				ret=MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH	, buf);	/* Add 2002.07.15 */
				pbuf_rcv_release = NULL;
#ifdef USE_UART_MESSAGE
				if (ret != MSC_OK ) {
					RS_printf("write relase memory error(write error)\r\n");
				}
#endif
				/* Endpoint Stalled */
				if( (*residue - retsize) > 0 )
					MSCP_EPStalled( BULK_READ_FIFO  );	/* Bulk-Out EndPoint is Stalled */
				MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
				_setSenseKey( DI , 0x03 , 0x03 , 0x00 ); /* WRITE FAULT */
#ifdef USE_UART_MESSAGE
				RS_printf("write media error\r\n");
#endif
				return(bCSW_Failed);
			}
			*residue -= retsize;
			ret=MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH	, buf);
			pbuf_rcv_release = NULL;
			if (ret != MSC_OK ) {
#ifdef USE_UART_MESSAGE
				RS_printf("write relase memory error\r\n");
#endif
				return(bCSW_Failed);
			}
			Length -= size ;
			LBA += size ;
		}
#else
/****************/
/* SERIAL WRITE */
/****************/
		/* Get Memory buffer */
		ret = MSC_get_blf( (MSC_VP *)&pbuf_rcv_release ,(MSC_ID)W_MPFID_EXTBUF_HIGH);
		if (ret != MSC_OK ) {
			#ifdef DEBUG
			RS_printf("usb relase message memory error\r\n");
			return( bCSW_Failed);
			#endif
		}
		residue_size = *residue;
		while( residue_size > 0 ) {
			buf = pbuf_rcv_release;
					
			/* Receive Data from USB */
			/* *residue > (Length * bytepersector) */
			if ( residue_size > W_MPFID_EXTBUF_HIGH_SIZE ) {
				retsize = MWUSB_Read( buf , W_MPFID_EXTBUF_HIGH_SIZE , BULK_READ_FIFO);
				if (retsize <= 0 ) {
					#ifdef DEBUG
					RS_printf("usb read:1 error\r\n");
					#endif
					MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH	, pbuf_rcv_release);
					pbuf_rcv_release = NULL;
					return(bCSW_Failed);
				}
				if ( Length > (W_MPFID_EXTBUF_HIGH_SIZE>>9) ) {
					size	= (W_MPFID_EXTBUF_HIGH_SIZE>>9);	/* sector */
					Length -= (W_MPFID_EXTBUF_HIGH_SIZE>>9);	/* sector */
				} else {
					size    = (LONG)Length;                     /* sector */
					Length  = 0;
				}
			} else {
				retsize = MWUSB_Read( buf , residue_size, BULK_READ_FIFO);
				if (retsize <= 0 ) {
					#ifdef DEBUG
					RS_printf("usb read:2 error\r\n");
					#endif
					MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH	, pbuf_rcv_release);
					pbuf_rcv_release = NULL;
					return(bCSW_Failed);
				}
				size	= (LONG)Length;	/* sector */
				Length	= 0;
			}

			if ( (size > 0) ) {
				/* Write Data to Media */
				if( MSCA_WriteLBA[DI->MSCS_DriveNo]( LBA , size , buf ) != MSC_OK ) {
					/* Error */
					ret=MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH	, pbuf_rcv_release);	/* Add 2002.07.15 */
					pbuf_rcv_release = NULL;
					#ifdef DEBUG
					if (ret != MSC_OK ) {
						RS_printf("write relase memory error(write error)\r\n");
					}
					#endif
					/* Endpoint Stalled */
					if( (*residue - retsize) > 0 )
						MSCP_EPStalled( BULK_READ_FIFO );	/* Bulk-Out EndPoint is Stalled */
					MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
					_setSenseKey( DI , 0x03 , 0x03 , 0x00 ); /* WRITE FAULT */
					#ifdef DEBUG
					RS_printf("write media error\r\n");
					#endif
					return(bCSW_Failed);
				}
				*residue -= ( size * (DI->MSCA_IDIdata.bytepersector) );	// 2006.11.09 add for USBCV 1.3 MSC Test
				LBA += size ;	/* sector */
			}
			residue_size -= retsize;
		}
		/* Release memory buffer */
		ret=MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH	, pbuf_rcv_release);
		pbuf_rcv_release = NULL;
		if (ret != MSC_OK ) {
#ifdef DEBUG
			RS_printf("write relase memory error\r\n");
#endif
			return(bCSW_Failed);
		}
		pbuf_rcv_release = NULL;
#endif /* MSC_SERIAL_WRITE */
	}	/* end of if( Length>0) */
	
	return( bCSW_Passed );
}


int MSCS_Cmd_Invalid(MSCS_CB* CmdBlk,MSCS_DriveInfo_t *DI, Ulong *residue)
{
	/* Endpoint Stalled after Bulk-Out data was read */
//	MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
	_setSenseKey(DI,0x05,0x20,0x00);	/*	INVALID COMMAND OPERATION CODE */
	return( bCSW_Failed );
}
/*  Exec User SCSI Command */
#if 1
/** 2010.11.11 by tomida. add bypass flag(pass_stall) to STALL. **/
static int MSCS_ExecUserCommand(mscs_user_cmdtbl_t *tbl, MSCP_CBW* CBWdata, Ulong *residue, unsigned char pass_stall)
{
	int ret = bCSW_Passed;
	mscs_user_cmd_t		func_noparam;
	mscs_user2_cmd_t	func_2param;
	
	if( tbl->type == 0 )
	{	// No Parameter
		func_noparam = *((mscs_user_cmd_t *)&(tbl->func));
		ret = func_noparam();
		if( ret == MSC_OK )
		{	// Pass
			*residue = 0;
			ret = bCSW_Passed;
		}
		else
		{	// Fail
			/* add if 2010.11.11 by tomida */
			if( pass_stall != 1 )
			{
				MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
			}
			//_setSenseKey(DI,0x05,0x20,0x00);	/*	INVALID COMMAND OPERATION CODE */
			ret = bCSW_Failed;
		}
	}
	else
	{	// 2 Paramter
		func_2param = *((mscs_user2_cmd_t *)&(tbl->func));
		ret = func_2param((USBCBW*)CBWdata, residue);
		if( ret == MSC_OK )
		{	// Pass
			ret = bCSW_Passed;
		}
		else
		{	// Fail
			/* add if 2010.11.11 by tomida */
			if( pass_stall != 1 )
			{
				MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
			}
			//_setSenseKey(DI,0x05,0x20,0x00);	/*	INVALID COMMAND OPERATION CODE */
			ret = bCSW_Failed;
		}
	}
	return ret;
}
#else
// 2009.06.16
static int MSCS_ExecUserCommand(mscs_user_cmdtbl_t *tbl, MSCP_CBW* CBWdata, Ulong *residue)
{
	int ret = bCSW_Passed;
	mscs_user_cmd_t		func_noparam;
	mscs_user2_cmd_t	func_2param;
	
	if( tbl->type == 0 )
	{	// No Parameter
		func_noparam = (mscs_user_cmd_t)tbl->func;
		ret = func_noparam();
		if( ret == MSC_OK )
		{	// Pass
			*residue = 0;
			ret = bCSW_Passed;
		}
		else
		{	// Fail
			MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
			//_setSenseKey(DI,0x05,0x20,0x00);	/*	INVALID COMMAND OPERATION CODE */
			ret = bCSW_Failed;
		}
	}
	else
	{	// 2 Paramter
		func_2param = (mscs_user2_cmd_t)tbl->func;
		ret = func_2param((USBCBW*)CBWdata, residue);
		if( ret == MSC_OK )
		{	// Pass
			ret = bCSW_Passed;
		}
		else
		{	// Fail
			MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
			//_setSenseKey(DI,0x05,0x20,0x00);	/*	INVALID COMMAND OPERATION CODE */
			ret = bCSW_Failed;
		}
	}
	return ret;
}

// Add 2006.12.25
static int MSCS_ExecUserCommand(mscs_user_cmdtbl_t *tbl, MSCS_DriveInfo_t *DI, Ulong *residue)
{
	int ret;
	
	ret = tbl->func();
	if( ret == MSC_OK )
	{	// Pass
		*residue = 0;
		ret = bCSW_Passed;
	}
	else
	{	// Fail
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		//_setSenseKey(DI,0x05,0x20,0x00);	/*	INVALID COMMAND OPERATION CODE */
		ret = bCSW_Failed;
	}
	return ret;
}
#endif

/*****************************************/
/* Storage sub-Class Common Functions	 */
/*****************************************/
int MSCS_Init(void)
{
	int i;
	MSCS_DriveInfo_t *DI;

	for( i=0 ; i<MSCS_LUN ; i++ ) {
		MSCA_SelectDrive(i);

		MSCS_Reset(i);

		DI=&MSCS_DriveInfo[i];
		/* Set Defaulf sense data */
		memset( (Uchar *)&(DI->MSCS_SENSEdata) , 0 , sizeof(MSCS_REQUEST_SENSE) );

		/* Set Inquiry data */	/* Must do before _setModePage(); */
		_setInquiry(DI);
		/* Set Mode Page data( usesd for MODE_SENSE,SELECT_SENSE command) */
		_setModePage(DI);

		DI->MSCS_MEDIUM_EJECT 	= 0;
		DI->MSCS_MEDIUM_REMOVAL	= 0;	/* Medium prevent removal */
		DI->MSCS_MEDIUM_CHANGED = 0;	/* add 2002.07.16 */
		
		gMedia_become_ready[i] = 1;
	}
	return(MSC_OK);
}

int MSCS_Reset(unsigned char driveno)
{
	MSCS_DriveInfo_t *DI;
	DI=&MSCS_DriveInfo[driveno];

	DI->MSCS_SENSEdata.ErrorCode = 0x70;

	/* Medium not present? */ /* Medium already eject ? */
	if( ((DI->MSCS_MEDIUM_REMOVAL == 0) && (DI->MSCS_MEDIUM_EJECT == 1)) || 	/* Medium allow removal & eject */
		 (MSCA_CheckMedia[DI->MSCS_DriveNo]() != MSC_OK) ) return(MSC_ERROR);

	/* Read IDI data */
	MSCA_GetDriveInfo[DI->MSCS_DriveNo](&DI->MSCA_IDIdata);

	return(MSC_OK);
}
static unsigned char LastTargetLUN=0xFF;
int MSCS_ExeCmd(MSCP_CBW* CBWdata, Ulong *residue)
{
	int result = 0;
	MSCS_CB *CmdBlk;
	unsigned char TargetLUN;
	unsigned char DatRcv;
	mscs_user_cmdtbl_t	*usrcmd;	// add 2006.12.25
	unsigned char pass_stall;       // add 2010.11.11
	unsigned int dummy_size = 0;
	
	CmdBlk = (MSCS_CB *)CBWdata->CBWCB ;
//	TargetLUN = ((CmdBlk->CmdDat[1])>>5)&&0x07;
	TargetLUN = (CBWdata->bCBWLUN)&0x0F;
	*residue = CBWdata->dCBWDataTransferLength;
	if( (CBWdata->dCBWFlags & 0x80) == 0x00 )
		DatRcv = 1;	// Receive 
	else
		DatRcv = 0;	// Send
	
	if( TargetLUN > MSCS_LUN ) { /* Out OF Supported LUN */	/* Add 2002.07.15 */
		/* Endpoint Stalled */
		if( (*residue > 0)&&(DatRcv == 1) )
			MSCP_EPStalled( BULK_READ_FIFO );	/* Bulk-Out EndPoint is Stalled */
		MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		/* Error */
		return(MSC_ERROR);
	}

	if( TargetLUN != LastTargetLUN ) {
		MSCA_SelectDrive(TargetLUN);
		LastTargetLUN = TargetLUN;
	}
	
	/* >> 2009.06.16 user command check */
	usrcmd = search_user_cmdtbl(CmdBlk->Cmd);
	if( usrcmd != NULL )
	{	/* Exec Vendor command */
#if 1
		/* add 2010.11.11 by tomida. add flag to pass_stall */
		if( CmdBlk->Cmd == Cmd_TestUnitReady )
		{
			// if host is old MAC than device is not stalled.
			pass_stall = 1;
		}
		else
		{
			pass_stall = 0;
		}
		result = MSCS_ExecUserCommand(usrcmd, CBWdata, residue, pass_stall);
#else	
		result = MSCS_ExecUserCommand(usrcmd, CBWdata, residue);
#endif
		return result;
	}
	/* << 2009.06.16  */
	/* Execute standard command */
	switch( CmdBlk->Cmd ) {
	case Cmd_Inquiry:
#ifdef USB_DEBUG2
		RS_printf("!");
#endif
//		result = MSCS_Cmd_Inquiry(CmdBlk);
#ifdef USE_UART_MESSAGE
		RS_printf("       <CMD>:INQUIRY\n\r");
#endif
		if( (*residue > 0)&&(DatRcv == 1) )
		{
			MSCP_EPStalled( BULK_READ_FIFO );	/* Bulk-Out EndPoint is Stalled */
			MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
			/* Error */
			return(MSC_ERROR);
		}
			
		result = MSCS_CmdTbl[TargetLUN][N_Cmd_Inquiry](CmdBlk,&MSCS_DriveInfo[TargetLUN], residue);
		break;
	case Cmd_ModeSense_0x1A:
#ifdef USB_DEBUG2
		RS_printf("#");
#endif
#ifdef USE_UART_MESSAGE
		RS_printf("       <CMD>:MODE_SENSE0x1A\n\r");
#endif

//		result = MSCS_Cmd_ModeSense_0x1A(CmdBlk);
		result = MSCS_CmdTbl[TargetLUN][N_Cmd_ModeSense_0x1A](CmdBlk,&MSCS_DriveInfo[TargetLUN], residue);
		break;
	case Cmd_ModeSense_0x5A:
#ifdef USB_DEBUG2
		RS_printf("$");
#endif
#ifdef USE_UART_MESSAGE
		RS_printf("       <CMD>:MODE_SENSE0x5A\n\r");
#endif

//		result = MSCS_Cmd_ModeSense_0x5A(CmdBlk);
		if( (*residue > 0)&&(DatRcv == 1) )
		{
			MSCP_EPStalled( BULK_READ_FIFO );	/* Bulk-Out EndPoint is Stalled */
			MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
			/* Error */
			return(MSC_ERROR);
		}
		result = MSCS_CmdTbl[TargetLUN][N_Cmd_ModeSense_0x5A](CmdBlk,&MSCS_DriveInfo[TargetLUN], residue);
		break;
	case Cmd_PreventAllowMediumRemoval:
#ifdef USB_DEBUG2
		RS_printf("~\r\n");
#endif
#ifdef USE_UART_MESSAGE
		RS_printf("       <CMD>:PREVENT_ALLOW_MEDIUM_REMOVAL\n\r");
#endif

//		result = MSCS_Cmd_PreventAllowMediumRemoval(CmdBlk);
		result = MSCS_CmdTbl[TargetLUN][N_Cmd_PreventAllowMediumRemoval](CmdBlk,&MSCS_DriveInfo[TargetLUN], residue);
		break;
	case Cmd_Read:
#ifdef USB_DEBUG2
			RS_printf("&");
			
#endif
#ifdef USE_UART_MESSAGE
		RS_printf("       <CMD>:READ\n\r");
#endif
//		result = MSCS_Cmd_Read(CmdBlk);
		if( (*residue > 0)&&(DatRcv == 1) )
		{
			/* Dummy read */
			while (1) {
				if ((*residue) <= dummy_size) {
			    	break;
				} else if (((*residue) - dummy_size) < 1024) {
					MWUSB_Read((unsigned char *)&dummy[0],
					           ((*residue) - dummy_size), BULK_READ_FIFO);
					break;
				} else {
					MWUSB_Read((unsigned char *)&dummy[0], 1024, BULK_READ_FIFO);
					dummy_size = dummy_size + 1024;
				}
			}

			MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
			/* Error */
			return(bCSW_PhaseError);
		}
		result = MSCS_CmdTbl[TargetLUN][N_Cmd_Read](CmdBlk,&MSCS_DriveInfo[TargetLUN], residue);
		break;
	case Cmd_ReadCapacity:
#ifdef USB_DEBUG2
			RS_printf("'");
			
#endif
#ifdef USE_UART_MESSAGE
		RS_printf("       <CMD>:READ_CAPACITY\n\r");
#endif
//		result = MSCS_Cmd_ReadCapacity(CmdBlk);
		if( (*residue > 0)&&(DatRcv == 1) )
		{
			MSCP_EPStalled( BULK_READ_FIFO );	/* Bulk-Out EndPoint is Stalled */
			MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
			/* Error */
			return(MSC_ERROR);
		}
		result = MSCS_CmdTbl[TargetLUN][N_Cmd_ReadCapacity](CmdBlk,&MSCS_DriveInfo[TargetLUN], residue);
		break;
#if 0		
	case 0x23:
#ifdef USB_DEBUG2
			RS_printf("'");
			
#endif
//		result = MSCS_Cmd_ReadCapacity(CmdBlk);
		if( (*residue > 0)&&(DatRcv == 1) )
		{
			MSCP_EPStalled( BULK_READ_FIFO );	/* Bulk-Out EndPoint is Stalled */
			MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
			/* Error */
			return(MSC_ERROR);
		}
		result = MSCS_Cmd_ReadCapacity(CmdBlk,&MSCS_DriveInfo[TargetLUN], residue);
		break;
#endif		
	case Cmd_RequestSense:
#ifdef USB_DEBUG2
			RS_printf("(");
			
#endif
#ifdef USE_UART_MESSAGE
		RS_printf("       <CMD>:REQUEST_SENSE\n\r");
#endif
//		result = MSCS_Cmd_RequestSense(CmdBlk);
		if( (*residue > 0)&&(DatRcv == 1) )
		{
			MSCP_EPStalled( BULK_READ_FIFO );	/* Bulk-Out EndPoint is Stalled */
			MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
			/* Error */
			return(MSC_ERROR);
		}
		result = MSCS_CmdTbl[TargetLUN][N_Cmd_RequestSense](CmdBlk,&MSCS_DriveInfo[TargetLUN], residue);
		break;
	case Cmd_StartStop:
#ifdef USB_DEBUG2
			RS_printf(")");
			
#endif
#ifdef USE_UART_MESSAGE
		RS_printf("       <CMD>:START_STOP\n\r");
#endif
//		result = MSCS_Cmd_StartStop(CmdBlk);
		result = MSCS_CmdTbl[TargetLUN][N_Cmd_StartStop](CmdBlk,&MSCS_DriveInfo[TargetLUN], residue);
		break;
	case Cmd_TestUnitReady:
#ifdef USB_DEBUG2
			RS_printf("=");
			
#endif
#ifdef USE_UART_MESSAGE
		RS_printf("       <CMD>:TEST_UNIT_READY\n\r");
#endif
//		result = MSCS_Cmd_TestUnitReady(CmdBlk);
		result = MSCS_CmdTbl[TargetLUN][N_Cmd_TestUnitReady](CmdBlk,&MSCS_DriveInfo[TargetLUN], residue);
		
		if ( (*residue > 0) && (DatRcv == 1) )
		{
			while(1) {
				if ((*residue) <= dummy_size) {
					break;
				} else if (((*residue) - dummy_size) < 1024) {
					MWUSB_Read((unsigned char *)&dummy[0], ((*residue) - dummy_size), BULK_READ_FIFO);
					break;
				} else {
					MWUSB_Read((unsigned char *)&dummy[0], 1024, BULK_READ_FIFO);
					dummy_size = dummy_size + 1024;
				}
			}
		}
		else if ( (*residue > 0) && (DatRcv == 0) )
		{
			MSCP_EPStalled(BULK_WRITE_FIFO);  /* Bulk-In EndPoint is Stalled */
		}

		break;
	case Cmd_Verify:
#ifdef USB_DEBUG2
			RS_printf("+");
			
#endif
#ifdef USE_UART_MESSAGE
		RS_printf("       <CMD>:VERIFY\n\r");
#endif
//		result = MSCS_Cmd_Verify(CmdBlk);
		if( CBWdata->dCBWDataTransferLength != 0 ) {	/* add 2002.07.15 */
			/* Error */
			/* Endpoint Stalled */
			if( (*residue > 0)&&(DatRcv == 1) )
				MSCP_EPStalled( BULK_READ_FIFO );	/* Bulk-Out EndPoint is Stalled */
			MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
			_setSenseKey(&MSCS_DriveInfo[TargetLUN],0x05,0x24,0x00);	/* INVALID FIELD IN COMMAND PACKET */
			result = MSC_ERROR;
		}
		result = MSCS_CmdTbl[TargetLUN][N_Cmd_Verify](CmdBlk,&MSCS_DriveInfo[TargetLUN], residue);
		break;
	case Cmd_Write:
#ifdef USB_DEBUG2
			RS_printf("@");
			
#endif
#ifdef USE_UART_MESSAGE
		RS_printf("       <CMD>:WRITE\n\r");
#endif
//		result = MSCS_Cmd_Write(CmdBlk);
		if( (*residue > 0)&&(DatRcv == 0) )
		{
			MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
			/* Error */
			return(MSC_ERROR);
		}
		result = MSCS_CmdTbl[TargetLUN][N_Cmd_Write](CmdBlk,&MSCS_DriveInfo[TargetLUN], residue);
		break;
	default:
#if 1	// Modify for Vendor command 2009.06.16
#ifdef USB_DEBUG2
		RS_printf("?");
			
#endif
#ifdef USE_UART_MESSAGE
		RS_printf("       <CMD>:ERROR-INVALID\n\r");
#endif
		result = MSCS_Cmd_Invalid(CmdBlk,&MSCS_DriveInfo[TargetLUN], residue);
#else		
		/* serach user command table */
		usrcmd = search_user_cmdtbl(CmdBlk->Cmd);
		if( usrcmd != NULL )
		{	// Exec User Command
			result = MSCS_ExecUserCommand(usrcmd, &MSCS_DriveInfo[TargetLUN], residue);
		}
		else
		{
			/* Command Error */
			/* Invalid Command */
#ifdef USB_DEBUG2
			RS_printf("?");
			
#endif
			result = MSCS_Cmd_Invalid(CmdBlk,&MSCS_DriveInfo[TargetLUN], residue);
		}
#endif		
		if(((*residue) > 0)&&((CBWdata->dCBWFlags & 0x80)==0))
		{	/* Data Phase Exist */
			/* Dummy read */
			while (1) {
				if ((*residue) <= dummy_size) {
				    result = -1;
			    	break;
				} else if (((*residue) - dummy_size) < 1024) {
					MWUSB_Read((unsigned char *)&dummy[0],
					           ((*residue) - dummy_size), BULK_READ_FIFO);
					result = -1;
					break;
				} else {
					MWUSB_Read((unsigned char *)&dummy[0], 1024, BULK_READ_FIFO);
					dummy_size = dummy_size + 1024;
				}
			}
		}
		MSCP_EPStalled( BULK_WRITE_FIFO );
		break;
	}
	return( result ); /* OK */
}


int MSCS_TaskTerm(void)
{
	MSCP_USB_MSG *msg=0;
	MSC_ER	ret;
	T_MSC_RMBX	ref;
	
	/* Task termination of MSCS_Cmd_READ function */
	/* Receive all jammed message in W_MID_MSCP_SND_RESPONSE */
	/* W_MID_MSCP_SND_RESPONSEのメイルボックスに溜まっているメイルを取り出す */
	while( (ret=MSC_prcv_msg( (T_MSC_MSG **)&msg , W_MID_MSCP_SND_RESPONSE )) == MSC_OK) {
		/* Release message buffer */
		ret=MSC_rel_blf( (MSC_ID)W_MPFID_OTHERMSG , msg );
		if( ret != MSC_OK ) { return(MSC_ERROR); }
	}
	/* Release memory pool in MSCS_Cmd_Read function */
	if( pbuf_snd_release != NULL ) {
		/* Release reserved memory pool */
		ret=MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH , pbuf_snd_release );
		if( ret != MSC_OK ) { return(MSC_ERROR); }
	}
	/* Storage Task が受信待ちの場合、ダミーメッセージを送り */
	/* 待ちを解除させる。                                    */
	if( MSC_ref_mbx(&ref, W_MID_MSCP_SND_RESPONSE) == MSC_OK )
	{
		if( ref.wtskid != 0 )
		{	// 待ちタスク有り  ダミーメッセージを送る */
			MSC_get_blf((MSC_VP *)&msg, (MSC_ID)W_MPFID_OTHERMSG);
			msg->size = -1;
			MSC_snd_msg(W_MID_MSCP_SND_RESPONSE, (T_MSC_MSG *)msg);
		}
	}
	/* Task termination of MSCS_Cmd_WRITE function */
	/* Receive all jammed message in W_MID_MSCP_SND_RESPONSE */
	/* W_MID_MSCP_RCV_RESPONSEのメイルボックスに溜まっているメイルを取り出す */
	while( (ret=MSC_prcv_msg( (T_MSC_MSG **)&msg , W_MID_MSCP_RCV_RESPONSE )) == MSC_OK ) {
		/* Release reserved memory pool */
		if( msg->buf != NULL ) {
			ret=MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH , msg->buf );
			if( ret != MSC_OK ) { return(MSC_ERROR); }
		}
		/* Release message buffer */
		ret=MSC_rel_blf( (MSC_ID)W_MPFID_OTHERMSG , msg );
		if( ret != MSC_OK ) { return(MSC_ERROR); }
	}
	
	if( pbuf_rcv_release != NULL ) {
		/* Release reserved memory pool */
		ret=MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH , pbuf_rcv_release );
		if( ret != MSC_OK ) { return(MSC_ERROR); }
	}
	/* Storage Task が受信待ちの場合、ダミーメッセージを送り */
	/* 待ちを解除させる。                                    */
	if( MSC_ref_mbx(&ref, W_MID_MSCP_RCV_RESPONSE) == MSC_OK )
	{
		if( ref.wtskid != 0 )
		{	// 待ちタスク有り  ダミーメッセージを送る */
			MSC_get_blf((MSC_VP *)&msg, (MSC_ID)W_MPFID_OTHERMSG);
			msg->size = -1;
			MSC_snd_msg(W_MID_MSCP_RCV_RESPONSE, (T_MSC_MSG *)msg);
		}
	}

	return(MSC_OK);
}

/*  Add Vendor SCSI Command to table */ // 2009.06.16
static int add_vendor_cmd(unsigned char code, void *func, int type)
{
	mscs_user_cmdtbl_t *tbl;
	int newflg=0;
	
	tbl = search_user_cmdtbl(code);
	if( tbl == NULL )
	{	// Not Found
		tbl = new_user_cmdtbl();
		newflg=1;
		if( tbl == NULL )
		{	// No Unused table
			return MSC_ERROR;
		}
	}
	if( func != NULL )
	{
		if( newflg == 1 )
		{
			Mscs_valid_vendcmd++;
			if( Mscs_valid_vendcmd > MSCS_MAX_USER_CMD )
				Mscs_valid_vendcmd = MSCS_MAX_USER_CMD;
		}
		tbl->valid = 1;
		tbl->code = code;
		tbl->func = func;
		tbl->type = type;
	}
	else
	{
		if( (newflg == 0)&&(tbl->valid == 1) )
		{
			Mscs_valid_vendcmd--;
			if( Mscs_valid_vendcmd < 0 )
				Mscs_valid_vendcmd = 0;
		}
		tbl->valid = 0;
		tbl->code = 0;
		tbl->func = NULL;
		tbl->type = 0;
	}
	return MSC_OK;
}

/*  Add User SCSI Command Function No Params  */ // 2006.12.25

int MSCS_AddCmdFunction(unsigned char code, int (*func)(void))
{
#if 1	// 2009.06.16
	return (add_vendor_cmd(code, (void*)func, 0));
#else
	mscs_user_cmdtbl_t *tbl;
	
	tbl = search_user_cmdtbl(code);
	if( tbl == NULL )
	{	// Not Found
		tbl = new_user_cmdtbl();
		if( tbl == NULL )
		{	// No Unused table
			return MSC_ERROR;
		}
	}
	tbl->valid = 1;
	tbl->code = code;
	tbl->func = (void*)func;
	tbl->type = 0;
	return MSC_OK;
#endif	
}

/*  Add Vendor SCSI Command Function 2 Params */ // 2009.06.16
int MSCS_AddVendorCmdFunc2(unsigned char code, int(*func)(USBCBW* cbw, unsigned long* residue))
{
	return (add_vendor_cmd(code, (void*)func, 1));
}

/* Set Sense key code */
int MSCS_SetSenseKey(unsigned char lun, unsigned char skey, unsigned char asc, unsigned char ascq)
{
	MSCS_DriveInfo_t *DI;
	
	if( lun > MSCS_LUN ) { /* Out OF Supported LUN */
		return MSC_ERROR;
	}
	DI = &MSCS_DriveInfo[lun];
	_setSenseKey(DI, skey, asc, ascq);
	return MSC_OK;
}

