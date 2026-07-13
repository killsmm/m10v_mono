/**
 * @file		dd_gpv.h
 * @brief		GPV (Global Programmers View) driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_gpv
@{
	- @ref dd_gpv_sample_section
*/

#ifndef _DD_GPV_H_
#define _DD_GPV_H_

#include "driver_common.h"
#include "exstop.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Return value of GPV processing result
#define	D_DD_GPV_INPUT_PARAM_ERR				(D_DD_GPV | D_DDIM_INPUT_PARAM_ERROR)		/**< Input Parameter Error */

// Number of REMAP register
#define D_DD_GPV_REMAP_NANDFLASH				(0x01)										/**< REMAP NANDFLASH Register Number */
#define D_DD_GPV_REMAP_SD1						(0x02)										/**< REMAP SD1(eMMC) Register Number */
#define D_DD_GPV_REMAP_SD3						(0x04)										/**< REMAP SD3(UHS2) Register Number */
#define D_DD_GPV_REMAP_PCIE						(0x08)										/**< REMAP PCIe0/PCIe1 Register Number */
#define D_DD_GPV_REMAP_USB3						(0x10)										/**< REMAP USB3.0 Register Number */
#define D_DD_GPV_REMAP_SD_RELC_HDMAC_USB2		(0x20)										/**< REMAP SD2(UHS1 Wifi)/SD4(UHS1 Card)/SD3(UHS1)/RELC/HDMAC/USB2 Register Number */
#define D_DD_GPV_REMAP_NETSEC					(0x40)										/**< REMAP NETSEC Register Number */

// Max value of REMAP
#define D_DD_GPV_REMAP_VAL_MAX					(255)										/**< Remap Max Value */

#define D_DD_GPV_FMIB_VAL_MAX					(3)											/**< FMIB Max Value */
#define D_DD_GPV_FM2_VAL_MAX					(1)											/**< FM2 Max Value */
#define D_DD_GPV_FMA_VAL_MAX					(3)											/**< FMA Max Value */
#define D_DD_GPV_FML_VAL_MAX					(3)											/**< FML Max Value */
#define D_DD_GPV_WT_VAL_MAX						(8)											/**< WT Max Value */
#define D_DD_GPV_AC_VAL_MAX						(3)											/**< AC Max Value */
#define D_DD_GPV_FM_VAL_MAX						(3)											/**< FM Max Value */


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
/** Control information (Master)*/
typedef struct {
	unsigned long		FMIB;			/**< Base_(0008h - 000Bh) fn_mod_iss_bm */
	unsigned long		FM2;			/**< Base_(0024h - 0027h) fn_mod2 */
	unsigned long		FML;			/**< Base_(002Ch - 002Fh) fn_mod_lb */
	unsigned long		WT;				/**< Base_(0040h - 0043h) wr_tidemark */
	unsigned long		AC;				/**< Base_(0044h - 0047h) ahb_cntl */
	unsigned long		FM;				/**< Base_(0108h - 010Bh) fn_mod */
} T_DD_GPV_CTRL_M;

/** Control information (Slave)*/
typedef struct {
	unsigned long		FMA;			/**< Base_(0028h - 002Bh) fn_mod_ahb */
	unsigned long		FM;				/**< Base_(0108h - 010Bh) fn_mod */
} T_DD_GPV_CTRL_S;

/** Control information (IB)*/
typedef struct {
	unsigned long		FMIB;			/**< Base_(0008h - 000Bh) fn_mod_iss_bm */
	unsigned long		FM2;			/**< Base_(0024h - 0027h) fn_mod2 */
	unsigned long		FML;			/**< Base_(002Ch - 002Fh) fn_mod_lb */
	unsigned long		WT;				/**< Base_(0040h - 0043h) wr_tidemark */
	unsigned long		FM;				/**< Base_(0108h - 010Bh) fn_mod */
} T_DD_GPV_CTRL_IB;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* M_Usb3_r_ib Registers */
#define Dd_GPV_Get_M_Usb3_r_ib_FMIB()			(IO_GPV.MUSB3RIB.FMIB)			/**< Get M_Usb3_r_ib fn_mod_iss_bm */
#define Dd_GPV_Get_M_Usb3_r_ib_FM2()			(IO_GPV.MUSB3RIB.FM2)			/**< Get M_Usb3_r_ib fn_mod2 */
#define Dd_GPV_Get_M_Usb3_r_ib_FM()				(IO_GPV.MUSB3RIB.FM)			/**< Get M_Usb3_r_ib fn_mod */

/* M_Emmc_r_ib Registers */
#define Dd_GPV_Get_M_Emmc_r_ib_FMIB()			(IO_GPV.MEMMCRIB.FMIB)			/**< Get M_Emmc_r_ib fn_mod_iss_bm */
#define Dd_GPV_Get_M_Emmc_r_ib_FM2()			(IO_GPV.MEMMCRIB.FM2)			/**< Get M_Emmc_r_ib fn_mod2 */
#define Dd_GPV_Get_M_Emmc_r_ib_FML()			(IO_GPV.MEMMCRIB.FML)			/**< Get M_Emmc_r_ib fn_mod_lb */
#define Dd_GPV_Get_M_Emmc_r_ib_FM()				(IO_GPV.MEMMCRIB.FM)			/**< Get M_Emmc_r_ib fn_mod */

