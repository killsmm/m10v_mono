/**
 * @file		_peripheral.h
 * @brief		Definition Peripheral Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/* 32bit reload timer register */

/* structure of TMCSR0		(1E00_0000h)	*/
/* structure of TMCSR1		(1E00_0010h)	*/
/* structure of TMCSR2		(1E00_0020h)	*/
/* structure of TMCSR3		(1E00_0030h)	*/
/* structure of TMCSR4		(1E00_0040h)	*/
/* structure of TMCSR5		(1E00_0050h)	*/
/* structure of TMCSR6		(1E00_0060h)	*/
/* structure of TMCSR7		(1E00_0070h)	*/
/* structure of TMCSR8		(1E00_0080h)	*/
/* structure of TMCSR9		(1E00_0090h)	*/
/* structure of TMCSR10		(1E00_00A0h)	*/
/* structure of TMCSR11		(1E00_00B0h)	*/
/* structure of TMCSR12		(1E00_00C0h)	*/
/* structure of TMCSR13		(1E00_00D0h)	*/
union io_tmr_tmcsr {
	unsigned long		word;
	struct {
		unsigned long	__TRG			:1;
		unsigned long	__CNTE			:1;
		unsigned long	__UF			:1;
		unsigned long	__INTE			:1;
		unsigned long	__RELD			:1;
		unsigned long	__OUTL			:1;
		unsigned long					:1;
		unsigned long	__MOD			:3;
		unsigned long	__CSL			:2;
		unsigned long	__RELS			:1;
		unsigned long					:3;
		unsigned long					:16;
	} bit;
};


/* 64bit free run timer register */

/* structure of TMCSR64_1	(1E00_0100h)	*/
union io_tmr64_tmcsr1 {
	unsigned long		word;
	struct {
		unsigned long	TRG				:1;
		unsigned long	CNTE			:1;
		unsigned long					:8;
		unsigned long	CSL				:2;
		unsigned long					:4;
		unsigned long					:16;
	} bit;
};

/* structure of TMCSR64_2	(1E00_0110h)	*/
union io_tmr64_tmcsr2 {
	unsigned long		word;
	struct {
		unsigned long	__CVT			:1;
		unsigned long					:15;
		unsigned long	__TSEN			:1;
		unsigned long					:15;
	} bit;
};


/* PWM timer register */

/* structure of GCN1		(1E00_1000h)	*/
union io_pwm_gcn1 {
	unsigned long		word;
	struct {
		unsigned long	TSEL0		:4;
		unsigned long	TSEL1		:4;
		unsigned long	TSEL2		:4;
		unsigned long	TSEL3		:4;
		
		unsigned long				:16;
	}bit;
};
/* structure of GCN2		(1E00_1004h)	*/
union io_pwm_gcn2 {
	unsigned long		word;
	struct {
		unsigned long	EN0 		:1;
		unsigned long	EN1 		:1;
		unsigned long	EN2 		:1;
		unsigned long	EN3 		:1;
		unsigned long	CHAS		:1;
		unsigned long				:2;
		unsigned long	TMEEN		:1;
		
		unsigned long	MSK0		:1;
		unsigned long	MSK1		:1;
		unsigned long	MSK2		:1;
		unsigned long	MSK3		:1;
		unsigned long	STP0		:1;
		unsigned long	STP1		:1;
		unsigned long	STP2		:1;
		unsigned long	STP3		:1;
		
		unsigned long				:16;
	}bit;
};

/* structure of PTMRA		(1E00_1010h)	*/
/* structure of PTMRB		(1E00_1020h)	*/
/* structure of PTMRC		(1E00_1030h)	*/
/* structure of PTMRD		(1E00_1040h)	*/
/* structure of PTMRE		(1E00_1050h)	*/

/* structure of PCSRA		(1E00_1014h)	*/
/* structure of PCSRB		(1E00_1024h)	*/
/* structure of PCSRC		(1E00_1034h)	*/
/* structure of PCSRD		(1E00_1044h)	*/

/* structure of PDUTA		(1E00_1018h)	*/
/* structure of PDUTB		(1E00_1028h)	*/
/* structure of PDUTC		(1E00_1038h)	*/
/* structure of PDUTD		(1E00_1048h)	*/

/* structure of PCNA		(1E00_101Ch)	*/
/* structure of PCNB		(1E00_102Ch)	*/
/* structure of PCNC		(1E00_103Ch)	*/
/* structure of PCND		(1E00_104Ch)	*/
/* structure of PCNE		(1E00_1054h)	*/
union io_pwm_pcn {
	unsigned long		word;
	struct {
		unsigned long	__OSEL		:1;
		unsigned long				:1;
		unsigned long	__IRS		:2;
		unsigned long	__IRQF		:1;
		unsigned long	__IREN		:1;
		unsigned long	__EGS		:2;
		
