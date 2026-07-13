/**
 * @file		__jctpmc_exiu.h
 * @brief		Definition JCTPMC EXIU I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */


#ifndef _IO_PMC_EXIU_H_
#define _IO_PMC_EXIU_H_

/* structure of EIMASK			(1DFF_3000h) */
union io_pmc_exiu_eimask {
	unsigned long		word;
	struct {
		unsigned long	EIMASK0 		:1;
		unsigned long	EIMASK1 		:1;
		unsigned long	EIMASK2 		:1;
		unsigned long	EIMASK3 		:1;
		unsigned long	EIMASK4 		:1;
		unsigned long	EIMASK5 		:1;
		unsigned long	EIMASK6 		:1;
		unsigned long	EIMASK7 		:1;
		unsigned long	EIMASK8 		:1;
		unsigned long	EIMASK9 		:1;
		unsigned long	EIMASK10		:1;
		unsigned long	EIMASK11		:1;
		unsigned long	EIMASK12		:1;
		unsigned long	EIMASK13		:1;
		unsigned long	EIMASK14		:1;
		unsigned long	EIMASK15		:1;
		unsigned long	EIMASK16		:1;
		unsigned long					:15;
	}bit;
};

/* structure of EISRCSEL		(1DFF_3004h) */
union io_pmc_exiu_eisrcsel {
	unsigned long		word;
	struct {
		unsigned long	EISRCSEL0		:1;
		unsigned long	EISRCSEL1		:1;
		unsigned long	EISRCSEL2		:1;
		unsigned long	EISRCSEL3		:1;
		unsigned long	EISRCSEL4		:1;
		unsigned long	EISRCSEL5		:1;
		unsigned long	EISRCSEL6		:1;
		unsigned long	EISRCSEL7		:1;
		unsigned long	EISRCSEL8		:1;
		unsigned long	EISRCSEL9		:1;
		unsigned long	EISRCSEL10		:1;
		unsigned long	EISRCSEL11		:1;
		unsigned long	EISRCSEL12		:1;
		unsigned long	EISRCSEL13		:1;
		unsigned long	EISRCSEL14		:1;
		unsigned long	EISRCSEL15		:1;
		unsigned long	EISRCSEL16		:1;
		unsigned long					:15;
	}bit;
};

/* structure of EIREQSTA		(1DFF_3008h) */
union io_pmc_exiu_eireqsta {
	unsigned long		word;
	struct {
		unsigned long	EIREQSTA0		:1;
		unsigned long	EIREQSTA1		:1;
		unsigned long	EIREQSTA2		:1;
		unsigned long	EIREQSTA3		:1;
		unsigned long	EIREQSTA4		:1;
		unsigned long	EIREQSTA5		:1;
		unsigned long	EIREQSTA6		:1;
		unsigned long	EIREQSTA7		:1;
		unsigned long	EIREQSTA8		:1;
		unsigned long	EIREQSTA9		:1;
		unsigned long	EIREQSTA10		:1;
		unsigned long	EIREQSTA11		:1;
		unsigned long	EIREQSTA12		:1;
		unsigned long	EIREQSTA13		:1;
		unsigned long	EIREQSTA14		:1;
		unsigned long	EIREQSTA15		:1;
		unsigned long	EIREQSTA16		:1;
		unsigned long					:15;
	}bit;
};

/* structure of EIRAWREQSTA		(1DFF_300Ch) */
union io_pmc_exiu_eirawreqsta {
	unsigned long		word;
	struct {
		unsigned long	EIRAWREQSTA0	:1;
		unsigned long	EIRAWREQSTA1	:1;
		unsigned long	EIRAWREQSTA2	:1;
		unsigned long	EIRAWREQSTA3	:1;
		unsigned long	EIRAWREQSTA4	:1;
		unsigned long	EIRAWREQSTA5	:1;
		unsigned long	EIRAWREQSTA6	:1;
		unsigned long	EIRAWREQSTA7	:1;
		unsigned long	EIRAWREQSTA8	:1;
		unsigned long	EIRAWREQSTA9	:1;
		unsigned long	EIRAWREQSTA10	:1;
		unsigned long	EIRAWREQSTA11	:1;
		unsigned long	EIRAWREQSTA12	:1;
		unsigned long	EIRAWREQSTA13	:1;
		unsigned long	EIRAWREQSTA14	:1;
		unsigned long	EIRAWREQSTA15	:1;
		unsigned long	EIRAWREQSTA16	:1;
		unsigned long					:15;
	}bit;
};

