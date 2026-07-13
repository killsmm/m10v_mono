/**
 * @file		user_utility_pcie.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#include "user_utility_pcie.h"
#include "user.h"
#include "sdram_map_define.h"

#include "dd_timestamp.h"
#include "l1l2cache.h"
#include "fj_memory.h"

#include "dd_pcie_rc.h"
#include "dd_pcie_ep.h"
#include "fj_pcie.h"
#include "fj_pcie_ep.h"
#include "fj_pcie_rc.h"

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
#define M_USER_UTILITY_PCIE_MAKE_ALIGNMENT( val, align )	(((UINT32)(val) + (align -1)) & (~(align -1)))


/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
#define	D_USER_UTILITY_PCIE_TEST_TIMESTAMP_FREQUENCY	(0x2625A00)		// 40MHz (Timestamp frequency)
#define D_USER_UTILITY_PCIE_TEST_TIMESTAMP_K			(0.025)			// 1000000/PCIE_TIMESTAMP_FREQUENCY
#define D_USER_UTILITY_PCIE_TEST_TRANS_SIZE		(0x400000) // temporary
#define D_USER_UTILITY_PCIE_TEST_EP_ADDRESS		M_USER_UTILITY_PCIE_MAKE_ALIGNMENT((SDRAM_ADR_HOST_LCD_OSD_FRAME_DATA_LOAD_AREA), 65536) // temporary
#define D_USER_UTILITY_PCIE_TEST_RC_ADDRESS		M_USER_UTILITY_PCIE_MAKE_ALIGNMENT((D_USER_UTILITY_PCIE_TEST_EP_ADDRESS + D_USER_UTILITY_PCIE_TEST_TRANS_SIZE), 65536) // temporary
#define D_USER_UTILITY_PCIE_TEST_WK_ADDRESS		M_USER_UTILITY_PCIE_MAKE_ALIGNMENT((D_USER_UTILITY_PCIE_TEST_RC_ADDRESS + D_USER_UTILITY_PCIE_TEST_TRANS_SIZE), 65536) // temporary

#define D_USER_UTILITY_PCIE_EP_MODE			(0)
#define D_USER_UTILITY_PCIE_RC_MODE			(1)


/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
static INT32 g_User_Utility_Pcie_EP_DMA_TransferStatus = 0;
static INT32 g_User_Utility_Pcie_RC_DMA_TransferStatus = 0;


/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static VOID user_utility_pcie_ep_sam1_link_up_cb( UINT32 status );
static VOID user_utility_pcie_ep_sam1_int_dma_cb(INT32 status);
static VOID user_utility_pcie_rc_sam1_link_up_cb( UINT32 status );
static VOID user_utility_pcie_rc_sam1_int_dma_cb(INT32 status);
static VOID user_utility_pcie_timestamp_debug( VOID );


/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
VOID User_Utility_PCIE_HW_Init( UCHAR mode )
{
	Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PY2, D_DD_TOP_GPIO_DIR_IN);
	Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PY3, D_DD_TOP_GPIO_DIR_IN);
	
	Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PY0, D_DD_TOP_GPIO_FUNC_HWMODE);
	Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PY1, D_DD_TOP_GPIO_FUNC_HWMODE);
	Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PY4, D_DD_TOP_GPIO_FUNC_HWMODE);
	Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PY5, D_DD_TOP_GPIO_FUNC_HWMODE);
	Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PY6, D_DD_TOP_GPIO_FUNC_HWMODE);
	Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PY7, D_DD_TOP_GPIO_FUNC_HWMODE);

//	Dd_Top_Set_PERSEL3_PRT0SCK3(0);
//	Dd_Top_Set_PERSEL3_PRT1SOU3(0);
//	Dd_Top_Set_PERSEL3_PXW0WP12(0);
//	Dd_Top_Set_PERSEL3_PXW1SOU7(0);
//	Dd_Top_Set_PERSEL3_PXC0SCK7(0);
//	Dd_Top_Set_PERSEL3_PXC1WP15(0);

	if(mode == D_USER_UTILITY_PCIE_EP_MODE){
		UPRINTF("[USER_PCIE] EP_MODE ");
		
	}
	else if(mode == D_USER_UTILITY_PCIE_RC_MODE){
		Dd_Pcie_Rc_Ctrl_Detect_Card(0, NULL);
		UPRINTF("[USER_PCIE] RC_MODE ");
	}

	UPRINTF("EP_ADDR:0x%08x, RC_ADDR:0x%08x, WORK_ADDR:0x%08x, TRAN_SIZE:%d\n"
		, (UINT32)D_USER_UTILITY_PCIE_TEST_EP_ADDRESS
		, (UINT32)D_USER_UTILITY_PCIE_TEST_RC_ADDRESS
		, (UINT32)D_USER_UTILITY_PCIE_TEST_WK_ADDRESS
		, (UINT32)D_USER_UTILITY_PCIE_TEST_TRANS_SIZE);
}


/*==============================================================================*/
/* For PCIE EP mode																*/
/*==============================================================================*/
VOID User_Utility_PCIE_EP_DMATrans_Sam1( VOID )
{
	FJ_ERR_CODE ret=0;

	User_Utility_PCIE_HW_Init(D_USER_UTILITY_PCIE_EP_MODE);

	ret = FJ_Pcie_Ep_Start_Task();
	UPRINTF_ERR(ret);

	ret = FJ_Pcie_Ep_Set_Lu_Cb( user_utility_pcie_ep_sam1_link_up_cb );
	UPRINTF("FJ_Pcie_Ep_Set_Lu_Cb() completed. ret=0x%04X\n", ret);

	ret = FJ_Pcie_Ep_Set_Controller_Flag( FJ_PCIE_EP_CONTROLLER_START1 );
	UPRINTF("%s() completed. ercd=0x%04X\n", __func__, ret);
}

