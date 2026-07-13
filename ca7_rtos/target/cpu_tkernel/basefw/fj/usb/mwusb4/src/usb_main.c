/**
 * @file		usb_main.c
 * @brief		Main routine of USB Driver
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include "os_user_custom.h"
#include "mwusb_driver4.h"			/* driver4 source Header */
#include "msclib.h"
#include "dd_top.h"
#include "dd_gic.h"
#include "dd_exs.h"

/* wait time is  10s */
#define ENUM_WAIT_TIME      10000
#define ENUM_WAIT_TIME_DCP   2000
static int errTaskResult = MWUSB_OK;

extern void USB_Buffer_Init(void);

void usb_main(void)
{
	unsigned char	usb_State = MWUSB_PLUG_RELEASE;
	T_OS_USER_RTSK	flag;
	unsigned int	flg_ptn = FLG_MAIN_USB_Disconnect;
	OS_USER_FLGPTN	rtn_ptn;

	/* External port control (EPCRNM) */
//	Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PM1, 1);
	Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PM3, 0);
	Dd_Top_Set_Gpio_Function(E_DD_TOP_GPIO_PM4, 1);
	
	/* Data direction (DDR) */
//	Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PM0, 0);
//	Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PM1, 0);
	Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PM2, 0);
	Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PM3, 0);
	Dd_Top_Set_Gpio_Direction(E_DD_TOP_GPIO_PM4, 0);
	
	/* Pull up/down control (PUDCR) */
//	Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PM0, 0);
//	Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PM1, 0);
	Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PM3, 1);
	Dd_Top_Set_Gpio_Pull_Up_Down_Ctrl(E_DD_TOP_GPIO_PM4, 0);
	
	/* Pull up/down enable (PUDER) */
//	Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PM0, 0);
//	Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PM1, 0);
	Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PM3, 1);
	Dd_Top_Set_Gpio_Pull_Up_Down_Enable(E_DD_TOP_GPIO_PM4, 0);
	
	/* Clear ram sleep bit for USB macro in EXS module */
	Dd_Exs_Set_RAM_PD_Ctrl_USB2(0);
	
	/* Set enable interrupt for GIC */
	OS_User_EnableInt((OS_USER_INTID)E_DD_GIC_INTID_USB20_HDC_INT, (OS_USER_INTLV)D_DD_GIC_PRI14);
	OS_User_EnableInt((OS_USER_INTID)E_DD_GIC_INTID_EXS_HDMAC_CH0_INT, (OS_USER_INTLV)D_DD_GIC_PRI14);
	OS_User_EnableInt((OS_USER_INTID)E_DD_GIC_INTID_EXS_HDMAC_CH1_INT, (OS_USER_INTLV)D_DD_GIC_PRI14);

	/* Initialize Buffer for USB MSC */
	USB_Buffer_Init();
	
	/* initialize usb driver */
	MWUSB_Initialize(MWUSB_CONFIG_MANUAL);

	errTaskResult = MWUSB_OK;

	BF_Debug_Print_Information((">>>USB Connect Start.\n\r"));

	usb_State = MWUSB_CheckConnect(); /** keep state **/
	if( usb_State == MWUSB_PLUG_CONNECT )
	{
		/** connect usb plug **/
		BF_Debug_Print_Information((">>>Change state(Connect).\n\r"));
		/* start enumeration abort task */
		OS_User_Ref_Tsk(TID_USB_ENUM_ERR, &flag);
		if ((flag.tskstat & D_OS_USER_TTS_DMT) == D_OS_USER_TTS_DMT) {
			OS_User_Sta_Tsk(TID_USB_ENUM_ERR, 0);
		}
		
		/* connect USB */
		MWUSB_IntConnect();
		
		flg_ptn = FLG_MAIN_USB_Disconnect;
		OS_User_Clr_Flg(FID_USB_MAIN , ~(flg_ptn));

	}
	else /* if( usb_State == MWUSB_PLUG_RELEASE ) */
	{
		flg_ptn = FLG_MAIN_USB_Error;
		OS_User_Set_Flg( FID_USB_API , flg_ptn);
		
		/* Clear ram sleep bit for USB macro in EXS module */
		Dd_Exs_Set_RAM_PD_Ctrl_USB2(0);

		BF_Debug_Print_Information((">>>USB Cable NG.\n\r"));

		/* End task */
		OS_User_Ext_Tsk();
	}

	if(errTaskResult == MWUSB_OK)
	{
		flg_ptn = FLG_MAIN_USB_Complete;
		OS_User_Set_Flg( FID_USB_API , flg_ptn);

		BF_Debug_Print_Information((">>>USB Connect Complete.\n\r"));
	}
	else
	{
		flg_ptn = FLG_MAIN_USB_Error;
		OS_User_Set_Flg( FID_USB_API , flg_ptn);
		
		/* Clear ram sleep bit for USB macro in EXS module */
		Dd_Exs_Set_RAM_PD_Ctrl_USB2(0);

		BF_Debug_Print_Information((">>>USB Connect NG.\n\r"));

		/* End task */
		OS_User_Ext_Tsk();
	}

	// Wait Flg
	flg_ptn = FLG_MAIN_USB_Disconnect;
	OS_User_Wai_Flg(FID_USB_MAIN, flg_ptn, D_OS_USER_TWF_ORW, &rtn_ptn);
	OS_User_Clr_Flg(FID_USB_MAIN , ~(flg_ptn));

	BF_Debug_Print_Information((">>>USB Disconnect Start.\n\r"));

	/* disconnect USB */
	MWUSB_IntDisconnect();
	if (MWUSBC_GetUsbClass() == USB_MODE_STORAGE) {
		/* terminate MSC lib. */
		MSCM_End();
	}

	usb_State = MWUSB_PLUG_RELEASE;

	/* Clear ram sleep bit for USB macro in EXS module */
	Dd_Exs_Set_RAM_PD_Ctrl_USB2(0);

	flg_ptn = FLG_MAIN_USB_Complete;
	OS_User_Set_Flg( FID_USB_API , flg_ptn);

	BF_Debug_Print_Information((">>>USB Disconnect Complete.\n\r"));

	/* End task */
	OS_User_Ext_Tsk();

}

