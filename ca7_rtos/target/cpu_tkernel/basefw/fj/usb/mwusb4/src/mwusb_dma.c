/**
 * @file		mwusb_dma.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

// DeviceDriver Header
#include "l1l2cache.h"

#include "mwusb_driver4.h"
#include "mwusb_dma.h"
#include "dd_arm.h"
#include "sdram_map_common.h"
#include "USBHDC_StdDef.h"
#include "USBHDC_Dma_Dmac.h"


/*-----------------------------------------------*/
/*   Definition of Internal Function ProtoType   */
/*-----------------------------------------------*/
/* 
 * Do not call this function in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
static void mwusb_dma_wait_event(unsigned char ch);
#endif /* M-7M */


/************************************************************************/
/* <Function>	void mwusb_dma_wait_event								*/
/* <Arguments>	ch	DMA channel no										*/
/* <Return  >	void													*/
/* <Description> wait for DMA event										*/
/************************************************************************/
/* 
 * Do not call this function in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
static void mwusb_dma_wait_event(unsigned char ch)
{
#if 1	// tmp
	while ((IO_DMAC.DMAC[ch].DMACB.word & D_MWDMA_DMACB_SS_MSK) == 0);

	if ((IO_DMAC.DMAC[ch].DMACB.word & D_MWDMA_DMACB_SS_MSK) != D_MWDMA_DMACB_SS_NORMAL) {
		// Error
	}

	IO_DMAC.DMAC[ch].DMACB.bit.SS = 0;		// status clear
#endif
	return;
}
#endif /* M-7M */

static INT32 mwusb_dma_cache_aera_check( ULONG addr )
{
	UINT32 ttbaddr;				// Translation table base Address
	UINT32 ttbdata;				// Translation table base Data
	UINT32 l1cache_flg = 0;		// L1 Cache run flag

	// Get Translation table base Address
	ttbaddr = Dd_ARM_Get_TTB();
	// Translation table base Convert to PageTable
	ttbaddr = (ttbaddr & 0xFFFFC000) | ((addr & 0xFFF00000) >> 20) * 4;
	ttbdata = *((volatile unsigned int*)(ttbaddr));
	// Cache run flag
	switch((ttbdata >> 2) & 0x3){
		case 1:
		case 2:
		case 3:
			l1cache_flg = 1;
			break;
	}
	return l1cache_flg;
}

/************************************************************************/
/* <Function>	void MWUSB_Init_Dma										*/
/* <Arguments>	void													*/
/* <Return  >	void													*/
/* <Description> initalize DMA		 									*/
/************************************************************************/
extern void MWUSB_Init_Dma(void)
{
    /* Initalize DMA */
    if (HdcDev_InitDmac() == HDC_FAILURE) {
#ifdef	USE_UART_MESSAGE
        printf("ERROR: HdcDev_InitDmac in MWUSB_Init_Dma.\n\r");
#endif
    }
    
    return;
}



/************************************************************************/
/* <Function>	int MWUSB_Set_Dma_Write									*/
/* <Arguments>	usb_dma_ch	DMA channel no								*/
/*				src_addr	source data address							*/
/*				size		size of transfer data						*/
/*				block		size of transfer data block					*/
/* <Return  >	condition OK=0,NG=-1									*/
/* <Description> start to DMA write	 									*/
/************************************************************************/
extern int MWUSB_Set_Dma_Write(unsigned char usb_dma_ch, unsigned long src_addr, unsigned long size, unsigned long block)
{
	int						ret = MWUSB_OK;
	T_L1L2CACHE_INFORMATION	l1l2cache;

	/* Cache Flush */
	if ( mwusb_dma_cache_aera_check( (ULONG)src_addr) == 1 ) {
		l1l2cache.l1l2cache_type	= E_BF_L1L2CACHE_ADDR;
		l1l2cache.l1l2cache_method	= E_BF_L1L2CACHE_CLEAN_FLUSH;
		l1l2cache.l1l2cache_address	= src_addr;
		l1l2cache.l1l2cache_size	= size;
		BF_L1l2cache_Run( l1l2cache );
	}

	/* Start DMA */
	if ( HdcDev_StartDmac( usb_dma_ch, 0/* RAM to USB */, src_addr, size, block ) == HDC_FAILURE ) {
#ifdef	USE_UART_MESSAGE
		printf("ERROR: HdcDev_StartDmac in MWUSB_Set_Dma_Write.\n\r");
#endif
		ret = MWUSB_NG;
	}

	return ret;
}



