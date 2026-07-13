/**
 * @file		fj_peripheral.h
 * @brief		Peripheral Controller
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_PERIPHERAL_H
#define _FJ_PERIPHERAL_H

#include "ddim_typedef.h"
#include "fj_std.h"

///////////////////////////////////////////////////////////////////////////////
//  overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_peripheral_overview
@{
	The following Peripheral device controls can be achieved by using this APIs.<br>
	<br>
	<table style="text-align:left;" border="1" cellpadding="2" cellspacing="2">
		<tr><th valign=top style="background-color: rgb(204, 255, 255);">
			PWM</th>
			<td> Pulse Width Modulation<br>
				 - Built-in 16ch.<br>
				<br>
				[PWM timer A-D start mode]<br>
				 - Four independent PWM timers: A, B, C, and D are provided.<br>
				   Each timer supports 1-ch PWM output, and thus total 4-ch of PWM output are supported.<br>
				<br>
				[PWM timer E start mode]<br>
				 - PWM timer E that is applied to the control of stepping motor is provided.<br>
				   PWM output is supported by cooperating 4ch.</td>
		</tr>
		<tr><th valign=top style="background-color: rgb(204, 255, 255);">
			I2C</th>
			<td> Inter-Integrated Circuit<br>
				 - Built-in 3ch.</td>
				 - Supports master reception and transmission.</td>
				 - Supports standard mode (Max. 100KHz) / fast mode (Max. 400KHz).</td>
		</tr>
		<tr><th valign=top style="background-color: rgb(204, 255, 255);">
			UART</th>
			<td> Universal Asynchronous Receiver Transmitter<br>
				 - Built-in 8ch. (Common with SIO)<br>
				   Channel number specify to the UART APIs, please from 0 to 7.<br>
				 - Supports Normal mode and multi processor mode and debug mode.<br>
				 - Debug mode supports HDMAC.</td>
		</tr>
		<tr><th valign=top style="background-color: rgb(204, 255, 255);">
			SIO</th>
			<td> Serial I/O Communication<br>
				 - Built-in 8ch. (Common with UART)<br>
				   Channel number specify to the SIO APIs, please from 0 to 7.<br>
				 - Supports SPI (both master and slave support).<br>
				 - Supports full duplex mode and half duplex mode.<br>
				 - Supports HDMAC transfer.</td>
		</tr>
		<tr><th valign=top style="background-color: rgb(204, 255, 255);">
			SPI</th>
			<td> Serial Peripheral Interface<br>
				 - Built-in 3ch. (ch0 - ch2)<br>
				 - Supports full duplex mode and half duplex mode.<br>
				 - Supports HDMAC transfer.</td>
				 - SPI channel specifications are as follows.
					<table><tr>			<th>SPI ch</th>					<th>SSOUT max</th>	<th>Baudrate</th>	<th>FIFO size</th>	<th>Connectable to Gyro</th></tr>
					<tr align=right>	<td><b>ch0</b> (HW ch:1)</td>		<td>4</td>			<td>108MHz</td>		<td>128</td></tr>	<td>x</td></tr>
					<tr align=right>	<td><b>ch1</b> (HW ch:2)</td>		<td>4</td>			<td>54MHz</td>		<td>64</td></tr>	<td>x</td></tr>
					<tr align=right>	<td><b>ch2</b> (HW ch:3)</td>		<td>1</td>			<td>54MHz</td>		<td>16</td></tr>	<td>o</td></tr></table>
					Note:Channel number specify to the SPI APIs, please from 0 to 2.<br>
		</tr>
		<tr><th valign=top style="background-color: rgb(204, 255, 255);">
			Gyro IF</th>
			<td> Gyro Interface<br>
				 - 1 channel of SPI(ch 2) and 1 channel of 64-bit timer is embedded in this macro.<br>
				 - Maximum timer accuracy is 0.01us (=100MHz).<br>
				 - Timer count is up to 2^64 bits.<br>
				 - Stores Gyro data and TimeStamp data to SDRAM via DMA transfer on the interrupt from Gyro.</td>
				 - Gyro IF uses the HDMAC1 ch0..ch4.</td>
		</tr>
		<tr><th valign=top style="background-color: rgb(204, 255, 255);">
			Timer32</th>
			<td> Peripheral 32Bit Reload Timer<br>
				 - Built-in 14ch.<br>
				 - To be used for starting PWM and cycle of PWM and change settings for automatic DUTY.<br>
				 - external trigger input:ch0-5</td>
				 - external output (TOUT):ch3</td>
		</tr>
		<tr><th valign=top style="background-color: rgb(204, 255, 255);">
			WDOG</th>
			<td> Watchdog Timer<br>
				 - Supports 32-bit down counter.</td>
				 - Supports two modes. Watchdog mode occurs a system reset, timer mode is only to interrupt occurs.
		</tr>
		<tr><th valign=top style="background-color: rgb(204, 255, 255);">
			UDC</th>
			<td> Up/Down Counter<br>
				 - Built-in 6ch.<br>
				 - Counts up or down is possible by external trigger input.<br>
				 - Supports imer mode/up-down count mode/phase difference count mode.<br>
		</tr>
		<tr><th valign=top style="background-color: rgb(204, 255, 255);">
			GPIO</th>
			<td> General Purpose I/O Port<br>
				 - This LSI has 124 port pins of port-sharing pins and port-dedicated pins combined.<br>
				 - The following registers are controllable. 
					 - EPCR(External Port Control Register) 
					 - DDR(Data Direction Register)
					 - PDR(Port Data Register)
					 - PUDER(Pull-up/down Enable Register)
					 - PUDCR(Pull-up/down Control Register)
		</tr>
		<tr><th valign=top style="background-color: rgb(204, 255, 255);">
			ExtInterrupt</th>
			<td> External Interrupt Control<br>
				 - Conforms to AMBA 2.0 APB.<br>
				 - 1-32 interrupt request signals, configurable with a granularity of 1 signal at a time.<br>
				 - 4 types of detection methods (High Level, Low Level, Rising Edge, Falling Edge) can be selected for each interrupt request signal.<br>
		</tr>
		<tr><th valign=top style="background-color: rgb(204, 255, 255);">
			SLIMBus</th>
			<td> SLIMbus Control<br>
				 - Built-in 4ch.<br>
				   Channel number specify to the SLIMBus APIs, please from 0 to 3.</td>
		</tr>
	</table>
	<br>
@}*//* --- end of fj_peripheral_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_peripheral_sequence
@{
	- @ref PWM_One_Shot_Sequence
	- @ref PWM_Continuous_Sequence
	- @ref PWM_Stepping_Motor_Sequence
	- @ref I2C_Master_Read_Sequence
	- @ref I2C_Master_Write_Sequence
	- @ref I2C_Slave_Sequence
	- @ref SIO_Full_Duplex_Master
	- @ref SIO_Full_Duplex_Slave
	- @ref GYROIF_Read_Size_Fix
	- @ref fj_watchdog_init
	- @ref fj_watchdog_timermode
	- @ref fj_watchdog_wdogmode
	- @ref fj_watchdog_end
	- @ref fj_udc_init
	- @ref fj_udc_start
	- @ref fj_udc_end
	- @ref Timer32_Sequence
	- @ref UART_normal_mode
	- @ref UART_multi_mode_master
	- @ref UART_multi_mode_slave
	- @ref SLIMBUS_Sequence

	@section PWM_One_Shot_Sequence PWM One-Shot Mode Sequence
	@image html fj_peripheral_pwm_one_shot_sequence.png
	<br>
	<br>
	@section PWM_Continuous_Sequence PWM Continuous Mode Sequence
	@image html fj_peripheral_pwm_continuous_sequence.png
	<br>
	<br>
	@section PWM_Stepping_Motor_Sequence PWM Stepping Motor Mode Sequence
	@image html fj_peripheral_pwm_stepping_motor_sequence.png
	<br>
	<br>
	@section I2C_Master_Read_Sequence I2C Master Read Sequence
	@image html fj_peripheral_i2c_master_read.png
	<br>
	<br>
	@section I2C_Master_Write_Sequence I2C Master Write Sequence
	@image html fj_peripheral_i2c_master_write.png
	<br>
	<br>
	@section I2C_Slave_Sequence I2C Slave Sequence
	@image html fj_peripheral_i2c_slave.png
	<br>
	<br>
	@section SIO_Full_Duplex_Master Serial I/O Full Duplex Communication Sequence (Master)
	@image html fj_peripheral_sio_full_duplex_master.png
	<br>
	<br>
	@section SIO_Full_Duplex_Slave Serial I/O Full Duplex Communication Sequence (Slave)
	@image html fj_peripheral_sio_full_duplex_slave.png
	<br>
	<br>
	@section GYROIF_Read_Size_Fix Gyro I/F Read Sequence (Fixed Size)
	@image html fj_peripheral_gyroif_fix_size.png
	<br>
	<br>
	@section fj_watchdog_init Watchdog function initialize sequence
	@image html fj_peripheral_wdog_timer_initialize.png
	<br>
	<br>
	@section fj_watchdog_timermode Watchdog fuction basic sequence (Timer mode)
	@image html fj_peripheral_wdog_timer_timermode_sequence.png
	<br>
	<br>
	@section fj_watchdog_wdogmode Watchdog fuction basic sequence (Watchdog mode)
	@image html fj_peripheral_wdog_timer_wdogmode_sequence.png
	<br>
	<br>
	@section fj_watchdog_end Watchdog function end sequence
	@image html fj_peripheral_wdog_timer_end.png
	<br>
	<br>
	@section fj_udc_init Up down counter function initialize sequence
	@image html fj_peripheral_udc_initialization.png
	<br>
	<br>
	@section fj_udc_start Up down counter function basic sequence
	@image html fj_peripheral_udc_basic_sequence.png
	<br>
	<br>
	@section fj_udc_end Up down counter function end sequence
	@image html fj_peripheral_udc_ending.png
	<br>
	<br>
	@section Timer32_Sequence Timer32 Sequence
	@image html fj_peripheral_timer32.png
	<br>
	<br>
	@section UART_normal_mode UART Normal Mode Sequence
	@image html fj_peripheral_uart_normal_mode.png
	<br>
	<br>
	@section UART_multi_mode_master UART Multi Mode Sequence (Master)
	@image html fj_peripheral_uart_multi_mode_master.png
	<br>
	<br>
	@section UART_multi_mode_slave UART Multi Mode Sequence (Slave)
	@image html fj_peripheral_uart_multi_mode_slave.png
	<br>
	<br>
	@section SLIMBUS_Sequence SLIMBus Sequence
	@image html fj_peripheral_slimbus_sequence.png
	<br>
	<br>

@}*//* --- end of fj_peripheral_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_peripheral_definition
@{*/
/* ------------- */
/* PWM           */
/* ------------- */
#define	FJ_PWM_STATUS_TRG_INPUT			(0)		/**< PWM Status is Trigger input */
#define	FJ_PWM_STATUS_CYCLE_MATCH		(1)		/**< PWM Status is Matching cycle */
#define	FJ_PWM_STATUS_DUTY_MATCH		(2)		/**< PWM Status is Matching duty */
#define	FJ_PWM_STATUS_CYC_DUTY_MATCH	(3)		/**< PWM Status is Matching duty */
#define	FJ_PWM_STATUS_UNDER_RUN			(4)		/**< PWM Status is Under run error for stepping motor */
#define	FJ_PWM_STATUS_FORCE_STOP		(5)		/**< PWM Status is Force stop for stepping motor */
#define	FJ_PWM_STATUS_AUTO_STOP			(6)		/**< PWM Status is Auto stop for stepping motor */

/* ------------- */
/* I2C           */
/* ------------- */
// Slave mode
#define FJ_I2C_SEND_DATA		(1)		/**< Send data to master */
#define FJ_I2C_SEND_ACK			(2)		/**< Send ACK to master */
#define FJ_I2C_SEND_NO_ACK 		(3)		/**< Send NO ACK to master */

/* ------------- */
/* SPI           */
/* ------------- */
#define FJ_SPI_SS_MAX			(4)		/**< Maximum connectable slave select */

/* ------------- */
/* GPIO          */
/* ------------- */
// GPIO functional type (EPCR)
#define FJ_GPIO_FUNC_GPIO	(0)		/**< GPIO mode */
#define FJ_GPIO_FUNC_HWMODE	(1)		/**< Hardware mode (Resource output signal) */

// GPIO direction (DDR)
#define FJ_GPIO_DIR_IN		(0)		/**< Input port */
#define FJ_GPIO_DIR_OUT		(1)		/**< Output port */

// GPIO status (PDR)
#define FJ_GPIO_STATUS_LOW	(0)		/**< Low status */
#define FJ_GPIO_STATUS_HIGH	(1)		/**< High status */

// GPIO pull-up/down register enable (PUDER)
#define FJ_GPIO_PUD_OFF		(0)		/**< Pull-up/down disable */
#define FJ_GPIO_PUD_ON		(1)		/**< Pull-up/down enable */

// GPIO pull-up/down register control (PUDCR)
#define FJ_GPIO_PUD_DOWN	(0)		/**< Pull-down */
#define FJ_GPIO_PUD_UP		(1)		/**< Pull-Up */


///////////////////////////////////////////////////////////////////////////////
// typedef
///////////////////////////////////////////////////////////////////////////////
/* ------ */
/* Ch     */
/* ------ */
/** Peripheral channel number. */
typedef enum{
	FJ_PERI_CH_0 = 0,		/**< Peripheral ch0 */
	FJ_PERI_CH_1,			/**< Peripheral ch1 */
	FJ_PERI_CH_2,			/**< Peripheral ch2 */
	FJ_PERI_CH_3,			/**< Peripheral ch3 */
	FJ_PERI_CH_4,			/**< Peripheral ch4 */
	FJ_PERI_CH_5,			/**< Peripheral ch5 */
	FJ_PERI_CH_6,			/**< Peripheral ch6 */
	FJ_PERI_CH_7,			/**< Peripheral ch7 */
	FJ_PERI_CH_8,			/**< Peripheral ch8 */
	FJ_PERI_CH_9,			/**< Peripheral ch9 */
	FJ_PERI_CH_10,			/**< Peripheral ch10 */
	FJ_PERI_CH_11,			/**< Peripheral ch11 */
	FJ_PERI_CH_12,			/**< Peripheral ch12 */
	FJ_PERI_CH_13,			/**< Peripheral ch13 */
	FJ_PERI_CH_14,			/**< Peripheral ch14 */
	FJ_PERI_CH_15,			/**< Peripheral ch15 */
} FJ_PERI_CH;

/* ------------- */
/* SIO           */
/* ------------- */
/** SIO master/slave selection. */
typedef enum{
	FJ_SIO_TYPE_MASTER = 0,			/**< Master side */
	FJ_SIO_TYPE_SLAVE				/**< Slave side */
}FJ_SIO_TYPE;

/** SIO transmit mode selection. */
typedef enum{
	FJ_SIO_MODE_NORMAL_SC_H = 0,	/**< Normal communication & serial clock is "H" format */
	FJ_SIO_MODE_NORMAL_SC_L,		/**< Normal communication & serial clock is "L" format */
	FJ_SIO_MODE_SPI_SC_H,			/**< SPI communication & serial clock is "H" format */
	FJ_SIO_MODE_SPI_SC_L			/**< SPI communication & serial clock is "L" format */
} FJ_SIO_MODE;

/** SIO transferring direction selection. */
typedef enum {
	FJ_SIO_BIT_DIR_LSB_FIRST = 0,	/**< LSB first */
	FJ_SIO_BIT_DIR_MSB_FIRST		/**< MSB first */
} FJ_SIO_BIT_DIR;

/** SIO data bit length selection. */
typedef enum{
	FJ_SIO_LENGTH_8 = 0,			/**< 8 bit length */
	FJ_SIO_LENGTH_5,				/**< 5 bit length */
	FJ_SIO_LENGTH_6,				/**< 6 bit length */
	FJ_SIO_LENGTH_7,				/**< 7 bit length */
	FJ_SIO_LENGTH_9					/**< 9 bit length */
} FJ_SIO_LENGTH;

/** SIO transferring data direction selection. */
typedef enum {
	FJ_SIO_DATA_DIR_SEND = 0,	/**< Send data */
	FJ_SIO_DATA_DIR_RECV		/**< Receive data */
} FJ_SIO_DATA_DIR;

/* ------------- */
/* SPI           */
/* ------------- */
/** SPI master/slave selection. */
typedef enum{
	FJ_SPI_TYPE_SLAVE = 0,			/**< Slave side */
	FJ_SPI_TYPE_MASTER				/**< Master side */
}FJ_SPI_TYPE;

/** SPI transmit mode selection. */
typedef enum{
	FJ_SPI_MODE_0_CPOL0_CPHA0 = 0,	/**< Mode0:Clock Polarity=0, Clock Phase=0<br>
											 Data are captured on the clock's rising edge and data is output on a falling edge. */
	FJ_SPI_MODE_1_CPOL0_CPHA1,		/**< Mode1:Clock Polarity=0, Clock Phase=1
											 Data are captured on the clock's falling edge and data is output on a rising edge. */
	FJ_SPI_MODE_2_CPOL1_CPHA0,		/**< Mode2:Clock Polarity=1, Clock Phase=0
											 Data are captured on clock's falling edge and data is output on a rising edge. */
	FJ_SPI_MODE_3_CPOL1_CPHA1,		/**< Mode3:Clock Polarity=1, Clock Phase=1
											 Data are captured on clock's rising edge and data is output on a falling edge. */
} FJ_SPI_MODE;

/** SPI transferring direction selection. */
typedef enum {
	FJ_SPI_BIT_DIR_LSB_FIRST = 0,	/**< LSB first */
	FJ_SPI_BIT_DIR_MSB_FIRST		/**< MSB first */
} FJ_SPI_BIT_DIR;

/** SPI data bit length selection. */
typedef enum{
	FJ_SPI_LENGTH_4 = 3,			/**<  4 bit length */
	FJ_SPI_LENGTH_5,				/**<  5 bit length */
	FJ_SPI_LENGTH_6,				/**<  6 bit length */
	FJ_SPI_LENGTH_7,				/**<  7 bit length */
	FJ_SPI_LENGTH_8,				/**<  8 bit length */
	FJ_SPI_LENGTH_9,				/**<  9 bit length */
	FJ_SPI_LENGTH_10,				/**< 10 bit length */
	FJ_SPI_LENGTH_11,				/**< 11 bit length */
	FJ_SPI_LENGTH_12,				/**< 12 bit length */
	FJ_SPI_LENGTH_13,				/**< 13 bit length */
	FJ_SPI_LENGTH_14,				/**< 14 bit length */
	FJ_SPI_LENGTH_15,				/**< 15 bit length */
	FJ_SPI_LENGTH_16,				/**< 16 bit length */
	FJ_SPI_LENGTH_17,				/**< 17 bit length */
	FJ_SPI_LENGTH_18,				/**< 18 bit length */
	FJ_SPI_LENGTH_19,				/**< 19 bit length */
	FJ_SPI_LENGTH_20,				/**< 20 bit length */
	FJ_SPI_LENGTH_21,				/**< 21 bit length */
	FJ_SPI_LENGTH_22,				/**< 22 bit length */
	FJ_SPI_LENGTH_23,				/**< 23 bit length */
	FJ_SPI_LENGTH_24,				/**< 24 bit length */
	FJ_SPI_LENGTH_25,				/**< 25 bit length */
	FJ_SPI_LENGTH_26,				/**< 26 bit length */
	FJ_SPI_LENGTH_27,				/**< 27 bit length */
	FJ_SPI_LENGTH_28,				/**< 28 bit length */
	FJ_SPI_LENGTH_29,				/**< 29 bit length */
	FJ_SPI_LENGTH_30,				/**< 30 bit length */
	FJ_SPI_LENGTH_31,				/**< 31 bit length */
	FJ_SPI_LENGTH_32,				/**< 32 bit length */
} FJ_SPI_LENGTH;

/** SPI transferring data direction selection. */
typedef enum {
	FJ_SPI_DATA_DIR_SEND = 0,	/**< Send data */
	FJ_SPI_DATA_DIR_RECV		/**< Receive data */
} FJ_SPI_DATA_DIR;

/** ssOut continuous selection. */
typedef enum {
	FJ_SPI_CONT_TRANS_INACT_BETWEEN = 0,	/**< ssOut goes inactive between successive transfers. */
	FJ_SPI_CONT_TRANS_INACT_FIFO_EMPTY,		/**< ssOut become inactive when the TX FIFO is empty. */
	FJ_SPI_CONT_TRANS_ACT_FIFO_EMPTY		/**< ssOut remain active even when the TX FIFO is empty. */
} FJ_SPI_CONT_TRANS;


/** SIO and UART and SPI DMA selection. */
typedef enum{
	FJ_USIO_DMA_CH_NOT_USE = -1,	/**< DMA is not use */
	FJ_USIO_DMA_CH_0,				/**< HDMAC1 ch0 */
	FJ_USIO_DMA_CH_1,				/**< HDMAC1 ch1 */
	FJ_USIO_DMA_CH_2,				/**< HDMAC1 ch2 */
	FJ_USIO_DMA_CH_3,				/**< HDMAC1 ch3 */
	FJ_USIO_DMA_CH_4,				/**< HDMAC1 ch4 */
	FJ_USIO_DMA_CH_5,				/**< HDMAC1 ch5 */
	FJ_USIO_DMA_CH_6,				/**< HDMAC1 ch6 */
	FJ_USIO_DMA_CH_7,				/**< HDMAC1 ch7 */
} FJ_USIO_DMA_CH;

