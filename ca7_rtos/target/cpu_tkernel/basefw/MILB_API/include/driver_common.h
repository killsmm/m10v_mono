/**
 * @file		driver_common.h
 * @brief		Device Driver Common Definition
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup common
@{
*/

#ifndef _DRIVER_COMMON_H_
#define _DRIVER_COMMON_H_

#include "ddim_typedef.h"
#include "ddim_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Compile option */
#define		CO_PARAM_CHECK								/**< Compile Option of Parametre Check. */
#define		CO_MSG_PRINT_ON								/**< Compile Option of Message Print Output. */
//#define		CO_ACT_CLOCK								/**< Compile Option of Active Clock for CLK. */
//#define		CO_ACT_ICLOCK								/**< Compile Option of Active Clock for AXICLK. */
//#define		CO_ACT_PCLOCK								/**< Compile Option of Active Clock for APBCLK. */
//#define		CO_ACT_HCLOCK								/**< Compile Option of Active Clock for AHBCLK. */
//#define		CO_ACT_IMAGE_CLOCK							/**< Compile Option of Active Clock for Image Macro CLK. */
//#define		CO_ACT_IMAGE_ICLOCK							/**< Compile Option of Active Clock for Image Macro AXICLK. */
//#define		CO_ACT_IMAGE_PCLOCK							/**< Compile Option of Active Clock for Image Macro APBCLK. */
#define		CO_ACT_IMAGE_HCLOCK							/**< Compile Option of Active Clock for Image Macro AHBCLK. */
//#define		CO_ASSERT_ON								/**< Compile Option of Assertion. */
//#define		CO_MEMORY_DEVICE							/**< Compile Option of Memory device. */
#ifndef		CO_DDIM_UTILITY_USE
#define		CO_DDIM_UTILITY_USE							/**< Compile Option of DDIM utility. */
#endif	// CO_DDIM_UTILITY_USE

#if defined(CO_IM_TUNING_ON)
#define     CO_DDIM_IM_TUNING_ON
#endif
/* Device type (Upper 16bit on error code) */
/*---------- ChipTop/ARM/Peripheral/Image Macro -----*/
/* --------- ChipTop ------------------------------- */
#define		D_DD_TOP					(0x00010000)	/**< ChipTop */

/* --------- ARM ----------------------------------- */
#define		D_DD_ARM					(0x00020000)	/**< ARM */
#define		D_DD_CACHE					(0x00030000)	/**< CACHE */
#define		D_DD_EXIU					(0x00040000)	/**< EXIU(External Interrupt Unit) */
#define		D_DD_GIC					(0x00050000)	/**< GIC(Generic Interrupt Controller) */
#define		D_DD_IPCU					(0x00060000)	/**< IPCU(Inter-Processor Communications Unit) */
#define		D_DD_NIC					(0x00070000)	/**< NIC(Network Inter Connect) */
#define		D_DD_TIMESTAMP				(0x00080000)	/**< TIMESTAMP */
#define		D_DD_WDOG					(0x00090000)	/**< Watchdog Timer */
#define		D_DD_XDMASNAP				(0x000A0000)	/**< XDMAC SNAP */

/*---------- Peripheral ---------------------------- */
#define		D_DD_TMR32					(0x00100000)	/**< Reload Timer-32bit */
#define		D_DD_TMR64					(0x00110000)	/**< Timer-64bit */
#define		D_DD_PWM					(0x00120000)	/**< PWM */
#define		D_DD_I2C					(0x00130000)	/**< I2C */
#define		D_DD_UDC					(0x00140000)	/**< UDC */
#define		D_DD_RELC					(0x00150000)	/**< RELC */
#define		D_DD_AUDIO					(0x00160000)	/**< Audio IF */
#define		D_DD_UART					(0x00170000)	/**< UART */
#define		D_DD_CSIO					(0x00180000)	/**< CSIO */
#define		D_DD_HDMI					(0x00190000)	/**< HDMI */
#define		D_DD_HDMAC0					(0x001A0000)	/**< HDMAC0 */
#define		D_DD_HDMAC1					(0x001B0000)	/**< HDMAC1 */
#define		D_DD_PDM					(0x001C0000)	/**< PDM */
#define		D_DD_SPI					(0x001D0000)	/**< PDM */
#define		D_DD_SLIMBUS				(0x001E0000)	/**< SLIMBus */

/*---------- External Storage ---------------------- */
#define		D_DD_GPV					(0x00200000)	/**< GPV(Global Programmers View) */
#define		D_DD_XDMAC					(0x00210000)	/**< XDMAC */
#define		D_DD_HDMAC					(0x00220000)	/**< HDMAC */
#define		D_DD_PCIE					(0x00230000)	/**< PCIE */

