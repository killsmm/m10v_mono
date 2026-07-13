/**
 * @file		USBHDC_Dev_Control.h
 * @brief		This is a header file for USB Device control function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef H_USBHDC_DEV_CONTROL
#define H_USBHDC_DEV_CONTROL

#include "USBHDC_StdDef.h"


/* macro definition declaration */

/* Charging port detection wait time */
/* Tdcd_timeout : 300ms */
#define T_DCD_TIMEOUT			300
/* wait time : 10ms */
#define T_DCD_CHK_WAIT			10

/* register bit position & value */
/* DEVC */
#define DEVC_BUSSPEED			0x00000001
#define DEVC_REQRESUME			0x00000004
#define DEVC_ENRMTWKUP			0x00000008
#define DEVC_SELFPOWER			0x00000010
#define DEVC_DISCONNECT			0x00000020
#define DEVC_PHYSUSP			0x00010000

/* DEVS */
#define DEVS_CRTSPEED			0x00020000

/* FADDR */
#define FADDR_DEV_CONFIGURED	0x00000100
#define FADDR_FUNCADDR			0x0000007F
/*TSTAMP */
#define TIMESTAMP				0x00000FFF

/* OTGC */
#define OTGC_dp_pull_down		0x00000100
#define OTGC_dm_pull_down		0x00000080

///* BCCTRL */
//#define BCCTRL_BCEN				0x00000001
//#define BCCTRL_BCCTRLMODE		0x00000004
//#define BCCTRL_ISRCDPEN			0x00000008
//#define BCCTRL_ISNKDMEN			0x00000100
//#define BCCTRL_VCMPDMEN			0x00000200
//#define BCCTRL_VSRCDPSWT		0x00000400
//#define BCCTRL_VSRCDPEN			0x00000800
//#define BCCTRL_ISNKDPEN			0x00001000
//#define BCCTRL_VCMPDPEN			0x00002000
//#define BCCTRL_VSRCDMSWT		0x00004000
//#define BCCTRL_VSRCDMEN			0x00008000
//
///* BCSTS */
//#define BCSTS_LINESTE			0x20000000
//#define BCSTS_BCDMDET			0x40000000
//#define BCSTS_BCDPDET			0x80000000
//
///* BCSTSC */
//#define BCSTSC_LINESTE_l_int	0x00200000
//#define BCSTSC_BCDMDET_l_int	0x00400000
//#define BCSTSC_BCDPDET_l_int	0x00800000
//#define BCSTSC_LINESTE_h_int	0x20000000
//#define BCSTSC_BCDMDET_h_int	0x40000000
//#define BCSTSC_BCDPDET_h_int	0x80000000

// USB2_CKCTL value
#define CLKCTL_HCKEN_STOP      0
#define CLKCTL_HCKEN_RUN       1

// USB2_RCTL value
#define SRST_RESET_ON          0
#define SRST_RESET_OFF         1

// USB2_ANPD value
#define ANPD_PD_OFF            0
#define ANPD_PD_ON             1

// USB2_DMAFSM1/2 value
#define DMAFSM_DMA_TRANS_STATE_OFF     0
#define DMAFSM_DMA_TRANS_STATE_ON      1
#define DMAFSM_FSMRST_RESET_ON         0
#define DMAFSM_FSMRST_RESET_OFF        1

// USB2_HDMAC1/2 value
#define HDMAC_BT_INCR          0
#define HDMAC_BT_INCR16        3

#define HDMAC_BC_0             0

/* function prototype declaration */
/* set default bus speed */
extern void	Dev_SetBusSpeed(u8 val);

#if 0
/* set resume request */
extern void Dev_SetReqResume(void);
#endif

/* set remote wakeup function */
extern void Dev_SetRemoteWakeup(u8 val);

/* get remote wakeup function */
extern u8	Dev_GetRemoteWakeup(void);

/* set disconnetion to host */
extern void Dev_SetDisconnect(u8 val);

/* get disconnetion to host */
extern u8	Dev_GetDisconnect(void);

/* set PHY suspend mode */
extern void Dev_SetPhySuspend(u8 val);

#if 0
/* get PHY suspend mod */
extern u8	Dev_GetPhySuspend(void);
#endif

/* Inquire state of device suspend */
extern s8	Dev_IsSuspend(void);

#if 0
/* Inquire state of bus reset */
extern s8	Dev_IsBusReset(void);
#endif

/* Inquire state of PHY reset */
extern u8	Dev_IsPhyReset(void);

/* get current bus speed */
extern u8	Dev_GetBusSpeed(void);

/* clear device interrupt state */
extern void Dev_ClearDevIntState(u32 int_pos);

/* set configuration end */
extern void Dev_SetConfigured(u8 val);

/* get state of configuration */
extern u8	Dev_GetConfigured(void);

/* set function address */
extern void Dev_SetFunctionAddress(u8 address);

#if 0
/* get function address */
extern u8	Dev_GetFunctionAddress(void);
 
/* get time stamp */
extern u16	Dev_GetTimeStamp(void);
#endif

///* data contact detect */
//s32 Dev_DataContactDetect(void);
//
///* primary detection */
//s32 Dev_PrimaryDetection(void);
//
///* secondary detection */
//s32 Dev_SecondaryDetection(void);

#endif
