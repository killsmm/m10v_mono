/************************************************************************/
/*	<Contents>		USB Mass Storage class protocol (Bulk-Only)			*/
/*	<Filename>		MSCP_Bulkonly.c										*/
/*	<Enviroment> Softune V6 for FR-80									*/
/*	<Creation Date/author>												*/
/*		2001/07/06		N.Uenishi										*/
/*	<Latest modify Date/author>											*/
/*		2006/11/13		S.Hibino										*/
/*		2008/10/30		S.Hibino										*/
/*			データ受信タスクがメモリ確保失敗時にタスクスリープを追加	*/
/*		2016/01/26		H.Hata											*/
/*	<Version>		0.2													*/
/*							Socionext Middleware Library				*/
/*								Copyright Socionext Inc. 2001-20016		*/
/************************************************************************/
/*      USB_EN_SECTION	Enable Section Definition						*/
/************************************************************************/

/* Section Definition */
#ifdef USB_EN_SECTION
#include "MSC_Section.h"
#endif

#include <string.h>
#include "dd_arm.h"

#include "msc_wrapos.h"
#include "msclib.h"
#include "MSCP_Bulkonly.h"
#include "mwusb_driver4.h"

#ifdef DMA_TCHECK
#include "os_user_custom.h"
#endif

/* USE_UART_MESSAGE : Use UART output  */
//#define USE_UART_MESSAGE

#ifdef USE_UART_MESSAGE
	#define RS_printf printf
#endif

#ifdef USB_COMPLIANCE_CHECK
	#define RS_printf printf
#endif

#define MSC_SERIAL_READ
#define MSC_SERIAL_WRITE

#ifdef DEBUG
MSCP_CBW	CBWBuf;
MSCP_CSW	CSWBuf;
#else
static MSCP_CBW	CBWBuf;
static MSCP_CSW	CSWBuf;
#endif
const char MSCP_CBWSIGN[] = "USBC";
const char MSCP_CSWSIGN[] = "USBS";

/* For Memory release at terminaion task */
#ifdef DEBUG
unsigned char *pbuf_rcv_release=(void *)NULL;
unsigned char *pbuf_snd_release=(void *)NULL;
#else
static unsigned char *pbuf_rcv_release=(void *)NULL;
static unsigned char *pbuf_snd_release=(void *)NULL;
#endif

#ifndef MSC_SERIAL_READ
/* Data Send Stop Flag for Parallel Read */
static int send_stop = 0;
/* Total size of send data for parallel read */
static unsigned long total_send = 0;
#endif


int MSCP_Init(void)
{
#ifndef MSC_SERIAL_READ
	send_stop = 0;
	total_send = 0;
#endif
	
	MSC_sta_tsk();

	MSCP_Reset();

#if 0	/* ライブラリの外で行う事 */
	/* USB Reset */
	MWUSB_AbortReset();
#endif

	return(MSC_OK);
}

int MSCP_Reset(void)
{
	/* Buffer Memory Clear */
	memset( (char*)&CBWBuf, 0 , sizeof(MSCP_CBW) );
	memset( (char*)&CSWBuf, 0 , sizeof(MSCP_CSW) );

	/* Initialize Buffer Data */
	memcpy( (char*)&(CSWBuf.dCSWSignature) , MSCP_CSWSIGN , 4 );
	CSWBuf.dCSWDataResidue = 0;

#ifndef MSC_SERIAL_READ
	send_stop = 0;
	total_send = 0;
#endif
	
	return(MSC_OK);

}

