/**
 * @file		mwsdio1.h
 * @brief		SD Memory card driver header file
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _MWSDIO1_H_
#define _MWSDIO1_H_

/** @weakgroup overview
@{
*/
/*@} --- end of overview */

/** @weakgroup difinition
@{
*/
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/**************/
/* ERROR CODE */
/**************/
#define D_SD1_OK			(0)				/**< No Error */
#define D_SD1_ERR			(-1)			/**< Error occurred */
#define D_SD1_UNDEFINED_ERR	(-2)

#define D_SD1_ERR_TOP						(0xFFF20000)
#define D_SD1_ERR_NP						(0x00000000)				/**< No error status */
#define D_SD1_ERR_READ						(D_SD1_ERR_TOP | 0x0001)	/**< Sector read error */
#define D_SD1_ERR_WRITE						(D_SD1_ERR_TOP | 0x0002)	/**< Sector write error */
#define D_SD1_ERR_NOCARD					(D_SD1_ERR_TOP | 0x0003)	/**< Medium is not inserted */
#define D_SD1_ERR_WP						(D_SD1_ERR_TOP | 0x0004)	/**< Medium is write-protected */
#define D_SD1_ERR_CRC						(D_SD1_ERR_TOP | 0x0005)	/**< CRC error */
#define D_SD1_ERR_OUTOFRANGE				(D_SD1_ERR_TOP | 0x0006)	/**< LBA is out of range */
#define D_SD1_ERR_TIMEOUT					(D_SD1_ERR_TOP | 0x0007)	/**< Timeout */
#define D_SD1_ERR_ABORT						(D_SD1_ERR_TOP | 0x0008)	/**< Access is aborted(unknown error) */
#define D_SD1_ERR_NOTREADY					(D_SD1_ERR_TOP | 0x0009)	/**< Drive is not ready */
#define D_SD1_ERR_PBR						(D_SD1_ERR_TOP | 0x000A)	/**< Not PBR data */

#define D_SD1_INT_NORMAL	(0)				/**< Normal Interrupt (return value of MWSDIO1_Ex3_IntHandler().) */
#define D_SD1_INT_INSERT	(1)				/**< Card Insert Interrupt (return value of MWSDIO1_Ex3_IntHandler().) */
#define D_SD1_INT_EJECT		(2)				/**< Card Eject Interrupt (return value of MWSDIO1_Ex3_IntHandler().) */

/*********************************/
/*  definition of Media Capacity */
/*********************************/
#define D_SD1_CAPACITY_2MB					(2)			/**< Media Capacity 2MB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_4MB					(4)			/**< Media Capacity 4MB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_8MB					(8)			/**< Media Capacity 8MB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_16MB					(16)		/**< Media Capacity 16MB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_32MB					(32)		/**< Media Capacity 32MB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_64MB					(64)		/**< Media Capacity 64MB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_128MB				(128)		/**< Media Capacity 128MB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_256MB				(256)		/**< Media Capacity 256MB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_512MB				(512)		/**< Media Capacity 512MB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_1GB					(1024)		/**< Media Capacity 1GB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_2GB					(2048)		/**< Media Capacity 2GB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_4GB					(4096)		/**< Media Capacity 4GB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_8GB					(8192)		/**< Media Capacity 8GMB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_16GB					(16384)		/**< Media Capacity 16GB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_32GB					(32768)		/**< Media Capacity 32GB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_64GB					(65536)		/**< Media Capacity 64GB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_128GB				(131072)	/**< Media Capacity 128GB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_256GB				(262144)	/**< Media Capacity 256GB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_512GB				(524288)	/**< Media Capacity 512GB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_1TB					(1048576)	/**< Media Capacity 1TB (for T_SD1_MNG capacity.) */
#define D_SD1_CAPACITY_2TB					(2097152)	/**< Media Capacity 2TB (for T_SD1_MNG capacity.) */

