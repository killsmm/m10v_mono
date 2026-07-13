/**
 * @file		ct_im_mipi.c
 * @brief		This is ct code for im_mipi.c.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "jdsdisp3a.h"
#include "ct_im_mipi.h"
#include "im_mipi.h"
#include "im_disp.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#ifdef CO_DEBUG_ON_PC
#define D_MIPI_TEST_CMD_LEN		20
#endif	// CO_DEBUG_ON_PC

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// nothing special.

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static const U_IM_MIPI_GEN_HDR gCt_Im_Mipi_Header[D_IM_MIPI_PACKET_NUM_MAX] = {
	{0x00000000}, {0x00FFFFFF}, {0x00010141}, {0x00020282},
	{0x000304C3}, {0x0005063F}, {0x00070871}, {0x00090A98},
	{0x00BB0C00}, {0x000DEECF}, {0x00F0F055}, {0x00111183},
	{0x000101AC}, {0x000101E2}, {0x00010144}, {0x00010101}
};
static const ULONG gCt_Im_Mipi_Payload[D_IM_MIPI_PACKET_NUM_MAX] = {
	0x00000001, 0x00000020, 0x00000300, 0x00004000,
	0x000F000E, 0x00D000C0, 0x0B000A00, 0x90008000,
	0x00664422, 0x13579B00, 0x0888AAA0, 0xFEF00212,
	0x11223344, 0x8901AB23, 0x76543210, 0xFEDCBA98
};

#ifdef CO_DEBUG_ON_PC
static char* gCmd_Argv[25];
#endif	// CO_DEBUG_ON_PC

/*----------------------------------------------------------------------*/
/* Local Method															*/
/*----------------------------------------------------------------------*/
static VOID ct_im_mipi_pclk_on(VOID)
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
static VOID ct_im_mipi_pclk_off(VOID)
{
#ifdef CO_ACT_PCLOCK
	Im_DISP_APB_Clock_Off();
#endif	// CO_ACT_PCLOCK
}

static VOID ct_im_mipi_cb0( VOID )
{
	Ddim_Print((">> call ct_im_mipi_cb0()\n"));
	return;
}

#ifdef CO_DEBUG_ON_PC
static void ct_im_mipi_set_cmd( int cmd_argc, char* cmd_argv ) 
{ 
	INT32 j;
	INT32 cmd_pos = 0;

	for( j = 0; j < cmd_argc; j++ ) { 
		gCmd_Argv[ j ] = ( cmd_argv + ( cmd_pos * D_MIPI_TEST_CMD_LEN ) ); 
		cmd_pos++;
	}
}
#endif	// CO_DEBUG_ON_PC

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
 * @brief       Command processing of MIPI.
 * @param[in]   INT32 argc
 * @param[in]   CHAR** argv
 * @return      VOID
 */
