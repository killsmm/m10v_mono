/**
 * @file		sd0_driver_info.h
 * @brief		SD Memory card driver information header file
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef	_SD0_DRIVER_INFO_H_
#define	_SD0_DRIVER_INFO_H_

/*****************************************   Outline   *******************************************
# Your system only includes this file and a series of APIs can be used for it.
*************************************************************************************************/
// ===============================================
//   Compile Option
// ===============================================
#define CO_SD0_DEBUG_PRINT_ON
#define CO_SD0_ERR_PRINT_ON
//#define CO_SD_DETECT_INT_ON

/*----- Big endian setting -----------------------------------------------------------------------
# The compile option is the big endian setting as a MPU characteristic. In case of setting(Big
# endian), describe "1", in case of not setting(Little endian), describe "0".
------------------------------------------------------------------------------------------------*/
#define	CO_SD_BIG_ENDIAN_DATA	(0)							// 1:Big Endian 0:Little Endian

/*----- Big endian setting -----------------------------------------------------------------------
# This compile option is to select the power supply control to the SD card.
------------------------------------------------------------------------------------------------*/
#define	CO_SD_POWER_SUPPLY_GPIO	(1)							// 0:Register(LEDX)  1:GPIO

/*------ Compile Option --------------------------------------------------------------------------
# D_SD_USE_BUS_WIDTH are for Bus Width.
# D_SD_USE_BUS_WIDTH is for card interface.
------------------------------------------------------------------------------------------------*/
#define	D_SD_USE_BUS_WIDTH		(1)							// 0:1bit Bus 1:4bit Bus 2:8bit Bus

// ===============================================
//   Debug Print
// ===============================================
#ifdef CO_SD0_DEBUG_PRINT_ON
#define	M_SD0_DEBUG_PRINT(arg)	(Media_Custom_Info_Printer arg)
#else	// CO_SD0_DEBUG_PRINT_ON
#define M_SD0_DEBUG_PRINT(arg)
#endif	// CO_SD0_DEBUG_PRINT_ON

#ifdef CO_SD0_ERR_PRINT_ON
#define M_SD0_ERR_PRINT(arg)		(Media_Custom_Err_Printer arg)
#else	// CO_SD0_ERR_PRINT_ON
#define M_SD0_ERR_PRINT(arg)
#endif	// CO_SD0_ERR_PRINT_ON

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_SD_WRITEMODE_UNKNOWN_DATA		(0)					// Writing of an unknown data.
#define D_SD_WRITEMODE_SYSTEM_DATA		(1)					// Directory, and writing other than the user data.
#define D_SD_WRITEMODE_DIRECTORY_ENTRY	(2)					// Write to the directory.
#define D_SD_WRITEMODE_USER_DATA		(3)					// Writing of user data.

/*-----  Write Protect Status bit  ---------------------------------------------------------------
------------------------------------------------------------------------------------------------*/
#define D_SD_LOCK_CARD	(0x01)
#define D_SD_PERM_WP	(0x02)
#define D_SD_TEMP_WP	(0x04)
#define D_SD_GRP_WP		(0x08)

/*-----  Error Retry  ----------------------------------------------------------------------------
------------------------------------------------------------------------------------------------*/
#define D_SD_MOUNT_MAX_RETRY_CNT			10
#define D_SD_MOUNT_RETRY_POWER_WAIT_TIME	500

#define D_SD_MAX_RETRY_LEVEL				3
#define D_SD_RETRY_LEVEL2_WAIT_TIME			500
#define D_SD_RETRY_LEVEL3_WAIT_TIME			500

/*-----  Define is versions of SDHC -------------------------------------------------------------
# Please set "1". Please set "2" in the case of use with the old hardware. 
# Please refer to document "2.0 Version at before 5th Draft August 25, 2,006".
------------------------------------------------------------------------------------------------*/
#define D_SDHC_VER				(1)							// Version 2.0

/*-----  Command retry number  -------------------------------------------------------------------
# Retries in case of NO_RESP or ERROR, except for CMD0 (since CMD0 is not retried.).
# D_SD_INIT_CMD_RTRY(=0xHHZZYYXX) is for discriminate media(CMD52:XX,CMD5:YY,CMD55:ZZ,CMD8:HH) during
# initialization.
# D_SD_CMD_RTRY is for other cases(CMD52, CMD5, CMD55 and CMD8(MMC4.0 over) are included.).
------------------------------------------------------------------------------------------------*/
#define	D_SD_INIT_CMD_RTRY		(0x01010101)
#define	D_SD_CMD_RTRY			(10)

/*-----  OCR Registor  ---------------------------------------------------------------------------
# This is prepared to set the supply voltage to card in case of issuing CMD1,CMD5,ACMD41.
# Please refer to the System Specification.
# Ellen-H suppors 1.8V,3.0V or 3.3V.
# If the plural voltages are enabled, the lowest voltage is selected by default on account of media.
# If there is any difficulty with a certain voltage level, set without such level.
# Example: 3.3V : 0x00300000(3.2-3.4V), 3.0V : 0x00060000(2.9-3.1V), 1.8V : 0x00000060(1.7-1.9V)
------------------------------------------------------------------------------------------------*/
#define	D_SD_OCR_SET			(0x00360060)				// 3.3V,3.0V,1.8V(Default)