/* M_Uhs2_r_ib Registers */
#define Dd_GPV_Get_M_Uhs2_r_ib_FMIB()			(IO_GPV.MUHS2RIB.FMIB)			/**< Get M_Uhs2_r_ib fn_mod_iss_bm */
#define Dd_GPV_Get_M_Uhs2_r_ib_FM2()			(IO_GPV.MUHS2RIB.FM2)			/**< Get M_Uhs2_r_ib fn_mod2 */
#define Dd_GPV_Get_M_Uhs2_r_ib_FML()			(IO_GPV.MUHS2RIB.FML)			/**< Get M_Uhs2_r_ib fn_mod_lb */
#define Dd_GPV_Get_M_Uhs2_r_ib_FM()				(IO_GPV.MUHS2RIB.FM)			/**< Get M_Uhs2_r_ib fn_mod */

/* M_Mxic1_m_ib Registers */
#define Dd_GPV_Get_M_Mxic1_m_ib_FMIB()			(IO_GPV.MMXIC1MIB.FMIB)			/**< Get M_Mxic1_m_ib fn_mod_iss_bm */
#define Dd_GPV_Get_M_Mxic1_m_ib_WT()			(IO_GPV.MMXIC1MIB.WT)			/**< Get M_Mxic1_m_ib wr_tidemark */
#define Dd_GPV_Get_M_Mxic1_m_ib_FM()			(IO_GPV.MMXIC1MIB.FM)			/**< Get M_Mxic1_m_ib fn_mod */

/* M_Mxic2_m_ib Registers */
#define Dd_GPV_Get_M_Mxic2_m_ib_FMIB()			(IO_GPV.MMXIC2MIB.FMIB)			/**< Get M_Mxic2_m_ib fn_mod_iss_bm */
#define Dd_GPV_Get_M_Mxic2_m_ib_WT()			(IO_GPV.MMXIC2MIB.WT)			/**< Get M_Mxic2_m_ib wr_tidemark */
#define Dd_GPV_Get_M_Mxic2_m_ib_FM()			(IO_GPV.MMXIC2MIB.FM)			/**< Get M_Mxic2_m_ib fn_mod */

/* M_Xdmac_r_ib Registers */
#define Dd_GPV_Get_M_Xdmac_r_ib_FMIB()			(IO_GPV.MXDMACRIB.FMIB)			/**< Get M_Xdmac_r_ib fn_mod_iss_bm */
#define Dd_GPV_Get_M_Xdmac_r_ib_FM2()			(IO_GPV.MXDMACRIB.FM2)			/**< Get M_Xdmac_r_ib fn_mod2 */
#define Dd_GPV_Get_M_Xdmac_r_ib_FM()			(IO_GPV.MXDMACRIB.FM)			/**< Get M_Xdmac_r_ib fn_mod */

/* M_Nf_r_ib Registers */
#define Dd_GPV_Get_M_Nf_r_ib_FMIB()				(IO_GPV.MNFRIB.FMIB)			/**< Get M_Nf_r_ib fn_mod_iss_bm */
#define Dd_GPV_Get_M_Nf_r_ib_FM2()				(IO_GPV.MNFRIB.FM2)				/**< Get M_Nf_r_ib fn_mod2 */
#define Dd_GPV_Get_M_Nf_r_ib_FML()				(IO_GPV.MNFRIB.FML)				/**< Get M_Nf_r_ib fn_mod_lb */
#define Dd_GPV_Get_M_Nf_r_ib_FM()				(IO_GPV.MNFRIB.FM)				/**< Get M_Nf_r_ib fn_mod */

/* M_Pcie0_r_ib Registers */
#define Dd_GPV_Get_M_Pcie0_r_ib_FMIB()			(IO_GPV.MPCIE0RIB.FMIB)			/**< Get M_Pcie0_r_ib fn_mod_iss_bm */
#define Dd_GPV_Get_M_Pcie0_r_ib_FM2()			(IO_GPV.MPCIE0RIB.FM2)			/**< Get M_Pcie0_r_ib fn_mod2 */
#define Dd_GPV_Get_M_Pcie0_r_ib_FML()			(IO_GPV.MPCIE0RIB.FML)			/**< Get M_Pcie0_r_ib fn_mod_lb */
#define Dd_GPV_Get_M_Pcie0_r_ib_FM()			(IO_GPV.MPCIE0RIB.FM)			/**< Get M_Pcie0_r_ib fn_mod */

/* M_Pcie1_r_ib Registers */
#define Dd_GPV_Get_M_Pcie1_r_ib_FMIB()			(IO_GPV.MPCIE1RIB.FMIB)			/**< Get M_Pcie1_r_ib fn_mod_iss_bm */
#define Dd_GPV_Get_M_Pcie1_r_ib_FM2()			(IO_GPV.MPCIE1RIB.FM2)			/**< Get M_Pcie1_r_ib fn_mod2 */
#define Dd_GPV_Get_M_Pcie1_r_ib_FML()			(IO_GPV.MPCIE1RIB.FML)			/**< Get M_Pcie1_r_ib fn_mod_lb */
#define Dd_GPV_Get_M_Pcie1_r_ib_FM()			(IO_GPV.MPCIE1RIB.FM)			/**< Get M_Pcie1_r_ib fn_mod */

/* M_NicPeriAHB_m_ib Registers */
#define Dd_GPV_Get_M_NicPeriAHB_m_ib_FMIB()		(IO_GPV.MNICPERIAHBMIB.FMIB)	/**< Get M_NicPeriAHB_m_ib fn_mod_iss_bm */
#define Dd_GPV_Get_M_NicPeriAHB_m_ib_FM2()		(IO_GPV.MNICPERIAHBMIB.FM2)		/**< Get M_NicPeriAHB_m_ib fn_mod2 */
#define Dd_GPV_Get_M_NicPeriAHB_m_AC()			(IO_GPV.MNICPERIAHBMIB.AC)		/**< Get M_NicPeriAHB_m_ib ahb_cntl */

