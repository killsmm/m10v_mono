/**
 * @file		_sd40h.h
 * @brief		Definition JMLEXS_SD40H Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/* structure of DebounceSetting  		(1913_0004h)(32bit only) */
union io_sdxc_hrs01 {
	unsigned long		word;
	struct {
		unsigned long	DP			:24;		// Debounce Period
		unsigned long				:8;			// reserved
    } bit;
};

/* structure of BusSetting				(1913_0008h)(32bit only) */
union io_sdxc_hrs02 {
	unsigned long		word;
	struct {
		unsigned long	PBL			:4;			// Programmable Burst Length
		unsigned long				:12;		// reserved
		unsigned long	OTN			:2;			// Number of Outstanding Transfers
		unsigned long				:14;		// reserved
    } bit;
};

/* structure of AXIErrorInterrupt 		(1913_000Ch)(32bit only) */
union io_sdxc_hrs03 {
	unsigned long		word;
	struct {
		unsigned long	__AERRD		:1;			// AXI ERROR Response R channel: DECERR
		unsigned long	__AERRS		:1;			// AXI ERROR Response R channel: SLVERR
		unsigned long	__AERBD		:1;			// AXI ERROR Response B channel: DECERR
		unsigned long	__AERBS		:1;			// AXI ERROR Response B channel: SLVERR
		unsigned long				:4;			// reserved
		unsigned long	__AERSENRD	:1;			// Status Enable for AXI ERROR Response R channel: DECERR
		unsigned long	__AERSENRS	:1;			// Status Enable for AXI ERROR Response R channel: SLVERR
		unsigned long	__AERSENBD	:1;			// Status Enable for AXI ERROR Response B channel: DECERR
		unsigned long	__AERSENBS	:1;			// Status Enable for AXI ERROR Response B channel: SLVERR
		unsigned long				:4;			// reserved
		unsigned long	__AERIERD	:1;			// Signal Enable for AXI ERROR Response R channel: DECERR
		unsigned long	__AERIERS	:1;			// Signal Enable for AXI ERROR Response R channel: SLVERR
		unsigned long	__AERIEBD	:1;			// Signal Enable for AXI ERROR Response B channel: DECERR
		unsigned long	__AERIEBS	:1;			// Signal Enable for AXI ERROR Response B channel: SLVERR
		unsigned long				:12;		// reserved
    } bit;
};

/* structure of LINK-PHY I/F signals  	(1913_0060h)(32bit only) */
union io_sdxc_hrs32 {
	unsigned long		word;
	struct {
		unsigned long	DET			:1;			// ST[0] signal
		unsigned long	LOCK		:1;			// ST[1] signal
		unsigned long	PACK		:1;			// ST[2] signal
		unsigned long	ERR			:1;			// ST[3] signal
		unsigned long	RDS			:2;			// ST[5:4] signal
		unsigned long	RDTS		:2;			// ST[7:6] signal
		unsigned long				:4;			// reserved
		unsigned long				:4;			// reserved
		unsigned long				:5;			// reserved
		unsigned long				:2;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:8;			// reserved
    } bit;
};

/* structure of PHY PerformanceAdjustmentSignalsStatus2   (1913_0068h)(32bit only) */
union io_sdxc_hrs34 {
	unsigned long		word;
	struct {
		unsigned long	TMD1		:8;			// O_TMD1 Signal
		unsigned long	TMD2		:8;			// O_TMD2 Signal
		unsigned long	TMACTIVE	:1;			// O_TMACTIVE Signal
		unsigned long				:4;			// reserved
		unsigned long	RTT			:11;		// O_RTT Signal
    } bit;
};

/* structure of AXI MasterOutputControl	(1913_0074h)(32bit only) */
union io_sdxc_hrs37 {
	unsigned long		word;
	struct {
		unsigned long	AWCACHE		:4;			// AM2_AWCACHE_O Signal
		unsigned long				:4;			// reserved
		unsigned long	AWPROT		:3;			// AM2_AWPROT_O Signal
		unsigned long				:5;			// reserved
		unsigned long	ARCACHE		:4;			// AM2_ARCACHE_O Signal
		unsigned long				:4;			// reserved
		unsigned long	ARPROT		:3;			// AM2_ARPROT_O Signal
		unsigned long				:5;			// reserved
    } bit;
};