#ifdef USB_COMPLIANCE_CHECK
int cmd_no = 0;
#endif
int MSCP_RcvCmd( Uchar *CmdBuf )
{
	long ret;
#ifdef MSC_BIG_ENDIAN
	union {
		unsigned long ul;
		unsigned char uc[4];
	} swp;
	unsigned char wk;
#endif /* MSC_BIG_ENDIAN */

	/* Receive Command Block Wrapper Data */
	ret = MWUSB_Read((unsigned char*)&CBWBuf,31,BULK_READ_FIFO);
#ifdef USB_COMPLIANCE_CHECK
	{
		RS_printf("----<data size %ld>-----\n\r",ret);
		RS_printf("   CBW->dCBWSignature          = %lX \n\r",CBWBuf.dCBWSignature);
		RS_printf("   CBW->dCBWTag                = %lX \n\r",CBWBuf.dCBWTag);
		RS_printf("   CBW->dCBWDataTransferLength = %lX \n\r",CBWBuf.dCBWDataTransferLength);
		RS_printf("   CBW->dCBWFlags              = %X \n\r",CBWBuf.dCBWFlags);
		RS_printf("   CBW->bCBWLUN                = %X \n\r",CBWBuf.bCBWLUN);
		RS_printf("   CBW->bCBWCBLength           = %X \n\r",CBWBuf.bCBWCBLength);
	}
	cmd_no++;
#else 
//	dly_tsk(1);
#endif
	if( ret > 0 )
	{	
		if( ret != 31 )
		{	// Bad Container Length
			/* Endpoint Stalled  add 2002.07.16 */
			MSCP_EPStalled( BULK_READ_FIFO );	/* Bulk-Out EndPoint is Stalled */	
			MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
			return( MSC_ERROR);
		}	
		/* Check Command Length */
		if( ( CBWBuf.bCBWCBLength < 1 ) || ( CBWBuf.bCBWCBLength > 16 ) )
		{
			/* Endpoint Stalled  add 2002.07.16 */
			MSCP_EPStalled( BULK_READ_FIFO );	/* Bulk-Out EndPoint is Stalled */	
			MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
			return( MSC_ERROR);
		}
	
		/* Check Signature */
		if( memcmp( (char *)&(CBWBuf.dCBWSignature),(char *)MSCP_CBWSIGN,4) == 0 ) {
#ifdef MSC_BIG_ENDIAN
			/* Transfer Swap */
			swp.ul = CBWBuf.dCBWDataTransferLength;
			wk = swp.uc[0]; swp.uc[0] = swp.uc[3]; swp.uc[3] = wk;
			wk = swp.uc[1]; swp.uc[1] = swp.uc[2]; swp.uc[2] = wk;
			CBWBuf.dCBWDataTransferLength = swp.ul;
#endif /* MSC_BIG_ENDIAN */
			/* Copy Command Block data to buffer */
			memcpy( CmdBuf,(Uchar *)&CBWBuf,31);
			return( MSC_OK ); /* OK */
		}
		else
		{
			/* Error */
			/* Endpoint Stalled */	/* add 2002.07.16 */
/* comment out in M7 */
//			MSCP_EPStalled( BULK_READ_FIFO );	/* Bulk-Out EndPoint is Stalled */	
//			MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		}
		return( MSC_ERROR ); /* Error */
	}
	else
	{
		if( (ret == MWUSB_ERR_NULLPACKET)||(ret == MWUSB_ERR_SHORTPACKET) )
		{	// Bulk Pipe is Active
			MSCP_EPStalled( BULK_READ_FIFO );	/* Bulk-Out EndPoint is Stalled */	
			MSCP_EPStalled( BULK_WRITE_FIFO );	/* Bulk-In EndPoint is Stalled */
		}
		if((ret == MWUSB_ERR_CLSRESET) || (ret == 0))
		{ /* MSC Reset */
		  /* 
		   * When MSC Reset is received between until the event
		   * flag(FLG_USB_BulkReadRdy) waiting in MWUSB_Read,
		   * and BulkOut_Trans.phase becomes IDLE, ret is 0.
		  */
			return (MSC_ABORT);
		}
		else
		{
			return (MSC_ERROR );
		}
	}
}

int MSCP_RcvData( Uchar *DatBuf , Ulong Size)
{
	long ret;
	
	ret = MWUSB_Read(DatBuf,Size,BULK_READ_FIFO);
	return( (int)ret );
}

int MSCP_SndData( Uchar *DatBuf , Ulong Size)
{
	long ret;
	ret = MWUSB_Write(DatBuf,Size,BULK_WRITE_FIFO);
	return( (int)ret );
}
// 2006.11.13 add arg residue for USBCV 1.3
int MSCP_SndStatus( MSCP_CSW_STATUS status, Ulong residue )
{
#ifdef MSC_BIG_ENDIAN
	union {
		unsigned long ul;
		unsigned char uc[4];
	} swp;
	unsigned char wk;
#endif /* MSC_BIG_ENDIAN */

#ifdef USB_COMPLIANCE_CHECK
	{
		RS_printf("  ===>MSCP_SendStatus<%d:residue=0x%lX:tag[%lX]>\n\r",status,residue,CBWBuf.dCBWTag);
	}
#else
//	dly_tsk(1);
#endif

	/* Copy CBWTag to CSWTag */
	memcpy( (char*)&(CSWBuf.dCSWTag) , (char*)&(CBWBuf.dCBWTag) , 4 );
	/* Set Status */
	CSWBuf.bCSWStatus = (Uchar)status ;
	/* Set Residue */
#ifdef MSC_BIG_ENDIAN
	swp.ul = residue;
	wk = swp.uc[0]; swp.uc[0] = swp.uc[3]; swp.uc[3] = wk;
	wk = swp.uc[1]; swp.uc[1] = swp.uc[2]; swp.uc[2] = wk;
	CSWBuf.dCSWDataResidue = swp.ul;
#else /* MSC_BIG_ENDIAN */
/* Little Endian M-7 */
	CSWBuf.dCSWDataResidue = residue;
#endif /* MSC_BIG_ENDIAN */

	/* Send Status Data */
	MWUSB_Write((unsigned char*)&CSWBuf,13,BULK_WRITE_FIFO);

	return(MSC_OK);	/* OK */
}

void MSCP_Rcv_Request(int stacd )
{
#ifndef MSC_SERIAL_WRITE
	MSCP_USB_MSG	*Request_msg=0,*Response_msg;
	Uchar *buf;
	LONG residual_size;
	LONG size;
	//ULONG	currentILM;
	MSC_ER ret = 0;
	long retsize;

	while(1)
	{
		ret = MSC_rcv_msg( (T_MSC_MSG **)&Request_msg , W_MID_MSCP_RCV_REQUEST ) ;
		if (ret != MSC_OK )
		{
#ifdef USE_UART_MESSAGE
			RS_printf("usb rcv request error\n");
#endif
			continue;
		}
		residual_size = Request_msg->size; /* Total size of transfer */
		/* Release message buffer */
		ret = MSC_rel_blf( (MSC_ID)W_MPFID_OTHERMSG , Request_msg );
		if (ret != MSC_OK ) {
#ifdef USE_UART_MESSAGE
			RS_printf("RcvMsg:%x Size:%x\n", Request_msg, residual_size);
			RS_printf("usb relase message memory error\n");
			MSC_tslp_tsk(20);
#endif
			buf = NULL;
			size = -1;
			residual_size = 0;
			ret = MSC_get_blf( (MSC_VP *)&Response_msg ,(MSC_ID)W_MPFID_OTHERMSG );
			if (ret != MSC_OK ) {
#ifdef USE_UART_MESSAGE
				RS_printf("usb get message memory error\n");
#endif
			}
			Response_msg->buf = NULL;
			Response_msg->size = -123 ;
			ret = MSC_snd_msg( W_MID_MSCP_RCV_RESPONSE , (T_MSC_MSG *)Response_msg );
			if (ret != MSC_OK ) {
#ifdef USE_UART_MESSAGE
				RS_printf("usb snd response error\n");
#endif
			}
		}
		while( residual_size > 0 )
		{
			/* Get Memory buffer */
			if( pbuf_rcv_release == NULL )
			{
				ret = MSC_get_blf( (MSC_VP *)&pbuf_rcv_release ,(MSC_ID)W_MPFID_EXTBUF_HIGH);
				if (ret != MSC_OK ) {
#ifdef USE_UART_MESSAGE
					RS_printf("usb get memory error\n");
#endif
/* >> add 2008.10.30  */
					if( ret == E_NOMEM_USB )
					{	// メモリ不足
						pbuf_rcv_release = NULL;
						MSC_tslp_tsk(1);
						continue;
					}
					/* System error */
					buf = NULL;
					size = -1;
					residual_size = 0;
/* << add 2008.10.30  */
				}
			}
			buf=pbuf_rcv_release;

			if( residual_size > W_MPFID_EXTBUF_HIGH_SIZE )
			{
				/* Receive data from USB */
				retsize = MWUSB_Read( buf , W_MPFID_EXTBUF_HIGH_SIZE , BULK_READ_FIFO);
				if (retsize <= 0 ) {
#ifdef USE_UART_MESSAGE
					RS_printf("usb read:1 error\n");
#endif
					size = retsize;
					residual_size = 0;
				}
				else
				{
					size = W_MPFID_EXTBUF_HIGH_SIZE ;
					residual_size -= W_MPFID_EXTBUF_HIGH_SIZE;
					
				}
			}
			else
			{
				/* Receive data from USB */
				retsize = MWUSB_Read( buf , residual_size , BULK_READ_FIFO);
				if (retsize <= 0 ) {
#ifdef USE_UART_MESSAGE
					RS_printf("usb read:2 error\n");
#endif
					size = retsize;
					residual_size = 0;
				}
				else
				{
					size = residual_size ;
					residual_size = 0;
				}
			}
			/* Get Memory buffer of message */
			ret = MSC_get_blf( (MSC_VP *)&Response_msg ,(MSC_ID)W_MPFID_OTHERMSG );
			if (ret != MSC_OK ) {
#ifdef USE_UART_MESSAGE
				RS_printf("usb get message memory error\n");
#endif
				continue;
			}

/* >> critical point */
			__DI();
			Response_msg->buf = buf;
			Response_msg->size = size ;
			__EI();
/* << critical point */
			ret = MSC_snd_msg( W_MID_MSCP_RCV_RESPONSE , (T_MSC_MSG *)Response_msg );
			if (ret != MSC_OK ) {
#ifdef USE_UART_MESSAGE
				RS_printf("usb snd response error\n");
#endif
				continue;
			}

			pbuf_rcv_release=(void *)NULL;
		}
	}
#else
	MSC_ext_tsk();
#endif
}

void MSCP_SendStop(void)
{
#ifndef MSC_SERIAL_READ
	send_stop = 1;
#endif
}

void MSCP_Snd_Request(int stacd )
{
#ifndef MSC_SERIAL_READ
	MSCP_USB_MSG	*Request_msg=0;
	MSC_ER ret=0;
	LONG	size;
	Uchar	*buf;
	//UINT	currentILM;
	long retsize=0;
	
	while(1) {
		if( send_stop == 1 )
		{	// Stop Data send
			/* メールの空読み */
			while( (ret = MSC_prcv_msg( (T_MSC_MSG **)&Request_msg , W_MID_MSCP_SND_REQUEST )) == MSC_OK )
			{	// Exist Mail
				/* Release message buffer */
				MSC_rel_blf( (MSC_ID)W_MPFID_OTHERMSG , Request_msg );
			}
			/* 終了メッセージ送信 */
			ret = MSC_get_blf( (MSC_VP *)&Request_msg ,(MSC_ID)W_MPFID_OTHERMSG );
			if (ret != MSC_OK ) {
#ifdef USE_UART_MESSAGE
				RS_printf("usb get message memory error\n");
#endif
			}
			Request_msg->size = total_send;
			MSC_snd_msg( W_MID_MSCP_SND_RESPONSE , (T_MSC_MSG *)Request_msg );
			send_stop = 0;
			total_send = 0;
			continue;
		}
			   
		ret=MSC_rcv_msg( (T_MSC_MSG **)&Request_msg , W_MID_MSCP_SND_REQUEST ) ;
#ifdef DEBUG
		if( ret != MSC_OK ) {
			RS_printf("Usb Snd Err1\r\n");
		}
#endif

/* >> Critical point */
		__DI();
		pbuf_snd_release= Request_msg->buf;
		buf 			= Request_msg->buf;
		size			= Request_msg->size;
		__EI();
/* << Critical point */
		if( size == 0 )
		{
			/* Send check message to Connect task */
			Request_msg->size = total_send;
			total_send = 0;
			MSC_snd_msg( W_MID_MSCP_SND_RESPONSE , (T_MSC_MSG *)Request_msg );
		}
		/*  Add 2009.12.22 bug fix */
		else if( size == -1 )
		{	// Send Stop */
			send_stop = 1;
			ret=MSC_rel_blf( (MSC_ID)W_MPFID_OTHERMSG , Request_msg );
		}
		/* << 2009.12.22 */
		else
		{
#ifdef DMA_TCHECK
			static OS_USER_SYSTIM st1, ed1;
#endif

			ret=MSC_rel_blf( (MSC_ID)W_MPFID_OTHERMSG , Request_msg );
#ifdef DEBUG
		if( ret != MSC_OK ) {
			RS_printf("Usb Snd Err2\r\n");
		}
#endif
#ifdef DMA_TCHECK
			if( size >= 0x4000 ) {
				OS_User_Get_Tim(&st1);
			}
#endif
			if( send_stop != 1 )
			{	// Not Stop
				retsize = MWUSB_Write( buf, size , BULK_WRITE_FIFO);
				/* Release reserved memory pool */
#ifdef DMA_TCHECK
			if( size >= 0x4000 ) {
				OS_User_Get_Tim(&ed1);
			}
#endif			
				ret=MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH	, buf);
#ifdef USE_UART_MESSAGE
		if( ret != MSC_OK ) {
			RS_printf("Usb Snd Err3\r\n");
		}
#endif
				pbuf_snd_release = NULL;
				if( retsize > 0 )
					total_send += retsize;
			}
		}
	}
#else
	MSC_ext_tsk();
#endif
}

