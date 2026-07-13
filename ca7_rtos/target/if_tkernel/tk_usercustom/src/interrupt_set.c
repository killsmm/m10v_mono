/**
 * @file		interrupt_set.c
 * @brief		interrupt setting process api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <mptk_cfg_label.h>
#include <mptk_typedef.h>
#include <mptk_syscall.h>

#include "interrupt_set.h"
#include "os_user_custom.h"

/* Interrupt Handler Function */
extern void BF_Sgi_Interrupt0(void);
extern void BF_Sgi_Interrupt1(void);
extern void BF_Sgi_Interrupt2(void);
extern void BF_Sgi_Interrupt3(void);
extern void BF_Sgi_Interrupt4(void);
extern void BF_Sgi_Interrupt5(void);
extern void BF_Sgi_Interrupt6(void);
extern void BF_Sgi_Interrupt7(void);
extern void BF_Sgi_Interrupt8(void);
extern void BF_Sgi_Interrupt9(void);
extern void BF_Sgi_Interrupt10(void);
extern void BF_Sgi_Interrupt11(void);
extern void BF_Sgi_Interrupt12(void);
extern void BF_Sgi_Interrupt13(void);
extern void BF_Sgi_Interrupt14(void);
extern void BF_Sgi_Interrupt15(void);
extern void BF_Ppi_Interrupt0(void);
extern void BF_Ppi_Interrupt1(void);
extern void BF_Ppi_Interrupt2(void);
extern void BF_Ppi_Interrupt3(void);
extern void BF_Ppi_Interrupt4(void);
extern void BF_Ppi_Interrupt5(void);
extern void BF_Ppi_Interrupt6(void);
extern void BF_Ppi_Interrupt7(void);
extern void BF_Ppi_Interrupt8(void);
extern void BF_Ppi_Interrupt9(void);
extern void BF_Ppi_Interrupt10(void);
extern void BF_Ppi_Interrupt11(void);
extern void BF_Ppi_Interrupt12(void);
extern void BF_Ppi_Interrupt13(void);
extern void BF_Ppi_Interrupt14(void);
extern void BF_Ppi_Interrupt15(void);
extern void BF_CortexA7_Cpu0_Communication_Ch_Receive_Int(void);
extern void BF_CortexA7_Cpu0_Communication_Ch_Trnsmit_Int(void);
extern void BF_CortexA7_Cpu1_Communication_Ch_Receive_Int(void);
extern void BF_CortexA7_Cpu1_Communication_Ch_Trnsmit_Int(void);
extern void BF_CortexA7_Cpu2_Communication_Ch_Receive_Int(void);
extern void BF_CortexA7_Cpu2_Communication_Ch_Trnsmit_Int(void);
extern void BF_CortexA7_Cpu3_Communication_Ch_Receive_Int(void);
extern void BF_CortexA7_Cpu3_Communication_Ch_Trnsmit_Int(void);
extern void BF_CortexA7_Cpu0_Crosstrigger_Int(void);
extern void BF_CortexA7_Cpu1_Crosstrigger_Int(void);
extern void BF_CortexA7_Cpu2_Crosstrigger_Int(void);
extern void BF_CortexA7_Cpu3_Crosstrigger_Int(void);
extern void BF_CortexA7_Cpu0_System_Metrics_Int(void);
extern void BF_CortexA7_Cpu1_System_Metrics_Int(void);
extern void BF_CortexA7_Cpu2_System_Metrics_Int(void);
extern void BF_CortexA7_Cpu3_System_Metrics_Int(void);
extern void BF_Axi_Transaction_Error_Int(void);
extern void BF_Snap_Peri_Xdmac_Ch0_Int(void);
extern void BF_Snap_Peri_Xdmac_Ch1_Int(void);
extern void BF_Snap_Peri_Xdmac_Ch2_Int(void);
extern void BF_Snap_Peri_Xdmac_Ch3_Int(void);
extern void BF_Watchdog_Timer_Int(void);
extern void BF_Ipcu0_Ch0_Int(void);
extern void BF_Ipcu0_Ch1_Int(void);
extern void BF_Ipcu0_Ch2_Int(void);
extern void BF_Ipcu0_Ch3_Int(void);
extern void BF_Ipcu0_Ch4_Int(void);
extern void BF_Ipcu0_Ch5_Int(void);
extern void BF_Ipcu0_Ch6_Int(void);
extern void BF_Ipcu0_Ch7_Int(void);
extern void BF_Ipcu0_Ch8_Int(void);
extern void BF_Ipcu0_Ch9_Int(void);
extern void BF_Ipcu0_Ch10_Int(void);
extern void BF_Ipcu0_Ch11_Int(void);
extern void BF_Ipcu0_Ch12_Int(void);
extern void BF_Ipcu0_Ch13_Int(void);
extern void BF_Ipcu0_Ch14_Int(void);
extern void BF_Ipcu0_Ch15_Int(void);
extern void BF_Ipcu1_Ch0_Int(void);
extern void BF_Ipcu1_Ch1_Int(void);
extern void BF_Ipcu1_Ch2_Int(void);
extern void BF_Ipcu1_Ch3_Int(void);
extern void BF_Ipcu1_Ch4_Int(void);
extern void BF_Ipcu1_Ch5_Int(void);
extern void BF_Ipcu1_Ch6_Int(void);
extern void BF_Ipcu1_Ch7_Int(void);
extern void BF_Ipcu1_Ch8_Int(void);
extern void BF_Ipcu1_Ch9_Int(void);
extern void BF_Ipcu1_Ch10_Int(void);
extern void BF_Ipcu1_Ch11_Int(void);
extern void BF_Ipcu1_Ch12_Int(void);
extern void BF_Ipcu1_Ch13_Int(void);
extern void BF_Ipcu1_Ch14_Int(void);
extern void BF_Ipcu1_Ch15_Int(void);
extern void BF_External_Ch16_Int(void);
extern void BF_External_Ch17_Int(void);
extern void BF_External_Ch18_Int(void);
extern void BF_External_Ch19_Int(void);
extern void BF_External_Ch20_Int(void);
extern void BF_External_Ch21_Int(void);
extern void BF_External_Ch22_Int(void);
extern void BF_External_Ch23_Int(void);
extern void BF_External_Ch24_Int(void);
extern void BF_External_Ch25_Int(void);
extern void BF_External_Ch26_Int(void);
extern void BF_External_Ch27_Int(void);
extern void BF_External_Ch28_Int(void);
extern void BF_External_Ch29_Int(void);
extern void BF_External_Ch30_Int(void);
extern void BF_External_Ch31_Int(void);
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
extern void BF_External_Ch32_Int(void);
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
extern void BF_Pmc_Dmac_Adc_Int(void);
extern void BF_Pmc_Dmac_Receive_Int(void);
extern void BF_Pmc_Dmac_Uart_Transmission_Data_Int(void);
extern void BF_Pmc_Dmac_Qspi_Transmission_Int(void);
extern void BF_Pmc_Dmac_Qspi_Receive_Int(void);
extern void BF_Pmc_Uart_Receive_Relation_Int(void);
extern void BF_Pmc_Uart_Transmission_Relation_Int(void);
extern void BF_Pmc_Qspi_Int(void);
extern void BF_Calendar_Alarm_Int(void);
extern void BF_Pmc_32bit_Timer_Under_Flow_Int(void);
extern void BF_Adc_Int(void);
extern void BF_Pmc_I2c_Int(void);
extern void BF_Pmc_Watchdog_Int(void);
extern void BF_Temperature_Int(void);
extern void BF_Pmu_Int(void);
extern void BF_Pmc_Gpio_Int(void);
extern void BF_Timer_Ch0_Int(void);
extern void BF_Timer_Ch1_Int(void);
extern void BF_Timer_Ch2_Int(void);
extern void BF_Timer_Ch3_Int(void);
extern void BF_Timer_Ch4_Int(void);
extern void BF_Timer_Ch5_Int(void);
extern void BF_Timer_Ch6_Int(void);
extern void BF_Timer_Ch7_Int(void);
extern void BF_Timer_Ch8_Int(void);
extern void BF_Timer_Ch9_Int(void);
extern void BF_Timer_Ch10_Int(void);
extern void BF_Timer_Ch11_Int(void);
extern void BF_Timer_Ch12_Int(void);
extern void BF_Timer_Ch13_Int(void);
extern void BF_I2c_Ch0_Int(void);
extern void BF_I2c_Ch1_Int(void);
extern void BF_I2c_Ch2_Int(void);
extern void BF_Updown_Counter_Ch0_Int(void);
extern void BF_Updown_Counter_Ch1_Int(void);
extern void BF_Updown_Counter_Ch2_Int(void);
extern void BF_Updown_Counter_Ch3_Int(void);
extern void BF_Updown_Counter_Ch4_Int(void);
extern void BF_Updown_Counter_Ch5_Int(void);
extern void BF_Pwm_Ch0_Ch3_Int(void);
extern void BF_Pwm_Ch4_Ch7_Int(void);
extern void BF_Pwm_Ch8_Ch11_Int(void);
extern void BF_Pwm_Ch12_Ch15_Int(void);
extern void BF_Pwm_Ch0_Ch3_Stp_Int(void);
extern void BF_Pwm_Ch4_Ch7_Stp_Int(void);
extern void BF_Pwm_Ch8_Ch11_Stp_Int(void);
extern void BF_Pwm_Ch12_Ch15_Stp_Int(void);
extern void BF_Audio_If_Ch0_Int(void);
extern void BF_Audio_If_Ch1_Int(void);
extern void BF_Audio_If_Ch2_Int(void);
extern void BF_Audio_If_Ch3_Int(void);
extern void BF_Audio_If_Ch4_Int(void);
extern void BF_Audio_If_Ch5_Int(void);
extern void BF_Peri_Hdmac0_Ch0_Int(void);
extern void BF_Peri_Hdmac0_Ch1_Int(void);
extern void BF_Peri_Hdmac0_Ch2_Int(void);
extern void BF_Peri_Hdmac0_Ch3_Int(void);
extern void BF_Peri_Hdmac0_Ch4_Int(void);
extern void BF_Peri_Hdmac0_Ch5_Int(void);
extern void BF_Peri_Hdmac0_Ch6_Int(void);
extern void BF_Peri_Hdmac0_Ch7_Int(void);
extern void BF_Peri_Hdmac1_Ch0_Int(void);
extern void BF_Peri_Hdmac1_Ch1_Int(void);
extern void BF_Peri_Hdmac1_Ch2_Int(void);
extern void BF_Peri_Hdmac1_Ch3_Int(void);
extern void BF_Peri_Hdmac1_Ch4_Int(void);
extern void BF_Peri_Hdmac1_Ch5_Int(void);
extern void BF_Peri_Hdmac1_Ch6_Int(void);
extern void BF_Peri_Hdmac1_Ch7_Int(void);
extern void BF_Uart_Ch0_Reception_Int(void);
extern void BF_Uart_Ch1_Reception_Int(void);
extern void BF_Uart_Ch2_Reception_Int(void);
extern void BF_Uart_Ch3_Reception_Int(void);
extern void BF_Uart_Ch4_Reception_Int(void);
extern void BF_Uart_Ch5_Reception_Int(void);
extern void BF_Uart_Ch6_Reception_Int(void);
extern void BF_Uart_Ch7_Reception_Int(void);
extern void BF_Uart_Ch0_Transmission_Int(void);
extern void BF_Uart_Ch1_Transmission_Int(void);
extern void BF_Uart_Ch2_Transmission_Int(void);
extern void BF_Uart_Ch3_Transmission_Int(void);
extern void BF_Uart_Ch4_Transmission_Int(void);
extern void BF_Uart_Ch5_Transmission_Int(void);
extern void BF_Uart_Ch6_Transmission_Int(void);
extern void BF_Uart_Ch7_Transmission_Int(void);
extern void BF_Spi_Ch0_Int(void);
extern void BF_Spi_Ch1_Int(void);
extern void BF_Spi_Ch2_Int(void);
extern void BF_Pdm_Ch0_Dma0_Int(void);
extern void BF_Pdm_Ch0_Dma1_Int(void);
extern void BF_Pdm_Ch0_Fifo_Overflow_Int(void);
extern void BF_Pdm_Ch1_Dma0_Int(void);
extern void BF_Pdm_Ch1_Dma1_Int(void);
extern void BF_Pdm_Ch1_Fifo_Overflow_Int(void);
extern void BF_Slimbus_Ch0_Int(void);
extern void BF_Slimbus_Ch1_Int(void);
extern void BF_Slimbus_Ch2_Int(void);
extern void BF_Slimbus_Ch3_Int(void);
extern void BF_Sdramc_Ch0_Int(void);
extern void BF_Sdramc_Ch1_Int(void);
extern void BF_Mxic_Unit0_Int(void);
extern void BF_Mxic_Unit1_Int(void);
extern void BF_Mxic_Unit2_Int(void);
extern void BF_Mxic_Unit3_Int(void);
extern void BF_Mxic_Unit4_Int(void);
extern void BF_Mxic_Unit5_Int(void);
extern void BF_Mxic_Unit6_Int(void);
extern void BF_Sen_Int(void);
extern void BF_Sen_Vd_Int(void);
extern void BF_Sen_Hd_Int(void);
extern void BF_Sensor_If_Macro0_Int(void);
extern void BF_Sensor_If_Macro1_Int(void);
extern void BF_Sensor_If_Macro2_Int(void);
extern void BF_Sensor_If_Macro3_Int(void);
extern void BF_Sro_Unit1_Int(void);
extern void BF_Sro_Unit1_Vd_Int(void);
extern void BF_Sro_Unit1_Hd_Int(void);
extern void BF_Sro_Unit2_Int(void);
extern void BF_Sro_Unit2_Vd_Int(void);
extern void BF_Sro_Unit2_Hd_Int(void);
extern void BF_B2b_Unit1_Int(void);
extern void BF_B2b_Unit1_Vd_Int(void);
extern void BF_B2b_Unit1_Hd_Int(void);
extern void BF_B2b_Unit2_Int(void);
extern void BF_B2b_Unit2_Vd_Int(void);
extern void BF_B2b_Unit2_Hd_Int(void);
extern void BF_Stat_Int(void);
extern void BF_Pas_Int(void);
extern void BF_Pas_Vd_Int(void);
extern void BF_Pas_Hd_Int(void);
extern void BF_B2r_Unit1_Int(void);
extern void BF_B2r_Unit2_Int(void);
extern void BF_Ltm_Unit1_Int(void);
extern void BF_Ltm_Unit2_Int(void);
extern void BF_R2y_Unit1_Int(void);
extern void BF_R2y_Unit2_Int(void);
extern void BF_Cnr_Unit1_Int(void);
extern void BF_Cnr_Unit2_Int(void);
extern void BF_Iip_Int(void);
extern void BF_Shdr_Int(void);
extern void BF_Shdr_Line_Int(void);
extern void BF_Me_Int(void);
extern void BF_Graphics_Int(void);
extern void BF_Jpeg_Int(void);
extern void BF_Raw_Int(void);
extern void BF_Ipu_Int(void);
extern void BF_Ela_Unit_Int(void);
extern void BF_Xch_Int(void);
extern void BF_Fpt_Unit0_Int(void);
extern void BF_Fpt_Unit1_Int(void);
extern void BF_Disp_Lcd_Int(void);
extern void BF_Disp_Hdmi_Int(void);
extern void BF_Disp_Mipi_Dsi_Int(void);
extern void BF_Mipi_Dsi_Tx_Int(void);
extern void BF_Disp_Dwch_Int(void);
extern void BF_Rdma_Int(void);
extern void BF_Hdmi_Master_Int(void);
extern void BF_Hdmi_Cec_Int(void);
extern void BF_BMH_Unit0_Int(void);
extern void BF_BMH_Unit1_Int(void);
extern void BF_Ribery_Rbr_Int(void);
extern void BF_Ribery_Rwdt_Int(void);
extern void BF_Interprocessor0_Int(void);
extern void BF_Interprocessor1_Int(void);
extern void BF_Interprocessor2_Int(void);
extern void BF_Interprocessor3_Int(void);
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
extern void BF_Wave_Int(void);
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
extern void BF_Dsp_Peripheral_Unit_Ipcu0_Int(void);
extern void BF_Dsp_Peripheral_Unit_Ipcu1_Int(void);
extern void BF_Dsp_Peripheral_Unit_Ipcu2_Int(void);
extern void BF_Dsp_Peripheral_Unit_Ipcu3_Int(void);
extern void BF_Dsp_Debug_Int(void);
extern void BF_Nand_Flash_Int(void);
extern void BF_Pcie_Ch0_Msi_Int(void);
extern void BF_Pcie_Ch0_System_Error_Int(void);
extern void BF_Pcie_Ch0_Intx_Int(void);
extern void BF_Pcie_Ch0_Core_Int(void);
extern void BF_Pcie_Ch0_DMA_Int(void);
extern void BF_Pcie_Ch1_Msi_Int(void);
extern void BF_Pcie_Ch1_System_Error_Int(void);
extern void BF_Pcie_Ch1_Intx_Int(void);
extern void BF_Pcie_Ch1_Core_Int(void);
extern void BF_Pcie_Ch1_DMA_Int(void);
extern void BF_Exs_Xdmac_Ch0_Int(void);
extern void BF_Exs_Xdmac_Ch1_Int(void);
extern void BF_Exs_Xdmac_Ch2_Int(void);
extern void BF_Exs_Xdmac_Ch3_Int(void);
extern void BF_Exs_Hdmac_Ch0_Int(void);
extern void BF_Exs_Hdmac_Ch1_Int(void);
extern void BF_Exs_Hdmac_Ch2_Int(void);
extern void BF_Exs_Hdmac_Ch3_Int(void);
extern void BF_Usb20_Hdc_Int(void);
extern void BF_Usb20_Host_Ehci_Int(void);
extern void BF_Usb20_Host_Ohci_Int(void);
extern void BF_Usb30_Int(void);
extern void BF_Sd_Ch3_Int(void);
extern void BF_Sd_Ch3_Sdio_Int(void);
extern void BF_Sd_Ch3_Wakeup_Int(void);
extern void BF_Sd_Ch2_Uhs1_Int(void);
extern void BF_Sd_Ch2_Sdio_Int(void);
extern void BF_Sd_Ch2_Uhs1_Wakeup_Int(void);
extern void BF_Sd_Ch2_Uhs2_Int(void);
extern void BF_Sd_Ch2_Uhs2_Wakeup_Int(void);
extern void BF_Sd_Ch1_Int(void);
extern void BF_Sd_Ch1_Sdio_Int(void);
extern void BF_Sd_Ch1_Wakeup_Int(void);
extern void BF_Emmc_Int(void);
extern void BF_Emmc_Wakeup_Int(void);
extern void BF_Relc_Int(void);
extern void BF_Netsec_A_Int(void);
extern void BF_Netsec_B_Int(void);
extern void BF_Sd_Pcie_Detect_Int(void);

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
/* Interrupt Priority Value */
#define	D_INT_PRI00		(0x00)		// Interrupt Priority 00:(0x00)
#define	D_INT_PRI01		(0x08)		// Interrupt Priority 01:(0x08)
#define	D_INT_PRI02		(0x10)		// Interrupt Priority 02:(0x10)
#define	D_INT_PRI03		(0x18)		// Interrupt Priority 03:(0x18)
#define	D_INT_PRI04		(0x20)		// Interrupt Priority 04:(0x20)
#define	D_INT_PRI05		(0x28)		// Interrupt Priority 05:(0x28)
#define	D_INT_PRI06		(0x30)		// Interrupt Priority 06:(0x30)
#define	D_INT_PRI07		(0x38)		// Interrupt Priority 07:(0x38)
#define	D_INT_PRI08		(0x40)		// Interrupt Priority 08:(0x40)
#define	D_INT_PRI09		(0x48)		// Interrupt Priority 09:(0x48)
#define	D_INT_PRI10		(0x50)		// Interrupt Priority 10:(0x50)
#define	D_INT_PRI11		(0x58)		// Interrupt Priority 11:(0x58)
#define	D_INT_PRI12		(0x60)		// Interrupt Priority 12:(0x60)
#define	D_INT_PRI13		(0x68)		// Interrupt Priority 13:(0x68)
#define	D_INT_PRI14		(0x70)		// Interrupt Priority 14:(0x70)
#define	D_INT_PRI15		(0x78)		// Interrupt Priority 15:(0x78)
#define	D_INT_PRI16		(0x80)		// Interrupt Priority 16:(0x80)
#define	D_INT_PRI17		(0x88)		// Interrupt Priority 17:(0x88)
#define	D_INT_PRI18		(0x90)		// Interrupt Priority 18:(0x90)
#define	D_INT_PRI19		(0x98)		// Interrupt Priority 19:(0x98)
#define	D_INT_PRI20		(0xA0)		// Interrupt Priority 20:(0xA0)
#define	D_INT_PRI21		(0xA8)		// Interrupt Priority 21:(0xA8)
#define	D_INT_PRI22		(0xB0)		// Interrupt Priority 22:(0xB0)
#define	D_INT_PRI23		(0xB8)		// Interrupt Priority 23:(0xB8)
#define	D_INT_PRI24		(0xC0)		// Interrupt Priority 24:(0xC0)
#define	D_INT_PRI25		(0xC8)		// Interrupt Priority 25:(0xC8)
#define	D_INT_PRI26		(0xD0)		// Interrupt Priority 26:(0xD0)
#define	D_INT_PRI27		(0xD8)		// Interrupt Priority 27:(0xD8)
#define	D_INT_PRI28		(0xE0)		// Interrupt Priority 28:(0xE0)
#define	D_INT_PRI29		(0xE8)		// Interrupt Priority 29:(0xE8)
#define	D_INT_PRI30		(0xF0)		// Interrupt Priority 30:(0xF0)
#define	D_INT_PRI31		(0xF8)		// Interrupt Priority 31:(0xF8)

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/* Interrupt Handler Structure */
typedef struct {
	UINT	enable;			// 0:Disable 1:Enable
	UINT	assprc;
	UINT	trigger;
	UINT	priority;
} T_OS_USR_INTERRUPT_VER;

