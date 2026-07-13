/**
 * @file		_chiptop.h
 * @brief		Definition Chip-Top I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//chiptop registers are allowed word-accsess only.

/*	structure of CLKSEL1  (1D02_1000h)    */
union io_chiptop_clksel1 {
	unsigned long		word;
	struct {
		unsigned long	RCLK		:3;
		unsigned long	UHS1CLK0	:5;
		unsigned long	UHS1CLK1	:5;
		unsigned long	UHS1CLK2	:5;
		unsigned long	UHS2CLK		:4;
		unsigned long	NFCLK		:6;
		unsigned long	EMMCCLK		:3;
		unsigned long				:1;
	} bit;
};

/*	structure of CLKSEL2  (1D02_1004h)    */
union io_chiptop_clksel2 {
	unsigned long		word;
	struct {
		unsigned long	ELACLK		:4;
		unsigned long	JPGCLK		:4;
		unsigned long	GPUCLK		:4;
		unsigned long	IPUTMECLK	:4;
		unsigned long	IPUCLK		:4;
		unsigned long	MIFCLK		:4;
		unsigned long	HIFCLK		:4;
		unsigned long	RAWCLK		:3;
		unsigned long				:1;
	} bit;
};

/*	structure of CLKSEL3  (1D02_1008h)    */
union io_chiptop_clksel3 {
	unsigned long		word;
	struct {
		unsigned long	VDFCLK		:4;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		unsigned long	PXFCLK		:4;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		unsigned long	HEVENCCLK	:4;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		unsigned long	IPPCLK		:5;
		unsigned long	PASCLK		:4;
		unsigned long	IIPCLK		:4;
		unsigned long	SENMSKCLK	:2;
		unsigned long	SENCLK		:4;
		unsigned long	PIPESEL		:2;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		unsigned long				:3;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		unsigned long	HEVENCSEL	:2;
		unsigned long				:1;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	} bit;
};

/*	structure of CLKSEL4  (1D02_100Ch)    */
union io_chiptop_clksel4 {
	unsigned long		word;
	struct {
		unsigned long	CNR1CLK		:7;
		unsigned long	B2R1CLK		:7;
		unsigned long	LTM1CLK		:7;
		unsigned long	R2Y1CLK		:7;
		unsigned long	SRO1CLK_2	:4;
	} bit;
};

/*	structure of CLKSEL5  (1D02_1010h)    */
union io_chiptop_clksel5 {
	unsigned long		word;
	struct {
		unsigned long	LTMRBK1CLK	:6;
		unsigned long	B2B1CLK		:6;
		unsigned long	SRO1CLK		:5;
		unsigned long				:15;
	} bit;
};

/*	structure of CLKSEL6	(1D02_1014h)	*/
union io_chiptop_clksel6 {
	unsigned long word;
	struct {
		unsigned long	CNR2CLK		:7;
		unsigned long	B2R2CLK		:7;
		unsigned long	LTM2CLK		:7;
		unsigned long	R2Y2CLK		:7;
		unsigned long	SRO2CLK_2	:4;
	} bit;
};

/*	structure of CLKSEL7	(1D02_1018h)	*/
union io_chiptop_clksel7 {
	unsigned long word;
	struct {
		unsigned long	LTMRBK2CLK	:6;
		unsigned long	B2B2CLK		:6;
		unsigned long	SRO2CLK		:5;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		unsigned long				:15;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		unsigned long	MIPISEL		:2;
		unsigned long	PDM0SEL		:3;
		unsigned long	PDM1SEL		:3;
		unsigned long				:7;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	} bit;
};

/*	structure of CLKSEL8  (1D02_101Ch)    */
union io_chiptop_clksel8 {
	unsigned long		word;
	struct {
		unsigned long	DSPCLK		:3;
		unsigned long	SPICLK		:3;
		unsigned long	AUCLK		:3;
		unsigned long	AU0SEL		:2;
		unsigned long	AU2SEL		:2;
		unsigned long	AU3SEL		:2;
		unsigned long	NETAUSEL	:2;
		unsigned long	APCLK		:6;
		unsigned long	AP0SEL		:2;
		unsigned long	AP1SEL		:2;
		unsigned long	AP2SEL		:2;
		unsigned long	AP3SEL		:2;
		unsigned long				:1;
	} bit;
};

/*	structure of CLKSEL9  (1D02_1020h)    */
union io_chiptop_clksel9 {
	unsigned long		word;
	struct {
		unsigned long	PCLK		:7;
		unsigned long	HCLK		:5;
		unsigned long	HCLKBMH		:4;
		unsigned long	ACLKEXS		:4;
		unsigned long	ACLK		:4;
		unsigned long				:8;
	} bit;
};

/*	structure of CLKSEL10  (1D02_1024h)    */
union io_chiptop_clksel10 {
	unsigned long		word;
	struct {
		unsigned long	ACLK400		:3;
		unsigned long	MCLK200		:4;
		unsigned long	MCLK400		:3;
		unsigned long				:22;
	} bit;
};

/*	structure of CLKSEL11  (1D02_1028h)    */
union io_chiptop_clksel11 {
	unsigned long		word;
	struct {
		unsigned long	DCHREQ		:1;
		unsigned long				:31;
	} bit;
};

/*	structure of CLKSEL12  (1D02_102Ch)    */
union io_chiptop_clksel12 {
	unsigned long		word;
	struct {
		unsigned long	ACLK300		:2;
		unsigned long	GYROCLK		:3;
		unsigned long				:3;
		unsigned long	FPT0CLK		:4;
		unsigned long	FPT1CLK		:4;
		unsigned long	MECLK		:3;
		unsigned long	NFBCHCLK	:2;
		unsigned long	RIBCLK		:5;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		unsigned long	SHDRCLK		:4;
		unsigned long				:2;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		unsigned long	SHDRCLK		:5;
		unsigned long				:1;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	} bit;
};


/*	structure of PLLCNT1	(1D02_1030h)	*/
union io_chiptop_pllcnt1 {
	unsigned long word;
	struct {
		unsigned long	PL00ST		:1;
		unsigned long	PL01ST		:1;
		unsigned long	PL02ST		:1;
		unsigned long	PL03ST		:1;

		unsigned long	PL04ST		:1;
		unsigned long	PL05ST		:1;
		unsigned long	PL05AST		:1;
		unsigned long	PL06ST		:1;

		unsigned long	PL07ST		:1;
		unsigned long	PL08ST		:1;
		unsigned long	PL10ST		:1;
		unsigned long	PL10AST		:1;

		unsigned long	PL11ST		:1;
		unsigned long	DPL00ST		:1;
		unsigned long	DPL01ST		:1;
		unsigned long	DPL02ST		:1;

		unsigned long	DPL10ST		:1;
		unsigned long	DPL11ST		:1;
		unsigned long	DPL12ST		:1;
		unsigned long				:13;
	} bit;
};

/*	structure of PLLCNT2	(1D02_1034h)	*/
union io_chiptop_pllcnt2 {
	unsigned long word;
	struct {
		unsigned long	PL00SEL		:1;
		unsigned long	PL01SEL		:1;
		unsigned long	PL02SEL		:1;
		unsigned long	PL03SEL		:1;

		unsigned long	PL04SEL		:1;
		unsigned long	PL05SEL		:1;
		unsigned long	PL05ASEL	:1;
		unsigned long	PL06SEL		:1;

		unsigned long	PL07SEL		:1;
		unsigned long	PL08SEL		:1;
		unsigned long	PL10SEL		:1;
		unsigned long	PL10ASEL	:1;

		unsigned long	PL11SEL		:1;
		unsigned long				:3;

		unsigned long	PL01SSEN	:1;
		unsigned long	PL02SSEN	:1;
		unsigned long				:14;
	} bit;
};

/*	structure of PLLCNT3	(1D02_1038h)	*/
union io_chiptop_pllcnt3 {
	unsigned long word;
	struct {
		unsigned long	P00POSTDIV	:8;
		unsigned long	P00PREDIV	:2;
		unsigned long				:6;
		unsigned long	P00PLLDIV	:8;
		unsigned long				:8;
	} bit;
};

/*	structure of PLLCNT4	(1D02_103Ch)	*/
union io_chiptop_pllcnt4 {
	unsigned long word;
	struct {
		unsigned long	P00FNUM		:32;
	} bit;
};

/*	structure of PLLCNT5	(1D02_1040h)	*/
union io_chiptop_pllcnt5 {
	unsigned long word;
	struct {
		unsigned long	P00FDEN		:32;
	} bit;
};

/*	structure of PLLCNT6	(1D02_1044h)	*/
union io_chiptop_pllcnt6 {
	unsigned long word;
	struct {
		unsigned long	P01RATE		:10;
		unsigned long	P01FREQ		:2;
		unsigned long	P01MODE		:1;
		unsigned long				:3;
		unsigned long	P02RATE		:10;
		unsigned long	P02FREQ		:2;
		unsigned long	P02MODE		:1;
		unsigned long				:3;
	} bit;
};