/* structure of UHS-II I/O Control		(1913_0078h)(32bit only) */
union io_sdxc_hrs38 {
	unsigned long		word;
	struct {
		unsigned long	UHS2PC		:1;			// Cut off pull-up resister in the I/Os for DAT0 line and DAT1 line
		unsigned long				:7;			// reserved
		unsigned long	UHS2CS2		:1;			// Force CS port in the I/O for DAT2 line
		unsigned long				:23;		// reserved
    } bit;
};

/* structure of UHS-II BCLKSEL Status	(1913_007Ch)(32bit only) */
union io_sdxc_hrs39 {
	unsigned long		word;
	struct {
		unsigned long	SDBCLKSTP	:1;			// SD Base Clock Stopped Status
		unsigned long				:31;		// reserved
    } bit;
};

/* structure of PHY NRST Control	(1913_0090h)(32bit only) */
union io_sdxc_hrs44 {
	unsigned long		word;
	struct {
		unsigned long	PHYSRSTTIMVAL	:16;	// PHY Software Reset Period Timer Count
		unsigned long					:16;	// reserved
    } bit;
};

/* structure of UHS-II PHY Compliance Test Control	(1913_0094h)(32bit only) */
union io_sdxc_hrs45 {
	unsigned long		word;
	struct {
		unsigned long	CTSEL		:1;			// PHY-LINK I/F Signals Select for PHY compliance test
		unsigned long				:3;			// reserved
		unsigned long	CTNRST		:1;			// NRST for PHY compliance test
		unsigned long	CTMODE		:1;			// MODE for PHY compliace test
		unsigned long	RCLK2ENTST	:1;			// RCLK2EN for PHY comliance test
		unsigned long				:1;			// reserved
		unsigned long	CTCT		:8;			// CT for PHY compliance test
		unsigned long				:8;			// reserved
		unsigned long	SLVMODE		:1;			// Slave Mode for PHY compliance test
		unsigned long				:7;			// reserved
    } bit;
};

/* structure of UHS-II PHY Capabilities Setting	(1913_0098h)(32bit only) */
union io_sdxc_hrs46 {
	unsigned long		word;
	struct {
		unsigned long	NLSSSEL		:1;			// N_LSS_DIR/N_LSS_SYN Capabilities Select
		unsigned long				:7;			// reserved
		unsigned long	SFTNLSSSYN	:4;			// N_LSS_SYN Capability Soft Setting
		unsigned long	SFTNLSSDIR	:4;			// N_LSS_DIR Capability Soft Setting
		unsigned long				:16;		// reserved
    } bit;
};

/* structure of Generic					(1913_00ECh)(32bit only) */
union io_sdxc_hrs59 {
	unsigned long		word;
	struct {
		unsigned long	U2CRE		:1;			// UHS-II Command/Response Endian Select
		unsigned long				:31;		// reserved
    } bit;
};


/* structure of Host Controller Version / Slot Interrupt Status	(1913_00FCh)(32bit only) */
union io_sdxc_crs63 {
	unsigned long		word;
	struct {
		unsigned long							:16;	// reserved (16 bit)
		unsigned long	HostControllerVersion	:16;	// HostControllerVersion Register (16 bit)
	} hword;
	struct {
		unsigned long				:8;			// reserved
		unsigned long				:8;			// reserved
		unsigned long	SVN			:8;			// Specification Version Number
		unsigned long				:8;			// reserved
    } bit;
};


/* structure of PresentState			(1913_0224h)(32bit only) */
union io_sdxc_srs09 {
	unsigned long		word;
	struct {
		unsigned long	CIHCMD		:1;			// Command InHibit CMD
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:4;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long	BWE			:1;			// Buffer Write Enable
		unsigned long	BRE			:1;			// Buffer Read  Enable
		unsigned long				:4;			// reserved
		unsigned long	CI			:1;			// Card Inserted
		unsigned long	CSS			:1;			// Card State Stable
		unsigned long	CDPL		:1;			// Card Detect Pin Level
		unsigned long	WPSPL		:1;			// Write Protect Switch Pin Level
		unsigned long				:4;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:4;			// reserved
		unsigned long	U2DOR		:1;			// In Dormant State (UHS-II only)
		unsigned long	U2SYN		:1;			// Lane Synchronization (UHS-II only)
		unsigned long	U2DET		:1;			// STB.L Detection (UHS-II only)
    } bit;
};

/* structure of WakeupControl & BlockGapControl
	 	 & PowerControl & HostControl1	(1913_0228h)(32bit only) */
