/**
 * @file		dd_pcie_ep.c
 * @brief		PCIe driver for Endpoint mode
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "ddim_user_custom.h"
#include "chiptop.h"
#include "dd_exs.h"
#include "dd_pcie_ep.h"
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
static volatile T_DD_PCIE_EP_INT_EP_CB	gDD_PCIE_EP_Int_Ep_Cb	= {NULL, NULL};
static volatile T_DD_PCIE_EP_INT_OWN_CB	gDD_PCIE_EP_Int_Own_Cb	= {NULL, NULL, NULL, NULL};
static volatile T_DD_PCIE_INT_DMA_CB	gDD_PCIE_EP_Int_Dma_Cb	= {NULL, NULL};

/** PCIe IP mode */
volatile UCHAR	gDD_PCIE_EP_RE_TYPE = E_DD_PCIE_RE_TYPE_US;	/**< Type Undettled */

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
 * @brief	Check EXSTOP PCIe register access parameter.
 * @param	USHORT	offset
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR/D_DD_PCIE_ACCESS_ERR
 */
static INT32 dd_pcie_ep_chk_exs_reg_par(USHORT offset)
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
 * @param	USHORT	offset
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR/D_DD_PCIE_ACCESS_ERR
 */
static INT32 dd_pcie_ep_chk_dbi_reg_par(USHORT offset)
{
	// Word boundary check.
	if((offset % 4) != 0){
		return D_DD_PCIE_INPUT_PARAM_ERROR;
	}

	// Check on reserved address. Access Type = DBI(Normal Register Access).
	if((offset >= 0x0038 && offset <= 0x003B) ||
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

	if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){
		// PCIe power down
		return D_DD_PCIE_ACCESS_ERR;
	}

	return D_DDIM_OK;
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief	Initialize PCIe Endpoint.
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_TIMEOUT
 */
INT32 Dd_Pcie_Ep_Init(VOID)
{
	UCHAR			function;
	UCHAR			direction;
	UCHAR			perst=0;
	UINT32			wait_counter;
	DDIM_USER_ER	ercd;

	Dd_Top_Set_PERSEL3_PRT0SCK3(1);		// bit23 FPSCK4		System reset input when used as Endpoint.
	Dd_Top_Set_PERSEL3_PXW0WP12(1);		// bit12 WPPG12_1	Wakeup request signal from EP to RC.
	Dd_Top_Set_PERSEL3_PXC0SCK7(1);		// bit14 FPSCK8		Reference clock control signal when L1 sub state.
	
	Dd_Top_Set_CLKSTOP2_EXSAX(2);		// AXI bus clock of EXS is supplied

	Dd_Exs_Set_PCIE0_BUS_CLOCK_Stop_Ctrl(0);	// Output PCIE0 BUS CLOCK
	Dd_Exs_Set_RAM_PD_Ctrl_PCIE0(0);			// Power-down mode is released

	// 1 Assert all PHY / LINK resets
	IO_PCIE0.RESET_CONTROL_1.word	= 0x00000000;
	IO_PCIE0.RESET_CONTROL_2.word	= 0x00000000;
	IO_PCIE0.RESET_SELECT_1.word	= 0x31111000;
	IO_PCIE0.RESET_SELECT_2.word	= 0x00000111;

	// 2 Set P<n>_app_ltssm_enable='0' for reprogramming before linkup.
	// 3 Set device_type (EP)
	IO_PCIE0.CORE_CONTROL.bit.DEVICE_TYPE = 0;	// PCI Express Endpoint

	// 4 pipe_port_sel=2'b01 ( Disable )
	IO_PCIE0.PHY_CONFIG_COM_6.bit.PIPE_PORT_SEL = 1;

	// 5 Supply Reference (It has executed)
	// 6 Wait for 10usec (Reference Clocks is stable)
	Dd_ARM_Wait_ns( 10000 );

	// 7 Wait for de asserting of PERST#
	Dd_Top_Get_Gpio_Function(E_DD_TOP_GPIO_PY0, &function);
	Dd_Top_Get_Gpio_Direction(E_DD_TOP_GPIO_PY0, &direction);
	Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PY0, D_DD_TOP_GPIO_FUNC_GPIO);
	Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PY0, D_DD_TOP_GPIO_DIR_IN);

	IO_PCIE0.RESET_SELECT_1.bit.PERST_SEL = 1;

	wait_counter = 0;
	Dd_Top_Get_Gpio_Status(E_DD_TOP_GPIO_PY0, &perst);
	while(perst == D_DD_TOP_GPIO_STATUS_LOW){	// PERSR#==1
		ercd = DDIM_User_Dly_Tsk(0);
		if (ercd != D_DDIM_USER_E_OK){
			dd_pcie_debug_print(("[DD_PCIE_EP]Dd_Pcie_Ep_Init: dly_tsk error. ercd = %d\n", ercd));
		}

		if(wait_counter > 100){	// 100ms wait
			dd_pcie_debug_print(("[DD_PCIE_EP]Dd_Pcie_Ep_Init: Timeout wait_counter=%d.\n", wait_counter));
			Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PY0, function);
			Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PY0, direction);
			return D_DD_PCIE_TIMEOUT;
		}
		wait_counter++;

		Dd_Top_Get_Gpio_Status(E_DD_TOP_GPIO_PY0, &perst);
	}
	dd_pcie_debug_print(("[DD_PCIE_EP]Dd_Pcie_Ep_Init: PERSR#==1 wait_counter=%d.\n", wait_counter));
	Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PY0, function);
	Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PY0, direction);

	// 8 Assert SYS_AUX_PWR_DET
	IO_PCIE0.PM_CONTROL_2.bit.SYS_AUX_PWR_DET = 1;

	// 9 Supply following clocks
	IO_PCIE0.AXI_CLK_STOP.word = 0x00000222;
	Dd_Top_Set_CLKSTOP2_PCISUPPCK(2);	// Sub clock of PCI is supplied.

	// 10 De assert PHY reset
	// 11 De assert LINK's PMC reset
	IO_PCIE0.RESET_CONTROL_1.word = 0x00000033;

	// 12 PHY auto
	// 13 Wrapper auto
	// 14-17 PHY auto
	// 18 Wrapper auto

	// 20 Set P<n>_app_ltssm_enable='1'
	IO_PCIE0.CORE_CONTROL.bit.APP_LTSSM_ENABLE = 1;

	gDD_PCIE_EP_RE_TYPE = E_DD_PCIE_RE_TYPE_EP;

	return D_DDIM_OK;
}

