/**
 *
 * @file		rs_dsp.c
 * @brief		DSP I/F
 * @author		H.Maeda (SNI)
 * @since		2014/07/02
 * @note		
 * @attention	
 * @Copyright	Copyright (C) 2015 SOCIONEXT INCORPORATED. All Rights Reserved.
 *
 */


#include <stdio.h>								// Std C lib
#include <string.h>								// Std C lib
#include <unistd.h>								// Std C lib

#include "../include/rs_dsp.h"					// DSP I/F

#include "../dsp_peri/include/dsp_peri.h"		// DSP Peripheral Driver

#include "fj_top.h"								// Clock
#include "fj_arm.h"								// Interrupt
#include "fj_memory.h"							// Data cache

#include "dd_arm.h"								// Interrupt

#include "os_user_custom.h"						// OS Wrapper



// #define CO_DSP_DBG_ON
// #define CO_DSP_DBG_TEST



/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_FALSE = 0,
	E_TRUE = 1
} E_BOOL;

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

// NULL
#ifndef NULL
#	define NULL	((VOID*)0x0)
#endif

#define D_RS_DSP_NUM_REQ		(6)
#define D_RS_DSP_STA_MBOX		(0)
#define D_RS_DSP_END_MBOX		(D_RS_DSP_STA_MBOX + D_RS_DSP_NUM_REQ)

#define D_RS_DSP_FLG_END		(0x1)
#define D_RS_DSP_FLG_ABORT		(0x2)

#define D_RS_DSP_ADR_VP_VINT	(0x1C0)

#define D_RS_DSP_FALSE	(0)
#define D_RS_DSP_TRUE	(1)


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

#define M_GET_PROC_ID( data )				( (UINT16)(data)       )
#define M_GET_SEND_ID( data )				( (UINT16)(data >> 16) )

#define M_GET_MSG_HEADER( sndID, procID )	( ((sndID) << 16) | (procID) )

#define M_GET_FLG_BIT_POS( sndID )			( sndID << 1 )

#define M_UPDATE_RING_IDX( index, max )		{ if ( (++index) >= max  ) { index = 0; } }

#ifdef CO_DSP_DBG_ON
#	define M_DBG_PRINTF( arg )				printf arg
#else
#	define M_DBG_PRINTF( arg )
#endif


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

typedef struct {
	E_BOOL			isLocked;
	UINT16			sndID;
	T_RS_DSP_MSG	msg;
} T_RS_DSP_MSG_MNG;


/*----------------------------------------------------------------------*/
/* Prototype															*/
/*----------------------------------------------------------------------*/

static VOID rs_dsp_Proc_Reply( T_RS_DSP_MSG *rcvMsg );
static VOID rs_dsp_Proc_DebugPrint( T_RS_DSP_MSG *rcvMsg );


/*----------------------------------------------------------------------*/
/* Global variables														*/
/*----------------------------------------------------------------------*/

static volatile INT32 				gRsDsp_SndID;

static volatile T_RS_DSP_MSG_MNG	gRsDsp_ReplyMsg[D_RS_DSP_NUM_REQ];

static VOID	(*gRsDsp_FuncTbl[D_NUM_PROCID_CPU])( T_RS_DSP_MSG *rcvMsg ) = {
	rs_dsp_Proc_Reply,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	rs_dsp_Proc_DebugPrint
};


/*----------------------------------------------------------------------*/
/* Static Function														*/
/*----------------------------------------------------------------------*/

