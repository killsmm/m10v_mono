/**
 * @file		im_mipi.c
 * @brief		MIPI driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "jdsdisp3a.h"
#include "im_mipi.h"
#include "dd_top.h"
#include "dd_arm.h"
#include "im_disp.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Bit of PHY_STATUS register corresponding to the specified number of lanes.
#define IM_MIPI_PHY_STATUS_1LANE				(0x003C)		// 1 data lane (lane 0) and clock lane					: PHY_STATUS[5:2].
#define IM_MIPI_PHY_STATUS_2LANES				(0x01BC)		// 2 lanes (lane 0 and lane 1) and clock lane			: PHY_STATUS[8:2].
#define IM_MIPI_PHY_STATUS_3LANES				(0x07BC)		// 3 lanes (lane 0, 1, and lane 2) and clock lane		: PHY_STATUS[10:2].
#define IM_MIPI_PHY_STATUS_4LANES				(0x1FBC)		// 4 lanes (lane 0, 1, 2,  and lane 3) and clock lane	: PHY_STATUS[12:2].

#define IM_MIPI_PHY_STATUS_ALL_ACTIVE_NOT		(0x1528)		// phy_ulpsactivenot0/1/2/3/clklane bits				: PHY_STATUS[12][10][8][5][3].

// Interrupt status.
#define IM_MIPI_INT_TO_HS_TX					(0x00000001)	// INT_ST1.to_hs_tx.
#define IM_MIPI_INT_DPI_PLD_WR_ERR				(0x00000080)	// INT_ST1.dpi_pld_wr_err.
#define IM_MIPI_INT_GEN_CMD_WR_ERR				(0x00000100)	// INT_ST1.gen_cmd_wr_err.
#define IM_MIPI_INT_GEN_PLD_WR_ERR				(0x00000200)	// INT_ST1.gen_pld_wr_err.
#define IM_MIPI_INT_GEN_PLD_SEND_ERR			(0x00000400)	// INT_ST1.gen_pld_send_err.
#define IM_MIPI_INT_NUM							(5)				// Max number.

// wait.
#define im_mipi_wait_us( usec )					Dd_ARM_Wait_ns( usec * 1000 )

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
static volatile VP_CALLBACK		gIM_MIPI_INT_Callback[IM_MIPI_INT_NUM] = {
	NULL, NULL, NULL, NULL, NULL
};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static VOID im_mipi_pclk_on(VOID);
static VOID im_mipi_pclk_off(VOID);

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief	PCLK change to ON.
 * @param	VOID.
 * @retval	VOID.
 */
static VOID im_mipi_pclk_on(VOID)
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
static VOID im_mipi_pclk_off(VOID)
{
#ifdef CO_ACT_PCLOCK
	Im_DISP_APB_Clock_Off();
#endif	// CO_ACT_PCLOCK
}

/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief	Initialization and recovery form shutdown.
 */
VOID Im_MIPI_Init(E_IM_MIPI_ACTIVE_DATA_LANE lane)
{
	im_mipi_pclk_on();
	
	// Initialization and Recovery form Shutdown.
	IO_DISP.MIPI_DSI.PHY_IF_CFG.bit.n_lanes			= lane;								// data lane.
	IO_DISP.MIPI_DSI.PHY_RSTZ.bit.phy_shutdownz		= 1;								// Power-down state.
	IO_DISP.MIPI_DSI.PWR_UP.bit.shutdownz			= E_IM_MIPI_SHUTDOWNZ_POWER_UP;		// Power-up.
	
	im_mipi_pclk_off();
}

/**
 * @brief	LPCLK control.
 */
VOID Im_MIPI_Lpclk_Ctrl(UCHAR lpclk)
{
	im_mipi_pclk_on();
	
	IO_DISP.MIPI_DSI.LPCLK_CTRL.bit.phy_txrequestclkhs	= lpclk;	// D-PHY PPI txrequestclkhs signal.
	
	// Wait Tinit (Tinit : min 1us).
	im_mipi_wait_us(1);

	im_mipi_pclk_off();
}