/***********************/
/* Card Protect Status */
/***********************/
#define D_SD1_STATE_CARD_LOCKED				(0x01)		/**< Card is Locked (for MWSDIO1_CardCheckWP_Status().) */
#define D_SD1_STATE_PERM_WP					(0x02)		/**< Permanent Write Protected (for MWSDIO1_CardCheckWP_Status().) */
#define D_SD1_STATE_TEMP_WP					(0x04)		/**< Temporary Write Protected (for MWSDIO1_CardCheckWP_Status().) */
#define D_SD1_STATE_GROUP_WP				(0x08)		/**< Group Write Protected (for MWSDIO1_CardCheckWP_Status().) */

/***********************/
/* Singnal polarity    */
/***********************/
#define D_SD1_SIGNAL_CD				(0x01)
#define D_SD1_SIGNAL_WP				(0x02)
#define D_SD1_SIGNAL_LED			(0x03)

#define D_SD1_NOT_INV				(0x00)
#define D_SD1_INV					(0x01)

/***********************/
/* EXS CD INT STATUS   */
/***********************/
#define D_SD1_EXS_SD1FIS_BIT			(1 << 3)	/**< Bit Location of INTSTAT[3] on INTSTAT */
#define D_SD1_EXS_SD1RIS_BIT			(1 << 2)	/**< Bit Location of INTSTAT[2] on INTSTAT */

//User Custom 
//#define	D_SD1_FID_SD	(FID_SD1)	//Event flag ID
//#define D_SD1_SID_SD	(SID_SD1)	//Semafore ID
extern volatile UINT32	gSD1_FID_SD;		//Event flag ID
extern volatile UINT32	gSD1_SID_SD;		//Semafore ID

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/**
Media Type
*/
typedef enum {
	E_SD1_MEDIA_TYPE_2MB   = 0,			/**< Media Type 2MB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_4MB   = 1,			/**< Media Type 4MB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_8MB   = 2,			/**< Media Type 8MB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_16MB  = 3,			/**< Media Type 16MB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_32MB  = 4,			/**< Media Type 32MB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_64MB  = 5,			/**< Media Type 64MB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_128MB = 6,			/**< Media Type 128MB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_256MB = 7,			/**< Media Type 256MB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_512MB = 8,			/**< Media Type 512MB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_1GB   = 9,			/**< Media Type 1GB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_2GB   = 10,		/**< Media Type 2GB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_4GB   = 11,		/**< Media Type 4GB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_8GB   = 12,		/**< Media Type 8GB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_16GB  = 13,		/**< Media Type 16GB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_32GB  = 14,		/**< Media Type 32GB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_64GB  = 15,		/**< Media Type 64GB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_128GB = 16,		/**< Media Type 128GB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_256GB = 17,		/**< Media Type 256GB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_512GB = 18,		/**< Media Type 512GB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_1TB   = 19,		/**< Media Type 1TB (for T_SD1_MNG mediatype.) */
	E_SD1_MEDIA_TYPE_2TB   = 20			/**< Media Type 2TB (for T_SD1_MNG mediatype.) */
} E_SD1_MEDIA_TYPE;
/**
Hold Value
*/
typedef enum {
	E_SD1_DATA_HOLD_VAL_1_4			= 0x00000000,			/**< Hold Value of DATA (1/4 of SD_SDBLK_I.)  */
	E_SD1_DATA_HOLD_VAL_1_4_1_4		= 0x000C0000,			/**< Hold Value of DATA ((1/4 of SD_SDBLK_I) + (1/4 of SD_SDBLK_I).) */
	E_SD1_DATA_HOLD_VAL_1_4_1_4_1_4	= 0x000F0000			/**< Hold Value of DATA ((1/4 of SD_SDBLK_I) + (1/4 of SD_SDBLK_I) + (1/4 of SD_SDBLK_I).) */
} E_SD1_DATA_HOLD_VAL;

typedef enum {
	E_SD1_CMD_HOLD_VAL_1_4			= 0x00000000,			/**< Hold Value of CMD (1/4 of SD_SDBLK_I.) */
	E_SD1_CMD_HOLD_VAL_1_4_1_4		= 0x00000C00,			/**< Hold Value of CMD ((1/4 of SD_SDBLK_I) + (1/4 of SD_SDBLK_I).) */
	E_SD1_CMD_HOLD_VAL_1_4_1_4_1_4	= 0x00000F00			/**< Hold Value of CMD ((1/4 of SD_SDBLK_I) + (1/4 of SD_SDBLK_I) + (1/4 of SD_SDBLK_I).) */
} E_SD1_CMD_HOLD_VAL;

