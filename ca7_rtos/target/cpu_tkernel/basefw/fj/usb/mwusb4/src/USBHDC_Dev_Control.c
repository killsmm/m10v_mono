/**
 * @file		USBHDC_Dev_Control.c
 * @brief		USB Device control function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include "USBHDC_Dev_Control.h"
#include "USBHDC_Dev_Api.h"
#include "USBHDC_MemMap.h"
#include "USBHDC_GeneralFunc.h"
#include "USBHDC_AccessIF.h"
//#include "cmd_usb_test.h"			/* usb begin task */
#include "usb_user_wrapper.h"

extern u8 byteOrder;  /* 0=little endian, 1=big endian */

/*****************************************************************
 * [ Function ]		set default bus speed.                       *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 bus_speed : 0 = High speed                *
 *                                 1 = Full speed                *
 *                                 2 = Low speed (no support)    *
 *****************************************************************/
void Dev_SetBusSpeed(u8 bus_speed)
{
	u32 reg_devc_val;

	/* read register */
	reg_devc_val = ReadRegister(DEVC);
	/* set bus speed to register */
	reg_devc_val = (reg_devc_val & ~(DEVC_BUSSPEED)) | (bus_speed & DEVC_BUSSPEED);
	/* write register */
	WriteRegister(DEVC, reg_devc_val);
}


#if 0
/*****************************************************************
 * [ Function ]		set resume request.                          *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Dev_SetReqResume(void)
{
	//u32 set_val;
	/* set resume request */
	SetBitRegister(DEVC, DEVC_REQRESUME);
}
#endif


/*****************************************************************
 * [ Function ]		set remote wakeup function.                  *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 remote_wakeup : 0 = remote wakeup disable *
 *                                     1 = remote wakeup enable  *
 *****************************************************************/
void Dev_SetRemoteWakeup(u8 remote_wakeup)
{
	/* disable remote wakeup enable */
	if (remote_wakeup == 0) {		
		ClearBitRegister(DEVC, DEVC_ENRMTWKUP);
	/* enable remote wakeup enable */
	} else {
		SetBitRegister(DEVC, DEVC_ENRMTWKUP);
	}
}


/*****************************************************************
 * [ Function ]		get remote wakeup function.                  *
 * [ Return code ]	u8 : (0) remote wakeup disable               *
 *                       (1) remote wakeup enable                *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
u8 Dev_GetRemoteWakeup(void)
{
	u32 reg_devc_val;

	/* read DEVC register */
	reg_devc_val = ReadRegister(DEVC);

	/* return remote wakeup(bit3) */
	return (u8)((reg_devc_val >> 3) & 0x1);
}


/*****************************************************************
 * [ Function ]		set disconnetion to host.                    *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 disconnect : 0 = connection to host       *
 *                                  1 = disconnection to host    *
 *****************************************************************/
void Dev_SetDisconnect(u8 disconnect)
{
	/* notify connection to host */
	if( disconnect == 0 )
	{
		ClearBitRegister(DEVC, DEVC_DISCONNECT);
	/* notify disconnection to host */
	}
	else
	{
		SetBitRegister(DEVC, DEVC_DISCONNECT);
	}
}


/*****************************************************************
 * [ Function ]		get disconnetion to host.                    *
 * [ Return code ]	u8 : (0) connection to host                  *
 *                       (1) disconnection to host               *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
u8 Dev_GetDisconnect(void)
{
	u32 reg_devc_val;

	/* read DEVC register */
	reg_devc_val = ReadRegister(DEVC);

	/* return disconnect(bit5) */
	return (u8)((reg_devc_val >> 5) & 0x1);
}


/*****************************************************************
 * [ Function ]		set PHY suspend mode.                        *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 phy_suspend : 0 = normal suspend mode     *
 *                                   1 = compulsion suspend mode *
 *****************************************************************/
