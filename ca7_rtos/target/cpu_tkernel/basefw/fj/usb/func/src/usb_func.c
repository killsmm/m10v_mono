/**
 * @file		usb_func.c
 * @brief		Transfer Commands on Windows 2000
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */
//#pragma arm section rwdata="BASEFW_COMM_INIT"
//#pragma arm section rodata="BASEFW_COMM_CONST"
//#pragma arm section code="BASEFW_COMM_CODE"

/*--------------------------------------------------------------------------
Include Files
--------------------------------------------------------------------------*/
#include "ddim_typedef.h"
#include "fj_playback.h"
#include "fj_filesystem.h" 
#include "fj_usb.h"
#include "fj_usbfunc.h"
#include "mwusb_driver4.h"
#include "fj_param.h"
#include "debug.h"
#include "realos_w.h"
#include "system_attr.h"
#include "memory.h"
#include <stdlib.h>
#include <string.h>

/*--------------------------------------------------------------------------
Types/Constants
--------------------------------------------------------------------------*/
#define MAX_BUF_LEN 68
#define ALIGN_NUM   4

typedef struct Transfer_s {
	struct {
		USHORT cmd;
		USHORT param;
		ULONG len;
	} proto;

	struct {
		int data;
		int ddr;
		int nand;
	} recv;

	struct {
		int done;
		int success;
	} poll_status;

	struct {
		USHORT blks;
		ULONG len;
	} firmware;

	struct {
		ULONG dirpath;
		ULONG file_path_pkg;
	} len;
} Transfer;

typedef struct TranMsg_s
{
	ULONG op;
	ULONG tran_addr;
} TranMsg_t;

/*------------------------------------------------------------------------------
Macros
------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
Variables
--------------------------------------------------------------------------*/
const CHAR aprotocolStart[]={0xaa, 0xbb, 0xcc};

/*-------------------------------------------------------------------------
Function Prototypes
--------------------------------------------------------------------------*/
static VOID usbFuncProtoInit(VOID);
static VOID usbFuncNAck(Transfer* tran);
static VOID usbFuncAck(Transfer* tran, UCHAR status);
static VOID usbFuncVerifyParam(Transfer* tran);
static VOID usbFuncAction(Transfer* tran);
static INT32 convertValue(BYTE* p_value, INT32 sizeByByte);

/*--------------------------------------------------------------------------
Functions
--------------------------------------------------------------------------*/
VOID Usb_Func(INT32 stacd)
{
	CHAR*       pRawTransfer = NULL;
	Transfer*   tran;
//	TranMsg_t   tran_msg;
	ULONG       align = ALIGN_NUM;
	ULONG       misalign;
	UINT32      running = 1;
	LONG        Bread = 0;
	UCHAR       status;

    BF_Debug_Print_Information(("Usb_Func(), the task, TID_USB_FUNC start!\n"));
    
    /* Initialize transfer protocol */
	usbFuncProtoInit();
	
	/* Start USB Command Process Task */
	//sta_tsk(TID_USB_CMD, 0);

	pRawTransfer = (CHAR*)malloc(sizeof(Transfer) + ALIGN_NUM);
	if (pRawTransfer == NULL) {
		BF_Debug_Print_Error(("malloc failed!\r\n"));
		return;
	}

	memset(pRawTransfer, 0x0, sizeof(Transfer) + ALIGN_NUM);
	tran = (Transfer*)pRawTransfer;
	misalign = (ULONG)(pRawTransfer) & (align - 1);
	if (misalign)
		tran = (Transfer*)((ULONG)(pRawTransfer) + (align - misalign));
	
    for (; running == 1 ;) 
    {
		Bread = MWUSB_Read((UCHAR*)&tran->proto, sizeof(tran->proto), BULK_READ_FIFO);
	    
        if(Bread != sizeof(tran->proto))
        {
            OS_User_Dly_Tsk(200);
            memset(tran, 0x0 , sizeof(Transfer));
            continue;
        }


		if (tran->proto.cmd != CMD_SETPARAM             && tran->proto.cmd != CMD_GETPARAM &&
		    tran->proto.cmd != CMD_GETPARAMLONG         && tran->proto.cmd != CMD_MONITORCMD &&
		    tran->proto.cmd != CMD_SETMEMLOCATIONPTR    && tran->proto.cmd != CMD_CONFIGMEMACCESS &&
		    tran->proto.cmd != CMD_GETLONG              && tran->proto.cmd != CMD_SETLONG)
		{
		    ULONG   length;
		    
		    length = sizeof(aprotocolStart);
		    if((memcmp(aprotocolStart, (UCHAR*)&tran->proto, length) == 0) && Bread == length)
		    {
		        BF_Debug_Print_Information(("Usb_Func(), USB connection \n" ));
		    }
		    else
		    {
			    //usbFuncNAck(tran);
			    BF_Debug_Print_Information(("Usb_Func(), tran->proto.len %d \n", tran->proto.len));
			    BF_Debug_Print_Information(("Usb_Func(), tran->proto.cmd %d \n", tran->proto.cmd));
			}
			memset(tran, 0x0 , sizeof(Transfer));
			continue;
		}

		usbFuncVerifyParam(tran);
		
		if (tran->proto.param == 0x00ff)
		{
			/* Non-Ack */
			usbFuncNAck(tran);
			continue;
		}
		status = 0;

		if (tran->proto.cmd == CMD_POLL_STATUS)
		{		
			if (tran->poll_status.done && 
			    tran->poll_status.success == -1) {
				status = OP_FAILED;
			}

			if (tran->poll_status.done) {
				tran->poll_status.done = 0;
				tran->poll_status.success = 0;
			}
		}
		/* Ack */
		usbFuncAck(tran, status);

		if (tran->proto.cmd == CMD_EXIT) 
		{
			OS_User_Dly_Tsk(500);
			running = 0;
		}
		
		usbFuncAction(tran);
	}
	
	BF_Debug_Print_Information(("Usb_Func(), the task, TID_USB_FUNC End!\n"));
	
	FREE(pRawTransfer);
	    
	OS_User_Ext_Tsk();
}


