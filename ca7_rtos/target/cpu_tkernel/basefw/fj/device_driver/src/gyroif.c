/**
 * @file		gyroif.c
 * @brief		Gyro I/F Controller
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2016 Socionext Inc.</I></B>\n
 */

//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "fj_peripheral.h"
#include "gyroif.h"
#include "peripheral.h"
#include "chiptop.h"
#include "../../../MILB_API/MILB_Header/include/spi.h"
#include "dd_spi.h"
#include "dd_tmr64.h"
#include "dd_hdmac1.h"
#include "dd_arm.h"
#include "debug.h"
#include "os_user_custom.h"
#include "sdram_map_cache_define.h"
#include "l1l2cache.h"
#include "mba_if.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_BF_GYROIF_SPI_CH					(FJ_PERI_CH_2)

#define D_BF_GYROIF_HDMAC1_CH_TMR64			(FJ_USIO_DMA_CH_0)
#define D_BF_GYROIF_HDMAC1_CH_SPI_CONTXFER1	(FJ_USIO_DMA_CH_1)
#define D_BF_GYROIF_HDMAC1_CH_SPI_SEND		(FJ_USIO_DMA_CH_2)
#define D_BF_GYROIF_HDMAC1_CH_SPI_RECV		(FJ_USIO_DMA_CH_3)
#define D_BF_GYROIF_HDMAC1_CH_SPI_CONTXFER0	(FJ_USIO_DMA_CH_4)

//#define D_GYROIF_TIMESTAMP_DMA

//#define D_GYROIF_DEBUG

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Callback Function Prototype                                          */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static volatile FJ_GYROIF_READ_CTRL		gBF_GyroIF_Read_Ctrl;
static volatile BOOL					gBF_GyroIF_Stop_Flag;
static volatile union io_spi_auxctrl	gAUX_CTRL_valu_1 __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));	// D-Cache line aligned.;
static volatile union io_spi_auxctrl	gAUX_CTRL_valu_2 __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));	// D-Cache line aligned.;

static ULONG gBF_GyroIF_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/**
Callback function from TMR64 DMA interrupt.
*/
static VOID gyroif_tmr64_callback( UCHAR dma_ch, USHORT* const status )
{
	if( *status != D_DD_HDMAC1_SS_NORMAL_END ){
		if( *status != D_DD_HDMAC1_SS_STOP_REQ ){
			BF_Debug_Print_Warning(("FJ_GyroIF: TMR64 HDMAC[%d] stop status error!! : line=%d, status=%d\n", dma_ch, __LINE__, *status));
			if( gBF_GyroIF_Read_Ctrl.stamp_ctrl.pcallback != NULL ) {
				(*gBF_GyroIF_Read_Ctrl.stamp_ctrl.pcallback)( FJ_ERR_NG );
			}
		}
		else {
			BF_Debug_Print_Information(("FJ_GyroIF: TMR64 HDMAC[%d] stop end : line=%d, status=%d\n", dma_ch, __LINE__, *status));
		}
	}
}

/**
Callback function from SPI send DMA interrupt.
*/
static VOID gyroif_spi_send_callback( UCHAR dma_ch, USHORT* const status )
{
	if( *status != D_DD_HDMAC1_SS_NORMAL_END ){
		if( *status != D_DD_HDMAC1_SS_STOP_REQ ){
			BF_Debug_Print_Warning(("FJ_GyroIF: SPI Send HDMAC[%d] stop status error!! : line=%d, status=%d\n", dma_ch, __LINE__, *status));
			if( gBF_GyroIF_Read_Ctrl.data_ctrl.pcallback != NULL ) {
				(*gBF_GyroIF_Read_Ctrl.data_ctrl.pcallback)( FJ_ERR_NG );
			}
		}
		else {
			BF_Debug_Print_Information(("FJ_GyroIF: SPI Send HDMAC[%d] stop end : line=%d, status=%d\n", dma_ch, __LINE__, *status));
		}
	}
}

/**
Callback function from SPI receive DMA interrupt.
*/
static VOID gyroif_spi_recv_callback( UCHAR dma_ch, USHORT* const status )
{
	if( *status != D_DD_HDMAC1_SS_NORMAL_END ){
		if( *status != D_DD_HDMAC1_SS_STOP_REQ ){
			BF_Debug_Print_Warning(("FJ_GyroIF: SPI Recv HDMAC[%d] stop status error!! : line=%d, status=%d\n", dma_ch, __LINE__, *status));
			if( gBF_GyroIF_Read_Ctrl.data_ctrl.pcallback != NULL ) {
				(*gBF_GyroIF_Read_Ctrl.data_ctrl.pcallback)( FJ_ERR_NG );
			}
		}
		else {
			BF_Debug_Print_Information(("FJ_GyroIF: SPI Recv HDMAC[%d] stop end : line=%d, status=%d\n", dma_ch, __LINE__, *status));
		}
	}
}