		unsigned long				:1;
		unsigned long	__PGMS		:1;
		unsigned long	__CKS		:2;
		unsigned long	__RTRG		:1;
		unsigned long	__MDSE		:1;
		unsigned long	__STGR		:1;
		unsigned long	__CNTE		:1;
		
		unsigned long				:16;
	}bit;
};

/* structure of TSEL		(1E00_1058h)	*/
union io_pwm_tsel {
	unsigned long		word;
	struct {
		unsigned long	SEL0		:2;
		unsigned long	SEL1		:2;
		unsigned long	SEL2		:2;
		unsigned long	SEL3		:2;
		
		unsigned long				:24;
	}bit;
};

/* structure of PREV		(1E00_105Ch)	*/
union io_pwm_prev {
	unsigned long		word;
	struct {
		unsigned long				:8;
		
		unsigned long	PREVE0		:1;
		unsigned long	PREVE1		:1;
		unsigned long	PREVE2		:1;
		unsigned long	PREVE3		:1;
		unsigned long				:4;
		
		unsigned long				:16;
	}bit;
};

/* structure of PCSRQA		(1E00_1080h)	*/
/* structure of PCSRQB		(1E00_1088h)	*/
/* structure of PCSRQC		(1E00_1090h)	*/
/* structure of PCSRQD		(1E00_1098h)	*/

/* structure of PDUTQA		(1E00_1084h)	*/
/* structure of PDUTQB		(1E00_108Ch)	*/
/* structure of PDUTQC		(1E00_1094h)	*/
/* structure of PDUTQD		(1E00_109Ch)	*/

/* structure of PCSRRA		(1E00_10A0h)	*/
/* structure of PCSRRB		(1E00_10A8h)	*/
/* structure of PCSRRC		(1E00_10B0h)	*/
/* structure of PCSRRD		(1E00_10B8h)	*/

/* structure of PDUTRA		(1E00_10A4h)	*/
/* structure of PDUTRB		(1E00_10ACh)	*/
/* structure of PDUTRC		(1E00_10B4h)	*/
/* structure of PDUTRD		(1E00_10BCh)	*/

/* structure of PCHSET0 	(1E00_10C0h)	*/
union io_pwm_pchset0 {
	unsigned long		word;
	struct {
		unsigned long	FRCEN0		:1;
		unsigned long	FRCEN1		:1;
		unsigned long	FRCEN2		:1;
		unsigned long	FRCEN3		:1;
		unsigned long	OSTA0		:1;
		unsigned long	OSTA1		:1;
		unsigned long	OSTA2		:1;
		unsigned long	OSTA3		:1;
		
		unsigned long	FRCVAL0 	:1;
		unsigned long	FRCVAL1 	:1;
		unsigned long	FRCVAL2 	:1;
		unsigned long	FRCVAL3 	:1;
		unsigned long	HLD0		:1;
		unsigned long	HLD1		:1;
		unsigned long	HLD2		:1;
		unsigned long	HLD3		:1;
		
		unsigned long				:16;
	}bit;
};

/* structure of PCHSET1 	(1E00_10C4h)	*/
union io_pwm_pchset1 {
	unsigned long		word;
	struct {
		unsigned long	UDEN		:1;
		unsigned long				:7;
		
		unsigned long	RQTIM 		:2;
		unsigned long				:6;
		
		unsigned long				:16;
	}bit;
};

/* structure of PINTCT0 	(1E00_10C8h)	*/
union io_pwm_pintct0 {
	unsigned long		word;
	struct {
		unsigned long	__AIE		:1;
		unsigned long	__FIE		:1;
		unsigned long				:2;
		unsigned long	__UIE		:1;
		unsigned long				:3;
		
		unsigned long	__AIS		:1;
		unsigned long	__FIS		:1;
		unsigned long				:2;
		unsigned long	__UIS		:1;
		unsigned long				:3;
		
		unsigned long				:16;
	}bit;
};

/* structure of PINTCT1 	(1E00_10CCh)	*/
union io_pwm_pintct1 {
	unsigned long		word;
	struct {
		unsigned long				:7;
		unsigned long	DRQE		:1;
		
		unsigned long				:7;
		unsigned long	DRQS		:1;
		
		unsigned long				:16;
	}bit;
};

/* structure of STMRH		(1E00_10D0h)	*/
/* structure of STMRL		(1E00_10D4h)	*/