/**
  Get the reply message buffer.

  @param[out]	rcvMsg	: The beinning address of reply message buffer.
  @param[in]	sndID	: ID for sent message.
*/
static VOID rs_dsp_GetReplyMsgBuf( T_RS_DSP_MSG **rcvMsg, UINT16 sndID )
{
	INT32 	i;
//	ULONG	status = Dd_ARM_DI();
	
	
	for (i = 0; i < D_RS_DSP_NUM_REQ; i++) {
		if ((E_TRUE == gRsDsp_ReplyMsg[i].isLocked) &&
		    (sndID  == gRsDsp_ReplyMsg[i].sndID)) {
			*rcvMsg = (T_RS_DSP_MSG*)&gRsDsp_ReplyMsg[i].msg;
			M_DBG_PRINTF( ("RS_DSP: Get reply message buffer [idx=%d|addr=0x%x]\n", i, (*rcvMsg)) );
			break;
		}
	}
	
//	Dd_ARM_EI(status);
}


/**
  Lock the reply message buffer.

  @param[out]	rcvMsg	: The beinning address of reply message buffer.
  @param[in]	sndID	: ID for sent message.
  
  @remarks	Should be called at critical section.
            This function is called at only dsp peripheral ipcu interrupt handler.
*/
static VOID rs_dsp_LockReplyMsgBuf( T_RS_DSP_MSG **rcvMsg, UINT16 sndID )
{
	INT32 i;
	
	
	for (i = 0; i < D_RS_DSP_NUM_REQ; i++) {
		if (E_FALSE == gRsDsp_ReplyMsg[i].isLocked) {
			gRsDsp_ReplyMsg[i].isLocked = E_TRUE;
			gRsDsp_ReplyMsg[i].sndID = sndID;
			*rcvMsg = (T_RS_DSP_MSG*)&gRsDsp_ReplyMsg[i].msg;
			M_DBG_PRINTF( ("RS_DSP: Lock reply message buffer [idx=%d|addr=0x%x]\n", i, (*rcvMsg)) );
			return;
		}
	}
	
	*rcvMsg = NULL;
	
	M_DBG_PRINTF( ("RS_DSP: can not lock reply message buffer!!!\n") );
}


/**
  Unlock the reply message buffer.

  @param[in]	rcvMsg	: The beinning address of reply message buffer.
*/
static VOID rs_dsp_UnlockReplyMsgBuf( T_RS_DSP_MSG *rcvMsg )
{
	INT32 i;
//	ULONG	status = Dd_ARM_DI();
	
	
	for (i = 0; i < D_RS_DSP_NUM_REQ; i++) {
		if ((E_TRUE == gRsDsp_ReplyMsg[i].isLocked) &&
		    (rcvMsg == &gRsDsp_ReplyMsg[i].msg)) {
			gRsDsp_ReplyMsg[i].isLocked = E_FALSE;
			M_DBG_PRINTF( ("RS_DSP: Unlock reply message buffer [idx=%d|addr=0x%x]\n", i, (*rcvMsg)) );
			break;
		}
	}
	
//	Dd_ARM_EI(status);
}


/**
  Processing when CPU has detected DSP's reply.

  @param[in]     rcvMsg	: Received message.
*/
static VOID rs_dsp_Proc_Reply( T_RS_DSP_MSG *rcvMsg )
{
	UINT16			sndID;
	INT32			bitPos;
	T_RS_DSP_MSG	*replyMsg;
	
	
	// Get SEND ID.
	// NOTE: This ID is sent by CPU when CPU requested to DSP.
	sndID  = M_GET_SEND_ID( rcvMsg->data[1] );
	
	// Copy to reply message buffer
	rs_dsp_LockReplyMsgBuf(&replyMsg, sndID);
	memcpy(replyMsg, rcvMsg, sizeof(T_RS_DSP_MSG));
	
	// Set event-flag to cancel waiting.
	bitPos = M_GET_FLG_BIT_POS( sndID );
	
	OS_User_Set_Flg( FID_DSPIF_REQ, 
					 (D_RS_DSP_FLG_END << bitPos) );
}


/**
  Debug Print.

  @param[in]     rcvMsg	: Received message.
*/
static VOID rs_dsp_Proc_DebugPrint( T_RS_DSP_MSG *rcvMsg )
{
	CHAR 	*ptr = (CHAR*)rcvMsg->data[1];
	UINT32	size  = rcvMsg->data[2];
	
	FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN, (UINT32)ptr, size );
	
	write(0, ptr, size);
}