/**
Callback function from SPI contXferExtend=1 DMA interrupt.
*/
static VOID gyroif_spi_contxfer1_callback( UCHAR dma_ch, USHORT* const status )
{
	if( *status != D_DD_HDMAC1_SS_NORMAL_END ){
		if( *status != D_DD_HDMAC1_SS_STOP_REQ ){
			BF_Debug_Print_Warning(("FJ_GyroIF: SPI Cont1 HDMAC[%d] stop status error!! : line=%d, status=%d\n", dma_ch, __LINE__, *status));
			if( gBF_GyroIF_Read_Ctrl.data_ctrl.pcallback != NULL ) {
				(*gBF_GyroIF_Read_Ctrl.data_ctrl.pcallback)( FJ_ERR_NG );
			}
		}
		else {
			BF_Debug_Print_Information(("FJ_GyroIF: SPI Cont1 HDMAC[%d] stop end : line=%d, status=%d\n", dma_ch, __LINE__, *status));
		}
	}
}

/**
Callback function from SPI contXferExtend=0 DMA interrupt.
*/
static VOID gyroif_spi_contxfer0_callback( UCHAR dma_ch, USHORT* const status )
{
#ifdef D_GYROIF_DEBUG
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_START, "GyroIF CB"));
#endif

#ifdef D_GYROIF_DEBUG
	IO_CHIPTOP.PDR.word[0] = 0x20002000;
	Dd_ARM_Dsb_Pou();
#endif

	if( *status != D_DD_HDMAC1_SS_NORMAL_END ){
		if( *status != D_DD_HDMAC1_SS_STOP_REQ ){
			BF_Debug_Print_Warning(("FJ_GyroIF: SPI Cont0 HDMAC[%d] stop status error!! : line=%d, status=%d\n", dma_ch, __LINE__, *status));
			if( gBF_GyroIF_Read_Ctrl.data_ctrl.pcallback != NULL ) {
				(*gBF_GyroIF_Read_Ctrl.data_ctrl.pcallback)( FJ_ERR_NG );
			}
		}
		else {
			BF_Debug_Print_Information(("FJ_GyroIF: SPI Cont0 HDMAC[%d] stop end : line=%d, status=%d\n", dma_ch, __LINE__, *status));
		}
	}
	else {
		Dd_ARM_Critical_Section_Start(gBF_GyroIF_Spin_Lock);
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		// Disable input Gyro INT
		Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PB4, 0);
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		IO_HDMAC1.DMAC[D_BF_GYROIF_HDMAC1_CH_TMR64].DMACA.bit.EB = 1;
		IO_HDMAC1.DMAC[D_BF_GYROIF_HDMAC1_CH_SPI_CONTXFER1].DMACA.bit.EB = 1;
		IO_HDMAC1.DMAC[D_BF_GYROIF_HDMAC1_CH_SPI_SEND].DMACA.bit.EB = 1;
		IO_HDMAC1.DMAC[D_BF_GYROIF_HDMAC1_CH_SPI_RECV].DMACA.bit.EB = 1;
		IO_HDMAC1.DMAC[D_BF_GYROIF_HDMAC1_CH_SPI_CONTXFER0].DMACA.bit.EB = 1;
		Dd_ARM_Dsb_Pou();
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		// Enable input Gyro INT
		Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PB4, 1);
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		Dd_ARM_Critical_Section_End(gBF_GyroIF_Spin_Lock);
	}

#ifdef D_GYROIF_DEBUG
	IO_CHIPTOP.PDR.word[0] = 0x20000000;
	Dd_ARM_Dsb_Pou();
	BF_Debug_Print_Information(("-GyroIF CB-\n"));
#endif

#ifdef D_GYROIF_DEBUG
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_GYRO, FJ_MBALOG_TYPE_END, "GyroIF CB"));
#endif
}


/**
Start TMR64.
*/
static FJ_ERR_CODE gyroif_start_tmr64( FJ_GYROIF_STAMP_CTRL const* const stamp_ctrl )
{
	INT32			ret;
	T_DD_TMR64_CTRL	tmr64_ctrl;

	// Open TMR64
	ret = Dd_TMR64_Open( D_DDIM_USER_SEM_WAIT_POL );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_TMR64_Open Error!! : line=%d, ret=0x%08x\n", __LINE__, ret));
		return FJ_ERR_NG;
	}

	// Ctrl TMR64
	tmr64_ctrl.csr1.word	= 0;
	tmr64_ctrl.csr1.bit.csl	= stamp_ctrl->timer_clk;
	tmr64_ctrl.csr2.word	= 0;

	ret = Dd_TMR64_Ctrl( &tmr64_ctrl );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_TMR64_Ctrl Error!! : line=%d, ret=0x%08x\n", __LINE__, ret));
		Dd_TMR64_Close();
		return FJ_ERR_NG;
	}

	// Start TMR64
	ret = Dd_TMR64_Start( FALSE );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_TMR64_Start Error!! : line=%d, ret=0x%08x\n", __LINE__, ret));
		Dd_TMR64_Close();
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
Stop TMR64.
*/
static FJ_ERR_CODE gyroif_stop_tmr64( VOID )
{
	FJ_ERR_CODE	err_code = FJ_ERR_OK;
	INT32		ret;

	// Start TMR64
	ret = Dd_TMR64_Stop();
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_TMR64_Stop Error!! : line=%d, ret=0x%08x\n", __LINE__, ret));
		err_code = FJ_ERR_NG;
	}

	// Open TMR64
	ret = Dd_TMR64_Close();
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_TMR64_Close Error!! : line=%d, ret=0x%08x\n", __LINE__, ret));
		err_code = FJ_ERR_NG;
	}

	return err_code;
}

