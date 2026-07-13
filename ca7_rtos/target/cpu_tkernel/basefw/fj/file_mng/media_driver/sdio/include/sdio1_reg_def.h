/**
 * @file		sdio1_reg_def.h
 * @brief		SD Memory card driver register header file
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef	_SDIO1_REG_DEF_H_
#define	_SDIO1_REG_DEF_H_

#include "sd.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------
	SDIO Common Define										
------------------------------------------------------------
*/
#define D_SD_REG_CLR						0				// Register Clear

#define D_SD_W_ALL							0xFFFFFFFF		// WORD ALL Bit

#define D_SD_W_MASK_HI						0x0000FFFF		// WORD Mask High
#define D_SD_W_MASK_LOW						0xFFFF0000		// WORD Mask Low

#define D_SD_HW_ALL							0xFFFF			// HWORD ALL Bit
#define D_SD_HW_MASK_HI						0x00FF			// HWORD Mask High
#define D_SD_HW_MASK_LOW					0xFF00			// HWORD Mask Low

#define D_SD_B_MASK_HI						0x0F			// BYTE Mask High
#define D_SD_B_MASK_LOW						0xF0			// BYTE Mask Low

#define D_SD_MASK_0F						0x0F			// BYTE Mask High

#define D_SD_NULL							0				// NULL

/*----------------------------------------------------------
	SDIO Registers											
------------------------------------------------------------
*/
#define D_SD_REG_SYS_ADDR			IO_SDXC31.SYSADDR							// System Address Register
#define D_SD_REG_BLK_SIZE			IO_SDXC31.BLKINFO.hword.BlockSize			// Block Size Register
#define D_SD_REG_BLK_CNT			IO_SDXC31.BLKINFO.hword.BlockCount			// Block Count Register
#define D_SD_REG_ARG1_0				IO_SDXC31.ARG1_0							// Argument1_0 Register
#define D_SD_REG_CMD_TRNS			IO_SDXC31.TRNSINFO.word						// Command Register & Transfer Mode Register
#define D_SD_REG_TRNS_MODE			IO_SDXC31.TRNSINFO.hword.TransferMode		// Transfer Mode Register
#define D_SD_REG_CMD				IO_SDXC31.TRNSINFO.hword.Command			// Command Register
#define D_SD_REG_RESP1_0			IO_SDXC31.RESP1_0							// Response1_0 Register
#define D_SD_REG_RESP3_2			IO_SDXC31.RESP3_2							// Response3_2 Register
#define D_SD_REG_RESP5_4			IO_SDXC31.RESP5_4							// Response5_4 Register
#define D_SD_REG_RESP7_6			IO_SDXC31.RESP7_6							// Response7_6 Register
#define D_SD_REG_BUF_DAT			IO_SDXC31.BUFDAT							// Buffer Data Port Register
#define D_SD_REG_PRE_STAT			IO_SDXC31.PREST.word						// Present State Register
#define D_SD_REG_HOST_CTRL			IO_SDXC31.CTRL1.byte.HostControl1			// Host Control1 Register
#define D_SD_REG_PWR_CTRL			IO_SDXC31.CTRL1.byte.PowerControl			// Power Control Register
#define D_SD_REG_BLK_GAP_CTRL		IO_SDXC31.CTRL1.byte.BlockGapControl		// Block Gap Control Register
#define D_SD_REG_WAKEUP_CTRL		IO_SDXC31.CTRL1.byte.WakeupControl			// Wakeup Control Register
#define D_SD_REG_CLK_CTRL			IO_SDXC31.CTRL3.byte.ClockControl			// Clock Control Register
#define D_SD_REG_TOUT_CTRL			IO_SDXC31.CTRL3.byte.TimeoutControl			// Timeout Control Register
#define D_SD_REG_SOFT_RST			IO_SDXC31.CTRL3.byte.SoftReset				// Software Reset Register
#define D_SD_REG_EINT_ST_NINT_ST	IO_SDXC31.INTST.word						// Error Interrupt Status Register & Normal Interrupt Status Register
#define D_SD_REG_NINT_ST			IO_SDXC31.INTST.hword.NIntStatus			// Normal Interrupt Status Register
#define D_SD_REG_EINT_ST			IO_SDXC31.INTST.hword.EIntStatus			// Error Interrupt Status Register
#define D_SD_REG_NINT_ST_EN			IO_SDXC31.INTSTENB.hword.NIntStatusEnable	// Normal Interrupt Status Enable Register
#define D_SD_REG_EINT_ST_EN			IO_SDXC31.INTSTENB.hword.EIntStatusEnable	// Error Interrupt Status Enable Register
#define D_SD_REG_NINT_SIG_EN		IO_SDXC31.INTSIGENB.hword.NIntSignalEnable	// Normal Interrupt Signal Enable Register
#define D_SD_REG_EINT_SIG_EN		IO_SDXC31.INTSIGENB.hword.EIntSignalEnable	// Error Interrupt Signal Enable Register
#define D_SD_REG_ACMD12E_ST			IO_SDXC31.CTRL2.hword.ACMDEStatus			// Auto CMD Error Status Register
#define D_SD_REG_HOST_CTRL2			IO_SDXC31.CTRL2.hword.HostControl2			// Host Control2 Register
#define D_SD_REG_CAPA0				IO_SDXC31.CAP1_0.hword.Capa0				// Capabilities0 Register
#define D_SD_REG_CAPA1				IO_SDXC31.CAP1_0.hword.Capa1				// Capabilities1 Register
#define D_SD_REG_MAX_CUR_CAPA0		IO_SDXC31.MCURCAP1_0.hword.MaxCurrentCapa0	// Maximum Current Capabilities0 Register
#define D_SD_REG_MAX_CUR_CAPA1		IO_SDXC31.MCURCAP1_0.hword.MaxCurrentCapa1	// Maximum Current Capabilities1 Register
#define D_SD_REG_FRC_EVT_ACMD		IO_SDXC31.hword.ForceEventForACMD			// Force Eveent for Auto CMD Register
#define D_SD_REG_FRC_EVT_ERR_INT	IO_SDXC31.FOREVT.hword.ForceEventForErrInt	// Force Event for Error Interrupt Register
#define D_SD_REG_ADMA_ERR_ST		IO_SDXC31.ADMAEST.word						// ADMA Error Status Register
#define D_SD_REG_ADMA_SYS_ADDR		IO_SDXC31.ADMASYSADDR_L						// ADMA System Address Register (Low)
#define D_SD_REG_PRE_VAL_INIT		IO_SDXC31.PREVAL1.hword.PresetValueForInit	// Preset Value for Initialize
#define D_SD_REG_PRE_VAL_DS			IO_SDXC31.PREVAL1.hword.PresetValueForDS	// Preset Value for Default Speed
#define D_SD_REG_PRE_VAL_HS			IO_SDXC31.PREVAL2.hword.PresetValueForHS	// Preset Value for High Speed
#define D_SD_REG_PRE_VAL_SDR12		IO_SDXC31.PREVAL2.hword.PresetValueForSDR12	// Preset Value for SDR12
#define D_SD_REG_PRE_VAL_SDR25		IO_SDXC31.PREVAL3.hword.PresetValueForSDR25	// Preset Value for SDR25
#define D_SD_REG_PRE_VAL_SDR50		IO_SDXC31.PREVAL3.hword.PresetValueForSDR50	// Preset Value for SDR50
#define D_SD_REG_PRE_VAL_SDR104		IO_SDXC31.PREVAL4.hword.PresetValueForSDR104	// Preset Value for SDR104
#define D_SD_REG_PRE_VAL_DDR50		IO_SDXC31.PREVAL4.hword.PresetValueForDDR50	// Preset Value for DDR50
#define D_SD_REG_SHR_BUS_CTRL		IO_SDXC31.SharedBusControl.word				// Shared Bus Control Register
#define D_SD_REG_SLOT_INT_ST		IO_SDXC31.HCTRLVER.hword.SIntStatus			// Slot Interrupt Status Register
#define D_SD_REG_HOST_CTRL_SPEC		IO_SDXC31.HCTRLVER.hword.HostContSPEC		// Host Controller Spec Register
#define D_SD_REG_HOST_CTRL_VENDER	IO_SDXC31.HCTRLVER.hword.HostContVENDER		// Host Controller Vender Register

#define D_SD_REG_AHB_CONFIG			IO_SDXC31.AHBCNF.word						// AHB Config Register
#define D_SD_REG_POWER_SWITCH		IO_SDXC31.POWSW.word						// Power Switching Register
#define D_SD_REG_TUNING_SET			IO_SDXC31.TUNSET.word						// Tuning Setting Register
#define D_SD_REG_TUNING_ST			IO_SDXC31.TUNST.word						// Tuning Status Register
#define D_SD_REG_IO_CTRL1			IO_SDXC31.IOCTRL1.word						// I/O Control1 Register
#define D_SD_REG_IO_CTRL2			IO_SDXC31.IOCTRL2.word						// I/O Control2 Register
#define D_SD_REG_POW_SW_INT_ST		IO_SDXC31.POWSWST.word						// Power switching Interrupt Status Register
#define D_SD_REG_POW_SW_INT_ST_EN	IO_SDXC31.POWSWSTENB.word					// Power switching Interrupt Status Enable Register
#define D_SD_REG_POW_SW_INT_SIG_EN	IO_SDXC31.POWSWSIGENB.word					// Power switching Interrupt Status Signal Enable Register
#define D_SD_REG_MMC_ESD_CTRL		IO_SDXC31.MMC_ESDCTRL.word					// MMC/eSD Control Register
#define D_SD_REG_MMC_WAITIRQ_CTRL1	IO_SDXC31.MMC_WAIT_IRQ_CTRL1.word			// MMC Wait IRQ Control1 Register
#define D_SD_REG_MMC_WAITIRQ_CTRL2	IO_SDXC31.MMC_WAIT_IRQ_CTRL2.word			// MMC Wait IRQ Control2 Register
#define D_SD_REG_MMC_RES_CHK		IO_SDXC31.MMC_RES_CHK.word					// MMC Response Check Bit Register
#define D_SD_REG_CARD_DETECT_SET	IO_SDXC31.CDSET.word						// Card Detect Setting Register (* ReadOnly)
#define D_SD_REG_BCLKSELCTRL		IO_SDXC31.BCLKSELCTRL.word					// BCLKSELControl Register
#define D_SD_REG_UHS1IOCS_PORTCTRL	IO_SDXC31.UHS1IOCS_PORTCTRL.word			// UHS1IOCSPortControl Register
#define D_SD_REG_SOFT_RESET			IO_SDXC31.SOFT_RESET.word					// SOFT_RESET Register
#define D_SD_REG_HDCLK_SEL			IO_SDXC31.HDCLK_SEL.word					// HDCLK_SEL Register
#define D_SD_REG_HPROT_SET			IO_SDXC31.HPORT_SET.word					// HPROT_SET Register
#define D_SD_REG_CLK_STOP			IO_SDXC31.CLK_STOP							// Clock Stop Register
#define D_SD_REG_WP_CD_LED_SET		IO_SDXC31.WP_CD_LED_SET.word				// WP_CD_LED_SET Register
#define D_SD_REG_RETUN_REQ			IO_SDXC31.RETUNING_REQ.word					// RE-TUNING_REQ Register
#define D_SD_REG_RETUN_REQ_CLR		IO_SDXC31.RETUNING_REQ_CLR.word				// RE-TUNING_REQ_CLR Register
#define D_SD_REG_CR_SET				IO_SDXC31.CR_SET.word						// CR_SET Register
#define D_SD_REG_CR_SET2			IO_SDXC31.CR_SET2.word						// CR_SET2 Register
#define D_SD_REG_CDR_SET			IO_SDXC31.CDR_SET.word						// CDR_SET Register
#define D_SD_REG_PV_SET1			IO_SDXC31.PV_SET1.word						// PV_SET1 Register
#define D_SD_REG_PV_SET2			IO_SDXC31.PV_SET2.word						// PV_SET2 Register
#define D_SD_REG_PV_SET3			IO_SDXC31.PV_SET3.word						// PV_SET3 Register
#define D_SD_REG_PV_SET4			IO_SDXC31.PV_SET4.word						// PV_SET4 Register
#define D_SD_REG_PV_SET5			IO_SDXC31.PV_SET5.word						// PV_SET5 Register

// HRS
#define D_SD2_REG_DEBOUNCE_SET		IO_SDXC32.HRS01.word						// Debounce Setting Register
#define D_SD2_REG_BUS_SET			IO_SDXC32.HRS02.word						// Bus Setting Register
#define D_SD2_REG_AXI_ERRINT		IO_SDXC32.HRS03.word						// AXI Error Interrupt
#define D_SD2_REG_LINK_PHY_IF		IO_SDXC32.HRS32.word						// LINK-PHY I/F signals
#define D_SD2_REG_PHY_PERF_ST		IO_SDXC32.HRS34.word						// PHY Performance Adjustment Signals Status 2 Register
#define D_SD2_REG_AXI_MASTER_OUT	IO_SDXC32.HRS37.word						// AXI Master Output Control Register
#define D_SD2_REG_U2IO_CTRL			IO_SDXC32.HRS38.word						// UHS-II I/O Control Register
#define D_SD2_REG_U2BCLKSEL_ST		IO_SDXC32.HRS39.word						// UHS-II BCLKSEL Status Register
#define D_SD2_REG_PHY_NRST			IO_SDXC32.HRS44.word						// PHY NRST Control
#define D_SD2_REG_U2PHY_COMPTEST	IO_SDXC32.HRS45.word						// UHS-II PHY Compliance Test Control
#define D_SD2_REG_U2PHY_CAPA_SET	IO_SDXC32.HRS46.word						// UHS-II PHY Capabilities Setting
#define D_SD2_REG_GENERIC			IO_SDXC32.HRS59.word						// Generic register

// CRS
#define D_SD2_REG_HOST_VER			IO_SDXC32.CRS63.hword.HostControllerVersion	// Host Controller Version
#define D_SD2_REG_SLOT_INT			IO_SDXC32.CRS63.hword.SlotInterruptStatus	// Slot Interrupt Status