/*	structure of PLLCNT7	(1D02_1048h)	*/
union io_chiptop_pllcnt7 {
	unsigned long word;
	struct {
		unsigned long	P03POSTDIV0	:4;
		unsigned long	P03POSTDIV1	:4;
		unsigned long	P03PREDIV	:3;
		unsigned long				:5;
		unsigned long	P03PLLDIV	:8;
		unsigned long	P03OC0		:1;
		unsigned long	P03OC1		:1;
		unsigned long	D0XOC		:1;
		unsigned long	D1XOC		:1;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		unsigned long	DPLX8		:1;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		unsigned long				:1;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		unsigned long	DSEL		:1;
		unsigned long				:2;
	} bit;
};

/*	structure of PLLCNT8	(1D02_104Ch)	*/
union io_chiptop_pllcnt8 {
	unsigned long word;
	struct {
		unsigned long	P04POSTDIV	:8;
		unsigned long	P04PREDIV	:3;
		unsigned long				:5;
		unsigned long	P04PLLDIV	:8;
		unsigned long				:8;
	} bit;
};

/*	structure of PLLCNT9	(1D02_1050h)	*/
union io_chiptop_pllcnt9 {
	unsigned long word;
	struct {
		unsigned long	P10APLLDIV	:2;
		unsigned long				:6;
		unsigned long	P08PLLDIV	:7;
		unsigned long				:1;
		unsigned long	P05CHG		:1;
		unsigned long				:15;
	} bit;
};


/*	structure of CLKSTOP1	(1D02_1054h)	*/
union io_chiptop_clkstop1 {
	unsigned long		word;
	struct {
		unsigned long	DSPCK		:2;
		unsigned long	DSPAX		:2;
		unsigned long	SENCK		:2;
		unsigned long	SENAX		:2;
		unsigned long	SENAH		:2;
		unsigned long	SENAP		:2;
		unsigned long	GPIOAP		:2;
		unsigned long	AU0CK		:2;
		unsigned long	AU2CK		:2;
		unsigned long	AU3CK		:2;
		unsigned long	AU4CK		:2;
		unsigned long	AU5CK		:2;
		unsigned long	NETAUCK		:2;
		unsigned long				:2;
		unsigned long	TEMPCK		:2;
		unsigned long				:2;
	}bit;
};

/*	structure of CLKSTOP2	(1D02_1058h)	*/
union io_chiptop_clkstop2 {
	unsigned long		word;
	struct {
		unsigned long	RCK			:2;
		unsigned long	UHS1CK0		:2;
		unsigned long	UHS1CK1		:2;
		unsigned long	UHS1CK2		:2;
		unsigned long	UHS2CK		:2;
		unsigned long	NFCK		:2;
		unsigned long	EMMCCK		:2;
		unsigned long	NETSECCK	:2;
		unsigned long	NETRCK		:2;
		unsigned long	EXSAX		:2;
		unsigned long	SPICK		:2;
		unsigned long	SLIMB00CK	:2;
		unsigned long	SLIMB01CK	:2;
		unsigned long	SLIMB10CK	:2;
		unsigned long	SLIMB11CK	:2;
		unsigned long	PCISUPPCK	:2;
	}bit;
};

/*	structure of CLKSTOP3	(1D02_105Ch)	*/
union io_chiptop_clkstop3 {
	unsigned long		word;
	struct {
		unsigned long	IIPCK		:2;
		unsigned long	IIPAP		:2;
		unsigned long	IIPAH		:2;
		unsigned long	IIPAX		:2;
		unsigned long	LCDCK		:2;
		unsigned long	HIFCK		:2;
		unsigned long	MIFCK		:2;
		unsigned long	DISPAP		:2;
		unsigned long	DISPAH		:2;
		unsigned long	DISPAX		:2;
		unsigned long	JPGCK		:2;
		unsigned long	JPGAX		:2;
		unsigned long	JPGAH		:2;
		unsigned long	JPGAP		:2;
		unsigned long	PDM0CK		:2;
		unsigned long	PDM1CK		:2;
	}bit;
};

/*	structure of CLKSTOP4	(1D02_1060h)	*/
union io_chiptop_clkstop4 {
	unsigned long		word;
	struct {
		unsigned long	GPUCK		:2;
		unsigned long	GPUAP		:2;
		unsigned long	GPUAH		:2;
		unsigned long	GPUAX		:2;
		unsigned long	FPT0CK		:2;
		unsigned long	FPT0AX		:2;
		unsigned long	FPT0AH		:2;
		unsigned long	FPT0AP		:2;
		unsigned long	FPT1CK		:2;
		unsigned long	FPT1AP		:2;
		unsigned long	FPT1AH		:2;
		unsigned long	FPT1AX		:2;
		unsigned long	APCK0		:2;
		unsigned long	APCK1		:2;
		unsigned long	APCK2		:2;
		unsigned long	APCK3		:2;
	}bit;
};

/*	structure of CLKSTOP5	(1D02_1064h)	*/
union io_chiptop_clkstop5 {
	unsigned long		word;
	struct {
		unsigned long	MICAX0		:2;
		unsigned long	MICAX1		:2;
		unsigned long	MICAX2		:2;
		unsigned long	MICAX3		:2;
		unsigned long	MICAX4		:2;
		unsigned long	MICAX5		:2;
		unsigned long	MICAX6		:2;
		unsigned long	MICAP0		:2;
		unsigned long	MICAP1		:2;
		unsigned long	MICAP2		:2;
		unsigned long	MICAP3		:2;
		unsigned long	MICAP4		:2;
		unsigned long	MICAP5		:2;
		unsigned long	MICAP6		:2;
		unsigned long				:4;
	}bit;
};

/*	structure of CLKSTOP6	(1D02_1068h)	*/
union io_chiptop_clkstop6 {
	unsigned long		word;
	struct {
		unsigned long	MICAH1		:2;
		unsigned long	MICAH2		:2;
		unsigned long	MICAH3		:2;
		unsigned long	IMGAX		:2;
		unsigned long	IMGAH0		:2;
		unsigned long	IMGAH1		:2;
		unsigned long	IMGAH3		:2;
		unsigned long	IMGAP3		:2;
		unsigned long	REGAP		:2;
		unsigned long	XCHAX		:2;
		unsigned long	XCHAP		:2;
		unsigned long	ELACK		:2;
		unsigned long	ELAAX		:2;
		unsigned long	ELAAP		:2;
		unsigned long				:4;
	}bit;
};

/*	structure of CLKSTOP7	(1D02_106Ch)	*/
union io_chiptop_clkstop7 {
	unsigned long		word;
	struct {
		unsigned long	IPUFDCK		:2;
		unsigned long	IPUVISCK	:2;
		unsigned long	IPUAX		:2;
		unsigned long	IPUAH		:2;
		unsigned long	RAWCK		:2;
		unsigned long	RAWAX		:2;
		unsigned long	RAWAP		:2;
		unsigned long	SHDRCK		:2;
		unsigned long	SHDRAX		:2;
		unsigned long	SHDRAH		:2;
		unsigned long	SHDRAP		:2;
		unsigned long	M0CK		:2;
		unsigned long	MECK		:2;
		unsigned long	MEAX		:2;
		unsigned long	MEAP		:2;
		unsigned long				:2;
	}bit;
};

/*	structure of CLKSTOP8	(1D02_1070h)	*/
union io_chiptop_clkstop8 {
	unsigned long		word;
	struct {
		unsigned long	RIBCK		:2;
		unsigned long	RIBAH		:2;
		unsigned long	HEVDFCK		:2;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		unsigned long	HEPXFCK		:2;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		unsigned long	HEVENCCK	:2;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		unsigned long	HEIPPCK		:2;
		unsigned long	UMC0HEVCMX2	:2;
		unsigned long	UMC0HEVCMX4	:2;
		unsigned long	UMC0RBRMX4	:2;
		unsigned long	UMC1HEVCMX2	:2;
		unsigned long	UMC1HEVCMX4	:2;
		unsigned long	UMC1RBRMX4	:2;
		unsigned long	UMC0CMNAX	:2;
		unsigned long	UMC0MX1AX	:2;
		unsigned long	UMC0MX2AX	:2;
		unsigned long	UMC0MX3AX	:2;
		unsigned long	UMC0MX4AX	:2;
	}bit;
};

/*	structure of CLKSTOP9	(1D02_1074h)	*/
union io_chiptop_clkstop9 {
	unsigned long		word;
	struct {
		unsigned long	UMC0MX5AX	:2;
		unsigned long	UMC1CMNAX	:2;
		unsigned long	UMC1MX1AX	:2;
		unsigned long	UMC1MX2AX	:2;
		unsigned long	UMC1MX3AX	:2;
		unsigned long	UMC1MX4AX	:2;
		unsigned long	UMC1MX5AX	:2;
		unsigned long	UMC0MX0AX3	:2;
		unsigned long	UMC0MX6AX4	:2;
		unsigned long	UMC0HEVCAX4	:2;
		unsigned long	UMC1MX0AX3	:2;
		unsigned long	UMC1MX6AX4	:2;
		unsigned long	UMC1HEVCAX4	:2;
		unsigned long	UMC0AP		:2;
		unsigned long	UMC1AP		:2;
		unsigned long				:2;
	}bit;
};

