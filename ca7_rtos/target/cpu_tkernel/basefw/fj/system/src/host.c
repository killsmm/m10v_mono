/**
 * @file		host.c
 * @brief		host process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "ddim_typedef.h"
#include "os_user_custom.h"
#include "fj_message.h"
#include "fj_mode.h"
#include "fj_std.h"
#include "fj_custom.h"
#include "fj_param.h"
#include "interrupt.h"
#if 0	// Unnecessary
#include "LCDCtrl.h"	 // It changes timing of D-LCD Initialzation.
#endif
#include "debug.h"
#include "dd_arm.h"


/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define	D_BF_HOST_BLOCK_CNT_MAX		(80)
#if 1
#define FJ_HM_BUTTON	(0x00400000) /**< HOST message of button ID */
#endif

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/**
  * Local Processing for  Host Message
  *
  */
typedef struct {
	ULONG msg_id;
	ULONG msg_param;
} T_BF_HOST_MESSGE_INFO;

typedef struct {
	INT32 read_pos;
	INT32 write_pos;
	INT32 count;
} T_BF_HOST_RING_BUFFER_CTRL;

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
static T_BF_HOST_MESSGE_INFO gBF_Host_Message_Table[ NUMBER_OF_HOST_MESSAGE_BUFFER];
static OS_USER_ID gBF_Host_Sem_id;
static int gBF_Host_Lock_Key_Event = 0;
static int gBF_Host_Redirect_Key_Event = 0;
static T_BF_HOST_RING_BUFFER_CTRL gBF_Host_HM_Buf_Ctrl = { 0, 0, 0};

static UINT32 gBF_Host_Block_cnt = 0;
static ULONG gBF_Host_Block_Message[D_BF_HOST_BLOCK_CNT_MAX];
/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
static FJ_ERR_CODE host_msg_post(ULONG msg_id, ULONG msg_param )
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	
	if( gBF_Host_HM_Buf_Ctrl.count >= NUMBER_OF_HOST_MESSAGE_BUFFER){	// Buffer OVER
		BF_Debug_Print_Error(("Host Message Buffer OVER Flow!!\n"));
		return FJ_ERR_NG;
	}
	Dd_ARM_Critical_Section_Start( critseq );
	gBF_Host_Message_Table[gBF_Host_HM_Buf_Ctrl.write_pos ].msg_id = msg_id;
	gBF_Host_Message_Table[gBF_Host_HM_Buf_Ctrl.write_pos ].msg_param = msg_param;

	++gBF_Host_HM_Buf_Ctrl.write_pos;
	if( gBF_Host_HM_Buf_Ctrl.write_pos >= NUMBER_OF_HOST_MESSAGE_BUFFER){
		gBF_Host_HM_Buf_Ctrl.write_pos = 0;
	}
	++gBF_Host_HM_Buf_Ctrl.count;
	Dd_ARM_Critical_Section_End( critseq );

	return FJ_ERR_OK;
}

static FJ_ERR_CODE host_msg_get(ULONG* msg_id, ULONG* msg_param )
{
	static ULONG critseq __attribute__((section(".LOCK_SECTION"), aligned(64)));
	
	if( gBF_Host_HM_Buf_Ctrl.count <= 0){	 // No Message
		//	BF_Debug_Print_Error(("Host Message Buffer Under Flow!!\n"));
		return FJ_ERR_NG;
	}

	Dd_ARM_Critical_Section_Start( critseq );
	*msg_id = gBF_Host_Message_Table[gBF_Host_HM_Buf_Ctrl.read_pos ].msg_id;
	*msg_param = gBF_Host_Message_Table[gBF_Host_HM_Buf_Ctrl.read_pos ].msg_param;

	++gBF_Host_HM_Buf_Ctrl.read_pos;
	if( gBF_Host_HM_Buf_Ctrl.read_pos >= NUMBER_OF_HOST_MESSAGE_BUFFER){
		gBF_Host_HM_Buf_Ctrl.read_pos = 0;
	}
	--gBF_Host_HM_Buf_Ctrl.count;
	Dd_ARM_Critical_Section_End( critseq );

	return FJ_ERR_OK;
}

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
/**
 * @brief	Host Message IF initalize setting Semphore id
 * @param[in] sem_id
 * @return None
 * @note	None
 * @attention	None
*/
VOID BF_Host_Init_Message(OS_USER_ID sem_id)
{
	gBF_Host_Sem_id = sem_id;
}

/**
 *	@brief		Once some HM is blocked by this API, it will not be sent to HCE\n
 *				until it's unblocked (see PRMID_HOST_MSG_UNBLOCK).\n
 *				The HM that should be blocked has to be specified as a parameter.\n
 *	@param		i:message_id	Host Message ID
 *	@return		TRUE			Not entry message on Block Table
 *	@return		FALSE			entry message on Block Table
 *	@note		None
 *	@attention	None
 */
