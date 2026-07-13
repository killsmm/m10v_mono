/**
 * @file		im_hdmi.c
 * @brief		HDMI Transmitter Controller driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "jdsdisp3a.h"
#include "im_hdmi.h"
#include "dd_arm.h"
#include "im_disp.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Interrupt flag.
#define D_IM_HDMI_INT_FLG_I2CM				(0x00000001)	// i2cm.
#define D_IM_HDMI_INT_FLG_I2CMPHY			(0x00000002)	// i2cmphy.
#define D_IM_HDMI_INT_FLG_PHY_LOCK			(0x00000004)	// phy_lock.

// wait.
#define im_hdmi_wait_us( usec )				Dd_ARM_Wait_ns( usec * 1000 )
#define im_hdmi_wait_ns( nsec )				Dd_ARM_Wait_ns( nsec )

// PHY Register Address (Offset).
#define D_IM_HDMI_PHY_ADR_PWRCTRL			(0x00)			// PWRCTRL.
#define D_IM_HDMI_PHY_ADR_OPMODE_PLLCFG		(0x06)			// OPMODE_PLLCFG.
#define D_IM_HDMI_PHY_ADR_CKSYMTXCTRL		(0x09)			// CKSYMTXCTRL.
#define D_IM_HDMI_PHY_ADR_VLEVCTRL			(0x0E)			// VLEVCTRL.
#define D_IM_HDMI_PHY_ADR_PLLCURRCTRL		(0x10)			// PLLCURRCTRL.
#define D_IM_HDMI_PHY_ADR_PLLCTRL			(0x14)			// PLLCTRL.
#define D_IM_HDMI_PHY_ADR_PLLGMPCTRL		(0x15)			// PLLGMPCTRL.
#define D_IM_HDMI_PHY_ADR_TXTERM			(0x19)			// TXTERM.

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
static volatile VP_HDMI_CALLBACK gIM_HDMI_INT_Callback = NULL;

const static UCHAR gIM_HDMI_Phy_Register_Address[D_IM_HDMI_PHY_REG_NUM] = {
	D_IM_HDMI_PHY_ADR_PWRCTRL,
	D_IM_HDMI_PHY_ADR_OPMODE_PLLCFG,
	D_IM_HDMI_PHY_ADR_CKSYMTXCTRL,
	D_IM_HDMI_PHY_ADR_VLEVCTRL,
	D_IM_HDMI_PHY_ADR_PLLCURRCTRL,
	D_IM_HDMI_PHY_ADR_PLLCTRL,
	D_IM_HDMI_PHY_ADR_PLLGMPCTRL,
	D_IM_HDMI_PHY_ADR_TXTERM
};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
static VOID im_hdmi_pclk_on(VOID);
static VOID im_hdmi_pclk_off(VOID);
static VOID im_hdmi_callback_interrupt_i2cm(VOID);
static VOID im_hdmi_callback_interrupt_i2cmphy(VOID);
static VOID im_hdmi_callback_interrupt_phy_lock(VOID);
static INT32 im_hdmi_phy_write(UCHAR address, USHORT data);
static INT32 im_hdmi_phy_read(UCHAR address, USHORT *data);		// No use.
static INT32 im_hdmi_configure_pll(T_IM_HDMI_PLL_CONFIG const *const config);
static INT32 im_hdmi_configure_infoframes(T_IM_HDMI_INFO_FRAMES const *const config);
static E_IM_HDMI_INT_TYPE im_hdmi_get_interrupt_type(E_IM_HDMI_INT_REG int_reg, UCHAR reg_bit);
static VOID im_hdmi_check_interrupt_fc(VOID);
static VOID im_hdmi_check_interrupt_as(VOID);
static VOID im_hdmi_check_interrupt_phy(VOID);
static VOID im_hdmi_check_interrupt_i2cm(VOID);
static VOID im_hdmi_check_interrupt_cec(VOID);
static VOID im_hdmi_check_interrupt_vp(VOID);
static VOID im_hdmi_check_interrupt_i2cmphy(VOID);

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief	PCLK change to ON.
 * @param	VOID.
 * @retval	VOID.
 */
static VOID im_hdmi_pclk_on(VOID)
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
static VOID im_hdmi_pclk_off(VOID)
{
#ifdef CO_ACT_PCLOCK
	Im_DISP_APB_Clock_Off();
#endif	// CO_ACT_PCLOCK
}

/**
 * @brief	Interrupt callback (i2cm).
 * @param	VOID.
 * @retval	VOID.
 */
static VOID im_hdmi_callback_interrupt_i2cm(VOID)
{
	DDIM_USER_ER ret = D_DDIM_USER_E_OK;
	
	ret = DDIM_User_Set_Flg(FID_IM_HDMI, D_IM_HDMI_INT_FLG_I2CM);
	if(ret != D_DDIM_USER_E_OK){
		Ddim_Print(("Error DDIM_User_Set_Flg!! ret = 0x%x\n", ret));
	}
}

/**
 * @brief	Interrupt callback (i2cmphy).
 * @param	VOID.
 * @retval	VOID.
 */
static VOID im_hdmi_callback_interrupt_i2cmphy(VOID)
{
	DDIM_USER_ER ret = D_DDIM_USER_E_OK;
	
	ret = DDIM_User_Set_Flg(FID_IM_HDMI, D_IM_HDMI_INT_FLG_I2CMPHY);
	if(ret != D_DDIM_USER_E_OK){
		Ddim_Print(("im_hdmi_callback_interrupt_i2cmphy Error DDIM_User_Set_Flg!! ret = 0x%x\n", ret));
	}
}

/**
 * @brief	Interrupt callback (phy_lock).
 * @param	VOID.
 * @retval	VOID.
 */
static VOID im_hdmi_callback_interrupt_phy_lock(VOID)
{
	DDIM_USER_ER ret = D_DDIM_USER_E_OK;
	
	ret = DDIM_User_Set_Flg(FID_IM_HDMI, D_IM_HDMI_INT_FLG_PHY_LOCK);
	if(ret != D_DDIM_USER_E_OK){
		Ddim_Print(("im_hdmi_callback_interrupt_phy_lock Error DDIM_User_Set_Flg!! ret = 0x%x\n", ret));
	}
}

/**
 * @brief	PHY Write.
 * @param[in]	address		PHY Register Address (Offset).
 * @param[in]	data		Write data.
 * @retval	D_DDIM_OK						Success.
 * @retval	D_IM_HDMI_TIMEOUT				timeout.
 */
static INT32 im_hdmi_phy_write(UCHAR address, USHORT data)
{
	DDIM_USER_FLGPTN flgptn = 0;
	
	// clear Interrupt flag.
	DDIM_User_Clr_Flg(FID_IM_HDMI, ~(D_IM_HDMI_INT_FLG_I2CMPHY));

	IO_DISP.HDMI_TX.phy_i2cm_div.bit.fast_std_mode		= 0; // 0:Standard Mode

	// Set the PHY slave address.
	IO_DISP.HDMI_TX.phy_i2cm_slave.byte[0]				= 0x69;
	// Write register address.
	IO_DISP.HDMI_TX.phy_i2cm_address					= address;
	// Write data.
	IO_DISP.HDMI_TX.phy_i2cm_datao_1					= (UCHAR)(data >> 8);
	IO_DISP.HDMI_TX.phy_i2cm_datao_0					= (UCHAR)(data & 0x00FF);
	// Write operation.
	IO_DISP.HDMI_TX.phy_i2cm_operation.byte[0]			= 0x10;
	
	// Wait for a done interruption from the I2C master.
	if( DDIM_User_Twai_Flg(FID_IM_HDMI, D_IM_HDMI_INT_FLG_I2CMPHY, D_DDIM_USER_TWF_ORW, &flgptn, D_DDIM_WAIT_END_TIME) != D_DDIM_USER_E_OK) {
		return D_IM_HDMI_TIMEOUT;
	}
	
	return D_DDIM_OK;
}

/**
 * @brief	PHY Read.
 * @param[in]	address		PHY Register Address (Offset).
 * @param[out]	data		Read data.
 * @retval	D_DDIM_OK						Success.
 * @retval	D_IM_HDMI_TIMEOUT				timeout.
 */
static INT32 im_hdmi_phy_read(UCHAR address, USHORT *data)
{
	DDIM_USER_FLGPTN flgptn = 0;
	
	// clear Interrupt flag.
	DDIM_User_Clr_Flg(FID_IM_HDMI, ~(D_IM_HDMI_INT_FLG_I2CMPHY));
	
	IO_DISP.HDMI_TX.phy_i2cm_div.bit.fast_std_mode		= 0; // 0:Standard Mode

	// Set the PHY slave address.
	IO_DISP.HDMI_TX.phy_i2cm_slave.byte[0]				= 0x69;
	// Read register address.
	IO_DISP.HDMI_TX.phy_i2cm_address					= address;
	// Read operation.
	IO_DISP.HDMI_TX.phy_i2cm_operation.byte[0]			= 0x01;
	
	// Wait for a done interruption from the I2C master.
	if( DDIM_User_Twai_Flg(FID_IM_HDMI, D_IM_HDMI_INT_FLG_I2CMPHY, D_DDIM_USER_TWF_ORW, &flgptn, D_DDIM_WAIT_END_TIME) != D_DDIM_USER_E_OK) {
		return D_IM_HDMI_TIMEOUT;
	}
	
	// Read data.
	*data = (IO_DISP.HDMI_TX.phy_i2cm_datai_1 << 8) | IO_DISP.HDMI_TX.phy_i2cm_datai_0;
	
	return D_DDIM_OK;
}

/**
 * @brief	Configure the PLL.
 * @param[in]	pll_config		configure paramter.
 * @retval	D_DDIM_OK						Success.
 * @retval	D_IM_HDMI_NG					NG.
 * @retval	D_IM_HDMI_INPUT_PARAM_ERROR		Input parameter error.
 * @retval	D_IM_HDMI_TIMEOUT				timeout.
 */
