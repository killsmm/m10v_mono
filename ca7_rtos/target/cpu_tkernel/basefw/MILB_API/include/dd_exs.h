/**
 * @file		dd_exs.h
 * @brief		Exs driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _DD_EXS_H_
#define _DD_EXS_H_

/**
@addtogroup dd_exs
@{
*/

#include "ddim_typedef.h"
#include "driver_common.h"
#include "dd_arm.h"
#include "exstop.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/**
// Error Code
*/
// CD INT STATUS
#define D_DD_EXS_USB3ID_FIS_BIT		(1 << 17)	/**< Bit Location of INTSTAT[17] on INTSTAT */
#define D_DD_EXS_USB3ID_RIS_BIT		(1 << 16)	/**< Bit Location of INTSTAT[16] on INTSTAT */
#define D_DD_EXS_USB3VBUS_FIS_BIT	(1 << 15)	/**< Bit Location of INTSTAT[15] on INTSTAT */
#define D_DD_EXS_USB3VBUS_RIS_BIT	(1 << 14)	/**< Bit Location of INTSTAT[14] on INTSTAT */
#define D_DD_EXS_USB2ID_FIS_BIT		(1 << 13)	/**< Bit Location of INTSTAT[13] on INTSTAT */
#define D_DD_EXS_USB2ID_RIS_BIT		(1 << 12)	/**< Bit Location of INTSTAT[12] on INTSTAT */
#define D_DD_EXS_USB2VBUS_FIS_BIT	(1 << 11)	/**< Bit Location of INTSTAT[11] on INTSTAT */
#define D_DD_EXS_USB2VBUS_RIS_BIT	(1 << 10)	/**< Bit Location of INTSTAT[10] on INTSTAT */
#define D_DD_EXS_PCIe1_FIS_BIT		(1 << 9)	/**< Bit Location of INTSTAT[9] on INTSTAT */
#define D_DD_EXS_PCIe1_RIS_BIT		(1 << 8)	/**< Bit Location of INTSTAT[8] on INTSTAT */
#define D_DD_EXS_PCIe0_FIS_BIT		(1 << 7)	/**< Bit Location of INTSTAT[7] on INTSTAT */
#define D_DD_EXS_PCIe0_RIS_BIT		(1 << 6)	/**< Bit Location of INTSTAT[6] on INTSTAT */
#define D_DD_EXS_SD4_FIS_BIT		(1 << 5)	/**< Bit Location of INTSTAT[5] on INTSTAT */
#define D_DD_EXS_SD4_RIS_BIT		(1 << 4)	/**< Bit Location of INTSTAT[4] on INTSTAT */
#define D_DD_EXS_SD3_FIS_BIT		(1 << 3)	/**< Bit Location of INTSTAT[3] on INTSTAT */
#define D_DD_EXS_SD3_RIS_BIT		(1 << 2)	/**< Bit Location of INTSTAT[2] on INTSTAT */
#define D_DD_EXS_SD2_FIS_BIT		(1 << 1)	/**< Bit Location of INTSTAT[1] on INTSTAT */
#define D_DD_EXS_SD2_RIS_BIT		(1 << 0)	/**< Bit Location of INTSTAT[0] on INTSTAT */

// CD INT MASK
#define D_DD_EXS_USB3ID_FIM_BIT		(1 << 17)	/**< Bit Location of INTMSK[17] on INTMSK */
#define D_DD_EXS_USB3ID_RIM_BIT		(1 << 16)	/**< Bit Location of INTMSK[16] on INTMSK */
#define D_DD_EXS_USB3VBUS_FIM_BIT	(1 << 15)	/**< Bit Location of INTMSK[15] on INTMSK */
#define D_DD_EXS_USB3VBUS_RIM_BIT	(1 << 14)	/**< Bit Location of INTMSK[14] on INTMSK */
#define D_DD_EXS_USB2ID_FIM_BIT		(1 << 13)	/**< Bit Location of INTMSK[13] on INTMSK */
#define D_DD_EXS_USB2ID_RIM_BIT		(1 << 12)	/**< Bit Location of INTMSK[12] on INTMSK */
#define D_DD_EXS_USB2VBUS_FIM_BIT	(1 << 11)	/**< Bit Location of INTMSK[11] on INTMSK */
#define D_DD_EXS_USB2VBUS_RIM_BIT	(1 << 10)	/**< Bit Location of INTMSK[10] on INTMSK */
#define D_DD_EXS_PCIe1_FIM_BIT		(1 << 9)	/**< Bit Location of INTMSK[9] on INTMSK */
#define D_DD_EXS_PCIe1_RIM_BIT		(1 << 8)	/**< Bit Location of INTMSK[8] on INTMSK */
#define D_DD_EXS_PCIe0_FIM_BIT		(1 << 7)	/**< Bit Location of INTMSK[7] on INTMSK */
#define D_DD_EXS_PCIe0_RIM_BIT		(1 << 6)	/**< Bit Location of INTMSK[6] on INTMSK */
#define D_DD_EXS_SD4_FIM_BIT		(1 << 5)	/**< Bit Location of INTMSK[5] on INTMSK */
#define D_DD_EXS_SD4_RIM_BIT		(1 << 4)	/**< Bit Location of INTMSK[4] on INTMSK */
#define D_DD_EXS_SD3_FIM_BIT		(1 << 3)	/**< Bit Location of INTMSK[3] on INTMSK */
#define D_DD_EXS_SD3_RIM_BIT		(1 << 2)	/**< Bit Location of INTMSK[2] on INTMSK */
#define D_DD_EXS_INTMSK_01_BIT		(1 << 1)	/**< Bit Location of INTMSK[1] on INTMSK  (Reserved) */
#define D_DD_EXS_INTMSK_00_BIT		(1 << 0)	/**< Bit Location of INTMSK[0] on INTMSK  (Reserved) */

// CD INT MONITOR
#define D_DD_EXS_USB3ID_SIGMON2_BIT		(1 << 24)	/**< Bit Location of SIGMON2[8] on SIGMON */
#define D_DD_EXS_USB3VBUS_SIGMON2_BIT	(1 << 23)	/**< Bit Location of SIGMON2[7] on SIGMON */
#define D_DD_EXS_USB2ID_SIGMON2_BIT		(1 << 22)	/**< Bit Location of SIGMON2[6] on SIGMON */
#define D_DD_EXS_USB2VBUS_SIGMON2_BIT	(1 << 21)	/**< Bit Location of SIGMON2[5] on SIGMON */
#define D_DD_EXS_PCIe1_SIGMON2_BIT		(1 << 20)	/**< Bit Location of SIGMON2[4] on SIGMON */
#define D_DD_EXS_PCIe0_SIGMON2_BIT		(1 << 19)	/**< Bit Location of SIGMON2[3] on SIGMON */
#define D_DD_EXS_SD4_SIGMON2_BIT		(1 << 18)	/**< Bit Location of SIGMON2[2] on SIGMON */
#define D_DD_EXS_SD3_SIGMON2_BIT		(1 << 17)	/**< Bit Location of SIGMON2[1] on SIGMON */
#define D_DD_EXS_SIGMON2_00_BIT			(1 << 16)	/**< Bit Location of SIGMON2[0] on SIGMON  (Reserved) */

#define D_DD_EXS_USB3ID_SIGMON_BIT		(1 << 08)	/**< Bit Location of SIGMON[8] on SIGMON */
#define D_DD_EXS_USB3VBUS_SIGMON_BIT	(1 << 07)	/**< Bit Location of SIGMON[7] on SIGMON */
#define D_DD_EXS_USB2ID_SIGMON_BIT		(1 << 06)	/**< Bit Location of SIGMON[6] on SIGMON */
#define D_DD_EXS_USB2VBUS_SIGMON_BIT	(1 << 05)	/**< Bit Location of SIGMON[5] on SIGMON */
#define D_DD_EXS_PCIe1_SIGMON_BIT		(1 << 04)	/**< Bit Location of SIGMON[4] on SIGMON */
#define D_DD_EXS_PCIe0_SIGMON_BIT		(1 << 03)	/**< Bit Location of SIGMON[3] on SIGMON */
#define D_DD_EXS_SD4_SIGMON_BIT			(1 << 02)	/**< Bit Location of SIGMON[2] on SIGMON */
#define D_DD_EXS_SD3_SIGMON_BIT			(1 << 01)	/**< Bit Location of SIGMON[1] on SIGMON */
#define D_DD_EXS_SIGMON_00_BIT			(1 << 00)	/**< Bit Location of SIGMON[0] on SIGMON  (Reserved) */

#define D_DD_EXS_SET_0					(0)			/**< SET_0 */
#define D_DD_EXS_SET_1					(1)			/**< SET_1 */

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

// XDMACLPCTRL
#define Dd_Exs_Get_XDMAC_LowPower_Ctl_CSYSREQ()				(IO_EXSTOP.XDMACLPCTRL.bit.CSYSREQ)						/**< Get status of transition requests to LowPower mode of XDMAC<br>
																													Values:
																													<ul><li>0:LowPower mode request 
																													<li>1:Normal mode request </ul> */

#define Dd_Exs_Get_XDMAC_LowPower_Ctl_CSYSACK()				(IO_EXSTOP.XDMACLPCTRL.bit.CSYSACK)						/**< Get acknowledge LowPower mode transition of XDMAC<br>
																													Values:
																													<ul><li>0:LowPower mode enabled 
																													<li>1:Normal mode enabled</ul> */

#define Dd_Exs_Get_XDMAC_LowPower_Ctl_CACTIVE()				(IO_EXSTOP.XDMACLPCTRL.bit.CACTIVE)						/**< Get the state of the internal clock XDMAC<br>
																													Values:
																													<ul><li>0:The bus clock not request 
																													<li>1:The bus clock request</ul> */

// SOFTRESET
#define Dd_Exs_Get_nf_rst_n_Soft_Reset()					(IO_EXSTOP.SOFTRESET.bit.NFRSTN)						/**< Get status of reset control of nf_rst_n terminal<br>
																													Values:
																													<ul><li>0:nf_rst_n terminal reset is released 
																													<li>1:nf_rst_n terminal reset is set</ul> */

#define Dd_Exs_Get_bch_rst_n_Soft_Reset()					(IO_EXSTOP.SOFTRESET.bit.NFRSTNBCH)						/**< Get status of reset control of bch_rst_n terminal<br>
																													Values:
																													<ul><li>0:bch_rst_n terminal reset is released 
																													<li>1:bch_rst_n terminal reset is set</ul> */

#define Dd_Exs_Get_reg_rst_n_Soft_Reset()					(IO_EXSTOP.SOFTRESET.bit.NFRSTNREG)						/**< Get status of reset control of reg_rst_n terminal<br>
																													Values:
																													<ul><li>0:reg_rst_n terminal reset is released 
																													<li>1:reg_rst_n terminal reset is set</ul> */

#define Dd_Exs_Get_NETSEC_Soft_Reset()						(IO_EXSTOP.SOFTRESET.bit.NETSECRST)						/**< Get status of reset control of NETSEC macro<br>
																													Values:
																													<ul><li>0:NETSEC macro reset is released 
																													<li>1:NETSEC macro reset is set</ul> */

#define Dd_Exs_Get_eMMC_Soft_Reset()						(IO_EXSTOP.SOFTRESET.bit.EMCRST)						/**< Get status of reset control of eMMC macro<br>
																													Values:
																													<ul><li>0:eMMC macro reset is released 
																													<li>1:eMMC macro reset is set</ul> */

#define Dd_Exs_Get_RELC_Soft_Reset()						(IO_EXSTOP.SOFTRESET.bit.RELCRST)						/**< Get status of reset control of RELC macro<br>
																													Values:
																													<ul><li>0:RELC macro reset is released 
																													<li>1:RELC macro reset is set</ul> */

// SOFTRESET2
#define Dd_Exs_Get_SD3_CD_Debounce_Soft_Reset()				(IO_EXSTOP.SOFTRESET2.bit.DEBRSTX_SD3)					/**< Get status of reset control of Card Debounce SD3 (UHS2)<br>
																													Values:
																													<ul><li>0:Card Debounce reset is released 
																													<li>1:Card Debounce reset is set</ul> */

#define Dd_Exs_Get_SD4_CD_Debounce_Soft_Reset()				(IO_EXSTOP.SOFTRESET2.bit.DEBRSTX_SD4)					/**< Get status of reset control of Card Debounce SD4 (UHS1 Card)<br>
																													Values:
																													<ul><li>0:Card Debounce reset is released 
																													<li>1:Card Debounce reset is set</ul> */

#define Dd_Exs_Get_PCIE0_CD_Debounce_Soft_Reset()			(IO_EXSTOP.SOFTRESET2.bit.DEBRSTX_PCIE0)				/**< Get status of reset control of Card Debounce PCIe0<br>
																													Values:
																													<ul><li>0:Card Debounce reset is released 
																													<li>1:Card Debounce reset is set</ul> */

#define Dd_Exs_Get_PCIE1_CD_Debounce_Soft_Reset()			(IO_EXSTOP.SOFTRESET2.bit.DEBRSTX_PCIE1)				/**< Get status of reset control of Card Debounce PCIe1<br>
																													Values:
																													<ul><li>0:Card Debounce reset is released 
																													<li>1:Card Debounce reset is set</ul> */