/*	structure of CLKSTOP10	(1D02_1078h)	*/
union io_chiptop_clkstop10 {
	unsigned long		word;
	struct {
		unsigned long	SRO1CK		:2;
		unsigned long	SRO1CK_2	:2;
		unsigned long	SRO1AX		:2;
		unsigned long	SRO1AH		:2;
		unsigned long	SRO1AP		:2;
		unsigned long	B2B1CK		:2;
		unsigned long	B2B1AX		:2;
		unsigned long	B2B1AH		:2;
		unsigned long	B2B1AP		:2;
		unsigned long	LTMRBK1CK	:2;
		unsigned long	B2R1CK		:2;
		unsigned long	B2R1AX		:2;
		unsigned long	B2R1AH		:2;
		unsigned long	B2R1AP		:2;
		unsigned long				:4;
	}bit;
};

/*	structure of CLKSTOP11	(1D02_107Ch)	*/
union io_chiptop_clkstop11 {
	unsigned long		word;
	struct {
		unsigned long	LTM1CK		:2;
		unsigned long	LTM1AX		:2;
		unsigned long	LTM1AH		:2;
		unsigned long	LTM1AP		:2;
		unsigned long	R2Y1CK		:2;
		unsigned long	R2Y1AX		:2;
		unsigned long	R2Y1AH		:2;
		unsigned long	R2Y1AP		:2;
		unsigned long	CNR1CK		:2;
		unsigned long	CNR1AX		:2;
		unsigned long	CNR1AP		:2;
		unsigned long	APAH		:2;
		unsigned long	DBGAP		:2;
		unsigned long	NFBCHCK		:2;
		unsigned long				:4;
	}bit;
};

/*	structure of CLKSTOP12	(1D02_1080h)	*/
union io_chiptop_clkstop12 {
	unsigned long		word;
	struct {
		unsigned long	SRO2CK		:2;
		unsigned long	SRO2CK_2	:2;
		unsigned long	SRO2AX		:2;
		unsigned long	SRO2AH		:2;
		unsigned long	SRO2AP		:2;
		unsigned long	B2B2CK		:2;
		unsigned long	B2B2AX		:2;
		unsigned long	B2B2AH		:2;
		unsigned long	B2B2AP		:2;
		unsigned long	LTMRBK2CK	:2;
		unsigned long	B2R2CK		:2;
		unsigned long	B2R2AX		:2;
		unsigned long	B2R2AH		:2;
		unsigned long	B2R2AP		:2;
		unsigned long				:4;
	}bit;
};

/*	structure of CLKSTOP13	(1D02_1084h)	*/
union io_chiptop_clkstop13 {
	unsigned long		word;
	struct {
		unsigned long	LTM2CK		:2;
		unsigned long	LTM2AX		:2;
		unsigned long	LTM2AH		:2;
		unsigned long	LTM2AP		:2;
		unsigned long	R2Y2CK		:2;
		unsigned long	R2Y2AX		:2;
		unsigned long	R2Y2AH		:2;
		unsigned long	R2Y2AP		:2;
		unsigned long	CNR2CK		:2;
		unsigned long	CNR2AX		:2;
		unsigned long	CNR2AP		:2;
		unsigned long	UMCVDFMX4	:2;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		unsigned long	UMCPXFMX4	:2;
		unsigned long	UMCVDFMX2	:2;
		unsigned long	UMCPXFMX2	:2;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		unsigned long	UMCHEVENCAX4:2;
		unsigned long	UMCVDFMX2	:2;
		unsigned long	HEVENCAP	:2;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		unsigned long	BMH1CK		:2;
	}bit;
};

/*	structure of CLKSTOP14	(1D02_1088h)	*/
union io_chiptop_clkstop14 {
	unsigned long		word;
	struct {
		unsigned long	STATAX		:2;
		unsigned long	STATAH		:2;
		unsigned long	STATAP		:2;
		unsigned long	PERIAH		:2;
		unsigned long	PERIAP		:2;
		unsigned long	SENMSKCK	:2;
		unsigned long	GYROCK		:2;
		unsigned long	EXSAH		:2;
		unsigned long	EXSAP		:2;
		unsigned long	PASCK		:2;
		unsigned long	BMH0CK		:2;
		unsigned long	BMH0AX		:2;
		unsigned long				:2;
		unsigned long	RDMAAX		:2;
		unsigned long	RDMAAP		:2;
		unsigned long	BMH1AX		:2;
	}bit;
};


/*	structure of CRSWR	(1D02_108Ch)	*/
union io_chiprst_crswr {
	unsigned long		word;
	struct {
		unsigned long	SRREQ		:1;
		unsigned long				:31;
	}bit;
};

/*	structure of CRRRS	(1D02_1090h)	*/
union io_chiprst_crrrs {
	unsigned long		word;
	struct {
		unsigned long	RIBSR		:1;
		unsigned long	HEIPPASYNCSR:1;
		unsigned long	HEIPPSYNCSR	:1;
		unsigned long	HEVDFSR		:1;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		unsigned long	HEPXFSR		:1;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		unsigned long	HEVENCSR	:1;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		unsigned long	DSPSR		:1;
		unsigned long				:2;
		unsigned long	CPU0_SR		:1;
		unsigned long	CPU1_SR		:1;
		unsigned long	CPU2_SR		:1;
		unsigned long	CPU3_SR		:1;
		unsigned long				:20;
	}bit;
};

/*	structure of CRRSM	(1D02_1094h)	*/
union io_chiprst_crrsm {
	unsigned long		word;
	struct {
		unsigned long	__WDRST			:1;
		unsigned long	__SWRST			:1;
		unsigned long	__SRST			:1;
		unsigned long	__PORST			:1;
		unsigned long					:28;
	}bit;
};


