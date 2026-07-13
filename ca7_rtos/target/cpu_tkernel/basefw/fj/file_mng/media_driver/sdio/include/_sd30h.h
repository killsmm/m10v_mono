/**
 * @file		_sd30h.h
 * @brief		Definition JMLEXS_SD30H Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/* structure of SystemAddres			(1B00_0000h/1B01_0000h/1B02_0000h)(32bit only) */
/* structure of BlockCount & BlockSize	(1B00_0004h/1B01_0004h/1B02_0004h)(32bit only) */
union io_sdxc_blkinfo {
	unsigned long		word;
	struct {
		unsigned long	BlockSize	:16;		// BlockSize  Register (16 bit)
		unsigned long	BlockCount	:16;		// BlockCount Register (16 bit)
	} hword;
	struct {
		// BlockSize Register bit
		unsigned long	size	:12;			// Block Size
		unsigned long	bound	:3;				// Buff Bound
		unsigned long			:1;				// reserved
		// BlockCount Register bit
		unsigned long	count	:16;			// Block Count
    } bit;
};

/* structure of Argument1_0			(1B00_0008h/1B01_0008h/1B02_0008h)(32bit only) */

/* structure of Command & TransferMode	(1B00_000Ch/1B01_000Ch/1B02_000Ch)(32bit only) */
union io_sdxc_trnsinfo {
	unsigned long		word;
	struct {
		unsigned long	TransferMode	:16;	// TransferMode  Register (16 bit)
		unsigned long	Command			:16;	// Command       Register (16 bit)
    } hword;
	struct {
		// TransferMode Register bit
		unsigned long	__DMAE		:1;			// DMA Enable
		unsigned long	__BCE		:1;			// Bleck Count Enable
		unsigned long	__ACMDE		:2;			// Auto CMD Enable
		unsigned long	__DTDS		:1;			// Data Transfer Direction Select
		unsigned long	__MSBS		:1;			// Multi/Single Block Select
		unsigned long				:2;			// reserved
		unsigned long				:8;			// reserved
		// Command Register bit
		unsigned long	__RTS		:2;			// Response Type Select
		unsigned long				:1;			// reserved
		unsigned long	__CCRCCE	:1;			// Command CRC Check Enable
		unsigned long	__CMDICE	:1;			// Command Index Check Enable
		unsigned long	__DPS		:1;			// Data Present Select
		unsigned long	__CMDT		:2;			// Command Type
		unsigned long	__CMDIND	:6;			// Command Index
		unsigned long				:2;			// reserved
    } bit;
};

/* structure of Response				(1B00_0010h/1B01_0010/1B02_0010 - 001Fh)(32bit only) */
/* structure of BufferDataPort			(1B00_0020h/1B01_0020h/1B02_0020h)(32bit only) */

/* structure of PresentState			(1B00_0024h/1B01_0024h/1B02_0024h)(32bit only) */
union io_sdxc_presentstate {
	unsigned long		word;
	struct {
		unsigned long	CIHCMD		:1;			// Command InHibit CMD
		unsigned long	CIHDAT		:1;			// Command InHibit DAT
		unsigned long	DATLA		:1;			// DAT Line Active
		unsigned long	RTREQ		:1;			// Re-Tuning Request
		unsigned long				:4;			// reserved
		unsigned long	WTA			:1;			// Write Transfer Active
		unsigned long	RTA			:1;			// Read  Transfer Active
		unsigned long	BWE			:1;			// Buffer Write Enable
		unsigned long	BRE			:1;			// Buffer Read  Enable
		unsigned long				:4;			// reserved
		unsigned long	CI			:1;			// Card Inserted
		unsigned long	CSS			:1;			// Card State Stable
		unsigned long	CDPL		:1;			// Card Detect Pin Level
		unsigned long	WPSPL		:1;			// Write Protect Switch Pin Level
		unsigned long	DATSL		:4;			// DAT[3:0] Line Signal Level
		unsigned long	CMDLSL		:1;			// CMD Line Signal Level
		unsigned long				:7;			// reserved
    } bit;
};

/* structure of WakeupControl & BlockGapControl
	 	 & PowerControl & HostControl1	(1B00_0028h/1B01_0028h/1B02_0028h)(32bit only) */
union io_sdxc_ctrl1 {
	unsigned long		word;
	struct {
		unsigned long	HostControl1	:8;		// HostControl1    Register (8 bit)
		unsigned long	PowerControl	:8;		// PowerControl    Register (8 bit)
		unsigned long	BlockGapControl	:8;		// BlockGapControl Register (8 bit)
		unsigned long	WakeupControl	:8;		// WakeupControl   Register (8 bit)
    } byte;
	struct {
		// HostControl1 Register bit
		unsigned long	LEDX		:1;			// LEDX Control
		unsigned long	DTW			:1;			// Data Transfer Width
		unsigned long	HSE			:1;			// High Speed Enable
		unsigned long	DMASEL		:2;			// DMA Select
		unsigned long	EDTW		:1;			// Extended Data Transfer Width
		unsigned long	CDTL		:1;			// Card Detect Test Level
		unsigned long	CDSS		:1;			// Card Detect Signal Selection
		// PowerControl Register bit
		unsigned long	SDBP		:1;			// SD Bus Power
		unsigned long	SDBVS		:3;			// SD Bus Voltage Select
		unsigned long				:4;			// reserved
		// BlockGapControl Register bit
		unsigned long	SBGR		:1;			// Stop at Block Gap Request
		unsigned long	CR			:1;			// Continue Request
		unsigned long	RWC			:1;			// Read Wait Control
		unsigned long	IBG			:1;			// Interrupt at Block Gap
		unsigned long				:4;			// reserved
		// WakeuppControl Register bit
		unsigned long	WEESCIR		:1;			// Wakeup Event Enable of SD Card Interrupt
		unsigned long	WEESCI		:1;			// Wakeup Event Enable of SD Card Insersion
		unsigned long	WEESCR		:1;			// Wakeup Event Enable of SD Card Removal
		unsigned long				:5;			// reserved
    } bit;
};

/* structure of 
	SoftwareReset & TimeoutControl & ClockControl	(1B00_002Ch/1B01_002Ch/1B02_002Ch)(32bit only) */
union io_sdxc_ctrl3 {
	unsigned long		word;
	struct {
		unsigned long	ClockControl	:16;	// ClockControl   Register (16 bit)
		unsigned long	TimeoutControl	:8;		// TimeoutControl Register ( 8 bit)
		unsigned long	SoftReset		:8;		// SoftReset      Register ( 8 bit)
    } byte;
	struct {
		// ClockControl
		unsigned long	ICE			:1;			// Internal Clock Enable
		unsigned long	ICS			:1;			// Internal Clock Stable
		unsigned long	SDCE		:1;			// SD Clock Enable
		unsigned long				:2;			// reserved
		unsigned long	CGS			:1;			// Clock Genarator Select
		unsigned long	UBSDICLK	:2;			// SDCLK Frequency Select
		unsigned long	SDICLK		:8;			// SDCLK Frequency Select
		// TimeoutControl
		unsigned long	DTCV		:4;			// Data Timeout Counter Value
		unsigned long				:4;			// reserved
		// SoftReset
		unsigned long	SRA			:1;			// Software Reset for All
		unsigned long	SRCL		:1;			// Software Reset for CMD Line
		unsigned long	SRDL		:1;			// Software Reset for DAT Line
		unsigned long				:5;			// reserved
    } bit;
};

/* structure of 
	ErrorInterruptStatus & NormalInterruptStatus	(1B00_0030h/1B01_0030h/1B02_0030h)(32bit only) */
