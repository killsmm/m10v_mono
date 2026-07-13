/**
 * @file		dd_gic.h
 * @brief		GIC (Generic Interrupt Controller) driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/**
@addtogroup dd_gic
@{
	- @ref dd_gic_sample_section1
	- @ref dd_gic_sample_section2
	- @ref dd_gic_sample_section3
	- @ref dd_gic_sample_section4
*/

#ifndef _DD_GIC_H_
#define _DD_GIC_H_

#include "driver_common.h"
#include "arm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Return value of GIC processing result
#define	D_DD_GIC_INPUT_PARAM_ERR		(D_DD_GIC | D_DDIM_INPUT_PARAM_ERROR)	/**< Input Parameter Error */

/** Interrupt ID and Mask */
#define	D_DD_GIC_INTID_MAX				(320)					/**< Interrupt ID MAX */
#define	D_DD_GIC_INTID_SGI_MAX			(15)					/**< Interrupt ID SGI(Software Generated Interrupt) MAX */
#define	D_DD_GIC_INTID_PPI_MAX			(31)					/**< Interrupt ID PPI(Private Peripheral Interrupt) MAX */
#define	D_DD_GIC_INTID_SPI_MAX			(319)					/**< Interrupt ID SPI(Shared Peripheral Interrupt) MAX */
#define	D_DD_GIC_SPURIOUS_INTERRUPT		(1023)					/**< Spurious Interrupt ID */
#define D_DD_GIC_INTERRUPT_MASK			(0x000003FF)			/**< Interrupt Acknowledge Register MASK */