/**
Start DMA for TMR64.
*/
static FJ_ERR_CODE gyroif_start_dma_tmr64( FJ_GYROIF_STAMP_CTRL const* const stamp_ctrl )
{
	INT32				ret;
	T_DD_HDMAC1_CTRL	hdmac1_ctrl;
	UCHAR				dma_ch = D_BF_GYROIF_HDMAC1_CH_TMR64;

	// Open TMR64 DMA.
	ret = Dd_HDMAC1_Open( dma_ch, D_DDIM_SEM_WAIT_POL );
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Open Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		return FJ_ERR_NG;
	}

	// Set TMR64 DMA Ctrl.
	hdmac1_ctrl.config_a.word	= 0;
	hdmac1_ctrl.config_b.word	= 0;
	hdmac1_ctrl.config_a.bit.is	= D_DD_HDMAC1_IS_IDREQ_24;
	hdmac1_ctrl.config_a.bit.bt	= D_DD_HDMAC1_BT_NORMAL;
	hdmac1_ctrl.config_a.bit.bc	= 2 - 1;
	hdmac1_ctrl.config_a.bit.tc	= stamp_ctrl->total_data_count - 1;
//	hdmac1_ctrl.config_b.bit.tt	= 0;
	hdmac1_ctrl.config_b.bit.ms	= D_DD_HDMAC1_MS_BLK;
	hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_WORD;
	hdmac1_ctrl.config_b.bit.fs	= D_DD_HDMAC1_FS_FIX;
	hdmac1_ctrl.config_b.bit.fd	= D_DD_HDMAC1_FD_INCR;
	hdmac1_ctrl.config_b.bit.rc	= D_DD_HDMAC1_RC_ENABLE;
	hdmac1_ctrl.config_b.bit.rs	= D_DD_HDMAC1_RS_DISABLE;
	hdmac1_ctrl.config_b.bit.rd	= D_DD_HDMAC1_RD_ENABLE;
//	hdmac1_ctrl.config_b.bit.ei = 1;
//	hdmac1_ctrl.config_b.bit.ci = 1;
//	hdmac1_ctrl.config_b.bit.ss = 0;
	hdmac1_ctrl.config_b.bit.sp = 8;
	hdmac1_ctrl.config_b.bit.dp = 8;
	hdmac1_ctrl.src_addr		= (ULONG)&(IO_PERI.TMR64.GSTMP);
	hdmac1_ctrl.dst_addr		= stamp_ctrl->buff_addr;
	hdmac1_ctrl.int_handler		= (VP_CALLBACK)gyroif_tmr64_callback;

	// Ctrl TMR64 DMA.
	ret = Dd_HDMAC1_Ctrl_Common( dma_ch, &hdmac1_ctrl );
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Ctrl_Common Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

	// Start TMR64 DMA.
	ret = Dd_HDMAC1_Start_Async( dma_ch );
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Start_Async Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

	IO_HDMAC1.DMAC[dma_ch].DMACB.bit.CI = 0;

	return FJ_ERR_OK;
}