// SRS
#define D_SD2_REG_RESP1_0			IO_SDXC32.SRS04								// Response1_0 Register
#define D_SD2_REG_RESP3_2			IO_SDXC32.SRS05								// Response3_2 Register
#define D_SD2_REG_RESP5_4			IO_SDXC32.SRS06								// Response5_4 Register
#define D_SD2_REG_RESP7_6			IO_SDXC32.SRS07								// Response7_6 Register
#define D_SD2_REG_BUF_DAT			IO_SDXC32.SRS08								// Buffer Data Port Register
#define D_SD2_REG_PRE_STAT			IO_SDXC32.SRS09.word						// Present State Register
#define D_SD2_REG_HOST_CTRL			IO_SDXC32.SRS10.byte.HostControl1			// Host Control1 Register
#define D_SD2_REG_PWR_CTRL			IO_SDXC32.SRS10.byte.PowerControl			// Power Control Register
#define D_SD2_REG_BLK_GAP_CTRL		IO_SDXC32.SRS10.byte.BlockGapControl		// Block Gap Control Register
#define D_SD2_REG_WAKEUP_CTRL		IO_SDXC32.SRS10.byte.WakeupControl			// Wakeup Control Register
#define D_SD2_REG_CLK_CTRL			IO_SDXC32.SRS11.byte.ClockControl			// Clock Control Register
#define D_SD2_REG_TOUT_CTRL			IO_SDXC32.SRS11.byte.TimeoutControl			// Timeout Control Register
#define D_SD2_REG_SOFT_RST			IO_SDXC32.SRS11.byte.SoftReset				// Software Reset Register
#define D_SD2_REG_EINT_ST_NINT_ST	IO_SDXC32.SRS12.word						// Error Interrupt Status Register & Normal Interrupt Status Register
#define D_SD2_REG_NINT_ST			IO_SDXC32.SRS12.hword.NIntStatus			// Normal Interrupt Status Register
#define D_SD2_REG_EINT_ST			IO_SDXC32.SRS12.hword.EIntStatus			// Error Interrupt Status Register
#define D_SD2_REG_NINT_ST_EN		IO_SDXC32.SRS13.hword.NIntStatusEnable		// Normal Interrupt Status Enable Register
#define D_SD2_REG_EINT_ST_EN		IO_SDXC32.SRS13.hword.EIntStatusEnable		// Error Interrupt Status Enable Register
#define D_SD2_REG_NINT_SIG_EN		IO_SDXC32.SRS14.hword.NIntSignalEnable		// Normal Interrupt Signal Enable Register
#define D_SD2_REG_EINT_SIG_EN		IO_SDXC32.SRS14.hword.EIntSignalEnable		// Error Interrupt Signal Enable Register
#define D_SD2_REG_ACMD12E_ST		IO_SDXC32.SRS15.hword.ACMDEStatus			// Auto CMD Error Status Register
#define D_SD2_REG_HOST_CTRL2		IO_SDXC32.SRS15.hword.HostControl2			// Host Control2 Register
#define D_SD2_REG_CAPA0				IO_SDXC32.SRS16.hword.Capa0					// Capabilities0 Register
#define D_SD2_REG_CAPA1				IO_SDXC32.SRS16.hword.Capa1					// Capabilities1 Register
#define D_SD2_REG_CAPA2				IO_SDXC32.SRS17.hword.Capa2					// Capabilities2 Register
#define D_SD2_REG_CAPA3				IO_SDXC32.SRS17.hword.Capa3					// Capabilities3 Register
#define D_SD2_REG_MAX_CUR_CAPA0		IO_SDXC32.SRS18.hword.MaxCurrentCapa0		// Maximum Current Capabilities0 Register
#define D_SD2_REG_MAX_CUR_CAPA1		IO_SDXC32.SRS18.hword.MaxCurrentCapa1		// Maximum Current Capabilities1 Register
#define D_SD2_REG_MAX_CUR_CAPA2		IO_SDXC32.SRS19.hword.MaxCurrentCapa2		// Maximum Current Capabilities2 Register
#define D_SD2_REG_ADMA_ERR_ST		IO_SDXC32.SRS21.word						// ADMA Error Status Register
#define D_SD2_REG_ADMA_SYS_ADDR		IO_SDXC32.SRS22								// ADMA System Address Register (Low)
#define D_SD2_REG_U2SDMA_BUF_BOUND	IO_SDXC32.SRS32.word						// UHS-II SDMA Buffer Boundary Register
#define D_SD2_REG_U2BLK_CNT			IO_SDXC32.SRS33								// UHS-II Block Count Register
#define D_SD2_REG_U2CMD_PACKET1		IO_SDXC32.SRS34								// UHS-II Command Packet1 Register
#define D_SD2_REG_U2CMD_PACKET2		IO_SDXC32.SRS35								// UHS-II Command Packet2 Register
#define D_SD2_REG_U2CMD_PACKET3		IO_SDXC32.SRS36								// UHS-II Command Packet3 Register
#define D_SD2_REG_U2CMD_PACKET4		IO_SDXC32.SRS37								// UHS-II Command Packet4 Register
#define D_SD2_REG_U2CMD_PACKET5		IO_SDXC32.SRS38								// UHS-II Command Packet5 Register
#define D_SD2_REG_U2CMD_TRNS		IO_SDXC32.SRS39.word						// UHS-II Command Register & Transfer Mode Register
#define D_SD2_REG_U2TRNS_MODE		IO_SDXC32.SRS39.hword.TransferMode			// UHS-II Transfer Mode Register
#define D_SD2_REG_U2CMD				IO_SDXC32.SRS39.hword.Command				// UHS-II Command Register
#define D_SD2_REG_U2REP_PACKET1		IO_SDXC32.SRS40								// UHS-II Response Packet1 Register
#define D_SD2_REG_U2REP_PACKET2		IO_SDXC32.SRS41								// UHS-II Response Packet2 Register
#define D_SD2_REG_U2REP_PACKET3		IO_SDXC32.SRS42								// UHS-II Response Packet3 Register
#define D_SD2_REG_U2REP_PACKET4		IO_SDXC32.SRS43								// UHS-II Response Packet4 Register
#define D_SD2_REG_U2REP_PACKET5		IO_SDXC32.SRS44								// UHS-II Response Packet5 Register
#define D_SD2_REG_U2MSG_SEL			IO_SDXC32.SRS45.word						// UHS-II MSG Select Register
#define D_SD2_REG_U2MSG				IO_SDXC32.SRS46								// UHS-II MSG Register
#define D_SD2_REG_U2TIM_CTRL		IO_SDXC32.SRS48.hword.TimerControl			// UHS-II Timer Control Register
#define D_SD2_REG_U2SOFT_RST		IO_SDXC32.SRS48.hword.SoftReset				// UHS-II Software Reset Register
#define D_SD2_REG_U2EINT_ST			IO_SDXC32.SRS49.word						// UHS-II Error Interrupt Status Register
#define D_SD2_REG_U2EINT_ST_EN		IO_SDXC32.SRS50.word						// UHS-II Error Interrupt Status Enable Register
#define D_SD2_REG_U2EINT_SIG_EN		IO_SDXC32.SRS51.word						// UHS-II Error Interrupt Signal Enable Register
#define D_SD2_REG_U2CAPA			IO_SDXC32.SRS56.hword.Capabilities			// Pointer for UHS-II Capabilities
#define D_SD2_REG_U2SET				IO_SDXC32.SRS56.hword.Settings				// Pointer for UHS-II Settings Registers
#define D_SD2_REG_TEST				IO_SDXC32.SRS57.hword.Test					// Pointer for Test Register

// ERS
#define D_SD2_REG_U2GEN_SET			IO_SDXC32.ERS00.word						// UHS-II General Settings
#define D_SD2_REG_U2PHY_SET			IO_SDXC32.ERS01.word						// UHS-II PHY Settings
#define D_SD2_REG_U2LINK_TRAN_SET1	IO_SDXC32.ERS02.word						// UHS-II LINK/TRAN Settings 1
#define D_SD2_REG_U2LINK_TRAN_SET2	IO_SDXC32.ERS03.word						// UHS-II LINK/TRAN Settings 2
#define D_SD2_REG_U2GEN_CAPA		IO_SDXC32.ERS04.word						// UHS-II General Capabilities
#define D_SD2_REG_U2PHY_CAPA		IO_SDXC32.ERS05.word						// UHS-II PHY Capabilities
#define D_SD2_REG_U2LINK_TRAN_CAPA1	IO_SDXC32.ERS06.word						// UHS-II LINK/TRAN Capabilities 1
#define D_SD2_REG_U2LINK_TRAN_CAPA2	IO_SDXC32.ERS07.word						// UHS-II LINK/TRAN Capabilities 2
#define D_SD2_REG_U2TEST			IO_SDXC32.ERS08.word						// UHS-II Test Register

/*----------------------------------------------------------
	SDIO Registers Bit 										
------------------------------------------------------------
------------------------------------
	Block Size Register				
----------------------------------*/
//---- bit Check -----
#define D_SD_BOUND							0x7000			// Buff Bound bit
#define D_SD_BLKSIZE						0x0FFF			// Block Size bit

//---- Status -----
#define D_SD_BOUND_512KB					0x7000			// Buff Bound 512KByte
#define D_SD_BOUND_256KB					0x6000			// Buff Bound 256KByte
#define D_SD_BOUND_128KB					0x5000			// Buff Bound 128KByte
#define D_SD_BOUND_64KB						0x4000			// Buff Bound 64KByte
#define D_SD_BOUND_32KB						0x3000			// Buff Bound 32KByte
#define D_SD_BOUND_16KB						0x2000			// Buff Bound 16KByte
#define D_SD_BOUND_8KB						0x1000			// Buff Bound 8KByte
#define D_SD_BOUND_4KB						0x0000			// Buff Bound 4KByte

#define D_SD_BLKSIZE_2048B					0x0800			// Buff Bound 2048KByte
#define D_SD_BLKSIZE_1024B					0x0400			// Buff Bound 1024KByte
#define D_SD_BLKSIZE_512B					0x0200			// Buff Bound 512KByte

/*----------------------------------
	Argument Register				
----------------------------------*/

//---- Parameter -----
#define	D_SD_CHK_PATTERN					0xAA			// Check Pattern for SD 2.0 Command8

/*----------------------------------
	Transfer Mode Register			
----------------------------------*/
//---- bit Check -----
#define D_SD_MSBS							0x0020			// Multi/Single Block Select bit
#define D_SD_DTDS							0x0010			// Data Transfer Direction Select bit
#define D_SD_ACMDE							0x000C			// ACMD12,23 Enable bit
#define D_SD_BCE							0x0002			// Block Count Enable bit
#define D_SD_DMAE							0x0001			// DMA Enable bit

//---- Status -----
#define D_SD_MULTI_BLK						0x0020			// Multi Block Select bit
#define D_SD_SINGLE_BLK						0x0000			// Single Block Select bit

#define D_SD_DAT_TRNS_RD					0x0010			// Transfer Read
#define D_SD_DAT_TRNS_WR					0x0000			// Transfer Write

#define D_SD_ACMD23_ENB						0x0008			// ACMD23 Enable
#define D_SD_ACMD12_ENB						0x0004			// ACMD12 Enable
#define D_SD_ACMD_DIB						0x0000			// ACMD Disable

#define D_SD_BLK_CNT_ENB					0x0002			// BCE(Block Count Enable
#define D_SD_BLK_CNT_DIB					0x0000			// BCE(Block Count Disable

#define D_SD_DMA_ENB						0x0001			// DMA Enable
#define D_SD_DMA_DIB						0x0000			// DMA Disable

/*----------------------------------
	Command Register				
----------------------------------*/
//---- bit Check -----
#define D_SD_CMDIND							0x3F00			// Command Index bit
#define D_SD_CMDT							0x00C0			// Command Type bit
#define D_SD_DPS							0x0020			// Data Present Select bit
#define D_SD_CMDICE							0x0010			// Command Index Check Enable bit
#define D_SD_CCRCCE							0x0008			// Command CRC Check Enable bit
#define D_SD_RTS							0x0003			// Response Type Select bit

//---- Status -----
#define D_SD_ABORT_CMD12					0x00C0			// Command Type (Abort:CMD12)
#define D_SD_RESUME_CMD52					0x0080			// Command Type (Resume:CMD52 for writing Function Select in CCCR)
#define D_SD_SUSPEND_CMD52					0x0040			// Command Type (Suspend:CMD52 for writing Bus Suspend in CCCR)

#define D_SD_DAT_PESENT						0x0020			// Data Present
#define D_SD_DAT_NO_PRESENT					0x0000			// Data No Present

#define D_SD_CMD_IND_CHK_ENB				0x0010			// Command Index Check Enable
#define D_SD_CMD_IND_CHK_DIB				0x0000			// Command Index Check Disable

#define D_SD_CMD_CRC_CHK_ENB				0x0008			// Command CRC Check Enable
#define D_SD_CMD_CRC_CHK_DIB				0x0000			// Command CRC Check Disable

#define D_SD_RESP_LEN_48_BUSY				0x0003			// Response length 48 bit with busy
#define D_SD_RESP_LEN_48_NO BUSY			0x0002			// Response length 48 bit with no busy
#define D_SD_RESP_LEN_136					0x0001			// Response length 136bit
#define D_SD_NO_RESP						0x0000			// No Response

/*----------------------------------
	Response1_0						
			3_2						
			5_4						
			7_6 Register			
----------------------------------*/
//---- Status -----
#define D_SD_NO_RESP_DAT					0x00000000		// No Response

