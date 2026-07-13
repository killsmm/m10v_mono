/**
 * @file		fj_disp.c
 * @brief		LCD, HDMI, Image and OSD control
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_disp.h"
#include "disp.h"
#include "mipi.h"
#include "filesystem.h"

/******************************************************************************
 Program
******************************************************************************/
/* ------------- */
/* LCD/HDMI/MIPI */
/* ------------- */
/* Open Display Instance. */
FJ_ERR_CODE FJ_Disp_Open( FJ_DISP_TYPE disp_type, UCHAR *disp_id )
{
	return fj_disp_open( disp_type, disp_id );
}

/* Close Display Instance. */
FJ_ERR_CODE FJ_Disp_Close( UCHAR disp_id )
{
	return fj_disp_close( disp_id );
}

/* ------------- */
/* LCD           */
/* ------------- */
/* Set LCD timing generator to DISP macro. */
FJ_ERR_CODE FJ_Disp_LCD_Config( UCHAR disp_id, FJ_DISP_LCD_CONFIG *lcd_config_info )
{
	return fj_disp_lcd_config( disp_id, lcd_config_info );
}	

/* Start LCD clock(LCDCK), VSYNC output, image layer trigger. */
FJ_ERR_CODE FJ_Disp_LCD_Start_Sync( UCHAR disp_id )
{
	return fj_disp_lcd_start_sync( disp_id );
}

/* Stop LCD clock(LCDCK), VSYNC output, image layer trigger. */
FJ_ERR_CODE FJ_Disp_LCD_Stop_Sync( UCHAR disp_id )
{
	return fj_disp_lcd_stop_sync( disp_id );
}

FJ_DISP_LCD_CONNECT_STATE FJ_Disp_LCD_Get_State( VOID )
{
	return fj_disp_lcd_get_status();
}

/* ------------------- */
/* HDMI				   */
/* ------------------- */
/* Regist callback function pointer for hotplug detection. */
FJ_ERR_CODE FJ_Disp_HDMI_Set_Detect_Callback( DISP_HDMI_CALLBACK interrupt_callback, FJ_DISP_HDMI_DETECT_SETTING const* const detect_setting )
{
	return fj_disp_hdmi_set_detect_callback( interrupt_callback, detect_setting );
}

/* Get HDMI Detect status. */
FJ_DISP_HDMI_DETECT FJ_Disp_HDMI_Get_Detect_Status( VOID )
{
	return fj_disp_hdmi_get_detect_status();
}

/* Get HDMI Sinc information. */
FJ_ERR_CODE FJ_Disp_HDMI_Get_Sink( UCHAR disp_id, FJ_DISP_HDMI_SINK_INFO* sink_info )
{
	return fj_disp_hdmi_get_sink( disp_id, sink_info );
}

/* Get HDMI Sinc Vendor ID and Product Code. */
FJ_ERR_CODE FJ_Disp_HDMI_Get_DeviceVendorProductID( UCHAR disp_id, FJ_DISP_HDMI_DEV_VENDOR_PRODUCT_ID* vendor_product_id )
{
	return fj_disp_hdmi_get_devicevendorproductid( disp_id, vendor_product_id );
}

FJ_ERR_CODE FJ_Disp_HDMI_Set_VicInfo( UCHAR disp_id, FJ_DISP_HDMI_VIC_INFO* vic_info )
{
	return fj_disp_hdmi_set_vic_info( disp_id, vic_info );
}

/* Set HDMI Source Product Description (SPD) InfoFrame. */
FJ_ERR_CODE FJ_Disp_HDMI_Set_SPD( UCHAR disp_id, FJ_DISP_HDMI_SPD_INFO* spd_info )
{
	return fj_disp_hdmi_set_spd( disp_id, spd_info );
}

/* Set HDMI Vendor Specific InfoFrame. */
FJ_ERR_CODE FJ_Disp_HDMI_Set_Vendor_Specific( UCHAR disp_id, FJ_DISP_HDMI_VENDOR_SPEC_INFO* vendor_spec_info )
{
	return fj_disp_hdmi_set_vendor_specific( disp_id, vendor_spec_info );
}

/* Configure Video format. */
FJ_ERR_CODE FJ_Disp_HDMI_Config_Video( UCHAR disp_id, FJ_DISP_VIDEO_ID video_id, UCHAR freq_ctrl )
{
	return fj_disp_hdmi_config_video( disp_id, video_id, freq_ctrl );
}

/* Configure Video format. */
FJ_ERR_CODE FJ_Disp_HDMI_Config_Audio( UCHAR disp_id, ULONG samp_freq )
{
	return fj_disp_hdmi_config_audio( disp_id, samp_freq );
}

/* HDMI output start. */
FJ_ERR_CODE FJ_Disp_HDMI_Start_Sync( UCHAR disp_id )
{
	return fj_disp_hdmi_start_sync( disp_id );
}

/* HDMI output stop. */
FJ_ERR_CODE FJ_Disp_HDMI_Stop_Sync( UCHAR disp_id )
{
	return fj_disp_hdmi_stop_sync( disp_id );
}

/* ------------------- */
/* MIPI				   */
/* ------------------- */
/* Set MIPI timing generator to DISP macro. */
FJ_ERR_CODE FJ_Disp_MIPI_Config_Video( UCHAR disp_id, FJ_DISP_LCD_CONFIG *lcd_config_info )
{
	return fj_disp_mipi_config_video( disp_id, lcd_config_info );
}

/* Start MIPI clock(MIFCK), VSYNC output, image layer trigger. */
FJ_ERR_CODE FJ_Disp_MIPI_Start_Sync( UCHAR disp_id )
{
	return fj_disp_mipi_start_sync( disp_id );
}

/* Stop MIPI clock(MIFCK), VSYNC output, image layer trigger. */
FJ_ERR_CODE FJ_Disp_MIPI_Stop_Sync( UCHAR disp_id )
{
	return fj_disp_mipi_stop_sync( disp_id );
}