typedef struct {
	UINT					dintno;
	T_DINT					dint;
	T_OS_USR_INTERRUPT_VER	tkernel_ver;
	T_OS_USR_INTERRUPT_VER	linux_ver;
	T_OS_USR_INTERRUPT_VER	tkernel_only;
} T_OS_USR_INTERRUPT;

/* Interrupt Handler Initialize Table */
static const T_OS_USR_INTERRUPT gOS_Int_Tbl[] =
{
//    {   0,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sgi_Interrupt0},
//          {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00} },
//    {   1,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sgi_Interrupt1},
//          {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00} },
//    {   2,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sgi_Interrupt2},
//          {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00} },
    {   3,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sgi_Interrupt3},
          {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00} },
    {   4,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sgi_Interrupt4},
          {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00} },
    {   5,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sgi_Interrupt5},
          {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00} },          
    {   6,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sgi_Interrupt6},
          {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00} },
    {   7,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sgi_Interrupt7},
          {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00} },
    {   8,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sgi_Interrupt8},
          {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00} },
    {   9,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sgi_Interrupt9},
          {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00} },
    {  10,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sgi_Interrupt10},
          {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00} },
    {  11,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sgi_Interrupt11},
          {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00} },
    {  12,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sgi_Interrupt12},
          {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00} },
    {  13,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sgi_Interrupt13},
          {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00} },
    {  14,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sgi_Interrupt14},
          {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00} },
    {  15,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sgi_Interrupt15},
          {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI00} },
    {  16,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ppi_Interrupt0},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    {  17,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ppi_Interrupt1},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    {  18,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ppi_Interrupt2},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    {  19,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ppi_Interrupt3},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    {  20,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ppi_Interrupt4},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    {  21,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ppi_Interrupt5},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    {  22,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ppi_Interrupt6},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    {  23,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ppi_Interrupt7},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    {  24,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ppi_Interrupt8},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    {  25,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ppi_Interrupt9},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    {  26,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ppi_Interrupt10},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    {  27,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ppi_Interrupt11},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    {  28,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ppi_Interrupt12},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    {  29,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ppi_Interrupt13},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    {  30,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ppi_Interrupt14},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    {  31,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ppi_Interrupt15},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    {  32,{ (TA_HLNG | TA_ASSPRC),(FP)BF_CortexA7_Cpu0_Communication_Ch_Receive_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  33,{ (TA_HLNG | TA_ASSPRC),(FP)BF_CortexA7_Cpu0_Communication_Ch_Trnsmit_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  34,{ (TA_HLNG | TA_ASSPRC),(FP)BF_CortexA7_Cpu1_Communication_Ch_Receive_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  35,{ (TA_HLNG | TA_ASSPRC),(FP)BF_CortexA7_Cpu1_Communication_Ch_Trnsmit_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  36,{ (TA_HLNG | TA_ASSPRC),(FP)BF_CortexA7_Cpu2_Communication_Ch_Receive_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  37,{ (TA_HLNG | TA_ASSPRC),(FP)BF_CortexA7_Cpu2_Communication_Ch_Trnsmit_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  38,{ (TA_HLNG | TA_ASSPRC),(FP)BF_CortexA7_Cpu3_Communication_Ch_Receive_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  39,{ (TA_HLNG | TA_ASSPRC),(FP)BF_CortexA7_Cpu3_Communication_Ch_Trnsmit_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  40,{ (TA_HLNG | TA_ASSPRC),(FP)BF_CortexA7_Cpu0_Crosstrigger_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  41,{ (TA_HLNG | TA_ASSPRC),(FP)BF_CortexA7_Cpu1_Crosstrigger_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  42,{ (TA_HLNG | TA_ASSPRC),(FP)BF_CortexA7_Cpu2_Crosstrigger_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  43,{ (TA_HLNG | TA_ASSPRC),(FP)BF_CortexA7_Cpu3_Crosstrigger_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  44,{ (TA_HLNG | TA_ASSPRC),(FP)BF_CortexA7_Cpu0_System_Metrics_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  45,{ (TA_HLNG | TA_ASSPRC),(FP)BF_CortexA7_Cpu1_System_Metrics_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  46,{ (TA_HLNG | TA_ASSPRC),(FP)BF_CortexA7_Cpu2_System_Metrics_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  47,{ (TA_HLNG | TA_ASSPRC),(FP)BF_CortexA7_Cpu3_System_Metrics_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  48,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Axi_Transaction_Error_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  49,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Snap_Peri_Xdmac_Ch0_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  50,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Snap_Peri_Xdmac_Ch1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  51,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Snap_Peri_Xdmac_Ch2_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  52,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Snap_Peri_Xdmac_Ch3_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  53,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Watchdog_Timer_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  54,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu0_Ch0_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  55,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu0_Ch1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  56,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu0_Ch2_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  57,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu0_Ch3_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  58,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu0_Ch4_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  59,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu0_Ch5_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  60,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu0_Ch6_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  61,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu0_Ch7_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  62,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu0_Ch8_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  63,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu0_Ch9_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  64,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu0_Ch10_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  65,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu0_Ch11_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  66,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu0_Ch12_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  67,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu0_Ch13_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  68,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu0_Ch14_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  69,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu0_Ch15_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  70,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu1_Ch0_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  71,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu1_Ch1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  72,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu1_Ch2_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  73,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu1_Ch3_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  74,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu1_Ch4_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  75,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu1_Ch5_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  76,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu1_Ch6_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  77,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu1_Ch7_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  78,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu1_Ch8_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  79,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu1_Ch9_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  80,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu1_Ch10_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  81,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu1_Ch11_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  82,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu1_Ch12_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  83,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu1_Ch13_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  84,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu1_Ch14_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  85,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipcu1_Ch15_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    {  86,{ (TA_HLNG | TA_ASSPRC),(FP)BF_External_Ch16_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  87,{ (TA_HLNG | TA_ASSPRC),(FP)BF_External_Ch17_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  88,{ (TA_HLNG | TA_ASSPRC),(FP)BF_External_Ch18_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  89,{ (TA_HLNG | TA_ASSPRC),(FP)BF_External_Ch19_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  90,{ (TA_HLNG | TA_ASSPRC),(FP)BF_External_Ch20_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  91,{ (TA_HLNG | TA_ASSPRC),(FP)BF_External_Ch21_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  92,{ (TA_HLNG | TA_ASSPRC),(FP)BF_External_Ch22_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  93,{ (TA_HLNG | TA_ASSPRC),(FP)BF_External_Ch23_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  94,{ (TA_HLNG | TA_ASSPRC),(FP)BF_External_Ch24_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  95,{ (TA_HLNG | TA_ASSPRC),(FP)BF_External_Ch25_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  96,{ (TA_HLNG | TA_ASSPRC),(FP)BF_External_Ch26_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  97,{ (TA_HLNG | TA_ASSPRC),(FP)BF_External_Ch27_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  98,{ (TA_HLNG | TA_ASSPRC),(FP)BF_External_Ch28_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    {  99,{ (TA_HLNG | TA_ASSPRC),(FP)BF_External_Ch29_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 100,{ (TA_HLNG | TA_ASSPRC),(FP)BF_External_Ch30_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 101,{ (TA_HLNG | TA_ASSPRC),(FP)BF_External_Ch31_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 102,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pmc_Dmac_Adc_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 103,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pmc_Dmac_Receive_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 104,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pmc_Dmac_Uart_Transmission_Data_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 105,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pmc_Dmac_Qspi_Transmission_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 106,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pmc_Dmac_Qspi_Receive_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 107,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pmc_Uart_Receive_Relation_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 108,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pmc_Uart_Transmission_Relation_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 109,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pmc_Qspi_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 110,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Calendar_Alarm_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 111,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pmc_32bit_Timer_Under_Flow_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 112,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Adc_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 113,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pmc_I2c_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 114,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pmc_Watchdog_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 115,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Temperature_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 116,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pmu_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 117,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pmc_Gpio_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 118,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Timer_Ch0_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 119,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Timer_Ch1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 120,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Timer_Ch2_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 121,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Timer_Ch3_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 122,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Timer_Ch4_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 123,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Timer_Ch5_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 124,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Timer_Ch6_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 125,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Timer_Ch7_Int},
          {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 126,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Timer_Ch8_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 127,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Timer_Ch9_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 128,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Timer_Ch10_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 129,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Timer_Ch11_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 130,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Timer_Ch12_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
//    { 131 },
//    { 132 },
    { 133,{ (TA_HLNG | TA_ASSPRC),(FP)BF_I2c_Ch0_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 134,{ (TA_HLNG | TA_ASSPRC),(FP)BF_I2c_Ch1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 135,{ (TA_HLNG | TA_ASSPRC),(FP)BF_I2c_Ch2_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 136,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Updown_Counter_Ch0_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 137,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Updown_Counter_Ch1_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 138,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Updown_Counter_Ch2_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 139,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Updown_Counter_Ch3_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 140,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Updown_Counter_Ch4_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 141,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Updown_Counter_Ch5_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 142,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pwm_Ch0_Ch3_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 143,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pwm_Ch4_Ch7_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 144,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pwm_Ch8_Ch11_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 145,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pwm_Ch12_Ch15_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 146,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pwm_Ch0_Ch3_Stp_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 147,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pwm_Ch4_Ch7_Stp_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 148,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pwm_Ch8_Ch11_Stp_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 149,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pwm_Ch12_Ch15_Stp_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 150,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Audio_If_Ch0_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 151,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Audio_If_Ch1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 152,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Audio_If_Ch2_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 153,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Audio_If_Ch3_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 154,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Audio_If_Ch4_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 155,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Audio_If_Ch5_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 156,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac0_Ch0_Int},
          {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08} },
    { 157,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac0_Ch1_Int},
          {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08} },
    { 158,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac0_Ch2_Int},
          {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08} },
    { 159,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac0_Ch3_Int},
          {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08} },
    { 160,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac0_Ch4_Int},
          {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08} },
    { 161,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac0_Ch5_Int},
          {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08} },
    { 162,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac0_Ch6_Int},
          {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08} },
    { 163,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac0_Ch7_Int},
          {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI08} },
#ifndef CO_RTOS_CPUx2
    { 164,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac1_Ch0_Int},
          {1, (TP_PRC1 | TP_PRC2 | TP_PRC3), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2 | TP_PRC3), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2 | TP_PRC3), D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    { 165,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac1_Ch1_Int},
          {1, (TP_PRC1 | TP_PRC2 | TP_PRC3), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2 | TP_PRC3), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2 | TP_PRC3), D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    { 166,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac1_Ch2_Int},
          {1, (TP_PRC1 | TP_PRC2 | TP_PRC3), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2 | TP_PRC3), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2 | TP_PRC3), D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    { 167,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac1_Ch3_Int},
          {1, (TP_PRC1 | TP_PRC2 | TP_PRC3), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2 | TP_PRC3), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2 | TP_PRC3), D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    { 168,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac1_Ch4_Int},
          {1, (TP_PRC1 | TP_PRC2 | TP_PRC3), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2 | TP_PRC3), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2 | TP_PRC3), D_OS_USER_IM_LEVEL, D_INT_PRI00} },
#else
    { 164,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac1_Ch0_Int},
          {1, (TP_PRC1 | TP_PRC2), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2), D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    { 165,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac1_Ch1_Int},
          {1, (TP_PRC1 | TP_PRC2), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2), D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    { 166,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac1_Ch2_Int},
          {1, (TP_PRC1 | TP_PRC2), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2), D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    { 167,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac1_Ch3_Int},
          {1, (TP_PRC1 | TP_PRC2), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2), D_OS_USER_IM_LEVEL, D_INT_PRI00} },
    { 168,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac1_Ch4_Int},
          {1, (TP_PRC1 | TP_PRC2), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2), D_OS_USER_IM_LEVEL, D_INT_PRI00}, {1, (TP_PRC1 | TP_PRC2), D_OS_USER_IM_LEVEL, D_INT_PRI00} },
#endif
    { 169,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac1_Ch5_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 170,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac1_Ch6_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 171,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Peri_Hdmac1_Ch7_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 172,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Uart_Ch0_Reception_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 173,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Uart_Ch1_Reception_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 174,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Uart_Ch2_Reception_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 175,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Uart_Ch3_Reception_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 176,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Uart_Ch4_Reception_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 177,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Uart_Ch5_Reception_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 178,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Uart_Ch6_Reception_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 179,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Uart_Ch7_Reception_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 180,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Uart_Ch0_Transmission_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 181,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Uart_Ch1_Transmission_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 182,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Uart_Ch2_Transmission_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 183,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Uart_Ch3_Transmission_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 184,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Uart_Ch4_Transmission_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 185,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Uart_Ch5_Transmission_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 186,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Uart_Ch6_Transmission_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 187,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Uart_Ch7_Transmission_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 188,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Spi_Ch0_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 189,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Spi_Ch1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 190,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Spi_Ch2_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 191,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pdm_Ch0_Dma0_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI08} },
    { 192,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pdm_Ch0_Dma1_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI08} },
    { 193,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pdm_Ch0_Fifo_Overflow_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 194,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pdm_Ch1_Dma0_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI08} },
    { 195,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pdm_Ch1_Dma1_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI08}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI08} },
    { 196,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pdm_Ch1_Fifo_Overflow_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 197,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Slimbus_Ch0_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 198,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Slimbus_Ch1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 199,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Slimbus_Ch2_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 200,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Slimbus_Ch3_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 201,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sdramc_Ch0_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 202,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sdramc_Ch1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 203,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Mxic_Unit0_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI02}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 204,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Mxic_Unit1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI02}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI02}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI02} },
    { 205,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Mxic_Unit2_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI02}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI02}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI02} },
    { 206,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Mxic_Unit3_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI02}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI02}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI02} },
    { 207,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Mxic_Unit4_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI02}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI02}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI02} },
    { 208,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Mxic_Unit5_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI02}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI02}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI02} },
    { 209,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Mxic_Unit6_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI02}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI02}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI02} },
    { 210,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sen_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 211,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sen_Vd_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 212,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sen_Hd_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 213,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sensor_If_Macro0_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 214,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sensor_If_Macro1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 215,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sensor_If_Macro2_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 216,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sensor_If_Macro3_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 217,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sro_Unit1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 218,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sro_Unit1_Vd_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 219,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sro_Unit1_Hd_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 220,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sro_Unit2_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 221,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sro_Unit2_Vd_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 222,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sro_Unit2_Hd_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 223,{ (TA_HLNG | TA_ASSPRC),(FP)BF_B2b_Unit1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 224,{ (TA_HLNG | TA_ASSPRC),(FP)BF_B2b_Unit1_Vd_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 225,{ (TA_HLNG | TA_ASSPRC),(FP)BF_B2b_Unit1_Hd_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 226,{ (TA_HLNG | TA_ASSPRC),(FP)BF_B2b_Unit2_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 227,{ (TA_HLNG | TA_ASSPRC),(FP)BF_B2b_Unit2_Vd_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 228,{ (TA_HLNG | TA_ASSPRC),(FP)BF_B2b_Unit2_Hd_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 229,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Stat_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 230,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pas_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 231,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pas_Vd_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 232,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pas_Hd_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI05} },
    { 233,{ (TA_HLNG | TA_ASSPRC),(FP)BF_B2r_Unit1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 234,{ (TA_HLNG | TA_ASSPRC),(FP)BF_B2r_Unit2_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 235,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ltm_Unit1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 236,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ltm_Unit2_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 237,{ (TA_HLNG | TA_ASSPRC),(FP)BF_R2y_Unit1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 238,{ (TA_HLNG | TA_ASSPRC),(FP)BF_R2y_Unit2_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 239,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Cnr_Unit1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 240,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Cnr_Unit2_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 241,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Iip_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 242,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Shdr_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 243,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Shdr_Line_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 244,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Me_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 245,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Graphics_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 246,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Jpeg_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 247,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Raw_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 248,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ipu_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 249,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ela_Unit_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 250,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Xch_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 251,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Fpt_Unit0_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 252,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Fpt_Unit1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 253,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Disp_Lcd_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI10}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI10}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI10} },
    { 254,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Disp_Hdmi_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 255,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Disp_Mipi_Dsi_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 256,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Mipi_Dsi_Tx_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 257,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Disp_Dwch_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 258,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Rdma_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 259,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Hdmi_Master_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 260,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Hdmi_Cec_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 261,{ (TA_HLNG | TA_ASSPRC),(FP)BF_BMH_Unit0_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 262,{ (TA_HLNG | TA_ASSPRC),(FP)BF_BMH_Unit1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 263,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ribery_Rbr_Int},
          {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 264,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Ribery_Rwdt_Int},
          {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 265,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Interprocessor0_Int},
          {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 266,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Interprocessor1_Int},
          {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 267,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Interprocessor2_Int},
          {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 268,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Interprocessor3_Int},
          {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 269,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Dsp_Peripheral_Unit_Ipcu0_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 270,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Dsp_Peripheral_Unit_Ipcu1_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 271,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Dsp_Peripheral_Unit_Ipcu2_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 272,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Dsp_Peripheral_Unit_Ipcu3_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 273,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Dsp_Debug_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 274,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Nand_Flash_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 275,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pcie_Ch0_Msi_Int},
          {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI15} },
    { 276,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pcie_Ch0_System_Error_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 277,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pcie_Ch0_Intx_Int},
          {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI15} },
    { 278,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pcie_Ch0_Core_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 279,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pcie_Ch0_DMA_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 280,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pcie_Ch1_Msi_Int},
          {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI15} },
    { 281,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pcie_Ch1_System_Error_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 282,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pcie_Ch1_Intx_Int},
          {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_EDGE, D_INT_PRI15} },
    { 283,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pcie_Ch1_Core_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 284,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Pcie_Ch1_DMA_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
    { 285,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Exs_Xdmac_Ch0_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 286,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Exs_Xdmac_Ch1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 287,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Exs_Xdmac_Ch2_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 288,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Exs_Xdmac_Ch3_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 289,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Exs_Hdmac_Ch0_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 290,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Exs_Hdmac_Ch1_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 291,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Exs_Hdmac_Ch2_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 292,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Exs_Hdmac_Ch3_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 293,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Usb20_Hdc_Int},
// --- REMOVE_USB BEGIN ---
#if !defined(CO_LINUX_VERSION_ON) && defined(CO_USB_ON)
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
#else
// --- REMOVE_USB END ---
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
// --- REMOVE_USB BEGIN ---
#endif
// --- REMOVE_USB END ---
    { 294,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Usb20_Host_Ehci_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 295,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Usb20_Host_Ohci_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 296,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Usb30_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 297,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sd_Ch3_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 298,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sd_Ch3_Sdio_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 299,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sd_Ch3_Wakeup_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 300,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sd_Ch2_Uhs1_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 301,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sd_Ch2_Sdio_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 302,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sd_Ch2_Uhs1_Wakeup_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 303,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sd_Ch2_Uhs2_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 304,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sd_Ch2_Uhs2_Wakeup_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 305,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sd_Ch1_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 306,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sd_Ch1_Sdio_Int},
          {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 307,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sd_Ch1_Wakeup_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 308,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Emmc_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 309,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Emmc_Wakeup_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 310,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Relc_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 311,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Netsec_A_Int},
          {1, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 312,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Netsec_B_Int},
          {1, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC4, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 313,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Sd_Pcie_Detect_Int},
          {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
    { 314,{ (TA_HLNG | TA_ASSPRC),(FP)BF_Wave_Int},
          {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14}, {1, TP_PRC2, D_OS_USER_IM_LEVEL, D_INT_PRI14} },
    { 315,{ (TA_HLNG | TA_ASSPRC),(FP)BF_External_Ch32_Int},
          {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15}, {0, TP_PRC1, D_OS_USER_IM_LEVEL, D_INT_PRI15} },
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
    { -1,{ 0, NULL}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} }
};

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
void Interrupt_Set_Enable(void)
{
	T_DINT	dint;
	INT		i;
	ER		ercd;
	INT		val[1];
	
	// Interrupt Set
	i = 0;
	tk_get_cfn(MPTK_CFG_TKERPRNUM, val, 1);
	for ( ;; ) {
		if ( gOS_Int_Tbl[i].dintno == -1 ) {
			break;
		}
		// Set INTID
		dint = gOS_Int_Tbl[i].dint;
		dint.assprc = 0;
#ifdef CO_TKERNEL_LINUX_COOP
#ifdef CO_LINUX_VERSION_ON
		if ( gOS_Int_Tbl[i].linux_ver.assprc != TP_PRC4 ) {
			dint.assprc = gOS_Int_Tbl[i].linux_ver.assprc;
		}
#else
		if ( gOS_Int_Tbl[i].tkernel_ver.assprc != TP_PRC4 ) {
			dint.assprc = gOS_Int_Tbl[i].tkernel_ver.assprc;
		}
#endif
#else
		if ( gOS_Int_Tbl[i].tkernel_only.assprc != TP_PRC4 ) {
			dint.assprc = gOS_Int_Tbl[i].tkernel_only.assprc;
		}
#endif
		if ( dint.assprc != 0 ) {
			if ( val[0] == 1 ) {
				dint.assprc = TP_PRC1;
			}
			ercd = tk_def_int( gOS_Int_Tbl[i].dintno, &dint );
			if ( ercd < 0 ) {
				while (1);
			}
		}
		
		// Set Trigger
#ifdef CO_TKERNEL_LINUX_COOP
#ifdef CO_LINUX_VERSION_ON
		OS_User_SetIntMode( gOS_Int_Tbl[i].dintno, gOS_Int_Tbl[i].linux_ver.trigger );
#else
		OS_User_SetIntMode( gOS_Int_Tbl[i].dintno, gOS_Int_Tbl[i].tkernel_ver.trigger );
#endif
#else
		OS_User_SetIntMode( gOS_Int_Tbl[i].dintno, gOS_Int_Tbl[i].tkernel_only.trigger );
#endif
		
		// Set Enable Interrupt
#ifdef CO_TKERNEL_LINUX_COOP
#ifdef CO_LINUX_VERSION_ON
		if ( gOS_Int_Tbl[i].linux_ver.enable == 1 ) {
			OS_User_EnableInt( gOS_Int_Tbl[i].dintno, gOS_Int_Tbl[i].linux_ver.priority );
		}
#else
		if ( gOS_Int_Tbl[i].tkernel_ver.enable == 1 ) {
			OS_User_EnableInt( gOS_Int_Tbl[i].dintno, gOS_Int_Tbl[i].tkernel_ver.priority );
		}
#endif
#else
		if ( gOS_Int_Tbl[i].tkernel_only.enable == 1 ) {
			OS_User_EnableInt( gOS_Int_Tbl[i].dintno, gOS_Int_Tbl[i].tkernel_only.priority );
		}
#endif
		i++;
	}
}

void Interrupt_ReSet_Enable(int intid, int intrpt_pri, int cpu_target){
	T_DINT	dint;
	INT		num;
	INT		i;
	
	// Interrupt Set
	num = sizeof(gOS_Int_Tbl) / sizeof(T_OS_USR_INTERRUPT);
	
	for ( i = 0; i < num; i++ ) {
		if( gOS_Int_Tbl[i].dintno == intid ){
			dint = gOS_Int_Tbl[i].dint;
			dint.assprc = cpu_target;
			tk_def_int( intid, &dint );
			OS_User_EnableInt( (OS_USER_INTID)intid, (OS_USER_INTLV)intrpt_pri );
			break;
		}
	}
}