/** Interrupt Priority Value */
#define	D_DD_GIC_PRI00					(0x00)					/**< Interrupt Priority 00:(0x00) */
#define	D_DD_GIC_PRI01					(0x08)					/**< Interrupt Priority 01:(0x08) */
#define	D_DD_GIC_PRI02					(0x10)					/**< Interrupt Priority 02:(0x10) */
#define	D_DD_GIC_PRI03					(0x18)					/**< Interrupt Priority 03:(0x18) */
#define	D_DD_GIC_PRI04					(0x20)					/**< Interrupt Priority 04:(0x20) */
#define	D_DD_GIC_PRI05					(0x28)					/**< Interrupt Priority 05:(0x28) */
#define	D_DD_GIC_PRI06					(0x30)					/**< Interrupt Priority 06:(0x30) */
#define	D_DD_GIC_PRI07					(0x38)					/**< Interrupt Priority 07:(0x38) */
#define	D_DD_GIC_PRI08					(0x40)					/**< Interrupt Priority 08:(0x40) */
#define	D_DD_GIC_PRI09					(0x48)					/**< Interrupt Priority 09:(0x48) */
#define	D_DD_GIC_PRI10					(0x50)					/**< Interrupt Priority 10:(0x50) */
#define	D_DD_GIC_PRI11					(0x58)					/**< Interrupt Priority 11:(0x58) */
#define	D_DD_GIC_PRI12					(0x60)					/**< Interrupt Priority 12:(0x60) */
#define	D_DD_GIC_PRI13					(0x68)					/**< Interrupt Priority 13:(0x68) */
#define	D_DD_GIC_PRI14					(0x70)					/**< Interrupt Priority 14:(0x70) */
#define	D_DD_GIC_PRI15					(0x78)					/**< Interrupt Priority 15:(0x78) */
#define	D_DD_GIC_PRI16					(0x80)					/**< Interrupt Priority 16:(0x80) */
#define	D_DD_GIC_PRI17					(0x88)					/**< Interrupt Priority 17:(0x88) */
#define	D_DD_GIC_PRI18					(0x90)					/**< Interrupt Priority 18:(0x90) */
#define	D_DD_GIC_PRI19					(0x98)					/**< Interrupt Priority 19:(0x98) */
#define	D_DD_GIC_PRI20					(0xA0)					/**< Interrupt Priority 20:(0xA0) */
#define	D_DD_GIC_PRI21					(0xA8)					/**< Interrupt Priority 21:(0xA8) */
#define	D_DD_GIC_PRI22					(0xB0)					/**< Interrupt Priority 22:(0xB0) */
#define	D_DD_GIC_PRI23					(0xB8)					/**< Interrupt Priority 23:(0xB8) */
#define	D_DD_GIC_PRI24					(0xC0)					/**< Interrupt Priority 24:(0xC0) */
#define	D_DD_GIC_PRI25					(0xC8)					/**< Interrupt Priority 25:(0xC8) */
#define	D_DD_GIC_PRI26					(0xD0)					/**< Interrupt Priority 26:(0xD0) */
#define	D_DD_GIC_PRI27					(0xD8)					/**< Interrupt Priority 27:(0xD8) */
#define	D_DD_GIC_PRI28					(0xE0)					/**< Interrupt Priority 28:(0xE0) */
#define	D_DD_GIC_PRI29					(0xE8)					/**< Interrupt Priority 29:(0xE8) */
#define	D_DD_GIC_PRI30					(0xF0)					/**< Interrupt Priority 30:(0xF0) */
#define	D_DD_GIC_PRI31					(0xF8)					/**< Interrupt Priority 31:(0xF8) */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Interrupt ID */
typedef enum {
	E_DD_GIC_INTID_SGI_INTERRUPT0		= 0,						/**< 000 */
	E_DD_GIC_INTID_SGI_INTERRUPT1,									/**< 001 */
	E_DD_GIC_INTID_SGI_INTERRUPT2,									/**< 002 */
	E_DD_GIC_INTID_SGI_INTERRUPT3,									/**< 003 */
	E_DD_GIC_INTID_SGI_INTERRUPT4,									/**< 004 */
	E_DD_GIC_INTID_SGI_INTERRUPT5,									/**< 005 */
	E_DD_GIC_INTID_SGI_INTERRUPT6,									/**< 006 */
	E_DD_GIC_INTID_SGI_INTERRUPT7,									/**< 007 */
	E_DD_GIC_INTID_SGI_INTERRUPT8,									/**< 008 */
	E_DD_GIC_INTID_SGI_INTERRUPT9,									/**< 009 */
	E_DD_GIC_INTID_SGI_INTERRUPT10,									/**< 010 */
	E_DD_GIC_INTID_SGI_INTERRUPT11,									/**< 011 */
	E_DD_GIC_INTID_SGI_INTERRUPT12,									/**< 012 */
	E_DD_GIC_INTID_SGI_INTERRUPT13,									/**< 013 */
	E_DD_GIC_INTID_SGI_INTERRUPT14,									/**< 014 */
	E_DD_GIC_INTID_SGI_INTERRUPT15,									/**< 015 */
	E_DD_GIC_INTID_PPI_INTERRUPT0,									/**< 016 */
	E_DD_GIC_INTID_PPI_INTERRUPT1,									/**< 017 */
	E_DD_GIC_INTID_PPI_INTERRUPT2,									/**< 018 */
	E_DD_GIC_INTID_PPI_INTERRUPT3,									/**< 019 */
	E_DD_GIC_INTID_PPI_INTERRUPT4,									/**< 020 */
	E_DD_GIC_INTID_PPI_INTERRUPT5,									/**< 021 */
	E_DD_GIC_INTID_PPI_INTERRUPT6,									/**< 022 */
	E_DD_GIC_INTID_PPI_INTERRUPT7,									/**< 023 */
	E_DD_GIC_INTID_PPI_INTERRUPT8,									/**< 024 */
	E_DD_GIC_INTID_PPI_INTERRUPT9,									/**< 025 */
	E_DD_GIC_INTID_PPI_INTERRUPT10,									/**< 026 */
	E_DD_GIC_INTID_PPI_INTERRUPT11,									/**< 027 */
	E_DD_GIC_INTID_PPI_INTERRUPT12,									/**< 028 */
	E_DD_GIC_INTID_PPI_INTERRUPT13,									/**< 029 */
	E_DD_GIC_INTID_PPI_INTERRUPT14,									/**< 030 */
	E_DD_GIC_INTID_PPI_INTERRUPT15,									/**< 031 */
	E_DD_GIC_INTID_CORTEXA7_CPU0_COMMUNICATION_CH_RECEIVE_INT,		/**< 032 */
	E_DD_GIC_INTID_CORTEXA7_CPU0_COMMUNICATION_TRNSMIT_CH_INT,		/**< 033 */
	E_DD_GIC_INTID_CORTEXA7_CPU1_COMMUNICATION_CH_RECEIVE_INT,		/**< 034 */
	E_DD_GIC_INTID_CORTEXA7_CPU1_COMMUNICATION_TRNSMIT_CH_INT,		/**< 035 */
	E_DD_GIC_INTID_CORTEXA7_CPU2_COMMUNICATION_CH_RECEIVE_INT,		/**< 036 */
	E_DD_GIC_INTID_CORTEXA7_CPU2_COMMUNICATION_TRNSMIT_CH_INT,		/**< 037 */
	E_DD_GIC_INTID_CORTEXA7_CPU3_COMMUNICATION_CH_RECEIVE_INT,		/**< 038 */
	E_DD_GIC_INTID_CORTEXA7_CPU3_COMMUNICATION_TRNSMIT_CH_INT,		/**< 039 */
	E_DD_GIC_INTID_CORTEXA7_CPU0_CROSSTRIGGER_INT,					/**< 040 */
	E_DD_GIC_INTID_CORTEXA7_CPU1_CROSSTRIGGER_INT,					/**< 041 */
	E_DD_GIC_INTID_CORTEXA7_CPU2_CROSSTRIGGER_INT,					/**< 042 */
	E_DD_GIC_INTID_CORTEXA7_CPU3_CROSSTRIGGER_INT,					/**< 043 */
	E_DD_GIC_INTID_CORTEXA7_CPU0_SYSTEM_MATRICS_INT,				/**< 044 */
	E_DD_GIC_INTID_CORTEXA7_CPU1_SYSTEM_MATRICS_INT,				/**< 045 */
	E_DD_GIC_INTID_CORTEXA7_CPU2_SYSTEM_MATRICS_INT,				/**< 046 */
	E_DD_GIC_INTID_CORTEXA7_CPU3_SYSTEM_MATRICS_INT,				/**< 047 */
	E_DD_GIC_INTID_AXI_TRANSACTION_ERROR_INT,						/**< 048 */
	E_DD_GIC_INTID_SNAP_PERI_XDMAC_CH0_INT,							/**< 049 */
	E_DD_GIC_INTID_SNAP_PERI_XDMAC_CH1_INT,							/**< 050 */
	E_DD_GIC_INTID_SNAP_PERI_XDMAC_CH2_INT,							/**< 051 */
	E_DD_GIC_INTID_SNAP_PERI_XDMAC_CH3_INT,							/**< 052 */
	E_DD_GIC_INTID_WATCHDOG_TIMER_INT,								/**< 053 */
	E_DD_GIC_INTID_IPCU0_CH0_INT,									/**< 054 */
	E_DD_GIC_INTID_IPCU0_CH1_INT,									/**< 055 */
	E_DD_GIC_INTID_IPCU0_CH2_INT,									/**< 056 */
	E_DD_GIC_INTID_IPCU0_CH3_INT,									/**< 057 */
	E_DD_GIC_INTID_IPCU0_CH4_INT,									/**< 058 */
	E_DD_GIC_INTID_IPCU0_CH5_INT,									/**< 059 */
	E_DD_GIC_INTID_IPCU0_CH6_INT,									/**< 060 */
	E_DD_GIC_INTID_IPCU0_CH7_INT,									/**< 061 */
	E_DD_GIC_INTID_IPCU0_CH8_INT,									/**< 062 */
	E_DD_GIC_INTID_IPCU0_CH9_INT,									/**< 063 */
	E_DD_GIC_INTID_IPCU0_CH10_INT,									/**< 064 */
	E_DD_GIC_INTID_IPCU0_CH11_INT,									/**< 065 */
	E_DD_GIC_INTID_IPCU0_CH12_INT,									/**< 066 */
	E_DD_GIC_INTID_IPCU0_CH13_INT,									/**< 067 */
	E_DD_GIC_INTID_IPCU0_CH14_INT,									/**< 068 */
	E_DD_GIC_INTID_IPCU0_CH15_INT,									/**< 069 */
	E_DD_GIC_INTID_IPCU1_CH0_INT,									/**< 070 */
	E_DD_GIC_INTID_IPCU1_CH1_INT,									/**< 071 */
	E_DD_GIC_INTID_IPCU1_CH2_INT,									/**< 072 */
	E_DD_GIC_INTID_IPCU1_CH3_INT,									/**< 073 */
	E_DD_GIC_INTID_IPCU1_CH4_INT,									/**< 074 */
	E_DD_GIC_INTID_IPCU1_CH5_INT,									/**< 075 */
	E_DD_GIC_INTID_IPCU1_CH6_INT,									/**< 076 */
	E_DD_GIC_INTID_IPCU1_CH7_INT,									/**< 077 */
	E_DD_GIC_INTID_IPCU1_CH8_INT,									/**< 078 */
	E_DD_GIC_INTID_IPCU1_CH9_INT,									/**< 079 */
	E_DD_GIC_INTID_IPCU1_CH10_INT,									/**< 080 */
	E_DD_GIC_INTID_IPCU1_CH11_INT,									/**< 081 */
	E_DD_GIC_INTID_IPCU1_CH12_INT,									/**< 082 */
	E_DD_GIC_INTID_IPCU1_CH13_INT,									/**< 083 */
	E_DD_GIC_INTID_IPCU1_CH14_INT,									/**< 084 */
	E_DD_GIC_INTID_IPCU1_CH15_INT,									/**< 085 */
	E_DD_GIC_INTID_EXTERNAL_CH16_INT,								/**< 086 */
	E_DD_GIC_INTID_EXTERNAL_CH17_INT,								/**< 087 */
	E_DD_GIC_INTID_EXTERNAL_CH18_INT,								/**< 088 */
	E_DD_GIC_INTID_EXTERNAL_CH19_INT,								/**< 089 */
	E_DD_GIC_INTID_EXTERNAL_CH20_INT,								/**< 090 */
	E_DD_GIC_INTID_EXTERNAL_CH21_INT,								/**< 091 */
	E_DD_GIC_INTID_EXTERNAL_CH22_INT,								/**< 092 */
	E_DD_GIC_INTID_EXTERNAL_CH23_INT,								/**< 093 */
	E_DD_GIC_INTID_EXTERNAL_CH24_INT,								/**< 094 */
	E_DD_GIC_INTID_EXTERNAL_CH25_INT,								/**< 095 */
	E_DD_GIC_INTID_EXTERNAL_CH26_INT,								/**< 096 */
	E_DD_GIC_INTID_EXTERNAL_CH27_INT,								/**< 097 */
	E_DD_GIC_INTID_EXTERNAL_CH28_INT,								/**< 098 */
	E_DD_GIC_INTID_EXTERNAL_CH29_INT,								/**< 099 */
	E_DD_GIC_INTID_EXTERNAL_CH30_INT,								/**< 100 */
	E_DD_GIC_INTID_EXTERNAL_CH31_INT,								/**< 101 */
	E_DD_GIC_INTID_PMC_DMAC_ADC_INT,								/**< 102 */
	E_DD_GIC_INTID_PMC_DMAC_RECEIVE_INT,							/**< 103 */
	E_DD_GIC_INTID_PMC_UART_TRANSMISSION_DATA_INT,					/**< 104 */
	E_DD_GIC_INTID_PMC_DMAC_QSPI_TRANSMISSION_INT,					/**< 105 */
	E_DD_GIC_INTID_PMC_DMAC_QSPI_RECEIVE_INT,						/**< 106 */
	E_DD_GIC_INTID_PMC_UART_RECEIVE_RELATION_INT,					/**< 107 */
	E_DD_GIC_INTID_PMC_UART_TRANSMISSION_RELATION_INT,				/**< 108 */
	E_DD_GIC_INTID_PMC_QSPI_INT,									/**< 109 */
	E_DD_GIC_INTID_CALENDER_ALARM_INT,								/**< 110 */
	E_DD_GIC_INTID_PMC_32BIT_TIMER_UNDER_FLOW_INT,					/**< 111 */
	E_DD_GIC_INTID_ADC_INT,											/**< 112 */
	E_DD_GIC_INTID_PMC_I2C_INT,										/**< 113 */
	E_DD_GIC_INTID_PMC_WATCHDOG_INT,								/**< 114 */
	E_DD_GIC_INTID_TEMPERATURE_INT,									/**< 115 */
	E_DD_GIC_INTID_PMU_INT,											/**< 116 */
	E_DD_GIC_INTID_PMC_GPIO_INT,									/**< 117 */
	E_DD_GIC_INTID_TIMER_CH0_INT,									/**< 118 */
	E_DD_GIC_INTID_TIMER_CH1_INT,									/**< 119 */
	E_DD_GIC_INTID_TIMER_CH2_INT,									/**< 120 */
	E_DD_GIC_INTID_TIMER_CH3_INT,									/**< 121 */
	E_DD_GIC_INTID_TIMER_CH4_INT,									/**< 122 */
	E_DD_GIC_INTID_TIMER_CH5_INT,									/**< 123 */
	E_DD_GIC_INTID_TIMER_CH6_INT,									/**< 124 */
	E_DD_GIC_INTID_TIMER_CH7_INT,									/**< 125 */
	E_DD_GIC_INTID_TIMER_CH8_INT,									/**< 126 */
	E_DD_GIC_INTID_TIMER_CH9_INT,									/**< 127 */
	E_DD_GIC_INTID_TIMER_CH10_INT,									/**< 128 */
	E_DD_GIC_INTID_TIMER_CH11_INT,									/**< 129 */
	E_DD_GIC_INTID_TIMER_CH12_INT,									/**< 130 */
	E_DD_GIC_INTID_TIMER_CH13_INT,									/**< 131 */
	E_DD_GIC_INTID_NULL132,											/**< 132 */
	E_DD_GIC_INTID_I2C_CH0_INT,										/**< 133 */
	E_DD_GIC_INTID_I2C_CH1_INT,										/**< 134 */
	E_DD_GIC_INTID_I2C_CH2_INT,										/**< 135 */
	E_DD_GIC_INTID_UPDOWN_COUNTER_CH0_INT,							/**< 136 */
	E_DD_GIC_INTID_UPDOWN_COUNTER_CH1_INT,							/**< 137 */
	E_DD_GIC_INTID_UPDOWN_COUNTER_CH2_INT,							/**< 138 */
	E_DD_GIC_INTID_UPDOWN_COUNTER_CH3_INT,							/**< 139 */
	E_DD_GIC_INTID_UPDOWN_COUNTER_CH4_INT,							/**< 140 */
	E_DD_GIC_INTID_UPDOWN_COUNTER_CH5_INT,							/**< 141 */
	E_DD_GIC_INTID_PWM_CH0_CH3_INT,									/**< 142 */
	E_DD_GIC_INTID_PWM_CH4_CH7_INT,									/**< 143 */
	E_DD_GIC_INTID_PWM_CH8_CH11_INT,								/**< 144 */
	E_DD_GIC_INTID_PWM_CH12_CH15_INT,								/**< 145 */
	E_DD_GIC_INTID_PWM_CH0_CH3_STP_INT,								/**< 146 */
	E_DD_GIC_INTID_PWM_CH4_CH7_STP_INT,								/**< 147 */
	E_DD_GIC_INTID_PWM_CH8_CH11_STP_INT,							/**< 148 */
	E_DD_GIC_INTID_PWM_CH12_CH15_STP_INT,							/**< 149 */
	E_DD_GIC_INTID_AUDIO_IF_CH0_INT,								/**< 150 */
	E_DD_GIC_INTID_AUDIO_IF_CH1_INT,								/**< 151 */
	E_DD_GIC_INTID_AUDIO_IF_CH2_INT,								/**< 152 */
	E_DD_GIC_INTID_AUDIO_IF_CH3_INT,								/**< 153 */
	E_DD_GIC_INTID_AUDIO_IF_CH4_INT,								/**< 154 */
	E_DD_GIC_INTID_AUDIO_IF_CH5_INT,								/**< 155 */
	E_DD_GIC_INTID_HDMAC0_CH0_INT,									/**< 156 */
	E_DD_GIC_INTID_HDMAC0_CH1_INT,									/**< 157 */
	E_DD_GIC_INTID_HDMAC0_CH2_INT,									/**< 158 */
	E_DD_GIC_INTID_HDMAC0_CH3_INT,									/**< 159 */
	E_DD_GIC_INTID_HDMAC0_CH4_INT,									/**< 160 */
	E_DD_GIC_INTID_HDMAC0_CH5_INT,									/**< 161 */
	E_DD_GIC_INTID_HDMAC0_CH6_INT,									/**< 162 */
	E_DD_GIC_INTID_HDMAC0_CH7_INT,									/**< 163 */
	E_DD_GIC_INTID_HDMAC1_CH0_INT,									/**< 164 */
	E_DD_GIC_INTID_HDMAC1_CH1_INT,									/**< 165 */
	E_DD_GIC_INTID_HDMAC1_CH2_INT,									/**< 166 */
	E_DD_GIC_INTID_HDMAC1_CH3_INT,									/**< 167 */
	E_DD_GIC_INTID_HDMAC1_CH4_INT,									/**< 168 */
	E_DD_GIC_INTID_HDMAC1_CH5_INT,									/**< 169 */
	E_DD_GIC_INTID_HDMAC1_CH6_INT,									/**< 170 */
	E_DD_GIC_INTID_HDMAC1_CH7_INT,									/**< 171 */
	E_DD_GIC_INTID_UART_CH0_RECEPTION_INT,							/**< 172 */
	E_DD_GIC_INTID_UART_CH1_RECEPTION_INT,							/**< 173 */
	E_DD_GIC_INTID_UART_CH2_RECEPTION_INT,							/**< 174 */
	E_DD_GIC_INTID_UART_CH3_RECEPTION_INT,							/**< 175 */
	E_DD_GIC_INTID_UART_CH4_RECEPTION_INT,							/**< 176 */
	E_DD_GIC_INTID_UART_CH5_RECEPTION_INT,							/**< 177 */
	E_DD_GIC_INTID_UART_CH6_RECEPTION_INT,							/**< 178 */
	E_DD_GIC_INTID_UART_CH7_RECEPTION_INT,							/**< 179 */
	E_DD_GIC_INTID_UART_CH0_TRANSMISSION_INT,						/**< 180 */
	E_DD_GIC_INTID_UART_CH1_TRANSMISSION_INT,						/**< 181 */
	E_DD_GIC_INTID_UART_CH2_TRANSMISSION_INT,						/**< 182 */
	E_DD_GIC_INTID_UART_CH3_TRANSMISSION_INT,						/**< 183 */
	E_DD_GIC_INTID_UART_CH4_TRANSMISSION_INT,						/**< 184 */
	E_DD_GIC_INTID_UART_CH5_TRANSMISSION_INT,						/**< 185 */
	E_DD_GIC_INTID_UART_CH6_TRANSMISSION_INT,						/**< 186 */
	E_DD_GIC_INTID_UART_CH7_TRANSMISSION_INT,						/**< 187 */
	E_DD_GIC_INTID_SPI_CH0_INT,										/**< 188 */
	E_DD_GIC_INTID_SPI_CH1_INT,										/**< 189 */
	E_DD_GIC_INTID_SPI_CH2_INT,										/**< 190 */
	E_DD_GIC_INTID_PDM_CH0_DMA0_INT,								/**< 191 */
	E_DD_GIC_INTID_PDM_CH0_DMA1_INT,								/**< 192 */
	E_DD_GIC_INTID_PDM_CH0_FIFO_OVERFLOW_INT,						/**< 193 */
	E_DD_GIC_INTID_PDM_CH1_DMA0_INT,								/**< 194 */
	E_DD_GIC_INTID_PDM_CH1_DMA1_INT,								/**< 195 */
	E_DD_GIC_INTID_PDM_CH1_FIFO_OVERFLOW_INT,						/**< 196 */
	E_DD_GIC_INTID_SLIMBUS_CH0_INT,									/**< 197 */
	E_DD_GIC_INTID_SLIMBUS_CH1_INT,									/**< 198 */
	E_DD_GIC_INTID_SLIMBUS_CH2_INT,									/**< 199 */
	E_DD_GIC_INTID_SLIMBUS_CH3_INT,									/**< 200 */
	E_DD_GIC_INTID_SDRAMC_CH0_INT,									/**< 201 */
	E_DD_GIC_INTID_SDRAMC_CH1_INT,									/**< 202 */
	E_DD_GIC_INTID_MXIC_UNIT0_INT,									/**< 203 */
	E_DD_GIC_INTID_MXIC_UNIT1_INT,									/**< 204 */
	E_DD_GIC_INTID_MXIC_UNIT2_INT,									/**< 205 */
	E_DD_GIC_INTID_MXIC_UNIT3_INT,									/**< 206 */
	E_DD_GIC_INTID_MXIC_UNIT4_INT,									/**< 207 */
	E_DD_GIC_INTID_MXIC_UNIT5_INT,									/**< 208 */
	E_DD_GIC_INTID_MXIC_UNIT6_INT,									/**< 209 */
	E_DD_GIC_INTID_SEN_INT,											/**< 210 */
	E_DD_GIC_INTID_SEN_VD_INT,										/**< 211 */
	E_DD_GIC_INTID_SEN_HD_INT,										/**< 212 */
	E_DD_GIC_INTID_SENSOR_IF_MACRO0_INT,							/**< 213 */
	E_DD_GIC_INTID_SENSOR_IF_MACRO1_INT,							/**< 214 */
	E_DD_GIC_INTID_SENSOR_IF_MACRO2_INT,							/**< 215 */
	E_DD_GIC_INTID_SENSOR_IF_MACRO3_INT,							/**< 216 */
	E_DD_GIC_INTID_SRO_UNIT1_INT,									/**< 217 */
	E_DD_GIC_INTID_SRO_UNIT1_VD_INT,								/**< 218 */
	E_DD_GIC_INTID_SRO_UNIT1_HD_INT,								/**< 219 */
	E_DD_GIC_INTID_SRO_UNIT2_INT,									/**< 220 */
	E_DD_GIC_INTID_SRO_UNIT2_VD_INT,								/**< 221 */
	E_DD_GIC_INTID_SRO_UNIT2_HD_INT,								/**< 222 */
	E_DD_GIC_INTID_B2B_UNIT1_INT,									/**< 223 */
	E_DD_GIC_INTID_B2B_UNIT1_VD_INT,								/**< 224 */
	E_DD_GIC_INTID_B2B_UNIT1_HD_INT,								/**< 225 */
	E_DD_GIC_INTID_B2B_UNIT2_INT,									/**< 226 */
	E_DD_GIC_INTID_B2B_UNIT2_VD_INT,								/**< 227 */
	E_DD_GIC_INTID_B2B_UNIT2_HD_INT,								/**< 228 */
	E_DD_GIC_INTID_START_INT,										/**< 229 */
	E_DD_GIC_INTID_PAS_UNIT2_INT,									/**< 230 */
	E_DD_GIC_INTID_PAS_UNIT2_VD_INT,								/**< 231 */
	E_DD_GIC_INTID_PAS_UNIT2_HD_INT,								/**< 232 */
	E_DD_GIC_INTID_B2R_UNIT1_INT,									/**< 233 */
	E_DD_GIC_INTID_B2R_UNIT2_INT,									/**< 234 */
	E_DD_GIC_INTID_LTM_UNIT1_INT,									/**< 235 */
	E_DD_GIC_INTID_LTM_UNIT2_INT,									/**< 236 */
	E_DD_GIC_INTID_R2Y_UNIT1_INT,									/**< 237 */
	E_DD_GIC_INTID_R2Y_UNIT2_INT,									/**< 238 */
	E_DD_GIC_INTID_CNR_UNIT1_INT,									/**< 239 */
	E_DD_GIC_INTID_CNR_UNIT2_INT,									/**< 240 */
	E_DD_GIC_INTID_IIP_INT,											/**< 241 */
	E_DD_GIC_INTID_SHDR_INT,										/**< 242 */
	E_DD_GIC_INTID_SHDR_LINE_INT,									/**< 243 */
	E_DD_GIC_INTID_ME_INT,											/**< 244 */
	E_DD_GIC_INTID_GRAPHICS_INT,									/**< 245 */
	E_DD_GIC_INTID_JPEG_INT,										/**< 246 */
	E_DD_GIC_INTID_RAW_INT,											/**< 247 */
	E_DD_GIC_INTID_IPU_INT,											/**< 248 */
	E_DD_GIC_INTID_ELA_UNIT_INT,									/**< 249 */
	E_DD_GIC_INTID_XCH_INT,											/**< 250 */
	E_DD_GIC_INTID_FPT_UNIT0_INT,									/**< 251 */
	E_DD_GIC_INTID_FPT_UNIT1_INT,									/**< 252 */
	E_DD_GIC_INTID_DISP_LCD_INT,									/**< 253 */
	E_DD_GIC_INTID_DISP_HDMI_INT,									/**< 254 */
	E_DD_GIC_INTID_DISP_MIPI_DSI_INT,								/**< 255 */
	E_DD_GIC_INTID_MIPI_DSI_Tx_INT,									/**< 256 */
	E_DD_GIC_INTID_DISP_DWCH_INT,									/**< 257 */
	E_DD_GIC_INTID_RDMA_INT,										/**< 258 */
	E_DD_GIC_INTID_HDMI_MASTER_INT,									/**< 259 */
	E_DD_GIC_INTID_HDMI_CEC_INT,									/**< 260 */
	E_DD_GIC_INTID_BMH_UNIT0_INT,									/**< 261 */
	E_DD_GIC_INTID_BMH_UNIT1_INT,									/**< 262 */
	E_DD_GIC_INTID_RIBERY_RBR_INT,									/**< 263 */
	E_DD_GIC_INTID_RIBERY_RWDT_INT,									/**< 264 */
	E_DD_GIC_INTID_INTERPROCESSOR0_INT,								/**< 265 */
	E_DD_GIC_INTID_INTERPROCESSOR1_INT,								/**< 266 */
	E_DD_GIC_INTID_INTERPROCESSOR2_INT,								/**< 267 */
	E_DD_GIC_INTID_INTERPROCESSOR3_INT,								/**< 268 */
	E_DD_GIC_INTID_DSP_PERIPHERAL_UNIT_IPCU0_INT,					/**< 269 */
	E_DD_GIC_INTID_DSP_PERIPHERAL_UNIT_IPCU1_INT,					/**< 270 */
	E_DD_GIC_INTID_DSP_PERIPHERAL_UNIT_IPCU2_INT,					/**< 271 */
	E_DD_GIC_INTID_DSP_PERIPHERAL_UNIT_IPCU3_INT,					/**< 272 */
	E_DD_GIC_INTID_DSP_DEBUG_INT,									/**< 273 */
	E_DD_GIC_INTID_NANDFLASH_INT,									/**< 274 */
	E_DD_GIC_INTID_PCIE_CH0_MSI_INT,								/**< 275 */
	E_DD_GIC_INTID_PCIE_CH0_SYSTEM_ERROR_INT,						/**< 276 */
	E_DD_GIC_INTID_PCIE_CH0_INTX_INT,								/**< 277 */
	E_DD_GIC_INTID_PCIE_CH0_CORE_INT,								/**< 278 */
	E_DD_GIC_INTID_PCIE_CH0_DMA_INT,								/**< 279 */
	E_DD_GIC_INTID_PCIE_CH1_MSI_INT,								/**< 280 */
	E_DD_GIC_INTID_PCIE_CH1_SYSTEM_ERROR_INT,						/**< 281 */
	E_DD_GIC_INTID_PCIE_CH1_INTX_INT,								/**< 282 */
	E_DD_GIC_INTID_PCIE_CH1_CORE_INT,								/**< 283 */
	E_DD_GIC_INTID_PCIE_CH1_DMA_INT,								/**< 284 */
	E_DD_GIC_INTID_EXS_XDMAC_CH0_INT,								/**< 285 */
	E_DD_GIC_INTID_EXS_XDMAC_CH1_INT,								/**< 286 */
	E_DD_GIC_INTID_EXS_XDMAC_CH2_INT,								/**< 287 */
	E_DD_GIC_INTID_EXS_XDMAC_CH3_INT,								/**< 288 */
	E_DD_GIC_INTID_EXS_HDMAC_CH0_INT,								/**< 289 */
	E_DD_GIC_INTID_EXS_HDMAC_CH1_INT,								/**< 290 */
	E_DD_GIC_INTID_EXS_HDMAC_CH2_INT,								/**< 291 */
	E_DD_GIC_INTID_EXS_HDMAC_CH3_INT,								/**< 292 */
	E_DD_GIC_INTID_USB20_HDC_INT,									/**< 293 */
	E_DD_GIC_INTID_USB20_HOST_EHCI_INT,								/**< 294 */
	E_DD_GIC_INTID_USB20_HOST_OHCI_INT,								/**< 295 */
	E_DD_GIC_INTID_USB30_INT,										/**< 296 */
	E_DD_GIC_INTID_SD_CH3_INT,										/**< 297 */
	E_DD_GIC_INTID_SD_CH3_SDIO_INT,									/**< 298 */
	E_DD_GIC_INTID_SD_CH3_WAKEUP_INT,								/**< 299 */
	E_DD_GIC_INTID_SD_CH2_UHS1_INT,									/**< 300 */
	E_DD_GIC_INTID_SD_CH2_SDIO_INT,									/**< 301 */
	E_DD_GIC_INTID_SD_CH2_UHS1_WAKEUP_INT,							/**< 302 */
	E_DD_GIC_INTID_SD_CH2_UHS2_INT,									/**< 303 */
	E_DD_GIC_INTID_SD_CH2_UHS2_WAKEUP_INT,							/**< 304 */
	E_DD_GIC_INTID_SD_CH1_INT,										/**< 305 */
	E_DD_GIC_INTID_SD_CH1_SDIO_INT,									/**< 306 */
	E_DD_GIC_INTID_SD_CH1_WAKEUP_INT,								/**< 307 */
	E_DD_GIC_INTID_EMMC_INT,										/**< 308 */
	E_DD_GIC_INTID_EMMC_WAKEUP_INT,									/**< 309 */
	E_DD_GIC_INTID_RELC_INT,										/**< 310 */
	E_DD_GIC_INTID_NETSEC_A_INT,									/**< 311 */
	E_DD_GIC_INTID_NETSEC_B_INT,									/**< 312 */
	E_DD_GIC_INTID_SD_PCIE_DETECT_INT,								/**< 313 */
	E_DD_GIC_INTID_NULL314,											/**< 314 */
	E_DD_GIC_INTID_NULL315,											/**< 315 */
	E_DD_GIC_INTID_NULL316,											/**< 316 */
	E_DD_GIC_INTID_NULL317,											/**< 317 */
	E_DD_GIC_INTID_NULL318,											/**< 318 */
	E_DD_GIC_INTID_NULL319											/**< 319 */
} E_DD_GIC_INTID;

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
/* Distributor Register */

