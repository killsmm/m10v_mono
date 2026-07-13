/**
 * @file		sd_cmd.h
 * @brief		SD Memory card Command function header file
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef	_SD_CMD_H_
#define	_SD_CMD_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// SDIO Support
#define D_SDIO_FLG_BRE				0x00010000
#define D_SDIO_FLG_BWE				0x00020000
#define D_SDIO_FLG_TC				0x00040000
#define D_SDIO_FLG_ERR				0x00080000
#define D_SDIO_TOUT_RW				5000
#define D_SDIO_TOUT_TC				5000

#define D_SD_CHECK_CMD20			0x01		// Check CMD20 Support
#define D_SD_CHECK_CMD23			0x02		// Check CMD23 Support

// ACMD41 SD Card Initialize
#define D_SD_CHECK_S18A_BIT			0x01000000	// Check ACMD41 S18A bit Value
#define D_SD_S18R_BIT_REQ			1			// Switch to 1.8V signal Voltage (ACMD41 S18R bit Value)
#define D_SD_S18R_BIT_NOT_REQ		0			// Use Current Signal Voltage    (ACMD41 S18R bit Value)
#define D_SD_S18A_BIT_ACCEPT		1			// Ready for switching signal voltage  (ACMD41 Response S18A bit Value)
#define D_SD_S18A_BIT_NOT_ACCEPT	0			// Continues Current Voltage Signaling (ACMD41 Response S18A bit Value)
#define D_SD_UHS_SUP_CARD			1			// UHS-I Mode Support Card
#define D_SD_UHS_NOT_SUP_CARD		0			// UHS-I Mode Not Support Card
#define D_SD_XPC_BIT_MAX_PERFORM	1			// Maximum Performance Mode (ACMD41 SDXC Power Control bit Value)
#define D_SD_XPC_BIT_POW_SAVE		0			// Power Saving Mode        (ACMD41 SDXC Power Control bit Value)

// CMD20 Support Speed Class
#define D_SD_SCC_START_RECORD		0x00000000	// Start Recording(Argument in CMD20)
#define D_SD_SCC_CREATE_DIR			0x10000000	// Create DIR(Argument in CMD20)
#define D_SD_SCC_END_REC_NO_MOVE	0x20000000	// End Recording without Move(Argument in CMD20)
#define D_SD_SCC_END_REC_MOVE		0x30000000	// End Recordng with Move(Argument in CMD20)
#define D_SD_SCC_UPDATE_CI			0x40000000	// Update CI(Argument in CMD20)

// CMD11 Switch Signal Voltage
#define D_SD_CHECK_CMD11_WAIT_1MS			1		// Power Switch Wait 1ms
#define D_SD_CHECK_CMD11_WAIT_5MS_BEFORE	3		// Power Switch Wait 5ms
#define D_SD_CHECK_CMD11_WAIT_5MS_AFTER		2		// Power Switch Wait 5ms

// CMD6 Switch Function
// Access Mode(Function Group 1)
#define D_SD_ACCESS_DEFAULT_SPEED	0				// Default Speed Mode
#define D_SD_ACCESS_HI_SPEED		1				// High Speed Mode
#define D_SD_ACCESS_UHS_SDR12		2				// UHS-I SDR12 Mode
#define D_SD_ACCESS_UHS_SDR25		3				// UHS-I SDR25 Mode
#define D_SD_ACCESS_UHS_SDR50		4				// UHS-I SDR50 Mode
#define D_SD_ACCESS_UHS_SDR104		5				// UHS-I SDR104 Mode
#define D_SD_ACCESS_UHS2			6				// UHS-II
#define D_SD_UHS_VALUE				2				// Check UHS-I Mode
//#define D_SD_UHS_MAX_SPEED_MODE		(D_SD_ACCESS_UHS_SDR50 - D_SD_UHS_VALUE)	// UHS-I Max Speed Mode(in Macro Support)
#define D_SD_UHS_MAX_SPEED_MODE		(D_SD_ACCESS_UHS_SDR104 - D_SD_UHS_VALUE)	// UHS-I Max Speed Mode(in Macro Support)

// Driver Strength(Function Group 3)
#define D_SD_DRV_POW_TYPE_B			0				// Type B Mode(default)
#define D_SD_DRV_POW_TYPE_A			1				// Type A Mode
#define D_SD_DRV_POW_TYPE_C			2				// Type C Mode
#define D_SD_DRV_POW_TYPE_D			3				// Type D Mode

// Current Limit(Function Group 4)
#define D_SD_CUR_LIM_200MA			0				// Max 200mA Mode(default)
#define D_SD_CUR_LIM_400MA			1				// Max 400mA Mode
#define D_SD_CUR_LIM_600MA			2				// Max 600mA Mode
#define D_SD_CUR_LIM_800MA			3				// Max 800mA Mode

// Current Limit(Function Group 4) UHS2
#define D_SD_PWR_LIM_0_72W			0				// Max 0.72W Mode(default)
#define D_SD_PWR_LIM_1_44W			1				// Max 1.44W Mode
#define D_SD_PWR_LIM_2_16W			2				// Max 2.16W Mode
#define D_SD_PWR_LIM_2_88W			3				// Max 2.88W Mode
#define D_SD_PWR_LIM_1_80W			4				// Max 1.80W Mode

// UHS2 Interface mode
#define D_SD_UHS2_MODE_FD			0				// Full Duplex(FD) mode(default)
#define D_SD_UHS2_MODE_2LHD			1				// 2Line-Half Duplex(2L-HD) mode

// UHS2 Speed range
#define D_SD_UHS2_RANGE_A			0				// Speed range : A(x15)
#define D_SD_UHS2_RANGE_B			1				// Speed range : B(x30)(default)

// Check New Command Responce
#define	D_SDXC_CARD_ST_CMD11		0x01180000		// Check CMD11 Response Status
#define	D_SDXC_CARD_ST_CMD19		0xc7380000		// Check CMD19 Response Status
#define	D_SDXC_CARD_ST_CMD20		0xc7380000		// Check CMD20 Response Status
#define	D_SDXC_CARD_ST_CMD23		0xc7380000		// Check CMD23 Response Status

// CMD11 Switch Signal Voltage Control -> Hardware control is not supported by MILB
//#define	D_POWER_SWITCH_HARD_CONTROL		1			// Switch Bus Power 3.3V to 1.8V Hard Ware Control ON

// UHS-II CCMD
#define	D_SD2_DEV_REP				0x00800000
#define	D_SD2_DEV_REP_NACK			0x00800000
#define	D_SD2_DEV_REP_ACK			0x00000000

// UHS-II CCMD(DEVICE INIT)
#define	D_SD2_DEV_DAP				0x0000F000
#define	D_SD2_DEV_CF				0x00000800
#define	D_SD2_DEV_GAP				0x0000000F

// UHS-II CCMD(ENUMARATION)
#define	D_SD2_ENU_ID_F				0x000000F0
#define	D_SD2_ENU_ID_L				0x0000000F

// UHS-II CCMD(INQUIRY_CONFIG/SET_COMMON_CONFIG)
#define	D_SD2_CONFIG_GEN			0x00000000
#define	D_SD2_CONFIG_PHY			0x02000000
#define	D_SD2_CONFIG_LNK			0x04000000


/*----------------------------------------------------------
	Command for SD controler								
------------------------------------------------------------
*/