/* ------------- */
/* Gyro IF       */
/* ------------- */
/** Gyro IF 64bit Timer count clock select. */
typedef enum {
	FJ_GYROIF_TMR_CLK_DIV_2 = 0,	/**< 2 dividing frequency */
	FJ_GYROIF_TMR_CLK_DIV_8,		/**< 8 dividing frequency */
	FJ_GYROIF_TMR_CLK_DIV_32,		/**< 32 dividing frequency */
} FJ_GYROIF_TMR_CLK;


/* ------------- */
/* GPIO          */
/* ------------- */
/** GPIO port name */
typedef enum {
	FJ_GPIO_P60	= 0,		/**< GPIO port P60 */
	FJ_GPIO_P61,				/**< GPIO port P61 */
	FJ_GPIO_P62,				/**< GPIO port P62 */
	FJ_GPIO_P63,				/**< GPIO port P63 */
	FJ_GPIO_P64,				/**< GPIO port P64 */
	FJ_GPIO_P65,				/**< GPIO port P65 */
	FJ_GPIO_P66,				/**< GPIO port P66 */
	FJ_GPIO_P67,				/**< GPIO port P67 */
	FJ_GPIO_P70,				/**< GPIO port P70 */
	FJ_GPIO_P71,				/**< GPIO port P71 */
	FJ_GPIO_P72,				/**< GPIO port P72 */
	FJ_GPIO_P73,				/**< GPIO port P73 */
	FJ_GPIO_P74,				/**< GPIO port P74 */
	FJ_GPIO_P75,				/**< GPIO port P75 */
	FJ_GPIO_P76,				/**< GPIO port P76 */
	FJ_GPIO_P77,				/**< GPIO port P77 */
	FJ_GPIO_P80,				/**< GPIO port P80 */
	FJ_GPIO_P81,				/**< GPIO port P81 */
	FJ_GPIO_P82,				/**< GPIO port P82 */
	FJ_GPIO_P83,				/**< GPIO port P83 */
	FJ_GPIO_P84,				/**< GPIO port P84 */
	FJ_GPIO_P85,				/**< GPIO port P85 */
	FJ_GPIO_P86,				/**< GPIO port P86 */
	FJ_GPIO_P87,				/**< GPIO port P87 */
	FJ_GPIO_P90,				/**< GPIO port P90 */
	FJ_GPIO_P91,				/**< GPIO port P91 */
	FJ_GPIO_P92,				/**< GPIO port P92 */
	FJ_GPIO_P93,				/**< GPIO port P93 */
	FJ_GPIO_PA0,				/**< GPIO port PA0 */
	FJ_GPIO_PA1,				/**< GPIO port PA1 */
	FJ_GPIO_PA2,				/**< GPIO port PA2 */
	FJ_GPIO_PA3,				/**< GPIO port PA3 */
	FJ_GPIO_PA4,				/**< GPIO port PA4 */
	FJ_GPIO_PA5,				/**< GPIO port PA5 */
	FJ_GPIO_PA6,				/**< GPIO port PA6 */
	FJ_GPIO_PA7,				/**< GPIO port PB7 */
	FJ_GPIO_PB0,				/**< GPIO port PB0 */
	FJ_GPIO_PB1,				/**< GPIO port PB1 */
	FJ_GPIO_PB2,				/**< GPIO port PB2 */
	FJ_GPIO_PB3,				/**< GPIO port PB3 */
	FJ_GPIO_PB4,				/**< GPIO port PB4 */
	FJ_GPIO_PC0L,				/**< GPIO port PC0L */
	FJ_GPIO_PC1L,				/**< GPIO port PC1L */
	FJ_GPIO_PC2L,				/**< GPIO port PC2L */
	FJ_GPIO_PC3L,				/**< GPIO port PC3L */
	FJ_GPIO_PC4L,				/**< GPIO port PC4L */
	FJ_GPIO_PC5L,				/**< GPIO port PC5L */
	FJ_GPIO_PD0,				/**< GPIO port PD0 */
	FJ_GPIO_PD1,				/**< GPIO port PD1 */
	FJ_GPIO_PD2,				/**< GPIO port PD2 */
	FJ_GPIO_PD3,				/**< GPIO port PD3 */
	FJ_GPIO_PD4,				/**< GPIO port PD4 */
	FJ_GPIO_PD5,				/**< GPIO port PD5 */
	FJ_GPIO_PD6,				/**< GPIO port PD6 */
	FJ_GPIO_PE0,				/**< GPIO port PE0 */
	FJ_GPIO_PE1,				/**< GPIO port PE1 */
	FJ_GPIO_PE2,				/**< GPIO port PE2 */
	FJ_GPIO_PE3,				/**< GPIO port PE3 */
	FJ_GPIO_PE4,				/**< GPIO port PE4 */
	FJ_GPIO_PE5,				/**< GPIO port PE5 */
	FJ_GPIO_PF0,				/**< GPIO port PF0 */
	FJ_GPIO_PF1,				/**< GPIO port PF1 */
	FJ_GPIO_PF2,				/**< GPIO port PF2 */
	FJ_GPIO_PF3,				/**< GPIO port PF3 */
	FJ_GPIO_PF4,				/**< GPIO port PF4 */
	FJ_GPIO_PG0,				/**< GPIO port PG0 */
	FJ_GPIO_PG1,				/**< GPIO port PG1 */
	FJ_GPIO_PG2,				/**< GPIO port PG2 */
	FJ_GPIO_PG3,				/**< GPIO port PG3 */
	FJ_GPIO_PG4,				/**< GPIO port PG4 */
	FJ_GPIO_PG5,				/**< GPIO port PG5 */
	FJ_GPIO_PG6,				/**< GPIO port PG6 */
	FJ_GPIO_PG7,				/**< GPIO port PG7 */
	FJ_GPIO_PH0,				/**< GPIO port PH0 */
	FJ_GPIO_PH1,				/**< GPIO port PH1 */
	FJ_GPIO_PH2,				/**< GPIO port PH2 */
	FJ_GPIO_PH3,				/**< GPIO port PH3 */
	FJ_GPIO_PW0,				/**< GPIO port PW0 */
	FJ_GPIO_PW1,				/**< GPIO port PW1 */
	FJ_GPIO_PW2,				/**< GPIO port PW2 */
	FJ_GPIO_PW3,				/**< GPIO port PW3 */
	FJ_GPIO_PW4,				/**< GPIO port PW4 */
	FJ_GPIO_PW5,				/**< GPIO port PW5 */
	FJ_GPIO_PW6,				/**< GPIO port PW6 */
	FJ_GPIO_PW7,				/**< GPIO port PW7 */
	FJ_GPIO_PJ0,				/**< GPIO port PJ0 */
	FJ_GPIO_PJ1,				/**< GPIO port PJ1 */
	FJ_GPIO_PJ2,				/**< GPIO port PJ2 */
	FJ_GPIO_PJ3,				/**< GPIO port PJ3 */
	FJ_GPIO_PJ4,				/**< GPIO port PJ4 */
	FJ_GPIO_PJ5,				/**< GPIO port PJ5 */
	FJ_GPIO_PJ6,				/**< GPIO port PJ6 */
	FJ_GPIO_PJ7,				/**< GPIO port PJ7 */
	FJ_GPIO_PK0,				/**< GPIO port PK0 */
	FJ_GPIO_PK1,				/**< GPIO port PK1 */
	FJ_GPIO_PK2,				/**< GPIO port PK2 */
	FJ_GPIO_PK3,				/**< GPIO port PK3 */
	FJ_GPIO_PK4,				/**< GPIO port PK4 */
	FJ_GPIO_PK5,				/**< GPIO port PK5 */
	FJ_GPIO_PK6,				/**< GPIO port PK6 */
	FJ_GPIO_PK7,				/**< GPIO port PK7 */
	FJ_GPIO_PL0,				/**< GPIO port PL0 */
	FJ_GPIO_PL1,				/**< GPIO port PL1 */
	FJ_GPIO_PL2,				/**< GPIO port PL2 */
	FJ_GPIO_PL3,				/**< GPIO port PL3 */
	FJ_GPIO_PL4,				/**< GPIO port PL4 */
	FJ_GPIO_PL5,				/**< GPIO port PL5 */
	FJ_GPIO_PM0,				/**< GPIO port PM0 */
	FJ_GPIO_PM1,				/**< GPIO port PM1 */
	FJ_GPIO_PM2,				/**< GPIO port PM2 */
	FJ_GPIO_PM3,				/**< GPIO port PM3 */
	FJ_GPIO_PM4,				/**< GPIO port PM4 */
	FJ_GPIO_PN0,				/**< GPIO port PN0 */
	FJ_GPIO_PN1,				/**< GPIO port PN1 */
	FJ_GPIO_PN2,				/**< GPIO port PN2 */
	FJ_GPIO_PN3,				/**< GPIO port PN3 */
	FJ_GPIO_PN4,				/**< GPIO port PN4 */
	FJ_GPIO_PY0,				/**< GPIO port PY0 */
	FJ_GPIO_PY1,				/**< GPIO port PY1 */
	FJ_GPIO_PY2,				/**< GPIO port PY2 */
	FJ_GPIO_PY3,				/**< GPIO port PY3 */
	FJ_GPIO_PY4,				/**< GPIO port PY4 */
	FJ_GPIO_PY5,				/**< GPIO port PY5 */
	FJ_GPIO_PY6,				/**< GPIO port PY6 */
	FJ_GPIO_PY7,				/**< GPIO port PY7 */
	FJ_GPIO_PP0,				/**< GPIO port PP0 */
	FJ_GPIO_PP1,				/**< GPIO port PP1 */
	FJ_GPIO_PP2,				/**< GPIO port PP2 */
	FJ_GPIO_PP3,				/**< GPIO port PP3 */
	FJ_GPIO_PORT_MAX			/**< GPIO port maximum value */
} FJ_GPIO_PORT;

/** External Interrupt Trigger Type */
typedef enum{
	FJ_EXT_INT_TRIGGER_TYPE_LOWLEVEL = 0,	/**< Trigger Type Low Level */
	FJ_EXT_INT_TRIGGER_TYPE_HIGHLEVEL,		/**< Trigger Type Hight Level */
	FJ_EXT_INT_TRIGGER_TYPE_RISEEDGE,		/**< Trigger Type Rise Edge */
	FJ_EXT_INT_TRIGGER_TYPE_FALLEDGE,		/**< Trigger Type Fall Edge */
	FJ_EXT_INT_TRIGGER_TYPE_MAX				/**< Trigger Type maximum value */
}FJ_EXT_INT_TRIGGER_TYPE;

/** External Interrupt Target CPU */
typedef enum{
	FJ_EXT_INT_TARGET_CPU_0 = 1,			/**< Target CPU0						*/
	FJ_EXT_INT_TARGET_CPU_1,				/**< Target CPU1 						*/
	FJ_EXT_INT_TARGET_CPU_0_1,				/**< Target CPU0 & CPU1					*/
	FJ_EXT_INT_TARGET_CPU_2,				/**< Target CPU2						*/
	FJ_EXT_INT_TARGET_CPU_0_2,				/**< Target CPU0 & CPU2					*/
	FJ_EXT_INT_TARGET_CPU_1_2,				/**< Target CPU1 & CPU2					*/
	FJ_EXT_INT_TARGET_CPU_0_1_2,			/**< Target CPU0 & CPU1 & CPU2			*/
	FJ_EXT_INT_TARGET_CPU_3,				/**< Target CPU3						*/
	FJ_EXT_INT_TARGET_CPU_0_3,				/**< Target CPU0 & CPU3					*/
	FJ_EXT_INT_TARGET_CPU_1_3,				/**< Target CPU1 & CPU3					*/
	FJ_EXT_INT_TARGET_CPU_0_1_3,			/**< Target CPU0 & CPU1 & CPU3			*/
	FJ_EXT_INT_TARGET_CPU_2_3,				/**< Target CPU2 & CPU3					*/
	FJ_EXT_INT_TARGET_CPU_0_2_3,			/**< Target CPU0 & CPU2 & CPU3			*/
	FJ_EXT_INT_TARGET_CPU_1_2_3,			/**< Target CPU1 & CPU2 & CPU3			*/
	FJ_EXT_INT_TARGET_CPU_0_1_2_3			/**< Target CPU0 & CPU1 & CPU2 & CPU3	*/
}FJ_EXT_INT_TARGET_CPU;

/* ------------- */
/* PWM           */
/* ------------- */
/** PWM mode. */
typedef enum{
	FJ_PWM_MODE_CONTINUOUS = 0,	/**< Continuous mode */
	FJ_PWM_MODE_ONE_SHOT		/**< One-Shot mode */
} FJ_PWM_MODE;

/** PWM output polarity. */
typedef enum{
	FJ_PWM_POLARITY_NORMAL = 0,	/**< Normal polarity */
	FJ_PWM_POLARITY_REVERSE		/**< Reverse polarity */
} FJ_PWM_POLARITY;

/** PWM count clock select. */
typedef enum{
	FJ_PWM_CLK_0	= 0,		/**< not dividing frequency	*/
	FJ_PWM_CLK_4,				/**< 4 dividing frequency */
	FJ_PWM_CLK_16,				/**< 16 dividing frequency */
	FJ_PWM_CLK_64,				/**< 64 dividing frequency */
}FJ_PWM_CLK;

/** PWM trigger edge select. */
typedef enum{
	FJ_PWM_EDGE_RISE	= 1,	/**< rise edge */
	FJ_PWM_EDGE_FALL,			/**< fall edge */
	FJ_PWM_EDGE_BOTH,			/**< both edge */
}FJ_PWM_EDGE;

/** PWM stepping motor channel assignment factor. */
typedef enum{
	FJ_PWM_STP_CHAS_NORMAL	= 0,	/**< Normal  : A B C D */
	FJ_PWM_STP_CHAS_REVERSE 		/**< Reverse : C B A D */
} FJ_PWM_STP_CHAS;

/** PWM restart enable. */
typedef enum{
	FJ_PWM_RESTART_DISABLE = 0,	/**< Restart disable */
	FJ_PWM_RESTART_ENABLE		/**< Restart enable */
} FJ_PWM_RESTART;

/** PWM force at stop select. */
typedef enum{
	FJ_PWM_FORCE_DISABLE	= -1,	/**< Force disable */
	FJ_PWM_FORCE_0,					/**< Force to 0 */
	FJ_PWM_FORCE_1,					/**< Force to 1 */
}FJ_PWM_FORCE;

/* ------------- */
/* Watchdog      */
/* ------------- */
/** Watchdog mode setting. */
typedef enum {
	FJ_WDOG_MODE_TIMER	= 0,				/**< Timer mode (Interrupt only) */
	FJ_WDOG_MODE_WDOG						/**< Watchdog mode (Assert reset) */
} FJ_WDOG_MODE;

/** Raw interrupt status from the watchdog counter */
typedef enum {
	FJ_WDOG_RESET_STATUS_NONE	= 0,		/**< Not occured */
	FJ_WDOG_RESET_STATUS_OCCUR				/**< Occured */
} FJ_WDOG_RESET_STATUS;

/** Masked interrupt status from the watchdog counter */
typedef enum {
	FJ_WDOG_RESET_MASK_STATUS_NONE	= 0,	/**< Not maks occured */
	FJ_WDOG_RESET_MASK_STATUS_OCCUR			/**< Make occured */
} FJ_WDOG_RESET_MASK_STATUS;

/* ------------- */
/* UDC			 */
/* ------------- */
/** Up_Down counting clear function enable. */
typedef enum {
	FJ_UDC_COMP_CLEAR_DISABLE = 0,		/**< counter clear disable */
	FJ_UDC_COMP_CLEAR_ENABLE  = 1		/**< counter clear enable */
} FJ_UDC_COMP_CLEAR;

/** Up_Down counting reload function enble.  */
typedef enum {
	FJ_UDC_RELOAD_DISABLE = 0,			/**< counter reload disable */
	FJ_UDC_RELOAD_ENABLE  = 1			/**< counter reload enable */
} FJ_UDC_RELOAD;

/** Up_Down counting select Z-terminal function for counter clear or gate. */
typedef enum {
	FJ_UDC_ZIN_COUNT_CLEAR = 0,			/**< select counter clear function  */
	FJ_UDC_ZIN_COUNT_GATE  = 1			/**< select gate function */
} FJ_UDC_ZIN_COUNT;

/** Up_Down counting select Z-terminal edge on counter clear function and level on gate function  */
typedef enum {
	FJ_UDC_ZIN_EDGE_DISABLE   = 0,		/**< Clear function and gate function disable  */
	FJ_UDC_ZIN_EDGE_FALL_LOW  = 1,		/**< Crear function edge fall / gate function level low */
	FJ_UDC_ZIN_EDGE_RISE_HIGH = 2		/**< Crear function edge rise / gate function level high */
} FJ_UDC_ZIN_EDGE;

/** Up_Down counting callback function enable for compare counter match up down counter. */
typedef enum {
	FJ_UDC_COMP_CB_DISABLE = 0,			/**< compare counter callback disable  */
	FJ_UDC_COMP_CB_ENABLE  = 1			/**< compare counter callback enable */
} FJ_UDC_COMP_CB;

/** Up_Down counting callback function enable for under/over flow. */
typedef enum {
	FJ_UDC_UNDER_OVER_CB_DISABLE = 0,	/**< Under/over flow callback disable  */
	FJ_UDC_UNDER_OVER_CB_ENABLE  = 1	/**< Under/over flow callback enable */
} FJ_UDC_UNDER_OVER_CB;

/** Up_Down counting callback function enable for shift count direction. */
typedef enum {
	FJ_UDC_CNT_DIR_CB_DISABLE = 0,		/**< Shift Count direction callback disable  */
	FJ_UDC_CNT_DIR_CB_ENABLE  = 1		/**< Shift Count direction callback enable */
} FJ_UDC_CNT_DIR_CB;

/** Up_Down counting Z-terminal input control. */
typedef enum {
	FJ_UDC_ZIN_CTRL_DISABLE = 0,		/**< Z-terminal disable  */
	FJ_UDC_ZIN_CTRL_ENABLE	= 1			/**< Z-terminal enable */
} FJ_UDC_ZIN_CTRL;

/** Up_Down counting interrupt factor. */
typedef enum {
	FJ_UDC_INT_FACTOR_COMPARE,			/**< Match up down counter and compare counter */
	FJ_UDC_INT_FACTOR_OVER_FLOW,		/**< Counter over flow */
	FJ_UDC_INT_FACTOR_UNDER_FLOW,		/**< Counter under flow */
	FJ_UDC_INT_FACTOR_CNT_DIR			/**< Count direction shift */
} FJ_UDC_INT_FACTOR;

/** Up_Down counting detection edge for up down count mode. */
typedef enum {
	FJ_UDC_COUNT_EDGE_DISABLE = 0,		/**< Detection edge disable */
	FJ_UDC_COUNT_EDGE_FALL    = 1,		/**< Select fall edge */
	FJ_UDC_COUNT_EDGE_RISE    = 2,		/**< Select rise edge */
	FJ_UDC_COUNT_EDGE_BOTH    = 3		/**< Select fall/rise both */
} FJ_UDC_COUNT_EDGE;

/** Up_Down counting select count mode for Phase-Lag count mode.<br> */
typedef enum {
	FJ_UDC_PHASE_COUNT_SINGLE = 0,		/**< Select A-Terminal count  */
	FJ_UDC_PHASE_COUNT_DOUBLE			/**< Select A-Terminal and B-Terminal count */
} FJ_UDC_PHASE_COUNT;

/** Up_Down counting direction. */
typedef enum {
	FJ_UDC_COUNT_DIR_NO_INPUT = 0,		/**< No Input */
	FJ_UDC_COUNT_DIR_UP       = 1,		/**< Up count direction */
	FJ_UDC_COUNT_DIR_DOWN     = 2,		/**< Down count direction */
	FJ_UDC_COUNT_DIR_UNKNOWN  = 3		/**< UnKnown direction */
} FJ_UDC_COUNT_DIR;

/* ------------- */
/* I2C           */
/* ------------- */
/** I2C side type. */
typedef enum{
	FJ_I2C_TYPE_MASTER = 0,					/**< Master */
	FJ_I2C_TYPE_SLAVE						/**< Slave */
} FJ_I2C_TYPE;

/** I2C speed mode. */
typedef enum{
	FJ_I2C_SPEED_STANDARD = 0,				/**< Standard mode (100KHz) */
	FJ_I2C_SPEED_FAST,						/**< Fast mode (400KHz) */
	FJ_I2C_SPEED_FAST_PLUS,					/**< Fast mode plus (Not support) */
	FJ_I2C_SPEED_HIGH						/**< High-speed mode (Not support) */
} FJ_I2C_SPEED;

/** I2C slave address bit. */
typedef enum{
	FJ_I2C_ADDRESS_FORMAT_7_BIT = 0,		/**< 7 bit slave address */
	FJ_I2C_ADDRESS_FORMAT_10_BIT			/**< 10 bit slave address */
} FJ_I2C_ADDRESS_FORMAT;

/** I2C recieve from master Side */
typedef enum {
	FJ_I2C_RECEIVE_FROM_MASTER_NO_ACK = 0,	/**< Receive NO ACK from Master */
	FJ_I2C_RECEIVE_FROM_MASTER_ACK,			/**< Receive ACK from Master */
	FJ_I2C_RECEIVE_FROM_MASTER_DATA,		/**< Receive Data from Master */
	FJ_I2C_RECEIVE_FROM_MASTER_ADDRESS,		/**< Receive Address from Master.*/
	FJ_I2C_RECEIVE_FROM_MASTER_ERROR,		/**< Error occured.*/
} FJ_I2C_RECEIVE_FROM_MASTER;

