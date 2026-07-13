/**
 * @file		interrupt.h
 * @brief		interrupt control api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include "ddim_typedef.h"
#include "fj_arm.h"
#include "fj_std.h"
#include "fj_peripheral.h"
#include "os_user_custom.h"

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
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/* Nothing Special */

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
FJ_ERR_CODE fj_ext_interrupt_set_config( FJ_GPIO_PORT port, FJ_EXT_INT_CONFIG* int_config );
FJ_ERR_CODE fj_ext_interrupt_set_enable( FJ_GPIO_PORT port, UCHAR enable );
FJ_ERR_CODE fj_ext_interrupt_set_level( FJ_GPIO_PORT port, UCHAR level );
FJ_ERR_CODE fj_ext_interrupt_get_level( FJ_GPIO_PORT port, UCHAR* level );
FJ_ERR_CODE fj_ext_interrupt_clear_req( FJ_GPIO_PORT port ); 
FJ_ERR_CODE fj_gic_ctrl(UINT32 intid, ULONG set_enable, LONG intrpt_pri, E_FJ_GIC_TARGET_CPU cpu_target);
FJ_ERR_CODE fj_gic_clear_pending(UINT32 intid);

VOID BF_Sgi_Interrupt0(VOID);
VOID BF_Sgi_Interrupt1(VOID);
VOID BF_Sgi_Interrupt2(VOID);
VOID BF_Sgi_Interrupt3(VOID);
VOID BF_Sgi_Interrupt4(VOID);
VOID BF_Sgi_Interrupt5(VOID);
VOID BF_Sgi_Interrupt6(VOID);
VOID BF_Sgi_Interrupt7(VOID);
VOID BF_Sgi_Interrupt8(VOID);
VOID BF_Sgi_Interrupt9(VOID);
VOID BF_Sgi_Interrupt10(VOID);
VOID BF_Sgi_Interrupt11(VOID);
VOID BF_Sgi_Interrupt12(VOID);
VOID BF_Sgi_Interrupt13(VOID);
VOID BF_Sgi_Interrupt14(VOID);
VOID BF_Sgi_Interrupt15(VOID);
VOID BF_Ppi_Interrupt0(VOID);
VOID BF_Ppi_Interrupt1(VOID);
VOID BF_Ppi_Interrupt2(VOID);
VOID BF_Ppi_Interrupt3(VOID);
VOID BF_Ppi_Interrupt4(VOID);
VOID BF_Ppi_Interrupt5(VOID);
VOID BF_Ppi_Interrupt6(VOID);
VOID BF_Ppi_Interrupt7(VOID);
VOID BF_Ppi_Interrupt8(VOID);
VOID BF_Ppi_Interrupt9(VOID);
VOID BF_Ppi_Interrupt10(VOID);
VOID BF_Ppi_Interrupt11(VOID);
VOID BF_Ppi_Interrupt12(VOID);
VOID BF_Ppi_Interrupt13(VOID);
VOID BF_Ppi_Interrupt14(VOID);
VOID BF_Ppi_Interrupt15(VOID);
VOID BF_CortexA7_Cpu0_Communication_Ch_Receive_Int(VOID);
VOID BF_CortexA7_Cpu0_Communication_Ch_Trnsmit_Int(VOID);
VOID BF_CortexA7_Cpu1_Communication_Ch_Receive_Int(VOID);
VOID BF_CortexA7_Cpu1_Communication_Ch_Trnsmit_Int(VOID);
VOID BF_CortexA7_Cpu2_Communication_Ch_Receive_Int(VOID);
VOID BF_CortexA7_Cpu2_Communication_Ch_Trnsmit_Int(VOID);
VOID BF_CortexA7_Cpu3_Communication_Ch_Receive_Int(VOID);
VOID BF_CortexA7_Cpu3_Communication_Ch_Trnsmit_Int(VOID);
VOID BF_CortexA7_Cpu0_Crosstrigger_Int(VOID);
VOID BF_CortexA7_Cpu1_Crosstrigger_Int(VOID);
VOID BF_CortexA7_Cpu2_Crosstrigger_Int(VOID);
VOID BF_CortexA7_Cpu3_Crosstrigger_Int(VOID);
VOID BF_CortexA7_Cpu0_System_Metrics_Int(VOID);
VOID BF_CortexA7_Cpu1_System_Metrics_Int(VOID);
VOID BF_CortexA7_Cpu2_System_Metrics_Int(VOID);
VOID BF_CortexA7_Cpu3_System_Metrics_Int(VOID);
VOID BF_Axi_Transaction_Error_Int(VOID);
VOID BF_Snap_Peri_Xdmac_Ch0_Int(VOID);
VOID BF_Snap_Peri_Xdmac_Ch1_Int(VOID);
VOID BF_Snap_Peri_Xdmac_Ch2_Int(VOID);
VOID BF_Snap_Peri_Xdmac_Ch3_Int(VOID);
VOID BF_Watchdog_Timer_Int(VOID);
VOID BF_Ipcu0_Ch0_Int(VOID);
VOID BF_Ipcu0_Ch1_Int(VOID);
VOID BF_Ipcu0_Ch2_Int(VOID);
VOID BF_Ipcu0_Ch3_Int(VOID);
VOID BF_Ipcu0_Ch4_Int(VOID);
VOID BF_Ipcu0_Ch5_Int(VOID);
VOID BF_Ipcu0_Ch6_Int(VOID);
VOID BF_Ipcu0_Ch7_Int(VOID);
VOID BF_Ipcu0_Ch8_Int(VOID);
VOID BF_Ipcu0_Ch9_Int(VOID);
VOID BF_Ipcu0_Ch10_Int(VOID);
VOID BF_Ipcu0_Ch11_Int(VOID);
VOID BF_Ipcu0_Ch12_Int(VOID);
VOID BF_Ipcu0_Ch13_Int(VOID);
VOID BF_Ipcu0_Ch14_Int(VOID);
VOID BF_Ipcu0_Ch15_Int(VOID);
VOID BF_Ipcu1_Ch0_Int(VOID);
VOID BF_Ipcu1_Ch1_Int(VOID);
VOID BF_Ipcu1_Ch2_Int(VOID);
VOID BF_Ipcu1_Ch3_Int(VOID);
VOID BF_Ipcu1_Ch4_Int(VOID);
VOID BF_Ipcu1_Ch5_Int(VOID);
VOID BF_Ipcu1_Ch6_Int(VOID);
VOID BF_Ipcu1_Ch7_Int(VOID);
VOID BF_Ipcu1_Ch8_Int(VOID);
VOID BF_Ipcu1_Ch9_Int(VOID);
VOID BF_Ipcu1_Ch10_Int(VOID);
VOID BF_Ipcu1_Ch11_Int(VOID);
VOID BF_Ipcu1_Ch12_Int(VOID);
VOID BF_Ipcu1_Ch13_Int(VOID);
VOID BF_Ipcu1_Ch14_Int(VOID);
VOID BF_Ipcu1_Ch15_Int(VOID);
VOID BF_External_Ch16_Int(VOID);
VOID BF_External_Ch17_Int(VOID);
VOID BF_External_Ch18_Int(VOID);
VOID BF_External_Ch19_Int(VOID);
VOID BF_External_Ch20_Int(VOID);
VOID BF_External_Ch21_Int(VOID);
VOID BF_External_Ch22_Int(VOID);
VOID BF_External_Ch23_Int(VOID);
VOID BF_External_Ch24_Int(VOID);
VOID BF_External_Ch25_Int(VOID);
VOID BF_External_Ch26_Int(VOID);
VOID BF_External_Ch27_Int(VOID);
VOID BF_External_Ch28_Int(VOID);
VOID BF_External_Ch29_Int(VOID);
VOID BF_External_Ch30_Int(VOID);
VOID BF_External_Ch31_Int(VOID);
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
VOID BF_External_Ch32_Int(VOID);
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
VOID BF_Pmc_Dmac_Adc_Int(VOID);
VOID BF_Pmc_Dmac_Receive_Int(VOID);
VOID BF_Pmc_Dmac_Uart_Transmission_Data_Int(VOID);
VOID BF_Pmc_Dmac_Qspi_Transmission_Int(VOID);
VOID BF_Pmc_Dmac_Qspi_Receive_Int(VOID);
VOID BF_Pmc_Uart_Receive_Relation_Int(VOID);
VOID BF_Pmc_Uart_Transmission_Relation_Int(VOID);
VOID BF_Pmc_Qspi_Int(VOID);
VOID BF_Calendar_Alarm_Int(VOID);
VOID BF_Pmc_32bit_Timer_Under_Flow_Int(VOID);
VOID BF_Adc_Int(VOID);
VOID BF_Pmc_I2c_Int(VOID);
VOID BF_Pmc_Watchdog_Int(VOID);
VOID BF_Temperature_Int(VOID);
VOID BF_Pmu_Int(VOID);
VOID BF_Pmc_Gpio_Int(VOID);
VOID BF_Timer_Ch0_Int(VOID);
VOID BF_Timer_Ch1_Int(VOID);
VOID BF_Timer_Ch2_Int(VOID);
VOID BF_Timer_Ch3_Int(VOID);
VOID BF_Timer_Ch4_Int(VOID);
VOID BF_Timer_Ch5_Int(VOID);
VOID BF_Timer_Ch6_Int(VOID);
VOID BF_Timer_Ch7_Int(VOID);
VOID BF_Timer_Ch8_Int(VOID);
VOID BF_Timer_Ch9_Int(VOID);
VOID BF_Timer_Ch10_Int(VOID);
VOID BF_Timer_Ch11_Int(VOID);
VOID BF_Timer_Ch12_Int(VOID);
VOID BF_Timer_Ch13_Int(VOID);
VOID BF_I2c_Ch0_Int(VOID);
VOID BF_I2c_Ch1_Int(VOID);
VOID BF_I2c_Ch2_Int(VOID);
VOID BF_Updown_Counter_Ch0_Int(VOID);
VOID BF_Updown_Counter_Ch1_Int(VOID);
VOID BF_Updown_Counter_Ch2_Int(VOID);
VOID BF_Updown_Counter_Ch3_Int(VOID);
VOID BF_Updown_Counter_Ch4_Int(VOID);
VOID BF_Updown_Counter_Ch5_Int(VOID);
VOID BF_Pwm_Ch0_Ch3_Int(VOID);
VOID BF_Pwm_Ch4_Ch7_Int(VOID);
VOID BF_Pwm_Ch8_Ch11_Int(VOID);
VOID BF_Pwm_Ch12_Ch15_Int(VOID);
VOID BF_Pwm_Ch0_Ch3_Stp_Int(VOID);
VOID BF_Pwm_Ch4_Ch7_Stp_Int(VOID);
VOID BF_Pwm_Ch8_Ch11_Stp_Int(VOID);
VOID BF_Pwm_Ch12_Ch15_Stp_Int(VOID);
VOID BF_Audio_If_Ch0_Int(VOID);
VOID BF_Audio_If_Ch1_Int(VOID);
VOID BF_Audio_If_Ch2_Int(VOID);
VOID BF_Audio_If_Ch3_Int(VOID);
VOID BF_Audio_If_Ch4_Int(VOID);
VOID BF_Audio_If_Ch5_Int(VOID);
VOID BF_Peri_Hdmac0_Ch0_Int(VOID);
VOID BF_Peri_Hdmac0_Ch1_Int(VOID);
VOID BF_Peri_Hdmac0_Ch2_Int(VOID);
VOID BF_Peri_Hdmac0_Ch3_Int(VOID);
VOID BF_Peri_Hdmac0_Ch4_Int(VOID);
VOID BF_Peri_Hdmac0_Ch5_Int(VOID);
VOID BF_Peri_Hdmac0_Ch6_Int(VOID);
VOID BF_Peri_Hdmac0_Ch7_Int(VOID);
VOID BF_Peri_Hdmac1_Ch0_Int(VOID);
VOID BF_Peri_Hdmac1_Ch1_Int(VOID);
VOID BF_Peri_Hdmac1_Ch2_Int(VOID);
VOID BF_Peri_Hdmac1_Ch3_Int(VOID);
VOID BF_Peri_Hdmac1_Ch4_Int(VOID);
VOID BF_Peri_Hdmac1_Ch5_Int(VOID);
VOID BF_Peri_Hdmac1_Ch6_Int(VOID);
VOID BF_Peri_Hdmac1_Ch7_Int(VOID);
VOID BF_Uart_Ch0_Reception_Int(VOID);
VOID BF_Uart_Ch1_Reception_Int(VOID);
VOID BF_Uart_Ch2_Reception_Int(VOID);
VOID BF_Uart_Ch3_Reception_Int(VOID);
VOID BF_Uart_Ch4_Reception_Int(VOID);
VOID BF_Uart_Ch5_Reception_Int(VOID);
VOID BF_Uart_Ch6_Reception_Int(VOID);
VOID BF_Uart_Ch7_Reception_Int(VOID);
VOID BF_Uart_Ch0_Transmission_Int(VOID);
VOID BF_Uart_Ch1_Transmission_Int(VOID);
VOID BF_Uart_Ch2_Transmission_Int(VOID);
VOID BF_Uart_Ch3_Transmission_Int(VOID);
VOID BF_Uart_Ch4_Transmission_Int(VOID);
VOID BF_Uart_Ch5_Transmission_Int(VOID);
VOID BF_Uart_Ch6_Transmission_Int(VOID);
VOID BF_Uart_Ch7_Transmission_Int(VOID);
VOID BF_Spi_Ch0_Int(VOID);
VOID BF_Spi_Ch1_Int(VOID);
VOID BF_Spi_Ch2_Int(VOID);
VOID BF_Pdm_Ch0_Dma0_Int(VOID);
VOID BF_Pdm_Ch0_Dma1_Int(VOID);
VOID BF_Pdm_Ch0_Fifo_Overflow_Int(VOID);
VOID BF_Pdm_Ch1_Dma0_Int(VOID);
VOID BF_Pdm_Ch1_Dma1_Int(VOID);
VOID BF_Pdm_Ch1_Fifo_Overflow_Int(VOID);
VOID BF_Slimbus_Ch0_Int(VOID);
VOID BF_Slimbus_Ch1_Int(VOID);
VOID BF_Slimbus_Ch2_Int(VOID);
VOID BF_Slimbus_Ch3_Int(VOID);
VOID BF_Sdramc_Ch0_Int(VOID);
VOID BF_Sdramc_Ch1_Int(VOID);
VOID BF_Mxic_Unit0_Int(VOID);
VOID BF_Mxic_Unit1_Int(VOID);
VOID BF_Mxic_Unit2_Int(VOID);
VOID BF_Mxic_Unit3_Int(VOID);
VOID BF_Mxic_Unit4_Int(VOID);
VOID BF_Mxic_Unit5_Int(VOID);
VOID BF_Mxic_Unit6_Int(VOID);
VOID BF_Sen_Int(VOID);
VOID BF_Sen_Vd_Int(VOID);
VOID BF_Sen_Hd_Int(VOID);
VOID BF_Sensor_If_Macro0_Int(VOID);
VOID BF_Sensor_If_Macro1_Int(VOID);
VOID BF_Sensor_If_Macro2_Int(VOID);
VOID BF_Sensor_If_Macro3_Int(VOID);
VOID BF_Sro_Unit1_Int(VOID);
VOID BF_Sro_Unit1_Vd_Int(VOID);
VOID BF_Sro_Unit1_Hd_Int(VOID);
VOID BF_Sro_Unit2_Int(VOID);
VOID BF_Sro_Unit2_Vd_Int(VOID);
VOID BF_Sro_Unit2_Hd_Int(VOID);
VOID BF_B2b_Unit1_Int(VOID);
VOID BF_B2b_Unit1_Vd_Int(VOID);
VOID BF_B2b_Unit1_Hd_Int(VOID);
VOID BF_B2b_Unit2_Int(VOID);
VOID BF_B2b_Unit2_Vd_Int(VOID);
VOID BF_B2b_Unit2_Hd_Int(VOID);
VOID BF_Stat_Int(VOID);
VOID BF_Pas_Int(VOID);
VOID BF_Pas_Vd_Int(VOID);
VOID BF_Pas_Hd_Int(VOID);
VOID BF_B2r_Unit1_Int(VOID);
VOID BF_B2r_Unit2_Int(VOID);
VOID BF_Ltm_Unit1_Int(VOID);
VOID BF_Ltm_Unit2_Int(VOID);
VOID BF_R2y_Unit1_Int(VOID);
VOID BF_R2y_Unit2_Int(VOID);
VOID BF_Cnr_Unit1_Int(VOID);
VOID BF_Cnr_Unit2_Int(VOID);
VOID BF_Iip_Int(VOID);
VOID BF_Shdr_Int(VOID);
VOID BF_Shdr_Line_Int(VOID);
VOID BF_Me_Int(VOID);
VOID BF_Graphics_Int(VOID);
VOID BF_Jpeg_Int(VOID);
VOID BF_Raw_Int(VOID);
VOID BF_Ipu_Int(VOID);
VOID BF_Ela_Unit_Int(VOID);
VOID BF_Xch_Int(VOID);
VOID BF_Fpt_Unit0_Int(VOID);
VOID BF_Fpt_Unit1_Int(VOID);
VOID BF_Disp_Lcd_Int(VOID);
VOID BF_Disp_Hdmi_Int(VOID);
VOID BF_Disp_Mipi_Dsi_Int(VOID);
VOID BF_Mipi_Dsi_Tx_Int(VOID);
VOID BF_Disp_Dwch_Int(VOID);
VOID BF_Rdma_Int(VOID);
VOID BF_Hdmi_Master_Int(VOID);
VOID BF_Hdmi_Cec_Int(VOID);
VOID BF_BMH_Unit0_Int(VOID);
VOID BF_BMH_Unit1_Int(VOID);
VOID BF_Ribery_Rbr_Int(VOID);
VOID BF_Ribery_Rwdt_Int(VOID);
VOID BF_Interprocessor0_Int(VOID);
VOID BF_Interprocessor1_Int(VOID);
VOID BF_Interprocessor2_Int(VOID);
VOID BF_Interprocessor3_Int(VOID);
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
VOID BF_Wave_Int(VOID);
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
VOID BF_Dsp_Peripheral_Unit_Ipcu0_Int(VOID);
VOID BF_Dsp_Peripheral_Unit_Ipcu1_Int(VOID);
VOID BF_Dsp_Peripheral_Unit_Ipcu2_Int(VOID);
VOID BF_Dsp_Peripheral_Unit_Ipcu3_Int(VOID);
VOID BF_Dsp_Debug_Int(VOID);
VOID BF_Nand_Flash_Int(VOID);
VOID BF_Pcie_Ch0_Msi_Int(VOID);
VOID BF_Pcie_Ch0_System_Error_Int(VOID);
VOID BF_Pcie_Ch0_Intx_Int(VOID);
VOID BF_Pcie_Ch0_Core_Int(VOID);
VOID BF_Pcie_Ch0_DMA_Int(VOID);
VOID BF_Pcie_Ch1_Msi_Int(VOID);
VOID BF_Pcie_Ch1_System_Error_Int(VOID);
VOID BF_Pcie_Ch1_Intx_Int(VOID);
VOID BF_Pcie_Ch1_Core_Int(VOID);
VOID BF_Pcie_Ch1_DMA_Int(VOID);
VOID BF_Exs_Xdmac_Ch0_Int(VOID);
VOID BF_Exs_Xdmac_Ch1_Int(VOID);
VOID BF_Exs_Xdmac_Ch2_Int(VOID);
VOID BF_Exs_Xdmac_Ch3_Int(VOID);
VOID BF_Exs_Hdmac_Ch0_Int(VOID);
VOID BF_Exs_Hdmac_Ch1_Int(VOID);
VOID BF_Exs_Hdmac_Ch2_Int(VOID);
VOID BF_Exs_Hdmac_Ch3_Int(VOID);
VOID BF_Usb20_Hdc_Int(VOID);
VOID BF_Usb20_Host_Ehci_Int(VOID);
VOID BF_Usb20_Host_Ohci_Int(VOID);
VOID BF_Usb30_Int(VOID);
VOID BF_Sd_Ch3_Int(VOID);
VOID BF_Sd_Ch3_Sdio_Int(VOID);
VOID BF_Sd_Ch3_Wakeup_Int(VOID);
VOID BF_Sd_Ch2_Uhs1_Int(VOID);
VOID BF_Sd_Ch2_Sdio_Int(VOID);
VOID BF_Sd_Ch2_Uhs1_Wakeup_Int(VOID);
VOID BF_Sd_Ch2_Uhs2_Int(VOID);
VOID BF_Sd_Ch2_Uhs2_Wakeup_Int(VOID);
VOID BF_Sd_Ch1_Int(VOID);
VOID BF_Sd_Ch1_Sdio_Int(VOID);
VOID BF_Sd_Ch1_Wakeup_Int(VOID);
VOID BF_Emmc_Int(VOID);
VOID BF_Emmc_Wakeup_Int(VOID);
VOID BF_Relc_Int(VOID);
VOID BF_Netsec_A_Int(VOID);
VOID BF_Netsec_B_Int(VOID);
VOID BF_Sd_Pcie_Detect_Int(VOID);

#ifdef __cplusplus
}
#endif

#endif	// _INTERRUPT_H_
