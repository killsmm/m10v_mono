/**
 * @file		user_task.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

//#pragma GCC optimize ("O0")

#include "user.h"
#include "dd_arm.h"
#include "TKernel_id.h"
#include "user_streaming.h"
/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define D_USER_MBX_MAX	(256)


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	OS_USER_VP		_os_reserve[2];	// OS reserve
	ULONG			event;
	ULLONG			frame_no;
} T_USER_MBX;

typedef struct {
	UINT32			head;
	UINT32			num;
	T_USER_MBX*	data[D_USER_MBX_MAX];
} T_USER_MBX_DTQ;

typedef E_USER_STATE (*T_USER_FUNC)( E_USER_EVENT event, ULLONG frame_no );

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static volatile T_USER_MBX		g_user_mbx[D_USER_MBX_MAX] __attribute__((section(".LOCK_SECTION"), aligned(64)));
static volatile T_USER_MBX_DTQ	g_user_mbx_empty_dtq __attribute__((section(".LOCK_SECTION"), aligned(64)));
static volatile ULONG			g_user_spin_lock	__attribute__((section(".LOCK_SECTION"), aligned(64)));

static T_USER_FUNC	g_user_func_cur = NULL;
static T_USER_FUNC	g_user_func_next = NULL;
static E_USER_EVENT	g_user_event_next = E_USER_EVENT_NONE;
static ULLONG		g_user_frame_no_next = 0;
/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static void rcv_event( E_USER_EVENT* event, ULLONG* frame_no );
static INT32 enqueue( T_USER_MBX* data );
static INT32 dequeue( T_USER_MBX** data );


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void User_Task(INT32 stacd)
{
	E_USER_STATE	state = E_USER_STATE_IDLE;
	E_USER_EVENT	event = E_USER_EVENT_NONE;
	ULLONG			frame_no = 0;
	INT32			i;
	
	g_user_mbx_empty_dtq.head = 0;
	g_user_mbx_empty_dtq.num = 0;
	memset( (void*)g_user_mbx, 0, sizeof(g_user_mbx) );
	for( i = 0; i < D_USER_MBX_MAX; i ++ ){
		enqueue( (T_USER_MBX*)&g_user_mbx[i] );
	}
	
	User_Boot();
	
	while(1)
	{
		rcv_event( &event, &frame_no );
		
		// change func
		switch( event )
		{
			case E_USER_EVENT_VIEW_START:
				if( g_user_func_cur == NULL ){
					UPRINTF(" User View\n");
					g_user_func_cur   = User_View;
					g_user_func_next  = NULL;
					g_user_event_next = E_USER_EVENT_NONE;
				}else{
					UPRINTF(" User Next is View\n");
					g_user_func_next  = User_View;
					g_user_event_next = event;
					g_user_frame_no_next = frame_no;
					event = E_USER_EVENT_STANDBY; // stop exec
				}
			break;

			case E_USER_EVENT_SIMPLE_VIEW_START:
				if( g_user_func_cur == NULL ){
					UPRINTF(" User Simple View\n");
					g_user_func_cur   = User_Simple_View;
					g_user_func_next  = NULL;
					g_user_event_next = E_USER_EVENT_NONE;
				}else{
					UPRINTF(" User Next is Simple View\n");
					g_user_func_next  = User_Simple_View;
					g_user_event_next = event;
					g_user_frame_no_next = frame_no;
					event = E_USER_EVENT_STANDBY; // stop exec
				}
			break;
			
			case E_USER_EVENT_YUV_IN_VIEW_START:
				if( g_user_func_cur == NULL ){
					UPRINTF(" User YUVin View\n");
					g_user_func_cur   = User_YUVin_View;
					g_user_func_next  = NULL;
					g_user_event_next = E_USER_EVENT_NONE;
				}else{
					UPRINTF(" User Next is YUVin View\n");
					g_user_func_next  = User_YUVin_View;
					g_user_event_next = event;
					g_user_frame_no_next = frame_no;
					event = E_USER_EVENT_STANDBY; // stop exec
				}
			break;

			case E_USER_EVENT_CAPTURE_START:
			case E_USER_EVENT_CAPTURE_START_JPG_RAW_YUV:
				if( g_user_func_cur == NULL ){
					UPRINTF(" User Capture\n");
					g_user_func_cur   = User_Capture;
					g_user_func_next  = NULL;
					g_user_event_next = E_USER_EVENT_NONE;
				}else{
					UPRINTF(" User Next is Capture\n");
					g_user_func_next  = User_Capture;
					g_user_event_next = event;
					g_user_frame_no_next = frame_no;
					event = E_USER_EVENT_STANDBY; // stop exec
				}
			break;

			case E_USER_EVENT_PLAY_START:
				if( g_user_func_cur == NULL ){
					UPRINTF(" User Play\n");
					g_user_func_cur   = User_Play;
					g_user_func_next  = NULL;
					g_user_event_next = E_USER_EVENT_NONE;
				}else{
					UPRINTF(" User Next is Play\n");
					g_user_func_next  = User_Play;
					g_user_event_next = event;
					g_user_frame_no_next = frame_no;
					event = E_USER_EVENT_STANDBY; // stop exec
				}
			break;

			case E_USER_EVENT_STREAMING:
				if( g_user_func_cur == NULL ){
					UPRINTF(" User Streaming\n");
					g_user_func_cur   = User_Streaming;
					g_user_func_next  = NULL;
					g_user_event_next = E_USER_EVENT_NONE;
				}else{
					UPRINTF(" User Next is Streaming\n");
					g_user_func_next  = User_Streaming;
					g_user_event_next = event;
					g_user_frame_no_next = frame_no;
					event = E_USER_EVENT_STANDBY; // stop exec
				}
			break;

			default:
			break;
		}

		// run func
		if( g_user_func_cur ){
			state = g_user_func_cur( event, frame_no );
			
			// check state
			if( state == E_USER_STATE_IDLE ){
				if( g_user_func_next == NULL ){
					// standby
					UPRINTF(" User Standby\n");
					g_user_func_cur  = NULL;
					g_user_func_next = NULL;
					g_user_event_next = E_USER_EVENT_NONE;
				}else{
					// next
					UPRINTF(" User Next\n");
					g_user_func_cur   = g_user_func_next;
					g_user_func_next  = NULL;
					state = g_user_func_cur( g_user_event_next, g_user_frame_no_next );
					g_user_event_next = E_USER_EVENT_NONE;
				}
			}
		}
	}
}


INT32 Snd_Event_to_User( E_USER_EVENT event, ULLONG frame_no )
{
	OS_USER_ER		ercd = 0;
	T_USER_MBX*	mbx;
	
	// Get Memory
	Dd_ARM_Critical_Section_Start( g_user_spin_lock );
	ercd = dequeue( &mbx );
	Dd_ARM_Critical_Section_End( g_user_spin_lock );
	
	// Error Check
	if( ercd ){
		UPRINTF_ERR(ercd);
		return ercd;
	}

	// Set Data
	mbx->event    = event;
	mbx->frame_no = frame_no;
	
	//UPRINTF("send event %d, %x, %x\n", (INT32)mbx->event, (INT32)mbx, (INT32)mbx->_os_reserve[0] );

	// Send Memory
	ercd = OS_User_Snd_Mbx( MID_USER, (T_OS_USER_MSG*)mbx );
	UPRINTF_ERR(ercd);
	return ercd;
}

BOOL User_Is_Standby_Mode(VOID)
{
	if( g_user_func_cur == NULL && g_user_func_next == NULL && g_user_event_next == E_USER_EVENT_NONE ) {
		return TRUE;
	} else {
		return FALSE;
	}
}
/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static void rcv_event( E_USER_EVENT* event, ULLONG* frame_no )
{
	OS_USER_ER		ercd;
	T_USER_MBX*	mbx;
	
	// Get Memory
	ercd = OS_User_Rcv_Mbx( MID_USER, (T_OS_USER_MSG**)&mbx );
	if( ercd ){
		UPRINTF_ERR(ercd);
		*event    = E_USER_EVENT_NONE;
		*frame_no = 0;
		return;
	}

	// Set from Memory
	*event    = mbx->event;
	*frame_no = mbx->frame_no;

	//UPRINTF("receive event %d, addr %x, next %x\n", (INT32)mbx->event, (INT32)(mbx), (INT32)mbx->_os_reserve[0]);

	// Update Memory
	Dd_ARM_Critical_Section_Start( g_user_spin_lock );
	enqueue( (T_USER_MBX*)mbx );
	Dd_ARM_Critical_Section_End( g_user_spin_lock );
}


static INT32 enqueue( T_USER_MBX* data )
{
	if( g_user_mbx_empty_dtq.num < D_USER_MBX_MAX ){
		g_user_mbx_empty_dtq.data[(g_user_mbx_empty_dtq.head+g_user_mbx_empty_dtq.num)%D_USER_MBX_MAX] = data;
		g_user_mbx_empty_dtq.num++;
		return D_DDIM_USER_E_OK;
	}
	else{
		return D_DDIM_USER_E_SYS; // full
	}
}


static INT32 dequeue( T_USER_MBX** data )
{
	if( g_user_mbx_empty_dtq.num > 0 ){
		*data = g_user_mbx_empty_dtq.data[g_user_mbx_empty_dtq.head];
		g_user_mbx_empty_dtq.head = (g_user_mbx_empty_dtq.head+1)%D_USER_MBX_MAX;
		g_user_mbx_empty_dtq.num--;
		return D_DDIM_USER_E_OK;
	}
	else{
		return D_DDIM_USER_E_QOVR; // empty
	}
}