union io_sdxc_intst {
	unsigned long		word;
	struct {
		unsigned long	NIntStatus	:16;		// NormalInterruptStatus Register (16 bit)
		unsigned long	EIntStatus	:16;		// ErrorInterruptStatus  Register (16 bit)
	} hword;
	struct {
		// NormalInterruptStatus
		unsigned long	__CC		:1;			// Command  Complete
		unsigned long	__TC		:1;			// Transfer Complete
		unsigned long	__BGE		:1;			// Block Gap Event
		unsigned long	__DI		:1;			// DMA Interrupt
		unsigned long	__BWR		:1;			// Buffer Write Ready
		unsigned long	__BRR		:1;			// Buffer Read  Ready
		unsigned long	__CIS		:1;			// Card Insertion
		unsigned long	__CRE		:1;			// Card Removal
		unsigned long	__CI		:1;			// Card Interrupt
		unsigned long	__INTA		:1;			// INT_A
		unsigned long	__INTB		:1;			// INT_B
		unsigned long	__INTC		:1;			// INT_C
		unsigned long	__RTE		:1;			// Re-Tuning Event
		unsigned long				:2;			// reserved
		unsigned long	__EI		:1;			// Error Interrupt
		// ErrorInterruptStatus
		unsigned long	__CTE		:1;			// Command Timeout Error
		unsigned long	__CCE		:1;			// Command CRC Error
		unsigned long	__CEBE		:1;			// Command End Bit Error
		unsigned long	__CIE		:1;			// Command Index Error
		unsigned long	__DTE		:1;			// Data Timeout Error
		unsigned long	__DCE		:1;			// Data CRC Error
		unsigned long	__DEBE		:1;			// Data End Bit Error
		unsigned long	__CLE		:1;			// Current Limit Error
		unsigned long	__ACE		:1;			// Auto CMD Error
		unsigned long	__ADMAE		:1;			// ADMA Error
		unsigned long	__TE		:1;			// Tuning Error
		unsigned long				:1;			// reserved
		unsigned long	__BAE		:1;			// Boot Acknowledge Error
		unsigned long	__AC19E		:1;			// Auto CMD19 Error
		unsigned long	__AME		:1;			// AHB Master Error
		unsigned long				:1;			// reserved
    } bit;
};

/* structure of 
	ErrorInterruptStatusEnable & NormalInterruptStatusEnable	(1B00_0034h/1B01_0034h/1B02_0034h)(32bit only) */
union io_sdxc_intstenb {
	unsigned long		word;
	struct {
		unsigned long	NIntStatusEnable	:16;	// NormalInterruptStatusEnable Register (16 bit)
		unsigned long	EIntStatusEnable	:16;	// ErrorInterruptStatusEnable  Register (16 bit)
	} hword;
	struct {
		// NormalInterruptStatusEnable
		unsigned long	CCSTE		:1;			// Command  Complete Status Enable
		unsigned long	TCSTE		:1;			// Transfer Complete Status Enable
		unsigned long	BGESTE		:1;			// Block Gap Event Status Enable
		unsigned long	DISTE		:1;			// DMA Interrupt Status Enable
		unsigned long	BWRSTE		:1;			// Buffer Write Ready Status Enable
		unsigned long	BRRSTE		:1;			// Buffer Read  Ready Status Enable
		unsigned long	CISSTE		:1;			// Card Insertion Status Enable
		unsigned long	CRESTE		:1;			// Card Removal Status Enable
		unsigned long	CISTE		:1;			// Card Interrupt Status Enable
		unsigned long	INTASTE		:1;			// INT_A Status Enable
		unsigned long	INTBSTE		:1;			// INT_B Status Enable
		unsigned long	INTCSTE		:1;			// INT_C Status Enable
		unsigned long	RTESTE		:1;			// Re-Tuning Event Status Enable
		unsigned long				:3;			// reserved
		// ErrorInterruptStatusEnable
		unsigned long	CTESTE		:1;			// Command Timeout Error Status Enable
		unsigned long	CCESTE		:1;			// Command CRC Error Status Enable
		unsigned long	CEBSTE		:1;			// Command End Bit Error Status Enable
		unsigned long	CIESTE		:1;			// Command Index Error Status Enable
		unsigned long	DTESTE		:1;			// Data Timeout Error Status Enable
		unsigned long	DCESTE		:1;			// Data CRC Error Status Enable
		unsigned long	DEBESTE		:1;			// Data End Bit Error Status Enable
		unsigned long	CLESTE		:1;			// Current Limit Error Status Enable
		unsigned long	ACESTE		:1;			// Auto CMD12 Error Status Enable
		unsigned long	ADMAESTE	:1;			// ADMA Error Status Enable
		unsigned long	TESTE		:1;			// Tuning Error Status Enable
		unsigned long				:1;			// reserved
		unsigned long	BAESTE		:1;			// Boot Acknowledge Error Status Enable
		unsigned long	AC19ESTE	:1;			// Auto CMD19 Error Status Enable
		unsigned long	AMESTE		:1;			// AHB Master Error Status Enable
		unsigned long				:1;			// reserved
    } bit;
};

/* structure of 
	ErrorInterruptSignalEnable & NormalInterruptSignalEnable	(1B00_0038h/1B01_0038h/1B02_0038h)(32bit only) */
union io_sdxc_intsigenb {
	unsigned long		word;
	struct {
		unsigned long	NIntSignalEnable	:16;	// NormalInterruptSignalEnable Register (16 bit)
		unsigned long	EIntSignalEnable	:16;	// ErrorInterruptSignalEnable  Register (16 bit)
	} hword;
	struct {
		// NormalInterruptSignalEnable
		unsigned long	CCSE		:1;			// Command  Complete Signal Enable
		unsigned long	TCSE		:1;			// Transfer Complete Signal Enable
		unsigned long	BGESE		:1;			// Block Gap Event Signal Enable
		unsigned long	DISE		:1;			// DMA Interrupt Signal Enable
		unsigned long	BWRSE		:1;			// Buffer Write Ready Signal Enable
		unsigned long	BRRSE		:1;			// Buffer Read  Ready Signal Enable
		unsigned long	CISSE		:1;			// Card Insertion Signal Enable
		unsigned long	CRESE		:1;			// Card Removal Signal Enable
		unsigned long	CISE		:1;			// Card Interrupt Signal Enable
		unsigned long	INTASE		:1;			// INT_A Signal Enable
		unsigned long	INTBSE		:1;			// INT_B Signal Enable
		unsigned long	INTCSE		:1;			// INT_C Signal Enable
		unsigned long	RTESE		:1;			// Re-Tuning Event Signal Enable
		unsigned long				:3;			// reserved
		// ErrorInterruptSignalEnable
		unsigned long	CTESE		:1;			// Command Timeout Error Signal Enable
		unsigned long	CCESE		:1;			// Command CRC Error Signal Enable
		unsigned long	CEBESE		:1;			// Command End Bit Error Signal Enable
		unsigned long	CIESE		:1;			// Command Index Error Signal Enable
		unsigned long	DTESE		:1;			// Data Timeout Error Signal Enable
		unsigned long	DCESE		:1;			// Data CRC Error Signal Enable
		unsigned long	DEBESE		:1;			// Data End Bit Error Signal Enable
		unsigned long	CLESE		:1;			// Current Limit Error Signal Enable
		unsigned long	ACESE		:1;			// Auto CMD12 Error Signal Enable
		unsigned long	ADMAESE		:1;			// ADMA Error Signal Enable
		unsigned long	TESE		:1;			// Tuning Error Signal Enable
		unsigned long				:1;			// reserved
		unsigned long	BAESE		:1;			// Boot Acknowledge Error Status Enable
		unsigned long	AC19ESE		:1;			// Auto CMD19 Error Signal Enable
		unsigned long	AMESE		:1;			// AHB Master Error Signal Enable
		unsigned long				:1;			// reserved
    } bit;
};

/* structure of HostControl2 & AutoCMD12errorStatus	(1B00_003Ch/1B01_003Ch/1B02_003Ch)(32bit only) */
union io_sdxc_ctrl2 {
	unsigned long		word;
	struct {
		unsigned long	ACMD12EStatus	:16;	// AutoCMDErrorStatus Register (16 bit)
		unsigned long	HostControl2	:16;	// HostControl2       Register (16 bit)
	} hword;
	struct {
		// AutoCMD12ErrorStatus
		unsigned long	ACEE		:1;			// Auto CMD12 not Executed Error
		unsigned long	ACRTE		:1;			// Auto CMD Response Timeout Error
		unsigned long	ACRCE		:1;			// Auto CMD Response CRC Error
		unsigned long	ACEBE		:1;			// Auto CMD End Bit Error
		unsigned long	ACIE		:1;			// Auto CMD12 Index Error
		unsigned long				:2;			// reserved
		unsigned long	CDACE		:1;			// CMC_wo_DAT not executed by Auto CMD12 Error
		unsigned long				:8;			// reserved
		// HostControl2
		unsigned long	UHSMS		:3;			// UHS Mode Select
		unsigned long	S18VE		:1;			// 1.8V Signaling Enable
		unsigned long	DSS			:2;			// Driver Strength Select
		unsigned long	EXT			:1;			// Execute Tuning
		unsigned long	SCS			:1;			// Sampling Clock Select
		unsigned long				:6;			// reserved
		unsigned long	ASIE		:1;			// Asynchronous Interrupt Enable
		unsigned long	PVE			:1;			// Preset Value Enable
    } bit;
};