/* M_NicBmAHB1_r_ib Registers */
#define Dd_GPV_Get_M_NicBmAHB1_r_ib_FMIB()		(IO_GPV.MNICBMAHB1RIB.FMIB)		/**< Get M_NicBmAHB1_r_ib fn_mod_iss_bm */
#define Dd_GPV_Get_M_NicBmAHB1_r_AC()			(IO_GPV.MNICBMAHB1RIB.AC)		/**< Get M_NicBmAHB1_r_ib ahb_cntl */

/* NsecX2P_ib Registers */
#define Dd_GPV_Get_NsecX2P_ib_FMIB()			(IO_GPV.NSECX2PIB.FMIB)			/**< Get NsecX2P_ib fn_mod_iss_bm */
#define Dd_GPV_Get_NsecX2P_ib_FM2()				(IO_GPV.NSECX2PIB.FM2)			/**< Get NsecX2P_ib fn_mod2 */

/* ToprX2P_ib Registers */
#define Dd_GPV_Get_ToprX2P_ib_FMIB()			(IO_GPV.TOPRX2PIB.FMIB)			/**< Get ToprX2P_ib fn_mod_iss_bm */
#define Dd_GPV_Get_ToprX2P_ib_FM2()				(IO_GPV.TOPRX2PIB.FM2)			/**< Get ToprX2P_ib fn_mod2 */

/* M_NicBmAHB2_r_ib Registers */
#define Dd_GPV_Get_M_NicBmAHB2_r_ib_FMIB()		(IO_GPV.MNICBMAHB2RIB.FMIB)		/**< Get M_NicBmAHB2_r_ib fn_mod_iss_bm */
#define Dd_GPV_Get_M_NicBmAHB2_r_ib_FM2()		(IO_GPV.MNICBMAHB2RIB.FM2)		/**< Get M_NicBmAHB2_r_ib fn_mod2 */
#define Dd_GPV_Get_M_NicBmAHB2_r_AC()			(IO_GPV.MNICBMAHB2RIB.AC)		/**< Get M_NicBmAHB2_r_ib ahb_cntl */

/* S_Main_rm_ib Registers */
#define Dd_GPV_Get_S_Main_rm_ib_FM()			(IO_GPV.SMAINRMIB.FM)			/**< Get S_Main_rm_ib fn_mod */

/* S_Xdmac_m_ib Registers */
#define Dd_GPV_Get_S_Xdmac_m_ib_FM()			(IO_GPV.SXDMACMIB.FM)			/**< Get S_Xdmac_m_ib fn_mod */

/* S_Netsec_m Registers */
#define Dd_GPV_Get_S_Netsec_m_FM()				(IO_GPV.SNETSECM.FM)			/**< Get S_Netsec_m fn_mod */

/* S_WifiAHB_m_ib Registers */
#define Dd_GPV_Get_S_WifiAHB_m_FMA()			(IO_GPV.SWIFIAHBMIB.FMA)		/**< Get S_WifiAHB_m_ib fn_mod_ahb */
#define Dd_GPV_Get_S_WifiAHB_m_ib_FM()			(IO_GPV.SWIFIAHBMIB.FM)			/**< Get S_WifiAHB_m_ib fn_mod */

/* S_Uhs2AHB_m Registers */
#define Dd_GPV_Get_S_Uhs2AHB_m_FMA()			(IO_GPV.SUHS2AHBM.FMA)			/**< Get S_Uhs2AHB_m fn_mod_ahb */
#define Dd_GPV_Get_S_Uhs2AHB_m_FM()				(IO_GPV.SUHS2AHBM.FM)			/**< Get S_Uhs2AHB_m fn_mod */

/* S_RelcrAHB_m Registers */
#define Dd_GPV_Get_S_RelcrAHB_m_FMA()			(IO_GPV.SRELCRAHBM.FMA)			/**< Get S_RelcrAHB_m fn_mod_ahb */
#define Dd_GPV_Get_S_RelcrAHB_m_FM()			(IO_GPV.SRELCRAHBM.FM)			/**< Get S_RelcrAHB_m fn_mod */

/* S_Uhs1AHB_m Registers */
#define Dd_GPV_Get_S_Uhs1AHB_m_FMA()			(IO_GPV.SUHS1AHBM.FMA)			/**< Get S_Uhs1AHB_m fn_mod_ahb */
#define Dd_GPV_Get_S_Uhs1AHB_m_FM()				(IO_GPV.SUHS1AHBM.FM)			/**< Get S_Uhs1AHB_m fn_mod */

/* S_RelcwAHB_m Registers */
#define Dd_GPV_Get_S_RelcwAHB_m_FMA()			(IO_GPV.SRELCWAHBM.FMA)			/**< Get S_RelcwAHB_m fn_mod_ahb */
#define Dd_GPV_Get_S_RelcwAHB_m_FM()			(IO_GPV.SRELCWAHBM.FM)			/**< Get S_RelcwAHB_m fn_mod */

/* S_BmNicAHB_m Registers */
#define Dd_GPV_Get_S_BmNicAHB_m_FMA()			(IO_GPV.SBMNICAHBM.FMA)			/**< Get S_BmNicAHB_m fn_mod_ahb */
#define Dd_GPV_Get_S_BmNicAHB_m_FM()			(IO_GPV.SBMNICAHBM.FM)			/**< Get S_BmNicAHB_m fn_mod */