/* ------------- */
/* UART          */
/* ------------- */
/** Baud rate selection. */
typedef enum{
	FJ_UART_BAUD_RATE_1200		= 1200,		/**< Baud rate:1200 */
	FJ_UART_BAUD_RATE_2400		= 2400,		/**< Baud rate:2400 */
	FJ_UART_BAUD_RATE_4800		= 4800,		/**< Baud rate:4800 */
	FJ_UART_BAUD_RATE_9600		= 9600,		/**< Baud rate:9600 */
	FJ_UART_BAUD_RATE_14400		= 14400,	/**< Baud rate:14400 */
	FJ_UART_BAUD_RATE_19200		= 19200,	/**< Baud rate:19200 */
	FJ_UART_BAUD_RATE_38400		= 38400,	/**< Baud rate:38400 */
	FJ_UART_BAUD_RATE_57600		= 57600,	/**< Baud rate:57600 */
	FJ_UART_BAUD_RATE_115200	= 115200,	/**< Baud rate:115200 */
	FJ_UART_BAUD_RATE_230400	= 230400,	/**< Baud rate:230400 */
	FJ_UART_BAUD_RATE_460800	= 460800,	/**< Baud rate:460800 */
	FJ_UART_BAUD_RATE_921600	= 921600,	/**< Baud rate:921600 */
	FJ_UART_BAUD_RATE_1000000	= 1000000,	/**< Baud rate:1000000 */
	FJ_UART_BAUD_RATE_2000000	= 2000000,	/**< Baud rate:2000000 */
	FJ_UART_BAUD_RATE_4000000	= 4000000	/**< Baud rate:4000000 */
} FJ_UART_BAUD_RATE;

/** UART mode selection. */
typedef enum{
	FJ_UART_MODE_NORMAL		= 0,			/**< Asynchronization normal mode */
	FJ_UART_MODE_MULTI						/**< Asynchronization multiprocessor mode */
} FJ_UART_MODE;

/** Transferring direction selection. */
typedef enum {
	FJ_UART_BIT_DIR_LSB_FIRST	= 0,		/**< LSB first */
	FJ_UART_BIT_DIR_MSB_FIRST				/**< MSB first */
} FJ_UART_BIT_DIR;

/** Stop bit length selection. */
typedef enum {
	FJ_UART_STOP_BIT_1		= 0,			/**< 1 bit */
	FJ_UART_STOP_BIT_2						/**< 2 bit */
} FJ_UART_STOP_BIT;

/** Parity bit type selection. */
typedef enum {
	FJ_UART_PARITY_BIT_NONE	= 0,			/**< Presence of parity addition: Parity none */
	FJ_UART_PARITY_BIT_EVEN,				/**< Presence of parity addition: Parity even number */
	FJ_UART_PARITY_BIT_ODD					/**< Presence of parity addition: Parity odd number */
} FJ_UART_PARITY_BIT;

/** Data length selection. */
typedef enum{
	FJ_UART_DATA_LENGTH_8	= 0,			/**< 8 bit length */
	FJ_UART_DATA_LENGTH_5, 					/**< 5 bit length */
	FJ_UART_DATA_LENGTH_6, 					/**< 6 bit length */
	FJ_UART_DATA_LENGTH_7, 					/**< 7 bit length */
} FJ_UART_DATA_LENGTH;

/** Callback information. */
typedef enum{
	FJ_UART_CB_INFO_SUCCESS = 0,			/**< success */
	FJ_UART_CB_INFO_OVERRUN_ERROR = 3,		/**< overrun error */
	FJ_UART_CB_INFO_FRAMING_ERROR,			/**< framing error */
	FJ_UART_CB_INFO_PARITY_ERROR,			/**< parity error */
	FJ_UART_CB_INFO_OVERFLOW_ERROR,			/**< overflow error (receive buffer of FJ_UART_CTRL) */
	FJ_UART_CB_INFO_DMA_ERROR,				/**< DMA status error */
	FJ_UART_CB_INFO_SYSTEM_ERROR,			/**< sytem error */
} FJ_UART_CB_INFO;

/* ------------- */
/* SLIMBus       */
/* ------------- */
/** SLIMBus channel */
typedef enum {
	FJ_SLIMBUS_CH0 = 0,				/**< Channel 0	*/
	FJ_SLIMBUS_CH1,					/**< Channel 1	*/
	FJ_SLIMBUS_CH2,					/**< Channel 2	*/
	FJ_SLIMBUS_CH3,					/**< Channel 3	*/
} FJ_SLIMBUS_CH;

/** Manager mode */
typedef enum {
	FJ_SLIMBUS_MANAGER_MODE_DISABLE = 0,		/**< Manager mode disable	*/
	FJ_SLIMBUS_MANAGER_MODE_ENABLE				/**< Manager mode enable	*/
} FJ_SLIMBUS_MANAGER_MODE;


/** Framer enable */
typedef enum {
	FJ_SLIMBUS_FR_DISABLE = 0,				/**< Framer disable	*/
	FJ_SLIMBUS_FR_ENABLE					/**< Framer enable	*/
} FJ_SLIMBUS_FR_EN;

/*=============================================================================
   Sturuct definition
===============================================================================*/
/* ------------- */
/* PWM           */
/* ------------- */
/** Type is defined to Callback function pointer for PWM */
typedef void (*PWM_CALLBACK)( UINT32 status );

/** Structure defining PWM control data for time. */
typedef struct{
	FJ_PERI_CH			ch;							/**< Channel number (range of 0..15) */
	FJ_PWM_MODE			mode;						/**< PWM operation mode */
	FJ_PWM_POLARITY		polarity;					/**< Output polarity */
	FJ_PWM_RESTART		restart_enable; 			/**< Restart enable */
	PWM_CALLBACK		pcallback1;					/**< The pointer of Callback Function for matching duty<br>
														 @remarks Specified callback function is called from task-independent portions.
													*/
	PWM_CALLBACK		pcallback2;					/**< The pointer of Callback Function for matching cycle<br>
														 @remarks Specified callback function is called from task-independent portions.
													*/
} FJ_PWM_CTRL;

/** Structure defining PWM stepping motor pulse and link data. */
typedef union {
	USHORT  hword;									/**< for USHORT access 			*/
	struct {
		USHORT	pulse_num	:10;					/**< number of pulse			*/
		USHORT				:5;
		USHORT	link		:1;						/**< link enable				*/
	} bit;
} FJ_PWM_STEP_PULSE_LINK;

/** Structure defining PWM stepping motor data. */
typedef struct {
	USHORT					cyc_num;				/**< number of cycle<br>
														 <br>
														 Please set the value that subtracted 1 from multiples of 4.<br>
														 For example, if the cycle is 49, a set value is 47 or 51. 
													*/
	USHORT					duty_num;				/**< number of duty				*/
	FJ_PWM_STEP_PULSE_LINK	pulse_link;				/**< number of pulse and link	*/
} FJ_PWM_STEP_DATA;


/** Structure defining PWM stepping motor data. */
typedef struct {
	UINT32					cycle_usec;					/**< cycle (usec)					*/
	UINT32					duty_usec;					/**< duty (usec)					*/
} FJ_PWM_STEP_PARAM_FOR_2CH;

/** PWM stepping motor control info */
typedef struct {
	FJ_PERI_CH			ch;					/**< Channel number (0 or 4 or 8 or 12)			*/
	FJ_PWM_CLK			clk;				/**< clock select								*/
	FJ_PWM_POLARITY		polarity; 			/**< output polarity							*/
	FJ_PWM_RESTART		restart_enable; 	/**< Restart enable								*/
	FJ_PWM_STP_CHAS		ch_assign;			/**< channel assignment							*/
	UCHAR				hold[4];			/**< hold setting at stop						*/
	FJ_PWM_FORCE		force_enable[4]; 	/**< force setting at stop						*/
	UCHAR				step_data_num;		/**< No. of stepping motor data					*/
	UCHAR				undr_run_data_num;	/**< No. of under run stepping motor data		*/
	FJ_PWM_STEP_DATA*	step_data;			/**< stepping motor data(cycle,duty,pulse,link)	*/
	FJ_PWM_STEP_DATA*	under_run_data;		/**< under run data(cycle,duty,pulse,link)		*/
	PWM_CALLBACK		pcallback;			/**< The pointer of Callback Function<br>
												 @remarks Specified callback function is called from task-independent portions.
												*/
} FJ_PWM_STEP_CTRL;

/** PWM stepping motor control info */
typedef struct {
	FJ_PERI_CH					ch;					/**< Channel number (0 or 4 or 8 or 12)			*/
	FJ_PWM_MODE					mode;				/**< PWM operation mode 						*/
	FJ_PWM_CLK					clk;				/**< clock select								*/
	FJ_PWM_POLARITY				polarity; 			/**< output polarity							*/
	UINT32						total_pulse_num;	/**< number of total_pulse			*/
	FJ_PWM_STEP_PARAM_FOR_2CH	step_data[4];		/**< stepping motor data(cycle,duty,pulse,link)	*/
													
	PWM_CALLBACK				pcallback1;			/**< The pointer of Callback Function for matching duty<br>
														 @remarks Specified callback function is called from task-independent portions.
													*/
	PWM_CALLBACK				pcallback2;			/**< The pointer of Callback Function for matching cycle<br>
														 @remarks Specified callback function is called from task-independent portions.
													*/
} FJ_PWM_CTRL_PARAM;
												 

/* ------------- */
/* SIO           */
/* ------------- */
/** Type is defined to Callback function pointer for serial I/O communication */
typedef void (*SIO_CALLBACK)( FJ_ERR_CODE result );

/** Structure defining serial I/O communication control data. */
typedef struct{
	FJ_SIO_TYPE		type;			/**< Master/Slave */
	FJ_SIO_MODE		mode;			/**< Transmit mode */
	FJ_SIO_BIT_DIR	bit_direction;	/**< Transmit bit direction, LSB first or MSB first */
	FJ_SIO_LENGTH	data_length;	/**< Data bit length */
	ULONG			baudrate;		/**< Baudrate (bps) */
	SIO_CALLBACK	pcallback;		/**< Callback function pointer.<br>
										 If pcallback was specified, @ref FJ_SIO_Transfer is asynchronization.<br>
										 If pcallback was not specified, @ref FJ_SIO_Transfer is synchronization.<br>
										 @remarks Specified callback function is called from task-independent portions.
									 */
} FJ_SIO_CTRL;

/** Structure defining serial I/O communication transfer control data. */
typedef struct{
	FJ_SIO_DATA_DIR direction;		/**< Direction of transfer. See @ref FJ_SIO_DATA_DIR */
	FJ_GPIO_PORT	cs_port;		/**< Port name of GPIO for chip select. See @ref FJ_GPIO_PORT */
	UCHAR			cs_status;		/**< Port status of GPIO for chip select.
									  <ul>
										  <li>0:@ref FJ_GPIO_STATUS_LOW
										  <li>1:@ref FJ_GPIO_STATUS_HIGH
									  </ul>
									*/
	BOOL			cs_restore_flg;	/**< CS restore control flag. TRUE: Restore, FALSE: No restore */
	VOID*			data_ptr;		/**< Data address. */
	UINT32			data_count;		/**< Count of data. */
} FJ_SIO_CTRL_TRANSFER;

/** Structure defining serial I/O communication control data(Full duplex). */
typedef struct{
	FJ_GPIO_PORT	cs_port;		/**< Port name of GPIO for chip select. See @ref FJ_GPIO_PORT */
	UCHAR			cs_status;		/**< Port status of GPIO for chip select.
									  <ul>
										  <li>0:@ref FJ_GPIO_STATUS_LOW
										  <li>1:@ref FJ_GPIO_STATUS_HIGH
									  </ul>
									*/
	BOOL			cs_restore_flg;	/**< CS restore control flag. TRUE: Restore, FALSE: No restore */
	VOID*			send_ptr;		/**< Send data address */
	VOID*			recv_ptr;		/**< Receive data address */
	UINT32			data_count;		/**< Count of data */
} FJ_SIO_CTRL_FULL_DUPLEX;

/* ------------- */
/* SPI           */
/* ------------- */
/** Type is defined to Callback function pointer for Serial Peripheral Interface */
typedef void (*SPI_CALLBACK)( FJ_ERR_CODE result );

/** Slave Select setting information. */
typedef struct{
	FJ_SPI_CONT_TRANS	cont_trans;				/**< Continuous Transfer enable. See @ref FJ_SPI_CONT_TRANS. */
	UCHAR				ssout[FJ_SPI_SS_MAX];	/**< Slave Select Output selection.<br>
														0: Do not select slave for TX/RX<br>
														1: Select Slave for TX/RX<br>
													 <br>
												 The maximum SSOUT is as follows.<br>
													ch0: 4<br>
													ch1: 4<br>
													ch2: 1 */
	UCHAR				sspol[FJ_SPI_SS_MAX];	/**< Slave Select Polarity selection.<br>
													sspol[0] = 0  : SSOUT/SSIN[0] is an active Low<br>
													sspol[0] = 1  : SSOUT/SSIN[0] is an active High<br>
													sspol[1~3] = 0: SSOUT[1~3] is an active Low<br>
													sspol[1~3] = 1: SSOUT[1~3] is an active High */
} FJ_SPI_SS_INFO;

/** Structure defining serial I/O communication control data. */
typedef struct{
	FJ_SPI_TYPE		type;			/**< Master/Slave */
	FJ_SPI_MODE		mode;			/**< Transmit mode */
	FJ_SPI_BIT_DIR	bit_direction;	/**< Transmit bit direction, LSB first or MSB first */
	FJ_SPI_LENGTH	data_length;	/**< Data bit length */
	ULONG			baudrate;		/**< Baudrate (bps).<br>
										 Maximum SCLK baudrate is as follows.<br>
											ch0: 108Mbps<br>
											ch1: 50Mbps<br>
											ch2: 50Mbps */
	UCHAR			fifo_wmark_tx;	/**< TX FIFO Watermark value.<br>
										 FIFO size is as follows.<br>
											ch0: 128 words<br>
											ch1: 64 words<br>
											ch2: 16 words */
	UCHAR			fifo_wmark_rx;	/**< RX FIFO Watermark value.<br>
										 In the case of master reception, please also set fifo_wmark_tx.<br>
										 FIFO size is as follows.<br>
											ch0: 128 words<br>
											ch1: 64 words<br>
											ch2: 16 words */
	UCHAR			dma_en;			/**< DMA enable.<br>
										 If set to 1, please specify the DMA channel in starting API.<br>
										 In addition, please also set to 1, if using the FJ_SPI_Transfer() with FJ_SPI_DATA_DIR_RECV mode 
										 or Dd_SPI_Start_Full_Duplex() by asynchronization, when DMA is not use. */
	UINT32			dma_recv_to;	/**< DMA Receive Timeout. (0 to 16,777,215)<br>
										 If the receive FIFO remains less than the RX watermark level for dma_recv_to(+1) HCLK cycles,
										 the DMA logic will issue a dma interrupt request to the CPU.<br>
										 In addition, please also set, if using the FJ_SPI_Transfer() with FJ_SPI_DATA_DIR_RECV mode 
										 or Dd_SPI_Start_Full_Duplex() by asynchronization, when DMA is not use. */
	SPI_CALLBACK	pcallback;		/**< Callback function pointer.<br>
										 If pcallback was specified, @ref FJ_SPI_Transfer is asynchronization.<br>
										 If pcallback was not specified, @ref FJ_SPI_Transfer is synchronization.<br>
										 @remarks Specified callback function is called from task-independent portions.
									 */
} FJ_SPI_CTRL;

/** Structure defining serial I/O communication transfer control data. */
typedef struct{
	FJ_SPI_DATA_DIR	direction;		/**< Direction of transfer. See @ref FJ_SPI_DATA_DIR */
	FJ_SPI_SS_INFO	ss_info;		/**< Slave Select setting information. See @ref FJ_SPI_SS_INFO. */
	VOID*			data_ptr;		/**< Data address. */
	UINT32			data_count;		/**< Count of data. */
} FJ_SPI_CTRL_TRANSFER;

/** Structure defining serial I/O communication control data(Full duplex). */
typedef struct{
	FJ_SPI_SS_INFO	ss_info;		/**< Slave Select setting information. See @ref FJ_SPI_SS_INFO. */
	VOID*			send_ptr;		/**< Send data address */
	VOID*			recv_ptr;		/**< Receive data address */
	UINT32			data_count;		/**< Count of data */
} FJ_SPI_CTRL_FULL_DUPLEX;


/* ------------- */
/* Gyro IF       */
/* ------------- */
/** Type is defined to Callback function pointer for Gyro I/F */
typedef void (*GYROIF_CALLBACK)( FJ_ERR_CODE result );

/** Gyro data control. */
typedef struct{
	FJ_SPI_MODE			mode;				/**< SPI transmit mode */
	FJ_SPI_BIT_DIR		bit_direction;		/**< SPI transmit bit direction, LSB first or MSB first */
	FJ_SPI_LENGTH		data_length;		/**< SPI Data bit length */
	ULONG				baudrate;			/**< SPI baudrate (bps). Maximum baudrate is 50Mbps. */
	UCHAR				fifo_wmark_tx;		/**< TX FIFO Watermark value. FIFO size is 16 words.*/
	UCHAR				fifo_wmark_rx;		/**< RX FIFO Watermark value. FIFO size is 16 words.<br>
												 In the case of master reception, please also set fifo_wmark_tx. */
	FJ_SPI_SS_INFO		ss_info;			/**< SPI Slave Select setting information. See @ref FJ_SPI_SS_INFO.<br>
												 Please specify FJ_SPI_CONT_TRANS_INACT_FIFO_EMPTY to cont_trans. */
	ULONG				send_addr;			/**< SPI send buffer address. Buffer size is same as data_count.<br>
												 It includes register address and dummy data for data receive.<br>
												 The first byte should have been setting the Gyro data read address.<br>
												 Please prepare buffer size of <data_count * data_length>.
												 In fact it is sent repeatedly by the number of data_count the first byte to the Gyro sensor. */
	ULONG				recv_addr;			/**< SPI receive buffer address. Please specify the top address of ring buffer for read data.<br>
												 Please prepare buffer size of <total_data_count * data_count * data_length>. */
	UINT32				data_count;			/**< Count of Gyro read data per Gyro interrupt (1 to 16) */
	UINT32				total_data_count;	/**< Total count of Gyro read data buffer (1 to 65536).<br>
												 When data reaches total_data_count, return address to recv_addr, and returns data count to 0. */
	UINT32				dma_recv_to;		/**< DMA Receive Timeout. (0 to 16,777,215)<br>
												 The recommended setting is the maximum value.*/
	GYROIF_CALLBACK		pcallback;			/**< Callback function pointer when error interrupt. */
} FJ_GYROIF_DATA_CTRL;

/** Gyro time stamp control. */
typedef struct{
	FJ_GYROIF_TMR_CLK	timer_clk;			/**< 64bit Timer count clock select. */
	ULONG				buff_addr;			/**< Time stamp buffer address. Please specify the top address of ring buffer for Time stamp.
												 Please prepare buffer size of <total_data_count * 2byte(lower 32bit and upper 32bit)>. */
	UINT32				total_data_count;	/**< Total count of time stamp buffer (1 to 65536).<br>
												 When data reaches total_data_count, return address to buff_addr, and returns data count to 0. */
	GYROIF_CALLBACK		pcallback;			/**< Callback function pointer when error interrupt. */
} FJ_GYROIF_STAMP_CTRL;

/** Gyro data read control. */
typedef struct{
	FJ_GYROIF_DATA_CTRL		data_ctrl;		/**< Gyro data control data. */
	FJ_GYROIF_STAMP_CTRL	stamp_ctrl;		/**< Gyro time stamp control data. */
} FJ_GYROIF_READ_CTRL;

/* ---------------------- */
/* External Interrupt     */
/* ---------------------- */
/** Type is defined to Callback function pointer for External Interrupt */
typedef void (*EXT_INT_CALLBACK)( FJ_GPIO_PORT port );

/** External Interrupt config */
typedef struct {
	UINT32					int_priority;	/**< Interrupt priority (0-31) */
	FJ_EXT_INT_TRIGGER_TYPE	trigger_type;	/**< Trigger type for interrupt detect. See FJ_EXT_INT_TRIGGER_TYPE. */
	FJ_EXT_INT_TARGET_CPU	cpu_target;		/**< Interrupt Processor Targets. See FJ_EXT_INT_TARGET_CPU. */
	EXT_INT_CALLBACK		int_callback;	/**< Call back function pointer<br>
												 @remarks Specified callback function is called from task-independent portions.
											 */
} FJ_EXT_INT_CONFIG;

/* ------------- */
/* I2C           */
/* ------------- */
/** Next action for slave */
typedef struct {
	BYTE mode;			/**< slave mode. */
						/**< @ref FJ_I2C_SEND_DATA */
						/**< @ref FJ_I2C_SEND_ACK */
						/**< @ref FJ_I2C_SEND_NO_ACK */
	BYTE data;			/**< Send data */
}FJ_I2C_ACTION_SLAVE;

/** Type is defined to Callback function pointer for I2C */
typedef FJ_I2C_ACTION_SLAVE (*I2C_CALLBACK)( FJ_PERI_CH ch, FJ_I2C_RECEIVE_FROM_MASTER receive, BYTE data);