/**
 * Initialize transfer protocol.
 */
VOID usbFuncProtoInit(VOID)
{
	LONG    nByteRead = 0;
	UCHAR   buf[MAX_BUF_LEN];
	UCHAR*  pBuf = NULL;	
	ULONG   align = ALIGN_NUM;
	ULONG   misalign;
	OS_USER_FLGPTN  flg_ptn;
	ULONG   length;
    
    /* Set Bypass Mode */
    MWUSB_SetMode(MWUSB_BYPASS_MODE);
	MWUSBC_SetUsbClass(USB_MODE_WDM);    /* for BYPASS_MODE */
	//MWUSBC_SetUsbClass(USB_MODE_FUNC);
	/* Set Usb Speed */
	MWUSB_SetReqSpeed(MWUSB_HISPEED);  /* switch to high-speed 2010.11.20 tomida */
#if 0
    /* Change USB Descriptor */
	MWUSBC_ChangeUsbDescriptor();
	/* Set Descriptor */
	MWUSBC_DefineDescriptor();
	/* USB Initalize */
#endif
	BF_Debug_Print_Information(("usbFuncProtoInit(), Wait for USB plug start!\r\n"));
#if 0
	MWUSB_PlugWait();
#else
    /* Change USB Descriptor & Set Descriptor & USB Initalize */
	MWUSB_Initialize(MWUSB_CONFIG_MANUAL); /* MWUSB4 2010.11.22 MWUSB_Initialize() is called Initial USB Boot */
#endif
	BF_Debug_Print_Information(("usbFuncProtoInit(), Wait for USB plug end!\r\n"));

    BF_Debug_Print_Information(("usbFuncProtoInit(), USB configuration start!\r\n"));
	OS_User_Clr_Flg(FID_USB_FUNC, 0);
	OS_User_Wai_Flg(FID_USB_FUNC, FLG_USB_FUNC_CFG, D_OS_USER_TWF_ANDW, &flg_ptn);
    BF_Debug_Print_Information(("usbFuncProtoInit(), USB configuration end!\r\n"));

	memset(buf, 0, sizeof(buf));
	pBuf = buf;
	misalign = (ULONG)(buf) & (align - 1);
	if (misalign)
		pBuf = (UCHAR*)((ULONG)(buf) + (align - misalign));

    BF_Debug_Print_Information(("usbFuncProtoInit(), USB Protocol init start.\n" ));
	//dly_tsk(500);
	length = sizeof(aprotocolStart);
	for (;;) {
		nByteRead = MWUSB_Read(pBuf, length, BULK_READ_FIFO);
		if (nByteRead != 0) {
			if ((memcmp(aprotocolStart, pBuf, length) == 0) && nByteRead == length)
				break;
		}
	}
	BF_Debug_Print_Information(("usbFuncProtoInit(), USB Protocol init end.\n" ));
}