/*-----  Card Interface Condition  ---------------------------------------------------------------
# This is prepared to set check pattern in case of issuing CMD8 for SD2.0 over.
# Please refer to the System Specification.
# D_SD_CHK_PATTEN is set to any 8-bit pattern.
------------------------------------------------------------------------------------------------*/
#define	D_SD_CHK_PATTEN			(0xaa)						// Check Pattern for SD 2.0 Command8

/*-----  Time Out Value  -------------------------------------------------------------------------
# D_SD_TIMEOUT_CLK is prepared to set Time Out Clock Frequency Value.(KHz).
# Substituted D_SD_TIMEOUT_CLK is utilized, if Capabilities Register cannot be read.
# D_SD_DAT_TIMEOUT is prepared to set the DAT Line Time Out Value.(Default:=1000ms)
# D_SD_OCR_TIMEOUT is prepared to set the OCR Time Out Value.(Default:=1500ms)
# D_SD_ABRT_TIMEOUT is prepared to set the Abort Time Out Value.(Default:=250ms)
# D_SD_DMA_TIMEOUT is prepared to set the DMA Time Out Value.(Default:=10000ms)
# D_SD_PON_TIME is prepared to set the Power up Time Out Value.(Default:=20ms)
# D_SD_POFF_TIME is prepared to set the Power down Time Out Value.(Default:=1000ms)
# D_SD_RAMP_TIME is prepared to set the Supply ramp up(for 74CLK) Time Out Value.(Default:=20ms)
------------------------------------------------------------------------------------------------*/
#define	D_SD_TIMEOUT_CLK		(50000)						// Time Out Clock 50MHz
#define	D_SD_DAT_TIMEOUT		(1000)						// Example is 1000ms for DAT time

#define	D_SD_OCR_TIMEOUT		(1500)						// Example is 1500ms for OCR time
#define	D_SD_ABRT_TIMEOUT		(250)						// Example is 250ms for Abort time
#define	D_SD_DMA_TIMEOUT		(1000)						// Example is 1000ms for DMA time

#define	D_SD_PON_TIME			(0)
#define	D_SD_POFF_TIME			(0)
#define	D_SD_RAMP_TIME			(35)						// Example is 35ms for supply ramp up time
#define D_SD_ICS_TIME			(10)						// Internal Clock Stable time (ms)

#define	D_SD_DMA_BOUND			(512)
#define D_SD_MAX_DMA_BLOCK		(D_SD_DMA_BOUND * 2)

/*-----  TRUE/FALSE  -----------------------------------------------------------------------------
------------------------------------------------------------------------------------------------*/
#define	D_SD_FALSE		(0)
#define	D_SD_TRUE		(1)

/*-----  Write Protect Status bit  ---------------------------------------------------------------
------------------------------------------------------------------------------------------------*/
#define D_SD_LOCK_CARD	(0x01)
#define D_SD_PERM_WP	(0x02)
#define D_SD_TEMP_WP	(0x04)
#define D_SD_GRP_WP		(0x08)

/*-----  Card Lock manage ------------------------------------------------------------------------
------------------------------------------------------------------------------------------------*/
#define D_SD_NEW			(0x00)
#define D_SD_OLD			(0x01)
#define D_SD_PASSWORD_SIZE	(16)

#define D_SD_LOCK_STATE_CLEAR			(0x00)
#define D_SD_LOCK_STATE_EXIST			(0x01)
#define D_SD_LOCK_STATE_LOCK			(0x10)

/*-----  Other Macro  ----------------------------------------------------------------------------
------------------------------------------------------------------------------------------------*/
#define	D_SD_ARG_RCA		(gSD0_MMC_RCA << 16)
#define	D_SD_CD_TYPE		(gSD0_Card_Type)
#define	D_SD_CD_FLAG		(gSD0_Detect_Flag)

/*-----  Command  --------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------*/
#define	D_SD_FUNC_READ_SECTOR			(0x20)						// read data from the sector(LBA)
#define	D_SD_FUNC_WRITE_SECTOR			(0x30)						// write data to the sector(LBA)
#define	D_SD_FUNC_INITIALIZE			(0xff)						// initilize the condition information
#define	D_SD_FUNC_READ_STATUS			(0xf0)						// read the condition of target media
#define	D_SD_FUNC_RESET					(0x00)						// Ellen Reset for All Set

/*-----  Status bit  -----------------------------------------------------------------------------
# If any kinds of error occurs, error bit(D_SD_ST_ERR) is set.
------------------------------------------------------------------------------------------------*/
#define	D_SD_ST_DRV1_RDY				(0x80)						// drive 1 ready bit
#define	D_SD_ST_DRV0_RDY				(0x40)						// drive 0 ready bit
#define	D_SD_ST_DRV1_WR_EN				(0x20)						// drive 1 write enable bit
#define	D_SD_ST_DRV0_WR_EN				(0x10)						// drive 0 write enable bit
#define	D_SD_ST_UNC_ERR					(0x09)						// uncorrectable data error bit
#define	D_SD_ST_IDNF_ERR				(0x05)						// ID not found error bit
#define	D_SD_ST_ABRT_ERR				(0x03)						// command abort error bit
#define	D_SD_ST_ERR						(0x01)						// status error bit
#define	D_SD_ST_ERR_CLR					(0xf0)						// clear the error bit of status

