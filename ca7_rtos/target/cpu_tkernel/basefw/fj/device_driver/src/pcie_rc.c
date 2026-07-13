/**
 * @file		pcie_rc.c
 * @brief		PCIe RootComplex Controller
 * @note	  : None
 * @attention : None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "fj_pcie.h"
#include "fj_pcie_rc.h"
#include "fj_memory.h"
#include "pcie_rc.h"
#include "dd_pcie_comm.h"
#include "dd_pcie_rc.h"
#include "debug.h"
#include "os_user_custom.h"
#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define	D_PCIE_RC_DEBUG
#define PCIE0_MEM_BAR_ADR	0x19400000	// Memory Base Address ch0 (PIO area)
#define PCIE0_MEM_LMT_ADR	0x1940FFFF	// Limit Address (64KBytes)
#define PCIE0_MEM_TGT_ADR	0x58000000	// Memory Target Address

#define PCIE0_CFG_BAR_ADR	0x19600000	// Config Base Address ch0 (PIO area)
#define PCIE0_CFG_LMT_ADR	0x1960FFFF	// Limit Address (64KBytes)

#define PCIE1_MEM_BAR_ADR	0x19800000	// Memory Base Address ch1 (PIO area)
#define PCIE1_MEM_LMT_ADR	0x1980FFFF	// Limit Address (64KBytes)
#define PCIE1_MEM_TGT_ADR	0x59000000	// Memory Target Address

#define PCIE1_CFG_BAR_ADR	0x19A00000	// Config Base Address ch1 (PIO area)
#define PCIE1_CFG_LMT_ADR	0x19A0FFFF	// Limit Address (64KBytes)

#define PCIE0_MSI_CMP_ADR	0x18000000	// MSI compare address ch0 (INTMEM)
#define PCIE1_MSI_CMP_ADR	0x18004000	// MSI compare address ch1 (INTMEM)

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
/* bifurcation mode */
static volatile E_FJ_PCIE_BIFURCATION gBF_PCIE_RC_bifurcation = E_FJ_PCIE_BIFURCATION_DISABLE;

static volatile E_DD_PCIE_DMA_DIR gBF_PCIE_RC_DMA_Dir[2] = {E_DD_PCIE_DMA_DIR_STOD, E_DD_PCIE_DMA_DIR_STOD};

/* ASPM on_off */
static volatile BOOL gBF_PCIE_RC_aspm[2] = {FALSE, FALSE};

static volatile E_FJ_PCIE_BIFURCATION	gBF_PCIE_RC_Bifurcation = -1;

static volatile UINT32	gBF_PCIE_RC_TargetAddr_Ch0 = PCIE0_MEM_TGT_ADR;
static volatile UINT32	gBF_PCIE_RC_TargetAddr_Ch1 = PCIE1_MEM_TGT_ADR;

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static VOID pcie_rc_int_msi_cb(INT32 status)
{
	// status
	// bit0 MSI Interrupt. (value:1)

	BF_Debug_Print_Information(("pcie_rc_int_msi_cb called. Status=0x%08X\n", status));
}

static FJ_ERR_CODE pcie_rc_config_ctrl( E_FJ_PCIE_CH ch )
{
	FJ_ERR_CODE				ret = FJ_ERR_OK;
	T_FJ_PCIE_RC_CTRL_CFG	pcie_ctrl_cfg;

	if(ch == E_FJ_PCIE_CH0){
		pcie_ctrl_cfg.ch		= E_FJ_PCIE_CH0;
		pcie_ctrl_cfg.base_adr	= PCIE0_CFG_BAR_ADR;
		pcie_ctrl_cfg.limit_adr	= PCIE0_CFG_LMT_ADR;
	}
	else{
		pcie_ctrl_cfg.ch		= E_FJ_PCIE_CH1;
		pcie_ctrl_cfg.base_adr	= PCIE1_CFG_BAR_ADR;
		pcie_ctrl_cfg.limit_adr	= PCIE1_CFG_LMT_ADR;
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
		memory_ctrl.target_adr_low	= gBF_PCIE_RC_TargetAddr_Ch0;
		memory_ctrl.target_adr_upp	= 0;
	}
	else{
		memory_ctrl.ch				= E_FJ_PCIE_CH1;
		memory_ctrl.base_adr		= PCIE1_MEM_BAR_ADR;
		memory_ctrl.limit_adr		= PCIE1_MEM_LMT_ADR;
		memory_ctrl.target_adr_low	= gBF_PCIE_RC_TargetAddr_Ch1;
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
		FJ_Pcie_Rc_Config_Write(ch, 0x0918, gBF_PCIE_RC_TargetAddr_Ch0);
		FJ_Pcie_Rc_Config_Write(ch, 0x0010, gBF_PCIE_RC_TargetAddr_Ch0);
	}
	else{
		FJ_Pcie_Rc_Config_Write(ch, 0x0918, gBF_PCIE_RC_TargetAddr_Ch1);
		FJ_Pcie_Rc_Config_Write(ch, 0x0010, gBF_PCIE_RC_TargetAddr_Ch1);
	}
	FJ_Pcie_Rc_Config_Write(ch, 0x091C, 0x00000000);
	FJ_Pcie_Rc_Config_Write(ch, 0x0908, 0xC0000000);

#ifdef D_PCIE_RC_DEBUG
	BF_Debug_Print_Information(("pcie_rc_milb_ep_config normal end.\n"));
#endif
	return FJ_ERR_OK;
}

