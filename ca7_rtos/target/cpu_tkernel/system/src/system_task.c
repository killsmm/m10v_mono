/**
 * @file		system_task.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

//#pragma GCC optimize ("O0")

#include "system.h"
#include "mode_stm.h"
#include "os_user_custom.h"
#include "debug.h"

#include <string.h>

// for init
#include "sdram_map_manager.h"
#include "camera_config.h"
#include "mode.h"
#include "LT6911UX.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define D_SYSTEM_MBX_MAX	(256)

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
// change MPFID_SYSTEM_IDX if change size.
typedef struct {
	OS_USER_VP		_os_reserve[2];	// OS reserve
	ULONG			event;
	ULLONG			frame_no;
} T_SYSTEM_MBX;

typedef struct {
	UINT32			head;
	UINT32			num;
	T_SYSTEM_MBX*	data[D_SYSTEM_MBX_MAX];
} T_SYSTEM_MBX_DTQ;

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static volatile T_SYSTEM_MBX		g_system_mbx[D_SYSTEM_MBX_MAX] __attribute__((section(".LOCK_SECTION"), aligned(64)));
static volatile T_SYSTEM_MBX_DTQ	g_system_mbx_empty_dtq __attribute__((section(".LOCK_SECTION"), aligned(64)));
static volatile ULONG				g_system_spin_lock	__attribute__((section(".LOCK_SECTION"), aligned(64)));
static E_SYSTEM_VIEW_MODE g_view_mode = D_SENSOR_VIEW_MODE;
/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static void init(void);
static void rcv_event( E_SYSTEM_EVENT* event, ULLONG* frame_no );
static INT32 enqueue( T_SYSTEM_MBX* data );
static INT32 dequeue( T_SYSTEM_MBX** data );


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void System_Task(INT32 stacd)
{
	E_SYSTEM_EVENT	event = E_SYSTEM_EVENT_NONE;
	ULLONG			frame_no = 0;
	INT32			i;
	
	MPRINTF(( D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SYSTEM, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "System_Task Start"));
	
	g_system_mbx_empty_dtq.head = 0;
	g_system_mbx_empty_dtq.num = 0;
	memset( (void*)g_system_mbx, 0, sizeof(g_system_mbx) );
	for( i = 0; i < D_SYSTEM_MBX_MAX; i ++ ){
		enqueue( (T_SYSTEM_MBX*)&g_system_mbx[i] );
	}
	
	init();
	
	while(1)
	{
		rcv_event( &event, &frame_no );
		
		switch ( event )
		{
		case E_SYSTEM_EVENT_MODE_VIEW:
			Mode_View_Start();
			g_view_mode |= D_SENSOR_VIEW_MODE;
			break;

		case E_SYSTEM_EVENT_MODE_YUVIN_VIEW:
			Mode_YUVIN_View_Start();
			g_view_mode |= D_YUV_VIEW_MODE;
			break;
		
		case E_SYSTEM_EVENT_STOP_MODE:
			Mode_View_Stop();
			g_view_mode &= ~D_SENSOR_VIEW_MODE;
			break;
		
		case E_SYSTEM_EVENT_STOP_YUVIN_MODE:
			Mode_YUVIN_View_Stop();
			g_view_mode &= ~D_YUV_VIEW_MODE;
			break;

		case E_SYSTEM_EVENT_MODE_END:
			
			break;
		
		default:
			g_view_mode == D_SENSOR_VIEW_MODE ? Mode_View_Event( event, frame_no ) : Mode_YUVIN_View_Event( event, frame_no );
			break;
		}
		
	}
}


void Snd_Event_to_System( E_SYSTEM_EVENT event, ULLONG frame_no )
{
	OS_USER_ER		ercd;
	T_SYSTEM_MBX*	mbx;
	
	// Get Memory
	Dd_ARM_Critical_Section_Start( g_system_spin_lock );
	ercd = dequeue( &mbx );
	Dd_ARM_Critical_Section_End( g_system_spin_lock );
	
	// Error Check
	if( ercd ){
		UPRINTF_ERR(ercd);
		return;
	}
	
	// Set Data
	mbx->event    = event;
	mbx->frame_no = frame_no;
	
	//UPRINTF("send event %d, %x, %x\n", (INT32)mbx->event, (INT32)mbx, (INT32)mbx->_os_reserve[0] );
	
	// Send Memory
	ercd = OS_User_Snd_Mbx( MID_SYSTEM, (T_OS_USER_MSG*)mbx );
	UPRINTF_ERR(ercd);
}


void Wait_Queue_is_Empty_in_System( UINT32 wait_ms_pre, UINT32 timeout_ms )
{
	E_SYSTEM_EVENT	dummy_event;
	ULLONG			dummy_frame_no;
	UINT32			queue_num;
	UINT32			queue_cnt;
	UINT32			cnt;
	BOOL			become_empty = FALSE;
	
	for( cnt = 0; cnt < wait_ms_pre; cnt ++ ){
		// Wait 1ms
		OS_User_Dly_Tsk( 1 );
		UPRINTF( "Wait 1ms \n");
	}
	
	for( cnt = 0; cnt < timeout_ms; cnt ++ ){
		
		// Get queue
		Dd_ARM_Critical_Section_Start( g_system_spin_lock );
		if( D_SYSTEM_MBX_MAX >= g_system_mbx_empty_dtq.num ){
			queue_num = D_SYSTEM_MBX_MAX - g_system_mbx_empty_dtq.num;
		}else{
			queue_num = 0;
		}
		Dd_ARM_Critical_Section_End( g_system_spin_lock );
	
		// Check Empty
		if( queue_num == 0 ){
			become_empty = TRUE;
			break;
		}
		
		// Rcv
		for( queue_cnt = 0; queue_cnt < queue_num; queue_cnt ++ ){
			rcv_event( &dummy_event, &dummy_frame_no );
			UPRINTF( "rcv event:%d, rcv frame_no:%d\n", (INT32)dummy_event, (INT32)dummy_frame_no );
		}
		
		// Wait 1ms
		OS_User_Dly_Tsk( 1 );
		UPRINTF( "Wait 1ms \n");
	}
	
	if( become_empty != TRUE ){
		UPRINTF_ERR(1);
	}
}


void Wait_Specified_Event( E_SYSTEM_EVENT wait_event )
{
	E_SYSTEM_EVENT event;
	ULLONG frame_no;
	
	do{
		rcv_event( &event, &frame_no );
	}while( event != wait_event );
}


void System_Estimate_AE_ROI( ULLONG frame_no, const T_SHARE_AE_ROI_IN* in, T_SHARE_AE_ROI_OUT* out )
{
	Mode_View_Estimate_AE_ROI( frame_no, in, out );
}

E_SYSTEM_VIEW_MODE System_get_view_mode(void)
{
	return g_view_mode;
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static void init(void)
{
	// Sensor Initialization
	if( CAMERA.Func_Clock_GPIO_Init ){
		CAMERA.Func_Clock_GPIO_Init();
	}
}


static void rcv_event( E_SYSTEM_EVENT* event, ULLONG* frame_no )
{
	OS_USER_ER		ercd;
	T_SYSTEM_MBX*	mbx;
	
	// Get Memory
	ercd = OS_User_Rcv_Mbx( MID_SYSTEM, (T_OS_USER_MSG**)&mbx );
	if( ercd ){
		UPRINTF_ERR(ercd);
		*event    = E_SYSTEM_EVENT_ERR;
		*frame_no = 0;
		return;
	}
	
	// Set from Memory
	*event    = mbx->event;
	*frame_no = mbx->frame_no;
	
	//UPRINTF("receive event %d, addr %x, next %x\n", (INT32)mbx->event, (INT32)(mbx), (INT32)mbx->_os_reserve[0]);
	
	// Update Memory
	Dd_ARM_Critical_Section_Start( g_system_spin_lock );
	enqueue( (T_SYSTEM_MBX*)mbx );
	Dd_ARM_Critical_Section_End( g_system_spin_lock );
}


static INT32 enqueue( T_SYSTEM_MBX* data )
{
	if( g_system_mbx_empty_dtq.num < D_SYSTEM_MBX_MAX ){
		g_system_mbx_empty_dtq.data[(g_system_mbx_empty_dtq.head+g_system_mbx_empty_dtq.num)%D_SYSTEM_MBX_MAX] = data;
		g_system_mbx_empty_dtq.num++;
		return D_DDIM_USER_E_OK;
	}
	else{
		return D_DDIM_USER_E_SYS; // full
	}
}


static INT32 dequeue( T_SYSTEM_MBX** data )
{
	if( g_system_mbx_empty_dtq.num > 0 ){
		*data = g_system_mbx_empty_dtq.data[g_system_mbx_empty_dtq.head];
		g_system_mbx_empty_dtq.head = (g_system_mbx_empty_dtq.head+1)%D_SYSTEM_MBX_MAX;
		g_system_mbx_empty_dtq.num--;
		return D_DDIM_USER_E_OK;
	}
	else{
		return D_DDIM_USER_E_QOVR; // empty
	}
}