static INT32 im_hdmi_configure_pll(T_IM_HDMI_PLL_CONFIG const *const pll_config)
{
	INT32 ret = D_DDIM_OK;
	DDIM_USER_FLGPTN flgptn = 0;
	int loop;
	
#ifdef CO_PARAM_CHECK
	if (pll_config == NULL) {
		Ddim_Assertion(("im_hdmi_configure_pll Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
	if (pll_config->phy_reg_num > D_IM_HDMI_PHY_REG_NUM) {
		Ddim_Assertion(("im_hdmi_configure_pll phy_reg_num Max size over\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	
	// Power-on, configure the PLL, and reset the HDMI TX PHY.
	// Configure a HDMI TX PHY.
	IO_DISP.HDMI_TX.phy_conf0.byte[0]						= 0x36;		// PHY SVSRET, PHY PDDQ, PHY ENHPDRXSENSE signal.
	
	// Reset.
	IO_DISP.HDMI_TX.mc_phyrstz.byte[0]						= D_IM_HDMI_PHYRSTZ_RESET;
	
	// Video Packetizer Pixel Repetition and Color Depth.
	IO_DISP.HDMI_TX.vp_pr_cd.byte[0]						= pll_config->vp_pr_cd.byte;

	// wait 450ns.
	im_hdmi_wait_ns(450);
	
	// Reset cancel.
	IO_DISP.HDMI_TX.mc_phyrstz.byte[0]						= D_IM_HDMI_PHYRSTZ_CANCEL;
	
	// PHY Register setting.
	for (loop = 0; loop < pll_config->phy_reg_num; loop++) {
		ret = im_hdmi_phy_write(pll_config->phy_reg_addr[loop], pll_config->phy_reg_data[loop]);
		if (ret != D_DDIM_OK) {
			return ret;
		}
	}
	
	// set power-on mode
	IO_DISP.HDMI_TX.phy_conf0.byte[0]						= 0x2E;		// PHY SVSRET, PHY TXPWRON, PHY ENHPDRXSENSE signal.
	
	// clear Interrupt flag.
	DDIM_User_Clr_Flg(FID_IM_HDMI, ~(D_IM_HDMI_INT_FLG_PHY_LOCK));

	// check PHY lock
	// Wait for a done interruption from the PHY.
	if( DDIM_User_Twai_Flg(FID_IM_HDMI, D_IM_HDMI_INT_FLG_PHY_LOCK, D_DDIM_USER_TWF_ORW, &flgptn, D_DDIM_WAIT_END_TIME) != D_DDIM_USER_E_OK) {
		return D_IM_HDMI_TIMEOUT;
	}
	
	return ret;
}

/**
 * @brief	Configure Infoframes.
 * @param[in]	info_frames		configure paramter.
 * @retval		D_DDIM_OK						Success.
 * @retval		D_IM_HDMI_NG					NG.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR		Input parameter error.
 */
static INT32 im_hdmi_configure_infoframes(T_IM_HDMI_INFO_FRAMES const *const info_frames)
{
	INT32 ret = D_DDIM_OK;
	int index;
#ifdef CO_PARAM_CHECK
	if (info_frames == NULL) {
		Ddim_Assertion(("im_hdmi_configure_infoframes Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	
	// AVI InfoFrame.
	IO_DISP.HDMI_TX.fc_aviconf0.byte[0]				= info_frames->fc_aviconf0.byte;
	IO_DISP.HDMI_TX.fc_aviconf1.byte[0]				= info_frames->fc_aviconf1.byte;
	IO_DISP.HDMI_TX.fc_aviconf2.byte[0]				= info_frames->fc_aviconf2.byte;
	IO_DISP.HDMI_TX.fc_aviconf3.byte[0]				= info_frames->fc_aviconf3.byte;
	IO_DISP.HDMI_TX.fc_avivid.byte[0]				= info_frames->fc_avivid;
	IO_DISP.HDMI_TX.fc_prconf.byte[0]				= info_frames->fc_prconf.byte;

	IO_DISP.HDMI_TX.fc_scrambler_ctrl.bit.scrambler_on	= info_frames->scrambler_on;

	// HDMI Vendor-Specific Infoframe.
	IO_DISP.HDMI_TX.fc_vsdieeeid0					= info_frames->fc_vsdieeeid[0];
	IO_DISP.HDMI_TX.fc_vsdieeeid1					= info_frames->fc_vsdieeeid[1];
	IO_DISP.HDMI_TX.fc_vsdieeeid2					= info_frames->fc_vsdieeeid[2];

	// Video Mode (HDMI or 3D)
	IO_DISP.HDMI_TX.fc_vsdpayload[0].byte[0]		= info_frames->video_mode;

	if (info_frames->video_mode == D_IM_HDMI_VIDEO_MODE_HDMI) {
		// HDMI Video Mode.
		IO_DISP.HDMI_TX.fc_vsdpayload[1].byte[0]	= info_frames->video_format;
	}
	else if (info_frames->video_mode == D_IM_HDMI_VIDEO_MODE_3D) {
		// 3D Video Mode.
		IO_DISP.HDMI_TX.fc_vsdpayload[1].byte[0]	= (info_frames->video_format << 4) | (info_frames->meta_present << 3);
		IO_DISP.HDMI_TX.fc_vsdpayload[2].byte[0]	= info_frames->additional_video_format << 4;
		IO_DISP.HDMI_TX.fc_vsdpayload[3].byte[0]	= (info_frames->meta_type << 5) | info_frames->meta_length;
		for (index = 0; index < 8; index++) {
			IO_DISP.HDMI_TX.fc_vsdpayload[4 + index].byte[0]	= info_frames->meta_data[index];
		}
	}
	else {
		// input paramter NG.
		ret = D_IM_HDMI_INPUT_PARAM_ERROR;
	}

	// Enables SPD automatic packet scheduling
	IO_DISP.HDMI_TX.fc_datauto0.bit.spd_auto		= 1;
	// Enables VSD automatic packet scheduling
	IO_DISP.HDMI_TX.fc_datauto0.bit.vsd_auto		= 1;
	// auto_frame_packets
	IO_DISP.HDMI_TX.fc_datauto2.bit.auto_frame_packets = 1;

	IO_DISP.HDMI_TX.fc_datauto0.bit.acp_auto		= 0;
	IO_DISP.HDMI_TX.fc_datauto0.bit.iscr1_auto		= 0;
	IO_DISP.HDMI_TX.fc_datauto0.bit.iscr2_auto		= 0;
	
	return ret;
}

/**
 * @brief	Get interrupt type.
 * @param[in]	int_reg		Interrupt register.
 * @param[in]	reg_bit		Bit of the specified interrupt register.
 * @retval		E_IM_HDMI_INT_TYPE		Interrupt type.
 */
E_IM_HDMI_INT_TYPE im_hdmi_get_interrupt_type(E_IM_HDMI_INT_REG int_reg, UCHAR reg_bit)
{
	E_IM_HDMI_INT_TYPE interrupt_type = E_IM_HDMI_INT_TYPE_MAX;
	
	// Interrupt register.
	switch (int_reg) {
		case E_IM_HDMI_INT_REG_FC_STAT0:	// ih_fc_stat0.
			// register bit.
			switch (reg_bit) {
				case E_IM_HDMI_IH_FC_STAT0_NULL:	// NULL.
					interrupt_type = E_IM_HDMI_INT_TYPE_FC_NULL;
					break;
				case E_IM_HDMI_IH_FC_STAT0_ACR:		// ACR.
					interrupt_type = E_IM_HDMI_INT_TYPE_FC_ACR;
					break;
				case E_IM_HDMI_IH_FC_STAT0_AUDS:	// AUDS.
					interrupt_type = E_IM_HDMI_INT_TYPE_FC_AUDS;
					break;
				case E_IM_HDMI_IH_FC_STAT0_NVBI:	// NVBI.
					interrupt_type = E_IM_HDMI_INT_TYPE_FC_NVBI;
					break;
				case E_IM_HDMI_IH_FC_STAT0_MAS:		// MAS.
					interrupt_type = E_IM_HDMI_INT_TYPE_FC_MAS;
					break;
				case E_IM_HDMI_IH_FC_STAT0_HBR:		// HBR.
					interrupt_type = E_IM_HDMI_INT_TYPE_FC_HBR;
					break;
				case E_IM_HDMI_IH_FC_STAT0_ACP:		// ACP.
					interrupt_type = E_IM_HDMI_INT_TYPE_FC_ACP;
					break;
				case E_IM_HDMI_IH_FC_STAT0_AUDI:	// AUDI.
					interrupt_type = E_IM_HDMI_INT_TYPE_FC_AUDI;
					break;
				default:
					// no process.
					break;
			}
			break;

		case E_IM_HDMI_INT_REG_FC_STAT1:	// ih_fc_stat1.
			// register bit.
			switch (reg_bit) {
				case E_IM_HDMI_IH_FC_STAT1_GCP:		// GCP.
					interrupt_type = E_IM_HDMI_INT_TYPE_FC_GCP;
					break;
				case E_IM_HDMI_IH_FC_STAT1_AVI:		// AVI.
					interrupt_type = E_IM_HDMI_INT_TYPE_FC_AVI;
					break;
				case E_IM_HDMI_IH_FC_STAT1_AMP:		// AMP.
					interrupt_type = E_IM_HDMI_INT_TYPE_FC_AMP;
					break;
				case E_IM_HDMI_IH_FC_STAT1_SPD:		// SPD.
					interrupt_type = E_IM_HDMI_INT_TYPE_FC_SPD;
					break;
				case E_IM_HDMI_IH_FC_STAT1_VSD:		// VSD.
					interrupt_type = E_IM_HDMI_INT_TYPE_FC_VSD;
					break;
				case E_IM_HDMI_IH_FC_STAT1_ISCR2:	// ISCR2.
					interrupt_type = E_IM_HDMI_INT_TYPE_FC_ISCR2;
					break;
				case E_IM_HDMI_IH_FC_STAT1_ISCR1:	// ISCR1.
					interrupt_type = E_IM_HDMI_INT_TYPE_FC_ISCR1;
					break;
				case E_IM_HDMI_IH_FC_STAT1_GMD:		// GMD.
					interrupt_type = E_IM_HDMI_INT_TYPE_FC_GMD;
					break;
				default:
					// no process.
					break;
			}
			break;

		case E_IM_HDMI_INT_REG_FC_STAT2:	// ih_fc_stat2.
			// register bit.
			switch (reg_bit) {
				case E_IM_HDMI_IH_FC_STAT2_HP_OF:	// HighPriority_overflow.
					interrupt_type = E_IM_HDMI_INT_TYPE_FC_HP_OF;
					break;
				case E_IM_HDMI_IH_FC_STAT2_LP_OF:	// LowPriority_overflow.
					interrupt_type = E_IM_HDMI_INT_TYPE_FC_LP_OF;
					break;
				default:
					// no process.
					break;
			}
			break;

		case E_IM_HDMI_INT_REG_AS_STAT0:	// ih_as_stat0.
			// register bit.
			switch (reg_bit) {
				case E_IM_HDMI_IH_AS_STAT0_OF:	// Aud_fifo_overflow.
					interrupt_type = E_IM_HDMI_INT_TYPE_AS_OF;
					break;
				case E_IM_HDMI_IH_AS_STAT0_UF:	// Aud_fifo_underflow.
					interrupt_type = E_IM_HDMI_INT_TYPE_AS_UF;
					break;
				case E_IM_HDMI_IH_AS_STAT0_OR:		// fifo_overrun.
					interrupt_type = E_IM_HDMI_INT_TYPE_AS_OR;
					break;
				default:
					// no process.
					break;
			}
			break;

		case E_IM_HDMI_INT_REG_PHY_STAT0:	// ih_phy_stat0.
			// register bit.
			switch (reg_bit) {
				case E_IM_HDMI_IH_PHY_STAT0_HDP:			// HDP.
					interrupt_type = E_IM_HDMI_INT_TYPE_PHY_HDP;
					break;
				case E_IM_HDMI_IH_PHY_STAT0_TX_PHY_LOCK:	// TX_PHY_LOCK.
					interrupt_type = E_IM_HDMI_INT_TYPE_PHY_TX_PHY_LOCK;
					break;
				case E_IM_HDMI_IH_PHY_STAT0_RX_SENSE_0:		// RX_SENSE_0.
					interrupt_type = E_IM_HDMI_INT_TYPE_PHY_RX_SENSE_0;
					break;
				case E_IM_HDMI_IH_PHY_STAT0_RX_SENSE_1:		// RX_SENSE_1.
					interrupt_type = E_IM_HDMI_INT_TYPE_PHY_RX_SENSE_1;
					break;
				case E_IM_HDMI_IH_PHY_STAT0_RX_SENSE_2:		// RX_SENSE_2.
					interrupt_type = E_IM_HDMI_INT_TYPE_PHY_RX_SENSE_2;
					break;
				case E_IM_HDMI_IH_PHY_STAT0_RX_SENSE_3:		// RX_SENSE_3.
					interrupt_type = E_IM_HDMI_INT_TYPE_PHY_RX_SENSE_3;
					break;
				default:
					// no process.
					break;
			}
			break;

		case E_IM_HDMI_INT_REG_I2CM_STAT0:	// ih_i2cm_stat0.
			// register bit.
			switch (reg_bit) {
				case E_IM_HDMI_IH_I2CM_STAT0_MASTER_ERR:	// I2Cmastererror.
					interrupt_type = E_IM_HDMI_INT_TYPE_I2CM_M_ERR;
					break;
				case E_IM_HDMI_IH_I2CM_STAT0_MASTER_DONE:	// I2Cmasterdone.
					interrupt_type = E_IM_HDMI_INT_TYPE_I2CM_M_DONE;
					break;
				case E_IM_HDMI_IH_I2CM_STAT0_SDSC:			// scdc_readreq.
					interrupt_type = E_IM_HDMI_INT_TYPE_I2CM_SDSC;
					break;
				default:
					// no process.
					break;
			}
			break;

		case E_IM_HDMI_INT_REG_CEC_STAT0:	// ih_cec_stat0.
			// register bit.
			switch (reg_bit) {
				case E_IM_HDMI_IH_CEC_STAT0_DONE:			// DONE.
					interrupt_type = E_IM_HDMI_INT_TYPE_CEC_DONE;
					break;
				case E_IM_HDMI_IH_CEC_STAT0_EOM:			// EOM.
					interrupt_type = E_IM_HDMI_INT_TYPE_CEC_EOM;
					break;
				case E_IM_HDMI_IH_CEC_STAT0_NACK:			// NACK.
					interrupt_type = E_IM_HDMI_INT_TYPE_CEC_NACK;
					break;
				case E_IM_HDMI_IH_CEC_STAT0_ARB_LOST:		// ARB_LOST.
					interrupt_type = E_IM_HDMI_INT_TYPE_CEC_ARB_LOST;
					break;
				case E_IM_HDMI_IH_CEC_STAT0_ERR_INITIATOR:	// ERROR_INITIATOR.
					interrupt_type = E_IM_HDMI_INT_TYPE_CEC_ERR_INIT;
					break;
				case E_IM_HDMI_IH_CEC_STAT0_ERR_FOLLOW:		// ERROR_FOLLOW.
					interrupt_type = E_IM_HDMI_INT_TYPE_CEC_ERR_FOL;
					break;
				case E_IM_HDMI_IH_CEC_STAT0_WAKEUP:			// WAKEUP.
					interrupt_type = E_IM_HDMI_INT_TYPE_CEC_WAKEUP;
					break;
				default:
					// no process.
					break;
			}
			break;

		case E_IM_HDMI_INT_REG_VP_STAT0:	// ih_vp_stat0.
			// register bit.
			switch (reg_bit) {
				case E_IM_HDMI_IH_VP_STAT0_EMPTY_BYP:		// fifoemptybyp.
					interrupt_type = E_IM_HDMI_INT_TYPE_VP_E_BYP;
					break;
				case E_IM_HDMI_IH_VP_STAT0_FULL_BYP:		// fifofullbyp.
					interrupt_type = E_IM_HDMI_INT_TYPE_VP_F_BYP;
					break;
				case E_IM_HDMI_IH_VP_STAT0_EMPTY_REMAP:		// fifoemptyremap.
					interrupt_type = E_IM_HDMI_INT_TYPE_VP_E_REMAP;
					break;
				case E_IM_HDMI_IH_VP_STAT0_FULL_REMAP:		// fifofullremap.
					interrupt_type = E_IM_HDMI_INT_TYPE_VP_F_REMAP;
					break;
				case E_IM_HDMI_IH_VP_STAT0_EMPTY_PP:		// fifoemptypp.
					interrupt_type = E_IM_HDMI_INT_TYPE_VP_E_PP;
					break;
				case E_IM_HDMI_IH_VP_STAT0_FULL_PP:			// fifofullpp.
					interrupt_type = E_IM_HDMI_INT_TYPE_VP_F_PP;
					break;
				case E_IM_HDMI_IH_VP_STAT0_EMPTY_REPET:		// fifoemptyrepet.
					interrupt_type = E_IM_HDMI_INT_TYPE_VP_E_REPET;
					break;
				case E_IM_HDMI_IH_VP_STAT0_FULL_REPET:		// fifofullrepet.
					interrupt_type = E_IM_HDMI_INT_TYPE_VP_F_REPET;
					break;
				default:
					// no process.
					break;
			}
			break;

		default :							// ih_i2cmphy_stat0.
			// register bit.
			switch (reg_bit) {
				case E_IM_HDMI_IH_I2CMPHY_STAT0_MASTER_ERR:		// I2Cmphyerror.
					interrupt_type = E_IM_HDMI_INT_TYPE_I2CMPHY_ERR;
					break;
				case E_IM_HDMI_IH_I2CMPHY_STAT0_MASTER_DONE:	// I2Cmphydone.
					interrupt_type = E_IM_HDMI_INT_TYPE_I2CMPHY_DONE;
					break;
				default:
					// no process.
					break;
			}
			break;
	}
	
	return interrupt_type;
}

/**
 * @brief	Interrupt check (ih_fc_stat0/ih_fc_stat1/ih_fc_stat2).
 * @param	VOID.
 * @return	VOID.
 */
static VOID im_hdmi_check_interrupt_fc(VOID)
{
	INT32 loop;
	E_IM_HDMI_INT_TYPE interrupt_type = E_IM_HDMI_INT_TYPE_MAX;
	
	// ih_fc_stat0 register bit.
	E_IM_HDMI_IH_FC_STAT0 fc_stat0_bit[] = {
		E_IM_HDMI_IH_FC_STAT0_NULL,
		E_IM_HDMI_IH_FC_STAT0_ACR,
		E_IM_HDMI_IH_FC_STAT0_AUDS,
		E_IM_HDMI_IH_FC_STAT0_NVBI,
		E_IM_HDMI_IH_FC_STAT0_MAS,
		E_IM_HDMI_IH_FC_STAT0_HBR,
		E_IM_HDMI_IH_FC_STAT0_ACP,
		E_IM_HDMI_IH_FC_STAT0_AUDI
	};
	// ih_fc_stat1 register bit.
	E_IM_HDMI_IH_FC_STAT1 fc_stat1_bit[] = {
		E_IM_HDMI_IH_FC_STAT1_GCP,
		E_IM_HDMI_IH_FC_STAT1_AVI,
		E_IM_HDMI_IH_FC_STAT1_AMP,
		E_IM_HDMI_IH_FC_STAT1_SPD,
		E_IM_HDMI_IH_FC_STAT1_VSD,
		E_IM_HDMI_IH_FC_STAT1_ISCR2,
		E_IM_HDMI_IH_FC_STAT1_ISCR1,
		E_IM_HDMI_IH_FC_STAT1_GMD
	};
	// ih_fc_stat2 register bit.
	E_IM_HDMI_IH_FC_STAT2 fc_stat2_bit[] = {
		E_IM_HDMI_IH_FC_STAT2_HP_OF,
		E_IM_HDMI_IH_FC_STAT2_LP_OF
	};
	
	// Frame Composer Interrupt Status Register 0.
	for (loop = 0; loop < sizeof(fc_stat0_bit)/sizeof(fc_stat0_bit[0]); loop++) {
		// check register bit.
		if ((IO_DISP.HDMI_TX.ih_fc_stat0.byte[0] & fc_stat0_bit[loop]) == fc_stat0_bit[loop]) {
			
			// Get Interrupt type.
			interrupt_type = im_hdmi_get_interrupt_type(E_IM_HDMI_INT_REG_FC_STAT0, (UCHAR)fc_stat0_bit[loop]);
			// Illegal type(fail safe).
			if (interrupt_type == E_IM_HDMI_INT_TYPE_MAX) {
				continue;
			}
			
			// Interrupt bit clear.
			IO_DISP.HDMI_TX.ih_fc_stat0.byte[0] = fc_stat0_bit[loop];

			// callback.
			if (gIM_HDMI_INT_Callback != NULL) {
				gIM_HDMI_INT_Callback((UCHAR)interrupt_type);
			}
		}
	}
	
	// Frame Composer Interrupt Status Register 1.
	for (loop = 0; loop < sizeof(fc_stat1_bit)/sizeof(fc_stat1_bit[0]); loop++) {
		// check register bit.
		if ((IO_DISP.HDMI_TX.ih_fc_stat1.byte[0] & fc_stat1_bit[loop]) == fc_stat1_bit[loop]) {
			
			// Get Interrupt type.
			interrupt_type = im_hdmi_get_interrupt_type(E_IM_HDMI_INT_REG_FC_STAT1, (UCHAR)fc_stat1_bit[loop]);
			// Illegal type(fail safe).
			if (interrupt_type == E_IM_HDMI_INT_TYPE_MAX) {
				continue;
			}
			
			// Interrupt bit clear.
			IO_DISP.HDMI_TX.ih_fc_stat1.byte[0] = fc_stat1_bit[loop];

			// callback.
			if (gIM_HDMI_INT_Callback != NULL) {
				gIM_HDMI_INT_Callback((UCHAR)interrupt_type);
			}
		}
	}
	
	// Frame Composer Interrupt Status Register 2.
	for (loop = 0; loop < sizeof(fc_stat2_bit)/sizeof(fc_stat2_bit[0]); loop++) {
		// check register bit.
		if ((IO_DISP.HDMI_TX.ih_fc_stat2.byte[0] & fc_stat2_bit[loop]) == fc_stat2_bit[loop]) {
			
			// Get Interrupt type.
			interrupt_type = im_hdmi_get_interrupt_type(E_IM_HDMI_INT_REG_FC_STAT2, (UCHAR)fc_stat2_bit[loop]);
			// Illegal type(fail safe).
			if (interrupt_type == E_IM_HDMI_INT_TYPE_MAX) {
				continue;
			}
			
			// Interrupt bit clear.
			IO_DISP.HDMI_TX.ih_fc_stat2.byte[0] = fc_stat2_bit[loop];
			
			// callback.
			if (gIM_HDMI_INT_Callback != NULL) {
				gIM_HDMI_INT_Callback((UCHAR)interrupt_type);
			}
		}
	}
}

/**
 * @brief	Interrupt check (ih_as_stat0).
 * @param	VOID.
 * @return	VOID.
 */
static VOID im_hdmi_check_interrupt_as(VOID)
{
	INT32 loop;
	E_IM_HDMI_INT_TYPE interrupt_type = E_IM_HDMI_INT_TYPE_MAX;
	
	// ih_as_stat0 register bit.
	E_IM_HDMI_IH_AS_STAT0 as_stat0_bit[] = {
		E_IM_HDMI_IH_AS_STAT0_OF,
		E_IM_HDMI_IH_AS_STAT0_UF,
		E_IM_HDMI_IH_AS_STAT0_OR
	};
	
	// Audio Sampler Interrupt Status Register.
	for (loop = 0; loop < sizeof(as_stat0_bit)/sizeof(as_stat0_bit[0]); loop++) {
		// check register bit.
		if ((IO_DISP.HDMI_TX.ih_as_stat0.byte[0] & as_stat0_bit[loop]) == as_stat0_bit[loop]) {
			
			// Get Interrupt type.
			interrupt_type = im_hdmi_get_interrupt_type(E_IM_HDMI_INT_REG_AS_STAT0, (UCHAR)as_stat0_bit[loop]);
			// Illegal type(fail safe).
			if (interrupt_type == E_IM_HDMI_INT_TYPE_MAX) {
				continue;
			}
			
			// Interrupt bit clear.
			IO_DISP.HDMI_TX.ih_as_stat0.byte[0] = as_stat0_bit[loop];
			
			// callback.
			if (gIM_HDMI_INT_Callback != NULL) {
				gIM_HDMI_INT_Callback((UCHAR)interrupt_type);
			}
		}
	}
}

/**
 * @brief	Interrupt check (ih_phy_stat0).
 * @param	VOID.
 * @return	VOID.
 */
static VOID im_hdmi_check_interrupt_phy(VOID)
{
	INT32 loop;
	E_IM_HDMI_INT_TYPE interrupt_type = E_IM_HDMI_INT_TYPE_MAX;
	
	// ih_phy_stat0 register bit.
	E_IM_HDMI_IH_PHY_STAT0 phy_stat0_bit[] = {
		E_IM_HDMI_IH_PHY_STAT0_HDP,
		E_IM_HDMI_IH_PHY_STAT0_TX_PHY_LOCK,
		E_IM_HDMI_IH_PHY_STAT0_RX_SENSE_0,
		E_IM_HDMI_IH_PHY_STAT0_RX_SENSE_1,
		E_IM_HDMI_IH_PHY_STAT0_RX_SENSE_2,
		E_IM_HDMI_IH_PHY_STAT0_RX_SENSE_3
	};
	
	// PHY Interface Interrupt Status Register.
	for (loop = 0; loop < sizeof(phy_stat0_bit)/sizeof(phy_stat0_bit[0]); loop++) {
		// check register bit.
		if ((IO_DISP.HDMI_TX.ih_phy_stat0.byte[0] & phy_stat0_bit[loop]) == phy_stat0_bit[loop]) {
			
			// Get Interrupt type.
			interrupt_type = im_hdmi_get_interrupt_type(E_IM_HDMI_INT_REG_PHY_STAT0, (UCHAR)phy_stat0_bit[loop]);
			// Illegal type(fail safe).
			if (interrupt_type == E_IM_HDMI_INT_TYPE_MAX) {
				continue;
			}
			
			//  bit clear.
			if(phy_stat0_bit[loop] == E_IM_HDMI_IH_PHY_STAT0_HDP){
				IO_DISP.HDMI_TX.phy_pol0.bit.HPD = ~IO_DISP.HDMI_TX.phy_pol0.bit.HPD;
			}else if(phy_stat0_bit[loop] == E_IM_HDMI_IH_PHY_STAT0_TX_PHY_LOCK){
				IO_DISP.HDMI_TX.phy_pol0.bit.TX_PHY_LOCK = ~IO_DISP.HDMI_TX.phy_pol0.bit.TX_PHY_LOCK;
			}else if(phy_stat0_bit[loop] == E_IM_HDMI_IH_PHY_STAT0_RX_SENSE_0){
				IO_DISP.HDMI_TX.phy_pol0.bit.RX_SENSE_0 = ~IO_DISP.HDMI_TX.phy_pol0.bit.RX_SENSE_0;
			}else if(phy_stat0_bit[loop] == E_IM_HDMI_IH_PHY_STAT0_RX_SENSE_1){
				IO_DISP.HDMI_TX.phy_pol0.bit.RX_SENSE_1 = ~IO_DISP.HDMI_TX.phy_pol0.bit.RX_SENSE_1;
			}else if(phy_stat0_bit[loop] == E_IM_HDMI_IH_PHY_STAT0_RX_SENSE_2){
				IO_DISP.HDMI_TX.phy_pol0.bit.RX_SENSE_2 = ~IO_DISP.HDMI_TX.phy_pol0.bit.RX_SENSE_2;
			}else if(phy_stat0_bit[loop] == E_IM_HDMI_IH_PHY_STAT0_RX_SENSE_3){
				IO_DISP.HDMI_TX.phy_pol0.bit.RX_SENSE_3 = ~IO_DISP.HDMI_TX.phy_pol0.bit.RX_SENSE_3;
			}
			// Interrupt bit clear.
			IO_DISP.HDMI_TX.ih_phy_stat0.byte[0] = phy_stat0_bit[loop];
			
			// Interrupt type : ih_phy_stat0.TX_PHY_LOCK
			if (interrupt_type == E_IM_HDMI_INT_TYPE_PHY_TX_PHY_LOCK) {
				im_hdmi_callback_interrupt_phy_lock();
			}
			
			// callback.
			if (gIM_HDMI_INT_Callback != NULL) {
				gIM_HDMI_INT_Callback((UCHAR)interrupt_type);
			}
		}
	}
}

/**
 * @brief	Interrupt check (ih_i2cm_stat0).
 * @param	VOID.
 * @return	VOID.
 */
static VOID im_hdmi_check_interrupt_i2cm(VOID)
{
	INT32 loop;
	E_IM_HDMI_INT_TYPE interrupt_type = E_IM_HDMI_INT_TYPE_MAX;
	
	// ih_i2cm_stat0 register bit.
	E_IM_HDMI_IH_I2CM_STAT0 i2cm_stat0_bit[] = {
		E_IM_HDMI_IH_I2CM_STAT0_MASTER_ERR,
		E_IM_HDMI_IH_I2CM_STAT0_MASTER_DONE,
		E_IM_HDMI_IH_I2CM_STAT0_SDSC
	};
	
	// E-DDC I2C Master Interrupt Status Register.
	for (loop = 0; loop < sizeof(i2cm_stat0_bit)/sizeof(i2cm_stat0_bit[0]); loop++) {
		// check register bit.
		if ((IO_DISP.HDMI_TX.ih_i2cm_stat0.byte[0] & i2cm_stat0_bit[loop]) == i2cm_stat0_bit[loop]) {
			// Get Interrupt type.
			interrupt_type = im_hdmi_get_interrupt_type(E_IM_HDMI_INT_REG_I2CM_STAT0, (UCHAR)i2cm_stat0_bit[loop]);
			// Illegal type(fail safe).
			if (interrupt_type == E_IM_HDMI_INT_TYPE_MAX) {
				continue;
			}
			
			// Interrupt bit clear.
			IO_DISP.HDMI_TX.ih_i2cm_stat0.byte[0] = i2cm_stat0_bit[loop];
			
			// Interrupt type : ih_i2cm_stat0 Register.I2Cmasterdone
			if (interrupt_type == E_IM_HDMI_INT_TYPE_I2CM_M_DONE) {
				im_hdmi_callback_interrupt_i2cm();
			}
			
			// callback.
			if (gIM_HDMI_INT_Callback != NULL) {
				gIM_HDMI_INT_Callback((UCHAR)interrupt_type);
			}
		}
	}
}

/**
 * @brief	Interrupt check (ih_cec_stat0).
 * @param	VOID.
 * @return	VOID.
 */
static VOID im_hdmi_check_interrupt_cec(VOID)
{
	INT32 loop;
	E_IM_HDMI_INT_TYPE interrupt_type = E_IM_HDMI_INT_TYPE_MAX;
	
	// ih_cec_stat0 register bit.
	E_IM_HDMI_IH_CEC_STAT0 cec_stat0_bit[] = {
		E_IM_HDMI_IH_CEC_STAT0_DONE,
		E_IM_HDMI_IH_CEC_STAT0_EOM,
		E_IM_HDMI_IH_CEC_STAT0_NACK,
		E_IM_HDMI_IH_CEC_STAT0_ARB_LOST,
		E_IM_HDMI_IH_CEC_STAT0_ERR_INITIATOR,
		E_IM_HDMI_IH_CEC_STAT0_ERR_FOLLOW,
		E_IM_HDMI_IH_CEC_STAT0_WAKEUP
	};
	
	// CEC Interrupt Status Register.
	for (loop = 0; loop < sizeof(cec_stat0_bit)/sizeof(cec_stat0_bit[0]); loop++) {
		// check register bit.
		if ((IO_DISP.HDMI_TX.ih_cec_stat0.byte[0] & cec_stat0_bit[loop]) == cec_stat0_bit[loop]) {
			// Get Interrupt type.
			interrupt_type = im_hdmi_get_interrupt_type(E_IM_HDMI_INT_REG_CEC_STAT0, (UCHAR)cec_stat0_bit[loop]);
			// Illegal type(fail safe).
			if (interrupt_type == E_IM_HDMI_INT_TYPE_MAX) {
				continue;
			}
			
			// Interrupt bit clear.
			IO_DISP.HDMI_TX.ih_cec_stat0.byte[0] = cec_stat0_bit[loop];
			
			// callback.
			if (gIM_HDMI_INT_Callback != NULL) {
				gIM_HDMI_INT_Callback((UCHAR)interrupt_type);
			}
		}
	}
}

/**
 * @brief	Interrupt check (ih_vp_stat0).
 * @param	VOID.
 * @return	VOID.
 */
static VOID im_hdmi_check_interrupt_vp(VOID)
{
	INT32 loop;
	E_IM_HDMI_INT_TYPE interrupt_type = E_IM_HDMI_INT_TYPE_MAX;
	
	// ih_vp_stat0 register bit.
	E_IM_HDMI_IH_VP_STAT0 vp_stat0_bit[] = {
		E_IM_HDMI_IH_VP_STAT0_EMPTY_BYP,
		E_IM_HDMI_IH_VP_STAT0_FULL_BYP,
		E_IM_HDMI_IH_VP_STAT0_EMPTY_REMAP,
		E_IM_HDMI_IH_VP_STAT0_FULL_REMAP,
		E_IM_HDMI_IH_VP_STAT0_EMPTY_PP,
		E_IM_HDMI_IH_VP_STAT0_FULL_PP,
		E_IM_HDMI_IH_VP_STAT0_EMPTY_REPET,
		E_IM_HDMI_IH_VP_STAT0_FULL_REPET
	};
	
	// Video Packetizer Interrupt Status Register.
	for (loop = 0; loop < sizeof(vp_stat0_bit)/sizeof(vp_stat0_bit[0]); loop++) {
		// check register bit.
		if ((IO_DISP.HDMI_TX.ih_vp_stat0.byte[0] & vp_stat0_bit[loop]) == vp_stat0_bit[loop]) {
			// Get Interrupt type.
			interrupt_type = im_hdmi_get_interrupt_type(E_IM_HDMI_INT_REG_VP_STAT0, (UCHAR)vp_stat0_bit[loop]);
			// Illegal type(fail safe).
			if (interrupt_type == E_IM_HDMI_INT_TYPE_MAX) {
				continue;
			}
			
			// Interrupt bit clear.
			IO_DISP.HDMI_TX.ih_vp_stat0.byte[0] = vp_stat0_bit[loop];
			
			// callback.
			if (gIM_HDMI_INT_Callback != NULL) {
				gIM_HDMI_INT_Callback((UCHAR)interrupt_type);
			}
		}
	}
}

/**
 * @brief	Interrupt check (ih_i2cmphy_stat0).
 * @param	VOID.
 * @return	VOID.
 */
static VOID im_hdmi_check_interrupt_i2cmphy(VOID)
{
	INT32 loop;
	E_IM_HDMI_INT_TYPE interrupt_type = E_IM_HDMI_INT_TYPE_MAX;
	
	// ih_i2cmphy_stat0 register bit.
	E_IM_HDMI_IH_FC_STAT0 i2cmphy_stat0_bit[] = {
		E_IM_HDMI_IH_I2CMPHY_STAT0_MASTER_ERR,
		E_IM_HDMI_IH_I2CMPHY_STAT0_MASTER_DONE
	};
	
	// PHY GEN2 I2C Master Interrupt Status Register.
	for (loop = 0; loop < sizeof(i2cmphy_stat0_bit)/sizeof(i2cmphy_stat0_bit[0]); loop++) {
		// check register bit.
		if ((IO_DISP.HDMI_TX.ih_i2cmphy_stat0.byte[0] & i2cmphy_stat0_bit[loop]) == i2cmphy_stat0_bit[loop]) {
			// Get Interrupt type.
			interrupt_type = im_hdmi_get_interrupt_type(E_IM_HDMI_INT_REG_I2CMPHY_STAT0, (UCHAR)i2cmphy_stat0_bit[loop]);
			// Illegal type(fail safe).
			if (interrupt_type == E_IM_HDMI_INT_TYPE_MAX) {
				continue;
			}
			
			// Interrupt bit clear.
			IO_DISP.HDMI_TX.ih_i2cmphy_stat0.byte[0] = i2cmphy_stat0_bit[loop];
			
			// Interrupt type : ih_i2cmphy_stat0.I2Cmphydone
			if (interrupt_type == E_IM_HDMI_INT_TYPE_I2CMPHY_DONE) {
				im_hdmi_callback_interrupt_i2cmphy();
			}
			
			// callback.
			if (gIM_HDMI_INT_Callback != NULL) {
				gIM_HDMI_INT_Callback((UCHAR)interrupt_type);
			}
		}
	}
}

/*----------------------------------------------------------------------*/
/* Grobal Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief	HDMI PHY Power On/Off.
 */
INT32 Im_HDMI_Power_On(UCHAR power)
{
#ifdef CO_PARAM_CHECK
	if((power != D_IM_HDMI_ENABLE_OFF) && (power != D_IM_HDMI_ENABLE_ON)) {
		Ddim_Assertion(("Im_HDMI_Power_On Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	
	// PHY TXPWRON.
	IO_DISP.HDMI_TX.phy_conf0.bit.txpwron = power;
	
	return D_DDIM_OK;
}

/**
 * @brief	Initialize HDMI Transmitter macro.
 */
INT32 Im_HDMI_Init(VOID)
{
	im_hdmi_pclk_on();

	// status clear
	IO_DISP.HDMI_TX.ih_fc_stat0.byte[0]			= 0xff;
	IO_DISP.HDMI_TX.ih_fc_stat1.byte[0]			= 0xff;
	IO_DISP.HDMI_TX.ih_fc_stat2.byte[0]			= 0x03;
	IO_DISP.HDMI_TX.ih_as_stat0.byte[0]			= 0x0b;
	IO_DISP.HDMI_TX.ih_phy_stat0.byte[0]		= 0x3f;
	IO_DISP.HDMI_TX.ih_i2cm_stat0.byte[0]		= 0x07;
	IO_DISP.HDMI_TX.ih_cec_stat0.byte[0]		= 0x7f;
	IO_DISP.HDMI_TX.ih_vp_stat0.byte[0]			= 0xff;
	IO_DISP.HDMI_TX.ih_i2cmphy_stat0.byte[0]	= 0x03;

	// mute off
	IO_DISP.HDMI_TX.ih_mute.byte[0]				= 0x00;

	// mute on
	IO_DISP.HDMI_TX.ih_mute_fc_stat0.byte[0]	= 0xff;
	IO_DISP.HDMI_TX.ih_mute_fc_stat1.byte[0]	= 0xff;
	IO_DISP.HDMI_TX.ih_mute_fc_stat2.byte[0]	= 0x03;
	IO_DISP.HDMI_TX.ih_mute_as_stat0.byte[0]	= 0x0b;
//	IO_DISP.HDMI_TX.ih_mute_phy_stat0.byte[0]	= 0x3f;
	IO_DISP.HDMI_TX.ih_mute_cec_stat0.byte[0]	= 0x7f;
	IO_DISP.HDMI_TX.ih_mute_vp_stat0.byte[0]	= 0xff;
	// mask on
	IO_DISP.HDMI_TX.fc_mask0.byte[0]			= 0xff;
	IO_DISP.HDMI_TX.fc_mask1.byte[0]			= 0xff;
	IO_DISP.HDMI_TX.aud_int.byte[0]				= 0xff;
//	IO_DISP.HDMI_TX.phy_mask0.byte[0]			= 0xff;
	IO_DISP.HDMI_TX.cec_mask.byte[0]			= 0xff;
	IO_DISP.HDMI_TX.vp_mask.byte[0]				= 0xff;

	// Power-on the HDMI TX PHY HPD Detector.
	IO_DISP.HDMI_TX.phy_conf0.bit.enhpdrxsense = 1;
	
	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Configure VGA DVI Video Mode.
 */
INT32 Im_HDMI_Configure_VGA_DVI_Video_Mode(T_IM_HDMI_PLL_CONFIG const *const pll_config)
{
#ifdef CO_PARAM_CHECK
	if( pll_config == NULL ) {
		Ddim_Assertion(("Im_HDMI_Configure_VGA_DVI_Video_Mode Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif

	im_hdmi_pclk_on();
	
	// Clock Domain Disable
	IO_DISP.HDMI_TX.mc_clkdis.bit.pixelclk_disable		= 1;
	IO_DISP.HDMI_TX.mc_clkdis.bit.tmdsclk_disable		= 1;
	
	// Set VGA (640x480p) in DVI mode.
	// Select the Video Mapping input RGB444, 8-bits per color components (24-bits RGB).
	IO_DISP.HDMI_TX.tx_invid0.bit.video_mapping			= E_IM_HDMI_VIDEO_MAPPING_RGB444_8BIT;
	// Configure VGA timing information (CEA mode 1)
	IO_DISP.HDMI_TX.fc_invidconf.bit.vsync_in_polarity	= D_IM_HDMI_ACTIVE_LOW;
	IO_DISP.HDMI_TX.fc_invidconf.bit.hsync_in_polarity	= D_IM_HDMI_ACTIVE_LOW;
	IO_DISP.HDMI_TX.fc_invidconf.bit.de_in_polarity		= D_IM_HDMI_ACTIVE_HIGH;
	IO_DISP.HDMI_TX.fc_invidconf.bit.r_v_blank_in_osc	= D_IM_HDMI_ACTIVE_LOW;
	IO_DISP.HDMI_TX.fc_invidconf.bit.in_I_P				= D_IM_HDMI_IN_I_P_PROGRESSIVE;
	// 640 H active pixels: 0x280
	IO_DISP.HDMI_TX.fc_inhactiv0.bit.H_in_activ			= 0x80;
	IO_DISP.HDMI_TX.fc_inhactiv1.bit.H_in_activ			= 0x02;
	// 480 V active pixels: 0x1E0
	IO_DISP.HDMI_TX.fc_invactiv0.bit.V_in_activ			= 0xE0;
	IO_DISP.HDMI_TX.fc_invactiv1.bit.V_in_activ			= 0x01;
	// 160 H blanking pixels: 0xA0
	IO_DISP.HDMI_TX.fc_inhblank0.bit.H_in_blank			= 0xA0;
	IO_DISP.HDMI_TX.fc_inhblank1.bit.H_in_blank			= 0x00;
	// 45 V blanking pixels: 0x2D
	IO_DISP.HDMI_TX.fc_invblank							= 0x2D;
	// 16 Sync offset: 0x10
	IO_DISP.HDMI_TX.fc_hsyncindelay0.bit.H_in_delay		= 0x10;
	IO_DISP.HDMI_TX.fc_hsyncindelay1.bit.H_in_delay		= 0x00;
	// 10 Vsync offset: 0x0A
	IO_DISP.HDMI_TX.fc_vsyncindelay						= 0x0A;
	// 96 HSync pulse width: 0x60
	IO_DISP.HDMI_TX.fc_hsyncinwidth0.bit.H_in_width		= 0x60;
	IO_DISP.HDMI_TX.fc_hsyncinwidth1.bit.H_in_width		= 0x00;
	// 2 VSync pulse width: 0x02
	IO_DISP.HDMI_TX.fc_vsyncinwidth.bit.V_in_width		= 0x02;
	// To select the DVI mode
	IO_DISP.HDMI_TX.fc_invidconf.bit.DVI_modez			= D_IM_HDMI_DVI_MODEZ_DVI;
	
	// Configuring the PLL.
	if (im_hdmi_configure_pll(pll_config) != D_DDIM_OK) {
		// the PLL does not lock and no activity.
		im_hdmi_pclk_off();
		return D_IM_HDMI_NG;
	}
	
	// Enable the video path.
	// Set default parameters.
	IO_DISP.HDMI_TX.fc_ctrldur							= 0x0C;		// Control period duration.
	IO_DISP.HDMI_TX.fc_exctrldur						= 0x20;		// Extended Control period duration.
	IO_DISP.HDMI_TX.fc_exctrlspac						= 0x01;		// Max spacing between extended Control period duration.
	// Preamble filters to fill TMDS data channels.
	IO_DISP.HDMI_TX.fc_ch0pream							= 0x0B;
	IO_DISP.HDMI_TX.fc_ch1pream.byte[0]					= 0x16;
	IO_DISP.HDMI_TX.fc_ch2pream.byte[0]					= 0x21;
	
	// Enable pixel clock data path.
	IO_DISP.HDMI_TX.mc_clkdis.bit.pixelclk_disable		= 0;
	// Enable TMDS clock data path.
	IO_DISP.HDMI_TX.mc_clkdis.bit.tmdsclk_disable		= 0;
	IO_DISP.HDMI_TX.fc_vsyncinwidth.bit.V_in_width		= 0x02;
	
	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set TMDS Scramble by I2C
 * @retval	D_DDIM_OK						Success.
 * @retval	D_IM_HDMI_TIMEOUT				timeout.
 */
INT32 Im_HDMI_Set_TMDS_Scramble(BOOL enable)
{
	DDIM_USER_FLGPTN flg_ptn;

	im_hdmi_pclk_on();
	
	// clear Interrupt flag.
	DDIM_User_Clr_Flg(FID_IM_HDMI, ~(D_IM_HDMI_INT_FLG_I2CM));
	
	// configure.
	IO_DISP.HDMI_TX.i2cm_softrstz.byte[0]	= 0x00;		// PHY I2C SW reset control register.
	IO_DISP.HDMI_TX.i2cm_div.byte[0]		= 0x03;		// I2C DDC Speed Control Register.
	IO_DISP.HDMI_TX.i2cm_slave.byte[0]		= 0x54;		// I2C slave. (SCDC)
	IO_DISP.HDMI_TX.i2cm_address			= 0x20;		// I2C slave address. (TMDS_Config)
	if( enable ){
		IO_DISP.HDMI_TX.i2cm_datao			= 0x03;		// I2C datao. (Scrambling_Enable=1, TMDS_Bit_Clock_Ratio=1)
	}
	else {
		IO_DISP.HDMI_TX.i2cm_datao			= 0x00;		// I2C datao. (Scrambling_Enable=1, TMDS_Bit_Clock_Ratio=1)
	}
	IO_DISP.HDMI_TX.i2cm_operation.byte[0]	= 0x10;		// I2C operation.
	
	// Wait for interruption.
	if( DDIM_User_Twai_Flg(FID_IM_HDMI, D_IM_HDMI_INT_FLG_I2CM, D_DDIM_USER_TWF_ORW, &flg_ptn, D_DDIM_WAIT_END_TIME) != D_DDIM_USER_E_OK) {
		im_hdmi_pclk_off();
		return D_IM_HDMI_TIMEOUT;
	}

	im_hdmi_pclk_off();

	return D_DDIM_OK;
}

/**
 * @brief		Read Sink's E-EDID.
 */
INT32 Im_HDMI_Read_Sinks_E_EDID(T_IM_HDMI_I2CM_CONFIG const *const i2cm_config, UCHAR read_data[8])
{
	DDIM_USER_FLGPTN flg_ptn;
	INT32 index;
#ifdef CO_PARAM_CHECK
	if( i2cm_config == NULL ) {
		Ddim_Assertion(("Im_HDMI_Read_Sinks_E_EDID Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	
	// clear Interrupt flag.
	DDIM_User_Clr_Flg(FID_IM_HDMI, ~(D_IM_HDMI_INT_FLG_I2CM));
	
	im_hdmi_pclk_on();
	
	// configure.
	IO_DISP.HDMI_TX.i2cm_slave.byte[0]			= i2cm_config->i2cm_slaveaddr;	// I2C slave address.
	IO_DISP.HDMI_TX.i2cm_div.byte[0]			= 0x03;
	if( (i2cm_config->i2cm_operation == E_IM_HDMI_I2CM_OPERATION_RD8) ||
		(i2cm_config->i2cm_operation == E_IM_HDMI_I2CM_OPERATION_RD) ) {
		IO_DISP.HDMI_TX.i2cm_address			= i2cm_config->i2cm_address;	// i2C address.
	}
	else{
		IO_DISP.HDMI_TX.i2cm_segaddr.byte[0]	= i2cm_config->i2cm_segaddr;	// I2C segment address.
		IO_DISP.HDMI_TX.i2cm_segptr				= i2cm_config->i2cm_segptr;		// I2C segment pointer.
		IO_DISP.HDMI_TX.i2cm_address			= i2cm_config->i2cm_address;	// i2C address.
	}
	IO_DISP.HDMI_TX.i2cm_operation.byte[0]	= i2cm_config->i2cm_operation;		// I2C operation.
	
	// Wait for interruption.
	if( DDIM_User_Twai_Flg(FID_IM_HDMI, D_IM_HDMI_INT_FLG_I2CM, D_DDIM_USER_TWF_ORW, &flg_ptn, D_DDIM_WAIT_END_TIME) != D_DDIM_USER_E_OK) {
		Ddim_Print(("Im_HDMI_Read_Sinks_E_EDID Wait i2cmphy INT is timeout!!\n"));
		im_hdmi_pclk_off();
		return D_IM_HDMI_TIMEOUT;
	}
	
	// Read data result.
	if ((i2cm_config->i2cm_operation == E_IM_HDMI_I2CM_OPERATION_RD8) ||
		(i2cm_config->i2cm_operation == E_IM_HDMI_I2CM_OPERATION_RD8_EXT)) {
		for (index = 0; index < 8; index++) {
			read_data[index] = IO_DISP.HDMI_TX.i2cm_read_buff[index].byte[0];
		}
	}
	else {
		read_data[0] = IO_DISP.HDMI_TX.i2cm_datai;
	}
	
	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set control data of HDMI Transmitter. 
 * @param[in]	ctrl	Control data.
 * @retval		D_DDIM_OK					success.
 * @retval		D_IM_HDMI_INPUT_PARAM_ERROR	Input Parameter Error
 */
INT32 Im_HDMI_Ctrl( T_IM_HDMI_CTRL const *const ctrl )
{
	INT32 ret = D_DDIM_OK;
#ifdef CO_PARAM_CHECK
	if( ctrl == NULL ) {
		Ddim_Assertion(("Im_HDMI_Ctrl Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	im_hdmi_pclk_on();
	
	// Power-down HDMI TX PHY Detector.
	IO_DISP.HDMI_TX.phy_conf0.byte[0]						= 0x26;		// PHY SVSRET, PHY ENHPDRXSENSE signal.
	
	IO_DISP.HDMI_TX.phy_conf0.byte[0]						= 0x16;		// PHY PDDQ, PHY ENHPDRXSENSE signal.

	// Set desired video mode.
	
	// select the Video Mapping input mode.
	IO_DISP.HDMI_TX.tx_invid0.bit.video_mapping				= ctrl->video_input_mapping;
	
	// Set video timing information configuration.
	// Set fc_invidconf.
	IO_DISP.HDMI_TX.fc_invidconf.bit.HDCP_keepout			= ctrl->hdcp_keepout;
	IO_DISP.HDMI_TX.fc_invidconf.bit.vsync_in_polarity		= ctrl->vsync_in_polarity;
	IO_DISP.HDMI_TX.fc_invidconf.bit.hsync_in_polarity		= ctrl->hsync_in_polarity;
	IO_DISP.HDMI_TX.fc_invidconf.bit.de_in_polarity			= ctrl->de_in_polarity;
	IO_DISP.HDMI_TX.fc_invidconf.bit.r_v_blank_in_osc		= ctrl->r_v_blank_in_osc;
	IO_DISP.HDMI_TX.fc_invidconf.bit.in_I_P					= ctrl->in_i_p;
	// Set fc_invactiv.
	IO_DISP.HDMI_TX.fc_inhactiv0.byte[0]					= (ctrl->h_in_activ&0x00FF);
	IO_DISP.HDMI_TX.fc_inhactiv1.byte[0]					= (ctrl->h_in_activ >> 8);
	
	IO_DISP.HDMI_TX.fc_invactiv0.byte[0]					= (ctrl->v_in_activ&0x00FF);
	IO_DISP.HDMI_TX.fc_invactiv1.byte[0]					= (ctrl->v_in_activ >> 8);
	// Set fc_invact_2d_0/1, fc_actspc_hdlr_cfg.
	IO_DISP.HDMI_TX.fc_invact_2d0.byte[0]					= (ctrl->fc_invact_2d&0x00FF);
	IO_DISP.HDMI_TX.fc_invact_2d1.byte[0]					= (ctrl->fc_invact_2d >> 8);
	
	IO_DISP.HDMI_TX.fc_actspc_hdlr_cfg.byte[0]				= ctrl->fc_actspc_hdlr_cfg.byte;
	// H blanking pixels / V blanking pixels.
	IO_DISP.HDMI_TX.fc_inhblank0.byte[0]					= (ctrl->fc_inhblank&0x00FF);
	IO_DISP.HDMI_TX.fc_inhblank1.byte[0]					= (ctrl->fc_inhblank >> 8);
	
	IO_DISP.HDMI_TX.fc_invblank								= ctrl->fc_invblank;
	// HSync offset / VSync offset.
	IO_DISP.HDMI_TX.fc_hsyncindelay0.byte[0]				= (ctrl->fc_hsyncindelay&0x00FF);
	IO_DISP.HDMI_TX.fc_hsyncindelay1.byte[0]				= (ctrl->fc_hsyncindelay >> 8);

	IO_DISP.HDMI_TX.fc_vsyncindelay							= ctrl->fc_vsyncindelay;
	// HSync pulse width / VSync pulse width.
	IO_DISP.HDMI_TX.fc_hsyncinwidth0.byte[0]				= (ctrl->fc_hsyncinwidth&0x00FF);
	IO_DISP.HDMI_TX.fc_hsyncinwidth1.byte[0]				= (ctrl->fc_hsyncinwidth >> 8);

	IO_DISP.HDMI_TX.fc_vsyncinwidth.byte[0]					= ctrl->fc_vsyncinwidth;
	
	// DVI mode
	IO_DISP.HDMI_TX.fc_invidconf.bit.DVI_modez				= ctrl->DVI_modez;

	IO_DISP.HDMI_TX.i2cm_div.byte[0]						= 0x03;

	// Configuring the PLL.
	if (im_hdmi_configure_pll(&(ctrl->pll_config)) != D_DDIM_OK) {
		// the PLL does not lock and no activity.
		im_hdmi_pclk_off();
		return D_IM_HDMI_NG;
	}
	
	// Perform a reset to all clock domains.
	IO_DISP.HDMI_TX.mc_swrstzreq.byte[0]					= 0x00;
	// Re-Write the VSync pulse width.
	IO_DISP.HDMI_TX.fc_vsyncinwidth.bit.V_in_width			= ctrl->fc_vsyncinwidth;

	// configure Infoframes.
	ret = im_hdmi_configure_infoframes(&(ctrl->info_frames));
	if (ret != D_DDIM_OK) {
		// result NG.
	}

	im_hdmi_pclk_off();
	return ret;
}

/**
 * @brief	Get control data of HDMI Transmitter. 
 * @param[out]	ctrl	Control data.
 * @retval				D_DDIM_OK					success.
 * @retval				D_IM_HDMI_INPUT_PARAM_ERROR	Input Parameter Error
 */
INT32 Im_HDMI_Get_Ctrl(T_IM_HDMI_CTRL *const ctrl)
{
	INT32 index;
#ifdef CO_PARAM_CHECK
	if( ctrl == NULL ) {
		Ddim_Assertion(("Im_HDMI_Get_Ctrl Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	im_hdmi_pclk_on();
	
	// Get Video Mapping input mode.
	ctrl->video_input_mapping			= IO_DISP.HDMI_TX.tx_invid0.bit.video_mapping;
	
	// Get fc_invidconf.
	ctrl->hdcp_keepout					= IO_DISP.HDMI_TX.fc_invidconf.bit.HDCP_keepout;
	ctrl->vsync_in_polarity				= IO_DISP.HDMI_TX.fc_invidconf.bit.vsync_in_polarity;
	ctrl->hsync_in_polarity				= IO_DISP.HDMI_TX.fc_invidconf.bit.hsync_in_polarity;
	ctrl->de_in_polarity				= IO_DISP.HDMI_TX.fc_invidconf.bit.de_in_polarity;
	ctrl->r_v_blank_in_osc				= IO_DISP.HDMI_TX.fc_invidconf.bit.r_v_blank_in_osc;
	ctrl->in_i_p						= IO_DISP.HDMI_TX.fc_invidconf.bit.in_I_P;
	// Get fc_invactiv.
	ctrl->h_in_activ					= IO_DISP.HDMI_TX.fc_inhactiv1.byte[0];
	ctrl->h_in_activ					= ((ctrl->h_in_activ << 8) | IO_DISP.HDMI_TX.fc_inhactiv0.byte[0]);

	ctrl->v_in_activ					= IO_DISP.HDMI_TX.fc_invactiv1.byte[0];
	ctrl->v_in_activ					= ((ctrl->v_in_activ << 8) | IO_DISP.HDMI_TX.fc_invactiv0.byte[0]);

	// Get fc_invact_2d_0/1, fc_actspc_hdlr_cfg.
	ctrl->fc_invact_2d					= IO_DISP.HDMI_TX.fc_invact_2d1.byte[0];
	ctrl->fc_invact_2d					= ((ctrl->fc_invact_2d << 8) | IO_DISP.HDMI_TX.fc_invact_2d0.byte[0]);

	ctrl->fc_actspc_hdlr_cfg.byte		= IO_DISP.HDMI_TX.fc_actspc_hdlr_cfg.byte[0];
	// Get H blanking pixels / V blanking pixels.
	ctrl->fc_inhblank					= IO_DISP.HDMI_TX.fc_inhblank1.byte[0];
	ctrl->fc_inhblank					= ((ctrl->fc_inhblank << 8) | IO_DISP.HDMI_TX.fc_inhblank0.byte[0]);

	ctrl->fc_invblank					= IO_DISP.HDMI_TX.fc_invblank;
	// Get HSync offset / VSync offset.
	ctrl->fc_hsyncindelay				= IO_DISP.HDMI_TX.fc_hsyncindelay1.byte[0];
	ctrl->fc_hsyncindelay				= ((ctrl->fc_hsyncindelay << 8) | IO_DISP.HDMI_TX.fc_hsyncindelay0.byte[0]);
	ctrl->fc_vsyncindelay				= IO_DISP.HDMI_TX.fc_vsyncindelay;
	// Get HSync pulse width / VSync pulse width
	ctrl->fc_hsyncinwidth				= IO_DISP.HDMI_TX.fc_hsyncinwidth1.byte[0];
	ctrl->fc_hsyncinwidth				= ((ctrl->fc_hsyncinwidth << 8) | IO_DISP.HDMI_TX.fc_hsyncinwidth0.byte[0]);

	ctrl->fc_vsyncinwidth				= IO_DISP.HDMI_TX.fc_vsyncinwidth.byte[0];
	
	// DVI mode
	ctrl->DVI_modez						= IO_DISP.HDMI_TX.fc_invidconf.bit.DVI_modez;

	// Get PLL.
	ctrl->pll_config.vp_pr_cd.byte		= IO_DISP.HDMI_TX.vp_pr_cd.byte[0];
	// PHY Register.
	ctrl->pll_config.phy_reg_num		= D_IM_HDMI_PHY_REG_NUM;
	for (index = 0; index < D_IM_HDMI_PHY_REG_NUM; index++) {
		// Address.
		ctrl->pll_config.phy_reg_addr[index]	= gIM_HDMI_Phy_Register_Address[index];
		// Value.
		if (im_hdmi_phy_read(gIM_HDMI_Phy_Register_Address[index], &(ctrl->pll_config.phy_reg_data[index])) != D_DDIM_OK) {
			ctrl->pll_config.phy_reg_num		= index;
			Ddim_Print(("Im_HDMI_Get_Ctrl PHY Register data read NG!!\n"));
			break;
		}
	}
	
	// Get Audio mode.
	ctrl->audio_config.i2s_select		= (E_IM_HDMI_I2S_SELECT)IO_DISP.HDMI_TX.aud_conf0.bit.i2s_select;
	ctrl->audio_config.aud_conf1.byte	= IO_DISP.HDMI_TX.aud_conf1.byte[0];
	ctrl->audio_config.aud_conf2.byte	= IO_DISP.HDMI_TX.aud_conf2.byte[0];
	ctrl->audio_config.aud_spdif1.byte	= IO_DISP.HDMI_TX.aud_spdif1.byte[0];
	ctrl->audio_config.aud_inputclkfs	= IO_DISP.HDMI_TX.aud_inputclkfs.byte[0];
	ctrl->audio_config.aud_n1			= IO_DISP.HDMI_TX.aud_n1;
	ctrl->audio_config.aud_n2			= IO_DISP.HDMI_TX.aud_n2;
	ctrl->audio_config.aud_n3_audn		= IO_DISP.HDMI_TX.aud_n3.bit.AudN;
	ctrl->audio_config.cts_manual		= (E_IM_HDMI_CTS_MANUAL)IO_DISP.HDMI_TX.aud_cts3.bit.CTS_manual;
	ctrl->audio_config.aud_cts1			= IO_DISP.HDMI_TX.aud_cts1;
	ctrl->audio_config.aud_cts2			= IO_DISP.HDMI_TX.aud_cts2;
	ctrl->audio_config.aud_cts3_audcts	= IO_DISP.HDMI_TX.aud_cts3.bit.AudCTS;
	
	// Infoframes.
	ctrl->info_frames.fc_aviconf0.byte	= IO_DISP.HDMI_TX.fc_aviconf0.byte[0];
	ctrl->info_frames.fc_aviconf1.byte	= IO_DISP.HDMI_TX.fc_aviconf1.byte[0];
	ctrl->info_frames.fc_aviconf2.byte	= IO_DISP.HDMI_TX.fc_aviconf2.byte[0];
	ctrl->info_frames.fc_aviconf3.byte	= IO_DISP.HDMI_TX.fc_aviconf3.byte[0];
	ctrl->info_frames.fc_avivid			= IO_DISP.HDMI_TX.fc_avivid.byte[0];
	ctrl->info_frames.fc_prconf.byte	= IO_DISP.HDMI_TX.fc_prconf.byte[0];
	ctrl->info_frames.fc_audiconf0.byte	= IO_DISP.HDMI_TX.fc_audiconf0.byte[0];
	ctrl->info_frames.fc_audiconf1.byte	= IO_DISP.HDMI_TX.fc_audiconf1.byte[0];
	ctrl->info_frames.fc_audiconf2		= IO_DISP.HDMI_TX.fc_audiconf2;
	ctrl->info_frames.fc_audiconf3.byte	= IO_DISP.HDMI_TX.fc_audiconf3.byte[0];
	ctrl->info_frames.fc_vsdieeeid[0]	= IO_DISP.HDMI_TX.fc_vsdieeeid0;
	ctrl->info_frames.fc_vsdieeeid[1]	= IO_DISP.HDMI_TX.fc_vsdieeeid1;
	ctrl->info_frames.fc_vsdieeeid[2]	= IO_DISP.HDMI_TX.fc_vsdieeeid2;
	ctrl->info_frames.scrambler_on		= IO_DISP.HDMI_TX.fc_scrambler_ctrl.bit.scrambler_on;
	ctrl->info_frames.video_mode		= (IO_DISP.HDMI_TX.fc_vsdpayload[0].byte[0] & 0xE0);
	if (ctrl->info_frames.video_mode == D_IM_HDMI_VIDEO_MODE_HDMI) {
		// HDMI Video Mode.
		ctrl->info_frames.video_format				= IO_DISP.HDMI_TX.fc_vsdpayload[1].byte[0];
	}
	else {
		// 3D Video Mode.
		ctrl->info_frames.video_format				= (IO_DISP.HDMI_TX.fc_vsdpayload[1].byte[0] & 0xF0) >> 4;
		ctrl->info_frames.meta_present				= (IO_DISP.HDMI_TX.fc_vsdpayload[1].byte[0] & 0x80) >> 3;
		ctrl->info_frames.additional_video_format	= (IO_DISP.HDMI_TX.fc_vsdpayload[2].byte[0] & 0xF0) >> 4;
		ctrl->info_frames.meta_type					= (IO_DISP.HDMI_TX.fc_vsdpayload[3].byte[0] & 0xE0) >> 5;
		ctrl->info_frames.meta_length				= (IO_DISP.HDMI_TX.fc_vsdpayload[3].byte[0] & 0x1F);
		for (index = 0; index < 8; index++) {
			ctrl->info_frames.meta_data[index]		= IO_DISP.HDMI_TX.fc_vsdpayload[4 + index].byte[0];
		}
	}
	
	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Color Space Converter.
 */
INT32 Im_HDMI_CSC(T_IM_HDMI_CSC_CONFIG const *const config, const T_IM_HDMI_CSC_COEF matrix[D_IM_HDMI_CSC_CONF_COLUMN_NUM][D_IM_HDMI_CSC_CONF_ROW_NUM])
{
	INT32 loop1, loop2;
	
#ifdef CO_PARAM_CHECK
	if ((config == NULL) || (matrix == NULL)) {
		Ddim_Assertion(("Im_HDMI_CSC Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	im_hdmi_pclk_on();
	
	// Set csc_cfg/csc_scale.
	IO_DISP.HDMI_TX.csc_cfg.byte[0]	= config->csc_cfg.byte;
	IO_DISP.HDMI_TX.csc_scale.byte[0]	= config->csc_scale.byte;
	
	// Set csc_coef_a1_msb~csc_coef_c4_lsb.
	for (loop1 = 0; loop1 < D_IM_HDMI_CSC_CONF_COLUMN_NUM; loop1++) {
		for (loop2 = 0; loop2 < D_IM_HDMI_CSC_CONF_ROW_NUM; loop2++) {
			IO_DISP.HDMI_TX.csc_coef[loop1][loop2].msb	= matrix[loop1][loop2].msb;
			IO_DISP.HDMI_TX.csc_coef[loop1][loop2].lsb	= matrix[loop1][loop2].lsb;
		}
	}
	
	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Video Sampler.
 */
VOID Im_HDMI_Set_Video_Sampler(U_IM_HDMI_TX_INVID0 tx_invid0)
{
	im_hdmi_pclk_on();
	
	// Set tx_invid0.
	IO_DISP.HDMI_TX.tx_invid0.byte[0]	= tx_invid0.byte;
	
	im_hdmi_pclk_off();
}

/**
 * @brief	Get Video Sampler.
 */
INT32 Im_HDMI_Get_Video_Sampler(U_IM_HDMI_TX_INVID0* tx_invid0)
{
#ifdef CO_PARAM_CHECK
	if (tx_invid0 == NULL) {
		Ddim_Assertion(("Im_HDMI_Get_Video_Sampler Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	im_hdmi_pclk_on();
	
	// Get tx_invid0.
	tx_invid0->byte = IO_DISP.HDMI_TX.tx_invid0.byte[0];
	
	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Video Packetizer.
 */
INT32 Im_HDMI_Set_Video_Packetizer(T_IM_HDMI_VIDEO_PACKETIZER const *const video_packetizer)
{
#ifdef CO_PARAM_CHECK
	if (video_packetizer == NULL) {
		Ddim_Assertion(("Im_HDMI_Set_Video_Packetizer Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	im_hdmi_pclk_on();
	
	// Set vp_pr_cd/vp_remap/vp_conf.
	IO_DISP.HDMI_TX.vp_pr_cd.byte[0]	= video_packetizer->vp_pr_cd.byte;
	IO_DISP.HDMI_TX.vp_remap.byte[0]	= video_packetizer->vp_remap;
	IO_DISP.HDMI_TX.vp_conf.byte[0]		= video_packetizer->vp_conf.byte;
	
	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Get Video Packetizer.
 */
INT32 Im_HDMI_Get_Video_Packetizer(T_IM_HDMI_VIDEO_PACKETIZER *const video_packetizer)
{
#ifdef CO_PARAM_CHECK
	if (video_packetizer == NULL) {
		Ddim_Assertion(("Im_HDMI_Get_Video_Packetizer Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	im_hdmi_pclk_on();
	
	// Get vp_pr_cd/vp_remap/vp_conf.
	video_packetizer->vp_pr_cd.byte	= IO_DISP.HDMI_TX.vp_pr_cd.byte[0];
	video_packetizer->vp_remap		= (E_IM_HDMI_VP_REMAP)IO_DISP.HDMI_TX.vp_remap.byte[0];
	video_packetizer->vp_conf.byte	= IO_DISP.HDMI_TX.vp_conf.byte[0];
	
	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Frame Composer.
 */
INT32 Im_HDMI_Set_Frame_Composer(T_IM_HDMI_FRAME_COMPOSER const *const frame_composer)
{
	unsigned char write_work = 0;
	
#ifdef CO_PARAM_CHECK
	if (frame_composer == NULL) {
		Ddim_Assertion(("Im_HDMI_Set_Frame_Composer Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	im_hdmi_pclk_on();
	
	// Set fc_invidconf.
	IO_DISP.HDMI_TX.fc_invidconf.byte[0]		= frame_composer->fc_invidconf.byte;
	
	// Set fc_inhactiv0/1.
	write_work = frame_composer->fc_inhactiv;
	IO_DISP.HDMI_TX.fc_inhactiv0.byte[0]		= write_work;
	write_work = frame_composer->fc_inhactiv >> 8;
	IO_DISP.HDMI_TX.fc_inhactiv1.byte[0]		= write_work;
	
	// Set fc_inhblank0/1.
	write_work = frame_composer->fc_inhblank;
	IO_DISP.HDMI_TX.fc_inhblank0.byte[0]		= write_work;
	write_work = frame_composer->fc_inhblank >> 8;
	IO_DISP.HDMI_TX.fc_inhblank1.byte[0]		= write_work;
	
	// Set fc_invactiv0/1.
	write_work = frame_composer->fc_invactiv;
	IO_DISP.HDMI_TX.fc_invactiv0.byte[0]		= write_work;
	write_work = frame_composer->fc_invactiv >> 8;
	IO_DISP.HDMI_TX.fc_invactiv1.byte[0]		= write_work;
	
	// Set fc_invblank.
	IO_DISP.HDMI_TX.fc_invblank				= frame_composer->fc_invblank;
	
	// Set fc_hsyncindelay0/1.
	IO_DISP.HDMI_TX.fc_hsyncindelay0.byte[0]	= (frame_composer->fc_hsyncindelay&0x00FF);
	IO_DISP.HDMI_TX.fc_hsyncindelay1.byte[0]	= (frame_composer->fc_hsyncindelay >> 8);
	// Set fc_hsyncinwidth0/1.
	IO_DISP.HDMI_TX.fc_hsyncinwidth0.byte[0]	= (frame_composer->fc_hsyncinwidth&0x00FF);
	IO_DISP.HDMI_TX.fc_hsyncinwidth1.byte[0]	= (frame_composer->fc_hsyncinwidth >> 8);
	// Set fc_vsyncindelay/fc_vsyncinwidth.
	IO_DISP.HDMI_TX.fc_vsyncindelay			= frame_composer->fc_vsyncindelay;
	IO_DISP.HDMI_TX.fc_vsyncinwidth.byte[0]	= frame_composer->fc_vsyncinwidth;
	
	// Set fc_aviconf3/fc_aviconf0/fc_aviconf1/fc_aviconf2.
	IO_DISP.HDMI_TX.fc_aviconf3.byte[0]		= frame_composer->fc_aviconf3.byte;
	IO_DISP.HDMI_TX.fc_aviconf0.byte[0]		= frame_composer->fc_aviconf0.byte;
	IO_DISP.HDMI_TX.fc_aviconf1.byte[0]		= frame_composer->fc_aviconf1.byte;
	IO_DISP.HDMI_TX.fc_aviconf2.byte[0]		= frame_composer->fc_aviconf2.byte;
	// Set fc_avivid.
	IO_DISP.HDMI_TX.fc_avivid.byte[0]			= frame_composer->fc_avivid;
	// Set fc_prconf.
	IO_DISP.HDMI_TX.fc_prconf.byte[0]			= frame_composer->fc_prconf.byte;
	
	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Get Frame Composer.
 */
INT32 Im_HDMI_Get_Frame_Composer(T_IM_HDMI_FRAME_COMPOSER *const frame_composer)
{
#ifdef CO_PARAM_CHECK
	if (frame_composer == NULL) {
		Ddim_Assertion(("Im_HDMI_Get_Frame_Composer Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	im_hdmi_pclk_on();
	
	// Get fc_invidconf.
	frame_composer->fc_invidconf.byte	= IO_DISP.HDMI_TX.fc_invidconf.byte[0];
	// Get fc_inhactiv0/1.
	frame_composer->fc_inhactiv			= IO_DISP.HDMI_TX.fc_inhactiv1.byte[0];
	frame_composer->fc_inhactiv			= ((frame_composer->fc_inhactiv << 8) | IO_DISP.HDMI_TX.fc_inhactiv0.byte[0]);

	// Get fc_inhblank0/1.
	frame_composer->fc_inhblank			= IO_DISP.HDMI_TX.fc_inhblank1.byte[0];
	frame_composer->fc_inhblank			= ((frame_composer->fc_inhblank << 8) | IO_DISP.HDMI_TX.fc_inhblank0.byte[0]);

	// Get fc_invactiv0/1.
	frame_composer->fc_invactiv			= IO_DISP.HDMI_TX.fc_invactiv1.byte[0];
	frame_composer->fc_invactiv			= ((frame_composer->fc_invactiv << 8) | IO_DISP.HDMI_TX.fc_invactiv0.byte[0]);
	// Get fc_invblank.
	frame_composer->fc_invblank			= IO_DISP.HDMI_TX.fc_invblank;
	
	// Get fc_hsyncindelay0/1.
	frame_composer->fc_hsyncindelay		= IO_DISP.HDMI_TX.fc_hsyncindelay1.byte[0];
	frame_composer->fc_hsyncindelay		= ((frame_composer->fc_hsyncindelay << 8) | IO_DISP.HDMI_TX.fc_hsyncindelay0.byte[0]);
	// Get fc_hsyncinwidth0/1.
	frame_composer->fc_hsyncinwidth		= IO_DISP.HDMI_TX.fc_hsyncinwidth1.byte[0];
	frame_composer->fc_hsyncinwidth		= ((frame_composer->fc_hsyncinwidth << 8) | IO_DISP.HDMI_TX.fc_hsyncinwidth0.byte[0]);

	// Get fc_vsyncindelay/fc_vsyncinwidth.
	frame_composer->fc_vsyncindelay		= IO_DISP.HDMI_TX.fc_vsyncindelay;
	frame_composer->fc_vsyncinwidth		= IO_DISP.HDMI_TX.fc_vsyncinwidth.byte[0];
	
	// Get fc_aviconf3/fc_aviconf0/fc_aviconf1/fc_aviconf2.
	frame_composer->fc_aviconf3.byte	= IO_DISP.HDMI_TX.fc_aviconf3.byte[0];
	frame_composer->fc_aviconf0.byte	= IO_DISP.HDMI_TX.fc_aviconf0.byte[0];
	frame_composer->fc_aviconf1.byte	= IO_DISP.HDMI_TX.fc_aviconf1.byte[0];
	frame_composer->fc_aviconf2.byte	= IO_DISP.HDMI_TX.fc_aviconf2.byte[0];
	// Get fc_avivid.
	frame_composer->fc_avivid			= IO_DISP.HDMI_TX.fc_avivid.byte[0];
	// Get fc_prconf.
	frame_composer->fc_prconf.byte		= IO_DISP.HDMI_TX.fc_prconf.byte[0];
	
	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Frame Composer (SPD Packet Data).
 */
INT32 Im_HDMI_Set_Frame_Composer_Spd(T_IM_HDMI_FC_SPD const *const fc_spd)
{
	INT32 loop;
#ifdef CO_PARAM_CHECK
	if (fc_spd == NULL) {
		Ddim_Assertion(("Im_HDMI_Set_Frame_Composer_Spd Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	im_hdmi_pclk_on();
	
	// Vendor Name.
	for (loop = 0; loop < D_IM_HDMI_FC_SPD_VENDER_NAME_NUM; loop++) {
		IO_DISP.HDMI_TX.fc_spdvendorname[loop].byte[0]		= fc_spd->fc_spdvendorname[loop];
	}
	// Product Name.
	for (loop = 0; loop < D_IM_HDMI_FC_SPD_PRO_NAME_NUM; loop++) {
		IO_DISP.HDMI_TX.fc_spdproductname[loop].byte[0]	= fc_spd->fc_spdproductname[loop];
	}
	// Source Product Descriptor.
	IO_DISP.HDMI_TX.fc_spddeviceinf				= fc_spd->fc_spddeviceinf;
	// Enables SPD automatic packet scheduling
	IO_DISP.HDMI_TX.fc_datauto0.bit.spd_auto		= 1;
	// auto_frame_packets
	IO_DISP.HDMI_TX.fc_datauto2.bit.auto_frame_packets = 1;
	
	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Get Frame Composer (SPD Packet Data).
 */
INT32 Im_HDMI_Get_Frame_Composer_Spd(T_IM_HDMI_FC_SPD *const fc_spd)
{
	INT32 loop;
#ifdef CO_PARAM_CHECK
	if (fc_spd == NULL) {
		Ddim_Assertion(("Im_HDMI_Get_Frame_Composer_Spd Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	im_hdmi_pclk_on();
	
	// Vendor Name.
	for (loop = 0; loop < D_IM_HDMI_FC_SPD_VENDER_NAME_NUM; loop++) {
		fc_spd->fc_spdvendorname[loop]	= IO_DISP.HDMI_TX.fc_spdvendorname[loop].byte[0];
	}
	// Product Name.
	for (loop = 0; loop < D_IM_HDMI_FC_SPD_PRO_NAME_NUM; loop++) {
		fc_spd->fc_spdproductname[loop]	= IO_DISP.HDMI_TX.fc_spdproductname[loop].byte[0];
	}
	// Source Product Descriptor.
	fc_spd->fc_spddeviceinf				= IO_DISP.HDMI_TX.fc_spddeviceinf;
	
	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Frame Composer (Vendor Specific).
 */
INT32 Im_HDMI_Set_Frame_Composer_Vsd(T_IM_HDMI_FC_VSD const *const fc_vsd)
{
	INT32 loop;
#ifdef CO_PARAM_CHECK
	if (fc_vsd == NULL) {
		Ddim_Assertion(("Im_HDMI_Set_Frame_Composer_Vsd Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	im_hdmi_pclk_on();
	
	// IEEE registration identifier.
	IO_DISP.HDMI_TX.fc_vsdieeeid0			= fc_vsd->fc_vsdieeeid[0];
	IO_DISP.HDMI_TX.fc_vsdieeeid1			= fc_vsd->fc_vsdieeeid[1];
	IO_DISP.HDMI_TX.fc_vsdieeeid2			= fc_vsd->fc_vsdieeeid[2];
	
	// VSI Packet Data Size.
	IO_DISP.HDMI_TX.fc_vsdsize.bit.VSDSIZE	= fc_vsd->fc_vsdsize;
	
	// Payload Register Array.
	for (loop = 0; loop < D_IM_HDMI_FC_VSD_PAYLOAD_NUM; loop++) {
		IO_DISP.HDMI_TX.fc_vsdpayload[loop].byte[0]	= fc_vsd->fc_vsdpayload[loop];
	}
	// Enables VSD automatic packet scheduling
	IO_DISP.HDMI_TX.fc_datauto0.bit.vsd_auto		= 1;
	// auto_frame_packets
	IO_DISP.HDMI_TX.fc_datauto2.bit.auto_frame_packets = 1;
	
	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Get Frame Composer (Vendor Specific).
 */
INT32 Im_HDMI_Get_Frame_Composer_Vsd(T_IM_HDMI_FC_VSD *const fc_vsd)
{
	INT32 loop;
#ifdef CO_PARAM_CHECK
	if (fc_vsd == NULL) {
		Ddim_Assertion(("Im_HDMI_Get_Frame_Composer_Vsd Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	im_hdmi_pclk_on();
	
	// IEEE registration identifier.
	fc_vsd->fc_vsdieeeid[0]	= IO_DISP.HDMI_TX.fc_vsdieeeid0;
	fc_vsd->fc_vsdieeeid[1]	= IO_DISP.HDMI_TX.fc_vsdieeeid1;
	fc_vsd->fc_vsdieeeid[2]	= IO_DISP.HDMI_TX.fc_vsdieeeid2;
	
	// VSI Packet Data Size.
	fc_vsd->fc_vsdsize		= IO_DISP.HDMI_TX.fc_vsdsize.byte[0];
	
	// Payload Register Array.
	for (loop = 0; loop < D_IM_HDMI_FC_VSD_PAYLOAD_NUM; loop++) {
		fc_vsd->fc_vsdpayload[loop]	= IO_DISP.HDMI_TX.fc_vsdpayload[loop].byte[0];
	}
	
	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Main Controller.
 */
INT32 Im_HDMI_Set_Main_Controller(UCHAR mc_flowctrl)
{
#ifdef CO_PARAM_CHECK
	if ((mc_flowctrl != D_IM_HDMI_ENABLE_OFF) && (mc_flowctrl != D_IM_HDMI_ENABLE_ON)) {
		Ddim_Assertion(("Im_HDMI_Set_Main_Controller Input_Param_Err\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	im_hdmi_pclk_on();
	
	// Set mc_flowctrl.
	IO_DISP.HDMI_TX.mc_flowctrl.byte[0] = mc_flowctrl;
	
	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Get Main Controller.
 */
INT32 Im_HDMI_Get_Main_Controller(UCHAR* mc_flowctrl)
{
#ifdef CO_PARAM_CHECK
	if (mc_flowctrl == NULL) {
		Ddim_Assertion(("Im_HDMI_Get_Main_Controller Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	im_hdmi_pclk_on();
	
	// Get mc_flowctrl.
	*mc_flowctrl = IO_DISP.HDMI_TX.mc_flowctrl.byte[0];
	
	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Audio data.
 */
INT32 Im_HDMI_Set_Audio(T_IM_HDMI_AUDIO const *const audio)
{
#ifdef CO_PARAM_CHECK
	if (audio == NULL) {
		Ddim_Assertion(("Im_HDMI_Set_Audio Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	im_hdmi_pclk_on();
	
	// Clock Domain Disable.
	IO_DISP.HDMI_TX.mc_clkdis.bit.audclk_disable		= 1;

	// Set aud_inputclkfs.
	IO_DISP.HDMI_TX.aud_inputclkfs.byte[0]	= audio->aud_inputclkfs;
	
	// Set aud_conf0/aud_conf1/aud_conf2.
	IO_DISP.HDMI_TX.aud_conf0.byte[0]		= audio->aud_conf0.byte;
	IO_DISP.HDMI_TX.aud_conf1.byte[0]		= audio->aud_conf1.byte;
	IO_DISP.HDMI_TX.aud_conf2.byte[0]		= audio->aud_conf2.byte;
	
	// Set fc_audsconf.
	IO_DISP.HDMI_TX.fc_audsconf.byte[0]		= audio->fc_audsconf.byte;
	
	// Set aud_spdif1.
	IO_DISP.HDMI_TX.aud_spdif1.byte[0]		= audio->aud_spdif1.byte;

	// Audio Infoframe.
	IO_DISP.HDMI_TX.fc_audiconf0.byte[0]				= audio->fc_audiconf0.byte;
	IO_DISP.HDMI_TX.fc_audiconf1.byte[0]				= audio->fc_audiconf1.byte;
	IO_DISP.HDMI_TX.fc_audiconf2						= audio->fc_audiconf2;
	IO_DISP.HDMI_TX.fc_audiconf3.byte[0]				= audio->fc_audiconf3.byte;

	// Frame Composer Audio Sample Channel Status Configuration Register 7(Sampling frequency)
	IO_DISP.HDMI_TX.fc_audschnl7.bit.oiec_sampfreq		= audio->sampfreq;
	// Frame Composer Audio Sample Channel Status Configuration Register 8(Word length configuration)
	IO_DISP.HDMI_TX.fc_audschnl8.bit.oiec_wordlength	= audio->oiec_wordlength;

	// Clock Domain Disable.
	IO_DISP.HDMI_TX.mc_clkdis.bit.audclk_disable		= 0;

	// CTS Automatic/Manual select.
	if (audio->cts_manual == E_IM_HDMI_CTS_MANUAL_MANUAL) {
		// N3 ncts_atomic_write
		IO_DISP.HDMI_TX.aud_n3.bit.ncts_atomic_write	= audio->aud_ncts_atomic_write;
		// CTS Manual.
		IO_DISP.HDMI_TX.aud_cts3.bit.CTS_manual			= E_IM_HDMI_CTS_MANUAL_MANUAL;
		//Set Audio fixed N factor.
		IO_DISP.HDMI_TX.aud_cts3.bit.AudCTS				= audio->aud_cts3_audcts;
		IO_DISP.HDMI_TX.aud_cts2						= audio->aud_cts2;
		IO_DISP.HDMI_TX.aud_cts1						= audio->aud_cts1;
		// N3 ncts_atomic_write
		IO_DISP.HDMI_TX.aud_n3.bit.ncts_atomic_write	= audio->aud_ncts_atomic_write;
		//Set Audio fixed N factor.
		IO_DISP.HDMI_TX.aud_n3.bit.AudN					= audio->aud_n3_audn;
		IO_DISP.HDMI_TX.aud_n2							= audio->aud_n2;
		IO_DISP.HDMI_TX.aud_n1							= audio->aud_n1;
	}
	else {
		// CTS Automatic.
		IO_DISP.HDMI_TX.aud_cts3.bit.CTS_manual			= E_IM_HDMI_CTS_MANUAL_AUTO;
		// N3 ncts_atomic_write
		IO_DISP.HDMI_TX.aud_n3.bit.ncts_atomic_write	= audio->aud_ncts_atomic_write;
		//Set Audio fixed N factor.
		IO_DISP.HDMI_TX.aud_n3.bit.AudN					= audio->aud_n3_audn;
		IO_DISP.HDMI_TX.aud_n2							= audio->aud_n2;
		IO_DISP.HDMI_TX.aud_n1							= audio->aud_n1;
	}

	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Get Audio data.
 */
INT32 Im_HDMI_Get_Audio(T_IM_HDMI_AUDIO *const audio)
{
#ifdef CO_PARAM_CHECK
	if (audio == NULL) {
		Ddim_Assertion(("Im_HDMI_Get_Audio Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	im_hdmi_pclk_on();
	
	// Get aud_inputclkfs.
	audio->aud_inputclkfs		= IO_DISP.HDMI_TX.aud_inputclkfs.byte[0];
	
	// Get aud_conf0/aud_conf1/aud_conf2.
	audio->aud_conf0.byte		= IO_DISP.HDMI_TX.aud_conf0.byte[0];
	audio->aud_conf1.byte		= IO_DISP.HDMI_TX.aud_conf1.byte[0];
	audio->aud_conf2.byte		= IO_DISP.HDMI_TX.aud_conf2.byte[0];
	
	// Get fc_audsconf.
	audio->fc_audsconf.byte		= IO_DISP.HDMI_TX.fc_audsconf.byte[0];
	
	// Get aud_spdif1.
	audio->aud_spdif1.byte		= IO_DISP.HDMI_TX.aud_spdif1.byte[0];

	// Get N3 ncts_atomic_write
	audio->aud_ncts_atomic_write	= IO_DISP.HDMI_TX.aud_n3.bit.ncts_atomic_write;
	// Get CTS Manual.
	audio->cts_manual			= IO_DISP.HDMI_TX.aud_cts3.bit.CTS_manual;
	// Get Audio fixed N factor.
	audio->aud_cts3_audcts		= IO_DISP.HDMI_TX.aud_cts3.bit.AudCTS;
	audio->aud_cts2				= IO_DISP.HDMI_TX.aud_cts2;
	audio->aud_cts1				= IO_DISP.HDMI_TX.aud_cts1;
	// Get Audio fixed N factor.
	audio->aud_n3_audn			= IO_DISP.HDMI_TX.aud_n3.bit.AudN;
	audio->aud_n2				= IO_DISP.HDMI_TX.aud_n2;
	audio->aud_n1				= IO_DISP.HDMI_TX.aud_n1;
	
	// Get Audio Infoframe.
	audio->fc_audiconf0.byte	= IO_DISP.HDMI_TX.fc_audiconf0.byte[0];
	audio->fc_audiconf1.byte	= IO_DISP.HDMI_TX.fc_audiconf1.byte[0];
	audio->fc_audiconf2			= IO_DISP.HDMI_TX.fc_audiconf2;
	audio->fc_audiconf3.byte	= IO_DISP.HDMI_TX.fc_audiconf3.byte[0];

	// Get Frame Composer Audio Sample Channel Status Configuration Register 7(Sampling frequency)
	audio->sampfreq				= IO_DISP.HDMI_TX.fc_audschnl7.bit.oiec_sampfreq;
	// Get Frame Composer Audio Sample Channel Status Configuration Register 8(Word length configuration)
	audio->oiec_wordlength		= IO_DISP.HDMI_TX.fc_audschnl8.bit.oiec_wordlength;

	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Set Interrupt callback function.
 */
VOID Im_HDMI_Set_Int_Callback(VP_HDMI_CALLBACK vp_callback)
{
	// Set interrupt callback.
	gIM_HDMI_INT_Callback = vp_callback;
}

/**
 * @brief	Set Interrupt Mute.
 */
VOID Im_HDMI_Set_Int_Mute(E_IM_HDMI_INT_REG interrupt_register, UCHAR mute)
{
	im_hdmi_pclk_on();
	
	// Check Interrupt type.
	switch (interrupt_register) {
		case E_IM_HDMI_INT_REG_FC_STAT0:	// ih_mute_fc_stat0 Register.
			IO_DISP.HDMI_TX.ih_mute_fc_stat0.byte[0]	= mute;
			break;
		case E_IM_HDMI_INT_REG_FC_STAT1:	// ih_mute_fc_stat1 Register.
			IO_DISP.HDMI_TX.ih_mute_fc_stat1.byte[0]	= mute;
			break;
		case E_IM_HDMI_INT_REG_FC_STAT2:	// ih_mute_fc_stat2 Register.
			IO_DISP.HDMI_TX.ih_mute_fc_stat2.byte[0]	= mute;
			break;
		case E_IM_HDMI_INT_REG_AS_STAT0:	// ih_mute_as_stat0 Register.
			IO_DISP.HDMI_TX.ih_mute_as_stat0.byte[0]	= mute;
			break;
		case E_IM_HDMI_INT_REG_PHY_STAT0:	// ih_mute_phy_stat0 Register.
			IO_DISP.HDMI_TX.ih_mute_phy_stat0.byte[0]	= mute;
			break;
		case E_IM_HDMI_INT_REG_I2CM_STAT0:	// ih_mute_i2cm_stat0 Register.
			IO_DISP.HDMI_TX.ih_mute_i2cm_stat0.byte[0]	= mute;
			break;
		case E_IM_HDMI_INT_REG_CEC_STAT0:	// ih_mute_cec_stat0 Register.
			IO_DISP.HDMI_TX.ih_mute_cec_stat0.byte[0]	= mute;
			break;
		case E_IM_HDMI_INT_REG_VP_STAT0:	// ih_mute_vp_stat0 Register.
			IO_DISP.HDMI_TX.ih_mute_vp_stat0.byte[0]	= mute;
			break;
		default:							// default (ih_mute_i2cmphy_stat0).
			IO_DISP.HDMI_TX.ih_i2cmphy_stat0.byte[0]	= mute;
			break;
	}
	
	im_hdmi_pclk_off();
}

/**
 * @brief	Get Interrupt Mute.
 */
INT32 Im_HDMI_Get_Int_Mute(E_IM_HDMI_INT_REG interrupt_register, UCHAR* mute)
{
#ifdef CO_PARAM_CHECK
	if (mute == NULL) {
		Ddim_Assertion(("Im_HDMI_Get_Int_Mute Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	im_hdmi_pclk_on();
	
	// Check Interrupt type.
	switch (interrupt_register) {
		case E_IM_HDMI_INT_REG_FC_STAT0:	// ih_mute_fc_stat0 Register.
			*mute = IO_DISP.HDMI_TX.ih_mute_fc_stat0.byte[0];
			break;
		case E_IM_HDMI_INT_REG_FC_STAT1:	// ih_mute_fc_stat1 Register.
			*mute = IO_DISP.HDMI_TX.ih_mute_fc_stat1.byte[0];
			break;
		case E_IM_HDMI_INT_REG_FC_STAT2:	// ih_mute_fc_stat2 Register.
			*mute = IO_DISP.HDMI_TX.ih_mute_fc_stat2.byte[0];
			break;
		case E_IM_HDMI_INT_REG_AS_STAT0:	// ih_mute_as_stat0 Register.
			*mute = IO_DISP.HDMI_TX.ih_mute_as_stat0.byte[0];
			break;
		case E_IM_HDMI_INT_REG_PHY_STAT0:	// ih_mute_phy_stat0 Register.
			*mute = IO_DISP.HDMI_TX.ih_mute_phy_stat0.byte[0];
			break;
		case E_IM_HDMI_INT_REG_I2CM_STAT0:	// ih_mute_i2cm_stat0 Register.
			*mute = IO_DISP.HDMI_TX.ih_mute_i2cm_stat0.byte[0];
			break;
		case E_IM_HDMI_INT_REG_CEC_STAT0:	// ih_mute_cec_stat0 Register.
			*mute = IO_DISP.HDMI_TX.ih_mute_cec_stat0.byte[0];
			break;
		case E_IM_HDMI_INT_REG_VP_STAT0:	// ih_mute_vp_stat0 Register.
			*mute = IO_DISP.HDMI_TX.ih_mute_vp_stat0.byte[0];
			break;
		default:							// default (ih_mute_i2cmphy_stat0).
			*mute = IO_DISP.HDMI_TX.ih_i2cmphy_stat0.byte[0];
			break;
	}
	
	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}

/**
 * @brief	Get PHY status.
 */
INT32 Im_HDMI_Get_Phy_Status(U_IM_HDMI_PHY_STAT0 *status)
{
#ifdef CO_PARAM_CHECK
	if (status == NULL) {
		Ddim_Assertion(("Im_HDMI_Get_Phy_Status Input_Param_Err status NULL\n"));
		return D_IM_HDMI_INPUT_PARAM_ERROR;
	}
#endif
	im_hdmi_pclk_on();
	
	// Get phy_stat0 Register.
	status->byte = IO_DISP.HDMI_TX.phy_stat0.byte[0];
	
	im_hdmi_pclk_off();
	
	return D_DDIM_OK;
}
/**
 * @brief	HDMI Interrupt Handler.
 */
VOID Im_HDMI_Int_Handler(VOID)
{
	im_hdmi_pclk_on();
	
	// check ih_fc_stat0/ih_fc_stat1/ih_fc_stat2.
	im_hdmi_check_interrupt_fc();
	// check as_stat0.
	im_hdmi_check_interrupt_as();
	// check ih_phy_stat0.
	im_hdmi_check_interrupt_phy();
	// check ih_i2cm_stat0.
	im_hdmi_check_interrupt_i2cm();
	// check ih_cec_stat0.
	im_hdmi_check_interrupt_cec();
	// check ih_vp_stat0.
	im_hdmi_check_interrupt_vp();
	// check ih_i2cmphy_stat0.
	im_hdmi_check_interrupt_i2cmphy();
	
	im_hdmi_pclk_off();
}

