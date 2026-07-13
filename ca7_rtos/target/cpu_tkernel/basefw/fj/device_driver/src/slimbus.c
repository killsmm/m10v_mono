/**
 * @file	  : uart.c
 * @brief	  : Uart controller
 * @note	  : None
 * @attention : None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>\n
 */

//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "os_user_custom.h"
//#include "dd_arm.h"
//#include "dd_gic.h"
#include "dd_slimbus.h"
#include "debug.h"
//#include "l1l2cache.h"
//#include "fj_host.h"
//#include "sdram_map_common.h"
#include "../include/slimbus.h"
#include <string.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define D_SLIMBUS_DEBUG
#define	D_BF_SLIMBUS_RX_ADDR_MAX	16

/*----------------------------------------------------------------------*/
/* Callback Function Prototype                                          */
/*----------------------------------------------------------------------*/    
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	UINT32 addr[64];
	UINT32 size;
} T_BF_SLIMBUS_RX_ADDR;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static volatile UINT32	gBF_SLIMBUS_Rx_Msg_Addr_Top = 0;
static volatile T_BF_SLIMBUS_RX_ADDR	gBF_SLIMBUS_Rx_Msg_Addr[D_BF_SLIMBUS_RX_ADDR_MAX] = {{{0}, 0}};
static volatile UINT32	gBF_SLIMBUS_Rx_Data_Addr_Top = 0;
static volatile T_BF_SLIMBUS_RX_ADDR	gBF_SLIMBUS_Rx_Data_Addr[D_BF_SLIMBUS_RX_ADDR_MAX] = {{{0}, 0}};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
 * @brief 		: SLIMBUS control task routine
 * @param		: i:stacd
 * @return		: None
 * @note		: None
 * @attention	: None