#define	D_SD_ST_DRV1_FORMATTED			(0x20)						// drive 1 formatted bit(alpha)
#define	D_SD_ST_DRV0_FORMATTED			(0x10)						// drive 0 formatted bit(alpha)
#define	D_SD_ST_DRV1_CARD_DETECT		(0x08)						// card in drive 1(alpha)
#define	D_SD_ST_DRV0_CARD_DETECT		(0x04)						// card in drive 0(alpha)
#define	D_SD_ST_DRV1_NO_CARD			(0x57)						// no card in drive 1(alpha)
#define	D_SD_ST_DRV0_NO_CARD			(0xab)						// no card in drive 0(alpha)

#define	D_SD_ST_RW_RDY					(0x00)						// ready for read / write interrupt
#define	D_SD_ST_TRAN_BSY				(0x40)						// Transfer Busy for read / write interrupt
#define	D_SD_ST_RW_BSY					(0x80)						// busy by read / write interrupt
#define	D_SD_ST_DETECT_CARD				D_SD_ST_DRV0_CARD_DETECT	// 0x04:detect memory card
#define	D_SD_ST_DETECT_SDIO				(0x10)						// detect SDIO card

/*-----  File attribute  -------------------------------------------------------------------------
------------------------------------------------------------------------------------------------*/
#define	D_SD_FILE_ATTR_ARCHIVE			(0x20)						// save condition attribute
#define	D_SD_FILE_ATTR_DIR				(0x10)						// directory attribute
#define	D_SD_FILE_ATTR_VOL_LABEL		(0x08)						// volume label attribute
#define	D_SD_FILE_ATTR_SYSTEM			(0x04)						// system attribute
#define	D_SD_FILE_ATTR_HIDDEN			(0x02)						// masked file attribute
#define	D_SD_FILE_ATTR_READ_ONLY		(0x01)						// read only file attribute
#define	D_SD_FILE_ATTR_LONG_NAME		(0x0f)						// long file name attribute

/*-----  The other definition  -------------------------------------------------------------------
------------------------------------------------------------------------------------------------*/
#define	D_SD_DRIVE_NUM					(1)							// SD Drive Num
#define	D_SD_MEDIA_BUFF_SIZE			(512)						// Buffer size of Media

#define	D_SD_FLAG_DRIVE					(0x80)						// flag bit to "designate drive"
#define	D_SD_FLAG_END					(0x80)						// flag bit of end
#define	D_SD_FLAG_VAL_END_BYTE			(0xff)						// value as end
#define	D_SD_FLAG_MSB_BYTE				(0x80)						// MSB flag
#define	D_SD_FLAG_MASK_MSB_BYTE			(0x7f)						// Mask flag
#define	D_SD_FLAG_VAL_END_HW			(0xffff)					// value as end
#define	D_SD_FLAG_MSB_HW				(0x8000)					// MSB flag
#define	D_SD_FLAG_MASK_MSB_HW			(0x7fff)					// Mask flag
#define	D_SD_FLAG_VAL_END_W				(0xffffffff)				// value as end
#define	D_SD_FLAG_MSB_W					(0x80000000)				// MSB flag
#define	D_SD_FLAG_MASK_MSB_W			(0x7fffffff)				// Mask flag
#define	D_SD_FLAG_CHECK_DRIVE			(0x80000000)				// check bit of the drive
#define	D_SD_FLAG_MASK_DRIVE			(0x7fffffff)				// mask bit of the drive
#define	D_SD_FLAG_VAL_END				D_SD_FLAG_VAL_END_W			// value of end
#define	D_SD_FLAG_MSB					D_SD_FLAG_MSB_W				// MSB flag
#define	D_SD_FLAG_MASK_MSB				D_SD_FLAG_MASK_MSB_W		// Mask flag

#undef	D_SD_FLAG_CHECK_DRIVE
#define	D_SD_FLAG_CHECK_DRIVE			(0)							// flag bit of single drive

#define	D_SD_FLAG_SV_MODE_START			(0)
#define	D_SD_FLAG_SV_MODE_END			(1)
#define	D_SD_FLAG_SV_CMD				(0xff)
#define	D_SD_FLAG_LONG_NAME_END			(0x8000)					// End flag of long filename

#define	D_SD_AM_INS_DATA				(0)							//  Constant Data for f1_aAmIns