/* ------------- */
/* Image Layer   */
/* ------------- */
/* Sets the Disp instance window setting. */
FJ_ERR_CODE	FJ_Set_Display_Window( UCHAR disp_id, FJ_DISP_WINDOW_CONFIG* window_config )
{
	return fj_set_display_window( disp_id, window_config );
}

/* Gets the Disp instance window setting. */
FJ_ERR_CODE	FJ_Get_Display_Window( UCHAR disp_id, FJ_DISP_WINDOW_CONFIG* window_config )
{
	return fj_get_display_window( disp_id, window_config );
}

/* Render image layer. */
FJ_ERR_CODE FJ_Disp_Video_Render( UCHAR disp_id, FJ_DISP_YCC_ADDR* ycc_addr, UINT32 global_width )
{
	return fj_disp_video_render( disp_id, ycc_addr, global_width );
}

/* Set display image layer rotation. */
FJ_ERR_CODE FJ_Disp_Video_Set_Rotate( UCHAR disp_id, FJ_DISP_ROTATE_DEGREE rotate_degree )
{
	return fj_disp_video_set_rotate( disp_id, rotate_degree );
}

/* Get display image layer rotation. */
FJ_ERR_CODE FJ_Disp_Video_Get_Rotate( UCHAR disp_id, FJ_DISP_ROTATE_DEGREE* rotate_degree )
{
	return fj_disp_video_get_rotate( disp_id, rotate_degree );
}

/* Enable image layer. */
FJ_ERR_CODE FJ_Disp_Video_Enable( UCHAR disp_id )
{
	return fj_disp_video_enable( disp_id );
}

/* Disable image layer. */
FJ_ERR_CODE FJ_Disp_Video_Disable( UCHAR disp_id )
{
	return fj_disp_video_disable( disp_id );

}
/* Draw a logo at startup. */
FJ_ERR_CODE FJ_Disp_Show_Boot_Image( const FJ_DISP_IMAGE_CONFIG* config_info )
{
	return fj_disp_show_boot_image( config_info );
}

/* ------------- */
/* OSD Layer   */
/* ------------- */
/* Set the configuration of drawing OSD. */
FJ_ERR_CODE FJ_Disp_OSD_Set_Config( UCHAR disp_id, FJ_DISP_LAYERID_TYPE layer_id, FJ_DISP_OSD_CONFIG* config_info )
{
	return fj_disp_osd_set_config( disp_id, layer_id, config_info );
}

/* BaseFW will enable the OSD of specified layer. */
FJ_ERR_CODE FJ_Disp_OSD_Enable( UCHAR disp_id, FJ_DISP_LAYERID_TYPE layer_id )
{
	return fj_disp_osd_enable( disp_id, layer_id );
}

/* BaseFW will disconnect the OSD of specified layer. */
FJ_ERR_CODE FJ_Disp_OSD_Disable( UCHAR disp_id, FJ_DISP_LAYERID_TYPE layer_id )
{
	return fj_disp_osd_disable( disp_id, layer_id );
}

/* Rotate and Copy the OSD data to display in the OSD area. */
FJ_ERR_CODE FJ_Disp_OSD_Rotate_Copy( UCHAR disp_id, FJ_DISP_LAYERID_TYPE layer_id, FJ_DISP_OSD_RECT_INFO* rect_info, FJ_DISP_OSD_BUFF_ADDR* buff_addr )
{
	return fj_disp_osd_rotate_copy( disp_id, layer_id, rect_info, buff_addr );
}

/* Render OSD frame. */
FJ_ERR_CODE FJ_Disp_OSD_Render( UCHAR disp_id, FJ_DISP_LAYERID_TYPE layer_id, UINT32 disp_buff_addr )
{
	return fj_disp_osd_render( disp_id, layer_id, disp_buff_addr );
}

/* ------------- */
/* Write Channel */
/* ------------- */
/* Set config for Write Channel. */
FJ_ERR_CODE FJ_Disp_Write_Channel_Set_Config( const FJ_DWCH_CONFIG *const dwch_config )
{
	return fj_disp_write_channel_set_config( dwch_config );
}

/* Draw Write Channel. */
FJ_ERR_CODE FJ_Disp_Write_Channel_Draw( const FJ_DWCH_IMG_INFO* const img_info, E_OSDLayer layer_id )
{
	return fj_disp_write_channel_draw( img_info, layer_id );
}


// CUSTOMER_M8M_NOT_USED
// from fj_hdmi.c
/******************************************************************************
 Include
******************************************************************************/
#include "fj_disp.h"
#include "hdmi.h"

/******************************************************************************
 Program
******************************************************************************/

VOID FJ_HDMI_Init( HDMI_CALLBACK interrupt_callback )
{
	fj_hdmi_init(interrupt_callback);
}
#if 0
FJ_HDMI_HPD FJ_HDMI_Detect( VOID )
#else
FJ_ERR_CODE FJ_HDMI_Detect( VOID )
#endif
{
	return fj_hdmi_detect();
}

FJ_ERR_CODE FJ_HDMI_Set_OutFormat( UCHAR format )
{
	return fj_hdmi_set_outformat(format);
}

FJ_ERR_CODE FJ_HDMI_Connect_HDMI( UINT32 video_format )
{
	return fj_hdmi_connect_hdmi(video_format);
}

FJ_ERR_CODE FJ_HDMI_Disconnect_HDMI( VOID )
{
	return fj_hdmi_disconnect_hdmi();
}

VOID FJ_HDMI_Start_Sync( VOID )
{
	fj_hdmi_start_sync();
}

VOID FJ_HDMI_Stop_Sync( VOID )
{
	fj_hdmi_stop_sync();
}

FJ_HDMI_HPD FJ_HDMI_Get_EDID( FJ_HDMI_DTD_MAN** dtd_addr, UCHAR edid_data_block_tag_flags )
{
	return fj_hdmi_get_edid( dtd_addr, edid_data_block_tag_flags );
}

