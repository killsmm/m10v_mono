/**
 * @file		share_data.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */
//#pragma GCC optimize ("O0")

#include "share_data.h"
#include "user.h"
#include "system.h"
#include "dd_arm.h"
#include "sdram_map_define.h"
#include "mode_stm.h"

#include "category_parameter.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define D_SHARE_DATA_MAX	(0x20)	// should be 0x10 times


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define GET_INDEX(frame_no)		( frame_no & (D_SHARE_DATA_MAX-1) )
#define CRITICAL_START			static ULONG	critseq __attribute__((section(".LOCK_SECTION"), aligned(64))); \
								Dd_ARM_Critical_Section_Start( critseq );
#define CRITICAL_END			Dd_ARM_Critical_Section_End( critseq );

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
typedef struct {
	T_SHARE_SYSTEM_PARAM	system_param;
	T_SHARE_YUV				yuv;
	T_SHARE_PIPE_YUV		pipe_yuv;
	T_SHARE_PIPE_DOL_YUV	dol_yuv;
	T_SHARE_YUV				vhdr_yuv;
	T_SHARE_YUV				sgde_yuv;
	T_SHARE_YUV				qview_yuv;
	T_SHARE_YUV				stitch_yuv;
	T_SHARE_STITCH_INFO		stitch_info;
	T_SHARE_MOTION_VECTOR	motion;
	T_SHARE_YUV				shdr_yuv;
	T_SHARE_DOL_COMBINE		dol_combine;
	T_SHARE_RAW				sen_raw;
	T_SHARE_AEAWB_STAT		aeawb_stat;
	T_SHARE_SENSOR_EXP		sensor_exp;
	T_SHARE_ISP_GAIN		isp_gain;
	T_SHARE_APEX			apex;
	T_SHARE_AWB_PARAM		awb_param;
	T_SHARE_R2Y_HIST		r2y_hist;
	T_SHARE_R2Y_DOL_HIST	r2y_dol_hist;
	T_SHARE_PRO_HIST		pro_hist;
	T_SHARE_PRO_DOL_HIST	pro_dol_hist;
	T_SHARE_AF_DATA			af_data;
	T_SHARE_FRAME_TIMESTAMP	frame_stamp;
} T_SHARE_FRAME;

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static volatile T_SHARE_FRAME         g_share_frame[D_SHARE_DATA_MAX] __attribute__((section(".LOCK_SECTION"), aligned(64))); // need critical?
static volatile T_SHARE_SYSTEM_CONFIG g_system_config                 __attribute__((section(".LOCK_SECTION"), aligned(64))); // need critical?
static volatile T_SHARE_SYSTEM_PARAM  g_system_param                  __attribute__((section(".LOCK_SECTION"), aligned(64))); // need critical?
static volatile T_SHARE_AE_PARAM      g_ae_param                      __attribute__((section(".LOCK_SECTION"), aligned(64))); // need critical?
static volatile T_SHARE_AWB_PARAM     g_awb_param                     __attribute__((section(".LOCK_SECTION"), aligned(64))); // need critical?
static volatile T_SHARE_AF_PARAM      g_af_param                      __attribute__((section(".LOCK_SECTION"), aligned(64))); // need critical?
static volatile T_SHARE_MOVIE_REC_CONFIG g_movie_rec_config			  __attribute__((section(".LOCK_SECTION"), aligned(64))); // need critical?
static volatile T_SHARE_RGB_RATIO     g_rgb_param                     __attribute__((section(".LOCK_SECTION"), aligned(64))); // need critical?
static volatile ULLONG                g_iq_dbg2_save_reg_frame        __attribute__((section(".LOCK_SECTION"), aligned(64))); // need critical?