// Distributor Control Register(GICD_CTLR)
#define Dd_GIC_DIST_Get_Ctlr_EnableGrp0					(IO_GIC_DIST.GICD_CTLR.bit.ENABLEGR0)				/**< Get GICD_CTLR EnableGrp0(0:Disable, 1:Enable) */
#define Dd_GIC_DIST_Set_Ctlr_EnableGrp0(val)			{IO_GIC_DIST.GICD_CTLR.bit.ENABLEGR0 = (val);}		/**< Set GICD_CTLR EnableGrp0(0:Disable, 1:Enable) */
#define Dd_GIC_DIST_Get_Ctlr_EnableGrp1					(IO_GIC_DIST.GICD_CTLR.bit.ENABLEGR1)				/**< Get GICD_CTLR EnableGrp1(0:Disable, 1:Enable) */
#define Dd_GIC_DIST_Set_Ctlr_EnableGrp1(val)			{IO_GIC_DIST.GICD_CTLR.bit.ENABLEGR1 = (val);}		/**< Set GICD_CTLR EnableGrp1(0:Disable, 1:Enable) */

// Interrupt Controller Type Register(GICD_TYPER)
#define Dd_GIC_DIST_Get_Typer_ItLinesNumber				(IO_GIC_DIST.GICD_TYPER.bit.ITLINESNUMBER)			/**< Get GICD_TYPER Number of Interrupts<br>
																													0b00000 : Up to 32 interruptsa, no external interrupt lines<br>
																													0b00001 : Up to 64 interrupts, 32 external interrupt lines<br>
																													0b00010 : Up to 96 interrupts, 64 external interrupt lines<br>
																													.<br>
																													.<br>
																													0b01111 Up to 512 interrupts, 480 external interrupt lines<br>
																											*/