/*----------------------------------
	Present State Register			
----------------------------------*/
//---- bit Check -----
#define D_SD_U2DET							0x80000000		// STB.L Detection bit (SPEC4.x only)
#define D_SD_U2SYN							0x40000000		// Lane Synchronization bit (SPEC4.x only)
#define D_SD_U2DOR							0x20000000		// In Dormant State bit (SPEC4.x only)
#define D_SD_CMDLSL							0x01000000		// CMD Line Signal Level bit
#define D_SD_DATSL							0x00F00000		// DAT[3:0] Line Signal Level bit
#define D_SD_WPSPL							0x00080000		// Write Protect Switch Pin Level(invert) bit
#define D_SD_CDPL							0x00040000		// Card Detect Pin Level(invert) bit
#define D_SD_CSS							0x00020000		// Card State Stable bit
#define D_SD_CI								0x00010000		// Card Inserted bit
#define D_SD_BRE							0x00000800		// Buffer Read Enable bit
#define D_SD_BWE							0x00000400		// Buffer Write Enable bit
#define D_SD_RTA							0x00000200		// Read Transfer Active bit
#define D_SD_WTA							0x00000100		// Write Transfer Active bit
#define D_SD_RTREQ							0x00000008		// Re-Tuning Request bit
#define D_SD_DATLA							0x00000004		// DAT Line Active
#define D_SD_CIHDAT							0x00000002		// Command InHibit DAT bit
#define D_SD_CIHCMD							0x00000001		// Command InHibit CMD bit

//---- Status -----
#define D_SD_CMD_LINE_H						0x01000000		// CMD Line Signal Level High
#define D_SD_CMD_LINE_L						0x00000000		// CMD Line Signal Level Low

#define D_SD_DATA_LINE_ALL_H				0x00F00000		// DAT[3:0] Line Signal Level ALL High
#define D_SD_DATA_LINE_ALL_L				0x00000000		// DAT[3:0] Line Signal Level ALL Low

#define D_SD_WR_PROTECT_OFF					0x00080000		// Write Protect Switch OFF
#define D_SD_WR_PROTECT_ON					0x00000000		// Write Protect Switch ON

#define D_SD_CARD_INS						0x00010000		// Card Inserted
#define D_SD_CARD_REMOVE					0x00000000		// Card Remove

#define D_SD_BUFF_RD_ENB					0x00000800		// Buffer Read Enable
#define D_SD_BUFF_RD_DIB					0x00000000		// Buffer Read Disable

#define D_SD_BUFF_WR_ENB					0x00000400		// Buffer Write Enable
#define D_SD_BUFF_WR_DIB					0x00000000		// Buffer Write Disable

#define D_SD_RD_TRNS_ACT					0x00000200		// Read Transfer Active
#define D_SD_RD_TRNS_COMP					0x00000000		// Read Transfer Complete

#define D_SD_WR_TRNS_ACT					0x00000100		// Write Transfer Active
#define D_SD_WR_TRNS_COMP					0x00000000		// Write Transfer Complete

#define D_SD_TUNE_REQ						0x00000008		// re-tuning request
#define D_SD_TUNE_FIX						0x00000000		// tuning fixed

#define D_SD_DAT_LINE_USE					0x00000004		// DAT Line Use
#define D_SD_DAT_LINE_NOT_USE				0x00000000		// DAT Line Not Use

#define D_SD_CMD_INHB_DAT_DIB				0x00000002		// Command InHibit DAT Disable
#define D_SD_CMD_INHB_DAT_ENB				0x00000000		// Command InHibit DAT Enable

#define D_SD_CMD_INHB_CMD_DIB				0x00000001		// Command InHibit  CMD All Output Disable
#define D_SD_CMD_INHB_CMD_ENB				0x00000000		// Command InHibit  CMD Output Enable

/*----------------------------------
	Host Control1 Register			
----------------------------------*/
//---- bit Check -----
#define D_SD_BUS_8MODE						0x20			// bus 8		(dummy:macro not support)
#define D_SD_DMAS							0x18			// DMA Select bit
#define D_SD_HSE							0x04			// High Speed Enable bit
#define D_SD_DTW							0x02			// Data Transfer Width bit
#define D_SD_LEDX							0x01			// LEDX Control bit

//---- Status -----
#define D_SD_ADMA32_2						0x10			// ADMA 32-2	(dummy:macro not support)
#define D_SD_ADMA32_1						0x08			// ADMA 32-1
#define D_SD_ADMA_32						0x10			// ADMA2 32bit
#define D_SD_SDMA							0x00			// SDMA

#define D_SD_HI_SPEED						0x04			// High Speed
#define D_SD_DEF_SPEED						0x00			// Deault Speed

#define D_SD_BUS_4BIT						0x02			// Data Transfer Width 4bit
#define D_SD_BUS_1BIT						0x00			// Data Transfer Width 1bit

#define D_SD_LEDX_L							0x01			// LEDX Control Low Output
#define D_SD_LEDX_H							0x00			// LEDX Control High Output

/*----------------------------------
	Power Control Control Register	
----------------------------------*/
//---- bit Check -----
#define D_SD_SDBVS							0x0E			// SD Bus Voltage Select bit
#define D_SD_SDBP							0x01			// SD Bus Power bit
#define D_SD_SDBVS2							0xE0			// SD Bus Voltage Select for VDD2 bit (SPEC4.x only)
#define D_SD_SDBP2							0x10			// SD Bus Power for VDD2 bit (SPEC4.x only)

//---- Status -----
#define D_SD_BUS_VOLT_33					0x0E			// Bus Voltage 3.3V
#define D_SD_BUS_VOLT_30					0x0C			// Bus Voltage 3.0V
#define D_SD_BUS_VOLT_18					0x0A			// Bus Voltage 1.8V
#define D_SD_BUS2_VOLT_18					0xA0			// Bus Voltage 1.8V for VDD2 (SPEC4.x only)

#define D_SD_BUS_POW_ON						0x01			// SD Bus Power ON
#define D_SD_BUS_POW_OFF					0x00			// SD Bus Power OFF
#define D_SD_BUS2_POW_ON					0x10			// SD Bus Power ON  for VDD2 (SPEC4.x only)
#define D_SD_BUS2_POW_OFF					0x00			// SD Bus Power OFF for VDD2 (SPEC4.x only)

/*----------------------------------
	Block Gap Control Register		
----------------------------------*/
//---- bit Check -----
#define D_SD_IBG							0x08			// Interrupt at Block Gap bit
#define D_SD_RWC							0x04			// Read Wait Control bit
#define D_SD_CR								0x02			// Continue Request bit
#define D_SD_SBGR							0x01			// Stop at Block Gap Request bit

//---- Status -----
#define D_SD_INT_ENB						0x08			// Interrupt Enable
#define D_SD_INT_DIB						0x00			// Interrupt Disable

#define D_SD_RD_WAIT_ENB					0x04			// Read Wait Control Enabel
#define D_SD_RD_WAIT_DIB					0x00			// Read Wait Control Disable

#define D_SD_CR_RESTART						0x02			// Continue Request Restart
#define D_SD_CR_NOT_AFFECT					0x00			// Continue Request Not Affect

#define D_SD_SBGR_STOP						0x01			// Stop at Block Gap Request STOP
#define D_SD_SBGR_TRANS						0x00			// Stop at Block Gap Request Transfer

/*----------------------------------
	Wakeup Control Register			
----------------------------------*/
//---- bit Check -----
#define D_SD_WEESCR							0x04			// Wakeup Event Enable of SD Card Removal bit
#define D_SD_WEESCI							0x02			// Wakeup Event Enable of SD Card Insersion bit
#define D_SD_WEESCIR						0x01			// Wakeup Event Enable of SD Card Interrupt bit

//---- Status -----
#define D_SD_REMOVE_WAKEUP_ENB				0x04			// Wakeup Event Enable of SD Card Removal
#define D_SD_REMOVE_WAKEUP_DIB				0x00			// Wakeup Event Disable of SD Card Removal

#define D_SD_INS_WAKEUP_ENB					0x02			// Wakeup Event Enable of SD Card Insersion
#define D_SD_INS_WAKEUP_DIB					0x00			// Wakeup Event Disable of SD Card Insersion

#define D_SD_INT_WAKEUP_ENB					0x01			// Wakeup Event Enable of SD Card Interrupt
#define D_SD_INT_WAKEUP_DIB					0x00			// Wakeup Event Disable of SD Card Interrupt

/*----------------------------------
	Clock Control Register			
----------------------------------*/
//---- bit Check -----
#define D_SD_SDICLK							0xFF00			// SDCLK frequency select bit
#define D_SD_UBSDICLK						0x00C0			// Upper Bits of SDCLK Frequency Select bit
#define D_SD_CGS							0x0020			// Clock Generator Select bit
#define D_SD_SDCE							0x0004			// SD Clock Enable bit
#define D_SD_ICS							0x0002			// Internal Clock Stable bit
#define D_SD_ICE							0x0001			// Internal Clock Enable bit

//---- Status -----
#define D_SD_BCLK_DIV_256					0x8000			// SD Base Clock frequency 1/256
#define D_SD_BCLK_DIV_128					0x4000			// SD Base Clock frequency 1/128
#define D_SD_BCLK_DIV_64					0x2000			// SD Base Clock frequency 1/64
#define D_SD_BCLK_DIV_32					0x1000			// SD Base Clock frequency 1/32
#define D_SD_BCLK_DIV_16					0x0800			// SD Base Clock frequency 1/16
#define D_SD_BCLK_DIV_8						0x0400			// SD Base Clock frequency 1/8
#define D_SD_BCLK_DIV_4						0x0200			// SD Base Clock frequency 1/4
#define D_SD_BCLK_DIV_2						0x0100			// SD Base Clock frequency 1/2
#define D_SD_BCLK_DIV_1						0x0000			// SD Base Clock frequency 1/1

#define D_SD_CLK_OUT_ENB					0x0004			// SD Clock Enable
#define D_SD_CLK_OUT_DIB					0x0000			// SD Clock Disable

#define	D_SD_OSC_STB						0x0002			// Internal Clock Stable
#define	D_SD_OSC_NOT_STB					0x0000			// Internal Clock Not Stable

#define	D_SD_OSC_ENB						0x0001			// Internal Clock Enable
#define	D_SD_OSC_DIB						0x0000			// Internal Clock Disable

/*----------------------------------
	Timeout Control Register		
----------------------------------*/
//---- bit Check -----
#define D_SD_DTCV							0x0F			// Data Timeout Counter Value bit

//---- Status -----
#define	D_SD_TIM_OUTC_27					0x0E			// Data Timeout Counter 2 exponentiation 27
#define	D_SD_TIM_OUTC_26					0x0D			// Data Timeout Counter 2 exponentiation 26
#define	D_SD_TIM_OUTC_25					0x0C			// Data Timeout Counter 2 exponentiation 25
#define	D_SD_TIM_OUTC_24					0x0B			// Data Timeout Counter 2 exponentiation 24
#define	D_SD_TIM_OUTC_23					0x0A			// Data Timeout Counter 2 exponentiation 23
#define	D_SD_TIM_OUTC_22					0x09			// Data Timeout Counter 2 exponentiation 22
#define	D_SD_TIM_OUTC_21					0x08			// Data Timeout Counter 2 exponentiation 21
#define	D_SD_TIM_OUTC_20					0x07			// Data Timeout Counter 2 exponentiation 20
#define	D_SD_TIM_OUTC_19					0x06			// Data Timeout Counter 2 exponentiation 19
#define	D_SD_TIM_OUTC_18					0x05			// Data Timeout Counter 2 exponentiation 18
#define	D_SD_TIM_OUTC_17					0x04			// Data Timeout Counter 2 exponentiation 17
#define	D_SD_TIM_OUTC_16					0x03			// Data Timeout Counter 2 exponentiation 16
#define	D_SD_TIM_OUTC_15					0x02			// Data Timeout Counter 2 exponentiation 15
#define	D_SD_TIM_OUTC_14					0x01			// Data Timeout Counter 2 exponentiation 14
#define	D_SD_TIM_OUTC_13					0x00			// Data Timeout Counter 2 exponentiation 13

/*----------------------------------
	Software Reset Register			
----------------------------------*/
//---- bit Check -----
#define D_SD_SRDL							0x04			// Software Reset for DAT Line bit
#define D_SD_SRCL							0x02			// Software Reset for CMD Line bit
#define D_SD_SRA							0x01			// Software Reset for All bit

//---- Status -----
#define D_SD_DAT_LINE_RESET					0x04			// Software Reset for DAT Line
#define D_SD_DAT_LINE_RESET_CLR				0x00			// Software Not Reset for DAT Line

#define D_SD_CMD_LINE_RESET					0x02			// Software Reset for CMD Line
#define D_SD_CMD_LINE_RESET_CLR				0x00			// Software Not Reset for CMD Line

#define D_SD_ALL_RESET						0x01			// Software Reset for All
#define D_SD_ALL_RESET_CLR					0x00			// Software Not Reset for All

/*----------------------------------
	Normal Interrupt Status Register
	Normal Interrput Status Enable Register
	Normal Interrput Signal Enable Register
----------------------------------*/
//---- bit Check -----
#define D_SD_EI								0x8000			// Error Interrupt bit
#define D_SD_RTE							0x1000			// Re-Tuning Event
#define D_SD_INTC							0x0800			// INT_C
#define D_SD_INTB							0x0400			// INT_B
#define D_SD_INTA							0x0200			// INT_A
#define D_SD_CINT							0x0100			// Card Interrupt bit
#define D_SD_CRE							0x0080			// Card Removal bit
#define D_SD_CIS							0x0040			// Card Insertion bit
#define D_SD_BRR							0x0020			// Buffer Read Ready bit
#define D_SD_BWR							0x0010			// Buffer Write Ready bit
#define D_SD_DI								0x0008			// DM Interrupt bit
#define D_SD_BGE							0x0004			// Block Gap Event bit
#define D_SD_TC								0x0002			// Transfer Complete bit
#define D_SD_CC								0x0001			// Command Complete bit

#define D_SD_TC_CC							(D_SD_TC | D_SD_CC)					// Transfer and Command Complete
#define D_SD_BRR_CC							(D_SD_BRR | D_SD_CC)				// Buffer Read Ready and Command Complete
#define D_SD_BWR_CC							(D_SD_BWR | D_SD_CC)				// Buffer Write Ready and Command Complete
#define D_SD_DI_TC							(D_SD_DI | D_SD_TC)					// DMA and Transfer Complete
#define D_SD_DI_TC_CC						(D_SD_DI | D_SD_TC | D_SD_CC)		// DMA and Transfer Complete and Command Complete

//---- Status -----
#define D_SD_ERR_INT						0x8000			// Error Interrupt
#define D_SD_NO_ERR_INT						0x0000			// Error No Interrupt