VOID User_Utility_PCIE_EP_DMATrans_DataSend_Sam1( VOID )
{
	FJ_ERR_CODE	ret;
	UINT32	trans_size = D_USER_UTILITY_PCIE_TEST_TRANS_SIZE;
	UINT32	addr_ep = D_USER_UTILITY_PCIE_TEST_EP_ADDRESS;
	UINT32	addr_rc = D_USER_UTILITY_PCIE_TEST_RC_ADDRESS;

	T_FJ_PCIE_DMA_CTRL 	dma_ctrl;

	UPRINTF("=====DMA Transfer Start. EP:0x%08X -> RC:0x%08X=====\n", addr_ep, addr_rc);
	// EP -> RC by EP
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, addr_ep, trans_size);
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, addr_rc, trans_size);
	// Write data to Root Complex from Endpoint
	g_User_Utility_Pcie_EP_DMA_TransferStatus = 0; // clear status
	dma_ctrl.ch 			= E_FJ_PCIE_CH0;
	dma_ctrl.src_addr_low 	= addr_ep;
	dma_ctrl.src_addr_upp 	= 0;
	dma_ctrl.dst_addr_low 	= addr_rc;
	dma_ctrl.dst_addr_upp 	= 0;
	dma_ctrl.trans_size 	= trans_size;
	dma_ctrl.direction		= E_FJ_PCIE_DMA_DIR_STOD;
	dma_ctrl.int_dma_callback = (PCIE_CALLBACK)user_utility_pcie_ep_sam1_int_dma_cb;
	user_utility_pcie_timestamp_debug();
	ret = FJ_Pcie_Ep_Dma_Transfer(&dma_ctrl);
	UPRINTF("FJ_Pcie_Ep_Dma_Transfer() completed. ret=0x%04X\n", ret);
	while(g_User_Utility_Pcie_EP_DMA_TransferStatus == 0){
		OS_User_Dly_Tsk(2);
	}
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, addr_ep, trans_size);
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, addr_rc, trans_size);
	UPRINTF("=====DMA Transfer End. status=0x%08X======\n", g_User_Utility_Pcie_EP_DMA_TransferStatus);
}