union io_sdxc_srs10 {
	unsigned long		word;
	struct {
		unsigned long	HostControl1	:8;		// HostControl1    Register (8 bit)
		unsigned long	PowerControl	:8;		// PowerControl    Register (8 bit)
		unsigned long	BlockGapControl	:8;		// BlockGapControl Register (8 bit)
		unsigned long	WakeupControl	:8;		// WakeupControl   Register (8 bit)
    } byte;
	struct {
		// HostControl1 Register bit
		unsigned long	LEDC		:1;			// LEDC Control
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long	DMASEL		:2;			// DMA Select
		unsigned long				:1;			// reserved
		unsigned long	CDTL		:1;			// Card Detect Test Level
		unsigned long	CDSS		:1;			// Card Detect Signal Selection
		// PowerControl Register bit
		unsigned long	SDBP		:1;			// SD Bus Power
		unsigned long	SDBVS		:3;			// SD Bus Voltage Select
		unsigned long	SDBP2		:1;			// SD Bus Power for VDD2 (UHS-II only)
		unsigned long	SDBVS2		:3;			// SD Bus Voltage Select for VDD2 (UHS-II only)
		// BlockGapControl Register bit
		unsigned long	SBGR		:1;			// Stop at Block Gap Request
		unsigned long	CR			:1;			// Continue Request
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:4;			// reserved
		// WakeuppControl Register bit
		unsigned long	WEESIQ		:1;			// Wakeup Event Enable On Card Interrupt
		unsigned long	WEESCI		:1;			// Wakeup Event Enable of SD Card Inserted
		unsigned long	WEESCR		:1;			// Wakeup Event Enable of SD Card Removal
		unsigned long				:5;			// reserved
    } bit;
};

/* structure of 
	SoftwareReset & TimeoutControl & ClockControl	(1913_022Ch)(32bit only) */
union io_sdxc_srs11 {
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
		unsigned long				:4;			// reserved
		unsigned long				:4;			// reserved
		// SoftReset
		unsigned long	SRA			:1;			// Software Reset for All
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:5;			// reserved
    } bit;
};

/* structure of 
	ErrorInterruptStatus & NormalInterruptStatus	(1913_0230h)(32bit only) */
union io_sdxc_srs12 {
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
		unsigned long	__CINT		:1;			// Card Interrupt
		unsigned long				:6;			// reserved
		unsigned long	__EI		:1;			// Error Interrupt
		// ErrorInterruptStatus
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long	__CLE		:1;			// Current Limit Error
		unsigned long				:1;			// reserved
		unsigned long	__ADMAE		:1;			// ADMA Error
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:4;			// reserved
    } bit;
};

/* structure of 
	ErrorInterruptStatusEnable & NormalInterruptStatusEnable	(1913_0234h)(32bit only) */
union io_sdxc_srs13 {
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
		unsigned long	CINSTE		:1;			// Card Interrupt Status Enable
		unsigned long				:7;			// reserved
		// ErrorInterruptStatusEnable
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long	CLESTE		:1;			// Current Limit Error Status Enable
		unsigned long				:1;			// reserved
		unsigned long	ADMAESTE	:1;			// ADMA Error Status Enable
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:4;			// reserved
    } bit;
};

/* structure of 
	ErrorInterruptSignalEnable & NormalInterruptSignalEnable	(1913_0238h)(32bit only) */
union io_sdxc_srs14 {
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
		unsigned long	CIRSE		:1;			// Card Interrupt Signal Enable
		unsigned long				:7;			// reserved
		// ErrorInterruptSignalEnable
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long	CLESE		:1;			// Current Limit Error Signal Enable
		unsigned long				:1;			// reserved
		unsigned long	ADMAESE		:1;			// ADMA Error Signal Enable
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:4;			// reserved
    } bit;
};

/* structure of HostControl2 & AutoCMD12errorStatus	(1913_023Ch)(32bit only) */
union io_sdxc_srs15 {
	unsigned long		word;
	struct {
		unsigned long	ACMD12EStatus	:16;	// AutoCMDErrorStatus Register (16 bit)
		unsigned long	HostControl2	:16;	// HostControl2       Register (16 bit)
	} hword;
	struct {
		// AutoCMD12ErrorStatus
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:8;			// reserved
		// HostControl2
		unsigned long	UHSMS		:3;			// UHS Mode Select
		unsigned long				:1;			// reserved
		unsigned long				:2;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long	U2IE		:1;			// UHS-II Interface Enable
		unsigned long				:3;			// reserved
		unsigned long	HV4E		:1;			// Host Version 4.00 Enable
		unsigned long	A64B		:1;			// 64-bit Addressing
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
    } bit;
};