static volatile ULONG g_shd_tbl_addr[E_SHARE_SENSOR_ID_MAX][E_SHD_INDEX_MAX];
static volatile UCHAR* g_aeawb_blk_valid_for_circle[E_SHARE_SENSOR_ID_MAX];
static volatile E_SHARE_SYSTEM_MODE   g_mode;
static T_SHARE_ENTRY g_system_config_callback;
static T_SHARE_ENTRY1 g_raw_callback;
static ULONG g_ae_param_spin_lock	__attribute__((section(".LOCK_SECTION"), aligned(64)));
static ULONG g_awb_param_spin_lock	__attribute__((section(".LOCK_SECTION"), aligned(64)));
static ULONG g_af_param_spin_lock	__attribute__((section(".LOCK_SECTION"), aligned(64)));
static ULONG g_system_param_spin_lock	__attribute__((section(".LOCK_SECTION"), aligned(64)));

static volatile ULONG g_share_available_memory_addr_ch0;
static volatile ULONG g_share_available_memory_addr_ch1;
static ULONG g_available_memory_spin_lock	__attribute__((section(".LOCK_SECTION"), aligned(64)));

static UINT32 g_media_bank_num;

static E_SHARE_DYNAMIC_MODE g_dynamic_capture_mode;
static UINT8 g_dynamic_capture_yuv;
/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void Set_Share_System_Config( const T_SHARE_SYSTEM_CONFIG* config, E_SHARE_SYSTEM_MODE mode, T_SHARE_ENTRY callback )
{
	g_system_config = *config;
	g_mode = mode;
	g_system_config_callback = callback;
}


void Set_Share_System_Param( const T_SHARE_SYSTEM_PARAM* param )
{
	Dd_ARM_Critical_Section_Start(g_system_param_spin_lock);
	g_system_param = *param;
	Dd_ARM_Critical_Section_End(g_system_param_spin_lock);
}


void Send_Share_System_Event( E_SHARE_SYSTEM_EVENT event, ULLONG frame_no )
{
	switch( event ){
	case E_SHARE_SYSTEM_EVENT_START_MODE:
		if( g_mode == E_SHARE_SYSTEM_MODE_VIEW ){
			Snd_Event_to_System( E_SYSTEM_EVENT_MODE_VIEW, 0 );
		} else if( g_mode == E_SHARE_SYSTEM_MODE_YUVIN_VIEW ) {
			Snd_Event_to_System( E_SYSTEM_EVENT_MODE_YUVIN_VIEW, 0 );
		}
		break;
		
	case E_SHARE_SYSTEM_EVENT_STOP_MODE:
		if( g_mode == E_SHARE_SYSTEM_MODE_VIEW ){
			Snd_Event_to_System( E_SYSTEM_EVENT_STOP_MODE, 0 );
		} else if( g_mode == E_SHARE_SYSTEM_MODE_YUVIN_VIEW ) {
			Snd_Event_to_System( E_SYSTEM_EVENT_STOP_YUVIN_MODE, 0 );
		}
		break;
		
	case E_SHARE_SYSTEM_EVENT_AE_END:
		Snd_Event_to_System( E_SYSTEM_EVENT_AE_END, frame_no );
		break;
		
	case E_SHARE_SYSTEM_EVENT_AWB_END:
		Snd_Event_to_System( E_SYSTEM_EVENT_AWB_END, frame_no );
		break;
		
	case E_SHARE_SYSTEM_EVENT_AF_UPDATE:
		Snd_Event_to_System( E_SYSTEM_EVENT_AF_UPDATE, frame_no );
		break;
	case E_SHARE_SYSTEM_EVENT_IQ_DBG2_SAVE_REG:
		Snd_Event_to_System( E_SYSTEM_EVENT_IQ_DBG2_SAVE_REG, frame_no );
		break;
		
	case E_SHARE_SYSTEM_EVENT_CAP_MEDIA_REL:
		Snd_Event_to_System( E_SYSTEM_EVENT_CAP_MEDIA_REL, frame_no );
		break;
	case E_SHARE_SYSTEM_EVENT_VIEW_RECONFIG:
		Snd_Event_to_System( E_SYSTEM_EVENT_VIEW_RECONFIG, frame_no );
		break;
	case E_SHARE_SYSTEM_EVENT_CAP_WITHOUT_RESET:
		Snd_Event_to_System( E_SYSTEM_EVENT_CAP_WITHOUT_RESET, frame_no );
		break;
	break;
	default:
		break;
	}
}