//--- Register:CMD Set Data ----
#define D_SD_CMD00		0x0000
#define	D_SD_CMD01		0x0102
#define D_SD_CMD02		0x0209
#define D_SD_CMD03		0x031A
#define D_SD_CMD04		0x0400
#define D_SD_CMD06		0x063A
#define D_SD_CMD07		0x071B
#define D_SD_CMD08		0x081A
#define D_SD_CMD09		0x0909
#define D_SD_CMD10		0x0A09
#define D_SD_CMD11		0x0B1B
#define D_SD_CMD12		0x0CDB
#define D_SD_CMD13		0x0D1A
#define D_SD_CMD15		0x0F00
#define D_SD_CMD16		0x101A
#define D_SD_CMD17		0x113A
#define D_SD_CMD18		0x123A
#define D_SD_CMD19		0x133B
#define D_SD_CMD20		0x141B
#define D_SD_CMD23		0x171A
#define D_SD_CMD24		0x183A
#define D_SD_CMD25		0x193A
#define D_SD_CMD27		0x1B3A
#define D_SD_CMD28		0x1C1B
#define D_SD_CMD29		0x1D1B
#define D_SD_CMD30		0x1E3A
#define D_SD_CMD32		0x201A
#define D_SD_CMD33		0x211A
#define D_SD_CMD38		0x261B
#define D_SD_CMD42		0x2A3A
#define D_SD_CMD55		0x371A
#define D_SD_CMD56R		0x381B
#define D_SD_CMD56W		0x381B

