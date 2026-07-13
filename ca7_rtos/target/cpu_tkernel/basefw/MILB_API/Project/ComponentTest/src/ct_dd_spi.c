/**
 * @file		ct_dd_spi.c
 * @brief		This is ct code for dd_spi
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "stdlib.h"
#include "string.h"
#include "ct_dd_spi.h"
#include "spi.h"
#include "dd_spi.h"
#include "dd_top.h"
#include "peripheral.h"
#include "dd_hdmac1.h"
#include "dd_tmr32.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_CT_DD_SPI_CH_MAX					(3)

#define D_CT_DD_SPI_DMA_CH_1				(0)
#define D_CT_DD_SPI_DMA_CH_2				(1)
#define D_CT_DD_SPI_TIMER_CH				(7)

#define D_CT_DD_SPI_TIMER_MODE_DMA			(0)
#define D_CT_DD_SPI_TIMER_MODE_STOP			(1)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_CT_DD_SPI_TRNAS_TYPE_SEND = 0,
	E_CT_DD_SPI_TRNAS_TYPE_RECV,
	E_CT_DD_SPI_TRNAS_TYPE_FULL,
} E_CT_DD_SPI_TRNAS_TYPE;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
#ifdef CO_DEBUG_ON_PC
static UCHAR  gDD_CT_SPI_Send_Buf8[262144];
static USHORT gDD_CT_SPI_Send_Buf16[262144];
static UINT32 gDD_CT_SPI_Send_Buf32[262144];
static UCHAR  gDD_CT_SPI_Recv_Buf8[262144];
static USHORT gDD_CT_SPI_Recv_Buf16[262144];
static UINT32 gDD_CT_SPI_Recv_Buf32[262144];
#else
static UCHAR  gDD_CT_SPI_Send_Buf8[32];
static USHORT gDD_CT_SPI_Send_Buf16[32];
static UINT32 gDD_CT_SPI_Send_Buf32[32];
static UCHAR  gDD_CT_SPI_Recv_Buf8[32];
static USHORT gDD_CT_SPI_Recv_Buf16[32];
static UINT32 gDD_CT_SPI_Recv_Buf32[32];
#endif

static UCHAR gDD_CT_SPI_FIFO_Depth[D_CT_DD_SPI_CH_MAX] = {128, 64, 16};

static E_CT_DD_SPI_TRNAS_TYPE gCT_DD_SPI_Trans_Type = E_CT_DD_SPI_TRNAS_TYPE_SEND;
static UINT32 gCT_DD_SPI_Trans_Num;
static E_DD_SPI_BIT_LEN gCT_DD_SPI_Bit_Len;

static UCHAR gCT_DD_SPI_Async;
static ULONG gDD_CT_SPI_Send_Data_Addr;
static ULONG gDD_CT_SPI_Recv_Data_Addr;

static INT32 gDD_CT_SPI_Result = D_DDIM_OK;

static INT32 gDD_CT_SPI_Timer_Mode = 0;
static UCHAR gDD_CT_SPI_Timer_Num = 0;

static UCHAR gDD_CT_SPI_Ch = 0;


static VOID ct_dd_spi_set_timer( UCHAR spi_ch, ULONG msec, UCHAR num, INT32 mode );

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static VOID ct_dd_spi_print_send_data( VOID )
{
	INT32 index;

	if( !gDD_CT_SPI_Send_Data_Addr ){
		Ddim_Print(("---Sned data---\n"));
		if( gCT_DD_SPI_Bit_Len > E_DD_SPI_BIT_LEN_16 ){
			for( index = 0; index < gCT_DD_SPI_Trans_Num; index++ ){
				Ddim_Print(("gDD_CT_SPI_Send_Buf32[%d] = 0x%08X\n", index, gDD_CT_SPI_Send_Buf32[index]));
			}
		}
		else if( gCT_DD_SPI_Bit_Len > E_DD_SPI_BIT_LEN_8 ){
			for( index = 0; index < gCT_DD_SPI_Trans_Num; index++ ){
				Ddim_Print(("gDD_CT_SPI_Send_Buf16[%d] = 0x%04X\n", index, gDD_CT_SPI_Send_Buf16[index]));
			}
		}
		else {
			for( index = 0; index < gCT_DD_SPI_Trans_Num; index++ ){
				Ddim_Print(("gDD_CT_SPI_Send_Buf8[%d] = 0x%02X\n", index, gDD_CT_SPI_Send_Buf8[index]));
			}
		}
	}
}

static VOID ct_dd_spi_print_recv_data( VOID )
{
	INT32 index;

	if( !gDD_CT_SPI_Recv_Data_Addr ){
		Ddim_Print(("---Receive data---\n"));
		if( gCT_DD_SPI_Bit_Len > E_DD_SPI_BIT_LEN_16 ){
			for( index = 0; index < gCT_DD_SPI_Trans_Num; index++ ){
				Ddim_Print(("gDD_CT_SPI_Recv_Buf32[%d] = 0x%08X\n", index, gDD_CT_SPI_Recv_Buf32[index]));
			}
		}
		else if( gCT_DD_SPI_Bit_Len > E_DD_SPI_BIT_LEN_8 ){
			for( index = 0; index < gCT_DD_SPI_Trans_Num; index++ ){
				Ddim_Print(("gDD_CT_SPI_Recv_Buf16[%d] = 0x%04X\n", index, gDD_CT_SPI_Recv_Buf16[index]));
			}
		}
		else {
			for( index = 0; index < gCT_DD_SPI_Trans_Num; index++ ){
				Ddim_Print(("gDD_CT_SPI_Recv_Buf8[%d] = 0x%02X\n", index, gDD_CT_SPI_Recv_Buf8[index]));
			}
		}
	}
}

static VOID ct_dd_spi_print_data( VOID )
{
	if( gCT_DD_SPI_Trans_Type == E_CT_DD_SPI_TRNAS_TYPE_SEND ){
		ct_dd_spi_print_send_data();
	}
	else if( gCT_DD_SPI_Trans_Type == E_CT_DD_SPI_TRNAS_TYPE_RECV ){
		ct_dd_spi_print_recv_data();
	}
	else {	// E_CT_DD_SPI_TRNAS_TYPE_FULL
		ct_dd_spi_print_send_data();
		ct_dd_spi_print_recv_data();
	}
}

static VOID ct_dd_spi_print_ctrl( UCHAR ch, T_DD_SPI_CTRL const* const spi_ctrl )
{
	Ddim_Print(("SPI type               = %d\n", spi_ctrl->type));
	Ddim_Print(("SPI mode               = %d\n", spi_ctrl->mode));
	Ddim_Print(("SPI enable_sig         = %d\n", spi_ctrl->enable_sig));
	Ddim_Print(("SPI bit_direction      = %d\n", spi_ctrl->bit_direction));
	Ddim_Print(("SPI bit_length         = %d\n", spi_ctrl->bit_length));
	Ddim_Print(("SPI clk_div            = %d\n", spi_ctrl->clk_div));
	Ddim_Print(("SPI delay_en           = %d\n", spi_ctrl->delay_en));
	Ddim_Print(("SPI delay_val          = %d\n", spi_ctrl->delay_val));
	Ddim_Print(("SPI rx_inhibit_en      = %d\n", spi_ctrl->rx_inhibit_en));
	Ddim_Print(("SPI fifo_wmark_tx      = %d\n", spi_ctrl->fifo_wmark_tx));
	Ddim_Print(("SPI fifo_wmark_rx      = %d\n", spi_ctrl->fifo_wmark_rx));
	Ddim_Print(("SPI dma_en             = %d\n", spi_ctrl->dma_en));
	Ddim_Print(("SPI dma_recv_to        = %d\n", spi_ctrl->dma_recv_to));
	Ddim_Print(("SPI ss_info.cont_trans = %d\n", spi_ctrl->ss_info.cont_trans));
	Ddim_Print(("SPI ss_info.ssout[0]   = %d\n", spi_ctrl->ss_info.ssout[0]));
	Ddim_Print(("SPI ss_info.ssout[1]   = %d\n", spi_ctrl->ss_info.ssout[1]));
	Ddim_Print(("SPI ss_info.ssout[2]   = %d\n", spi_ctrl->ss_info.ssout[2]));
	Ddim_Print(("SPI ss_info.ssout[3]   = %d\n", spi_ctrl->ss_info.ssout[3]));
	Ddim_Print(("SPI ss_info.sspol[0]   = %d\n", spi_ctrl->ss_info.sspol[0]));
	Ddim_Print(("SPI ss_info.sspol[1]   = %d\n", spi_ctrl->ss_info.sspol[1]));
	Ddim_Print(("SPI ss_info.sspol[2]   = %d\n", spi_ctrl->ss_info.sspol[2]));
	Ddim_Print(("SPI ss_info.sspol[3]   = %d\n", spi_ctrl->ss_info.sspol[3]));
	Ddim_Print(("SPI pcallback          = 0x%08X\n", (UINT32)spi_ctrl->pcallback));
	Ddim_Print(("SPI pcallback_ss       = 0x%08X\n", (UINT32)spi_ctrl->pcallback_ss));
}

static VOID ct_dd_spi_print_ctrl_reg( UCHAR ch )
{
	Ddim_Print(("CTRL.bit.MASTER             = %d\n", IO_SPI[ch].CTRL.bit.MASTER));
	Ddim_Print(("CTRL.bit.CPOL               = %d\n", IO_SPI[ch].CTRL.bit.CPOL));
	Ddim_Print(("CTRL.bit.CPHA               = %d\n", IO_SPI[ch].CTRL.bit.CPHA));
	Ddim_Print(("EN.bit.__EXTENSEL           = %d\n", IO_SPI[ch].EN.bit.__EXTENSEL));
	Ddim_Print(("CTRL.bit.MSB1ST             = %d\n", IO_SPI[ch].CTRL.bit.MSB1ST));
	Ddim_Print(("AUX_CTRL.bit.BITSIZE        = %d\n", IO_SPI[ch].AUX_CTRL.bit.BITSIZE));
	Ddim_Print(("CTRL.bit.DIVENABLE          = %d\n", IO_SPI[ch].CTRL.bit.DIVENABLE));
	Ddim_Print(("DIV.bit.DIVISOR             = %d\n", IO_SPI[ch].DIV.bit.DIVISOR));
	Ddim_Print(("CTRL.bit.MWAITEN            = %d\n", IO_SPI[ch].CTRL.bit.MWAITEN));
	Ddim_Print(("MS_DLY.bit.MWAIT            = %d\n", IO_SPI[ch].MS_DLY.bit.MWAIT));
	Ddim_Print(("AUX_CTRL.bit.INHIBITDIN     = %d\n", IO_SPI[ch].AUX_CTRL.bit.INHIBITDIN));
	Ddim_Print(("FIFO_WMK.bit.TXWMARKSET     = %d\n", IO_SPI[ch].FIFO_WMK.bit.TXWMARKSET));
	Ddim_Print(("FIFO_WMK.bit.RXWMARKSET     = %d\n", IO_SPI[ch].FIFO_WMK.bit.RXWMARKSET));
	Ddim_Print(("CTRL.bit.DMA                = %d\n", IO_SPI[ch].CTRL.bit.DMA));
	Ddim_Print(("DMA_TO.bit.TIMEOUT          = %d\n", IO_SPI[ch].DMA_TO.bit.TIMEOUT));
	Ddim_Print(("CTRL.bit.CONTXFER           = %d\n", IO_SPI[ch].CTRL.bit.CONTXFER));
	Ddim_Print(("AUX_CTRL.bit.CONTXFEREXTEND = %d\n", IO_SPI[ch].AUX_CTRL.bit.CONTXFEREXTEND));
	Ddim_Print(("SLV_SEL.bit.SSOUT0          = %d\n", IO_SPI[ch].SLV_SEL.bit.SSOUT0));
	Ddim_Print(("SLV_SEL.bit.SSOUT1          = %d\n", IO_SPI[ch].SLV_SEL.bit.SSOUT1));
	Ddim_Print(("SLV_SEL.bit.SSOUT2          = %d\n", IO_SPI[ch].SLV_SEL.bit.SSOUT2));
	Ddim_Print(("SLV_SEL.bit.SSOUT3          = %d\n", IO_SPI[ch].SLV_SEL.bit.SSOUT3));
	Ddim_Print(("SLV_POL.bit.SSPOL0          = %d\n", IO_SPI[ch].SLV_POL.bit.SSPOL0));
	Ddim_Print(("SLV_POL.bit.SSPOL1          = %d\n", IO_SPI[ch].SLV_POL.bit.SSPOL1));
	Ddim_Print(("SLV_POL.bit.SSPOL2          = %d\n", IO_SPI[ch].SLV_POL.bit.SSPOL2));
	Ddim_Print(("SLV_POL.bit.SSPOL3          = %d\n", IO_SPI[ch].SLV_POL.bit.SSPOL3));
	Ddim_Print(("AUX_CTRL.bit.SPIMODE        = %d\n", IO_SPI[ch].AUX_CTRL.bit.SPIMODE));
	Ddim_Print(("AUX_CTRL.bit.XFERFORMAT     = %d\n", IO_SPI[ch].AUX_CTRL.bit.XFERFORMAT));
	Ddim_Print(("EN.bit.__ENABLEREQ          = %d\n", IO_SPI[ch].EN.bit.__ENABLEREQ));
	Ddim_Print(("INT_CLR.word                = 0x%08lX\n", IO_SPI[ch].INT_CLR.word));
	Ddim_Print(("INT_EN..word                = 0x%08lX\n", IO_SPI[ch].INT_EN.word));
}

static VOID ct_dd_spi_print_send_reg( UCHAR ch )
{
	Ddim_Print(("EN.bit.__ENABLEREQ = %d\n", IO_SPI[ch].EN.bit.__ENABLEREQ));
	Ddim_Print(("INT_EN.word        = 0x%08lX\n", IO_SPI[ch].INT_EN.word));
	Ddim_Print(("TXDATA             = 0x%04lX\n", IO_SPI[ch].TXDATA));
}

static VOID ct_dd_spi_print_recv_reg( UCHAR ch )
{
	Ddim_Print(("EN.bit.__ENABLEREQ      = %d\n", IO_SPI[ch].EN.bit.__ENABLEREQ));
	Ddim_Print(("INT_EN.word             = 0x%08lX\n", IO_SPI[ch].INT_EN.word));
	Ddim_Print(("FIFO_WMK.bit.RXWMARKSET = %d\n", IO_SPI[ch].FIFO_WMK.bit.RXWMARKSET));
	Ddim_Print(("TX_DWR.word             = 0x%04lX\n", IO_SPI[ch].TX_DWR.word));
}

static VOID ct_dd_spi_print_full_reg( UCHAR ch )
{
	Ddim_Print(("EN.bit.__ENABLEREQ      = %d\n", IO_SPI[ch].EN.bit.__ENABLEREQ));
	Ddim_Print(("INT_EN.word             = 0x%08lX\n", IO_SPI[ch].INT_EN.word));
	Ddim_Print(("TXDATA                  = 0x%04lX\n", IO_SPI[ch].TXDATA));
	Ddim_Print(("FIFO_WMK.bit.RXWMARKSET = %d\n", IO_SPI[ch].FIFO_WMK.bit.RXWMARKSET));
}

static VOID ct_dd_spi_print_dma_reg( UCHAR ch, UCHAR dma_ch )
{
	Ddim_Print(("EN.bit.__ENABLEREQ      = %d\n", IO_SPI[ch].EN.bit.__ENABLEREQ));
	Ddim_Print(("INT_EN.word             = 0x%08lX\n", IO_SPI[ch].INT_EN.word));
	Ddim_Print(("DMACA.bit.EB            = %d\n", IO_HDMAC1.DMAC[dma_ch].DMACA.bit.EB));
	Ddim_Print(("DMACA.bit.IS            = 0x%02X\n", IO_HDMAC1.DMAC[dma_ch].DMACA.bit.IS));
	Ddim_Print(("DMACB.bit.TW            = %d\n", IO_HDMAC1.DMAC[dma_ch].DMACB.bit.TW));
	Ddim_Print(("DMACA.bit.TC            = %d\n", IO_HDMAC1.DMAC[dma_ch].DMACA.bit.TC));
	Ddim_Print(("DMACB.bit.FD            = %d\n", IO_HDMAC1.DMAC[dma_ch].DMACB.bit.FD));
	Ddim_Print(("DMACB.bit.FS            = %d\n", IO_HDMAC1.DMAC[dma_ch].DMACB.bit.FS));
	Ddim_Print(("DMACB.bit.RC            = %d\n", IO_HDMAC1.DMAC[dma_ch].DMACB.bit.RC));
}

static VOID ct_dd_spi_print_dma_full_reg( UCHAR ch, UCHAR send_dma_ch, UCHAR recv_dma_ch )
{
	Ddim_Print(("EN.bit.__ENABLEREQ      = %d\n", IO_SPI[ch].EN.bit.__ENABLEREQ));
	Ddim_Print(("INT_EN.word             = 0x%08lX\n", IO_SPI[ch].INT_EN.word));
	Ddim_Print(("DMACA.bit.EB(send)      = %d\n", IO_HDMAC1.DMAC[send_dma_ch].DMACA.bit.EB));
	Ddim_Print(("DMACA.bit.IS(send)      = 0x%02X\n", IO_HDMAC1.DMAC[send_dma_ch].DMACA.bit.IS));
	Ddim_Print(("DMACA.bit.TC(send)      = %d\n", IO_HDMAC1.DMAC[send_dma_ch].DMACA.bit.TC));
	Ddim_Print(("DMACB.bit.RC(send)      = %d\n", IO_HDMAC1.DMAC[send_dma_ch].DMACB.bit.RC));
	Ddim_Print(("DMACA.bit.EB(recv)      = %d\n", IO_HDMAC1.DMAC[recv_dma_ch].DMACA.bit.EB));
	Ddim_Print(("DMACA.bit.IS(recv)      = 0x%02X\n", IO_HDMAC1.DMAC[recv_dma_ch].DMACA.bit.IS));
	Ddim_Print(("DMACA.bit.TC(recv)      = %d\n", IO_HDMAC1.DMAC[recv_dma_ch].DMACA.bit.TC));
	Ddim_Print(("DMACB.bit.RC(recv)      = %d\n", IO_HDMAC1.DMAC[recv_dma_ch].DMACB.bit.RC));
}
static VOID ct_dd_spi_print_ss( UCHAR ch )
{
	Ddim_Print(("CTRL.bit.CONTXFER           = %d\n", IO_SPI[ch].CTRL.bit.CONTXFER));
	Ddim_Print(("AUX_CTRL.bit.CONTXFEREXTEND = %d\n", IO_SPI[ch].AUX_CTRL.bit.CONTXFEREXTEND));
}

static VOID ct_dd_spi_callback( UCHAR ch, INT32 result )
{
	Ddim_Print(("SPI[%d] Callback. result=0x%08X\n", ch, result));

	if( result == D_DDIM_OK ){
		if( gCT_DD_SPI_Async ){
			ct_dd_spi_print_data();
		}
	}

	gDD_CT_SPI_Result = result;
}

static VOID ct_dd_spi_callback_ss( UCHAR ch )
{
	Ddim_Print(("SPI[%d] Callback of SSOUT/IN\n", ch));
}

static VOID ct_dd_spi_callback_timer( VOID )
{
	if( gDD_CT_SPI_Timer_Mode == D_CT_DD_SPI_TIMER_MODE_DMA )
		IO_HDMAC1.DMAC[D_CT_DD_SPI_DMA_CH_1].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
		Dd_HDMAC1_Int_Handler( D_CT_DD_SPI_DMA_CH_1 );

		if( gCT_DD_SPI_Trans_Type == E_CT_DD_SPI_TRNAS_TYPE_FULL ){
			IO_HDMAC1.DMAC[D_CT_DD_SPI_DMA_CH_2 ].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
			Dd_HDMAC1_Int_Handler( D_CT_DD_SPI_DMA_CH_2 );
		}
	else if( gDD_CT_SPI_Timer_Mode == D_CT_DD_SPI_TIMER_MODE_STOP ){
		Ddim_Print(("---Stop SPI(%d)---\n", gDD_CT_SPI_Ch));
		Dd_SPI_Stop( gDD_CT_SPI_Ch );
	}

	gDD_CT_SPI_Timer_Num--;
	if( gDD_CT_SPI_Timer_Num > 0 ){
		ct_dd_spi_set_timer( gDD_CT_SPI_Ch, 100, gDD_CT_SPI_Timer_Num, gDD_CT_SPI_Timer_Mode );
	}
}

static VOID ct_dd_spi_set_timer( UCHAR spi_ch, ULONG msec, UCHAR num, INT32 mode )
{
	INT32 ret;

	gDD_CT_SPI_Ch			= spi_ch;
	gDD_CT_SPI_Timer_Mode	= mode;
	gDD_CT_SPI_Timer_Num	= num;

	ret = Dd_TMR32_Open( D_CT_DD_SPI_TIMER_CH, D_DDIM_USER_SEM_WAIT_POL );
	if (ret != D_DDIM_OK) {
		Ddim_Print(("timer open error : ret=%d \n", ret));
		return;
	}

	ret = Dd_TMR32_SetTimer( D_CT_DD_SPI_TIMER_CH, msec * 1000, ct_dd_spi_callback_timer );
	if(ret != D_DDIM_OK) {
		Dd_TMR32_Stop(D_CT_DD_SPI_TIMER_CH);
		Dd_TMR32_Close(D_CT_DD_SPI_TIMER_CH);
		Ddim_Print(("timer set error : ret=%d \n", ret));
		return;
	}

	ret = Dd_TMR32_Start(D_CT_DD_SPI_TIMER_CH);
	if(ret != D_DDIM_OK) {
		Ddim_Print(("timer start error : ret=%d \n", ret));
		return;
	}
}

static VOID ct_dd_spi_cmd_wrap( CHAR* cmd )
{
	INT32	argc = 0;
	CHAR*	argv[32];
	CHAR	arg[32][32];
	INT32	pos = 0;
	INT32	len = 0;
	INT32	max = strlen(cmd);
	INT32	i;

//	printf("CMD> %s\n", cmd);

	// Clear arg buffer
	for(i = 0; i < 32; i++){
		argv[i] = arg[i];
		memset(argv[i], 0, 32);
	}

	// Set arg buffer
	for(i = 0; i < max; i++){
		if(cmd[i] == ' '){
			if(len){
				memcpy(argv[argc], &cmd[pos], len);
				argc++;
				len = 0;
			}
			pos = i + 1;
		}
		else if(i == max - 1){
			memcpy(argv[argc], &cmd[pos], len + 1);
			argc++;
			len = 0;
			pos = i + 1;
		}
		else{
			len++;
		}
	}

	// Execute command
	Ct_Dd_SPI_Main(argc, argv);
}

// open normal (D_DDIM_USER_SEM_WAIT_POL)
static VOID ct_dd_spi_pc_test_open_1( VOID )
{
	INT32 ret;
	UCHAR ch = 0;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	ct_dd_spi_cmd_wrap("ddspi open 0 0");
	ret = gDD_CT_SPI_Result;
	ct_dd_spi_cmd_wrap("ddspi close 0");

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// open normal (D_DDIM_USER_SEM_WAIT_FEVR)
static VOID ct_dd_spi_pc_test_open_2( VOID )
{
	INT32 ret;
	UCHAR ch = 1;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	ct_dd_spi_cmd_wrap("ddspi open 1 -1");
	ret = gDD_CT_SPI_Result;
	ct_dd_spi_cmd_wrap("ddspi close 1");

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// open error (D_DDIM_USER_SEM_WAIT_POL->D_DD_SPI_SEM_TIMEOUT)
static VOID ct_dd_spi_pc_test_open_3( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	ct_dd_spi_cmd_wrap("ddspi open 2 0");
	ct_dd_spi_cmd_wrap("ddspi open 2 0");
	ret = gDD_CT_SPI_Result;
	ct_dd_spi_cmd_wrap("ddspi close 2");

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// open error (1msec->D_DD_SPI_SEM_TIMEOUT)
static VOID ct_dd_spi_pc_test_open_4( VOID )
{
	INT32 ret;
	UCHAR ch = 0;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	ct_dd_spi_cmd_wrap("ddspi open 0 0");
	ct_dd_spi_cmd_wrap("ddspi open 0 1");
	ret = gDD_CT_SPI_Result;
	ct_dd_spi_cmd_wrap("ddspi close 0");

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// close normal
static VOID ct_dd_spi_pc_test_close_1( VOID )
{
	INT32 ret;
	UCHAR ch = 1;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	ct_dd_spi_cmd_wrap("ddspi open 1 0");
	ct_dd_spi_cmd_wrap("ddspi close 1");
	ret = gDD_CT_SPI_Result;

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// close error
static VOID ct_dd_spi_pc_test_close_2( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	ct_dd_spi_cmd_wrap("ddspi close 2");
	ret = gDD_CT_SPI_Result;

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// ctrl normal1
static VOID ct_dd_spi_pc_test_ctrl_1( VOID )
{
	INT32 ret;
	UCHAR ch = 0;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	//                                                                    +---dma_to:0-16777215
	//                                                                    | +---ssout:0-3
	//                                                      +-dly:0-255   | | +---sspol:0-1
	//                                  +-mode:0-3          | +-Inhi:0-1  | | | +---cont_trans:0-2
	//                                  | +-sig:0-3         | | +-txwmk   | | | | +---cb
	//                                  | |     +-bit:4-16  | | |  +-rxwmk| | | | | +---cb_ss
	//                                  | |     |           | | |  |      | | | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 0 sl 0 0 lsb 4 108000000 0 0 64 32 cpu 1 0 0 0 0 0");
	ret = gDD_CT_SPI_Result;
	ct_dd_spi_cmd_wrap("ddspi get_ctrl 0");

	ct_dd_spi_print_ctrl_reg( 0 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// ctrl normal2
static VOID ct_dd_spi_pc_test_ctrl_2( VOID )
{
	INT32 ret;
	UCHAR ch = 0;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	//                                                                    +---dma_to:0-16777215
	//                                                                    |        +---ssout:0-3
	//                                                      +-dly:0-255   |        | +---sspol:0-1
	//                                  +-mode:0-3          | +-Inhi:0-1  |        | | +---cont_trans:0-2
	//                                  | +-sig:0-3         | | +-txwmk   |        | | | +---cb
	//                                  | |     +-bit:4-16  | | |  +-rxwmk|        | | | | +---cb_ss
	//                                  | |     |           | | |  |      |        | | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 0 ms 3 3 msb 16 421875 255 1 32 16 dma 16777215 3 1 2 1 1");
	ret = gDD_CT_SPI_Result;
	ct_dd_spi_cmd_wrap("ddspi get_ctrl 0");

	ct_dd_spi_print_ctrl_reg( 0 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// set send normal1
static VOID ct_dd_spi_pc_test_set_send_data_1( VOID )
{
	INT32 ret;
	UCHAR ch = 1;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Bit_Len = 4;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 1 ms 0 0 lsb 4 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 1 1 data 0xA5");
	ret = gDD_CT_SPI_Result;

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// set send normal2
static VOID ct_dd_spi_pc_test_set_send_data_2( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Bit_Len = 16;

	//                                                                   +---dma_to:0-16777215
	//                                                                   |+---ssout:0-3
	//                                                      +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3          | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3         | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16  | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |           | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 32 data 0xA5A5");
	ret = gDD_CT_SPI_Result;

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// set recv normal1
static VOID ct_dd_spi_pc_test_set_recv_data_1( VOID )
{
	INT32 ret;
	UCHAR ch = 0;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Bit_Len = 4;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 0 ms 0 0 lsb 4 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 0 1 data 0xA5");
	ret = gDD_CT_SPI_Result;

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// set recv normal2
static VOID ct_dd_spi_pc_test_set_recv_data_2( VOID )
{
	INT32 ret;
	UCHAR ch = 1;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Bit_Len = 16;

	//                                                                   +---dma_to:0-16777215
	//                                                                   |+---ssout:0-3
	//                                                      +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3          | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3         | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16  | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |           | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 1 ms 0 0 lsb 16 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 1 32 data 0xA5A5");
	ret = gDD_CT_SPI_Result;

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start send normal (8bit 1byte async)
static VOID ct_dd_spi_pc_test_start_send_1( VOID )
{
	INT32 ret;
	UCHAR ch = 0;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Bit_Len = 8;

	//                                                                   +---dma_to:0-16777215
	//                                                                   | +---ssout:0-3
	//                                                     +-dly:0-255   | | +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1  | | | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk   | | | | +---cb
	//                                  | |     +-bit:4-16 | | |  +-rxwmk| | | | | +---cb_ss
	//                                  | |     |          | | |  |      | | | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 0 ms 0 0 lsb 8 12000000 0 0 64 64 cpu 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 0 1 data 0xA5");

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 1;

	ct_dd_spi_cmd_wrap("ddspi start_send 0");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_send_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start send normal (8bit 1byte sync)
static VOID ct_dd_spi_pc_test_start_send_2( VOID )
{
	INT32 ret;
	UCHAR ch = 0;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Bit_Len = 8;

	//                                                                   +---dma_to:0-16777215
	//                                                                   | +---ssout:0-3
	//                                                     +-dly:0-255   | | +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1  | | | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk   | | | | +---cb
	//                                  | |     +-bit:4-16 | | |  +-rxwmk| | | | | +---cb_ss
	//                                  | |     |          | | |  |      | | | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 0 ms 0 0 lsb 8 12000000 0 0 64 64 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 0 1 data 0xA5");

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 1;

	ct_dd_spi_cmd_wrap("ddspi start_send 0");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_send_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start send normal (8bit 32byte sync)
static VOID ct_dd_spi_pc_test_start_send_3( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 32 data 0xA5");

	gDD_CT_SPI_Send_Buf8[31] = 0xB6;

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 1;

	ct_dd_spi_cmd_wrap("ddspi start_send 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_send_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start send error (D_DD_SPI_FORCE_STOP) (8bit 32byte sync) (TXFULL=1)
static VOID ct_dd_spi_pc_test_start_send_4( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 32 data 0xA5");

	gDD_CT_SPI_Send_Buf8[31] = 0xB6;

	IO_SPI[ch].ST.bit.TXFULL = 1;
	IO_SPI[ch].ST.bit.TXEMPTY = 1;

	ct_dd_spi_cmd_wrap("ddspi stop 2");

	ct_dd_spi_cmd_wrap("ddspi start_send 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_send_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start send normal (16bit 1byte async)
static VOID ct_dd_spi_pc_test_start_send_5( VOID )
{
	INT32 ret;
	UCHAR ch = 0;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Bit_Len = 16;

	//                                                                   +---dma_to:0-16777215
	//                                                                   | +---ssout:0-3
	//                                                     +-dly:0-255   | | +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1  | | | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk   | | | | +---cb
	//                                  | |     +-bit:4-16 | | |  +-rxwmk| | | | | +---cb_ss
	//                                  | |     |          | | |  |      | | | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 0 ms 0 0 lsb 16 12000000 0 0 64 64 cpu 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 0 1 data 0xA5A5");

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 1;

	ct_dd_spi_cmd_wrap("ddspi start_send 0");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_send_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start send normal (16bit 1byte sync)
static VOID ct_dd_spi_pc_test_start_send_6( VOID )
{
	INT32 ret;
	UCHAR ch = 0;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Bit_Len = 16;

	//                                                                   +---dma_to:0-16777215
	//                                                                   | +---ssout:0-3
	//                                                     +-dly:0-255   | | +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1  | | | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk   | | | | +---cb
	//                                  | |     +-bit:4-16 | | |  +-rxwmk| | | | | +---cb_ss
	//                                  | |     |          | | |  |      | | | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 0 ms 0 0 lsb 16 12000000 0 0 64 64 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 0 1 data 0xA5A5");

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 1;

	ct_dd_spi_cmd_wrap("ddspi start_send 0");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_send_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start send normal (16bit 32byte sync)
static VOID ct_dd_spi_pc_test_start_send_7( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 32 data 0xA5A5");

	gDD_CT_SPI_Send_Buf16[31] = 0xB6B6;

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 1;

	ct_dd_spi_cmd_wrap("ddspi start_send 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_send_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start send error (D_DD_SPI_FORCE_STOP) (16bit 32byte sync) (TXFULL=1)
static VOID ct_dd_spi_pc_test_start_send_8( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 32 data 0xA5A5");

	gDD_CT_SPI_Send_Buf16[31] = 0xB6B6;

	IO_SPI[ch].ST.bit.TXFULL = 1;
	IO_SPI[ch].ST.bit.TXEMPTY = 1;

	ct_dd_spi_cmd_wrap("ddspi stop 2");

	ct_dd_spi_cmd_wrap("ddspi start_send 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_send_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start send error (D_DD_SPI_FORCE_STOP) (16bit 32byte sync) (TXEMPTY=0)
static VOID ct_dd_spi_pc_test_start_send_9( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 32 data 0xA5A5");

	gDD_CT_SPI_Send_Buf16[31] = 0xB6B6;

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;

	ct_dd_spi_cmd_wrap("ddspi stop 2");

	ct_dd_spi_cmd_wrap("ddspi start_send 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_send_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start recv normal (8bit 1byte async)
static VOID ct_dd_spi_pc_test_start_recv_1( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 1 data");

	IO_SPI[ch].RXDATA = 0x00A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;

	ct_dd_spi_cmd_wrap("ddspi start_recv 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_recv_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start recv normal (8bit 18byte async)
static VOID ct_dd_spi_pc_test_start_recv_2( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].RXDATA = 0x00A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;

	ct_dd_spi_cmd_wrap("ddspi start_recv 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_recv_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start recv normal (8bit 1byte sync)
static VOID ct_dd_spi_pc_test_start_recv_3( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 1 data");

	IO_SPI[ch].RXDATA = 0x00A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;

	ct_dd_spi_cmd_wrap("ddspi start_recv 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_recv_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start recv normal (8bit 18byte sync)
static VOID ct_dd_spi_pc_test_start_recv_4( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].RXDATA = 0x00A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;

	ct_dd_spi_cmd_wrap("ddspi start_recv 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_recv_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start recv error (D_DD_SPI_FORCE_STOP) (RXEMPTY=1) (8bit 18byte sync)
static VOID ct_dd_spi_pc_test_start_recv_5( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].RXDATA = 0x00A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 1;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;

	ct_dd_spi_cmd_wrap("ddspi stop 2");

	ct_dd_spi_cmd_wrap("ddspi start_recv 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_recv_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start recv error (D_DD_SPI_RECV_OVERRUN_ERROR) (8bit 18byte sync)
static VOID ct_dd_spi_pc_test_start_recv_6( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].RXDATA = 0x00A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 1;

	ct_dd_spi_cmd_wrap("ddspi start_recv 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_recv_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start recv normal (16bit 1byte sync)
static VOID ct_dd_spi_pc_test_start_recv_7( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 1 data");

	IO_SPI[ch].RXDATA = 0xA5A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;

	ct_dd_spi_cmd_wrap("ddspi start_recv 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_recv_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start recv normal (16bit 18byte sync)
static VOID ct_dd_spi_pc_test_start_recv_8( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].RXDATA = 0xA5A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;

	ct_dd_spi_cmd_wrap("ddspi start_recv 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_recv_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start recv error (D_DD_SPI_FORCE_STOP) (RXEMPTY=1) (16bit 18byte sync)
static VOID ct_dd_spi_pc_test_start_recv_9( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].RXDATA = 0xA5A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 1;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;

	ct_dd_spi_cmd_wrap("ddspi stop 2");

	ct_dd_spi_cmd_wrap("ddspi start_recv 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_recv_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start recv error (D_DD_SPI_RECV_OVERRUN_ERROR) (16bit 18byte sync)
static VOID ct_dd_spi_pc_test_start_recv_10( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].RXDATA = 0xA5A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 1;

	ct_dd_spi_cmd_wrap("ddspi start_recv 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_recv_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start recv error (D_DD_SPI_FORCE_STOP) (RXEMPTY=0) (16bit 18byte sync)
static VOID ct_dd_spi_pc_test_start_recv_11( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].RXDATA = 0xA5A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;

	ct_dd_spi_cmd_wrap("ddspi stop 2");

	ct_dd_spi_cmd_wrap("ddspi start_recv 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_recv_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start full normal (8bit 1byte async)
static VOID ct_dd_spi_pc_test_start_full_1( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 1 data 0xA5");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 1 data");

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;
	IO_SPI[ch].RXDATA = 0x00A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;

	ct_dd_spi_cmd_wrap("ddspi start_full 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_full_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start full normal (8bit 18byte async)
static VOID ct_dd_spi_pc_test_start_full_2( VOID )
{
	INT32 ret;
	UCHAR ch = 2;
	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 18 data 0xA5");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;
	IO_SPI[ch].RXDATA = 0x00A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;

	gDD_CT_SPI_Send_Buf8[17] = 0xB6;

	ct_dd_spi_cmd_wrap("ddspi start_full 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_full_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start full normal (8bit 1byte sync)
static VOID ct_dd_spi_pc_test_start_full_3( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 1 data 0xA5");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 1 data");

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;
	IO_SPI[ch].RXDATA = 0x00A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;

	ct_dd_spi_cmd_wrap("ddspi start_full 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_full_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start full normal (8bit 18byte sync)
static VOID ct_dd_spi_pc_test_start_full_4( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 18 data 0xA5");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;
	IO_SPI[ch].RXDATA = 0x00A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;

	gDD_CT_SPI_Send_Buf8[17] = 0xB6;

	ct_dd_spi_cmd_wrap("ddspi start_full 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_full_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start full error (D_DD_SPI_FORCE_STOP) (RXEMPTY=1) (8bit 18byte sync)
static VOID ct_dd_spi_pc_test_start_full_5( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 18 data 0xA5");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;
	IO_SPI[ch].RXDATA = 0x00A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 1;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;

	gDD_CT_SPI_Send_Buf8[17] = 0xB6;

	ct_dd_spi_cmd_wrap("ddspi stop 2");

	ct_dd_spi_cmd_wrap("ddspi start_full 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_full_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start full error (D_DD_SPI_RECV_OVERRUN_ERROR) (8bit 18byte sync)
static VOID ct_dd_spi_pc_test_start_full_6( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 18 data 0xA5");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;
	IO_SPI[ch].RXDATA = 0x00A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 1;

	gDD_CT_SPI_Send_Buf8[17] = 0xB6;

	ct_dd_spi_cmd_wrap("ddspi start_full 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_full_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start full normal (16bit 1byte sync)
static VOID ct_dd_spi_pc_test_start_full_7( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 1 data 0xA5A5");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 1 data");

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;
	IO_SPI[ch].RXDATA = 0xA5A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;

	ct_dd_spi_cmd_wrap("ddspi start_full 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_full_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start full normal (16bit 18byte sync)
static VOID ct_dd_spi_pc_test_start_full_8( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 18 data 0xA5A5");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;
	IO_SPI[ch].RXDATA = 0xA5A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;

	gDD_CT_SPI_Send_Buf16[17] = 0xB6B6;

	ct_dd_spi_cmd_wrap("ddspi start_full 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_full_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start full error (D_DD_SPI_FORCE_STOP) (RXEMPTY=1) (16bit 18byte sync)
static VOID ct_dd_spi_pc_test_start_full_9( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 18 data 0xA5A5");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;
	IO_SPI[ch].RXDATA = 0xA5A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 1;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;

	gDD_CT_SPI_Send_Buf16[17] = 0xB6B6;

	ct_dd_spi_cmd_wrap("ddspi stop 2");

	ct_dd_spi_cmd_wrap("ddspi start_full 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_full_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start full error (D_DD_SPI_RECV_OVERRUN_ERROR) (16bit 18byte sync)
static VOID ct_dd_spi_pc_test_start_full_10( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 18 data 0xA5A5");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;
	IO_SPI[ch].RXDATA = 0xA5A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 1;

	gDD_CT_SPI_Send_Buf16[17] = 0xB6B6;

	ct_dd_spi_cmd_wrap("ddspi start_full 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_full_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start full error (D_DD_SPI_FORCE_STOP) (RXEMPTY=0) (16bit 18byte sync)
static VOID ct_dd_spi_pc_test_start_full_11( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 cpu 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 18 data 0xA5A5");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;
	IO_SPI[ch].RXDATA = 0xA5A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;

	gDD_CT_SPI_Send_Buf16[17] = 0xB6B6;

	ct_dd_spi_cmd_wrap("ddspi stop 2");

	ct_dd_spi_cmd_wrap("ddspi start_full 2");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_full_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// stop normal with CPU
static VOID ct_dd_spi_pc_test_stop_1( VOID )
{
	INT32 ret;
	UCHAR ch = 0;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 0 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 0 1 data 0xA5");

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;

	ct_dd_spi_cmd_wrap("ddspi start_send 0");
	ct_dd_spi_cmd_wrap("ddspi stop 0");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_send_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// stop normal with DMA
static VOID ct_dd_spi_pc_test_stop_2( VOID )
{
	INT32 ret;
	UCHAR ch = 0;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 0 ms 0 0 lsb 8 12000000 0 0 8 8 dma 1 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 0 1 data 0xA5");

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;

	ct_dd_spi_cmd_wrap("ddspi start_send_dma 0 0");
	ct_dd_spi_cmd_wrap("ddspi stop 0");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_reg( ch, 0 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// set SS info normal1
static VOID ct_dd_spi_pc_test_setget_ss_1( VOID )
{
	INT32 ret;
	UCHAR ch = 0;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	// ddspi set_ss [ch] [cont_trans] [ssout0] [ssout1] [ssout2] [ssout3] [sspol0] [sspol1] [sspol2] [sspol3]
	ct_dd_spi_cmd_wrap("ddspi set_ss 0 0 1 0 0 1 0 1 1 0");
	ret = gDD_CT_SPI_Result;
	ct_dd_spi_cmd_wrap("ddspi get_ss 0");

	ct_dd_spi_print_ss( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// set SS info normal2
static VOID ct_dd_spi_pc_test_setget_ss_2( VOID )
{
	INT32 ret;
	UCHAR ch = 1;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	// ddspi set_ss [ch] [cont_trans] [ssout0] [ssout1] [ssout2] [ssout3] [sspol0] [sspol1] [sspol2] [sspol3]
	ct_dd_spi_cmd_wrap("ddspi set_ss 1 1 0 1 1 0 1 0 0 1");
	ret = gDD_CT_SPI_Result;
	ct_dd_spi_cmd_wrap("ddspi get_ss 1");

	ct_dd_spi_print_ss( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// set SS info normal3
static VOID ct_dd_spi_pc_test_setget_ss_3( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	// ddspi set_ss [ch] [cont_trans] [ssout0] [ssout1] [ssout2] [ssout3] [sspol0] [sspol1] [sspol2] [sspol3]
	ct_dd_spi_cmd_wrap("ddspi set_ss 2 2 1 1 1 1 1 1 1 1");
	ret = gDD_CT_SPI_Result;
	ct_dd_spi_cmd_wrap("ddspi get_ss 2");

	ct_dd_spi_print_ss( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// set get clkdiv normal
static VOID ct_dd_spi_pc_test_setget_clkdiv_1( VOID )
{
	INT32 ret;
	UCHAR ch = 0;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	ct_dd_spi_cmd_wrap("ddspi set_div 0 255");
	ret = gDD_CT_SPI_Result;
	ct_dd_spi_cmd_wrap("ddspi get_div 0");

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// handler (XFERDONEPULSE interrupt)
static VOID ct_dd_spi_pc_test_handler_1( VOID )
{
	INT32 ret;
	UCHAR ch = 0;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 0 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 1 1");

	IO_SPI[ch].INT_CLR.word = 0;
	IO_SPI[ch].INT_ST.bit.XFERDONEPULSE = 1;

	Dd_SPI_Int_Handler( ch );

	ret = gDD_CT_SPI_Result;

	Ddim_Print(("INT_CLR.word = 0x%08lX\n", IO_SPI[ch].INT_CLR.word));

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// handler (TXEMPTYPULSE interrupt, dma_wait = 2)
static VOID ct_dd_spi_pc_test_handler_2( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 dma 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 18 data 0xA5");

	gDD_CT_SPI_Send_Buf8[17] = 0xB6;

	IO_SPI[ch].ST.bit.TXFULL = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;
	IO_SPI[ch].INT_ST.bit.TXEMPTYPULSE = 1;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_send_dma 2 0");

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	Dd_SPI_Int_Handler( ch );

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_send_reg( ch );
	Ddim_Print(("INT_CLR.word = 0x%08lX\n", IO_SPI[ch].INT_CLR.word));

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// handler (TXEMPTYPULSE interrupt, dma_wait = 0, spi_mode = SEND, send_pos < num)
static VOID ct_dd_spi_pc_test_handler_3( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 18 data 0xA5");

	gDD_CT_SPI_Send_Buf8[17] = 0xB6;

	IO_SPI[ch].ST.bit.TXFULL = 1;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;
	IO_SPI[ch].INT_ST.bit.TXEMPTYPULSE = 1;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_send 2");

	Dd_SPI_Int_Handler( ch );

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_send_reg( ch );
	Ddim_Print(("INT_CLR.word = 0x%08lX\n", IO_SPI[ch].INT_CLR.word));

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// handler (TXEMPTYPULSE interrupt, dma_wait = 0, spi_mode = SEND, send_pos == num)
static VOID ct_dd_spi_pc_test_handler_4( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 18 data 0xA5");

	gDD_CT_SPI_Send_Buf8[17] = 0xB6;

	IO_SPI[ch].ST.bit.TXFULL = 1;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;
	IO_SPI[ch].INT_ST.bit.TXEMPTYPULSE = 1;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_send 2");

	IO_SPI[ch].ST.bit.TXFULL = 0;

	Dd_SPI_Int_Handler( ch );

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_send_reg( ch );
	Ddim_Print(("INT_CLR.word = 0x%08lX\n", IO_SPI[ch].INT_CLR.word));

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// handler (TXWMARKPULSE interrupt, dma_wait = 0, spi_mode = SEND)
static VOID ct_dd_spi_pc_test_handler_5( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 18 data 0xA5");

	gDD_CT_SPI_Send_Buf8[17] = 0xB6;

	IO_SPI[ch].ST.bit.TXFULL = 1;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;
	IO_SPI[ch].INT_ST.bit.TXEMPTYPULSE = 0;
	IO_SPI[ch].INT_ST.bit.TXWMARKPULSE = 1;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_send 2");

	IO_SPI[ch].ST.bit.TXFULL = 0;

	Dd_SPI_Int_Handler( ch );

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_send_reg( ch );
	Ddim_Print(("INT_CLR.word = 0x%08lX\n", IO_SPI[ch].INT_CLR.word));

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// handler (TXEMPTYPULSE interrupt, spi_mode = RECV, TX_FIFO_LEVEL = 8)
static VOID ct_dd_spi_pc_test_handler_6( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].RXDATA = 0x00A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;
	IO_SPI[ch].INT_ST.bit.TXEMPTYPULSE = 1;
	IO_SPI[ch].TXFIFO.bit.TX_FIFO_LEVEL = 8;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_recv 2");

	Dd_SPI_Int_Handler( ch );

	Dd_SPI_Int_Handler( ch );

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_recv_reg( ch );
	Ddim_Print(("INT_CLR.word = 0x%08lX\n", IO_SPI[ch].INT_CLR.word));

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// handler (TXWMARKPULSE interrupt, spi_mode = RECV, TX_FIFO_LEVEL = 8)
static VOID ct_dd_spi_pc_test_handler_7( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].RXDATA = 0x00A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;
	IO_SPI[ch].INT_ST.bit.TXWMARKPULSE = 1;
	IO_SPI[ch].TXFIFO.bit.TX_FIFO_LEVEL = 8;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_recv 2");

	Dd_SPI_Int_Handler( ch );

	Dd_SPI_Int_Handler( ch );

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_recv_reg( ch );
	Ddim_Print(("INT_CLR.word = 0x%08lX\n", IO_SPI[ch].INT_CLR.word));

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// handler (RXFIFOOVERFLOW interrupt)
static VOID ct_dd_spi_pc_test_handler_8( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].RXDATA = 0x00A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].INT_ST.bit.RXFIFOOVERFLOW = 1;
	IO_SPI[ch].INT_ST.bit.TXEMPTYPULSE = 0;
	IO_SPI[ch].TXFIFO.bit.TX_FIFO_LEVEL = 8;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_recv 2");

	Dd_SPI_Int_Handler( ch );

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_recv_reg( ch );
	Ddim_Print(("INT_CLR.word = 0x%08lX\n", IO_SPI[ch].INT_CLR.word));

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// handler (RXWMARKPULSE interrupt, RX_FIFO_LEVEL = 8)
static VOID ct_dd_spi_pc_test_handler_9( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].RXDATA = 0x00A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;
	IO_SPI[ch].INT_ST.bit.RXWMARKPULSE = 1;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 8;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_recv 2");

	Dd_SPI_Int_Handler( ch );
	Ddim_Print(("INT_CLR.word = 0x%08lX\n", IO_SPI[ch].INT_CLR.word));

	Dd_SPI_Int_Handler( ch );
	Ddim_Print(("INT_CLR.word = 0x%08lX\n", IO_SPI[ch].INT_CLR.word));

	Dd_SPI_Int_Handler( ch );
	Ddim_Print(("INT_CLR.word = 0x%08lX\n", IO_SPI[ch].INT_CLR.word));

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_recv_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// handler (RXFULLPULSE interrupt, RX_FIFO_LEVEL = 16)
static VOID ct_dd_spi_pc_test_handler_10( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 cpu 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].RXDATA = 0x00A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;
	IO_SPI[ch].INT_ST.bit.RXFULLPULSE = 1;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 16;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_recv 2");

	Dd_SPI_Int_Handler( ch );
	Ddim_Print(("INT_CLR.word = 0x%08lX\n", IO_SPI[ch].INT_CLR.word));

	Dd_SPI_Int_Handler( ch );
	Ddim_Print(("INT_CLR.word = 0x%08lX\n", IO_SPI[ch].INT_CLR.word));

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_recv_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// handler (RXTIMEOUT interrupt, RX_FIFO_LEVEL = 8, 16bit)
static VOID ct_dd_spi_pc_test_handler_11( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 18;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 cpu 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 18 data");

	IO_SPI[ch].RXDATA = 0xA5A5;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 0;
	IO_SPI[ch].ST.bit.RXEMPTY = 0;
	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;
	IO_SPI[ch].INT_ST.bit.RXTIMEOUT = 1;
	IO_SPI[ch].RXFIFO.bit.RX_FIFO_LEVEL = 8;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_recv 2");

	Dd_SPI_Int_Handler( ch );
	Ddim_Print(("INT_CLR.word = 0x%08lX\n", IO_SPI[ch].INT_CLR.word));

	Dd_SPI_Int_Handler( ch );
	Ddim_Print(("INT_CLR.word = 0x%08lX\n", IO_SPI[ch].INT_CLR.word));

	Dd_SPI_Int_Handler( ch );
	Ddim_Print(("INT_CLR.word = 0x%08lX\n", IO_SPI[ch].INT_CLR.word));

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_recv_reg( ch );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

static VOID ct_dd_spi_pc_test_calc_1( VOID )
{
	INT32 ret;
	UCHAR ch = 0;

	IO_CHIPTOP.CLKSEL8.bit.SPICLK = 0;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	ct_dd_spi_cmd_wrap("ddspi cal 108000000");

	ret = gDD_CT_SPI_Result;

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

static VOID ct_dd_spi_pc_test_calc_2( VOID )
{
	INT32 ret;
	UCHAR ch = 0;

	IO_CHIPTOP.CLKSEL8.bit.SPICLK = 1;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	ct_dd_spi_cmd_wrap("ddspi cal 300000");

	ret = gDD_CT_SPI_Result;

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

static VOID ct_dd_spi_pc_test_calc_3( VOID )
{
	INT32 ret;
	UCHAR ch = 0;

	IO_CHIPTOP.CLKSEL8.bit.SPICLK = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	ct_dd_spi_cmd_wrap("ddspi cal 20000000");

	ret = gDD_CT_SPI_Result;

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

static VOID ct_dd_spi_pc_test_calc_4( VOID )
{
	INT32 ret;
	UCHAR ch = 0;

	IO_CHIPTOP.CLKSEL8.bit.SPICLK = 0;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	ct_dd_spi_cmd_wrap("ddspi cal 108000001");

	ret = gDD_CT_SPI_Result;

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start send DMA (sync, 8bit, 1byte)
static VOID ct_dd_spi_pc_test_start_send_dma_1( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 dma 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 1 data 0xA5");

	IO_SPI[ch].ST.bit.TXEMPTY = 1;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_send_dma 2 0");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_reg( ch, 0 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start send DMA (sync, 8bit, 262143byte)
static VOID ct_dd_spi_pc_test_start_send_dma_2( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 262143;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 dma 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 262143 data 0xA5");

	IO_SPI[ch].ST.bit.TXEMPTY = 1;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_send_dma 2 0");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_reg( ch, 0 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start send DMA (async, 16bit, 1byte)
static VOID ct_dd_spi_pc_test_start_send_dma_3( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 dma 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 1 data 0xA5A5");

	IO_SPI[ch].ST.bit.TXEMPTY = 1;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_send_dma 2 0");

	gCT_DD_SPI_Async = 0;	// not need data print

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_reg( ch, 0 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start send DMA (async, 16bit, 262143byte)
static VOID ct_dd_spi_pc_test_start_send_dma_4( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 262143;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 dma 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 262143 data 0xA5A5");

	IO_SPI[ch].ST.bit.TXEMPTY = 1;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_send_dma 2 0");

	gCT_DD_SPI_Async = 0;	// not need data print

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_reg( ch, 0 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start send DMA (sync, 8bit, 1byte, TXEMPTY=0)
static VOID ct_dd_spi_pc_test_start_send_dma_5( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 dma 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 1 data 0xA5");

	IO_SPI[ch].ST.bit.TXEMPTY = 0;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi stop 2");
	ct_dd_spi_cmd_wrap("ddspi start_send_dma 2 0");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_reg( ch, 0 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start send DMA (sync, 8bit, 1byte, force_stop_flg=1)
static VOID ct_dd_spi_pc_test_start_send_dma_6( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 dma 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 1 data 0xA5");

	IO_SPI[ch].ST.bit.TXEMPTY = 1;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi stop 2");
	ct_dd_spi_cmd_wrap("ddspi start_send_dma 2 0");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_reg( ch, 0 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start send DMA (async, 8bit, 1byte, TXEMPTY=0)
static VOID ct_dd_spi_pc_test_start_send_dma_7( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 dma 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 1 data 0xA5");

	IO_SPI[ch].ST.bit.TXEMPTY = 0;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_send_dma 2 0");

	gCT_DD_SPI_Async = 0;	// not need data print

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_reg( ch, 0 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start send DMA (async, 16bit, 1byte, force_stop_flg=1)
static VOID ct_dd_spi_pc_test_start_send_dma_8( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 dma 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 1 data 0xA5A5");

	IO_SPI[ch].ST.bit.TXEMPTY = 1;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi stop 2");

	ct_dd_spi_cmd_wrap("ddspi start_send_dma 2 0");

	gCT_DD_SPI_Async = 0;	// not need data print

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_reg( ch, 0 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start recv DMA (sync, 8bit, 1byte)
static VOID ct_dd_spi_pc_test_start_recv_dma_1( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 dma 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 1 data");

	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_recv_dma 2 0");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_reg( ch, 0 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start recv DMA (sync, 8bit, 262143byte)
static VOID ct_dd_spi_pc_test_start_recv_dma_2( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 262143;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 dma 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 262143 data");

	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_recv_dma 2 0");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_reg( ch, 0 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start recv DMA (async, 16bit, 1byte)
static VOID ct_dd_spi_pc_test_start_recv_dma_3( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 dma 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 1 data 0xA5A5");

	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_recv_dma 2 0");

	gCT_DD_SPI_Async = 0;	// not need data print

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_reg( ch, 0 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start recv DMA (async, 16bit, 262143byte)
static VOID ct_dd_spi_pc_test_start_recv_dma_4( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 262143;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 dma 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 262143 data 0xA5A5");

	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_recv_dma 2 0");

	gCT_DD_SPI_Async = 0;	// not need data print

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_reg( ch, 0 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start recv DMA (sync, 8bit, 1byte, RXOVERFLOW=1)
static VOID ct_dd_spi_pc_test_start_recv_dma_5( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 dma 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 1 data");

	IO_SPI[ch].ST.bit.RXOVERFLOW = 1;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_recv_dma 2 0");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_reg( ch, 0 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start recv DMA (sync, 8bit, 1byte, force_stop_flg=1)
static VOID ct_dd_spi_pc_test_start_recv_dma_6( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 dma 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 1 data");

	IO_SPI[ch].ST.bit.RXOVERFLOW =0;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi stop 2");
	ct_dd_spi_cmd_wrap("ddspi start_recv_dma 2 0");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_reg( ch, 0 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start recv DMA (async, 8bit, 1byte, RXOVERFLOW=1)
static VOID ct_dd_spi_pc_test_start_recv_dma_7( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 dma 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 1 data");

	IO_SPI[ch].ST.bit.RXOVERFLOW = 1;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_recv_dma 2 0");

	gCT_DD_SPI_Async = 0;	// not need data print

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_reg( ch, 0 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start recv DMA (async, 16bit, 1byte, force_stop_flg=1)
static VOID ct_dd_spi_pc_test_start_recv_dma_8( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 dma 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 1 data 0xA5A5");

	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi stop 2");

	ct_dd_spi_cmd_wrap("ddspi start_recv_dma 2 0");

	gCT_DD_SPI_Async = 0;	// not need data print

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_reg( ch, 0 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start full DMA (sync, 8bit, 1byte)
static VOID ct_dd_spi_pc_test_start_full_dma_1( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 dma 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 1 data 0xA5");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 1 data");

	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 1;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_full_dma 2 0 1");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_full_reg( ch, 0, 1 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start full DMA (sync, 8bit, 262143byte)
static VOID ct_dd_spi_pc_test_start_full_dma_2( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 262143;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 dma 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 262143 data 0xA5");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 262143 data");

	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 1;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_full_dma 2 0 1");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_full_reg( ch, 0, 1 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start full DMA (async, 16bit, 1byte)
static VOID ct_dd_spi_pc_test_start_full_dma_3( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 dma 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 1 data 0xA5A5");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 1 data");

	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 1;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_full_dma 2 0 1");

	gCT_DD_SPI_Async = 0;	// not need data print

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	IO_HDMAC1.DMAC[1].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 1 );

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_full_reg( ch, 0, 1 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start full DMA (async, 16bit, 262143byte)
static VOID ct_dd_spi_pc_test_start_full_dma_4( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 262143;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 dma 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 262143 data 0xA5A5");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 262143 data");

	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 1;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_full_dma 2 0 1");

	gCT_DD_SPI_Async = 0;	// not need data print

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	IO_HDMAC1.DMAC[1].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 1 );

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	IO_HDMAC1.DMAC[1].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 1 );

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	IO_HDMAC1.DMAC[1].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 1 );

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	IO_HDMAC1.DMAC[1].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 1 );

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_full_reg( ch, 0, 1 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start full DMA (sync, 8bit, 1byte, RXOVERFLOW=1)
static VOID ct_dd_spi_pc_test_start_full_dma_5( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 dma 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 1 data 0xA5");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 1 data");

	IO_SPI[ch].ST.bit.RXOVERFLOW = 1;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_full_dma 2 0 1");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_full_reg( ch, 0, 1 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start full DMA (sync, 8bit, 1byte, force_stop_flg=1)
static VOID ct_dd_spi_pc_test_start_full_dma_6( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 dma 0 1 1 2 0 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 1 data 0xA5");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 1 data");

	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 1;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi stop 2");
	ct_dd_spi_cmd_wrap("ddspi start_full_dma 2 0 1");

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_full_reg( ch, 0, 1 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start full DMA (async, 8bit, 1byte, RXOVERFLOW=1)
static VOID ct_dd_spi_pc_test_start_full_dma_7( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 8;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 8 12000000 0 0 8 8 dma 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 1 data 0xA5");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 1 data");

	IO_SPI[ch].ST.bit.RXOVERFLOW = 1;
	IO_SPI[ch].ST.bit.TXEMPTY = 0;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi start_full_dma 2 0 1");

	gCT_DD_SPI_Async = 0;	// not need data print

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	IO_HDMAC1.DMAC[1].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 1 );

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_full_reg( ch, 0, 1 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

// start full DMA (async, 16bit, 1byte, force_stop_flg=1)
static VOID ct_dd_spi_pc_test_start_full_dma_8( VOID )
{
	INT32 ret;
	UCHAR ch = 2;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	gCT_DD_SPI_Trans_Num = 1;
	gCT_DD_SPI_Bit_Len = 16;

	//                                                                  +---dma_to:0-16777215
	//                                                                  |+---ssout:0-3
	//                                                     +-dly:0-255  || +---sspol:0-1
	//                                  +-mode:0-3         | +-Inhi:0-1 || | +---cont_trans:0-2
	//                                  | +-sig:0-3        | | +-txwmk  || | | +---cb
	//                                  | |     +-bit:4-16 | | | +-rxwmk|| | | | +---cb_ss
	//                                  | |     |          | | | |      || | | | |
	ct_dd_spi_cmd_wrap("ddspi ctrl 2 ms 0 0 lsb 16 12000000 0 0 8 8 dma 0 1 1 2 1 0");
	ct_dd_spi_cmd_wrap("ddspi set_send 2 1 data 0xA5A5");
	ct_dd_spi_cmd_wrap("ddspi set_recv 2 1 data");

	IO_SPI[ch].ST.bit.RXOVERFLOW = 0;
	IO_SPI[ch].ST.bit.TXEMPTY = 1;
	IO_SPI[ch].INT_CLR.word = 0;

	ct_dd_spi_cmd_wrap("ddspi stop 2");

	ct_dd_spi_cmd_wrap("ddspi start_full_dma 2 0 1");

	gCT_DD_SPI_Async = 0;	// not need data print

	IO_HDMAC1.DMAC[0].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 0 );

	IO_HDMAC1.DMAC[1].DMACB.bit.SS = D_DD_HDMAC1_SS_NORMAL_END;
	Dd_HDMAC1_Int_Handler( 1 );

	ret = gDD_CT_SPI_Result;

	ct_dd_spi_print_dma_full_reg( ch, 0, 1 );

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

static VOID ct_dd_spi_pc_test_set_enable_1( VOID )
{
	INT32 ret;
	UCHAR ch = 0;

	Ddim_Print(( "<%s> Start\n", __FUNCTION__ ));

	IO_SPI[ch].EN.bit.__ENABLEREQ = 1;

	ct_dd_spi_cmd_wrap("ddspi set_enable 0 8 1");

	ct_dd_spi_cmd_wrap("ddspi set_enable 0 8 0");

	ret = gDD_CT_SPI_Result;

	Ddim_Print(("<%s> End. result = 0x%08X, ch=%d\n", __FUNCTION__, ret, ch));
}

/**
 * @brief  Command main function for SPI test.
 * @param  INT32 argc	:The number of parameters
 *		   CGAR** argv	:The value of parameters
 * @return VOID
 */