/**
  Get SEND ID for CPU.

  @param[out]	sndID	: SEND ID for CPU.
  
  @remarks	Should be called at critical section.
*/
static VOID rs_dsp_GetSndID( UINT16 *sndID )
{
	*sndID = gRsDsp_SndID;
	
	M_UPDATE_RING_IDX( gRsDsp_SndID, D_RS_DSP_NUM_REQ );
}


/**
  IPCU Request interrupt callback.

  @param[in]	intStatus : IPCU interrupt status.
*/
static VOID rs_dsp_reqCallback( unsigned int intStatus )
{
	E_IPCU_MBOX		mBox = (E_IPCU_MBOX)M_IPCU_GET_MBOX_IDX( intStatus );
	T_RS_DSP_MSG	rcvMsg;
	UINT16			procID;
	
	
	// Put received message to buffer.
	DspPeri_IPCU_Get_Data( mBox,
		                   0,
		                   D_RS_DSP_NUM_DATA,
						   rcvMsg.data );
	
	
	// Get the Processing ID from received message.
	procID = M_GET_PROC_ID( rcvMsg.data[0] );
	
	
	M_DBG_PRINTF( ("REQ INT PROC ID >>> %d\n", procID) );
	M_DBG_PRINTF( ("REQ INT DATA %d >>> %8xh\n", 0, rcvMsg.data[0]) );
	M_DBG_PRINTF( ("REQ INT DATA %d >>> %8xh\n", 1, rcvMsg.data[1]) );
	M_DBG_PRINTF( ("REQ INT DATA %d >>> %8xh\n", 2, rcvMsg.data[2]) );
	M_DBG_PRINTF( ("REQ INT DATA %d >>> %8xh\n", 3, rcvMsg.data[3]) );
	M_DBG_PRINTF( ("REQ INT DATA %d >>> %8xh\n", 4, rcvMsg.data[4]) );
	M_DBG_PRINTF( ("REQ INT DATA %d >>> %8xh\n", 5, rcvMsg.data[5]) );
	M_DBG_PRINTF( ("REQ INT DATA %d >>> %8xh\n", 6, rcvMsg.data[6]) );
	M_DBG_PRINTF( ("REQ INT DATA %d >>> %8xh\n", 7, rcvMsg.data[7]) );
	M_DBG_PRINTF( ("REQ INT DATA %d >>> %8xh\n", 8, rcvMsg.data[8]) );
	
	
	// Processing according to procID.
	if ( (procID < D_NUM_PROCID_CPU) &&
		 (gRsDsp_FuncTbl[procID] != NULL) ) {
		
		gRsDsp_FuncTbl[procID]( &rcvMsg );
		
	}
}


/**
  IPCU Acknowledge interrupt callback.

  @param[in]	intStatus : IPCU interrupt status.
*/
static VOID rs_dsp_ackCallback( unsigned int intStatus )
{
	E_IPCU_MBOX	mBox = (E_IPCU_MBOX)M_IPCU_GET_MBOX_IDX( intStatus );
	
	// Disable MailBox.
	DspPeri_IPCU_Disable_MBOX( (E_IPCU_MBOX)mBox );
}


/*----------------------------------------------------------------------*/
/* External Function													*/
/*----------------------------------------------------------------------*/