#define D_SD_ACMD06		0x061A
#define D_SD_ACMD13		0x0D3A
#define D_SD_ACMD22		0x163A
#define D_SD_ACMD23		0x171A
#define D_SD_ACMD41		0x2902
#define D_SD_ACMD42		0x2A1A
#define D_SD_ACMD51		0x333A

//--- Register:TRansfer Mode Set Data ----
#define D_SD_CMD00_TM	0x0000
#define	D_SD_CMD01_TM	0x0000
#define D_SD_CMD02_TM	0x0000
#define D_SD_CMD03_TM	0x0000
#define D_SD_CMD04_TM	0x0000
#define D_SD_CMD06_TM	0x0010
#define D_SD_CMD07_TM	0x0000
#define D_SD_CMD08_TM	0x0000
#define D_SD_CMD09_TM	0x0000
#define D_SD_CMD10_TM	0x0000
#define D_SD_CMD11_TM	0x0000
#define D_SD_CMD12_TM	0x0000
#define D_SD_CMD13_TM	0x0000
#define D_SD_CMD15_TM	0x0000
#define D_SD_CMD16_TM	0x0000
#define D_SD_CMD17_TM	0x0010
#define D_SD_CMD18_TM	0x0036
#define D_SD_CMD19_TM	0x0010
#define D_SD_CMD20_TM	0x0000
#define D_SD_CMD23_TM	0x0000
#define D_SD_CMD24_TM	0x0000
#define D_SD_CMD25_TM	0x0026
#define D_SD_CMD27_TM	0x0000
#define D_SD_CMD28_TM	0x0000
#define D_SD_CMD29_TM	0x0000
#define D_SD_CMD30_TM	0x0010
#define D_SD_CMD32_TM	0x0000
#define D_SD_CMD33_TM	0x0000
#define D_SD_CMD38_TM	0x0000
#define D_SD_CMD42_TM	0x0000
#define D_SD_CMD55_TM	0x0000
#define D_SD_CMD56R_TM	0x0010
#define D_SD_CMD56W_TM	0x0000

#define D_SD_ACMD06_TM	0x0000
#define D_SD_ACMD13_TM	0x0010
#define D_SD_ACMD22_TM	0x0010
#define D_SD_ACMD23_TM	0x0000
#define D_SD_ACMD41_TM	0x0000
#define D_SD_ACMD42_TM	0x0000
#define D_SD_ACMD51_TM	0x0010

/*----------------------------------------------------------
	Command for SD controler(UHS-II)						
------------------------------------------------------------
*/
//--- Register:CMD Set Data ----
#define D_SD_DEVICE			0x0800
#define D_SD_ENUMERATION	0x0800
#define D_SD_CFG_READ		0x0400
#define D_SD_SET_CFG		0x0C00
#define D_SD_GO_DORMANT_ST	0x08C0
#define D_SD_CCMD			0x0800
#define D_SD_DCMD_LEN8		0x0820
#define D_SD_DCMD_LEN12		0x0C20
#define D_SD_TRANS_ABORT	0x0480

//--- Register:TRansfer Mode Set Data ----
#define D_SD_DEVICE_TM			0x0000
#define D_SD_ENUMERATION_TM		0x0000
#define D_SD_CFG_READ_TM		0x0000
#define D_SD_SET_CFG_TM			0x0000
#define D_SD_GO_DORMANT_ST_TM	0x0000
#define D_SD_CCMD_TM			0x0000
#define D_SD_CCMD_BUSY_TM		0x4000
#define D_SD_TRANS_ABORT_TM		0x0000

#define D_SD_DCMD_W_TM			0x4092
#define D_SD_DCMD_R_TM			0x4082
#define D_SD_DCMD17_TM			0x4082
#define D_SD_DCMD18_TM			0x4082
#define D_SD_DCMD24_TM			0x4092
#define D_SD_DCMD25_TM			0x4092