/* S_Usb2AHB_ehci Registers */
#define Dd_GPV_Get_S_Usb2AHB_ehci_FMA()			(IO_GPV.SUSB2AHBEHCI.FMA)		/**< Get S_Usb2AHB_ehci fn_mod_ahb */
#define Dd_GPV_Get_S_Usb2AHB_ehci_FM()			(IO_GPV.SUSB2AHBEHCI.FM)		/**< Get S_Usb2AHB_ehci fn_mod */

/* S_Usb2AHB_ohci Registers */
#define Dd_GPV_Get_S_Usb2AHB_ohci_FMA()			(IO_GPV.SUSB2AHBOHCI.FMA)		/**< Get S_Usb2AHB_ohci fn_mod_ahb */
#define Dd_GPV_Get_S_Usb2AHB_ohci_FM()			(IO_GPV.SUSB2AHBOHCI.FM)		/**< Get S_Usb2AHB_ohci fn_mod */

/* S_Netsec2_m Registers */
#define Dd_GPV_Get_S_Netsec2_m_FM()				(IO_GPV.SNETSEC2M.FM)			/**< Get S_Netsec2_m fn_mod */

/* IB8 Registers */
#define Dd_GPV_Get_IB8_FMIB()					(IO_GPV.IB8.FMIB)				/**< Get IB8 fn_mod_iss_bm */
#define Dd_GPV_Get_IB8_FM2()					(IO_GPV.IB8.FM2)				/**< Get IB8 fn_mod2 */
#define Dd_GPV_Get_IB8_WT()						(IO_GPV.IB8.WT)					/**< Get IB8 wr_tidemark */
#define Dd_GPV_Get_IB8_FM()						(IO_GPV.IB8.FM)					/**< Get IB8 fn_mod */

/* IB13 Registers */
#define Dd_GPV_Get_IB13_FMIB()					(IO_GPV.IB13.FMIB)				/**< Get IB13 fn_mod_iss_bm */
#define Dd_GPV_Get_IB13_FM2()					(IO_GPV.IB13.FM2)				/**< Get IB13 fn_mod2 */
#define Dd_GPV_Get_IB13_FML()					(IO_GPV.IB13.FML)				/**< Get IB13 fn_mod_lb */
#define Dd_GPV_Get_IB13_FM()					(IO_GPV.IB13.FM)				/**< Get IB13 fn_mod */

/* IB12 Registers */
#define Dd_GPV_Get_IB12_FMIB()					(IO_GPV.IB12.FMIB)				/**< Get IB12 fn_mod_iss_bm */
#define Dd_GPV_Get_IB12_FM2()					(IO_GPV.IB12.FM2)				/**< Get IB12 fn_mod2 */
#define Dd_GPV_Get_IB12_FM()					(IO_GPV.IB12.FM)				/**< Get IB12 fn_mod */

/* IB9 Registers */
#define Dd_GPV_Get_IB9_FMIB()					(IO_GPV.IB9.FMIB)				/**< Get IB9 fn_mod_iss_bm */
#define Dd_GPV_Get_IB9_FM2()					(IO_GPV.IB9.FM2)				/**< Get IB9 fn_mod2 */
#define Dd_GPV_Get_IB9_WT()						(IO_GPV.IB9.WT)					/**< Get IB9 wr_tidemark */
#define Dd_GPV_Get_IB9_FM()						(IO_GPV.IB9.FM)					/**< Get IB9 fn_mod */


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
M_Usb3_r_ib register Value is gotten
@param [out]	*gpv_ctrl_m				: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_M_Usb3_r_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m);

