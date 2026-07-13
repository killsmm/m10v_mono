/**
 * @file		_gic.h
 * @brief		Definition GIC (Generic Interrupt Controller) I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/* structure of GICD_CTLR		(1D00_1000h) */
union io_gic_dist_ctlr {
	unsigned long		word;
	struct {
		unsigned long	ENABLEGR0			:1;
		unsigned long	ENABLEGR1			:1;
		unsigned long						:30;
	}bit;
};

/* structure of GICD_TYPER		(1D00_1004h) */
union io_gic_dist_typer {
	unsigned long		word;
	struct {
		unsigned long	ITLINESNUMBER		:5;
		unsigned long	CPUNUMBER			:3;
		unsigned long						:2;
		unsigned long	SECURITY_EXTN		:1;
		unsigned long	LSPI				:5;
		unsigned long						:16;
	}bit;
};

/* structure of GICD_IIDR		(1D00_1008h) */
union io_gic_dist_iidr {
	unsigned long		word;
	struct {
		unsigned long	IMPLEMENTER			:12;
		unsigned long	REVISION			:4;
		unsigned long	VARIANT				:4;
		unsigned long						:4;
		unsigned long	PRODUCTID     		:8;
	}bit;
};

/* structure of GICD_PPISR		(1D00_1D00h) */
union io_gic_dist_ppisr {
	unsigned long		word;
	struct {
		unsigned long						:9;
		unsigned long	PPI6_STATUS			:1;
		unsigned long	PPI5_STATUS			:1;
		unsigned long	PPI4_STATUS			:1;
		unsigned long	PPI0_STATUS			:1;
		unsigned long	PPI1_STATUS			:1;
		unsigned long	PPI2_STATUS			:1;
		unsigned long	PPI3_STATUS			:1;
		unsigned long						:16;
	}bit;
};

/* structure of GICD_SGIR		(1D00_1F00h) */
union io_gic_dist_sgir {
	unsigned long		word;
	struct {
		unsigned long	SGIINTID			:4;
		unsigned long						:11;
		unsigned long	NSATT				:1;
		unsigned long	CPUTARGETLIST		:8;
		unsigned long	TARGETLISTFILTER	:2;
		unsigned long						:6;
	}bit;
};


/* structure of GICC_CTLR		(1D00_2000h) */
union io_gic_cpu_ctlr {
	unsigned long		word;
	struct {
		unsigned long	ENABLEGRP0			:1;
		unsigned long	ENABLEGRP1			:1;
		unsigned long	ACKCTL				:1;
		unsigned long	FIQEN				:1;
		unsigned long	CBPR				:1;
		unsigned long	FIQBYPDISGRP0		:1;
		unsigned long	IRQBYPDISGRP0		:1;
		unsigned long	FIQBYPDISGRP1		:1;
		unsigned long	IRQBYPDISGRP1		:1;
		unsigned long	EOIMODES			:1;
		unsigned long	EOIMODENS			:1;
		unsigned long						:21;
	}bit;
};

/* structure of GICC_PMR		(1D00_2004h) */
union io_gic_cpu_pmr {
	unsigned long		word;
	struct {
		unsigned long	PRIORITY			:8;
		unsigned long						:24;
	}bit;
};

/* structure of GICC_BPR		(1D00_2008h) */
union io_gic_cpu_bpr {
	unsigned long		word;
	struct {
		unsigned long	BINARYPOINT			:3;
		unsigned long						:29;
	}bit;
};

/* structure of GICC_IAR		(1D00_200Ch) */
union io_gic_cpu_iar {
	unsigned long		word;
	struct {
		unsigned long	ACKINTID			:10;
		unsigned long	CPUID				:3;
		unsigned long						:19;
	}bit;
};

/* structure of GICC_EOIR		(1D00_2010h) */
union io_gic_cpu_eoir {
	unsigned long		word;
	struct {
		unsigned long	EOIINTID			:10;
		unsigned long	CPUID				:3;
		unsigned long						:19;
	}bit;
};

/* structure of GICC_RPR		(1D00_2014h) */
union io_gic_cpu_rpr {
	unsigned long		word;
	struct {
		unsigned long	PRIORITY			:8;
		unsigned long						:24;
	}bit;
};