/************************************************************************/
/* <Function>	int MWUSB_Set_Dma_Read									*/
/* <Arguments>	usb_dma_ch	DMA channel no								*/
/*				dst_addr	dstination address							*/
/*				size		size of transfer data						*/
/*				block		size of transfer data block					*/
/* <Return  >	condition OK=0,NG=-1									*/
/* <Description> start to DMA read	 									*/
/************************************************************************/
extern int MWUSB_Set_Dma_Read(unsigned char usb_dma_ch, unsigned long dst_addr, unsigned long size, unsigned long block)
{
	int						ret = MWUSB_OK;
	T_L1L2CACHE_INFORMATION	l1l2cache;

	if ( mwusb_dma_cache_aera_check( (ULONG)dst_addr) == 1 ) {
		l1l2cache.l1l2cache_type	= E_BF_L1L2CACHE_ADDR;
		l1l2cache.l1l2cache_method	= E_BF_L1L2CACHE_CLEAN_FLUSH;
		l1l2cache.l1l2cache_address	= dst_addr;
		l1l2cache.l1l2cache_size	= size;
		BF_L1l2cache_Run( l1l2cache );
	}

	/* Start DMA */
	if ( HdcDev_StartDmac( usb_dma_ch, 1/* USB to RAM */, dst_addr, size, block ) == HDC_FAILURE ) {
#ifdef	USE_UART_MESSAGE
		printf("ERROR: HdcDev_StartDmac in MWUSB_Set_Dma_Read.\n\r");
#endif
		ret = MWUSB_NG;
	}

	return ret;
}



/************************************************************************/
/* <Function>	int MWUSB_Wait_Dma_End									*/
/* <Arguments>	usb_dma_ch	DMA channel no								*/
/* <Return  >	condition OK=0,NG=-1									*/
/* <Description> wait for DMA transfer									*/
/************************************************************************/
extern int MWUSB_Wait_Dma_End(unsigned char usb_dma_ch)
{
    int ret = MWUSB_OK;
    
    /* Wait DMA End */
    if (HdcDev_WaitDmac(usb_dma_ch, 300/* ms :time out */) == HDC_FAILURE) {
#ifdef	USE_UART_MESSAGE
        printf("ERROR: HdcDev_WaitDma in MWUSB_Wait_Dma_End.\n\r");
#endif
        ret = MWUSB_NG;
    }
    
    return ret;
}


/************************************************************************/
/* <Function>	int MWUSB_Wait_Dma_End_Event							*/
/* <Arguments>	usb_dma_ch	DMA channel no								*/
/* <Return  >	condition OK=0,NG=-1									*/
/* <Description> wait for DMA transfer Event							*/
/************************************************************************/
/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
extern int MWUSB_Wait_Dma_End_Event(unsigned char usb_dma_ch)
{
	if(usb_dma_ch > 1){
		printf("paramter error\n");
		return -1;
	}
	mwusb_dma_wait_event(usb_dma_ch);
	Im_Sdiu_Flush_Buffer(E_IM_SDIU_FRI_NUM_2);
	
	return 0;
}
#endif /* M-7M */


/************************************************************************/
/* <Function>	int MWUSB_Abort_Dma										*/
/* <Arguments>	usb_dma_ch	DMA channel no								*/
/* <Return  >	condition OK=0,NG=-1									*/
/* <Description> abort DMA transfer										*/
/************************************************************************/
int MWUSB_Abort_Dma(unsigned char usb_dma_ch)
{
    int ret = MWUSB_OK;
    
    /* Abort */
    if (HdcDev_AbortDmac(usb_dma_ch) == HDC_FAILURE) {
        ret = MWUSB_NG;
    }
    
    return ret;
}