FJ_ERR_CODE FJ_HDMI_Get_DTD_No( UINT32 video_format, UINT32* dtd_no )
{
	return fj_hdmi_get_dtd_no( video_format, dtd_no );
}

FJ_ERR_CODE FJ_HDMI_Get_Format_Tbl_Addr( UINT32 video_format, FJ_HDMI_EDID_DTD** edid_addr )
{
	return fj_hdmi_get_format_tbl_addr( video_format, (FJ_HDMI_EDID_DTD**)edid_addr );
}

FJ_ERR_CODE FJ_HDMI_Set_Ctrl_Common( FJ_HDMI_CTRL_COMMON const *const ctrl )
{
	return fj_hdmi_set_ctrl_common( ctrl );
}

FJ_ERR_CODE FJ_HDMI_Get_Ctrl_Common( FJ_HDMI_CTRL_COMMON *const ctrl )
{
	return fj_hdmi_get_ctrl_common( ctrl );
}

FJ_ERR_CODE FJ_HDMI_Set_Ctrl_Video( FJ_HDMI_CTRL_VIDEO const *const ctrl )
{
	return fj_hdmi_set_ctrl_video( ctrl );
}

FJ_ERR_CODE FJ_HDMI_Get_Ctrl_Video( FJ_HDMI_CTRL_VIDEO *const ctrl )
{
	return fj_hdmi_get_ctrl_video( ctrl );
}

FJ_ERR_CODE FJ_HDMI_Set_Ctrl_Audio( FJ_HDMI_CTRL_AUDIO const *const ctrl )
{
	return fj_hdmi_set_ctrl_audio( ctrl );
}

FJ_ERR_CODE FJ_HDMI_Get_Ctrl_Audio( FJ_HDMI_CTRL_AUDIO *const ctrl )
{
	return fj_hdmi_get_ctrl_audio( ctrl );
}

FJ_ERR_CODE FJ_HDMI_Audio_Mute( BOOL enable )
{
	return fj_hdmi_audio_mute(enable);
}

FJ_ERR_CODE FJ_HDMI_Set_Ctrl_I2s( FJ_HDMI_CTRL_AUDIO_I2S const *const ctrl )
{
	return fj_hdmi_set_ctrl_i2s( ctrl );
}

FJ_ERR_CODE FJ_HDMI_Get_Ctrl_I2s( FJ_HDMI_CTRL_AUDIO_I2S *const ctrl )
{
	return fj_hdmi_get_ctrl_i2s( ctrl );
}

FJ_ERR_CODE FJ_HDMI_Audio_Ctrl( unsigned long samp_freq )
{
	return fj_hdmi_audio_ctrl( samp_freq );
}

VOID FJ_HDMI_Audio_Start( VOID )
{
	fj_hdmi_audio_start();
}

VOID FJ_HDMI_Audio_Stop( VOID )
{
	fj_hdmi_audio_stop();
}

VOID FJ_HDMI_Audio_Set_Start( ULONG sample_freq )
{
	fj_hdmi_audio_set_start( sample_freq);
}

FJ_ERR_CODE FJ_HDMI_Get_System_Status(FJ_HDMI_SYS_STAT* status)
{
	return fj_hdmi_get_system_status(status);
}

#if 0
FJ_ERR_CODE FJ_HDMI_CEC_Init(VOID)
{
	return fj_hdmi_cec_init();
}

VOID FJ_HDMI_CEC_Stop(VOID)
{
 	fj_hdmi_cec_stop();
}

#if 0
FJ_ERR_CODE 	FJ_HDMI_CEC_GetUserControlKeyEvent(FJ_CEC_USER_CONTROL_KEY_EVENT * const keyEvent)
{
	return fj_hdmi_cec_get_user_control_key_event(keyEvent);
}
#endif

FJ_ERR_CODE FJ_HDMI_CEC_SendMessage(FJ_CEC_OPCODE opcode, BYTE operand[],BYTE operandCount)
{
 	return fj_hdmi_cec_send_message(opcode, operand, operandCount);
}

FJ_ERR_CODE	FJ_HDMI_CEC_InactiveSource(VOID)
{
	return fj_hdmi_cec_inactive_source();
}

FJ_ERR_CODE FJ_HDMI_CEC_OneTouchPlay(VOID)
{
	return fj_hdmi_cec_one_touch_play();
}

FJ_ERR_CODE FJ_HDMI_CEC_SendPing(USHORT *pingMap)
{
	return fj_hdmi_cec_send_ping(pingMap);
}

FJ_ERR_CODE FJ_HDMI_CEC_MenuStatus(FJ_CEC_MENUSTATE menuStatue)
{
	return fj_hdmi_cec_menu_status(menuStatue);
}

#if 0
FJ_ERR_CODE FJ_HDMI_CEC_ReportPowerStatus( FJ_CEC_POWERSTATUS powerStatus)
{
	return fj_hdmi_cec_report_power_status( powerStatus);
}

FJ_ERR_CODE FJ_HDMI_CEC_GiveDevicePowerStatus(VOID)
{
	return fj_hdmi_cec_give_device_power_status();
}

FJ_ERR_CODE FJ_HDMI_CEC_RequestActiveSource(VOID)
{
	return fj_hdmi_cec_request_active_source();
}

FJ_ERR_CODE FJ_HDMI_CEC_GetCECVersion(VOID)
{
	return fj_hdmi_cec_get_cec_version();
}

FJ_ERR_CODE FJ_HDMI_CEC_MenuRequest(FJ_CEC_MENUREQ requestType)
{
	return fj_hdmi_cec_menu_request( requestType);
}

FJ_ERR_CODE FJ_HDMI_CEC_GivePhysicalAddr(FJ_CEC_LOG_ADDR dest)
{
	return fj_hdmi_cec_give_physical_addr(dest);
}
#endif

