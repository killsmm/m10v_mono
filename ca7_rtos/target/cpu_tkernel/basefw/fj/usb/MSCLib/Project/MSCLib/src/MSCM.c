/************************************************************************/
/*	<Contents>		USB Mass Storage class Manager 						*/
/*	<Filename>		MSCM.c												*/
/*	<Enviroment>	Softune V6 for FR-80								*/
/*	<Creation Date/author>												*/
/*		2001/07/06		N.Uenishi										*/
/*	<Latest modify Date/author>											*/
/*		2005/12/28		S.Hibino										*/
/*		2006/06/27		S.Hibino										*/
/*						Add Version String								*/
/*		2007/06/27		S.Hibino										*/
/*						Version change.									*/
/*		2016/01/26		H.Hata											*/
/*	<Version>		0.1													*/
/*							Socionext Middleware Library				*/
/*								Copyright Socionext Inc. 2001-20016		*/
/************************************************************************/
/*      USB_EN_SECTION	Enable Section Definition						*/
/************************************************************************/

/* Section Definition */
#ifdef USB_EN_SECTION
#include "MSC_Section.h"
#endif

#include "msclib.h"
#include "MSCP_Bulkonly.h"
#include "MSCS.h"
#include "MSCS_SCSI.h"
#include "MSCA.h"
#include "msca_mediachange.h"
#include "msc_wrapos.h"
#include "mwusb_driver4.h"

#ifdef USB_EVENT_TIME
#include "os_user_custom.h"
#endif

/* USE_UART_MESSAGE : Use UART output  */
//#define USE_UART_MESSAGE

#ifdef USE_UART_MESSAGE
	#define RS_printf printf
#else
	#define RS_printf(...)
#endif

/* USE_Event_Time : check execution time */
//#define USE_EVENT_TIME
#ifdef USE_EVENT_TIME
static OS_USER_SYSTIM   stime,etime;
static OS_USER_ER       ercd;
#endif

/* static char Msc_Library_Version[] = "Msc Lib Ver. 2.00 2011.02.22"; */

static LONG	usb_status = MSC_STS_TERM;

static int ChipCheck(void)	/* for Mlna-2/2L~ (Not Mlna-1) */
{
#if 0					/* 2016.01.21 */
	ULONG *usb_reg_rst=(ULONG *)(0x2B020320);		/* for M8M otgtc_otg control register(chip-top version) */
	unsigned long val_little,val_big;
	
	val_little = (*usb_reg_rst >> 8 )&0xff;
	val_big    = (*usb_reg_rst >> 16)&0xff;

	if( (val_little == 0x95) || (val_big == 0x95) )  {	// Modify 2011.02.22 60MHz=149
		return(MSC_OK);
	} else {
		return(MSC_ERROR);
	}
#endif
	return MSC_OK;		/* 201601.21 */
}

int MSCM_Init(void)
{
	unsigned int i;
	
	if( ChipCheck() != MSC_OK) {
		return(MSC_ERROR);
	}
	
	for(i=0;i<MSCS_LUN ;i++){
//		while( MSCA_Init[i](i) != MSC_OK );
			MSCA_Init[i](i);
	}
//	while( MSCS_Init() != MSC_OK );
	MSCS_Init();
//	while( MSCP_Init() != MSC_OK );
	MSCP_Init();
	if( usb_status == MSC_STS_TERM )
	{
		usb_status = MSC_STS_UNCONF;
	}
	
	return(MSC_OK);
}

int MSCM_InitChgDrive(void)
{
	unsigned int i;
	
	
	for(i=0;i<MSCS_LUN ;i++){
		MSCA_Init[i](i);
	}
	MSCS_Init();
	MSCP_Reset();
	
	return(MSC_OK);
}

int MSCM_Reset(void)
{
	unsigned int i;
	for( i=0;i<MSCS_LUN;i++){
		MSCA_Reset[i]();
		MSCS_Reset((unsigned char)i);
	}
	MSCP_Reset();
	
	return(MSC_OK);
}