/* structure of Capabilities1	(1913_0240h)(32bit only) */
union io_sdxc_srs16 {
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
		unsigned long	ADMA2		:1;			// ADMA2 Support
		unsigned long	ADMA1		:1;			// ADMA1 Support
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

/* structure of Capabilities2	(1913_0244h)(32bit only) */
union io_sdxc_srs17 {
	unsigned long		word;
	struct {
		unsigned long	Capa2		:16;		// Capabilities2 Register (16 bit)
		unsigned long	Capa3		:16;		// Capabilities3 Register (16 bit)
	} hword;
	struct {
		// Capabilities2
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long	UHSII		:1;			// UHS-II Supported
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:4;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:1;			// reserved
		unsigned long				:2;			// reserved
		// Capabilities3
		unsigned long	CM			:8;			// Clock Multiplier
		unsigned long				:4;			// reserved
		unsigned long	VDD2S		:1;			// VDD2 Supported
		unsigned long				:3;			// reserved
	} bit;
};

/* structure of Capabilities4	(1913_024Ch)(32bit only) */
union io_sdxc_srs19 {
	unsigned long		word;
	struct {
		unsigned long	MaxCurrentCapa2		:16;	// MaximumCurrentCapabilities2 Register (16 bit)
		unsigned long						:16;	// reserved
	} hword;
	struct {
		unsigned long	MC18V2		:8;			// Maximum Current for 1.8V VDD2
		unsigned long				:24;		// reserved
	} bit;
};

/* structure of PresetValue6	(1913_0274h)(32bit only) */
union io_sdxc_srs29 {
	unsigned long		word;
	struct {
		// PresetValue for UHS2
		unsigned long	UHS2_SCFSV	:10;		// SDCLK Frequency Select Value
		unsigned long	UHS2_CGSV	:1;			// Clock Generator Select Value
		unsigned long				:3;			// reserved
		unsigned long	UHS2_DSSV	:2;			// Driver Strength Select Value
		unsigned long				:16;		// reserved
	} bit;
};

/* structure of UHS-II SDMA Buffer Boundary	(1913_0280h)(32bit only) */
union io_sdxc_srs32 {
	unsigned long		word;
	struct {
		// PresetValue for UHS2
		unsigned long	U2TBS		:12;		// UHS-II Transfer Block Size
		unsigned long	U2SDMABB	:3;			// UHS-II SDMA Buffer Boundary
		unsigned long				:17;		// reserved
	} bit;
};

/* structure of UHS-II Command & TransferMode	(1913_029Ch)(32bit only) */
union io_sdxc_srs39 {
	unsigned long		word;
	struct {
		unsigned long	TransferMode	:16;	// TransferMode  Register (16 bit)
		unsigned long	Command			:16;	// Command       Register (16 bit)
    } hword;
	struct {
		// TransferMode Register bit
		unsigned long	__DMAE		:1;			// DMA Enable
		unsigned long	__U2BCE		:1;			// Bleck Count Enable
		unsigned long				:2;			// reserved
		unsigned long	__U2DTDS	:1;			// Data Transfer Direction
		unsigned long	__U2BBM		:1;			// Block/Byte Mode
		unsigned long	__U2RT		:1;			// Response Type R1/R5
		unsigned long	__U2RECE	:1;			// Response Error Check Enable
		unsigned long	__U2RID		:1;			// Response Interrupt Disable
		unsigned long				:5;			// reserved
		unsigned long	__U2EBSY	:1;			// EBSY Wait
		unsigned long	__U2HFD		:1;			// Half/Full Select
		// Command Register bit
		unsigned long				:5;			// reserved
		unsigned long	__U2DPS		:1;			// Data Present Select
		unsigned long	__U2CTP		:2;			// Command Type
		unsigned long	__U2CPL		:5;			// UHS-II Command Packet Length
		unsigned long				:3;			// reserved
    } bit;
};

/* structure of UHS-II MSG Select	(1913_02B4h)(32bit only) */
union io_sdxc_srs45 {
	unsigned long		word;
	struct {
		// PresetValue for UHS2
		unsigned long	U2MSEL		:2;			// UHS-II MSG Select
		unsigned long				:30;		// reserved
	} bit;
};

/* structure of UHS-II TimerControl & SoftwareReset	(1913_02C0h)(32bit only) */
union io_sdxc_srs48 {
	unsigned long		word;
	struct {
		unsigned long	SoftReset		:16;	// SoftReset     Register (16 bit)
		unsigned long	TimerControl	:16;	// TimerControl  Register (16 bit)
    } hword;
	struct {
		// SoftReset Register bit
		unsigned long	FULLRST		:1;			// Host Full Reset
		unsigned long	TRANRST		:1;			// Host SD-TRAN Reset
		unsigned long				:14;		// reserved
		// TimerControl Register bit
		unsigned long	TCFCR		:4;			// Timeout Counter Value for CMD_RES
		unsigned long	TCFDL		:4;			// Timeout Counter Value for Deadlock
		unsigned long				:8;			// reserved
    } bit;
};

/* structure of UHS-II ErrorInterruptStatus	(1913_02C4h)(32bit only) */
union io_sdxc_srs49 {
	unsigned long		word;
	struct {
		// PresetValue for UHS2
		unsigned long	__EHDR		:1;			// Header Error
		unsigned long	__ERSP		:1;			// RES Packet Error
		unsigned long	__EREX		:1;			// Retry Expired
		unsigned long	__ECRC		:1;			// CRC Error
		unsigned long	__EFRM		:1;			// Framing Error
		unsigned long	__ETID		:1;			// TID Error
		unsigned long				:1;			// reserved
		unsigned long	__EUR		:1;			// Unrecoverable Error
		unsigned long	__EEBSY		:1;			// EBSY Error
		unsigned long				:6;			// reserved
		unsigned long	__EADMA		:1;			// ADMA2 Error
		unsigned long	__ETCR		:1;			// Timeout for CMD_RES
		unsigned long	__ETDL		:1;			// Timeout for Deadlock
		unsigned long				:14;		// reserved
	} bit;
};

/* structure of UHS-II ErrorInterruptStatusEnable	(1913_02C8h)(32bit only) */
union io_sdxc_srs50 {
	unsigned long		word;
	struct {
		// PresetValue for UHS2
		unsigned long	EHDRSE		:1;			// Header Error Status Enable
		unsigned long	ERSPSE		:1;			// RES Packet Error Status Enable
		unsigned long	EREXSE		:1;			// Retry Expired Status Enable
		unsigned long	ECRCSE		:1;			// CRC Error Status Enable
		unsigned long	EFRMSE		:1;			// Framing Error Status Enable
		unsigned long	ETIDSE		:1;			// TID Error Status Enable
		unsigned long				:1;			// reserved
		unsigned long	EURSE		:1;			// Unrecoverable Error Status Enable
		unsigned long	EEBSYSE		:1;			// EBSY Error Status Enable
		unsigned long				:6;			// reserved
		unsigned long	EADMASE		:1;			// ADMA2 Error Status Enable
		unsigned long	ETCRSE		:1;			// Timeout for CMD_RES Status Enable
		unsigned long	ETDLSE		:1;			// Timeout for Deadlock Status Enable
		unsigned long				:14;		// reserved
	} bit;
};

/* structure of UHS-II ErrorInterruptSignalEnable	(1913_02CCh)(32bit only) */
union io_sdxc_srs51 {
	unsigned long		word;
	struct {
		// PresetValue for UHS2
		unsigned long	EHDRSIGE	:1;			// Header Error Signal Enable
		unsigned long	ERSPSIGE	:1;			// RES Packet Error Signal Enable
		unsigned long	EREXSIGE	:1;			// Retry Expired Signal Enable
		unsigned long	ECRCSIGE	:1;			// CRC Error Signal Enable
		unsigned long	EFRMSIGE	:1;			// Framing Error Signal Enable
		unsigned long	ETIDSIGE	:1;			// TID Error Signal Enable
		unsigned long				:1;			// reserved
		unsigned long	EURSIGE		:1;			// Unrecoverable Error Signal Enable
		unsigned long	EEBSYSIGE	:1;			// EBSY Error Signal Enable
		unsigned long				:6;			// reserved
		unsigned long	EADMASIGE	:1;			// ADMA2 Error Signal Enable
		unsigned long	ETCRSIGE	:1;			// Timeout for CMD_RES Signal Enable
		unsigned long	ETDLSIGE	:1;			// Timeout for Deadlock Signal Enable
		unsigned long				:14;		// reserved
	} bit;
};

/* structure of Pointer for UHS-II Capabilities & Settings	(1913_02E0h)(32bit only) */
union io_sdxc_srs56 {
	unsigned long		word;
	struct {
		unsigned long	Settings		:16;	// Settings      Register (16 bit)
		unsigned long	Capabilities	:16;	// Capabilities  Register (16 bit)
    } hword;
	struct {
		// Settings Register bit
		unsigned long	PU2S		:9;			// Pointer for UHS-II Settings
		unsigned long				:7;			// reserved
		// Capabilities Register bit
		unsigned long	PU2HC		:9;			// Pointer for UHS-II Host Capabilities
		unsigned long				:7;			// reserved
    } bit;
};

/* structure of Pointer for Embedded & Test		(1913_02E4h)(32bit only) */
union io_sdxc_srs57 {
	unsigned long		word;
	struct {
		unsigned long	Test			:16;	// Test      Register (16 bit)
		unsigned long	Embedded		:16;	// Embedded  Register (16 bit)
    } hword;
	struct {
		// Test Register bit
		unsigned long	PU2TR		:9;			// Pointer for UHS-II Test Register
		unsigned long				:7;			// reserved
		// Embedded Register bit
		unsigned long				:16;		// reserved
    } bit;
};


/* structure of UHS-II General Settings		(1913_0300h)(32bit only) */
union io_sdxc_ers00 {
	unsigned long		word;
	struct {
		unsigned long	PWRM		:1;			// Power Mode
		unsigned long				:7;			// reserved
		unsigned long	NFL			:4;			// Number and function of lanes
		unsigned long				:20;		// reserved
    } bit;
};

/* structure of UHS-II PHY Settings			(1913_0304h)(32bit only) */
union io_sdxc_ers01 {
	unsigned long		word;
	struct {
		unsigned long				:6;			// reserved
		unsigned long	SPDR		:1;			// Speed Range
		unsigned long				:8;			// reserved
		unsigned long	HIBE		:1;			// Hibernate Enable
		unsigned long	HNLS		:4;			// Host N_LSS_SYN
		unsigned long	HNLD		:4;			// Host N_LSS_DIR
		unsigned long				:8;			// reserved
    } bit;
};

/* structure of UHS-II LINK/TRAN Settings1	(1913_0308h)(32bit only) */
union io_sdxc_ers02 {
	unsigned long		word;
	struct {
		unsigned long				:8;			// reserved
		unsigned long	HNF			:8;			// Host N_FCU
		unsigned long	RTCNT		:2;			// Retry Count
		unsigned long				:14;		// reserved
    } bit;
};

/* structure of UHS-II LINK/TRAN Settings2	(1913_030Ch)(32bit only) */
union io_sdxc_ers03 {
	unsigned long		word;
	struct {
		unsigned long	HNDG		:8;			// Host N_DATA_GAP
		unsigned long				:24;		// reserved
    } bit;
};

/* structure of UHS-II General Capabilities	(1913_0310h)(32bit only) */
union io_sdxc_ers04 {
	unsigned long		word;
	struct {
		unsigned long	DAP			:4;			// Device Allocation Power
		unsigned long	GAPOW		:4;			// Group Allocation Power
		unsigned long	NFLS		:6;			// Number and function of lanes supported
		unsigned long	A64BS		:1;			// 64-bit Addressing supported
		unsigned long	BCL			:1;			// Boot Code Loading supported
		unsigned long	RMEM		:2;			// Removable / Embedded
		unsigned long	NSD			:4;			// Number of Supported Devices
		unsigned long	BT			:2;			// Bus Topology
		unsigned long				:8;			// reserved
    } bit;
};

/* structure of UHS-II PHY Capabilities		(1913_0314h)(32bit only) */
union io_sdxc_ers05 {
	unsigned long		word;
	struct {
		unsigned long	PHYREV		:6;			// PHY Revision
		unsigned long	SPDRNG		:2;			// Speed Range
		unsigned long				:8;			// reserved
		unsigned long	HNLSS		:4;			// Host N_LSS_SYN
		unsigned long	HNLDS		:4;			// Host N_LSS_DIR
		unsigned long				:8;			// reserved
    } bit;
};

/* structure of UHS-II LINK/TRAN Capabilities1	(1913_0318h)(32bit only) */
union io_sdxc_ers06 {
	unsigned long		word;
	struct {
		unsigned long	LNKREV		:6;			// LINK Revision
		unsigned long				:2;			// reserved
		unsigned long	HNFS		:8;			// Host N_FCU
		unsigned long	HDT			:3;			// Host Device Type
		unsigned long				:1;			// reserved
		unsigned long	HMBL		:12;		// Host Maximum Block Length
    } bit;
};

/* structure of UHS-II LINK/TRAN Capabilities2	(1913_031Ch)(32bit only) */
union io_sdxc_ers07 {
	unsigned long		word;
	struct {
		unsigned long	HNDG		:8;			// Host N_DATA_GAP
		unsigned long				:24;		// reserved
    } bit;
};

/* structure of UHS-II Test					(1913_0320h)(32bit only) */
union io_sdxc_ers08 {
	unsigned long		word;
	struct {
		unsigned long	FEHDR		:1;			// Force Event for Headed Error
		unsigned long	FERES		:1;			// Force Event for RES Packet Error
		unsigned long	FEREX		:1;			// Force Event for Retry Expired
		unsigned long	FECRC		:1;			// Force Event for CRC Error
		unsigned long	FEFRM		:1;			// Force Event for Framing Error
		unsigned long	FETID		:1;			// Force Event for TID Error
		unsigned long				:1;			// reserved
		unsigned long	FEUNR		:1;			// Force Event for Unrecoverable Error
		unsigned long	FEEBY		:1;			// Force Event for EBSY Error
		unsigned long				:6;			// reserved
		unsigned long	FEAD2		:1;			// Force Event for ADMA2 Error
		unsigned long	FETCR		:1;			// Force Event for Timeout for CMD_RES
		unsigned long	FETDL		:1;			// Force Event for Timeout for Deadlock
		unsigned long				:14;		// reserved
    } bit;
};


struct io_sdxc2 {
	unsigned char						dmy_sdio_000_003[0x0004-0x0000];	/* 1913_(0000 - 0003h) */
	union io_sdxc_hrs01					HRS01;								/* 1913_(0004 - 0007h) */
	union io_sdxc_hrs02					HRS02;								/* 1913_(0008 - 000Bh) */
	union io_sdxc_hrs03					HRS03;								/* 1913_(000C - 000Fh) */
	unsigned char						dmy_sdio_010_05F[0x0060-0x0010];	/* 1913_(0010 - 005Fh) */
	union io_sdxc_hrs32					HRS32;								/* 1913_(0060 - 0063h) */
	unsigned char						dmy_sdio_064_067[0x0068-0x0064];	/* 1913_(0064 - 0067h) */
	union io_sdxc_hrs34					HRS34;								/* 1913_(0068 - 006Bh) */
	unsigned char						dmy_sdio_070_073[0x0074-0x006C];	/* 1913_(006C - 0073h) */
	union io_sdxc_hrs37					HRS37;								/* 1913_(0074 - 0077h) */
	union io_sdxc_hrs38					HRS38;								/* 1913_(0078 - 007Bh) */
	union io_sdxc_hrs39					HRS39;								/* 1913_(007C - 007Fh) */
	unsigned char						dmy_sdio_080_08F[0x0090-0x0080];	/* 1913_(0080 - 008Fh) */
	union io_sdxc_hrs44					HRS44;								/* 1913_(0090 - 0093h) */
	union io_sdxc_hrs45					HRS45;								/* 1913_(0094 - 0097h) */
	union io_sdxc_hrs46					HRS46;								/* 1913_(0098 - 009Bh) */
	unsigned char						dmy_sdio_0A0_0EB[0x00EC-0x009C];	/* 1913_(009C - 00EBh) */
	union io_sdxc_hrs59					HRS59;								/* 1913_(00EC - 00EFh) */
	unsigned char						dmy_sdio_0F0_0FB[0x00FC-0x00F0];	/* 1913_(00F0 - 00FBh) */
	union io_sdxc_crs63					CRS63;								/* 1913_(00FC - 00FFh) */
	unsigned char						dmy_sdio_100_1FF[0x0200-0x0100];	/* 1913_(0100 - 01FFh) */

