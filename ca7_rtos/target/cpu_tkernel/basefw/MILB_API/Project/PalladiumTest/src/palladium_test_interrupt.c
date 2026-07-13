/**
 * @file		palladium_test_interrupt.c
 * @brief		Interrupt Control
 * @note		None
 * @attention	None
 *
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#include "palladium_test_interrupt.h"
#include "palladium_test.h"
#include "driver_common.h"
#include "dd_tmr32.h"

#include "im_b2r.h"
#include "im_bmh.h"
#include "im_cnr.h"
#include "im_ela.h"
#include "im_fpt.h"
#include "im_iip.h"
#include "im_ltm.h"
#include "im_mxic.h"
#include "im_r2y.h"
#include "im_raw.h"
#include "im_sdramc.h"
#include "im_shdr.h"
#include "im_xch.h"
#include "im_jpeg.h"
#include "im_me.h"

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
// SGI Interrupt0
VOID Pt_Sgi_Interrupt0(void)
{
	Ddim_Print(("SGI_Interrupt0\n"));
}

// SGI Interrupt1
VOID Pt_Sgi_Interrupt1(void)
{
	Ddim_Print(("SGI_Interrupt1\n"));
}

// SGI Interrupt2
VOID Pt_Sgi_Interrupt2(void)
{
	Ddim_Print(("SGI_Interrupt2\n"));
}

// SGI Interrupt3
VOID Pt_Sgi_Interrupt3(void)
{
	Ddim_Print(("SGI_Interrupt3\n"));
}

// SGI Interrupt4
VOID Pt_Sgi_Interrupt4(void)
{
	Ddim_Print(("SGI_Interrupt4\n"));
}

// SGI Interrupt5
VOID Pt_Sgi_Interrupt5(void)
{
	Ddim_Print(("SGI_Interrupt5\n"));
}

// SGI Interrupt6
VOID Pt_Sgi_Interrupt6(void)
{
	Ddim_Print(("SGI_Interrupt6\n"));
}

// SGI Interrupt7
VOID Pt_Sgi_Interrupt7(void)
{
	Ddim_Print(("SGI_Interrupt7\n"));
}

// SGI Interrupt8
VOID Pt_Sgi_Interrupt8(void)
{
	Ddim_Print(("SGI_Interrupt8\n"));
}

// SGI Interrupt9
VOID Pt_Sgi_Interrupt9(void)
{
	Ddim_Print(("SGI_Interrupt9\n"));
}

// SGI Interrupt10
VOID Pt_Sgi_Interrupt10(void)
{
	Ddim_Print(("SGI_Interrupt10\n"));
}

// SGI Interrupt11
VOID Pt_Sgi_Interrupt11(void)
{
	Ddim_Print(("SGI_Interrupt11\n"));
}

// SGI Interrupt12
VOID Pt_Sgi_Interrupt12(void)
{
	Ddim_Print(("SGI_Interrupt12\n"));
}

// SGI Interrupt13
VOID Pt_Sgi_Interrupt13(void)
{
	Ddim_Print(("SGI_Interrupt13\n"));
}

// SGI Interrupt14
VOID Pt_Sgi_Interrupt14(void)
{
	Ddim_Print(("SGI_Interrupt14\n"));
}

// SGI Interrupt15
VOID Pt_Sgi_Interrupt15(void)
{
	Ddim_Print(("SGI_Interrupt15\n"));
}

// PPI Interrupt0
VOID Pt_Ppi_Interrupt0(void)
{
	Ddim_Print(("PPI_Interrupt0\n"));
}

// PPI Interrupt1
VOID Pt_Ppi_Interrupt1(void)
{
	Ddim_Print(("PPI_Interrupt1\n"));
}

// PPI Interrupt2
VOID Pt_Ppi_Interrupt2(void)
{
	Ddim_Print(("PPI_Interrupt2\n"));
}

// PPI Interrupt3
VOID Pt_Ppi_Interrupt3(void)
{
	Ddim_Print(("PPI_Interrupt3\n"));
}

// PPI Interrupt4
VOID Pt_Ppi_Interrupt4(void)
{
	Ddim_Print(("PPI_Interrupt4\n"));
}

// PPI Interrupt5
VOID Pt_Ppi_Interrupt5(void)
{
	Ddim_Print(("PPI_Interrupt5\n"));
}

// PPI Interrupt6
VOID Pt_Ppi_Interrupt6(void)
{
	Ddim_Print(("PPI_Interrupt6\n"));
}

// PPI Interrupt7
VOID Pt_Ppi_Interrupt7(void)
{
	Ddim_Print(("PPI_Interrupt7\n"));
}

// PPI Interrupt8
VOID Pt_Ppi_Interrupt8(void)
{
	Ddim_Print(("PPI_Interrupt8\n"));
}

// PPI Interrupt9
VOID Pt_Ppi_Interrupt9(void)
{
	Ddim_Print(("PPI_Interrupt9\n"));
}

// PPI Interrupt10
VOID Pt_Ppi_Interrupt10(void)
{
	Ddim_Print(("PPI_Interrupt10\n"));
}

// PPI Interrupt11
VOID Pt_Ppi_Interrupt11(void)
{
	Ddim_Print(("PPI_Interrupt11\n"));
}

// PPI Interrupt12
VOID Pt_Ppi_Interrupt12(void)
{
	Ddim_Print(("PPI_Interrupt12\n"));
}

// PPI Interrupt13
VOID Pt_Ppi_Interrupt13(void)
{
	Ddim_Print(("PPI_Interrupt13\n"));
}

// PPI Interrupt14
VOID Pt_Ppi_Interrupt14(void)
{
	Ddim_Print(("PPI_Interrupt14\n"));
}

// PPI Interrupt15
VOID Pt_Ppi_Interrupt15(void)
{
	Ddim_Print(("PPI_Interrupt15\n"));
}

// Cortex-A7 CPU0 Communication Receive
VOID Pt_CortexA7_Cpu0_Communication_Ch_Receive_Int(void)
{
	Ddim_Print(("CortexA7_Cpu0_Communication_Ch_Receive_Int\n"));
}

// Cortex-A7 CPU0 Communication Trnsmit
VOID Pt_CortexA7_Cpu0_Communication_Ch_Trnsmit_Int(void)
{
	Ddim_Print(("CortexA7_Cpu0_Communication_Ch_Trnsmit_Int\n"));
}

// Cortex-A7 CPU1 Communication Receive
VOID Pt_CortexA7_Cpu1_Communication_Ch_Receive_Int(void)
{
	Ddim_Print(("CortexA7_Cpu1_Communication_Ch_Receive_Int\n"));
}

// Cortex-A7 CPU1 Communication Trnsmit
VOID Pt_CortexA7_Cpu1_Communication_Ch_Trnsmit_Int(void)
{
	Ddim_Print(("CortexA7_Cpu1_Communication_Ch_Trnsmit_Int\n"));
}

// Cortex-A7 CPU2 Communication Receive
VOID Pt_CortexA7_Cpu2_Communication_Ch_Receive_Int(void)
{
	Ddim_Print(("CortexA7_Cpu2_Communication_Ch_Receive_Int\n"));
}

// Cortex-A7 CPU2 Communication Trnsmit
VOID Pt_CortexA7_Cpu2_Communication_Ch_Trnsmit_Int(void)
{
	Ddim_Print(("CortexA7_Cpu2_Communication_Ch_Trnsmit_Int\n"));
}

// Cortex-A7 CPU3 Communication Receive
VOID Pt_CortexA7_Cpu3_Communication_Ch_Receive_Int(void)
{
	Ddim_Print(("CortexA7_Cpu3_Communication_Ch_Receive_Int\n"));
}

// Cortex-A7 CPU3 Communication Trnsmit
VOID Pt_CortexA7_Cpu3_Communication_Ch_Trnsmit_Int(void)
{
	Ddim_Print(("CortexA7_Cpu3_Communication_Ch_Trnsmit_Int\n"));
}

// Cortex-A7 CPU0 Crosstrigger
VOID Pt_CortexA7_Cpu0_Crosstrigger_Int(void)
{
	Ddim_Print(("CortexA7_Cpu0_Crosstrigger_Int\n"));
}

// Cortex-A7 CPU1 Crosstrigger
VOID Pt_CortexA7_Cpu1_Crosstrigger_Int(void)
{
	Ddim_Print(("CortexA7_Cpu1_Crosstrigger_Int\n"));
}

// Cortex-A7 CPU2 Crosstrigger
VOID Pt_CortexA7_Cpu2_Crosstrigger_Int(void)
{
	Ddim_Print(("CortexA7_Cpu2_Crosstrigger_Int\n"));
}

// Cortex-A7 CPU3 Crosstrigger
VOID Pt_CortexA7_Cpu3_Crosstrigger_Int(void)
{
	Ddim_Print(("CortexA7_Cpu3_Crosstrigger_Int\n"));
}

// Cortex-A7 CPU0 System Metrics
VOID Pt_CortexA7_Cpu0_System_Metrics_Int(void)
{
	Ddim_Print(("CortexA7_Cpu0_System_Metrics_Int\n"));
}

// Cortex-A7 CPU1 System Metrics
VOID Pt_CortexA7_Cpu1_System_Metrics_Int(void)
{
	Ddim_Print(("CortexA7_Cpu1_System_Metrics_Int\n"));
}

// Cortex-A7 CPU2 System Metrics
VOID Pt_CortexA7_Cpu2_System_Metrics_Int(void)
{
	Ddim_Print(("CortexA7_Cpu2_System_Metrics_Int\n"));
}

// Cortex-A7 CPU3 System Metrics
VOID Pt_CortexA7_Cpu3_System_Metrics_Int(void)
{
	Ddim_Print(("CortexA7_Cpu3_System_Metrics_Int\n"));
}

// AXI Transaction Error
VOID Pt_Axi_Transaction_Error_Int(void)
{
	Ddim_Print(("AXI_Transaction_Error_Int\n"));
}

// SNAP Peri XDMAC ch0
VOID Pt_Snap_Peri_Xdmac_Ch0_Int(void)
{
	Ddim_Print(("SNAP_Peri_Xdmac_Ch0_Int\n"));
}

// SNAP Peri XDMAC ch1
VOID Pt_Snap_Peri_Xdmac_Ch1_Int(void)
{
	Ddim_Print(("SNAP_Peri_Xdmac_Ch1_Int\n"));
}

// SNAP Peri XDMAC ch2
VOID Pt_Snap_Peri_Xdmac_Ch2_Int(void)
{
	Ddim_Print(("SNAP_Peri_Xdmac_Ch2_Int\n"));
}

// SNAP Peri XDMAC ch3
VOID Pt_Snap_Peri_Xdmac_Ch3_Int(void)
{
	Ddim_Print(("SNAP_Peri_Xdmac_Ch3_Int\n"));
}

// Watchdog Timer
VOID Pt_Watchdog_Timer_Int(void)
{
	Ddim_Print(("Watchdog_Timer_Int\n"));
}

// IPCU0 interrupt ch0
VOID Pt_Ipcu0_Ch0_Int(void)
{
	Ddim_Print(("Ipcu0_Ch0_Int\n"));
}

// IPCU0 interrupt ch1
VOID Pt_Ipcu0_Ch1_Int(void)
{
	Ddim_Print(("Ipcu0_Ch1_Int\n"));
}

// IPCU0 interrupt ch2
VOID Pt_Ipcu0_Ch2_Int(void)
{
	Ddim_Print(("Ipcu0_Ch2_Int\n"));
}

// IPCU0 interrupt ch3
VOID Pt_Ipcu0_Ch3_Int(void)
{
	Ddim_Print(("Ipcu0_Ch3_Int\n"));
}

// IPCU0 interrupt ch4
VOID Pt_Ipcu0_Ch4_Int(void)
{
	Ddim_Print(("Ipcu0_Ch4_Int\n"));
}

// IPCU0 interrupt ch5
VOID Pt_Ipcu0_Ch5_Int(void)
{
	Ddim_Print(("Ipcu0_Ch5_Int\n"));
}

// IPCU0 interrupt ch6
VOID Pt_Ipcu0_Ch6_Int(void)
{
	Ddim_Print(("Ipcu0_Ch6_Int\n"));
}

// IPCU0 interrupt ch7
VOID Pt_Ipcu0_Ch7_Int(void)
{
	Ddim_Print(("Ipcu0_Ch7_Int\n"));
}

// IPCU0 interrupt ch8
VOID Pt_Ipcu0_Ch8_Int(void)
{
	Ddim_Print(("Ipcu0_Ch8_Int\n"));
}

// IPCU0 interrupt ch9
VOID Pt_Ipcu0_Ch9_Int(void)
{
	Ddim_Print(("Ipcu0_Ch9_Int\n"));
}

// IPCU0 interrupt ch10
VOID Pt_Ipcu0_Ch10_Int(void)
{
	Ddim_Print(("Ipcu0_Ch10_Int\n"));
}

// IPCU0 interrupt ch11
VOID Pt_Ipcu0_Ch11_Int(void)
{
	Ddim_Print(("Ipcu0_Ch11_Int\n"));
}

// IPCU0 interrupt ch12
VOID Pt_Ipcu0_Ch12_Int(void)
{
	Ddim_Print(("Ipcu0_Ch12_Int\n"));
}

// IPCU0 interrupt ch13
VOID Pt_Ipcu0_Ch13_Int(void)
{
	Ddim_Print(("Ipcu0_Ch13_Int\n"));
}

// IPCU0 interrupt ch14
VOID Pt_Ipcu0_Ch14_Int(void)
{
	Ddim_Print(("Ipcu0_Ch14_Int\n"));
}

// IPCU0 interrupt ch15
VOID Pt_Ipcu0_Ch15_Int(void)
{
	Ddim_Print(("Ipcu0_Ch15_Int\n"));
}

// IPCU1 interrupt ch0
VOID Pt_Ipcu1_Ch0_Int(void)
{
	Ddim_Print(("Ipcu1_Ch0_Int\n"));
}

// IPCU1 interrupt ch1
VOID Pt_Ipcu1_Ch1_Int(void)
{
	Ddim_Print(("Ipcu1_Ch1_Int\n"));
}

// IPCU1 interrupt ch2
VOID Pt_Ipcu1_Ch2_Int(void)
{
	Ddim_Print(("Ipcu1_Ch2_Int\n"));
}

// IPCU1 interrupt ch3
VOID Pt_Ipcu1_Ch3_Int(void)
{
	Ddim_Print(("Ipcu1_Ch3_Int\n"));
}

// IPCU1 interrupt ch4
VOID Pt_Ipcu1_Ch4_Int(void)
{
	Ddim_Print(("Ipcu1_Ch4_Int\n"));
}

// IPCU1 interrupt ch5
VOID Pt_Ipcu1_Ch5_Int(void)
{
	Ddim_Print(("Ipcu1_Ch5_Int\n"));
}

// IPCU1 interrupt ch6
VOID Pt_Ipcu1_Ch6_Int(void)
{
	Ddim_Print(("Ipcu1_Ch6_Int\n"));
}

// IPCU1 interrupt ch7
VOID Pt_Ipcu1_Ch7_Int(void)
{
	Ddim_Print(("Ipcu1_Ch7_Int\n"));
}

// IPCU1 interrupt ch8
VOID Pt_Ipcu1_Ch8_Int(void)
{
	Ddim_Print(("Ipcu1_Ch8_Int\n"));
}

// IPCU1 interrupt ch9
VOID Pt_Ipcu1_Ch9_Int(void)
{
	Ddim_Print(("Ipcu1_Ch9_Int\n"));
}

// IPCU1 interrupt ch10
VOID Pt_Ipcu1_Ch10_Int(void)
{
	Ddim_Print(("Ipcu1_Ch10_Int\n"));
}

// IPCU1 interrupt ch11
VOID Pt_Ipcu1_Ch11_Int(void)
{
	Ddim_Print(("Ipcu1_Ch11_Int\n"));
}

// IPCU1 interrupt ch12
VOID Pt_Ipcu1_Ch12_Int(void)
{
	Ddim_Print(("Ipcu1_Ch12_Int\n"));
}

// IPCU1 interrupt ch13
VOID Pt_Ipcu1_Ch13_Int(void)
{
	Ddim_Print(("Ipcu1_Ch13_Int\n"));
}

// IPCU1 interrupt ch14
VOID Pt_Ipcu1_Ch14_Int(void)
{
	Ddim_Print(("Ipcu1_Ch14_Int\n"));
}

// IPCU1 interrupt ch15
VOID Pt_Ipcu1_Ch15_Int(void)
{
	Ddim_Print(("Ipcu1_Ch15_Int\n"));
}

// External Interrupt ch16
VOID Pt_External_Ch16_Int(void)
{
	Ddim_Print(("External_Ch16_Int\n"));
}

// External Interrupt ch17
VOID Pt_External_Ch17_Int(void)
{
	Ddim_Print(("External_Ch17_Int\n"));
}

// External Interrupt ch18
VOID Pt_External_Ch18_Int(void)
{
	Ddim_Print(("External_Ch18_Int\n"));
}

// External Interrupt ch19
VOID Pt_External_Ch19_Int(void)
{
	Ddim_Print(("External_Ch19_Int\n"));
}

// External Interrupt ch20
VOID Pt_External_Ch20_Int(void)
{
	Ddim_Print(("External_Ch20_Int\n"));
}

// External Interrupt ch21
VOID Pt_External_Ch21_Int(void)
{
	Ddim_Print(("External_Ch21_Int\n"));
}

// External Interrupt ch22
VOID Pt_External_Ch22_Int(void)
{
	Ddim_Print(("External_Ch22_Int\n"));
}

// External Interrupt ch23
VOID Pt_External_Ch23_Int(void)
{
	Ddim_Print(("External_Ch23_Int\n"));
}

// External Interrupt ch24
VOID Pt_External_Ch24_Int(void)
{
	Ddim_Print(("External_Ch24_Int\n"));
}

// External Interrupt ch25
VOID Pt_External_Ch25_Int(void)
{
	Ddim_Print(("External_Ch25_Int\n"));
}

// External Interrupt ch26
VOID Pt_External_Ch26_Int(void)
{
	Ddim_Print(("External_Ch26_Int\n"));
}

// External Interrupt ch27
VOID Pt_External_Ch27_Int(void)
{
	Ddim_Print(("External_Ch27_Int\n"));
}

// External Interrupt ch28
VOID Pt_External_Ch28_Int(void)
{
	Ddim_Print(("External_Ch28_Int\n"));
}

// External Interrupt ch29
VOID Pt_External_Ch29_Int(void)
{
	Ddim_Print(("External_Ch29_Int\n"));
}

// External Interrupt ch30
VOID Pt_External_Ch30_Int(void)
{
	Ddim_Print(("External_Ch30_Int\n"));
}

// External Interrupt ch31
VOID Pt_External_Ch31_Int(void)
{
	Ddim_Print(("External_Ch31_Int\n"));
}

// PMC DMAC ADC.
VOID Pt_Pmc_Dmac_Adc_Int(void)
{
	Ddim_Print(("PMC_DMAC_ADC_Int\n"));
}

// PMC DMAC Receive.
VOID Pt_Pmc_Dmac_Receive_Int(void)
{
	Ddim_Print(("PMC_DMAC_Receive_Int\n"));
}

// PMC UART Transmission Data.
VOID Pt_Pmc_Uart_Transmission_Data_Int(void)
{
	Ddim_Print(("PMC_UART_Transmission_Data_Int\n"));
}

// PMC DMAC QSPI Transmission.
VOID Pt_Pmc_Dmac_Qspi_Transmission_Int(void)
{
	Ddim_Print(("PMC_DMAC_QSPI_Transmission_Int\n"));
}

// PMC DMAC QSPI Receive.
VOID Pt_Pmc_Dmac_Qspi_Receive_Int(void)
{
	Ddim_Print(("PMC_DMAC_QSPI_Receive_Int\n"));
}

// PMC UART Receive Relation.
VOID Pt_Pmc_Uart_Receive_Relation_Int(void)
{
	Ddim_Print(("PMC_UART_Receive_Relation_Int\n"));
}

// PMC UART Transmission Relation.
VOID Pt_Pmc_Uart_Transmission_Relation_Int(void)
{
	Ddim_Print(("Pmc_Uart_Transmission_Relation_Int\n"));
}

// PMC QSPI.
VOID Pt_Pmc_Qspi_Int(void)
{
	Ddim_Print(("PMC_Qspi_Int\n"));
}

// CALENDAR Alarm.
VOID Pt_Calendar_Alarm_Int(void)
{
	Ddim_Print(("Calendar_Alarm_Int\n"));
}

// PMC 32bit Timer Under Flow.
VOID Pt_Pmc_32bit_Timer_Under_Flow_Int(void)
{
	Ddim_Print(("PMC_32bit_Timer_Under_Floe_Int\n"));
}

// ADC.
VOID Pt_Adc_Int(void)
{
	Ddim_Print(("ADC_Int\n"));
}

// PMC I2C.
VOID Pt_Pmc_I2c_Int(void)
{
	Ddim_Print(("PMC_I2c_Int\n"));
}

// PMC Watchdog.
VOID Pt_Pmc_Watchdog_Int(void)
{
	Ddim_Print(("PMC_Watchdog_Int\n"));
}

// Temperature.
VOID Pt_Temperature_Int(void)
{
	Ddim_Print(("Temperature_Int\n"));
}

// PMU.
VOID Pt_Pmu_Int(void)
{
	Ddim_Print(("PMU_Int\n"));
}

// PMC Gpio.
VOID Pt_Pmc_Gpio_Int(void)
{
	Ddim_Print(("PMC_Gpio_Int\n"));
}

// Timer ch0
VOID Pt_Timer_Ch0_Int(void)
{
	Ddim_Print(("Timer_Ch0_Int\n"));
	Dd_TMR32_Int_Handler(0);
}

// Timer ch1
VOID Pt_Timer_Ch1_Int(void)
{
	Ddim_Print(("Timer_Ch1_Int\n"));
	Dd_TMR32_Int_Handler(1);
}

// Timer ch2
VOID Pt_Timer_Ch2_Int(void)
{
	Ddim_Print(("Timer_Ch2_Int\n"));
	Dd_TMR32_Int_Handler(2);
}

// Timer ch3
VOID Pt_Timer_Ch3_Int(void)
{
	Ddim_Print(("Timer_Ch3_Int\n"));
	Dd_TMR32_Int_Handler(3);
}

// Timer ch4
VOID Pt_Timer_Ch4_Int(void)
{
	Ddim_Print(("Timer_Ch4_Int\n"));
	Dd_TMR32_Int_Handler(4);
}

// Timer ch5
VOID Pt_Timer_Ch5_Int(void)
{
	Ddim_Print(("Timer_Ch5_Int\n"));
	Dd_TMR32_Int_Handler(5);
}

// Timer ch6
VOID Pt_Timer_Ch6_Int(void)
{
	Ddim_Print(("Timer_Ch6_Int\n"));
	Dd_TMR32_Int_Handler(6);
}

// Timer ch7
VOID Pt_Timer_Ch7_Int(void)
{
	Ddim_Print(("Timer_Ch7_Int\n"));
	Dd_TMR32_Int_Handler(7);
}

// Timer ch8
VOID Pt_Timer_Ch8_Int(void)
{
	Ddim_Print(("Timer_Ch8_Int\n"));
	Dd_TMR32_Int_Handler(8);
}

// Timer ch9
VOID Pt_Timer_Ch9_Int(void)
{
	Ddim_Print(("Timer_Ch9_Int\n"));
	Dd_TMR32_Int_Handler(9);
}

// Timer ch10
VOID Pt_Timer_Ch10_Int(void)
{
	Ddim_Print(("Timer_Ch10_Int\n"));
	Dd_TMR32_Int_Handler(10);
}

// Timer ch11
VOID Pt_Timer_Ch11_Int(void)
{
	Ddim_Print(("Timer_Ch11_Int\n"));
	Dd_TMR32_Int_Handler(11);
}

// Timer ch12
VOID Pt_Timer_Ch12_Int(void)
{
	Ddim_Print(("Timer_Ch12_Int\n"));
	Dd_TMR32_Int_Handler(12);
}

// Timer ch13
VOID Pt_Timer_Ch13_Int(void)
{
	Ddim_Print(("Timer_Ch13_Int\n"));
	Dd_TMR32_Int_Handler(13);
}

// I2C ch0
VOID Pt_I2c_Ch0_Int(void)
{
	Ddim_Print(("I2c_Ch0_Int\n"));
}

// I2C ch1
VOID Pt_I2c_Ch1_Int(void)
{
	Ddim_Print(("I2c_Ch1_Int\n"));
}

// I2C ch2
VOID Pt_I2c_Ch2_Int(void)
{
	Ddim_Print(("I2c_Ch2_Int\n"));
}

// Up/Down Counter ch0
VOID Pt_Updown_Counter_Ch0_Int(void)
{
	Ddim_Print(("Updown_Counter_Ch0_Int\n"));
}

// Up/Down Counter ch1
VOID Pt_Updown_Counter_Ch1_Int(void)
{
	Ddim_Print(("Updown_Counter_Ch1_Int\n"));
}

// Up/Down Counter ch2
VOID Pt_Updown_Counter_Ch2_Int(void)
{
	Ddim_Print(("Updown_Counter_Ch2_Int\n"));
}

// Up/Down Counter ch3
VOID Pt_Updown_Counter_Ch3_Int(void)
{
	Ddim_Print(("Updown_Counter_Ch3_Int\n"));
}

// Up/Down Counter ch4
VOID Pt_Updown_Counter_Ch4_Int(void)
{
	Ddim_Print(("Updown_Counter_Ch4_Int\n"));
}

// Up/Down Counter ch5
VOID Pt_Updown_Counter_Ch5_Int(void)
{
	Ddim_Print(("Updown_Counter_Ch5_Int\n"));
}

// PWM ch0-3
VOID Pt_Pwm_Ch0_Ch3_Int(void)
{
	Ddim_Print(("Pwm_Ch0_Ch3_Int\n"));
}

// PWM ch4-7
VOID Pt_Pwm_Ch4_Ch7_Int(void)
{
	Ddim_Print(("Pwm_Ch4_Ch7_Int\n"));
}

// PWM ch8-11
VOID Pt_Pwm_Ch8_Ch11_Int(void)
{
	Ddim_Print(("Pwm_Ch8_Ch11_Int\n"));
}

// PWM ch12-15
VOID Pt_Pwm_Ch12_Ch15_Int(void)
{
	Ddim_Print(("Pwm_Ch12_Ch15_Int\n"));
}

// PWM ch0-3 STP
VOID Pt_Pwm_Ch0_Ch3_Stp_Int(void)
{
	Ddim_Print(("Pwm_Ch0_Ch3_Stp_Int\n"));
}

// PWM ch4-7 STP
VOID Pt_Pwm_Ch4_Ch7_Stp_Int(void)
{
	Ddim_Print(("Pwm_Ch4_Ch7_Stp_Int\n"));
}

// PWM ch8-11 STP
VOID Pt_Pwm_Ch8_Ch11_Stp_Int(void)
{
	Ddim_Print(("Pwm_Ch8_Ch11_Stp_Int\n"));
}

// PWM ch12-15 STP
VOID Pt_Pwm_Ch12_Ch15_Stp_Int(void)
{
	Ddim_Print(("Pwm_Ch12_Ch15_Stp_Int\n"));
}

// Audio I/F ch0
VOID Pt_Audio_If_Ch0_Int(void)
{
	Ddim_Print(("Audio_If_Ch0_Int\n"));
}

// Audio I/F ch1
VOID Pt_Audio_If_Ch1_Int(void)
{
	Ddim_Print(("Audio_If_Ch1_Int\n"));
}

// Audio I/F ch2
VOID Pt_Audio_If_Ch2_Int(void)
{
	Ddim_Print(("Audio_If_Ch2_Int\n"));
}

// Audio I/F ch3
VOID Pt_Audio_If_Ch3_Int(void)
{
	Ddim_Print(("Audio_If_Ch3_Int\n"));
}

// Audio I/F ch4
VOID Pt_Audio_If_Ch4_Int(void)
{
	Ddim_Print(("Audio_If_Ch4_Int\n"));
}

// Audio I/F ch5
VOID Pt_Audio_If_Ch5_Int(void)
{
	Ddim_Print(("Audio_If_Ch5_Int\n"));
}

// Peripheral HDMAC0 ch0
VOID Pt_Peri_Hdmac0_Ch0_Int(void)
{
	Ddim_Print(("Peri_Hdmac0_Ch0_Int\n"));
}

// Peripheral HDMAC0 ch1
VOID Pt_Peri_Hdmac0_Ch1_Int(void)
{
	Ddim_Print(("Peri_Hdmac0_Ch1_Int\n"));
}

// Peripheral HDMAC0 ch2
VOID Pt_Peri_Hdmac0_Ch2_Int(void)
{
	Ddim_Print(("Peri_Hdmac0_Ch2_Int\n"));
}

// Peripheral HDMAC0 ch3
VOID Pt_Peri_Hdmac0_Ch3_Int(void)
{
	Ddim_Print(("Peri_Hdmac0_Ch3_Int\n"));
}

// Peripheral HDMAC0 ch4
VOID Pt_Peri_Hdmac0_Ch4_Int(void)
{
	Ddim_Print(("Peri_Hdmac0_Ch4_Int\n"));
}

// Peripheral HDMAC0 ch5
VOID Pt_Peri_Hdmac0_Ch5_Int(void)
{
	Ddim_Print(("Peri_Hdmac0_Ch5_Int\n"));
}

// Peripheral HDMAC0 ch6
VOID Pt_Peri_Hdmac0_Ch6_Int(void)
{
	Ddim_Print(("Peri_Hdmac0_Ch6_Int\n"));
}

// Peripheral HDMAC0 ch7
VOID Pt_Peri_Hdmac0_Ch7_Int(void)
{
	Ddim_Print(("Peri_Hdmac0_Ch7_Int\n"));
}

// Peripheral HDMAC1 ch0
VOID Pt_Peri_Hdmac1_Ch0_Int(void)
{
	Ddim_Print(("Peri_Hdmac1_Ch0_Int\n"));
}

// Peripheral HDMAC1 ch1
VOID Pt_Peri_Hdmac1_Ch1_Int(void)
{
	Ddim_Print(("Peri_Hdmac1_Ch1_Int\n"));
}

// Peripheral HDMAC1 ch2
VOID Pt_Peri_Hdmac1_Ch2_Int(void)
{
	Ddim_Print(("Peri_Hdmac1_Ch2_Int\n"));
}

// Peripheral HDMAC1 ch3
VOID Pt_Peri_Hdmac1_Ch3_Int(void)
{
	Ddim_Print(("Peri_Hdmac1_Ch3_Int\n"));
}

// Peripheral HDMAC1 ch4
VOID Pt_Peri_Hdmac1_Ch4_Int(void)
{
	Ddim_Print(("Peri_Hdmac1_Ch4_Int\n"));
}

// Peripheral HDMAC1 ch5
VOID Pt_Peri_Hdmac1_Ch5_Int(void)
{
	Ddim_Print(("Peri_Hdmac1_Ch5_Int\n"));
}

// Peripheral HDMAC1 ch6
VOID Pt_Peri_Hdmac1_Ch6_Int(void)
{
	Ddim_Print(("Peri_Hdmac1_Ch6_Int\n"));
}

// Peripheral HDMAC1 ch7
VOID Pt_Peri_Hdmac1_Ch7_Int(void)
{
	Ddim_Print(("Peri_Hdmac1_Ch7_Int\n"));
}

// UART ch0 Reception
VOID Pt_Uart_Ch0_Reception_Int(void)
{
	Ddim_Print(("Uart_Ch0_Reception_Int\n"));
}

// UART ch1 Reception
VOID Pt_Uart_Ch1_Reception_Int(void)
{
	Ddim_Print(("Uart_Ch1_Reception_Int\n"));
}


// UART ch2 Reception
VOID Pt_Uart_Ch2_Reception_Int(void)
{
	Ddim_Print(("Uart_Ch2_Reception_Int\n"));
}

// UART ch3 Reception
VOID Pt_Uart_Ch3_Reception_Int(void)
{
	Ddim_Print(("Uart_Ch3_Reception_Int\n"));
}

// UART ch4 Reception
VOID Pt_Uart_Ch4_Reception_Int(void)
{
	Ddim_Print(("Uart_Ch4_Reception_Int\n"));
}

// UART ch5 Reception
VOID Pt_Uart_Ch5_Reception_Int(void)
{
	Ddim_Print(("Uart_Ch5_Reception_Int\n"));
}

// UART ch6 Reception
VOID Pt_Uart_Ch6_Reception_Int(void)
{
	Ddim_Print(("Uart_Ch6_Reception_Int\n"));
}

// UART ch7 Reception
VOID Pt_Uart_Ch7_Reception_Int(void)
{
	Ddim_Print(("Uart_Ch7_Reception_Int\n"));
}

// UART ch0 Transmission
VOID Pt_Uart_Ch0_Transmission_Int(void)
{
	Ddim_Print(("Uart_Ch0_Transmission_Int\n"));
}

// UART ch1 Transmission
VOID Pt_Uart_Ch1_Transmission_Int(void)
{
	Ddim_Print(("Uart_Ch1_Transmission_Int\n"));
}

// UART ch2 Transmission
VOID Pt_Uart_Ch2_Transmission_Int(void)
{
	Ddim_Print(("Uart_Ch2_Transmission_Int\n"));
}

// UART ch3 Transmission
VOID Pt_Uart_Ch3_Transmission_Int(void)
{
	Ddim_Print(("Uart_Ch3_Transmission_Int\n"));
}

// UART ch4 Transmission
VOID Pt_Uart_Ch4_Transmission_Int(void)
{
	Ddim_Print(("Uart_Ch4_Transmission_Int\n"));
}

// UART ch5 Transmission
VOID Pt_Uart_Ch5_Transmission_Int(void)
{
	Ddim_Print(("Uart_Ch5_Transmission_Int\n"));
}

// UART ch6 Transmission
VOID Pt_Uart_Ch6_Transmission_Int(void)
{
	Ddim_Print(("Uart_Ch6_Transmission_Int\n"));
}

// UART ch7 Transmission
VOID Pt_Uart_Ch7_Transmission_Int(void)
{
	Ddim_Print(("Uart_Ch7_Transmission_Int\n"));
}

// SPI ch0
VOID Pt_Spi_Ch0_Int(void)
{
	Ddim_Print(("SPI_Ch0_Int\n"));
}

// SPI ch1
VOID Pt_Spi_Ch1_Int(void)
{
	Ddim_Print(("SPI_Ch1_Int\n"));
}

// SPI ch2
VOID Pt_Spi_Ch2_Int(void)
{
	Ddim_Print(("SPI_Ch2_Int\n"));
}

// PDM ch0 DMA0
VOID Pt_Pdm_Ch0_Dma0_Int(void)
{
	Ddim_Print(("PDM_Ch0_DMA0_Int\n"));
}

// PDM ch0 DMA1
VOID Pt_Pdm_Ch0_Dma1_Int(void)
{
	Ddim_Print(("PDM_Ch0_DMA1_Int\n"));
}

// PDM ch0 Fifo Overflow
VOID Pt_Pdm_Ch0_Fifo_Overflow_Int(void)
{
	Ddim_Print(("PDM_Ch0_Fifo_Overflow_Int\n"));
}

// PDM ch1 DMA0
VOID Pt_Pdm_Ch1_Dma0_Int(void)
{
	Ddim_Print(("PDM_Ch1_DMA0_Int\n"));
}

// PDM ch1 DMA1
VOID Pt_Pdm_Ch1_Dma1_Int(void)
{
	Ddim_Print(("PDM_Ch1_DMA1_Int\n"));
}

// PDM ch1 Fifo Overflow
VOID Pt_Pdm_Ch1_Fifo_Overflow_Int(void)
{
	Ddim_Print(("PDM_Ch1_Fifo_Overflow_Int\n"));
}

// SLIMBUS ch0
VOID Pt_Slimbus_Ch0_Int(void)
{
	Ddim_Print(("SLIMBUS_Ch0_Int\n"));
}

// SLIMBUS ch1
VOID Pt_Slimbus_Ch1_Int(void)
{
	Ddim_Print(("SLIMBUS_Ch1_Int\n"));
}

// SLIMBUS ch2
VOID Pt_Slimbus_Ch2_Int(void)
{
	Ddim_Print(("SLIMBUS_Ch2_Int\n"));
}

// SLIMBUS ch3
VOID Pt_Slimbus_Ch3_Int(void)
{
	Ddim_Print(("SLIMBUS_Ch3_Int\n"));
}

// Image SDRAMC0
VOID Pt_Sdramc_Ch0_Int(void)
{
	Ddim_Print(("Sdramc_Ch0_Int\n"));
	Im_SDRAMC_Int_Handler( E_IM_SDRAMC_CH_0 );
}

// Image SDRAMC1
VOID Pt_Sdramc_Ch1_Int(void)
{
	Ddim_Print(("Sdramc_Ch1_Int\n"));
	Im_SDRAMC_Int_Handler( E_IM_SDRAMC_CH_1 );
}

// Image MXIC Unit0
VOID Pt_Mxic_Unit0_Int(void)
{
	Ddim_Print(("Mxic_Unit0_Int\n"));
	Im_MXIC_Int_Handler( E_IM_MXIC_UNIT_0 );
}

// Image MXIC Unit1
VOID Pt_Mxic_Unit1_Int(void)
{
	Ddim_Print(("Mxic_Unit1_Int\n"));
	Im_MXIC_Int_Handler( E_IM_MXIC_UNIT_1 );
}

// Image MXIC Unit2
VOID Pt_Mxic_Unit2_Int(void)
{
	Ddim_Print(("Mxic_Unit2_Int\n"));
	Im_MXIC_Int_Handler( E_IM_MXIC_UNIT_2 );
}

// Image MXIC Unit3
VOID Pt_Mxic_Unit3_Int(void)
{
	Ddim_Print(("Mxic_Unit3_Int\n"));
	Im_MXIC_Int_Handler( E_IM_MXIC_UNIT_3 );
}

// Image MXIC Unit4
VOID Pt_Mxic_Unit4_Int(void)
{
	Ddim_Print(("Mxic_Unit4_Int\n"));
	Im_MXIC_Int_Handler( E_IM_MXIC_UNIT_4 );
}

// Image MXIC Unit5
VOID Pt_Mxic_Unit5_Int(void)
{
	Ddim_Print(("Mxic_Unit5_Int\n"));
	Im_MXIC_Int_Handler( E_IM_MXIC_UNIT_5 );
}

// Image MXIC Unit6
VOID Pt_Mxic_Unit6_Int(void)
{
	Ddim_Print(("Mxic_Unit6_Int\n"));
	Im_MXIC_Int_Handler( E_IM_MXIC_UNIT_6 );
}

// Image SEN
VOID Pt_Sen_Int(void)
{
	Ddim_Print(("SEN_Int\n"));
}

// Image SEN VD
VOID Pt_Sen_Vd_Int(void)
{
	Ddim_Print(("SEN_VD_Int\n"));
}

// Image SEN HD
VOID Pt_Sen_Hd_Int(void)
{
	Ddim_Print(("SEN_HD_Int\n"));
}

// Image SENSOR IF Macro0
VOID Pt_Sensor_If_Macro0_Int(void)
{
	Ddim_Print(("SENSOR_IF_Macro0_Int\n"));
}

// Image SENSOR IF Macro1
VOID Pt_Sensor_If_Macro1_Int(void)
{
	Ddim_Print(("SENSOR_IF_Macro1_Int\n"));
}

// Image SENSOR IF Macro2
VOID Pt_Sensor_If_Macro2_Int(void)
{
	Ddim_Print(("SENSOR_IF_Macro2_Int\n"));
}

// Image SENSOR IF Macro3
VOID Pt_Sensor_If_Macro3_Int(void)
{
	Ddim_Print(("SENSOR_IF_Macro3_Int\n"));
}

// Image SRO Unit1
VOID Pt_Sro_Unit1_Int(void)
{
	Ddim_Print(("SRO_Unit1_Int\n"));
}

// Image SRO Unit1 VD
VOID Pt_Sro_Unit1_Vd_Int(void)
{
	Ddim_Print(("SRO_Unit1_VD_Int\n"));
}

// Image SRO Unit1 HD
VOID Pt_Sro_Unit1_Hd_Int(void)
{
	Ddim_Print(("SRO_Unit1_HD_Int\n"));
}

// Image SRO Unit2
VOID Pt_Sro_Unit2_Int(void)
{
	Ddim_Print(("SRO_Unit2_Int\n"));
}

// Image SRO Unit2 VD
VOID Pt_Sro_Unit2_Vd_Int(void)
{
	Ddim_Print(("SRO_Unit2_VD_Int\n"));
}

// Image SRO Unit2 HD
VOID Pt_Sro_Unit2_Hd_Int(void)
{
	Ddim_Print(("SRO_Unit2_HD_Int\n"));
}

// Image B2B Unit1
VOID Pt_B2b_Unit1_Int(void)
{
	Ddim_Print(("B2B_Unit1_Int\n"));
}

// Image B2B Unit1 VD
VOID Pt_B2b_Unit1_Vd_Int(void)
{
	Ddim_Print(("B2B_Unit1_VD_Int\n"));
}

// Image B2B Unit1 HD
VOID Pt_B2b_Unit1_Hd_Int(void)
{
	Ddim_Print(("B2B_Unit1_HD_Int\n"));
}

// Image B2B Unit2
VOID Pt_B2b_Unit2_Int(void)
{
	Ddim_Print(("B2B_Unit2_Int\n"));
}

// Image B2B Unit2 VD
VOID Pt_B2b_Unit2_Vd_Int(void)
{
	Ddim_Print(("B2B_Unit2_VD_Int\n"));
}

// Image B2B Unit2 HD
VOID Pt_B2b_Unit2_Hd_Int(void)
{
	Ddim_Print(("B2B_Unit2_HD_Int\n"));
}

// Image Start
VOID Pt_Start_Int(void)
{
	Ddim_Print(("Start_Int\n"));
}

// Image PAS Unit2
VOID Pt_Pas_Unit2_Int(void)
{
	Ddim_Print(("PAS_Unit2_Int\n"));
}

// Image PAS Unit2 VD
VOID Pt_Pas_Unit2_Vd_Int(void)
{
	Ddim_Print(("PAS_Unit2_VD_Int\n"));
}

// Image PAS Unit2 HD
VOID Pt_Pas_Unit2_Hd_Int(void)
{
	Ddim_Print(("PAS_Unit2_HD_Int\n"));
}

// Image B2R Unit1
VOID Pt_B2r_Unit1_Int(void)
{
	Ddim_Print(("B2R_Unit1_Int\n"));
	Im_B2R_Int_Handler( D_IM_B2R_PIPE1 );
}

// Image B2R Unit2
VOID Pt_B2r_Unit2_Int(void)
{
	Ddim_Print(("B2R_Unit2_Int\n"));
	Im_B2R_Int_Handler( D_IM_B2R_PIPE2 );
}

// Image LTM Unit1
VOID Pt_Ltm_Unit1_Int(void)
{
	Ddim_Print(("LTM_Unit1_Int\n"));
	Im_LTM_Int_Handler( D_IM_LTM_PIPE1 );
}

// Image LTM Unit2
VOID Pt_Ltm_Unit2_Int(void)
{
	Ddim_Print(("LTM_Unit2_Int\n"));
	Im_LTM_Int_Handler( D_IM_LTM_PIPE2 );
}

// Image R2Y Unit1
VOID Pt_R2y_Unit1_Int(void)
{
	Ddim_Print(("R2Y_Unit1_Int\n"));
	Im_R2Y_Int_Handler( D_IM_R2Y_PIPE1 );
}

// Image R2Y Unit2
VOID Pt_R2y_Unit2_Int(void)
{
	Ddim_Print(("R2Y_Unit2_Int\n"));
	Im_R2Y_Int_Handler( D_IM_R2Y_PIPE2 );
}

// Image CNR Unit1
VOID Pt_Cnr_Unit1_Int(void)
{
	Ddim_Print(("CNR_Unit1_Int\n"));
	Im_CNR0_Int_Handler();
}

// Image CNR Unit2
VOID Pt_Cnr_Unit2_Int(void)
{
	Ddim_Print(("CNR_Unit2_Int\n"));
	Im_CNR1_Int_Handler();
}

// Image IIP
VOID Pt_Iip_Int(void)
{
	Ddim_Print(("IIP_Int\n"));
	Im_IIP_Int_Handler();
}

// Image SHDR
VOID Pt_Shdr_Int(void)
{
	Ddim_Print(("SHDR_Int\n"));
	Im_SHDR_Int_Handler();
}

// Image SHDR Line
VOID Pt_Shdr_Line_Int(void)
{
	Ddim_Print(("SHDR_Line_Int\n"));
}

// Image ME
VOID Pt_Me_Int(void)
{
	Ddim_Print(("ME_Int\n"));
	Im_ME_Int_Handler();
}

// Image Graphics
VOID Pt_Graphics_Int(void)
{
	Ddim_Print(("Graphics_Int\n"));
}

// Image JPEG
VOID Pt_Jpeg_Int(void)
{
	Ddim_Print(("Jpeg_Int start\n"));
	Im_JPEG_Int_Handler();
	Ddim_Print(("Jpeg_Int end\n"));
}

// Image RAW
VOID Pt_Raw_Int(void)
{
	Ddim_Print(("RAW_Int\n"));
	Im_RAW_Int_Handler();
}

// Image IPU
VOID Pt_Ipu_Int(void)
{
	Ddim_Print(("IPU_Int\n"));
}

// Image ELA Unit
VOID Pt_Ela_Unit_Int(void)
{
	Ddim_Print(("Ela_Unit_Int\n"));
	Im_ELA_Int_Handler();
}

// Image Xch
VOID Pt_Xch_Int(void)
{
	Ddim_Print(("Xch_Int\n"));
	Im_Xch_Int_Handler();
}

// Image FPT Unit0
VOID Pt_Fpt_Unit0_Int(void)
{
	Ddim_Print(("FPT_Unit0_Int\n"));
	Im_FPT_Int_Handler( 0 );
}

// Image FPT Unit1
VOID Pt_Fpt_Unit1_Int(void)
{
	Ddim_Print(("FPT_Unit1_Int\n"));
	Im_FPT_Int_Handler( 1 );
}

// Image DISP LCD
VOID Pt_Disp_Lcd_Int(void)
{
	Ddim_Print(("DISP_LCD_Int\n"));
}

// Image DISP HDMI
VOID Pt_Disp_Hdmi_Int(void)
{
	Ddim_Print(("DISP_HDMI_Int\n"));
}

// Image DISP MIPI DSI
VOID Pt_Disp_Mipi_Dsi_Int(void)
{
	Ddim_Print(("DISP_MIPI_DSI_Int\n"));
}

// Image MIPI DSI Tx
VOID Pt_Mipi_Dsi_Tx_Int(void)
{
	Ddim_Print(("MIPI_DSI_Tx_Int\n"));
}

// Image DISP DWCH
VOID Pt_Disp_Dwch_Int(void)
{
	Ddim_Print(("DISP_Dwch_Int\n"));
}

// Image RDMA
VOID Pt_Rdma_Int(void)
{
	Ddim_Print(("RDMA_Int\n"));
}

// HDMI Master
VOID Pt_Hdmi_Master_Int(void)
{
	Ddim_Print(("HDMI_Master_Int\n"));
}

// HDMI Cec
VOID Pt_Hdmi_Cec_Int(void)
{
	Ddim_Print(("HDMI_Cec_Int\n"));
}

// BMH Unit0
VOID Pt_BMH_Unit0_Int(void)
{
	Ddim_Print(("BMH_Unit0_Int\n"));
	Im_BMH_Int_Handler( 0 );
}

// BMH Unit1
VOID Pt_BMH_Unit1_Int(void)
{
	Ddim_Print(("BMH_Unit1_Int\n"));
	Im_BMH_Int_Handler( 1 );
}

// RIBERY RBR
VOID Pt_Ribery_Rbr_Int(void)
{
	Ddim_Print(("Ribery_Rbr_Int\n"));
}

// RIBERY RWDT
VOID Pt_Ribery_Rwdt_Int(void)
{
	Ddim_Print(("Ribery_RWDT_Int\n"));
}

// INTERPROCESSOR0
VOID Pt_Interprocessor0_Int(void)
{
	Ddim_Print(("INTERPROCESSOR0_Int\n"));
}

// INTERPROCESSOR1
VOID Pt_Interprocessor1_Int(void)
{
	Ddim_Print(("INTERPROCESSOR1_Int\n"));
}

// INTERPROCESSOR2
VOID Pt_Interprocessor2_Int(void)
{
	Ddim_Print(("INTERPROCESSOR2_Int\n"));
}

// INTERPROCESSOR3
VOID Pt_Interprocessor3_Int(void)
{
	Ddim_Print(("INTERPROCESSOR3_Int\n"));
}

// DSP Peripheral Unit Ipcu0
VOID Pt_Dsp_Peripheral_Unit_Ipcu0_Int(void)
{
	Ddim_Print(("DSP_Peripheral_Unit_Ipcu0_Int\n"));
}

// DSP Peripheral Unit Ipcu1
VOID Pt_Dsp_Peripheral_Unit_Ipcu1_Int(void)
{
	Ddim_Print(("DSP_Peripheral_Unit_Ipcu1_Int\n"));
}

// DSP Peripheral Unit Ipcu2
VOID Pt_Dsp_Peripheral_Unit_Ipcu2_Int(void)
{
	Ddim_Print(("DSP_Peripheral_Unit_Ipcu2_Int\n"));
}

// DSP Peripheral Unit Ipcu3
VOID Pt_Dsp_Peripheral_Unit_Ipcu3_Int(void)
{
	Ddim_Print(("DSP_Peripheral_Unit_Ipcu3_Int\n"));
}

// DSP Debug
VOID Pt_Dsp_Debug_Int(void)
{
	Ddim_Print(("Dsp_Debug_Int\n"));
}

// NandFlash
VOID Pt_Nand_Flash_Int(void)
{
	Ddim_Print(("Nand_Flash_Int\n"));
}

// PCIE ch0 Msi
VOID Pt_Pcie_Ch0_Msi_Int(void)
{
	Ddim_Print(("Pcie_Ch0_Msi_Int\n"));
}

// PCIE ch0 System Error
VOID Pt_Pcie_Ch0_System_Error_Int(void)
{
	Ddim_Print(("Pcie_Ch0_System_Error_Int\n"));
}

// PCIE ch0 Intx
VOID Pt_Pcie_Ch0_Intx_Int(void)
{
	Ddim_Print(("Pcie_Ch0_Intx_Int\n"));
}

// PCIE ch0 Core
VOID Pt_Pcie_Ch0_Core_Int(void)
{
	Ddim_Print(("Pcie_Ch0_Core_Int\n"));
}

// PCIE ch0 DMA
VOID Pt_Pcie_Ch0_DMA_Int(void)
{
	Ddim_Print(("Pcie_Ch0_DMA_Int\n"));
}

// PCIE ch1 Msi
VOID Pt_Pcie_Ch1_Msi_Int(void)
{
	Ddim_Print(("Pcie_Ch1_Msi_Int\n"));
}

// PCIE ch1 System Error
VOID Pt_Pcie_Ch1_System_Error_Int(void)
{
	Ddim_Print(("Pcie_Ch1_System_Error_Int\n"));
}

// PCIE ch1 Intx
VOID Pt_Pcie_Ch1_Intx_Int(void)
{
	Ddim_Print(("Pcie_Ch1_Intx_Int\n"));
}

// PCIE ch1 Core
VOID Pt_Pcie_Ch1_Core_Int(void)
{
	Ddim_Print(("Pcie_Ch1_Core_Int\n"));
}

// PCIE ch1 DMA
VOID Pt_Pcie_Ch1_DMA_Int(void)
{
	Ddim_Print(("Pcie_Ch1_DMA_Int\n"));
}

// External Storage XDMAC ch0
VOID Pt_Exs_Xdmac_Ch0_Int(void)
{
	Ddim_Print(("Exs_XDMAC_Ch0_Int\n"));
}

// External Storage XDMAC ch1
VOID Pt_Exs_Xdmac_Ch1_Int(void)
{
	Ddim_Print(("Exs_XDMAC_Ch1_Int\n"));
}

// External Storage XDMAC ch2
VOID Pt_Exs_Xdmac_Ch2_Int(void)
{
	Ddim_Print(("Exs_XDMAC_Ch2_Int\n"));
}

// External Storage XDMAC ch3
VOID Pt_Exs_Xdmac_Ch3_Int(void)
{
	Ddim_Print(("Exs_XDMAC_Ch3_Int\n"));
}

// External Storage HDMAC ch0
VOID Pt_Exs_Hdmac_Ch0_Int(void)
{
	Ddim_Print(("Exs_HDMAC_Ch0_Int\n"));
}

// External Storage HDMAC ch1
VOID Pt_Exs_Hdmac_Ch1_Int(void)
{
	Ddim_Print(("Exs_HDMAC_Ch1_Int\n"));
}

// External Storage HDMAC ch2
VOID Pt_Exs_Hdmac_Ch2_Int(void)
{
	Ddim_Print(("Exs_HDMAC_Ch2_Int\n"));
}

// External Storage HDMAC ch3
VOID Pt_Exs_Hdmac_Ch3_Int(void)
{
	Ddim_Print(("Exs_HDMAC_Ch3_Int\n"));
}

// USB20 Hdc
VOID Pt_Usb20_Hdc_Int(void)
{
	Ddim_Print(("USB20_Hdc_Int\n"));
}

// USB20 Host EHCI
VOID Pt_Usb20_Host_Ehci_Int(void)
{
	Ddim_Print(("USB20_Host_Ehci_Int\n"));
}

// USB20 Host OHCI
VOID Pt_Usb20_Host_Ohci_Int(void)
{
	Ddim_Print(("USB20_Host_Ohci_Int\n"));
}

// USB30
VOID Pt_Usb30_Int(void)
{
	Ddim_Print(("USB30_Int\n"));
}

// Output Signal1
VOID Pt_Output_Signal1_Int(void)
{
	Ddim_Print(("Output_Signal1_Int\n"));
}

// Output Signal2
VOID Pt_Output_Signal2_Int(void)
{
	Ddim_Print(("Output_Signal2_Int\n"));
}

// PME SD card1
VOID Pt_Pme_Sd_Card1_Int(void)
{
	Ddim_Print(("PME_SD_Card1_Int\n"));
}

// PME F SDH30 UHS1
VOID Pt_Pme_F_Sdh30_Uhs1_Int(void)
{
	Ddim_Print(("PME_F_SDH30_Uhs1_Int\n"));
}

// Output Signal3
VOID Pt_Output_Signal3_Int(void)
{
	Ddim_Print(("Output_Signal3_Int\n"));
}

// F SDH30 UHS1
VOID Pt_F_Sdh30_Uhs1_Int(void)
{
	Ddim_Print(("F_SDH30_Uhs1_Int\n"));
}

// F SDH40 UHS2
VOID Pt_F_Sdh40_Uhs2_Int(void)
{
	Ddim_Print(("F_SDH40_Uhs2_Int\n"));
}

// PME F SDH40 UHS2
VOID Pt_Pme_F_Sdh40_Uhs2_Int(void)
{
	Ddim_Print(("PME_F_SDH40_Uhs2_Int\n"));
}

// Output Signal4
VOID Pt_Output_Signal4_Int(void)
{
	Ddim_Print(("Output_Signal4_Int\n"));
}

// Output Signal5
VOID Pt_Output_Signal5_Int(void)
{
	Ddim_Print(("Output_Signal5_Int\n"));
}

// PME SD card2
VOID Pt_Pme_Sd_Card2_Int(void)
{
	Ddim_Print(("PME_SD_Card2_Int\n"));
}

// Output Signal6
VOID Pt_Output_Signal6_Int(void)
{
	Ddim_Print(("Output_Signal6_Int\n"));
}

// Wakeup Signal
VOID Pt_Wakeup_Signal_Int(void)
{
	Ddim_Print(("Wakeup_Signal_Int\n"));
}

// RELC
VOID Pt_Relc_Int(void)
{
	Ddim_Print(("Relc_Int\n"));
}

// NETSEC A
VOID Pt_Netsec_A_Int(void)
{
	Ddim_Print(("Netsec_A_Int\n"));
}

// NETSEC B
VOID Pt_Netsec_B_Int(void)
{
	Ddim_Print(("Netsec_B_Int\n"));
}

// Output Signal7
VOID Pt_Output_Signal7_Int(void)
{
	Ddim_Print(("Output_Signal7_Int\n"));
}



// File End
