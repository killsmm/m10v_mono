/**
 * @file		ct_im_hdmi.c
 * @brief		This is ct code for im_hdmi.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "ct_im_hdmi.h"
#include "jdsdisp3a.h"
#include "im_hdmi.h"
#include "im_disp.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#ifdef CO_DEBUG_ON_PC
#define D_HDMI_TEST_CMD_LEN		20
#endif	// CO_DEBUG_ON_PC

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// nothing special.

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static const T_IM_HDMI_CSC_COEF gCt_Im_Hdmi_Csc_Matrix[D_IM_HDMI_CSC_CONF_COLUMN_NUM][D_IM_HDMI_CSC_CONF_ROW_NUM] = {
	{{0x00, 0x00}, {0x00, 0x0F}, {0x0F, 0x00}, {0x0F, 0x0F}},
	{{0x01, 0x01}, {0x20, 0x02}, {0x04, 0x40}, {0x80, 0x80}},
	{{0x44, 0x44}, {0x88, 0x88}, {0xCC, 0xCC}, {0xFF, 0xFF}}
};

#ifdef CO_DEBUG_ON_PC
static char* gCmd_Argv[25];
#endif	// CO_DEBUG_ON_PC

/*----------------------------------------------------------------------*/
/* Local Method															*/
/*----------------------------------------------------------------------*/
static VOID ct_im_hdmi_pclk_on(VOID)
{
#ifdef CO_ACT_PCLOCK
	Im_DISP_APB_Clock_On();
#endif	// CO_ACT_PCLOCK
}

/**
 * @brief	PCLK change to OFF.
 * @param	VOID.
 * @retval	VOID.
 */
static VOID ct_im_hdmi_pclk_off(VOID)
{
#ifdef CO_ACT_PCLOCK
	Im_DISP_APB_Clock_Off();
#endif	// CO_ACT_PCLOCK
}

static VOID ct_im_hdmi_cb0( UCHAR interrupt_type )
{
	Ddim_Print((">> call ct_im_hdmi_cb0() : interrupt_type=%d\n", interrupt_type));
	return;
}

#ifdef CO_DEBUG_ON_PC
static void ct_im_hdmi_set_cmd( int cmd_argc, char* cmd_argv ) 
{ 
	INT32 j;
	INT32 cmd_pos = 0;

	for( j = 0; j < cmd_argc; j++ ) { 
		gCmd_Argv[ j ] = ( cmd_argv + ( cmd_pos * D_HDMI_TEST_CMD_LEN ) ); 
		cmd_pos++;
	}
}
#endif	// CO_DEBUG_ON_PC

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
 * @brief       Command processing of HDMI.
 * @param[in]   INT32 argc
 * @param[in]   CHAR** argv
 * @return      VOID
 */
