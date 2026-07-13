// ************************************************************* //
//                                                               //
//        Copyright (C) 2015 by Socionext Inc.                   //
//        All rights reserved.                                   //
//        Socionext Inc. Proprietary and Confidential.           //
//                                                               //
// ************************************************************* //
/*****************************************************************************/
/**
* @file em_command.h
* @brief eMMC issue commands
* @note None
* @attention None
*
*/
/*****************************************************************************/
#ifndef EM_COMMAND_H_INCLUDE
#define EM_COMMAND_H_INCLUDE

// include -------------------------------------
#include "em_int_cmn.h"
#include "em_types.h"
#include "em_general.h"



//---------------------------------------------------------------------------
/// @name Command definition
//---------------------------------------------------------------------------
//@{
/// Resets the Device to idle state
#define EM_CMD0	   0
/// Asks Device, in idle state, to send its Operating Conditions Register contents in the response on the CMD line.
#define EM_CMD1	   1
/// Asks Device to send its CID number on the CMD line
#define EM_CMD2	   2
/// Assigns relative address to the Device
#define EM_CMD3	   3
/// Switches the mode of operation of the selected Device or modifies the EXT_CSD registers.
#define EM_CMD6	   6
/// Command toggles a device between the standby and transfer states or between the programming and disconnect states
#define EM_CMD7	   7
/// Device sends its EXT_CSD register as a block of data.
#define EM_CMD8	   8
/// Addressed Device sends its Device-specific data (CSD) on the CMD line
#define EM_CMD9	   9
/// Forces the Device to stop transmission. 
#define EM_CMD12   12
/// Device sends status. 
#define EM_CMD13   13
/// Sets the block length (in bytes) for all following block commands
#define EM_CMD16   16
/// Reads a block of the size selected by the SET_BLOCKLEN command.
#define EM_CMD17   17
/// Continuously transfers data blocks from Device to host until interrupted by a stop command,
#define EM_CMD18   18
/// 128 clocks of tuning pattern (64byte in 4 bit mode or 128byte in 8 bit mode) is sent for HS200 optimal sampling point detection.
#define EM_CMD21   21
/// Writes a block of the size selected by the SET_BLOCKLEN command.
#define EM_CMD24   24
/// Continuously writes blocks of data until a STOP_TRANSMISSION follows or the requested number of block received.
#define EM_CMD25   25

//}@
//---------------------------------------------------------------------------
/// @name CMD6:Access modes 
//---------------------------------------------------------------------------
//@{
/// The command set is changed according to the Cmd Set field of the argument
#define EM_CMD6_ARG_MODE_COMMAND_SET (0x0uL << 24)
/// The bits in the pointed byte are set, according to the '1' bits in the Value field
#define EM_CMD6_ARG_MODE_SET_BITS	 (0x1uL << 24)
/// bits in the pointed byte are cleared, according to the '1' bits in the Value field
#define EM_CMD6_ARG_MODE_CLEAR_BITS	 (0x2uL << 24)
/// The Value field is written into the pointed byte
#define EM_CMD6_ARG_MODE_WRITE_BYTE	 (0x3uL << 24)
//@}


/// @brief command issue
/// @details Set the value of each parameter to the req parameter.
/// @li req...@ref EM_REQUEST_PARAM
/// @li cmd...Set value to the uc_command
/// @li arg...Set value to the ul_argument
/// @li cmd_type...Set value to the sRequestFlags.e_commandType
/// @li resp_type...Set value to the sRequestFlags.e_responseType
#define EM_REQ_INIT_CMD(req, cmd, arg, cmd_type, resp_type ) \
{\
	req.uc_command = cmd;\
	req.ul_argument = arg;\
	req.uc_status = EM_DISABLE;\
	req.sRequestFlags.e_commandType = cmd_type;\
	req.sRequestFlags.e_responseType = resp_type;\
	req.sRequestFlags.uc_dataPresent = EM_DISABLE;\
	req.sRequestFlags.uc_dma_use = EM_DISABLE;\
	req.sRequestFlags.e_dataTransferDirection = EM_TRANSFER_DEFAULT;\
	req.ul_blockCount = 0;\
	req.us_blockLen = 0;\
	req.pDataBuffer = NULL;\
	req.pBufferPos = NULL;\
	req.uc_InterruptCheck = 0;\
} 