VOID User_Utility_PCIE_EP_DMATrans_DataGet_Sam1( VOID )
{
	FJ_ERR_CODE	ret;
	UINT32	trans_size = D_USER_UTILITY_PCIE_TEST_TRANS_SIZE;
	UINT32	addr_ep = D_USER_UTILITY_PCIE_TEST_WK_ADDRESS;
	UINT32	addr_rc = D_USER_UTILITY_PCIE_TEST_RC_ADDRESS;

	T_FJ_PCIE_DMA_CTRL 	dma_ctrl;

	UPRINTF("=====DMA Transfer Start. RC:0x%08X -> EP:0x%08X=====\n", addr_rc, addr_ep);
	// RC -> EP by EP
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, addr_ep, trans_size);
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, addr_rc, trans_size);
	// Read data from Root Complex to Endpoint
	g_User_Utility_Pcie_EP_DMA_TransferStatus = 0; // clear status
	dma_ctrl.ch 			= E_FJ_PCIE_CH0;
	dma_ctrl.src_addr_low 	= addr_rc;
	dma_ctrl.src_addr_upp 	= 0;
	dma_ctrl.dst_addr_low 	= addr_ep;
	dma_ctrl.dst_addr_upp 	= 0;
	dma_ctrl.trans_size 	= trans_size;
	dma_ctrl.direction		= E_FJ_PCIE_DMA_DIR_DTOS;
	dma_ctrl.int_dma_callback = (PCIE_CALLBACK)user_utility_pcie_ep_sam1_int_dma_cb;
	user_utility_pcie_timestamp_debug();
	ret = FJ_Pcie_Ep_Dma_Transfer(&dma_ctrl);
	UPRINTF("FJ_Pcie_Ep_Dma_Transfer() completed. ret=0x%04X\n", ret);
	while(g_User_Utility_Pcie_EP_DMA_TransferStatus == 0){
		OS_User_Dly_Tsk(2);
	}
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, addr_ep, trans_size);
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, addr_rc, trans_size);
	UPRINTF("=====DMA Transfer End. status=0x%08X======\n", g_User_Utility_Pcie_EP_DMA_TransferStatus);
}

/*==============================================================================*/
/* For PCIE RC mode																*/
/*==============================================================================*/
VOID User_Utility_PCIE_RC_DMATrans_Sam1( VOID )
{
	FJ_ERR_CODE ret=0;

	User_Utility_PCIE_HW_Init(D_USER_UTILITY_PCIE_RC_MODE);

	ret = FJ_Pcie_Rc_Start_Task();
	UPRINTF_ERR(ret);

	ret = FJ_Pcie_Rc_Set_Lu_Cb( E_FJ_PCIE_CH0, user_utility_pcie_rc_sam1_link_up_cb );
	UPRINTF("FJ_Pcie_Rc_Set_Lu_Cb() completed. ret=0x%04X\n", ret);

	ret = FJ_Pcie_Rc_Set_Controller_Flag( FJ_PCIE_RC_CONTROLLER_START1 );
	UPRINTF("%s() completed. ercd=0x%04X\n", __func__, ret);
}