/* structure of PCSRE0		(1E00_1100h)	*/
/* structure of PCSRE1		(1E00_1110h)	*/
/* structure of PCSRE2		(1E00_1120h)	*/
/* structure of PCSRE3		(1E00_1130h)	*/
/* structure of PCSRE4		(1E00_1140h)	*/
/* structure of PCSRE5		(1E00_1150h)	*/
/* structure of PCSRE6		(1E00_1160h)	*/
/* structure of PCSRE7		(1E00_1170h)	*/

/* structure of PDUTE0		(1E00_1104h)	*/
/* structure of PDUTE1		(1E00_1114h)	*/
/* structure of PDUTE2		(1E00_1124h)	*/
/* structure of PDUTE3		(1E00_1134h)	*/
/* structure of PDUTE4		(1E00_1144h)	*/
/* structure of PDUTE5		(1E00_1154h)	*/
/* structure of PDUTE6		(1E00_1164h)	*/
/* structure of PDUTE7		(1E00_1174h)	*/

/* structure of PPALE0		(1E00_1108h)	*/
/* structure of PPALE1		(1E00_1118h)	*/
/* structure of PPALE2		(1E00_1128h)	*/
/* structure of PPALE3		(1E00_1138h)	*/
/* structure of PPALE4		(1E00_1148h)	*/
/* structure of PPALE5		(1E00_1158h)	*/
/* structure of PPALE6		(1E00_1168h)	*/
/* structure of PPALE7		(1E00_1178h)	*/
union io_pwm_ppale {
	unsigned long		word;
	struct {
		unsigned long	PPALE			:10;
		unsigned long					:5;
		unsigned long	LINK			:1;
		
		unsigned long					:16;
	} bit;
};

/* structure of PCSRU0		(1E00_1180h)	*/
/* structure of PCSRU1		(1E00_1190h)	*/
/* structure of PCSRU2		(1E00_11A0h)	*/
/* structure of PCSRU3		(1E00_11B0h)	*/

/* structure of PDUTU0		(1E00_1184h)	*/
/* structure of PDUTU1		(1E00_1194h)	*/
/* structure of PDUTU2		(1E00_11A4h)	*/
/* structure of PDUTU3		(1E00_11B4h)	*/

/* structure of PPALU0		(1E00_1188h)	*/
/* structure of PPALU1		(1E00_1198h)	*/
/* structure of PPALU2		(1E00_11A8h)	*/
/* structure of PPALU3		(1E00_11B8h)	*/
union io_pwm_ppalu {
	unsigned long		word;
	struct {
		unsigned long	PPALU			:10;
		unsigned long					:5;
		unsigned long	LINK			:1;
		
		unsigned long					:16;
	} bit;
};

/* I2C */
/* structure of SDAT0		(1E00_3000h)	*/
/* structure of SDAT1		(1E00_3400h)	*/
/* structure of SDAT2		(1E00_3800h)	*/
union io_i2c_sdat {
	unsigned long		word;
	struct {
		unsigned long	DATA		:8;
		
		unsigned long				:24;
	}bit;
};

/* structure of ST0			(1E00_3004h)	*/
/* structure of ST1			(1E00_3404h)	*/
/* structure of ST2			(1E00_3804h)	*/
union io_i2c_st {
	unsigned long		word;
	struct {
		unsigned long	MODE 		:5;
		unsigned long				:2;
		unsigned long	INT 		:1;
		
		unsigned long				:24;
	}bit;
};

/* structure of CST0		(1E00_3008h)	*/
/* structure of CST1		(1E00_3408h)	*/
/* structure of CST2		(1E00_3808h)	*/
union io_i2c_cst {
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

/* structure of CTL1_0		(1E00_300Ch)	*/
/* structure of CTL1_1		(1E00_340Ch)	*/
/* structure of CTL1_2		(1E00_380Ch)	*/
union io_i2c_ctl1 {
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

/* structure of ADDR0		(1E00_3010h)	*/
/* structure of ADDR1		(1E00_3410h)	*/
/* structure of ADDR2		(1E00_3810h)	*/
union io_i2c_addr {
	unsigned long		word;
	struct {
		unsigned long	ADDR		:7;
		unsigned long	SAEN		:1;
		
		unsigned long				:24;
	}bit;
};

/* structure of CTL2_0		(1E00_3014h)	*/
/* structure of CTL2_1		(1E00_3414h)	*/
/* structure of CTL2_2		(1E00_3814h)	*/
union io_i2c_ctl2 {
	unsigned long		word;
	struct {
		unsigned long	ENABLE		:1;
		unsigned long	SCLFRQ		:7;
		