/**
M_Emmc_r_ib register Value is gotten
@param [out]	*gpv_ctrl_m				: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_M_Emmc_r_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m);


/**
M_Uhs2_r_ib register Value is gotten
@param [out]	*gpv_ctrl_m				: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_M_Uhs2_r_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m);

/**
M_Mxic1_m_ib register Value is gotten
@param [out]	*gpv_ctrl_m				: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_M_Mxic1_m_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m);

/**
M_Mxic2_m_ib register Value is gotten
@param [out]	*gpv_ctrl_m				: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_M_Mxic2_m_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m);

/**
M_Xdmac_r_ib register Value is gotten
@param [out]	*gpv_ctrl_m				: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_M_Xdmac_r_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m);

/**
M_Nf_r_ib register Value is gotten
@param [out]	*gpv_ctrl_m				: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_M_Nf_r_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m);

/**
M_Pcie0_r_ib register Value is gotten
@param [out]	*gpv_ctrl_m				: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_M_Pcie0_r_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m);

/**
M_Pcie1_r_ib register Value is gotten
@param [out]	*gpv_ctrl_m				: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_M_Pcie1_r_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m);

/**
M_NicPeriAHB_m_ib register Value is gotten
@param [out]	*gpv_ctrl_m				: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_M_NicPeriAHB_m_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m);

/**
M_NicBmAHB1_r_ib register Value is gotten
@param [out]	*gpv_ctrl_m				: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_M_NicBmAHB1_r_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m);

/**
NsecX2P_ib register Value is gotten
@param [out]	*gpv_ctrl_m				: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_NsecX2P_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m);

/**
ToprX2P_ib register Value is gotten
@param [out]	*gpv_ctrl_m				: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_ToprX2P_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m);

/**
M_NicBmAHB2_r_ib register Value is gotten
@param [out]	*gpv_ctrl_m				: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_M_NicBmAHB2_r_ib(T_DD_GPV_CTRL_M* const gpv_ctrl_m);

/**
S_WifiAHB_m_ib register Value is gotten
@param [out]	*gpv_ctrl_s				: Gpv Control Information (Slave)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_S_WifiAHB_m_ib(T_DD_GPV_CTRL_S* const gpv_ctrl_s);

/**
S_Uhs2AHB_m register Value is gotten
@param [out]	*gpv_ctrl_s				: Gpv Control Information (Slave)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_S_Uhs2AHB_m(T_DD_GPV_CTRL_S* const gpv_ctrl_s);

/**
S_RelcrAHB_m register Value is gotten
@param [out]	*gpv_ctrl_s				: Gpv Control Information (Slave)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_S_RelcrAHB_m(T_DD_GPV_CTRL_S* const gpv_ctrl_s);

/**
S_Uhs1AHB_m register Value is gotten
@param [out]	*gpv_ctrl_s				: Gpv Control Information (Slave)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_S_Uhs1AHB_m(T_DD_GPV_CTRL_S* const gpv_ctrl_s);

/**
S_RelcwAHB_m register Value is gotten
@param [out]	*gpv_ctrl_s				: Gpv Control Information (Slave)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_S_RelcwAHB_m(T_DD_GPV_CTRL_S* const gpv_ctrl_s);

/**
S_BmNicAHB_m register Value is gotten
@param [out]	*gpv_ctrl_s				: Gpv Control Information (Slave)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_S_BmNicAHB_m(T_DD_GPV_CTRL_S* const gpv_ctrl_s);

/**
S_Usb2AHB_ehci register Value is gotten
@param [out]	*gpv_ctrl_s				: Gpv Control Information (Slave)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_S_Usb2AHB_ehci(T_DD_GPV_CTRL_S* const gpv_ctrl_s);

/**
S_Usb2AHB_ohci register Value is gotten
@param [out]	*gpv_ctrl_s				: Gpv Control Information (Slave)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_S_Usb2AHB_ohci(T_DD_GPV_CTRL_S* const gpv_ctrl_s);

/**
IB8 register Value is gotten
@param [out]	*gpv_ctrl_ib			: Gpv Control Information (IB)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_IB8(T_DD_GPV_CTRL_IB* const gpv_ctrl_ib);

/**
IB13 register Value is gotten
@param [out]	*gpv_ctrl_ib			: Gpv Control Information (IB)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_IB13(T_DD_GPV_CTRL_IB* const gpv_ctrl_ib);

/**
IB12 register Value is gotten
@param [out]	*gpv_ctrl_ib			: Gpv Control Information (IB)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_IB12(T_DD_GPV_CTRL_IB* const gpv_ctrl_ib);

/**
IB9 register Value is gotten
@param [out]	*gpv_ctrl_ib			: Gpv Control Information (IB)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Get_IB9(T_DD_GPV_CTRL_IB* const gpv_ctrl_ib);

/**
Get to REMAP register.
@retval	gRemap							: REMAP state ( 0 to @ref D_DD_GPV_REMAP_VAL_MAX )
*/
UINT32 Dd_GPV_Get_ARC_Remap(VOID);