//--- Register:Packet Set Data ----
#define D_SD_PAC_DEVICE			0x02920080
#define D_SD_PAC_ENUMERATION	0x03920080
#define D_SD_PAC_CFG_READ		0x00200080
#define D_SD_PAC_SET_CFG		0x08A00080
#define D_SD_PAC_GO_DORMANT_ST	0x01920080
#define D_SD_PAC_TRANS_ABORT	0x04820080

#define D_SD_PAC_CMDIND			0x7F000000
#define D_SD_PAC_CMD00			0x00000000
#define D_SD_PAC_CMD02			0x02000000
#define D_SD_PAC_CMD03			0x03000000
#define D_SD_PAC_CMD06			0x06000010
#define D_SD_PAC_CMD07			0x07000000
#define D_SD_PAC_CMD08			0x08000000
#define D_SD_PAC_CMD09			0x09000000
#define D_SD_PAC_CMD10			0x0A000000
#define D_SD_PAC_CMD12			0x0C000000
#define D_SD_PAC_CMD13			0x0D000000
#define D_SD_PAC_CMD16			0x10000000
#define D_SD_PAC_CMD17			0x11000010	// F_SDH40_LP errata Rev1.1 (Use only FD mode)
#define D_SD_PAC_CMD18			0x12200010	// F_SDH40_LP errata Rev1.1 (Use only FD mode)
#define D_SD_PAC_CMD20			0x14000000
#define D_SD_PAC_CMD24			0x18800010	// F_SDH40_LP errata Rev1.1 (Use only FD mode)
#define D_SD_PAC_CMD25			0x19A00010	// F_SDH40_LP errata Rev1.1 (Use only FD mode)
#define D_SD_PAC_CMD30			0x1E000010	// F_SDH40_LP errata Rev1.1 (Use only FD mode)
#define D_SD_PAC_CMD42			0x2A800010	// F_SDH40_LP errata Rev1.1 (Use only FD mode)
#define D_SD_PAC_ACMD13			0x4D000010	// F_SDH40_LP errata Rev1.1 (Use only FD mode)
#define D_SD_PAC_ACMD22			0x56000010	// F_SDH40_LP errata Rev1.1 (Use only FD mode)
#define D_SD_PAC_ACMD41			0x69000000
#define D_SD_PAC_ACMD51			0x73000010	// F_SDH40_LP errata Rev1.1 (Use only FD mode)

//--- Register:Packet Bit ----
#define D_SD_PAC_DM_FD			0x00000000	// Duplex Mode:FD
#define D_SD_PAC_DM_2LHD		0x00400000	// Duplex Mode:2LHD


/*----------------------------------------------------------
	SD Card Command Status									
	# except bit23(COM_CRC_ERROR) because it is the CRC 	
	  check of the previous command failed					
	# except bit22(ILLEGAL_COMMAND) because it is the 		
	  previous command not legal for the card state			
------------------------------------------------------------
*/
#define	D_SD_CARD_ST_CMD03		(0x00006000)
#define	D_SD_CARD_ST_CMD06		(0xcd3e0000)
#define	D_SD_CARD_ST_CMD07		(0x01180000)
#define	D_SD_CARD_ST_CMD12		(0x073e0000)
#define	D_SD_CARD_ST_CMD13		(0x07380000)
#define	D_SD_CARD_ST_CMD16		(0x21180000)
#define	D_SD_CARD_ST_CMD17		(0xc7380000)
#define	D_SD_CARD_ST_CMD18		(0xc7380000)
#define	D_SD_CARD_ST_CMD24		(0xc7380000)
#define	D_SD_CARD_ST_CMD25		(0xc7380000)
#define D_SD_CARD_ST_CMD42		(0x21180000)
#define D_SD_CARD_ST_CMD55		(0x01180000)
#define D_SD_CARD_ST_ACMD06		(0x01180000)
#define D_SD_CARD_ST_ACMD13		(0xcd3e0000)
#define D_SD_CARD_ST_ACMD22		(0xc7180000)
#define D_SD_CARD_ST_ACMD42		(0x01180000)
#define D_SD_CARD_ST_ACMD51		(0xc7180000)
#define D_SD_CARD_ST_APPCMD		(0x00000020)
#define	D_SD_CARD_ST_SWERR		(0x00000080)
#define	D_SD_CARD_ST_CDLKD		(0x02000000)
#define	D_SD_CARD_ST_ERR		(0x011f4000)

