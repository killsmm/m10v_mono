/**
 * @file		_wdog.h
 * @brief		Definition Watchdog Timer I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/* WdogControl */
union io_wdog_control {
	unsigned long		word;
	struct {
		unsigned long	INTEN			:1;
		unsigned long	RESEN			:1;
		unsigned long					:30;
	}bit;
};

/* WdogRIS */
union io_wdog_ris {
	unsigned long		word;
	struct {
		unsigned long	STATUS			:1;
		unsigned long					:31;
	}bit;
};

/* WdogMIS */
union io_wdog_mis {
	unsigned long		word;
	struct {
		unsigned long	STATUS			:1;
		unsigned long					:31;
	}bit;
};

/* WdogLOCK */
union io_wdog_lock {
	unsigned long		word;
	struct {
		unsigned long	EN_STATUS		:1;
		unsigned long	EN_REGISTER		:31;
	}bit;
};

/* WdogITCR */
union io_wdog_itcr {
	unsigned long		word;
	struct {
		unsigned long	ITEN			:1;
		unsigned long					:31;
	}bit;
};

/* WdogITOP */
union io_wdog_itop {
	unsigned long		word;
	struct {
		unsigned long	WDOGRES			:1;
		unsigned long	WDOGINT			:1;
		unsigned long					:30;
	}bit;
};

/* WdogPeriphID0 */
union io_wdog_periphid0 {
	unsigned long		word;
	struct {
		unsigned long	PARTNUMBER0		:8;
		unsigned long					:24;
	}bit;
};

/* WdogPeriphID1 */
union io_wdog_periphid1 {
	unsigned long		word;
	struct {
		unsigned long	PARTNUMBER1		:4;
		unsigned long	DESIGNER0		:4;
		unsigned long					:24;
	}bit;
};

/* WdogPeriphID2 */
union io_wdog_periphid2 {
	unsigned long		word;
	struct {
		unsigned long	DESIGNER1		:4;
		unsigned long	REVISION		:4;
		unsigned long					:24;
	}bit;
};

/* WdogPeriphID3 */
union io_wdog_periphid3 {
	unsigned long		word;
	struct {
		unsigned long	CONFIGURATION	:8;
		unsigned long					:24;
	}bit;
};

/* WdogPCellID0-3 */
union io_wdog_pcellid {
	unsigned long		word;
	struct {
		unsigned long	WDOGPCELLID		:8;
		unsigned long					:24;
	}bit;
};

/* Watchdog Timer register							*/
struct io_wdog {
	unsigned long				WDOGLOAD;									/* 1C26_(C000 - C003h)	*/
	unsigned long				WDOGVALUE;									/* 1C26_(C004 - C007h)	*/
	union io_wdog_control		WDOGCONTROL;								/* 1C26_(C008 - C00Bh)	*/
	unsigned long				WDOGINTCLR;									/* 1C26_(C00C - C00Fh)	*/
	union io_wdog_ris			WDOGRIS;									/* 1C26_(C010 - C013h)	*/
	union io_wdog_mis			WDOGMIS;									/* 1C26_(C014 - C017h)	*/
	unsigned char				dmy_018_BFF[0xC00 - 0x018];					/* 1C26_(C018 - CBFFh)	*/
	union io_wdog_lock			WDOGLOCK;									/* 1C26_(CC00 - CC03h)	*/
	unsigned char				dmy_C04_EFF[0xF00 - 0xC04];					/* 1C26_(CC04 - CEFFh)	*/
	union io_wdog_itcr			WDOGITCR;									/* 1C26_(CF00 - CF03h)	*/
	union io_wdog_itop			WDOGITOP;									/* 1C26_(CF04 - CF07h)	*/
	unsigned char				dmy_F08_FDF[0xFE0 - 0xF08];					/* 1C26_(CF08 - CFDFh)	*/
	union io_wdog_periphid0		WDOGPERIPHID0;								/* 1C26_(CFE0 - CFE3h)	*/
	union io_wdog_periphid1		WDOGPERIPHID1;								/* 1C26_(CFE4 - CFE7h)	*/
	union io_wdog_periphid2		WDOGPERIPHID2;								/* 1C26_(CFE8 - CFEBh)	*/
	union io_wdog_periphid3		WDOGPERIPHID3;								/* 1C26_(CFEC - CFEFh)	*/
	union io_wdog_pcellid		WDOGPCELLID[4];								/* 1C26_(CFF0 - CFFFh)	*/
};

extern volatile struct io_wdog		IO_WDOG;