int MSCP_TaskTerm( void )
{
	MSCP_USB_MSG *msg=0;
	MSC_ER	ret;

	MSC_ter_tsk();	/* add 2002.07.16 */

	/* Task termination of MSCP_Rcv_Request task */
	/* Receive all jammed message in W_MID_MSCP_RCV_REQUEST */
	/* W_MID_MSCP_RCV_REQUESTのメイルボックスに溜まっているメイルを取り出す */
	while( (ret=MSC_prcv_msg( (T_MSC_MSG **)&msg , W_MID_MSCP_RCV_REQUEST )) == MSC_OK) {
		if( ret != MSC_OK ) { return(MSC_ERROR); }
		/* Release message buffer */
		ret=MSC_rel_blf( (MSC_ID)W_MPFID_OTHERMSG , msg );
		if( ret != MSC_OK ) { return(MSC_ERROR); }
	}
	/* during USB_READ , release buffer memory before task termination */
	if( pbuf_rcv_release != NULL ) {
		/* Release reserved memory pool */
		ret=MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH , pbuf_rcv_release );
		pbuf_rcv_release = NULL;
		if( ret != MSC_OK ) { return(MSC_ERROR); }
	}

	/* Task termination of MSCP_Snd_Request task */
	/* Receive all jammed message in W_MID_MSCP_SND_REQUEST */
	/* W_MID_MSCP_SND_REQUESTのメイルボックスに溜まっているメイルを取り出す */
	while( (ret=MSC_prcv_msg( (T_MSC_MSG **)&msg , W_MID_MSCP_SND_REQUEST )) == MSC_OK ) {
		if( ret != MSC_OK ) { return(MSC_ERROR); }
		/* Release reserved memory pool */
		if( msg->buf != NULL ) {
			ret=MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH	, msg->buf);
			if( ret != MSC_OK ) { return(MSC_ERROR); }
		}
		/* Release message buffer */
		ret=MSC_rel_blf( (MSC_ID)W_MPFID_OTHERMSG , msg );
		if( ret != MSC_OK ) { return(MSC_ERROR); }
	}
	/* during USB_WRITE , release buffer memory before task termination */
	if( pbuf_snd_release != NULL ) {
		/* Release reserved memory pool */
		ret=MSC_rel_blf( (MSC_ID)W_MPFID_EXTBUF_HIGH	, pbuf_snd_release);
		pbuf_snd_release = NULL;
		if( ret != MSC_OK ) { return(MSC_ERROR); }
	}

	return(MSC_OK);
}