#define D_SD_RE_TUNE_INT					0x1000			// Re-Tuning Event Interrupt
#define D_SD_RE_TUNE_NO_INT					0x0000			// Re-Tuning Event No Interrupt

#define D_SD_CD_INT							0x0100			// Card Interrupt
#define D_SD_CD_NO_INT						0x0000			// Card No Interrupt

#define D_SD_CD_REMOVE_INT					0x0080			// Card Removal Interruput
#define D_SD_CD_REMOVE_NO_INT				0x0000			// Card Removal No Interruput

#define D_SD_CD_INS_INT						0x0040			// Card Insert Interrupt
#define D_SD_CD_INS_NO_INT					0x0000			// Card Insert No Interrupt

#define D_SD_BUF_RD_RDY_INT					0x0020			// Buffer Read Ready
#define D_SD_BUF_RD_RDY_NO_INT				0x0000			// Buffer No Read Ready

#define D_SD_BUF_WR_RDY_INT					0x0010			// Buffer Write Ready
#define D_SD_BUF_WR_RDY_NO_INT				0x0000			// Buffer Write No Ready

#define D_SD_DMA_INT						0x0008			// DMA Interrupt
#define D_SD_DMA_NO_INT						0x0000			// DMA No Interrupt

#define D_SD_BLK_GAP_EVENT					0x0004			// Block Gap Event
#define D_SD_BLK_GAP_NO_EVENT				0x0000			// Block Gap No Event

#define D_SD_TRNS_COMP						0x0002			// Transfer Complete
#define D_SD_TRNS_NO_COMP					0x0000			// Transfer No Complete

#define D_SD_CMD_COMP						0x0001			// Command Complete
#define D_SD_CMD_NO_COMP					0x0000			// Command No Complete

/*----------------------------------
	Error Interrupt Status Register
	Error Interrput Status Enable Register
	Error Interrput Signal Enable Register
----------------------------------*/
//---- bit Check -----
#define D_SD_VEN3							0x8000			// Vender-3 Error	(dummy:macro not support)
#define D_SD_VEN2							0x4000			// Vender-2 Error	(dummy:macro not support)
#define D_SD_VEN1							0x2000			// Vender-1 Error	(dummy:macro not support)
#define D_SD_VEN0							0x1000			// Vender-0 Error	(dummy:macro not support)

#define D_SD_AME							0x4000			// AHB Master Error
#define D_SD_AC19E							0x2000			// Auto CMD19 Error
#define D_SD_BAE							0x1000			// Boot Acknowledge Error
#define D_SD_TE								0x0400			// Tuning Error bit
#define D_SD_ADMA							0x0200			// ADMA Error

#define D_SD_ACE							0x0100			// ACMD12 Error bit
#define D_SD_CLE							0x0080			// Current Limit Error bit
#define D_SD_DEBE							0x0040			// Data End Bit Error bit
#define D_SD_DCE							0x0020			// Data CRC Error bit
#define D_SD_DTE							0x0010			// Data Timeout Error bit
#define D_SD_CIE							0x0008			// Command Index Error bit
#define D_SD_CEBE							0x0004			// Command End Bit Error bit
#define D_SD_CCE							0x0002			// Command CRC Error bit
#define D_SD_CTE							0x0001			// Command Timeout Error bit

#define D_SD_EINT_ALL						0xF3FF			// Error Status All
#define D_SD_EINT_CMD						0x000F			// Command Error Status
#define D_SD_EINT_DAT						0x0070			// Data Error Status
#define D_SD_EINT_CONF						0x0003			// Command CRC/Timeout Error

#define D_SD_EINT_DMA						(D_SD_VEN0 | D_SD_ADMA | D_SD_AME)	// DMA Error Status
#define D_SD_EINT_DAT_CMD					(D_SD_EINT_DAT | D_SD_EINT_CMD)		// DAT/CMD Error Status
#define D_SD_EINT_DAT_ACE					(D_SD_EINT_DAT | D_SD_ACE)			// DAT/AutoCMD12 Error Status
#define D_SD_EINT_DAT_DMA					(D_SD_EINT_DAT | D_SD_EINT_DMA)		// DAT/DMA Error Status
#define D_SD_EINT_DAT_DMA_ACE				(D_SD_EINT_DAT | D_SD_EINT_DMA | D_SD_ACE)					// DAT/DMA/AutoCMD12 Error Status
#define D_SD_EINT_DAT_DMA_CMD				(D_SD_EINT_DAT | D_SD_EINT_DMA | D_SD_EINT_CMD)				// DAT/DMA/CMD Error Status
#define D_SD_EINT_DAT_DMA_CMD_ACE			(D_SD_EINT_DAT | D_SD_EINT_DMA | D_SD_EINT_CMD | D_SD_ACE)	// DAT/DMA/CMD/AutoCMD12 Error Status

//---- Status -----
#define D_SD_AHB_MASTER_ERR					0x4000			// AHB Master Error
#define D_SD_AHB_MASTER_NO_ERR				0x0000			// AHB Master No Error

#define D_SD_AUTO_CMD19_ERR					0x2000			// Auto CMD19 Error
#define D_SD_AUTO_CMD19_NO_ERR				0x0000			// Auto CMD19 No Error

#define D_SD_BOOT_ACK_ERR					0x1000			// Boot Acknowledge Error
#define D_SD_BOOT_ACK_NO_ERR				0x0000			// Boot Acknowledge  No Error

#define D_SD_TUNE_ERR						0x0400			// Tuning Error
#define D_SD_TUNE_NO_ERR					0x0000			// Tuning No Error

#define D_SD_ADMA_ERR						0x0200			// ADMA Error
#define D_SD_ADMA_NO_ERR					0x0000			// ADMA No Error

#define D_SD_ACMD12_ERR						0x0100			// ACMD12 Error
#define D_SD_ACMD12_NO_ERR					0x0000			// ACMD12 No Error

#define D_SD_CURR_LIMT_ERR					0x0080			// Current Limit Error
#define D_SD_CURR_LIMT_NO_ERR				0x0000			// Current Limit No Error

#define D_SD_DAT_END_BIT_ERR				0x0040			// Data End Bit Error
#define D_SD_DAT_END_BIT_NO_ERR				0x0000			// Data End Bit No Error

#define D_SD_DAT_CRC_ERR					0x0020			// Data CRC Error
#define D_SD_DAT_CRC_NO_ERR					0x0000			// Data CRC No Error

#define D_SD_DAT_TM_OUT_ERR					0x0010			// Data Timeout Error
#define D_SD_DAT_TM_OUT_NO_ERR				0x0000			// Data Timeout No Error

#define D_SD_CMD_IND_ERR					0x0008			// Command Index Error
#define D_SD_CMD_IND_NO_ERR					0x0000			// Command Index No Error

#define D_SD_CMD_END_BIT_ERR				0x0004			// Command End Bit Error
#define D_SD_CMD_END_BIT_NO_ERR				0x0000			// Command End Bit No Error

#define D_SD_CMD_CRC_ERR					0x0002			// Command CRC Error
#define D_SD_CMD_CRC_NO_ERR					0x0000			// Command CRC No Error

#define D_SD_CMD_TM_OUT_ERR					0x0001			// Command Timeout Error
#define D_SD_CMD_TM_OUT_NO_ERR				0x0000			// Command Timeout No Error

/*----------------------------------
	ACMD12 Error Status Register	
----------------------------------*/
//---- bit Check -----
#define D_SD_CDACE							0x80			// CMC_wo_DAT not executed by ACMD12 Error bit
#define D_SD_ACIE							0x10			// ACMD12 Index Error bit
#define D_SD_ACEBE							0x08			// ACMD12 End Bit Error bit
#define D_SD_ACRCE							0x04			// ACMD12 Response CRC Error bit
#define D_SD_ACRTE							0x02			// ACMD12 Response Timeout Error bit
#define D_SD_ACEE							0x01			// ACMD12 not Executed Error bit

#define D_SD_ACRCE_ACRTE					(D_SD_ACRCE | D_SD_ACRTE)	// ACMD12 Response CRC Error,TimeOut Error bit

//---- Status -----
#define D_SD_CDACE_NOT_EXEUTE				0x80			// CMC_wo_DAT Not Executed by AMD12 Error
#define D_SD_CDACE_EXEUTE					0x00			// CMC_wo_DAT Executed by AMD12 Error

#define D_SD_ACIE_ERR						0x10			// ACMD12 Index Error
#define D_SD_ACIE_NO_ERR					0x00			// ACMD12 Index No Error

#define D_SD_ACEBE_ERR						0x08			// ACMD12 End Bit Error
#define D_SD_ACEBE_NO_ERR					0x00			// ACMD12 End Bit NO Error

#define D_SD_ACRCE_ERR						0x04			// ACMD12 Response CRC Error
#define D_SD_ACRCE_NO_ERR					0x00			// ACMD12 Response No CRC Error

#define D_SD_ACRTE_ERR						0x02			// ACMD12 Response Timeout Error
#define D_SD_ACRTE_NO_ERR					0x00			// ACMD12 Response No Timeout Error

#define D_SD_ACEE_ERR						0x01			// ACMD12 not Executed Error bit
#define D_SD_ACEE_NO_ERR					0x00			// ACMD12 not Executed No Error bit

#define D_SD_ACMD12_CONF_ERR				0x06			// ACMD12 Conflict Error
#define D_SD_ACMD12_RESP_CRC_ERR			0x04			// ACMD12 Response CRC Error
#define D_SD_ACMD12_RESP_TM_OUT_ERR			0x02			// ACMD12 Response Timeout Error
#define D_SD_ACMD12_NO_ERR_ST				0x00			// ACMD12 Response CRC,TimeOut No Error

/*----------------------------------
	Host Control2 Register			
----------------------------------*/
//---- bit Check -----
#define D_SD_PVE							0x8000			// Preset Value Enable bit
#define D_SD_AIE							0x4000			// Asynchronous Interrupt Enable bit
#define D_SD_A64B							0x2000			// 64-bit Addressing bit (SPEC4.x only)
#define D_SD_HV4E							0x1000			// Host Version 4.00 Enable bit (SPEC4.x only)
#define D_SD_U2IE							0x0100			// UHS-II Interface Enable bit (SPEC4.x only)

#define D_SD_SCS							0x0080			// Sampling Clock Select
#define D_SD_ET								0x0040			// Execute Tuning
#define D_SD_DTS							0x0030			// Driver Strength Select
#define D_SD_S18VE							0x0008			// 1.8V Signaling Enable
#define D_SD_UHSMS							0x0007			// UHS Mode Select

//---- Status -----
#define D_SD_PRE_VAL_ENB					0x8000			// Preset Value Enable
#define D_SD_PRE_VAL_DIS					0x0000			// Preset Value Disable

#define D_SD_ASYNC_INT_ENB					0x4000			// Asynchronous Interrupt Enable
#define D_SD_ASYNC_INT_DIS					0x0000			// Asynchronous Interrupt Disable

#define D_SD_A64B_VAL_ENB					0x2000			// 64-bit Addressing Enable (SPEC4.x only)
#define D_SD_A64B_VAL_DIS					0x0000			// 64-bit Addressing Disable (SPEC4.x only)

#define D_SD_HV4E_VAL_ENB					0x1000			// Host Version 4.00 Enable (SPEC4.x only)
#define D_SD_HV4E_VAL_DIS					0x0000			// Host Version 4.00 Disable (SPEC4.x only)

#define D_SD_U2IE_VAL_ENB					0x0100			// UHS-II Interface Enable (SPEC4.x only)
#define D_SD_U2IE_VAL_DIS					0x0000			// UHS-II Interface Disable (SPEC4.x only)

#define D_SD_SMP_CLK_TUNED					0x0080			// Tuned Clock
#define D_SD_SMP_CLK_FIXED					0x0000			// Fixed Clock

#define D_SD_TUNE_EXECT						0x0040			// Execute Tuning
#define D_SD_TUNE_NO_EXECT					0x0000			// Not Tuned or Tuning Completed
#define D_SD_TUNE_P8TRTR_MASK				0x000000ff		// 8PTR Mask
#define D_SD_TUNE_PHASE_SEL_ENB				0x00000100		// Tuning Phase Select Enable
#define D_SD_TUNE_PHASE0					0x00000001		// Tuning Phase0


#define D_SD_DRV_STR_TYPE_B					0x0000			// Driver Type B (Default)
#define D_SD_DRV_STR_TYPE_A					0x0010			// Driver Type A
#define D_SD_DRV_STR_TYPE_C					0x0020			// Driver Type C
#define D_SD_DRV_STR_TYPE_D					0x0030			// Driver Type D

#define D_SD_SIG_18V						0x0008			// 1.8V Signaling
#define D_SD_SIG_33V						0x0000			// 3.3V Signaling

#define D_SD_UHS_SDR12						0x0000			// SDR12
#define D_SD_UHS_SDR25						0x0001			// SDR25
#define D_SD_UHS_SDR50						0x0002			// SDR50
#define D_SD_UHS_SDR104						0x0003			// SDR104
#define D_SD_UHS_DDR50						0x0004			// DDR50
#define D_SD_UHS_UHS2						0x0007			// UHS-II (SPEC4.x only)

/*----------------------------------
	Capabilities0 Register			
----------------------------------*/
//---- bit Check -----
#define D_SD_CAPA0_BCFSC					0xFF00			// BCFSC bit
#define D_SD_CAPA0_TC						0x0080			// Timeout Clock unit bit
#define D_SD_CAPA0_TCF						0x003F			// TCF bit

//---- Status -----
#define D_SD_BCFSC_6BIT						0x3F00			// 6-bit Base Clock Frequency
#define D_SD_BCFSC_8BIT						0xFF00			// 8-bit Base Clock Frequency

#define D_SD_CAPA0_TC_MHZ					0x0080			// Timeout Clock unit = MHz
#define D_SD_CAPA0_TC_KHZ					0x0000			// Timeout Clock unit = KHz

