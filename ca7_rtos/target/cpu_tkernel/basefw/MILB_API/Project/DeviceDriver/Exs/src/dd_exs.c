/**
 * @file		dd_exs.c
 * @brief		Exs driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include "exstop.h"
#include "dd_exs.h"
#include "driver_common.h"
#include "ddim_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

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
static ULONG gDD_Exs_Spin_Lock __attribute__((section(".LOCK_SECTION"), aligned(64)));

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/

/**
Set status of transition requests to LowPower mode of XDMAC.<br>
@param[in]  *val Setting value
					<ul><li> 0:LowPower mode request.
						<li> 1:Normal mode request.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_XDMAC_LowPower_Ctl_CSYSREQ(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.XDMACLPCTRL.bit.CSYSREQ = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set reset control of nf_rst_n terminal.<br>
@param[in]  *val Setting value.
					<ul><li> 0:nf_rst_n terminal reset is released.
						<li> 1:nf_rst_n terminal reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_nf_rst_n_Soft_Reset(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.SOFTRESET.bit.NFRSTN = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set reset control of bch_rst_n terminal.<br>
@param[in]  *val Setting value
					<ul><li> 0:bch_rst_n terminal reset is released.
						<li> 1:bch_rst_n terminal reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_bch_rst_n_Soft_Reset(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.SOFTRESET.bit.NFRSTNBCH = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set reset control of reg_rst_n terminal.<br>
@param[in]  *val Setting value
					<ul><li> 0:reg_rst_n terminal reset is released.
						<li> 1:reg_rst_n terminal reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_reg_rst_n_Soft_Reset(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.SOFTRESET.bit.NFRSTNREG = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set reset control of NETSEC macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:NETSEC macro reset is released.
						<li> 1:NETSEC macro reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_NETSEC_Soft_Reset(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.SOFTRESET.bit.NETSECRST = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set reset control of eMMC macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:eMMC macro reset is released.
						<li> 1:eMMC macro reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_eMMC_Soft_Reset(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.SOFTRESET.bit.EMCRST = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set reset control of RELC macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:RELC macro reset is released.
						<li> 1:RELC macro reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RELC_Soft_Reset(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.SOFTRESET.bit.RELCRST = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set reset control of Card Debounce SD3 (UHS2).<br>
@param[in]  *val Setting value
					<ul><li> 0:Card Debounce reset is released.
					<ul><li> 1:Card Debounce reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_SD3_CD_Debounce_Soft_Reset(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.SOFTRESET2.bit.DEBRSTX_SD3 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set reset control of Card Debounce SD4 (UHS1 Card).<br>
@param[in]  *val Setting value
					<ul><li> 0:Card Debounce reset is released.
						<li> 1:Card Debounce reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_SD4_CD_Debounce_Soft_Reset(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.SOFTRESET2.bit.DEBRSTX_SD4 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set reset control of Card Debounce PCIe0.<br>
@param[in]  *val Setting value
					<ul><li> 0:Card Debounce reset is released.
						<li> 1:Card Debounce reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_PCIE0_CD_Debounce_Soft_Reset(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.SOFTRESET2.bit.DEBRSTX_PCIE0 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set reset control of Card Debounce PCIe1.<br>
@param[in]  *val Setting value
					<ul><li> 0:Card Debounce reset is released.
						<li> 1:Card Debounce reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_PCIE1_CD_Debounce_Soft_Reset(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.SOFTRESET2.bit.DEBRSTX_PCIE1 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set reset control of Card Debounce USB2 VBUS.<br>
@param[in]  *val Setting value
					<ul><li> 0:Card Debounce reset is released.
						<li> 1:Card Debounce reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_USB2VBUS_CD_Debounce_Soft_Reset(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.SOFTRESET2.bit.DEBRSTX_USB2VBUS = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set reset control of Card Debounce USB2 ID.<br>
@param[in]  *val Setting value
					<ul><li> 0:Card Debounce reset is released.
						<li> 1:Card Debounce reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_USB2ID_CD_Debounce_Soft_Reset(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.SOFTRESET2.bit.DEBRSTX_USB2ID = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set reset control of Card Debounce USB3 VBUS.<br>
@param[in]  *val Setting value
					<ul><li> 0:Card Debounce reset is released.
						<li> 1:Card Debounce reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_USB3VBUS_CD_Debounce_Soft_Reset(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.SOFTRESET2.bit.DEBRSTX_USB3VBUS = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set reset control of Card Debounce USB3 ID.<br>
@param[in]  *val Setting value
					<ul><li> 0:Card Debounce reset is released.
						<li> 1:Card Debounce reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_USB3ID_CD_Debounce_Soft_Reset(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.SOFTRESET2.bit.DEBRSTX_USB3ID = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SD3( UHS2 ) Detect INT Mask(Rising Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the SD3( UHS2 ) detection rising edge detection factor is output.
						<li> 1:The interrupt signal by the SD3( UHS2 ) detection rising edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_SD3_RIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTMSK.bit.INTMSK02 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SD3( UHS2 ) Detect INT Mask(Falling Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the SD3( UHS2 ) detection falling edge detection factor is output.
						<li> 1:The interrupt signal by the SD3( UHS2 ) detection falling edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_SD3_FIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTMSK.bit.INTMSK03 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SD4( UHS1 Card ) Detect INT Mask(Rising Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the SD4( UHS1 Card ) detection rising edge detection factor is output.
						<li> 1:The interrupt signal by the SD4( UHS1 Card ) detection rising edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_SD4_RIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTMSK.bit.INTMSK04 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SD4( UHS1 Card ) Detect INT Mask(Falling Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the SD4( UHS1 Card ) detection falling edge detection factor is output.
						<li> 1:The interrupt signal by the SD4( UHS1 Card ) detection falling edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_SD4_FIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTMSK.bit.INTMSK05 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PCIe0 Detect INT Mask(Rising Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the PCIe0 detection rising edge detection factor is output.
						<li> 1:The interrupt signal by the PCIe0 detection rising edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_PCIe0_RIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTMSK.bit.INTMSK06 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PCIe0 Detect INT Mask(Falling Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the PCIe0 detection falling edge detection factor is output.
						<li> 1:The interrupt signal by the PCIe0 detection falling edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_PCIe0_FIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTMSK.bit.INTMSK07 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PCIe1 Detect INT Mask(Rising Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the PCIe1 detection rising edge detection factor is output.
						<li> 1:The interrupt signal by the PCIe1 detection rising edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_PCIe1_RIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTMSK.bit.INTMSK08 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PCIe1 Detect INT Mask(Falling Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the PCIe1 detection falling edge detection factor is output.
						<li> 1:The interrupt signal by the PCIe1 detection falling edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_PCIe1_FIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTMSK.bit.INTMSK09 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB2 VBUS Detect INT Mask(Rising Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the USB2 VBUS detection rising edge detection factor is output.
						<li> 1:The interrupt signal by the USB2 VBUS detection rising edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_USB2VBUS_RIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTMSK.bit.INTMSK10 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB2 VBUS Detect INT Mask(Falling Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the USB2 VBUS detection falling edge detection factor is output.
						<li> 1:The interrupt signal by the USB2 VBUS detection falling edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_USB2VBUS_FIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTMSK.bit.INTMSK11 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB2 ID Detect INT Mask(Rising Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the USB2 ID detection rising edge detection factor is output.
						<li> 1:The interrupt signal by the USB2 ID detection rising edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_USB2ID_RIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTMSK.bit.INTMSK12 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB2 ID Detect INT Mask(Falling Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the USB2 ID detection falling edge detection factor is output.
						<li> 1:The interrupt signal by the USB2 ID detection falling edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_USB2ID_FIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTMSK.bit.INTMSK13 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB3 VBUS Detect INT Mask(Rising Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the USB3 VBUS detection rising edge detection factor is output.
						<li> 1:The interrupt signal by the USB3 VBUS detection rising edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_USB3VBUS_RIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTMSK.bit.INTMSK14 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB3 VBUS Detect INT Mask(Falling Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the USB3 VBUS detection falling edge detection factor is output.
						<li> 1:The interrupt signal by the USB3 VBUS detection falling edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_USB3VBUS_FIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTMSK.bit.INTMSK14 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB3 ID Detect INT Mask(Rising Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the USB3 ID detection rising edge detection factor is output.
						<li> 1:The interrupt signal by the USB3 ID detection rising edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_USB3ID_RIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTMSK.bit.INTMSK16 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB3 ID Detect INT Mask(Falling Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the USB3 ID detection falling edge detection factor is output.
						<li> 1:The interrupt signal by the USB3 ID detection falling edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_USB3ID_FIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTMSK.bit.INTMSK17 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}
/**
Set SD3( UHS2 ) Debounce ON/OFF.<br>
@param[in]  *val Setting value
					<ul><li> 0:The debounce of the SD3( UHS2 ) signal is turned off.
						<li> 1:The debounce of the SD3( UHS2 ) signal is turned on.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_SD3_EN(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.DEBEN.bit.DEBEN_SD3 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SD4( UHS1 Card ) Debounce ON/OFF.<br>
@param[in]  *val Setting value
					<ul><li> 0:The debounce of the SD4( UHS1 Card ) signal is turned off.
						<li> 1:The debounce of the SD4( UHS1 Card ) signal is turned on.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_SD4_EN(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.DEBEN.bit.DEBEN_SD4 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PCIe0 Debounce ON/OFF.<br>
@param[in]  *val Setting value
					<ul><li> 0:The debounce of the PCIe0 signal is turned off.
						<li> 1:The debounce of the PCIe0 signal is turned on.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_PCIE0_EN(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.DEBEN.bit.DEBEN_PCIE0 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PCIe1 Debounce ON/OFF.<br>
@param[in]  *val Setting value
					<ul><li> 0:The debounce of the PCIe1 signal is turned off.
						<li> 1:The debounce of the PCIe1 signal is turned on.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_PCIE1_EN(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.DEBEN.bit.DEBEN_PCIE1 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB2 VBUS Debounce ON/OFF.<br>
@param[in]  *val Setting value
					<ul><li> 0:The debounce of the USB2 VBUS signal is turned off.
						<li> 1:The debounce of the USB2 VBUS signal is turned on.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_USB2VBUS_EN(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.DEBEN.bit.DEBEN_USB2VBUS = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB2 ID Debounce ON/OFF.<br>
@param[in]  *val Setting value
					<ul><li> 0:The debounce of the USB2 ID signal is turned off.
						<li> 1:The debounce of the USB2 ID signal is turned on.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_USB2ID_EN(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.DEBEN.bit.DEBEN_USB2ID = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB3 VBUS Debounce ON/OFF.<br>
@param[in]  *val Setting value
					<ul><li> 0:The debounce of the USB3 VBUS signal is turned off.
						<li> 1:The debounce of the USB3 VBUS signal is turned on.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_USB3VBUS_EN(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.DEBEN.bit.DEBEN_USB3VBUS = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB3 ID Debounce ON/OFF.<br>
@param[in]  *val Setting value
					<ul><li> 0:The debounce of the USB3 ID signal is turned off.
						<li> 1:The debounce of the USB3 ID signal is turned on.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_USB3ID_EN(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.DEBEN.bit.DEBEN_USB3ID = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SD-Card ch3 Debounce time.<br>
@param[in]  *val Setting value
					<ul><li> 00:APBCLK 131072 cycles<br>
						<li> 01:APBCLK 65536 cycles (131072 cycles /2).
						<li> 10:APBCLK 262144 cycles (131072 cycles *2).
						<li> 11:APBCLK 524288 cycles (131072 cycles *4).</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_SD3DETIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.DEBEN.bit.SD3DETIM = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SD-Card ch4 Debounce time..<br>
@param[in]  *val Setting value
					<ul><li> 00:APBCLK 131072 cycles<br>
						<li> 01:APBCLK 65536 cycles (131072 cycles /2).
						<li> 10:APBCLK 262144 cycles (131072 cycles *2).
						<li> 11:APBCLK 524288 cycles (131072 cycles *4).</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_SD4DETIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.DEBEN.bit.SD4DETIM = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PCIe0 Debounce time.<br>
@param[in]  *val Setting value
					<ul><li> 00:APBCLK 131072 cycles<br>
						<li> 01:APBCLK 65536 cycles (131072 cycles /2).
						<li> 10:APBCLK 262144 cycles (131072 cycles *2).
						<li> 11:APBCLK 524288 cycles (131072 cycles *4).</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_PCIE0DETIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.DEBEN.bit.PCIE0DETIM = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PCIe1 Debounce time.<br>
@param[in]  *val Setting value
					<ul><li> 00:APBCLK 131072 cycles<br>
						<li> 01:APBCLK 65536 cycles (131072 cycles /2).
						<li> 10:APBCLK 262144 cycles (131072 cycles *2).
						<li> 11:APBCLK 524288 cycles (131072 cycles *4).</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_PCIE1DETIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.DEBEN.bit.PCIE1DETIM = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB2 VBUS Debounce time.<br>
@param[in]  *val Setting value
					<ul><li> 00:APBCLK 131072 cycles<br>
						<li> 01:APBCLK 65536 cycles (131072 cycles /2).
						<li> 10:APBCLK 262144 cycles (131072 cycles *2).
						<li> 11:APBCLK 524288 cycles (131072 cycles *4).</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_USB2VBUSDETIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.DEBEN.bit.USB2VBUSDETIM = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB2 ID Debounce time.<br>
@param[in]  *val Setting value
					<ul><li> 00:APBCLK 131072 cycles<br>
						<li> 01:APBCLK 65536 cycles (131072 cycles /2).
						<li> 10:APBCLK 262144 cycles (131072 cycles *2).
						<li> 11:APBCLK 524288 cycles (131072 cycles *4).</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_USB2IDDETIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.DEBEN.bit.USB2IDDETIM = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB3 VBUS Debounce time.<br>
@param[in]  *val Setting value
					<ul><li> 00:APBCLK 131072 cycles<br>
						<li> 01:APBCLK 65536 cycles (131072 cycles /2).
						<li> 10:APBCLK 262144 cycles (131072 cycles *2).
						<li> 11:APBCLK 524288 cycles (131072 cycles *4).</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_USB3VBUSDETIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.DEBEN.bit.USB3VBUSDETIM = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB3 ID Debounce time.<br>
@param[in]  *val Setting value
					<ul><li> 00:APBCLK 131072 cycles<br>
						<li> 01:APBCLK 65536 cycles (131072 cycles /2).
						<li> 10:APBCLK 262144 cycles (131072 cycles *2).
						<li> 11:APBCLK 524288 cycles (131072 cycles *4).</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_USB3IDDETIM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.DEBEN.bit.USB3IDDETIM = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the mode setting of the card detection SD3( UHS2 ).<br>
param[in]  *val Setting value
					<ul><li> 0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the SD3( UHS2 ) signal.
						<li> 1:After exiting reset at debounce circuit, generates an interrupt by the state of the SD3( UHS2 ) signal.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_CDINS_SD3_MOD(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.CDINSMDCTL.bit.CDIMOD_SD3 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the mode setting of the card detection SD4( UHS1 Card ).<br>
@param[in]  *val Setting value
					<ul><li> 0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the SD4( UHS1 Card ) signal.
						<li> 1:After exiting reset at debounce circuit, generates an interrupt by the state of the SD4( UHS1 Card ) signal.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_CDINS_SD4_MOD(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.CDINSMDCTL.bit.CDIMOD_SD4 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the mode setting of the card detection PCIe0.<br>
@param[in]  *val Setting value
					<ul><li> 0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the PCIe0 signal.
						<li> 1:After exiting reset at debounce circuit, generates an interrupt by the state of the PCIe0 signal.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_CDINS_PCIE0_MOD(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.CDINSMDCTL.bit.CDIMOD_PCIE0 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the mode setting of the card detection PCIe1.<br>
@param[in]  *val Setting value
					<ul><li> 0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the PCIe1 signal.
						<li> 1:After exiting reset at debounce circuit, generates an interrupt by the state of the PCIe1 signal.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_CDINS_PCIE1_MOD(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.CDINSMDCTL.bit.CDIMOD_PCIE1 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the mode setting of the card detection USB2 VBUS.<br>
@param[in]  *val Setting value
					<ul><li> 0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the USB2 VBUS signal.
						<li> 1:After exiting reset at debounce circuit, generates an interrupt by the state of the USB2 VBUS signal.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_CDINS_USB2VBUS_MOD(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.CDINSMDCTL.bit.CDIMOD_USB2VBUS = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the mode setting of the card detection USB2 ID.<br>
@param[in]  *val Setting value
					<ul><li> 0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the USB2 ID signal.
						<li> 1:After exiting reset at debounce circuit, generates an interrupt by the state of the USB2 ID signal.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_CDINS_USB2ID_MOD(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.CDINSMDCTL.bit.CDIMOD_USB2ID = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the mode setting of the card detection USB3 VBUS.<br>
@param[in]  *val Setting value
					<ul><li> 0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the USB3 VBUS signal.
						<li> 1:After exiting reset at debounce circuit, generates an interrupt by the state of the USB3 VBUS signal.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_CDINS_USB3VBUS_MOD(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.CDINSMDCTL.bit.CDIMOD_USB3VBUS = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the mode setting of the card detection USB3 ID.<br>
@param[in]  *val Setting value
					<ul><li> 0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the USB3 ID signal.
						<li> 1:After exiting reset at debounce circuit, generates an interrupt by the state of the USB3 ID signal.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_CDINS_USB3ID_MOD(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.CDINSMDCTL.bit.CDIMOD_USB3ID = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the RELC expanded data read side HPROT setting(Data/Opcode).<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_RELC_HPROT_Read(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.RLC_HPT_CTL.bit.RERHPT = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the RELC expanded data write side HPROT setting(Data/Opcode).<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_RELC_HPROT_Write(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.RLC_HPT_CTL.bit.REWHPT = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SD2( UHS1 Wifi ) macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_SD2(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.RAM_PD.bit.RAMPD_SD2 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SD3( UHS2 ) macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_SD3(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.RAM_PD.bit.RAMPD_SD3 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SD4( UHS1 Card ) macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_SD4(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.RAM_PD.bit.RAMPD_SD4 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set Nand Flash macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_NF(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.RAM_PD.bit.RAMPD_NF = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set RELC macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_RELC(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.RAM_PD.bit.RAMPD_RELC = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set NETSEC macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_NETSEC(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.RAM_PD.bit.RAMPD_NETSEC = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB2 macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_USB2(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.RAM_PD.bit.RAMPD_USB2 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB3 macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_USB3(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.RAM_PD.bit.RAMPD_USB3 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PCIE0 macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_PCIE0(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.RAM_PD.bit.RAMPD_PCIE0 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PCIE1 macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_PCIE1(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.RAM_PD.bit.RAMPD_PCIE1 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set eMMC macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_EMMC(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.RAM_PD.bit.RAMPD_EMMC = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the output a signal to stop control of the Nand Flash BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output Nand Flash BUS CLOCK.
						<li> 1:Stop Nand Flash BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_NF_BUS_CLOCK_Stop_Ctrl(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.BUSCKSTP.bit.NF_CKSTP = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the output a signal to stop control of the PCIE0 BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output PCIE0 BUS CLOCK.
						<li> 1:Stop PCIE0 BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_PCIE0_BUS_CLOCK_Stop_Ctrl(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.BUSCKSTP.bit.PCIE0_CKSTP = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the output a signal to stop control of the PCIE1 BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output PCIE1 BUS CLOCK.
						<li> 1:Stop PCIE1 BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_PCIE1_BUS_CLOCK_Stop_Ctrl(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.BUSCKSTP.bit.PCIE1_CKSTP = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the output a signal to stop control of the XDMAC BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output XDMAC BUS CLOCK.
						<li> 1:Stop XDMAC BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_XDMAC_BUS_CLOCK_Stop_Ctrl(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.BUSCKSTP.bit.XDMAC_CKSTP = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the output a signal to stop control of the HDMAC BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output HDMAC BUS CLOCK.
						<li> 1:Stop HDMAC BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_HDMAC_BUS_CLOCK_Stop_Ctrl(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.BUSCKSTP.bit.HDMAC_CKSTP = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the output a signal to stop control of the eMMC BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output eMMC BUS CLOCK.
						<li> 1:Stop eMMC BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_EMMC_BUS_CLOCK_Stop_Ctrl(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.BUSCKSTP.bit.EMMC_CKSTP = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the output a signal to stop control of the SD2 (Wifi) BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output SD2 (Wifi) BUS CLOCK.
						<li> 1:Stop SD2 (Wifi) BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_SD2_BUS_CLOCK_Stop_Ctrl(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.BUSCKSTP.bit.SD2_CKSTP = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the output a signal to stop control of the SD3 (UHS2) BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output SD3 (UHS2) BUS CLOCK.
						<li> 1:Stop SD3 (UHS2) BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_SD3_BUS_CLOCK_Stop_Ctrl(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.BUSCKSTP.bit.SD3_CKSTP = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the output a signal to stop control of the SD4 (UHS1 Card) BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output SD4 (UHS1 Card) BUS CLOCK.
						<li> 1:Stop SD4 (UHS1 Card) BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_SD4_BUS_CLOCK_Stop_Ctrl(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.BUSCKSTP.bit.SD4_CKSTP = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the output a signal to stop control of the NETSEC BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output NETSEC BUS CLOCK.
						<li> 1:Stop NETSEC BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_NETSEC_BUS_CLOCK_Stop_Ctrl(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.BUSCKSTP.bit.NETSEC_CKSTP = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the output a signal to stop control of the RELC BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output RELC BUS CLOCK.
						<li> 1:Stop RELC BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RELC_BUS_CLOCK_Stop_Ctrl(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.BUSCKSTP.bit.RELC_CKSTP = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the output a signal to stop control of the PTP_CLK NETSEC macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output PTP_CLK.
						<li> 1:Stop PTP_CLK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_NETSEC_PTPCLK_Stop_Ctrl_NETSEC_PTPSTP(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.NSEC_CKSTP.bit.NETSECPTPSTP = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set the output a signal to stop control of the MMC_BCLK_I.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output MMC_BCLK_I.
						<li> 1:Stop MMC_BCLK_I.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_EMMC_CKSTP_EMCLKSTP(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.EMMC_CKSTP.bit.EMCLKSTP = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PV_DTIMEC.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_EM_PV_DTIMEC(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.EM_PV_DTIMEC.bit.PV_DTIMEC = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PV_AMPBL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_EM_PV_AMPBL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.EM_PV_AMPBL.bit.PV_AMPBL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set CR_SLOTTYPE.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_EM_CR_SLOTTYPE(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.EM_CR_SLOTTYPE.bit.CR_SLOTTYPE = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set CR_BCLKFREQ.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_EM_CR_BCLKFREQ(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.EM_CR_BCLKFREQ.bit.CR_BCLKFREQ = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set EMCD.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_EM_CDDET_EMCD(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.EM_CDDET.bit.EMCD = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set HDC_HCLKEN.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_CKCTL_HDC_HCLKEN(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_CKCTL.bit.HDC_HCLKEN = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set HO_HCLKEN.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_CKCTL_HO_HCLKEN(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_CKCTL.bit.HO_HCLKEN = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set HDC_SRST.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_RCTL_HDC_SRST(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_RCTL.bit.HDC_SRST = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set HO_SRST.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_RCTL_HO_SRST(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_RCTL.bit.HO_SRST = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set ANPD.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_ANPD_ANPD(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_ANPD.bit.ANPD = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set HF_SEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_HFSEL_HF_SEL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_HFSEL.bit.HF_SEL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set FSEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_FSEL_FSEL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_FSEL.bit.FSEL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PLLBTUNE.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_FSEL_PLLBTUNE(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_FSEL.bit.PLLBTUNE = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SS_FADJ_HOST_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_EHCI_STRAP_SS_FADJ_HOST_I(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_EHCI_STRAP.bit.SS_FADJ_HOST_I = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SS_FLADJ_VAL_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_EHCI_STRAP_SS_FLADJ_VAL_I(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_EHCI_STRAP.bit.SS_FLADJ_VAL_I = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SS_FLADJ_VAL_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_OHCI_IF_OHCI_CLKSEL_I(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_OHCI_IF.bit.OHCI_CLKSEL_I = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SS_ENA_INCR4_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_LMODSET_SS_ENA_INCR4_I(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_LMODSET.bit.SS_ENA_INCR4_I = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SS_ENA_INCR8_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_LMODSET_SS_ENA_INCR8_I(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_LMODSET.bit.SS_ENA_INCR8_I = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SS_ENA_INCR16_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_LMODSET_SS_ENA_INCR16_I(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_LMODSET.bit.SS_ENA_INCR16_I = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SS_ENA_INC_AL_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_LMODSET_SS_ENA_INC_AL_I(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_LMODSET.bit.SS_ENA_INC_AL_I = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SS_EHCI64B_EN_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_LMODSET_SS_EHCI64B_EN_I(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_LMODSET.bit.SS_EHCI64B_EN_I = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set RPDPEN_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_PMODSET_RPDPEN_I(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_PMODSET.bit.RPDPEN_I = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set RPDMEN_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_PMODSET_RPDMEN_I(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_PMODSET.bit.RPDMEN_I = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB2_EHPT.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_HPT_CTL_USB2_EHPT(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_HPT_CTL.bit.USB2_EHPT = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB2_OHPT.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_HPT_CTL_USB2_OHPT(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_HPT_CTL.bit.USB2_OHPT = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB2_IDPU_VAL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_IDVBUSCTL_USB2_IDPU_VAL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_IDVBUSCTL.bit.USB2_IDPU_VAL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB2_IDPU_SEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_IDVBUSCTL_USB2_IDPU_SEL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_IDVBUSCTL.bit.USB2_IDPU_SEL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB2_ID_SEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_IDVBUSCTL_USB2_ID_SEL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_IDVBUSCTL.bit.USB2_ID_SEL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB2_ID_VAL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_IDVBUSCTL_USB2_ID_VAL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_IDVBUSCTL.bit.USB2_ID_VAL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB2_VB_SEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_IDVBUSCTL_USB2_VB_SEL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_IDVBUSCTL.bit.USB2_VB_SEL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB2_VB_VAL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_IDVBUSCTL_USB2_VB_VAL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_IDVBUSCTL.bit.USB2_VB_VAL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB2_VBDET_SEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_IDVBUSCTL_USB2_VBDET_SEL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_IDVBUSCTL.bit.USB2_VBDET_SEL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB2_VBDET_VAL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_IDVBUSCTL_USB2_VBDET_VAL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_IDVBUSCTL.bit.USB2_VBDET_VAL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set HDMAC_BC1.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_HDMAC1_HDMAC_BC1(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_HDMAC1.bit.HDMAC_BC1 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set HDMAC_BT1.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_HDMAC1_HDMAC_BT1(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_HDMAC1.bit.HDMAC_BT1 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set HDMAC_MS1.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_HDMAC1_HDMAC_MS1(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_HDMAC1.bit.HDMAC_MS1 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set HDMAC_BC2.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_HDMAC2_HDMAC_BC2(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_HDMAC2.bit.HDMAC_BC2 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set HDMAC_BT2.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_HDMAC2_HDMAC_BT2(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_HDMAC2.bit.HDMAC_BT2 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set HDMAC_MS2.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_HDMAC2_HDMAC_MS2(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_HDMAC2.bit.HDMAC_MS2 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set FSMRST1.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_DMAFSM1_FSMRST1(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_DMAFSM1.bit.FSMRST1 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set FSMRST2.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_DMAFSM2_FSMRST2(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB2_DMAFSM2.bit.FSMRST2 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set ACLKEN.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_CKCTL_ACLKEN(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_CKCTL.bit.ACLKEN = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SFTRST.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_RCTL_SFTRST(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_RCTL.bit.SFTRST = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PHY_RESET_N.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_RCTL_PHY_RESET_N(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_RCTL.bit.PHY_RESET_N = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set ACJT_LEVEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_RCV_SNS_ACJT_LEVEL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_RCV_SNS.bit.ACJT_LEVEL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set FSEL3.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_FSEL_FSEL3(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_FSEL.bit.FSEL3 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set MPLL_MULTIPLIER.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_FSEL_MPLL_MULTIPLIER(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_FSEL.bit.MPLL_MULTIPLIER = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set SSC_REF_CLK_SEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_FSEL_SSC_REF_CLK_SEL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_FSEL.bit.SSC_REF_CLK_SEL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set REF_CLKDIV2.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_FSEL_REF_CLKDIV2(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_FSEL.bit.REF_CLKDIV2 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set REF_SSP_EN.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_RFC_CTL_REF_SSP_EN(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_RFC_CTL.bit.REF_SSP_EN = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set REF_USE_PAD.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_RFC_CTL_REF_USE_PAD(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_RFC_CTL.bit.REF_USE_PAD = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PCI_IMPLEMENT.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_HO_IF_PCI_IMPLEMENT(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_HO_IF.bit.PCI_IMPLEMENT = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set HOST_MSI_ENABLE.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_HO_IF_HOST_MSI_ENABLE(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_HO_IF.bit.HOST_MSI_ENABLE = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set XHC_BME.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_HO_IF_XHC_BME(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_HO_IF.bit.XHC_BME = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set XHC_AC64.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_HO_IF_XHC_AC64(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_HO_IF.bit.XHC_AC64 = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set FLADJ_30MHZ_REG.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_JT_ADJ_FLADJ_30MHZ_REG(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_JT_ADJ.bit.FLADJ_30MHZ_REG = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB3_OVRCUR_EN.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_JT_ADJ_USB3_OVRCUR_EN(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_JT_ADJ.bit.USB3_OVRCUR_EN = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB3_IDPU_VAL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_IDVBUSCTL_USB3_IDPU_VAL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_IDVBUSCTL.bit.USB3_IDPU_VAL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB3_ID_SEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_IDVBUSCTL_USB3_ID_SEL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_IDVBUSCTL.bit.USB3_ID_SEL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB3_ID_VAL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_IDVBUSCTL_USB3_ID_VAL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_IDVBUSCTL.bit.USB3_ID_VAL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB3_VB_SEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_IDVBUSCTL_USB3_VB_SEL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_IDVBUSCTL.bit.USB3_VB_SEL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB3_VB_VAL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_IDVBUSCTL_USB3_VB_VAL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_IDVBUSCTL.bit.USB3_VB_VAL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB3_VBDET_SEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_IDVBUSCTL_USB3_VBDET_SEL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_IDVBUSCTL.bit.USB3_VBDET_SEL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set USB3_VBDET_VAL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_IDVBUSCTL_USB3_VBDET_VAL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.USB3_IDVBUSCTL.bit.USB3_VBDET_VAL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set DSCV_INHIBIT.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_IP_CTL_DSCV_INHIBIT(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.NF_IP_CTL.bit.DSCV_INHIBIT = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set DSCV_IGNR_CRC.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_IP_CTL_DSCV_IGNR_CRC(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.NF_IP_CTL.bit.DSCV_IGNR_CRC = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set BOOT_EN.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_IP_CTL_BOOT_EN(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.NF_IP_CTL.bit.BOOT_EN = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set IDDQ_EN.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_IP_CTL_IDDQ_EN(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.NF_IP_CTL.bit.IDDQ_EN = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set RB_VALID_TIME.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_RB_VALID_TM(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.NF_RB_VALID_TM.bit.RB_VALID_TIME = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PHY_CTRL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_PHY_CTRL_REG(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.NF_PHY_CTRL_REG.bit.PHY_CTRL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PHY_DQ_TIMENG.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_PHY_DQ_TIMING(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.NF_PHY_DQ_TIM.bit.PHY_DQ_TIMING = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PHY_DQS_TIMING.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_PHY_DQS_TIMING(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.NF_PHY_DQS_TIM.bit.PHY_DQS_TIMING = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PHY_GT_LPBK_CTL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_PHY_GT_LPBK_CTL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.NF_PHY_GT_LPBK.bit.PHY_GT_LPBK_CTL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PHY_DLL_MST_CTL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_PHY_DLL_MCTL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.NF_PHY_DLL_MCTL.bit.PHY_DLL_MST_CTL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set PHY_DLL_SLV_CTL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_PHY_DLL_SCTL(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.NF_PHY_DLL_SCTL.bit.PHY_DLL_SLV_CTL = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set BOOT_ECC_SEC_SIZ.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_BOOT_ECC_CTL_SEC_SIZ(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.NF_BOOT_ECC_CTL.bit.BOOT_ECC_SEC_SIZ = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set BT_ECC_CORR_STR.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_BOOT_ECC_CTL_CORR_STR(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.NF_BOOT_ECC_CTL.bit.BT_ECC_CORR_STR = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set BOOT_ECC_EN.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_BOOT_ECC_CTL_EN(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.NF_BOOT_ECC_CTL.bit.BOOT_ECC_EN = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
Set NFWPX_CNT.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_NFWPX_CNT(ULONG val)
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.NF_NFWPX_CNT.bit.NFWPX_CNT = val;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
The status of INTSTAT of SD2_RIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_SD2_RIS( VOID )
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTSTAT.word = D_DD_EXS_SD2_RIS_BIT;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
The status of INTSTAT of SD2_FIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_SD2_FIS( VOID )
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTSTAT.word = D_DD_EXS_SD2_FIS_BIT;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
The status of INTSTAT of SD3_RIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_SD3_RIS( VOID )
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTSTAT.word = D_DD_EXS_SD3_RIS_BIT;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
The status of INTSTAT of SD3_FIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_SD3_FIS( VOID )
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTSTAT.word = D_DD_EXS_SD3_FIS_BIT;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
The status of INTSTAT of SD4_RIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_SD4_RIS( VOID )
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTSTAT.word = D_DD_EXS_SD4_RIS_BIT;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
The status of INTSTAT of SD4_FIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_SD4_FIS( VOID )
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTSTAT.word = D_DD_EXS_SD4_FIS_BIT;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
The status of INTSTAT of PCIe0_RIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_PCIE0_RIS( VOID )
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTSTAT.word = D_DD_EXS_PCIe0_RIS_BIT;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
The status of INTSTAT of PCIe0_FIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_PCIE0_FIS( VOID )
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTSTAT.word = D_DD_EXS_PCIe0_FIS_BIT;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
The status of INTSTAT of PCIe1_RIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_PCIE1_RIS( VOID )
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTSTAT.word = D_DD_EXS_PCIe1_RIS_BIT;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
The status of INTSTAT of PCIe1_FIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_PCIE1_FIS( VOID )
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTSTAT.word = D_DD_EXS_PCIe1_FIS_BIT;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
The status of INTSTAT of USB2VBUS_RIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_USB2VBUS_RIS( VOID )
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTSTAT.word = D_DD_EXS_USB2VBUS_RIS_BIT;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
The status of INTSTAT of USB2VBUS_FIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_USB2VBUS_FIS( VOID )
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTSTAT.word = D_DD_EXS_USB2VBUS_FIS_BIT;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
The status of INTSTAT of USB2ID_RIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_USB2ID_RIS( VOID )
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTSTAT.word = D_DD_EXS_USB2ID_RIS_BIT;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
The status of INTSTAT of USB2ID_FIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_USB2ID_FIS( VOID )
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTSTAT.word = D_DD_EXS_USB2ID_FIS_BIT;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
The status of INTSTAT of USB3VBUS_RIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_USB3VBUS_RIS( VOID )
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTSTAT.word = D_DD_EXS_USB3VBUS_RIS_BIT;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
The status of INTSTAT of USB3VBUS_FIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_USB3VBUS_FIS( VOID )
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTSTAT.word = D_DD_EXS_USB3VBUS_FIS_BIT;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
The status of INTSTAT of USB3ID_RIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_USB3ID_RIS( VOID )
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTSTAT.word = D_DD_EXS_USB3ID_RIS_BIT;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
The status of INTSTAT of USB3ID_FIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_USB3ID_FIS( VOID )
{
	Dd_ARM_Critical_Section_Start( gDD_Exs_Spin_Lock );
	IO_EXSTOP.INTSTAT.word = D_DD_EXS_USB3ID_FIS_BIT;
	Dd_ARM_Critical_Section_End( gDD_Exs_Spin_Lock );
}

/**
External Strage CD(CARD detector) INT handler.<br>
@param[in]  *val Setting value
					<ul><li> 0-1 bit : SD2( UHS1 Wifi ).
						<li> 2-3 bit : SD3( UHS2 ).
						<li> 4-5 bit : SD4( UHS1 Card ).
						<li> 6-7 bit : PCIe0.
						<li> 8-9 bit : PCIe1.
						<li> 10-11 bit : USB2 VBUS.
						<li> 12-13 bit : USB2 ID.
						<li> 14-15 bit : USB3 VBUS.
						<li> 16-17 bit : USB3 ID.
						<li> Even bit of 0 : The interrupt signal by the card detection rising edge detection factor is output.
						<li> Even bit of 1 : The interrupt signal by the card detection rising edge detection factor is mask.
						<li> Odd bit of 0 : The interrupt signal by the card detection falling edge detection factor is output.
						<li> Odd bit of 1 : The interrupt signal by the card detection falling edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_EXS_Int_Handler(ULONG *val)
{
	*val = IO_EXSTOP.INTSTAT.word;
	IO_EXSTOP.INTSTAT.word = *val;	// Int clear
}


// File End
