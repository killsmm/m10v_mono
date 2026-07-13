/**
 * @file		USBHDC_Cmn_Otg.c
 * @brief		USB Host & Device otg control function
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifdef USE_ON_THE_GO
#include "USBHDC_Cmn_Otg.h"

#include "USBHDC_MemMap.h"
#include "USBHDC_AccessIF.h"
#include "USBHDC_GeneralFunc.h"


/*****************************************************************
 * [ Function ]		get vbus state.                              *
 * [ Return code ]	u8 : (0) vbus invalid                        *
 *                       (1) vbus valid                          *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
u8 Cmn_OtgGetVbusState(void)
{
	u32 reg_otgsts_val;

	/* read OTGSTS register */
	reg_otgsts_val = ReadRegister(OTGSTS);

	/* return vbus state(bit10) */
	return (u8)((reg_otgsts_val >> 10) & 0x1);
}


/*****************************************************************
 * [ Function ]		Otg rise interrupt enable.                   *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u32 bit_pos : interrupt bit position         *
 *****************************************************************/
void Cmn_OtgIntEnRise(u32 bit_pos)
{
	SetBitRegister(OTGSTSRISE, bit_pos);
}


/*****************************************************************
 * [ Function ]		Otg rise interrupt disable.                  *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u32 bit_pos : interrupt bit position         *
 *****************************************************************/
void Cmn_OtgIntDisRise(u32 bit_pos)
{
	ClearBitRegister(OTGSTSRISE, bit_pos);
}


/*****************************************************************
 * [ Function ]		clear otg interrupt state.                   *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u32 int_pos : bit position of interrupt state*
 *****************************************************************/
void Cmn_OtgClearIntState(u32 int_pos)
{
	/* write OTGSTSC register */
	WriteRegister(OTGSTSC, ~int_pos & 0x000007CF);
}


/*****************************************************************
 * [ Function ]		start timer.                                 *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u16 time : time(down counter)                *
 *                  u8 scale : time scale. (p_clk_i/4)           *
 *                  u8 mode  : 0=one shot, 1=interval            *
 *****************************************************************/
void Cmn_OtgTimerStart(u16 time, u8 scale, u8 mode)
{
	u32 reg_otgtc_val;

	/* set time */
	WriteRegister(OTGT, time & 0xFFFF);

	/* set timer scale & mode */
	reg_otgtc_val = ((scale << 8) & 0xFF00) | ((mode << 1) & 0x2) | 0x1;
	WriteRegister(OTGTC, reg_otgtc_val);
}


/*****************************************************************
 * [ Function ]		stop timer.                                  *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	none.                                        *
 *****************************************************************/
void Cmn_OtgTimerStop(void)
{
	/* timer stop */
	ClearBitRegister(OTGTC, 0x1);
}


/*****************************************************************
 * [ Function ]		wait time.                                   *
 * [ Return code ]	none.                                        *
 * [ Parameter ]	u16 time : wait time(down counter)           *
 *                  u8 scale : time scale. (p_clk_i/4)           *
 *                  u8 mode  : 0=one shot, 1=interval            *
 *****************************************************************/
void Cmn_WaitTime(u16 time, u8 scale, u8 mode)
{
	u32 reg_otgstsc_val;

	/* timeout int enable */
	Cmn_OtgIntEnRise(0x1);

	/* timer start */
	Cmn_OtgTimerStart(time, scale, mode);

	/* wait timeout */
	while(1) {
		reg_otgstsc_val = ReadRegister(OTGSTSC);
		if ((reg_otgstsc_val & 0x1) == 0x1) {
			Cmn_OtgTimerStop();
			Cmn_OtgClearIntState(0x1);
			break;
		}
	}

	/* timeout int disable */
	Cmn_OtgIntDisRise(0x1);
}

#endif  /* USE_ON_THE_GO */

