/**
 * @file		dd_pcie_rc.c
 * @brief		PCIe RootComplex driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "ddim_user_custom.h"
#include "chiptop.h"
#include "dd_exs.h"
#include "dd_pcie_rc.h"
#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define D_DEBUG_PRINT_INFO_ON

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static volatile E_DD_PCIE_BIFURCATION			gDD_PCIE_RC_Bifurcation = E_DD_PCIE_BIFURCATION_DISABLE;
static volatile T_DD_PCIE_INT_DMA_CB			gDD_PCIE_RC_Int_Dma_Cb[D_DD_PCIE_RC_MAX_CH] = {{NULL, NULL},
																							   {NULL, NULL}};
static volatile VP_CALLBACK_PCIE				gDD_PCIE_RC_Int_Msi_Cb[D_DD_PCIE_RC_MAX_CH] = {NULL, NULL};
static volatile T_DD_PCIE_RC_INT_TRS_ICS_MSI_CB	gDD_PCIE_RC_Int_Trs_Ics_msi_Cb[D_DD_PCIE_RC_MAX_CH] = {{NULL, NULL, NULL},
																									   {NULL, NULL, NULL}};
static volatile T_DD_PCIE_RC_INT_SYS_ERR_CB		gDD_PCIE_RC_Int_Sys_Err_Cb[D_DD_PCIE_RC_MAX_CH] = {{NULL, NULL},
																								   {NULL, NULL}};
static volatile T_DD_PCIE_RC_INT_EP_CB			gDD_PCIE_RC_Int_Ep_Cb[D_DD_PCIE_RC_MAX_CH] = {{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
																							  {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}};
static volatile T_DD_PCIE_RC_INT_OWN_CB			gDD_PCIE_RC_Int_Own_Cb[D_DD_PCIE_RC_MAX_CH] = {{NULL, NULL, NULL, NULL},
																							   {NULL, NULL, NULL, NULL}};
static volatile VP_CALLBACK_PCIE				gDD_PCIE_RC_Hp_Detect_Card[D_DD_PCIE_RC_MAX_CH] = {NULL, NULL};
static volatile USHORT							gDD_PCIE_RC_Config_ADR[D_DD_PCIE_RC_MAX_CH] = {(D_DD_PCIE_0_PIO_ADR >> 16), (D_DD_PCIE_1_PIO_ADR >> 16)};

/** PCIe IP mode */
volatile UCHAR	gDD_PCIE_RC_RE_TYPE = E_DD_PCIE_RE_TYPE_US;	/**< Type Undettled */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#ifndef D_DEBUG_PRINT_INFO_ON
#define dd_pcie_debug_print(arg)
#else
#define dd_pcie_debug_print(arg)		{DDIM_User_Printf arg;}
#endif	// D_DEBUG_PRINT_INFO_ON

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief	Initialize PCIe Root Complex.
 * @param	volatile struct io_pcie* io_pcie
 * @param	E_DD_PCIE_BIFURCATION bifurcation
 */
static VOID dd_pcie_rc_init(volatile struct io_pcie* io_pcie, volatile union io_pcie_core* io_pcie_core, E_DD_PCIE_BIFURCATION bifurcation)
{
	// 1 Assert all PHY / LINK resets
	io_pcie->RESET_CONTROL_1.word	= 0x00000000;
	io_pcie->RESET_CONTROL_2.word	= 0x00000000;
	io_pcie->RESET_SELECT_1.word	= 0x31111000;
	io_pcie->RESET_SELECT_2.word	= 0x00000111;

	// 2 Set P<n>_app_ltssm_enable='0' for reprogramming before linkup.
	io_pcie->CORE_CONTROL.bit.APP_LTSSM_ENABLE = 0;

	// 3 Set device_type (RC)
	io_pcie->CORE_CONTROL.bit.DEVICE_TYPE = 4;	// Root Port of PCI Express Root Complex

	if(bifurcation == E_DD_PCIE_BIFURCATION_DISABLE){
		// 4 pipe_port_sel=2'b01 ( Disable )
		io_pcie->PHY_CONFIG_COM_6.bit.PIPE_PORT_SEL = 1;
	}
	else{
		// 4 pipe_port_sel=2'b10 ( Enable )
		io_pcie->PHY_CONFIG_COM_6.bit.PIPE_PORT_SEL = 2;
	}

	// 5 Supply Reference (It has executed)
	// 6 Wait for 10usec (Reference Clocks is stable)
	Dd_ARM_Wait_ns( 10000 );

	// 7 De assert PERST#
	io_pcie->RESET_CONTROL_1.word = 0x00000030;

	// 8 Assert SYS_AUX_PWR_DET
	io_pcie->PM_CONTROL_2.bit.SYS_AUX_PWR_DET = 1;

	// 9 Supply following clocks
	io_pcie->AXI_CLK_STOP.word = 0x00000222;
	Dd_Top_Set_CLKSTOP2_PCISUPPCK(2);	// Sub clock of PCI is supplied.

	// 10 De assert PHY reset
	// 11 De assert LINK's PMC reset
	io_pcie->RESET_CONTROL_1.word = 0x00000033;

	// 12 PHY auto
	// 13 Wrapper auto
	// 14-17 PHY auto
	// 18 Wrapper auto

	// 19 Update registers through DBI AXI Slave interface
	io_pcie_core->RC.T1CSH.STATUS_COMMAND.bit.IOS = 1;	// Allow access response of IO space.
	io_pcie_core->RC.T1CSH.STATUS_COMMAND.bit.MS  = 1;	// Allow access response of Memory space.
	io_pcie_core->RC.T1CSH.STATUS_COMMAND.bit.BME = 1;	// Device issues the Memory/IO Request. (including MSI)
	io_pcie_core->RC.PCIECR.DCSR.bit.MRRS = 1;	// 256Byte max Read Request size
	io_pcie_core->RC.PCIECR.DCSR.bit.MPS  = 2;	// 512Byte max Payload size
	io_pcie_core->RC.PPL.MC1.bit.DRWE = 1;
	io_pcie_core->RC.PCIECR.LCR.bit.LBNC = 1;	// PCIE_CAP_LINK_BW_NOT_CAP
	io_pcie_core->RC.PPL.MC1.bit.DRWE = 0;

	// 20 Set P<n>_app_ltssm_enable='1'
	io_pcie->CORE_CONTROL.bit.APP_LTSSM_ENABLE = 1;
}

/**
 * @brief	Check configration read and write parameter.
 * @param	E_DD_PCIE_CH	ch
 * @param	USHORT			offset
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR/D_DD_PCIE_COMM_ERROR
 */
static INT32 dd_pcie_rc_chk_config_rw_par(E_DD_PCIE_CH ch, USHORT offset)
{
	if((offset % 4) != 0){	// Word boundary violation
		return D_DD_PCIE_INPUT_PARAM_ERROR;
	}

	if(Dd_Pcie_Rc_Get_Link_Status(ch) == FALSE){	// Not link up
		return D_DD_PCIE_COMM_ERROR;
	}

	return D_DDIM_OK;
}

/**
 * @brief	Check EXSTOP PCIe register access parameter.
 * @param	E_DD_PCIE_CH	ch
 * @param	USHORT			offset
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR/D_DD_PCIE_ACCESS_ERR
 */
static INT32 dd_pcie_rc_chk_exs_reg_par(E_DD_PCIE_CH ch, USHORT offset)
{
	// Word boundary check.
	if((offset % 4) != 0){
		return D_DD_PCIE_INPUT_PARAM_ERROR;
	}

	// Check on reserved address.
	if((offset >= 0x0060 && offset <= 0x00FF) ||
	   (offset >= 0x0118 && offset <= 0x01D7) ||
	   (offset == 0x0204) ||
	   (offset == 0x0214) ||
	   (offset == 0x0234) ||
	   (offset >= 0x0288)){
		return D_DD_PCIE_INPUT_PARAM_ERROR;
	}

	return D_DDIM_OK;
}

/**
 * @brief	Check DBI AXI slave register access parameter.
 * @param	E_DD_PCIE_CH	ch
 * @param	USHORT			offset
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR/D_DD_PCIE_ACCESS_ERR
 */
static INT32 dd_pcie_rc_chk_dbi_reg_par(E_DD_PCIE_CH ch, USHORT offset)
{
	// Word boundary check.
	if((offset % 4) != 0){
		return D_DD_PCIE_INPUT_PARAM_ERROR;
	}

	// Check on reserved address. Access Type = DBI(Normal Register Access).
	if((offset >= 0x0010 && offset <= 0x0017) ||
	   (offset >= 0x0048 && offset <= 0x004F) ||
	   (offset >= 0x0068 && offset <= 0x006F) ||
	   (offset >= 0x00A4 && offset <= 0x00AF) ||
	   (offset >= 0x00B0 && offset <= 0x00FF) ||
	   (offset >= 0x0160 && offset <= 0x06FF) ||
	   (offset >= 0x0754 && offset <= 0x080B) ||
	   (offset >= 0x0818 && offset <= 0x081F) ||
	   (offset >= 0x0834 && offset <= 0x0887) ||
	   (offset >= 0x088C && offset <= 0x08B7) ||
	   (offset >= 0x08C8 && offset <= 0x08CB) ||
	   (offset >= 0x08DC && offset <= 0x08DF) ||
	   (offset >= 0x08EC && offset <= 0x08EF) ||
	   (offset >= 0x08F8 && offset <= 0x08FF) ||
	   (offset >= 0x0920 && offset <= 0x096F) ||
	   (offset >= 0x0974 && offset <= 0x0977) ||
	   (offset >= 0x0984 && offset <= 0x0987) ||
	   (offset >= 0x0990 && offset <= 0x099B) ||
	   (offset >= 0x09A4 && offset <= 0x09A7) ||
	   (offset >= 0x09B0 && offset <= 0x09BB) ||
	   (offset >= 0x09C0 && offset <= 0x09C3) ||
	   (offset >= 0x09F0 && offset <= 0x09FF) ||
	   (offset >= 0x0A04 && offset <= 0x0A0F) ||
	   (offset >= 0x0A14 && offset <= 0x0A17) ||
	   (offset >= 0x0A20 && offset <= 0x0A23) ||
	   (offset >= 0x0A2C && offset <= 0x0A33) ||
	   (offset >= 0x0A38 && offset <= 0x0A3B) ||
	   (offset >= 0x0A5C && offset <= 0x0A6B) ||
	   (offset >= 0x0A74 && offset <= 0x0A77) ||
	   (offset >= 0x0A94 && offset <= 0x0B2F) ||
	   (offset >= 0x0B34 && offset <= 0x0B3F) ||
	   (offset >= 0x0B48 && offset <= 0x0FFF)){
		return D_DD_PCIE_INPUT_PARAM_ERROR;
	}

	if(ch == E_DD_PCIE_CH0){
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){
			// PCIe power down
			return D_DD_PCIE_ACCESS_ERR;
		}
	}
	else{
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE1() != 0){
			return D_DD_PCIE_ACCESS_ERR;
		}
	}

	return D_DDIM_OK;
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief	Initialize PCIe Root Complex.
 * @param	E_DD_PCIE_BIFURCATION	bifurcation
 */
VOID Dd_Pcie_Rc_Init(E_DD_PCIE_BIFURCATION	bifurcation)
{
	Dd_Top_Set_PERSEL3_PRT0SCK3(0);		// PXRSTX0
	Dd_Top_Set_PERSEL3_PXW0WP12(0);		// PXWAKE0
	Dd_Top_Set_PERSEL3_PXC0SCK7(0);		// PXCLKREQ0
	Dd_Top_Set_CLKSTOP2_EXSAX(2);		// AXI bus clock of EXS is supplied

	Dd_Exs_Set_PCIE0_BUS_CLOCK_Stop_Ctrl(0);	// Output PCIE0 BUS CLOCK
	Dd_Exs_Set_RAM_PD_Ctrl_PCIE0(0);			// Power-down mode is released

	gDD_PCIE_RC_Bifurcation = bifurcation;

	dd_pcie_rc_init(&IO_PCIE0, &IO_PCIE0_CORE, bifurcation);

	if(bifurcation == E_DD_PCIE_BIFURCATION_ENABLE){
		Dd_Top_Set_PERSEL3_PRT1SOU3(0);		// PXRSTX1
		Dd_Top_Set_PERSEL3_PXW1SOU7(0);		// PXWAKE1
		Dd_Top_Set_PERSEL3_PXC1WP15(0);		// PXCLKREQ1

		Dd_Exs_Set_PCIE1_BUS_CLOCK_Stop_Ctrl(0);	// Output PCIE1 BUS CLOCK
		Dd_Exs_Set_RAM_PD_Ctrl_PCIE1(0);			// Power-down mode is released

		dd_pcie_rc_init(&IO_PCIE1, &IO_PCIE1_CORE, bifurcation);
	}

	gDD_PCIE_RC_RE_TYPE = E_DD_PCIE_RE_TYPE_RC;
}

/**
 * @brief	PCIe End.
 */
VOID Dd_Pcie_Rc_Deinit(VOID)
{
	UCHAR	ch;

	// Global data reset
	gDD_PCIE_RC_RE_TYPE = E_DD_PCIE_RE_TYPE_US;
	gDD_PCIE_RC_Bifurcation = E_DD_PCIE_BIFURCATION_DISABLE;

	for(ch = 0 ; ch < D_DD_PCIE_RC_MAX_CH ; ch++){
		gDD_PCIE_RC_Int_Dma_Cb[ch].wr_cb = NULL;
		gDD_PCIE_RC_Int_Dma_Cb[ch].rd_cb = NULL;

		gDD_PCIE_RC_Int_Msi_Cb[ch] = NULL;

		gDD_PCIE_RC_Int_Trs_Ics_msi_Cb[ch].cfg_link_auto_bw_int_cb	= NULL;
		gDD_PCIE_RC_Int_Trs_Ics_msi_Cb[ch].cfg_bw_mgt_int_cb		= NULL;
		gDD_PCIE_RC_Int_Trs_Ics_msi_Cb[ch].smlh_req_rst_not_cb		= NULL;

		gDD_PCIE_RC_Int_Sys_Err_Cb[ch].cfg_aer_rc_err_cb = NULL;
		gDD_PCIE_RC_Int_Sys_Err_Cb[ch].cfg_sys_err_rc_cb = NULL;

		gDD_PCIE_RC_Int_Ep_Cb[ch].radm_intx_cb				= NULL;
		gDD_PCIE_RC_Int_Ep_Cb[ch].radm_pm_pme_cb			= NULL;
		gDD_PCIE_RC_Int_Ep_Cb[ch].radm_correctable_err_cb	= NULL;
		gDD_PCIE_RC_Int_Ep_Cb[ch].radm_nonfatal_err_cb		= NULL;
		gDD_PCIE_RC_Int_Ep_Cb[ch].radm_fatal_err_cb		= NULL;
		gDD_PCIE_RC_Int_Ep_Cb[ch].radm_msg_ltr_cb			= NULL;
		gDD_PCIE_RC_Int_Ep_Cb[ch].radm_pm_to_ack_cb		= NULL;
		gDD_PCIE_RC_Int_Ep_Cb[ch].radm_vendor_msg_cb		= NULL;

		gDD_PCIE_RC_Int_Own_Cb[ch].cfg_pme_cb		= NULL;
		gDD_PCIE_RC_Int_Own_Cb[ch].hp_cb			= NULL;
		gDD_PCIE_RC_Int_Own_Cb[ch].rdlh_link_up_cb	= NULL;
		gDD_PCIE_RC_Int_Own_Cb[ch].wake_cb			= NULL;

		gDD_PCIE_RC_Hp_Detect_Card[ch] = NULL;

		gDD_PCIE_RC_Config_ADR[ch] = (D_DD_PCIE_0_PIO_ADR >> 16);
	}

	Dd_Exs_Set_RAM_PD_Ctrl_PCIE0(1);	// PCIE0 power down.
	Dd_Exs_Set_RAM_PD_Ctrl_PCIE1(1);	// PCIE1 power down.
	Dd_Top_Set_CLKSTOP2_PCISUPPCK(3);	// Sub clock of PCI is stopped.
}

/**
 * @brief	Execute exclusive control for PCIe channel.
 * @param	INT32	tmout
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR/D_DD_PCIE_SEM_TIMEOUT/D_DD_PCIE_SEM_NG
 */
INT32 Dd_Pcie_Rc_Open(INT32 tmout)
{
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if (tmout < D_DDIM_USER_SEM_WAIT_FEVR) {
		Ddim_Assertion(("PCIE: input param error. tmout = %x\n", tmout));
		return D_DD_PCIE_INPUT_PARAM_ERROR;
	}
#endif

	if(tmout == D_DDIM_USER_SEM_WAIT_POL){
		ercd = DDIM_User_Pol_Sem(SID_DD_PCIE);
	}
	else{
		ercd = DDIM_User_Twai_Sem(SID_DD_PCIE, (DDIM_USER_TMO)tmout);
	}

	switch (ercd){
		case D_DDIM_USER_E_OK:
			return D_DDIM_OK;
		case D_DDIM_USER_E_TMOUT:
			return D_DD_PCIE_SEM_TIMEOUT;
		default:
			return D_DD_PCIE_SEM_NG;
	}
}

/**
 * @brief	Cancel exclusive control for PCIe channel.
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_SEM_NG
 */
INT32 Dd_Pcie_Rc_Close(VOID)
{
	DDIM_USER_ER	ercd;

	ercd = DDIM_User_Sig_Sem(SID_DD_PCIE);	// Exclusive release
	if(D_DDIM_USER_E_OK != ercd){
		return D_DD_PCIE_SEM_NG;
	}
	else{
		return D_DDIM_OK;
	}
}