int MSCM_Main(void)
{
	MSCP_CBW cmd_blk;
	int i,ret;
	Ulong residue=0;

#ifdef USE_UART_CONDITION
	if( Cur_Stat != usb_status )
	{
		/** current status **/
		if( Cur_Stat == MSC_STS_CONFIG )
		{
			RS_printf(" STATUS_CHANGE[CONFIG]->");
		}
		else if( Cur_Stat == MSC_STS_TERM )
		{
			RS_printf(" STATUS_CHANGE[TERM]--->");
		}
		else
		{
			RS_printf(" STATUS_CHANGE[UNCONF]->");
		}
		/** now status **/
		if( usb_status == MSC_STS_CONFIG )
		{
			RS_printf("[CONFIG]\n\r");
		}
		else if( usb_status == MSC_STS_TERM )
		{
			RS_printf("[TERM]\n\r");
		}
		else
		{
			RS_printf("[UNCONF]\n\r");
		}
		Cur_Stat = usb_status;
	}
#endif
	
	if( usb_status == MSC_STS_CONFIG )
	{
		/* Get Command Block */
#ifdef USE_UART_MESSAGE
 		RS_printf("CMD Rcv\n");
#endif
#ifdef USE_EVENT_TIME
		ercd =OS_User_Get_Tim(&stime);
		if( ercd != 0 )printf("  (%d)====>TIME:error\n\r",ercd);
#endif
		ret = MSCP_RcvCmd( (Uchar*)&cmd_blk);
		if( ret != MSC_OK ) {
#ifdef USE_EVENT_TIME
			ercd = OS_User_Get_Tim(&etime);
			printf("  (%d)====>TIME: %lld (MSCP_RcvCmd)ErrorEnd\n\r",ercd,etime-stime);
#endif
			if( (usb_status != MSC_STS_UNCONF)&&(usb_status != MSC_STS_TERM) )
			{	// Bukj Pipe is Active
				if( ret != MSC_ABORT )
				{
					/* Send Status(PhaseError) */
					MSCP_SndStatus( bCSW_PhaseError, 0 );
				}
			}
			/* Error */
#ifdef USE_UART_MESSAGE
			RS_printf("CMD Rcv err\n");
			MSC_tslp_tsk(10);
#endif
			MSCM_Reset();
			return(MSC_ERROR);
		}
#ifdef USE_EVENT_TIME
		ercd = OS_User_Get_Tim(&etime);
		printf("  (%d)====>TIME: %lld (MSCP_RcvCmd)NormalEnd\n\r",ercd,etime-stime);
#endif
		// 20051227 Modify for media change problem -->
		//  Check & Set Media Change
		for(i=0;i<MSCS_LUN ;i++) {
			if( MSCA_MDChkChange_Drive(i) == MSC_CHANGED )
			{
				MSCA_MediaIntHandler[i]();
				MSCA_MDChangeClr_Drive((unsigned char)i);
			}
		}
		//<---- 20051227
		
#ifdef USE_UART_MESSAGE
		RS_printf("CMD Exec : %x\n", ((MSCS_CB *)cmd_blk.CBWCB)->Cmd);
//		RS_printf("CMD Exec\n");
#endif
		/* Execute Command */
#ifdef USE_EVENT_TIME
		ercd = OS_User_Get_Tim(&stime);
		if( ercd != 0 )printf("  (%d)====>TIME:error\n\r",ercd);
#endif
        ret = MSCS_ExeCmd( &cmd_blk, &residue );
		if( ret != MSC_OK ) {
#ifdef USE_EVENT_TIME
		ercd = OS_User_Get_Tim(&etime);
		printf("  (%d)====>TIME: %lld (MSCP_ExeCmd)ErrorEnd\n\r",ercd,etime-stime);
#endif
			if( (usb_status != MSC_STS_UNCONF)&&(usb_status != MSC_STS_TERM) )
			{
				/* Init Bulk Read FIFO */
				if( (residue != 0)&&((cmd_blk.dCBWFlags & 0x80) == 0)&&
				    (ret == bCSW_Failed) )
				{	// 未受信データがFIFOに溜まっている
					/* Init Bulk Read FIFO */
					MSC_tslp_tsk(10);
					MSCP_CancelCmd(BULK_READ_FIFO,&cmd_blk); // add 2011.02.22 for HDC macro
					MSCP_EPStallCancel(READ_PIPE);	// add 2008.07.17 for USBCV V1.3 MSC Test
					MSCP_EPInit( READ_PIPE );
				}
				/* Send Status(Error) */
                if (ret == bCSW_PhaseError) {
					MSCP_SndStatus( bCSW_PhaseError, residue );
				} else {
					MSCP_SndStatus( bCSW_Failed, residue );
				}
#ifdef USE_UART_MESSAGE
				RS_printf("CMD(%x) LUN(%d) err\n", ((MSCS_CB *)cmd_blk.CBWCB)->Cmd, cmd_blk.bCBWLUN);
				MSC_tslp_tsk(10);
#endif
			}
			/* Error */
			MSCM_Reset();
			if( usb_status == MSC_STS_TERM )
				return(MSC_TERM);	// add 2008.10.27
			else
				return(MSC_ERROR);
		}
		else
		{
#ifdef USE_EVENT_TIME
		ercd = OS_User_Get_Tim(&etime);
		printf("  (%d)====>TIME: %lld (MSCP_ExeCmd)NormalEnd\n\r",ercd,etime-stime);
#endif
			if( (usb_status != MSC_STS_UNCONF)&&(usb_status != MSC_STS_TERM) )
			{
				if( (residue != 0)&&((cmd_blk.dCBWFlags & 0x80) == 0) )
				{	// 未受信データがFIFOに溜まっている
					/* Init Bulk Read FIFO */
					MSC_tslp_tsk(10);
					MSCP_CancelCmd(BULK_READ_FIFO,&cmd_blk); // add 2011.02.22 for HDC macro
					MSCP_EPStallCancel(READ_PIPE);	// add 2008.07.17 for USBCV V1.3 MSC Test
					MSCP_EPInit( READ_PIPE );
				}
				MSCP_SndStatus( bCSW_Passed, residue );
			}
#ifdef USE_UART_MESSAGE
			RS_printf("CMD OK\n");
			MSC_tslp_tsk(10);
#endif
			return(MSC_OK);
		}
	}
	else if( usb_status == MSC_STS_TERM )
	{	// Storage Task Term
		return(MSC_TERM);
	}
	else
	{
		//MSC_tslp_tsk(5);
		MSC_tslp_tsk(20);
		return(MSC_OK);
	}
}