/**
Start DMA for SPI send.
*/
static FJ_ERR_CODE gyroif_start_dma_spi_send( FJ_GYROIF_DATA_CTRL const* const data_ctrl )
{
	INT32				ret;
	T_DD_HDMAC1_CTRL	hdmac1_ctrl;
	UCHAR				spi_ch = D_BF_GYROIF_SPI_CH;
	UCHAR				dma_ch = D_BF_GYROIF_HDMAC1_CH_SPI_SEND;

	// Open SPI send DMA.
	ret = Dd_HDMAC1_Open( dma_ch, D_DDIM_SEM_WAIT_POL );
	if(ret != D_DDIM_OK){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Open Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		return FJ_ERR_NG;
	}

	// Set SPI send DMA Ctrl.
	hdmac1_ctrl.config_a.word		= 0;
	hdmac1_ctrl.config_b.word		= 0;
	hdmac1_ctrl.config_a.bit.is		= D_DD_HDMAC1_IS_IDREQ_24;
	hdmac1_ctrl.config_a.bit.bt		= D_DD_HDMAC1_BT_NORMAL;
	hdmac1_ctrl.config_a.bit.bt		= D_DD_HDMAC1_BT_INCR;
	hdmac1_ctrl.config_a.bit.bc		= data_ctrl->data_count - 1;
	hdmac1_ctrl.config_a.bit.tc		= data_ctrl->total_data_count - 1;
//	hdmac1_ctrl.config_b.bit.tt		= 0;
	hdmac1_ctrl.config_b.bit.ms		= D_DD_HDMAC1_MS_BLK;
	if( data_ctrl->data_length > FJ_SPI_LENGTH_8 ){
		hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_HWORD;
	}
	else{
		hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_BYTE;
	}
	hdmac1_ctrl.config_b.bit.fs		= D_DD_HDMAC1_FS_FIX;
	hdmac1_ctrl.config_b.bit.fd		= D_DD_HDMAC1_FD_FIX;
	hdmac1_ctrl.config_b.bit.rc		= D_DD_HDMAC1_RC_ENABLE;
	hdmac1_ctrl.config_b.bit.rs		= D_DD_HDMAC1_RS_ENABLE;
	hdmac1_ctrl.config_b.bit.rd		= D_DD_HDMAC1_RD_DISABLE;
//	hdmac1_ctrl.config_b.bit.ei		= 1;
//	hdmac1_ctrl.config_b.bit.ci		= 1;
//	hdmac1_ctrl.config_b.bit.ss		= 0;
	hdmac1_ctrl.config_b.bit.sp		= 8;
	hdmac1_ctrl.config_b.bit.dp		= 8;
	hdmac1_ctrl.src_addr			= data_ctrl->send_addr;
	hdmac1_ctrl.dst_addr			= (ULONG)&(IO_SPI[spi_ch].TXDATA);
	hdmac1_ctrl.int_handler			= (VP_CALLBACK)gyroif_spi_send_callback;

	// Ctrl SPI send DMA.
	ret = Dd_HDMAC1_Ctrl_Common( dma_ch, &hdmac1_ctrl );
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Ctrl_Common Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

	// Start SPI send DMA.
	ret = Dd_HDMAC1_Start_Async( dma_ch );
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Start_Async Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

	IO_HDMAC1.DMAC[dma_ch].DMACB.bit.CI = 0;

	return FJ_ERR_OK;
}

/**
Start DMA for SPI receive.
*/
static FJ_ERR_CODE gyroif_start_dma_spi_recv( FJ_GYROIF_DATA_CTRL const* const data_ctrl )
{
	INT32				ret;
	T_DD_HDMAC1_CTRL	hdmac1_ctrl;
	UCHAR				spi_ch = D_BF_GYROIF_SPI_CH;
	UCHAR				dma_ch = D_BF_GYROIF_HDMAC1_CH_SPI_RECV;

	// Open SPI receive DMA.
	ret = Dd_HDMAC1_Open( dma_ch, D_DDIM_SEM_WAIT_POL );
	if(ret != D_DDIM_OK){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Open Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		return FJ_ERR_NG;
	}

	// Set SPI receive DMA Ctrl.
	hdmac1_ctrl.config_a.word		= 0;
	hdmac1_ctrl.config_b.word		= 0;
	hdmac1_ctrl.config_a.bit.is		= D_DD_HDMAC1_IS_IDREQ_20;
	hdmac1_ctrl.config_a.bit.bt		= D_DD_HDMAC1_BT_INCR;
	hdmac1_ctrl.config_a.bit.bc		= data_ctrl->data_count - 1;
	hdmac1_ctrl.config_a.bit.tc		= data_ctrl->total_data_count - 1;
//	hdmac1_ctrl.config_b.bit.tt		= 0;
	hdmac1_ctrl.config_b.bit.ms		= D_DD_HDMAC1_MS_BLK;
	if( data_ctrl->data_length > FJ_SPI_LENGTH_8 ){
		hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_HWORD;
	}
	else{
		hdmac1_ctrl.config_b.bit.tw	= D_DD_HDMAC1_TW_BYTE;
	}
	hdmac1_ctrl.config_b.bit.fs		= D_DD_HDMAC1_FS_FIX;
	hdmac1_ctrl.config_b.bit.fd		= D_DD_HDMAC1_FD_INCR;
	hdmac1_ctrl.config_b.bit.rc		= D_DD_HDMAC1_RC_ENABLE;
	hdmac1_ctrl.config_b.bit.rs		= D_DD_HDMAC1_RS_DISABLE;
	hdmac1_ctrl.config_b.bit.rd 	= D_DD_HDMAC1_RD_ENABLE;
//	hdmac1_ctrl.config_b.bit.ei		= 1;
//	hdmac1_ctrl.config_b.bit.ci		= 1;
//	hdmac1_ctrl.config_b.bit.ss		= 0;
	hdmac1_ctrl.config_b.bit.sp		= 8;
	hdmac1_ctrl.config_b.bit.dp		= 8;
	hdmac1_ctrl.src_addr			= (ULONG)&(IO_SPI[spi_ch].RXDATA);
	hdmac1_ctrl.dst_addr			= data_ctrl->recv_addr;
	hdmac1_ctrl.int_handler			= (VP_CALLBACK)gyroif_spi_recv_callback;

	// Ctrl SPI receive DMA.
	ret = Dd_HDMAC1_Ctrl_Common( dma_ch, &hdmac1_ctrl );
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Ctrl_Common Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

	// Start SPI receive DMA.
	ret = Dd_HDMAC1_Start_Async( dma_ch );
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Start_Async Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

	IO_HDMAC1.DMAC[dma_ch].DMACB.bit.CI = 0;

	return FJ_ERR_OK;
}