FJ_ERR_CODE FJ_HDMI_CEC_GiveDeviceVendorID(VOID)
{
	return fj_hdmi_cec_give_device_vendor_id();
}

USHORT FJ_HDMI_CEC_GetPhysicalAddress(VOID)
{
	return fj_hdmi_cec_get_physical_address();
}
#endif

FJ_ERR_CODE FJ_MIPI_Video_Ctrl( FJ_MIPI_CTRL_VIDEO* mipi_video_ctrl )
{
	return fj_mipi_video_ctrl( mipi_video_ctrl );
}

FJ_ERR_CODE FJ_MIPI_Init( UCHAR freq_ctrl )
{
	return fj_mipi_init( freq_ctrl );
}

FJ_ERR_CODE FJ_MIPI_Set_Mode( UCHAR mode )
{
	return fj_mipi_set_mode( mode );
}

FJ_ERR_CODE FJ_MIPI_Command_Ctrl( FJ_MIPI_CMD_INFO* mipi_command_info )
{
	return fj_mipi_command_ctrl( mipi_command_info );
}


// CUSTOMER_M8M_NOT_USED
// from fj_osd.c
#include "hdmi.h"
#include "mode.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
#include "../disp/src/osd.c"
#include "../disp/src/osd_comm.c"

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
///////////////////////////////////////////////////
//  Following are BASEFW_BOOT_CODE
///////////////////////////////////////////////////
/* Configures the OSD display attributes.
*/
FJ_ERR_CODE FJ_SetOSDDisplayAttr(FJ_DISP_TYPE disp_type,
								 E_OSDLayer layer_id, E_FJ_OSD_ATTR_TYPE Attr,
								 INT32 wParam1, INT32 wParam2, INT32 wParam3, INT32 wParam4)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_BOOT);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_setosddisplayattr(disp_type, layer_id, Attr, wParam1, wParam2, wParam3, wParam4);
	}

	return ret;
}

/* Set the MAIN's input size.
*/
FJ_ERR_CODE FJ_OSD_Set_Main_Input_Size(E_OSDLayer layer_id, const T_Rect* disp_rect)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_BOOT);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_set_main_input_size(layer_id, disp_rect);
	}

	return ret;
}

/* Get gain, clip, offset value setting at DISP macro.
*/
FJ_ERR_CODE FJ_OSD_Get_Clip(E_DISP_DEV disp_device, T_CLIP_CAL_DATA* cal_data)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_BOOT);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_get_clip(disp_device, cal_data);
	}

	return ret;
}

/* Set gain, clip, offset value setting at DISP macro.
*/
FJ_ERR_CODE FJ_OSD_Set_Clip(E_DISP_DEV disp_device, T_CLIP_CAL_DATA const* const cal_data)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_BOOT);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_set_clip(disp_device, cal_data);
	}

	return ret;
}

/* Set Back Ground Color.
*/
FJ_ERR_CODE FJ_Show_SetBackColor(FJ_DISP_TYPE disp_type, UCHAR r_color, UCHAR g_color, UCHAR b_color)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_BOOT);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		fj_show_setbackcolor(disp_type, r_color, g_color, b_color);
	}

	return ret;
}

/* Enable show or not display Back Ground Color.
*/
FJ_ERR_CODE FJ_Show_BackColor(FJ_DISP_TYPE disp_type, BOOL enable)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_BOOT);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_show_backcolor(disp_type, enable);
	}

	return ret;
}

/* Set Enable/Disable each Display Layer.
*/
FJ_ERR_CODE FJ_Show_Enable(E_OSDLayer layer_id, BOOL enable)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_BOOT);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_show_enable(layer_id, enable);
	}

	return ret;
}

/* Initialize all OSD layer(OSD-0,OSD-1).
*/
FJ_ERR_CODE FJ_OSD_Initialize(VOID)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_BOOT);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		fj_osd_initialize();
	}

	return ret;
}

/* Initialize OSD layer.
*/
FJ_ERR_CODE FJ_Initialize_OSD_Layer(E_OSDLayer layer_id)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_BOOT);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		fj_initialize_osd_layer(layer_id);
	}

	return ret;
}

/* Set matrix.
*/
FJ_ERR_CODE FJ_OSD_Set_Matrix(FJ_DISP_TYPE disp_type, FJ_OSD_MATRIX_KIND kind, BYTE enable, T_MATRIX_DATA const* const matrix)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_BOOT);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_set_matrix(disp_type, kind, enable, matrix);
	}

	return ret;
}

///////////////////////////////////////////////////
//  Following are BASEFW_VIEW_CODE
///////////////////////////////////////////////////
/* Set control data for LCD or HDMI.
Or set control data and hunt memory for HDMI showing.
*/
FJ_ERR_CODE FJ_OSD_Connect(E_DISP_DEV disp_device, const T_HDMI_SET *HDMI_Setting_Data)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		if(disp_device == E_DISP_DEV_LCD){
#ifdef CO_LCD_ENABLE
			// Processing is executable.
			ret = fj_osd_connect_lcd();
#else
			return FJ_ERR_NG_INPUT_PARAM;
#endif
		}
		else if(disp_device == E_DISP_DEV_HDMI){
			ret = fj_osd_connect_hdmi(HDMI_Setting_Data);
		}
		else{
			ret = FJ_ERR_NG_INPUT_PARAM;
		}
	}

	return ret;
}

/* Stop output for LCD.
Or stop output for HDMI and free the resource.
*/
FJ_ERR_CODE FJ_OSD_Disconnect(E_DISP_DEV disp_device)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		if(disp_device == E_DISP_DEV_LCD){
#ifdef CO_LCD_ENABLE
			// Processing is executable.
			fj_osd_disconnect_lcd();
#else
			return FJ_ERR_NG;
#endif
		}
		else if(disp_device == E_DISP_DEV_HDMI){
			// Processing is executable.
			fj_osd_disconnect_hdmi();
		}
		else{
			ret = FJ_ERR_NG_INPUT_PARAM;
		}
	}
	return ret;
}