/**
 * @brief	PCIe End.
 */
VOID Dd_Pcie_Ep_Deinit(VOID)
{
	// Global data reset
	gDD_PCIE_EP_RE_TYPE					= E_DD_PCIE_RE_TYPE_US;
	gDD_PCIE_EP_Int_Ep_Cb.msg_unlock_cb	= NULL;
	gDD_PCIE_EP_Int_Ep_Cb.pm_turnof_cb	= NULL;
	gDD_PCIE_EP_Int_Own_Cb.lu_cb		= NULL;
	gDD_PCIE_EP_Int_Own_Cb.ce_cb		= NULL;
	gDD_PCIE_EP_Int_Own_Cb.nfe_cb		= NULL;
	gDD_PCIE_EP_Int_Own_Cb.fe_cb		= NULL;
	gDD_PCIE_EP_Int_Dma_Cb.wr_cb		= NULL;
	gDD_PCIE_EP_Int_Dma_Cb.rd_cb		= NULL;

	Dd_Exs_Set_RAM_PD_Ctrl_PCIE0(1);	// PCIE0 power down.
	Dd_Top_Set_CLKSTOP2_PCISUPPCK(3);	// Sub clock of PCI is stopped.
}

/**
 * @brief	Execute exclusive control for PCIe channel.
 * @param	INT32	tmout
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR/D_DD_PCIE_SEM_TIMEOUT/D_DD_PCIE_SEM_NG
 */
INT32 Dd_Pcie_Ep_Open(INT32 tmout)
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
INT32 Dd_Pcie_Ep_Close(VOID)
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
 * @param	E_DD_PCIE_CH	ch
 * @return	BOOL			TRUE/FALSE
 */
BOOL Dd_Pcie_Ep_Get_Link_Status(VOID)
{
	return (BOOL)IO_PCIE0.LINK_MONITOR.bit.SMLH_LINK_UP;
}