/**
Start DMA for SPI contXferExtend=1 set.
*/
static FJ_ERR_CODE gyroif_start_dma_spi_contxfer1( FJ_GYROIF_DATA_CTRL const* const data_ctrl )
{
	INT32				ret;
	T_DD_HDMAC1_CTRL	hdmac1_ctrl;
	UCHAR				spi_ch = D_BF_GYROIF_SPI_CH;
	UCHAR				dma_ch = D_BF_GYROIF_HDMAC1_CH_SPI_CONTXFER1;

	// Open CS control1 DMA.
	ret = Dd_HDMAC1_Open( dma_ch, D_DDIM_SEM_WAIT_POL );
	if(ret != D_DDIM_OK){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Open Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		return FJ_ERR_NG;
	}

	// Set CS control1 DMA Ctrl.
	hdmac1_ctrl.config_a.word		= 0;
	hdmac1_ctrl.config_b.word		= 0;
	hdmac1_ctrl.config_a.bit.is		= D_DD_HDMAC1_IS_IDREQ_24;
	hdmac1_ctrl.config_a.bit.bt		= D_DD_HDMAC1_BT_NORMAL;
	hdmac1_ctrl.config_a.bit.bc		= 0;
	hdmac1_ctrl.config_a.bit.tc		= data_ctrl->total_data_count - 1;
//	hdmac1_ctrl.config_b.bit.tt		= 0;
	hdmac1_ctrl.config_b.bit.ms		= D_DD_HDMAC1_MS_BLK;
	hdmac1_ctrl.config_b.bit.tw		= D_DD_HDMAC1_TW_WORD;
	hdmac1_ctrl.config_b.bit.fs		= D_DD_HDMAC1_FS_FIX;
	hdmac1_ctrl.config_b.bit.fd		= D_DD_HDMAC1_FD_FIX;
	hdmac1_ctrl.config_b.bit.rc		= D_DD_HDMAC1_RC_ENABLE;
	hdmac1_ctrl.config_b.bit.rs		= D_DD_HDMAC1_RS_DISABLE;
	hdmac1_ctrl.config_b.bit.rd 	= D_DD_HDMAC1_RD_DISABLE;
//	hdmac1_ctrl.config_b.bit.ei		= 1;
//	hdmac1_ctrl.config_b.bit.ci		= 1;
//	hdmac1_ctrl.config_b.bit.ss		= 0;
	hdmac1_ctrl.config_b.bit.sp		= 8;
	hdmac1_ctrl.config_b.bit.dp		= 8;
	hdmac1_ctrl.src_addr			= (ULONG)&(gAUX_CTRL_valu_1);
	hdmac1_ctrl.dst_addr			= (ULONG)&(IO_SPI[spi_ch].AUX_CTRL);
	hdmac1_ctrl.int_handler			= (VP_CALLBACK)gyroif_spi_contxfer1_callback;

	// Ctrl CS control1 DMA.
	ret = Dd_HDMAC1_Ctrl_Common( dma_ch, &hdmac1_ctrl );
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Ctrl_Common Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

	// Start CS control1 DMA.
	ret = Dd_HDMAC1_Start_Async( dma_ch );
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Start_Async Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

	IO_HDMAC1.DMAC[dma_ch].DMACB.bit.CI = 0;

	return FJ_ERR_OK;
}

