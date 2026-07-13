/**
 * @file		fj_pcie_ep.h
 * @brief		PCIe Endpoint Controller
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_PCIE_EP_H_
#define _FJ_PCIE_EP_H_

#include "ddim_typedef.h"
#include "fj_std.h"

///////////////////////////////////////////////////////////////////////////////
//  overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_pcie_ep_overview
@{

The PCI Express Endpoint controls can be achieved by using API.<br>
<br>
<span style="font-weight: bold;">Features</span><br>
<br>
- Bifurcation Disable Mode: LINK layer 1, LANE witdh x2<br>
	- It corresponds to Gen1(2.5GT/s) and Gen2(5.0GT/s).<br>
	- The number of LANE is x2. <br>
<br>
- ASPM(Active State Power Management)<br>
<br>
- Demand DMA Mode and Linked List DMA Mode<br>
<br>
- MSI<br>

@}*//* --- end of fj_pcie_ep_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_pcie_ep_sequence
@{
	- @ref PCIe_EP_Sequence

	@section PCIe_EP_Sequence PCIe Endpoint Sequence
	@image html fj_pcie_ep_sequence.png
	<br>
	<br>
@}*//* --- end of fj_pcie_ep_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_pcie_ep_definition
@{*/
#define FJ_PCIE_EP_CONTROLLER_START1		(0x00000001UL)
#define FJ_PCIE_EP_CONTROLLER_LU_INT1		(0x00000002UL)
#define FJ_PCIE_EP_CONTROLLER_END			(0x80000000UL)	

///////////////////////////////////////////////////////////////////////////////
// typedef
///////////////////////////////////////////////////////////////////////////////

/*@} --- end of fj_pcie_ep_definition */

///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_pcie_ep_api
@{*/
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
It initializes the PCIe.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
@remarks
	 - This API Sample code is as follows.<br>
@code
[Sample Code]
#define PCIE0_MEM_BAR_ADR	0x19400000	// Memory Base Address (PIO area)
#define PCIE0_MEM_LMT_ADR	0x1940FFFF	// Limit Address (64KBytes)
#define PCIE0_MEM_TGT_ADR	0x58000000	// Memory Target Address

static VOID pcie_ep_lu_cb( UINT32 status )
{
	printf("Link Up. status=0x%08X\n", status);
}

static FJ_ERR_CODE pcie_ep_mem_ctrl( VOID )
{
	FJ_ERR_CODE				ret = FJ_ERR_OK;
	T_FJ_PCIE_MEMORY_CTRL	memory_ctrl;

	memory_ctrl.ch				= E_FJ_PCIE_CH0;
	memory_ctrl.base_adr		= PCIE0_MEM_BAR_ADR;
	memory_ctrl.limit_adr		= PCIE0_MEM_LMT_ADR;
	memory_ctrl.target_adr_low	= PCIE0_MEM_TGT_ADR;
	memory_ctrl.target_adr_upp	= 0;
	ret = FJ_Pcie_Ep_Memory_Ctrl(&memory_ctrl);
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Ep_Memory_Ctrl Error. ret=0x%04X\n", ret);
	}
	return ret;
}

// EP initial start.
static VOID pcie_ep_sample1( VOID )
{
	FJ_ERR_CODE	ret;

	// Set link up callback.
	ret = FJ_Pcie_Ep_Set_Lu_Cb(pcie_ep_lu_cb);
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Ep_Set_Lu_Cb Error. ret=0x%04X\n", ret);
		return;
	}

	// Init
	ret = FJ_Pcie_Ep_Init();
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Ep_Init Error. ret=0x%04X\n", ret);
		return;
	}

	// Open
	ret = FJ_Pcie_Ep_Open();
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Ep_Open Error. ret=0x%04X\n", ret);
		return;
	}
}