/*	structure of PDR	(1D02_200Ch)	*/
union io_chiptop_pdr {
	unsigned long		word[10];
	struct {
	/* PDR76 (1D02_200Ch) */
		unsigned long	P60			:1;
		unsigned long	P61			:1;
		unsigned long	P62			:1;
		unsigned long	P63			:1;
		unsigned long	P64			:1;
		unsigned long	P65			:1;
		unsigned long	P66			:1;
		unsigned long	P67			:1;
		unsigned long	P70			:1;
		unsigned long	P71			:1;
		unsigned long	P72			:1;
		unsigned long	P73			:1;
		unsigned long	P74			:1;
		unsigned long	P75			:1;
		unsigned long	P76			:1;
		unsigned long	P77			:1;
		unsigned long	PWE60		:1;
		unsigned long	PWE61		:1;
		unsigned long	PWE62		:1;
		unsigned long	PWE63		:1;
		unsigned long	PWE64		:1;
		unsigned long	PWE65		:1;
		unsigned long	PWE66		:1;
		unsigned long	PWE67		:1;
		unsigned long	PWE70		:1;
		unsigned long	PWE71		:1;
		unsigned long	PWE72		:1;
		unsigned long	PWE73		:1;
		unsigned long	PWE74		:1;
		unsigned long	PWE75		:1;
		unsigned long	PWE76		:1;
		unsigned long	PWE77		:1;
	/* PDR98 (1D02_2010h) */
		unsigned long	P80			:1;
		unsigned long	P81			:1;
		unsigned long	P82			:1;
		unsigned long	P83			:1;
		unsigned long	P84			:1;
		unsigned long	P85			:1;
		unsigned long	P86			:1;
		unsigned long	P87			:1;
		unsigned long	P90			:1;
		unsigned long	P91			:1;
		unsigned long	P92			:1;
		unsigned long	P93			:1;
		unsigned long				:4;
		unsigned long	PWE80		:1;
		unsigned long	PWE81		:1;
		unsigned long	PWE82		:1;
		unsigned long	PWE83		:1;
		unsigned long	PWE84		:1;
		unsigned long	PWE85		:1;
		unsigned long	PWE86		:1;
		unsigned long	PWE87		:1;
		unsigned long	PWE90		:1;
		unsigned long	PWE91		:1;
		unsigned long	PWE92		:1;
		unsigned long	PWE93		:1;
		unsigned long				:4;
	/* PDRBA (1D02_2014h) */
		unsigned long	PA0			:1;
		unsigned long	PA1			:1;
		unsigned long	PA2			:1;
		unsigned long	PA3			:1;
		unsigned long	PA4			:1;
		unsigned long	PA5			:1;
		unsigned long	PA6			:1;
		unsigned long	PA7			:1;
		unsigned long	PB0			:1;
		unsigned long	PB1			:1;
		unsigned long	PB2			:1;
		unsigned long	PB3			:1;
		unsigned long	PB4			:1;
		unsigned long				:3;
		unsigned long	PWEA0		:1;
		unsigned long	PWEA1		:1;
		unsigned long	PWEA2		:1;
		unsigned long	PWEA3		:1;
		unsigned long	PWEA4		:1;
		unsigned long	PWEA5		:1;
		unsigned long	PWEA6		:1;
		unsigned long	PWEA7		:1;
		unsigned long	PWEB0		:1;
		unsigned long	PWEB1		:1;
		unsigned long	PWEB2		:1;
		unsigned long	PWEB3		:1;
		unsigned long	PWEB4		:1;
		unsigned long				:3;
	/* PDRDC (1D02_2018h) */
		unsigned long	PC0			:1;
		unsigned long	PC1			:1;
		unsigned long	PC2			:1;
		unsigned long	PC3			:1;
		unsigned long	PC4			:1;
		unsigned long	PC5			:1;
		unsigned long				:2;
		unsigned long	PD0			:1;
		unsigned long	PD1			:1;
		unsigned long	PD2			:1;
		unsigned long	PD3			:1;
		unsigned long	PD4			:1;
		unsigned long	PD5			:1;
		unsigned long	PD6			:1;
		unsigned long				:1;
		unsigned long	PWEC0		:1;
		unsigned long	PWEC1		:1;
		unsigned long	PWEC2		:1;
		unsigned long	PWEC3		:1;
		unsigned long	PWEC4		:1;
		unsigned long	PWEC5		:1;
		unsigned long				:2;
		unsigned long	PWED0		:1;
		unsigned long	PWED1		:1;
		unsigned long	PWED2		:1;
		unsigned long	PWED3		:1;
		unsigned long	PWED4		:1;
		unsigned long	PWED5		:1;
		unsigned long	PWED6		:1;
		unsigned long				:1;
	/* PDRFE (1D02_201Ch) */
		unsigned long	PE0			:1;
		unsigned long	PE1			:1;
		unsigned long	PE2			:1;
		unsigned long	PE3			:1;
		unsigned long	PE4			:1;
		unsigned long	PE5			:1;
		unsigned long				:2;
		unsigned long	PF0			:1;
		unsigned long	PF1			:1;
		unsigned long	PF2			:1;
		unsigned long	PF3			:1;
		unsigned long	PF4			:1;
		unsigned long				:3;
		unsigned long	PWEE0		:1;
		unsigned long	PWEE1		:1;
		unsigned long	PWEE2		:1;
		unsigned long	PWEE3		:1;
		unsigned long	PWEE4		:1;
		unsigned long	PWEE5		:1;
		unsigned long				:2;
		unsigned long	PWEF0		:1;
		unsigned long	PWEF1		:1;
		unsigned long	PWEF2		:1;
		unsigned long	PWEF3		:1;
		unsigned long	PWEF4		:1;
		unsigned long				:3;
	/* PDRHG (1D02_2020h) */
		unsigned long	PG0			:1;
		unsigned long	PG1			:1;
		unsigned long	PG2			:1;
		unsigned long	PG3			:1;
		unsigned long	PG4			:1;
		unsigned long	PG5			:1;
		unsigned long	PG6			:1;
		unsigned long	PG7			:1;
		unsigned long	PH0			:1;
		unsigned long	PH1			:1;
		unsigned long	PH2			:1;
		unsigned long	PH3			:1;
		unsigned long				:4;
		unsigned long	PWEG0		:1;
		unsigned long	PWEG1		:1;
		unsigned long	PWEG2		:1;
		unsigned long	PWEG3		:1;
		unsigned long	PWEG4		:1;
		unsigned long	PWEG5		:1;
		unsigned long	PWEG6		:1;
		unsigned long	PWEG7		:1;
		unsigned long	PWEH0		:1;
		unsigned long	PWEH1		:1;
		unsigned long	PWEH2		:1;
		unsigned long	PWEH3		:1;
		unsigned long				:4;
	/* PDRJW (1D02_2024h) */
		unsigned long	PW0			:1;
		unsigned long	PW1			:1;
		unsigned long	PW2			:1;
		unsigned long	PW3			:1;
		unsigned long	PW4			:1;
		unsigned long	PW5			:1;
		unsigned long	PW6			:1;
		unsigned long	PW7			:1;
		unsigned long	PJ0			:1;
		unsigned long	PJ1			:1;
		unsigned long	PJ2			:1;
		unsigned long	PJ3			:1;
		unsigned long	PJ4			:1;
		unsigned long	PJ5			:1;
		unsigned long	PJ6			:1;
		unsigned long	PJ7			:1;
		unsigned long	PWEW0		:1;
		unsigned long	PWEW1		:1;
		unsigned long	PWEW2		:1;
		unsigned long	PWEW3		:1;
		unsigned long	PWEW4		:1;
		unsigned long	PWEW5		:1;
		unsigned long	PWEW6		:1;
		unsigned long	PWEW7		:1;
		unsigned long	PWEJ0		:1;
		unsigned long	PWEJ1		:1;
		unsigned long	PWEJ2		:1;
		unsigned long	PWEJ3		:1;
		unsigned long	PWEJ4		:1;
		unsigned long	PWEJ5		:1;
		unsigned long	PWEJ6		:1;
		unsigned long	PWEJ7		:1;
	/* PDRLK (1D02_2028h) */
		unsigned long	PK0			:1;
		unsigned long	PK1			:1;
		unsigned long	PK2			:1;
		unsigned long	PK3			:1;
		unsigned long	PK4			:1;
		unsigned long	PK5			:1;
		unsigned long	PK6			:1;
		unsigned long	PK7			:1;
		unsigned long	PL0			:1;
		unsigned long	PL1			:1;
		unsigned long	PL2			:1;
		unsigned long	PL3			:1;
		unsigned long	PL4			:1;
		unsigned long	PL5			:1;
		unsigned long				:2;
		unsigned long	PWEK0		:1;
		unsigned long	PWEK1		:1;
		unsigned long	PWEK2		:1;
		unsigned long	PWEK3		:1;
		unsigned long	PWEK4		:1;
		unsigned long	PWEK5		:1;
		unsigned long	PWEK6		:1;
		unsigned long	PWEK7		:1;
		unsigned long	PWEL0		:1;
		unsigned long	PWEL1		:1;
		unsigned long	PWEL2		:1;
		unsigned long	PWEL3		:1;
		unsigned long	PWEL4		:1;
		unsigned long	PWEL5		:1;
		unsigned long				:2;
	/* PDRNM (1D02_202Ch) */
		unsigned long	PM0			:1;
		unsigned long	PM1			:1;
		unsigned long	PM2			:1;
		unsigned long	PM3			:1;
		unsigned long	PM4			:1;
		unsigned long				:3;
		unsigned long	PN0			:1;
		unsigned long	PN1			:1;
		unsigned long	PN2			:1;
		unsigned long	PN3			:1;
		unsigned long	PN4			:1;
		unsigned long				:3;
		unsigned long	PWEM0		:1;
		unsigned long	PWEM1		:1;
		unsigned long	PWEM2		:1;
		unsigned long	PWEM3		:1;
		unsigned long	PWEM4		:1;
		unsigned long				:3;
		unsigned long	PWEN0		:1;
		unsigned long	PWEN1		:1;
		unsigned long	PWEN2		:1;
		unsigned long	PWEN3		:1;
		unsigned long	PWEN4		:1;
		unsigned long				:3;
	/* PDRPY (1D02_2030h) */
		unsigned long	PY0			:1;
		unsigned long	PY1			:1;
		unsigned long	PY2			:1;
		unsigned long	PY3			:1;
		unsigned long	PY4			:1;
		unsigned long	PY5			:1;
		unsigned long	PY6			:1;
		unsigned long	PY7			:1;
		unsigned long	PP0			:1;
		unsigned long	PP1			:1;
		unsigned long	PP2			:1;
		unsigned long	PP3			:1;
		unsigned long				:4;
		unsigned long	PWEY0		:1;
		unsigned long	PWEY1		:1;
		unsigned long	PWEY2		:1;
		unsigned long	PWEY3		:1;
		unsigned long	PWEY4		:1;
		unsigned long	PWEY5		:1;
		unsigned long	PWEY6		:1;
		unsigned long	PWEY7		:1;
		unsigned long	PWEP0		:1;
		unsigned long	PWEP1		:1;
		unsigned long	PWEP2		:1;
		unsigned long	PWEP3		:1;
		unsigned long				:4;
	}bit;
};


