/**
 * @file		_jctpmc.h
 * @brief		Definition JCTPMC Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


#ifndef _IO_PMC_H_
#define _IO_PMC_H_

#include "__jctpmc_adc.h"
#include "__jctpmc_crg11.h"
#include "__jctpmc_calendar.h"
#include "__jctpmc_exiu.h"
#include "__jctpmc_gpio.h"
#include "__jctpmc_hdmac.h"
#include "__jctpmc_i2c.h"
#include "__jctpmc_pmu.h"
#include "__jctpmc_spi.h"
#include "__jctpmc_sensor_if.h"
#include "__jctpmc_uart.h"
#include "__jctpmc_watchdog.h"

/* PMC */

/* MDR (Mode indication register) */
union io_pmc_mdr {
	unsigned long		word;
	struct {
		unsigned long	MD				:6;
		unsigned long					:22;
		unsigned long	VER				:4;
	}bit;
};

/* PERSELM1 (Peripheral blcok function select 1) register */
union io_pmc_perselm1 {
	unsigned long		word;
	struct {
		unsigned long	I2CHCNT			:2;
		unsigned long	SDANCH			:1;
		unsigned long	SCLNCH			:1;
		unsigned long					:4;
		unsigned long	SP0MISOPE		:1;
		unsigned long	SP0MOSIPE		:1;
		unsigned long	SP0SCKPE		:1;
		unsigned long	SP0CSPE			:1;
		unsigned long	SP0MISOUDC		:1;
		unsigned long	SP0MOSIUDC		:1;
		unsigned long	SP0SCKUDC		:1;
		unsigned long	SP0CSUDC		:1;
		unsigned long					:16;
	}bit;
};

/* DBCNTM1 (Drivability Control M1) register */
union io_pmc_dbcntm1 {
	unsigned long		word;
	struct {
		unsigned long	T00DCM			:2;
		unsigned long	T01DCM			:2;
		unsigned long	T02DCM			:2;
		unsigned long	T03DCM			:2;
		unsigned long	T04DCM			:2;
		unsigned long	T05DCM			:2;
		unsigned long	T06DCM			:2;
		unsigned long					:18;
	}bit;
};

/* ASV(Adaptive Supply Voltage) code register */
union io_pmc_asv {
	unsigned long		word;
	struct {
		unsigned long	ASV				:5;
		unsigned long					:27;
	}bit;
};

/* GPIO initial inputs code register */
union io_pmc_gpio_iicr {
	unsigned long		word;
	struct {
		unsigned long	P07_00			:8;
		unsigned long	P17_10			:8;
		unsigned long					:16;
	}bit;
};

/* Internal reset signal indication register */
union io_pmc_irsir {
	unsigned long		word;
	struct {
		unsigned long	HRRSTX			:1;
		unsigned long					:31;
	}bit;
};


/* Cycle Time Counter CTL register */
union io_pmc_ctc_ctl {
	unsigned long		word;
	struct {
		unsigned long	TRG				:1;
		unsigned long					:31;
	}bit;
};

/* Cycle Time Counter INTCTL register */
union io_pmc_ctc_intctl {
	unsigned long		word;
	struct {
		unsigned long	MEIE			:1;
		unsigned long					:3;
		unsigned long	SWFI			:1;
		unsigned long					:27;
	}bit;
};

/* Cycle Time Counter INTSTAT register */
union io_pmc_ctc_intstat {
	unsigned long		word;
	struct {
		unsigned long	MEST			:1;
		unsigned long					:31;
	}bit;
};

/* Cycle Time Counter OBCLEN register */
union io_pmc_ctc_obclen {
	unsigned long		word;
	struct {
		unsigned long	OBCLEN			:16;
		unsigned long					:16;
	}bit;
};


/* 32-Bit Reload Timer TMCSR register */
union io_pmc_tmr_tmcsr {
	unsigned long		word;
	struct {
		unsigned long	TRG				:1;
		unsigned long	CNTE			:1;
		unsigned long	UF				:1;
		unsigned long	INTE			:1;
		unsigned long	RELD			:1;
		unsigned long	OUTL			:1;
		unsigned long					:4;
		unsigned long	CSL				:2;
		unsigned long	RELS			:1;
		unsigned long					:19;
	}bit;
};


