/**
 * @file		dd_pcie_ep.h
 * @brief		PCIe Endpoint driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
/**
@weakgroup dd_pcie_ep
@{

<hr>

- @ref PCIe_EP_Sequence_Initialize
- @ref PCIe_EP_Sequence_Speed_change
- @ref PCIe_EP_Sequence_Memory_access
- @ref PCIe_EP_Sequence_DMA_transfer
- @ref PCIe_EP_Sequence_MSI_transfer
- @ref PCIe_EP_Sequence_INTx_message
- @ref PCIe_EP_Sequence_end
@section dd_pcie_ep_sample Sample Code
- @ref PCIe_EP_Sample_Initialize
- @ref PCIe_EP_Sample_Speed_Change
- @ref PCIe_EP_Sample_Memory_Access
- @ref PCIe_EP_Sample_Demand_DMA_Transfer
- @ref PCIe_EP_Sample_LL_DMA_Transfer
- @ref PCIe_EP_Sample_ASPM
- @ref PCIe_EP_Sample_Interrupt
- @ref PCIe_EP_Sample_End

@section PCIe_EP_Sequence_Initialize					Initial setting.
@image html dd_pcie_ep_Initialize.png

@section PCIe_EP_Sequence_Speed_change					Change of transfer speed.
@image html dd_pcie_ep_transfer_speed_change.png

@section PCIe_EP_Sequence_Memory_access					Access to the memory of Root Complex side.
@image html dd_pcie_ep_memory.png

@section PCIe_EP_Sequence_DMA_transfer					Data transfer by DMA.
@image html dd_pcie_ep_dma_transfer.png

@section PCIe_EP_Sequence_MSI_transfer					MSI transfer.
@image html dd_pcie_ep_MSI_transfer.png

@section PCIe_EP_Sequence_INTx_message					INTx message send.
@image html dd_pcie_ep_INTx_message.png

@section PCIe_EP_Sequence_end							PCIe end.
@image html dd_pcie_ep_end.png

@}
*/

/**
@addtogroup dd_pcie_ep
@{

*/

#ifndef _DD_PCIE_EP_H_
#define _DD_PCIE_EP_H_

#include "driver_common.h"
#include "exstop.h"
#include "dd_pcie_comm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
/** MSI Number */
typedef enum {
	E_DD_PCIE_EP_MSI_NUM_00 = 0,			/**< msi number 0	*/
	E_DD_PCIE_EP_MSI_NUM_01,				/**< msi number 1	*/
	E_DD_PCIE_EP_MSI_NUM_02,				/**< msi number 2	*/
	E_DD_PCIE_EP_MSI_NUM_03,				/**< msi number 3	*/
	E_DD_PCIE_EP_MSI_NUM_04,				/**< msi number 4	*/
	E_DD_PCIE_EP_MSI_NUM_05,				/**< msi number 5	*/
	E_DD_PCIE_EP_MSI_NUM_06,				/**< msi number 6	*/
	E_DD_PCIE_EP_MSI_NUM_07,				/**< msi number 7	*/
	E_DD_PCIE_EP_MSI_NUM_08,				/**< msi number 8	*/
	E_DD_PCIE_EP_MSI_NUM_09,				/**< msi number 9	*/
	E_DD_PCIE_EP_MSI_NUM_10,				/**< msi number 10	*/
	E_DD_PCIE_EP_MSI_NUM_11,				/**< msi number 11	*/
	E_DD_PCIE_EP_MSI_NUM_12,				/**< msi number 12	*/
	E_DD_PCIE_EP_MSI_NUM_13,				/**< msi number 13	*/
	E_DD_PCIE_EP_MSI_NUM_14,				/**< msi number 14	*/
	E_DD_PCIE_EP_MSI_NUM_15,				/**< msi number 15	*/
	E_DD_PCIE_EP_MSI_NUM_16,				/**< msi number 16	*/
	E_DD_PCIE_EP_MSI_NUM_17,				/**< msi number 17	*/
	E_DD_PCIE_EP_MSI_NUM_18,				/**< msi number 18	*/
	E_DD_PCIE_EP_MSI_NUM_19,				/**< msi number 19	*/
	E_DD_PCIE_EP_MSI_NUM_20,				/**< msi number 20	*/
	E_DD_PCIE_EP_MSI_NUM_21,				/**< msi number 21	*/
	E_DD_PCIE_EP_MSI_NUM_22,				/**< msi number 22	*/
	E_DD_PCIE_EP_MSI_NUM_23,				/**< msi number 23	*/
	E_DD_PCIE_EP_MSI_NUM_24,				/**< msi number 24	*/
	E_DD_PCIE_EP_MSI_NUM_25,				/**< msi number 25	*/
	E_DD_PCIE_EP_MSI_NUM_26,				/**< msi number 26	*/
	E_DD_PCIE_EP_MSI_NUM_27,				/**< msi number 27	*/
	E_DD_PCIE_EP_MSI_NUM_28,				/**< msi number 28	*/
	E_DD_PCIE_EP_MSI_NUM_29,				/**< msi number 29	*/
	E_DD_PCIE_EP_MSI_NUM_30,				/**< msi number 30	*/
	E_DD_PCIE_EP_MSI_NUM_31					/**< msi number 31	*/
} E_DD_PCIE_EP_MSI_NUM;

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
/** PCIe interrupt x callback. */
typedef struct{
	VP_CALLBACK_PCIE	msg_unlock_cb;	/**< radm_msg_unlock callback	*/
	VP_CALLBACK_PCIE	pm_turnof_cb;	/**< radm_pm_turnof callback	*/
} T_DD_PCIE_EP_INT_EP_CB;