/**
 * @brief	Get PCIe link status.
 * @param	E_DD_PCIE_CH		ch
 * @return	TRUE/FALSE
 */
BOOL Dd_Pcie_Rc_Get_Link_Status(E_DD_PCIE_CH ch)
{
	BOOL link_status;

	if(ch == E_DD_PCIE_CH0){
		link_status = (BOOL)IO_PCIE0.LINK_MONITOR.bit.SMLH_LINK_UP;
	}
	else{	// PCIe ch1
		link_status = (BOOL)IO_PCIE1.LINK_MONITOR.bit.SMLH_LINK_UP;
	}

	return link_status;
}

/**
 * @brief	It will be setting up the transfer of config.
 * @param	T_DD_PCIE_RC_CTRL_CFG const* const pcie_ctrl_cfg
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR/D_DD_PCIE_COMM_ERROR
 */
INT32 Dd_Pcie_Rc_Ctrl_Config(T_DD_PCIE_RC_CTRL_CFG const* const pcie_ctrl_cfg)
{
#ifdef CO_PARAM_CHECK
	if(pcie_ctrl_cfg == NULL){
		return D_DD_PCIE_INPUT_PARAM_ERROR;
	}
#endif

	if(Dd_Pcie_Rc_Get_Link_Status(pcie_ctrl_cfg->ch) == FALSE){	// Not link up
		return D_DD_PCIE_COMM_ERROR;
	}

	if(pcie_ctrl_cfg->ch == E_DD_PCIE_CH0){
		IO_PCIE0_CORE.RC.PPL.IV.word			= 0x00000000;
		IO_PCIE0_CORE.RC.PPL.IRC1.word			= 0x00000004;						// define the type of the region to be CFG0
		IO_PCIE0_CORE.RC.PPL.ILBA.word			= pcie_ctrl_cfg->base_adr;			// set the Lower Base Address
		gDD_PCIE_RC_Config_ADR[E_DD_PCIE_CH0]	= IO_PCIE0_CORE.RC.PPL.ILBA.bit.LBR;
		IO_PCIE0_CORE.RC.PPL.IUBA.word			= 0x00000000;						// set the Upper Base Address
		IO_PCIE0_CORE.RC.PPL.ILA.word			= pcie_ctrl_cfg->limit_adr;			// set the Limit Address
		IO_PCIE0_CORE.RC.PPL.ILTA.OB.bit.LTRO	= 0x00000000;						// set the Lower Target Address
		IO_PCIE0_CORE.RC.PPL.IUTA.bit.UTR		= 0x00000000;						// set the Upper Target Address
		IO_PCIE0_CORE.RC.PPL.IRC2.word			= 0x80000000;						// enable the region in routing id match mode
	}
	else{	// PCIe ch1
		IO_PCIE0_CORE.RC.PPL.IV.word			= 0x00000000;
		IO_PCIE1_CORE.RC.PPL.IRC1.word			= 0x00000004;						// define the type of the region to be CFG0
		IO_PCIE1_CORE.RC.PPL.ILBA.word			= pcie_ctrl_cfg->base_adr;			// set the Lower Base Address
		gDD_PCIE_RC_Config_ADR[E_DD_PCIE_CH1]	= IO_PCIE1_CORE.RC.PPL.ILBA.bit.LBR;
		IO_PCIE1_CORE.RC.PPL.IUBA.word			= 0x00000000;						// set the Upper Base Address
		IO_PCIE1_CORE.RC.PPL.ILA.word			= pcie_ctrl_cfg->limit_adr;			// set the Limit Address
		IO_PCIE1_CORE.RC.PPL.ILTA.OB.bit.LTRO	= 0x00000000;						// set the Lower Target Address
		IO_PCIE1_CORE.RC.PPL.IUTA.bit.UTR		= 0x00000000;						// set the Upper Target Address
		IO_PCIE1_CORE.RC.PPL.IRC2.word			= 0x80000000;						// enable the region in routing id match mode
	}

	return D_DDIM_OK;
}

/**
 * @brief	It will be setting up the dma transfer.
 * @param	T_DD_PCIE_CTRL_DMA const* const	pcie_ctrl_dma
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Rc_Ctrl_Dma(T_DD_PCIE_CTRL_DMA const* const pcie_ctrl_dma)
{
#ifdef CO_PARAM_CHECK
	if(pcie_ctrl_dma == NULL){
		return D_DD_PCIE_INPUT_PARAM_ERROR;
	}
#endif

	if(pcie_ctrl_dma->ch == E_DD_PCIE_CH0){
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){
			return D_DD_PCIE_ACCESS_ERR;
		}

		IO_PCIE0_CORE.RC.PPL.DWIC.bit.WDIC = 0xFF;	// Interrupt clear
		IO_PCIE0_CORE.RC.PPL.DWIC.bit.WAIC = 0xFF;	// Interrupt clear
		IO_PCIE0_CORE.RC.PPL.DRIC.bit.RDIC = 0xFF;	// Interrupt clear
		IO_PCIE0_CORE.RC.PPL.DRIC.bit.RAIC = 0xFF;	// Interrupt clear

		if(pcie_ctrl_dma->direction == E_DD_PCIE_DMA_DIR_STOD){
			IO_PCIE0_CORE.RC.PPL.DWEE.bit.DWE = 1;	// Write Engine Enable
			IO_PCIE0_CORE.RC.PPL.DREE.bit.DRE = 0;	// Read Engine Disable
			gDD_PCIE_RC_Int_Dma_Cb[pcie_ctrl_dma->ch].wr_cb = pcie_ctrl_dma->int_dma_cb;		// interrupt dma callback function
		}
		else{
			IO_PCIE0_CORE.RC.PPL.DWEE.bit.DWE = 0;	// Write Engine Disable
			IO_PCIE0_CORE.RC.PPL.DREE.bit.DRE = 1;	// Read Engine Enable
			gDD_PCIE_RC_Int_Dma_Cb[pcie_ctrl_dma->ch].rd_cb = pcie_ctrl_dma->int_dma_cb;		// interrupt dma callback function
		}

		IO_PCIE0_CORE.RC.PPL.DVS.bit.CN = pcie_ctrl_dma->dma_ch;		// Channel Number
		IO_PCIE0_CORE.RC.PPL.DVS.bit.CD = ~pcie_ctrl_dma->direction;	// Direction

		if(pcie_ctrl_dma->mode == E_DD_PCIE_DMA_DEMAND_MODE){	// dmac mode control
			IO_PCIE0_CORE.RC.PPL.DCC1.word = 0x00000008;	// Local Interrupt Enable

			IO_PCIE0_CORE.RC.PPL.DTS.bit.DTS  = pcie_ctrl_dma->dmnd_trans_size;	// Transfer Size
			IO_PCIE0_CORE.RC.PPL.DSL.bit.SARL = pcie_ctrl_dma->dmnd_src_addr_l;	// Source Address Low
			IO_PCIE0_CORE.RC.PPL.DSH.bit.SARH = pcie_ctrl_dma->dmnd_src_addr_h;	// Source Address Hight
			IO_PCIE0_CORE.RC.PPL.DDL.bit.DARL = pcie_ctrl_dma->dmnd_dst_addr_l;	// Destination Address Low
			IO_PCIE0_CORE.RC.PPL.DDH.bit.DARH = pcie_ctrl_dma->dmnd_dst_addr_h;	// Destination Address Hight
		}
		else{	// Linked List mode
			if(pcie_ctrl_dma->direction == E_DD_PCIE_DMA_DIR_STOD){
				IO_PCIE0_CORE.RC.PPL.DWLLEE.bit.WCLLLAIE = 1;	// Write Channel LL Local Abort Interrupt Enable.
				IO_PCIE0_CORE.RC.PPL.DWLLEE.bit.WCLLRAIE = 0;	// Read Channel LL Local Abort Interrupt Disable.
			}
			else{
				IO_PCIE0_CORE.RC.PPL.DWLLEE.bit.WCLLLAIE = 0;	// Write Channel LL Local Abort Interrupt Disable.
				IO_PCIE0_CORE.RC.PPL.DWLLEE.bit.WCLLRAIE = 1;	// Read Channel LL Local Abort Interrupt Enable.
			}

			// DMA Channel Control 1 register
			// Linked List Enable (LLE) = 1
			// Consumer Cycle State (CCS) = 1
			// Traffic Digest TLP Header Bit(TD) = 1
			IO_PCIE0_CORE.RC.PPL.DCC1.word = 0x04000300;

			IO_PCIE0_CORE.RC.PPL.DLL.bit.LL = pcie_ctrl_dma->ll_addr;	// Linked List Pointer Low
		}

		IO_PCIE0_CORE.RC.PPL.DWIM.bit.WDIM = 0;	// Write Done Interrupt Mask Off
		IO_PCIE0_CORE.RC.PPL.DWIM.bit.WAIM = 0;	// Write Abort Interrupt Mask Off
		IO_PCIE0_CORE.RC.PPL.DRIM.bit.RDIM = 0;	// Read Done Interrupt Mask Off
		IO_PCIE0_CORE.RC.PPL.DRIM.bit.RAIM = 0;	// Read Abort Interrupt Mask Off
	}
	else{	// PCIe ch1
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE1() != 0){
			return D_DD_PCIE_ACCESS_ERR;
		}

		IO_PCIE1_CORE.RC.PPL.DWIC.bit.WDIC = 0xFF;	// Interrupt clear
		IO_PCIE1_CORE.RC.PPL.DWIC.bit.WAIC = 0xFF;	// Interrupt clear
		IO_PCIE1_CORE.RC.PPL.DRIC.bit.RDIC = 0xFF;	// Interrupt clear
		IO_PCIE1_CORE.RC.PPL.DRIC.bit.RAIC = 0xFF;	// Interrupt clear

		if(pcie_ctrl_dma->direction == E_DD_PCIE_DMA_DIR_STOD){
			IO_PCIE1_CORE.RC.PPL.DWEE.bit.DWE = 1;	// Write Engine Enable
			IO_PCIE1_CORE.RC.PPL.DREE.bit.DRE = 0;	// Read Engine Disable
			gDD_PCIE_RC_Int_Dma_Cb[pcie_ctrl_dma->ch].wr_cb = pcie_ctrl_dma->int_dma_cb;		// interrupt dma callback function
		}
		else{
			IO_PCIE1_CORE.RC.PPL.DWEE.bit.DWE = 0;	// Write Engine Disable
			IO_PCIE1_CORE.RC.PPL.DREE.bit.DRE = 1;	// Read Engine Enable
			gDD_PCIE_RC_Int_Dma_Cb[pcie_ctrl_dma->ch].rd_cb = pcie_ctrl_dma->int_dma_cb;		// interrupt dma callback function
		}

		IO_PCIE1_CORE.RC.PPL.DVS.bit.CN = pcie_ctrl_dma->dma_ch;		// Channel Number
		IO_PCIE1_CORE.RC.PPL.DVS.bit.CD = ~pcie_ctrl_dma->direction;	// Direction

		if(pcie_ctrl_dma->mode == E_DD_PCIE_DMA_DEMAND_MODE){	// dmac mode control
			IO_PCIE1_CORE.RC.PPL.DCC1.word = 0x00000008;	// Local Interrupt Enable

			IO_PCIE1_CORE.RC.PPL.DTS.bit.DTS  = pcie_ctrl_dma->dmnd_trans_size;	// Transfer Size
			IO_PCIE1_CORE.RC.PPL.DSL.bit.SARL = pcie_ctrl_dma->dmnd_src_addr_l;	// Source Address Low
			IO_PCIE1_CORE.RC.PPL.DSH.bit.SARH = pcie_ctrl_dma->dmnd_src_addr_h;	// Source Address Hight
			IO_PCIE1_CORE.RC.PPL.DDL.bit.DARL = pcie_ctrl_dma->dmnd_dst_addr_l;	// Destination Address Low
			IO_PCIE1_CORE.RC.PPL.DDH.bit.DARH = pcie_ctrl_dma->dmnd_dst_addr_h;	// Destination Address Hight
		}
		else{
			if(pcie_ctrl_dma->direction == E_DD_PCIE_DMA_DIR_STOD){
				IO_PCIE1_CORE.RC.PPL.DWLLEE.bit.WCLLLAIE = 1;	// Write Channel LL Local Abort Interrupt Enable.
				IO_PCIE1_CORE.RC.PPL.DWLLEE.bit.WCLLRAIE = 0;	// Read Channel LL Local Abort Interrupt Disable.
			}
			else{
				IO_PCIE1_CORE.RC.PPL.DWLLEE.bit.WCLLLAIE = 0;	// Write Channel LL Local Abort Interrupt Disable.
				IO_PCIE1_CORE.RC.PPL.DWLLEE.bit.WCLLRAIE = 1;	// Read Channel LL Local Abort Interrupt Enable.
			}

			// DMA Channel Control 1 register
			// Linked List Enable (LLE) = 1
			// Consumer Cycle State (CCS) = 1
			// Traffic Digest TLP Header Bit(TD) = 1
			IO_PCIE1_CORE.RC.PPL.DCC1.word = 0x04000300;

			IO_PCIE1_CORE.RC.PPL.DLL.bit.LL = pcie_ctrl_dma->ll_addr;	// Linked List Pointer Low
		}

		IO_PCIE1_CORE.RC.PPL.DWIM.bit.WDIM = 0;	// Write Done Interrupt Mask Off
		IO_PCIE1_CORE.RC.PPL.DWIM.bit.WAIM = 0;	// Write Abort Interrupt Mask Off
		IO_PCIE1_CORE.RC.PPL.DRIM.bit.RDIM = 0;	// Read Done Interrupt Mask Off
		IO_PCIE1_CORE.RC.PPL.DRIM.bit.RAIM = 0;	// Read Abort Interrupt Mask Off
	}

	return D_DDIM_OK;
}

/**
 * @brief	Start DMA.
 * @param	E_DD_PCIE_CH		ch
 * @param	E_DD_PCIE_DMA_CH	dma_ch
 * @param	E_DD_PCIE_DMA_DIR	dir
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Rc_Start_Dma(E_DD_PCIE_CH ch, E_DD_PCIE_DMA_CH dma_ch, E_DD_PCIE_DMA_DIR dir)
{
	if(ch == E_DD_PCIE_CH0){
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){
			return D_DD_PCIE_ACCESS_ERR;
		}

		if(dir == E_DD_PCIE_DMA_DIR_STOD){	// source to destination (write)
			IO_PCIE0_CORE.RC.PPL.DWD.bit.WDN	= dma_ch;
			IO_PCIE0_CORE.RC.PPL.DWD.bit.WS		= 0;
		}
		else{								// destination to source (read)
			IO_PCIE0_CORE.RC.PPL.DRD.bit.RDN	= dma_ch;
			IO_PCIE0_CORE.RC.PPL.DRD.bit.RS		= 0;
		}
	}
	else{	// PCIe ch1
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE1() != 0){
			return D_DD_PCIE_ACCESS_ERR;
		}

		if(dir == E_DD_PCIE_DMA_DIR_STOD){	// source to destination (write)
			IO_PCIE1_CORE.RC.PPL.DWD.bit.WDN	= dma_ch;
			IO_PCIE1_CORE.RC.PPL.DWD.bit.WS		= 0;
		}
		else{								// destination to source (read)
			IO_PCIE1_CORE.RC.PPL.DRD.bit.RDN	= dma_ch;
			IO_PCIE1_CORE.RC.PPL.DRD.bit.RS		= 0;
		}
	}

	return D_DDIM_OK;
}

/**
 * @brief	Stop DMA.
 * @param	E_DD_PCIE_CH		ch
 * @param	E_DD_PCIE_DMA_CH	dma_ch
 * @param	E_DD_PCIE_DMA_DIR	dir
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Rc_Stop_Dma(E_DD_PCIE_CH ch, E_DD_PCIE_DMA_CH dma_ch, E_DD_PCIE_DMA_DIR dir)
{
	if(ch == E_DD_PCIE_CH0){
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){
			return D_DD_PCIE_ACCESS_ERR;
		}

		if(dir == E_DD_PCIE_DMA_DIR_STOD){	// source to destination (write)
			IO_PCIE0_CORE.RC.PPL.DWD.bit.WDN	= dma_ch;
			IO_PCIE0_CORE.RC.PPL.DWD.bit.WS		= 1;

			IO_PCIE0_CORE.RC.PPL.DWIC.bit.WDIC = 0xFF;	// Interrupt clear
			IO_PCIE0_CORE.RC.PPL.DWIC.bit.WAIC = 0xFF;	// Interrupt clear
		}
		else{								// destination to source (read)
			IO_PCIE0_CORE.RC.PPL.DRD.bit.RDN	= dma_ch;
			IO_PCIE0_CORE.RC.PPL.DRD.bit.RS		= 1;

			IO_PCIE0_CORE.RC.PPL.DRIC.bit.RDIC = 0xFF;	// Interrupt clear
			IO_PCIE0_CORE.RC.PPL.DRIC.bit.RAIC = 0xFF;	// Interrupt clear
		}
	}
	else{	// PCIe ch1
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE1() != 0){
			return D_DD_PCIE_ACCESS_ERR;
		}

		if(dir == E_DD_PCIE_DMA_DIR_STOD){	// source to destination (write)
			IO_PCIE1_CORE.RC.PPL.DWD.bit.WDN	= dma_ch;
			IO_PCIE1_CORE.RC.PPL.DWD.bit.WS		= 1;

			IO_PCIE1_CORE.RC.PPL.DWIC.bit.WDIC = 0xFF;	// Interrupt clear
			IO_PCIE1_CORE.RC.PPL.DWIC.bit.WAIC = 0xFF;	// Interrupt clear
		}
		else{								// destination to source (read)
			IO_PCIE1_CORE.RC.PPL.DRD.bit.RDN	= dma_ch;
			IO_PCIE1_CORE.RC.PPL.DRD.bit.RS		= 1;

			IO_PCIE1_CORE.RC.PPL.DRIC.bit.RDIC = 0xFF;	// Interrupt clear
			IO_PCIE1_CORE.RC.PPL.DRIC.bit.RAIC = 0xFF;	// Interrupt clear
		}
	}

	return D_DDIM_OK;
}

/**
 * @brief	It will be setting up the transfer of memory. CPU transfer.
 * @param	T_DD_PCIE_CTRL_MEM const* const pcie_ctrl_mem
 * @return	INT32 D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR/D_DD_PCIE_COMM_ERROR
 */