/*----------------------------------
	Capabilities1 Register			
----------------------------------*/
//---- bit Check -----
#define D_SD_ST								0xC000			// Slot Type bit
#define D_SD_AIS							0x2000			// Asynchronous Interrupt Support bit
#define D_SD_SB64S							0x1000			// 64-bit System Bus Support bit
#define D_SD_VS18							0x0400			// Voltage Support for 1.8V bit
#define D_SD_VS30							0x0200			// Voltage Support for 3.0V bit
#define D_SD_VS33							0x0100			// Voltage Support for 3.3V bit
#define D_SD_SR								0x0080			// Suspend/Resume support bit
#define D_SD_DMA							0x0040			// DMA support bit
#define D_SD_HSS							0x0020			// High Speed Support bit
#define D_SD_ADMA_SUPP						0x0010			// ADMA Support bit			(dummy:macro not support)
#define D_SD_ADMA2_SUPP						0x0008			// ADMA2 Support bit
#define D_SD_BUSW8							0x0004			// Bus Witdh 8 Support bit	(dummy:macro not support)
#define D_SD_MBL							0x0003			// Max Block Length bit

//---- Status -----
#define D_SD_REMOVABLE_SLOT					0x0000			// Removable Card Slot
#define D_SD_EMBEDDED_SLOT					0x4000			// Embedded Slot for One Drive
#define D_SD_SHARED_BUS_SLOT				0x8000			// Shared Bus Slot

#define D_SD_ASYNC_INT_SUPP					0x2000			// Asynchronous Interrupt Support
#define D_SD_ASYNC_INT_NOT_SUPP				0x0000			// Asynchronous Interrupt Not Support

#define D_SD_64BUS_SUPP						0x1000			// 64-bit System Bus Support
#define D_SD_64BUS_NOT_SUPP					0x0000			// 64-bit System Bus Not Support

#define D_SD_VOLT_18						0x0400			// Voltage Support for 1.8V
#define D_SD_VOLT_NOT_18					0x0000			// Voltage Support for Not 1.8V

#define D_SD_VOLT_30						0x0200			// Voltage Support for 3.0V
#define D_SD_VOLT_NOT_30					0x0000			// Voltage Support for Not 3.0V

#define D_SD_VOLT_33						0x0100			// Voltage Support for 3.3V
#define D_SD_VOLT_NOT_33					0x0000			// Voltage Support for Not 3.3V

#define D_SD_SR_SUPP						0x0080			// Suspend/Resume support
#define D_SD_SR_NO_SUPP						0x0000			// Suspend/Resume No support

#define D_SD_DMA_SUPP						0x0040			// DMA Support
#define D_SD_DMA_NOT_SUPP					0x0000			// DMA Not Support

#define D_SD_HSS_SPUPP						0x0020			// High Speed Support
#define D_SD_HSS_NOT_SPUPP					0x0000			// High Speed Not Support

#define D_SD_MBL_2KB						0x0002			// Max Block Length 2KByte
#define D_SD_MBL_1KB						0x0001			// Max Block Length 1KByte
#define D_SD_MBL_512B						0x0000			// Max Block Length 512Byte

/*----------------------------------
	Capabilities2 Register			
----------------------------------*/
//---- bit Check -----
#define D_SD_RTM							0xC000			// Re-Tuning Modes bit
#define D_SD_UTSDR50						0x2000			// Use Tuning for SDR50 bit
#define D_SD_TCRT							0x0F00			// Timer Count for Re-Tuning bit
#define D_SD_DT_DS							0x0040			// Driver Type D Support bit
#define D_SD_DT_CS							0x0020			// Driver Type C Support bit
#define D_SD_DT_AS							0x0010			// Driver Type A Support bit
#define D_SD_UHS2S							0x0008			// UHS-II Support bit (SPEC4.x only)
#define D_SD_DDR50S							0x0004			// DDR50 Support bit
#define D_SD_SDR104S						0x0002			// SDR104 Support bit
#define D_SD_SDR50S							0x0001			// SDR50 Support bit

//---- Status -----
#define D_SD_RE_TUNE_MODE1					0x0000			// Re-Tuning Mode1
#define D_SD_RE_TUNE_MODE2					0x4000			// Re-Tuning Mode2
#define D_SD_RE_TUNE_MODE3					0x8000			// Re-Tuning Mode3

#define D_SD_TUNE_SDR50						0x2000			// SDR50 requires tuning
#define D_SD_NO_TUNE_SDR50					0x0000			// SDR50 does not require tuning

#define D_SD_DRV_TYPE_D_SUPP				0x0040			// Driver Type D Support
#define D_SD_DRV_TYPE_D_NOT_SUPP			0x0000			// Driver Type D Not Support

#define D_SD_DRV_TYPE_C_SUPP				0x0020			// Driver Type C Support
#define D_SD_DRV_TYPE_C_NOT_SUPP			0x0000			// Driver Type C Not Support

#define D_SD_DRV_TYPE_A_SUPP				0x0010			// Driver Type A Support
#define D_SD_DRV_TYPE_A_NOT_SUPP			0x0000			// Driver Type A Not Support

#define D_SD_UHS2_SUPP						0x0008			// UHS-II Support (SPEC4.x only)
#define D_SD_UHS2_NOT_SUPP					0x0000			// UHS-II Not Support (SPEC4.x only)

#define D_SD_DDR50_SUPP						0x0004			// DDR50 Support
#define D_SD_DDR50_NOT_SUPP					0x0000			// DDR50 Not Support

#define D_SD_SDR104_SUPP					0x0002			// SDR104 Support
#define D_SD_SDR104_NOT_SUPP				0x0000			// SDR104 Not Support

#define D_SD_SDR50_SUPP						0x0001			// SDR50 Support
#define D_SD_SDR50_NOT_SUPP					0x0000			// SDR50 Not Support

/*----------------------------------
	Capabilities3 Register			
----------------------------------*/
//---- bit Check -----
#define D_SD_CM								0x00FF			// Clock Multiplier bit

//---- Status -----

/*----------------------------------
	Maximum Current 				
		Capabilities0 Register		
----------------------------------*/
//---- bit Check -----
#define D_SD_MAX_CURR_CAPA0_MC30			0xFF00			// Max Current for 3.0V bit
#define D_SD_MAX_CURR_CAPA0_MC33			0x00FF			// Max Current for 3.3V bit

/*----------------------------------
	Maximum Current 				
		Capabilities1 Register		
----------------------------------*/
//---- bit Check -----
#define D_SD_MAX_CURR_CAPA1_MC18			0x00FF			// Max Current for 1.8V bit

/*----------------------------------
	Force Event Register			
		for Auto CMD Error Status	
----------------------------------*/
//---- bit Check -----
#define D_SD_FECNAC12E						0x0080			// Force Event for Command Not Issued By Auto CMD12 Error bit
#define D_SD_FEACIE							0x0010			// Force Event for Auto CMD Index Error bit
#define D_SD_FEACEBE						0x0008			// Force Event for Auto CMD End Bit Error bit
#define D_SD_FEACCRCE						0x0004			// Force Event for Auto CMD CRC Error bit
#define D_SD_FEACTE							0x0002			// Force Event for Auto CMD Timeout Error bit
#define D_SD_FEAC12NE						0x0001			// Force Event for Auto CMD12 Not Executed

//---- Status -----

/*----------------------------------
	Force Event Register			
		for Error Interrupt Status	
----------------------------------*/
//---- bit Check -----
#define D_SD_FEVSES							0xF000			// Force Event for Vendor Specific Error Status bit
#define D_SD_FEADMAE						0x0200			// Force Event for ADMA Error bit
#define D_SD_FEACMDE						0x0100			// Force Event for Auto CMD Error bit
#define D_SD_FECLE							0x0080			// Force Event for Current Limit Error bit
#define D_SD_FEDEBE							0x0040			// Force Event for Data End Bit Error bit
#define D_SD_FEDCRCE						0x0020			// Force Event for Data CRC Error bit
#define D_SD_FEDTE							0x0010			// Force Event for Data Timeout Error bit
#define D_SD_FECIE							0x0008			// Force Event for Command Index Error bit
#define D_SD_FECEBE							0x0004			// Force Event for Command End Bit Error bit
#define D_SD_FECCRCE						0x0002			// Force Event for Command CRC Error bit
#define D_SD_FECTE							0x0001			// Force Event for Command Timeout Error bit

//---- Status -----

/*----------------------------------
	ADMA Error Status Register	
----------------------------------*/
//---- bit Check -----
#define D_SD_ADMALME						0x00000004			// ADMA Length Mismatch Error bit
#define D_SD_ADMAES							0x00000003			// ADMA Error State bit

//---- Status -----
#define D_SD_ADMA_ERR_MASK					0x00000007			// ADMA Error Check

#define D_SD_ADMA_LEN_MISMATCH_ERR			0x00000004			// ADMA Length Mismatch Error
#define D_SD_ADMA_LEN_MISMATCH_NO_ERR		0x00000000			// ADMA Length Mismatch No Error

#define D_SD_ADMA_ERR_ST_STOP				0x00000000			// ADMA Error State STOP
#define D_SD_ADMA_ERR_ST_FDS				0x00000001			// ADMA Error State Fetch Descriptor
#define D_SD_ADMA_ERR_ST_TFR				0x00000003			// ADMA Error State Transfer Data

/*----------------------------------
	Slot Interrupt Status Register	
----------------------------------*/
//---- bit Check -----
#define D_SD_ISES7							0x80			// Interrupt Signal for Each Slot bit
#define D_SD_ISES6							0x40			// Interrupt Signal for Each Slot bit
#define D_SD_ISES5							0x20			// Interrupt Signal for Each Slot bit
#define D_SD_ISES4							0x10			// Interrupt Signal for Each Slot bit
#define D_SD_ISES3							0x08			// Interrupt Signal for Each Slot bit
#define D_SD_ISES2							0x04			// Interrupt Signal for Each Slot bit
#define D_SD_ISES1							0x02			// Interrupt Signal for Each Slot bit
#define D_SD_ISES0							0x01			// Interrupt Signal for Each Slot bit

//---- Status -----
#define D_SD_INT_WAKEUP						0x01			// Interrupt Signal for Each Slot INT or Wakeup

/*----------------------------------
	AHB Config Register				
----------------------------------*/
//---- bit Check -----
#define D_SD_SELECT_ENDIAN					0x0040			// Endian Select bit
#define D_SD_BSLOCK_SEL						0x0020			// Bus Lock Mode Select bit
#define D_SD_BSLOCK							0x0010			// Bus Lock Enable bit
#define D_SD_SIN_EN							0x0008			// Set Burst Type bit
#define D_SD_INCR_SEL						0x0007			// Set INCR Type bit

//---- Status -----
#define D_SD_SEL_BIG_ENDIAN					0x0040			// Select Big Endian
#define D_SD_SEL_LITTLE_ENDIAN				0x0000			// Select Little Endian
#define D_SD_BUS_LOCK_DMA					0x0020			// Bus Lock for 1 Packet(512Byte) Transfer
#define D_SD_BUS_LOCK_PACKET				0x0000			// Bus Lock for DMA Transfer
#define D_SD_BUS_LOCK_ON					0x0010			// Bus Lock Enable
#define D_SD_BUS_LOCK_OFF					0x0000			// Bus Lock Disable
#define D_SD_SEL_SINGLE						0x0008			// Use SINGLE
#define D_SD_SEL_INCR						0x0000			// Use INCR
#define D_SD_USE_INCR_4_8_16				0x0007			// Use INCR4, INCR8, INCR16
#define D_SD_USE_INCR_8_16					0x0006			// Use INCR8, INCR16
#define D_SD_USE_INCR_4_16					0x0005			// Use INCR4, INCR16
#define D_SD_USE_INCR_16					0x0004			// Use INCR16
#define D_SD_USE_INCR_4_8					0x0003			// Use INCR4, INCR8
#define D_SD_USE_INCR_8						0x0002			// Use INCR8
#define D_SD_USE_INCR_4						0x0001			// Use INCR4
#define D_SD_NOT_USE_INCR					0x0000			// No Use INCR4, INCR8, INCR16

/*----------------------------------
	Power Switching Register		
----------------------------------*/
//---- bit Check -----
#define D_SD_AUTO_POW_SW_EN					0x0001			// Auto Power Switching Enable bit
#define D_SD_IO_REG_SEL						0x0002			// I/O Register Selecting

//---- Status -----
#define D_SD_POW_SW_HARD					0x0001			// Power Switching for Hardware Control
#define D_SD_POW_SW_SOFT					0x0000			// Power Switching for Softwere Control

#define D_SD_SEL_IO_CTRL1					0x0002			// I/O Control1 Register select
#define D_SD_SEL_IO_CTRL2					0x0000			// I/O Control2 Register select

/*----------------------------------
	Tuning Setting Register			
----------------------------------*/
//---- bit Check -----
#define D_SD_DAT_TOUT_AUTO_RE_TUNE			0x0F000000		// Data Timeout Counter Value For Auto Re-Tuning
#define D_SD_CMD_CNF_CHK_SEL				0x00010000		// CMD Conflict Check Select bit
#define D_SD_RE_TUNE_RANGE_SEL				0x00001800		// Re-Tuning Range Select bit
#define D_SD_RE_TUNE_TAP_SEL				0x00000400		// Re-Tuning Phase Select bit
#define D_SD_TUNE_ERR_BORDER_SEL			0x00000200		// Tuning/Re-tuning Fail Border Select bit
#define D_SD_TUNE_PHASE_SEL					0x00000100		// Tuning/Re-tuning Phase Set bit
#define D_SD_TUNE_POINT_SEL					0x000000FF		// Tuning/Re-tuning Phase Select bit

