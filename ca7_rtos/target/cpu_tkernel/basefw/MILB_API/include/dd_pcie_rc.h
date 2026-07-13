/**
 * @file		dd_pcie_rc.h
 * @brief		PCIe RootComplex driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
/**
@weakgroup dd_pcie_rc
@{

<hr>

@section dd_pcie_rc_sequence Sequence Chart
- @ref PCIe_RC_Sequence_Initialize
- @ref PCIe_RC_Sequence_Configuration
- @ref PCIe_RC_Sequence_Speed_Change
- @ref PCIe_RC_Sequence_Memory_Access
- @ref PCIe_RC_Sequence_DMA_Transfer
- @ref PCIe_RC_Sequence_MSI_Transfer
- @ref PCIe_RC_Sequence_ASPM
- @ref PCIe_RC_Sequence_PM_D1D2
- @ref PCIe_RC_Sequence_end
@section dd_pcie_rc_sample Sample Code
- @ref PCIe_RC_Sample_Initialize
- @ref PCIe_RC_Sample_Configuration
- @ref PCIe_RC_Sample_Speed_Change
- @ref PCIe_RC_Sample_Memory_Access
- @ref PCIe_RC_Sample_Demand_DMA_Transfer
- @ref PCIe_RC_Sample_LL_DMA_Transfer
- @ref PCIe_RC_Sample_ASPM
- @ref PCIe_RC_Sample_Interrupt
- @ref PCIe_RC_Sample_End
- @ref PCIe_RC_Sample_L2_Exit
- @ref PCIe_RC_Sample_Detect_Card

@section PCIe_RC_Sequence_Initialize	Initial setting.
@image html dd_pcie_rc_Initialize.png

@section PCIe_RC_Sequence_Configuration	Configuration access.
@image html dd_pcie_rc_configuration.png

@section PCIe_RC_Sequence_Speed_Change	Change of transfer speed.
@image html dd_pcie_rc_transfer_speed_change.png

@section PCIe_RC_Sequence_Memory_Access	Access to the memory of Endpoint side.
@image html dd_pcie_rc_memory.png

@section PCIe_RC_Sequence_DMA_Transfer	Data transfer by DMA.
@image html dd_pcie_rc_dma_transfer.png

@section PCIe_RC_Sequence_MSI_Transfer	MSI transfer.
@image html dd_pcie_rc_MSI_transfer.png

@section PCIe_RC_Sequence_ASPM			Control the power management by ASPM(Active State Power Management).
@image html dd_pcie_rc_ASPM.png

@section PCIe_RC_Sequence_PM_D1D2		Control the power management by software.
@image html dd_pcie_rc_PM_D1D2_by_SW.png

@section PCIe_RC_Sequence_end			PCIe end.
@image html dd_pcie_rc_end.png

@}
*/

/**
@addtogroup dd_pcie_rc
@{

*/

#ifndef _DD_PCIE_RC_H_
#define _DD_PCIE_RC_H_

#include "driver_common.h"
#include "exstop.h"
#include "dd_pcie_comm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//---------------------- driver  section -------------------------------
#define D_DD_PCIE_RC_MAX_CH		(2)		/**< The maximum number of channels */

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
/** Hot-Plug Status */
typedef enum {
	E_DD_PCIE_HOT_PLUG_ST_ABP = 0,	/**< Attention Button Pressed		*/
	E_DD_PCIE_HOT_PLUG_ST_PFD,		/**< Power Fault Detected			*/
	E_DD_PCIE_HOT_PLUG_ST_MRLSC,	/**< MRL Sensor Changed				*/
	E_DD_PCIE_HOT_PLUG_ST_PDC,		/**< Presence Detect Changed		*/
	E_DD_PCIE_HOT_PLUG_ST_CC,		/**< Command Completed				*/
	E_DD_PCIE_HOT_PLUG_ST_DLLSC		/**< Data Link Layer State Change	*/
} E_DD_PCIE_HOT_PLUG_ST;

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
/** PCIe config control data */
typedef struct{
	E_DD_PCIE_CH		ch;				/**< PCIe channel			*/
	UINT32				base_adr;		/**< Lower base address		*/
	UINT32				limit_adr;		/**< limit address			*/
} T_DD_PCIE_RC_CTRL_CFG;

/** PCIe msi control data */
typedef struct{
	E_DD_PCIE_CH		ch;				/**< PCIe channel			*/
	UINT32				int_en;			/**< Interrupt enable		*/
	UINT32				cmp_adr;		/**< compare address	*/
	VP_CALLBACK_PCIE	int_msi_cb;		/**< MSI callback			*/
} T_DD_PCIE_RC_CTRL_MSI;

/** PCIe interrupt trs_ics_msi callback */
typedef struct{
	VP_CALLBACK_PCIE	cfg_link_auto_bw_int_cb;	/**< Callback of cfg_link_auto_bw_int port interrupt	*/
	VP_CALLBACK_PCIE	cfg_bw_mgt_int_cb;			/**< Callback of cfg_bw_mgt_int port interrupt			*/
	VP_CALLBACK_PCIE	smlh_req_rst_not_cb;		/**< Callback of smlh_req_rst_not port interrupt		*/
} T_DD_PCIE_RC_INT_TRS_ICS_MSI_CB;

/** PCIe interrupt system error callback. */
typedef struct{
	VP_CALLBACK_PCIE	cfg_aer_rc_err_cb;			/**< Callback of cfg_aer_rc_err_int/msi port interrupt	*/
	VP_CALLBACK_PCIE	cfg_sys_err_rc_cb;			/**< Callback of cfg_sys_err_rc port interrupt			*/
} T_DD_PCIE_RC_INT_SYS_ERR_CB;