void Send_Share_User_Event( E_SHARE_USER_EVENT event, ULLONG frame_no )
{
	switch( event ){
	case E_SHARE_USER_EVENT_VD:
		Snd_Event_to_User( E_USER_EVENT_VD, frame_no );
		break;
	case E_SHARE_USER_EVENT_QVIEW_END:
		Snd_Event_to_User( E_USER_EVENT_QVIEW, frame_no );
		break;
	case E_SHARE_USER_EVENT_YUV_END:
		Snd_Event_to_User( E_USER_EVENT_YUV, frame_no );
		break;
	case E_SHARE_USER_EVENT_AEAWB_STAT_END:
		Snd_Event_to_User( E_USER_EVENT_AEAWB_STAT, frame_no );
		break;
	case E_SHARE_USER_EVENT_AF_STAT_END:
		Snd_Event_to_User( E_USER_EVENT_AF_STAT, frame_no );
		break;
	case E_SHARE_USER_EVENT_MODE_END:
		Snd_Event_to_User( E_USER_EVENT_SYSTEM_MODE_END, frame_no );
		break;
	default:
		break;
	}
}


INT32 Callback_Share_System_Config( const T_SHARE_SYSTEM_CONFIG_RETURN* config_ret )
{
	if( g_system_config_callback ){
		return g_system_config_callback( (void*)config_ret );
	}

	return 0;
}


void Get_Share_System_Config( T_SHARE_SYSTEM_CONFIG* const config )
{
	*config = g_system_config;
}


void Get_Share_System_Param( T_SHARE_SYSTEM_PARAM* const param )
{
	*param = g_system_param;
}


void Set_Share_AE_Param( const T_SHARE_AE_PARAM* param )
{
	Dd_ARM_Critical_Section_Start(g_ae_param_spin_lock);
	g_ae_param = *param;
	Dd_ARM_Critical_Section_End(g_ae_param_spin_lock);
}


void Get_Share_AE_Param( T_SHARE_AE_PARAM* const param )
{
	Dd_ARM_Critical_Section_Start(g_ae_param_spin_lock);
	*param = g_ae_param;
	Dd_ARM_Critical_Section_End(g_ae_param_spin_lock);
}

void Set_Share_RGB_Param( const T_SHARE_RGB_RATIO* param )
{
	Dd_ARM_Critical_Section_Start(g_ae_param_spin_lock);
	g_rgb_param = *param;
	Dd_ARM_Critical_Section_End(g_ae_param_spin_lock);
}


void Get_Share_RGB_Param( T_SHARE_RGB_RATIO* const param )
{
	Dd_ARM_Critical_Section_Start(g_ae_param_spin_lock);
	*param = g_rgb_param;
	Dd_ARM_Critical_Section_End(g_ae_param_spin_lock);
}


void Set_Share_AWB_Param( const T_SHARE_AWB_PARAM* param )
{
	Dd_ARM_Critical_Section_Start(g_awb_param_spin_lock);
	g_awb_param = *param;
	Dd_ARM_Critical_Section_End(g_awb_param_spin_lock);
}


void Set_Share_SHD_Tbl( ULONG tbl_addr[E_SHARE_SENSOR_ID_MAX][E_SHD_INDEX_MAX] )
{
	INT32 i, j;
	
	for( i = 0; i < E_SHARE_SENSOR_ID_MAX; i ++ ){
		for( j = 0; j < E_SHD_INDEX_MAX; j ++ ){
			g_shd_tbl_addr[i][j] = tbl_addr[i][j];
		}
	}
}