void Dev_SetPhySuspend(u8 phy_suspend)
{
	/* set normal PHY suspend mode */
	if (phy_suspend == 0) {
		ClearBitRegister(DEVC, DEVC_DISCONNECT);
	/* set compulsion PHY suspend mode */
	} else {
		SetBitRegister(DEVC, DEVC_DISCONNECT);
	}
}


#if 0
/*****************************************************************
 * [ Function ]		get PHY suspend mode.                        *
 * [ Return code ]	u8 : (0) normal suspend mode                 *
 *                       (1) compulsion suspend mode             *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
u8 Dev_GetPhySuspend(void)
{
	u32 reg_devc_val;

	/* read DEVC register */
	reg_devc_val = ReadRegister(DEVC);
	
	/* return PHY suspend mode(bit16) */
	return (u8)((reg_devc_val >> 16) & 0x1);
}
#endif


/*****************************************************************
 * [ Function ]		Inquire state of device suspend.             *
 * [ Return code ]	u8 : (0) suspend release state               *
 *                       (1) suspend state                       *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
s8 Dev_IsSuspend(void)
{
	u32 reg_devs_val;

	/* read DEVS register */
	reg_devs_val = ReadRegister(DEVS);

	/* return suspend state(bit0) */
	return (u8)(reg_devs_val & 0x1);
}


#if 0
/*****************************************************************
 * [ Function ]		Inquire state of bus reset.                  *
 * [ Return code ]	u8 : (0) bus reset is not being executed.    *
 *                       (1) bus reset is being executed.        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
s8 Dev_IsBusReset(void)
{
	u32 reg_devs_val;

	/* read DEVS register */
	reg_devs_val = ReadRegister(DEVS);

	/* return bus reset state(bit1) */
	return (u8)((reg_devs_val >> 1) & 0x1);
}
#endif


/*****************************************************************
 * [ Function ]		Inquire state of PHY reset.                  *
 * [ Return code ]	u8 : (0) PHY reset is not being executed.    *
 *                       (1) PHY reset is being executed.        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
u8 Dev_IsPhyReset(void)
{
	u32 reg_devs_val;

	/* read DEVS register */
	reg_devs_val = ReadRegister(DEVS);

	/* return PHY reset state(bit16) */
	return (u8)((reg_devs_val >> 16) & 0x1);
}


/*****************************************************************
 * [ Function ]		get current bus speed.                       *
 * [ Return code ]	u8 : (0) High speed                          *
 *                       (1) Full speed                          *
 *                       (2) Low speed (no support)              *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
u8 Dev_GetBusSpeed(void)
{
	u32 reg_devs_val;
	u8  ret;

	/* read DEVS register */
	reg_devs_val = ReadRegister(DEVS);

	if((reg_devs_val & DEVS_CRTSPEED) == 0) {
		ret = HDC_HIGH_SPEED;
	} else {
		ret = HDC_FULL_SPEED;
	}

	return ret;
}


/*****************************************************************
 * [ Function ]		clear device interrupt state.                *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u32 int_pos : bit position of interrupt state*
 *****************************************************************/
void Dev_ClearDevIntState(u32 int_pos)
{
	/* write DEVS register */
	WriteRegister(DEVS, ~int_pos & 0xFF000000);
}


/*****************************************************************
 * [ Function ]		set configuration end.                       *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 configured : 0 = clear configuration      *
 *                                  1 = set configuration end    *
 *****************************************************************/
void Dev_SetConfigured(u8 configured)
{
	/* clear configured */
	if (configured == 0) {
		ClearBitRegister(FADDR, FADDR_DEV_CONFIGURED);
	/* set configured */
	} else {
		SetBitRegister(FADDR, FADDR_DEV_CONFIGURED);
	}
}


