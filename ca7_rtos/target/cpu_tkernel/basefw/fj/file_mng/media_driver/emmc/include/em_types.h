// ************************************************************* //
//                                                               //
//        Copyright (C) 2015 by Socionext Inc.                   //
//        All rights reserved.                                   //
//        Socionext Inc. Proprietary and Confidential.           //
//                                                               //
// ************************************************************* //
/*****************************************************************************/
/**
 * @file em_types.h
 * @brief common type definition
 * @note None
 * @attention None
 *
 */
/*****************************************************************************/

#ifndef EM_TYPES_H
#define EM_TYPES_H

#include "em_config.h"
#include "em_host_register.h"
#include "em_dev_register.h"
#include "em_int_cmn.h"

/// Host information
typedef struct EM_HOST EM_HOST ;

/**
 * @struct EM_REQUEST_FLAGS
 * @brief flag group for command requst information
 *
 */
typedef struct EM_REQUEST_FLAGS
{
	/// Command Type 
	EM_COMMAND_TYPE e_commandType;
	/// Response type
	EM_RESPONSE_TYPE e_responseType;
	/// Transfer derection
	EM_TRANSFER_DIRECTION e_dataTransferDirection;
	/// @brief data flag
	/// @details 1 - use, 0 - not use 
	unsigned char uc_dataPresent;
	/// @brief DMA flag
	/// @details 1 - use, 0 - not use
	unsigned char uc_dma_use;
	/// padding
	unsigned char uc_reserv[2];
}  EM_REQUEST_FLAGS;


/**
 * @struct EM_REQUEST_PARAM
 * @brief Command request parameters
 *
 */
typedef struct EM_REQUEST_PARAM
{
	/// Response pointer
	unsigned long* p_response;
	/// Data buffer pointer
	void* pDataBuffer;
	/// Buffer pointer in access
	void* pBufferPos;
	/// ADMA descriptor table pointer
	unsigned long* pAdmaDescriptorTable;
	/// Owner host pointer
	EM_HOST* pOwnerHost;
	/// Request flags
	EM_REQUEST_FLAGS sRequestFlags;
	/// Argument(set to SRS2)
	unsigned long ul_argument;
	/// Response
	unsigned long ul_responseTab[ EM_RESP_TAB_SIZE ];
	/// Block count
	unsigned long ul_blockCount;
	/// Access byte num
	unsigned long ul_dataRemaining;
	/// Block length
	unsigned short us_blockLen;
	/// Command number
	unsigned char uc_command;
	/// Command status
	unsigned char uc_status;
	/// Interrupt check area
	unsigned char uc_InterruptCheck ;
	/// Response sizes
	unsigned char uc_respSize;
	/// padding
	unsigned char uc_reserv[2];
}  EM_REQUEST_PARAM;

/**
 * @struct EM_MEMORY_CARD_INFO
 * @brief Card information
 */
typedef struct EM_MEMORY_CARD_INFO
{
	/// Device size
	unsigned long ul_DeviceSizeMB;
	/// Max frequency
	unsigned long ul_max_frequency;
	/// Block size
	unsigned short us_BlockSize;
	/// padding
	unsigned char uc_reserv[2];
} EM_MEMORY_CARD_INFO, *PEM_MEMORY_CARD_INFO;
	
/**
 * @struct EM_DEVICE
 * @brief Device information
 */
typedef struct EM_DEVICE
{
	/// Slot
	struct EM_SLOT *pSlot;
	/// Card informatoin
	EM_MEMORY_CARD_INFO sCardInfo;
	/// Relative card address
	unsigned short us_RCA;
	/// Device capacity
	unsigned char uc_DeviceCapacity;
	/// Specification Version Number
	unsigned char uc_SpecVersNumb;
	/// Support Bus width @ref EM_BUS_WIDTH
	unsigned char uc_SupportedBusWidths;
	/// Device select
	unsigned char uc_IsSelected;
	/// padding
	unsigned char uc_reserv[2];
} EM_DEVICE;

/**
 * @struct EM_SLOT
 * @brief Slot information
 *
 */
typedef struct EM_SLOT {
	/// Work area
	unsigned long ul_AuxBuff[ EXTCSD_SIZE / 4 ];
	/// Extends CSD register area
	unsigned long ul_ExtendsCSD[ EXTCSD_SIZE / 4 ];
	/// CSD register area
	unsigned long ul_CSDinfo[4];
	/// SRS base address
	EM_REGISTER_SRS *pRegSRS ; 
	/// Device
	EM_DEVICE *pDevice;
	/// Current request
	EM_REQUEST_PARAM* pCurrentRequest;
	/// Parent host
	struct EM_HOST *pParentHost;
	/// Descriptor buffer
	unsigned long *pDescriptorBuffer;
	/// Access mode
	EM_ACCESS_MODE e_AccessMode;
	/// DMA mode
	EM_DMAMODE eDmaMode;
	/// DMA buffer boundary(for SRS1)
	unsigned long ul_DMABufferBoundary;
	/// Device information
	EM_DEVICE sDevices[ EM_CONFIG_MAX_DEV_PER_SLOT ];
	/// Bus width
	unsigned char uc_BusWidth;
	/// @brief Card inserted
	/// @details 1 - Inset, 0 - Eject
	unsigned char uc_CardInserted;
	/// Slot number
	unsigned char uc_SlotNum;
	/// @brief Attach request
	/// @details After the card is inserted, the flag to make sure to attach process is complete.
	unsigned char uc_NeedAttach;
	/// Error recover
	unsigned char uc_errorRecover ;
	/// padding
	unsigned char uc_reserv[3];

} EM_SLOT;

/**
 * @struct EM_HOST
 * @brief Host information
 *
 */
struct EM_HOST {
	/// Slot information
	EM_SLOT sSlots[ EM_CONFIG_SLOT_COUNT ];
	/// Slot count
	unsigned char uc_SlotCount;
	/// Specification Version Number
	unsigned char uc_SpecVerNum;
	/// HRS base address
	EM_REGISTER_SFR *pBaseAddress ;
	/// padding
	unsigned char uc_reserv[2];
} ;

#endif