/** PCIe interrupt EP callback */
typedef struct{
	VP_CALLBACK_PCIE	radm_intx_cb;				/**< Callback of radm_inta/b/c/d port interrupt			*/
	VP_CALLBACK_PCIE	radm_pm_pme_cb;				/**< Callback of radm_pm_pme port interrupt				*/
	VP_CALLBACK_PCIE	radm_correctable_err_cb;	/**< Callback of radm_correctable_err port interrupt	*/
	VP_CALLBACK_PCIE	radm_nonfatal_err_cb;		/**< Callback of radm_nonfatal_err port interrupt		*/
	VP_CALLBACK_PCIE	radm_fatal_err_cb;			/**< Callback of radm_fatal_err port interrupt			*/
	VP_CALLBACK_PCIE	radm_msg_ltr_cb;			/**< Callback of radm_msg_ltr port interrupt			*/
	VP_CALLBACK_PCIE	radm_pm_to_ack_cb;			/**< Callback of radm_pm_to_ack port interrupt			*/
	VP_CALLBACK_PCIE	radm_vendor_msg_cb;			/**< Callback of radm_vendor_msg port interrupt			*/
} T_DD_PCIE_RC_INT_EP_CB;

/** PCIe interrupt own callback. */
typedef struct{
	VP_CALLBACK_PCIE	cfg_pme_cb;					/**< Callback of cfg_pme_int/msi port interrupt		*/
	VP_CALLBACK_PCIE	hp_cb;						/**< Callback of hp_pme/int/msi port interrupt		*/
	VP_CALLBACK_PCIE	rdlh_link_up_cb;			/**< Callback of rdlh_link_up port interrupt		*/
	VP_CALLBACK_PCIE	wake_cb;					/**< Callback of wake port interrupt				*/
} T_DD_PCIE_RC_INT_OWN_CB;

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
Initialize PCIe Root Complex.<br>
@param[in]	bifurcation		See @ref E_DD_PCIE_BIFURCATION.
*/
extern VOID		Dd_Pcie_Rc_Init(E_DD_PCIE_BIFURCATION bifurcation);

/**
PCIe end.<br>
*/
extern VOID		Dd_Pcie_Rc_Deinit(VOID);

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
extern INT32	Dd_Pcie_Rc_Open(INT32 tmout);

/**
Cancel exclusive control for PCIe channel.<br>
@retval D_DDIM_OK			OK
@retval D_DD_PCIE_SEM_NG	Semaphore release NG.
@remarks This API uses DDIM_User_Sig_Sem().
*/
extern INT32	Dd_Pcie_Rc_Close(VOID);

/**
Get PCIe link status.<br>
@param[in]	ch	Pcie channel.	See @ref E_DD_PCIE_CH.
@retval TRUE	link up
@retval FALSE	link down
@remarks In the case of bifurcation disable, ch1 can not be used.
*/
extern BOOL		Dd_Pcie_Rc_Get_Link_Status(E_DD_PCIE_CH ch);

/**
It will be setting up the transfer of config.<br>
@param[in]	pcie_ctrl_cfg			config control data. See @ref T_DD_PCIE_RC_CTRL_CFG.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_INPUT_PARAM_ERROR	Input parameter error.
@retval D_DD_PCIE_COMM_ERROR		Communication error. (Not link up)
@remarks In the case of bifurcation disable, ch1 can not be used.
*/
extern INT32	Dd_Pcie_Rc_Ctrl_Config(T_DD_PCIE_RC_CTRL_CFG const* const pcie_ctrl_cfg);

/**
It will be setting up the dma transfer.<br>
@param[in]	pcie_ctrl_dma	dma control data.	See @ref T_DD_PCIE_CTRL_DMA.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_INPUT_PARAM_ERROR	Input parameter error.
@retval D_DD_PCIE_ACCESS_ERR		PCIe power down.(Dd_Pcie_Rc_Init unexecution)
@remarks
	- With the function for the opposing device to make the memory light a trigger and to generate interrupt, DMA Remote Interrupt can be used. <br>
	- In the case of bifurcation disable, ch1 can not be used.
*/
extern INT32	Dd_Pcie_Rc_Ctrl_Dma(T_DD_PCIE_CTRL_DMA const* const pcie_ctrl_dma);

/**
Start DMA.<br>
@param[in]	ch		Pcie channel.	See @ref E_DD_PCIE_CH.
@param[in]	dma_ch	Dma channel.	See @ref E_DD_PCIE_DMA_CH.
@param[in]	dir		Direction.		See @ref E_DD_PCIE_DMA_DIR.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_ACCESS_ERR		PCIe power down.(Dd_Pcie_Rc_Init unexecution)
@remarks In the case of bifurcation disable, ch1 can not be used.
*/
extern INT32	Dd_Pcie_Rc_Start_Dma(E_DD_PCIE_CH ch, E_DD_PCIE_DMA_CH dma_ch, E_DD_PCIE_DMA_DIR dir);

/**
Stop DMA.<br>
@param[in]	ch		Pcie channel.	See @ref E_DD_PCIE_CH.
@param[in]	dma_ch	Dma channel.	See @ref E_DD_PCIE_DMA_CH.
@param[in]	dir		Direction.		See @ref E_DD_PCIE_DMA_DIR.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_ACCESS_ERR		PCIe power down.(Dd_Pcie_Rc_Init unexecution)
@remarks In the case of bifurcation disable, ch1 can not be used.
*/
extern INT32	Dd_Pcie_Rc_Stop_Dma(E_DD_PCIE_CH ch, E_DD_PCIE_DMA_CH dma_ch, E_DD_PCIE_DMA_DIR dir);

