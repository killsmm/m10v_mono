/**
 * @file		__jctpmc_gpio.h
 * @brief		Definition JCTPMC GPIO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


#ifndef _IO_PMC_GPIO_H_
#define _IO_PMC_GPIO_H_

/* GPIO */

/* PDR */
union io_pmc_gpio_pdr {
	unsigned long		word[3];
	struct {
		/* PDR10  */
		unsigned long	P00				:1;
		unsigned long	P01				:1;
		unsigned long	P02				:1;
		unsigned long	P03				:1;
		unsigned long	P04				:1;
		unsigned long	P05				:1;
		unsigned long	P06				:1;
		unsigned long	P07				:1;
		unsigned long	P10				:1;
		unsigned long	P11				:1;
		unsigned long	P12				:1;
		unsigned long	P13				:1;
		unsigned long	P14				:1;
		unsigned long	P15				:1;
		unsigned long	P16				:1;
		unsigned long	P17				:1;
		unsigned long	PWE00			:1;
		unsigned long	PWE01			:1;
		unsigned long	PWE02			:1;
		unsigned long	PWE03			:1;
		unsigned long	PWE04			:1;
		unsigned long	PWE05			:1;
		unsigned long	PWE06			:1;
		unsigned long	PWE07			:1;
		unsigned long	PWE10			:1;
		unsigned long	PWE11			:1;
		unsigned long	PWE12			:1;
		unsigned long	PWE13			:1;
		unsigned long	PWE14			:1;
		unsigned long	PWE15			:1;
		unsigned long	PWE16			:1;
		unsigned long	PWE17			:1;
		/* PDR32  */
		unsigned long	P20				:1;
		unsigned long	P21				:1;
		unsigned long	P22				:1;
		unsigned long	P23				:1;
		unsigned long	P24				:1;
		unsigned long	P25				:1;
		unsigned long	P26				:1;
		unsigned long	P27				:1;
		unsigned long	P30				:1;
		unsigned long	P31				:1;
		unsigned long	P32				:1;
		unsigned long	P33				:1;
		unsigned long	P34				:1;
		unsigned long	P35				:1;
		unsigned long	P36				:1;
		unsigned long	P37				:1;
		unsigned long	PWE20			:1;
		unsigned long	PWE21			:1;
		unsigned long	PWE22			:1;
		unsigned long	PWE23			:1;
		unsigned long	PWE24			:1;
		unsigned long	PWE25			:1;
		unsigned long	PWE26			:1;
		unsigned long	PWE27			:1;
		unsigned long	PWE30			:1;
		unsigned long	PWE31			:1;
		unsigned long	PWE32			:1;
		unsigned long	PWE33			:1;
		unsigned long	PWE34			:1;
		unsigned long	PWE35			:1;
		unsigned long	PWE36			:1;
		unsigned long	PWE37			:1;
		/* PDR54  */
		unsigned long					:4;
		unsigned long	P44				:1;
		unsigned long	P45				:1;
		unsigned long	P46				:1;
		unsigned long					:13;
		unsigned long	PWE44			:1;
		unsigned long	PWE45			:1;
		unsigned long	PWE46			:1;
		unsigned long					:9;
	}bit;
};

/* DDR */
union io_pmc_gpio_ddr {
	unsigned long		word[3];
	struct {
		/* DDR10 */
		unsigned long	P00				:1;
		unsigned long	P01				:1;
		unsigned long	P02				:1;
		unsigned long	P03				:1;
		unsigned long	P04				:1;
		unsigned long	P05				:1;
		unsigned long	P06				:1;
		unsigned long	P07				:1;
		unsigned long	P10				:1;
		unsigned long	P11				:1;
		unsigned long	P12				:1;
		unsigned long	P13				:1;
		unsigned long	P14				:1;
		unsigned long	P15				:1;
		unsigned long	P16				:1;
		unsigned long	P17				:1;
		unsigned long					:16;
		/* DDR32 */
		unsigned long	P20				:1;
		unsigned long	P21				:1;
		unsigned long	P22				:1;
		unsigned long	P23				:1;
		unsigned long	P24				:1;
		unsigned long	P25				:1;
		unsigned long	P26				:1;
		unsigned long	P27				:1;
		unsigned long	P30				:1;
		unsigned long	P31				:1;
		unsigned long	P32				:1;
		unsigned long	P33				:1;
		unsigned long	P34				:1;
		unsigned long	P35				:1;
		unsigned long	P36				:1;
		unsigned long	P37				:1;
		unsigned long					:16;
		/* DDR54 */
		unsigned long					:4;
		unsigned long	P44				:1;
		unsigned long	P45				:1;
		unsigned long	P46				:1;
		unsigned long					:25;
	}bit;
};

