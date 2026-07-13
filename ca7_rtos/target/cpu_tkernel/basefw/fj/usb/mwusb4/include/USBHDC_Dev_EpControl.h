/**
 * @file		USBHDC_Dev_EpControl.h
 * @brief		This is a header file for USB Device endpoint control function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef H_USBHDC_DEV_EPCONTROL
#define H_USBHDC_DEV_EPCONTROL

#include "USBHDC_StdDef.h"

/* macro definition declaration */
/* data transfer direction */
#define OUT	0
#define IN	1

/* register bit position & value */
/* EPCMDx */
#define EPCMDx_STOP				0x00000002
#define EPCMDx_INIT				0x00000004
#define EPCMDx_STALL_SET		0x00000020
#define EPCMDx_STALL_CLR		0x00000040
#define EPCMDx_NULLRESP			0x00000200
#define EPCMDx_NACKRESP			0x00000400
#define EPCMDx_WRITE_EN			0x00000800
#define EPCMD0_READYI_INTEN		0x00001000
#define EPCMDx_READY_INTEN		0x00001000
#define EPCMD0_READYO_INTEN		0x00002000
#define EPCMDx_EMPTY_INTEN		0x00002000
#define EPCMDx_PING_INTEN		0x00004000
#define EPCMDx_STALLED_INTEN	0x00008000
#define EPCMDx_NACK_INTEN		0x00010000
#define EPCMD0_READYI_INTCLR	0x00040000
#define EPCMDx_READY_INTCLR		0x00040000
#define EPCMD0_READYO_INTCLR	0x00080000
#define EPCMDx_EMPTY_INTCLR		0x00080000
#define EPCMDx_PING_INTCLR		0x00100000
#define EPCMDx_STALLED_INTCLR	0x00200000
#define EPCMDx_NACK_INTCLR		0x00400000
#define EPCMDx_EPCMD_BUSY		0x80000000

/* EPCTRLx */
#define EPCTRLx_STALL			0x00001000
#define EPCTRLx_TOGGLE			0x00002000


/* function prototype declaration */
/* set data for IN transaction */
extern void Dev_SetInTransactionData(unsigned char ep, unsigned char *p_in_trans_data, unsigned short packet_size);

/* get data of OUT transaction */
extern void Dev_GetOutTransactionData(unsigned char ep, unsigned char *p_out_trans_data, unsigned short packet_size);

/* set endpoint disable */
extern void	Dev_SetEpDisable(u8 ep, u8 wait_flag);

/* initialize endpoint */
extern void	Dev_InitEp(u8 ep);

/* set endpoint stall */
extern void	Dev_SetEpStall(u8 ep);

/* clear endpoint stall */
extern void	Dev_ClearEpStall(u8 ep);

/* set null response */
extern void Dev_SetNullResp(u8 ep, u8 val);

/* set nack response */
extern void Dev_SetNackResp(u8 ep, u8 val);

/* set endpoint interrupt enable */
extern void Dev_SetEpIntEn(u8 ep, u32 cmd_int_pos);

/* set endpoint interrupt disable */
extern void Dev_SetEpIntDis(u8 ep, u32 cmd_int_pos);

/* clear endpoint interrupt state */
extern void Dev_SetEpIntStateClr(u8 ep, u32 cmd_intclr_pos);

/* set endpoint transfer type */
extern void Dev_SetTransferType(u8 ep, u8 transfer_type);

/* set endpoint direction */
extern void Dev_SetDirection(u8 ep, u8 direction);

/* set number of endpoint bufferes */
extern void Dev_SetBufferNum(u8 ep, u8 buffer_num);

/* set endpoint hiband */
extern void Dev_SetHiband(u8 ep, u8 hiband);

/* clear endpoint control */
extern void Dev_ClearEpControl(u8 ep);


/* read EPCTRLx register */
extern u32 Dev_ReadEpctrlxReg(u32 reg_epctrlx_addr, u32 check_bit);

/* inquire endpoint enable */
extern u8	Dev_IsEpEnable(u8 ep);

/* get endpoint transfer type */
extern u8	Dev_GetTransferType(u8 ep);

/* get endpoint transfer direction */
extern u8	Dev_GetTransferDirection(u8 ep);

/* get number of endpoint buffers */
extern u8	Dev_GetEpBufferNumber(u8 ep);

/* get current endpoint buffer number */
extern u8	Dev_GetCurrentEpBuffer(u8 ep);

/* inquire endpoint buffer empty */
extern u8	Dev_IsEpEmpty(u8 ep, u8 ep0_dir);

/* inquire endpoint buffer full */
extern u8	Dev_IsEpFull(u8 ep, u8 ep0_dir);

/* inquire endpoint stall */
extern u8	Dev_IsEpStall(u8 ep);

#if 0
/* get toggle bit */
extern u8	Dev_GetToggleBit(u8 ep);
#endif

/* get endpoint buffer size */
extern u16	Dev_GetEpBufferSize(u8 ep);

/* get current endpoint buffer offset */
extern u16	Dev_GetEpBufferOffset(u8 ep, u8 ep0_dir);

/* set transfer data size */
extern void Dev_SetEpWriteSize(u8 ep, u16 size);

/* get transfer data size */
extern u16	Dev_GetEpReadSize(u8 ep);

#endif
