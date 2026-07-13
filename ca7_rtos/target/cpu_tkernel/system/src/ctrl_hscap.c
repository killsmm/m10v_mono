/**
 * @file		ctrl_hscap.c
 * @brief		control hs( high speed ) capture
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2019 Socionext Inc.</I></B>
 */

#include "ctrl_hscap.h"
#include "share_data.h"
#include "mode_stm.h"
#include "ctrl_pro2pipe.h"
#include "fj_peripheral.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define D_HSCAP_PIPE_MAX				D_PIPE_MEM_BANK_Y
#define D_HSCAP_MEDIA_MAX				32					// temporary
#define D_HSCAP_MSHUTTER_CLOSE_BUF		16					// temporary


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define D_HSCAP_Q_IS_FULL(a)			( g_hscap_mng.q[ a ].num == g_hscap_mng.q[ a ].max_num )
#define D_HSCAP_Q_IS_EMPTY(a)			( g_hscap_mng.q[ a ].num == 0 )
#define D_HSCAP_Q_HEAD_FRAME(a)			( g_hscap_mng.q[ a ].frame_no[ g_hscap_mng.q[ a ].head ] )


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/
typedef enum {
	E_HSCAP_Q_PIPE = 0,
	E_HSCAP_Q_MEDIA,
	E_HSCAP_Q_MAX,
} E_HSCAP_Q;


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	
	struct {
		UINT32	head;
		UINT32	num;
		UINT32	max_num;
		ULLONG*	frame_no;
	} q[ E_HSCAP_Q_MAX ];
	
	BOOL			mshutter_close[ D_HSCAP_MSHUTTER_CLOSE_BUF ];
	BOOL			is_nextprocess_running;
	BOOL			pipe_danger_at_least_once;
	INT32			pipe_danger_level;
	INT32			media_danger_level;
	FJ_PERI_CH		timer_ch;
	ULONG			timer_usec;
	ULONG			seq_cnt;
	BOOL			seq_continue;
	ULLONG			seq_pre;
	UINT64			measure_start;
	FLOAT			fps;
	BOOL			is_slow_case;
	
} T_HSCAP_MNG;


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static T_HSCAP_MNG		g_hscap_mng;
static ULLONG			g_hscap_frame_no_pipe[ D_HSCAP_PIPE_MAX ];
static ULLONG			g_hscap_frame_no_media[ D_HSCAP_MEDIA_MAX ];


/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static INT32 hscap_enqueue( E_HSCAP_Q q, ULLONG frame_no )
{
	UINT32 index;
	
	if( g_hscap_mng.q[ q ].num < g_hscap_mng.q[ q ].max_num ){
		index = ( g_hscap_mng.q[q].head + g_hscap_mng.q[ q ].num ) % g_hscap_mng.q[ q ].max_num;
		g_hscap_mng.q[ q ].frame_no[ index ] = frame_no;
		g_hscap_mng.q[ q ].num ++;
		return OK;
	}else{
		return NG; // full
	}
}