/**
Set to REMAP register.
@param [in]	val							: REMAP state ( 0 to @ref D_DD_GPV_REMAP_VAL_MAX )
@param [in]	set							: Set Flag ( 0 or 1 )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_ARC_Remap(ULONG val, UINT32 set);

/**
Set to M_Usb3_r_ib register.
@param [in]	*gpv_ctrl_m					: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Usb3_r_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m);

/**
Set to M_Usb3_r_ib FMIB register.
@param [in]	val							: FMIB ( 0 to @ref D_DD_GPV_FMIB_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Usb3_r_ib_FMIB(ULONG val);

/**
Set to M_Usb3_r_ib FM2 register.
@param [in]	val							: FM2 ( 0 to @ref D_DD_GPV_FM2_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Usb3_r_ib_FM2(ULONG val);

/**
Set to M_Usb3_r_ib FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Usb3_r_ib_FM(ULONG val);

/**
Set to M_Emmc_r_ib register.
@param [in]	*gpv_ctrl_m					: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Emmc_r_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m);

/**
Set to M_Emmc_r_ib FMIB register.
@param [in]	val							: FMIB ( 0 to @ref D_DD_GPV_FMIB_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Emmc_r_ib_FMIB(ULONG val);

/**
Set to M_Emmc_r_ib FM2 register.
@param [in]	val							: FM2 ( 0 to @ref D_DD_GPV_FM2_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Emmc_r_ib_FM2(ULONG val);

/**
Set to M_Emmc_r_ib FML register.
@param [in]	val							: FML ( 0 to @ref D_DD_GPV_FML_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Emmc_r_ib_FML(ULONG val);

/**
Set to M_Emmc_r_ib FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Emmc_r_ib_FM(ULONG val);

/**
Set to M_Uhs2_r_ib register.
@param [in]	*gpv_ctrl_m					: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Uhs2_r_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m);

/**
Set to M_Uhs2_r_ib FMIB register.
@param [in]	val							: FMIB ( 0 to @ref D_DD_GPV_FMIB_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Uhs2_r_ib_FMIB(ULONG val);

/**
Set to M_Uhs2_r_ib FM2 register.
@param [in]	val							: FM2 ( 0 to @ref D_DD_GPV_FM2_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Uhs2_r_ib_FM2(ULONG val);

/**
Set to M_Uhs2_r_ib FML register.
@param [in]	val							: FML ( 0 to @ref D_DD_GPV_FML_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Uhs2_r_ib_FML(ULONG val);

/**
Set to M_Uhs2_r_ib FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Uhs2_r_ib_FM(ULONG val);

/**
Set to M_Mxic1_m_ib register.
@param [in]	*gpv_ctrl_m					: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Mxic1_m_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m);

/**
Set to M_Mxic1_m_ib FMIB register.
@param [in]	val							: FMIB ( 0 to @ref D_DD_GPV_FMIB_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Mxic1_m_ib_FMIB(ULONG val);

/**
Set to M_Mxic1_m_ib WT register.
@param [in]	val							: WT ( 0 to @ref D_DD_GPV_WT_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Mxic1_m_ib_WT(ULONG val);

/**
Set to M_Mxic1_m_ib FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Mxic1_m_ib_FM(ULONG val);

/**
Set to M_Mxic2_m_ib register.
@param [in]	*gpv_ctrl_m					: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Mxic2_m_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m);

/**
Set to M_Mxic2_m_ib FMIB register.
@param [in]	val							: FMIB ( 0 to @ref D_DD_GPV_FMIB_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Mxic2_m_ib_FMIB(ULONG val);

/**
Set to M_Mxic2_m_ib WT register.
@param [in]	val							: WT ( 0 to @ref D_DD_GPV_WT_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Mxic2_m_ib_WT(ULONG val);

/**
Set to M_Mxic2_m_ib FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Mxic2_m_ib_FM(ULONG val);

/**
Set to M_Xdmac_r_ib register.
@param [in]	*gpv_ctrl_m					: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Xdmac_r_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m);

/**
Set to M_Xdmac_r_ib FMIB register.
@param [in]	val							: FMIB ( 0 to @ref D_DD_GPV_FMIB_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Xdmac_r_ib_FMIB(ULONG val);

/**
Set to M_Xdmac_r_ib FM2 register.
@param [in]	val							: FM2 ( 0 to @ref D_DD_GPV_FM2_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Xdmac_r_ib_FM2(ULONG val);

/**
Set to M_Xdmac_r_ib FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Xdmac_r_ib_FM(ULONG val);

/**
Set to M_Nf_r_ib register.
@param [in]	*gpv_ctrl_m					: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Nf_r_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m);

/**
Set to M_Nf_r_ib FMIB register.
@param [in]	val							: FMIB ( 0 to @ref D_DD_GPV_FMIB_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Nf_r_ib_FMIB(ULONG val);

/**
Set to M_Nf_r_ib FM2 register.
@param [in]	val							: FM2 ( 0 to @ref D_DD_GPV_FM2_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Nf_r_ib_FM2(ULONG val);

/**
Set to M_Nf_r_ib FML register.
@param [in]	val							: FML ( 0 to @ref D_DD_GPV_FML_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Nf_r_ib_FML(ULONG val);

/**
Set to M_Nf_r_ib FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Nf_r_ib_FM(ULONG val);

/**
Set to M_Pcie0_r_ib register.
@param [in]	*gpv_ctrl_m					: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Pcie0_r_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m);

/**
Set to M_Pcie0_r_ib FMIB register.
@param [in]	val							: FMIB ( 0 to @ref D_DD_GPV_FMIB_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Pcie0_r_ib_FMIB(ULONG val);

/**
Set to M_Pcie0_r_ib FM2 register.
@param [in]	val							: FM2 ( 0 to @ref D_DD_GPV_FM2_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Pcie0_r_ib_FM2(ULONG val);

/**
Set to M_Pcie0_r_ib FML register.
@param [in]	val							: FML ( 0 to @ref D_DD_GPV_FML_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Pcie0_r_ib_FML(ULONG val);

/**
Set to M_Pcie0_r_ib FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Pcie0_r_ib_FM(ULONG val);

/**
Set to M_Pcie1_r_ib register.
@param [in]	*gpv_ctrl_m					: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Pcie1_r_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m);

/**
Set to M_Pcie1_r_ib FMIB register.
@param [in]	val							: FMIB ( 0 to @ref D_DD_GPV_FMIB_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Pcie1_r_ib_FMIB(ULONG val);

/**
Set to M_Pcie1_r_ib FM2 register.
@param [in]	val							: FM2 ( 0 to @ref D_DD_GPV_FM2_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Pcie1_r_ib_FM2(ULONG val);

/**
Set to M_Pcie1_r_ib FML register.
@param [in]	val							: FML ( 0 to @ref D_DD_GPV_FML_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Pcie1_r_ib_FML(ULONG val);

/**
Set to M_Pcie1_r_ib FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_Pcie1_r_ib_FM(ULONG val);

/**
Set to M_NicPeriAHB_m_ib register.
@param [in]	*gpv_ctrl_m					: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_NicPeriAHB_m_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m);

/**
Set to M_NicPeriAHB_m_ib FMIB register.
@param [in]	val							: FMIB ( 0 to @ref D_DD_GPV_FMIB_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_NicPeriAHB_m_ib_FMIB(ULONG val);

/**
Set to M_NicPeriAHB_m_ib FM2 register.
@param [in]	val							: FM2 ( 0 to @ref D_DD_GPV_FM2_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_NicPeriAHB_m_ib_FM2(ULONG val);

/**
Set to M_NicPeriAHB_m_ib AC register.
@param [in]	val							: AC ( 0 to @ref D_DD_GPV_AC_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_NicPeriAHB_m_AC(ULONG val);

/**
Set to M_NicBmAHB1_r_ib register.
@param [in]	*gpv_ctrl_m					: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_NicBmAHB1_r_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m);

/**
Set to M_NicBmAHB1_r_ib FMIB register.
@param [in]	val							: FMIB ( 0 to @ref D_DD_GPV_FMIB_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_NicBmAHB1_r_ib_FMIB(ULONG val);

/**
Set to M_NicBmAHB1_r_ib AC register.
@param [in]	val							: AC ( 0 to @ref D_DD_GPV_AC_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_NicBmAHB1_r_AC(ULONG val);

/**
Set to NsecX2P_ib FMIB register.
@param [in]	*gpv_ctrl_m					: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_NsecX2P_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m);

/**
Set to NsecX2P_ib FMIB register.
@param [in]	val							: FMIB ( 0 to @ref D_DD_GPV_FMIB_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_NsecX2P_ib_FMIB(ULONG val);

/**
Set to NsecX2P_ib FM2 register.
@param [in]	val							: FM2 ( 0 to @ref D_DD_GPV_FM2_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_NsecX2P_ib_FM2(ULONG val);

/**
Set to ToprX2P_ib FMIB register.
@param [in]	*gpv_ctrl_m					: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_ToprX2P_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m);

/**
Set to ToprX2P_ib FMIB register.
@param [in]	val							: FMIB ( 0 to @ref D_DD_GPV_FMIB_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_ToprX2P_ib_FMIB(ULONG val);

/**
Set to ToprX2P_ib FM2 register.
@param [in]	val							: FM2 ( 0 to @ref D_DD_GPV_FM2_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_ToprX2P_ib_FM2(ULONG val);

/**
Set to M_NicBmAHB2_r_ib FMIB register.
@param [in]	*gpv_ctrl_m					: Gpv Control Information (Master)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_NicBmAHB2_r_ib(T_DD_GPV_CTRL_M const* const gpv_ctrl_m);

/**
Set to M_NicBmAHB2_r_ib FMIB register.
@param [in]	val							: FMIB ( 0 to @ref D_DD_GPV_FMIB_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_NicBmAHB2_r_ib_FMIB(ULONG val);

/**
Set to M_NicBmAHB2_r_ib FM2 register.
@param [in]	val							: FM2 ( 0 to @ref D_DD_GPV_FM2_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_NicBmAHB2_r_ib_FM2(ULONG val);

/**
Set to M_NicBmAHB2_r_ib AC register.
@param [in]	val							: AC ( 0 to @ref D_DD_GPV_AC_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_M_NicBmAHB2_r_AC(ULONG val);

/**
Set to S_Main_rm_ib FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_Main_rm_ib_FM(ULONG val);

/**
Set to S_Xdmac_m_ib FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_Xdmac_m_ib_FM(ULONG val);

/**
Set to S_Netsec_m FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_Netsec_m_FM(ULONG val);

/**
Set to S_WifiAHB_m_ib register.
@param [in]	*gpv_ctrl_s					: Gpv Control Information (Slave)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_WifiAHB_m_ib(T_DD_GPV_CTRL_S const* const gpv_ctrl_s);

/**
Set to S_WifiAHB_m_ib FMA register.
@param [in]	val							: FMA ( 0 to @ref D_DD_GPV_FMA_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_WifiAHB_m_FMA(ULONG val);

/**
Set to S_WifiAHB_m_ib FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_WifiAHB_m_ib_FM(ULONG val);

/**
Set to S_Uhs2AHB_m register.
@param [in]	*gpv_ctrl_s					: Gpv Control Information (Slave)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_Uhs2AHB_m(T_DD_GPV_CTRL_S const* const gpv_ctrl_s);

/**
Set to S_Uhs2AHB_m FMA register.
@param [in]	val							: FMA ( 0 to @ref D_DD_GPV_FMA_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_Uhs2AHB_m_FMA(ULONG val);

/**
Set to S_Uhs2AHB_m FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_Uhs2AHB_m_FM(ULONG val);

/**
Set to S_RelcrAHB_m register.
@param [in]	*gpv_ctrl_s					: Gpv Control Information (Slave)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_RelcrAHB_m(T_DD_GPV_CTRL_S const* const gpv_ctrl_s);

/**
Set to S_RelcrAHB_m FMA register.
@param [in]	val							: FMA ( 0 to @ref D_DD_GPV_FMA_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_RelcrAHB_m_FMA(ULONG val);

/**
Set to S_RelcrAHB_m FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_RelcrAHB_m_FM(ULONG val);

/**
Set to S_Uhs1AHB_m register.
@param [in]	*gpv_ctrl_s					: Gpv Control Information (Slave)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_Uhs1AHB_m(T_DD_GPV_CTRL_S const* const gpv_ctrl_s);

/**
Set to S_Uhs1AHB_m FMA register.
@param [in]	val							: FMA ( 0 to @ref D_DD_GPV_FMA_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_Uhs1AHB_m_FMA(ULONG val);

/**
Set to S_Uhs1AHB_m FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_Uhs1AHB_m_FM(ULONG val);

/**
Set to S_RelcwAHB_m register.
@param [in]	*gpv_ctrl_s					: Gpv Control Information (Slave)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_RelcwAHB_m(T_DD_GPV_CTRL_S const* const gpv_ctrl_s);

/**
Set to S_RelcwAHB_m FMA register.
@param [in]	val							: FMA ( 0 to @ref D_DD_GPV_FMA_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_RelcwAHB_m_FMA(ULONG val);

/**
Set to S_RelcwAHB_m FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_RelcwAHB_m_FM(ULONG val);

/**
Set to S_BmNicAHB_m register.
@param [in]	*gpv_ctrl_s					: Gpv Control Information (Slave)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_BmNicAHB_m(T_DD_GPV_CTRL_S const* const gpv_ctrl_s);

/**
Set to S_BmNicAHB_m FMA register.
@param [in]	val							: FMA ( 0 to @ref D_DD_GPV_FMA_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_BmNicAHB_m_FMA(ULONG val);

/**
Set to S_BmNicAHB_m FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_BmNicAHB_m_FM(ULONG val);

/**
Set to S_Usb2AHB_ehci register.
@param [in]	*gpv_ctrl_s					: Gpv Control Information (Slave)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_Usb2AHB_ehci(T_DD_GPV_CTRL_S const* const gpv_ctrl_s);

/**
Set to S_Usb2AHB_ehci FMA register.
@param [in]	val							: FMA ( 0 to @ref D_DD_GPV_FMA_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_Usb2AHB_ehci_FMA(ULONG val);

/**
Set to S_Usb2AHB_ehci FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_Usb2AHB_ehci_FM(ULONG val);

/**
Set to S_Usb2AHB_ohci register.
@param [in]	*gpv_ctrl_s					: Gpv Control Information (Slave)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_Usb2AHB_ohci(T_DD_GPV_CTRL_S const* const gpv_ctrl_s);

/**
Set to S_Usb2AHB_ohci FMA register.
@param [in]	val							: FMA ( 0 to @ref D_DD_GPV_FMA_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_Usb2AHB_ohci_FMA(ULONG val);

/**
Set to S_Usb2AHB_ohci FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_Usb2AHB_ohci_FM(ULONG val);

/**
Set to S_Netsec2_m FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_S_Netsec2_m_FM(ULONG val);

/**
Set to IB8 register.
@param [in]	*gpv_ctrl_ib				: Gpv Control Information (IB)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_IB8(T_DD_GPV_CTRL_IB const* const gpv_ctrl_ib);

/**
Set to IB8 FMIB register.
@param [in]	val							: FMIB ( 0 to @ref D_DD_GPV_FMIB_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_IB8_FMIB(ULONG val);

/**
Set to IB8 FM2 register.
@param [in]	val							: FM2 ( 0 to @ref D_DD_GPV_FM2_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_IB8_FM2(ULONG val);

/**
Set to IB8 WT register.
@param [in]	val							: WT ( 0 to @ref D_DD_GPV_WT_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_IB8_WT(ULONG val);

/**
Set to IB8 FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_IB8_FM(ULONG val);

/**
Set to IB13 register.
@param [in]	*gpv_ctrl_ib				: Gpv Control Information (IB)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_IB13(T_DD_GPV_CTRL_IB const* const gpv_ctrl_ib);

/**
Set to IB13 FMIB register.
@param [in]	val							: FMIB ( 0 to @ref D_DD_GPV_FMIB_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_IB13_FMIB(ULONG val);

/**
Set to IB13 FM2 register.
@param [in]	val							: FM2 ( 0 to @ref D_DD_GPV_FM2_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_IB13_FM2(ULONG val);

/**
Set to IB13 FML register.
@param [in]	val							: FML ( 0 to @ref D_DD_GPV_FML_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_IB13_FML(ULONG val);

/**
Set to IB13 FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_IB13_FM(ULONG val);

/**
Set to IB12 register.
@param [in]	*gpv_ctrl_ib				: Gpv Control Information (IB)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_IB12(T_DD_GPV_CTRL_IB const* const gpv_ctrl_ib);

/**
Set to IB12 FMIB register.
@param [in]	val							: FMIB ( 0 to @ref D_DD_GPV_FMIB_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_IB12_FMIB(ULONG val);

/**
Set to IB12 FM2 register.
@param [in]	val							: FM2 ( 0 to @ref D_DD_GPV_FM2_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_IB12_FM2(ULONG val);

/**
Set to IB12 FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_IB12_FM(ULONG val);

/**
Set to IB9 register.
@param [in]	*gpv_ctrl_ib				: Gpv Control Information (IB)
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_IB9(T_DD_GPV_CTRL_IB const* const gpv_ctrl_ib);

/**
Set to IB9 FMIB register.
@param [in]	val							: FMIB ( 0 to @ref D_DD_GPV_FMIB_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_IB9_FMIB(ULONG val);

/**
Set to IB9 FM2 register.
@param [in]	val							: FM2 ( 0 to @ref D_DD_GPV_FM2_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_IB9_FM2(ULONG val);

/**
Set to IB9 WT register.
@param [in]	val							: WT ( 0 to @ref D_DD_GPV_WT_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_IB9_WT(ULONG val);

/**
Set to IB9 FM register.
@param [in]	val							: FM ( 0 to @ref D_DD_GPV_FM_VAL_MAX )
@retval	D_DDIM_OK						: Normal end
@retval	D_DD_GPV_INPUT_PARAM_ERR		: Input Parameter Error
*/
INT32 Dd_GPV_Set_IB9_FM(ULONG val);

#ifdef __cplusplus
}
#endif

#endif	// _DD_GPV_H_

/*@}*/

/**
@weakgroup dd_gpv
@{
	@section dd_gpv_sample_section		Sample of "NETSEC and if you want to enable the region NandFlash" using Dd_GPV_Set_ARC_Remap function.
    @code
	//
	// Remap settings(Valid NETSEC Area and Valid NandFlash)
	//
	Dd_GPV_Set_ARC_Remap(D_DD_GPV_REMAP_NETSEC | D_DD_GPV_REMAP_NANDFLASH, 1);
	@endcode
@}*/
