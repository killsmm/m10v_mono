/**
 * @file		spi.c
 * @brief		SPI Controller
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2016 Socionext Inc.</I></B>\n
 */

//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "fj_peripheral.h"
#include "dd_spi.h"
#include "debug.h"
#include "os_user_custom.h"
#include "sdram_map_cache_define.h"
#include "l1l2cache.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_BF_SPI_CH_MAX		(3)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Information for SPI channel control.
typedef struct {
	FJ_SPI_LENGTH	data_length;	// Data bit length
	SPI_CALLBACK	callback;		// Callback function pointer.
} T_BF_SPI_MNG;

/*----------------------------------------------------------------------*/
/* Callback Function Prototype                                          */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Information for SPI channel control.
static volatile T_BF_SPI_MNG gBF_SPI_Mng[D_BF_SPI_CH_MAX];


/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
// Nothing Special

/**
Callback function for SPI interrupt.
*/
static VOID spi_callback( UCHAR ch, INT32 status )
{
	FJ_ERR_CODE err_code;

	/* Result converting */
	switch ( status ) {
		case D_DDIM_OK:
		case D_DD_SPI_FORCE_STOP:
			err_code = FJ_ERR_OK;
			break;

		case D_DD_SPI_RECV_OVERRUN_ERROR:
			err_code = FJ_ERR_SPI_OVERRUN_ERROR;
			break;

		case D_DD_SPI_DMA_SS_ERROR:
			err_code = FJ_ERR_SPI_DMA_ERROR;
			break;

		default:
			err_code = FJ_ERR_NG;
			break;
	}

	if( gBF_SPI_Mng[ch].callback != NULL ) {
		(*gBF_SPI_Mng[ch].callback)( err_code );
	}
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
SPI Open API.
*/
FJ_ERR_CODE fj_spi_open( FJ_PERI_CH ch )
{
	INT32 ret;

	if( ch >= D_BF_SPI_CH_MAX ){
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ch=%d\n", __LINE__, ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Open SPI
	ret = Dd_SPI_Open( ch, D_DDIM_WAIT_END_TIME );
	if( ret != D_DDIM_OK ) {	// SPI Open NG?
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));

		if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	return FJ_ERR_OK;
}

/**
SPI Close API.
*/
FJ_ERR_CODE fj_spi_close( FJ_PERI_CH ch )
{
	INT32 ret;

	if( ch >= D_BF_SPI_CH_MAX ){
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ch=%d\n", __LINE__, ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Close SPI
	ret = Dd_SPI_Close( ch );
	if( ret != D_DDIM_OK ) {	// SPI Close NG?
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));

		if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	return FJ_ERR_OK;
}

/**
SPI Control API.
*/
FJ_ERR_CODE fj_spi_ctrl( FJ_PERI_CH ch, FJ_SPI_CTRL const* const spi_ctrl )
{
	INT32 ret;
	T_DD_SPI_CTRL dd_spi_ctrl;
	UCHAR clk_div;

	if( ch >= D_BF_SPI_CH_MAX ){
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ch=%d\n", __LINE__, ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Calculate baudrate
	ret = Dd_SPI_Calculate( spi_ctrl->baudrate, &clk_div );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));
		if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	// Set Ctrl
	dd_spi_ctrl.type				= (E_DD_SPI_TYPE)spi_ctrl->type;
	dd_spi_ctrl.mode				= (E_DD_SPI_MODE)spi_ctrl->mode;
	dd_spi_ctrl.enable_sig			= E_DD_SPI_ENABLE_SIG_CPU;
	dd_spi_ctrl.bit_direction 		= (E_DD_SPI_BIT_DIR)spi_ctrl->bit_direction;
	dd_spi_ctrl.bit_length			= (E_DD_SPI_BIT_LEN)spi_ctrl->data_length;
	gBF_SPI_Mng[ch].data_length		= spi_ctrl->data_length;
	dd_spi_ctrl.clk_div				= clk_div;
	dd_spi_ctrl.delay_en			= 0;
	dd_spi_ctrl.delay_val			= 0;
	dd_spi_ctrl.rx_inhibit_en		= 0;
	dd_spi_ctrl.fifo_wmark_tx		= spi_ctrl->fifo_wmark_tx;
	dd_spi_ctrl.fifo_wmark_rx		= spi_ctrl->fifo_wmark_rx;
	dd_spi_ctrl.dma_en				= spi_ctrl->dma_en;
	dd_spi_ctrl.dma_recv_to			= spi_ctrl->dma_recv_to;
	dd_spi_ctrl.ss_info.cont_trans	= E_DD_SPI_CONT_TRANS_INACT_FIFO_EMPTY;
	dd_spi_ctrl.ss_info.ssout[0]	= 0;
	dd_spi_ctrl.ss_info.ssout[1]	= 0;
	dd_spi_ctrl.ss_info.ssout[2]	= 0;
	dd_spi_ctrl.ss_info.ssout[3]	= 0;
	dd_spi_ctrl.ss_info.sspol[0]	= 0;
	dd_spi_ctrl.ss_info.sspol[1]	= 0;
	dd_spi_ctrl.ss_info.sspol[2]	= 0;
	dd_spi_ctrl.ss_info.sspol[3]	= 0;
	if( spi_ctrl->pcallback ){
		dd_spi_ctrl.pcallback		= (VP_SPI_CALLBACK)spi_callback;
		gBF_SPI_Mng[ch].callback	= spi_ctrl->pcallback;
	}
	else {
		dd_spi_ctrl.pcallback		= NULL;
		gBF_SPI_Mng[ch].callback	= NULL;
	}
	dd_spi_ctrl.pcallback_ss		= NULL;

	// Call SPI driver
	ret = Dd_SPI_Ctrl( ch, &dd_spi_ctrl );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));
		if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	return FJ_ERR_OK;
}