/* structure of Capabilities1 & Capabilities0	(1B00_0040h/1B01_0040h/1B02_0040h)(32bit only) */
union io_sdxc_cap1_0 {
	unsigned long		word;
	struct {
		unsigned long	Capa0		:16;		// Capabilities0 Register (16 bit)
		unsigned long	Capa1		:16;		// Capabilities1 Register (16 bit)
	} hword;
	struct {
		// Capabilities0
		unsigned long	TCF			:6;			// Timeout Clock Frequency
		unsigned long				:1;			// reserved
		unsigned long	TC			:1;			// Timeout Clock Unit
		unsigned long	BCFSC		:8;			// Base Clock Frequency for SD Clock
		// Capabilities1
		unsigned long	MBL			:2;			// Max Block Length
		unsigned long	S8ED		:1;			// 8-bit Support for Embedded Device
		unsigned long	ADMA		:1;			// ADMA2 Support
		unsigned long				:1;			// reserved
		unsigned long	HSS			:1;			// High Speed Support
		unsigned long	SDMA		:1;			// SDMA Support
		unsigned long	SR			:1;			// Suspend/Resume Support
		unsigned long	VS33		:1;			// Voltage Support 3.3V
		unsigned long	VS30		:1;			// Voltage Support 3.0V
		unsigned long	VS18		:1;			// Voltage Support 1.8V
		unsigned long				:1;			// reserved
		unsigned long	SB64S		:1;			// 64-bit System Bus Support
		unsigned long	ASIS		:1;			// Asynchronous Interrupt Support
		unsigned long	SLT			:2;			// Slot Type
	} bit;
};

/* structure of Capabilities3 & Capabilities2	(1B00_0044h/1B01_0044h/1B02_0044h)(32bit only) */
union io_sdxc_cap3_2 {
	unsigned long		word;
	struct {
		unsigned long	Capa2		:16;		// Capabilities2 Register (16 bit)
		unsigned long	Capa3		:16;		// Capabilities3 Register (16 bit)
	} hword;
	struct {
		// Capabilities2
		unsigned long	SDR50S		:1;			// SDR50  Support
		unsigned long	SDR104S		:1;			// SDR104 Support
		unsigned long	DDRS		:1;			// DDR50  Support
		unsigned long				:1;			// reserved
		unsigned long	DTAS		:1;			// Driver Type A Support
		unsigned long	DTCS		:1;			// Driver Type C Support
		unsigned long	DTDS		:1;			// Driver Type D Support
		unsigned long				:1;			// reserved
		unsigned long	TCRT		:4;			// Timer Count for Re-Tuning
		unsigned long				:1;			// reserved
		unsigned long	UTSD50		:1;			// Use Tuning for SDR50
		unsigned long	RTM			:2;			// Re-Tuning Modes
		// Capabilities3
		unsigned long	CM			:8;			// Clock Multiplier
		unsigned long				:8;			// reserved
	} bit;
};

/* structure of MaximumCurrentCapabilities1 & MaximumCurrentCapabilities0	(1B00_0048h/1B01_0048h/1B02_0048h)(32bit only) */
union io_sdxc_mcurcap1_0 {
	unsigned long		word;
	struct {
		unsigned long	MaxCurrentCapa0		:16;	// MaximumCurrentCapabilities0 Register (16 bit)
		unsigned long	MaxCurrentCapa1		:16;	// MaximumCurrentCapabilities1 Register (16 bit)
	} hword;
	struct {
		// MaximumCurrentCapabilities0
		unsigned long	MC33		:8;			// Max Current for 3.3V
		unsigned long	MC30		:8;			// Max Current for 3.0V
		// MaximumCurrentCapabilities1
		unsigned long	MC18		:8;			// Max Current for 1.8V
		unsigned long				:8;			// reserved
	} bit;
};

/* structure of ForceEventforErrorInterruptStatus & ForceEventforAutoCMDErrorStatus	(1B00_0050h/1B01_0050h/1B02_0050h)(32bit only) */
union io_sdxc_forevt {
	unsigned long		word;
	struct {
		unsigned long	ForceEventForACMD	:16;	// ForceEventforErrorInterruptStatus Register (16 bit)
		unsigned long	ForceEventForErrInt	:16;	// ForceEventforErrorStatus          Register (16 bit)
	} hword;
	struct {
		// ForceEventforAutoCMDErrorStatus
		unsigned long	FEAC12NE	:1;			// Force Event for Auto CMD12 Not Executed
		unsigned long	FEACTE		:1;			// Force Event for Auto CMD Timeout Error
		unsigned long	FEACCRCE	:1;			// Force Event for Auto CMD CRC Error
		unsigned long	FEACEBE		:1;			// Force Event for Auto CMD End Bit Error
		unsigned long	FEACIE		:1;			// Force Event for Auto CMD Index Error
		unsigned long				:2;			// reserved
		unsigned long	FECNAC12E	:1;			// Force Event for Command Not Issued By Auto CMD12 Error
		unsigned long				:8;			// reserved
		// ForceEventforErrorInterruptStatus
		unsigned long	FECTE		:1;			// Force Event for Command Timeout Error
		unsigned long	FECCRCE		:1;			// Force Event for Command CRC Error
		unsigned long	FECEBE		:1;			// Force Event for Command End Bit Error
		unsigned long	FECIE		:1;			// Force Event for Command Index Error
		unsigned long	FEDTE		:1;			// Force Event for Data Timeout Error
		unsigned long	FEDCRCE		:1;			// Force Event for Data CRC Error
		unsigned long	FEDEBE		:1;			// Force Event for Data End Bit Error
		unsigned long	FECLE		:1;			// Force Event for Current Limit Error
		unsigned long	FEACMDE		:1;			// Force Event for Auto CMD12 Error
		unsigned long	FEADMAE		:1;			// Force Event for ADMA Error
		unsigned long	FETUNE		:1;			// Force Event for Tuning Error
		unsigned long				:1;			// reserved
		unsigned long	FEBAE		:1;			// Force Event for Boot Acknowledge Error
		unsigned long	FEAC19E		:1;			// Force Event for Auto CMD19 Error
		unsigned long	FEAHBME		:1;			// Force Event for AHB Master Error
		unsigned long				:1;			// reserved
	} bit;
};

/* structure of ADMAErrorStatus	(1B00_0054h/1B01_0054h/1B02_0054h)(32bit only) */
union io_sdxc_admaest {
	unsigned long		word;
	struct {
		unsigned long	ADMAES		:2;			// ADMA Error State
		unsigned long	ADMALME		:1;			// ADMA Length Mismatch Error
		unsigned long				:29;		// reserved
	} bit;
};

/* structure of PresetValue1	(1B00_0060h/1B01_0060h/1B02_0060h)(32bit only) */
union io_sdxc_preval1 {
	unsigned long		word;
	struct {
		// PresetValue for Init
		unsigned long	INIT_SCFSV	:10;		// SDCLK Frequency Select Value
		unsigned long	INIT_CGSV	:1;			// Clock Generator Select Value
		unsigned long				:3;			// reserved
		unsigned long	INIT_DSSV	:2;			// Driver Strength Select Value
		// PresetValue for DS
		unsigned long	DS_SCFSV	:10;		// SDCLK Frequency Select Value
		unsigned long	DS_CGSV		:1;			// Clock Generator Select Value
		unsigned long				:3;			// reserved
		unsigned long	DS_DSSV		:2;			// Driver Strength Select Value
	} bit;
};