/** Structure defining control data for I2C. */
typedef struct{
	FJ_I2C_TYPE				type;			/**< I2C side type */
	FJ_I2C_SPEED			speed;			/**< I2C speed mode */
	FJ_I2C_ADDRESS_FORMAT	address_format;	/**< I2C slave address bit */
	UINT16					slave_address;	/**< I2C slave address */
}FJ_I2C_CTRL;

/** Data buffer struct*/
typedef struct {
	BYTE*	data;							/**< Send or receive data buffer	*/
	USHORT	data_length;					/**< Data buffer's length*/
}FJ_I2C_TRANSFER_DATA_BUFFER;

/** I2C communication data struct */
typedef struct {
	FJ_I2C_TRANSFER_DATA_BUFFER	slave_register_address;		/**< The pointer of slave register address. <br>
																 They are sent to slave at first.*/
	FJ_I2C_TRANSFER_DATA_BUFFER	trn_data;					/**< Send/Receive data */
	BYTE	dirction;										/**< 0: Send data to slave.<br>
																 1: Receive data from slave.*/
	UINT32	timeout_threshold;								/**< communication timeout value */
	UINT32  retry_num;										/**< Retry num when communication error occur */
}FJ_I2C_TRANSFER_DATA;

/* ------------- */
/* UDC			 */
/* ------------- */
/** Type is defined to Callback function pointer for up down counter. */
typedef void (*UDC_CALLBACK)( FJ_PERI_CH ch, FJ_UDC_INT_FACTOR factor );

/** Structure defining control data for up down counter. */
typedef struct {
	USHORT					up_down_counter;		/**< Up/Down counter value.<br>						*/
	USHORT					comp_counter;			/**< Reload/Compare counter value.<br>				*/
	FJ_UDC_COMP_CLEAR		comp_clear;				/**< Compare clear enable(only up count).<br>		*/
	FJ_UDC_RELOAD			reload;					/**< Reload enable(only down count).<br>			*/
	FJ_UDC_ZIN_COUNT		zin_mode;				/**< Function of ZIN terminal.<br>					*/
	FJ_UDC_ZIN_EDGE			zin_edge;				/**< Detect edge of ZIN terminal.<br>				*/
	FJ_UDC_COMP_CB			comp_cb_enable;			/**< Compare callback enable.<br>					*/
	FJ_UDC_UNDER_OVER_CB	under_over_cb_enable;	/**< Under/Over flow callback enable.<br>			*/
	FJ_UDC_CNT_DIR_CB		cnt_dir_cb_enable;		/**< Count direction shift callback enable.<br>		*/
	UDC_CALLBACK			pCallBack;				/**< callback function pointer.<br>
									 					@remarks Specified callback function is called from task-independent portions.
													*/
} FJ_UDC_CTRL;

/* ------------- */
/* UART          */
/* ------------- */
/** Structure defining UART communication control data. */
typedef struct{
	FJ_UART_BAUD_RATE		baud_rate;			/**< Baud rate */
	FJ_UART_MODE			mode;				/**< UART Mode, Normal or Multi */
	FJ_UART_BIT_DIR			bit_direction;		/**< Transferring Direction, LSB first or MSB first */
	FJ_UART_STOP_BIT		stop_bit_length;	/**< Stop bit length */
	FJ_UART_PARITY_BIT		parity_bit;			/**< Parity bit type */
	FJ_UART_DATA_LENGTH		data_length;		/**< Data Length */
	BOOL					inv_nrz;			/**< Inversion serial data format (FALSE:NRZ, TRUE:NRZ-Inversion) */
	BOOL					flow_enable;		/**< Hardware flow control enable (FALSE:Disable, TRUE:Enable) */
	UCHAR*					receive_buff_addr;	/**< Buffering address of the received data (ring buffer) */
	UINT32					receive_buff_size;	/**< Buffering size of the received data */
} FJ_UART_CTRL;

/** Structure defining UART debug configuration data. */
typedef struct{
	BOOL					overwrite;			/**< UART buffer overwrite (TRUE:overwrite, FALSE:not overwrite) */
	BOOL					suspend;			/**< UART buffer suspend (TRUE:suspend, FALSE:not suspend) */
} FJ_UART_DEBUG_CONFIG;

/* ------------- */
/* Timer32       */
/* ------------- */
/** Structure defining control data for 32bit reload timer. */

/** CSR of reload timer */
typedef union {
	ULONG		word;			/**< for ULONG access */
	struct {
		ULONG	trg_fixed0	:1;	/**< TRG is "0" fiexed ( 1 )			*/
		ULONG	cnte		:1;	/**< Timer counter Enable ( 0 or 1 )	*/
		ULONG	uf_fixed1	:1;	/**< UF is "1" fiexed ( 1 )				*/
		ULONG	inte		:1;	/**< Interrupt Enable ( 0 or 1 )		*/
		ULONG	reld		:1;	/**< Reload Enable ( 0 or 1 )			*/
		ULONG	outl		:1;	/**< Out Level ( 0 or 1 )				*/
		ULONG				:1;	/**< Reserved							*/
		ULONG	mod			:3;	/**< Mode select ( 0 to 7 )				*/
		ULONG	csl			:2;	/**< Count clock select ( 0 to 3 )		*/
		ULONG	rels		:1;	/**< Reload register select ( 0 or 1 )	*/
		ULONG				:3;	/**< Reserved							*/
		ULONG				:16;/**< Reserved							*/
    } bit;						/**< for bit access						*/
} FJ_TMR32_CTRL_CSR;

/** Control information */
typedef struct {
	FJ_TMR32_CTRL_CSR	csr;			/**< Control Status (Please set TMCSR register value.)	*/
	ULONG				rlr1;			/**< Reload Timer1 (Please set TMRLR1 register value.)	*/
	ULONG				rlr2;			/**< Reload Timer2 (Please set TMRLR2 register value.)	*/
} FJ_TMR32_CTRL;

/* ------------- */
/* SLIMBus       */
/* ------------- */
/** Type is defined to Callback function pointer for SLIMBus */
typedef VOID (*SLIMBUS_CALLBACK)( UCHAR ch, INT32 status );

/** Structure defining control data for SLIMBus. */
/** SLIMBus control data. */
typedef struct{
	FJ_SLIMBUS_CH				ch;					/**< SLIMBus channel	*/
	FJ_SLIMBUS_MANAGER_MODE		manager_mode;		/**< Manager mode		*/
	FJ_SLIMBUS_FR_EN			fr_en;				/**< framer enable		*/
	UCHAR						src_thr;			/**< source threshold (0 to 63)	*/
	UCHAR						sink_thr;			/**< sink threshold (0 to 63)	*/
	SLIMBUS_CALLBACK			manager_int_cb;		/**< Callback function pointer when manager interrupt is generated.<br>
														 The following are set to argument status of callback.<br>
														   bit1: RX_INT(Set HIGH when enabled and RX_FIFO is not empty.)<br>
														   bit2: TX_INT(Set HIGH when enabled and TX_FIFO becomes empty (after transmitting all messages).)<br>
														   bit3: TX_ERR(Set HIGH when enabled and when message from TX_FIFO is not transmitted successfully.)<br>
														   bit4: SYNC_LOST(Set HIGH when enabled and when any of main synchronization bits (F_SYNC, SF_SYNC,M_SYNC) toggles into LOW, and the IP is not detached due to REPORT_ABSENT message.)<br>
														   bit7: PORT_INT(HIGH when bit of PINT register is HIGH.) */
	SLIMBUS_CALLBACK			data_port_int_cb;	/**< Callback function pointer when data port interrupt is generated.<br>
														 The following are set to argument status of callback.<br>
														   bit0: ACT_INT(Channel activation)<br>
														   bit1: CON_INT(Channel content definition)<br>
														   bit2: CHAN_INT(Channel definition)<br>
														   bit4: OVF_ERR(Data Port FIFO overflow)<br>
														   bit5: UND_INT(Data port FIFO underrun) */
} FJ_SLIMBUS_CTRL;