#define Dd_Exs_Get_USB2VBUS_CD_Debounce_Soft_Reset()		(IO_EXSTOP.SOFTRESET2.bit.DEBRSTX_USB2VBUS)				/**< Get status of reset control of Card Debounce USB2 VBUS<br>
																													Values:
																													<ul><li>0:Card Debounce reset is released 
																													<li>1:Card Debounce reset is set</ul> */

#define Dd_Exs_Get_USB2ID_CD_Debounce_Soft_Reset()			(IO_EXSTOP.SOFTRESET2.bit.DEBRSTX_USB2ID)				/**< Get status of reset control of Card Debounce USB2 ID<br>
																													Values:
																													<ul><li>0:Card Debounce reset is released 
																													<li>1:Card Debounce reset is set</ul> */

#define Dd_Exs_Get_USB3VBUS_CD_Debounce_Soft_Reset()		(IO_EXSTOP.SOFTRESET2.bit.DEBRSTX_USB3VBUS)				/**< Get status of reset control of Card Debounce USB3 VBUS<br>
																													Values:
																													<ul><li>0:Card Debounce reset is released 
																													<li>1:Card Debounce reset is set</ul> */

#define Dd_Exs_Get_USB3ID_CD_Debounce_Soft_Reset()			(IO_EXSTOP.SOFTRESET2.bit.DEBRSTX_USB3ID)				/**< Get status of reset control of Card Debounce USB3 ID<br>
																													Values:
																													<ul><li>0:Card Debounce reset is released 
																													<li>1:Card Debounce reset is set</ul> */

// INTMSK
#define Dd_Exs_Get_INTMSK_SD3_RIM()							(IO_EXSTOP.INTMSK.bit.INTMSK02)							/**< Get SD3( UHS2 ) Detect INT Mask(Rising Edge)<br>
																													Values:
																													<ul><li>0:The interrupt signal by the SD3( UHS2 ) detection rising edge detection factor is output
																													<li>1:The interrupt signal by the SD3( UHS2 ) detection rising edge detection factor is mask</ul> */

#define Dd_Exs_Get_INTMSK_SD3_FIM()							(IO_EXSTOP.INTMSK.bit.INTMSK03)							/**< Get SD3( UHS2 ) Detect INT Mask(Falling Edge)<br>
																													Values:
																													<ul><li>0:The interrupt signal by the SD3( UHS2 ) detection falling edge detection factor is output
																													<li>1:The interrupt signal by the SD3( UHS2 ) detection falling edge detection factor is mask</ul> */

#define Dd_Exs_Get_INTMSK_SD4_RIM()							(IO_EXSTOP.INTMSK.bit.INTMSK04)							/**< Get SD4( UHS1 Card ) Detect INT Mask(Rising Edge)<br>
																													Values:
																													<ul><li>0:The interrupt signal by the SD4( UHS1 Card ) detection rising edge detection factor is output
																													<li>1:The interrupt signal by the SD4( UHS1 Card ) detection rising edge detection factor is mask</ul> */

#define Dd_Exs_Get_INTMSK_SD4_FIM()							(IO_EXSTOP.INTMSK.bit.INTMSK05)							/**< Get SD4( UHS1 Card ) Detect INT Mask(Falling Edge)<br>
																													Values:
																													<ul><li>0:The interrupt signal by the SD4( UHS1 Card ) detection falling edge detection factor is output
																													<li>1:The interrupt signal by the SD4( UHS1 Card ) detection falling edge detection factor is mask</ul> */

#define Dd_Exs_Get_INTMSK_PCIe0_RIM()						(IO_EXSTOP.INTMSK.bit.INTMSK06)							/**< Get PCIe0 Detect INT Mask(Rising Edge)<br>
																													Values:
																													<ul><li>0:The interrupt signal by the PCIe0 detection rising edge detection factor is output
																													<li>1:The interrupt signal by the PCIe0 detection rising edge detection factor is mask</ul> */

#define Dd_Exs_Get_INTMSK_PCIe0_FIM()						(IO_EXSTOP.INTMSK.bit.INTMSK07)							/**< Get PCIe0 Detect INT Mask(Falling Edge)<br>
																													Values:
																													<ul><li>0:The interrupt signal by the PCIe0 detection falling edge detection factor is output
																													<li>1:The interrupt signal by the PCIe0 detection falling edge detection factor is mask</ul> */

#define Dd_Exs_Get_INTMSK_PCIe1_RIM()						(IO_EXSTOP.INTMSK.bit.INTMSK08)							/**< Get PCIe1 Detect INT Mask(Rising Edge)<br>
																													Values:
																													<ul><li>0:The interrupt signal by the PCIe1 detection rising edge detection factor is output
																													<li>1:The interrupt signal by the PCIe1 detection rising edge detection factor is mask</ul> */

#define Dd_Exs_Get_INTMSK_PCIe1_FIM()						(IO_EXSTOP.INTMSK.bit.INTMSK09)							/**< Get PCIe1 Detect INT Mask(Falling Edge)<br>
																													Values:
																													<ul><li>0:The interrupt signal by the PCIe1 detection falling edge detection factor is output
																													<li>1:The interrupt signal by the PCIe1 detection falling edge detection factor is mask</ul> */

#define Dd_Exs_Get_INTMSK_USB2VBUS_RIM()					(IO_EXSTOP.INTMSK.bit.INTMSK10)							/**< Get USB2 VBUS Detect INT Mask(Rising Edge)<br>
																													Values:
																													<ul><li>0:The interrupt signal by the USB2 VBUS detection rising edge detection factor is output
																													<li>1:The interrupt signal by the USB2 VBUS detection rising edge detection factor is mask</ul> */

#define Dd_Exs_Get_INTMSK_USB2VBUS_FIM()					(IO_EXSTOP.INTMSK.bit.INTMSK11)							/**< Get USB2 VBUS Detect INT Mask(Falling Edge)<br>
																													Values:
																													<ul><li>0:The interrupt signal by the USB2 VBUS detection falling edge detection factor is output
																													<li>1:The interrupt signal by the USB2 VBUS detection falling edge detection factor is mask</ul> */

#define Dd_Exs_Get_INTMSK_USB2ID_RIM()						(IO_EXSTOP.INTMSK.bit.INTMSK12)							/**< Get USB2 ID Detect INT Mask(Rising Edge)<br>
																													Values:
																													<ul><li>0:The interrupt signal by the USB2 ID detection rising edge detection factor is output
																													<li>1:The interrupt signal by the USB2 ID detection rising edge detection factor is mask</ul> */

#define Dd_Exs_Get_INTMSK_USB2ID_FIM()						(IO_EXSTOP.INTMSK.bit.INTMSK13)							/**< Get USB2 ID Detect INT Mask(Falling Edge)<br>
																													Values:
																													<ul><li>0:The interrupt signal by the USB2 ID detection falling edge detection factor is output
																													<li>1:The interrupt signal by the USB2 ID detection falling edge detection factor is mask</ul> */

#define Dd_Exs_Get_INTMSK_USB3VBUS_RIM()					(IO_EXSTOP.INTMSK.bit.INTMSK14)							/**< Get USB3 VBUS Detect INT Mask(Rising Edge)<br>
																													Values:
																													<ul><li>0:The interrupt signal by the USB3 VBUS detection rising edge detection factor is output
																													<li>1:The interrupt signal by the USB3 VBUS detection rising edge detection factor is mask</ul> */

#define Dd_Exs_Get_INTMSK_USB3VBUS_FIM()					(IO_EXSTOP.INTMSK.bit.INTMSK15)							/**< Get USB3 VBUS Detect INT Mask(Falling Edge)<br>
																													Values:
																													<ul><li>0:The interrupt signal by the USB3 VBUS detection falling edge detection factor is output
																													<li>1:The interrupt signal by the USB3 VBUS detection falling edge detection factor is mask</ul> */

#define Dd_Exs_Get_INTMSK_USB3ID_RIM()						(IO_EXSTOP.INTMSK.bit.INTMSK16)							/**< Get USB3 ID Detect INT Mask(Rising Edge)<br>
																													Values:
																													<ul><li>0:The interrupt signal by the USB3 ID detection rising edge detection factor is output
																													<li>1:The interrupt signal by the USB3 ID detection rising edge detection factor is mask</ul> */

#define Dd_Exs_Get_INTMSK_USB3ID_FIM()						(IO_EXSTOP.INTMSK.bit.INTMSK17)							/**< Get USB3 ID Detect INT Mask(Falling Edge)<br>
																													Values:
																													<ul><li>0:The interrupt signal by the USB3 ID detection falling edge detection factor factor is output
																													<li>1:The interrupt signal by the USB3 ID detection falling edge detection factor is mask</ul> */

// SIGMON
#define Dd_Exs_Get_SIGMON_SD3_CD()							(IO_EXSTOP.SIGMON.bit.SIGMON_SD3)						/**< Get SD3( UHS2 ) Detect Monitor for after the debounce */
#define Dd_Exs_Get_SIGMON_SD4_CD()							(IO_EXSTOP.SIGMON.bit.SIGMON_SD4)						/**< Get SD4( UHS1 Card ) Detect Monitor for after the debounce */
#define Dd_Exs_Get_SIGMON_PCIE0_CD()						(IO_EXSTOP.SIGMON.bit.SIGMON_PCIE0)						/**< Get PCIE0 Detect Monitor for after the debounce */
#define Dd_Exs_Get_SIGMON_PCIE1_CD()						(IO_EXSTOP.SIGMON.bit.SIGMON_PCIE1)						/**< Get PCIE1 Detect Monitor for after the debounce */
#define Dd_Exs_Get_SIGMON_USB2VBUS_CD()						(IO_EXSTOP.SIGMON.bit.SIGMON_USB2VBUS)					/**< Get USB2 VBUS Detect Monitor for after the debounce */
#define Dd_Exs_Get_SIGMON_USB2ID_CD()						(IO_EXSTOP.SIGMON.bit.SIGMON_USB2ID)					/**< Get USB2 ID Detect Monitor for after the debounce */
#define Dd_Exs_Get_SIGMON_USB3VBUS_CD()						(IO_EXSTOP.SIGMON.bit.SIGMON_USB3VBUS)					/**< Get USB3 VBUS Detect Monitor for after the debounce */
#define Dd_Exs_Get_SIGMON_USB3ID_CD()						(IO_EXSTOP.SIGMON.bit.SIGMON_USB3ID)					/**< Get USB3 ID Detect Monitor for after the debounce */

#define Dd_Exs_Get_SIGMON2_SD3_CD()							(IO_EXSTOP.SIGMON.bit.SIGMON2_SD3)						/**< Get SD3( UHS2 ) Detect Monitor for chip terminal status */
#define Dd_Exs_Get_SIGMON2_SD4_CD()							(IO_EXSTOP.SIGMON.bit.SIGMON2_SD4)						/**< Get SD4( UHS1 Card ) Detect Monitor for chip terminal status */
#define Dd_Exs_Get_SIGMON2_PCIE0_CD()						(IO_EXSTOP.SIGMON.bit.SIGMON2_PCIE0)					/**< Get PCIE0 Detect Monitor for chip terminal status */
#define Dd_Exs_Get_SIGMON2_PCIE1_CD()						(IO_EXSTOP.SIGMON.bit.SIGMON2_PCIE1)					/**< Get PCIE1 Detect Monitor for chip terminal status */
#define Dd_Exs_Get_SIGMON2_USB2VBUS_CD()					(IO_EXSTOP.SIGMON.bit.SIGMON2_USB2VBUS)					/**< Get USB2 VBUS Detect Monitor for chip terminal status */
#define Dd_Exs_Get_SIGMON2_USB2ID_CD()						(IO_EXSTOP.SIGMON.bit.SIGMON2_USB2ID)					/**< Get USB2 ID Detect Monitor for chip terminal status */
#define Dd_Exs_Get_SIGMON2_USB3VBUS_CD()					(IO_EXSTOP.SIGMON.bit.SIGMON2_USB3VBUS)					/**< Get USB3 VBUS Detect Monitor for chip terminal status */
#define Dd_Exs_Get_SIGMON2_USB3ID_CD()						(IO_EXSTOP.SIGMON.bit.SIGMON2_USB3ID)					/**< Get USB3 ID Detect Monitor for chip terminal status */

// DEBEN
#define Dd_Exs_Get_DEBEN_SD3_EN()							(IO_EXSTOP.DEBEN.bit.DEBEN_SD3)							/**< Get SD3( UHS2 ) Debounce ON/OFF<br>
																													Values:
																													<ul><li>0:The debounce of the SD3( UHS2 ) signal is turned off
																													<li>1:The debounce of the SD3( UHS2 ) signal is turned on</ul> */

#define Dd_Exs_Get_DEBEN_SD4_EN()							(IO_EXSTOP.DEBEN.bit.DEBEN_SD4)							/**< Get SD4( UHS1 Card ) Debounce ON/OFF<br>
																													Values:
																													<ul><li>0:The debounce of the SD4( UHS1 Card ) signal is turned off
																													<li>1:The debounce of the SD4( UHS1 Card ) signal is turned on</ul> */

