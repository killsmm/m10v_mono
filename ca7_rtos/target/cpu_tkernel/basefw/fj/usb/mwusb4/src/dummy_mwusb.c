/**
 * @file		dummy_mwusb.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include "fj_usb.h"

void EndUsbBusTimer(void)
{
	return;
}
void StartUsbBusTimer(unsigned long time)
{
	return;
}

long MWUSBC_CustomSetUsbDescriptor(FJ_PRMID_USB_DESC_CUSTOM uCusDescID, unsigned char* uInBuf, unsigned short uBufSize)
{
	return 0;
}
#if 0  /* M-7M */
#ifndef __TEST_MODE_DF
void MWUSBC_ModeTest_Main(void){return;}
void MWUSB_USBtest_registStatusCb(void (*func)(unsigned char requestType, unsigned char status)){return;}
#endif
#endif  /* M-7M */