/* structure of PresetValue2	(1B00_0064h/1B01_0064h/1B02_0064h)(32bit only) */
union io_sdxc_preval2 {
	unsigned long		word;
	struct {
		// PresetValue for HS
		unsigned long	HS_SCFSV		:10;	// SDCLK Frequency Select Value
		unsigned long	HS_CGSV			:1;		// Clock Generator Select Value
		unsigned long					:3;		// reserved
		unsigned long	HS_DSSV			:2;		// Driver Strength Select Value
		// PresetValue for SDR12
		unsigned long	SDR12_SCFSV		:10;	// SDCLK Frequency Select Value
		unsigned long	SDR12_CGSV		:1;		// Clock Generator Select Value
		unsigned long					:3;		// reserved
		unsigned long	SDR12_DSSV		:2;		// Driver Strength Select Value
	} bit;
};

/* structure of PresetValue3	(1B00_0068h/1B01_0068h/1B02_0068h)(32bit only) */
union io_sdxc_preval3 {
	unsigned long		word;
	struct {

		// PresetValue for SDR25
		unsigned long	SDR25_SCFSV		:10;	// SDCLK Frequency Select Value
		unsigned long	SDR25_CGSV		:1;		// Clock Generator Select Value
		unsigned long					:3;		// reserved
		unsigned long	SDR25_DSSV		:2;		// Driver Strength Select Value
		// PresetValue for SDR50
		unsigned long	SDR50_SCFSV		:10;	// SDCLK Frequency Select Value
		unsigned long	SDR50_CGSV		:1;		// Clock Generator Select Value
		unsigned long					:3;		// reserved
		unsigned long	SDR50_DSSV		:2;		// Driver Strength Select Value
	} bit;
};

/* structure of PresetValue4	(1B00_006Ch/1B01_006C/1B02_006C-6Fh)(32bit only) */
union io_sdxc_preval4 {
	unsigned long		word;
	struct {
		// PresetValue for SDR104
		unsigned long	SDR104_SCFSV	:10;	// SDCLK Frequency Select Value
		unsigned long	SDR104_CGSV		:1;		// Clock Generator Select Value
		unsigned long					:3;		// reserved
		unsigned long	SDR104_DSSV		:2;		// Driver Strength Select Value
		// PresetValue for DDR50
		unsigned long	DDR50_SCFSV		:10;	// SDCLK Frequency Select Value
		unsigned long	DDR50_CGSV		:1;		// Clock Generator Select Value
		unsigned long					:3;		// reserved
		unsigned long	DDR50_DSSV		:2;		// Driver Strength Select Value
	} bit;
};

/* structure of SharedBusControl	(1B00_00E0h/1B01_00E0h/1B02_00E0h)(32bit only) */
union io_sdxc_sharedbuscontrol {
	unsigned long		word;
	struct {
		unsigned long	NCP			:3;			// Number of Clock Pins
		unsigned long				:1;			// reserved
		unsigned long	NIIP		:2;			// Number of Interrupt Input Pins
		unsigned long				:2;			// reserved
		unsigned long	BWP			:7;			// Bus Width Preset
		unsigned long				:1;			// reserved
		unsigned long	CPS			:3;			// Clock Pin Select
		unsigned long				:1;			// reserved
		unsigned long	IPS			:3;			// Interrupt Pin Select
		unsigned long				:1;			// reserved
		unsigned long	BEPC		:7;			// Back-End Power Control
		unsigned long				:1;			// reserved
	} bit;
};

/* structure of HostControllerVersionVENDER & HostControllerVersionSPEC & SlotInterruptStatus	(1B00_00FCh/1B01_00FCh/1B02_00FCh)(32bit only) */
union io_sdxc_hctrlver {
	unsigned long		word;
	struct {
		unsigned long	SIntStatus		:16;	// SlotInterruptStatus   Register (16 bit)
		unsigned long	HostContSPEC	:8;		// HostControllerVersion Register ( 8 bit)
		unsigned long	HostContVENDER	:8;		// HostControllerVersion Register ( 8 bit)
	} hword;
	struct {
		// SlotInterruptStatus
		unsigned long	ISES0		:1;			// Interrupt Signal for Each Slot0
		unsigned long	ISES1		:1;			// Interrupt Signal for Each Slot1
		unsigned long	ISES2		:1;			// Interrupt Signal for Each Slot2
		unsigned long	ISES3		:1;			// Interrupt Signal for Each Slot3
		unsigned long	ISES4		:1;			// Interrupt Signal for Each Slot4
		unsigned long	ISES5		:1;			// Interrupt Signal for Each Slot5
		unsigned long	ISES6		:1;			// Interrupt Signal for Each Slot6
		unsigned long	ISES7		:1;			// Interrupt Signal for Each Slot7
		unsigned long				:8;			// reserved
		// HostControllerVersionSPEC
		unsigned long	SVN			:8;			// Specification Version Number
		// HostControllerVersionVENDER
		unsigned long	VVN			:8;			// Vender Version Number
	} bit;
};

/* structure of AHBConfig(QRST)	(1B00_0100h/1B01_0100h/1B02_0100h)(32bit only) */
union io_sdxc_ahbconfig {
	unsigned long		word;
	struct {
		unsigned long	INCRSEL		:3;			// INCR Select
		unsigned long	SINEN		:1;			// INCR/SINGLE select
		unsigned long	BSLOCK		:1;			// Bus Lock
		unsigned long	BSLOCKSEL	:1;			// Bus Lock Select
		unsigned long	BLES		:1;			// Big/Little Endian Select
		unsigned long				:25;		// reserved
	} bit;
};

/* structure of PowerSwitching	(1B00_0104h/1B01_0104h/1B02_0104h)(32bit only) */
union io_sdxc_powerswitching {
	unsigned long		word;
	struct {
		unsigned long	APSWEN		:1;			// Auto Power Switching Enable
		unsigned long	IORS		:1;			// I/O Register Selecting
		unsigned long				:30;		// reserved
	} bit;
};

/* structure of TuningSetting	(1B00_0108h/1B01_0108h/1B02_0108h)(32bit only) */
union io_sdxc_tuningsetting {
	unsigned long		word;
	struct {
		unsigned long	TPS			:8;			// Tuning Point Select
		unsigned long	TPSE		:1;			// Tuning Phase Select Enable
		unsigned long	TEBS		:1;			// Tuning Error Border Select
		unsigned long	RTTS		:1;			// Re-Tuning Tap Select
		unsigned long	RTRS		:2;			// Re-Tuning Range Select
		unsigned long				:3;			// reserved
		unsigned long	CCCD		:1;			// CMD Conflict Check Disable
		unsigned long				:7;			// reserved
		unsigned long	DTCVART		:4;			// Data Timeout Counter Value For Auto Re-Tuning
		unsigned long				:4;			// reserved
	} bit;
};

/* structure of TuningStatus	(1B00_010Ch/1B01_010Ch/1B02_010Ch)(32bit only) */
union io_sdxc_tuningstate {
	unsigned long		word;
	struct {
		unsigned long	P8TRTR		:8;			// 8Phase Tuning/Re-Tuning Result
		unsigned long	P3RTR		:3;			// 3Phase Re-Tuning Result
		unsigned long				:5;			// reserved
		unsigned long	PTP			:8;			// Preset Tuning Point
		unsigned long				:8;			// reserved
	} bit;
};

/* structure of IOControl1	(1B00_0110h/1B01_0110h/1B02_0110h)(32bit only) */
union io_sdxc_iocontrol1 {
	unsigned long		word;
	struct {
		unsigned long	SDXDAT33OE	:4;			// SD_XDAT33OE_O Control
		unsigned long	SDXDAT18OE	:4;			// SD_XDAT18OE_O Control
		unsigned long	SDDAT33PC	:4;			// SD_DAT33PC_O  Control
		unsigned long	SDDAT18PC	:4;			// SD_DAT18PC_O  Control
		unsigned long	SDXCMD33OE	:1;			// SD_XCMD33OE_O Control
		unsigned long	SDXCMD18OE	:1;			// SD_XCMD18OE_O Control
		unsigned long	SDCMD33PC	:1;			// SD_CMD33PC_O  Control
		unsigned long	SDCMD18PC	:1;			// SD_CMD18PC_O  Control
		unsigned long	SDXCLK33OE	:1;			// SD_XCLK33OE_O Control
		unsigned long	SDXCLK18OE	:1;			// SD_XCLK18OE_O Control
		unsigned long	SDDAT		:4;			// SD_DAT_O Control
		unsigned long	SDCMD		:1;			// SD_CMD_O Control
		unsigned long	IOSW		:1;			// IO Switch
		unsigned long				:4;			// reserved
	} bit;
};

