/**
 * @file		fj_pcie_rc.h
 * @brief		PCIe RootComplex Controller
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_PCIE_RC_H_
#define _FJ_PCIE_RC_H_

#include "ddim_typedef.h"
#include "fj_std.h"
#include "fj_pcie.h"

///////////////////////////////////////////////////////////////////////////////
//  overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_pcie_rc_overview
@{

The Root Port of PCI Express Root Complex controls can be achieved by using API.<br>
<br>
<span style="font-weight: bold;">Features</span><br>
<br>
- Bifurcation Disable Mode: LINK layer 1, LANE witdh x2<br>
	- It corresponds to Gen1(2.5GT/s) and Gen2(5.0GT/s).<br>
	- The number of LANE is x2. <br>
<br>
- Bifurcation Enable Mode: LINK layer 2, LANE witdh x1<br>
	- It corresponds to Gen1(2.5GT/s) and Gen2(5.0GT/s).<br>
	- The number of LANE is x1.<br>
<br>
- ASPM(Active State Power Management)<br>
<br>
- Demand DMA Mode and Linked List DMA Mode<br>
<br>
- MSI<br>

@}*//* --- end of fj_pcie_rc_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_pcie_rc_sequence
@{
	- @ref PCIe_RC_Sequence

	@section PCIe_RC_Sequence PCIe Root Complex Sequence
	@image html fj_pcie_rc_sequence.png
	<br>
	<br>

@}*//* --- end of fj_pcie_rc_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_pcie_rc_definition
@{*/
#define FJ_PCIE_RC_CONTROLLER_START1		(0x00000001UL)
#define FJ_PCIE_RC_CONTROLLER_START2		(0x00000002UL)
#define FJ_PCIE_RC_CONTROLLER_LU_INT1		(0x00000004UL)
#define FJ_PCIE_RC_CONTROLLER_LU_INT2_CH0	(0x00000010UL)
#define FJ_PCIE_RC_CONTROLLER_LU_INT2_CH1	(0x00000020UL)
#define FJ_PCIE_RC_CONTROLLER_END			(0x80000000UL)

///////////////////////////////////////////////////////////////////////////////
// typedef
///////////////////////////////////////////////////////////////////////////////
/** PCIe config control data */
typedef struct{
	E_FJ_PCIE_CH		ch;					/**< PCIe channel			*/
	UINT32			base_adr;				/**< AXI slave memory base address.<br>
													Please set the 16-31 bit.<br>
													0-15 bit not used.<br>
													The start address must be aligned to a 64 kB boundary.<br>
													PCIe ch0 PIO area : 0x19400000 .. 0x197FFFFF<br>
													PCIe ch1 PIO area : 0x19800000 .. 0x198FFFFF
											*/
	UINT32			limit_adr;				/**< AXI slave memory limit address.<br>
													Please set the 16-31 bit.<br>
													0-15 bit not used.<br>
													The end address must be aligned to a 64 kB boundary.<br>
													PCIe ch0 PIO area : 0x19400000 .. 0x197FFFFF<br>
													PCIe ch1 PIO area : 0x19800000 .. 0x198FFFFF
											*/
} T_FJ_PCIE_RC_CTRL_CFG;

/** MSI control data */
typedef struct{
	E_FJ_PCIE_CH	ch;						/**< PCIe channel							*/
	UINT32			int_en;					/**< Interrupt enable						*/
	UINT32			cmp_adr;				/**< compare address						*/
	PCIE_CALLBACK	int_msi_callback;		/**< Callback function of the MSI interrupt	*/
} T_FJ_PCIE_MSI_CTRL;

/*@}*//* --- end of fj_pcie_rc_definition */