/* structure of GICC_HPPIR		(1D00_2018h) */
union io_gic_cpu_hpir {
	unsigned long		word;
	struct {
		unsigned long	PENDINTID			:10;
		unsigned long	CPUID				:3;
		unsigned long						:19;
	}bit;
};

/* structure of GICC_ABPR		(1D00_201Ch) */
union io_gic_cpu_abpr {
	unsigned long		word;
	struct {
		unsigned long	BINARYPOINT			:3;
		unsigned long						:29;
	}bit;
};

/* structure of GICC_AIAR		(1D00_2020h) */
union io_gic_cpu_aiar {
	unsigned long		word;
	struct {
		unsigned long	ACKINTID			:10;
		unsigned long	CPUID				:3;
		unsigned long						:19;
	}bit;
};

/* structure of GICC_AEOIR		(1D00_2024h) */
union io_gic_cpu_aeoir {
	unsigned long		word;
	struct {
		unsigned long	EOIINTID			:10;
		unsigned long	CPUID				:3;
		unsigned long						:19;
	}bit;
};

/* structure of GICC_AHPPIR		(1D00_2028h) */
union io_gic_cpu_ahppir {
	unsigned long		word;
	struct {
		unsigned long	PENDINTID			:10;
		unsigned long	CPUID				:3;
		unsigned long						:19;
	}bit;
};

/* structure of GICC_IIDR		(1D00_20FCh) */
union io_gic_cpu_iidr {
	unsigned long		word;
	struct {
		unsigned long	IMPLEMENTER			:12;
		unsigned long	REVISION			:4;
		unsigned long	VERSION				:4;
		unsigned long	PRODUCTID			:12;
	}bit;
};

/* structure of GICC_DIR		(1D00_3000h) */
union io_gic_cpu_dir {
	unsigned long		word;
	struct {
		unsigned long	INTID				:10;
		unsigned long	CPUID				:3;
		unsigned long						:19;
	}bit;
};


/*	GIC Distributor register				*/
struct io_gic_dist {
	union io_gic_dist_ctlr				GICD_CTLR;						// 1D00_(1000 - 1003h) RW Distributor Control Register
	union io_gic_dist_typer				GICD_TYPER;						// 1D00_(1004 - 1007h) RO Interrupt Controller Type Register
	union io_gic_dist_iidr				GICD_IIDR;						// 1D00_(1008 - 100Bh) RO Distributor Implementer Identification Register
	unsigned char						dmy_100C_107F[0x1080 - 0x100C];	// 1D00_(100C - 107Fh) Reserved
	unsigned long						GICD_IGROUPR[32];				// 1D00_(1080 - 10FFh) RW Interrupt Group Registers
	unsigned long						GICD_ISENABLER[32];				// 1D00_(1100 - 117Fh) RW Interrupt Set-Enable Registers
	unsigned long						GICD_ICENABLER[32];				// 1D00_(1180 - 11FFh) RW Interrupt Clear-Enable Registers
	unsigned long						GICD_ISPENDR[32];				// 1D00_(1200 - 127Fh) RW Interrupt Set-Pending Registers
	unsigned long						GICD_ICPENDR[32];				// 1D00_(1280 - 12FFh) RW Interrupt Clear-Pending Registers
	unsigned long						GICD_ISACTIVER[32];				// 1D00_(1300 - 137Fh) RW Interrupt Set-Active Registers
	unsigned long						GICD_ICACTIVER[32];				// 1D00_(1380 - 13FFh) RW Interrupt Clear-Active Registers
	unsigned char						GICD_IPRIORITYR[512];			// 1D00_(1400 - 15FFh) RW Interrupt Priority Registers
	unsigned char						dmy_1600_17FF[0x1800 - 0x1600];	// 1D00_(1600 - 17FFh) Reserved
	unsigned char						GICD_ITARGETSR[512];			// 1D00_(1800 - 19FFh) RW Interrupt Processor Targets Registers
	unsigned char						dmy_1A00_1BFF[0x1C00 - 0x1A00];	// 1D00_(1A00 - 1BFFh) Reserved
	unsigned long						GICD_ICFGR[32];					// 1D00_(1C00 - 1C7Fh) RW Interrupt Clear-Active Registers
	unsigned char						dmy_1C80_1CFF[0x1D00 - 0x1C80];	// 1D00_(1C80 - 1CFFh) Reserved
	union io_gic_dist_ppisr				GICD_PPISR;						// 1D00_(1D00 - 1D03h) RO Private Peripheral Interrupt Status Register
	unsigned long						GICD_SPISR[15];					// 1D00_(1D04 - 1D3Fh) RO Shared Peripheral Interrupt Status Registers
	unsigned char						dmy_1D40_1DFF[0x1F00 - 0x1D40];	// 1D00_(1D40 - 1DFFh) Reserved
	union io_gic_dist_sgir				GICD_SGIR;						// 1D00_(1F00 - 1F03h) WO Software Generated Interrupt Register
	unsigned char						dmy_1F04_1F0F[0x1F10 - 0x1F04];	// 1D00_(1F04 - 1F0Fh) Reserved
	unsigned long						GICD_CPENDSGIR[4];				// 1D00_(1F10 - 1F1Fh) RW SGI Clear-Pending Registers
	unsigned long						GICD_SPENDSGIR[4];				// 1D00_(1F20 - 1F2Fh) RW SGI Set-Pending Registers
	unsigned char						dmy_1F30_1FFF[0x2000 - 0x1F30];	// 1D00_(1F30 - 1FFFh) Reserved
};