/**
 * Spceified USB protocol Non-Ack back to PC.
 *
 * @param tran - Pointer to Transfer structure.
 */
VOID usbFuncNAck(Transfer *tran)
{
	UCHAR resp[MAX_BUF_LEN];
	UCHAR *pResp = NULL;
	ULONG align = ALIGN_NUM;
	ULONG misalign;

	memset(resp, 0x0, sizeof(resp));
	pResp = resp;
	misalign = (ULONG)(resp) & (align - 1);
	if (misalign)
		pResp = (UCHAR*)((ULONG)(resp) + (align - misalign));

	pResp[1] = (UCHAR)tran->proto.cmd;
	memset(&pResp[2], 0xff, 2);
	MWUSB_Write(pResp, MAX_CMD_LEN_BYBYTE, BULK_WRITE_FIFO);
	
	BF_Debug_Print_Information(("usbFuncNAck(), cmd %x\n",tran->proto.cmd ));
	
	memset(tran, 0x0 , sizeof(Transfer));
}


/**
 * Spceified USB protocol Ack back to PC.
 *
 * @param tran - Pointer to Transfer structure.
 * @param status - Status code.
 */
VOID usbFuncAck(Transfer *tran, UCHAR status)
{
	UCHAR   resp[MAX_BUF_LEN];
	UCHAR*  pResp = NULL;
	ULONG   align = ALIGN_NUM;
	ULONG   misalign;
        
	memset(resp, 0x0, sizeof(resp));
	pResp = resp;
	misalign = (ULONG)(resp) & (align - 1);
	if (misalign)
		pResp = (UCHAR*)((ULONG)(resp) + (align - misalign));

	pResp[1] = (UCHAR)tran->proto.cmd;
	pResp[2] = status;
	
	MWUSB_Write(pResp, MAX_CMD_LEN_BYBYTE, BULK_WRITE_FIFO);

	BF_Debug_Print_Information(("usbFuncAck(), cmd %x\n",tran->proto.cmd ));
}


VOID Usb_Cmd_Process(INT32 stacd)
{
//	FilePathPkgHdr_s_t *pkg_hdr = NULL;
	TranMsg_t tran_msg;
//	Transfer *tran = NULL;
//	char dir_path[256];
//	char *pfiledata = NULL;
//	ULONG wlen;
	OS_USER_ER ercd;
//	INT32 rval;
//	int fileno;

	BF_Debug_Print_Information(("USB Command Process Task start!\n"));

	for( ;; )
	{
		ercd = BF_Mbox_rcv_blfmsg(MID_USB_PROCESS_REQ, MPFID_USB_PROCESS, &tran_msg, sizeof(tran_msg));
		if (ercd != D_OS_USER_E_OK) 
		{
			BF_Debug_Print_Error(("BF_Mbox_rcv_blfmsg MID_USB_PROCESS_REQ failed\n"));
			continue;
		}
//		tran = (Transfer*)tran_msg.tran_addr;
		switch (tran_msg.op) //pgr0712
		{
		    default:
		    break;
		}
	}
	
}



/**
 * Verify command parameters and data size.
 *
 * @param tran - Pointer to Transfer structure.
 */
