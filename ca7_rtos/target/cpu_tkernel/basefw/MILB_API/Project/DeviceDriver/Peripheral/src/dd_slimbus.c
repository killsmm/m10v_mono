/**
 * @file		dd_slimbus.c
 * @brief		SLIMbus Driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "_slimbus.h"
#include "dd_slimbus.h"
#include "ddim_user_custom.h"
#include "dd_top.h"
#include "dd_hdmac1.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_DD_SLIMBUS_CH_MAX_NUM					(4)
#define D_DD_SLIMBUS_MC_FIFO_MSG_MAX_SIZE		(64)
#define D_DD_SLIMBUS_P0_FIFO_DATA_MAX_NUM		(64)

#define D_DD_SLIMBUS_DMACA_IS_BASE				(D_DD_HDMAC1_IS_IDREQ_25)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef volatile struct io_slimbus	T_DD_SLIMBUS_REG;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static volatile VP_SLIMBUS_CALLBACK		gDD_SLIMBUS_Manager_Int_Cb[D_DD_SLIMBUS_CH_MAX_NUM] = {NULL, NULL, NULL, NULL};
static volatile VP_SLIMBUS_CALLBACK		gDD_SLIMBUS_Data_Port_Int_Cb[D_DD_SLIMBUS_CH_MAX_NUM] = {NULL, NULL, NULL, NULL};
static volatile VP_SLIMBUS_CALLBACK		gDD_SLIMBUS_DMA_Int_Cb[D_DD_SLIMBUS_CH_MAX_NUM] = {NULL, NULL, NULL, NULL};
static volatile UCHAR					gDD_SLIMBUS_Ch_Info[D_DD_HDMAC1_CH_NUM_MAX] = {0, 0, 0, 0, 0, 0, 0, 0};
static volatile UCHAR					gDD_SLIMBUS_DMA_Ch_Info[D_DD_SLIMBUS_CH_MAX_NUM] = {0, 0, 0, 0};
static volatile UCHAR					gDD_SLIMBUS_DMA_Wait[D_DD_SLIMBUS_CH_MAX_NUM] = {0, 0, 0, 0};
static volatile T_DD_SLIMBUS_CTRL		gDD_SLIMBUS_Ctrl[D_DD_SLIMBUS_CH_MAX_NUM] = {{0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
//#define D_DEBUG_PRINT_INFO_ON
#ifndef D_DEBUG_PRINT_INFO_ON
#define dd_slimbus_debug_print(arg)
#else
#define dd_slimbus_debug_print(arg)		{DDIM_User_Printf arg;}
#endif	// D_DEBUG_PRINT_INFO_ON

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Nothing Special
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
// Get SLIMBus Register Address
static T_DD_SLIMBUS_REG* dd_slimbus_get_register( E_DD_SLIMBUS_CH ch )
{
	T_DD_SLIMBUS_REG*	slimbus_reg;

	switch (ch) {
		case E_DD_SLIMBUS_CH0:		// for Audio.
			slimbus_reg = &IO_SLIMBUS0;
			break;
		case E_DD_SLIMBUS_CH1:		// for Audio.
			slimbus_reg = &IO_SLIMBUS1;
			break;
		case E_DD_SLIMBUS_CH2:		// for Peripheral.
			slimbus_reg = &IO_SLIMBUS2;
			break;
		case E_DD_SLIMBUS_CH3:		// for Peripheral.
			slimbus_reg = &IO_SLIMBUS3;
			break;
		default:
			slimbus_reg = NULL;
	}
	return slimbus_reg;
}

// Start DMA
static INT32 dd_slimbus_start_dma( UCHAR dma_ch, T_DD_HDMAC1_CTRL* const hdmac1_ctrl )
{
	INT32	ret;

	ret = Dd_HDMAC1_Ctrl_Common( dma_ch, hdmac1_ctrl );
	if(ret == D_DDIM_OK){
		ret = Dd_HDMAC1_Start_Async( dma_ch );
		if(ret != D_DDIM_OK){
			dd_slimbus_debug_print(("Dd_HDMAC1_Start_Async() error. ret=0x%x\n", ret));
		}
	}
	else{
		dd_slimbus_debug_print(("Dd_HDMAC1_Ctrl_Common() error. ret=0x%x\n", ret));
	}

	return ret;
}

// DMA interrupt callback
static VOID dd_slimbus_dma_int_callback( UCHAR dma_ch, USHORT* const status )
{
	INT32 result;
	UCHAR ch;

	ch = gDD_SLIMBUS_Ch_Info[dma_ch];
	if(*status == D_DD_HDMAC1_SS_NORMAL_END){
		result = D_DDIM_OK;
	}
	else{
		dd_slimbus_debug_print(("dd_slimbus_dma_int_callback status=0x%x\n.", *status));
		result = D_DD_SLIMBUS_DMA_SS_ERROR;
	}

	Dd_HDMAC1_Close(dma_ch);
	gDD_SLIMBUS_DMA_Wait[ch] = 0;

	if(gDD_SLIMBUS_DMA_Int_Cb[ch]){
		(*gDD_SLIMBUS_DMA_Int_Cb[ch])(ch, result);
	}
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Check SLIMBus register parameter.
static INT32 dd_slimbus_chk_reg_par( USHORT offset )
{
	// Word boundary check.
	if((offset % 4) != 0){
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}

	// Check on reserved address.
	if((offset >= 0x0014 && offset <= 0x0017) ||
	   (offset >= 0x0030 && offset <= 0x0037) ||
	   (offset >= 0x0084 && offset <= 0x00BF) ||
	   (offset >= 0x00C4 && offset <= 0x00FF) ||
	   (offset >= 0x0108 && offset <= 0x0FFF) ||
	   (offset >= 0x1040)){
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}

	return D_DDIM_OK;
}
//---------------------------- colabo section ----------------------------
// Nothing Special
#endif

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
//---------------------------- driver section ----------------------------
// Open SLIMBus Channel
INT32 Dd_Slimbus_Open( E_DD_SLIMBUS_CH ch, INT32 tmout )
{
	DDIM_USER_ER	ercd;

#ifdef CO_PARAM_CHECK
	if( tmout < D_DDIM_USER_SEM_WAIT_FEVR ) {
		Ddim_Assertion(("SLIMBus input param error. [tmout] = %x\n", tmout));
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}
#endif	// CO_PARAM_CHECK

	if( tmout == D_DDIM_USER_SEM_WAIT_POL ) {
		ercd = DDIM_User_Pol_Sem( SID_DD_SLIMBUS(ch) );
	}
	else{
		ercd = DDIM_User_Twai_Sem( SID_DD_SLIMBUS(ch), (DDIM_USER_TMO)tmout );
	}

	if( ercd != D_DDIM_USER_E_OK ) {
		if( D_DDIM_USER_E_TMOUT == ercd ) {
			return D_DD_SLIMBUS_SEM_TIMEOUT;
		}
		else {
			return D_DD_SLIMBUS_SEM_NG;
		}
	}
	return D_DDIM_OK;
}

// Close SLIMBus Channel
INT32 Dd_Slimbus_Close( E_DD_SLIMBUS_CH ch )
{
	DDIM_USER_ER	ercd;

	ercd = DDIM_User_Sig_Sem( SID_DD_SLIMBUS(ch) );				// sig_sem()

	if( D_DDIM_USER_E_OK != ercd ) {
		return D_DD_SLIMBUS_SEM_NG;
	}
	return D_DDIM_OK;
}

// Set SLIMBus Control
INT32 Dd_Slimbus_Ctrl( T_DD_SLIMBUS_CTRL const* const slimbus_ctrl )
{
	T_DD_SLIMBUS_REG*	slimbus_reg;

#ifdef CO_PARAM_CHECK
	if(slimbus_ctrl == NULL){
		Ddim_Assertion(("SLIMBus input param error. [slimbus_ctrl] = NULL\n"));
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}
	if(slimbus_ctrl->manager_int_cb == NULL){
		Ddim_Assertion(("SLIMBus input param error. [manager_int_cb] = NULL\n"));
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}
	if(slimbus_ctrl->data_port_int_cb == NULL){
		Ddim_Assertion(("SLIMBus input param error. [data_port_int_cb] = NULL\n"));
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}
#endif

	slimbus_reg = dd_slimbus_get_register(slimbus_ctrl->ch);

	switch(slimbus_ctrl->ch){
		case E_DD_SLIMBUS_CH0:
		case E_DD_SLIMBUS_CH1:
			if(slimbus_ctrl->ch == E_DD_SLIMBUS_CH0){
				Dd_Top_Set_CLKSTOP2_SLIMB00CK(2);
			}
			else{
				Dd_Top_Set_CLKSTOP2_SLIMB01CK(2);
			}

			slimbus_reg->CONFIG_PR_TP.word = 0x07FF00FE;	// Configuration – Generic device
															// [26:24]TP_SUPP
															// [23:0]PR_SUPP : 24.576MHz
			slimbus_reg->CONFIG_FR.word = 0x00030001;		// Configuration – Framer device
															// [17:16]QUALITY=3
															// [15:0]RF_SUPP=1(24.576MHz)
			break;
		case E_DD_SLIMBUS_CH2:
		case E_DD_SLIMBUS_CH3:
			if(slimbus_ctrl->ch == E_DD_SLIMBUS_CH2){
				Dd_Top_Set_CLKSTOP2_SLIMB10CK(2);
			}
			else{
				Dd_Top_Set_CLKSTOP2_SLIMB11CK(2);
			}

			slimbus_reg->CONFIG_PR_TP.word = 0x073F007E;	// Configuration – Generic device
															// [26:24]TP_SUPP
															// [23:0]PR_SUPP : 28.8MHz
			slimbus_reg->CONFIG_FR.word = 0x00030000;		// Configuration – Framer device
															// [17:16]QUALITY=3
															// [15:0]RF_SUPP=0(28.8MHz Not Indicated)
			break;
		default:
			return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}

	gDD_SLIMBUS_Ctrl[slimbus_ctrl->ch] = *slimbus_ctrl;

	slimbus_reg->CONFIG_MODE.word = 0x00000811;	// Configuration – Mode
												// [11:8]RETRY_LMT=8
												// [4]DEV_EN=1
												// [0]ENABLE=1

	slimbus_reg->CONFIG_MODE.bit.MANAGER_MODE = slimbus_ctrl->manager_mode;

	slimbus_reg->CONFIG_MODE.bit.FR_EN = slimbus_ctrl->fr_en;

	if(slimbus_reg->CONFIG_MODE.bit.MANAGER_MODE == E_DD_SLIMBUS_MANAGER_MODE_ENABLE){
		slimbus_reg->CONFIG_EA.bit.INSTANCE_VAL = 0;
	}
	else{
		slimbus_reg->CONFIG_EA.bit.INSTANCE_VAL = 1;
	}

	slimbus_reg->CONFIG_DPORT.word = 0x00000600;	// Reset value

	slimbus_reg->CONFIG_THR.bit.SRC_THR  = slimbus_ctrl->src_thr;	// source threshold
	slimbus_reg->CONFIG_THR.bit.SINK_THR = slimbus_ctrl->sink_thr;	// sink threshold

	slimbus_reg->INT.word = 0x0000007E;		// Interrupt Clear

	slimbus_reg->INT_EN.word = 0x0000001F;	// Interrupt Enable
											// [4]SYNC_LOST_EN=1
											// [3]TX_ERR_EN=1
											// [2]TX_INT_EN=1
											// [1]RX_INT_EN=1
											// [0]IN_EN=1

	slimbus_reg->P0_INT.word = 0x0000003F;		// Interrupt Clear

	slimbus_reg->P0_INT_EN.word = 0x000000F7;	// Only DMA_INT_EN is 0.

	gDD_SLIMBUS_Manager_Int_Cb[slimbus_ctrl->ch] = slimbus_ctrl->manager_int_cb;
	gDD_SLIMBUS_Data_Port_Int_Cb[slimbus_ctrl->ch] = slimbus_ctrl->data_port_int_cb;

	return D_DDIM_OK;
}

// Get SLIMBus Control
INT32 Dd_Slimbus_Get_Ctrl( E_DD_SLIMBUS_CH ch, T_DD_SLIMBUS_CTRL* const slimbus_ctrl )
{
	if(slimbus_ctrl == NULL){
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}

	*slimbus_ctrl = gDD_SLIMBUS_Ctrl[ch];

	return D_DDIM_OK;
}

// Start SLIMBus Macro
INT32 Dd_Slimbus_Start( E_DD_SLIMBUS_CH ch )
{
	T_DD_SLIMBUS_REG* slimbus_reg;

	slimbus_reg = dd_slimbus_get_register(ch);

	if( slimbus_reg->COMMAND.bit.CFG_STROBE == 0 ){
		slimbus_reg->COMMAND.bit.CFG_STROBE = 1;
	}
	else{	// previous reconfiguration
		return D_DD_SLIMBUS_STATUS_ABNORMAL;
	}

	return D_DDIM_OK;
}

// Stop SLIMBus Macro
INT32 Dd_Slimbus_Stop( E_DD_SLIMBUS_CH ch )
{
	T_DD_SLIMBUS_REG* slimbus_reg;

	slimbus_reg = dd_slimbus_get_register(ch);

	if( slimbus_reg->COMMAND.bit.CFG_STROBE == 1 ){
		slimbus_reg->COMMAND.bit.CFG_STROBE_CLR = 1;
	}

    // Disable all interrupts
	slimbus_reg->P0_INT_EN.word = 0x00000040;
	slimbus_reg->INT_EN.bit.IN_EN = 0;

    // Disable component
	slimbus_reg->CONFIG_MODE.bit.ENABLE = 0;
	slimbus_reg->COMMAND.bit.CFG_STROBE = 1;

	if(gDD_SLIMBUS_DMA_Wait[ch] == 1){
		Dd_HDMAC1_Close(gDD_SLIMBUS_DMA_Ch_Info[ch]);
		gDD_SLIMBUS_DMA_Wait[ch] = 0;
	}

	return D_DDIM_OK;
}

// Interrupt Handler of SLIMBus.
VOID Dd_Slimbus_Int_Handler( E_DD_SLIMBUS_CH ch )
{
	T_DD_SLIMBUS_REG* slimbus_reg;
	UINT32	int_reg;
	UINT32	p0_int_reg;
	VP_SLIMBUS_CALLBACK	int_cb;
	VP_SLIMBUS_CALLBACK	port_int_cb;

	int_cb = gDD_SLIMBUS_Manager_Int_Cb[ch];
	port_int_cb = gDD_SLIMBUS_Data_Port_Int_Cb[ch];

	slimbus_reg = dd_slimbus_get_register(ch);

	int_reg = slimbus_reg->INT.word;
	if(int_cb != NULL){
		(*int_cb)((UCHAR)ch, int_reg);
	}

	slimbus_reg->INT.word = int_reg;	// Interrupt Clear

	if(int_reg & 0x00000080){	// PORT_INT Hight ?
		p0_int_reg = slimbus_reg->P0_INT.word;
		if(port_int_cb != NULL){
			(*port_int_cb)((UCHAR)ch, p0_int_reg);
		}
		slimbus_reg->P0_INT.word = p0_int_reg;	// Interrupt Clear
	}
}

// Write message to TX_FIFO.
INT32 Dd_Slimbus_Write_Msg( E_DD_SLIMBUS_CH ch, UINT32* msg, UINT32 size )
{
	T_DD_SLIMBUS_REG* slimbus_reg;
	UINT32 i, send_reg_num;

#ifdef CO_PARAM_CHECK
	if(msg == NULL){
		Ddim_Assertion(("SLIMBus input param error. [msg] = NULL\n"));
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}
	if(size > D_DD_SLIMBUS_MC_FIFO_MSG_MAX_SIZE){
		Ddim_Assertion(("SLIMBus input param error. [size] = %d\n", size));
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}
#endif

	slimbus_reg = dd_slimbus_get_register(ch);

	if(slimbus_reg->STATE.bit.TX_PUSH || slimbus_reg->STATE.bit.TX_FULL){
		return D_DD_SLIMBUS_PROCESSING;
	}

	send_reg_num = (size >> 2);
	if(size & 0x3){
		send_reg_num++;
	}

	for (i = 0; i < send_reg_num; i++, msg++) {
		slimbus_reg->MC_FIFO[i] = *msg;
	}

	slimbus_reg->COMMAND.bit.TX_PUSH = 1;	// Send MC_FIFO

	return D_DDIM_OK;
}

// Read message from RX_FIFO.
INT32 Dd_Slimbus_Read_Msg( E_DD_SLIMBUS_CH ch, UINT32* msg, UINT32* size )
{
	T_DD_SLIMBUS_REG* slimbus_reg;
	UINT32 i, read_reg_num;

#ifdef CO_PARAM_CHECK
	if(msg == NULL){
		Ddim_Assertion(("SLIMBus input param error. [msg] = NULL\n"));
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}
	if(size == NULL){
		Ddim_Assertion(("SLIMBus input param error. [size] = NULL\n"));
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}
#endif

	slimbus_reg = dd_slimbus_get_register(ch);

	if(slimbus_reg->MC_FIFO[15] & 0x00000100){	// RX_OVERFLOW
		return D_DD_SLIMBUS_OVERFLOW_ERR;
	}

	*size = slimbus_reg->MC_FIFO[15] & 0x0000003F;			// The size is acquired from RX_FIFO_FLAG.

	read_reg_num = (*size >> 2);
	if(*size & 0x3){
		read_reg_num++;
	}

	for (i = 0; i < read_reg_num; i++, msg++) {
		*msg = slimbus_reg->MC_FIFO[i];
	}

	slimbus_reg->COMMAND.bit.RX_PULL = 1;	// Read MC_FIFO

	return D_DDIM_OK;
}

// Write data to P0_FIFO.
INT32 Dd_Slimbus_Write_Data( E_DD_SLIMBUS_CH ch, UCHAR dma_ch, UINT32* data, UINT32 count, VP_SLIMBUS_CALLBACK pcallback )
{
	INT32				ret;
	T_DD_HDMAC1_CTRL	hdmac1_ctrl;
	T_DD_SLIMBUS_REG* 	slimbus_reg;

#ifdef CO_PARAM_CHECK
	if( dma_ch >= D_DD_HDMAC1_CH_NUM_MAX ){
		Ddim_Assertion(("SLIMBus input param error. [dma_ch] = %d\n", dma_ch));
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}
	if(data == NULL){
		Ddim_Assertion(("SLIMBus input param error. [data] = NULL\n"));
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}
	if(count == 0 || count > D_DD_SLIMBUS_P0_FIFO_DATA_MAX_NUM){
		Ddim_Assertion(("SLIMBus input param error. [count] = %d\n", count));
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}
	if(pcallback == NULL) {
		Ddim_Assertion(("SLIMBus input param error. [pcallback] = %p\n", pcallback));
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}
#endif

	// Open DMA.
	ret = Dd_HDMAC1_Open( dma_ch, D_DDIM_WAIT_END_TIME );
	if(ret != D_DDIM_OK){
		dd_slimbus_debug_print(("Dd_HDMAC1_Open() error. ret=0x%x\n", ret));
		return D_DD_SLIMBUS_STATUS_ABNORMAL;
	}

	memset(&hdmac1_ctrl, 0, sizeof(hdmac1_ctrl));
	hdmac1_ctrl.config_a.bit.is	= D_DD_SLIMBUS_DMACA_IS_BASE + ch;
	hdmac1_ctrl.config_a.bit.tc	= count - 1;
	hdmac1_ctrl.config_a.bit.bt	= D_DD_HDMAC1_BT_NORMAL;
	hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_WORD;
	hdmac1_ctrl.config_b.bit.ms	= D_DD_HDMAC1_MS_DEMAND;
	hdmac1_ctrl.config_b.bit.fd	= D_DD_HDMAC1_FD_FIX;

	hdmac1_ctrl.src_addr	= (ULONG)data;
	slimbus_reg = dd_slimbus_get_register(ch);
	hdmac1_ctrl.dst_addr	= (ULONG)(slimbus_reg->P0_FIFO);
	if(pcallback){
		hdmac1_ctrl.int_handler = (VP_CALLBACK)dd_slimbus_dma_int_callback;
		gDD_SLIMBUS_DMA_Int_Cb[ch] = pcallback;
		gDD_SLIMBUS_DMA_Wait[ch] = 1;
	}
	else{
		hdmac1_ctrl.int_handler = NULL;
		gDD_SLIMBUS_DMA_Int_Cb[ch] = NULL;
	}
	gDD_SLIMBUS_Ch_Info[dma_ch] = (UCHAR)ch;
	gDD_SLIMBUS_DMA_Ch_Info[ch] = dma_ch;

	ret = dd_slimbus_start_dma(dma_ch, &hdmac1_ctrl);
	if(ret != D_DDIM_OK){
		Dd_HDMAC1_Close(dma_ch);
		gDD_SLIMBUS_DMA_Wait[ch] = 0;
		return ret;
	}

	return D_DDIM_OK;
}

// Read data from P0_FIFO.
INT32 Dd_Slimbus_Read_Data( E_DD_SLIMBUS_CH ch, UCHAR dma_ch, UINT32* data, UINT32 count, VP_SLIMBUS_CALLBACK pcallback )
{
	INT32				ret;
	T_DD_HDMAC1_CTRL	hdmac1_ctrl;
	T_DD_SLIMBUS_REG* 	slimbus_reg;

#ifdef CO_PARAM_CHECK
	if( dma_ch >= D_DD_HDMAC1_CH_NUM_MAX ){
		Ddim_Assertion(("SLIMBus input param error. [dma_ch] = %d\n", dma_ch));
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}
	if(data == NULL){
		Ddim_Assertion(("SLIMBus input param error. [data] = NULL\n"));
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}
	if(count == 0 || count > D_DD_SLIMBUS_P0_FIFO_DATA_MAX_NUM){
		Ddim_Assertion(("SLIMBus input param error. [count] = %d\n", count));
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}
	if(pcallback == NULL) {
		Ddim_Assertion(("SLIMBus input param error. [pcallback] = %p\n", pcallback));
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}
#endif

	// Open DMA.
	ret = Dd_HDMAC1_Open( dma_ch, D_DDIM_WAIT_END_TIME );
	if(ret != D_DDIM_OK){
		dd_slimbus_debug_print(("Dd_HDMAC1_Open() error. ret=0x%x\n", ret));
		return D_DD_SLIMBUS_STATUS_ABNORMAL;
	}

	memset(&hdmac1_ctrl, 0, sizeof(hdmac1_ctrl));
	hdmac1_ctrl.config_a.bit.is	= D_DD_SLIMBUS_DMACA_IS_BASE + ch;
	hdmac1_ctrl.config_a.bit.tc	= count - 1;
	hdmac1_ctrl.config_a.bit.bt	= D_DD_HDMAC1_BT_NORMAL;
	hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_WORD;
	hdmac1_ctrl.config_b.bit.ms	= D_DD_HDMAC1_MS_DEMAND;
	hdmac1_ctrl.config_b.bit.fs	= D_DD_HDMAC1_FS_FIX;
	hdmac1_ctrl.config_b.bit.dp	= 8;	// HPROT of destination protection (8:cacheable)

	slimbus_reg = dd_slimbus_get_register(ch);
	hdmac1_ctrl.src_addr	= (ULONG)(slimbus_reg->P0_FIFO);
	hdmac1_ctrl.dst_addr	= (ULONG)data;
	if(pcallback){
		hdmac1_ctrl.int_handler = (VP_CALLBACK)dd_slimbus_dma_int_callback;
		gDD_SLIMBUS_DMA_Int_Cb[ch] = pcallback;
		gDD_SLIMBUS_DMA_Wait[ch] = 1;
	}
	else{
		hdmac1_ctrl.int_handler = NULL;
		gDD_SLIMBUS_DMA_Int_Cb[ch] = NULL;
	}
	gDD_SLIMBUS_Ch_Info[dma_ch] = (UCHAR)ch;
	gDD_SLIMBUS_DMA_Ch_Info[ch] = dma_ch;

	ret = dd_slimbus_start_dma(dma_ch, &hdmac1_ctrl);
	if(ret != D_DDIM_OK){
		Dd_HDMAC1_Close(dma_ch);
		gDD_SLIMBUS_DMA_Wait[ch] = 0;
		return ret;
	}

	return D_DDIM_OK;
}

#ifdef CO_DDIM_UTILITY_USE
//---------------------------- utility section ---------------------------
// Get SLIMBus Register value.
INT32 Dd_Slimbus_Get_Reg(E_DD_SLIMBUS_CH ch, USHORT offset, UINT32* data)
{
	INT32				ret = D_DDIM_OK;
	UINT32*				addr;
	T_DD_SLIMBUS_REG*	slimbus_reg;

#ifdef CO_PARAM_CHECK
	if(data == NULL){
		Ddim_Assertion(("SLIMBus input param error. [data] = NULL\n"));
		return D_DD_SLIMBUS_INPUT_PARAM_ERROR;
	}
#endif

	ret = dd_slimbus_chk_reg_par(offset);
	if(ret != D_DDIM_OK){
		return ret;
	}

	slimbus_reg = dd_slimbus_get_register(ch);
	addr = (UINT32*)((UCHAR*)slimbus_reg + offset);

	dd_slimbus_debug_print(("[DD_SLIMBUS] addr  = 0x%08X\n", addr));
	dd_slimbus_debug_print(("[DD_SLIMBUS] *addr = 0x%08X\n", *addr));

	*data = *addr;

	return ret;
}

// Set SLIMBus Register value.
INT32 Dd_Slimbus_Set_Reg(E_DD_SLIMBUS_CH ch, USHORT offset, UINT32 data)
{
	INT32				ret = D_DDIM_OK;
	UINT32*				addr;
	T_DD_SLIMBUS_REG*	slimbus_reg;

	ret = dd_slimbus_chk_reg_par(offset);
	if(ret != D_DDIM_OK){
		return ret;
	}

	slimbus_reg = dd_slimbus_get_register(ch);
	addr = (UINT32*)((UCHAR*)slimbus_reg + offset);

	dd_slimbus_debug_print(("[DD_SLIMBUS] addr  = 0x%08X\n", addr));
	dd_slimbus_debug_print(("[DD_SLIMBUS] *addr = 0x%08X\n", *addr));

	*addr = data;

	return ret;
}

//---------------------------- colabo section ----------------------------
// Nothing Special
#endif