// FIFO Clear
void MSCP_EPInit( Uchar EP )
{
	
	switch( EP )
	{
	case READ_PIPE :
		MWUSB_EPInit(BULK_READ_FIFO);
		break;
	case WRITE_PIPE :
		MWUSB_EPInit(BULK_WRITE_FIFO);
		break;
	}
}

// FIFO Empty Check
int MSCP_CheckEPEmpty(Uchar EP)
{
	int ret=MSC_OK;
	unsigned char epno;
	
	switch( EP )
	{
	case READ_PIPE :
		epno = BULK_READ_FIFO;
		break;
	case WRITE_PIPE :
		epno = BULK_WRITE_FIFO;
		break;
	default :
		return ret;
	}
	if( MWUSB_CheckFifoEmpty(epno) == MWUSB_NG )
	{	// Not Empty
		ret = MSC_ERROR;
	}
	return ret;
}
// Cancel stall request if endpoint is not stalled.
int MSCP_EPStallCancel(Uchar EP)
{
	int ret=MSC_OK;
	unsigned char epno;
	
	switch( EP )
	{
	case READ_PIPE :
		epno = BULK_READ_FIFO;
		break;
	case WRITE_PIPE :
		epno = BULK_WRITE_FIFO;
		break;
	default :
		return ret;
	}
	if( MWUSB_EPStallCancel(epno) == MWUSB_NG )
	{	// Not Empty
		ret = MSC_ERROR;
	}
	return ret;
}

/* add 2011.01.18 Cancel SBW by tomida */
void MSCP_CancelCmd(Uchar EP, MSCP_CBW * tcbw)
{
	unsigned int size = 0;
	size = (unsigned int)tcbw->dCBWDataTransferLength;
	
	if( (size != 0)&&((tcbw->dCBWFlags & 0x80) == 0) ) 
	{
		MWUSB_BulkRead_Dummy(EP,size);
	}
	return;
}

