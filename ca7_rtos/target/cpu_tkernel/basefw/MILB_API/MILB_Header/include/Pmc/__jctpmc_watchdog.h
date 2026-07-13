/**
 * @file		__jctpmc_watchdog.h
 * @brief		Definition JCTPMC Watchdog Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


#ifndef _IO_PMC_WATCHDOG_H_
#define _IO_PMC_WATCHDOG_H_

/* Watchdog */

/* WdogControl */
union io_pmc_wdog_control {
	unsigned long		word;
	struct {
		unsigned long	INTEN			:1;
		unsigned long	RESEN			:1;
		unsigned long					:30;
	}bit;
};

/* WdogRIS */
union io_pmc_wdog_ris {
	unsigned long		word;
	struct {
		unsigned long	WDOGRIS			:1;
		unsigned long					:31;
	}bit;
};

/* WdogMIS */
union io_pmc_wdog_mis {
	unsigned long		word;
	struct {
		unsigned long	WDOGMIS			:1;
		unsigned long					:31;
	}bit;
};

/* WdogITCR */
union io_pmc_wdog_itcr {
	unsigned long		word;
	struct {
		unsigned long	ITEN			:1;
		unsigned long					:31;
	}bit;
};

/* WdogITOP */
union io_pmc_wdog_itop {
	unsigned long		word;
	struct {
		unsigned long	WDOGRES			:1;
		unsigned long	WDOGINT			:1;
		unsigned long					:30;
	}bit;
};

/* WdogPeriphID0 */
union io_pmc_wdog_periphid0 {
	unsigned long		word;
	struct {
		unsigned long	PartNumber0		:8;
		unsigned long					:24;
	}bit;
};

/* WdogPeriphID1 */
union io_pmc_wdog_periphid1 {
	unsigned long		word;
	struct {
		unsigned long	PartNumber1		:4;
		unsigned long	Designer0		:4;
		unsigned long					:24;
	}bit;
};

/* WdogPeriphID2 */
union io_pmc_wdog_periphid2 {
	unsigned long		word;
	struct {
		unsigned long	Designer1		:4;
		unsigned long	Revision		:4;
		unsigned long					:24;
	}bit;
};

/* WdogPeriphID3 */
union io_pmc_wdog_periphid3 {
	unsigned long		word;
	struct {
		unsigned long	Configuration	:8;
		unsigned long					:24;
	}bit;
};

/* WdogPCellID0-3 */
union io_pmc_wdog_pcellid {
	unsigned long		word;
	struct {
		unsigned long	WdogPCellID		:8;
		unsigned long					:24;
	}bit;
};


/* Watchdog */
struct io_pmc_watchdog {
	unsigned long					WdogLoad;									/* 1DFF_(D000 - D003h)	*/
	unsigned long					WdogValue;									/* 1DFF_(D004 - D007h)	*/
	union	io_pmc_wdog_control		WdogControl;								/* 1DFF_(D008 - D00Bh)	*/
	unsigned long					WdogIntClr;									/* 1DFF_(D00C - D00Fh)	*/
	union	io_pmc_wdog_ris			WdogRIS;									/* 1DFF_(D010 - D013h)	*/
	union	io_pmc_wdog_mis			WdogMIS;									/* 1DFF_(D014 - D017h)	*/
	
	unsigned char					dmy_pmc_wdog_018_BFF[0xC00 - 0x018];		/* 1DFF_(D018 - DBFFh)	*/
	
	unsigned long					WdogLock;									/* 1DFF_(DC00 - DC03h)	*/
	
	unsigned char					dmy_pmc_wdog_C04_EFF[0xF00 - 0xC04];		/* 1DFF_(DC04 - DEFFh)	*/
	
	union	io_pmc_wdog_itcr		WdogITCR;									/* 1DFF_(DF00 - DF03h)	*/
	union	io_pmc_wdog_itop		WdogITOP;									/* 1DFF_(DF04 - DF07h)	*/
	
	unsigned char					dmy_pmc_wdog_F08_FDF[0xFE0 - 0xF08];		/* 1DFF_(DF08 - DFDFh)	*/
	
	union	io_pmc_wdog_periphid0	WdogPeriphID0;								/* 1DFF_(DFE0 - DFE3h)	*/
	union	io_pmc_wdog_periphid1	WdogPeriphID1;								/* 1DFF_(DFE4 - DFE7h)	*/
	union	io_pmc_wdog_periphid2	WdogPeriphID2;								/* 1DFF_(DFE8 - DFEBh)	*/
	union	io_pmc_wdog_periphid3	WdogPeriphID3;								/* 1DFF_(DFEC - DFEFh)	*/
	union	io_pmc_wdog_pcellid		WdogPCellID[4];								/* 1DFF_(DFF0 - DFFFh)	*/
};

#endif	// _IO_PMC_WATCHDOG_H_