#define Dd_GIC_DIST_Get_Typer_CpuNumber					(IO_GIC_DIST.GICD_TYPER.bit.CPUNUMBER)				/**< Get GICD_TYPER Number of Implemented Processors(1:Two CPUs)<br>
																													0b000 : One processor<br>
																													0b001 : Two processor<br>
																													0b010 : Three processor<br>
																													0b011 : Four processors<br>
																											*/
#define Dd_GIC_DIST_Get_Typer_SecurityExtn				(IO_GIC_DIST.GICD_TYPER.bit.SECURITY_EXTN)			/**< Get GICD_TYPER Security Extensions<br>
																													1 : Security Extensions<br>
																											*/
#define Dd_GIC_DIST_Get_Typer_Lspi						(IO_GIC_DIST.GICD_TYPER.bit.LSPI)					/**< Get GICD_TYPER Lockable Shared Peripheral Interrupts<br>
																													0b11111 : 31 LSPIs<br>
																											*/

// Distributor Implementer Identification Register(GICD_IIDR)
#define Dd_GIC_DIST_Get_IIdr_Implementer				(IO_GIC_DIST.GICD_IIDR.bit.IMPLEMENTER)				/**< Get GICD_IIDR Implementer(0x43B:ARM implementation) */
#define Dd_GIC_DIST_Get_IIdr_Revision					(IO_GIC_DIST.GICD_IIDR.bit.REVISION)				/**< Get GICD_IIDR Minor Revision Number(0x1:Revision number) */
#define Dd_GIC_DIST_Get_IIdr_Variant					(IO_GIC_DIST.GICD_IIDR.bit.VARIANT)					/**< Get GICD_IIDR Major Revision Number(0x0:Variant number) */
#define Dd_GIC_DIST_Get_IIdr_ProductId					(IO_GIC_DIST.GICD_IIDR.bit.PRODUCTID)				/**< Get GICD_IIDR ProductID(0x01:Cortex-A7) */

