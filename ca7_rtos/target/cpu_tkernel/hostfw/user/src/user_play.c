/**
 * @file		template.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#include "user.h"
#include "clk.h"
#include "arb.h"
#include "category_parameter.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
/*==============================================================================*/
// The begin/end address need to be revised after all parts of memory are designed
// (CUSTOM_VIEW_REC_TOP	0x50200000 ~ MOVIE_RECORD0_SDRAM0_END 0x6A1172C0,	SDRAM_MAP_Generate_Tool_CMOS.xlsb)	Åötekito-
// (VIEW_COMMON_END		0xACB39000 ~ MOVIE_RECORD0_SDRAM1_END 0xC25D8000,	SDRAM_MAP_Generate_Tool_CMOS.xlsb)	Åötekito-


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static E_USER_STATE				g_user_play_state = E_USER_STATE_IDLE;
static E_USER_OP_STATE			g_user_play_op_state = E_USER_OP_NONE;

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
E_USER_STATE User_Play( E_USER_EVENT event, ULLONG frame_no )
{
	INT32 ret = NG;
	E_SHARE_DISP_IF disp_if;

	switch( event )
	{
		case E_USER_EVENT_PLAY_START:
			UPRINTF("[CLOCK]Change Image Clock Mode to Play\n");
			FJ_Top_Set_Image_Clock( IMAGE_CLOCK[E_IMG_CLOCK_MODE_PLAY] );
			Set_Arb_MXIC(0);
			Set_Arb_SDRAMC(2);
			ret = User_Utility_Playback_Print_File_List();
			if( ret == OK ) {
				g_user_play_state = E_USER_STATE_BUSY;
				g_user_play_op_state = E_USER_OP_PLAY_START_DONE;
			}
			else {
				g_user_play_op_state = E_USER_OP_NONE;
			}
		break;
		
		case E_USER_EVENT_STANDBY:
			g_user_play_state = E_USER_STATE_IDLE;
			g_user_play_op_state = E_USER_OP_NONE;
		break;
		
		case E_USER_EVENT_PLAY_IMAGE_NO:
			disp_if = Cate_Get_Disp_If();
			User_Utility_Playback_Set_Disp_If( disp_if );
			ret = User_Utility_Playback_Jpeg_Play( frame_no );
			g_user_play_op_state = ( ( ret == NG ) ? E_USER_OP_NONE : E_USER_OP_PLAY_IMAGE_NO_DONE );
		break;

		case E_USER_EVENT_PLAY_VIDEO:
			disp_if = Cate_Get_Disp_If();
			User_Utility_Playback_Set_Disp_If( disp_if );
			ret = User_Utility_Playback_Video_Play( frame_no );
			g_user_play_op_state = ( ( ret == NG ) ? E_USER_OP_NONE : E_USER_OP_PLAY_VIDEO_DONE );
		break;

		case E_USER_EVENT_PLAY_PAUSE:
			ret = User_Utility_Playback_Video_Pause();
			g_user_play_op_state = ( ( ret == NG ) ? E_USER_OP_NONE : E_USER_OP_PLAY_PAUSE_DONE );
		break;

		case E_USER_EVENT_PLAY_STOP:
			ret = User_Utility_Playback_Video_Stop();
			g_user_play_op_state = ( ( ret == NG ) ? E_USER_OP_NONE : E_USER_OP_PLAY_STOP_DONE );
		break;

		case E_USER_EVENT_PLAY_FAST_FORWARD:
			ret = User_Utility_Playback_Video_Fast_Forward( frame_no );
			g_user_play_op_state = ( ( ret == NG ) ? E_USER_OP_NONE : E_USER_OP_PLAY_FAST_FORWARD_DONE );
		break;

		case E_USER_EVENT_PLAY_FAST_REWIND:
			ret = User_Utility_Playback_Video_Fast_Rewind( frame_no );
			g_user_play_op_state = ( ( ret == NG ) ? E_USER_OP_NONE : E_USER_OP_PLAY_FAST_REWIND_DONE );
		break;

		case E_USER_EVENT_PLAY_FRAME_BY_FRAME:
			ret = User_Utility_Playback_Video_Frame_By_Frame();
			g_user_play_op_state = ( ( ret == NG ) ? E_USER_OP_NONE : E_USER_OP_PLAY_FRAME_BY_FRAME_DONE );
		break;

		case E_USER_EVENT_PLAY_SPEC_TIME:
			ret = User_Utility_Playback_Video_Play_Spec_Time( frame_no );
			g_user_play_op_state = ( ( ret == NG ) ? E_USER_OP_NONE : E_USER_OP_PLAY_SPEC_TIME_DONE );
		break;

		case E_USER_EVENT_PLAY_THUMBNAIL_START:
			disp_if = Cate_Get_Disp_If();
			User_Utility_Playback_Set_Disp_If( disp_if );
			ret = User_Utility_Playback_Thumbnail_Play( frame_no );
			g_user_play_op_state = ( ( ret == NG ) ? E_USER_OP_NONE : E_USER_OP_PLAY_THUMBNAIL_START_DONE );
		break;

		case E_USER_EVENT_PLAY_THUMBNAIL_KEY_UP:
			ret = User_Utility_Playback_Thumbnail_Key_Up();
			g_user_play_op_state = ( ( ret == NG ) ? E_USER_OP_NONE : E_USER_OP_PLAY_THUMBNAIL_KEY_UP_DONE );
		break;

		case E_USER_EVENT_PLAY_THUMBNAIL_KEY_DOWN:
			ret = User_Utility_Playback_Thumbnail_Key_Down();
			g_user_play_op_state = ( ( ret == NG ) ? E_USER_OP_NONE : E_USER_OP_PLAY_THUMBNAIL_KEY_DOWN_DONE );
		break;

		case E_USER_EVENT_PLAY_THUMBNAIL_KEY_LEFT:
			ret = User_Utility_Playback_Thumbnail_Key_Left();
			g_user_play_op_state = ( ( ret == NG ) ? E_USER_OP_NONE : E_USER_OP_PLAY_THUMBNAIL_KEY_LEFT_DONE );
		break;

		case E_USER_EVENT_PLAY_THUMBNAIL_KEY_RIGHT:
			ret = User_Utility_Playback_Thumbnail_Key_Right();
			g_user_play_op_state = ( ( ret == NG ) ? E_USER_OP_NONE : E_USER_OP_PLAY_THUMBNAIL_KEY_RIGHT_DONE );
		break;

		case E_USER_EVENT_PLAY_THUMBNAIL_KEY_ENTER:
			ret = User_Utility_Playback_Thumbnail_Key_Enter();
			g_user_play_op_state = ( ( ret == NG ) ? E_USER_OP_NONE : E_USER_OP_PLAY_THUMBNAIL_KEY_ENTER_DONE );
		break;

		case E_USER_EVENT_PLAY_THUMBNAIL_END:
			ret = User_Utility_Playback_Thumbnail_End();
			g_user_play_op_state = ( ( ret == NG ) ? E_USER_OP_NONE : E_USER_OP_PLAY_THUMBNAIL_END_DONE );
		break;

		case E_USER_EVENT_PLAY_VIDEO_DECODE:
			User_Utility_Playback_VideoDecode();
		break;

		case E_USER_EVENT_PLAY_AUDIO_DECODE:
			User_Utility_Playback_AudioDecode();
		break;

		default:
		break;
		
	}
	
	return g_user_play_state;
}

BOOL User_Play_Is_Busy( VOID )
{
	return ( g_user_play_state == E_USER_STATE_BUSY );
}

E_USER_OP_STATE User_Play_Get_OP_Stat( VOID )
{
	return g_user_play_op_state;
}

VOID User_Play_Clear_OP_Stat( VOID )
{
	g_user_play_op_state = E_USER_OP_NONE;
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