#define Dd_Exs_Get_DEBEN_PCIE0_EN()							(IO_EXSTOP.DEBEN.bit.DEBEN_PCIE0)						/**< Get PCIe0 Debounce ON/OFF<br>
																													Values:
																													<ul><li>0:The debounce of the PCIe0 signal is turned off
																													<li>1:The debounce of the PCIe0 signal is turned on</ul> */

#define Dd_Exs_Get_DEBEN_PCIE1_EN()							(IO_EXSTOP.DEBEN.bit.DEBEN_PCIE1)						/**< Get PCIe1 Debounce ON/OFF<br>
																													Values:
																													<ul><li>0:The debounce of the PCIe1 signal is turned off
																													<li>1:The debounce of the PCIe1 signal is turned on</ul> */

#define Dd_Exs_Get_DEBEN_USB2VBUS_EN()						(IO_EXSTOP.DEBEN.bit.DEBEN_USB2VBUS)					/**< Get USB2 VBUS Debounce ON/OFF<br>
																													Values:
																													<ul><li>0:The debounce of the USB2 VBUS signal is turned off
																													<li>1:The debounce of the USB2 VBUS signal is turned on</ul> */

#define Dd_Exs_Get_DEBEN_USB2ID_EN()						(IO_EXSTOP.DEBEN.bit.DEBEN_USB2ID)						/**< Get USB2 ID Debounce ON/OFF<br>
																													Values:
																													<ul><li>0:The debounce of the USB2 ID signal is turned off
																													<li>1:The debounce of the USB2 ID signal is turned on</ul> */

#define Dd_Exs_Get_DEBEN_USB3VBUS_EN()						(IO_EXSTOP.DEBEN.bit.DEBEN_USB3VBUS)					/**< Get USB3 VBUS Debounce ON/OFF<br>
																													Values:
																													<ul><li>0:The debounce of the USB3 VBUS signal is turned off
																													<li>1:The debounce of the USB3 VBUS signal is turned on</ul> */

#define Dd_Exs_Get_DEBEN_USB3ID_EN()						(IO_EXSTOP.DEBEN.bit.DEBEN_USB3ID)						/**< Get USB3 ID Debounce ON/OFF<br>
																													Values:
																													<ul><li>0:The debounce of the USB3 ID signal is turned off
																													<li>1:The debounce of the USB3 ID signal is turned on</ul> */

#define Dd_Exs_Get_DEBEN_SD3DETIM()							(IO_EXSTOP.DEBEN.bit.SD3DETIM)							/**< Get SD-Card ch3 Debounce time<br>
																													Values:
																													<ul><li>00:APBCLK 131072 cycles
																													<li>01:APBCLK 65536 cycles (131072 cycles /2)
																													<li>10:APBCLK 262144 cycles (131072 cycles *2) 
																													<li>11:APBCLK 524288 cycles (131072 cycles *4)</ul> */

#define Dd_Exs_Get_DEBEN_SD4DETIM()							(IO_EXSTOP.DEBEN.bit.SD4DETIM)							/**< Get SD-Card ch4 Debounce time<br>
																													Values:
																													<ul><li>00:APBCLK 131072 cycles
																													<li>01:APBCLK 65536 cycles (131072 cycles /2)
																													<li>10:APBCLK 262144 cycles (131072 cycles *2) 
																													<li>11:APBCLK 524288 cycles (131072 cycles *4)</ul> */

#define Dd_Exs_Get_DEBEN_PCIE0DETIM()						(IO_EXSTOP.DEBEN.bit.PCIE0DETIM)						/**< Get PCIe0 Debounce time<br>
																													Values:
																													<ul><li>00:APBCLK 131072 cycles
																													<li>01:APBCLK 65536 cycles (131072 cycles /2)
																													<li>10:APBCLK 262144 cycles (131072 cycles *2) 
																													<li>11:APBCLK 524288 cycles (131072 cycles *4)</ul> */

#define Dd_Exs_Get_DEBEN_PCIE1DETIM()						(IO_EXSTOP.DEBEN.bit.PCIE1DETIM)						/**< Get PCIe1 Debounce time<br>
																													Values:
																													<ul><li>00:APBCLK 131072 cycles
																													<li>01:APBCLK 65536 cycles (131072 cycles /2)
																													<li>10:APBCLK 262144 cycles (131072 cycles *2) 
																													<li>11:APBCLK 524288 cycles (131072 cycles *4)</ul> */

#define Dd_Exs_Get_DEBEN_USB2VBUSDETIM()					(IO_EXSTOP.DEBEN.bit.USB2VBUSDETIM)						/**< Get USB2 VBUS Debounce time<br>
																													Values:
																													<ul><li>00:APBCLK 131072 cycles
																													<li>01:APBCLK 65536 cycles (131072 cycles /2)
																													<li>10:APBCLK 262144 cycles (131072 cycles *2) 
																													<li>11:APBCLK 524288 cycles (131072 cycles *4)</ul> */

#define Dd_Exs_Get_DEBEN_USB2IDDETIM()						(IO_EXSTOP.DEBEN.bit.USB2IDDETIM)						/**< Get USB2 ID Debounce time<br>
																													Values:
																													<ul><li>00:APBCLK 131072 cycles
																													<li>01:APBCLK 65536 cycles (131072 cycles /2)
																													<li>10:APBCLK 262144 cycles (131072 cycles *2) 
																													<li>11:APBCLK 524288 cycles (131072 cycles *4)</ul> */

#define Dd_Exs_Get_DEBEN_USB3VBUSDETIM()					(IO_EXSTOP.DEBEN.bit.USB3VBUSDETIM)						/**< Get USB3 VBUS Debounce time<br>
																													Values:
																													<ul><li>00:APBCLK 131072 cycles
																													<li>01:APBCLK 65536 cycles (131072 cycles /2)
																													<li>10:APBCLK 262144 cycles (131072 cycles *2) 
																													<li>11:APBCLK 524288 cycles (131072 cycles *4)</ul> */

#define Dd_Exs_Get_DEBEN_USB3IDDETIM()						(IO_EXSTOP.DEBEN.bit.USB3IDDETIM)						/**< Get USB3 ID Debounce time<br>
																													Values:
																													<ul><li>00:APBCLK 131072 cycles
																													<li>01:APBCLK 65536 cycles (131072 cycles /2)
																													<li>10:APBCLK 262144 cycles (131072 cycles *2) 
																													<li>11:APBCLK 524288 cycles (131072 cycles *4)</ul> */

// CDINSMDCTL
#define Dd_Exs_Get_CDINS_SD3_MOD()							(IO_EXSTOP.CDINSMDCTL.bit.CDIMOD_SD3)					/**< Get the mode setting of the card detection SD3( UHS2 )<br>
																													Values:
																													<ul><li>0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the SD3( UHS2 ) signal 
																													<li>1:After exiting reset at debounce circuit, generates an interrupt by the state of the SD3( UHS2 ) signal</ul> */

#define Dd_Exs_Get_CDINS_SD4_MOD()							(IO_EXSTOP.CDINSMDCTL.bit.CDIMOD_SD4)					/**< Get the mode setting of the card detection SD4( UHS1 Card )<br>
																													Values:
																													<ul><li>0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the SD4( UHS1 Card ) signal 
																													<li>1:After exiting reset at debounce circuit, generates an interrupt by the state of the SD4( UHS1 Card ) signal</ul> */

#define Dd_Exs_Get_CDINS_PCIE0_MOD()						(IO_EXSTOP.CDINSMDCTL.bit.CDIMOD_PCIE0)					/**< Get the mode setting of the card detection PCIe0<br>
																													Values:
																													<ul><li>0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the PCIe0 signal 
																													<li>1:After exiting reset at debounce circuit, generates an interrupt by the state of the PCIe0 signal</ul> */

#define Dd_Exs_Get_CDINS_PCIE1_MOD()						(IO_EXSTOP.CDINSMDCTL.bit.CDIMOD_PCIE1)					/**< Get the mode setting of the card detection PCIe1<br>
																													Values:
																													<ul><li>0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the PCIe1 signal 
																													<li>1:After exiting reset at debounce circuit, generates an interrupt by the state of the PCIe1 signal</ul> */

#define Dd_Exs_Get_CDINS_USB2VBUS_MOD()						(IO_EXSTOP.CDINSMDCTL.bit.CDIMOD_USB2VBUS)				/**< Get the mode setting of the card detection USB2 VBUS<br>
																													Values:
																													<ul><li>0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the USB2 VBUS signal 
																													<li>1:After exiting reset at debounce circuit, generates an interrupt by the state of the USB2 VBUS signal</ul> */

#define Dd_Exs_Get_CDINS_USB2ID_MOD()						(IO_EXSTOP.CDINSMDCTL.bit.CDIMOD_USB2ID)				/**< Get the mode setting of the card detection USB2 ID<br>
																													Values:
																													<ul><li>0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the USB2 ID signal 
																													<li>1:After exiting reset at debounce circuit, generates an interrupt by the state of the USB2 ID signal</ul> */

#define Dd_Exs_Get_CDINS_USB3VBUS_MOD()						(IO_EXSTOP.CDINSMDCTL.bit.CDIMOD_USB3VBUS)				/**< Get the mode setting of the card detection USB3 VBUS<br>
																													Values:
																													<ul><li>0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the USB3 VBUS signal 
																													<li>1:After exiting reset at debounce circuit, generates an interrupt by the state of the USB3 VBUS signal</ul> */

#define Dd_Exs_Get_CDINS_USB3ID_MOD()						(IO_EXSTOP.CDINSMDCTL.bit.CDIMOD_USB3ID)				/**< Get the mode setting of the card detection USB3 ID<br>
																													Values:
																													<ul><li>0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the USB3 ID signal 
																													<li>1:After exiting reset at debounce circuit, generates an interrupt by the state of the USB3 ID signal</ul> */

// RLC_HPT_CTL
#define Dd_Exs_Get_RELC_HPROT_Read()						(IO_EXSTOP.RLC_HPT_CTL.bit.RERHPT)						/**< Get the RELC expanded data read side HPROT setting(Data/Opcode) */

#define Dd_Exs_Get_RELC_HPROT_Write()						(IO_EXSTOP.RLC_HPT_CTL.bit.REWHPT)						/**< Get the RELC expanded data write side HPROT setting(Data/Opcode) */

// RAM_PD
#define Dd_Exs_Get_RAM_PD_Ctrl_SD2()						(IO_EXSTOP.RAM_PD.bit.RAMPD_SD2)						/**< Get SD2( UHS1 Wifi ) macro PD signal output of RAMCELL with PD<br>
																													Values:
																													<ul><li>0:Power-down mode is released 
																													<li>1:Enters Power-down mode</ul> */

#define Dd_Exs_Get_RAM_PD_Ctrl_SD3()						(IO_EXSTOP.RAM_PD.bit.RAMPD_SD3)						/**< Get SD3( UHS2 ) macro PD signal output of RAMCELL with PD<br>
																													Values:
																													<ul><li>0:Power-down mode is released 
																													<li>1:Enters Power-down mode</ul> */

#define Dd_Exs_Get_RAM_PD_Ctrl_SD4()						(IO_EXSTOP.RAM_PD.bit.RAMPD_SD4)						/**< Get SD4( UHS1 Card ) macro PD signal output of RAMCELL with PD<br>
																													Values:
																													<ul><li>0:Power-down mode is released 
																													<li>1:Enters Power-down mode</ul> */

#define Dd_Exs_Get_RAM_PD_Ctrl_NF()							(IO_EXSTOP.RAM_PD.bit.RAMPD_NF)							/**< Get Nand Flash macro PD signal output of RAMCELL with PD<br>
																													Values:
																													<ul><li>0:Power-down mode is released 
																													<li>1:Enters Power-down mode</ul> */

#define Dd_Exs_Get_RAM_PD_Ctrl_RELC()						(IO_EXSTOP.RAM_PD.bit.RAMPD_RELC)						/**< Get RELC macro PD signal output of RAMCELL with PD<br>
																													Values:
																													<ul><li>0:Power-down mode is released 
																													<li>1:Enters Power-down mode</ul> */

#define Dd_Exs_Get_RAM_PD_Ctrl_NETSEC()						(IO_EXSTOP.RAM_PD.bit.RAMPD_NETSEC)						/**< Get NETSEC macro PD signal output of RAMCELL with PD<br>
																													Values:
																													<ul><li>0:Power-down mode is released 
																													<li>1:Enters Power-down mode</ul> */

#define Dd_Exs_Get_RAM_PD_Ctrl_USB2()						(IO_EXSTOP.RAM_PD.bit.RAMPD_USB2)						/**< Get USB2 macro PD signal output of RAMCELL with PD<br>
																													Values:
																													<ul><li>0:Power-down mode is released 
																													<li>1:Enters Power-down mode</ul> */

#define Dd_Exs_Get_RAM_PD_Ctrl_USB3()						(IO_EXSTOP.RAM_PD.bit.RAMPD_USB3)						/**< Get USB3 macro PD signal output of RAMCELL with PD<br>
																													Values:
																													<ul><li>0:Power-down mode is released 
																													<li>1:Enters Power-down mode</ul> */