*/
VOID BF_Slimbus_Controller(INT32 stacd)
{
	UINT32	size = 0;
	UINT32	rx_msg[64];
	UCHAR	loop, i;
	UCHAR	ch = FJ_SLIMBUS_CH0;
	OS_USER_ER		ercd;
	OS_USER_FLGPTN	flg = 0;
	OS_USER_FLGPTN	flgptn = 0;

	BF_Debug_Print_Information(("BF_Slimbus_Controller task start\n"));
	flg  = FLG_SLIMBUS_CONTROLLER_MANAGER_CH0;
	flg |= FLG_SLIMBUS_CONTROLLER_DATAPORT_CH0;
	flg |= FLG_SLIMBUS_CONTROLLER_DMA_CH0;
	flg |= FLG_SLIMBUS_CONTROLLER_MANAGER_CH1;
	flg |= FLG_SLIMBUS_CONTROLLER_DATAPORT_CH1;
	flg |= FLG_SLIMBUS_CONTROLLER_DMA_CH1;
	flg |= FLG_SLIMBUS_CONTROLLER_MANAGER_CH2;
	flg |= FLG_SLIMBUS_CONTROLLER_DATAPORT_CH2;
	flg |= FLG_SLIMBUS_CONTROLLER_DMA_CH2;
	flg |= FLG_SLIMBUS_CONTROLLER_MANAGER_CH3;
	flg |= FLG_SLIMBUS_CONTROLLER_DATAPORT_CH3;
	flg |= FLG_SLIMBUS_CONTROLLER_DMA_CH3;
	flg |= FLG_SLIMBUS_CONTROLLER_PORT_INT;
	flg |= FLG_SLIMBUS_CONTROLLER_MCH_INT;
	flg |= FLG_SLIMBUS_CONTROLLER_RCFG_INT;
	flg |= FLG_SLIMBUS_CONTROLLER_SYNC_LOST;
	flg |= FLG_SLIMBUS_CONTROLLER_TX_ERR;
	flg |= FLG_SLIMBUS_CONTROLLER_TX_INT;
	flg |= FLG_SLIMBUS_CONTROLLER_RX_INT;
	ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~flg );
	for(;;){
#ifdef D_SLIMBUS_DEBUG
		BF_Debug_Print_Information(("BF_Slimbus_Controller task loop start wai=0x%08x\n", flg));
#endif
		ercd = OS_User_Wai_Flg( FID_SLIMBUS_CONTROLLER, flg, D_OS_USER_TWF_ORW, &flgptn);
		
		if( ercd != D_OS_USER_E_OK ) {
			BF_Debug_Print_Error(("[BF_Slimbus_Controller]wai_flg error(%d) L=%d\n", ercd, __LINE__));
#ifdef D_SLIMBUS_DEBUG
			OS_User_Dly_Tsk(1);
#endif
		}
		else {
#ifdef D_SLIMBUS_DEBUG
			BF_Debug_Print_Information(("BF_Slimbus_Controller FLG=0x%08x\n", flgptn));
#endif
			if (flgptn & FLG_SLIMBUS_CONTROLLER_MANAGER_CH0) {
				ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~FLG_SLIMBUS_CONTROLLER_MANAGER_CH0 );
				ch = FJ_SLIMBUS_CH0;
			}
			else if (flgptn & FLG_SLIMBUS_CONTROLLER_MANAGER_CH1) {
				ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~FLG_SLIMBUS_CONTROLLER_MANAGER_CH1 );
				ch = FJ_SLIMBUS_CH1;
			}
			else if (flgptn & FLG_SLIMBUS_CONTROLLER_MANAGER_CH2) {
				ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~FLG_SLIMBUS_CONTROLLER_MANAGER_CH2 );
				ch = FJ_SLIMBUS_CH2;
			}
			else if (flgptn & FLG_SLIMBUS_CONTROLLER_MANAGER_CH3) {
				ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~FLG_SLIMBUS_CONTROLLER_MANAGER_CH3 );
				ch = FJ_SLIMBUS_CH3;
			}
			{
				if (flgptn & FLG_SLIMBUS_CONTROLLER_PORT_INT) {
					ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~FLG_SLIMBUS_CONTROLLER_PORT_INT );
					BF_Debug_Print_Information(("\t PORT_INT ch=%d\n", ch));
				}
				if (flgptn & FLG_SLIMBUS_CONTROLLER_MCH_INT) {
					ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~FLG_SLIMBUS_CONTROLLER_MCH_INT );
					BF_Debug_Print_Information(("\t MCH_INT ch=%d\n", ch));
				}
				if (flgptn & FLG_SLIMBUS_CONTROLLER_RCFG_INT) {
					ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~FLG_SLIMBUS_CONTROLLER_RCFG_INT );
					BF_Debug_Print_Information(("\t RCFG_INT ch=%d\n", ch));
				}
				if (flgptn & FLG_SLIMBUS_CONTROLLER_SYNC_LOST) {
					ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~FLG_SLIMBUS_CONTROLLER_SYNC_LOST );
					BF_Debug_Print_Information(("\t SYNC_LOST ch=%d\n", ch));
				}
				if (flgptn & FLG_SLIMBUS_CONTROLLER_TX_ERR) {
					BF_Debug_Print_Information(("\t TX_ERR ch=%d\n", ch));
					ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~FLG_SLIMBUS_CONTROLLER_TX_ERR );
				}
				if (flgptn & FLG_SLIMBUS_CONTROLLER_TX_INT) {
					BF_Debug_Print_Information(("\t TX_INT ch=%d\n", ch));
					ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~FLG_SLIMBUS_CONTROLLER_TX_INT );
				}
				if (flgptn & FLG_SLIMBUS_CONTROLLER_RX_INT) {
					ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~FLG_SLIMBUS_CONTROLLER_RX_INT );
					BF_Debug_Print_Information(("\t RX_INT ch=%d\n", ch));

#ifdef D_SLIMBUS_DEBUG
				BF_Debug_Print_Information(("BF_Slimbus_Controller flg=0x%08x\n", flgptn));
