/**
 * @file		__jctpmc_sensor_if.h
 * @brief		Definition JCTPMC Temperature Sensor I/F Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


#ifndef _IO_PMC_SENSOR_IF_H_
#define _IO_PMC_SENSOR_IF_H_

/* Temperature Sensor I/F */

/* MACROINFO */
union io_pmc_sensor_if_macroinfo {
	unsigned long		word;
	struct {
		unsigned long	TS_NUM				:4;
		unsigned long						:4;
		unsigned long	MACRO_REV			:8;
		unsigned long	MACRO_ID			:16;
	}bit;
};

/* ALLCONFIG */
union io_pmc_sensor_if_allconfig {
	unsigned long		word;
	struct {
		unsigned long	CONFIG_EN			:1;
		unsigned long						:31;
	}bit;
};

/* TS_RESET */
union io_pmc_sensor_if_ts_reset {
	unsigned long		word;
	struct {
		unsigned long	RESET				:1;
		unsigned long						:31;
	}bit;
};

/* TS_EN */
union io_pmc_sensor_if_ts_en {
	unsigned long		word;
	struct {
		unsigned long	EN					:1;
		unsigned long						:31;
	}bit;
};

/* TS_CT */
union io_pmc_sensor_if_ts_ct {
	unsigned long		word;
	struct {
		unsigned long	CT					:2;
		unsigned long						:30;
	}bit;
};

/* IRQOUT */
union io_pmc_sensor_if_irqout {
	unsigned long		word;
	struct {
		unsigned long	TS0_IRQOUT			:1;
		unsigned long	TS_IRQOUT			:15;
		unsigned long	ALL_LF_IRQOUT		:1;
		unsigned long	ALL_LN_IRQOUT		:1;
		unsigned long	ALL_HN_IRQOUT		:1;
		unsigned long	ALL_HF_IRQOUT		:1;
		unsigned long						:4;
		unsigned long	ANY_LF_IRQOUT		:1;
		unsigned long	ANY_LN_IRQOUT		:1;
		unsigned long	ANY_HN_IRQOUT		:1;
		unsigned long	ANY_HF_IRQOUT		:1;
		unsigned long						:4;
	}bit;
};

/* IRQCLR */
union io_pmc_sensor_if_irqclr {
	unsigned long		word;
	struct {
		unsigned long						:16;
		unsigned long	ALL_LF_IRQCLR		:1;
		unsigned long	ALL_LN_IRQCLR		:1;
		unsigned long	ALL_HN_IRQCLR		:1;
		unsigned long	ALL_HF_IRQCLR		:1;
		unsigned long						:4;
		unsigned long	ANY_LF_IRQCLR		:1;
		unsigned long	ANY_LN_IRQCLR		:1;
		unsigned long	ANY_HN_IRQCLR		:1;
		unsigned long	ANY_HF_IRQCLR		:1;
		unsigned long						:4;
	}bit;
};

/* IRQMASK */
union io_pmc_sensor_if_irqmask {
	unsigned long		word;
	struct {
		unsigned long	TS0_IRQMASK			:1;
		unsigned long	TS_IRQMASK			:15;
		unsigned long	ALL_LF_IRQMASK		:1;
		unsigned long	ALL_LN_IRQMASK		:1;
		unsigned long	ALL_HN_IRQMASK		:1;
		unsigned long	ALL_HF_IRQMASK		:1;
		unsigned long						:4;
		unsigned long	ANY_LF_IRQMASK		:1;
		unsigned long	ANY_LN_IRQMASK		:1;
		unsigned long	ANY_HN_IRQMASK		:1;
		unsigned long	ANY_HF_IRQMASK		:1;
		unsigned long						:4;
	}bit;
};

/* IRQSTATUS */
union io_pmc_sensor_if_irqstatus {
	unsigned long		word;
	struct {
		unsigned long	TS0_IRQSTATUS		:1;
		unsigned long	TS_IRQSTATUS		:15;
		unsigned long	ALL_LF_IRQSTATUS	:1;
		unsigned long	ALL_LN_IRQSTATUS	:1;
		unsigned long	ALL_HN_IRQSTATUS	:1;
		unsigned long	ALL_HF_IRQSTATUS	:1;
		unsigned long						:4;
		unsigned long	ANY_LF_IRQSTATUS	:1;
		unsigned long	ANY_LN_IRQSTATUS	:1;
		unsigned long	ANY_HN_IRQSTATUS	:1;
		unsigned long	ANY_HF_IRQSTATUS	:1;
		unsigned long						:4;
	}bit;
};

/* IRQOUT#x (x=0-4) */
union	io_pmc_sensor_if_sensor_irqout {
	unsigned long		word;
	struct {
		unsigned long	LF_IRQOUT			:1;
		unsigned long	LN_IRQOUT			:1;
		unsigned long	HN_IRQOUT			:1;
		unsigned long	HF_IRQOUT			:1;
		unsigned long						:28;
	}bit;
};


/* IRQCLR#x (x=0-4) */
union	io_pmc_sensor_if_sensor_irqclr {
	unsigned long		word;
	struct {
		unsigned long	LF_IRQCLR			:1;
		unsigned long	LN_IRQCLR			:1;
		unsigned long	HN_IRQCLR			:1;
		unsigned long	HF_IRQCLR			:1;
		unsigned long						:28;
	}bit;
};