/** PCIe interrupt own callback. */
typedef struct{
	VP_CALLBACK_PCIE	lu_cb;	/**< Link Up callback			*/
	VP_CALLBACK_PCIE	ce_cb;	/**< Correctable Error callback	*/
	VP_CALLBACK_PCIE	nfe_cb;	/**< Non-Fatal Error callback	*/
	VP_CALLBACK_PCIE	fe_cb;	/**< Fatal Error callback		*/
} T_DD_PCIE_EP_INT_OWN_CB;

#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
// Nothing Special
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
// Nothing Special
#ifdef CO_DDIM_UTILITY_USE
//---------------------- utility section -------------------------------
// Nothing Special
//---------------------- colabo  section -------------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Function 															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
/**
Initialize PCIe Endpoint.<br>
@retval D_DDIM_OK			OK
@retval D_DD_PCIE_TIMEOUT	Initialize timeout NG.
@remarks This API uses DDIM_User_Dly_Tsk().
*/
extern INT32	Dd_Pcie_Ep_Init(VOID);

/**
PCIe end.<br>
*/
extern VOID		Dd_Pcie_Ep_Deinit(VOID);

/**
Execute exclusive control for PCIe channel.<br>
@param [in]	tmout	Time of timeout<br>
						<ul><li>Positive Value(Time of timeout)
							<li>@ref D_DDIM_USER_SEM_WAIT_POL
							<li>@ref D_DDIM_USER_SEM_WAIT_FEVR</ul>
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_INPUT_PARAM_ERROR	Input parameter error.
@retval D_DD_PCIE_SEM_TIMEOUT		Semaphore acquisition Time Out.
@retval D_DD_PCIE_SEM_NG			Semaphore acquisition NG.
@remarks This API uses DDIM_User_Pol_Sem() when wait_time is set to 0. <br>
		 This API uses DDIM_User_Twai_Sem() when wait_time is set to the value except for 0.
*/
extern INT32	Dd_Pcie_Ep_Open(INT32 tmout);

/**
Cancel exclusive control for PCIe channel.<br>
@retval D_DDIM_OK			OK
@retval D_DD_AUDIO_SEM_NG	Semaphore release NG.
@remarks This API uses DDIM_User_Sig_Sem().
*/
extern INT32	Dd_Pcie_Ep_Close(VOID);

/**
Get PCIe link status.<br>
@retval TRUE	link up
@retval FALSE	link down
*/
extern BOOL		Dd_Pcie_Ep_Get_Link_Status(VOID);

