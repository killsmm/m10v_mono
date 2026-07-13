/**
 * @file		USBHDC_Dev_Initialize.h
 * @brief		This is a header file for USB Device initialize function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef H_USBHDC_DEV_INITIALIZE
#define H_USBHDC_DEV_INITIALIZE

#include "USBHDC_StdDef.h"
#include "USBHDC_Dev_Global.h"

/* macro definition declaration */
/* interrupt enable default setting flag */
#ifdef USE_ON_THE_GO
#define INTEN_DEFAULT_HDC_DEV	0x0000000E
#else
#define INTEN_DEFAULT_HDC_DEV	0x0000000A
#endif
#define DEVC_INTEN_DEFAULT		0xFB000000
#define EPCMD0_INTEN_DEFAULT	0x00000000
#define EPCMDx_INTEN_DEFAULT	0x00001000

/* register bit position & value */
#define EPCTRL0_INITVAL	0x00000500
#define EPCTRLx_INITVAL	0x00000400

/* function prototype declaration */
/* set USB Device's interrupt enable */
extern void Dev_SetInterruptEnable(void);

/* initialize endpoint */
extern void Dev_InitEndpoint(u32 ram_base, u32 ram_size);

/* setup endpoint0 */
extern void Dev_SetupEndpoint0(u8 *p_device_desc);

/* setup endpoint(1-15) */
extern void Dev_SetupEndpoint(u8 *p_ep_desc, u8 buffer_num, u8 config_flag);

/* makeup descriptor information */
//extern int Dev_MakeupDescInfo(CONFIG_DESC_INFO *p_config_info);
extern int Dev_MakeupDescInfo(CONFIG_DESC_INFO *p_config_info, u8 *config_descriptor);


/* set endpoint number of endpoint buffers. */
extern int Dev_SetEpBufferNum(int config_count, CONFIG_DESC_INFO **p_desc_info, unsigned char config_no, unsigned char if_no, unsigned char alt_no, unsigned char ep_no, unsigned char buffer_num);

/* makeup interface descriptor information */
int Dev_MakeupIntfDescInfo(IF_DESC_INFO *p_intf_info, u8 *p_intf_descriptor, u16 *p_total_length);

/* free memory of CONFIG_DESC_INFO members */
void Dev_FreeConfigDescInfo(CONFIG_DESC_INFO *p_config_info);

#endif