//---- Status -----
#define D_SD_DTOUT_AUTO_RETUNE_2_27			0x0E000000		// Auto Re-Tuning Data Timeout Count SD_TOCLK_I * 2^27
#define D_SD_DTOUT_AUTO_RETUNE_2_26			0x0D000000		// Auto Re-Tuning Data Timeout Count SD_TOCLK_I * 2^26
#define D_SD_DTOUT_AUTO_RETUNE_2_25			0x0C000000		// Auto Re-Tuning Data Timeout Count SD_TOCLK_I * 2^25
#define D_SD_DTOUT_AUTO_RETUNE_2_24			0x0B000000		// Auto Re-Tuning Data Timeout Count SD_TOCLK_I * 2^24
#define D_SD_DTOUT_AUTO_RETUNE_2_23			0x0A000000		// Auto Re-Tuning Data Timeout Count SD_TOCLK_I * 2^23
#define D_SD_DTOUT_AUTO_RETUNE_2_22			0x09000000		// Auto Re-Tuning Data Timeout Count SD_TOCLK_I * 2^22
#define D_SD_DTOUT_AUTO_RETUNE_2_21			0x08000000		// Auto Re-Tuning Data Timeout Count SD_TOCLK_I * 2^21
#define D_SD_DTOUT_AUTO_RETUNE_2_20			0x07000000		// Auto Re-Tuning Data Timeout Count SD_TOCLK_I * 2^20
#define D_SD_DTOUT_AUTO_RETUNE_2_19			0x06000000		// Auto Re-Tuning Data Timeout Count SD_TOCLK_I * 2^19
#define D_SD_DTOUT_AUTO_RETUNE_2_18			0x05000000		// Auto Re-Tuning Data Timeout Count SD_TOCLK_I * 2^18
#define D_SD_DTOUT_AUTO_RETUNE_2_17			0x04000000		// Auto Re-Tuning Data Timeout Count SD_TOCLK_I * 2^17
#define D_SD_DTOUT_AUTO_RETUNE_2_16			0x03000000		// Auto Re-Tuning Data Timeout Count SD_TOCLK_I * 2^16
#define D_SD_DTOUT_AUTO_RETUNE_2_15			0x02000000		// Auto Re-Tuning Data Timeout Count SD_TOCLK_I * 2^15
#define D_SD_DTOUT_AUTO_RETUNE_2_14			0x01000000		// Auto Re-Tuning Data Timeout Count SD_TOCLK_I * 2^14
#define D_SD_DTOUT_AUTO_RETUNE_2_13			0x00000000		// Auto Re-Tuning Data Timeout Count SD_TOCLK_I * 2^13

#define D_SD_CMD_CNF_CHK_DIS				0x00010000		// CMD Bus Conflict Check Disable
#define D_SD_CMD_CNF_CHK_ENB				0x00000000		// CMD Bus Conflict Check Enable

#define D_SD_RE_TUNE_RANGE_3				0x00001000		// Re-Tuning Range +-3
#define D_SD_RE_TUNE_RANGE_2				0x00000800		// Re-Tuning Range +-2
#define D_SD_RE_TUNE_RANGE_1				0x00000000		// Re-Tuning Range +-1

#define D_SD_RE_TUNE_3_PHASE				0x00000400		// Re-Tuning 3 Phase Execution
#define D_SD_RE_TUNE_8_PHASE				0x00000000		// Re-Tuning 8 Phase Execution

#define D_SD_TUNE_BORDER_1_PHASE			0x00000200		// Tuning/Re-tuning Success More Than 1 Phase
#define D_SD_TUNE_BORDER_3_PHASE			0x00000000		// Tuning/Re-tuning Success More Than 3 Phase

#define D_SD_TUNE_PHASE_SEL_ENB				0x00000100		// Tuning/Re-tuning Phase Select Enable
#define D_SD_TUNE_PHASE_SEL_DIB				0x00000000		// Tuning/Re-tuning Phase Select Disable

#define D_SD_TUNE_POINT_7					0x00000080		// Select Tuning/Re-tuning Point 7 Phase
#define D_SD_TUNE_POINT_6					0x00000040		// Select Tuning/Re-tuning Point 6 Phase
#define D_SD_TUNE_POINT_5					0x00000020		// Select Tuning/Re-tuning Point 5 Phase
#define D_SD_TUNE_POINT_4					0x00000010		// Select Tuning/Re-tuning Point 4 Phase
#define D_SD_TUNE_POINT_3					0x00000008		// Select Tuning/Re-tuning Point 3 Phase
#define D_SD_TUNE_POINT_2					0x00000004		// Select Tuning/Re-tuning Point 2 Phase
#define D_SD_TUNE_POINT_1					0x00000002		// Select Tuning/Re-tuning Point 1 Phase
#define D_SD_TUNE_POINT_0					0x00000001		// Select Tuning/Re-tuning Point 0 Phase

/*----------------------------------
	Tuning Status Register			
----------------------------------*/
//---- bit Check -----
#define D_SD_PRE_TUNE_POINT					0x00FF0000		// Selected Tuning Point bit
#define D_SD_3_PHASE_RE_TUNE_RESULT			0x00000700		// 3 Phase Re-tuning Result bit
#define D_SD_8_PHASE_TUNE_RESULT			0x000000FF		// 8 Phase Tuning/Re-tuning Result bit

//---- Status -----
#define D_SD_PRE_TUNE_POINT_7				0x00800000		// 7 Phase Select
#define D_SD_PRE_TUNE_POINT_6				0x00400000		// 6 Phase Select
#define D_SD_PRE_TUNE_POINT_5				0x00200000		// 5 Phase Select
#define D_SD_PRE_TUNE_POINT_4				0x00100000		// 4 Phase Select
#define D_SD_PRE_TUNE_POINT_3				0x00080000		// 3 Phase Select
#define D_SD_PRE_TUNE_POINT_2				0x00040000		// 2 Phase Select
#define D_SD_PRE_TUNE_POINT_1				0x00020000		// 1 Phase Select
#define D_SD_PRE_TUNE_POINT_0				0x00010000		// 0 Phase Select

#define D_SD_3_RE_TUNE_RESULT_7				0x00000400		// Tuning/Re-tuning Point After Phase Success
#define D_SD_3_RE_TUNE_RESULT_6				0x00000200		// Tuning/Re-tuning Point Select Phase Success
#define D_SD_3_RE_TUNE_RESULT_5				0x00000100		// Tuning/Re-tuning Point Before Phase Success

#define D_SD_8_TUNE_RESULT_7				0x00000080		// Tuning/Re-tuning Point 7 Phase Success
#define D_SD_8_TUNE_RESULT_6				0x00000040		// Tuning/Re-tuning Point 6 Phase Success
#define D_SD_8_TUNE_RESULT_5				0x00000020		// Tuning/Re-tuning Point 5 Phase Success
#define D_SD_8_TUNE_RESULT_4				0x00000010		// Tuning/Re-tuning Point 4 Phase Success
#define D_SD_8_TUNE_RESULT_3				0x00000008		// Tuning/Re-tuning Point 3 Phase Success
#define D_SD_8_TUNE_RESULT_2				0x00000004		// Tuning/Re-tuning Point 2 Phase Success
#define D_SD_8_TUNE_RESULT_1				0x00000002		// Tuning/Re-tuning Point 1 Phase Success
#define D_SD_8_TUNE_RESULT_0				0x00000001		// Tuning/Re-tuning Point 0 Phase Success

/*----------------------------------
	I/O Control1 Register			
----------------------------------*/
//---- bit Check -----
#define D_SD_IO_SWITCH						0x08000000		// Power Switch bit
#define D_SD_CMD_O_CTL						0x04000000		// SD_CMD_O Pin bit
#define D_SD_DAT_O_CTL						0x03C00000		// SD_DAT_O[3:0] Pin bit
#define D_SD_XCLK18OE_O_CTL					0x00200000		// SD_XCLK18OE_O Pin bit
#define D_SD_XCLK33OE_O_CTL					0x00100000		// SD_XCLK33OE_O Pin bit
#define D_SD_CMD18PC_O_CTL					0x00080000		// SD_CMD18PC_O Pin bit
#define D_SD_CMD33PC_O_CTL					0x00040000		// SD_CMD33PC_O Pin bit
#define D_SD_XCMD18OE_O_CTL					0x00020000		// SD_XCMD18OE_O Pin bit
#define D_SD_XCMD33OE_O_CTL					0x00010000		// SD_XCMD33OE_O Pin bit
#define D_SD_DAT18PC_O_CTL					0x0000F000		// SD_DAT18PC_O[3:0] Pin bit
#define D_SD_DAT33PC_O_CTL					0x00000F00		// SD_DAT33PC_O[3:0] Pin bit
#define D_SD_XDAT18OE_O_CTL					0x000000F0		// SD_XDAT18OE_O[3:0] Pin bit
#define D_SD_XDAT33OE_O_CTL					0x0000000F		// SD_XDAT33OE_O[3:0] Pin bit

//---- Status -----
#define D_SD_IO_SWITCH_18V					0x08000000		// 1.8V I/O Enable
#define D_SD_IO_SWITCH_33V					0x00000000		// 1.8V I/O Disable (Default)

#define D_SD_CMD_O_CTL_HI					0x04000000		// SD_CMD_O Hi (Default)
#define D_SD_CMD_O_CTL_LOW					0x00000000		// SD_CMD_O low
#define D_SD_DAT_O_CTL_HI					0x03C00000		// SD_DAT_O[3:0] Hi (Default)
#define D_SD_DAT_O_CTL_LOW					0x00000000		// SD_DAT_O[3:0] low
#define D_SD_XCLK18OE_O_CTL_HI				0x00200000		// SD_XCLK18OE_O Hi (Default)
#define D_SD_XCLK18OE_O_CTL_LOW				0x00000000		// SD_XCLK18OE_O low
#define D_SD_XCLK33OE_O_CTL_HI				0x00100000		// SD_XCLK33OE_O Hi
#define D_SD_XCLK33OE_O_CTL_LOW				0x00000000		// SD_XCLK33OE_O low (Default)
#define D_SD_CMD18PC_O_CTL_HI				0x00080000		// SD_CMD18PC_O Hi (Default)
#define D_SD_CMD18PC_O_CTL_LOW				0x00000000		// SD_CMD18PC_O low
#define D_SD_CMD33PC_O_CTL_HI				0x00040000		// SD_CMD33PC_O Hi
#define D_SD_CMD33PC_O_CTL_LOW				0x00000000		// SD_CMD33PC_O low (Default)
#define D_SD_XCMD18OE_O_CTL_HI				0x00020000		// SD_XCMD18OE_O Hi (Default)
#define D_SD_XCMD18OE_O_CTL_LOW				0x00000000		// SD_XCMD18OE_O low
#define D_SD_XCMD33OE_O_CTL_HI				0x00010000		// SD_XCMD33OE_O Hi (Default)
#define D_SD_XCMD33OE_O_CTL_LOW				0x00000000		// SD_XCMD33OE_O low
#define D_SD_DAT18PC_O_CTL_HI				0x0000F000		// SD_DAT18PC_O[3:0] Hi (Default)
#define D_SD_DAT18PC_O_CTL_LOW				0x00000000		// SD_DAT18PC_O[3:0] low
#define D_SD_DAT33PC_O_CTL_HI				0x00000F00		// SD_DAT33PC_O[3:0] Hi
#define D_SD_DAT33PC_O_CTL_LOW				0x00000000		// SD_DAT33PC_O[3:0] low (Default)
#define D_SD_XDAT18OE_O_CTL_HI				0x000000F0		// SD_XDAT18OE_O[3:0] Hi (Default)
#define D_SD_XDAT18OE_O_CTL_LOW				0x00000000		// SD_XDAT18OE_O[3:0] low
#define D_SD_XDAT33OE_O_CTL_HI				0x0000000F		// SD_XDAT33OE_O[3:0] Hi (Default)
#define D_SD_XDAT33OE_O_CTL_LOW				0x00000000		// SD_XDAT33OE_O[3:0] low

/*----------------------------------
	I/O Control2 Register			
----------------------------------*/
//---- bit Check -----
#define D_SD_IO_CTL2_SD_CRES_O_CTL			0x00080000		// SD_CRES_O Control
#define D_SD_IO_CTL2_SD_MSEL_O_CTL			0x00040000		// SD_MSEL_O Control
#define D_SD_IO_CTL2_SD_CMD_O_CTL			0x00020000		// SD_CMD_O Control
#define D_SD_IO_CTL2_SD_XCMODE_O_CTL		0x00010000		// SD_XCMDOE_O Control
#define D_SD_IO_CTL2_SD_DAT_O_CTL			0x00000F00		// SD_DAT_O Control
#define D_SD_IO_CTL2_SD_XDATOE_O_CTL		0x0000000F		// SD_XDATOE_O Control

//---- Status -----
#define D_SD_IO_CTL2_IO_PULL_UP_OFF			0x00080000		// I/O Pull-up OFF
#define D_SD_IO_CTL2_IO_PULL_UP_ON			0x00000000		// I/O Pull-up ON

#define D_SD_IO_CTL2_IO_18V					0x00040000		// I/O 1.8V
#define D_SD_IO_CTL2_IO_33V					0x00000000		// I/O 3.3V

#define D_SD_IO_CTL2_CMD_O_HI				0x00020000		// SD_CMD_O High
#define D_SD_IO_CTL2_CMD_O_LOW				0x00000000		// SD_CMD_O Low

#define D_SD_IO_CTL2_XCMDOE_O_HI			0x00010000		// SD_XCMDOE_O High
#define D_SD_IO_CTL2_XCMDOE_O_LOW			0x00000000		// SD_XCMDOE_O Low

#define D_SD_IO_CTL2_DAT_O_HI				0x00000F00		// SD_DATO[3:0] High
#define D_SD_IO_CTL2_DAT_O_LOW				0x00000000		// SD_DATO[3:0] Low

#define D_SD_IO_CTL2_XDATOE_O_HI			0x0000000F		// SD_XDATOE_O[3:0] High
#define D_SD_IO_CTL2_XDATOE_O_LOW			0x00000000		// SD_XDATOE_O[3:0] Low

/*----------------------------------
	Power Switching Interrupt Status Register
	Power Switching Interrput Status Enable Register
	Power Switching Interrput Signal Enable Register
----------------------------------*/
//---- bit Check -----
#define D_SD_POW_SW_1WAIT_INT				0x0002			// 1ms Wait Interruput bit
#define D_SD_POW_SW_5WAIT_INT				0x0001			// 5ms Wait Interruput bit

//---- Status -----
#define D_SD_1MS_WAIT_INT					0x0002			// 1ms Wait Interrupt
#define D_SD_1MS_WAIT_NO_INT				0x0000			// 1ms Wait No Interrupt
#define D_SD_5MS_WAIT_INT					0x0001			// 5ms Wait Interrupt
#define D_SD_5MS_WAIT_NO_INT				0x0000			// 5ms Wait No Interrupt