INT32 Dd_Pcie_Rc_Ctrl_Memory(T_DD_PCIE_CTRL_MEM const* const pcie_ctrl_mem)
{
#ifdef CO_PARAM_CHECK
	if(pcie_ctrl_mem == NULL){
		return D_DD_PCIE_INPUT_PARAM_ERROR;
	}
#endif

	if(Dd_Pcie_Rc_Get_Link_Status(pcie_ctrl_mem->ch) == FALSE){	// Not link up
		return D_DD_PCIE_COMM_ERROR;
	}

	if(pcie_ctrl_mem->ch == E_DD_PCIE_CH0){
		IO_PCIE0_CORE.RC.PPL.IV.word			= 0x00000001;
		IO_PCIE0_CORE.RC.PPL.ILBA.word			= pcie_ctrl_mem->base_adr;			// set the Lower Base Address
		IO_PCIE0_CORE.RC.PPL.IUBA.word			= 0x00000000;						// set the Upper Base Address
		IO_PCIE0_CORE.RC.PPL.ILA.word			= pcie_ctrl_mem->limit_adr;			// set the Limit Address
		IO_PCIE0_CORE.RC.PPL.ILTA.OB.bit.LTRO	= pcie_ctrl_mem->low_target_adr;	// set the Lower Target Address
		IO_PCIE0_CORE.RC.PPL.IUTA.bit.UTR		= pcie_ctrl_mem->upp_target_adr;	// set the Upper Target Address
		IO_PCIE0_CORE.RC.PPL.IRC1.word			= 0x00000000;						// define the type of the region to be MEM
		IO_PCIE0_CORE.RC.PPL.IRC2.word			= 0x80000000;						// enable the region in address match mode
	}
	else{	// PCIe ch1
		IO_PCIE1_CORE.RC.PPL.IV.word			= 0x00000001;
		IO_PCIE1_CORE.RC.PPL.ILBA.word			= pcie_ctrl_mem->base_adr;			// set the Lower Base Address
		IO_PCIE1_CORE.RC.PPL.IUBA.word			= 0x00000000;						// set the Upper Base Address
		IO_PCIE1_CORE.RC.PPL.ILA.word			= pcie_ctrl_mem->limit_adr;			// set the Limit Address
		IO_PCIE1_CORE.RC.PPL.ILTA.OB.bit.LTRO	= pcie_ctrl_mem->low_target_adr;	// set the Lower Target Address
		IO_PCIE1_CORE.RC.PPL.IUTA.bit.UTR		= pcie_ctrl_mem->upp_target_adr;	// set the Upper Target Address
		IO_PCIE1_CORE.RC.PPL.IRC1.word			= 0x00000000;						// define the type of the region to be MEM
		IO_PCIE1_CORE.RC.PPL.IRC2.word			= 0x80000000;						// enable the region in address match mode
	}

	return D_DDIM_OK;
}

/**
 * @brief	It will be setting up the interrupt MSI.
 * @param	T_DD_PCIE_RC_CTRL_MSI const* const pcie_ctrl_msi
 * @return	INT32 D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Rc_Ctrl_Msi(T_DD_PCIE_RC_CTRL_MSI const* const pcie_ctrl_msi)
{
#ifdef CO_PARAM_CHECK
	if(pcie_ctrl_msi == NULL){
		return D_DD_PCIE_INPUT_PARAM_ERROR;
	}
#endif

	if(pcie_ctrl_msi->ch == E_DD_PCIE_CH0){
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){
			return D_DD_PCIE_ACCESS_ERR;
		}

		IO_PCIE0_CORE.RC.PPL.MCI0S.bit.MCI0S		= 0xFFFFFFFF;	// interrupt clear
		IO_PCIE0_CORE.RC.PPL.MCA.bit.MCA			= pcie_ctrl_msi->cmp_adr;
		IO_PCIE0_CORE.RC.PPL.MCI0E.bit.MCI0E		= pcie_ctrl_msi->int_en;
		gDD_PCIE_RC_Int_Msi_Cb[pcie_ctrl_msi->ch]	= pcie_ctrl_msi->int_msi_cb;
		IO_PCIE0_CORE.RC.PPL.MCI0M.bit.MCI0M		= 0;	// Mask off
	}
	else{	// PCIe ch1
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE1() != 0){
			return D_DD_PCIE_ACCESS_ERR;
		}

		IO_PCIE0_CORE.RC.PPL.MCI0S.bit.MCI0S		= 0xFFFFFFFF;	// interrupt clear
		IO_PCIE1_CORE.RC.PPL.MCA.bit.MCA			= pcie_ctrl_msi->cmp_adr;
		IO_PCIE1_CORE.RC.PPL.MCI0E.bit.MCI0E		= pcie_ctrl_msi->int_en;
		gDD_PCIE_RC_Int_Msi_Cb[pcie_ctrl_msi->ch]	= pcie_ctrl_msi->int_msi_cb;
		IO_PCIE1_CORE.RC.PPL.MCI0M.bit.MCI0M		= 0;	// Mask off
	}

	return D_DDIM_OK;
}

/**
 * @brief	Interrupt handler for PCIe ch0 own.
 */
VOID Dd_Pcie_Rc_Ch0_Int_Own_Handler(VOID)
{
	VP_CALLBACK_PCIE callback;

	dd_pcie_debug_print(("[DD_PCIE_RC] CH0 INT_OWN Occurred.\n"));

	// cfg_pme_int/cfg_pme_msi
	if(IO_PCIE0_CORE.RC.PCIECR.RSR.bit.PMES == 1){
		dd_pcie_debug_print(("[DD_PCIE_RC] cfg_pme_int/cfg_pme_msi.\n"));
		IO_PCIE0_CORE.RC.PCIECR.RSR.bit.PMES = 1;	// Clear Interrupt
		callback = gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH0].cfg_pme_cb;
		if(callback != NULL){
			(*callback)(0);
		}
	}

	// hp_mpe/hp_int/hp_msi
	// PCIE_CAP_ATTENTION_BUTTON_PRESSED
	else if(IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.ABP == 1){
		dd_pcie_debug_print(("[DD_PCIE_RC] hp_mpe/hp_int/hp_msi: PCIE_CAP_ATTENTION_BUTTON_PRESSED.\n"));
		Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(E_DD_PCIE_CH0, TRUE);
		IO_PCIE0_CORE.RC.PCIECR.SCR.bit.ABP = 0;
		Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(E_DD_PCIE_CH0, FALSE);
		IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.ABP = 1;	// Clear Interrupt
		callback = gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH0].hp_cb;
		if(callback != NULL){
			(*callback)(E_DD_PCIE_HOT_PLUG_ST_ABP);
		}
	}

	// PCIE_CAP_POWER_FAULT_DETECTED
	else if(IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.PFD == 1){
		dd_pcie_debug_print(("[DD_PCIE_RC] hp_mpe/hp_int/hp_msi: PCIE_CAP_POWER_FAULT_DETECTED.\n"));
		Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(E_DD_PCIE_CH0, TRUE);
		IO_PCIE0_CORE.RC.PCIECR.SCR.bit.PCP = 0;
		Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(E_DD_PCIE_CH0, FALSE);
		IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.PFD = 1;	// Clear Interrupt
		callback = gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH0].hp_cb;
		if(callback != NULL){
			(*callback)(E_DD_PCIE_HOT_PLUG_ST_PFD);
		}
	}

	// PCIE_CAP_MRL_SENSOR_CHANGED
	else if(IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.MRLSC == 1){
		dd_pcie_debug_print(("[DD_PCIE_RC] hp_mpe/hp_int/hp_msi: PCIE_CAP_MRL_SENSOR_CHANGED.\n"));
		IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.MRLSC = 1;	// Clear Interrupt
		callback = gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH0].hp_cb;
		if(callback != NULL){
			(*callback)(E_DD_PCIE_HOT_PLUG_ST_MRLSC);
		}
	}

	// PCIE_CAP_PRESENCE_DETECTED_CHANGED
	else if(IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.PDC == 1){
		dd_pcie_debug_print(("[DD_PCIE_RC] hp_mpe/hp_int/hp_msi: PCIE_CAP_PRESENCE_DETECTED_CHANGED.\n"));
		IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.PDC = 1;	// Clear Interrupt
		callback = gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH0].hp_cb;
		if(callback != NULL){
			(*callback)(E_DD_PCIE_HOT_PLUG_ST_PDC);
		}
	}

	// PCIE_CAP_CMD_CPLD
	else if(IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.CC == 1){
		dd_pcie_debug_print(("[DD_PCIE_RC] hp_mpe/hp_int/hp_msi: PCIE_CAP_CMD_CPLD.\n"));
		IO_PCIE0.EM_CONTROL.bit.CMD_CPLED_INT = 0;
		IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.CC = 1;	// Clear Interrupt
		callback = gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH0].hp_cb;
		if(callback != NULL){
			(*callback)(E_DD_PCIE_HOT_PLUG_ST_CC);
		}
	}

	// PCIE_CAP_DLL_STATE_CHANGED
	else if(IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.DLLSC == 1){
		dd_pcie_debug_print(("[DD_PCIE_RC] hp_mpe/hp_int/hp_msi: PCIE_CAP_DLL_STATE_CHANGED.\n"));
		IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.DLLSC = 1;	// Clear Interrupt
		callback = gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH0].hp_cb;
		if(callback != NULL){
			(*callback)(E_DD_PCIE_HOT_PLUG_ST_DLLSC);
		}
	}

	// rdlh_link_up
	else if(IO_PCIE0.INT_CONTROL_2.bit.RDLH_LINK_UP_STT == 1){
		dd_pcie_debug_print(("[DD_PCIE_RC] rdlh_link_up.\n"));
		IO_PCIE0.INT_CONTROL_2.bit.RDLH_LINK_UP_STT = 1;	// Clear Interrupt
		callback = gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH0].rdlh_link_up_cb;
		if(callback != NULL){
			(*callback)(0);
		}
	}

	// wake
	else if(IO_PCIE0.INT_CONTROL_2.bit.WAKE_STATUS == 1){
		dd_pcie_debug_print(("[DD_PCIE_RC] wake.\n"));
		IO_PCIE0.INT_CONTROL_2.bit.WAKE_STATUS = 1;	// Clear Interrupt
		callback = gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH0].wake_cb;
		if(callback != NULL){
			(*callback)(0);
		}
	}
}

/**
 * @brief	Interrupt handler for PCIe ch0 endpoint.
 */
VOID Dd_Pcie_Rc_Ch0_Int_Ep_Handler(VOID)
{
	UINT32 int_stat;
	VP_CALLBACK_PCIE callback;

	dd_pcie_debug_print(("[DD_PCIE_RC] CH0 INT_EP Occurred.\n"));

	// radm_inta/radm_intb/radm_intc/radm_intd/
	if(IO_PCIE0.INT_CONTROL_1.word & 0xF0000000){
		int_stat = IO_PCIE0.INT_CONTROL_1.word;
		IO_PCIE0.INT_CONTROL_1.word &= int_stat;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] INTX_Status \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE0.INT_CONTROL_1.word, int_stat));

		callback = gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH0].radm_intx_cb;
		if(callback != NULL) {
			dd_pcie_debug_print(("[DD_PCIE_RC] INT_X callback\n"));
			(*callback)(int_stat >> 28);	// argument: 1(INTA), 2(INTB), 4(INTC), 8(INTD)
		}
	}

	// radm_pm_pme
	else if(IO_PCIE0.INT_CONTROL_3.bit.RADM_PM_PME_STT == 1){
		IO_PCIE0.INT_CONTROL_3.bit.RADM_PM_PME_STT = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] radm_pm_pme\n"));

		callback = gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH0].radm_pm_pme_cb;
		if(callback != NULL) {
			(*callback)(0);
		}
	}

	// radm_correctable_err
	else if(IO_PCIE0.INT_CONTROL_3.bit.RADM_COR_ERR_STT == 1){
		IO_PCIE0.INT_CONTROL_3.bit.RADM_COR_ERR_STT = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] radm_correctable_err\n"));

		callback = gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH0].radm_correctable_err_cb;
		if(callback != NULL) {
			(*callback)(0);
		}
	}

	// radm_nonfatal_err
	else if(IO_PCIE0.INT_CONTROL_3.bit.RADM_NF_ERR_STT == 1){
		IO_PCIE0.INT_CONTROL_3.bit.RADM_NF_ERR_STT = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] radm_nonfatal_err\n"));

		callback = gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH0].radm_nonfatal_err_cb;
		if(callback != NULL) {
			(*callback)(0);
		}
	}

	// radm_fatal_err
	else if(IO_PCIE0.INT_CONTROL_3.bit.RADM_F_ERR_STT == 1){
		IO_PCIE0.INT_CONTROL_3.bit.RADM_F_ERR_STT = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] radm_fatal_err\n"));

		callback = gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH0].radm_fatal_err_cb;
		if(callback != NULL) {
			(*callback)(0);
		}
	}

	// radm_pm_to_ack
	else if(IO_PCIE0.INT_CONTROL_3.bit.RADM_PM2ACK_STT == 1){
		IO_PCIE0.INT_CONTROL_3.bit.RADM_PM2ACK_STT = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] radm_pm_to_ack\n"));

		callback = gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH0].radm_pm_to_ack_cb;
		if(callback != NULL) {
			(*callback)(0);
		}
	}

	// radm_vendor_msg
	else if(IO_PCIE0.INT_CONTROL_2.bit.VENDOR_MSG_STT == 1){
		IO_PCIE0.INT_CONTROL_2.bit.VENDOR_MSG_STT = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] radm_vendor_msg\n"));

		callback = gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH0].radm_vendor_msg_cb;
		if(callback != NULL) {
			(*callback)(0);
		}
	}

	// radm_msg_ltr
	else if(IO_PCIE0.INT_CONTROL_3.bit.RADM_MSG_LTR_STT){	// LTR Interrupt Status
		IO_PCIE0.INT_CONTROL_3.bit.RADM_MSG_LTR_STT = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] radm_msg_ltr\n"));

		callback = gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH0].radm_msg_ltr_cb;
		if(callback != NULL) {
			(*callback)(0);
		}
	}
}

/**
 * @brief	Interrupt handler for PCIe ch0 system error.
 */
VOID Dd_Pcie_Rc_Ch0_Int_Sys_Err_Handler(VOID)
{
	UINT32 int_stat;
	VP_CALLBACK_PCIE callback;

	dd_pcie_debug_print(("[DD_PCIE_RC] CH0 INT_SYS_ERR Occurred.\n"));

	// cfg_aer_rc_err_int/cfg_aer_rc_err_msi
	if((IO_PCIE0_CORE.RC.AERC.RESR.word & 0x0000007F) != 0){
		int_stat = IO_PCIE0_CORE.RC.AERC.RESR.word;
		IO_PCIE0_CORE.RC.AERC.RESR.word &= 0x0000007F;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] cfg_aer_rc_err \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE0_CORE.RC.AERC.RESR.word, int_stat));

		callback = gDD_PCIE_RC_Int_Sys_Err_Cb[E_DD_PCIE_CH0].cfg_aer_rc_err_cb;
		if(callback != NULL){
			dd_pcie_debug_print(("[DD_PCIE_RC] cfg_aer_rc_err callback\n"));
			(*callback)(int_stat & 0x0000007F);
		}
	}

	// cfg_sys_err_rc
	else if(IO_PCIE0.INT_CONTROL_2.bit.SYS_ERR_RC_STT != 0){
		IO_PCIE0.INT_CONTROL_2.bit.SYS_ERR_RC_STT = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] cfg_sys_err_rc\n"));

		callback = gDD_PCIE_RC_Int_Sys_Err_Cb[E_DD_PCIE_CH0].cfg_sys_err_rc_cb;
		if(callback != NULL){
			dd_pcie_debug_print(("[DD_PCIE_RC] cfg_sys_err_rc callback\n"));
			(*callback)(0);
		}
	}
}

/**
 * @brief	Interrupt handler for PCIe ch0 dma.
 */