/**
 * @brief	Power-up/down control.
 */
VOID Im_MIPI_Power_Ctrl(E_IM_MIPI_PHY_SHUTDOWNZ power, E_IM_MIPI_SHUTDOWNZ reset)
{
	im_mipi_pclk_on();
	
	IO_DISP.MIPI_DSI.PHY_RSTZ.bit.phy_shutdownz		= power;	// Power-down state.
	
	// Power-up.
	if (power == E_IM_MIPI_PHY_SHUTDOWNZ_POWER_UP) {
		IO_DISP.MIPI_DSI.PWR_UP.bit.shutdownz		= reset;	// Power-up / Reset.
	}

	im_mipi_pclk_off();
}

/**
 * @brief	Enter ULPS on both clock lane and data lane.
 */
INT32 Im_MIPI_Enter_Ulps(VOID)
{
	ULONG	phy_status	= 0;
	
	im_mipi_pclk_on();
	
	// Verify the initial status.
	if ((IO_DISP.MIPI_DSI.PHY_ULPS_CTRL.word != 0) || 
		(IO_DISP.MIPI_DSI.PHY_TX_TRIGGERS.word != 0)) {
		// status NG.
		im_mipi_pclk_off();
		return D_IM_MIPI_NG;
	}
	
	// check number of data lanes.
	switch (IO_DISP.MIPI_DSI.PHY_IF_CFG.bit.n_lanes) {
		case E_IM_MIPI_ACTIVE_DATA_LANE_1:		// One data lane.
			phy_status = IM_MIPI_PHY_STATUS_1LANE;
			break;
		case E_IM_MIPI_ACTIVE_DATA_LANE_2:		// Two data lanes.
			phy_status = IM_MIPI_PHY_STATUS_2LANES;
			break;
		case E_IM_MIPI_ACTIVE_DATA_LANE_3:		// Three data lanes.
			phy_status = IM_MIPI_PHY_STATUS_3LANES;
			break;
		default:								// Four data lanes.
			phy_status = IM_MIPI_PHY_STATUS_4LANES;
			break;
	}
	
	// Verify that all active lanes are in Stop state.
	if ((IO_DISP.MIPI_DSI.PHY_STATUS.word & phy_status) != phy_status) {
		// status NG.
		im_mipi_pclk_off();
		return D_IM_MIPI_NG;
	}
	
	// ULPS mode Request on all active data lanes, Request on clock lane.
	IO_DISP.MIPI_DSI.PHY_ULPS_CTRL.word	= 0x00000005;

	// Wait until the D-PHY active lanes enter into ULPS.
	while ((IO_DISP.MIPI_DSI.PHY_STATUS.word & IM_MIPI_PHY_STATUS_4LANES) != 0) {
		// Wait.
		DDIM_User_Dly_Tsk(1);
	}
	
	im_mipi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Exit ULPS on both clock lane and data lane.
 */
INT32 Im_MIPI_Exit_Ulps(VOID)
{
	ULONG	phy_status	= 0;
	
	im_mipi_pclk_on();
	
	// check number of data lanes.
	switch (IO_DISP.MIPI_DSI.PHY_IF_CFG.bit.n_lanes) {
		case E_IM_MIPI_ACTIVE_DATA_LANE_1:		// One data lane.
			phy_status = IM_MIPI_PHY_STATUS_1LANE;
			break;
		case E_IM_MIPI_ACTIVE_DATA_LANE_2:		// Two data lanes.
			phy_status = IM_MIPI_PHY_STATUS_2LANES;
			break;
		case E_IM_MIPI_ACTIVE_DATA_LANE_3:		// Three data lanes.
			phy_status = IM_MIPI_PHY_STATUS_3LANES;
			break;
		default:								// Four data lanes.
			phy_status = IM_MIPI_PHY_STATUS_4LANES;
			break;
	}
	
	// Verify that all active lanes are ULPS.
	if ((IO_DISP.MIPI_DSI.PHY_STATUS.word & phy_status) != 0) {
		// status NG.
		im_mipi_pclk_off();
		return D_IM_MIPI_NG;
	}
	
	// Without deasserting the ULPS request bits, assert the Exit ULPS bits by setting.
	IO_DISP.MIPI_DSI.PHY_ULPS_CTRL.word	= 0x000F;

	// Wait until all active lanes exit ULPS.
	while ((IO_DISP.MIPI_DSI.PHY_STATUS.word & IM_MIPI_PHY_STATUS_ALL_ACTIVE_NOT) != IM_MIPI_PHY_STATUS_ALL_ACTIVE_NOT) {
		// Wait.
		DDIM_User_Dly_Tsk(1);
	}
	
	// Deassert the ULPS request and the ULPS exit bits by setting.
	IO_DISP.MIPI_DSI.PHY_ULPS_CTRL.word	= 0x0000;
	
	im_mipi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set clock lane / data lane / TCLK POST.
 */
INT32 Im_MIPI_Set_Lane_Info( T_IM_MIPI_DPI_LANE_INFO const *const lane_info )
{
#ifdef CO_PARAM_CHECK
	if (lane_info == NULL) {
		Ddim_Assertion(("Im_MIPI_Set_Lane_Info Input_Param_Err status NULL\n"));
		return D_IM_MIPI_INPUT_PARAM_ERROR;
	}
#endif
	im_mipi_pclk_on();
	
	// set Lane.
	IO_DISP.MIPI_DSI.PHY_SETUP_CL.word			= lane_info->clock_lane.word;	// clock lane.
	IO_DISP.MIPI_DSI.PHY_SETUP_DL.word			= lane_info->data_lane.word;	// data lane.
	IO_DISP.MIPI_DSI.PHY_TCLKPOST.bit.TCLK_POST	= lane_info->tclkp_post;		// TCLK POST.
	
	im_mipi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Get clock lane / data lane / TCLK POST.
 */
INT32 Im_MIPI_Get_Lane_Info( T_IM_MIPI_DPI_LANE_INFO *const lane_info )
{
#ifdef CO_PARAM_CHECK
	if (lane_info == NULL) {
		Ddim_Assertion(("Im_MIPI_Set_Lane_Info Input_Param_Err status NULL\n"));
		return D_IM_MIPI_INPUT_PARAM_ERROR;
	}
#endif
	im_mipi_pclk_on();
	
	// Get Lane.
	lane_info->clock_lane.word	= IO_DISP.MIPI_DSI.PHY_SETUP_CL.word;			// clock lane.
	lane_info->data_lane.word	= IO_DISP.MIPI_DSI.PHY_SETUP_DL.word;			// data lane.
	lane_info->tclkp_post		= IO_DISP.MIPI_DSI.PHY_TCLKPOST.bit.TCLK_POST;	// TCLK POST.
	
	im_mipi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set DPI Resolution data.
 */
INT32 Im_MIPI_Set_DPI_Resolution( T_IM_MIPI_DPI_RESOLUTION const *const dpi )
{
#ifdef CO_PARAM_CHECK
	if (dpi == NULL) {
		Ddim_Assertion(("Im_MIPI_Set_DPI_Resolution Input_Param_Err status NULL\n"));
		return D_IM_MIPI_INPUT_PARAM_ERROR;
	}
#endif
	im_mipi_pclk_on();
	
	// Global configuration.
	IO_DISP.MIPI_DSI.PHY_IF_CFG.bit.n_lanes					= dpi->lane;				// data lane.
	
	// Configure the DPI Interface to define how the DPI interface interacts with the controller.
	IO_DISP.MIPI_DSI.DPI_VCID.word							= dpi->vcid;				// virtual channel.
	IO_DISP.MIPI_DSI.DPI_COLOR_CODING.bit.dpi_color_coding	= dpi->color_coding;		// color coding.
	IO_DISP.MIPI_DSI.DPI_COLOR_CODING.bit.loosely18_en		= dpi->loosely18_en;		// loosely enable.
	IO_DISP.MIPI_DSI.DPI_CFG_POL.word						= dpi->cfg_pol.word;		// configures the polarity of the DPI signals.
	
	// set operation mode.
	IO_DISP.MIPI_DSI.MODE_CFG.bit.cmd_video_mode			= dpi->mode_cfg;
	
	if (dpi->mode_cfg == E_IM_MIPI_MODE_CFG_VIDEO) {
		// Video mode.
		
		// Select the Video Transmission Mode.
		IO_DISP.MIPI_DSI.VID_MODE_CFG.word						= dpi->vid_mode_cfg.word;	// configure low-power transitions.
		IO_DISP.MIPI_DSI.VID_PKT_SIZE.word						= dpi->vid_pkt_size;		// configure size of the active line period.
		
		// check Video mode transmission type : Burst Mode / Non-Burst.
		if ((dpi->vid_mode_cfg.bit.vid_mode_type & D_IM_MIPI_VID_MODE_TYPE_BURST) != D_IM_MIPI_VID_MODE_TYPE_BURST)
		{
			// Non-Burst.
			IO_DISP.MIPI_DSI.VID_NUM_CHUNKS.word				= dpi->vid_num_chunks;		// configures the number of chunks to use.
			IO_DISP.MIPI_DSI.VID_NULL_SIZE.word					= dpi->vid_null_size;		// configures ths size of null packets.
		}
		
		// Define the DPI Horizontal timing configuration.
		IO_DISP.MIPI_DSI.VID_HLINE_TIME.word					= dpi->vid_hline_time;		// the overall time for each video line.
	    IO_DISP.MIPI_DSI.VID_HSA_TIME.word						= dpi->vid_hsa_time;		// the video Horizontal Synchronism Active time.
		IO_DISP.MIPI_DSI.VID_HBP_TIME.word						= dpi->vid_hbp_time;		// the video Horizontal Back Porch time.
		
		// Define the Vertical line configuration.
		IO_DISP.MIPI_DSI.VID_VSA_LINES.word						= dpi->vid_vsa_lines;		// DPI Vertical Sync Active period.
	    IO_DISP.MIPI_DSI.VID_VBP_LINES.word						= dpi->vid_vbp_lines;		// DPI Vertical Back Porch period.
		IO_DISP.MIPI_DSI.VID_VFP_LINES.word						= dpi->vid_vfp_lines;		// DPI Vertical Front Porch period.
		IO_DISP.MIPI_DSI.VID_VACTIVE_LINES.word					= dpi->vid_vactive_lines;	// DPI Vertical Active period.
	}
	else {
		// Commnad mode.
		IO_DISP.MIPI_DSI.CMD_MODE_CFG.word						= dpi->cmd_mode_cfg.word;	// configure command transitions.
	}
	
	im_mipi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Get DPI Resolution data.
 */
INT32 Im_MIPI_Get_DPI_Resolution(T_IM_MIPI_DPI_RESOLUTION *const dpi)
{
#ifdef CO_PARAM_CHECK
	if (dpi == NULL) {
		Ddim_Assertion(("Im_MIPI_Get_DPI_Resolution Input_Param_Err status NULL\n"));
		return D_IM_MIPI_INPUT_PARAM_ERROR;
	}
#endif
	im_mipi_pclk_on();
	
	// Get DPI Resolution.
	// PHY_IF_CFG.
	dpi->lane				= (E_IM_MIPI_ACTIVE_DATA_LANE)IO_DISP.MIPI_DSI.PHY_IF_CFG.bit.n_lanes;
	// DPI_VCID/DPI_COLOR_CODING/DPI_CFG_POL.
	dpi->vcid				= IO_DISP.MIPI_DSI.DPI_VCID.word;
	dpi->color_coding		= IO_DISP.MIPI_DSI.DPI_COLOR_CODING.bit.dpi_color_coding;
	dpi->loosely18_en		= IO_DISP.MIPI_DSI.DPI_COLOR_CODING.bit.loosely18_en;
	dpi->cfg_pol.word		= IO_DISP.MIPI_DSI.DPI_CFG_POL.word;
	// MODE_CFG.
	dpi->mode_cfg			= (E_IM_MIPI_MODE_CFG)IO_DISP.MIPI_DSI.MODE_CFG.bit.cmd_video_mode;
	// VID_MODE_CFG/VID_PKT_SIZE/VID_NUM_CHUNKS/VID_NULL_SIZE.
	dpi->vid_mode_cfg.word	= IO_DISP.MIPI_DSI.VID_MODE_CFG.word;
	dpi->vid_pkt_size		= IO_DISP.MIPI_DSI.VID_PKT_SIZE.word;
	dpi->vid_num_chunks		= IO_DISP.MIPI_DSI.VID_NUM_CHUNKS.word;
	dpi->vid_null_size		= IO_DISP.MIPI_DSI.VID_NULL_SIZE.word;
	// VID_HLINE_TIME/VID_HSA_TIME/VID_HBP_TIME.
	dpi->vid_hline_time		= IO_DISP.MIPI_DSI.VID_HLINE_TIME.word;
    dpi->vid_hsa_time		= IO_DISP.MIPI_DSI.VID_HSA_TIME.word;
	dpi->vid_hbp_time		= IO_DISP.MIPI_DSI.VID_HBP_TIME.word;
	// VID_VSA_LINES/VID_VBP_LINES/VID_VFP_LINES/VID_VACTIVE_LINES.
	dpi->vid_vsa_lines		= IO_DISP.MIPI_DSI.VID_VSA_LINES.word;
    dpi->vid_vbp_lines		= IO_DISP.MIPI_DSI.VID_VBP_LINES.word;
	dpi->vid_vfp_lines		= IO_DISP.MIPI_DSI.VID_VFP_LINES.word;
	dpi->vid_vactive_lines	= IO_DISP.MIPI_DSI.VID_VACTIVE_LINES.word;
	// CMD_MODE_CFG.
	dpi->cmd_mode_cfg.word	= IO_DISP.MIPI_DSI.CMD_MODE_CFG.word;
	
	im_mipi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Updating the DPI Configuration.
 */
INT32 Im_MIPI_Update_Dpi(VOID)
{
	im_mipi_pclk_on();
	
	// Update availability check.
	if (IO_DISP.MIPI_DSI.VID_SHADOW_CTRL.bit.vid_shadow_en == D_IM_MIPI_ENABLE_OFF) {
		// Update disable.
		im_mipi_pclk_off();
		return D_IM_MIPI_NG;
	}

	// Update check.
	if (IO_DISP.MIPI_DSI.VID_SHADOW_CTRL.bit.vid_shadow_req == D_IM_MIPI_ENABLE_ON) {
		// Updateing now.
		im_mipi_pclk_off();
		return D_IM_MIPI_NG;
	}
	
	// Request update.
	IO_DISP.MIPI_DSI.VID_SHADOW_CTRL.bit.vid_shadow_req = D_IM_MIPI_ENABLE_ON;
	
	// wait update complete.
	if (IO_DISP.MIPI_DSI.VID_SHADOW_CTRL.bit.vid_shadow_req == D_IM_MIPI_ENABLE_ON) {
		// 100us wait.
		im_mipi_wait_us(100);
	}
	
	im_mipi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Transmission Configuration.
 */
INT32 Im_MIPI_Set_Transmission_Configuration(T_IM_MIPI_TRANS_CONFIG const *const config)
{
#ifdef CO_PARAM_CHECK
	if (config == NULL) {
		Ddim_Assertion(("Im_MIPI_Set_Transmission_Configuration Input_Param_Err status NULL\n"));
		return D_IM_MIPI_INPUT_PARAM_ERROR;
	}
#endif
	im_mipi_pclk_on();
	
	// Set CLKMGR_CFG.
	IO_DISP.MIPI_DSI.CLKMGR_CFG.word			= config->clkmgr_cfg.word;
	
	// Set DPI_LP_CMD_TIM.
	IO_DISP.MIPI_DSI.DPI_LP_CMD_TIM.word		= config->dpi_lp_cmd_tim.word;
	
	// Set TO_CNT_CFG(timer counter).
	IO_DISP.MIPI_DSI.TO_CNT_CFG.bit.hstx_to_cnt	= config->timer_count;
	
	// Set PHY_TMR_LPCLK_CFG.
	IO_DISP.MIPI_DSI.PHY_TMR_LPCLK_CFG.word		= config->phy_tmr_lpclk_cfg.word;

	// Set PHY_TMR_CFG.
	IO_DISP.MIPI_DSI.PHY_TMR_CFG.word			= config->phy_tmr_cfg.word;
	
	im_mipi_pclk_off();
	
	return D_DDIM_OK;
}


/**
 * @brief	Get Transmission Configuration.
 */
INT32 Im_MIPI_Get_Transmission_Configuration(T_IM_MIPI_TRANS_CONFIG *const config)
{
#ifdef CO_PARAM_CHECK
	if (config == NULL) {
		Ddim_Assertion(("Im_MIPI_Get_Transmission_Configuration Input_Param_Err status NULL\n"));
		return D_IM_MIPI_INPUT_PARAM_ERROR;
	}
#endif
	im_mipi_pclk_on();
	
	// Get CLKMGR_CFG.
	config->clkmgr_cfg.word		= IO_DISP.MIPI_DSI.CLKMGR_CFG.word;
	
	// Get DPI_LP_CMD_TIM.
	config->dpi_lp_cmd_tim.word	= IO_DISP.MIPI_DSI.DPI_LP_CMD_TIM.word;
	
	// Get TO_CNT_CFG.
	config->timer_count			= IO_DISP.MIPI_DSI.TO_CNT_CFG.bit.hstx_to_cnt;
	
	// Get PHY_TMR_LPCLK_CFG.
	config->phy_tmr_lpclk_cfg.word	= IO_DISP.MIPI_DSI.PHY_TMR_LPCLK_CFG.word;

	// Get PHY_TMR_CFG.
	config->phy_tmr_cfg.word	= IO_DISP.MIPI_DSI.PHY_TMR_CFG.word;
	
	im_mipi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Paket Transmission.
 */
INT32 Im_MIPI_Packet_Transmission(UCHAR header_num, UCHAR payload_num, const U_IM_MIPI_GEN_HDR header[D_IM_MIPI_PACKET_NUM_MAX], const ULONG payload[D_IM_MIPI_PACKET_NUM_MAX])
{
	INT32 loop;
	INT32 wait_counter1 = 0;
	INT32 wait_counter2 = 0;
	
#ifdef CO_PARAM_CHECK
	if ((header == NULL) || (payload == NULL)) {
		Ddim_Assertion(("Im_MIPI_Packet_Transmission Input_Param_Err status NULL\n"));
		return D_IM_MIPI_INPUT_PARAM_ERROR;
	}
	if (header_num > D_IM_MIPI_PACKET_NUM_MAX) {
		Ddim_Assertion(("Im_MIPI_Packet_Transmission Input_Param_Err pkt_num=0\n"));
		return D_IM_MIPI_INPUT_PARAM_ERROR;
	}
#endif
	im_mipi_pclk_on();

	for (loop = 0; loop < payload_num; loop++) {
		// Write payload data.
		IO_DISP.MIPI_DSI.GEN_PLD_DATA.word = payload[loop];
	}
	
	for (loop = 0; loop < header_num; loop++) {
		// Write header data.
		IO_DISP.MIPI_DSI.GEN_HDR.word = header[loop].word;
	}
	
	// wait FIFO empty.
	while(wait_counter1 < 100) {
		if( IO_DISP.MIPI_DSI.CMD_PKT_STATUS.bit.gen_cmd_empty != D_IM_MIPI_ENABLE_OFF) {
			break;
		}
		wait_counter1++;
		// wait.
		im_mipi_wait_us(100);
	}
	
	// PHY status check
	while(wait_counter2 < 100) {
		if( (IO_DISP.MIPI_DSI.PHY_STATUS.bit.phy_stopstate0lane != D_IM_MIPI_ENABLE_OFF) &&
			(IO_DISP.MIPI_DSI.PHY_STATUS.bit.phy_stopstate1lane != D_IM_MIPI_ENABLE_OFF) &&
			(IO_DISP.MIPI_DSI.PHY_STATUS.bit.phy_stopstate2lane != D_IM_MIPI_ENABLE_OFF) &&
			(IO_DISP.MIPI_DSI.PHY_STATUS.bit.phy_stopstate3lane != D_IM_MIPI_ENABLE_OFF) ) {
			break;
		}
		wait_counter2++;
		// wait.
		im_mipi_wait_us(100);
	}
	if( (wait_counter1 == 100) || (wait_counter2 == 100) ){
		Ddim_Print(("Im_MIPI_Packet_Transmission TimeOut Error. wait1=%x,wait2=%x\n", wait_counter1,wait_counter2));
		return D_DDIM_TIMEOUT;
	}

	im_mipi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Timeer count.
 */
VOID Im_MIPI_Set_Timer_Count(UCHAR count)
{
	im_mipi_pclk_on();
	
	// Set CLKMGR_CFG.to_clk_division.
	IO_DISP.MIPI_DSI.CLKMGR_CFG.bit.to_clk_division	= count;
	
	im_mipi_pclk_off();
}

/**
 * @brief	Set Interrupt mask.
 */
VOID Im_MIPI_Set_Int_Mask(U_IM_MIPI_INT_MSK1 int_msk1)
{
	im_mipi_pclk_on();
	
	// Set INT_MSK1.
	IO_DISP.MIPI_DSI.INT_MSK1.word	= int_msk1.word;
	
	im_mipi_pclk_off();
}

/**
 * @brief	Get Interrupt mask.
 */
INT32 Im_MIPI_Get_Int_Mask(U_IM_MIPI_INT_MSK1* int_msk1)
{
#ifdef CO_PARAM_CHECK
	if (int_msk1 == NULL) {
		Ddim_Assertion(("Im_MIPI_Get_Int_Mask Input_Param_Err status NULL\n"));
		return D_IM_MIPI_INPUT_PARAM_ERROR;
	}
#endif
	im_mipi_pclk_on();
	
	// Get INT_MSK1.
	int_msk1->word	= IO_DISP.MIPI_DSI.INT_MSK1.word;
	
	im_mipi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set interrupt callback function.
 */
INT32 Im_MIPI_Set_Int_Callback(E_IM_MIPI_INT_TYPE interrupt_type, VP_CALLBACK vp_callback)
{
#ifdef CO_PARAM_CHECK
	if (interrupt_type == E_IM_MIPI_INT_TYPE_MAX) {
		Ddim_Assertion(("Im_MIPI_Set_Int_Callback Input_Param_Err NG\n"));
		return D_IM_MIPI_INPUT_PARAM_ERROR;
	}
#endif

	// Set interrupt callback.
	gIM_MIPI_INT_Callback[interrupt_type] = vp_callback;

	return D_DDIM_OK;
}

/**
 * @brief	Interrupt handler.
 */
VOID Im_MIPI_Int_Handler(VOID)
{
	INT32 loop;
	
	// INT_ST1 register bit.
	ULONG int_st1_bit[] = {
		IM_MIPI_INT_TO_HS_TX,
		IM_MIPI_INT_DPI_PLD_WR_ERR,
		IM_MIPI_INT_GEN_CMD_WR_ERR,
		IM_MIPI_INT_GEN_PLD_WR_ERR,
		IM_MIPI_INT_GEN_PLD_SEND_ERR
	};
	
	im_mipi_pclk_on();
	
	// Interrupt status.
	for (loop = 0; loop < sizeof(int_st1_bit)/sizeof(int_st1_bit[0]); loop++) {
		// check register bit.
		if ((IO_DISP.MIPI_DSI.INT_ST1.word & int_st1_bit[loop]) == int_st1_bit[loop]) {
			// callback.
			if (gIM_MIPI_INT_Callback[loop] != NULL) {
				gIM_MIPI_INT_Callback[loop]();
			}
		}
	}
	
	im_mipi_pclk_off();
	
	return;
}

