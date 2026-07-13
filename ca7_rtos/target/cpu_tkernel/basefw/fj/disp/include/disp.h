/**
 * @file		disp.h
 * @brief		LCD, HDMI, Image and OSD control
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _DISP_H
#define _DISP_H

#include "ddim_typedef.h"
#include "fj_std.h"
#include "fj_disp.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Image format type */
typedef enum {
//	E_BF_DISP_IMAGE_FORMAT_YCC422_RP_8BIT		= 0,	/**< YCbCr422 respect point 8bit format.			*/
	E_BF_DISP_IMAGE_FORMAT_422					= 0,	/**< YCbCr422 respect point 8bit format.			*/
	E_BF_DISP_IMAGE_FORMAT_YCC422_RP_12BIT_PACK = 1,	/**< YCbCr422 respect point 12bit format.			*/
	E_BF_DISP_IMAGE_FORMAT_YCC422_RP_10BIT_PACK	= 3,	/**< YCbCr422 respect point 10bit format.			*/
//	E_BF_DISP_IMAGE_FORMAT_YCC420_RP_8BIT		= 4,	/**< YCbCr420 respect point 8bit format.			*/
	E_BF_DISP_IMAGE_FORMAT_420					= 4,	/**< YCbCr420 respect point 8bit format.			*/
	E_BF_DISP_IMAGE_FORMAT_YCC420_RP_10BIT_PACK	= 7,	/**< YCbCr420 respect point 10bit format.			*/
	E_BF_DISP_IMAGE_FORMAT_GENERAL_10BIT		= 11,	/**< General purpose 10bit input data.				*/
	E_BF_DISP_IMAGE_FORMAT_GENERAL_10BITx2		= 27,	/**< General purpose 10bit input data (10bitx2).	*/
	E_BF_DISP_IMAGE_FORMAT_VIDEO				= 0xFF,	/**< Video format									*/
//	E_BF_DISP_IMAGE_FORMAT_420		= 0x04,		/**< 420 Semi -Planar 8bit format	*/
//	E_BF_DISP_IMAGE_FORMAT_422		= 0x00,		/**< 422 Semi -Planar 8bit format	*/
//	E_BF_DISP_IMAGE_FORMAT_RIBERY	= 0x0C,		/**< Ribery format					*/
} E_BF_DISP_IMAGE_FORMAT;

/** YCC color space */
typedef enum {
	E_BF_DISP_YCC_COLOR_SPACE_601 = 0,			/**< ITU-R BT.601	*/
	E_BF_DISP_YCC_COLOR_SPACE_709				/**< ITU-R BT.709	*/
} E_BF_DISP_YCC_COLOR_SPACE;

/** HDMI Audio I2S SD justify */
typedef enum {
	E_BF_DISP_HDMI_AUDIO_I2S_JUST_LEFT = 0,		/**< Data is left-justified		*/
	E_BF_DISP_HDMI_AUDIO_I2S_JUST_RIGHT,		/**< Data is right-justified	*/
	E_BF_DISP_HDMI_AUDIO_I2S_STANDARD,			/**< Data is standard			*/
} E_BF_DISP_HDMI_AUDIO_I2S_JUST;

/** HDMI Audio I2S sample word length */
typedef enum {
	E_BF_DISP_HDMI_AUDIO_I2S_LEN_16 = 0,		/**< 16bit		*/
	E_BF_DISP_HDMI_AUDIO_I2S_LEN_24 = 5,		/**< 24bit		*/
} E_BF_DISP_HDMI_AUDIO_I2S_LEN;

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
/* Function																*/
/*----------------------------------------------------------------------*/
extern FJ_ERR_CODE BF_Disp_Set_Main_Addr( UCHAR disp_id, FJ_DISP_YCC_ADDR* ycc_addr, E_BF_DISP_IMAGE_FORMAT image_format, E_BF_DISP_YCC_COLOR_SPACE color_space, ULONG global_width );
extern VOID BF_Disp_Wait_Vsync( FJ_DISP_TYPE disp_type );
extern FJ_ERR_CODE BF_Disp_Set_Audio_I2S( ULONG samp_freq, E_BF_DISP_HDMI_AUDIO_I2S_JUST i2s_just, E_BF_DISP_HDMI_AUDIO_I2S_LEN i2s_len );
extern VOID BF_Disp_Clear_Addr( VOID );
extern BOOL BF_Disp_HDMI_Get_SPD_InfoFrame_Enable( VOID );
extern BOOL BF_Disp_HDMI_Get_MPEG_InfoFrame_Enable( VOID );
extern VOID BF_Disp_Instance_Print_Info( VOID );
extern VOID BF_Disp_OSD_Print_Info( UCHAR disp_id );