/* Connect to HDMI and set control data for HDMI showing.
*/
FJ_ERR_CODE FJ_Connect_HDMI(const T_HDMI_SET* hdmi_setting_data)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_VIEW);

	if(ret == FJ_ERR_OK) {
		UINT32 video_format = FJ_HDMI_1280x720p60Hz_16_9;
			
		switch ( hdmi_setting_data->HD_resolution ) {
			case E_HD_RESOLUTION_720P_60HZ:
				video_format = FJ_HDMI_1280x720p60Hz_16_9;
				break;
			case E_HD_RESOLUTION_720P_50HZ:
				video_format = FJ_HDMI_1280x720p50Hz_16_9;
				break;
			case E_HD_RESOLUTION_1080I_60H:
				video_format = FJ_HDMI_1920x1080i60Hz_16_9;
				break;
			case E_HD_RESOLUTION_1080I_50H:
				video_format = FJ_HDMI_1920x1080i50Hz_16_9;
				break;
			case E_HD_RESOLUTION_480P_60HZ:
				video_format = FJ_HDMI_720x480p60Hz_4_3;
				break;
			case E_HD_RESOLUTION_576P_50HZ:
				video_format = FJ_HDMI_720x576p50Hz_4_3;
				break;
			case E_HD_RESOLUTION_1080P_30H:
				video_format = FJ_HDMI_1920x1080p30Hz_16_9;
				break;
			case E_HD_RESOLUTION_1080P_25H:
				video_format = FJ_HDMI_1920x1080p25Hz_16_9;
				break;
			case E_HD_RESOLUTION_1080P_60H:
				video_format = FJ_HDMI_1920x1080p60Hz_16_9;
				break;
			case E_HD_RESOLUTION_1080P_50H:
				video_format = FJ_HDMI_1920x1080p50Hz_16_9;
				break;
			case E_HD_RESOLUTION_720P_30HZ:
				video_format = FJ_HDMI_1280x720p30Hz_16_9;
				break;
			case E_HD_RESOLUTION_720P_25HZ:
				video_format = FJ_HDMI_1280x720p25Hz_16_9;
				break;
			case E_HD_RESOLUTION_2160P_25H:
				video_format = FJ_HDMI_3840x2160p25Hz_16_9;
				break;
			case E_HD_RESOLUTION_2160P_30H:
				video_format = FJ_HDMI_3840x2160p30Hz_16_9;
				break;
			default:
				return FJ_ERR_NG;
		}
		
		// Processing is executable.
		ret = fj_hdmi_connect_hdmi( video_format );
		if ( ret == FJ_ERR_OK ) {
			ret = fj_osd_connect_hdmi(hdmi_setting_data);
		}
	}

	return ret;
}

/* Set the area to draw.
*/
FJ_ERR_CODE FJ_OSD_Draw_OSD_Area(E_OSDLayer layer_id, const T_Rect drawArea[OSD_DRAW_AREA_NUM], BYTE drawAreaNum)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_BOOT);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_draw_osd_area(layer_id, drawArea, drawAreaNum);
	}

	return ret;
}

/* Sets the display state of the drawing area.
*/
FJ_ERR_CODE FJ_OSD_Set_OSD_Area_Enable(E_OSDLayer layer_id, UINT32 enable_area)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_set_osd_area_enable(layer_id, enable_area);
	}

	return ret;
}

/* Gets the display state of the drawing area.
*/
FJ_ERR_CODE FJ_OSD_Get_OSD_Area_Enable(E_OSDLayer layer_id, UINT32* enable_area)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_get_osd_area_enable(layer_id, enable_area);
	}

	return ret;
}

/* Get the area to draw buffer address.
*/
ULONG FJ_OSD_Get_OSD_Area_Addr(E_OSDLayer layer_id, UINT32 drawAreaNum, UINT32* global_width)
{
	ULONG ret_addr = 0;
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret_addr = fj_osd_get_osd_area_addr(layer_id, drawAreaNum, global_width);
	}

	return ret_addr;
}

/* Clear the drawing area.
*/
FJ_ERR_CODE FJ_Clear_Area_Buffer(E_OSDLayer layer_id, UINT32 clear_area)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_clear_area_buffer(layer_id, clear_area);
	}

	return ret;
}

#if 0	// MILB Not Use
/* Set tone correction data.
*/
FJ_ERR_CODE FJ_OSD_Set_Tone_Correction(E_OSDLayer layer_id, T_Y_CONV const *const y_conv, T_TC_DATA const *const tc_data)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_set_tone_correction(layer_id, y_conv, tc_data);
	}

	return ret;
}
#endif	// MILB Not Use

#if 0	// MILB Not Use
/* Get tone correction data.
*/
FJ_ERR_CODE FJ_OSD_Get_Tone_Correction(E_OSDLayer layer_id, T_Y_CONV* y_conv, T_TC_DATA* tc_data)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_get_tone_correction(layer_id, y_conv, tc_data);
	}

	return ret;
}
#endif	// MILB Not Use

#if 0	// MILB Not Use
/* Set tone correction table.
*/
FJ_ERR_CODE FJ_OSD_Set_Tc_Table(E_OSDLayer layer_id, T_TONE_CORRECTION_TBL const *const tone_correction)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_set_tc_table(layer_id, tone_correction);
	}

	return ret;
}
#endif	// MILB Not Use

#if 0	// MILB Not Use
/* Get tone correction table.
*/
FJ_ERR_CODE FJ_OSD_Get_Tc_Table(E_OSDLayer layer_id, T_TONE_CORRECTION_TBL* tone_correction)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_get_tc_table(layer_id, tone_correction);
	}

	return ret;
}
#endif	// MILB Not Use

/* Set inverse gamma table.
*/
FJ_ERR_CODE FJ_OSD_Set_Inverse_Gamma(E_OSDLayer layer_id, T_INVERSE_GAMMA_TBL const *const inverse_gamma)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_set_inverse_gamma(layer_id, inverse_gamma);
	}

	return ret;
}