/*-----  Detailed error status  ------------------------------------------------------------------
# This is not initialized, It is valid right after error occurs and can be reference.
# However, in case of recognizing improper device,the reason will be reported in the
# error status of that drive.
# As the value might be changed when version-up is released, please use the defined character.
------------------------------------------------------------------------------------------------*/
#define	D_SD_ERR_ST_NO_CARD_INIT		(0x10)						// Media does not exist when initialize
#define	D_SD_ERR_ST_DEVICE_CODE			(0x11)						// different device code when initialize
#define	D_SD_ERR_ST_READ_ADDR			(0x31)						// Address over in Read Sector
#define	D_SD_ERR_ST_WRITE_PROTECT		(0x33)						// Write Protect Detection in Write Sector
#define	D_SD_ERR_ST_WRITE_ADDR			(0x35)						// Address over in Write Sector
#define	D_SD_ERR_ST_MISC				(0x50)						// misc ERROR
#define	D_SD_ERR_ST_NO_RESP				(0x51)						// MMC does not exist
#define	D_SD_ERR_ST_CARD_CLK			(0x52)						// MMC clock controle error
#define	D_SD_ERR_ST_WRITE_FAIL			(0x53)						// MMC write fault
#define	D_SD_ERR_ST_READ_FAIL			(0x54)						// MMC read fault

#define	D_SD_ERR_ST_MBR					(0x80)						// unsupported MBR format when initialize
#define	D_SD_ERR_ST_PBR					(0x81)						// unsupported PBR format when initialize
#define	D_SD_ERR_ST_HUGE_SEC			(0x82)						// unsupported HugeSec format
#define	D_SD_ERR_ST_READ_EOF			(0x85)						// read end of file
#define	D_SD_ERR_ST_SEEK_SEC			(0x87)						// sector error when seek
#define	D_SD_ERR_ST_FILE_NAME			(0x88)						// none of applicable file name
#define	D_SD_ERR_ST_READ_FAT			(0x89)						// FAT broken when reading
#define	D_SD_ERR_ST_BLOKEN_FAT			(0x8a)						// FAT broken
#define	D_SD_ERR_ST_WRITE_ENTRY_FULL	(0x8b)						// full FileEntry when writing
#define	D_SD_ERR_ST_WRITE_DISK_FULL		(0x8c)						// disk full when writing
#define	D_SD_ERR_ST_FILE_POINTER		(0x8d)						// under usage of file pointer
#define	D_SD_ERR_ST_NO_FILE_DATA		(0x8e)						// un-opened file or finish data transfer
#define	D_SD_ERR_ST_NO_END_FLAG			(0x8f)						// cannot finish without D_SD_FLAG_END
#define	D_SD_ERR_ST_DELETE_DISK_ATTR	(0x90)						// disk attribute when deleting
#define	D_SD_ERR_ST_READ_ONLY_ATTR		(0x91)						// read-only attribute
#define	D_SD_ERR_ST_HIDDEN_ATTR			(0x92)						// masked file attribute
#define	D_SD_ERR_ST_SYSTEM_ATTR			(0x93)						// system attribute
#define	D_SD_ERR_ST_VOL_LABEL_ATTR		(0x94)						// volume label attribute
#define	D_SD_ERR_ST_FILE_EXIST			(0x97)						// Target file name exist
#define	D_SD_ERR_ST_MKDIR_ENTRY_FULL	(0x98)						// full FileEntry when Mkdir
#define	D_SD_ERR_ST_RMDIR_FILE_EXIST	(0x99)						// file exists when Rmdir
#define	D_SD_ERR_ST_HOT_REMOVAL			(0xb0)						// Hot Removal Error
#define	D_SDIO_ERR_ST_WRITE				(0xd0)						// SDIO Write Error
#define	D_SDIO_ERR_ST_READ				(0xd1)						// SDIO Read Error

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/************************/
/* CID Registers		*/
/************************/
typedef struct {
	UCHAR	MID;					// Manufacture_ID
	USHORT	OID;					// OEM/Application_ID
	UCHAR	PNM[5];					// ProductName
	UCHAR	PRV;					// Revision
	ULONG	PSN;					// SerialNo
	USHORT	MDT;					// Manufacture_Date
} T_SD_CID_REG;

/************************/
/* CSD Registers		*/
/************************/
typedef struct {
	UCHAR	CSD_STRUCTURE;
	UCHAR	TAAC;					// Data read access time-1
	UCHAR	NSAC;					// Data read access time-2
	UCHAR	TRAN_SPEED;				// Max. data transfer rate
	USHORT	CCC;					// Card common class
	UCHAR	READ_BL_LEN;			// Max.read data block length
	UCHAR	READ_BL_PARTIAL;		// Partial blocks for read allowed
	UCHAR	WRITE_BLK_MISALIGN;		// Write block misalignment
	UCHAR	READ_BLK_MISALIGN;		// Read block misalignment
	UCHAR	DSR_IMP;				// DSR implemented
	USHORT	C_SIZE;					// Device size
	UCHAR	VDD_R_CURR_MIN;			// Max. read current@Vdd min
	UCHAR	VDD_R_CURR_MAX;			// Max. read current@Vdd max
	UCHAR	VDD_W_CURR_MIN;			// Max. write current@Vdd min
	UCHAR	VDD_W_CURR_MAX;			// Max. write current@Vdd max
	UCHAR	C_SIZE_MULT;			// Device size muliplier
	UCHAR	ERASE_BLK_EN;			// Erase single block enable
	UCHAR	SECTOR_SIZE;			// Erase sector size
	UCHAR	WP_GRP_SIZE;			// Write protected group size
	UCHAR	WP_GRP_ENABLE;			// Write protected group enable
	UCHAR	R2W_FACTOR;				// Write speed factor
	UCHAR	WRITE_BL_LEN;			// Max.write data block length
	UCHAR	WRITE_BL_PARTIAL;		// Partial blocks for write allowed
	UCHAR	FILE_FORMAT_GRP;		// File format group
	UCHAR	COPY;					// Copy flag(OTP)
	UCHAR	PERM_WRITE_PROTECT;		// Permanent write protection
	UCHAR	TMP_WRITE_PROTECT;		// Temporary write protection
	UCHAR	FILE_FORMAT;			// File format
} T_SD_CSD_REG;