/*----------------------------------
	MMC/eSD Control Register		
----------------------------------*/
//---- bit Check -----
#define D_SD_MMC_ESD_CTL_BMEMMC				0x00040000		// Boot Mode Enable for MMC
#define D_SD_MMC_ESD_CTL_BAAEMMC			0x00020000		// Boot Auto Abort Enable for MMC
#define D_SD_MMC_ESD_CTL_BAEMMC				0x00010000		// Boot Ack Enable for MMC
#define D_SD_MMC_ESD_CTL_CDDSEL				0x00000200		// CMD/DAT Delay Select
#define D_SD_MMC_ESD_CTL_MDSEL				0x00000100		// MMC DDR Select
#define D_SD_MMC_ESD_CTL_VCCQCMMC			0x00000008		// VCCQ Control for MMC
#define D_SD_MMC_ESD_CTL_VCCCMMC			0x00000004		// VCC Control for MMC
#define D_SD_MMC_ESD_CTL_RSTCMMC			0x00000002		// RST_n Control for MMC
#define D_SD_MMC_ESD_CTL_LKRSTCESD			0x00000001		// Lock-Reset Control for eSD

//---- Status -----
#define D_SD_MMC_BOOT_MODE_ENB				0x00040000		// Boot Mode Operation Enable
#define D_SD_MMC_BOOT_MODE_DIS				0x00000000		// Boot Mode Operation Disable

#define D_SD_MMC_BOOT_AUTO_ABORT_ENB		0x00020000		// Boot Auto Abort Enable
#define D_SD_MMC_BOOT_AUTO_ABORT_DIS		0x00020000		// Boot Auto Abort Disable

#define D_SD_MMC_BOOT_ACK_ENB				0x00010000		// Boot Acknowledge Enable
#define D_SD_MMC_BOOT_ACK_DIS				0x00000000		// Boot Acknowledge Disable

#define D_SD_CMD_DAT_DELAY_ENB				0x00000200		// CMD/DAT Delay Enable
#define D_SD_CMD_DAT_DELAY_DIS				0x00000000		// CMD/DAT Delay Disable

#define D_SD_MMC_DDR_TRANS					0x00000100		// MMC DDR Transfer
#define D_SD_MMC_SDR_TRANS					0x00000000		// MMC SDR Transfer

#define D_SD_MMC_VCCQ_HI					0x00000008		// SD_VCCQ_O High
#define D_SD_MMC_VCCQ_LOW					0x00000000		// SD_VCCQ_O Low

#define D_SD_MMC_VCC_HI						0x00000004		// SD_VCC_O High
#define D_SD_MMC_VCC_LOW					0x00000000		// SD_VCC_O Low

#define D_SD_MMC_RSTN_HI					0x00000002		// SD_RSTN_O High
#define D_SD_MMC_RSTN_LOW					0x00000000		// SD_RSTN_O Low

#define D_SD_ESD_LOCKRST_ON					0x00000001		// eSD Lock-Reset ON
#define D_SD_ESD_LOCKRST_OFF				0x00000000		// eSD Lock-Reset OFF

/*----------------------------------
	MMC Wait IRQ Control1 Register	
----------------------------------*/
//---- bit Check -----
#define D_SD_MMC_WAIT_IRQ_ST				0x00000002		// Wait IRQ State bit
#define D_SD_MMC_WAIT_IRQ_SEL				0x00000001		// Wait IRQ Enable bit

//---- Status -----
#define D_SD_MMC_WAIT_IRQ					0x00000002		// Wait IRQ
#define D_SD_MMC_NOT_WAIT_IRQ				0x00000000		// Not Wait IRQ

#define D_SD_MMC_WAIT_IRQ_ENB				0x00000001		// Wait IRQ Enable
#define D_SD_MMC_WAIT_IRQ_DIS				0x00000000		// Wait IRQ Disable

/*----------------------------------
	MMC Response Check bit Register	
----------------------------------*/
//---- bit Check -----
#define D_SD_MMC_CHKBIT2					0x00001F80		// Check Bit 2
#define D_SD_MMC_CHKBIT1					0x0000007F		// Check Bit 1

//---- Status -----

/*----------------------------------
	Card Detect Setting Register	
----------------------------------*/
//---- bit Check -----
#define D_SD_DEBOUNCE_TIMER					0x0F00			// Debounce Time Check bit

//---- Status -----
#define D_SD_DEBOUNCE_CLK_2_27				0x0E00			// Debounce Time for SD_SDCLK_I * 2^27
#define D_SD_DEBOUNCE_CLK_2_26				0x0D00			// Debounce Time for SD_SDCLK_I * 2^26

#define D_SD_DEBOUNCE_CLK_2_24				0x0B00			// Debounce Time for SD_SDCLK_I * 2^24
#define D_SD_DEBOUNCE_CLK_2_23				0x0A00			// Debounce Time for SD_SDCLK_I * 2^23
#define D_SD_DEBOUNCE_CLK_2_22				0x0900			// Debounce Time for SD_SDCLK_I * 2^22
#define D_SD_DEBOUNCE_CLK_2_21				0x0800			// Debounce Time for SD_SDCLK_I * 2^21
#define D_SD_DEBOUNCE_CLK_2_20				0x0700			// Debounce Time for SD_SDCLK_I * 2^20
#define D_SD_DEBOUNCE_CLK_2_19				0x0600			// Debounce Time for SD_SDCLK_I * 2^19
#define D_SD_DEBOUNCE_CLK_2_18				0x0500			// Debounce Time for SD_SDCLK_I * 2^18
#define D_SD_DEBOUNCE_CLK_2_17				0x0400			// Debounce Time for SD_SDCLK_I * 2^17
#define D_SD_DEBOUNCE_CLK_2_16				0x0300			// Debounce Time for SD_SDCLK_I * 2^16
#define D_SD_DEBOUNCE_CLK_2_15				0x0200			// Debounce Time for SD_SDCLK_I * 2^15
#define D_SD_DEBOUNCE_CLK_2_14				0x0100			// Debounce Time for SD_SDCLK_I * 2^14
#define D_SD_DEBOUNCE_CLK_2_25				0x0000			// Debounce Time for SD_SDCLK_I * 2^25 (default)

/*----------------------------------
	BCLKSELControl Register			
----------------------------------*/
//---- bit Check -----
#define D_SD_REGSEL						0x00010000			// REGSEL
#define D_SD_BCLKDIV					0x00000600			// BCLKDIV
#define D_SD_BCLKSEL					0x00000100			// BCLKSEL

//---- Status -----
#define D_SD_REGSEL_ORG_AREA			0x00000000			// Access to F_SDH30 macro register area
#define D_SD_REGSEL_ADD_AREA			0x00010000			// Access to F_SDH30 additional register area

#define D_SD_BCLKDIV_1_BCLK				0x00000000			// BCLK frequency 1/1
#define D_SD_BCLKDIV_2_BCLK				0x00000200			// BCLK frequency 1/2
#define D_SD_BCLKDIV_4_BCLK				0x00000400			// BCLK frequency 1/4
#define D_SD_BCLKDIV_8_BCLK				0x00000600			// BCLK frequency 1/8

#define D_SD_BCLKSEL_BCLK1				0x00000000			// SD_BCLK1_I is selected
#define D_SD_BCLKSEL_BCLK2				0x00000100			// SD_BCLK2_I is selected

/*----------------------------------
	SOFT_RESET Register				
----------------------------------*/
//---- bit Check -----
#define D_SD_SR_RST1X						0x00000001		// Soft Reset(F_SDH30) bit
#define D_SD_SR_RST2X						0x00010000		// Soft Reset(F_SDH40) bit

//---- Status -----
#define D_SD_SOFT_RESET1X_DIS				0x00000001		// SOFT_RESET(F_SDH30) Not Reset
#define D_SD_SOFT_RESET2X_DIS				0x00010000		// SOFT_RESET(F_SDH40) Not Reset
#define D_SD_SOFT_RESET_ENB					0x00000000		// SOFT_RESET Reset

/*----------------------------------
	HDCLK SEL Register
----------------------------------*/
//---- bit Check -----
#define D_SD_DATHOLD_SEL					0x000F0000		// DATA HOLD Select
#define D_SD_CMDHOLD_SEL					0x00000F00		// CMD HOLD Select
#define D_SD_HDCLK_SEL1						0x00000002		// HLDCLK_I Polarity Select
#define D_SD_HDCLK_SEL						0x00000001		// HLDCLK_I clock Select

//---- Status -----
#define D_SD_DATHOLD_SEL_SDBCLK_1_4_3		0x000F0000		// SD_SDBLK_I 1/4 + 1/4 + 1/4 select
#define D_SD_DATHOLD_SEL_SDBCLK_1_4_2		0x000C0000		// SD_SDBLK_I 1/4 + 1/4 select
#define D_SD_DATHOLD_SEL_SDBCLK_1_4			0x00000000		// SD_SDBLK_I 1/4 select

#define D_SD_CMDHOLD_SEL_SDBCLK_1_4_3		0x00000F00		// SD_SDBLK_I 1/4 + 1/4 + 1/4 select
#define D_SD_CMDHOLD_SEL_SDBCLK_1_4_2		0x00000C00		// SD_SDBLK_I 1/4 + 1/4 select
#define D_SD_CMDHOLD_SEL_SDBCLK_1_4			0x00000000		// SD_SDBLK_I 1/4 select

#define D_SD_HDCLK_SEL_HDCLK_RE				0x00000001		// SD_HLDCLK_I reverse select
#define D_SD_HDCLK_SEL_HDCLK_DI				0x00000000		// SD_HLDCLK_I direct select

#define D_SD_HDCLK_SEL_SD4CLK_1_2			0x00000001		// SD_SD4CLK_I 1/2 select
#define D_SD_HDCLK_SEL_SD4CLK_1_1			0x00000000		// SD_SD4CLK_I 1/1 select

/*----------------------------------
	HPORT SEL Register
----------------------------------*/
//---- bit Check -----
#define D_SD_HPORT_SEL						0x0000000F		// HPORT Select

//---- Status -----
#define D_SD_HPORT_SEL_ENB					0x00000008		// HPORT ON
#define D_SD_HPORT_SEL_DIS					0x00000000		// HPORT OFF

/*----------------------------------
	CLK_STOP Register				
----------------------------------*/
//---- bit Check -----
#define D_SD_TOCLK_STP						0x00000020		// TOCLK_STP bit
#define D_SD_CDCLK_STP						0x00000010		// CDCLK_STP bit
#define D_SD_SD4CLK_STP						0x00000008		// SD4CLK_STP bit
#define D_SD_SDBCLK_STP						0x00000004		// SDBCLK_STP bit
#define D_SD_HLDCLK_STP						0x00000002		// HLDCLK_STP bit
#define D_SD_HCLK_STP						0x00000001		// HCLK_STP bit

//---- Status -----
#define D_SD_TOCLK_STOP						0x00000020		// TOCLK Stop
#define D_SD_TOCLK_START					0x00000000		// TOCLK Start

#define D_SD_CDCLK_STOP						0x00000010		// CDCLK Stop
#define D_SD_CDCLK_START					0x00000000		// CDCLK Start

#define D_SD_SD4CLK_STOP					0x00000008		// SD4CLK Stop
#define D_SD_SD4CLK_START					0x00000000		// SD4CLK Start

#define D_SD_SDBCLK_STOP					0x00000004		// SDBCLK Stop
#define D_SD_SDBCLK_START					0x00000000		// SDBCLK Start

#define D_SD_HLDCLK_STOP					0x00000002		// HLDCLK Stop
#define D_SD_HLDCLK_START					0x00000000		// HLDCLK Start

#define D_SD_HCLK_STOP						0x00000001		// HCLK Stop
#define D_SD_HCLK_START						0x00000000		// HCLK Start

/*----------------------------------
	WP_CD_LED_SET Register			
----------------------------------*/
//---- bit Check -----
#define D_SD_WP_SEL							0x00020000		// WP Signal Select
#define D_SD_CD_SEL							0x00010000		// CD Signal Select
#define D_SD_WP_VAL							0x00000200		// WP Value
#define D_SD_CD_VAL							0x00000100		// CD Value
#define D_SD_LED_INV						0x00000004		// LED Invert
#define D_SD_WP_INV							0x00000002		// WP Invert
#define D_SD_CD_INV							0x00000001		// CD Invert

//---- Status -----
#define D_SD_WP_SIG_PORT_VAL				0x00000000		// WP Signal Input by Port
#define D_SD_WP_SIG_REG_VAL					0x00020000		// WP Signal Input by Register value

#define D_SD_CD_SIG_PORT_VAL				0x00000000		// CD Signal Input by Port
#define D_SD_CD_SIG_REG_VAL					0x00010000		// CD Signal Input by Register value

#define D_SD_WP_VAL_LOW						0x00000000		// WP Signal Low
#define D_SD_WP_VAL_HIGH					0x00000200		// WP Signal High

#define D_SD_CD_VAL_LOW						0x00000000		// CD Signal Low
#define D_SD_CD_VAL_HIGH					0x00000100		// WP Signal High

#define D_SD_LED_NOT_REV					0x00000000		// LED Signal not reverse
#define D_SD_LED_REV						0x00000004		// LED Signal reverse

#define D_SD_WP_NOT_REV						0x00000000		// WP Signal not reverse
#define D_SD_WP_REV							0x00000002		// WP Signal reverse

#define D_SD_CD_NOT_REV						0x00000000		// CD Signal not reverse
#define D_SD_CD_REV							0x00000001		// CD_Signal reverse

/*----------------------------------
	RE-TUNING_REQ Register			
----------------------------------*/
//---- bit Check -----
#define D_SD_RT_REQ							0x00000001		// Re-Tuning Request bit

//---- Status -----
#define D_SD_RE_TUNE_REQ					0x00000001		// Re-Tuning Request
#define D_SD_RE_TUNE_NOT_REQ				0x00000000		// Re-Tuning Not Request