/**
Lock/Unlock operation
*/
typedef enum {
	E_SD1_UNLOCK		= 0x00,			/**< Unlock  */
	E_SD1_SET_PASSWORD	= 0x01,			/**< Set Password  */
	E_SD1_CLR_PASSWORD	= 0x02,			/**< Clear Password  */
	E_SD1_LOCK			= 0x04,			/**< Lock  */
	E_SD1_ERASE			= 0x08			/**< Erase  */
} E_SD1_LOCK_OPERATION;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/******************/
/* Data Structure */
/******************/
/** 
Media Control command Information
*/
typedef struct {
	unsigned char  command;			/**< command number of media control */
	unsigned long  secno;			/**< number of sector for read/write */
	unsigned long  lba;				/**< start LBA to read/write */
	unsigned char* buf;				/**< pointer of read/write memory buffer */
} T_SD1_MCI;

/** 
Drive Information Data structure
*/
typedef struct {
	unsigned char  ostype;			/**< Filesystem type */
	unsigned long  pbr_secno;		/**< PBR start sector no */
	unsigned long  status;			/**< indicate error status if Bread/Bwrite */
	unsigned char  err;				/**< error status for SD low-level driver(f1_seven) */
	unsigned long  errsecno;		/**< indicate error sector no if Bread/Bwrite failed */
	unsigned char  wp;				/**< Write protected (1:Write-Protected) */
	unsigned short head;			/**< number of head */
	unsigned short sectrack;		/**< sectors per track */
	unsigned short cylinder;		/**< number of cylinder */
	unsigned short bytepersector;	/**< bytes per sector */
	unsigned long  totalsector;		/**< total sector(LBA) */
	unsigned short mediatype;		/**< media type */
	unsigned long  rca;				/**< RCA value */
	T_SD1_MCI*     cmd;				/**< media control command */
	unsigned long  cid[4];			/**< media CID data */
	unsigned char  card_type;		/**< media type  SD or MMC */
	unsigned char  sd_spec;			/**< SD Spec version */
	unsigned long  capacity;		/**< media size (MByte) */
} T_SD1_MNG;

/** 
Select UHS Operation Mode
*/
typedef struct{
	unsigned short sup_uhs_mode;	/**< Select UHS-I Mode */
	unsigned short sup_driver_pow;	/**< Select Driver Power */
	unsigned short sup_cur_lim;		/**< Select Current Limit */
} T_SD1_SWITCH_MODE;
/** 
SDXC Card Support Mode Information
*/
typedef struct {
	unsigned char  access_mode;		/**< Access Mode */
	unsigned char  drv_pow;			/**< Driver Strength Mode */
	unsigned char  cur_lim;			/**< Current Limit Mode */
	unsigned long  clock_speed;		/**< SD_Clock Speed */
} T_SD1_OPERATION_MODE;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern volatile T_SD1_MNG         gSD1_dev_mng;				/**< SD device information */
extern volatile T_SD1_SWITCH_MODE MWSDIO1_switch_mode;		/**< Card Support Mode Check */

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
/*@} --- end of difinition */

/** @weakgroup api
@{
*/
/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
This function set card reset flag.
@param[in] Reset flag: 0 : Reset, 1 : Not Reset.
*/
extern void MWSDIO1_SetResetFlag(unsigned char flag);

/**
This function initialize a card interface.
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern int  MWSDIO1_InitCardIF(void);

/**
This function check card inserted or not.
@retval D_SD1_OK Card Inserted.
@retval D_SD1_ERR Card not Inserted.
*/
extern int  MWSDIO1_CardCheck(void);

/**
This function get SD Card CID register data.
@param[out] buf buffer address to store CID data.
*/
extern void MWSDIO1_Get_CID_Data(unsigned char* buf);