///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_pcie_rc_api
@{*/
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
It initializes the PCIe.
@param [in]	bifurcation		bifurcation mode. See @ref E_FJ_PCIE_BIFURCATION
@retval FJ_ERR_OK			Success.
@retval FJ_ERR_NG			Error occurred.
@remarks
	 - This API Sample code is as follows.<br>
@code
[Sample Code]
#define PCIE0_MEM_BAR_ADR	0x19400000	// Memory Base Address ch0 (PIO area)
#define PCIE0_MEM_LMT_ADR	0x1940FFFF	// Limit Address (64KBytes)
#define PCIE0_MEM_TGT_ADR	0x58000000	// Memory Target Address (EP PIO area)

#define PCIE0_CFG_BAR_ADR	0x19600000	// Config Base Address ch0 (PIO area)
#define PCIE0_CFG_LMT_ADR	0x1960FFFF	// Limit Address (64KBytes)

#define PCIE1_MEM_BAR_ADR	0x19800000	// Memory Base Address ch1 (PIO area)
#define PCIE1_MEM_LMT_ADR	0x1980FFFF	// Limit Address (64KBytes)
#define PCIE1_MEM_TGT_ADR	0x58000000	// Memory Target Address (EP PIO area)

#define PCIE1_CFG_BAR_ADR	0x19A00000	// Config Base Address ch1 (PIO area)
#define PCIE1_CFG_LMT_ADR	0x19A0FFFF	// Limit Address (64KBytes)

#define PCIE0_MSI_CMP_ADR	0x18000000	// MSI compare address ch0 (INTMEM)
#define PCIE1_MSI_CMP_ADR	0x18004000	// MSI compare address ch1 (INTMEM)

static VOID pcie_rc_lu_cb( UINT32 status )
{
	printf("Link Up. status=0x%08X\n", status);
}

static VOID pcie_rc_int_msi_cb( INT32 status )
{
	// status
	// bit0 MSI Interrupt. (value:1)
}

static FJ_ERR_CODE pcie_rc_config_ctrl( E_FJ_PCIE_CH ch )
{
	FJ_ERR_CODE				ret = FJ_ERR_OK;
	T_FJ_PCIE_RC_CTRL_CFG	pcie_ctrl_cfg;

	if(ch == E_FJ_PCIE_CH0){
		pcie_ctrl_cfg.ch				= E_FJ_PCIE_CH0;
		pcie_ctrl_cfg.base_adr			= PCIE0_CFG_BAR_ADR;
		pcie_ctrl_cfg.limit_adr			= PCIE0_CFG_LMT_ADR;
	}
	else{
		pcie_ctrl_cfg.ch				= E_FJ_PCIE_CH1;
		pcie_ctrl_cfg.base_adr			= PCIE1_CFG_BAR_ADR;
		pcie_ctrl_cfg.limit_adr			= PCIE1_CFG_LMT_ADR;
	}
	ret = FJ_Pcie_Rc_Config_Ctrl(&pcie_ctrl_cfg);
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Rc_Config_Ctrl(%d) Error. ret=0x%04X\n", ch, ret);
	}
	return ret;
}

static FJ_ERR_CODE pcie_rc_mem_ctrl( E_FJ_PCIE_CH ch )
{
	FJ_ERR_CODE				ret = FJ_ERR_OK;
	T_FJ_PCIE_MEMORY_CTRL	memory_ctrl;

	if(ch == E_FJ_PCIE_CH0){
		memory_ctrl.ch				= E_FJ_PCIE_CH0;
		memory_ctrl.base_adr		= PCIE0_MEM_BAR_ADR;
		memory_ctrl.limit_adr		= PCIE0_MEM_LMT_ADR;
		memory_ctrl.target_adr_low	= PCIE0_MEM_TGT_ADR;
		memory_ctrl.target_adr_upp	= 0;
	}
	else{
		memory_ctrl.ch				= E_FJ_PCIE_CH1;
		memory_ctrl.base_adr		= PCIE1_MEM_BAR_ADR;
		memory_ctrl.limit_adr		= PCIE1_MEM_LMT_ADR;
		memory_ctrl.target_adr_low	= PCIE1_MEM_TGT_ADR;
		memory_ctrl.target_adr_upp	= 0;
	}
	ret = FJ_Pcie_Rc_Memory_Ctrl(&memory_ctrl);
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Rc_Memory_Ctrl(%d) Error. ret=0x%04X\n", ch, ret);
	}
	return ret;
}