		unsigned long				:24;
	}bit;
};

/* structure of TOPR0		(1E00_3018h)	*/
/* structure of TOPR1		(1E00_3418h)	*/
/* structure of TOPR2		(1E00_3818h)	*/
union io_i2c_topr {
	unsigned long		word;
	struct {
		unsigned long	TOPR 		:8;
		
		unsigned long				:24;
	}bit;
};

/* structure of CTL3_0		(1E00_301Ch)	*/
/* structure of CTL3_1		(1E00_341Ch)	*/
/* structure of CTL3_2		(1E00_381Ch)	*/
union io_i2c_ctl3 {
	unsigned long		word;
	struct {
		unsigned long	S10ADR		:3;
		unsigned long	S10EN		:1;
		unsigned long	HSCDIV 		:4;
		
		unsigned long				:24;
	}bit;
};


/* UDC */
/* structure of UDCR0		(1E00_2000h)	*/
/* structure of UDCR1		(1E00_2020h)	*/
/* structure of UDCR2		(1E00_2040h)	*/
/* structure of UDCR3		(1E00_2060h)	*/
/* structure of UDCR4		(1E00_2080h)	*/
/* structure of UDCR5		(1E00_20A0h)	*/
union io_udc_udcr {
	unsigned long		word;
	struct {
		unsigned long	D			:16;
		
		unsigned long				:16;
	}bit;
};

/* structure of RCR0		(1E00_2004h)	*/
/* structure of RCR1		(1E00_2024h)	*/
/* structure of RCR2		(1E00_2044h)	*/
/* structure of RCR3		(1E00_2064h)	*/
/* structure of RCR4		(1E00_2084h)	*/
/* structure of RCR5		(1E00_20A4h)	*/
union io_udc_rcr {
	unsigned long		word;
	struct {
		unsigned long	D			:16;
		
		unsigned long				:16;
	}bit;
};

/* structure of CSR0		(1E00_200Ch)	*/
/* structure of CSR1		(1E00_202Ch)	*/
/* structure of CSR2		(1E00_204Ch)	*/
/* structure of CSR3		(1E00_206Ch)	*/
/* structure of CSR4		(1E00_208Ch)	*/
/* structure of CSR5		(1E00_20ACh)	*/
union io_udc_csr {
	unsigned long		word;
	struct {
		unsigned long	__UDF		:2;
		unsigned long	__UDFF		:1;
		unsigned long	__OVFF		:1;
		unsigned long	__CMPF		:1;
		unsigned long	__UDIE		:1;
		unsigned long	__CITE		:1;
		unsigned long	__CSTR		:1;
		
		unsigned long				:24;
	} bit;
};

/* structure of CCR0		(1E00_2014h)	*/
/* structure of CCR1		(1E00_2034h)	*/
/* structure of CCR2		(1E00_2054h)	*/
/* structure of CCR3		(1E00_2074h)	*/
/* structure of CCR4		(1E00_2094h)	*/
/* structure of CCR5		(1E00_20B4h)	*/
union io_udc_ccr {
	unsigned long		word;
	struct {
		unsigned long	__CGE		:2;
		unsigned long	__CGSC		:1;
		unsigned long	__UDCC		:1;
		unsigned long	__RLDE		:1;
		unsigned long	__UCRE		:1;
		unsigned long	__CTUT		:1;
		unsigned long				:1;
		
		unsigned long	__CES		:2;
		unsigned long	__CMS		:2;
		unsigned long	__CLKS		:1;
		unsigned long	__CFIE		:1;
		unsigned long	__CDCF		:1;
		unsigned long	__M16E		:1; // Reserve bit. However, it is necessary to be "1".
		