/* IRQMASK#x (x=0-4) */
union	io_pmc_sensor_if_sensor_irqmask {
	unsigned long		word;
	struct {
		unsigned long	LF_IRQMASK			:1;
		unsigned long	LN_IRQMASK			:1;
		unsigned long	HN_IRQMASK			:1;
		unsigned long	HF_IRQMASK			:1;
		unsigned long						:28;
	}bit;
};


/* IRQSTATUS#x (x=0-4) */
union	io_pmc_sensor_if_sensor_irqstatus {
	unsigned long		word;
	struct {
		unsigned long	LF_IRQSTATUS		:1;
		unsigned long	LN_IRQSTATUS		:1;
		unsigned long	HN_IRQSTATUS		:1;
		unsigned long	HF_IRQSTATUS		:1;
		unsigned long						:28;
	}bit;
};


/* TSDATA_VALID#x (x=0-4) */
union	io_pmc_sensor_if_sensor_tsdata_valid {
	unsigned long		word;
	struct {
		unsigned long	DATA_VALID			:1;
		unsigned long						:31;
	}bit;
};

/* HYS_TEMP */
union io_pmc_sensor_if_hys_temp {
	unsigned long		word;
	struct {
		unsigned long	HYS_TEMP			:8;
		unsigned long						:24;
	}bit;
};


/* Register for temperature sensor #0 */
struct io_pmc_sensor_if_sensor {
	unsigned char									dmy_pmc_sensor_if_sensor_000_00F[0x010];
	
	union	io_pmc_sensor_if_ts_reset				TS_RESET;
	union	io_pmc_sensor_if_ts_en					TS_EN;
	union	io_pmc_sensor_if_ts_ct					TS_CT;
	
	unsigned char									dmy_pmc_sensor_if_sensor_01C_01F[0x020 - 0x01C];
	
	union	io_pmc_sensor_if_sensor_irqout			IRQOUT;
	union	io_pmc_sensor_if_sensor_irqclr			IRQCLR;
	union	io_pmc_sensor_if_sensor_irqmask			IRQMASK;
	union	io_pmc_sensor_if_sensor_irqstatus		IRQSTATUS;
	union	io_pmc_sensor_if_sensor_tsdata_valid	TSDATA_VALID;
	unsigned long									TSDATA;
	
	unsigned char									dmy_pmc_sensor_if_sensor_038_03F[0x040 - 0x038];
	
	unsigned long									HI_FULL_TEMP;
	unsigned long									HI_NEAR_TEMP;
	unsigned long									LO_NEAR_TEMP;
	unsigned long									LO_FULL_TEMP;
	union	io_pmc_sensor_if_hys_temp				HYS_TEMP;
	
	unsigned char									dmy_pmc_sensor_if_sensor_054_07F[0x080 - 0x054];
};


/* Temperature Sensor I/F */
struct io_pmc_sensor_if {
	union	io_pmc_sensor_if_macroinfo				MACROINFO;									/* 1DFF_(A000 - A003h)	*/
	
	unsigned char									dmy_pmc_sensor_if_004_00B[0x00C - 0x004];	/* 1DFF_(A004 - A00Bh)	*/
	
	union	io_pmc_sensor_if_allconfig				ALLCONFIG;									/* 1DFF_(A00C - A00Fh)	*/
	union	io_pmc_sensor_if_ts_reset				TS_RESET;									/* 1DFF_(A010 - A013h)	*/
	union	io_pmc_sensor_if_ts_en					TS_EN;										/* 1DFF_(A014 - A017h)	*/
	union	io_pmc_sensor_if_ts_ct					TS_CT;										/* 1DFF_(A018 - A01Bh)	*/
	
	unsigned char									dmy_pmc_sensor_if_01C_01F[0x020 - 0x01C];	/* 1DFF_(A01C - A01Fh)	*/
	
	union	io_pmc_sensor_if_irqout					IRQOUT;										/* 1DFF_(A020 - A023h)	*/
	union	io_pmc_sensor_if_irqclr					IRQCLR;										/* 1DFF_(A024 - A027h)	*/
	union	io_pmc_sensor_if_irqmask				IRQMASK;									/* 1DFF_(A028 - A02Bh)	*/
	union	io_pmc_sensor_if_irqstatus				IRQSTATUS;									/* 1DFF_(A02C - A02Fh)	*/
	
	unsigned char									dmy_pmc_sensor_if_030_03F[0x040 - 0x030];	/* 1DFF_(A030 - A03Fh)	*/
	
	unsigned long									HI_FULL_TEMP;								/* 1DFF_(A040 - A043h)	*/
	unsigned long									HI_NEAR_TEMP;								/* 1DFF_(A044 - A047h)	*/
	unsigned long									LO_NEAR_TEMP;								/* 1DFF_(A048 - A04Bh)	*/
	unsigned long									LO_FULL_TEMP;								/* 1DFF_(A04C - A04Fh)	*/
	union	io_pmc_sensor_if_hys_temp				HYS_TEMP;									/* 1DFF_(A050 - A053h)	*/
	
	unsigned char									dmy_pmc_sensor_if_054_7FF[0x800 - 0x054];	/* 1DFF_(A054 - A7FFh)	*/
	
	struct	io_pmc_sensor_if_sensor					SENSOR;										/* 1DFF_(A800 - A87Fh)	*/
	
	unsigned char									dmy_pmc_sensor_if_880_FFF[0x1000 - 0x880];	/* 1DFF_(A880 - AFFFh)	*/
};

#endif	// _IO_PMC_SENSOR_IF_H_