/* structure of EIREQCLR		(1DFF_3010h) */
union io_pmc_exiu_eireqclr {
	unsigned long		word;
	struct {
		unsigned long	EIREQCLR0		:1;
		unsigned long	EIREQCLR1		:1;
		unsigned long	EIREQCLR2		:1;
		unsigned long	EIREQCLR3		:1;
		unsigned long	EIREQCLR4		:1;
		unsigned long	EIREQCLR5		:1;
		unsigned long	EIREQCLR6		:1;
		unsigned long	EIREQCLR7		:1;
		unsigned long	EIREQCLR8		:1;
		unsigned long	EIREQCLR9		:1;
		unsigned long	EIREQCLR10		:1;
		unsigned long	EIREQCLR11		:1;
		unsigned long	EIREQCLR12		:1;
		unsigned long	EIREQCLR13		:1;
		unsigned long	EIREQCLR14		:1;
		unsigned long	EIREQCLR15		:1;
		unsigned long	EIREQCLR16		:1;
		unsigned long					:15;
	}bit;
};

/* structure of EILVL			(1DFF_3014h) */
union io_pmc_exiu_eilvl {
	unsigned long		word;
	struct {
		unsigned long	EILVL0			:1;
		unsigned long	EILVL1			:1;
		unsigned long	EILVL2			:1;
		unsigned long	EILVL3			:1;
		unsigned long	EILVL4			:1;
		unsigned long	EILVL5			:1;
		unsigned long	EILVL6			:1;
		unsigned long	EILVL7			:1;
		unsigned long	EILVL8			:1;
		unsigned long	EILVL9			:1;
		unsigned long	EILVL10 		:1;
		unsigned long	EILVL11 		:1;
		unsigned long	EILVL12 		:1;
		unsigned long	EILVL13 		:1;
		unsigned long	EILVL14 		:1;
		unsigned long	EILVL15 		:1;
		unsigned long	EILVL16 		:1;
		unsigned long			 		:15;
	}bit;
};

/* structure of EIEDG			(1DFF_3018h) */
union io_pmc_exiu_eiedg {
	unsigned long		word;
	struct {
		unsigned long	EIEDG0			:1;
		unsigned long	EIEDG1			:1;
		unsigned long	EIEDG2			:1;
		unsigned long	EIEDG3			:1;
		unsigned long	EIEDG4			:1;
		unsigned long	EIEDG5			:1;
		unsigned long	EIEDG6			:1;
		unsigned long	EIEDG7			:1;
		unsigned long	EIEDG8			:1;
		unsigned long	EIEDG9			:1;
		unsigned long	EIEDG10 		:1;
		unsigned long	EIEDG11 		:1;
		unsigned long	EIEDG12 		:1;
		unsigned long	EIEDG13 		:1;
		unsigned long	EIEDG14 		:1;
		unsigned long	EIEDG15 		:1;
		unsigned long	EIEDG16 		:1;
		unsigned long			 		:15;
	}bit;
};

/* structure of EISIR			(1DFF_301Ch) */
union io_pmc_exiu_eisir {
	unsigned long		word;
	struct {
		unsigned long	EISIR0			:1;
		unsigned long	EISIR1			:1;
		unsigned long	EISIR2			:1;
		unsigned long	EISIR3			:1;
		unsigned long	EISIR4			:1;
		unsigned long	EISIR5			:1;
		unsigned long	EISIR6			:1;
		unsigned long	EISIR7			:1;
		unsigned long	EISIR8			:1;
		unsigned long	EISIR9			:1;
		unsigned long	EISIR10 		:1;
		unsigned long	EISIR11 		:1;
		unsigned long	EISIR12 		:1;
		unsigned long	EISIR13 		:1;
		unsigned long	EISIR14 		:1;
		unsigned long	EISIR15 		:1;
		unsigned long	EISIR16 		:1;
		unsigned long			 		:15;
	}bit;
};

/*	EXIU register							*/
struct io_pmc_exiu {
	union io_pmc_exiu_eimask			EIMASK; 		/* 1DFF_(3000 - 3003h) Interrupt Masking Register */
	union io_pmc_exiu_eisrcsel			EISRCSEL;		/* 1DFF_(3004 - 3007h) Interrupt Source Select Register */
	union io_pmc_exiu_eireqsta			EIREQSTA;		/* 1DFF_(3008 - 300Bh) Internal Interrupt Request Status Register */
	union io_pmc_exiu_eirawreqsta		EIRAWREQSTA;	/* 1DFF_(300C - 300Fh) Raw Interrupt Request Status Register */
	union io_pmc_exiu_eireqclr			EIREQCLR;		/* 1DFF_(3010 - 3013h) Interrupt Clear Register */
	union io_pmc_exiu_eilvl 			EILVL;			/* 1DFF_(3014 - 3017h) External Interrupt Level Setting Register */
	union io_pmc_exiu_eiedg 			EIEDG;			/* 1DFF_(3018 - 301Bh) Interrupt Detection Method Setting Register */
	union io_pmc_exiu_eisir 			EISIR;			/* 1DFF_(301C - 301Fh) Software Interrupt Register */
	
	unsigned char						dmy_pmc_exiu_3020_3FFF[0x4000 - 0x3020];	/* 1DFF_3020 - 1DFF_3FFFh)	*/
};

#endif	// _IO_PMC_EXIU_H_