VOID Dd_Pcie_Rc_Ch0_Int_Dma_Handler(VOID)
{
	UINT32 status;
	VP_CALLBACK_PCIE callback;

	dd_pcie_debug_print(("[DD_PCIE_RC] CH0 INT_DMA Occurred.\n"));

	if(IO_PCIE0_CORE.RC.PPL.DWIS.word){				// DMA Write Interupt Status (Done and Abort)
		dd_pcie_debug_print(("[DD_PCIE_RC] DMA Write Interrupt Status \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE0_CORE.RC.PPL.DWIS.word, (UINT32)IO_PCIE0_CORE.RC.PPL.DWIS.word));

		// IO_PCIE0_CORE.RC.PPL.DWIS.word
		// 0x00000001  Done Interrupt status(DMA ch0)
		// 0x00000002  Done Interrupt status(DMA ch1)
		// 0x00010000  Abort Interrupt status(DMA ch0)
		// 0x00020000  Abort Interrupt status(DMA ch1)
		status = IO_PCIE0_CORE.RC.PPL.DWIS.word;

		IO_PCIE0_CORE.RC.PPL.DWIC.word = IO_PCIE0_CORE.RC.PPL.DWIS.word;	// Interrupt clear

		callback = gDD_PCIE_RC_Int_Dma_Cb[E_DD_PCIE_CH0].wr_cb;
		if(callback){
			dd_pcie_debug_print(("[DD_PCIE_RC] INT_DMA write callback\n"));
			(*callback)(status);
		}
	}
	else if(IO_PCIE0_CORE.RC.PPL.DRIS.word){		// DMA Read Interupt Status (Done and Abort)
		dd_pcie_debug_print(("[DD_PCIE_RC] DMA Read Interrupt Status \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE0_CORE.RC.PPL.DRIS.word, (UINT32)IO_PCIE0_CORE.RC.PPL.DRIS.word));

		// IO_PCIE0_CORE.RC.PPL.DRIS.word
		// 0x00000001  Done Interrupt status(DMA ch0)
		// 0x00000002  Done Interrupt status(DMA ch1)
		// 0x00010000  Abort Interrupt status(DMA ch0)
		// 0x00020000  Abort Interrupt status(DMA ch1)
		status = IO_PCIE0_CORE.RC.PPL.DRIS.word;

		IO_PCIE0_CORE.RC.PPL.DRIC.word = IO_PCIE0_CORE.RC.PPL.DRIS.word;	// Interrupt clear

		callback = gDD_PCIE_RC_Int_Dma_Cb[E_DD_PCIE_CH0].rd_cb;
		if(callback){
			dd_pcie_debug_print(("[DD_PCIE_RC] INT_DMA read callback\n"));
			(*callback)(status);
		}
	}
}

/**
 * @brief	Interrupt handler for PCIe ch0 Trs_Ics_Msi.
 */
VOID Dd_Pcie_Rc_Ch0_Int_Trs_Ics_Msi_Handler(VOID)
{
	UINT32 int_stat;
	VP_CALLBACK_PCIE callback;

	dd_pcie_debug_print(("[DD_PCIE_RC] CH0 INT_TRS_ICS_MSI Occurred.\n"));

	// msi_ctrl_int (MSI interrupt)
	if(IO_PCIE0_CORE.RC.PPL.MCI0S.bit.MCI0S != 0){
		int_stat = IO_PCIE0_CORE.RC.PPL.MCI0S.bit.MCI0S;
		IO_PCIE0_CORE.RC.PPL.MCI0S.bit.MCI0S = int_stat;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] msi_ctrl_int \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE0_CORE.RC.PPL.MCI0S.word, int_stat));

		callback = gDD_PCIE_RC_Int_Msi_Cb[E_DD_PCIE_CH0];
		if(callback != NULL){
			dd_pcie_debug_print(("[DD_PCIE_RC] msi_ctrl_int callback\n"));
			(*callback)(int_stat);
		}
	}

	// cfg_link_auto_bw_int
	else if(IO_PCIE0_CORE.RC.PCIECR.LCSR.bit.LABS != 0){
		int_stat = IO_PCIE0_CORE.RC.PCIECR.LCSR.word;
		IO_PCIE0_CORE.RC.PCIECR.LCSR.bit.LABS = 1;		// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] cfg_link_auto_bw_int \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE0_CORE.RC.PCIECR.LCSR.word, int_stat));

		callback = gDD_PCIE_RC_Int_Trs_Ics_msi_Cb[E_DD_PCIE_CH0].cfg_link_auto_bw_int_cb;
		if(callback != NULL){
			dd_pcie_debug_print(("[DD_PCIE_RC] cfg_link_auto_bw_int callback\n"));
			(*callback)(0);
		}
	}

	// cfg_bw_mgt_int
	else if(IO_PCIE0_CORE.RC.PCIECR.LCSR.bit.LBMS != 0){
		int_stat = IO_PCIE0_CORE.RC.PCIECR.LCSR.word;
		IO_PCIE0_CORE.RC.PCIECR.LCSR.bit.LBMS = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] cfg_bw_mgt_int \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE0_CORE.RC.PCIECR.LCSR.word, int_stat));

		callback = gDD_PCIE_RC_Int_Trs_Ics_msi_Cb[E_DD_PCIE_CH0].cfg_bw_mgt_int_cb;
		if(callback != NULL){
			dd_pcie_debug_print(("[DD_PCIE_RC] cfg_bw_mgt_int callback\n"));
			(*callback)(0);
		}
	}

	// smlh_req_rst_not
	else if(IO_PCIE0.INT_CONTROL_2.bit.SMLH_REQ_RST_STT != 0){
		int_stat = IO_PCIE0.INT_CONTROL_2.word;
		IO_PCIE0.INT_CONTROL_2.bit.SMLH_REQ_RST_STT = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] smlh_req_rst_not \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE0.INT_CONTROL_2.word, int_stat));

		callback = gDD_PCIE_RC_Int_Trs_Ics_msi_Cb[E_DD_PCIE_CH0].smlh_req_rst_not_cb;
		if(callback != NULL){
			dd_pcie_debug_print(("[DD_PCIE_RC] smlh_req_rst_not callback\n"));
			(*callback)(0);
		}
	}
}

/**
 * @brief	Interrupt handler for PCIe ch1 own.
 */
VOID Dd_Pcie_Rc_Ch1_Int_Own_Handler(VOID)
{
	VP_CALLBACK_PCIE callback;

	dd_pcie_debug_print(("[DD_PCIE_RC] CH1 INT_OWN Occurred.\n"));

	// cfg_pme_int/cfg_pme_msi
	if(IO_PCIE1_CORE.RC.PCIECR.RSR.bit.PMES == 1){
		dd_pcie_debug_print(("[DD_PCIE_RC] cfg_pme_int/cfg_pme_msi.\n"));
		IO_PCIE1_CORE.RC.PCIECR.RSR.bit.PMES = 1;	// Clear Interrupt
		callback = gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH1].cfg_pme_cb;
		if(callback != NULL){
			(*callback)(0);
		}
	}

	// hp_mpe/hp_int/hp_msi
	// PCIE_CAP_ATTENTION_BUTTON_PRESSED
	else if(IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.ABP == 1){
		dd_pcie_debug_print(("[DD_PCIE_RC] hp_mpe/hp_int/hp_msi: PCIE_CAP_ATTENTION_BUTTON_PRESSED.\n"));
		Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(E_DD_PCIE_CH1, TRUE);
		IO_PCIE1_CORE.RC.PCIECR.SCR.bit.ABP = 0;
		Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(E_DD_PCIE_CH1, FALSE);
		IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.ABP = 1;	// Clear Interrupt
		callback = gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH1].hp_cb;
		if(callback != NULL){
			(*callback)(E_DD_PCIE_HOT_PLUG_ST_ABP);
		}
	}

	// PCIE_CAP_POWER_FAULT_DETECTED
	else if(IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.PFD == 1){
		dd_pcie_debug_print(("[DD_PCIE_RC] hp_mpe/hp_int/hp_msi: PCIE_CAP_POWER_FAULT_DETECTED.\n"));
		Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(E_DD_PCIE_CH1, TRUE);
		IO_PCIE1_CORE.RC.PCIECR.SCR.bit.PCP = 0;
		Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(E_DD_PCIE_CH1, FALSE);
		IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.PFD = 1;	// Clear Interrupt
		callback = gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH1].hp_cb;
		if(callback != NULL){
			(*callback)(E_DD_PCIE_HOT_PLUG_ST_PFD);
		}
	}

	// PCIE_CAP_MRL_SENSOR_CHANGED
	else if(IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.MRLSC == 1){
		dd_pcie_debug_print(("[DD_PCIE_RC] hp_mpe/hp_int/hp_msi: PCIE_CAP_MRL_SENSOR_CHANGED.\n"));
		IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.MRLSC = 1;	// Clear Interrupt
		callback = gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH1].hp_cb;
		if(callback != NULL){
			(*callback)(E_DD_PCIE_HOT_PLUG_ST_MRLSC);
		}
	}

	// PCIE_CAP_PRESENCE_DETECTED_CHANGED
	else if(IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.PDC == 1){
		dd_pcie_debug_print(("[DD_PCIE_RC] hp_mpe/hp_int/hp_msi: PCIE_CAP_PRESENCE_DETECTED_CHANGED.\n"));
		IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.PDC = 1;	// Clear Interrupt
		callback = gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH1].hp_cb;
		if(callback != NULL){
			(*callback)(E_DD_PCIE_HOT_PLUG_ST_PDC);
		}
	}

	// PCIE_CAP_CMD_CPLD
	else if(IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.CC == 1){
		dd_pcie_debug_print(("[DD_PCIE_RC] hp_mpe/hp_int/hp_msi: PCIE_CAP_CMD_CPLD.\n"));
		IO_PCIE1.EM_CONTROL.bit.CMD_CPLED_INT = 0;
		IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.CC = 1;	// Clear Interrupt
		callback = gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH1].hp_cb;
		if(callback != NULL){
			(*callback)(E_DD_PCIE_HOT_PLUG_ST_CC);
		}
	}

	// PCIE_CAP_DLL_STATE_CHANGED
	else if(IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.DLLSC == 1){
		dd_pcie_debug_print(("[DD_PCIE_RC] hp_mpe/hp_int/hp_msi: PCIE_CAP_DLL_STATE_CHANGED.\n"));
		IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.DLLSC = 1;	// Clear Interrupt
		callback = gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH1].hp_cb;
		if(callback != NULL){
			(*callback)(E_DD_PCIE_HOT_PLUG_ST_DLLSC);
		}
	}

	// rdlh_link_up
	else if(IO_PCIE1.INT_CONTROL_2.bit.RDLH_LINK_UP_STT == 1){
		dd_pcie_debug_print(("[DD_PCIE_RC] rdlh_link_up.\n"));
		IO_PCIE1.INT_CONTROL_2.bit.RDLH_LINK_UP_STT = 1;	// Clear Interrupt
		callback = gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH1].rdlh_link_up_cb;
		if(callback != NULL){
			(*callback)(0);
		}
	}

	// wake
	else if(IO_PCIE1.INT_CONTROL_2.bit.WAKE_STATUS == 1){
		dd_pcie_debug_print(("[DD_PCIE_RC] wake.\n"));
		IO_PCIE1.INT_CONTROL_2.bit.WAKE_STATUS = 1;	// Clear Interrupt
		callback = gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH1].wake_cb;
		if(callback != NULL){
			(*callback)(0);
		}
	}
}

/**
 * @brief	Interrupt handler for PCIe ch1 endpoint.
 */
VOID Dd_Pcie_Rc_Ch1_Int_Ep_Handler(VOID)
{
	UINT32 int_stat;
	VP_CALLBACK_PCIE callback;

	dd_pcie_debug_print(("[DD_PCIE_RC] CH1 INT_EP Occurred.\n"));

	// radm_inta/b/c/d
	if(IO_PCIE1.INT_CONTROL_1.word & 0xF0000000){
		int_stat = IO_PCIE1.INT_CONTROL_1.word;
		IO_PCIE1.INT_CONTROL_1.word &= int_stat;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] INTX_Status \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE1.INT_CONTROL_1.word, int_stat));

		callback = gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH1].radm_intx_cb;
		if(callback != NULL) {
			dd_pcie_debug_print(("[DD_PCIE_RC] INT_X callback\n"));
			(*callback)(int_stat >> 28);	// argument: 1(INTA), 2(INTB), 4(INTC), 8(INTD)
		}
	}

	// radm_pm_pme
	else if(IO_PCIE1.INT_CONTROL_3.bit.RADM_PM_PME_STT == 1){
		IO_PCIE1.INT_CONTROL_3.bit.RADM_PM_PME_STT = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] radm_pm_pme\n"));

		callback = gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH1].radm_pm_pme_cb;
		if(callback != NULL) {
			(*callback)(0);
		}
	}

	// radm_correctable_err
	else if(IO_PCIE1.INT_CONTROL_3.bit.RADM_COR_ERR_STT == 1){
		IO_PCIE1.INT_CONTROL_3.bit.RADM_COR_ERR_STT = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] radm_correctable_err\n"));

		callback = gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH1].radm_correctable_err_cb;
		if(callback != NULL) {
			(*callback)(0);
		}
	}

	// radm_nonfatal_err
	else if(IO_PCIE1.INT_CONTROL_3.bit.RADM_NF_ERR_STT == 1){
		IO_PCIE1.INT_CONTROL_3.bit.RADM_NF_ERR_STT = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] radm_nonfatal_err\n"));

		callback = gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH1].radm_nonfatal_err_cb;
		if(callback != NULL) {
			(*callback)(0);
		}
	}

	// radm_fatal_err
	else if(IO_PCIE1.INT_CONTROL_3.bit.RADM_F_ERR_STT == 1){
		IO_PCIE1.INT_CONTROL_3.bit.RADM_F_ERR_STT = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] radm_fatal_err\n"));

		callback = gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH1].radm_fatal_err_cb;
		if(callback != NULL) {
			(*callback)(0);
		}
	}

	// radm_pm_to_ack
	else if(IO_PCIE1.INT_CONTROL_3.bit.RADM_PM2ACK_STT == 1){
		IO_PCIE1.INT_CONTROL_3.bit.RADM_PM2ACK_STT = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] radm_pm_to_ack\n"));

		callback = gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH1].radm_pm_to_ack_cb;
		if(callback != NULL) {
			(*callback)(0);
		}
	}

	// radm_vendor_msg
	else if(IO_PCIE1.INT_CONTROL_2.bit.VENDOR_MSG_STT == 1){
		IO_PCIE1.INT_CONTROL_2.bit.VENDOR_MSG_STT = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] radm_vendor_msg\n"));

		callback = gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH1].radm_vendor_msg_cb;
		if(callback != NULL) {
			(*callback)(0);
		}
	}

	// radm_msg_ltr
	else if(IO_PCIE1.INT_CONTROL_3.bit.RADM_MSG_LTR_STT){	// LTR Interrupt Status
		IO_PCIE1.INT_CONTROL_3.bit.RADM_MSG_LTR_STT = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] radm_msg_ltr\n"));

		callback = gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH1].radm_msg_ltr_cb;
		if(callback != NULL) {
			(*callback)(0);
		}
	}
}

/**
 * @brief	Interrupt handler for PCIe ch1 system error.
 */
VOID Dd_Pcie_Rc_Ch1_Int_Sys_Err_Handler(VOID)
{
	UINT32 int_stat;
	VP_CALLBACK_PCIE callback;

	dd_pcie_debug_print(("[DD_PCIE_RC] CH1 INT_SYS_ERR Occurred.\n"));

	// cfg_aer_rc_err_int/cfg_aer_rc_err_msi
	if((IO_PCIE1_CORE.RC.AERC.RESR.word & 0x0000007F) != 0){
		int_stat = IO_PCIE1_CORE.RC.AERC.RESR.word;
		IO_PCIE1_CORE.RC.AERC.RESR.word &= 0x0000007F;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] cfg_aer_rc_err \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE1_CORE.RC.AERC.RESR.word, int_stat));

		callback = gDD_PCIE_RC_Int_Sys_Err_Cb[E_DD_PCIE_CH1].cfg_aer_rc_err_cb;
		if(callback != NULL){
			dd_pcie_debug_print(("[DD_PCIE_RC] cfg_aer_rc_err callback\n"));
			(*callback)(int_stat & 0x0000007F);
		}
	}

	// cfg_sys_err_rc
	else if(IO_PCIE1.INT_CONTROL_2.bit.SYS_ERR_RC_STT != 0){
		IO_PCIE1.INT_CONTROL_2.bit.SYS_ERR_RC_STT = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] cfg_sys_err_rc\n"));

		callback = gDD_PCIE_RC_Int_Sys_Err_Cb[E_DD_PCIE_CH1].cfg_sys_err_rc_cb;
		if(callback != NULL){
			dd_pcie_debug_print(("[DD_PCIE_RC] cfg_sys_err_rc callback\n"));
			(*callback)(0);
		}
	}
}

/**
 * @brief	Interrupt handler for PCIe ch1 dma.
 */
