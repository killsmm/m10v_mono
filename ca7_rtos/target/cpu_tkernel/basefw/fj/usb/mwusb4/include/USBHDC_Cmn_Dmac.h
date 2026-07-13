/**
 * @file		USBHDC_Cmn_Dmac.h
 * @brief		This is a header file for USB Host & Device dma control function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef H_USBHDC_CMN_DMAC
#define H_USBHDC_CMN_DMAC

#include "USBHDC_StdDef.h"

/* macro definition declaration */
#define DMA_CH1		0
#define DMA_CH2		1

/* register bit position & value */
/* DMAC1 or DMAC2 register */
#define DMAC_DMA_ST			0x1
#define DMAC_DMA_BUSY		0x2
#define DMAC_DMA_MODE		0x4
#define DMAC_DMA_SENDNULL	0x8
#define DMAC_DMA_INT_EMPTY	0x10
#define DMAC_DMA_SPR		0x20

/* DMAS1 or DMAS2 register */
#define DMAS_DMA_NP		0x00000001
#define DMAS_DMA_SP		0x00000002

/* dma soft state */
#define DMA_STATE_NORMAL		0x0
#define DMA_STATE_BUSY			0x4
#define DMA_STATE_BUSRESET		0x5
#define DMA_STATE_SETCONFIG		0x6
#define DMA_STATE_SETINTERFACE	0x7
#define DMA_STATE_SOFT_STOP		0x8

/* event flag */
#define DMA_FLAG_CH1_NORMAL_END		0x00000011
#define DMA_FLAG_CH1_ERROR_END		0x00000012
#define DMA_FLAG_CH1_ABORT			0x00000014
#define DMA_FLAG_CH2_NORMAL_END		0x00002100
#define DMA_FLAG_CH2_ERROR_END		0x00002200
#define DMA_FLAG_CH2_ABORT			0x00002400

/* function prototype declaration */
/* start dmac. */
extern void Cmn_StartDmac(u8 dma_ch);

/* stop dmac. */
extern void Cmn_StopDmac(u8 dma_ch);

/* inquire dmac busy. */
extern u8 Cmn_IsBusyDmac(u8 dma_ch);

/* set dmac transfer mode. */
extern void Cmn_SetDmacTransferMode(u8 dma_ch, u8 mode);

/* set null packet tarnasmission for dmac IN */
extern void Cmn_SetDmacNullTransfer(u8 dma_ch, u8 val);

/* set stop condition of dmac IN transfer. */
extern void Cmn_SetDmacEndCondition(u8 dma_ch, u8 val);

/* set stop condition of dmac OUT transfer. */ 
extern void Cmn_SetDmacShortPacketRcv(u8 dma_ch, u8 val);

/* set endpoint of dmac transfer object. */
extern void Cmn_SetDmacEndpoint(u8 dma_ch, u8 ep);

#if 0
/* get endpoint number of dmac setting. */
extern u8 Cmn_GetDmacEndpoint(u8 dma_ch);
#endif

/* set block tarnsfer size of dmac. */
extern void Cmn_SetDmacBlockTransferSize(u8 dma_ch, u16 size);

/* get dma transfer state. */
extern u8 Cmn_GetDmaTransferState(u8 dma_ch);

/* set tarnsfer size of dmac. */
extern void Cmn_SetDmacTransferSize(u8 dma_ch, u32 size);

/* get transfer size of dmac. */
extern u32 Cmn_GetDmacTransferSize(u8 dma_ch);

#endif