// Interrupt Group Register(GICD_IGROUPR)
#define Dd_GIC_DIST_Get_IGroupr(uni)					(IO_GIC_DIST.GICD_IGROUPR[uni])						/**< Get GICD_IGROUPR Group Status bit(uni:0 to 31)<br>
																													Bit Field Value<br>
																													0 : Group0<br>
																													1 : Group1<br>
																													INTID<br>
																													INTID = ((uni*32) + Bit position)<br>
																											*/
#define Dd_GIC_DIST_Set_IGroupr(uni,val)				{IO_GIC_DIST.GICD_IGROUPR[uni] = (val);}			/**< Set GICD_IGROUPR Group Status bit(uni:0 to 31)<br>
																													Bit Field Value<br>
																													0 : Group0<br>
																													1 : Group1<br>
																													INTID<br>
																													INTID = ((uni*32) + Bit position)<br>
																											*/

// Interrupt Set-Enable Register(GICD_ISENABLER)
#define Dd_GIC_DIST_Get_ISEnabler(uni)					(IO_GIC_DIST.GICD_ISENABLER[uni])					/**< Get GICD_ISENABLER Set Enable bit(uni:0 to 31)<br>
																													Bit Field Value<br>
																													0 : Interrupt Disable<br>
																													1 : Interrupt Enaable<br>
																													INTID<br>
																													INTID = ((uni*32) + Bit position)<br>
																											*/
#define Dd_GIC_DIST_Set_ISEnabler(uni,val)				{IO_GIC_DIST.GICD_ISENABLER[uni] = (val);}			/**< Set GICD_ISENABLER Set Enable bit(uni:0 to 31)<br>
																													Bit Field Value(val)<br>
																													0 : Has no effect<br>
																													1 : Interrupt Enaable<br>
																													INTID<br>
																													INTID = ((uni*32) + Bit position)<br>
																											*/

// Interrupt Clear-Enable Register(GICD_ICENABLER)
#define Dd_GIC_DIST_Get_ICEnabler(uni)					(IO_GIC_DIST.GICD_ICENABLER[uni])					/**< Get GICD_ICENABLER Clear Enable bit(uni:0 to 31)<br>
																													Bit Field Value<br>
																													0 : Interrupt Disable<br>
																													1 : Interrupt Enaable<br>
																													INTID<br>
																													INTID = ((uni*32) + Bit position)<br>
																											*/
#define Dd_GIC_DIST_Set_ICEnabler(uni,val)				{IO_GIC_DIST.GICD_ICENABLER[uni] = (val);}			/**< Set GICD_ICENABLER Clear Enable bit(uni:0 to 31)<br>
																													Bit Field Value<br>
																													0 : Has no effect<br>
																													1 : Interrupt Disable<br>
																													INTID<br>
																													INTID = ((uni*32) + Bit position)<br>
																											*/

// Interrupt Set-Pending Register(GICD_ISPENDR)
#define Dd_GIC_DIST_Get_ISPendr(uni)					(IO_GIC_DIST.GICD_ISPENDR[uni])						/**< Get GICD_ISPENDR Set-Pending bit(uni:0 to 31)<br>
																													Bit Field Value<br>
																													0 : Not pending<br>
																													1 : SGI and PPI, Pending on this processor<br>
																														PPI, Pending on at least one processer<br>
																													INTID<br>
																													INTID = ((uni*32) + Bit position)<br>
																											*/
#define Dd_GIC_DIST_Set_ISPendr(uni,val)				{IO_GIC_DIST.GICD_ISPENDR[uni] = (val);}			/**< Set GICD_ISPENDR Set-Pending bit(uni:0 to 31)<br>
																													Bit Field Value<br>
																													0 : Has no effect<br>
																													1 : Interrupt is pending<br>
																													INTID<br>
																													INTID = ((uni*32) + Bit position)<br>
																											*/

// Interrupt Clear-Pending Register(GICD_ICPENDR)
#define Dd_GIC_DIST_Get_ICPendr(uni)					(IO_GIC_DIST.GICD_ICPENDR[uni])						/**< Get GICD_ICPENDR Clear-Pending bit(uni:0 to 31)<br>
																													Bit Field Value<br>
																													0 : Not pending<br>
																													1 : SGI and PPI, Pending on this processor<br>
																														PPI, Pending on at least one processer<br>
																													INTID<br>
																													INTID = ((uni*32) + Bit position)<br>
																											*/
#define Dd_GIC_DIST_Set_ICPendr(uni,val)				{IO_GIC_DIST.GICD_ICPENDR[uni] = (val);}			/**< Set GICD_ICPENDR Clear-Pending bit(uni:0 to 7)<br>
																													Bit Field Value<br>
																													0 : Has no effect<br>
																													1 : Interrupt is not pending<br>
																													INTID<br>
																													INTID = ((uni*32) + Bit position)<br>
																											*/

// Interrupt Set-Active Register(GICD_ISACTIVER)
#define Dd_GIC_DIST_Get_ISActiver(uni)					(IO_GIC_DIST.GICD_ISACTIVER[uni])					/**< Get GICD_ISACTIVER Set-Active bit(uni:0 to 31)<br>
																													Bit Field Value<br>
																													0 : Interrupt is not active<br>
																													1 : Interrupt is active<br>
																													INTID<br>
																													INTID = ((uni*32) + Bit position)<br>
																											*/
#define Dd_GIC_DIST_Set_ISActiver(uni,val)				{IO_GIC_DIST.GICD_ISACTIVER[uni] = (val);}			/**< Set GICD_ISACTIVER Set-Active bit(uni:0 to 31)<br>
																													Bit Field Value<br>
																													0 : Has no effect<br>
																													1 : Activates the interrupt<br>
																													INTID<br>
																													INTID = ((uni*32) + Bit position)<br>
																											*/

// Interrupt Clear-Active Register(GICD_ICACTIVER)
#define Dd_GIC_DIST_Get_ICActiver(uni)					(IO_GIC_DIST.GICD_ICACTIVER[uni])					/**< Get GICD_ICACTIVER Clear-Active bit(uni:0 to 31)<br>
																													Bit Field Value<br>
																													0 : Interrupt is not active<br>
																													1 : Interrupt is active<br>
																													INTID<br>
																													INTID = ((uni*32) + Bit position)<br>
																											*/
#define Dd_GIC_DIST_Set_ICActiver(uni,val)				{IO_GIC_DIST.GICD_ICACTIVER[uni] = (val);}			/**< Set GICD_ICACTIVER Clear-Active bit(uni:0 to 31)<br>
																													Bit Field Value<br>
																													0 : Has no effect<br>
																													1 : Deactivates the interrupt<br>
																													INTID<br>
																													INTID = ((uni*32) + Bit position)<br>
																											*/

// Interrupt Priority Register(GICD_IPRIORITYR)
#define Dd_GIC_DIST_Get_IPriorityr(uni)					(IO_GIC_DIST.GICD_IPRIORITYR[uni])					/**< Get GICD_IPRIORITYR Priority byte(uni:0 to 511)<br>
																													Byte Field Value<br>
																													Range of priority mask<br>
																													32 supported levels : 0x00 to 0xF8<br>
																													INTID<br>
																													INTID = uni<br>
																											*/
#define Dd_GIC_DIST_Set_IPriorityr(uni,val)				{IO_GIC_DIST.GICD_IPRIORITYR[uni] = (val);}			/**< Set GICD_IPRIORITYR Priority byte(uni:0 to 511)<br>
																													Byte Field Value<br>
																													Range of priority mask<br>
																													32 supported levels : 0x00 to 0xF8<br>
																													INTID<br>
																													INTID = uni<br>
																											*/

// Interrupt Processor Targets Register(GICD_ITARGETSR)
#define Dd_GIC_DIST_Get_ITargetsr(uni)					(IO_GIC_DIST.GICD_ITARGETSR[uni])					/**< Get GICD_ITARGETSR CPU target byte(uni:0 to 511)<br>
																													Byte Field Value<br>
																													0bxxxxxxx1 : CPU interface 0<br>
																													0bxxxxxx1x : CPU interface 1<br>
																													0bxxxxx1xx : CPU interface 2<br>
																													.<br>
																													.<br>
																													0b1xxxxxxx : CPU interface 7<br>
																													INTID<br>
																													INTID = uni<br>
																											*/
#define Dd_GIC_DIST_Set_ITargetsr(uni,val)				{IO_GIC_DIST.GICD_ITARGETSR[uni] = (val);}			/**< Set GICD_ITARGETSR CPU target byte(uni:0 to 511)<br>
																													Byte Field Value<br>
																													0bxxxxxxx1 : CPU interface 0<br>
																													0bxxxxxx1x : CPU interface 1<br>
																													0bxxxxx1xx : CPU interface 2<br>
																													.<br>
																													.<br>
																													0b1xxxxxxx : CPU interface 7<br>
																													INTID<br>
																													INTID = uni<br>
																											*/

// Interrupt Configuration Register(GICD_ICFGR)
#define Dd_GIC_DIST_Get_ICfgr(uni)						(IO_GIC_DIST.GICD_ICFGR[uni])						/**< Get GICD_ICFGR Interrupt Config field(uni:0 to 31)<br>
																													Field Value<br>
																													[2F]     : Reserved<br>
																													[2F+1] 0 : Level-Sensitive<br>
																													       1 : Edge-Triggered<br>
																													INTID<br>
																													INTID = ((uni*16) + Field offset)<br>
																											*/