/* Get inverse gamma table.
*/
FJ_ERR_CODE FJ_OSD_Get_Inverse_Gamma(E_OSDLayer layer_id, T_INVERSE_GAMMA_TBL* inverse_gamma)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_get_inverse_gamma(layer_id, inverse_gamma);
	}

	return ret;
}

/* Set gamma table.
*/
FJ_ERR_CODE FJ_OSD_Set_Gamma(E_OSDLayer layer_id, T_GAMMA_TBL const *const gamma)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_set_gamma(layer_id, gamma);
	}

	return ret;
}

/* Get gamma table.
*/
FJ_ERR_CODE FJ_OSD_Get_Gamma(E_OSDLayer layer_id, T_GAMMA_TBL* gamma)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_get_gamma(layer_id, gamma);
	}

	return ret;
}

/* Set enable Gamma correct or inverse-Gamma correct or gradation adjustment.
*/
FJ_ERR_CODE FJ_OSD_Set_Compensation_Table_Enable(FJ_DISP_TYPE disp_type, E_CORRECT_SELECT select, T_TBL_ACCESS_SET const *const compensation_table)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_set_compensation_table_enable(disp_type, select, compensation_table);
	}

	return ret;
}

/* Get enable Gamma correct or inverse-Gamma correct or gradation adjustment.
*/
FJ_ERR_CODE FJ_OSD_Get_Compensation_Table_Enable(FJ_DISP_TYPE disp_type, E_CORRECT_SELECT select, T_TBL_ACCESS_SET* compensation_table)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_get_compensation_table_enable(disp_type, select, compensation_table);
	}

	return ret;
}

/* Set order of the layer.
*/
FJ_ERR_CODE FJ_OSD_Set_Layer_Order(FJ_DISP_TYPE disp_type, T_ORDER_LAYER const *const order_layer)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_set_layer_order(disp_type, order_layer);
	}

	return ret;
}

/* Get order of the layer.
*/
FJ_ERR_CODE FJ_OSD_Get_Layer_Order(FJ_DISP_TYPE disp_type, T_ORDER_LAYER* order_layer)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_get_layer_order(disp_type, order_layer);
	}

	return ret;
}

/* Draw grid line.
*/
FJ_ERR_CODE FJ_OSD_Draw_Grid_Line(FJ_DISP_TYPE disp_type, T_GRID_SET const *const grid_set)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_draw_grid_line(disp_type, grid_set);
	}

	return ret;
}


/* Draw grid line.
*/
FJ_ERR_CODE FJ_OSD_Set_Grid_Line_Num(FJ_DISP_TYPE disp_type, T_GRID_NUM_SET const *const grid_num_set)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_set_grid_line_num(disp_type, grid_num_set);
	}

	return ret;
}

/* Set enable grid line.
*/
FJ_ERR_CODE FJ_OSD_Set_Grid_Line_Enable(FJ_DISP_TYPE disp_type, BOOL enable)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_set_grid_line_enable(disp_type, enable);
	}

	return ret;
}

/* Draw face frame.
*/
FJ_ERR_CODE FJ_OSD_Draw_Face_Frame(FJ_DISP_TYPE disp_type, const T_FACE_FRAME_EACH_SET face_pos[OSD_DRAW_FACE_NUM],  const T_FACE_FRAME_SET* face_set)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_draw_face_frame(disp_type, face_pos, face_set);
	}

	return ret;
}

/* Set enable face frame.
*/
FJ_ERR_CODE FJ_OSD_Set_Face_Frame_Enable(FJ_DISP_TYPE disp_type, UINT32 enable, UINT32 top_face)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_set_face_frame_enable(disp_type, enable, top_face);
	}

	return ret;
}

/* Set Color bar parameter.
*/
FJ_ERR_CODE FJ_OSD_Set_Color_Bar(FJ_DISP_TYPE disp_type, U_RGB_COLOR color[OSD_COLOR_BAR_NUM],  ULONG color_bar_num)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if( ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_set_color_bar(disp_type, color, color_bar_num);
	}

	return ret;
}

/* Set Main layer display buffer address.
*/
FJ_ERR_CODE FJ_SetMainDispBuffAddr(E_OSDLayer layer_id, ULONG addr[3])
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_setmaindispbuffaddr(layer_id, addr);
	}

	return ret;
}

/* Get Main layer display buffer address.
*/
FJ_ERR_CODE FJ_GetMainDispBuffAddr(E_OSDLayer layer_id, ULONG addr[3])
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_getmaindispbuffaddr(layer_id, addr);
	}

	return ret;
}

/* Set the area to draw display buffer address.
*/
FJ_ERR_CODE FJ_SetOsdDispBuffAddr(E_OSDLayer layer_id, UINT32 drawAreaNum, ULONG addr)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_setosddispbuffaddr(layer_id, drawAreaNum, addr);
	}

	return ret;
}

/* Get the area to draw display buffer address.
*/
FJ_ERR_CODE FJ_GetOsdDispBuffAddr(E_OSDLayer layer_id, UINT32 drawAreaNum, ULONG* addr)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_getosddispbuffaddr(layer_id, drawAreaNum, addr);
	}

	return ret;
}

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set DISP's parameter by every input image size.
@param[in]	image_type		Input image's type
@param[in]	image_format	Input image's format
@param[in]	image_global_width	Input image's global_width
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG				Parameter error.
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
FJ_ERR_CODE FJ_Set_Main_Display(E_DISP_DEV disp_device, E_FJ_IMAGE_SIZE image_type,E_FJ_IMAGE_FORMAT image_format, ULONG image_global_width)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		if(disp_device == E_DISP_DEV_LCD){
#ifdef CO_LCD_ENABLE
			// Processing is executable.
			ret = fj_set_main_display(image_type,image_format,image_global_width);
#else
			return FJ_ERR_NG;
#endif
		}
		else if(disp_device == E_DISP_DEV_HDMI){
			// Processing is executable.
			ret = fj_set_main_display_hdmi(image_type,image_format,image_global_width);
		}
		else{
			ret = FJ_ERR_NG_INPUT_PARAM;
		}
	}

	return ret;
}