/**
It will be setting up the dma transfer.<br>
@param[in]	pcie_ctrl_dma		dma control table.	See @ref T_DD_PCIE_CTRL_DMA.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_INPUT_PARAM_ERROR	Input parameter error.
@retval D_DD_PCIE_ACCESS_ERR		PCIe power down.(Dd_Pcie_Ep_Init unexecution)
@remarks
	- With the function for the opposing device to make the memory light a trigger and to generate interrupt, DMA Remote Interrupt can be used.
*/
extern INT32	Dd_Pcie_Ep_Ctrl_Dma(T_DD_PCIE_CTRL_DMA const* const pcie_ctrl_dma);

/**
Start DMA.<br>
@param[in]	dma_ch	Dma channel	See @ref E_DD_PCIE_DMA_CH.
@param[in]	dir		Direction	See @ref E_DD_PCIE_DMA_DIR.
@retval D_DDIM_OK				OK
@retval D_DD_PCIE_ACCESS_ERR	PCIe power down.(Dd_Pcie_Ep_Init unexecution)
*/
extern INT32	Dd_Pcie_Ep_Start_Dma(E_DD_PCIE_DMA_CH dma_ch, E_DD_PCIE_DMA_DIR dir);

/**
Stop DMA.<br>
@param[in]	dma_ch	Dma channel	See @ref E_DD_PCIE_DMA_CH.
@param[in]	dir		Direction	See @ref E_DD_PCIE_DMA_DIR.
@retval D_DDIM_OK				OK
@retval D_DD_PCIE_ACCESS_ERR	PCIe power down.(Dd_Pcie_Ep_Init unexecution)
*/
extern INT32	Dd_Pcie_Ep_Stop_Dma(E_DD_PCIE_DMA_CH dma_ch, E_DD_PCIE_DMA_DIR dir);

/**
It will be setting up the transfer of memory. CPU transfer.<br>
@param[in]	pcie_ctrl_mem		memory control data. See @ref T_DD_PCIE_CTRL_MEM.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_INPUT_PARAM_ERROR	Input parameter error.
@retval D_DD_PCIE_COMM_ERROR		Communication error. (Not link up)
@remarks 
			- Please use by Link Up state.
*/
extern INT32	Dd_Pcie_Ep_Ctrl_Memory(T_DD_PCIE_CTRL_MEM const* const pcie_ctrl_mem);

/**
It will be controll the INTx send request.<br>
@retval	SYS_INT Status value. (0:Assert_INTx message was send / 1:Deassert_INTx message was send)
@retval D_DD_PCIE_ACCESS_ERR	PCIe power down. (Dd_Pcie_Ep_Init unexecution)
@remarks
			- This API is reverse the SYS_INT value, and return the SYS_INT value after reverved.
			- If the SYS_INT is 0 to 1, send the Assert_INTx message.
			- If the SYS_INT is 1 to 0, send the Deassert_INTx message.
*/
extern UINT32	Dd_Pcie_Ep_Req_Int_EP(VOID);

/**
It will be controll the MSI transfer request.
@param[in]	msi_num		MSI number. See @ref E_DD_PCIE_EP_MSI_NUM.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_ACCESS_ERR		PCIe power down. (Dd_Pcie_Ep_Init unexecution)
*/
extern UINT32	Dd_Pcie_Ep_Transfer_Msi(E_DD_PCIE_EP_MSI_NUM msi_num);

/**
Interrupt handler for PCIe ch0 ep.<br>
*/
extern VOID		Dd_Pcie_Ep_Ch0_Int_Ep_Handler(VOID);

/**
Interrupt handler for PCIe ch0 own.<br>
*/
extern VOID		Dd_Pcie_Ep_Ch0_Int_Own_Handler(VOID);

/**
Interrupt handler for PCIe ch0 dma.<br>
*/
extern VOID		Dd_Pcie_Ep_Ch0_Int_Dma_Handler(VOID);

