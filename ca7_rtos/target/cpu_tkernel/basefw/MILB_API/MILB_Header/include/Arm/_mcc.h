/**
 * @file		_mcc.h
 * @brief		Definition MCC (Multi-Core Controller) I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/* structure of Standby WFI monitor register	(1C26_A010h) */
union io_mcswfi {
	unsigned long		word;
	struct {
		unsigned long	CA70_WFI	:1;
		unsigned long	CA71_WFI	:1;
		unsigned long	CA72_WFI	:1;
		unsigned long	CA73_WFI	:1;
		unsigned long	CA70_WFE	:1;
		unsigned long	CA71_WFE	:1;
		unsigned long	CA72_WFE	:1;
		unsigned long	CA73_WFE	:1;
		unsigned long				:24;
	}bit;
};

/*	MCC register							*/
struct io_mcc {
	unsigned char					dmy_A000_A00F[0xA010 - 0xA000];	// 1C26_(A000 - A00Fh) Reserved
	union io_mcswfi					MCSWFI;							// 1C26_(A010 - A013h) Standby WFI and WFE monitor Register
	unsigned char					dmy_A014_AFFF[0xB000 - 0xA014];	// 1C26_(A014 - AFFFh) Reserved
};

extern volatile struct io_mcc		IO_MCC;
