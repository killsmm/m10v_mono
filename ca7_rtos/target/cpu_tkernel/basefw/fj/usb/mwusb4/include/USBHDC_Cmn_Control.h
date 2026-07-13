/**
 * @file		USBHDC_Cmn_Control.h
 * @brief		This is a header file for USB Host & Device control function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef H_USBHDC_CMN_CONTROL
#define H_USBHDC_CMN_CONTROL

//#define USBHDC_DEBUG_CAPTURE

#include <stdlib.h>
#include <stdio.h>
#include "USBHDC_StdDef.h"

//#define STD_printf printf

/* loop max count */
#define SOFT_RESET_LOOP_MAX		40

/* macro definition declaration */
/* register bit position & value */
/* CONF register */
#define CONF_BYTE_ORDER		0x1
#define CONF_BURST_WAIT		0x2
#define CONF_SOFT_RESET		0x4

#define UTMIVC_VLOAD_N		0x1

#define EPCMDx_START			0x00000001
#define EPCMDx_STOP				0x00000002
#define EPCMDx_BUFWR			0x00000008
#define EPCMDx_BUFRD			0x00000010
#define EPCMDx_TOGGLE_SET		0x00000080
#define EPCMDx_TOGGLE_CLR		0x00000100

/* INTEN */
#define INTEN_dma1_inten		0x00000100
#define INTEN_dma2_inten		0x00000200

/* INTS */
#define INTS_dma1_int			0x00000100
#define INTS_dma2_int			0x00000200

/* function prototype declaration */
/* set byte order */
extern void Cmn_SetByteOrder(u8 val);

/* set burst wait */
extern void Cmn_SetBurstWait(u8 val);

/* set soft reset function */
extern s32 Cmn_SoftReset(u8 byte_order);

/* set active mode of USB macro */
extern void Cmn_SetActiveMode(u8 mode);

/* get active mode of USB macro */
extern u8	Cmn_GetActiveMode(void);

/* clear global interrupt state. */
extern void Cmn_ClearIntState(u32 int_pos);

#if 0
/* set vload_n field value of UTMIVC register. */
extern void Cmn_SetUtmiVload(u8 vload);

/* get vload_n field value of UTMIVC register. */
extern u8 Cmn_GetUtmiVload(void);

/* set vcntl field value of UTMIVC register. */
extern void Cmn_SetUtmiVcntl(u8 vcntl);

/* get vcntl field value of UTMIVC register. */
extern u8 Cmn_GetUtmiVcntl(void);

/* get vstatus field value of UTMIVC register. */
extern u8 Cmn_GetUtmiVstatus(void);

/* set gpcnt field value of GPCNT register. */
extern void Cmn_SetGpcnt(u8 gpcnt);

/* get gpcnt field value of GPCNT register. */
extern u8 Cmn_GetGpcnt(void);
#endif

/* set endpoint enable */
extern void	Cmn_SetEpEnable(u8 ep);

/* set writing completetion */
extern void	Cmn_SetWriteComplete(u8 ep);

/* set reading completetion */
extern void	Cmn_SetReadComplete(u8 ep);

/* set toggle bit */
extern void	Cmn_SetToggleBit(u8 ep);

/* clear toggle bit */
extern void	Cmn_ClearToggleBit(u8 ep);

/* inquire endpoint command busy. */
extern u8 Cmn_IsEpCommandBusy(u32 reg_epcmdx_offset);

/* wait endpoint command busy. */
extern void Cmn_WaitEpCommand(u32 reg_epcmdx_addr);

/* read data from endpoint buffer */
extern void	Cmn_ReadEpBuffer(u16 offset, u8 *data, u16 size);

/* write data to endpoint buffer */
extern void Cmn_WriteEpBuffer(u16 offset, u8 *data, u16 size);

#endif