VOID Ct_Dd_SPI_Main( int argc, char** argv )
{
	INT32				ret = D_DDIM_OK;
	UINT32				val;
	UINT32				index;
	INT32				tmout = D_DDIM_USER_SEM_WAIT_POL;
	UCHAR				ch;
	UCHAR				dma_ch[2];
	UCHAR				clk_div = 0;
	CHAR*				endstr;
	ULONG				baudrate;
	T_DD_SPI_CTRL		spi_ctrl;
	T_DD_SPI_SS_INFO	ss_info;
	E_DD_SPI_ENABLE_SIG	enable_sig;
	BOOL				enable;

	if( strcmp(argv[1], "open") == 0 ){				// Open
		ch = atoi( argv[2] );
		tmout = atoi(argv[3]);

		ret = Dd_SPI_Open( ch, tmout );

		Ddim_Print(("Dd_SPI_Open() End. ret=0x%08X\n", ret));
	}
	else if( strcmp(argv[1], "close") == 0 ){		// Close
		ch = atoi( argv[2] );

		ret = Dd_SPI_Close( ch );
		Ddim_Print(("Dd_SPI_Close() End. ret=0x%08X\n", ret));
	}
	else if( strcmp(argv[1], "cal") == 0 ){			// Calc
		baudrate = atoi( argv[2] );

		ret = Dd_SPI_Calculate( baudrate, &clk_div );

		Ddim_Print(("Dd_SPI_Calculate() End. ret=0x%08X, clk_div=%d\n", ret, clk_div));
	}
	else if( strcmp(argv[1], "ctrl") == 0 ){		// Set Control
		//
		// ddspi ctrl [ch] [type] [mode] [sig] [msb] [bit] [bps] [dly] [inhrx] [txwmk] [rxwmk] [dma/cpu] [timeout] [ssout] [sspol] [cont_trans] [cb] [cbss]
		//
		ch = atoi( argv[2] );

		// Type
		if( strcmp(argv[3], "ms") == 0 ){
			spi_ctrl.type = E_DD_SPI_TYPE_MASTER;
		}
		else if( strcmp(argv[3], "sl") == 0 ){
			spi_ctrl.type = E_DD_SPI_TYPE_SLAVE;
		}
		else{
			Ddim_Print( ("please check type parameter!!\n" ) );
			return;
		}

		// Mode
		spi_ctrl.mode = atoi( argv[4] );
		if( spi_ctrl.mode > E_DD_SPI_MODE_3_CPOL1_CPHA1 ){
			Ddim_Print( ("please check mode parameter!!\n" ) );
			return;
		}

		// Enable signal
		spi_ctrl.enable_sig = atoi( argv[5] );
		if( spi_ctrl.enable_sig > E_DD_SPI_ENABLE_SIG_EXT_2 ){
			Ddim_Print( ("please check enable_sig parameter!!\n" ) );
			return;
		}

		// LSB first or MSB first
		if( strcmp(argv[6], "lsb") == 0 ){
			spi_ctrl.bit_direction = E_DD_SPI_BIT_DIR_LSB_FIRST;
		}
		else if( strcmp(argv[6], "msb") == 0 ){
			spi_ctrl.bit_direction = E_DD_SPI_BIT_DIR_MSB_FIRST;
		}
		else{
			Ddim_Print( ("please check bit_direction parameter!!\n" ) );
			return;
		}

		// Bit length
		spi_ctrl.bit_length = atoi( argv[7] ) - 1;
		if( ( spi_ctrl.bit_length < E_DD_SPI_BIT_LEN_4 ) ||
			( spi_ctrl.bit_length > E_DD_SPI_BIT_LEN_32 ) ){
			Ddim_Print( ("please check bit_length parameter!!\n" ) );
			return;
		}
		gCT_DD_SPI_Bit_Len = spi_ctrl.bit_length;

		// Baudrate (bps)
		baudrate = atol(argv[8]);
		ret = Dd_SPI_Calculate( baudrate, &spi_ctrl.clk_div );
		if( ret !=D_DDIM_OK ){
			Ddim_Print(("SPI Calc ERR. ret=0x%08X\n", ret));
			return;
		}

		// Inter-Transfer Delay
		val = atoi( argv[9] );
		if( val == 0 ){
			spi_ctrl.delay_val = 0;
			spi_ctrl.delay_en = 0;
		}
		else if( val < 256 ){
			spi_ctrl.delay_val = val;
			spi_ctrl.delay_en = 1;
		}
		else {
			Ddim_Print( ("please check delay parameter!!\n" ) );
			return;
		}

		// Inhibit the RX_FIFO
		val = atoi( argv[10] );
		if( val == 0 ){
			spi_ctrl.rx_inhibit_en = 0;
		}
		else if( val == 1 ){
			spi_ctrl.rx_inhibit_en = 1;
		}
		else {
			Ddim_Print( ("please check rx_inhibit_en parameter!!\n" ) );
			return;
		}

		// TX FIFO Watermark
		spi_ctrl.fifo_wmark_tx = atoi( argv[11] );
		if( spi_ctrl.fifo_wmark_tx > gDD_CT_SPI_FIFO_Depth[ch] ){
			Ddim_Print( ("please check fifo_wmark_tx parameter!!\n" ) );
			return;
		}

		// RX FIFO Watermark
		spi_ctrl.fifo_wmark_rx = atoi( argv[12] );
		if( spi_ctrl.fifo_wmark_rx > gDD_CT_SPI_FIFO_Depth[ch] ){
			Ddim_Print( ("please check fifo_wmark_rx parameter!!\n" ) );
			return;
		}

		// DMA enable
		if( strcmp(argv[13], "dma") == 0 ){
			spi_ctrl.dma_en = 1;

			val = atoi( argv[14] );
			if( val <= 0xFFFFFF ){
				spi_ctrl.dma_recv_to = val;
			}
			else {
				Ddim_Print( ("please check dma_recv_to parameter!!\n" ) );
				return;
			}
		}
		else if( strcmp(argv[13], "cpu") == 0 ){
			spi_ctrl.dma_en = 0;
			spi_ctrl.dma_recv_to = 0;
		}
		else{
			Ddim_Print( ("please check dma_en parameter!!\n" ) );
			return;
		}

		// Slave Select Output
		val = atoi( argv[15] );
		if( val >= D_DD_SPI_SS_MAX ){
			Ddim_Print( ("please check ssout parameter!!\n" ) );
			return;
		}
		else {
			for( index = 0; index < D_DD_SPI_SS_MAX; index++ ){
				if( val == index ){
					spi_ctrl.ss_info.ssout[index] = 1;
				}
				else {
					spi_ctrl.ss_info.ssout[index] = 0;
				}
			}
		}

		// Slave Select Polarity
		val = atoi( argv[16] );
		if( val > 1 ){
			Ddim_Print( ("please check sspol parameter!!\n" ) );
			return;
		}
		else {
			for( index = 0; index < D_DD_SPI_SS_MAX; index++ ){
				spi_ctrl.ss_info.sspol[index] = val;
			}
		}

		// ssOUT Continuous Transfer
		spi_ctrl.ss_info.cont_trans = atoi( argv[17] );
		if( spi_ctrl.ss_info.cont_trans > E_DD_SPI_CONT_TRANS_ACT_FIFO_EMPTY ){
			Ddim_Print( ("please check cont_trans parameter!!\n" ) );
			return;
		}

		// Callback function
		val = atoi( argv[18] );
		if( val == 0 ){
			spi_ctrl.pcallback = NULL;
		}
		else{
			spi_ctrl.pcallback = ct_dd_spi_callback;
			gCT_DD_SPI_Async = 1;
		}

		// Callback function for ssIN/OUT
		val = atoi( argv[19] );
		if( val == 0 ){
			spi_ctrl.pcallback_ss = NULL;
		}
		else{
			spi_ctrl.pcallback_ss = ct_dd_spi_callback_ss;
			gCT_DD_SPI_Async = 1;
		}

		// Set Control
		ret = Dd_SPI_Ctrl( ch, &spi_ctrl );

		Ddim_Print(("Dd_SPI_Ctrl() End. ret=0x%08X\n", ret));
		ct_dd_spi_print_ctrl( ch, &spi_ctrl );
	}
	else if( strcmp(argv[1], "get_ctrl") == 0 ){
		T_DD_SPI_CTRL spi_ctrl;

		ch = atoi( argv[2] );

		ret = Dd_SPI_Get_Ctrl( ch, &spi_ctrl );

		Ddim_Print(("Dd_SPI_Get_Ctrl() End. ret=0x%08X\n", ret));
		ct_dd_spi_print_ctrl( ch, &spi_ctrl );
	}
	else if( strcmp(argv[1], "set_send") == 0 ){			// Set send data
		// ddspi set_send [ch] [num] data [data value]
		// ddspi set_send [ch] [num] addr [addr value] [data value]
		UCHAR	data;

		ch = atoi( argv[2] );

		// Data size
		gCT_DD_SPI_Trans_Num = atoi(argv[3]);
		if( strcmp(argv[4], "data") == 0 ){
			gDD_CT_SPI_Send_Data_Addr = 0;

			data = strtoul( argv[5], &endstr, 16 );

			if( gCT_DD_SPI_Bit_Len > E_DD_SPI_BIT_LEN_16 ){
				memset( gDD_CT_SPI_Send_Buf32, data, sizeof(gDD_CT_SPI_Send_Buf32) );	/* pgr0539 */

				// Set send data
				ret = Dd_SPI_Set_Send_Data( ch, gDD_CT_SPI_Send_Buf32, gCT_DD_SPI_Trans_Num );
			}
			
			else if( gCT_DD_SPI_Bit_Len > E_DD_SPI_BIT_LEN_8 ){
				memset( gDD_CT_SPI_Send_Buf16, data, sizeof(gDD_CT_SPI_Send_Buf16) );	/* pgr0539 */

				// Set send data
				ret = Dd_SPI_Set_Send_Data( ch, gDD_CT_SPI_Send_Buf16, gCT_DD_SPI_Trans_Num );
			}
			else {
				memset( gDD_CT_SPI_Send_Buf8, data, sizeof(gDD_CT_SPI_Send_Buf8) );		/* pgr0539 */

				// Set send data
				ret = Dd_SPI_Set_Send_Data( ch, gDD_CT_SPI_Send_Buf8, gCT_DD_SPI_Trans_Num );
			}

			Ddim_Print(("Dd_SPI_Set_Send_Data() End. ret=0x%08X\n", ret));
		}
		else if( strcmp(argv[4], "addr") == 0 ){
			gDD_CT_SPI_Send_Data_Addr = strtoul( argv[5], &endstr, 16 );

			data = strtoul( argv[6], &endstr, 16 );

			if( gCT_DD_SPI_Bit_Len > E_DD_SPI_BIT_LEN_16 ){
				memset( (VOID*)gDD_CT_SPI_Send_Data_Addr, data, (gCT_DD_SPI_Trans_Num * 4) );	/* pgr0539 */
			}
			else if( gCT_DD_SPI_Bit_Len > E_DD_SPI_BIT_LEN_8 ){
				memset( (VOID*)gDD_CT_SPI_Send_Data_Addr, data, (gCT_DD_SPI_Trans_Num * 2) );	/* pgr0539 */
			}
			else {
				memset( (VOID*)gDD_CT_SPI_Send_Data_Addr, data, gCT_DD_SPI_Trans_Num );		/* pgr0539 */
			}

			// Set send data
			ret = Dd_SPI_Set_Send_Data( ch, (VOID*)gDD_CT_SPI_Send_Data_Addr, gCT_DD_SPI_Trans_Num );

			Ddim_Print(("Dd_SPI_Set_Send_Data() End. ret=0x%08X\n", ret));
		}
		else {
			Ddim_Print( ("please check data/addr parameter!!\n" ) );
		}
	}
	else if( strcmp(argv[1], "set_recv") == 0 ){			// Set revceive data
		// ddspi set_recv [ch] [num] data
		// ddspi set_recv [ch] [num] addr [addr value]
		ch = atoi( argv[2] );

		// Data size
		gCT_DD_SPI_Trans_Num = atoi(argv[3]);

		if( strcmp(argv[4], "data") == 0 ){
			gDD_CT_SPI_Recv_Data_Addr = 0;

			if( gCT_DD_SPI_Bit_Len > E_DD_SPI_BIT_LEN_16 ){
				memset( gDD_CT_SPI_Recv_Buf32, 0, sizeof(gDD_CT_SPI_Recv_Buf32) );

				// Set receive data
				ret = Dd_SPI_Set_Recv_Data( ch, gDD_CT_SPI_Recv_Buf16, gCT_DD_SPI_Trans_Num );
			}
			else if( gCT_DD_SPI_Bit_Len > E_DD_SPI_BIT_LEN_8 ){
				memset( gDD_CT_SPI_Recv_Buf16, 0, sizeof(gDD_CT_SPI_Recv_Buf16) );

				// Set receive data
				ret = Dd_SPI_Set_Recv_Data( ch, gDD_CT_SPI_Recv_Buf16, gCT_DD_SPI_Trans_Num );
			}
			else {
				memset( gDD_CT_SPI_Recv_Buf8, 0, sizeof(gDD_CT_SPI_Recv_Buf8) );

				// Set receive data
				ret = Dd_SPI_Set_Recv_Data( ch, gDD_CT_SPI_Recv_Buf8, gCT_DD_SPI_Trans_Num );
			}
		}
		else if( strcmp(argv[4], "addr") == 0 ){
			gDD_CT_SPI_Recv_Data_Addr = strtoul( argv[5], &endstr, 16 );

			if( gCT_DD_SPI_Bit_Len > E_DD_SPI_BIT_LEN_16 ){
				memset( (VOID*)gDD_CT_SPI_Recv_Data_Addr, 0, (gCT_DD_SPI_Trans_Num * 4) );	/* pgr0539 */
			}
			else if( gCT_DD_SPI_Bit_Len > E_DD_SPI_BIT_LEN_8 ){
				memset( (VOID*)gDD_CT_SPI_Recv_Data_Addr, 0, (gCT_DD_SPI_Trans_Num * 2) );	/* pgr0539 */
			}
			else {
				memset( (VOID*)gDD_CT_SPI_Recv_Data_Addr, 0, gCT_DD_SPI_Trans_Num );			/* pgr0539 */
			}

			// Set send data
			ret = Dd_SPI_Set_Recv_Data( ch, (VOID*)gDD_CT_SPI_Recv_Data_Addr, gCT_DD_SPI_Trans_Num );
		}
		else {
			Ddim_Print( ("please check data/addr parameter!!\n" ) );
		}

		Ddim_Print(("Dd_SPI_Set_Recv_Data() End. ret=0x%08X\n", ret));
	}
	else if ( strcmp( argv[1], "start_send" ) == 0 ){		// Start send
		ch = atoi( argv[2] );
		gCT_DD_SPI_Trans_Type = E_CT_DD_SPI_TRNAS_TYPE_SEND;

		ret = Dd_SPI_Start_Send( ch );

		Ddim_Print(("Dd_SPI_Start_Send() End. ret=0x%08X\n", ret));

		if( ret == D_DDIM_OK ){
			if( !gCT_DD_SPI_Async ){
				ct_dd_spi_print_data();
			}
		}
	}
	else if ( strcmp( argv[1], "start_recv" ) == 0 ){		// Start receive
		ch = atoi( argv[2] );
		gCT_DD_SPI_Trans_Type = E_CT_DD_SPI_TRNAS_TYPE_RECV;

		ret = Dd_SPI_Start_Recv( ch );

		Ddim_Print(("Dd_SPI_Start_Recv() End. ret=0x%08X\n", ret));

		if( ret == D_DDIM_OK ){
			if( !gCT_DD_SPI_Async ){
				ct_dd_spi_print_data();
			}
		}
	}
	else if ( strcmp( argv[1], "start_full" ) == 0 ){
		ch = atoi( argv[2] );
		gCT_DD_SPI_Trans_Type = E_CT_DD_SPI_TRNAS_TYPE_FULL;

		ret = Dd_SPI_Start_Full_Duplex( ch );

		Ddim_Print(("Dd_SPI_Start_Full_Duplex() End. ret=0x%08X\n", ret));

		if( ret == D_DDIM_OK ){
			if( !gCT_DD_SPI_Async ){
				ct_dd_spi_print_data();
			}
		}
	}
	else if ( strcmp( argv[1], "start_send_dma" ) == 0 ){
		ch = atoi( argv[2] );
		dma_ch[0] = atoi( argv[3] );

		if( gCT_DD_SPI_Bit_Len > E_DD_SPI_BIT_LEN_16 ){
			if( gDD_CT_SPI_Send_Data_Addr ){
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Send_Data_Addr, gCT_DD_SPI_Trans_Num * 4 );
			}
			else {
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Send_Buf32, sizeof(gDD_CT_SPI_Send_Buf32) );
			}
		}
		else if( gCT_DD_SPI_Bit_Len > E_DD_SPI_BIT_LEN_8 ){
			if( gDD_CT_SPI_Send_Data_Addr ){
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Send_Data_Addr, gCT_DD_SPI_Trans_Num * 2 );
			}
			else {
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Send_Buf16, sizeof(gDD_CT_SPI_Send_Buf16) );
			}
		}
		else {
			if( gDD_CT_SPI_Send_Data_Addr ){
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Send_Data_Addr, gCT_DD_SPI_Trans_Num );
			}
			else {
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Send_Buf8, sizeof(gDD_CT_SPI_Send_Buf8) );
			}
		}

		ret = Dd_SPI_Start_Send_DMA( ch, dma_ch[0] );

		Ddim_Print(("Dd_SPI_Start_Send_DMA() End. ret=0x%08X\n", ret));

		if( ret == D_DDIM_OK ){
			if( !gCT_DD_SPI_Async ){
#ifndef CO_DEBUG_ON_PC
				ct_dd_spi_print_data();
#endif
			}
		}
	}
	else if ( strcmp( argv[1], "start_recv_dma" ) == 0 ){
		ch = atoi( argv[2] );
		dma_ch[0] = atoi( argv[3] );

		if( gCT_DD_SPI_Bit_Len > E_DD_SPI_BIT_LEN_16 ){
			if( gDD_CT_SPI_Recv_Data_Addr ){
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Recv_Data_Addr, gCT_DD_SPI_Trans_Num * 4 );
			}
			else {
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Recv_Buf32, sizeof(gDD_CT_SPI_Recv_Buf32) );
			}
		}
		else if( gCT_DD_SPI_Bit_Len > E_DD_SPI_BIT_LEN_8 ){
			if( gDD_CT_SPI_Recv_Data_Addr ){
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Recv_Data_Addr, gCT_DD_SPI_Trans_Num * 2 );
			}
			else {
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Recv_Buf16, sizeof(gDD_CT_SPI_Recv_Buf16) );
			}
		}
		else {
			if( gDD_CT_SPI_Recv_Data_Addr ){
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Recv_Data_Addr, gCT_DD_SPI_Trans_Num );
			}
			else {
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Recv_Buf8, sizeof(gDD_CT_SPI_Recv_Buf8) );
			}
		}

		ret = Dd_SPI_Start_Recv_DMA( ch, dma_ch[0] );

		Ddim_Print(("Dd_SPI_Start_Recv_DMA() End. ret=0x%08X\n", ret));

		if( ret == D_DDIM_OK ){
			if( !gCT_DD_SPI_Async ){
#ifndef CO_DEBUG_ON_PC
				ct_dd_spi_print_data();
#endif
			}
		}
	}
	else if ( strcmp( argv[1], "start_full_dma" ) == 0 ){
		ch = atoi( argv[2] );
		dma_ch[0] = atoi( argv[3] );
		dma_ch[1] = atoi( argv[4] );

		if( gCT_DD_SPI_Bit_Len > E_DD_SPI_BIT_LEN_16 ){
			if( gDD_CT_SPI_Send_Data_Addr ){
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Send_Data_Addr, gCT_DD_SPI_Trans_Num * 4 );
			}
			else {
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Send_Buf32, sizeof(gDD_CT_SPI_Send_Buf32) );
			}

			if( gDD_CT_SPI_Recv_Data_Addr ){
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Recv_Data_Addr, gCT_DD_SPI_Trans_Num * 4 );
			}
			else {
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Recv_Buf32, sizeof(gDD_CT_SPI_Recv_Buf32) );
			}
		}
		else if( gCT_DD_SPI_Bit_Len > E_DD_SPI_BIT_LEN_8 ){
			if( gDD_CT_SPI_Send_Data_Addr ){
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Send_Data_Addr, gCT_DD_SPI_Trans_Num * 2 );
			}
			else {
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Send_Buf16, sizeof(gDD_CT_SPI_Send_Buf16) );
			}

			if( gDD_CT_SPI_Recv_Data_Addr ){
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Recv_Data_Addr, gCT_DD_SPI_Trans_Num * 2 );
			}
			else {
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Recv_Buf16, sizeof(gDD_CT_SPI_Recv_Buf16) );
			}
		}
		else {
			if( gDD_CT_SPI_Send_Data_Addr ){
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Send_Data_Addr, gCT_DD_SPI_Trans_Num );
			}
			else {
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Send_Buf8, sizeof(gDD_CT_SPI_Send_Buf8) );
			}

			if( gDD_CT_SPI_Recv_Data_Addr ){
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Recv_Data_Addr, gCT_DD_SPI_Trans_Num );
			}
			else {
				DDIM_User_L1l2cache_Clean_Flush_Addr( (ULONG)gDD_CT_SPI_Recv_Buf8, sizeof(gDD_CT_SPI_Recv_Buf8) );
			}
		}

		ret = Dd_SPI_Start_Full_Duplex_DMA( ch, dma_ch[0], dma_ch[1] );

		Ddim_Print(("Dd_SPI_Start_Full_Duplex_DMA() End. ret=0x%08X\n", ret));

		if( ret == D_DDIM_OK ){
			if( !gCT_DD_SPI_Async ){
#ifndef CO_DEBUG_ON_PC
				ct_dd_spi_print_data();
#endif
			}
		}
	}
	else if ( strcmp( argv[1], "stop" ) == 0 ){
		ch = atoi( argv[2] );

		ret = Dd_SPI_Stop( ch );

		Ddim_Print(("Dd_SPI_Stop() End. ret=0x%08X\n", ret));
	}
	else if ( strcmp( argv[1], "set_ss" ) == 0 ){
		// ddspi set_ss [ch] [cont_trans] [ssout0] [ssout1] [ssout2] [ssout3] [sspol0] [sspol1] [sspol2] [sspol3]
		if( argc < 12 ){
			Ddim_Print( ("please check 12 parameter!!\n" ) );
			return;
		}

		ch = atoi( argv[2] );
		ss_info.cont_trans = atoi( argv[3] );

		for( index = 0; index < D_DD_SPI_SS_MAX; index++ ){
			ss_info.ssout[index] = atoi( argv[4 + index] );
		}
		for( index = 0; index < D_DD_SPI_SS_MAX; index++ ){
			ss_info.sspol[index] = atoi( argv[4 + D_DD_SPI_SS_MAX + index] );
		}

		ret = Dd_SPI_Set_Slave_Select( ch, &ss_info );

		Ddim_Print(("Dd_SPI_Set_Slave_Select End. ret=0x%08X\n", ret));
	}
	else if ( strcmp( argv[1], "get_ss" ) == 0 ){
		ch = atoi( argv[2] );

		ret = Dd_SPI_Get_Slave_Select( ch, &ss_info );

		Ddim_Print(("Dd_SPI_Get_Slave_Select End. ret=0x%08X\n", ret));

		Ddim_Print(("---Get SS_INFO---\n"));
		Ddim_Print(("cont_trans = %d\n", ss_info.cont_trans));
		for( index = 0; index < D_DD_SPI_SS_MAX; index++ ){
			Ddim_Print(("ssout[%d] = %d\n", index, ss_info.ssout[index]));
		}
		for( index = 0; index < D_DD_SPI_SS_MAX; index++ ){
			Ddim_Print(("sspol[%d] = %d\n", index, ss_info.sspol[index]));
		}
	}
	else if ( strcmp( argv[1], "set_div" ) == 0 ){
		ch = atoi( argv[2] );
		clk_div = atoi( argv[3] );

		ret = Dd_SPI_Set_Clock_Divisor( ch, clk_div );

		Ddim_Print(("Dd_SPI_Set_Clock_Divisor End. ret=0x%08X\n", ret));
	}
	else if ( strcmp( argv[1], "get_div" ) == 0 ){
		ch = atoi( argv[2] );

		ret = Dd_SPI_Get_Clock_Divisor( ch, &clk_div );

		Ddim_Print(("Dd_SPI_Get_Clock_Divisor End. ret=0x%08X, clk_div=%d\n", ret, clk_div));
	}
	else if ( strcmp( argv[1], "set_enable" ) == 0 ){
		ch = atoi( argv[2] );
		enable_sig = atoi( argv[3] );
		enable = atoi( argv[4] );

		ret = Dd_SPI_Set_SPI_Enable( ch, enable_sig, enable );

		Ddim_Print(("Dd_SPI_Set_SPI_Enable End. ret=0x%08X, EN.word=0x%02lX\n", ret, IO_SPI[ch].EN.word));
	}
	else if (strcmp(argv[1], "test") == 0){
		switch (atoi(argv[2])) {
			case 1:
				switch (atoi(argv[3])) {
					case 1:
						ct_dd_spi_pc_test_open_1();
						break;
					case 2:
						ct_dd_spi_pc_test_open_2();
						break;
					case 3:
						ct_dd_spi_pc_test_open_3();
						break;
					case 4:
						ct_dd_spi_pc_test_open_4();
						break;
					default :
						Ddim_Print(("ddspi : ERROR test number\n"));
						break;
				}
				break;
			case 2:
				switch (atoi(argv[3])) {
					case 1:
						ct_dd_spi_pc_test_close_1();
						break;
					case 2:
						ct_dd_spi_pc_test_close_2();
						break;
					default :
						Ddim_Print(("ddspi : ERROR test number\n"));
						break;
				}
				break;
			case 3:
				switch (atoi(argv[3])) {
					case 1:
						ct_dd_spi_pc_test_ctrl_1();
						break;
					case 2:
						ct_dd_spi_pc_test_ctrl_2();
						break;
					default :
						Ddim_Print(("ddspi : ERROR test number\n"));
						break;
				}
				break;
			case 4:
				switch (atoi(argv[3])) {
					case 1:
						ct_dd_spi_pc_test_set_send_data_1();
						break;
					case 2:
						ct_dd_spi_pc_test_set_send_data_2();
						break;
					default :
						Ddim_Print(("ddspi : ERROR test number\n"));
						break;
				}
				break;
			case 5:
				switch (atoi(argv[3])) {
					case 1:
						ct_dd_spi_pc_test_set_recv_data_1();
						break;
					case 2:
						ct_dd_spi_pc_test_set_recv_data_2();
						break;
					default :
						Ddim_Print(("ddspi : ERROR test number\n"));
						break;
				}
				break;
			case 6:
				switch (atoi(argv[3])) {
					case 1:
						ct_dd_spi_pc_test_start_send_1();
						break;
					case 2:
						ct_dd_spi_pc_test_start_send_2();
						break;
					case 3:
						ct_dd_spi_pc_test_start_send_3();
						break;
					case 4:
						ct_dd_spi_pc_test_start_send_4();
						break;
					case 5:
						ct_dd_spi_pc_test_start_send_5();
						break;
					case 6:
						ct_dd_spi_pc_test_start_send_6();
						break;
					case 7:
						ct_dd_spi_pc_test_start_send_7();
						break;
					case 8:
						ct_dd_spi_pc_test_start_send_8();
						break;
					case 9:
						ct_dd_spi_pc_test_start_send_9();
						break;
					default :
						Ddim_Print(("ddspi : ERROR test number\n"));
						break;
				}
				break;
			case 7:
				switch (atoi(argv[3])) {
					case 1:
						ct_dd_spi_pc_test_start_recv_1();
						break;
					case 2:
						ct_dd_spi_pc_test_start_recv_2();
						break;
					case 3:
						ct_dd_spi_pc_test_start_recv_3();
						break;
					case 4:
						ct_dd_spi_pc_test_start_recv_4();
						break;
					case 5:
						ct_dd_spi_pc_test_start_recv_5();
						break;
					case 6:
						ct_dd_spi_pc_test_start_recv_6();
						break;
					case 7:
						ct_dd_spi_pc_test_start_recv_7();
						break;
					case 8:
						ct_dd_spi_pc_test_start_recv_8();
						break;
					case 9:
						ct_dd_spi_pc_test_start_recv_9();
						break;
					case 10:
						ct_dd_spi_pc_test_start_recv_10();
						break;
					case 11:
						ct_dd_spi_pc_test_start_recv_11();
						break;
					default :
						Ddim_Print(("ddspi : ERROR test number\n"));
						break;
				}
				break;
			case 8:
				switch (atoi(argv[3])) {
					case 1:
						ct_dd_spi_pc_test_start_full_1();
						break;
					case 2:
						ct_dd_spi_pc_test_start_full_2();
						break;
					case 3:
						ct_dd_spi_pc_test_start_full_3();
						break;
					case 4:
						ct_dd_spi_pc_test_start_full_4();
						break;
					case 5:
						ct_dd_spi_pc_test_start_full_5();
						break;
					case 6:
						ct_dd_spi_pc_test_start_full_6();
						break;
					case 7:
						ct_dd_spi_pc_test_start_full_7();
						break;
					case 8:
						ct_dd_spi_pc_test_start_full_8();
						break;
					case 9:
						ct_dd_spi_pc_test_start_full_9();
						break;
					case 10:
						ct_dd_spi_pc_test_start_full_10();
						break;
					case 11:
						ct_dd_spi_pc_test_start_full_11();
						break;
					default :
						Ddim_Print(("ddspi : ERROR test number\n"));
						break;
				}
				break;
			case 9:
				switch (atoi(argv[3])) {
					case 1:
						ct_dd_spi_pc_test_stop_1();
						break;
					case 2:
						ct_dd_spi_pc_test_stop_2();
						break;
					default :
						Ddim_Print(("ddspi : ERROR test number\n"));
						break;
				}
				break;
			case 10:
				switch (atoi(argv[3])) {
					case 1:
						ct_dd_spi_pc_test_setget_ss_1();
						break;
					case 2:
						ct_dd_spi_pc_test_setget_ss_2();
						break;
					case 3:
						ct_dd_spi_pc_test_setget_ss_3();
						break;
					default :
						Ddim_Print(("ddspi : ERROR test number\n"));
						break;
				}
				break;
			case 11:
				switch (atoi(argv[3])) {
					case 1:
						ct_dd_spi_pc_test_setget_clkdiv_1();
						break;
					default :
						Ddim_Print(("ddspi : ERROR test number\n"));
						break;
				}
				break;
			case 12:
				switch (atoi(argv[3])) {
					case 1:
						ct_dd_spi_pc_test_handler_1();
						break;
					case 2:
						ct_dd_spi_pc_test_handler_2();
						break;
					case 3:
						ct_dd_spi_pc_test_handler_3();
						break;
					case 4:
						ct_dd_spi_pc_test_handler_4();
						break;
					case 5:
						ct_dd_spi_pc_test_handler_5();
						break;
					case 6:
						ct_dd_spi_pc_test_handler_6();
						break;
					case 7:
						ct_dd_spi_pc_test_handler_7();
						break;
					case 8:
						ct_dd_spi_pc_test_handler_8();
						break;
					case 9:
						ct_dd_spi_pc_test_handler_9();
						break;
					case 10:
						ct_dd_spi_pc_test_handler_10();
						break;
					case 11:
						ct_dd_spi_pc_test_handler_11();
						break;
					default :
						Ddim_Print(("ddspi : ERROR test number\n"));
						break;
				}
				break;
			case 13:
				switch (atoi(argv[3])) {
					case 1:
						ct_dd_spi_pc_test_calc_1();
						break;
					case 2:
						ct_dd_spi_pc_test_calc_2();
						break;
					case 3:
						ct_dd_spi_pc_test_calc_3();
						break;
					case 4:
						ct_dd_spi_pc_test_calc_4();
						break;
					default :
						Ddim_Print(("ddspi : ERROR test number\n"));
						break;
				}
				break;
			case 14:
				switch (atoi(argv[3])) {
					case 1:
						ct_dd_spi_pc_test_start_send_dma_1();
						break;
					case 2:
						ct_dd_spi_pc_test_start_send_dma_2();
						break;
					case 3:
						ct_dd_spi_pc_test_start_send_dma_3();
						break;
					case 4:
						ct_dd_spi_pc_test_start_send_dma_4();
						break;
					case 5:
						ct_dd_spi_pc_test_start_send_dma_5();
						break;
					case 6:
						ct_dd_spi_pc_test_start_send_dma_6();
						break;
					case 7:
						ct_dd_spi_pc_test_start_send_dma_7();
						break;
					case 8:
						ct_dd_spi_pc_test_start_send_dma_8();
						break;
					default :
						Ddim_Print(("ddspi : ERROR test number\n"));
						break;
				}
				break;
			case 15:
				switch (atoi(argv[3])) {
					case 1:
						ct_dd_spi_pc_test_start_recv_dma_1();
						break;
					case 2:
						ct_dd_spi_pc_test_start_recv_dma_2();
						break;
					case 3:
						ct_dd_spi_pc_test_start_recv_dma_3();
						break;
					case 4:
						ct_dd_spi_pc_test_start_recv_dma_4();
						break;
					case 5:
						ct_dd_spi_pc_test_start_recv_dma_5();
						break;
					case 6:
						ct_dd_spi_pc_test_start_recv_dma_6();
						break;
					case 7:
						ct_dd_spi_pc_test_start_recv_dma_7();
						break;
					case 8:
						ct_dd_spi_pc_test_start_recv_dma_8();
						break;
					default :
						Ddim_Print(("ddspi : ERROR test number\n"));
						break;
				}
				break;
			case 16:
				switch (atoi(argv[3])) {
					case 1:
						ct_dd_spi_pc_test_start_full_dma_1();
						break;
					case 2:
						ct_dd_spi_pc_test_start_full_dma_2();
						break;
					case 3:
						ct_dd_spi_pc_test_start_full_dma_3();
						break;
					case 4:
						ct_dd_spi_pc_test_start_full_dma_4();
						break;
					case 5:
						ct_dd_spi_pc_test_start_full_dma_5();
						break;
					case 6:
						ct_dd_spi_pc_test_start_full_dma_6();
						break;
					case 7:
						ct_dd_spi_pc_test_start_full_dma_7();
						break;
					case 8:
						ct_dd_spi_pc_test_start_full_dma_8();
						break;
					default :
						Ddim_Print(("ddspi : ERROR test number\n"));
						break;
				}
				break;
			case 17:
				switch (atoi(argv[3])) {
					case 1:
						ct_dd_spi_pc_test_set_enable_1();
						break;
					default :
						Ddim_Print(("ddspi : ERROR test number\n"));
						break;
				}
				break;
			default :
				Ddim_Print(("ddspi : ERROR test number\n"));
				break;
		}
	}
	else if (strcmp(argv[1], "err") == 0){
		ret = Dd_SPI_Open( D_CT_DD_SPI_CH_MAX, tmout );
		Ddim_Print(("Dd_SPI_Open(): CH number max error. ret=%X\n", ret ));

		ret = Dd_SPI_Open( 0, -2 );
		Ddim_Print(("Dd_SPI_Open(): tmout value error. ret=%X\n", ret ));


		ret = Dd_SPI_Close( D_CT_DD_SPI_CH_MAX );
		Ddim_Print(("Dd_SPI_Close(): CH number max error. ret=%X\n", ret ));


		ret = Dd_SPI_Ctrl( D_CT_DD_SPI_CH_MAX, &spi_ctrl );
		Ddim_Print(("Dd_SPI_Ctrl(): CH number max error. ret=%X\n", ret ));

		ret = Dd_SPI_Ctrl( 0, NULL );
		Ddim_Print(("Dd_SPI_Ctrl(): spi_ctrl is NULL. ret=%X\n", ret ));


		ret = Dd_SPI_Get_Ctrl( D_CT_DD_SPI_CH_MAX, &spi_ctrl );
		Ddim_Print(("Dd_SPI_Get_Ctrl(): CH number max error. ret=%X\n", ret ));

		ret = Dd_SPI_Get_Ctrl( 0, NULL );
		Ddim_Print(("Dd_SPI_Get_Ctrl(): spi_ctrl is NULL. ret=%X\n", ret ));


		ret = Dd_SPI_Set_Send_Data( D_CT_DD_SPI_CH_MAX, gDD_CT_SPI_Send_Buf8, 1 );
		Ddim_Print(("Dd_SPI_Set_Send_Data(): CH number max error. ret=%X\n", ret ));

		ret = Dd_SPI_Set_Send_Data( 0, NULL, 1 );
		Ddim_Print(("Dd_SPI_Set_Send_Data(): send_addr is NULL. ret=%X\n", ret ));

		ret = Dd_SPI_Set_Send_Data( 0, gDD_CT_SPI_Send_Buf8, 0 );
		Ddim_Print(("Dd_SPI_Set_Send_Data(): num is 0. ret=%X\n", ret ));


		ret = Dd_SPI_Start_Send( D_CT_DD_SPI_CH_MAX );
		Ddim_Print(("Dd_SPI_Start_Send(): CH number max error. ret=%X\n", ret ));


		ret = Dd_SPI_Set_Recv_Data( D_CT_DD_SPI_CH_MAX, gDD_CT_SPI_Recv_Buf8, 1 );
		Ddim_Print(("Dd_SPI_Set_Recv_Data(): CH number max error. ret=%X\n", ret ));

		ret = Dd_SPI_Set_Recv_Data( 0, NULL, 1 );
		Ddim_Print(("Dd_SPI_Set_Recv_Data(): recv_addr is NULL. ret=%X\n", ret ));

		ret = Dd_SPI_Set_Recv_Data( 0, gDD_CT_SPI_Recv_Buf8, 0 );
		Ddim_Print(("Dd_SPI_Set_Recv_Data(): num is 0. ret=%X\n", ret ));


		ret = Dd_SPI_Start_Recv( D_CT_DD_SPI_CH_MAX );
		Ddim_Print(("Dd_SPI_Start_Recv(): CH number max error. ret=%X\n", ret ));


		ret = Dd_SPI_Start_Full_Duplex( D_CT_DD_SPI_CH_MAX );
		Ddim_Print(("Dd_SPI_Start_Full_Duplex(): CH number max error. ret=%X\n", ret ));


		ret = Dd_SPI_Stop( D_CT_DD_SPI_CH_MAX );
		Ddim_Print(("Dd_SPI_Stop(): CH number max error. ret=%X\n", ret ));


		ret = Dd_SPI_Set_Slave_Select( D_CT_DD_SPI_CH_MAX, &ss_info );
		Ddim_Print(("Dd_SPI_Set_Slave_Select(): CH number max error. ret=%X\n", ret ));

		ret = Dd_SPI_Set_Slave_Select( 0, NULL );
		Ddim_Print(("Dd_SPI_Set_Slave_Select(): ss_info is NULL. ret=%X\n", ret ));


		ret = Dd_SPI_Get_Slave_Select( D_CT_DD_SPI_CH_MAX, &ss_info );
		Ddim_Print(("Dd_SPI_Get_Slave_Select(): CH number max error. ret=%X\n", ret ));

		ret = Dd_SPI_Get_Slave_Select( 0, NULL );
		Ddim_Print(("Dd_SPI_Get_Slave_Select(): ss_info is NULL. ret=%X\n", ret ));


		ret = Dd_SPI_Set_Clock_Divisor( D_CT_DD_SPI_CH_MAX, clk_div );
		Ddim_Print(("Dd_SPI_Set_Clock_Divisor(): CH number max error. ret=%X\n", ret ));


		ret = Dd_SPI_Get_Clock_Divisor( D_CT_DD_SPI_CH_MAX, &clk_div );
		Ddim_Print(("Dd_SPI_Get_Clock_Divisor(): CH number max error. ret=%X\n", ret ));

		ret = Dd_SPI_Get_Clock_Divisor( 0, NULL );
		Ddim_Print(("Dd_SPI_Get_Clock_Divisor(): clk_div is NULL. ret=%X\n", ret ));


		ret = Dd_SPI_Set_SPI_Enable( D_CT_DD_SPI_CH_MAX, E_DD_SPI_ENABLE_SIG_CPU, TRUE );
		Ddim_Print(("Dd_SPI_Set_SPI_Enable(): CH number max error. ret=%X\n", ret ));

		Dd_SPI_Int_Handler( D_CT_DD_SPI_CH_MAX );
		Ddim_Print(("Dd_SPI_Int_Handler(): CH number error.\n"));


		ret = Dd_SPI_Calculate( 1, NULL );
		Ddim_Print(("Dd_SPI_Calculate(): clk_div is NULL. ret=%X\n", ret ));


		ret = Dd_SPI_Start_Send_DMA( D_CT_DD_SPI_CH_MAX, 5 );
		Ddim_Print(("Dd_SPI_Start_Send_DMA(): SPI CH number max error. ret=%X\n", ret ));

		ret = Dd_SPI_Start_Send_DMA( 0, D_DD_HDMAC1_CH_NUM_MAX );
		Ddim_Print(("Dd_SPI_Start_Send_DMA(): DMA CH number max error. ret=%X\n", ret ));


		ret = Dd_SPI_Start_Recv_DMA( D_CT_DD_SPI_CH_MAX, 5 );
		Ddim_Print(("Dd_SPI_Start_Recv_DMA(): SPI CH number max error. ret=%X\n", ret ));

		ret = Dd_SPI_Start_Recv_DMA( 0, D_DD_HDMAC1_CH_NUM_MAX  );
		Ddim_Print(("Dd_SPI_Start_Recv_DMA(): DMA CH number max error. ret=%X\n", ret ));


		ret = Dd_SPI_Start_Full_Duplex_DMA( D_CT_DD_SPI_CH_MAX, 0, 1 );
		Ddim_Print(("Dd_SPI_Start_Full_DMA(): SPI CH number max error. ret=%X\n", ret ));

		ret = Dd_SPI_Start_Full_Duplex_DMA( 0, D_DD_HDMAC1_CH_NUM_MAX, 7 );
		Ddim_Print(("Dd_SPI_Start_Full_DMA(): DMA CH number max error. ret=%X\n", ret ));

		ret = Dd_SPI_Start_Full_Duplex_DMA( 0, 7, D_DD_HDMAC1_CH_NUM_MAX );
		Ddim_Print(("Dd_SPI_Start_Full_DMA(): DMA CH number max error. ret=%X\n", ret ));
	}
	else{
		Ddim_Print(("please check 1st parameter!!\n"));
	}

	gDD_CT_SPI_Result = ret;

	return ;
}