/* structure of IOControl2	(1B00_0114h/1B01_0114h/1B02_0114h)(32bit only) */
union io_sdxc_iocontrol2 {
	unsigned long		word;
	struct {
		unsigned long	XDATOEC		:4;			// SD_DATOEC_O Control
		unsigned long				:4;			// reserved
		unsigned long	DATC		:4;			// SD_DAT_O Control
		unsigned long				:4;			// reserved
		unsigned long	XCMDOC		:1;			// SD_XCMDOE_O Control
		unsigned long	CMDC		:1;			// SD_CMD_O    Control
		unsigned long	MSELC		:1;			// SD_MSEL_O   Control
		unsigned long	CRESC		:1;			// SD_CRES_O   Control
		unsigned long				:12;		// reserved
	} bit;
};

/* structure of PowerSwitchinInterruptStatus	(1B00_0118h/1B01_0118h/1B02_0118h)(32bit only) */
union io_sdxc_pwswintstatus {
	unsigned long		word;
	struct {
		unsigned long	WINT5MS		:1;			// 5ms Wait Interrupt
		unsigned long	WINT1MS		:1;			// 1ms Wait Interrupt
		unsigned long				:30;		// reserved
	} bit;
};

/* structure of PowerSwitchinInterruptStatusEnable	(1B00_011Ch/1B01_011Ch/1B02_011Ch)(32bit only) */
union io_sdxc_pwswintstatusenable {
	unsigned long		word;
	struct {
		unsigned long	WINT5MSSTE	:1;			// 5ms Wait Interrupt Status Enable
		unsigned long	WINT1MSSTE	:1;			// 1ms Wait Interrupt Status Enable
		unsigned long				:30;		// reserved
	} bit;
};

/* structure of PowerSwitchinInterruptSignalEnable	(1B00_0120h/1B01_0120h/1B02_0120h)(32bit only) */
union io_sdxc_pwswintsignalenable {
	unsigned long		word;
	struct {
		unsigned long	WINT5MSSE	:1;			// 5ms Wait Interrupt Signal Enable
		unsigned long	WINT1MSSE	:1;			// 1ms Wait Interrupt Signal Enable
		unsigned long				:30;		// reserved
	} bit;
};

/* structure of MMC/eSDControl	(1B00_0124h/1B01_0124h/1B02_0124h)(32bit only) */
union io_sdxc_mmcesdcontrol {
	unsigned long		word;
	struct {
		unsigned long	LKRSTCESD	:1;			// Lock-Reset Control for eSD
		unsigned long	RSTCMMC		:1;			// RST_n Control for MMC
		unsigned long	VCCCMMC		:1;			// VCC   Control for MMC
		unsigned long	VCCQCMMC	:1;			// VCCQ  Control for MMC
		unsigned long				:4;			// reserved
		unsigned long	MDSEL		:1;			// MMC DDR Select
		unsigned long	CDDSEL		:1;			// CMD/DATA Delay Select
		unsigned long				:6;			// reserved
		unsigned long	BAEMMC		:1;			// Boot Ack Enable for MMC
		unsigned long	BAAEMMC		:1;			// Boot Auto Abort Enable for MMC
		unsigned long	BMEMMC		:1;			// Boot Mode Enable for MMC
		unsigned long				:13;		// reserved
	} bit;
};

/* structure of MMCWaitIRQControl1	(1B00_0128h/1B01_0128h/1B02_0128h)(32bit only) */
union io_sdxc_mmcwaitirqcontrol1 {
	unsigned long		word;
	struct {
		unsigned long	WIEN		:1;			// Wait IRQ Enable
		unsigned long	WIST		:1;			// Wait IRQ State
		unsigned long				:14;		// reserved
		unsigned long	WICR		:16;		// Wait IRQ Cancel Response
	} bit;
};

/* structure of MMCWaitIRQControl2	(1B00_012Ch/1B01_012Ch/1B02_012Ch)(32bit only) */

/* structure of MMCResponseCheckBit	(1B00_0130h/1B01_0130h/1B02_0130h)(32bit only) */
union io_sdxc_mmcresponsecheckbit {
	unsigned long		word;
	struct {
		unsigned long	CHKBIT1		:7;			// Check Bit 1
		unsigned long	CHKBIT2		:6;			// Check Bit 2
		unsigned long				:19;		// reserved
	} bit;
};

/* structure of CardDetectSetting	(1B00_0154h/1B01_0154h/1B02_0154h)(32bit only) */
union io_sdxc_carddetectsetting {
	unsigned long		word;
	struct {
		unsigned long				:8;			// reserved
		unsigned long	CDDTCV		:4;			// Card Detect Debounce Timer Counter Value
		unsigned long				:20;		// reserved
	} bit;
};

/* structure of BCLKSELControl	(1B02_01E0h)(32bit only)(ch3 only) */
union io_sdxc_bclkselcontrol {
	unsigned long		word;
	struct {
		unsigned long	SDBCS		:1;			// SD Base Clock Stop
		unsigned long				:7;			// reserved
		unsigned long	BCLKSEL		:1;			// Base clock selector
		unsigned long	BCLKDIV		:2;			// Base clock devider
		unsigned long				:5;			// reserved
		unsigned long	REGSEL		:1;			// F_SDH30 register are selector
		unsigned long				:15;		// reserved
	} bit;
};

/* structure of UHS1IOCSPortControl	(1B02_01E4h)(32bit only)(ch3 only) */
union io_sdxc_uhs1iocscontrol {
	unsigned long		word;
	struct {
		unsigned long	CLKCS		:1;			// Clock CS
		unsigned long				:7;			// reserved
		unsigned long	CMDCS		:1;			// Command CS
		unsigned long				:7;			// reserved
		unsigned long	DATCS		:8;			// Data CS
		unsigned long				:8;			// reserved
	} bit;
};

/* structure of PLLCounterValueSetting1	(1B02_01E8h)(32bit only)(ch3 only) */
union io_sdxc_pllcntvalsetting1 {
	unsigned long		word;
	struct {
		unsigned long	PLLOFFTIMVAL :8;		// PLL Off Timer Count
		unsigned long				 :8;		// reserved
		unsigned long	PLLLOCKTIMVAL:16;		// PLL Lock Timer Count
	} bit;
};

/* structure of SOFT_RESET	(1B00_0200h/1B01_0200h)(32bit only)(ch2/4 only) */
union io_sdxc_softreset0 {
	unsigned long		word;
	struct {
		unsigned long	SR_RSTX		:1;			// SD Core IP Reset
		unsigned long				:31;		// reserved
	} bit;
};

/* structure of SOFT_RESET	(1B02_0200h)(32bit only)(ch3 only) */
union io_sdxc_softreset1 {
	unsigned long		word;
	struct {
		unsigned long	SR_RST1X	:1;			// SD Core IP Reset(F_SDH30)
		unsigned long				:15;		// reserved
		unsigned long	SR_RST2X	:1;			// SD Core IP Reset(F_SDH40)
		unsigned long				:15;		// reserved
	} bit;
};

/* structure of HDCLK_SEL	(1B00_0204h/1B01_0204h/1B02_0204h)(32bit only) */
union io_sdxc_hdclksel {
	unsigned long		word;
	struct {
		unsigned long	HLDCLK_SEL0	:1;			// SD_HLDCLK_I Clock Select
		unsigned long	HLDCLK_SEL1	:1;			// SD_HLDCLK_I Polarity Select
		unsigned long				:6;			// reserved
		unsigned long	CMDHOLD_SEL	:4;			// CMD HOLD Select
		unsigned long				:4;			// reserved
		unsigned long	DATHOLD_SEL	:4;			// DATA HOLD Select
		unsigned long				:4;			// reserved
		unsigned long	SDCLKI_SEL	:1;			// SD_CLK_I Select
		unsigned long				:7;			// reserved
	} bit;
};

/* structure of HPROT_SET	(1B00_0208h/1B01_0208h/1B02_0208h)(32bit only) */
union io_sdxc_hprotset {
	unsigned long		word;
	struct {
		unsigned long	HPROT		:4;			// HPROT Set
		unsigned long				:28;		// reserved
	} bit;
};