static FJ_ERR_CODE pcie_rc_m8m_ep_config( E_FJ_PCIE_CH ch )
{
	UINT32	msi_addr;
	UINT32	data;

	if(ch == E_FJ_PCIE_CH0){
		msi_addr = PCIE0_MSI_CMP_ADR;
	}
	else{
		msi_addr = PCIE1_MSI_CMP_ADR;
	}

	FJ_Pcie_Rc_Config_Read(ch, 0x0004, &data);
	data |= 0x00000004;	// bit2 Bus Master Enable
	FJ_Pcie_Rc_Config_Write(ch, 0x0004, data);
	FJ_Pcie_Rc_Config_Read(ch, 0x00C0, &data);
	data |= 0x00210000;	// 0x00C2 bit0 MSI Enable, bit6:4 Multiple Message Enable=2
	FJ_Pcie_Rc_Config_Write(ch, 0x00C0, data);
	FJ_Pcie_Rc_Config_Write(ch, 0x00C4, msi_addr);
	FJ_Pcie_Rc_Config_Write(ch, 0x00CC, 0);

	// ASPM ON
	FJ_Pcie_Rc_Config_Read(ch, 0x090, &data);
	data |= 0x00000003;	// bit1:0 L0s and L1 Entry Enabled
	FJ_Pcie_Rc_Config_Write(ch, 0x090, data);

#ifdef D_PCIE_RC_DEBUG
	BF_Debug_Print_Information(("pcie_rc_m8m_ep_config normal end.\n"));
#endif

	return FJ_ERR_OK;
}

static VOID pcie_rc_pre_process1( VOID )
{
	FJ_ERR_CODE ret=0;

	// Init
	ret = FJ_Pcie_Rc_Init(E_FJ_PCIE_BIFURCATION_DISABLE);
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Rc_Init() Error. ret=0x%04X\n", ret);
		return;
	}
	gBF_PCIE_RC_Bifurcation = E_FJ_PCIE_BIFURCATION_DISABLE;

	// Open
	ret = FJ_Pcie_Rc_Open();
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Rc_Open() Error. ret=0x%04X\n", ret);
		return;
	}

#ifdef D_PCIE_RC_DEBUG
	BF_Debug_Print_Information(("pcie_rc_pre_process1 normal end.\n"));
#endif
}

static VOID pcie_rc_post_process1( VOID )
{
	FJ_ERR_CODE			ret=0;
	E_FJ_PCIE_ST_LTSSM	ltssm;

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

	while( 1 ){
		if( FJ_Pcie_Rc_Get_Pmst(E_FJ_PCIE_CH0) == E_FJ_PCIE_PMST_D0 ){
			break;
		}
		else{
			OS_User_Dly_Tsk(1);
		}
	}

	while( 1 ){
		ltssm = FJ_Pcie_Rc_Get_St_Ltssm( E_FJ_PCIE_CH0 );
		if( ltssm >= E_FJ_PCIE_S_L0 && ltssm <= E_FJ_PCIE_S_L1_IDLE ){
			break;
		}
		else{
			OS_User_Dly_Tsk(1);
		}
	}

	// ASPM ON
	ret = FJ_Pcie_Rc_Aspm_On(E_FJ_PCIE_CH0);
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Rc_Aspm_On() Error. ret=0x%04X\n", ret);
		return;
	}

#ifdef D_PCIE_RC_DEBUG
	BF_Debug_Print_Information(("pcie_rc_post_process1 normal end.\n"));
#endif
}

static VOID pcie_rc_pre_process2( VOID )
{
	FJ_ERR_CODE ret=0;

	// Init
	ret = FJ_Pcie_Rc_Init(E_FJ_PCIE_BIFURCATION_ENABLE);
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Rc_Init() Error. ret=0x%04X\n", ret);
		return;
	}
	gBF_PCIE_RC_Bifurcation = E_FJ_PCIE_BIFURCATION_ENABLE;

	// Open
	ret = FJ_Pcie_Rc_Open();
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Rc_Open() Error. ret=0x%04X\n", ret);
		return;
	}

#ifdef D_PCIE_RC_DEBUG
	BF_Debug_Print_Information(("pcie_rc_pre_process2 normal end.\n"));
#endif
}