		unsigned long				:16;
	} bit;
};


struct io_tmr {
	union io_tmr_tmcsr	TMCSR;									/* 1E00_(0000 - 0003h) */
	unsigned long		TMR;									/* 1E00_(0004 - 0007h) */
	unsigned long		TMRLR0; 								/* 1E00_(0008 - 000Bh) */
	unsigned long		TMRLR1; 								/* 1E00_(000C - 000Fh) */
};

struct io_tmr64 {
	union io_tmr64_tmcsr1	TMCSR64_1;							/* 1E00_(0100 - 0103h) */
	unsigned char			dmy_peri_0104_010F[0x0110-0x0104];	/* 1E00_(0104 - 010Fh) */
	union io_tmr64_tmcsr2	TMCSR64_2;							/* 1E00_(0110 - 0113h) */
	unsigned char			dmy_peri_0114_011F[0x0120-0x0114];	/* 1E00_(0114 - 011Fh) */
	unsigned long			GSTMP;								/* 1E00_(0120 - 0123h) */
	unsigned char			dmy_peri_0124_01FF[0x0200-0x0124];	/* 1E00_(0124 - 01FFh) */
	unsigned long			FSSTMP00L;							/* 1E00_(0200 - 0203h) */
	unsigned long			FSSTMP00H;							/* 1E00_(0204 - 0207h) */
	unsigned long			FSSTMP01L;							/* 1E00_(0208 - 020Bh) */
	unsigned long			FSSTMP01H;							/* 1E00_(020C - 020Fh) */
	unsigned long			FSSTMP02L;							/* 1E00_(0210 - 0213h) */
	unsigned long			FSSTMP02H;							/* 1E00_(0214 - 0217h) */
	unsigned char			dmy_peri_0218_021F[0x0220-0x0218];	/* 1E00_(0218 - 021Fh) */
	unsigned long			FSSTMP10L;							/* 1E00_(0220 - 0223h) */
	unsigned long			FSSTMP10H;							/* 1E00_(0224 - 0227h) */
	unsigned long			FSSTMP11L;							/* 1E00_(0228 - 022Bh) */
	unsigned long			FSSTMP11H;							/* 1E00_(022C - 022Fh) */
	unsigned long			FSSTMP12L;							/* 1E00_(0230 - 0233h) */
	unsigned long			FSSTMP12H;							/* 1E00_(0234 - 0237h) */
	unsigned char			dmy_peri_0238_023F[0x0240-0x0238];	/* 1E00_(0238 - 023Fh) */
	unsigned long			FSSTMP20L;							/* 1E00_(0240 - 0243h) */
	unsigned long			FSSTMP20H;							/* 1E00_(0240 - 0247h) */
	unsigned long			FSSTMP21L;							/* 1E00_(0248 - 024Bh) */
	unsigned long			FSSTMP21H;							/* 1E00_(024C - 024Fh) */
	unsigned long			FSSTMP22L;							/* 1E00_(0250 - 0253h) */
	unsigned long			FSSTMP22H;							/* 1E00_(0253 - 0257h) */
	unsigned char			dmy_peri_0258_025F[0x0260-0x0258];	/* 1E00_(0258 - 025Fh) */
	unsigned long			FSSTMP30L;							/* 1E00_(0260 - 0263h) */
	unsigned long			FSSTMP30H;							/* 1E00_(0264 - 0267h) */
	unsigned long			FSSTMP31L;							/* 1E00_(0268 - 026Bh) */
	unsigned long			FSSTMP31H;							/* 1E00_(026C - 026Fh) */
	unsigned long			FSSTMP32L;							/* 1E00_(0270 - 0273h) */
	unsigned long			FSSTMP32H;							/* 1E00_(0274 - 0277h) */
	unsigned char			dmy_peri_0278_02FF[0x0300-0x0278];	/* 1E00_(0278 - 027Fh) */
	unsigned long			FESTMP00L;							/* 1E00_(0300 - 0303h) */
	unsigned long			FESTMP00H;							/* 1E00_(0304 - 0307h) */
	unsigned long			FESTMP01L;							/* 1E00_(0308 - 030Bh) */
	unsigned long			FESTMP01H;							/* 1E00_(030C - 030Fh) */
	unsigned long			FESTMP02L;							/* 1E00_(0310 - 0313h) */
	unsigned long			FESTMP02H;							/* 1E00_(0314 - 0317h) */
	unsigned char			dmy_peri_0318_031F[0x0320-0x0318];	/* 1E00_(0318 - 031Fh) */
	unsigned long			FESTMP10L;							/* 1E00_(0320 - 0323h) */
	unsigned long			FESTMP10H;							/* 1E00_(0324 - 0327h) */
	unsigned long			FESTMP11L;							/* 1E00_(0328 - 032Bh) */
	unsigned long			FESTMP11H;							/* 1E00_(032C - 032Fh) */
	unsigned long			FESTMP12L;							/* 1E00_(0330 - 0333h) */
	unsigned long			FESTMP12H;							/* 1E00_(0334 - 0337h) */
	unsigned char			dmy_peri_0338_033F[0x0340-0x0338];	/* 1E00_(0338 - 033Fh) */
	unsigned long			FESTMP20L;							/* 1E00_(0340 - 0343h) */
	unsigned long			FESTMP20H;							/* 1E00_(0340 - 0347h) */
	unsigned long			FESTMP21L;							/* 1E00_(0348 - 034Bh) */
	unsigned long			FESTMP21H;							/* 1E00_(034C - 034Fh) */
	unsigned long			FESTMP22L;							/* 1E00_(0350 - 0353h) */
	unsigned long			FESTMP22H;							/* 1E00_(0353 - 0357h) */
	unsigned char			dmy_peri_0358_035F[0x0360-0x0358];	/* 1E00_(0358 - 035Fh) */
	unsigned long			FESTMP30L;							/* 1E00_(0360 - 0363h) */
	unsigned long			FESTMP30H;							/* 1E00_(0364 - 0367h) */
	unsigned long			FESTMP31L;							/* 1E00_(0368 - 036Bh) */
	unsigned long			FESTMP31H;							/* 1E00_(036C - 036Fh) */
	unsigned long			FESTMP32L;							/* 1E00_(0370 - 0373h) */
	unsigned long			FESTMP32H;							/* 1E00_(0374 - 0377h) */
	unsigned char			dmy_peri_0378_03FF[0x0400-0x0378];	/* 1E00_(0378 - 037Fh) */
	unsigned long			TMR64_L;							/* 1E00_(0400 - 0403h) */
	unsigned long			TMR64_H;							/* 1E00_(0404 - 0407h) */
};

struct io_pwm {
	union io_pwm_gcn1		GCN1;								/* 1E00_(1000 - 1003h) */
	union io_pwm_gcn2		GCN2;								/* 1E00_(1004 - 1007h) */
	