/**
This function get Card Write Protect status.
@param[out] wpStatus Write Protect Status.<br>
						= @ref D_SD1_STATE_CARD_LOCKED Card is Locked. <br>
						= @ref D_SD1_STATE_PERM_WP     Permanent Write Protected. <br>
						= @ref D_SD1_STATE_TEMP_WP     Temporary Write Protected. <br>
						= @ref D_SD1_STATE_GROUP_WP    Group Write Protected.
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern int MWSDIO1_CardCheckWP_Status(unsigned char* wpStatus);

/**
This function check card is protected or not.
@retval D_SD1_OK Write Protected (Protect SW ON).
@retval D_SD1_ERR not Write Protected (Protect SW OFF).
*/
extern int  MWSDIO1_CardCheckWP(void);

/**
This function reset the card.
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern int  MWSDIO1_CardReset(void);

/**
This function initialize the card.
@param[in] DevInfo device information.
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern int  MWSDIO1_Init(T_SD1_MNG* DevInfo);

/**
This function check the UHS2 card.
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern INT32 MWSDIO1_Uhs2CardCheck(VOID);

/**
This function initialize the UHS2 card.
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern INT32 MWSDIO1_Uhs2Init(T_SD1_MNG* DevInfo);

/**
This function slave mode for PHY compliance test.
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern INT32 MWSDIO1_Uhs2SlaveMode(unsigned char mode);

/**
This function change the Dormant state.
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern INT32 MWSDIO1_Uhs2DormantState(unsigned char mode);

/**
This function change the Hibernate state.
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern INT32 MWSDIO1_Uhs2HibernateState(unsigned char mode);

/**
This function read data of the card by 512 bytes.
@param[in]  lba_sector read start sector number.
@param[in]  rdSecN read sector count.
@param[out] lpbuf buffer address to store the read data.
@param[out] errorSecNo sector number that error occurred.
@param[out] status error information.
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern int  MWSDIO1_ReadSectors(unsigned long lba_sector, unsigned long rdSecN, unsigned char* lpbuf, unsigned long* errorSecNo, unsigned char* status);

/**
This function write data to the card by 512 bytes.
@param[in]  lba_sector write start sector number.
@param[in]  wtSecN write sector count.
@param[in]  lpbuf buffer address to store the write data.
@param[out] errorSecNo sector number that error occurred.
@param[out] status error information.
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern int  MWSDIO1_WriteSectors(unsigned long lba_sector, unsigned long wtSecN, unsigned char* lpbuf, unsigned long* errorSecNo, unsigned char* status);

/**
Interrupt handler of SD1 driver.
@retval D_SD1_INT_INSERT Card insert interrupt.
@retval D_SD1_INT_EJECT Card eject interrupt.
@retval D_SD1_INT_NORMAL other interrupt.
@remarks
 Please call this API when the interrupt of SD macro ch1 occurred.
*/
extern int  MWSDIO1_Ex3_IntHandler(void);

/**
Interrupt handler of SD1 driver(UHS2).
@retval D_SD1_INT_INSERT Card insert interrupt.
@retval D_SD1_INT_EJECT Card eject interrupt.
@retval D_SD1_INT_NORMAL other interrupt.
@remarks
 Please call this API when the interrupt of SD macro ch1 occurred.
*/
extern int  MWSDIO1_Uhs2_IntHandler(void);

/**
Interrupt handler of Exstop card detect.
@retval D_SD1_INT_INSERT Card insert interrupt.
@retval D_SD1_INT_EJECT Card eject interrupt.
@retval D_SD1_INT_NORMAL other interrupt.
@remarks
 Please call this API when the interrupt of Exstop card detect.
*/
extern int  MWSDIO1_Exs_IntHandler(ULONG intst);

/**
This function format the SD card.
@param[in] DevInfo device information.
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern int  MWSDIO1_PhyFormat(T_SD1_MNG* DevInfo);

/**
This function set volume label to SD card PBR.
@param[in] DevInfo device information.
@param[in] label volume label string.
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern int  MWSDIO1_Set_VolumeLabel(T_SD1_MNG* DevInfo, const char* label);

/**
This function set DMA enable.
@param[in] enable DMA enable select (1:enable, 0:disable).
*/
extern void MWSDIO1_Set_DMA_Enable(unsigned char enable);