/************************/
/* SCR Registers		*/
/************************/
typedef struct {
	UCHAR	SCR_STRUCTURE;			// SCR Structure
	UCHAR	SD_SPEC;				// SD Memory Card-Spec.version
	UCHAR	DATA_STAT_AFTER_ERASE;	// DATA Status after erase
	UCHAR	SD_SECURITY;			// SD Security
	UCHAR	SD_BUS_WIDTHS;			// DAT BUS width supported
	UCHAR	SD_SPEC3;				// Spec. Version 3.00 or higher
} T_SD_SCR_REG;

/************************/
/* Lock password		*/
/************************/
typedef struct {
	ULONG	size;					// password size
	UCHAR	data[16];				// password data
} T_SD0_PASSWORD;

/*****************************************   Outline   *******************************************
Generally the following definitions are not to be changed.
*************************************************************************************************/

/*-----  File information structure --------------------------------------------------------------
# file name :character row without "\0".(8+3 type)
# file attribute:bit 5=save, 4=directory,3=volume label, 2=system, 1=masked file, 0=read only
------------------------------------------------------------------------------------------------*/
typedef struct tagF1STAT{
	UCHAR	sFnm[11];									// file name
	BYTE	byAtb;										// file attribute
	ULONG	lTim;										// the latest changed time
	ULONG	lSiz;										// file size
} T_FILE_INFO;


/*-----  File type structure  --------------------------------------------------------------------
# This is the file type that is used on the dir/write operation of F1PACKalpha API.
------------------------------------------------------------------------------------------------*/
typedef struct tagF1FILE{
	ULONG	iScCnt;										// sector count
	ULONG	iScNo;										// sector number
	ULONG	iCurSC;										// current sector count
	USHORT	iStCl;										// start cluster number
	USHORT	iPreCl;										// preceding cluster number
	USHORT	iCl;										// next cluster number
	BYTE	byMod;										// mode
} T_FILE_TYPE;

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
/*-----  External variable  ----------------------------------------------------------------------
------------------------------------------------------------------------------------------------*/
extern volatile BYTE			gSD0_Detect_Flag;
extern volatile BYTE			gSD0_Cmd_No;
extern volatile BYTE			gSD0_Card_Type;
extern volatile BYTE			gSD0_Status;
extern volatile BYTE			gSD0_Err_Status[D_SD_DRIVE_NUM];
extern volatile BYTE			gSD0_Multi_State_Flg;
extern volatile ULONG			gSD0_Card_Status;
extern volatile BYTE			gSD0_Sv_Mode_Flg;
extern volatile ULONG			gSD0_Max_Lba;
extern volatile ULONG			gSD0_Max_Lba_Exp;
extern volatile BYTE			gSD0_Clock_Stop_Mode;
extern volatile BYTE			gSD0_SDIO_Multi_State_Flg;
extern volatile USHORT			gSD0_Block_Size;
extern volatile USHORT			gSD0_PrevBlock_Size;
extern volatile BYTE			gSD0_Stop_State;
extern volatile BYTE			gSD0_SDIO_Abort_Function;
extern volatile BYTE			gSD0_SDIO_Abort_Exe;
extern volatile	ULONG			gSD0_Int_State;
extern volatile BYTE			gSD0_Int_Exe_Flg;
extern volatile BYTE			gSD0_Int_During;
extern volatile BYTE			gSD0_Auto_Cmd_Flg;
extern volatile ULONG			gSD0_DMA_Mode_Flg;
extern volatile BYTE			gSD0_ADMA_Err_State;
extern volatile ULONG			gSD0_ADMA_Err_Addr;
extern volatile BYTE			gSD0_SCR[8];
extern volatile USHORT			gSD0_Sv_Block_Size;
extern volatile ULONG			gSD0_Sector_Cnt_Back;
extern volatile BYTE			gSD0_Busy_Status;			// Busy Status
extern volatile ULONG			gSD0_MMC_RCA;				// RCA value
extern volatile ULONG			gSD0_Max_Clk;				// Max SD Clock Frequency
extern volatile INT32			gSD0_Access_Mode;			// SD Access mode
extern volatile BYTE			gSD0_CMD20_Enable;			// CMD20 Enable Flg
extern volatile INT32			gSD0_Timeout_Single;		// Single R/W timeout
extern volatile INT32			gSD0_Timeout_Multi;			// Multi R/W timeout
extern volatile INT32			gSD0_Power_Wait_Time;		// Power On/Off Wait Time
extern volatile USHORT			gSD0_Ramp_Up_Time;			// Ramp Up Time
extern volatile T_SD0_PASSWORD	gSD0_Password[2];			// Card Password
extern volatile BYTE			gSD0_Lock_State;			// Card Lock state
extern volatile BYTE			gSD0_Driver_Type;			// Driver type


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
extern VOID SD0_INT_Vender0_Err(VOID);
extern VOID SD0_INT_Vender1_Err(VOID);
extern VOID SD0_INT_Vender2_Err(VOID);
extern VOID SD0_INT_Vender3_Err(VOID);
extern VOID SD0_Usr_Enable_Int(VOID);
extern VOID SD0_Usr_Disable_Int(VOID);
extern VOID SD0_Usr_Start_DMA(VOID);
extern VOID SD0_Usr_End_DMA(VOID);
extern VOID SD0_Usr_DMA_Int(ULONG addr);
extern VOID SD0_Usr_Card_Insert_Int(VOID);
extern VOID SD0_Usr_Card_Remove_Int(VOID);
extern VOID SD0_SDIO_Usr_Int(VOID);
extern VOID	SD0_Set_DMA_Next_Bound_Addr(ULONG addr);