VOID Dd_Pcie_Rc_Ch1_Int_Dma_Handler(VOID)
{
	UINT32 status;
	VP_CALLBACK_PCIE callback;

	dd_pcie_debug_print(("[DD_PCIE_RC] CH1 INT_DMA Occurred.\n"));

	if(IO_PCIE1_CORE.RC.PPL.DWIS.word){				// DMA Write Interupt Status (Done and Abort)
		dd_pcie_debug_print(("[DD_PCIE_RC] DMA Write Interrupt Status \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE1_CORE.RC.PPL.DWIS.word, (UINT32)IO_PCIE1_CORE.RC.PPL.DWIS.word));

		// IO_PCIE1_CORE.RC.PPL.DWIS.word
		// 0x00000001  Done Interrupt status(DMA ch0)
		// 0x00000002  Done Interrupt status(DMA ch1)
		// 0x00010000  Abort Interrupt status(DMA ch0)
		// 0x00020000  Abort Interrupt status(DMA ch1)
		status = IO_PCIE1_CORE.RC.PPL.DWIS.word;

		IO_PCIE1_CORE.RC.PPL.DWIC.word = IO_PCIE1_CORE.RC.PPL.DWIS.word;	// Interrupt clear

		callback = gDD_PCIE_RC_Int_Dma_Cb[E_DD_PCIE_CH1].wr_cb;
		if(callback){
			dd_pcie_debug_print(("[DD_PCIE_RC] INT_DMA write callback\n"));
			(*callback)(status);
		}
	}
	else if(IO_PCIE1_CORE.RC.PPL.DRIS.word){		// DMA Read Interupt Status (Done and Abort)
		dd_pcie_debug_print(("[DD_PCIE_RC] DMA Read Interrupt Status \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE1_CORE.RC.PPL.DRIS.word, (UINT32)IO_PCIE1_CORE.RC.PPL.DRIS.word));

		// IO_PCIE1_CORE.RC.PPL.DRIS.word
		// 0x00000001  Done Interrupt status(DMA ch0)
		// 0x00000002  Done Interrupt status(DMA ch1)
		// 0x00010000  Abort Interrupt status(DMA ch0)
		// 0x00020000  Abort Interrupt status(DMA ch1)
		status = IO_PCIE1_CORE.RC.PPL.DRIS.word;

		IO_PCIE1_CORE.RC.PPL.DRIC.word = IO_PCIE1_CORE.RC.PPL.DRIS.word;	// Interrupt clear

		callback = gDD_PCIE_RC_Int_Dma_Cb[E_DD_PCIE_CH1].rd_cb;
		if(callback){
			dd_pcie_debug_print(("[DD_PCIE_RC] INT_DMA read callback\n"));
			(*callback)(status);
		}
	}
}

/**
 * @brief	Interrupt handler for PCIe ch1 Trs_Ics_Msi.
 */
VOID Dd_Pcie_Rc_Ch1_Int_Trs_Ics_Msi_Handler(VOID)
{
	UINT32 int_stat;
	VP_CALLBACK_PCIE callback;

	dd_pcie_debug_print(("[DD_PCIE_RC] CH1 INT_TRS_ICS_MSI Occurred.\n"));

	// msi_ctrl_int (MSI interrupt)
	if(IO_PCIE1_CORE.RC.PPL.MCI0S.bit.MCI0S != 0){
		int_stat = IO_PCIE1_CORE.RC.PPL.MCI0S.bit.MCI0S;
		IO_PCIE1_CORE.RC.PPL.MCI0S.bit.MCI0S = int_stat;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] msi_ctrl_int \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE1_CORE.RC.PPL.MCI0S.word, int_stat));

		callback = gDD_PCIE_RC_Int_Msi_Cb[E_DD_PCIE_CH1];
		if(callback != NULL){
			dd_pcie_debug_print(("[DD_PCIE_RC] msi_ctrl_int callback\n"));
			(*callback)(int_stat);
		}
	}

	// cfg_link_auto_bw_int
	else if(IO_PCIE1_CORE.RC.PCIECR.LCSR.bit.LABS != 0){
		int_stat = IO_PCIE1_CORE.RC.PCIECR.LCSR.word;
		IO_PCIE1_CORE.RC.PCIECR.LCSR.bit.LABS = 1;		// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] cfg_link_auto_bw_int \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE1_CORE.RC.PCIECR.LCSR.word, int_stat));

		callback = gDD_PCIE_RC_Int_Trs_Ics_msi_Cb[E_DD_PCIE_CH1].cfg_link_auto_bw_int_cb;
		if(callback != NULL){
			dd_pcie_debug_print(("[DD_PCIE_RC] cfg_link_auto_bw_int callback\n"));
			(*callback)(0);
		}
	}

	// cfg_bw_mgt_int
	else if(IO_PCIE1_CORE.RC.PCIECR.LCSR.bit.LBMS != 0){
		int_stat = IO_PCIE1_CORE.RC.PCIECR.LCSR.word;
		IO_PCIE1_CORE.RC.PCIECR.LCSR.bit.LBMS = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] cfg_bw_mgt_int \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE1_CORE.RC.PCIECR.LCSR.word, int_stat));

		callback = gDD_PCIE_RC_Int_Trs_Ics_msi_Cb[E_DD_PCIE_CH1].cfg_bw_mgt_int_cb;
		if(callback != NULL){
			dd_pcie_debug_print(("[DD_PCIE_RC] cfg_bw_mgt_int callback\n"));
			(*callback)(0);
		}
	}

	// smlh_req_rst_not
	else if(IO_PCIE1.INT_CONTROL_2.bit.SMLH_REQ_RST_STT != 0){
		int_stat = IO_PCIE1.INT_CONTROL_2.word;
		IO_PCIE1.INT_CONTROL_2.bit.SMLH_REQ_RST_STT = 1;	// interrupt clear
		dd_pcie_debug_print(("[DD_PCIE_RC] smlh_req_rst_not \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE1.INT_CONTROL_2.word, int_stat));

		callback = gDD_PCIE_RC_Int_Trs_Ics_msi_Cb[E_DD_PCIE_CH1].smlh_req_rst_not_cb;
		if(callback != NULL){
			dd_pcie_debug_print(("[DD_PCIE_RC] smlh_req_rst_not callback\n"));
			(*callback)(0);
		}
	}
}

/**
 * @brief	Set the function to call back when INT_OWN interrupt occurs.
 * @param	E_DD_PCIE_CH	ch
 * @param	T_DD_PCIE_RC_INT_OWN_CB	int_own_cb
 * @return	INT32	D_DDIM_OK
 */
INT32 Dd_Pcie_Rc_Set_Int_Own_Func(E_DD_PCIE_CH ch, T_DD_PCIE_RC_INT_OWN_CB const* const int_own_cb)
{
	gDD_PCIE_RC_Int_Own_Cb[ch] = *int_own_cb;

	if(ch == E_DD_PCIE_CH0){
		// rdlh_link_up
		if(gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH0].rdlh_link_up_cb != NULL){
			if(Dd_Pcie_Rc_Get_Link_Status(ch) == FALSE){	// Not link up
				IO_PCIE0.INT_CONTROL_2.bit.RDLH_LINK_UP_STT	= 1;	// Clear Interrupt
				IO_PCIE0.INT_CONTROL_2.bit.RDLH_LINK_UP_EN	= 1;	// Interrupt Enable
				IO_PCIE0.INT_CONTROL_2.bit.RDLH_LINK_UP_CEN	= 1;	// Interrupt Clear Enable
				IO_PCIE0.INT_CONTROL_2.bit.RDLH_LINK_UP_M	= 0;	// Mask Off
			}
		}
		else{
			IO_PCIE0.INT_CONTROL_2.bit.RDLH_LINK_UP_EN	= 0;	// Interrupt Disable
			IO_PCIE0.INT_CONTROL_2.bit.RDLH_LINK_UP_CEN	= 0;	// Interrupt Clear Disable
		}

		if(gDD_PCIE_RC_RE_TYPE == E_DD_PCIE_RE_TYPE_RC){
			// cfg_pme_int/cfg_pme_msi
			if(gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH0].cfg_pme_cb != NULL){
				IO_PCIE0_CORE.RC.PCIECR.RSR.bit.PMES	= 1;	// Clear Interrupt
				IO_PCIE0_CORE.RC.PCIECR.RCCR.bit.PMEIE	= 1;	// PME Interrupt Enable
				IO_PCIE0_CORE.RC.PMCR.PMCS.bit.PMEEN	= 1;	// PME Enable
			}
			else{
				IO_PCIE0_CORE.RC.PCIECR.RCCR.bit.PMEIE	= 0;	// PME Interrupt Disable
				IO_PCIE0_CORE.RC.PMCR.PMCS.bit.PMEEN	= 0;	// PME Disable
			}

			// hp_mpe/hp_int/hp_msi
			if(gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH0].hp_cb != NULL){
				// Clear Interrupt
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.ABP	= 1;	// PCIE_CAP_ATTENTION_BUTTON_PRESSED
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.PFD	= 1;	// PCIE_CAP_POWER_FAULT_DETECTED
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.MRLSC	= 1;	// PCIE_CAP_MRL_SENSOR_CHANGED
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.PDC	= 1;	// PCIE_CAP_PRESENCE_DETECT_CHANGE
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.CC		= 1;	// PCIE_CAP_CMD_CPLD
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.DLLSC	= 1;	// PCIE_CAP_DLL_STATE_CHANGED
				// Interrupt Enable
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.HPIE	= 1;	// Hot Plug Interrupt Enable
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.ABPE	= 1;	// PCIE_CAP_ATTENTION_BUTTON_PRESSED
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.PFDE	= 1;	// PCIE_CAP_POWER_FAULT_DETECTED
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.MRLSCE	= 1;	// PCIE_CAP_MRL_SENSOR_CHANGED
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.PDCE	= 1;	// PCIE_CAP_PRESENCE_DETECT_CHANGE
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.CCIE	= 1;	// PCIE_CAP_CMD_CPLD
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.DLLSCE	= 1;	// PCIE_CAP_DLL_STATE_CHANGED

				IO_PCIE0.EM_SELECT.bit.PRE_DET_STT_SEL	= 1;	// Control by relay terminal

				Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(E_DD_PCIE_CH0, TRUE);
				IO_PCIE0_CORE.RC.PCIECR.SCR.bit.ABP		= 1;
				IO_PCIE0_CORE.RC.PCIECR.SCR.bit.PCP		= 1;
				IO_PCIE0_CORE.RC.PCIECR.SCR.bit.MRLSP	= 1;
				Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(E_DD_PCIE_CH0, FALSE);
			}
			else{
				Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(E_DD_PCIE_CH0, TRUE);
				IO_PCIE0_CORE.RC.PCIECR.SCR.bit.ABP		= 0;
				IO_PCIE0_CORE.RC.PCIECR.SCR.bit.PCP		= 0;
				IO_PCIE0_CORE.RC.PCIECR.SCR.bit.MRLSP	= 0;
				Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(E_DD_PCIE_CH0, FALSE);

				// Interrupt Disable
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.HPIE	= 0;	// Hot Plug Interrupt Enable
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.ABPE	= 0;	// PCIE_CAP_ATTENTION_BUTTON_PRESSED
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.PFDE	= 0;	// PCIE_CAP_POWER_FAULT_DETECTED
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.MRLSCE	= 0;	// PCIE_CAP_MRL_SENSOR_CHANGED
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.PDCE	= 0;	// PCIE_CAP_PRESENCE_DETECT_CHANGE
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.CCIE	= 0;	// PCIE_CAP_CMD_CPLD
				IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.DLLSCE	= 0;	// PCIE_CAP_DLL_STATE_CHANGED
			}

			// wake
			if(gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH0].wake_cb != NULL){
				Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PY4, D_DD_TOP_GPIO_FUNC_HWMODE);
				IO_PCIE0.PM_SELECT_1.bit.WAKE_SEL			= 1;	// Control by terminal wake_n_i
				IO_PCIE0.INT_CONTROL_2.bit.WAKE_MASK		= 0;	// Mask Off
				IO_PCIE0.INT_CONTROL_2.bit.WAKE_CLEAR_EN	= 1;	// Interrupt Clear Enable
				IO_PCIE0.INT_CONTROL_2.bit.WAKE_EN			= 1;	// Interrupt Enable
			}
			else{
				IO_PCIE0.INT_CONTROL_2.bit.WAKE_CLEAR_EN	= 0;	// Interrupt Clear Disable
				IO_PCIE0.INT_CONTROL_2.bit.WAKE_EN			= 0;	// Interrupt Disable
			}
		}
	}
	else{
		// rdlh_link_up
		if(gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH1].rdlh_link_up_cb != NULL){
			if(Dd_Pcie_Rc_Get_Link_Status(ch) == FALSE){	// Not link up
				IO_PCIE1.INT_CONTROL_2.bit.RDLH_LINK_UP_STT	= 1;	// Clear Interrupt
				IO_PCIE1.INT_CONTROL_2.bit.RDLH_LINK_UP_EN	= 1;	// Interrupt Enable
				IO_PCIE1.INT_CONTROL_2.bit.RDLH_LINK_UP_CEN	= 1;	// Interrupt Clear Enable
				IO_PCIE1.INT_CONTROL_2.bit.RDLH_LINK_UP_M	= 0;	// Mask Off
			}
		}
		else{
			IO_PCIE1.INT_CONTROL_2.bit.RDLH_LINK_UP_EN	= 0;	// Interrupt Disable
			IO_PCIE1.INT_CONTROL_2.bit.RDLH_LINK_UP_CEN	= 0;	// Interrupt Clear Disable
		}

		if(gDD_PCIE_RC_RE_TYPE == E_DD_PCIE_RE_TYPE_RC && gDD_PCIE_RC_Bifurcation == E_DD_PCIE_BIFURCATION_ENABLE){
			// cfg_pme_int/cfg_pme_msi
			if(gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH1].cfg_pme_cb != NULL){
				IO_PCIE1_CORE.RC.PCIECR.RSR.bit.PMES	= 1;	// Clear Interrupt
				IO_PCIE1_CORE.RC.PCIECR.RCCR.bit.PMEIE	= 1;	// PME Interrupt Enable
				IO_PCIE1_CORE.RC.PMCR.PMCS.bit.PMEEN	= 1;	// PME Enable
			}
			else{
				IO_PCIE1_CORE.RC.PCIECR.RCCR.bit.PMEIE	= 0;	// PME Interrupt Disable
				IO_PCIE1_CORE.RC.PMCR.PMCS.bit.PMEEN	= 0;	// PME Disable
			}

			// hp_mpe/hp_int/hp_msi
			if(gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH1].hp_cb != NULL){
				// Clear Interrupt
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.ABP	= 1;	// PCIE_CAP_ATTENTION_BUTTON_PRESSED
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.PFD	= 1;	// PCIE_CAP_POWER_FAULT_DETECTED
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.MRLSC	= 1;	// PCIE_CAP_MRL_SENSOR_CHANGED
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.PDC	= 1;	// PCIE_CAP_PRESENCE_DETECT_CHANGE
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.CC		= 1;	// PCIE_CAP_CMD_CPLD
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.DLLSC	= 1;	// PCIE_CAP_DLL_STATE_CHANGED
				// Interrupt Enable
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.HPIE	= 1;	// Hot Plug Interrupt Enable
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.ABPE	= 1;	// PCIE_CAP_ATTENTION_BUTTON_PRESSED
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.PFDE	= 1;	// PCIE_CAP_POWER_FAULT_DETECTED
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.MRLSCE	= 1;	// PCIE_CAP_MRL_SENSOR_CHANGED
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.PDCE	= 1;	// PCIE_CAP_PRESENCE_DETECT_CHANGE
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.CCIE	= 1;	// PCIE_CAP_CMD_CPLD
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.DLLSCE	= 1;	// PCIE_CAP_DLL_STATE_CHANGED

				IO_PCIE1.EM_SELECT.bit.PRE_DET_STT_SEL	= 1;	// Control by relay terminal

				Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(E_DD_PCIE_CH1, TRUE);
				IO_PCIE1_CORE.RC.PCIECR.SCR.bit.ABP		= 1;
				IO_PCIE1_CORE.RC.PCIECR.SCR.bit.PCP		= 1;
				IO_PCIE1_CORE.RC.PCIECR.SCR.bit.MRLSP	= 1;
				Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(E_DD_PCIE_CH1, FALSE);
			}
			else{
				Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(E_DD_PCIE_CH1, TRUE);
				IO_PCIE1_CORE.RC.PCIECR.SCR.bit.ABP		= 0;
				IO_PCIE1_CORE.RC.PCIECR.SCR.bit.PCP		= 0;
				IO_PCIE1_CORE.RC.PCIECR.SCR.bit.MRLSP	= 0;
				Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(E_DD_PCIE_CH1, FALSE);

				// Interrupt Disable
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.HPIE	= 0;	// Hot Plug Interrupt Enable
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.ABPE	= 0;	// PCIE_CAP_ATTENTION_BUTTON_PRESSED
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.PFDE	= 0;	// PCIE_CAP_POWER_FAULT_DETECTED
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.MRLSCE	= 0;	// PCIE_CAP_MRL_SENSOR_CHANGED
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.PDCE	= 0;	// PCIE_CAP_PRESENCE_DETECT_CHANGE
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.CCIE	= 0;	// PCIE_CAP_CMD_CPLD
				IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.DLLSCE	= 0;	// PCIE_CAP_DLL_STATE_CHANGED
			}

			// wake
			if(gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH1].wake_cb != NULL){
				Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PY5, D_DD_TOP_GPIO_FUNC_HWMODE);
				IO_PCIE1.PM_SELECT_1.bit.WAKE_SEL			= 1;	// Control by terminal wake_n_i
				IO_PCIE1.INT_CONTROL_2.bit.WAKE_MASK		= 0;	// Mask Off
				IO_PCIE1.INT_CONTROL_2.bit.WAKE_CLEAR_EN	= 1;	// Interrupt Clear Enable
				IO_PCIE1.INT_CONTROL_2.bit.WAKE_EN			= 1;	// Interrupt Enable
			}
			else{
				IO_PCIE1.INT_CONTROL_2.bit.WAKE_CLEAR_EN	= 0;	// Interrupt Clear Disable
				IO_PCIE1.INT_CONTROL_2.bit.WAKE_EN			= 0;	// Interrupt Disable
			}
		}
	}

	return D_DDIM_OK;
}

/**
 * @brief	Set the function to call back when INT_EP interrupt occurs.
 * @param	E_DD_PCIE_CH	ch
 * @param	T_DD_PCIE_RC_INT_EP_CB	int_ep_cb
 * @return	INT32	D_DDIM_OK
 */