static VOID pcie_rc_post_process2_ch0( VOID )
{
	FJ_ERR_CODE			ret=0;
	E_FJ_PCIE_ST_LTSSM	ltssm;

	// Config Control
	ret = pcie_rc_config_ctrl(E_FJ_PCIE_CH0);
	if(ret != FJ_ERR_OK){
		BF_Debug_Print_Information(("pcie_rc_config_ctrl(0) Error. ret=0x%04X\n", ret));
		return;
	}

	pcie_rc_milb_ep_config(E_FJ_PCIE_CH0);

	// Link Spped Change (2.5GT/s -> 5.0GT/s)
	ret = FJ_Pcie_Rc_Transfer_Speed_Chg(E_FJ_PCIE_CH0, E_FJ_PCIE_LINK_SPEED_GEN2);
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Rc_Transfer_Speed_Chg(0) Error. ret=0x%04X\n", ret);
		return;
	}

	// MSI Control
	ret = pcie_rc_msi_ctrl(E_FJ_PCIE_CH0);
	if(ret != FJ_ERR_OK){
		BF_Debug_Print_Information(("pcie_rc_msi_ctrl(0) Error. ret=0x%04X\n", ret));
		return;
	}

	// Memory Control
	ret = pcie_rc_mem_ctrl(E_FJ_PCIE_CH0);
	if(ret != FJ_ERR_OK){
		BF_Debug_Print_Information(("pcie_rc_mem_ctrl(0) Error. ret=0x%04X\n", ret));
		return;
	}

	memcpy((void*)PCIE0_MEM_BAR_ADR, "PCIe RC Start 00", 16);
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, PCIE0_MEM_BAR_ADR, 16);

	while( 1 ){
		if( FJ_Pcie_Rc_Get_Pmst(E_FJ_PCIE_CH0) == E_FJ_PCIE_PMST_D0 ){
			break;
		}
		else{
			OS_User_Dly_Tsk(1);
		}
	}

	while( 1 ){
		ltssm = FJ_Pcie_Rc_Get_St_Ltssm( E_FJ_PCIE_CH0 );
		if( ltssm >= E_FJ_PCIE_S_L0 && ltssm <= E_FJ_PCIE_S_L1_IDLE ){
			break;
		}
		else{
			OS_User_Dly_Tsk(1);
		}
	}

	// ASPM ON
	ret = FJ_Pcie_Rc_Aspm_On(E_FJ_PCIE_CH0);
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Rc_Aspm_On(0) Error. ret=0x%04X\n", ret);
		return;
	}

#ifdef D_PCIE_RC_DEBUG
	BF_Debug_Print_Information(("pcie_rc_post_process2 normal end.\n"));
#endif
}

static VOID pcie_rc_post_process2_ch1( VOID )
{
	FJ_ERR_CODE			ret=0;
	E_FJ_PCIE_ST_LTSSM	ltssm;

	// Config Control
	ret = pcie_rc_config_ctrl(E_FJ_PCIE_CH1);
	if(ret != FJ_ERR_OK){
		BF_Debug_Print_Information(("pcie_rc_config_ctrl(1) Error. ret=0x%04X\n", ret));
		return;
	}

//	pcie_rc_milb_ep_config(E_FJ_PCIE_CH1);
	pcie_rc_m8m_ep_config(E_FJ_PCIE_CH1);
	FJ_Pcie_Rc_Set_Mps(E_FJ_PCIE_CH1, E_FJ_PCIE_DMA_MPS_128);	// Set M8M Max Payload Size.

	// Link Spped Change (2.5GT/s -> 5.0GT/s)
	ret = FJ_Pcie_Rc_Transfer_Speed_Chg(E_FJ_PCIE_CH1, E_FJ_PCIE_LINK_SPEED_GEN2);
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Rc_Transfer_Speed_Chg(1) Error. ret=0x%04X\n", ret);
		return;
	}

	// MSI Control
	ret = pcie_rc_msi_ctrl(E_FJ_PCIE_CH1);
	if(ret != FJ_ERR_OK){
		BF_Debug_Print_Information(("pcie_rc_msi_ctrl(1) Error. ret=0x%04X\n", ret));
		return;
	}

	// Memory Control
	ret = pcie_rc_mem_ctrl(E_FJ_PCIE_CH1);
	if(ret != FJ_ERR_OK){
		BF_Debug_Print_Information(("pcie_rc_mem_ctrl(1) Error. ret=0x%04X\n", ret));
		return;
	}

	memcpy((void*)PCIE1_MEM_BAR_ADR, "PCIe RC Start 01", 16);
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, PCIE1_MEM_BAR_ADR, 16);

	while( 1 ){
		if( FJ_Pcie_Rc_Get_Pmst(E_FJ_PCIE_CH1) == E_FJ_PCIE_PMST_D0 ){
			break;
		}
		else{
			OS_User_Dly_Tsk(1);
		}
	}

	while( 1 ){
		ltssm = FJ_Pcie_Rc_Get_St_Ltssm( E_FJ_PCIE_CH1 );
		if( ltssm >= E_FJ_PCIE_S_L0 && ltssm <= E_FJ_PCIE_S_L1_IDLE ){
			break;
		}
		else{
			OS_User_Dly_Tsk(1);
		}
	}

	// ASPM ON
	ret = FJ_Pcie_Rc_Aspm_On(E_FJ_PCIE_CH1);
	if(ret != FJ_ERR_OK){
		printf("FJ_Pcie_Rc_Aspm_On(1) Error. ret=0x%04X\n", ret);
		return;
	}

#ifdef D_PCIE_RC_DEBUG
	BF_Debug_Print_Information(("pcie_rc_post_process2 normal end.\n"));
#endif
}