VOID usbFuncVerifyParam(Transfer *tran)
{
	switch (tran->proto.cmd) 
	{
	    case CMD_SETPARAM:
	    case CMD_GETPARAM:
	    case CMD_GETPARAMLONG:
	    case CMD_MONITORCMD:
	    case CMD_SETMEMLOCATIONPTR:
	    case CMD_CONFIGMEMACCESS:
	    break;
	    
	    #if 0
	    case CMD_CRE_FILE:
	    	if (tran->proto.len > DOWNLOAD_RAM_SIZE)
	    		tran->proto.param = 0x00ff;
	    break;
	    case CMD_PROGRAM:
	    	if (tran->recv.data == 0 || tran->recv.ddr == 0 || tran->recv.nand == 0) {
	    		tran->proto.param = 0x00ff;
	    	} else {
	    		tran->firmware.blks = tran->firmware.len % (SM_Info.page_per_blk * 512) ?
	    		(USHORT)(tran->firmware.len / (SM_Info.page_per_blk * 512) + 1) :
	    		(USHORT)(tran->firmware.len / (SM_Info.page_per_blk * 512));
        
	    		if (tran->firmware.blks > CODE_BLK_SIZE) {
	    			BF_Debug_Print_Error(("nCodeBlock > MaxCodeBlks \r\n"));
	    			tran->proto.param = 0x00ff;
	    		}
	    	}
	    break;
	    #endif //0
	    
	    case CMD_POLL_STATUS:
	    	if (tran->poll_status.done == 0)
	    	    tran->proto.param = 0x00ff;
	    break;
	    default:
	    break;
	}
}


/**
 * Load data to DDR ram.
 *
 * @param tran - Pointer to Transfer structure.
 */