/*----------------------------------
	RE-TUNING_REQ_CLR Register		
----------------------------------*/
//---- bit Check -----
#define D_SD_RT_REQ_CLR						0x00000001		// Re-Tuning Request Clear bit

//---- Status -----
#define D_SD_RE_TUNE_REQ_CLR				0x00000001		// Re-Tuning Request Clear
#define D_SD_RE_TUNE_REQ_NOT_CLR			0x00000000		// Re-Tuning Not Request not Clear

/*----------------------------------
	CR_SET Register					
----------------------------------*/
//---- bit Check -----
#define D_SD_CR_SLOTTYPE0					0x10000000		// CR_SLOTTYPE0 bit
#define D_SD_CR_TOCLKUNIT					0x01000000		// CR_TOCLKUNIT bit
#define D_SD_CR_TOCLKFREQ					0x003F0000		// CR_TOCLKFREQ bit
#define D_SD_CR_BCLKFREQ					0x0000FF00		// CR_BCLKFREQ bit
#define D_SD_CR_RTUNTIMER					0x000000F0		// CR_RTUNTIMER bit
#define D_SD_CR_RTUNMODE					0x00000003		// CR_RTUNMODE bit

//---- Status -----
#define D_SD_CR_SLOT_TYPE_EMBEDDED			0x10000000		// Slot Type Embedded
#define D_SD_CR_SLOT_TYPE_REMOVAL			0x00000000		// Slot Type Removal

#define D_SD_CR_TOCLK_UNIT_MHZ				0x01000000		// Timeout Clock Unit MHz
#define D_SD_CR_TOCLK_UNIT_KHZ				0x00000000		// Tiueomt Clock Unit KHz

#define D_SD_CR_RE_TUNE_MODE3				0x00000002		// Re-Tuning Mode3
#define D_SD_CR_RE_TUNE_MODE2				0x00000001		// Re-Tuning Mode2
#define D_SD_CR_RE_TUNE_MODE1				0x00000000		// Re-Tuning Mode1

/*----------------------------------
	CDR_SET Register				
----------------------------------*/
//---- bit Check -----
#define D_SD_CDR_DTIMEC							0x000F		// Debounce Time Set bit

//---- Status -----
#define D_SD_SET_DEBOUNCE_CLK_2_27				0x000E		// Debounce Time Set for SD_SDCLK_I * 2^27
#define D_SD_SET_DEBOUNCE_CLK_2_26				0x000D		// Debounce Time Set for SD_SDCLK_I * 2^26

#define D_SD_SET_DEBOUNCE_CLK_2_24				0x000B		// Debounce Time Set for SD_SDCLK_I * 2^24
#define D_SD_SET_DEBOUNCE_CLK_2_23				0x000A		// Debounce Time Set for SD_SDCLK_I * 2^23
#define D_SD_SET_DEBOUNCE_CLK_2_22				0x0009		// Debounce Time Set for SD_SDCLK_I * 2^22
#define D_SD_SET_DEBOUNCE_CLK_2_21				0x0008		// Debounce Time Set for SD_SDCLK_I * 2^21
#define D_SD_SET_DEBOUNCE_CLK_2_20				0x0007		// Debounce Time Set for SD_SDCLK_I * 2^20
#define D_SD_SET_DEBOUNCE_CLK_2_19				0x0006		// Debounce Time Set for SD_SDCLK_I * 2^19
#define D_SD_SET_DEBOUNCE_CLK_2_18				0x0005		// Debounce Time Set for SD_SDCLK_I * 2^18
#define D_SD_SET_DEBOUNCE_CLK_2_17				0x0004		// Debounce Time Set for SD_SDCLK_I * 2^17
#define D_SD_SET_DEBOUNCE_CLK_2_16				0x0003		// Debounce Time Set for SD_SDCLK_I * 2^16
#define D_SD_SET_DEBOUNCE_CLK_2_15				0x0002		// Debounce Time Set for SD_SDCLK_I * 2^15
#define D_SD_SET_DEBOUNCE_CLK_2_14				0x0001		// Debounce Time Set for SD_SDCLK_I * 2^14
#define D_SD_SET_DEBOUNCE_CLK_2_25				0x0000		// Debounce Time Set for SD_SDCLK_I * 2^25 (default)

/*----------------------------------
	PV_SET1 Register				
----------------------------------*/
//---- bit Check -----
#define D_SD_PV_FREQDS						0x03FF0000		// SDCLK Frequency Select Value(for Default Speed) of Preset Value
#define D_SD_PV_FREQINI						0x000003FF		// SDCLK Frequency Select Value(for Initialize) of Preset Value

//---- Status -----


/*----------------------------------
	PV_SET2 Register				
----------------------------------*/
//---- bit Check -----
#define D_SD_PV_FREQSDR12					0x03FF0000		// SDCLK Frequency Select Value(for SDR12) of Preset Value
#define D_SD_PV_FREQHS						0x000003FF		// SDCLK Frequency Select Value(for High Speed) of Preset Value

//---- Status -----


/*----------------------------------
	PV_SET3 Register				
----------------------------------*/
//---- bit Check -----
#define D_SD_PV_FREQSDR50					0x03FF0000		// SDCLK Frequency Select Value(for SDR50) of Preset Value
#define D_SD_PV_FREQSDR25					0x000003FF		// SDCLK Frequency Select Value(for SDR25) of Preset Value

//---- Status -----


/*----------------------------------
	PV_SET4 Register				
----------------------------------*/
//---- bit Check -----
#define D_SD_PV_FREQDDR50					0x03FF0000		// SDCLK Frequency Select Value(for DDR50) of Preset Value
#define D_SD_PV_FREQSDR104					0x000003FF		// SDCLK Frequency Select Value(for SDR104) of Preset Value

//---- Status -----


/*----------------------------------
	HRS09 UHS-II PHY Configuration 2
----------------------------------*/
//---- bit Check -----
#define D_SD2_U2PHY_TSTPHY4					0x00000004		// I_TSTPHY4 Signal Control
#define D_SD2_U2PHY_TSTPHY3					0x00000002		// I_TSTPHY3 Signal Control
#define D_SD2_U2PHY_TSTPHY2					0x00000001		// I_TSTPHY2 Signal Control

/*----------------------------------
	HRS23 PHY Performaince Adjustment Signals Control 1
----------------------------------*/
//---- bit Check -----
#define D_SD2_D0TRSEL						0x00280000		// This field has to be set 101b.

/*----------------------------------
	HRS23 PHY Performaince Adjustment Signals Control 2
----------------------------------*/
//---- bit Check -----
#define D_SD2_CLT							0x00000006		// This field has to be set 11b.

/*----------------------------------
	HRS32 LINK-PHY I/F signals
----------------------------------*/
//---- bit Check -----
#define D_SD2_LINKPHY_LOCK					0x00000002		// ST[1] signal

//---- Status -----
#define D_SD2_LINKPHY_LOCK_LOW				0x00000000		// ST[1] signal low
#define D_SD2_LINKPHY_LOCK_HIGH				0x00000002		// ST[1] signal high

/*----------------------------------
	HRS38 UHS-II I/O Control Register
----------------------------------*/
//---- bit Check -----
#define D_SD2_UHS2PC						0x00000001		// Cut off pull-up resister in the I/Os for DAT0 line and DAT1 line bit

//---- Status -----
#define D_SD2_UHS2PC_VAL_ON					0x00000000		// pull-up resister ON
#define D_SD2_UHS2PC_VAL_OFF				0x00000001		// pull-up resister OFF

/*----------------------------------
	HRS39 UHS-II BCLKSEL Status Register
----------------------------------*/
//---- bit Check -----
#define D_SD2_U2BCLKSEL_STP					0x00000001		// SD Base Clock Stopped Status bit

/*----------------------------------
	HRS45 UHS-II PHY Compliance Test Control
----------------------------------*/
//---- bit Check -----
#define D_SD2_SLVMODE						0x01000000		// Slave Mode for PHY compliance test
#define D_SD2_CT_CT							0x0000FF00		// CT for PHY compliance test
#define D_SD2_RCLK2EN_TST					0x00000040		// RCLK2EN for PHY comliance test
#define D_SD2_CT_MODE						0x00000020		// MODE for PHY compliace test
#define D_SD2_CT_NRST						0x00000010		// NRST for PHY compliance test
#define D_SD2_CTSEL							0x00000001		// PHY-LINK I/F Signals Select for PHY compliance test

//---- Status -----
#define D_SD2_SLVMODE_SLAVE					0x01000000		// Slave Mode
#define D_SD2_SLVMODE_NORMAL				0x00000000		// Normal Mode

#define D_SD2_CT_CT_PHY_CONTROL				0x00004000		// PHY control
#define D_SD2_CT_CT_COMP_TEST				0x00004400		// UHS-II compliance test
#define D_SD2_CT_CT_CLR						0x00000000		// CTCT clear

#define D_SD2_RCLK2EN_TST_ON				0x00000040		// RCLK2EN for PHY comliance test ON
#define D_SD2_RCLK2EN_TST_OFF				0x00000000		// RCLK2EN for PHY comliance test OFF

#define D_SD2_CT_MODE_ON					0x00000020		// MODE for PHY compliace test ON
#define D_SD2_CT_MODE_OFF					0x00000000		// MODE for PHY compliace test OFF

#define D_SD2_CT_NRST_ON					0x00000010		// NRST for PHY compliance test ON
#define D_SD2_CT_NRST_OFF					0x00000000		// NRST for PHY compliance test OFF

#define D_SD2_CTSEL_ON						0x00000001		// PHY-LINK I/F Signals Select for PHY compliance test ON
#define D_SD2_CTSEL_OFF						0x00000000		// PHY-LINK I/F Signals Select for PHY compliance test OFF

/*----------------------------------
	SRS32 UHS-II SDMA Buffer Boundary Register
----------------------------------*/
//---- bit Check -----
#define D_SD2_BOUND							0x00007000		// Buff Bound bit
#define D_SD2_BLKSIZE						0x00000FFF		// Block Size bit

/*----------------------------------
	SRS39 UHS-II Transfer Mode Register
----------------------------------*/
//---- bit Check -----
#define D_SD2_U2HFD_HD						0x8000			// half duplex
#define D_SD2_U2HFD_FD						0x0000			// full duplex

/*----------------------------------
	SRS48 UHS-II Software Reset Register
----------------------------------*/
//---- bit Check -----
#define D_SD2_TRAN							0x02			// Host SD-TRAN Reset
#define D_SD2_FULL							0x01			// Host Full Reset

/*----------------------------------
	SRS49 UHS-II Error Interrupt Status Register
	SRS50 UHS-II Error Interrupt Status Enable Register
	SRS51 UHS-II Error Interrupt Signal Enable Register
----------------------------------*/
//---- bit Check -----
#define D_SD2_ETDL							0x00020000		// Timeout for Deadlock
#define D_SD2_ETCR							0x00010000		// Timeout for CMD_RES
#define D_SD2_EADMA							0x00008000		// ADMA2 Error
#define D_SD2_EBSY							0x00000100		// EBSY Error
#define D_SD2_EUR							0x00000080		// Unrecoverable Error
#define D_SD2_ETID							0x00000020		// TID Error
#define D_SD2_EFRM							0x00000010		// Framing Error
#define D_SD2_ECRC							0x00000008		// CRC Error
#define D_SD2_EREX							0x00000004		// Retry Expired
#define D_SD2_ERSP							0x00000002		// RES Packet Error
#define D_SD2_EHDR							0x00000001		// Header Error

#define D_SD2_EINT_ALL						0x000381BF		// Error Status All
#define D_SD2_EINT_CMD						0x000300BF		// Command Error Status
#define D_SD2_EINT_DAT						0x0002819F		// Data Error Status
#define D_SD2_EINT_CONF						0x0003			// Command CRC/Timeout Error

#define D_SD2_EINT_DAT_CMD					(D_SD2_EINT_DAT | D_SD2_EINT_CMD)		// DAT/CMD Error Status
#define D_SD2_EINT_DAT_DMA					(D_SD2_EINT_DAT | D_SD2_EADMA)			// DAT/DMA Error Status
#define D_SD2_EINT_DAT_DMA_CMD				(D_SD2_EINT_DAT | D_SD2_EADMA | D_SD2_EINT_CMD)		// DAT/DMA/CMD Error Status
#define D_SD2_EINT_DAT_DMA_CMD_ACE			(D_SD2_EINT_DAT | D_SD2_EADMA | D_SD2_EINT_CMD | D_SD_ACE)	// DAT/DMA/CMD/AutoCMD12 Error Status

/*----------------------------------
	ERS01 UHS-II PHY Settings
----------------------------------*/
//---- bit Check -----
#define D_SD2_HNLD							0x00F00000		// Host N_LSS_DIR
#define D_SD2_HNLS							0x000F0000		// Host N_LSS_SYN
#define D_SD2_HIBE							0x00008000		// Hibernate Enable
#define D_SD2_SPDR							0x00000040		// Speed Range

//---- Status -----
#define D_SD2_SPDR_A						0x00000000		// Range A
#define D_SD2_SPDR_B						0x00000040		// Range B

#define D_SD2_HIBE_ENB						0x00008000		// Hibernate Enable
#define D_SD2_HIBE_DIS						0x00000000		// Hibernate Disable

/*----------------------------------
	ERS04 UHS-II General Capabilities
----------------------------------*/
//---- bit Check -----
#define D_SD2_GEN_CAPA_DAP					0x0000000F		// Device Allocation Power
#define D_SD2_GEN_CAPA_GAP					0x000000F0		// Group Allocation Power

/*----------------------------------
	ERS05 UHS-II UHS-II PHY Capabilities
----------------------------------*/
//---- bit Check -----
#define D_SD2_PHY_CAPA_DIR					0x00F00000		// N_LSS_DIR
#define D_SD2_PHY_CAPA_SYN					0x000F0000		// N_LSS_SYN

//---- Value -----
#define D_SD2_PHY_CAPA_DIR_3				0x03			// N_LSS_DIR value : 3h
#define D_SD2_PHY_CAPA_SYN_6				0x06			// N_LSS_SYN value : 6h


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

#endif /* _SDIO1_REG_DEF_H_ */