/**
 * @brief	It will be setting up the dma transfer.
 * @param	T_DD_PCIE_CTRL_DMA const* const	pcie_ctrl_dma
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Ep_Ctrl_Dma(T_DD_PCIE_CTRL_DMA const* const pcie_ctrl_dma)
{
#ifdef CO_PARAM_CHECK
	if(pcie_ctrl_dma == NULL){
		return D_DD_PCIE_INPUT_PARAM_ERROR;
	}
#endif

	if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){	// PCIe power down
		return D_DD_PCIE_ACCESS_ERR;
	}

	// Interrupt Clear
	IO_PCIE0_CORE.EP.PPL.DWIC.bit.WDIC = 0xFF;
	IO_PCIE0_CORE.EP.PPL.DWIC.bit.WAIC = 0xFF;
	IO_PCIE0_CORE.EP.PPL.DRIC.bit.RDIC = 0xFF;
	IO_PCIE0_CORE.EP.PPL.DRIC.bit.RAIC = 0xFF;

	if(pcie_ctrl_dma->direction == E_DD_PCIE_DMA_DIR_STOD){
		IO_PCIE0_CORE.EP.PPL.DWEE.bit.DWE = 1;						// Write Engine Enable
		gDD_PCIE_EP_Int_Dma_Cb.wr_cb = pcie_ctrl_dma->int_dma_cb;	// interrupt dma callback function
	}
	else{
		IO_PCIE0_CORE.EP.PPL.DREE.bit.DRE = 1;						// Read Engine Enable
		gDD_PCIE_EP_Int_Dma_Cb.rd_cb = pcie_ctrl_dma->int_dma_cb;	// interrupt dma callback function
	}

	IO_PCIE0_CORE.EP.PPL.DVS.bit.CN = pcie_ctrl_dma->dma_ch;		// Channel Number
	IO_PCIE0_CORE.EP.PPL.DVS.bit.CD = ~pcie_ctrl_dma->direction;	// Direction

	if(pcie_ctrl_dma->mode == E_DD_PCIE_DMA_DEMAND_MODE){	// dmac mode control
		IO_PCIE0_CORE.EP.PPL.DCC1.word = 0x00000008;		// Local Interrupt Enable

		IO_PCIE0_CORE.EP.PPL.DTS.bit.DTS  = pcie_ctrl_dma->dmnd_trans_size;	// Transfer Size
		IO_PCIE0_CORE.EP.PPL.DSL.bit.SARL = pcie_ctrl_dma->dmnd_src_addr_l;	// Source Address Low
		IO_PCIE0_CORE.EP.PPL.DSH.bit.SARH = pcie_ctrl_dma->dmnd_src_addr_h;	// Source Address Hight
		IO_PCIE0_CORE.EP.PPL.DDL.bit.DARL = pcie_ctrl_dma->dmnd_dst_addr_l;	// Destination Address Low
		IO_PCIE0_CORE.EP.PPL.DDH.bit.DARH = pcie_ctrl_dma->dmnd_dst_addr_h;	// Destination Address Hight
	}
	else{	// Linked List mode
		if(pcie_ctrl_dma->direction == E_DD_PCIE_DMA_DIR_STOD){
			IO_PCIE0_CORE.EP.PPL.DWLLEE.bit.WCLLLAIE = 1;	// Write Channel LL Local Abort Interrupt Enable.
			IO_PCIE0_CORE.EP.PPL.DWLLEE.bit.WCLLRAIE = 0;	// Read Channel LL Local Abort Interrupt Disable.
		}
		else{
			IO_PCIE0_CORE.EP.PPL.DWLLEE.bit.WCLLLAIE = 0;	// Write Channel LL Local Abort Interrupt Disable.
			IO_PCIE0_CORE.EP.PPL.DWLLEE.bit.WCLLRAIE = 1;	// Read Channel LL Local Abort Interrupt Enable.
		}

		// DMA Channel Control 1 register
		// Linked List Enable (LLE) = 1
		// Consumer Cycle State (CCS) = 1
		// Traffic Digest TLP Header Bit(TD) = 1
		IO_PCIE0_CORE.EP.PPL.DCC1.word = 0x04000300;

		IO_PCIE0_CORE.EP.PPL.DLL.bit.LL = pcie_ctrl_dma->ll_addr;	// Linked List Pointer Low
	}

	IO_PCIE0_CORE.EP.PPL.DWIM.bit.WDIM &= 0;	// Write Done Interrupt Mask Off
	IO_PCIE0_CORE.EP.PPL.DWIM.bit.WAIM &= 0;	// Write Abort Interrupt Mask Off
	IO_PCIE0_CORE.EP.PPL.DRIM.bit.RDIM &= 0;	// Read Done Interrupt Mask Off
	IO_PCIE0_CORE.EP.PPL.DRIM.bit.RAIM &= 0;	// Read Abort Interrupt Mask Off

	return D_DDIM_OK;
}

/**
 * @brief	Start DMA.
 * @param	E_DD_PCIE_DMA_CH	dma_ch
 * @param	E_DD_PCIE_DMA_DIR	dir
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Ep_Start_Dma(E_DD_PCIE_DMA_CH dma_ch, E_DD_PCIE_DMA_DIR dir)
{
	if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){	// PCIe power down
		return D_DD_PCIE_ACCESS_ERR;
	}

	if(dir == E_DD_PCIE_DMA_DIR_STOD){	// source to destination (write)
		IO_PCIE0_CORE.EP.PPL.DWD.bit.WDN	= dma_ch;
		IO_PCIE0_CORE.EP.PPL.DWD.bit.WS		= 0;
	}
	else{								// destination to source (read)
		IO_PCIE0_CORE.EP.PPL.DRD.bit.RDN	= dma_ch;
		IO_PCIE0_CORE.EP.PPL.DRD.bit.RS		= 0;
	}

	return D_DDIM_OK;
}

/**
 * @brief	Stop DMA.
 * @param	E_DD_PCIE_DMA_CH	dma_ch
 * @param	E_DD_PCIE_DMA_DIR	dir
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Ep_Stop_Dma(E_DD_PCIE_DMA_CH dma_ch, E_DD_PCIE_DMA_DIR dir)
{
	if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){	// PCIe power down
		return D_DD_PCIE_ACCESS_ERR;
	}

	if(dir == E_DD_PCIE_DMA_DIR_STOD){	// source to destination (write)
		IO_PCIE0_CORE.EP.PPL.DWD.bit.WDN	= dma_ch;
		IO_PCIE0_CORE.EP.PPL.DWD.bit.WS		= 1;
	}
	else{								// destination to source (read)
		IO_PCIE0_CORE.EP.PPL.DRD.bit.RDN	= dma_ch;
		IO_PCIE0_CORE.EP.PPL.DRD.bit.RS		= 1;
	}

	return D_DDIM_OK;
}

/**
 * @brief	It will be setting up the transfer of memory. CPU transfer.
 * @param	T_DD_PCIE_CTRL_MEM const* const pcie_ctrl_mem
 * @return	INT32 D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR/D_DD_PCIE_COMM_ERROR
 */