#endif

					while (1) {
						FJ_Slimbus_Get_Read_Msg_Area_Addr( ch, rx_msg, &size );

						if (size) {
							loop = (size >> 2);
							if(size & 0x3){
								loop++;
							}
							BF_Debug_Print_Information(("-- Read Message size:%d(byte) --\n", size));
							for(i=0;i<loop;i++){
								BF_Debug_Print_Information(("MC_FIFO[%d]=0x%08X\n", i, rx_msg[i]));
							}
						}
						else {
							break;
						}
					}
				}
			}
			if (flgptn &  FLG_SLIMBUS_CONTROLLER_DATAPORT_CH0) {
				ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~FLG_SLIMBUS_CONTROLLER_DATAPORT_CH0 );
				BF_Debug_Print_Information(("FLG_SLIMBUS_CONTROLLER_DATAPORT_CH0 called. ch=0\n"));
			}
			else if (flgptn &  FLG_SLIMBUS_CONTROLLER_DATAPORT_CH1) {
				ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~FLG_SLIMBUS_CONTROLLER_DATAPORT_CH1 );
				BF_Debug_Print_Information(("FLG_SLIMBUS_CONTROLLER_DATAPORT_CH1 called. ch=1\n"));
			}
			else if (flgptn &  FLG_SLIMBUS_CONTROLLER_DATAPORT_CH2) {
				ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~FLG_SLIMBUS_CONTROLLER_DATAPORT_CH2 );
				BF_Debug_Print_Information(("FLG_SLIMBUS_CONTROLLER_DATAPORT_CH2 called. ch=2\n"));
			}
			else if (flgptn &  FLG_SLIMBUS_CONTROLLER_DATAPORT_CH3) {
				ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~FLG_SLIMBUS_CONTROLLER_DATAPORT_CH3 );
				BF_Debug_Print_Information(("FLG_SLIMBUS_CONTROLLER_DATAPORT_CH3 called. ch=3\n"));
			}
			if (flgptn &  FLG_SLIMBUS_CONTROLLER_DMA_CH0) {
				ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~FLG_SLIMBUS_CONTROLLER_DMA_CH0 );
				BF_Debug_Print_Information(("FLG_SLIMBUS_CONTROLLER_DMA_CH0 called. dma_ch=0\n"));
				FJ_Slimbus_Get_Read_Data_Area_Addr( FJ_SLIMBUS_CH0, rx_msg, &size );
				{
					int i;

					BF_Debug_Print_Information(("=====================\n"));
					for (i = 0; i < size; i ++) {
						BF_Debug_Print_Information(("rx_data[%d]=0x%08X\n", i, rx_msg[i]));
					}
					BF_Debug_Print_Information(("=====================\n"));
				}
			}
			else if (flgptn &  FLG_SLIMBUS_CONTROLLER_DMA_CH1) {
				ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~FLG_SLIMBUS_CONTROLLER_DMA_CH1 );
				BF_Debug_Print_Information(("FLG_SLIMBUS_CONTROLLER_DMA_CH1 called. dma_ch=1\n"));
				FJ_Slimbus_Get_Read_Data_Area_Addr( FJ_SLIMBUS_CH1, rx_msg, &size );
				{
					int i;

					BF_Debug_Print_Information(("=====================\n"));
					for (i = 0; i < size; i ++) {
						BF_Debug_Print_Information(("rx_data[%d]=0x%08X\n", i, rx_msg[i]));
					}
					BF_Debug_Print_Information(("=====================\n"));
				}
			}
			else if (flgptn &  FLG_SLIMBUS_CONTROLLER_DMA_CH2) {
				ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~FLG_SLIMBUS_CONTROLLER_DMA_CH2 );
				BF_Debug_Print_Information(("FLG_SLIMBUS_CONTROLLER_DMA_CH2 called. dma_ch=2\n"));
				FJ_Slimbus_Get_Read_Data_Area_Addr( FJ_SLIMBUS_CH2, rx_msg, &size );
				{
					int i;

					BF_Debug_Print_Information(("=====================\n"));
					for (i = 0; i < size; i ++) {
						BF_Debug_Print_Information(("rx_data[%d]=0x%08X\n", i, rx_msg[i]));
					}
					BF_Debug_Print_Information(("=====================\n"));
				}
			}
			else if (flgptn &  FLG_SLIMBUS_CONTROLLER_DMA_CH3) {
				ercd = OS_User_Clr_Flg( FID_SLIMBUS_CONTROLLER, ~FLG_SLIMBUS_CONTROLLER_DMA_CH3 );
				BF_Debug_Print_Information(("FLG_SLIMBUS_CONTROLLER_DMA_CH3 called. dma_ch=3\n"));
				FJ_Slimbus_Get_Read_Data_Area_Addr( FJ_SLIMBUS_CH3, rx_msg, &size );
				{
					int i;

					BF_Debug_Print_Information(("=====================\n"));
					for (i = 0; i < size; i ++) {
						BF_Debug_Print_Information(("rx_data[%d]=0x%08X\n", i, rx_msg[i]));
					}
					BF_Debug_Print_Information(("=====================\n"));
				}
			}
		}
#ifdef D_SLIMBUS_DEBUG
		BF_Debug_Print_Information(("BF_Slimbus_Controller task loop next\n"));
		OS_User_Dly_Tsk(1);
#endif
	}
	BF_Debug_Print_Information(("BF_Slimbus_Controller task end\n"));
	OS_User_Ext_Tsk();
}

// ***************************************************************************
//                    API
// ***************************************************************************
/**
 *	@brief		This function open SLIMBus Channel.
 */