/* EPCR */
union io_pmc_gpio_epcr {
	unsigned long		word[3];
	struct {
		/* EPCR10 */
		unsigned long					:32;
		/* EPCR32 */
		unsigned long					:2;
		unsigned long	P22				:1;
		unsigned long	P23				:1;
		unsigned long					:5;
		unsigned long	P31				:1;
		unsigned long					:1;
		unsigned long	P33				:1;
		unsigned long					:1;
		unsigned long	P35				:1;
		unsigned long					:1;
		unsigned long	P37				:1;
		unsigned long					:16;
		/* EPCR54 */
		unsigned long					:5;
		unsigned long	P45				:1;
		unsigned long					:26;
	}bit;
};

/* PUDER */
union io_pmc_gpio_puder {
	unsigned long		word[3];
	struct {
		/* PUDER10 */
		unsigned long	P00				:1;
		unsigned long	P01				:1;
		unsigned long	P02				:1;
		unsigned long	P03				:1;
		unsigned long	P04				:1;
		unsigned long	P05				:1;
		unsigned long	P06				:1;
		unsigned long	P07				:1;
		unsigned long	P10				:1;
		unsigned long	P11				:1;
		unsigned long	P12				:1;
		unsigned long	P13				:1;
		unsigned long	P14				:1;
		unsigned long	P15				:1;
		unsigned long	P16				:1;
		unsigned long	P17				:1;
		unsigned long					:16;
		/* PUDER32 */
		unsigned long	P20				:1;
		unsigned long	P21				:1;
		unsigned long	P22				:1;
		unsigned long	P23				:1;
		unsigned long	P24				:1;
		unsigned long	P25				:1;
		unsigned long	P26				:1;
		unsigned long	P27				:1;
		unsigned long	P30				:1;
		unsigned long	P31				:1;
		unsigned long	P32				:1;
		unsigned long	P33				:1;
		unsigned long	P34				:1;
		unsigned long	P35				:1;
		unsigned long	P36				:1;
		unsigned long	P37				:1;
		unsigned long					:16;
		/* PUDER54 */
		unsigned long					:4;
		unsigned long	P44				:1;
		unsigned long	P45				:1;
		unsigned long	P46				:1;
		unsigned long					:25;
	}bit;
};

/* PUDCR */
union io_pmc_gpio_pudcr {
	unsigned long		word[3];
	struct {
		/* PUDCR10 */
		unsigned long	P00				:1;
		unsigned long	P01				:1;
		unsigned long	P02				:1;
		unsigned long	P03				:1;
		unsigned long	P04				:1;
		unsigned long	P05				:1;
		unsigned long	P06				:1;
		unsigned long	P07				:1;
		unsigned long	P10				:1;
		unsigned long	P11				:1;
		unsigned long	P12				:1;
		unsigned long	P13				:1;
		unsigned long	P14				:1;
		unsigned long	P15				:1;
		unsigned long	P16				:1;
		unsigned long	P17				:1;
		unsigned long					:16;
		/* PUDCR32 */
		unsigned long	P20				:1;
		unsigned long	P21				:1;
		unsigned long	P22				:1;
		unsigned long	P23				:1;
		unsigned long	P24				:1;
		unsigned long	P25				:1;
		unsigned long	P26				:1;
		unsigned long	P27				:1;
		unsigned long	P30				:1;
		unsigned long	P31				:1;
		unsigned long	P32				:1;
		unsigned long	P33				:1;
		unsigned long	P34				:1;
		unsigned long	P35				:1;
		unsigned long	P36				:1;
		unsigned long	P37				:1;
		unsigned long					:16;
		/* PUDCR54 */
		unsigned long					:4;
		unsigned long	P44				:1;
		unsigned long	P45				:1;
		unsigned long	P46				:1;
		unsigned long					:25;
	}bit;
};

/* GPIO */
struct io_pmc_gpio {
	union io_pmc_gpio_pdr		PDR;									/* 1DFF_(B000 - B00Bh) */
	
	unsigned char				dmy_gpio_00C_0FF[0x100 - 0x00C];		/* 1DFF_(B00C - B0FFh)	*/
	
	union io_pmc_gpio_ddr		DDR;									/* 1DFF_(B100 - B10Bh) */
	
	unsigned char				dmy_gpio_10C_1FF[0x200 - 0x10C];		/* 1DFF_(B10C - B1FFh)	*/
	
	union io_pmc_gpio_epcr		EPCR; 									/* 1DFF_(B200 - B20Bh) */
	
	unsigned char				dmy_gpio_20C_2FF[0x300 - 0x20C];		/* 1DFF_(B20C - B2FFh)	*/
	
	union io_pmc_gpio_puder		PUDER; 									/* 1DFF_(B300 - B30Bh) */
	
	unsigned char				dmy_gpio_30C_3FF[0x400 - 0x30C];		/* 1DFF_(B30C - B3FFh)	*/
	
	union io_pmc_gpio_pudcr		PUDCR; 									/* 1DFF_(B400 - B40Bh) */
	
	unsigned char				dmy_gpio_40C_FFF[0x1000 - 0x40C];		/* 1DFF_(B40C - BFFFh)	*/
};

#endif	// _IO_PMC_GPIO_H_