/*-----  Support OCR Value  ----------------------------------------------------------------------
------------------------------------------------------------------------------------------------*/
#define	D_SD_OCR_SPTALL				(0x00360060)
#define	D_SD_OCR_SPT33				(0x00300000)
#define	D_SD_OCR_SPT30				(0x00060000)
#define	D_SD_OCR_SPT18				(0x00000060)

/*-----  define ----------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------*/
#define D_SD_ARG_NULL				(0x00000000)
#define D_SD_BLK_NULL				(0x00)
#define D_SD_ERR_NULL				(0x0000)
#define D_SD_SIZE_NULL				(0x0000)
#define D_SD_CNT_NULL				(0x0000)
#define D_SD_INTST_NULL				(0x00000000)
#define D_SD_OCR_NULL				(0x00000000)

#define	D_SD_RCA					(0x00000001)
#define	D_SD_OC_PWR_BSY				(0x80000000)
#define	D_SD_ACCESS_MODE			(0x60000000)
#define	D_SD_OC_CARD_CAP			(0x40000000)
#define	D_SD_ECHO_BK_VOL			(0x00000fff)
#define	D_SD_BUSPOW_TM				(2000)
#define	D_SD_ERRREC_TM				(10)

#define	D_SD_BLK_LEN				(512)

#define	D_SD_STS_SIZE				(64)
#define	D_SD_SCR_SIZE				(8)
#define	D_SD_WR_BLKS_SIZE			(4)
#define	D_SD_SFS_SIZE				(64)
#define	D_MMC_EXT_CSD_SIZE			(512)
#define	D_SD_BCNT_MAX				(0xFFFF)
#define	D_SDXC_BCNT_MAX				(0xFFFFFFFF)
#define	D_SDIO_MAX_BLOCK_SIZE		(2048)

#define	D_SD_BUSWIDTH1				(0x00000000)
#define	D_SD_BUSWIDTH4				(0x00000002)
#define	D_SD_SCR_BUSW1				(0x01)
#define	D_SD_SCR_BUSW4				(0x04)

#define	D_SD_CARD_TYPE				(2)

#define	D_SD_WP_VIOLATION			(0x04000000)

#define	D_SD_CARD_TYPE_MMC			(1)
#define	D_SD_CARD_TYPE_SD			(2)
#define	D_SD_CARD_TYPE_SDIO			(3)
#define	D_SD_DN						(0)

#define	D_SD_SPECVER1_0				(0)
#define	D_SD_SPECVER1_4				(1)
#define	D_SD_SPECVER2_2				(2)
#define	D_SD_SPECVER3_0				(3)

#define	D_SD_START_TIMER			(0)
#define	D_SD_STOP_TIMER				(1)

#define	D_SD_MULTI_STATE_STOP		(0)
#define	D_SD_MULTI_STATE_READ		(1)
#define	D_SD_MULTI_STATE_WRITE		(2)

#define	D_SD_IDLE_STATE				(0)
#define	D_SD_READY_STATE			(1)
#define	D_SD_IDENT_STATE			(2)
#define	D_SD_STBY_STATE				(3)
#define	D_SD_TRAN_STATE				(4)
#define	D_SD_DATA_STATE				(5)
#define	D_SD_RCV_STATE				(6)
#define	D_SD_PRG_STATE				(7)
#define	D_SD_DIS_STATE				(8)

#define	D_SDIO_CISTPL_NULL			(0x00)
#define	D_SDIO_CISTPL_CHECKSUM		(0x10)
#define	D_SDIO_CISTPL_VERS_1		(0x15)
#define	D_SDIO_CISTPL_ALTSTR		(0x16)
#define	D_SDIO_CISTPL_MANFID		(0x20)
#define	D_SDIO_CISTPL_FUNCID		(0x21)
#define	D_SDIO_CISTPL_FUNCE			(0x22)
#define	D_SDIO_CISTPL_VENDER_MIN	(0x80)
#define	D_SDIO_CISTPL_VENDER_MAX	(0x80)
#define	D_SDIO_CISTPL_END			(0xff)

#define SD_Min(a, b)				(((a) < (b)) ? (a) : (b))