/*	structure of DDR	(1D02_210Ch)	*/
union io_chiptop_ddr {
	unsigned long		word[10];
	struct {
	/* DDR76 (1D02_210Ch) */
		unsigned long	P60			:1;
		unsigned long	P61			:1;
		unsigned long	P62			:1;
		unsigned long	P63			:1;
		unsigned long	P64			:1;
		unsigned long	P65			:1;
		unsigned long	P66			:1;
		unsigned long	P67			:1;
		unsigned long	P70			:1;
		unsigned long	P71			:1;
		unsigned long	P72			:1;
		unsigned long	P73			:1;
		unsigned long	P74			:1;
		unsigned long	P75			:1;
		unsigned long	P76			:1;
		unsigned long	P77			:1;
		unsigned long				:16;
	/* DDR98 (1D02_2110h) */
		unsigned long	P80			:1;
		unsigned long	P81			:1;
		unsigned long	P82			:1;
		unsigned long	P83			:1;
		unsigned long	P84			:1;
		unsigned long	P85			:1;
		unsigned long	P86			:1;
		unsigned long	P87			:1;
		unsigned long	P90			:1;
		unsigned long	P91			:1;
		unsigned long	P92			:1;
		unsigned long	P93			:1;
		unsigned long				:20;
	/* DDRBA (1D02_2114h) */
		unsigned long	PA0			:1;
		unsigned long	PA1			:1;
		unsigned long	PA2			:1;
		unsigned long	PA3			:1;
		unsigned long	PA4			:1;
		unsigned long	PA5			:1;
		unsigned long	PA6			:1;
		unsigned long	PA7			:1;
		unsigned long	PB0			:1;
		unsigned long	PB1			:1;
		unsigned long	PB2			:1;
		unsigned long	PB3			:1;
		unsigned long	PB4			:1;
		unsigned long				:19;
	/* DDRDC (1D02_2118h) */
		unsigned long	PC0			:1;
		unsigned long	PC1			:1;
		unsigned long	PC2			:1;
		unsigned long	PC3			:1;
		unsigned long	PC4			:1;
		unsigned long	PC5			:1;
		unsigned long				:2;
		unsigned long	PD0			:1;
		unsigned long	PD1			:1;
		unsigned long	PD2			:1;
		unsigned long	PD3			:1;
		unsigned long	PD4			:1;
		unsigned long	PD5			:1;
		unsigned long	PD6			:1;
		unsigned long				:17;
	/* DDRFE (1D02_211Ch) */
		unsigned long	PE0			:1;
		unsigned long	PE1			:1;
		unsigned long	PE2			:1;
		unsigned long	PE3			:1;
		unsigned long	PE4			:1;
		unsigned long	PE5			:1;
		unsigned long				:2;
		unsigned long	PF0			:1;
		unsigned long	PF1			:1;
		unsigned long	PF2			:1;
		unsigned long	PF3			:1;
		unsigned long	PF4			:1;
		unsigned long				:19;
	/* DDRHG (1D02_2120h) */
		unsigned long	PG0			:1;
		unsigned long	PG1			:1;
		unsigned long	PG2			:1;
		unsigned long	PG3			:1;
		unsigned long	PG4			:1;
		unsigned long	PG5			:1;
		unsigned long	PG6			:1;
		unsigned long	PG7			:1;
		unsigned long	PH0			:1;
		unsigned long	PH1			:1;
		unsigned long	PH2			:1;
		unsigned long	PH3			:1;
		unsigned long				:20;
	/* DDRJW (1D02_2124h) */
		unsigned long	PW0			:1;
		unsigned long	PW1			:1;
		unsigned long	PW2			:1;
		unsigned long	PW3			:1;
		unsigned long	PW4			:1;
		unsigned long	PW5			:1;
		unsigned long	PW6			:1;
		unsigned long	PW7			:1;
		unsigned long	PJ0			:1;
		unsigned long	PJ1			:1;
		unsigned long	PJ2			:1;
		unsigned long	PJ3			:1;
		unsigned long	PJ4			:1;
		unsigned long	PJ5			:1;
		unsigned long	PJ6			:1;
		unsigned long	PJ7			:1;
		unsigned long				:16;
	/* DDRLK (1D02_2128h) */
		unsigned long	PK0			:1;
		unsigned long	PK1			:1;
		unsigned long	PK2			:1;
		unsigned long	PK3			:1;
		unsigned long	PK4			:1;
		unsigned long	PK5			:1;
		unsigned long	PK6			:1;
		unsigned long	PK7			:1;
		unsigned long	PL0			:1;
		unsigned long	PL1			:1;
		unsigned long	PL2			:1;
		unsigned long	PL3			:1;
		unsigned long	PL4			:1;
		unsigned long	PL5			:1;
		unsigned long				:18;
	/* DDRNM (1D02_212Ch) */
		unsigned long	PM0			:1;
		unsigned long	PM1			:1;
		unsigned long	PM2			:1;
		unsigned long	PM3			:1;
		unsigned long	PM4			:1;
		unsigned long				:3;
		unsigned long	PN0			:1;
		unsigned long	PN1			:1;
		unsigned long	PN2			:1;
		unsigned long	PN3			:1;
		unsigned long	PN4			:1;
		unsigned long				:19;
	/* DDRPY (1D02_2130h) */
		unsigned long	PY0			:1;
		unsigned long	PY1			:1;
		unsigned long	PY2			:1;
		unsigned long	PY3			:1;
		unsigned long	PY4			:1;
		unsigned long	PY5			:1;
		unsigned long	PY6			:1;
		unsigned long	PY7			:1;
		unsigned long	PP0			:1;
		unsigned long	PP1			:1;
		unsigned long	PP2			:1;
		unsigned long	PP3			:1;
		unsigned long				:20;
	}bit;
};

/*	structure of EPCR	(0x1D02_220C) */
union io_chiptop_epcr {
	unsigned long		word[10];
	struct {
	/* EPCR76 (1D02_220Ch) */
		unsigned long	P60			:1;
		unsigned long	P61			:1;
		unsigned long	P62			:1;
		unsigned long	P63			:1;
		unsigned long				:28;
	/* EPCR98 (1D02_2210h) */
		unsigned long	P80			:1;
		unsigned long	P81			:1;
		unsigned long	P82			:1;
		unsigned long	P83			:1;
		unsigned long	P84			:1;
		unsigned long	P85			:1;
		unsigned long	P86			:1;
		unsigned long	P87			:1;
		unsigned long	P90			:1;
		unsigned long				:1;
		unsigned long	P92			:1;
		unsigned long				:21;
	/* EPCRBA (1D02_2214h) */
		unsigned long	PA0			:1;
		unsigned long	PA1			:1;
		unsigned long				:1;
		unsigned long	PA3			:1;
		unsigned long	PA4			:1;
		unsigned long	PA5			:1;
		unsigned long				:1;
		unsigned long	PA7			:1;
		unsigned long	PB0			:1;
		unsigned long	PB1			:1;
		unsigned long	PB2			:1;
		unsigned long	PB3			:1;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		unsigned long	PB4			:1;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		unsigned long	TRGEN		:1;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		unsigned long				:19;
	/* EPCRDC (1D02_2218h) */
		unsigned long	PC0			:1;
		unsigned long	PC1			:1;
		unsigned long	PC2			:1;
		unsigned long	PC3			:1;
		unsigned long	PC4			:1;
		unsigned long	PC5			:1;
		unsigned long				:2;
		unsigned long	PD0			:1;
		unsigned long	PD1			:1;
		unsigned long	PD2			:1;
		unsigned long	PD3			:1;
		unsigned long	PD4			:1;
		unsigned long	PD5			:1;
		unsigned long	PD6			:1;
		unsigned long				:17;
	/* EPCRFE (1D02_221Ch) */
		unsigned long				:1;
		unsigned long	PE1			:1;
		unsigned long				:1;
		unsigned long	PE3			:1;
		unsigned long				:1;
		unsigned long	PE5			:1;
		unsigned long				:6;
		unsigned long	PF4			:1;
		unsigned long				:19;
	/* EPCRHG (1D02_2220h) */
		unsigned long	PG0			:1;
		unsigned long	PG1			:1;
		unsigned long	PG2			:1;
		unsigned long	PG3			:1;
		unsigned long	PG4			:1;
		unsigned long	PG5			:1;
		unsigned long	PG6			:1;
		unsigned long	PG7			:1;
		unsigned long	PH0			:1;
		unsigned long	PH1			:1;
		unsigned long	PH2			:1;
		unsigned long	PH3			:1;
		unsigned long				:20;
	/* EPCRJW (1D02_2224h) */
		unsigned long	PW0			:1;
		unsigned long	PW1			:1;
		unsigned long	PW2			:1;
		unsigned long	PW3			:1;
		unsigned long	PW4			:1;
		unsigned long	PW5			:1;
		unsigned long	PW6			:1;
		unsigned long	PW7			:1;
		unsigned long	PJ0			:1;
		unsigned long	PJ1			:1;
		unsigned long	PJ2			:1;
		unsigned long	PJ3			:1;
		unsigned long	PJ4			:1;
		unsigned long	PJ5			:1;
		unsigned long	PJ6			:1;
		unsigned long	PJ7			:1;
		unsigned long				:16;
	/* EPCRLK (1D02_2228h) */
		unsigned long	PK0			:1;
		unsigned long	PK1			:1;
		unsigned long	PK2			:1;
		unsigned long	PK3			:1;
		unsigned long	PK4			:1;
		unsigned long	PK5			:1;
		unsigned long	PK6			:1;
		unsigned long	PK7			:1;
		unsigned long	PL0			:1;
		unsigned long	PL1			:1;
		unsigned long	PL2			:1;
		unsigned long	PL3			:1;
		unsigned long	PL4			:1;
		unsigned long	PL5			:1;
		unsigned long				:18;
	/* EPCRNM (1D02_222Ch) */
		unsigned long				:1;
		unsigned long	PM1			:1;
		unsigned long				:1;
		unsigned long	PM3			:1;
		unsigned long	PM4			:1;
		unsigned long				:4;
		unsigned long	PN1			:1;
		unsigned long				:1;
		unsigned long	PN3			:1;
		unsigned long	PN4			:1;
		unsigned long				:19;
	/* EPCRPY (1D02_2230h) */
		unsigned long	PY0			:1;
		unsigned long	PY1			:1;
		unsigned long				:2;
		unsigned long	PY4			:1;
		unsigned long	PY5			:1;
		unsigned long	PY6			:1;
		unsigned long	PY7			:1;
		unsigned long	PP0			:1;
		unsigned long	PP1			:1;
		unsigned long	PP2			:1;
		unsigned long	PP3			:1;
		unsigned long				:20;
	}bit;
};