VOID User_Utility_PCIE_RC_DMATrans_Config_Sam1( VOID )
{
	FJ_ERR_CODE		ret;
	UINT32			data;
	USHORT			offset;
	BOOL 			pcie0_aspm, pcie1_aspm;
	
	E_FJ_PCIE_CH	ch = E_FJ_PCIE_CH0;
	UINT32			addr_ep = D_USER_UTILITY_PCIE_TEST_EP_ADDRESS;
	
	E_FJ_PCIE_ST_LTSSM 		st_ltssm;
	E_FJ_PCIE_PMST 			pmst;
	E_FJ_PCIE_LINK_SPEED 	link_speed;
	E_FJ_PCIE_MPS			mps;
	
	ch = E_FJ_PCIE_CH0;
	
	// Argument of cfgrd, ch info of Root complex and offset of EP register
	// DEVICE_ID_VENDOR_ID_REG  Offset=0x4
	offset = 0x0004;
	ret = FJ_Pcie_Rc_Config_Read(ch, offset, &data);
	UPRINTF("offset=0x%04X data=0x%08X\n", offset, data);
	UPRINTF("FJ_Pcie_Rc_Config_Read() completed. ret=0x%04X\n", ret);
	st_ltssm = FJ_Pcie_Rc_Get_St_Ltssm(ch);
	UPRINTF("FJ_Pcie_Rc_Get_St_Ltssm() completed. st_ltssm=0x%X\n", st_ltssm);
	pmst = FJ_Pcie_Rc_Get_Pmst(ch);
	UPRINTF("FJ_Pcie_Rc_Get_Pmst() completed. pmst=0x%X\n", pmst);
	ret = FJ_Pcie_Rc_Get_Current_Link_Speed(ch, &link_speed);
	UPRINTF("Current Link Speed = 0x%08X\n", link_speed);
	UPRINTF("FJ_Pcie_Rc_Get_Current_Link_Speed() completed. ret=0x%04X\n", ret);
	ret = FJ_Pcie_Rc_Get_Mps(ch, &mps);
	if(ret == FJ_ERR_OK){
		UPRINTF("Max Payload Size = %d\n", mps);
	}
	UPRINTF("FJ_Pcie_Rc_Get_Mps() completed. ret=0x%04X\n", ret);
	ret = FJ_Pcie_Rc_Get_St_Aspm(&pcie0_aspm, &pcie1_aspm);
	UPRINTF("PCIe0 ASPM = %d\n", pcie0_aspm);
	UPRINTF("PCIe1 ASPM = %d\n", pcie1_aspm);
	UPRINTF("Fj_Pcie_Rc_Get_St_Aspm() completed. ret=0x%04X\n", ret);

	// Argument of cfgrd, ch info of Root complex and offset of EP register
	// BAR0_REG / BAR0_MASK  Offset=0x10
	offset = 0x10;
	ret = FJ_Pcie_Rc_Config_Read(ch, offset, &data);
	UPRINTF("offset=0x%04X data=0x%08X\n", offset, data);
	UPRINTF("FJ_Pcie_Rc_Config_Read() completed. ret=0x%04X\n", ret);
	data = addr_ep;
	ret = FJ_Pcie_Rc_Config_Write(ch, offset, data);
	UPRINTF("FJ_Pcie_Rc_Config_Write( offset=0x%04X data=0x%08X) completed. ret=0x%04X\n", offset, data, ret);
	ret = FJ_Pcie_Rc_Config_Read(ch, offset, &data);
	UPRINTF("offset=0x%04X data=0x%08X\n", offset, data);
	UPRINTF("FJ_Pcie_Rc_Config_Read() completed. ret=0x%04X\n", ret);

	// Argument of cfgrd, ch info of Root complex and offset of EP register
	// IATU_VIEWPORT_OFF  Offset=0x900
	offset = 0x900;
	ret = FJ_Pcie_Rc_Config_Read(ch, offset, &data);
	UPRINTF("offset=0x%04X data=0x%08X\n", offset, data);
	UPRINTF("FJ_Pcie_Rc_Config_Read() completed. ret=0x%04X\n", ret);
	data = 0x80000000;
	ret = FJ_Pcie_Rc_Config_Write(ch, offset, data);
	UPRINTF("FJ_Pcie_Rc_Config_Write( offset=0x%04X data=0x%08X) completed. ret=0x%04X\n", offset, data, ret);
	ret = FJ_Pcie_Rc_Config_Read(ch, offset, &data);
	UPRINTF("offset=0x%04X data=0x%08X\n", offset, data);
	UPRINTF("FJ_Pcie_Rc_Config_Read() completed. ret=0x%04X\n", ret);

	// Argument of cfgrd, ch info of Root complex and offset of EP register
	// IATU_REGION_CTRL   Offset=0x908
	offset = 0x908;
	ret = FJ_Pcie_Rc_Config_Read(ch, offset, &data);
	UPRINTF("offset=0x%04X data=0x%08X\n", offset, data);
	UPRINTF("FJ_Pcie_Rc_Config_Read() completed. ret=0x%04X\n", ret);
	data = 0xC0000000;
	ret = FJ_Pcie_Rc_Config_Write(ch, offset, data);
	UPRINTF("FJ_Pcie_Rc_Config_Write( offset=0x%04X data=0x%08X) completed. ret=0x%04X\n", offset, data, ret);
	ret = FJ_Pcie_Rc_Config_Read(ch, offset, &data);
	UPRINTF("offset=0x%04X data=0x%08X\n", offset, data);
	UPRINTF("FJ_Pcie_Rc_Config_Read() completed. ret=0x%04X\n", ret);

	// Argument of cfgrd, ch info of Root complex and offset of EP register
	// IATU_LWR_TARGET_ADDR_OFF_INBOND  Offset=0x918
	offset = 0x918;
	ret = FJ_Pcie_Rc_Config_Read(ch, offset, &data);
	UPRINTF("offset=0x%04X data=0x%08X\n", offset, data);
	UPRINTF("FJ_Pcie_Rc_Config_Read() completed. ret=0x%04X\n", ret);
	data = addr_ep;
	ret = FJ_Pcie_Rc_Config_Write(ch, offset, data);
	UPRINTF("offset=0x%04X data=0x%08X\n", offset, data);
	UPRINTF("FJ_Pcie_Rc_Config_Write() completed. ret=0x%04X\n", ret);
	ret = FJ_Pcie_Rc_Config_Read(ch, offset, &data);
	UPRINTF("offset=0x%04X data=0x%08X\n", offset, data);
	UPRINTF("FJ_Pcie_Rc_Config_Read() completed. ret=0x%04X\n", ret);
}