/**
It will be setting up the transfer of memory. CPU transfer.<br>
@param[in]	pcie_ctrl_mem			memory control data. See @ref T_DD_PCIE_CTRL_MEM.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_INPUT_PARAM_ERROR	Input parameter error.
@retval D_DD_PCIE_COMM_ERROR		Communication error. (Not link up)
@remarks 
			- Please use by Link Up state.
			- In the case of bifurcation disable, ch1 can not be used.
*/
extern INT32	Dd_Pcie_Rc_Ctrl_Memory(T_DD_PCIE_CTRL_MEM const* const pcie_ctrl_mem);

/**
It will be setting up the interrupt MSI.<br>
@param[in]	pcie_ctrl_msi	msi control data. See @ref T_DD_PCIE_RC_CTRL_MSI.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_INPUT_PARAM_ERROR	Input parameter error.
@retval D_DD_PCIE_ACCESS_ERR		PCIe power down.(Dd_Pcie_Rc_Init unexecution)
@remarks
			- Please use by Link Up state.
			- In the case of bifurcation disable, ch1 can not be used.
*/
extern INT32	Dd_Pcie_Rc_Ctrl_Msi(T_DD_PCIE_RC_CTRL_MSI const* const pcie_ctrl_msi);

/**
Interrupt handler for PCIe ch0 own.<br>
*/
extern VOID		Dd_Pcie_Rc_Ch0_Int_Own_Handler(VOID);

/**
Interrupt handler for PCIe ch0 endpoint.<br>
*/
extern VOID		Dd_Pcie_Rc_Ch0_Int_Ep_Handler(VOID);

/**
Interrupt handler for PCIe ch0 system error.<br>
*/
extern VOID		Dd_Pcie_Rc_Ch0_Int_Sys_Err_Handler(VOID);

/**
Interrupt handler for PCIe ch0 dma.<br>
*/
extern VOID		Dd_Pcie_Rc_Ch0_Int_Dma_Handler(VOID);

/**
Interrupt handler for PCIe ch0 Trs_Ics_Msi.<br>
*/
extern VOID		Dd_Pcie_Rc_Ch0_Int_Trs_Ics_Msi_Handler(VOID);

/**
Interrupt handler for PCIe ch1 own.<br>
*/
extern VOID		Dd_Pcie_Rc_Ch1_Int_Own_Handler(VOID);

/**
Interrupt handler for PCIe ch1 endpoint.<br>
*/
extern VOID		Dd_Pcie_Rc_Ch1_Int_Ep_Handler(VOID);

/**
Interrupt handler for PCIe ch1 system error.<br>
*/
extern VOID		Dd_Pcie_Rc_Ch1_Int_Sys_Err_Handler(VOID);

/**
Interrupt handler for PCIe ch1 dma.<br>
*/
extern VOID		Dd_Pcie_Rc_Ch1_Int_Dma_Handler(VOID);

/**
Interrupt handler for PCIe ch1 Trs_Ics_Msi.<br>
*/
extern VOID		Dd_Pcie_Rc_Ch1_Int_Trs_Ics_Msi_Handler(VOID);

/**
Interrupt handler for detect card of PCIe ch0.<br>
@param[in]	intst	Card edge detection. 6-7 bit.
*/
extern VOID		Dd_Pcie_Rc_Ch0_Int_Exs_Handler(ULONG intst);

/**
Interrupt handler for detect card of PCIe ch1.<br>
@param[in]	intst	Card edge detection. 8-9 bit.
*/
extern VOID		Dd_Pcie_Rc_Ch1_Int_Exs_Handler(ULONG intst);

/**
Set the function to call back when INT_OWN interrupt occurs.
@param[in]	ch			Pcie channel. See @ref E_DD_PCIE_CH.
@param [in] int_own_cb	callback function table. See @ref T_DD_PCIE_RC_INT_OWN_CB.
@retval D_DDIM_OK	OK
@remarks
			- The lu_cb setting in Link up is invalid.
			- When Dd_Pcie_Rc_Init is unexecution, fg_pme_cb/hp_cb/wake is not set.
*/
extern INT32	Dd_Pcie_Rc_Set_Int_Own_Func(E_DD_PCIE_CH ch, T_DD_PCIE_RC_INT_OWN_CB const* const int_own_cb);

/**
Set the function to call back when INT_EP interrupt occurs.
@param[in]	ch			Pcie channel. See @ref E_DD_PCIE_CH.
@param [in] int_ep_cb	callback function table. See @ref T_DD_PCIE_RC_INT_EP_CB.
@retval D_DDIM_OK	OK
@remarks
			- When Dd_Pcie_Rc_Init is unexecution, radm_intx_cb is not set.
*/
extern INT32	Dd_Pcie_Rc_Set_Int_Ep_Func(E_DD_PCIE_CH ch, T_DD_PCIE_RC_INT_EP_CB const* const int_ep_cb);

/**
Set the function to call back when INT_SY_SERR interrupt occurs.
@param[in]	ch			Pcie channel. See @ref E_DD_PCIE_CH.
@param [in] sys_err_cb	callback function table. See @ref T_DD_PCIE_RC_INT_SYS_ERR_CB.
@retval D_DDIM_OK	OK
@remarks
			- When Dd_Pcie_Rc_Init is unexecution, cfg_aer_rc_err_cb is not set.
*/
extern INT32	Dd_Pcie_Rc_Set_Int_Sys_Err_Func(E_DD_PCIE_CH ch, T_DD_PCIE_RC_INT_SYS_ERR_CB const* const sys_err_cb);