VOID Ct_Im_Hdmi_Main( INT32 argc, char** argv )
{
	INT32	ret;
	INT32	param2;
	INT32	param3;
	INT32	param4;
	INT32	loop1;
	T_IM_HDMI_PLL_CONFIG pll_config;
	T_IM_HDMI_I2CM_CONFIG i2cm_config;
	UCHAR read_data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	T_IM_HDMI_CTRL ctrl;
#if 0
	USHORT gmp_ctrl;
	U_IM_HDMI_PLLCURRCTRL pll_curr_ctrl;
	U_IM_HDMI_OPMODE_PLLCFG op_mode;
	E_IM_HDMI_TXTERM txterm;
	U_IM_HDMI_CKSYMTXCTRL cksymtxctrl;
	U_IM_HDMI_VLEVCTRL vlevctrl;
#endif
	T_IM_HDMI_CSC_CONFIG csc_config;
	U_IM_HDMI_TX_INVID0 tx_invid0;
	T_IM_HDMI_VIDEO_PACKETIZER video_packetizer;
	T_IM_HDMI_FRAME_COMPOSER frame_composer;
	UCHAR mc_flowctrl;
	T_IM_HDMI_AUDIO audio;
	VP_HDMI_CALLBACK vp_callback;
	E_IM_HDMI_INT_REG interrupt_register;
	UCHAR mute;
	
	// check number of parameter
	if( argc > 20 ){
		Ddim_Print(("The number of input parameters is exceeded!\n"));
		return;
	}

	// 1st param check
	if( strcmp(argv[1], "init") == 0 ) {
		// Init
		ret = Im_HDMI_Init();
		if (ret == D_DDIM_OK) {
			Ddim_Print(("Im_HDMI_Init() : Normal end\n"));
			// check register
			ct_im_hdmi_pclk_on();
			Ddim_Print( ("----------\n") );
			Ddim_Print( ("tx_invid0        = %p\n",   IO_DISP.HDMI_TX.tx_invid0.byte) );
			Ddim_Print( ("fc_invidconf     = %p\n",   IO_DISP.HDMI_TX.fc_invidconf.byte) );
			Ddim_Print( ("fc_inhactiv0     = %p\n",   IO_DISP.HDMI_TX.fc_inhactiv0.byte) );
			Ddim_Print( ("fc_inhactiv1     = %p\n",   IO_DISP.HDMI_TX.fc_inhactiv1.byte) );
			Ddim_Print( ("fc_invactiv0     = %p\n",   IO_DISP.HDMI_TX.fc_invactiv0.byte) );
			Ddim_Print( ("fc_invactiv1     = %p\n",   IO_DISP.HDMI_TX.fc_invactiv1.byte) );
			Ddim_Print( ("fc_inhblank0     = %p\n",   IO_DISP.HDMI_TX.fc_inhblank0.byte) );
			Ddim_Print( ("fc_inhblank1     = %p\n",   IO_DISP.HDMI_TX.fc_inhblank1.byte) );
			Ddim_Print( ("fc_invblank      = 0x%x\n", IO_DISP.HDMI_TX.fc_invblank) );
			Ddim_Print( ("fc_hsyncindelay0 = %p\n",   IO_DISP.HDMI_TX.fc_hsyncindelay0.byte) );
			Ddim_Print( ("fc_hsyncindelay1 = %p\n",   IO_DISP.HDMI_TX.fc_hsyncindelay1.byte) );
			Ddim_Print( ("fc_vsyncindelay  = 0x%x\n", IO_DISP.HDMI_TX.fc_vsyncindelay) );
			Ddim_Print( ("fc_hsyncinwidth0 = %p\n",   IO_DISP.HDMI_TX.fc_hsyncinwidth0.byte) );
			Ddim_Print( ("fc_hsyncinwidth1 = %p\n",   IO_DISP.HDMI_TX.fc_hsyncinwidth1.byte) );
			Ddim_Print( ("fc_vsyncinwidth  = %p\n",   IO_DISP.HDMI_TX.fc_vsyncinwidth.byte) );
			Ddim_Print( ("fc_ctrldur       = 0x%x\n", IO_DISP.HDMI_TX.fc_ctrldur) );
			Ddim_Print( ("fc_exctrldur     = 0x%x\n", IO_DISP.HDMI_TX.fc_exctrldur) );
			Ddim_Print( ("fc_exctrlspac    = 0x%x\n", IO_DISP.HDMI_TX.fc_exctrlspac) );
			Ddim_Print( ("fc_ch0pream      = 0x%x\n", IO_DISP.HDMI_TX.fc_ch0pream) );
			Ddim_Print( ("fc_ch1pream      = %p\n",   IO_DISP.HDMI_TX.fc_ch1pream.byte) );
			Ddim_Print( ("fc_ch2pream      = %p\n",   IO_DISP.HDMI_TX.fc_ch2pream.byte) );
			Ddim_Print( ("mc_clkdis        = %p\n",   IO_DISP.HDMI_TX.mc_clkdis.byte) );
			Ddim_Print( ("----------\n") );
			ct_im_hdmi_pclk_off();
		}
		else {
			Ddim_Print(("im_HDMI_Init() : Error  err_code=0x%x\n", ret));
		}
	}
	else if( strcmp(argv[1], "conf_dvi") == 0 ) {
		// Init
		// 2nd param check
		param2 = atoi((const char *)argv[2]);
		if (param2 == 0) {
			// 2nd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// PLL Config (Normal).
				pll_config.vp_pr_cd.bit.desired_pr_factor	= 0;
				pll_config.vp_pr_cd.bit.color_depth			= E_IM_HDMI_COLOR_DEPTH_8BIT;
//				pll_config.phy_i2cm_address					= 0x00;
//				pll_config.phy_i2cm_datao[0]				= 0x00;
//				pll_config.phy_i2cm_datao[1]				= 0x00;
			}
			else {
				// end
				Ddim_Print(("!! 3rd parameter fraud : %d\n", param3));
				return;
			}
		}
		else if (param2 == 1) {
			// Invalid parameter (pll_config = NULL)
			ret = Im_HDMI_Configure_VGA_DVI_Video_Mode(NULL);
			Ddim_Print(("Im_HDMI_Configure_VGA_DVI_Video_Mode() : Error  err_code=0x%x\n", ret));
			// end
			return;
		}
		else if (param2 == 2) {
			// PLL Config (NG).
			pll_config.vp_pr_cd.bit.desired_pr_factor	= 0;
			pll_config.vp_pr_cd.bit.color_depth			= E_IM_HDMI_COLOR_DEPTH_8BIT;
//			pll_config.phy_i2cm_address					= 0x00;
//			pll_config.phy_i2cm_datao[0]				= 0x00;
//			pll_config.phy_i2cm_datao[1]				= 0x00;
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud : %d\n", param2));
			return;
		}
		
		ret = Im_HDMI_Configure_VGA_DVI_Video_Mode(&pll_config);
		if (ret == D_DDIM_OK) {
			Ddim_Print(("Im_HDMI_Configure_VGA_DVI_Video_Mode() : Normal end\n"));
			// check register
			ct_im_hdmi_pclk_on();
			Ddim_Print( ("----------\n") );
			Ddim_Print( ("tx_invid0        = %p\n",   IO_DISP.HDMI_TX.tx_invid0.byte) );
			Ddim_Print( ("fc_invidconf     = %p\n",   IO_DISP.HDMI_TX.fc_invidconf.byte) );
			Ddim_Print( ("fc_inhactiv0     = %p\n",   IO_DISP.HDMI_TX.fc_inhactiv0.byte) );
			Ddim_Print( ("fc_inhactiv1     = %p\n",   IO_DISP.HDMI_TX.fc_inhactiv1.byte) );
			Ddim_Print( ("fc_invactiv0     = %p\n",   IO_DISP.HDMI_TX.fc_invactiv0.byte) );
			Ddim_Print( ("fc_invactiv1     = %p\n",   IO_DISP.HDMI_TX.fc_invactiv1.byte) );
			Ddim_Print( ("fc_inhblank0     = %p\n",   IO_DISP.HDMI_TX.fc_inhblank0.byte) );
			Ddim_Print( ("fc_inhblank1     = %p\n",   IO_DISP.HDMI_TX.fc_inhblank1.byte) );
			Ddim_Print( ("fc_invblank      = 0x%x\n", IO_DISP.HDMI_TX.fc_invblank) );
			Ddim_Print( ("fc_hsyncindelay0 = %p\n",   IO_DISP.HDMI_TX.fc_hsyncindelay0.byte) );
			Ddim_Print( ("fc_hsyncindelay1 = %p\n",   IO_DISP.HDMI_TX.fc_hsyncindelay1.byte) );
			Ddim_Print( ("fc_vsyncindelay  = 0x%x\n", IO_DISP.HDMI_TX.fc_vsyncindelay) );
			Ddim_Print( ("fc_hsyncinwidth0 = %p\n",   IO_DISP.HDMI_TX.fc_hsyncinwidth0.byte) );
			Ddim_Print( ("fc_hsyncinwidth1 = %p\n",   IO_DISP.HDMI_TX.fc_hsyncinwidth1.byte) );
			Ddim_Print( ("fc_vsyncinwidth  = %p\n",   IO_DISP.HDMI_TX.fc_vsyncinwidth.byte) );
			Ddim_Print( ("fc_ctrldur       = 0x%x\n", IO_DISP.HDMI_TX.fc_ctrldur) );
			Ddim_Print( ("fc_exctrldur     = 0x%x\n", IO_DISP.HDMI_TX.fc_exctrldur) );
			Ddim_Print( ("fc_exctrlspac    = 0x%x\n", IO_DISP.HDMI_TX.fc_exctrlspac) );
			Ddim_Print( ("fc_ch0pream      = 0x%x\n", IO_DISP.HDMI_TX.fc_ch0pream) );
			Ddim_Print( ("fc_ch1pream      = %p\n",   IO_DISP.HDMI_TX.fc_ch1pream.byte) );
			Ddim_Print( ("fc_ch2pream      = %p\n",   IO_DISP.HDMI_TX.fc_ch2pream.byte) );
			Ddim_Print( ("mc_clkdis        = %p\n",   IO_DISP.HDMI_TX.mc_clkdis.byte) );
			Ddim_Print( ("----------\n") );
			ct_im_hdmi_pclk_off();
		}
		else {
			Ddim_Print(("Im_HDMI_Configure_VGA_DVI_Video_Mode() : Error  err_code=0x%x\n", ret));
		}
	}
	else if( strcmp(argv[1], "sinks") == 0 ) {
		// Init
		// 2nd param check
		param2 = atoi((const char *)argv[2]);
		if (param2 == 0) {
			
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// i2cm Config.
				i2cm_config.i2cm_address	= 0x00;
				i2cm_config.i2cm_segaddr	= 0x00;
				i2cm_config.i2cm_segptr		= 0x00;
				i2cm_config.i2cm_operation	= E_IM_HDMI_I2CM_OPERATION_RD;
			}
			else if (param3 == 1) {
				// i2cm Config.
				i2cm_config.i2cm_address	= 0xFF;
				i2cm_config.i2cm_segaddr	= 0xFF;
				i2cm_config.i2cm_segptr		= 0xFF;
				i2cm_config.i2cm_operation	= E_IM_HDMI_I2CM_OPERATION_WR;
			}
			else {
				// end
				Ddim_Print(("!! 3rd parameter fraud : %d\n", param3));
				return;
			}
		}
		else if (param2 == 1) {
			ret = Im_HDMI_Read_Sinks_E_EDID(NULL, read_data);
			Ddim_Print(("Im_HDMI_Read_Sinks_E_EDID() : Error  err_code=0x%x\n", ret));
			// end
			return;
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud : %d\n", param2));
			return;
		}
		
		ret = Im_HDMI_Read_Sinks_E_EDID(&i2cm_config, read_data);
		if (ret == D_DDIM_OK) {
			Ddim_Print(("Im_HDMI_Read_Sinks_E_EDID() : Normal end\n"));
			// check register
			ct_im_hdmi_pclk_on();
			Ddim_Print( ("----------\n") );
			Ddim_Print( ("read data:\n") );
			Ddim_Print( ("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
						 read_data[0], read_data[1], read_data[2], read_data[3],
						 read_data[4], read_data[5], read_data[6], read_data[7]) );
			Ddim_Print( ("register:\n") );
			Ddim_Print( ("i2cm_slave     = %p\n",   IO_DISP.HDMI_TX.i2cm_slave.byte) );
			Ddim_Print( ("i2cm_segaddr   = %p\n",   IO_DISP.HDMI_TX.i2cm_segaddr.byte) );
			Ddim_Print( ("i2cm_segptr    = 0x%x\n", IO_DISP.HDMI_TX.i2cm_segptr) );
			Ddim_Print( ("i2cm_operation = %p\n",   IO_DISP.HDMI_TX.i2cm_operation.byte) );
			Ddim_Print( ("----------\n") );
			ct_im_hdmi_pclk_off();
		}
		else {
			Ddim_Print(("Im_HDMI_Read_Sinks_E_EDID() : Error  err_code=0x%x\n", ret));
		}
	}
	else if( strcmp(argv[1], "ctrl") == 0 ) {
		// 2nd param check
		if( strcmp(argv[2], "set") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 1) {
				// Invalid parameter (ctrl = NULL)
				ret = Im_HDMI_Ctrl(NULL);
				Ddim_Print(("Im_HDMI_Ctrl() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			else if (param3 == 2) {
				// PLL NG.
				ct_im_hdmi_pclk_on();
				IO_DISP.HDMI_TX.phy_stat0.bit.TX_PHY_LOCK = 0;
				ct_im_hdmi_pclk_off();
			}
			else if (param3 != 0) {
				// end
				Ddim_Print(("!! 3rd parameter fraud : %d\n", param3));
				return;
			}
			
			memset(&ctrl, 0, sizeof( T_IM_HDMI_CTRL ));

			// 4th param check
			param4 = atoi((const char *)argv[4]);
			if (param4 == 0) {
				// set paramter.
				ctrl.video_input_mapping										= E_IM_HDMI_VIDEO_MAPPING_RGB444_8BIT;
				ctrl.vsync_in_polarity											= D_IM_HDMI_ACTIVE_LOW;
				ctrl.hsync_in_polarity											= D_IM_HDMI_ACTIVE_LOW;
				ctrl.de_in_polarity												= D_IM_HDMI_ACTIVE_LOW;
				ctrl.DVI_modez													= D_IM_HDMI_DVI_MODEZ_DVI;
				ctrl.r_v_blank_in_osc											= D_IM_HDMI_ACTIVE_LOW;
				ctrl.in_i_p														= D_IM_HDMI_IN_I_P_PROGRESSIVE;
				ctrl.h_in_activ													= 0;
				ctrl.v_in_activ													= 0;
				ctrl.fc_invact_2d												= 0;
				ctrl.fc_actspc_hdlr_cfg.bit.actspc_hdlr_en						= D_IM_HDMI_ENABLE_OFF;
				ctrl.fc_actspc_hdlr_cfg.bit.actspc_hdlr_tgl						= D_IM_HDMI_ENABLE_OFF;
				ctrl.fc_inhblank												= 0;
				ctrl.fc_invblank												= 0;
				ctrl.fc_hsyncindelay											= 0;
				ctrl.fc_hsyncinwidth											= 0;
				ctrl.fc_vsyncindelay											= 0;
				ctrl.fc_vsyncinwidth											= 0;
				// PLL
				ctrl.pll_config.vp_pr_cd.bit.desired_pr_factor					= 0;
				ctrl.pll_config.vp_pr_cd.bit.color_depth						= E_IM_HDMI_COLOR_DEPTH_8BIT;
//				ctrl.pll_config.phy_i2cm_address								= 0x00;
//				ctrl.pll_config.phy_i2cm_datao[0]								= 0x00;
//				ctrl.pll_config.phy_i2cm_datao[1]								= 0x00;
				// Audio
				ctrl.audio_config.i2s_select									= E_IM_HDMI_I2S_SELECT_I2S;
				ctrl.audio_config.aud_conf1.bit.I2S_width						= 16;
				ctrl.audio_config.aud_conf1.bit.I2S_mode						= E_IM_HDMI_I2S_MODE_STANDARD;
				ctrl.audio_config.aud_conf2.bit.HBR								= D_IM_HDMI_ENABLE_OFF;
				ctrl.audio_config.aud_conf2.bit.NLPCM							= D_IM_HDMI_ENABLE_OFF;
				ctrl.audio_config.aud_spdif1.bit.spdif_width					= 16;
				ctrl.audio_config.aud_spdif1.bit.spdif_hbr_mode					= 0;
				ctrl.audio_config.aud_spdif1.bit.setnlpcm						= 0;
				ctrl.audio_config.aud_inputclkfs								= E_IM_HDMI_AUD_IFSFACTOR_128XFS;
				ctrl.audio_config.aud_n1										= 0;
				ctrl.audio_config.aud_n2										= 0;
				ctrl.audio_config.aud_n3_audn									= 0;
				ctrl.audio_config.cts_manual									= E_IM_HDMI_CTS_MANUAL_AUTO;
				ctrl.audio_config.aud_cts1										= 0;
				ctrl.audio_config.aud_cts2										= 0;
				ctrl.audio_config.aud_cts3_audcts								= 0;
				// Infoframes
				ctrl.info_frames.fc_aviconf0.bit.rgc_ycc_indication				= 0;
				ctrl.info_frames.fc_aviconf0.bit.bar_information				= 0;
				ctrl.info_frames.fc_aviconf0.bit.scan_information				= 0;
				ctrl.info_frames.fc_aviconf0.bit.active_format_present			= 0;
				ctrl.info_frames.fc_aviconf0.bit.rgc_ycc_indication_2			= 0;
				ctrl.info_frames.fc_aviconf1.bit.active_aspect_ratio			= 0;
				ctrl.info_frames.fc_aviconf1.bit.picture_aspect_ratio			= 0;
				ctrl.info_frames.fc_aviconf1.bit.Colorimetry					= 0;
				ctrl.info_frames.fc_aviconf2.bit.non_uniform_picture_scaling	= 0;
				ctrl.info_frames.fc_aviconf2.bit.quantization_range				= 0;
				ctrl.info_frames.fc_aviconf2.bit.extended_colorimetry			= 0;
				ctrl.info_frames.fc_aviconf2.bit.it_content						= 0;
				ctrl.info_frames.fc_aviconf3.bit.CN								= 0;
				ctrl.info_frames.fc_aviconf3.bit.YQ								= 0;
				ctrl.info_frames.fc_avivid										= 0;
				ctrl.info_frames.fc_prconf.bit.output_pr_factor					= 0;
				ctrl.info_frames.fc_prconf.bit.incoming_pr_factor				= 0;
				ctrl.info_frames.fc_audiconf0.bit.CT							= 0;
				ctrl.info_frames.fc_audiconf0.bit.CC							= 0;
				ctrl.info_frames.fc_audiconf1.bit.SF							= 0;
				ctrl.info_frames.fc_audiconf1.bit.SS							= 0;
				ctrl.info_frames.fc_audiconf2									= 0;
				ctrl.info_frames.fc_audiconf3.bit.LSV							= 0;
				ctrl.info_frames.fc_audiconf3.bit.DM_INH						= D_IM_HDMI_ENABLE_OFF;
				ctrl.info_frames.fc_audiconf3.bit.LFEPBL						= 0;
				ctrl.info_frames.fc_vsdieeeid[0]								= 0;
				ctrl.info_frames.fc_vsdieeeid[1]								= 0;
				ctrl.info_frames.fc_vsdieeeid[2]								= 0;
				ctrl.info_frames.video_mode										= D_IM_HDMI_VIDEO_MODE_HDMI;
				ctrl.info_frames.video_format									= 0;
				ctrl.info_frames.meta_present									= 0;
				ctrl.info_frames.additional_video_format						= 0;
				ctrl.info_frames.meta_type										= 0;
				ctrl.info_frames.meta_length									= 0;
				ctrl.info_frames.meta_data[0]									= 0;
				ctrl.info_frames.meta_data[1]									= 0;
				ctrl.info_frames.meta_data[2]									= 0;
				ctrl.info_frames.meta_data[3]									= 0;
				ctrl.info_frames.meta_data[4]									= 0;
				ctrl.info_frames.meta_data[5]									= 0;
				ctrl.info_frames.meta_data[6]									= 0;
				ctrl.info_frames.meta_data[7]									= 0;
			}
			else if (param4 == 1) {
				// set paramter.
				ctrl.video_input_mapping										= E_IM_HDMI_VIDEO_MAPPING_YCC420_IPI_16BIT;
				ctrl.vsync_in_polarity											= D_IM_HDMI_ACTIVE_HIGH;
				ctrl.hsync_in_polarity											= D_IM_HDMI_ACTIVE_HIGH;
				ctrl.de_in_polarity												= D_IM_HDMI_ACTIVE_HIGH;
				ctrl.DVI_modez													= D_IM_HDMI_DVI_MODEZ_HDMI;
				ctrl.r_v_blank_in_osc											= D_IM_HDMI_ACTIVE_HIGH;
				ctrl.in_i_p														= D_IM_HDMI_IN_I_P_INTERLACED;
				ctrl.h_in_activ													= 16383;
				ctrl.v_in_activ													= 8191;
				ctrl.fc_invact_2d												= 0xFF;
				ctrl.fc_actspc_hdlr_cfg.bit.actspc_hdlr_en						= D_IM_HDMI_ENABLE_ON;
				ctrl.fc_actspc_hdlr_cfg.bit.actspc_hdlr_tgl						= D_IM_HDMI_ENABLE_ON;
				ctrl.fc_inhblank												= 8191;
				ctrl.fc_invblank												= 255;
				ctrl.fc_hsyncindelay											= 8191;
				ctrl.fc_hsyncinwidth											= 1023;
				ctrl.fc_vsyncindelay											= 255;
				ctrl.fc_vsyncinwidth											= 63;
				// PLL
				ctrl.pll_config.vp_pr_cd.bit.desired_pr_factor					= 9;
				ctrl.pll_config.vp_pr_cd.bit.color_depth						= E_IM_HDMI_COLOR_DEPTH_16BIT;
//				ctrl.pll_config.phy_i2cm_address								= 0xFF;
//				ctrl.pll_config.phy_i2cm_datao[0]								= 0xFF;
//				ctrl.pll_config.phy_i2cm_datao[1]								= 0xFF;
				// Audio
				ctrl.audio_config.i2s_select									= E_IM_HDMI_I2S_SELECT_I2S;
				ctrl.audio_config.aud_conf1.bit.I2S_width						= 16;
				ctrl.audio_config.aud_conf1.bit.I2S_mode						= E_IM_HDMI_I2S_MODE_STANDARD;
				ctrl.audio_config.aud_conf2.bit.HBR								= D_IM_HDMI_ENABLE_OFF;
				ctrl.audio_config.aud_conf2.bit.NLPCM							= D_IM_HDMI_ENABLE_OFF;
				ctrl.audio_config.aud_spdif1.bit.spdif_width					= 24;
				ctrl.audio_config.aud_spdif1.bit.spdif_hbr_mode					= 1;
				ctrl.audio_config.aud_spdif1.bit.setnlpcm						= 1;
				ctrl.audio_config.aud_inputclkfs								= E_IM_HDMI_AUD_IFSFACTOR_64XFS;
				ctrl.audio_config.aud_n1										= 0xFF;
				ctrl.audio_config.aud_n2										= 0xFF;
				ctrl.audio_config.aud_n3_audn									= 0xF;
				ctrl.audio_config.cts_manual									= E_IM_HDMI_CTS_MANUAL_MANUAL;
				ctrl.audio_config.aud_cts1										= 0xFF;
				ctrl.audio_config.aud_cts2										= 0xFF;
				ctrl.audio_config.aud_cts3_audcts								= 0xF;
				// Infoframes
				ctrl.info_frames.fc_aviconf0.bit.rgc_ycc_indication				= 3;
				ctrl.info_frames.fc_aviconf0.bit.bar_information				= 3;
				ctrl.info_frames.fc_aviconf0.bit.scan_information				= 3;
				ctrl.info_frames.fc_aviconf0.bit.active_format_present			= 1;
				ctrl.info_frames.fc_aviconf0.bit.rgc_ycc_indication_2			= 1;
				ctrl.info_frames.fc_aviconf1.bit.active_aspect_ratio			= 15;
				ctrl.info_frames.fc_aviconf1.bit.picture_aspect_ratio			= 3;
				ctrl.info_frames.fc_aviconf1.bit.Colorimetry					= 3;
				ctrl.info_frames.fc_aviconf2.bit.non_uniform_picture_scaling	= 3;
				ctrl.info_frames.fc_aviconf2.bit.quantization_range				= 3;
				ctrl.info_frames.fc_aviconf2.bit.extended_colorimetry			= 7;
				ctrl.info_frames.fc_aviconf2.bit.it_content						= 1;
				ctrl.info_frames.fc_aviconf3.bit.CN								= 3;
				ctrl.info_frames.fc_aviconf3.bit.YQ								= 3;
				ctrl.info_frames.fc_avivid										= 255;
				ctrl.info_frames.fc_prconf.bit.output_pr_factor					= 10;
				ctrl.info_frames.fc_prconf.bit.incoming_pr_factor				= 10;
				ctrl.info_frames.fc_audiconf0.bit.CT							= 15;
				ctrl.info_frames.fc_audiconf0.bit.CC							= 7;
				ctrl.info_frames.fc_audiconf1.bit.SF							= 7;
				ctrl.info_frames.fc_audiconf1.bit.SS							= 3;
				ctrl.info_frames.fc_audiconf2									= 255;
				ctrl.info_frames.fc_audiconf3.bit.LSV							= 0xF;
				ctrl.info_frames.fc_audiconf3.bit.DM_INH						= D_IM_HDMI_ENABLE_ON;
				ctrl.info_frames.fc_audiconf3.bit.LFEPBL						= 3;
				ctrl.info_frames.fc_vsdieeeid[0]								= 255;
				ctrl.info_frames.fc_vsdieeeid[1]								= 255;
				ctrl.info_frames.fc_vsdieeeid[2]								= 255;
				ctrl.info_frames.video_mode										= D_IM_HDMI_VIDEO_MODE_3D;
				ctrl.info_frames.video_format									= 255;
				ctrl.info_frames.meta_present									= 1;
				ctrl.info_frames.additional_video_format						= 15;
				ctrl.info_frames.meta_type										= 7;
				ctrl.info_frames.meta_length									= 31;
				ctrl.info_frames.meta_data[0]									= 255;
				ctrl.info_frames.meta_data[1]									= 255;
				ctrl.info_frames.meta_data[2]									= 255;
				ctrl.info_frames.meta_data[3]									= 255;
				ctrl.info_frames.meta_data[4]									= 255;
				ctrl.info_frames.meta_data[5]									= 255;
				ctrl.info_frames.meta_data[6]									= 255;
				ctrl.info_frames.meta_data[7]									= 255;
			}
			else if (param4 == 2) {
				// set paramter.
				ctrl.video_input_mapping										= E_IM_HDMI_VIDEO_MAPPING_RGB444_8BIT;
				ctrl.vsync_in_polarity											= D_IM_HDMI_ACTIVE_LOW;
				ctrl.hsync_in_polarity											= D_IM_HDMI_ACTIVE_LOW;
				ctrl.de_in_polarity												= D_IM_HDMI_ACTIVE_LOW;
				ctrl.DVI_modez													= D_IM_HDMI_DVI_MODEZ_HDMI;
				ctrl.r_v_blank_in_osc											= D_IM_HDMI_ACTIVE_LOW;
				ctrl.in_i_p														= D_IM_HDMI_IN_I_P_PROGRESSIVE;
				ctrl.h_in_activ													= 0;
				ctrl.v_in_activ													= 0;
				ctrl.fc_invact_2d												= 0;
				ctrl.fc_actspc_hdlr_cfg.bit.actspc_hdlr_en						= D_IM_HDMI_ENABLE_OFF;
				ctrl.fc_actspc_hdlr_cfg.bit.actspc_hdlr_tgl						= D_IM_HDMI_ENABLE_OFF;
				ctrl.fc_inhblank												= 0;
				ctrl.fc_invblank												= 0;
				ctrl.fc_hsyncindelay											= 0;
				ctrl.fc_hsyncinwidth											= 0;
				ctrl.fc_vsyncindelay											= 0;
				ctrl.fc_vsyncinwidth											= 0;
				// PLL
				ctrl.pll_config.vp_pr_cd.bit.desired_pr_factor					= 0;
				ctrl.pll_config.vp_pr_cd.bit.color_depth						= E_IM_HDMI_COLOR_DEPTH_8BIT;
//				ctrl.pll_config.phy_i2cm_address								= 0x00;
//				ctrl.pll_config.phy_i2cm_datao[0]								= 0x00;
//				ctrl.pll_config.phy_i2cm_datao[1]								= 0x00;
				// Audio
				ctrl.audio_config.i2s_select									= E_IM_HDMI_I2S_SELECT_I2S;
				ctrl.audio_config.aud_conf1.bit.I2S_width						= 16;
				ctrl.audio_config.aud_conf1.bit.I2S_mode						= E_IM_HDMI_I2S_MODE_STANDARD;
				ctrl.audio_config.aud_conf2.bit.HBR								= D_IM_HDMI_ENABLE_OFF;
				ctrl.audio_config.aud_conf2.bit.NLPCM							= D_IM_HDMI_ENABLE_OFF;
				ctrl.audio_config.aud_spdif1.bit.spdif_width					= 16;
				ctrl.audio_config.aud_spdif1.bit.spdif_hbr_mode					= 0;
				ctrl.audio_config.aud_spdif1.bit.setnlpcm						= 0;
				ctrl.audio_config.aud_inputclkfs								= E_IM_HDMI_AUD_IFSFACTOR_128XFS;
				ctrl.audio_config.aud_n1										= 0;
				ctrl.audio_config.aud_n2										= 0;
				ctrl.audio_config.aud_n3_audn									= 0;
				ctrl.audio_config.cts_manual									= E_IM_HDMI_CTS_MANUAL_AUTO;
				ctrl.audio_config.aud_cts1										= 0;
				ctrl.audio_config.aud_cts2										= 0;
				ctrl.audio_config.aud_cts3_audcts								= 0;
				// Infoframes
				ctrl.info_frames.fc_aviconf0.bit.rgc_ycc_indication				= 0;
				ctrl.info_frames.fc_aviconf0.bit.bar_information				= 0;
				ctrl.info_frames.fc_aviconf0.bit.scan_information				= 0;
				ctrl.info_frames.fc_aviconf0.bit.active_format_present			= 0;
				ctrl.info_frames.fc_aviconf0.bit.rgc_ycc_indication_2			= 0;
				ctrl.info_frames.fc_aviconf1.bit.active_aspect_ratio			= 0;
				ctrl.info_frames.fc_aviconf1.bit.picture_aspect_ratio			= 0;
				ctrl.info_frames.fc_aviconf1.bit.Colorimetry					= 0;
				ctrl.info_frames.fc_aviconf2.bit.non_uniform_picture_scaling	= 0;
				ctrl.info_frames.fc_aviconf2.bit.quantization_range				= 0;
				ctrl.info_frames.fc_aviconf2.bit.extended_colorimetry			= 0;
				ctrl.info_frames.fc_aviconf2.bit.it_content						= 0;
				ctrl.info_frames.fc_aviconf3.bit.CN								= 0;
				ctrl.info_frames.fc_aviconf3.bit.YQ								= 0;
				ctrl.info_frames.fc_avivid										= 0;
				ctrl.info_frames.fc_prconf.bit.output_pr_factor					= 0;
				ctrl.info_frames.fc_prconf.bit.incoming_pr_factor				= 0;
				ctrl.info_frames.fc_audiconf0.bit.CT							= 0;
				ctrl.info_frames.fc_audiconf0.bit.CC							= 0;
				ctrl.info_frames.fc_audiconf1.bit.SF							= 0;
				ctrl.info_frames.fc_audiconf1.bit.SS							= 0;
				ctrl.info_frames.fc_audiconf2									= 0;
				ctrl.info_frames.fc_audiconf3.bit.LSV							= 0;
				ctrl.info_frames.fc_audiconf3.bit.DM_INH						= D_IM_HDMI_ENABLE_OFF;
				ctrl.info_frames.fc_audiconf3.bit.LFEPBL						= 0;
				ctrl.info_frames.fc_vsdieeeid[0]								= 0;
				ctrl.info_frames.fc_vsdieeeid[1]								= 0;
				ctrl.info_frames.fc_vsdieeeid[2]								= 0;
				
				// paramter ng
				ctrl.info_frames.video_mode										= 0;
				
				ctrl.info_frames.video_format									= 0;
				ctrl.info_frames.meta_present									= 0;
				ctrl.info_frames.additional_video_format						= 0;
				ctrl.info_frames.meta_type										= 0;
				ctrl.info_frames.meta_length									= 0;
				ctrl.info_frames.meta_data[0]									= 0;
				ctrl.info_frames.meta_data[1]									= 0;
				ctrl.info_frames.meta_data[2]									= 0;
				ctrl.info_frames.meta_data[3]									= 0;
				ctrl.info_frames.meta_data[4]									= 0;
				ctrl.info_frames.meta_data[5]									= 0;
				ctrl.info_frames.meta_data[6]									= 0;
				ctrl.info_frames.meta_data[7]									= 0;
			}
			else {
				// end
				Ddim_Print(("!! 4th parameter fraud : %d\n", param4));
				return;
			}
			
			ret = Im_HDMI_Ctrl(&ctrl);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_HDMI_Ctrl() : Normal end\n"));
				// check register
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("tx_invid0          = %p\n",   IO_DISP.HDMI_TX.tx_invid0.byte) );
				Ddim_Print( ("fc_invidconf       = %p\n",   IO_DISP.HDMI_TX.fc_invidconf.byte) );
				Ddim_Print( ("fc_inhactiv0       = %p\n",   IO_DISP.HDMI_TX.fc_inhactiv0.byte) );
				Ddim_Print( ("fc_inhactiv1       = %p\n",   IO_DISP.HDMI_TX.fc_inhactiv1.byte) );
				Ddim_Print( ("fc_invactiv0       = %p\n",   IO_DISP.HDMI_TX.fc_invactiv0.byte) );
				Ddim_Print( ("fc_invactiv1       = %p\n",   IO_DISP.HDMI_TX.fc_invactiv1.byte) );
				Ddim_Print( ("fc_invact_2d0      = %p\n",   IO_DISP.HDMI_TX.fc_invact_2d0.byte) );
				Ddim_Print( ("fc_invact_2d1      = %p\n",   IO_DISP.HDMI_TX.fc_invact_2d1.byte) );
				Ddim_Print( ("fc_actspc_hdlr_cfg = %p\n",   IO_DISP.HDMI_TX.fc_actspc_hdlr_cfg.byte) );
				Ddim_Print( ("fc_inhblank0       = %p\n",   IO_DISP.HDMI_TX.fc_inhblank0.byte) );
				Ddim_Print( ("fc_inhblank1       = %p\n",   IO_DISP.HDMI_TX.fc_inhblank1.byte) );
				Ddim_Print( ("fc_invblank        = 0x%x\n", IO_DISP.HDMI_TX.fc_invblank) );
				Ddim_Print( ("fc_hsyncindelay0   = %p\n",   IO_DISP.HDMI_TX.fc_hsyncindelay0.byte) );
				Ddim_Print( ("fc_hsyncindelay1   = %p\n",   IO_DISP.HDMI_TX.fc_hsyncindelay1.byte) );
				Ddim_Print( ("fc_vsyncindelay    = 0x%x\n", IO_DISP.HDMI_TX.fc_vsyncindelay) );
				Ddim_Print( ("fc_hsyncinwidth0   = %p\n",   IO_DISP.HDMI_TX.fc_hsyncinwidth0.byte) );
				Ddim_Print( ("fc_hsyncinwidth1   = %p\n",   IO_DISP.HDMI_TX.fc_hsyncinwidth1.byte) );
				Ddim_Print( ("fc_vsyncinwidth    = %p\n",   IO_DISP.HDMI_TX.fc_vsyncinwidth.byte) );
				Ddim_Print( ("PLL\n") );
				Ddim_Print( (" - vp_pr_cd         = %p\n",   IO_DISP.HDMI_TX.vp_pr_cd.byte) );
				Ddim_Print( (" - phy_i2cm_address = 0x%x\n", IO_DISP.HDMI_TX.phy_i2cm_address) );
				Ddim_Print( (" - phy_i2cm_datao_0 = 0x%x\n", IO_DISP.HDMI_TX.phy_i2cm_datao_0) );
				Ddim_Print( (" - phy_i2cm_datao_1 = 0x%x\n", IO_DISP.HDMI_TX.phy_i2cm_datao_1) );
				Ddim_Print( ("Audio\n") );
				Ddim_Print( (" - aud_conf0      = %p\n",   IO_DISP.HDMI_TX.aud_conf0.byte) );
				Ddim_Print( (" - aud_conf1      = %p\n",   IO_DISP.HDMI_TX.aud_conf1.byte) );
				Ddim_Print( (" - aud_conf2      = %p\n",   IO_DISP.HDMI_TX.aud_conf2.byte) );
				Ddim_Print( (" - aud_spdif1     = %p\n",   IO_DISP.HDMI_TX.aud_spdif1.byte) );
				Ddim_Print( (" - aud_inputclkfs = %p\n",   IO_DISP.HDMI_TX.aud_inputclkfs.byte) );
				Ddim_Print( (" - aud_n1         = 0x%x\n", IO_DISP.HDMI_TX.aud_n1) );
				Ddim_Print( (" - aud_n2         = 0x%x\n", IO_DISP.HDMI_TX.aud_n2) );
				Ddim_Print( (" - aud_n3         = %p\n",   IO_DISP.HDMI_TX.aud_n3.byte) );
				Ddim_Print( (" - aud_cts1       = 0x%x\n", IO_DISP.HDMI_TX.aud_cts1) );
				Ddim_Print( (" - aud_cts2       = 0x%x\n", IO_DISP.HDMI_TX.aud_cts2) );
				Ddim_Print( (" - aud_cts3       = %p\n",   IO_DISP.HDMI_TX.aud_cts3.byte) );
				Ddim_Print( ("Infoframes\n") );
				Ddim_Print( (" - fc_aviconf0       = %p\n",   IO_DISP.HDMI_TX.fc_aviconf0.byte) );
				Ddim_Print( (" - fc_aviconf1       = %p\n",   IO_DISP.HDMI_TX.fc_aviconf1.byte) );
				Ddim_Print( (" - fc_aviconf2       = %p\n",   IO_DISP.HDMI_TX.fc_aviconf2.byte) );
				Ddim_Print( (" - fc_aviconf3       = %p\n",   IO_DISP.HDMI_TX.fc_aviconf3.byte) );
				Ddim_Print( (" - fc_avivid         = %p\n",   IO_DISP.HDMI_TX.fc_avivid.byte) );
				Ddim_Print( (" - fc_prconf         = %p\n",   IO_DISP.HDMI_TX.fc_prconf.byte) );
				Ddim_Print( (" - fc_audiconf0      = %p\n",   IO_DISP.HDMI_TX.fc_audiconf0.byte) );
				Ddim_Print( (" - fc_audiconf1      = %p\n",   IO_DISP.HDMI_TX.fc_audiconf1.byte) );
				Ddim_Print( (" - fc_audiconf2      = 0x%x\n", IO_DISP.HDMI_TX.fc_audiconf2) );
				Ddim_Print( (" - fc_audiconf3      = %p\n",   IO_DISP.HDMI_TX.fc_audiconf3.byte) );
				Ddim_Print( (" - fc_vsdieeeid0     = 0x%x\n", IO_DISP.HDMI_TX.fc_vsdieeeid0) );
				Ddim_Print( (" - fc_vsdieeeid1     = 0x%x\n", IO_DISP.HDMI_TX.fc_vsdieeeid1) );
				Ddim_Print( (" - fc_vsdieeeid2     = 0x%x\n", IO_DISP.HDMI_TX.fc_vsdieeeid2) );
				for (loop1 = 0; loop1 < 24; loop1++) {
					Ddim_Print( (" - fc_vsdpayload[%d] = 0x%x\n", loop1, IO_DISP.HDMI_TX.fc_vsdpayload[loop1].bit.fc_vsdpayload) );
				}
				Ddim_Print( ("----------\n") );
			}
			else {
				Ddim_Print(("Im_HDMI_Ctrl() : Error  err_code=0x%x\n", ret));
			}
		}
		else if( strcmp(argv[2], "get") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// table clear.
				memset( &ctrl, 0, sizeof( T_IM_HDMI_CTRL ) );
			}
			else {
				// Invalid parameter (ctrl = NULL)
				ret = Im_HDMI_Get_Ctrl(NULL);
				Ddim_Print(("Im_HDMI_Get_Ctrl() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			ret = Im_HDMI_Get_Ctrl(&ctrl);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_HDMI_Get_Ctrl() : Normal end\n"));
				// check register
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("result:\n") );
				Ddim_Print( ("video_input_mapping = 0x%x\n", ctrl.video_input_mapping) );
				Ddim_Print( ("vsync_in_polarity   = 0x%x\n", ctrl.vsync_in_polarity) );
				Ddim_Print( ("hsync_in_polarity   = 0x%x\n", ctrl.hsync_in_polarity) );
				Ddim_Print( ("de_in_polarity      = 0x%x\n", ctrl.de_in_polarity) );
				Ddim_Print( ("DVI_modez           = 0x%x\n", ctrl.DVI_modez) );
				Ddim_Print( ("r_v_blank_in_osc    = 0x%x\n", ctrl.r_v_blank_in_osc) );
				Ddim_Print( ("in_i_p              = 0x%x\n", ctrl.in_i_p) );
				Ddim_Print( ("h_in_activ          = 0x%x\n", ctrl.h_in_activ) );
				Ddim_Print( ("v_in_activ          = 0x%x\n", ctrl.v_in_activ) );
				Ddim_Print( ("fc_invact_2d        = 0x%x\n", ctrl.fc_invact_2d) );
				Ddim_Print( ("fc_actspc_hdlr_cfg  = 0x%x\n", ctrl.fc_actspc_hdlr_cfg.byte) );
				Ddim_Print( ("fc_inhblank         = 0x%x\n", ctrl.fc_inhblank) );
				Ddim_Print( ("fc_invblank         = 0x%x\n", ctrl.fc_invblank) );
				Ddim_Print( ("fc_hsyncindelay     = 0x%x\n", ctrl.fc_hsyncindelay) );
				Ddim_Print( ("fc_hsyncinwidth     = 0x%x\n", ctrl.fc_hsyncinwidth) );
				Ddim_Print( ("fc_vsyncindelay     = 0x%x\n", ctrl.fc_vsyncindelay) );
				Ddim_Print( ("fc_vsyncinwidth     = 0x%x\n", ctrl.fc_vsyncinwidth) );
				Ddim_Print( ("pll_config\n") );
				Ddim_Print( (" - vp_pr_cd          = 0x%x\n", ctrl.pll_config.vp_pr_cd.byte) );