	unsigned char			dmy_peri_1008_100F[0x1010-0x1008];	/* 1E00_(1008 - 100Fh) */
	
	unsigned long			PTMRA;								/* 1E00_(1010 - 1013h) */
	unsigned long			PCSRA;								/* 1E00_(1014 - 1017h) */
	unsigned long			PDUTA;								/* 1E00_(1018 - 101Bh) */
	union io_pwm_pcn		PCNA;								/* 1E00_(101C - 101Fh) */
	
	unsigned long			PTMRB;								/* 1E00_(1020 - 1023h) */
	unsigned long			PCSRB;								/* 1E00_(1024 - 1027h) */
	unsigned long			PDUTB;								/* 1E00_(1028 - 102Bh) */
	union io_pwm_pcn		PCNB;								/* 1E00_(102C - 102Fh) */
	
	unsigned long			PTMRC;								/* 1E00_(1030 - 1033h) */
	unsigned long			PCSRC;								/* 1E00_(1034 - 1037h) */
	unsigned long			PDUTC;								/* 1E00_(1038 - 103Bh) */
	union io_pwm_pcn		PCNC;								/* 1E00_(103C - 103Fh) */
	
	unsigned long			PTMRD;								/* 1E00_(1040 - 1043h) */
	unsigned long			PCSRD;								/* 1E00_(1044 - 1047h) */
	unsigned long			PDUTD;								/* 1E00_(1048 - 104Bh) */
	union io_pwm_pcn		PCND;								/* 1E00_(104C - 104Fh) */
	
	unsigned long			PTMRE;								/* 1E00_(1050 - 1053h) */
	union io_pwm_pcn		PCNE;								/* 1E00_(1054 - 1057h) */
	
	union io_pwm_tsel		TSEL;								/* 1E00_(1058 - 105Bh) */
	union io_pwm_prev		PREV;								/* 1E00_(105C - 105Fh) */
	
	unsigned char			dmy_peri_1060_107F[0x1080-0x1060];	/* 1E00_(1060 - 107Fh) */
	
	unsigned long			PCSRQA; 							/* 1E00_(1080 - 1083h) */
	unsigned long			PDUTQA; 							/* 1E00_(1084 - 1087h) */
	unsigned long			PCSRQB; 							/* 1E00_(1088 - 108Bh) */
	unsigned long			PDUTQB; 							/* 1E00_(108C - 108Fh) */
	unsigned long			PCSRQC; 							/* 1E00_(1090 - 1093h) */
	unsigned long			PDUTQC; 							/* 1E00_(1094 - 1097h) */
	unsigned long			PCSRQD; 							/* 1E00_(1098 - 109Bh) */
	unsigned long			PDUTQD; 							/* 1E00_(109C - 109Fh) */
	
	unsigned long			PCSRRA; 							/* 1E00_(10A0 - 10A3h) */
	unsigned long			PDUTRA; 							/* 1E00_(10A4 - 10A7h) */
	unsigned long			PCSRRB; 							/* 1E00_(10A8 - 10ABh) */
	unsigned long			PDUTRB; 							/* 1E00_(10AC - 10AFh) */
	unsigned long			PCSRRC; 							/* 1E00_(10B0 - 10B3h) */
	unsigned long			PDUTRC; 							/* 1E00_(10B4 - 10B7h) */
	unsigned long			PCSRRD; 							/* 1E00_(10B8 - 10BBh) */
	unsigned long			PDUTRD; 							/* 1E00_(10BC - 10BFh) */
	