/*@} --- end of fj_peripheral_definition */
///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_peripheral_api
@{*/
/* ------------- */
/* PWM           */
/* ------------- */
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function acquire semaphore of PWM timer channel.
@param [in]	ch	Channel number. (range of 0..15)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Semaphore acquisition NG.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_PWM_Open( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function  acquire semaphore of PWM timer channel for stepping motor utility.
@param [in]	ch	Channel number. (0 or 4 or 8 or 12)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Semaphore acquisition NG.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_PWM_Open_Step( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function  acquire semaphore of PWM timer channel for stepping motor utility.
@param [in]	ch_1st	Channel number. (0 or 4 or 8 or 12)
@param [in]	ch_2nd	Channel number. (0 or 4 or 8 or 12)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Semaphore acquisition NG.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_PWM_Open_Step_2ch( FJ_PERI_CH ch_1st,  FJ_PERI_CH ch_2nd);


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function  calculate conutor clock
@param [in]	usec				target time
@param [in]	clk_sel				clk_sel value
@param [in]	cycle				cycle value
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Fail - FJ error code according to the specific error.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_PWM_Calculate_Clock_Sel(ULONG usec, UCHAR* const clk_sel, USHORT* const cycle);


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function  calculate cycle value(resister value fo PWM)
@param [in]	usec_array			target period [usec]
@param [in]	clk_sel				clk_sel value
@param [in]	step_data			resister value of PWM
@param [in]	step_data_num		the number of array
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Fail - FJ error code according to the specific error.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_PWM_Calculate_Array_Clock_Sel(ULONG usec_array[][2], UCHAR* const clk_sel, FJ_PWM_STEP_DATA step_data[], UCHAR step_data_num);


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function  set buffer data window for DMA transfer of PWM data
@param [in]	ch	Channel number. (0 or 4 or 8 or 12)
@param [in]	usec_array			target period [usec]
@param [in]	step_data			resister value of PWM
@param [in]	data_num			the number of data
@param [in]	pulse_end_flag		0: continue to output pwm, 1: the end of pwm pulse
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Fail - FJ error code according to the specific error.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_PWM_Set_Buffer_Data_Window(FJ_PERI_CH ch, ULONG usec_array[][2], FJ_PWM_STEP_DATA step_data[], UCHAR data_num, UCHAR pulse_end_flag);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function  set buffer data window for DMA transfer of PWM data
@param [in]	ch	Channel number. (0 or 4 or 8 or 12)
@param [in]	step_data			resister value of PWM
@param [in]	data_num			the number of data
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Fail - FJ error code according to the specific error.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_PWM_Set_Buffer_Data_Window_by_Step_Data(FJ_PERI_CH ch, FJ_PWM_STEP_DATA step_data[], UCHAR data_num);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets PWM timer by cycle and duty ratio.<br><br>
@image html fj_peripheral_pwm_set_by_ratio.png
@param [in] ctrl		PWM control data. See @ref FJ_PWM_CTRL.
@param [in] cycle_hz	Cycle frequency. (hertz order)
@param [in] duty_ratio	Duty ratio. (percent order)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Fail - FJ error code according to the specific error.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks
		 - When both pcallback1 and pcallback2 are set and the time of cycle_hz passes during processing of pcallback1, pcallback2 is not called.<br>
		 - The limitation of cycle_hz is as follows.<br>
		   &nbsp;&nbsp;Maximum frequency : 1,000,000 Hz<br>
		   &nbsp;&nbsp;Minimum frequency : 16 Hz<br>
		 - Specified callback function is called from task-independent portions.<br>
		   See @ref FJ_PWM_CTRL.<br>
*/
extern FJ_ERR_CODE FJ_PWM_Set_By_Ratio( FJ_PWM_CTRL const* const ctrl, UINT32 cycle_hz, UINT32 duty_ratio );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets PWM timer by period time.<br><br>
@image html fj_peripheral_pwm_set_by_time.png
@param [in] ctrl		PWM control data. See @ref FJ_PWM_CTRL.
@param [in] time1		The 1st time to set. (usec order)
@param [in] time2		The 2nd time to set. (usec order)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Fail - FJ error code according to the specific error.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks
		 - When both pcallback1 and pcallback2 are set and the time of time2 passes during processing of pcallback1, pcallback2 is not called.
		 - The total limitation of time1 and time2 is as follows.<br>
		   &nbsp;&nbsp;Maximum time : 65535 us<br>
		   &nbsp;&nbsp;Minimum time : 2 us<br>
		 - Specified callback function is called from task-independent portions.<br>
		   See @ref FJ_PWM_CTRL.<br>
*/
extern FJ_ERR_CODE FJ_PWM_Set_By_Time( FJ_PWM_CTRL const* const ctrl, ULONG time1, ULONG time2 );


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets PWM parameter (2ch simultaneously).<br><br>
@param [in] ctrl_a		PWM control data. See @ref FJ_PWM_CTRL_PARAM.
@param [in] ctrl_b		PWM control data. See @ref FJ_PWM_CTRL_PARAM.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Fail - FJ error code according to the specific error.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks
*/
extern FJ_ERR_CODE FJ_PWM_Set_2ch( FJ_PWM_CTRL_PARAM const* const ctrl_a, FJ_PWM_CTRL_PARAM const* const ctrl_b);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets PWM timer stepping motor.<br><br>
@param [in] ctrl	PWM stepping motor control data. See @ref FJ_PWM_STEP_CTRL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Fail - FJ error code according to the specific error.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks
		 - This API uses the following DMA channel.<br>
		   <br>
		   &nbsp;&nbsp;PWM CH0  : HDMAC0 CH4<br>
		   &nbsp;&nbsp;PWM CH4  : HDMAC0 CH5<br>
		   &nbsp;&nbsp;PWM CH8  : HDMAC0 CH6<br>
		   &nbsp;&nbsp;PWM CH12 : HDMAC0 CH7<br>
		   <br>
		 - Specified callback function is called from task-independent portions.<br>
		   See @ref FJ_PWM_STEP_CTRL.<br>
*/
extern FJ_ERR_CODE FJ_PWM_Set_Step( FJ_PWM_STEP_CTRL const* const ctrl );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function start PWM timer.
@param [in]	ch	Channel number. (range of 0..15)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Fail - FJ error code according to the specific error.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks The following status is notified by the callback.<br>
	@code
	FJ_PWM_STATUS_TRG_INPUT			: PWM Status is Trigger input
	FJ_PWM_STATUS_CYCLE_MATCH		: PWM Status is Matching cycle
	FJ_PWM_STATUS_DUTY_MATCH		: PWM Status is Matching duty
	FJ_PWM_STATUS_CYC_DUTY_MATCH	: PWM Status is Matching duty
	FJ_PWM_STATUS_UNDER_RUN			: PWM Status is Under run error for stepping motor
	FJ_PWM_STATUS_FORCE_STOP		: PWM Status is Force stop for stepping motor
	FJ_PWM_STATUS_AUTO_STOP			: PWM Status is Auto stop for stepping motor
	@endcode
*/
extern FJ_ERR_CODE FJ_PWM_Start( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function start PWM timer (2ch simultaneously).
@param [in]	ch_a	Channel number. (range of 0..15)
@param [in]	ch_b	Channel number. (range of 0..15)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Fail - FJ error code according to the specific error.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks The following status is notified by the callback.<br>
	@code
	FJ_PWM_STATUS_TRG_INPUT			: PWM Status is Trigger input
	FJ_PWM_STATUS_CYCLE_MATCH		: PWM Status is Matching cycle
	FJ_PWM_STATUS_DUTY_MATCH		: PWM Status is Matching duty
	FJ_PWM_STATUS_CYC_DUTY_MATCH	: PWM Status is Matching duty
	FJ_PWM_STATUS_UNDER_RUN			: PWM Status is Under run error for stepping motor
	FJ_PWM_STATUS_FORCE_STOP		: PWM Status is Force stop for stepping motor
	FJ_PWM_STATUS_AUTO_STOP			: PWM Status is Auto stop for stepping motor
	@endcode
*/
extern FJ_ERR_CODE FJ_PWM_2ch_Start( FJ_PERI_CH ch_a, FJ_PERI_CH ch_b );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function stop PWM timer.
@param [in]	ch	Channel number. (range of 0..15)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Fail - FJ error code according to the specific error.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_PWM_Stop( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function close PWM timer.
@param [in]	ch	Channel number. (range of 0..15)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Fail - FJ error code according to the specific error.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_PWM_Close( FJ_PERI_CH ch );

/* ------------- */
/* Timer32       */
/* ------------- */
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function acquire semaphore of Timer32 channel.
@param [in]	ch		Channel number. (range of 0..13)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Semaphore acquisition NG.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks
		 - Please be sure to check the channel number which is not used inside FJ.
*/
extern FJ_ERR_CODE FJ_TMR32_Open( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets Timer32. (assignment 32bit reload timer)
@param [in]	ch			Channel number. (range of 0..13)
@param [in] usec		The time to set. (usec order)
@param [in] pcallback	The pointer of Callback Function.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks Specified callback function is called from task-independent portions.<br>
*/
extern FJ_ERR_CODE FJ_TMR32_Set( FJ_PERI_CH ch, ULONG usec, VP_CALLBACK pcallback );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets Timer32 for pwm. (assignment 32bit reload timer)
@param [in]	reload_ch		Channel number. (range of 0..13)
@param [in]	pwm_ch			Channel number. (ch4 or ch8)
@param [in]	mode			0:for 60fps, 1:for 240fps
@param [in] pcallback	The pointer of Callback Function.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks Specified callback function is called from task-independent portions.<br>
*/
extern FJ_ERR_CODE FJ_TMR32_Set_Pwm( FJ_PERI_CH reload_ch, FJ_PERI_CH pwm_ch, UCHAR	mode, VP_CALLBACK pcallback);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets Timer32 control parameters. (assignment 32bit reload timer)
@param [in]	ch			Channel number. (range of 0..13)
@param [in]	tmr32_ctrl	Timer32 Control Parameters. See @ref FJ_TMR32_CTRL.
@param [in] pcallback	The pointer of Callback Function.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks Specified callback function is called from task-independent portions.<br>
*/
extern FJ_ERR_CODE FJ_TMR32_Ctrl( FJ_PERI_CH ch, FJ_TMR32_CTRL const* const tmr32_ctrl, VP_CALLBACK pcallback );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set to TOUT enable. (assignment 32bit reload timer ch3)
@param [in]	ch			Channel number. (only 3)
@param [in] enable		TOUT enable. (=TRUE:enable, =FALSE:disable)
@param [in] out_level	Out level. (0/1)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occured.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks
	By [out_level] and [reload(FJ_TMR32_Start() parameter)], TOUT will change.<br>
	- Case of reload=0(1shot)<br>
		out_level=0 : The output is "High" from Start trigger to Underflow.<br>
		out_level=1 : The output is "Low" from Start trigger to Underflow.<br><br>
@image html fj_peripheral_tout_1shot.png
	- Case of reload=1(reload)<br>
		out_level=0 : The output is "Low" at the Start trigger.<br>
		out_level=1 : The output is "High" at the Start trigger.<br>
		- After that, the output is inverted by the Underflow.(toggle output)<br><br>
@image html fj_peripheral_tout_reload.png
*/
extern FJ_ERR_CODE FJ_TMR32_Tout_Ctrl( FJ_PERI_CH ch, BOOL enable, UINT32 out_level );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function start Timer32. (assignment 32bit reload timer)
@param [in]	ch		Channel number. (range of 0..13)
@param [in] reload	Operation mode. (=TRUE:Reload mode, =FALSE:One-shot mode)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occured.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_TMR32_Start( FJ_PERI_CH ch, BOOL reload );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function stops Timer32. (assignment 32bit reload timer)
@param [in]	ch		Channel number. (range of 0..13)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occured.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_TMR32_Stop( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function closes Timer32. (assignment 32bit reload timer)
@param [in]	ch		Channel number. (range of 0..13)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occured.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_TMR32_Close( FJ_PERI_CH ch );

/* ------------- */
/* Interface     */
/* ------------- */
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function acquire semaphore of I2C channel.
@param [in]	ch	Channel number. (range of 0..2)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Semaphore acquisition NG.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_I2C_Open( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function close I2C channel.
@param [in]	ch	Channel number. (range of 0..2)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Semaphore acquisition NG.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_I2C_Close( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets I2C parameters.
@param [in]	ch			Channel number. (range of 0..2)
@param [in] i2c_ctrl	Slave side information. See @ref FJ_I2C_CTRL
@param [in] pcallback	The pointer of Callback Function.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_I2C_Ctrl( FJ_PERI_CH ch, FJ_I2C_CTRL i2c_ctrl, I2C_CALLBACK pcallback );


/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Send or receive data to slave side by I2C.
If change of frequency is required, please set first by FJ_I2C_Set_ApproxClock().
@param [in]	ch				Channel number. (range of 0..2)
@param [in,out] trans_ctrl	Send or receive data array. See @ref FJ_I2C_TRANSFER_DATA
@param [in] length			data's lenght
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@retval FJ_ERR_NG_COMMUNICATON	Communication error.
@remarks
	 - This API Sample code is as follows.<br>
	@code
[Sample Code]
	FJ_I2C_CTRL slave_infor;
	FJ_I2C_TRANSFER_DATA  data[2]; //Transfer data
	BYTE slave_register_address[2] = {
		0x30,
		0x40
	};//Register address
	BYTE send_data[10] = {
		0,1,2,3,4,5,6,7,8,9
	};
	BYTE receive_data[15] ; //Receive data buffer
	FJ_ERR_CODE ret;

	ret = FJ_I2C_Open( FJ_PERI_CH_0 );
	if( ret != FJ_ERR_OK ) {
		// error correction code here.
		return ;
	}

	memset(&slave_infor, 0, sizeof(slave_infor));
	slave_infor.slave_address = 0x80; //Set slave address
	slave_infor.address_format = FJ_I2C_ADDRESS_FORMAT_7_BIT; //Set to 7 bit
	slave_infor.speed = FJ_I2C_SPEED_FAST;
	slave_infor.type = FJ_I2C_TYPE_MASTER;

	ret = FJ_I2C_Ctrl( FJ_PERI_CH_0, slave_infor, NULL );
	if( ret != FJ_ERR_OK ) {
		// error correction code here.
		ret = FJ_I2C_Close( FJ_PERI_CH_0 );
		if( ret != FJ_ERR_OK ) {
			// error correction code here.
			:
		}
		return ;
	}

	data[0].slave_register_address.data_length = 1;
	data[1].slave_register_address.data_length = 1;
	data[0].slave_register_address.data = &slave_register_address[0];
	data[1].slave_register_address.data = &slave_register_address[1];

	data[0].trn_data.data = send_data; //Write data buffer
	data[1].trn_data.data = receive_data; //Read data buffer
	data[0].trn_data.data_length = sizeof(send_data); //Send data size
	data[1].trn_data.data_length = sizeof(receive_data); //Receive data size

	data[0].dirction = 0; //Write data to slave
	data[1].dirction = 1; //Read data from slave

	data[0].retry_num =  3; //Set retry num to 3
	data[1].retry_num =  3; //Set retry num to 3
	data[0].timeout_threshold = 3; //Set timeout to 3ms
	data[1].timeout_threshold = 3; //Set timeout to 3ms

	ret = FJ_I2C_Transfer( FJ_PERI_CH_0, data, 2 );
	if( ret == FJ_ERR_OK ) {
		//Commnication OK
	}
	else{
		// error correction code here.
		:
	}

	ret = FJ_I2C_Close( FJ_PERI_CH_0 );
	if( ret != FJ_ERR_OK ) {
		// error correction code here.
		return ;
	}

	@endcode
*/
extern FJ_ERR_CODE FJ_I2C_Transfer(FJ_PERI_CH ch, FJ_I2C_TRANSFER_DATA trans_ctrl[], UINT32 length);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function set I2C serial clock frequency.<br>
@param [in]  ch			I2C's channel no (range of 0..2)
@param [in]  req_clock	Request clock of I2C serial clock frequency (KHz)
@param [out] act_clock	Actual clock of I2C serial clock frequency (KHz)
@param [in]  under_flag	Approximation flag (=TRUE:Under, =FALSE:Over)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks Calculating the actual clock from the request clock based on the I2C specification.<br><br>
		 ex)<br>
		 req_clock=100KHz under_flag=TRUE  -> When I2C Clock=27000KHz. act_clock=96KHz.<br>
		 req_clock=100KHz under_flag=TRUE  -> When I2C Clock=13500KHz. act_clock=99KHz.<br>
 		 req_clock=100KHz under_flag=FALSE -> When I2C Clock=27000KHz. act_clock=101KHz.<br>
  		 req_clock=100KHz under_flag=FALSE -> When I2C Clock=13500KHz. act_clock=109KHz.<br>
*/
extern FJ_ERR_CODE FJ_I2C_Set_ApproxClock(FJ_PERI_CH ch, UINT32 req_clock, UINT32* act_clock, BOOL under_flag);

/* ------------- */
/* UART          */
/* ------------- */
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function will start the debug UART for the system.
@param [in]	ch		UART channel number. (range of 0..7)
@param [in]	dma_ch	DMA channel number. (Send only)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks
	- If use ch5, please set following CHIPTOP settings(Please see LSI specification)<br>
<table style="text-align:left;" border="1" cellpadding="2" cellspacing="2">
    <tr>
      <th style="background-color: rgb(204, 255, 255);">Port</th>
      <th style="background-color: rgb(204, 255, 255);">Settings</th>
    </tr>
    <tr>
      <td>
        PD[5:3]<br>
        (FPSIN/OUT/CK:5)
      </td>
      <td>
        VDOSEL.VMDHS = 3<br>
        EPCRD[3] = 1<br>
        EPCRD[4] = 1<br>
        EPCRD[5] = 0<br>
        DDRD[5] = 0<br>
      </td>
    </tr>
</table>
*/
extern FJ_ERR_CODE FJ_UART_Debug_Start( FJ_PERI_CH ch, FJ_USIO_DMA_CH dma_ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function will stop the debug UART for the system.
@param [in]	ch	Channel number. (range of 0..7)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_UART_Debug_Stop( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets character on UART.
@param [in] *Buf The pointer of string to be filled.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred. (No recieve data)
@retval FJ_ERR_UART_OVERFLOW_ERROR	When the receive data storage area overflows, "Overflow Error" is sent back.
@remarks
	- Can be used for FJ_UART_Debug_Start running.
*/
extern FJ_ERR_CODE	FJ_UART_Debug_Get_Char( CHAR *Buf );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function Saves the Uart memory as a binary file.
@param [in] file_name file name.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM Input Parameter error.
@retval FJ_ERR_WRITE_FAIL_TO_MEDIA Failed to write to the media.
 */
extern FJ_ERR_CODE FJ_UART_Debug_Save( const CHAR* file_name );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function Set the configure of UART Debug.
@param [in]	config	UART debug Configuration table. See @ref FJ_UART_DEBUG_CONFIG.
@retval FJ_ERR_OK Normal end.
@retval FJ_ERR_NG Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM Input Parameter error.
 */
extern FJ_ERR_CODE FJ_UART_Debug_Configure( FJ_UART_DEBUG_CONFIG const* config );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function acquire semaphore of UART channel.
@param [in]	ch	Channel number. (range of 0..7)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Semaphore acquisition NG.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks
	- If use ch5, please set following CHIPTOP settings(Please see LSI specification)<br>
<table style="text-align:left;" border="1" cellpadding="2" cellspacing="2">
    <tr>
      <th style="background-color: rgb(204, 255, 255);">Port</th>
      <th style="background-color: rgb(204, 255, 255);">Settings</th>
    </tr>
    <tr>
      <td>
        PD[5:3]<br>
        (FPSIN/OUT/CK:5)
      </td>
      <td>
        VDOSEL.VMDHS = 3<br>
        EPCRD[3] = 1<br>
        EPCRD[4] = 1<br>
        EPCRD[5] = 0<br>
        DDRD[5] = 0<br>
      </td>
    </tr>
</table>
*/
extern FJ_ERR_CODE FJ_UART_Open( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function close a communication channel of the UART.
@param [in]	ch	Channel number. (range of 0..7)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occurred. (Not using a specified channel)
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks
	- The following is a sample code for this API.
	@code
	[Sample Code]
	static UCHAR gUART_ch = 0;

	VOID sample_uart_close()
	{
		FJ_UART_Stop( gUART_ch );		// stop sending and receiving.
		FJ_UART_Close( gUART_ch );
	}
	@endcode
*/
extern FJ_ERR_CODE FJ_UART_Close( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets UART operation for the specified channel.
@param [in]	ch			Channel number. (range of 0..7)
@param [in]	uart_ctrl	UART Control table. See @ref FJ_UART_CTRL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occurred. (When UART_MODE = 1, you have set the 7,8 non DATA_LENGHT)
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_UART_Ctrl( FJ_PERI_CH ch, FJ_UART_CTRL const* const uart_ctrl );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function start sending and receiving UART.
@param [in]	ch	Channel number.(range of 0..7)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occurred. (FJ_UART_Ctrl is not running)
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks
	- The following is a sample code for this API.
	@code
	[Sample Code]
	static UCHAR gUART_ch = 0;
	static UCHAR gUART_recv_buf[4096];	// receive data ring beffer

	VOID sample_uart_normal()
	{
		FJ_UART_CTRL		uart_ctrl;
		INT32				result;
		CHAR				sample1[] = "sample1";
		UINT32				length;

		uart_ctrl.baud_rate				= FJ_UART_BAUD_RATE_115200;		// Baud rate
		uart_ctrl.mode					= FJ_UART_MODE_NORMAL;			// UART Normal Mode
		uart_ctrl.bit_direction			= FJ_UART_BIT_DIR_LSB_FIRST;	// Transferring Direction = LSB
		uart_ctrl.stop_bit_length		= FJ_UART_STOP_BIT_1;			// Stop bit length = 1
		uart_ctrl.parity_bit			= FJ_UART_PARITY_BIT_NONE;		// Parity 8bit
		uart_ctrl.data_length			= FJ_UART_DATA_LENGTH_8;		// Data Length
		uart_ctrl.inv_nrz				= FALSE;						// NRZ
		uart_ctrl.receive_buff_addr		= gUART_recv_buf;				// Receive buffer address
		uart_ctrl.receive_buff_size		= sizeof(gUART_recv_buf);		// Receive buffer size

		result = FJ_UART_Open( gUART_ch );
		if( result != FJ_ERR_OK ) {
			// error correction code here.
			return ;
		}

		FJ_UART_Ctrl( gUART_ch, &uart_ctrl );
		FJ_UART_Start( gUART_ch );

		// Transfer process (sample1)
		length = strlen( sample1 );
		FJ_UART_Send( gUART_ch, sample1, length, FJ_USIO_DMA_CH_NOT_USE );
	}
	@endcode
*/
extern FJ_ERR_CODE FJ_UART_Start( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function stop sending and receiving UART.
@param [in]	ch	Channel number. (range of 0..7)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_UART_Stop( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function send data on UART.
@param [in]	ch			Channel number. (range of 0..7)
@param [in]	send_ptr	Send data address.
@param [in]	data_count	Count of data.
@param [in]	dma_ch		DMA channel number.
@retval FJ_ERR_OK					Success.
@retval FJ_ERR_NG					Error occurred. (UART is not starting)
@retval FJ_ERR_NG_INPUT_PARAM		Parameter error.
*/
extern FJ_ERR_CODE FJ_UART_Send( FJ_PERI_CH ch, VOID const* const send_ptr, UINT32 data_count, FJ_USIO_DMA_CH dma_ch );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function receive data on UART.
@param [in]	ch			Channel number. (range of 0..7)
@param [in]	recv_ptr	Address of the area for storing the data.
@param [in]	data_count	Count of data.
@param [in]	dma_ch		DMA channel number.
@retval FJ_ERR_OK					Success.
@retval FJ_ERR_NG					Error occurred. (FJ_UART_Receive is running)
@retval FJ_ERR_NG_INPUT_PARAM		Parameter error.
@remarks
    -  When receiving a data_count, and notifies the callback API. See @ref FJ_Host_UART_Receive_CB.
*/
extern FJ_ERR_CODE FJ_UART_Receive( FJ_PERI_CH ch, VOID const* const recv_ptr, UINT32 data_count, FJ_USIO_DMA_CH dma_ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sends a slave address on the target.
@param [in]	ch			Channel number.(range of 0..7)
@param [in]	slave_addr	Slave address(target).
@retval FJ_ERR_OK					Success.
@retval FJ_ERR_NG					Error occurred. (UART is not starting)
@retval FJ_ERR_NG_INPUT_PARAM		Parameter error.
@retval	FJ_ERR_UART_MODE_UNMATCH	Mode unmacth.
@remarks
	- This API can be called in the following order:<br>
		FJ_UART_Ctrl() -> FJ_UART_Start()
		-> FJ_UART_Send_Target_Slave_Addr() -> FJ_UART_Send()
	- If want to specify a slave of another communication and after a slave<br>
		communication end -> FJ_UART_Send_Target_Slave_Addr() -> FJ_UART_Send()
	- The following is a sample code for this API.
	@code
	[Sample Code]
	static UCHAR gUART_ch = 0;
	static UCHAR gUART_recv_buf[4096];	// receive data ring beffer

	VOID sample_uart_multi_master()
	{
		FJ_UART_CTRL		uart_ctrl;
		INT32				result;
		CHAR				sample1[] = "sample1";
		CHAR				sample2[] = "sample2";
		UINT32				length;

		uart_ctrl.baud_rate				= FJ_UART_BAUD_RATE_115200;		// Baud rate
		uart_ctrl.mode					= FJ_UART_MODE_MULTI;			// UART Multi Mode
		uart_ctrl.bit_direction			= FJ_UART_BIT_DIR_LSB_FIRST;	// Transferring Direction = LSB
		uart_ctrl.stop_bit_length		= FJ_UART_STOP_BIT_1;			// Stop bit length = 1
		uart_ctrl.parity_bit			= FJ_UART_PARITY_BIT_NONE;		// Parity 8bit
		uart_ctrl.data_length			= FJ_UART_DATA_LENGTH_8;		// Data Length
		uart_ctrl.inv_nrz				= FALSE;						// NRZ
		uart_ctrl.receive_buff_addr		= gUART_recv_buf;				// Receive buffer address
		uart_ctrl.receive_buff_size		= sizeof(gUART_recv_buf);		// Receive buffer size

		result = FJ_UART_Open( gUART_ch );
		if( result != FJ_ERR_OK ) {
			// error correction code here.
			return ;
		}

		FJ_UART_Ctrl( gUART_ch, &uart_ctrl );

		FJ_UART_Start( gUART_ch );

		// Send the address to slave 1
		FJ_UART_Send_Target_Slave_Addr( gUART_ch, 0x01 );
		if( ret != FJ_ERR_OK ){
			// error correction code here.
			return;
		}

		// Transfer process (sample1)
		length = strlen( sample1 );
		FJ_UART_Send( gUART_ch, sample1, length, FJ_USIO_DMA_CH_NOT_USE );

		// Send address to slave 2
		FJ_UART_Send_Target_Slave_Addr( gUART_ch, 0x02 );
		if( ret != FJ_ERR_OK ){
			// error correction code here.
			return;
		}

		// Transfer process (sample2)
		length = strlen( sample2 );
		FJ_UART_Send( gUART_ch, sample2, length, FJ_USIO_DMA_CH_NOT_USE );
	}
	@endcode
*/
extern FJ_ERR_CODE FJ_UART_Send_Target_Slave_Addr( FJ_PERI_CH ch, UCHAR slave_addr );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets a slave address on the source.
@param [in]	ch			Channel number.(range of 0..7)
@param [in]	slave_addr	Slave address(source).
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks
    - Slave address receive is notified by callback API. See @ref FJ_Host_UART_recv_Slave_Addr_CB.
	- The following is a sample code for this API.
	@code
	[Sample Code]
	static UCHAR gUART_ch = 0;
	static UCHAR gUART_recv_buf[4096];	// receive data ring beffer

	VOID sample_uart_multi_slave()
	{
		FJ_UART_CTRL		uart_ctrl;
		INT32				result;

		uart_ctrl.baud_rate				= FJ_UART_BAUD_RATE_115200;		// Baud rate
		uart_ctrl.mode					= FJ_UART_MODE_MULTI;			// UART Multi Mode
		uart_ctrl.bit_direction			= FJ_UART_BIT_DIR_LSB_FIRST;	// Transferring Direction = LSB
		uart_ctrl.stop_bit_length		= FJ_UART_STOP_BIT_1;			// Stop bit length = 1
		uart_ctrl.parity_bit			= FJ_UART_PARITY_BIT_NONE;		// Parity 8bit
		uart_ctrl.data_length			= FJ_UART_DATA_LENGTH_8;		// Data Length
		uart_ctrl.inv_nrz				= FALSE;						// NRZ
		uart_ctrl.receive_buff_addr		= gUART_recv_buf;				// Receive buffer address
		uart_ctrl.receive_buff_size		= sizeof(gUART_recv_buf);		// Receive buffer size

		result = FJ_UART_Open( gUART_ch );
		if( result != FJ_ERR_OK ) {
			// error correction code here.
			return ;
		}

		FJ_UART_Ctrl( gUART_ch, &uart_ctrl );

		FJ_UART_Set_Slave_Addr( gUART_ch, 0x01 );
		if( ret != FJ_ERR_OK ){
			// error correction code here.
			return;
		}

		FJ_UART_Start( gUART_ch );
	}
	@endcode
*/
extern FJ_ERR_CODE FJ_UART_Set_Slave_Addr( FJ_PERI_CH ch, UCHAR slave_addr );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function send data on UART. (Synchronous)
@param [in]  ch					Channel number. (range of 0..7)
@param [in]  send_ptr			Send data address.
@param [in]  buffer_size		Number of bytes to be received. The value sets one or more.
@param [out] data_bytes_sent	Actual number of bytes sent.
@param [in]  timeout			timeout in milli seconds. The value sets one or more.
@retval FJ_ERR_OK					Success.
@retval FJ_ERR_NG					Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM		Parameter error.
@retval FJ_ERR_NG_TMOUT				Timeout error.
@remarks
	- Please execute "FJ_UART_Ctrl -> FJ_UART_Start" again when FJ_ERR_NG_TMOUT is generated.
*/
extern FJ_ERR_CODE FJ_UART_Send_Sync( FJ_PERI_CH ch, VOID const* const send_ptr, UINT32 buffer_size, UINT32 *data_bytes_sent, UINT32 timeout );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
This function receive data on UART. (Synchronous)
@param [in]  ch					Channel number. (range of 0..7)
@param [out] recv_ptr			Address of the area for storing the data.
@param [in]  buffer_size		Number of bytes to be received. The value sets one or more.
@param [out] data_bytes_read	Actual number of bytes received.
@param [in]  timeout			timeout in milli seconds.
@retval FJ_ERR_OK					Success.
@retval FJ_ERR_NG					Error occurred. (FJ_UART_Start no execution. FJ_UART_Receive or FJ_UART_Receive_Sync is running.)
@retval FJ_ERR_NG_INPUT_PARAM		Parameter error.
@retval FJ_ERR_NG_TMOUT				Timeout error.
@retval FJ_ERR_UART_OVERFLOW_ERROR	Overflow error.
@retval FJ_ERR_UART_OVERRUN_ERROR	Overrun error.
@retval FJ_ERR_UART_FRAMING_ERROR	Framing error.
@retval FJ_ERR_UART_PARITY_ERROR	Parity error.
@remarks
	- It returns by FJ_ERR_NG when there is no receive data when timeout value is 0. 
*/
extern FJ_ERR_CODE FJ_UART_Receive_Sync( FJ_PERI_CH ch, VOID const* const recv_ptr, UINT32 buffer_size, UINT32 *data_bytes_read, UINT32 timeout );

/* ------------- */
/* SIO           */
/* ------------- */

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function acquire semaphore of SIO channel.
@param [in]	ch	Channel number. (range of 0..7)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Semaphore acquisition NG.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks - If use ch5, please set following CHIPTOP settings(Please see LSI specification)<br>
<table style="text-align:left;" border="1" cellpadding="2" cellspacing="2">
    <tr>
      <th style="background-color: rgb(204, 255, 255);">Port</th>
      <th style="background-color: rgb(204, 255, 255);">Settings</th>
    </tr>
    <tr>
      <td>
        PD[5:3]<br>
        (FPSIN/OUT/CK:5)
      </td>
      <td>
        VDOSEL.VMDHS = 3<br>
        EPCRD[3] = 1<br>
        EPCRD[4] = 1<br>
        EPCRD[5] = 0<br>
        DDRD[5] = 0<br>
      </td>
    </tr>
</table>
*/
extern FJ_ERR_CODE FJ_SIO_Open( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function close serial I/O communication channels.
@param [in]	ch	Channel number. (range of 0..7)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_SIO_Close( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets serial I/O communication operation and callback function for the specified channel.
@param [in]	ch			Channel number. (range of 0..7)
@param [in]	sio_ctrl	Serial I/O communication Control data. See @ref FJ_SIO_CTRL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks
			Please set the baudrate in the range below. (UART Clock = 200MHz cases)<br>
			6104 - 50000000 (bps)<br><br>
			- Baudrate calculation<br>
			 reload value : V = Clock / b - 1<br>
			 baudrate(calculated value) : b = Clock / (V + 1)<br>
			 (V:reload value  b:baudrate)<br><br>
			- Example (UART Clock:200MHz, baudrate:9750000)<br>
			 reload value : 200000000 / 9750000 - 1 = 19<br>
			 baudrate(calculated value) : 200000000 / (19 + 1) = 10000000 (bps)<br>
			- Specified callback function is called from task-independent portions.<br>
			  See @ref FJ_SIO_CTRL.<br>
*/
extern FJ_ERR_CODE FJ_SIO_Ctrl( FJ_PERI_CH ch, FJ_SIO_CTRL const* const sio_ctrl );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function stop serial I/O communication channels.
@param [in]	ch	Channel number. (range of 0..7)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks
			- If want to run again after running this API, please reconfigure FJ_SIO_Ctrl().
			- Please call this API using the timer if want to stop the transfer.(in synchronous transfer)
*/
extern FJ_ERR_CODE FJ_SIO_Stop( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function start data transfer.<br>
@param [in]	ch			Channel number. (range of 0..7)
@param [in]	trans_ctrl	Serial I/O communication Transfer Control data. See @ref FJ_SIO_CTRL_TRANSFER.
@param [in]	dma_ch		DMA channel number.
@retval FJ_ERR_OK					Success.
@retval FJ_ERR_NG					Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM		Parameter error.
@retval FJ_ERR_SIO_OVERRUN_ERROR	Receive buffer overrun error.
@remarks
			- In send data case, the value must be guaranteed about the send data until the send processing is completed.
			- Set "USHORT" pointer to recv_ptr when Data Length is "FJ_SIO_LENGTH_9".<br>
			  Set "UCHAR" pointer to recv_ptr when Data Length is "FJ_SIO_LENGTH_8" or less.
			- If pcallback was specified, this API is asynchronization.<br>
			  If pcallback was not specified, this API is synchronization.
*/
extern FJ_ERR_CODE FJ_SIO_Transfer( FJ_PERI_CH ch, FJ_SIO_CTRL_TRANSFER const* const trans_ctrl, FJ_USIO_DMA_CH dma_ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function start full duplex communication.<br>
Please input send data pointer and receive buffer pointer and size information.
@param [in]	ch			Channel number. (range of 0..7)
@param [in]	sio_ctrl	Serial I/O communication Control data(Full duplex). See @ref FJ_SIO_CTRL_FULL_DUPLEX.
@retval FJ_ERR_OK					Success.
@retval FJ_ERR_NG					Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM		Parameter error.
@retval FJ_ERR_SIO_OVERRUN_ERROR	Receive buffer overrun error.
@remarks
			- Master and the slave should execute the handshake signal so that the communication begin.<br>
			  In master side, wait for the slave's preparation completion before calling this API.<br>
			  In slave side, notify the preparation completion to master side after calling this API.
			- If pcallback was specified, this API is asynchronization.<br>
			  If pcallback was not specified, this API is synchronization.
*/
extern FJ_ERR_CODE FJ_SIO_Full_Duplex( FJ_PERI_CH ch, FJ_SIO_CTRL_FULL_DUPLEX const* const sio_ctrl );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function start full duplex communication.<br>
Please input send data pointer and receive buffer pointer and size information.
@param [in]	ch			Channel number. (range of 0..7)
@param [in]	sio_ctrl	Serial I/O communication Control data(Full duplex). See @ref FJ_SIO_CTRL_FULL_DUPLEX.
@param [in]	dma_ch_send	DMA channel number.
@param [in]	dma_ch_recv	DMA channel number.
@retval FJ_ERR_OK					Success.
@retval FJ_ERR_NG					Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM		Parameter error.
@retval FJ_ERR_SIO_OVERRUN_ERROR	Receive buffer overrun error.
@remarks
			- Master and the slave should execute the handshake signal so that the communication begin.<br>
			  In master side, wait for the slave's preparation completion before calling this API.<br>
			  In slave side, notify the preparation completion to master side after calling this API.
			- If pcallback was specified, this API is asynchronization.<br>
			  If pcallback was not specified, this API is synchronization.
*/
extern FJ_ERR_CODE FJ_SIO_Full_Duplex_DMA( FJ_PERI_CH ch, FJ_SIO_CTRL_FULL_DUPLEX const* const sio_ctrl, FJ_USIO_DMA_CH dma_ch_send, FJ_USIO_DMA_CH dma_ch_recv  );

/* ------------- */
/* SPI           */
/* ------------- */

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function acquire semaphore of SPI channel.
@param [in]	ch	Channel number. (range of 0..2)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Semaphore acquisition NG.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_SPI_Open( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function close serial I/O communication channels.
@param [in]	ch	Channel number. (range of 0..2)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_SPI_Close( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets serial I/O communication operation and callback function for the specified channel.
@param [in]	ch			Channel number. (range of 0..2)
@param [in]	spi_ctrl	Serial I/O communication Control data. See @ref FJ_SPI_CTRL.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
*/
extern FJ_ERR_CODE FJ_SPI_Ctrl( FJ_PERI_CH ch, FJ_SPI_CTRL const* const spi_ctrl );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function stop serial I/O communication channels.
@param [in]	ch	Channel number. (range of 0..2)
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks
			- If want to run again after running this API, please reconfigure FJ_SPI_Ctrl().
			- Please call this API using the timer if want to stop the transfer.(in synchronous transfer)
*/
extern FJ_ERR_CODE FJ_SPI_Stop( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function start data transfer.<br>
@param [in]	ch			Channel number. (range of 0..2)
@param [in]	trans_ctrl	Serial I/O communication Transfer Control data. See @ref FJ_SPI_CTRL_TRANSFER.
@param [in]	dma_ch		DMA channel number.
@retval FJ_ERR_OK					Success.
@retval FJ_ERR_NG					Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM		Parameter error.
@retval FJ_ERR_SPI_OVERRUN_ERROR	Receive buffer overrun error.
@remarks
			- In send data case, the value must be guaranteed about the send data until the send processing is completed.
			- Set "USHORT" pointer to recv_ptr when Data Length is "FJ_SPI_LENGTH_9".<br>
			  Set "UCHAR" pointer to recv_ptr when Data Length is "FJ_SPI_LENGTH_8" or less.
			- If pcallback was specified, this API is asynchronization.<br>
			  If pcallback was not specified, this API is synchronization.
*/
extern FJ_ERR_CODE FJ_SPI_Transfer( FJ_PERI_CH ch, FJ_SPI_CTRL_TRANSFER const* const trans_ctrl, FJ_USIO_DMA_CH dma_ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function start full duplex communication.<br>
Please input send data pointer and receive buffer pointer and size information.
@param [in]	ch			Channel number. (range of 0..2)
@param [in]	spi_ctrl	Serial I/O communication Control data(Full duplex). See @ref FJ_SPI_CTRL_FULL_DUPLEX.
@retval FJ_ERR_OK					Success.
@retval FJ_ERR_NG					Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM		Parameter error.
@retval FJ_ERR_SPI_OVERRUN_ERROR	Receive buffer overrun error.
@remarks
			- Master and the slave should execute the handshake signal so that the communication begin.<br>
			  In master side, wait for the slave's preparation completion before calling this API.<br>
			  In slave side, notify the preparation completion to master side after calling this API.
			- If pcallback was specified, this API is asynchronization.<br>
			  If pcallback was not specified, this API is synchronization.
*/
extern FJ_ERR_CODE FJ_SPI_Full_Duplex( FJ_PERI_CH ch, FJ_SPI_CTRL_FULL_DUPLEX const* const spi_ctrl );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function start full duplex communication.<br>
Please input send data pointer and receive buffer pointer and size information.
@param [in]	ch			Channel number. (range of 0..2)
@param [in]	spi_ctrl	Serial I/O communication Control data(Full duplex). See @ref FJ_SPI_CTRL_FULL_DUPLEX.
@param [in]	dma_ch_send	DMA channel number.
@param [in]	dma_ch_recv	DMA channel number.
@retval FJ_ERR_OK					Success.
@retval FJ_ERR_NG					Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM		Parameter error.
@retval FJ_ERR_SPI_OVERRUN_ERROR	Receive buffer overrun error.
@remarks
			- Master and the slave should execute the handshake signal so that the communication begin.<br>
			  In master side, wait for the slave's preparation completion before calling this API.<br>
			  In slave side, notify the preparation completion to master side after calling this API.
			- If pcallback was specified, this API is asynchronization.<br>
			  If pcallback was not specified, this API is synchronization.
*/
extern FJ_ERR_CODE FJ_SPI_Full_Duplex_DMA( FJ_PERI_CH ch, FJ_SPI_CTRL_FULL_DUPLEX const* const spi_ctrl, FJ_USIO_DMA_CH dma_ch_send, FJ_USIO_DMA_CH dma_ch_recv  );


/* ------------- */
/* Gyro IF       */
/* ------------- */
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Start Gyro data read.<br>
@param [in]	gyro_ctrl	Gyro data read control data. See @ref FJ_GYROIF_READ_CTRL.
@retval FJ_ERR_OK					Success.
@retval FJ_ERR_NG					Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM		Parameter error.
@remarks
		- Please execute the cache clean and flush as necessary.
		- Gyro IF API use the SPI ch2. (Hardware SPI ch 3)
		- The following is a sample code for this API.
	@code
[Sample Code]

// Gyro data bffer. Assigned to non cache area.
UCHAR gSample_Gyro_Send_Data[15] __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));		// SPI Send buffer. Size is same as data_count(register address and dummy data for data receive.).  D-Cache line aligned.
UCHAR gSample_Gyro_Recv_Data[65536][15] __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));	// SPI Receive buffer. D-Cache line aligned.
ULONG gSample_Gyro_TimeStamp[65536][2] __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));	// TimeStamp buffer. Size is Same as element count of Gyro data. D-Cache line aligned.

VOID Sample_GyroIF_Data_Read_Callbak( FJ_ERR_CODE result )
{
	FJ_Debug_Print_Info("Gyro Data Callback. result=0x%08X\n", result);

	if( result != FJ_ERR_OK ){
		FJ_GyroIF_Stop_Read();
	}
}

VOID Sample_GyroIF_TimeStamp_Callbak( FJ_ERR_CODE result )
{
	FJ_Debug_Print_Info("Gyro TimeStamp Callback. result=0x%08X\n", result);

	if( result != FJ_ERR_OK ){
		FJ_GyroIF_Stop_Read();
	}
}

VOID Sample_GyroIF_Read_Data( VOID )
{
	FJ_ERR_CODE			ret;
	FJ_GYROIF_READ_CTRL	gyro_ctrl;

	gSample_Gyro_Send_Data[0] = 0x80 | 0x74;	// R/W bit and Register address for Gyro read.

	// Set SPI for Gyro data
	gyro_ctrl.data_ctrl.mode				= FJ_SPI_MODE_3_CPOL1_CPHA1;		// Clock Polarity=1, Clock Phase=1
	gyro_ctrl.data_ctrl.bit_direction		= FJ_SPI_BIT_DIR_MSB_FIRST;			// MSB First
	gyro_ctrl.data_ctrl.data_length			= FJ_SPI_LENGTH_8;					// 8bit
	gyro_ctrl.data_ctrl.baudrate			= 18000000;							// 18MHz (ch2 is max 54MHz. when SPICLK=216MHz, div=5)
	gyro_ctrl.data_ctrl.fifo_wmark_tx		= 8;								// TX FIFO Watermark (ch2 Tx FIFO size is 16)
	gyro_ctrl.data_ctrl.fifo_wmark_rx		= 15;								// RX FIFO Watermark (ch2 Rx FIFO size is 16)
	gyro_ctrl.data_ctrl.ss_info.cont_trans	= FJ_SPI_CONT_TRANS_INACT_FIFO_EMPTY;	// ssOut become inactive when the TX FIFO is empty.
	gyro_ctrl.data_ctrl.ss_info.ssout[0]	= 1;								// Connected to the device
	gyro_ctrl.data_ctrl.ss_info.ssout[1]	= 0;								// Not Connected
	gyro_ctrl.data_ctrl.ss_info.ssout[2]	= 0;								// Not Connected
	gyro_ctrl.data_ctrl.ss_info.ssout[3]	= 0;								// Not Connected
	gyro_ctrl.data_ctrl.ss_info.sspol[0]	= 0;								// Active Low
	gyro_ctrl.data_ctrl.ss_info.sspol[1]	= 0;								// Not Connected
	gyro_ctrl.data_ctrl.ss_info.sspol[2]	= 0;								// Not Connected
	gyro_ctrl.data_ctrl.ss_info.sspol[3]	= 0;								// Not Connected
	gyro_ctrl.data_ctrl.send_addr			= (ULONG)gSample_Gyro_Send_Data;	// Send buffer address
	gyro_ctrl.data_ctrl.recv_addr			= (ULONG)gSample_Gyro_Recv_Data;	// Receive buffer address
	gyro_ctrl.data_ctrl.data_count			= 15;								// e.g. Register address(1byte) + Gyro(2byte * 3) + Temp(2byte * 1) + Accel(2byte * 3)
	gyro_ctrl.data_ctrl.total_data_count	= 65536;							// Count of receive buffer
	gyro_ctrl.data_ctrl.dma_recv_to			= 16777215;							// Should be set Max value
	gyro_ctrl.data_ctrl.pcallback			= (GYROIF_CALLBACK)Sample_GyroIF_Data_Read_Callbak;		// Callback function pointer

	// Set TMR64 for Gyro TimeStamp
	gyro_ctrl.stamp_ctrl.timer_clk			= FJ_GYROIF_TMR_CLK_DIV_2;			// 64bit Timer is 2 dividing frequency
	gyro_ctrl.stamp_ctrl.buff_addr			= (ULONG)gSample_Gyro_TimeStamp;	// TimeStamp buffer address
	gyro_ctrl.stamp_ctrl.total_data_count	= 65536;							// Count of TimeStamp buffer (Same as Gyro data total_data_count)
	gyro_ctrl.stamp_ctrl.pcallback			= (GYROIF_CALLBACK)Sample_GyroIF_TimeStamp_Callbak;		// Callback function pointer

	// Start Gyro Read
	ret = FJ_GyroIF_Start_Read( &gyro_ctrl );
	if( ret != FJ_ERR_OK ){
		FJ_Debug_Print_Err("Start Gyro Read ERR. ret=0x%08X\n", ret);
		return;
	}
	else {
		FJ_Debug_Print_Info("Start Gyro Read OK\n");
	}

	// 
	// Get the Read data count at any time and to use the Gyro data. (e.g. SOF/EOF interrupt)
	// FJ_GyroIF_Get_Read_Count( &data_count );
	// 
}
@endcode
*/
extern FJ_ERR_CODE FJ_GyroIF_Start_Read( FJ_GYROIF_READ_CTRL const* const gyro_ctrl );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Stop Gyro data read.<br>
@retval FJ_ERR_OK					Success.
@retval FJ_ERR_NG					Error occurred.
*/
extern FJ_ERR_CODE FJ_GyroIF_Stop_Read( VOID );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Get Gyro data read count.<br>
@param [in]	read_count	Count of read Gyro data for total_data_count. (0 - 65535)<br>
						If read_count is 0, indicating that it has just received total_data_count or data not yet been received.
@retval FJ_ERR_OK					Success.
@retval FJ_ERR_NG					Error occurred.
@retval FJ_ERR_NG_INPUT_PARAM		Parameter error.
*/
FJ_ERR_CODE FJ_GyroIF_Get_Read_Count( UINT32* const read_count );


/* ------------- */
/* GPIO          */
/* ------------- */
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set the functionality of a selectable GPIO port.<br>
It is set to the EPCR(External Port Control Register) value.
@param [in]  port		Port name. See @ref FJ_GPIO_PORT
@param [in]  function	Function type.
						  <ul>
							  <li>0:@ref FJ_GPIO_FUNC_GPIO
							  <li>1:@ref FJ_GPIO_FUNC_HWMODE
						  </ul>
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE	FJ_Gpio_Set_Function( FJ_GPIO_PORT port, UCHAR function );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set the direction of a selectable GPIO port.<br>
It is set to the DDR(Data Direction Register) value.
@param [in]  port		Port name. See @ref FJ_GPIO_PORT
@param [in]  direction	Direction type.
						  <ul>
							  <li>0:@ref FJ_GPIO_DIR_IN
							  <li>1:@ref FJ_GPIO_DIR_OUT
						  </ul>
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE	FJ_Gpio_Set_Direction( FJ_GPIO_PORT port, UCHAR direction );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set the state of a selectable GPIO port.<br>
It is set to the PDR(Port Data Register) value.
@param [in]  port		Port name. See @ref FJ_GPIO_PORT
@param [in]  status		Port status.
						  <ul>
							  <li>0:@ref FJ_GPIO_STATUS_LOW
							  <li>1:@ref FJ_GPIO_STATUS_HIGH
						  </ul>
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE	FJ_Gpio_Set_Status( FJ_GPIO_PORT port, UCHAR status );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get the state of a selectable GPIO port.<br>
It is get from the PDR(Port Data Register) value.
@param [in]  port		Port name. See @ref FJ_GPIO_PORT
@param [out] status		Port status.
						  <ul>
							  <li>0:@ref FJ_GPIO_STATUS_LOW
							  <li>1:@ref FJ_GPIO_STATUS_HIGH
						  </ul>
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE	FJ_Gpio_Get_Status( FJ_GPIO_PORT port, UCHAR* status );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set the Pull-up/down enable state of a selectable GPIO port.<br>
It is set the PUDER(Pull-up/down Enable Register) value.
@param [in]  port		Port name. See @ref FJ_GPIO_PORT
@param [in]  enable		Pull-up/down enable state.
						  <ul>
							  <li>0:@ref FJ_GPIO_PUD_OFF
							  <li>1:@ref FJ_GPIO_PUD_ON
						  </ul>
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE	FJ_Gpio_Set_Pull_Up_Down_Enable( FJ_GPIO_PORT port, UCHAR enable );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set the Pull-up/down state of a selectable GPIO port.<br>
It is set the PUDCR(Pull-up/down Control Register) value.
@param [in]  port		Port name. See @ref FJ_GPIO_PORT
@param [in]  control	Pull-up/down state.
						  <ul>
							  <li>0:@ref FJ_GPIO_PUD_DOWN
							  <li>1:@ref FJ_GPIO_PUD_UP
						  </ul>
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern FJ_ERR_CODE	FJ_Gpio_Set_Pull_Up_Down_Ctrl( FJ_GPIO_PORT port, UCHAR control );


/* ------------- */
/* SLIMBus       */
/* ------------- */
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function acquire semaphore of SLIMBus channel.
@param[in]	ch		Channel number.	See @ref FJ_SLIMBUS_CH.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG				Semaphore acquisition NG.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks
	 - This API Sample code is as follows.<br>
	@code
[Sample Code]

#define	CONFIG_MSG_NUM	(16)

static T_MC_FIFO_MSG	gSlimbus_MC_FIFO_Tx_Msg[] = {
// ASSIGN_LOGICAL_ADDRESS
	// ch1 Interface Device (LA=0x40)	DT 11 MI 5 MR 0
	{0x10, {0x020AFFF3, 0x008C0311, 0x40010000, 0x00000050}},	// 0
	// ch1 Generic Device (LA=0x42)	DT 11 MI 5 MR 0
	{0x10, {0x020AFFF3, 0x008C0311, 0x42010200, 0x00000050}},	// 1
	// ch0 Interface Device (LA=0x30)	DT 11 MI 14 MR 0
	{0x10, {0x020AFFF3, 0x008C0311, 0x30000000, 0x000000D0}},	// 2
	// ch0 Generic Device (LA=0x32)	DT 11 MI 7 MR 0
	{0x10, {0x020AFFF3, 0x008C0311, 0x32000200, 0x00000070}},	// 3
	// ch0 Framer Device (LA=0x31)	DT 11 MI 8 MR 0
	{0x10, {0x020AFFF3, 0x008C0311, 0x31000100, 0x00000080}},	// 4
// REQUEST_SELF_ANNOUNCEMENT
	{0x8,  {0x0C03FFF3, 0x00000035, 0x00000000, 0x00000000}},	// 5
// BEGIN_RECONFIGURATION
	{0x8,  {0x4003FFF3, 0x00000035, 0x00000000, 0x00000000}},	// 6
// NEXT_ROOT_FREQUENCY	F 1
	{0x8,  {0x4704FFF3, 0x00300133, 0x00000000, 0x00000000}},	// 7
// NEXT_CLOCK_GEAR	G 9
	{0x8,  {0x4604FFF3, 0x00B00936, 0x00000000, 0x00000000}},	// 8
// NEXT_SUBFRAME_MODE	M 19
	{0x8,  {0x4504FFF3, 0x00C01339, 0x00000000, 0x00000000}},	// 9
// CONNECT_SOURCE	CN 27 PN 0
	{0xc,  {0x1006FFF3, 0x1B00320C, 0x000000A0, 0x00000000}},	// 10
// CONNECT_SINK	CN 27 PN 3
	{0xc,  {0x1106FFF3, 0x1B004209, 0x000000F0, 0x00000000}},	// 11
// NEXT_DEFINE_CHANNEL	CN 27 TP 1 SD 0xC46 SL 9
	{0xc,  {0x5007FFF3, 0x1C461B39, 0x00009009, 0x00000000}},	// 12
// NEXT_DEFINE_CONTENT	CN 27 FL 0 PR 17 AF 0 DT 1 CL 1 DL 31
	{0xc,  {0x5107FFF3, 0x01111B37, 0x00005008, 0x00000000}},	// 13
// ACTIVATE_CHANNEL	CN 27
	{0x8,  {0x5404FFF3, 0x00B01B33, 0x00000000, 0x00000000}},	// 14
// RECONFIGURE_NOW
	{0x8,  {0x5F03FFF3, 0x0000003C, 0x00000000, 0x00000000}},	// 15
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// DISCONNECT_PORT(SOURCE)	LA 18 PN 0
	{0xc,  {0x1405FFF3, 0xA000420D, 0x00000000, 0x00000000}},	// 16
// DISCONNECT_PORT(SINK)	LA 50 PN 0
	{0xc,  {0x1405FFF3, 0xB000320D, 0x00000000, 0x00000000}},	// 17
	{0x0,  {0x00000000, 0x00000000, 0x00000000, 0x00000000}},	// terminator
};

static const UINT32	gSlimbus_Write_Data[WRITE_DATA_NUM] =
{
	0x12345678,0x23456789,0x3456789a,0x456789ab,	// 0
	0x55555555,0x66666666,0x77777777,0x88888888,	// 1
	0x99999999,0xaaaaaaaa,0xbbbbbbbb,0xcccccccc,	// 2
	0xdddddddd,0xeeeeeeee,0xffffffff,0x00000000,	// 3
	0x11111111,0x22222222,0x33333333,0x44444444,	// 4
	0x55555555,0x66666666,0x77777777,0x88888888,	// 5
	0x99999999,0xaaaaaaaa,0xbbbbbbbb,0xcccccccc,	// 6
	0xdddddddd,0xeeeeeeee,0xffffffff,0x00000000,	// 7
	0x11111111,0x22222222,0x33333333,0x44444444,	// 8
	0x55555555,0x66666666,0x77777777,0x88888888,	// 9
	0x99999999,0xaaaaaaaa,0xbbbbbbbb,0xcccccccc,	// 10
	0xdddddddd,0xeeeeeeee,0xffffffff,0x00000000,	// 11
	0x11111111,0x22222222,0x33333333,0x44444444,	// 12
	0x55555555,0x66666666,0x77777777,0x88888888,	// 13
	0x99999999,0xaaaaaaaa,0xbbbbbbbb,0xcccccccc,	// 14
	0xdddddddd,0xeeeeeeee,0xffffffff,0x00000000,	// 15
};

static UCHAR gSlimbus_Run_Status = 0;
static UINT32	gSlimbus_Write_Data_Size = 0;
static UINT32	gSlimbus_Read_Data_Size = 0;
static UINT32	gSlimbus_Read_Data[READ_DATA_NUM] __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));
static UINT32	gSlimbus_Read_Msg[READ_DATA_NUM] __attribute__((section(".DCACHE_ALIGN_SECTION"), aligned(64)));

static VOID ct_slimbus_enumeration(FJ_SLIMBUS_CH ch)
{
	INT32			ret;
	UCHAR			index;
	DDIM_USER_ER	ercd;
	UINT32			msg;
	UINT32			la;
	T_MC_FIFO_MSG	t_msg;

	for(index=0 ; index<CONFIG_MSG_NUM ; index++){
		gSlimbus_Tx_Int_Flag[ch] = 0;
		t_msg = gSlimbus_MC_FIFO_Tx_Msg[index];
		if(index==10){		// CONNECT_SOURCE

			msg = gSlimbus_MC_FIFO_Tx_Msg[index+1].msg[1];
			la = msg & 0x0000FF00;
			t_msg.msg[1] &= 0xFFFF00FF;
			t_msg.msg[1] |= la;
		}
		else if(index==11){	// CONNECT_SINK

			msg = gSlimbus_MC_FIFO_Tx_Msg[index-1].msg[1];
			la = msg & 0x0000FF00;
			t_msg.msg[1] &= 0xFFFF00FF;
			t_msg.msg[1] |= la;
		}
		ret = FJ_Slimbus_Write_Msg(ch, &t_msg.msg[0], t_msg.size);
		if(ret != FJ_ERR_OK){
			BF_Debug_Print_Information(("FJ_Slimbus_Write_Msg(%d) Error. Return Value=0x%08X\n", ch, ret));
		}

		while(1){
			ercd = OS_User_Dly_Tsk(1);	// 1ms wait
			if (ercd != D_DDIM_USER_E_OK){
				BF_Debug_Print_Information(("OS_User_Dly_Tsk Error. ercd = %d\n", ercd));
			}
			if(gSlimbus_Tx_Int_Flag[ch] == 1){
				break;
			}
		}
	}
}

static VOID ct_slimbus_disconnect(FJ_SLIMBUS_CH ch)
{
	INT32			ret;
	UCHAR			index;
	DDIM_USER_ER	ercd;

	for(index=CONFIG_MSG_NUM ; index<CONFIG_MSG_NUM+2 ; index++){
		gSlimbus_Tx_Int_Flag[ch] = 0;
		ret = FJ_Slimbus_Write_Msg(ch, &gSlimbus_MC_FIFO_Tx_Msg[index].msg[0], gSlimbus_MC_FIFO_Tx_Msg[index].size);
		if(ret != FJ_ERR_OK){
			BF_Debug_Print_Information(("FJ_Slimbus_Write_Msg(%d) Error. Return Value=0x%08X\n", ch, ret));
		}

		while(1){
			ercd = OS_User_Dly_Tsk(1);	// 1ms wait
			if (ercd != D_DDIM_USER_E_OK){
				BF_Debug_Print_Information(("OS_User_Dly_Tsk Error. ercd = %d\n", ercd));
			}
			if(gSlimbus_Tx_Int_Flag[ch] == 1){
				break;
			}
		}
	}
}


VOID ct_slimbus_manager_int_cb(UCHAR ch, INT32 status)
{
	FJ_ERR_CODE			ret=0;
	OS_USER_ER		ercd = D_OS_USER_E_OK;
	OS_USER_FLGPTN flg = 0;
	UINT32	size = 0;
	UCHAR*	s;

	if (ch == 1) {
		flg |= FLG_SLIMBUS_CONTROLLER_MANAGER_CH1;
	}
	else if (ch == 2) {
		flg |= FLG_SLIMBUS_CONTROLLER_MANAGER_CH2;
	}
	else if (ch == 3) {
		flg |= FLG_SLIMBUS_CONTROLLER_MANAGER_CH3;
	}
	else {
		flg |= FLG_SLIMBUS_CONTROLLER_MANAGER_CH0;
	}
	if (status & 0x80) {	// 7 PORT_INT R
		flg |= FLG_SLIMBUS_CONTROLLER_PORT_INT;
	}
	if (status & 0x40) {	// 6 MCH_INT R/C
		flg |= FLG_SLIMBUS_CONTROLLER_MCH_INT;
	}
	if (status & 0x20) {	// 5 RCFG_INT R/C
		flg |= FLG_SLIMBUS_CONTROLLER_RCFG_INT;
	}
	if (status & 0x10) {	// 4 SYNC_LOST R/C
		flg |= FLG_SLIMBUS_CONTROLLER_SYNC_LOST;
	}
	if (status & 0x08) {	// 3 TX_ERR R/C
		flg |= FLG_SLIMBUS_CONTROLLER_TX_ERR;
	}
	if (status & 0x04) {	// 2 TX_INT R/C
		flg |= FLG_SLIMBUS_CONTROLLER_TX_INT;
	}
	if (status & 0x02) {	// 1 RX_INT R/C
		flg |= FLG_SLIMBUS_CONTROLLER_RX_INT;
		memset(gSlimbus_Read_Msg, 0, sizeof(gSlimbus_Read_Msg));
		ret = FJ_Slimbus_Read_Msg(ch, gSlimbus_Read_Msg, &size);
		if(ret != FJ_ERR_OK){
			FJ_Debug_Print_Info(("FJ_Slimbus_Read_Msg(%d) Error. Return Value=0x%08X\n", ch, ret));
		}
		else {
			FJ_Debug_Print_Info(("FJ_Slimbus_Read_Msg(%d) Return data=0x%08X size=%d\n", ch, gSlimbus_Read_Msg[0], size));
			FJ_Slimbus_Set_Read_Msg_Area_Addr( ch, (UINT32)gSlimbus_Read_Msg, size );

			s = (UCHAR*)gSlimbus_Read_Msg;

			if (*(s + 3) == 0x00) {
				FJ_Debug_Print_Info(("REPORT_PRESENT\n"));
				if (*(s + 4) == 0x00 && *(s + 5) == 0x02 && *(s + 6) == 0x00) {
					// ch0 Generic Device (LA=0x32)
					// EA = ((*(s + 2) << 8) + *(s + 1)) << 8;
					FJ_Debug_Print_Info(("ch0 Generic Device [0x%04X]\n",  (*(s + 1) << 8) + *(s + 2)));
					gSlimbus_Run_Status ++;
				}
				if (*(s + 4) == 0x00 && *(s + 5) == 0x00 && *(s + 6) == 0x00) {
					// ch0 Interface Device (LA=0x30)
					// EA = ((*(s + 2) << 8) + *(s + 1)) << 8;
					FJ_Debug_Print_Info(("ch0 Interface Device [0x%04X]\n",  (*(s + 1) << 8) + *(s + 2)));
					gSlimbus_Run_Status ++;
				}
				if (*(s + 4) == 0x00 && *(s + 5) == 0x00 && *(s + 6) == 0x01) {
					// ch1 Interface Device (LA=0x40)
					// EA = ((*(s + 2) << 8) + *(s + 1)) << 8;
					FJ_Debug_Print_Info(("ch1 Interface Device [0x%04X]\n",  (*(s + 1) << 8) + *(s + 2)));
					gSlimbus_Run_Status ++;
				}
				if (*(s + 4) == 0x00 && *(s + 5) == 0x01 && *(s + 6) == 0x00) {
					// ch0 Framer Device (LA=0x31)
					// EA = ((*(s + 2) << 8) + *(s + 1)) << 8;
					FJ_Debug_Print_Info(("ch0 Framer Device [0x%04X]\n",  (*(s + 1) << 8) + *(s + 2)));
					gSlimbus_Run_Status ++;
				}
				if (*(s + 4) == 0x00 && *(s + 5) == 0x02 && *(s + 6) == 0x01) {
					// ch1 Generic Device (LA=0x42)
					// EA = ((*(s + 2) << 8) + *(s + 1)) << 8;
					FJ_Debug_Print_Info(("ch1 Generic Device [0x%04X]\n",  (*(s + 1) << 8) + *(s + 2)));
					gSlimbus_Run_Status ++;
				}
			}
		}
		FJ_Debug_Print_Info(("FJ_Slimbus_Read_Msg(%d) completed. ret=0x%08X\n", ch, ret));
	}
	if(flg & 0xFFFF0000){
		ercd = OS_User_Set_Flg( FID_SLIMBUS_CONTROLLER, flg );
		if (ercd != D_OS_USER_E_OK){
			FJ_Debug_Print_Error(("ct_slimbus_data_port_int_cb set_flg error(%d) L=%d\n", ercd, __LINE__));
		}
	}
}

VOID ct_slimbus_data_port_int_cb(UCHAR ch, INT32 status)
{
	OS_USER_ER		ercd = D_OS_USER_E_OK;
	OS_USER_FLGPTN flg = 0;

	if (ch == 1) {
		flg = FLG_SLIMBUS_CONTROLLER_DATAPORT_CH1;
	}
	else if (ch == 2) {
		flg = FLG_SLIMBUS_CONTROLLER_DATAPORT_CH2;
	}
	else if (ch == 3) {
		flg = FLG_SLIMBUS_CONTROLLER_DATAPORT_CH3;
	}
	else {
		flg = FLG_SLIMBUS_CONTROLLER_DATAPORT_CH0;
	}
	ercd = OS_User_Set_Flg( FID_SLIMBUS_CONTROLLER, flg );
	if (ercd != D_OS_USER_E_OK){
		FJ_Debug_Print_Error(("ct_slimbus_data_port_int_cb set_flg error(%d) L=%d\n", ercd, __LINE__));
	}
}

static VOID ct_slimbus_dma_int_write_cb(UCHAR ch, INT32 status)
{
}

VOID ct_slimbus_dma_int_read_cb(UCHAR ch, INT32 status)
{
	OS_USER_ER		ercd = D_OS_USER_E_OK;
	OS_USER_FLGPTN flg = 0;

	FJ_Slimbus_Set_Read_Data_Area_Addr( ch, (UINT32)gSlimbus_Read_Data, gSlimbus_Read_Data_Size );
	if (ch == 1) {
		flg = FLG_SLIMBUS_CONTROLLER_DMA_CH1;
	}
	else if (ch == 2) {
		flg = FLG_SLIMBUS_CONTROLLER_DMA_CH2;
	}
	else if (ch == 3) {
		flg = FLG_SLIMBUS_CONTROLLER_DMA_CH3;
	}
	else {
		flg = FLG_SLIMBUS_CONTROLLER_DMA_CH0;
	}
	ercd = OS_User_Set_Flg( FID_SLIMBUS_CONTROLLER, flg );
	if (ercd != D_OS_USER_E_OK){
		FJ_Debug_Print_Error(("ct_slimbus_data_port_int_cb set_flg error(%d) L=%d\n", ercd, __LINE__));
	}
	gSlimbus_Run_Status = 1;
}


VOID sequence()
{
	// FJ_Slimbus_Open Start
	ret = FJ_Slimbus_Open(FJ_SLIMBUS_CH1);
	FJ_Debug_Print_Info(("FJ_Slimbus_Open(1) completed. ret=0x%08X\n", ret));
	ret = FJ_Slimbus_Open(FJ_SLIMBUS_CH0);
	FJ_Debug_Print_Info(("FJ_Slimbus_Open(0) completed. ret=0x%08X\n", ret));

	// FJ_Slimbus_Ctrl Start
	slimbus_ctrl.ch				= FJ_SLIMBUS_CH1;
	slimbus_ctrl.manager_mode	= FJ_SLIMBUS_MANAGER_MODE_DISABLE;
	slimbus_ctrl.fr_en			= FJ_SLIMBUS_FR_DISABLE;
	slimbus_ctrl.src_thr		= 16;
	slimbus_ctrl.sink_thr		= 0;
	slimbus_ctrl.manager_int_cb = (SLIMBUS_CALLBACK)ct_slimbus_manager_int_cb;
	slimbus_ctrl.data_port_int_cb = (SLIMBUS_CALLBACK)ct_slimbus_data_port_int_cb;
	ret = FJ_Slimbus_Ctrl(&slimbus_ctrl);
	FJ_Debug_Print_Info(("FJ_Slimbus_Ctrl(%d) completed. ret=0x%08X\n", slimbus_ctrl.ch, ret));

	slimbus_ctrl.ch				= FJ_SLIMBUS_CH0;
	slimbus_ctrl.manager_mode	= FJ_SLIMBUS_MANAGER_MODE_ENABLE;
	slimbus_ctrl.fr_en			= FJ_SLIMBUS_FR_ENABLE;
	slimbus_ctrl.src_thr		= 16;
	slimbus_ctrl.sink_thr		= 0;
	slimbus_ctrl.manager_int_cb = (SLIMBUS_CALLBACK)ct_slimbus_manager_int_cb;
	slimbus_ctrl.data_port_int_cb = (SLIMBUS_CALLBACK)ct_slimbus_data_port_int_cb;
	ret = FJ_Slimbus_Ctrl(&slimbus_ctrl);
	FJ_Debug_Print_Info(("FJ_Slimbus_Ctrl(%d) completed. ret=0x%08X\n", slimbus_ctrl.ch, ret));

	// Start receive task
	ercd = OS_User_Ref_Tsk( TID_SLIMBUS_CONTROLLER, &pk_rtsk );
	if (ercd == D_OS_USER_E_OK && pk_rtsk.tskstat == D_OS_USER_TTS_DMT) {
		OS_User_Sta_Tsk( TID_SLIMBUS_CONTROLLER, 0 ) ;
	}

	// FJ_Slimbus_Start Start
	ret = FJ_Slimbus_Start(FJ_SLIMBUS_CH1);
	FJ_Debug_Print_Info(("FJ_Slimbus_Start(1) completed. ret=0x%08X\n", ret));
	ret = FJ_Slimbus_Start(FJ_SLIMBUS_CH0);
	FJ_Debug_Print_Info(("FJ_Slimbus_Start(0) completed. ret=0x%08X\n", ret));

	// wait 5 REPORT_PRESENT message
	while (gSlimbus_Run_Status < 5) {
		OS_User_Dly_Tsk(1);
	}
	gSlimbus_Run_Status = 0;


	// enumeration Start
	ct_slimbus_enumeration(FJ_SLIMBUS_CH0);
	FJ_Debug_Print_Info(("ct_slimbus_enumeration completed.\n"));

	// FJ_Slimbus_Read_Data Start
	gSlimbus_Read_Data_Size = 16;
	memset(gSlimbus_Read_Data, 0, sizeof(gSlimbus_Read_Data));
	FJ_MEMORY_Dcache_Addr( FJ_MEMORY_CACHE_L1_L2, FJ_MEMORY_CACHE_CLEAN_FLUSH, (UINT32)gSlimbus_Read_Data, sizeof(gSlimbus_Read_Data) );
	ret = FJ_Slimbus_Read_Data(FJ_SLIMBUS_CH1, 3, gSlimbus_Read_Data, gSlimbus_Read_Data_Size, (SLIMBUS_CALLBACK)ct_slimbus_dma_int_read_cb);
	FJ_Debug_Print_Info(("FJ_Slimbus_Read_Data(1) completed. ret=0x%08X\n", ret));

	// FJ_Slimbus_Write_Data Start
	gSlimbus_Write_Data_Size = 16;
	ret = FJ_Slimbus_Write_Data(FJ_SLIMBUS_CH0, 4, (UINT32*)gSlimbus_Write_Data, gSlimbus_Write_Data_Size, (SLIMBUS_CALLBACK)ct_slimbus_dma_int_write_cb);
	FJ_Debug_Print_Info(("FJ_Slimbus_Write_Data(0) completed. ret=0x%08X\n", ret));

	// wait receive data
	while (gSlimbus_Run_Status < 1) {
		OS_User_Dly_Tsk(1);
	}
	gSlimbus_Run_Status = 0;

	// disconnect Start
	ct_slimbus_disconnect(FJ_SLIMBUS_CH0);
	FJ_Debug_Print_Info(("disconnect(0) completed.\n"));
	ct_slimbus_disconnect(FJ_SLIMBUS_CH1);
	FJ_Debug_Print_Info(("disconnect(1) completed.\n"));

	// FJ_Slimbus_Stop Start
	ret = FJ_Slimbus_Stop(FJ_SLIMBUS_CH0);
	FJ_Debug_Print_Info(("FJ_Slimbus_Stop(0) completed. ret=0x%08X\n", ret));
	ret = FJ_Slimbus_Stop(FJ_SLIMBUS_CH1);
	FJ_Debug_Print_Info(("FJ_Slimbus_Stop(1) completed. ret=0x%08X\n", ret));

	// FJ_Slimbus_Close Start
	ret = FJ_Slimbus_Close(FJ_SLIMBUS_CH0);
	FJ_Debug_Print_Info(("FJ_Slimbus_Close(0) completed. ret=0x%08X\n", ret));
	ret = FJ_Slimbus_Close(FJ_SLIMBUS_CH1);
	FJ_Debug_Print_Info(("FJ_Slimbus_Close(1) completed. ret=0x%08X\n", ret));
}
@endcode
*/
extern	FJ_ERR_CODE FJ_Slimbus_Open( FJ_SLIMBUS_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function close SLIMBus channel.
@param[in]	ch		Channel number.	See @ref FJ_SLIMBUS_CH.
@retval FJ_ERR_OK	Success.
@remarks
	 - This API Sample code is See @ref FJ_Slimbus_Open.<br>
*/
extern	FJ_ERR_CODE FJ_Slimbus_Close( FJ_SLIMBUS_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set SLIMBus operation condition and callback function for the specified channel.
@param [in]	slimbus_ctrl	SLIMBus Control data. See @ref FJ_SLIMBUS_CTRL.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
@remarks
	 - This API Sample code is See @ref FJ_Slimbus_Open.<br>
*/
extern	FJ_ERR_CODE FJ_Slimbus_Ctrl( FJ_SLIMBUS_CTRL const* const slimbus_ctrl );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get SLIMBus operation condition and callback function for the specified channel.
@param[in]		ch				Channel number.	See @ref FJ_SLIMBUS_CH.
@param [out]	slimbus_ctrl	SLIMBus Control data. See @ref FJ_SLIMBUS_CTRL.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
*/
extern	FJ_ERR_CODE FJ_Slimbus_Get_Ctrl( FJ_SLIMBUS_CH ch, FJ_SLIMBUS_CTRL* const slimbus_ctrl );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Start SLIMBus Manager.<br>
@param[in]	ch	Channel number.	See @ref FJ_SLIMBUS_CH.
@retval FJ_ERR_OK	Success.
@remarks
	 - This API Sample code is See @ref FJ_Slimbus_Open.<br>
*/
extern	FJ_ERR_CODE FJ_Slimbus_Start( FJ_SLIMBUS_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Stop SLIMBus Manager.<br>
@param[in]	ch	Channel number.	See @ref FJ_SLIMBUS_CH.
@retval FJ_ERR_OK	Success.
@remarks
	 - This API Sample code is See @ref FJ_Slimbus_Open.<br>
*/
extern	FJ_ERR_CODE FJ_Slimbus_Stop( FJ_SLIMBUS_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Write slimbus message.<br>
@param[in]	ch		Channel number.	See @ref FJ_SLIMBUS_CH.
@param[in]	msg		Message address.
@param[in]	size	Message size(1 to 64).
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
@remarks
	 - This API Sample code is See @ref FJ_Slimbus_Open.<br>
*/
extern FJ_ERR_CODE FJ_Slimbus_Write_Msg( FJ_SLIMBUS_CH ch, UINT32* msg, UINT32 size );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Read slimbus message.<br>
@param[in]	ch		Channel number.	See @ref FJ_SLIMBUS_CH.
@param[out]	msg		Message address.
@param[out]	size	Message size.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
@remarks
	 - This API Sample code is See @ref FJ_Slimbus_Open.<br>
*/
extern FJ_ERR_CODE FJ_Slimbus_Read_Msg( FJ_SLIMBUS_CH ch, UINT32* msg, UINT32* size );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Write data to DTA_FIFO.<br>
@param[in]	ch			Channel number.	See @ref FJ_SLIMBUS_CH.
@param[in]	dma_ch		DMA channel number(0 to 7)
@param[in]	data		Write Data address.
@param[in]	count		Write count(1 to 16). Writing one word is executed by one count.
@param[in]	pcallback	Callback function pointer when writing is completed. 
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
@remarks
	 - This API Sample code is See @ref FJ_Slimbus_Open.<br>
*/
extern FJ_ERR_CODE FJ_Slimbus_Write_Data( FJ_SLIMBUS_CH ch, UCHAR dma_ch, UINT32* data, UINT32 count, SLIMBUS_CALLBACK pcallback );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Read data from DATA_FIFO.<br>
@param[in]	ch			Channel number.	See @ref FJ_SLIMBUS_CH.
@param[in]	dma_ch		DMA channel number(0 to 7)
@param[out]	data		Read Data address.
@param[in]	count		Read count(1 to 16). Reading one word is executed by one count.
@param[in]	pcallback	Callback function pointer when reading is completed. 
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Error occurred.
@remarks
	 - This API Sample code is See @ref FJ_Slimbus_Open.<br>
*/
extern FJ_ERR_CODE FJ_Slimbus_Read_Data( FJ_SLIMBUS_CH ch, UCHAR dma_ch, UINT32* data, UINT32 count, SLIMBUS_CALLBACK pcallback );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set read data area address and size.<br>
@param[in]	ch			Channel number.	See @ref FJ_SLIMBUS_CH.
@param[in]	addr		Read data area address.	See @ref FJ_SLIMBUS_CH.
@param[in]	size		Read data area size.
@remarks
	 - This API Sample code is See @ref FJ_Slimbus_Open.<br>
*/
extern FJ_ERR_CODE FJ_Slimbus_Set_Read_Msg_Area_Addr( FJ_SLIMBUS_CH ch, UINT32 addr, UINT32 size );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get read data area address and size.<br>
@param[in]	ch			Channel number.	See @ref FJ_SLIMBUS_CH.
@param[in]	addr		Read data area address.	See @ref FJ_SLIMBUS_CH.
@param[out]	size		Read data area size.
*/
extern VOID FJ_Slimbus_Get_Read_Msg_Area_Addr( FJ_SLIMBUS_CH ch, UINT32* addr, UINT32* size );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set read data area address and size.<br>
@param[in]	ch			Channel number.	See @ref FJ_SLIMBUS_CH.
@param[in]	addr		Read data area address.	See @ref FJ_SLIMBUS_CH.
@param[in]	size		Read data area size.
@remarks
	 - This API Sample code is See @ref FJ_Slimbus_Open.<br>
*/
extern FJ_ERR_CODE FJ_Slimbus_Set_Read_Data_Area_Addr( FJ_SLIMBUS_CH ch, UINT32 addr, UINT32 size );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get read data area address and size.<br>
@param[in]	ch			Channel number.	See @ref FJ_SLIMBUS_CH.
@param[in]	addr		Read data area address.	See @ref FJ_SLIMBUS_CH.
@param[out]	size		Read data area size.
*/
extern VOID FJ_Slimbus_Get_Read_Data_Area_Addr( FJ_SLIMBUS_CH ch, UINT32* addr, UINT32* size );



/* ---------------------- */
/* External Interrupt     */
/* ---------------------- */
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set configuration of External Interrupt.
@param [in]  port			Port name. See @ref FJ_GPIO_PORT.
@param [in] *int_config		Pointer of configuration of @ref FJ_EXT_INT_CONFIG.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks
			- Specified callback function is called from task-independent portions.<br>
			  See @ref FJ_EXT_INT_CONFIG.<br>
			- Callback function to notify the port name in the argument.<br>
			  See @ref EXT_INT_CALLBACK.<br>
			- If you want to change the trigger type(@ref FJ_EXT_INT_TRIGGER_TYPE) when an interrupt occurs,
			  please call @ref FJ_Ext_Interrupt_Set_Level from the callback function.
			- This function supports only for "FPINT[31:0] port" --> Interrupt of External signal<br>
			- Please invalidate external interrupt that corresponds by FJ_Ext_Interrupt_Set_Enable() before change of a set position. <br>
			  And, the interrupt enable setting of the target port is unconditionally set disable.<br>
			  Therefore, please set "Enable" by FJ_Ext_Interrupt_Set_Enable(), if you want to permit interrupt.<br>
*/
extern FJ_ERR_CODE	FJ_Ext_Interrupt_Set_Config( FJ_GPIO_PORT port, FJ_EXT_INT_CONFIG* int_config );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set enable of External Interrupt.
@param [in] port		Port name. See @ref FJ_GPIO_PORT.
@param [in] enable		Set the state of the target port.
						  <ul>
							  <li>0:Disable
							  <li>1:Enable
						  </ul>
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks	This function supports only for "FPINT[31:0] port" --> Interrupt of External signal<br>
			And, the interrupt flag is unconditionally set "0(clear)".
*/
extern FJ_ERR_CODE	FJ_Ext_Interrupt_Set_Enable( FJ_GPIO_PORT port, UCHAR enable );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set trigger level of External Interrupt.
@param [in] port		Port name. See @ref FJ_GPIO_PORT.
@param [in] level 		External Interrupt trigger level.
						  <ul>
							  <li>0:Low level or Falling edge
							  <li>1:High level or Rising edge
						  </ul>
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks	This function supports only for "FPINT[31:0] port" --> Interrupt of External signal<br>
			And, the interrupt flag is unconditionally set "0(clear)".
*/
extern FJ_ERR_CODE FJ_Ext_Interrupt_Set_Level( FJ_GPIO_PORT port, UCHAR level );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Get trigger level of External Interrupt.
@param [in]  port		Port name. See @ref FJ_GPIO_PORT.
@param [out] *level 	External Interrupt trigger level.
						  <ul>
							  <li>0:Low level or Falling edge
							  <li>1:High level or Rising edge
						  </ul>
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks	This function supports only for "FPINT[31:0] port" --> Interrupt of External signal<br>
*/
extern FJ_ERR_CODE FJ_Ext_Interrupt_Get_Level( FJ_GPIO_PORT port, UCHAR* level );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Clear request on external interrupt.
@param [in]  port		Port name. See @ref FJ_GPIO_PORT.
@retval FJ_ERR_OK				Success.
@retval FJ_ERR_NG_INPUT_PARAM	Parameter error.
@remarks	This function supports only for "FPINT[31:0] port" --> Interrupt of External signal<br>
*/
extern FJ_ERR_CODE FJ_Ext_Interrupt_Clear_Req( FJ_GPIO_PORT port );

/* ------------------- */
/* Watchdog timer	   */
/* ------------------- */

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Initialize Watchdog function.
@retval FJ_ERR_OK	Success.
@remarks The reset state is cleared by this API, Please check the reset state by FJ_Wdog_Timer_Get_Reset_Status() before that.
*/
extern FJ_ERR_CODE FJ_Wdog_Timer_Init( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function acquire semaphore of Watchdog timer.
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Semaphore acquisition NG.
*/
extern FJ_ERR_CODE FJ_Wdog_Timer_Open( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Setting of common control variable for Watchdog function.
@param [in] wdog_mode Watchdog mode setting. See @ref FJ_WDOG_MODE.<br>
						@ref FJ_WDOG_MODE_TIMER : Timer mode (Interrupt only)<br>
						@ref FJ_WDOG_MODE_WDOG  : Watchdog mode (Assert reset)<br>
@param [in] msec Watchdog function count cycle. The unit of msec is milli seconds.<br>
				 The setable time is obtained from the following calculations.<br>
				 <br>
				 msec = Load_Value / PERIPHCLK * 1000<br>
				 <br>
				 Maximum Load_Value of the watchdog counter register is 0xFFFFFFFF.<br>
				 In the watchdog mode, if PERIPHCLK is 50[MHz], setable time is 1 to 85899[msec].<br>
				 In the timer mode, if msec exceeded the maximum value of Load_Value(0xFFFFFFFF=4294967295) as a result of calculation, API internally uses soft counter.<br>
				 The determined Load_Value is possible get from FJ_Wdog_Timer_Get_Load_Value().<br>
@param [in] user_handler Pointer of the user function when the timer interrupt.(for @ref FJ_WDOG_MODE_TIMER)
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Watchdog function access error.
@remarks  Specified callback function is called from task-independent portions.
*/
extern FJ_ERR_CODE FJ_Wdog_Timer_Ctrl( FJ_WDOG_MODE wdog_mode, ULONG msec, VP_CALLBACK user_handler );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Refresh Watchdog timer count.
@retval FJ_ERR_OK	Success.
*/
extern FJ_ERR_CODE FJ_Wdog_Timer_Refresh( VOID );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get Watchdog reset status.
@param [out]	raw_wdog_int		: Raw interrupt status
@param [out]	mask_wdog_int		: Masked interrupt status
@retval FJ_ERR_OK	Success.
*/
extern FJ_ERR_CODE FJ_Wdog_Timer_Get_Reset_Status( FJ_WDOG_RESET_STATUS* raw_wdog_int, FJ_WDOG_RESET_MASK_STATUS* mask_wdog_int );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get Load_Value and Prescaler_Value.
@param [out]	load_value			: Watchdog load value (counter value)
@retval FJ_ERR_OK	Success.
@retval FJ_ERR_NG	Watchdog function access error.
*/
extern FJ_ERR_CODE FJ_Wdog_Timer_Get_Load_Value( ULONG* load_value );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
The exclusive control of Watchdog is released
@retval FJ_ERR_OK	Success.
*/
extern FJ_ERR_CODE FJ_Wdog_Timer_Close( VOID );

/* ------------------- */
/* UDC				   */
/* ------------------- */
/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Initialize the up down counter for all channels.<br>
@param [in] zin_ctrl Select Z-terminal enable.See @ref FJ_UDC_ZIN_CTRL<br>
@retval FJ_ERR_OK    Success.<br>
@remarks	Select Z-Terminal function is used by zin_ctrl. Z-Terminal has<br>
			the function of counter clear and gate. It is possible to set it<br>
			in FJ_UDC_CTRL. See @ref FJ_UDC_CTRL. And, the function that<br>
			cannot be used by using Z-Trminal function is provided. Please<br>
			refer to the following tables for details.<br><br>
			<b>When zin_ctrl=FJ_UDC_ZIN_CTRL_DISABLE</b>
			<table><tr><th rowspan=3>Channel</th>
			           <th colspan=5>Count mode</th>
			           <th rowspan=3>Z-Terminal function <br> (*1)</th></tr>
			       <tr><th rowspan=2>Timer</th>
			           <th colspan=2>Up/Down count</th>
			           <th rowspan=2>Phase lag count <br> (Single)</th>
			           <th rowspan=2>Phase lag count <br> (Double)</th></tr>
			       <tr><th>Up count <br> (A-Terminal)</th>
			           <th>Down count <br> (B-Terminal)</th></tr>
			       <tr align=center><td>0</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td></tr>
			       <tr align=center><td>1</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td></tr>
			       <tr align=center><td>2</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td></tr>
			       <tr align=center><td>3</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td></tr>
			       <tr align=center><td>4</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td></tr>
			       <tr align=center><td>5</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td></tr></table>
			<br>
			<b>When zin_ctrl=FJ_UDC_ZIN_CTRL_ENABLE </b>
			<table><tr><th rowspan=3>Channel</th>
			           <th colspan=5>Count mode</th>
			           <th rowspan=3>Z-Terminal function <br> (*1)</th></tr>
			       <tr><th rowspan=2>Timer</th>
			           <th colspan=2>Up/Down count</th>
			           <th rowspan=2>Phase lag count <br> (Single)</th>
			           <th rowspan=2>Phase lag count <br> (Double)</th></tr>
			       <tr><th>Up count <br> (A-Terminal)</th>
			           <th>Down count <br> (B-Terminal)</th></tr>
			       <tr align=center><td>0</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td></tr>
			       <tr align=center><td>1</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td> <td>o</td></tr>
			       <tr align=center><td>2</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td> <td>x</td> <td>x</td></tr>
			       <tr align=center><td>3</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td> <td>x</td> <td>x</td></tr>
			       <tr align=center><td>4</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td> <td>x</td> <td>x</td></tr>
			       <tr align=center><td>5</td> <td>o</td> <td>o</td> <td>o</td> <td>x</td> <td>x</td> <td>x</td></tr></table>
			<br>
			(*1) Please specify the following parameter when you do not use the ZIN function.<br>
			@code
			FJ_UDC_CTRL udc_ctrl;
			udc_ctrl.zin_mode = FJ_UDC_ZIN_COUNT_CLEAR;
			udc_ctrl.zin_edge = FJ_UDC_ZIN_EDGE_DISABLE;
			@endcode
*/
extern FJ_ERR_CODE		FJ_UDC_Init( FJ_UDC_ZIN_CTRL zin_ctrl );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function acquire semaphore of UDC channel.
@param [in] ch					Channel number(range of 0..5).<br>
@retval FJ_ERR_OK				Success.<br>
@retval FJ_ERR_NG				Semaphore acquisition NG.
@retval FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.<br>
*/
extern FJ_ERR_CODE		FJ_UDC_Open( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set parameter and callback function for timer mode.<br>
@param [in] ch					Channel number(range of 0..5).<br>
@param [in] usec				Count time(usec order, range of 1..4294967295).<br>
@param [in] pCallback			Pointer of function that does call backing when timer ends.<br>
								The "factor" parameter of the callback function is unnecessary.<br>
								Please disregard the value returned by the "factor" parameter.<br>
@retval FJ_ERR_OK				Success.<br>
@retval FJ_ERR_NG_INPUT_PARAM	Input parameter error.<br>
@retval FJ_ERR_NG_CONDITION		Up down counter busy.<br>
@retval FJ_ERR_NG				Fail(corresponding to an unexpected error).<br>
@remarks Specified callback function is called from task-independent portions.<br>
*/
extern FJ_ERR_CODE		FJ_UDC_Set_Timer_Mode( FJ_PERI_CH ch, ULONG usec, UDC_CALLBACK pCallback );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Start the operation of up down counter.<br>
@param [in] ch					Channel number(range of 0..5).<br>
@retval FJ_ERR_OK				Success.<br>
@retval FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.<br>
@retval FJ_ERR_NG				Fail(corresponding to an unexpected error).<br>
*/
extern FJ_ERR_CODE		FJ_UDC_Start( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Stop the operation of up down counter.<br>
@param [in] ch					Channel number(range of 0..5).<br>
@retval FJ_ERR_OK				Success
@retval FJ_ERR_NG_INPUT_PARAM	Input Parameter Error
@retval FJ_ERR_NG				Fail(corresponding to an unexpected error).<br>
*/
extern FJ_ERR_CODE		FJ_UDC_Stop( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set parameter and callback function for up down count mode.<br>
@param [in] ch					Channel number(range of 0..5).<br>
@param [in] edge				Edge selection.See @ref FJ_UDC_COUNT_EDGE.<br>
@param [in] udc_ctrl			Each mode common settings parameter pointer.See @ref FJ_UDC_CTRL.<br>
@retval FJ_ERR_OK				Success.<br>
@retval FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.<br>
@retval FJ_ERR_NG_CONDITION		Up down counter busy.<br>
@retval FJ_ERR_NG				Fail(corresponding to an unexpected error).<br>
@remarks Specified callback function is called from task-independent portions.<br>
		 See @ref FJ_UDC_CTRL.<br>
*/
extern FJ_ERR_CODE		FJ_UDC_Set_Up_Down_Count_Mode( FJ_PERI_CH ch, FJ_UDC_COUNT_EDGE edge, FJ_UDC_CTRL const* const udc_ctrl );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set parameter and callback function for phase lag mode.<br>
@param [in] ch					Channel number(range of 0..5).<br>
@param [in] count_mode			Phase lag count mode selection.See @ref FJ_UDC_PHASE_COUNT.<br>
@param [in] udc_ctrl			Each mode common settings parameter pointer.See @ref FJ_UDC_CTRL.<br>
@retval FJ_ERR_OK				Success.<br>
@retval FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.<br>
@retval FJ_ERR_NG_CONDITION		Up down counter busy.<br>
@retval FJ_ERR_NG				Fail(corresponding to an unexpected error).<br>
@remarks	Specified callback function is called from task-independent portions.<br>
			See @ref FJ_UDC_CTRL.<br>
			<br>
			<br>
			How to count the phase lag is selected by count_mode. case of<br>
			select FJ_UDC_PHASE_COUNT_SINGLE, input signal from A-terminal<br>
			is counted by the edge of the input signal from B-terminal. case of<br>
			select FJ_UDC_PHASE_COUNT_DOUBLE, input signal from A-terminal<br>
			will be counted as above. In addition, input signal from B-terminal is<br>
			counted by the edge of the input signal from A-terminal.<br>
			<br>
			<b>When count_mode=FJ_UDC_PHASE_COUNT_SINGLE</b><br>
			<br>
			<table><tr><th>Edge of B-Terminal</th> <th>Level of A-Terminal</th> <th>Count direction</th></tr>
			       <tr align=center><td>Rise</td>  <td>H</td> 					<td>Up count</td></tr>
			       <tr align=center><td>Rise</td>  <td>L</td> 					<td>Down count</td></tr>
			       <tr align=center><td>Fall</td>  <td>H</td> 					<td>Down count</td></tr>
			       <tr align=center><td>Fall</td>  <td>L</td> 					<td>Up count</td></tr></table>
			<br>
			@image html fj_peripheral_udc_phase_lag_count_single.png<br>
			<br>
			<br>
			<b>When count_mode=FJ_UDC_PHASE_COUNT_DOUBLE</b><br>
			<br>
			<table><tr><th>Input edge</th>					<th>Edge</th>	<th>Input level</th> 			<th>Level</th>	<th>Count direction</th></tr>
			<tr align=center><td rowspan=4>B-Terminal</td>	<td>Rise</td>	<td rowspan=4>A-Terminal</td>	<td>H</td>		<td>Up count</td></tr>
			<tr align=center>								<td>Rise</td>  									<td>L</td> 		<td>Down count</td></tr>
			<tr align=center>								<td>Fall</td>  									<td>H</td> 		<td>Down count</td></tr>
			<tr align=center>								<td>Fall</td>									<td>L</td> 		<td>Up count</td></tr>
			<tr align=center><td rowspan=4>A-Terminal</td>	<td>Rise</td>	<td rowspan=4>B-Terminal</td>	<td>H</td>		<td>Down count</td></tr>
			<tr align=center>								<td>Rise</td>  									<td>L</td> 		<td>Up count</td></tr>
			<tr align=center>								<td>Fall</td>  									<td>H</td> 		<td>Up count</td></tr>
			<tr align=center>								<td>Fall</td>									<td>L</td> 		<td>Down count</td></tr></table>
			<br>
			@image html fj_peripheral_udc_phase_lag_count_double.png<br>
*/
extern FJ_ERR_CODE		FJ_UDC_Set_Phase_Lag_Mode( FJ_PERI_CH ch, FJ_UDC_PHASE_COUNT count_mode, FJ_UDC_CTRL const* const udc_ctrl );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set counter of the up down counter.<br>
@param [in] ch					Channel number(range of 0..5).<br>
@param [in] up_down_counter		counter of the up down counter.<br>
@retval FJ_ERR_OK				Success.<br>
@retval FJ_ERR_NG_CONDITION		Up down counter function busy.<br>
@retval FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.<br>
@retval FJ_ERR_NG				Fail(corresponding to an unexpected error).<br>
@remarks	When the channel is working, it becomes an error.<br>
*/
extern FJ_ERR_CODE		FJ_UDC_Set_Up_Down_Counter( FJ_PERI_CH ch, USHORT up_down_counter );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Set reload/compare counter of the up down counter.<br>
@param [in] ch					Channel number(range of 0..5).<br>
@param [in] comp_counter 		reload/compare counter of the up down counter.<br>
@retval FJ_ERR_OK				Success.<br>
@retval FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.<br>
@retval FJ_ERR_NG				Fail(corresponding to an unexpected error).<br>
*/
extern FJ_ERR_CODE		FJ_UDC_Set_Comp_Counter( FJ_PERI_CH ch, USHORT comp_counter );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set callback function for when an interrupt occurs.<br>
@param [in] ch					Channel number(range of 0..5).<br>
@param [in]	pCallBack			Call Back function pointer
@retval FJ_ERR_OK				Success.<br>
@retval FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.<br>
@remarks Specified callback function is called from task-independent portions.<br>
*/
extern FJ_ERR_CODE		FJ_UDC_Set_CallBack( FJ_PERI_CH ch, UDC_CALLBACK pCallBack );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Return counter of the up down counter.<br>
@param [in] ch					Channel number(range of 0..5).<br>
@retval 0						Input Parameter Error.<br>
@retval USHORT 					Counter of the up down counter.<br>
*/
extern USHORT			FJ_UDC_Get_Up_Down_Counter( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Get direction of the count immediately before.
@param [in] ch						Channel number(range of 0..5).<br>
@retval FJ_UDC_COUNT_DIR_NO_INPUT	Input Parameter Error.<br>
@retval FJ_UDC_COUNT_DIR 			Flag of direction of the count.<br>
*/
extern FJ_UDC_COUNT_DIR	FJ_UDC_Get_Up_Down_Flag( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Clear counter of the up down counter.<br>
@param [in] ch					Channel number(range of 0..5).<br>
@retval FJ_ERR_OK				Success.<br>
@retval FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.<br>
@retval FJ_ERR_NG				Fail(corresponding to an unexpected error).<br>
*/
extern FJ_ERR_CODE		FJ_UDC_Clear_Up_Down_Counter( FJ_PERI_CH ch );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Close up down counter, and specified channel release.<br>
@param [in] ch					Channel number(range of 0..5).<br>
@retval FJ_ERR_OK				Success.<br>
@retval FJ_ERR_NG_INPUT_PARAM	Input Parameter Error.<br>
@retval FJ_ERR_NG				Fail(corresponding to an unexpected error).<br>
*/
extern FJ_ERR_CODE		FJ_UDC_Close( FJ_PERI_CH ch );



/*@} --- end of fj_peripheral_api */

///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
/*+@weakgroup fj_peripheral_message
@{*/
/*@} --- end of fj_peripheral_message */

#endif	// _FJ_PERIPHERAL_H