INT32 Dd_Pcie_Rc_Set_Int_Ep_Func(E_DD_PCIE_CH ch, T_DD_PCIE_RC_INT_EP_CB const* const int_ep_cb)
{
	gDD_PCIE_RC_Int_Ep_Cb[ch] = *int_ep_cb;

	if(ch == E_DD_PCIE_CH0){
		// radm_pm_pme
		if(gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH0].radm_pm_pme_cb != NULL) {
			IO_PCIE0.INT_CONTROL_3.bit.RADM_PM_PME_STT	= 1;	// interrupt clear
			IO_PCIE0.INT_CONTROL_3.bit.RADM_PM_PME_EN	= 1;	// Interrupt Enable
			IO_PCIE0.INT_CONTROL_3.bit.RADM_PM_PME_CEN	= 1;	// Interrupt Clear Enable
			IO_PCIE0.INT_CONTROL_3.bit.RADM_PM_PME_M	= 0;	// Mask Off
		}
		else{
			IO_PCIE0.INT_CONTROL_3.bit.RADM_PM_PME_EN	= 0;	// Interrupt Disable
			IO_PCIE0.INT_CONTROL_3.bit.RADM_PM_PME_CEN	= 0;	// Interrupt Clear Disable
		}

		// radm_correctable_err
		if(gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH0].radm_correctable_err_cb != NULL) {
			IO_PCIE0.INT_CONTROL_3.bit.RADM_COR_ERR_STT	= 1;	// interrupt clear
			IO_PCIE0.INT_CONTROL_3.bit.RADM_COR_ERR_EN	= 1;	// Interrupt Enable
			IO_PCIE0.INT_CONTROL_3.bit.RADM_COR_ERR_CEN	= 1;	// Interrupt Clear Enable
			IO_PCIE0.INT_CONTROL_3.bit.RADM_COR_ERR_M	= 0;	// Mask Off
		}
		else{
			IO_PCIE0.INT_CONTROL_3.bit.RADM_COR_ERR_EN	= 0;	// Interrupt Disable
			IO_PCIE0.INT_CONTROL_3.bit.RADM_COR_ERR_CEN	= 0;	// Interrupt Clear Disable
		}

		// radm_nonfatal_err
		if(gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH0].radm_nonfatal_err_cb != NULL) {
			IO_PCIE0.INT_CONTROL_3.bit.RADM_NF_ERR_STT	= 1;	// interrupt clear
			IO_PCIE0.INT_CONTROL_3.bit.RADM_NF_ERR_EN	= 1;	// Interrupt Enable
			IO_PCIE0.INT_CONTROL_3.bit.RADM_NF_ERR_CEN	= 1;	// Interrupt Clear Enable
			IO_PCIE0.INT_CONTROL_3.bit.RADM_NF_ERR_M	= 0;	// Mask Off
		}
		else{
			IO_PCIE0.INT_CONTROL_3.bit.RADM_NF_ERR_EN	= 0;	// Interrupt Disable
			IO_PCIE0.INT_CONTROL_3.bit.RADM_NF_ERR_CEN	= 0;	// Interrupt Clear Disable
		}

		// radm_fatal_err
		if(gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH0].radm_fatal_err_cb != NULL) {
			IO_PCIE0.INT_CONTROL_3.bit.RADM_F_ERR_STT	= 1;	// interrupt clear
			IO_PCIE0.INT_CONTROL_3.bit.RADM_F_ERR_EN	= 1;	// Interrupt Enable
			IO_PCIE0.INT_CONTROL_3.bit.RADM_F_ERR_CEN	= 1;	// Interrupt Clear Enable
			IO_PCIE0.INT_CONTROL_3.bit.RADM_F_ERR_M		= 0;	// Mask Off
		}
		else{
			IO_PCIE0.INT_CONTROL_3.bit.RADM_F_ERR_EN	= 0;	// Interrupt Disable
			IO_PCIE0.INT_CONTROL_3.bit.RADM_F_ERR_CEN	= 0;	// Interrupt Clear Disable
		}

		// radm_pm_to_ack
		if(gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH0].radm_pm_to_ack_cb != NULL) {
			IO_PCIE0.INT_CONTROL_3.bit.RADM_PM2ACK_STT	= 1;	// interrupt clear
			IO_PCIE0.INT_CONTROL_3.bit.RADM_PM2ACK_EN	= 1;	// Interrupt Enable
			IO_PCIE0.INT_CONTROL_3.bit.RADM_PM2ACK_CEN	= 1;	// Interrupt Clear Enable
			IO_PCIE0.INT_CONTROL_3.bit.RADM_PM2ACK_M	= 0;	// Mask Off
		}
		else{
			IO_PCIE0.INT_CONTROL_3.bit.RADM_PM2ACK_EN	= 0;	// Interrupt Disable
			IO_PCIE0.INT_CONTROL_3.bit.RADM_PM2ACK_CEN	= 0;	// Interrupt Clear Disable
		}

		// radm_vendor_msg
		if(gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH0].radm_vendor_msg_cb != NULL) {
			IO_PCIE0.INT_CONTROL_2.bit.VENDOR_MSG_STT	= 1;	// interrupt clear
			IO_PCIE0.INT_CONTROL_2.bit.VENDOR_MSG_EN	= 1;	// Interrupt Enable
			IO_PCIE0.INT_CONTROL_2.bit.VENDOR_MSG_CEN	= 1;	// Interrupt Clear Enable
			IO_PCIE0.INT_CONTROL_2.bit.VENDOR_MSG_M		= 0;	// Mask Off
		}
		else{
			IO_PCIE0.INT_CONTROL_2.bit.VENDOR_MSG_EN	= 0;	// Interrupt Disable
			IO_PCIE0.INT_CONTROL_2.bit.VENDOR_MSG_CEN	= 0;	// Interrupt Clear Disable
		}

		// radm_msg_ltr
		if(gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH0].radm_msg_ltr_cb != NULL) {
			IO_PCIE0.INT_CONTROL_3.bit.RADM_MSG_LTR_STT	= 1;	// interrupt clear
			IO_PCIE0.INT_CONTROL_3.bit.RADM_MSG_LTR_EN	= 1;	// Interrupt Enable
			IO_PCIE0.INT_CONTROL_3.bit.RADM_MSG_LTR_CEN	= 1;	// Interrupt Clear Enable
			IO_PCIE0.INT_CONTROL_3.bit.RADM_MSG_LTR_M	= 0;	// Mask Off
		}
		else{
			IO_PCIE0.INT_CONTROL_3.bit.RADM_MSG_LTR_EN	= 0;	// Interrupt Disable
			IO_PCIE0.INT_CONTROL_3.bit.RADM_MSG_LTR_CEN	= 0;	// Interrupt Clear Disable
		}

		if(gDD_PCIE_RC_RE_TYPE == E_DD_PCIE_RE_TYPE_RC){
			// radm_inta/b/c/d
			if(gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH0].radm_intx_cb != NULL) {
				IO_PCIE0.INT_CONTROL_1.word &= 0xF0000000;			// interrupt clear
				IO_PCIE0_CORE.RC.PPL.MCI0E.bit.MCI0E = 0;			// MSI interrupt disable
				IO_PCIE0_CORE.RC.T1CSH.STATUS_COMMAND.bit.ID = 0;	// interrupt enable
			}
		}
	}
	else{
		// radm_pm_pme
		if(gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH1].radm_pm_pme_cb != NULL) {
			IO_PCIE1.INT_CONTROL_3.bit.RADM_PM_PME_STT	= 1;	// interrupt clear
			IO_PCIE1.INT_CONTROL_3.bit.RADM_PM_PME_EN	= 1;	// Interrupt Enable
			IO_PCIE1.INT_CONTROL_3.bit.RADM_PM_PME_CEN	= 1;	// Interrupt Clear Enable
			IO_PCIE1.INT_CONTROL_3.bit.RADM_PM_PME_M	= 0;	// Mask Off
		}
		else{
			IO_PCIE1.INT_CONTROL_3.bit.RADM_PM_PME_EN	= 0;	// Interrupt Disable
			IO_PCIE1.INT_CONTROL_3.bit.RADM_PM_PME_CEN	= 0;	// Interrupt Clear Disable
		}

		// radm_correctable_err
		if(gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH1].radm_correctable_err_cb != NULL) {
			IO_PCIE1.INT_CONTROL_3.bit.RADM_COR_ERR_STT	= 1;	// interrupt clear
			IO_PCIE1.INT_CONTROL_3.bit.RADM_COR_ERR_EN	= 1;	// Interrupt Enable
			IO_PCIE1.INT_CONTROL_3.bit.RADM_COR_ERR_CEN	= 1;	// Interrupt Clear Enable
			IO_PCIE1.INT_CONTROL_3.bit.RADM_COR_ERR_M	= 0;	// Mask Off
		}
		else{
			IO_PCIE1.INT_CONTROL_3.bit.RADM_COR_ERR_EN	= 0;	// Interrupt Disable
			IO_PCIE1.INT_CONTROL_3.bit.RADM_COR_ERR_CEN	= 0;	// Interrupt Clear Disable
		}

		// radm_nonfatal_err
		if(gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH1].radm_nonfatal_err_cb != NULL) {
			IO_PCIE1.INT_CONTROL_3.bit.RADM_NF_ERR_STT	= 1;	// interrupt clear
			IO_PCIE1.INT_CONTROL_3.bit.RADM_NF_ERR_EN	= 1;	// Interrupt Enable
			IO_PCIE1.INT_CONTROL_3.bit.RADM_NF_ERR_CEN	= 1;	// Interrupt Clear Enable
			IO_PCIE1.INT_CONTROL_3.bit.RADM_NF_ERR_M	= 0;	// Mask Off
		}
		else{
			IO_PCIE1.INT_CONTROL_3.bit.RADM_NF_ERR_EN	= 0;	// Interrupt Disable
			IO_PCIE1.INT_CONTROL_3.bit.RADM_NF_ERR_CEN	= 0;	// Interrupt Clear Disable
		}

		// radm_fatal_err
		if(gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH1].radm_fatal_err_cb != NULL) {
			IO_PCIE1.INT_CONTROL_3.bit.RADM_F_ERR_STT	= 1;	// interrupt clear
			IO_PCIE1.INT_CONTROL_3.bit.RADM_F_ERR_EN	= 1;	// Interrupt Enable
			IO_PCIE1.INT_CONTROL_3.bit.RADM_F_ERR_CEN	= 1;	// Interrupt Clear Enable
			IO_PCIE1.INT_CONTROL_3.bit.RADM_F_ERR_M		= 0;	// Mask Off
		}
		else{
			IO_PCIE1.INT_CONTROL_3.bit.RADM_F_ERR_EN	= 0;	// Interrupt Disable
			IO_PCIE1.INT_CONTROL_3.bit.RADM_F_ERR_CEN	= 0;	// Interrupt Clear Disable
		}

		// radm_pm_to_ack
		if(gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH1].radm_pm_to_ack_cb != NULL) {
			IO_PCIE1.INT_CONTROL_3.bit.RADM_PM2ACK_STT	= 1;	// interrupt clear
			IO_PCIE1.INT_CONTROL_3.bit.RADM_PM2ACK_EN	= 1;	// Interrupt Enable
			IO_PCIE1.INT_CONTROL_3.bit.RADM_PM2ACK_CEN	= 1;	// Interrupt Clear Enable
			IO_PCIE1.INT_CONTROL_3.bit.RADM_PM2ACK_M	= 0;	// Mask Off
		}
		else{
			IO_PCIE1.INT_CONTROL_3.bit.RADM_PM2ACK_EN	= 0;	// Interrupt Disable
			IO_PCIE1.INT_CONTROL_3.bit.RADM_PM2ACK_CEN	= 0;	// Interrupt Clear Disable
		}

		// radm_vendor_msg
		if(gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH1].radm_vendor_msg_cb != NULL) {
			IO_PCIE1.INT_CONTROL_2.bit.VENDOR_MSG_STT	= 1;	// interrupt clear
			IO_PCIE1.INT_CONTROL_2.bit.VENDOR_MSG_EN	= 1;	// Interrupt Enable
			IO_PCIE1.INT_CONTROL_2.bit.VENDOR_MSG_CEN	= 1;	// Interrupt Clear Enable
			IO_PCIE1.INT_CONTROL_2.bit.VENDOR_MSG_M		= 0;	// Mask Off
		}
		else{
			IO_PCIE1.INT_CONTROL_2.bit.VENDOR_MSG_EN	= 0;	// Interrupt Disable
			IO_PCIE1.INT_CONTROL_2.bit.VENDOR_MSG_CEN	= 0;	// Interrupt Clear Disable
		}

		// radm_msg_ltr
		if(gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH1].radm_msg_ltr_cb != NULL) {
			IO_PCIE1.INT_CONTROL_3.bit.RADM_MSG_LTR_STT	= 1;	// interrupt clear
			IO_PCIE1.INT_CONTROL_3.bit.RADM_MSG_LTR_EN	= 1;	// Interrupt Enable
			IO_PCIE1.INT_CONTROL_3.bit.RADM_MSG_LTR_CEN	= 1;	// Interrupt Clear Enable
			IO_PCIE1.INT_CONTROL_3.bit.RADM_MSG_LTR_M	= 0;	// Mask Off
		}
		else{
			IO_PCIE1.INT_CONTROL_3.bit.RADM_MSG_LTR_EN	= 0;	// Interrupt Disable
			IO_PCIE1.INT_CONTROL_3.bit.RADM_MSG_LTR_CEN	= 0;	// Interrupt Clear Disable
		}

		if(gDD_PCIE_RC_RE_TYPE == E_DD_PCIE_RE_TYPE_RC && gDD_PCIE_RC_Bifurcation == E_DD_PCIE_BIFURCATION_ENABLE){
			// radm_inta/b/c/d
			if(gDD_PCIE_RC_Int_Ep_Cb[E_DD_PCIE_CH1].radm_intx_cb != NULL) {
				IO_PCIE1.INT_CONTROL_1.word &= 0xF0000000;			// interrupt clear
				IO_PCIE1_CORE.RC.PPL.MCI0E.bit.MCI0E = 0;			// MSI interrupt disable
				IO_PCIE1_CORE.RC.T1CSH.STATUS_COMMAND.bit.ID = 0;	// interrupt enable
			}
		}
	}

	return D_DDIM_OK;
}

/**
 * @brief	Set the function to call back when INT_SYS_ERR interrupt occurs.
 * @param	E_DD_PCIE_CH	ch
 * @param	T_DD_PCIE_RC_INT_SYS_ERR_CB	int_sys_err_cb
 * @return	INT32	D_DDIM_OK
 */