/* Cycle Time Counter */
struct io_pmc_ctc {
	union io_pmc_ctc_ctl		CTL;									/* 1DFF_(1000 - 1003h) */
	union io_pmc_ctc_intctl		INTCTL;									/* 1DFF_(1004 - 1007h) */
	union io_pmc_ctc_intstat	INTSTAT; 								/* 1DFF_(1008 - 100Bh) */
	union io_pmc_ctc_obclen		OBCLEN; 								/* 1DFF_(100C - 100Fh) */
	unsigned long				CNTVAL; 								/* 1DFF_(1010 - 1013h) */
	
	unsigned char				dmy_pmc_tmr_0014_0FFF[0x1000 - 0x014];	/* 1DFF_(1014 - 1FFFh) */
};


/* 32-Bit Reload Timer */
struct io_pmc_tmr {
	union io_pmc_tmr_tmcsr	TMCSR;										/* 1DFF_(6000 - 6003h) */
	unsigned long			TMR;										/* 1DFF_(6004 - 6007h) */
	unsigned long			TMRLR0; 									/* 1DFF_(6008 - 600Bh) */
	unsigned long			TMRLR1; 									/* 1DFF_(600C - 600Fh) */
	
	unsigned char			dmy_pmc_tmr_0010_0FFF[0x1000 - 0x010];		/* 1DFF_(6010 - 6FFFh) */
};

/* PMC */
struct io_pmc {
	struct	io_pmc_hdmac 		HDMAC;									/* 1DC0_0000h	*/
	struct	io_pmc_spi 			SPI;									/* 1DD0_0000h	*/
	struct	io_pmc_uart 		UART;									/* 1DE0_0000h	*/
	
	unsigned char				dmy_pmc_1DF00000_1DFF0FFF[0x1DFF1000 - 0x1DF00000];		/* 1DF0_0000 - 1DFF_0FFFh	*/
	
	struct	io_pmc_ctc 			CTC;									/* 1DFF_1000h	*/
	
	unsigned char				dmy_pmc_1DFF2000_1DFF2FFF[0x1DFF3000 - 0x1DFF2000];		/* 1DFF_2000 - 1DFF_2FFFh	*/
	
	struct	io_pmc_exiu			EXIU;									/* 1DFF_3000h	*/
	union	io_pmc_mdr			MDR;									/* 1DFF_4000h	*/
	union	io_pmc_perselm1		PERSELM1;								/* 1DFF_4004h	*/
	union	io_pmc_dbcntm1		DBCNTM1;								/* 1DFF_4008h	*/
	
	unsigned char				dmy_pmc_1DFF400C_1DFF400F[0x1DFF4010 - 0x1DFF400C];		/* 1DFF_400C - 1DFF_400Fh	*/
	
	union	io_pmc_asv			ASV;									/* 1DFF_4010h	*/
	
	unsigned char				dmy_pmc_1DFF4014_1DFF4FFF[0x1DFF5000 - 0x1DFF4014];		/* 1DFF_4014 - 1DFF_4FFFh	*/
	
	struct	io_pmc_adc	 		ADC;									/* 1DFF_5000h	*/
	struct	io_pmc_tmr			TMR;									/* 1DFF_6000h	*/
	struct	io_pmc_i2c 			I2C;									/* 1DFF_7000h	*/
	
	unsigned char				dmy_pmc_1DFF7400_1DFF7FFF[0x1DFF8000 - 0x1DFF7400];		/* 1DFF_7400 - 1DFF_7FFFh	*/
	
	union	io_pmc_gpio_iicr 	GPIO_IICR;								/* 1DFF_8000h	*/
	unsigned char				dmy_pmc_1DFF8004_1DFF800F[0x1DFF8010 - 0x1DFF8004];		/* 1DFF_8004 - 1DFF_800Fh	*/
	union	io_pmc_irsir	 	IRSIR;									/* 1DFF_8010h	*/
	
	unsigned char				dmy_pmc_1DFF8014_1DFF9FFF[0x1DFFA000 - 0x1DFF8014];		/* 1DFF_8014 - 1DFF_9FFFh	*/
	
	struct	io_pmc_sensor_if	TEMP_SENSOR_IF;							/* 1DFF_A000h	*/
	struct	io_pmc_gpio 		GPIO;									/* 1DFF_B000h	*/
	struct	io_pmc_calendar 	CALENDAR;								/* 1DFF_C000h	*/
	struct	io_pmc_watchdog 	WATCHDOG;								/* 1DFF_D000h	*/
	struct	io_pmc_crg11 		CRG11;									/* 1DFF_E000h	*/
	struct	io_pmc_pmu 			PMU;									/* 1DFF_F000h	*/
};

extern volatile struct io_pmc	IO_PMC;									/* addr 1DC0_0000h- */

#endif	// _IO_PMC_H_