/*---------- Image Macro ----------------------------*/
#define		D_IM_PRO					(0x01000000)	/**< PRO(Pre Processor) */
#define		D_IM_JMILIMG				(0x02000000)	/**< JMILIMG */
#define		D_IM_B2R					(0x03000000)	/**< B2R */
#define		D_IM_HDMI					(0x04000000)	/**< HDMI */
#define		D_IM_MIPI					(0x05000000)	/**< MIPI */
#define		D_IM_MXIC					(0x06000000)	/**< MXIC */
#define		D_IM_XCH					(0x07000000)	/**< Xch */
#define		D_IM_DISP					(0x08000000)	/**< DISP */
#define		D_IM_CNR					(0x09000000)	/**< CNR */
#define		D_IM_ELA					(0x0A000000)	/**< ELA */
#define		D_IM_SDRAMC					(0x0B000000)	/**< SDRAMC */
#define		D_IM_IIP					(0x0C000000)	/**< IIP */
#define		D_IM_JPEG					(0x0D000000)	/**< JPEG */
#define		D_IM_SHDR					(0x0E000000)	/**< SHDR */
#define		D_IM_RAW					(0x0F000000)	/**< RAW */
#define		D_IM_R2Y					(0x10000000)	/**< R2Y */
#define		D_IM_BMH					(0x20000000)	/**< BMH */
#define		D_IM_RDMA					(0x30000000)	/**< RDMA */
#define		D_IM_LTM					(0x40000000)	/**< LTM */
#define		D_IM_ME						(0x50000000)	/**< ME */

/*---------- PMU ------------------------------------*/
#define		D_DD_PMU					(0x60000000)	/**< PMU */

/* Error definition (Lower 16bit on error code) */
#define		D_DDIM_OK					(0x00000000)	/**< Normal end */
#define		D_DDIM_INPUT_PARAM_ERROR	(0x00000001)	/**< Input parameter error */
#define		D_DDIM_EXT_LOCK_NG			(0x00000002)	/**< Lock error */
#define		D_DDIM_NO_RECEIVE			(0x00000003)	/**< No receive */
#define		D_DDIM_OVERFLOW_ERR			(0x00000004)	/**< Overflow */
#define		D_DDIM_MACRO_BUSY_NG		(0x00000005)	/**< Macro busy */
#define		D_DDIM_NO_SEND_DATA			(0x00000006)	/**< No sending data */
#define		D_DDIM_PARITY_DATA			(0x00000007)	/**< Parity data */
#define		D_DDIM_OVERRUN_DATA			(0x00000008)	/**< Over running */
#define		D_DDIM_FRAMING_DATA			(0x00000009)	/**< Framing error */
#define		D_DDIM_ACCESS_ERR			(0x0000000A)	/**< Access error */
#define		D_DDIM_PROCESSING			(0x0000000B)	/**< Job Processing  */
#define		D_DDIM_SEM_NG				(0x00000010)	/**< Semaphore acquisition NG */
#define		D_DDIM_SEM_TIMEOUT			(0x00000011)	/**< Semaphore acquisition Time Out */
#define		D_DDIM_STATUS_ABNORMAL		(0x00000012)	/**< Abnormal Status */
#define		D_DDIM_TIMEOUT				(0x00000013)	/**< Time Out */
#define		D_DDIM_OVERFLOW_OK			(0x00000017)	/**< Overflow OK */
#define		D_DDIM_DATA_EXIST_ERROR		(0x00000018)	/**< Data exist */
#define		D_DDIM_OVERWRITE_ERROR		(0x00000019)	/**< Overwrite error */
#define		D_DDIM_AXI_ERROR			(0x0000001A)	/**< AXI BUS error */
#define		D_DDIM_R_AXI_ERROR			(0x0000001B)	/**< AXI BUS error(Read ch) */
#define		D_DDIM_W_AXI_ERROR			(0x0000001C)	/**< AXI BUS error(Write ch) */
#define		D_DDIM_BUS_ERROR			(0x0000001D)	/**< BUS error */
#define		D_DDIM_ARBITRATION_LOST		(0x0000001E)	/**< Arbitration lost detect */
#define		D_DDIM_COMM_ERROR			(0x0000001F)	/**< Communication error */
#define		D_DDIM_FORCE_STOP			(0x00000020)	/**< Forced stop */
#define		D_DDIM_SYSTEM_ERROR			(0x00000099)	/**< System error */

/* Definition used with semaphore */
#define 	D_DDIM_SEM_WAIT_FEVR		(-1)			/**< Wait forever (TMO_FEVR) */
#define 	D_DDIM_SEM_WAIT_POL			(0)				/**< Wait by Polling (TMO_POL) */

/* Wait end time definition */
#define		D_DDIM_WAIT_END_TIME		(3000)			/**< Wait end time */
#define		D_DDIM_WAIT_END_FOREVER		(-1)			/**< Wait forever (TMO_FEVR)*/

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#ifndef Ddim_Print
#ifndef CO_MSG_PRINT_ON
#define Ddim_Print(arg)									/**< printf function inside DDIM */
#else
#include <stdio.h>
#define Ddim_Print(arg)		{DDIM_User_Printf arg;}		/**< printf function inside DDIM */
#endif	// CO_MSG_PRINT_ON
#endif	// Ddim_Print

#ifdef CO_ASSERT_ON
#define Ddim_Assertion(arg) {							\
    Ddim_Print(("!! Assert : %s (%d)", __FILE__, __LINE__));\
    Ddim_Print(arg);\
    DDIM_User_Assertion_Error();\
}														/**< assertion code */
#else
#define Ddim_Assertion(arg)		{DDIM_User_Printf arg;}		/**< printf function inside DDIM */
#endif	// CO_ASSERT_ON

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

#endif	// _DRIVER_COMMON_H_

/*@}*/
