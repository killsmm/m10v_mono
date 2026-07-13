/**
 * @file		_exiu.h
 * @brief		Definition EXIU (External Interrupt Unit) I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/* structure of EIMASK			(1C26_F000h) */
union io_exiu_eimask {
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
		unsigned long	EIMASK17		:1;
		unsigned long	EIMASK18		:1;
		unsigned long	EIMASK19		:1;
		unsigned long	EIMASK20		:1;
		unsigned long	EIMASK21		:1;
		unsigned long	EIMASK22		:1;
		unsigned long	EIMASK23		:1;
		unsigned long	EIMASK24		:1;
		unsigned long	EIMASK25		:1;
		unsigned long	EIMASK26		:1;
		unsigned long	EIMASK27		:1;
		unsigned long	EIMASK28		:1;
		unsigned long	EIMASK29		:1;
		unsigned long	EIMASK30		:1;
		unsigned long	EIMASK31		:1;
	}bit;
};

/* structure of EISRCSEL		(1C26_F004h) */
union io_exiu_eisrcsel {
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
		unsigned long	EISRCSEL17		:1;
		unsigned long	EISRCSEL18		:1;
		unsigned long	EISRCSEL19		:1;
		unsigned long	EISRCSEL20		:1;
		unsigned long	EISRCSEL21		:1;
		unsigned long	EISRCSEL22		:1;
		unsigned long	EISRCSEL23		:1;
		unsigned long	EISRCSEL24		:1;
		unsigned long	EISRCSEL25		:1;
		unsigned long	EISRCSEL26		:1;
		unsigned long	EISRCSEL27		:1;
		unsigned long	EISRCSEL28		:1;
		unsigned long	EISRCSEL29		:1;
		unsigned long	EISRCSEL30		:1;
		unsigned long	EISRCSEL31		:1;
	}bit;
};

/* structure of EIREQSTA		(1C26_F008h) */
union io_exiu_eireqsta {
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
		unsigned long	EIREQSTA17		:1;
		unsigned long	EIREQSTA18		:1;
		unsigned long	EIREQSTA19		:1;
		unsigned long	EIREQSTA20		:1;
		unsigned long	EIREQSTA21		:1;
		unsigned long	EIREQSTA22		:1;
		unsigned long	EIREQSTA23		:1;
		unsigned long	EIREQSTA24		:1;
		unsigned long	EIREQSTA25		:1;
		unsigned long	EIREQSTA26		:1;
		unsigned long	EIREQSTA27		:1;
		unsigned long	EIREQSTA28		:1;
		unsigned long	EIREQSTA29		:1;
		unsigned long	EIREQSTA30		:1;
		unsigned long	EIREQSTA31		:1;
	}bit;
};

/* structure of EIRAWREQSTA		(1C26_F00Ch) */
union io_exiu_eirawreqsta {
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
		unsigned long	EIRAWREQSTA17	:1;
		unsigned long	EIRAWREQSTA18	:1;
		unsigned long	EIRAWREQSTA19	:1;
		unsigned long	EIRAWREQSTA20	:1;
		unsigned long	EIRAWREQSTA21	:1;
		unsigned long	EIRAWREQSTA22	:1;
		unsigned long	EIRAWREQSTA23	:1;
		unsigned long	EIRAWREQSTA24	:1;
		unsigned long	EIRAWREQSTA25	:1;
		unsigned long	EIRAWREQSTA26	:1;
		unsigned long	EIRAWREQSTA27	:1;
		unsigned long	EIRAWREQSTA28	:1;
		unsigned long	EIRAWREQSTA29	:1;
		unsigned long	EIRAWREQSTA30	:1;
		unsigned long	EIRAWREQSTA31	:1;
	}bit;
};