INT32 Dd_Pcie_Ep_Ctrl_Memory(T_DD_PCIE_CTRL_MEM const* const pcie_ctrl_mem)
{
#ifdef CO_PARAM_CHECK
	if(pcie_ctrl_mem == NULL){
		return D_DD_PCIE_INPUT_PARAM_ERROR;
	}
#endif

	if(Dd_Pcie_Ep_Get_Link_Status() == FALSE){	// Not link up
		return D_DD_PCIE_COMM_ERROR;
	}

	IO_PCIE0_CORE.EP.PPL.IV.word			= 0x00000001;
	IO_PCIE0_CORE.EP.PPL.ILBA.word			= pcie_ctrl_mem->base_adr;			// set the Lower Base Address
	IO_PCIE0_CORE.EP.PPL.IUBA.word			= 0x00000000;						// set the Upper Base Address
	IO_PCIE0_CORE.EP.PPL.ILA.word			= pcie_ctrl_mem->limit_adr;			// set the Limit Address
	IO_PCIE0_CORE.EP.PPL.ILTA.OB.bit.LTRO	= pcie_ctrl_mem->low_target_adr;	// set the Lower Target Address
	IO_PCIE0_CORE.EP.PPL.IUTA.bit.UTR		= pcie_ctrl_mem->upp_target_adr;	// set the Upper Target Address
	IO_PCIE0_CORE.EP.PPL.IRC1.word			= 0x00000000;						// define the type of the region to be MEM
	IO_PCIE0_CORE.EP.PPL.IRC2.word			= 0x80000000;						// enable the region in address match mode

	return D_DDIM_OK;
}

/**
 * @brief	It will be controll the INTx send request.
 * @return	UINT32				SYS_INT Status/D_DD_PCIE_ACCESS_ERR
 */
UINT32 Dd_Pcie_Ep_Req_Int_EP(VOID)
{
	UINT32	sys_int_stat = 0;
	UCHAR	msie;

	if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){
		// PCIe power down
		return D_DD_PCIE_ACCESS_ERR;
	}

	msie = IO_PCIE0_CORE.EP.MSICR.MSI_CAP_ID_NEXT_CTRL.bit.MSIE;
	if(msie == 1){	// MSI Enable
		IO_PCIE0_CORE.EP.MSICR.MSI_CAP_ID_NEXT_CTRL.bit.MSIE = 0;	// Set MSI Disable
	}

	if(Dd_Pcie_Ep_Get_ST_LTSSM_Status() == E_DD_PCIE_S_L1_IDLE){
		IO_PCIE0.PM_CONTROL_2.bit.APP_REQ_EXIT_L1 = 1;
	}

	sys_int_stat = (~(IO_PCIE0.INT_CONTROL_1.bit.SYS_INT) & 1);
	IO_PCIE0.INT_CONTROL_1.bit.SYS_INT = sys_int_stat;

	IO_PCIE0_CORE.EP.MSICR.MSI_CAP_ID_NEXT_CTRL.bit.MSIE = msie;
	IO_PCIE0.PM_CONTROL_2.bit.APP_REQ_EXIT_L1 = 0;

	return sys_int_stat;
}

/**
 * @brief	It will be controll the MSI transfer request.
 * @param	E_DD_PCIE_MSI_NUM	msi_num
 * @return	INT32				D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
UINT32 Dd_Pcie_Ep_Transfer_Msi(E_DD_PCIE_EP_MSI_NUM msi_num)
{
	UCHAR bme;
	UCHAR msie;
	UCHAR mme;

	if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){
		// PCIe power down
		return D_DD_PCIE_ACCESS_ERR;
	}

	bme = IO_PCIE0_CORE.EP.T0CSH.STATUS_COMMAND.bit.BME;
	if(bme == 0){	// Bus Master Disable
		IO_PCIE0_CORE.EP.T0CSH.STATUS_COMMAND.bit.BME = 1;			// Set Bus Master Enable
	}

	msie = IO_PCIE0_CORE.EP.MSICR.MSI_CAP_ID_NEXT_CTRL.bit.MSIE;
	if(msie == 0){	// MSI Disable
		IO_PCIE0_CORE.EP.MSICR.MSI_CAP_ID_NEXT_CTRL.bit.MSIE = 1;	// Set MSI Enable
	}

	mme = IO_PCIE0_CORE.EP.MSICR.MSI_CAP_ID_NEXT_CTRL.bit.MME;
	if(mme != 5){
		IO_PCIE0_CORE.EP.MSICR.MSI_CAP_ID_NEXT_CTRL.bit.MME = 5;	// Multiple Message Enable 32 kinds
	}

	if(Dd_Pcie_Ep_Get_ST_LTSSM_Status() == E_DD_PCIE_S_L1_IDLE){
		IO_PCIE0.PM_CONTROL_2.bit.APP_REQ_EXIT_L1 = 1;
	}

	IO_PCIE0.MSI_REQUEST_1.bit.LOCAL_INTERRUPT = (UINT32)1 << msi_num;	// MSI Transfer

	IO_PCIE0_CORE.EP.T0CSH.STATUS_COMMAND.bit.BME			= bme;
	IO_PCIE0_CORE.EP.MSICR.MSI_CAP_ID_NEXT_CTRL.bit.MSIE	= msie;
	IO_PCIE0_CORE.EP.MSICR.MSI_CAP_ID_NEXT_CTRL.bit.MME		= mme;
	IO_PCIE0.PM_CONTROL_2.bit.APP_REQ_EXIT_L1 = 0;

	return D_DDIM_OK;
}

/**
 * @brief	Interrupt handler for PCIe ch0 INTx.
 */