static VOID pcie_rc_end( VOID )
{
	FJ_ERR_CODE	ret;

	if(gBF_PCIE_RC_Bifurcation == E_FJ_PCIE_BIFURCATION_DISABLE){
		ret = FJ_Pcie_Rc_Dma_Stop(E_FJ_PCIE_CH0);
		if(ret != FJ_ERR_OK){
			BF_Debug_Print_Information(("FJ_Pcie_Rc_Dma_Stop(0) Error. ret=0x%04X\n", ret));
			return;
		}
	}
	else if(gBF_PCIE_RC_Bifurcation == E_FJ_PCIE_BIFURCATION_ENABLE){
		ret = FJ_Pcie_Rc_Dma_Stop(E_FJ_PCIE_CH0);
		if(ret != FJ_ERR_OK){
			BF_Debug_Print_Information(("FJ_Pcie_Rc_Dma_Stop(0) Error. ret=0x%04X\n", ret));
			return;
		}
		ret = FJ_Pcie_Rc_Dma_Stop(E_FJ_PCIE_CH1);
		if(ret != FJ_ERR_OK){
			BF_Debug_Print_Information(("FJ_Pcie_Rc_Dma_Stop(1) Error. ret=0x%04X\n", ret));
			return;
		}
	}

	if(gBF_PCIE_RC_Bifurcation == E_FJ_PCIE_BIFURCATION_DISABLE ||
	   gBF_PCIE_RC_Bifurcation == E_FJ_PCIE_BIFURCATION_ENABLE){
		ret = FJ_Pcie_Rc_Close();
		if(ret != FJ_ERR_OK){
			BF_Debug_Print_Information(("FJ_Pcie_Rc_Close() Error. ret=0x%04X\n", ret));
			return;
		}

		FJ_Pcie_Rc_Deinit();
	}

#ifdef D_PCIE_RC_DEBUG
	BF_Debug_Print_Information(("pcie_rc_end normal end.\n"));
#endif
}


/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief 		: PCIE RC control task routine
 * @param		: i:stacd
 * @return		: None
 * @note		: None
 * @attention	: None
*/
VOID BF_PCIE_RC_Controller(INT32 stacd)
{
	OS_USER_ER		ercd;
	OS_USER_FLGPTN	flg = 0;
	OS_USER_FLGPTN	flgptn = 0;

#ifdef D_PCIE_RC_DEBUG
	BF_Debug_Print_Information(("BF_PCIE_RC_Controller task start\n"));
#endif
	flg  = FLG_PCIE_RC_CONTROLLER_START1;
	flg |= FLG_PCIE_RC_CONTROLLER_START2;
	flg |= FLG_PCIE_RC_CONTROLLER_LU_INT1;
	flg |= FLG_PCIE_RC_CONTROLLER_LU_INT2_CH0;
	flg |= FLG_PCIE_RC_CONTROLLER_LU_INT2_CH1;
	flg |= FLG_PCIE_RC_CONTROLLER_END;
	ercd = OS_User_Clr_Flg( FID_PCIE_RC_CONTROLLER, ~flg );

	for(;;){
#ifdef D_PCIE_RC_DEBUG
		BF_Debug_Print_Information(("BF_PCIE_RC_Controller task loop start wai=0x%08x\n", (UINT32)flg));
#endif
		ercd = OS_User_Wai_Flg( FID_PCIE_RC_CONTROLLER, flg, D_OS_USER_TWF_ORW, &flgptn);
		
		if( ercd != D_OS_USER_E_OK ) {
			BF_Debug_Print_Error(("[BF_PCIE_RC_Controller]wai_flg error(%d) L=%d\n", ercd, __LINE__));
		}
		else {
#ifdef D_PCIE_RC_DEBUG
			BF_Debug_Print_Information(("BF_PCIE_RC_Controller FLG=0x%08x\n", (UINT32)flgptn));
#endif

			if (flgptn & FLG_PCIE_RC_CONTROLLER_START1) {
				ercd = OS_User_Clr_Flg( FID_PCIE_RC_CONTROLLER, ~FLG_PCIE_RC_CONTROLLER_START1 );
#ifdef D_PCIE_RC_DEBUG
				BF_Debug_Print_Information(("FLG_PCIE_RC_CONTROLLER_START1 called.\n"));
#endif
				pcie_rc_pre_process1();
			}
			else if (flgptn & FLG_PCIE_RC_CONTROLLER_START2) {
				ercd = OS_User_Clr_Flg( FID_PCIE_RC_CONTROLLER, ~FLG_PCIE_RC_CONTROLLER_START2 );
#ifdef D_PCIE_RC_DEBUG
				BF_Debug_Print_Information(("FLG_PCIE_RC_CONTROLLER_START2 called.\n"));
#endif
				pcie_rc_pre_process2();
			}
			else if (flgptn & FLG_PCIE_RC_CONTROLLER_LU_INT1) {
				ercd = OS_User_Clr_Flg( FID_PCIE_RC_CONTROLLER, ~FLG_PCIE_RC_CONTROLLER_LU_INT1 );
#ifdef D_PCIE_RC_DEBUG
				BF_Debug_Print_Information(("FLG_PCIE_RC_CONTROLLER_LU_INT1 called.\n"));
#endif
				pcie_rc_post_process1();
			}
			else if (flgptn & FLG_PCIE_RC_CONTROLLER_LU_INT2_CH0) {
				ercd = OS_User_Clr_Flg( FID_PCIE_RC_CONTROLLER, ~FLG_PCIE_RC_CONTROLLER_LU_INT2_CH0 );
#ifdef D_PCIE_RC_DEBUG
				BF_Debug_Print_Information(("FLG_PCIE_RC_CONTROLLER_LU_INT2_CH0 called.\n"));
#endif
				pcie_rc_post_process2_ch0();
			}
			else if (flgptn & FLG_PCIE_RC_CONTROLLER_LU_INT2_CH1) {
				ercd = OS_User_Clr_Flg( FID_PCIE_RC_CONTROLLER, ~FLG_PCIE_RC_CONTROLLER_LU_INT2_CH1 );
#ifdef D_PCIE_RC_DEBUG
				BF_Debug_Print_Information(("FLG_PCIE_RC_CONTROLLER_LU_INT2_CH1 called.\n"));
#endif
				pcie_rc_post_process2_ch1();
			}

			else if (flgptn & FLG_PCIE_RC_CONTROLLER_END) {
				ercd = OS_User_Clr_Flg( FID_PCIE_RC_CONTROLLER, ~FLG_PCIE_RC_CONTROLLER_END );
#ifdef D_PCIE_RC_DEBUG
				BF_Debug_Print_Information(("FLG_PCIE_RC_CONTROLLER_END called.\n"));
#endif
				pcie_rc_end();
				break;
			}
		}
#ifdef D_PCIE_RC_DEBUG
		BF_Debug_Print_Information(("BF_PCIE_RC_Controller task loop next\n"));
#endif
		OS_User_Dly_Tsk(1);
	}
#ifdef D_PCIE_RC_DEBUG
	BF_Debug_Print_Information(("BF_PCIE_RC_Controller task end\n"));
#endif
	OS_User_Ext_Tsk();
}