/**
Set the function to call back when INT_EP interrupt occurs.
@param [in] int_ep_cb	callback function table. See @ref T_DD_PCIE_EP_INT_EP_CB.
@retval D_DDIM_OK	OK
*/
extern INT32	Dd_Pcie_Ep_Set_Int_Ep_Func(T_DD_PCIE_EP_INT_EP_CB const* const int_ep_cb);

/**
Set the function to call back when INT_OWN interrupt occurs.
@param [in] int_own_cb	callback function table. See @ref T_DD_PCIE_EP_INT_OWN_CB.
@retval D_DDIM_OK	OK
@remarks 
			- The lu_cb setting in Link up is invalid.
*/
extern INT32	Dd_Pcie_Ep_Set_Int_Own_Func(T_DD_PCIE_EP_INT_OWN_CB const* const int_own_cb);

/**
ASPM control.
@param[in]	aspm_control	ASPM control.	See @ref E_DD_PCIE_ASPM_CONTROL.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_ACCESS_ERR		Power status not D0.<br>
									Link status is not L0/L0s/L1.
*/
extern INT32	Dd_Pcie_Ep_Ctrl_Aspm(E_DD_PCIE_ASPM_CONTROL aspm_control);

/**
Get PCIe Power status.
@retval See @ref E_DD_PCIE_PMST.
*/
extern E_DD_PCIE_PMST	Dd_Pcie_Ep_Get_PMST_Status(VOID);

/**
Get PCIe LTSSM status.
@retval See @ref E_DD_PCIE_ST_LTSSM.
*/
extern E_DD_PCIE_ST_LTSSM	Dd_Pcie_Ep_Get_ST_LTSSM_Status(VOID);

/**
Transfer speed change.
@param[in]	link_speed	Link Speed. See @ref E_DD_PCIE_LINK_SPEED.
@retval D_DDIM_OK				OK
@retval D_DD_PCIE_ACCESS_ERR	Link status is not L0/L0s/L1.
@remarks
		- It opposite device is 5.0GT/s support.<br>
*/
extern INT32	Dd_Pcie_Ep_Transfer_Speed_Change(E_DD_PCIE_LINK_SPEED link_speed);

/**
Get current link speed.
@param[in]	current_link_speed	Current link speed. See @ref E_DD_PCIE_LINK_SPEED.
@retval D_DDIM_OK				OK
@retval D_DD_PCIE_ACCESS_ERR	PCIe power down. (Dd_Pcie_Ep_Init unexecution)
*/
extern INT32	Dd_Pcie_Ep_Get_Current_Link_Speed(E_DD_PCIE_LINK_SPEED* current_link_speed);

/**
Get ASPM status.
@param[in]	aspm_status	ASPM status. See @ref E_DD_PCIE_ASPM_CONTROL.
@retval D_DDIM_OK				OK
@retval D_DD_PCIE_ACCESS_ERR	PCIe power down. (Dd_Pcie_Ep_Init unexecution)
*/
extern INT32	Dd_Pcie_Ep_Get_Aspm_Status(E_DD_PCIE_ASPM_CONTROL* aspm_status);

/**
Set max payload size.
@param[in]	mps		Max Payload Size. See @ref E_DD_PCIE_MPS.
@retval D_DDIM_OK				OK
@retval D_DD_PCIE_ACCESS_ERR	PCIe power down. (Dd_Pcie_Ep_Init unexecution)
*/
extern INT32 Dd_Pcie_Ep_Set_Mps(E_DD_PCIE_MPS mps);

/**
Get max payload size.
@param[out]	mps		Max Payload Size. See @ref E_DD_PCIE_MPS.
@retval D_DDIM_OK				OK
@retval D_DD_PCIE_ACCESS_ERR	PCIe power down. (Dd_Pcie_Ep_Init unexecution)
*/
extern INT32 Dd_Pcie_Ep_Get_Mps(E_DD_PCIE_MPS* mps);