/* structure of CLK_STOP	(1B00_020Ch/1B01_020Ch)(32bit only)(ch2/4 only) */
union io_sdxc_clkstop0 {
	unsigned long		word;
	struct {
		unsigned long	HCLK_STP	:1;			// A_HCLK_I    Stop
		unsigned long	HLDCLK_STP	:1;			// SD_HLDCLK_I Stop
		unsigned long	SDBCLK_STP	:1;			// SD_BCLK_I   Stop
		unsigned long	SD4CLK_STP	:1;			// SD_SD4CLK_I Stop
		unsigned long	CDCLK_STP	:1;			// SD_CDCLK_I  Stop
		unsigned long	TOCLK_STP	:1;			// SD_TOCLK_I  Stop
		unsigned long				:26;		// reserved
	} bit;
};

/* structure of CLK_STOP	(1B02_020Ch)(32bit only)(ch3 only) */
union io_sdxc_clkstop1 {
	unsigned long		word;
	struct {
		unsigned long	HCLK_STP	:1;			// A_HCLK_I    Stop
		unsigned long				:1;			// reserved
		unsigned long	SDBCLK_STP	:1;			// SD_BCLK_I   Stop
		unsigned long				:1;			// reserved
		unsigned long	CDCLK_STP	:1;			// SD_CDCLK_I  Stop
		unsigned long	TOCLK_STP	:1;			// SD_TOCLK_I  Stop
		unsigned long				:26;		// reserved
	} bit;
};

/* structure of WP_CD_LED_SET	(1B00_0210h/1B01_0210h/1B02_0210h)(32bit only) */
union io_sdxc_wpcdledset {
	unsigned long		word;
	struct {
		unsigned long	CD_INV		:1;			// CD  Signal Polarity Inverse
		unsigned long	WP_INV		:1;			// WP  Signal Polarity Inverse
		unsigned long	LED_INV		:1;			// LED Signal Polarity Inverse
		unsigned long				:5;			// reserved
		unsigned long	CD_VAL		:1;			// CD Signal Set
		unsigned long	WP_VAL		:1;			// WP Signal Set
		unsigned long				:6;			// reserved
		unsigned long	CD_SEL		:1;			// CD Signal Select
		unsigned long	WP_SEL		:1;			// WP Signal Select
		unsigned long				:14;		// reserved
	} bit;
};

/* structure of RE-TUNING_REQ	(1B00_0214h/1B01_0214h/1B02_0214h)(32bit only) */
union io_sdxc_retuningreq {
	unsigned long		word;
	struct {
		unsigned long	RT_REQ		:1;			// Re-Tuning Request
		unsigned long				:31;		// reserved
	} bit;
};

/* structure of RE-TUNING_REQ_CLR	(1B00_0218h/1B01_0218h/1B02_0218h)(32bit only) */
union io_sdxc_retuningreqclr {
	unsigned long		word;
	struct {
		unsigned long	RT_REQ_CLR	:1;			// Re-Tuning Request Clear
		unsigned long				:31;		// reserved
	} bit;
};

/* structure of CR_SET	(1B00_0220h/1B01_0220h/1B02_0220h)(32bit only) */
union io_sdxc_crset {
	unsigned long		word;
	struct {
		unsigned long	CR_RTUNMODE		:2;		// Re-Tuning Mode(Capabilities Reg) Set
		unsigned long					:2;		// reserved
		unsigned long	CD_RTUNTIMER	:4;		// Timer Count for Re-Tuning(Capabilities Reg) Set
		unsigned long	CR_BCLKFREQ		:8;		// Base Clock Frequency For SD Clock(Capabilities Reg) Set
		unsigned long	CR_TOCLKFREQ	:6;		// Timeout Clock Frequency(Capabilities Reg) Set
		unsigned long					:2;		// reserved
		unsigned long	CR_TOCLKUNIT	:1;		// Timeout Clock Unit(Capabilities Reg) Set
		unsigned long					:3;		// reserved
		unsigned long	CR_SLOTTYPE0	:1;		// Slot Type bit0(Capabilities Reg) Set
		unsigned long					:3;		// reserved
	} bit;
};

/* structure of CR_SET2	(1B02_0224h)(32bit only)(ch3 only) */
union io_sdxc_crset2 {
	unsigned long		word;
	struct {
		unsigned long	CR_UHS2NFCU		:8;		// Host N_FCU
		unsigned long	CR_UHS2MAXBLEN	:12;	// Host Maximum Block Length
		unsigned long	CR_UHS2DAP		:4;		// DAT
		unsigned long	CR_UHS2GAP		:4;		// GAP
		unsigned long	CR_18VDD2SUP	:1;		// 1.8V VDD2 Support
		unsigned long					:3;		// reserved
	} bit;
};

/* structure of CDR_SET	(1B00_0230h/1B01_0230h/1B02_0230h)(32bit only) */
union io_sdxc_cdrset {
	unsigned long		word;
	struct {
		unsigned long	CDR_DTIMEC		:4;		// Card Detect Debounce Time
		unsigned long					:28;	// reserved
	} bit;
};

/* structure of PV_SET1	(1B00_0240h/1B01_0240h/1B02_0240h)(32bit only) */
union io_sdxc_pvset1 {
	unsigned long		word;
	struct {
		unsigned long	PV_FREQINI		:10;	// Preset Value (SDCLK Frequency Select Value for Initialize) Set
		unsigned long					:6;		// reserved
		unsigned long	PV_FREQDS		:10;	// Preset Value (SDCLK Frequency Select Value for Default Speed) Set
		unsigned long					:6;		// reserved
	} bit;
};

/* structure of PV_SET2	(1B00_0244h/1B01_0244h/1B02_0244h)(32bit only) */
union io_sdxc_pvset2 {
	unsigned long		word;
	struct {
		unsigned long	PV_FREQHS		:10;	// Preset Value (SDCLK Frequency Select Value for High Speed) Set
		unsigned long					:6;		// reserved
		unsigned long	PV_FREQSDR12	:10;	// Preset Value (SDCLK Frequency Select Value for SDR12) Set
		unsigned long					:6;		// reserved
	} bit;
};

/* structure of PV_SET3	(1B00_0248h/1B01_0248h/1B02_0248h)(32bit only) */
union io_sdxc_pvset3 {
	unsigned long		word;
	struct {
		unsigned long	PV_FREQSDR25	:10;	// Preset Value (SDCLK Frequency Select Value for SDR25) Set
		unsigned long					:6;		// reserved
		unsigned long	PV_FREQSDR50	:10;	// Preset Value (SDCLK Frequency Select Value for SDR50) Set
		unsigned long					:6;		// reserved

	} bit;
};

/* structure of PV_SET4	(1B00_024Ch/1B01_024Ch/1B02_024Ch)(32bit only) */
union io_sdxc_pvset4 {
	unsigned long		word;
	struct {
		unsigned long	PV_FREQSDR104	:10;	// Preset Value (SDCLK Frequency Select Value for SDR104) Set
		unsigned long					:6;		// reserved
		unsigned long	PV_FREQDDR50	:10;	// Preset Value (SDCLK Frequency Select Value for DDR50) Set
		unsigned long					:6;		// reserved
	} bit;
};

/* structure of PV_SET5	(1B02_0250h)(32bit only)(ch3 only) */
union io_sdxc_pvset5 {
	unsigned long		word;
	struct {
		unsigned long	PV_AM2PBL		:4;		// Bus Setting Register"PBL"
		unsigned long					:4;		// reserved
		unsigned long	PV_DTIMEC		:24;	// Bus Setting Register"DP"
	} bit;
};