/**
This function sets limit of Speed mode and clock.
@param[in] mode Speed mode
@param[in] max_clock Max clock
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern int MWSDIO1_Set_Transmode_Select(int mode, unsigned long max_sd_clk);

/**
This function get speed mode and clock.
@param[out] mode Speed mode
@param[out] max_clock Max clock
*/
extern void MWSDIO1_Get_Transmode_Select(int* mode, unsigned long* sd_clk);

/**
This function sets limit of UHS-II mode, speed range and power limit.
@param[in] mode UHS-II mode
@param[in] range Speed range
@param[in] power Power limit
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern int MWSDIO1_Set_UHS2mode_Select(unsigned int mode, unsigned int range, unsigned int power);

/**
This function get UHS-II mode, speed range and power limit.
@param[out] mode UHS-II mode
@param[out] range Speed range
@param[out] power Power limit
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern int MWSDIO1_Get_UHS2mode_Select(unsigned int* mode, unsigned int* range, unsigned int* power);

/**
This function change RCA value.
@param[in] RCA RCA value.
*/
extern void MWSDIO1_ChangeRCA(unsigned long RCA);

/**
This function get Mount retry count.
@return Mount retry count.
*/
extern unsigned long MWSDIO1_Get_Mount_Retry_Cnt(void);

/**
This function clear Mount retry count.
*/
extern void MWSDIO1_Clr_Mount_Retry_Cnt(void);

/**
This function get read/write retry count.
@param[in] retry_level retry revel (1, 2, 3)
@return read/write retry count.
*/
extern unsigned long MWSDIO1_Get_RW_Retry_Cnt(int retry_level);

/**
This function clear read/write retry count.
@param[in] retry_level retry revel (1, 2, 3)
*/
extern void MWSDIO1_Clr_RW_Retry_Cnt(int retry_level);

/**
Get card state using SD1CDX.
@retval D_SD1_NO_CARD	: @ref D_SD1_NO_CARD
@retval D_SD1_ANY_CARD	: @ref D_SD1_ANY_CARD
*/
extern int MWSDIO1_Get_SDCDX(void);

/**
Get card state using Present State.
@retval D_SD1_NO_CARD	: @ref D_SD1_NO_CARD
@retval D_SD1_ANY_CARD	: @ref D_SD1_ANY_CARD
*/
extern int MWSDIO1_Get_State(void);

/**
Clear card state for SD1 Interrupt State.
*/
extern void MWSDIO1_Clear_State(void);

/**
This function read data of the card by 512 bytes.
@param[in]  devinfo device information.
@param[in]  start read start sector number.
@param[in]  cnt read sector count.
@param[out] data buffer address to store the read data.
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern int MWSDIO1_Bread(void* devinfo, unsigned long start, unsigned long cnt, char* data);

/**
This function write data to the card by 512 bytes.
@param[in]  devinfo device information.
@param[in]  start write start sector number.
@param[in]  cnt write sector count.
@param[in]  data buffer address to store the write data.
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern int MWSDIO1_Bwrite(void* devinfo, unsigned long start, unsigned long cnt, char* data);

/**
This function get device information.
@param[in]  devinfo device information.
@param[out] fskind file system type.
@param[out] BlkSize sector size.
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern int MWSDIO1_GetFSData(void* devinfo, unsigned char* fskind, unsigned long* BlkSize);

/**
This function set write mode.
@param[in]  tdevinfo device information.
@param[out] write_mode write mode.
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
extern int MWSDIO1_SetWriteMode(void* tdevinfo, unsigned long write_mode);

/**
This function initialize the card.
@retval D_SD1_OK Normal end.
@retval other Error occurred.
*/
int SD1_sdio_init(void);