VOID Ct_Im_Mipi_Main( INT32 argc, char** argv )
{
	INT32						ret;
	INT32						param2;
	INT32						param3;
	INT32						param4;
	E_IM_MIPI_ACTIVE_DATA_LANE	lane;
	E_IM_MIPI_PHY_SHUTDOWNZ		power;
	E_IM_MIPI_SHUTDOWNZ			reset;
	T_IM_MIPI_DPI_LANE_INFO		lane_info;
	T_IM_MIPI_DPI_RESOLUTION	dpi;
	T_IM_MIPI_TRANS_CONFIG		config;
	UCHAR						header_num;
	UCHAR						payload_num;
	UCHAR						count;
	E_IM_MIPI_INT_TYPE			interrupt_type;
	VP_CALLBACK					vp_callback;
	U_IM_MIPI_INT_MSK1			int_msk1;
	
	// check number of parameter
	if( argc > 20 ){
		Ddim_Print(("The number of input parameters is exceeded!\n"));
		return;
	}

	// 1st param check
	if( strcmp(argv[1], "init") == 0 ) {
		// Init
		// 2nd param check
		param2 = atoi((const char *)argv[2]);
		if ((param2 >= E_IM_MIPI_ACTIVE_DATA_LANE_1) && (param2 <= E_IM_MIPI_ACTIVE_DATA_LANE_4)) {
			// set paramter
			lane = (E_IM_MIPI_ACTIVE_DATA_LANE)param2;
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud : %d\n", param2));
			return;
		}
		
		Im_MIPI_Init(lane);
		Ddim_Print(("Im_MIPI_Init() : Normal end\n"));
		ct_im_mipi_pclk_on();
		// check register
		Ddim_Print( ("----------\n") );
		Ddim_Print( ("register:\n") );
		Ddim_Print( ("PHY_IF_CFG      = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_IF_CFG.word) );
		Ddim_Print( ("PHY_RSTZ        = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_RSTZ.word) );
		Ddim_Print( ("PWR_UP          = 0x%lx\n", IO_DISP.MIPI_DSI.PWR_UP.word) );
		Ddim_Print( ("LPCLK_CTRL      = 0x%lx\n", IO_DISP.MIPI_DSI.LPCLK_CTRL.word) );
		Ddim_Print( ("VID_SHADOW_CTRL = 0x%lx\n", IO_DISP.MIPI_DSI.VID_SHADOW_CTRL.word) );
		Ddim_Print( ("----------\n") );
		ct_im_mipi_pclk_off();
	}
	else if ( strcmp(argv[1], "pc") == 0 ) {
		// 2nd param check
		param2 = atoi((const char *)argv[2]);
		if (param2 == 0) {
			power	= E_IM_MIPI_PHY_SHUTDOWNZ_POWER_DOWN;
			reset	= E_IM_MIPI_SHUTDOWNZ_RESET;
		}
		else if (param2 == 1) {
			power	= E_IM_MIPI_PHY_SHUTDOWNZ_POWER_UP;
			reset	= E_IM_MIPI_SHUTDOWNZ_RESET;
		}
		else if (param2 == 2) {
			power	= E_IM_MIPI_PHY_SHUTDOWNZ_POWER_UP;
			reset	= E_IM_MIPI_SHUTDOWNZ_POWER_UP;
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud\n"));
			return;
		}
		
		Im_MIPI_Power_Ctrl(power, reset);
		Ddim_Print(("Im_MIPI_Power_Ctrl() : Normal end\n"));
		ct_im_mipi_pclk_on();
		// check register
		Ddim_Print( ("----------\n") );
		Ddim_Print( ("register:\n") );
		Ddim_Print( ("PHY_RSTZ = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_RSTZ.word) );
		Ddim_Print( ("PWR_UP   = 0x%lx\n", IO_DISP.MIPI_DSI.PWR_UP.word) );
		Ddim_Print( ("----------\n") );
		ct_im_mipi_pclk_off();
	}
	else if ( strcmp(argv[1], "ulps") == 0 ) {
		// 2nd param check
		if( strcmp(argv[2], "enter") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// 4th param check
				param4 = atoi((const char *)argv[4]);
				if (param4 == 0) {
					// register setting
					ct_im_mipi_pclk_on();
					IO_DISP.MIPI_DSI.PHY_ULPS_CTRL.bit.phy_txrequlpsclk		= 0;
					IO_DISP.MIPI_DSI.PHY_ULPS_CTRL.bit.phy_txexitulpsclk	= 0;
					IO_DISP.MIPI_DSI.PHY_ULPS_CTRL.bit.phy_txrequlpslan		= 0;
					IO_DISP.MIPI_DSI.PHY_ULPS_CTRL.bit.phy_txexitulpslan	= 0;
					IO_DISP.MIPI_DSI.PHY_TX_TRIGGERS.bit.phy_tx_triggers	= 0;
					IO_DISP.MIPI_DSI.PHY_IF_CFG.bit.n_lanes					= E_IM_MIPI_ACTIVE_DATA_LANE_1;
					IO_DISP.MIPI_DSI.PHY_STATUS.word						= 0x003C;
					ct_im_mipi_pclk_off();
				}
				else {
					// end
					Ddim_Print(("!! 4th parameter fraud : %d\n", param4));
					return;
				}
			}
			else if (param3 == 1) {
				// Invalid setting (Status NG)
				ct_im_mipi_pclk_on();
				IO_DISP.MIPI_DSI.PHY_ULPS_CTRL.bit.phy_txrequlpsclk		= 1;
				IO_DISP.MIPI_DSI.PHY_ULPS_CTRL.bit.phy_txexitulpsclk	= 1;
				IO_DISP.MIPI_DSI.PHY_ULPS_CTRL.bit.phy_txrequlpslan		= 1;
				IO_DISP.MIPI_DSI.PHY_ULPS_CTRL.bit.phy_txexitulpslan	= 1;
				IO_DISP.MIPI_DSI.PHY_TX_TRIGGERS.bit.phy_tx_triggers	= 1;
				IO_DISP.MIPI_DSI.PHY_IF_CFG.bit.n_lanes					= E_IM_MIPI_ACTIVE_DATA_LANE_1;
				IO_DISP.MIPI_DSI.PHY_STATUS.word						= 0x003C;
				ct_im_mipi_pclk_off();
			}
			else if (param3 == 2) {
				// Invalid setting (Data lane setting unmatch)
				ct_im_mipi_pclk_on();
				IO_DISP.MIPI_DSI.PHY_ULPS_CTRL.bit.phy_txrequlpsclk		= 0;
				IO_DISP.MIPI_DSI.PHY_ULPS_CTRL.bit.phy_txexitulpsclk	= 0;
				IO_DISP.MIPI_DSI.PHY_ULPS_CTRL.bit.phy_txrequlpslan		= 0;
				IO_DISP.MIPI_DSI.PHY_ULPS_CTRL.bit.phy_txexitulpslan	= 0;
				IO_DISP.MIPI_DSI.PHY_TX_TRIGGERS.bit.phy_tx_triggers	= 0;
				IO_DISP.MIPI_DSI.PHY_IF_CFG.bit.n_lanes					= E_IM_MIPI_ACTIVE_DATA_LANE_4;
				IO_DISP.MIPI_DSI.PHY_STATUS.word						= 0x01BC;
				ct_im_mipi_pclk_off();
			}
			else {
				// end
				Ddim_Print(("!! 3rd parameter fraud : %d\n", param3));
				return;
			}
			
			ret = Im_MIPI_Enter_Ulps();
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_MIPI_Enter_Ulps() : Normal end\n"));
				ct_im_mipi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("PHY_ULPS_CTRL   = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_ULPS_CTRL.word) );
				Ddim_Print( ("PHY_TX_TRIGGERS = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_TX_TRIGGERS.word) );
				Ddim_Print( ("PHY_IF_CFG      = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_IF_CFG.word) );
				Ddim_Print( ("PHY_STATUS      = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_STATUS.word) );
				Ddim_Print( ("----------\n") );
				ct_im_mipi_pclk_off();
			}
			else {
				Ddim_Print(("Im_MIPI_Enter_Ulps() : Error  err_code=0x%x\n", ret));
			}
		}
		else if( strcmp(argv[2], "exit") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// 4th param check
				param4 = atoi((const char *)argv[4]);
				if (param4 == 0) {
					// register setting
					ct_im_mipi_pclk_on();
					IO_DISP.MIPI_DSI.PHY_IF_CFG.bit.n_lanes = E_IM_MIPI_ACTIVE_DATA_LANE_1;
					IO_DISP.MIPI_DSI.PHY_STATUS.word = 0;
					ct_im_mipi_pclk_off();
				}
				else if (param4 == 1) {
					// register setting
					ct_im_mipi_pclk_on();
					IO_DISP.MIPI_DSI.PHY_IF_CFG.bit.n_lanes = E_IM_MIPI_ACTIVE_DATA_LANE_4;
					IO_DISP.MIPI_DSI.PHY_STATUS.word = 0;
					ct_im_mipi_pclk_off();
				}
				else {
					// end
					Ddim_Print(("!! 4th parameter fraud : %d\n", param4));
					return;
				}
			}
			else if (param3 == 1) {
				// Invalid setting (Data lane setting unmatch)
				ct_im_mipi_pclk_on();
				IO_DISP.MIPI_DSI.PHY_IF_CFG.bit.n_lanes = E_IM_MIPI_ACTIVE_DATA_LANE_3;
				IO_DISP.MIPI_DSI.PHY_STATUS.word = 0x01BC;
				ct_im_mipi_pclk_off();
			}
			else {
				// end
				Ddim_Print(("!! 3rd parameter fraud : %d\n", param3));
				return;
			}
			
			ret = Im_MIPI_Exit_Ulps();
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_MIPI_Exit_Ulps() : Normal end\n"));
				ct_im_mipi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("PHY_IF_CFG    = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_IF_CFG.word) );
				Ddim_Print( ("PHY_STATUS    = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_STATUS.word) );
				Ddim_Print( ("PHY_ULPS_CTRL = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_ULPS_CTRL.word) );
				Ddim_Print( ("----------\n") );
				ct_im_mipi_pclk_off();
			}
			else {
				Ddim_Print(("Im_MIPI_Exit_Ulps() : Error  err_code=0x%x\n", ret));
			}
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud\n"));
			return;
		}
	}
	else if ( strcmp(argv[1], "lane") == 0 ) {
		// 2nd param check
		if( strcmp(argv[2], "set") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 != 0) {
				// Invalid parameter (lane_info = NULL)
				ret = Im_MIPI_Set_Lane_Info(NULL);
				Ddim_Print(("Im_MIPI_Set_Lane_Info() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			// 4th param check
			param4 = atoi((const char *)argv[4]);
			if (param4 == 0) {
				// set paramter.
				lane_info.clock_lane.bit.SAP_TLPX	= 0;
				lane_info.clock_lane.bit.SAP_HS0	= 0;
				lane_info.clock_lane.bit.SAP_TRAIL	= 0;
				lane_info.clock_lane.bit.SAP_PRE	= 0;
				lane_info.data_lane.bit.SAP_TLPX	= 0;
				lane_info.data_lane.bit.SAP_HS0		= 0;
				lane_info.data_lane.bit.SAP_TRAIL	= 0;
				lane_info.data_lane.bit.SAP_PRE		= 0;
				lane_info.tclkp_post				= 0;
			}
			else if (param4 == 1) {
				// set paramter.
				lane_info.clock_lane.bit.SAP_TLPX	= 255;
				lane_info.clock_lane.bit.SAP_HS0	= 255;
				lane_info.clock_lane.bit.SAP_TRAIL	= 255;
				lane_info.clock_lane.bit.SAP_PRE	= 255;
				lane_info.data_lane.bit.SAP_TLPX	= 255;
				lane_info.data_lane.bit.SAP_HS0		= 255;
				lane_info.data_lane.bit.SAP_TRAIL	= 255;
				lane_info.data_lane.bit.SAP_PRE		= 255;
				lane_info.tclkp_post				= 63;
			}
			else {
				// end
				Ddim_Print(("!! 4rd parameter fraud : %d\n", param3));
				return;
			}
			
			ret = Im_MIPI_Set_Lane_Info(&lane_info);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_MIPI_Set_Lane_Info() : Normal end\n"));
				ct_im_mipi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("PHY_SETUP_CL = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_SETUP_CL.word) );
				Ddim_Print( ("PHY_SETUP_DL = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_SETUP_DL.word) );
				Ddim_Print( ("PHY_TCLKPOST = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_TCLKPOST.word) );
				Ddim_Print( ("----------\n") );
				ct_im_mipi_pclk_off();
			}
			else {
				Ddim_Print(("Im_MIPI_Set_Lane_Info() : Error  err_code=0x%x\n", ret));
			}
		}
		else if( strcmp(argv[2], "get") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// table clear.
				memset( &lane_info, 0, sizeof( T_IM_MIPI_DPI_LANE_INFO ) );
			}
			else {
				// Invalid parameter (lane_info = NULL)
				ret = Im_MIPI_Get_Lane_Info(NULL);
				Ddim_Print(("Im_MIPI_Get_Lane_Info() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			ret = Im_MIPI_Get_Lane_Info(&lane_info);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_MIPI_Get_Lane_Info() : Normal end\n"));
				ct_im_mipi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("result:\n") );
				Ddim_Print( ("clock_lane : 0x%lx\n", lane_info.clock_lane.word) );
				Ddim_Print( ("data_lane  : 0x%lx\n", lane_info.data_lane.word) );
				Ddim_Print( ("tclkp_post : 0x%x\n", lane_info.tclkp_post) );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("PHY_SETUP_CL = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_SETUP_CL.word) );
				Ddim_Print( ("PHY_SETUP_DL = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_SETUP_DL.word) );
				Ddim_Print( ("PHY_TCLKPOST = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_TCLKPOST.word) );
				Ddim_Print( ("----------\n") );
				ct_im_mipi_pclk_off();
			}
			else {
				Ddim_Print(("Im_MIPI_Get_Lane_Info() : Error  err_code=0x%x\n", ret));
			}
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud\n"));
			return;
		}
	}
	else if ( strcmp(argv[1], "dpi") == 0 ) {
		// 2nd param check
		if( strcmp(argv[2], "set") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 != 0) {
				// Invalid parameter (dpi = NULL)
				ret = Im_MIPI_Set_DPI_Resolution(NULL);
				Ddim_Print(("Im_MIPI_Set_DPI_Resolution() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			// 4th param check
			param4 = atoi((const char *)argv[4]);
			if (param4 == 0) {
				// set paramter.
				dpi.lane								= E_IM_MIPI_ACTIVE_DATA_LANE_1;
				dpi.vcid								= 0;
				dpi.color_coding						= D_IM_MIPI_COLOR_CODING_16BIT_CONFIG1;
				dpi.loosely18_en						= 0;
				dpi.cfg_pol.bit.dataen_active_low		= 0;
				dpi.cfg_pol.bit.vsync_active_low		= 0;
				dpi.cfg_pol.bit.hsync_active_low		= 0;
				dpi.cfg_pol.bit.shutd_active_low		= 0;
				dpi.cfg_pol.bit.colorm_acvite_low		= 0;
				dpi.mode_cfg							= E_IM_MIPI_MODE_CFG_VIDEO;
				dpi.vid_mode_cfg.bit.vid_mode_type		= D_IM_MIPI_VID_MODE_TYPE_NON_BURST_SYNC_PULSES;
				dpi.vid_mode_cfg.bit.lp_vsa_en			= 0;
				dpi.vid_mode_cfg.bit.lp_vbp_en			= 0;
				dpi.vid_mode_cfg.bit.lp_vfp_en			= 0;
				dpi.vid_mode_cfg.bit.lp_vact_en			= 0;
				dpi.vid_mode_cfg.bit.lp_hbp_en			= 0;
				dpi.vid_mode_cfg.bit.lp_hfp_en			= 0;
				dpi.vid_mode_cfg.bit.lp_cmd_en			= 0;
				dpi.vid_mode_cfg.bit.vpg_en				= 0;
				dpi.vid_mode_cfg.bit.vpg_mode			= 0;
				dpi.vid_mode_cfg.bit.vpg_orientation	= 0;
				dpi.vid_pkt_size						= 1;
				dpi.vid_num_chunks						= 0;
				dpi.vid_null_size						= 0;
				dpi.vid_hline_time						= 1;
				dpi.vid_hsa_time						= 1;
				dpi.vid_hbp_time						= 1;
				dpi.vid_vsa_lines						= 1;
				dpi.vid_vbp_lines						= 1;
				dpi.vid_vfp_lines						= 1;
				dpi.vid_vactive_lines					= 1;
				dpi.cmd_mode_cfg.bit.gen_sw_0p_tx		= 0;
				dpi.cmd_mode_cfg.bit.gen_sw_1p_tx		= 0;
				dpi.cmd_mode_cfg.bit.gen_sw_2p_tx		= 0;
				dpi.cmd_mode_cfg.bit.gen_lw_tx			= 0;
				dpi.cmd_mode_cfg.bit.dcs_sw_0p_tx		= 0;
				dpi.cmd_mode_cfg.bit.dcs_sw_1p_tx		= 0;
				dpi.cmd_mode_cfg.bit.dcs_lw_tx			= 0;
			}
			else if (param4 == 1) {
				// set paramter.
				dpi.lane								= E_IM_MIPI_ACTIVE_DATA_LANE_4;
				dpi.vcid								= 3;
				dpi.color_coding						= D_IM_MIPI_COLOR_CODING_12BIT_YCC420;
				dpi.loosely18_en						= 1;
				dpi.cfg_pol.bit.dataen_active_low		= 1;
				dpi.cfg_pol.bit.vsync_active_low		= 1;
				dpi.cfg_pol.bit.hsync_active_low		= 1;
				dpi.cfg_pol.bit.shutd_active_low		= 1;
				dpi.cfg_pol.bit.colorm_acvite_low		= 1;
				dpi.mode_cfg							= E_IM_MIPI_MODE_CFG_CMD;
				dpi.vid_mode_cfg.bit.vid_mode_type		= D_IM_MIPI_VID_MODE_TYPE_BURST;
				dpi.vid_mode_cfg.bit.lp_vsa_en			= 1;
				dpi.vid_mode_cfg.bit.lp_vbp_en			= 1;
				dpi.vid_mode_cfg.bit.lp_vfp_en			= 1;
				dpi.vid_mode_cfg.bit.lp_vact_en			= 1;
				dpi.vid_mode_cfg.bit.lp_hbp_en			= 1;
				dpi.vid_mode_cfg.bit.lp_hfp_en			= 1;
				dpi.vid_mode_cfg.bit.lp_cmd_en			= 1;
				dpi.vid_mode_cfg.bit.vpg_en				= 1;
				dpi.vid_mode_cfg.bit.vpg_mode			= 1;
				dpi.vid_mode_cfg.bit.vpg_orientation	= 1;
				dpi.vid_pkt_size						= 16383;
				dpi.vid_num_chunks						= 16383;
				dpi.vid_null_size						= 16383;
				dpi.vid_hline_time						= 32767;
				dpi.vid_hsa_time						= 4095;
				dpi.vid_hbp_time						= 4095;
				dpi.vid_vsa_lines						= 1023;
				dpi.vid_vbp_lines						= 1023;
				dpi.vid_vfp_lines						= 1023;
				dpi.vid_vactive_lines					= 16383;
				dpi.cmd_mode_cfg.bit.gen_sw_0p_tx		= 1;
				dpi.cmd_mode_cfg.bit.gen_sw_1p_tx		= 1;
				dpi.cmd_mode_cfg.bit.gen_sw_2p_tx		= 1;
				dpi.cmd_mode_cfg.bit.gen_lw_tx			= 1;
				dpi.cmd_mode_cfg.bit.dcs_sw_0p_tx		= 1;
				dpi.cmd_mode_cfg.bit.dcs_sw_1p_tx		= 1;
				dpi.cmd_mode_cfg.bit.dcs_lw_tx			= 1;
			}
			else {
				// end
				Ddim_Print(("!! 4th parameter fraud : %d\n", param4));
				return;
			}
			
			ret = Im_MIPI_Set_DPI_Resolution(&dpi);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_MIPI_Set_DPI_Resolution() : Normal end\n"));
				ct_im_mipi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("PHY_IF_CFG        = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_IF_CFG.word) );
				Ddim_Print( ("DPI_VCID          = 0x%lx\n", IO_DISP.MIPI_DSI.DPI_VCID.word) );
				Ddim_Print( ("DPI_COLOR_CODING  = 0x%lx\n", IO_DISP.MIPI_DSI.DPI_COLOR_CODING.word) );
				Ddim_Print( ("DPI_CFG_POL       = 0x%lx\n", IO_DISP.MIPI_DSI.DPI_CFG_POL.word) );
				Ddim_Print( ("MODE_CFG          = 0x%lx\n", IO_DISP.MIPI_DSI.MODE_CFG.word) );
				Ddim_Print( ("VID_MODE_CFG      = 0x%lx\n", IO_DISP.MIPI_DSI.VID_MODE_CFG.word) );
				Ddim_Print( ("VID_PKT_SIZE      = 0x%lx\n", IO_DISP.MIPI_DSI.VID_PKT_SIZE.word) );
				Ddim_Print( ("VID_NUM_CHUNKS    = 0x%lx\n", IO_DISP.MIPI_DSI.VID_NUM_CHUNKS.word) );
				Ddim_Print( ("VID_NULL_SIZE     = 0x%lx\n", IO_DISP.MIPI_DSI.VID_NULL_SIZE.word) );
				Ddim_Print( ("VID_HLINE_TIME    = 0x%lx\n", IO_DISP.MIPI_DSI.VID_HLINE_TIME.word) );
				Ddim_Print( ("VID_HSA_TIME      = 0x%lx\n", IO_DISP.MIPI_DSI.VID_HSA_TIME.word) );
				Ddim_Print( ("VID_HBP_TIME      = 0x%lx\n", IO_DISP.MIPI_DSI.VID_HBP_TIME.word) );
				Ddim_Print( ("VID_VSA_LINES     = 0x%lx\n", IO_DISP.MIPI_DSI.VID_VSA_LINES.word) );
				Ddim_Print( ("VID_VBP_LINES     = 0x%lx\n", IO_DISP.MIPI_DSI.VID_VBP_LINES.word) );
				Ddim_Print( ("VID_VFP_LINES     = 0x%lx\n", IO_DISP.MIPI_DSI.VID_VFP_LINES.word) );
				Ddim_Print( ("VID_VACTIVE_LINES = 0x%lx\n", IO_DISP.MIPI_DSI.VID_VACTIVE_LINES.word) );
				Ddim_Print( ("CMD_MODE_CFG      = 0x%lx\n", IO_DISP.MIPI_DSI.CMD_MODE_CFG.word) );
				Ddim_Print( ("----------\n") );
				ct_im_mipi_pclk_off();
			}
			else {
				Ddim_Print(("Im_MIPI_Set_DPI_Resolution() : Error  err_code=0x%x\n", ret));
			}
		}
		else if( strcmp(argv[2], "get") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// table clear.
				memset( &dpi, 0, sizeof( T_IM_MIPI_DPI_RESOLUTION ) );
			}
			else {
				// Invalid parameter (dpi = NULL)
				ret = Im_MIPI_Get_DPI_Resolution(NULL);
				Ddim_Print(("Im_MIPI_Get_DPI_Resolution() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			ret = Im_MIPI_Get_DPI_Resolution(&dpi);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_MIPI_Get_DPI_Resolution() : Normal end\n"));
				ct_im_mipi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("result:\n") );
				Ddim_Print( ("lane              : 0x%x\n", dpi.lane) );
				Ddim_Print( ("vcid              : 0x%x\n", dpi.vcid) );
				Ddim_Print( ("color_coding      : 0x%x\n", dpi.color_coding) );
				Ddim_Print( ("loosely18_en      : 0x%x\n", dpi.loosely18_en) );
				Ddim_Print( ("cfg_pol           : 0x%lx\n", dpi.cfg_pol.word) );
				Ddim_Print( ("mode_cfg          : 0x%x\n", dpi.mode_cfg) );
				Ddim_Print( ("vid_mode_cfg      : 0x%lx\n", dpi.vid_mode_cfg.word) );
				Ddim_Print( ("vid_pkt_size      : 0x%x\n", dpi.vid_pkt_size) );
				Ddim_Print( ("vid_num_chunks    : 0x%x\n", dpi.vid_num_chunks) );
				Ddim_Print( ("vid_null_size     : 0x%x\n", dpi.vid_null_size) );
				Ddim_Print( ("vid_hline_time    : 0x%x\n", dpi.vid_hline_time) );
				Ddim_Print( ("vid_hsa_time      : 0x%x\n", dpi.vid_hsa_time) );
				Ddim_Print( ("vid_hbp_time      : 0x%x\n", dpi.vid_hbp_time) );
				Ddim_Print( ("vid_vsa_lines     : 0x%x\n", dpi.vid_vsa_lines) );
				Ddim_Print( ("vid_vbp_lines     : 0x%x\n", dpi.vid_vbp_lines) );
				Ddim_Print( ("vid_vfp_lines     : 0x%x\n", dpi.vid_vfp_lines) );
				Ddim_Print( ("vid_vactive_lines : 0x%x\n", dpi.vid_vactive_lines) );
				Ddim_Print( ("cmd_mode_cfg      : 0x%lx\n", dpi.cmd_mode_cfg.word) );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("PHY_IF_CFG        = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_IF_CFG.word) );
				Ddim_Print( ("DPI_VCID          = 0x%lx\n", IO_DISP.MIPI_DSI.DPI_VCID.word) );
				Ddim_Print( ("DPI_COLOR_CODING  = 0x%lx\n", IO_DISP.MIPI_DSI.DPI_COLOR_CODING.word) );
				Ddim_Print( ("DPI_CFG_POL       = 0x%lx\n", IO_DISP.MIPI_DSI.DPI_CFG_POL.word) );
				Ddim_Print( ("MODE_CFG          = 0x%lx\n", IO_DISP.MIPI_DSI.MODE_CFG.word) );
				Ddim_Print( ("VID_MODE_CFG      = 0x%lx\n", IO_DISP.MIPI_DSI.VID_MODE_CFG.word) );
				Ddim_Print( ("VID_PKT_SIZE      = 0x%lx\n", IO_DISP.MIPI_DSI.VID_PKT_SIZE.word) );
				Ddim_Print( ("VID_NUM_CHUNKS    = 0x%lx\n", IO_DISP.MIPI_DSI.VID_NUM_CHUNKS.word) );
				Ddim_Print( ("VID_NULL_SIZE     = 0x%lx\n", IO_DISP.MIPI_DSI.VID_NULL_SIZE.word) );
				Ddim_Print( ("VID_HLINE_TIME    = 0x%lx\n", IO_DISP.MIPI_DSI.VID_HLINE_TIME.word) );
				Ddim_Print( ("VID_HSA_TIME      = 0x%lx\n", IO_DISP.MIPI_DSI.VID_HSA_TIME.word) );
				Ddim_Print( ("VID_HBP_TIME      = 0x%lx\n", IO_DISP.MIPI_DSI.VID_HBP_TIME.word) );
				Ddim_Print( ("VID_VSA_LINES     = 0x%lx\n", IO_DISP.MIPI_DSI.VID_VSA_LINES.word) );
				Ddim_Print( ("VID_VBP_LINES     = 0x%lx\n", IO_DISP.MIPI_DSI.VID_VBP_LINES.word) );
				Ddim_Print( ("VID_VFP_LINES     = 0x%lx\n", IO_DISP.MIPI_DSI.VID_VFP_LINES.word) );
				Ddim_Print( ("VID_VACTIVE_LINES = 0x%lx\n", IO_DISP.MIPI_DSI.VID_VACTIVE_LINES.word) );
				Ddim_Print( ("CMD_MODE_CFG      = 0x%lx\n", IO_DISP.MIPI_DSI.CMD_MODE_CFG.word) );
				Ddim_Print( ("----------\n") );
				ct_im_mipi_pclk_off();
			}
			else {
				Ddim_Print(("Im_MIPI_Get_DPI_Resolution() : Error  err_code=0x%x\n", ret));
			}
		}
		else if( strcmp(argv[2], "update") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// Set register
				ct_im_mipi_pclk_on();
				IO_DISP.MIPI_DSI.VID_SHADOW_CTRL.bit.vid_shadow_en	= D_IM_MIPI_ENABLE_ON;
				IO_DISP.MIPI_DSI.VID_SHADOW_CTRL.bit.vid_shadow_req	= D_IM_MIPI_ENABLE_OFF;
				ct_im_mipi_pclk_off();
			}
			else if (param3 == 1) {
				// Invalid register (Update disable)
				ct_im_mipi_pclk_on();
				IO_DISP.MIPI_DSI.VID_SHADOW_CTRL.bit.vid_shadow_en	= D_IM_MIPI_ENABLE_OFF;
				IO_DISP.MIPI_DSI.VID_SHADOW_CTRL.bit.vid_shadow_req	= D_IM_MIPI_ENABLE_OFF;
				ct_im_mipi_pclk_off();
			}
			else if (param3 == 2) {
				// Invalid register (Updateing now)
				ct_im_mipi_pclk_on();
				IO_DISP.MIPI_DSI.VID_SHADOW_CTRL.bit.vid_shadow_en	= D_IM_MIPI_ENABLE_ON;
				IO_DISP.MIPI_DSI.VID_SHADOW_CTRL.bit.vid_shadow_req	= D_IM_MIPI_ENABLE_ON;
				ct_im_mipi_pclk_off();
			}
			else {
				// end
				Ddim_Print(("!! 3rd parameter fraud : %d\n", param3));
				return;
			}
			
			ret = Im_MIPI_Update_Dpi();
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_MIPI_Update_Dpi() : Normal end\n"));
				ct_im_mipi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("VID_SHADOW_CTRL = 0x%lx\n", IO_DISP.MIPI_DSI.VID_SHADOW_CTRL.word) );
				Ddim_Print( ("----------\n") );
				ct_im_mipi_pclk_off();
			}
			else {
				Ddim_Print(("Im_MIPI_Update_Dpi() : Error  err_code=0x%x\n", ret));
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
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 != 0) {
				// Invalid parameter (config = NULL)
				ret = Im_MIPI_Set_Transmission_Configuration(NULL);
				Ddim_Print(("Im_MIPI_Set_Transmission_Configuration() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			// 4th param check
			param4 = atoi((const char *)argv[4]);
			if (param4 == 0) {
				// set paramter.
				config.clkmgr_cfg.bit.tx_esc_clk_division		= 0;
				config.clkmgr_cfg.bit.to_clk_division			= 0;
				config.dpi_lp_cmd_tim.bit.invact_lpcmd_time		= 0;
				config.dpi_lp_cmd_tim.bit.outvact_lpcmd_time	= 0;
				config.timer_count								= 0;
				config.phy_tmr_cfg.bit.phy_lp2hs_time			= 0;
				config.phy_tmr_cfg.bit.phy_hs2lp_time			= 0;
			}
			else if (param4 == 1) {
				// set paramter.
				config.clkmgr_cfg.bit.tx_esc_clk_division		= 255;
				config.clkmgr_cfg.bit.to_clk_division			= 255;
				config.dpi_lp_cmd_tim.bit.invact_lpcmd_time		= 255;
				config.dpi_lp_cmd_tim.bit.outvact_lpcmd_time	= 255;
				config.timer_count								= 65535;
				config.phy_tmr_cfg.bit.phy_lp2hs_time			= 255;
				config.phy_tmr_cfg.bit.phy_hs2lp_time			= 255;
			}
			else {
				// end
				Ddim_Print(("!! 4th parameter fraud : %d\n", param4));
				return;
			}
			
			ret = Im_MIPI_Set_Transmission_Configuration(&config);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_MIPI_Set_Transmission_Configuration() : Normal end\n"));
				ct_im_mipi_pclk_on();
				// check register
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("CLKMGR_CFG     = 0x%lx\n", IO_DISP.MIPI_DSI.CLKMGR_CFG.word) );
				Ddim_Print( ("DPI_LP_CMD_TIM = 0x%lx\n", IO_DISP.MIPI_DSI.DPI_LP_CMD_TIM.word) );
				Ddim_Print( ("TO_CNT_CFG     = 0x%lx\n", IO_DISP.MIPI_DSI.TO_CNT_CFG.word) );
				Ddim_Print( ("PHY_TMR_CFG    = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_TMR_CFG.word) );
				Ddim_Print( ("----------\n") );
				ct_im_mipi_pclk_off();
			}
			else {
				Ddim_Print(("Im_MIPI_Set_Transmission_Configuration() : Error  err_code=0x%x\n", ret));
			}
		}
		else if( strcmp(argv[2], "get") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// table clear.
				memset( &config, 0, sizeof( T_IM_MIPI_TRANS_CONFIG ) );
			}
			else {
				// Invalid parameter (config = NULL)
				ret = Im_MIPI_Get_Transmission_Configuration(NULL);
				Ddim_Print(("Im_MIPI_Get_Transmission_Configuration() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			ret = Im_MIPI_Get_Transmission_Configuration(&config);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_MIPI_Get_Transmission_Configuration() : Normal end\n"));
				ct_im_mipi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("result:\n") );
				Ddim_Print( ("clkmgr_cfg     : 0x%lx\n", config.clkmgr_cfg.word) );
				Ddim_Print( ("dpi_lp_cmd_tim : 0x%lx\n", config.dpi_lp_cmd_tim.word) );
				Ddim_Print( ("timer_count    : %d\n",   config.timer_count) );
				Ddim_Print( ("phy_tmr_cfg    : 0x%lx\n", config.phy_tmr_cfg.word) );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("CLKMGR_CFG     = 0x%lx\n", IO_DISP.MIPI_DSI.CLKMGR_CFG.word) );
				Ddim_Print( ("DPI_LP_CMD_TIM = 0x%lx\n", IO_DISP.MIPI_DSI.DPI_LP_CMD_TIM.word) );
				Ddim_Print( ("TO_CNT_CFG     = 0x%lx\n", IO_DISP.MIPI_DSI.TO_CNT_CFG.word) );
				Ddim_Print( ("PHY_TMR_CFG    = 0x%lx\n", IO_DISP.MIPI_DSI.PHY_TMR_CFG.word) );
				Ddim_Print( ("----------\n") );
				ct_im_mipi_pclk_off();
			}
			else {
				Ddim_Print(("Im_MIPI_Get_Transmission_Configuration() : Error  err_code=0x%x\n", ret));
			}
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud\n"));
			return;
		}
	}
	else if ( strcmp(argv[1], "pt") == 0 ) {
		// 2nd param check
		param2 = atoi((const char *)argv[2]);
		if (param2 == 0) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if ((param3 >= 0) && (param3 <= D_IM_MIPI_PACKET_NUM_MAX)) {
				// set paramter.
				header_num = param3;
			}
			else {
				// end
				Ddim_Print(("!! 3rd parameter fraud : %d\n", param3));
				return;
			}
			param4 = atoi((const char *)argv[4]);
			if (param4 >= 0) {
				// set paramter.
				payload_num = param4;
			}
			else {
				// end
				Ddim_Print(("!! 4rd parameter fraud : %d\n", param4));
				return;
			}
		}
		else if (param2 == 1) {
			header_num = 1;
			payload_num	= 0;
			ret = Im_MIPI_Packet_Transmission(header_num, payload_num, NULL, gCt_Im_Mipi_Payload);
			Ddim_Print(("Im_MIPI_Packet_Transmission() : Error  err_code=0x%x\n", ret));
			return;
		}
		else if (param2 == 2) {
			header_num	= 0;
			payload_num	= 1;
			ret = Im_MIPI_Packet_Transmission(header_num, payload_num, gCt_Im_Mipi_Header, NULL);
			Ddim_Print(("Im_MIPI_Packet_Transmission() : Error  err_code=0x%x\n", ret));
			return;
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud : %d\n", param2));
			return;
		}

		header_num	= 1;
		payload_num	= 1;
		ret = Im_MIPI_Packet_Transmission(header_num, payload_num, gCt_Im_Mipi_Header, gCt_Im_Mipi_Payload);
		if (ret == D_DDIM_OK) {
			Ddim_Print(("Im_MIPI_Packet_Transmission() : Normal end\n"));
			ct_im_mipi_pclk_on();
			// check register
			Ddim_Print( ("----------\n") );
			Ddim_Print( ("register:\n") );
			Ddim_Print( ("GEN_PLD_DATA   = 0x%lx\n", IO_DISP.MIPI_DSI.GEN_PLD_DATA.word) );
			Ddim_Print( ("GEN_HDR        = 0x%lx\n", IO_DISP.MIPI_DSI.GEN_HDR.word) );
			Ddim_Print( ("CMD_PKT_STATUS = 0x%lx\n", IO_DISP.MIPI_DSI.CMD_PKT_STATUS.word) );
			Ddim_Print( ("----------\n") );
			ct_im_mipi_pclk_off();
		}
		else {
			Ddim_Print(("Im_MIPI_Packet_Transmission() : Error  err_code=0x%x\n", ret));
		}
	}
	else if ( strcmp(argv[1], "timer") == 0 ) {
		// 2nd param check
		if( strcmp(argv[2], "set") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				// set paramter.
				count = 0x40;
			}
			else if (param3 == 1) {
				// set paramter.
				count = 0xFF;
			}
			else {
				// end
				Ddim_Print(("!! 3rd parameter fraud : %d\n", param3));
				return;
			}
			
			Im_MIPI_Set_Timer_Count(count);
			Ddim_Print(("Im_MIPI_Set_Timer_Count() : Normal end\n"));
			ct_im_mipi_pclk_on();
			// check get data
			Ddim_Print( ("----------\n") );
			Ddim_Print( ("register:\n") );
			Ddim_Print( ("CLKMGR_CFG = 0x%lx\n", IO_DISP.MIPI_DSI.CLKMGR_CFG.word) );
			Ddim_Print( ("----------\n") );
			ct_im_mipi_pclk_off();
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud\n"));
			return;
		}
	}
	else if ( strcmp(argv[1], "mask") == 0 ) {
		// 2nd param check
		if( strcmp(argv[2], "set") == 0 ) {
			// Input parameter clear
			int_msk1.word = 0;
			
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				int_msk1.bit.to_hs_tx			= 1;
				int_msk1.bit.dpi_pld_wr_err		= 0;
				int_msk1.bit.gen_cmd_wr_err		= 0;
				int_msk1.bit.gen_pld_wr_err		= 0;
				int_msk1.bit.gen_pld_send_err	= 0;
			}
			else if (param3 == 1) {
				int_msk1.bit.to_hs_tx			= 1;
				int_msk1.bit.dpi_pld_wr_err		= 1;
				int_msk1.bit.gen_cmd_wr_err		= 1;
				int_msk1.bit.gen_pld_wr_err		= 1;
				int_msk1.bit.gen_pld_send_err	= 1;
			}
			else {
				// end
				Ddim_Print(("!! 3rd parameter fraud : %d\n", param3));
				return;
			}
			
			Im_MIPI_Set_Int_Mask(int_msk1);
			
			// check register
			Ddim_Print(("Im_MIPI_Set_Int_Mask() : Normal end\n"));
			ct_im_mipi_pclk_on();
			Ddim_Print( ("----------\n") );
			Ddim_Print( ("register:\n") );
			Ddim_Print( ("INT_MSK1 = 0x%lx\n", IO_DISP.MIPI_DSI.INT_MSK1.word) );
			Ddim_Print( ("----------\n") );
			ct_im_mipi_pclk_off();
		}
		else if( strcmp(argv[2], "get") == 0 ) {
			// 3rd param check
			param3 = atoi((const char *)argv[3]);
			if (param3 == 0) {
				int_msk1.word = 0;
			}
			else {
				// Invalid parameter (int_msk1 = NULL)
				ret = Im_MIPI_Get_Int_Mask(NULL);
				Ddim_Print(("Im_MIPI_Get_Int_Mask() : Error  err_code=0x%x\n", ret));
				// end
				return;
			}
			
			ret = Im_MIPI_Get_Int_Mask(&int_msk1);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_MIPI_Get_Int_Mask() : Normal end\n"));
				ct_im_mipi_pclk_on();
				// check get data
				Ddim_Print( ("----------\n") );
				Ddim_Print( ("result:\n") );
				Ddim_Print( ("int_msk1 = 0x%lx\n", int_msk1.word) );
				Ddim_Print( ("register:\n") );
				Ddim_Print( ("INT_MSK1 = 0x%lx\n", IO_DISP.MIPI_DSI.INT_MSK1.word) );
				Ddim_Print( ("----------\n") );
				ct_im_mipi_pclk_off();
			}
			else {
				Ddim_Print(("Im_MIPI_Get_Int_Mask() : Error  err_code=0x%x\n", ret));
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
			if ((param3 >= E_IM_MIPI_INT_TYPE_TO_HS_TX) && (param3 <= E_IM_MIPI_INT_TYPE_GEN_PLD_SEND_ERR)) {
				interrupt_type = (E_IM_MIPI_INT_TYPE)param3;
			}
			else {
				// Invalid parameter (out of range)
				interrupt_type = E_IM_MIPI_INT_TYPE_MAX;
			}
			
			// 4th param check
			param4 = atoi((const char *)argv[4]);
			if (param4 == 0) {
				vp_callback = NULL;
			}
			else if (param4 == 1) {
				vp_callback = ct_im_mipi_cb0;
			}
			else {
				// end
				Ddim_Print(("!! 4th parameter fraud : %d\n", param4));
				return;
			}
			
			ret = Im_MIPI_Set_Int_Callback(interrupt_type, vp_callback);
			if (ret == D_DDIM_OK) {
				Ddim_Print(("Im_MIPI_Set_Int_Callback() : Normal end\n"));
			}
			else {
				Ddim_Print(("Im_MIPI_Set_Int_Callback() : Error  err_code=0x%x\n", ret));
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
			Im_MIPI_Set_Int_Callback(E_IM_MIPI_INT_TYPE_TO_HS_TX, ct_im_mipi_cb0);
			// interrupt enalbe (unmask).
			int_msk1.word = 0;
			Im_MIPI_Set_Int_Mask(int_msk1);
#ifdef CO_DEBUG_ON_PC
			// set Interrupt register : INT_ST1.to_hs_tx on.
			ct_im_mipi_pclk_on();
			IO_DISP.MIPI_DSI.INT_ST1.bit.to_hs_tx = 1;
			ct_im_mipi_pclk_off();
#endif	// CO_DEBUG_ON_PC
		}
		else {
			// end
			Ddim_Print(("!! 2nd parameter fraud : %d\n", param2));
			return;
		}
		
		// Interrupt Handler.
		Im_MIPI_Int_Handler();
		Ddim_Print(("Im_MIPI_Int_Handler() : Normal end\n"));
	}
	else{
		Ddim_Print( ("please check 1st parameter!\n") );
	}

	return ;
}

VOID Ct_Im_Mipi_PCSim_Test(VOID)
{
#ifdef CO_DEBUG_ON_PC
	int cmd_cnt = 0;
	
	// MIPI Test command data
	char mipi_test_argv1[][D_MIPI_TEST_CMD_LEN] = { "immipi", "init", "0" };
	char mipi_test_argv2[][D_MIPI_TEST_CMD_LEN] = { "immipi", "pc", "1" };
//	char mipi_test_argv3[][D_MIPI_TEST_CMD_LEN] = { "immipi", "ulps", "enter", "0", "0" };
	char mipi_test_argv4_1[][D_MIPI_TEST_CMD_LEN] = { "immipi", "ulps", "enter", "1" };
	char mipi_test_argv4_2[][D_MIPI_TEST_CMD_LEN] = { "immipi", "ulps", "enter", "2" };
//	char mipi_test_argv5[][D_MIPI_TEST_CMD_LEN] = { "immipi", "ulps", "exit", "0", "0" };
	char mipi_test_argv6[][D_MIPI_TEST_CMD_LEN] = { "immipi", "ulps", "exit", "1" };
	char mipi_test_argv7[][D_MIPI_TEST_CMD_LEN] = { "immipi", "lane", "set", "0", "0" };
	char mipi_test_argv8[][D_MIPI_TEST_CMD_LEN] = { "immipi", "lane", "set", "1" };
	char mipi_test_argv9[][D_MIPI_TEST_CMD_LEN] = { "immipi", "lane", "get", "0", "0" };
	char mipi_test_argv10[][D_MIPI_TEST_CMD_LEN] = { "immipi", "lane", "get", "1" };
	char mipi_test_argv11[][D_MIPI_TEST_CMD_LEN] = { "immipi", "dpi", "set", "0", "0" };
	char mipi_test_argv12[][D_MIPI_TEST_CMD_LEN] = { "immipi", "dpi", "set", "1" };
	char mipi_test_argv13[][D_MIPI_TEST_CMD_LEN] = { "immipi", "dpi", "get", "0", "0" };
	char mipi_test_argv14[][D_MIPI_TEST_CMD_LEN] = { "immipi", "dpi", "get", "1" };
//	char mipi_test_argv15[][D_MIPI_TEST_CMD_LEN] = { "immipi", "dpi", "update", "0" };
//	char mipi_test_argv16_1[][D_MIPI_TEST_CMD_LEN] = { "immipi", "dpi", "update", "1" };
//	char mipi_test_argv16_2[][D_MIPI_TEST_CMD_LEN] = { "immipi", "dpi", "update", "2" };
	char mipi_test_argv17[][D_MIPI_TEST_CMD_LEN] = { "immipi", "tc", "set", "0", "0" };
	char mipi_test_argv18[][D_MIPI_TEST_CMD_LEN] = { "immipi", "tc", "set", "1" };
	char mipi_test_argv19[][D_MIPI_TEST_CMD_LEN] = { "immipi", "tc", "get", "0", "0" };
	char mipi_test_argv20[][D_MIPI_TEST_CMD_LEN] = { "immipi", "tc", "get", "1" };
//	char mipi_test_argv21[][D_MIPI_TEST_CMD_LEN] = { "immipi", "pt", "0", "1" };
	char mipi_test_argv22_1[][D_MIPI_TEST_CMD_LEN] = { "immipi", "pt", "0", "0" };
	char mipi_test_argv22_2[][D_MIPI_TEST_CMD_LEN] = { "immipi", "pt", "1" };
	char mipi_test_argv23[][D_MIPI_TEST_CMD_LEN] = { "immipi", "timer", "set", "0" };
	char mipi_test_argv24[][D_MIPI_TEST_CMD_LEN] = { "immipi", "mask", "set", "0" };
	char mipi_test_argv25[][D_MIPI_TEST_CMD_LEN] = { "immipi", "mask", "get", "0", "0" };
	char mipi_test_argv26[][D_MIPI_TEST_CMD_LEN] = { "immipi", "mask", "get", "1" };
	char mipi_test_argv27[][D_MIPI_TEST_CMD_LEN] = { "immipi", "cb", "set", "0", "0" };
	char mipi_test_argv28[][D_MIPI_TEST_CMD_LEN] = { "immipi", "cb", "set", "5", "0" };
	char mipi_test_argv29[][D_MIPI_TEST_CMD_LEN] = { "immipi", "int", "0" };
	
	Ddim_Print(("\n======== MIPI ========\n"));
	Ddim_Print((">> Test 1-1-1\n"));
	cmd_cnt = (sizeof(mipi_test_argv1) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv1);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-2\n"));
	cmd_cnt = (sizeof(mipi_test_argv2) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv2);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-3\n"));
	
	// Set 
	
//	cmd_cnt = (sizeof(mipi_test_argv3) / D_MIPI_TEST_CMD_LEN);
//	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv3);
//	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-4\n"));
	cmd_cnt = (sizeof(mipi_test_argv4_1) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv4_1);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	cmd_cnt = (sizeof(mipi_test_argv4_2) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv4_2);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-5\n"));
//	cmd_cnt = (sizeof(mipi_test_argv5) / D_MIPI_TEST_CMD_LEN);
//	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv5);
//	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-6\n"));
	cmd_cnt = (sizeof(mipi_test_argv6) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv6);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-7\n"));
	cmd_cnt = (sizeof(mipi_test_argv7) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv7);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-8\n"));
	cmd_cnt = (sizeof(mipi_test_argv8) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv8);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-9\n"));
	cmd_cnt = (sizeof(mipi_test_argv9) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv9);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-10\n"));
	cmd_cnt = (sizeof(mipi_test_argv10) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv10);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-11\n"));
	cmd_cnt = (sizeof(mipi_test_argv11) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv11);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-12\n"));
	cmd_cnt = (sizeof(mipi_test_argv12) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv12);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-13\n"));
	cmd_cnt = (sizeof(mipi_test_argv13) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv13);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-14\n"));
	cmd_cnt = (sizeof(mipi_test_argv14) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv14);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-15\n"));
//	cmd_cnt = (sizeof(mipi_test_argv15) / D_MIPI_TEST_CMD_LEN);
//	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv15);
//	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-16\n"));
//	cmd_cnt = (sizeof(mipi_test_argv16_1) / D_MIPI_TEST_CMD_LEN);
//	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv16_1);
//	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
//	cmd_cnt = (sizeof(mipi_test_argv16_2) / D_MIPI_TEST_CMD_LEN);
//	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv16_2);
//	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-17\n"));
	cmd_cnt = (sizeof(mipi_test_argv17) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv17);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-18\n"));
	cmd_cnt = (sizeof(mipi_test_argv18) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv18);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-19\n"));
	cmd_cnt = (sizeof(mipi_test_argv19) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv19);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-20\n"));
	cmd_cnt = (sizeof(mipi_test_argv20) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv20);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-21\n"));
//	cmd_cnt = (sizeof(mipi_test_argv21) / D_MIPI_TEST_CMD_LEN);
//	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv21);
//	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-22\n"));
	cmd_cnt = (sizeof(mipi_test_argv22_1) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv22_1);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	cmd_cnt = (sizeof(mipi_test_argv22_2) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv22_2);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-23\n"));
	cmd_cnt = (sizeof(mipi_test_argv23) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv23);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-24\n"));
	cmd_cnt = (sizeof(mipi_test_argv24) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv24);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-25\n"));
	cmd_cnt = (sizeof(mipi_test_argv25) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv25);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-26\n"));
	cmd_cnt = (sizeof(mipi_test_argv26) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv26);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-27\n"));
	cmd_cnt = (sizeof(mipi_test_argv27) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv27);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-28\n"));
	cmd_cnt = (sizeof(mipi_test_argv28) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv28);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
	
	Ddim_Print(("\n>> Test 1-1-29\n"));
	cmd_cnt = (sizeof(mipi_test_argv29) / D_MIPI_TEST_CMD_LEN);
	ct_im_mipi_set_cmd(cmd_cnt, (char*)mipi_test_argv29);
	Ct_Im_Mipi_Main(cmd_cnt, gCmd_Argv);
#endif	// CO_DEBUG_ON_PC
}
