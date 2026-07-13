/**
 * @file		__jctpmc_adc.h
 * @brief		Definition JCTPMC A/D Convertor Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


#ifndef _IO_PMC_ADC_H_
#define _IO_PMC_ADC_H_

/* A/D Convertor */

/* ADC1 */
union io_pmc_adc_adc1 {
	unsigned long		word;
	struct {
		unsigned long	ADCS			:8;
		unsigned long	STRG			:1;
		unsigned long	XPD				:1;
		unsigned long	TGS				:2;
		unsigned long	ADRST			:1;
		unsigned long	TSEL			:1;
		unsigned long	XPDCTL			:1;
		unsigned long					:17;
	}bit;
};

/* ADC2 */
union io_pmc_adc_adc2 {
	unsigned long		word;
	struct {
		unsigned long					:12;
		unsigned long	STG				:3;
		unsigned long	ETEDG			:1;
		unsigned long					:16;
	}bit;
};

/* ADMR */
union io_pmc_adc_admr {
	unsigned long		word;
	struct {
		unsigned long	STS				:8;
		unsigned long	MD				:2;
		unsigned long	INTEL			:1;
		unsigned long	INTEH			:1;
		unsigned long	INTE			:1;
		unsigned long					:19;
	}bit;
};

/* ADST */
union io_pmc_adc_adst {
	unsigned long		word;
	struct {
		unsigned long	BUSY			:1;
		unsigned long	INTL			:1;
		unsigned long	INTH			:1;
		unsigned long	INTF			:1;
		unsigned long	CS				:3;
		unsigned long					:25;
	}bit;
};

/* ADDx (x=0-6) */
union io_pmc_adc_add {
	unsigned long		word;
	struct {
		unsigned long	ADD				:10;
		unsigned long					:22;
	}bit;
};

/* ADCRL */
union io_pmc_adc_adcrl {
	unsigned long		word;
	struct {
		unsigned long	ADCRL			:10;
		unsigned long					:22;
	}bit;
};

/* ADCRH */
union io_pmc_adc_adcrh {
	unsigned long		word;
	struct {
		unsigned long	ADCRH			:10;
		unsigned long					:22;
	}bit;
};


/* A/D Convertor */
struct io_pmc_adc {
	union	io_pmc_adc_adc1			ADC1;										/* 1DFF_(5000 - 5003h)	*/
	union	io_pmc_adc_adc2			ADC2;										/* 1DFF_(5004 - 5007h)	*/
	union	io_pmc_adc_admr			ADMR;										/* 1DFF_(5008 - 500Bh)	*/
	union	io_pmc_adc_adst			ADST;										/* 1DFF_(500C - 500Fh)	*/
	union	io_pmc_adc_add			ADD[8];										/* 1DFF_(5010 - 502Fh)	*/
	union	io_pmc_adc_adcrl		ADCRL;										/* 1DFF_(5030 - 5033h)	*/
	union	io_pmc_adc_adcrh		ADCRH;										/* 1DFF_(5034 - 5037h)	*/
	
	unsigned char					dmy_pmc_adc_038_FFF[0x1000 - 0x038];		/* 1DFF_(5038 - 5FFFh)	*/
};

#endif	// _IO_PMC_ADC_H_