// After the link up, Memory write and the ASPM setting are executed.
static VOID pcie_ep_sample2( VOID )
{
	FJ_ERR_CODE	ret;

	// DBI read only register and DBI2 register are initialized.
	ret = FJ_Pcie_Ep_Init_Reg();
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Ep_Init_Reg Error. ret=0x%04X\n", ret);
		return;
	}

	// Memory Control
	ret = pcie_ep_mem_ctrl();
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Ep_Memory_Ctrl Error. ret=0x%04X\n", ret);
		return;
	}

	memcpy((void*)PCIE0_MEM_BAR_ADR, "PCIe EP Start --", 16);
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, PCIE0_MEM_BAR_ADR, 16);

	// ASPM ON
	ret = FJ_Pcie_Ep_Aspm_On();
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Ep_Aspm_On() Error. ret=0x%04X\n", ret);
		return;
	}
}
@endcode
*/
extern FJ_ERR_CODE FJ_Pcie_Ep_Init( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
PCIe end.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
@remarks
	 - This API Sample code is as follows.<br>
@code
[Sample Code]
static VOID pcie_ep_end( VOID )
{
	FJ_Pcie_Ep_Close();
	FJ_Pcie_Ep_Deinit();
}
@endcode
*/
extern FJ_ERR_CODE FJ_Pcie_Ep_Deinit( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
It opens the PCIe.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	PCIe open error. (PCIe is used.)
*/
extern FJ_ERR_CODE FJ_Pcie_Ep_Open( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
It closes the PCIe.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	PCIe close error. (PCIe is not used.)
*/
extern FJ_ERR_CODE FJ_Pcie_Ep_Close( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
DMA transfer.
@param [in]	dma_ctrl	DMA control data. See @ref T_FJ_PCIE_DMA_CTRL
@retval FJ_ERR_OK		Success.
@retval FJ_ERR_NG		Error occurred.
@remarks
	- This API Sample code is as follows.<br>
@code
[Sample Code]
static VOID pcie_ep_int_dma_cb( INT32 status )
{
	// status
	// 0x00000001  Done Interrupt status
	// 0x00010000  Abort Interrupt status
}

static VOID pcie_ep_dma_transfer( VOID )
{
	T_FJ_PCIE_DMA_CTRL dma_ctrl;

	// Write
	dma_ctrl.src_addr_low		= 0x58000000;	// RC Memory Address
	dma_ctrl.src_addr_upp		= 0;
	dma_ctrl.dst_addr_low		= 0x5F000000;	// EP Memory Address
	dma_ctrl.dst_addr_upp		= 0
	dma_ctrl.trans_size			= 0x3FC000;
	dma_ctrl.direction			= E_FJ_PCIE_DMA_DIR_STOD;	// RC->EP
	dma_ctrl.int_dma_callback	= (PCIE_CALLBACK)pcie_ep_int_dma_cb;
	FJ_Pcie_Ep_Demand_Dma_Transfer(&dma_ctrl);

	// Read
	dma_ctrl.src_addr_low		= 0x5F000000;	// EP Memory Address
	dma_ctrl.src_addr_upp		= 0;
	dma_ctrl.dst_addr_low		= 0x59000000;	// RC Memory Address
	dma_ctrl.dst_addr_upp		= 0
	dma_ctrl.trans_size			= 0x3FC000;
	dma_ctrl.direction			= E_FJ_PCIE_DMA_DIR_DTOS;	// EP->RC
	dma_ctrl.int_dma_callback	= (PCIE_CALLBACK)pcie_ep_int_dma_cb;
	FJ_Pcie_Ep_Demand_Dma_Transfer(&dma_ctrl);

}
@endcode
*/
extern FJ_ERR_CODE FJ_Pcie_Ep_Dma_Transfer( T_FJ_PCIE_DMA_CTRL const* const dma_ctrl );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
DMA transfer stop.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE FJ_Pcie_Ep_Dma_Stop( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Memory transfer settings.
@param [in]	memory_ctrl	Memory control data. See @ref T_FJ_PCIE_MEMORY_CTRL
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE FJ_Pcie_Ep_Memory_Ctrl( T_FJ_PCIE_MEMORY_CTRL const* const memory_ctrl );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
It will be controll the INTx send request.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE	FJ_Pcie_Ep_Req_Int_EP( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
It will be controll the MSI transfer request.
@param [in]	msi_num	MSI number. See @ref E_FJ_PCIE_MSI_NUM
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE FJ_Pcie_Ep_Transfer_Msi( E_FJ_PCIE_MSI_NUM msi_num );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
It wants to turn ON the ASPM. (L0s and L1)
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE FJ_Pcie_Ep_Aspm_On( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
It wants to turn OFF the ASPM.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE FJ_Pcie_Ep_Aspm_Off( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
It wants to change the transfer speed.
@param [in]	link_speed	Link speed. See @ref E_FJ_PCIE_LINK_SPEED
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE FJ_Pcie_Ep_Transfer_Speed_Chg( E_FJ_PCIE_LINK_SPEED link_speed );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get the STate of Data communication Link Up.
@retval E_FJ_PCIE_ST_DL_UP	STate of Data communication Link UP. See @ref E_FJ_PCIE_ST_DL_UP
*/
extern E_FJ_PCIE_ST_DL_UP FJ_Pcie_Ep_Get_St_Dl_Up( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get the STate of Link Training and Status State Machine.
@retval E_FJ_PCIE_ST_LTSSM	STate of Link Training and Status State Machine. See @ref E_FJ_PCIE_ST_LTSSM
*/
extern E_FJ_PCIE_ST_LTSSM FJ_Pcie_Ep_Get_St_Ltssm( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get the Power Management Status State.
@retval E_FJ_PCIE_PMST	Power state. See @ref E_FJ_PCIE_PMST
*/
extern E_FJ_PCIE_PMST FJ_Pcie_Ep_Get_Pmst( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get current link speed.
@param [out]	link_speed	Link speed. See @ref E_FJ_PCIE_LINK_SPEED
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE FJ_Pcie_Ep_Get_Current_Link_Speed( E_FJ_PCIE_LINK_SPEED* link_speed );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get the STate of ASPM.
@param [out]	pcie_aspm	PCIE ASPM status.
@retval FJ_ERR_OK			Success.
*/
extern FJ_ERR_CODE FJ_Pcie_Ep_Get_St_Aspm( BOOL* pcie_aspm );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set max payload size.
@param [in]	mps		Max payload size. See @ref E_FJ_PCIE_MPS
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE FJ_Pcie_Ep_Set_Mps( E_FJ_PCIE_MPS mps );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get max payload size.
@param [out]	mps		Max payload size. See @ref E_FJ_PCIE_MPS
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE FJ_Pcie_Ep_Get_Mps( E_FJ_PCIE_MPS* mps );

/**
DBI read only register and DBI2 register are initialized.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
@remarks
	 - Please execute it after link up.<br>
*/
extern FJ_ERR_CODE FJ_Pcie_Ep_Init_Reg( VOID );

/**
Link up callback function setting.
@param [in]	lu_cb	Link Up CallBack function pointer.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
@remarks
*/
extern FJ_ERR_CODE FJ_Pcie_Ep_Set_Lu_Cb( PCIE_CALLBACK lu_cb );

extern FJ_ERR_CODE FJ_Pcie_Ep_Start_Task( VOID );
extern FJ_ERR_CODE FJ_Pcie_Ep_Set_Controller_Flag( UINT32 flag );
extern FJ_ERR_CODE FJ_Pcie_Ep_Set_Init_Address( UINT32 addr );

#ifdef CO_HOST_PCIE_EP
/**
Register setting for YUV forwarding.
@param [in]	addr	Address.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
@remarks
*/
extern FJ_ERR_CODE FJ_Pcie_Ep_Yuv_Trans_Set_Reg( UINT32 addr );

/**
Register setting for YUV forwarding.
@param [in]		offset	register offset.
@param [out]	data	register data.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
@remarks
*/
extern FJ_ERR_CODE FJ_Pcie_Ep_Get_Reg( UINT32 offset, UINT32* data );
#endif // CO_HOST_PCIE_EP


/*@} --- end of fj_pcie_ep_api */

///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_pcie_ep_message
@{*/
/*@} --- end of fj_pcie_ep_message */

#endif	// _FJ_PCIE_EP_H_
