/**
 * @file		USBHDC_Dma_Dmac.h
 * @brief		DMAC control function has been described
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef H_USBHDC_DMA_DMAC
#define H_USBHDC_DMA_DMAC

#include "USBHDC_StdDef.h"

/* DMAC API function */
/* Interrupt Handler */
void HdcDev_DmaIntHandler(unsigned char dma_ch);

/* initialize function */
int HdcDev_InitDmac(void);

/* start function */
int HdcDev_StartDmac(unsigned char dma_ch, unsigned char dir, unsigned long addr, unsigned long size, unsigned long block);

/* abort function */
int HdcDev_AbortDmac(unsigned char dma_ch);

/* wait for dma end function */
int HdcDev_WaitDmac(unsigned char dma_ch, unsigned long timeout);

#endif