#define Dd_Exs_Get_RAM_PD_Ctrl_PCIE0()						(IO_EXSTOP.RAM_PD.bit.RAMPD_PCIE0)						/**< Get PCIE0 macro PD signal output of RAMCELL with PD<br>
																													Values:
																													<ul><li>0:Power-down mode is released 
																													<li>1:Enters Power-down mode</ul> */

#define Dd_Exs_Get_RAM_PD_Ctrl_PCIE1()						(IO_EXSTOP.RAM_PD.bit.RAMPD_PCIE1)						/**< Get PCIE1 macro PD signal output of RAMCELL with PD<br>
																													Values:
																													<ul><li>0:Power-down mode is released 
																													<li>1:Enters Power-down mode</ul> */

#define Dd_Exs_Get_RAM_PD_Ctrl_EMMC()						(IO_EXSTOP.RAM_PD.bit.RAMPD_EMMC)						/**< Get eMMC macro PD signal output of RAMCELL with PD<br>
																													Values:
																													<ul><li>0:Power-down mode is released 
																													<li>1:Enters Power-down mode</ul> */

// BUSCKSTP
#define Dd_Exs_Get_NF_BUS_CLOCK_Stop_Ctrl()					(IO_EXSTOP.BUSCKSTP.bit.NF_CKSTP)						/**< Get the output a signal to stop control of the Nand Flash BUS CLOCK macro<br>
																													Values:
																													<ul><li>0:Output Nand Flash BUS CLOCK 
																													<li>1:Stop Nand Flash BUS CLOCK</ul> */

#define Dd_Exs_Get_PCIE0_BUS_CLOCK_Stop_Ctrl()				(IO_EXSTOP.BUSCKSTP.bit.PCIE0_CKSTP)					/**< Get the output a signal to stop control of the PCIE0 BUS CLOCK macro<br>
																													Values:
																													<ul><li>0:Output PCIE0 BUS CLOCK 
																													<li>1:Stop PCIE0 BUS CLOCK</ul> */

#define Dd_Exs_Get_PCIE1_BUS_CLOCK_Stop_Ctrl()				(IO_EXSTOP.BUSCKSTP.bit.PCIE1_CKSTP)					/**< Get the output a signal to stop control of the PCIE1 BUS CLOCK macro<br>
																													Values:
																													<ul><li>0:Output PCIE1 BUS CLOCK 
																													<li>1:Stop PCIE1 BUS CLOCK</ul> */

#define Dd_Exs_Get_XDMAC_BUS_CLOCK_Stop_Ctrl()				(IO_EXSTOP.BUSCKSTP.bit.XDMAC_CKSTP)					/**< Get the output a signal to stop control of the XDMAC BUS CLOCK macro<br>
																													Values:
																													<ul><li>0:Output XDMAC BUS CLOCK 
																													<li>1:Stop XDMAC BUS CLOCK</ul> */

#define Dd_Exs_Get_HDMAC_BUS_CLOCK_Stop_Ctrl()				(IO_EXSTOP.BUSCKSTP.bit.HDMAC_CKSTP)					/**< Get the output a signal to stop control of the HDMAC BUS CLOCK macro<br>
																													Values:
																													<ul><li>0:Output HDMAC BUS CLOCK 
																													<li>1:Stop HDMAC BUS CLOCK</ul> */

#define Dd_Exs_Get_EMMC_BUS_CLOCK_Stop_Ctrl()				(IO_EXSTOP.BUSCKSTP.bit.EMMC_CKSTP)						/**< Get the output a signal to stop control of the eMMC BUS CLOCK macro<br>
																													Values:
																													<ul><li>0:Output eMMC BUS CLOCK 
																													<li>1:Stop eMMC BUS CLOCK</ul> */

#define Dd_Exs_Get_SD2_BUS_CLOCK_Stop_Ctrl()				(IO_EXSTOP.BUSCKSTP.bit.SD2_CKSTP)						/**< Get the output a signal to stop control of the SD2 (Wifi) BUS CLOCK macro<br>
																													Values:
																													<ul><li>0:Output SD2 (Wifi) BUS CLOCK 
																													<li>1:Stop SD2 (Wifi) BUS CLOCK</ul> */

#define Dd_Exs_Get_SD3_BUS_CLOCK_Stop_Ctrl()				(IO_EXSTOP.BUSCKSTP.bit.SD3_CKSTP)						/**< Get the output a signal to stop control of the SD3 (UHS2) BUS CLOCK macro<br>
																													Values:
																													<ul><li>0:Output SD3 (UHS2) BUS CLOCK 
																													<li>1:Stop SD3 (UHS2) BUS CLOCK</ul> */

#define Dd_Exs_Get_SD4_BUS_CLOCK_Stop_Ctrl()				(IO_EXSTOP.BUSCKSTP.bit.SD4_CKSTP)						/**< Get the output a signal to stop control of the SD4 (UHS1 Card) BUS CLOCK macro<br>
																													Values:
																													<ul><li>0:Output SD4 (UHS1 Card) BUS CLOCK 
																													<li>1:Stop SD4 (UHS1 Card) BUS CLOCK</ul> */

#define Dd_Exs_Get_NETSEC_BUS_CLOCK_Stop_Ctrl()				(IO_EXSTOP.BUSCKSTP.bit.NETSEC_CKSTP)					/**< Get the output a signal to stop control of the NETSEC BUS CLOCK macro<br>
																													Values:
																													<ul><li>0:Output NETSEC BUS CLOCK 
																													<li>1:Stop NETSEC BUS CLOCK</ul> */

#define Dd_Exs_Get_RELC_BUS_CLOCK_Stop_Ctrl()				(IO_EXSTOP.BUSCKSTP.bit.RELC_CKSTP)						/**< Get the output a signal to stop control of the RELC BUS CLOCK macro<br>
																													Values:
																													<ul><li>0:Output RELC BUS CLOCK 
																													<li>1:Stop RELC BUS CLOCK</ul> */

// NSEC_CKSTP
#define Dd_Exs_Get_NETSEC_PTPCLK_Stop_Ctrl_NETSEC_PTPSTP()	(IO_EXSTOP.NSEC_CKSTP.bit.NETSECPTPSTP)					/**< Get the output a signal to stop control of the PTP_CLK NETSEC macro<br>
																													Values:
																													<ul><li>0:Output PTP_CLK
																													<li>1:Stop PTP_CLK</ul> */

// EMMC_CKSTP
#define Dd_Exs_Get_EMMC_CKSTP_EMCLKSTP()					(IO_EXSTOP.EMMC_CKSTP.bit.EMCLKSTP)						/**< Get the output a signal to stop control of the MMC_BCLK_I<br>
																													Values:
																													<ul><li>0:Output MMC_BCLK_I
																													<li>1:Stop MMC_BCLK_I</ul> */

// EM_DRVSTR
#define Dd_Exs_Get_EM_DRVSTR()								(IO_EXSTOP.EM_DRVSTR.bit.EM_DSEL)						/**< Get EM_DSEL */

// EM_PV_DTIMEC
#define Dd_Exs_Get_EM_PV_DTIMEC()							(IO_EXSTOP.EM_PV_DTIMEC.bit.PV_DTIMEC)					/**< Get PV_DTIMEC */

// EM_PV_AMPBL
#define Dd_Exs_Get_EM_PV_AMPBL()							(IO_EXSTOP.EM_PV_AMPBL.bit.PV_AMPBL)					/**< Get PV_AMPBL */

// EM_CR_SLOTTYPE
#define Dd_Exs_Get_EM_CR_SLOTTYPE()							(IO_EXSTOP.EM_CR_SLOTTYPE.bit.CR_SLOTTYPE)				/**< Get CR_SLOTTYPE */

// EM_CR_BCLKFREQ
#define Dd_Exs_Get_EM_CR_BCLKFREQ()							(IO_EXSTOP.EM_CR_BCLKFREQ.bit.CR_BCLKFREQ)				/**< Get CR_BCLKFREQ */

// EM_CDDET
#define Dd_Exs_Get_EM_CDDET_EMCD()							(IO_EXSTOP.EM_CDDET.bit.EMCD)							/**< Get EMCD */

#define Dd_Exs_Get_EM_CDDET_EMPHYLK()						(IO_EXSTOP.EM_CDDET.bit.EMPHYLK)						/**< Get EMPHYLK */

// USB2_CKCTL
#define Dd_Exs_Get_USB2_CKCTL_HDC_HCLKEN()					(IO_EXSTOP.USB2_CKCTL.bit.HDC_HCLKEN)					/**< Get the state of USB2.0 core(IP) Device clock<br>
																													Values:
																													<ul><li>0:clock stop
																													<li>1:clock run</ul> */

#define Dd_Exs_Get_USB2_CKCTL_HO_HCLKEN()					(IO_EXSTOP.USB2_CKCTL.bit.HO_HCLKEN)					/**< Get the state of USB2.0 core(IP) Host clock<br>
																													Values:
																													<ul><li>0:clock stop
																													<li>1:clock run</ul> */

// USB2_RCTL
#define Dd_Exs_Get_USB2_RCTL_HDC_SRST()						(IO_EXSTOP.USB2_RCTL.bit.HDC_SRST)						/**< Get the reset state of USB2.0 core(IP) Device clock<br>
																													Values:
																													<ul><li>0:reset is set.
																													<li>1:reset is released.</ul> */

#define Dd_Exs_Get_USB2_RCTL_HO_SRST()						(IO_EXSTOP.USB2_RCTL.bit.HO_SRST)						/**< Get HO_SRST */

// USB2_ANPD
#define Dd_Exs_Get_USB2_ANPD_ANPD()							(IO_EXSTOP.USB2_ANPD.bit.ANPD)							/**< Get ANPD */

// USB2_HFSEL
#define Dd_Exs_Get_USB2_HFSEL_HF_SEL()						(IO_EXSTOP.USB2_HFSEL.bit.HF_SEL)						/**< Get HF_SEL */

// USB2_FSEL
#define Dd_Exs_Get_USB2_FSEL_FSEL()							(IO_EXSTOP.USB2_FSEL.bit.FSEL)							/**< Get FSEL */

#define Dd_Exs_Get_USB2_FSEL_PLLBTUNE()						(IO_EXSTOP.USB2_FSEL.bit.PLLBTUNE)						/**< Get PLLBTUNE */

// USB2_EHCI_SB
#define Dd_Exs_Get_USB2_EHCI_SB_EHCI_XFER_CNT_O()			(IO_EXSTOP.USB2_EHCI_SB.bit.EHCI_XFER_CNT_O)			/**< Get EHCI_XFER_CNT_O */

#define Dd_Exs_Get_USB2_EHCI_SB_EHCI_XFER_PRDC_O()			(IO_EXSTOP.USB2_EHCI_SB.bit.EHCI_XFER_PRDC_O)			/**< Get EHCI_XFER_PRDC_O */

#define Dd_Exs_Get_USB2_EHCI_SB_EHCI_EHCI_BUFACC_O()		(IO_EXSTOP.USB2_EHCI_SB.bit.EHCI_BUFACC_O)				/**< Get EHCI_BUFACC_O */

#define Dd_Exs_Get_USB2_EHCI_SB_EHCI_MCU_INC_FRDX_O()		(IO_EXSTOP.USB2_EHCI_SB.bit.MCU_INC_FRDX_O)				/**< Get MCU_INC_FRDX_O */

#define Dd_Exs_Get_USB2_EHCI_SB_EHCI_EOR_RUNSTOP_O()		(IO_EXSTOP.USB2_EHCI_SB.bit.EOR_RUNSTOP_O)				/**< Get EOR_RUNSTOP_O */

#define Dd_Exs_Get_USB2_EHCI_SB_EHCI_EHCI_USBSTS_O()		(IO_EXSTOP.USB2_EHCI_SB.bit.EHCI_USBSTS_O)				/**< Get EHCI_USBSTS_O */

// USB2_EHCI_STRAP
#define Dd_Exs_Get_USB2_EHCI_STRAP_SS_FADJ_HOST_I()			(IO_EXSTOP.USB2_EHCI_STRAP.bit.SS_FADJ_HOST_I)			/**< Get SS_FADJ_HOST_I */

#define Dd_Exs_Get_USB2_EHCI_STRAP_SS_FLADJ_VAL_I()			(IO_EXSTOP.USB2_EHCI_STRAP.bit.SS_FLADJ_VAL_I)			/**< Get SS_FLADJ_VAL_I */

// USB2_OHCI_IF
#define Dd_Exs_Get_USB2_OHCI_IF_OHCI_CLKSEL_I()				(IO_EXSTOP.USB2_OHCI_IF.bit.OHCI_CLKSEL_I)				/**< Get OHCI_CLKSEL_I */

#define Dd_Exs_Get_USB2_OHCI_IF_UTMI_SUSPEND_O_N()			(IO_EXSTOP.USB2_OHCI_IF.bit.UTMI_SUSPEND_O_N)			/**< Get UTMI_SUSPEND_O_N */

#define Dd_Exs_Get_USB2_OHCI_IF_OHCI_GSUSPEND_O()			(IO_EXSTOP.USB2_OHCI_IF.bit.OHCI_GSUSPEND_O)			/**< Get OHCI_GSUSPEND_O */