BOOL BF_Block_Message_Check( ULONG message_id )
{
	UINT32 loop_count;

	for( loop_count = 0; loop_count < D_BF_HOST_BLOCK_CNT_MAX; loop_count++ ){
		if( gBF_Host_Block_Message[loop_count] == 0 ){
			continue;
		}
		if( gBF_Host_Block_Message[loop_count] == message_id ){
			return FALSE;
		}
	}
	return TRUE;
}

/**
 *	@brief		FJI host should wait for the message using this function.\n
 *	@param		i/o:pulMessage	Message identification
 *	@param		i/o:pulParam	Message parameter(if any)
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID fj_hostgetmessage( ULONG* pulMessage, ULONG* pulParam )
{
	FJ_ERR_CODE ercd;
	if( (pulMessage == NULL ) || (pulParam == NULL) ){
		return;
	}

	/* Default setting. if no message, it will return 0 data. */
	*pulMessage = 0;
	*pulParam = 0;

	OS_User_Wai_Sem( gBF_Host_Sem_id);
	ercd = host_msg_get(pulMessage, pulParam);
	if(ercd != FJ_ERR_OK){
		//BF_Debug_Print_Error(("Get Host Message Error=%d\n", ercd ));
	}

	return;
}

/**
 *	@brief		FJ host should post the message to queue using this function.\n
 *	@param		i:ulMessage		Message identification
 *	@param		i:ulParam		Message parameter(if any)
 *	@return		None
 *	@note		None
 *	@attention	None
 */
VOID fj_hostpostmessage( ULONG ulMessage, ULONG ulParam )
{
	FJ_ERR_CODE ercd;

	if(gBF_Host_Lock_Key_Event && ulMessage == FJ_HM_BUTTON  && !gBF_Host_Redirect_Key_Event){
		//Normal key event is locked
		return;
	}
	if(gBF_Host_Redirect_Key_Event){
		gBF_Host_Redirect_Key_Event = 0;
	}

	/* Check Block Status */
	if( FALSE == BF_Block_Message_Check(ulMessage) ){
		/* Block Status. So, it is not send message to HOST */
		return;
	}

	ercd = host_msg_post(ulMessage, ulParam);
	if(ercd != FJ_ERR_OK){
		BF_Debug_Print_Error(("Post Host Message Error=%d\n", ercd ));
	}
	OS_User_Sig_Sem( gBF_Host_Sem_id);
	return;
}

/**
 *	@brief		Once some HM is blocked by this API, it will not be sent to HCE until\n
 *				it's unblocked (see PRMID_HOST_MSG_UNBLOCK).\n
 *				The HM that should be blocked has to be specified as a parameter.\n
 *	@param		i:wConfig		PRMID_HOST_MSG_BLOCK
 *	@param		i:wValue		Blocks HOST MSG (HM) Message Id
 *	@return		FJ_ERR_OK		Success
 *	@return		not FJ_ERR_OK		Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_confighostmessage( USHORT wConfig,  ULONG wValue )
{
	UINT32 loop_count;
	UINT32 exist_count = 0;

	if( wValue == 0 ){
		return FJ_ERR_NG_INPUT_PARAM;
	}
	if( gBF_Host_Block_cnt == 0 ){
		for( loop_count = 0; loop_count < D_BF_HOST_BLOCK_CNT_MAX; loop_count++ ){
			gBF_Host_Block_Message[loop_count] = 0;
		}
	}
	switch( wConfig ){
		case FJ_PARA_HOST_MSG_BLOCK:
			for( loop_count = 0; loop_count < D_BF_HOST_BLOCK_CNT_MAX; loop_count++ ){
				if( gBF_Host_Block_Message[loop_count] == 0 ){
					/* no entry */
					continue;
				}
				if( gBF_Host_Block_Message[loop_count] == wValue ){
					/* it already entry message as block object */
					return FJ_ERR_OK;
				}
				exist_count++;
				if( exist_count >= gBF_Host_Block_cnt ){
					break;
				}
			}
			/* entry Block Message */
			for( loop_count = 0; loop_count < D_BF_HOST_BLOCK_CNT_MAX; loop_count++ ){
				if( gBF_Host_Block_Message[loop_count] == 0 ){
					gBF_Host_Block_Message[loop_count] = wValue;
					gBF_Host_Block_cnt++;
					break;
				}
			}
			break;

		case FJ_PARA_HOST_MSG_UNBLOCK:
			for( loop_count = 0; loop_count < D_BF_HOST_BLOCK_CNT_MAX; loop_count++ ){
				if( gBF_Host_Block_Message[loop_count] == wValue ){
					gBF_Host_Block_Message[loop_count] = 0;
					gBF_Host_Block_cnt--;
					break;
				}
			}
			break;

		default:
			return FJ_ERR_NG_INPUT_PARAM;
	}
	return FJ_ERR_OK;
}