VOID usbFuncAction(Transfer* tran)
{
    UCHAR* pActionBuf = NULL;
    
    pActionBuf = (UCHAR*)malloc(MAX_PACKET_LEN_BYBYTE);
    if(pActionBuf != NULL && tran->proto.len != 0)
    {
        MWUSB_Read(pActionBuf, 0, BULK_READ_FIFO);
	}
	if (pActionBuf == NULL) {
		// error
		return;
	}
	
	switch(tran->proto.cmd)
	{
	    case CMD_SETPARAM:
	    {
	        FJ_ERR_CODE err;
	        SetParamInfo* pSetParamInfo = (SetParamInfo*)pActionBuf;
	        
	        pSetParamInfo->wParamID = (UINT16)convertValue((BYTE*)&(pSetParamInfo->wParamID),sizeof(pSetParamInfo->wParamID)); //pgr0520
	        pSetParamInfo->wValue   = (UINT16)convertValue((BYTE*)&(pSetParamInfo->wValue),sizeof(pSetParamInfo->wValue));

	        BF_Debug_Print_Information(( "CMD_SETPARAM wParamID %x \r\n", pSetParamInfo->wParamID));
	        BF_Debug_Print_Information(( "CMD_SETPARAM wValue %x \r\n", pSetParamInfo->wValue));

            
            err = (FJ_ERR_CODE)fj_setparam(pSetParamInfo->wParamID, pSetParamInfo->wValue);
            {
	            UCHAR resp[MAX_BUF_LEN];
	            UCHAR *pResp = NULL;
	            ULONG align = ALIGN_NUM;
	            ULONG misalign;
	            UCHAR status;

                if(FJ_ERR_OK==err)
                    status = FJ_ERR_OK;
                else
                    status = OP_FAILED;
                
	            memset(resp, 0x0, sizeof(resp));
	            pResp = resp;
	            misalign = (ULONG)(resp) & (align - 1);
	            if (misalign)
	            	pResp = (UCHAR*)((ULONG)(resp) + (align - misalign));
                
	            pResp[1] = (UCHAR)tran->proto.cmd;
	            pResp[2] = status;
	            
	            MWUSB_Write(pResp, MAX_CMD_LEN_BYBYTE, BULK_WRITE_FIFO);
            }                
                
	        BF_Debug_Print_Information(( "CMD_SETPARAM err %d \r\n", err));
	    }    
	    break;
	    case CMD_GETPARAM:
	    {
	        FJ_ERR_CODE err;
	        GetParamInfo* pGetParamInfo = (GetParamInfo*)pActionBuf;
	        USHORT wValue;
	        
	        BF_Debug_Print_Information(( "CMD_GETPARAM wParamID %x \r\n", pGetParamInfo->wParamID));//pgr0520

	        pGetParamInfo->wParamID = (USHORT)convertValue((BYTE*)&(pGetParamInfo->wParamID),sizeof(pGetParamInfo->wParamID));

	        BF_Debug_Print_Information(( "CMD_GETPARAM wParamID %x \r\n", pGetParamInfo->wParamID));
            
            err = (FJ_ERR_CODE)fj_getparam(pGetParamInfo->wParamID, &wValue);
            BF_Debug_Print_Information(( "CMD_GETPARAM wValue %x \r\n", wValue));
            {
	            UCHAR resp[MAX_BUF_LEN];
	            UCHAR *pResp = NULL;
	            ULONG align = ALIGN_NUM;
	            ULONG misalign;
	            UCHAR status;

                if(FJ_ERR_OK==err)
                    status = FJ_ERR_OK;
                else
                    status = OP_FAILED;
                
	            memset(resp, 0x0, sizeof(resp));
	            pResp = resp;
	            misalign = (ULONG)(resp) & (align - 1);
	            if (misalign)
	            	pResp = (UCHAR*)((ULONG)(resp) + (align - misalign));
                
	            pResp[1] = (UCHAR)tran->proto.cmd;
	            pResp[2] = status;
	            
	            MWUSB_Write(pResp, MAX_CMD_LEN_BYBYTE, BULK_WRITE_FIFO);
	            
	            memset(resp, 0x0, sizeof(resp));
	            ((GetParamInfo*)pResp)->wValue = wValue;
	            MWUSB_Write(pResp, sizeof(GetParamInfo), BULK_WRITE_FIFO);
            }
                
	        BF_Debug_Print_Information(( "CMD_GETPARAM err %d \r\n", err));
	    }
	    break;
	    case CMD_GETPARAMLONG:
	    {
	        FJ_ERR_CODE err;
	        GetParamLongInfo* pGetParamLongInfo = (GetParamLongInfo*)pActionBuf;
	        LONG value;
	        
	        BF_Debug_Print_Information(( "CMD_GETPARAMLONG ParamID %x \r\n", pGetParamLongInfo->ParamID));//pgr0520

	        pGetParamLongInfo->ParamID = (INT32)convertValue((BYTE*)&(pGetParamLongInfo->ParamID),sizeof(pGetParamLongInfo->ParamID));

	        BF_Debug_Print_Information(( "CMD_GETPARAMLONG ParamID %x \r\n", pGetParamLongInfo->ParamID));
            
            err = (FJ_ERR_CODE)fj_getparamlong(pGetParamLongInfo->ParamID, &value);
            BF_Debug_Print_Information(( "CMD_GETPARAMLONG wValue %x \r\n", value));
            {
	            UCHAR resp[MAX_BUF_LEN];
	            UCHAR *pResp = NULL;
	            ULONG align = ALIGN_NUM;
	            ULONG misalign;
	            UCHAR status;

                if(FJ_ERR_OK==err)
                    status = FJ_ERR_OK;
                else
                    status = OP_FAILED;
                
	            memset(resp, 0x0, sizeof(resp));
	            pResp = resp;
	            misalign = (ULONG)(resp) & (align - 1);
	            if (misalign)
	            	pResp = (UCHAR*)((ULONG)(resp) + (align - misalign));
                
	            pResp[1] = (UCHAR)tran->proto.cmd;
	            pResp[2] = status;
	            
	            MWUSB_Write(pResp, MAX_CMD_LEN_BYBYTE, BULK_WRITE_FIFO);
	            
	            memset(resp, 0x0, sizeof(resp));
	            ((GetParamLongInfo*)pResp)->value = value;
	            MWUSB_Write(pResp, sizeof(GetParamLongInfo), BULK_WRITE_FIFO);
            }
                
	        BF_Debug_Print_Information(( "CMD_GETPARAMLONG err %d \r\n", err));
	    }
	    break;
	    case CMD_MONITORCMD:
	    {
	        FJ_ERR_CODE err = FJ_ERR_NG;
	        PBYTE pCmd = (PBYTE)pActionBuf;
	        USHORT cmdLength = strlen((const char *)pCmd);//pgr0520
	        
	        BF_Debug_Print_Information(( "CMD_MONITORCMD cmdLength %d \r\n", cmdLength));
	        BF_Debug_Print_Information(( "CMD_MONITORCMD pCmd %s \r\n", pCmd));
            
            {
	            UCHAR resp[MAX_BUF_LEN];
	            UCHAR *pResp = NULL;
	            ULONG align = ALIGN_NUM;
	            ULONG misalign;
	            UCHAR status;

                if(FJ_ERR_OK==err)
                    status = FJ_ERR_OK;
                else
                    status = OP_FAILED;
	    
	            memset(resp, 0x0, sizeof(resp));
	            pResp = resp;
	            misalign = (ULONG)(resp) & (align - 1);
	            if (misalign)
	            	pResp = (UCHAR*)((ULONG)(resp) + (align - misalign));
                
	            pResp[1] = (UCHAR)tran->proto.cmd;
	            pResp[2] = status;
	            
	            MWUSB_Write(pResp, MAX_CMD_LEN_BYBYTE, BULK_WRITE_FIFO);
            }                
	        
	    }
	    break;
	    case CMD_SETMEMLOCATIONPTR:
	    {
	        FJ_ERR_CODE err;
	        SetMemLocationPtrInfo* pSetMemLocationPtrInfo = (SetMemLocationPtrInfo*)pActionBuf;
	        //LONG value;
	        UCHAR* pBuffer = NULL;
	        
	        BF_Debug_Print_Information(( "CMD_SETMEMLOCATIONPTR wLocation %x \r\n", pSetMemLocationPtrInfo->wLocation));//pgr0520
	        BF_Debug_Print_Information(( "CMD_SETMEMLOCATIONPTR size %x \r\n", pSetMemLocationPtrInfo->size));

	        pSetMemLocationPtrInfo->wLocation = (FJ_MEMORY_LOCATION)convertValue((BYTE*)&(pSetMemLocationPtrInfo->wLocation),
	                                                                                      sizeof(pSetMemLocationPtrInfo->wLocation));
	        pSetMemLocationPtrInfo->size = (INT32)convertValue((BYTE*)&(pSetMemLocationPtrInfo->size),
	                                                                    sizeof(pSetMemLocationPtrInfo->size));

	        BF_Debug_Print_Information(( "CMD_SETMEMLOCATIONPTR wLocation %x \r\n", pSetMemLocationPtrInfo->wLocation));
	        BF_Debug_Print_Information(( "CMD_SETMEMLOCATIONPTR size %x \r\n", pSetMemLocationPtrInfo->size));
            
//          err = (FJ_ERR_CODE)fj_setmemlocationptr(pSetMemLocationPtrInfo->wLocation, &pBuffer, pSetMemLocationPtrInfo->size);
			err = FJ_GetMem( FJ_HOST_GENERIC_MEM_LOCATION, (UINT32 *)pBuffer, &pSetMemLocationPtrInfo->size );
            BF_Debug_Print_Information(( "CMD_SETMEMLOCATIONPTR pBuffer %x \r\n", pBuffer));
            {
	            UCHAR resp[MAX_BUF_LEN];
	            UCHAR *pResp = NULL;
	            ULONG align = ALIGN_NUM;
	            ULONG misalign;
	            UCHAR status;

                if(FJ_ERR_OK==err)
                    status = FJ_ERR_OK;
                else
                    status = OP_FAILED;
                
	            memset(resp, 0x0, sizeof(resp));
	            pResp = resp;
	            misalign = (ULONG)(resp) & (align - 1);
	            if (misalign)
	            	pResp = (UCHAR*)((ULONG)(resp) + (align - misalign));
                
	            pResp[1] = (UCHAR)tran->proto.cmd;
	            pResp[2] = status;
	            
	            MWUSB_Write(pResp, MAX_CMD_LEN_BYBYTE, BULK_WRITE_FIFO);
	            
	            memset(resp, 0x0, sizeof(resp));
	            ((SetMemLocationPtrInfo*)pResp)->bufferAddr = (LONG)pBuffer;
	            MWUSB_Write(pResp, sizeof(SetMemLocationPtrInfo), BULK_WRITE_FIFO);
            }
                
	        BF_Debug_Print_Information(( "CMD_SETMEMLOCATIONPTR err %d \r\n", err));
	    }
	    break;
		case CMD_GETLONG:
	    {
	        FJ_ERR_CODE err;
	        PBYTE pByte = NULL;
            USHORT nKB;
            GetLongInfo* pGetLongInfo = (GetLongInfo*)pActionBuf;
	        
	        BF_Debug_Print_Information(( "CMD_GETLONG sizeByByte %x \r\n", pGetLongInfo->sizeByByte));//pgr0520
	        
	        pGetLongInfo->sizeByByte = (USHORT)convertValue((BYTE*)&(pGetLongInfo->sizeByByte),sizeof(pGetLongInfo->sizeByByte));
            
            BF_Debug_Print_Information(( "CMD_GETLONG sizeByByte %x \r\n", pGetLongInfo->sizeByByte));
            
            if(pGetLongInfo->sizeByByte<MAX_PACKET_LEN_BYBYTE)
                nKB = 1;
            else
                nKB = (pGetLongInfo->sizeByByte%MAX_PACKET_LEN_BYBYTE) ? 
                      (pGetLongInfo->sizeByByte/MAX_PACKET_LEN_BYBYTE+1) : 
                      (pGetLongInfo->sizeByByte/MAX_PACKET_LEN_BYBYTE);
            
            BF_Debug_Print_Information(( "CMD_GETLONG nKB %d \r\n", nKB));

	        pByte = (PBYTE)malloc(nKB*MAX_PACKET_LEN_BYBYTE);
	        if(!pByte)
            {
                BF_Debug_Print_Error(("CMD_GETLONG allocating memory fail! \r\n"));
                err = FJ_ERR_NG;
            }
            else
            {   
                memset(pByte, 0x0, nKB*MAX_PACKET_LEN_BYBYTE);
                err = FJ_GetLong(pByte, pGetLongInfo->sizeByByte);
            }
            {
	            UCHAR resp[MAX_BUF_LEN];
	            UCHAR *pResp = NULL;
	            ULONG align = ALIGN_NUM;
	            ULONG misalign;
	            UCHAR status;

                if(FJ_ERR_OK==err)
                    status = FJ_ERR_OK;
                else
                    status = OP_FAILED;
                
	            memset(resp, 0x0, sizeof(resp));
	            pResp = resp;
	            misalign = (ULONG)(resp) & (align - 1);
	            if (misalign)
	            	pResp = (UCHAR*)((ULONG)(resp) + (align - misalign));
                
	            pResp[1] = (UCHAR)tran->proto.cmd;
	            pResp[2] = status;
	            
	            MWUSB_Write(pResp, MAX_CMD_LEN_BYBYTE, BULK_WRITE_FIFO);
	            
	            if(FJ_ERR_OK==err)
                {
	                USHORT idx;
                    for(idx=0; idx < nKB; idx++) 
                    {
                        MWUSB_Write(pByte + idx*MAX_PACKET_LEN_BYBYTE, MAX_PACKET_LEN_BYBYTE, BULK_WRITE_FIFO);//pgr0520
                    }
                    
	            }
	            
	            if(pByte)
	                FREE(pByte);
            }
                
	        BF_Debug_Print_Information(( "CMD_GETLONG err %d \r\n", err));
	    }
	    break;
	    case CMD_SETLONG:
	    {
	        FJ_ERR_CODE err;
	        PBYTE pByte = NULL;
            USHORT nKB;
            SetLongInfo* pSetLongInfo = (SetLongInfo*)pActionBuf;
	        
	        BF_Debug_Print_Information(( "CMD_SETLONG sizeByByte %x \r\n", pSetLongInfo->sizeByByte));//pgr0520
	        
	        pSetLongInfo->sizeByByte = (USHORT)convertValue((BYTE*)&(pSetLongInfo->sizeByByte),sizeof(pSetLongInfo->sizeByByte));
            
            BF_Debug_Print_Information(( "CMD_SETLONG sizeByByte %x \r\n", pSetLongInfo->sizeByByte));
	        
            if(pSetLongInfo->sizeByByte<MAX_PACKET_LEN_BYBYTE)
                nKB = 1;
            else
                nKB = (pSetLongInfo->sizeByByte%MAX_PACKET_LEN_BYBYTE) ? 
                      (pSetLongInfo->sizeByByte/MAX_PACKET_LEN_BYBYTE+1) : 
                      (pSetLongInfo->sizeByByte/MAX_PACKET_LEN_BYBYTE);

	        BF_Debug_Print_Information(( "CMD_SETLONG nKB %d \r\n", nKB));

	        pByte = (PBYTE)malloc(nKB*MAX_PACKET_LEN_BYBYTE);
	        if(!pByte)
            {
                BF_Debug_Print_Error(("CMD_SETLONG allocating memory fail! \r\n"));
                err = FJ_ERR_NG;
            }
            else
            {   
	            USHORT idx;
                
                memset(pByte, 0x0, nKB*MAX_PACKET_LEN_BYBYTE);
                
                for(idx=0; idx < nKB; idx++) 
                {
                    MWUSB_Read((UCHAR*)(pByte + idx*MAX_PACKET_LEN_BYBYTE), 0, BULK_READ_FIFO);
                }
                
                err = FJ_SetLong(pByte, pSetLongInfo->sizeByByte);
                
                FREE(pByte);
            }
            {
	            UCHAR resp[MAX_BUF_LEN];
	            UCHAR *pResp = NULL;
	            ULONG align = ALIGN_NUM;
	            ULONG misalign;
	            UCHAR status;

                if(FJ_ERR_OK==err)
                    status = FJ_ERR_OK;
                else
                    status = OP_FAILED;
                
	            memset(resp, 0x0, sizeof(resp));
	            pResp = resp;
	            misalign = (ULONG)(resp) & (align - 1);
	            if (misalign)
	            	pResp = (UCHAR*)((ULONG)(resp) + (align - misalign));
                
	            pResp[1] = (UCHAR)tran->proto.cmd;
	            pResp[2] = status;
	            
	            MWUSB_Write(pResp, MAX_CMD_LEN_BYBYTE, BULK_WRITE_FIFO);
            }
                
	        BF_Debug_Print_Information(( "CMD_SETLONG err %d \r\n", err));
	    }
	    break;

	    default:
	    break;
	}
	
	FREE(pActionBuf);

	memset(tran, 0x0 , sizeof(Transfer));

}