static FJ_ERR_CODE pcie_rc_msi_ctrl( E_FJ_PCIE_CH ch )
{
	FJ_ERR_CODE			ret;
	UINT32				cmp_addr;
	T_FJ_PCIE_MSI_CTRL	msi_ctrl;

	msi_ctrl.ch		= ch;
	msi_ctrl.int_en	= 0xFFFFFFFF;	// 32 message enable
	if(ch == E_FJ_PCIE_CH0){
		cmp_addr = PCIE0_MSI_CMP_ADR;
	}
	else{
		cmp_addr = PCIE1_MSI_CMP_ADR;
	}
	msi_ctrl.cmp_adr			= cmp_addr;
	msi_ctrl.int_msi_callback	= (PCIE_CALLBACK)pcie_rc_int_msi_cb;
	ret = FJ_Pcie_Rc_Msi_Ctrl(&msi_ctrl);
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Rc_Msi_Ctrl(%d) Error. ret=0x%04X\n", ch, ret);
		return ret;
	}

	return FJ_ERR_OK;
}

// RC initial start.
static VOID pcie_rc_sample1( VOID )
{
	FJ_ERR_CODE	ret;

	// Set link up callback.
	ret = FJ_Pcie_Rc_Set_Lu_Cb(E_FJ_PCIE_CH0, pcie_rc_lu_cb);
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Rc_Set_Lu_Cb Error. ret=0x%04X\n", ret);
		return;
	}

	// Init
	ret = FJ_Pcie_Rc_Init(E_FJ_PCIE_BIFURCATION_DISABLE);
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Rc_Init() Error. ret=0x%04X\n", ret);
		return;
	}

	// Open
	ret = FJ_Pcie_Rc_Open();
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Rc_Open() Error. ret=0x%04X\n", ret);
		return;
	}
}

// The register of EP is set.
static FJ_ERR_CODE pcie_rc_milb_ep_config( E_FJ_PCIE_CH ch )
{
	UINT32	data;
	UINT32	msi_addr;

	// bit1 Allow access response of Memory space. bit2 Bus Master Enable
	FJ_Pcie_Rc_Config_Read(ch, 0x0004, &data);
	data |= 0x00000006;
	FJ_Pcie_Rc_Config_Write(ch, 0x0004, data);

	// bit7:5 512Byte max Payload size, bit14:12 256Byte max Read Request size
	FJ_Pcie_Rc_Config_Read(ch, 0x0078, &data);
	data &= 0xFFFF8F1F;
	data |= 0x00001040;
	FJ_Pcie_Rc_Config_Write(ch, 0x0078, data);

	if(ch == E_FJ_PCIE_CH0){
		msi_addr = PCIE0_MSI_CMP_ADR;
	}
	else{
		msi_addr = PCIE1_MSI_CMP_ADR;
	}
	FJ_Pcie_Rc_Config_Write(ch, 0x0054, msi_addr);

	// ASPM ON
	FJ_Pcie_Rc_Config_Read(ch, 0x0080, &data);
	data |= 0x00000003;	// bit1:0 L0s and L1 Entry Enabled
	FJ_Pcie_Rc_Config_Write(ch, 0x0080, data);

	// iATU inbound set
	FJ_Pcie_Rc_Config_Write(ch, 0x0900, 0x80000000);
	FJ_Pcie_Rc_Config_Write(ch, 0x0904, 0x00000000);
	if(ch == E_FJ_PCIE_CH0){
		FJ_Pcie_Rc_Config_Write(ch, 0x0918, PCIE0_MEM_TGT_ADR);
		FJ_Pcie_Rc_Config_Write(ch, 0x0010, PCIE0_MEM_TGT_ADR);
	}
	else{
		FJ_Pcie_Rc_Config_Write(ch, 0x0918, PCIE1_MEM_TGT_ADR);
		FJ_Pcie_Rc_Config_Write(ch, 0x0010, PCIE1_MEM_TGT_ADR);
	}
	FJ_Pcie_Rc_Config_Write(ch, 0x091C, 0x00000000);
	FJ_Pcie_Rc_Config_Write(ch, 0x0908, 0xC0000000);

	return FJ_ERR_OK;
}