#define Dd_GIC_DIST_Set_ICfgr(uni,val)					{IO_GIC_DIST.GICD_ICFGR[uni] = (val);}				/**< Set GICD_ICFGR Interrupt Config field(uni: 0 to 31)<br>
																													Field Value<br>
																													[2F]     : Reserved<br>
																													[2F+1] 0 : Level-Sensitive<br>
																													       1 : Edge-Triggered<br>
																													INTID<br>
																													INTID = ((uni*16) + Field offset)<br>
																											*/

// Private Peripheral Interrupt Status Register(GICD_PPISR)
#define Dd_GIC_DIST_Get_Ppisr							(IO_GIC_DIST.GICD_PPISR.word)						/**< Get GICD_PPISR PPI status<br>
																													PPI Status Value<br>
																													[9]  : Virtual Maintenance Interrupt<br>
																													[10] : Hypervisor timer event<br>
																													[11] : Virtual timer event<br>
																													[12] : nIRQ<br>
																													[13] : Non-secure physical timer event<br>
																													[14] : Secure physical timer event<br>
																													[15] : nFIQ<br>
																											*/

// Shared Peripheral Interrupt Status Register(GICD_SPISR)
#define Dd_GIC_DIST_Get_Spisr(uni)						(IO_GIC_DIST.GICD_SPISR[uni])						/**< Get GICD_SPISR SPI status(uni:0 to 14)<br>
																													SPI Status Value<br>
																													 0 : IRQS is LOW<br>
																													 1 : IRQS is HIGH<br>
																													INTID<br>
																													INTID = ((uni*32) + (bit position + 32))<br>
																											*/


// Software Generated Interrupt Register(GICD_SGIR)
#define Dd_GIC_DIST_Set_Sgir(val)						{IO_GIC_DIST.GICD_SGIR.word = (val);}				/**< Set GICD_SGIR SGIR bit<br>
																													SGIR Bit Field Value<br>
																													[25:24] : Process the requested SGI<br>
																													          0 : Specified in TargetList<br>
																													          1 : All CPU except the processor requested<br>
																													          2 : Only CPU of processor requested<br>
																													          3 : Reserved<br>
																													[23:16] : TargetList Filter<br>
																													          0bxxxxxxx1 : CPU interface 0<br>
																													          0bxxxxxx1x : CPU interface 1<br>
																													          0bxxxxx1xx : CPU interface 2<br>
																													          .<br>
																													          .<br>
																													          0b1xxxxxxx : CPU interface 7<br>
																													[15]    : NSATT<br>
																													          0 : Group0<br>
																													          1 : Group1<br>
																													[3:0]   : Interrupt ID(0 to 15)<br>
																											*/
// SGI Clear-Pending Register(GICD_CPENDSGIR)
#define Dd_GIC_DIST_Get_CPendSgir(uni)					(IO_GIC_DIST.GICD_CPENDSGIR[uni])					/**< Get GICD_CPENDSGIR SGI Clear-Pending bit(uni:0 to 3)<br>
																													Bit Field Value<br>
																													0 : Not pending<br>
																													1 : Pending<br>
																													INTID<br>
																													INTID = ((uni*4) + Field offset)<br>
																											*/
#define Dd_GIC_DIST_Set_CPendSgir(uni,val)				{IO_GIC_DIST.GICD_CPENDSGIR[uni] = (val);}			/**< Set GICD_CPENDSGIR SGI Clear-Pending bit(uni:0 to 3)<br>
																													Bit Field Value<br>
																													0 : Has no effect<br>
																													1 : Removes the pending<br>
																													INTID<br>
																													INTID = ((uni*4) + Field offset)<br>
																											*/

// SGI Set-Pending Register(GICD_SPENDSGIR)
#define Dd_GIC_DIST_Get_SPendSgir(uni)					(IO_GIC_DIST.GICD_SPENDSGIR[uni])					/**< Get GICD_SPENDSGIR SGI Set-Pending bit(uni:0 to 3)<br>
																													Bit Field Value<br>
																													0 : Not pending<br>
																													1 : Pending<br>
																													INTID<br>
																													INTID = ((uni*4) + Field offset)<br>
																											*/
#define Dd_GIC_DIST_Set_SPendSgir(uni,val)				{IO_GIC_DIST.GICD_SPENDSGIR[uni] = (val);}			/**< Set GICD_SPENDSGIR SGI Set-Pending bit(uni:0 to 3)<br>
																													Bit Field Value<br>
																													0 : Has no effect<br>
																													1 : Adds the pending<br>
																													INTID<br>
																													INTID = ((uni*4) + Field offset)<br>
																											*/

/* CPU Interface Control Register */

// CPU Interface Control Register(GICC_CTLR)
#define Dd_GIC_CPU_Get_Ctlr_EnableGrp0					(IO_GIC_CPU.GICC_CTLR.bit.ENABLEGRP0)				/**< Get GICC_CTLR Group0 interrupts(0:Disable, 1:Enable) */
#define Dd_GIC_CPU_Set_Ctlr_EnableGrp0(val)				{IO_GIC_CPU.GICC_CTLR.bit.ENABLEGRP0 = (val);}		/**< Set GICC_CTLR Group0 interrupts(0:Disable, 1:Enable) */
#define Dd_GIC_CPU_Get_Ctlr_EnableGrp1					(IO_GIC_CPU.GICC_CTLR.bit.ENABLEGRP1)				/**< Get GICC_CTLR Group1 interrupts(0:Disable, 1:Enable) */
#define Dd_GIC_CPU_Set_Ctlr_EnableGrp1(val)				{IO_GIC_CPU.GICC_CTLR.bit.ENABLEGRP1 = (val);}		/**< Set GICC_CTLR Group1 interrupts(0:Disable, 1:Enable) */
#define Dd_GIC_CPU_Get_Ctlr_Ackctl						(IO_GIC_CPU.GICC_CTLR.bit.ACKCTL)					/**< Get GICC_CTLR Acknowledged of GICC_IAR(0:Not Acknowledged, 1:Acknowledges) */
#define Dd_GIC_CPU_Set_Ctlr_Ackctl(val)					{IO_GIC_CPU.GICC_CTLR.bit.ACKCTL = (val);}			/**< Set GICC_CTLR Acknowledged of GICC_IAR(0:Not Acknowledged, 1:Acknowledges) */
#define Dd_GIC_CPU_Get_Ctlr_Fiqen						(IO_GIC_CPU.GICC_CTLR.bit.FIQEN)					/**< Get GICC_CTLR Use of FIQ or IRQ(0:IRQ, 1:FIQ) */
#define Dd_GIC_CPU_Set_Ctlr_Fiqen(val)					{IO_GIC_CPU.GICC_CTLR.bit.FIQEN = (val);}			/**< Set GICC_CTLR Use of FIQ or IRQ(0:IRQ, 1:FIQ) */
#define Dd_GIC_CPU_Get_Ctlr_Sbpr						(IO_GIC_CPU.GICC_CTLR.bit.CBPR)						/**< Get GICC_CTLR Control of GICC_BPR(0:Group0, 1:Both Group0 and Group1) */
#define Dd_GIC_CPU_Set_Ctlr_Sbpr(val)					{IO_GIC_CPU.GICC_CTLR.bit.CBPR = (val);}			/**< Set GICC_CTLR Control of GICC_BPR(0:Group0, 1:Both Group0 and Group1) */
#define Dd_GIC_CPU_Get_Ctlr_FiqByDisGrp0				(IO_GIC_CPU.GICC_CTLR.bit.FIQBYPDISGRP0)			/**< Get GICC_CTLR Control of Group0 FIQ Signal(0:Signal, 1:Not Signal) */
#define Dd_GIC_CPU_Set_Ctlr_FiqByDisGrp0(val)			{IO_GIC_CPU.GICC_CTLR.bit.FIQBYPDISGRP0 = (val);}	/**< Set GICC_CTLR Control of Group0 FIQ Signal(0:Signal, 1:Not Signal) */
#define Dd_GIC_CPU_Get_Ctlr_IrqByDisGrp0				(IO_GIC_CPU.GICC_CTLR.bit.IRQBYPDISGRP0)			/**< Get GICC_CTLR Control of Group0 IRQ Signal(0:Signal, 1:Not Signal) */
#define Dd_GIC_CPU_Set_Ctlr_IrqByDisGrp0(val)			{IO_GIC_CPU.GICC_CTLR.bit.IRQBYPDISGRP0 = (val);}	/**< Set GICC_CTLR Control of Group0 IRQ Signal(0:Signal, 1:Not Signal) */
#define Dd_GIC_CPU_Get_Ctlr_FiqByDisGrp1				(IO_GIC_CPU.GICC_CTLR.bit.FIQBYPDISGRP1)			/**< Get GICC_CTLR Control of Group1 FIQ Signal(0:Signal, 1:Not Signal) */
#define Dd_GIC_CPU_Set_Ctlr_FiqByDisGrp1(val)			{IO_GIC_CPU.GICC_CTLR.bit.FIQBYPDISGRP1 = (val);}	/**< Set GICC_CTLR Control of Group1 FIQ Signal(0:Signal, 1:Not Signal) */
#define Dd_GIC_CPU_Get_Ctlr_IrqByDisGrp1				(IO_GIC_CPU.GICC_CTLR.bit.IRQBYPDISGRP1)			/**< Get GICC_CTLR Control of Group1 IRQ Signal(0:Signal, 1:Not Signal) */
#define Dd_GIC_CPU_Set_Ctlr_IrqByDisGrp1(val)			{IO_GIC_CPU.GICC_CTLR.bit.IRQBYPDISGRP1 = (val);}	/**< Set GICC_CTLR Control of Group1 IRQ Signal(0:Signal, 1:Not Signal) */
#define Dd_GIC_CPU_Get_Ctlr_EoiModeS					(IO_GIC_CPU.GICC_CTLR.bit.EOIMODES)					/**< Get GICC_CTLR Control of GICC_EOIR and GICC_DIR<br>
																													0 : GICC_EOIR has both priority drop and deactivate interrupt functionality.
																													    Accesses to the GICC_DIR are UNPREDICTABLE.
																													1 : GICC_EOIR has priority drop functionality only.
																													    GICC_DIR has deactivate interrupt functionality.
																											*/