/*****************************************************************
 * [ Function ]		get state of configuration.                  *
 * [ Return code ]	u8 : (0) not configured                      *
 *                       (1) configured                          *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
u8 Dev_GetConfigured(void)
{
	u32 reg_faddr_val;

	/* read FADDR register */
	reg_faddr_val = ReadRegister(FADDR);

	/* return configured state(bit8) */
	return (u8)(reg_faddr_val >> 8) & 0x1;
}


/*****************************************************************
 * [ Function ]		set function address.                        *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u8 func_address : function address from Host *
 *****************************************************************/
void Dev_SetFunctionAddress(u8 func_address)
{
	u32 reg_faddr_val;

	/* read FADDR register */
	reg_faddr_val = ReadRegister(FADDR);
	/* set function address to register */
	reg_faddr_val = (reg_faddr_val & ~(FADDR_FUNCADDR)) | (func_address & FADDR_FUNCADDR);
	/* write FADDR register */
	WriteRegister(FADDR, reg_faddr_val);
}


#if 0
/*****************************************************************
 * [ Function ]		get function address.                        *
 * [ Return code ]	u8 func_address : function address from Host.*
 * [ Parameter ]	none.                                        *
 *****************************************************************/
u8 Dev_GetFunctionAddress(void)
{
	u32 reg_faddr_val;

	/* read FADDR register */
	reg_faddr_val = ReadRegister(FADDR);

	/* return function address */
	return (u8)(reg_faddr_val & FADDR_FUNCADDR);
}


/*****************************************************************
 * [ Function ]		get time stamp.                              *
 * [ Return code ]	u16 : time stamp.                            *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
u16 Dev_GetTimeStamp(void)
{
	u32 reg_tstamp_val;

	/* read TSTAMP register */
	reg_tstamp_val = ReadRegister(TSTAMP);

	/* return time stamp */
	return (u16)(reg_tstamp_val & TIMESTAMP);
}
#endif


/*****************************************************************
 * [ Function ]		Data Contact Detect.                         *
 * [ Return code ]	s32 : HDC_SUCCESS                            *
 *                        HDC_FAILURE                            *
 *                        HDC_TIMEOUT                            *
 * [ Parameter ]	none.                                        *
******************************************************************/
// Battery charge function is not used.
//s32 Dev_DataContactDetect(void)
//{
//	s32 ret = HDC_SUCCESS;
//	s32 i;
//	
//	/* set BCCTRLMODE bit of BCCTRL register */
//	SetBitRegister(BCCTRL, BCCTRL_BCCTRLMODE);
//	
//	/* set BCEN bit of BCCTRL regiseter */
//	SetBitRegister(BCCTRL, BCCTRL_BCEN);
//	
//	/* wait for 30us (T_PRDMEN) */
//	WaitTime(30);
//	
//	/* set dm_pull_down bit of OTGC register */
//	SetBitRegister(OTGC, OTGC_dm_pull_down);
//	
//	/* wait for 30us (T_ISRCEN) */
//	WaitTime(30);
//	
//	/* set ISRCDPEN bit of BCCTRL register */
//	SetBitRegister(BCCTRL, BCCTRL_ISRCDPEN);
//	
//	/* wait for 30us (T_DELAY) */
//	WaitTime(30);
//	
//	/* clear LINESTE_l_int bit of BCSTSC register */
//	ClearBitRegister(BCSTSC, BCSTSC_LINESTE_l_int);
//	
//	/* Check VBUS condition */
//	if (D_USB_USER_PLUG_RELEASE == USB_User_Wrapper_Check_Connect(D_USB_USER_WRAP_OFF)) {
//		/* ERROR */
//		ret = HDC_FAILURE;
//	} else {
//		if ((ReadRegister(BCSTSC) & BCSTSC_LINESTE_l_int) == 0) {
//			ret = HDC_TIMEOUT;
//			for (i = 0; i < (T_DCD_TIMEOUT / T_DCD_CHK_WAIT); i++) {
//				/* Wait for 10ms */
//				WaitTime((T_DCD_CHK_WAIT) * 1000);
//				
//				/* Check VBUS condition */
//				if (D_USB_USER_PLUG_RELEASE == USB_User_Wrapper_Check_Connect(D_USB_USER_WRAP_OFF)) {
//					/* ERROR */
//					ret = HDC_FAILURE;
//					break;
//				}
//				
//				if ((ReadRegister(BCSTSC) & BCSTSC_LINESTE_l_int) != 0) {
//					ret = HDC_SUCCESS;
//					break;
//				}
//			}
//		}
//		
//		if (ret == HDC_SUCCESS)  {
//			/* clear LINESTE_h_int bit of BCSTSC register */
//			ClearBitRegister(BCSTSC, BCSTSC_LINESTE_h_int);
//			
//			/* wait for 10ms (T_DCD_DBNC) */
//			WaitTime(10000);
//			
//			if (((ReadRegister(BCSTSC) & BCSTSC_LINESTE_h_int) != 0) ||
//			    ((ReadRegister(BCSTS) & BCSTS_LINESTE) != 0)) {
//				ret = HDC_FAILURE;
//			} else {
//				ret = HDC_SUCCESS;
//			}
//		}
//	}
//	
//	/* clear ISRCDPEN bit of BCCTRL register */
//	ClearBitRegister(BCCTRL, BCCTRL_ISRCDPEN);
//	
//	/* clear dm_pull_down bit of OTGC register */
//	ClearBitRegister(OTGC, OTGC_dm_pull_down);
//	
//	if (ret == HDC_FAILURE) {
//		/* clear BCEN bit of BCCTRL register */
//		ClearBitRegister(BCCTRL, BCCTRL_BCEN);
//		
//		/* clear BCCRTLMODE bit of BCCTRL register */
//		ClearBitRegister(BCCTRL, BCCTRL_BCCTRLMODE);
//	}
//	
//	return ret;
//}