INT32 convertValue(BYTE* p_value, INT32 sizeByByte)
{
    INT32 shiftBit[]={0,8,16,24};
    //INT32 shiftBitNum = sizeByByte;
    INT32 retValue = 0;
    INT32 idx;
    
    if(sizeByByte>(sizeof(shiftBit)/sizeof(INT32)))
    {
        BF_Debug_Print_Error(( "convertValue() too many byte %d \r\n", sizeByByte));
        
        return (retValue);
    }
    
    for(idx=0;idx<sizeByByte;idx++)
    {
		INT32 temp = (*(p_value+idx))<<(shiftBit[idx]);
        retValue = retValue | temp;
    }
    
    return (retValue);
}


FJ_ERR_CODE FJ_GetLong(PBYTE pBuffer, UINT32 sizeByByte)
{
    FJ_ERR_CODE err;
    LONG bufAddress;
    
    if(!pBuffer)
    {
        BF_Debug_Print_Error(("FJ_GetLong(), invaild buffer pointer! \r\n"));
        return (FJ_ERR_NG);
    }
    if(sizeByByte>CHIP_TEST_RESERVED_SIZE)
    {
        BF_Debug_Print_Error(("FJ_GetLong(), over the reserved memory size %d! \r\n", sizeByByte));
        return (FJ_ERR_NG);
    }
    
    err = (FJ_ERR_CODE)fj_getparamlong(FJ_PARA_CHIPTEST_ADDR, &bufAddress);
    if(FJ_ERR_NG==err)
    {
        BF_Debug_Print_Error(("FJ_GetLong(), fj_getparamlong fail! \r\n"));
        return (err);
    }
    
    memcpy(pBuffer, (CHAR*)bufAddress, sizeByByte);
    
    return (err);
}


FJ_ERR_CODE FJ_SetLong(PBYTE pBuffer, UINT32 sizeByByte)
{
    FJ_ERR_CODE err;
    LONG bufAddress;
    
    if(!pBuffer)
    {
        BF_Debug_Print_Error(("FJ_SetLong(), invaild buffer pointer! \r\n"));
        return (FJ_ERR_NG);
    }
    if(sizeByByte>CHIP_TEST_RESERVED_SIZE)
    {
        BF_Debug_Print_Error(("FJ_SetLong(), over the reserved memory size %d! \r\n", sizeByByte));
        return (FJ_ERR_NG);
    }
    
    err = (FJ_ERR_CODE)fj_getparamlong(FJ_PARA_CHIPTEST_ADDR, &bufAddress);

    if(FJ_ERR_NG==err)
    {
        BF_Debug_Print_Error(("FJ_SetLong(), fj_getparamlong fail! \r\n"));
        return (err);
    }
    
    memcpy((CHAR*)bufAddress, pBuffer, sizeByByte);
    
    return (err);
}