#define Dd_Exs_Get_USB2_OHCI_IF_OHCI_RWE_O()				(IO_EXSTOP.USB2_OHCI_IF.bit.OHCI_RWE_O)					/**< Get OHCI_RWE_O */

#define Dd_Exs_Get_USB2_OHCI_IF_OHCI_RMTWKP_O()				(IO_EXSTOP.USB2_OHCI_IF.bit.OHCI_RMTWKP_O)				/**< Get OHCI_RMTWKP_O */

#define Dd_Exs_Get_USB2_OHCI_IF_OHCI_DRWE_O()				(IO_EXSTOP.USB2_OHCI_IF.bit.OHCI_DRWE_O)				/**< Get OHCI_DRWE_O */

#define Dd_Exs_Get_USB2_OHCI_IF_OHCI_CCS_O()				(IO_EXSTOP.USB2_OHCI_IF.bit.OHCI_CCS_O)					/**< Get OHCI_CCS_O */

#define Dd_Exs_Get_USB2_OHCI_IF_OHCI_SOF_O_N()				(IO_EXSTOP.USB2_OHCI_IF.bit.OHCI_SOF_O_N)				/**< Get OHCI_SOF_O_N */

#define Dd_Exs_Get_USB2_OHCI_IF_OHCI_BUFACC_O()				(IO_EXSTOP.USB2_OHCI_IF.bit.OHCI_BUFACC_O)				/**< Get OHCI_BUFACC_O */

// USB2_AHB_SLVIF
#define Dd_Exs_Get_USB2_AHB_SLVIF_REG_RDY_O()				(IO_EXSTOP.USB2_AHB_SLVIF.bit.REG_RDY_O)				/**< Get REG_RDY_O */

// USB2_LMODSET
#define Dd_Exs_Get_USB2_LMODSET_SS_ENA_INCR4_I()			(IO_EXSTOP.USB2_LMODSET.bit.SS_ENA_INCR4_I)				/**< Get SS_ENA_INCR4_I */

#define Dd_Exs_Get_USB2_LMODSET_SS_ENA_INCR8_I()			(IO_EXSTOP.USB2_LMODSET.bit.SS_ENA_INCR8_I)				/**< Get SS_ENA_INCR8_I */

#define Dd_Exs_Get_USB2_LMODSET_SS_ENA_INCR16_I()			(IO_EXSTOP.USB2_LMODSET.bit.SS_ENA_INCR16_I)			/**< Get SS_ENA_INCR16_I */

#define Dd_Exs_Get_USB2_LMODSET_SS_ENA_INC_AL_I()			(IO_EXSTOP.USB2_LMODSET.bit.SS_ENA_INC_AL_I)			/**< Get SS_ENA_INC_AL_I */

#define Dd_Exs_Get_USB2_LMODSET_SS_EHCI64B_EN_I()			(IO_EXSTOP.USB2_LMODSET.bit.SS_EHCI64B_EN_I)			/**< Get SS_EHCI64B_EN_I */

// USB2_PMODSET
#define Dd_Exs_Get_USB2_PMODSET_RPDPEN_I()					(IO_EXSTOP.USB2_PMODSET.bit.RPDPEN_I)					/**< Get RPDPEN_I */

#define Dd_Exs_Get_USB2_PMODSET_RPDMEN_I()					(IO_EXSTOP.USB2_PMODSET.bit.RPDMEN_I)					/**< Get RPDMEN_I */

// USB2_HPT_CTL
#define Dd_Exs_Get_USB2_HPT_CTL_USB2_EHPT()					(IO_EXSTOP.USB2_HPT_CTL.bit.USB2_EHPT)					/**< Get USB2_EHPT */

#define Dd_Exs_Get_USB2_HPT_CTL_USB2_OHPT()					(IO_EXSTOP.USB2_HPT_CTL.bit.USB2_OHPT)					/**< Get USB2_OHPT */

// USB2_IDVBUSCTL
#define Dd_Exs_Get_USB2_IDVBUSCTL_USB2_IDPU_VAL()			(IO_EXSTOP.USB2_IDVBUSCTL.bit.USB2_IDPU_VAL)			/**< Get USB2_IDPU_VAL */

#define Dd_Exs_Get_USB2_IDVBUSCTL_USB2_IDPU_SEL()			(IO_EXSTOP.USB2_IDVBUSCTL.bit.USB2_IDPU_SEL)			/**< Get USB2_IDPU_SEL */

#define Dd_Exs_Get_USB2_IDVBUSCTL_USB2_ID_SEL()				(IO_EXSTOP.USB2_IDVBUSCTL.bit.USB2_ID_SEL)				/**< Get USB2_ID_SEL */

#define Dd_Exs_Get_USB2_IDVBUSCTL_USB2_ID_VAL()				(IO_EXSTOP.USB2_IDVBUSCTL.bit.USB2_ID_VAL)				/**< Get USB2_ID_VAL */

#define Dd_Exs_Get_USB2_IDVBUSCTL_USB2_VB_SEL()				(IO_EXSTOP.USB2_IDVBUSCTL.bit.USB2_VB_SEL)				/**< Get USB2_VB_SEL */

#define Dd_Exs_Get_USB2_IDVBUSCTL_USB2_VB_VAL()				(IO_EXSTOP.USB2_IDVBUSCTL.bit.USB2_VB_VAL)				/**< Get USB2_VB_VAL */

#define Dd_Exs_Get_USB2_IDVBUSCTL_USB2_VBDET_SEL()			(IO_EXSTOP.USB2_IDVBUSCTL.bit.USB2_VBDET_SEL)			/**< Get USB2_VBDET_SEL */

#define Dd_Exs_Get_USB2_IDVBUSCTL_USB2_VBDET_VAL()			(IO_EXSTOP.USB2_IDVBUSCTL.bit.USB2_VBDET_VAL)			/**< Get USB2_VBDET_VAL */

// USB2_HDMAC1
#define Dd_Exs_Get_USB2_HDMAC1_HDMAC_BC1()					(IO_EXSTOP.USB2_HDMAC1.bit.HDMAC_BC1)					/**< Get HDMAC_BC1 */

#define Dd_Exs_Get_USB2_HDMAC1_HDMAC_BT1()					(IO_EXSTOP.USB2_HDMAC1.bit.HDMAC_BT1)					/**< Get HDMAC_BT1 */

#define Dd_Exs_Get_USB2_HDMAC1_HDMAC_MS1()					(IO_EXSTOP.USB2_HDMAC1.bit.HDMAC_MS1)					/**< Get HDMAC_MS1 */

// USB2_HDMAC2
#define Dd_Exs_Get_USB2_HDMAC2_HDMAC_BC2()					(IO_EXSTOP.USB2_HDMAC2.bit.HDMAC_BC2)					/**< Get HDMAC_BC2 */

#define Dd_Exs_Get_USB2_HDMAC2_HDMAC_BT2()					(IO_EXSTOP.USB2_HDMAC2.bit.HDMAC_BT2)					/**< Get HDMAC_BT2 */

#define Dd_Exs_Get_USB2_HDMAC2_HDMAC_MS2()					(IO_EXSTOP.USB2_HDMAC2.bit.HDMAC_MS2)					/**< Get HDMAC_MS2 */

// USB2_DMAFSM1
#define Dd_Exs_Get_USB2_DMAFSM1_FSMRST1()					(IO_EXSTOP.USB2_DMAFSM1.bit.FSMRST1)					/**< Get FSMRST1 */

#define Dd_Exs_Get_USB2_DMAFSM1_DMA_TRANS_STATE1()			(IO_EXSTOP.USB2_DMAFSM1.bit.DMA_TRANS_STATE1)			/**< Get DMA_TRANS_STATE1 */

// USB2_DMAFSM2
#define Dd_Exs_Get_USB2_DMAFSM2_FSMRST2()					(IO_EXSTOP.USB2_DMAFSM2.bit.FSMRST2)					/**< Get FSMRST2 */

#define Dd_Exs_Get_USB2_DMAFSM2_DMA_TRANS_STATE2()			(IO_EXSTOP.USB2_DMAFSM2.bit.DMA_TRANS_STATE2)			/**< Get DMA_TRANS_STATE2 */

// USB3_CKCTL
#define Dd_Exs_Get_USB3_CKCTL_ACLKEN()						(IO_EXSTOP.USB3_CKCTL.bit.ACLKEN)						/**< Get ACLKEN */

// USB3_RCTL
#define Dd_Exs_Get_USB3_RCTL_SFTRST()						(IO_EXSTOP.USB3_RCTL.bit.SFTRST)						/**< Get SFTRST */

#define Dd_Exs_Get_USB3_PHY_RESET_N()						(IO_EXSTOP.USB3_RCTL.bit.PHY_RESET_N)					/**< Get PHY_RESET_N */

// USB3_RCV_SNS
#define Dd_Exs_Get_USB3_RCV_SNS_ACJT_LEVEL()				(IO_EXSTOP.USB3_RCV_SNS.bit.ACJT_LEVEL)					/**< Get ACJT_LEVEL */

// USB3_FSEL
#define Dd_Exs_Get_USB3_FSEL_FSEL3()						(IO_EXSTOP.USB3_FSEL.bit.FSEL3)							/**< Get FSEL3 */

#define Dd_Exs_Get_USB3_FSEL_MPLL_MULTIPLIER()				(IO_EXSTOP.USB3_FSEL.bit.MPLL_MULTIPLIER)				/**< Get MPLL_MULTIPLIER */

#define Dd_Exs_Get_USB3_FSEL_SSC_REF_CLK_SEL()				(IO_EXSTOP.USB3_FSEL.bit.SSC_REF_CLK_SEL)				/**< Get SSC_REF_CLK_SEL */

#define Dd_Exs_Get_USB3_FSEL_REF_CLKDIV2()					(IO_EXSTOP.USB3_FSEL.bit.REF_CLKDIV2)					/**< Get REF_CLKDIV2 */

// USB3_RFC_CTL
#define Dd_Exs_Get_USB3_RFC_CTL_REF_SSP_EN()				(IO_EXSTOP.USB3_RFC_CTL.bit.REF_SSP_EN)					/**< Get REF_SSP_EN */

#define Dd_Exs_Get_USB3_RFC_CTL_REF_USE_PAD()				(IO_EXSTOP.USB3_RFC_CTL.bit.REF_USE_PAD)				/**< Get REF_USE_PAD */

// USB3_HO_IF
#define Dd_Exs_Get_USB3_HO_IF_PCI_IMPLEMENT()				(IO_EXSTOP.USB3_HO_IF.bit.PCI_IMPLEMENT)				/**< Get PCI_IMPLEMENT */

#define Dd_Exs_Get_USB3_HO_IF_HOST_MSI_ENABLE()				(IO_EXSTOP.USB3_HO_IF.bit.HOST_MSI_ENABLE)				/**< Get HOST_MSI_ENABLE */

#define Dd_Exs_Get_USB3_HO_IF_XHC_BME()						(IO_EXSTOP.USB3_HO_IF.bit.XHC_BME)						/**< Get XHC_BME */

#define Dd_Exs_Get_USB3_HO_IF_XHC_AC64()					(IO_EXSTOP.USB3_HO_IF.bit.XHC_AC64)						/**< Get XHC_AC64 */

#define Dd_Exs_Get_USB3_HO_IF_HOST_SYSTEM_ERR()				(IO_EXSTOP.USB3_HO_IF.bit.HOST_SYSTEM_ERR)				/**< Get HOST_SYSTEM_ERR */

// USB3_JT_ADJ
#define Dd_Exs_Get_USB3_JT_ADJ_FLADJ_30MHZ_REG()			(IO_EXSTOP.USB3_JT_ADJ.bit.FLADJ_30MHZ_REG)				/**< Get FLADJ_30MHZ_REG */

#define Dd_Exs_Get_USB3_JT_ADJ_USB3_OVRCUR_EN()				(IO_EXSTOP.USB3_JT_ADJ.bit.USB3_OVRCUR_EN)				/**< Get USB3_OVRCUR_EN */

// USB3_IDVBUSCTL
#define Dd_Exs_Get_USB3_IDVBUSCTL_USB3_IDPU_VAL()			(IO_EXSTOP.USB3_IDVBUSCTL.bit.USB3_IDPU_VAL)			/**< Get USB3_IDPU_VAL */

#define Dd_Exs_Get_USB3_IDVBUSCTL_USB3_ID_SEL()				(IO_EXSTOP.USB3_IDVBUSCTL.bit.USB3_ID_SEL)				/**< Get USB3_ID_SEL */

#define Dd_Exs_Get_USB3_IDVBUSCTL_USB3_ID_VAL()				(IO_EXSTOP.USB3_IDVBUSCTL.bit.USB3_ID_VAL)				/**< Get USB3_ID_VAL */

#define Dd_Exs_Get_USB3_IDVBUSCTL_USB3_VB_SEL()				(IO_EXSTOP.USB3_IDVBUSCTL.bit.USB3_VB_SEL)				/**< Get USB3_VB_SEL */

#define Dd_Exs_Get_USB3_IDVBUSCTL_USB3_VB_VAL()				(IO_EXSTOP.USB3_IDVBUSCTL.bit.USB3_VB_VAL)				/**< Get USB3_VB_VAL */

