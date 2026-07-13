/**
 * @file		detect_task.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#include "detect_task.h"
#include "user.h"
#include "fj_disp.h"
#include <string.h>
#include "os_user_custom.h"
#include "fj_media.h"
#include "fj_param.h"
#include "fj_filesystem.h"
#include "fj_dcf.h"
#include "category_parameter.h"
#include "dd_gic.h"
#include "mwsdio.h"
#include "sd_cmd.h"
#include "os_if.h"
#include "hdmi.h"
/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define D_SDCH1			(1)

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define IS_FLGPTN_EQUAL(flgptn, FLG)	((((flgptn) & (FLG)) == (FLG)) ? TRUE : FALSE)

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
struct hdmi_reso {
	USHORT width;
	USHORT height;
};

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static UCHAR gID_HDMI = 0xFF;
static FJ_DISP_VIDEO_ID	g_select_video_id = FJ_DISP_VIDEO_ID_NONE;
static FLOAT g_hdmi_fps = 0.0;
static struct hdmi_reso g_hdmi_vid_reso[FJ_DISP_VIDEO_ID_MAX] =
{
	{   0,   0 },
	{ 640, 480 },
	{ 720, 480 },
	{ 720, 480 },
	{1280, 720 },
	{1920, 1080},
	{1440, 480 },
	{1440, 480 },
	{1440, 240 },
	{1440, 240 },
	{2880, 480 },
	{2880, 480 },
	{2880, 240 },
	{2880, 240 },
	{1440, 480 },
	{1440, 480 },
	{1920, 1080},
	{ 720, 576 },
	{ 720, 576 },
	{1280, 720 },
	{1920, 1080},
	{1440, 576 },
	{1440, 576 },
	{1440, 288 },
	{1440, 288 },
	{2880, 576 },
	{2880, 576 },
	{2880, 288 },
	{2880, 288 },
	{1440, 576 },
	{1440, 576 },
	{1920, 1080},
	{1920, 1080},
	{1920, 1080},
	{1920, 1080},
	{2880, 480 },
	{2880, 480 },
	{2880, 576 },
	{2880, 576 },
	{1920, 1080},
	{1920, 1080},
	{1280, 720 },
	{ 720, 576 },
	{ 720, 576 },
	{1440, 576 },
	{1440, 576 },
	{1920, 1080},
	{1280, 720 },
	{ 720, 480 },
	{ 720, 480 },
	{1440, 480 },
	{1440, 480 },
	{ 720, 576 },
	{ 720, 576 },
	{1440, 576 },
	{1440, 576 },
	{ 720, 480 },
	{ 720, 480 },
	{1440, 480 },
	{1440, 480 },
	{1280, 720 },
	{1280, 720 },
	{1280, 720 },
	{1920, 1080},
	{1920, 1080},
	{1280, 720 },
	{1280, 720 },
	{1280, 720 },
	{1280, 720 },
	{1280, 720 },
	{1280, 720 },
	{1280, 720 },
	{1920, 1080},
	{1920, 1080},
	{1920, 1080},
	{1920, 1080},
	{1920, 1080},
	{1920, 1080},
	{1920, 1080},
	{1680, 720 },
	{1680, 720 },
	{1680, 720 },
	{1680, 720 },
	{1680, 720 },
	{1680, 720 },
	{1680, 720 },
	{2560, 1080},
	{2560, 1080},
	{2560, 1080},
	{2560, 1080},
	{2560, 1080},
	{2560, 1080},
	{2560, 1080},
	{3840, 2160},
	{3840, 2160},
	{3840, 2160},
	{3840, 2160},
	{3840, 2160},
	{4096, 2160},
	{4096, 2160},
	{4096, 2160},
	{4096, 2160},
	{4096, 2160},
	{3840, 2160},
	{3840, 2160},
	{3840, 2160},
	{3840, 2160},
	{3840, 2160},
};

static INT32 g_sd1_mode = D_SD_ACCESS_DEFAULT_SPEED;

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static void init(void);
static void hdmi_detect_callback( FJ_DISP_HDMI_DETECT detect );
static void sd_inserted_callback( UCHAR ch );
static void sd_ejected_callback( UCHAR ch );
static void hdmi_setup(void);
static void hdmi_disconnect(void);
static void sd1_mount(void);
static void sd1_unmount(void);

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void Detect_Task(INT32 stacd)
{
	OS_USER_FLGPTN	flgptn = 0;
	ULONG param[4];

	init();
	
	while(1)
	{
		// wait event
		OS_User_Twai_Flg( FID_DETECT, FLG_DETECT_WAITPTN, D_OS_USER_TWF_ORW, &flgptn, D_OS_USER_TMO_FEVR );
		OS_User_Clr_Flg( FID_DETECT, ~FLG_DETECT_WAITPTN );
		
		memset(param, 0, sizeof(param));

		// HDMI Detect
		if( IS_FLGPTN_EQUAL( flgptn, FLG_DETECT_HDMI_SETUP ) ){
			hdmi_setup();

			if( gID_HDMI != 0xFF ) {
				Bootup_Image_Display();
				#ifdef CO_HDMI_COMPLIANCE_TEST
				FJ_Show_BackColor((FJ_DISP_TYPE)Cate_Get_Disp_If(), FALSE);
				#endif
			}
			param[0] = D_STATUS_INSERTED;
			param[1] = g_hdmi_vid_reso[g_select_video_id].width;
			param[2] = g_hdmi_vid_reso[g_select_video_id].height;
			OS_IF_Rtos_Event_Notify(E_CPU_IF_COMMAND_SET_NOTIFY_EVENT_STATUS_CHANGED, E_CPU_IF_COMMAND_NOTIFY_EVENT_HDMI_STATUS_CHANGED, param);
		}
		
		// HDMI Not Detect
		if( IS_FLGPTN_EQUAL( flgptn, FLG_DETECT_HDMI_UNSET ) ){
			hdmi_disconnect();
			param[0] = D_STATUS_REMOVED;
			OS_IF_Rtos_Event_Notify(E_CPU_IF_COMMAND_SET_NOTIFY_EVENT_STATUS_CHANGED, E_CPU_IF_COMMAND_NOTIFY_EVENT_HDMI_STATUS_CHANGED, param);
		}
		
		// SD1 Inserted
		if( IS_FLGPTN_EQUAL( flgptn, FLG_DETECT_SD1_MOUNT ) ){
			sd1_mount();
			param[0] = D_STATUS_INSERTED;
			OS_IF_Rtos_Event_Notify(E_CPU_IF_COMMAND_SET_NOTIFY_EVENT_STATUS_CHANGED, E_CPU_IF_COMMAND_NOTIFY_EVENT_SD_STATUS_CHANGED, param);
		}
		
		// SD1 Ejected
		if( IS_FLGPTN_EQUAL( flgptn, FLG_DETECT_SD1_UNMOUNT ) ){
			sd1_unmount();
			param[0] = D_STATUS_REMOVED;
			OS_IF_Rtos_Event_Notify(E_CPU_IF_COMMAND_SET_NOTIFY_EVENT_STATUS_CHANGED, E_CPU_IF_COMMAND_NOTIFY_EVENT_SD_STATUS_CHANGED, param);
		}
	}
}

VOID hdmi_get_video_id_reso(UINT32 *width, UINT32 *height)
{
	*width = g_hdmi_vid_reso[g_select_video_id].width;
	*height = g_hdmi_vid_reso[g_select_video_id].height;
}

FLOAT hdmi_get_video_id_fps( VOID )
{
	return g_hdmi_fps;
}

INT32 Linux_Get_SD1( VOID )
{
	ULONG		clock;
	UCHAR		wp_status;
	
	// RTOS Release SD1
	sd1_unmount();
	
	FJ_SD_GetCardInfo(D_SDCH1, &g_sd1_mode, &clock, &wp_status);

	if ( g_sd1_mode != D_SD_ACCESS_UHS2 ) {
		/* UHS2 clock on */
		Media_Custom_SD1_Start_Clock();
		OS_User_Dly_Tsk(1);
	}
	
	return OK;
}