struct io_sdxc0 {
	unsigned long						SYSADDR;							/* 1B00_(0000 - 0003h) / 1B01_(0000 - 0003h) */
	union io_sdxc_blkinfo				BLKINFO;							/* 1B00_(0004 - 0007h) / 1B01_(0004 - 0007h) */
	unsigned long						ARG1_0;								/* 1B00_(0008 - 000Bh) / 1B01_(0008 - 000Bh) */
	union io_sdxc_trnsinfo				TRNSINFO;							/* 1B00_(000C - 000Fh) / 1B01_(000C - 000Fh) */
	unsigned long						RESP1_0;							/* 1B00_(0010 - 0013h) / 1B01_(0010 - 0013h) */
	unsigned long						RESP3_2;							/* 1B00_(0014 - 0017h) / 1B01_(0014 - 0017h) */
	unsigned long						RESP5_4;							/* 1B00_(0018 - 001Bh) / 1B01_(0018 - 001Bh) */
	unsigned long						RESP7_6;							/* 1B00_(001C - 001Fh) / 1B01_(001C - 001Fh) */
	unsigned long						BUFDAT;								/* 1B00_(0020 - 0023h) / 1B01_(0020 - 0023h) */
	union io_sdxc_presentstate			PREST;								/* 1B00_(0024 - 0027h) / 1B01_(0024 - 0027h) */
	union io_sdxc_ctrl1					CTRL1;								/* 1B00_(0028 - 002Bh) / 1B01_(0028 - 002Bh) */
	union io_sdxc_ctrl3					CTRL3;								/* 1B00_(002C - 002Fh) / 1B01_(002C - 002Fh) */
	union io_sdxc_intst					INTST;								/* 1B00_(0030 - 0033h) / 1B01_(0030 - 0033h) */
	union io_sdxc_intstenb				INTSTENB;							/* 1B00_(0034 - 0037h) / 1B01_(0034 - 0037h) */
	union io_sdxc_intsigenb				INTSIGENB;							/* 1B00_(0038 - 003Bh) / 1B01_(0038 - 003Bh) */
	union io_sdxc_ctrl2					CTRL2;								/* 1B00_(003C - 003Fh) / 1B01_(003C - 003Fh) */
	union io_sdxc_cap1_0				CAP1_0;								/* 1B00_(0040 - 0043h) / 1B01_(0040 - 0043h) */
	union io_sdxc_cap3_2				CAP3_2;								/* 1B00_(0044 - 0047h) / 1B01_(0044 - 0047h) */
	union io_sdxc_mcurcap1_0			MCURCAP1_0;							/* 1B00_(0048 - 004Bh) / 1B01_(0048 - 004Bh) */
	unsigned long						MCURCAP3_2;							/* 1B00_(004C - 004Fh) / 1B01_(004C - 004Fh) */
	union io_sdxc_forevt				FOREVT;								/* 1B00_(0050 - 0053h) / 1B01_(0050 - 0053h) */
	union io_sdxc_admaest				ADMAEST;							/* 1B00_(0054 - 0057h) / 1B01_(0054 - 0057h) */
	unsigned long						ADMASYSADDR_L;						/* 1B00_(0058 - 005Bh) / 1B01_(0058 - 005Bh) */
	unsigned long						ADMASYSADDR_U;						/* 1B00_(005C - 005Fh) / 1B01_(005C - 005Fh) */
	union io_sdxc_preval1				PREVAL1;							/* 1B00_(0060 - 0063h) / 1B01_(0060 - 0063h) */
	union io_sdxc_preval2				PREVAL2;							/* 1B00_(0064 - 0067h) / 1B01_(0064 - 0067h) */
	union io_sdxc_preval3				PREVAL3;							/* 1B00_(0068 - 006Bh) / 1B01_(0068 - 006Bh) */
	union io_sdxc_preval4				PREVAL4;							/* 1B00_(006C - 006Fh) / 1B01_(006C - 006Fh) */
	unsigned char						dmy_sdio_070_0DF[0x00E0-0x0070];	/* 1B00_(0070 - 00DFh) / 1B01_(0070 - 00DFh) */
	union io_sdxc_sharedbuscontrol		SHRBUSCTRL;							/* 1B00_(00E0 - 00E3h) / 1B01_(00E0 - 00E3h) */
	unsigned char						dmy_sdio_0E4_0FB[0x00FC-0x00E4];	/* 1B00_(00E4 - 00FBh) / 1B01_(00E4 - 00FBh) */
	union io_sdxc_hctrlver				HCTRLVER;							/* 1B00_(00FC - 00FFh) / 1B01_(00FC - 00FFh) */
	union io_sdxc_ahbconfig				AHBCNF;								/* 1B00_(0100 - 0103h) / 1B01_(0100 - 0103h) */
	union io_sdxc_powerswitching		POWSW;								/* 1B00_(0104 - 0107h) / 1B01_(0104 - 0107h) */
	union io_sdxc_tuningsetting			TUNSET;								/* 1B00_(0108 - 010Bh) / 1B01_(0108 - 010Bh) */
	union io_sdxc_tuningstate			TUNST;								/* 1B00_(010C - 010Fh) / 1B01_(010C - 010Fh) */
	union io_sdxc_iocontrol1			IOCTRL1;							/* 1B00_(0110 - 0113h) / 1B01_(0110 - 0113h) */
	union io_sdxc_iocontrol2			IOCTRL2;							/* 1B00_(0114 - 0117h) / 1B01_(0114 - 0117h) */
	union io_sdxc_pwswintstatus			POWSWST;							/* 1B00_(0118 - 011Bh) / 1B01_(0118 - 011Bh) */
	union io_sdxc_pwswintstatusenable	POWSWSTENB;							/* 1B00_(011C - 011Fh) / 1B01_(011C - 011Fh) */
	union io_sdxc_pwswintsignalenable	POWSWSIGENB;						/* 1B00_(0120 - 0123h) / 1B01_(0120 - 0123h) */
	union io_sdxc_mmcesdcontrol			MMC_ESDCTRL;						/* 1B00_(0124 - 0127h) / 1B01_(0124 - 0127h) */
	union io_sdxc_mmcwaitirqcontrol1	MMC_WAIT_IRQ_CTRL1;					/* 1B00_(0128 - 012Bh) / 1B01_(0128 - 012Bh) */
	unsigned long						MMC_WAIT_IRQ_CTRL2;					/* 1B00_(012C - 012Fh) / 1B01_(012C - 012Fh) */
	union io_sdxc_mmcresponsecheckbit	MMC_RES_CHK;						/* 1B00_(0130 - 0133h) / 1B01_(0130 - 0133h) */
	unsigned char						dmy_sdio_134_153[0x0154-0x0134];	/* 1B00_(0134 - 0153h) / 1B01_(0134 - 0153h) */
	union io_sdxc_carddetectsetting		CDSET;								/* 1B00_(0154 - 0157h) / 1B01_(0154 - 0157h) */
	unsigned char						dmy_sdio_158_1FF[0x0200-0x0158];	/* 1B00_(0158 - 01FFh) / 1B01_(0158 - 01FFh) */
	union io_sdxc_softreset0			SOFT_RESET;							/* 1B00_(0200 - 0203h) / 1B01_(0200 - 0203h) */
	union io_sdxc_hdclksel				HDCLK_SEL;							/* 1B00_(0204 - 0207h) / 1B01_(0204 - 0207h) */
	union io_sdxc_hprotset				HPORT_SET;							/* 1B00_(0208 - 020Bh) / 1B01_(0208 - 020Bh) */
	union io_sdxc_clkstop0				CLK_STOP;							/* 1B00_(020C - 020Fh) / 1B01_(020C - 020Fh) */
	union io_sdxc_wpcdledset			WP_CD_LED_SET;						/* 1B00_(0210 - 0213h) / 1B01_(0210 - 0213h) */
	union io_sdxc_retuningreq			RETUNING_REQ;						/* 1B00_(0214 - 0217h) / 1B01_(0214 - 0217h) */
	union io_sdxc_retuningreqclr		RETUNING_REQ_CLR;					/* 1B00_(0218 - 021Bh) / 1B01_(0218 - 021Bh) */
	unsigned char						dmy_sdio_21C_21F[0x0220-0x021C];	/* 1B00_(021C - 021Fh) / 1B01_(021C - 021Fh) */
	union io_sdxc_crset					CR_SET;								/* 1B00_(0220 - 0223h) / 1B01_(0220 - 0223h) */
	unsigned char						dmy_sdio_224_22F[0x0230-0x0224];	/* 1B00_(0224 - 022Fh) / 1B01_(0224 - 022Fh) */
	union io_sdxc_cdrset				CDR_SET;							/* 1B00_(0230 - 0233h) / 1B01_(0230 - 0233h) */
	unsigned char						dmy_sdio_234_23F[0x0240-0x0234];	/* 1B00_(0234 - 023Fh) / 1B01_(0234 - 023Fh) */
	union io_sdxc_pvset1				PV_SET1;							/* 1B00_(0240 - 0243h) / 1B01_(0240 - 0243h) */
	union io_sdxc_pvset2				PV_SET2;							/* 1B00_(0244 - 0247h) / 1B01_(0244 - 0247h) */
	union io_sdxc_pvset3				PV_SET3;							/* 1B00_(0248 - 024Bh) / 1B01_(0248 - 024Bh) */
	union io_sdxc_pvset4				PV_SET4;							/* 1B00_(024C - 024Fh) / 1B01_(024C - 024Fh) */
};