#define Dd_Exs_Get_USB3_IDVBUSCTL_USB3_VBDET_SEL()			(IO_EXSTOP.USB3_IDVBUSCTL.bit.USB3_VBDET_SEL)			/**< Get USB3_VBDET_SEL */

#define Dd_Exs_Get_USB3_IDVBUSCTL_USB3_VBDET_VAL()			(IO_EXSTOP.USB3_IDVBUSCTL.bit.USB3_VBDET_VAL)			/**< Get USB3_VBDET_VAL */

// NF_IP_CTL
#define Dd_Exs_Get_NF_IP_CTL_DSCV_INHIBIT()					(IO_EXSTOP.NF_IP_CTL.bit.DSCV_INHIBIT)					/**< Get DSCV_INHIBIT */

#define Dd_Exs_Get_NF_IP_CTL_DSCV_IGNR_CRC()				(IO_EXSTOP.NF_IP_CTL.bit.DSCV_IGNR_CRC)					/**< Get DSCV_IGNR_CRC */

#define Dd_Exs_Get_NF_IP_CTL_BOOT_EN()						(IO_EXSTOP.NF_IP_CTL.bit.BOOT_EN)						/**< Get BOOT_EN */

#define Dd_Exs_Get_NF_IP_CTL_IDDQ_EN()						(IO_EXSTOP.NF_IP_CTL.bit.IDDQ_EN)						/**< Get IDDQ_EN */

// NF_BTSQ_STATE
#define Dd_Exs_Get_NF_BTSQ_STATE_INIT_FAIL()				(IO_EXSTOP.NF_BTSQ_STATE.bit.INIT_FAIL)					/**< Get INIT_FAIL */

#define Dd_Exs_Get_NF_BTSQ_STATE_BOOT_COMP()				(IO_EXSTOP.NF_BTSQ_STATE.bit.BOOT_COMP)					/**< Get BOOT_COMP */

#define Dd_Exs_Get_NF_BTSQ_STATE_BOOT_ERROR()				(IO_EXSTOP.NF_BTSQ_STATE.bit.BOOT_ERROR)				/**< Get BOOT_ERROR */

#define Dd_Exs_Get_NF_BTSQ_STATE_CTRL_BUSY()				(IO_EXSTOP.NF_BTSQ_STATE.bit.CTRL_BUSY)					/**< Get CTRL_BUSY */

// NF_RB_VALID_TM
#define Dd_Exs_Get_NF_RB_VALID_TM()							(IO_EXSTOP.NF_RB_VALID_TM.bit.RB_VALID_TIME)			/**< Get RB_VALID_TIME */

// NF_PHY_CTRL_REG
#define Dd_Exs_Get_NF_PHY_CTRL_REG()						(IO_EXSTOP.NF_PHY_CTRL_REG.bit.PHY_CTRL)				/**< Get PHY_CTRL */

// NF_PHY_DQ_TIM
#define Dd_Exs_Get_NF_PHY_DQ_TIMING()						(IO_EXSTOP.NF_PHY_DQ_TIM.bit.PHY_DQ_TIMING)				/**< Get PHY_DQ_TIMING */

// NF_PHY_DQS_TIM
#define Dd_Exs_Get_NF_PHY_DQS_TIMING()						(IO_EXSTOP.NF_PHY_DQS_TIM.bit.PHY_DQS_TIMING)			/**< Get PHY_DQS_TIMING */

// NF_PHY_GT_LPBK
#define Dd_Exs_Get_NF_PHY_GT_LPBK_CTL()						(IO_EXSTOP.NF_PHY_GT_LPBK.bit.PHY_GT_LPBK_CTL)			/**< Get PHY_GT_LPBK_CTL */

// NF_PHY_DLL_MCTL
#define Dd_Exs_Get_NF_PHY_DLL_MCTL()						(IO_EXSTOP.NF_PHY_DLL_MCTL.bit.PHY_DLL_MST_CTL)			/**< Get PHY_DLL_MST_CTL */

// NF_PHY_DLL_SCTL
#define Dd_Exs_Get_NF_PHY_DLL_SCTL()						(IO_EXSTOP.NF_PHY_DLL_SCTL.bit.PHY_DLL_SLV_CTL)			/**< Get PHY_DLL_SLV_CTL */

// NF_BOOT_ECC_CTL
#define Dd_Exs_Get_NF_BOOT_ECC_CTL_SEC_SIZ()				(IO_EXSTOP.NF_BOOT_ECC_CTL.bit.BOOT_ECC_SEC_SIZ)		/**< Get BOOT_ECC_SEC_SIZ */

#define Dd_Exs_Get_NF_BOOT_ECC_CTL_CORR_STR()				(IO_EXSTOP.NF_BOOT_ECC_CTL.bit.BT_ECC_CORR_STR)			/**< Get BT_ECC_CORR_STR */

#define Dd_Exs_Get_NF_BOOT_ECC_CTL_EN()						(IO_EXSTOP.NF_BOOT_ECC_CTL.bit.BOOT_ECC_EN)				/**< Get BOOT_ECC_EN */

#define Dd_Exs_Get_NF_NFWPX_CNT()							(IO_EXSTOP.NF_NFWPX_CNT.bit.NFWPX_CNT)					/**< Get NFWPX_CNT */

// INTSTAT
#define Dd_Exs_Get_INTSTAT_SD2_RIS()						(IO_EXSTOP.INTSTAT.bit.__INTSTAT00)						/**< Get SD2 (UHS1 Wifi) rising edge detection */
#define Dd_Exs_Get_INTSTAT_SD2_FIS()						(IO_EXSTOP.INTSTAT.bit.__INTSTAT01)						/**< Get SD2 (UHS1 Wifi) falling edge detection */
#define Dd_Exs_Get_INTSTAT_SD3_RIS()						(IO_EXSTOP.INTSTAT.bit.__INTSTAT02)						/**< Get SD3 (UHS2) rising edge detection */
#define Dd_Exs_Get_INTSTAT_SD3_FIS()						(IO_EXSTOP.INTSTAT.bit.__INTSTAT03)						/**< Get SD3 (UHS2) falling edge detection */
#define Dd_Exs_Get_INTSTAT_SD4_RIS()						(IO_EXSTOP.INTSTAT.bit.__INTSTAT04)						/**< Get SD4 (UHS1 Card) rising edge detection */
#define Dd_Exs_Get_INTSTAT_SD4_FIS()						(IO_EXSTOP.INTSTAT.bit.__INTSTAT05)						/**< Get SD4 (UHS1 Card) falling edge detection */
#define Dd_Exs_Get_INTSTAT_PCIe0_RIS()						(IO_EXSTOP.INTSTAT.bit.__INTSTAT06)						/**< Get PCIe0 rising edge detection */
#define Dd_Exs_Get_INTSTAT_PCIe0_FIS()						(IO_EXSTOP.INTSTAT.bit.__INTSTAT07)						/**< Get PCIe0 falling edge detection */
#define Dd_Exs_Get_INTSTAT_PCIe1_RIS()						(IO_EXSTOP.INTSTAT.bit.__INTSTAT08)						/**< Get PCIe1 rising edge detection */
#define Dd_Exs_Get_INTSTAT_PCIe1_FIS()						(IO_EXSTOP.INTSTAT.bit.__INTSTAT09)						/**< Get PCIe1 falling edge detection */
#define Dd_Exs_Get_INTSTAT_USB2VBUS_RIS()					(IO_EXSTOP.INTSTAT.bit.__INTSTAT10)						/**< Get USB2 VBUS rising edge detection */
#define Dd_Exs_Get_INTSTAT_USB2VBUS_FIS()					(IO_EXSTOP.INTSTAT.bit.__INTSTAT11)						/**< Get USB2 VBUS falling edge detection */
#define Dd_Exs_Get_INTSTAT_USB2ID_RIS()						(IO_EXSTOP.INTSTAT.bit.__INTSTAT12)						/**< Get USB2 ID rising edge detection */
#define Dd_Exs_Get_INTSTAT_USB2ID_FIS()						(IO_EXSTOP.INTSTAT.bit.__INTSTAT13)						/**< Get USB2 ID falling edge detection */
#define Dd_Exs_Get_INTSTAT_USB3VBUS_RIS()					(IO_EXSTOP.INTSTAT.bit.__INTSTAT14)						/**< Get USB3 VBUS rising edge detection */
#define Dd_Exs_Get_INTSTAT_USB3VBUS_FIS()					(IO_EXSTOP.INTSTAT.bit.__INTSTAT15)						/**< Get USB3 VBUS falling edge detection */
#define Dd_Exs_Get_INTSTAT_USB3ID_RIS()						(IO_EXSTOP.INTSTAT.bit.__INTSTAT16)						/**< Get USB3 ID rising edge detection */
#define Dd_Exs_Get_INTSTAT_USB3ID_FIS()						(IO_EXSTOP.INTSTAT.bit.__INTSTAT17)						/**< Get USB3 ID falling edge detection */


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