/* structure of EIREQCLR		(1C26_F010h) */
union io_exiu_eireqclr {
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
		unsigned long	EIREQCLR17		:1;
		unsigned long	EIREQCLR18		:1;
		unsigned long	EIREQCLR19		:1;
		unsigned long	EIREQCLR20		:1;
		unsigned long	EIREQCLR21		:1;
		unsigned long	EIREQCLR22		:1;
		unsigned long	EIREQCLR23		:1;
		unsigned long	EIREQCLR24		:1;
		unsigned long	EIREQCLR25		:1;
		unsigned long	EIREQCLR26		:1;
		unsigned long	EIREQCLR27		:1;
		unsigned long	EIREQCLR28		:1;
		unsigned long	EIREQCLR29		:1;
		unsigned long	EIREQCLR30		:1;
		unsigned long	EIREQCLR31		:1;
	}bit;
};

/* structure of EILVL			(1C26_F014h) */
union io_exiu_eilvl {
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
		unsigned long	EILVL17 		:1;
		unsigned long	EILVL18 		:1;
		unsigned long	EILVL19 		:1;
		unsigned long	EILVL20 		:1;
		unsigned long	EILVL21 		:1;
		unsigned long	EILVL22 		:1;
		unsigned long	EILVL23 		:1;
		unsigned long	EILVL24 		:1;
		unsigned long	EILVL25 		:1;
		unsigned long	EILVL26 		:1;
		unsigned long	EILVL27 		:1;
		unsigned long	EILVL28 		:1;
		unsigned long	EILVL29 		:1;
		unsigned long	EILVL30 		:1;
		unsigned long	EILVL31	 		:1;
	}bit;
};

/* structure of EIEDG			(1C26_F018h) */
union io_exiu_eiedg {
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
		unsigned long	EIEDG17 		:1;
		unsigned long	EIEDG18 		:1;
		unsigned long	EIEDG19 		:1;
		unsigned long	EIEDG20 		:1;
		unsigned long	EIEDG21 		:1;
		unsigned long	EIEDG22 		:1;
		unsigned long	EIEDG23 		:1;
		unsigned long	EIEDG24 		:1;
		unsigned long	EIEDG25 		:1;
		unsigned long	EIEDG26 		:1;
		unsigned long	EIEDG27 		:1;
		unsigned long	EIEDG28 		:1;
		unsigned long	EIEDG29 		:1;
		unsigned long	EIEDG30 		:1;
		unsigned long	EIEDG31	 		:1;
	}bit;
};

/* structure of EISIR			(1C26_F01Ch) */
union io_exiu_eisir {
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
		unsigned long	EISIR17 		:1;
		unsigned long	EISIR18 		:1;
		unsigned long	EISIR19 		:1;
		unsigned long	EISIR20 		:1;
		unsigned long	EISIR21 		:1;
		unsigned long	EISIR22 		:1;
		unsigned long	EISIR23 		:1;
		unsigned long	EISIR24 		:1;
		unsigned long	EISIR25 		:1;
		unsigned long	EISIR26 		:1;
		unsigned long	EISIR27 		:1;
		unsigned long	EISIR28 		:1;
		unsigned long	EISIR29 		:1;
		unsigned long	EISIR30 		:1;
		unsigned long	EISIR31	 		:1;
	}bit;
};

/*	EXIU register							*/
struct io_exiu {
	union io_exiu_eimask			EIMASK; 		/* 1C26_(F000 - F003h) Interrupt Masking Register */
	union io_exiu_eisrcsel			EISRCSEL;		/* 1C26_(F004 - F007h) Interrupt Source Select Register */
	union io_exiu_eireqsta			EIREQSTA;		/* 1C26_(F008 - F00Bh) Internal Interrupt Request Status Register */
	union io_exiu_eirawreqsta		EIRAWREQSTA;	/* 1C26_(F00C - F00Fh) Raw Interrupt Request Status Register */
	union io_exiu_eireqclr			EIREQCLR;		/* 1C26_(F010 - F013h) Interrupt Clear Register */
	union io_exiu_eilvl 			EILVL;			/* 1C26_(F014 - F017h) External Interrupt Level Setting Register */
	union io_exiu_eiedg 			EIEDG;			/* 1C26_(F018 - F01Bh) Interrupt Detection Method Setting Register */
	union io_exiu_eisir 			EISIR;			/* 1C26_(F01C - F01Fh) Software Interrupt Register */
};

extern volatile struct io_exiu		IO_EXIU;