// After the link up, Configration and Link Spped Change, etc are executed.
static VOID pcie_rc_sample2( VOID )
{
	FJ_ERR_CODE	ret;

	// Config Control
	ret = pcie_rc_config_ctrl(E_FJ_PCIE_CH0);
	if(ret != FJ_ERR_OK){
		printf("pcie_rc_config_ctrl() Error. ret=0x%04X\n", ret);
		return;
	}

	pcie_rc_milb_ep_config(E_FJ_PCIE_CH0);

	// Link Spped Change (2.5GT/s -> 5.0GT/s)
	ret = FJ_Pcie_Rc_Transfer_Speed_Chg(E_FJ_PCIE_CH0, E_FJ_PCIE_LINK_SPEED_GEN2);
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Rc_Transfer_Speed_Chg() Error. ret=0x%04X\n", ret);
		return;
	}

	// MSI Control
	ret = pcie_rc_msi_ctrl(E_FJ_PCIE_CH0);
	if(ret != FJ_ERR_OK){
		printf("pcie_rc_msi_ctrl() Error. ret=0x%04X\n", ret);
		return;
	}

	// Memory Control
	ret = pcie_rc_mem_ctrl(E_FJ_PCIE_CH0);
	if(ret != FJ_ERR_OK){
		printf("pcie_rc_mem_ctrl() Error. ret=0x%04X\n", ret);
		return;
	}

	memcpy((void*)PCIE0_MEM_BAR_ADR, "PCIe RC Start --", 16);
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, PCIE0_MEM_BAR_ADR, 16);

	// ASPM ON
	ret = FJ_Pcie_Rc_Aspm_On(E_FJ_PCIE_CH0);
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Rc_Aspm_On() Error. ret=0x%04X\n", ret);
		return;
	}
}
@endcode
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Init( E_FJ_PCIE_BIFURCATION bifurcation );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
PCIe end.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
@remarks
	 - This API Sample code is as follows.<br>