/**
Set status of transition requests to LowPower mode of XDMAC.<br>
@param[in]  val		Setting value
					<ul><li> 0:LowPower mode request.
						<li> 1:Normal mode request.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_XDMAC_LowPower_Ctl_CSYSREQ(ULONG val);

/**
Set reset control of nf_rst_n terminal.<br>
@param[in]  *val Setting value.
					<ul><li> 0:nf_rst_n terminal reset is released.
						<li> 1:nf_rst_n terminal reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_nf_rst_n_Soft_Reset(ULONG val);

/**
Set reset control of bch_rst_n terminal.<br>
@param[in]  *val Setting value
					<ul><li> 0:bch_rst_n terminal reset is released.
						<li> 1:bch_rst_n terminal reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_bch_rst_n_Soft_Reset(ULONG val);

/**
Set reset control of reg_rst_n terminal.<br>
@param[in]  *val Setting value
					<ul><li> 0:reg_rst_n terminal reset is released.
						<li> 1:reg_rst_n terminal reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_reg_rst_n_Soft_Reset(ULONG val);

/**
Set reset control of NETSEC macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:NETSEC macro reset is released.
						<li> 1:NETSEC macro reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_NETSEC_Soft_Reset(ULONG val);

/**
Set reset control of eMMC macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:eMMC macro reset is released.
						<li> 1:eMMC macro reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_eMMC_Soft_Reset(ULONG val);

/**
Set reset control of RELC macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:RELC macro reset is released.
						<li> 1:RELC macro reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RELC_Soft_Reset(ULONG val);

/**
Set reset control of Card Debounce SD3 (UHS2).<br>
@param[in]  *val Setting value
					<ul><li> 0:Card Debounce reset is released.
					<ul><li> 1:Card Debounce reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_SD3_CD_Debounce_Soft_Reset(ULONG val);

/**
Set reset control of Card Debounce SD4 (UHS1 Card).<br>
@param[in]  *val Setting value
					<ul><li> 0:Card Debounce reset is released.
						<li> 1:Card Debounce reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_SD4_CD_Debounce_Soft_Reset(ULONG val);

/**
Set reset control of Card Debounce PCIe0.<br>
@param[in]  *val Setting value
					<ul><li> 0:Card Debounce reset is released.
						<li> 1:Card Debounce reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_PCIE0_CD_Debounce_Soft_Reset(ULONG val);

/**
Set reset control of Card Debounce PCIe1.<br>
@param[in]  *val Setting value
					<ul><li> 0:Card Debounce reset is released.
						<li> 1:Card Debounce reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_PCIE1_CD_Debounce_Soft_Reset(ULONG val);

/**
Set reset control of Card Debounce USB2 VBUS.<br>
@param[in]  *val Setting value
					<ul><li> 0:Card Debounce reset is released.
						<li> 1:Card Debounce reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_USB2VBUS_CD_Debounce_Soft_Reset(ULONG val);

/**
Set reset control of Card Debounce USB2 ID.<br>
@param[in]  *val Setting value
					<ul><li> 0:Card Debounce reset is released.
						<li> 1:Card Debounce reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_USB2ID_CD_Debounce_Soft_Reset(ULONG val);

/**
Set reset control of Card Debounce USB3 VBUS.<br>
@param[in]  *val Setting value
					<ul><li> 0:Card Debounce reset is released.
						<li> 1:Card Debounce reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_USB3VBUS_CD_Debounce_Soft_Reset(ULONG val);

/**
Set reset control of Card Debounce USB3 ID.<br>
@param[in]  *val Setting value
					<ul><li> 0:Card Debounce reset is released.
						<li> 1:Card Debounce reset is set.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_USB3ID_CD_Debounce_Soft_Reset(ULONG val);

/**
Set SD3( UHS2 ) Detect INT Mask(Rising Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the SD3( UHS2 ) detection rising edge detection factor is output.
						<li> 1:The interrupt signal by the SD3( UHS2 ) detection rising edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_SD3_RIM(ULONG val);

/**
Set SD3( UHS2 ) Detect INT Mask(Falling Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the SD3( UHS2 ) detection falling edge detection factor is output.
						<li> 1:The interrupt signal by the SD3( UHS2 ) detection falling edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_SD3_FIM(ULONG val);

/**
Set SD4( UHS1 Card ) Detect INT Mask(Rising Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the SD4( UHS1 Card ) detection rising edge detection factor is output.
						<li> 1:The interrupt signal by the SD4( UHS1 Card ) detection rising edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_SD4_RIM(ULONG val);

/**
Set SD4( UHS1 Card ) Detect INT Mask(Falling Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the SD4( UHS1 Card ) detection falling edge detection factor is output.
						<li> 1:The interrupt signal by the SD4( UHS1 Card ) detection falling edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_SD4_FIM(ULONG val);

/**
Set PCIe0 Detect INT Mask(Rising Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the PCIe0 detection rising edge detection factor is output.
						<li> 1:The interrupt signal by the PCIe0 detection rising edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_PCIe0_RIM(ULONG val);

/**
Set PCIe0 Detect INT Mask(Falling Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the PCIe0 detection falling edge detection factor is output.
						<li> 1:The interrupt signal by the PCIe0 detection falling edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_PCIe0_FIM(ULONG val);

/**
Set PCIe1 Detect INT Mask(Rising Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the PCIe1 detection rising edge detection factor is output.
						<li> 1:The interrupt signal by the PCIe1 detection rising edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_PCIe1_RIM(ULONG val);

/**
Set PCIe1 Detect INT Mask(Falling Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the PCIe1 detection falling edge detection factor is output.
						<li> 1:The interrupt signal by the PCIe1 detection falling edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_PCIe1_FIM(ULONG val);

/**
Set USB2 VBUS Detect INT Mask(Rising Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the USB2 VBUS detection rising edge detection factor is output.
						<li> 1:The interrupt signal by the USB2 VBUS detection rising edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_USB2VBUS_RIM(ULONG val);

/**
Set USB2 VBUS Detect INT Mask(Falling Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the USB2 VBUS detection falling edge detection factor is output.
						<li> 1:The interrupt signal by the USB2 VBUS detection falling edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_USB2VBUS_FIM(ULONG val);

/**
Set USB2 ID Detect INT Mask(Rising Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the USB2 ID detection rising edge detection factor is output.
						<li> 1:The interrupt signal by the USB2 ID detection rising edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_USB2ID_RIM(ULONG val);

/**
Set USB2 ID Detect INT Mask(Falling Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the USB2 ID detection falling edge detection factor is output.
						<li> 1:The interrupt signal by the USB2 ID detection falling edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_USB2ID_FIM(ULONG val);

/**
Set USB3 VBUS Detect INT Mask(Rising Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the USB3 VBUS detection rising edge detection factor is output.
						<li> 1:The interrupt signal by the USB3 VBUS detection rising edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_USB3VBUS_RIM(ULONG val);

/**
Set USB3 VBUS Detect INT Mask(Falling Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the USB3 VBUS detection falling edge detection factor is output.
						<li> 1:The interrupt signal by the USB3 VBUS detection falling edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_USB3VBUS_FIM(ULONG val);

/**
Set USB3 ID Detect INT Mask(Rising Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the USB3 ID detection rising edge detection factor is output.
						<li> 1:The interrupt signal by the USB3 ID detection rising edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_USB3ID_RIM(ULONG val);

/**
Set USB3 ID Detect INT Mask(Falling Edge).<br>
@param[in]  *val Setting value
					<ul><li> 0:The interrupt signal by the USB3 ID detection falling edge detection factor is output.
						<li> 1:The interrupt signal by the USB3 ID detection falling edge detection factor is mask.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_INTMSK_USB3ID_FIM(ULONG val);

/**
Set SD3( UHS2 ) Debounce ON/OFF.<br>
@param[in]  *val Setting value
					<ul><li> 0:The debounce of the SD3( UHS2 ) signal is turned off.
						<li> 1:The debounce of the SD3( UHS2 ) signal is turned on.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_SD3_EN(ULONG val);

/**
Set SD4( UHS1 Card ) Debounce ON/OFF.<br>
@param[in]  *val Setting value
					<ul><li> 0:The debounce of the SD4( UHS1 Card ) signal is turned off.
						<li> 1:The debounce of the SD4( UHS1 Card ) signal is turned on.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_SD4_EN(ULONG val);

/**
Set PCIe0 Debounce ON/OFF.<br>
@param[in]  *val Setting value
					<ul><li> 0:The debounce of the PCIe0 signal is turned off.
						<li> 1:The debounce of the PCIe0 signal is turned on.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_PCIE0_EN(ULONG val);

/**
Set PCIe1 Debounce ON/OFF.<br>
@param[in]  *val Setting value
					<ul><li> 0:The debounce of the PCIe1 signal is turned off.
						<li> 1:The debounce of the PCIe1 signal is turned on.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_PCIE1_EN(ULONG val);

/**
Set USB2 VBUS Debounce ON/OFF.<br>
@param[in]  *val Setting value
					<ul><li> 0:The debounce of the USB2 VBUS signal is turned off.
						<li> 1:The debounce of the USB2 VBUS signal is turned on.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_USB2VBUS_EN(ULONG val);

/**
Set USB2 ID Debounce ON/OFF.<br>
@param[in]  *val Setting value
					<ul><li> 0:The debounce of the USB2 ID signal is turned off.
						<li> 1:The debounce of the USB2 ID signal is turned on.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_USB2ID_EN(ULONG val);

/**
Set USB3 VBUS Debounce ON/OFF.<br>
@param[in]  *val Setting value
					<ul><li> 0:The debounce of the USB3 VBUS signal is turned off.
						<li> 1:The debounce of the USB3 VBUS signal is turned on.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_USB3VBUS_EN(ULONG val);

/**
Set USB3 ID Debounce ON/OFF.<br>
@param[in]  *val Setting value
					<ul><li> 0:The debounce of the USB3 ID signal is turned off.
						<li> 1:The debounce of the USB3 ID signal is turned on.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_USB3ID_EN(ULONG val);

/**
Set SD-Card ch3 Debounce time.<br>
@param[in]  *val Setting value
					<ul><li> 00:APBCLK 131072 cycles<br>
						<li> 01:APBCLK 65536 cycles (131072 cycles /2).
						<li> 10:APBCLK 262144 cycles (131072 cycles *2).
						<li> 11:APBCLK 524288 cycles (131072 cycles *4).</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_SD3DETIM(ULONG val);

/**
Set SD-Card ch4 Debounce time..<br>
@param[in]  *val Setting value
					<ul><li> 00:APBCLK 131072 cycles<br>
						<li> 01:APBCLK 65536 cycles (131072 cycles /2).
						<li> 10:APBCLK 262144 cycles (131072 cycles *2).
						<li> 11:APBCLK 524288 cycles (131072 cycles *4).</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_SD4DETIM(ULONG val);

/**
Set PCIe0 Debounce time.<br>
@param[in]  *val Setting value
					<ul><li> 00:APBCLK 131072 cycles<br>
						<li> 01:APBCLK 65536 cycles (131072 cycles /2).
						<li> 10:APBCLK 262144 cycles (131072 cycles *2).
						<li> 11:APBCLK 524288 cycles (131072 cycles *4).</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_PCIE0DETIM(ULONG val);

/**
Set PCIe1 Debounce time.<br>
@param[in]  *val Setting value
					<ul><li> 00:APBCLK 131072 cycles<br>
						<li> 01:APBCLK 65536 cycles (131072 cycles /2).
						<li> 10:APBCLK 262144 cycles (131072 cycles *2).
						<li> 11:APBCLK 524288 cycles (131072 cycles *4).</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_PCIE1DETIM(ULONG val);

/**
Set USB2 VBUS Debounce time.<br>
@param[in]  *val Setting value
					<ul><li> 00:APBCLK 131072 cycles<br>
						<li> 01:APBCLK 65536 cycles (131072 cycles /2).
						<li> 10:APBCLK 262144 cycles (131072 cycles *2).
						<li> 11:APBCLK 524288 cycles (131072 cycles *4).</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_USB2VBUSDETIM(ULONG val);

/**
Set USB2 ID Debounce time.<br>
@param[in]  *val Setting value
					<ul><li> 00:APBCLK 131072 cycles<br>
						<li> 01:APBCLK 65536 cycles (131072 cycles /2).
						<li> 10:APBCLK 262144 cycles (131072 cycles *2).
						<li> 11:APBCLK 524288 cycles (131072 cycles *4).</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_USB2IDDETIM(ULONG val);

/**
Set USB3 VBUS Debounce time.<br>
@param[in]  *val Setting value
					<ul><li> 00:APBCLK 131072 cycles<br>
						<li> 01:APBCLK 65536 cycles (131072 cycles /2).
						<li> 10:APBCLK 262144 cycles (131072 cycles *2).
						<li> 11:APBCLK 524288 cycles (131072 cycles *4).</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_USB3VBUSDETIM(ULONG val);

/**
Set USB3 ID Debounce time.<br>
@param[in]  *val Setting value
					<ul><li> 00:APBCLK 131072 cycles<br>
						<li> 01:APBCLK 65536 cycles (131072 cycles /2).
						<li> 10:APBCLK 262144 cycles (131072 cycles *2).
						<li> 11:APBCLK 524288 cycles (131072 cycles *4).</ul>
@retval	None.
*/
VOID Dd_Exs_Set_DEBEN_USB3IDDETIM(ULONG val);

/**
Set the mode setting of the card detection SD3( UHS2 ).<br>
param[in]  *val Setting value
					<ul><li> 0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the SD3( UHS2 ) signal.
						<li> 1:After exiting reset at debounce circuit, generates an interrupt by the state of the SD3( UHS2 ) signal.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_CDINS_SD3_MOD(ULONG val);

/**
Set the mode setting of the card detection SD4( UHS1 Card ).<br>
@param[in]  *val Setting value
					<ul><li> 0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the SD4( UHS1 Card ) signal.
						<li> 1:After exiting reset at debounce circuit, generates an interrupt by the state of the SD4( UHS1 Card ) signal.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_CDINS_SD4_MOD(ULONG val);

/**
Set the mode setting of the card detection PCIe0.<br>
@param[in]  *val Setting value
					<ul><li> 0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the PCIe0 signal.
						<li> 1:After exiting reset at debounce circuit, generates an interrupt by the state of the PCIe0 signal.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_CDINS_PCIE0_MOD(ULONG val);

/**
Set the mode setting of the card detection PCIe1.<br>
@param[in]  *val Setting value
					<ul><li> 0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the PCIe1 signal.
						<li> 1:After exiting reset at debounce circuit, generates an interrupt by the state of the PCIe1 signal.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_CDINS_PCIE1_MOD(ULONG val);

/**
Set the mode setting of the card detection USB2 VBUS.<br>
@param[in]  *val Setting value
					<ul><li> 0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the USB2 VBUS signal.
						<li> 1:After exiting reset at debounce circuit, generates an interrupt by the state of the USB2 VBUS signal.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_CDINS_USB2VBUS_MOD(ULONG val);

/**
Set the mode setting of the card detection USB2 ID.<br>
@param[in]  *val Setting value
					<ul><li> 0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the USB2 ID signal.
						<li> 1:After exiting reset at debounce circuit, generates an interrupt by the state of the USB2 ID signal.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_CDINS_USB2ID_MOD(ULONG val);

/**
Set the mode setting of the card detection USB3 VBUS.<br>
@param[in]  *val Setting value
					<ul><li> 0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the USB3 VBUS signal.
						<li> 1:After exiting reset at debounce circuit, generates an interrupt by the state of the USB3 VBUS signal.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_CDINS_USB3VBUS_MOD(ULONG val);

/**
Set the mode setting of the card detection USB3 ID.<br>
@param[in]  *val Setting value
					<ul><li> 0:After exiting reset at debounce circuit, generates an interrupt regardless of the state of the USB3 ID signal.
						<li> 1:After exiting reset at debounce circuit, generates an interrupt by the state of the USB3 ID signal.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_CDINS_USB3ID_MOD(ULONG val);

/**
Set the RELC expanded data read side HPROT setting(Data/Opcode).<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_RELC_HPROT_Read(ULONG val);

/**
Set the RELC expanded data write side HPROT setting(Data/Opcode).<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_RELC_HPROT_Write(ULONG val);

/**
Set SD2( UHS1 Wifi ) macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_SD2(ULONG val);

/**
Set SD3( UHS2 ) macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_SD3(ULONG val);

/**
Set SD4( UHS1 Card ) macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_SD4(ULONG val);

/**
Set Nand Flash macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_NF(ULONG val);

/**
Set RELC macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_RELC(ULONG val);

/**
Set NETSEC macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_NETSEC(ULONG val);

/**
Set USB2 macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_USB2(ULONG val);

/**
Set USB3 macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_USB3(ULONG val);

/**
Set PCIE0 macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_PCIE0(ULONG val);

/**
Set PCIE1 macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_PCIE1(ULONG val);

/**
Set eMMC macro PD signal output of RAMCELL with PD.<br>
@param[in]  *val Setting value
					<ul><li> 0:Power-down mode is released.
						<li> 1:Enters Power-down mode.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RAM_PD_Ctrl_EMMC(ULONG val);

/**
Set the output a signal to stop control of the Nand Flash BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output Nand Flash BUS CLOCK.
						<li> 1:Stop Nand Flash BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_NF_BUS_CLOCK_Stop_Ctrl(ULONG val);

/**
Set the output a signal to stop control of the PCIE0 BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output PCIE0 BUS CLOCK.
						<li> 1:Stop PCIE0 BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_PCIE0_BUS_CLOCK_Stop_Ctrl(ULONG val);

/**
Set the output a signal to stop control of the PCIE1 BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output PCIE1 BUS CLOCK.
						<li> 1:Stop PCIE1 BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_PCIE1_BUS_CLOCK_Stop_Ctrl(ULONG val);

/**
Set the output a signal to stop control of the XDMAC BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output XDMAC BUS CLOCK.
						<li> 1:Stop XDMAC BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_XDMAC_BUS_CLOCK_Stop_Ctrl(ULONG val);

/**
Set the output a signal to stop control of the HDMAC BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output HDMAC BUS CLOCK.
						<li> 1:Stop HDMAC BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_HDMAC_BUS_CLOCK_Stop_Ctrl(ULONG val);

/**
Set the output a signal to stop control of the eMMC BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output eMMC BUS CLOCK.
						<li> 1:Stop eMMC BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_EMMC_BUS_CLOCK_Stop_Ctrl(ULONG val);

/**
Set the output a signal to stop control of the SD2 (Wifi) BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output SD2 (Wifi) BUS CLOCK.
						<li> 1:Stop SD2 (Wifi) BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_SD2_BUS_CLOCK_Stop_Ctrl(ULONG val);

/**
Set the output a signal to stop control of the SD3 (UHS2) BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output SD3 (UHS2) BUS CLOCK.
						<li> 1:Stop SD3 (UHS2) BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_SD3_BUS_CLOCK_Stop_Ctrl(ULONG val);

/**
Set the output a signal to stop control of the SD4 (UHS1 Card) BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output SD4 (UHS1 Card) BUS CLOCK.
						<li> 1:Stop SD4 (UHS1 Card) BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_SD4_BUS_CLOCK_Stop_Ctrl(ULONG val);

/**
Set the output a signal to stop control of the NETSEC BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output NETSEC BUS CLOCK.
						<li> 1:Stop NETSEC BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_NETSEC_BUS_CLOCK_Stop_Ctrl(ULONG val);

/**
Set the output a signal to stop control of the RELC BUS CLOCK macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output RELC BUS CLOCK.
						<li> 1:Stop RELC BUS CLOCK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_RELC_BUS_CLOCK_Stop_Ctrl(ULONG val);

/**
Set the output a signal to stop control of the PTP_CLK NETSEC macro.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output PTP_CLK.
						<li> 1:Stop PTP_CLK.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_NETSEC_PTPCLK_Stop_Ctrl_NETSEC_PTPSTP(ULONG val);

/**
Set the output a signal to stop control of the MMC_BCLK_I.<br>
@param[in]  *val Setting value
					<ul><li> 0:Output MMC_BCLK_I.
						<li> 1:Stop MMC_BCLK_I.</ul>
@retval	None.
*/
VOID Dd_Exs_Set_EMMC_CKSTP_EMCLKSTP(ULONG val);

