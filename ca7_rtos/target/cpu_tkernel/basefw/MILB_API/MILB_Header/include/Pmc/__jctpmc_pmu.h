/**
 * @file		__jctpmc_pmu.h
 * @brief		Definition JCTPMC PMU Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


#ifndef _IO_PMC_PMU_H_
#define _IO_PMC_PMU_H_

/* PMU */

/* PMU_CTL */
union io_pmc_pmu_ctl {
	unsigned long		word;
	struct {
		unsigned long	PUSET			:1;
		unsigned long	PDSET			:1;
		unsigned long					:6;
		unsigned long	SETEN			:8;
		unsigned long					:15;
		unsigned long	CKGE			:1;
	}bit;
};

/* PMU_MSR */
union io_pmc_pmu_msr {
	unsigned long		word;
	struct {
		unsigned long	RSTSEL			:1;
		unsigned long					:3;
		unsigned long	WTOFFSEL		:1;
		unsigned long					:27;
	}bit;
};

/* PMU_INT_EN */
union io_pmc_pmu_int_en {
	unsigned long		word;
	struct {
		unsigned long					:4;
		unsigned long	PONIE			:1;
		unsigned long	PDNIE			:1;
		unsigned long					:26;
	}bit;
};

/* PMU_INT_ST */
union io_pmc_pmu_int_st {
	unsigned long		word;
	struct {
		unsigned long					:4;
		unsigned long	PONI			:1;
		unsigned long	PDNI			:1;
		unsigned long					:26;
	}bit;
};

/* PMU_INT_CLR */
union io_pmc_pmu_int_clr {
	unsigned long		word;
	struct {
		unsigned long					:4;
		unsigned long	PONIC			:1;
		unsigned long	PDNIC			:1;
		unsigned long					:26;
	}bit;
};

/* PMU_STR */
union io_pmc_pmu_str {
	unsigned long		word;
	struct {
		unsigned long	PMU_ST			:1;
		unsigned long					:14;
		unsigned long	PWR_SW			:1;
		unsigned long					:16;
	}bit;
};

/* PWR_ON_CYCx (x=0-1) */
union io_pmc_pmu_pwr_on_cyc {
	unsigned long		word;
	struct {
		unsigned long	PCYC0			:8;
		unsigned long	PCYC1			:8;
		unsigned long	PCYC2			:8;
		unsigned long	PCYC3			:8;
	}bit;
};

/* PWR_DLY_CTLx (x=0-1) */
union io_pmc_pmu_pwr_dly_ctl {
	unsigned long		word;
	struct {
		unsigned long	PON0			:5;
		unsigned long					:3;
		unsigned long	PON1			:5;
		unsigned long					:3;
		unsigned long	PON2			:5;
		unsigned long					:3;
		unsigned long	PON3			:5;
		unsigned long					:3;
	}bit;
};

/* PWR_TM_CTL0/PWR_TM_S1_CTL0/PWR_TM_S2_CTL0 */
union io_pmc_pmu_pwr_tm_ctl {
	unsigned long		word;
	struct {
		unsigned long	PTC0			:3;
		unsigned long					:1;
		unsigned long	PTC1			:3;
		unsigned long					:1;
		unsigned long	PTC2			:3;
		unsigned long					:1;
		unsigned long	PTC3			:3;
		unsigned long					:1;
		unsigned long	PTC4			:3;
		unsigned long					:1;
		unsigned long	PTC5			:3;
		unsigned long					:1;
		unsigned long	PTC6			:3;
		unsigned long					:1;
		unsigned long	PTC7			:3;
		unsigned long					:1;
	}bit;
};

/* PMU_CFG */
union io_pmc_pmu_cfg {
	unsigned long		word;
	struct {
		unsigned long	CFG				:8;
		unsigned long					:24;
	}bit;
};

/* PMU */
struct io_pmc_pmu {
	union	io_pmc_pmu_ctl			PMU_CTL;									/* 1DFF_(F000 - F003h)	*/
	union	io_pmc_pmu_msr			PMU_MSR;									/* 1DFF_(F004 - F007h)	*/
	union	io_pmc_pmu_int_en		PMU_INT_EN;									/* 1DFF_(F008 - F00Bh)	*/
	union	io_pmc_pmu_int_st		PMU_INT_ST;									/* 1DFF_(F00C - F00Fh)	*/
	union	io_pmc_pmu_int_clr		PMU_INT_CLR;								/* 1DFF_(F010 - F013h)	*/
	union	io_pmc_pmu_str			PMU_STR;									/* 1DFF_(F014 - F017h)	*/
	unsigned long					PWR_STR;									/* 1DFF_(F018 - F01Bh)	*/
	
	unsigned char					dmy_pmc_pmu_01C_01F[0x020 - 0x01C];			/* 1DFF_(F01C - F01Fh)	*/
	
	unsigned long					PWR_ON_EN0;									/* 1DFF_(F020 - F023h)	*/
	
	unsigned char					dmy_pmc_pmu_024_03F[0x040 - 0x024];			/* 1DFF_(F024 - F03Fh)	*/
	
	unsigned long					PWR_DN_EN;									/* 1DFF_(F040 - F043h)	*/
	
	unsigned char					dmy_pmc_pmu_044_04F[0x050 - 0x044];			/* 1DFF_(F044 - F04Fh)	*/
	
	union	io_pmc_pmu_pwr_on_cyc	PWR_ON_CYC[2];								/* 1DFF_(F050 - F057h)	*/
	
	unsigned char					dmy_pmc_pmu_058_06F[0x070 - 0x058];			/* 1DFF_(F058 - F06Fh)	*/
	
	unsigned long					PWR_CYC_SEL;								/* 1DFF_(F070 - F073h)	*/
	
	unsigned char					dmy_pmc_pmu_074_07F[0x080 - 0x074];			/* 1DFF_(F074 - F07Fh)	*/
	
	union	io_pmc_pmu_pwr_dly_ctl	PWR_DLY_CTL[2];								/* 1DFF_(F080 - F087h)	*/
	
	unsigned char					dmy_pmc_pmu_088_09F[0x0A0 - 0x088];			/* 1DFF_(F088 - F09Fh)	*/
	
	union	io_pmc_pmu_pwr_tm_ctl	PWR_TM_CTL0;								/* 1DFF_(F0A0 - F0A3h)	*/
	
	unsigned char					dmy_pmc_pmu_0A4_0AF[0x0B0 - 0x0A4];			/* 1DFF_(F0A4 - F0AFh)	*/
	
	union	io_pmc_pmu_pwr_tm_ctl	PWR_TM_S1_CTL0;								/* 1DFF_(F0B0 - F0B3h)	*/
	
	unsigned char					dmy_pmc_pmu_0B4_0BF[0x0C0 - 0x0B4];			/* 1DFF_(F0B4 - F0BFh)	*/
	
	union	io_pmc_pmu_pwr_tm_ctl	PWR_TM_S2_CTL0;								/* 1DFF_(F0C0 - F0C3h)	*/
	
	unsigned char					dmy_pmc_pmu_0C4_0F7[0x0F8 - 0x0C4];			/* 1DFF_(F0C4 - F0F7h)	*/
	
	unsigned long					MLTSTP_PD;									/* 1DFF_(F0F8 - F0FBh)	*/
	union	io_pmc_pmu_cfg			PMU_CFG;									/* 1DFF_(F0FC - F0FFh)	*/
	
	unsigned char					dmy_pmc_pmu_100_FFF[0x1000 - 0x100];		/* 1DFF_(F100 - FFFFh)	*/
};

#endif	// _IO_PMC_PMU_H_