	unsigned char						dmy_sdio_200_20F[0x0210-0x0200];	/* 1913_(0200 - 020Fh) */
	unsigned long						SRS04;								/* 1913_(0210 - 0213h) */
	unsigned long						SRS05;								/* 1913_(0214 - 0217h) */
	unsigned long						SRS06;								/* 1913_(0218 - 021Bh) */
	unsigned long						SRS07;								/* 1913_(021C - 021Fh) */
	unsigned long						SRS08;								/* 1913_(0220 - 0223h) */
	union io_sdxc_srs09					SRS09;								/* 1913_(0224 - 0227h) */
	union io_sdxc_srs10					SRS10;								/* 1913_(0228 - 022Bh) */
	union io_sdxc_srs11					SRS11;								/* 1913_(022C - 022Fh) */
	union io_sdxc_srs12					SRS12;								/* 1913_(0230 - 0233h) */
	union io_sdxc_srs13					SRS13;								/* 1913_(0234 - 0237h) */
	union io_sdxc_srs14					SRS14;								/* 1913_(0238 - 023Bh) */
	union io_sdxc_srs15					SRS15;								/* 1913_(023C - 023Fh) */
	union io_sdxc_srs16					SRS16;								/* 1913_(0240 - 0243h) */
	union io_sdxc_srs17					SRS17;								/* 1913_(0244 - 0247h) */
	union io_sdxc_mcurcap1_0			SRS18;								/* 1913_(0248 - 024Bh) */
	union io_sdxc_srs19					SRS19;								/* 1913_(024C - 024Fh) */
	unsigned char						dmy_sdio_250_253[0x0254-0x0250];	/* 1913_(0250 - 0253h) */
	union io_sdxc_admaest				SRS21;								/* 1913_(0254 - 0257h) */
	unsigned long						SRS22;								/* 1913_(0258 - 025Bh) */
	unsigned long						SRS23;								/* 1913_(025C - 025Fh) */
	unsigned char						dmy_sdio_260_273[0x0274-0x0260];	/* 1913_(0260 - 0273h) */
	union io_sdxc_srs29					SRS29;								/* 1913_(0274 - 0277h) */
	unsigned char						dmy_sdio_278_27F[0x0280-0x0278];	/* 1913_(0278 - 027Fh) */
	union io_sdxc_srs32					SRS32;								/* 1913_(0280 - 0283h) */
	unsigned long						SRS33;								/* 1913_(0284 - 0287h) */
	unsigned long						SRS34;								/* 1913_(0288 - 028Bh) */
	unsigned long						SRS35;								/* 1913_(028C - 028Fh) */
	unsigned long						SRS36;								/* 1913_(0290 - 0293h) */
	unsigned long						SRS37;								/* 1913_(0294 - 0297h) */
	unsigned long						SRS38;								/* 1913_(0298 - 029Bh) */
	union io_sdxc_srs39					SRS39;								/* 1913_(029C - 029Fh) */
	unsigned long						SRS40;								/* 1913_(02A0 - 02A3h) */
	unsigned long						SRS41;								/* 1913_(02A4 - 02A7h) */
	unsigned long						SRS42;								/* 1913_(02A8 - 02ABh) */
	unsigned long						SRS43;								/* 1913_(02AC - 02AFh) */
	unsigned long						SRS44;								/* 1913_(02B0 - 02B3h) */
	union io_sdxc_srs45					SRS45;								/* 1913_(02B4 - 02B7h) */
	unsigned long						SRS46;								/* 1913_(02B8 - 02BBh) */
	unsigned char						dmy_sdio_2BC_2BF[0x02C0-0x02BC];	/* 1913_(02BC - 02BFh) */
	union io_sdxc_srs48					SRS48;								/* 1913_(02C0 - 02C3h) */
	union io_sdxc_srs49					SRS49;								/* 1913_(02C4 - 02C7h) */
	union io_sdxc_srs50					SRS50;								/* 1913_(02C8 - 02CBh) */
	union io_sdxc_srs51					SRS51;								/* 1913_(02CC - 02CFh) */
	unsigned char						dmy_sdio_2D0_2DF[0x02E0-0x02D0];	/* 1913_(02D0 - 02DFh) */
	union io_sdxc_srs56					SRS56;								/* 1913_(02E0 - 02E3h) */
	union io_sdxc_srs57					SRS57;								/* 1913_(02E4 - 02E7h) */
	unsigned char						dmy_sdio_2E8_2FF[0x0300-0x02E8];	/* 1913_(02E8 - 02FFh) */

	union io_sdxc_ers00					ERS00;								/* 1913_(0300 - 0303h) */
	union io_sdxc_ers01					ERS01;								/* 1913_(0304 - 0307h) */
	union io_sdxc_ers02					ERS02;								/* 1913_(0308 - 030Bh) */
	union io_sdxc_ers03					ERS03;								/* 1913_(030C - 030Fh) */
	union io_sdxc_ers04					ERS04;								/* 1913_(0310 - 0313h) */
	union io_sdxc_ers05					ERS05;								/* 1913_(0314 - 0317h) */
	union io_sdxc_ers06					ERS06;								/* 1913_(0318 - 031Bh) */
	union io_sdxc_ers07					ERS07;								/* 1913_(031C - 031Fh) */
	union io_sdxc_ers08					ERS08;								/* 1913_(0320 - 0323h) */
};

extern volatile struct io_sdxc2 IO_SDXC32;		/* 1913_0000 */