@code
[Sample Code]
static VOID pcie_rc_end( VOID )
{
	FJ_Pcie_Rc_Close();
	FJ_Pcie_Rc_Deinit();
}
@endcode
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Deinit( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
It opens the PCIe.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	PCIe open error. (PCIe is used.)
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Open( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
It closes the PCIe.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	PCIe close error. (PCIe is not used.)
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Close( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Configuration transfer set.
@param [in]	pcie_ctrl_cfg	config control data. See @ref T_FJ_PCIE_RC_CTRL_CFG
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred. (Not link up.)
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Config_Ctrl( T_FJ_PCIE_RC_CTRL_CFG const* const pcie_ctrl_cfg );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
It will read the config.
@param [in]	ch		Channel number. See @ref E_FJ_PCIE_CH
@param [in]	offset	Configuration register address offset.
@param [in]	data	Read data.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
@remarks
		- FJ_Pcie_Rc_Config_Ctrl must be executed. 
		- Offset by word specified.
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Config_Read( E_FJ_PCIE_CH ch, USHORT offset, UINT32* data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
It will write the config.
@param [in]	ch		Channel number. See @ref E_FJ_PCIE_CH
@param [in]	offset	Configuration register address offset.
@param [in]	data	Write data.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
@remarks
		- FJ_Pcie_Rc_Config_Ctrl must be executed. 
		- Offset by word specified.
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Config_Write( E_FJ_PCIE_CH ch, USHORT offset, UINT32 data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
DMA transfer.
@param [in]	dma_ctrl	Demand DMA control data. See @ref T_FJ_PCIE_DMA_CTRL
@retval FJ_ERR_OK		Success.
@retval FJ_ERR_NG		Error occurred.
@remarks
	- This API Sample code is as follows.<br>
@code
[Sample Code]
static VOID pcie_rc_int_dma_cb( INT32 status )
{
	// status
	// 0x00000001  Done Interrupt status(DMA ch0)
	// 0x00000002  Done Interrupt status(DMA ch1)
	// 0x00010000  Abort Interrupt status(DMA ch0)
	// 0x00020000  Abort Interrupt status(DMA ch1)
}

static VOID pcie_rc_dma_transfer( VOID )
{
	T_FJ_PCIE_DMA_CTRL dma_ctrl;

	// Write
	dma_ctrl.ch					= E_FJ_PCIE_CH0;
	dma_ctrl.src_addr_low		= 0x58000000;	// RC Memory Address
	dma_ctrl.src_addr_upp		= 0;
	dma_ctrl.dst_addr_low		= 0x5F000000;	// EP Memory Address
	dma_ctrl.dst_addr_upp		= 0
	dma_ctrl.trans_size			= 0x3FC000;
	dma_ctrl.direction			= E_FJ_PCIE_DMA_DIR_STOD;	// RC->EP
	dma_ctrl.int_dma_callback	= (PCIE_CALLBACK)pcie_rc_int_dma_cb;
	FJ_Pcie_Rc_Dma_Transfer(&dma_ctrl);

	// Read
	dma_ctrl.ch					= E_FJ_PCIE_CH0;
	dma_ctrl.src_addr_low		= 0x5F000000;	// EP Memory Address
	dma_ctrl.src_addr_upp		= 0;
	dma_ctrl.dst_addr_low		= 0x59000000;	// RC Memory Address
	dma_ctrl.dst_addr_upp		= 0
	dma_ctrl.trans_size			= 0x3FC000;
	dma_ctrl.direction			= E_FJ_PCIE_DMA_DIR_DTOS;	// EP->RC
	dma_ctrl.int_dma_callback	= (PCIE_CALLBACK)pcie_rc_int_dma_cb;
	FJ_Pcie_Rc_Dma_Transfer(&dma_ctrl);
}
@endcode
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Dma_Transfer( T_FJ_PCIE_DMA_CTRL const* const dma_ctrl );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
DMA transfer stop.
@param [in]	ch		Channel number. See @ref E_FJ_PCIE_CH
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Dma_Stop( E_FJ_PCIE_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Memory transfer settings.
@param [in]	memory_ctrl	Memory control data. See @ref T_FJ_PCIE_MEMORY_CTRL
@retval FJ_ERR_OK		Success.
@retval FJ_ERR_NG		Error occurred. (Not link up.)
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Memory_Ctrl( T_FJ_PCIE_MEMORY_CTRL const* const memory_ctrl );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
MSI settings.
@param [in]	msi_ctrl	MSI control data. See @ref T_FJ_PCIE_MSI_CTRL
@retval FJ_ERR_OK		Success.
@retval FJ_ERR_NG		Error occurred.
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Msi_Ctrl( T_FJ_PCIE_MSI_CTRL const* const msi_ctrl );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
It will be the setting of INTx detection.
@param [in]	ch		Channel number. See @ref E_FJ_PCIE_CH
@param [in]	intx_cb	Callback function of the INTx.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
@remarks
	 - This API Sample code is as follows.<br>
@code
[Sample Code]
static VOID pcie_rc_int_x_cb( INT32 status )
{
	// status
	// 1	INTA
	// 2	INTB
	// 4	INTC
	// 8	INTD
}

static VOID cmd_ct_pcie_rc_intx_ctrl( VOID );
{
	FJ_Pcie_Rc_Intx_Ctrl(E_FJ_PCIE_CH0, (PCIE_CALLBACK)pcie_rc_int_x_cb);
}
@endcode
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Intx_Ctrl( E_FJ_PCIE_CH ch, PCIE_CALLBACK intx_cb );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
It wants to turn ON the ASPM. (L0s and L1)
@param [in]	ch		Channel number. See @ref E_FJ_PCIE_CH
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Aspm_On( E_FJ_PCIE_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
It wants to turn OFF the ASPM.
@param [in]	ch		Channel number. See @ref E_FJ_PCIE_CH
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Aspm_Off( E_FJ_PCIE_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
It wants to change the transfer speed.
@param [in]	ch			Channel number. See @ref E_FJ_PCIE_CH
@param [in]	link_speed	Link speed. See @ref E_FJ_PCIE_LINK_SPEED
@retval FJ_ERR_OK		Success.
@retval FJ_ERR_NG		Error occurred.
@remarks
		- If you specify a Bifurcation Disable the argument of FJ_Pcie_Rc_Init, it is valid.<br>
		- It opposite device is 5.0GT/s support.<br>
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Transfer_Speed_Chg(  E_FJ_PCIE_CH ch, E_FJ_PCIE_LINK_SPEED link_speed );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get the STate of Data communication Link Up.
@param [in]	ch				Channel number. See @ref E_FJ_PCIE_CH
@retval E_FJ_PCIE_ST_DL_UP	STate of Data communication Link UP. See @ref E_FJ_PCIE_ST_DL_UP
*/
extern E_FJ_PCIE_ST_DL_UP FJ_Pcie_Rc_Get_St_Dl_Up( E_FJ_PCIE_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get the STate of Link Training and Status State Machine.
@param [in]	ch				Channel number. See @ref E_FJ_PCIE_CH
@retval E_FJ_PCIE_ST_LTSSM	STate of Link Training and Status State Machine. See @ref E_FJ_PCIE_ST_LTSSM
*/
extern E_FJ_PCIE_ST_LTSSM FJ_Pcie_Rc_Get_St_Ltssm( E_FJ_PCIE_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get the Power Management Status State.
@param [in]	ch			Channel number. See @ref E_FJ_PCIE_CH
@retval E_FJ_PCIE_PMST	Power state. See @ref E_FJ_PCIE_PMST
*/
extern E_FJ_PCIE_PMST FJ_Pcie_Rc_Get_Pmst( E_FJ_PCIE_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get current link speed.
@param [in]		ch			Channel number. See @ref E_FJ_PCIE_CH
@param [out]	link_speed	Link speed. See @ref E_FJ_PCIE_LINK_SPEED
@retval FJ_ERR_OK			Success.
@retval FJ_ERR_NG			Error occurred.
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Get_Current_Link_Speed( E_FJ_PCIE_CH ch, E_FJ_PCIE_LINK_SPEED* link_speed );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get the STate of ASPM.
@param [out]	pcie0_aspm	PCIE0 ASPM status.
@param [out]	pcie1_aspm	PCIE1 ASPM status.
@retval FJ_ERR_OK			Success.
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Get_St_Aspm( BOOL* pcie0_aspm, BOOL* pcie1_aspm );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set max payload size.
@param [in]	ch		Channel number. See @ref E_FJ_PCIE_CH
@param [in]	mps		Max payload size. See @ref E_FJ_PCIE_MPS
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Set_Mps( E_FJ_PCIE_CH ch, E_FJ_PCIE_MPS mps );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get max payload size.
@param [in]		ch		Channel number. See @ref E_FJ_PCIE_CH
@param [out]	mps		Max payload size. See @ref E_FJ_PCIE_MPS
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Get_Mps( E_FJ_PCIE_CH ch, E_FJ_PCIE_MPS* mps );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Link up callback function setting.
@param [in]	ch		Channel number. See @ref E_FJ_PCIE_CH
@param [in]	lu_cb	Link Up CallBack function pointer.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE FJ_Pcie_Rc_Set_Lu_Cb( E_FJ_PCIE_CH ch, PCIE_CALLBACK lu_cb );

extern FJ_ERR_CODE FJ_Pcie_Rc_Start_Task( VOID );
extern FJ_ERR_CODE FJ_Pcie_Rc_Set_Controller_Flag( UINT32 flag );
extern FJ_ERR_CODE FJ_Pcie_Rc_Set_Init_Address( E_FJ_PCIE_CH ch, UINT32 addr );

/*@} --- end of fj_pcie_rc_api */

///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/*+ @weakgroup fj_pcie_rc_message
@{*/
/*@} --- end of fj_pcie_rc_message */

#endif	// _FJ_PCIE_RC_H_