ULONG Get_Share_SHD_Tbl_Addr( E_SHARE_SENSOR_ID sensor_id, E_SHD_INDEX index )
{
	if( sensor_id < E_SHARE_SENSOR_ID_MAX && index < E_SHD_INDEX_MAX ){
		return g_shd_tbl_addr[sensor_id][index];
	}else{
		return g_shd_tbl_addr[E_SHARE_SENSOR_ID_0][E_SHD_INDEX_0];
	}
}


void Set_Share_AEAWB_Blk_Valid_For_Circle( E_SHARE_SENSOR_ID sensor_id, UCHAR* addr )
{
	g_aeawb_blk_valid_for_circle[sensor_id] = (UCHAR*)addr;
}


const UCHAR* Get_Share_AEAWB_Blk_Valid_For_Circle( E_SHARE_SENSOR_ID sensor_id )
{
	return (const UCHAR*)g_aeawb_blk_valid_for_circle[sensor_id];
}


void Get_Share_AWB_Param( T_SHARE_AWB_PARAM* const param )
{
	Dd_ARM_Critical_Section_Start(g_awb_param_spin_lock);
	*param = g_awb_param;
	Dd_ARM_Critical_Section_End(g_awb_param_spin_lock);
}


void Set_Share_AF_Param( const T_SHARE_AF_PARAM* param )
{
	Dd_ARM_Critical_Section_Start(g_af_param_spin_lock);
	g_af_param = *param;
	Dd_ARM_Critical_Section_End(g_af_param_spin_lock);
}


void Get_Share_AF_Param( T_SHARE_AF_PARAM* const param )
{
	Dd_ARM_Critical_Section_Start(g_af_param_spin_lock);
	*param = g_af_param;
	Dd_ARM_Critical_Section_End(g_af_param_spin_lock);
}


void Set_Share_Movie_Rec_Config( const T_SHARE_MOVIE_REC_CONFIG	*cfg )
{
	g_movie_rec_config = *cfg;
}

void Get_Share_Movie_Rec_Config( T_SHARE_MOVIE_REC_CONFIG* const cfg )
{
	*cfg = g_movie_rec_config;
}


void Set_Share_IQ_DBG2_Save_Reg_Frame( ULLONG frame_no )
{
	g_iq_dbg2_save_reg_frame = frame_no;
}


ULLONG	Get_Share_IQ_DBG2_Save_Reg_Frame( void )
{
	return g_iq_dbg2_save_reg_frame;
}


void Get_Frame_Share_System_Param( T_SHARE_SYSTEM_PARAM* const param, ULLONG frame_no )
{
	*param = g_share_frame[GET_INDEX(frame_no)].system_param;
}


void Copy_Frame_Share_System_Param( ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].system_param = g_system_param;
}


void Set_Frame_Share_YUV( const T_SHARE_YUV* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].yuv = *in;
}

void Set_Frame_Share_PIPE_DOL_YUV( const T_SHARE_PIPE_DOL_YUV* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].dol_yuv = *in;
}

void Get_Frame_Share_YUV( T_SHARE_YUV* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].yuv;
}

void Get_Frame_Share_PIPE_DOL_YUV( T_SHARE_PIPE_DOL_YUV* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].dol_yuv;
}


void Set_Frame_Share_PIPE_YUV( const T_SHARE_PIPE_YUV* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].pipe_yuv = *in;
}

void Get_Frame_Share_PIPE_YUV( T_SHARE_PIPE_YUV* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].pipe_yuv;
}

void Set_Frame_Share_VHDR_YUV( const T_SHARE_YUV* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].vhdr_yuv = *in;
}

void Get_Frame_Share_VHDR_YUV( T_SHARE_YUV* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].vhdr_yuv;
}

void Set_Frame_Share_SGDE_YUV( const T_SHARE_YUV* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].sgde_yuv = *in;
}

void Get_Frame_Share_SGDE_YUV( T_SHARE_YUV* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].sgde_yuv;
}

void Set_Frame_Share_QVIEW_YUV( const T_SHARE_YUV* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].qview_yuv = *in;
}

