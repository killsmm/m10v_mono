/**
 * @file		__jctpmc_calendar.h
 * @brief		Definition JCTPMC Calendar Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


#ifndef _IO_PMC_CALENDAR_H_
#define _IO_PMC_CALENDAR_H_

/* Calendar */

/* CAC */
union io_pmc_calendar_cac {
	unsigned long		word;
	struct {
		unsigned long	MODE			:2;
		unsigned long	ALEN			:1;
		unsigned long	OUT1HZEN		:1;
		unsigned long	ALRST			:1;
		unsigned long	SCPURST			:1;
		unsigned long					:1;
		unsigned long	IRST			:1;
		unsigned long					:24;
	}bit;
};

/* CA1 */
union io_pmc_calendar_ca1 {
	unsigned long		word;
	struct {
		unsigned long	SEC				:6;
		unsigned long					:26;
	}bit;
};

/* CA2 */
union io_pmc_calendar_ca2 {
	unsigned long		word;
	struct {
		unsigned long	MIN				:6;
		unsigned long					:26;
	}bit;
};

/* CA3 */
union io_pmc_calendar_ca3 {
	unsigned long		word;
	struct {
		unsigned long	HOUR			:5;
		unsigned long					:27;
	}bit;
};

/* CA4 */
union io_pmc_calendar_ca4 {
	unsigned long		word;
	struct {
		unsigned long	DAY				:5;
		unsigned long					:27;
	}bit;
};

/* CA5 */
union io_pmc_calendar_ca5 {
	unsigned long		word;
	struct {
		unsigned long	WEEK			:3;
		unsigned long					:29;
	}bit;
};

/* CA6 */
union io_pmc_calendar_ca6 {
	unsigned long		word;
	struct {
		unsigned long	MONTH			:4;
		unsigned long					:28;
	}bit;
};

/* CA7 */
union io_pmc_calendar_ca7 {
	unsigned long		word;
	struct {
		unsigned long	YEAR			:7;
		unsigned long					:25;
	}bit;
};

/* AL1 */
union io_pmc_calendar_al1 {
	unsigned long		word;
	struct {
		unsigned long	AMIN			:6;
		unsigned long					:26;
	}bit;
};

/* AL2 */
union io_pmc_calendar_al2 {
	unsigned long		word;
	struct {
		unsigned long	AHOUR			:5;
		unsigned long					:27;
	}bit;
};

/* AL3 */
union io_pmc_calendar_al3 {
	unsigned long		word;
	struct {
		unsigned long	ADAY			:5;
		unsigned long					:27;
	}bit;
};

/* AL4 */
union io_pmc_calendar_al4 {
	unsigned long		word;
	struct {
		unsigned long	AWEEK			:3;
		unsigned long					:29;
	}bit;
};

/* TST */
union io_pmc_calendar_tst {
	unsigned long		word;
	struct {
		unsigned long	TEST			:4;
		unsigned long					:28;
	}bit;
};


/* Calendar */
struct io_pmc_calendar {
	union	io_pmc_calendar_cac		CAC;										/* 1DFF_(C000 - C003h)	*/
	union	io_pmc_calendar_ca1		CA1;										/* 1DFF_(C004 - C007h)	*/
	union	io_pmc_calendar_ca2		CA2;										/* 1DFF_(C008 - C00Bh)	*/
	union	io_pmc_calendar_ca3		CA3;										/* 1DFF_(C00C - C00Fh)	*/
	union	io_pmc_calendar_ca4		CA4;										/* 1DFF_(C010 - C013h)	*/
	union	io_pmc_calendar_ca5		CA5;										/* 1DFF_(C014 - C017h)	*/
	union	io_pmc_calendar_ca6		CA6;										/* 1DFF_(C018 - C01Bh)	*/
	union	io_pmc_calendar_ca7		CA7;										/* 1DFF_(C01C - C01Fh)	*/
	union	io_pmc_calendar_al1		AL1;										/* 1DFF_(C020 - C023h)	*/
	union	io_pmc_calendar_al2		AL2;										/* 1DFF_(C024 - C027h)	*/
	union	io_pmc_calendar_al3		AL3;										/* 1DFF_(C028 - C02Bh)	*/
	union	io_pmc_calendar_al4		AL4;										/* 1DFF_(C02C - C02Fh)	*/
	union	io_pmc_calendar_tst		TST;										/* 1DFF_(C030 - C033h)	*/
	
	unsigned char					dmy_pmc_calendar_034_FFF[0x1000 - 0x034];	/* 1DFF_(C034 - CFFFh)	*/
};

#endif	// _IO_PMC_CALENDAR_H_