void usb_main_task(int stacd) {
	usb_main();
}

void usb_enum_err_task(int stacd)
{
	OS_USER_SYSTIM st_sys_time;
	OS_USER_SYSTIM chk_sys_time;
	
	OS_User_Get_Tim(&st_sys_time);
	
	while(1) {
		if (MWUSB_CheckConnect() == MWUSB_PLUG_CONNECT) {
			if (MWUSB_CheckConfig() == MWUSB_STAT_UNCONF) {
				OS_User_Get_Tim(&chk_sys_time);
				if ((chk_sys_time - st_sys_time) > ENUM_WAIT_TIME) {
					errTaskResult = MWUSB_NG;
					/* disconnect USB plug */
					MWUSB_IntDisconnect();
					break;  /* ERROR END */
				}
			} else if (MWUSB_CheckConfig() == MWUSB_STAT_CONFIG) {
				break;  /* NORMAL END */
			} else if ((MWUSB_CheckConfig() == MWUSB_STAT_SUSPEND) ||
					   (MWUSB_CheckConfig() == MWUSB_STAT_BUSRESET)) {
				/* Reset the measurement of enumeration period */
				OS_User_Get_Tim(&st_sys_time);
			}
		} else {  /* MWUSB_PLUG_RELEASE */
			errTaskResult = MWUSB_NG;
			/* disconnect USB plug */
			MWUSB_IntDisconnect();
			break;  /* ERROR END */
		}
		OS_User_Dly_Tsk(100);
	}
	
	OS_User_Ext_Tsk();
}


void BF_Start_USB(void)
{

	T_OS_USER_RTSK flag;

	OS_User_Ref_Tsk(TID_USB_MAIN, &flag);
	if ((flag.tskstat & D_OS_USER_TTS_DMT) == D_OS_USER_TTS_DMT) {
		OS_User_Sta_Tsk(TID_USB_MAIN, 0);
	}
}