#define	D_SD_FNC_CHK				(0x00000000)
#define	D_SD_FNC_SW					(0x80000000)
#define	D_SD_FNC_NOINFL				(0x00FFFFFF)
#define	D_SD_FNC_G1_INFL			(0x00FFFFF0)
#define	D_SD_FNC_G2_INFL			(0x00FFFF0F)
#define	D_SD_FNC_G3_INFL			(0x00FFF0FF)
#define	D_SD_FNC_G4_INFL			(0x00FF0FFF)
#define	D_SD_FNC_G5_INFL			(0x00F0FFFF)
#define	D_SD_FNC_G6_INFL			(0x000FFFFF)

#define	D_SD_FNC_GRP1				(1)
#define	D_SD_FNC_GRP2				(2)
#define	D_SD_FNC_GRP3				(3)
#define	D_SD_FNC_GRP4				(4)
#define	D_SD_FNC_GRP5				(5)
#define	D_SD_FNC_GRP6				(6)

#define	D_SD_FNC_NO_0				(0x0)
#define	D_SD_FNC_NO_1				(0x1)
#define	D_SD_FNC_NO_2				(0x2)
#define	D_SD_FNC_NO_3				(0x3)
#define	D_SD_FNC_NO_4				(0x4)
#define	D_SD_FNC_NO_5				(0x5)
#define	D_SD_FNC_NO_6				(0x6)
#define	D_SD_FNC_NO_7				(0x7)
#define	D_SD_FNC_NO_8				(0x8)
#define	D_SD_FNC_NO_9				(0x9)
#define	D_SD_FNC_NO_A				(0xA)
#define	D_SD_FNC_NO_B				(0xB)
#define	D_SD_FNC_NO_C				(0xC)
#define	D_SD_FNC_NO_D				(0xD)
#define	D_SD_FNC_NO_E				(0xE)
#define	D_SD_FNC_NO_F				(0xF)

#define	D_SD_FNC_GET				(0x0000000F)

#define	D_SD_DMA_MODE_NONDMA		(0x00000000)
#define	D_SD_DMA_MODE_SDMA			(0x00000001)
#define	D_SD_DMA_MODE_ADMA32		(0x00000002)
#define	D_SD_DMA_MODE_ADMA32_2		(0x00000003)
#define	D_SD_DMA_KIND_MASK			(0x00000003)


#define	D_SD_DMA_MODE_CHK			(0x00000003)

#define	D_SD_SDMA_BUN_KB			(0x000003FC)
#define	D_SD_DMA_INT_1CNT			(0x00010000)
#define	D_SD_DMA_INT_CHK			(0xFFFF0000)
#define	D_SD_DMA_MIN_BUN			(0x1000)

// SD DMA Event wait
#define D_SD_FLG_DMA_INT			(0x00000001)
#define D_SD_FLG_DMA_ERR			(0x00000002)
#define D_SD_FLG_REMOVE				(0x00000004)
#define D_SD_TOUT_DMA				(16000)
#define D_SD_TOUT_ERASE				(100000)

#define	D_MMC_SPEC_VER4				(0x04)

#define	D_MMC_EXTCSDA_CMDS			(0x00000000)
#define	D_MMC_EXTCSDA_SET			(0x01000000)
#define	D_MMC_EXTCSDA_CLEAR			(0x02000000)
#define	D_MMC_EXTCSDA_WRITE			(0x03000000)

#define	D_MMC_EXTCSDI_BUSW			(0x00B70000)
#define	D_MMC_EXTCSDI_HISPD			(0x00B90000)

#define	D_MMC_EXTCSDV_SHIFT			(8)
#define	D_MMC_EXTCSDV_NULL			(0x00000000)
#define	D_MMC_EXTCSDV_HISPD			(0x00000100)

#define	D_MMC_EXTCSDC_ATA			(0x00000004)

#define	D_MMC_ATA_CMD_SPT			(0x08)
#define	D_MMC_HISPD_CLK				(26000)
#define	D_MMC_HS_TIM_DIS			(0x00)
#define	D_MMC_HS_TIM_EN				(0x01)
#define	D_MMC_CARD_TYPE1			(0x01)
#define	D_MMC_CARD_TYPE2			(0x02)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
// Nothing Special

#endif /* _SD_CMD_H_ */