// ***************************************************************************
//                    API
// ***************************************************************************
/**
 *	@brief		It opens the PCIe.
 *	@param		None
 *	@return		FJ_ERR_OK	Success
 *	@return		FJ_ERR_NG	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_rc_open( VOID )
{
	INT32 ret;

	ret = Dd_Pcie_Rc_Open(D_DDIM_WAIT_END_TIME);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_rc_open() - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		It closes the PCIe.
 *	@param		None
 *	@return		FJ_ERR_OK	Success
 *	@return		FJ_ERR_NG	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_rc_close( VOID )
{
	INT32 ret;

	ret = Dd_Pcie_Rc_Close();
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_rc_close() - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		It initializes the PCIe.
 *	@param		i:bifurcation	bifurcation mode.
 *	@return		FJ_ERR_OK		Success
 *	@return		FJ_ERR_NG		Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_rc_init( E_FJ_PCIE_BIFURCATION bifurcation )
{
	gBF_PCIE_RC_bifurcation = bifurcation;

	Dd_Pcie_Rc_Init((E_DD_PCIE_BIFURCATION)bifurcation);

	return FJ_ERR_OK;
}

/**
 *	@brief		PCIe end.
 *	@param		None
 *	@return		FJ_ERR_OK		Success
 *	@return		FJ_ERR_NG		Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_rc_deinit( VOID )
{
	Dd_Pcie_Rc_Deinit();
	gBF_PCIE_RC_bifurcation = E_FJ_PCIE_BIFURCATION_DISABLE;
	gBF_PCIE_RC_aspm[E_DD_PCIE_CH0] = FALSE;
	gBF_PCIE_RC_aspm[E_DD_PCIE_CH0] = FALSE;

	return FJ_ERR_OK;
}

/**
 *	@brief		Configuration transfer set.
 *	@param		i:pcie_ctrl_cfg	config control data
 *	@return		FJ_ERR_OK	Success
 *	@return		FJ_ERR_NG	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_rc_config_ctrl( T_FJ_PCIE_RC_CTRL_CFG const* const pcie_ctrl_cfg )
{
	INT32	ret;
	T_DD_PCIE_RC_CTRL_CFG	dd_pcie_ctrl_cfg;

	dd_pcie_ctrl_cfg.ch				= (E_DD_PCIE_CH)pcie_ctrl_cfg->ch;
	dd_pcie_ctrl_cfg.base_adr		= pcie_ctrl_cfg->base_adr;
	dd_pcie_ctrl_cfg.limit_adr		= pcie_ctrl_cfg->limit_adr;
	ret = Dd_Pcie_Rc_Ctrl_Config(&dd_pcie_ctrl_cfg);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_rc_config_ctrl(%d) - Error! : ret = 0x%08X\n", pcie_ctrl_cfg->ch, ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		It will read the config.
 *	@param		i:ch		Channel number
 *	@param		i:offset	Configuration register address offset
 *	@param		i:data		Read data
 *	@return		FJ_ERR_OK	Success
 *	@return		FJ_ERR_NG	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_rc_config_read( E_FJ_PCIE_CH ch, USHORT offset, UINT32* data )
{
	INT32	ret;

	ret = Dd_Pcie_Rc_Config_Read((E_DD_PCIE_CH)ch, offset, data);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_rc_config_read(%d) - Error! : ret = 0x%08X\n", ch, ret));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		It will write the config.
 *	@param		i:ch		Channel number
 *	@param		i:offset	Configuration register address offset
 *	@param		i:data		Write data
 *	@return		FJ_ERR_OK	Success
 *	@return		FJ_ERR_NG	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_rc_config_write( E_FJ_PCIE_CH ch, USHORT offset, UINT32 data )
{
	INT32	ret;

	ret = Dd_Pcie_Rc_Config_Write((E_DD_PCIE_CH)ch, offset, data);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_rc_config_write(%d) - Error! : ret = 0x%08X\n", ch, ret));
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 *	@brief		DMA transfer.
 *	@param		i:dma_ctrl	DMA control data
 *	@return		FJ_ERR_OK	Success
 *	@return		FJ_ERR_NG	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_rc_dma_transfer( T_FJ_PCIE_DMA_CTRL const* const dma_ctrl )
{
	INT32				ret;
	T_DD_PCIE_CTRL_DMA	dd_ctrl_dma;

	dd_ctrl_dma.mode			= E_DD_PCIE_DMA_DEMAND_MODE;
	dd_ctrl_dma.ch				= (E_DD_PCIE_CH)dma_ctrl->ch;
	dd_ctrl_dma.dma_ch			= E_DD_PCIE_DMA_CH0;
	dd_ctrl_dma.dmnd_src_addr_l	= dma_ctrl->src_addr_low;
	dd_ctrl_dma.dmnd_src_addr_h	= dma_ctrl->src_addr_upp;
	dd_ctrl_dma.dmnd_dst_addr_l	= dma_ctrl->dst_addr_low;
	dd_ctrl_dma.dmnd_dst_addr_h	= dma_ctrl->dst_addr_upp;
	dd_ctrl_dma.dmnd_trans_size	= dma_ctrl->trans_size;
	dd_ctrl_dma.ll_addr			= 0;
	dd_ctrl_dma.direction		= (E_DD_PCIE_DMA_DIR)dma_ctrl->direction;
	gBF_PCIE_RC_DMA_Dir[dma_ctrl->ch] = dd_ctrl_dma.direction;
	dd_ctrl_dma.int_dma_cb		= dma_ctrl->int_dma_callback;
	ret = Dd_Pcie_Rc_Ctrl_Dma(&dd_ctrl_dma);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_rc_demand_dma_transfer(%d) - Error! : ret = 0x%08X\n", dma_ctrl->ch, ret));
		return FJ_ERR_NG;
	}

	Dd_Pcie_Rc_Start_Dma(dd_ctrl_dma.ch, dd_ctrl_dma.dma_ch, dd_ctrl_dma.direction);

	return FJ_ERR_OK;
}

/**
 *	@brief		DMA transfer stop.
 *	@param		i:ch		Channel number
 *	@return		FJ_ERR_OK	Success
 *	@return		FJ_ERR_NG	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_rc_dma_stop( E_FJ_PCIE_CH ch )
{
	Dd_Pcie_Rc_Stop_Dma((E_DD_PCIE_CH)ch, E_DD_PCIE_DMA_CH0, gBF_PCIE_RC_DMA_Dir[ch]);

	return FJ_ERR_OK;
}

/**
 *	@brief		Memory transfer settings.
 *	@param		i:memory_ctrl	Memory control data
 *	@return		FJ_ERR_OK		Success
 *	@return		FJ_ERR_NG		Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_rc_memory_ctrl( T_FJ_PCIE_MEMORY_CTRL const* const memory_ctrl )
{
	INT32				ret;
	T_DD_PCIE_CTRL_MEM	dd_pcie_ctrl_mem;

	dd_pcie_ctrl_mem.ch				= memory_ctrl->ch;
	dd_pcie_ctrl_mem.base_adr		= memory_ctrl->base_adr;
	dd_pcie_ctrl_mem.limit_adr		= memory_ctrl->limit_adr;
	dd_pcie_ctrl_mem.low_target_adr	= memory_ctrl->target_adr_low;
	dd_pcie_ctrl_mem.upp_target_adr	= memory_ctrl->target_adr_upp;
	ret = Dd_Pcie_Rc_Ctrl_Memory(&dd_pcie_ctrl_mem);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_rc_memory_ctrl(%d) - Error! : ret = 0x%08X\n", memory_ctrl->ch, ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		MSI settings.
 *	@param		i:msi_ctrl	MSI control data
 *	@return		FJ_ERR_OK	Success
 *	@return		FJ_ERR_NG	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_rc_msi_ctrl( T_FJ_PCIE_MSI_CTRL const* const msi_ctrl )
{
	INT32					ret;
	T_DD_PCIE_RC_CTRL_MSI	pcie_ctrl_msi;

	pcie_ctrl_msi.ch			= msi_ctrl->ch;
	pcie_ctrl_msi.int_en		= msi_ctrl->int_en;
	pcie_ctrl_msi.cmp_adr		= msi_ctrl->cmp_adr;
	pcie_ctrl_msi.int_msi_cb	= msi_ctrl->int_msi_callback;
	ret = Dd_Pcie_Rc_Ctrl_Msi(&pcie_ctrl_msi);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_rc_msi_ctrl(%d) - Error! : ret = 0x%08X\n", msi_ctrl->ch, ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		It will be the setting of INTx detection.
 *	@param		i:ch		Channel number
 *	@param		i:intx_cb	Callback function of the INTx
 *	@return		FJ_ERR_OK	Success
 *	@return		FJ_ERR_NG	Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_rc_intx_ctrl( E_FJ_PCIE_CH ch, PCIE_CALLBACK intx_cb )
{
	INT32					ret;
	T_DD_PCIE_RC_INT_EP_CB	int_ep_cb;

	memset(&int_ep_cb, 0, sizeof(int_ep_cb));
	int_ep_cb.radm_intx_cb = intx_cb;
	ret = Dd_Pcie_Rc_Set_Int_Ep_Func((E_DD_PCIE_CH)ch, &int_ep_cb);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("Dd_Pcie_Rc_Set_Int_Ep_Func(%d) - Error! : ret = 0x%08X\n", ch, ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		It wants to turn ON the ASPM. (L0s and L1)
 *	@param		i:ch		Channel number
 *	@return		FJ_ERR_OK			Success
 *	@return		FJ_ERR_NG			Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_rc_aspm_on( E_FJ_PCIE_CH ch )
{
	INT32	ret;

	ret = Dd_Pcie_Rc_Ctrl_Aspm((E_DD_PCIE_CH)ch, E_DD_PCIE_ASPM_CONTROL_L0S_L1);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_rc_aspm_on(%d) - Error! : ret = 0x%08X\n", ch, ret));
		return FJ_ERR_NG;
	}

	gBF_PCIE_RC_aspm[ch] = TRUE;

	return FJ_ERR_OK;
}

/**
 *	@brief		It wants to turn OFF the ASPM.
 *	@param		i:ch		Channel number
 *	@return		FJ_ERR_OK			Success
 *	@return		FJ_ERR_NG			Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_rc_aspm_off( E_FJ_PCIE_CH ch )
{
	INT32	ret;

	ret = Dd_Pcie_Rc_Ctrl_Aspm((E_DD_PCIE_CH)ch, E_DD_PCIE_ASPM_CONTROL_DISABLE);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_rc_aspm_off(0) - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}
	gBF_PCIE_RC_aspm[ch] = FALSE;

	return FJ_ERR_OK;
}

/**
 *	@brief		It wants to change the transfer speed.
 *	@param		i:ch			Channel number
 *	@param		i:link_speed	Link speed
 *	@return		FJ_ERR_OK		Success
 *	@return		FJ_ERR_NG		Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_rc_transfer_speed_chg( E_FJ_PCIE_CH ch, E_FJ_PCIE_LINK_SPEED link_speed )
{
	INT32	ret;

	ret = Dd_Pcie_Rc_Transfer_Speed_Change((E_DD_PCIE_CH)ch, (E_DD_PCIE_LINK_SPEED)link_speed);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_rc_transfer_speed_chg(%d) - Error! : ret = 0x%08X\n", ch, ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		Get the STate of Data communication Link UP.
 *	@param		i:ch				Channel number
 *	@return		E_FJ_PCIE_ST_DL_UP	STate of Data communication Link UP
 *	@note		None
 *	@attention	None
 */