/*	structure of PUDER	(0x1D02_230C) */
union io_chiptop_puder {
	unsigned long		word[10];
	struct {
	/* PUDER76 (1D02_230Ch) */
		unsigned long				:32;
	/* PUDER98 (1D02_2310h) */
		unsigned long	P80			:1;
		unsigned long	P81			:1;
		unsigned long	P82			:1;
		unsigned long	P83			:1;
		unsigned long				:2;
		unsigned long	P86			:1;
		unsigned long	P87			:1;
		unsigned long	P90			:1;
		unsigned long	P91			:1;
		unsigned long	P92			:1;
		unsigned long	P93			:1;
		unsigned long				:20;
	/* PUDERBA (1D02_2314h) */
		unsigned long	PA0			:1;
		unsigned long	PA1			:1;
		unsigned long	PA2			:1;
		unsigned long	PA3			:1;
		unsigned long	PA4			:1;
		unsigned long	PA5			:1;
		unsigned long	PA6			:1;
		unsigned long	PA7			:1;
		unsigned long	PB0			:1;
		unsigned long	PB1			:1;
		unsigned long	PB2			:1;
		unsigned long	PB3			:1;
		unsigned long	PB4			:1;
		unsigned long				:19;
	/* PUDERDC (1D02_2318h) */
		unsigned long				:8;
		unsigned long	PD0			:1;
		unsigned long	PD1			:1;
		unsigned long	PD2			:1;
		unsigned long	PD3			:1;
		unsigned long	PD4			:1;
		unsigned long	PD5			:1;
		unsigned long	PD6			:1;
		unsigned long				:17;
	/* PUDERFE (1D02_231Ch) */
		unsigned long	PE0			:1;
		unsigned long	PE1			:1;
		unsigned long				:8;
		unsigned long	PF2			:1;
		unsigned long	PF3			:1;
		unsigned long	PF4			:1;
		unsigned long				:19;
	/* PUDERHG (1D02_2320h) */
		unsigned long	PG0			:1;
		unsigned long	PG1			:1;
		unsigned long	PG2			:1;
		unsigned long	PG3			:1;
		unsigned long	PG4			:1;
		unsigned long	PG5			:1;
		unsigned long	PG6			:1;
		unsigned long	PG7			:1;
		unsigned long	PH0			:1;
		unsigned long	PH1			:1;
		unsigned long	PH2			:1;
		unsigned long	PH3			:1;
		unsigned long				:20;
	/* PUDERJW (1D02_2324h) */
		unsigned long	PW0			:1;
		unsigned long	PW1			:1;
		unsigned long	PW2			:1;
		unsigned long	PW3			:1;
		unsigned long	PW4			:1;
		unsigned long	PW5			:1;
		unsigned long	PW6			:1;
		unsigned long	PW7			:1;
		unsigned long	PJ0			:1;
		unsigned long	PJ1			:1;
		unsigned long	PJ2			:1;
		unsigned long	PJ3			:1;
		unsigned long	PJ4			:1;
		unsigned long	PJ5			:1;
		unsigned long	PJ6			:1;
		unsigned long	PJ7			:1;
		unsigned long				:16;
	/* PUDERLK (1D02_2328h) */
		unsigned long	PK0			:1;
		unsigned long	PK1			:1;
		unsigned long	PK2			:1;
		unsigned long	PK3			:1;
		unsigned long	PK4			:1;
		unsigned long	PK5			:1;
		unsigned long	PK6			:1;
		unsigned long	PK7			:1;
		unsigned long	PL0			:1;
		unsigned long	PL1			:1;
		unsigned long	PL2			:1;
		unsigned long	PL3			:1;
		unsigned long	PL4			:1;
		unsigned long	PL5			:1;
		unsigned long				:18;
	/* PUDERNM (1D02_232Ch) */
		unsigned long	PM0			:1;
		unsigned long	PM1			:1;
		unsigned long				:1;
		unsigned long	PM3			:1;
		unsigned long	PM4			:1;
		unsigned long				:3;
		unsigned long	PN0			:1;
		unsigned long	PN1			:1;
		unsigned long				:1;
		unsigned long	PN3			:1;
		unsigned long	PN4			:1;
		unsigned long				:19;
	/* PUDERPY (1D02_2330h) */
		unsigned long	PY0			:1;
		unsigned long	PY1			:1;
		unsigned long	PY2			:1;
		unsigned long	PY3			:1;
		unsigned long	PY4			:1;
		unsigned long	PY5			:1;
		unsigned long	PY6			:1;
		unsigned long	PY7			:1;
		unsigned long	PP0			:1;
		unsigned long	PP1			:1;
		unsigned long	PP2			:1;
		unsigned long	PP3			:1;
		unsigned long				:20;
	}bit;
};


/*	structure of PUDCR	(0x1D02_2400) */
union io_chiptop_pudcr {
	unsigned long		word[10];
	struct {
	/* PUDCR76 (1D02_240Ch) */
		unsigned long				:32;
	/* PUDCR98 (1D02_2410h) */
		unsigned long	P80			:1;
		unsigned long	P81			:1;
		unsigned long	P82			:1;
		unsigned long	P83			:1;
		unsigned long				:2;
		unsigned long	P86			:1;
		unsigned long	P87			:1;
		unsigned long	P90			:1;
		unsigned long	P91			:1;
		unsigned long	P92			:1;
		unsigned long	P93			:1;
		unsigned long				:20;
	/* PUDCRBA (1D02_2414h) */
		unsigned long	PA0			:1;
		unsigned long	PA1			:1;
		unsigned long	PA2			:1;
		unsigned long	PA3			:1;
		unsigned long	PA4			:1;
		unsigned long	PA5			:1;
		unsigned long	PA6			:1;
		unsigned long	PA7			:1;
		unsigned long	PB0			:1;
		unsigned long	PB1			:1;
		unsigned long	PB2			:1;
		unsigned long	PB3			:1;
		unsigned long	PB4			:1;
		unsigned long				:19;
	/* PUDCRDC (1D02_2418h) */
		unsigned long				:8;
		unsigned long	PD0			:1;
		unsigned long	PD1			:1;
		unsigned long	PD2			:1;
		unsigned long	PD3			:1;
		unsigned long	PD4			:1;
		unsigned long	PD5			:1;
		unsigned long	PD6			:1;
		unsigned long				:17;
	/* PUDCRFE (1D02_241Ch) */
		unsigned long	PE0			:1;
		unsigned long	PE1			:1;
		unsigned long				:8;
		unsigned long	PF2			:1;
		unsigned long	PF3			:1;
		unsigned long	PF4			:1;
		unsigned long				:19;
	/* PUDCRHG (1D02_2420h) */
		unsigned long	PG0			:1;
		unsigned long	PG1			:1;
		unsigned long	PG2			:1;
		unsigned long	PG3			:1;
		unsigned long	PG4			:1;
		unsigned long	PG5			:1;
		unsigned long	PG6			:1;
		unsigned long	PG7			:1;
		unsigned long	PH0			:1;
		unsigned long	PH1			:1;
		unsigned long	PH2			:1;
		unsigned long	PH3			:1;
		unsigned long				:20;
	/* PUDCRJW (1D02_2424h) */
		unsigned long	PW0			:1;
		unsigned long	PW1			:1;
		unsigned long	PW2			:1;
		unsigned long	PW3			:1;
		unsigned long	PW4			:1;
		unsigned long	PW5			:1;
		unsigned long	PW6			:1;
		unsigned long	PW7			:1;
		unsigned long	PJ0			:1;
		unsigned long	PJ1			:1;
		unsigned long	PJ2			:1;
		unsigned long	PJ3			:1;
		unsigned long	PJ4			:1;
		unsigned long	PJ5			:1;
		unsigned long	PJ6			:1;
		unsigned long	PJ7			:1;
		unsigned long				:16;
	/* PUDCRLK (1D02_2428h) */
		unsigned long	PK0			:1;
		unsigned long	PK1			:1;
		unsigned long	PK2			:1;
		unsigned long	PK3			:1;
		unsigned long	PK4			:1;
		unsigned long	PK5			:1;
		unsigned long	PK6			:1;
		unsigned long	PK7			:1;
		unsigned long	PL0			:1;
		unsigned long	PL1			:1;
		unsigned long	PL2			:1;
		unsigned long	PL3			:1;
		unsigned long	PL4			:1;
		unsigned long	PL5			:1;
		unsigned long				:18;
	/* PUDCRNM (1D02_242Ch) */
		unsigned long	PM0			:1;
		unsigned long	PM1			:1;
		unsigned long				:1;
		unsigned long	PM3			:1;
		unsigned long	PM4			:1;
		unsigned long				:3;
		unsigned long	PN0			:1;
		unsigned long	PN1			:1;
		unsigned long				:1;
		unsigned long	PN3			:1;
		unsigned long	PN4			:1;
		unsigned long				:19;
	/* PUDCRPY (1D02_2430h) */
		unsigned long	PY0			:1;
		unsigned long	PY1			:1;
		unsigned long	PY2			:1;
		unsigned long	PY3			:1;
		unsigned long	PY4			:1;
		unsigned long	PY5			:1;
		unsigned long	PY6			:1;
		unsigned long	PY7			:1;
		unsigned long	PP0			:1;
		unsigned long	PP1			:1;
		unsigned long	PP2			:1;
		unsigned long	PP3			:1;
		unsigned long				:20;
	}bit;
};