FJ_ERR_CODE fj_slimbus_open( FJ_SLIMBUS_CH ch )
{
	INT32 ret;

	ret = Dd_Slimbus_Open( ch, D_DDIM_WAIT_END_TIME );

	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("fj_slimbus_open() - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		This function close SLIMBus Channel.
 */
FJ_ERR_CODE fj_slimbus_close( FJ_SLIMBUS_CH ch )
{
	INT32 ret;

	ret = Dd_Slimbus_Close( ch );

	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("fj_slimbus_close() - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		This function set SLIMBus Control.
 */
FJ_ERR_CODE fj_slimbus_ctrl( FJ_SLIMBUS_CTRL const* const slimbus_ctrl )
{
	INT32 ret;

	T_DD_SLIMBUS_CTRL ctrl;

	if( slimbus_ctrl == NULL ){
		BF_Debug_Print_Warning(("fj_slimbus_ctrl() - Input Parameter Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	ctrl.ch					 = (E_DD_SLIMBUS_CH)slimbus_ctrl->ch;
	ctrl.manager_mode		 = (E_DD_SLIMBUS_MANAGER_MODE)slimbus_ctrl->manager_mode;
	ctrl.fr_en				 = (E_DD_SLIMBUS_FR_EN)slimbus_ctrl->fr_en;
	ctrl.src_thr			 = slimbus_ctrl->src_thr;
	ctrl.sink_thr			 = slimbus_ctrl->sink_thr;
	ctrl.manager_int_cb		 = (VP_SLIMBUS_CALLBACK)slimbus_ctrl->manager_int_cb;
	ctrl.data_port_int_cb	 = (VP_SLIMBUS_CALLBACK)slimbus_ctrl->data_port_int_cb;

	ret = Dd_Slimbus_Ctrl( &ctrl );

	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("fj_slimbus_ctrl() - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		This function get SLIMBus Control on the source.
 */
FJ_ERR_CODE fj_slimbus_get_ctrl( FJ_SLIMBUS_CH ch, FJ_SLIMBUS_CTRL* const slimbus_ctrl )
{
	INT32 ret;
	T_DD_SLIMBUS_CTRL ctrl;

	if( slimbus_ctrl == NULL ){
		BF_Debug_Print_Warning(("fj_slimbus_get_ctrl() - Input Parameter Error!\n"));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	ret = Dd_Slimbus_Get_Ctrl( ch, &ctrl );
	slimbus_ctrl->ch				 = (FJ_SLIMBUS_CH)ctrl.ch;
	slimbus_ctrl->manager_mode		 = (FJ_SLIMBUS_MANAGER_MODE)ctrl.manager_mode;
	slimbus_ctrl->fr_en				 = (FJ_SLIMBUS_FR_EN)ctrl.fr_en;
	slimbus_ctrl->src_thr			 = ctrl.src_thr;
	slimbus_ctrl->sink_thr			 = ctrl.sink_thr;
	slimbus_ctrl->manager_int_cb	 = (SLIMBUS_CALLBACK)ctrl.manager_int_cb;
	slimbus_ctrl->data_port_int_cb	 = (SLIMBUS_CALLBACK)ctrl.data_port_int_cb;

	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("fj_slimbus_get_ctrl() - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		This function start SLIMBus Macro.
 */
FJ_ERR_CODE fj_slimbus_start( FJ_SLIMBUS_CH ch )
{
	INT32 ret;

	ret = Dd_Slimbus_Start( ch );

	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("fj_slimbus_start() - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		This function stop SLIMBus Macro.
 */
FJ_ERR_CODE fj_slimbus_stop( FJ_SLIMBUS_CH ch )
{
	INT32 ret;

	ret = Dd_Slimbus_Stop( ch );

	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("fj_slimbus_stop() - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		This function write message to TX_FIFO.
 */
FJ_ERR_CODE fj_slimbus_write_msg( FJ_SLIMBUS_CH ch, UINT32* msg, UINT32 size )
{
	INT32 ret;

	ret = Dd_Slimbus_Write_Msg( ch, msg, size );

	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("fj_slimbus_write_msg() - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		This function read message from RX_FIFO.
 */
FJ_ERR_CODE fj_slimbus_read_msg( FJ_SLIMBUS_CH ch, UINT32* msg, UINT32* size )
{
	INT32 ret;

	ret = Dd_Slimbus_Read_Msg( ch,  msg,  size );

	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("fj_slimbus_read_msg() - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		This function write data to P0_FIFO.
 */
FJ_ERR_CODE fj_slimbus_write_data( FJ_SLIMBUS_CH ch, UCHAR dma_ch, UINT32* data, UINT32 count, SLIMBUS_CALLBACK pcallback )
{
	INT32 ret;

	ret = Dd_Slimbus_Write_Data( ch, dma_ch, data, count, (VP_SLIMBUS_CALLBACK)pcallback );

	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("fj_slimbus_write_data() - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		This function read data from P0_FIFO.
 */
FJ_ERR_CODE fj_slimbus_read_data( FJ_SLIMBUS_CH ch, UCHAR dma_ch, UINT32* data, UINT32 count, SLIMBUS_CALLBACK pcallback )
{
	INT32 ret;

	ret = Dd_Slimbus_Read_Data( ch, dma_ch, data, count, (VP_SLIMBUS_CALLBACK)pcallback );

	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("fj_slimbus_read_data() - Error! : ret = 0x%08X\n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 *	@brief		This function sets SLIMBus data read area address.
 */
FJ_ERR_CODE fj_slimbus_set_read_msg_area_addr(FJ_SLIMBUS_CH ch, UINT32 addr, UINT32 size)
{
	UINT32 i = 0;
	T_BF_SLIMBUS_RX_ADDR* next = (T_BF_SLIMBUS_RX_ADDR*)&gBF_SLIMBUS_Rx_Msg_Addr[gBF_SLIMBUS_Rx_Msg_Addr_Top];

	while (next->size != 0) {
		i ++;
		if (i >= D_BF_SLIMBUS_RX_ADDR_MAX) {
			break;
		}
		next = (T_BF_SLIMBUS_RX_ADDR*)&gBF_SLIMBUS_Rx_Msg_Addr[i % D_BF_SLIMBUS_RX_ADDR_MAX];
	}
	if (i >= D_BF_SLIMBUS_RX_ADDR_MAX) {
		BF_Debug_Print_Warning(("fj_slimbus_set_read_msg_area_addr buffer insufficient - Error!\n"));
		return FJ_ERR_NG;
	}
	memcpy(next->addr, (VOID*)addr, sizeof(UINT32) * 64);
	next->size = size;

	return FJ_ERR_OK;
}

/**
 *	@brief		This function gets SLIMBus data read area address.
 */
VOID fj_slimbus_get_read_msg_area_addr(FJ_SLIMBUS_CH ch, UINT32* addr, UINT32* size)
{
	T_BF_SLIMBUS_RX_ADDR* top = (T_BF_SLIMBUS_RX_ADDR*)&gBF_SLIMBUS_Rx_Msg_Addr[gBF_SLIMBUS_Rx_Msg_Addr_Top];

	memcpy(addr, top->addr, sizeof(UINT32) * 64);
	*size = top->size;

	memset(top->addr, 0, sizeof(UINT32) * 64);
	top->size = 0;

	gBF_SLIMBUS_Rx_Msg_Addr_Top = ((gBF_SLIMBUS_Rx_Msg_Addr_Top + 1) % D_BF_SLIMBUS_RX_ADDR_MAX);
}
/**
 *	@brief		This function sets SLIMBus data read area address.
 */
FJ_ERR_CODE fj_slimbus_set_read_data_area_addr(FJ_SLIMBUS_CH ch, UINT32 addr, UINT32 size)
{
	UINT32 i = 0;
	T_BF_SLIMBUS_RX_ADDR* next = (T_BF_SLIMBUS_RX_ADDR*)&gBF_SLIMBUS_Rx_Data_Addr[gBF_SLIMBUS_Rx_Data_Addr_Top];

	while (next->size != 0) {
		i ++;
		if (i >= D_BF_SLIMBUS_RX_ADDR_MAX) {
			break;
		}
		next = (T_BF_SLIMBUS_RX_ADDR*)&gBF_SLIMBUS_Rx_Data_Addr[i % D_BF_SLIMBUS_RX_ADDR_MAX];
	}
	if (i >= D_BF_SLIMBUS_RX_ADDR_MAX) {
		BF_Debug_Print_Warning(("fj_slimbus_set_read_data_area_addr buffer insufficient - Error!\n"));
		return FJ_ERR_NG;
	}
	memcpy(next->addr, (VOID*)addr, sizeof(UINT32) * 64);
	next->size = size;

	return FJ_ERR_OK;
}

/**
 *	@brief		This function gets SLIMBus data read area address.
 */
VOID fj_slimbus_get_read_data_area_addr(FJ_SLIMBUS_CH ch, UINT32* addr, UINT32* size)
{
	T_BF_SLIMBUS_RX_ADDR* top = (T_BF_SLIMBUS_RX_ADDR*)&gBF_SLIMBUS_Rx_Data_Addr[gBF_SLIMBUS_Rx_Data_Addr_Top];

	memcpy(addr, top->addr, sizeof(UINT32) * 64);
	*size = top->size;

	memset(top->addr, 0, sizeof(UINT32) * 64);
	top->size = 0;

	gBF_SLIMBUS_Rx_Data_Addr_Top = ((gBF_SLIMBUS_Rx_Data_Addr_Top + 1) % D_BF_SLIMBUS_RX_ADDR_MAX);
}