static INT32 hscap_dequeue( E_HSCAP_Q q, ULLONG* frame_no )
{
	if( g_hscap_mng.q[q].num > 0 ){
		if( frame_no ){
			*frame_no = g_hscap_mng.q[ q ].frame_no[ g_hscap_mng.q[ q ].head ];
		}
		g_hscap_mng.q[ q ].head = ( g_hscap_mng.q[q].head + 1 ) % g_hscap_mng.q[ q ].max_num;
		g_hscap_mng.q[ q ].num --;
		return OK;
	}else{
		return NG; // empty
	}
}

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void HSCap_Init( UINT32 pipe_bank_num, UINT32 media_bank_num, FLOAT fps, BOOL is_slow_case )
{
	memset( &g_hscap_mng, 0, sizeof(g_hscap_mng) );
	
	// Timer
	g_hscap_mng.timer_ch = FJ_PERI_CH_7; // used at movie ( D_BF_MOVIE_COMMON_RTIMER_ENC_USE_CH )
	
	// FPS, etc
	g_hscap_mng.fps = fps;
	g_hscap_mng.is_slow_case = is_slow_case;
	
	// Pipe
	if( pipe_bank_num > D_HSCAP_PIPE_MAX ){
		// warning
		g_hscap_mng.q[ E_HSCAP_Q_PIPE ].max_num = D_HSCAP_PIPE_MAX;
	}else{
		g_hscap_mng.q[ E_HSCAP_Q_PIPE ].max_num = pipe_bank_num;
	}
	if( g_hscap_mng.q[ E_HSCAP_Q_PIPE ].max_num == 0 ){
		g_hscap_mng.q[ E_HSCAP_Q_PIPE ].max_num = 1;
	}
	g_hscap_mng.q[E_HSCAP_Q_PIPE].frame_no = g_hscap_frame_no_pipe;
	
	// Media
	if( media_bank_num > D_HSCAP_MEDIA_MAX ){
		// warning
		g_hscap_mng.q[ E_HSCAP_Q_MEDIA ].max_num = D_HSCAP_MEDIA_MAX;
	}else{
		g_hscap_mng.q[ E_HSCAP_Q_MEDIA ].max_num = media_bank_num;
	}
	if( g_hscap_mng.q[ E_HSCAP_Q_MEDIA ].max_num == 0 ){
		g_hscap_mng.q[ E_HSCAP_Q_MEDIA ].max_num = 1;
	}
	g_hscap_mng.q[ E_HSCAP_Q_MEDIA ].frame_no = g_hscap_frame_no_media;
}


BOOL HSCap_Is_Close_MShutter( ULLONG frame_no )
{
	return g_hscap_mng.mshutter_close[ frame_no % D_HSCAP_MSHUTTER_CLOSE_BUF ];
}


void HSCap_Judge_MShutter( ULLONG frame_no )
{
	BOOL	mshutter_close = FALSE;
	
	// PIPE danger level
	if( g_hscap_mng.q[ E_HSCAP_Q_PIPE ].num >= g_hscap_mng.q[ E_HSCAP_Q_PIPE ].max_num ){
		if( g_hscap_mng.is_slow_case ){
			g_hscap_mng.pipe_danger_level += 2;
		}else if( g_hscap_mng.q[ E_HSCAP_Q_PIPE ].max_num <= 2 && g_hscap_mng.pipe_danger_at_least_once == FALSE ){
			// special case, 1st time and bank <=2
			g_hscap_mng.pipe_danger_level = 2;
		}else{
			if( g_hscap_mng.fps > (30+1) ){ // temporary for >= 30fps, 1 is margin
				g_hscap_mng.pipe_danger_level += 2;
			}else{
				g_hscap_mng.pipe_danger_level ++;
			}
		}
		g_hscap_mng.pipe_danger_at_least_once = TRUE;
	}else{
		g_hscap_mng.pipe_danger_level = 0;
	}
	// Judge PIPE
	if( g_hscap_mng.pipe_danger_level >= 2 ){
		g_hscap_mng.pipe_danger_level = 0;
		mshutter_close = TRUE;
	}
	
	// MEDIA danger level
	if( g_hscap_mng.q[ E_HSCAP_Q_MEDIA ].num >= g_hscap_mng.q[ E_HSCAP_Q_MEDIA ].max_num ){
		g_hscap_mng.media_danger_level = 2;
	}else{
		g_hscap_mng.media_danger_level = 0;
	}
	// Judge Meddia
	if( g_hscap_mng.media_danger_level >= 2){
		g_hscap_mng.media_danger_level = 0;
		mshutter_close = TRUE;
	}
	
	// Update Mshutter
	g_hscap_mng.mshutter_close[ frame_no % D_HSCAP_MSHUTTER_CLOSE_BUF ] = mshutter_close;
	
	// DEBUG_LOG
	if( mshutter_close ){
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHUTTER, FJ_MBALOG_FUNC_ID_COMMON, FJ_MBALOG_TYPE_POINT, "Close %d", (INT32)frame_no));
	}else{
		MPRINTF((D_MBA_KIND_HOST, FJ_MBALOG_LINE_ID_SHUTTER, FJ_MBALOG_FUNC_ID_SYSTEM, FJ_MBALOG_TYPE_POINT, "Open %d", (INT32)frame_no));
	}
}