int MSCM_End(void)	/* old name MSCM_TaskTerm() */
{
	unsigned int i;
	
	usb_status = MSC_STS_TERM;
	MSCP_TaskTerm();
	for(i=0;i<MSCS_LUN;i++){
		MSCA_TaskTerm[i]();
	}
	MSCS_TaskTerm();
	return(MSC_OK);
}

int MSCM_MediaChanged(unsigned char DriveNo)
{
	int ret;
	extern MSCS_DriveInfo_t MSCS_DriveInfo[];
	MSCS_DriveInfo_t *DI;
	DI = &MSCS_DriveInfo[DriveNo];

	DI->MSCS_MEDIUM_EJECT 	= 0;	/* add 2002.07.15 */
	DI->MSCS_MEDIUM_REMOVAL	= 0;	/* add 2002.07.15 */
	
	ret=MSCS_Reset(DriveNo);
	if( ret == MSC_OK ) {
		DI->MSCS_MEDIUM_CHANGED = 1;	/* add 2002.07.16 */
	}
	return(ret);
}

void MSCM_NotifyStatus(long status)
{
	switch( status )
	{
	case MSC_STS_TERM :
	case MSC_STS_CONFIG :
		usb_status = (LONG)status;
		break;
	case MSC_STS_UNCONF :
		if( usb_status != MSC_STS_TERM)
		{
			usb_status = (LONG)status;
			MSCS_AllSetBecomeReady();
		}
		break;
	case MSC_STS_SUSPEND_START :
		if( usb_status == MSC_STS_CONFIG)
		{
			usb_status = MSC_STS_SUSPEND;
		}
		break;
	case MSC_STS_SUSPEND_END :
		if( usb_status == MSC_STS_SUSPEND)
		{
			usb_status = MSC_STS_CONFIG;
		}
		break;
	}
#if 0	
	if( usb_status != MSC_STS_TERM )
	{	// 終了要求あり
		switch( status )
		{
		case MSC_STS_TERM :
		case MSC_STS_UNCONF :
		case MSC_STS_CONFIG :
			usb_status = (LONG)status;
			break;
		case MSC_STS_SUSPEND_START :
			if( usb_status == MSC_STS_CONFIG)
			{
				usb_status = MSC_STS_SUSPEND;
			}
			break;
		case MSC_STS_SUSPEND_END :
			if( usb_status == MSC_STS_SUSPEND)
			{
				usb_status = MSC_STS_CONFIG;
			}
			break;
		}
	}
#endif	
}

long MSCM_GetUsbStatus(void)
{
	return usb_status;
}

/************* Add User SCSI Command Function **********************/
// Add 2006.12.25
int MSCM_Add_Cmd_Function(unsigned char command, int (*func)(void))
{
	return (MSCS_AddCmdFunction(command, func));
}

/************* Add Vendor Command with data phase ******************/
// Add 2009.06.16
int MSCM_Add_Vendor_Cmd_Func2(unsigned char code, int(*func)(USBCBW* cbw, unsigned long* residue))
{
	if( MSCS_AddVendorCmdFunc2(code, func) != MSC_OK)
	{	// Error
		return -1;
	}
	return 0;
}

/************* Set Sense Key Code **********************************/
// add 2009.06.18
int MSCM_Set_SenseKey(unsigned char lun, unsigned char sensekey, unsigned char asc,
					  unsigned char ascq)
{
	if( MSCS_SetSenseKey(lun, sensekey, asc, ascq) != MSC_OK )
	{	// Error 
		return -1;
	}
	return 0;
}


