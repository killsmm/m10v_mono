// ************************************************************* //
//                                                               //
//        Copyright (C) 2015 by Socionext Inc.                   //
//        All rights reserved.                                   //
//        Socionext Inc. Proprietary and Confidential.           //
//                                                               //
// ************************************************************* //
/*****************************************************************************/
/**
 * @file em_dma.h
 * @brief 
 * @note None
 * @attention None
 *
 */
/*****************************************************************************/

#ifndef EM_DMA_H
#define EM_DMA_H

#include "em_types.h"

/// ADMA descriptor size
#define EM_ADMA2_SIZE_OF_DESCRIPTOR_32		8u
/// Set address/lenght field
#define EM_ADMA2_DESCRIPTOR_LENGHT(Val) ((unsigned long)(Val & 0xFFFF) << 16)
/// Transfer data from the pointed page and go to next descriptor on the list.
#define EM_ADMA2_DESCRIPTOR_TYPE_TRAN  (0x2 << 4)
/// @brief it signals termination of the transfer
/// and generates Transfer Complete Interrupt
/// when this transfer is completed
#define EM_ADMA2_DESCRIPTOR_END		   (0x1 << 1)
/// it indicates the valid descriptor on a list
#define EM_ADMA2_DESCRIPTOR_VALID	   (0x1 << 0)



/**
 * @struct EM_DMA_SUBBUFFER
 * @brief structure contains info about one buffer. Used to transfer group of buffers
 */
typedef struct EM_DMA_SUBBUFFER
{
	/** Address of sub buffer */
	unsigned long address;
	/** Size in bytes of sub buffer */
	unsigned long size;
}  EM_DMA_SUBBUFFER;


extern unsigned long em_DMA_handleInterrupt(
	EM_SLOT* pSlot,
	EM_REQUEST_PARAM* pRequest,
	unsigned long ul_srs12
);

extern void em_DMA_HandleADMAError(
	EM_SLOT* pSlot
);

extern EM_DMAMODE em_DMA_specTransMode(
	EM_SLOT* pSlot,
	EM_REQUEST_PARAM* pRequest
);

extern unsigned char em_DMA_prepareTransfer(
	EM_SLOT* pSlot,
	EM_REQUEST_PARAM* pRequest
);

#endif