E_FJ_PCIE_ST_DL_UP fj_pcie_rc_get_st_dl_up( E_FJ_PCIE_CH ch )
{
	return (E_FJ_PCIE_ST_DL_UP)Dd_Pcie_Rc_Get_Link_Status((E_DD_PCIE_CH)ch);
}

/**
 *	@brief		Get the STate of Link Training and Status State Machine.
 *	@param		i:ch				Channel number
 *	@return		E_FJ_PCIE_ST_LTSSM	STate of Link Training and Status State Machine
 *	@note		None
 *	@attention	None
 */
E_FJ_PCIE_ST_LTSSM fj_pcie_rc_get_st_ltssm( E_FJ_PCIE_CH ch )
{
	return (E_FJ_PCIE_ST_LTSSM)Dd_Pcie_Rc_Get_ST_LTSSM_Status((E_DD_PCIE_CH)ch);
}

/**
 *	@brief		Get the Power Management Status State.
 *	@param		i:ch			Channel number
 *	@return		E_FJ_PCIE_PMST	Power Management Status State
 *	@note		None
 *	@attention	None
 */
E_FJ_PCIE_PMST fj_pcie_rc_get_pmst( E_FJ_PCIE_CH ch )
{
	return (E_FJ_PCIE_PMST)Dd_Pcie_Rc_Get_PMST_Status((E_DD_PCIE_CH)ch);
}

