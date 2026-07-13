/**
 * @file		sio.c
 * @brief		CSIO Controller
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>\n
 */

//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "fj_peripheral.h"
#include "sio.h"
#include "dd_csio.h"
#include "dd_uart.h"
#include "dd_gic.h"
#include "debug.h"
#include "os_user_custom.h"
#include "sdram_map_common.h"
#include "sdram_map_cache_define.h"
#include "l1l2cache.h"
#include "fj_host.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_SIO_CH_COUNT		(D_DD_USIO_CH_NUM_MAX)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Information for SIO channel control.
typedef struct {
	FJ_SIO_LENGTH	data_length;	// Data bit length
	SIO_CALLBACK	callback;		// Callback function pointer.
	BOOL			cs_restore_flg; // Chip select restore flag.
	FJ_GPIO_PORT	cs_port;		// Port name of GPIO for chip select. See @ref FJ_GPIO_PORT
	UCHAR			cs_status;		// Port status of GPIO for chip select.
	FJ_ERR_CODE		result;			// Callback result
} T_SIO_MNG;

/*----------------------------------------------------------------------*/
/* Callback Function Prototype                                          */
/*----------------------------------------------------------------------*/
static VOID sio_callback_ch0( INT32 result );
static VOID sio_callback_ch1( INT32 result );
static VOID sio_callback_ch2( INT32 result );
static VOID sio_callback_ch3( INT32 result );
static VOID sio_callback_ch4( INT32 result );
static VOID sio_callback_ch5( INT32 result );
static VOID sio_callback_ch6( INT32 result );
static VOID sio_callback_ch7( INT32 result );

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Information for SIO channel control.
static volatile T_SIO_MNG gSIO_Mng[D_SIO_CH_COUNT];

static const VP_CALLBACK gSIO_Cb_Func_Tbl[D_SIO_CH_COUNT] = {
	sio_callback_ch0,
	sio_callback_ch1,
	sio_callback_ch2,
	sio_callback_ch3,
	sio_callback_ch4,
	sio_callback_ch5,
	sio_callback_ch6,
	sio_callback_ch7
};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
// Nothing Special