extern INT32 MWSDIO0_GetCID(T_SD_CID_REG* pCID, UCHAR* buf);
extern INT32 MWSDIO0_GetCSD(T_SD_CSD_REG* pCSD, UCHAR* buf);
extern INT32 MWSDIO0_GetSCR(T_SD_SCR_REG* pCSR, UCHAR* buf);

/*----- System call function ----------------------------------------------------------------------
# Designate function call that executes FAT management.
-------------------------------------------------------------------------------------------------*/
extern BYTE		SD0_System_Func(VOID);
extern VOID		SD0_Start_Internal_Clock(VOID);
extern VOID		SD0_Stop_Internal_Clock(VOID);
extern VOID		SD0_Set_Auto_Command(BYTE);
extern VOID		SD0_Set_High_Speed_Mode(BYTE);
extern BYTE		SD0_Set_DMA_Mode(BYTE, USHORT, USHORT);
extern BYTE		SD0_Get_ADMA_Err_State(BYTE*, ULONG*);
extern VOID		SD0_Start_SV_Mode(VOID);
extern VOID		SD0_End_SV_Mode(VOID);
extern BYTE		SD0_Send_SV_Command(USHORT, USHORT, ULONG, USHORT);
extern BYTE		SD0_Read_CID(ULONG*);
extern BYTE		SD0_Read_CSD(ULONG *);
extern ULONG	SD0_Read_Card_Status(VOID);
extern ULONG	SD0_Get_Status_SV_Mode(VOID);
extern BYTE		SD0_Read_SV_Mode(USHORT, BYTE*, ULONG);
extern BYTE		SD0_Write_SV_Mode(USHORT, BYTE*, ULONG);
extern BYTE		SD0_Stop_Multi_Transfer_SV_Mode(VOID);
extern BYTE		SD0_Set_Block_Len_SV_Mode(USHORT);
extern BYTE		SD0_Check_MDR(VOID);

/*-----  Timer function  -------------------------------------------------------------------------
------------------------------------------------------------------------------------------------*/
extern VOID		SD0_Set_Timer(VOID);
extern BYTE		SD0_Check_Timer(USHORT);
extern ULONG	SD0_Start_Abort_Timer(VOID);
extern BYTE		SD0_Check_Abort_Timer(USHORT, ULONG);

extern ULONG	SD0_Get_Timeout_Clock(void);
extern ULONG	SD0_Get_System_Time(void);