/*	structure of PERSEL1 (1D02_0000h) */
union io_chiptop_persel1 {
	unsigned long word;
	struct {
		unsigned long	I2C0CNT		:2;
		unsigned long	SDANC0		:1;
		unsigned long	SCLNC0		:1;
		unsigned long	I2C1CNT		:2;
		unsigned long	SDANC1		:1;
		unsigned long	SCLNC1		:1;
		unsigned long	I2C2CNT		:2;
		unsigned long	SDANC2		:1;
		unsigned long	SCLNC2		:1;
		unsigned long	DVWT2		:1;
		unsigned long	DHWT3		:1;
		unsigned long				:2;
		unsigned long	AU0DO0C		:1;
		unsigned long	AU0DO1C		:1;
		unsigned long	AU1DOC		:1;
		unsigned long	AU0MCC		:1;
		unsigned long	AU1MCC		:1;
		unsigned long	RLSEL3		:1;
		unsigned long	RLSEL4		:1;
		unsigned long	RLSEL5		:1;
		unsigned long	INTVDSEL	:3;
		unsigned long				:5;
	} bit;
};

/*	structure of PERSEL2 (1D02_0004h) */
union io_chiptop_persel2 {
	unsigned long word;
	struct {
		unsigned long	UDCTRG		:1;
		unsigned long	UDC0AM		:1;
		unsigned long	UDC0BM		:1;
		unsigned long	UDC1AM		:1;
		unsigned long	UDC1BM		:1;
		unsigned long	UDC2AM		:1;
		unsigned long	UDC2BM		:1;
		unsigned long	UDC3AM		:1;
		unsigned long	UDC3BM		:1;
		unsigned long	UDC4AM		:1;
		unsigned long	UDC4BM		:1;
		unsigned long	UDC5AM		:1;
		unsigned long	UDC5BM		:1;
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
		unsigned long				:19;
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
		unsigned long	SPIMSEN0	:1;
		unsigned long	SPIMSEN1	:1;
		unsigned long	SPIMSEN2	:1;
		unsigned long				:16;
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	} bit;
};

/*	structure of PERSEL3 (1D02_0008h) */
union io_chiptop_persel3 {
	unsigned long word;
	struct {
		unsigned long	AU1WPPS		:1;
		unsigned long	AU0WP11		:1;
		unsigned long	SNH2WP04	:1;
		unsigned long	SNV2WP05	:1;
		unsigned long	SNA2WP06	:1;
		unsigned long	SNH3WP07	:1;
		unsigned long	SNV3WP08	:1;
		unsigned long	SNA3WP09	:1;
		unsigned long	U1VBSOU5	:1;
		unsigned long	U2VBSOU6	:1;
		unsigned long	U2IDWP14	:1;
		unsigned long	PRT1SOU3	:1;
		unsigned long	PXW0WP12	:1;
		unsigned long	PXW1SOU7	:1;
		unsigned long	PXC0SCK7	:1;
		unsigned long	PXC1WP15	:1;
		unsigned long	SCK4WP13	:1;
		unsigned long	S3LSOU5WP14	:2;
		unsigned long	S3SWP15		:1;
		unsigned long	RTS1SCK2	:1;
		unsigned long				:1;
		unsigned long	U1IDWP13	:1;
		unsigned long	PRT0SCK3	:1;
		unsigned long				:8;
	} bit;
};

/*	structure of PERSEL4 (1D02_000Ch) */
union io_chiptop_persel4 {
	unsigned long word;
	struct {
		unsigned long	EI22UD0A	:1;
		unsigned long	EI23UD0B	:1;
		unsigned long	EI24UD1A	:1;
		unsigned long	EI25UD1B	:1;
		unsigned long	EI26UD2A	:1;
		unsigned long	EI27UD2B	:1;
		unsigned long	EI28UD3A	:1;
		unsigned long	EI29UD3B	:1;
		unsigned long	EI30UD4A	:1;
		unsigned long	EI31UD4B	:1;
		unsigned long	SD3CDXS		:1;
		unsigned long	SD3WPS		:1;
		unsigned long	U1OCDXS		:1;
		unsigned long	U1IDDIGS	:1;
		unsigned long	U2OCDXS		:1;
		unsigned long	U2IDDIGS	:1;
		unsigned long				:16;
	} bit;
};


/*	structure of MSELC (1D02_0010h) */
union io_chiptop_mselc {
	unsigned long word;
	struct {
		unsigned long	MSEL		:4;
		unsigned long				:28;
	} bit;
};


/*	structure of DBCNT1 (1D02_0014h) */
union io_chiptop_dbcnt1 {
	unsigned long word;
	struct {
		unsigned long				:14;
		unsigned long	T07DC		:2;
		unsigned long	T08DC		:2;
		unsigned long	T09DC		:2;
		unsigned long	T10DC		:2;
		unsigned long	T11DC		:2;
		unsigned long	T12DC		:2;
		unsigned long	T13DC		:2;
		unsigned long	T14DC		:2;
		unsigned long	T15DC		:2;
	} bit;
};

/*	structure of DBCNT2 (1D02_0018h) */
union io_chiptop_dbcnt2 {
	unsigned long word;
	struct {
		unsigned long	T16DC		:2;
		unsigned long	T17DC		:2;
		unsigned long	T18DC		:2;
		unsigned long	T19DC		:2;
		unsigned long	T20DC		:2;
		unsigned long	T21DC		:2;
		unsigned long	T22DC		:2;
		unsigned long	T23DC		:2;
		unsigned long	T24DC		:2;
		unsigned long	T25DC		:2;
		unsigned long	T26DC		:2;
		unsigned long	T27DC		:2;
		unsigned long	T28DC		:2;
		unsigned long	T29DC		:2;
		unsigned long	T30DC		:2;
		unsigned long	T31DC		:2;
	} bit;
};

/*	structure of DBCNT3 (1D02_001Ch) */
union io_chiptop_dbcnt3 {
	unsigned long word;
	struct {
		unsigned long	T32DC		:2;
		unsigned long	T33DC		:2;
		unsigned long	T34DC		:2;
		unsigned long	T35DC		:2;
		unsigned long	T36DC		:2;
		unsigned long	T37DC		:2;
		unsigned long	T38DC		:2;
		unsigned long	T39DC		:2;
		unsigned long	T40DC		:2;
		unsigned long	T41DC		:2;
		unsigned long	T42DC		:2;
		unsigned long	T43DC		:2;
		unsigned long	T44DC		:2;
		unsigned long	T45DC		:2;
		unsigned long	T46DC		:2;
		unsigned long	T47DC		:2;
	} bit;
};

/*	structure of DBCNT4 (1D02_0020h) */
union io_chiptop_dbcnt4 {
	unsigned long word;
	struct {
		unsigned long	T48DC		:2;
		unsigned long	T49DC		:2;
		unsigned long	T50DC		:2;
		unsigned long	T51DC		:2;
		unsigned long	T52DC		:2;
		unsigned long	T53DC		:2;
		unsigned long	T54DC		:2;
		unsigned long	T55DC		:2;
		unsigned long	T56DC		:2;
		unsigned long	T57DC		:2;
		unsigned long	T58DC		:2;
		unsigned long	T59DC		:2;
		unsigned long	T60DC		:2;
		unsigned long	T61DC		:2;
		unsigned long	T62DC		:2;
		unsigned long	T63DC		:2;
	} bit;
};

/*	structure of DBCNT5 (1D02_0024h) */
union io_chiptop_dbcnt5 {
	unsigned long word;
	struct {
		unsigned long	T64DC		:2;
		unsigned long	T65DC		:2;
		unsigned long	T66DC		:2;
		unsigned long				:26;
	} bit;
};