//				Ddim_Print( (" - phy_i2cm_address  = 0x%x\n", ctrl.pll_config.phy_i2cm_address) );
//				Ddim_Print( (" - phy_i2cm_datao[0] = 0x%x\n", ctrl.pll_config.phy_i2cm_datao[0]) );
//				Ddim_Print( (" - phy_i2cm_datao[1] = 0x%x\n", ctrl.pll_config.phy_i2cm_datao[1]) );
				Ddim_Print( ("audio_confi\n") );
				Ddim_Print( (" - i2s_select      = 0x%x\n", ctrl.audio_config.i2s_select) );
				Ddim_Print( (" - aud_conf1       = 0x%x\n", ctrl.audio_config.aud_conf1.byte) );
				Ddim_Print( (" - aud_conf2       = 0x%x\n", ctrl.audio_config.aud_conf2.byte) );
				Ddim_Print( (" - aud_spdif1      = 0x%x\n", ctrl.audio_config.aud_spdif1.byte) );
				Ddim_Print( (" - aud_inputclkfs  = 0x%x\n", ctrl.audio_config.aud_inputclkfs) );
				Ddim_Print( (" - aud_n1          = 0x%x\n", ctrl.audio_config.aud_n1) );
				Ddim_Print( (" - aud_n2          = 0x%x\n", ctrl.audio_config.aud_n2) );
				Ddim_Print( (" - aud_n3_audn     = 0x%x\n", ctrl.audio_config.aud_n3_audn) );
				Ddim_Print( (" - cts_manual      = 0x%x\n", ctrl.audio_config.cts_manual) );
				Ddim_Print( (" - aud_cts1        = 0x%x\n", ctrl.audio_config.aud_cts1) );
				Ddim_Print( (" - aud_cts2        = 0x%x\n", ctrl.audio_config.aud_cts2) );
				Ddim_Print( (" - aud_cts3_audcts = 0x%x\n", ctrl.audio_config.aud_cts3_audcts) );
				Ddim_Print( ("info_frames\n") );
				Ddim_Print( (" - fc_aviconf0             = 0x%x\n", ctrl.info_frames.fc_aviconf0.byte) );
				Ddim_Print( (" - fc_aviconf1             = 0x%x\n", ctrl.info_frames.fc_aviconf1.byte) );
				Ddim_Print( (" - fc_aviconf2             = 0x%x\n", ctrl.info_frames.fc_aviconf2.byte) );
				Ddim_Print( (" - fc_aviconf3             = 0x%x\n", ctrl.info_frames.fc_aviconf3.byte) );
				Ddim_Print( (" - fc_avivid               = 0x%x\n", ctrl.info_frames.fc_avivid) );
				Ddim_Print( (" - fc_prconf               = 0x%x\n", ctrl.info_frames.fc_prconf.byte) );
				Ddim_Print( (" - fc_audiconf0            = 0x%x\n", ctrl.info_frames.fc_audiconf0.byte) );
				Ddim_Print( (" - fc_audiconf1            = 0x%x\n", ctrl.info_frames.fc_audiconf1.byte) );
				Ddim_Print( (" - fc_audiconf2            = 0x%x\n", ctrl.info_frames.fc_audiconf2) );
				Ddim_Print( (" - fc_audiconf3            = 0x%x\n", ctrl.info_frames.fc_audiconf3.byte) );
				Ddim_Print( (" - fc_vsdieeeid[0]         = 0x%x\n", ctrl.info_frames.fc_vsdieeeid[0]) );
				Ddim_Print( (" - fc_vsdieeeid[1]         = 0x%x\n", ctrl.info_frames.fc_vsdieeeid[1]) );
				Ddim_Print( (" - fc_vsdieeeid[2]         = 0x%x\n", ctrl.info_frames.fc_vsdieeeid[2]) );
				Ddim_Print( (" - video_mode              = 0x%x\n", ctrl.info_frames.video_mode) );
				Ddim_Print( (" - video_format            = 0x%x\n", ctrl.info_frames.video_format) );
				Ddim_Print( (" - meta_present            = 0x%x\n", ctrl.info_frames.meta_present) );
				Ddim_Print( (" - additional_video_format = 0x%x\n", ctrl.info_frames.additional_video_format) );
				Ddim_Print( (" - meta_type               = 0x%x\n", ctrl.info_frames.meta_type) );
				Ddim_Print( (" - meta_length             = 0x%x\n", ctrl.info_frames.meta_length) );
				Ddim_Print( (" - meta_data[0]            = 0x%x\n", ctrl.info_frames.meta_data[0]) );
				Ddim_Print( (" - meta_data[1]            = 0x%x\n", ctrl.info_frames.meta_data[1]) );
				Ddim_Print( (" - meta_data[2]            = 0x%x\n", ctrl.info_frames.meta_data[2]) );
				Ddim_Print( (" - meta_data[3]            = 0x%x\n", ctrl.info_frames.meta_data[3]) );
				Ddim_Print( (" - meta_data[4]            = 0x%x\n", ctrl.info_frames.meta_data[4]) );
				Ddim_Print( (" - meta_data[5]            = 0x%x\n", ctrl.info_frames.meta_data[5]) );
				Ddim_Print( (" - meta_data[6]            = 0x%x\n", ctrl.info_frames.meta_data[6]) );
				Ddim_Print( (" - meta_data[7]            = 0x%x\n", ctrl.info_frames.meta_data[7]) );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("tx_invid0          = %p\n",   IO_DISP.HDMI_TX.tx_invid0.byte) );
				Ddim_Print( ("fc_invidconf       = %p\n",   IO_DISP.HDMI_TX.fc_invidconf.byte) );
				Ddim_Print( ("fc_inhactiv0       = %p\n",   IO_DISP.HDMI_TX.fc_inhactiv0.byte) );
				Ddim_Print( ("fc_inhactiv1       = %p\n",   IO_DISP.HDMI_TX.fc_inhactiv1.byte) );
				Ddim_Print( ("fc_invactiv0       = %p\n",   IO_DISP.HDMI_TX.fc_invactiv0.byte) );
				Ddim_Print( ("fc_invactiv1       = %p\n",   IO_DISP.HDMI_TX.fc_invactiv1.byte) );
				Ddim_Print( ("fc_invact_2d0      = %p\n",   IO_DISP.HDMI_TX.fc_invact_2d0.byte) );
				Ddim_Print( ("fc_invact_2d1      = %p\n",   IO_DISP.HDMI_TX.fc_invact_2d1.byte) );
				Ddim_Print( ("fc_actspc_hdlr_cfg = %p\n",   IO_DISP.HDMI_TX.fc_actspc_hdlr_cfg.byte) );
				Ddim_Print( ("fc_inhblank0       = %p\n",   IO_DISP.HDMI_TX.fc_inhblank0.byte) );
				Ddim_Print( ("fc_inhblank1       = %p\n",   IO_DISP.HDMI_TX.fc_inhblank1.byte) );
				Ddim_Print( ("fc_invblank        = 0x%x\n", IO_DISP.HDMI_TX.fc_invblank) );
				Ddim_Print( ("fc_hsyncindelay0   = %p\n",   IO_DISP.HDMI_TX.fc_hsyncindelay0.byte) );
				Ddim_Print( ("fc_hsyncindelay1   = %p\n",   IO_DISP.HDMI_TX.fc_hsyncindelay1.byte) );
				Ddim_Print( ("fc_vsyncindelay    = 0x%x\n", IO_DISP.HDMI_TX.fc_vsyncindelay) );
				Ddim_Print( ("fc_hsyncinwidth0   = %p\n",   IO_DISP.HDMI_TX.fc_hsyncinwidth0.byte) );
				Ddim_Print( ("fc_hsyncinwidth1   = %p\n",   IO_DISP.HDMI_TX.fc_hsyncinwidth1.byte) );
				Ddim_Print( ("fc_vsyncinwidth    = %p\n",   IO_DISP.HDMI_TX.fc_vsyncinwidth.byte) );
				Ddim_Print( ("PLL\n") );
				Ddim_Print( (" - vp_pr_cd         = %p\n",   IO_DISP.HDMI_TX.vp_pr_cd.byte) );
				Ddim_Print( (" - phy_i2cm_address = 0x%x\n", IO_DISP.HDMI_TX.phy_i2cm_address) );
				Ddim_Print( (" - phy_i2cm_datao_0 = 0x%x\n", IO_DISP.HDMI_TX.phy_i2cm_datao_0) );
				Ddim_Print( (" - phy_i2cm_datao_1 = 0x%x\n", IO_DISP.HDMI_TX.phy_i2cm_datao_1) );
				Ddim_Print( ("Audio\n") );
				Ddim_Print( (" - aud_conf0      = %p\n",   IO_DISP.HDMI_TX.aud_conf0.byte) );
				Ddim_Print( (" - aud_conf1      = %p\n",   IO_DISP.HDMI_TX.aud_conf1.byte) );
				Ddim_Print( (" - aud_conf2      = %p\n",   IO_DISP.HDMI_TX.aud_conf2.byte) );
				Ddim_Print( (" - aud_spdif1     = %p\n",   IO_DISP.HDMI_TX.aud_spdif1.byte) );
				Ddim_Print( (" - aud_inputclkfs = %p\n",   IO_DISP.HDMI_TX.aud_inputclkfs.byte) );
				Ddim_Print( (" - aud_n1         = 0x%x\n", IO_DISP.HDMI_TX.aud_n1) );
				Ddim_Print( (" - aud_n2         = 0x%x\n", IO_DISP.HDMI_TX.aud_n2) );
				Ddim_Print( (" - aud_n3         = %p\n",   IO_DISP.HDMI_TX.aud_n3.byte) );
				Ddim_Print( (" - aud_cts1       = 0x%x\n", IO_DISP.HDMI_TX.aud_cts1) );
				Ddim_Print( (" - aud_cts2       = 0x%x\n", IO_DISP.HDMI_TX.aud_cts2) );
				Ddim_Print( (" - aud_cts3       = %p\n",   IO_DISP.HDMI_TX.aud_cts3.byte) );
				Ddim_Print( ("Infoframes\n") );
				Ddim_Print( (" - fc_aviconf0       = %p\n",   IO_DISP.HDMI_TX.fc_aviconf0.byte) );
				Ddim_Print( (" - fc_aviconf1       = %p\n",   IO_DISP.HDMI_TX.fc_aviconf1.byte) );
				Ddim_Print( (" - fc_aviconf2       = %p\n",   IO_DISP.HDMI_TX.fc_aviconf2.byte) );
				Ddim_Print( (" - fc_aviconf3       = %p\n",   IO_DISP.HDMI_TX.fc_aviconf3.byte) );
				Ddim_Print( (" - fc_avivid         = %p\n",   IO_DISP.HDMI_TX.fc_avivid.byte) );
				Ddim_Print( (" - fc_prconf         = %p\n",   IO_DISP.HDMI_TX.fc_prconf.byte) );
				Ddim_Print( (" - fc_audiconf0      = %p\n",   IO_DISP.HDMI_TX.fc_audiconf0.byte) );
				Ddim_Print( (" - fc_audiconf1      = %p\n",   IO_DISP.HDMI_TX.fc_audiconf1.byte) );
				Ddim_Print( (" - fc_audiconf2      = 0x%x\n", IO_DISP.HDMI_TX.fc_audiconf2) );
				Ddim_Print( (" - fc_audiconf3      = %p\n",   IO_DISP.HDMI_TX.fc_audiconf3.byte) );
				Ddim_Print( (" - fc_vsdieeeid0     = 0x%x\n", IO_DISP.HDMI_TX.fc_vsdieeeid0) );
				Ddim_Print( (" - fc_vsdieeeid1     = 0x%x\n", IO_DISP.HDMI_TX.fc_vsdieeeid1) );
				Ddim_Print( (" - fc_vsdieeeid2     = 0x%x\n", IO_DISP.HDMI_TX.fc_vsdieeeid2) );
				for (loop1 = 0; loop1 < 24; loop1++) {
					Ddim_Print( (" - fc_vsdpayload[%d] = 0x%x\n", loop1, IO_DISP.HDMI_TX.fc_vsdpayload[loop1].bit.fc_vsdpayload) );
				}
				Ddim_Print( ("----------\n") );
			}
			else {
				Ddim_Print(("Im_HDMI_Get_Ctrl() : Error  err_code=0x%x\n", ret));
			}
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud\n"));
			return;
		}
	}