VOID User_Utility_PCIE_RC_DMATrans_DataSend_Sam1( VOID )
{
	FJ_ERR_CODE	ret;
	UINT32	trans_size = D_USER_UTILITY_PCIE_TEST_TRANS_SIZE;
	UINT32	addr_rc = D_USER_UTILITY_PCIE_TEST_RC_ADDRESS;
	UINT32	addr_ep = D_USER_UTILITY_PCIE_TEST_EP_ADDRESS;

	T_FJ_PCIE_DMA_CTRL 	dma_ctrl;

	UPRINTF("=====DMA Transfer Start. RC:0x%08X -> EP:0x%08X=====\n", addr_rc, addr_ep);
	// RC -> EP by RC
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, addr_rc, trans_size);
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, addr_ep, trans_size);
	// Write data from Root Complex to Endpoint
	g_User_Utility_Pcie_RC_DMA_TransferStatus = 0; // clear status
	dma_ctrl.ch 			= E_FJ_PCIE_CH0;
	dma_ctrl.src_addr_low	= addr_rc;
	dma_ctrl.src_addr_upp	= 0;
	dma_ctrl.dst_addr_low	= addr_ep;
	dma_ctrl.dst_addr_upp	= 0;
	dma_ctrl.trans_size 	= trans_size;
	dma_ctrl.direction		= E_FJ_PCIE_DMA_DIR_STOD;
	dma_ctrl.int_dma_callback = (PCIE_CALLBACK)user_utility_pcie_rc_sam1_int_dma_cb;
	user_utility_pcie_timestamp_debug();
	ret = FJ_Pcie_Rc_Dma_Transfer(&dma_ctrl);
	UPRINTF("FJ_Pcie_Rc_Dma_Transfer() completed. ret=0x%04X\n", ret);
	while(g_User_Utility_Pcie_RC_DMA_TransferStatus == 0){
		OS_User_Dly_Tsk(2);
	}
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, addr_rc, trans_size);
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, addr_ep, trans_size);
	UPRINTF("=====DMA Transfer End. status=0x%08X======\n", g_User_Utility_Pcie_RC_DMA_TransferStatus);
}

VOID User_Utility_PCIE_RC_DMATrans_DataGet_Sam1( VOID )
{
	FJ_ERR_CODE	ret;
	UINT32	trans_size = 256;
	UINT32	addr_ep = D_USER_UTILITY_PCIE_TEST_EP_ADDRESS;
	UINT32	addr_rc = D_USER_UTILITY_PCIE_TEST_WK_ADDRESS;

	T_FJ_PCIE_DMA_CTRL 	dma_ctrl;

	UPRINTF("=====DMA Transfer Start. EP:0x%08X -> RC:0x%08X=====\n", addr_ep, addr_rc);
	// EP -> RC by RC
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, addr_rc, trans_size);
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, addr_ep, trans_size);
	// Read data to Root Complex from Endpoint
	g_User_Utility_Pcie_RC_DMA_TransferStatus = 0; // clear status
	dma_ctrl.ch 			= E_FJ_PCIE_CH0;
	dma_ctrl.src_addr_low 	= addr_ep;
	dma_ctrl.src_addr_upp 	= 0;
	dma_ctrl.dst_addr_low 	= addr_rc;
	dma_ctrl.dst_addr_upp 	= 0;
	dma_ctrl.trans_size 	= trans_size;
	dma_ctrl.direction		= E_FJ_PCIE_DMA_DIR_DTOS;
	dma_ctrl.int_dma_callback = (PCIE_CALLBACK)user_utility_pcie_rc_sam1_int_dma_cb;
	user_utility_pcie_timestamp_debug();
	ret = FJ_Pcie_Rc_Dma_Transfer(&dma_ctrl);
	UPRINTF("FJ_Pcie_Rc_Dma_Transfer() completed. ret=0x%04X\n", ret);
	while(g_User_Utility_Pcie_RC_DMA_TransferStatus == 0){
		OS_User_Dly_Tsk(2);
	}
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, addr_rc, trans_size);
	FJ_MEMORY_Dcache_Addr(FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, addr_ep, trans_size);
	UPRINTF("=====DMA Transfer End. status=0x%08X======\n", g_User_Utility_Pcie_RC_DMA_TransferStatus);
}

