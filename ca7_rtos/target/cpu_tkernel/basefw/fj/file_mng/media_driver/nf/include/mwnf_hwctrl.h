/**
 * @file		mwnf_hwctrl.h
 * @brief		Nand Flash driver hardware control header file
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef __MWNF_HWCTRL_H
#define __MWNF_HWCTRL_H

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

// change mode
extern INT32 mwnf_change_mode(UINT32 mode, UINT32 bank);

// command DMA (MetaMode)
extern INT32 MWNF_hwctrl_CMD_DMA(T_MWNF_CMD_DMA_CTRL *c, UINT32 *cor_bit);

// data DMA
extern INT32 MWNF_hwctrl_Data_DMA(T_MWNF_DATA_DMA_CTRL *c, UINT32 *cor_bit);

// copy back
extern INT32 MWNF_hwctrl_copy_back(T_MWNF_FLASH_POINTER *src,T_MWNF_FLASH_POINTER *dst,UINT32 PP);

// erase block
extern INT32 MWNF_hwctrl_eraseblk(UINT32 phy_addr);

// set soft reset
extern INT32 MWNF_hwctrl_softreset_poll(VOID);

// reset
extern INT32 MWNF_hwctrl_reset(UINT32 ce);

// set features
extern INT32 MWNF_hwctrl_setfeatures(UINT32 timing_mode);

// timing mode
extern INT32 MWNF_hwctrl_timing_mode(T_NF_ATTRIBUTE* attr);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// __MWNF_HWCTRL_H