/**
Start DMA for SPI contXferExtend=0 set.
*/
static FJ_ERR_CODE gyroif_start_dma_spi_contxfer0( FJ_GYROIF_DATA_CTRL const* const data_ctrl )
{
	INT32				ret;
	T_DD_HDMAC1_CTRL	hdmac1_ctrl;
	UCHAR				spi_ch = D_BF_GYROIF_SPI_CH;
	UCHAR				dma_ch = D_BF_GYROIF_HDMAC1_CH_SPI_CONTXFER0;

	// Open CS control2 DMA.
	ret = Dd_HDMAC1_Open( dma_ch, D_DDIM_SEM_WAIT_POL );
	if(ret != D_DDIM_OK){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Open Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		return FJ_ERR_NG;
	}

	// Set CS control2 DMA Ctrl.
	hdmac1_ctrl.config_a.word		= 0;
	hdmac1_ctrl.config_b.word		= 0;
	hdmac1_ctrl.config_a.bit.is		= D_DD_HDMAC1_IS_IDREQ_20;
	hdmac1_ctrl.config_a.bit.bt		= D_DD_HDMAC1_BT_NORMAL;
	hdmac1_ctrl.config_a.bit.bc		= 0;
	hdmac1_ctrl.config_a.bit.tc		= data_ctrl->total_data_count - 1;
//	hdmac1_ctrl.config_b.bit.tt		= 0;
	hdmac1_ctrl.config_b.bit.ms		= D_DD_HDMAC1_MS_BLK;
	hdmac1_ctrl.config_b.bit.tw		= D_DD_HDMAC1_TW_WORD;
	hdmac1_ctrl.config_b.bit.fs		= D_DD_HDMAC1_FS_FIX;
	hdmac1_ctrl.config_b.bit.fd		= D_DD_HDMAC1_FD_FIX;
	hdmac1_ctrl.config_b.bit.rc		= D_DD_HDMAC1_RC_ENABLE;
	hdmac1_ctrl.config_b.bit.rs		= D_DD_HDMAC1_RS_DISABLE;
	hdmac1_ctrl.config_b.bit.rd 	= D_DD_HDMAC1_RD_DISABLE;
//	hdmac1_ctrl.config_b.bit.ei		= 1;
//	hdmac1_ctrl.config_b.bit.ci		= 1;
//	hdmac1_ctrl.config_b.bit.ss		= 0;
	hdmac1_ctrl.config_b.bit.sp		= 8;
	hdmac1_ctrl.config_b.bit.dp		= 8;
	hdmac1_ctrl.src_addr			= (ULONG)&(gAUX_CTRL_valu_2);
	hdmac1_ctrl.dst_addr			= (ULONG)&(IO_SPI[spi_ch].AUX_CTRL);
	hdmac1_ctrl.int_handler			= (VP_CALLBACK)gyroif_spi_contxfer0_callback;

	// Ctrl CS control2 DMA.
	ret = Dd_HDMAC1_Ctrl_Common( dma_ch, &hdmac1_ctrl );
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Ctrl_Common Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

	// Start CS control2 DMA.
	ret = Dd_HDMAC1_Start_Async( dma_ch );
	if( ret != D_DDIM_OK ){
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Start_Async Error!! : line=%d, ret=0x%08x, dma_ch=%d\n", __LINE__, ret, dma_ch));
		Dd_HDMAC1_Close( dma_ch );
		return FJ_ERR_NG;
	}

	gAUX_CTRL_valu_1.word				= IO_SPI[spi_ch].AUX_CTRL.word;
	gAUX_CTRL_valu_1.bit.CONTXFEREXTEND	= 1;
	gAUX_CTRL_valu_2.word				= gAUX_CTRL_valu_1.word;
	gAUX_CTRL_valu_2.bit.CONTXFEREXTEND	= 0;

	BF_L1l2cache_Clean_Flush_Addr( (UINT32)&gAUX_CTRL_valu_1.word, sizeof(gAUX_CTRL_valu_1.word) );
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)&gAUX_CTRL_valu_2.word, sizeof(gAUX_CTRL_valu_2.word) );

	return FJ_ERR_OK;
}

/**
Stop DMA.
*/
static FJ_ERR_CODE gyroif_stop_dma( UCHAR dma_ch )
{
	FJ_ERR_CODE	err_code = FJ_ERR_OK;
	INT32		ret;

	// Stop HDMAC
	ret = Dd_HDMAC1_Stop( dma_ch );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Stop Error!! : line=%d, ret=0x%08x\n", __LINE__, ret));
		err_code = FJ_ERR_NG;
	}

	// Close HDMAC
	ret = Dd_HDMAC1_Close( dma_ch );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_HDMAC1_Close Error!! : line=%d, ret=0x%08x\n", __LINE__, ret));
		err_code = FJ_ERR_NG;
	}

	return err_code;
}