void Get_Frame_Share_QVIEW_YUV( T_SHARE_YUV* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].qview_yuv;
}

void Set_Frame_Share_Stitch_YUV( const T_SHARE_YUV* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].stitch_yuv = *in;
}

void Get_Frame_Share_Stitch_YUV( T_SHARE_YUV* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].stitch_yuv;
}

void Set_Frame_Share_Stitch_Info( const T_SHARE_STITCH_INFO* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].stitch_info = *in;
}

void Get_Frame_Share_Stitch_Info( T_SHARE_STITCH_INFO* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].stitch_info;
}

void Set_Frame_Share_Motion_Vector( const T_SHARE_MOTION_VECTOR* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].motion = *in;
}

void Get_Frame_Share_Motion_Vector( T_SHARE_MOTION_VECTOR* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].motion;
}

void Set_Frame_Share_SHDR_YUV( const T_SHARE_YUV* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].shdr_yuv = *in;
}

void Get_Frame_Share_SHDR_YUV( T_SHARE_YUV* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].shdr_yuv;
}

void Set_Frame_Share_Dol_Combine( const T_SHARE_DOL_COMBINE* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].dol_combine = *in;
}

void Get_Frame_Share_Dol_Combine( T_SHARE_DOL_COMBINE* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].dol_combine;
}

void Set_Frame_Share_Sensor_Raw( const T_SHARE_RAW* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].sen_raw = *in;
}

void Get_Frame_Share_Sensor_Raw( T_SHARE_RAW* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].sen_raw;
}

void Set_Frame_Share_AEAWB_STAT( const T_SHARE_AEAWB_STAT* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].aeawb_stat = *in;
}

void Get_Frame_Share_AEAWB_STAT( T_SHARE_AEAWB_STAT* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].aeawb_stat;
}

void Set_Frame_Share_Sensor_Exp( const T_SHARE_SENSOR_EXP* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].sensor_exp = *in;
}

void Get_Frame_Share_Sensor_Exp( T_SHARE_SENSOR_EXP* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].sensor_exp;
}

void Set_Frame_Share_ISP_Gain( const T_SHARE_ISP_GAIN* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].isp_gain = *in;
}

void Get_Frame_Share_ISP_Gain( T_SHARE_ISP_GAIN* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].isp_gain;
}

void Set_Frame_Share_APEX( const T_SHARE_APEX* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].apex = *in;
}

void Get_Frame_Share_APEX( T_SHARE_APEX* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].apex;
}

void Set_Frame_Share_AWB_Param( const T_SHARE_AWB_PARAM* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].awb_param = *in;
}

void Get_Frame_Share_AWB_Param( T_SHARE_AWB_PARAM* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].awb_param;
}

void Set_Frame_Share_R2Y_Hist( const T_SHARE_R2Y_HIST* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].r2y_hist = *in;
}

void Get_Frame_Share_R2Y_Hist( T_SHARE_R2Y_HIST* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].r2y_hist;
}

void Set_Frame_Share_R2Y_DOL_Hist( const T_SHARE_R2Y_DOL_HIST* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].r2y_dol_hist = *in;
}

void Get_Frame_Share_R2Y_DOL_Hist( T_SHARE_R2Y_DOL_HIST* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].r2y_dol_hist;
}

void Set_Frame_Share_PRO_Hist( const T_SHARE_PRO_HIST* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].pro_hist = *in;
}

void Get_Frame_Share_PRO_Hist( T_SHARE_PRO_HIST* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].pro_hist;
}

void Set_Frame_Share_PRO_DOL_Hist( const T_SHARE_PRO_DOL_HIST* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].pro_dol_hist = *in;
}

void Get_Frame_Share_PRO_DOL_Hist( T_SHARE_PRO_DOL_HIST* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].pro_dol_hist;
}

void Set_Frame_Share_AF_Data( const T_SHARE_AF_DATA* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].af_data = *in;
}

void Get_Frame_Share_AF_Data( T_SHARE_AF_DATA* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].af_data;
}