extern FJ_ERR_CODE fj_disp_open( FJ_DISP_TYPE disp_type, UCHAR *disp_id );
extern FJ_ERR_CODE fj_disp_close( UCHAR disp_id );
extern FJ_ERR_CODE fj_disp_lcd_config( UCHAR disp_id, FJ_DISP_LCD_CONFIG *lcd_config_info );
extern FJ_ERR_CODE fj_disp_lcd_start_sync( UCHAR disp_id );
extern FJ_ERR_CODE fj_disp_lcd_stop_sync( UCHAR disp_id );
extern FJ_DISP_LCD_CONNECT_STATE fj_disp_lcd_get_status( VOID );
extern FJ_ERR_CODE fj_disp_hdmi_set_detect_callback( DISP_HDMI_CALLBACK interrupt_callback , FJ_DISP_HDMI_DETECT_SETTING const* const detect_setting );
extern FJ_DISP_HDMI_DETECT fj_disp_hdmi_get_detect_status( VOID );
extern FJ_ERR_CODE fj_disp_hdmi_get_sink( UCHAR disp_id, FJ_DISP_HDMI_SINK_INFO* sink_info );
extern FJ_ERR_CODE fj_disp_hdmi_get_devicevendorproductid( UCHAR disp_id, FJ_DISP_HDMI_DEV_VENDOR_PRODUCT_ID* vendor_product_id );
extern FJ_ERR_CODE fj_disp_hdmi_set_vic_info( UCHAR disp_id, FJ_DISP_HDMI_VIC_INFO* vic_info );
extern FJ_ERR_CODE fj_disp_hdmi_set_spd( UCHAR disp_id, FJ_DISP_HDMI_SPD_INFO* spd_info );
extern FJ_ERR_CODE fj_disp_hdmi_set_vendor_specific( UCHAR disp_id, FJ_DISP_HDMI_VENDOR_SPEC_INFO* vendor_spec_info );
extern FJ_ERR_CODE fj_disp_hdmi_config_video( UCHAR disp_id, FJ_DISP_VIDEO_ID video_id, UCHAR freq_ctrl );
extern FJ_ERR_CODE fj_disp_hdmi_config_audio( UCHAR disp_id, ULONG samp_freq );
extern FJ_ERR_CODE fj_disp_hdmi_start_sync( UCHAR disp_id );
extern FJ_ERR_CODE fj_disp_hdmi_stop_sync( UCHAR disp_id );
extern FJ_ERR_CODE fj_disp_mipi_config_video( UCHAR disp_id, FJ_DISP_LCD_CONFIG *lcd_config_info );
extern FJ_ERR_CODE fj_disp_mipi_start_sync( UCHAR disp_id );
extern FJ_ERR_CODE fj_disp_mipi_stop_sync( UCHAR disp_id );
extern FJ_ERR_CODE fj_set_display_window( UCHAR disp_id, FJ_DISP_WINDOW_CONFIG* window_config );
extern FJ_ERR_CODE fj_get_display_window( UCHAR disp_id, FJ_DISP_WINDOW_CONFIG* window_config );
extern FJ_ERR_CODE fj_disp_video_render( UCHAR disp_id, FJ_DISP_YCC_ADDR* ycc_addr, UINT32 global_width );
extern FJ_ERR_CODE fj_disp_video_set_rotate( UCHAR disp_id, FJ_DISP_ROTATE_DEGREE rotate_degree );
extern FJ_ERR_CODE fj_disp_video_get_rotate( UCHAR disp_id, FJ_DISP_ROTATE_DEGREE* rotate_degree );
extern FJ_ERR_CODE fj_disp_video_enable( UCHAR disp_id );
extern FJ_ERR_CODE fj_disp_video_disable( UCHAR disp_id );
extern FJ_ERR_CODE fj_disp_show_boot_image( const FJ_DISP_IMAGE_CONFIG* config_info );
extern FJ_ERR_CODE fj_disp_osd_set_config( UCHAR disp_id, FJ_DISP_LAYERID_TYPE layer_id, FJ_DISP_OSD_CONFIG* config_info );
extern FJ_ERR_CODE fj_disp_osd_enable( UCHAR disp_id, FJ_DISP_LAYERID_TYPE layer_id );
extern FJ_ERR_CODE fj_disp_osd_disable( UCHAR disp_id, FJ_DISP_LAYERID_TYPE layer_id );
extern FJ_ERR_CODE fj_disp_osd_rotate_copy( UCHAR disp_id, FJ_DISP_LAYERID_TYPE layer_id, FJ_DISP_OSD_RECT_INFO* rect_info, FJ_DISP_OSD_BUFF_ADDR* buff_addr );
extern FJ_ERR_CODE fj_disp_osd_render( UCHAR disp_id, FJ_DISP_LAYERID_TYPE layer_id, UINT32 disp_buff_addr );
extern FJ_ERR_CODE fj_disp_write_channel_draw( const FJ_DWCH_IMG_INFO* const img_info, E_OSDLayer layer_id );
extern FJ_ERR_CODE fj_disp_write_channel_set_config( const FJ_DWCH_CONFIG *const dwch_config );


#endif	// _DISP_H