/*****************************************************************
 * [ Function ]		Primary Detection.                           *
 * [ Return code ]	s32 : HDC_PORT_SDP                           *
 *                        HDC_PORT_CDP                           *
 *                        HDC_PORT_FAILURE                       *
 * [ Parameter ]	none.                                        *
******************************************************************/
// Battery charge function is not used.
//s32 Dev_PrimaryDetection(void)
//{
//	s32 ret = 0;
//	u32 bcsts_reg;
//	u32 bcstsc_reg;
//	
//	/* check BCEN bit of BCCTRL register */
//	if ((ReadRegister(BCCTRL) & BCCTRL_BCEN) == 0) {
//		return HDC_FAILURE;
//	}
//	
//	/* Set VCMPDMEN bit and ISNKDMEN bit of BCCTRL register */
//	SetBitRegister(BCCTRL, (BCCTRL_VCMPDMEN | BCCTRL_ISNKDMEN));
//	
//	/* Wait for 30us (T_VSRCEN) */
//	WaitTime(30);
//	
//	/* Set VSRCDPEN bit of BCCTRL register */
//	SetBitRegister(BCCTRL, BCCTRL_VSRCDPEN);
//	
//	/* Wait for 30us (T_VSRCSWT) */
//	WaitTime(30);
//	
//	/* Set VSRCDPSWT bit of BCCTRL register */
//	SetBitRegister(BCCTRL, BCCTRL_VSRCDPSWT);
//	
//	/* Wait for 30us (T_CMPSTBL) */
//	WaitTime(30);
//	
//	/* Clear BCDMDET_h_int bit of BCSTSC register */
//	ClearBitRegister(BCSTSC, BCSTSC_BCDMDET_h_int);
//	
//	/* Check VBUS condition */
//	if (D_USB_USER_PLUG_RELEASE == USB_User_Wrapper_Check_Connect(D_USB_USER_WRAP_OFF)) {
//		/* ERROR */
//		ret = HDC_FAILURE;
//	} else {
//		/* Wait for 20ms (T_VDMSRC_EN) */
//		WaitTime(20000);
//		
//		/* Check VBUS condition */
//		if (D_USB_USER_PLUG_RELEASE == USB_User_Wrapper_Check_Connect(D_USB_USER_WRAP_OFF)) {
//			/* ERROR */
//			ret = HDC_FAILURE;
//		} else {
//			bcsts_reg = ReadRegister(BCSTS);
//			bcstsc_reg = ReadRegister(BCSTSC);
//			
//			if ((bcsts_reg & BCSTS_BCDMDET) != 0) {
//				if((bcstsc_reg & BCSTSC_BCDMDET_h_int) != 0) {
//					/* Clear BCDMDET_l_int bit of BCSTSC register */
//					ClearBitRegister(BCSTSC, BCSTSC_BCDMDET_l_int);
//					
//					/* Wait for 20ms (T_VDPSRC_EN) */
//					WaitTime(20000);
//				}
//			} else {
//				if ((bcstsc_reg & BCSTSC_BCDMDET_h_int) != 0) {
//					/* ERROR */
//					ret = HDC_FAILURE;
//				} else {
//					/* Wait for 20ms (T_VDPSRC_EN) */
//					WaitTime(20000);
//				}
//			}
//		}
//	}
//	
//	if (ret == 0) {
//		if (D_USB_USER_PLUG_RELEASE == USB_User_Wrapper_Check_Connect(D_USB_USER_WRAP_OFF)) {
//			/* ERROR */
//			ret = HDC_FAILURE;
//		} else {
//			bcsts_reg = ReadRegister(BCSTS);
//			bcstsc_reg = ReadRegister(BCSTSC);
//			
//			if ((bcsts_reg & BCSTS_BCDMDET) != 0) {
//				if ((bcstsc_reg & BCSTSC_BCDMDET_l_int) == 0) {
//					/* Charging port is CDP or DCP */
//					ret = HDC_PORT_CDP;
//				} else {
//					/* ERROR */
//					ret = HDC_FAILURE;
//				}
//			} else {
//				if ((bcstsc_reg & BCSTSC_BCDMDET_h_int) == 0) {
//					/* Charging port is SDP */
//					ret = HDC_PORT_SDP;
//				} else {
//					/* ERROR */
//					ret = HDC_FAILURE;
//				}
//			}
//		}
//	}
//	
//	/* Clear VSRCDPSWT bit of BCCTRL register */
//	ClearBitRegister(BCCTRL, BCCTRL_VSRCDPSWT);
//	
//	/* Wait for 30us (T_VSRCDIS) */
//	WaitTime(30);
//	
//	/* Clear VSRCDPEN bit, VCMPDMEN bit, and ISNKDMEN bit of BCCTRL register */
//	ClearBitRegister(BCCTRL, (BCCTRL_VSRCDPEN | BCCTRL_VCMPDMEN | BCCTRL_ISNKDMEN));
//	
//	if (ret == HDC_PORT_CDP) {
//		/* Wait for 20ms (TVDMSRC_DIS) */
//		WaitTime(20000);
//	} else {
//		/* Clear BCEN bit of BCCTRL register */
//		ClearBitRegister(BCCTRL, BCCTRL_BCEN);
//		
//		/* Clear BCCTRLMODE bit of BCCTRL register */
//		ClearBitRegister(BCCTRL, BCCTRL_BCCTRLMODE);
//	}
//	
//	return ret;
//}