INT32 Dd_Pcie_Rc_Set_Int_Sys_Err_Func(E_DD_PCIE_CH ch, T_DD_PCIE_RC_INT_SYS_ERR_CB const* const int_sys_err_cb)
{
	gDD_PCIE_RC_Int_Sys_Err_Cb[ch] = *int_sys_err_cb;

	if(ch == E_DD_PCIE_CH0){
		if(gDD_PCIE_RC_RE_TYPE == E_DD_PCIE_RE_TYPE_RC){
			// cfg_sys_err_rc
			if(gDD_PCIE_RC_Int_Sys_Err_Cb[E_DD_PCIE_CH0].cfg_sys_err_rc_cb != NULL){
				IO_PCIE0.INT_CONTROL_2.bit.SYS_ERR_RC_STT	= 1;	// interrupt clear
				IO_PCIE0.INT_CONTROL_2.bit.SYS_ERR_RC_EN	= 1;	// Interrupt Enable
				IO_PCIE0.INT_CONTROL_2.bit.SYS_ERR_RC_CEN	= 1;	// Interrupt Clear Enable
				IO_PCIE0.INT_CONTROL_2.bit.SYS_ERR_RC_M		= 0;	// Mask Off
			}
			else{
				IO_PCIE0.INT_CONTROL_2.bit.SYS_ERR_RC_EN	= 0;	// Interrupt Disable
				IO_PCIE0.INT_CONTROL_2.bit.SYS_ERR_RC_CEN	= 0;	// Interrupt Clear Disable
			}

			// cfg_aer_rc_err_int/cfg_aer_rc_err_msi
			if(gDD_PCIE_RC_Int_Sys_Err_Cb[E_DD_PCIE_CH0].cfg_aer_rc_err_cb != NULL){
				IO_PCIE0_CORE.RC.AERC.RESR.word &= 0x0000007F;	// interrupt clear
				IO_PCIE0_CORE.RC.AERC.RECR.bit.CERE		= 1;	// Correctable Error Reporting Enable
				IO_PCIE0_CORE.RC.AERC.RECR.bit.NFERE	= 1;	// Non-Fatal Error Reporting Enable
				IO_PCIE0_CORE.RC.AERC.RECR.bit.FERE		= 1;	// Fatal Error Reporting Enable
			}
			else{
				IO_PCIE0_CORE.RC.AERC.RECR.bit.CERE		= 0;	// Correctable Error Reporting Disable
				IO_PCIE0_CORE.RC.AERC.RECR.bit.NFERE	= 0;	// Non-Fatal Error Reporting Disable
				IO_PCIE0_CORE.RC.AERC.RECR.bit.FERE		= 0;	// Fatal Error Reporting Disable
			}

			if(gDD_PCIE_RC_Int_Sys_Err_Cb[E_DD_PCIE_CH0].cfg_sys_err_rc_cb != NULL ||
			   gDD_PCIE_RC_Int_Sys_Err_Cb[E_DD_PCIE_CH0].cfg_aer_rc_err_cb != NULL){
				IO_PCIE0_CORE.RC.PCIECR.RCCR.bit.SECEE	= 1;
				IO_PCIE0_CORE.RC.PCIECR.RCCR.bit.SENFEE	= 1;
				IO_PCIE0_CORE.RC.PCIECR.RCCR.bit.SEFEE	= 1;

				IO_PCIE0_CORE.RC.T1CSH.STATUS_COMMAND.bit.SERRE = 1;
				IO_PCIE0_CORE.RC.T1CSH.BRIDGE_CTRL_INT_PIN_INT_LINE.bit.SERRE = 1;

				IO_PCIE0_CORE.RC.PCIECR.DCSR.bit.CERE	= 1;
				IO_PCIE0_CORE.RC.PCIECR.DCSR.bit.NFERE	= 1;
				IO_PCIE0_CORE.RC.PCIECR.DCSR.bit.FERE	= 1;
			}
			else{
				IO_PCIE0_CORE.RC.PCIECR.RCCR.bit.SECEE	= 0;
				IO_PCIE0_CORE.RC.PCIECR.RCCR.bit.SENFEE	= 0;
				IO_PCIE0_CORE.RC.PCIECR.RCCR.bit.SEFEE	= 0;

				IO_PCIE0_CORE.RC.T1CSH.STATUS_COMMAND.bit.SERRE = 0;
				IO_PCIE0_CORE.RC.T1CSH.BRIDGE_CTRL_INT_PIN_INT_LINE.bit.SERRE = 0;

				IO_PCIE0_CORE.RC.PCIECR.DCSR.bit.CERE	= 0;
				IO_PCIE0_CORE.RC.PCIECR.DCSR.bit.NFERE	= 0;
				IO_PCIE0_CORE.RC.PCIECR.DCSR.bit.FERE	= 0;
			}
		}
	}
	else{
		if(gDD_PCIE_RC_RE_TYPE == E_DD_PCIE_RE_TYPE_RC && gDD_PCIE_RC_Bifurcation == E_DD_PCIE_BIFURCATION_ENABLE){
			// cfg_sys_err_rc
			if(gDD_PCIE_RC_Int_Sys_Err_Cb[E_DD_PCIE_CH1].cfg_sys_err_rc_cb != NULL){
				IO_PCIE1.INT_CONTROL_2.bit.SYS_ERR_RC_STT	= 1;	// interrupt clear
				IO_PCIE1.INT_CONTROL_2.bit.SYS_ERR_RC_EN	= 1;	// Interrupt Enable
				IO_PCIE1.INT_CONTROL_2.bit.SYS_ERR_RC_CEN	= 1;	// Interrupt Clear Enable
				IO_PCIE1.INT_CONTROL_2.bit.SYS_ERR_RC_M		= 0;	// Mask Off
			}
			else{
				IO_PCIE1.INT_CONTROL_2.bit.SYS_ERR_RC_EN	= 0;	// Interrupt Disable
				IO_PCIE1.INT_CONTROL_2.bit.SYS_ERR_RC_CEN	= 0;	// Interrupt Clear Disable
			}

			// cfg_aer_rc_err_int/cfg_aer_rc_err_msi
			if(gDD_PCIE_RC_Int_Sys_Err_Cb[E_DD_PCIE_CH1].cfg_aer_rc_err_cb != NULL){
				IO_PCIE1_CORE.RC.AERC.RESR.word &= 0x0000007F;	// interrupt clear
				IO_PCIE1_CORE.RC.AERC.RECR.bit.CERE		= 1;	// Correctable Error Reporting Enable
				IO_PCIE1_CORE.RC.AERC.RECR.bit.NFERE	= 1;	// Non-Fatal Error Reporting Enable
				IO_PCIE1_CORE.RC.AERC.RECR.bit.FERE		= 1;	// Fatal Error Reporting Enable
			}
			else{
				IO_PCIE1_CORE.RC.AERC.RECR.bit.CERE		= 0;	// Correctable Error Reporting Disable
				IO_PCIE1_CORE.RC.AERC.RECR.bit.NFERE	= 0;	// Non-Fatal Error Reporting Disable
				IO_PCIE1_CORE.RC.AERC.RECR.bit.FERE		= 0;	// Fatal Error Reporting Disable
			}

			if(gDD_PCIE_RC_Int_Sys_Err_Cb[E_DD_PCIE_CH1].cfg_sys_err_rc_cb != NULL ||
			   gDD_PCIE_RC_Int_Sys_Err_Cb[E_DD_PCIE_CH1].cfg_aer_rc_err_cb != NULL){
				IO_PCIE1_CORE.RC.PCIECR.RCCR.bit.SECEE	= 1;
				IO_PCIE1_CORE.RC.PCIECR.RCCR.bit.SENFEE	= 1;
				IO_PCIE1_CORE.RC.PCIECR.RCCR.bit.SEFEE	= 1;

				IO_PCIE1_CORE.RC.T1CSH.STATUS_COMMAND.bit.SERRE = 1;
				IO_PCIE1_CORE.RC.T1CSH.BRIDGE_CTRL_INT_PIN_INT_LINE.bit.SERRE = 1;

				IO_PCIE1_CORE.RC.PCIECR.DCSR.bit.CERE	= 1;
				IO_PCIE1_CORE.RC.PCIECR.DCSR.bit.NFERE	= 1;
				IO_PCIE1_CORE.RC.PCIECR.DCSR.bit.FERE	= 1;
			}
			else{
				IO_PCIE1_CORE.RC.PCIECR.RCCR.bit.SECEE	= 0;
				IO_PCIE1_CORE.RC.PCIECR.RCCR.bit.SENFEE	= 0;
				IO_PCIE1_CORE.RC.PCIECR.RCCR.bit.SEFEE	= 0;

				IO_PCIE1_CORE.RC.T1CSH.STATUS_COMMAND.bit.SERRE = 0;
				IO_PCIE1_CORE.RC.T1CSH.BRIDGE_CTRL_INT_PIN_INT_LINE.bit.SERRE = 0;

				IO_PCIE1_CORE.RC.PCIECR.DCSR.bit.CERE	= 0;
				IO_PCIE1_CORE.RC.PCIECR.DCSR.bit.NFERE	= 0;
				IO_PCIE1_CORE.RC.PCIECR.DCSR.bit.FERE	= 0;
			}
		}
	}

	return D_DDIM_OK;
}

/**
 * @brief	Set the function to call back when INT_TRS_ICS_MSI interrupt occurs.
 * @param	E_DD_PCIE_CH	ch
 * @param	T_DD_PCIE_RC_INT_TRS_ICS_MSI_CB	int_trs_ics_msi_cb
 * @return	INT32	D_DDIM_OK
 */
INT32 Dd_Pcie_Rc_Set_Int_Trs_Ics_Msi_Func(E_DD_PCIE_CH ch, T_DD_PCIE_RC_INT_TRS_ICS_MSI_CB const* const int_trs_ics_msi_cb)
{
	gDD_PCIE_RC_Int_Trs_Ics_msi_Cb[ch] = *int_trs_ics_msi_cb;

	if(ch == E_DD_PCIE_CH0){
		// smlh_req_rst_not
		if(gDD_PCIE_RC_Int_Trs_Ics_msi_Cb[E_DD_PCIE_CH0].smlh_req_rst_not_cb != NULL){
			IO_PCIE0.INT_CONTROL_2.bit.SMLH_REQ_RST_STT	= 1;	// interrupt clear
			IO_PCIE0.INT_CONTROL_2.bit.SMLH_REQ_RST_EN	= 1;	// Interrupt Enable
			IO_PCIE0.INT_CONTROL_2.bit.SMLH_REQ_RST_CEN	= 1;	// Interrupt Clear Enable
			IO_PCIE0.INT_CONTROL_2.bit.SMLH_REQ_RST_M	= 0;	// Mask Off
		}
		else{
			IO_PCIE0.INT_CONTROL_2.bit.SMLH_REQ_RST_EN	= 0;	// Interrupt Disable
			IO_PCIE0.INT_CONTROL_2.bit.SMLH_REQ_RST_CEN	= 0;	// Interrupt Clear Disable
		}

		if(gDD_PCIE_RC_RE_TYPE == E_DD_PCIE_RE_TYPE_RC){
			// cfg_link_auto_bw_int
			if(gDD_PCIE_RC_Int_Trs_Ics_msi_Cb[E_DD_PCIE_CH0].cfg_link_auto_bw_int_cb != NULL){
				IO_PCIE0_CORE.RC.PCIECR.LCSR.bit.LABS	= 1;	// interrupt clear
				IO_PCIE0_CORE.RC.PCIECR.LCSR.bit.LABIE	= 1;	// PCIE_CAP_LINK_AUTO_BW_INT_EN Enable
			}
			else{
				IO_PCIE0_CORE.RC.PCIECR.LCSR.bit.LABIE	= 0;	// PCIE_CAP_LINK_AUTO_BW_INT_EN Disable
			}

			// cfg_bw_mgt_int
			if(gDD_PCIE_RC_Int_Trs_Ics_msi_Cb[E_DD_PCIE_CH0].cfg_bw_mgt_int_cb != NULL){
				IO_PCIE0_CORE.RC.PCIECR.LCSR.bit.LBMS	= 1;	// interrupt clear
				IO_PCIE0_CORE.RC.PCIECR.LCSR.bit.LBMIE	= 1;	// PCIE_CAP_LINK_BW_MAN_INT_EN Enable
			}
			else{
				IO_PCIE0_CORE.RC.PCIECR.LCSR.bit.LBMIE	= 0;	// PCIE_CAP_LINK_BW_MAN_INT_EN Disable
			}
		}


	}
	else{
		// smlh_req_rst_not
		if(gDD_PCIE_RC_Int_Trs_Ics_msi_Cb[E_DD_PCIE_CH1].smlh_req_rst_not_cb != NULL){
			IO_PCIE1.INT_CONTROL_2.bit.SMLH_REQ_RST_STT	= 1;	// interrupt clear
			IO_PCIE1.INT_CONTROL_2.bit.SMLH_REQ_RST_EN	= 1;	// Interrupt Enable
			IO_PCIE1.INT_CONTROL_2.bit.SMLH_REQ_RST_CEN	= 1;	// Interrupt Clear Enable
			IO_PCIE1.INT_CONTROL_2.bit.SMLH_REQ_RST_M	= 0;	// Mask Off
		}
		else{
			IO_PCIE1.INT_CONTROL_2.bit.SMLH_REQ_RST_EN	= 0;	// Interrupt Disable
			IO_PCIE1.INT_CONTROL_2.bit.SMLH_REQ_RST_CEN	= 0;	// Interrupt Clear Disable
		}

		if(gDD_PCIE_RC_RE_TYPE == E_DD_PCIE_RE_TYPE_RC && gDD_PCIE_RC_Bifurcation == E_DD_PCIE_BIFURCATION_ENABLE){
			// cfg_link_auto_bw_int
			if(gDD_PCIE_RC_Int_Trs_Ics_msi_Cb[E_DD_PCIE_CH1].cfg_link_auto_bw_int_cb != NULL){
				IO_PCIE1_CORE.RC.PCIECR.LCSR.bit.LABS	= 1;	// interrupt clear
				IO_PCIE1_CORE.RC.PCIECR.LCSR.bit.LABIE	= 1;	// PCIE_CAP_LINK_AUTO_BW_INT_EN Enable
			}
			else{
				IO_PCIE1_CORE.RC.PCIECR.LCSR.bit.LABIE	= 0;	// PCIE_CAP_LINK_AUTO_BW_INT_EN Disable
			}

			// cfg_bw_mgt_int
			if(gDD_PCIE_RC_Int_Trs_Ics_msi_Cb[E_DD_PCIE_CH1].cfg_bw_mgt_int_cb != NULL){
				IO_PCIE1_CORE.RC.PCIECR.LCSR.bit.LBMS	= 1;	// interrupt clear
				IO_PCIE1_CORE.RC.PCIECR.LCSR.bit.LBMIE	= 1;	// PCIE_CAP_LINK_BW_MAN_INT_EN Enable
			}
			else{
				IO_PCIE1_CORE.RC.PCIECR.LCSR.bit.LBMIE	= 0;	// PCIE_CAP_LINK_BW_MAN_INT_EN Disable
			}
		}
	}

	return D_DDIM_OK;
}

/**
 * @brief	Interrupt handler for detect card of PCIe ch0.
 * @param	intst	Card edge detection
 */
VOID Dd_Pcie_Rc_Ch0_Int_Exs_Handler(ULONG intst)
{
	VP_CALLBACK_PCIE callback;

	if((intst & D_DD_EXS_PCIe0_RIS_BIT) != 0) {
		dd_pcie_debug_print(("[DD_PCIE_RC] Dd_Pcie_Rc_Ch0_Int_Exs_Handler : Rising Interrupt.\n"));
	}
	else if((intst & D_DD_EXS_PCIe0_FIS_BIT) != 0) {
		dd_pcie_debug_print(("[DD_PCIE_RC] Dd_Pcie_Rc_Ch0_Int_Exs_Handler : Falling Interrupt.\n"));

		// Hot-Plug flow (Card Detect)
		if((gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH0].hp_cb != NULL) && IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.PDCE){
			IO_PCIE0.EM_CONTROL.bit.PRE_DET_CHGED = 1;
		}
	}

	callback = gDD_PCIE_RC_Hp_Detect_Card[E_DD_PCIE_CH0];
	if(callback != NULL){
		(*callback)((UINT32)Dd_Exs_Get_SIGMON_PCIE0_CD());
	}
}

/**
 * @brief	Interrupt handler for detect card of PCIe ch1.
 * @param	intst	Card edge detection
 */
VOID Dd_Pcie_Rc_Ch1_Int_Exs_Handler(ULONG intst)
{
	VP_CALLBACK_PCIE callback;

	if((intst & D_DD_EXS_PCIe1_RIS_BIT) != 0) {
		dd_pcie_debug_print(("[DD_PCIE_RC] Dd_Pcie_Rc_Ch1_Int_Exs_Handler : Rising Interrupt.\n"));
	}
	else if((intst & D_DD_EXS_PCIe1_FIS_BIT) != 0) {
		dd_pcie_debug_print(("[DD_PCIE_RC] Dd_Pcie_Rc_Ch1_Int_Exs_Handler : Falling Interrupt.\n"));

		// Hot-Plug flow (Card Detect)
		if((gDD_PCIE_RC_Int_Own_Cb[E_DD_PCIE_CH1].hp_cb != NULL) && IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.PDCE){
			IO_PCIE1.EM_CONTROL.bit.PRE_DET_CHGED = 1;
		}
	}

	callback = gDD_PCIE_RC_Hp_Detect_Card[E_DD_PCIE_CH1];
	if(callback != NULL){
		(*callback)((UINT32)Dd_Exs_Get_SIGMON_PCIE1_CD());
	}
}

/**
 * @brief	ASPM Control.
 * @param	E_DD_PCIE_CH	ch
 * @param	E_DD_PCIE_ASPM_CONTROL aspm_control
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Rc_Ctrl_Aspm(E_DD_PCIE_CH ch, E_DD_PCIE_ASPM_CONTROL aspm_control)
{
	E_DD_PCIE_ST_LTSSM	ltssm_status;

	if(Dd_Pcie_Rc_Get_PMST_Status(ch) != E_DD_PCIE_PMST_D0){
		return D_DD_PCIE_ACCESS_ERR;
	}

	ltssm_status = Dd_Pcie_Rc_Get_ST_LTSSM_Status(ch);
	if(ltssm_status < E_DD_PCIE_S_L0 || ltssm_status > E_DD_PCIE_S_L1_IDLE){
		return D_DD_PCIE_ACCESS_ERR;
	}

	if(ch == E_DD_PCIE_CH0){
		IO_PCIE0_CORE.RC.PCIECR.LCSR.bit.ASPMC = aspm_control;
	}
	else{	// PCIe ch1
		IO_PCIE1_CORE.RC.PCIECR.LCSR.bit.ASPMC = aspm_control;
	}

	return D_DDIM_OK;
}

/**
 * @brief	Power Turn Off Request. (L2 Entry)
 * @param	E_DD_PCIE_CH	ch
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Rc_Pm_Pw_Turn_Off_Req(E_DD_PCIE_CH ch)
{
	if(Dd_Pcie_Rc_Get_ST_LTSSM_Status(ch) != E_DD_PCIE_S_L0){
		return D_DD_PCIE_ACCESS_ERR;
	}

	if(ch == E_DD_PCIE_CH0){
		IO_PCIE0.PM_UNLOCK_REQ.bit.PM_XMT_TURNOFF = 0;	// deassert
		IO_PCIE0.PM_UNLOCK_REQ.bit.PM_XMT_TURNOFF = 1;	// assert
	}
	else{	// PCIe ch1
		IO_PCIE1.PM_UNLOCK_REQ.bit.PM_XMT_TURNOFF = 0;	// deassert
		IO_PCIE1.PM_UNLOCK_REQ.bit.PM_XMT_TURNOFF = 1;	// assert
	}

	return D_DDIM_OK;
}

/**
 * @brief	Power L2 Exit. (L0 Entry)
 * @param	E_DD_PCIE_CH	ch
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Rc_Pm_Pw_L2_Exit(E_DD_PCIE_CH ch)
{
	UCHAR	function;

	if(Dd_Pcie_Rc_Get_ST_LTSSM_Status(ch) != E_DD_PCIE_S_L2_IDLE){
		return D_DD_PCIE_ACCESS_ERR;
	}

	if(ch == E_DD_PCIE_CH0){
		Dd_Top_Get_Gpio_Function(E_DD_TOP_GPIO_PY0, &function);
		Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PY0, D_DD_TOP_GPIO_FUNC_HWMODE);

		IO_PCIE0_CORE.RC.PMCR.PMCS.bit.PS			= 0;	// D0
		IO_PCIE0.RESET_CONTROL_1.bit.PERST_N_O_REG	= 0;	// PERST# Assert
		IO_PCIE0_CORE.RC.PCIECR.SCSR.bit.PCC		= 0;	// Power ON
		IO_PCIE0.RESET_CONTROL_1.bit.PERST_N_O_REG	= 1;	// PERST# Deassert

		Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PY0, function);
	}
	else{	// PCIe ch1
		Dd_Top_Get_Gpio_Function(E_DD_TOP_GPIO_PY1, &function);
		Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PY1, D_DD_TOP_GPIO_FUNC_HWMODE);

		IO_PCIE1_CORE.RC.PMCR.PMCS.bit.PS			= 0;	// D0
		IO_PCIE1.RESET_CONTROL_1.bit.PERST_N_O_REG	= 0;	// PERST# Assert
		IO_PCIE1_CORE.RC.PCIECR.SCSR.bit.PCC		= 0;	// Power ON
		IO_PCIE1.RESET_CONTROL_1.bit.PERST_N_O_REG	= 1;	// PERST# Deassert

		Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PY1, function);
	}

	return D_DDIM_OK;
}

/**
 * @brief	Get PCIe Power status.
 * @param	E_DD_PCIE_CH	ch
 * @return	E_DD_PCIE_PMST
 */