/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
static VOID user_utility_pcie_ep_sam1_link_up_cb( UINT32 status )
{
	FJ_ERR_CODE ret=0;

	FJ_Pcie_Ep_Set_Init_Address(D_USER_UTILITY_PCIE_TEST_EP_ADDRESS);
	ret = FJ_Pcie_Ep_Set_Controller_Flag( FJ_PCIE_EP_CONTROLLER_LU_INT1 );
	UPRINTF("%s() completed. ercd=0x%04X\n", __func__, ret);
}

static VOID user_utility_pcie_ep_sam1_int_dma_cb(INT32 status)
{
	UINT64	timestamp_counter;
	ULONG	transfer_time;

	// Transfer rate measurement [Start]
	Dd_TIMESTAMP_Get_Counter(&timestamp_counter);
	transfer_time = (ULONG)(timestamp_counter * D_USER_UTILITY_PCIE_TEST_TIMESTAMP_K);
	UPRINTF("[PCIe EP] DMA transfer time=%ld[us]\n", transfer_time);
	Dd_TIMESTAMP_Stop();
	Dd_TIMESTAMP_Close();
	// Transfer rate measurement [End]

	g_User_Utility_Pcie_EP_DMA_TransferStatus = status;
	UPRINTF("%s() called. Status=0x%08X\n", __func__, status);
}

static VOID user_utility_pcie_rc_sam1_link_up_cb( UINT32 status )
{
	FJ_ERR_CODE ret=0;

	FJ_Pcie_Rc_Set_Init_Address(E_FJ_PCIE_CH0, D_USER_UTILITY_PCIE_TEST_EP_ADDRESS);
	ret = FJ_Pcie_Rc_Set_Controller_Flag( FJ_PCIE_RC_CONTROLLER_LU_INT1 );
	UPRINTF("%s() completed. ercd=0x%04X\n", __func__, ret);
}

static VOID user_utility_pcie_rc_sam1_int_dma_cb(INT32 status)
{
	UINT64	timestamp_counter;
	ULONG	transfer_time;

	// Transfer rate measurement [Start]
	Dd_TIMESTAMP_Get_Counter(&timestamp_counter);
	transfer_time = (ULONG)(timestamp_counter * D_USER_UTILITY_PCIE_TEST_TIMESTAMP_K);
	UPRINTF("[PCIe RC] DMA transfer time=%ld[us]\n", transfer_time);
	Dd_TIMESTAMP_Stop();
	Dd_TIMESTAMP_Close();
	// Transfer rate measurement [End]

	g_User_Utility_Pcie_RC_DMA_TransferStatus = status;
	UPRINTF("%s() called. Status=0x%08X\n", __func__, status);
}

static VOID user_utility_pcie_timestamp_debug( VOID )
{
	T_DD_TIMESTAMP_CTRL timestamp_ctrl;
	
	// Transfer rate measurement [Start]
	timestamp_ctrl.hdbg 		= 0;			// has no effect
	timestamp_ctrl.counter		= 0;			// Counter
	timestamp_ctrl.frequency	= D_USER_UTILITY_PCIE_TEST_TIMESTAMP_FREQUENCY;
	Dd_TIMESTAMP_Init();
	if (Dd_TIMESTAMP_Open(D_DDIM_USER_SEM_WAIT_POL) != D_DDIM_OK) {
		UPRINTF("Dd_TIMESTAMP has already opened.\n");
		Dd_TIMESTAMP_Stop();
		Dd_TIMESTAMP_Close();
		if (Dd_TIMESTAMP_Open(D_DDIM_USER_SEM_WAIT_POL) != D_DDIM_OK) {
			UPRINTF("Dd_TIMESTAMP_Open Error.\n");
		}
	}
	Dd_TIMESTAMP_Ctrl(&timestamp_ctrl);
	Dd_TIMESTAMP_Start();
	// Transfer rate measurement [End]
}


