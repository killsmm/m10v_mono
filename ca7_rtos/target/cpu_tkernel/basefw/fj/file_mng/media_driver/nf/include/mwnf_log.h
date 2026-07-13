/**
 * @file		mwnf_log.h
 * @brief		Nand Flash driver logical header file
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef __MWNF_LOG_H
#define __MWNF_LOG_H

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype Declaration										*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

// lba to ce
extern UINT32 MWNF_log_lba2ce(UINT32 lba);

// pba to ce
extern UINT32 MWNF_log_pba2ce(UINT32 pba);

// lba to lba_device
extern UINT32 MWNF_log_lba2lba_device(UINT32 lba);

// pba to pba_device
extern UINT32 MWNF_log_pba2pba_device(UINT32 pba);

// pba_device to pba
extern UINT32 MWNF_log_pba_device2pba(UINT32 ce, UINT32 pba_device);

// lba to pba
extern UINT32 MWNF_log_lba2pba(UINT32 lba);

// set log2phy_tbl
extern VOID MWNF_log_set_log2phy_tbl(UINT32 lba, UINT32 pba);

// free_blk_index to ce
extern UINT32 MWNF_log_free_blk_index2ce(UINT32 index);

// free_blk_index to index_device
extern UINT32 MWNF_log_free_blk_index2index_device(UINT32 index);

// free_blk_index to pba
extern UINT32 MWNF_log_free_blk_index2pba(UINT32 index);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// __MWNF_LOG_H