extern	VOID	SD0_Power_Off(VOID);
extern	VOID	SD0_Power_On(VOID);
extern	BYTE	SD0_Mount_Retry(VOID);
extern	ULONG	SD0_Get_Mount_Retry_Cnt(VOID);
extern	VOID	SD0_Clr_Mount_Retry_Cnt(VOID);
extern	ULONG	SD0_Get_RW_Retry_Cnt(INT32 retry_level);
extern	VOID	SD0_Clr_RW_Retry_Cnt(INT32 retry_level);
extern	VOID	SD0_Get_CID_Data(BYTE*);
extern	BYTE	SD0_Check_Group_Write_Protect(BYTE*);
extern	VOID	SD0_Check_Card_WP_Status(BYTE*);
extern	VOID	f1_eiResume(VOID);
extern	BYTE	SD0_Wait_Status(USHORT, USHORT);
extern	BYTE	SD0_Wait_Tran_State(VOID);
extern	BYTE	SD0_Initialize(VOID);
extern	BYTE	SD0_Read_Sector(BYTE* data, ULONG sector_no, ULONG sector_cnt);
extern	BYTE	SD0_Write_Sector(BYTE* data, ULONG sector_no, ULONG sector_cnt);
extern	BYTE	SD0_Card_Init(VOID);
extern	BYTE	SD0_Send_Command00(VOID);
extern	BYTE	SD0_Send_Command01(VOID);
extern	BYTE	SD0_Send_Command02(VOID);
extern	BYTE	SD0_Send_Command03(VOID);
extern	BYTE	SD0_Send_Command06(ULONG);
extern	BYTE	SD0_Send_Command07(VOID);
extern	BYTE	SD0_Send_Command08(VOID);
extern	BYTE	SD0_Send_Command09(VOID);
extern	BYTE	SD0_Send_Command10(VOID);
extern	BYTE	SD0_Send_Command12(VOID);
extern	ULONG	SD0_Send_Command13(VOID);
extern	BYTE	SD0_Send_Command16(USHORT);
extern	BYTE	SD0_Send_Command17(BYTE*, ULONG, ULONG);
extern	BYTE	SD0_Send_Command18(BYTE*, ULONG, ULONG);
extern	BYTE	SD0_Send_Command24(BYTE*, ULONG, ULONG);
extern	BYTE	SD0_Send_Command25(BYTE*, ULONG, ULONG);
extern	BYTE	SD0_Send_Command30(ULONG, ULONG*);
extern	BYTE	SD0_Send_Command42(BYTE*, ULONG);
extern	BYTE	SD0_Send_Command55(VOID);
extern	BYTE	SD0_Send_Command_Other(USHORT,USHORT,ULONG);
extern	BYTE	SD0_Send_ACommand06(ULONG);
extern	BYTE	SD0_Send_ACommand13(VOID);
extern	BYTE	SD0_Send_ACommand22(ULONG* wr_blks);
extern	BYTE	SD0_Send_ACommand41(ULONG);
extern	BYTE	SD0_Send_ACommand42(VOID);
extern	BYTE	SD0_Send_ACommand51(VOID);
extern	BYTE	SD0_Check_Cmd_Issue(USHORT);
extern	VOID	SD0_Set_Cmd_Transmode_Reg(USHORT,USHORT);
extern	VOID	SD0_Set_Int_Sts_Sig_Enable(USHORT,USHORT);
extern	VOID	SD0_Set_Int_Sts_Sig_Disable(USHORT,USHORT);
extern	BYTE	SD0_Check_Cmd_Complete(volatile ULONG*);
extern	BYTE	SD0_Check_Buff_Read_Rdy(volatile ULONG*);
extern	BYTE	SD0_Check_Buff_Read_Rdy_State(volatile ULONG*, BYTE*, ULONG);
extern	BYTE	SD0_Check_Buff_Write_Rdy(volatile ULONG*);
extern	BYTE	SD0_Check_Buff_Write_Rdy_State(volatile ULONG*, BYTE*, ULONG);
extern	BYTE	SD0_Check_Trans_Complete(volatile ULONG*);
extern	BYTE	SD0_Check_Trans_Complete_RW(volatile ULONG* int_status, ULONG sector_cnt, INT32 dma_time);
extern	BYTE	SD0_Check_DMA_Int(ULONG, volatile ULONG*);
extern	BYTE	SD0_Set_Length_Reg(USHORT);
extern	USHORT	SD0_Get_Length_Reg(VOID);
extern	VOID	SD0_Set_DMA_Bound(USHORT);
extern	VOID	SD0_Set_Block_Count_Reg(USHORT);
extern	USHORT	SD0_Get_Block_Count_Reg(VOID);
extern	BYTE	SD0_Err_Recovery(VOID);
extern	VOID	SD0_Set_Err_Code(ULONG);
extern	VOID	SD0_Set_Max_LBA(VOID);
extern	VOID	SD0_Set_Card_Status(VOID);
extern	VOID	SD0_Set_Card_Status_A12(VOID);
extern	VOID	SD0_Set_CID(VOID);
extern	VOID	SD0_Set_CSD(VOID);
extern	VOID	SD0_Set_RCA(VOID);
extern	BYTE	SD0_Set_SCR(VOID);
extern	BYTE	SD0_Set_Write_Blocks(ULONG* wr_blks);
extern	BYTE	SD0_Set_SD_Status(VOID);
extern	BYTE	SD0_Set_Switch_Function_Status(VOID);
extern	BYTE	SD0_Get_Transfer_Clock(ULONG*);
extern	VOID	SD0_Get_RW_Block_Length(USHORT*, USHORT*, BYTE*, BYTE*);
extern	BYTE	SD0_Get_Spec(VOID);
extern	BYTE	SD0_MMC_Get_Spec(VOID);
extern	USHORT	SD0_Swap_HW(USHORT);
extern	ULONG	SD0_Swap_W(ULONG);
extern	VOID	SD0_Set_Bus_Witdh4(BYTE);
extern	VOID	SD0_Set_Timeout_Cnt(VOID);
extern	VOID	SD0_Set_Clock_Divide(ULONG);
extern	VOID	SD0_Set_Internal_Clock_Enable(BYTE);
extern	VOID	SD0_Start_Chg_Clock(ULONG);
extern	VOID	SD0_Stop_Card_Clock(USHORT);
extern	ULONG	SD0_Read_Support_Bus_Power(BYTE);
extern	ULONG	SD0_Get_Enable_Bus_Power(ULONG);
extern	BYTE	SD0_Check_Cur_Bus_Power(ULONG);
extern	VOID	SD0_Set_Bus_Power(BYTE,ULONG);
extern	VOID	SD0_Soft_Reset(BYTE);
extern	BYTE	SD0_Get_Support_Function(USHORT *,ULONG *);
extern	BYTE	SD0_Get_Cur_Consumption(ULONG,USHORT *);
extern	BYTE	SD0_Switch_Function(ULONG);
extern	BYTE	SD0_Set_High_Speed(BYTE);
extern	VOID	SD0_Start_DMA(ULONG);
extern	VOID	SD0_End_DMA(VOID);
extern	VOID	SD0_Set_ADMA_Descriptor_Tbl(ULONG addr, ULONG data_length);

