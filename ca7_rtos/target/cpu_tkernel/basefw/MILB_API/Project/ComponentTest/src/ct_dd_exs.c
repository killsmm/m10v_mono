/**
 * @file		ct_dd_exs.c
 * @brief		dd_exs Component Test
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include "driver_common.h"
#include "arm.h"
#include "exstop.h"
//#include "chiptop.h"
#include "dd_gpv.h"
#include "dd_exs.h"
//#include "dd_xdmafasp.h"
//#include "ddim_user_custom.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
static int dat1 = 0;
static ULONG input = 0;

INT32 get_val = 0;
ULONG intst = 0;

/*----------------------------------------------------------------------*/
/* Local Method Definition												*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/

/**
 * @brief  Command main function for EXS test.
 * @param  int argc	:The number of parameters
 *		   char** argv	:The value of parameters
 * @return VOID
 *
 * The meaning of parameters
 * +==========+=====+=================+==================+=======+========================================================+
 * | P0       | P1  | P2              | P3               | P4    | Meaning                                                |
 * +==========+=====+=================+==================+=======+========================================================+
 * | ddexstop | get | xd_lpc          | csysreq          |       | Dd_Exs_Get_XDMAC_LowPower_Ctl_CSYSREQ()                |
 * | ddexstop | get | xd_lpc          | csysack          |       | Dd_Exs_Get_XDMAC_LowPower_Ctl_CSYSACK()                |
 * | ddexstop | get | xd_lpc          | cactrive         |       | Dd_Exs_Get_XDMAC_LowPower_Ctl_CACTIVE()                |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | softreset       | nfrstn           |       | Dd_Exs_Get_nf_rst_n_Soft_Reset()                       |
 * | ddexstop | get | softreset       | nfrstnbch        |       | Dd_Exs_Get_bch_rst_n_Soft_Reset()                      |
 * | ddexstop | get | softreset       | nfrstnreg        |       | Dd_Exs_Get_reg_rst_n_Soft_Reset()                      |
 * | ddexstop | get | softreset       | netsecrst        |       | Dd_Exs_Get_NETSEC_Soft_Reset()                         |
 * | ddexstop | get | softreset       | emcrst           |       | Dd_Exs_Get_eMMC_Soft_Reset()                           |
 * | ddexstop | get | softreset       | relcrst          |       | Dd_Exs_Get_RELC_Soft_Reset()                           |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | softreset2      | debretx_sd3      |       | Dd_Exs_Get_SD3_CD_Debounce_Soft_Reset()                |
 * | ddexstop | get | softreset2      | debretx_sd4      |       | Dd_Exs_Get_SD4_CD_Debounce_Soft_Reset()                |
 * | ddexstop | get | softreset2      | debretx_pcie0    |       | Dd_Exs_Get_PCIE0_CD_Debounce_Soft_Reset()              |
 * | ddexstop | get | softreset2      | debretx_pcie1    |       | Dd_Exs_Get_PCIE1_CD_Debounce_Soft_Reset()              |
 * | ddexstop | get | softreset2      | debretx_usb2vbus |       | Dd_Exs_Get_USB2VBUS_CD_Debounce_Soft_Reset()           |
 * | ddexstop | get | softreset2      | debretx_usb2id   |       | Dd_Exs_Get_USB2ID_CD_Debounce_Soft_Reset()             |
 * | ddexstop | get | softreset2      | debretx_usb3vbus |       | Dd_Exs_Get_USB3VBUS_CD_Debounce_Soft_Reset()           |
 * | ddexstop | get | softreset2      | debretx_usb3id   |       | Dd_Exs_Get_USB3ID_CD_Debounce_Soft_Reset()             |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | intmask         | intmsk02         |       | Dd_Exs_Get_INTMSK_SD3_RIM()                            |
 * | ddexstop | get | intmask         | intmsk03         |       | Dd_Exs_Get_INTMSK_SD3_FIM()                            |
 * | ddexstop | get | intmask         | intmsk04         |       | Dd_Exs_Get_INTMSK_SD4_RIM()                            |
 * | ddexstop | get | intmask         | intmsk05         |       | Dd_Exs_Get_INTMSK_SD4_FIM()                            |
 * | ddexstop | get | intmask         | intmsk06         |       | Dd_Exs_Get_INTMSK_PCIe0_RIM()                          |
 * | ddexstop | get | intmask         | intmsk07         |       | Dd_Exs_Get_INTMSK_PCIe0_FIM()                          |
 * | ddexstop | get | intmask         | intmsk08         |       | Dd_Exs_Get_INTMSK_PCIe1_RIM()                          |
 * | ddexstop | get | intmask         | intmsk09         |       | Dd_Exs_Get_INTMSK_PCIe1_FIM()                          |
 * | ddexstop | get | intmask         | intmsk10         |       | Dd_Exs_Get_INTMSK_USB2VBUS_RIM()                       |
 * | ddexstop | get | intmask         | intmsk11         |       | Dd_Exs_Get_INTMSK_USB2VBUS_FIM()                       |
 * | ddexstop | get | intmask         | intmsk12         |       | Dd_Exs_Get_INTMSK_USB2ID_RIM()                         |
 * | ddexstop | get | intmask         | intmsk13         |       | Dd_Exs_Get_INTMSK_USB2ID_FIM()                         |
 * | ddexstop | get | intmask         | intmsk14         |       | Dd_Exs_Get_INTMSK_USB3VBUS_RIM()                       |
 * | ddexstop | get | intmask         | intmsk15         |       | Dd_Exs_Get_INTMSK_USB3VBUS_FIM()                       |
 * | ddexstop | get | intmask         | intmsk16         |       | Dd_Exs_Get_INTMSK_USB3ID_RIM()                         |
 * | ddexstop | get | intmask         | intmsk17         |       | Dd_Exs_Get_INTMSK_USB3ID_FIM()                         |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | sigmon          | sigmon_sd3       |       | Dd_Exs_Get_SIGMON_SD3_CD()                             |
 * | ddexstop | get | sigmon          | sigmon_sd4       |       | Dd_Exs_Get_SIGMON_SD4_CD()                             |
 * | ddexstop | get | sigmon          | sigmon_pcie0     |       | Dd_Exs_Get_SIGMON_PCIE0_CD()                           |
 * | ddexstop | get | sigmon          | sigmon_pcie1     |       | Dd_Exs_Get_SIGMON_PCIE1_CD()                           |
 * | ddexstop | get | sigmon          | sigmon_usb2vbus  |       | Dd_Exs_Get_SIGMON_USB2VBUS_CD()                        |
 * | ddexstop | get | sigmon          | sigmon_usb2id    |       | Dd_Exs_Get_SIGMON_USB2ID_CD()                          |
 * | ddexstop | get | sigmon          | sigmon_usb3vbus  |       | Dd_Exs_Get_SIGMON_USB3VBUS_CD()                        |
 * | ddexstop | get | sigmon          | sigmon_usb3id    |       | Dd_Exs_Get_SIGMON_USB3ID_CD()                          |
 * | ddexstop | get | sigmon          | sigmon2_sd3      |       | Dd_Exs_Get_SIGMON2_SD3_CD()                            |
 * | ddexstop | get | sigmon          | sigmon2_sd4      |       | Dd_Exs_Get_SIGMON2_SD4_CD()                            |
 * | ddexstop | get | sigmon          | sigmon2_pcie0    |       | Dd_Exs_Get_SIGMON2_PCIE0_CD()                          |
 * | ddexstop | get | sigmon          | sigmon2_pcie1    |       | Dd_Exs_Get_SIGMON2_PCIE1_CD()                          |
 * | ddexstop | get | sigmon          | sigmon2_usb2vbus |       | Dd_Exs_Get_SIGMON2_USB2VBUS_CD()                       |
 * | ddexstop | get | sigmon          | sigmon2_usb2id   |       | Dd_Exs_Get_SIGMON2_USB2ID_CD()                         |
 * | ddexstop | get | sigmon          | sigmon2_usb3vbus |       | Dd_Exs_Get_SIGMON2_USB3VBUS_CD()                       |
 * | ddexstop | get | sigmon          | sigmon2_usb3id   |       | Dd_Exs_Get_SIGMON2_USB3ID_CD()                         |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | deben           | deben_sd3        |       | Dd_Exs_Get_DEBEN_SD3_EN()                              |
 * | ddexstop | get | deben           | deben_sd4        |       | Dd_Exs_Get_DEBEN_SD4_EN()                              |
 * | ddexstop | get | deben           | deben_pcie0      |       | Dd_Exs_Get_DEBEN_PCIE0_EN()                            |
 * | ddexstop | get | deben           | deben_pcie1      |       | Dd_Exs_Get_DEBEN_PCIE1_EN()                            |
 * | ddexstop | get | deben           | deben_usb2vbus   |       | Dd_Exs_Get_DEBEN_USB2VBUS_EN()                         |
 * | ddexstop | get | deben           | deben_usb2id     |       | Dd_Exs_Get_DEBEN_USB2ID_EN()                           |
 * | ddexstop | get | deben           | deben_usb3vbus   |       | Dd_Exs_Get_DEBEN_USB3VBUS_EN()                         |
 * | ddexstop | get | deben           | deben_usb3id     |       | Dd_Exs_Get_DEBEN_USB3ID_EN()                           |
 * | ddexstop | get | deben           | sd3detim         |       | Dd_Exs_Get_DEBEN_SD3DETIM()                            |
 * | ddexstop | get | deben           | sd4detim         |       | Dd_Exs_Get_DEBEN_SD4DETIM()                            |
 * | ddexstop | get | deben           | pcie0detim       |       | Dd_Exs_Get_DEBEN_PCIE0DETIM()                          |
 * | ddexstop | get | deben           | pcie1detim       |       | Dd_Exs_Get_DEBEN_PCIE1DETIM()                          |
 * | ddexstop | get | deben           | usb2vbdetim      |       | Dd_Exs_Get_DEBEN_USB2VBUSDETIM()                       |
 * | ddexstop | get | deben           | usb2iddetim      |       | Dd_Exs_Get_DEBEN_USB2IDDETIM()                         |
 * | ddexstop | get | deben           | usb3vbdetim      |       | Dd_Exs_Get_DEBEN_USB3VBUSDETIM()                       |
 * | ddexstop | get | deben           | usb3iddetim      |       | Dd_Exs_Get_DEBEN_USB3IDDETIM()                         |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | cdinsmdctl      | cdmod_sd3        |       | Dd_Exs_Get_CDINS_SD3_MOD()                             |
 * | ddexstop | get | cdinsmdctl      | cdmod_sd4        |       | Dd_Exs_Get_CDINS_SD4_MOD()                             |
 * | ddexstop | get | cdinsmdctl      | cdmod_pcie0      |       | Dd_Exs_Get_CDINS_PCIE0_MOD()                           |
 * | ddexstop | get | cdinsmdctl      | cdmod_pcie1      |       | Dd_Exs_Get_CDINS_PCIE1_MOD()                           |
 * | ddexstop | get | cdinsmdctl      | cdmod_usb2vbus   |       | Dd_Exs_Get_CDINS_USB2VBUS_MOD()                        |
 * | ddexstop | get | cdinsmdctl      | cdmod_usb2id     |       | Dd_Exs_Get_CDINS_USB2ID_MOD()                          |
 * | ddexstop | get | cdinsmdctl      | cdmod_usb3vbus   |       | Dd_Exs_Get_CDINS_USB3VBUS_MOD()                        |
 * | ddexstop | get | cdinsmdctl      | cdmod_usb3id     |       | Dd_Exs_Get_CDINS_USB3ID_MOD()                          |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | rlc_hpt_ctl     | rerhpt           |       | Dd_Exs_Get_RELC_HPROT_Read()                           |
 * | ddexstop | get | rlc_hpt_ctl     | rewhpt           |       | Dd_Exs_Get_RELC_HPROT_Write()                          |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | ram_pd          | rampd_sd2        |       | Dd_Exs_Get_RAM_PD_Ctrl_SD2()                           |
 * | ddexstop | get | ram_pd          | rampd_sd3        |       | Dd_Exs_Get_RAM_PD_Ctrl_SD3()                           |
 * | ddexstop | get | ram_pd          | rampd_sd4        |       | Dd_Exs_Get_RAM_PD_Ctrl_SD4()                           |
 * | ddexstop | get | ram_pd          | rampd_nf         |       | Dd_Exs_Get_RAM_PD_Ctrl_NF()                            |
 * | ddexstop | get | ram_pd          | rampd_relc       |       | Dd_Exs_Get_RAM_PD_Ctrl_RELC()                          |
 * | ddexstop | get | ram_pd          | rampd_netsec     |       | Dd_Exs_Get_RAM_PD_Ctrl_NETSEC()                        |
 * | ddexstop | get | ram_pd          | rampd_usb2       |       | Dd_Exs_Get_RAM_PD_Ctrl_USB2()                          |
 * | ddexstop | get | ram_pd          | rampd_usb3       |       | Dd_Exs_Get_RAM_PD_Ctrl_USB3()                          |
 * | ddexstop | get | ram_pd          | rampd_pcie0      |       | Dd_Exs_Get_RAM_PD_Ctrl_PCIE0()                         |
 * | ddexstop | get | ram_pd          | rampd_pcie1      |       | Dd_Exs_Get_RAM_PD_Ctrl_PCIE1()                         |
 * | ddexstop | get | ram_pd          | rampd_emmc       |       | Dd_Exs_Get_RAM_PD_Ctrl_EMMC()                          |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | busckstp        | nf_ckstp         |       | Dd_Exs_Get_NF_BUS_CLOCK_Stop_Ctrl()                    |
 * | ddexstop | get | busckstp        | pcie0_ckstp      |       | Dd_Exs_Get_PCIE0_BUS_CLOCK_Stop_Ctrl()                 |
 * | ddexstop | get | busckstp        | pcie1_ckstp      |       | Dd_Exs_Get_PCIE1_BUS_CLOCK_Stop_Ctrl()                 |
 * | ddexstop | get | busckstp        | xdmac_ckstp      |       | Dd_Exs_Get_XDMAC_BUS_CLOCK_Stop_Ctrl()                 |
 * | ddexstop | get | busckstp        | hdmac_ckstp      |       | Dd_Exs_Get_HDMAC_BUS_CLOCK_Stop_Ctrl()                 |
 * | ddexstop | get | busckstp        | emmc_ckstp       |       | Dd_Exs_Get_EMMC_BUS_CLOCK_Stop_Ctrl()                  |
 * | ddexstop | get | busckstp        | sd2_ckstp        |       | Dd_Exs_Get_SD2_BUS_CLOCK_Stop_Ctrl()                   |
 * | ddexstop | get | busckstp        | sd3_ckstp        |       | Dd_Exs_Get_SD3_BUS_CLOCK_Stop_Ctrl()                   |
 * | ddexstop | get | busckstp        | sd4_ckstp        |       | Dd_Exs_Get_SD4_BUS_CLOCK_Stop_Ctrl()                   |
 * | ddexstop | get | busckstp        | netsec_ckstp     |       | Dd_Exs_Get_NETSEC_BUS_CLOCK_Stop_Ctrl()                |
 * | ddexstop | get | busckstp        | relc_ckstp       |       | Dd_Exs_Get_RELC_BUS_CLOCK_Stop_Ctrl()                  |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | nsec_ckstp      | netsec_ptpstp    |       | Dd_Exs_Get_NETSEC_PTPCLK_Stop_Ctrl_NETSEC_PTPSTP()     |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | emmc_ckstp      | emclkstp         |       | Dd_Exs_Get_EMMC_CKSTP_EMCLKSTP()                       |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | em_drvstr       | em_dsel          |       | Dd_Exs_Get_EM_DRVSTR()                                 |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | em_pv_dtimec    | pv_dtimec        |       | Dd_Exs_Get_EM_PV_DTIMEC()                              |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | em_pv_ampbl     | pv_ampbl         |       | Dd_Exs_Get_EM_PV_AMPBL()                               |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | em_cr_slottype  | cr_slottype      |       | Dd_Exs_Get_EM_CR_SLOTTYPE()                            |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | em_cr_bclkfreq  | cr_bclkfreq      |       | Dd_Exs_Get_EM_CR_BCLKFREQ()                            |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | em_cddet        | emcd             |       | Dd_Exs_Get_EM_CDDET_EMCD()                             |
 * | ddexstop | get | em_cddet        | emphylk          |       | Dd_Exs_Get_EM_CDDET_EMPHYLK()                          |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | nf_ip_ctl       | dscv_inhibit     |       | Dd_Exs_Get_NF_IP_CTL_DSCV_INHIBIT()                    |
 * | ddexstop | get | nf_ip_ctl       | dscv_ignr_crc    |       | Dd_Exs_Get_NF_IP_CTL_DSCV_IGNR_CRC()                   |
 * | ddexstop | get | nf_ip_ctl       | boot_en          |       | Dd_Exs_Get_NF_IP_CTL_BOOT_EN()                         |
 * | ddexstop | get | nf_ip_ctl       | iddq_en          |       | Dd_Exs_Get_NF_IP_CTL_IDDQ_EN()                         |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | nf_btsq_state   | init_fail        |       | Dd_Exs_Get_NF_BTSQ_STATE_INIT_FAIL()                   |
 * | ddexstop | get | nf_btsq_state   | boot_comp        |       | Dd_Exs_Get_NF_BTSQ_STATE_BOOT_COMP()                   |
 * | ddexstop | get | nf_btsq_state   | boot_error       |       | Dd_Exs_Get_NF_BTSQ_STATE_BOOT_ERROR()                  |
 * | ddexstop | get | nf_btsq_state   | boot_busy        |       | Dd_Exs_Get_NF_BTSQ_STATE_CTRL_BUSY()                   |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | nf_rb_valid_tm  | rb_valid_time    |       | Dd_Exs_Get_NF_RB_VALID_TM()                            |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | nf_phy_ctrl_reg | phy_ctrl         |       | Dd_Exs_Get_NF_PHY_CTRL_REG()                           |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | nf_phy_dq_tim   | phy_dq_timing    |       | Dd_Exs_Get_NF_PHY_DQ_TIMING()                          |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | nf_phy_dqs_tim  | phy_dqs_timing   |       | Dd_Exs_Get_NF_PHY_DQS_TIMING()                         |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | nf_phy_gt_lpbk  | phy_gt_lpbk_ctl  |       | Dd_Exs_Get_NF_PHY_GT_LPBK_CTL()                        |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | nf_phy_dll_mctl | phy_dll_mst_ctl  |       | Dd_Exs_Get_NF_PHY_DLL_MCTL()                           |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | nf_phy_dll_sctl | phy_dll_slv_ctl  |       | Dd_Exs_Get_NF_PHY_DLL_SCTL()                           |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | nf_boot_ecc_ctl | boot_ecc_sec_siz |       | Dd_Exs_Get_NF_BOOT_ECC_CTL_SEC_SIZ()                   |
 * | ddexstop | get | nf_boot_ecc_ctl | bt_ecc_corr_str  |       | Dd_Exs_Get_NF_BOOT_ECC_CTL_CORR_STR()                  |
 * | ddexstop | get | nf_boot_ecc_ctl | boot_ecc_en      |       | Dd_Exs_Get_NF_BOOT_ECC_CTL_EN()                        |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | nf_wpx_cnt      | wpx_cnt          |       | Dd_Exs_Get_NF_NFWPX_CNT()                              |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | get | intstat         | sd2_ris          |       | Dd_Exs_Get_INTSTAT_SD2_RIS()                           |
 * | ddexstop | get | intstat         | sd2_fis          |       | Dd_Exs_Get_INTSTAT_SD2_FIS()                           |
 * | ddexstop | get | intstat         | sd3_ris          |       | Dd_Exs_Get_INTSTAT_SD3_RIS()                           |
 * | ddexstop | get | intstat         | sd3_fis          |       | Dd_Exs_Get_INTSTAT_SD3_FIS()                           |
 * | ddexstop | get | intstat         | sd4_ris          |       | Dd_Exs_Get_INTSTAT_SD4_RIS()                           |
 * | ddexstop | get | intstat         | sd4_fis          |       | Dd_Exs_Get_INTSTAT_SD4_FIS()                           |
 * | ddexstop | get | intstat         | pcie0_ris        |       | Dd_Exs_Get_INTSTAT_PCIe0_RIS()                         |
 * | ddexstop | get | intstat         | pcie0_fis        |       | Dd_Exs_Get_INTSTAT_PCIe0_FIS()                         |
 * | ddexstop | get | intstat         | pcie1_ris        |       | Dd_Exs_Get_INTSTAT_PCIe1_RIS()                         |
 * | ddexstop | get | intstat         | pcie1_fis        |       | Dd_Exs_Get_INTSTAT_PCIe1_FIS()                         |
 * | ddexstop | get | intstat         | usb2vbus_ris     |       | Dd_Exs_Get_INTSTAT_USB2VBUS_RIS()                      |
 * | ddexstop | get | intstat         | usb2vbus_fis     |       | Dd_Exs_Get_INTSTAT_USB2VBUS_FIS()                      |
 * | ddexstop | get | intstat         | usb2id_ris       |       | Dd_Exs_Get_INTSTAT_USB2ID_RIS()                        |
 * | ddexstop | get | intstat         | usb2id_fis       |       | Dd_Exs_Get_INTSTAT_USB2ID_FIS()                        |
 * | ddexstop | get | intstat         | usb3vbus_ris     |       | Dd_Exs_Get_INTSTAT_USB2VBUS_RIS()                      |
 * | ddexstop | get | intstat         | usb3vbus_fis     |       | Dd_Exs_Get_INTSTAT_USB2VBUS_FIS()                      |
 * | ddexstop | get | intstat         | usb3id_ris       |       | Dd_Exs_Get_INTSTAT_USB2ID_RIS()                        |
 * | ddexstop | get | intstat         | usb3id_fis       |       | Dd_Exs_Get_INTSTAT_USB2ID_FIS()                        |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | xd_lpc          | csysreq          | [val] | Dd_Exs_Set_XDMAC_LowPower_Ctl_CSYSREQ(val)             |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | softreset       | nfrstn           | [val] | Dd_Exs_Set_nf_rst_n_Soft_Reset(val)                    |
 * | ddexstop | set | softreset       | nfrstnbch        | [val] | Dd_Exs_Set_bch_rst_n_Soft_Reset(val)                   |
 * | ddexstop | set | softreset       | nfrstnreg        | [val] | Dd_Exs_Set_reg_rst_n_Soft_Reset(val)                   |
 * | ddexstop | set | softreset       | netsecrst        | [val] | Dd_Exs_Set_NETSEC_Soft_Reset(val)                      |
 * | ddexstop | set | softreset       | emcrst           | [val] | Dd_Exs_Set_eMMC_Soft_Reset(val)                        |
 * | ddexstop | set | softreset       | relcrst          | [val] | Dd_Exs_Set_RELC_Soft_Reset(val)                        |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | softreset2      | debretx_sd3      | [val] | Dd_Exs_Set_SD3_CD_Debounce_Soft_Reset(val)             |
 * | ddexstop | set | softreset2      | debretx_sd4      | [val] | Dd_Exs_Set_SD4_CD_Debounce_Soft_Reset(val)             |
 * | ddexstop | set | softreset2      | debretx_pcie0    | [val] | Dd_Exs_Set_PCIE0_CD_Debounce_Soft_Reset(val)           |
 * | ddexstop | set | softreset2      | debretx_pcie1    | [val] | Dd_Exs_Set_PCIE1_CD_Debounce_Soft_Reset(val)           |
 * | ddexstop | set | softreset2      | debretx_usb2vbus | [val] | Dd_Exs_Set_USB2VBUS_CD_Debounce_Soft_Reset(val)        |
 * | ddexstop | set | softreset2      | debretx_usb2id   | [val] | Dd_Exs_Set_USB2ID_CD_Debounce_Soft_Reset(val)          |
 * | ddexstop | set | softreset2      | debretx_usb3vbus | [val] | Dd_Exs_Set_USB3VBUS_CD_Debounce_Soft_Reset(val)        |
 * | ddexstop | set | softreset2      | debretx_usb3id   | [val] | Dd_Exs_Set_USB3ID_CD_Debounce_Soft_Reset(val)          |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | intmask         | intmsk02         | [val] | Dd_Exs_Set_INTMSK_SD3_RIM(val)                         |
 * | ddexstop | set | intmask         | intmsk03         | [val] | Dd_Exs_Set_INTMSK_SD3_FIM(val)                         |
 * | ddexstop | set | intmask         | intmsk04         | [val] | Dd_Exs_Set_INTMSK_SD4_RIM(val)                         |
 * | ddexstop | set | intmask         | intmsk05         | [val] | Dd_Exs_Set_INTMSK_SD4_FIM(val)                         |
 * | ddexstop | set | intmask         | intmsk06         | [val] | Dd_Exs_Set_INTMSK_PCIe0_RIM(val)                       |
 * | ddexstop | set | intmask         | intmsk07         | [val] | Dd_Exs_Set_INTMSK_PCIe0_FIM(val)                       |
 * | ddexstop | set | intmask         | intmsk08         | [val] | Dd_Exs_Set_INTMSK_PCIe1_RIM(val)                       |
 * | ddexstop | set | intmask         | intmsk09         | [val] | Dd_Exs_Set_INTMSK_PCIe1_FIM(val)                       |
 * | ddexstop | set | intmask         | intmsk10         | [val] | Dd_Exs_Set_INTMSK_USB2VBUS_RIM(val)                    |
 * | ddexstop | set | intmask         | intmsk11         | [val] | Dd_Exs_Set_INTMSK_USB2VBUS_FIM(val)                    |
 * | ddexstop | set | intmask         | intmsk12         | [val] | Dd_Exs_Set_INTMSK_USB2ID_RIM(val)                      |
 * | ddexstop | set | intmask         | intmsk13         | [val] | Dd_Exs_Set_INTMSK_USB2ID_FIM(val)                      |
 * | ddexstop | set | intmask         | intmsk14         | [val] | Dd_Exs_Set_INTMSK_USB3VBUS_RIM(val)                    |
 * | ddexstop | set | intmask         | intmsk15         | [val] | Dd_Exs_Set_INTMSK_USB3VBUS_FIM(val)                    |
 * | ddexstop | set | intmask         | intmsk16         | [val] | Dd_Exs_Set_INTMSK_USB3ID_RIM(val)                      |
 * | ddexstop | set | intmask         | intmsk17         | [val] | Dd_Exs_Set_INTMSK_USB3ID_FIM(val)                      |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | deben           | deben_sd3        | [val] | Dd_Exs_Set_DEBEN_SD3_EN(val)                           |
 * | ddexstop | set | deben           | deben_sd4        | [val] | Dd_Exs_Set_DEBEN_SD4_EN(val)                           |
 * | ddexstop | set | deben           | deben_pcie0      | [val] | Dd_Exs_Set_DEBEN_PCIE0_EN(val)                         |
 * | ddexstop | set | deben           | deben_pcie1      | [val] | Dd_Exs_Set_DEBEN_PCIE1_EN(val)                         |
 * | ddexstop | set | deben           | deben_usb2vbus   | [val] | Dd_Exs_Set_DEBEN_USB2VBUS_EN(val)                      |
 * | ddexstop | set | deben           | deben_usb2id     | [val] | Dd_Exs_Set_DEBEN_USB2ID_EN(val)                        |
 * | ddexstop | set | deben           | deben_usb3vbus   | [val] | Dd_Exs_Set_DEBEN_USB3VBUS_EN(val)                      |
 * | ddexstop | set | deben           | deben_usb3id     | [val] | Dd_Exs_Set_DEBEN_USB3ID_EN(val)                        |
 * | ddexstop | set | deben           | sd3detim         | [val] | Dd_Exs_Set_DEBEN_SD3DETIM(val)                         |
 * | ddexstop | set | deben           | sd4detim         | [val] | Dd_Exs_Set_DEBEN_SD4DETIM(val)                         |
 * | ddexstop | set | deben           | pcie0detim       | [val] | Dd_Exs_Set_DEBEN_PCIE0DETIM(val)                       |
 * | ddexstop | set | deben           | pcie1detim       | [val] | Dd_Exs_Set_DEBEN_PCIE1DETIM(val)                       |
 * | ddexstop | set | deben           | usb2vbdetim      | [val] | Dd_Exs_Set_DEBEN_USB2VBUSDETIM(val)                    |
 * | ddexstop | set | deben           | usb2iddetim      | [val] | Dd_Exs_Set_DEBEN_USB2IDDETIM(val)                      |
 * | ddexstop | set | deben           | usb3vbdetim      | [val] | Dd_Exs_Set_DEBEN_USB3VBUSDETIM(val)                    |
 * | ddexstop | set | deben           | usb3iddetim      | [val] | Dd_Exs_Set_DEBEN_USB3IDDETIM(val)                      |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | cdinsmdctl      | cdmod_sd3        | [val] | Dd_Exs_Set_CDINS_SD3_MOD(val)                          |
 * | ddexstop | set | cdinsmdctl      | cdmod_sd4        | [val] | Dd_Exs_Set_CDINS_SD4_MOD(val)                          |
 * | ddexstop | set | cdinsmdctl      | cdmod_pcie0      | [val] | Dd_Exs_Set_CDINS_PCIE0_MOD(val)                        |
 * | ddexstop | set | cdinsmdctl      | cdmod_pcie1      | [val] | Dd_Exs_Set_CDINS_PCIE1_MOD(val)                        |
 * | ddexstop | set | cdinsmdctl      | cdmod_usb2vbus   | [val] | Dd_Exs_Set_CDINS_USB2VBUS_MOD(val)                     |
 * | ddexstop | set | cdinsmdctl      | cdmod_usb2id     | [val] | Dd_Exs_Set_CDINS_USB2ID_MOD(val)                       |
 * | ddexstop | set | cdinsmdctl      | cdmod_usb3vbus   | [val] | Dd_Exs_Set_CDINS_USB3VBUS_MOD(val)                     |
 * | ddexstop | set | cdinsmdctl      | cdmod_usb3id     | [val] | Dd_Exs_Set_CDINS_USB3ID_MOD(val)                       |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | rlc_hpt_ctl     | rerhpt           | [val] | Dd_Exs_Set_RELC_HPROT_Read(val)                        |
 * | ddexstop | set | rlc_hpt_ctl     | rewhpt           | [val] | Dd_Exs_Set_RELC_HPROT_Write(val)                       |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | ram_pd          | rampd_sd2        | [val] | Dd_Exs_Set_RAM_PD_Ctrl_SD2(val)                        |
 * | ddexstop | set | ram_pd          | rampd_sd3        | [val] | Dd_Exs_Set_RAM_PD_Ctrl_SD3(val)                        |
 * | ddexstop | set | ram_pd          | rampd_sd4        | [val] | Dd_Exs_Set_RAM_PD_Ctrl_SD4(val)                        |
 * | ddexstop | set | ram_pd          | rampd_nf         | [val] | Dd_Exs_Set_RAM_PD_Ctrl_NF(val)                         |
 * | ddexstop | set | ram_pd          | rampd_relc       | [val] | Dd_Exs_Set_RAM_PD_Ctrl_RELC(val)                       |
 * | ddexstop | set | ram_pd          | rampd_netsec     | [val] | Dd_Exs_Set_RAM_PD_Ctrl_NETSEC(val)                     |
 * | ddexstop | set | ram_pd          | rampd_usb2       | [val] | Dd_Exs_Set_RAM_PD_Ctrl_USB2(val)                       |
 * | ddexstop | set | ram_pd          | rampd_usb3       | [val] | Dd_Exs_Set_RAM_PD_Ctrl_USB3(val)                       |
 * | ddexstop | set | ram_pd          | rampd_pcie0      | [val] | Dd_Exs_Set_RAM_PD_Ctrl_PCIE0(val)                      |
 * | ddexstop | set | ram_pd          | rampd_pcie1      | [val] | Dd_Exs_Set_RAM_PD_Ctrl_PCIE1(val)                      |
 * | ddexstop | set | ram_pd          | rampd_emmc       | [val] | Dd_Exs_Set_RAM_PD_Ctrl_EMMC(val)                       |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | busckstp        | nf_ckstp         | [val] | Dd_Exs_Set_NF_BUS_CLOCK_Stop_Ctrl(val)                 |
 * | ddexstop | set | busckstp        | pcie0_ckstp      | [val] | Dd_Exs_Set_PCIE0_BUS_CLOCK_Stop_Ctrl(val)              |
 * | ddexstop | set | busckstp        | pcie1_ckstp      | [val] | Dd_Exs_Set_PCIE1_BUS_CLOCK_Stop_Ctrl(val)              |
 * | ddexstop | set | busckstp        | xdmac_ckstp      | [val] | Dd_Exs_Set_XDMAC_BUS_CLOCK_Stop_Ctrl(val)              |
 * | ddexstop | set | busckstp        | hdmac_ckstp      | [val] | Dd_Exs_Set_HDMAC_BUS_CLOCK_Stop_Ctrl(val)              |
 * | ddexstop | set | busckstp        | emmc_ckstp       | [val] | Dd_Exs_Set_EMMC_BUS_CLOCK_Stop_Ctrl(val)               |
 * | ddexstop | set | busckstp        | sd2_ckstp        | [val] | Dd_Exs_Set_SD2_BUS_CLOCK_Stop_Ctrl(val)                |
 * | ddexstop | set | busckstp        | sd3_ckstp        | [val] | Dd_Exs_Set_SD3_BUS_CLOCK_Stop_Ctrl(val)                |
 * | ddexstop | set | busckstp        | sd4_ckstp        | [val] | Dd_Exs_Set_SD4_BUS_CLOCK_Stop_Ctrl(val)                |
 * | ddexstop | set | busckstp        | netsec_ckstp     | [val] | Dd_Exs_Set_NETSEC_BUS_CLOCK_Stop_Ctrl(val)             |
 * | ddexstop | set | busckstp        | relc_ckstp       | [val] | Dd_Exs_Set_RELC_BUS_CLOCK_Stop_Ctrl(val)               |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | nsec_ckstp      | netsec_ptpstp    | [val] | Dd_Exs_Set_NETSEC_PTPCLK_Stop_Ctrl_NETSEC_PTPSTP(val)  |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | emmc_ckstp      | emclkstp         | [val] | Dd_Exs_Set_EMMC_CKSTP_EMCLKSTP(val)                    |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | em_pv_dtimec    | pv_dtimec        | [val] | Dd_Exs_Set_EM_PV_DTIMEC(val)                           |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | em_pv_ampbl     | pv_ampbl         | [val] | Dd_Exs_Set_EM_PV_AMPBL(val)                            |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | em_cr_slottype  | cr_slottype      | [val] | Dd_Exs_Set_EM_CR_SLOTTYPE(val)                         |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | em_cr_bclkfreq  | cr_bclkfreq      | [val] | Dd_Exs_Set_EM_CR_BCLKFREQ(val)                         |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | em_cddet        | emcd             | [val] | Dd_Exs_Set_EM_CDDET_EMCD(val)                          |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | nf_ip_ctl       | dscv_inhibit     | [val] | Dd_Exs_Set_NF_IP_CTL_DSCV_INHIBIT(val)                 |
 * | ddexstop | set | nf_ip_ctl       | dscv_ignr_crc    | [val] | Dd_Exs_Set_NF_IP_CTL_DSCV_IGNR_CRC(val)                |
 * | ddexstop | set | nf_ip_ctl       | boot_en          | [val] | Dd_Exs_Set_NF_IP_CTL_BOOT_EN(val)                      |
 * | ddexstop | set | nf_ip_ctl       | iddq_en          | [val] | Dd_Exs_Set_NF_IP_CTL_IDDQ_EN(val)                      |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | nf_rb_valid_tm  | rb_valid_time    | [val] | Dd_Exs_Set_NF_RB_VALID_TM(val)                         |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | nf_phy_ctrl_reg | phy_ctrl         | [val] | Dd_Exs_Set_NF_PHY_CTRL_REG(val)                        |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | nf_phy_dq_tim   | phy_dq_timing    | [val] | Dd_Exs_Set_NF_PHY_DQ_TIMING(val)                       |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | nf_phy_dqs_tim  | phy_dqs_timing   | [val] | Dd_Exs_Set_NF_PHY_DQS_TIMING(val)                      |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | nf_phy_gt_lpbk  | phy_gt_lpbk_ctl  | [val] | Dd_Exs_Set_NF_PHY_GT_LPBK_CTL(val)                     |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | nf_phy_dll_mctl | phy_dll_mst_ctl  | [val] | Dd_Exs_Set_NF_PHY_DLL_MCTL(val)                        |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | nf_phy_dll_sctl | phy_dll_slv_ctl  | [val] | Dd_Exs_Set_NF_PHY_DLL_SCTL(val)                        |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | nf_boot_ecc_ctl | boot_ecc_sec_siz | [val] | Dd_Exs_Set_NF_BOOT_ECC_CTL_SEC_SIZ(val)                |
 * | ddexstop | set | nf_boot_ecc_ctl | bt_ecc_corr_str  | [val] | Dd_Exs_Set_NF_BOOT_ECC_CTL_CORR_STR(val)               |
 * | ddexstop | set | nf_boot_ecc_ctl | boot_ecc_en      | [val] | Dd_Exs_Set_NF_BOOT_ECC_CTL_EN(val)                     |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | set | nf_wpx_cnt      | wpx_cnt          | [val] | Dd_Exs_Set_NF_NFWPX_CNT(val)                           |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | clr | intstat         | sd2_ris          |       | Dd_Exs_Get_INTSTAT_SD2_RIS()                           |
 * | ddexstop | clr | intstat         | sd2_fis          |       | Dd_Exs_Get_INTSTAT_SD2_FIS()                           |
 * | ddexstop | clr | intstat         | sd3_ris          |       | Dd_Exs_Get_INTSTAT_SD3_RIS()                           |
 * | ddexstop | clr | intstat         | sd3_fis          |       | Dd_Exs_Get_INTSTAT_SD3_FIS()                           |
 * | ddexstop | clr | intstat         | sd4_ris          |       | Dd_Exs_Get_INTSTAT_SD4_RIS()                           |
 * | ddexstop | clr | intstat         | sd4_fis          |       | Dd_Exs_Get_INTSTAT_SD4_FIS()                           |
 * | ddexstop | clr | intstat         | pcie0_ris        |       | Dd_Exs_Get_INTSTAT_PCIe0_RIS()                         |
 * | ddexstop | clr | intstat         | pcie0_fis        |       | Dd_Exs_Get_INTSTAT_PCIe0_FIS()                         |
 * | ddexstop | clr | intstat         | pcie1_ris        |       | Dd_Exs_Get_INTSTAT_PCIe1_RIS()                         |
 * | ddexstop | clr | intstat         | pcie1_fis        |       | Dd_Exs_Get_INTSTAT_PCIe1_FIS()                         |
 * | ddexstop | clr | intstat         | usb2vbus_ris     |       | Dd_Exs_Get_INTSTAT_USB2VBUS_RIS()                      |
 * | ddexstop | clr | intstat         | usb2vbus_fis     |       | Dd_Exs_Get_INTSTAT_USB2VBUS_FIS()                      |
 * | ddexstop | clr | intstat         | usb2id_ris       |       | Dd_Exs_Get_INTSTAT_USB2ID_RIS()                        |
 * | ddexstop | clr | intstat         | usb2id_fis       |       | Dd_Exs_Get_INTSTAT_USB2ID_FIS()                        |
 * | ddexstop | clr | intstat         | usb3vbus_ris     |       | Dd_Exs_Get_INTSTAT_USB2VBUS_RIS()                      |
 * | ddexstop | clr | intstat         | usb3vbus_fis     |       | Dd_Exs_Get_INTSTAT_USB2VBUS_FIS()                      |
 * | ddexstop | clr | intstat         | usb3id_ris       |       | Dd_Exs_Get_INTSTAT_USB2ID_RIS()                        |
 * | ddexstop | clr | intstat         | usb3id_fis       |       | Dd_Exs_Get_INTSTAT_USB2ID_FIS()                        |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 * | ddexstop | int | hdr             |                  |       | Dd_EXS_Int_Handler()                                   |
 * +----------+-----+-----------------+------------------+-------+--------------------------------------------------------+
 */