/*****************************************************************
 * [ Function ]		Secondary Detection.                         *
 * [ Return code ]	s32 : HDC_PORT_CDP                           *
 *                        HDC_PORT_DCP                           *
 *                        HDC_PORT_FAILURE                       *
 * [ Parameter ]	none.                                        *
******************************************************************/
// Battery charge function is not used.
//s32 Dev_SecondaryDetection(void)
//{
//	s32 ret = 0;
//	u32 bcsts_reg;
//	u32 bcstsc_reg;
//	
//	if ((ReadRegister(BCCTRL) & BCCTRL_BCEN) == 0) {
//		/* ERROR */
//		return HDC_FAILURE;
//	}
//	
//	/* Set VCMPDPEN bit and ISNKDPEN bit of BCCTRL register */
//	SetBitRegister(BCCTRL, (BCCTRL_VCMPDPEN | BCCTRL_ISNKDPEN));
//	
//	/* Wait for 30us (T_VSRCEN) */
//	WaitTime(30);
//	
//	/* Set VSRCDMEN bit of BCCTRL register */
//	SetBitRegister(BCCTRL, BCCTRL_VSRCDMEN);
//	
//	/* Wait for 30us (T_VSRCSWT) */
//	WaitTime(30);
//	
//	/* Set VSRCDMSWT bit of BCCTRL register */
//	SetBitRegister(BCCTRL, BCCTRL_VSRCDMSWT);
//	
//	/* Wait for 30us (T_CMPSTBL) */
//	WaitTime(30);
//	
//	/* Clear BCDPDET_h_int bit and BCDPDET_l_int bit of BCSTSC register */
//	ClearBitRegister(BCSTSC, (BCSTSC_BCDPDET_h_int | BCSTSC_BCDPDET_l_int));
//	
//	/* Check VBUS condition */
//	if (D_USB_USER_PLUG_RELEASE == USB_User_Wrapper_Check_Connect(D_USB_USER_WRAP_OFF)) {
//		/* ERROR */
//		ret = HDC_FAILURE;
//	} else {
//		/* Wait for 40ms (T_VDMSRC_EN) */
//		WaitTime(40000);
//		
//		/* Check VBUS condition */
//		if (D_USB_USER_PLUG_RELEASE == USB_User_Wrapper_Check_Connect(D_USB_USER_WRAP_OFF)) {
//			/* ERROR */
//			ret = HDC_FAILURE;
//		} else {
//			bcsts_reg = ReadRegister(BCSTS);
//			bcstsc_reg = ReadRegister(BCSTSC);
//			
//			if ((bcsts_reg & BCSTS_BCDPDET) != 0) {
//				if (((bcstsc_reg & BCSTSC_BCDPDET_h_int) != 0) &&
//				    ((bcstsc_reg & BCSTSC_BCDPDET_l_int) == 0)) {
//					/* Charging port is DCP */
//					ret = HDC_PORT_DCP;
//				} else {
//					/* ERROR */
//					ret = HDC_FAILURE;
//				}
//			} else {
//				if (((bcstsc_reg & BCSTSC_BCDPDET_h_int) == 0) &&
//				    ((bcstsc_reg & BCSTSC_BCDPDET_l_int) != 0)) {
//					/* Charging port is CDP */
//					ret = HDC_PORT_CDP;
//				} else {
//					/* ERROR */
//					ret = HDC_FAILURE;
//				}
//			}
//		}
//	}
//	
//	/* Clear VSRCDMSWT bit of BCCTRL register */
//	ClearBitRegister(BCCTRL, BCCTRL_VSRCDMSWT);
//	
//	/* Wait for 30us (T_VSRCDIS) */
//	WaitTime(30);
//	
//	/* Clear VSRCDMEN bit, VCMPDPEN bit, ISNKDPEN bit of BCCTRL register */
//	ClearBitRegister(BCCTRL, (BCCTRL_VSRCDMEN | BCCTRL_VCMPDPEN | BCCTRL_ISNKDPEN));
//	
//	/* Clear BCEN bit of BCCTRL register */
//	ClearBitRegister(BCCTRL, BCCTRL_BCEN);
//	
//	/* Clear BCCTRLMODE bit of BCCTRL register */
//	ClearBitRegister(BCCTRL, BCCTRL_BCCTRLMODE);
//	
//	return ret;
//	
//}