/**
  Configure Clock.
  Start DSP core.

  @param[in]	dspBootAddr : DSP core's reset vector.
  @param[in]	dspClk 		: Clock configuration value.
   								- D_RS_DSP_CLK_400MHz_DIV2
   								- D_RS_DSP_CLK_267MHz_DIV2
   								- D_RS_DSP_CLK_200MHz_DIV2
   								- D_RS_DSP_CLK_100MHz_DIV2
   								- D_RS_DSP_CLK_200MHz_DIV1
   								- D_RS_DSP_CLK_100MHz_DIV1

  @retval D_RS_DSP_OK         : Success.
  @retval D_RS_DSP_NG_BUSY    : Failure. HW is busy.
  @retval D_RS_DSP_NG_UNKNOWN : Failure. Unexpected error.
*/
INT32 RS_DSP_Start( UINT32 	dspBootAddr,
				    INT32 	dspClk )
{
	FJ_ERR_CODE			retval;
	FJ_TOP_DSP_CLKSEL	clkSel;
	unsigned int		divClk;
	UCHAR				counter_DSPCK = 0;
	UCHAR				counter_DSPAX = 0;
	const INT32			negate = 0;
	
	
	// Check the state of DSP's reset.
	if ( negate == FJ_Top_Get_Reset_State_DSP() ) {
		return D_RS_DSP_NG_BUSY;
	}
	
	
	// Initialization
	//   IPCU Callback
	DspPeri_IPCU_Set_Callback( D_IPCU_INT0_CPU,
							   rs_dsp_reqCallback,
							   rs_dsp_ackCallback );
	//   Event-flag
	OS_User_Clr_Flg( FID_DSPIF_REQ, 0x0 );
	
	//   Internal variables.
	{
		INT32 i;
		
		gRsDsp_SndID  = 0;
		for (i = 0; i < D_RS_DSP_NUM_REQ; i++) {
			gRsDsp_ReplyMsg[i].isLocked = E_FALSE;
		}
	}
	
	
	////////////////////////////////
	//
	// DSP Booting.
	//
	////////////////////////////////
	
	//   Start Clock relevant for DSP.
	retval  = FJ_Top_Start_Clock( FJ_TOP_CLOCK_NAME_DSPAX, 
							      &counter_DSPAX );
	retval |= FJ_Top_Start_Clock( FJ_TOP_CLOCK_NAME_DSPCK, 
							      &counter_DSPCK );
	
	//   Release reset of DSP Peripheral Block.
	DspPeri_Reset( D_DSPPERI_OFF );
	
	//   Power-up SRAM
	DspPeri_Ctrl_SramPower( D_DSPPERI_ON );
	
	//   Setup reset vector
	DspPeri_Ctrl_VINT_ADDR( dspBootAddr );
	
	//   Stop DSP clock
	retval |= FJ_Top_Stop_Clock( FJ_TOP_CLOCK_NAME_DSPCK, 
							     &counter_DSPCK );
	
	//   Select Freq.
	switch ( dspClk ) {
		case D_RS_DSP_CLK_400MHz_DIV2:
			divClk = D_DSPPERI_DSPCLK_DIV_2;
			clkSel = FJ_TOP_DSP_CLKSEL_400MHZ;
			break;
		case D_RS_DSP_CLK_266MHz_DIV2:
			divClk = D_DSPPERI_DSPCLK_DIV_2;
			clkSel = FJ_TOP_DSP_CLKSEL_266MHZ;
			break;
		case D_RS_DSP_CLK_200MHz_DIV2:
			divClk = D_DSPPERI_DSPCLK_DIV_2;
			clkSel = FJ_TOP_DSP_CLKSEL_200MHZ;
			break;
		case D_RS_DSP_CLK_100MHz_DIV2:
			divClk = D_DSPPERI_DSPCLK_DIV_2;
			clkSel = FJ_TOP_DSP_CLKSEL_100MHZ;
			break;
		case D_RS_DSP_CLK_200MHz_DIV1:
			divClk = D_DSPPERI_DSPCLK_DIV_1;
			clkSel = FJ_TOP_DSP_CLKSEL_200MHZ;
			break;
		case D_RS_DSP_CLK_100MHz_DIV1:
			divClk = D_DSPPERI_DSPCLK_DIV_1;
			clkSel = FJ_TOP_DSP_CLKSEL_100MHZ;
			break;
		default:
			return D_RS_DSP_NG_UNKNOWN;
	}
	
	//   Setup DSP's core clock.
	retval |= FJ_Top_Set_Clock_Select_DSP( clkSel );
	
	//   Setup DSP's bus clock.
	DspPeri_Ctrl_AxiClk( divClk );
	
	//   Enable Interrupt of DSP Peri's IPCU.
	// 	 note: This setting is done in following file.
	//		   target\if_tkernel\tk_usercustom\src\interrupt_set.c
//	retval |= FJ_GIC_Ctrl( 269, 						// INTID
//						   1, 							// Enable
//						   (29 << 3), 					// Priority
//						   E_FJ_GIC_TARGET_CPU_ID0 );	// CPU target
	
	//	 Relase reset of DSP core.
	if ( retval == FJ_ERR_OK ) {
		FJ_Top_Negate_Reset_DSP();
	}
	else {
		return D_RS_DSP_NG_UNKNOWN;
	}
	
	//   Restart DSP clock. => DSP is running.
	FJ_Top_Start_Clock( FJ_TOP_CLOCK_NAME_DSPCK,
					    &counter_DSPCK );
	
	
	return D_RS_DSP_OK;
}