VOID Dd_Pcie_Ep_Ch0_Int_Ep_Handler(VOID)
{
	VP_CALLBACK_PCIE callback;

	dd_pcie_debug_print(("[DD_PCIE_EP] CH0 INT_EP Occurred.\n"));
	dd_pcie_debug_print(("[DD_PCIE_EP] INT_CONTROL_3_Status \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE0.INT_CONTROL_3.word, (UINT32)IO_PCIE0.INT_CONTROL_3.word));

	// Unlock Message
	if(IO_PCIE0.INT_CONTROL_3.bit.RADM_MSG_UL_STT){
		IO_PCIE0.INT_CONTROL_3.bit.RADM_MSG_UL_STT = 1;	// Interrupt clear
		callback = gDD_PCIE_EP_Int_Ep_Cb.msg_unlock_cb;
		if(callback){
			dd_pcie_debug_print(("[DD_PCIE_EP] INT_EP msg_unlock callback\n"));
			(*callback)(0);
		}
	}

	// PME Turn Off
	if(IO_PCIE0.INT_CONTROL_3.bit.RADM_PM_TOFF_STT){
		IO_PCIE0.INT_CONTROL_3.bit.RADM_PM_TOFF_STT = 1;	// Interrupt clear
		callback = gDD_PCIE_EP_Int_Ep_Cb.pm_turnof_cb;
		if(callback){
			dd_pcie_debug_print(("[DD_PCIE_EP] INT_EP pme turn off callback\n"));
			(*callback)(0);
		}
	}
}

/**
 * @brief	Interrupt handler for PCIe ch0 own.
 */
VOID Dd_Pcie_Ep_Ch0_Int_Own_Handler(VOID)
{
	VP_CALLBACK_PCIE callback;

	dd_pcie_debug_print(("[DD_PCIE_EP] CH0 INT_OWN Occurred.\n"));
	dd_pcie_debug_print(("[DD_PCIE_EP] INT_CONTROL_2_Status \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE0.INT_CONTROL_2.word, (UINT32)IO_PCIE0.INT_CONTROL_2.word));

	// Link Up
	if(IO_PCIE0.INT_CONTROL_2.bit.RDLH_LINK_UP_STT){
		IO_PCIE0.INT_CONTROL_2.bit.RDLH_LINK_UP_STT = 1;	// Interrupt clear
		callback = gDD_PCIE_EP_Int_Own_Cb.lu_cb;
		if(callback){
			dd_pcie_debug_print(("[DD_PCIE_EP] INT_OWN Link Up callback\n"));
			(*callback)(0);
		}
	}

	// Correctable Error
	if(IO_PCIE0.INT_CONTROL_2.bit.SEND_COR_ERR_STT){
		IO_PCIE0.INT_CONTROL_2.bit.SEND_COR_ERR_STT = 1;	// Interrupt clear
		callback = gDD_PCIE_EP_Int_Own_Cb.ce_cb;
		if(callback){
			dd_pcie_debug_print(("[DD_PCIE_EP] INT_OWN Correctable Error callback\n"));
			(*callback)(0);
		}
	}

	// Non-Fatal Error
	if(IO_PCIE0.INT_CONTROL_2.bit.SEND_NF_ERR_STT){
		IO_PCIE0.INT_CONTROL_2.bit.SEND_NF_ERR_STT = 1;	// Interrupt clear
		callback = gDD_PCIE_EP_Int_Own_Cb.nfe_cb;
		if(callback){
			dd_pcie_debug_print(("[DD_PCIE_EP] INT_OWN Non-Fatal Error callback\n"));
			(*callback)(0);
		}
	}

	// Fatal Error
	if(IO_PCIE0.INT_CONTROL_2.bit.SEND_F_ERR_STT){
		IO_PCIE0.INT_CONTROL_2.bit.SEND_F_ERR_STT = 1;	// Interrupt clear
		callback = gDD_PCIE_EP_Int_Own_Cb.fe_cb;
		if(callback){
			dd_pcie_debug_print(("[DD_PCIE_EP] INT_OWN Fatal Error callback\n"));
			(*callback)(0);
		}
	}
}

/**
 * @brief	Interrupt handler for PCIe ch0 dma.
 */
VOID Dd_Pcie_Ep_Ch0_Int_Dma_Handler(VOID)
{
	UINT32 status;
	VP_CALLBACK_PCIE callback;

	dd_pcie_debug_print(("[DD_PCIE_EP] CH0 INT_DMA Occurred.\n"));

	if(IO_PCIE0_CORE.EP.PPL.DWIS.word){				// DMA Write Interupt Status (Done and Abort)
		// DMA Write Interrupt Status
		dd_pcie_debug_print(("[DD_PCIE_EP] DMA Write Interrupt Status \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE0_CORE.EP.PPL.DWIS.word, (UINT32)IO_PCIE0_CORE.EP.PPL.DWIS.word));

		// IO_PCIE0_CORE.EP.PPL.DWIS.word
		// 0x00000001  Done Interrupt status(DMA ch0)
		// 0x00010000  Abort Interrupt status(DMA ch0)
		status = IO_PCIE0_CORE.EP.PPL.DWIS.word;

		IO_PCIE0_CORE.EP.PPL.DWIC.word = IO_PCIE0_CORE.EP.PPL.DWIS.word;	// Interrupt clear

		callback = gDD_PCIE_EP_Int_Dma_Cb.wr_cb;
		if(callback){
			dd_pcie_debug_print(("[DD_PCIE_EP] INT_DMA write callback\n"));
			(*callback)(status);
		}
	}
	else if(IO_PCIE0_CORE.EP.PPL.DRIS.word){		// DMA Read Interupt Status (Done and Abort)
		dd_pcie_debug_print(("[DD_PCIE_EP] DMA Read Interrupt Status \t0x%08X\t0x%08X\n", (UINT32)&IO_PCIE0_CORE.EP.PPL.DRIS.word, (UINT32)IO_PCIE0_CORE.EP.PPL.DRIS.word));

		// IO_PCIE0_CORE.EP.PPL.DRIS.word
		// 0x00000001  Done Interrupt status(DMA ch0)
		// 0x00010000  Abort Interrupt status(DMA ch0)
		status = IO_PCIE0_CORE.EP.PPL.DRIS.word;

		IO_PCIE0_CORE.EP.PPL.DRIC.word = IO_PCIE0_CORE.EP.PPL.DRIS.word;	// Interrupt clear

		callback = gDD_PCIE_EP_Int_Dma_Cb.rd_cb;
		if(callback){
			dd_pcie_debug_print(("[DD_PCIE_EP] INT_DMA read callback\n"));
			(*callback)(status);
		}
	}
}

/**
 * @brief	Set the function to call back when INT_EP interrupt occurs.
 * @param	T_DD_PCIE_EP_INT_EP_CB	INT_EP callback
 * @return	INT32	D_DDIM_OK
 */
INT32 Dd_Pcie_Ep_Set_Int_Ep_Func(T_DD_PCIE_EP_INT_EP_CB const* const int_ep_cb)
{
	gDD_PCIE_EP_Int_Ep_Cb = *int_ep_cb;

	// Unlock Message
	if(int_ep_cb->msg_unlock_cb != NULL){
		IO_PCIE0.INT_CONTROL_3.bit.RADM_MSG_UL_EN	= 1;	// Interrupt Enable
		IO_PCIE0.INT_CONTROL_3.bit.RADM_MSG_UL_CEN	= 1;	// Interrupt Clear Enable
		IO_PCIE0.INT_CONTROL_3.bit.RADM_MSG_UL_M	= 0;	// Mask Off
	}
	else{
		IO_PCIE0.INT_CONTROL_3.bit.RADM_MSG_UL_EN	= 0;	// Interrupt Disable
		IO_PCIE0.INT_CONTROL_3.bit.RADM_MSG_UL_CEN	= 0;	// Interrupt Clear Disable
	}

	// PME Turn Off
	if(int_ep_cb->pm_turnof_cb != NULL){
		IO_PCIE0.INT_CONTROL_3.bit.RADM_PM_TOFF_EN	= 1;	// Interrupt Enable
		IO_PCIE0.INT_CONTROL_3.bit.RADM_PM_TOFF_CEN	= 1;	// Interrupt Clear Enable
		IO_PCIE0.INT_CONTROL_3.bit.RADM_PM_TOFF_M	= 0;	// Mask Off
	}
	else{
		IO_PCIE0.INT_CONTROL_3.bit.RADM_PM_TOFF_EN	= 0;	// Interrupt Disable
		IO_PCIE0.INT_CONTROL_3.bit.RADM_PM_TOFF_CEN	= 0;	// Interrupt Clear Disable
	}

	return D_DDIM_OK;
}

/**
 * @brief	Set the function to call back when INT_TRS_ICS_MSI interrupt occurs.
 * @param	T_DD_PCIE_EP_INT_OWN_CB	INT_OWN callback
 * @return	INT32	D_DDIM_OK
 */
INT32 Dd_Pcie_Ep_Set_Int_Own_Func(T_DD_PCIE_EP_INT_OWN_CB const* const int_own_cb)
{
	gDD_PCIE_EP_Int_Own_Cb = *int_own_cb;

	// Link Up
	if(int_own_cb->lu_cb != NULL){
		if(Dd_Pcie_Ep_Get_Link_Status() == FALSE){	// Not link up
			IO_PCIE0.INT_CONTROL_2.bit.RDLH_LINK_UP_EN	= 1;	// Interrupt Enable
			IO_PCIE0.INT_CONTROL_2.bit.RDLH_LINK_UP_CEN	= 1;	// Interrupt Clear Enable
			IO_PCIE0.INT_CONTROL_2.bit.RDLH_LINK_UP_M	= 0;	// Mask Off
		}
	}
	else{
		IO_PCIE0.INT_CONTROL_2.bit.RDLH_LINK_UP_EN	= 0;	// Interrupt Disable
		IO_PCIE0.INT_CONTROL_2.bit.RDLH_LINK_UP_CEN	= 0;	// Interrupt Clear Disable
	}

	// Correctable Error
	if(int_own_cb->ce_cb != NULL){
		IO_PCIE0.INT_CONTROL_2.bit.SEND_COR_ERR_EN	= 1;	// Interrupt Enable
		IO_PCIE0.INT_CONTROL_2.bit.SEND_COR_ERR_CEN	= 1;	// Interrupt Clear Enable
		IO_PCIE0.INT_CONTROL_2.bit.SEND_COR_ERR_M	= 0;	// Mask Off
	}
	else{
		IO_PCIE0.INT_CONTROL_2.bit.SEND_COR_ERR_EN	= 0;	// Interrupt Disable
		IO_PCIE0.INT_CONTROL_2.bit.SEND_COR_ERR_CEN	= 0;	// Interrupt Clear Disable
	}

	// Non-Fatal Error
	if(int_own_cb->nfe_cb != NULL){
		IO_PCIE0.INT_CONTROL_2.bit.SEND_NF_ERR_EN	= 1;	// Interrupt Enable
		IO_PCIE0.INT_CONTROL_2.bit.SEND_NF_ERR_CEN	= 1;	// Interrupt Clear Enable
		IO_PCIE0.INT_CONTROL_2.bit.SEND_NF_ERR_M	= 0;	// Mask Off
	}
	else{
		IO_PCIE0.INT_CONTROL_2.bit.SEND_NF_ERR_EN	= 0;	// Interrupt Disable
		IO_PCIE0.INT_CONTROL_2.bit.SEND_NF_ERR_CEN	= 0;	// Interrupt Clear Disable
	}

	// Fatal Error
	if(int_own_cb->fe_cb != NULL){
		IO_PCIE0.INT_CONTROL_2.bit.SEND_F_ERR_EN	= 1;	// Interrupt Enable
		IO_PCIE0.INT_CONTROL_2.bit.SEND_F_ERR_CEN	= 1;	// Interrupt Clear Enable
		IO_PCIE0.INT_CONTROL_2.bit.SEND_F_ERR_M		= 0;	// Mask Off
	}
	else{
		IO_PCIE0.INT_CONTROL_2.bit.SEND_F_ERR_EN	= 0;	// Interrupt Disable
		IO_PCIE0.INT_CONTROL_2.bit.SEND_F_ERR_CEN	= 0;	// Interrupt Clear Disable
	}

	return D_DDIM_OK;
}

/**
 * @brief	ASPM Control.
 * @param	E_DD_PCIE_ASPM_CONTROL aspm_control
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Ep_Ctrl_Aspm(E_DD_PCIE_ASPM_CONTROL aspm_control)
{
	E_DD_PCIE_ST_LTSSM	ltssm_status;

	ltssm_status = Dd_Pcie_Ep_Get_ST_LTSSM_Status();
	if(ltssm_status < E_DD_PCIE_S_L0 || ltssm_status > E_DD_PCIE_S_L1_IDLE){
		return D_DD_PCIE_ACCESS_ERR;
	}

	IO_PCIE0_CORE.EP.PCIECR.LCSR.bit.ASPMC = aspm_control;

	return D_DDIM_OK;
}

/**
 * @brief	Get PCIe Power status.
 * @return	E_DD_PCIE_PMST
 */
E_DD_PCIE_PMST Dd_Pcie_Ep_Get_PMST_Status(VOID)
{
	return (E_DD_PCIE_PMST)IO_PCIE0.PM_MONITOR_1.bit.PM_DSTATE;
}

/**
 * @brief	Get PCIe LTSSM status.
 * @return	E_DD_PCIE_ST_LTSSM
 */
E_DD_PCIE_ST_LTSSM Dd_Pcie_Ep_Get_ST_LTSSM_Status(VOID)
{
	return (E_DD_PCIE_ST_LTSSM)IO_PCIE0.PM_MONITOR_1.bit.SMLH_LTSSM_STATE;
}

/**
 * @brief	Transfer speed change.
 * @param	E_DD_PCIE_LINK_SPEED	link_speed
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Ep_Transfer_Speed_Change(E_DD_PCIE_LINK_SPEED link_speed)
{
	E_DD_PCIE_ST_LTSSM	ltssm_status;

	ltssm_status = Dd_Pcie_Ep_Get_ST_LTSSM_Status();
	if(ltssm_status < E_DD_PCIE_S_L0 || ltssm_status > E_DD_PCIE_S_L1_IDLE){
		return D_DD_PCIE_ACCESS_ERR;
	}

	if(link_speed == E_DD_PCIE_LINK_SPEED_GEN1){
		IO_PCIE0_CORE.EP.PCIECR.LC2SR.bit.TLS = 0x1;	// 2.5GT/s
	}
	else{
		IO_PCIE0_CORE.EP.PCIECR.LC2SR.bit.TLS = 0x2;	// 5.0GT/s
	}

	IO_PCIE0_CORE.EP.PPL.GEN2C.bit.DSC = 0x0;	// Assert
	IO_PCIE0_CORE.EP.PPL.GEN2C.bit.DSC = 0x1;	// Deassert

	return D_DDIM_OK;
}

/**
 * @brief	Get current link speed.
 * @param	E_DD_PCIE_LINK_SPEED	current_link_speed
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Ep_Get_Current_Link_Speed(E_DD_PCIE_LINK_SPEED* current_link_speed)
{
	if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){
		// PCIe power down
		return D_DD_PCIE_ACCESS_ERR;
	}

	*current_link_speed = (E_DD_PCIE_LINK_SPEED)IO_PCIE0_CORE.EP.PCIECR.LCSR.bit.CLS;

	return D_DDIM_OK;
}

/**
 * @brief	Get ASPM Status.
 * @param	E_DD_PCIE_CH			ch
 * @param	E_DD_PCIE_ASPM_CONTROL	aspm_status
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Ep_Get_Aspm_Status(E_DD_PCIE_ASPM_CONTROL* aspm_status)
{
	if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){
		return D_DD_PCIE_ACCESS_ERR;
	}

	*aspm_status = (E_DD_PCIE_ASPM_CONTROL)IO_PCIE0_CORE.EP.PCIECR.LCSR.bit.ASPMC;

	return D_DDIM_OK;
}

/**
 * @brief	Set max payload size.
 * @param	E_DD_PCIE_MPS	mps
 * @return	INT32			D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Ep_Set_Mps(E_DD_PCIE_MPS mps)
{
	if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){
		return D_DD_PCIE_ACCESS_ERR;
	}

	IO_PCIE0_CORE.EP.PCIECR.DCSR.bit.MPS = mps;

	return D_DDIM_OK;
}

/**
 * @brief	Get max payload size.
 * @param	E_DD_PCIE_MPS*	mps
 * @return	INT32			D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Ep_Get_Mps(E_DD_PCIE_MPS* mps)
{
	if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){
		return D_DD_PCIE_ACCESS_ERR;
	}

	*mps = IO_PCIE0_CORE.EP.PCIECR.DCSR.bit.MPS;

	return D_DDIM_OK;
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
/**
 * @brief	Get EXSTOP PCIe Register
 * @param	USHORT			offset
 * @param	UINT32*			data
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR
 */
INT32 Dd_Pcie_Ep_Get_Exs_Reg(USHORT offset, UINT32* data)
{
	INT32	ret = D_DDIM_OK;
	UINT32*	addr;

	ret = dd_pcie_ep_chk_exs_reg_par(offset);
	if(ret != D_DDIM_OK){
		return ret;
	}

	addr = (UINT32*)(D_DD_PCIE_0_EXS_ADR + offset);

	dd_pcie_debug_print(("[DD_PCIE_EP] addr  = 0x%08X\n", (UINT32)addr));
	dd_pcie_debug_print(("[DD_PCIE_EP] *addr = 0x%08X\n", *addr));

	*data = *addr;

	return ret;
}

/**
 * @brief	Set EXSTOP PCIe Register
 * @param	USHORT			offset
 * @param	UINT32			data
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR
 */
INT32 Dd_Pcie_Ep_Set_Exs_Reg(USHORT offset, UINT32 data)
{
	INT32	ret = D_DDIM_OK;
	UINT32*	addr;

	ret = dd_pcie_ep_chk_exs_reg_par(offset);
	if(ret != D_DDIM_OK){
		return ret;
	}

	addr = (UINT32*)(D_DD_PCIE_0_EXS_ADR + offset);

	*addr = data;

	return ret;
}

/**
 * @brief	Get DBI AXI Slave Register
 * @param	USHORT			offset
 * @param	UINT32*			data
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Ep_Get_Dbi_Reg(USHORT offset, UINT32* data)
{
	INT32	ret = D_DDIM_OK;
	UINT32*	addr;

	ret = dd_pcie_ep_chk_dbi_reg_par(offset);
	if(ret != D_DDIM_OK){
		return ret;
	}

	addr = (UINT32*)(D_DD_PCIE_0_DBI_ADR + offset);

	dd_pcie_debug_print(("[DD_PCIE_EP] addr  = 0x%08X\n", (UINT32)addr));
	dd_pcie_debug_print(("[DD_PCIE_EP] *addr = 0x%08X\n", *addr));

	*data = *addr;

	return ret;
}

/**
 * @brief	Set DBI AXI Slave Register
 * @param	USHORT			offset
 * @param	UINT32			data
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_INPUT_PARAM_ERROR/D_DD_PCIE_ACCESS_ERR
 */
INT32 Dd_Pcie_Ep_Set_Dbi_Reg(USHORT offset, UINT32 data)
{
	INT32	ret = D_DDIM_OK;
	UINT32*	addr;

	ret = dd_pcie_ep_chk_dbi_reg_par(offset);
	if(ret != D_DDIM_OK){
		return ret;
	}

	addr = (UINT32*)(D_DD_PCIE_0_DBI_ADR + offset);

	*addr = data;

	return ret;
}

/**
 * @brief	Set write access to DBI read-only register.
 * @return	INT32	D_DDIM_OK/D_DD_PCIE_ACCESS_ERR
 */
INT32  Dd_Pcie_Ep_Set_Wr_Acc_To_Ro_Reg(BOOL access)
{
	if(Dd_Exs_Get_RAM_PD_Ctrl_PCIE0() != 0){
		// PCIe power down
		return D_DD_PCIE_ACCESS_ERR;
	}

	IO_PCIE0_CORE.EP.PPL.MC1.bit.DRWE = access;

	return D_DDIM_OK;
}

#endif