	union io_pwm_pchset0	PCHSET0;							/* 1E00_(10C0 - 10C3h) */
	union io_pwm_pchset1	PCHSET1;							/* 1E00_(10C4 - 10C7h) */
	union io_pwm_pintct0	PINTCT0;							/* 1E00_(10C8 - 10CBh) */
	union io_pwm_pintct1	PINTCT1;							/* 1E00_(10CC - 10CFh) */
	
	unsigned long			STMRH;								/* 1E00_(10D0 - 10D3h) */
	unsigned long			STMRL;								/* 1E00_(10D4 - 10D7h) */
	
	unsigned char			dmy_peri_10D8_10FF[0x1100-0x10D8];	/* 1E00_(10D8 - 10FFh) */
	
	unsigned long			PCSRE0; 							/* 1E00_(1100 - 1103h) */
	unsigned long			PDUTE0; 							/* 1E00_(1104 - 1107h) */
	union io_pwm_ppale		PPALE0; 							/* 1E00_(1108 - 110Bh) */
	
	unsigned char			dmy_peri_110C_110F[0x1110-0x110C];	/* 1E00_(110C - 110Fh) */

	unsigned long			PCSRE1; 							/* 1E00_(1110 - 1113h) */
	unsigned long			PDUTE1; 							/* 1E00_(1114 - 1117h) */
	union io_pwm_ppale		PPALE1; 							/* 1E00_(1118 - 111Bh) */
	
	unsigned char			dmy_peri_111C_111F[0x1120-0x111C];	/* 1E00_(111C - 111Fh) */
	
	unsigned long			PCSRE2; 							/* 1E00_(1120 - 1123h) */
	unsigned long			PDUTE2; 							/* 1E00_(1124 - 1127h) */
	union io_pwm_ppale		PPALE2; 							/* 1E00_(1128 - 112Bh) */
	
	unsigned char			dmy_peri_112C_112F[0x1130-0x112C];	/* 1E00_(112C - 112Fh) */
	
	unsigned long			PCSRE3; 							/* 1E00_(1130 - 1133h) */
	unsigned long			PDUTE3; 							/* 1E00_(1134 - 1137h) */
	union io_pwm_ppale		PPALE3; 							/* 1E00_(1138 - 113Bh) */
	
	unsigned char			dmy_peri_113C_113F[0x1140-0x113C];	/* 1E00_(113C - 113Fh) */
	
	unsigned long			PCSRE4; 							/* 1E00_(1140 - 1143h) */
	unsigned long			PDUTE4; 							/* 1E00_(1144 - 1147h) */
	union io_pwm_ppale		PPALE4; 							/* 1E00_(1148 - 114Bh) */
	
	unsigned char			dmy_peri_114C_114F[0x1150-0x114C];	/* 1E00_(114C - 114Fh) */
	
	unsigned long			PCSRE5; 							/* 1E00_(1150 - 1153h) */
	unsigned long			PDUTE5; 							/* 1E00_(1154 - 1157h) */
	union io_pwm_ppale		PPALE5; 							/* 1E00_(1158 - 115Bh) */
	
	unsigned char			dmy_peri_115C_115F[0x1160-0x115C];	/* 1E00_(115C - 115Fh) */
	
	unsigned long			PCSRE6; 							/* 1E00_(1160 - 1163h) */
	unsigned long			PDUTE6; 							/* 1E00_(1164 - 1167h) */
	union io_pwm_ppale		PPALE6; 							/* 1E00_(1168 - 116Bh) */
	
	unsigned char			dmy_peri_116C_116F[0x1170-0x116C];	/* 1E00_(116C - 116Fh) */
	
	unsigned long			PCSRE7; 							/* 1E00_(1170 - 1173h) */
	unsigned long			PDUTE7; 							/* 1E00_(1174 - 1177h) */
	union io_pwm_ppale		PPALE7; 							/* 1E00_(1178 - 117Bh) */
	
	unsigned char			dmy_peri_117C_117F[0x1180-0x117C];	/* 1E00_(117C - 117Fh) */
	
	unsigned long			PCSRU0; 							/* 1E00_(1180 - 1183h) */
	unsigned long			PDUTU0; 							/* 1E00_(1184 - 1187h) */
	union io_pwm_ppalu		PPALU0; 							/* 1E00_(1188 - 118Bh) */
	
	unsigned char			dmy_peri_118C_118F[0x1190-0x118C];	/* 1E00_(118C - 118Fh) */
	
