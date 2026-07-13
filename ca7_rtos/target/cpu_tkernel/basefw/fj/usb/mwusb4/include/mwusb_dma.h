/**
 * @file		mwusb_dma.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef _MWUSB_DMA_H_
#define _MWUSB_DMA_H_

#define	MWUSB_DMA_INCR_IN_BLK           (64)    //block size
#define	MWUSB_DMA_INCR_OUT_BLK          (64)    //block size
#define	MWUSB_DMA_INCR_IN_BLK_SHORT     (4)     //block size for short packet
#define	MWUSB_DMA_INCR_OUT_BLK_SHORT    (4)     //block size for short packet
#define	MWUSB_DMA_BLKCNT_MAX            (65536) //Maximum block count on HDMAC
#define MWUSB_DMA_TRANS_DATASIZE_MAX    (32768) //Bulk-IN Data Max size

/*--------------------------------------*/
/*   Definition of Function ProtoType   */
/*--------------------------------------*/
// for USB
extern void MWUSB_Init_Dma(void);
extern int MWUSB_Set_Dma_Write(unsigned char usb_dma_ch, unsigned long src_addr, unsigned long size, unsigned long block);
extern int MWUSB_Set_Dma_Read(unsigned char usb_dma_ch, unsigned long dst_addr, unsigned long size, unsigned long block);
extern int MWUSB_Wait_Dma_End(unsigned char usb_dma_ch);
extern int MWUSB_Abort_Dma(unsigned char usb_dma_ch);

/* 
 * Do not use this function(API) in MB86S22A(M-7M).
 */
#if 0 /* M-7M */
extern int MWUSB_Wait_Dma_End_Event(unsigned char usb_dma_ch);
#endif /* M-7M */

#endif