/**
SPI Transfer API.
*/
FJ_ERR_CODE fj_spi_transfer( FJ_PERI_CH ch, FJ_SPI_CTRL_TRANSFER const* const trans_ctrl, FJ_USIO_DMA_CH dma_ch )
{
	INT32 ret;
	T_DD_SPI_SS_INFO ss_info;
	VOID* send_addr;
	VOID* recv_addr;

	if( ch >= D_BF_SPI_CH_MAX ){
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ch=%d\n", __LINE__, ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	ss_info.cont_trans	= trans_ctrl->ss_info.cont_trans;
	ss_info.ssout[0]	= trans_ctrl->ss_info.ssout[0];
	ss_info.ssout[1]	= trans_ctrl->ss_info.ssout[1];
	ss_info.ssout[2]	= trans_ctrl->ss_info.ssout[2];
	ss_info.ssout[3]	= trans_ctrl->ss_info.ssout[3];
	ss_info.sspol[0]	= trans_ctrl->ss_info.sspol[0];
	ss_info.sspol[1]	= trans_ctrl->ss_info.sspol[1];
	ss_info.sspol[2]	= trans_ctrl->ss_info.sspol[2];
	ss_info.sspol[3]	= trans_ctrl->ss_info.sspol[3];

	// Set Slave Select setting
	ret = Dd_SPI_Set_Slave_Select( ch, &ss_info );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));
		if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	if( trans_ctrl->direction == FJ_SPI_DATA_DIR_SEND ){	// Send
		if( dma_ch == FJ_USIO_DMA_CH_NOT_USE ){		// CPU
			// Set send data
			ret = Dd_SPI_Set_Send_Data( ch, trans_ctrl->data_ptr, trans_ctrl->data_count );
			if( ret != D_DDIM_OK ) {
				BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));
				if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
					return FJ_ERR_NG_INPUT_PARAM;
				}
				else {
					return FJ_ERR_NG;
				}
			}

			// Start send (CPU)
			ret = Dd_SPI_Start_Send( ch );
			if( ret != D_DDIM_OK ) {
				BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));

				if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
					return FJ_ERR_NG_INPUT_PARAM;
				}
				else {
					return FJ_ERR_NG;
				}
			}
		}
		else {	// DMA
			unsigned tmp = (unsigned)trans_ctrl->data_ptr;
			send_addr = (VOID*)M_SDRAM_MAP_COMMON_ADR_LOG2PHY(tmp);

			// Set send data
			ret = Dd_SPI_Set_Send_Data( ch, send_addr, trans_ctrl->data_count );
			if( ret != D_DDIM_OK ) {
				BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));
				if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
					return FJ_ERR_NG_INPUT_PARAM;
				}
				else {
					return FJ_ERR_NG;
				}
			}

			if( gBF_SPI_Mng[ch].data_length > FJ_SPI_LENGTH_16 ){
				BF_L1l2cache_Clean_Flush_Addr( (UINT32)trans_ctrl->data_ptr, (sizeof(UINT32) * trans_ctrl->data_count) );
			}
			else if( gBF_SPI_Mng[ch].data_length > FJ_SPI_LENGTH_8 ){
				BF_L1l2cache_Clean_Flush_Addr( (UINT32)trans_ctrl->data_ptr, (sizeof(USHORT) * trans_ctrl->data_count) );
			}
			else {
				BF_L1l2cache_Clean_Flush_Addr( (UINT32)trans_ctrl->data_ptr, (sizeof(UCHAR) * trans_ctrl->data_count) );
			}

			// Start send (DMA)
			ret = Dd_SPI_Start_Send_DMA( ch, (UCHAR)dma_ch );

			if( ret != D_DDIM_OK ) {
				BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));

				if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
					return FJ_ERR_NG_INPUT_PARAM;
				}
				else {
					return FJ_ERR_NG;
				}
			}
		}
	}
	else {	// Receive
		if( dma_ch == FJ_USIO_DMA_CH_NOT_USE ){		// CPU
			// Set receive buffer
			ret = Dd_SPI_Set_Recv_Data( ch, trans_ctrl->data_ptr, trans_ctrl->data_count );
			if( ret != D_DDIM_OK ) {
				BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));
				if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
					return FJ_ERR_NG_INPUT_PARAM;
				}
				else {
					return FJ_ERR_NG;
				}
			}

			// Start receive (CPU)
			ret = Dd_SPI_Start_Recv( ch );

			if( ret != D_DDIM_OK ) {
				BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));

				if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
					return FJ_ERR_NG_INPUT_PARAM;
				}
				else if( ret == D_DD_SPI_RECV_OVERRUN_ERROR ) {	// overrun error?
					return FJ_ERR_SPI_OVERRUN_ERROR;
				}
				else {
					return FJ_ERR_NG;
				}
			}
		}
		else {	// DMA
			unsigned tmp = (unsigned)trans_ctrl->data_ptr;
			recv_addr = (VOID*)M_SDRAM_MAP_COMMON_ADR_LOG2PHY(tmp);

			// Set receive buffer
			ret = Dd_SPI_Set_Recv_Data( ch, recv_addr, trans_ctrl->data_count );
			if( ret != D_DDIM_OK ) {
				BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));
				if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
					return FJ_ERR_NG_INPUT_PARAM;
				}
				else {
					return FJ_ERR_NG;
				}
			}

			if( gBF_SPI_Mng[ch].data_length > FJ_SPI_LENGTH_16 ){
				BF_L1l2cache_Clean_Flush_Addr( (UINT32)trans_ctrl->data_ptr, (sizeof(UINT32) * trans_ctrl->data_count) );
			}
			else if( gBF_SPI_Mng[ch].data_length > FJ_SPI_LENGTH_8 ){
				BF_L1l2cache_Clean_Flush_Addr( (UINT32)trans_ctrl->data_ptr, (sizeof(USHORT) * trans_ctrl->data_count) );
			}
			else {
				BF_L1l2cache_Clean_Flush_Addr( (UINT32)trans_ctrl->data_ptr, (sizeof(UCHAR) * trans_ctrl->data_count) );
			}

			// Start receive (DMA)
			ret = Dd_SPI_Start_Recv_DMA( ch, (UCHAR)dma_ch );

			if( ret != D_DDIM_OK ) {
				BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));

				if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
					return FJ_ERR_NG_INPUT_PARAM;
				}
				else if( ret == D_DD_SPI_RECV_OVERRUN_ERROR ) {	// overrun error?
					return FJ_ERR_SPI_OVERRUN_ERROR;
				}
				else {
					return FJ_ERR_NG;
				}
			}
		}
	}

	return FJ_ERR_OK;
}