#ifdef CO_DDIM_UTILITY_USE
/** @name Utility Functions
@{*/
//---------------------- utility section -------------------------------
/**
Get EXSTOP PCIe register.
@param[in]	offset	Offset of EXSTOP PCIe register.
@param[out]	data	Get data.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_INPUT_PARAM_ERROR	Violation of word boundary or reservation register.
@retval D_DD_PCIE_ACCESS_ERR		PCIe power down. (Dd_Pcie_Ep_Init unexecution)
*/
extern INT32 Dd_Pcie_Ep_Get_Exs_Reg(USHORT offset, UINT32* data);

/**
Set EXSTOP PCIe register.
@param[in]	offset	Offset of EXSTOP PCIe register.
@param[in]	data	Set data.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_INPUT_PARAM_ERROR	Violation of word boundary or reservation register.
@retval D_DD_PCIE_ACCESS_ERR		PCIe power down. (Dd_Pcie_Ep_Init unexecution)
*/
extern INT32 Dd_Pcie_Ep_Set_Exs_Reg(USHORT offset, UINT32 data);

/**
Get DBI AXI Slave register.
@param[in]	offset	Offset of DBI AXI Slave register.
@param[out]	data	Get data.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_INPUT_PARAM_ERROR	Violation of word boundary or reservation register.
@retval D_DD_PCIE_ACCESS_ERR		PCIe power down. (Dd_Pcie_Ep_Init unexecution)
*/
extern INT32 Dd_Pcie_Ep_Get_Dbi_Reg(USHORT offset, UINT32* data);

/**
Set DBI AXI Slave register.
@param[in]	offset	Offset of DBI AXI Slave register.
@param[in]	data	Set data.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_INPUT_PARAM_ERROR	Violation of word boundary or reservation register.
@retval D_DD_PCIE_ACCESS_ERR		PCIe power down. (Dd_Pcie_Ep_Init unexecution)
*/
extern INT32 Dd_Pcie_Ep_Set_Dbi_Reg(USHORT offset, UINT32 data);

/**
Set write access to DBI read-only register.
@param[in]	access	DBI Read only register access mode. (TRUE:access, FALSE:not access)
@retval D_DDIM_OK	OK
@retval D_DD_PCIE_ACCESS_ERR	PCIe power down.(Dd_Pcie_Ep_Init unexecution)
*/
extern INT32	Dd_Pcie_Ep_Set_Wr_Acc_To_Ro_Reg(BOOL access);

//---------------------- colabo  section -------------------------------
// Nothing Special

/*  @}*/
#endif


#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

#endif	// _DD_PCIE_EP_H_

/*@}*/

