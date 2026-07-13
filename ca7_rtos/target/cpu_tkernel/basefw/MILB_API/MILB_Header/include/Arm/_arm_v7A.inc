/**
 * @file		_arm_v7A.inc
 * @brief		Definition ARM_V7A I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

//==============================================================================
// Base Address
//==============================================================================
	.equ	NIC_BASE,				0x1D100000					// NIC-400
	.equ	NIC_ASIB_BASE,			(NIC_BASE + 0x42000)		// NIC-400(Slave Interface)
	.equ	NIC_ASIB0_BASE,			(NIC_ASIB_BASE + 0x00)		// NIC-400(ASIB0)
	.equ	NIC_ASIB1_BASE,			(NIC_ASIB_BASE + 0x1000)	// NIC-400(ASIB1)
	.equ	NIC_ASIB2_BASE,			(NIC_ASIB_BASE + 0x2000)	// NIC-400(ASIB2)

	.equ	GIC_BASE,				0x1D000000 					// GIC-400(Interrupt Interface)
	.equ	GIC_DIST_BASE,			(GIC_BASE + 0x1000)			// GIC-400(Distributor)
	.equ	GIC_CPU_BASE,			(GIC_BASE + 0x2000)			// GIC-400(CPU Interface)

//==============================================================================
// NIC-400 Register Address
//==============================================================================
	.equ	NIC_REMAP,				(NIC_BASE + 0x00)			// Remap Register

	.equ	NIC_TIDEMARK_0,			(NIC_ASIB0_BASE + 0x40)		// ASIB0 Tidemark Value Register
	.equ	NIC_READ_QOS_0,			(NIC_ASIB0_BASE + 0x100)	// ASIB0 Read Channel QoS Value Register
	.equ	NIC_WRITE_QOS_0,		(NIC_ASIB0_BASE + 0x104)	// ASIB0 Write Channel QoS Value Register
	.equ	NIC_FUNC_MOD_0,			(NIC_ASIB0_BASE + 0x108)	// ASIB0 Functionality Modification Register
	.equ	NIC_TIDEMARK_1,			(NIC_ASIB1_BASE + 0x40)		// ASIB1 Tidemark Value Register
	.equ	NIC_READ_QOS_1,			(NIC_ASIB1_BASE + 0x100)	// ASIB1 Read Channel QoS Value Register
	.equ	NIC_WRITE_QOS_1,		(NIC_ASIB1_BASE + 0x104)	// ASIB1 Write Channel QoS Value Register
	.equ	NIC_FUNC_MOD_1,			(NIC_ASIB1_BASE + 0x108)	// ASIB1 Functionality Modification Register
	.equ	NIC_TIDEMARK_2,			(NIC_ASIB2_BASE + 0x40)		// ASIB2 Tidemark Value Register
	.equ	NIC_READ_QOS_2,			(NIC_ASIB2_BASE + 0x100)	// ASIB2 Read Channel QoS Value Register
	.equ	NIC_WRITE_QOS_2,		(NIC_ASIB2_BASE + 0x104)	// ASIB2 Write Channel QoS Value Register
	.equ	NIC_FUNC_MOD_2,			(NIC_ASIB2_BASE + 0x108)	// ASIB2 Functionality Modification Register

//==============================================================================
// GIC-400(Distributor) Register Address
//==============================================================================
	.equ	GICD_CTLR,				(GIC_DIST_BASE + 0x000)		// Distributor Control Register
	.equ	GICD_TYPER,				(GIC_DIST_BASE + 0x004)		// Interrupt Controller Type Register
	.equ	GICD_IIDR,				(GIC_DIST_BASE + 0x008)		// Distributor Implementer Identification Register
	.equ	GICD_IGROUPR,			(GIC_DIST_BASE + 0x080)		// Interrupt Group Registers
	.equ	GICD_ISENABLER,			(GIC_DIST_BASE + 0x100)		// Interrupt Set-Enable Registers
	.equ	GICD_ICENABLER,			(GIC_DIST_BASE + 0x180)		// Interrupt Clear-Enable Registers
	.equ	GICD_ISPENDR,			(GIC_DIST_BASE + 0x200)		// Interrupt Set-Pending Registers
	.equ	GICD_ICPENDR,			(GIC_DIST_BASE + 0x280)		// Interrupt Clear-Pending Registers
	.equ	GICD_ISACTIVER,			(GIC_DIST_BASE + 0x300)		// Interrupt Set-Active Registers
	.equ	GICD_ICACTIVER,			(GIC_DIST_BASE + 0x380)		// Interrupt Clear-Active Registers
	.equ	GICD_IPRIORITYR,		(GIC_DIST_BASE + 0x400)		// Interrupt Priority Registers
	.equ	GICD_ITARGETSR,			(GIC_DIST_BASE + 0x800)		// Interrupt Processor Targets Registers
	.equ	GICD_ICFGR,				(GIC_DIST_BASE + 0xC00)		// Interrupt Configuration Registers
	.equ	GICD_PPISR,				(GIC_DIST_BASE + 0xD00)		// Private Peripheral Interrupt Status Register
	.equ	GICD_SPISR,				(GIC_DIST_BASE + 0xD04)		// Shared Peripheral Interrupt Status Registers
	.equ	GICD_SGIR,				(GIC_DIST_BASE + 0xF00)		// Software Generated Interrupt Register
	.equ	GICD_CPENDSGIR,			(GIC_DIST_BASE + 0xF10)		// SGI Clear-Pending Registers
	.equ	GICD_SPENDSGIR,			(GIC_DIST_BASE + 0xF20)		// SGI Set-Pending Registers
	.equ	GICD_PIDR4,				(GIC_DIST_BASE + 0xFD0)		// Peripheral ID 4 Register
	.equ	GICD_PIDR5,				(GIC_DIST_BASE + 0xFD4)		// Peripheral ID 5 Register
	.equ	GICD_PIDR6,				(GIC_DIST_BASE + 0xFD8)		// Peripheral ID 6 Register
	.equ	GICD_PIDR7,				(GIC_DIST_BASE + 0xFDC)		// Peripheral ID 7 Register
	.equ	GICD_PIDR0,				(GIC_DIST_BASE + 0xFE0)		// Peripheral ID 0 Register
	.equ	GICD_PIDR1,				(GIC_DIST_BASE + 0xFE4)		// Peripheral ID 1 Register
	.equ	GICD_PIDR2,				(GIC_DIST_BASE + 0xFE8)		// Peripheral ID 2 Register
	.equ	GICD_PIDR3,				(GIC_DIST_BASE + 0xFEC)		// Peripheral ID 3 Register
	.equ	GICD_CIDR0,				(GIC_DIST_BASE + 0xFF0)		// Component ID 0 Register
	.equ	GICD_CIDR1,				(GIC_DIST_BASE + 0xFF4)		// Component ID 1 Register
	.equ	GICD_CIDR2,				(GIC_DIST_BASE + 0xFF8)		// Component ID 2 Register
	.equ	GICD_CIDR3,				(GIC_DIST_BASE + 0xFFC)		// Component ID 3 Register

//==============================================================================
// GIC-400(CPU Interface) Register Address
//==============================================================================
	.equ	GICC_CTLR,				(GIC_CPU_BASE + 0x000)		// CPU Interface Control Register
	.equ	GICC_PMR,				(GIC_CPU_BASE + 0x004)		// Interrupt Priority Mask Register
	.equ	GICC_BPR,				(GIC_CPU_BASE + 0x008)		// Binary Point Register
	.equ	GICC_IAR,				(GIC_CPU_BASE + 0x00C)		// Interrupt Acknowledge Register
	.equ	GICC_EOIR,				(GIC_CPU_BASE + 0x010)		// End of Interrupt Register
	.equ	GICC_RPR,				(GIC_CPU_BASE + 0x014)		// Running Priority Register
	.equ	GICC_HPPIR,				(GIC_CPU_BASE + 0x018)		// Highest Priority Pending Interrupt Register
	.equ	GICC_ABPR,				(GIC_CPU_BASE + 0x01C)		// Aliased Binary Point Register
	.equ	GICC_AIAR,				(GIC_CPU_BASE + 0x020)		// Aliased Interrupt Acknowledge Register
	.equ	GICC_AEOIR,				(GIC_CPU_BASE + 0x024)		// Aliased End of Interrupt Register
	.equ	GICC_AHPPIR,			(GIC_CPU_BASE + 0x028)		// Aliased Highest Priority Pending Interrupt Register
	.equ	GICC_APR0,				(GIC_CPU_BASE + 0x0D0)		// Active Priority Register
	.equ	GICC_NSAPR0,			(GIC_CPU_BASE + 0x0E0)		// Non-Secure Active Priority Register
	.equ	GICC_IIDR,				(GIC_CPU_BASE + 0x0FC)		// CPU Interface Identification Register
	.equ	GICC_DIR,				(GIC_CPU_BASE + 0x1000)		// Deactivate Interrupt Register

//==============================================================================
// System Control Register Value
//==============================================================================
	.equ	CP15_SCTLR_M_Bit,		(1<<0)						// Address translation enable bit
	.equ	CP15_SCTLR_A_Bit,		(1<<1)						// Alignment bit
	.equ	CP15_SCTLR_C_Bit,		(1<<2)						// Cache enable bit
	.equ	CP15_SCTLR_SW_Bit,		(1<<10)						// SWP and SWPB enable bit
	.equ	CP15_SCTLR_Z_Bit,		(1<<11)						// RAO/WI. Branch prediction enable bit
	.equ	CP15_SCTLR_I_Bit,		(1<<12)						// Instruction cache enable bit
	.equ	CP15_SCTLR_V_Bit,		(1<<13)						// Vectors bit
	.equ	CP15_SCTLR_WXN_Bit,		(1<<19)						// Write permission implies Execute Never
	.equ	CP15_SCTLR_UWXN_Bit,	(1<<20)						// Unprivileged write permission implies PL1 Execute Never
	.equ	CP15_SCTLR_EE_Bit,		(1<<25)						// Exception Endianness bit
	.equ	CP15_SCTLR_TRE_Bit,		(1<<28)						// TEX remap enable bit
	.equ	CP15_SCTLR_AFE_Bit,		(1<<29)						// Access flag enable bit
	.equ	CP15_SCTLR_TE_Bit,		(1<<30)						// Thumb Exception enable

//==============================================================================
// Auxiliary Control Register Value
//==============================================================================
	.equ	CP15_ACTLR_SMP_Bit,			(1<<6)					// Address translation enable bit
	.equ	CP15_ACTLR_DODMBS_Bit,		(1<<10)					// Alignment bit
	.equ	CP15_ACTLR_L2RADIS_Bit,		(1<<11)					// Cache enable bit
	.equ	CP15_ACTLR_L1RADIS_Bit,		(1<<12)					// SWP and SWPB enable bit
	.equ	CP15_ACTLR_L1PCTL_11_Bit,	(11<<13)				// RAO/WI. L1 Data prefetch control.(1 outstanding pre-fetch permitted)
	.equ	CP15_ACTLR_L1PCTL_10_Bit,	(10<<13)				// RAO/WI. L1 Data prefetch control.(2 outstanding pre-fetches permitted)
	.equ	CP15_ACTLR_L1PCTL_01_Bit,	(01<<13)				// RAO/WI. L1 Data prefetch control.(3 outstanding pre-fetches permitted)
	.equ	CP15_ACTLR_DDVM_Bit,		(1<<15)					// Disable Distributed Virtual Memory (DVM) transactions
	.equ	CP15_ACTLR_DDI_Bit,			(1<<28)					// Disable dual issue

//==============================================================================
// Program Status Register
//==============================================================================
	.equ	USR_MODE,				0x10						// User mode
	.equ	FIQ_MODE,				0x11						// FIQ mode
	.equ	IRQ_MODE,				0x12						// IRQ mode
	.equ	SVC_MODE,				0x13						// Supervisor mode
	.equ	ABT_MODE,				0x17						// Abort mode
	.equ	UNDEF_MODE,				0x1B						// Undefined mode
	.equ	SYS_MODE,				0x1F						// System mode
	.equ	T_Bit,					0x20						// Thumb execution state
	.equ	F_Bit,					0x40						// FIQ mask
	.equ	I_Bit,					0x80						// IRQ mask