/*	structure of PUDCNT (1D02_0028h) */
union io_chiptop_pudcnt {
	unsigned long word;
	struct {
		unsigned long	EMMCNICS	:2;
		unsigned long	EMDATPUE	:1;
		unsigned long	EMCMDPUE	:1;
		unsigned long	EMDSPDE		:1;
		unsigned long	NFDATPUE	:1;
		unsigned long	NFRBXPUE	:1;
		unsigned long	NFDQSPUE	:1;
		unsigned long	SP1MISOPE	:1;
		unsigned long	SP1MOSIPE	:1;
		unsigned long	SP1SCKPE	:1;
		unsigned long	SP1CS0PE	:1;
		unsigned long	SP1MISOUDC	:1;
		unsigned long	SP1MOSIUDC	:1;
		unsigned long	SP1SCKUDC	:1;
		unsigned long	SP1CS0UDC	:1;
		unsigned long				:16;
	} bit;
};

/*	structure of PLLFREQ1	(1D02_3000h)	*/
union io_chiptop_pllfreq1 {
	unsigned long		word;
	struct {
		unsigned long	PLLIDIV			:8;
		unsigned long					:24;
	}bit;
};

/*	structure of ODIVCH0	(1D02_3040h)	*/
union io_chiptop_odivch0 {
	unsigned long		word;
	struct {
		unsigned long	ODIVCH0NUM		:8;
		unsigned long					:24;
	}bit;
};

/*	structure of CMDEN	(1D02_3080h)	*/
union io_chiptop_cmden {
	unsigned long		word;
	struct {
		unsigned long	CMDEN			:3;
		unsigned long					:29;
	}bit;
};

/* Define i/o mapping */
struct io_chiptop {
	union io_chiptop_persel1		PERSEL1;		/* 1D02_(0000 - 0003h) */
	union io_chiptop_persel2		PERSEL2;		/* 1D02_(0004 - 0007h) */
	union io_chiptop_persel3		PERSEL3;		/* 1D02_(0008 - 000Bh) */
	union io_chiptop_persel4		PERSEL4;		/* 1D02_(000C - 000Fh) */
	union io_chiptop_mselc			MSELC;			/* 1D02_(0010 - 0013h) */
	union io_chiptop_dbcnt1			DBCNT1;			/* 1D02_(0014 - 0017h) */
	union io_chiptop_dbcnt2			DBCNT2;			/* 1D02_(0018 - 001Bh) */
	union io_chiptop_dbcnt3			DBCNT3;			/* 1D02_(001C - 001Fh) */
	union io_chiptop_dbcnt4			DBCNT4;			/* 1D02_(0020 - 0023h) */
	union io_chiptop_dbcnt5			DBCNT5;			/* 1D02_(0024 - 0027h) */
	union io_chiptop_pudcnt			PUDCNT;			/* 1D02_(0028 - 002Bh) */
	unsigned char dmy_002C_0FFF[0x1000-0x002C];		/* 1D02_(002C - 0FFFh) */

	union io_chiptop_clksel1		CLKSEL1;		/* 1D02_(1000 - 1003h) */
	union io_chiptop_clksel2		CLKSEL2;		/* 1D02_(1004 - 1007h) */
	union io_chiptop_clksel3		CLKSEL3;		/* 1D02_(1008 - 100Bh) */
	union io_chiptop_clksel4		CLKSEL4;		/* 1D02_(100C - 100Fh) */
	union io_chiptop_clksel5		CLKSEL5;		/* 1D02_(1010 - 1013h) */
	union io_chiptop_clksel6		CLKSEL6;		/* 1D02_(1014 - 1017h) */
	union io_chiptop_clksel7		CLKSEL7;		/* 1D02_(1018 - 101Bh) */
	union io_chiptop_clksel8		CLKSEL8;		/* 1D02_(101C - 101Fh) */
	union io_chiptop_clksel9		CLKSEL9;		/* 1D02_(1020 - 1023h) */
	union io_chiptop_clksel10		CLKSEL10;		/* 1D02_(1024 - 1027h) */
	union io_chiptop_clksel11		CLKSEL11;		/* 1D02_(1028 - 102Bh) */
	union io_chiptop_clksel12		CLKSEL12;		/* 1D02_(102C - 102Fh) */

	union io_chiptop_pllcnt1		PLLCNT1;		/* 1D02_(1030 - 1033h) */
	union io_chiptop_pllcnt2		PLLCNT2;		/* 1D02_(1034 - 1037h) */
	union io_chiptop_pllcnt3		PLLCNT3;		/* 1D02_(1038 - 103Bh) */
	union io_chiptop_pllcnt4		PLLCNT4;		/* 1D02_(103C - 103Fh) */
	union io_chiptop_pllcnt5		PLLCNT5;		/* 1D02_(1040 - 1043h) */
	union io_chiptop_pllcnt6		PLLCNT6;		/* 1D02_(1044 - 1047h) */
	union io_chiptop_pllcnt7		PLLCNT7;		/* 1D02_(1048 - 104Bh) */
	union io_chiptop_pllcnt8		PLLCNT8;		/* 1D02_(104C - 104Fh) */
	union io_chiptop_pllcnt9		PLLCNT9;		/* 1D02_(1050 - 1053h) */

	union io_chiptop_clkstop1		CLKSTOP1;		/* 1D02_(1054 - 1057h) */
	union io_chiptop_clkstop2		CLKSTOP2;		/* 1D02_(1058 - 105Bh) */
	union io_chiptop_clkstop3		CLKSTOP3;		/* 1D02_(105C - 105Fh) */
	union io_chiptop_clkstop4		CLKSTOP4;		/* 1D02_(1060 - 1063h) */
	union io_chiptop_clkstop5		CLKSTOP5;		/* 1D02_(1064 - 1067h) */
	union io_chiptop_clkstop6		CLKSTOP6;		/* 1D02_(1068 - 106Bh) */
	union io_chiptop_clkstop7		CLKSTOP7;		/* 1D02_(106C - 106Fh) */
	union io_chiptop_clkstop8		CLKSTOP8;		/* 1D02_(1070 - 1073h) */
	union io_chiptop_clkstop9		CLKSTOP9;		/* 1D02_(1074 - 1077h) */
	union io_chiptop_clkstop10		CLKSTOP10;		/* 1D02_(1078 - 107Bh) */
	union io_chiptop_clkstop11		CLKSTOP11;		/* 1D02_(107C - 107Fh) */
	union io_chiptop_clkstop12		CLKSTOP12;		/* 1D02_(1080 - 1083h) */
	union io_chiptop_clkstop13		CLKSTOP13;		/* 1D02_(1084 - 1087h) */
	union io_chiptop_clkstop14		CLKSTOP14;		/* 1D02_(1088 - 108Bh) */

	union io_chiprst_crswr			CRSWR;			/* 1D02_(108C - 108Fh) */
	union io_chiprst_crrrs			CRRRS;			/* 1D02_(1090 - 1093h) */
	union io_chiprst_crrsm			CRRSM;			/* 1D02_(1094 - 1097h) */
	unsigned char dmy_1098_1FFF[0x2000-0x1098];		/* 1D02_(1098 - 1FFFh) */

	unsigned char dmy_2000_200B[0x200C-0x2000];		/* 1D02_(2000 - 200Bh) */
	union io_chiptop_pdr			PDR;			/* 1D02_(200C - 2033h) */
	unsigned char dmy_2034_20FF[0x2100-0x2034];		/* 1D02_(2034 - 20FFh) */
	unsigned char dmy_2100_210B[0x210C-0x2100];		/* 1D02_(2100 - 210Bh) */
	union io_chiptop_ddr			DDR;			/* 1D02_(210C - 2133h) */
	unsigned char dmy_2134_21FF[0x2200-0x2134];		/* 1D02_(2134 - 21FFh) */
	unsigned char dmy_2200_220B[0x220C-0x2200];		/* 1D02_(2200 - 220Bh) */
	union io_chiptop_epcr			EPCR;			/* 1D02_(220C - 2233h) */
	unsigned char dmy_2234_22FF[0x2300-0x2234];		/* 1D02_(2234 - 22FFh) */
	unsigned char dmy_2300_230B[0x230C-0x2300];		/* 1D02_(2300 - 230Bh) */
	union io_chiptop_puder			PUDER;			/* 1D02_(230C - 2333h) */
	unsigned char dmy_2334_23FF[0x2400-0x2334];		/* 1D02_(2334 - 23FFh) */
	unsigned char dmy_2400_240B[0x240C-0x2400];		/* 1D02_(2400 - 240Bh) */
	union io_chiptop_pudcr			PUDCR;			/* 1D02_(240C - 2433h) */
	unsigned char dmy_2434_2FFF[0x3000-0x2434];		/* 1D02_(2434 - 2FFFh) */
	union io_chiptop_pllfreq1		PLLFREQ1;		/* 1D02_(3000 - 3003h) */
	unsigned char dmy_3004_303F[0x3040-0x3004];		/* 1D02_(3004 - 303Fh) */
	union io_chiptop_odivch0		ODIVCH0;		/* 1D02_(3040 - 3043h) */
	unsigned char dmy_3044_307F[0x3080-0x3044];		/* 1D02_(3044 - 307Fh) */
	union io_chiptop_cmden			CMDEN;			/* 1D02_(3080 - 3083h) */
	unsigned char dmy_3084_FFFF[0x4000-0x3084];		/* 1D02_(3084 - 3FFFh) */
};

extern volatile struct io_chiptop IO_CHIPTOP;