/**
 *	@brief		Get current link speed.
 *	@param		i:ch			Channel number
 *	@param		o:link_speed	Link speed
 *	@return		FJ_ERR_OK		Success
 *	@return		FJ_ERR_NG		Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_rc_get_current_link_speed( E_FJ_PCIE_CH ch, E_FJ_PCIE_LINK_SPEED* link_speed  )
{
	INT32	ret;

	ret = Dd_Pcie_Rc_Get_Current_Link_Speed((E_DD_PCIE_CH)ch, (E_DD_PCIE_LINK_SPEED*)link_speed);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_rc_get_current_link_speed(%d) - Error! : ret = 0x%08X\n", ch, ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		Get the STate of ASPM.
 *	@param		o:pcie0_aspm	PCIE0 ASPM status
 *	@param		o:pcie1_aspm	PCIE1 ASPM status
 *	@return		FJ_ERR_OK		Success
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_rc_get_st_aspm( BOOL* pcie0_aspm, BOOL* pcie1_aspm )
{
	*pcie0_aspm = gBF_PCIE_RC_aspm[E_FJ_PCIE_CH0];
	*pcie1_aspm = gBF_PCIE_RC_aspm[E_FJ_PCIE_CH1];

	return FJ_ERR_OK;
}

/**
 *	@brief		Set max payload size.
 *	@param		i:ch			Channel number
 *	@param		i:mps			Max Payload Size
 *	@return		FJ_ERR_OK		Success
 *	@return		FJ_ERR_NG		Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_rc_set_mps( E_FJ_PCIE_CH ch, E_FJ_PCIE_MPS mps )
{
	INT32	ret;

	ret = Dd_Pcie_Rc_Set_Mps((E_DD_PCIE_CH)ch, (E_DD_PCIE_MPS)mps);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_rc_set_mps(%d) - Error! : ret = 0x%08X\n", ch, ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		Get max payload size.
 *	@param		i:ch			Channel number
 *	@param		o:mps			Max Payload Size
 *	@return		FJ_ERR_OK		Success
 *	@return		FJ_ERR_NG		Fail - FJ error code according to the specific error.
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_pcie_rc_get_mps( E_FJ_PCIE_CH ch, E_FJ_PCIE_MPS* mps )
{
	INT32	ret;

	ret = Dd_Pcie_Rc_Get_Mps((E_DD_PCIE_CH)ch, (E_DD_PCIE_MPS*)mps);
	if(ret != D_DDIM_OK) {
		BF_Debug_Print_Warning(("fj_pcie_rc_get_mps(%d) - Error! : ret = 0x%08X\n", ch, ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

// Link up callback function setting.
FJ_ERR_CODE fj_pcie_rc_set_lu_cb( E_FJ_PCIE_CH ch, PCIE_CALLBACK lu_cb )
{
	T_DD_PCIE_RC_INT_OWN_CB int_own_cb;
	
	memset(&int_own_cb, 0, sizeof(int_own_cb));
	int_own_cb.rdlh_link_up_cb = lu_cb;
	Dd_Pcie_Rc_Set_Int_Own_Func((E_DD_PCIE_CH)ch, &int_own_cb);

	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_pcie_rc_start_task( VOID )
{
	FJ_ERR_CODE 	ret = FJ_ERR_OK;
	OS_USER_ER		ercd = D_OS_USER_E_OK;
	T_OS_USER_RTSK	pk_rtsk;

	ercd = OS_User_Ref_Tsk( TID_PCIE_RC_CONTROLLER, &pk_rtsk );
	if (ercd == D_OS_USER_E_OK && pk_rtsk.tskstat == D_OS_USER_TTS_DMT) {
		OS_User_Sta_Tsk( TID_PCIE_RC_CONTROLLER, 0 ) ;
	}
	else{
		ret = FJ_ERR_NG;
	}
	OS_User_Dly_Tsk(1);

	return ret;
}

FJ_ERR_CODE fj_pcie_rc_set_controller_flag( UINT32 flag )
{
	FJ_ERR_CODE 	ret = FJ_ERR_OK;
	OS_USER_ER		ercd = D_OS_USER_E_OK;

	switch(flag){
		case FJ_PCIE_RC_CONTROLLER_START1:
			ercd = OS_User_Set_Flg( FID_PCIE_RC_CONTROLLER, FLG_PCIE_RC_CONTROLLER_START1 );
		break;
		case FJ_PCIE_RC_CONTROLLER_START2:
			ercd = OS_User_Set_Flg( FID_PCIE_RC_CONTROLLER, FLG_PCIE_RC_CONTROLLER_START2 );
		break;
		case FJ_PCIE_RC_CONTROLLER_LU_INT1:
			ercd = OS_User_Set_Flg( FID_PCIE_RC_CONTROLLER, FLG_PCIE_RC_CONTROLLER_LU_INT1 );
		break;
		case FJ_PCIE_RC_CONTROLLER_LU_INT2_CH0:
			ercd = OS_User_Set_Flg( FID_PCIE_RC_CONTROLLER, FLG_PCIE_RC_CONTROLLER_LU_INT2_CH0 );
		break;
		case FJ_PCIE_RC_CONTROLLER_LU_INT2_CH1:
			ercd = OS_User_Set_Flg( FID_PCIE_RC_CONTROLLER, FLG_PCIE_RC_CONTROLLER_LU_INT2_CH1 );
		break;
		case FJ_PCIE_RC_CONTROLLER_END:
			ercd = OS_User_Set_Flg( FID_PCIE_RC_CONTROLLER, FLG_PCIE_RC_CONTROLLER_END );
		break;
		default:
			ret = FJ_ERR_NG;
		break;
	}

	if (ercd != D_OS_USER_E_OK) {
		BF_Debug_Print_Error(("fj_pcie_rc_set_controller_flag - Error! : ret = 0x%08X\n", ercd));
		ret = FJ_ERR_NG;
	}

	return ret;
}

FJ_ERR_CODE fj_pcie_rc_set_init_address( E_FJ_PCIE_CH ch, UINT32 addr )
{
	if(ch == E_FJ_PCIE_CH0){
		gBF_PCIE_RC_TargetAddr_Ch0 = addr;
	}
	else{
		gBF_PCIE_RC_TargetAddr_Ch1 = addr;
	}

	return FJ_ERR_OK;
}