#define Dd_GIC_CPU_Set_Ctlr_EoiModeS(val)				{IO_GIC_CPU.GICC_CTLR.bit.EOIMODES = (val);}		/**< Set GICC_CTLR Control of GICC_EOIR and GICC_DIR<br>
																													0 : GICC_EOIR has both priority drop and deactivate interrupt functionality.
																													    Accesses to the GICC_DIR are UNPREDICTABLE.
																													1 : GICC_EOIR has priority drop functionality only.
																													    GICC_DIR has deactivate interrupt functionality.
																											*/
#define Dd_GIC_CPU_Get_Ctlr_EoiModeNS					(IO_GIC_CPU.GICC_CTLR.bit.EOIMODENS)				/**< Get GICC_CTLR Control of GICC_EOIR and GICC_DIR<br>
																													0 : GICC_EOIR has both priority drop and deactivate interrupt functionality.
																													    Accesses to the GICC_DIR are UNPREDICTABLE.
																													1 : GICC_EOIR has priority drop functionality only.
																													    GICC_DIR has deactivate interrupt functionality.
																											*/
#define Dd_GIC_CPU_Set_Ctlr_EoiModeNS(val)				{IO_GIC_CPU.GICC_CTLR.bit.EOIMODENS = (val);}		/**< Set GICC_CTLR Control of GICC_EOIR and GICC_DIR<br>
																													0 : GICC_EOIR has both priority drop and deactivate interrupt functionality.
																													    Accesses to the GICC_DIR are UNPREDICTABLE.
																													1 : GICC_EOIR has priority drop functionality only.
																													    GICC_DIR has deactivate interrupt functionality.
																											*/

// Interrupt Priority Mask Register(GICC_PMR)
#define Dd_GIC_CPU_Get_Pmr_Priority						(IO_GIC_CPU.GICC_PMR.bit.PRIORITY)					/**< Get GICC_PMR Priority bit<br>
																													Bit Field Value<br>
																													0xFF : 256 supported levels<br>
																													0xFE : 128 supported levels<br>
																													0xFC :  64 supported levels<br>
																													0xF8 :  32 supported levels<br>
																													0xF0 :  16 supported levels<br>
																											*/
#define Dd_GIC_CPU_Set_Pmr_Priority(val)				{IO_GIC_CPU.GICC_PMR.bit.PRIORITY = (val);}			/**< Set GICC_PMR Priority bit<br>
																													Bit Field Value<br>
																													0xFF : 256 supported levels<br>
																													0xFE : 128 supported levels<br>
																													0xFC :  64 supported levels<br>
																													0xF8 :  32 supported levels<br>
																													0xF0 :  16 supported levels<br>
																											*/

// Binary Point Register(GICC_BPR)
#define Dd_GIC_CPU_Get_Bpr_BinaryPoint					(IO_GIC_CPU.GICC_BPR.bit.BINARYPOINT)				/**< Get GICC_BPR BinaryPoint bit<br>
																													Bit Field Value<br>
																													0 to 7 : Interrupt preemption<br>
																											*/
#define Dd_GIC_CPU_Set_Bpr_BinaryPoint(val)				{IO_GIC_CPU.GICC_BPR.bit.BINARYPOINT = (val);}		/**< Set GICC_BPR BinaryPoint bit<br>
																													Bit Field Value<br>
																													<table>
																													<tr><th>val</th><th>Group priority field</th><th>Subpriority field</th><th>Field with binary point</th></tr>
																													<tr><td>0</td><td>[7:1]</td><td>[0]</td><td>ggggggg.s</td></tr>
																													<tr><td>1</td><td>[7:2]</td><td>[1:0]</td><td>gggggg.ss</td></tr>
																													<tr><td>2</td><td>[7:3]</td><td>[2:0]</td><td>ggggg.sss</td></tr>
																													<tr><td>3</td><td>[7:4]</td><td>[3:0]</td><td>gggg.ssss</td></tr>
																													<tr><td>4</td><td>[7:5]</td><td>[4:0]</td><td>ggg.sssss</td></tr>
																													<tr><td>5</td><td>[7:6]</td><td>[5:0]</td><td>gg.ssssss</td></tr>
																													<tr><td>6</td><td>[7]</td><td>[6:0]</td><td>g.sssssss</td></tr>
																													<tr><td>7</td><td>No preemption</td><td>[7:0]</td><td>.ssssssss</td></tr>
																													</table>
																											*/

// Interrupt Acknowledge Register(GICC_IAR)
#define Dd_GIC_CPU_Get_Iar								(IO_GIC_CPU.GICC_IAR.word)							/**< Get GICC_IAR Acknowledge Register */
#define Dd_GIC_CPU_Get_Iar_AckIntId						(IO_GIC_CPU.GICC_IAR.bit.ACKINTID)					/**< Get GICC_IAR AckIntId bit */
#define Dd_GIC_CPU_Get_Iar_CpuId						(IO_GIC_CPU.GICC_IAR.bit.CPUID)						/**< Get GICC_IAR CpuId bit */

// End of Interrupt Register(GICC_EOIR)
#define Dd_GIC_CPU_Set_Eoir(val)						{IO_GIC_CPU.GICC_EOIR.word = (val);}				/**< Set GICC_EOIR End of Interrupt Register */
#define Dd_GIC_CPU_Set_Eoir_EoiIntId(val)				{IO_GIC_CPU.GICC_EOIR.bit.EOIINTID = (val);}		/**< Set GICC_EOIR EoiIntId bit */
#define Dd_GIC_CPU_Set_Eoir_CpuId(val)					{IO_GIC_CPU.GICC_EOIR.bit.CPUID = (val);}			/**< Set GICC_EOIR CpuId bit */

// Running Priority Register(GICC_RPR)
#define Dd_GIC_CPU_Get_Rpr_Priority						(IO_GIC_CPU.GICC_RPR.bit.PRIORITY)					/**< Get GICC_RPR Priority bit */

// Highest Priority Pending Interrupt Register(GICC_HPPIR)
#define Dd_GIC_CPU_Get_Hppir_PendIntId					(IO_GIC_CPU.GICC_HPPIR.bit.PENDINTID)				/**< Get GICC_HPPIR PendIntId */
#define Dd_GIC_CPU_Get_Hppir_CpuId						(IO_GIC_CPU.GICC_HPPIR.bit.CPUID)					/**< Get GICC_HPPIR CpuId */

// Aliased Binary Point Register(GICC_ABPR)
#define Dd_GIC_CPU_Get_ABpr_BinaryPoint					(IO_GIC_CPU.GICC_ABPR.bit.BINARYPOINT)				/**< Get BINARYPOINT BinaryPoint bit<br>
																													Bit Field Value<br>
																													0 to 7 : Interrupt preemption<br>
																											*/

#define Dd_GIC_CPU_Set_ABpr_BinaryPoint(val)			{IO_GIC_CPU.GICC_ABPR.bit.BINARYPOINT = (val);}		/**< Set BINARYPOINT BinaryPoint bit <br>
																													Bit Field Value<br>
																													<table>
																													<tr><th>val</th><th>Group priority field</th><th>Subpriority field</th><th>Field with binary point</th></tr>
																													<tr><td>0</td><td>[7:1]</td><td>[0]</td><td>ggggggg.s</td></tr>
																													<tr><td>1</td><td>[7:2]</td><td>[1:0]</td><td>gggggg.ss</td></tr>
																													<tr><td>2</td><td>[7:3]</td><td>[2:0]</td><td>ggggg.sss</td></tr>
																													<tr><td>3</td><td>[7:4]</td><td>[3:0]</td><td>gggg.ssss</td></tr>
																													<tr><td>4</td><td>[7:5]</td><td>[4:0]</td><td>ggg.sssss</td></tr>
																													<tr><td>5</td><td>[7:6]</td><td>[5:0]</td><td>gg.ssssss</td></tr>
																													<tr><td>6</td><td>[7]</td><td>[6:0]</td><td>g.sssssss</td></tr>
																													<tr><td>7</td><td>No preemption</td><td>[7:0]</td><td>.ssssssss</td></tr>
																													</table>
																											*/

// Aliased Interrupt Acknowledge Register(GICC_AIAR)
#define Dd_GIC_CPU_Get_AIar								(IO_GIC_CPU.GICC_AIAR.word)							/**< Get GICC_AIAR Acknowledge Register */
#define Dd_GIC_CPU_Get_AIar_AckIntId					(IO_GIC_CPU.GICC_AIAR.bit.ACKINTID)					/**< Get GICC_AIAR AckIntId bit */
#define Dd_GIC_CPU_Get_AIar_CpuId						(IO_GIC_CPU.GICC_AIAR.bit.CPUID)					/**< Get GICC_AIAR CpuId bit */

// Aliased End of Interrupt Register(GICC_AEOIR)
#define Dd_GIC_CPU_Set_AEoir(val)						{IO_GIC_CPU.GICC_AEOIR.word = (val);}				/**< Set GICC_AEOIR End of Interrupt Register */
#define Dd_GIC_CPU_Set_AEoir_EoiIntId(val)				{IO_GIC_CPU.GICC_AEOIR.bit.EOIINTID = (val);}		/**< Set GICC_AEOIR EoiIntId bit */
#define Dd_GIC_CPU_Set_AEoir_CpuId(val)					{IO_GIC_CPU.GICC_AEOIR.bit.CPUID = (val);}			/**< Set GICC_AEOIR CpuId bit */