/**
  Stop DSP core.
  
  @retval D_RS_DSP_OK         : Success.
*/
INT32 RS_DSP_Stop( VOID )
{
	int 			i;
	int				bitPos;
	unsigned int 	allAbort;
	unsigned int	status;
	UCHAR			counter_DSPCK = 0;
	UCHAR			counter_DSPAX = 0;
	const INT32		assert = 1;
	
	
	// Check the state of DSP's reset.
	if ( assert == FJ_Top_Get_Reset_State_DSP() ) {
		return D_RS_DSP_OK;
	}
	
	
	// Force Stop DSP using VP_VINT
	DspPeri_Ctrl_VINT_ADDR( D_RS_DSP_ADR_VP_VINT );
	DspPeri_Request_VINT();
	
	
	// Check the DSP's running status
	do {
		
		DspPeri_Get_PowerStatus( &status );
		
		if ( M_DSPPERI_BITMSK_IDLE_CORE( status ) &&
		     M_DSPPERI_BITMSK_IDLE_DSP(status)   ) {
			break;
		}
		
	} while (1);
	
	
	// Terminate DSP Block.
	//   Stop Clock
	FJ_Top_Stop_Clock( FJ_TOP_CLOCK_NAME_DSPAX, 
					   &counter_DSPAX );
	FJ_Top_Stop_Clock( FJ_TOP_CLOCK_NAME_DSPCK, 
					   &counter_DSPCK );
	
	//   Assert reset of DSP.
	FJ_Top_Assert_Reset_DSP();
	
	//   Power-off SRAM
	DspPeri_Ctrl_SramPower( D_DSPPERI_OFF );
	
	//   Assert reset of DSP Peri.
	DspPeri_Reset( D_DSPPERI_ON );
	
	
	// Create flag-pattern to cancel waiting.
	allAbort = 0;
	for ( i = 0; i < D_RS_DSP_NUM_REQ; i++ ) {
		bitPos    = M_GET_FLG_BIT_POS( i );
		allAbort |= ( D_RS_DSP_FLG_ABORT << bitPos );
	}
	
	// Cancel all wait process.
	OS_User_Set_Flg( FID_DSPIF_REQ, allAbort );
	
	
	return D_RS_DSP_OK;
}