/**
This function set Hold Value of DATA & Command line output.
@param[in] d_hold Hold Value of DATA line output.<br>
						= @ref E_SD1_DATA_HOLD_VAL_1_4			1/4 of SD_SDBLK_I.<br>
						= @ref E_SD1_DATA_HOLD_VAL_1_4_1_4		(1/4 of SD_SDBLK_I) + (1/4 of SD_SDBLK_I).<br>
						= @ref E_SD1_DATA_HOLD_VAL_1_4_1_4_1_4	(1/4 of SD_SDBLK_I) + (1/4 of SD_SDBLK_I) + (1/4 of SD_SDBLK_I)
@param[in] c_hold Hold Value of Command line output.<br>
						= @ref E_SD1_CMD_HOLD_VAL_1_4			1/4 of SD_SDBLK_I.<br>
						= @ref E_SD1_CMD_HOLD_VAL_1_4_1_4		(1/4 of SD_SDBLK_I) + (1/4 of SD_SDBLK_I).<br>
						= @ref E_SD1_CMD_HOLD_VAL_1_4_1_4_1_4	(1/4 of SD_SDBLK_I) + (1/4 of SD_SDBLK_I) + (1/4 of SD_SDBLK_I)
*/
extern void MWSDIO1_Set_Hold_Time_Adjust(E_SD1_DATA_HOLD_VAL d_hold, E_SD1_CMD_HOLD_VAL c_hold);

/**
This function get Hold Value of DATA & Command line output.
*/
extern void MWSDIO1_Get_Hold_Time_Adjust(E_SD1_DATA_HOLD_VAL* d_hold, E_SD1_CMD_HOLD_VAL* c_hold);

/**
This function set SD driving force control.
@param[in]  dbcnt Driving force control.
*/
extern void MWSDIO1_Set_Driving_Force(ULONG dbcnt);

extern int MWSDIO1_Set_Signal_Polarity( unsigned char signal_kind, unsigned char inv );

extern int MWSDIO1_Get_CardType(VOID);
extern int MWSDIO1_Get_CardStatus(VOID);
extern int  MWSDIO1_Set_CMD20_Enable(unsigned char enable);
extern int  MWSDIO1_Set_Mount_Retry_Max(int max);
extern void MWSDIO1_Set_Data_Complete_Timeout(int single_time, int multi_time);
extern void MWSDIO1_Set_Power_Wait_Time(int time);
extern void MWSDIO1_Set_Ramp_Up_Time(unsigned short time);
extern void MWSDIO1_Set_Data_Hold_Clock( UCHAR data );
extern void MWSDIO1_Set_Cmd_Hold_Clock( UCHAR cmd );
extern int MWSDIO1_Set_Card_Password(CHAR* password, ULONG size);
extern int MWSDIO1_Set_Card_Lock_Unlock(E_SD1_LOCK_OPERATION operation);
extern int MWSDIO1_Set_Card_Power(BYTE enable);
extern int MWSDIO1_Set_INT_CB(UCHAR type, void (*callback_func)(UCHAR ch));
extern void MWSDIO1_Set_UHS2_Detect_Time(UINT32 time);

/*----------------------------------------------------------------------*/
/* Function (for SDIO)													*/
/*----------------------------------------------------------------------*/
int MWSDIO1_IO_PowerON_Init(void);
int MWSDIO1_IO_ReInit(void);
int MWSDIO1_IO_Enable_HighSpeed(void);
int MWSDIO1_IO_Disable_HighSpeed(void);
int MWSDIO1_Send_CMD0(void);
int MWSDIO1_Send_CMD3(unsigned long* RCA_value);
int MWSDIO1_Send_CMD7(unsigned long RCA_value);
unsigned long MWSDIO1_Send_CMD5(unsigned long cmd_ctrl, unsigned long cmd_arg);
unsigned long MWSDIO1_Send_CMD52(unsigned long cmd_ctrl, unsigned long cmd_arg);
unsigned long MWSDIO1_Send_CMD53(unsigned long cmd_ctrl, unsigned long cmd_arg, unsigned long blk_size, unsigned char* buf);
unsigned long MWSDIO1_Send_CMD53_DMA(unsigned long cmd_ctrl, unsigned long cmd_arg, unsigned long blk_size, unsigned char* buf);
int MWSDIO1_Set_SDIO_INT_CB(void (*callback_func)(void));

/*@} --- end of api */
#endif // _MWSDIO1_H_
