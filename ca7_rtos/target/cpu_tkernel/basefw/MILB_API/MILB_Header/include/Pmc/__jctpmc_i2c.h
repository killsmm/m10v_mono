/**
 * @file		__jctpmc_i2c.h
 * @brief		Definition JCTPMC I2C Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


#ifndef _IO_PMC_I2C_H_
#define _IO_PMC_I2C_H_

/* I2C */
union io_pmc_i2c_sdat {
	unsigned long		word;
	struct {
		unsigned long	DATA		:8;
		
		unsigned long				:24;
	}bit;
};

union io_pmc_i2c_st {
	unsigned long		word;
	struct {
		unsigned long	MODE 		:5;
		unsigned long				:2;
		unsigned long	INT 		:1;
		
		unsigned long				:24;
	}bit;
};

union io_pmc_i2c_cst {
	unsigned long		word;
	struct {
		unsigned long	BB			:1;
		unsigned long	TOCDIV		:2;
		unsigned long	TERR		:1;
		unsigned long	TSDA		:1;
		unsigned long	TGSCL		:1;
		unsigned long	PECNEXT		:1;
		unsigned long	PECFAULT	:1;
		
		unsigned long				:24;
	}bit;
};

union io_pmc_i2c_ctl1 {
	unsigned long		word;
	struct {
		unsigned long	START		:1;
		unsigned long	STOP		:1;
		unsigned long	INTEN		:1;
		unsigned long				:1;
		unsigned long	ACK			:1;
		unsigned long	GCMEN		:1;
		unsigned long	SMBARE		:1;
		unsigned long	CLRST		:1;
		
		unsigned long				:24;
	}bit;
};

union io_pmc_i2c_addr {
	unsigned long		word;
	struct {
		unsigned long	ADDR		:7;
		unsigned long	SAEN		:1;
		
		unsigned long				:24;
	}bit;
};

union io_pmc_i2c_ctl2 {
	unsigned long		word;
	struct {
		unsigned long	ENABLE		:1;
		unsigned long	SCLFRQ		:7;
		
		unsigned long				:24;
	}bit;
};

union io_pmc_i2c_topr {
	unsigned long		word;
	struct {
		unsigned long	TOPR 		:8;
		
		unsigned long				:24;
	}bit;
};

union io_pmc_i2c_ctl3 {
	unsigned long		word;
	struct {
		unsigned long	S10ADR		:3;
		unsigned long	S10EN		:1;
		unsigned long	HSCDIV 		:4;
		
		unsigned long				:24;
	}bit;
};


/* UART */
struct io_pmc_i2c {
	union	io_pmc_i2c_sdat		SDAT;							/* 1DFF_(7000 - 7003h) */
	union	io_pmc_i2c_st		ST;								/* 1DFF_(7004 - 7007h) */
	union	io_pmc_i2c_cst		CST;							/* 1DFF_(7008 - 700Bh) */
	union	io_pmc_i2c_ctl1		CTL1;							/* 1DFF_(700C - 700Fh) */
	union	io_pmc_i2c_addr		ADDR;							/* 1DFF_(7010 - 7013h) */
	union	io_pmc_i2c_ctl2		CTL2;							/* 1DFF_(7014 - 7017h) */
	union	io_pmc_i2c_topr		TOPR;							/* 1DFF_(7018 - 701Bh) */
	union	io_pmc_i2c_ctl3		CTL3;							/* 1DFF_(701C - 701Fh) */
	unsigned char		dmy_pmc_i2c_7020_73FF[0x7400-0x7020];	/* 1DFF_(7020 - 73FFh) */
};

#endif	// _IO_PMC_I2C_H_