/**
Set callback function for VSYNC.
@param[in] disp_type	display type.<br>
			@ref FJ_DISP_TYPE_LCD		: LCD<br>
			@ref FJ_DISP_TYPE_HDMI		: HDMI<br>
@param[in] bSetup 	  	Set(TRUE) or clear(FALSE) callback funktion.
@param[in] pcallback  	The pointer of Callback Function.  
@retval FJ_ERR_OK	  	Normal End.
@retval FJ_ERR_NG	  	Abnormal End.
*/
FJ_ERR_CODE FJ_Set_OSD_Vsync_Callback(FJ_DISP_TYPE disp_type, BOOL bSetup, VP_CALLBACK pcallback)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_set_osd_vsync_callback(disp_type, bSetup, pcallback);
	}

	return ret;
}

/* Flush drawing buffer.
*/
FJ_ERR_CODE FJ_OSD_Flush_Draw_Area(const FJ_OSD_COMM_FLUSH_DARA_AREA_INFO* switch_buffer_infor,UINT32 switch_buffer_infor_size)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_BOOT);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_flush_draw_area(switch_buffer_infor, switch_buffer_infor_size);
	}

	return ret;
}

/* Load a icon data from file to be used for OSD drawing.
*/
FJ_ERR_CODE FJ_LoadFontFile(CHAR const* const FontName)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_BOOT);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_loadfontfile(FontName);
	}

	return ret;
}

/* Clear draw buffer of OSD layer.
*/
FJ_ERR_CODE FJ_Clear_OSD_Buffer(E_OSDLayer layer_id)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_BOOT);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_clear_osd_buffer(layer_id);
	}

	return ret;
}

/* Set blink method of the OSD data.
*/
FJ_ERR_CODE FJ_OSD_Set_Blink_On_Off(E_OSDLayer layer_id, BYTE blink_show_period[OSD_DRAW_AREA_NUM], BYTE blink_hide_period[OSD_DRAW_AREA_NUM], ULONG area)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osd_set_blink_on_off(layer_id, blink_show_period, blink_hide_period, area);
	}

	return ret;
}

/* Set histogram display area position.
*/
FJ_ERR_CODE FJ_SetDispHistogramPosition(E_OSDLayer layer_id, unsigned short histo_x, unsigned short histo_y,
										unsigned short histo_width, unsigned short histo_height, BYTE drawAreaNum)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_setdisphistogramposition(layer_id, histo_x, histo_y, histo_width, histo_height, drawAreaNum);
	}

	return ret;
}

/* Get Histogram box ouside width.
*/
UINT16 FJ_GetHistogramWidth(E_OSDLayer layer_id)
{
	UINT16 ret = 0;

	if(BF_Mode_Check(VALIDMODECATEGORY_COMM) == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_gethistogramwidth( layer_id );
	}
	else {
		// Mode check error
		BF_Debug_Print_Error(("FJ_GetHistogramWidth: MODE Error!\n"));
	}

	return ret;
}

/* Get Histogram box outside height.
*/
UINT16 FJ_GetHistogramHeight(E_OSDLayer layer_id)
{
	UINT16 ret = 0;

	if(BF_Mode_Check(VALIDMODECATEGORY_COMM) == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_gethistogramheight( layer_id );
	}
	else {
		// Mode check error
		BF_Debug_Print_Error(("FJ_GetHistogramHeight: MODE Error!\n"));
	}

	return ret;
}

/* Set threshold and draw frame of histogram.
*/
FJ_ERR_CODE FJ_PreHistogram(E_OSDLayer layer_id, UINT32 threshold)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_prehistogram(layer_id, threshold);
	}

	return ret;
}

/* Draw histogram.
*/
FJ_ERR_CODE FJ_HistogramEnable(E_OSDLayer layer_id)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		fj_histogramenable(layer_id);
	}

	return ret;
}

/* Draw a shape on osd by display id and type.
*/
FJ_ERR_CODE	FJ_OSDDrawShape(E_OSDLayer layer_id, E_FJ_OSD_SHAPE_TYPE wType,
							INT32 wStartX, INT32 wStartY, INT32 uwWidth, INT32 uwHeight, BYTE drawAreaNum)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osddrawshape(layer_id, wType, wStartX, wStartY, uwWidth, uwHeight, drawAreaNum);
	}

	return ret;
}

#if 0	// MILB Not Use
/* Draw a line to specified layer.
*/
FJ_ERR_CODE FJ_OSDDrawLine(E_OSDLayer layer_id, const FJ_LINE_COORDINATES* line, USHORT Thickness, BYTE TerminatorType, BYTE drawAreaNum)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osddrawline(layer_id, *line, Thickness, TerminatorType, drawAreaNum);
	}

	return ret;
}
#endif	// MILB Not Use

/* Draw specified image to draw buffer of V or HD layer.
*/
FJ_ERR_CODE FJ_OSDDrawImage(E_OSDLayer layer_id, UCHAR* y_addr, UCHAR* c_addr,
							USHORT start_x, USHORT start_y, USHORT width, USHORT height)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osddrawimage(layer_id, y_addr, c_addr, start_x, start_y, width, height);
	}

	return ret;
}

/* Draw specified image to draw buffer of OSD0 or OSD1 or HDMI OSD0 or HDMI OSD1 layer.
*/
FJ_ERR_CODE FJ_OSDDrawBufferOSD(E_OSDLayer layer_id, BYTE* addr_src, T_Rect const* const draw_info, BYTE drawAreaNum, USHORT global_width)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osddrawbufferosd(layer_id, addr_src, draw_info, drawAreaNum, global_width);
	}

	return ret;
}