// Aliased Highest Priority Pending Interrupt Register(GICC_AHPPIR)
#define Dd_GIC_CPU_Get_AHppir_PendIntId					(IO_GIC_CPU.GICC_AHPPIR.bit.PENDINTID)				/**< Get GICC_AHPPIR PendIntId */
#define Dd_GIC_CPU_Get_AHppir_CpuId						(IO_GIC_CPU.GICC_AHPPIR.bit.CPUID)					/**< Get GICC_AHPPIR CpuId */

// Active Priorities Register(GICC_APR0)
#define Dd_GIC_CPU_Get_Apr0								(IO_GIC_CPU.GICC_APR0)								/**< Get GICC_APR0 Active Priorities Register */
#define Dd_GIC_CPU_Set_Apr0(val)						{IO_GIC_CPU.GICC_APR0 = (val);}						/**< Set GICC_APR0 Active Priorities Register */

// Non-secure Active Priorities Register(GICC_NSAPR0)
#define Dd_GIC_CPU_Get_NsApr0							(IO_GIC_CPU.GICC_NSAPR0)							/**< Get GICC_NSAPR0 Non-secure Active Priorities Register */
#define Dd_GIC_CPU_Set_NsApr0(val)						{IO_GIC_CPU.GICC_NSAPR0 = (val);}					/**< Set GICC_NSAPR0 Non-secure Active Priorities Register */

// CPU Interface Identification Register(GICC_IIDR)
#define Dd_GIC_CPU_Get_IIdr_Implementer					(IO_GIC_CPU.GICC_IIDR.bit.IMPLEMENTER)				/**< Get GICC_IIDR Implementer(0x43B:ARM implementation)<br>
																													[11:8] : 0x4 The JEP106 continuation code of the implementer<br>
																													[7]    : Always 0<br>
																													[6:0]  : 0x3B The JEP106 identity code of the implementer<br>
																											*/
#define Dd_GIC_CPU_Get_IIdr_Revision					(IO_GIC_CPU.GICC_IIDR.bit.REVISION)					/**< Get GICC_IIDR Revision(0x1:Revision r0p1) */
#define Dd_GIC_CPU_Get_IIdr_Version						(IO_GIC_CPU.GICC_IIDR.bit.VERSION)					/**< Get GICC_IIDR Architecture version(0x2:Version 2.0) */
#define Dd_GIC_CPU_Get_IIdr_ProductId					(IO_GIC_CPU.GICC_IIDR.bit.PRODUCTID)				/**< Get GICC_IIDR ProductID(0x010:Cortex-A7 MPCore product ID) */

// Deactivate Interrupt Register(GICC_DIR)
#define Dd_GIC_CPU_Set_Dir(val)							{IO_GIC_CPU.GICC_DIR.word = (val);}					/**< Set GICC_DIR Deactivate Interrupt Register<br>
																													Bit Field Value<br>
																													0 to 9   : Interrupt ID<br>
																													10 to 12 : CPU ID<br>
																											*/

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
Initialize Generic Interrupt Controller.
*/
extern VOID Dd_GIC_Init(VOID);

/**
Initialize Distributor Bank Register.
*/
extern VOID Dd_GIC_Dist_Bank_Init(VOID);

/**
Initialize CPU Interface Control Register.
*/
extern VOID Dd_GIC_Cpu_Init(VOID);

/**
End Generic Interrupt Controller.
*/
extern VOID Dd_GIC_End(VOID);

/**
Control Generic Interrupt Controller to set resource.
@param [in]	intid					: Interrupt ID (0 to @ref D_DD_GIC_INTID_SPI_MAX).
@param [in]	set_enable				: Interrupt Set-Enable Value.
									<ul>
									<li>0:disable
									<li>1:enable
									</ul>
@param [in]	intrpt_pri				: Interrupt Priority Value (-1 to 248).
									<ul>
									<li>0, 8, 16, 24, ..., 248 (in steps of 8):priority
									<li>-1:Not set
									</ul>
@param [in]	cpu_target				: Interrupt Processor Target (-1 to 15).
									<ul>
									<li>bit0:CPU0
									<li>bit1:CPU1
									<li>bit2:CPU2
									<li>bit3:CPU3
									<li>-1:No target
									</ul>
@retval	D_DDIM_OK					: Normal end
@retval	D_DD_GIC_INPUT_PARAM_ERR	: Input Parameter Error
@remarks	Disable interrupt is set after this function. If start, Dd_GIC_Init() should be used.
*/
extern INT32 Dd_GIC_Ctrl(E_DD_GIC_INTID intid, ULONG set_enable, LONG intrpt_pri, LONG cpu_target);

/**
Control Set Interrupt Priority Registers.
@param [in]	intid					: Interrupt ID (0 to @ref D_DD_GIC_INTID_SPI_MAX).
@param [in]	intrpt_pri				: Interrupt Priority Value (-1 to 248).
									<ul>
									<li>0, 8, 16, 24, ..., 248 (in steps of 8):priority
									<li>-1:Not set
									</ul>
@retval	D_DDIM_OK					: Normal end
@retval	D_DD_GIC_INPUT_PARAM_ERR	: Input Parameter Error
@remarks	Disable interrupt is set after this function. If start, Dd_GIC_Init() should be used.
*/
extern INT32 Dd_GIC_Set_Priority(E_DD_GIC_INTID intid, LONG intrpt_pri);

/**
Control Set Interrupt Processor Targets Registers.
@param [in]	intid					: Interrupt ID (0 to @ref D_DD_GIC_INTID_SPI_MAX).
@param [in]	cpu_target				: Interrupt Processor Target (-1 to 15).
									<ul>
									<li>bit0:CPU0
									<li>bit1:CPU1
									<li>bit2:CPU2
									<li>bit3:CPU3
									<li>-1:No target
									</ul>
@retval	D_DDIM_OK					: Normal end
@retval	D_DD_GIC_INPUT_PARAM_ERR	: Input Parameter Error
@remarks	Disable interrupt is set after this function. If start, Dd_GIC_Init() should be used.
*/
extern INT32 Dd_GIC_Set_Target_Cpu(E_DD_GIC_INTID intid, LONG cpu_target);

/**
SGI(Software Generated Interrupt) send to target CPU.<br>
Set Interrupt Software Generated Interrupt Register(ICDSGIR).
@param [in]	intid					: Software Generated Interrupt ID (0 to @ref D_DD_GIC_INTID_SGI_MAX).
@param [in]	target_filter			: Target List Filter (0 to 2).
									<ul>
									<li>0:Send the interrupt to the CPU interfaces specified in the CPUTargetList(cpu_target value).
									<li>1:Send the interrupt to all CPU interfaces except the CPU interface that requested the interrupt.
									<li>2:Send the interrupt only to the CPU interface that requested the interrupt.
									</ul>
@param [in]	cpu_target				: CPU Target (0 to 15).
									<ul>
									<li>0:No target (target_filter value 1 or 2)
									<li>1:CPU0 ([0]bit ON)
									<li>2:CPU1 ([1]bit ON)
									<li>4:CPU2 ([2]bit ON)
									<li>8:CPU3 ([3]bit ON)
									<li>15:All CPU
									</ul>
@retval	D_DDIM_OK					: Normal end
@retval	D_DD_GIC_INPUT_PARAM_ERR	: Input Parameter Error
@remarks	When this API call, send the interrupt to target CPU.
*/
extern INT32 Dd_GIC_Send_Sgi(E_DD_GIC_INTID intid, ULONG target_filter, ULONG cpu_target);

#ifdef __cplusplus
}
#endif

#endif	// _DD_GIC_H_

/*@}*/

/**
@weakgroup dd_gic
@{
	@section dd_gic_sample_section1		Sample of "operating procedure for generic interrupt start(CPU0)" using Dd_GIC_Init function.
	@code
	__align(64) ULONG gic_spin_lock;		// Necessarily 64 Byte Align & Cachable Area
	
	Dd_ARM_Spin_Lock(&gic_spin_lock);
	
	Dd_GIC_Init();							// Initialize(All GIC Register)
	
	Dd_ARM_Spin_Unlock(&gic_spin_lock);
	@endcode
	<br>
	@section dd_gic_sample_section2		Sample of "operating procedure for generic interrupt start(CPU1)" using Dd_GIC_Dist_Bank_Init function.
	@code
	__align(64) ULONG gic_spin_lock;		// Necessarily 64 Byte Align & Cachable Area
	
	Dd_ARM_Spin_Lock(&gic_spin_lock);
	
	Dd_GIC_Dist_Bank_Init();				// Initialize(Bank Register only)
	
	Dd_ARM_Spin_Unlock(&gic_spin_lock);
	@endcode
	<br>
	@section dd_gic_sample_section3		Sample of "operating procedure for generic interrupt Control" using Dd_GIC_Ctrl function.
	@code
	__align(64) ULONG gic_spin_lock;		// Necessarily 64 Byte Align & Cachable Area
	
	Dd_ARM_Spin_Lock(&gic_spin_lock);
	
	Dd_GIC_Ctrl(intid, set_enable, intrpt_pri, 0);
	
	Dd_ARM_Spin_Unlock(&gic_spin_lock);
	@endcode
	<br>
	@section dd_gic_sample_section4		Sample of "operating procedure for generic interrupt Stop" using Dd_GIC_End function.
	@code
	__align(64) ULONG gic_spin_lock;		// Necessarily 64 Byte Align & Cachable Area
	
	Dd_ARM_Spin_Lock(&gic_spin_lock);
	
	Dd_GIC_End();
	
	Dd_ARM_Spin_Unlock(&gic_spin_lock);
	@endcode
@}*/