/**
Set the function to call back when INT_TRS_ICS_MSI interrupt occurs.
@param[in]	ch					Pcie channel. See @ref E_DD_PCIE_CH.
@param [in] int_trs_ics_msi_cb	callback function table. See @ref T_DD_PCIE_RC_INT_TRS_ICS_MSI_CB.
@retval D_DDIM_OK				OK
@remarks
			- When Dd_Pcie_Rc_Init is unexecution, cfg_link_auto_bw_int_cb/cfg_bw_mgt_int_cb is not set.
*/
extern INT32	Dd_Pcie_Rc_Set_Int_Trs_Ics_Msi_Func(E_DD_PCIE_CH ch, T_DD_PCIE_RC_INT_TRS_ICS_MSI_CB const* const int_trs_ics_msi_cb);

/**
ASPM control.
@param[in]	ch				Pcie channel.	See @ref E_DD_PCIE_CH.
@param[in]	aspm_control	ASPM control.	See @ref E_DD_PCIE_ASPM_CONTROL.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_ACCESS_ERR		Power status not D0.<br>
									Link status not L0/L0s/L1.
@remarks In the case of bifurcation disable, ch1 can not be used.
*/
extern INT32	Dd_Pcie_Rc_Ctrl_Aspm(E_DD_PCIE_CH ch, E_DD_PCIE_ASPM_CONTROL aspm_control);

/**
Power Turn Off Request. (L2 Entry)
@param[in]	ch	Pcie channel. See @ref E_DD_PCIE_CH.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_ACCESS_ERR		Link status is not L0.
@remarks In the case of bifurcation disable, ch1 can not be used.
*/
extern INT32	Dd_Pcie_Rc_Pm_Pw_Turn_Off_Req(E_DD_PCIE_CH ch);

/**
Power L2 Exit. (L0 Entry)
@param[in]	ch	Pcie channel. See @ref E_DD_PCIE_CH.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_ACCESS_ERR		Link status is not L2.
@remarks In the case of bifurcation disable, ch1 can not be used.
*/
extern INT32	Dd_Pcie_Rc_Pm_Pw_L2_Exit(E_DD_PCIE_CH ch);

/**
Get PCIe Power status.
@param[in]	ch	Pcie channel. See @ref E_DD_PCIE_CH.
@retval See @ref E_DD_PCIE_PMST.
@remarks In the case of bifurcation disable, ch1 can not be used.
*/
extern E_DD_PCIE_PMST		Dd_Pcie_Rc_Get_PMST_Status(E_DD_PCIE_CH ch);

/**
Get PCIe LTSSM status.
@param[in]	ch	Pcie channel. See @ref E_DD_PCIE_CH.
@retval See @ref E_DD_PCIE_ST_LTSSM.
@remarks In the case of bifurcation disable, ch1 can not be used.
*/
extern E_DD_PCIE_ST_LTSSM	Dd_Pcie_Rc_Get_ST_LTSSM_Status(E_DD_PCIE_CH ch);

/**
Transfer speed change.
@param[in]	ch	Pcie channel. See @ref E_DD_PCIE_CH.
@param[in]	link_speed	Link Speed. See @ref E_DD_PCIE_LINK_SPEED.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_ACCESS_ERR		Link status not L0/L0s/L1.
@remarks
		- If the opposing device supports  5.0GT/s, function can use.<br>
*/
extern INT32	Dd_Pcie_Rc_Transfer_Speed_Change(E_DD_PCIE_CH ch, E_DD_PCIE_LINK_SPEED link_speed);

/**
Get current link speed.
@param[in]	ch	Pcie channel. See @ref E_DD_PCIE_CH.
@param[in]	current_link_speed	Current link speed. See @ref E_DD_PCIE_LINK_SPEED.
@retval D_DDIM_OK				OK
@retval D_DD_PCIE_ACCESS_ERR	PCIe power down. (Dd_Pcie_Rc_Init unexecution)
*/
extern INT32	Dd_Pcie_Rc_Get_Current_Link_Speed(E_DD_PCIE_CH ch, E_DD_PCIE_LINK_SPEED* current_link_speed);

/**
Get ASPM status.
@param[in]	ch			Pcie channel. See @ref E_DD_PCIE_CH.
@param[in]	aspm_status	ASPM status. See @ref E_DD_PCIE_ASPM_CONTROL.
@retval D_DDIM_OK				OK
@retval D_DD_PCIE_ACCESS_ERR	PCIe power down. (Dd_Pcie_Rc_Init unexecution)
*/
extern INT32	Dd_Pcie_Rc_Get_Aspm_Status(E_DD_PCIE_CH ch, E_DD_PCIE_ASPM_CONTROL* aspm_status);

/**
Read the config read register.
@param[in]	ch		Pcie channel. See @ref E_DD_PCIE_CH.
@param[in]	offset	Offset of the read config register.
@param[in]	data	Read data.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_INPUT_PARAM_ERROR	address word boundary violation.
@retval D_DD_PCIE_COMM_ERROR		Communication error. (Not link up)
*/
extern INT32	Dd_Pcie_Rc_Config_Read(E_DD_PCIE_CH ch, USHORT offset, UINT32* data);

/**
Write the config read register.
@param[in]	ch		Pcie channel. See @ref E_DD_PCIE_CH.
@param[in]	offset	Offset of the write config register.
@param[in]	data	Write data.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_INPUT_PARAM_ERROR	address word boundary violation.
@retval D_DD_PCIE_COMM_ERROR		Communication error. (Not link up)
*/
extern INT32	Dd_Pcie_Rc_Config_Write(E_DD_PCIE_CH ch, USHORT offset, UINT32 data);

