// ************************************************************* //
//                                                               //
//        Copyright (C) 2015 by Socionext Inc.                   //
//        All rights reserved.                                   //
//        Socionext Inc. Proprietary and Confidential.           //
//                                                               //
// ************************************************************* //
/*****************************************************************************/
/**
* @file em_memorycard.h
* @brief memory card access related definition
* @note None
* @attention None
*
*/
/*****************************************************************************/
#ifndef EM_MEMORYCARD_H
#define EM_MEMORYCARD_H


// include -------------------------------------
#include "em_types.h"

/// Sector Size
#define EM_SECTOR_SIZE						  512

// function -------------------------------------
extern unsigned long em_memoryCard_Initialize(
	EM_SLOT *pSlot
);
	
extern unsigned long em_memoryCard_GetCSD(
	EM_DEVICE* pDevice
);

extern unsigned long em_memoryCard_SetBlockLength(
	EM_DEVICE* pDevice,
	unsigned long ul_BlockLength
);

extern unsigned long em_memoryCard_DataTransfer(
	EM_DEVICE* pDevice,
	unsigned long ul_Sector,
	unsigned char *pBuffer,
	unsigned long ul_BufferSize,
	EM_TRANSFER_DIRECTION e_TransferDirection
);

#endif