/**
Callback function for CSIO interrupt.
*/
static VOID sio_callback( UCHAR ch, INT32 result )
{
	FJ_ERR_CODE err_code;
	INT32 ret;
	UCHAR cs_status = 1;

	/* Result converting */
	switch ( result ) {
		case D_DD_CSIO_NORMAL_COMPLETE:
		case D_DD_CSIO_FORCE_STOP:
			err_code = FJ_ERR_OK;
			break;

		case D_DD_CSIO_RECV_OVERRUN_ERROR:
			err_code = FJ_ERR_SIO_OVERRUN_ERROR;
			break;

		case D_DD_CSIO_DMA_SS_ERROR:
			err_code = FJ_ERR_SIO_DMA_ERROR;
			break;

		default:
			err_code = FJ_ERR_NG;
			break;
	}

	if( gSIO_Mng[ch].callback != NULL ) {
		if( gSIO_Mng[ch].cs_restore_flg ) {
			cs_status ^= gSIO_Mng[ch].cs_status;
			ret = FJ_Gpio_Set_Status( gSIO_Mng[ch].cs_port, cs_status );
			if( ret != FJ_ERR_OK ){
				BF_Debug_Print_Warning(("sio_callback() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));
			}
		}

		(*gSIO_Mng[ch].callback)( err_code );
	}
	else {
		gSIO_Mng[ch].result = err_code;
		OS_User_Set_Flg( FID_CSIO, (1 << ch) );
	}
}

static VOID sio_callback_ch0( INT32 result )
{
	sio_callback( 0, result );
}

static VOID sio_callback_ch1( INT32 result )
{
	sio_callback( 1, result );
}

static VOID sio_callback_ch2( INT32 result )
{
	sio_callback( 2, result );
}

static VOID sio_callback_ch3( INT32 result )
{
	sio_callback( 3, result );
}

static VOID sio_callback_ch4( INT32 result )
{
	sio_callback( 4, result );
}

static VOID sio_callback_ch5( INT32 result )
{
	sio_callback( 5, result );
}

static VOID sio_callback_ch6( INT32 result )
{
	sio_callback( 6, result );
}

static VOID sio_callback_ch7( INT32 result )
{
	sio_callback( 7, result );
}

static FJ_ERR_CODE sio_cs_on( FJ_PERI_CH ch )
{
	FJ_ERR_CODE ret;

	ret = FJ_Gpio_Set_Status( gSIO_Mng[ch].cs_port, gSIO_Mng[ch].cs_status );
	if( ret != FJ_ERR_OK ){
		BF_Debug_Print_Warning(("sio_cs_on() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

static VOID sio_cs_off( FJ_PERI_CH ch )
{
	FJ_ERR_CODE ret;
	UCHAR cs_status = 1;

	if( ( gSIO_Mng[ch].callback == NULL ) && ( gSIO_Mng[ch].cs_restore_flg ) ) {
		cs_status ^= gSIO_Mng[ch].cs_status;
		ret = FJ_Gpio_Set_Status( gSIO_Mng[ch].cs_port, cs_status );
		if( ret != FJ_ERR_OK ){
			BF_Debug_Print_Warning(("sio_cs_off() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));
		}
	}
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
/**
SIO Open API.
*/
FJ_ERR_CODE fj_sio_open( FJ_PERI_CH ch )
{
	INT32 ret;

	// Open CSIO
	ret = Dd_CSIO_Open( ch, D_DDIM_WAIT_END_TIME );
	if( ret != D_DDIM_OK ) {	// CSIO Open NG?
		BF_Debug_Print_Warning(("fj_sio_open() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));

		if( ret == D_DD_CSIO_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	return FJ_ERR_OK;
}

/**
SIO Close API.
*/
FJ_ERR_CODE fj_sio_close( FJ_PERI_CH ch )
{
	INT32 ret;

	// Close CSIO
	ret = Dd_CSIO_Close( ch );
	if( ret != D_DDIM_OK ) {	// CSIO Close NG?
		BF_Debug_Print_Warning(("fj_sio_close() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));

		if( ret == D_DD_CSIO_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	return FJ_ERR_OK;
}

/**
SIO Control API.
*/
FJ_ERR_CODE fj_sio_ctrl( FJ_PERI_CH ch, FJ_SIO_CTRL const* const sio_ctrl )
{
	INT32 ret;
	T_DD_CSIO_CAL  cal_tbl;
	T_DD_CSIO_CTRL csio_ctrl;
	T_DD_CSIO_FIFO_CTRL fifo_ctrl;

	csio_ctrl.fifo_ctrl = &fifo_ctrl;

	// Calculate baudrate
	cal_tbl.in_freq  = sio_ctrl->baudrate;
	cal_tbl.cal_freq = 0;
	ret = Dd_CSIO_Calculate( &cal_tbl );
	if( ret != D_DDIM_OK ) {
		BF_Debug_Print_Warning(("fj_sio_ctrl() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));
	}

	csio_ctrl.type					= (E_DD_CSIO_TYPE)sio_ctrl->type;				// Master/Slave
	csio_ctrl.mode					= (E_DD_CSIO_MODE)sio_ctrl->mode;				// Trasmit mode
	csio_ctrl.bit_direction 		= (E_DD_CSIO_BIT_DIR)sio_ctrl->bit_direction;	// Transferring direction, LSB first or MSB first
	csio_ctrl.data_length			= (E_DD_CSIO_DATA_LENGTH)sio_ctrl->data_length;	// Data bit length
	gSIO_Mng[ch].data_length		= sio_ctrl->data_length;
	csio_ctrl.baudrate				= cal_tbl.cal_freq;								// Baudrate (bps)
	csio_ctrl.sop					= 0;											// Serial output port setting(uninfluential)
	csio_ctrl.fifo_ctrl->fsel		= 0;											// FIFO1 is transfer, FIFO02 is receive
	csio_ctrl.fifo_ctrl->flste		= 0;											// Data-lost check permmision is disable
	csio_ctrl.fifo_ctrl->fset		= 0;											// FIFO transfer reload pointer storing is disable
	csio_ctrl.fifo_ctrl->fe1		= 1;											// FIFO01 is enable
	csio_ctrl.fifo_ctrl->fe2		= 1;											// FIFO02 is enable
	csio_ctrl.fifo_ctrl->fbyte_recv = 1;											// 1byte
	if(sio_ctrl->pcallback){
		csio_ctrl.pcallback			= gSIO_Cb_Func_Tbl[ch];
		gSIO_Mng[ch].callback		= sio_ctrl->pcallback;
	}
	else{
		csio_ctrl.pcallback			= NULL;
		gSIO_Mng[ch].callback		= NULL;
	}

	// Call CSIO driver
	ret = Dd_CSIO_Ctrl( ch, &csio_ctrl );
	if( ret == D_DD_CSIO_INPUT_PARAM_ERROR ) {	// parameter error?
		BF_Debug_Print_Warning(("fj_sio_ctrl() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	gSIO_Mng[ch].cs_restore_flg	= FALSE;
	gSIO_Mng[ch].cs_port		= FJ_GPIO_PORT_MAX;
	gSIO_Mng[ch].cs_status		= FJ_GPIO_STATUS_LOW;

	return FJ_ERR_OK;
}

/**
SIO Transfer API.
*/
FJ_ERR_CODE fj_sio_transfer( FJ_PERI_CH ch, FJ_SIO_CTRL_TRANSFER const* const trans_ctrl, FJ_USIO_DMA_CH dma_ch )
{
	INT32 ret;
	VOID* send_addr;
	VOID* recv_addr;

	gSIO_Mng[ch].cs_restore_flg	= trans_ctrl->cs_restore_flg;
	gSIO_Mng[ch].cs_port		= trans_ctrl->cs_port;
	gSIO_Mng[ch].cs_status		= trans_ctrl->cs_status;

	if( trans_ctrl->direction == FJ_SIO_DATA_DIR_SEND ){	// Send
		if( dma_ch == FJ_USIO_DMA_CH_NOT_USE ){		// CPU
			// Set send data
			ret = Dd_CSIO_Set_Send_Data( ch, trans_ctrl->data_ptr, trans_ctrl->data_count );
			if( ret == D_DD_CSIO_INPUT_PARAM_ERROR ) {	// parameter error?
				BF_Debug_Print_Warning(("fj_sio_transfer() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));
				return FJ_ERR_NG_INPUT_PARAM;
			}

			// Enable CSIO interrupt
			OS_User_EnableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + ch), (OS_USER_INTLV)D_DD_GIC_PRI14);

			// CS On
			sio_cs_on( ch );

			// Start send (CPU)
			ret = Dd_CSIO_Start_Send( ch );

			// CS Off
			sio_cs_off( ch );

			if( ret != D_DDIM_OK ) {
				BF_Debug_Print_Warning(("fj_sio_transfer() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));

				if( ret == D_DD_CSIO_INPUT_PARAM_ERROR ) {	// parameter error?
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
			ret = Dd_CSIO_Set_Send_Data( ch, send_addr, trans_ctrl->data_count );
			if( ret == D_DD_CSIO_INPUT_PARAM_ERROR ) {	// parameter error?
				BF_Debug_Print_Warning(("fj_sio_transfer() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));
				return FJ_ERR_NG_INPUT_PARAM;
			}

			// Disable CSIO interrupt
			OS_User_DisableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + ch));

			if( gSIO_Mng[ch].data_length == FJ_SIO_LENGTH_9 ){
				BF_L1l2cache_Clean_Flush_Addr( (UINT32)trans_ctrl->data_ptr, (sizeof(USHORT) * trans_ctrl->data_count) );
			}
			else {
				BF_L1l2cache_Clean_Flush_Addr( (UINT32)trans_ctrl->data_ptr, (sizeof(UCHAR) * trans_ctrl->data_count) );
			}

			// CS On
			sio_cs_on( ch );

			// Start send (DMA)
			ret = Dd_CSIO_Start_Send_DMA( ch, (UCHAR)dma_ch, D_DD_CSIO_SEND_DMA_MODE_DEMAND );

			// CS Off
			sio_cs_off( ch );

			if( ret != D_DDIM_OK ) {
				BF_Debug_Print_Warning(("fj_sio_transfer() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));

				if( ret == D_DD_CSIO_INPUT_PARAM_ERROR ) {	// parameter error?
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
			ret = Dd_CSIO_Set_Recv_Data( ch, trans_ctrl->data_ptr, trans_ctrl->data_count );
			if( ret == D_DD_CSIO_INPUT_PARAM_ERROR ) {	// parameter error?
				BF_Debug_Print_Warning(("fj_sio_transfer() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));
				return FJ_ERR_NG_INPUT_PARAM;
			}

			// Enable CSIO interrupt
			OS_User_EnableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + ch), (OS_USER_INTLV)D_DD_GIC_PRI14);

			// CS On
			sio_cs_on( ch );

			// Start receive (CPU)
			ret = Dd_CSIO_Start_Recv( ch );

			// CS Off
			sio_cs_off( ch );

			if( ret != D_DDIM_OK ) {
				BF_Debug_Print_Warning(("fj_sio_transfer() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));

				if( ret == D_DD_CSIO_INPUT_PARAM_ERROR ) {	// parameter error?
					return FJ_ERR_NG_INPUT_PARAM;
				}
				else if( ret == D_DD_CSIO_RECV_OVERRUN_ERROR ) {	// overrun error?
					return FJ_ERR_SIO_OVERRUN_ERROR;
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
			ret = Dd_CSIO_Set_Recv_Data( ch, recv_addr, trans_ctrl->data_count );
			if( ret == D_DD_CSIO_INPUT_PARAM_ERROR ) {	// parameter error?
				BF_Debug_Print_Warning(("fj_sio_transfer() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));
				return FJ_ERR_NG_INPUT_PARAM;
			}

			// Disable CSIO interrupt
			DDIM_User_DisableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + ch));

			if( gSIO_Mng[ch].data_length == FJ_SIO_LENGTH_9 ){
				BF_L1l2cache_Clean_Flush_Addr( (UINT32)trans_ctrl->data_ptr, (sizeof(USHORT) * trans_ctrl->data_count) );
			}
			else {
				BF_L1l2cache_Clean_Flush_Addr( (UINT32)trans_ctrl->data_ptr, (sizeof(UCHAR) * trans_ctrl->data_count) );
			}

			// CS On
			sio_cs_on( ch );

			// Start receive (DMA)
			ret = Dd_CSIO_Start_Recv_DMA( ch, (UCHAR)dma_ch, D_DD_CSIO_SEND_DMA_MODE_DEMAND );

			// CS Off
			sio_cs_off( ch );

			if( ret != D_DDIM_OK ) {
				BF_Debug_Print_Warning(("fj_sio_transfer() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));

				if( ret == D_DD_CSIO_INPUT_PARAM_ERROR ) {	// parameter error?
					return FJ_ERR_NG_INPUT_PARAM;
				}
				else if( ret == D_DD_CSIO_RECV_OVERRUN_ERROR ) {	// overrun error?
					return FJ_ERR_SIO_OVERRUN_ERROR;
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
SIO Stop API.
*/
FJ_ERR_CODE fj_sio_stop( FJ_PERI_CH ch )
{
	INT32 ret;

	// Stop CSIO
	ret = Dd_CSIO_Stop( ch );
	if( ret != D_DDIM_OK ) {	// CSIO Stop NG?
		BF_Debug_Print_Warning(("fj_sio_stop() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));

		if( ret == D_DD_CSIO_INPUT_PARAM_ERROR ) {	// parameter error?
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}

	return FJ_ERR_OK;
}

/**
SIO full duplex API.
*/
FJ_ERR_CODE fj_sio_full_duplex( FJ_PERI_CH ch, FJ_SIO_CTRL_FULL_DUPLEX const* const sio_ctrl )
{
	INT32 ret;
	T_DD_CSIO_CTRL csio_ctrl;
	T_DD_CSIO_FIFO_CTRL fifo_ctrl;
	OS_USER_FLGPTN flgptn;
	FJ_ERR_CODE result = FJ_ERR_OK;

	csio_ctrl.fifo_ctrl = &fifo_ctrl;

	gSIO_Mng[ch].cs_restore_flg	= sio_ctrl->cs_restore_flg;
	gSIO_Mng[ch].cs_port		= sio_ctrl->cs_port;
	gSIO_Mng[ch].cs_status		= sio_ctrl->cs_status;

	if( gSIO_Mng[ch].callback == NULL ) {
		Dd_CSIO_Get_Ctrl( ch, &csio_ctrl );
		csio_ctrl.pcallback = gSIO_Cb_Func_Tbl[ch];
		Dd_CSIO_Ctrl( ch, &csio_ctrl );
	}

	// Enable CSIO receive interrupt
	OS_User_EnableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + ch), (OS_USER_INTLV)D_DD_GIC_PRI14);
	// Enable CSIO send interrupt
	OS_User_EnableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + ch), (OS_USER_INTLV)D_DD_GIC_PRI14);

	// Set send data
	ret = Dd_CSIO_Set_Send_Data( ch, sio_ctrl->send_ptr, sio_ctrl->data_count );
	if( ret == D_DD_CSIO_INPUT_PARAM_ERROR ) {	// parameter error?
		BF_Debug_Print_Warning(("fj_sio_full_duplex() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Set receive buffer
	ret = Dd_CSIO_Set_Recv_Data( ch, sio_ctrl->recv_ptr, sio_ctrl->data_count );
	if( ret == D_DD_CSIO_INPUT_PARAM_ERROR ) {	// parameter error?
		BF_Debug_Print_Warning(("fj_sio_full_duplex() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// CS On
	sio_cs_on( ch );

	// Start full duplex communication
	ret = Dd_CSIO_Start_Full_Duplex( ch );
	if( ret == D_DD_CSIO_INPUT_PARAM_ERROR ) {	// parameter error?
		BF_Debug_Print_Warning(("fj_sio_full_duplex() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if( gSIO_Mng[ch].callback == NULL ) {
		OS_User_Wai_Flg( FID_CSIO, (1 << ch), D_OS_USER_TWF_ORW, &flgptn );
		OS_User_Clr_Flg( FID_CSIO, (1 << ch) );

		// CS Off
		sio_cs_off( ch );

		result = gSIO_Mng[ch].result;
	}

	return result;
}

/**
SIO full duplex API.(DMA transfer)
*/
FJ_ERR_CODE fj_sio_full_duplex_dma( FJ_PERI_CH ch, FJ_SIO_CTRL_FULL_DUPLEX const* const sio_ctrl, FJ_USIO_DMA_CH dma_ch_send, FJ_USIO_DMA_CH dma_ch_recv )
{
	INT32 ret;
	VOID* send_addr;
	VOID* recv_addr;
	T_DD_CSIO_CTRL csio_ctrl;
	T_DD_CSIO_FIFO_CTRL fifo_ctrl;
	OS_USER_FLGPTN flgptn;
	FJ_ERR_CODE result = FJ_ERR_OK;

	csio_ctrl.fifo_ctrl = &fifo_ctrl;

	gSIO_Mng[ch].cs_restore_flg	= sio_ctrl->cs_restore_flg;
	gSIO_Mng[ch].cs_port		= sio_ctrl->cs_port;
	gSIO_Mng[ch].cs_status		= sio_ctrl->cs_status;

	if( gSIO_Mng[ch].callback == NULL ) {
		Dd_CSIO_Get_Ctrl( ch, &csio_ctrl );
		csio_ctrl.pcallback = gSIO_Cb_Func_Tbl[ch];
		Dd_CSIO_Ctrl( ch, &csio_ctrl );
	}

	send_addr = (VOID*)M_SDRAM_MAP_COMMON_ADR_LOG2PHY((UINT32)sio_ctrl->send_ptr);
	recv_addr = (VOID*)M_SDRAM_MAP_COMMON_ADR_LOG2PHY((UINT32)sio_ctrl->recv_ptr);

	// Enable CSIO receive interrupt
	OS_User_DisableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_RECEPTION_INT + ch));

	// Enable CSIO send interrupt
	OS_User_DisableInt((OS_USER_INTID)(E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT + ch));

	if( gSIO_Mng[ch].data_length == FJ_SIO_LENGTH_9 ){
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)recv_addr, (sizeof(USHORT)* sio_ctrl->data_count) );
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)send_addr, (sizeof(USHORT)* sio_ctrl->data_count) );
	}
	else {
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)recv_addr, (sizeof(UCHAR)* sio_ctrl->data_count) );
		BF_L1l2cache_Clean_Flush_Addr( (UINT32)send_addr, (sizeof(UCHAR)* sio_ctrl->data_count) );
	}

	// Set send data
	ret = Dd_CSIO_Set_Send_Data( ch, send_addr, sio_ctrl->data_count );
	if( ret == D_DD_CSIO_INPUT_PARAM_ERROR ) {	// parameter error?
		BF_Debug_Print_Warning(("fj_sio_full_duplex_dma() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// Set receive buffer
	ret = Dd_CSIO_Set_Recv_Data( ch, recv_addr, sio_ctrl->data_count );
	if( ret == D_DD_CSIO_INPUT_PARAM_ERROR ) {	// parameter error?
		BF_Debug_Print_Warning(("fj_sio_full_duplex_dma() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	// CS On
	sio_cs_on( ch );

	// Start full duplex communication (DMA)
	ret = Dd_CSIO_Start_Full_Duplex_DMA( ch, dma_ch_send, dma_ch_recv );
	if( ret == D_DD_CSIO_INPUT_PARAM_ERROR ) {	// parameter error?
		BF_Debug_Print_Warning(("fj_sio_full_duplex_dma() - Error! : line=%d, ret=0x%x\n", __LINE__, ret));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if( gSIO_Mng[ch].callback == NULL ) {
		OS_User_Wai_Flg( FID_CSIO, (1 << ch), D_OS_USER_TWF_ORW, &flgptn );
		OS_User_Clr_Flg( FID_CSIO, (1 << ch) );

		// CS Off
		sio_cs_off( ch );

		result = gSIO_Mng[ch].result;
	}
	
	return result;
}

