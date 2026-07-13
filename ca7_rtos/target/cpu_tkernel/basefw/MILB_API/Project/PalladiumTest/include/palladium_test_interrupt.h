/**
 * @file		dd_interrupt.h
 * @brief		Interrupt Control
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _DD_INTERRUPT_H_
#define _DD_INTERRUPT_H_

/**
@addtogroup dd_interrupt
@{
*/

#include "ddim_typedef.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

extern VOID Pt_Sgi_Interrupt0(void);
extern VOID Pt_Sgi_Interrupt1(void);
extern VOID Pt_Sgi_Interrupt2(void);
extern VOID Pt_Sgi_Interrupt3(void);
extern VOID Pt_Sgi_Interrupt4(void);
extern VOID Pt_Sgi_Interrupt5(void);
extern VOID Pt_Sgi_Interrupt6(void);
extern VOID Pt_Sgi_Interrupt7(void);
extern VOID Pt_Sgi_Interrupt8(void);
extern VOID Pt_Sgi_Interrupt9(void);
extern VOID Pt_Sgi_Interrupt10(void);
extern VOID Pt_Sgi_Interrupt11(void);
extern VOID Pt_Sgi_Interrupt12(void);
extern VOID Pt_Sgi_Interrupt13(void);
extern VOID Pt_Sgi_Interrupt14(void);
extern VOID Pt_Sgi_Interrupt15(void);
extern VOID Pt_Ppi_Interrupt0(void);
extern VOID Pt_Ppi_Interrupt1(void);
extern VOID Pt_Ppi_Interrupt2(void);
extern VOID Pt_Ppi_Interrupt3(void);
extern VOID Pt_Ppi_Interrupt4(void);
extern VOID Pt_Ppi_Interrupt5(void);
extern VOID Pt_Ppi_Interrupt6(void);
extern VOID Pt_Ppi_Interrupt7(void);
extern VOID Pt_Ppi_Interrupt8(void);
extern VOID Pt_Ppi_Interrupt9(void);
extern VOID Pt_Ppi_Interrupt10(void);
extern VOID Pt_Ppi_Interrupt11(void);
extern VOID Pt_Ppi_Interrupt12(void);
extern VOID Pt_Ppi_Interrupt13(void);
extern VOID Pt_Ppi_Interrupt14(void);
extern VOID Pt_Ppi_Interrupt15(void);
extern VOID Pt_CortexA7_Cpu0_Communication_Ch_Receive_Int(void);
extern VOID Pt_CortexA7_Cpu0_Communication_Ch_Trnsmit_Int(void);
extern VOID Pt_CortexA7_Cpu1_Communication_Ch_Receive_Int(void);
extern VOID Pt_CortexA7_Cpu1_Communication_Ch_Trnsmit_Int(void);
extern VOID Pt_CortexA7_Cpu2_Communication_Ch_Receive_Int(void);
extern VOID Pt_CortexA7_Cpu2_Communication_Ch_Trnsmit_Int(void);
extern VOID Pt_CortexA7_Cpu3_Communication_Ch_Receive_Int(void);
extern VOID Pt_CortexA7_Cpu3_Communication_Ch_Trnsmit_Int(void);
extern VOID Pt_CortexA7_Cpu0_Crosstrigger_Int(void);
extern VOID Pt_CortexA7_Cpu1_Crosstrigger_Int(void);
extern VOID Pt_CortexA7_Cpu2_Crosstrigger_Int(void);
extern VOID Pt_CortexA7_Cpu3_Crosstrigger_Int(void);
extern VOID Pt_CortexA7_Cpu0_System_Metrics_Int(void);
extern VOID Pt_CortexA7_Cpu1_System_Metrics_Int(void);
extern VOID Pt_CortexA7_Cpu2_System_Metrics_Int(void);
extern VOID Pt_CortexA7_Cpu3_System_Metrics_Int(void);
extern VOID Pt_Axi_Transaction_Error_Int(void);
extern VOID Pt_Snap_Peri_Xdmac_Ch0_Int(void);
extern VOID Pt_Snap_Peri_Xdmac_Ch1_Int(void);
extern VOID Pt_Snap_Peri_Xdmac_Ch2_Int(void);
extern VOID Pt_Snap_Peri_Xdmac_Ch3_Int(void);
extern VOID Pt_Watchdog_Timer_Int(void);
extern VOID Pt_Ipcu0_Ch0_Int(void);
extern VOID Pt_Ipcu0_Ch1_Int(void);
extern VOID Pt_Ipcu0_Ch2_Int(void);
extern VOID Pt_Ipcu0_Ch3_Int(void);
extern VOID Pt_Ipcu0_Ch4_Int(void);
extern VOID Pt_Ipcu0_Ch5_Int(void);
extern VOID Pt_Ipcu0_Ch6_Int(void);
extern VOID Pt_Ipcu0_Ch7_Int(void);
extern VOID Pt_Ipcu0_Ch8_Int(void);
extern VOID Pt_Ipcu0_Ch9_Int(void);
extern VOID Pt_Ipcu0_Ch10_Int(void);
extern VOID Pt_Ipcu0_Ch11_Int(void);
extern VOID Pt_Ipcu0_Ch12_Int(void);
extern VOID Pt_Ipcu0_Ch13_Int(void);
extern VOID Pt_Ipcu0_Ch14_Int(void);
extern VOID Pt_Ipcu0_Ch15_Int(void);
extern VOID Pt_Ipcu1_Ch0_Int(void);
extern VOID Pt_Ipcu1_Ch1_Int(void);
extern VOID Pt_Ipcu1_Ch2_Int(void);
extern VOID Pt_Ipcu1_Ch3_Int(void);
extern VOID Pt_Ipcu1_Ch4_Int(void);
extern VOID Pt_Ipcu1_Ch5_Int(void);
extern VOID Pt_Ipcu1_Ch6_Int(void);
extern VOID Pt_Ipcu1_Ch7_Int(void);
extern VOID Pt_Ipcu1_Ch8_Int(void);
extern VOID Pt_Ipcu1_Ch9_Int(void);
extern VOID Pt_Ipcu1_Ch10_Int(void);
extern VOID Pt_Ipcu1_Ch11_Int(void);
extern VOID Pt_Ipcu1_Ch12_Int(void);
extern VOID Pt_Ipcu1_Ch13_Int(void);
extern VOID Pt_Ipcu1_Ch14_Int(void);
extern VOID Pt_Ipcu1_Ch15_Int(void);
extern VOID Pt_External_Ch16_Int(void);
extern VOID Pt_External_Ch17_Int(void);
extern VOID Pt_External_Ch18_Int(void);
extern VOID Pt_External_Ch19_Int(void);
extern VOID Pt_External_Ch20_Int(void);
extern VOID Pt_External_Ch21_Int(void);
extern VOID Pt_External_Ch22_Int(void);
extern VOID Pt_External_Ch23_Int(void);
extern VOID Pt_External_Ch24_Int(void);
extern VOID Pt_External_Ch25_Int(void);
extern VOID Pt_External_Ch26_Int(void);
extern VOID Pt_External_Ch27_Int(void);
extern VOID Pt_External_Ch28_Int(void);
extern VOID Pt_External_Ch29_Int(void);
extern VOID Pt_External_Ch30_Int(void);
extern VOID Pt_External_Ch31_Int(void);
extern VOID Pt_Pmc_Dmac_Adc_Int(void);
extern VOID Pt_Pmc_Dmac_Receive_Int(void);
extern VOID Pt_Pmc_Uart_Transmission_Data_Int(void);
extern VOID Pt_Pmc_Dmac_Qspi_Transmission_Int(void);
extern VOID Pt_Pmc_Dmac_Qspi_Receive_Int(void);
extern VOID Pt_Pmc_Uart_Receive_Relation_Int(void);
extern VOID Pt_Pmc_Uart_Transmission_Relation_Int(void);
extern VOID Pt_Pmc_Qspi_Int(void);
extern VOID Pt_Calendar_Alarm_Int(void);
extern VOID Pt_Pmc_32bit_Timer_Under_Flow_Int(void);
extern VOID Pt_Adc_Int(void);
extern VOID Pt_Pmc_I2c_Int(void);
extern VOID Pt_Pmc_Watchdog_Int(void);
extern VOID Pt_Temperature_Int(void);
extern VOID Pt_Pmu_Int(void);
extern VOID Pt_Pmc_Gpio_Int(void);
extern VOID Pt_Timer_Ch0_Int(void);
extern VOID Pt_Timer_Ch1_Int(void);
extern VOID Pt_Timer_Ch2_Int(void);
extern VOID Pt_Timer_Ch3_Int(void);
extern VOID Pt_Timer_Ch4_Int(void);
extern VOID Pt_Timer_Ch5_Int(void);
extern VOID Pt_Timer_Ch6_Int(void);
extern VOID Pt_Timer_Ch7_Int(void);
extern VOID Pt_Timer_Ch8_Int(void);
extern VOID Pt_Timer_Ch9_Int(void);
extern VOID Pt_Timer_Ch10_Int(void);
extern VOID Pt_Timer_Ch11_Int(void);
extern VOID Pt_Timer_Ch12_Int(void);
extern VOID Pt_Timer_Ch13_Int(void);
extern VOID Pt_I2c_Ch0_Int(void);
extern VOID Pt_I2c_Ch1_Int(void);
extern VOID Pt_I2c_Ch2_Int(void);
extern VOID Pt_Updown_Counter_Ch0_Int(void);
extern VOID Pt_Updown_Counter_Ch1_Int(void);
extern VOID Pt_Updown_Counter_Ch2_Int(void);
extern VOID Pt_Updown_Counter_Ch3_Int(void);
extern VOID Pt_Updown_Counter_Ch4_Int(void);
extern VOID Pt_Updown_Counter_Ch5_Int(void);
extern VOID Pt_Pwm_Ch0_Ch3_Int(void);
extern VOID Pt_Pwm_Ch4_Ch7_Int(void);
extern VOID Pt_Pwm_Ch8_Ch11_Int(void);
extern VOID Pt_Pwm_Ch12_Ch15_Int(void);
extern VOID Pt_Pwm_Ch0_Ch3_Stp_Int(void);
extern VOID Pt_Pwm_Ch4_Ch7_Stp_Int(void);
extern VOID Pt_Pwm_Ch8_Ch11_Stp_Int(void);
extern VOID Pt_Pwm_Ch12_Ch15_Stp_Int(void);
extern VOID Pt_Audio_If_Ch0_Int(void);
extern VOID Pt_Audio_If_Ch1_Int(void);
extern VOID Pt_Audio_If_Ch2_Int(void);
extern VOID Pt_Audio_If_Ch3_Int(void);
extern VOID Pt_Audio_If_Ch4_Int(void);
extern VOID Pt_Audio_If_Ch5_Int(void);
extern VOID Pt_Peri_Hdmac0_Ch0_Int(void);
extern VOID Pt_Peri_Hdmac0_Ch1_Int(void);
extern VOID Pt_Peri_Hdmac0_Ch2_Int(void);
extern VOID Pt_Peri_Hdmac0_Ch3_Int(void);
extern VOID Pt_Peri_Hdmac0_Ch4_Int(void);
extern VOID Pt_Peri_Hdmac0_Ch5_Int(void);
extern VOID Pt_Peri_Hdmac0_Ch6_Int(void);
extern VOID Pt_Peri_Hdmac0_Ch7_Int(void);
extern VOID Pt_Peri_Hdmac1_Ch0_Int(void);
extern VOID Pt_Peri_Hdmac1_Ch1_Int(void);
extern VOID Pt_Peri_Hdmac1_Ch2_Int(void);
extern VOID Pt_Peri_Hdmac1_Ch3_Int(void);
extern VOID Pt_Peri_Hdmac1_Ch4_Int(void);
extern VOID Pt_Peri_Hdmac1_Ch5_Int(void);
extern VOID Pt_Peri_Hdmac1_Ch6_Int(void);
extern VOID Pt_Peri_Hdmac1_Ch7_Int(void);
extern VOID Pt_Uart_Ch0_Reception_Int(void);
extern VOID Pt_Uart_Ch1_Reception_Int(void);
extern VOID Pt_Uart_Ch2_Reception_Int(void);
extern VOID Pt_Uart_Ch3_Reception_Int(void);
extern VOID Pt_Uart_Ch4_Reception_Int(void);
extern VOID Pt_Uart_Ch5_Reception_Int(void);
extern VOID Pt_Uart_Ch6_Reception_Int(void);
extern VOID Pt_Uart_Ch7_Reception_Int(void);
extern VOID Pt_Uart_Ch0_Transmission_Int(void);
extern VOID Pt_Uart_Ch1_Transmission_Int(void);
extern VOID Pt_Uart_Ch2_Transmission_Int(void);
extern VOID Pt_Uart_Ch3_Transmission_Int(void);
extern VOID Pt_Uart_Ch4_Transmission_Int(void);
extern VOID Pt_Uart_Ch5_Transmission_Int(void);
extern VOID Pt_Uart_Ch6_Transmission_Int(void);
extern VOID Pt_Uart_Ch7_Transmission_Int(void);
extern VOID Pt_Spi_Ch0_Int(void);
extern VOID Pt_Spi_Ch1_Int(void);
extern VOID Pt_Spi_Ch2_Int(void);
extern VOID Pt_Pdm_Ch0_Dma0_Int(void);
extern VOID Pt_Pdm_Ch0_Dma1_Int(void);
extern VOID Pt_Pdm_Ch0_Fifo_Overflow_Int(void);
extern VOID Pt_Pdm_Ch1_Dma0_Int(void);
extern VOID Pt_Pdm_Ch1_Dma1_Int(void);
extern VOID Pt_Pdm_Ch1_Fifo_Overflow_Int(void);
extern VOID Pt_Slimbus_Ch0_Int(void);
extern VOID Pt_Slimbus_Ch1_Int(void);
extern VOID Pt_Slimbus_Ch2_Int(void);
extern VOID Pt_Slimbus_Ch3_Int(void);
extern VOID Pt_Sdramc_Ch0_Int(void);
extern VOID Pt_Sdramc_Ch1_Int(void);
extern VOID Pt_Mxic_Unit0_Int(void);
extern VOID Pt_Mxic_Unit1_Int(void);
extern VOID Pt_Mxic_Unit2_Int(void);
extern VOID Pt_Mxic_Unit3_Int(void);
extern VOID Pt_Mxic_Unit4_Int(void);
extern VOID Pt_Mxic_Unit5_Int(void);
extern VOID Pt_Mxic_Unit6_Int(void);
extern VOID Pt_Sen_Int(void);
extern VOID Pt_Sen_Vd_Int(void);
extern VOID Pt_Sen_Hd_Int(void);
extern VOID Pt_Sensor_If_Macro0_Int(void);
extern VOID Pt_Sensor_If_Macro1_Int(void);
extern VOID Pt_Sensor_If_Macro2_Int(void);
extern VOID Pt_Sensor_If_Macro3_Int(void);
extern VOID Pt_Sro_Unit1_Int(void);
extern VOID Pt_Sro_Unit1_Vd_Int(void);
extern VOID Pt_Sro_Unit1_Hd_Int(void);
extern VOID Pt_Sro_Unit2_Int(void);
extern VOID Pt_Sro_Unit2_Vd_Int(void);
extern VOID Pt_Sro_Unit2_Hd_Int(void);
extern VOID Pt_B2b_Unit1_Int(void);
extern VOID Pt_B2b_Unit1_Vd_Int(void);
extern VOID Pt_B2b_Unit1_Hd_Int(void);
extern VOID Pt_B2b_Unit2_Int(void);
extern VOID Pt_B2b_Unit2_Vd_Int(void);
extern VOID Pt_B2b_Unit2_Hd_Int(void);
extern VOID Pt_Start_Int(void);
extern VOID Pt_Pas_Unit2_Int(void);
extern VOID Pt_Pas_Unit2_Vd_Int(void);
extern VOID Pt_Pas_Unit2_Hd_Int(void);
extern VOID Pt_B2r_Unit1_Int(void);
extern VOID Pt_B2r_Unit2_Int(void);
extern VOID Pt_Ltm_Unit1_Int(void);
extern VOID Pt_Ltm_Unit2_Int(void);
extern VOID Pt_R2y_Unit1_Int(void);
extern VOID Pt_R2y_Unit2_Int(void);
extern VOID Pt_Cnr_Unit1_Int(void);
extern VOID Pt_Cnr_Unit2_Int(void);
extern VOID Pt_Iip_Int(void);
extern VOID Pt_Shdr_Int(void);
extern VOID Pt_Shdr_Line_Int(void);
extern VOID Pt_Me_Int(void);
extern VOID Pt_Graphics_Int(void);
extern VOID Pt_Jpeg_Int(void);
extern VOID Pt_Raw_Int(void);
extern VOID Pt_Ipu_Int(void);
extern VOID Pt_Ela_Unit_Int(void);
extern VOID Pt_Xch_Int(void);
extern VOID Pt_Fpt_Unit0_Int(void);
extern VOID Pt_Fpt_Unit1_Int(void);
extern VOID Pt_Disp_Lcd_Int(void);
extern VOID Pt_Disp_Hdmi_Int(void);
extern VOID Pt_Disp_Mipi_Dsi_Int(void);
extern VOID Pt_Mipi_Dsi_Tx_Int(void);
extern VOID Pt_Disp_Dwch_Int(void);
extern VOID Pt_Rdma_Int(void);
extern VOID Pt_Hdmi_Master_Int(void);
extern VOID Pt_Hdmi_Cec_Int(void);
extern VOID Pt_BMH_Unit0_Int(void);
extern VOID Pt_BMH_Unit1_Int(void);
extern VOID Pt_Ribery_Rbr_Int(void);
extern VOID Pt_Ribery_Rwdt_Int(void);
extern VOID Pt_Interprocessor0_Int(void);
extern VOID Pt_Interprocessor1_Int(void);
extern VOID Pt_Interprocessor2_Int(void);
extern VOID Pt_Interprocessor3_Int(void);
extern VOID Pt_Dsp_Peripheral_Unit_Ipcu0_Int(void);
extern VOID Pt_Dsp_Peripheral_Unit_Ipcu1_Int(void);
extern VOID Pt_Dsp_Peripheral_Unit_Ipcu2_Int(void);
extern VOID Pt_Dsp_Peripheral_Unit_Ipcu3_Int(void);
extern VOID Pt_Dsp_Debug_Int(void);
extern VOID Pt_Nand_Flash_Int(void);
extern VOID Pt_Pcie_Ch0_Msi_Int(void);
extern VOID Pt_Pcie_Ch0_System_Error_Int(void);
extern VOID Pt_Pcie_Ch0_Intx_Int(void);
extern VOID Pt_Pcie_Ch0_Core_Int(void);
extern VOID Pt_Pcie_Ch0_DMA_Int(void);
extern VOID Pt_Pcie_Ch1_Msi_Int(void);
extern VOID Pt_Pcie_Ch1_System_Error_Int(void);
extern VOID Pt_Pcie_Ch1_Intx_Int(void);
extern VOID Pt_Pcie_Ch1_Core_Int(void);
extern VOID Pt_Pcie_Ch1_DMA_Int(void);
extern VOID Pt_Exs_Xdmac_Ch0_Int(void);
extern VOID Pt_Exs_Xdmac_Ch1_Int(void);
extern VOID Pt_Exs_Xdmac_Ch2_Int(void);
extern VOID Pt_Exs_Xdmac_Ch3_Int(void);
extern VOID Pt_Exs_Hdmac_Ch0_Int(void);
extern VOID Pt_Exs_Hdmac_Ch1_Int(void);
extern VOID Pt_Exs_Hdmac_Ch2_Int(void);
extern VOID Pt_Exs_Hdmac_Ch3_Int(void);
extern VOID Pt_Usb20_Hdc_Int(void);
extern VOID Pt_Usb20_Host_Ehci_Int(void);
extern VOID Pt_Usb20_Host_Ohci_Int(void);
extern VOID Pt_Usb30_Int(void);
extern VOID Pt_Output_Signal1_Int(void);
extern VOID Pt_Output_Signal2_Int(void);
extern VOID Pt_Pme_Sd_Card1_Int(void);
extern VOID Pt_Pme_F_Sdh30_Uhs1_Int(void);
extern VOID Pt_Output_Signal3_Int(void);
extern VOID Pt_F_Sdh30_Uhs1_Int(void);
extern VOID Pt_F_Sdh40_Uhs2_Int(void);
extern VOID Pt_Pme_F_Sdh40_Uhs2_Int(void);
extern VOID Pt_Output_Signal4_Int(void);
extern VOID Pt_Output_Signal5_Int(void);
extern VOID Pt_Pme_Sd_Card2_Int(void);
extern VOID Pt_Output_Signal6_Int(void);
extern VOID Pt_Wakeup_Signal_Int(void);
extern VOID Pt_Relc_Int(void);
extern VOID Pt_Netsec_A_Int(void);
extern VOID Pt_Netsec_B_Int(void);
extern VOID Pt_Output_Signal7_Int(void);


#ifdef __cplusplus
}
#endif

/*@}*/

#endif