/**
  Send message to DSP.

  @param[out]		sndID 	 : SEND ID.
  @param[in]		procID 	 : Processing ID for DSP.
  @param[in]		waitTime : Maximum waiting time of getting sending semaphore. [ms]
  @param[in,out]	sndMsg 	 : Sending message.
                               1st element is written in this API.
  
  @retval D_RS_DSP_OK         : Success.
  @retval D_RS_DSP_NG_UNKNOWN : Failure. Unexpected error.
  @retval D_RS_DSP_NG_OS      : Failure. Error at system-call.
  @retval D_RS_DSP_NG_TIMEOUT : Failure. Exceed maximum wating time.
*/
INT32 RS_DSP_SendMsg( UINT16		*sndID,
					  UINT16		procID,
					  INT32			waitTime,
					  T_RS_DSP_MSG 	*sndMsg )
{
	OS_USER_ER		ercd;
	int				retval;
	unsigned int	mBoxStatus;
	int				mBox;
	
	
	// Get semaphore for request.
	ercd = OS_User_Twai_Sem( SID_DSPIF_REQ,
							 waitTime );
	if ( D_OS_USER_E_OK != ercd ) {
		return ( D_OS_USER_E_TMOUT == ercd ) ? 
			D_RS_DSP_NG_TIMEOUT : D_RS_DSP_NG_OS;
	}
	
#ifdef CO_DSP_DBG_TEST
		{
			OS_USER_ID	tid;
			OS_User_Get_Tid(&tid);
			printf("GetReq [%d]\n", (INT32)tid);
		}
#endif
	
	// --- Start of Critical section
	{
		// Get semaphore of exclusion process.
		ercd = OS_User_Wai_Sem( SID_DSPIF_EXC );
		if ( D_OS_USER_E_OK != ercd ) {
			OS_User_Sig_Sem( SID_DSPIF_REQ );
			return D_RS_DSP_NG_OS;
		}
		
		//	Get the index of Free MailBox.
		DspPeri_IPCU_Get_MBOX_Status( &mBoxStatus );
		mBox         = D_RS_DSP_STA_MBOX;
		mBoxStatus >>= mBox;
		for ( ; mBox < D_RS_DSP_END_MBOX; mBox++ ) {
			if ( 0x0 == (mBoxStatus & 0x1) ) {
				break;
			}
			mBoxStatus >>= 1;
		}
		if( D_RS_DSP_END_MBOX <= mBox ) {
			OS_User_Sig_Sem( SID_DSPIF_EXC );
			OS_User_Sig_Sem( SID_DSPIF_REQ );
			return D_RS_DSP_NG_UNKNOWN;
		}
		
#ifdef CO_DSP_DBG_TEST
		{
			OS_USER_ID	tid;
			OS_User_Get_Tid(&tid);
			if ( tid == 64 ) {
				printf("DlyTsk [%d]\n", (INT32)tid);
				OS_User_Dly_Tsk( 1000 );
			}
		}
#endif
		
		//	Enable MailBox.
		retval = DspPeri_IPCU_Enable_MBOX( (E_IPCU_MBOX)mBox, 
										   D_IPCU_INT0_CPU, 
										   D_IPCU_INT4_DSP );
		if ( D_IPCU_OK != retval ) {
			OS_User_Sig_Sem( SID_DSPIF_EXC );
			OS_User_Sig_Sem( SID_DSPIF_REQ );
			return D_RS_DSP_NG_UNKNOWN;
		}
		
		//	Get SEND ID.
		rs_dsp_GetSndID( sndID );
		
#ifdef CO_DSP_DBG_TEST
		{
			OS_USER_ID	tid;
			OS_User_Get_Tid(&tid);
			printf("SID  %d [%d]\n", (*sndID), (INT32)tid);
		}
#endif
		
		// Release semaphore of exclusion process.
		OS_User_Sig_Sem( SID_DSPIF_EXC );
	}
	// --- End of Critical section
	
	
	// Set the header to message buffer.
	sndMsg->data[0] = M_GET_MSG_HEADER( (*sndID), procID );
	
	
	// Set message data to MailBox.
	DspPeri_IPCU_Set_Data( (E_IPCU_MBOX)mBox,
		                   0,
		                   D_RS_DSP_NUM_DATA,
		                   sndMsg->data );
	
	
	// Start message communication.
	//   CPU -> DSP
	DspPeri_IPCU_Send( (E_IPCU_MBOX)mBox,
					   D_IPCU_MODE_ACK );
	
	
	M_DBG_PRINTF( ("CPU SEND ID >>> %d\n", (*sndID)) );
	
	
	return D_RS_DSP_OK;
}