/*	GIC CPU Interface register				*/
struct io_gic_cpu {
	union io_gic_cpu_ctlr				GICC_CTLR;						// 1D00_(2000 - 2003h) RW CPU Interface Control Register
	union io_gic_cpu_pmr				GICC_PMR;						// 1D00_(2004 - 2007h) RW Interrupt Priority Mask Register
	union io_gic_cpu_bpr				GICC_BPR;						// 1D00_(2008 - 200Bh) RW Binary Point Register
	union io_gic_cpu_iar				GICC_IAR;						// 1D00_(200C - 200Fh) RO Interrupt Acknowledge Register
	union io_gic_cpu_eoir				GICC_EOIR;						// 1D00_(2010 - 2013h) WO End of Interrupt Register
	union io_gic_cpu_rpr				GICC_RPR;						// 1D00_(2014 - 2017h) RO Running Priority Register
	union io_gic_cpu_hpir				GICC_HPPIR;						// 1D00_(2018 - 201Bh) RO Highest Priority Pending Interrupt Register
	union io_gic_cpu_abpr				GICC_ABPR;						// 1D00_(201C - 201Fh) RW Active Priorities Registers
	union io_gic_cpu_aiar				GICC_AIAR;						// 1D00_(2020 - 2023h) RO Aliased Interrupt Acknowledge Register
	union io_gic_cpu_aeoir				GICC_AEOIR;						// 1D00_(2024 - 2027h) WO Aliased End of Interrupt Register
	union io_gic_cpu_ahppir				GICC_AHPPIR;					// 1D00_(2028 - 202Bh) RO Aliased Highest Priority Pending Interrupt Register
	unsigned char						dmy_202C_20CF[0x20D0 - 0x202C];	// 1D00_(202C - 20CFh) Reserved
	unsigned long						GICC_APR0;						// 1D00_(20D0 - 20D3h) RW Active Priorities Registers
	unsigned char						dmy_20D4_20DF[0x20E0 - 0x20D4];	// 1D00_(20D4 - 20DFh) Reserved
	unsigned long						GICC_NSAPR0;					// 1D00_(20E0 - 20E3h) RW Non-secure Active Priorities Registers
	unsigned char						dmy_20E4_20FB[0x20FC - 0x20E4];	// 1D00_(20E4 - 20FBh) Reserved
	union io_gic_cpu_iidr				GICC_IIDR;						// 1D00_(20FC - 20FFh) RO CPU Interface Identification Register
	unsigned char						dmy_2100_2FFF[0x3000 - 0x2100];	// 1D00_(2100 - 2FFFh) Reserved
	union io_gic_cpu_dir				GICC_DIR;						// 1D00_(3000 - 3003h) WO Deactivate Interrupt Register
};

extern volatile struct io_gic_dist		IO_GIC_DIST;
extern volatile struct io_gic_cpu		IO_GIC_CPU;