/// @brief command issue(data transfer)
/// @details Set the value of each parameter to the req parameter.
/// @li req...@ref EM_REQUEST_PARAM
/// @li cmd...Set value to the uc_command
/// @li arg...Set value to the ul_argument
/// @li cmd_type...Set value to the sRequestFlags.e_commandType
/// @li resp_type...Set value to the sRequestFlags.e_responseType
/// @li buf...Set value to the sRequestFlags.pDataBuffer
/// @li blk_count...Set value to the sRequestFlags.ul_blockCount
/// @li blk_len...Set value to the sRequestFlags.us_blockLen
/// @li dir...Set value to the sRequestFlags.e_dataTransferDirection
#define EM_REQ_INIT_CMD_WITH_DATA(req, cmd, arg, cmd_type, resp_type, buf, blk_count, blk_len, dir) \
{\
	req.uc_command = cmd;\
	req.ul_argument = arg;\
	req.uc_status = EM_DISABLE;\
	req.sRequestFlags.e_commandType = cmd_type;\
	req.sRequestFlags.e_responseType = resp_type;\
	req.sRequestFlags.uc_dataPresent = EM_ENABLE;\
	req.sRequestFlags.uc_dma_use = EM_DISABLE;\
	req.sRequestFlags.e_dataTransferDirection = dir;\
	req.ul_blockCount = blk_count;\
	req.us_blockLen = blk_len;\
	req.pDataBuffer = buf;\
	req.pBufferPos = NULL;\
	req.uc_InterruptCheck = 0;\
} 

// function -------------------------------------
extern unsigned long em_CmdResetCard(
	EM_SLOT* pSlot
);

extern unsigned long em_CmdSetMemCardVoltage(
	EM_SLOT* pSlot,
	unsigned long Voltage,
	unsigned char *CCS
);

extern unsigned long em_CmdSendCID(
	EM_SLOT* pSlot
);

extern unsigned long em_CmdSetRCA(
	EM_SLOT* pSlot
);

extern unsigned long em_CmdSelectCard(
	EM_SLOT* pSlot,
	unsigned short us_RCA,
	unsigned char uc_forced
);

extern unsigned long em_CmdReadCSD(
	EM_SLOT* pSlot,
	unsigned long *pBuffer
);

extern unsigned long em_CmdSetBlockLen(
	EM_SLOT* pSlot,
	unsigned long ul_argument,
	unsigned char uc_forced
);

unsigned long  em_CmdReadExCSD(
	EM_SLOT* pSlot,
	unsigned char *pBuffer
);

unsigned long em_CmdMmcSwitch(
	EM_SLOT* pSlot, 
	unsigned char uc_ArgIndex, 
	unsigned char uc_ArgValue
);

extern unsigned long em_CmdStopTransmission(
	EM_SLOT* pSlot,
	EM_RESPONSE_TYPE e_ResponseType
);

extern unsigned long em_CmdAbortTransmission(
	EM_SLOT* pSlot,
	EM_RESPONSE_TYPE e_ResponseType
);

extern unsigned long em_CmdBlockReadWrite(
	EM_SLOT* pSlot,
	unsigned long ul_argument,
	unsigned long ul_BlockCount,
	unsigned short us_BlockLen,
	unsigned char *pBuffer,
	unsigned char *pDMA_Use,
	EM_TRANSFER_DIRECTION e_TransferDirection
);

extern unsigned long em_CmdSendStatus(
	EM_SLOT* pSlot,
	unsigned long *p_device_status
);

extern unsigned long  em_CmdSendTuning(
	EM_SLOT* pSlot,
	unsigned char *pBuffer
);
#endif