/**
@weakgroup dd_pcie_ep
@{

	<br>
	<br>
	The example of PCIe is as follows.<br>
	Sample code, use the PCIE to the opposing device.
	<hr>
	@section PCIe_EP_Sample_Initialize	Sample of Initialize.
	@code
	static VOID pcie_ep_lu_cb(INT32 status)
	{
		Ddim_Print(("Link Up.\n"));
	}

	VOID PCIe_EP_Sample_Initialize(VOID)
	{
		INT32 ret;
		T_DD_PCIE_EP_INT_OWN_CB int_own_cb

		memset(&int_own_cb, 0, sizeof(int_own_cb));
		int_own_cb.lu_cb = (VP_CALLBACK_PCIE)pcie_ep_lu_cb;
		Dd_Pcie_Ep_Set_Int_Own_Func(&int_own_cb);

		ret = Dd_Pcie_Ep_Init();
		if(ret != D_DDIM_OK){
			Ddim_Print(("Dd_Pcie_Ep_Init Error. ret=0x%08x\n", ret));
			return;
		}

		ret = Dd_Pcie_EP_Open(D_DDIM_USER_SEM_WAIT_POL);
		if(ret != D_DDIM_OK){
			Ddim_Print(("Dd_Pcie_Ep_Open Error. ret=0x%08x\n", ret));
			return;
		}
	}
	@endcode

	<br>
	<br>
	@section PCIe_EP_Sample_Speed_Change	Sample of transfer speed change.
	@code
	VOID PCIe_EP_Sample_Speed_Change(VOID)
	{
		INT32 ret;

		if(Dd_Pcie_Ep_Get_Link_Status() == TRUE){
			ret = Dd_Pcie_Ep_Transfer_Speed_Change(E_DD_PCIE_LINK_SPEED_GEN2);	// GEN1(2.5GT/s) -> GEN2(5.0GT/s)
			if(ret == D_DDIM_OK){
				Ddim_Print(("Current Link Speed. %d\n", Dd_Pcie_Ep_Get_Current_Link_Speed()));
			}
			else{
				Ddim_Print(("Dd_Pcie_Ep_Transfer_Speed_Change Error. ret=0x%08x\n", ret));
			}
		}
		else{
			Ddim_Print(("Transfer speed change is not available.\n"));
		}
	}
	@endcode

	<br>
	<br>
	@section PCIe_EP_Sample_Memory_Access	Sample of memory access.
	@code
	VOID PCIe_EP_Sample_Memory_Access(VOID)
	{
		INT32 ret;
		T_DD_PCIE_CTRL_MEM pcie_ctrl_mem;

		if(Dd_Pcie_Ep_Get_Link_Status() == TRUE){
			memset(&pcie_ctrl_mem, 0, sizeof(pcie_ctrl_mem));
			pcie_ctrl_mem.ch				= E_DD_PCIE_CH0;
			pcie_ctrl_mem.base_adr			= 0x19400000;
			pcie_ctrl_mem.limit_adr			= 0x1940ffff;
			pcie_ctrl_mem.low_target_adr	= 0x58000000;
			ret = Dd_Pcie_Ep_Ctrl_Memory(&pcie_ctrl_mem);
			if(ret == D_DDIM_OK){
				memcpy((VOID*)0x19400000, "PCIe Start 1", 12);	// RC write 0x19400000
				DDIM_User_L1l2cache_Clean_Flush_Addr(0x19400000, 12);
			}
			else{
				Ddim_Print(("Dd_Pcie_Ep_Ctrl_Memory Error. ret=0x%08x\n", ret));
			}
		}
		else{
			Ddim_Print(("Memory access is not available.\n"));
		}
	}
	@endcode

	<br>
	<br>
	@section PCIe_EP_Sample_Demand_DMA_Transfer	Sample of Demand mode DMA transfer.
	@code
	static VOID pcie_ep_int_dma_cb( INT32 status )
	{
		// status
		// 0x00000001  Done Interrupt status(DMA ch0)
		// 0x00000002  Done Interrupt status(DMA ch1)
		// 0x00010000  Abort Interrupt status(DMA ch0)
		// 0x00020000  Abort Interrupt status(DMA ch1)
	}

	VOID PCIe_EP_Sample_Demand_DMA_Transfer(VOID)
	{
		INT32 ret;
		T_DD_PCIE_CTRL_DMA pcie_ctrl_dma;

		memset(&pcie_ctrl_dma, 0, sizeof(pcie_ctrl_dma));
		pcie_ctrl_dma.mode				= E_DD_PCIE_DMA_DEMAND;
		pcie_ctrl_dma.ch				= E_DD_PCIE_CH0;
		pcie_ctrl_dma.dma_ch			= E_DD_PCIE_DMA_CH0;
		pcie_ctrl_dma.dmnd_src_addr		= 0x58000000;
		pcie_ctrl_dma.dmnd_dst_addr_l	= 0x5F000000;
		pcie_ctrl_dma.dmnd_trans_size	= 0x3FC000;
		pcie_ctrl_dma.direction			= E_DD_PCIE_DMA_DIR_STOD;
		pcie_ctrl_dma.int_dma_callback	= (VP_CALLBACK_PCIE)pcie_ep_int_dma_cb;
		ret = Dd_Pcie_Ep_Ctrl_Dma(&pcie_ctrl_dma);
		if(ret == D_DDIM_OK){
			Dd_Pcie_Ep_Start_Dma(pcie_ctrl_dma.ch, pcie_ctrl_dma.direction);
		}
		else{
			Ddim_Print(("Dd_Pcie_Ep_Ctrl_Dma Error. ret=0x%08x\n", ret));
		}
	}
	@endcode

	<br>
	<br>
	@section PCIe_EP_Sample_LL_DMA_Transfer	Sample of Linked List mode DMA Transfer.
	@code

	#define PCIE_DMA_LL_ADDR	(0x55000000)

	static VOID pcie_ep_ll_dma(VOID)
	{
		UINT32 Linked_list[22];

		Linked_list[0]	= 0x00000001;	// Channel Control
		Linked_list[1]	= 0x00000004;	// Transfer Size
		Linked_list[2]	= 0x58000000;	// SAR Low
		Linked_list[3]	= 0x0;			// SAR High
		Linked_list[4]	= 0x5F000000;	// DAR Low
		Linked_list[5]	= 0x0;			// DAR High
		// Element 1
		Linked_list[6]	= 0x00000001;	// Channel Control
		Linked_list[7]	= 0x00000004;	// Transfer Size
		Linked_list[8]	= 0x58000004;	// SAR Low
		Linked_list[9]	= 0x0;			// SAR High
		Linked_list[10]	= 0x5F000004;	// DAR Low
		Linked_list[11]	= 0x0;			// DAR High
		// Element 2
		Linked_list[12]	= 0x00000009;	// Channel Control (Local Interrupt Enable)
		Linked_list[13]	= 0x00000004;	// Transfer Size
		Linked_list[14]	= 0x58000008;	// SAR Low
		Linked_list[15]	= 0x0;			// SAR High
		Linked_list[16]	= 0x5F000008;	// DAR Low
		Linked_list[17]	= 0x0;			// DAR High
		// Element 3
		Linked_list[18]	= 0x00000004;	// Channel Control (Linked List End)
		Linked_list[19]	= 0x0;
		Linked_list[20]	= 0x0;
		Linked_list[21]	= 0x0;

		memcpy((void*)PCIE_DMA_LL_ADDR, (void*)Linked_list, sizeof(Linked_list));
		DDIM_User_L1l2cache_Clean_Flush_Addr(PCIE_DMA_LL_ADDR, sizeof(Linked_list));
	}

	VOID PCIe_EP_Sample_LL_DMA_Transfer(VOID)
	{
		INT32 ret;
		T_DD_PCIE_CTRL_DMA pcie_ctrl_dma;

		pcie_ep_ll_dma();
		memset(&pcie_ctrl_dma, 0, sizeof(pcie_ctrl_dma));
		pcie_ctrl_dma.mode				= E_DD_PCIE_DMA_LL_MODE;
		pcie_ctrl_dma.ch				= E_DD_PCIE_CH0;
		pcie_ctrl_dma.dma_ch			= E_DD_PCIE_DMA_CH0;
		pcie_ctrl_dma.ll_addr			= PCIE_DMA_LL_ADDR;
		pcie_ctrl_dma.direction			= E_DD_PCIE_DMA_DIR_STOD;
		pcie_ctrl_dma.int_dma_callback	= (VP_CALLBACK_PCIE)pcie_ep_int_dma_cb;
		ret = Dd_Pcie_Ep_Ctrl_Dma(&pcie_ctrl_dma);
		if(ret == D_DDIM_OK){
			Dd_Pcie_Ep_Start_Dma(pcie_ctrl_dma.ch, pcie_ctrl_dma.direction);
		}
		else{
			Ddim_Print(("Dd_Pcie_Ep_Ctrl_Dma Error. ret=0x%08x\n", ret));
		}
	}
	@endcode

	<br>
	<br>
	@section PCIe_EP_Sample_ASPM	Sample of ASPM.
	@code
	VOID PCIe_EP_Sample_ASPM(VOID)
	{
		INT32 ret;

		if(Dd_Pcie_Ep_Get_Link_Status() == TRUE){
			ret = Dd_Pcie_Ep_Ctrl_Aspm(E_DD_PCIE_ASPM_CONTROL_L0S_L1);
			if(ret == D_DDIM_OK){
				Ddim_Print(("Dd_Pcie_Ep_Ctrl_Aspm OK.\n"));
			}
			else{
				Ddim_Print(("Dd_Pcie_Ep_Ctrl_Aspm Error. ret=0x%08x\n", ret));
			}
		}
		else{
			Ddim_Print(("ASPM is not available.\n"));
		}
	}
	@endcode

	<br>
	<br>
	@section PCIe_EP_Sample_Message		Sample of Message transfer.
	@code
	// Message transfer. -------------------------------------------------------
	// MSI transfer-------------------------------------------------------------
	VOID PCIe_EP_Sample_Transfer_MSI(VOID)
	{
		INT32  ret;

		if(Dd_Pcie_Ep_Get_Link_Status() == TRUE){
			ret =Dd_Pcie_Ep_Transfer_Msi(E_DD_PCIE_EP_MSI_NUM_00);
			if(ret == D_DDIM_OK){
			}
			else{
				Ddim_Print(("Dd_Pcie_Ep_Transfer_Msi Error. ret=0x%08x\n", ret));
			}
		}
		else{
			Ddim_Print(("Configuration is not available.\n"));
		}
	}

	// INTx message ------------------------------------------------------------

	VOID PCIe_EP_Sample_INTx_message(VOID)
	{
		INT32 ret;
		UINT32 msg_f0_status;

		if(Dd_Pcie_Ep_Get_Link_Status() == TRUE){
			msg_f0_status = Dd_Pcie_Ep_Req_Int_EP();
			if(msg_f0_status == 0){
				Ddim_Print(("Dd_Pcie_Ep_Req_Int_EP INTx Deassert\n"));
			}
			else{
				Ddim_Print(("Dd_Pcie_Ep_Req_Int_EP INTx Assert\n"));
			}
		}
		else{
			Ddim_Print(("Configuration is not available.\n"));
		}
	}
	@endcode

	<br>
	<br>
	@section PCIe_EP_Sample_Interrupt	Sample of Interrupt.
	@code
	// Interrupt of INT_EP. -----------------------------------------------
	static VOID pcie_ep_int_ep_msg_unlock_cb(INT32 status)
	{
		// status = 0 (none)
	}

	static VOID pcie_ep_int_ep_pm_turnof_cb(INT32 status)
	{
		// status = 0 (none)
	}

	VOID PCIe_EP_Sample_INT_EP(VOID)
	{
		T_DD_PCIE_EP_INT_EP_CB int_ep_cb;

		int_ep_cb.msg_unlock_cb	= (VP_CALLBACK_PCIE)pcie_ep_int_ep_msg_unlock_cb;
		int_ep_cb.pm_turnof_cb	= (VP_CALLBACK_PCIE)pcie_ep_int_ep_pm_turnof_cb;
		Dd_Pcie_Ep_Set_Int_Ep_Func(&int_ep_cb);
	}

	// Interrupt of INT_OWN.  ------------------------------------------
	static VOID pcie_ep_int_own_lu_cb(INT32 status)
	{
		// status = 0 (none)
	}
	static VOID pcie_ep_int_own_ce_cb(INT32 status)
	{
		// status = 0 (none)
	}
	static VOID pcie_ep_int_own_nfe_cb(INT32 status)
	{
		// status = 0 (none)
	}
	static VOID pcie_ep_int_own_fe_cb(INT32 status)
	{
		// status = 0 (none)
	}

	VOID PCIe_EP_Sample_INT_OWN(VOID)
	{
		T_DD_PCIE_EP_INT_OWN_CB int_own_cb;

		int_own_cb.lu_cb	= pcie_ep_int_own_lu_cb;
		int_own_cb.ce_cb	= pcie_ep_int_own_ce_cb;
		int_own_cb.nfe_cb	= pcie_ep_int_own_nfe_cb;
		int_own_cb.fe_cb	= pcie_ep_int_own_fe_cb;
		Dd_Pcie_Ep_Set_Int_Own_Func(&int_own_cb);
	}
	@endcode

	<br>
	<br>
	@section PCIe_EP_Sample_End		Sample of PCIe End.
	@code
	VOID PCIe_EP_Sample_End(VOID)
	{
		Dd_Pcie_Ep_Close();
		Dd_Pcie_Ep_Deinit();
	}
	@endcode

@}*/