/**
SPI Stop API.
*/
FJ_ERR_CODE fj_spi_stop( FJ_PERI_CH ch )
{
	INT32 ret;

	if( ch >= D_BF_SPI_CH_MAX ){
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ch=%d\n", __LINE__, ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Stop SPI
	ret = Dd_SPI_Stop( ch );
	if( ret != D_DDIM_OK ) {	// SPI Stop NG?
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));

		if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	return FJ_ERR_OK;
}

/**
SPI full duplex API.
*/
FJ_ERR_CODE fj_spi_full_duplex( FJ_PERI_CH ch, FJ_SPI_CTRL_FULL_DUPLEX const* const spi_ctrl )
{
	INT32 ret;
	T_DD_SPI_SS_INFO ss_info;

	if( ch >= D_BF_SPI_CH_MAX ){
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ch=%d\n", __LINE__, ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	ss_info.cont_trans	= spi_ctrl->ss_info.cont_trans;
	ss_info.ssout[0]	= spi_ctrl->ss_info.ssout[0];
	ss_info.ssout[1]	= spi_ctrl->ss_info.ssout[1];
	ss_info.ssout[2]	= spi_ctrl->ss_info.ssout[2];
	ss_info.ssout[3]	= spi_ctrl->ss_info.ssout[3];
	ss_info.sspol[0]	= spi_ctrl->ss_info.sspol[0];
	ss_info.sspol[1]	= spi_ctrl->ss_info.sspol[1];
	ss_info.sspol[2]	= spi_ctrl->ss_info.sspol[2];
	ss_info.sspol[3]	= spi_ctrl->ss_info.sspol[3];

	// Set Slave Select setting
	ret = Dd_SPI_Set_Slave_Select( ch, &ss_info );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));

		if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	// Set send data
	ret = Dd_SPI_Set_Send_Data( ch, spi_ctrl->send_ptr, spi_ctrl->data_count );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));
		if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	// Set receive buffer
	ret = Dd_SPI_Set_Recv_Data( ch, spi_ctrl->recv_ptr, spi_ctrl->data_count );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));
		if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	// Start full duplex communication
	ret = Dd_SPI_Start_Full_Duplex( ch );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));

		if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else if( ret == D_DD_SPI_RECV_OVERRUN_ERROR ) {	// overrun error?
			return FJ_ERR_SPI_OVERRUN_ERROR;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	return FJ_ERR_OK;
}