/**
It will be setting up Detect Card.
@param[in]	ch			Pcie channel. See @ref E_DD_PCIE_CH.
@param[in]	callback	callback function.
@retval D_DDIM_OK		OK
@remarks
			- Value of callback argument. 0:Insert , 1:Eject
*/
extern INT32 Dd_Pcie_Rc_Ctrl_Detect_Card(E_DD_PCIE_CH ch, VP_CALLBACK_PCIE callback);

/**
Set max payload size.
@param[in]	ch		Pcie channel. See @ref E_DD_PCIE_CH.
@param[in]	mps		Max Payload Size. See @ref E_DD_PCIE_MPS.
@retval D_DDIM_OK				OK
@retval D_DD_PCIE_ACCESS_ERR	PCIe power down. (Dd_Pcie_Rc_Init unexecution)
*/
extern INT32 Dd_Pcie_Rc_Set_Mps(E_DD_PCIE_CH ch, E_DD_PCIE_MPS mps);

/**
Get max payload size.
@param[in]	ch		Pcie channel. See @ref E_DD_PCIE_CH.
@param[out]	mps		Max Payload Size. See @ref E_DD_PCIE_MPS.
@retval D_DDIM_OK				OK
@retval D_DD_PCIE_ACCESS_ERR	PCIe power down. (Dd_Pcie_Rc_Init unexecution)
*/
extern INT32 Dd_Pcie_Rc_Get_Mps(E_DD_PCIE_CH ch, E_DD_PCIE_MPS* mps);

#ifdef CO_DDIM_UTILITY_USE
/** @name Utility Functions
@{*/
//---------------------- utility section -------------------------------
/**
Get EXSTOP PCIe register.
@param[in]	ch		Pcie channel. See @ref E_DD_PCIE_CH.
@param[in]	offset	Offset of EXSTOP PCIe register.
@param[out]	data	Get data.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_INPUT_PARAM_ERROR	Violation of word boundary or reservation register.
@retval D_DD_PCIE_ACCESS_ERR		PCIe power down. (Dd_Pcie_Rc_Init unexecution)
*/
extern INT32 Dd_Pcie_Rc_Get_Exs_Reg(E_DD_PCIE_CH ch, USHORT offset, UINT32* data);

/**
Set EXSTOP PCIe register.
@param[in]	ch		Pcie channel. See @ref E_DD_PCIE_CH.
@param[in]	offset	Offset of EXSTOP PCIe register.
@param[in]	data	Set data.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_INPUT_PARAM_ERROR	Violation of word boundary or reservation register.
@retval D_DD_PCIE_ACCESS_ERR		PCIe power down. (Dd_Pcie_Rc_Init unexecution)
*/
extern INT32 Dd_Pcie_Rc_Set_Exs_Reg(E_DD_PCIE_CH ch, USHORT offset, UINT32 data);

/**
Get DBI AXI Slave register.
@param[in]	ch		Pcie channel. See @ref E_DD_PCIE_CH.
@param[in]	offset	Offset of DBI AXI Slave register.
@param[out]	data	Get data.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_INPUT_PARAM_ERROR	Violation of word boundary or reservation register.
@retval D_DD_PCIE_ACCESS_ERR		PCIe power down. (Dd_Pcie_Rc_Init unexecution)
*/
extern INT32 Dd_Pcie_Rc_Get_Dbi_Reg(E_DD_PCIE_CH ch, USHORT offset, UINT32* data);

/**
Set DBI AXI Slave register.
@param[in]	ch		Pcie channel. See @ref E_DD_PCIE_CH.
@param[in]	offset	Offset of DBI AXI Slave register.
@param[out]	data	Set data.
@retval D_DDIM_OK					OK
@retval D_DD_PCIE_INPUT_PARAM_ERROR	Violation of word boundary or reservation register.
@retval D_DD_PCIE_ACCESS_ERR		PCIe power down. (Dd_Pcie_Rc_Init unexecution)
*/
extern INT32 Dd_Pcie_Rc_Set_Dbi_Reg(E_DD_PCIE_CH ch, USHORT offset, UINT32 data);

/**
Set write access to DBI read-only register.
@param[in]	ch	Pcie channel. See @ref E_DD_PCIE_CH.
@param[in]	access	DBI Read only register access mode. (TRUE:access, FALSE:not access)
@retval D_DDIM_OK				OK
@retval D_DD_PCIE_ACCESS_ERR	PCIe power down.(Dd_Pcie_Rc_Init unexecution)<br>
								It is not possible to access it because of Link up.
*/
extern INT32 Dd_Pcie_Rc_Set_Wr_Acc_To_Ro_Reg(E_DD_PCIE_CH ch, BOOL access);

//---------------------- colabo  section -------------------------------
// Nothing Special
/*  @}*/
#endif


#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

#endif	// _DD_PCIE_RC_H_

/*@}*/