INT32 Linux_Rel_SD1( VOID )
{
	Dd_GIC_Ctrl(E_DD_GIC_INTID_SD_CH2_UHS2_INT, 1, D_DD_GIC_PRI14, 0x1);
	OS_User_Dly_Tsk(1);
	Dd_GIC_Ctrl(E_DD_GIC_INTID_SD_CH2_UHS1_INT, 1, D_DD_GIC_PRI14, 0x1);
	OS_User_Dly_Tsk(1);
	
	if ( g_sd1_mode != D_SD_ACCESS_UHS2 ) {
		/* UHS2 clock off */
		/* Restore SD Mount */
		Media_Custom_SD1_Stop_Clock();
		OS_User_Dly_Tsk(1);
		
		MWSDIO1_SetResetFlag(0);
		OS_User_Dly_Tsk(1);
		
		// Reset SD1 Regist
		MWSDIO1_Set_Signal_Polarity(D_SD1_SIGNAL_WP, D_SD1_NOT_INV);
		OS_User_Dly_Tsk(1);
	}
	
	sd1_mount();
	
	return OK;
}

VOID Hdmi_ForceChangeVid( UINT32 vid)
{
	FJ_ERR_CODE				ercd;
	UINT32					vic_num;
	BOOL					found = FALSE;

	UCHAR					disp_id = gID_HDMI;
	FJ_DISP_HDMI_SINK_INFO	sink_info;
	
	if((disp_id != 0xFF) && ((FJ_DISP_VIDEO_ID)vid != FJ_DISP_VIDEO_ID_NONE)){
		gID_HDMI= 0xFF;
		// HDMI Suspend
		FJ_Disp_Video_Disable( D_FJ_DISP_ID_HDMI );
		FJ_Disp_HDMI_Stop_Sync(disp_id);
		FJ_Disp_Close( disp_id );

		// HDMI Resume
		// open
		ercd = FJ_Disp_Open( FJ_DISP_TYPE_HDMI, &disp_id );
		if( ercd ){
			UPRINTF_ERR(ercd);
			return;
		}
		if( disp_id != D_FJ_DISP_ID_HDMI ){
			UPRINTF_ERR(ercd);
			return;
		}
		// Get sink information
		ercd = FJ_Disp_HDMI_Get_Sink( disp_id, &sink_info );
		if( ercd ){
			UPRINTF_ERR(ercd);
			FJ_Disp_Close( disp_id );
			return;
		}
		// Reset Video Information from EDID
		FJ_Disp_HDMI_Set_VicInfo(disp_id, NULL);
			
		for( vic_num = 0; vic_num < sink_info.vic_info_num; vic_num++ ){
			if (sink_info.vic_info[vic_num].video_id == (FJ_DISP_VIDEO_ID)vid) {
				g_select_video_id = (FJ_DISP_VIDEO_ID)vid;
				found = TRUE;
				break;
			}
		}
		
		if( found ){
			UPRINTF("hdmi video id:%d\n", g_select_video_id);
			UPRINTF("hdmi width:%d, height:%d\n", g_hdmi_vid_reso[g_select_video_id].width, g_hdmi_vid_reso[g_select_video_id].height);
					
			g_hdmi_fps = (FLOAT)sink_info.vic_info[vic_num].pixel_freq * 10000.0 / sink_info.vic_info[vic_num].h_total / sink_info.vic_info[vic_num].v_total;
			UPRINTF("hdmi fps:%.2f\n", g_hdmi_fps );
		}
		else{
			UPRINTF("HDMI Target format not supported!\n");
			FJ_Disp_Close( disp_id );
			return;
		}
		
		// Set Video Information from EDID
		FJ_Disp_HDMI_Set_VicInfo(disp_id, (FJ_DISP_HDMI_VIC_INFO*)&sink_info.vic_info[vic_num]);
			
		// Configure Video format
		ercd = FJ_Disp_HDMI_Config_Video( disp_id, g_select_video_id, FJ_DISP_HDMI_FREQ_CTRL_INACTIVE );
		if( ercd ){
			UPRINTF_ERR(ercd);
			FJ_Disp_Close( disp_id );
			return;
		}
		// Configure Audio format
		ercd = FJ_Disp_HDMI_Config_Audio( disp_id, 48000 );
		if( ercd ){
			UPRINTF_ERR(ercd);
			FJ_Disp_Close( disp_id );
			return;
		}
		// Start HDMI communication w/ Synchronous
		ercd = FJ_Disp_HDMI_Start_Sync( disp_id );
		if( ercd ){
			UPRINTF_ERR(ercd);
			FJ_Disp_Close( disp_id );
			return;
		}
		ercd = FJ_Disp_Video_Enable( disp_id );
		if( ercd ){
			UPRINTF_ERR(ercd);
			FJ_Disp_Close( disp_id );
			return;
		}

		gID_HDMI = disp_id;
	}
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static void init(void)
{
	FJ_ERR_CODE						ercd;
	FJ_DISP_HDMI_DETECT_SETTING		hdmi_detect = {
		.check_signal  = FJ_DISP_HDMI_CHECK_SIGNAL_HPD_ONLY,
		.interval_ms  = 1,											//  1 for CO_RS_HDMI_COMPLIANCE_TEST
		.detect_count = 50,											//  5 for CO_RS_HDMI_COMPLIANCE_TEST
		.check_count  = 100,										// 10 for CO_RS_HDMI_COMPLIANCE_TEST
	};
	
	// HDMI
	ercd = FJ_Disp_HDMI_Set_Detect_Callback( hdmi_detect_callback, &hdmi_detect );
	UPRINTF_ERR(ercd);
	
	// SD1
	
	sd1_mount();
	ercd = FJ_SD_Set_Event_CB( D_SDCH1, FJ_MEDIA_SD_CB_INSERTED, sd_inserted_callback );
	UPRINTF_ERR(ercd);
	ercd = FJ_SD_Set_Event_CB( D_SDCH1, FJ_MEDIA_SD_CB_EJECTED,  sd_ejected_callback );
	
	UPRINTF_ERR(ercd);
}


static void hdmi_detect_callback( FJ_DISP_HDMI_DETECT detect )
{
	if( detect == FJ_DISP_HDMI_DETECT_CONNECTED ){
		OS_User_Set_Flg( FID_DETECT, FLG_DETECT_HDMI_SETUP );
	}else{
		OS_User_Set_Flg( FID_DETECT, FLG_DETECT_HDMI_UNSET );
	}
}


static void sd_inserted_callback( UCHAR ch )
{
	if( ch == D_SDCH1 ){
		OS_User_Set_Flg( FID_DETECT, FLG_DETECT_SD1_MOUNT );
	}
}


static void sd_ejected_callback( UCHAR ch )
{
	if( ch == D_SDCH1 ){
		OS_User_Set_Flg( FID_DETECT, FLG_DETECT_SD1_UNMOUNT );
	}
}

static void hdmi_setup(void)
{
	FJ_ERR_CODE							ercd;
	FJ_DISP_HDMI_SPD_INFO				spd_info;
	FJ_DISP_HDMI_VENDOR_SPEC_INFO		vendor_spec_info;
	FJ_DISP_HDMI_SINK_INFO				sink_info;
	FJ_DISP_HDMI_DEV_VENDOR_PRODUCT_ID	vendor_product_id;
	FJ_DISP_VIDEO_ID		target_vic_list[] = {
										FJ_DISP_VIDEO_ID_2160p_3840,	// 97
										FJ_DISP_VIDEO_ID_2160p50_3840,  // 96
										FJ_DISP_VIDEO_ID_2160p30_3840,	// 95
										FJ_DISP_VIDEO_ID_2160p25_3840,	// 94
										FJ_DISP_VIDEO_ID_2160p24_3840,	// 93
										FJ_DISP_VIDEO_ID_1080p,			// 16
										FJ_DISP_VIDEO_ID_1080p50,		// 31
										FJ_DISP_VIDEO_ID_1080p30,		// 34
										FJ_DISP_VIDEO_ID_1080p25,		// 33
										FJ_DISP_VIDEO_ID_1080i,			// 5
										FJ_DISP_VIDEO_ID_1080i25,		// 20
										FJ_DISP_VIDEO_ID_720p,			// 4
										FJ_DISP_VIDEO_ID_720p50,		// 19
										FJ_DISP_VIDEO_ID_720p30,		// 62
										FJ_DISP_VIDEO_ID_720p25,		// 61
										FJ_DISP_VIDEO_ID_576p,			// 17
										FJ_DISP_VIDEO_ID_576pH,			// 18
										FJ_DISP_VIDEO_ID_480p,			// 2
										FJ_DISP_VIDEO_ID_DMT0659,		// 1
									};
	UINT32					vic_num;
	UINT32					list_num;
	BOOL					found = FALSE;
	FJ_DISP_VIDEO_ID		vid = 0;
	UCHAR					list_num_max = 0;
	
	UCHAR	disp_id;
//	INT32	cnt;
	
	gID_HDMI= 0xFF;
	// open
	ercd = FJ_Disp_Open( FJ_DISP_TYPE_HDMI, &disp_id );
	if( ercd ){
		UPRINTF_ERR(ercd);
		return;
	}
	if( disp_id != D_FJ_DISP_ID_HDMI ){
		UPRINTF_ERR(ercd);
		return;
	}
	// SPD InfoFrame
	memcpy( spd_info.vendor_id, "TESTVID1", sizeof(spd_info.vendor_id) );
	memcpy( spd_info.product_code, "TESTPRODUCTCODE1", sizeof(spd_info.product_code) );
	spd_info.device_type = FJ_DISP_HDMI_SPD_DEV_TYPE_DVC;
	ercd = FJ_Disp_HDMI_Set_SPD( disp_id, &spd_info );
	if( ercd ){
		UPRINTF_ERR(ercd);
		FJ_Disp_Close( disp_id );
		return;
	}
	// Vendor Specific (HVS) InfoFrame
	vendor_spec_info.video_format = FJ_DISP_HDMI_VENDOR_SPEC_VIDEO_FORMAT_EX;
	vendor_spec_info.vic =FJ_DISP_HDMI_VENDOR_SPEC_VIC_1_4K2K_30HZ;
	vendor_spec_info.structure_3d =FJ_DISP_HDMI_VENDOR_SPEC_3D_STRUCTURE_SIDE_BY_SIDE_FULL;
	ercd = FJ_Disp_HDMI_Set_Vendor_Specific( disp_id, &vendor_spec_info );
	if( ercd ){
		UPRINTF_ERR(ercd);
		FJ_Disp_Close( disp_id );
		return;
	}
	// Get sink information
	ercd = FJ_Disp_HDMI_Get_Sink( disp_id, &sink_info );
	if( ercd ){
		UPRINTF_ERR(ercd);
		FJ_Disp_Close( disp_id );
		return;
	}
#if 0 // print	
{
	USHORT cnt;
	for( cnt = 0; cnt < sink_info.vic_info_num; cnt++ ){
		UPRINTF("  VIC[%d]:video_id        = %d\n", cnt, sink_info.vic_info[cnt].video_id );
		UPRINTF("  VIC[%d]:h_active        = %d\n", cnt, sink_info.vic_info[cnt].h_active );
		UPRINTF("  VIC[%d]:v_active        = %d\n", cnt, sink_info.vic_info[cnt].v_active );
		UPRINTF("  VIC[%d]:h_total         = %d\n", cnt, sink_info.vic_info[cnt].h_total );
		UPRINTF("  VIC[%d]:h_blank         = %d\n", cnt, sink_info.vic_info[cnt].h_blank );
		UPRINTF("  VIC[%d]:v_total         = %d\n", cnt, sink_info.vic_info[cnt].v_total );
		UPRINTF("  VIC[%d]:v_blank         = %d\n", cnt, sink_info.vic_info[cnt].v_blank );
		UPRINTF("  VIC[%d]:h_sync_offset   = %d\n", cnt, sink_info.vic_info[cnt].h_sync_offset );
		UPRINTF("  VIC[%d]:v_sync_offset   = %d\n", cnt, sink_info.vic_info[cnt].v_sync_offset );
		UPRINTF("  VIC[%d]:h_pulse_width   = %d\n", cnt, sink_info.vic_info[cnt].h_pulse_width );
		UPRINTF("  VIC[%d]:v_pulse_width   = %d\n", cnt, sink_info.vic_info[cnt].v_pulse_width );
		UPRINTF("  VIC[%d]:h_image_size    = %d\n", cnt, sink_info.vic_info[cnt].h_image_size );
		UPRINTF("  VIC[%d]:v_image_size    = %d\n", cnt, sink_info.vic_info[cnt].v_image_size );
		UPRINTF("  VIC[%d]:pixel_freq      = %d\n", cnt, sink_info.vic_info[cnt].pixel_freq );
		UPRINTF("  VIC[%d]:format_flags    = %d\n", cnt, sink_info.vic_info[cnt].format_flags );
		UPRINTF("  VIC[%d]:h_sync_polarity = %d\n", cnt, (sink_info.vic_info[cnt].format_flags & FJ_DISP_HDMI_FLAG_BIT_MAP_HSYNC) >> 1 );
		UPRINTF("  VIC[%d]:v_sync_polarity = %d\n", cnt, (sink_info.vic_info[cnt].format_flags & FJ_DISP_HDMI_FLAG_BIT_MAP_VSYNC) >> 2 );
		UPRINTF("  VIC[%d]:interlace       = %d\n", cnt, (sink_info.vic_info[cnt].format_flags & FJ_DISP_HDMI_FLAG_BIT_MAP_INTERLACE) >> 7 );
		UPRINTF("  VIC[%d]:phy_index       = %d\n", cnt, sink_info.vic_info[cnt].phy_index );
	}
}
#endif
	// Reset Video Information from EDID
	FJ_Disp_HDMI_Set_VicInfo(disp_id, NULL);
	
	// Get sink Vendor ID and Product Code
	ercd = FJ_Disp_HDMI_Get_DeviceVendorProductID( disp_id, &vendor_product_id );
	if( ercd ){
		UPRINTF_ERR(ercd);
		FJ_Disp_Close( disp_id );
		return;
	}
#if 0 // print
	UPRINTF( "  vendor_id         = [%c][%c][%c]\n", vendor_product_id.vendor_id[0], vendor_product_id.vendor_id[1], vendor_product_id.vendor_id[2] );
	UPRINTF( "  product_code      = [0x%02X][0x%02X]\n", vendor_product_id.product_code[0], vendor_product_id.product_code[1] );
	UPRINTF( "  serial_id         = [0x%02X][0x%02X][0x%02X][0x%02X]\n", vendor_product_id.serial_id[0], vendor_product_id.serial_id[1], vendor_product_id.serial_id[2], vendor_product_id.serial_id[3]);
	UPRINTF( "  manufacture_model = [0x%02X][0x%02X]\n", vendor_product_id.manufacture_model[0], vendor_product_id.manufacture_model[1] );
#endif

	vid = (FJ_DISP_VIDEO_ID)Cate_Get_HDMI_VID_SEL();
	if( vid == 0 ) {
		// auto,  scan the list
		list_num_max = sizeof(target_vic_list) / sizeof(FJ_DISP_VIDEO_ID);
	} else {
		// specific VID selected, list num only 1
		list_num_max = 1;
	}

	UPRINTF("HDMI VID support numner:%d\n", sink_info.vic_info_num);
	for( vic_num = 0; vic_num < sink_info.vic_info_num; vic_num++ ){
		UPRINTF("HDMI VID:%d support\n", sink_info.vic_info[vic_num].video_id);
	}

	for( list_num = 0; list_num < list_num_max; list_num++ ) {
		for( vic_num = 0; vic_num < sink_info.vic_info_num; vic_num++ ){
			if( vid == 0) {
				if (sink_info.vic_info[vic_num].video_id == target_vic_list[list_num] ){
					g_select_video_id = sink_info.vic_info[vic_num].video_id;
					found = TRUE;
					break;
				}
			} else if (sink_info.vic_info[vic_num].video_id == vid) {
				g_select_video_id = vid;
				found = TRUE;
				break;
			}
		}
		if( found ){
			UPRINTF("hdmi video id:%d\n", g_select_video_id);
			UPRINTF("hdmi width:%d, height:%d\n", g_hdmi_vid_reso[g_select_video_id].width, g_hdmi_vid_reso[g_select_video_id].height);
			
			g_hdmi_fps = (FLOAT)sink_info.vic_info[vic_num].pixel_freq * 10000.0 / sink_info.vic_info[vic_num].h_total / sink_info.vic_info[vic_num].v_total;
			UPRINTF("hdmi fps:%.2f\n", g_hdmi_fps );
			break;
		}
	}

	if( g_select_video_id == FJ_DISP_VIDEO_ID_NONE ){
		UPRINTF("HDMI Target format not supported!\n");
		FJ_Disp_Close( disp_id );
		return;
	}
	else{
		UPRINTF("HDMI Target VIC[%d]\n", g_select_video_id);
	}		

	// Set Video Information from EDID
	FJ_Disp_HDMI_Set_VicInfo(disp_id, (FJ_DISP_HDMI_VIC_INFO*)&sink_info.vic_info[vic_num]);
	
	// Configure Video format
	ercd = FJ_Disp_HDMI_Config_Video( disp_id, g_select_video_id, FJ_DISP_HDMI_FREQ_CTRL_INACTIVE );
	if( ercd ){
		UPRINTF_ERR(ercd);
		FJ_Disp_Close( disp_id );
		return;
	}
	// Configure Audio format
	ercd = FJ_Disp_HDMI_Config_Audio( disp_id, 48000 );
	if( ercd ){
		UPRINTF_ERR(ercd);
		FJ_Disp_Close( disp_id );
		return;
	}
	// Start HDMI communication w/ Synchronous
	ercd = FJ_Disp_HDMI_Start_Sync( disp_id );
	if( ercd ){
		UPRINTF_ERR(ercd);
		FJ_Disp_Close( disp_id );
		return;
	}

	gID_HDMI = disp_id;
}

static void hdmi_disconnect(void)
{
	UCHAR	disp_id;
	
	UPRINTF("HDMI Disconnect (id=%d)\n", gID_HDMI );
	
	if(gID_HDMI != 0xFF){
		disp_id = gID_HDMI;
		
		FJ_Disp_Video_Disable( D_FJ_DISP_ID_HDMI );
		FJ_Disp_HDMI_Stop_Sync(disp_id);
		FJ_Disp_Close( disp_id );
		
		gID_HDMI= 0xFF;
	}
}


static void sd1_mount(void)
{
	FJ_ERR_CODE		ercd;
	UCHAR			detect;

#ifdef CO_DCF_EXIF_ON
	USHORT			dirCnt;
	CHAR			obj_name[6];
#endif	// CO_DCF_EXIF_ON

	ercd = FJ_SD_Init(D_SDCH1);
	UPRINTF_ERR(ercd);
	
	ercd = FJ_Get_MediaDetect(D_SDCH1, &detect);
	UPRINTF_ERR(ercd);
	
	if( detect == 0 ){
		sd1_unmount();
		return;//?
	}
	
	ercd = FJ_SD_Card_Init(D_SDCH1);
	UPRINTF_ERR(ercd);
	
	ercd = FJ_SetParam(FJ_PARA_EXTERNAL_MEDIA, 2);
	UPRINTF_ERR(ercd);
	
	ercd = FJ_Mount(FJ_MEDIA_SD1, D_USER_SD1_DRIVE);
	UPRINTF_ERR(ercd);
	
#ifdef CO_DCF_EXIF_ON
	Cate_Get_Dcf_Dir_Name( obj_name );
	ercd = FJ_SetDCFFreeChars( 1, FJ_DIR_SET_ONLY, obj_name );
	UPRINTF_ERR(ercd);

	Cate_Get_Dcf_File_Name( obj_name );
	ercd = FJ_SetDCFFreeChars( 0, FJ_FILE_SET_ONLY, obj_name );
	UPRINTF_ERR(ercd);

	// setup DCF
	ercd = FJ_SetDCFFreeChars( 0, FJ_FILE_SET_ALLOWED, ((CHAR*)"JPG\0RAW\0YUV\0MP4\0") );
	UPRINTF_ERR(ercd);
	
	ercd = FJ_DcfdCreate();
	UPRINTF_ERR(ercd);
	
	if (ercd == FJ_ERR_OK){
		ercd = FJ_DirectDcfDirNum(&dirCnt);
		UPRINTF_ERR(ercd);
		
		if( 0 == dirCnt ){
			; /* empty in DCIM directory */
		}
		else {
			/* Set last directory to current directory */
			FJ_SetCurrentDirectory( dirCnt );
		}
	}
#endif	// CO_DCF_EXIF_ON
}


static void sd1_unmount(void)
{
	FJ_ERR_CODE		ercd;
#ifdef CO_DCF_EXIF_ON
	USHORT			dirCnt;
	CHAR			obj_name[6];
#endif	// CO_DCF_EXIF_ON
	CHAR			drive[6];
	
	ercd = FJ_UnMount(D_USER_SD1_DRIVE);
	UPRINTF_ERR(ercd);

	/* set used media */
	Cate_Get_DCF_UserDrive((CHAR *)drive);
	ercd = FJ_Fs_Chdir((CHAR *)drive);
	UPRINTF_ERR(ercd);

#ifdef CO_DCF_EXIF_ON
	Cate_Get_Dcf_Dir_Name( obj_name );
	ercd = FJ_SetDCFFreeChars( 1, FJ_DIR_SET_ONLY, obj_name );
	UPRINTF_ERR(ercd);

	Cate_Get_Dcf_File_Name( obj_name );
	ercd = FJ_SetDCFFreeChars( 0, FJ_FILE_SET_ONLY, obj_name );
	UPRINTF_ERR(ercd);

	// setup DCF
	ercd = FJ_SetDCFFreeChars( 0, FJ_FILE_SET_ALLOWED, ((CHAR*)"JPG\0RAW\0YUV\0MP4\0") );
	UPRINTF_ERR(ercd);
	
	ercd = FJ_DcfdCreate();
	UPRINTF_ERR(ercd);
	
	if (ercd == FJ_ERR_OK){
		ercd = FJ_DirectDcfDirNum(&dirCnt);
		UPRINTF_ERR(ercd);
		
		if( 0 == dirCnt ){
			; /* empty in DCIM directory */
		}
		else {
			/* Set last directory to current directory */
			FJ_SetCurrentDirectory( dirCnt );
		}
	}
#endif	// CO_DCF_EXIF_ON
	
}