	unsigned long			PCSRU1; 							/* 1E00_(1190 - 1193h) */
	unsigned long			PDUTU1; 							/* 1E00_(1194 - 1197h) */
	union io_pwm_ppalu		PPALU1; 							/* 1E00_(1198 - 119Bh) */
	
	unsigned char			dmy_peri_119C_119F[0x11A0-0x119C];	/* 1E00_(119C - 119Fh) */
	
	unsigned long			PCSRU2; 							/* 1E00_(11A0 - 11A3h) */
	unsigned long			PDUTU2; 							/* 1E00_(11A4 - 11A7h) */
	union io_pwm_ppalu		PPALU2; 							/* 1E00_(11A8 - 11ABh) */
	
	unsigned char			dmy_peri_11AC_11AF[0x11B0-0x11AC];	/* 1E00_(11AC - 11AFh) */
	
	unsigned long			PCSRU3; 							/* 1E00_(11B0 - 11B3h) */
	unsigned long			PDUTU3; 							/* 1E00_(11B4 - 11B7h) */
	union io_pwm_ppalu		PPALU3; 							/* 1E00_(11B8 - 11BBh) */
	
	unsigned char			dmy_peri_11BC_13FF[0x1400-0x11BC];	/* 1E00_(11BC - 13FFh) */
};

struct io_udc {
	union io_udc_udcr	UDCR;								/* 1E00_(2000 - 2003h) */
	union io_udc_rcr	RCR;								/* 1E00_(2004 - 2007h) */
	unsigned char		dmy_peri_2008_200B[0x200C-0x2008];	/* 1E00_(2008 - 200Bh) */

	union io_udc_csr	CSR;								/* 1E00_(200C - 200Fh) */
	unsigned char		dmy_peri_2010_2013[0x2014-0x2010];	/* 1E00_(2010 - 2013h) */

	union io_udc_ccr	CCR;								/* 1E00_(2014 - 2017h) */
	unsigned char		dmy_peri_2018_201F[0x2020-0x2018];	/* 1E00_(2018 - 201Fh) */
};

struct io_i2c {
	union io_i2c_sdat	SDAT;								/* 1E00_(3000 - 3003h) */
	union io_i2c_st		ST;									/* 1E00_(3004 - 3007h) */
	union io_i2c_cst	CST;								/* 1E00_(3008 - 300Bh) */
	union io_i2c_ctl1	CTL1;								/* 1E00_(300C - 300Fh) */
	union io_i2c_addr	ADDR;								/* 1E00_(3010 - 3013h) */
	union io_i2c_ctl2	CTL2;								/* 1E00_(3014 - 3017h) */
	union io_i2c_topr	TOPR;								/* 1E00_(3018 - 301Bh) */
	union io_i2c_ctl3	CTL3;								/* 1E00_(301C - 301Fh) */
	unsigned char		dmy_peri_3020_33FF[0x3400-0x3020];	/* 1E00_(3020 - 33FFh) */
};


/* Define I/O mapping		*/
struct io_peri {
	/* 32bit reload timer registers */
	struct io_tmr		TMR[14];							/* 1E00_(0000 - 00DFh)	*/
	unsigned char		dmy_peri_00E0_00FF[0x0100-0x00E0];	/* 1E00_(00E0 - 00FFh)	*/

	/* 64bit free run timer registers */
	struct io_tmr64		TMR64;								/* 1E00_(0100 - 0407h)	*/
	unsigned char		dmy_peri_0408_0FFF[0x1000-0x0408];	/* 1E00_(0408 - 0FFFh)	*/

	/* PWM timer registers */
	struct io_pwm		PWM[4]; 							/* 1E00_(1000 - 1FFFh)	*/

	/* UDC interface registers */
	struct io_udc		UDC[6]; 							/* 1E00_(2000 - 20BFh)	*/
	unsigned char		dmy_peri_20C0_2FFF[0x3000-0x20C0];	/* 1E00_(20C0 - 2FFFh)	*/

	/* I2C interface registers */	
	struct io_i2c		I2C[3]; 							/* 1E00_(3000 - 3BFFh)	*/
	unsigned char		dmy_peri_3C00_3FFF[0x4000-0x3C00];	/* 1E00_(3C00 - 3FFFh)	*/
};


struct io_pwmdma {
	/* PWM buffer data window registers */
	unsigned short		BFDTPT[512];						/* 1E12_(0000 - 03FFh)	*/
	unsigned char		dmy_peri_0400_FFFF[0x30000-0x20400];/* 1E12_(0400 - FFFFh) */
};


extern volatile struct io_peri	 IO_PERI;
extern volatile struct io_pwmdma IO_PWMDMA[4];