/**
SPI full duplex API.(DMA transfer)
*/
FJ_ERR_CODE fj_spi_full_duplex_dma( FJ_PERI_CH ch, FJ_SPI_CTRL_FULL_DUPLEX const* const spi_ctrl, FJ_USIO_DMA_CH dma_ch_send, FJ_USIO_DMA_CH dma_ch_recv )
{
	INT32 ret;
	VOID* send_addr;
	VOID* recv_addr;
	T_DD_SPI_SS_INFO ss_info;

	if( ch >= D_BF_SPI_CH_MAX ){
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ch=%d\n", __LINE__, ch));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	ss_info.cont_trans	= spi_ctrl->ss_info.cont_trans;
	ss_info.ssout[0]	= spi_ctrl->ss_info.ssout[0];
	ss_info.ssout[1]	= spi_ctrl->ss_info.ssout[1];
	ss_info.ssout[2]	= spi_ctrl->ss_info.ssout[2];
	ss_info.ssout[3]	= spi_ctrl->ss_info.ssout[3];
	ss_info.sspol[0]	= spi_ctrl->ss_info.sspol[0];
	ss_info.sspol[1]	= spi_ctrl->ss_info.sspol[1];
	ss_info.sspol[2]	= spi_ctrl->ss_info.sspol[2];
	ss_info.sspol[3]	= spi_ctrl->ss_info.sspol[3];

	// Set Slave Select setting
	ret = Dd_SPI_Set_Slave_Select( ch, &ss_info );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));
		if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	send_addr = (VOID*)M_SDRAM_MAP_COMMON_ADR_LOG2PHY((UINT32)spi_ctrl->send_ptr);
	recv_addr = (VOID*)M_SDRAM_MAP_COMMON_ADR_LOG2PHY((UINT32)spi_ctrl->recv_ptr);

	if( gBF_SPI_Mng[ch].data_length > FJ_SPI_LENGTH_16 ){
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)recv_addr, (sizeof(UINT32)* spi_ctrl->data_count) );
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)send_addr, (sizeof(UINT32)* spi_ctrl->data_count) );
	}
	else if( gBF_SPI_Mng[ch].data_length > FJ_SPI_LENGTH_8 ){
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)recv_addr, (sizeof(USHORT)* spi_ctrl->data_count) );
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)send_addr, (sizeof(USHORT)* spi_ctrl->data_count) );
	}
	else {
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)recv_addr, (sizeof(UCHAR)* spi_ctrl->data_count) );
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)send_addr, (sizeof(UCHAR)* spi_ctrl->data_count) );
	}

	// Set send data
	ret = Dd_SPI_Set_Send_Data( ch, send_addr, spi_ctrl->data_count );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));
		if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	// Set receive buffer
	ret = Dd_SPI_Set_Recv_Data( ch, recv_addr, spi_ctrl->data_count );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));
		if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	// Start full duplex communication (DMA)
	ret = Dd_SPI_Start_Full_Duplex_DMA( ch, dma_ch_send, dma_ch_recv );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("FJ_SPI Error! : line=%d, ret=0x%x\n", __LINE__, ret));

		if( ret == D_DD_SPI_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else if( ret == D_DD_SPI_RECV_OVERRUN_ERROR ) {	// overrun error?
			return FJ_ERR_SPI_OVERRUN_ERROR;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	return FJ_ERR_OK;
}