void Set_Frame_Share_FrameStamp( const T_SHARE_FRAME_TIMESTAMP* in, ULLONG frame_no )
{
	g_share_frame[GET_INDEX(frame_no)].frame_stamp = *in;
}

void Get_Frame_Share_FrameStamp( T_SHARE_FRAME_TIMESTAMP* const out, ULLONG frame_no )
{
	*out = g_share_frame[GET_INDEX(frame_no)].frame_stamp;
}

void Set_Share_Available_Memory( ULONG addr, ULONG end_addr )
{
	Dd_ARM_Critical_Section_Start(g_available_memory_spin_lock); // for safety

	if( addr < SDRAM_ADR_CH1_TOP ){
		// ch0
		if( addr > end_addr ){
			UPRINTF_ERR(1);
		}else{
			g_share_available_memory_addr_ch0 = addr;
		}
	}else{
		// ch1
		if( addr > end_addr ){
			UPRINTF_ERR(1);
		}else{
			g_share_available_memory_addr_ch1 = addr;
		}
	}
	
	Dd_ARM_Critical_Section_End(g_available_memory_spin_lock); // for safety
}

ULONG Get_Share_Available_Memory( UCHAR ch )
{
	ULONG addr = 0;

	Dd_ARM_Critical_Section_Start(g_available_memory_spin_lock); // for safety
	if( ch == 0 ){
		addr = g_share_available_memory_addr_ch0;
	}else{
		addr = g_share_available_memory_addr_ch1;
	}
	
	Dd_ARM_Critical_Section_End(g_available_memory_spin_lock); // for safety

	return addr;
}

E_SDRAM_CH Get_Share_SDRAM_CH( ULONG addr )
{
	if( addr < SDRAM_ADR_CH1_TOP ){
		// ch0
		return E_SDRAM_CH0;
	}else{
		// ch1
		return E_SDRAM_CH1;
	}
}


void Estimate_Share_AE_ROI( ULLONG frame_no, const T_SHARE_AE_ROI_IN* in, T_SHARE_AE_ROI_OUT* out )
{
	System_Estimate_AE_ROI( frame_no, in, out );
}

void Set_Share_SensorRaw_Callback( T_SHARE_ENTRY1 callback )
{
	g_raw_callback = callback;
}

void Callback_Share_SensorRaw( const T_SHARE_RAW* sensor_raw, ULLONG f_no )
{
	if( g_raw_callback ){
		g_raw_callback( (void*)sensor_raw, f_no );
	}
}

void Set_Share_Media_Bank(UINT32 num)
{
	g_media_bank_num = num;
}

UINT32 Get_Share_Media_Bank(void)
{
	return g_media_bank_num;
}

void Set_Share_AutoFraming_Roi_Pos(UINT32 roiX, UINT32 roiY, UINT32 roiW, UINT32 roiH)
{
	Cate_Set_AutoFraming_Roi_Pos(roiX, roiY, roiW, roiH);
}

void Get_Share_AutoFraming_Win_Pos(UINT32 *winSize, UINT32 *winPos)
{
	Cate_Get_AutoFraming_Win_Pos(winSize, winPos);
}

void Update_Share_Roi0_PTZ_Info( FLOAT trim_ratio, FLOAT zoom_pos_x, FLOAT zoom_pos_y )
{
	Cate_Update_Roi0_PTZ_Info(trim_ratio, zoom_pos_x, zoom_pos_y);
}

void Set_Share_Dynamic_Capture_Mode(E_SHARE_DYNAMIC_MODE mode){
	g_dynamic_capture_mode = mode;
}
E_SHARE_DYNAMIC_MODE Get_Share_Dynamic_Capture_Mode(){
	return g_dynamic_capture_mode;
}

void Set_Share_Dynamic_Capture_YUV(UINT8 enable){
	g_dynamic_capture_yuv = enable;
}
UINT8 Get_Share_Dynamic_Capture_YUV(){
	return g_dynamic_capture_yuv;
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