#if 0
	else if ( strcmp(argv[1], "clk") == 0 ) {
		// 2nd param check
		if( strcmp(argv[2], "set") == 0 ) {
			// input parameter clear.
			pll_curr_ctrl.hword = 0;
			op_mode.hword = 0;
			
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// set parameter.
				gmp_ctrl = 0;
				pll_curr_ctrl.bit.mpll_int_cntrl	= 0;
				pll_curr_ctrl.bit.mpll_prop_cntrl	= 0;
				op_mode.bit.n_cntrl					= 0;
				op_mode.bit.ref_cntrl				= E_IM_HDMI_REF_CNTRL_DIVIDES_1;
				op_mode.bit.fbdiv1_cntrl			= E_IM_HDMI_FBDIV1_CNTRL_DIVIDES_1;
				op_mode.bit.fbdiv2_cntrl			= E_IM_HDMI_FBDIV2_CNTRL_DIVIDES_1;
				op_mode.bit.opmode					= E_IM_HDMI_OP_MODE_HDMI14;
				op_mode.bit.tmdsmhl_cntrl			= 0;
				op_mode.bit.pre_pdiv				= E_IM_HDMI_PRE_PDIV_DIVIDES_2;
			}
			else if (param3 == 1) {
				// set parameter.
				gmp_ctrl							= 3;
				pll_curr_ctrl.bit.mpll_int_cntrl	= 7;
				pll_curr_ctrl.bit.mpll_prop_cntrl	= 7;
				op_mode.bit.n_cntrl					= 3;
				op_mode.bit.ref_cntrl				= E_IM_HDMI_REF_CNTRL_DIVIDES_4;
				op_mode.bit.fbdiv1_cntrl			= E_IM_HDMI_FBDIV1_CNTRL_DIVIDES_4;
				op_mode.bit.fbdiv2_cntrl			= E_IM_HDMI_FBDIV2_CNTRL_DIVIDES_6;
				op_mode.bit.opmode					= E_IM_HDMI_OP_MODE_HDMI20;
				op_mode.bit.tmdsmhl_cntrl			= 0;
				op_mode.bit.pre_pdiv				= E_IM_HDMI_PRE_PDIV_DIVIDES_1;
			}
			else {
				// end
				Ddim_Print(("!! 3rd parameter fraud : %d\n", param3));
				return;
			}
			
			Im_HDMI_Set_Clock(gmp_ctrl, pll_curr_ctrl, op_mode);
			Ddim_Print(("Im_HDMI_Set_Clock() : Normal end\n"));
			ct_im_hdmi_pclk_on();
			// check register
			Ddim_Print( ("----------\n") );
			Ddim_Print( ("register:\n") );
			Ddim_Print( ("PLLCTRL       = 0x%x\n", IO_DISP.HDMI_TX.PLLCTRL.hword) );
			Ddim_Print( ("PLLGMPCTRL    = 0x%x\n", IO_DISP.HDMI_TX.PLLGMPCTRL.hword) );
			Ddim_Print( ("PLLCURRCTRL   = 0x%x\n", IO_DISP.HDMI_TX.PLLCURRCTRL.hword) );
			Ddim_Print( ("OPMODE_PLLCFG = 0x%x\n", IO_DISP.HDMI_TX.OPMODE_PLLCFG.hword) );
			Ddim_Print( ("----------\n") );
			ct_im_hdmi_pclk_off();
		}
		else if( strcmp(argv[2], "get") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// output parameter clear.
				gmp_ctrl = 0;
				pll_curr_ctrl.hword = 0;
				op_mode.hword = 0;
			}
			else if (param3 == 1) {
				// Invalid parameter (gmp_ctrl = NULL)
				ret = Im_HDMI_Get_Clock(NULL, &pll_curr_ctrl, &op_mode);
				Ddim_Print(("Im_HDMI_Get_Clock() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			else if (param3 == 2) {
				// Invalid parameter (pll_curr_ctrl = NULL)
				ret = Im_HDMI_Get_Clock(&gmp_ctrl, NULL, &op_mode);
				Ddim_Print(("Im_HDMI_Get_Clock() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			else if (param3 == 3) {
				// Invalid parameter (op_mode = NULL)
				ret = Im_HDMI_Get_Clock(&gmp_ctrl, &pll_curr_ctrl, NULL);
				Ddim_Print(("Im_HDMI_Get_Clock() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			else {
				// Invalid parameter (NULL)
				ret = Im_HDMI_Get_Clock(NULL, NULL, NULL);
				Ddim_Print(("Im_HDMI_Get_Clock() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			ret = Im_HDMI_Get_Clock(&gmp_ctrl, &pll_curr_ctrl, &op_mode);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_HDMI_Get_Clock() : Normal end\n"));
				ct_im_hdmi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("result:\n") );
				Ddim_Print( ("gmp_ctrl      : 0x%x\n", gmp_ctrl) );
				Ddim_Print( ("pll_curr_ctrl : 0x%x\n", pll_curr_ctrl.hword) );
				Ddim_Print( ("op_mode       : 0x%x\n", op_mode.hword) );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("PLLCTRL       = 0x%x\n", IO_DISP.HDMI_TX.PLLCTRL.hword) );
				Ddim_Print( ("PLLGMPCTRL    = 0x%x\n", IO_DISP.HDMI_TX.PLLGMPCTRL.hword) );
				Ddim_Print( ("PLLCURRCTRL   = 0x%x\n", IO_DISP.HDMI_TX.PLLCURRCTRL.hword) );
				Ddim_Print( ("OPMODE_PLLCFG = 0x%x\n", IO_DISP.HDMI_TX.OPMODE_PLLCFG.hword) );
				Ddim_Print( ("----------\n") );
				ct_im_hdmi_pclk_off();
			}
			else {
				Ddim_Print(("Im_HDMI_Get_Clock() : Error  err_code=0x%x\n", ret));
			}
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud\n"));
			return;
		}
	}
	else if ( strcmp(argv[1], "term") == 0 ) {
		// 2nd param check
		if( strcmp(argv[2], "set") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if ((param3 >= E_IM_HDMI_TXTERM_50OHM) && (param3 <= E_IM_HDMI_TXTERM_OPEN_CIRCUIT)) {
				// set paramter.
				txterm	= (E_IM_HDMI_TXTERM)param3;
			}
			else {
				// end
				Ddim_Print(("!! 3rd parameter fraud : %d\n", param3));
				return;
			}

			Im_HDMI_Set_Tx_Term(txterm);
			Ddim_Print(("Im_HDMI_Set_Tx_Term() : Normal end\n"));
			ct_im_hdmi_pclk_on();
			// check register
			Ddim_Print( ("----------\n") );
			Ddim_Print( ("register:\n") );
			Ddim_Print( ("TXTERM = 0x%x\n", IO_DISP.HDMI_TX.TXTERM.hword) );
			Ddim_Print( ("----------\n") );
			ct_im_hdmi_pclk_off();
		}
		else if( strcmp(argv[2], "get") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// output paramter clear.
				txterm = E_IM_HDMI_TXTERM_50OHM;
			}
			else {
				// Invalid parameter (txterm = NULL)
				ret = Im_HDMI_Get_Tx_Term(NULL);
				Ddim_Print(("Im_HDMI_Get_Tx_Term() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			ret = Im_HDMI_Get_Tx_Term(&txterm);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_HDMI_Get_Tx_Term() : Normal end\n"));
				ct_im_hdmi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("result:\n") );
				Ddim_Print( ("txterm : 0x%x\n", txterm) );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("TXTERM = 0x%x\n", IO_DISP.HDMI_TX.TXTERM.hword) );
				Ddim_Print( ("----------\n") );
				ct_im_hdmi_pclk_off();
			}
			else {
				Ddim_Print(("Im_HDMI_Get_Tx_Term() : Error  err_code=0x%x\n", ret));
			}
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud\n"));
			return;
		}
	}
	else if ( strcmp(argv[1], "tc") == 0 ) {
		// 2nd param check
		if( strcmp(argv[2], "set") == 0 ) {
			// input paramter clear.
			vlevctrl.hword = 0;
			
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// set parameter.
				cksymtxctrl.bit.Ck_symon	= D_IM_HDMI_ENABLE_OFF;
				cksymtxctrl.bit.Tx_trbon	= D_IM_HDMI_ENABLE_OFF;
				cksymtxctrl.bit.Tx_traon	= D_IM_HDMI_ENABLE_OFF;
				cksymtxctrl.bit.Tx_symon	= D_IM_HDMI_ENABLE_OFF;
				cksymtxctrl.bit.slopeboost	= E_IM_HDMI_SLOPE_BOOSTING_HDMI14_297GBPS;
				cksymtxctrl.bit.Override	= 0;
			}
			else if (param3 == 1) {
				// set parameter.
				cksymtxctrl.bit.Ck_symon	= D_IM_HDMI_ENABLE_ON;
				cksymtxctrl.bit.Tx_trbon	= D_IM_HDMI_ENABLE_ON;
				cksymtxctrl.bit.Tx_traon	= D_IM_HDMI_ENABLE_ON;
				cksymtxctrl.bit.Tx_symon	= D_IM_HDMI_ENABLE_ON;
				cksymtxctrl.bit.slopeboost	= E_IM_HDMI_SLOPE_BOOSTING_HDMI20;
				cksymtxctrl.bit.Override	= 1;
			}
			else {
				// end
				Ddim_Print(("!! 3rd parameter fraud : %d\n", param3));
				return;
			}
			
			Im_HDMI_Set_Transmitter_Control(cksymtxctrl);
			Ddim_Print(("Im_HDMI_Set_Transmitter_Control() : Normal end\n"));
			ct_im_hdmi_pclk_on();
			// check register
			Ddim_Print( ("----------\n") );
			Ddim_Print( ("register:\n") );
			Ddim_Print( ("CKSYMTXCTRL = 0x%x\n", IO_DISP.HDMI_TX.CKSYMTXCTRL.hword) );
			Ddim_Print( ("----------\n") );
			ct_im_hdmi_pclk_off();
		}
		else if( strcmp(argv[2], "get") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// output parameter clear.
				vlevctrl.hword = 0;
			}
			else {
				// Invalid parameter (vlevctrl = NULL)
				ret = Im_HDMI_Get_Transmitter_Control(NULL);
				Ddim_Print(("Im_HDMI_Get_Transmitter_Control() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			ret = Im_HDMI_Get_Transmitter_Control(&cksymtxctrl);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_HDMI_Get_Transmitter_Control() : Normal end\n"));
				ct_im_hdmi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("result:\n") );
				Ddim_Print( ("cksymtxctrl : 0x%x\n", cksymtxctrl.hword) );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("CKSYMTXCTRL = 0x%x\n", IO_DISP.HDMI_TX.CKSYMTXCTRL.hword) );
				Ddim_Print( ("----------\n") );
				ct_im_hdmi_pclk_off();
			}
			else {
				Ddim_Print(("Im_HDMI_Get_Transmitter_Control() : Error  err_code=0x%x\n", ret));
			}
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud\n"));
			return;
		}
	}
	else if ( strcmp(argv[1], "vol") == 0 ) {
		// 2nd param check
		if( strcmp(argv[2], "set") == 0 ) {
			// input parameter clear.
			vlevctrl.hword = 0;
			
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// set parameter.
				vlevctrl.bit.sup_ck_lvl	= E_IM_HDMI_SUP_LVL_HDMI2_34_6GBPS;
				vlevctrl.bit.sup_tx_lvl	= E_IM_HDMI_SUP_LVL_HDMI2_34_6GBPS;
				vlevctrl.bit.Override	= 0;
			}
			else if (param3 == 1) {
				// set parameter.
				vlevctrl.bit.sup_ck_lvl	= E_IM_HDMI_SUP_LVL_HDMI14_222GBPS;
				vlevctrl.bit.sup_tx_lvl	= E_IM_HDMI_SUP_LVL_HDMI14_222GBPS;
				vlevctrl.bit.Override	= 1;
			}
			else {
				// end
				Ddim_Print(("!! 3rd parameter fraud : %d\n", param3));
				return;
			}
			
			Im_HDMI_Set_Voltage_Level(vlevctrl);
			Ddim_Print(("Im_HDMI_Set_Voltage_Level() : Normal end\n"));
			ct_im_hdmi_pclk_on();
			// check register
			Ddim_Print( ("----------\n") );
			Ddim_Print( ("register:\n") );
			Ddim_Print( ("VLEVCTRL = 0x%x\n", IO_DISP.HDMI_TX.VLEVCTRL.hword) );
			Ddim_Print( ("----------\n") );
			ct_im_hdmi_pclk_off();
		}
		else if( strcmp(argv[2], "get") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// output parameter clear.
				vlevctrl.hword = 0;
			}
			else {
				// Invalid parameter (vlevctrl = NULL)
				ret = Im_HDMI_Get_Voltage_Level(NULL);
				Ddim_Print(("Im_HDMI_Get_Voltage_Level() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			ret = Im_HDMI_Get_Voltage_Level(&vlevctrl);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_HDMI_Get_Voltage_Level() : Normal end\n"));
				ct_im_hdmi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("result:\n") );
				Ddim_Print( ("vlevctrl : 0x%x\n", vlevctrl.hword) );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("VLEVCTRL = 0x%x\n", IO_DISP.HDMI_TX.VLEVCTRL.hword) );
				Ddim_Print( ("----------\n") );
				ct_im_hdmi_pclk_off();
			}
			else {
				Ddim_Print(("Im_HDMI_Get_Voltage_Level() : Error  err_code=0x%x\n", ret));
			}
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud\n"));
			return;
		}
	}