INT32 HSCap_Push_PipeQ_MediaQ( ULLONG frame_no )
{
	if( hscap_enqueue( E_HSCAP_Q_PIPE, frame_no ) ){
		UPRINTF_ERR( (UINT32)frame_no);// not expected
		return NG;
	}
	if( hscap_enqueue( E_HSCAP_Q_MEDIA, frame_no ) ){
		UPRINTF_ERR( (UINT32)frame_no);// not expected
		return NG;
	}
	
	return OK;
}


void HSCap_Timer_Callback( void )
{
	FJ_ERR_CODE		ercd;
	
	ercd = FJ_TMR32_Close(g_hscap_mng.timer_ch);
	UPRINTF_ERR(ercd);
	
	Snd_Event_to_System( E_SYSTEM_EVENT_CAP_PIPE_DUMMY, 0 );
}


void HSCap_Start_Pipe_Dummy( void )
{
	FJ_ERR_CODE		ercd;
	
	ercd = FJ_TMR32_Open( g_hscap_mng.timer_ch );
	UPRINTF_ERR( ercd );
	
	if( g_hscap_mng.timer_usec == 0 ){
		g_hscap_mng.timer_usec = 16*1000;// fail safe
	}
	ercd = FJ_TMR32_Set( g_hscap_mng.timer_ch, g_hscap_mng.timer_usec, HSCap_Timer_Callback );
	UPRINTF_ERR( ercd );
	
	ercd = FJ_TMR32_Start( g_hscap_mng.timer_ch, FALSE );
	UPRINTF_ERR( ercd );
}


BOOL HSCap_Judge_Start_NextProcess( ULLONG* frame_no )
{
	if( g_hscap_mng.is_nextprocess_running ){
		return FALSE;
	}else if( D_HSCAP_Q_IS_EMPTY( E_HSCAP_Q_PIPE ) ){
		return FALSE;
	}else{
		g_hscap_mng.is_nextprocess_running = TRUE;
		if( frame_no ){
			*frame_no = D_HSCAP_Q_HEAD_FRAME( E_HSCAP_Q_PIPE );
		}
		return TRUE;
	}
}


void HSCap_Pull_PipeQ( void )
{
	g_hscap_mng.is_nextprocess_running = FALSE;
	hscap_dequeue( E_HSCAP_Q_PIPE, NULL );
}


void HSCap_Pull_MediaQ( void )
{
	hscap_dequeue( E_HSCAP_Q_MEDIA, NULL );
}


void HSCap_Measure_Pipe_Start( ULLONG frame_no )
{
	if( g_hscap_mng.seq_pre == 0 ){
		// 1st
		g_hscap_mng.seq_continue = TRUE;
		g_hscap_mng.seq_cnt ++;
		g_hscap_mng.seq_pre = frame_no;
		g_hscap_mng.measure_start = BF_Get_Time_us_64();
	}
	else if( (g_hscap_mng.seq_pre+1) == frame_no ){
		g_hscap_mng.seq_cnt ++;
		g_hscap_mng.seq_pre = frame_no;
		g_hscap_mng.measure_start = BF_Get_Time_us_64();
	}else{
		g_hscap_mng.seq_continue = FALSE;
	}
}


void HSCap_Measure_Pipe_End( void )
{
	ULONG time;
	
	
	if( g_hscap_mng.seq_continue ){
		time = (ULONG)( BF_Get_Time_us_64() - g_hscap_mng.measure_start );
	
		if( g_hscap_mng.timer_usec == 0 ){
			g_hscap_mng.timer_usec = time;
		}else{
			g_hscap_mng.timer_usec = ( g_hscap_mng.timer_usec + time ) / 2;
		}
	}
}

void HSCap_Set_Media_Bank( UINT32 media_bank_num )
{
	// Media
	if( media_bank_num > D_HSCAP_MEDIA_MAX ){
		// warning
		g_hscap_mng.q[ E_HSCAP_Q_MEDIA ].max_num = D_HSCAP_MEDIA_MAX;
	}else{
		g_hscap_mng.q[ E_HSCAP_Q_MEDIA ].max_num = media_bank_num;
	}
	if( g_hscap_mng.q[ E_HSCAP_Q_MEDIA ].max_num == 0 ){
		g_hscap_mng.q[ E_HSCAP_Q_MEDIA ].max_num = 1;
	}
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