/**
Set SPI Contorl.
*/
static FJ_ERR_CODE gyroif_set_spi( FJ_GYROIF_DATA_CTRL const* const data_ctrl, E_DD_SPI_ENABLE_SIG enable_sig )
{
	INT32			ret;
	T_DD_SPI_CTRL	spi_ctrl;
	UCHAR			spi_ch = D_BF_GYROIF_SPI_CH;
	UCHAR			clk_div;

	// Open SPI
	ret = Dd_SPI_Open( spi_ch, D_DDIM_SEM_WAIT_POL );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_SPI_Open Error!! : line=%d, ret=0x%08x\n", __LINE__, ret));
		return FJ_ERR_NG;
	}

	// Calculate SPI baudrate
	ret = Dd_SPI_Calculate( data_ctrl->baudrate, &clk_div );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_SPI_Calculate Error!! : line=%d, ret=0x%08x\n", __LINE__, ret));
		Dd_SPI_Close( spi_ch );
		return FJ_ERR_NG;
	}

	// Set SPI Ctrl
	spi_ctrl.type				= E_DD_SPI_TYPE_MASTER;
	spi_ctrl.mode				= data_ctrl->mode;
	spi_ctrl.enable_sig			= enable_sig;
	spi_ctrl.bit_direction 		= data_ctrl->bit_direction;
	spi_ctrl.bit_length			= data_ctrl->data_length;
	spi_ctrl.clk_div			= clk_div;
	spi_ctrl.delay_en			= 0;
	spi_ctrl.delay_val			= 0;
	spi_ctrl.rx_inhibit_en		= 0;
	spi_ctrl.fifo_wmark_tx		= data_ctrl->fifo_wmark_tx;
	spi_ctrl.fifo_wmark_rx		= data_ctrl->fifo_wmark_rx;
	spi_ctrl.dma_en				= 1;
	spi_ctrl.dma_recv_to		= data_ctrl->dma_recv_to;
	spi_ctrl.ss_info.cont_trans	= data_ctrl->ss_info.cont_trans;
	spi_ctrl.ss_info.ssout[0]	= data_ctrl->ss_info.ssout[0];
	spi_ctrl.ss_info.ssout[1]	= data_ctrl->ss_info.ssout[1];
	spi_ctrl.ss_info.ssout[2]	= data_ctrl->ss_info.ssout[2];
	spi_ctrl.ss_info.ssout[3]	= data_ctrl->ss_info.ssout[3];
	spi_ctrl.ss_info.sspol[0]	= data_ctrl->ss_info.sspol[0];
	spi_ctrl.ss_info.sspol[1]	= data_ctrl->ss_info.sspol[1];
	spi_ctrl.ss_info.sspol[2]	= data_ctrl->ss_info.sspol[2];
	spi_ctrl.ss_info.sspol[3]	= data_ctrl->ss_info.sspol[3];
	spi_ctrl.pcallback			= NULL;
	spi_ctrl.pcallback_ss		= NULL;

	// Call SPI driver
	ret = Dd_SPI_Ctrl( spi_ch, &spi_ctrl );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_SPI_Ctrl Error!! : line=%d, ret=0x%08x\n", __LINE__, ret));
		Dd_SPI_Close( spi_ch );
		return FJ_ERR_NG;
	}

	gAUX_CTRL_valu_1.word				= IO_SPI[spi_ch].AUX_CTRL.word;
	gAUX_CTRL_valu_1.bit.CONTXFEREXTEND	= 1;
	gAUX_CTRL_valu_2.word				= gAUX_CTRL_valu_1.word;
	gAUX_CTRL_valu_2.bit.CONTXFEREXTEND	= 0;

	BF_L1l2cache_Clean_Flush_Addr( (UINT32)&gAUX_CTRL_valu_1.word, sizeof(gAUX_CTRL_valu_1.word) );
	BF_L1l2cache_Clean_Flush_Addr( (UINT32)&gAUX_CTRL_valu_2.word, sizeof(gAUX_CTRL_valu_2.word) );

	return FJ_ERR_OK;
}