#endif
	else if ( strcmp(argv[1], "csc") == 0 ) {
		// 2nd param check
		if( strcmp(argv[2], "set") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// 4th param check
				param4 = atoi((const char *)argv[4]);
				if (param4 == 0) {
					// set parameter.
					csc_config.csc_cfg.bit.decmode				= 0;							// Chroma decimation configuration
					csc_config.csc_cfg.bit.intmode				= 0;							// Chroma interpolation configuration.
					csc_config.csc_cfg.bit.csc_limit			= D_IM_HDMI_ENABLE_OFF;			// CSC limit setting disable.
					csc_config.csc_scale.bit.cscscale			= 0;							// Scale factor.
					csc_config.csc_scale.bit.csc_color_depth	= E_IM_HDMI_COLOR_DEPTH_8BIT;	// csc_color_depth.
				}
				else if (param4 == 1) {
					// set parameter.
					csc_config.csc_cfg.bit.decmode				= 3;							// Chroma decimation configuration
					csc_config.csc_cfg.bit.intmode				= 3;							// Chroma interpolation configuration.
					csc_config.csc_cfg.bit.csc_limit			= D_IM_HDMI_ENABLE_ON;			// CSC limit setting enable.
					csc_config.csc_scale.bit.cscscale			= 2;							// Scale factor.
					csc_config.csc_scale.bit.csc_color_depth	= E_IM_HDMI_COLOR_DEPTH_16BIT;	// csc_color_depth.
				}
				else {
					// end
					Ddim_Print(("!! 4th parameter fraud : %d\n", param4));
					return;
				}
			}
			else if (param3 == 1) {
				// Invalid parameter (config = NULL)
				ret = Im_HDMI_CSC(NULL, gCt_Im_Hdmi_Csc_Matrix);
				Ddim_Print(("Im_HDMI_CSC() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			else {
				// Invalid parameter (matrix = NULL)
				ret = Im_HDMI_CSC(&csc_config, NULL);
				Ddim_Print(("Im_HDMI_CSC() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			ret = Im_HDMI_CSC(&csc_config, gCt_Im_Hdmi_Csc_Matrix);
			Ddim_Print(("Im_HDMI_CSC() : Normal end\n"));
			ct_im_hdmi_pclk_on();
			// check register
			Ddim_Print( ("----------\n") );
			Ddim_Print( ("register:\n") );
			Ddim_Print( ("csc_cfg          = %p\n", IO_DISP.HDMI_TX.csc_cfg.byte) );
			Ddim_Print( ("csc_scale        = %p\n", IO_DISP.HDMI_TX.csc_scale.byte) );
			for (loop1 = 0; loop1 < D_IM_HDMI_CSC_CONF_ROW_NUM; loop1++) {
					Ddim_Print( ("csc_coef_a%d_msb = 0x%x\n", loop1, IO_DISP.HDMI_TX.csc_coef[0][loop1].msb) );
					Ddim_Print( ("csc_coef_a%d_lsb = 0x%x\n", loop1, IO_DISP.HDMI_TX.csc_coef[0][loop1].lsb) );
			}
			for (loop1 = 0; loop1 < D_IM_HDMI_CSC_CONF_ROW_NUM; loop1++) {
					Ddim_Print( ("csc_coef_b%d_msb = 0x%x\n", loop1, IO_DISP.HDMI_TX.csc_coef[1][loop1].msb) );
					Ddim_Print( ("csc_coef_b%d_lsb = 0x%x\n", loop1, IO_DISP.HDMI_TX.csc_coef[1][loop1].lsb) );
			}
			for (loop1 = 0; loop1 < D_IM_HDMI_CSC_CONF_ROW_NUM; loop1++) {
					Ddim_Print( ("csc_coef_c%d_msb = 0x%x\n", loop1, IO_DISP.HDMI_TX.csc_coef[2][loop1].msb) );
					Ddim_Print( ("csc_coef_c%d_lsb = 0x%x\n", loop1, IO_DISP.HDMI_TX.csc_coef[2][loop1].lsb) );
			}
			Ddim_Print( ("----------\n") );
			ct_im_hdmi_pclk_off();
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud\n"));
			return;
		}
	}
	else if ( strcmp(argv[1], "vs") == 0 ) {
		// 2nd param check
		if( strcmp(argv[2], "set") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// set parameter.
				tx_invid0.bit.video_mapping			= E_IM_HDMI_VIDEO_MAPPING_RGB444_8BIT;
				tx_invid0.bit.internal_de_generator	= D_IM_HDMI_ENABLE_OFF;
			}
			else if (param3 == 1) {
				// set parameter.
				tx_invid0.bit.video_mapping			= E_IM_HDMI_VIDEO_MAPPING_YCC420_IPI_16BIT;
				tx_invid0.bit.internal_de_generator	= D_IM_HDMI_ENABLE_ON;
			}
			else {
				// end
				Ddim_Print(("!! 3rd parameter fraud : %d\n", param3));
				return;
			}
			
			Im_HDMI_Set_Video_Sampler(tx_invid0);
			Ddim_Print(("Im_HDMI_Set_Video_Sampler() : Normal end\n"));
			ct_im_hdmi_pclk_on();
			// check register
			Ddim_Print( ("----------\n") );
			Ddim_Print( ("register:\n") );
			Ddim_Print( ("tx_invid0 = %p\n", IO_DISP.HDMI_TX.tx_invid0.byte) );
			Ddim_Print( ("----------\n") );
			ct_im_hdmi_pclk_off();
		}
		else if( strcmp(argv[2], "get") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// output parameter clear.
				tx_invid0.byte = 0;
			}
			else {
				// Invalid parameter (tx_invid0 = NULL)
				ret = Im_HDMI_Get_Video_Sampler(NULL);
				Ddim_Print(("Im_HDMI_Get_Video_Sampler() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			ret = Im_HDMI_Get_Video_Sampler(&tx_invid0);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_HDMI_Get_Video_Sampler() : Normal end\n"));
				ct_im_hdmi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("result:\n") );
				Ddim_Print( ("tx_invid0 : 0x%x\n", tx_invid0.byte) );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("tx_invid0 = %p\n", IO_DISP.HDMI_TX.tx_invid0.byte) );
				Ddim_Print( ("----------\n") );
				ct_im_hdmi_pclk_off();
			}
			else {
				Ddim_Print(("Im_HDMI_Get_Video_Sampler() : Error  err_code=0x%x\n", ret));
			}
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud\n"));
			return;
		}
	}
	else if ( strcmp(argv[1], "vp") == 0 ) {
		// 2nd param check
		if( strcmp(argv[2], "set") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 != 0) {
				// Invalid parameter (audio = NULL)
				ret = Im_HDMI_Set_Video_Packetizer(NULL);
				Ddim_Print(("Im_HDMI_Set_Video_Packetizer() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			// table clear.
			memset( &video_packetizer, 0, sizeof( T_IM_HDMI_VIDEO_PACKETIZER ) );
			
			// 4th param check
			param4 = atoi((const char *)argv[4]);
			if (param4 == 0) {
				// set parameter.
				video_packetizer.vp_pr_cd.bit.desired_pr_factor	= 0;							// No pixel repetition (pixel sent only once)
				video_packetizer.vp_pr_cd.bit.color_depth		= E_IM_HDMI_COLOR_DEPTH_8BIT;	// Color depth
				video_packetizer.vp_remap						= E_IM_HDMI_VP_REMAP_16BIT;		// Video Packetizer YCC422 Remapping
				video_packetizer.vp_conf.bit.output_selector	= E_IM_HDMI_OUTPUT_SELECTOR_PIXEL_PACKING;	// Video Packetizer output selection
				video_packetizer.vp_conf.bit.bypass_select		= E_IM_HDMI_BYPASS_SELECT_PIXEL_REPEATER;	// bypass select
				video_packetizer.vp_conf.bit.ycc422_en			= D_IM_HDMI_ENABLE_OFF;	// YCC422 select disable
				video_packetizer.vp_conf.bit.pr_en				= D_IM_HDMI_ENABLE_OFF;	// Pixel repeater disable
				video_packetizer.vp_conf.bit.pp_en				= D_IM_HDMI_ENABLE_OFF;	// Pixel packing disable
				video_packetizer.vp_conf.bit.bypass_en			= D_IM_HDMI_ENABLE_OFF;	// Bypass disable
				
			}
			else if (param4 == 1) {
				// set parameter.
				video_packetizer.vp_pr_cd.bit.desired_pr_factor	= 9;							// Pixel sent 10 times
				video_packetizer.vp_pr_cd.bit.color_depth		= E_IM_HDMI_COLOR_DEPTH_16BIT;	// Color depth
				video_packetizer.vp_remap						= E_IM_HDMI_VP_REMAP_24BIT;		// Video Packetizer YCC422 Remapping
				video_packetizer.vp_conf.bit.output_selector	= E_IM_HDMI_OUTPUT_SELECTOR_8BIT_BYPASS;	// Video Packetizer output selection
				video_packetizer.vp_conf.bit.bypass_select		= E_IM_HDMI_BYPASS_SELECT_VIDEO_PACKETIZER;	// bypass select
				video_packetizer.vp_conf.bit.ycc422_en			= D_IM_HDMI_ENABLE_ON;	// YCC422 select enable
				video_packetizer.vp_conf.bit.pr_en				= D_IM_HDMI_ENABLE_ON;	// Pixel repeater enable
				video_packetizer.vp_conf.bit.pp_en				= D_IM_HDMI_ENABLE_ON;	// Pixel packing enable
				video_packetizer.vp_conf.bit.bypass_en			= D_IM_HDMI_ENABLE_ON;	// Bypass enable
			}
			else {
				// end
				Ddim_Print(("!! 4th parameter fraud : %d\n", param4));
				return;
			}
			
			ret = Im_HDMI_Set_Video_Packetizer(&video_packetizer);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_HDMI_Set_Video_Packetizer() : Normal end\n"));
				ct_im_hdmi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("vp_pr_cd = %p\n", IO_DISP.HDMI_TX.vp_pr_cd.byte) );
				Ddim_Print( ("vp_remap = %p\n", IO_DISP.HDMI_TX.vp_remap.byte) );
				Ddim_Print( ("vp_conf  = %p\n", IO_DISP.HDMI_TX.vp_conf.byte) );
				Ddim_Print( ("----------\n") );
				ct_im_hdmi_pclk_off();
			}
			else {
				Ddim_Print(("Im_HDMI_Set_Video_Packetizer() : Error  err_code=0x%x\n", ret));
			}
		}
		else if( strcmp(argv[2], "get") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// table clear.
				memset( &video_packetizer, 0, sizeof( T_IM_HDMI_VIDEO_PACKETIZER ) );
			}
			else {
				// Invalid parameter (video_packetizer = NULL)
				ret = Im_HDMI_Get_Video_Packetizer(NULL);
				Ddim_Print(("Im_HDMI_Get_Video_Packetizer() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			ret = Im_HDMI_Get_Video_Packetizer(&video_packetizer);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_HDMI_Get_Video_Packetizer() : Normal end\n"));
				ct_im_hdmi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("result:\n") );
				Ddim_Print( ("vp_pr_cd : 0x%x\n", video_packetizer.vp_pr_cd.byte) );
				Ddim_Print( ("vp_remap : 0x%x\n", video_packetizer.vp_remap) );
				Ddim_Print( ("vp_conf  : 0x%x\n", video_packetizer.vp_conf.byte) );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("vp_pr_cd = %p\n", IO_DISP.HDMI_TX.vp_pr_cd.byte) );
				Ddim_Print( ("vp_remap = %p\n", IO_DISP.HDMI_TX.vp_remap.byte) );
				Ddim_Print( ("vp_conf  = %p\n", IO_DISP.HDMI_TX.vp_conf.byte) );
				Ddim_Print( ("----------\n") );
				ct_im_hdmi_pclk_off();
			}
			else {
				Ddim_Print(("Im_HDMI_Get_Video_Packetizer() : Error  err_code=0x%x\n", ret));
			}
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud\n"));
			return;
		}
	}
	else if ( strcmp(argv[1], "fc") == 0 ) {
		// 2nd param check
		if( strcmp(argv[2], "set") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 != 0) {
				// Invalid parameter (audio = NULL)
				ret = Im_HDMI_Set_Frame_Composer(NULL);
				Ddim_Print(("Im_HDMI_Set_Frame_Composer() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			// table clear.
			memset( &frame_composer, 0, sizeof( T_IM_HDMI_FRAME_COMPOSER ) );
			
			// 4th param check
			param4 = atoi((const char *)argv[4]);
			if (param4 == 0) {
				// set parameter.
				frame_composer.fc_invidconf.bit.in_I_P						= D_IM_HDMI_IN_I_P_PROGRESSIVE;	// Input video mode
				frame_composer.fc_invidconf.bit.r_v_blank_in_osc			= D_IM_HDMI_ACTIVE_LOW;			// Used for CEA861-D modes with fractional Vblank
				frame_composer.fc_invidconf.bit.DVI_modez					= D_IM_HDMI_DVI_MODEZ_DVI;		// DVI mode selected
				frame_composer.fc_invidconf.bit.de_in_polarity				= D_IM_HDMI_ACTIVE_LOW;			// Data enable input polarity
				frame_composer.fc_invidconf.bit.hsync_in_polarity			= D_IM_HDMI_ACTIVE_LOW;			// Hsync input polarity
				frame_composer.fc_invidconf.bit.vsync_in_polarity			= D_IM_HDMI_ACTIVE_LOW;			// Vsync input polarity
				frame_composer.fc_invidconf.bit.HDCP_keepout				= D_IM_HDMI_ACTIVE_LOW;			// Start/stop HDCP keepout window generation
				frame_composer.fc_inhactiv									= 16838;	// fc_inhactiv: Frame Composer Input Video HActive Pixels
				frame_composer.fc_inhblank									= 8191;		// fc_inhblank: Frame Composer Input Video HBlank Pixels
				frame_composer.fc_invactiv									= 8191;		// fc_invactiv: Frame Composer Input Video VActive Pixels
				frame_composer.fc_invblank									= 255;		// fc_invblank: Frame Composer Input Video VBlank Pixels
				frame_composer.fc_hsyncindelay								= 8191;		// fc_hsyncindelay: Frame Composer Input Video HSync Front
				frame_composer.fc_hsyncinwidth								= 1023;		// fc_hsyncinwidth: Frame Composer Input Video HSync Width
				frame_composer.fc_vsyncindelay								= 255;		// fc_vsyncindelay: Frame Composer Input Video VSync Front Porch
				frame_composer.fc_vsyncinwidth								= 63;		// Frame Composer Input Video VSync Width
				frame_composer.fc_aviconf3.bit.CN							= 3;		// IT content type according to CEA the specification
				frame_composer.fc_aviconf3.bit.YQ							= 3;		// YCC Quantization range according to the CEA specification
				frame_composer.fc_aviconf0.bit.rgc_ycc_indication			= 0;		// Y1,Y0 RGB or YCC indicator : 0:RGB
				frame_composer.fc_aviconf0.bit.bar_information				= 0;		// Bar information data valid
				frame_composer.fc_aviconf0.bit.scan_information				= 0;		// Scan information
				frame_composer.fc_aviconf0.bit.active_format_present		= 0;		// Active format present
				frame_composer.fc_aviconf0.bit.rgc_ycc_indication_2			= 0;		// Y2, Bit 2 of rgc_ycc_indication
				frame_composer.fc_aviconf1.bit.active_aspect_ratio			= 0;		// Active aspect ratio
				frame_composer.fc_aviconf1.bit.picture_aspect_ratio			= 0;		// Picture aspect ratio
				frame_composer.fc_aviconf1.bit.Colorimetry					= 0;		// Colorimetry
				frame_composer.fc_aviconf2.bit.non_uniform_picture_scaling	= 0;		// Non-uniform picture scaling
				frame_composer.fc_aviconf2.bit.quantization_range			= 0;		// Quantization range
				frame_composer.fc_aviconf2.bit.extended_colorimetry			= 0;		// Extended colorimetry
				frame_composer.fc_aviconf2.bit.it_content					= 0;		// IT content
				frame_composer.fc_avivid									= 0;		// Frame Composer AVI Packet VIC
				frame_composer.fc_prconf.bit.output_pr_factor				= 0;		// Input video pixel will be sent a specified number of times
				frame_composer.fc_prconf.bit.incoming_pr_factor				= 0;		// Output video pixel will be sent a specified number of times
			}
			else if (param4 == 1) {
				// set parameter.
				frame_composer.fc_invidconf.bit.in_I_P						= D_IM_HDMI_IN_I_P_INTERLACED;	// Input video mode
				frame_composer.fc_invidconf.bit.r_v_blank_in_osc			= D_IM_HDMI_ACTIVE_HIGH;		// Used for CEA861-D modes with fractional Vblank
				frame_composer.fc_invidconf.bit.DVI_modez					= D_IM_HDMI_DVI_MODEZ_HDMI;		// DVI mode selected
				frame_composer.fc_invidconf.bit.de_in_polarity				= D_IM_HDMI_ACTIVE_HIGH;		// Data enable input polarity
				frame_composer.fc_invidconf.bit.hsync_in_polarity			= D_IM_HDMI_ACTIVE_HIGH;		// Hsync input polarity
				frame_composer.fc_invidconf.bit.vsync_in_polarity			= D_IM_HDMI_ACTIVE_HIGH;		// Vsync input polarity
				frame_composer.fc_invidconf.bit.HDCP_keepout				= D_IM_HDMI_ACTIVE_HIGH;		// Start/stop HDCP keepout window generation
				frame_composer.fc_inhactiv									= 16838;	// fc_inhactiv: Frame Composer Input Video HActive Pixels
				frame_composer.fc_inhblank									= 8191;		// fc_inhblank: Frame Composer Input Video HBlank Pixels
				frame_composer.fc_invactiv									= 8191;		// fc_invactiv: Frame Composer Input Video VActive Pixels
				frame_composer.fc_invblank									= 255;		// fc_invblank: Frame Composer Input Video VBlank Pixels
				frame_composer.fc_hsyncindelay								= 8191;		// fc_hsyncindelay: Frame Composer Input Video HSync Front
				frame_composer.fc_hsyncinwidth								= 1023;		// fc_hsyncinwidth: Frame Composer Input Video HSync Width
				frame_composer.fc_vsyncindelay								= 255;		// fc_vsyncindelay: Frame Composer Input Video VSync Front Porch
				frame_composer.fc_vsyncinwidth								= 63;		// Frame Composer Input Video VSync Width
				frame_composer.fc_aviconf3.bit.CN							= 3;		// IT content type according to CEA the specification
				frame_composer.fc_aviconf3.bit.YQ							= 3;		// YCC Quantization range according to the CEA specification
				frame_composer.fc_aviconf0.bit.rgc_ycc_indication			= 3;		// Y1,Y0 RGB or YCC indicator : 3:YCC420
				frame_composer.fc_aviconf0.bit.bar_information				= 3;		// Bar information data valid
				frame_composer.fc_aviconf0.bit.scan_information				= 3;		// Scan information
				frame_composer.fc_aviconf0.bit.active_format_present		= 1;		// Active format present
				frame_composer.fc_aviconf0.bit.rgc_ycc_indication_2			= 1;		// Y2, Bit 2 of rgc_ycc_indication
				frame_composer.fc_aviconf1.bit.active_aspect_ratio			= 15;		// Active aspect ratio
				frame_composer.fc_aviconf1.bit.picture_aspect_ratio			= 3;		// Picture aspect ratio
				frame_composer.fc_aviconf1.bit.Colorimetry					= 3;		// Colorimetry
				frame_composer.fc_aviconf2.bit.non_uniform_picture_scaling	= 3;		// Non-uniform picture scaling
				frame_composer.fc_aviconf2.bit.quantization_range			= 3;		// Quantization range
				frame_composer.fc_aviconf2.bit.extended_colorimetry			= 7;		// Extended colorimetry
				frame_composer.fc_aviconf2.bit.it_content					= 1;		// IT content
				frame_composer.fc_avivid									= 255;		// Frame Composer AVI Packet VIC
				frame_composer.fc_prconf.bit.output_pr_factor				= 10;		// Input video pixel will be sent a specified number of times
				frame_composer.fc_prconf.bit.incoming_pr_factor				= 10;		// Output video pixel will be sent a specified number of times
			}
			else {
				// end
				Ddim_Print(("!! 4th parameter fraud : %d\n", param4));
				return;
			}
			
			ret = Im_HDMI_Set_Frame_Composer(&frame_composer);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_HDMI_Set_Frame_Composer() : Normal end\n"));
				ct_im_hdmi_pclk_on();
				// check register
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("fc_invidconf    = %p\n",   IO_DISP.HDMI_TX.fc_invidconf.byte) );
				Ddim_Print( ("fc_inhactiv0    = %p\n",   IO_DISP.HDMI_TX.fc_inhactiv0.byte) );
				Ddim_Print( ("fc_inhactiv1    = %p\n",   IO_DISP.HDMI_TX.fc_inhactiv1.byte) );
				Ddim_Print( ("fc_inhblank0    = %p\n",   IO_DISP.HDMI_TX.fc_inhblank0.byte) );
				Ddim_Print( ("fc_inhblank1    = %p\n",   IO_DISP.HDMI_TX.fc_inhblank1.byte) );
				Ddim_Print( ("fc_invactiv0    = %p\n",   IO_DISP.HDMI_TX.fc_invactiv0.byte) );
				Ddim_Print( ("fc_invactiv1    = %p\n",   IO_DISP.HDMI_TX.fc_invactiv1.byte) );
				Ddim_Print( ("fc_invblank     = 0x%x\n", IO_DISP.HDMI_TX.fc_invblank) );
				Ddim_Print( ("fc_hsyncindelay0 = %p\n",  IO_DISP.HDMI_TX.fc_hsyncindelay0.byte) );
				Ddim_Print( ("fc_hsyncindelay1 = %p\n",  IO_DISP.HDMI_TX.fc_hsyncindelay1.byte) );
				Ddim_Print( ("fc_hsyncinwidth0 = %p\n",  IO_DISP.HDMI_TX.fc_hsyncinwidth0.byte) );
				Ddim_Print( ("fc_hsyncinwidth1 = %p\n",  IO_DISP.HDMI_TX.fc_hsyncinwidth0.byte) );
				Ddim_Print( ("fc_vsyncindelay = 0x%x\n", IO_DISP.HDMI_TX.fc_vsyncindelay) );
				Ddim_Print( ("fc_vsyncinwidth = %p\n",   IO_DISP.HDMI_TX.fc_vsyncinwidth.byte) );
				Ddim_Print( ("fc_aviconf3     = %p\n",   IO_DISP.HDMI_TX.fc_aviconf3.byte) );
				Ddim_Print( ("fc_aviconf0     = %p\n",   IO_DISP.HDMI_TX.fc_aviconf0.byte) );
				Ddim_Print( ("fc_aviconf1     = %p\n",   IO_DISP.HDMI_TX.fc_aviconf1.byte) );
				Ddim_Print( ("fc_aviconf2     = %p\n",   IO_DISP.HDMI_TX.fc_aviconf2.byte) );
				Ddim_Print( ("fc_avivid       = %p\n",   IO_DISP.HDMI_TX.fc_avivid.byte) );
				Ddim_Print( ("fc_prconf       = %p\n",   IO_DISP.HDMI_TX.fc_prconf.byte) );
				Ddim_Print( ("----------\n") );
				ct_im_hdmi_pclk_off();
			}
			else {
				Ddim_Print(("Im_HDMI_Set_Frame_Composer() : Error  err_code=0x%x\n", ret));
			}
		}
		else if( strcmp(argv[2], "get") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// table clear.
				memset( &frame_composer, 0, sizeof( T_IM_HDMI_FRAME_COMPOSER ) );
			}
			else {
				// Invalid parameter (frame_composer = NULL)
				ret = Im_HDMI_Get_Frame_Composer(NULL);
				Ddim_Print(("Im_HDMI_Get_Frame_Composer() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			ret = Im_HDMI_Get_Frame_Composer(&frame_composer);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_HDMI_Get_Frame_Composer() : Normal end\n"));
				ct_im_hdmi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("result:\n") );
				Ddim_Print( ("fc_invidconf    : 0x%x\n", frame_composer.fc_invidconf.byte) );
				Ddim_Print( ("fc_inhactiv     : 0x%x\n", frame_composer.fc_inhactiv) );
				Ddim_Print( ("fc_inhblank     : 0x%x\n", frame_composer.fc_inhblank) );
				Ddim_Print( ("fc_invactiv     : 0x%x\n", frame_composer.fc_invactiv) );
				Ddim_Print( ("fc_invblank     : 0x%x\n", frame_composer.fc_invblank) );
				Ddim_Print( ("fc_hsyncindelay : 0x%x\n", frame_composer.fc_hsyncindelay) );
				Ddim_Print( ("fc_hsyncinwidth : 0x%x\n", frame_composer.fc_hsyncinwidth) );
				Ddim_Print( ("fc_vsyncindelay : 0x%x\n", frame_composer.fc_vsyncindelay) );
				Ddim_Print( ("fc_vsyncinwidth : 0x%x\n", frame_composer.fc_vsyncinwidth) );
				Ddim_Print( ("fc_aviconf3     : 0x%x\n", frame_composer.fc_aviconf3.byte) );
				Ddim_Print( ("fc_aviconf0     : 0x%x\n", frame_composer.fc_aviconf0.byte) );
				Ddim_Print( ("fc_aviconf1     : 0x%x\n", frame_composer.fc_aviconf1.byte) );
				Ddim_Print( ("fc_aviconf2     : 0x%x\n", frame_composer.fc_aviconf2.byte) );
				Ddim_Print( ("fc_avivid       : 0x%x\n", frame_composer.fc_avivid) );
				Ddim_Print( ("fc_prconf       : 0x%x\n", frame_composer.fc_prconf.byte) );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("fc_invidconf    = %p\n",   IO_DISP.HDMI_TX.fc_invidconf.byte) );
				Ddim_Print( ("fc_inhactiv0    = %p\n",   IO_DISP.HDMI_TX.fc_inhactiv0.byte) );
				Ddim_Print( ("fc_inhactiv1    = %p\n",   IO_DISP.HDMI_TX.fc_inhactiv1.byte) );
				Ddim_Print( ("fc_inhblank0    = %p\n",   IO_DISP.HDMI_TX.fc_inhblank0.byte) );
				Ddim_Print( ("fc_inhblank1    = %p\n",   IO_DISP.HDMI_TX.fc_inhblank1.byte) );
				Ddim_Print( ("fc_invactiv0    = %p\n",   IO_DISP.HDMI_TX.fc_invactiv0.byte) );
				Ddim_Print( ("fc_invactiv1    = %p\n",   IO_DISP.HDMI_TX.fc_invactiv1.byte) );
				Ddim_Print( ("fc_invblank     = 0x%x\n", IO_DISP.HDMI_TX.fc_invblank) );
				Ddim_Print( ("fc_hsyncindelay0 = %p\n",  IO_DISP.HDMI_TX.fc_hsyncindelay0.byte) );
				Ddim_Print( ("fc_hsyncindelay1 = %p\n",  IO_DISP.HDMI_TX.fc_hsyncindelay1.byte) );
				Ddim_Print( ("fc_hsyncinwidth0 = %p\n",  IO_DISP.HDMI_TX.fc_hsyncinwidth0.byte) );
				Ddim_Print( ("fc_hsyncinwidth0 = %p\n",  IO_DISP.HDMI_TX.fc_hsyncinwidth0.byte) );
				Ddim_Print( ("fc_vsyncindelay = 0x%x\n", IO_DISP.HDMI_TX.fc_vsyncindelay) );
				Ddim_Print( ("fc_vsyncinwidth = %p\n",   IO_DISP.HDMI_TX.fc_vsyncinwidth.byte) );
				Ddim_Print( ("fc_aviconf3     = %p\n",   IO_DISP.HDMI_TX.fc_aviconf3.byte) );
				Ddim_Print( ("fc_aviconf0     = %p\n",   IO_DISP.HDMI_TX.fc_aviconf0.byte) );
				Ddim_Print( ("fc_aviconf1     = %p\n",   IO_DISP.HDMI_TX.fc_aviconf1.byte) );
				Ddim_Print( ("fc_aviconf2     = %p\n",   IO_DISP.HDMI_TX.fc_aviconf2.byte) );
				Ddim_Print( ("fc_avivid       = %p\n",   IO_DISP.HDMI_TX.fc_avivid.byte) );
				Ddim_Print( ("fc_prconf       = %p\n",   IO_DISP.HDMI_TX.fc_prconf.byte) );
				Ddim_Print( ("----------\n") );
				ct_im_hdmi_pclk_off();
			}
			else {
				Ddim_Print(("Im_HDMI_Get_Frame_Composer() : Error  err_code=0x%x\n", ret));
			}
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud\n"));
			return;
		}
	}
	else if ( strcmp(argv[1], "mc") == 0 ) {
		// 2nd param check
		if( strcmp(argv[2], "set") == 0 ) {
			// 3rd param
			mc_flowctrl = atoi((const char *)argv[3]);
			
			ret = Im_HDMI_Set_Main_Controller(mc_flowctrl);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_HDMI_Set_Main_Controller() : Normal end\n"));
				ct_im_hdmi_pclk_on();
				// check register
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("mc_flowctrl = %p\n", IO_DISP.HDMI_TX.mc_flowctrl.byte) );
				Ddim_Print( ("----------\n") );
				ct_im_hdmi_pclk_off();
			}
			else {
				Ddim_Print(("Im_HDMI_Set_Main_Controller() : Error  err_code=0x%x\n", ret));
			}
		}
		else if( strcmp(argv[2], "get") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// output parameter clear.
				mc_flowctrl = 0;
			}
			else {
				// Invalid parameter (mc_flowctrl = NULL)
				ret = Im_HDMI_Get_Main_Controller(NULL);
				Ddim_Print(("Im_HDMI_Get_Main_Controller() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			ret = Im_HDMI_Get_Main_Controller(&mc_flowctrl);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_HDMI_Get_Main_Controller() : Normal end\n"));
				ct_im_hdmi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("result:\n") );
				Ddim_Print( ("mc_flowctrl : 0x%x\n", mc_flowctrl) );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("mc_flowctrl = %p\n", IO_DISP.HDMI_TX.mc_flowctrl.byte) );
				Ddim_Print( ("----------\n") );
				ct_im_hdmi_pclk_off();
			}
			else {
				Ddim_Print(("Im_HDMI_Get_Main_Controller() : Error  err_code=0x%x\n", ret));
			}
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud\n"));
			return;
		}
	}
	else if ( strcmp(argv[1], "audio") == 0 ) {
		// 2nd param check
		if( strcmp(argv[2], "set") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 != 0) {
				// Invalid parameter (audio = NULL)
				ret = Im_HDMI_Set_Audio(NULL);
				Ddim_Print(("Im_HDMI_Set_Audio() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			// table clear.
			memset( &audio, 0, sizeof( T_IM_HDMI_AUDIO ) );
			
			// 4th param check
			param4 = atoi((const char *)argv[4]);
			if (param4 == 0) {
				// set parameter.
				audio.aud_conf0.bit.I2S_in_en				= 0x00;	// I2Sdata[0]~I2Sdata[3] disable
				audio.aud_conf0.bit.i2s_select				= E_IM_HDMI_I2S_SELECT_S_PDIF;
				audio.aud_conf0.bit.sw_audio_fifo_rst		= 0;	//  no action taken
				audio.aud_conf1.bit.I2S_width				= 16;	// I2S input data width
				audio.aud_conf1.bit.I2S_mode				= E_IM_HDMI_I2S_MODE_STANDARD;
				audio.aud_conf2.bit.HBR						= D_IM_HDMI_ENABLE_OFF;	// I2S HBR Mode Disable
				audio.aud_conf2.bit.NLPCM					= D_IM_HDMI_ENABLE_OFF;	// I2S NLPCM Mode Disable
				audio.aud_spdif1.bit.spdif_width			= 16;	// SPDIF input data width=16
				audio.aud_spdif1.bit.spdif_hbr_mode			= 0;	// Transmitted in the HDMI HBR packets
				audio.aud_spdif1.bit.setnlpcm				= 0;	// Non-Linear PCM mode
				audio.aud_inputclkfs						= E_IM_HDMI_AUD_IFSFACTOR_128XFS;
				audio.fc_audiconf1.bit.SF					= 0;	// Sampling frequency=0.
				audio.fc_audiconf1.bit.SS					= 0;	// Sampling size=0.
				audio.fc_audsconf.bit.aud_packet_layout		= 0;	// layout 0.
				audio.fc_audsconf.bit.aud_packet_sampflt	= 0;	// audio packet sample flat value=0.
			}
			else if (param4 == 1) {
				// set parameter.
				audio.aud_conf0.bit.I2S_in_en				= 0x0F;	// I2Sdata[0]~I2Sdata[3] enable
				audio.aud_conf0.bit.i2s_select				= E_IM_HDMI_I2S_SELECT_I2S;
				audio.aud_conf0.bit.sw_audio_fifo_rst		= 1;	// Resets all audio FIFOs
				audio.aud_conf1.bit.I2S_width				= 24;	// I2S input data width
				audio.aud_conf1.bit.I2S_mode				= E_IM_HDMI_I2S_MODE_LEFT_JUSTIFIED;
				audio.aud_conf2.bit.HBR						= D_IM_HDMI_ENABLE_ON;	// I2S HBR Mode Enable
				audio.aud_conf2.bit.NLPCM					= D_IM_HDMI_ENABLE_ON;	// I2S NLPCM Mode Enable
				audio.aud_spdif1.bit.spdif_width			= 24;	// SPDIF input data width=24
				audio.aud_spdif1.bit.spdif_hbr_mode			= 1;	// Transmitted in the HDMI AUDS packets
				audio.aud_spdif1.bit.setnlpcm				= 1;	// Linear PCM mode
				audio.aud_inputclkfs						= E_IM_HDMI_AUD_IFSFACTOR_64XFS;
				audio.fc_audiconf1.bit.SF					= 7;	// Sampling frequency=7.
				audio.fc_audiconf1.bit.SS					= 3;	// Sampling size=3.
				audio.fc_audsconf.bit.aud_packet_layout		= 1;	// layout 1.
				audio.fc_audsconf.bit.aud_packet_sampflt	= 15;	// audio packet sample flat value=15.
			}
			else {
				// end
				Ddim_Print(("!! 4th parameter fraud : %d\n", param4));
				return;
			}
			
			ret = Im_HDMI_Set_Audio(&audio);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_HDMI_Set_Audio() : Normal end\n"));
				ct_im_hdmi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("aud_conf0      = %p\n", IO_DISP.HDMI_TX.aud_conf0.byte) );
				Ddim_Print( ("aud_conf1      = %p\n", IO_DISP.HDMI_TX.aud_conf1.byte) );
				Ddim_Print( ("aud_conf2      = %p\n", IO_DISP.HDMI_TX.aud_conf2.byte) );
				Ddim_Print( ("aud_spdif1     = %p\n", IO_DISP.HDMI_TX.aud_spdif1.byte) );
				Ddim_Print( ("aud_inputclkfs = %p\n", IO_DISP.HDMI_TX.aud_inputclkfs.byte) );
				Ddim_Print( ("fc_audiconf1   = %p\n", IO_DISP.HDMI_TX.fc_audiconf1.byte) );
				Ddim_Print( ("fc_audsconf    = %p\n", IO_DISP.HDMI_TX.fc_audsconf.byte) );
				Ddim_Print( ("----------\n") );
				ct_im_hdmi_pclk_off();
			}
			else {
				Ddim_Print(("Im_HDMI_Set_Audio() : Error  err_code=0x%x\n", ret));
			}
		}
		else if( strcmp(argv[2], "get") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// table clear.
				memset( &audio, 0, sizeof( T_IM_HDMI_AUDIO ) );
			}
			else {
				// Invalid parameter (audio = NULL)
				ret = Im_HDMI_Get_Audio(NULL);
				Ddim_Print(("Im_HDMI_Get_Audio() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			ret = Im_HDMI_Get_Audio(&audio);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_HDMI_Get_Audio() : Normal end\n"));
				ct_im_hdmi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("result:\n") );
				Ddim_Print( ("aud_conf0      : 0x%x\n", audio.aud_conf0.byte) );
				Ddim_Print( ("aud_conf1      : 0x%x\n", audio.aud_conf1.byte) );
				Ddim_Print( ("aud_conf2      : 0x%x\n", audio.aud_conf2.byte) );
				Ddim_Print( ("aud_spdif1     : 0x%x\n", audio.aud_spdif1.byte) );
				Ddim_Print( ("aud_inputclkfs : %d\n",   audio.aud_inputclkfs) );
				Ddim_Print( ("fc_audiconf1   : 0x%x\n", audio.fc_audiconf1.byte) );
				Ddim_Print( ("fc_audsconf    : 0x%x\n", audio.fc_audsconf.byte) );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("aud_conf0      = %p\n", IO_DISP.HDMI_TX.aud_conf0.byte) );
				Ddim_Print( ("aud_conf1      = %p\n", IO_DISP.HDMI_TX.aud_conf1.byte) );
				Ddim_Print( ("aud_conf2      = %p\n", IO_DISP.HDMI_TX.aud_conf2.byte) );
				Ddim_Print( ("aud_spdif1     = %p\n", IO_DISP.HDMI_TX.aud_spdif1.byte) );
				Ddim_Print( ("aud_inputclkfs = %p\n", IO_DISP.HDMI_TX.aud_inputclkfs.byte) );
				Ddim_Print( ("fc_audiconf1   = %p\n", IO_DISP.HDMI_TX.fc_audiconf1.byte) );
				Ddim_Print( ("fc_audsconf    = %p\n", IO_DISP.HDMI_TX.fc_audsconf.byte) );
				Ddim_Print( ("----------\n") );
				ct_im_hdmi_pclk_off();
			}
			else {
				Ddim_Print(("Im_HDMI_Get_Audio() : Error  err_code=0x%x\n", ret));
			}
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud\n"));
			return;
		}
	}
	else if ( strcmp(argv[1], "cb") == 0 ) {
		// 2nd param check
		if( strcmp(argv[2], "set") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				vp_callback = NULL;
			}
			else if (param3 == 1) {
				vp_callback = (VP_HDMI_CALLBACK)ct_im_hdmi_cb0;
			}
			else {
				// end
				Ddim_Print(("!! 3rd parameter fraud : %d\n", param3));
				return;
			}
			
			Im_HDMI_Set_Int_Callback(vp_callback);
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud\n"));
			return;
		}
	}
	else if ( strcmp(argv[1], "mute") == 0 ) {
		// 2nd param check
		if( strcmp(argv[2], "set") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if ((param3 >= E_IM_HDMI_INT_REG_FC_STAT0) && (param3 <= E_IM_HDMI_INT_REG_I2CMPHY_STAT0)) {
				interrupt_register = (E_IM_HDMI_INT_REG)param3;
			}
			else {
				// end
				Ddim_Print(("!! 3rd parameter fraud : %d\n", param3));
				return;
			}
			
			// 4th param
			param4 = atoi((const char *)argv[4]);
			if ((param4 >= 0) && (param4 <= 0xFF)) {
				mute = (UCHAR)param4;
			}
			else {
				// end
				Ddim_Print(("!! 4th parameter fraud : %d\n", param4));
				return;
			}
			
			Im_HDMI_Set_Int_Mute(interrupt_register, mute);
			
			// check register
			Ddim_Print(("Im_HDMI_Set_Int_Mute() : Normal end\n"));
			ct_im_hdmi_pclk_on();
			Ddim_Print( ("----------\n") );
			Ddim_Print( ("parameter:\n") );
			Ddim_Print( ("interrupt_register : %d\n", interrupt_register) );
			Ddim_Print( ("mute               : 0x%02x\n", mute) );
			Ddim_Print( ("register:\n") );
			Ddim_Print( ("ih_mute_fc_stat0   = %p\n", IO_DISP.HDMI_TX.ih_mute_fc_stat0.byte) );
			Ddim_Print( ("ih_mute_fc_stat1   = %p\n", IO_DISP.HDMI_TX.ih_mute_fc_stat1.byte) );
			Ddim_Print( ("ih_mute_fc_stat2   = %p\n", IO_DISP.HDMI_TX.ih_mute_fc_stat2.byte) );
			Ddim_Print( ("ih_mute_as_stat0   = %p\n", IO_DISP.HDMI_TX.ih_mute_as_stat0.byte) );
			Ddim_Print( ("ih_mute_phy_stat0  = %p\n", IO_DISP.HDMI_TX.ih_mute_phy_stat0.byte) );
			Ddim_Print( ("ih_mute_i2cm_stat0 = %p\n", IO_DISP.HDMI_TX.ih_mute_i2cm_stat0.byte) );
			Ddim_Print( ("ih_mute_cec_stat0  = %p\n", IO_DISP.HDMI_TX.ih_mute_cec_stat0.byte) );
			Ddim_Print( ("ih_mute_vp_stat0   = %p\n", IO_DISP.HDMI_TX.ih_mute_vp_stat0.byte) );
			Ddim_Print( ("ih_i2cmphy_stat0   = %p\n", IO_DISP.HDMI_TX.ih_i2cmphy_stat0.byte) );
			Ddim_Print( ("----------\n") );
			ct_im_hdmi_pclk_off();
		}
		else if( strcmp(argv[2], "get") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				mute = 0;
			}
			else {
				// Invalid parameter (mute = NULL)
				interrupt_register = E_IM_HDMI_INT_REG_FC_STAT0;
				ret = Im_HDMI_Get_Int_Mute(interrupt_register, NULL);
				Ddim_Print(("Im_HDMI_Get_Int_Mute() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			// 4th param check
			param4 = atoi((const char *)argv[4]);
			if ((param4 >= E_IM_HDMI_INT_REG_FC_STAT0) && (param4 <= E_IM_HDMI_INT_REG_I2CMPHY_STAT0)) {
				interrupt_register = (E_IM_HDMI_INT_REG)param4;
			}
			else {
				// end
				Ddim_Print(("!! 4th parameter fraud : %d\n", param4));
				return;
			}
			
			ret = Im_HDMI_Get_Int_Mute(interrupt_register, &mute);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_HDMI_Get_Int_Mute() : Normal end\n"));
				ct_im_hdmi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("result:\n") );
				Ddim_Print( ("interrupt_register : %d\n", interrupt_register) );
				Ddim_Print( ("mute               : 0x%02x\n", mute) );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("ih_mute_fc_stat0   = %p\n", IO_DISP.HDMI_TX.ih_mute_fc_stat0.byte) );
				Ddim_Print( ("ih_mute_fc_stat1   = %p\n", IO_DISP.HDMI_TX.ih_mute_fc_stat1.byte) );
				Ddim_Print( ("ih_mute_fc_stat2   = %p\n", IO_DISP.HDMI_TX.ih_mute_fc_stat2.byte) );
				Ddim_Print( ("ih_mute_as_stat0   = %p\n", IO_DISP.HDMI_TX.ih_mute_as_stat0.byte) );
				Ddim_Print( ("ih_mute_phy_stat0  = %p\n", IO_DISP.HDMI_TX.ih_mute_phy_stat0.byte) );
				Ddim_Print( ("ih_mute_i2cm_stat0 = %p\n", IO_DISP.HDMI_TX.ih_mute_i2cm_stat0.byte) );
				Ddim_Print( ("ih_mute_cec_stat0  = %p\n", IO_DISP.HDMI_TX.ih_mute_cec_stat0.byte) );
				Ddim_Print( ("ih_mute_vp_stat0   = %p\n", IO_DISP.HDMI_TX.ih_mute_vp_stat0.byte) );
				Ddim_Print( ("ih_i2cmphy_stat0   = %p\n", IO_DISP.HDMI_TX.ih_i2cmphy_stat0.byte) );
				Ddim_Print( ("----------\n") );
				ct_im_hdmi_pclk_off();
			}
			else {
				Ddim_Print(("Im_HDMI_Get_Int_Mute() : Error  err_code=0x%x\n", ret));
			}
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud\n"));
			return;
		}
	}
	else if ( strcmp(argv[1], "int") == 0 ) {
		// 2nd param check
		param2 = atoi((const char *)argv[2]);
		if (param2 == 0) {
			// set callback function.
			Im_HDMI_Set_Int_Callback(ct_im_hdmi_cb0);
			// ih_fc_stat0 interrupt enalbe (mute calcel).
			Im_HDMI_Set_Int_Mute(E_IM_HDMI_INT_REG_FC_STAT0, 0x00);
#ifdef CO_DEBUG_ON_PC
			// set Interrupt register : ih_fc_stat0.NULL_PKT on.
			ct_im_hdmi_pclk_on();
			IO_DISP.HDMI_TX.ih_fc_stat0.bit.NULL_PKT = 1;
			ct_im_hdmi_pclk_off();
#endif	// CO_DEBUG_ON_PC
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud : %d\n", param2));
			return;
		}
		
		Im_HDMI_Int_Handler();
		Ddim_Print(("Im_HDMI_Int_Handler() : Normal end\n"));
	}
	else{
		Ddim_Print( ("please check 1st parameter!\n") );
	}

	return ;
}