/**
@weakgroup dd_pcie_rc
@{

	<br>
	<br>
	The example of PCIe is as follows.<br>
	Sample code, use the PCIE to the opposing device.
	<hr>
	@section PCIe_RC_Sample_Initialize	Sample of Initialize. (Bifurcation Disable)
	@code
	static VOID pcie_rc_link_up_cb(INT32 status)
	{
		Ddim_Print(("Link Up.\n"));
	}

	VOID PCIe_RC_Sample_Initialize(VOID)
	{
		INT32 ret;
		T_DD_PCIE_INIT_CONF pcie_init_conf;
		T_DD_PCIE_RC_INT_OWN_CB int_own_cb

		memset(&int_own_cb, 0, sizeof(int_own_cb));
		int_own_cb.rdlh_link_up_cb = (VP_CALLBACK_PCIE)pcie_rc_link_up_cb;
		Dd_Pcie_Rc_Set_Int_Own_Func(E_DD_PCIE_CH0, &int_own_cb);

		ret = Dd_Pcie_Rc_Init(E_DD_PCIE_BIFURCATION_DISABLE);
		if(ret != D_DDIM_OK){
			Ddim_Print(("Dd_Pcie_Rc_Init Error. ret=0x%08x\n", ret));
			return;
		}

		ret = Dd_Pcie_Rc_Open(D_DDIM_USER_SEM_WAIT_POL);
		if(ret != D_DDIM_OK){
			Ddim_Print(("Dd_Pcie_Rc_Open Error. ret=0x%08x\n", ret));
			return;
		}
	}
	@endcode

	<br>
	<br>
	@section PCIe_RC_Sample_Configuration	Sample of Configuration.
	@code
	VOID PCIe_RC_Sample_Configuration(VOID)
	{
		INT32 ret;
		T_DD_PCIE_RC_CTRL_CFG pcie_ctrl_cfg;

		if(Dd_Pcie_Rc_Get_Link_Status(E_DD_PCIE_CH0) == TRUE){
			memset(&pcie_ctrl_cfg, 0, sizeof(pcie_ctrl_cfg));
			pcie_ctrl_cfg.ch				= E_DD_PCIE_CH0;
			pcie_ctrl_cfg.base_adr			= 0x19400000;
			pcie_ctrl_cfg.limit_adr			= 0x1940ffff;
			ret = Dd_Pcie_Rc_Ctrl_Config(&pcie_ctrl_cfg);	// ch0 configration
			if(ret == D_DDIM_OK){
				Ddim_Print(("Configuration success.\n"));
			}
			else{
				Ddim_Print(("Dd_Pcie_Rc_Ctrl_Config Error. ret=0x%08x\n", ret));
			}
		}
		else{
			Ddim_Print(("Configuration is not available.\n"));
		}
	}
	@endcode

	<br>
	<br>
	@section PCIe_RC_Sample_Speed_Change	Sample of transfer speed change.
	@code
	VOID PCIe_RC_Sample_Speed_Change(VOID)
	{
		INT32 ret;
		E_DD_PCIE_LINK_SPEED tran_speed_stat;

		if(Dd_Pcie_Rc_Get_Link_Status(E_DD_PCIE_CH0) == TRUE){
			ret = Dd_Pcie_Rc_Transfer_Speed_Change(E_DD_PCIE_CH0, E_DD_PCIE_LINK_SPEED_GEN2);	// GEN1(2.5GT/s) -> GEN2(5.0GT/s)
			if(ret == D_DDIM_OK){
				Ddim_Print(("Current Link Speed. %d\n", Dd_Pcie_Rc_Get_Current_Link_Speed(E_DD_PCIE_CH0)));
			}
			else{
				Ddim_Print(("Dd_Pcie_Rc_Transfer_Speed_Change Error. ret=0x%08x\n", ret));
			}
		}
		else{
			Ddim_Print(("Transfer speed change is not available.\n"));
		}
	}
	@endcode

	<br>
	<br>
	@section PCIe_RC_Sample_Memory_Access	Sample of memory access.
	@code
	VOID PCIe_RC_Sample_Memory_Access(VOID)
	{
		INT32 ret;
		T_DD_PCIE_CTRL_MEM pcie_ctrl_mem;

		if(Dd_Pcie_Rc_Get_Link_Status(E_DD_PCIE_CH0) == TRUE){
			memset(&pcie_ctrl_mem, 0, sizeof(pcie_ctrl_mem));
			pcie_ctrl_mem.ch				= E_DD_PCIE_CH0;
			pcie_ctrl_mem.base_adr			= 0x19600000;
			pcie_ctrl_mem.limit_adr			= 0x1960ffff;
			pcie_ctrl_mem.low_target_adr	= 0x58000000;
			ret = Dd_Pcie_Rc_Ctrl_Memory(&pcie_ctrl_mem);
			if(ret == D_DDIM_OK){
				memcpy((VOID*)0x0x19600000, "PCIe Start 1", 12);
				DDIM_User_L1l2cache_Clean_Flush_Addr(0x0x19600000, 12);
			}
			else{
				Ddim_Print(("Dd_Pcie_Rc_Ctrl_Memory Error. ret=0x%08x\n", ret));
			}
		}
		else{
			Ddim_Print(("Memory access is not available.\n"));
		}
	}
	@endcode

	<br>
	<br>
	@section PCIe_RC_Sample_Demand_DMA_Transfer	Sample of Demand mode DMA transfer.
	@code
	static VOID pcie_rc_int_dma_cb(INT32 status)
	{
		// status
		// 0x00000001  Done Interrupt status(DMA ch0)
		// 0x00000002  Done Interrupt status(DMA ch1)
		// 0x00010000  Abort Interrupt status(DMA ch0)
		// 0x00020000  Abort Interrupt status(DMA ch1)
	}

	VOID PCIe_RC_Sample_Demand_DMA_Transfer(VOID)
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
		pcie_ctrl_dma.int_dma_callback	= (VP_CALLBACK_PCIE)pcie_rc_int_dma_cb;
		ret = Dd_Pcie_Rc_Ctrl_Dma(&pcie_ctrl_dma);
		if(ret == D_DDIM_OK){
			Dd_Pcie_Rc_Start_Dma(pcie_ctrl_dma.ch, pcie_ctrl_dma.direction);
		}
		else{
			Ddim_Print(("Dd_Pcie_Rc_Ctrl_Dma Error. ret=0x%08x\n", ret));
		}
	}
	@endcode

	<br>
	<br>
	@section PCIe_RC_Sample_LL_DMA_Transfer	Sample of Linked List mode DMA Transfer.
	@code

	#define PCIE_DMA_LL_ADDR	(0x55000000)

	static VOID pcie_rc_ll_dma(VOID)
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

	VOID PCIe_RC_Sample_LL_DMA_Transfer(VOID)
	{
		INT32 ret;
		T_DD_PCIE_CTRL_DMA pcie_ctrl_dma;

		pcie_rc_ll_dma();
		memset(&pcie_ctrl_dma, 0, sizeof(pcie_ctrl_dma));
		pcie_ctrl_dma.mode				= E_DD_PCIE_DMA_LL_MODE;
		pcie_ctrl_dma.ch				= E_DD_PCIE_CH0;
		pcie_ctrl_dma.dma_ch			= E_DD_PCIE_DMA_CH0;
		pcie_ctrl_dma.ll_addr			= PCIE_DMA_LL_ADDR;
		pcie_ctrl_dma.direction			= E_DD_PCIE_DMA_DIR_STOD;
		pcie_ctrl_dma.int_dma_callback	= (VP_CALLBACK_PCIE)pcie_rc_int_dma_cb;
		ret = Dd_Pcie_Rc_Ctrl_Dma(&pcie_ctrl_dma);
		if(ret == D_DDIM_OK){
			Dd_Pcie_Rc_Start_Dma(pcie_ctrl_dma.ch, pcie_ctrl_dma.direction);
		}
		else{
			Ddim_Print(("Dd_Pcie_Rc_Ctrl_Dma Error. ret=0x%08x\n", ret));
		}
	}
	@endcode

	<br>
	<br>
	@section PCIe_RC_Sample_ASPM	Sample of ASPM.
	@code
	VOID PCIe_RC_Sample_ASPM(VOID)
	{
		INT32 ret;
		E_DD_PCIE_ST_LTSSM st_ltssm;

		if(Dd_Pcie_Rc_Get_Link_Status(E_DD_PCIE_CH0) == TRUE){
			ret = Dd_Pcie_Rc_Ctrl_Aspm(E_DD_PCIE_CH0, E_DD_PCIE_ASPM_CONTROL_L0S_L1);
			if(ret == D_DDIM_OK){
				Ddim_Print(("Dd_Pcie_Rc_Ctrl_Aspm OK.\n"));
			}
			else{
				Ddim_Print(("Dd_Pcie_Rc_Ctrl_Aspm Error. ret=0x%08x\n", ret));
			}
		}
		else{
			Ddim_Print(("ASPM is not available.\n"));
		}
	}
	@endcode

	<br>
	<br>
	@section PCIe_RC_Sample_Interrupt	Sample of Interrupt.
	@code
	// Interrupt of INT_OWN. ---------------------------------------------------
	static VOID pcie_rc_int_own_pme_cb(INT32 status)
	{
		// status = 0 (none)
	}

	static VOID pcie_rc_int_own_hp_cb(INT32 status)
	{
		// status
		//	E_DD_PCIE_HOT_PLUG_ST_ABP		Attention Button Pressed
		//	E_DD_PCIE_HOT_PLUG_ST_PFD		Power Fault Detected
		//	E_DD_PCIE_HOT_PLUG_ST_MRLSC		MRL Sensor Changed
		//	E_DD_PCIE_HOT_PLUG_ST_CC		Command Completed
		//	E_DD_PCIE_HOT_PLUG_ST_DLLSC		Data Link Layer State Change
	}

	static VOID pcie_rc_int_own_link_up_cb(INT32 status)
	{
		// status = 0 (none)
	}

	static VOID pcie_rc_int_own_wake_cb(INT32 status)
	{
		// status = 0 (none)
	}

	VOID PCIe_RC_Sample_INT_OWN(VOID)
	{
		T_DD_PCIE_RC_INT_OWN_CB int_own_cb;

		int_own_cb.cfg_pme_cb		= (VP_CALLBACK_PCIE)pcie_rc_int_own_pme_cb;
		int_own_cb.hp_cb			= (VP_CALLBACK_PCIE)pcie_rc_int_own_hp_cb;
		int_own_cb.rdlh_link_up_cb	= (VP_CALLBACK_PCIE)pcie_rc_int_own_link_up_cb;
		int_own_cb.wake_cb			= (VP_CALLBACK_PCIE)pcie_rc_int_own_wake_cb;
		Dd_Pcie_Rc_Set_Int_Own_Func(E_DD_PCIE_CH0, &int_own_cb);
	}

	// Interrupt of INT_EP. ----------------------------------------------------
	static VOID pcie_rc_int_ep_intx_cb(INT32 status)
	{
		// status = 0
		//	1  INTA
		//	2  INTB
		//	4  INTC
		//	8  INTD
	}

	static VOID pcie_rc_int_ep_pm_pme_cb(INT32 status)
	{
		// status = 0 (none)
	}

	static VOID pcie_rc_int_ep_correctable_err_cb(INT32 status)
	{
		// status = 0 (none)
	}

	static VOID pcie_rc_int_ep_nonfatal_err_cb(INT32 status)
	{
		// status = 0 (none)
	}

	static VOID pcie_rc_int_ep_fatal_err_cb(INT32 status)
	{
		// status = 0 (none)
	}

	static VOID pcie_rc_int_ep_msg_ltr_cb(INT32 status)
	{
		// status = 0 (none)
	}

	static VOID pcie_rc_int_ep_pm_to_ack_cb(INT32 status)
	{
		// status = 0 (none)
	}

	static VOID pcie_rc_int_ep_vendor_msg_cb(INT32 status)
	{
		// status = 0 (none)
	}

	VOID PCIe_RC_Sample_INT_EP(VOID)
	{
		T_DD_PCIE_RC_INT_EP_CB int_own_cb;

		int_ep_cb.radm_intx_cb				= (VP_CALLBACK_PCIE)pcie_rc_int_ep_intx_cb;
		int_ep_cb.radm_pm_pme_cb			= (VP_CALLBACK_PCIE)pcie_rc_int_ep_pm_pme_cb;
		int_ep_cb.radm_correctable_err_cb	= (VP_CALLBACK_PCIE)pcie_rc_int_ep_correctable_err_cb;
		int_ep_cb.radm_nonfatal_err_cb		= (VP_CALLBACK_PCIE)pcie_rc_int_ep_nonfatal_err_cb;
		int_ep_cb.radm_fatal_err_cb			= (VP_CALLBACK_PCIE)pcie_rc_int_ep_fatal_err_cb;
		int_ep_cb.radm_msg_ltr_cb			= (VP_CALLBACK_PCIE)pcie_rc_int_ep_msg_ltr_cb;
		int_ep_cb.radm_pm_to_ack_cb			= (VP_CALLBACK_PCIE)pcie_rc_int_ep_pm_to_ack_cb;
		int_ep_cb.radm_vendor_msg_cb		= (VP_CALLBACK_PCIE)pcie_rc_int_ep_vendor_msg_cb;
		Dd_Pcie_Rc_Set_Int_Ep_Func(E_DD_PCIE_CH0, &int_ep_cb);
	}

	// Interrupt of INT_SYS_ERR. -----------------------------------------------
	static VOID pcie_rc_int_aer_rc_err_cb(INT32 status)
	{
		// status = 0 (none)
		//	0x01	Correctable Error Received
		//	0x02	Multiple Correctable Errors Received
		//	0x04	Fatal or Non-Fatal Error Received
		//	0x08	Multiple Fatal or Non-Fatal Errors Received
		//	0x10	First Uncorrectable Error is Fatal
		//	0x20	One or more Non-Fatal Error Messages Received
		//	0x40	One or more Fatal Error Messages Received
	}

	static VOID pcie_rc_int_sys_err_rc_cb(INT32 status)
	{
		// status = 0 (none)
	}

	VOID PCIe_RC_Sample_INT_SYS_ERR(VOID)
	{
		T_DD_PCIE_RC_INT_SYS_ERR_CB sys_err_cb;

		sys_err_cb.cfg_aer_rc_err_cb	= (VP_CALLBACK_PCIE)pcie_rc_int_aer_rc_err_cb;
		sys_err_cb.cfg_sys_err_rc_cb	= (VP_CALLBACK_PCIE)pcie_rc_int_sys_err_rc_cb;
		Dd_Pcie_Rc_Set_Int_Sys_Err_Func(E_DD_PCIE_CH0, &sys_err_cb);
	}

	// Interrupt of INT_TRS_ICS_MSI. -------------------------------------------
	static VOID pcie_rc_int_cfg_link_auto_bw_int_cb(INT32 status)
	{
		// status = 0 (none)
	}

	static VOID pcie_rc_int_cfg_bw_mgt_int_cb(INT32 status)
	{
		// status = 0 (none)
	}

	static VOID pcie_rc_int_smlh_req_rst_not_cb(INT32 status)
	{
		// status = 0 (none)
	}

	VOID PCIe_RC_Sample_INT_TRS_ICS_MSI(VOID)
	{
		T_DD_PCIE_RC_INT_TRS_ICS_MSI_CB int_trs_ics_msi_cb;

		int_trs_ics_msi_cb.cfg_link_auto_bw_int_cb	= (VP_CALLBACK_PCIE)pcie_rc_int_cfg_link_auto_bw_int_cb;
		int_trs_ics_msi_cb.cfg_bw_mgt_int_cb		= (VP_CALLBACK_PCIE)pcie_rc_int_cfg_bw_mgt_int_cb;
		int_trs_ics_msi_cb.smlh_req_rst_not_cb		= (VP_CALLBACK_PCIE)pcie_rc_int_smlh_req_rst_not_cb;
		Dd_Pcie_Rc_Set_Int_Sys_Err_Func(E_DD_PCIE_CH0, &int_trs_ics_msi_cb);
	}

	@endcode

	<br>
	<br>
	@section PCIe_RC_Sample_End		Sample of PCIe End.
	@code
	VOID PCIe_RC_Sample_End(VOID)
	{
		Dd_Pcie_Rc_Close();
		Dd_Pcie_Rc_Deinit();
	}
	@endcode

	<br>
	<br>
	@section PCIe_RC_Sample_L2_Exit		Sample of L2 Exit.
	@code
	VOID PCIe_RC_Sample_End(VOID)
	{
		Dd_Pcie_Rc_Pm_Pw_L2_Exit(E_DD_PCIE_CH0);
		Dd_Pcie_Rc_Close();
		Dd_Pcie_Rc_Deinit();
	}
	@endcode

	<br>
	<br>
	@section PCIe_RC_Sample_Detect_Card		Sample of Detect Card.
	@code
	static VOID pcie_rc_detect_card_cb(INT32 status)
	{
		// status
		// 0  Insert
		// 1  Eject
	}

	VOID PCIe_RC_Sample_Detect_Card(VOID)
	{
		Dd_Pcie_Rc_Ctrl_Hp_Detect_Card(E_DD_PCIE_CH0, (VP_CALLBACK_PCIE)pcie_rc_detect_card_cb);
	}
	@endcode

@}*/