/* Clear a part of display buffer.
*/
FJ_ERR_CODE FJ_OSDClearPart(E_OSDLayer layer_id, T_Rect clear_rect, BYTE drawAreaNum)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osdclearpart(layer_id, clear_rect, drawAreaNum);
	}

	return ret;
}

///////////////////////////////////////////////////
//  Following are BASEFW_VIEW_CODE
///////////////////////////////////////////////////
#if 0	// MILB Not Use
/* Configures the image display attributes.
*/
FJ_ERR_CODE FJ_SetGDA(FJ_E_OSD_GDA_ATTR_TYPE bAttr, INT32 wParam1, INT32 wParam2)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_VIEW);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_setgda(bAttr, wParam1, wParam2);
	}

	return ret;
}
#endif	// MILB Not Use

#if 0	// MILB Not Use
/* Draw a shape on image by shape type.
*/
FJ_ERR_CODE FJ_DrawShape(FJ_E_OSD_SHAPE_TYPE bShapeType, SHORT wStartX, SHORT wStartY,
						 USHORT uwSizeX, USHORT uwSizeY, USHORT uwGlobalWidth, FJ_T_OSD_ADDR_YCC const* const image_addr)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_VIEW);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_drawshape(bShapeType, wStartX, wStartY, uwSizeX, uwSizeY, uwGlobalWidth, image_addr);
	}

	return ret;
}
#endif	// MILB Not Use

#if 0	// MILB Not Use
/* The text for debugging is displayed on OSD.
*/
FJ_ERR_CODE FJ_OSDPrint(E_OSDLayer layer_id, FJ_OSD_PRINT_COLOR color[2], T_POS postion, CHAR* string, BYTE drawAreaNum)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osdprint(layer_id, color, postion, string, drawAreaNum);
	}

	return ret;
}
#endif	// MILB Not Use

/* The kind of the target object for the processing such as drawing is selected. 
*/
BOOL FJ_DirectSelectFont(E_Draw_Type ulFontId)
{
	BOOL ret = (BF_Mode_Check(VALIDMODECATEGORY_BOOT) == FJ_ERR_OK) ? TRUE : FALSE;

	if(ret)	{
		// Processing is executable.
		ret = fj_directselectfont(ulFontId);
	}

	return ret;
}

/* Same as @ref FJ_LoadFontFile.
*/
FJ_ERR_CODE FJ_DirectLoadFont(CHAR const* const szFontName)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_BOOT);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_directloadfont(szFontName);
	}

	return ret;
}

/* Load a string file, to be used for text drawing.
*/
FJ_ERR_CODE	FJ_LoadStringFile(CHAR const* const StringFileName)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_BOOT);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_loadstringfile(StringFileName);
	}

	return ret;
}

/* Load a RGBA8888 icon's palette table from file
*/
extern FJ_ERR_CODE FJ_SetRGBAPalette(const char* RGBPaletteTableName)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_BOOT);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_setrgbapalette(RGBPaletteTableName);
	}

	return ret;
}

/* Set display stream.
*/
FJ_ERR_CODE FJ_Set_Display_Stream(UINT32 stream_idx)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_set_display_stream(stream_idx);
	}

	return ret;
}

/* Get display stream.
*/
FJ_ERR_CODE FJ_Get_Display_Stream(UINT32* stream_idx)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_get_display_stream(stream_idx);
	}

	return ret;
}

///////////////////////////////////////////////////
//  Following are BASEFW_COMM_CODE
///////////////////////////////////////////////////
/* Draw an icon string.
*/
FJ_ERR_CODE FJ_OSDTextOutID(E_OSDLayer layer_id, INT32 wStartX, INT32 wStartY, INT32 wLength, INT32 uwStringID
							,E_DrawMode DrawMode, BYTE alpha, BYTE drawAreaNum)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osdtextoutid(layer_id, wStartX, wStartY, wLength, uwStringID,DrawMode,alpha,drawAreaNum);
	}

	return ret;
}

/* Display image file.
*/
FJ_ERR_CODE	FJ_DrawFile(E_OSDLayer layer_id, FJ_GR_FILE_TYPE wFileType, CHAR* FileName,
						USHORT uStartX, USHORT uStartY, USHORT uWidth, USHORT uHight, BYTE drawAreaNum)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);
	E_FJ_DISP_COMM_BLOCK block = E_FJ_DISP_COMM_BLOCK_0;

	if(ret == FJ_ERR_OK) {
		if( (layer_id & E_OSDLayer_LCD ) == E_OSDLayer_LCD ){
			block = E_FJ_DISP_COMM_BLOCK_1;
		}
		else{
			block = E_FJ_DISP_COMM_BLOCK_0;
		}
		// Processing is executable.
		ret = fj_drawfile(block, layer_id, wFileType, FileName, uStartX, uStartY, uWidth, uHight, drawAreaNum);
	}

	return ret;
}

/* Enables the host to display a given string at a specific location on the display.
*/
FJ_ERR_CODE FJ_OSDTextOutbyASCIITable(E_OSDLayer layerID, const T_OSD_TEXT_OUT_BY_ASCII_TABLE_INFO* textOutInfo)
{
	FJ_ERR_CODE ret = BF_Mode_Check(VALIDMODECATEGORY_COMM);

	if(ret == FJ_ERR_OK) {
		// Processing is executable.
		ret = fj_osdtextoutbyasciitable(layerID, textOutInfo);
	}

	return ret;
}

FJ_ERR_CODE FJ_OSD_Icon_Notify_Loading_Complete( E_FJ_RESOURCE_TYPE type, UINT32 addr )
{
	FJ_ERR_CODE ret = FJ_ERR_NG;

	ret = BF_Mode_Check( VALIDMODECATEGORY_COMM );
	if( ret == FJ_ERR_OK ) {
		ret = fj_osd_icon_notify_loading_complete( type, addr );
	}

	return ret;
}