struct io_sdxc1 {
	unsigned long						SYSADDR;							/* 1B02_(0000 - 0003h) */
	union io_sdxc_blkinfo				BLKINFO;							/* 1B02_(0004 - 0007h) */
	unsigned long						ARG1_0;								/* 1B02_(0008 - 000Bh) */
	union io_sdxc_trnsinfo				TRNSINFO;							/* 1B02_(000C - 000Fh) */
	unsigned long						RESP1_0;							/* 1B02_(0010 - 0013h) */
	unsigned long						RESP3_2;							/* 1B02_(0014 - 0017h) */
	unsigned long						RESP5_4;							/* 1B02_(0018 - 001Bh) */
	unsigned long						RESP7_6;							/* 1B02_(001C - 001Fh) */
	unsigned long						BUFDAT;								/* 1B02_(0020 - 0023h) */
	union io_sdxc_presentstate			PREST;								/* 1B02_(0024 - 0027h) */
	union io_sdxc_ctrl1					CTRL1;								/* 1B02_(0028 - 002Bh) */
	union io_sdxc_ctrl3					CTRL3;								/* 1B02_(002C - 002Fh) */
	union io_sdxc_intst					INTST;								/* 1B02_(0030 - 0033h) */
	union io_sdxc_intstenb				INTSTENB;							/* 1B02_(0034 - 0037h) */
	union io_sdxc_intsigenb				INTSIGENB;							/* 1B02_(0038 - 003Bh) */
	union io_sdxc_ctrl2					CTRL2;								/* 1B02_(003C - 003Fh) */
	union io_sdxc_cap1_0				CAP1_0;								/* 1B02_(0040 - 0043h) */
	union io_sdxc_cap3_2				CAP3_2;								/* 1B02_(0044 - 0047h) */
	union io_sdxc_mcurcap1_0			MCURCAP1_0;							/* 1B02_(0048 - 004Bh) */
	unsigned long						MCURCAP3_2;							/* 1B02_(004C - 004Fh) */
	union io_sdxc_forevt				FOREVT;								/* 1B02_(0050 - 0053h) */
	union io_sdxc_admaest				ADMAEST;							/* 1B02_(0054 - 0057h) */
	unsigned long						ADMASYSADDR_L;						/* 1B02_(0058 - 005Bh) */
	unsigned long						ADMASYSADDR_U;						/* 1B02_(005C - 005Fh) */
	union io_sdxc_preval1				PREVAL1;							/* 1B02_(0060 - 0063h) */
	union io_sdxc_preval2				PREVAL2;							/* 1B02_(0064 - 0067h) */
	union io_sdxc_preval3				PREVAL3;							/* 1B02_(0068 - 006Bh) */
	union io_sdxc_preval4				PREVAL4;							/* 1B02_(006C - 006Fh) */
	unsigned char						dmy_sdio_070_0DF[0x00E0-0x0070];	/* 1B02_(0070 - 00DFh) */
	union io_sdxc_sharedbuscontrol		SHRBUSCTRL;							/* 1B02_(00E0 - 00E3h) */
	unsigned char						dmy_sdio_0E4_0FB[0x00FC-0x00E4];	/* 1B02_(00E4 - 00FBh) */
	union io_sdxc_hctrlver				HCTRLVER;							/* 1B02_(00FC - 00FFh) */
	union io_sdxc_ahbconfig				AHBCNF;								/* 1B02_(0100 - 0103h) */
	union io_sdxc_powerswitching		POWSW;								/* 1B02_(0104 - 0107h) */
	union io_sdxc_tuningsetting			TUNSET;								/* 1B02_(0108 - 010Bh) */
	union io_sdxc_tuningstate			TUNST;								/* 1B02_(010C - 010Fh) */
	union io_sdxc_iocontrol1			IOCTRL1;							/* 1B02_(0110 - 0113h) */
	union io_sdxc_iocontrol2			IOCTRL2;							/* 1B02_(0114 - 0117h) */
	union io_sdxc_pwswintstatus			POWSWST;							/* 1B02_(0118 - 011Bh) */
	union io_sdxc_pwswintstatusenable	POWSWSTENB;							/* 1B02_(011C - 011Fh) */
	union io_sdxc_pwswintsignalenable	POWSWSIGENB;						/* 1B02_(0120 - 0123h) */
	union io_sdxc_mmcesdcontrol			MMC_ESDCTRL;						/* 1B02_(0124 - 0127h) */
	union io_sdxc_mmcwaitirqcontrol1	MMC_WAIT_IRQ_CTRL1;					/* 1B02_(0128 - 012Bh) */
	unsigned long						MMC_WAIT_IRQ_CTRL2;					/* 1B02_(012C - 012Fh) */
	union io_sdxc_mmcresponsecheckbit	MMC_RES_CHK;						/* 1B02_(0130 - 0133h) */
	unsigned char						dmy_sdio_134_153[0x0154-0x0134];	/* 1B02_(0134 - 0153h) */
	union io_sdxc_carddetectsetting		CDSET;								/* 1B02_(0154 - 0157h) */
	unsigned char						dmy_sdio_158_1DF[0x01E0-0x0158];	/* 1B02_(0158 - 01DFh) */
	union io_sdxc_bclkselcontrol		BCLKSELCTRL;						/* 1B02_(01E0 - 01E3h) */
	union io_sdxc_uhs1iocscontrol		UHS1IOCS_PORTCTRL;					/* 1B02_(01E4 - 01E7h) */
	union io_sdxc_pllcntvalsetting1		PLLCNTVALSET1;						/* 1B02_(01E8 - 01EBh) */
	unsigned char						dmy_sdio_1EC_1FF[0x0200-0x01EC];	/* 1B02_(01EC - 01FFh) */
	union io_sdxc_softreset1			SOFT_RESET;							/* 1B02_(0200 - 0203h) */
	union io_sdxc_hdclksel				HDCLK_SEL;							/* 1B02_(0204 - 0207h) */
	union io_sdxc_hprotset				HPORT_SET;							/* 1B02_(0208 - 020Bh) */
	union io_sdxc_clkstop1				CLK_STOP;							/* 1B02_(020C - 020Fh) */
	union io_sdxc_wpcdledset			WP_CD_LED_SET;						/* 1B02_(0210 - 0213h) */
	union io_sdxc_retuningreq			RETUNING_REQ;						/* 1B02_(0214 - 0217h) */
	union io_sdxc_retuningreqclr		RETUNING_REQ_CLR;					/* 1B02_(0218 - 021Bh) */
	unsigned char						dmy_sdio_21C_21F[0x0220-0x021C];	/* 1B02_(021C - 021Fh) */
	union io_sdxc_crset					CR_SET;								/* 1B02_(0220 - 0223h) */
	union io_sdxc_crset2				CR_SET2;							/* 1B02_(0224 - 0227h) */
	unsigned char						dmy_sdio_228_22F[0x0230-0x0228];	/* 1B02_(0228 - 022Fh) */
	union io_sdxc_cdrset				CDR_SET;							/* 1B02_(0230 - 0233h) */
	unsigned char						dmy_sdio_234_23F[0x0240-0x0234];	/* 1B02_(0234 - 023Fh) */
	union io_sdxc_pvset1				PV_SET1;							/* 1B02_(0240 - 0243h) */
	union io_sdxc_pvset2				PV_SET2;							/* 1B02_(0244 - 0247h) */
	union io_sdxc_pvset3				PV_SET3;							/* 1B02_(0248 - 024Bh) */
	union io_sdxc_pvset4				PV_SET4;							/* 1B02_(024C - 024Fh) */
	union io_sdxc_pvset5				PV_SET5;							/* 1B02_(0250 - 0253h) */
};

extern volatile struct io_sdxc0 IO_SDXC2;		/* 1B00_0000 */
extern volatile struct io_sdxc0 IO_SDXC4;		/* 1B01_0000 */
extern volatile struct io_sdxc1 IO_SDXC31;		/* 1B02_0000 */