E_DD_PCIE_PMST Dd_Pcie_Rc_Get_PMST_Status(E_DD_PCIE_CH ch)
{
	if(ch == E_DD_PCIE_CH0){
		return (E_DD_PCIE_PMST)IO_PCIE0.PM_MONITOR_1.bit.PM_DSTATE;
	}
	else{	// PCIe ch1
		return (E_DD_PCIE_PMST)IO_PCIE1.PM_MONITOR_1.bit.PM_DSTATE;
	}
}

/**
 * @brief	Get PCIe LTSSM status.
 * @param	E_DD_PCIE_CH	ch
 * @return	E_DD_PCIE_ST_LTSSM
 */
E_DD_PCIE_ST_LTSSM Dd_Pcie_Rc_Get_ST_LTSSM_Status(E_DD_PCIE_CH ch)
{
	if(ch == E_DD_PCIE_CH0){
		return (E_DD_PCIE_ST_LTSSM)IO_PCIE0.PM_MONITOR_1.bit.SMLH_LTSSM_STATE;
	}
	else{	// PCIe ch1
		return (E_DD_PCIE_ST_LTSSM)IO_PCIE1.PM_MONITOR_1.bit.SMLH_LTSSM_STATE;
	}
}

/**
 * @brief	Transfer speed change.
 * @param	E_DD_PCIE_CH			ch
 * @param	E_DD_PCIE_LINK_SPEED	link_speed
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Rc_Transfer_Speed_Change(E_DD_PCIE_CH ch, E_DD_PCIE_LINK_SPEED link_speed)
{
	E_DD_PCIE_ST_LTSSM	ltssm_status;

	ltssm_status = Dd_Pcie_Rc_Get_ST_LTSSM_Status(ch);
	if(ltssm_status < E_DD_PCIE_S_L0 || ltssm_status > E_DD_PCIE_S_L1_IDLE){
		return D_DD_PCIE_ACCESS_ERR;
	}

	if(ch == E_DD_PCIE_CH0){
		if(link_speed == E_DD_PCIE_LINK_SPEED_GEN1){
			IO_PCIE0_CORE.RC.PCIECR.LC2SR.bit.TLS = 0x1;	// 2.5GT/s
		}
		else{
			IO_PCIE0_CORE.RC.PCIECR.LC2SR.bit.TLS = 0x2;	// 5.0GT/s
		}

		IO_PCIE0_CORE.RC.PPL.GEN2C.bit.DSC = 0x0;	// Deassert
		IO_PCIE0_CORE.RC.PPL.GEN2C.bit.DSC = 0x1;	// Assert

		if(IO_PCIE0_CORE.RC.PCIECR.LCSR.bit.LBMIE == 1){
			IO_PCIE0_CORE.RC.PCIECR.LCSR.bit.RL = 1;
		}
	}
	else{	// ch1
		if(link_speed == E_DD_PCIE_LINK_SPEED_GEN1){
			IO_PCIE1_CORE.RC.PCIECR.LC2SR.bit.TLS = 0x1;	// 2.5GT/s
		}
		else{
			IO_PCIE1_CORE.RC.PCIECR.LC2SR.bit.TLS = 0x2;	// 5.0GT/s
		}

		IO_PCIE1_CORE.RC.PPL.GEN2C.bit.DSC = 0x0;	// Deassert
		IO_PCIE1_CORE.RC.PPL.GEN2C.bit.DSC = 0x1;	// Assert

		if(IO_PCIE1_CORE.RC.PCIECR.LCSR.bit.LBMIE == 1){
			IO_PCIE1_CORE.RC.PCIECR.LCSR.bit.RL = 1;
		}
	}

	return D_DDIM_OK;
}

/**
 * @brief	Get current link speed.
 * @param	E_DD_PCIE_CH			ch
 * @param	E_DD_PCIE_LINK_SPEED	current_link_speed
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Rc_Get_Current_Link_Speed(E_DD_PCIE_CH ch, E_DD_PCIE_LINK_SPEED* current_link_speed)
{
	if(ch == E_DD_PCIE_CH0){
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){
			return D_DD_PCIE_ACCESS_ERR;
		}

		*current_link_speed = (E_DD_PCIE_LINK_SPEED)IO_PCIE0_CORE.RC.PCIECR.LCSR.bit.CLS;
	}
	else{	// PCIe ch1
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE1() != 0){
			return D_DD_PCIE_ACCESS_ERR;
		}

		*current_link_speed = (E_DD_PCIE_LINK_SPEED)IO_PCIE1_CORE.RC.PCIECR.LCSR.bit.CLS;
	}

	return D_DDIM_OK;
}

/**
 * @brief	Get ASPM Status.
 * @param	E_DD_PCIE_CH			ch
 * @param	E_DD_PCIE_ASPM_CONTROL	aspm_status
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Rc_Get_Aspm_Status(E_DD_PCIE_CH ch, E_DD_PCIE_ASPM_CONTROL* aspm_status)
{
	if(ch == E_DD_PCIE_CH0){
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){
			return D_DD_PCIE_ACCESS_ERR;
		}

		*aspm_status = (E_DD_PCIE_ASPM_CONTROL)IO_PCIE0_CORE.RC.PCIECR.LCSR.bit.ASPMC;
	}
	else{	// PCIe ch1
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE1() != 0){
			return D_DD_PCIE_ACCESS_ERR;
		}

		*aspm_status = (E_DD_PCIE_ASPM_CONTROL)IO_PCIE1_CORE.RC.PCIECR.LCSR.bit.ASPMC;
	}

	return D_DDIM_OK;
}

/**
 * @brief	Read the config read register.
 * @param	E_DD_PCIE_CH	ch
 * @param	USHORT			offset
 * @param	UINT32*			data
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR/D_DD_PCIE_COMM_ERROR
 */
INT32	Dd_Pcie_Rc_Config_Read(E_DD_PCIE_CH ch, USHORT offset, UINT32* data)
{
	INT32	ret = D_DDIM_OK;
	UINT32*	addr;

	ret = dd_pcie_rc_chk_config_rw_par(ch, offset);
	if(ret != D_DDIM_OK){
		return ret;
	}

	if(ch == E_DD_PCIE_CH0){
		addr = (UINT32*)((gDD_PCIE_RC_Config_ADR[E_DD_PCIE_CH0] << 16) + offset);
	}
	else{
		addr = (UINT32*)((gDD_PCIE_RC_Config_ADR[E_DD_PCIE_CH1] << 16) + offset);
	}

	dd_pcie_debug_print(("[DD_PCIE_RC]%s() addr  = 0x%08X\n", __func__, (UINT32)addr));
	dd_pcie_debug_print(("[DD_PCIE_RC]%s() *addr = 0x%08X\n", __func__, *addr));

	*data = *addr;

	return ret;
}

/**
 * @brief	Read the config write register.
 * @param	E_DD_PCIE_CH	ch
 * @param	USHORT		offset
 * @param	UINT32		data
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR/D_DD_PCIE_COMM_ERROR
 */
INT32	Dd_Pcie_Rc_Config_Write(E_DD_PCIE_CH ch, USHORT offset, UINT32 data)
{
	INT32	ret = D_DDIM_OK;
	UINT32*	addr;

	ret = dd_pcie_rc_chk_config_rw_par(ch, offset);
	if(ret != D_DDIM_OK){
		return ret;
	}

	if(ch == E_DD_PCIE_CH0){
		addr = (UINT32*)((gDD_PCIE_RC_Config_ADR[E_DD_PCIE_CH0] << 16) + offset);
	}
	else{
		addr = (UINT32*)((gDD_PCIE_RC_Config_ADR[E_DD_PCIE_CH1] << 16) + offset);
	}

	*addr = data;

	dd_pcie_debug_print(("[DD_PCIE_RC]%s() addr  = 0x%08X\n", __func__, (UINT32)addr));
	dd_pcie_debug_print(("[DD_PCIE_RC]%s() *addr = 0x%08X\n", __func__, *addr));

	return ret;
}

/**
 * @brief	It will be setting up Detect Card.
 * @param	E_DD_PCIE_CH		ch
 * @param	VP_CALLBACK_PCIE	callback
 * @return	INT32				D_DDIM_OK
 */
INT32 Dd_Pcie_Rc_Ctrl_Detect_Card(E_DD_PCIE_CH ch, VP_CALLBACK_PCIE callback)
{
	if(ch == E_DD_PCIE_CH0){
		Dd_Top_Set_PERSEL3_PRT0SCK3(0);	// CHIPTOP PERSEL3. Set PXRSTX0.

		Dd_Exs_Set_PCIE0_CD_Debounce_Soft_Reset(0);	// Reset
		Dd_Exs_Set_DEBEN_PCIE0_EN(1);				// PCIe0 Debounce ON
		Dd_Exs_Set_DEBEN_PCIE0DETIM(0);				// PCIe0 Debounce time
		Dd_Exs_Set_PCIE0_CD_Debounce_Soft_Reset(1);	// Reset Cancel

		Dd_Exs_Clear_INTSTAT_PCIE0_RIS();
		Dd_Exs_Clear_INTSTAT_PCIE0_FIS();
		Dd_Exs_Set_INTMSK_PCIe0_RIM(0);		// PCIe0 Detect INT Mask(Rising Edge) Off
		Dd_Exs_Set_INTMSK_PCIe0_FIM(0);		// PCIe0 Detect INT Mask(Falling Edge) Off


		gDD_PCIE_RC_Hp_Detect_Card[E_DD_PCIE_CH0] = callback;
	}
	else{	// PCIe ch1
		Dd_Top_Set_PERSEL3_PRT1SOU3(0);	// CHIPTOP RERSEL3. Set PXRSTX1.

		Dd_Exs_Set_PCIE1_CD_Debounce_Soft_Reset(0);	// Reset
		Dd_Exs_Set_DEBEN_PCIE1_EN(1);				// PCIe0 Debounce ON
		Dd_Exs_Set_DEBEN_PCIE1DETIM(0);				// PCIe0 Debounce time
		Dd_Exs_Set_PCIE1_CD_Debounce_Soft_Reset(1);	// Reset Cancel

		Dd_Exs_Clear_INTSTAT_PCIE1_RIS();
		Dd_Exs_Clear_INTSTAT_PCIE1_FIS();
		Dd_Exs_Set_INTMSK_PCIe1_RIM(0);		// PCIe0 Detect INT Mask(Rising Edge) Off
		Dd_Exs_Set_INTMSK_PCIe1_FIM(0);		// PCIe0 Detect INT Mask(Falling Edge) Off

		gDD_PCIE_RC_Hp_Detect_Card[E_DD_PCIE_CH1] = callback;
	}

	return D_DDIM_OK;
}

/**
 * @brief	Set max payload size.
 * @param	E_DD_PCIE_MPS	mps
 * @return	INT32			D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Rc_Set_Mps(E_DD_PCIE_CH ch, E_DD_PCIE_MPS mps)
{
	if(ch == E_DD_PCIE_CH0){
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){
			return D_DD_PCIE_ACCESS_ERR;
		}

		IO_PCIE0_CORE.RC.PCIECR.DCSR.bit.MPS = mps;
	}
	else{
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE1() != 0){
			return D_DD_PCIE_ACCESS_ERR;
		}

		IO_PCIE1_CORE.RC.PCIECR.DCSR.bit.MPS = mps;
	}

	return D_DDIM_OK;
}

/**
 * @brief	Get max payload size.
 * @param	E_DD_PCIE_MPS*	mps
 * @return	INT32			D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Rc_Get_Mps(E_DD_PCIE_CH ch, E_DD_PCIE_MPS* mps)
{
	if(ch == E_DD_PCIE_CH0){
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){
			return D_DD_PCIE_ACCESS_ERR;
		}

		*mps = IO_PCIE0_CORE.RC.PCIECR.DCSR.bit.MPS;
	}
	else{
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE1() != 0){
			return D_DD_PCIE_ACCESS_ERR;
		}

		*mps = IO_PCIE1_CORE.RC.PCIECR.DCSR.bit.MPS;
	}

	return D_DDIM_OK;
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/**
 * @brief	Get EXSTOP PCIe Register
 * @param	E_DD_PCIE_CH	ch
 * @param	USHORT			offset
 * @param	UINT32*			data
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR
 */
INT32 Dd_Pcie_Rc_Get_Exs_Reg(E_DD_PCIE_CH ch, USHORT offset, UINT32* data)
{
	INT32	ret = D_DDIM_OK;
	UINT32*	addr;

	ret = dd_pcie_rc_chk_exs_reg_par(ch, offset);
	if(ret != D_DDIM_OK){
		return ret;
	}

	if(ch == E_DD_PCIE_CH0){
		addr = (UINT32*)(D_DD_PCIE_0_EXS_ADR + offset);
	}
	else{
		addr = (UINT32*)(D_DD_PCIE_1_EXS_ADR + offset);
	}

//	dd_pcie_debug_print(("[DD_PCIE_RC] addr  = 0x%08X\n", addr));
//	dd_pcie_debug_print(("[DD_PCIE_RC] *addr = 0x%08X\n", *addr));

	*data = *addr;

	return ret;
}

/**
 * @brief	Set EXSTOP PCIe Register
 * @param	E_DD_PCIE_CH	ch
 * @param	USHORT			offset
 * @param	UINT32			data
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR
 */
INT32 Dd_Pcie_Rc_Set_Exs_Reg(E_DD_PCIE_CH ch, USHORT offset, UINT32 data)
{
	INT32	ret = D_DDIM_OK;
	UINT32*	addr;

	ret = dd_pcie_rc_chk_exs_reg_par(ch, offset);
	if(ret != D_DDIM_OK){
		return ret;
	}

	if(ch == E_DD_PCIE_CH0){
		addr = (UINT32*)(D_DD_PCIE_0_EXS_ADR + offset);
	}
	else{
		addr = (UINT32*)(D_DD_PCIE_1_EXS_ADR + offset);
	}

//	dd_pcie_debug_print(("[DD_PCIE_RC] addr  = 0x%08X\n", addr));
//	dd_pcie_debug_print(("[DD_PCIE_RC] *addr = 0x%08X\n", *addr));

	*addr = data;

	return ret;
}

/**
 * @brief	Get DBI AXI Slave Register
 * @param	E_DD_PCIE_CH	ch
 * @param	USHORT			offset
 * @param	UINT32*			data
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Rc_Get_Dbi_Reg(E_DD_PCIE_CH ch, USHORT offset, UINT32* data)
{
	INT32	ret = D_DDIM_OK;
	UINT32*	addr;

	ret = dd_pcie_rc_chk_dbi_reg_par(ch, offset);
	if(ret != D_DDIM_OK){
		return ret;
	}

	if(ch == E_DD_PCIE_CH0){
		addr = (UINT32*)(D_DD_PCIE_0_DBI_ADR + offset);
	}
	else{
		addr = (UINT32*)(D_DD_PCIE_1_DBI_ADR + offset);
	}

//	dd_pcie_debug_print(("[DD_PCIE_RC] addr  = 0x%08X\n", addr));
//	dd_pcie_debug_print(("[DD_PCIE_RC] *addr = 0x%08X\n", *addr));

	*data = *addr;

	return ret;
}

/**
 * @brief	Set DBI AXI Slave Register
 * @param	E_DD_PCIE_CH	ch
 * @param	USHORT			offset
 * @param	UINT32			data
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Rc_Set_Dbi_Reg(E_DD_PCIE_CH ch, USHORT offset, UINT32 data)
{
	INT32	ret = D_DDIM_OK;
	UINT32*	addr;

	ret = dd_pcie_rc_chk_dbi_reg_par(ch, offset);
	if(ret != D_DDIM_OK){
		return ret;
	}

	if(ch == E_DD_PCIE_CH0){
		addr = (UINT32*)(D_DD_PCIE_0_DBI_ADR + offset);
	}
	else{
		addr = (UINT32*)(D_DD_PCIE_1_DBI_ADR + offset);
	}

//	dd_pcie_debug_print(("[DD_PCIE_RC] addr  = 0x%08X\n", addr));
//	dd_pcie_debug_print(("[DD_PCIE_RC] *addr = 0x%08X\n", *addr));

	*addr = data;

	return ret;
}

/**
 * @brief	Set write access to DBI read-only register.
 * @param	E_DD_PCIE_CH	ch
 * @return	INT32			D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32  Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(E_DD_PCIE_CH ch, BOOL access)
{
	if(Dd_Pcie_Rc_Get_Link_Status(ch) == TRUE){	// Link up
		return D_DD_PCIE_ACCESS_ERR;
	}

	if(ch == E_DD_PCIE_CH0){
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){
			// PCIe power down
			return D_DD_PCIE_ACCESS_ERR;
		}

		IO_PCIE0_CORE.RC.PPL.MC1.bit.DRWE = access;
	}
	else{	// PCIe ch1
		if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE1() != 0){
			return D_DD_PCIE_ACCESS_ERR;
		}

		IO_PCIE1_CORE.RC.PPL.MC1.bit.DRWE = access;
	}

	return D_DDIM_OK;
}

#endif