/**
  Wait message from DSP.

  @param[in]		sndID 	 : SEND ID.
  @param[in]		waitTime : Maximum waiting time of getting sending semaphore. [ms]
  @param[out]		rcvMsg 	 : Received message.
  
  @retval D_RS_DSP_OK         : Success.
  @retval D_RS_DSP_NG_ABORT   : Failure. Detect abort.
  @retval D_RS_DSP_NG_UNKNOWN : Failure. Unexpected error.
  @retval D_RS_DSP_NG_OS      : Failure. Error at system-call.
  @retval D_RS_DSP_NG_TIMEOUT : Failure. Exceed maximum wating time.
*/
INT32 RS_DSP_WaitMsg( UINT16		sndID,
					  INT32 		waitTime,
					  T_RS_DSP_MSG	*rcvMsg )
{
	INT32			bitPos;
	INT32			retval;
	OS_USER_ER		ercd;
	OS_USER_FLGPTN	endPtn, abtPtn;
	OS_USER_FLGPTN	waiPtn, flgPtn;
	
	
	// Initialize event-flag pattern.
	//
	// NOTE: The usage of event-flag's.
	//       [ D_RS_DSP_NUM_REQ -> 6 ]
	// 
	// Each "sndID" has 2bits of event-flag as follows:
	// 
	// | Sending ID (sndID) | ... |   5   |   4  |   3  |   2  |   1  |   0  |
	// -----------------------------------------------------------------------
	// | Event flag's bit   | ... | 11 10 | 9  8 | 7  6 | 5  4 | 3  2 | 1  0 |
	// 
	// even bit : Complete normally, that is Detect DSP's reply message.
	//  odd bit : Detect abort.
	// 
	// Sending ID is incremented and rotated. [ 0->1->2->3->4->5->0->1->... ]
	// 
	
	bitPos = M_GET_FLG_BIT_POS( sndID );
	endPtn = (D_RS_DSP_FLG_END   << bitPos);	// Complete normally
	abtPtn = (D_RS_DSP_FLG_ABORT << bitPos);	// Detect abort
	waiPtn = (endPtn | abtPtn);					// Wait pattern
	flgPtn = 0;
	
	
	// Wait for SEND ID.
	ercd = OS_User_Twai_Flg( FID_DSPIF_REQ, 
							 waiPtn, 
							 D_OS_USER_TWF_ORW, 
							 &flgPtn, 
							 waitTime );
	if ( D_OS_USER_E_OK == ercd ) {
		
		ercd = OS_User_Clr_Flg( FID_DSPIF_REQ, ~(flgPtn & waiPtn) );
		
		if ( D_OS_USER_E_OK == ercd ) {
			
			if ( endPtn & flgPtn ) {
				T_RS_DSP_MSG*	replyMsg = NULL;
				
				retval = D_RS_DSP_OK;
				
				rs_dsp_GetReplyMsgBuf(&replyMsg, sndID);
				
				if( rcvMsg != NULL ) {
					memcpy( rcvMsg, replyMsg, sizeof(T_RS_DSP_MSG) );
				}
				
				rs_dsp_UnlockReplyMsgBuf(replyMsg);
			}
			else if ( abtPtn & flgPtn ) {
				
				retval = D_RS_DSP_NG_ABORT;
				
			}
			else {
				
				retval = D_RS_DSP_NG_UNKNOWN;
				
			}
		}
		else {
			
			retval = D_RS_DSP_NG_OS;
			
		}
		
	}
	else {
		
		retval = ( D_OS_USER_E_TMOUT == ercd ) ? 
			D_RS_DSP_NG_TIMEOUT : D_RS_DSP_NG_OS;
		
	}
	
	
	// Release semaphore for request.
	OS_User_Sig_Sem( SID_DSPIF_REQ );
	
	
	return retval;
}