/**
Set PV_DTIMEC.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_EM_PV_DTIMEC(ULONG val);

/**
Set PV_AMPBL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_EM_PV_AMPBL(ULONG val);

/**
Set CR_SLOTTYPE.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_EM_CR_SLOTTYPE(ULONG val);

/**
Set CR_BCLKFREQ.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_EM_CR_BCLKFREQ(ULONG val);

/**
Set EMCD.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_EM_CDDET_EMCD(ULONG val);

/**
Set HDC_HCLKEN.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_CKCTL_HDC_HCLKEN(ULONG val);

/**
Set HO_HCLKEN.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_CKCTL_HO_HCLKEN(ULONG val);

/**
Set HDC_SRST.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_RCTL_HDC_SRST(ULONG val);

/**
Set HO_SRST.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_RCTL_HO_SRST(ULONG val);

/**
Set ANPD.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_ANPD_ANPD(ULONG val);

/**
Set HF_SEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_HFSEL_HF_SEL(ULONG val);

/**
Set FSEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_FSEL_FSEL(ULONG val);

/**
Set PLLBTUNE.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_FSEL_PLLBTUNE(ULONG val);

/**
Set SS_FADJ_HOST_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_EHCI_STRAP_SS_FADJ_HOST_I(ULONG val);

/**
Set SS_FLADJ_VAL_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_EHCI_STRAP_SS_FLADJ_VAL_I(ULONG val);

/**
Set SS_FLADJ_VAL_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_OHCI_IF_OHCI_CLKSEL_I(ULONG val);

/**
Set SS_ENA_INCR4_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_LMODSET_SS_ENA_INCR4_I(ULONG val);

/**
Set SS_ENA_INCR8_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_LMODSET_SS_ENA_INCR8_I(ULONG val);

/**
Set SS_ENA_INCR16_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_LMODSET_SS_ENA_INCR16_I(ULONG val);

/**
Set SS_ENA_INC_AL_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_LMODSET_SS_ENA_INC_AL_I(ULONG val);

/**
Set SS_EHCI64B_EN_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_LMODSET_SS_EHCI64B_EN_I(ULONG val);

/**
Set RPDPEN_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_PMODSET_RPDPEN_I(ULONG val);

/**
Set RPDMEN_I.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_PMODSET_RPDMEN_I(ULONG val);

/**
Set USB2_EHPT.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_HPT_CTL_USB2_EHPT(ULONG val);

/**
Set USB2_OHPT.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_HPT_CTL_USB2_OHPT(ULONG val);

/**
Set USB2_IDPU_VAL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_IDVBUSCTL_USB2_IDPU_VAL(ULONG val);

/**
Set USB2_IDPU_SEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_IDVBUSCTL_USB2_IDPU_SEL(ULONG val);

/**
Set USB2_ID_SEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_IDVBUSCTL_USB2_ID_SEL(ULONG val);

/**
Set USB2_ID_VAL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_IDVBUSCTL_USB2_ID_VAL(ULONG val);

/**
Set USB2_VB_SEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_IDVBUSCTL_USB2_VB_SEL(ULONG val);

/**
Set USB2_VB_VAL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_IDVBUSCTL_USB2_VB_VAL(ULONG val);

/**
Set USB2_VBDET_SEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_IDVBUSCTL_USB2_VBDET_SEL(ULONG val);

/**
Set USB2_VBDET_VAL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_IDVBUSCTL_USB2_VBDET_VAL(ULONG val);

/**
Set HDMAC_BC1.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_HDMAC1_HDMAC_BC1(ULONG val);

/**
Set HDMAC_BT1.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_HDMAC1_HDMAC_BT1(ULONG val);

/**
Set HDMAC_MS1.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_HDMAC1_HDMAC_MS1(ULONG val);

/**
Set HDMAC_BC2.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_HDMAC2_HDMAC_BC2(ULONG val);

/**
Set HDMAC_BT2.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_HDMAC2_HDMAC_BT2(ULONG val);

/**
Set HDMAC_MS2.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_HDMAC2_HDMAC_MS2(ULONG val);

/**
Set FSMRST1.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_DMAFSM1_FSMRST1(ULONG val);

/**
Set FSMRST2.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB2_DMAFSM2_FSMRST2(ULONG val);

/**
Set ACLKEN.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_CKCTL_ACLKEN(ULONG val);

/**
Set SFTRST.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_RCTL_SFTRST(ULONG val);

/**
Set PHY_RESET_N.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_RCTL_PHY_RESET_N(ULONG val);

/**
Set ACJT_LEVEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_RCV_SNS_ACJT_LEVEL(ULONG val);

/**
Set FSEL3.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_FSEL_FSEL3(ULONG val);

/**
Set MPLL_MULTIPLIER.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_FSEL_MPLL_MULTIPLIER(ULONG val);

/**
Set SSC_REF_CLK_SEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_FSEL_SSC_REF_CLK_SEL(ULONG val);

/**
Set REF_CLKDIV2.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_FSEL_REF_CLKDIV2(ULONG val);

/**
Set REF_SSP_EN.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_RFC_CTL_REF_SSP_EN(ULONG val);

/**
Set REF_USE_PAD.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_RFC_CTL_REF_USE_PAD(ULONG val);

/**
Set PCI_IMPLEMENT.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_HO_IF_PCI_IMPLEMENT(ULONG val);

/**
Set HOST_MSI_ENABLE.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_HO_IF_HOST_MSI_ENABLE(ULONG val);

/**
Set XHC_BME.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_HO_IF_XHC_BME(ULONG val);

/**
Set XHC_AC64.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_HO_IF_XHC_AC64(ULONG val);

/**
Set FLADJ_30MHZ_REG.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_JT_ADJ_FLADJ_30MHZ_REG(ULONG val);

/**
Set USB3_OVRCUR_EN.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_JT_ADJ_USB3_OVRCUR_EN(ULONG val);

/**
Set USB3_IDPU_VAL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_IDVBUSCTL_USB3_IDPU_VAL(ULONG val);

/**
Set USB3_ID_SEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_IDVBUSCTL_USB3_ID_SEL(ULONG val);

/**
Set USB3_ID_VAL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_IDVBUSCTL_USB3_ID_VAL(ULONG val);

/**
Set USB3_VB_SEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_IDVBUSCTL_USB3_VB_SEL(ULONG val);

/**
Set USB3_VB_VAL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_IDVBUSCTL_USB3_VB_VAL(ULONG val);

/**
Set USB3_VBDET_SEL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_IDVBUSCTL_USB3_VBDET_SEL(ULONG val);

/**
Set USB3_VBDET_VAL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_USB3_IDVBUSCTL_USB3_VBDET_VAL(ULONG val);

/**
Set DSCV_INHIBIT.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_IP_CTL_DSCV_INHIBIT(ULONG val);

/**
Set DSCV_IGNR_CRC.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_IP_CTL_DSCV_IGNR_CRC(ULONG val);

/**
Set BOOT_EN.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_IP_CTL_BOOT_EN(ULONG val);

/**
Set IDDQ_EN.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_IP_CTL_IDDQ_EN(ULONG val);

/**
Set RB_VALID_TIME.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_RB_VALID_TM(ULONG val);

/**
Set PHY_CTRL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_PHY_CTRL_REG(ULONG val);

/**
Set PHY_DQ_TIMENG.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_PHY_DQ_TIMING(ULONG val);

/**
Set PHY_DQS_TIMING.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_PHY_DQS_TIMING(ULONG val);

/**
Set PHY_GT_LPBK_CTL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_PHY_GT_LPBK_CTL(ULONG val);

/**
Set PHY_DLL_MST_CTL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_PHY_DLL_MCTL(ULONG val);

/**
Set PHY_DLL_SLV_CTL.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_PHY_DLL_SCTL(ULONG val);

/**
Set BOOT_ECC_SEC_SIZ.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_BOOT_ECC_CTL_SEC_SIZ(ULONG val);

/**
Set BT_ECC_CORR_STR.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_BOOT_ECC_CTL_CORR_STR(ULONG val);

/**
Set BOOT_ECC_EN.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_BOOT_ECC_CTL_EN(ULONG val);

/**
Set NFWPX_CNT.<br>
@param[in]  *val Setting value
@retval	None.
*/
VOID Dd_Exs_Set_NF_NFWPX_CNT(ULONG val);

/**
The status of INTSTAT of SD2_RIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_SD2_RIS( VOID );

/**
The status of INTSTAT of SD2_FIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_SD2_FIS( VOID );

/**
The status of INTSTAT of SD3_RIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_SD3_RIS( VOID );

/**
The status of INTSTAT of SD3_FIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_SD3_FIS( VOID );

/**
The status of INTSTAT of SD4_RIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_SD4_RIS( VOID );

/**
The status of INTSTAT of SD4_FIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_SD4_FIS( VOID );

/**
The status of INTSTAT of PCIe0_RIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_PCIE0_RIS( VOID );

/**
The status of INTSTAT of PCIe0_FIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_PCIE0_FIS( VOID );

/**
The status of INTSTAT of PCIe1_RIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_PCIE1_RIS( VOID );

/**
The status of INTSTAT of PCIe1_FIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_PCIE1_FIS( VOID );

/**
The status of INTSTAT of USB2VBUS_RIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_USB2VBUS_RIS( VOID );

/**
The status of INTSTAT of USB2VBUS_FIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_USB2VBUS_FIS( VOID );

/**
The status of INTSTAT of USB2ID_RIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_USB2ID_RIS( VOID );

/**
The status of INTSTAT of USB2ID_FIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_USB2ID_FIS( VOID );

/**
The status of INTSTAT of USB3VBUS_RIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_USB3VBUS_RIS( VOID );

/**
The status of INTSTAT of USB3VBUS_FIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_USB3VBUS_FIS( VOID );

/**
The status of INTSTAT of USB3ID_RIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_USB3ID_RIS( VOID );

/**
The status of INTSTAT of USB3ID_FIS is cleared.<br>
@retval	None.
*/
VOID Dd_Exs_Clear_INTSTAT_USB3ID_FIS( VOID );

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
VOID Dd_EXS_Int_Handler(ULONG *val);


#ifdef __cplusplus
}
#endif

/*@}*/

#endif