/**
Stop SPI.
*/
static FJ_ERR_CODE gyroif_stop_spi( VOID )
{
	FJ_ERR_CODE	err_code = FJ_ERR_OK;
	INT32		ret;
	UCHAR		spi_ch = D_BF_GYROIF_SPI_CH;

	// Stop SPI
	ret = Dd_SPI_Stop( spi_ch );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_SPI_Stop Error!! : line=%d, ret=0x%08x\n", __LINE__, ret));
		err_code = FJ_ERR_NG;
	}

	// Close SPI
	ret = Dd_SPI_Close( spi_ch );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_GyroIF: Dd_SPI_Close Error!! : line=%d, ret=0x%08x\n", __LINE__, ret));
		err_code = FJ_ERR_NG;
	}

	return err_code;
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
Start Gyro data read.
*/
FJ_ERR_CODE fj_gyroif_start_read( FJ_GYROIF_READ_CTRL const* const gyro_ctrl )
{
	FJ_ERR_CODE	err_code = FJ_ERR_OK;

	gBF_GyroIF_Read_Ctrl = *gyro_ctrl;

	IO_HDMAC1.DMACR.bit.PR = 0;		// If Block Transfer Mode and set the contXferExtend, PR = 0 is mandatory setting.

#ifdef D_GYROIF_DEBUG
	IO_CHIPTOP.DDR.bit.P75 = 1;
	IO_CHIPTOP.PDR.word[0] = 0x20000000;
#endif

	// Start TMR64
	err_code = gyroif_start_tmr64( &gyro_ctrl->stamp_ctrl );
	if( err_code != FJ_ERR_OK ){
		return err_code;
	}

	// Start DMA for TMR64
	err_code = gyroif_start_dma_tmr64( &gyro_ctrl->stamp_ctrl );
	if( err_code != FJ_ERR_OK ){
		gyroif_stop_tmr64();
		gyroif_stop_spi();
		return err_code;
	}

	// Set SPI Control(EXT0).
	err_code = gyroif_set_spi( &gyro_ctrl->data_ctrl, E_DD_SPI_ENABLE_SIG_CPU );
	if( err_code != FJ_ERR_OK ){
		gyroif_stop_tmr64();
		return err_code;
	}

	// Start DMA for SPI receive.
	err_code = gyroif_start_dma_spi_recv( &gyro_ctrl->data_ctrl );
	if( err_code != FJ_ERR_OK ){
		gyroif_stop_tmr64();
		gyroif_stop_spi();
		gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_TMR64 );
		gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_SEND );
		return err_code;
	}

	// Start DMA for SPI send.
	err_code = gyroif_start_dma_spi_send( &gyro_ctrl->data_ctrl );
	if( err_code != FJ_ERR_OK ){
		gyroif_stop_tmr64();
		gyroif_stop_spi();
		gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_TMR64 );
		return err_code;
	}

	// Start DMA for SPI contXferExtend=1 set.
	err_code = gyroif_start_dma_spi_contxfer1( &gyro_ctrl->data_ctrl );
	if( err_code != FJ_ERR_OK ){
		gyroif_stop_tmr64();
		gyroif_stop_spi();
		gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_TMR64 );
		gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_SEND );
		gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_RECV );
		return err_code;
	}

	// Start DMA for SPI CS contXferExtend=0 set.
	err_code = gyroif_start_dma_spi_contxfer0( &gyro_ctrl->data_ctrl );
	if( err_code != FJ_ERR_OK ){
		gyroif_stop_tmr64();
		gyroif_stop_spi();
		gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_TMR64 );
		gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_SEND );
		gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_RECV );
		gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_CONTXFER1 );
		return err_code;
	}

	gBF_GyroIF_Stop_Flag = FALSE;

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	// Disable input Gyro INT
	Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PB4, 0);
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	// Enable SPI
	Dd_SPI_Set_SPI_Enable( D_BF_GYROIF_SPI_CH, E_DD_SPI_ENABLE_SIG_CPU, TRUE );
	// Enable Timestamp
	IO_PERI.TMR64.TMCSR64_2.word |= 0x00010000;	// TSEN=1

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	// Enable input Gyro INT
	Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PB4, 1);
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	return FJ_ERR_OK;
}

/**
Stopt Gyro data read.
*/
FJ_ERR_CODE fj_gyroif_stop_read( VOID )
{
	FJ_ERR_CODE err_code = FJ_ERR_OK;
	FJ_ERR_CODE ret_code = FJ_ERR_OK;

	gBF_GyroIF_Stop_Flag = TRUE;

	err_code = gyroif_stop_tmr64();
	if( err_code != FJ_ERR_OK ){
		ret_code = err_code;
	}

	err_code = gyroif_stop_spi();
	if( err_code != FJ_ERR_OK ){
		ret_code = err_code;
	}

	err_code = gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_TMR64 );
	if( err_code != FJ_ERR_OK ){
		ret_code = err_code;
	}

	err_code = gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_SEND );
	if( err_code != FJ_ERR_OK ){
		ret_code = err_code;
	}

	err_code = gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_RECV );
	if( err_code != FJ_ERR_OK ){
		ret_code = err_code;
	}

	err_code = gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_CONTXFER1 );
	if( err_code != FJ_ERR_OK ){
		ret_code = err_code;
	}

	err_code = gyroif_stop_dma( D_BF_GYROIF_HDMAC1_CH_SPI_CONTXFER0 );
	if( err_code != FJ_ERR_OK ){
		ret_code = err_code;
	}

	return ret_code;
}

/**
Get Gyro data read count.
*/
FJ_ERR_CODE fj_gyroif_get_read_count( UINT32* const read_count )
{
#if 1
	INT32	ret;
#ifdef D_GYROIF_DEBUG
	ULONG	tc_recv, tc_send, tc_stmp;
#else
	ULONG	tc_recv;
#endif

	// Get TC of receive SPI DMA
	ret = Dd_HDMAC1_Get_Trans_Count( D_BF_GYROIF_HDMAC1_CH_SPI_RECV, &tc_recv );
	if( ret != D_DDIM_OK ) {
		return FJ_ERR_NG;
	}

	*read_count = gBF_GyroIF_Read_Ctrl.data_ctrl.total_data_count - (tc_recv + 1);

#ifdef D_GYROIF_DEBUG
	tc_send = IO_HDMAC1.DMAC[D_BF_GYROIF_HDMAC1_CH_SPI_SEND].DMACA.bit.TC;
	tc_stmp = IO_HDMAC1.DMAC[D_BF_GYROIF_HDMAC1_CH_TMR64].DMACA.bit.TC;
	if( (tc_recv != tc_send) || (tc_recv != tc_stmp) || (tc_send != tc_stmp) ){
		BF_Debug_Print_Warning(("tc_recv=%d, tc_send=%d, tc_stmp=%d\n", tc_recv, tc_send, tc_stmp));
	}
#endif
#else
	*read_count = gBF_GyroIF_SPI_Recv_Count;
#endif

	return FJ_ERR_OK;
}