extern	VOID	SD0_INT_Command_Complete(VOID);
extern	VOID	SD0_INT_Transfer_Complete(ULONG sector_cnt);
extern	VOID	SD0_INT_Block_Gap_Event(VOID);
extern	VOID	SD0_INT_DMA(ULONG);
extern	VOID	SD0_INT_Buffer_Write_Ready(BYTE* data, ULONG sector_cnt);
extern	VOID	SD0_INT_Buffer_Read_Ready(BYTE* data, ULONG sector_cnt); 
extern	VOID	SD0_INT_Card_Insert(VOID);
extern	VOID	SD0_INT_Card_Remove(VOID);
extern	VOID	SD0_INT_SDIO_Card(VOID);
extern	VOID	SD0_INT_Read_Sector(BYTE* data, ULONG sector_cnt);
extern	VOID	SD0_INT_Write_Sector(BYTE* data, ULONG sector_cnt);
extern	VOID	SD0_INT_Cmd_Line_Conflict(VOID);
extern	VOID	SD0_INT_Cmd_Timeout_Err(VOID);
extern	VOID	SD0_INT_Cmd_CRC_Err(VOID);
extern	VOID	SD0_INT_Cmd_Endbit_Err(VOID);
extern	VOID	SD0_INT_Cmd_Index_Err(VOID);
extern	VOID	SD0_INT_Data_Timeout_Err(VOID);
extern	VOID	SD0_INT_Data_CRC_Err(VOID);
extern	VOID	SD0_INT_Data_Endbit_Err(VOID);
extern	VOID	SD0_INT_Cur_Limit_Err(VOID);
extern	VOID	SD0_INT_Auto_Cmd12_Err(VOID);
extern	VOID	SD0_INT_ADMA_Err(VOID);
extern	VOID	SD0_INT_AME_Err(VOID);
extern	VOID	SD0_INT_Err_Handler(ULONG);
extern	VOID	SD0_INT_Handler(VOID);
extern	VOID	SD0_Macro_Initialize(VOID);

extern	BYTE	SD0_Send_Command11(VOID);
extern	BYTE	SD0_Send_Command19(VOID);
extern	BYTE	SD0_Send_Command20(ULONG);
extern	BYTE	SD0_Send_Command23(ULONG);
extern	BYTE	SD0_Set_Switch_Func(BYTE, BYTE, BYTE);
extern	BYTE	SD0_Tuning_Execute(VOID);
extern	VOID	SD0_Soft_Phase_Select(VOID);

extern	BYTE	SD0_Set_Hold_Time_Adjust_Reg(ULONG);
extern	INT32	SD0_Get_CardType(VOID);
extern	VOID	SD0_Set_Signal_Low_Output(BYTE);
extern	BYTE	SD0_Set_Lock_Unlock(BYTE);

extern	BYTE	SD0_SDIO_Card_PowerON_Init(VOID);
extern	BYTE	SD0_SDIO_Card_ReInit(VOID);
extern	BYTE	SD0_SDIO_Card_Init_Core(VOID);
extern	VOID	SD0_SDIO_INT_Read_Block(BYTE* data);
extern	VOID	SD0_SDIO_INT_Write_Block(BYTE* data);
extern	BYTE	SD0_SDIO_Set_Card_Int_Mask(BYTE);
extern	ULONG	SD0_SDIO_Send_Command(USHORT, USHORT, ULONG, USHORT, ULONG);
extern	BYTE	SD0_SDIO_Write_Data(VOID);
extern	BYTE	SD0_SDIO_Read_Data(VOID);
extern	BYTE	SD0_SDIO_Stop_Multi_Transfer(VOID);
extern	BYTE	SD0_SDIO_Control_Int(BYTE);
extern	VOID	SD0_SDIO_Int_Mask(BYTE);
extern	BYTE	SD0_SDIO_Set_Bus_Width(BYTE);
extern	BYTE	SD0_SDIO_Set_E4MI(VOID);
extern	VOID	SD0_SDIO_Set_4bit_Bus(BYTE);
extern	BYTE	SD0_SDIO_Set_High_Speed(BYTE set);
extern	BYTE	SD0_SDIO_Card_Init(VOID);
extern	ULONG	SD0_SDIO_Send_Command05(USHORT, USHORT, ULONG);
extern	ULONG	SD0_SDIO_Send_Command52(USHORT, USHORT, ULONG);
extern	ULONG	SD0_SDIO_Send_Command53(USHORT cmd, USHORT trans, ULONG arg, USHORT bsz, ULONG sys_addr);

#endif /* _SD0_DRIVER_INFO_H_ */