/**
 * @brief       HDMI PCSim Test..
 * @param       VOID
 * @return      VOID
 */
VOID Ct_Im_Hdmi_PCSim_Test(VOID)
{
#ifdef CO_DEBUG_ON_PC
	int cmd_cnt = 0;
	
	// HDMI Test command data
//	char hdmi_test_argv1[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "init", "0", "0" };
	char hdmi_test_argv2[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "init", "1" };
//	char hdmi_test_argv3[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "init", "2" };
//	char hdmi_test_argv4[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "sinks", "0", "0" };
	char hdmi_test_argv5[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "sinks", "1" };
	char hdmi_test_argv6[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "sinks", "0", "1" };
	char hdmi_test_argv7[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "ctrl", "set", "0", "0" };
	char hdmi_test_argv8[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "ctrl", "set", "1" };
	char hdmi_test_argv9[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "ctrl", "set", "2", "0" };
	char hdmi_test_argv10[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "ctrl", "set", "0", "2" };
	char hdmi_test_argv11[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "ctrl", "get", "0" };
	char hdmi_test_argv12[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "ctrl", "get", "1" };
	char hdmi_test_argv13[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "clk", "set", "0" };
	char hdmi_test_argv14[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "clk", "get", "0" };
	char hdmi_test_argv15[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "clk", "get", "1" };
	char hdmi_test_argv16[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "term", "set", "0" };
	char hdmi_test_argv17[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "term", "get", "0" };
	char hdmi_test_argv18[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "term", "get", "1" };
	char hdmi_test_argv19[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "tc", "set", "0" };
	char hdmi_test_argv20[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "tc", "get", "0" };
	char hdmi_test_argv21[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "tc", "get", "1" };
	char hdmi_test_argv22[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "vol", "set", "0" };
	char hdmi_test_argv23[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "vol", "get", "0" };
	char hdmi_test_argv24[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "vol", "get", "1" };
	char hdmi_test_argv25[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "csc", "set", "0", "0" };
	char hdmi_test_argv26[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "csc", "set", "1" };
	char hdmi_test_argv27[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "vs", "set", "0" };
	char hdmi_test_argv28[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "vs", "get", "0" };
	char hdmi_test_argv29[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "vs", "get", "1" };
	char hdmi_test_argv30[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "vp", "set", "0", "0" };
	char hdmi_test_argv31[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "vp", "set", "1" };
	char hdmi_test_argv32[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "vp", "get", "0" };
	char hdmi_test_argv33[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "vp", "get", "1" };
	char hdmi_test_argv34[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "fc", "set", "0", "0" };
	char hdmi_test_argv35[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "fc", "set", "1" };
	char hdmi_test_argv36[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "fc", "get", "0" };
	char hdmi_test_argv37[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "fc", "get", "1" };
	char hdmi_test_argv38[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "mc", "set", "0", "0" };
	char hdmi_test_argv39[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "mc", "set", "4" };
	char hdmi_test_argv40[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "mc", "get", "0" };
	char hdmi_test_argv41[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "mc", "get", "1" };
	char hdmi_test_argv42[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "audio", "set", "0", "0" };
	char hdmi_test_argv43[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "audio", "set", "1" };
	char hdmi_test_argv44[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "audio", "get", "0" };
	char hdmi_test_argv45[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "audio", "get", "1" };
	char hdmi_test_argv46[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "cb", "set", "0", "0" };
	char hdmi_test_argv47[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "cb", "set", "47", "1" };
	char hdmi_test_argv48[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "mute", "set", "0" };
	char hdmi_test_argv49[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "mute", "get", "0" };
	char hdmi_test_argv50[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "mute", "get", "1" };
	char hdmi_test_argv51[][D_HDMI_TEST_CMD_LEN] = { "imhdmi", "int", "0" };
	
	// 
	memset( (VOID*)&(IO_DISP.HDMI_TX), 0, sizeof(IO_DISP.HDMI_TX) );
	
	Ddim_Print(("\n======== HDMI ========\n"));
	Ddim_Print((">> Test 1-1-1\n"));
//	cmd_cnt = (sizeof(hdmi_test_argv1) / D_HDMI_TEST_CMD_LEN);
//	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv1);
//	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-2\n"));
	cmd_cnt = (sizeof(hdmi_test_argv2) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv2);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-3\n"));
//	cmd_cnt = (sizeof(hdmi_test_argv3) / D_HDMI_TEST_CMD_LEN);
//	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv3);
//	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-4\n"));
//	cmd_cnt = (sizeof(hdmi_test_argv4) / D_HDMI_TEST_CMD_LEN);
//	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv4);
//	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-5\n"));
	cmd_cnt = (sizeof(hdmi_test_argv5) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv5);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-6\n"));
	cmd_cnt = (sizeof(hdmi_test_argv6) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv6);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-7\n"));
	cmd_cnt = (sizeof(hdmi_test_argv7) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv7);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-8\n"));
	cmd_cnt = (sizeof(hdmi_test_argv8) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv8);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-9\n"));
	cmd_cnt = (sizeof(hdmi_test_argv9) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv9);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-10\n"));
	cmd_cnt = (sizeof(hdmi_test_argv10) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv10);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-11\n"));
	cmd_cnt = (sizeof(hdmi_test_argv11) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv11);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-12\n"));
	cmd_cnt = (sizeof(hdmi_test_argv12) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv12);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-13\n"));
	cmd_cnt = (sizeof(hdmi_test_argv13) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv13);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-14\n"));
	cmd_cnt = (sizeof(hdmi_test_argv14) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv14);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-15\n"));
	cmd_cnt = (sizeof(hdmi_test_argv15) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv15);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-16\n"));
	cmd_cnt = (sizeof(hdmi_test_argv16) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv16);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-17\n"));
	cmd_cnt = (sizeof(hdmi_test_argv17) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv17);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-18\n"));
	cmd_cnt = (sizeof(hdmi_test_argv18) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv18);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-19\n"));
	cmd_cnt = (sizeof(hdmi_test_argv19) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv19);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-20\n"));
	cmd_cnt = (sizeof(hdmi_test_argv20) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv20);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-21\n"));
	cmd_cnt = (sizeof(hdmi_test_argv21) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv21);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-22\n"));
	cmd_cnt = (sizeof(hdmi_test_argv22) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv22);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-23\n"));
	cmd_cnt = (sizeof(hdmi_test_argv23) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv23);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-24\n"));
	cmd_cnt = (sizeof(hdmi_test_argv24) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv24);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-25\n"));
	cmd_cnt = (sizeof(hdmi_test_argv25) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv25);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-26\n"));
	cmd_cnt = (sizeof(hdmi_test_argv26) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv26);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-27\n"));
	cmd_cnt = (sizeof(hdmi_test_argv27) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv27);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-28\n"));
	cmd_cnt = (sizeof(hdmi_test_argv28) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv28);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-29\n"));
	cmd_cnt = (sizeof(hdmi_test_argv29) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv29);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-30\n"));
	cmd_cnt = (sizeof(hdmi_test_argv30) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv30);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-31\n"));
	cmd_cnt = (sizeof(hdmi_test_argv31) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv31);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-32\n"));
	cmd_cnt = (sizeof(hdmi_test_argv32) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv32);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-33\n"));
	cmd_cnt = (sizeof(hdmi_test_argv33) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv33);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-34\n"));
	cmd_cnt = (sizeof(hdmi_test_argv34) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv34);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-35\n"));
	cmd_cnt = (sizeof(hdmi_test_argv35) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv35);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-36\n"));
	cmd_cnt = (sizeof(hdmi_test_argv36) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv36);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-37\n"));
	cmd_cnt = (sizeof(hdmi_test_argv37) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv37);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-38\n"));
	cmd_cnt = (sizeof(hdmi_test_argv38) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv38);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-39\n"));
	cmd_cnt = (sizeof(hdmi_test_argv39) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv39);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-40\n"));
	cmd_cnt = (sizeof(hdmi_test_argv40) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv40);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-41\n"));
	cmd_cnt = (sizeof(hdmi_test_argv41) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv41);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-42\n"));
	cmd_cnt = (sizeof(hdmi_test_argv42) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv42);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-43\n"));
	cmd_cnt = (sizeof(hdmi_test_argv43) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv43);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-44\n"));
	cmd_cnt = (sizeof(hdmi_test_argv44) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv44);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-45\n"));
	cmd_cnt = (sizeof(hdmi_test_argv45) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv45);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-46\n"));
	cmd_cnt = (sizeof(hdmi_test_argv46) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv46);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-47\n"));
	cmd_cnt = (sizeof(hdmi_test_argv47) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv47);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-48\n"));
	cmd_cnt = (sizeof(hdmi_test_argv48) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv48);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-49\n"));
	cmd_cnt = (sizeof(hdmi_test_argv49) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv49);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-50\n"));
	cmd_cnt = (sizeof(hdmi_test_argv50) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv50);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);

	Ddim_Print(("\n>> Test 1-1-51\n"));
	cmd_cnt = (sizeof(hdmi_test_argv51) / D_HDMI_TEST_CMD_LEN);
	ct_im_hdmi_set_cmd(cmd_cnt, (char*)hdmi_test_argv51);
	Ct_Im_Hdmi_Main(cmd_cnt, gCmd_Argv);
#endif	// CO_DEBUG_ON_PC
}