/**
 * @brief  Command main function for EXS test.
 * @param  int argc	:The number of parameters
 *		   char** argv	:The value of parameters
 * @return VOID
*/
void Ct_Dd_EXS_Main(int argc, char** argv)
{
	if( 0 == strcmp( argv[1], "get" ) ) {
//-------------------------------------------------------// 
		if( 0 == strcmp( argv[2], "xd_lpc" ) ) {
			if( 0 == strcmp( argv[3], "csysreq" ) ) {
				dat1 = Dd_Exs_Get_XDMAC_LowPower_Ctl_CSYSREQ();
				Ddim_Print(("Dd_Exs_Get_XDMAC_LowPower_Ctl_CSYSREQ = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "csysack" ) ) {
				dat1 = Dd_Exs_Get_XDMAC_LowPower_Ctl_CSYSACK();
				Ddim_Print(("Dd_Exs_Get_XDMAC_LowPower_Ctl_CSYSACK = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "cactrive" ) ) {
				dat1 = Dd_Exs_Get_XDMAC_LowPower_Ctl_CSYSACK();
				Ddim_Print(("Dd_Exs_Get_XDMAC_LowPower_Ctl_CACTIVE = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if( 0 == strcmp( argv[2], "softreset" ) ) {
			if( 0 == strcmp( argv[3], "nfrstn" ) ) {
				dat1 = Dd_Exs_Get_nf_rst_n_Soft_Reset();
				Ddim_Print(("Dd_Exs_Get_nf_rst_n_Soft_Reset = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "nfrstnbch" ) ) {
				dat1 = Dd_Exs_Get_bch_rst_n_Soft_Reset();
				Ddim_Print(("Dd_Exs_Get_bch_rst_n_Soft_Reset = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "nfrstnreg" ) ) {
				dat1 = Dd_Exs_Get_reg_rst_n_Soft_Reset();
				Ddim_Print(("Dd_Exs_Get_reg_rst_n_Soft_Reset = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "netsecrst" ) ) {
				dat1 = Dd_Exs_Get_NETSEC_Soft_Reset();
				Ddim_Print(("Dd_Exs_Get_NETSEC_Soft_Reset = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "emcrst" ) ) {
				dat1 = Dd_Exs_Get_eMMC_Soft_Reset();
				Ddim_Print(("Dd_Exs_Get_eMMC_Soft_Reset = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "relcrst" ) ) {
				dat1 = Dd_Exs_Get_RELC_Soft_Reset();
				Ddim_Print(("Dd_Exs_Get_RELC_Soft_Reset = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "softreset2" ) ) {
			if( 0 == strcmp( argv[3], "debretx_sd3" ) ) {
				dat1 = Dd_Exs_Get_SD3_CD_Debounce_Soft_Reset();
				Ddim_Print(("Dd_Exs_Get_SD3_CD_Debounce_Soft_Reset = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "debretx_sd4" ) ) {
				dat1 = Dd_Exs_Get_SD4_CD_Debounce_Soft_Reset();
				Ddim_Print(("Dd_Exs_Get_SD4_CD_Debounce_Soft_Reset = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "debretx_pcie0" ) ) {
				dat1 = Dd_Exs_Get_PCIE0_CD_Debounce_Soft_Reset();
				Ddim_Print(("Dd_Exs_Get_PCIE0_CD_Debounce_Soft_Reset = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "debretx_pcie1" ) ) {
				dat1 = Dd_Exs_Get_PCIE1_CD_Debounce_Soft_Reset();
				Ddim_Print(("Dd_Exs_Get_PCIE1_CD_Debounce_Soft_Reset = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "debretx_usb2vbus" ) ) {
				dat1 = Dd_Exs_Get_USB2VBUS_CD_Debounce_Soft_Reset();
				Ddim_Print(("Dd_Exs_Get_USB2VBUS_CD_Debounce_Soft_Reset = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "debretx_usb2id" ) ) {
				dat1 = Dd_Exs_Get_USB2ID_CD_Debounce_Soft_Reset();
				Ddim_Print(("Dd_Exs_Get_USB2ID_CD_Debounce_Soft_Reset = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "debretx_usb3vbus" ) ) {
				dat1 = Dd_Exs_Get_USB3VBUS_CD_Debounce_Soft_Reset();
				Ddim_Print(("Dd_Exs_Get_USB3VBUS_CD_Debounce_Soft_Reset = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "debretx_usb3id" ) ) {
				dat1 = Dd_Exs_Get_USB3ID_CD_Debounce_Soft_Reset();
				Ddim_Print(("Dd_Exs_Get_USB3ID_CD_Debounce_Soft_Reset = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "intmask" ) ) {
			if( 0 == strcmp( argv[3], "intmsk02" ) ) {
				dat1 = Dd_Exs_Get_INTMSK_SD3_RIM();
				Ddim_Print(("Dd_Exs_Get_INTMSK_SD3_RIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "intmsk03" ) ) {
				dat1 = Dd_Exs_Get_INTMSK_SD3_FIM();
				Ddim_Print(("Dd_Exs_Get_INTMSK_SD3_FIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "intmsk04" ) ) {
				dat1 = Dd_Exs_Get_INTMSK_SD4_RIM();
				Ddim_Print(("Dd_Exs_Get_INTMSK_SD4_RIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "intmsk05" ) ) {
				dat1 = Dd_Exs_Get_INTMSK_SD4_FIM();
				Ddim_Print(("Dd_Exs_Get_INTMSK_SD4_FIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "intmsk06" ) ) {
				dat1 = Dd_Exs_Get_INTMSK_PCIe0_RIM();
				Ddim_Print(("Dd_Exs_Get_INTMSK_PCIe0_RIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "intmsk07" ) ) {
				dat1 = Dd_Exs_Get_INTMSK_PCIe0_FIM();
				Ddim_Print(("Dd_Exs_Get_INTMSK_PCIe0_FIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "intmsk08" ) ) {
				dat1 = Dd_Exs_Get_INTMSK_PCIe1_RIM();
				Ddim_Print(("Dd_Exs_Get_INTMSK_PCIe1_RIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "intmsk09" ) ) {
				dat1 = Dd_Exs_Get_INTMSK_PCIe1_FIM();
				Ddim_Print(("Dd_Exs_Get_INTMSK_PCIe1_FIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "intmsk10" ) ) {
				dat1 = Dd_Exs_Get_INTMSK_USB2VBUS_RIM();
				Ddim_Print(("Dd_Exs_Get_INTMSK_USB2VBUS_RIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "intmsk11" ) ) {
				dat1 = Dd_Exs_Get_INTMSK_USB2VBUS_FIM();
				Ddim_Print(("Dd_Exs_Get_INTMSK_USB2VBUS_FIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "intmsk12" ) ) {
				dat1 = Dd_Exs_Get_INTMSK_USB2ID_RIM();
				Ddim_Print(("Dd_Exs_Get_INTMSK_USB2ID_RIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "intmsk13" ) ) {
				dat1 = Dd_Exs_Get_INTMSK_USB2ID_FIM();
				Ddim_Print(("Dd_Exs_Get_INTMSK_USB2ID_FIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "intmsk14" ) ) {
				dat1 = Dd_Exs_Get_INTMSK_USB3VBUS_RIM();
				Ddim_Print(("Dd_Exs_Get_INTMSK_USB3VBUS_RIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "intmsk15" ) ) {
				dat1 = Dd_Exs_Get_INTMSK_USB3VBUS_FIM();
				Ddim_Print(("Dd_Exs_Get_INTMSK_USB3VBUS_FIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "intmsk16" ) ) {
				dat1 = Dd_Exs_Get_INTMSK_USB3ID_RIM();
				Ddim_Print(("Dd_Exs_Get_INTMSK_USB3ID_RIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "intmsk17" ) ) {
				dat1 = Dd_Exs_Get_INTMSK_USB3ID_FIM();
				Ddim_Print(("Dd_Exs_Get_INTMSK_USB3ID_FIM = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "sigmon" ) ) {
			if( 0 == strcmp( argv[3], "sigmon_sd3" ) ) {
				dat1 = Dd_Exs_Get_SIGMON_SD3_CD();
				Ddim_Print(("Dd_Exs_Get_SIGMON_SD3_CD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sigmon_sd4" ) ) {
				dat1 = Dd_Exs_Get_SIGMON_SD4_CD();
				Ddim_Print(("Dd_Exs_Get_SIGMON_SD4_CD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sigmon_pcie0" ) ) {
				dat1 = Dd_Exs_Get_SIGMON_PCIE0_CD();
				Ddim_Print(("Dd_Exs_Get_SIGMON_PCIE0_CD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sigmon_pcie1" ) ) {
				dat1 = Dd_Exs_Get_SIGMON_PCIE1_CD();
				Ddim_Print(("Dd_Exs_Get_SIGMON_PCIE1_CD = 0x%08x\n", dat1));
			}
			if( 0 == strcmp( argv[3], "sigmon_usb2vbus" ) ) {
				dat1 = Dd_Exs_Get_SIGMON_USB2VBUS_CD();
				Ddim_Print(("Dd_Exs_Get_SIGMON_USB2VBUS_CD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sigmon_usb2id" ) ) {
				dat1 = Dd_Exs_Get_SIGMON_USB2ID_CD();
				Ddim_Print(("Dd_Exs_Get_SIGMON_USB2ID_CD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sigmon_usb3vbus" ) ) {
				dat1 = Dd_Exs_Get_SIGMON_USB3VBUS_CD();
				Ddim_Print(("Dd_Exs_Get_SIGMON_USB3VBUS_CD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sigmon_usb3id" ) ) {
				dat1 = Dd_Exs_Get_SIGMON_USB3ID_CD();
				Ddim_Print(("Dd_Exs_Get_SIGMON_USB3ID_CD = 0x%08x\n", dat1));
			}
			if( 0 == strcmp( argv[3], "sigmon2_sd3" ) ) {
				dat1 = Dd_Exs_Get_SIGMON2_SD3_CD();
				Ddim_Print(("Dd_Exs_Get_SIGMON2_SD3_CD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sigmon2_sd4" ) ) {
				dat1 = Dd_Exs_Get_SIGMON2_SD4_CD();
				Ddim_Print(("Dd_Exs_Get_SIGMON2_SD4_CD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sigmon2_pcie0" ) ) {
				dat1 = Dd_Exs_Get_SIGMON2_PCIE0_CD();
				Ddim_Print(("Dd_Exs_Get_SIGMON2_PCIE0_CD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sigmon2_pcie1" ) ) {
				dat1 = Dd_Exs_Get_SIGMON2_PCIE1_CD();
				Ddim_Print(("Dd_Exs_Get_SIGMON2_PCIE1_CD = 0x%08x\n", dat1));
			}
			if( 0 == strcmp( argv[3], "sigmon2_usb2vbus" ) ) {
				dat1 = Dd_Exs_Get_SIGMON2_USB2VBUS_CD();
				Ddim_Print(("Dd_Exs_Get_SIGMON2_USB2VBUS_CD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sigmon2_usb2id" ) ) {
				dat1 = Dd_Exs_Get_SIGMON2_USB2ID_CD();
				Ddim_Print(("Dd_Exs_Get_SIGMON2_USB2ID_CD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sigmon2_usb3vbus" ) ) {
				dat1 = Dd_Exs_Get_SIGMON2_USB3VBUS_CD();
				Ddim_Print(("Dd_Exs_Get_SIGMON2_USB3VBUS_CD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sigmon2_usb3id" ) ) {
				dat1 = Dd_Exs_Get_SIGMON2_USB3ID_CD();
				Ddim_Print(("Dd_Exs_Get_SIGMON2_USB3ID_CD = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "deben" ) ) {
			if( 0 == strcmp( argv[3], "deben_sd3" ) ) {
				dat1 = Dd_Exs_Get_DEBEN_SD3_EN();
				Ddim_Print(("Dd_Exs_Get_DEBEN_SD3_EN = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "deben_sd4" ) ) {
				dat1 = Dd_Exs_Get_DEBEN_SD4_EN();
				Ddim_Print(("Dd_Exs_Get_DEBEN_SD4_EN = 0x%08x\n", dat1));
			}
			if( 0 == strcmp( argv[3], "deben_pcie0" ) ) {
				dat1 = Dd_Exs_Get_DEBEN_PCIE0_EN();
				Ddim_Print(("Dd_Exs_Get_DEBEN_PCIE0_EN = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "deben_pcie1" ) ) {
				dat1 = Dd_Exs_Get_DEBEN_PCIE1_EN();
				Ddim_Print(("Dd_Exs_Get_DEBEN_PCIE1_EN = 0x%08x\n", dat1));
			}
			if( 0 == strcmp( argv[3], "deben_usb2vbus" ) ) {
				dat1 = Dd_Exs_Get_DEBEN_USB2VBUS_EN();
				Ddim_Print(("Dd_Exs_Get_DEBEN_USB2VBUS_EN = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "deben_usb2id" ) ) {
				dat1 = Dd_Exs_Get_DEBEN_USB2ID_EN();
				Ddim_Print(("Dd_Exs_Get_DEBEN_USB2ID_EN = 0x%08x\n", dat1));
			}
			if( 0 == strcmp( argv[3], "deben_usb3vbus" ) ) {
				dat1 = Dd_Exs_Get_DEBEN_USB3VBUS_EN();
				Ddim_Print(("Dd_Exs_Get_DEBEN_USB3VBUS_EN = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "deben_usb3id" ) ) {
				dat1 = Dd_Exs_Get_DEBEN_USB3ID_EN();
				Ddim_Print(("Dd_Exs_Get_DEBEN_USB3ID_EN = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sd3detim" ) ) {
				dat1 = Dd_Exs_Get_DEBEN_SD3DETIM();
				Ddim_Print(("Dd_Exs_Get_DEBEN_SD3DETIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sd4detim" ) ) {
				dat1 = Dd_Exs_Get_DEBEN_SD4DETIM();
				Ddim_Print(("Dd_Exs_Get_DEBEN_SD4DETIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "pcie0detim" ) ) {
				dat1 = Dd_Exs_Get_DEBEN_PCIE0DETIM();
				Ddim_Print(("Dd_Exs_Get_DEBEN_PCIE0DETIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "pcie1detim" ) ) {
				dat1 = Dd_Exs_Get_DEBEN_PCIE1DETIM();
				Ddim_Print(("Dd_Exs_Get_DEBEN_PCIE1DETIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "usb2vbdetim" ) ) {
				dat1 = Dd_Exs_Get_DEBEN_USB2VBUSDETIM();
				Ddim_Print(("Dd_Exs_Get_DEBEN_USB2VBUSDETIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "usb2iddetim" ) ) {
				dat1 = Dd_Exs_Get_DEBEN_USB2IDDETIM();
				Ddim_Print(("Dd_Exs_Get_DEBEN_USB2IDDETIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "usb3vbdetim" ) ) {
				dat1 = Dd_Exs_Get_DEBEN_USB3VBUSDETIM();
				Ddim_Print(("Dd_Exs_Get_DEBEN_USB3VBUSDETIM = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "usb3iddetim" ) ) {
				dat1 = Dd_Exs_Get_DEBEN_USB3IDDETIM();
				Ddim_Print(("Dd_Exs_Get_DEBEN_USB3IDDETIM = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "cdinsmdctl" ) ) {
			if( 0 == strcmp( argv[3], "cdmod_sd3" ) ) {
				dat1 = Dd_Exs_Get_CDINS_SD3_MOD();
				Ddim_Print(("Dd_Exs_Get_CDINS_SD3_MOD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "cdmod_sd4" ) ) {
				dat1 = Dd_Exs_Get_CDINS_SD4_MOD();
				Ddim_Print(("Dd_Exs_Get_CDINS_SD4_MOD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "cdmod_pcie0" ) ) {
				dat1 = Dd_Exs_Get_CDINS_PCIE0_MOD();
				Ddim_Print(("Dd_Exs_Get_CDINS_PCIE0_MOD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "cdmod_pcie1" ) ) {
				dat1 = Dd_Exs_Get_CDINS_PCIE1_MOD();
				Ddim_Print(("Dd_Exs_Get_CDINS_PCIE1_MOD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "cdmod_usb2vbus" ) ) {
				dat1 = Dd_Exs_Get_CDINS_USB2VBUS_MOD();
				Ddim_Print(("Dd_Exs_Get_CDINS_USB2VBUS_MOD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "cdmod_usb2id" ) ) {
				dat1 = Dd_Exs_Get_CDINS_USB2ID_MOD();
				Ddim_Print(("Dd_Exs_Get_CDINS_USB2ID_MOD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "cdmod_usb3vbus" ) ) {
				dat1 = Dd_Exs_Get_CDINS_USB3VBUS_MOD();
				Ddim_Print(("Dd_Exs_Get_CDINS_USB3VBUS_MOD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "cdmod_usb3id" ) ) {
				dat1 = Dd_Exs_Get_CDINS_USB3ID_MOD();
				Ddim_Print(("Dd_Exs_Get_CDINS_USB3ID_MOD = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "rlc_hpt_ctl" ) ) {
			if( 0 == strcmp( argv[3], "rerhpt" ) ) {
				dat1 = Dd_Exs_Get_RELC_HPROT_Read();
				Ddim_Print(("Dd_Exs_Get_RELC_HPROT_Read = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "rewhpt" ) ) {
				dat1 = Dd_Exs_Get_RELC_HPROT_Write();
				Ddim_Print(("Dd_Exs_Get_RELC_HPROT_Write = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "ram_pd" ) ) {
			if( 0 == strcmp( argv[3], "rampd_sd2" ) ) {
				dat1 = Dd_Exs_Get_RAM_PD_Ctrl_SD2();
				Ddim_Print(("Dd_Exs_Get_RAM_PD_Ctrl_SD2 = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "rampd_sd3" ) ) {
				dat1 = Dd_Exs_Get_RAM_PD_Ctrl_SD3();
				Ddim_Print(("Dd_Exs_Get_RAM_PD_Ctrl_SD3 = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "rampd_sd4" ) ) {
				dat1 = Dd_Exs_Get_RAM_PD_Ctrl_SD4();
				Ddim_Print(("Dd_Exs_Get_RAM_PD_Ctrl_SD4 = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "rampd_nf" ) ) {
				dat1 = Dd_Exs_Get_RAM_PD_Ctrl_NF();
				Ddim_Print(("Dd_Exs_Get_RAM_PD_Ctrl_NF = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "rampd_relc" ) ) {
				dat1 = Dd_Exs_Get_RAM_PD_Ctrl_RELC();
				Ddim_Print(("Dd_Exs_Get_RAM_PD_Ctrl_RELC = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "rampd_netsec" ) ) {
				dat1 = Dd_Exs_Get_RAM_PD_Ctrl_NETSEC();
				Ddim_Print(("Dd_Exs_Get_RAM_PD_Ctrl_NETSEC = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "rampd_usb2" ) ) {
				dat1 = Dd_Exs_Get_RAM_PD_Ctrl_USB2();
				Ddim_Print(("Dd_Exs_Get_RAM_PD_Ctrl_USB2 = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "rampd_usb3" ) ) {
				dat1 = Dd_Exs_Get_RAM_PD_Ctrl_USB3();
				Ddim_Print(("Dd_Exs_Get_RAM_PD_Ctrl_USB3 = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "rampd_pcie0" ) ) {
				dat1 = Dd_Exs_Get_RAM_PD_Ctrl_PCIE0();
				Ddim_Print(("Dd_Exs_Get_RAM_PD_Ctrl_PCIE0 = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "rampd_pcie1" ) ) {
				dat1 = Dd_Exs_Get_RAM_PD_Ctrl_PCIE1();
				Ddim_Print(("Dd_Exs_Get_RAM_PD_Ctrl_PCIE1 = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "rampd_emmc" ) ) {
				dat1 = Dd_Exs_Get_RAM_PD_Ctrl_EMMC();
				Ddim_Print(("Dd_Exs_Get_RAM_PD_Ctrl_EMMC = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "busckstp" ) ) {
			if( 0 == strcmp( argv[3], "nf_ckstp" ) ) {
				dat1 = Dd_Exs_Get_NF_BUS_CLOCK_Stop_Ctrl();
				Ddim_Print(("Dd_Exs_Get_NF_BUS_CLOCK_Stop_Ctrl = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "pcie0_ckstp" ) ) {
				dat1 = Dd_Exs_Get_PCIE0_BUS_CLOCK_Stop_Ctrl();
				Ddim_Print(("Dd_Exs_Get_PCIE0_BUS_CLOCK_Stop_Ctrl = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "pcie1_ckstp" ) ) {
				dat1 = Dd_Exs_Get_PCIE1_BUS_CLOCK_Stop_Ctrl();
				Ddim_Print(("Dd_Exs_Get_PCIE1_BUS_CLOCK_Stop_Ctrl = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "xdmac_ckstp" ) ) {
				dat1 = Dd_Exs_Get_XDMAC_BUS_CLOCK_Stop_Ctrl();
				Ddim_Print(("Dd_Exs_Get_XDMAC_BUS_CLOCK_Stop_Ctrl = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "hdmac_ckstp" ) ) {
				dat1 = Dd_Exs_Get_HDMAC_BUS_CLOCK_Stop_Ctrl();
				Ddim_Print(("Dd_Exs_Get_HDMAC_BUS_CLOCK_Stop_Ctrl = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "emmc_ckstp" ) ) {
				dat1 = Dd_Exs_Get_EMMC_BUS_CLOCK_Stop_Ctrl();
				Ddim_Print(("Dd_Exs_Get_EMMC_BUS_CLOCK_Stop_Ctrl = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sd2_ckstp" ) ) {
				dat1 = Dd_Exs_Get_SD2_BUS_CLOCK_Stop_Ctrl();
				Ddim_Print(("Dd_Exs_Get_SD2_BUS_CLOCK_Stop_Ctrl = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sd3_ckstp" ) ) {
				dat1 = Dd_Exs_Get_SD3_BUS_CLOCK_Stop_Ctrl();
				Ddim_Print(("Dd_Exs_Get_SD3_BUS_CLOCK_Stop_Ctrl = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sd4_ckstp" ) ) {
				dat1 = Dd_Exs_Get_SD4_BUS_CLOCK_Stop_Ctrl();
				Ddim_Print(("Dd_Exs_Get_SD4_BUS_CLOCK_Stop_Ctrl = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "netsec_ckstp" ) ) {
				dat1 = Dd_Exs_Get_NETSEC_BUS_CLOCK_Stop_Ctrl();
				Ddim_Print(("Dd_Exs_Get_NETSEC_BUS_CLOCK_Stop_Ctrl = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "relc_ckstp" ) ) {
				dat1 = Dd_Exs_Get_RELC_BUS_CLOCK_Stop_Ctrl();
				Ddim_Print(("Dd_Exs_Get_RELC_BUS_CLOCK_Stop_Ctrl = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "nsec_ckstp" ) ) {
			if( 0 == strcmp( argv[3], "netsec_ptpstp" ) ) {
				dat1 = Dd_Exs_Get_NETSEC_PTPCLK_Stop_Ctrl_NETSEC_PTPSTP();
				Ddim_Print(("Dd_Exs_Get_NETSEC_PTPCLK_Stop_Ctrl_NETSEC_PTPSTP = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "emmc_ckstp" ) ) {
			if( 0 == strcmp( argv[3], "emclkstp" ) ) {
				dat1 = Dd_Exs_Get_EMMC_CKSTP_EMCLKSTP();
				Ddim_Print(("Dd_Exs_Get_EMMC_CKSTP_EMCLKSTP = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "em_drvstr" ) ) {
			if( 0 == strcmp( argv[3], "em_dsel" ) ) {
				dat1 = Dd_Exs_Get_EM_DRVSTR();
				Ddim_Print(("Dd_Exs_Get_EM_DRVSTR = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "em_pv_dtimec" ) ) {
			if( 0 == strcmp( argv[3], "pv_dtimec" ) ) {
				dat1 = Dd_Exs_Get_EM_PV_DTIMEC();
				Ddim_Print(("Dd_Exs_Get_EM_PV_DTIMEC = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "em_pv_ampbl" ) ) {
			if( 0 == strcmp( argv[3], "pv_ampbl" ) ) {
				dat1 = Dd_Exs_Get_EM_PV_AMPBL();
				Ddim_Print(("Dd_Exs_Get_EM_PV_AMPBL = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "em_cr_slottype" ) ) {
			if( 0 == strcmp( argv[3], "cr_slottype" ) ) {
				dat1 = Dd_Exs_Get_EM_CR_SLOTTYPE();
				Ddim_Print(("Dd_Exs_Get_EM_CR_SLOTTYPE = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "em_cr_bclkfreq" ) ) {
			if( 0 == strcmp( argv[3], "cr_bclkfreq" ) ) {
				dat1 = Dd_Exs_Get_EM_CR_BCLKFREQ();
				Ddim_Print(("Dd_Exs_Get_EM_CR_BCLKFREQ = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "em_cddet" ) ) {
			if( 0 == strcmp( argv[3], "emcd" ) ) {
				dat1 = Dd_Exs_Get_EM_CDDET_EMCD();
				Ddim_Print(("Dd_Exs_Get_EM_CDDET_EMCD = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "emphylk" ) ) {
				dat1 = Dd_Exs_Get_EM_CDDET_EMPHYLK();
				Ddim_Print(("Dd_Exs_Get_EM_CDDET_EMPHYLK = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "nf_ip_ctl" ) ) {
			if( 0 == strcmp( argv[3], "dscv_inhibit" ) ) {
				dat1 = Dd_Exs_Get_NF_IP_CTL_DSCV_INHIBIT();
				Ddim_Print(("Dd_Exs_Get_NF_IP_CTL_DSCV_INHIBIT = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "dscv_ignr_crc" ) ) {
				dat1 = Dd_Exs_Get_NF_IP_CTL_DSCV_IGNR_CRC();
				Ddim_Print(("Dd_Exs_Get_NF_IP_CTL_DSCV_IGNR_CRC = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "boot_en" ) ) {
				dat1 = Dd_Exs_Get_NF_IP_CTL_BOOT_EN();
				Ddim_Print(("Dd_Exs_Get_NF_IP_CTL_BOOT_EN = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "iddq_en" ) ) {
				dat1 = Dd_Exs_Get_NF_IP_CTL_IDDQ_EN();
				Ddim_Print(("Dd_Exs_Get_NF_IP_CTL_IDDQ_EN = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "nf_btsq_state" ) ) {
			if( 0 == strcmp( argv[3], "init_fail" ) ) {
				dat1 = Dd_Exs_Get_NF_BTSQ_STATE_INIT_FAIL();
				Ddim_Print(("Dd_Exs_Get_NF_BTSQ_STATE_INIT_FAIL = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "boot_comp" ) ) {
				dat1 = Dd_Exs_Get_NF_BTSQ_STATE_BOOT_COMP();
				Ddim_Print(("Dd_Exs_Get_NF_BTSQ_STATE_BOOT_COMP = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "boot_error" ) ) {
				dat1 = Dd_Exs_Get_NF_BTSQ_STATE_CTRL_BUSY();
				Ddim_Print(("Dd_Exs_Get_NF_BTSQ_STATE_CTRL_BUSY = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "boot_busy" ) ) {
				dat1 = Dd_Exs_Get_NF_BTSQ_STATE_CTRL_BUSY();
				Ddim_Print(("Dd_Exs_Get_NF_BTSQ_STATE_CTRL_BUSY = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "nf_rb_valid_tm" ) ) {
			if( 0 == strcmp( argv[3], "rb_valid_time" ) ) {
				dat1 = Dd_Exs_Get_NF_RB_VALID_TM();
				Ddim_Print(("Dd_Exs_Get_NF_RB_VALID_TM = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "nf_phy_ctrl_reg" ) ) {
			if( 0 == strcmp( argv[3], "phy_ctrl" ) ) {
				dat1 = Dd_Exs_Get_NF_PHY_CTRL_REG();
				Ddim_Print(("Dd_Exs_Get_NF_PHY_CTRL_REG = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "nf_phy_dq_tim" ) ) {
			if( 0 == strcmp( argv[3], "phy_dq_timing" ) ) {
				dat1 = Dd_Exs_Get_NF_PHY_DQ_TIMING();
				Ddim_Print(("Dd_Exs_Get_NF_PHY_DQ_TIMING = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "nf_phy_dqs_tim" ) ) {
			if( 0 == strcmp( argv[3], "phy_dqs_timing" ) ) {
				dat1 = Dd_Exs_Get_NF_PHY_DQS_TIMING();
				Ddim_Print(("Dd_Exs_Get_NF_PHY_DQS_TIMING = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "nf_phy_gt_lpbk" ) ) {
			if( 0 == strcmp( argv[3], "phy_gt_lpbk_ctl" ) ) {
				dat1 = Dd_Exs_Get_NF_PHY_GT_LPBK_CTL();
				Ddim_Print(("Dd_Exs_Get_NF_PHY_GT_LPBK_CTL = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "nf_phy_dll_mctl" ) ) {
			if( 0 == strcmp( argv[3], "phy_dll_mst_ctl" ) ) {
				dat1 = Dd_Exs_Get_NF_PHY_DLL_MCTL();
				Ddim_Print(("Dd_Exs_Get_NF_PHY_DLL_MCTL = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "nf_phy_dll_sctl" ) ) {
			if( 0 == strcmp( argv[3], "phy_dll_slv_ctl" ) ) {
				dat1 = Dd_Exs_Get_NF_PHY_DLL_SCTL();
				Ddim_Print(("Dd_Exs_Get_NF_PHY_DLL_SCTL = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "nf_boot_ecc_ctl" ) ) {
			if( 0 == strcmp( argv[3], "boot_ecc_sec_siz" ) ) {
				dat1 = Dd_Exs_Get_NF_BOOT_ECC_CTL_SEC_SIZ();
				Ddim_Print(("Dd_Exs_Get_NF_BOOT_ECC_CTL_SEC_SIZ = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "bt_ecc_corr_str" ) ) {
				dat1 = Dd_Exs_Get_NF_BOOT_ECC_CTL_CORR_STR();
				Ddim_Print(("Dd_Exs_Get_NF_BOOT_ECC_CTL_CORR_STR = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "boot_ecc_en" ) ) {
				dat1 = Dd_Exs_Get_NF_BOOT_ECC_CTL_EN();
				Ddim_Print(("Dd_Exs_Get_NF_BOOT_ECC_CTL_EN = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "nf_wpx_cnt" ) ) {
			if( 0 == strcmp( argv[3], "wpx_cnt" ) ) {
				dat1 = Dd_Exs_Get_NF_NFWPX_CNT();
				Ddim_Print(("Dd_Exs_Get_NF_NFWPX_CNT = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}else if( 0 == strcmp( argv[2], "intstat" ) ) {
			if( 0 == strcmp( argv[3], "sd2_ris" ) ) {
				dat1 = Dd_Exs_Get_INTSTAT_SD2_RIS();
				Ddim_Print(("Dd_Exs_Get_INTSTAT_SD2_RIS = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sd2_fis" ) ) {
				dat1 = Dd_Exs_Get_INTSTAT_SD2_FIS();
				Ddim_Print(("Dd_Exs_Get_INTSTAT_SD2_FIS = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sd3_ris" ) ) {
				dat1 = Dd_Exs_Get_INTSTAT_SD3_RIS();
				Ddim_Print(("Dd_Exs_Get_INTSTAT_SD3_RIS = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sd3_fis" ) ) {
				dat1 = Dd_Exs_Get_INTSTAT_SD3_FIS();
				Ddim_Print(("Dd_Exs_Get_INTSTAT_SD3_FIS = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sd4_ris" ) ) {
				dat1 = Dd_Exs_Get_INTSTAT_SD4_RIS();
				Ddim_Print(("Dd_Exs_Get_INTSTAT_SD4_RIS = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "sd4_fis" ) ) {
				dat1 = Dd_Exs_Get_INTSTAT_SD4_FIS();
				Ddim_Print(("Dd_Exs_Get_INTSTAT_SD4_FIS = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "pcie0_ris" ) ) {
				dat1 = Dd_Exs_Get_INTSTAT_PCIe0_RIS();
				Ddim_Print(("Dd_Exs_Get_INTSTAT_PCIe0_RIS = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "pcie0_fis" ) ) {
				dat1 = Dd_Exs_Get_INTSTAT_PCIe0_FIS();
				Ddim_Print(("Dd_Exs_Get_INTSTAT_PCIe0_FIS = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "pcie1_ris" ) ) {
				dat1 = Dd_Exs_Get_INTSTAT_PCIe1_RIS();
				Ddim_Print(("Dd_Exs_Get_INTSTAT_PCIe1_RIS = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "pcie1_fis" ) ) {
				dat1 = Dd_Exs_Get_INTSTAT_PCIe1_FIS();
				Ddim_Print(("Dd_Exs_Get_INTSTAT_PCIe1_FIS = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "usb2vbus_ris" ) ) {
				dat1 = Dd_Exs_Get_INTSTAT_USB2VBUS_RIS();
				Ddim_Print(("Dd_Exs_Get_INTSTAT_USB2VBUS_RIS = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "usb2vbus_fis" ) ) {
				dat1 = Dd_Exs_Get_INTSTAT_USB2VBUS_FIS();
				Ddim_Print(("Dd_Exs_Get_INTSTAT_USB2VBUS_FIS = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "usb2id_ris" ) ) {
				dat1 = Dd_Exs_Get_INTSTAT_USB2ID_RIS();
				Ddim_Print(("Dd_Exs_Get_INTSTAT_USB2ID_RIS = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "usb2id_fis" ) ) {
				dat1 = Dd_Exs_Get_INTSTAT_USB2ID_FIS();
				Ddim_Print(("Dd_Exs_Get_INTSTAT_USB2ID_FIS = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "usb3vbus_ris" ) ) {
				dat1 = Dd_Exs_Get_INTSTAT_USB3VBUS_RIS();
				Ddim_Print(("Dd_Exs_Get_INTSTAT_USB3VBUS_RIS = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "usb3vbus_fis" ) ) {
				dat1 = Dd_Exs_Get_INTSTAT_USB3VBUS_FIS();
				Ddim_Print(("Dd_Exs_Get_INTSTAT_USB3VBUS_FIS = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "usb3id_ris" ) ) {
				dat1 = Dd_Exs_Get_INTSTAT_USB3ID_RIS();
				Ddim_Print(("Dd_Exs_Get_INTSTAT_USB3ID_RIS = 0x%08x\n", dat1));
			}
			else if( 0 == strcmp( argv[3], "usb3id_fis" ) ) {
				dat1 = Dd_Exs_Get_INTSTAT_USB3ID_FIS();
				Ddim_Print(("Dd_Exs_Get_INTSTAT_USB3ID_FIS = 0x%08x\n", dat1));
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------//
		}
		else{
			// Input parameter NG
			Ddim_Print(("please check 2nd parameter!!\n"));
		}
	}
/*********************************************************/ 
	else if( 0 == strcmp( argv[1], "set" ) ) {
		input = atoi(argv[4]);
//-------------------------------------------------------// 
		if( 0 == strcmp( argv[2], "xd_lpc" ) ) {
			if( 0 == strcmp( argv[3], "csysreq" ) ) {
				Dd_Exs_Set_XDMAC_LowPower_Ctl_CSYSREQ(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "softreset" ) ) {
			if( 0 == strcmp( argv[3], "nfrstn" ) ) {
				Dd_Exs_Set_nf_rst_n_Soft_Reset(input);
			}
			else if( 0 == strcmp( argv[3], "nfrstnbch" ) ) {
				Dd_Exs_Set_bch_rst_n_Soft_Reset(input);
			}
			else if( 0 == strcmp( argv[3], "nfrstnreg" ) ) {
				Dd_Exs_Set_reg_rst_n_Soft_Reset(input);
			}
			else if( 0 == strcmp( argv[3], "netsecrst" ) ) {
				Dd_Exs_Set_NETSEC_Soft_Reset(input);
			}
			else if( 0 == strcmp( argv[3], "emcrst" ) ) {
				Dd_Exs_Set_eMMC_Soft_Reset(input);
			}
			else if( 0 == strcmp( argv[3], "relcrst" ) ) {
				Dd_Exs_Set_RELC_Soft_Reset(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "softreset2" ) ) {
			if( 0 == strcmp( argv[3], "debretx_sd3" ) ) {
				Dd_Exs_Set_SD3_CD_Debounce_Soft_Reset(input);
			}
			else if( 0 == strcmp( argv[3], "debretx_sd4" ) ) {
				Dd_Exs_Set_SD4_CD_Debounce_Soft_Reset(input);
			}
			else if( 0 == strcmp( argv[3], "debretx_pcie0" ) ) {
				Dd_Exs_Set_PCIE0_CD_Debounce_Soft_Reset(input);
			}
			else if( 0 == strcmp( argv[3], "debretx_pcie1" ) ) {
				Dd_Exs_Set_PCIE1_CD_Debounce_Soft_Reset(input);
			}
			else if( 0 == strcmp( argv[3], "debretx_usb2vbus" ) ) {
				Dd_Exs_Set_USB2VBUS_CD_Debounce_Soft_Reset(input);
			}
			else if( 0 == strcmp( argv[3], "debretx_usb2id" ) ) {
				Dd_Exs_Set_USB2ID_CD_Debounce_Soft_Reset(input);
			}
			else if( 0 == strcmp( argv[3], "debretx_usb3vbus" ) ) {
				Dd_Exs_Set_USB3VBUS_CD_Debounce_Soft_Reset(input);
			}
			else if( 0 == strcmp( argv[3], "debretx_usb3id" ) ) {
				Dd_Exs_Set_USB3ID_CD_Debounce_Soft_Reset(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "intmask" ) ) {
			if( 0 == strcmp( argv[3], "intmsk02" ) ) {
				Dd_Exs_Set_INTMSK_SD3_RIM(input);
			}
			else if( 0 == strcmp( argv[3], "intmsk03" ) ) {
				Dd_Exs_Set_INTMSK_SD3_FIM(input);
			}
			else if( 0 == strcmp( argv[3], "intmsk04" ) ) {
				Dd_Exs_Set_INTMSK_SD4_RIM(input);
			}
			else if( 0 == strcmp( argv[3], "intmsk05" ) ) {
				Dd_Exs_Set_INTMSK_SD4_FIM(input);
			}
			else if( 0 == strcmp( argv[3], "intmsk06" ) ) {
				Dd_Exs_Set_INTMSK_PCIe0_RIM(input);
			}
			else if( 0 == strcmp( argv[3], "intmsk07" ) ) {
				Dd_Exs_Set_INTMSK_PCIe0_FIM(input);
			}
			else if( 0 == strcmp( argv[3], "intmsk08" ) ) {
				Dd_Exs_Set_INTMSK_PCIe1_RIM(input);
			}
			else if( 0 == strcmp( argv[3], "intmsk09" ) ) {
				Dd_Exs_Set_INTMSK_PCIe1_FIM(input);
			}
			else if( 0 == strcmp( argv[3], "intmsk10" ) ) {
				Dd_Exs_Set_INTMSK_USB2VBUS_RIM(input);
			}
			else if( 0 == strcmp( argv[3], "intmsk11" ) ) {
				Dd_Exs_Set_INTMSK_USB2VBUS_FIM(input);
			}
			else if( 0 == strcmp( argv[3], "intmsk12" ) ) {
				Dd_Exs_Set_INTMSK_USB2ID_RIM(input);
			}
			else if( 0 == strcmp( argv[3], "intmsk13" ) ) {
				Dd_Exs_Set_INTMSK_USB2ID_FIM(input);
			}
			else if( 0 == strcmp( argv[3], "intmsk14" ) ) {
				Dd_Exs_Set_INTMSK_USB3VBUS_RIM(input);
			}
			else if( 0 == strcmp( argv[3], "intmsk15" ) ) {
				Dd_Exs_Set_INTMSK_USB3VBUS_FIM(input);
			}
			else if( 0 == strcmp( argv[3], "intmsk16" ) ) {
				Dd_Exs_Set_INTMSK_USB3ID_RIM(input);
			}
			else if( 0 == strcmp( argv[3], "intmsk17" ) ) {
				Dd_Exs_Set_INTMSK_USB3ID_FIM(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "deben" ) ) {
			if( 0 == strcmp( argv[3], "deben_sd3" ) ) {
				Dd_Exs_Set_DEBEN_SD3_EN(input);
			}
			else if( 0 == strcmp( argv[3], "deben_sd4" ) ) {
				Dd_Exs_Set_DEBEN_SD4_EN(input);
			}
			else if( 0 == strcmp( argv[3], "deben_pcie0" ) ) {
				Dd_Exs_Set_DEBEN_PCIE0_EN(input);
			}
			else if( 0 == strcmp( argv[3], "deben_pcie1" ) ) {
				Dd_Exs_Set_DEBEN_PCIE1_EN(input);
			}
			else if( 0 == strcmp( argv[3], "deben_usb2vbus" ) ) {
				Dd_Exs_Set_DEBEN_USB2VBUS_EN(input);
			}
			else if( 0 == strcmp( argv[3], "deben_usb2id" ) ) {
				Dd_Exs_Set_DEBEN_USB2ID_EN(input);
			}
			else if( 0 == strcmp( argv[3], "deben_usb3vbus" ) ) {
				Dd_Exs_Set_DEBEN_USB3VBUS_EN(input);
			}
			else if( 0 == strcmp( argv[3], "deben_usb3id" ) ) {
				Dd_Exs_Set_DEBEN_USB3ID_EN(input);
			}
			else if( 0 == strcmp( argv[3], "sd3detim" ) ) {
				Dd_Exs_Set_DEBEN_SD3DETIM(input);
			}
			else if( 0 == strcmp( argv[3], "sd4detim" ) ) {
				Dd_Exs_Set_DEBEN_SD4DETIM(input);
			}
			else if( 0 == strcmp( argv[3], "pcie0detim" ) ) {
				Dd_Exs_Set_DEBEN_PCIE0DETIM(input);
			}
			else if( 0 == strcmp( argv[3], "pcie1detim" ) ) {
				Dd_Exs_Set_DEBEN_PCIE1DETIM(input);
			}
			else if( 0 == strcmp( argv[3], "usb2vbdetim" ) ) {
				Dd_Exs_Set_DEBEN_USB2VBUSDETIM(input);
			}
			else if( 0 == strcmp( argv[3], "usb2iddetim" ) ) {
				Dd_Exs_Set_DEBEN_USB2IDDETIM(input);
			}
			else if( 0 == strcmp( argv[3], "usb3vbdetim" ) ) {
				Dd_Exs_Set_DEBEN_USB3VBUSDETIM(input);
			}
			else if( 0 == strcmp( argv[3], "usb3iddetim" ) ) {
				Dd_Exs_Set_DEBEN_USB3IDDETIM(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "cdinsmdctl" ) ) {
			if( 0 == strcmp( argv[3], "cdmod_sd3" ) ) {
				Dd_Exs_Set_CDINS_SD3_MOD(input);
			}
			else if( 0 == strcmp( argv[3], "cdmod_sd4" ) ) {
				Dd_Exs_Set_CDINS_SD4_MOD(input);
			}
			else if( 0 == strcmp( argv[3], "cdmod_pcie0" ) ) {
				Dd_Exs_Set_CDINS_PCIE0_MOD(input);
			}
			else if( 0 == strcmp( argv[3], "cdmod_pcie1" ) ) {
				Dd_Exs_Set_CDINS_PCIE1_MOD(input);
			}
			else if( 0 == strcmp( argv[3], "cdmod_usb2vbus" ) ) {
				Dd_Exs_Set_CDINS_USB2VBUS_MOD(input);
			}
			else if( 0 == strcmp( argv[3], "cdmod_usb2id" ) ) {
				Dd_Exs_Set_CDINS_USB2ID_MOD(input);
			}
			else if( 0 == strcmp( argv[3], "cdmod_usb3vbus" ) ) {
				Dd_Exs_Set_CDINS_USB3VBUS_MOD(input);
			}
			else if( 0 == strcmp( argv[3], "cdmod_usb3id" ) ) {
				Dd_Exs_Set_CDINS_USB3ID_MOD(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "rlc_hpt_ctl" ) ) {
			if( 0 == strcmp( argv[3], "rerhpt" ) ) {
				Dd_Exs_Set_RELC_HPROT_Read(input);
			}
			else if( 0 == strcmp( argv[3], "rewhpt" ) ) {
				Dd_Exs_Set_RELC_HPROT_Write(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "ram_pd" ) ) {
			if( 0 == strcmp( argv[3], "rampd_sd2" ) ) {
				Dd_Exs_Set_RAM_PD_Ctrl_SD2(input);
			}
			else if( 0 == strcmp( argv[3], "rampd_sd3" ) ) {
				Dd_Exs_Set_RAM_PD_Ctrl_SD3(input);
			}

			else if( 0 == strcmp( argv[3], "rampd_sd4" ) ) {
				Dd_Exs_Set_RAM_PD_Ctrl_SD4(input);
			}
			else if( 0 == strcmp( argv[3], "rampd_nf" ) ) {
				Dd_Exs_Set_RAM_PD_Ctrl_NF(input);
			}
			else if( 0 == strcmp( argv[3], "rampd_relc" ) ) {
				Dd_Exs_Set_RAM_PD_Ctrl_RELC(input);
			}
			else if( 0 == strcmp( argv[3], "rampd_netsec" ) ) {
				Dd_Exs_Set_RAM_PD_Ctrl_NETSEC(input);
			}
			else if( 0 == strcmp( argv[3], "rampd_usb2" ) ) {
				Dd_Exs_Set_RAM_PD_Ctrl_USB2(input);
			}
			else if( 0 == strcmp( argv[3], "rampd_usb3" ) ) {
				Dd_Exs_Set_RAM_PD_Ctrl_USB3(input);
			}
			else if( 0 == strcmp( argv[3], "rampd_pcie0" ) ) {
				Dd_Exs_Set_RAM_PD_Ctrl_PCIE0(input);
			}
			else if( 0 == strcmp( argv[3], "rampd_pcie1" ) ) {
				Dd_Exs_Set_RAM_PD_Ctrl_PCIE1(input);
			}
			else if( 0 == strcmp( argv[3], "rampd_emmc" ) ) {
				Dd_Exs_Set_RAM_PD_Ctrl_EMMC(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "busckstp" ) ) {
			if( 0 == strcmp( argv[3], "nf_ckstp" ) ) {
				Dd_Exs_Set_NF_BUS_CLOCK_Stop_Ctrl(input);
			}
			else if( 0 == strcmp( argv[3], "pcie0_ckstp" ) ) {
				Dd_Exs_Set_PCIE0_BUS_CLOCK_Stop_Ctrl(input);
			}
			else if( 0 == strcmp( argv[3], "pcie1_ckstp" ) ) {
				Dd_Exs_Set_PCIE1_BUS_CLOCK_Stop_Ctrl(input);
			}
			else if( 0 == strcmp( argv[3], "xdmac_ckstp" ) ) {
				Dd_Exs_Set_XDMAC_BUS_CLOCK_Stop_Ctrl(input);
			}
			else if( 0 == strcmp( argv[3], "hdmac_ckstp" ) ) {
				Dd_Exs_Set_HDMAC_BUS_CLOCK_Stop_Ctrl(input);
			}
			else if( 0 == strcmp( argv[3], "emmc_ckstp" ) ) {
				Dd_Exs_Set_EMMC_BUS_CLOCK_Stop_Ctrl(input);
			}
			else if( 0 == strcmp( argv[3], "sd2_ckstp" ) ) {
				Dd_Exs_Set_SD2_BUS_CLOCK_Stop_Ctrl(input);
			}
			else if( 0 == strcmp( argv[3], "sd3_ckstp" ) ) {
				Dd_Exs_Set_SD3_BUS_CLOCK_Stop_Ctrl(input);
			}
			else if( 0 == strcmp( argv[3], "sd4_ckstp" ) ) {
				Dd_Exs_Set_SD4_BUS_CLOCK_Stop_Ctrl(input);
			}
			else if( 0 == strcmp( argv[3], "netsec_ckstp" ) ) {
				Dd_Exs_Set_NETSEC_BUS_CLOCK_Stop_Ctrl(input);
			}
			else if( 0 == strcmp( argv[3], "relc_ckstp" ) ) {
				Dd_Exs_Set_RELC_BUS_CLOCK_Stop_Ctrl(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "nsec_ckstp" ) ) {
			if( 0 == strcmp( argv[3], "netsec_ptpstp" ) ) {
				Dd_Exs_Set_NETSEC_PTPCLK_Stop_Ctrl_NETSEC_PTPSTP(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "emmc_ckstp" ) ) {
			if( 0 == strcmp( argv[3], "emclkstp" ) ) {
				Dd_Exs_Set_EMMC_CKSTP_EMCLKSTP(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "em_pv_dtimec" ) ) {
			if( 0 == strcmp( argv[3], "pv_dtimec" ) ) {
				Dd_Exs_Set_EM_PV_DTIMEC(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "em_pv_ampbl" ) ) {
			if( 0 == strcmp( argv[3], "pv_ampbl" ) ) {
				Dd_Exs_Set_EM_PV_AMPBL(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "em_cr_slottype" ) ) {
			if( 0 == strcmp( argv[3], "cr_slottype" ) ) {
				Dd_Exs_Set_EM_CR_SLOTTYPE(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "em_cr_bclkfreq" ) ) {
			if( 0 == strcmp( argv[3], "cr_bclkfreq" ) ) {
				Dd_Exs_Set_EM_CR_BCLKFREQ(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "em_cddet" ) ) {
			if( 0 == strcmp( argv[3], "emcd" ) ) {
				Dd_Exs_Set_EM_CDDET_EMCD(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "nf_ip_ctl" ) ) {
			if( 0 == strcmp( argv[3], "dscv_inhibit" ) ) {
				Dd_Exs_Set_NF_IP_CTL_DSCV_INHIBIT(input);
			}
			else if( 0 == strcmp( argv[3], "dscv_ignr_crc" ) ) {
				Dd_Exs_Set_NF_IP_CTL_DSCV_IGNR_CRC(input);
			}
			else if( 0 == strcmp( argv[3], "boot_en" ) ) {
				Dd_Exs_Set_NF_IP_CTL_BOOT_EN(input);
			}
			else if( 0 == strcmp( argv[3], "iddq_en" ) ) {
				Dd_Exs_Set_NF_IP_CTL_IDDQ_EN(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "nf_rb_valid_tm" ) ) {
			if( 0 == strcmp( argv[3], "rb_valid_time" ) ) {
				Dd_Exs_Set_NF_RB_VALID_TM(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "nf_phy_ctrl_reg" ) ) {
			if( 0 == strcmp( argv[3], "phy_ctrl" ) ) {
				Dd_Exs_Set_NF_PHY_CTRL_REG(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "nf_phy_dq_tim" ) ) {
			if( 0 == strcmp( argv[3], "phy_dq_timing" ) ) {
				Dd_Exs_Set_NF_PHY_DQ_TIMING(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "nf_phy_dqs_tim" ) ) {
			if( 0 == strcmp( argv[3], "phy_dqs_timing" ) ) {
				Dd_Exs_Set_NF_PHY_DQS_TIMING(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "nf_phy_gt_lpbk" ) ) {
			if( 0 == strcmp( argv[3], "phy_gt_lpbk_ctl" ) ) {
				Dd_Exs_Set_NF_PHY_GT_LPBK_CTL(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "nf_phy_dll_mctl" ) ) {
			if( 0 == strcmp( argv[3], "phy_dll_mst_ctl" ) ) {
				Dd_Exs_Set_NF_PHY_DLL_MCTL(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "nf_phy_dll_sctl" ) ) {
			if( 0 == strcmp( argv[3], "phy_dll_slv_ctl" ) ) {
				Dd_Exs_Set_NF_PHY_DLL_SCTL(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "nf_boot_ecc_ctl" ) ) {
			if( 0 == strcmp( argv[3], "boot_ecc_sec_siz" ) ) {
				Dd_Exs_Set_NF_BOOT_ECC_CTL_SEC_SIZ(input);
			}
			else if( 0 == strcmp( argv[3], "bt_ecc_corr_str" ) ) {
				Dd_Exs_Set_NF_BOOT_ECC_CTL_CORR_STR(input);
			}
			else if( 0 == strcmp( argv[3], "boot_ecc_en" ) ) {
				Dd_Exs_Set_NF_BOOT_ECC_CTL_EN(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}else if ( 0 == strcmp( argv[2], "nf_wpx_cnt" ) ) {
			if( 0 == strcmp( argv[3], "wpx_cnt" ) ) {
				Dd_Exs_Set_NF_NFWPX_CNT(input);
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}
		else{
			// Input parameter NG
			Ddim_Print(("please check 2nd parameter!!\n"));
		}
	}
/*********************************************************/ 
	else if( 0 == strcmp( argv[1], "clr" ) ) {
//-------------------------------------------------------// 
		if( 0 == strcmp( argv[2], "intstat" ) ) {
			if( 0 == strcmp( argv[3], "sd2_ris" ) ) {
				Dd_Exs_Get_INTSTAT_SD2_RIS();
			}
			else if( 0 == strcmp( argv[3], "sd2_fis" ) ) {
				Dd_Exs_Get_INTSTAT_SD2_FIS();
			}
			else if( 0 == strcmp( argv[3], "sd3_ris" ) ) {
				Dd_Exs_Get_INTSTAT_SD3_RIS();
			}
			else if( 0 == strcmp( argv[3], "sd3_fis" ) ) {
				Dd_Exs_Get_INTSTAT_SD3_FIS();
			}
			else if( 0 == strcmp( argv[3], "sd4_ris" ) ) {
				Dd_Exs_Get_INTSTAT_SD4_RIS();
			}
			else if( 0 == strcmp( argv[3], "sd4_fis" ) ) {
				Dd_Exs_Get_INTSTAT_SD4_FIS();
			}
			else if( 0 == strcmp( argv[3], "pcie0_ris" ) ) {
				Dd_Exs_Get_INTSTAT_PCIe0_RIS();
			}
			else if( 0 == strcmp( argv[3], "pcie0_fis" ) ) {
				Dd_Exs_Get_INTSTAT_PCIe0_FIS();
			}
			else if( 0 == strcmp( argv[3], "pcie1_ris" ) ) {
				Dd_Exs_Get_INTSTAT_PCIe1_RIS();
			}
			else if( 0 == strcmp( argv[3], "pcie1_fis" ) ) {
				Dd_Exs_Get_INTSTAT_PCIe1_FIS();
			}
			else if( 0 == strcmp( argv[3], "usb2vbus_ris" ) ) {
				Dd_Exs_Get_INTSTAT_USB2VBUS_RIS();
			}
			else if( 0 == strcmp( argv[3], "usb2vbus_fis" ) ) {
				Dd_Exs_Get_INTSTAT_USB2VBUS_FIS();
			}
			else if( 0 == strcmp( argv[3], "usb2id_ris" ) ) {
				Dd_Exs_Get_INTSTAT_USB2ID_RIS();
			}
			else if( 0 == strcmp( argv[3], "usb2id_fis" ) ) {
				Dd_Exs_Get_INTSTAT_USB2ID_FIS();
			}
			else if( 0 == strcmp( argv[3], "usb3vbus_ris" ) ) {
				Dd_Exs_Get_INTSTAT_USB3VBUS_RIS();
			}
			else if( 0 == strcmp( argv[3], "usb3vbus_fis" ) ) {
				Dd_Exs_Get_INTSTAT_USB3VBUS_FIS();
			}
			else if( 0 == strcmp( argv[3], "usb3id_ris" ) ) {
				Dd_Exs_Get_INTSTAT_USB3ID_RIS();
			}
			else if( 0 == strcmp( argv[3], "usb3id_fis" ) ) {
				Dd_Exs_Get_INTSTAT_USB3ID_FIS();
			}
			else{
				// Input parameter NG
				Ddim_Print(("please check 3rd parameter!!\n"));
				return;
			}
//-------------------------------------------------------// 
		}
		else{
			// Input parameter NG
			Ddim_Print(("please check 2nd parameter!!\n"));
		}
	}
/*********************************************************/ 
	else if( 0 == strcmp( argv[1], "int" ) ) {
//-------------------------------------------------------// 
		if( 0 == strcmp( argv[2], "hdr" ) ) {
			Dd_EXS_Int_Handler(&intst);
//-------------------------------------------------------// 
		}
		else{
			// Input parameter NG
			Ddim_Print(("please check 2nd parameter!!\n"));
		}
	}
/*********************************************************/ 
}
